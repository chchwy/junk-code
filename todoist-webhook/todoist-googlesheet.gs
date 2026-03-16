const CONFIG = {
  EVENT_NAME: 'item:completed',
  MAIN_SHEET_NAME: 'Completed Tasks',
  FALLBACK_SHEET_NAME: 'Webhook Emergency Logs'
};

function doGet() {
  return ContentService
    .createTextOutput(JSON.stringify({
      ok: true,
      message: 'Todoist completed-task webhook is live',
      note: 'Send POST requests with Todoist webhook JSON payload'
    }))
    .setMimeType(ContentService.MimeType.JSON);
}

function doPost(e) {
  // USER RULE (keep this comment):
  // 1) DO NOT early return from webhook processing.
  // 2) Always append something to Google Sheet.
  // 3) If anything fails, append the error message to Google Sheet.
  let statusCode = 200;
  let processingStatus = 'success';
  let errorMessage = '';
  let rawBody = '';
  let payload = {};
  let task = {
    taskId: '',
    content: '',
    projectId: '',
    priority: '',
    completedAt: '',
    initiatorName: '',
    initiatorEmail: ''
  };

  try {
    if (!e || !e.postData || !e.postData.contents) {
      statusCode = 400;
      processingStatus = 'error_missing_body';
      errorMessage = 'Missing request body';
    } else {
      rawBody = e.postData.contents;

      try {
        payload = JSON.parse(rawBody);
      } catch (parseErr) {
        statusCode = 400;
        processingStatus = 'error_invalid_json';
        errorMessage = 'Invalid JSON body: ' + String(parseErr);
      }

      if (!errorMessage) {
        if (!payload || payload.event_name !== CONFIG.EVENT_NAME) {
          processingStatus = 'ignored_unsupported_event';
          errorMessage = 'Unsupported event: ' + String(payload && payload.event_name ? payload.event_name : '');
        } else {
          // Extract base task from webhook
          const baseTask = extractTask_(payload);

          // Enrich task data via API call
          task = fetchEnrichedTaskDetails_(baseTask);

          processingStatus = 'success';
        }
      }
    }
  } catch (err) {
    statusCode = 500;
    processingStatus = 'error_internal';
    errorMessage = String(err);
  }

  let appendResult = { ok: false, location: 'none', error: '' };
  try {
    appendResult = appendOutcomeWithFallback_(payload, task, processingStatus, errorMessage, rawBody);
  } catch (appendErr) {
    appendResult = { ok: false, location: 'none', error: String(appendErr) };
  }

  if (!appendResult.ok) {
    statusCode = 500;
    if (errorMessage) {
      errorMessage = errorMessage + ' | append_failed: ' + appendResult.error;
    } else {
      errorMessage = 'append_failed: ' + appendResult.error;
    }
    processingStatus = 'error_append_failed';
  }

  return ContentService
    .createTextOutput(JSON.stringify({
      statusCode: statusCode,
      payload: {
        ok: statusCode < 400,
        processingStatus: processingStatus,
        errorMessage: errorMessage,
        append: appendResult
      }
    }))
    .setMimeType(ContentService.MimeType.JSON);
}

function extractTask_(payload) {
  const eventData = payload.event_data || {};

  return {
    taskId: eventData.id || eventData.item_id || eventData.task_id || '',
    content: eventData.content || '',
    projectId: eventData.project_id || '',
    priority: eventData.priority || '',
    completedAt: eventData.completed_at || payload.triggered_at || new Date().toISOString(),
    labels: (eventData.labels || []).join(', ') // Always a string, never a raw array
  };
}

/**
 * Enriches the task data by fetching full item details from the Todoist Sync API.
 * The webhook's item:completed payload often omits fields like labels,
 * so we call items/get to retrieve the complete item object.
 *
 * Labels in the Todoist API (v9+) are plain name strings, not IDs.
 * e.g. ["Food", "Shopping"] — no mapping needed.
 */
function fetchEnrichedTaskDetails_(task) {
  const token = PropertiesService.getScriptProperties().getProperty('TODOIST_API_TOKEN');
  if (!token || !task.taskId) return task;

  try {
    const response = UrlFetchApp.fetch(
      'https://api.todoist.com/sync/v9/items/get?item_id=' + encodeURIComponent(task.taskId),
      {
        headers: { 'Authorization': 'Bearer ' + token },
        muteHttpExceptions: true
      }
    );

    if (response.getResponseCode() === 200) {
      const data = JSON.parse(response.getContentText());
      const item = data.item || {};

      // Override with richer data from the full item response
      task.labels = (item.labels || []).join(', ');
      task.description = item.description || '';
      task.content = item.content || task.content;
      task.priority = item.priority || task.priority;
    }
  } catch (err) {
    console.error('Enrichment failed: ' + err);
  }

  return task;
}

function appendOutcomeWithFallback_(payload, task, processingStatus, errorMessage, rawBody) {
  try {
    appendMainRow_(payload, task, processingStatus, errorMessage, rawBody);
    return { ok: true, location: CONFIG.MAIN_SHEET_NAME, error: '' };
  } catch (primaryErr) {
    const primaryErrorText = String(primaryErr);
    try {
      appendEmergencyRow_(payload, task, processingStatus, errorMessage, rawBody, primaryErrorText);
      return { ok: true, location: CONFIG.FALLBACK_SHEET_NAME, error: primaryErrorText };
    } catch (fallbackErr) {
      return {
        ok: false,
        location: 'none',
        error: 'Primary append failed: ' + primaryErrorText + ' | Fallback append failed: ' + String(fallbackErr)
      };
    }
  }
}

function appendMainRow_(payload, task, processingStatus, errorMessage, rawBody) {
  const spreadsheet = resolveSpreadsheet_();
  const sheetName = PropertiesService.getScriptProperties().getProperty('GOOGLE_SHEET_NAME') || CONFIG.MAIN_SHEET_NAME;
  const sheet = spreadsheet.getSheetByName(sheetName) || spreadsheet.insertSheet(sheetName);

  if (sheet.getLastRow() === 0) {
    sheet.appendRow([
      'completed_at',
      'task_content',
      'priority',
      'labels',
      'task_id',
      'project_id',
      'event_name',
      'processing_status',
      'error_message',
      'logged_at',
      'raw_payload'
    ]);
  }

  sheet.appendRow([
    task.completedAt || '',
    task.content || '',
    task.priority || '',
    task.labels || '',
    task.taskId || '',
    task.projectId || '',
    payload.event_name || '',
    processingStatus || 'unknown',
    errorMessage || '',
    new Date(),
    JSON.stringify(payload || {})
  ]);
}

function appendEmergencyRow_(payload, task, processingStatus, errorMessage, rawBody, primaryAppendError) {
  const spreadsheet = resolveSpreadsheet_();
  const sheet = spreadsheet.getSheetByName(CONFIG.FALLBACK_SHEET_NAME) || spreadsheet.insertSheet(CONFIG.FALLBACK_SHEET_NAME);

  if (sheet.getLastRow() === 0) {
    sheet.appendRow([
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

  sheet.appendRow([
    new Date(),
    processingStatus || 'unknown',
    errorMessage || '',
    primaryAppendError || '',
    payload.event_name || '',
    task.taskId || '',
    task.content || '',
    JSON.stringify(payload || {}),
    rawBody || ''
  ]);
}

function resolveSpreadsheet_() {
  const configuredSheetId = PropertiesService.getScriptProperties().getProperty('GOOGLE_SHEET_ID');

  if (configuredSheetId) {
    try {
      return SpreadsheetApp.openById(configuredSheetId);
    } catch (configuredErr) {
      // Continue to active spreadsheet fallback.
    }
  }

  try {
    const activeSpreadsheet = SpreadsheetApp.getActiveSpreadsheet();
    if (activeSpreadsheet) {
      return activeSpreadsheet;
    }
  } catch (activeErr) {
    // Continue to final error.
  }

  throw new Error('No spreadsheet available. Set GOOGLE_SHEET_ID or bind script to a spreadsheet.');
}
