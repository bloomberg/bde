#! /usr/bin/env python3

# Script to update all the hardcoded release numbers in the source tree.
#  - Documentation manpages.
#  - Bazel MODULE file.

# This script should be run in the main PCRE2 directory.

import glob

from UpdateCommon import update_file, CURRENT_RELEASE

def update_man_version(filename):
    print('  Updating %s' % filename)
    update_file(filename, r'(.TH.*? )"PCRE2 .*?"', '\\1"PCRE2 %s"' % CURRENT_RELEASE)

print('Updating man pages')

# doc/*.1
for filename in glob.glob('doc/*.1'):
    update_man_version(filename)

# doc/*.3
for filename in glob.glob('doc/*.3'):
    update_man_version(filename)

# MODULE.bazel
print('Updating MODULE.bazel')
update_file('MODULE.bazel', r'(?m)^    version = ".*?"', '    version = "%s"' % CURRENT_RELEASE)
