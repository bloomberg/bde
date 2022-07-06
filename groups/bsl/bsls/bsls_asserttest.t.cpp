// bsls_asserttest.t.cpp                                              -*-C++-*-
#include <bsls_asserttest.h>

#include <stdio.h>
// '<stdio.h>' **must** be included before anything else that may
// '#include <cstdio>', otherwise, on SunOS, the headers in '/usr/include' will
// hide 'popen'/'pclose' (as well as any name that is not in C++03).  Due
// already existing type definitions it is not even possible to bring those
// names in using header even if we undefined guard macros.

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_log.h>
#include <bsls_macroincrement.h>
#include <bsls_platform.h>

// We limit ourselves to direct access to the standard C library for packages
// in 'bsl'.  This also means no C++ '<cstdio>' etc headers as those may not be
// present (theoretically) or more often are broken enough that we can't use
// them (see the long '#include <stdio.h>' comment above).
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Platform specific headers
#ifdef BSLS_PLATFORM_OS_UNIX
#include <unistd.h>  // dup, dup2, close
//#endif
#elif BSLS_PLATFORM_OS_WINDOWS
#include <io.h>
#else
#warning Unknown platform, you will probably get a compilation error.
#endif

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -MN04   // Pointer members must end in '_p'
#endif

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component contains a few supporting functions for driving negative
// testing of assertions.  Tests will be a mix of validating that those
// functions work as expected with all inputs, and that the macros that call
// them work in various build modes.  The macro tests are the most unorthodox
// (similar to the tests for 'bsls_assert' and 'bsls_review') and involve
// numerous re-includes of this component's header after undefining the macros
// (using 'bsls_asserttest_macroreset.h') and then defining many permutations
// of the macros that control assert header behavior.
//-----------------------------------------------------------------------------
// [ 9] BSLS_ASSERTTEST_ASSERT_SAFE_PASS(FUNCTION)
// [ 9] BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(FUNCTION)
// [ 9] BSLS_ASSERTTEST_ASSERT_PASS(FUNCTION)
// [ 9] BSLS_ASSERTTEST_ASSERT_FAIL(FUNCTION)
// [ 9] BSLS_ASSERTTEST_ASSERT_OPT_PASS(FUNCTION)
// [ 9] BSLS_ASSERTTEST_ASSERT_OPT_FAIL(FUNCTION)
//
// [10] BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(FUNCTION)
// [10] BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(FUNCTION)
// [10] BSLS_ASSERTTEST_ASSERT_PASS_RAW(FUNCTION)
// [10] BSLS_ASSERTTEST_ASSERT_FAIL_RAW(FUNCTION)
// [10] BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(FUNCTION)
// [10] BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(FUNCTION)
//
// [11] BSLS_ASSERTTEST_CHECK_LEVEL
// [11] BSLS_ASSERTTEST_CHECK_LEVEL_ARG
//
// [ 5] BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG
// [ 5] BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
// [ 5] BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
// [ 5] BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
// [ 5] BSLS_ASSERTTEST_IS_ACTIVE(TYPE)
// [ 3] AssertTest::isValidAssertBuild(const char *);
// [ 4] AssertTest::isValidExpected(char);
// [ 4] AssertTest::isValidExpectedLevel(char);
// [ 7] AssertTest::tryProbe(char, char);
// [ 7] AssertTest::tryProbeRaw(char, char);
// [ 8] AssertTest::catchProbe(char, ...);
// [ 8] AssertTest::catchProbeRaw(char, ...);
// [ 6] AssertTest::failTestDriver(const AssertViolation &);
// [ 6] AssertTest::failTestDriverByReview(const ReviewViolation &);
// [12] Testing try-probes that write diagnostics to the console
// [13] class AssertTestHandlerGuard
// [13] AssertTestHandlerGuard::AssertTestHandlerGuard()
// [13] ~AssertTestHandlerGuard::AssertTestHandlerGuard()
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] STDIO MONITORING TEST
// [14] Test case 1: vector
// [14] Test case 2: 'PASS' macros
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                    GLOBAL CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
    // To enable free functions to follow BDE test driver verbosity guidelines
    // without too much noise of extra arguments.

//=============================================================================
//                  GLOBAL HELPER MACROS FOR TESTING
//-----------------------------------------------------------------------------

//  #define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
//  #define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
//  #define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
//  #define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)
//
// These standard aliases will be defined later as part of the usage example.


//=============================================================================
//                            HELPER FUNCTIONS
//-----------------------------------------------------------------------------

// These are named functions mostly to support better human readability of the
// test code.

static
bool isOneOf(char c, const char *charset)
    // Return 'true' if the specified 'c' is an element in the specified
    // null-terminated 'charset' excluding the null terminator, and 'false'
    // otherwise.
{
    return c != 0 && strchr(charset, c) != 0;
}

//=============================================================================
//                    STDIO CAPTURE AND CONTROL MACHINERY
//-----------------------------------------------------------------------------

// IMPLEMENTATION NOTES and implementation details of the local 'stdio' capture
// mechanisms are placed after 'main' to reduce distractions from the actual
// test cases.

                          // ======================
                          // class StdIoTestMonitor
                          // ======================

class StdIoTestMonitor {
    // This non-copyable mechanism optionally monitors (captures and silences)
    // a given 'stdio' output stream.
    //
    // An 'StdIoTestMonitor' has 2 major capabilities.  One is capturing to
    // determine the presence of output, the other is silencing (redirecting to
    // a null device).  Both modes are prepared during construction.
    // Monitoring/silencing can then be started and stopped with the
    // correspondingly named functions.
    //
    // SILENCING
    // - - - - -
    // Silencing is specified using a test-driver-convenient 'bool verbose'
    // parameter.  When verbosity is set (to 'true') the monitor will not
    // reopen the monitored stream onto a null device, so anything printed to
    // the stream will go to its original destination.  Silencing is provided
    // so as to ensure the rule of "silent test drivers" where a test driver
    // must not print anything to standard output or error unless a raised
    // verbosity level have been specified on its command line, modulo the test
    // case number printing and the message about missing test case.
    //
    // CAPTURING
    // - - - - -
    // Capturing provides the basis for monitoring an 'stdio' output stream.
    // We "capture" the output stream by giving it our own stream buffer and
    // letting it to write to that buffer.  Note that the buffer is specified
    // externally so that the writer of the test driver can determine the
    // necessary size for testing.  The buffer is filled with a "nul" character
    // upon construction and afterwards with calls to 'resetBuffer'.  (The
    // "null character" is configurable, and it is the 0 value by default,
    // which should suffice for any normal use case.)

  private:
    // PRIVATE TYPES
    class MallocedBuffer {
      private:
        // DATA
        char *d_ptr;
        int   d_size;
        char  d_nulChar;  // A character representing no output

      public:
        // CREATORS
        explicit MallocedBuffer(int size, char nulChar = '\0');
            // Create a 'MallocedBuffer' object of the specified 'size'.
            // Optionally specify 'nulChar' that is used to indicate unwritten
            // areas of the buffer.  When 'nulChar' is not specified use zero.

        ~MallocedBuffer();
            // Release all resources owned by it, and then destroy this object.

        // MANIPULATORS
        int prepareBufferForHumans(int maxSize = -1);
            // Prepare the optionally specified 'maxSize' bytes, or all of the
            // bytes of the buffer for presenting it to humans by printing it
            // by replacing characters that may not be presentable on a screen
            // like control characters and characters with their top bit set
            // (not 7 bit ASCII).  Return the number of characters that have
            // been prepared for output.

        void reset();
            // Fill the buffer of this object with the 'nulChar'.

        // ACCESSORS
        char nulChar() const;
            // Retrun the 'nulChar' used by this object.

        char *ptr() const;
            // Return a non-const pointer to the first character of the buffer
            // of this object.

        int size() const;
            // Return the size of the buffer of this object.

        bool wasThereAnyOutput() const;
            // Return 'true' if any of the characters stored in the entirety
            // of the capture buffer is unequal to the "null character".
            // Otherwise, if all characters in the buffer are the
            // "null character" return 'false'.  The behavior is undefined
            // unless 'd_ptr != 0'.
    };

    // DATA
    bool            d_active;          // we are capturing and/or redirecting
    FILE           *d_monitoredStream; // we capture/redirect this
    MallocedBuffer  d_buffer;          // the stream buffer or null
    FILE           *d_nullStream;      // we redirect to this, or it is null
    int             d_origFD;          // POSIX file number of monitored stream
    int             d_dupOrigFD;       // the saved the original open file #

  private:
    // NOT IMPLEMENTED
    StdIoTestMonitor(const StdIoTestMonitor&);
    StdIoTestMonitor& operator=(const StdIoTestMonitor&);

    // PRIVATE MANIPULATORS
    void init(FILE *stream, bool  verbose);
        // Initialize this object to monitor the specified output 'stream', and
        // silence it as well unless the specified 'verbose' is 'true'.

    void perrorAbort(const char *msg);
        // Print the specified 'msg' to 'stderr' with standard 'perror', then
        // abort the program.

    void safeSetBuf();
        // Set the stream buffer of the monitored stream safely, regardless if
        // it is null or not, to this object's 'd_buffer_p' with 'd_bufSize'
        // size.

    void setBuf();
        // Set the stream buffer of the monitored stream to this object's
        // 'd_buffer_p' with 'd_bufSize' size.  The behavior is undefined if
        // 'd_buffer_p' is null.

    void stopImpl();
        // The guts of stopping shared by 'stop' and the destructor.

    // PRIVATE ACCESSORS
    bool doesCapturing() const;
        // Return 'true' if this monitor will capture characters into a buffer
        // when monitoring is started.  Otherwise, return 'false' if capturing
        // is not enabled by the presence of a buffer.

    bool doesSilencing() const;
        // Return 'true' if this monitor does silence its monitored stream when
        // 'start'ed.  Otherwise, if this monitor does not silence its
        // monitored stream upon calling 'start' return 'false'.

    bool isNoop() const;
        // Return 'true' if this monitor will not capture or silence when
        // 'start'ed.  Otherwise, return 'false' if either capturing or
        // silencing is possible with this object.

    bool isVerbose() const;
        // Return 'true' if this monitor does not silence its monitored stream.
        // Otherwise, if this monitor silences its monitored stream upon
        // calling 'start', return 'false'.

  public:
    // CREATORS
    StdIoTestMonitor(FILE *stream,
                     int   bufSize,
                     bool  verbose,
                     char  nulChar = 0);
        // Initialize this object to monitor the specified output 'stream',
        // using a buffer of the specified 'bufSize' for capturing output.  If
        // the specified 'verbose' is 'false' also prepare to redirect the
        // stream to a null device so we can run a silent test driver.
        // Optionally specify 'nulChar' for the unlikely case that the output
        // may contain '\0' characters, in which case the value of 'nulChar'
        // will be used (instead of zero) to fill the buffer (before detection)
        // by 'resetBuffer'.  The behavior is undefined unless 'stream' is a an
        // open output stream.  The behavior is also undefined unless this is
        // the only monitor instantiated on the 'stream'.

    ~StdIoTestMonitor();
        // Stop both capturing and redirecting (if any was ongoing), close all
        // files and streams opened by this object (if any) then destroy this
        // object.

    // MANIPULATORS
    char *buffer();
        // Return a non-const pointer to the capture buffer.  If zero buffer
        // size was used to initialize this object the capture buffer pointer
        // will be null.

    int prepareBufferForHumans(int maxSize = -1);
        // Prepare the optionally specified 'maxSize' bytes, or all of the
        // bytes of the buffer for presenting it to humans by printing it by
        // replacing characters that may not be presentable on a screen like
        // control characters and characters with their top bit set (not 7 bit
        // ASCII).  Return the number of characters that have been prepared for
        // output.

    void resetBuffer();
        // If capturing is enabled (buffer is present) flush the monitored
        // stream, detach the buffer, fill the buffer with the
        // "null character", then reattach the buffer.  If capturing is
        // inactive do nothing.

    void start();
        // Start monitoring the stream if capturing or silencing is requested.
        // The behavior is undefined if monitoring is already active.

    void stop();
        // Stop monitoring.  The behavior is undefined if monitoring is not
        // active.

    // ACCESSORS
                            // Buffer Attributes
    const char *buffer() const;
        // Returns a const pointer to the buffer.  If zero buffer size was used
        // to initialize this object the capture buffer pointer will be null.

    int bufferSize() const;
        // Returns the size of the capture buffer.

    char nulChar() const;
        // Returns the "null character" that is used to pre-fill the buffer.

                           // Status Information

    bool isActive() const;
        // Return 'true' if this monitor is actively enabling

                    // monitor Characters' Information

    bool wasThereAnyOutput() const;
        // Return 'true' if any of the characters stored in the entirety of
        // this buffer is unequal to the "null character".  Otherwise, if all
        // characters in the buffer are the "null character" return 'false'.
};

                      // =========================
                      // class StdIoTestMonitorDuo
                      // =========================

class StdIoTestMonitorDuo {
    // This non-copyable mechanism is two 'StdIoTestMonitor' objects monitoring
    // 'stdout' and 'stderr'.  Please see 'StdIoTestMonitor' for the concept,
    // capabilities, limitations, and caveats.

  private:
    // DATA
    mutable StdIoTestMonitor d_stdoutMonitor;
    mutable StdIoTestMonitor d_stderrMonitor;

  private:
    // NOT IMPLEMENTED
    StdIoTestMonitorDuo(const StdIoTestMonitorDuo&);
    StdIoTestMonitorDuo&
        operator=(const StdIoTestMonitorDuo&);

  public:
    // CREATORS
    StdIoTestMonitorDuo(bool stdoutVerbose,
                        bool stderrVerbose);
        // Create an 'StdIoTestMonitorDuo' object that with both 'stdout', and
        // 'stderr' monitors initialized with 128 for buffer size, and the use
        // the specified 'stdoutVerbose', and 'stderrVerbose' for verbosity,
        // respectively.

    explicit StdIoTestMonitorDuo(bool verbosity);
        // Create an 'StdIoTestMonitorDuo' object that with both 'stdout', and
        // 'stderr' monitors initialized with 128 for buffer size, and the
        // specified 'verbosity' for both streams.

    // MANIPULATORS
                           // Monitoring Control
    void start();
        // Start both monitors.  The behavior is undefined unless both of the
        // monitors are inactive.

    void startStderr();
        // Start monitoring on 'stderr' only.  The behavior is undefined unless
        // both the 'stderr' monitor is inactive.

    void startStdout();
        // Start monitoring on 'stdout' only.  The behavior is undefined unless
        // both the 'stdout' monitor is inactive.

    void stop();
        // Stop both monitors.  Note that stopping an inactive monitor does
        // nothing.

    void stopStderr();
        // Stop monitoring 'stderr'.  Note that stopping an inactive monitor
        // does nothing.

    void stopStdout();
        // Stop monitoring 'stdout'.  Note that stopping an inactive monitor
        // does nothing.

                          // Buffer Control

    int prepareStderrForHumans(int maxSize = -1);
        // Replace unprintable and control characters (assuming ASCII) at the
        // beginning of the 'stderr' capture buffer up to the optionally
        // specified 'maxSize' bytes, or all of the bytes of the buffer.

    int prepareStdoutForHumans(int maxSize = -1);
        // Replace unprintable and control characters (assuming ASCII) at the
        // beginning of the 'stdout' capture buffer up to the optionally
        // specified 'maxSize' bytes, or all of the bytes of the buffer.

    void resetBuffers();
        // Reset both capture buffers.  See 'StdIoTestMonitor::resetBuffer' for
        // specifics.

    char *stderrBuffer();
        // Return a pointer to the non-const buffer provided during
        // construction for 'stderr', or a null pointer if zero buffer size was
        // requested at construction for 'stderr' capture buffer.

    char *stdoutBuffer();
        // Return a pointer to the non-const buffer provided during
        // construction for 'stdout', or a null pointer if zero buffer size was
        // requested at construction for 'stdout' capture buffer.

    // ACCESSORS
    bool isStderrActive() const;
        // Return 'true' if the 'stderr' monitor is active (9)have been
        // 'start'ed but not 'stop'ped).  Otherwise, return 'false' if the
        // 'stderr' monitor has not been 'start'ed or has since been 'stop'ped.

    bool isStdoutActive() const;
        // Return 'true' if the 'stdout' monitor is active (9)have been
        // 'start'ed but not 'stop'ped).  Otherwise, return 'false' if the
        // 'stdout' monitor has not been 'start'ed or has since been 'stop'ped.

    const char *monitorMessage(bool expectedOutputOnStdout,
                               bool expectedOutputOnStderr) const;
        // Return either a null pointer or a pointer to a static error message
        // based on the specified 'expectedOutputOnStdout', and
        // 'expectedOutputOnStderr' flags as well as the contents of the
        // buffers of the two corresponding monitors.  Null pointer is returned
        // if the presence of output on both streams matches the expectations
        // described by the argument "expected" arguments, or if those that do
        // not match expectations are ignored.  Otherwise, a pointer to a human
        // readable, English, but unspecified static error description is
        // returned.

    const char *stderrBuffer() const;
        // Return a pointer to const to the const buffer provided during
        // construction for 'stderr', or a null pointer if zero buffer size was
        // requested at construction for 'stderr' capture buffer.

    int stderrBufferSize() const;
        // Return the buffer size provided during construction for 'stderr'.
        // Note that if zero is returned 'stdoutBuffer()' will return a null
        // pointer regardless what buffer pointer was supplied at construction.

    const char *stdoutBuffer() const;
        // Return a pointer to const to the const buffer provided during
        // construction for 'stdout', or a null pointer if zero buffer size was
        // requested at construction for 'stdout' capture buffer.

    int stdoutBufferSize() const;
        // Return the buffer size provided during construction for 'stdout'.
        // Note that if zero is returned 'stdoutBuffer()' will return a null
        // pointer regardless what buffer pointer was supplied at construction.

    bool wasThereStderrOutput() const;
        // Return 'true' if the 'stderr' monitor has output-looking characters
        // in their buffers.  Otherwise, if all characters in the 'stderr'
        // capture buffers are the "null character", return 'false',

    bool wasThereStdoutOutput() const;
        // Return 'true' if the 'stdout' monitor has output-looking characters
        // in their buffers.  Otherwise, if all characters in the 'stdout'
        // capture buffers are the "null character", return 'false',
};

                        // =========================
                        // class MonitorExpectations
                        // =========================

class MonitorExpectations {
    // Little helper unconstrained attribute-class to make setting expectations
    // for the monitor implementations used for testing 'bsls_asserttest'.

  private:
    // DATA
    bool d_stdoutPrints;  // Do we expect 'stdout' to print?
    bool d_stderrPrints;  // Do we expect 'stderr' to print?

  public:
    // CREATORS
    MonitorExpectations(bool stdoutPrints, bool stderrPrints)
        // Create an object with 'stdout' and 'stderr' output expectations set
        // to 'e_EXPECT_OUTPUT' if the specified 'stdoutPrints', and
        // 'stderrPrints' value is 'true', while they are set to
        // 'e_EXPECT_SILENCE' if their corresponding value is false.
    : d_stdoutPrints(stdoutPrints)
    , d_stderrPrints(stderrPrints)
    {}

    MonitorExpectations()
        // Create an object with default expectations: eerie silence (on both
        // 'stdout', and 'stderr').
    : d_stdoutPrints(false), d_stderrPrints(false)
    {}

    // MANIPLUATORS
    void reset()
        // Set expectations to the default state: all silent.
    {
        d_stdoutPrints = d_stderrPrints = false;
    }
                         // Definite Output Expectations

    void select(bool isStderr)
        // Set output expectations exclusively to 'stderr' if the specified
        // 'isStderr' is 'true', or to 'stdout' if 'isStderr' is 'false'.
    {
        setOut(!isStderr);
        setErr(isStderr);
    }

    void setErr(bool value = true)
        // Change output expectations of 'stderr' according to the optionally
        // specified 'value'.
    {
        d_stderrPrints = value;
    }

    void setOut(bool value = true)
        // Change output expectations of 'stdout' according to the optionally
        // specified 'value'.
    {
        d_stdoutPrints = value;
    }

    void unlessStdoutStderrIf(bool noOut, bool isStderr)
        // If the specified 'noOut' is 'true' set expectations to silence.
        // Otherwise, if 'noOut' is 'false', always except output on 'stdout'
        // and expect output on 'stdErr' only if the specified 'isStderr' is
        // 'true'.
    {
        setOut(!noOut);
        setErr(isStderr && !noOut);
    }

    // ACCESSORS
    bool stderrPrints() const
        // Return 'true' if 'stderr' is expected to print, 'false' if it is
        // expected to be silent.
    {
        return d_stderrPrints;
    }

    bool stdoutPrints() const
        // Return 'true' if 'stdout' is expected to print, 'false' if it is
        // expected to be silent.
    {
        return d_stdoutPrints;
    }
};

                        // ==========================
                        // class StdioAssertDebugInfo
                        // ==========================

class StdioAssertDebugInfo {
    // This class mainly exists just to be able to gather and pretty print
    // location (and possibly later loop) information.

  private:
    // DATA
    int d_callLine;
    int d_dataLine;
        // Zero means no line info, as C and C++ numbers lines from 1.  If the
        // 'callLine' attribute is not set we consider the object empty.

  public:
    // CREATORS
    StdioAssertDebugInfo()
    : d_callLine(0), d_dataLine(0)
        // Create an empty 'StdioAssertDebugInfo' object.
    {}

    explicit StdioAssertDebugInfo(int assertLine)
    : d_callLine(assertLine), d_dataLine(0)
        // Create an 'StdioAssertDebugInfo' object using the specified
        // 'assertLine', and zero for 'dataLine'.
    {
    }

    StdioAssertDebugInfo(int assertLine, int dataLine)
    : d_callLine(assertLine), d_dataLine(dataLine)
        // Create an 'StdioAssertDebugInfo' object using the specified
        // 'assertLine', and 'dataLine'.
    {
    }

    // MANIPULATORS
    void reset()
        // Set this object to the empty state.
    {
        d_callLine = d_dataLine = 0;
    }

    // ACCESSORS
    int callLine() const
        // Return the 'callLine' attribute of this object.
    {
        return d_callLine;
    }

    int dataLine() const
        // Return the 'dataLine' attribute of this object.
    {
        return d_dataLine;
    }

    bool empty() const
        // Return 'true' if this object is in the empty state, or 'false' if
        // at least the 'callLine' attribute is not zero.
    {
        return (d_callLine == 0);
    }

    bool isSet() const
        // Return '!this->empty()'.
    {
        return (d_callLine != 0);
    }

    void print() const
        // Print the line information stored in the specified 'obj' to help
        // identifying the cause of test driver 'stdio' assertion failures.
    {
        if (this->empty()) {
            fputs("**NO LINE INFORMATION**", stdout);
            return;                                                   // RETURN
        }

        printf("CALL LINE#: %d", d_callLine);
        if (d_dataLine > 0) {
            printf(", DATA LINE#: %d", d_dataLine);
        }
    }
};

void debugprint(const StdioAssertDebugInfo &object)
    // Customization point for 'bsls::BslTestUtil': print the specified
    // 'object' to the standard output stream.
{
    object.print();
}

                        // =======================
                        // class AssertTestMonitor
                        // =======================

class AssertTestMonitor {
    // A mechanism to use with verifying 'stdio' output of 'BSLS_ASSERTTEST_*'
    // macro invocations.

  private:
    // PROTECTED DATA
    mutable StdIoTestMonitorDuo d_monitors;
    MonitorExpectations         d_expectations;
    StdioAssertDebugInfo        d_debugInfoForAssert;

  private:
    // NOT IMPLEMENTED
    AssertTestMonitor(AssertTestMonitor&);
    AssertTestMonitor& operator=(const AssertTestMonitor&);

  private:
    // PRIVATE MANIPULATORS
    void printIfErrorAndAssert();
        // Verify if expectations were met, if not, print out the offending
        // buffer and 'ASSERT' (as in test driver assertion).

  public:
    // CREATORS
    explicit AssertTestMonitor(bool verbosity);
        // Create an assert-test monitor that silences 'stdout' and 'stderr',
        // or not according to the specified 'verbosity'.

    // MANIPULATORS
    void start();
        // Start monitoring 'stdout' and 'stderr'.

    void stop();
        // Stop monitoring 'stdout' and 'stderr', verify expectations, and
        // assert (with additional information) if the expectations were not
        // met.

                    // Setting Definite Output Expectations

    AssertTestMonitor &l(int assertLine)
        // Set debug info to indicate test run on the specified 'assertLine'.
    {
        d_debugInfoForAssert = StdioAssertDebugInfo(assertLine);
        return *this;
    }

    AssertTestMonitor& ld(int assertLine, int dataLine)
        // Set debug info to indicate test run on the specified 'assertLine',
        // using data from the specified 'dataLine'.
    {
        d_debugInfoForAssert = StdioAssertDebugInfo(assertLine, dataLine);
        return *this;
    }

    AssertTestMonitor& select(bool isErr)
        // Set mutually exclusive output expectations for 'stderr' or 'stdout',
        // selecting between them based on the specified 'isErr' value.
    {
        d_expectations.select(isErr);
        return *this;
    }

    AssertTestMonitor& setBoth(bool prints = true)
        // Set output expectations for 'stdout' and 'stderr' both.  Optionally
        // specify 'prints' that is used to set output expectations.  If
        // 'print' is not specified set output expectations to 'true'.
    {
        d_expectations.setOut(prints);
        d_expectations.setErr(prints);
        return *this;
    }

    AssertTestMonitor& setErr(bool prints = true)
        // Set output expectations for 'stderr'.  Optionally specify 'prints'
        // that is used to set output expectations.  If 'print' is not
        // specified set output expectations to 'true'.
    {
        d_expectations.setErr(prints);
        return *this;
    }

    AssertTestMonitor& setOut(bool prints = true)
        // Set output expectations for 'stdout'.  Optionally specify 'prints'
        // that is used to set output expectations.  If 'print' is not
        // specified set output expectations to 'true'.
    {
        d_expectations.setOut(prints);
        return *this;
    }

    AssertTestMonitor& unlessStdoutStderrIf(bool noOut, bool isErr)
        // If the specified 'noOut' is 'true' set expectations to silence.
        // Otherwise, if 'noOut' is 'false', always expect output on 'stdout'
        // and expect output on 'stdErr' only if the specified 'isErr' is
        // 'true'.
    {
        d_expectations.unlessStdoutStderrIf(noOut, isErr);
        return *this;
    }

    // ACCESSORS
    bool wouldBeExtraOutput() const;
        // Return 'true' if this monitor would report unexpected output on
        // either 'stdout' or 'stderr' if it were stopped now.  Essentially
        // 'return wouldBeExtraStdout() || wouldBeExtraStderr()'.

    bool wouldBeExtraStderr() const;
        // Return 'true' if this monitor would report unexpected output on
        // 'stderr' if it were stopped now.

    bool wouldBeExtraStdout() const;
        // Return 'true' if this monitor would report unexpected output on
        // 'stdout' if it were stopped now.

    bool wouldFail() const;
        // Return 'true' if this monitor would report failure in case it would
        // be stopped now.  Essentially
        // 'return wouldBeExtraOutput() || wouldOutputBeMissing();'.

    bool wouldOutputBeMissing() const;
        // Return 'true' if this monitor would report failure of output to
        // appear on either 'stdout' or 'stderr' if it were stopped now.
        // Essentially
        // 'return wouldStdoutBeMissing() || wouldStderrBeMissing();'.

    bool wouldStderrBeMissing() const;
        // Return 'true' if this monitor would report failure of output to
        // appear on 'stderr' if it were stopped now.

    bool wouldStdoutBeMissing() const;
        // Return 'true' if this monitor would report failure of output to
        // appear on 'stdout' if it were stopped now.
};

                         // =======================
                         // class AssertTestProctor
                         // =======================

class AssertTestProctor  {
    // A proctor used to automatically stop an 'stdio' monitor (in case of an
    // escaped exception or just leaving a scope), as well as allowing
    // dedicated early stopping (and checked, as 'stop' does that, too), should
    // we need to allow the "calling" assertion macro to print.
  private:
    // DATA
    AssertTestMonitor *d_monitor_p;

  private:
    // NOT IMPLEMENTED
    AssertTestProctor(AssertTestProctor&);
    AssertTestProctor& operator=(const AssertTestProctor&);

  public:
    // CREATORS
    explicit AssertTestProctor(AssertTestMonitor *monitor)
        // Create a guard that stops the specified 'monitor' upon its
        // destruction.
    : d_monitor_p(monitor)
    {
    }

    ~AssertTestProctor()
        // Stop the guarded 'monitor' if it hasn't been dismissed before, then
        // destroy this object.
    {
        if (d_monitor_p) d_monitor_p->stop();
    }

    // MANIPULATORS
    void verifyAndDismiss()
        // Used in 'ASSERT_WITH' macros this enables early verification and
        // stopping of monitoring allowing the outer 'ASSERT_WITH' macros to
        // use 'stdio' for printing their own errors.
    {
        d_monitor_p->stop();
        d_monitor_p = 0;
    }

    // ACCESSORS
    AssertTestMonitor *operator->() const
        // Return a non-const pointer to the guarded 'monitor'.
    {
        return d_monitor_p;
    }
};

//=============================================================================
//                    STDIO BEHAVIOR VERIFICATION MACROS
//-----------------------------------------------------------------------------

#define PRE_STDIO_MONGUARD                                                    \
    do { AssertTestProctor stdioProctor(&monitor);                            \
         stdioProctor->ld(__LINE__ + 1, LINE);
    // To avoid repetition in the macros below.

#define EXPECT_SILENCE PRE_STDIO_MONGUARD  stdioProctor->start()

#define EXPECT_OUTPUT_ON_STDOUT                                               \
    PRE_STDIO_MONGUARD stdioProctor->setOut(); stdioProctor->start()
    // Expect output on 'stdout', expect silence on 'stderr' and start
    // monitoring 'stdio'.

#if defined(BDE_BUILD_TARGET_EXC)
#define EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC EXPECT_OUTPUT_ON_STDOUT
#else
#define EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC EXPECT_SILENCE
#endif

#define EXPECT_OUTPUT_ON_STDERR                                               \
    PRE_STDIO_MONGUARD stdioProctor->setErr(); stdioProctor->start()
    // Expect output on 'stderr', expect silence on 'stdout' and start
    // monitoring 'stdio'.

#define EXPECT_OUTPUT_ON_STDOUT_IF(prints)                                    \
    PRE_STDIO_MONGUARD stdioProctor->setOut(prints); stdioProctor->start()
    // Conditionally expect output on 'stdout' as determined by the specified
    // 'prints' argument, expect silence on 'stderr' and start monitoring
    // 'stdio'.

#define EXPECT_OUTPUT_ON_STDERR_IF(prints)                                    \
    PRE_STDIO_MONGUARD stdioProctor->setErr(prints); stdioProctor->start()
    // Conditionally expect output on 'stderr' as determined by the specified
    // 'prints' argument, expect silence on 'stdout' and start monitoring
    // 'stdio'.

#define EXPECT_OUTPUT_ON_STDOUT_STDERR_IF(prints)                             \
    PRE_STDIO_MONGUARD stdioProctor->setOut().setErr(prints);                 \
    stdioProctor->start()
    // Always expect output on 'stdout', and conditionally expect output on
    // 'stderr' as determined by the specified 'prints' argument and start
    // monitoring 'stdio'.

#define EXPECT_OUTPUT_ON_BOTH                                                 \
    PRE_STDIO_MONGUARD stdioProctor->setOut().setErr(); stdioProctor->start()
    // Expect output on both 'stdout', and on 'stderr' and start monitoring
    // 'stdio'.

#define EXPECT_OUTPUT_ON_BOTH_IF(isBoth)                                      \
    PRE_STDIO_MONGUARD stdioProctor->setBoth(isBoth); stdioProctor->start()
    // Expect output on both 'stdout', and on 'stderr' IFF 'isBoth' is defined
    // and start monitoring 'stdio'.

#define EXPECT_OUTPUT_ON_SELECTED(isStderr)                                   \
    PRE_STDIO_MONGUARD stdioProctor->select(isStderr); stdioProctor->start()
    // Exclusively (either/or) expect output on one of the streams *'stdout' or
    // 'stderr') as determined by the specified 'isErr' (choses 'stderr' if
    // 'true', 'stdout' when 'false') and start monitoring 'stdio'.

#define EXPECT_OUTPUT_UNLESS_STDOUT_STDERR_IF(noOut, isStderr)                \
    PRE_STDIO_MONGUARD stdioProctor->unlessStdoutStderrIf(noOut, isStderr);   \
    stdioProctor->start()
    // Exclusively (either/or) expect output on one of the streams ('stdout' or
    // 'stderr') as determined by the specified 'isErr' (choses 'stderr' if
    // 'true', 'stdout' when 'false') *but* expect no output on 'stdout' (when
    // selected) when the specified 'butOut' is 'true'  and start monitoring
    // 'stdio'.  Notice this means that when 'butOut' is 'false' this macro
    // works as 'EXPECT_SELECTED' does.

#define ASSERT_STDIO } while (false)
    // Stop monitoring 'stdio', verify output expectations and assert if any
    // has failed.  Place this macro *right after* the tested code line(s)!

                      // 'ASSERT_WITH_STDIO' macros

// Set up expectations as usual (with the macros above) then use one of the
// macros below (that are not imp detail) instead of the simple 'ASSERT_STDIO'
// to do both "your other" assert and the verification of 'stdio' output.

#define ASSERT_WITH_STDIO(X)                                                  \
    const bool aSsErTcOnDiTiOn = !(X);                                        \
    stdioProctor.verifyAndDismiss();                                          \
    aSsErT(aSsErTcOnDiTiOn, #X, __LINE__); } while (false)

#define LOOP0_ASSERT_WITH_STDIO ASSERT_WITH_STDIO

#define LOOP_ASSERT_WITH_STDIO(I,X)                                           \
    const bool aSsErTcOnDiTiOn = !(X);                                        \
    stdioProctor.verifyAndDismiss();                                          \
    if (aSsErTcOnDiTiOn) {                                                    \
        BloombergLP::bsls::BslTestUtil::callDebugprint(I, #I ": ", "\n");     \
        aSsErT(aSsErTcOnDiTiOn, #X, __LINE__); } } while (false)

#define LOOP1_ASSERT_WITH_STDIO LOOP_ASSERT_WITH_STDIO

#define LOOP2_ASSERT_WITH_STDIO(I,J,X)                                        \
    const bool aSsErTcOnDiTiOn = !(X);                                        \
    stdioProctor.verifyAndDismiss();                                          \
    if (aSsErTcOnDiTiOn) {                                                    \
        BloombergLP::bsls::BslTestUtil::callDebugprint(I, #I ": ", "\n");     \
        BloombergLP::bsls::BslTestUtil::callDebugprint(J, #J ": ", "\n");     \
        aSsErT(aSsErTcOnDiTiOn, #X, __LINE__); } } while (false)

#define LOOP3_ASSERT_WITH_STDIO(I,J,K,X)                                      \
    const bool aSsErTcOnDiTiOn = !(X);                                        \
    stdioProctor.verifyAndDismiss();                                          \
    if (aSsErTcOnDiTiOn) {                                                    \
        BloombergLP::bsls::BslTestUtil::callDebugprint(I, #I ": ", "\n");     \
        BloombergLP::bsls::BslTestUtil::callDebugprint(J, #J ": ", "\n");     \
        BloombergLP::bsls::BslTestUtil::callDebugprint(K, #K ": ", "\n");     \
        aSsErT(aSsErTcOnDiTiOn, #X, __LINE__); } } while (false)

#define LOOP4_ASSERT_WITH_STDIO(I,J,K,L,X)                                    \
    const bool aSsErTcOnDiTiOn = !(X);                                        \
    stdioProctor.verifyAndDismiss();                                          \
    if (aSsErTcOnDiTiOn) {                                                    \
        BloombergLP::bsls::BslTestUtil::callDebugprint(I, #I ": ", "\n");     \
        BloombergLP::bsls::BslTestUtil::callDebugprint(J, #J ": ", "\n");     \
        BloombergLP::bsls::BslTestUtil::callDebugprint(K, #K ": ", "\n");     \
        BloombergLP::bsls::BslTestUtil::callDebugprint(L, #L ": ", "\n");     \
        aSsErT(aSsErTcOnDiTiOn, #X, __LINE__); } } while (false)

#define LOOP5_ASSERT_WITH_STDIO(I,J,K,L,M,X)                                  \
    const bool aSsErTcOnDiTiOn = !(X);                                        \
    stdioProctor.verifyAndDismiss();                                          \
    if (aSsErTcOnDiTiOn) {                                                    \
        BloombergLP::bsls::BslTestUtil::callDebugprint(I, #I ": ", "\n");     \
        BloombergLP::bsls::BslTestUtil::callDebugprint(J, #J ": ", "\n");     \
        BloombergLP::bsls::BslTestUtil::callDebugprint(K, #K ": ", "\n");     \
        BloombergLP::bsls::BslTestUtil::callDebugprint(L, #L ": ", "\n");     \
        BloombergLP::bsls::BslTestUtil::callDebugprint(M, #M ": ", "\n");     \
        aSsErT(aSsErTcOnDiTiOn, #X, __LINE__); } } while (false)

#define LOOP6_ASSERT_WITH_STDIO(I,J,K,L,M,N,X)                                \
    const bool aSsErTcOnDiTiOn = !(X);                                        \
    stdioProctor.verifyAndDismiss();                                          \
    if (aSsErTcOnDiTiOn) {                                                    \
        BloombergLP::bsls::BslTestUtil::callDebugprint(I, #I ": ", "\n");     \
        BloombergLP::bsls::BslTestUtil::callDebugprint(J, #J ": ", "\n");     \
        BloombergLP::bsls::BslTestUtil::callDebugprint(K, #K ": ", "\n");     \
        BloombergLP::bsls::BslTestUtil::callDebugprint(L, #L ": ", "\n");     \
        BloombergLP::bsls::BslTestUtil::callDebugprint(M, #M ": ", "\n");     \
        BloombergLP::bsls::BslTestUtil::callDebugprint(N, #N ": ", "\n");     \
        aSsErT(aSsErTcOnDiTiOn, #X, __LINE__); } } while (false)

// These 3 macros we hope will keep coming from '<bsls_bsltestutil.h>'
//:#define BSLS_BSLTESTUTIL_EXPAND(X) X
//:
//:#define BSLS_BSLTESTUTIL_NUM_ARGS_IMPL(X6, X5, X4, X3, X2, X1, X0, N, ...) N
//:
//:#define BSLS_BSLTESTUTIL_NUM_ARGS(...)                                    \+
//:    BSLS_BSLTESTUTIL_EXPAND(BSLS_BSLTESTUTIL_NUM_ARGS_IMPL(               \+
//:                                      __VA_ARGS__, 6, 5, 4, 3, 2, 1, 0, ""))

#define LOOPN_ASSERT_WITH_STDIO_IMPL(N, ...)                                  \
    BSLS_BSLTESTUTIL_EXPAND(LOOP ## N ## _ASSERT_WITH_STDIO(__VA_ARGS__))

#define LOOPN_ASSERT_WITH_STDIO(N, ...)                                       \
    LOOPN_ASSERT_WITH_STDIO_IMPL(N, __VA_ARGS__)

#define ASSERTV_WITH_STDIO(...)                                               \
    LOOPN_ASSERT_WITH_STDIO(BSLS_BSLTESTUTIL_NUM_ARGS(__VA_ARGS__),           \
                            __VA_ARGS__)

              // 'LINE' variable to pick up when we have none

static const int LINE = -1;
    // This is a hack so we do not need to write 2 sets of macros.  Test cases
    // that do not have 'LINE' defined for the source of test data will pick
    // this up and due to its negative value the test machinery knows the value
    // is "not present".

//=============================================================================
//             TEST MACHINERY FOR STDIO CAPTURE TESTING (POPEN)
//-----------------------------------------------------------------------------

int popeneer(const char *commandLine, char *buffer, int size);
    // Start up a process with 'popen' using the specified 'commandLine', read
    // its 'stdout' into the specified 'buffer' of the specified 'size', then
    // return the actual number of characters read.  See implementation after
    // the 'main' function.

template <int BSIZE>
int popeneer(const char *commandLine, char (&buffer)[BSIZE])
    // Call 'popeneer' with the specified 'commandLine', 'buffer', and 'BSIZE'.
{
    return popeneer(commandLine, buffer, BSIZE);
}

//=============================================================================
//                    PRETTY PRINTERS FOR ASSERT AND 'P'
//-----------------------------------------------------------------------------

class PrettyChar {
    // Wrapper for printing non-printable characters as hexadecimal numbers.

    // DATA
    char d_char;

  public:
    // CREATORS
    explicit PrettyChar(char c)
        // Create a 'PrettyChar' wrapper for the specified character 'c'.
    : d_char(c)
    {
    }

    // ACCESSORS
    char theChar() const
        // Return the character wrapped by this object.
    {
        return d_char;
    }
};

void debugprint(PrettyChar pch)
    // Pretty print the character stored in the specified 'pch' by printing it
    // in hexadecimal if its value is deemed not printable.
{
    const char c = pch.theChar();

    if (c < ' ' || (c & 0x80)) {
        printf("0x%02X", c & 0xFF);
    }
    else {
        printf("%c", c);
    }
}

//=============================================================================
//                 USAGE EXAMPLE EXTRACTED AS STAND-ALONE CODE
//-----------------------------------------------------------------------------

///Usage
///-----
//
///Example 1: Testing Assertions In A Simple Vector Implementation
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First we will demonstrate how "negative testing" might be used to verify
// that the correct assertions are in place on 'std::vector::operator[]'.  We
// start by supplying a primitive vector-like class that offers the minimal set
// of operations necessary to demonstrate the test case.
//..
template <class T>
class AssertTestVector {
    // This class simulates a 'std::vector' with a fixed capacity of 10
    // elements.

  private:
    // DATA
    T   d_data[10];
    int d_size;

  public:
    // CREATORS
    AssertTestVector();
        // Create an empty 'AssertTestVector' object.

    // MANIPULATORS
    void push_back(const T& value);
        // Append the specified 'value' to the back of this object.  The
        // behavior is undefined unless this method has been called fewer
        // than 10 times on this object.

    // ACCESSORS
    const T& operator[](int index) const;
        // Return a reference with non-modifiable access to the object at
        // the specified 'index' in this object.
};
//..
// Next we implement the support functions.
//..
template <class T>
AssertTestVector<T>::AssertTestVector()
: d_data()
, d_size()
{
}

template<class T>
void AssertTestVector<T>::push_back(const T& value)
{
    BSLS_ASSERT_SAFE(d_size < 10);

    d_data[d_size] = value;
    ++d_size;
}
//..
// We conclude the definition of this support type with the implementation of
// the 'operator[]' overload.  Note the use of 'BSLS_ASSERT_SAFE', which is
// typical for function template definitions and inline function definitions.
// It is most appropriate in this case as the cost of evaluating each test is
// significant (> ~20%) compared to simply returning a reference to the result.
//..
template <class T>
const T& AssertTestVector<T>::operator[](int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < d_size);

    return d_data[index];
}
//..
// Finally, we can write the function to test that the 'BSLS_ASSERT_SAFE'
// macros placed in 'operator[]' work as expected.  We want to validate that
// the assertions trigger when the function preconditions are violated; we
// further want to validate that the assertion macros are enabled in the build
// modes that we expect.  We start by defining some macro aliases that will
// make the test driver more readable.  These macro aliases are a common
// feature of test drivers.
//..
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)
//..
// Then we implement the test function itself.  Note that we check that
// exceptions are available in the current build mode, as the test macros rely
// on the exception facility in order to return their diagnostic results.  If
// exceptions are not available, there is nothing for a "negative test" to do.
//..
void testVectorArrayAccess()
{
#ifdef BDE_BUILD_TARGET_EXC
    bsls::AssertTestHandlerGuard g;

    AssertTestVector<void *> mA; const AssertTestVector<void *> &A = mA;

    ASSERT_SAFE_FAIL(mA[-1]);
    ASSERT_SAFE_FAIL(mA[ 0]);
    ASSERT_SAFE_FAIL(mA[ 1]);

    ASSERT_SAFE_FAIL( A[-1]);
    ASSERT_SAFE_FAIL( A[ 0]);
    ASSERT_SAFE_FAIL( A[ 1]);

    mA.push_back(0);  // increase the length to one

    ASSERT_SAFE_FAIL(mA[-1]);
    ASSERT_PASS     (mA[ 0]);
    ASSERT_SAFE_FAIL(mA[ 1]);

    ASSERT_SAFE_FAIL( A[-1]);
    ASSERT_PASS     ( A[ 0]);
    ASSERT_SAFE_FAIL( A[ 1]);
#else  // defined(BDE_BUILD_TARGET_EXC)
//..
// If exceptions are not available, then we write a diagnostic message to the
// console alerting the user that this part of the test has not run, without
// failing the test.
//..
    if (verbose) puts("\tDISABLED in this (non-exception) build mode.");
#endif  // !defined(BDE_BUILD_TARGET_EXC)
}
//..
//
///Example 2: Using 'PASS' macros to help with formatting
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// When testing the various inputs to a function to be sure that some trigger
// an assertion and some are in contract, it often helps to align the testing
// macros so that the various arguments are easily readable in relation to one
// another.  We start by defining additional macro aliases to match the
// existing aliases already defined:
//..
#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
//..
// Considering the function 'testVectorArrayAccess' from {Example 1}, we could
// instead implement it without padded white space by using 'ASSERT_SAFE_PASS'
// to replace 'ASSERT_PASS', matching the existing 'ASSERT_SAFE_FAIL' tests,
// like this:
//..
void testVectorArrayAccess2()
{
#ifdef BDE_BUILD_TARGET_EXC
    bsls::AssertTestHandlerGuard g;

    AssertTestVector<void *> mA; const AssertTestVector<void *> &A = mA;

    ASSERT_SAFE_FAIL(mA[-1]);
    ASSERT_SAFE_FAIL(mA[ 0]);
    ASSERT_SAFE_FAIL(mA[ 1]);

    ASSERT_SAFE_FAIL( A[-1]);
    ASSERT_SAFE_FAIL( A[ 0]);
    ASSERT_SAFE_FAIL( A[ 1]);

    mA.push_back(0);  // increase the length to one

    ASSERT_SAFE_FAIL(mA[-1]);
    ASSERT_SAFE_PASS(mA[ 0]);
    ASSERT_SAFE_FAIL(mA[ 1]);

    ASSERT_SAFE_FAIL( A[-1]);
    ASSERT_SAFE_PASS( A[ 0]);
    ASSERT_SAFE_FAIL( A[ 1]);
#endif  // defined(BDE_BUILD_TARGET_EXC)
}
//..

//=============================================================================
// Forward declaration of functions to support test cases that may be
// implemented after 'main'.

void TestMacroBSLS_ASSERTTEST_IS_ACTIVE();
    // Verify the 'BSLS_ASSERTTEST_IS_ACTIVE' macro.

void TestMacroBSLS_ASSERTTEST_PASS_OR_FAIL(AssertTestMonitor *pMonitor);
    // Verify the 'BSLS_ASSERTTEST_ASSERT*_PASS', and
    // 'BSLS_ASSERTTEST_ASSERT*_FAIL' macros, and use the specified 'pMonitor'
    // to verify/suppress output.

void TestMacroBSLS_ASSERTTEST_PASS_OR_FAIL_RAW(AssertTestMonitor *pMonitor);
    // Verify the 'BSLS_ASSERTTEST_ASSERT*_PASS_RAW', and
    // 'BSLS_ASSERTTEST_ASSERT*_FAIL_RAW' macros, and use the specified
    // 'pMonitor' to verify/suppress output.

void TestMacroBSLS_ASSERTTEST_CHECK_LEVEL(AssertTestMonitor *pMonitor);
    // Verify that 'BSLS_ASSERTTEST_ASSERT*_PASS', and
    // 'BSLS_ASSERTTEST_ASSERT*_FAIL' macros are expanded/not expanded and
    // therefore execute or do not execute their assertion expression in the
    // different assertion-level modes.  Use the specified 'pMonitor' to
    // verify/suppress output.


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? atoi(argv[1]) : 0;
            verbose = argc > 2;
        veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bsls::Log::setLogMessageHandler(bsls::Log::stderrMessageHandler);

    AssertTestMonitor monitor(veryVeryVerbose);

    // We need to be completely silent when testing for silence.
    if (test != -2) {
        printf("TEST " __FILE__ " CASE %d\n", test);
    }

    switch (test) { case 0:  // zero is always the leading case
      case 14: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #1
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Run the usage example.
        //
        // Testing:
        //   Test case 1: vector
        //   Test case 2: 'PASS' macros
        // --------------------------------------------------------------------
        if (verbose) puts("\nUSAGE EXAMPLES"
                          "\n==============");

        testVectorArrayAccess();

        testVectorArrayAccess2();
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING ASSERTTESTHANDLERGUARD
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Run the usage example.
        //
        // Testing:
        //   class AssertTestHandlerGuard
        //   AssertTestHandlerGuard::AssertTestHandlerGuard()
        //   ~AssertTestHandlerGuard::AssertTestHandlerGuard()
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING ASSERTTESTHANDLERGUARD"
                          "\n==============================");

        bsls::AssertFailureHandlerGuard outerGuard(&bsls::Assert::failByAbort);

        ASSERT(&bsls::Assert::failByAbort == bsls::Assert::violationHandler());
        ASSERT(&bsls::Review::failByLog   == bsls::Review::violationHandler());
        {
            bsls::AssertTestHandlerGuard innerGuard;

            ASSERT(&bsls::AssertTest::failTestDriver ==
                                             bsls::Assert::violationHandler());
            ASSERT(&bsls::AssertTest::failTestDriverByReview ==
                                             bsls::Review::violationHandler());
        }
        ASSERT(&bsls::Assert::failByAbort == bsls::Assert::violationHandler());
        ASSERT(&bsls::Review::failByLog   == bsls::Review::violationHandler());
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING BAD INPUT TO TRY PROBES
        //
        // Concerns:
        //: 1 Each probe function must return 'true' if passed 'P' and 'false'
        //:   if passed 'F'.
        //:
        //: 2 Any other argument must return 'false' after writing a diagnostic
        //:   message regarding the bad argument to the console.
        //:
        //: 3 Nothing is written to the console for the two recognized
        //:   arguments, 'F' and 'P'.
        //:
        //: 4 The recognized values 'F' and 'P' are case sensitive.
        //:
        //: 5 Most of this test exists to generate desired output from the
        //:   catch-probe functions, so this test should be run manually and
        //:   have its output visually inspected.
        //
        // Plan:
        //: 1 Invoke each probe with every possible 'char' value.  Test that
        //;   'true' is returned for only the argument 'P', that nothing is
        //:   written to the console when the argument is 'P' or 'F', and that
        //:   something is written to the console for any other argument.  The
        //:   diagnostic message will be validated interactively by the user
        //:   reading the console.
        //
        // Testing:
        //   Testing try-probes that write diagnostics to the console
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING BAD INPUT TO TRY PROBES"
                          "\n===============================");

        if (verbose) puts("\nTESTING bsls::AssertTest::tryProbe"
                          "\n==================================");

        for (char c = CHAR_MIN; c != CHAR_MAX; ++c) {
            if ('P' == c) {
                //  expectedResult == 'P' should not print a diagnostic
                EXPECT_SILENCE;
                ASSERT_WITH_STDIO(true == bsls::AssertTest::tryProbe(c, 'S'));
            }
            else if ('F' == c) {
                // expectedResult == 'F' should print as this is a 'try' probe,
                // it should never be reached with an 'F' (fail) argument.
                EXPECT_OUTPUT_ON_STDOUT;
                ASSERT_WITH_STDIO(false == bsls::AssertTest::tryProbe(c, 'S'));
            }
            else {
                // invalid character should print a diagnostic to 'stdout'
                EXPECT_OUTPUT_ON_STDOUT;
                ASSERTV_WITH_STDIO(PrettyChar(c),
                                   false == bsls::AssertTest::tryProbe(c,'S'));
            }
        }

        for (char c = CHAR_MIN; c != CHAR_MAX; ++c) {
            if ('S' == c) {
                // expectedResult == 'S' should not print any diagnostic
                EXPECT_SILENCE;
                ASSERT_WITH_STDIO(true == bsls::AssertTest::tryProbe('P', c));
            }
            else if ('A' == c) {
                // expectedResult == 'A' should not print any diagnostic
                EXPECT_SILENCE;
                ASSERT_WITH_STDIO(true == bsls::AssertTest::tryProbe('P', c));
            }
            else if ('O' == c) {
                // expectedResult == 'O' should not print any diagnostic
                EXPECT_SILENCE;
                ASSERT_WITH_STDIO(true == bsls::AssertTest::tryProbe('P', c));
            }
            else if ('I' == c) {
                // expectedResult == 'I' should not print any diagnostic
                EXPECT_SILENCE;
                ASSERT_WITH_STDIO(true == bsls::AssertTest::tryProbe('P', c));
            }
            else {
                // invalid character should print a diagnostic to 'stdout'
                EXPECT_OUTPUT_ON_STDOUT;
                ASSERTV_WITH_STDIO(PrettyChar(c),
                                   false == bsls::AssertTest::tryProbe('P',c));
            }
        }

        if (verbose) puts("\nTESTING bsls::AssertTest::tryProbeRaw"
                          "\n=====================================");

        for (char c = CHAR_MIN; c != CHAR_MAX; ++c) {
            if ('P' == c) {
                // expectedResult == 'P' should not print any diagnostic
                EXPECT_SILENCE;
                ASSERT_WITH_STDIO(
                                true == bsls::AssertTest::tryProbeRaw(c, 'S'));
            }
            else if ('F' == c) {
                // expectedResult == 'F' should print as this is a 'try' probe,
                // it should never be reached with an 'F' (fail) argument.
                EXPECT_OUTPUT_ON_STDOUT;
                ASSERT_WITH_STDIO(
                               false == bsls::AssertTest::tryProbeRaw(c, 'S'));
            }
            else {
                // invalid character should print a diagnostic to 'stdout'
                EXPECT_OUTPUT_ON_STDOUT;
                ASSERTV_WITH_STDIO(PrettyChar(c),
                                false == bsls::AssertTest::tryProbeRaw(c,'S'));
            }
        }

        for (char c = CHAR_MIN; c != CHAR_MAX; ++c) {
            if ('S' == c) {
                // expectedResult == 'S' should not print any diagnostic
                EXPECT_SILENCE;
                ASSERT_WITH_STDIO(
                                true == bsls::AssertTest::tryProbeRaw('P', c));
            }
            else if ('A' == c) {
                // expectedResult == 'A' should not print any diagnostic
                EXPECT_SILENCE;
                ASSERT_WITH_STDIO(
                                true == bsls::AssertTest::tryProbeRaw('P', c));
            }
            else if ('O' == c) {
                // expectedResult == 'O' should not print any diagnostic
                EXPECT_SILENCE;
                ASSERT_WITH_STDIO(
                                true == bsls::AssertTest::tryProbeRaw('P', c));
            }
            else if ('I' == c) {
                // expectedResult == 'I' should not print any diagnostic
                EXPECT_SILENCE;
                ASSERT_WITH_STDIO(
                                true == bsls::AssertTest::tryProbeRaw('P', c));
            }
            else {
                // invalid character should print a diagnostic to 'stdout'
                EXPECT_OUTPUT_ON_STDOUT;
                ASSERTV_WITH_STDIO(
                               PrettyChar(c),
                               false == bsls::AssertTest::tryProbeRaw('P', c));
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING BSLS_ASSERTTEST_CHECK_LEVEL MACRO
        //
        // Concerns:
        //: 1 When 'BSLS_ASSERTTEST_CHECK_LEVEL' is enabled, the assert macros
        //:   should fail if a The assert that fails is not of a matching
        //:   level.
        //:
        //: 2 Earlier tests already thoroughly check that the macros behave
        //:   properly regardless of assert level when level checking is not
        //:   enabled.
        //
        // Plan:
        //: 1 Manually re-include 'bsls_assserttest.h' header after changing
        //:   build configurations with 'BSLS_ASSERTTEST_CHECK_LEVEL' defined,
        //:   then check that the macros behave appropriately.
        //
        // Testing:
        //   BSLS_ASSERTTEST_CHECK_LEVEL
        //   BSLS_ASSERTTEST_CHECK_LEVEL_ARG
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING BSLS_ASSERTTEST_CHECK_LEVEL MACRO"
                          "\n=========================================");

        TestMacroBSLS_ASSERTTEST_CHECK_LEVEL(&monitor);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING BSLS_ASSERT_TEST_*_RAW MACROS
        //
        // Concerns:
        //: 1 Each macro must not evaluate the passed expression unless enabled
        //:   by the assert level that is in effect.  A '_PASS' macro should
        //:   fail if and only if the passed expression raises an assertion
        //:   through a 'BSLS_ASSERT' macro.  A '_FAIL' macro should fail if
        //:   the passed expression does not raise an assertion.  Failure
        //:   should not depend on the file where the assertion is raised.
        //:   Failure should be indicated by invoking an 'ASSERT' macro that is
        //:   defined locally within the test driver.
        //
        // Plan:
        //: 1 The results of these macros depend on the initial definition of a
        //:   number of configuration macros, normally passed on the command
        //:   line.  In order to test the full set of potential interactions,
        //:   we shall take the unusual step of '#undef'ining the header guard
        //:   for both this component and 'bsls_assert.h', in order to
        //:   repeatedly '#include' these headers after setting up a "clean"
        //:   set of configuration macros for each such inclusion to test.
        //:   Note that the test code is extracted into a function defined
        //:   after main as we will be using the preprocessor to avoid
        //:   impacting any following test case that does not have the same
        //:   issue.
        //
        // Testing:
        //   BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(FUNCTION)
        //   BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(FUNCTION)
        //   BSLS_ASSERTTEST_ASSERT_PASS_RAW(FUNCTION)
        //   BSLS_ASSERTTEST_ASSERT_FAIL_RAW(FUNCTION)
        //   BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(FUNCTION)
        //   BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(FUNCTION)
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING BSLS_ASSERT_TEST_*_RAW MACROS"
                          "\n=====================================");

        TestMacroBSLS_ASSERTTEST_PASS_OR_FAIL_RAW(&monitor);
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING BSLS_ASSERT_TEST_* API  MACROS
        //
        // Concerns:
        //: 1 Each macro must not evaluate the passed expression unless enabled
        //:   by the assert level that is in effect.  A '_PASS' macro should
        //:   fail if and only if the passed expression raises an assertion
        //:   through a 'BSLS_ASSERT' macro.  A '_FAIL' macro should fail if
        //:   the passed expression does not raise an assertion, or if the
        //:   raised assertion originates in a file with a name that does not
        //:   correspond to the component under test.  Failure should be
        //:   indicated by invoking an 'ASSERT' macro that is defined locally
        //:   within the test driver.
        //
        // Plan:
        //: 1 The results of these macros depend on the initial definition of a
        //:   number of configuration macros, normally passed on the command
        //:   line.  In order to test the full set of potential interactions,
        //:   we shall take the unusual step of '#undef'ining the header guard
        //:   for both this component and 'bsls_assert.h', in order to
        //:   repeatedly '#include' these headers after setting up a "clean"
        //:   set of configuration macros for each such inclusion to test.
        //:   Note that the test code is extracted into a function defined
        //:   after main as we will be using the preprocessor to avoid
        //:   impacting any following test case that does not have the same
        //:   issue.
        //
        // Testing:
        //   BSLS_ASSERTTEST_ASSERT_SAFE_PASS(FUNCTION)
        //   BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(FUNCTION)
        //   BSLS_ASSERTTEST_ASSERT_PASS(FUNCTION)
        //   BSLS_ASSERTTEST_ASSERT_FAIL(FUNCTION)
        //   BSLS_ASSERTTEST_ASSERT_OPT_PASS(FUNCTION)
        //   BSLS_ASSERTTEST_ASSERT_OPT_FAIL(FUNCTION)
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING BSLS_ASSERT_TEST_* API MACROS"
                          "\n=====================================");

        TestMacroBSLS_ASSERTTEST_PASS_OR_FAIL(&monitor);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING CATCH PROBES
        //
        // Concerns:
        //: 1 Each probe function must return 'true' if passed 'P' and 'false'
        //:   if passed 'F'.  Any other argument must return 'false' after
        //:   writing a diagnostic message regarding the bad argument to the
        //:   console.  Nothing should be written to the console for the two
        //:   recognized arguments.  The recognized values 'F' and 'P' are case
        //:   sensitive.
        //:
        //: 2 Unless all three arguments to the catch-probe functions meet some
        //:   validity constraints, the function must return 'false' and write
        //:   a diagnostic message to the console.  Valid 'expectedResult' are
        //:   either 'F' or 'P'.  Valid 'caughtException' attributes are a
        //:   pointer to a non-empty null-terminated string for 'expression', a
        //:   pointer to a non-empty null-terminated string for 'filename', and
        //:   a non-negative 'lineNumber'.  Valid 'testDriverFileName' values
        //:   are either a null pointer, or a pointer to a non-empty
        //:   null-terminated string.
        //:
        //: 3 When passed valid arguments, a catch-probe shall return 'false'
        //:   unless 'expectedResult' == 'F' and either 'testDriverFileName' is
        //:   null, or 'testDriverFileName' points to a filename that
        //:   corresponds to the same component as referenced by the 'filename'
        //:   attribute of 'caughtException'.  Two filenames correspond to the
        //:   same component if the file extension is either ".h", ".cpp",
        //:   ".t.cpp", or ".g.cpp", and the filenames have the same value when
        //:   stripped of both the extension and any leading pathname.
        //
        // Plan:
        //: 1 We will take a table driven approach, verifying that each catch-
        //:   probe returns the correct result combination of valid and invalid
        //:   arguments.  As an 'expectedResult' of 'P' must always return
        //:   'false', the table contains only the remaining arguments, and the
        //:   "EXPECTED RESULT" from calling this function with 'F' for
        //:   'expectedResult'.  Each iteration of the loop will test with both
        //:   potential 'expectedResult' arguments, 'F' and 'P'.
        //:
        //: 2 Invoke each probe with every possible 'char' value.  Test that
        //:   'true' is returned for only the argument 'P', that nothing is
        //:   written to the console when the argument is 'P' or 'F', and that
        //:   something is written to the console for any other argument.  The
        //:   diagnostic message will be validated interactively by the user
        //:   reading the console.
        //:
        //: 3 Note that unless all three arguments to the catch-probe functions
        //:   meet some validity constraints, the function will return 'false'
        //:   and write a diagnostic message to the console.  As test drivers
        //:   must run without such messages unless there is an error, we defer
        //:   such test scenarios to test case -2.
        //:
        //: 4 By testing that two different sets of compatible names do not
        //:   match each other, we do not need to add other valid component
        //:   names into the set of potential invalid matches.
        //
        // Testing:
        //   AssertTest::catchProbe(char, ...);
        //   AssertTest::catchProbeRaw(char, ...);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING CATCH PROBES"
                          "\n====================");

        if (verbose) puts("\nTESTING catchProbe"
                          "\n==================");

        static struct ThrowNamesData {
            int         d_lineNumber;
            const char *d_throwFilename;  // The component source file name
            int         d_componentId;    // What component does it match
        } THROW_NAMES[] =

         {
            { L_, "abcp_a.h",         1 },
            { L_, "abcp_a.cpp",       1 },
            { L_, "/abcp_a.h",        1 },
            { L_, "/abcp_a.cpp",      1 },
            { L_, "//abcp_a.h",       1 },

            { L_, "abcp_z.z.h",       2 },
            { L_, "abcp_z.z.cpp",     2 },
            { L_, "//abcp_z.z.h",     2 },

            { L_, "abca_a.h",        11 },  // no match from test array
            { L_, "abca_a.cpp",      12 },
            { L_, "xyzpac_uvw.h",    13 },
            { L_, "xyzpac_uvw.cpp",  14 },
        };
        const int NUM_THROW_NAMES = sizeof THROW_NAMES / sizeof *THROW_NAMES;

        static struct TestNamesData {
            int         d_lineNumber;
            const char *d_testFilename;   // The component source file name
            int         d_componentId;    // What component does it match
        } TEST_NAMES[] =

         {
            { L_, "abcp_a.t.cpp",      1 },
            { L_, "abcp_a.g.cpp",      1 },
            { L_, "abcp_a.0.t.cpp",    1 },
            { L_, "abcp_a.1.g.cpp",    1 },
            { L_, "abcp_a.05.t.cpp",   1 },
            { L_, "abcp_a.12.g.cpp",   1 },

            { L_, "abcp_z.z.t.cpp",    2 },
            { L_, "abcp_z.z.g.cpp",    2 },
            { L_, "/abcp_z.z.t.cpp",   2 },
            { L_, "/abcp_z.z.g.cpp",   2 },

            { L_, "lowbar_now.t.cpp", 21 },  // no match from throw array
            { L_, "lowbar_now.g.cpp", 22 },

            // Test driver name is not allowed to be empty
            { L_, "",                 -1 }
        };
        const int NUM_TEST_NAMES = sizeof TEST_NAMES / sizeof *TEST_NAMES;

        static const struct {
            int         d_lineNumber;
            const char *d_expression;
            int         d_assertedLine;
            const char *d_assertedLevel;
            bool        d_checksResult;
        } DATA[] = {
            //     ASSERTED   LINE             CHECKS
            //LINE EXPRESSION NUMBER   LEVEL   RESULT
            //---- ---------- ------   ------  ------
            { L_,  "",             0,  "SAF",  false },
            { L_,  "0 != x",       0,  "SAF",  false },
            { L_,  "",            13,  "SAF",  false },
            { L_,  "0 != x",      13,  "SAF",  true  },
            { L_,  "false",       42,  "SAF",  true  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bool probeResult;
            // Use in every call.  No initializer as we *want* a warning if an
            // 'ASSERT' would read it without assignment.

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE           = DATA[ti].d_lineNumber;
            const char *const EXPRESSION     = DATA[ti].d_expression;
            const int         ASSERTED_LINE  = DATA[ti].d_assertedLine;
            const char *const ASSERTED_LEVEL = DATA[ti].d_assertedLevel;
            const bool        CHECKS_RESULT  = DATA[ti].d_checksResult;

            for (int i = 0; i != NUM_THROW_NAMES; ++i) {
                const ThrowNamesData& ADATA = THROW_NAMES[i]; // ADATA-ASSERT
                const int         ALINE           = ADATA.d_lineNumber;
                const char *const THROW_NAME      = ADATA.d_throwFilename;
                const int         THROW_COMPONENT = ADATA.d_componentId;

                const bsls::AssertTestException EXCEPTION(EXPRESSION,
                                                          THROW_NAME,
                                                          ASSERTED_LINE,
                                                          ASSERTED_LEVEL);

                EXPECT_OUTPUT_ON_STDOUT_STDERR_IF(!CHECKS_RESULT);
                probeResult = bsls::AssertTest::catchProbe('P',
                                                           true,
                                                           'S',
                                                           EXCEPTION,
                                                           0);
                ASSERT_STDIO;
                ASSERTV(LINE, probeResult, false == probeResult);

                EXPECT_OUTPUT_ON_BOTH_IF(!CHECKS_RESULT);
                probeResult = bsls::AssertTest::catchProbe('F',
                                                           true,
                                                           'S',
                                                           EXCEPTION,
                                                           0);
                ASSERT_STDIO;
                ASSERTV(LINE, probeResult, CHECKS_RESULT == probeResult);

                for (int j = 0; j != NUM_TEST_NAMES; ++j) {
                    const TestNamesData& TDATA = TEST_NAMES[j];

                    const int          TLINE          = TDATA.d_lineNumber;
                    const char * const TEST_NAME      = TDATA.d_testFilename;
                    const int          TEST_COMPONENT = TDATA.d_componentId;

                    const bool NAME_RESULT = THROW_COMPONENT == TEST_COMPONENT;

                    const bool BAD_TEST_NAME = TEST_COMPONENT < 0;

                    const bool EXPECTED_RETURN = CHECKS_RESULT && NAME_RESULT;

                    if (veryVerbose) {
                        P_(LINE) P_(ALINE) P(TLINE);
                        P_(EXPRESSION) P_(ASSERTED_LINE) P_(ASSERTED_LEVEL)
                                                              P(CHECKS_RESULT);
                        P_(THROW_NAME) P_(TEST_NAME) P(NAME_RESULT);
                    }

                    EXPECT_OUTPUT_ON_STDOUT_STDERR_IF(!CHECKS_RESULT);
                    probeResult = bsls::AssertTest::catchProbe('P',
                                                               true,
                                                               'S',
                                                               EXCEPTION,
                                                               TEST_NAME);
                    ASSERT_STDIO;
                    ASSERTV(LINE, TLINE, probeResult, false == probeResult);

                    EXPECT_OUTPUT_ON_STDOUT_STDERR_IF(!CHECKS_RESULT);
                    probeResult = bsls::AssertTest::catchProbe('P',
                                                               true,
                                                               'S',
                                                               EXCEPTION,
                                                               TEST_NAME);
                    ASSERT_STDIO;
                    ASSERTV(LINE, TLINE, probeResult, false == probeResult);

                    EXPECT_OUTPUT_ON_STDERR_IF(!CHECKS_RESULT);
                    // We set expectations for 'stdout' "by hand" here because
                    // there is no named macro that can handle this complexity.
                    // The next planned code update will make expectations a
                    // mini-language so the above will be something like:
                    // 'IF(!CHECK_RESULT, BOTH) | IF(BAD_TEST_NAME, STDOUT)'.
                    stdioProctor->setOut(BAD_TEST_NAME || !CHECKS_RESULT);
                    probeResult = bsls::AssertTest::catchProbe('F',
                                                               true,
                                                               'S',
                                                               EXCEPTION,
                                                               TEST_NAME);
                    ASSERT_STDIO;
                    ASSERTV(LINE, TLINE, EXPECTED_RETURN, probeResult,
                            EXPECTED_RETURN == probeResult);
                }
            }
        }

        if (verbose) puts("\nTESTING catchProbeRaw"
                          "\n=====================");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE           = DATA[ti].d_lineNumber;
            const char *const EXPRESSION     = DATA[ti].d_expression;
            const int         ASSERTED_LINE  = DATA[ti].d_assertedLine;
            const char *const ASSERTED_LEVEL = DATA[ti].d_assertedLevel;

            for (int i = 0; i != NUM_THROW_NAMES; ++i) {
                const ThrowNamesData& ADATA = THROW_NAMES[i]; // ASSERT-DATA
                const int         ALINE           = ADATA.d_lineNumber;
                const char *const THROW_NAME      = ADATA.d_throwFilename;

                if (veryVerbose) {
                    P_(LINE)  P(EXPRESSION);
                    P_(ALINE) P(THROW_NAME);
                }

                const bsls::AssertTestException EXCEPTION(EXPRESSION,
                                                          THROW_NAME,
                                                          ASSERTED_LINE,
                                                          ASSERTED_LEVEL);

                EXPECT_OUTPUT_ON_STDOUT;
                probeResult = bsls::AssertTest::catchProbeRaw('P',
                                                              true,
                                                              'S',
                                                              EXCEPTION);
                ASSERT_STDIO;
                ASSERTV(LINE, ALINE, probeResult, false == probeResult);

                EXPECT_SILENCE;
                probeResult = bsls::AssertTest::catchProbeRaw('F',
                                                              true,
                                                              'S',
                                                              EXCEPTION);
                ASSERT_STDIO;
                ASSERTV(LINE, ALINE, probeResult, true == probeResult);
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING TRY PROBES
        //
        // Concerns:
        //: 1 Each probe function returns 'true' if passed 'P' and 'false' if
        //:   passed 'F' for the 'expectedResult' parameter.
        //:
        //: 2 Any other argument returns 'false' after writing a diagnostic
        //:   message regarding the bad argument(s) to the console.
        //:
        //: 3 Nothing is written to the console for the two recognized
        //:   arguments, 'F' and 'P'.
        //:
        //: 4 The recognized values 'F' and 'P' are case sensitive.
        //
        // Plan:
        //: 1 A passing test case run in non-verbose mode should not write
        //:   anything to the console, so the testing of all arguments other
        //:   than 'F' or 'P' is deferred to the manually run test case, -1.
        //:   Each try-probe function is called to confirm it produces the
        //:   required value for the two supported arguments, 'F' must always
        //:   return 'false' and 'P' must always return 'true'.
        //
        // Testing:
        //   AssertTest::tryProbe(char, char);
        //   AssertTest::tryProbeRaw(char, char);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING TRY PROBES"
                          "\n==================");

        if (verbose) puts("\nTESTING tryProbe"
                          "\n================");

        EXPECT_SILENCE;
        ASSERT_WITH_STDIO( true == bsls::AssertTest::tryProbe('P', 'S'));

        EXPECT_OUTPUT_ON_STDOUT;
        ASSERT_WITH_STDIO(false == bsls::AssertTest::tryProbe('F', 'S'));

        if (verbose) puts("\nTESTING tryProbeRaw"
                          "\n===================");

        EXPECT_SILENCE;
        ASSERT_WITH_STDIO( true == bsls::AssertTest::tryProbeRaw('P', 'S'));
        EXPECT_OUTPUT_ON_STDOUT;
        ASSERT_WITH_STDIO(false == bsls::AssertTest::tryProbeRaw('F', 'S'));
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING FAILTESTDRIVER
        //
        // Concerns:
        //: 1 The function 'bsls::AssertTest::failTestDriver' can be installed
        //:   as an assertion-failure handler with 'bsls::Assert'.
        //: 2 The function throws an exception of type
        //:   'bsls::AssertTestException' whose attributes exactly match the
        //:   arguments to the function call.
        //: 3 'failTestDriverByReview' should always call into 'failTestDriver'
        //:   and behave in the same way, and can be installed as a
        //:   review-failure-handler with 'bsls::Review'.
        //
        // Plan:
        //: 1 First, we will install 'bsls::AssertTest::failTestDriver' as the
        //:   active assertion-failure handler function, demonstrating it has
        //:   the required signature.  Then a table-driven approach will
        //:   demonstrate that each call to the function throws an exception
        //:   having the expected attributes.
        //
        // Testing:
        //   AssertTest::failTestDriver(const AssertViolation &);
        //   AssertTest::failTestDriverByReview(const ReviewViolation &);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING FAILTESTDRIVER"
                          "\n======================");

#if !defined(BDE_BUILD_TARGET_EXC)
        if (verbose) {
            puts("\nTest not supported unless exceptions are available.");
        }
#else

        // First we install 'bsls::AssertTest::failTestDriver' as the active
        // assertion-failure handler, to verify it has the correct signature.
        bsls::AssertFailureHandlerGuard guard(
                                            &bsls::AssertTest::failTestDriver);

        // Next we verify that the installed assertion-failure handler throws
        // the expected exception when called directly.
        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_expression;
            const char *d_filename;
            int         d_testLine;
            const char *d_level;
        } DATA[] = {
            //LINE  EXPRESSION  TESTED_FILENAME  TESTLINE  LEVEL
            //----  ----------  -------   --------  ----
            { L_,   "",         "",       0,        "SAF"   },
            { L_,   "Testing",  "foo.h",  123,      "SAF"   },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE       = DATA[ti].d_lineNumber;
            const char *const EXPRESSION = DATA[ti].d_expression;
            const char *const FILENAME   = DATA[ti].d_filename;
            const int         TESTLINE   = DATA[ti].d_testLine;
            const char *const TESTLEVEL  = DATA[ti].d_level;

            if (veryVerbose) {
                T_ P_(EXPRESSION) P_(FILENAME) P(TESTLINE)
            }

            // Validate test description.
            try {
                bsls::AssertTest::failTestDriver(bsls::AssertViolation(
                                   EXPRESSION, FILENAME, TESTLINE, TESTLEVEL));
            }
            catch(const bsls::AssertTestException& ex) {
                ASSERTV(LINE, EXPRESSION, ex.expression(),
                        0 == strcmp(EXPRESSION, ex.expression()));
                ASSERTV(LINE, FILENAME, ex.filename(),
                        0 == strcmp(FILENAME, ex.filename()));
                ASSERTV(LINE, TESTLINE, ex.lineNumber(),
                        TESTLINE == ex.lineNumber());
            }

            // Validate test description.
            try {
                bsls::ReviewViolation violation(EXPRESSION, FILENAME, TESTLINE,
                                                "TEST", 1);
                bsls::AssertTest::failTestDriverByReview(violation);
            }
            catch(const bsls::AssertTestException& ex) {
                ASSERTV(LINE, EXPRESSION, ex.expression(),
                        0 == strcmp(EXPRESSION, ex.expression()));
                ASSERTV(LINE, FILENAME, ex.filename(),
                        0 == strcmp(FILENAME, ex.filename()));
                ASSERTV(LINE, TESTLINE, ex.lineNumber(),
                        TESTLINE == ex.lineNumber());
            }
        }
#endif  // defined(BDE_BUILD_TARGET_EXC)
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING THE MACRO BSLS_ASSERTTEST_IS_ACTIVE
        //
        // Concerns:
        //: 1 The macro expands to an expression that evaluates to either
        //:   'true' or 'false', according to the current build mode and the
        //:   string passed to the macro.
        //: 2 There are six valid strings, "S", "A", "O", "S2", "A2", and "O2",
        //:   each of which must be recognized, in all possible build modes.
        //
        // Plan:
        //: 1 The result of this macro depends on the initial definition of a
        //:   number of configuration macros, normally passed on the command
        //:   line.  In order to test the full set of potential interactions,
        //:   we shall take the unusual step of '#undef'ining the header guard
        //:   for both this component and 'bsls_assert.h', in order to
        //:   repeatedly '#include' these headers after setting up a "clean"
        //:   set of configuration macros for each such inclusion to test.
        //:   Each configuration shall validate the macro for all 6 valid
        //:   strings.
        //:
        //: 2 Note that the test code is extracted into a function defined
        //:   after main as we will be using the preprocessor to avoid
        //:   impacting any following test case that does not have the same
        //:   issue.
        //
        // Testing:
        //   BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG
        //   BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG
        //   BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
        //   BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
        //   BSLS_ASSERTTEST_IS_ACTIVE(TYPE)
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING THE MACRO BSLS_ASSERTTEST_IS_ACTIVE"
                          "\n===========================================");

        TestMacroBSLS_ASSERTTEST_IS_ACTIVE();
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'isValidExpected'
        //
        // Concerns:
        //: 1 'isValidExpected' should return 'true' for only two possible
        //:   values, 'F', and 'P'.
        //:
        //: 2 'isValidExpectedLevel' should return 'true' for only three
        //:   possible values, 'S', 'O', 'A', and 'I'.
        //:
        //: 3 All other test values return false.
        //:
        //: 4 The function is case-sensitive.
        //
        // Plan:
        //: 1 As the set of characters is enumerable and small, iterate over
        //:   all possible character values calling 'isValidExpected'.  The
        //:   result may be 'true' for only two values, 'F' and 'P'.
        //
        // Testing:
        //   AssertTest::isValidExpected(char);
        //   AssertTest::isValidExpectedLevel(char);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING \'isValidExpected\'"
                          "\n=========================");

        for (char c = CHAR_MIN; c != CHAR_MAX; ++c) {
            const bool expectedResult = isOneOf(c, "FP");
            if (veryVerbose) {
                T_ P_(c) P(expectedResult)
            }

            const bool testResult = bsls::AssertTest::isValidExpected(c);
            ASSERTV(c, expectedResult, expectedResult == testResult);
        }

        for (char c = CHAR_MIN; c != CHAR_MAX; ++c) {
            const bool expectedResult = isOneOf(c, "SOAI");
            if (veryVerbose) {
                T_ P_(c) P(expectedResult)
            }

            const bool testResult = bsls::AssertTest::isValidExpectedLevel(c);
            ASSERTV(c, expectedResult, expectedResult == testResult);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ISVALIDASSERTBUILD
        //
        // Concerns:
        //: 1 The function returns 'true' for the six recognized string values,
        //:   and false for all other strings.
        //:
        //: 2 The string comparison is case sensitive.
        //:
        //: 3 The order of the characters is significant for two character
        //:   strings.
        //:
        //: 4 Neither leading nor trailing whitespace are valid.
        //:
        //: 5 The function gracefully handles edge cases like a null pointer,
        //:   or the empty string, "".
        //
        // Plan:
        //: 1 Using a basic test-table, test a representative sample of 0, 1, 2
        //:   and 3 character strings, covering the valid values, and simple
        //:   permutation of those valid values.
        //
        // Testing:
        //   AssertTest::isValidAssertBuild(const char *);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING ISVALIDASSERTBUILD"
                          "\n==========================");

        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O", "O2"
            bool        d_expectedResult;   // 'true' or 'false'

        } DATA[] = {

            //LINE  TYPE   RESULT
            //----  ----   ------
            { L_,   0,     false  },
            { L_,   "",    false  },
            { L_,   "S",   true   },
            { L_,   "A",   true   },
            { L_,   "O",   true   },
            { L_,   "X",   false  },
            { L_,   "S2",  true   },
            { L_,   "A2",  true   },
            { L_,   "O2",  true   },
            { L_,   "X2",  false  },
            { L_,   "SY",  false  },
            { L_,   "AY",  false  },
            { L_,   "OY",  false  },
            { L_,   "XY",  false  },
            { L_,   "S2Z", false  },
            { L_,   "A2Z", false  },
            { L_,   "O2Z", false  },
            { L_,   "X2Z", false  },
            { L_,   "s",   false  },
            { L_,   "a",   false  },
            { L_,   "o",   false  },
            { L_,   "s2",  false  },
            { L_,   "a2",  false  },
            { L_,   "o2",  false  },
            { L_,   "SS",  false  },
            { L_,   "AA",  false  },
            { L_,   "OO",  false  },
            { L_,   "2S",  false  },
            { L_,   "2A",  false  },
            { L_,   "2O",  false  },
            { L_,   " S",  false  },
            { L_,   " A",  false  },
            { L_,   " O",  false  },
            { L_,   " S2", false  },
            { L_,   " A2", false  },
            { L_,   " O2", false  },
            { L_,   "S ",  false  },
            { L_,   "A ",  false  },
            { L_,   "O ",  false  },
            { L_,   "S2 ", false  },
            { L_,   "A2 ", false  },
            { L_,   "O2 ", false  },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNumber;
            const char *const TYPE   = DATA[ti].d_assertBuildType;
            bool              RESULT = DATA[ti].d_expectedResult;

            if (veryVerbose) {
                T_ P_(TYPE) P(RESULT)
            }

            // Validate test description.
            bool testResult = bsls::AssertTest::isValidAssertBuild(TYPE);
            ASSERTV(LINE, TYPE, RESULT, RESULT == testResult);
        }  // table-driven 'for' loop
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // STDIO MONITORING TEST
        //
        // Concerns:
        //: 1 We can silence both 'stdout' and 'stderr'.
        //:
        //: 2 We can detect writing to 'stdout' and 'stderr'.
        //
        // Plan:
        //: 1 We cannot easily detect what happens to 'stderr' but we can do so
        //:   for 'stdout' by running against ourselves and seeing if there is
        //:   output from the child process.  To make sure the output is not
        //:   just lost or a mess, we also start the child process with very,
        //:   very verbose arguments and check that it indeed prints to
        //:   'stdout' what is expected.
        //:
        //: 2 Plan is simple, we turn on the monitor, write, and check that is
        //:   has been detected, and what we did not write to is not detected.
        //
        // Testing:
        //   STDIO MONITORING TEST
        // --------------------------------------------------------------------

        if (verbose) puts("\nSTDIO MONITORING TEST"
                          "\n=====================");

        bsls::AssertFailureHandlerGuard guard(
                                            &bsls::AssertTest::failTestDriver);

        if (veryVerbose) puts("Verify output detection.");
        {
            StdIoTestMonitorDuo basicMon(veryVeryVerbose);
            basicMon.start();
            puts("I don't need it to be easy, I need it to be worth it.");
            basicMon.stop();
            ASSERT(true  == basicMon.wasThereStdoutOutput());
            ASSERT(false == basicMon.wasThereStderrOutput());

            if (veryVeryVerbose) {
                basicMon.prepareStdoutForHumans();
                printf("stdout: [%.*s]\n", basicMon.stdoutBufferSize(),
                                           basicMon.stdoutBuffer());
            }

            if (false != basicMon.wasThereStderrOutput()) {
                basicMon.prepareStderrForHumans();
                printf("stderr: [%.*s]\n", basicMon.stderrBufferSize(),
                                           basicMon.stderrBuffer());
            }

            basicMon.resetBuffers();
            basicMon.start();
            fputs("There is no substitute for hard work.\n", stderr);
            basicMon.stop();
            ASSERT(false == basicMon.wasThereStdoutOutput());
            ASSERT(true  == basicMon.wasThereStderrOutput());

            if (false != basicMon.wasThereStdoutOutput()) {
                basicMon.prepareStdoutForHumans();
                printf("stdout: [%.*s]\n", basicMon.stdoutBufferSize(),
                                           basicMon.stdoutBuffer());
            }

            if (veryVeryVerbose) {
                basicMon.prepareStderrForHumans();
                printf("stderr: [%.*s]\n", basicMon.stderrBufferSize(),
                                           basicMon.stderrBuffer());
            }
        }

        if (veryVerbose) puts("Verify output suppression.");
        {
            ASSERT(argv[0][0] != '\0');

            if (argv[0][0] == '\0') {
                puts("Cannot get executable name from argv[0], test skipped");
                break;                                                 // BREAK
            }

            static const char        k_ARGS[]   = " -2 v v v";
            static const std::size_t k_ARGS_LEN = (sizeof k_ARGS) - 1;

            const size_t len = strlen(argv[0]) + k_ARGS_LEN + 2 + 4;

            // Use local buffer or allocate if string too long for the stack.
            char  shortCmdLine[64];
            char *commandLine = len < sizeof shortCmdLine
                                    ? shortCmdLine
                                    : static_cast<char *>(malloc(len));
            ASSERT(0 != commandLine);
            if (!commandLine) {
                break;                                                 // BREAK
            }

            if (veryVerbose) puts("Run case -2 with silencing on");
            commandLine[0] = '"';
            strcpy(commandLine + 1, argv[0]);
            strcat(commandLine, "\" -2");
            if (veryVeryVerbose) P(commandLine);
            char readBuffer[129];
            int  numReadChars = popeneer(commandLine, readBuffer);
            ASSERTV(numReadChars, readBuffer, 0 == numReadChars);

            if (veryVerbose) puts("Run case -2 verbose");
            strcat(commandLine, " v v v");
            if (veryVeryVerbose) P(commandLine);
            numReadChars = popeneer(commandLine, readBuffer);
            ASSERT(numReadChars > 0);

            if (commandLine != shortCmdLine) free(commandLine);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 Want to observe the basic operation of this component.  This test
        //:   will be affected by the current build flags, and must produce the
        //:   correct result in each case.
        //
        // Plan:
        //: 1 Install the negative testing assertion-failure handler, and then
        //:   invoke the 'OPT' version of the test macros, as this is the form
        //:   enabled in most build modes.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) puts("\nBREATHING TEST"
                          "\n==============");

        bsls::AssertFailureHandlerGuard guard(
                                            &bsls::AssertTest::failTestDriver);

        BSLS_ASSERTTEST_ASSERT_OPT_PASS(BSLS_ASSERT_OPT(true));
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(BSLS_ASSERT_OPT(false));
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // CONDITIONALLY SILENCED OUTPUT TO STDOUT
        //
        // Concerns:
        //: 1 We can silence both 'stdout' and 'stderr'.
        //
        // Plan:
        //: 1 The test driver calls itself twice for case -2, with and without
        //:   'veryVeryVerbose' set.  We drive silencing of the monitor with
        //:   'veryVeryVerbose' (see top of 'main').  The calling code asserts
        //:   if it can read something while expecting silence, and if it reads
        //:   something other than what is expected when the -2 is run very,
        //:   very verbose.
        //
        // Testing:
        //   CONDITIONALLY SILENCED OUTPUT TO STDOUT
        // --------------------------------------------------------------------

        monitor.setOut().start();
        puts("Change the world by being yourself.");
        monitor.stop();
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (test == -2) testStatus = 0;

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

//-----------------------------------------------------------------------------
//             TEST MACHINERY FOR STDIO CAPTURE TESTING (POPEN)
//-----------------------------------------------------------------------------

int popeneer(const char *commandLine, char *buffer, int size)
{
#ifdef BSLS_PLATFORM_CMP_MSVC
#define popen  _popen
#define pclose _pclose
#endif

    FILE *childStdout = popen(commandLine, "r");
    if (childStdout == 0) {
        perror("Could not popen");
        P(commandLine);
        return 0;                                                     // RETURN
    }
    char *readChars;
    int   numReadChars = 0;
    while ((readChars = fgets(buffer, size, childStdout))) {
        numReadChars += static_cast<int>(strlen(readChars));
    }
    pclose(childStdout);

    return numReadChars;

#ifdef BSLS_PLATFORM_CMP_MSVC
#undef popen
#undef pclose
#endif
}


//=============================================================================
//                    STDIO CAPTURE AND CONTROL MACHINERY
//-----------------------------------------------------------------------------

///IMPLEMENTATION NOTES
///--------------------
//
///Why Make a Monitor
/// - - - - - - - - -
// The 'stdio' monitoring facilities are implemented here for 3 reasons.  The
// first one is that 'bsls_asserttest' is a very, very low level component that
// cannot depend on any component that itself uses 'BSLS_ASSERT", because then
// that component could not easily be tested.  That rules out using
// 'bsls_outputredirector' as that component uses 'BSLS_ASSERT'.  Testing the
// assert-testing facility should also not depend on access to the file system.
// 'bsls_outputredirector' uses temporary files.  We do not want testing of
// assert testing to fail because we are on a read only file system.  The
// facilities implemented here use only the NUL/null service that can
// reasonably be expected to exist even in a minimal chroot jail.  Last, but
// not least, our requirements here are different from output redirecting.  In
// essence in our testing we do not care *what* is printed exactly as long as
// some warning/error is printed.  The monitoring facility here trades off the
// guaranteed ability to determine what was output (printed) to the ability to
// do the monitoring without needing to write to the file system.
//
///The Capture Buffer
/// - - - - - - - - -
// The monitor implementation uses a stream buffer to capture output.  The C
// and POSIX standards guarantee almost nothing about 'stdio' buffers.  Namely,
// there is no guarantee that a stream will use a buffer at all (when
// requested), no guarantee if the buffer given to the stream will actually be
// the buffer used, or that the whole size/length will be used for buffering,
// or that the buffer is used to store stream output only.  In practice, this
// results in 3 consequences only.  One is that on Linux at minimum a 128 bytes
// buffer must be registered ('setvbuf'), otherwise the C library won't use it
// at all.  The other is that we cannot guarantee for the long term that only
// stream output will be placed into the buffer, so exact comparisons text may
// prove fragile (although it is unlikely).  The third, most unfortunate but
// highly unlikely caveat is that the standards allow this "buffer trick" to be
// broken by implementations at any time, even if it is only used for
// determining if there was any output.  Since this technique is used for
// decades all over the world, it is not very likely that any of it is going to
// break.

static const char *k_NULLFILE_NAME =     // Platform dependent null device name
#ifdef BSLS_PLATFORM_OS_UNIX
    "/dev/null";  // POSIX has /dev/null
#elif BSLS_PLATFORM_OS_WINDOWS
    "NUL";        // Windows has the special "NUL" file everywhere
#elif defined(BSLS_PLATFORM_NULLFILE_NAME)
    BSLS_PLATFORM_NULLFILE_NAME;
#error Unknown platform, use -DBSLS_PLATFORM_NULLFILE_NAME="<somename>"
    BSLS_PLATFORM_COMPILER_ERROR;
#endif

                  // --------------------------------------
                  // class StdIoTestMonitor::MallocedBuffer
                  // --------------------------------------

// CREATORS
StdIoTestMonitor::MallocedBuffer::MallocedBuffer(int size, char nulChar)
: d_ptr(size > 0 ? (char *)malloc(size) : 0)
, d_size(size)
, d_nulChar(nulChar)
{
    if (0 == d_ptr) {
        puts("Could not allocate memory for buffer");
        abort();                                                  // ABORT
    }
    reset();
}

StdIoTestMonitor::MallocedBuffer::~MallocedBuffer()
{
    free(d_ptr);
}

// MANIPULATORS
int StdIoTestMonitor::MallocedBuffer::prepareBufferForHumans(int maxSize)
{
    if (maxSize < 0) maxSize = d_size;
    const int len = d_size > maxSize ? maxSize : d_size;
    char *buf = d_ptr;
    for (int i = 0; i < len; ++i, ++buf) {
        if (d_nulChar == *buf) *buf = '#';
        if (*buf < 32 || (*buf & 0x80)) *buf = '~';
    }

    return len;
}

void StdIoTestMonitor::MallocedBuffer::reset()
{
    memset(d_ptr, d_nulChar, d_size);
}

// ACCESSORS
char StdIoTestMonitor::MallocedBuffer::nulChar() const
{
    return d_nulChar;
}

char *StdIoTestMonitor::MallocedBuffer::ptr() const
{
    return d_ptr;
}

int StdIoTestMonitor::MallocedBuffer::size() const
{
    return d_size;
}

bool StdIoTestMonitor::MallocedBuffer::wasThereAnyOutput() const
{
    if (0 == d_size) {
        puts("Output cannot be checked on a stream not monitored.");
        abort();                                                  // ABORT
    }

    const char * const end = d_ptr + d_size;
    for (const char *p = d_ptr; p < end; ++p) {
        if (d_nulChar != *p) return true;                             // RETURN
    }
    return false;
}

                          // ----------------------
                          // class StdIoTestMonitor
                          // ----------------------

// PRIVATE MANIPULATORS
void StdIoTestMonitor::init(FILE *stream, bool verbose)
{
    d_active = false;  // No capturing or silencing on construction

    if (!d_buffer.size() && verbose) {
        // If the object is constructed without a buffer and with verbosity
        // on, it actually does not have to do anything whatsoever.  Without a
        // buffer we cannot capture.  In verbose mode we don't redirect either.
        // Therefore we set the data to all "empty".

        d_monitoredStream = 0;   // This means not active at all
        d_nullStream      = 0;   // This means not silencing

        d_origFD = d_dupOrigFD = -1;

        return;                                                       // RETURN
    }

    d_monitoredStream = stream;

    if (verbose) {
        d_nullStream = 0;  // Indicates verbose mode

        d_origFD = d_dupOrigFD = -1;
    }
    else {
        d_origFD = fileno(d_monitoredStream);
        if (-1 == d_origFD) {
            perrorAbort("StdIoTestMonitor Unable to get file descriptor");
        }

        d_dupOrigFD = dup(d_origFD);
        if (-1 == d_dupOrigFD) {
            perrorAbort("StdIoTestMonitor Unable to duplicate "
                        "original file descriptor");
        }

        d_nullStream = fopen(k_NULLFILE_NAME, "ab");
        if (0 == d_nullStream) {
            perrorAbort("StdIoTestMonitor Unable to open null device");
        }

        if (-1 == fileno(d_nullStream)) {
            // To ensure it will work later, pure paranoia
            perrorAbort("StdIoTestMonitor Unable to get null file "
                        "descriptor");
        }
    }
}

void StdIoTestMonitor::perrorAbort(const char *msg)
{
    if (d_monitoredStream == stderr && d_active && d_dupOrigFD != -1) {
        // Best effort trying to get 'stderr' back.  If it does not work, we
        // have no way of reporting that problem as all output streams may be
        // silenced.
        (void)dup2(d_dupOrigFD, d_origFD);
    }
    perror(msg);
    abort();
}

void StdIoTestMonitor::safeSetBuf()
{
    if (0 == d_buffer.ptr()) {
        setbuf(d_monitoredStream, 0);
    }
    else {
        setBuf();
    }
}

void StdIoTestMonitor::setBuf()
{
    if (0 != setvbuf(d_monitoredStream,
                          d_buffer.ptr(),
                          _IOFBF,
                          d_buffer.size()))
    {
        perrorAbort("StdIoTestMonitor Unable to set buffer for capture");
    }
}

void StdIoTestMonitor::stopImpl()
{
    fflush(d_monitoredStream);
    setbuf(d_monitoredStream, 0);

    if (!isVerbose()) {
        if (-1 == dup2(d_dupOrigFD, d_origFD)) {
            perrorAbort("StdIoTestMonitor - Unable to restore stream");
        }
    }
}

// PRIVATE ACCESSORS
bool StdIoTestMonitor::doesCapturing() const
{
    return (0 != d_buffer.ptr());
}

bool StdIoTestMonitor::doesSilencing() const
{
    return (d_nullStream != 0);
}

bool StdIoTestMonitor::isNoop() const
{
    return (0 == d_monitoredStream);
}

bool StdIoTestMonitor::isVerbose() const
{
    return (0 == d_nullStream);
}

// CREATORS
StdIoTestMonitor::StdIoTestMonitor(FILE *stream,
                                   int   bufSize,
                                   bool  verbose,
                                   char  nulChar)
: d_buffer(bufSize, nulChar)
{
    init(stream, verbose);
}

StdIoTestMonitor::~StdIoTestMonitor()
{
    if (d_active) {
        stopImpl();
    }

    if (d_dupOrigFD != -1) {
        if (0 != close(d_dupOrigFD)) {
            perrorAbort("StdIoTestMonitor - Unable to close duplicated "
                        "file descriptor");
        }
    }

    if (d_nullStream != 0) {
        if (0 != fclose(d_nullStream)) {
            perrorAbort("StdIoTestMonitor - Unable to close null stream");
        }
    }
}

// MANIPULATORS
char* StdIoTestMonitor::buffer()
{
    return d_buffer.ptr();
}

int StdIoTestMonitor::prepareBufferForHumans(int maxSize)
{
    return d_buffer.prepareBufferForHumans(maxSize);
}

void StdIoTestMonitor::resetBuffer()
{
    if (doesCapturing()) {
        if (d_active) {
            fflush(d_monitoredStream);
            setbuf(d_monitoredStream, 0);
        }
        d_buffer.reset();
        if (d_active) {
            setBuf();
        }
    }
}

void StdIoTestMonitor::start()
{
    if (d_active) {
        stop(); // Make sure the user can *see* the assert error
        puts("Attempt to start again an active monitor.");
        abort();                                                  // ABORT
    }

    if (isNoop()) {
        return;                                                       // RETURN
    }

    fflush(d_monitoredStream);
    if (!isVerbose()) {
        if (-1 == dup2(fileno(d_nullStream), d_origFD)) {
            perrorAbort("StdIoTestMonitor Unable to duplicate "
                        "null file descriptor");
        }
    }
    d_active = true;
    safeSetBuf();
}

void StdIoTestMonitor::stop()
{
    if (!d_active) {
        puts("Attempt to stop an inactive monitor.");
        abort();                                                  // ABORT
    }
    if (!isNoop() && d_active) {
        stopImpl();
        d_active = false;
    }
}

// ACCESSORS
                        // Buffer Attributes
const char* StdIoTestMonitor::buffer() const
{
    return d_buffer.ptr();
}

int StdIoTestMonitor::bufferSize() const
{
    return d_buffer.size();
}

char StdIoTestMonitor::nulChar() const
{
    return d_buffer.nulChar();
}

                        // Status Information

bool StdIoTestMonitor::isActive() const
{
    return d_active;
}

                // monitor Characters' Information

bool StdIoTestMonitor::wasThereAnyOutput() const
{
    return d_buffer.wasThereAnyOutput();
}

                      // -------------------------
                      // class StdIoTestMonitorDuo
                      // -------------------------


// CREATORS
StdIoTestMonitorDuo::StdIoTestMonitorDuo(bool stdoutVerbose,
                                         bool stderrVerbose)
: d_stdoutMonitor(stdout, 128, stdoutVerbose)
, d_stderrMonitor(stderr, 128, stderrVerbose)
{}

StdIoTestMonitorDuo::StdIoTestMonitorDuo(bool verbosity)
: d_stdoutMonitor(stdout, 128, verbosity)
, d_stderrMonitor(stderr, 128, verbosity)
{}

// MANIPULATORS
                        // Monitoring Control
void StdIoTestMonitorDuo::start()
{
    d_stdoutMonitor.start();
    d_stderrMonitor.start();
}

void StdIoTestMonitorDuo::startStderr()
{
    d_stderrMonitor.start();
}

void StdIoTestMonitorDuo::startStdout()
{
    d_stdoutMonitor.start();
}

void StdIoTestMonitorDuo::stop()
{
    d_stdoutMonitor.stop();
    d_stderrMonitor.stop();
}

void StdIoTestMonitorDuo::stopStderr()
{
    d_stderrMonitor.stop();
}

void StdIoTestMonitorDuo::stopStdout()
{
    d_stdoutMonitor.stop();
}

                        // Buffer Control
const char *
StdIoTestMonitorDuo::monitorMessage(bool expectedOutputOnStdout,
                                    bool expectedOutputOnStderr) const
{
    const bool hadStdoutOutput = wasThereStdoutOutput();
    const bool hadStderrOutput = wasThereStderrOutput();

    enum {
        //:   -=[ 4 bits are describing the situation: 3210 ]=-
        //:
        //: 3 - wanted 'stdout' to print (N - No,     W - Yes    )
        //: 2 - 'stdout' has printed     (S - Silent, P - Printed)
        //: 1 - wanted 'stderr' to print (N - No,     W - Yes    )
        //: 0 - 'stderr' has printed     (S - Silent, P - Printed)
        //:
        //: Encoding of names: O[NW][SP]_E[NW][SP]

        ONS_ENS = 0x0,  // ____ -- GOOD
        ONS_ENP = 0x1,  // ___1 -- BAD  - stderr
        ONS_EWS = 0x2,  // __1_ -- BAD  - stderr
        ONS_EWP = 0x3,  // __11 -- GOOD
        ONP_ENS = 0x4,  // _1__ -- BAD  - stdout
        ONP_ENP = 0x5,  // _1_1 -- BAD  - both
        ONP_EWS = 0x6,  // _11_ -- BAD  - both
        ONP_EWP = 0x7,  // _111 -- BAD  - stdout
        OWS_ENS = 0x8,  // 1___ -- BAD  - stdout
        OWS_ENP = 0x9,  // 1__1 -- BAD  - both
        OWS_EWS = 0xA,  // 1_1_ -- BAD  - both
        OWS_EWP = 0xB,  // 1_11 -- BAD  - stdout
        OWP_ENS = 0xC,  // 11__ -- GOOD
        OWP_ENP = 0xD,  // 11_1 -- BAD  - stderr
        OWP_EWS = 0xE,  // 111_ -- BAD  - stderr
        OWP_EWP = 0xF   // 1111 -- GOOD
    };

    const int bits = expectedOutputOnStdout * 8 + hadStdoutOutput * 4 +
                     expectedOutputOnStderr * 2 + hadStderrOutput;

    switch (bits) {
      case ONS_ENS:
      case ONS_EWP:
      case OWP_ENS:
      case OWP_EWP:
        return 0;                                                     // RETURN

      case ONS_ENP:
      case OWP_ENP:
        return "Unexpected output on 'stderr'.";                      // RETURN

      case ONS_EWS:
      case OWP_EWS:
        return "No output on 'stderr', but expected.";                // RETURN

      case OWS_ENS:
      case OWS_EWP:
        return "No output on 'stdout', but expected.";                // RETURN

      case ONP_ENS:
      case ONP_EWP:
        return "Unexpected output on 'stdout'";                       // RETURN

      case ONP_ENP:
        return "Unexpected output on both 'stderr' and 'stdout'.";    // RETURN

      case ONP_EWS:
        return "Output on 'stdout' instead of 'stderr'.";             // RETURN

      case OWS_ENP:
        return "Output on 'stderr' instead of 'stdout'.";             // RETURN

      case OWS_EWS:
        return "No output on 'stdout' or 'stderr', both expected.";   // RETURN
    }

    return "** INTERNAL ERROR: monitorMessage";
}

int StdIoTestMonitorDuo::prepareStderrForHumans(int maxSize)
{
    return d_stderrMonitor.prepareBufferForHumans(maxSize);
}

int StdIoTestMonitorDuo::prepareStdoutForHumans(int maxSize)
{
    return d_stdoutMonitor.prepareBufferForHumans(maxSize);
}

void StdIoTestMonitorDuo::resetBuffers()
{
    d_stdoutMonitor.resetBuffer();
    d_stderrMonitor.resetBuffer();
}

char* StdIoTestMonitorDuo::stderrBuffer()
{
    return d_stderrMonitor.buffer();
}

int StdIoTestMonitorDuo::stderrBufferSize() const
{
    return d_stderrMonitor.bufferSize();
}

char *StdIoTestMonitorDuo::stdoutBuffer()
{
    return d_stdoutMonitor.buffer();
}

int StdIoTestMonitorDuo::stdoutBufferSize() const
{

    return d_stdoutMonitor.bufferSize();
}

// ACCESSORS
bool StdIoTestMonitorDuo::isStderrActive() const
{
    return d_stderrMonitor.isActive();
}

bool StdIoTestMonitorDuo::isStdoutActive() const
{
    return d_stdoutMonitor.isActive();
}

const char* StdIoTestMonitorDuo::stderrBuffer() const
{
    return d_stderrMonitor.buffer();
}

const char *StdIoTestMonitorDuo::stdoutBuffer() const
{
    return d_stdoutMonitor.buffer();
}

bool StdIoTestMonitorDuo::wasThereStderrOutput() const
{
    return d_stderrMonitor.wasThereAnyOutput();
}

bool StdIoTestMonitorDuo::wasThereStdoutOutput() const
{
    return d_stdoutMonitor.wasThereAnyOutput();
}

                        // -----------------------
                        // class AssertTestMonitor
                        // -----------------------

// PRIVATE MANIPULATORS
void AssertTestMonitor::printIfErrorAndAssert()
{
    const char *message =
                      d_monitors.monitorMessage(d_expectations.stdoutPrints(),
                                                d_expectations.stderrPrints());

    if (message) {
        d_debugInfoForAssert.print();
        puts("");

        static const int k_MAX_CHARS = 128;
        if (d_monitors.wasThereStdoutOutput())
        {
            const int len = d_monitors.prepareStdoutForHumans(k_MAX_CHARS);
            printf("stdout [%.*s]\n",
                    len, d_monitors.stdoutBuffer());
        }
        if (d_monitors.wasThereStderrOutput())
        {
            const int len = d_monitors.prepareStderrForHumans(k_MAX_CHARS);
            printf("stderr [%.*s]\n",
                    len, d_monitors.stderrBuffer());
        }

        const int line = d_debugInfoForAssert.isSet()
                                        ? d_debugInfoForAssert.callLine()
                                        : __LINE__;
        aSsErT(true, message, line);
        puts("");
    }
}

// CREATORS
AssertTestMonitor::AssertTestMonitor(bool verbosity)
: d_monitors(verbosity, verbosity)
{
}

// MANIPULATORS
void AssertTestMonitor::start()
{
    d_monitors.startStdout();
    d_monitors.startStderr();
}

void AssertTestMonitor::stop()
{
    if (d_monitors.isStdoutActive()) d_monitors.stopStdout();
    if (d_monitors.isStderrActive()) d_monitors.stopStderr();

    printIfErrorAndAssert();

    d_debugInfoForAssert.reset();
    d_monitors.resetBuffers();
    d_expectations.reset();
}

// ACCESSORS
bool AssertTestMonitor::wouldBeExtraOutput() const
{
    return wouldBeExtraStdout() || wouldBeExtraStderr();
}

inline
bool AssertTestMonitor::wouldBeExtraStderr() const
{
    return !d_expectations.stderrPrints()      // Expected silence on 'stderr',
        && d_monitors.isStderrActive()         // it was monitored,
        && d_monitors.wasThereStderrOutput();  // but it wasn't silent.
}

inline
bool AssertTestMonitor::wouldBeExtraStdout() const
{
    return !d_expectations.stdoutPrints()      // Expected silence on 'stdout',
        && d_monitors.isStdoutActive()         // it was monitored,
        && d_monitors.wasThereStdoutOutput();  // but it wasn't silent.
}

bool AssertTestMonitor::wouldFail() const
{
    return wouldOutputBeMissing() || wouldBeExtraOutput();
}

bool AssertTestMonitor::wouldOutputBeMissing() const
{
    return wouldStdoutBeMissing() || wouldStderrBeMissing();
}

bool AssertTestMonitor::wouldStderrBeMissing() const
{
    return d_expectations.stderrPrints()        // Expected output on 'stderr',
        && d_monitors.isStderrActive()          // it was monitored,
        && !d_monitors.wasThereStderrOutput();  // but it was actually silent.
}

bool AssertTestMonitor::wouldStdoutBeMissing() const
{
    return d_expectations.stdoutPrints()        // Expected output on 'stdout',
        && d_monitors.isStdoutActive()          // it was monitored,
        && !d_monitors.wasThereStdoutOutput();  // but it was actually silent.
}

// ----------------------------------------------------------------------------

void TestMacroBSLS_ASSERTTEST_IS_ACTIVE()
{
//    bsls::Assert::setViolationHandler(&AssertFailed::failMacroTest);

//  Config macros    Configuration  Expected results
//  OVERRIDE SAFE2  OPT  DBG  SAFE   O A S O2 A2 S2
//  -------- -----  ---  ---  ----   - - - -- -- --
//  _NONE       X
//  _OPT        X    X               X      X
//  _DEBUG      X    X    X          X X    X  X
//  _SAFE       X    X    X    X     X X X  X  X  X
//  _NONE
//  _OPT             X               X
//  _DEBUG           X    X          X X
//  _SAFE            X    X    X     X X X

//============================ SAFE_2 LEVEL_NONE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG);

    {
        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O", "O2"
            bool        d_expectedResult;   // assertion: 'P'ass or 'F'ail
        } DATA[] = {
            //LINE  TYPE  RESULT
            //----  ----  ------
            { L_,   "O",  false },
            { L_,   "O2", false },
            { L_,   "A",  false },
            { L_,   "A2", false },
            { L_,   "S",  false },
            { L_,   "S2", false },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNumber;
            const char *const TYPE   = DATA[ti].d_assertBuildType;
            bool              RESULT = DATA[ti].d_expectedResult;

            if (veryVerbose) {
                T_ P_(TYPE) P(RESULT)
            }

            // Validate test description.
            LOOP_ASSERT(LINE, bsls::AssertTest::isValidAssertBuild(TYPE));

            bool configResult = BSLS_ASSERTTEST_IS_ACTIVE(TYPE);
            ASSERTV(TYPE, RESULT, configResult, RESULT == configResult);

        }  // table-driven 'for' loop
    }

//========================= SAFE_2 LEVEL_ASSERT_OPT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG);


    {
        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O", "O2"
            bool        d_expectedResult;   // assertion: 'P'ass or 'F'ail
        } DATA[] = {
            //LINE  TYPE  RESULT
            //----  ----  ------
            { L_,   "O",  true  },
            { L_,   "O2", true  },
            { L_,   "A",  false },
            { L_,   "A2", false },
            { L_,   "S",  false },
            { L_,   "S2", false },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNumber;
            const char *const TYPE   = DATA[ti].d_assertBuildType;
            bool              RESULT = DATA[ti].d_expectedResult;

            if (veryVerbose) {
                T_ P_(TYPE) P(RESULT)
            }

            // Validate test description.
            LOOP_ASSERT(LINE, bsls::AssertTest::isValidAssertBuild(TYPE));

            bool configResult = BSLS_ASSERTTEST_IS_ACTIVE(TYPE);
            ASSERTV(TYPE, RESULT, configResult, RESULT == configResult);

        }  // table-driven 'for' loop
    }

//=========================== SAFE_2 LEVEL_ASSERT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG);

    {
        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O", "O2"
            bool        d_expectedResult;   // assertion: 'P'ass or 'F'ail
        } DATA[] = {
            //LINE  TYPE  RESULT
            //----  ----  ------
            { L_,   "O",  true  },
            { L_,   "O2", true  },
            { L_,   "A",  true  },
            { L_,   "A2", true  },
            { L_,   "S",  false },
            { L_,   "S2", false },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNumber;
            const char *const TYPE   = DATA[ti].d_assertBuildType;
            bool              RESULT = DATA[ti].d_expectedResult;

            if (veryVerbose) {
                T_ P_(TYPE) P(RESULT)
            }

            // Validate test description.
            LOOP_ASSERT(LINE, bsls::AssertTest::isValidAssertBuild(TYPE));

            bool configResult = BSLS_ASSERTTEST_IS_ACTIVE(TYPE);
            ASSERTV(TYPE, RESULT, configResult, RESULT == configResult);

        }  // table-driven 'for' loop
    }

//======================== SAFE_2 LEVEL_ASSERT_SAFE =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG);

    {
        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O", "O2"
            bool        d_expectedResult;   // assertion: 'P'ass or 'F'ail
        } DATA[] = {
            //LINE  TYPE  RESULT
            //----  ----  ------
            { L_,   "O",  true  },
            { L_,   "O2", true  },
            { L_,   "A",  true  },
            { L_,   "A2", true  },
            { L_,   "S",  true  },
            { L_,   "S2", true  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNumber;
            const char *const TYPE   = DATA[ti].d_assertBuildType;
            bool              RESULT = DATA[ti].d_expectedResult;

            if (veryVerbose) {
                T_ P_(TYPE) P(RESULT)
            }

            // Validate test description.
            LOOP_ASSERT(LINE, bsls::AssertTest::isValidAssertBuild(TYPE));

            bool configResult = BSLS_ASSERTTEST_IS_ACTIVE(TYPE);
            ASSERTV(TYPE, RESULT, configResult, RESULT == configResult);

        }  // table-driven 'for' loop
    }

//=============================== LEVEL_NONE ================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG);

    {
        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O", "O2"
            bool        d_expectedResult;   // assertion: 'P'ass or 'F'ail
        } DATA[] = {
            //LINE  TYPE  RESULT
            //----  ----  ------
            { L_,   "O",  false },
            { L_,   "O2", false },
            { L_,   "A",  false },
            { L_,   "A2", false },
            { L_,   "S",  false },
            { L_,   "S2", false },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNumber;
            const char *const TYPE   = DATA[ti].d_assertBuildType;
            bool              RESULT = DATA[ti].d_expectedResult;

            if (veryVerbose) {
                T_ P_(TYPE) P(RESULT)
            }

            // Validate test description.
            LOOP_ASSERT(LINE, bsls::AssertTest::isValidAssertBuild(TYPE));

            bool configResult = BSLS_ASSERTTEST_IS_ACTIVE(TYPE);
            ASSERTV(TYPE, RESULT, configResult, RESULT == configResult);

        }  // table-driven 'for' loop
    }

//============================ LEVEL_ASSERT_OPT =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG);

    {
        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O", "O2"
            bool        d_expectedResult;   // assertion: 'P'ass or 'F'ail
        } DATA[] = {
            //LINE  TYPE  RESULT
            //----  ----  ------
            { L_,   "O",  true  },
            { L_,   "O2", false },
            { L_,   "A",  false },
            { L_,   "A2", false },
            { L_,   "S",  false },
            { L_,   "S2", false },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNumber;
            const char *const TYPE   = DATA[ti].d_assertBuildType;
            bool              RESULT = DATA[ti].d_expectedResult;

            if (veryVerbose) {
                T_ P_(TYPE) P(RESULT)
            }

            // Validate test description.
            LOOP_ASSERT(LINE, bsls::AssertTest::isValidAssertBuild(TYPE));

            bool configResult = BSLS_ASSERTTEST_IS_ACTIVE(TYPE);
            ASSERTV(TYPE, RESULT, configResult, RESULT == configResult);

        }  // table-driven 'for' loop
    }

//============================== LEVEL_ASSERT ===============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG);

    {
        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O", "O2"
            bool        d_expectedResult;   // assertion: 'P'ass or 'F'ail
        } DATA[] = {
            //LINE  TYPE  RESULT
            //----  ----  ------
            { L_,   "O",  true  },
            { L_,   "O2", false },
            { L_,   "A",  true  },
            { L_,   "A2", false },
            { L_,   "S",  false },
            { L_,   "S2", false },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNumber;
            const char *const TYPE   = DATA[ti].d_assertBuildType;
            bool              RESULT = DATA[ti].d_expectedResult;

            if (veryVerbose) {
                T_ P_(TYPE) P(RESULT)
            }

            // Validate test description.
            LOOP_ASSERT(LINE, bsls::AssertTest::isValidAssertBuild(TYPE));

            bool configResult = BSLS_ASSERTTEST_IS_ACTIVE(TYPE);
            ASSERTV(TYPE, RESULT, configResult, RESULT == configResult);

        }  // table-driven 'for' loop
    }

//============================ LEVEL_ASSERT_SAFE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG);
    ASSERT(BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG);
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG);

    {
        static const struct {
            int         d_lineNumber;       // line # of the row in this table
            const char *d_assertBuildType;  // "S", "S2", "A", "A2", "O", "O2"
            bool        d_expectedResult;   // assertion: 'P'ass or 'F'ail
        } DATA[] = {
            //LINE  TYPE  RESULT
            //----  ----  ------
            { L_,   "O",  true  },
            { L_,   "O2", false },
            { L_,   "A",  true  },
            { L_,   "A2", false },
            { L_,   "S",  true  },
            { L_,   "S2", false },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNumber;
            const char *const TYPE   = DATA[ti].d_assertBuildType;
            bool              RESULT = DATA[ti].d_expectedResult;

            if (veryVerbose) {
                T_ P_(TYPE) P(RESULT)
            }

            // Validate test description.
            LOOP_ASSERT(LINE, bsls::AssertTest::isValidAssertBuild(TYPE));

            bool configResult = BSLS_ASSERTTEST_IS_ACTIVE(TYPE);
            ASSERTV(TYPE, RESULT, configResult, RESULT == configResult);

        }  // table-driven 'for' loop
    }
//---------------------------------------------------------------------------//
}

// These macros provide an easy way of switching test filenames for the tests
// that rely on the value of '__FILE__'.  Note that the macro
// 'BSLS_ASSERTTEST_THIS_FILENAME' should have the same value as the string
// literal returned by '__FILE__' before any '#line' directives are processed.
#define BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME \
    BSLS_MACROINCREMENT(__LINE__) "bsls_fictionaltestcomponent.t.cpp"
#define BSLS_ASSERTTEST_RESET_THIS_FILENAME \
    BSLS_MACROINCREMENT(__LINE__) "bsls_asserttest.t.cpp"

//--------------------------------------------------------------------GENERATOR
// The following Python script generates various macro test cases.  It requires
// a single parameter, and supports 3 potential arguments for that parameter:
// "PASS_OR_FAIL", "PASS_OR_FAIL_RAW", and "CHECK_LEVEL" to generate the bodies
// of distinct test case functions.
//..
//  #!/usr/bin/env python
//
//  # bsls_asserttest_gen.py
//
//  import sys, re
//
//  try:
//      import textwrap
//
//      textwrap.indent
//  except AttributeError:  # undefined function (appeared in Python 3.3)
//
//      def indent(text, amount, char=" "):
//          padding = amount * char
//          return "".join(padding + line for line in text.splitlines(True))
//
//  else:
//
//      def indent(text, amount, char=" "):
//          return textwrap.indent(text, amount * char)
//
//
//  testid = sys.argv[1]
//
//
//  control_table = """
//   Config macros   Expected results
//  OVERRIDE SAFE2  O A S I O2 A2 S2 I2
//  -------- -----  - - - - -- -- -- --
//  _NONE       X         X          X
//  _OPT        X   X     X X        X
//  _DEBUG      X   X X   X X  X     X
//  _SAFE       X   X X X X X  X  X  X
//  _NONE                 X
//  _OPT            X     X
//  _DEBUG          X X   X
//  _SAFE           X X X X
//  """
//
//
//  def echo(text, no_nl=False):
//      if testid == "PASS_OR_FAIL_RAW":
//          text = re.sub(r"(_PASS|_FAIL)", r"\1_RAW", text)
//      kwargs = {}
//      if no_nl:
//          kwargs["end"] = ""
//      print(text, **kwargs)
//
//
//  def printTitle(t):
//      e1 = (73 - len(t)) // 2
//      e2 = 73 - len(t) - e1
//      t1 = "//%s %s %s//" % (
//          "=" * e1,
//          t,
//          "=" * e2,
//      )
//      echo(t1)
//
//
//  def printCheckDef(macroname, isactive):
//      if isactive:
//          echo(
//              """
//  #if !defined(%s)
//  #error %s should be defined
//  SunStudioOnlyPrintsWarningsForPoundError;
//  #endif"""
//              % (
//                  macroname,
//                  macroname,
//              )
//          )
//      else:
//          echo(
//              """
//  #if defined(%s)
//  #error %s should not be defined
//  SunStudioOnlyPrintsWarningsForPoundError;
//  #endif"""
//              % (
//                  macroname,
//                  macroname,
//              )
//          )
//
//
//  def printCheckVal(macroname, isactive):
//      if isactive:
//          echo("    ASSERT(%s)" % (macroname,))
//      else:
//          echo("    ASSERT(!%s)" % (macroname,))
//
//
//  def printChecks(expected, badfile, results):
//      # expected - whether we expect the test macro to detect an error or not
//      # badexec - whether there is a filename mismatch
//      # results - list the 8 characters from the expected results columns of
//      #           the control table
//
//      # lists of (prefix, suffix, expression, earg)
//      checks = []
//      levelchecks = []
//      filechecks = []
//
//      prefixes = {
//          -1: "BSLS_ASSERTTEST_ASSERT_INVOKE",
//          0: "BSLS_ASSERTTEST_ASSERT_OPT",
//          1: "BSLS_ASSERTTEST_ASSERT",
//          2: "BSLS_ASSERTTEST_ASSERT_SAFE",
//      }
//
//      exprs = [
//          ("Production::callInvoke", -1, 3),
//          ("Production::callOpt", 0, 0),
//          ("Production::callAssert", 1, 1),
//          ("Production::callSafe", 2, 2),
//          ("Safe2::callInvoke", -1, 7),
//          ("Safe2::callOpt", 0, 4),
//          ("Safe2::callAssert", 1, 5),
//          ("Safe2::callSafe", 2, 6),
//      ]
//
//      eargs = {
//          True: "true",
//          False: "false",
//      }
//
//      suffixes = {
//          True: "PASS",
//          False: "FAIL",
//      }
//
//      stdio_expectations = {
//          # The index into this array is calculated as:
//          #    `(badfile and not suffix) or (not badfile and not expected)
//          False: "SILENCE",
//          True: "OUTPUT_ON_STDOUT_IF_TARGET_EXC",
//      }
//
//      for checklevel in range(-1, 2):
//          # checkenabled = checklevel < 0 or results[checklevel] == "X"
//          if checklevel >= 0 and results[checklevel] != "X":
//              continue
//
//          prefix = prefixes[checklevel]
//
//          for expr, exprlevel, exprindex in exprs:
//              for earg in [True, False]:
//                  for suffix in [True, False]:
//                      exprlist = None
//
//                      exprfails = not earg and results[exprindex] == "X"
//
//                      if not exprfails:
//                          if expected != suffix:
//                              continue
//                          exprlist = checks
//                      elif expected:
//                          if (
//                              testid == "CHECK_LEVEL"
//                              and exprlevel < checklevel
//                          ):
//                              continue
//                          if badfile:
//                              continue
//                          if not suffix:
//                              exprlist = checks
//                      else:
//                          if (
//                              testid == "CHECK_LEVEL"
//                              and exprlevel < checklevel
//                          ):
//                              exprlist = levelchecks
//                          elif badfile:
//                              exprlist = filechecks
//                          elif suffix:
//                              exprlist = checks
//
//                      if exprlist != None:
//                          if len(exprlist) > 0 and exprlist[-1][1] != prefix:
//                              exprlist.append(None)  ## makes an empty line
//
//                          # Compute if we have stdio output
//                          not_filecheck = filechecks is not exprlist
//                          plain_not_expected = not_filecheck and not expected
//                          filecheck_passing = not not_filecheck and suffix
//                          stdout_output = (
//                              plain_not_expected or filecheck_passing
//                          )
//
//                          exprlist.append(
//                              (
//                                  stdio_expectations[stdout_output],
//                                  prefix,
//                                  suffixes[suffix],
//                                  expr,
//                                  eargs[earg],
//                              )
//                          )
//
//      def printexprdetails(exprdetails, need_ws):
//          """print expression details without a newline or an empty line
//
//          Args:
//            exprdetails (tuple or None): the variable parts of the line
//            need_ws (bool): The previous 'printexprdetails_' call did not
//                            print a newline.  This call needs to print either
//                            a space or a newline before printing anything
//                          else.
//            returns (bool): was the closing newline printed.
//          """
//
//          if not exprdetails:
//              if need_ws:
//                  echo("")  # close previous printout
//
//              echo("")  # print an empty line
//              return True
//          else:
//              sep = " " if need_ws else "        "
//              allargs = (sep,) + exprdetails
//              echo(
//                  """%sEXPECT_%s;
//          %s_%s(%s(%s));
//          ASSERT_STDIO;"""
//                  % allargs,
//                  no_nl=True,
//              )
//              return False
//
//          #    echo("""%sEXPECT_%s;
//          #%s_%s(%s(%s));
//          # ASSERT_STDIO;""" % values)
//
//      def printexprlist(exprlist):
//          need_ws = False
//          for exprdetails in exprlist:
//              need_ws = not printexprdetails(exprdetails, need_ws)
//          if need_ws:
//              echo("")
//
//      if checks:
//          echo("")
//          if expected:
//              echo("        // The following tests will call ASSERT(true)")
//          else:
//              echo("        // The following tests will call ASSERT(false)")
//          echo("")
//          printexprlist(checks)
//
//      if levelchecks:
//          echo("")
//          if testid != "CHECK_LEVEL":
//              echo("#ifdef BSLS_ASSERTTEST_CAN_CHECK_LEVELS")
//          echo("        // These tests will fail with the wrong level")
//          printexprlist(levelchecks)
//          if testid != "CHECK_LEVEL":
//              echo("#endif // BSLS_ASSERTTEST_CAN_CHECK_LEVELS")
//
//      if filechecks:
//          echo(
//              """
//          // The following checks will fail with the wrong component")
//  """
//          )
//          printexprlist(filechecks)
//
//      if not checks and not levelchecks and not filechecks:
//          echo("        // No checks in this mode to verify")
//
//
//  def printLocalClasses():
//      echo(
//          """    {
//          struct Production {
//              static void callOpt(bool pass) {
//  #if defined(BDE_BUILD_TARGET_EXC)
//                  BSLS_ASSERT_OPT(pass);
//                  (void)pass;
//  #else
//                  if (pass) {
//                      BSLS_ASSERT_OPT(pass);
//                  }
//  #endif
//              }
//
//              static void callAssert(bool pass) {
//  #if defined(BDE_BUILD_TARGET_EXC)
//                  BSLS_ASSERT(pass);
//                  (void)pass;
//  #else
//                  if (pass) {
//                      BSLS_ASSERT(pass);
//                  }
//  #endif
//              }
//
//              static void callSafe(bool pass) {
//  #if defined(BDE_BUILD_TARGET_EXC)
//                  BSLS_ASSERT_SAFE(pass);
//                  (void)pass;
//  #else
//                  if (pass) {
//                      BSLS_ASSERT_SAFE(pass);
//                  }
//  #endif
//              }
//
//              static void callInvoke(bool pass) {
//  #if defined(BDE_BUILD_TARGET_EXC)
//                  if (!pass) {
//                      BSLS_ASSERT_INVOKE("!pass");
//                  }
//  #else
//                  (void)pass;
//  #endif
//              }
//          };
//
//          struct Safe2 {
//              static void callOpt(bool pass) {
//  #if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
//                  BSLS_ASSERT_OPT(pass);
//                  (void)pass;
//  #else
//                  if (pass) {
//                      BSLS_ASSERT_OPT(pass);
//                  }
//  #endif
//              }
//
//              static void callAssert(bool pass) {
//  #if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
//                  BSLS_ASSERT(pass);
//                  (void)pass;
//  #else
//                  if (pass) {
//                      BSLS_ASSERT(pass);
//                  }
//  #endif
//              }
//
//              static void callSafe(bool pass) {
//  #if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
//                  BSLS_ASSERT_SAFE(pass);
//                  (void)pass;
//  #else
//                  if (pass) {
//                      BSLS_ASSERT_SAFE(pass);
//                  }
//  #endif
//              }
//
//              static void callInvoke(bool pass) {
//  #if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
//                  if (!pass) {
//                      BSLS_ASSERT_INVOKE("!pass");
//                  }
//  #else
//                  (void)pass;
//  #endif
//              }
//          };"""
//      )
//
//
//  def printTestPassFail(flags, results):
//      printTitle(" ".join(flags))
//      echo(
//          """
//  // [1] Reset all configuration macros
//
//  #undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
//  #include <bsls_asserttest_macroreset.h>
//
//  // [2] Define the macros for this test case
//  """
//      )
//      for flag in flags:
//          if flag == "SAFE_2":
//              echo("#define BDE_BUILD_TARGET_SAFE_2")
//          else:
//              echo("#define BSLS_ASSERT_%s" % (flag,))
//      if testid == "CHECK_LEVEL":
//          echo("#define BSLS_ASSERTTEST_CHECK_LEVEL")
//      echo(
//          """
//  // [3] Re-include the 'bsls_asserttest.h' header
//
//  #include <bsls_asserttest.h>
//
//  // [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//  //     'IS_ACTIVE' macros and their values we intend to test."""
//      )
//      printCheckDef("BDE_BUILD_TARGET_SAFE_2", "SAFE_2" in flags)
//      printCheckDef("BSLS_ASSERT_OPT_IS_ACTIVE", results[0] == "X")
//      printCheckDef("BSLS_ASSERT_IS_ACTIVE", results[1] == "X")
//      printCheckDef("BSLS_ASSERT_SAFE_IS_ACTIVE", results[2] == "X")
//
//      echo(
//          """
//  // [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
//  // defined."""
//      )
//      printCheckVal("BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG", "SAFE_2" in flags)
//      printCheckVal(
//          "BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG", results[0] == "X"
//      )
//      printCheckVal("BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG", results[1] == "X")
//      printCheckVal(
//          "BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG", results[2] == "X"
//      )
//      printCheckVal(
//          "BSLS_ASSERTTEST_CHECK_LEVEL_ARG", testid == "CHECK_LEVEL"
//      )
//
//      echo(
//          """
//  // [6] Define a local struct 'Production' which makes use of the current
//  //     definitions of the 'BSLS_ASSERT' macros."""
//      )
//      printLocalClasses()
//      echo(
//          """
//  // [7] Verify that the *PASS macros are expanded in any build mode.
//          {
//              EXPECTED = true;
//
//              EXPECT_SILENCE;
//              BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
//              ASSERT_STDIO;  EXPECT_SILENCE;
//              BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
//              ASSERT_STDIO;  EXPECT_SILENCE;
//              BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
//              ASSERT_STDIO;  EXPECT_SILENCE;
//              BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(ASSERT(true));
//              ASSERT_STDIO;
//          }
//
//  // [8] Verify the macros that should not expand in this build mode
//          {
//              EXPECTED = false;
//  """
//      )
//      built_text = ""
//      if results[0] != "X":
//          built_text += """EXPECT_SILENCE;
//  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(ASSERT(true));
//  ASSERT_STDIO; """
//      if results[1] != "X":
//          built_text += """EXPECT_SILENCE;
//  BSLS_ASSERTTEST_ASSERT_FAIL(ASSERT(true));
//  ASSERT_STDIO; """
//      if results[2] != "X":
//          built_text += """EXPECT_SILENCE;
//  BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
//  ASSERT_STDIO;"""
//      echo(indent(built_text.rstrip(), 12))
//      del built_text
//      echo(
//          """        }
//
//  // [9] Test for expressions that should fail
//
//          EXPECTED = false;"""
//      )
//      printChecks(False, False, results)
//
//      echo(
//          """
//  // [10] Test for expressions that should pass
//
//          EXPECTED = true;"""
//      )
//      printChecks(True, False, results)
//
//      echo("""    }
//
//  #line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
//  // [11] Define a local struct 'Production' which makes use of
//  //     the current definitions of the 'BSLS_ASSERT' macros with an
//  //     alternate filename in place."""
//      )
//      printLocalClasses()
//      echo(
//          """
//          // Restore filename before evaluating the test macros
//  #line BSLS_ASSERTTEST_RESET_THIS_FILENAME
//
//  // [12] Test for expressions that should fail
//
//          EXPECTED = false;"""
//      )
//      printChecks(False, testid != "PASS_OR_FAIL_RAW", results)
//
//      echo(
//          """
//  // [13] Test for expressions that should pass
//
//          EXPECTED = true;"""
//      )
//      printChecks(True, testid != "PASS_OR_FAIL_RAW", results)
//
//      echo(
//          """    }
//
//      // Restore status-quo before starting the next test
//      EXPECTED = true;"""
//      )
//
//
//  levelflags = {
//      "_NONE": "LEVEL_NONE",
//      "_OPT": "LEVEL_ASSERT_OPT",
//      "_DEBUG": "LEVEL_ASSERT",
//      "_SAFE": "LEVEL_ASSERT_SAFE",
//  }
//
//  print(f"//{'-' * (66 - len(testid))}GENERATED: {sys.argv[1]}")
//
//  for n, line in enumerate(control_table.split("\n")):
//      if n <= 3 or not line.strip():
//          continue
//
//      flags = []
//      if line[12] == "X":
//          flags.append("SAFE_2")
//      flags.append(levelflags[line[0:7].rstrip()])
//
//      line = line + (" " * max(0, 34 - len(line)))
//      printTestPassFail(
//          flags,
//          [
//              line[16],  # Production::callOpt
//              line[18],  # Production::callAssert
//              line[20],  # Production::callSafe
//              line[22],  # Production::callInvoke
//              line[24],  # Safe2::callOpt
//              line[27],  # Safe2::callAssert
//              line[30],  # Safe2::callSafe
//              line[33],  # Safe2::callInvoke
//          ],
//      )
//  print(f"//{'-' * (62 - len(testid))}END GENERATED: {sys.argv[1]}")
//  print(f"//{'-' * 75}//")
//..
//----------------------------------------------------------------END GENERATOR

// We will be testing 8 macros, with a further test to handle the 8 RAW macros
//   BSLS_ASSERTTEST_ASSERT_SAFE_PASS(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_PASS(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_FAIL(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_OPT_PASS(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_OPT_FAIL(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(FUNCTION)

// The Requirement to make each call is that a macro called 'ASSERT' is defined
// to flag errors, if necessary, that are signaled by the 'Probe's.
//
// Concerns:
//   The necessary 'ASSERT' macro correctly diagnoses pass/fail conditions
//   Each of the tested macros is available only in the supported build modes
//   Each tested macro correctly identifies pass/fail conditions
//
// Test plan:
//   Set up each possible build configuration in the tried-and-tested fashion.
//   Install a new 'ASSERT' macro that will count expected fails, rather than
//   the default one from this test driver, that flags all fails as unexpected.

void TestMacroBSLS_ASSERTTEST_PASS_OR_FAIL(AssertTestMonitor *pMonitor)
{
    AssertTestMonitor& monitor = *pMonitor;

    bsls::Assert::setViolationHandler(&bsls::AssertTest::failTestDriver);

// Install the local ASSERT macro, that allows us to detect assert-fail test
// cases.
#undef ASSERT
#define ASSERT(X) { aSsErT((!(X)) == EXPECTED, #X, __LINE__); }

    bool EXPECTED = false;
    ASSERT(false);
    EXPECTED = true;
    ASSERT(true);

    // Note that we must leave each test block with EXPECTED = true
//
//------------------------------------------------------GENERATED: PASS_OR_FAIL
//============================ SAFE_2 LEVEL_NONE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(ASSERT(true));
            ASSERT_STDIO; EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_FAIL(ASSERT(true));
            ASSERT_STDIO; EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
            ASSERT_STDIO;
        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // The following checks will fail with the wrong component")

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//========================= SAFE_2 LEVEL_ASSERT_OPT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_FAIL(ASSERT(true));
            ASSERT_STDIO; EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
            ASSERT_STDIO;
        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // The following checks will fail with the wrong component")

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//=========================== SAFE_2 LEVEL_ASSERT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
            ASSERT_STDIO;
        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // The following checks will fail with the wrong component")

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//======================== SAFE_2 LEVEL_ASSERT_SAFE =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;


        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO;

        // The following checks will fail with the wrong component")

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//=============================== LEVEL_NONE ================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(ASSERT(true));
            ASSERT_STDIO; EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_FAIL(ASSERT(true));
            ASSERT_STDIO; EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
            ASSERT_STDIO;
        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // The following checks will fail with the wrong component")

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//============================ LEVEL_ASSERT_OPT =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_FAIL(ASSERT(true));
            ASSERT_STDIO; EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
            ASSERT_STDIO;
        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // The following checks will fail with the wrong component")

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//============================== LEVEL_ASSERT ===============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
            ASSERT_STDIO;
        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // The following checks will fail with the wrong component")

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//============================ LEVEL_ASSERT_SAFE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;


        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // The following checks will fail with the wrong component")

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//--------------------------------------------------END GENERATED: PASS_OR_FAIL
//---------------------------------------------------------------------------//

// Restore the default 'ASSERT' macro for this test driver.
#undef ASSERT
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
}

// We will be testing 8 RAW macros
//   BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_PASS_RAW(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_FAIL_RAW(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(FUNCTION)
//   BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(FUNCTION)

void TestMacroBSLS_ASSERTTEST_PASS_OR_FAIL_RAW(AssertTestMonitor *pMonitor)
{
    AssertTestMonitor& monitor = *pMonitor;

    bsls::Assert::setViolationHandler(&bsls::AssertTest::failTestDriver);

// Install the local ASSERT macro, that allows us to detect assert-fail test
// cases.
#undef ASSERT
#define ASSERT(X) { aSsErT((!(X)) == EXPECTED, #X, __LINE__); }

    bool EXPECTED = false;
    ASSERT(false);
    EXPECTED = true;
    ASSERT(true);

// Note that we must leave each test block with EXPECTED = true
//
//--------------------------------------------------GENERATED: PASS_OR_FAIL_RAW
//============================ SAFE_2 LEVEL_NONE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(ASSERT(true));
            ASSERT_STDIO; EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_FAIL_RAW(ASSERT(true));
            ASSERT_STDIO; EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(ASSERT(true));
            ASSERT_STDIO;
        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//========================= SAFE_2 LEVEL_ASSERT_OPT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_FAIL_RAW(ASSERT(true));
            ASSERT_STDIO; EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(ASSERT(true));
            ASSERT_STDIO;
        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//=========================== SAFE_2 LEVEL_ASSERT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(ASSERT(true));
            ASSERT_STDIO;
        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//======================== SAFE_2 LEVEL_ASSERT_SAFE =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;


        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//=============================== LEVEL_NONE ================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(ASSERT(true));
            ASSERT_STDIO; EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_FAIL_RAW(ASSERT(true));
            ASSERT_STDIO; EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(ASSERT(true));
            ASSERT_STDIO;
        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//============================ LEVEL_ASSERT_OPT =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_FAIL_RAW(ASSERT(true));
            ASSERT_STDIO; EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(ASSERT(true));
            ASSERT_STDIO;
        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//============================== LEVEL_ASSERT ===============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(ASSERT(true));
            ASSERT_STDIO;
        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//============================ LEVEL_ASSERT_SAFE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;


        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL_RAW(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS_RAW(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//----------------------------------------------END GENERATED: PASS_OR_FAIL_RAW
//---------------------------------------------------------------------------//
// Restore the default 'ASSERT' macro for this test driver.
#undef ASSERT
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

}

// We will be testing the check level configuration macro
//    BSLS_ASSERTTEST_CHECK_LEVEL

void TestMacroBSLS_ASSERTTEST_CHECK_LEVEL(AssertTestMonitor *pMonitor)
{
    AssertTestMonitor& monitor = *pMonitor;

    bsls::Assert::setViolationHandler(&bsls::AssertTest::failTestDriver);

#ifndef BSLS_ASSERTTEST_CAN_CHECK_LEVELS
    // Language-level contracts might not support properly checking levels and
    // in such cases this test does nothing useful.
    return;                                                           // RETURN
#endif

// Install the local ASSERT macro, that allows us to detect assert-fail test
// cases.
#undef ASSERT
#define ASSERT(X) { aSsErT((!(X)) == EXPECTED, #X, __LINE__); }

    bool EXPECTED = false;
    ASSERT(false);
    EXPECTED = true;
    ASSERT(true);

//-------------------------------------------------------GENERATED: CHECK_LEVEL
//============================ SAFE_2 LEVEL_NONE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_NONE
#define BSLS_ASSERTTEST_CHECK_LEVEL

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(ASSERT(true));
            ASSERT_STDIO; EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_FAIL(ASSERT(true));
            ASSERT_STDIO; EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
            ASSERT_STDIO;
        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // The following checks will fail with the wrong component")

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//========================= SAFE_2 LEVEL_ASSERT_OPT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_OPT
#define BSLS_ASSERTTEST_CHECK_LEVEL

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_FAIL(ASSERT(true));
            ASSERT_STDIO; EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
            ASSERT_STDIO;
        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // These tests will fail with the wrong level
        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // These tests will fail with the wrong level
        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO;

        // The following checks will fail with the wrong component")

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//=========================== SAFE_2 LEVEL_ASSERT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT
#define BSLS_ASSERTTEST_CHECK_LEVEL

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
            ASSERT_STDIO;
        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // These tests will fail with the wrong level
        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // These tests will fail with the wrong level
        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO;

        // The following checks will fail with the wrong component")

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//======================== SAFE_2 LEVEL_ASSERT_SAFE =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE
#define BSLS_ASSERTTEST_CHECK_LEVEL

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if !defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;


        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        // These tests will fail with the wrong level
        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO;

        // These tests will fail with the wrong level
        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO;

        // The following checks will fail with the wrong component")

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//=============================== LEVEL_NONE ================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BSLS_ASSERT_LEVEL_NONE
#define BSLS_ASSERTTEST_CHECK_LEVEL

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(ASSERT(true));
            ASSERT_STDIO; EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_FAIL(ASSERT(true));
            ASSERT_STDIO; EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
            ASSERT_STDIO;
        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // The following checks will fail with the wrong component")

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//============================ LEVEL_ASSERT_OPT =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BSLS_ASSERT_LEVEL_ASSERT_OPT
#define BSLS_ASSERTTEST_CHECK_LEVEL

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_FAIL(ASSERT(true));
            ASSERT_STDIO; EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
            ASSERT_STDIO;
        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // These tests will fail with the wrong level
        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // These tests will fail with the wrong level
        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO;

        // The following checks will fail with the wrong component")

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//============================== LEVEL_ASSERT ===============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BSLS_ASSERT_LEVEL_ASSERT
#define BSLS_ASSERTTEST_CHECK_LEVEL

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(!BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(ASSERT(true));
            ASSERT_STDIO;
        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // These tests will fail with the wrong level
        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // These tests will fail with the wrong level
        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(false));
        ASSERT_STDIO;

        // The following checks will fail with the wrong component")

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//============================ LEVEL_ASSERT_SAFE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#include <bsls_asserttest_macroreset.h>

// [2] Define the macros for this test case

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE
#define BSLS_ASSERTTEST_CHECK_LEVEL

// [3] Re-include the 'bsls_asserttest.h' header

#include <bsls_asserttest.h>

// [4] Confirm the value of the 'BDE_BUILD_TARGET_SAFE_2' macro and the 3
//     'IS_ACTIVE' macros and their values we intend to test.

#if defined(BDE_BUILD_TARGET_SAFE_2)
#error BDE_BUILD_TARGET_SAFE_2 should not be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
SunStudioOnlyPrintsWarningsForPoundError;
#endif

// [5] Confirm the values of the 'BSLS_ASSERTTEST' macros that should be
// defined.
    ASSERT(!BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG)
    ASSERT(BSLS_ASSERTTEST_CHECK_LEVEL_ARG)

// [6] Define a local struct 'Production' which makes use of the current
//     definitions of the 'BSLS_ASSERT' macros.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

// [7] Verify that the *PASS macros are expanded in any build mode.
        {
            EXPECTED = true;

            EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(ASSERT(true));
            ASSERT_STDIO;  EXPECT_SILENCE;
            BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(ASSERT(true));
            ASSERT_STDIO;
        }

// [8] Verify the macros that should not expand in this build mode
        {
            EXPECTED = false;


        }

// [9] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // These tests will fail with the wrong level
        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(false));
        ASSERT_STDIO;

// [10] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

#line BSLS_ASSERTTEST_SET_DIFFERENT_FILENAME
// [11] Define a local struct 'Production' which makes use of
//     the current definitions of the 'BSLS_ASSERT' macros with an
//     alternate filename in place.
    {
        struct Production {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        struct Safe2 {
            static void callOpt(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_OPT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_OPT(pass);
                }
#endif
            }

            static void callAssert(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT(pass);
                }
#endif
            }

            static void callSafe(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                BSLS_ASSERT_SAFE(pass);
                (void)pass;
#else
                if (pass) {
                    BSLS_ASSERT_SAFE(pass);
                }
#endif
            }

            static void callInvoke(bool pass) {
#if defined(BDE_BUILD_TARGET_SAFE_2) && defined(BDE_BUILD_TARGET_EXC)
                if (!pass) {
                    BSLS_ASSERT_INVOKE("!pass");
                }
#else
                (void)pass;
#endif
            }
        };

        // Restore filename before evaluating the test macros
#line BSLS_ASSERTTEST_RESET_THIS_FILENAME

// [12] Test for expressions that should fail

        EXPECTED = false;

        // The following tests will call ASSERT(false)

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Safe2::callSafe(false));
        ASSERT_STDIO;

        // These tests will fail with the wrong level
        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callOpt(false));
        ASSERT_STDIO;

        // The following checks will fail with the wrong component")

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callInvoke(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL(Production::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callOpt(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Production::callSafe(false));
        ASSERT_STDIO;

        EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callAssert(false));
        ASSERT_STDIO; EXPECT_OUTPUT_ON_STDOUT_IF_TARGET_EXC;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_FAIL(Production::callSafe(false));
        ASSERT_STDIO;

// [13] Test for expressions that should pass

        EXPECTED = true;

        // The following tests will call ASSERT(true)

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_INVOKE_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_OPT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;

        EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Production::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callInvoke(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callOpt(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callAssert(false));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(true));
        ASSERT_STDIO; EXPECT_SILENCE;
        BSLS_ASSERTTEST_ASSERT_PASS(Safe2::callSafe(false));
        ASSERT_STDIO;
    }

    // Restore status-quo before starting the next test
    EXPECTED = true;
//---------------------------------------------------END GENERATED: CHECK_LEVEL
//---------------------------------------------------------------------------//

// Restore the default 'ASSERT' macro for this test driver.
#undef ASSERT
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
}

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
