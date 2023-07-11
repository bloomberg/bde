// bdls_filesystemutil_windowsimputil.t.cpp                           -*-C++-*-
#include <bdls_filesystemutil_windowsimputil.h>

#include <bslim_testutil.h>

#include <bsla_maybeunused.h>

#include <bslmf_assert.h>
#include <bslmf_enableif.h>
#include <bslmf_isintegral.h>

#include <bsls_platform.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_deque.h>
#include <bsl_iostream.h>

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#define U_PLATFORM_IS_NATIVE 1
#else
#define U_PLATFORM_IS_NATIVE 0
#endif
    // The macro 'U_PLATFORM_IS_NATIVE' is defined to be 1 if the current
    // platform is Windows, and 0 otherwise.

#if U_PLATFORM_IS_NATIVE
#include <windows.h>
#endif

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test provides a class template,
// 'bdls::FilesystemUtil_WindowsImpUtil', that acts as a namespace template for
// a suite of functions that provide file-system operations on top of those
// provided by its class-type template parameter, which must be a namespace
// struct that provides a suite of primitive file-system operations.  Thus, in
// order to test this component, we will supply
// 'bdls::FilesystemUtil_WindowsImpUtil' with synthetic primitive file-system
// operations that record all input and output, in order to verify this
// component correctly operates a Windows system.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 1] Offset getFileSize(FileDescriptor);
// [ 2] int getLastModificationTime(bdlt::Datetime *, FileDescriptor);
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {
using namespace BloombergLP;
using namespace bsl;

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define ASSERT_EQ(X,Y) ASSERTV(X,Y,X == Y)
#define ASSERT_NE(X,Y) ASSERTV(X,Y,X != Y)

#define LOOP1_ASSERT_EQ(L,X,Y) ASSERTV(L,X,Y,X == Y)
#define LOOP1_ASSERT_NE(L,X,Y) ASSERTV(L,X,Y,X != Y)

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                           ENTITIES FOR TESTING
// ----------------------------------------------------------------------------

#if !U_PLATFORM_IS_NATIVE

namespace BloombergLP {
namespace {
namespace u {

// FORWARD DECLARATIONS
struct FiletimeImp;
struct SystemtimeImp;
union  UlargeIntegerImp;

}  // close namespace u
}  // close unnamed namespace
}  // close enterprise namespace

#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)

typedef int                                      BOOL;
typedef bsl::uint32_t                            DWORD;
typedef ::BloombergLP::bsls::Types::Uint64       ULONGLONG;
typedef struct ::BloombergLP::u::FiletimeImp     FILETIME;
typedef void                                    *HANDLE;
typedef ::BloombergLP::bsls::Types::Int64        INT64;
#if defined(BSLS_PLATFORM_CPU_64_BIT)
typedef bsls::Types::Int64                       LONG_PTR;
#elif defined(BSLS_PLATFORM_CPU_32_BIT)
typedef bsl::int32_t                             LONG_PTR;
#else
typedef bsls::Types::Int64                       LONG_PTR;
#endif
typedef bsl::uint32_t                           *LPDWORD;
typedef struct ::BloombergLP::u::FiletimeImp    *LPFILETIME;
typedef struct ::BloombergLP::u::SystemtimeImp  *LPSYSTEMTIME;
typedef struct ::BloombergLP::u::SystemtimeImp   SYSTEMTIME;
typedef union ::BloombergLP::u::UlargeIntegerImp ULARGE_INTEGER;
typedef ::BloombergLP::bsls::Types::Uint64       ULONG64;
typedef unsigned short                           WORD;

namespace BloombergLP {
namespace {
namespace u {

                             // ==================
                             // struct FiletimeImp
                             // ==================

struct FiletimeImp {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
};

                            // ====================
                            // struct SystemtimeImp
                            // ====================

struct SystemtimeImp {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds; /* Why is this one plural in the Win32 API spec? */
};

                            // ======================
                            // union UlargeIntegerImp
                            // ======================

#ifndef DUMMYSTRUCTNAME
#if defined(NONAMELESSUNION) || !defined(_MSC_EXTENSIONS)
#define DUMMYSTRUCTNAME  s
#else
#define DUMMYSTRUCTNAME
#endif
#endif // DUMMYSTRUCTNAME

union UlargeIntegerImp {
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } DUMMYSTRUCTNAME;
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } u;
    ULONGLONG QuadPart;
};

}  // close namespace u
}  // close unnamed namespace
}  // close enterprise namespace

#endif

namespace BloombergLP {
namespace {
namespace u {

                        // ============================
                        // struct TestWindowsFunctionId
                        // ============================

struct TestWindowsFunctionId {
    // This 'struct' provides a namespace for enumerating a set of identifiers
    // that denote some functions provided by Windows.

    // TYPES
    enum Enum {
        e_FILE_TIME_TO_SYSTEM_TIME,
        e_SYSTEM_TIME_TO_FILE_TIME,
        e_GET_FILE_SIZE,
        e_GET_FILE_TIME,
        e_GET_LAST_ERROR
    };
};

                      // ===============================
                      // struct TestWindowsInterfaceCall
                      // ===============================

struct TestWindowsInterfaceCall {
    // This in-core, aggregate-like 'struct' provides a representation of the
    // arguments of a call to a Windows function.

    // TYPES
    typedef TestWindowsFunctionId FunctionId;
        // 'FunctionId' is an alias to an enumeration for a set of identifiers
        // that denote some functions provided by Windows.

    struct FileTimeToSystemTimeCall {
        // PUBLIC DATA
        const FILETIME *d_lpFileTime;
        LPSYSTEMTIME    d_lpSystemTime;
    };

    struct GetFileSizeCall {
        // PUBLIC DATA
        HANDLE  d_hFile;
        LPDWORD d_lpFileSizeHigh;
    };

    struct GetFileTimeCall {
        // PUBLIC DATA
        HANDLE     d_hFile;
        LPFILETIME d_lpCreationTime;
        LPFILETIME d_lpLastAccessTime;
        LPFILETIME d_lpLastWriteTime;
    };

    struct GetLastErrorCall {
    };

    struct SystemTimeToFileTimeCall {
        // PUBLIC DATA
        const SYSTEMTIME *d_lpSystemTime;
        LPFILETIME        d_lpFileTime;
    };

    // PUBLIC DATA
    FunctionId::Enum d_functionId;
    union {
        FileTimeToSystemTimeCall d_fileTimeToSystemTime;
        GetFileSizeCall          d_getFileSize;
        GetFileTimeCall          d_getFileTime;
        GetLastErrorCall         d_getLastError;
        SystemTimeToFileTimeCall d_systemTimeToFileTime;
    };

    // CREATORS
    TestWindowsInterfaceCall();
        // Create a 'TestWindowsInterfaceCall' object have indeterminate value.

    TestWindowsInterfaceCall(const TestWindowsInterfaceCall& other);
        // Create a 'TestWindowsInterfaceCall' object having the same value as
        // the specified 'other'.  The behavior is undefined if 'other' has
        // indeterminate value or the active member of its union does not
        // correspond to the value of its 'd_functionId' data member.

    // MANIPULATORS
    BSLA_MAYBE_UNUSED
    TestWindowsInterfaceCall& operator=(const TestWindowsInterfaceCall& other);
        // Assign to this object the specified 'other' value and return a
        // reference to this object.  The behavior is undefined if 'other' has
        // indeterminate value or the active member of its union does not
        // correspond to the value of its 'd_functionId' data member.
};

                    // ===================================
                    // struct TestWindowsInterfaceResponse
                    // ===================================

struct TestWindowsInterfaceResponse {
    // This in-core, aggregate-like 'struct' provides a representation of the
    // results of a call to a Windows function.

    // TYPES
    typedef TestWindowsFunctionId FunctionId;
        // 'FunctionId' is an alias to an enumeration for a set of identifiers
        // that denote some functions provided by Windows.

    struct FileTimeToSystemTimeResponse {
        // PUBLIC DATA
        SYSTEMTIME d_lpSystemTime;
        BOOL       d_result;
    };

    struct GetFileSizeResponse {
        // PUBLIC DATA
        DWORD d_lpFileSizeHigh;
        DWORD d_result;
    };

    struct GetFileTimeResponse {
        // PUBLIC DATA
        FILETIME d_lpCreationTime;
        FILETIME d_lpLastAccessTime;
        FILETIME d_lpLastWriteTime;
        BOOL     d_result;
    };

    struct GetLastErrorResponse {
        // PUBLIC DATA
        DWORD d_result;
    };

    struct SystemTimeToFileTimeResponse {
        // PUBLIC DATA
        FILETIME d_lpFileTime;
        BOOL     d_result;
    };

    // PUBLIC DATA
    FunctionId::Enum d_functionId;
    union {
        FileTimeToSystemTimeResponse d_fileTimeToSystemTime;
        GetFileSizeResponse          d_getFileSize;
        GetFileTimeResponse          d_getFileTime;
        GetLastErrorResponse         d_getLastError;
        SystemTimeToFileTimeResponse d_systemTimeToFileTime;
    };

    // CREATORS
    TestWindowsInterfaceResponse();
        // Create a 'TestWindowsInterfaceResponse' object having indeterminate
        // value.

    TestWindowsInterfaceResponse(const TestWindowsInterfaceResponse& other);
        // Create a 'TestWindowsInterfaceResponse' object having the same value
        // as the specified 'other'.  The behavior is undefined if 'other' has
        // indeterminate value or the active member of its union does not
        // correspond to the value of its 'd_functionId' data member.

    // MANIPULATORS
    BSLA_MAYBE_UNUSED
    TestWindowsInterfaceResponse& operator=(
                                    const TestWindowsInterfaceResponse& other);
        // Assign to this object the specified 'other' value and return a
        // reference to this object.  The behavior is undefined if 'other' has
        // indeterminate value or the active member of its union does not
        // correspond to the value of its 'd_functionId' data member.
};

                         // ==========================
                         // class TestWindowsInterface
                         // ==========================

class TestWindowsInterface {
    // This mechanism class provides a set of member functions that mock
    // corresponding Windows functions.  It records the arguments to these
    // functions in a queue, which clients retrieve with 'popFrontCall', and
    // returns results from a queue that clients populate with
    // 'pushFrontResponse'.

  public:
    // TYPES
    typedef TestWindowsInterfaceCall     Call;
        // 'Call' is an alias to an in-core, aggregate-like 'struct' that
        // provides a representation of the arguments of a call to a Windows
        // function.

    typedef TestWindowsInterfaceResponse Response;
        // 'Response' is an alias to an in-core, aggregate-like 'struct' that
        // provides a representation of the results of a call to a Windows
        // function.

    typedef bsl::allocator<char> allocator_type;
        // 'allocator_type' is an alias to the type of allocator that supplies
        // memory to 'TestTransitionalUnixInterface' objects.

  private:
    // PRIVATE TYPES
    typedef TestWindowsFunctionId FunctionId;
        // 'FunctionId' is an alias to an enumeration for a set of identifiers
        // that denote some functions provided by Windows.

    // DATA
    bsl::deque<Call>     d_calls;
        // queue of call records that mock Windows functions populate and
        // 'popFrontCall' drains

    bsl::deque<Response> d_responses;
        // queue of response records that 'pushBackResponse' populates and
        // mock Windows functions drain

    // NOT IMPLEMENTED
    TestWindowsInterface(const TestWindowsInterface&);
    TestWindowsInterface& operator=(const TestWindowsInterface&);

  public:
    // CREATORS
    TestWindowsInterface();
    BSLA_MAYBE_UNUSED
    explicit TestWindowsInterface(const allocator_type& allocator);
        // Create a 'TestWindowsInterface' object that has empty call
        // and response queues.  Optionally specify an 'allocator' used to
        // supply memory; otherwise, the default allocator is used.

    // MANIPULATORS
    BOOL FileTimeToSystemTime(const FILETIME *lpFileTime,
                              LPSYSTEMTIME    lpSystemTime);
        // Push a 'Call' to the call queue that has a
        // 'FunctionId::e_FILE_TIME_TO_SYSTEM_TIME' 'd_functionId'.  Return the
        // 'd_result' of the next queued response.  The behavior is undefined
        // if the response queue is empty or the 'd_functionId' of the next
        // queued response is not 'FunctionId::e_GET_LAST_ERROR'.

    DWORD GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);
        // Push a 'Call' to the call queue that has a
        // 'FunctionId::e_GET_FILE_SIZE' 'd_functionId', a 'd_hFile' equal to
        // the specified 'hFile', and a 'd_lpFileSizeHigh' equal to the
        // specified 'lpFileSizeHigh'.  Load the 'd_lpFileSizeHigh' into
        // 'lpFileSizeHigh' and return the 'd_result' of the next queued
        // response.  The behavior is undefined if the response queue is empty
        // or the 'd_functionId' of the next queued response is not
        // 'FunctionId::e_GET_FILE_SIZE'.

    BOOL GetFileTime(HANDLE     hFile,
                     LPFILETIME lpCreationTime,
                     LPFILETIME lpLastAccessTime,
                     LPFILETIME lpLastWriteTime);

    DWORD GetLastError();
        // Push a 'Call' to the call queue that has a
        // 'FunctionId::e_GET_LAST_ERROR' 'd_functionId'.  Return the
        // 'd_result' of the next queued response.  The behavior is undefined
        // if the response queue is empty or the 'd_functionId' of the next
        // queued response is not 'FunctionId::e_GET_LAST_ERROR'.

    BOOL SystemTimeToFileTime(const SYSTEMTIME *lpSystemTime,
                              LPFILETIME        lpFileTime);
        // Push a 'Call' to the call queue that has a
        // 'FunctionId::e_SYSTEM_TIME_TO_FILE_TIME' 'd_functionId', a
        // 'd_lpSystemTime' equal to the specified 'lpSystemTime', and a
        // 'd_lpFileTime' equal to the specified 'lpFileTime'.  Load the
        // 'd_lpFileTime' into the 'lpFileTime' and return the 'd_result' of
        // the next queued response.  The behavior is undefined if the response
        // queue is empty or the 'd_functionId' of the next queued response is
        // not 'FunctionId::e_GET_LAST_ERROR'.

    void popFrontCall(Call *call);
        // Load the next queued call into the specified 'call' and remove it
        // from the queue.  The behavior is undefined if the call queue is
        // empty.

    void pushBackResponse(const Response& response);
        // Push the specified 'response' to the response queue.

    // ACCESSORS
    BSLA_MAYBE_UNUSED
    int numCalls() const;
        // Return the number of calls in the call queue.

    BSLA_MAYBE_UNUSED
    int numResponses() const;
        // Return the number of responses in the response queue.
};

                      // ===============================
                      // struct TestWindowsInterfaceUtil
                      // ===============================

struct TestWindowsInterfaceUtil {
    // This utility 'struct' provides an implementation of the requirements for
    // the 'WINDOWS_INTERFACE' template parameter of the functions provided by
    // 'FilesystemUtil_WindowsImpUtil' in terms of mock Windows calls.

    // TYPES
    typedef ::BOOL BOOL;
        // 'BOOL' is an alias to the unsigned integral 'BOOL' type provided
        // by the 'windows.h' header.

    typedef ::DWORD DWORD;
        // 'DWORD' is an alias to the unsigned integral 'DWORD' type provided
        // by the 'windows.h' header.

    typedef ::FILETIME FILETIME;
        // 'FILETIME' is an alias to the 'FILETIME' struct provided by the
        // 'windows.h' header.

    typedef ::HANDLE HANDLE;
        // 'HANDLE' is an alias to the 'HANDLE' type provided by the
        // 'windows.h' header.

    typedef ::INT64 INT64;
        // 'INT64' is an alias to the signed integral 'INT64' type provided by
        // the 'windows.h' header.

    typedef ::LPDWORD LPDWORD;
        // 'LPDWORD' is an alias to the 'LPDWORD' type provided by the
        // 'windows.h' header.

    typedef ::LPFILETIME LPFILETIME;
        // 'LPFILETIME' is an alias to the 'LPFILETIME' type provided by the
        // 'windows.h' header.

    typedef ::LPSYSTEMTIME LPSYSTEMTIME;
        // 'LPSYSTEMTIME' is an alias to the 'LPSYSTEMTIME' type provided by
        // the 'windows.h' header.

    typedef ::SYSTEMTIME SYSTEMTIME;
        // 'SYSTEMTIME' is an alias to the 'SYSTEMTIME' struct provided by the
        // 'windows.h' header.

    typedef ::ULARGE_INTEGER ULARGE_INTEGER;
        // 'ULARGE_INTEGER' is an alias to the unsigned integral
        // 'ULARGE_INTEGER' type provided by the 'windows.h' header.

    typedef ::ULONG64 ULONG64;
        // 'ULONG64' is an alias to the unsigned integral 'ULONG64' type
        // provided by the 'windows.h' header.

    typedef ::ULONGLONG ULONGLONG;
        // 'ULONGLONG' is an alias to the unsigned integral 'ULONGLONG' type
        // provided by the 'windows.h' header.

    typedef ::WORD WORD;
        // 'WORD' is an alias to the unsigned integral 'WORD' type provided by
        // the 'windows.h' header.
  private:
    // CLASS DATA
    static TestWindowsInterface *s_interface_p;
        // the currently-installed mock Windows interface mechanism

  public:
    // CLASS METHODS
    static BOOL FileTimeToSystemTime(const FILETIME *lpFileTime,
                                     LPSYSTEMTIME    lpSystemTime);
        // Push a 'Call' to the interface's call queue that has a
        // 'FunctionId::e_FILE_TIME_TO_SYSTEM_TIME' 'd_functionId', a
        // 'd_lpFileTime' equal to the specified 'lpFileTime', and a
        // 'd_lpSystemTime' equal to the specified 'lpSystemTime'.  Load the
        // 'd_lpSystemTime' into the 'lpSystemTime' and return the 'd_result'
        // of the interface's next queued response.  The behavior is undefined
        // if the response queue is empty or the next queued response is not
        // 'FunctionId::e_FILE_TIME_TO_SYSTEM_TIME'.

    static DWORD GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);
        // Push a 'Call' to the interface's call queue that has a
        // 'FunctionId::e_GET_FILE_SIZE' 'd_functionId', a 'd_hFile' equal to
        // the specified 'hFile', and a 'd_lpFileSizeHigh' equal to the
        // specified 'lpFileSizeHigh'.  Load the 'd_lpFileSizeHigh' into
        // 'lpFileSizeHigh' and return the 'd_result' of the interface's next
        // queued response.  The behavior is undefined if the response queue is
        // empty or the 'd_functionId' of the next queued response is not
        // 'FunctionId::e_GET_FILE_SIZE'.

    static BOOL GetFileTime(HANDLE     hFile,
                            LPFILETIME lpCreationTime,
                            LPFILETIME lpLastAccessTime,
                            LPFILETIME lpLastWriteTime);
        // Push a 'Call' to the interface's call queue that has a
        // 'FunctionId::e_GET_FILE_TIME' 'd_functionId', a 'd_hFile' equal to
        // the specified 'hFile', a 'd_lpCreationTime' equal to the specified
        // 'lpCreationTime', a 'd_lpLastAccessTime' equal to the specified
        // 'lpLastAccessTime', and a 'd_lpLastWriteTime' equal to the specified
        // 'lpLastWriteTime'.  Load the 'd_lpCreationTime' to the
        // 'lpCreatinTime', the 'd_lpLastAccessTime' to the 'lpLastAccessTime',
        // and the 'd_lpLastWriteTime' to the 'lpLastWriteTime' and return the
        // 'd_result' of the interface's next queued response.  The behavior is
        // undefined if the response queue is empty or the 'd_functionId' of
        // the next queued response is not 'FunctionId::e_GET_FILE_TIME'.

    static DWORD GetLastError();
        // Push a 'Call' to the interface's call queue that has a
        // 'FunctionId::e_GET_LAST_ERROR' 'd_functionId'.  Return the
        // 'd_result' of the interface's next queued response.  The behavior is
        // undefined if the response queue is empty or the 'd_functionId' of
        // the next queued response is not 'FunctionId::e_GET_LAST_ERROR'.

    static BOOL SystemTimeToFileTime(const SYSTEMTIME *lpSystemTime,
                                     LPFILETIME        lpFileTime);
        // Push a 'Call' to the interface's call queue that has a
        // 'FunctionId::e_SYSTEM_TIME_TO_FILE_TIME' 'd_functionId', a
        // 'd_lpFileTime' equal to the specified 'lpFileTime', and a
        // 'd_lpSystemTime' equal to the specified 'lpSystemTime'.  Load the
        // 'd_lpFileTime' into the 'lpFileTime' and return the 'd_result' of
        // the interface's next queued response.  The behavior is undefined if
        // the response queue is empty or the next queued response is not
        // 'FunctionId::e_SYSTEM_TIME_TO_FILE_TIME'.

    static void setInterface(TestWindowsInterface *interface);
        // Set the interface to the specified 'interface'.

    BSLA_MAYBE_UNUSED
    static TestWindowsInterface *interface();
        // Return the interface.
};

                        // =======================
                        // class IntegralConverter
                        // =======================

template <class INTEGRAL_TYPE>
class IntegralConverter {
    // This function-object class template provides a function call operator
    // that performs an integral conversion (or promotion if a conversion is
    // unnecessary) from an arbitrary integral type to the specified
    // 'INTEGRAL_TYPE'.  The behavior is undefined unless 'INTEGRAL_TYPE' is an
    // integral type.

  public:
    // TRAITS
    BSLMF_ASSERT((bsl::is_integral<INTEGRAL_TYPE>::value));

    // TYPES
    typedef INTEGRAL_TYPE ResultType;
        // 'ResultType' is an alias to the type returned by this class
        // template's function-call operator.

    // CREATORS
    IntegralConverter();
        // Create a new 'IntegralConverter' object.

    // ACCESSORS
    template <class SOURCE_INTEGRAL_TYPE>
    typename bsl::enable_if<
        bsl::is_integral<SOURCE_INTEGRAL_TYPE>::value,
        ResultType
    >::type
    operator()(SOURCE_INTEGRAL_TYPE value) const;
        // Return the result of converting (or promoting, if a conversion is
        // unnecessary) the specified 'value' to 'INTEGRAL_TYPE'.  This
        // function does not participate in overload resolution unless the
        // specified 'SOURCE_INTEGRAL_TYPE' is an integral type.
};

// ============================================================================
//                          DEFINITIONS FOR TESTING
// ----------------------------------------------------------------------------

                      // -------------------------------
                      // struct TestWindowsInterfaceCall
                      // -------------------------------

// CREATORS
TestWindowsInterfaceCall::TestWindowsInterfaceCall()
{
}

TestWindowsInterfaceCall::TestWindowsInterfaceCall(
                                         const TestWindowsInterfaceCall& other)
: d_functionId(other.d_functionId)
{
    switch (other.d_functionId) {
      case FunctionId::e_FILE_TIME_TO_SYSTEM_TIME: {
        d_fileTimeToSystemTime = other.d_fileTimeToSystemTime;
      } break;
      case FunctionId::e_SYSTEM_TIME_TO_FILE_TIME: {
        d_systemTimeToFileTime = other.d_systemTimeToFileTime;
      } break;
      case FunctionId::e_GET_FILE_SIZE: {
        d_getFileSize = other.d_getFileSize;
      } break;
      case FunctionId::e_GET_FILE_TIME: {
        d_getFileTime = other.d_getFileTime;
      } break;
      case FunctionId::e_GET_LAST_ERROR: {
        // Do nothing.
      } break;
    }
}

// MANIPULATORS
TestWindowsInterfaceCall& TestWindowsInterfaceCall::operator=(
                                         const TestWindowsInterfaceCall& other)
{
    d_functionId = other.d_functionId;

    switch (other.d_functionId) {
      case FunctionId::e_FILE_TIME_TO_SYSTEM_TIME: {
        d_fileTimeToSystemTime = other.d_fileTimeToSystemTime;
      } break;
      case FunctionId::e_SYSTEM_TIME_TO_FILE_TIME: {
        d_systemTimeToFileTime = other.d_systemTimeToFileTime;
      } break;
      case FunctionId::e_GET_FILE_SIZE: {
        d_getFileSize = other.d_getFileSize;
      } break;
      case FunctionId::e_GET_FILE_TIME: {
        d_getFileTime = other.d_getFileTime;
      } break;
      case FunctionId::e_GET_LAST_ERROR: {
        // Do nothing.
      } break;
    }

    return *this;
}

                    // -----------------------------------
                    // struct TestWindowsInterfaceResponse
                    // -----------------------------------

// CREATORS
TestWindowsInterfaceResponse::TestWindowsInterfaceResponse()
{
};

TestWindowsInterfaceResponse::TestWindowsInterfaceResponse(
                                     const TestWindowsInterfaceResponse& other)
: d_functionId(other.d_functionId)
{
    switch (other.d_functionId) {
      case FunctionId::e_FILE_TIME_TO_SYSTEM_TIME: {
        d_fileTimeToSystemTime = other.d_fileTimeToSystemTime;
      } break;
      case FunctionId::e_SYSTEM_TIME_TO_FILE_TIME: {
        d_systemTimeToFileTime = other.d_systemTimeToFileTime;
      } break;
      case FunctionId::e_GET_FILE_SIZE: {
        d_getFileSize = other.d_getFileSize;
      } break;
      case FunctionId::e_GET_FILE_TIME: {
        d_getFileTime = other.d_getFileTime;
      } break;
      case FunctionId::e_GET_LAST_ERROR: {
        d_getLastError = other.d_getLastError;
      } break;
    }
}

// MANIPULATORS
TestWindowsInterfaceResponse& TestWindowsInterfaceResponse::operator=(
                                     const TestWindowsInterfaceResponse& other)
{
    d_functionId = other.d_functionId;

    switch (other.d_functionId) {
      case FunctionId::e_FILE_TIME_TO_SYSTEM_TIME: {
          d_fileTimeToSystemTime = other.d_fileTimeToSystemTime;
      } break;
      case FunctionId::e_SYSTEM_TIME_TO_FILE_TIME: {
          d_systemTimeToFileTime = other.d_systemTimeToFileTime;
      } break;
      case FunctionId::e_GET_FILE_SIZE: {
          d_getFileSize = other.d_getFileSize;
      } break;
      case FunctionId::e_GET_FILE_TIME: {
          d_getFileTime = other.d_getFileTime;
      } break;
      case FunctionId::e_GET_LAST_ERROR: {
          d_getLastError = other.d_getLastError;
      } break;
    }

    return *this;
}


                         // --------------------------
                         // class TestWindowsInterface
                         // --------------------------

// CREATORS
TestWindowsInterface::TestWindowsInterface()
: d_calls()
, d_responses()
{
}

TestWindowsInterface::TestWindowsInterface(const allocator_type& allocator)
: d_calls(allocator.mechanism())
, d_responses(allocator.mechanism())
{
}

// MANIPULATORS
BOOL TestWindowsInterface::FileTimeToSystemTime(const FILETIME *lpFileTime,
                                                LPSYSTEMTIME    lpSystemTime)
{
    Call call;
    call.d_functionId = FunctionId::e_FILE_TIME_TO_SYSTEM_TIME;
    call.d_fileTimeToSystemTime.d_lpFileTime   = lpFileTime;
    call.d_fileTimeToSystemTime.d_lpSystemTime = lpSystemTime;
    d_calls.push_back(call);

    BSLS_ASSERT(0 < d_responses.size());
    const Response& response = d_responses.front();
    BSLS_ASSERT(FunctionId::e_FILE_TIME_TO_SYSTEM_TIME ==
                response.d_functionId);
    const BOOL result = response.d_fileTimeToSystemTime.d_result;
    *lpSystemTime     = response.d_fileTimeToSystemTime.d_lpSystemTime;
    d_responses.pop_front();
    return result;
}

DWORD TestWindowsInterface::GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
{
    Call call;
    call.d_functionId                   = FunctionId::e_GET_FILE_SIZE;
    call.d_getFileSize.d_hFile          = hFile;
    call.d_getFileSize.d_lpFileSizeHigh = lpFileSizeHigh;
    d_calls.push_back(call);

    BSLS_ASSERT(0 < d_responses.size());
    const Response& response = d_responses.front();
    BSLS_ASSERT(FunctionId::e_GET_FILE_SIZE == response.d_functionId);
    const DWORD result = response.d_getFileSize.d_result;
    *lpFileSizeHigh    = response.d_getFileSize.d_lpFileSizeHigh;
    d_responses.pop_front();
    return result;
}

BOOL TestWindowsInterface::GetFileTime(HANDLE     hFile,
                                       LPFILETIME lpCreationTime,
                                       LPFILETIME lpLastAccessTime,
                                       LPFILETIME lpLastWriteTime)
{
    Call call;
    call.d_functionId = FunctionId::e_GET_FILE_TIME;
    call.d_getFileTime.d_hFile            = hFile;
    call.d_getFileTime.d_lpCreationTime   = lpCreationTime;
    call.d_getFileTime.d_lpLastAccessTime = lpLastAccessTime;
    call.d_getFileTime.d_lpLastWriteTime  = lpLastWriteTime;
    d_calls.push_back(call);

    BSLS_ASSERT(0 < d_responses.size());
    const Response& response = d_responses.front();
    BSLS_ASSERT(FunctionId::e_GET_FILE_TIME == response.d_functionId);
    const BOOL result = response.d_getFileTime.d_result;
    if (0 != lpCreationTime) {
        *lpCreationTime = response.d_getFileTime.d_lpCreationTime;
    }
    if (0 != lpLastAccessTime) {
        *lpLastAccessTime = response.d_getFileTime.d_lpLastAccessTime;
    }
    if (0 != lpLastWriteTime) {
        *lpLastWriteTime = response.d_getFileTime.d_lpLastWriteTime;
    }
    d_responses.pop_front();
    return result;
}

DWORD TestWindowsInterface::GetLastError()
{
    Call call;
    call.d_functionId = FunctionId::e_GET_LAST_ERROR;
    d_calls.push_back(call);

    BSLS_ASSERT(0 < d_responses.size());
    const Response& response = d_responses.front();
    BSLS_ASSERT(FunctionId::e_GET_LAST_ERROR == response.d_functionId);
    const DWORD result = response.d_getLastError.d_result;
    d_responses.pop_front();
    return result;
}

BOOL TestWindowsInterface::SystemTimeToFileTime(const SYSTEMTIME *lpSystemTime,
                                                LPFILETIME        lpFileTime)
{
    Call call;
    call.d_functionId = FunctionId::e_SYSTEM_TIME_TO_FILE_TIME;
    call.d_systemTimeToFileTime.d_lpFileTime   = lpFileTime;
    call.d_systemTimeToFileTime.d_lpSystemTime = lpSystemTime;
    d_calls.push_back(call);

    BSLS_ASSERT(0 < d_responses.size());
    const Response& response = d_responses.front();
    BSLS_ASSERT(FunctionId::e_SYSTEM_TIME_TO_FILE_TIME ==
                response.d_functionId);
    const BOOL result = response.d_systemTimeToFileTime.d_result;
    *lpFileTime       = response.d_systemTimeToFileTime.d_lpFileTime;
    d_responses.pop_front();
    return result;
}

void TestWindowsInterface::popFrontCall(Call *call)
{
    BSLS_ASSERT(0 < d_calls.size());
    *call = d_calls.front();
    d_calls.pop_front();
}

void TestWindowsInterface::pushBackResponse(const Response& response)
{
    d_responses.push_back(response);
}

// ACCESSORS
int TestWindowsInterface::numCalls() const
{
    return static_cast<int>(d_calls.size());
}

int TestWindowsInterface::numResponses() const
{
    return static_cast<int>(d_responses.size());
}

                      // -------------------------------
                      // struct TestWindowsInterfaceUtil
                      // -------------------------------

// CLASS DATA
TestWindowsInterface *TestWindowsInterfaceUtil::s_interface_p = 0;

// CLASS METHODS
BSLA_MAYBE_UNUSED
BOOL TestWindowsInterfaceUtil::FileTimeToSystemTime(
                                                  const FILETIME *lpFileTime,
                                                  LPSYSTEMTIME    lpSystemTime)
{
    return s_interface_p->FileTimeToSystemTime(lpFileTime, lpSystemTime);
}

DWORD TestWindowsInterfaceUtil::GetFileSize(HANDLE  hFile,
                                            LPDWORD lpFileSizeHigh)
{
    return s_interface_p->GetFileSize(hFile, lpFileSizeHigh);
}

BOOL TestWindowsInterfaceUtil::GetFileTime(HANDLE     hFile,
                                           LPFILETIME lpCreationTime,
                                           LPFILETIME lpLastAccessTime,
                                           LPFILETIME lpLastWriteTime){
    return s_interface_p->GetFileTime(
        hFile, lpCreationTime, lpLastAccessTime, lpLastWriteTime);
}

DWORD TestWindowsInterfaceUtil::GetLastError()
{
    return s_interface_p->GetLastError();
}

BOOL TestWindowsInterfaceUtil::SystemTimeToFileTime(
                                                const SYSTEMTIME *lpSystemTime,
                                                LPFILETIME        lpFileTime)
{
    return s_interface_p->SystemTimeToFileTime(lpSystemTime, lpFileTime);
}

void TestWindowsInterfaceUtil::setInterface(TestWindowsInterface *interface)
{
    s_interface_p = interface;
}

TestWindowsInterface *TestWindowsInterfaceUtil::interface()
{
    return s_interface_p;
}

                        // -----------------------
                        // class IntegralConverter
                        // -----------------------

// CREATORS
template <class INTEGRAL_TYPE>
IntegralConverter<INTEGRAL_TYPE>::IntegralConverter()
{
}

// ACCESSORS
template <class INTEGRAL_TYPE>
template <class SOURCE_INTEGRAL_TYPE>
typename bsl::enable_if<
    bsl::is_integral<SOURCE_INTEGRAL_TYPE>::value,
    INTEGRAL_TYPE
>::type
IntegralConverter<INTEGRAL_TYPE>::operator()(SOURCE_INTEGRAL_TYPE value) const
{
    return static_cast<INTEGRAL_TYPE>(value);
}

                        // ---------------------------
                        // utility converter functions
                        // ---------------------------

static FILETIME unixEpochFileTimeUtc;

int initEpochFileTime()
    // Utility function to inititalize 'unixEpochFileTimeUtc' used for later
    // tests.  Return 0 on success or non-zero on failure.
{
#if U_PLATFORM_IS_NATIVE
    bdlt::Datetime unixEpochUtc = bdlt::EpochUtil::epoch();

    const SYSTEMTIME unixEpochSystemTimeUtc = {
        static_cast<WORD>(unixEpochUtc.year()),
        static_cast<WORD>(unixEpochUtc.month()),
        static_cast<WORD>(unixEpochUtc.dayOfWeek() - 1),
        static_cast<WORD>(unixEpochUtc.day()),
        static_cast<WORD>(unixEpochUtc.hour() == 24 ? 0
                                                    : unixEpochUtc.hour()),
        static_cast<WORD>(unixEpochUtc.minute()),
        static_cast<WORD>(unixEpochUtc.second()),
        static_cast<WORD>(unixEpochUtc.millisecond())};

    const BOOL systemTimeToFileTimeStatus =
        ::SystemTimeToFileTime(&unixEpochSystemTimeUtc, &unixEpochFileTimeUtc);

    if (!systemTimeToFileTimeStatus) {
        return -1;                                                // RETURN
    }
#else
    // For non-Windows test runs we are forced to hard-code this.
    unixEpochFileTimeUtc.dwLowDateTime = 3577643008UL;
    unixEpochFileTimeUtc.dwHighDateTime = 27111902UL;
#endif
    if ((3577643008UL != unixEpochFileTimeUtc.dwLowDateTime) ||
        (27111902UL != unixEpochFileTimeUtc.dwHighDateTime)) {
        return -1;                                                    // RETURN
    }
    return 0;
}

FILETIME toFT(const bdlt::Datetime& dt, bool doublecheck = true)
    // Utility function to convert the specified 'dt' into a windows 'FILETIME'
    // structure.  The optionally specified 'doublecheck' indicates that
    // additional tests are performed against the native functions when run on
    // Windows.  Return the value generated.
{
    bsls::Types::Int64 microseconds =
                           (dt - bdlt::EpochUtil::epoch()).totalMicroseconds();

    ULARGE_INTEGER unixEpochFileTimeInTicks;
    unixEpochFileTimeInTicks.u.HighPart = unixEpochFileTimeUtc.dwHighDateTime;
    unixEpochFileTimeInTicks.u.LowPart  = unixEpochFileTimeUtc.dwLowDateTime;

    ULARGE_INTEGER requiredFileTime;

    requiredFileTime.QuadPart =
           unixEpochFileTimeInTicks.QuadPart +
           microseconds * bdls::FilesystemUtil_WindowsImpUtil<
                    TestWindowsInterfaceUtil>::k_WINDOWS_TICKS_PER_MICROSECOND;

    FILETIME ret;
    ret.dwHighDateTime = requiredFileTime.u.HighPart;
    ret.dwLowDateTime  = requiredFileTime.u.LowPart;

#if U_PLATFORM_IS_NATIVE
    if (doublecheck) {
        SYSTEMTIME test;
        int        rc = ::FileTimeToSystemTime(&ret, &test);
        ASSERTV(dt, rc);
        ASSERTV(dt.year() == test.wYear);
        ASSERTV(dt.month() == test.wMonth);
        ASSERTV(dt.day() == test.wDay);
        ASSERTV(dt.hour() == test.wHour);
        ASSERTV(dt.minute() == test.wMinute);
        ASSERTV(dt.second() == test.wSecond);
        ASSERTV(dt.millisecond() == test.wMilliseconds);
    }
#else
    (void) doublecheck;
#endif

    return ret;
}

FILETIME toFT(int year,
              int month,
              int day,
              int hour        = 0,
              int minute      = 0,
              int second      = 0,
              int millisecond = 0,
              int microsecond = 0)
    // Utility function to call 'toFT(Datetime)' with a 'Datetime' constructed
    // from the specified 'year', 'month', and 'day', and the optionally
    // specified 'hour', 'minute', 'second', 'millisecond', and 'microsecond'.
    // Return the result fo the call to 'toFT(Datetime)'.
{
    return toFT(bdlt::Datetime(year,
                               month,
                               day,
                               hour,
                               minute,
                               second,
                               millisecond,
                               microsecond),
                true);
}

}  // close namespace u
}  // close unnamed namespace
}  // close enterprise namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    using namespace BloombergLP;

    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    static_cast<void>(verbose);
    static_cast<void>(veryVerbose);
    static_cast<void>(veryVeryVerbose);
    static_cast<void>(veryVeryVeryVerbose);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    using namespace bdls;

    typedef u::TestWindowsFunctionId        FunctionId;
    typedef u::TestWindowsInterfaceCall     Call;
    typedef u::TestWindowsInterfaceResponse Response;
    typedef u::TestWindowsInterface         Interface;
    typedef u::TestWindowsInterfaceUtil     InterfaceUtil;

    typedef FilesystemUtil_WindowsImpUtil<InterfaceUtil> Obj;
    typedef Obj::FileDescriptor                          FileDescriptor;
    typedef Obj::Offset                                  Offset;

    const FileDescriptor INVALID_FD = INVALID_HANDLE_VALUE;

    enum {
        NA = 0
            // a value that this text block uses to indicate that the
            // field is not applicable
    };

    // Use system calls if available to initialize global FILETIME epoch.
    ASSERT(0 == u::initEpochFileTime());

    static const bool F = false;
    static const bool T = true;

    switch(test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'getLastModificationTime'
        //   This case verifies, using a mock Windows interface, that
        //   'getLastModificationTime' correctly converts the values of its
        //   arguments into the values required by the underlying Windows
        //   system operations, converts the results of the operations to
        //   'getLastModificationTime's results, and invokes no undefined
        //   behavior when given or producing values near their limits.
        //
        // Concerns:
        //: 1 Error return codes from 'GetFileTime' or
        //:   'FileTimeToSystemTime' calls result in a non-zero return value.
        //:
        //: 1 Upon success, the value loaded into the Windows system time
        //:   from the file time retrieved via a call to 'GetFileTime'
        //:   is converted to the 'bdlt::Datetime' result that represents the
        //:   same value as the system time, if a 'bdlt::Datetime' is capable
        //:   of representing the value, and a non-zero value is returned
        //:   otherwise.
        //
        // Plan:
        //: 1 Create a table that enumerates several combinations of values for
        //:   'descriptor', as well as return codes and output values to
        //:   use as results from an underlying system calls, where the
        //:   numeric limits of each are present, and values known to be near
        //:   the limits of the corresponding results for
        //:   'getLastModificationTime'.
        //:
        //: 2 For each combination 'C' of the set of combinations, do the
        //:   following:
        //:
        //:   1 Prime a mock Windows interface to expect a call to
        //:     'GetFileTime' with the file descriptor in 'C' and to provide
        //:     the return code and 'FILETIME' struct values from 'C' as
        //:     results to the call to 'GetFileTime'.
        //:
        //:   2 If the 'GetFileTime' call is primed to succeed, prime the
        //:     interface to expect a call to 'FileTimeToSystemTime' with
        //:     expected input and output from 'C'.
        //:
        //:   2 Invoke 'getLastModificationTime' with the file descriptor from
        //:     'C'.
        //:
        //:   3 Verify that the results of the call to
        //:     'getLastModificationTime' satisfy the properties defined by
        //:     each concern relating to the particular result.
        //
        // Testing:
        //   Offset getLastModificationTime(bdlt::Datetime *, FileDescriptor);
        // --------------------------------------------------------------------

        typedef FILETIME       FTime;
        typedef bdlt::Datetime DTime;

        {
            // Initiate the static variables used by
            // 'convertFileTimeToSystemTime'.

            Interface interface;

            FTime unixEpoch = u::unixEpochFileTimeUtc;

            Response systemTimeToFileTimeResponse;
            systemTimeToFileTimeResponse.d_functionId =
                                        FunctionId::e_SYSTEM_TIME_TO_FILE_TIME;
            systemTimeToFileTimeResponse.d_systemTimeToFileTime.d_lpFileTime =
                                                                     unixEpoch;
            systemTimeToFileTimeResponse.d_systemTimeToFileTime.d_result = T;

            interface.pushBackResponse(systemTimeToFileTimeResponse);

            InterfaceUtil::setInterface(&interface);
            bdlt::Datetime time;
            const int result = Obj::convertFileTimeToDatetime(&time,
                                                              &unixEpoch);
            BSLS_ASSERT(0 == result);  (void)result;
            InterfaceUtil::setInterface(0);

            ASSERT(time == bdlt::EpochUtil::epoch());
        }

        enum { JAN = 1, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC };
        enum { MON = 0, TUE, WED, THU, FRI, SAT, SUN };

        const FTime FT_NA = {};
        const DTime DT_NA;

        const struct {
            int            d_line;
                // the line number

            FileDescriptor d_fileDescriptor;
                // the argument to 'getLastModificationTime'

            FILETIME       d_lastWriteTime;
                // result from the mock 'GetFileTime' Call

            BOOL           d_getFileTimeResult;
                // return value from the mock 'GetFileTime' call

            bdlt::Datetime d_time;
                // the expected value of 'time' after the call to
                // 'getLastModificationTime'

            int            d_result;
                // the expected 'getLastModificationTime' value
          } DATA[] = {
              {
                  L_, INVALID_FD,
                  FT_NA, F,
                  DT_NA,                                             -1
              },
              {
                  L_, INVALID_FD,
                  FT_NA, T,
                  DT_NA,                                             -1
              },
              {
                  L_, INVALID_FD,
                  u::toFT( 1980, JAN,       1,  0,  0,  0,   0    ),  F,
                  DT_NA,                                             -1
              },
              {
                  L_, INVALID_FD,
                  u::toFT( 1980, JAN,       1,  0,  0,  0,   0   ),   T,
                  DTime( 1980, JAN,       1,  0,  0,  0,   0,   0),   0
              },
              {
                  L_, INVALID_FD,
                  u::toFT( 1980, JAN,       1, 23, 59, 59, 999   ),   T,
                  DTime( 1980, JAN,       1, 23, 59, 59, 999,   0),   0
              },
              {
                  L_, INVALID_FD,
                  u::toFT( 1980, JAN,       1, 23, 59, 59, 999, 999), T,
                  DTime( 1980, JAN,       1, 23, 59, 59, 999, 999),   0
              },
              {
                  L_, INVALID_FD,
                  u::toFT( 1980, JAN,      24,  0,  0,  0,   0   ),   T,
                  DTime( 1980, JAN,      24,  0,  0,  0,   0,   0),   0
              },
              {
                  L_, INVALID_FD,
                  u::toFT(DTime( 1601, JAN, 1, 0, 0, 0), false),      T,
                  DT_NA,                                             -1
              },
              {
                  L_, INVALID_FD,
                  u::toFT( 9999, DEC,      31, 23, 59, 59, 999,   0), T,
                  DTime( 9999, DEC,      31, 23, 59, 59, 999,   0),   0
              },
              {
                  L_, INVALID_FD,
                  FT_NA, T,
                  DT_NA,                                             -1
              }
          };

          const int NUM_DATA = sizeof DATA / sizeof *DATA;

          for (int i = 0; i != NUM_DATA; ++i) {
              const int            LINE            = DATA[i].d_line;
              const FileDescriptor FILE_DESCRIPTOR = DATA[i].d_fileDescriptor;
                    FILETIME       LAST_WRITE_TIME = DATA[i].d_lastWriteTime;
              const BOOL GET_FILE_TIME_RESULT = DATA[i].d_getFileTimeResult;

              const bdlt::Datetime TIME   = DATA[i].d_time;
              const int            RESULT = DATA[i].d_result;

              Response getFileTimeResponse;
              getFileTimeResponse.d_functionId = FunctionId::e_GET_FILE_TIME;
              getFileTimeResponse.d_getFileTime.d_lpLastWriteTime =
                  LAST_WRITE_TIME;
              getFileTimeResponse.d_getFileTime.d_result =
                  GET_FILE_TIME_RESULT;

              Interface interface;
              interface.pushBackResponse(getFileTimeResponse);

              InterfaceUtil::setInterface(&interface);
              bdlt::Datetime time;
              const int      result =
                  Obj::getLastModificationTime(&time, FILE_DESCRIPTOR);
              InterfaceUtil::setInterface(0);

              LOOP1_ASSERT_EQ(LINE, RESULT, result);

              LOOP1_ASSERT_EQ(LINE, 1, interface.numCalls());
              if (1 != interface.numCalls()) {
                  continue;
              }

              Call getFileTimeCall;
              interface.popFrontCall(&getFileTimeCall);
              LOOP1_ASSERT_EQ(LINE,
                              FunctionId::e_GET_FILE_TIME,
                              getFileTimeCall.d_functionId);
              if (FunctionId::e_GET_FILE_TIME !=
                  getFileTimeCall.d_functionId) {
                  continue;
              }

              LOOP1_ASSERT_EQ(LINE,
                              FILE_DESCRIPTOR,
                              getFileTimeCall.d_getFileTime.d_hFile);

              if (!GET_FILE_TIME_RESULT) {
                  continue;
              }

              if (0 != RESULT) {
                  continue;
              }

              LOOP1_ASSERT_EQ(LINE, TIME, time);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'getFileSize'
        //   This case verifies, using a mock Windows interface, that
        //   'getFileSize' correctly converts the values of its arguments into
        //   the values required by the underlying Windows system operations,
        //   converts the results of the operations to 'getFileSize's results,
        //   and invokes no undefined behavior when given or producing values
        //   near their limits.
        //
        // Concerns:
        //: 1 Error return codes from 'GetFileSize' result in a non-zero return
        //:   value.
        //:
        //: 1 Upon success, the 2 32-bit result values from the call to
        //:   'GetFileSize' are combined into a single, signed 64-bit value
        //:   that is not less than 0, and this value is returned.
        //
        // Plan:
        //: 1 Create a table that enumerates several combinations of values for
        //:   'descriptor', as well as result values for the underlying call to
        //:   'GetFileSize', where the numeric limits of each are present.
        //:
        //: 2 For each combination 'C' of the set of combinations, do the
        //:   following:
        //:
        //:   1 Prime a mock Windows interface to expect a call to
        //:     'GetFileSize' with the file descriptor in 'C', and to provide
        //:     results from C.
        //:
        //:   2 Prime the same interface to expect a call to 'GetLastError' if
        //:     the return code from 'GetFileSize' is primed to indicate a
        //:     possible error.
        //:
        //:   2 Invoke 'getFileSize' with the file descriptor from 'C'.
        //:
        //:   3 Verify that the results of the call to 'getFileSize' satisfy
        //:     the properties defined by each concern relating to the
        //:     particular result.
        //
        // Testing:
        //   Offset getFileSize(FileDescriptor);
        // --------------------------------------------------------------------

        {
            // This block tests all code paths through the Windows
            // implementation of 'getFileSize', as well as boundary values for
            // its input and output, using a mock Windows interface.

            const u::IntegralConverter<Offset> I;

            const struct {
                int            d_line;
                    // the line number

                FileDescriptor d_fileDescriptor;
                    // the argument to 'getFileSize'

                DWORD          d_sizeHigh;
                    // the high-32 bits to return from 'GetFileSize'

                DWORD          d_sizeLow;
                    // the low-32 bits to return from 'GetFileSize'

                bool           d_expectGetLastError;
                    // whether to expect the implementation to call
                    // 'GetLastError'

                DWORD          d_lastError;
                    // the value to return from 'GetLastError', if called

                Offset         d_result;
                    // the expected 'getFileSize' result

            } DATA[] = {
  //   LINE         EXPECT GET LAST ERROR?       LAST ERROR
  //  .----         ----------------------.     .----------
  // /  FILE DESC.  SIZE HIGH   SIZE LOW   \   /           RESULT
  //-- ----------- ----------- ----------- -- --- --------------------------
  { L_, INVALID_FD,          0,          0, F, NA, I(                 0ULL) },
  { L_, INVALID_FD,          0,          1, F, NA, I(                 1ULL) },
  { L_, INVALID_FD,          0, 0xFFFFFFFE, F, NA, I(        0xFFFFFFFEULL) },
  { L_, INVALID_FD,          0, 0xFFFFFFFF, T,  0, I(        0xFFFFFFFFULL) },
  { L_, INVALID_FD,          0, 0xFFFFFFFF, T,  1, I(                -1ULL) },
  { L_, INVALID_FD,          0, 0x9ABCDEF0, F, NA, I(        0x9ABCDEF0ULL) },

  { L_, INVALID_FD,          1,          0, F, NA, I(       0x100000000ULL) },
  { L_, INVALID_FD,          1,          1, F, NA, I(       0x100000001ULL) },
  { L_, INVALID_FD,          1, 0xFFFFFFFE, F, NA, I(       0x1FFFFFFFEULL) },
  { L_, INVALID_FD,          1, 0xFFFFFFFF, T,  0, I(       0x1FFFFFFFFULL) },
  { L_, INVALID_FD,          1, 0xFFFFFFFF, T,  1, I(                -1ULL) },
  { L_, INVALID_FD,          1, 0x9ABCDEF0, F, NA, I(       0x19ABCDEF0ULL) },

  { L_, INVALID_FD, 0xFFFFFFFE,          0, F, NA, I(0xFFFFFFFE00000000ULL) },
  { L_, INVALID_FD, 0xFFFFFFFE,          1, F, NA, I(0xFFFFFFFE00000001ULL) },
  { L_, INVALID_FD, 0xFFFFFFFE, 0xFFFFFFFE, F, NA, I(0xFFFFFFFEFFFFFFFEULL) },
  { L_, INVALID_FD, 0xFFFFFFFE, 0xFFFFFFFF, T,  0, I(0xFFFFFFFEFFFFFFFFULL) },
  { L_, INVALID_FD, 0xFFFFFFFE, 0xFFFFFFFF, T,  1, I(                -1ULL) },
  { L_, INVALID_FD, 0xFFFFFFFE, 0x9ABCDEF0, F, NA, I(0xFFFFFFFE9ABCDEF0ULL) },

  { L_, INVALID_FD, 0xFFFFFFFF,          0, F, NA, I(0xFFFFFFFF00000000ULL) },
  { L_, INVALID_FD, 0xFFFFFFFF,          1, F, NA, I(0xFFFFFFFF00000001ULL) },
  { L_, INVALID_FD, 0xFFFFFFFF, 0xFFFFFFFE, F, NA, I(0xFFFFFFFFFFFFFFFEULL) },
  { L_, INVALID_FD, 0xFFFFFFFF, 0xFFFFFFFF, T,  0, I(0xFFFFFFFFFFFFFFFFULL) },
  { L_, INVALID_FD, 0xFFFFFFFF, 0xFFFFFFFF, T,  1, I(                -1ULL) },
  { L_, INVALID_FD, 0xFFFFFFFF, 0x9ABCDEF0, F, NA, I(0xFFFFFFFF9ABCDEF0ULL) },

  { L_, INVALID_FD, 0x12345678,          0, F, NA, I(0x1234567800000000ULL) },
  { L_, INVALID_FD, 0x12345678,          1, F, NA, I(0x1234567800000001ULL) },
  { L_, INVALID_FD, 0x12345678, 0xFFFFFFFE, F, NA, I(0x12345678FFFFFFFEULL) },
  { L_, INVALID_FD, 0x12345678, 0xFFFFFFFF, T,  0, I(0x12345678FFFFFFFFULL) },
  { L_, INVALID_FD, 0x12345678, 0xFFFFFFFF, T,  1, I(                -1ULL) },
  { L_, INVALID_FD, 0x12345678, 0x9ABCDEF0, F, NA, I(0x123456789ABCDEF0ULL) },

  { L_,          0,          0,          0, F, NA, I(                 0ULL) },
  { L_,          0,          0,          1, F, NA, I(                 1ULL) },
  { L_,          0,          0, 0xFFFFFFFE, F, NA, I(        0xFFFFFFFEULL) },
  { L_,          0,          0, 0xFFFFFFFF, T,  0, I(        0xFFFFFFFFULL) },
  { L_,          0,          0, 0xFFFFFFFF, T,  1, I(                -1ULL) },
  { L_,          0,          0, 0x9ABCDEF0, F, NA, I(        0x9ABCDEF0ULL) },

  { L_,          0,          1,          0, F, NA, I(       0x100000000ULL) },
  { L_,          0,          1,          1, F, NA, I(       0x100000001ULL) },
  { L_,          0,          1, 0xFFFFFFFE, F, NA, I(       0x1FFFFFFFEULL) },
  { L_,          0,          1, 0xFFFFFFFF, T,  0, I(       0x1FFFFFFFFULL) },
  { L_,          0,          1, 0xFFFFFFFF, T,  1, I(                -1ULL) },
  { L_,          0,          1, 0x9ABCDEF0, F, NA, I(       0x19ABCDEF0ULL) },

  { L_,          0, 0xFFFFFFFE,          0, F, NA, I(0xFFFFFFFE00000000ULL) },
  { L_,          0, 0xFFFFFFFE,          1, F, NA, I(0xFFFFFFFE00000001ULL) },
  { L_,          0, 0xFFFFFFFE, 0xFFFFFFFE, F, NA, I(0xFFFFFFFEFFFFFFFEULL) },
  { L_,          0, 0xFFFFFFFE, 0xFFFFFFFF, T,  0, I(0xFFFFFFFEFFFFFFFFULL) },
  { L_,          0, 0xFFFFFFFE, 0xFFFFFFFF, T,  1, I(                -1ULL) },
  { L_,          0, 0xFFFFFFFE, 0x9ABCDEF0, F, NA, I(0xFFFFFFFE9ABCDEF0ULL) },

  { L_,          0, 0xFFFFFFFF,          0, F, NA, I(0xFFFFFFFF00000000ULL) },
  { L_,          0, 0xFFFFFFFF,          1, F, NA, I(0xFFFFFFFF00000001ULL) },
  { L_,          0, 0xFFFFFFFF, 0xFFFFFFFE, F, NA, I(0xFFFFFFFFFFFFFFFEULL) },
  { L_,          0, 0xFFFFFFFF, 0xFFFFFFFF, T,  0, I(0xFFFFFFFFFFFFFFFFULL) },
  { L_,          0, 0xFFFFFFFF, 0xFFFFFFFF, T,  1, I(                -1ULL) },
  { L_,          0, 0xFFFFFFFF, 0x9ABCDEF0, F, NA, I(0xFFFFFFFF9ABCDEF0ULL) },

  { L_,          0, 0x12345678,          0, F, NA, I(0x1234567800000000ULL) },
  { L_,          0, 0x12345678,          1, F, NA, I(0x1234567800000001ULL) },
  { L_,          0, 0x12345678, 0xFFFFFFFE, F, NA, I(0x12345678FFFFFFFEULL) },
  { L_,          0, 0x12345678, 0xFFFFFFFF, T,  0, I(0x12345678FFFFFFFFULL) },
  { L_,          0, 0x12345678, 0xFFFFFFFF, T,  1, I(                -1ULL) },
  { L_,          0, 0x12345678, 0x9ABCDEF0, F, NA, I(0x123456789ABCDEF0ULL) }
            };

            static const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i != NUM_DATA; ++i) {
                const int            LINE = DATA[i].d_line;
                const FileDescriptor FILE_DESCRIPTOR =
                    DATA[i].d_fileDescriptor;
                const DWORD SIZE_HIGH = DATA[i].d_sizeHigh;
                const DWORD SIZE_LOW  = DATA[i].d_sizeLow;
                const bool  EXPECT_GET_LAST_ERROR =
                    DATA[i].d_expectGetLastError;
                const DWORD  LAST_ERROR = DATA[i].d_lastError;
                const Offset RESULT     = DATA[i].d_result;

                Response getFileSizeResponse;
                getFileSizeResponse.d_functionId = FunctionId::e_GET_FILE_SIZE;
                getFileSizeResponse.d_getFileSize.d_result         = SIZE_LOW;
                getFileSizeResponse.d_getFileSize.d_lpFileSizeHigh = SIZE_HIGH;

                Interface interface;
                interface.pushBackResponse(getFileSizeResponse);

                if (EXPECT_GET_LAST_ERROR) {
                    Response getLastErrorResponse;
                    getLastErrorResponse.d_functionId =
                        FunctionId::e_GET_LAST_ERROR;
                    getLastErrorResponse.d_getLastError.d_result = LAST_ERROR;

                    interface.pushBackResponse(getLastErrorResponse);
                }

                InterfaceUtil::setInterface(&interface);
                const Offset result = Obj::getFileSize(FILE_DESCRIPTOR);
                InterfaceUtil::setInterface(0);

                LOOP1_ASSERT_EQ(LINE, RESULT, result);

                if (EXPECT_GET_LAST_ERROR) {
                    LOOP1_ASSERT_EQ(LINE, 2, interface.numCalls());
                    if (2 != interface.numCalls()) {
                        continue;                                   // CONTINUE
                    }
                }
                else {
                    LOOP1_ASSERT_EQ(LINE, 1, interface.numCalls());
                    if (1 != interface.numCalls()) {
                        continue;                                   // CONTINUE
                    }
                }

                Call getFileSizeCall;
                interface.popFrontCall(&getFileSizeCall);
                LOOP1_ASSERT_EQ(LINE,
                                FunctionId::e_GET_FILE_SIZE,
                                getFileSizeCall.d_functionId);
                if (FunctionId::e_GET_FILE_SIZE !=
                    getFileSizeCall.d_functionId) {
                    continue;                                       // CONTINUE
                }

                LOOP1_ASSERT_EQ(LINE,
                                FILE_DESCRIPTOR,
                                getFileSizeCall.d_getFileSize.d_hFile);

                if (EXPECT_GET_LAST_ERROR) {
                    Call getLastErrorCall;
                    interface.popFrontCall(&getLastErrorCall);
                    LOOP1_ASSERT_EQ(LINE,
                                    FunctionId::e_GET_LAST_ERROR,
                                    getLastErrorCall.d_functionId);
                    if (FunctionId::e_GET_LAST_ERROR !=
                        getLastErrorCall.d_functionId) {
                        continue;                                   // CONTINUE
                    }
                }
            }
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
    }

    return testStatus;
}

#undef U_PLATFORM_IS_NATIVE

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
