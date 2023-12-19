import re

# Define a translation dictionary for information lines
dict1 = {
    "一": "Monday",
    "二": "Tuesday",
    "三": "Wednesday",
    "四": "Thursday",
    "五": "Friday",
    "六": "Saturday",
    "日": "Sunday",
    "上午": "AM ",
    "下午": "PM ",
    "1": "January",
    "2": "February",
    "3": "March",
    "4": "April",
    "5": "May",
    "6": "June",
    "7": "July",
    "8": "August",
    "9": "September",
    "10": "October",
    "11": "November",
    "12": "December",
}


# Function to translate a Chinese information line to English
def translate_information_line(line):
    # Example information line:
    # - 您在位置 #158-158的标注 | 添加于 2016年7月29日星期五 上午11:35:56
    # Replace the line to the following:
    # - Highlight Loc. 158-158 | Added on Friday, July 29, 2016, 11:35 AM

    # Extract the information from the line    
    match = re.search(r"您在位置 #(\d+-\d+)的标注 \| 添加于 (\d+)年(\d+)月(\d+)日星期(.) ([上|下]午)(\d+):(\d+):(\d+)", line)
    location = match.group(1)
    year = match.group(2)
    month = match.group(3)
    day = match.group(4)
    weekday = match.group(5)
    am_pm = match.group(6)
    hour = match.group(7)
    minute = match.group(8)
    second = match.group(9)

    #print(location, weekday, month, day, year, hour, minute, second, am_pm)

    # Translate the information
    translated_line = f"- Highlight Loc. {location} | Added on {dict1[weekday]}, {dict1[month]} {day}, {year}, {hour}:{minute} {dict1[am_pm]}"
    #print("Translating the following line: ")
    #print(line)
    #print(translated_line)

    return translated_line

def translate_information_line2(line):
    # Example information line:
    # - 您在第 20 页（位置 #493-496）的标注 | 添加于 2019年1月1日星期二 上午2:10:43
    # Replace the line to the following:
    # - Highlight Loc. 493-496 | Added on Tuesday, January 1, 2019, 2:10 AM

    # Extract the information from the line
    match = re.search(r"您在第 (\d+) 页（位置 #(\d+-\d+)）的标注 \| 添加于 (\d+)年(\d+)月(\d+)日星期(.) ([上|下]午)(\d+):(\d+):(\d+)", line)
    page = match.group(1)
    location = match.group(2)
    year = match.group(3)
    month = match.group(4)
    day = match.group(5)
    weekday = match.group(6)
    am_pm = match.group(7)
    hour = match.group(8)
    minute = match.group(9)
    second = match.group(10)

    print(page, location, weekday, month, day, year, hour, minute, second, am_pm)

    # Translate the information
    translated_line = f"- Highlight Loc. {location} | Added on {dict1[weekday]}, {dict1[month]} {day}, {year}, {hour}:{minute} {dict1[am_pm]}"
    print("Translating the following line: ")
    print(line)
    print(translated_line)

    return translated_line


# Read the Kindle highlights file
with open('My Clippings.txt', 'r', encoding='utf-8') as file:
    contents = file.readlines()    

# Process each record
translated_lines = []
for line in contents:
    
    if "- 您在位置" in line:
        # Translate the information line to English
        translated_line = translate_information_line(line)
        translated_lines.append(translated_line + '\n')
    elif "- 您在第" in line:
        # Translate the information line to English
        translated_line = translate_information_line2(line)
        translated_lines.append(translated_line + '\n')
    else:
        translated_lines.append(line)

# Reconstruct the content with translated records
translated_content = ''.join(translated_lines)

# Write the translated content back to the file
with open('MyClippings.out.txt', 'w', encoding='utf-8') as file:
    file.write(translated_content)

print("Translation complete. The corrected translated content is saved to 'translated_My_Clippings.txt'.")
