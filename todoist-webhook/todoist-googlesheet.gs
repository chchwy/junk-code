const SHEET_NAME = PropertiesService.getScriptProperties().getProperty('GOOGLE_SHEET_NAME') || 'Completed Tasks';
const FALLBACK_SHEET_NAME = 'Webhook Emergency Logs';

function doGet() {
  return ContentService
    .createTextOutput(JSON.stringify({ ok: true, message: 'Todoist webhook is live' }))
    .setMimeType(ContentService.MimeType.JSON);
}

function doPost(e) {
  // USER RULE (keep this comment):
  // 1) DO NOT early return from webhook processing.
  // 2) Always append something to Google Sheet.
  // 3) If anything fails, append the error message to Google Sheet.
  let status = 'success', error = '', task = {};

  try {
    const body = e && e.postData && e.postData.contents;
    if (!body) throw new Error('Missing request body');

    const payload = JSON.parse(body);
    if (payload.event_name !== 'item:completed') {
      status = 'ignored';
      error = 'Unsupported event: ' + (payload.event_name || '');
    } else {
      const d = payload.event_data || {};
      const taskId = d.id || d.item_id || d.task_id || '';
      task = {
        taskId,
        content: d.content || '',
        projectId: d.project_id || '',
        priority: d.priority || '',
        completedAt: d.completed_at || payload.triggered_at || new Date().toISOString(),
        labels: (d.labels || []).join(', ')
      };
      task = enrichFromApi_(task);
    }
  } catch (err) {
    status = 'error';
    error = String(err);
  }

  try {
    appendRow_(task, status, error);
  } catch (appendErr) {
    try {
      appendFallback_(task, status, error, String(appendErr));
    } catch (_) { }
  }

  return ContentService
    .createTextOutput(JSON.stringify({ ok: status !== 'error', status, error }))
    .setMimeType(ContentService.MimeType.JSON);
}

function enrichFromApi_(task) {
  const token = PropertiesService.getScriptProperties().getProperty('TODOIST_API_TOKEN');
  if (!token || !task.taskId) return task;

  try {
    const res = UrlFetchApp.fetch(
      'https://api.todoist.com/sync/v9/items/get?item_id=' + encodeURIComponent(task.taskId),
      { headers: { 'Authorization': 'Bearer ' + token }, muteHttpExceptions: true }
    );
    if (res.getResponseCode() === 200) {
      const item = JSON.parse(res.getContentText()).item || {};
      task.labels = (item.labels || []).join(', ');
      task.content = item.content || task.content;
      task.priority = item.priority || task.priority;
    }
  } catch (err) {
    console.error('API enrichment failed: ' + err);
  }
  return task;
}

function appendRow_(task, status, error) {
  const sheet = getSheet_(SHEET_NAME);
  sheet.appendRow([
    task.completedAt || '',
    task.content || '',
    task.priority || '',
    task.labels || '',
    task.taskId || '',
    task.projectId || '',
    status,
    error,
    new Date()
  ]);
}

function appendFallback_(task, status, error, appendError) {
  const sheet = getSheet_(FALLBACK_SHEET_NAME);
  sheet.appendRow([new Date(), status, error, appendError, task.taskId || '', task.content || '']);
}

function getSheet_(name) {
  const ss = (() => {
    const id = PropertiesService.getScriptProperties().getProperty('GOOGLE_SHEET_ID');
    if (id) try { return SpreadsheetApp.openById(id); } catch (_) { }
    return SpreadsheetApp.getActiveSpreadsheet();
  })();
  return ss.getSheetByName(name) || ss.insertSheet(name);
}
