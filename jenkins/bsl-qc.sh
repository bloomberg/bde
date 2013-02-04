#!/bin/bash
# ----------------------------------------------------------------------------
# Copyright (C) 2012-2013 Bloomberg L.P.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
# ----------------------------------------------------------------------------
#
# Number of CPUs to use for parallel work
NUMCPUS=8

#
# Clear ccache statistics and workspace
ccache -z
git clean -fdx

# Run a build using Clang for warnings analysis
python tools/bsl_gyp.py
make -k -C build CXX="ccache /usr/bin/clang++" BUILDTYPE=Release all_libs 2>&1 | tee /tmp/clang-3.1-libs.log
make_result=${PIPESTATUS[0]}
if [ $make_result -ne 0 ]; then exit $make_result; fi
make -k -C build CXX="ccache /usr/bin/clang++" BUILDTYPE=Release all_tests_build 2>&1 | tee /tmp/clang-3.1-tests.log
make_result=${PIPESTATUS[0]}
if [ $make_result -ne 0 ]; then exit $make_result; fi

# Clear workspace
git clean -fdx

# Run a build using GCC for warning analysis
python tools/bsl_gyp.py
make -k -C build CXX="ccache /usr/bin/g++-4.7" BUILDTYPE=Release all_libs 2>&1 | tee /tmp/gcc-4.7-libs.log
make_result=${PIPESTATUS[0]}
if [ $make_result -ne 0 ]; then exit $make_result; fi
make -k -C build CXX="ccache /usr/bin/g++-4.7" BUILDTYPE=Release all_tests_build 2>&1 | tee /tmp/gcc-4.7-tests.log
make_result=${PIPESTATUS[0]}
if [ $make_result -ne 0 ]; then exit $make_result; fi

# Report ccache statistics
ccache -s

# Clear workspace
git clean -fdx

# Run a build using GCC for code coverage analysis
python tools/bsl_gyp.py
make -k -j${NUMCPUS} -C build CXX="ccache /usr/bin/g++-4.7" BUILDTYPE=Profile all_tests_build
make_result=${PIPESTATUS[0]}
if [ $make_result -ne 0 ]; then exit $make_result; fi

# Prepare for code coverage analysis
tools/bsl_coverage init -j${NUMCPUS}

# Prepare to run unit tests
export HOST=VM
export TIMEOUT=60
mkdir test-results

# Construct a Makefile so unit tests can be run in parallel
echo "result-list = " > unit-tests.mk
for bsltest in `ls build/Profile/*.t | sort`
do
    result=test-results/${bsltest##*/}-junit.xml
    echo "result-list += ${result}" >> unit-tests.mk
    echo "${result}: ${bsltest}" >> unit-tests.mk
    echo -e "\tpython tools/run_unit_tests.py ${bsltest} /dev/null --abi=64 --lib=static_library --junit=${result}" >> unit-tests.mk
done
echo "test: \$(result-list)" >> unit-tests.mk

# Run unit tests
make -j${NUMCPUS} -f unit-tests.mk test

# Move logs from /tmp to workspace for analysis
mv /tmp/gcc-4.7-*.log .
mv /tmp/clang-3.1-*.log .

# Compute SLOC
sloccount --wide --details groups > sloccount.sc

# Run cppcheck analysis
#cppcheck -j${NUMCPUS} --xml-version=2 groups 2> cppcheck-result.xml

# Perform code coverage analysis
# (Note: 'gather' step fails if run in parallel)
tools/bsl_coverage gather
tools/bsl_coverage coverage -j${NUMCPUS}
