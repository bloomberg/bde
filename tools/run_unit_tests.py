#!/usr/bin/env python

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

# Usage: (python2.6) run_unit_tests.py test_driver.t test_driver_flag_file [--abi=<bits>] [--libs=<static or shared>] [--junit=junitoutput.xml]
# Arguments: test (*.t) and target (*.t.ran)
# Options  : --abi=<ABI_bits setting>
#            --libs=<static_library|shared_library>
#            --junit=<JUnit XML output file>

import os
import sys
import subprocess
import platform
import re
import threading

from datetime import datetime
from optparse import OptionParser

import xml.etree.ElementTree as ET

def parseOptions():
    parser = OptionParser()
    parser.add_option("--abi", dest="abi")
    parser.add_option("--lib", dest="lib")
    parser.add_option("--junit", dest="junit")
    parser.add_option("--valgrind", dest="valgrind")
    parser.add_option("--verbosity", type='int', default=None, dest='verbosity')
    parser.add_option("--timeout", type='int', default=None, dest='timeout')
    (options, args) = parser.parse_args()

    return options

commandLineOptions = parseOptions()

class TextOutputGenerator:
    msbuildErrorMarker = re.compile("Error([^:]*):")

    def __init__(self):
        self.verbose = False

        if PolicyFilter.criteria['OS'] == 'Windows':
            self.reformatWarnings = True
        else:
            self.reformatWarnings = False
        return

    def reformatWindowsOutput(self, line):
        # Reformat lines containing 'Error.*:' on Windows,
        # because msbuild will interpret them as test failures
        # even when they are legitimate test output.
        match = TextOutputGenerator.msbuildErrorMarker.search(line)
        if match:
            return "%sError%s - %s\n" % (line[:match.start(0)], line[match.start(1):match.end(1)], line[match.end(0):])
        else:
            return line

    def startTestSuite(self, name, verbosity, timeout):
        if verbosity >= 0:
            self.verbose = True
        return

    def endTestSuite(self, returncode):
        return returncode

    def reportTestSuiteError(self, variable, value):
        print >>sys.stderr, "Invalid value for environment variable '%s': %s" % (variable, value)
        return

    def startTestCase(self, testNumber):
        return

    def endTestCase(self):
        return

    def skipTestCase(self, command):
        if self.verbose:
            print "Skipping '%s'\n" % (command)
        return

    def runTestCase(self, command):
        if self.verbose:
            print "Running '%s'\n" % (command)
        return

    def startTestCaseOutput(self):
        return

    def printTestCaseOutput(self, line):
        if self.reformatWarnings:
            print self.reformatWindowsOutput(line) ,
        else:
            print line ,
        return

    def endTestCaseOutput(self):
        return

    def reportTestCaseError(self, prefix, command, testNumber, error):
        print >>sys.stderr, "Failed to run %s %d: %s" \
            % (sys.argv[1], testNumber, error)
        return

    def reportTestCaseFailure(self, returncode):
        print >>sys.stderr, "Abnormal test failure: %d" % (returncode)
        return

    def reportExpectedTestCaseFailure(self, test, testNumber, returncode):
        print >>sys.stderr, "Test failure for case %d of test %s was expected."\
                                                 % (testNumber, test)
        return

    def reportTestCaseTimeout(self):
        print >>sys.stderr, "Aborting test due to timeout"
        return

class JUnitOutputGenerator:
    def __init__(self, fileName):
        self.fileName = fileName
        self.suite = ET.Element('testsuite')
        ET.SubElement(self.suite, 'properties')
        self.tree = ET.ElementTree(self.suite)

        self.output = None
        self.currentCase = None

        self.testCount = 0
        self.failureCount = 0
        self.errorCount = 0
        self.skipCount = 0
        self.timestamp = datetime.utcnow()
        self.suite.set('timestamp', self.timestamp.isoformat())
        return

    def startTestSuite(self, name, verbosity, timeout):
        name = os.path.basename(name)
        name = name.partition(".")[0]
        self.suite.set('name', name.partition("_")[0] + "." + name.partition("_")[2])
        properties = self.suite.find('properties')
        verbosityProperty = ET.SubElement(properties, 'property')
        verbosityProperty.set('name', 'verbosity')
        verbosityProperty.set('value', '%d' % verbosity)
        timeoutProperty = ET.SubElement(properties, 'property')
        timeoutProperty.set('name', 'timeout')
        timeoutProperty.set('value', '%d' % timeout)
        self.timeout = timeout
        return

    def endTestSuite(self, returncode):
        if not self.currentCase.get('status'):
            # Last case never closed: missing test case
            self.suite.remove(self.currentCase)

        self.suite.set('tests', '%d' % (self.testCount))
        self.suite.set('failures', '%d' % (self.failureCount))
        self.suite.set('errors', '%d' % (self.errorCount))
        self.suite.set('skipped', '%d' % (self.skipCount))

        endTimestamp = datetime.utcnow()
        delta = endTimestamp - self.timestamp
        self.suite.set('time', '%d.%0.6d' % (delta.days * 3600 * 24 + delta.seconds, delta.microseconds))

        self.tree.write(self.fileName)
        # JUnit-style output means test failures should not stop execution of further tests,
        # so set the returncode to zero here
        return 0

    def reportTestSuiteError(self, variable, value):
        print >>sys.stderr, "Invalid value for environment variable '%s': %s" % (variable, value)
        properties = self.suite.find('properties')
        verbosityProperty = ET.SubElement(properties, 'property')
        verbosityProperty.set('name', 'error')
        verbosityProperty.set('value', "Invalid value for environment variable '%s': %s" % (variable, value))
        self.errorCount += 1
        return

    def startTestCase(self, testNumber):
        testcase = ET.SubElement(self.suite, 'testcase')
        testcase.set('name', '%d' % testNumber)
        self.currentCase = testcase
        self.testCount += 1
        self.currentCaseStartTime = datetime.utcnow()
        return

    def endTestCase(self):
        if not self.currentCase.get('status'):
            self.currentCase.set('status', 'passed')

        endTimestamp = datetime.utcnow()
        delta = endTimestamp - self.currentCaseStartTime
        self.currentCase.set('time', '%d.%0.6d' % (delta.days * 3600 * 24 + delta.seconds, delta.microseconds))
        self.currentCaseStartTime = None

        return

    def skipTestCase(self, command):
        ET.SubElement(self.currentCase, 'skipped')
        self.skipCount += 1
        return

    def runTestCase(self, command):
        return

    def startTestCaseOutput(self):
        self.output = ET.SubElement(self.currentCase, 'system-out')
        self.outputLines = []
        return

    def printTestCaseOutput(self, line):
        self.outputLines.append(line)
        return

    def endTestCaseOutput(self):
        self.output.text = ''.join(self.outputLines)
        self.output = None
        self.outputLines = None
        return

    def reportTestCaseError(self, prefix, command, testNumber, error):
        error = ET.SubElement(self.currentCase, 'error')
        error.set('message', "%s %s %d: %s" \
            % (prefix, command, testNumber, error))
        self.currentCase.set('status', 'error')
        self.errorCount += 1
        return

    def reportTestCaseFailure(self, returncode):
        failure = ET.SubElement(self.currentCase, 'failure')
        failure.set('type', 'test failure')
        failure.set('message', "Abnormal test failure: %d" % (returncode))
        self.currentCase.set('status', 'failed')
        self.failureCount += 1
        return

    def reportExpectedTestCaseFailure(self, test, testNumber, returncode):
        self.currentCase.set('status', 'expected failure')
        return

    def reportTestCaseTimeout(self):
        failure = ET.SubElement(self.currentCase, 'failure')
        failure.set('type', 'timeout')
        failure.set('message', "Test timed out (timeout was set for %d seconds)" % self.timeout)
        self.currentCase.set('status', 'failed')
        self.failureCount += 1
        return

def selectOutputType(options):
    if options.junit:
        return JUnitOutputGenerator(options.junit)
    else:
        return TextOutputGenerator()

class Policy:
    test   = 0
    skip   = 1
    ignore = 2

def determineCriteria(options):
    criteria = {
        'case': None,
        'OS': None,
        'CXX': None,
        'BUILDTYPE': None,
        'HOST' : None,
        'ABI': None,
        'library': None
    }
    criteria['OS'] = platform.uname()[0]
    criteria['CXX'] = os.environ.get('CXX', failobj='')
    criteria['BUILDTYPE'] = os.environ.get('BUILDTYPE', failobj='Release')
    criteria['HOST'] = os.environ.get('HOST', failobj='Physical')
    criteria['ABI'] = options.abi
    criteria['library'] = options.lib

    return criteria

def parsePolicyTable():
    filterFilePath = os.path.join( \
        os.path.dirname(os.path.abspath(sys.argv[0])), \
        'test_filter.py')

    if os.path.isfile(filterFilePath):
        filterFile = open(filterFilePath, 'r')
        # Evaluate the filter dictionary
        # But do not allow the execution of any methods
        policyTable = eval(filterFile.read(), {'__builtins__':None}, {})
        filterFile.close()
        return policyTable
    else:
        # There is no filter file, so the filter will be empty
        return {}

def testAny(id, val):
    try:
        return id == val
    except:
        return False

def testCompiler(id, val):
    try:
        return val.endswith(id)
    except:
        return False

class PolicyFilter:
    tests = {
        'case': testAny,
        'OS': testAny,
        'CXX': testCompiler,
        'BUILDTYPE': testAny,
        'HOST' : testAny,
        'ABI': testAny,
        'library': testAny
    }
    criteria = determineCriteria(commandLineOptions)
    policyTable = parsePolicyTable()

    @staticmethod
    def testFilter(oldValue, test):
        if not oldValue:
            return False

        if test[0] == 'policy':
            return True

        result = apply(PolicyFilter.tests[test[0]], [ test[1], PolicyFilter.criteria[test[0]] ])
        return result

    @staticmethod
    def getTestPolicy(component, testNumber):
        component = os.path.basename(component)
        component = component.partition('.')[0]
        if component in PolicyFilter.policyTable:
            PolicyFilter.criteria['case'] = testNumber
            for test in PolicyFilter.policyTable[component]:
                if reduce(PolicyFilter.testFilter, test.items(), True):
                    # This test matches this filter
                    if 'policy' in test:
                        # If there is a bad policy specified in the test
                        # default to Policy.test so that the test case will
                        # have the opportunity to fail visibly.
                        return getattr(Policy, test['policy'], Policy.test)
                    else:
                        return Policy.ignore

        return Policy.test

class TimeoutControl:
    def cancel(self):
        if timeout > 0:
            self.timer.cancel()
        return

    def timedOut(self):
        with self.timeoutStatusLock:
            processTimedOut = self.isTimedOut
        return processTimedOut

    def killProcess(self, process):
        with self.timeoutStatusLock:
            self.isTimedOut = True
        try:
            process.kill()
        except OSError, e:
            if e.errno == 3:
                # No such process: the process died just before we tried to kill it
                pass
            else:
                out.reportTestCaseError("Failed to kill process on timeout", process.pid, -1, e)

    def __init__ (self, process, interval):
        self.isTimedOut = False
        self.timeoutStatusLock = threading.Lock()
        if interval > 0:
            self.timer = threading.Timer(interval, self.killProcess, [ process ])
            self.timer.start()
        return

class TestRunner:
    testOver = re.compile("^WARNING: CASE `[0-9]+' NOT FOUND.")

    @staticmethod
    def isLastLine(line):
        return TestRunner.testOver.match(line)

    @staticmethod
    def runTest(test, verbosityLevel, timeout, valgrind):
        failures = 0
        testNumber = 1

        while True:
            if PolicyFilter.criteria['OS'] == 'Windows' \
                    and not test.lower().endswith('.exe'):
                program = test + '.exe'
            else:
                program = test

            policy = PolicyFilter.getTestPolicy(test, testNumber)

            args = []

            if (valgrind):
                valgrindFile = valgrind % (testNumber)
                args.extend(['valgrind','--quiet','--tool=memcheck','--leak-check=yes','--xml=yes',"--xml-file=%s" % valgrindFile])

            args.extend([program, str(testNumber)])

            out.startTestCase(testNumber)
            if verbosityLevel >= 0:
                args.extend(['v' for n in range(verbosityLevel)])

            if policy == Policy.skip:
                out.skipTestCase(' '.join(args))
            else:
                out.runTestCase(' '.join(args))

                try:

                    child = subprocess.Popen(args, \
                                                 stdout=subprocess.PIPE, \
                                                 stderr=subprocess.STDOUT)
                    # Set timer
                    timer = TimeoutControl(child, timeout)

                    output = child.stdout.readline()
                    # N.B. EOF is represented as an empty string.
                    # Empty line is represented as "\n"
                    if output:
                        out.startTestCaseOutput()

                        while output:
                            if not TestRunner.isLastLine(output):
                                out.printTestCaseOutput(output)
                            output = child.stdout.readline()

                        out.endTestCaseOutput()
                    returncode = child.wait()

                    timer.cancel()

                    if timer.timedOut():
                        returncode = 126

                except OSError, e:
                    # Couldn't start process, probably missing file
                    out.reportTestCaseError("Failed to run", sys.argv[1], testNumber, e)
                    return -1
                except ValueError, e:
                    # Invalid argument
                    out.reportTestCaseError("Invalid argument to Popen while running", sys.argv[1], testNumber, e)
                    return -1
                except Exception, e:
                    out.reportTestCaseError("ERROR while executing", sys.argv[1], testNumber, e)
                    return -1

                # On Linux, at least, returncode is always forced to unsigned,
                # but on Windows, at least, returncode is signed.
                # Cygwin, bless its heart, sees a -1 return code as 127!
                if returncode == -1 or returncode == 255 or returncode == 127:
                    # Missing test.  We're done.
                    return failures

                if returncode == 126:
                    out.reportTestCaseTimeout()
                    failures += 1
                elif returncode != 0:
                    if policy == Policy.ignore:
                        out.reportExpectedTestCaseFailure(test, testNumber, returncode)
                    else:
                        # Test failure. Report it.
                        out.reportTestCaseFailure(returncode)
                        failures += 1

            out.endTestCase()
            # Next test
            testNumber += 1


if __name__ == '__main__':
    out = None
    out = selectOutputType(commandLineOptions)

    verbosityLevel = -1
    if commandLineOptions.verbosity:
        verbosityLevel = commandLineOptions.verbosity

    # Timeout is specified in seconds
    timeout = 0
    if commandLineOptions.timeout:
        timeout = commandLineOptions.timeout

    out.startTestSuite(sys.argv[-1], verbosityLevel, timeout)
    returncode = TestRunner.runTest(sys.argv[-1], verbosityLevel, timeout, commandLineOptions.valgrind)
    returncode = out.endTestSuite(returncode)

    sys.exit(returncode)
