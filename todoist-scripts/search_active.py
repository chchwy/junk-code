import requests
import os

API_TOKEN = os.environ.get("TODOIST_API_TOKEN", "YOUR_API_TOKEN_HERE")
headers = {"Authorization": f"Bearer {API_TOKEN}"}

# Fetch all active tasks using the official REST API v2
url = "https://api.todoist.com/rest/v2/tasks"
try:
    response = requests.get(url, headers=headers)
    if response.status_code == 200:
        tasks = response.json()
        print(f"Fetched {len(tasks)} active tasks.")
        found = False
        for task in tasks:
            content = task.get("content", "")
            # Check for the keyword
            if "換PP泥沙濾心" in content or "濾心" in content or "濾" in content:
                print(f"MATCH FOUND -> Task ID: {task['id']} | Content: {content}")
                found = True
        
        if not found:
            print("Keyword not found in your active tasks.")
    else:
        print(f"Error: Status code {response.status_code}")
except Exception as e:
    print(f"Exception: {e}")
