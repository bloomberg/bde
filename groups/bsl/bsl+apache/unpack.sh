#! /bin/bash

# Instructions for unpacking an apache distribution into a flattened test
# directory.  Run in the bsl/bsl+apache directory.
#
# Usage: unpack.sh <tarfile>
#   tarfile - The apache STL tar file from
#             http://stdcxx.apache.org/download.html

tarfile=$1
if [ ! -s "$tarfile" ]; then
    echo >&2 usage "unpack.sh <tarfile>"
fi

top=$PWD
mkdir -p test testinc

gtar xzf $tarfile 'stdcxx-*/tests'	# Unpack tests directory
tests=$(ls -d stdcxx*/tests)		# Root directory of tests in tar file
mv $tests/src/* $top			# Move test-library srcs to top-level
mv $tests/include/* $top/testinc	# Move test-library hdrs to testinc

find $tests -type f -print | xargs -I X mv X test # Flatten test directories
rm -r stdcxx-*					  # remove empty dirs
