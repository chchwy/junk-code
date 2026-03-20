#!/usr/bin/env python3
import requests
import sys
import fnmatch
import os
from datetime import datetime, timedelta

# --- CONFIGURATION ---
# Replace this with your actual Todoist API token
API_TOKEN = os.environ.get("TODOIST_API_TOKEN", "YOUR_API_TOKEN_HERE")
# ---------------------

def fetch_completed_tasks(six_months_ago_str):
    url = 'https://api.todoist.com/api/v1/tasks/completed'
    headers = {
        'Authorization': f'Bearer {API_TOKEN}'
    }
    
    all_tasks = []
    limit = 200 # maximum limit allowed by Todoist API
    offset = 0
    
    while True:
        params = {
            'limit': limit,
            'offset': offset
        }
        
        response = requests.get(url, headers=headers, params=params)
        
        if response.status_code != 200:
            print(f"Error fetching tasks from Todoist API: {response.status_code}")
            print(response.text)
            sys.exit(1)
            
        data = response.json()
        items = data.get('items', [])
        all_tasks.extend(items)
        
        # If we received fewer items than the limit, we've reached the end
        if len(items) < limit:
            break
            
        offset += limit
        
    return all_tasks

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 search_todoist.py <keyword_with_wildcards>")
        print("Example 1 (substring): python3 search_todoist.py report")
        print("Example 2 (wildcard):  python3 search_todoist.py 'buy*milk'")
        sys.exit(1)
        
    if API_TOKEN == "YOUR_API_TOKEN_HERE":
        print("ERROR: Please open this script and replace 'YOUR_API_TOKEN_HERE' with your actual Todoist API token.")
        sys.exit(1)

    # Pattern provided by user via command line
    pattern = sys.argv[1].lower()
    
    # Calculate the date for 1 year ago (approx 365 days)
    from datetime import timezone
    six_months_ago = datetime.now(timezone.utc) - timedelta(days=365)
    
    # The Todoist API 'since' parameter accepts ISO 8601 strings
    since_str = six_months_ago.strftime('%Y-%m-%dT%H:%M:%SZ')
    
    print("Fetching absolutely all completed tasks available...")
    
    tasks = fetch_completed_tasks(None)
    
    print(f"API returned {len(tasks)} completed tasks total.")
    print(f"Scanning for pattern: '{pattern}' ...\n")
    
    matched_tasks = []
    for task in tasks:
        content = task.get('content', '')
        # Adding '*' around the pattern to behave as a substring match while supporting internal wildcards
        # e.g., 'report' becomes '*report*' (substring), 'rep*t' becomes '*rep*t*'
        if fnmatch.fnmatch(content.lower(), f"*{pattern}*"):
            matched_tasks.append(task)
            
    if not matched_tasks:
        print("No matching tasks found.")
    else:
        print(f"Found {len(matched_tasks)} matching task(s):")
        print("-" * 50)
        
        # Sort tasks by completion date (newest first)
        matched_tasks.sort(key=lambda x: x.get('completed_at', ''), reverse=True)
        
        for idx, task in enumerate(matched_tasks, 1):
            # Parse and format the output Date nicely
            completed_str = task.get('completed_at', 'Unknown Date')
            try:
                # String generally looks like "2023-11-04T12:00:00.000000Z"
                dt_str = completed_str.split('.')[0].replace('Z', '')
                dt = datetime.strptime(dt_str, '%Y-%m-%dT%H:%M:%S')
                formatted_date = dt.strftime('%b %d, %Y %I:%M %p')
            except ValueError:
                formatted_date = completed_str
                
            print(f"{idx:2d}. [{formatted_date}] {task.get('content')}")
            
if __name__ == "__main__":
    main()
