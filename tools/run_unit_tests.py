#!/usr/bin/env python

# Arguments: test (*.t) and target (*.t.ran)
# Options  : --abi=<ABI_bits setting>
#            --libs=<static_library|shared_library>

import os
import sys
import subprocess
import platform
import re
import threading

from optparse import OptionParser

class Policy:
    test   = 0
    skip   = 1
    ignore = 2

def determineCriteria():
    parser = OptionParser()
    parser.add_option("--abi", dest="abi")
    parser.add_option("--lib", dest="lib")
    (options, args) = parser.parse_args()

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
    criteria = determineCriteria()
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
            print >>sys.stderr, "Aborting test due to timeout"
            self.isTimedOut = True
        process.kill()

    def __init__ (self, process, interval):
        self.isTimedOut = False
        self.timeoutStatusLock = threading.Lock()
        if interval > 0:
            self.timer = threading.Timer(interval, self.killProcess, [ process ])
            self.timer.start()
        return

class TestRunner:
    testOver = re.compile("^WARNING: CASE `[0-9]+' NOT FOUND.")
    msbuildErrorMarker = re.compile("Error([^:]*):")
    
    @staticmethod
    def reformatWindowsOutput(line):
        # Reformat lines containing 'Error.*:' on Windows,
        # because msbuild will interpret them as test failures
        # even when they are legitimate test output.
        match = TestRunner.msbuildErrorMarker.search(line)
        if match:
            return "%sError%s - %s\n" % (line[:match.start(0)], line[match.start(1):match.end(1)], line[match.end(0):])
        else:
            return line
            
    @staticmethod
    def printOutput(line):
        if PolicyFilter.criteria['OS'] == 'Windows':
            print >>sys.stderr, TestRunner.reformatWindowsOutput(line) ,
        else:
            print line ,

    @staticmethod
    def isLastLine(line):
        return TestRunner.testOver.match(line)

    @staticmethod
    def runTest(test, verbosityLevel, timeout):
        failures = 0
        testNumber = 1
        while True:
            if PolicyFilter.criteria['OS'] == 'Windows' \
                    and not test.lower().endswith('.exe'):
                program = test + '.exe'
            else:
                program = test
                
            policy = PolicyFilter.getTestPolicy(test, testNumber)
                
            args = [program, str(testNumber)]
            if verbosityLevel >= 0:
                args.extend(['v' for n in range(verbosityLevel)])
                if policy == Policy.skip:
                    TestRunner.printOutput("Skipping '%s'\n" % (' '.join(args)))
                else:
                    TestRunner.printOutput("Running '%s'\n" % (' '.join(args)))
                        
            if policy != Policy.skip:
                try:
                    
                    child = subprocess.Popen(args, \
                                                 stdout=subprocess.PIPE, \
                                                 stderr=subprocess.STDOUT)
                    # Set timer
                    timer = TimeoutControl(child, timeout)

                    output = child.stdout.readline()
                    # N.B. EOF is represented as an empty string.
                    # Empty line is represented as "\n"
                    while output:
                        if not TestRunner.isLastLine(output):
                            TestRunner.printOutput(output)
                        output = child.stdout.readline()

                    returncode = child.wait()

                    timer.cancel()

                    if timer.timedOut():
                        return 126
                    
                except OSError, e:
                    # Couldn't start process, probably missing file
                    print >>sys.stderr, "Failed to run %s %d: %s" \
                        % (sys.argv[1], testNumber, e)
                    return -1
                except ValueError, e:
                    # Invalid argument
                    print >>sys.stderr, "Invalid argument to Popen while running %s %d: %s" \
                        % (sys.argv[1], testNumber, e)
                    return -1
                except Exception, e:
                    print >>sys.stderr, "ERROR while executing %s %d: %s" \
                        % (sys.argv[1], testNumber, e)
                    return -1
                
                # On Linux, at least, returncode is always forced to unsigned,
                # but on Windows, at least, returncode is signed.
                # Cygwin, bless its heart, sees a -1 return code as 127!
                if returncode == -1 or returncode == 255 or returncode == 127:
                    # Missing test.  We're done.
                    return failures
                
                if returncode != 0:
                    if policy == Policy.ignore:
                        print >>sys.stderr, "Test failure for case %d of test %s was expected."\
                                                 % (testNumber, test)
                    else:
                        # Test failure. Report it.
                        print >>sys.stderr, "Abnormal test failure: %d" % (returncode)
                        failures += 1

            # Next test
            testNumber += 1

verbosityLevel = -1
if 'V' in os.environ:
    try:
        verbosityLevel = int(os.environ['V'])
    except ValueError:
        print >>sys.stderr, "Invalid value for environment variable 'V': %s" % (os.environ['V'])

# Timeout is specified in seconds
timeout = 0
if 'TIMEOUT' in os.environ:
    try:
        timeout = int(os.environ['TIMEOUT'])
    except ValueError:
        print >>sys.stderr, "Invalid value for environment variable 'TIMEOUT': %s" % (os.environ['TIMEOUT'])
    

returncode = TestRunner.runTest(sys.argv[1], verbosityLevel, timeout)

if returncode == 0:
    # We passed the test, mark the target
    target = open(sys.argv[2], 'w')
    target.close()

sys.exit(returncode)
