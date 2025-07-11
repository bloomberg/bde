# Common helpers for UpdateRelease.py and UpdateDates.py.

import re

def get_current_release():
    with open('configure.ac', 'r') as file:
        content = file.read()

    matches = [match[1] for match in re.findall(r"m4_define\(pcre2_(major|minor|prerelease), \[(.*?)\]\)", content)]
    current_release = '%s.%s%s' % tuple(matches)

    return current_release

CURRENT_RELEASE = get_current_release()

# Update a file, using a pattern. Verify that it matches the file, and perform
# the replacement.
def update_file(filename, pattern, replacement):
    with open(filename, 'r') as file:
        content = file.read()

    if not re.search(pattern, content):
        raise Exception('Pattern not found in %s' % filename)

    content = re.sub(pattern, replacement, content)

    with open(filename, 'w') as file:
        file.write(content)
