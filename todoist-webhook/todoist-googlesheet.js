const CONFIG = {
  EVENT_NAME: 'item:completed',
  EVENT_CACHE_TTL_SECONDS: 60 * 60 * 24 * 7,
  OAUTH_STATE_CACHE_TTL_SECONDS: 60 * 10
};

function doGet(e) {
  const params = (e && e.parameter) ? e.parameter : {};

  if (params.action === 'oauth-start') {
    return startTodoistOAuth_();
  }

  if (params.action === 'debug-uri') {
    const uri = getTodoistRedirectUri_();
    return ContentService
      .createTextOutput(JSON.stringify({ redirectUri: uri }))
      .setMimeType(ContentService.MimeType.JSON);
  }

  if (params.code) {
    return handleTodoistOAuthCallback_(params);
  }

  return ContentService
    .createTextOutput(JSON.stringify({
      ok: true,
      message: 'Todoist webhook endpoint is live',
      note: 'Send POST requests with Todoist webhook payload',
      oauth: {
        start: 'Add ?action=oauth-start to this URL to start Todoist OAuth activation'
      }
    }))
    .setMimeType(ContentService.MimeType.JSON);
}

function doPost(e) {
  // USER REQUIREMENT:
  // - Never early-return from webhook processing.
  // - Always append one result row to a Google Sheet, even on errors.
  // - If normal append fails, append an emergency error row to a fallback sheet.
  // - Do not fail silently; persist error details in sheet rows.
  let statusCode = 200;
  let responsePayload = { ok: true };
  let processingStatus = 'success';
  let errorMessage = '';
  let rawBody = '';
  let payload = {};
  let completedTask = getEmptyCompletedTaskDetails_();
  let eventId = '';
  let shouldMarkProcessed = false;

  try {
    if (!e || !e.postData || !e.postData.contents) {
      statusCode = 400;
      responsePayload = { ok: false, error: 'Missing request body' };
      processingStatus = 'error_missing_body';
      errorMessage = 'Missing request body';
      logWebhookStep_('missing_body', {});
    } else {
      rawBody = e.postData.contents;
      const headers = normalizeHeaders_(e);
      logWebhookStep_('received', { rawLength: rawBody.length });

      const verifyResult = verifyTodoistSignature_(rawBody, headers);
      if (!verifyResult.ok) {
        statusCode = 401;
        responsePayload = { ok: false, error: verifyResult.error };
        processingStatus = 'error_signature_failed';
        errorMessage = verifyResult.error;
        logWebhookStep_('signature_failed', { error: verifyResult.error });
      } else {
        if (verifyResult.warning) {
          processingStatus = 'warning_signature_missing';
          errorMessage = verifyResult.warning;
          responsePayload = { ok: true, warning: verifyResult.warning };
          logWebhookStep_('signature_warning', { warning: verifyResult.warning });
        }

        try {
          payload = JSON.parse(rawBody);
          completedTask = getCompletedTaskDetails_(payload);
          console.log('Received event:', payload.event_name, '| data:', JSON.stringify(payload.event_data));
        } catch (parseErr) {
          statusCode = 400;
          responsePayload = { ok: false, error: 'Invalid JSON body' };
          processingStatus = 'error_invalid_json';
          errorMessage = 'Invalid JSON body: ' + String(parseErr);
          logWebhookStep_('invalid_json', { message: String(parseErr) });
        }

        if (statusCode < 400) {
          if (!payload || payload.event_name !== CONFIG.EVENT_NAME) {
            processingStatus = 'ignored_unsupported_event';
            responsePayload = { ok: true, ignored: true, reason: 'Unsupported event' };
            console.log('Ignored event:', payload && payload.event_name ? payload.event_name : '');
            logWebhookStep_('ignored_event', { eventName: payload && payload.event_name ? payload.event_name : '' });
          } else {
            const eventData = payload.event_data || {};
            eventId = eventData.id || eventData.item_id || Utilities.getUuid();
            console.log('Processing event_id:', eventId, '| content:', eventData.content);

            if (isDuplicateEvent_(eventId)) {
              processingStatus = 'ignored_duplicate';
              responsePayload = { ok: true, duplicate: true };
              logWebhookStep_('duplicate', { eventId: eventId });
            } else {
              shouldMarkProcessed = true;
              logWebhookStep_('success', { eventId: eventId });
            }
          }
        }
      }
    }
  } catch (err) {
    statusCode = 500;
    responsePayload = { ok: false, error: String(err) };
    processingStatus = 'error_internal';
    errorMessage = String(err);
    logWebhookStep_('error', { message: String(err), stack: err && err.stack ? err.stack : '' });
    console.error('Webhook error:', err && err.stack ? err.stack : err);
  }

  let appendSucceeded = false;
  let appendDetails = null;
  try {
    logWebhookStep_('append_start', { eventId: eventId, taskId: completedTask.taskId || '', status: processingStatus });
    appendDetails = appendWebhookOutcomeWithFallback_(payload, completedTask, processingStatus, errorMessage, rawBody);
    appendSucceeded = appendDetails.ok;
  } catch (appendErr) {
    appendSucceeded = false;
    appendDetails = { ok: false, error: String(appendErr), location: '' };
  }

  if (!appendSucceeded) {
    logWebhookStep_('append_failed', {
      message: appendDetails && appendDetails.error ? appendDetails.error : 'Unknown append failure',
      location: appendDetails && appendDetails.location ? appendDetails.location : ''
    });

    if (statusCode < 500) {
      statusCode = 500;
      responsePayload = {
        ok: false,
        error: 'Failed to write spreadsheet row: ' + (appendDetails && appendDetails.error ? appendDetails.error : 'Unknown append failure')
      };
    }
  }

  if (shouldMarkProcessed && eventId && appendSucceeded) {
    markEventProcessed_(eventId);
  }

  return jsonResponse(statusCode, responsePayload);
}

function verifyTodoistSignature_(rawBody, headers) {
  const requireSignature = isSignatureVerificationRequired_();
  const signatureHeader =
    headers['x-todoist-hmac-sha256'] ||
    headers['x_todoist_hmac_sha256'];

  if (!signatureHeader) {
    if (requireSignature) {
      return { ok: false, error: 'Missing Todoist signature header' };
    }

    return {
      ok: true,
      warning: 'Missing Todoist signature header (allowed because TODOIST_REQUIRE_SIGNATURE is not true)'
    };
  }

  const clientSecret = getRequiredProperty_('TODOIST_CLIENT_SECRET');
  const bytes = Utilities.computeHmacSha256Signature(rawBody, clientSecret);
  const expected = Utilities.base64Encode(bytes);

  if (expected !== signatureHeader) {
    return { ok: false, error: 'Invalid Todoist signature' };
  }

  return { ok: true };
}

function isSignatureVerificationRequired_() {
  return PropertiesService
    .getScriptProperties()
    .getProperty('TODOIST_REQUIRE_SIGNATURE') === 'true';
}

function getCompletedTaskDetails_(payload) {
  const eventData = payload.event_data || {};
  return {
    taskId: eventData.id || eventData.item_id || eventData.task_id || '',
    content: eventData.content || '',
    projectId: eventData.project_id || '',
    completedAt: eventData.completed_at || payload.event_date || new Date().toISOString(),
    priority: eventData.priority || ''
  };
}

function getEmptyCompletedTaskDetails_() {
  return {
    taskId: '',
    content: '',
    projectId: '',
    completedAt: '',
    priority: ''
  };
}

function appendCompletedTaskToSheet_(payload, task, processingStatus, errorMessage, rawBody) {
  const sheetName = PropertiesService.getScriptProperties().getProperty('GOOGLE_SHEET_NAME') || 'Completed Tasks';
  const spreadsheet = resolveSpreadsheet_();
  const sheet = spreadsheet.getSheetByName(sheetName) || spreadsheet.insertSheet(sheetName);

  ensureHeader_(sheet);

  const now = new Date();

  const row = [
    now,
    payload.event_name || '',
    task.taskId || '',
    task.content || '',
    task.projectId || '',
    task.priority || '',
    task.completedAt || '',
    processingStatus || 'unknown',
    errorMessage || '',
    JSON.stringify(payload || {}),
    rawBody || ''
  ];

  sheet.appendRow(row);
}

function appendWebhookOutcomeWithFallback_(payload, task, processingStatus, errorMessage, rawBody) {
  try {
    appendCompletedTaskToSheet_(payload, task, processingStatus, errorMessage, rawBody);
    return { ok: true, location: 'Completed Tasks' };
  } catch (primaryErr) {
    const primaryErrorText = String(primaryErr);

    try {
      appendEmergencyRow_(payload, task, processingStatus, errorMessage, rawBody, primaryErrorText);
      return { ok: true, location: 'Webhook Emergency Logs (fallback)' };
    } catch (fallbackErr) {
      return {
        ok: false,
        location: 'none',
        error: 'Primary append failed: ' + primaryErrorText + ' | Fallback append failed: ' + String(fallbackErr)
      };
    }
  }
}

function appendEmergencyRow_(payload, task, processingStatus, errorMessage, rawBody, primaryAppendError) {
  const spreadsheet = resolveSpreadsheet_();
  const emergencySheet = spreadsheet.getSheetByName('Webhook Emergency Logs') || spreadsheet.insertSheet('Webhook Emergency Logs');

  if (emergencySheet.getLastRow() === 0) {
    emergencySheet.appendRow([
      'logged_at',
      'processing_status',
      'error_message',
      'primary_append_error',
      'event_name',
      'task_id',
      'task_content',
      'raw_payload',
      'raw_body'
    ]);
  }

  emergencySheet.appendRow([
    new Date(),
    processingStatus || 'unknown',
    errorMessage || '',
    primaryAppendError || '',
    (payload && payload.event_name) || '',
    (task && task.taskId) || '',
    (task && task.content) || '',
    JSON.stringify(payload || {}),
    rawBody || ''
  ]);
}

function resolveSpreadsheet_() {
  const configuredSheetId = PropertiesService.getScriptProperties().getProperty('GOOGLE_SHEET_ID');
  if (configuredSheetId) {
    try {
      return SpreadsheetApp.openById(configuredSheetId);
    } catch (openErr) {
      // Fall through to active spreadsheet fallback.
    }
  }

  const activeSpreadsheet = SpreadsheetApp.getActiveSpreadsheet();
  if (activeSpreadsheet) {
    return activeSpreadsheet;
  }

  throw new Error('No spreadsheet available. Set GOOGLE_SHEET_ID or bind this script to a spreadsheet.');
}

function ensureHeader_(sheet) {
  const expectedHeaders = [
    'logged_at',
    'event_name',
    'task_id',
    'task_content',
    'project_id',
    'priority',
    'completed_at',
    'processing_status',
    'error_message',
    'raw_payload',
    'raw_body'
  ];

  if (sheet.getLastRow() === 0) {
    sheet.appendRow(expectedHeaders);
  }
}

function logWebhookStep_(step, data) {
  try {
    const spreadsheet = resolveSpreadsheet_();
    const sheet = spreadsheet.getSheetByName('Webhook Logs') || spreadsheet.insertSheet('Webhook Logs');

    if (sheet.getLastRow() === 0) {
      sheet.appendRow(['logged_at', 'step', 'data']);
    }

    sheet.appendRow([new Date(), step, JSON.stringify(data || {})]);
  } catch (logErr) {
    console.warn('Failed to write debug log row:', logErr);
  }
}

function isDuplicateEvent_(eventId) {
  const cache = CacheService.getScriptCache();
  return cache.get(cacheKey_(eventId)) !== null;
}

function markEventProcessed_(eventId) {
  const cache = CacheService.getScriptCache();
  cache.put(cacheKey_(eventId), '1', CONFIG.EVENT_CACHE_TTL_SECONDS);
}

function cacheKey_(eventId) {
  return 'todoist-event-' + eventId;
}

function getRequiredProperty_(key) {
  const value = PropertiesService.getScriptProperties().getProperty(key);
  if (!value) {
    throw new Error('Missing script property: ' + key);
  }
  return value;
}

function normalizeHeaders_(e) {
  const result = {};
  const source = (e && e.headers) ? e.headers : {};

  Object.keys(source).forEach(function(key) {
    result[String(key).toLowerCase()] = source[key];
  });

  return result;
}

function startTodoistOAuth_() {
  const state = Utilities.getUuid();
  CacheService
    .getScriptCache()
    .put('todoist-oauth-state-' + state, '1', CONFIG.OAUTH_STATE_CACHE_TTL_SECONDS);

  const authUrl = buildTodoistAuthorizeUrl_(state);
  const html = HtmlService.createHtmlOutput(
    '<!doctype html><html><head><meta charset="utf-8"></head><body>' +
    '<p>Redirecting to Todoist authorization...</p>' +
    '<script>window.location.replace(' + JSON.stringify(authUrl) + ');</script>' +
    '<p>If not redirected, open this URL:<br><a href="' + authUrl + '">' + authUrl + '</a></p>' +
    '</body></html>'
  );

  return html;
}

function handleTodoistOAuthCallback_(params) {
  if (params.error) {
    return ContentService
      .createTextOutput(JSON.stringify({
        ok: false,
        error: 'Todoist authorization error',
        details: params.error
      }))
      .setMimeType(ContentService.MimeType.JSON);
  }

  const state = params.state || '';
  const code = params.code || '';

  if (!code) {
    return ContentService
      .createTextOutput(JSON.stringify({ ok: false, error: 'Missing OAuth code' }))
      .setMimeType(ContentService.MimeType.JSON);
  }

  const allowManualOAuth = PropertiesService
    .getScriptProperties()
    .getProperty('TODOIST_ALLOW_MANUAL_OAUTH') === 'true';

  if (!validateOAuthState_(state) && !allowManualOAuth) {
    return ContentService
      .createTextOutput(JSON.stringify({ ok: false, error: 'Invalid or expired OAuth state' }))
      .setMimeType(ContentService.MimeType.JSON);
  }

  const tokenResponse = exchangeTodoistCodeForToken_(code);
  const accessToken = tokenResponse.access_token;

  if (!accessToken) {
    return ContentService
      .createTextOutput(JSON.stringify({ ok: false, error: 'Missing access_token in token response', tokenResponse: tokenResponse }))
      .setMimeType(ContentService.MimeType.JSON);
  }

  const scriptProps = PropertiesService.getScriptProperties();
  scriptProps.setProperty('TODOIST_API_TOKEN', accessToken);

  // Verify the token by fetching user info to confirm which account was activated.
  let userInfo = {};
  try {
    const userResponse = UrlFetchApp.fetch('https://api.todoist.com/api/v1/user', {
      headers: { 'Authorization': 'Bearer ' + accessToken },
      muteHttpExceptions: true
    });
    if (userResponse.getResponseCode() === 200) {
      const u = JSON.parse(userResponse.getContentText());
      userInfo = { id: u.id, full_name: u.full_name, email: u.email };
    }
  } catch (verifyErr) {
    console.warn('Could not verify token via user info:', verifyErr);
  }

  return ContentService
    .createTextOutput(JSON.stringify({
      ok: true,
      message: 'Todoist OAuth completed. Webhooks are now activated for this account.',
      storedProperty: 'TODOIST_API_TOKEN',
      activatedUser: userInfo
    }))
    .setMimeType(ContentService.MimeType.JSON);
}

function buildTodoistAuthorizeUrl_(state) {
  const clientId = getRequiredProperty_('TODOIST_CLIENT_ID');
  const redirectUri = getTodoistRedirectUri_();
  const scope = PropertiesService.getScriptProperties().getProperty('TODOIST_OAUTH_SCOPE') || 'data:read';

  return 'https://app.todoist.com/oauth/authorize?' +
    'client_id=' + encodeURIComponent(clientId) +
    '&scope=' + encodeURIComponent(scope) +
    '&state=' + encodeURIComponent(state) +
    '&redirect_uri=' + encodeURIComponent(redirectUri);
}

function exchangeTodoistCodeForToken_(code) {
  const clientId = getRequiredProperty_('TODOIST_CLIENT_ID');
  const clientSecret = getRequiredProperty_('TODOIST_CLIENT_SECRET');
  const redirectUri = getTodoistRedirectUri_();

  const response = UrlFetchApp.fetch('https://api.todoist.com/oauth/access_token', {
    method: 'post',
    payload: {
      client_id: clientId,
      client_secret: clientSecret,
      code: code,
      redirect_uri: redirectUri
    },
    muteHttpExceptions: true
  });

  if (response.getResponseCode() >= 300) {
    throw new Error('Todoist token exchange failed: ' + response.getResponseCode() + ' ' + response.getContentText());
  }

  return JSON.parse(response.getContentText());
}

function getTodoistRedirectUri_() {
  const explicitRedirect = PropertiesService.getScriptProperties().getProperty('TODOIST_REDIRECT_URI');
  if (explicitRedirect) {
    return explicitRedirect;
  }

  const webAppUrl = ScriptApp.getService().getUrl();
  if (!webAppUrl) {
    throw new Error('Missing TODOIST_REDIRECT_URI and could not resolve Script web app URL');
  }

  return webAppUrl;
}

function validateOAuthState_(state) {
  if (!state) {
    return false;
  }

  const cache = CacheService.getScriptCache();
  const key = 'todoist-oauth-state-' + state;
  const exists = cache.get(key);
  if (!exists) {
    return false;
  }

  cache.remove(key);
  return true;
}

function jsonResponse(statusCode, payload) {
  // Apps Script Web Apps always return HTTP 200; include statusCode in body.
  return ContentService
    .createTextOutput(JSON.stringify({ statusCode: statusCode, payload: payload }))
    .setMimeType(ContentService.MimeType.JSON);
}

function testLocalWebhookParse_() {
  const samplePayload = {
    event_name: 'item:completed',
    event_date: new Date().toISOString(),
    event_data: {
      event_id: 'sample-event-id',
      id: '1234567890',
      content: 'Sample completed task'
    }
  };

  Logger.log(getCompletedTaskDetails_(samplePayload));
}
