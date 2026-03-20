#!/usr/bin/env python3
import requests
import sys
import os

API_TOKEN = os.environ.get("TODOIST_API_TOKEN", "YOUR_API_TOKEN_HERE")
HEADERS = {"Authorization": f"Bearer {API_TOKEN}"}
TASK_ID = "6Vh478xXmfQ3q2p6"

def query_activity():
    url = "https://api.todoist.com/api/v1/activities"
    print(f"Querying activity log for Task ID {TASK_ID}...")
    
    matching_events = []
    cursor = None
    pages_fetched = 0
    max_pages = 50 # 5,000 events max
    
    while pages_fetched < max_pages:
        params = {"limit": 100}
        if cursor:
            params["cursor"] = cursor
            
        sys.stdout.write(f"\rFetching page {pages_fetched + 1}... ")
        sys.stdout.flush()
        response = requests.get(url, headers=HEADERS, params=params)
        
        if response.status_code != 200:
            print(f"\nError {response.status_code} fetching global activity logs.")
            return

        data = response.json()
        events = data.get("results", [])
        
        for event in events:
            if str(event.get("object_id", "")) == TASK_ID:
                matching_events.append(event)
                event_time = event.get("event_date", "Unknown time")
                event_type = event.get("event_type", "Unknown event type")
                # clear line and print
                print(f"\n--> FOUND MATCH: [{event_time}] Action: {event_type}")
                
        cursor = data.get("next_cursor")
        if not cursor or len(events) == 0:
            break
            
        pages_fetched += 1
            
    print(f"\nFinished scanning. Found {len(matching_events)} relevant events.")
        
    print(f"\nFound {len(matching_events)} activity events for this task:")
    for idx, event in enumerate(matching_events, 1):
        event_time = event.get("event_date", "Unknown time")
        event_type = event.get("event_type", "Unknown event type")
        print(f" {idx}. [{event_time}] Action: {event_type}")
        
if __name__ == "__main__":
    query_activity()
