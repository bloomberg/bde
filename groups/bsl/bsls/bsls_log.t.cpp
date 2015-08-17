// bsls_log.t.cpp                                                     -*-C++-*-
#include <bsls_log.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <fcntl.h>
#include <limits.h>    // PATH_MAX on linux, INT_MAX
#include <stdio.h>     // 'snprintf', '_snprintf'
#include <stdlib.h>    // abort
#include <string.h>    // strlen, strncpy
#include <sys/types.h> // struct stat: required on Sun and Windows only
#include <sys/stat.h>  // struct stat: required on Sun and Windows only

#if defined(BSLS_PLATFORM_OS_WINDOWS)
# include <windows.h>
# include <io.h>       // _dup2, _dup, _close
#else
# include <unistd.h>
# include <stdint.h>    // SIZE_MAX.  Cannot include on all Windows platforms.
#endif

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provides a global facility for low-level code to
// write log messages.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o By default, the 'platformDefaultMessageHandler' is used.
//: o Exceptions thrown in a log message handler are propagated.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o Setting, retrieving, and invoking the log message handler is thread-safe.
// ----------------------------------------------------------------------------
// MACROS
// [10] BSLS_LOG(format, ...)
// [ 9] BSLS_LOG_SIMPLE(message)
//
// TYPES
// [ 4] typedef void (*LogMessageHandler)(file, line, message);
//
// CLASS METHODS
// [ 7] static bsls::Log::LogMessageHandler logMessageHandler();
// [ 7] static void setLogMessageHandler(bsls::Log::LogMessageHandler);
// [10] static void logFormattedMessage(file, line, format, ...);
// [ 9] static void logMessage(file, line, message);
// [ 6] static void platformDefaultMessageHandler(file, line, message);
// [ 4] static void stdoutMessageHandler(file, line, message);
// [ 4] static void stderrMessageHandler(file, line, message);
// ----------------------------------------------------------------------------
// [ 5] Test Driver: static void fillBuffer(buffer, size);
// [ 3] WINDOWS DEBUG MESSAGE SINK
// [ 2] TEST-DRIVER LOG MESSAGE HANDLER
// [ 1] STREAM REDIRECTION APPARATUS
// [11] USAGE EXAMPLES
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: Exceptions thrown in a log message handler are propagated.
// [ *] CONCERN: Precondition violations are detected when enabled.
// [ 8] CONCERN: By default, the 'platformDefaultMessageHandler' is used.


// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number


// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                     GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

#ifndef SIZE_MAX
#define SIZE_MAX (static_cast<size_t>(-1))
    // 'SIZE_MAX' is only defined as part of C99, so it may not exist in some
    // pre-C++11 compilers.
#endif


#ifdef BSLS_PLATFORM_OS_WINDOWS
static const size_t PATH_BUFFER_SIZE = MAX_PATH + 1;
#else
static const size_t PATH_BUFFER_SIZE = PATH_MAX + 1;
#endif


static const size_t WINDOWS_DEBUG_MESSAGE_SINK_BUFFER_SIZE = 4096;
    // This represents the size of the buffer that will be used to store debug
    // messages captured in the Windows debugger.  This should generally be at
    // least '4092' (i.e. 4096 - sizeof(DWORD)), since this is the maximum
    // string size that 'OutputDebugString' will write.

static const char * const WINDOWS_SUBPROCESS_EVENT_NAME    = "BSLS_LOG_TEST";
    // This constant represents the name of the Windows 'Event' that will be
    // used for communication between the main test case and its subprocess so
    // that the 'OutputDebugString' functionality can be tested.  Both
    // processes attempt to create an 'Event' with the same name, and will
    // therefore be able to communicate using this event.

static const size_t LOG_MESSAGE_SINK_BUFFER_SIZE           = 4096;
    // This represents the size of the buffers used by the class
    // 'LogMessageSink' to store the file name and line number values received
    // as part of the logging interface.

static const size_t OUTPUT_REDIRECTOR_BUFFER_SIZE          = 4096;
    // This represents the size of the buffer used by the class
    // 'OutputRedirector' to store the captured values loaded in the 'stdout'
    // and 'stderr' error streams.

// Keep the below in sync with 'bsls_log.cpp'
static const size_t WINDOWS_DEBUG_STACK_BUFFER_SIZE        = 1024;
    // This represents the size of the initial stack buffer used in the method
    // 'bsls::Log::platformDefaultMessageHandler' when it is writing output to
    // 'OutputDebugStringA'.  Keep this in sync with 'bsls_log.cpp'

static const size_t LOG_FORMATTED_STACK_BUFFER_SIZE        = 1024;
    // This represents the size of the initial stack buffer used in the method
    // 'bsls::Log::logFormattedMessage' when it is formatting its
    // 'printf'-style output.


// Standard test driver globals:
static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

struct DefaultDataRow {
    int         d_sourceLine;
    const char *d_file;
    int         d_line;
    const char *d_message;
    const char *d_expected;
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    // This is a set of good, useful data that is the cross product of the sets
    // of situations in which the file name string is empty or non-empty, the
    // line is 0 or non-zero, and the message is empty or non-empty.  The file
    // name and message strings may also have 'printf'-style format specifiers
    // to ensure that no unwanted formatting is being done.
    //
    // All normal values:
    {L_,
     "Very VaLidF1%d%dLeN@me",
     408743,
     "Valid \n %fMessage String",
     "Very VaLidF1%d%dLeN@me:408743 Valid \n %fMessage String\n"},

    // Empty message string:
    {L_,
     "Good File Name!%s.cpp",
     2147483000,
     "",
     "Good File Name!%s.cpp:2147483000 \n"},

    // Zero line number:
    {L_,
     "%filename.cpp",
     0,
     "Message String!",
     "%filename.cpp:0 Message String!\n"},

    // Zero line number, empty message:
    {L_,
     "fi%ldename.cpp",
     0,
     "",
     "fi%ldename.cpp:0 \n"},

    // Empty file name:
    {L_,
     "",
     83274892,
     "Good MESSAGE STRING %x ~~~",
     ":83274892 Good MESSAGE STRING %x ~~~\n"},

    // Empty file name, empty message string:
    {L_,
     "",
     93874829,
     "",
     ":93874829 \n"},

    // Empty file name, zero line number:
    {L_,
     "",
     0,
     "Another Message :)",
     ":0 Another Message :)\n"},

    // Empty file name, zero line number, empty message:
    {L_,
     "",
     0,
     "",
     ":0 \n"},
};
static const size_t NUM_DEFAULT_DATA = sizeof(DEFAULT_DATA)
                                       / sizeof(DEFAULT_DATA[0]);

#ifdef BSLS_PLATFORM_OS_WINDOWS

// The Windows implementation of 'platformDefaultMessageHandler' has to format
// data to a stack-local buffer, and if it does not fit, it has to allocate a
// new buffer.  We need to test data that has a length around the size of the
// buffer.  The following buffer will be dynamically initialized by the test
// using 'fillBuffer', and a large enough sub-string of it will be used to
// ensure a formatted string of a specific size.
static const size_t WINDOWS_LARGE_DATA_BUFFER_SIZE =
                                           WINDOWS_DEBUG_STACK_BUFFER_SIZE + 5;
static char WINDOWS_LARGE_DATA_BUFFER[WINDOWS_LARGE_DATA_BUFFER_SIZE];

// What kind of final string lengths do we want to test for the Windows buffer?
// This describes the length of the *final* string, i.e. ":3 <message>\n".
const size_t WINDOWS_LARGE_DATA_LENGTHS[] = {
    WINDOWS_DEBUG_STACK_BUFFER_SIZE - 5,
    WINDOWS_DEBUG_STACK_BUFFER_SIZE - 4,
    WINDOWS_DEBUG_STACK_BUFFER_SIZE - 3,
    WINDOWS_DEBUG_STACK_BUFFER_SIZE - 2,
    WINDOWS_DEBUG_STACK_BUFFER_SIZE - 1,
    WINDOWS_DEBUG_STACK_BUFFER_SIZE,
    WINDOWS_DEBUG_STACK_BUFFER_SIZE + 1,
    WINDOWS_DEBUG_STACK_BUFFER_SIZE + 2,
    WINDOWS_DEBUG_STACK_BUFFER_SIZE + 3,
    WINDOWS_DEBUG_STACK_BUFFER_SIZE + 4,
    WINDOWS_DEBUG_STACK_BUFFER_SIZE + 5
};
const size_t NUM_WINDOWS_LARGE_DATA_LENGTHS =
      sizeof(WINDOWS_LARGE_DATA_LENGTHS)/sizeof(WINDOWS_LARGE_DATA_LENGTHS[0]);

#endif // BSLS_PLATFORM_OS_WINDOWS

// ============================================================================
//                       GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------
static void fillBuffer(char * const buffer, const size_t size)
    // Fill the specified 'size' - 1 number of characters in the specified
    // 'buffer' with a repeated sequence of ['A' - 'Z'], except for the last
    // repetition in which the sequence will be truncated such that only
    // 'size' - 1 total characters will have been written.  Write a null byte
    // to the final position in 'buffer' (index @ 'size' - 1).  The behavior is
    // undefined unless 'buffer' has at least 'size' bytes available.
{

    // Fill the substitution buffer with ['A' .. 'Z'] looping.  All of our
    // systems use ASCII, so 'A' - 'Z' are contiguous.
    const unsigned char numLetters = 26;
    for(size_t i = 0; i < size - 1; ++i) {
        buffer[i] = static_cast<char>('A' + i%numLetters);
    }
    buffer[size - 1] = '\0';
}

// ============================================================================
//                       GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------

                         // =============================
                         // class WindowsDebugMessageSink
                         // =============================
class WindowsDebugMessageSink {
    // This class provides a mechanism to allow the current process to act as a
    // Windows debugger, capturing all logs written to the 'OutputDebugString'
    // Windows API function by other processes.

  private:
    // TYPES
    union SharedMemoryData {
        unsigned char d_rawData[4096];
        struct InterpretedData {
            unsigned long d_pid;
            char d_message[4096 - sizeof(unsigned long)];
        } d_interpretedData;
    };

    // DATA
    bool              d_enabled;                // are we enabled?
    unsigned long     d_expectedPid;            // 'DWORD': our target PID
    void             *d_dbWinMutexHandle;       // 'HANDLE' to 'DBWin' mutex
    void             *d_dbWinDataReadyHandle;   // 'HANDLE' to Data Ready Event
    void             *d_dbWinBufferReadyHandle; // 'HANDLE' to Buffer Ready
    void             *d_dbWinBufferHandle;      // 'HANDLE' to Data Buffer
    SharedMemoryData *d_sharedData_p;           // local mapping of data buffer

    char d_localBuffer[WINDOWS_DEBUG_MESSAGE_SINK_BUFFER_SIZE];

  private:
    // NOT IMPLEMENTED
    WindowsDebugMessageSink(const WindowsDebugMessageSink&);       // = delete;
    WindowsDebugMessageSink& operator=(
                                  const WindowsDebugMessageSink&); // = delete;

  public:
    // CREATORS
    WindowsDebugMessageSink();
        // Create a 'WindowsDebugMessageSink' object that does not hold any
        // OS-provided handles.

    ~WindowsDebugMessageSink();
        // Destroy this object and de-register this process as the Windows
        // debugger.

    // MANIPULATORS
    void disable();
        // De-register this process as the Windows debugger.  The behavior is
        // undefined unless this function is called at most once during the
        // life of this object.

    bool enable(const unsigned long timeoutMilliseconds);
        // Register this object as the Windows debugger.  If a log message is
        // being written, block until this object can be safely registered or
        // until the specified 'timeoutMilliseconds' number of milliseconds
        // have passed.  Return 'true' if this object has been successfully
        // enabled, or return 'false' in case of a timeout or some other error.
        // The behavior is undefined unless unless this function is called at
        // most once during the life of this object.

    void setTargetProcessId(const unsigned long pid);
        // Set this object to accept only debug messages of processes with the
        // specified 'pid'.  Messages from other processes will be discarded.
        // The behavior is undefined unless 'pid' is non-zero.

    bool wait(const unsigned long timeoutMilliseconds);
        // Block until a debug message originating from the process with the
        // PID specified in 'setTargetProcessID' or until the specified
        // 'timeoutMilliseconds' have elapsed. Return 'true' if data was found
        // and can be inspected through a later call to 'message'.  Return
        // 'false' if a timeout occurs or if some other error occurs.  The
        // behavior is undefined unless 'enable' has been successfully called
        // and 'setTargetProcessId' has been called.

    // ACCESSORS
    const char *message();
        // Return the null-terminated message stored by the last call to
        // 'wait', if that call was successful.  The behavior is undefined
        // unless 'wait' has been called and the last call to 'wait' was
        // successful.

};

#ifdef BSLS_PLATFORM_OS_WINDOWS
// In non-Windows builds, this class will simply have no implemented methods.
// The class definition is kept in place under non-Windows builds for simple
// consistency reasons.

// CREATORS
WindowsDebugMessageSink::WindowsDebugMessageSink()
: d_enabled(false)
, d_expectedPid(0)
, d_dbWinMutexHandle(NULL) //HANDLE
, d_dbWinDataReadyHandle(NULL)
, d_dbWinBufferReadyHandle(NULL)
, d_dbWinBufferHandle(NULL)
, d_sharedData_p(NULL)
{
    d_localBuffer[0] = '\0';
}

WindowsDebugMessageSink::~WindowsDebugMessageSink()
{
    disable();
}

// MANIPULATORS
void WindowsDebugMessageSink::disable()
{
    if(!d_enabled) return;                                            // RETURN

    if(d_sharedData_p) {
        UnmapViewOfFile(d_sharedData_p);
        d_sharedData_p = NULL;
    }

    if(d_dbWinBufferHandle) {
        CloseHandle(d_dbWinBufferHandle);
        d_dbWinBufferHandle = NULL;
    }

    if(d_dbWinBufferReadyHandle) {
        CloseHandle(d_dbWinBufferReadyHandle);
        d_dbWinBufferReadyHandle = NULL;
    }

    if(d_dbWinDataReadyHandle) {
        CloseHandle(d_dbWinDataReadyHandle);
        d_dbWinDataReadyHandle = NULL;
    }

    if(d_dbWinMutexHandle) {
        // If, for whatever reason the mutex is still held by us, we should
        // release it.  This call is harmless if we do not own the mutex:
        ReleaseMutex(d_dbWinMutexHandle);
        CloseHandle(d_dbWinMutexHandle);
        d_dbWinMutexHandle = NULL;
    }

    d_enabled = false;
}

bool WindowsDebugMessageSink::enable(const unsigned long timeoutMilliseconds)
{
    // First, we attempt to get a handle to the possibly-already-existing
    // DBWinMutex.  We only need the 'SYNCHRONIZE' permission.  We use the 'A'
    // version so that we can pass an ASCII string for the name.
    d_dbWinMutexHandle = OpenMutexA(SYNCHRONIZE, FALSE, "DBWinMutex");
    if(!d_dbWinMutexHandle) {
        // If the mutex did not exist, we must create it.  Why not call
        // 'CreateMutexA' initially?  Simply because if the mutex exists, it
        // will be opened in 'MUTEX_ALL_ACCESS' mode, which is not what we want
        // since we may not have admin access.
        d_dbWinMutexHandle = CreateMutexA(NULL, FALSE, "DBWinMutex");

        if(!d_dbWinMutexHandle) {
            disable();
            return false;                                             // RETURN
        }
    }

    // Suppose that some program is currently executing 'OutputDebugString',
    // while we are attempting to do this setup.  There are various issues with
    // allowing this to occur, such as the fact that we may find that an event
    // does not exist and then the event may be created by the logging process
    // before we have a chance to create it.  If this happens, our call to
    // 'CreateEventA' may fail, since calling the Create* functions when the
    // item already exists requests the 'ALL_ACCESS' permission types, which we
    // will not be able to get.  There is no way to prevent this when initially
    // retrieving the mutex; the best we can do is first call 'OpenMutexA' and
    // hope for the best.  However, we can solve this with the later calls once
    // we have the mutex, since 'OutputDebugString' will block until it gets
    // the mutex.  Other debuggers may cause us problems if they do not respect
    // the mutex, but there is really nothing we can do about this.
    const unsigned long oldTimeMilliseconds = GetTickCount();
    const unsigned long waitResult = WaitForSingleObject(d_dbWinMutexHandle,
                                                         timeoutMilliseconds);

    if(waitResult == WAIT_TIMEOUT || waitResult == WAIT_FAILED) {
        return false;                                                 // RETURN
    } else if(waitResult == WAIT_ABANDONED) {
        // 'WAIT_ABANDONED' means that the owner of the mutex terminated before
        // the mutex was released.  We should just try to get the mutex
        // ourselves, since we don't rely on the state of the buffer itself.
        const unsigned long timeDiff = GetTickCount() - oldTimeMilliseconds;
        if(timeDiff > timeoutMilliseconds) {
            return false;                                             // RETURN
        }

        const unsigned long newTimeoutMilliseconds = timeoutMilliseconds
                                                     - timeDiff;

        if(WaitForSingleObject(d_dbWinMutexHandle, newTimeoutMilliseconds)
           != WAIT_OBJECT_0) {
            return false;                                             // RETURN
        }
    }

    // If we reached this point, we have gotten the WAIT_OBJECT_0 return value
    // and we now own the mutex.  We must be sure to release it whenever we
    // can.


    // Now we will attempt to retrieve a handle to the 'DBWIN_DATA_READY'
    // event, which is what 'OutputDebugString' uses to signal that it has
    // successfully written its data to the buffer.  We only need the
    // 'SYNCHRONIZE' permission because we are not responsible for setting the
    // data ready event.
    d_dbWinDataReadyHandle = OpenEventA(SYNCHRONIZE,
                                        FALSE,
                                        "DBWIN_DATA_READY");
    if(!d_dbWinDataReadyHandle) {
        // If we have to create the event, we do not want to accidentally
        // signal to ourselves that the data is ready until OutputDebugString
        // sets it.  Therefore, the initial state (third parameter) is 'FALSE'.
        d_dbWinDataReadyHandle = CreateEventA(NULL,
                                              FALSE,
                                              FALSE,
                                              "DBWIN_DATA_READY");
        if(!d_dbWinDataReadyHandle) {
            ReleaseMutex(d_dbWinMutexHandle);
            disable();
            return false;                                             // RETURN
        }
    }

    // Next, the DBWIN_BUFFER_READY event:
    d_dbWinBufferReadyHandle = OpenEventA(EVENT_MODIFY_STATE,
                                          FALSE,
                                          "DBWIN_BUFFER_READY");
    if(!d_dbWinBufferReadyHandle) {
        // If we have to create the event, we do not want to signal that the
        // buffer is ready until we are completely sure everything is ready.
        // Therefore, the initial state (third parameter) is 'FALSE'.
        d_dbWinBufferReadyHandle = CreateEventA(NULL,
                                                FALSE,
                                                FALSE,
                                                "DBWIN_BUFFER_READY");
        if(!d_dbWinBufferReadyHandle) {
            ReleaseMutex(d_dbWinMutexHandle);
            disable();
            return false;                                             // RETURN
        }
    }

    // Finally, we must attempt to get a handle to the file mapping object:
    d_dbWinBufferHandle = OpenFileMappingA(FILE_MAP_READ,
                                           FALSE,
                                           "DBWIN_BUFFER");
    if(!d_dbWinBufferHandle) {
        // If we have to create the event, we do not want to signal that the
        // buffer is ready until we are completely sure everything is ready.
        // Therefore, the initial state (third parameter) is 'FALSE'.
        d_dbWinBufferHandle = CreateFileMappingA(INVALID_HANDLE_VALUE,
                                                 NULL,
                                                 PAGE_READWRITE,
                                                 0,
                                                 sizeof(SharedMemoryData),
                                                 "DBWIN_BUFFER");
        if(!d_dbWinBufferHandle) {
            ReleaseMutex(d_dbWinMutexHandle);
            disable();
            return false;                                             // RETURN
        }
    }

    d_sharedData_p = static_cast<SharedMemoryData*>(
                        MapViewOfFile(d_dbWinBufferHandle,
                                      FILE_MAP_READ,
                                      0,
                                      0,
                                      0));
    if(!d_sharedData_p) {
        ReleaseMutex(d_dbWinMutexHandle);
        disable();
        return false;                                                 // RETURN
    }

    // Finally, now that everything is ready, we will release the mutex and
    // signal that the buffer is ready:
    ReleaseMutex(d_dbWinMutexHandle);
    SetEvent(d_dbWinBufferReadyHandle);

    d_enabled = true;

    return true;
}

void WindowsDebugMessageSink::setTargetProcessId(const unsigned long pid)
{
    d_expectedPid = pid;
}

bool WindowsDebugMessageSink::wait(const unsigned long timeoutMilliseconds)
{

    // Implementation note: The functionality in this class, including this
    // function, can have a much wider contract than it currently does.  The
    // contract was narrowed because the extra flexibility wasn't being used
    // so there was no need to add complexity to the test driver.  If this
    // class is expanded, the contract (and test cases) should be widened to
    // allow a zero-PID being like a wildcard PID, as well as the ability to
    // call 'enable' and 'disable' as many times as one wants.

    if(!d_enabled) {
        // It is too harmful to let this slide.  Even in non-assert mode, it is
        // worth checking our state.
        BSLS_ASSERT_OPT(d_enabled);
        return false;                                                 // RETURN
    }

    const unsigned long oldTimeMilliseconds = GetTickCount();

    bool keepGoing = true;
    while(keepGoing) {

        unsigned long timeDiff = GetTickCount() - oldTimeMilliseconds;
        if(timeDiff > timeoutMilliseconds) {
            return false;                                             // RETURN
        }
        unsigned long newTimeoutMilliseconds = timeoutMilliseconds - timeDiff;

        if(WaitForSingleObject(d_dbWinDataReadyHandle, newTimeoutMilliseconds)
           != WAIT_OBJECT_0) {
            return false;                                             // RETURN
        }

        // We need to hold the mutex to ensure that we have a consistent buffer
        // while we are reading the data.
        timeDiff = GetTickCount() - oldTimeMilliseconds;
        if(timeDiff > timeoutMilliseconds) {
            return false;                                             // RETURN
        }
        newTimeoutMilliseconds = timeoutMilliseconds - timeDiff;

        if(WaitForSingleObject(d_dbWinMutexHandle, newTimeoutMilliseconds)
           != WAIT_OBJECT_0) {
            // In this case, we want to ignore the 'WAIT_ABANDONED' value
            // because if the mutex was abandoned, the buffer will probably be
            // invalid.
            return false;                                             // RETURN
        }

        if(d_expectedPid == 0 ||
           d_expectedPid == d_sharedData_p->d_interpretedData.d_pid) {

            strncpy(d_localBuffer,
                    d_sharedData_p->d_interpretedData.d_message,
                    WINDOWS_DEBUG_MESSAGE_SINK_BUFFER_SIZE - 1);

            d_localBuffer[WINDOWS_DEBUG_MESSAGE_SINK_BUFFER_SIZE - 1] = '\0';

            keepGoing = false; // We must use the loop to break us since we
                               // want to release the mutex.
        }

        ReleaseMutex(d_dbWinMutexHandle);
        SetEvent(d_dbWinBufferReadyHandle);
    }

    return true;
}


// ACCESSORS
const char *WindowsDebugMessageSink::message()
{
    BSLS_ASSERT(d_enabled);
    return d_localBuffer;
}

#endif // defined(BSLS_PLATFORM_OS_WINDOWS)

                         // ====================
                         // class LogMessageSink
                         // ====================

struct LogMessageSink {
    // This struct provides a namespace for a utility function,
    // 'testMessageHandler', which is a valid log message handler
    // ('bsls::Log::LogMessageHandler') that will simply copy all arguments
    // into a set of 'public' 'static' data members.

    // This class is designed as fully static (instead of using a singleton) in
    // order to more easily support registration of the log message handler.
  public:
    // PUBLIC CLASS DATA
    static bool s_hasBeenCalled;                         // have we been called
                                                         // since the last
                                                         // reset?

    static char s_file[LOG_MESSAGE_SINK_BUFFER_SIZE];    // file name buffer

    static int  s_line;                                  // line number

    static char s_message[LOG_MESSAGE_SINK_BUFFER_SIZE]; // message buffer

    // CLASS METHODS
    static void reset();
        // Set 's_hasBeenCalled' to 'false', write a null byte to the beginning
        // of 's_file', set 's_line' to 0, and write a null byte to the
        // beginning of 's_message'.

    static void testMessageHandler(const char *file,
                                   int         line,
                                   const char *message);
        // Copy the specified 'file' string into 's_file'.  Write the specified
        // 'line' to 's_line'. Copy the specified 'message' into 's_message'.
        // The behavior is undefined unless 'f' is a null-terminated string,
        // 'line' is not negative, and 'message' is a null-terminated string.
};

// PUBLIC CLASS DATA
bool LogMessageSink::s_hasBeenCalled                         = false;
char LogMessageSink::s_file[LOG_MESSAGE_SINK_BUFFER_SIZE]    = {'\0'};
int  LogMessageSink::s_line                                  = 0;
char LogMessageSink::s_message[LOG_MESSAGE_SINK_BUFFER_SIZE] = {'\0'};

// CLASS METHODS
void LogMessageSink::reset()
{
    s_hasBeenCalled = false;
    s_file[0]       = '\0';
    s_line          = 0;
    s_message[0]    = '\0';
}

void LogMessageSink::testMessageHandler(const char *file,
                                        int         line,
                                        const char *message)
{
    ASSERT(file);
    ASSERT(line >= 0);
    ASSERT(message);

    s_hasBeenCalled = true;

    strncpy(s_file, file, LOG_MESSAGE_SINK_BUFFER_SIZE);
    s_line = line;
    strncpy(s_message, message, LOG_MESSAGE_SINK_BUFFER_SIZE);


    // Just to be safe.
    s_file   [LOG_MESSAGE_SINK_BUFFER_SIZE - 1] = '\0';
    s_message[LOG_MESSAGE_SINK_BUFFER_SIZE - 1] = '\0';
}


                         // ======================
                         // class OutputRedirector
                         // ======================

// Temp file creation and output redirection re-purposed from the pre-existing
// module in 'bsls_bsltestutil.t.cpp'.

class OutputRedirector {
    // This class provides a facility for redirecting 'stdout' and 'stderr' to
    // temporary files, retrieving output from the respective temporary file
    // and comparing the output to user-supplied character buffers.  An
    // 'OutputRedirector' object can be in an un-redirected state or a
    // redirected state.  If it is a redirected state, it will redirect either
    // 'stdout' or 'stderr', but not both simultaneously.  An
    // 'OutputRedirector' object has the concept of a scratch buffer, where
    // output captured from the process' 'stdout' or 'stderr' stream is stored
    // when the 'OutputRedirector' object is in the redirected state.
    // Throughout this class, the term "captured output" refers to data that
    // has been written to the 'stdout' or 'stderr' stream and is waiting to be
    // loaded into the scratch buffer.  Each time the 'load' method is called,
    // the scratch buffer is truncated, and the captured output is moved into
    // the scratch buffer.  When this is done, there is no longer any captured
    // output.
  public:
    // TYPES
    enum Stream {
        // The 'enum' 'Stream' represents the specific stream which our object
        // is responsible for redirecting.
        STDOUT_STREAM,
        STDERR_STREAM
    };

  private:
    // DATA
    char d_fileName[PATH_BUFFER_SIZE];        // name of temporary capture file

    char d_outputBuffer[OUTPUT_REDIRECTOR_BUFFER_SIZE];
    // 'd_outputBuffer' is the buffer that will hold the captured output.

    const Stream     d_stream;                // the stream for which this
                                              // object is responsible

    bool             d_isRedirectingFlag;     // Is this object currently
                                              // redirecting?

    bool             d_isFileCreatedFlag;     // has a temp file been created?

    bool             d_isOutputReadyFlag;     // has output been read from temp
                                              // file?

    size_t           d_outputSize;            // size of output loaded into
                                              // 'd_outputBuffer'

    struct stat      d_originalStat;          // status information for
                                              // 'stdout' or 'stderr' just
                                              // before redirection

    int              d_duplicatedOriginalFd;  // a file descriptor that is
                                              // associated with a duplicate of
                                              // the original target of the
                                              // redirected stream.  This is
                                              // made by calling 'dup' on the
                                              // original stream before any
                                              // redirection happens

    // PRIVATE MANIPULATORS
    void cleanup();
        // If the redirector is in a redirected state, restore the original
        // target of the redirected stream.  If the temporary file has been
        // created, delete it.

    void cleanupFiles();
        // Delete the temporary file, if it has been created.

    bool generateTempFileName();
        // Load into 'd_fileName' a file name string corresponding to the name
        // of a valid temp file on the system.  Return 'true' if the name was
        // successfully loaded, or 'false' otherwise.

  private:
    // NOT IMPLEMENTED
    OutputRedirector(const OutputRedirector&);                     // = delete;
    OutputRedirector& operator=(const OutputRedirector&);          // = delete;

  public:
    // CREATORS
    explicit OutputRedirector(Stream which);
        // Create an 'OutputRedirector' in an un-redirected state, and with an
        // empty scratch buffer.  Upon a call to 'enable', this redirector will
        // be responsible for redirecting the stream associated with the
        // specified 'which' to a temporary file.  The behavior is undefined
        // unless 'which' is equal to 'OutputRedirector::STDOUT_STREAM' or
        // 'OutputRedirector::STDERR_STREAM'.

    ~OutputRedirector();
        // Destroy this 'OutputRedirector' object.  If the object is in a
        // redirected state, the original stream will be restored to its
        // initial target and the temporary file to which the stream was
        // redirected will be deleted.

    // MANIPULATORS
    void disable();
        // If the redirector is in a redirected state, restore the original
        // target of the redirected stream and close the temporary buffer.  If
        // the redirector is not in a redirected state, this method is a no-op.
        // Calling this method invalidates all output in the temporary file, so
        // a call to 'load' after the next successful 'enable' call will not
        // load any output that was previously written to the file.  This
        // method does not clear the scratch buffer, so one may call 'load'
        // before calling 'disable', and the contents will be available after
        // 'disable' is called. If 'disable' fails to disable the redirection,
        // it will end the program by calling 'std::abort'.

    void enable();
        // If the 'Stream' specified at construction was 'STDOUT_STREAM',
        // redirect 'stdout' to a temporary file.  If the 'Stream' specified at
        // construction was 'STDERR_STREAM', redirect 'stderr' to a temporary
        // file. The temporary file to which the stream is redirected will be
        // created the first time 'enable' is called, and will be deleted when
        // this object is destroyed.  If 'enable' fails to redirect either
        // 'stdout' or 'stderr' it will end the program by calling
        // 'std::abort'.

    bool load();
        // Read captured output into the scratch buffer.  Return 'true' if all
        // captured output was successfully loaded, and 'false' otherwise.
        // Note that captured output is allowed to have zero length.  The
        // behavior is undefined unless 'enable' has been previously called
        // successfully (after the latest call to 'disable', if 'disable' has
        // been called successfully).

    void clear();
        // Reset the scratch buffer to empty.  The behavior is undefined unless
        // 'enable' has been previously called successfully (after the latest
        // call to 'disable' if 'disable' has been called successfully).

    // ACCESSORS
    int compare(const char *expected, size_t expectedLength) const;
        // Compare the character buffer pointed to by the specified pointer
        // 'expected' with any output that has been loaded into the scratch
        // buffer.  The length of the 'expected' buffer is supplied in the
        // specified 'expectedLength'.  Return 0 if the 'expected' buffer has
        // the same length and contents as the scratch buffer, and non-zero
        // otherwise.  Note that the 'expected' buffer is allowed to contain
        // embedded nulls.  The behavior is undefined unless 'enable' has
        // previously been called successfully.

    int compare(const char *expected) const;
        // Compare the character buffer pointed to by the specified pointer
        // 'expected' with any output that has been loaded into the scratch
        // buffer.  The 'expected' buffer is assumed to be a NTBS, and its
        // length is taken to be the string length of the NTBS.  Return 0 if
        // the 'expected' buffer has the same length and contents as the
        // scratch buffer, and non-zero otherwise.  The behavior is undefined
        // unless 'enable' has previously been called successfully.

    const char *getOutput() const;
        // Return the address of the scratch buffer.

    bool isOutputReady() const;
        // Return 'true' if the captured output has been loaded into the
        // scratch buffer, and 'false' otherwise.

    bool isRedirecting() const;
        // Return 'true' if 'stdout' or 'stderr' has been successfully
        // redirected, and 'false' otherwise.

    FILE *nonRedirectedStream() const;
        // Return the value of the global 'stdout' or 'stderr' corresponding to
        // the stream that is not intended to be redirected by this object.

    const struct stat& originalStat() const;
        // Return a reference to the status information for 'stdout' collected
        // just before redirection.  This method is used only to test the
        // correctness of 'OutputRedirector'.

    size_t outputSize() const;
        // Return the number of bytes currently loaded into the scratch buffer.

    FILE *redirectedStream() const;
        // Return the value of the global 'stdout' or 'stderr' corresponding to
        // the stream that is intended to be redirected by this object.

    OutputRedirector::Stream redirectedStreamId() const;
        // Return 'OutputRedirector::STDOUT_STREAM' if this object is
        // responsible for redirecting 'stdout', and
        // 'OutputRedirector::STDERR_STREAM' if this object is responsible for
        // redirecting 'stderr'.
};

// PRIVATE MANIPULATORS
void OutputRedirector::cleanup()
{
    disable();
    cleanupFiles();
}

void OutputRedirector::cleanupFiles()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    _close(d_duplicatedOriginalFd);
#else
    close(d_duplicatedOriginalFd);
#endif

    d_duplicatedOriginalFd = -1;

    if (d_isFileCreatedFlag) {
        unlink(d_fileName);
        d_isFileCreatedFlag = false;
    }
}

bool OutputRedirector::generateTempFileName()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    char tmpPathBuf[PATH_BUFFER_SIZE];
    if (! GetTempPathA(PATH_BUFFER_SIZE, tmpPathBuf) ||
        ! GetTempFileNameA(tmpPathBuf, "bsls", 0, d_fileName)) {
        return false;                                                 // RETURN
    }
#else
    char *fn = tempnam(0, "bsls");
    if (fn) {
        strncpy(d_fileName, fn, PATH_BUFFER_SIZE);
        free(fn);
        if(d_fileName[PATH_BUFFER_SIZE - 1] != '\0') {
            // Uh-oh! 'strncpy' didn't pad with zeroes.  Just fail here.
            return false;                                             // RETURN
        }
    } else {
        return false;                                                 // RETURN
    }
#endif
    if (veryVerbose) {
        fprintf(nonRedirectedStream(),
                "\tUsing '%s' as a base filename.\n",
                d_fileName);
    }
    return '\0' != d_fileName[0]; // Ensure that 'd_fileName' is not empty
}

// CREATORS
OutputRedirector::OutputRedirector(Stream which)
: d_stream(which)
, d_isRedirectingFlag(false)
, d_isFileCreatedFlag(false)
, d_isOutputReadyFlag(false)
, d_outputSize(0)
, d_duplicatedOriginalFd(-1)
{
    BSLS_ASSERT(which == STDOUT_STREAM || which == STDERR_STREAM);
    d_fileName[0] = '\0';
    memset(&d_originalStat, 0, sizeof(struct stat));
}

OutputRedirector::~OutputRedirector()
{
    cleanup();
}

// MANIPULATORS
void OutputRedirector::disable()
{
    if(!d_isRedirectingFlag) {
        return;                                                       // RETURN
    }

    d_isRedirectingFlag = false;

    fflush(redirectedStream());
    //fclose(redirectedStream());

#ifdef BSLS_PLATFORM_OS_WINDOWS
    const int status = _dup2(d_duplicatedOriginalFd,
                             _fileno(redirectedStream()));
#else
    const int status = dup2(d_duplicatedOriginalFd,
                            fileno(redirectedStream()));
#endif

    if(status < 0) {
        if (veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Error " __FILE__ "(%d): Bad 'dup2' status.\n",
                    __LINE__);
        }
        cleanupFiles();
        abort();
    }
}

void OutputRedirector::enable()
{
    if(d_isRedirectingFlag) {
        if (veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Warning " __FILE__ "(%d): Output already redirected\n",
                    __LINE__);
        }

        return;                                                       // RETURN
    }

    // Retain information about original file descriptor for use in later
    // tests.

    const int originalFD = fileno(redirectedStream());
    // These are meant to be the test driver 'ASSERT', not 'BSLS_ASSERT'
    ASSERT(-1 != originalFD);
    ASSERT(0 == fstat(originalFD, &d_originalStat));

    if(d_duplicatedOriginalFd == -1) {
#ifdef BSLS_PLATFORM_OS_WINDOWS
        d_duplicatedOriginalFd = _dup(originalFD);
#else
        d_duplicatedOriginalFd = dup(originalFD);
#endif
    }

    if(d_duplicatedOriginalFd < 0) {
        if (veryVerbose) {
                fprintf(nonRedirectedStream(),
                        "Error " __FILE__ "(%d): Bad 'dup' value.\n",
                        __LINE__);
            }
        cleanup();
        abort();
    }

    if(!d_isFileCreatedFlag) {
        if (!generateTempFileName()) {

            // Get temp file name

            if (veryVerbose) {
                fprintf(nonRedirectedStream(),
                        "Error "
                        __FILE__
                        "(%d): Failed to get temp file name for capture\n",
                        __LINE__);
            }
            cleanup();
            abort();
        }

        d_isFileCreatedFlag = true;
    }

    if (! freopen(d_fileName, "w+", redirectedStream())) {
        if (veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Error " __FILE__ "(%d): Failed to redirect stdout to"
                    " temp file '%s'\n",
                    __LINE__, d_fileName);
        }
        cleanup();
        abort();
    }

#ifdef BSLS_PLATFORM_OS_WINDOWS
    // In Windows, we need to convert the stream to binary mode because Windows
    // will automatically convert '\n' to '\r\n' in text mode output.  Normally
    // this would not be a problem, since the '\r\n' would be converted back to
    // '\n' when we read the file.  However, since we are using the size of the
    // written file to know how much to read, having extra characters will lead
    // to a faulty size reading.
    if (_setmode(_fileno(redirectedStream()), _O_BINARY) < 0) {
        if (veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Error " __FILE__ "(%d): Binary mode change failed.\n",
                    __LINE__);
        }
        cleanup();
        abort();
    }
#endif

    if (EOF == fflush(redirectedStream())) {
        if (veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Warning " __FILE__ "(%d): Error flushing stream\n",
                    __LINE__);
        }
    }

    d_isRedirectingFlag = true;
}

bool OutputRedirector::load()
{
    BSLS_ASSERT(d_isRedirectingFlag);

    if(ferror(redirectedStream()) != 0) {
        if (veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Error " __FILE__ "(%d): Stream has an error\n",
                    __LINE__);
        }
        return false;                                                 // RETURN
    }

    const long tempOutputSize = ftell(redirectedStream());
    const long incremented    = tempOutputSize + 1;

    if(tempOutputSize < 0 || incremented < 0) {
        // Protect against overflow or negative value
        if (veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Bad value from 'ftell': %ld",
                    incremented);
        }
        return false;                                                 // RETURN
    }

    // Conversion to 'unsigned long' is safe because 'incremented' > 0
    if(static_cast<unsigned long>(incremented) > SIZE_MAX) {
        // Our 'incremented' will not fit in a size_t, so it is too big for our
        // buffer.
        if (veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Bad value from 'ftell': %ld",
                    incremented);
        }
        return false;                                                 // RETURN
    }

    // Conversion to 'size_t' is safe because 'tempOutputSize' is nonnegative
    // and 'tempOutputSize'+1 is no larger than 'SIZE_MAX'.
    d_outputSize = static_cast<size_t>(tempOutputSize);

    if (static_cast<size_t>(incremented) > OUTPUT_REDIRECTOR_BUFFER_SIZE) {
        // Refuse to load output if it will not all fit in the scratch buffer,
        // INCLUDING the final null byte.
        if (veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Error "
                        __FILE__
                        "(%d): Captured output exceeds read buffer size\n",
                    __LINE__);
        }
        d_outputSize = 0u;
        return false;                                                 // RETURN
    }

    rewind(redirectedStream());

    const long charsRead = fread(d_outputBuffer,
                                 sizeof(char),
                                 d_outputSize,
                                 redirectedStream());

    if (charsRead < 0 || static_cast<unsigned long>(charsRead)!=d_outputSize) {
        // We failed to read all output from the capture file.
        if (veryVerbose) {
            if (ferror(redirectedStream())) {
                // We encountered a file error, not 'EOF'.
                fprintf(nonRedirectedStream(),
                        "Error "
                            __FILE__
                            "(%d): Non-EOF error with file\n",
                        __LINE__);
            }
            fprintf(nonRedirectedStream(),
                    "Error "
                        __FILE__
                        "(%d): Could not read all captured output\n",
                    __LINE__);
        }

        if(charsRead < 0) {
            d_outputBuffer[0] = '\0';
        } else if(static_cast<unsigned long>(charsRead) >= d_outputSize) {
            // This case should never happen.  This assignment is safe because
            // the total buffer size is enough to hold 'd_outputSize' + 1.
            d_outputBuffer[d_outputSize] = '\0';
        } else {
            d_outputBuffer[charsRead] = '\0';
        }
            // ...to ensure that direct inspection of buffer does not overflow

        return false;                                                 // RETURN

    } else {
        // We have read all output from the capture file.
        d_outputBuffer[d_outputSize] = '\0';
    }

    d_isOutputReadyFlag = true;

    return true;
}

void OutputRedirector::clear()
{
    BSLS_ASSERT(d_isRedirectingFlag);

    d_outputSize = 0u;
    d_isOutputReadyFlag = false;
    d_outputBuffer[0] = '\0';
    rewind(redirectedStream());
}

// ACCESSORS
int OutputRedirector::compare(const char *expected) const
{
    BSLS_ASSERT(expected);
    return compare(expected, strlen(expected));
}

int
OutputRedirector::compare(const char *expected, size_t expectedLength) const
{
    BSLS_ASSERT(expected || ! expectedLength);

    if (!d_isOutputReadyFlag) {
        if (veryVerbose) {
            fprintf(nonRedirectedStream(),
                    "Error " __FILE__ "(%d): No captured output available\n",
                    __LINE__);
        }
        return -1;                                                    // RETURN
    }

    // Use 'memcmp' instead of 'strncmp' to compare 'd_outputBuffer' to
    // 'expected', because 'expected' is allowed to contain embedded nulls.

    return static_cast<size_t>(d_outputSize) != expectedLength ||
           memcmp(d_outputBuffer, expected, expectedLength);
}

const char *OutputRedirector::getOutput() const
{
    return d_outputBuffer;
}

bool OutputRedirector::isOutputReady() const
{
    return d_isOutputReadyFlag;
}

bool OutputRedirector::isRedirecting() const
{
    return d_isRedirectingFlag;
}

FILE *OutputRedirector::nonRedirectedStream() const
{
    // This should return the opposite values
    if(d_stream == STDOUT_STREAM) {
        return stderr;                                                // RETURN
    } else {
        return stdout;                                                // RETURN
    }
}

const struct stat& OutputRedirector::originalStat() const
{
    return d_originalStat;
}

size_t OutputRedirector::outputSize() const
{
    return d_outputSize;
}

FILE *OutputRedirector::redirectedStream() const
{
    if(d_stream == STDOUT_STREAM) {
        return stdout;                                                // RETURN
    } else {
        return stderr;                                                // RETURN
    }
}

OutputRedirector::Stream OutputRedirector::redirectedStreamId() const
{
    return d_stream;
}

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------
namespace UsageExamples {

///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Logging Formatted Messages
///- - - - - - - - - - - - - - - - - - -
// Suppose that we want to write a formatted log message using 'printf'-style
// format specifiers when the preconditions of a function are not met.  The
// 'BSLS_LOG' macro can be used for this purpose.
//
// First, we begin to define a function, 'add', which will return the sum of
// two positive integer values:
//..
// myapp.cpp

unsigned int add(int a, int b)
    // Return the sum of the specified 'a' and the specified 'b'.  The behavior
    // is undefined unless 'a' and 'b' are not negative.
{
//..
//
// Now, we check the precondition of the function, and use the 'BSLS_LOG' macro
// to write a log message if one of the input parameters is less than 0:
//..
    if(a < 0 || b < 0) {
        BSLS_LOG("Error: Invalid input combination (%d, %d).", a, b);
        return 0;                                                     // RETURN
    }

    return static_cast<unsigned int>(a) + static_cast<unsigned int>(b);
}
//..
//
// Next, we may erroneously call the 'add' function with a negative argument:
//..
//  int x = add(3, -100);
//..
// Finally, assuming the default log message handler is currently installed, we
// observe the following output printed to 'stderr' or to the Windows debugger:
//..
//  myapp.cpp:8 Error: Invalid input combination (3, -100).
//..
// Note that an arbitrary string should never be passed to 'BSLS_LOG' as the
// format string.  If the string happens to contain 'printf'-style format
// specifiers but the expected substitutions are not present, it will lead to
// undefined behavior.
//
///Example 2: Logging Formatless Messages
/// - - - - - - - - - - - - - - - - - - -
// Suppose we want to write a raw string, which is not meant to be a
// 'printf'-style format string, to the log.  We can use the macro
// 'BSLS_LOG_SIMPLE' to do this.
//
// First, we define a global association of error codes with error strings:
//..
// myapp.cpp

static const char *errorStrings[4] = {
    "Invalid input value 'a'.",
    "Invalid input value 'b'.",
    "Percentages add up to more than 100%.",
    "Please use '%2f' for a slash character in a URI."
};
//..
// Notice that the fourth string has a sequence that could be misinterpreted as
// a 'printf'-style format specifier.
//
// Then, we define a function that handles error codes and logs an error based
// on the error code:
//..
void handleError(int code)
    // Log the error message associated with the specified 'code'.  The
    // behavior is undefined unless 'code' is in the range [0 .. 3].
{
    BSLS_ASSERT(static_cast<unsigned int>(code)
                < sizeof(errorStrings)/sizeof(errorStrings[0]));
//..
//
// In the case that we receive a valid error code, we would want to log the
// string associated with this code.  We use the macro 'BSLS_LOG_SIMPLE' to
// ensure that the true strings are logged and are not interpreted as format
// strings:
//..
    BSLS_LOG_SIMPLE(errorStrings[code]);
}
//..
// A user may attempt to use error code '3':
//..
//  handleError(3);
//..
// Assuming the default log message handler is the currently installed handler,
// the following line would be printed to 'stderr' or to the Windows debugger:
//..
//  myapp.cpp:14 Please use '%2f' for a slash character in a URI.
//..
//
///Example 3: Using a Different File Name or Line Number
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to define an error handling function that will use the
// logger to print an error message, except that we want to use the file name
// and line number of the code that *called* the error handling function
// instead of the file name and line number of the code *in* the error handling
// function.
//
// First, we will mimic Example 2 by defining a global array of error strings:
//..
// mylib.cpp

static const char *errorStringsNew[4] = {
    "Bad input attempt.",
    "Invalid types.",
    "Invalid username.",
    "Invalid password."
};
//..
// Then, we will define a function that takes in a file name and line number
// along with the error code:
//..
void handleErrorFlexible(const char *file, int line, int code)
    // Log the error message associated with the specified 'code', using the
    // specified 'file' and the specified 'line' as the source location for the
    // error.  The behavior is undefined unless 'file' is a null-terminated
    // string, 'line' is not negative, and 'code' is in the range [0 .. 3].
{
    BSLS_ASSERT(file);
    BSLS_ASSERT(line >= 0);
    BSLS_ASSERT(code >= 0);
    BSLS_ASSERT(static_cast<unsigned int>(code)
                < (sizeof(errorStringsNew)/sizeof(errorStringsNew[0])));
//..
// We can bypass the macros by calling the function 'bsls::Log::logMessage'
// directly, allowing us to pass in the given file name and line number:
//..
    bsls::Log::logMessage(file, line, errorStringsNew[code]);
}
//..
// A user in a different file may now specify the original source of an error:
//..
//  handleErrorFlexible(__FILE__, __LINE__, 2);
//..
// If this line of code were placed on line 5 of the file 'otherapp.cpp', the
// following line would be printed to 'stderr' or to the Windows debugger:
//..
//  otherapp.cpp:5 Invalid username.
//..
//
// Users may wrap their error function in a macro to automatically fill in the
// file name and line number parameters, as done by the 'BSLS_LOG*' macros.

}  // close namespace UsageExamples


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int         test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST %s CASE %d\n", __FILE__, test);

    switch(test) { case 0: // zero is always the leading case
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage examples provided in the component header file compile,
        //:   link, and run as shown.
        //
        // Plan:
        //: 1 Copy all parts of the component documentation that have any
        //:   compilable code into the header of the test driver, within the
        //:   namespace 'UsageExamples'.  Uncomment any usage example lines
        //:   that do not need to execute any code and can exist outside of the
        //:   'main' function.  For code that must be executed in 'main', copy
        //:   it into a case of the test driver, after a declaration of
        //:   'using namespace UsageExamples;'.  White-space may be changed so
        //:   that the code may fit in an indented block.  Code may be
        //:   prepended or appended to allow output to be suppressed.
        //
        // Testing:
        //   USAGE EXAMPLES
        // --------------------------------------------------------------------
        if (verbose) printf("\nUSAGE EXAMPLES"
                            "\n==============\n");

        OutputRedirector stderrRedirector(OutputRedirector::STDERR_STREAM);
        OutputRedirector stdoutRedirector(OutputRedirector::STDOUT_STREAM);

        stderrRedirector.enable();
        if(!verbose) {
            stdoutRedirector.enable();
        }

        using namespace UsageExamples;
        // START OF CODE

        add(3, -100);
        handleError(3);
        handleErrorFlexible(__FILE__, __LINE__, 2);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'logFormattedMessage', MACRO 'BSLS_LOG'
        //
        // Concerns:
        //: 1 'logFormattedMessage' invokes the installed handler with a string
        //:
        //: 2 'logFormattedMessage' properly formats the format string
        //:
        //: 3 'logFormattedMessage' can be called with no variadic arguments
        //:
        //: 4 'logFormattedMessage' properly handles final formatted strings of
        //:   length 0 and 1
        //:
        //: 5 'logFormattedMessage' properly handles final formatted strings of
        //:   lengths above, below, and equal to the size of the initial
        //:   stack-allocated buffer
        //:
        //: 6 'BSLS_LOG' properly invokes 'logFormattedMessage' with the
        //:   current file name, the line number on which the macro was
        //:   invoked, the format string, and any and all variadic arguments
        //:
        //: 7 'BSLS_LOG' can still be called with no variadic arguments
        //:
        //
        // Plan:
        //: 1 Install 'LogMessageSink::testMessageHandler' as the logger's
        //:   handler.
        //:
        //: 2 Call 'logFormattedMessage' with a simple format string requiring
        //:   no substitutions, and confirm that the log message sink has
        //:   received the expected parameters.  (C-1)  (C-3)
        //:
        //: 3 Pass into 'logFormattedMessage' a format string and variadic
        //:   arguments corresponding to very simple output.  (C-2)
        //:
        //: 4 Populate a sufficiently large buffer with a set of characters,
        //:   followed by a null byte.  This will be used to ensure a formatted
        //:   string of a specific length.
        //:
        //: 5 Pass into 'logFormattedMessage' the sets of arguments resulting
        //:   in expected formatted strings with the following formatted
        //:   lengths:
        //:              ===================================
        //:              0
        //:              1
        //:              LOG_FORMATTED_STACK_BUFFER_SIZE - 2
        //:              LOG_FORMATTED_STACK_BUFFER_SIZE - 1
        //:              LOG_FORMATTED_STACK_BUFFER_SIZE
        //:              LOG_FORMATTED_STACK_BUFFER_SIZE + 1
        //:              LOG_FORMATTED_STACK_BUFFER_SIZE + 2
        //:              LOG_FORMATTED_STACK_BUFFER_SIZE * 2
        //:              ===================================
        //:   (C-4) (C-5)
        //:
        //: 6 Call 'BSLS_LOG' with a simple format string and confirm that the
        //:   log message sink has received the correct results.  Ensure that
        //:   the line number is determined from the line of invocation.  Also
        //:   confirm that the file name is the current file name and the
        //:   expected formatted string has been received.  (C-6)
        //:
        //: 7 Call 'BSLS_LOG' with no variadic arguments.  (C-7)
        //
        // Testing:
        //   static void logFormattedMessage(file, line, format, ...);
        //   BSLS_LOG(format, ...)
        // --------------------------------------------------------------------
        if (verbose) {
            printf("\nCLASS METHOD 'logFormattedMessage', MACRO 'BSLS_LOG'"
                   "\n====================================================\n");
        }

        bsls::Log::setLogMessageHandler(&LogMessageSink::testMessageHandler);
        ASSERT(bsls::Log::logMessageHandler()
               == &LogMessageSink::testMessageHandler);

        {
            if (verbose) {
                puts("\nCall 'logFormattedMessage' with a simple string\n");
            }
            LogMessageSink::reset();

            const char * const testFile         = "testFile3.h";
            const int          testLine         = 65536;
            const char * const testFormat       = "Test of a simple format.";

            const char * const expectedMessage  = testFormat;

            bsls::Log::logFormattedMessage(testFile,
                                           testLine,
                                           testFormat);

            ASSERT(LogMessageSink::s_hasBeenCalled);

            LOOP2_ASSERT(testFile,
                         LogMessageSink::s_file,
                         strcmp(testFile, LogMessageSink::s_file) == 0);

            LOOP2_ASSERT(testLine,
                         LogMessageSink::s_line,
                         testLine == LogMessageSink::s_line);

            LOOP2_ASSERT(expectedMessage,
                         LogMessageSink::s_message,
                         strcmp(expectedMessage,LogMessageSink::s_message)==0);

        }

        {
            if (verbose) {
                puts("\nCall 'logFormattedMessage' with a complex string\n");
            }
            LogMessageSink::reset();

            const char * const testFile        = "myTestFile.cpp";
            const int          testLine        = 900123;
            const char * const testFormat      = "String: %s, Int: %d";
            const char * const substitutionStr = "This is a string";
            const int          substitutionInt = 172934;

            const char * const expectedMessage = "String: This is a string, "
                                                 "Int: 172934";

            bsls::Log::logFormattedMessage(testFile,
                                           testLine,
                                           testFormat,
                                           substitutionStr,
                                           substitutionInt);

            ASSERT(LogMessageSink::s_hasBeenCalled);

            LOOP2_ASSERT(testFile,
                         LogMessageSink::s_file,
                         strcmp(testFile, LogMessageSink::s_file) == 0);

            LOOP2_ASSERT(testLine,
                         LogMessageSink::s_line,
                         testLine == LogMessageSink::s_line);

            LOOP2_ASSERT(expectedMessage,
                         LogMessageSink::s_message,
                         strcmp(expectedMessage,LogMessageSink::s_message)==0);

        }

        {
            if (verbose) {
                puts("\nCall 'logFormattedMessage' with various lengths.\n");
            }

            const size_t SUBSTITUTION_BUFFER_SIZE = 1 +
                                             LOG_FORMATTED_STACK_BUFFER_SIZE*2;
            char substitutionBuffer[SUBSTITUTION_BUFFER_SIZE];



            if(veryVerbose) {
                P(substitutionBuffer)
                P(SUBSTITUTION_BUFFER_SIZE)
            }

            // We will index enough space from the end of the buffer to reach
            // each expected length.
            const size_t EXPECTED_LENGTHS[] = {
                0                                  ,
                1                                  ,
                LOG_FORMATTED_STACK_BUFFER_SIZE - 2,
                LOG_FORMATTED_STACK_BUFFER_SIZE - 1,
                LOG_FORMATTED_STACK_BUFFER_SIZE    ,
                LOG_FORMATTED_STACK_BUFFER_SIZE + 1,
                LOG_FORMATTED_STACK_BUFFER_SIZE + 2,
                LOG_FORMATTED_STACK_BUFFER_SIZE * 2 // Note: this is '*'
            };

            const size_t NUM_EXPECTED_LENGTHS = sizeof(EXPECTED_LENGTHS)/
                                                sizeof(EXPECTED_LENGTHS[0]);

            // We will pre-populate the indices into the buffer:
            size_t expectedIndices[NUM_EXPECTED_LENGTHS];
            for(size_t i = 0; i < NUM_EXPECTED_LENGTHS; ++i) {
                expectedIndices[i] = SUBSTITUTION_BUFFER_SIZE
                                     - 1
                                     - EXPECTED_LENGTHS[i];
                if(veryVerbose) {
                    T_ P(i)
                    T_ P(EXPECTED_LENGTHS[i])
                    T_ P(expectedIndices[i])
                }
            }

            // To confirm that we have gotten the indices right, we will ensure
            // that the first index points to the null byte in the buffer
            // (length 0) and the last index points to the first byte in the
            // buffer (length LOG_FORMATTED_STACK_BUFFER_SIZE * 2):
            ASSERT(expectedIndices[0] == SUBSTITUTION_BUFFER_SIZE - 1);
            ASSERT(expectedIndices[NUM_EXPECTED_LENGTHS - 1] == 0);

            const char * const testFile         = "myTestFile.cpp";
            const int          testLine         = 900123;

            for(size_t i = 0; i < NUM_EXPECTED_LENGTHS; ++i) {
                const size_t       localIndex  = expectedIndices[i];
                const char * const localBuffer = substitutionBuffer+localIndex;

                if(veryVerbose) {
                    T_ P(localIndex)
                    if(veryVeryVerbose) {
                        T_ P(localBuffer)
                    }
                }

                LogMessageSink::reset();

                bsls::Log::logFormattedMessage(testFile,
                                               testLine,
                                               "%s",
                                               localBuffer);

                ASSERT(LogMessageSink::s_hasBeenCalled);

                LOOP4_ASSERT(i,
                             localIndex,
                             testFile,
                             LogMessageSink::s_file,
                             strcmp(testFile, LogMessageSink::s_file) == 0);

                LOOP4_ASSERT(i,
                             localIndex,
                             testLine,
                             LogMessageSink::s_line,
                             testLine == LogMessageSink::s_line);

                if(veryVerbose) {
                    LOOP4_ASSERT(i,
                                 localIndex,
                                 localBuffer,
                                 LogMessageSink::s_message,
                                 strcmp(localBuffer, LogMessageSink::s_message)
                                                                          ==0);
                } else {
                    LOOP2_ASSERT(i,
                                 localIndex,
                                 strcmp(localBuffer, LogMessageSink::s_message)
                                                                          ==0);
                }
            }

        }

        {
            if (verbose) {
                puts("\nCall 'BSLS_LOG'\n");
            }

            LogMessageSink::reset();

            const char * const testFile        = __FILE__;
            const char * const testFormat      = "Int: %d, String: %s";
            const int          subInt          = 17372;
            const char * const subStr          = "Hello World!";

            const char * const expectedMessage = "Int: 17372, "
                                                 "String: Hello World!";

            // We are expanding the '__LINE__' macro on the same line as the
            // call to 'BSLS_LOG_SIMPLE' to ensure that the true line numbers
            // match.
            const int testLine =__LINE__; BSLS_LOG(testFormat, subInt, subStr);

            ASSERT(LogMessageSink::s_hasBeenCalled);

            LOOP2_ASSERT(testFile,
                         LogMessageSink::s_file,
                         strcmp(testFile, LogMessageSink::s_file) == 0);

            LOOP2_ASSERT(testLine,
                         LogMessageSink::s_line,
                         testLine == LogMessageSink::s_line);

            LOOP2_ASSERT(expectedMessage,
                         LogMessageSink::s_message,
                         strcmp(expectedMessage,LogMessageSink::s_message)==0);

        }

        {
            if (verbose) {
                puts("\nCall BSLS_LOG with one parameter\n");
            }

            LogMessageSink::reset();

            const char * const testFile        = __FILE__;
            const char * const testFormat      = "Hello World";
            const char * const expectedMessage = testFormat;

            // We are expanding the '__LINE__' macro on the same line as the
            // call to 'BSLS_LOG_SIMPLE' to ensure that the true line numbers
            // match.
            const int testLine =__LINE__; BSLS_LOG(testFormat);

            ASSERT(LogMessageSink::s_hasBeenCalled);

            LOOP2_ASSERT(testFile,
                         LogMessageSink::s_file,
                         strcmp(testFile, LogMessageSink::s_file) == 0);

            LOOP2_ASSERT(testLine,
                         LogMessageSink::s_line,
                         testLine == LogMessageSink::s_line);

            LOOP2_ASSERT(expectedMessage,
                         LogMessageSink::s_message,
                         strcmp(expectedMessage,LogMessageSink::s_message)==0);

        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'logMessage', MACRO 'BSLS_LOG_SIMPLE'
        //
        // Concerns:
        //: 1 'logMessage' properly calls the currently installed handler
        //:
        //: 2 The macro 'BSLS_LOG_SIMPLE' properly calls 'logMessage' with the
        //:   original line of instantiation, not any other line number
        //:
        //: 3 The macro 'BSLS_LOG_SIMPLE' does not interpret the message as a
        //:   'printf'-style format string
        //
        // Plan:
        //: 1 Set the currently installed log message handler to
        //:   'LogMessageSink::testMessageHandler'. (C-1)
        //:
        //: 2 Call 'bsls::Log::logMessage' with simple values. (C-1)
        //:
        //: 3 Confirm that the registered handler was called with the proper
        //:   parameters. (C-1)
        //:
        //: 4 Reset the sink.  Call 'BSLS_LOG_SIMPLE' with a simple message
        //:   that includes 'printf'-style formats that should not be formatted
        //    (C-2) (C-3)
        //:
        //: 5 Confirm that the registered handler was again called, with the
        //:   current file name, the proper line number, and the simple
        //:   message. (C-2) (C-3)
        //
        // Testing:
        //   static void logMessage(file, line, message);
        //   BSLS_LOG_SIMPLE(message)
        // --------------------------------------------------------------------
        if (verbose) {
            printf("\nCLASS METHOD 'logMessage', MACRO 'BSLS_LOG_SIMPLE'"
                   "\n==================================================\n");
        }

        bsls::Log::setLogMessageHandler(&LogMessageSink::testMessageHandler);
        ASSERT(bsls::Log::logMessageHandler()
               == &LogMessageSink::testMessageHandler);

        {
            if (verbose) {
                puts("\nCalling 'logMessage'.\n");
            }
            LogMessageSink::reset();

            const char * const testFile    = "helloworld.cpp";
            const int          testLine    = 18437;
            const char * const testMessage = "Hello World!";

            bsls::Log::logMessage(testFile, testLine, testMessage);

            ASSERT(LogMessageSink::s_hasBeenCalled);

            LOOP2_ASSERT(testFile,
                         LogMessageSink::s_file,
                         strcmp(testFile, LogMessageSink::s_file) == 0);

            LOOP2_ASSERT(testLine,
                         LogMessageSink::s_line,
                         testLine == LogMessageSink::s_line);

            LOOP2_ASSERT(testMessage,
                         LogMessageSink::s_message,
                         strcmp(testMessage, LogMessageSink::s_message) == 0);

        }

        {
            if (verbose) {
                puts("\nCall BSLS_LOG_SIMPLE.\n");
            }

            LogMessageSink::reset();

            const char * const testFile    = __FILE__;
            const char * const testMessage = "Hello%d Wor%ld%s!";

            // We are expanding the '__LINE__' macro on the same line as the
            // call to 'BSLS_LOG_SIMPLE' to ensure that the true line numbers
            // match.
            const int testLine =__LINE__; BSLS_LOG_SIMPLE(testMessage);

            ASSERT(LogMessageSink::s_hasBeenCalled);

            LOOP2_ASSERT(testFile,
                         LogMessageSink::s_file,
                         strcmp(testFile, LogMessageSink::s_file) == 0);

            LOOP2_ASSERT(testLine,
                         LogMessageSink::s_line,
                         testLine == LogMessageSink::s_line);

            LOOP2_ASSERT(testMessage,
                         LogMessageSink::s_message,
                         strcmp(testMessage, LogMessageSink::s_message) == 0);

            }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // DEFAULT HANDLER CONFIRMATION
        //
        // Concerns:
        //: 1 An initial call to 'logMessageHandler' will return
        //:   the address of 'platformDefaultMessageHandler'
        //
        // Plan:
        //: 1 Confirm that 'logMessageHandler' returns the address of
        //:   'platformDefaultMessageHandler'.
        //
        // Testing:
        //   CONCERN: By default, the 'platformDefaultMessageHandler' is used.
        // --------------------------------------------------------------------
        if (verbose) {
            printf("\nDEFAULT HANDLER CONFIRMATION"
                   "\n============================\n");
        }

        ASSERT(bsls::Log::logMessageHandler()
               == &bsls::Log::platformDefaultMessageHandler);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // SETTING AND RETRIEVING THE HANDLER
        //   Ensure that setting and retrieving a specific handler works as
        //   expected.
        //
        // Concerns:
        //: 1 The standard handlers ('stdoutMessageHandler',
        //:   'stderrMessageHandler', 'platformDefaultMessageHandler') can be
        //:   set and retrieved as the handler.
        //:
        //: 2 The handler defined in this test driver,
        //:   'LogMessageSink::testMessageHandler' can be set and retrieved as
        //:   the handler.
        //
        // Plan:
        //: 1 Call 'setLogMessageHandler' with 'stdoutMessageHandler' as a
        //:   parameter, and confirm that 'logMessageHandler()' returns
        //:   'stdoutMessageHandler'. (C-1)
        //:
        //: 2 Call 'setLogMessageHandler' with 'stderrMessageHandler' as a
        //:   parameter, and confirm that 'logMessageHandler()' returns
        //:   'stderrMessageHandler'. (C-1)
        //:
        //: 3 Call 'setLogMessageHandler' with 'platformDefaultMessageHandler'
        //:   as a parameter, and confirm that 'logMessageHandler()' returns
        //:   'platformDefaultMessageHandler'. (C-1)
        //:
        //: 4 Call 'setLogMessageHandler' with 'testMessageHandler' as a
        //:   parameter, and confirm that 'logMessageHandler()' returns
        //:   'testMessageHandler'. (C-1)
        //
        // Testing:
        //   static bsls::Log::LogMessageHandler logMessageHandler();
        //   static void setLogMessageHandler(bsls::Log::LogMessageHandler);
        // --------------------------------------------------------------------
        if (verbose) {
            printf("\nSETTING AND RETRIEVING THE HANDLER"
                   "\n==================================\n");
        }

        bsls::Log::setLogMessageHandler(&bsls::Log::stdoutMessageHandler);
        ASSERT(bsls::Log::logMessageHandler()
               == &bsls::Log::stdoutMessageHandler);

        bsls::Log::setLogMessageHandler(&bsls::Log::stderrMessageHandler);
        ASSERT(bsls::Log::logMessageHandler()
               == &bsls::Log::stderrMessageHandler);

        bsls::Log::setLogMessageHandler(
                                    &bsls::Log::platformDefaultMessageHandler);
        ASSERT(bsls::Log::logMessageHandler()
               == &bsls::Log::platformDefaultMessageHandler);

        bsls::Log::setLogMessageHandler(&LogMessageSink::testMessageHandler);
        ASSERT(bsls::Log::logMessageHandler()
               == &LogMessageSink::testMessageHandler);

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // PLATFORM DEFAULT MESSAGE HANDLER
        //
        // Concerns:
        //: 1 'platformDefaultMessageHandler' delegates log messages to
        //:   'stderrMessageHandler', except in Windows non-console mode.
        //:
        //: 2 In Windows non-console mode, 'platformDefaultMessageHandler'
        //:   writes a string to the Windows debugger.
        //:
        //: 3 In Windows non-console mode, the string formatted by
        //:   'platformDefaultMessageHandler' can handle all of the elements
        //:   in 'DEFAULT_DATA' with which all of the other handlers were
        //:   tested, as well as final formatted strings equal to and near the
        //:   internal stack-allocated buffer size.
        //
        // Plan:
        //: 1 For all systems, write a simple message to
        //:   'platformDefaultMessageHandler', capture 'stderr', and confirm
        //:   that the captured output is as expected.  (C-1)
        //:
        //: 2 Under Windows:
        //:
        //:   1 Create the Windows 'Event' 'BSLS_LOG_TEST'.
        //:
        //:   2 Spawn a copy of this test driver, specifying the manual test
        //:     case '-2'. [This test case will put itself into non-console
        //:     mode and wait for further instructions from us.]
        //:
        //:   3 Declare a 'WindowsDebugMessageSink' object and 'enable' it.
        //:
        //:   4 For each element in 'DEFAULT_DATA', set the 'BSLS_LOG_TEST'
        //:     event and confirm that the expected string was captured.  (C-2)
        //:
        //:   5 Fill the large-data buffer using 'fillBuffer()'.
        //:
        //:   6 For each length in 'WINDOWS_LARGE_DATA_LENGTHS', set the
        //:     'BSLS_LOG_TEST' event and confirm that the expected string was
        //:     captured.  (C-3).
        //:
        //:   7 Confirm that the sub-process exits with status '0'.
        //
        // Testing:
        //   static void platformDefaultMessageHandler(file, line, message);
        // --------------------------------------------------------------------
        if (verbose) {
            printf("\nPLATFORM DEFAULT MESSAGE HANDLER"
                   "\n================================\n");
        }

        {
            if (verbose) {
                fputs("\nConfirming 'stderr' behavior.\n",
                      stdout);
            }
            OutputRedirector stderrRedirector(OutputRedirector::STDERR_STREAM);

            const char * const tFile       = "testingAFile.cpp";
            const int          tLine       = 10272;
            const char * const tMsg        = "Platform default handler!";

            const char * const expectedMsg = "testingAFile.cpp:10272 "
                                             "Platform default handler!\n";

            stderrRedirector.enable();
            (bsls::Log::platformDefaultMessageHandler)(tFile,tLine,tMsg);
            ASSERT(stderrRedirector.load());
            ASSERT(stderrRedirector.isOutputReady());
            stderrRedirector.disable();

            LOOP2_ASSERT(expectedMsg,
                         stderrRedirector.getOutput(),
                         0 == stderrRedirector.compare(expectedMsg));

        }

#ifdef BSLS_PLATFORM_OS_WINDOWS
        {
            if(verbose) printf("\nCreating event '%s'.\n",
                                                WINDOWS_SUBPROCESS_EVENT_NAME);
            HANDLE event = CreateEventA(NULL,
                                        false,
                                        false,
                                        WINDOWS_SUBPROCESS_EVENT_NAME);

            ASSERT(event);

            // Note: This will fail if the current process name or path has a
            // quote in it, or if it is more than 1019 characters long.  To
            // make this completely safe, we'd need to implement a true quoting
            // function as well as dynamic buffer allocation.
            char commandLineBuffer[1024];
            int snprintfStatus;

            if(verbose) {
                // In verbose mode, start the subprocess in verbose too
                snprintfStatus = _snprintf(commandLineBuffer,
                                           1023,
                                           "\"%s\" -2 V",
                                           argv[0]);
            } else {
                // In non-verbose, start the subprocess in non-verbose
                snprintfStatus = _snprintf(commandLineBuffer,
                                           1023,
                                           "\"%s\" -2",
                                           argv[0]);
            }

            if(snprintfStatus < 0) {
                if(verbose) puts("\nCurrent file name too long.\n");
                CloseHandle(event);
                abort();
            }
            commandLineBuffer[1023] = '\0';

            if(verbose) printf("\nStarting process using command: %s\n",
                                                            commandLineBuffer);

            STARTUPINFO startupInfo;
            // MSDN docs say to use 'ZeroMemory'
            ZeroMemory(&startupInfo, sizeof(startupInfo));
            startupInfo.cb = sizeof(startupInfo);

            PROCESS_INFORMATION processInfo;
            ZeroMemory(&processInfo, sizeof(processInfo));

            if(!CreateProcessA(NULL,
                               commandLineBuffer,
                               NULL,
                               NULL,
                               false,
                               0,
                               NULL,
                               NULL,
                               &startupInfo,
                               &processInfo)) {
                if(verbose) puts("\nCould not create subprocess.\n");
                CloseHandle(event);
                abort();
            }

            const unsigned long pid = processInfo.dwProcessId;
            if(verbose) printf("\nChild's process ID is %d.\n", pid);

            if(verbose) puts("\nCreating debug sink.\n");

            WindowsDebugMessageSink sink;

            if(verbose)
                puts("\nAttempting to enable the sink, with 10s timeout.\n");

            ASSERT(sink.enable(10000));

            if(verbose) puts("\nSetting the child's PID in the sink.\n");
            sink.setTargetProcessId(pid);

            if(verbose) puts("\nChecking default data.\n");
            for(size_t i = 0; i < NUM_DEFAULT_DATA; ++i) {
                const int          SOURCE_LINE
                                    = DEFAULT_DATA[i].d_sourceLine;

                const char * const FILE
                                    = DEFAULT_DATA[i].d_file;

                const int          LINE
                                    = DEFAULT_DATA[i].d_line;

                const char * const MESSAGE
                                    = DEFAULT_DATA[i].d_message;

                const char * const EXPECTED
                                    = DEFAULT_DATA[i].d_expected;

                if(veryVerbose) {
                    T_
                    P_(SOURCE_LINE)
                    P_(i)
                    P_(FILE)
                    P_(LINE)
                    P_(MESSAGE)
                    P(EXPECTED)
                }

                if(veryVerbose) puts("\tSetting event\n");
                SetEvent(event);

                if(veryVerbose)
                    puts("\tAttempting to wait for message, 1s timeout.\n");
                ASSERT(sink.wait(1000));

                LOOP4_ASSERT(i,
                             SOURCE_LINE,
                             EXPECTED,
                             sink.message(),
                             0 == strcmp(EXPECTED, sink.message()));

            }

            if(verbose) puts("\nFinished with default data.\n");
            if(verbose) puts("\nTesting long data.\n");
            if(verbose) puts("\nFilling buffer.\n");
            fillBuffer(WINDOWS_LARGE_DATA_BUFFER,
                                               WINDOWS_LARGE_DATA_BUFFER_SIZE);
            if(veryVeryVerbose)
                printf("\nFilled buffer: %s\n", WINDOWS_LARGE_DATA_BUFFER);

            for(size_t i = 0; i < NUM_WINDOWS_LARGE_DATA_LENGTHS; i++) {
                const size_t expectedLength = WINDOWS_LARGE_DATA_LENGTHS[i];

                if(veryVerbose) { T_ P_(i) P(expectedLength) }

                if(veryVerbose) puts("\tSetting event\n");
                SetEvent(event);

                if(veryVerbose)
                    puts("\tAttempting to wait for message, 1s timeout.\n");
                ASSERT(sink.wait(1000));

                if(veryVerbose) puts("\tConfirming lengths.\n");
                const size_t realLength = strlen(sink.message());
                LOOP3_ASSERT(i,
                             expectedLength,
                             realLength,
                             realLength == expectedLength);

                if(veryVeryVerbose)
                    printf("\tReceived message: %s\n", sink.message());

                // Now here is the tricky part.  We first need to confirm that
                // the string is of the form ":0 <message>\n".
                if(veryVerbose) puts("\tConfirming format.\n");
                LOOP3_ASSERT(i,
                             ':',
                             sink.message()[0],
                             ':' == sink.message()[0]);

                LOOP3_ASSERT(i,
                             '0',
                             sink.message()[1],
                             '0' == sink.message()[1]);

                LOOP3_ASSERT(i,
                             ' ',
                             sink.message()[2],
                             ' ' == sink.message()[2]);

                LOOP3_ASSERT(i,
                             '\n',
                             sink.message()[realLength-1],
                             '\n' == sink.message()[realLength-1]);

                // Now, we will simply do an 'strncmp' on the main message.  We
                // need to know where to start in the string.  If
                // 'expectedLength' were '4', then we would start at
                // 'WINDOWS_LARGE_DATA_BUFFER_SIZE - 1', at the terminating
                // null byte. If 'expectedLength' were increased by 1, we would
                // decrease the starting point by 1.  Therefore, the formula is
                // that we start at index:
                // 'WINDOWS_LARGE_DATA_BUFFER_SIZE - 1 + (4 - expectedLength)'
                // What is the length of this new expression?  Well, of course
                // it will just be expectedLength - 4.  Let us express all of
                // this in variables:
                if(veryVerbose) puts("\tCalculating indices.\n");
                const size_t expectedLengthOfMessageSubstring = expectedLength
                                                                - 4;
                const size_t indexIntoLargeDataBuffer =
                                                 WINDOWS_LARGE_DATA_BUFFER_SIZE
                                                 + 3
                                                 - expectedLength;

                if(veryVerbose) {
                    T_
                    P_(expectedLengthOfMessageSubstring)
                    P(indexIntoLargeDataBuffer)
                }

                if(veryVerbose) puts("\tComparing middles.\n");
                // Now we can use 'strncmp' and find out our result:
                LOOP4_ASSERT(i,
                             expectedLength,
                             expectedLengthOfMessageSubstring,
                             indexIntoLargeDataBuffer,
                             0 == strncmp(WINDOWS_LARGE_DATA_BUFFER
                                                    + indexIntoLargeDataBuffer,
                                          sink.message() + 3,
                                          expectedLengthOfMessageSubstring));
            }

            if(verbose) puts("\nDone with large lengths.\n");

            if(verbose) puts("\nWaiting on child process.\n");
            ASSERT(WAIT_OBJECT_0 ==
                              WaitForSingleObject(processInfo.hProcess, 1000));

            unsigned long exitCode;
            ASSERT(GetExitCodeProcess(processInfo.hProcess, &exitCode));
            LOOP_ASSERT(exitCode, exitCode == 0);

            if(verbose) puts("\nClosing handles.\n");
            CloseHandle(event);
            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);
        }
#endif

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // BUFFER FILLING FUNCTION
        //
        // Concerns:
        //: 1 Function fills a buffer appropriately
        //
        // Plan:
        //: 1 Use 'fillBuffer' to fill a buffer, and compare the results with
        //:   an expected value.  The size specified for the buffer should be
        //:   less than its true size so that memory overflow sentinel values
        //:   can be examined.
        //
        // Testing:
        //   Test Driver: static void fillBuffer(buffer, size);
        // --------------------------------------------------------------------
        if (verbose) {
            printf("\nBUFFER FILLING FUNCTION"
                   "\n=======================\n");
        }

        const char expectedValue[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                     "ABC";

        const size_t originalSize = sizeof(expectedValue)
                                    / sizeof(expectedValue[0]);

        const size_t bufferSize = originalSize + 1; // For sentinel value
        char buffer[bufferSize];

        buffer[bufferSize - 1] = 0x4a;
        buffer[bufferSize - 2] = 0x4a;


        ASSERT(originalSize < bufferSize);
        fillBuffer(buffer, originalSize);

        // Make sure final sentinel value was not changed
        ASSERT(0x4a == buffer[bufferSize - 1]);

        // But that second to last sentinel value was change to '\0'
        ASSERT('\0' == buffer[bufferSize - 2]);

        // Now do a 'strcmp':
        ASSERT(0 == strcmp(expectedValue, buffer));

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // STDOUT AND STDERR MESSAGE HANDLERS
        //   Ensure that the methods correctly output a properly formatted
        //   string to their respective streams.
        //
        // Concerns:
        //: 1 'stdoutMessageHandler' writes to 'stdout', and
        //:   'stderrMessageHandler' writes to 'stderr'
        //:
        //: 2 Both methods format the output string with the same expected
        //:   format
        //:
        //: 3 Both methods properly format strings with all combinations of:
        //:   {empty file name, non-empty file name} X {zero line number,
        //:   other line number} X {empty message string, non-empty message
        //:   string}
        //:
        //: 4 Both methods properly handle the extreme value 'INT_MAX' for the
        //:   line number
        //
        // Plan:
        //: 1 For each handler, write a simple string and capture its output.
        //:   Ensure that the captured string matches what is expected. (C-1),
        //:   (C-2)
        //:
        //: 2 For each handler, apply the more extensive format inputs and
        //:   confirm that the output is formatted just like a predefined
        //:   expected format string. (C-3)
        //:
        //: 3 For each handler, send it a normal file name and message, but
        //:   send a line number of INT_MAX, and ensure that the captured
        //:   output is as expected. (C-4)
        //:
        //
        // Testing:
        //   typedef void (*LogMessageHandler)(file, line, message);
        //   static void stdoutMessageHandler(file, line, message);
        //   static void stderrMessageHandler(file, line, message);
        // --------------------------------------------------------------------
        if (verbose) {
            printf("\nSTDOUT AND STDERR MESSAGE HANDLERS"
                   "\n==================================\n");
        }
        static const struct {
            bsls::Log::LogMessageHandler d_handler;
            OutputRedirector::Stream d_stream;
        } HANDLERS[] = {
            {&bsls::Log::stdoutMessageHandler,OutputRedirector::STDOUT_STREAM},
            {&bsls::Log::stderrMessageHandler,OutputRedirector::STDERR_STREAM}
        };
        const size_t NUM_HANDLERS = sizeof(HANDLERS)/sizeof(HANDLERS[0]);

        // If more handlers are added, extra code changes will be required.
        ASSERT(NUM_HANDLERS == 2);

        for(size_t i=0; i < NUM_HANDLERS; ++i) {
            const bsls::Log::LogMessageHandler HANDLER = HANDLERS[i].d_handler;
            const OutputRedirector::Stream     STREAM  = HANDLERS[i].d_stream;

            if(veryVerbose) {
                if(STREAM == OutputRedirector::STDOUT_STREAM) {
                    puts("\nTesting 'stdoutMessageHandler'.\n");
                } else {
                    puts("\nTesting 'stderrMessageHandler'.\n");
                }
            }

            if(veryVerbose) { P_(i) P_(STREAM)}

            OutputRedirector redirector(STREAM);

            // 1 Basic Writing Operation'
            if(veryVerbose) puts("\nWriting simple string.\n");
            redirector.enable();
            HANDLER("testfile.cpp", 1073, "Testing basic operation.");
            redirector.load();
            redirector.disable();
            LOOP2_ASSERT(i, STREAM, redirector.isOutputReady());

            // 2 Proper Simple Format
            if(veryVerbose) puts("\nConfirming simple string.\n");
            LOOP3_ASSERT(i,
                         STREAM,
                         redirector.getOutput(),
                         0 == redirector.compare(
                              "testfile.cpp:1073 Testing basic operation.\n"));

            // 3 Proper Complex Format
            if(veryVerbose) puts("\nChecking complex combinations.\n");
            for(size_t j = 0; j < NUM_DEFAULT_DATA; ++j) {
                const int          SOURCE_LINE
                                    = DEFAULT_DATA[j].d_sourceLine;

                const char * const FILE
                                    = DEFAULT_DATA[j].d_file;

                const int          LINE
                                    = DEFAULT_DATA[j].d_line;

                const char * const MESSAGE
                                    = DEFAULT_DATA[j].d_message;

                const char * const EXPECTED
                                    = DEFAULT_DATA[j].d_expected;

                if(veryVerbose)
                { T_
                  P_(SOURCE_LINE)
                  P_(i)
                  P_(STREAM)
                  P_(j)
                  P_(FILE)
                  P_(LINE)
                  P_(MESSAGE)
                  P_(EXPECTED)
                }

                redirector.enable();
                HANDLER(FILE, LINE, MESSAGE);
                redirector.load();
                redirector.disable();

                LOOP5_ASSERT(i,
                             j,
                             SOURCE_LINE,
                             EXPECTED,
                             redirector.getOutput(),
                             0 == redirector.compare(EXPECTED));

            }

            // 4 INT_MAX handling
            if(veryVerbose) puts("\nChecking INT_MAX.\n");

            const char * const normalFile    = "some_file.cpp";
            const int          extremeLine   = INT_MAX;
            const char * const normalMessage = "Some message!";

            // There will be no system in which INT_MAX will be more than ~20
            // characters (in hypothetical 64-bit case), so we have a very good
            // buffer space here:
            char expectedString[50];

#ifdef BSLS_PLATFORM_OS_WINDOWS
            const int status = _snprintf(
                              expectedString,
                              sizeof(expectedString)/sizeof(expectedString[0]),
                              "%s:%d %s\n",
                              normalFile,
                              extremeLine,
                              normalMessage);
#else
            const int status = snprintf(
                              expectedString,
                              sizeof(expectedString)/sizeof(expectedString[0]),
                              "%s:%d %s\n",
                              normalFile,
                              extremeLine,
                              normalMessage);
#endif

            ASSERT(status > 0);
            ASSERT(static_cast<unsigned int>(status)
                           < sizeof(expectedString)/sizeof(expectedString[0]));

            redirector.enable();
            HANDLER(normalFile, extremeLine, normalMessage);
            redirector.load();
            redirector.disable();

            LOOP4_ASSERT(i,
                         STREAM,
                         expectedString,
                         redirector.getOutput(),
                         0 == redirector.compare(expectedString));

        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // WINDOWS DEBUG MESSAGE SINK
        //   Ensure that objects of the class 'WindowsDebugMessageSink' capture
        //   debug messages as expected.
        //
        // Concerns:
        //: 1 A call to 'enable' takes no longer than the specified time-out
        //:   limit (with an uncertainty).
        //:
        //: 2 After a successful call to 'enable', a call to 'wait' can be
        //:   used to wait for or capture a currently available debug message.
        //:
        //: 3 A call to 'wait' takes no longer than the specified time-out
        //:   limit (with an uncertainty).
        //:
        //: 4 After a successful call to 'wait', the captured message can be
        //:   retrieved by calling 'message'.
        //:
        //: 5 'wait' will only capture messages with the process ID set in
        //:   'setTargetProcessID'
        //:
        // Plan:
        //:  1 TBD
        //
        // Testing:
        //   WINDOWS DEBUG MESSAGE SINK
        // --------------------------------------------------------------------
        if (verbose) {
            printf("\nWINDOWS DEBUG MESSAGE SINK"
                   "\n==========================\n");
        }
#ifdef BSLS_PLATFORM_OS_WINDOWS
        {
            // TBD
        }
#else
        if (verbose) {
            puts("\nNot Windows, test passed trivially.\n");
        }
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST-DRIVER LOG MESSAGE HANDLER
        //:  Ensure that the locally defined handler exhibits proper behavior.
        //:
        // Concerns:
        //: 1 The 'static' state of 'LogMessageSink' is initialized as valid.
        //:
        //: 2 Calling 'LogMessageSink::testMessageHandler' results in the
        //:   proper values being set.
        //:
        //: 3 Calling 'LogMessageSink::reset' resets all 'static' members to
        //:   their state at initialization.
        //:
        // Plan:
        //: 1 Confirm that all four 'static' members of 'LogMessageSink' are
        //:   valid empty values.  (C-1)
        //:
        //: 2 Call 'LogMessageSink::testMessageHandler' with valid values, and
        //:   confirm that the state variables are valid.  (C-2)
        //:
        //: 3 Call 'LogMessageSink::reset' and confirm that all variables are
        //:   now empty again.  (C-3)
        //
        // Testing:
        //   TEST-DRIVER LOG MESSAGE HANDLER
        // --------------------------------------------------------------------
        if (verbose) {
            printf("\nTEST-DRIVER LOG MESSAGE HANDLER"
                   "\n===============================\n");
        }

        ASSERT(! LogMessageSink::s_hasBeenCalled);
        ASSERT(!*LogMessageSink::s_file);
        ASSERT(! LogMessageSink::s_line);
        ASSERT(!*LogMessageSink::s_message);

        const char * const file    = "TeStIng123.cpp";
        const int          line    = 893721;
        const char * const message = "Testing\nThe\nTest";

        (LogMessageSink::testMessageHandler)(file, line, message);

        ASSERT(LogMessageSink::s_hasBeenCalled);

        LOOP2_ASSERT(file,
                     LogMessageSink::s_file,
                     strcmp(file, LogMessageSink::s_file) == 0);

        LOOP2_ASSERT(line,
                     LogMessageSink::s_line,
                     line == LogMessageSink::s_line);

        LOOP2_ASSERT(message,
                     LogMessageSink::s_message,
                     strcmp(message, LogMessageSink::s_message) == 0);

        LogMessageSink::reset();

        ASSERT(! LogMessageSink::s_hasBeenCalled);
        ASSERT(!*LogMessageSink::s_file);
        ASSERT(! LogMessageSink::s_line);
        ASSERT(!*LogMessageSink::s_message);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // STREAM REDIRECTION APPARATUS
        //
        // Concerns:
        //:  1 Object can be initialized for 'stderr' and is in a proper state
        //:
        //:  2 'redirectedStream' and 'nonRedirectedStream' work
        //:
        //:  3 'enable' works and can be called many times in order
        //:
        //:  4 Output is redirected
        //:
        //:  5 Captured output is readable
        //:
        //:  6 'load' works
        //:
        //:  7 'clear' works
        //:
        //:  8 'compare' works
        //:
        //:  9 Incorrect output is correctly diagnosed
        //:
        //: 10 Embedded newlines work
        //:
        //: 11 Empty output works
        //:
        //: 12 Embedded nulls work
        //:
        //: 13 Filesystem-dependent control sequences work
        //:
        //: 14 Calling 'enable' after any number of 'disable' calls produces
        //:    consistent behavior
        //:
        //: 15 Object can be enabled for 'stdout' and is in a proper state
        //:
        //: 17 Output is redirected for 'stdout'
        //:
        //: 18 'compare' works for 'stdout'
        //:
        //
        // Plan:
        //:  1 Initialize an 'OutputRedirector' object and ask it to redirect
        //:    'stderr'.  Confirm that a call to 'redirectedStreamId' returns
        //:    'STDERR_STREAM'. (C-1)
        //:
        //:  2 Confirm that 'redirectedStream' returns a pointer equal to
        //:    'stderr', and 'nonRedirectedStream' returns a pointer equal to
        //:    'stdout'. (C-2)
        //:
        //:  3 Call 'enable', and ensure that 'isRedirecting' returns 'true'.
        //:    Again call 'enable' and ensure that 'isRedirecting' still
        //:    returns 'true'. (C-3)
        //:
        //:  4 Confirm that 'ftell(stderr)' succeeds.  This demonstrates that
        //:    'stderr' is a seekable file. (C-4)
        //:
        //:  5 Write a string to 'stderr', confirm that 'stderr's seek position
        //:    has changed, read back the contents of 'stderr' and compare them
        //:    to the original string. (C-5)
        //:
        //:  6 Write a string to 'stderr'.  Confirm that
        //:    'OutputRedirector::load' changes the contents of the output
        //:    buffer and that it changes the result of
        //:    'OutputRedirector::isOutputReady' from 'false' to 'true'.
        //:    Confirm that the contents of the output buffer match the
        //:    original string. (C-6)
        //:
        //:  7 Write a string to 'stderr' and load it with
        //:    'OutputRedirector::load'.  Confirm that
        //:    'OutputRedirector::clear' rewinds 'stderr', changes the output
        //:    of 'OutputRedirector::isOutputReady' from 'true' to 'false' and
        //:    sets the length of the output buffer to 0. (C-7)
        //:
        //:  8 Write a string to 'stderr' and read it back with
        //:    'OutputRedirector::load'.  Confirm that
        //:    'OutputRedirector::compare' gives the correct results when the
        //:    captured output is compared with the following data: (C-8)
        //:
        //:        Data                            Comparison Result
        //:    ------------                    -------------------------
        //:    input string                            true
        //:    input string with appended data         false
        //:    input string truncated                  false
        //:    string different from input:
        //:    at beginning                            false
        //:    at end                                  false
        //:    elsewhere                               false
        //:
        //:  9 Confirm that 'load' fails when there is more data in 'stderr'
        //:    than can be fit in the capture buffer.  Confirm that 'compare'
        //:    fails if 'load' has not been first called to read data into the
        //:    capture buffer. (C-9)
        //:
        //: 10 Confirm that strings containing embedded newlines are correctly
        //:    captured and correctly identified by 'compare'. (C-10)
        //:
        //: 11 Write an empty string to 'stderr'.  Confirm that it can be
        //:    correctly loaded and compared with the original. (C-11)
        //:
        //: 12 Write a series of strings to 'stderr', containing '\0' at the
        //:    beginning, end or interior of the string.  Confirm that the
        //:    captured output can be correctly loaded and compared with the
        //:    original input. (C-12)
        //:
        //: 13 Write a series of strings to 'stderr' containing '^D' and
        //:    '<CRLF>' and confirm that these strings are correctly captured
        //:    and loaded. (C-13)
        //:
        //: 14 Clear the scratch buffer.  Write a string to 'stderr'. Call
        //:    'load'.  Confirm 'isOutputReady'.  Call 'compare'.  Call
        //:    'disable'.  Confirm that 'isRedirecting' is false.  Call
        //:    'disable' and again confirm that 'isRedirecting' is false.
        //:    Confirm 'isOutputReady'.  Call 'compare'.  Call 'enable'.
        //:    Confirm that 'isRedirecting' is true.  Confirm 'isOutputReady'.
        //:    Call 'compare'.  Confirm that 'ftell(stderr)' succeeds.  This
        //:    demonstrates that 'stderr' is a seekable file.  Call 'load'.
        //:    Confirm 'isOutputReady'. Ensure that the buffer is now empty.
        //:    (C-14)
        //:
        //: 15 Initialize an 'OutputRedirector' object and ask it to redirect
        //:    'stdout'.  Confirm that a call to 'redirectedStreamId' returns
        //:    'STDOUT_STREAM'.  Confirm that 'redirectedStream' returns a
        //:    pointer equal to 'stdout', and 'nonRedirectedStream' returns a
        //:    pointer equal to 'stderr'.  Call 'enable' and confirm
        //:    'isRedirecting'.
        //:
        //: 17 Confirm that 'ftell(stdout)' succeeds.  This demonstrates that
        //:    'stdout' is a seekable file.
        //:
        //: 18 Write a simple string to 'stdout' and read it back with
        //:    'OutputRedirector::load'.  Confirm that
        //:    'OutputRedirector::compare' gives the correct results.
        //
        // Testing:
        //   STREAM REDIRECTION APPARATUS
        // --------------------------------------------------------------------

        // Test cases re-purposed from 'OutputRedirector' test cases in
        // 'bsls_bsltestutil.cpp'.

        if (verbose) {
            printf("\nSTREAM REDIRECTION APPARATUS"
                   "\n============================\n");
        }

        OutputRedirector stderrRedirector(OutputRedirector::STDERR_STREAM);

        {
            // 1 Object has valid state
            if (verbose) {
                fputs("\nTESTING OBJECT INITIALIZATION"
                      "\n-----------------------------\n",
                      stdout);
            }

            ASSERT(stderrRedirector.redirectedStreamId()
                   == OutputRedirector::STDERR_STREAM);
        }

        {
            // 3 Object has consistent internals
            if (verbose) {
                fputs("\nTESTING OBJECT CONSISTENCY"
                      "\n--------------------------\n",
                      stdout);
            }

            ASSERT(stderrRedirector.redirectedStream()    == stderr);
            ASSERT(stderrRedirector.nonRedirectedStream() == stdout);
        }

        {
            // 3 'enable' works
            if (verbose) {
                fputs("\nTESTING ENABLE CALL"
                      "\n-------------------\n",
                      stdout);
            }

            stderrRedirector.enable();
            ASSERT(stderrRedirector.isRedirecting());

            stderrRedirector.enable();
            ASSERT(stderrRedirector.isRedirecting());
        }

        {
            // 4 Output is redirected
            if (verbose) {
                fputs("\nTESTING OUTPUT REDIRECTION"
                      "\n--------------------------\n",
                      stdout);
            }

            ASSERT(-1 != ftell(stderr));
        }

        {
            // 5 Captured output is readable
            if (verbose) {
                fputs("\nTESTING OUTPUT CAPTURE"
                      "\n----------------------\n",
                      stdout);
            }

            enum { TEST_STRING_SIZE = 15 };
            const char *testString = "This is output";
            char buffer[TEST_STRING_SIZE];

            ASSERT(TEST_STRING_SIZE == strlen(testString) + 1);

            rewind(stderr);
            long initialStderrPosition = ftell(stderr);
            ASSERT(0 == initialStderrPosition);
            fprintf(stderr, "%s", testString);
            long finalStderrPosition = ftell(stderr);
            ASSERT(-1 != finalStderrPosition);
            ASSERT(finalStderrPosition > initialStderrPosition);
            long outputSize = finalStderrPosition - initialStderrPosition;
            ASSERT(outputSize + 1 == TEST_STRING_SIZE);
            rewind(stderr);
            size_t bytesWritten =
                fread(buffer, sizeof(char), outputSize, stderr);
            ASSERT(static_cast<long>(bytesWritten) == outputSize);
            buffer[TEST_STRING_SIZE - 1] = '\0';
            ASSERT(0 == strcmp(testString, buffer));
        }

        {
            // 6 'load' works
            if (verbose) {
                fputs("\nTESTING OUTPUT CAPTURE LOAD"
                      "\n---------------------------\n",
                      stdout);
            }

            const char *testString = "This is output";
            size_t testStringLength = strlen(testString);

            rewind(stderr);
            fprintf(stderr, "%s", testString);
            ASSERT(static_cast<long>(testStringLength) == ftell(stderr));
            ASSERT(false == stderrRedirector.isOutputReady());
            ASSERT(0 == stderrRedirector.outputSize());
            ASSERT(stderrRedirector.load());
            ASSERT(static_cast<long>(testStringLength) == ftell(stderr));
            ASSERT(true == stderrRedirector.isOutputReady());
            ASSERT(testStringLength == stderrRedirector.outputSize());
            ASSERT(0 == memcmp(testString,
                               stderrRedirector.getOutput(),
                               stderrRedirector.outputSize()));
        }

        {
            // 7 'clear' works
            if (verbose) {
                fputs("\nTESTING OUTPUT CAPTURE RESET"
                      "\n----------------------------\n",
                      stdout);
            }

            const char *testString = "This is output";
            size_t testStringLength = strlen(testString);

            rewind(stderr);
            fprintf(stderr, "%s", testString);
            stderrRedirector.load();
            ASSERT(static_cast<long>(testStringLength) == ftell(stderr));
            ASSERT(true == stderrRedirector.isOutputReady());
            ASSERT(testStringLength == stderrRedirector.outputSize());
            stderrRedirector.clear();
            ASSERT(0 == ftell(stderr));
            ASSERT(true != stderrRedirector.isOutputReady());
            ASSERT(0 == stderrRedirector.outputSize());
        }

        {
            // 8 'compare' works
            if (verbose) {
                fputs("\nTESTING OUTPUT CAPTURE VERIFICATION"
                      "\n-----------------------------------\n",
                      stdout);
            }

            const char *testString            = "This is output";
            const char *longString            = "This is outputA";
            const char *shortString           = "This is outpu";
            const char *differentStartString  = "Xhis is output";
            const char *differentEndString    = "This is outpuy";
            const char *differentMiddleString = "This iz output";

            stderrRedirector.clear();
            fprintf(stderr, "%s", testString);
            ASSERT(stderrRedirector.load());
            ASSERT(!!strcmp(testString, testString) ==
                       !!stderrRedirector.compare(testString));
            ASSERT(!!strcmp(testString, shortString) ==
                       !!stderrRedirector.compare(shortString));
            ASSERT(!!strcmp(testString, longString) ==
                       !!stderrRedirector.compare(longString));
            ASSERT(!!strcmp(testString, differentStartString) ==
                       !!stderrRedirector.compare(differentStartString));
            ASSERT(!!strcmp(testString, differentEndString) ==
                       !!stderrRedirector.compare(differentEndString));
            ASSERT(!!strcmp(testString, differentMiddleString) ==
                       !!stderrRedirector.compare(differentMiddleString));
        }

        {
            // 9 Incorrect output is correctly diagnosed
            if (verbose) {
                fputs("\nTESTING OUTPUT CAPTURE ERROR"
                      "\n----------------------------\n",
                      stdout);
            }

            // Reset verbosity levels to suppress expected error output
            const bool tempVeryVerbose     = veryVerbose;
            const bool tempVeryVeryVerbose = veryVeryVerbose;
            veryVerbose     = false;
            veryVeryVerbose = false;

            const char *testString = "This is good output";

            stderrRedirector.clear();
            size_t strLength = strlen(testString);

            // Print this string enough times to be *larger* than the expected
            // output buffer size
            for (int i=0; i*strLength <= OUTPUT_REDIRECTOR_BUFFER_SIZE; ++i) {
                fprintf(stderr, "%s", testString);
            }
            fprintf(stderr, "%s", testString);
            ASSERT(!stderrRedirector.load());

            stderrRedirector.clear();
            fprintf(stderr, "%s", testString);
            ASSERT(0 != stderrRedirector.compare(testString));

            stderrRedirector.clear();
            ASSERT(0 != stderrRedirector.compare("", 0));

            veryVerbose     = tempVeryVerbose;
            veryVeryVerbose = tempVeryVeryVerbose;
        }

        {
            // 10 Embedded newlines work
            if (verbose) {
                fputs("\nTESTING EMBEDDED NEWLINES"
                      "\n-------------------------\n",
                      stdout);
            }

            const char *testString = "This has an\nembedded newline";

            stderrRedirector.clear();
            fprintf(stderr, "%s", testString);
            ASSERT(stderrRedirector.load());
            ASSERT(0 == stderrRedirector.compare(testString));

            const char *twoNewlineTestString =
                "This has two\nembedded newlines\n";

            stderrRedirector.clear();
            fprintf(stderr, "%s", twoNewlineTestString);
            ASSERT(stderrRedirector.load());
            ASSERT(0 == stderrRedirector.compare(twoNewlineTestString));
        }

        {
            // 11 Empty output works
            if (verbose) {
                fputs("\nTESTING EMPTY OUTPUT"
                      "\n--------------------\n",
                      stdout);
            }

            const char *testString = "";

            stderrRedirector.clear();
            fprintf(stderr, "%s", testString);
            ASSERT(stderrRedirector.load());
            ASSERT(0 == stderrRedirector.compare(testString));
        }

        {
            // 12 Embedded nulls work
            if (verbose) {
                fputs("\nTESTING EMBEDDED NULLS"
                      "\n----------------------\n",
                      stdout);
            }

            const char *testString1 = "abc\0def";
            const char *testString2 = "\0def";
            const char *testString3 = "abc\0";

            stderrRedirector.clear();
            fwrite(testString1, sizeof(char), 7, stderr);
            ASSERT(stderrRedirector.load());
            ASSERT(0 == stderrRedirector.compare(testString1, 7));

            stderrRedirector.clear();
            fwrite(testString2, sizeof(char), 4, stderr);
            ASSERT(stderrRedirector.load());
            ASSERT(0 == stderrRedirector.compare(testString2, 4));

            stderrRedirector.clear();
            fwrite(testString3, sizeof(char), 4, stderr);
            ASSERT(stderrRedirector.load());
            ASSERT(0 == stderrRedirector.compare(testString3, 4));
        }

        {
            // 13 Filesystem-dependent control sequences work
            if (verbose) {
                fputs("\nTESTING CONTROL SEQUENCES"
                      "\n-------------------------\n",
                      stdout);
            }

            const char *crnlTestString = "ab\r\ncd";
            const char *ctrlDTestString = "ab" "\x04" "cd";

            stderrRedirector.clear();
            fprintf(stderr, "%s", crnlTestString);
            ASSERT(stderrRedirector.load());
            ASSERT(0 == stderrRedirector.compare(crnlTestString));

            stderrRedirector.clear();
            fprintf(stderr, "%s", ctrlDTestString);
            ASSERT(stderrRedirector.load());
            ASSERT(0 == stderrRedirector.compare(ctrlDTestString));
        }

        {
            // 14 'disable' then 'enable' works
            if (verbose) {
                fputs("\nTESTING DISABLE THEN ENABLE"
                      "\n---------------------------\n",
                      stdout);
            }

            const char *testString = "helloWorld\n123\nabcdefg\n";

            stderrRedirector.clear();
            fputs(testString, stderr);
            ASSERT(stderrRedirector.load());

            ASSERT(stderrRedirector.isOutputReady());

            ASSERT(0 == stderrRedirector.compare(testString));

            stderrRedirector.disable();

            ASSERT(!stderrRedirector.isRedirecting());

            stderrRedirector.disable();

            ASSERT(!stderrRedirector.isRedirecting());

            ASSERT(stderrRedirector.isOutputReady());

            ASSERT(0 == stderrRedirector.compare(testString));

            stderrRedirector.enable();

            ASSERT(stderrRedirector.isRedirecting());

            ASSERT(stderrRedirector.isOutputReady());

            ASSERT(0 == stderrRedirector.compare(testString));

            ASSERT(-1 != ftell(stderr));

            stderrRedirector.load();

            ASSERT(stderrRedirector.isOutputReady());

            ASSERT(0 == stderrRedirector.compare(""));
        }

        stderrRedirector.disable();

        OutputRedirector stdoutRedirector(OutputRedirector::STDOUT_STREAM);;

        {
            // 15 Object has valid state for 'stdout'
            if (verbose) {
                fputs("\nTESTING REDIRECTOR FOR STDOUT"
                      "\n-----------------------------\n",
                      stdout);
            }

            ASSERT(stdoutRedirector.redirectedStreamId()
                   == OutputRedirector::STDOUT_STREAM);
            ASSERT(stdoutRedirector.redirectedStream()    == stdout);
            ASSERT(stdoutRedirector.nonRedirectedStream() == stderr);
            stdoutRedirector.enable();
            ASSERT(stdoutRedirector.isRedirecting());
            stdoutRedirector.disable();
        }

        {
            // 16 Output is redirected
            if (verbose) {
                fputs("\nTESTING FTELL STATE"
                      "\n-------------------\n",
                      stdout);
            }
            stdoutRedirector.enable();
            ASSERT(-1 != ftell(stdout));
            stdoutRedirector.disable();
        }

        {
            // 17 'compare' works
            if (verbose) {
                fputs("\nTESTING A SIMPLE STRING"
                      "\n-----------------------\n",
                      stdout);
            }
            stdoutRedirector.enable();
            fprintf(stdout, "Hello\nWorld\n%s", "World, Hello");
            ASSERT(stdoutRedirector.load());
            ASSERT(stdoutRedirector.isOutputReady());
            ASSERT(0==stdoutRedirector.compare("Hello\nWorld\nWorld, Hello"));
            stdoutRedirector.disable();
        }

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // WINDOWS DEBUG MESSAGE SINK SUB-PROCESS
        //   This test is automatically run as a sub-process by the
        //   'WindowsDebugMessageSink' test case to assist with sub-process
        //   interaction.  It should not be run manually.
        //
        // Concerns:
        //: 1 TBD
        //:
        //
        // Plan:
        //: 1 TBD
        //:
        //
        // Testing:
        //   WINDOWS DEBUG MESSAGE SINK [SUB-PROCESS]
        // --------------------------------------------------------------------
        if (verbose) {
            printf("\nWINDOWS DEBUG MESSAGE SINK SUB-PROCESS"
                   "\n======================================\n");
        }

        puts("\nWARNING: Case '-1' should not be run manually.\n");

#ifdef BSLS_PLATFORM_OS_WINDOWS
        // TBD
#endif
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // PLATFORM DEFAULT MESSAGE HANDLER SUB-PROCESS
        //   This test is automatically run as a sub-process by the
        //   'platformDefaultMessageHandler' test case to allow the main test
        //   case to act as a debugger.  This case should not be run manually.
        //   Ensure that this case is in sync with 'case 6'.
        //
        // Concerns:
        //: 1 [See 'platformDefaultMessageHandler' test.]
        //:
        //
        // Plan:
        //: 1 Get spawned by test driver.
        //:
        //: 2 Delete 'stderr' handler so that 'platformDefaultMessageHandler'
        //:   thinks we are in non-console mode.
        //:
        //: 3 Open the 'BSLS_LOG_TEST' event.
        //:
        //: 4 For each element in 'DEFAULT_DATA', wait for the 'BSLS_LOG_TEST'
        //:   event to be set, and call 'platformDefaultMessageHandler' with
        //:   the input values.
        //:
        //: 5 Fill the large-data buffer using 'fillBuffer()'.
        //:
        //: 6 For each length in 'WINDOWS_LARGE_DATA_LENGTHS', wait for the
        //:   'BSLS_LOG_TEST' event and write a string of appropriate expected
        //:   length to 'platformDefaultMessageHandler'.
        //:
        //: 7 Close the event and exit with status '0'.
        //
        // Testing:
        //   PLATFORM DEFAULT MESSAGE HANDLER [SUB-PROCESS]
        // --------------------------------------------------------------------
        if (verbose) {
            printf("\nPLATFORM DEFAULT MESSAGE HANDLER SUB-PROCESS"
                   "\n============================================\n");
        }

        if(verbose) puts("\nWARNING: Case '-2' should not be run manually.\n");

#ifdef BSLS_PLATFORM_OS_WINDOWS
        {
            if(verbose) puts("\nDeleting 'stderr' handle.\n");
            const HANDLE stderrHandle = GetStdHandle(STD_ERROR_HANDLE);
            ASSERT(CloseHandle(stderrHandle));
            ASSERT(SetStdHandle(STD_ERROR_HANDLE, NULL));
            ASSERT(NULL == GetStdHandle(STD_ERROR_HANDLE));

            if(verbose) printf("\nOpening event '%s'.\n",
                                                WINDOWS_SUBPROCESS_EVENT_NAME);
            HANDLE event = OpenEventA(SYNCHRONIZE,
                                      false,
                                      WINDOWS_SUBPROCESS_EVENT_NAME);

            ASSERT(event);

            if(verbose) puts("\nWriting default data.\n");
            for(size_t i = 0; i < NUM_DEFAULT_DATA; ++i) {
                const int          SOURCE_LINE
                                    = DEFAULT_DATA[i].d_sourceLine;

                const char * const FILE
                                    = DEFAULT_DATA[i].d_file;

                const int          LINE
                                    = DEFAULT_DATA[i].d_line;

                const char * const MESSAGE
                                    = DEFAULT_DATA[i].d_message;

                const char * const EXPECTED
                                    = DEFAULT_DATA[i].d_expected;

                if(veryVerbose) {
                    T_
                    P_(SOURCE_LINE)
                    P_(i)
                    P_(FILE)
                    P_(LINE)
                    P_(MESSAGE)
                    P(EXPECTED)
                }

                if(veryVerbose) puts("\tWaiting on event, 10s timeout.\n");
                ASSERT(WAIT_OBJECT_0 == WaitForSingleObject(event, 10000));

                if(veryVerbose) puts("\tCalling handler.\n");
                bsls::Log::platformDefaultMessageHandler(FILE, LINE, MESSAGE);
            }

            if(verbose) puts("\nFinished with default data.\n");
            if(verbose) puts("\nWriting long data.\n");
            if(verbose) puts("\nFilling buffer.\n");
            fillBuffer(WINDOWS_LARGE_DATA_BUFFER,
                                               WINDOWS_LARGE_DATA_BUFFER_SIZE);
            if(veryVeryVerbose)
                printf("\nFilled buffer: %s\n", WINDOWS_LARGE_DATA_BUFFER);

            for(size_t i = 0; i < NUM_WINDOWS_LARGE_DATA_LENGTHS; i++) {
                const size_t expectedLength = WINDOWS_LARGE_DATA_LENGTHS[i];

                if(veryVerbose) { T_ P_(i) P(expectedLength) }

                if(veryVerbose) puts("\tCalculating index.\n");
                const size_t indexIntoLargeDataBuffer =
                                                 WINDOWS_LARGE_DATA_BUFFER_SIZE
                                                 + 3
                                                 - expectedLength;

                if(veryVerbose) {
                    T_ P(indexIntoLargeDataBuffer)
                }

                if(veryVerbose) puts("\tWaiting on event, 10s timeout.\n");
                ASSERT(WAIT_OBJECT_0 == WaitForSingleObject(event, 10000));

                if(veryVerbose) puts("\tCalling handler.\n");
                bsls::Log::platformDefaultMessageHandler(
                         "",
                         0,
                         WINDOWS_LARGE_DATA_BUFFER + indexIntoLargeDataBuffer);
            }

            if(verbose) puts("\nDone with large lengths.\n");

            if(verbose) puts("\nClosing event handle.\n");
            CloseHandle(event);
        }
#endif
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
