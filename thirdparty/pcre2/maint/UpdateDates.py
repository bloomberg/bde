#! /usr/bin/env python3

# Script to update all the hardcoded dates in the source tree.
#  - Documentation manpages have a "last updated" header and footer.
#  - So do the READMEs.
#  - The source files have copyright headers.

# This script should be run in the main PCRE2 directory.

import glob
import re
import subprocess

from UpdateCommon import update_file

date_regex = r'\d+ (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)\w* \d+'
header_regex = r'(?m)^(.TH.*? )"%s"' % date_regex
last_updated_regex = r'(?m)^Last updated: %s' % date_regex

def get_last_date(filename):
    result = subprocess.run(['git', 'log', '-n1', '--date=format:%d %B %Y', '--format=%cd', '--grep', '#noupdate', '--invert-grep', filename], capture_output=True, text=True)
    return result.stdout.strip()

def check_no_match(filename, pattern):
    with open(filename, 'r') as file:
        content = file.read()

    if re.search(pattern, content):
        raise Exception('Pattern unexpectedly found in %s' % filename)

def update_man_date(filename):
    print('  Updating %s' % filename)
    file_date = get_last_date(filename)

    update_file(filename, header_regex, '\\1"%s"' % file_date)

    if filename.startswith('doc/pcre2_') or filename == 'doc/pcre2demo.3':
        check_no_match(filename, last_updated_regex)
    else:
        update_file(filename, last_updated_regex, 'Last updated: %s' % file_date)

print('Updating man pages')

# doc/*.1
for filename in glob.glob('doc/*.1'):
    update_man_date(filename)

# doc/*.3
for filename in glob.glob('doc/*.3'):
    update_man_date(filename)

# README, NON-AUTOTOOLS-BUILD
print('Updating README and NON-AUTOTOOLS-BUILD')
for filename in ['README', 'NON-AUTOTOOLS-BUILD']:
    line = 'Last updated: %s' % get_last_date(filename)
    padding = '=' * len(line)
    update_file(filename, r'(?i)=+\nLast updated: .*?\n=+', '%s\n%s\n%s' % (padding, line, padding))
