// bdls_filesystemutil_transitionaluniximputil.t.cpp                  -*-C++-*-
#include <bdls_filesystemutil_transitionaluniximputil.h>

#include <bdls_filesystemutil_unixplatform.h>

#include <bsla_unused.h>
#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bsl_deque.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

#if defined(BSLS_PLATFORM_OS_UNIX) \
 && defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF64)
#define U_PLATFORM_IS_NATIVE 1
#else
#define U_PLATFORM_IS_NATIVE 0
#endif
    // The macro 'U_PLATFORM_IS_NATIVE' is defined to be 1 if the current
    // platform is a Unix and the current build settings specify the
    // transitional-compilation environment.  Otherwise, the macro is defined
    // to be 0.

#if U_PLATFORM_IS_NATIVE
#include <sys/stat.h>
#include <sys/types.h>
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
#include <windows.h>
#endif

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
// The component under test provides a class template,
// 'bdls::FilesystemUtil_TransitionalUnixImpUtil', that acts as a namespace
// template for a suite of functions that provide file-system operations on top
// of those provided by its class-type template parameter, which must be a
// namespace struct that provides a suite of primitive file-system operations.
// Thus, in order to test this component, we will supply
// 'bdls::FilesystemUtil_TransitionalUnixImpUtil' with synthetic primitive
// file-system operations that record all input and output, in order to verify
// this component correctly operates a Unix system with the
// transitional-compilation environment.
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

#define LOOP_ASSERT_EQ(L,X,Y) ASSERTV(L,X,Y,X == Y)
#define LOOP_ASSERT_NE(L,X,Y) ASSERTV(L,X,Y,X != Y)

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


namespace BloombergLP {
namespace {
namespace u {

#if U_PLATFORM_IS_NATIVE
typedef ::off64_t       off64_t;
typedef struct ::stat64 stat64;
typedef ::time_t        time_t;
#else
typedef bsls::Types::Int64 off64_t;
typedef struct Stat64Imp   stat64;
typedef bsls::Types::Int64 time_t;

                              // ====================
                              // struct Timespec64Imp
                              // ====================

struct Timespec64Imp {
    time_t tv_sec;
    long   tv_nsec;
};

                              // ================
                              // struct Stat64Imp
                              // ================

struct Stat64Imp {
    off64_t              st_size;
    struct Timespec64Imp st_mtim;
};
#define st_mtime st_mtim.tv_sec

// Need a "fake" definition so this test driver will run on Windows.
#define BDLS_FILESYSTEMUTIL_UNIXPLATFORM_STAT_NS_MEMBER st_mtim.tv_nsec

#endif

                   // =====================================
                   // struct TestTransitionalUnixFunctionId
                   // =====================================

struct TestTransitionalUnixFunctionId {
    // This 'struct' provides a namespace for enumerating a set of
    // identifiers that denote some functions provided by Unix operating
    // systems in the transitional-compilation environment.

    enum Enum {
        e_FSTAT64
    };
};

                  // ========================================
                  // struct TestTransitionalUnixInterfaceCall
                  // ========================================

struct TestTransitionalUnixInterfaceCall {
    // This in-core, aggregate-like 'struct' provides a representation of the
    // arguments of a call to a Unix function in the transitional-compilation
    // environment.

    // TYPES
    typedef TestTransitionalUnixFunctionId FunctionId;
        // 'FunctionId' is an alias to an enumeration for a set of identifiers
        // that denote some functions provided by Unix operating systems in the
        // transitional-compilation environment.

    struct Fstat64Call {
        // PUBLIC DATA
        int     d_fildes;
        stat64 *d_buf;
    };

    // PUBLIC DATA
    FunctionId::Enum d_functionId;
    union {
        Fstat64Call d_fstat64;
    };

    // CREATORS
    TestTransitionalUnixInterfaceCall();
        // Create a 'TestTransitionalUnixInterfaceCall' object having
        // indeterminate value.

    TestTransitionalUnixInterfaceCall(
                               const TestTransitionalUnixInterfaceCall& other);
        // Create a 'TestTransitionalUnixInterfaceCall' object having the same
        // value as the specified 'other'.  The behavior is undefined if
        // 'other' has indeterminate value or the active member of its union
        // does not correspond to the value of its 'd_functionId' data member.

    // MANIPULATORS
    TestTransitionalUnixInterfaceCall& operator=(
                   const TestTransitionalUnixInterfaceCall& other) BSLA_UNUSED;
        // Assign to this object the specified 'other' value and return a
        // reference to this object.  The behavior is undefined if 'other' has
        // indeterminate value or the active member of its union does not
        // correspond to the value of its 'd_functionId' data member.
};
                // ============================================
                // struct TestTransitionalUnixInterfaceResponse
                // ============================================

struct TestTransitionalUnixInterfaceResponse {
    // This in-core, aggregate-like 'struct' provides a representation of the
    // results of a call to a Unix function in the transitional-compilation
    // environment.

    // TYPES
    typedef TestTransitionalUnixFunctionId FunctionId;
        // 'FunctionId' is an alias to an enumeration for a set of identifiers
        // that denote some functions provided by Unix operating systems in the
        // transitional-compilation environment.

    struct Fstat64Response {
        // PUBLIC DATA
        int    d_result;
        stat64 d_buf;
    };

    // PUBLIC DATA
    FunctionId::Enum d_functionId;
    union {
        Fstat64Response d_fstat64;
    };

    // CREATORS
    TestTransitionalUnixInterfaceResponse();
        // Create a 'TestTransitionalUnixInterfaceResponse' object having
        // indeterminate value.

    TestTransitionalUnixInterfaceResponse(
                           const TestTransitionalUnixInterfaceResponse& other);
        // Create a 'TestTransitionalUnixInterfaceResponse' object having the
        // same value as the specified 'other'.  The behavior is undefined if
        // 'other' has indeterminate value or the active member of its union
        // does not correspond to the value of its 'd_functionId' data member.

    // MANIPULATORS
    TestTransitionalUnixInterfaceResponse& operator=(
               const TestTransitionalUnixInterfaceResponse& other) BSLA_UNUSED;
        // Assign to this object the specified 'other' value and return a
        // reference to this object.  The behavior is undefined if 'other' has
        // indeterminate value or the active member of its union does not
        // correspond to the value of its 'd_functionId' data member.
};

                    // ===================================
                    // class TestTransitionalUnixInterface
                    // ===================================

class TestTransitionalUnixInterface {
    // This mechanism class provides a set of member functions that mock
    // corresponding Unix system functions in the transitional-compilation
    // environment.  It records the arguments to these functions in a queue,
    // which clients retrieve with 'popFrontCall', and returns results from a
    // queue that clients populate with 'pushFrontResponse'.

  public:
    // TYPES
    typedef TestTransitionalUnixInterfaceCall     Call;
        // 'Call' is an alias to an in-core, aggregate-like 'struct' that
        // provides a representation of the arguments of a call to a Unix
        // function in the transitional-compilation environment.

    typedef TestTransitionalUnixInterfaceResponse Response;
        // 'Response' is an alias to an in-core, aggregate-like 'struct' that
        // provides a representation of the results of a call to a Unix
        // function in the transitional-compilation environment.

    typedef bsl::allocator<char>                  allocator_type;
        // 'allocator_type' is an alias to the type of allocator that supplies
        // memory to 'TestTransitionalUnixInterface' objects.

  private:
    // PRIVATE TYPES
    typedef TestTransitionalUnixFunctionId FunctionId;
        // 'FunctionId' is an alias to an enumeration for a set of identifiers
        // that denote some functions provided by Unix operating systems in the
        // transitional-compilation environment.

    // DATA
    bsl::deque<Call>     d_calls;
        // queue of call records that 'fstat64' populates and 'popFrontCall'
        // drains

    bsl::deque<Response> d_responses;
        // queue of response records that 'pushBackResponse' populates and
        // 'fstat64' drains

    // NOT IMPLEMENTED
    TestTransitionalUnixInterface(const TestTransitionalUnixInterface&);
    TestTransitionalUnixInterface& operator=(
                                         const TestTransitionalUnixInterface&);

  public:
    // CREATORS
    TestTransitionalUnixInterface();
    explicit TestTransitionalUnixInterface(
                                  const allocator_type& allocator) BSLA_UNUSED;
        // Create a 'TestTransitionalUnixInterface' object that has empty call
        // and response queues.  Optionally specify an 'allocator' used to
        // supply memory; otherwise, the default allocator is used.

    // MANIPULATORS
    int fstat64(int fildes, stat64 *buf);
        // Push a 'Call' to the call queue that has a 'FunctionId::e_FSTAT64'
        // 'd_functionId', a 'd_fildes' equal to the specified 'fildes', and a
        // 'd_buf' equal to the specified 'buf'. Load the 'd_buf' into 'buf'
        // and return the 'd_result' of the next queued response.  The behavior
        // is undefined if the response queue is empty or the 'd_functionId' of
        // the next queued response is not 'FunctionId::e_FSTAT64'.

    void popFrontCall(Call *call);
        // Load the next queued call into the specified 'call' and remove it
        // from the queue.  The behavior is undefined if the call queue is
        // empty.

    void pushBackResponse(const Response& response);
        // Push the specified 'response' to the response queue.

    // ACCESSORS
    int numCalls() const BSLA_UNUSED;
        // Return the number of calls in the call queue.

    int numResponses() const BSLA_UNUSED;
        // Return the number of responses in the response queue.
};

                  // ========================================
                  // struct TestTransitionalUnixInterfaceUtil
                  // ========================================

struct TestTransitionalUnixInterfaceUtil {
    // This utility 'struct' provides an implementation of the requirements for
    // the 'UNIX_INTERFACE' template parameter of the functions provided by
    // 'FilesystemUtil_TransitionalUnixImpUtil' in terms of mock Unix
    // interface calls in the transitional-compilation environment.

    // TYPES
    typedef ::BloombergLP::u::off64_t off64_t;
        // 'off_t' is an alias to the 'off_t' type provided by the
        // 'sys/types.h' header.  It is a signed integral type used to
        // represent quantities of bytes.

    typedef ::BloombergLP::u::stat64 stat64;
        // 'stat' is an alias to the 'stat' 'struct' provided by the
        // 'sys/stat.h' header.

    typedef ::BloombergLP::u::time_t time_t;
        // 'time_t' is an alias to the 'time_t' type provided by the
        // 'sys/types.h' header.  It represents a time point as number of
        // seconds since January 1st 1970 in Coordinated Universal Time.

  private:
    // CLASS DATA
    static TestTransitionalUnixInterface *s_interface_p;
        // the currently-installed mock Unix interface mechanism

  public:
    // CLASS METHODS
    static off64_t get_st_size(const stat64& stat);
        // Return the value of the 'st_size' member of the specified 'stat'.

    static long get_st_mtim_nsec(const stat64& stat);
        // Return the value of the 'st_mtim.tv_nsec' member of the specified
        // 'stat' if present.

    static time_t get_st_mtime(const stat64& stat);
        // Return the value of the 'st_mtime' member of the specified 'stat'.

    static int fstat64(int fildes, stat64 *buf);
        // Push a 'Call' to the interface's call queue that has a
        // 'FunctionId::e_FSTAT64' 'd_functionId', a 'd_fildes' equal to the
        // specified 'fildes', and a 'd_buf' equal to the specified 'buf', load
        // the 'd_buf' into 'buf' and return the 'd_result' of the interface's
        // next queued response.  The behavior is undefined if the response
        // queue is empty or the 'd_functionId' of the next queued response is
        // not 'FunctionId::e_FSTAT64'.

    static void setInterface(TestTransitionalUnixInterface *interface);
        // Set the interface to the specified 'interface'.

    static TestTransitionalUnixInterface *interface() BSLA_UNUSED;
        // Return the interface.
};

// ============================================================================
//                          DEFINITIONS FOR TESTING
// ----------------------------------------------------------------------------

                  // ----------------------------------------
                  // struct TestTransitionalUnixInterfaceCall
                  // ----------------------------------------

// CREATORS
TestTransitionalUnixInterfaceCall::TestTransitionalUnixInterfaceCall()
{
}

TestTransitionalUnixInterfaceCall::TestTransitionalUnixInterfaceCall(
                                const TestTransitionalUnixInterfaceCall& other)
: d_functionId(other.d_functionId)
{
    switch (other.d_functionId) {
      case FunctionId::e_FSTAT64: {
          d_fstat64 = other.d_fstat64;
      } break;
    }
}

// MANIPULATORS
TestTransitionalUnixInterfaceCall&
TestTransitionalUnixInterfaceCall::operator=(
                                const TestTransitionalUnixInterfaceCall& other)
{
    d_functionId = other.d_functionId;

    switch (other.d_functionId) {
      case FunctionId::e_FSTAT64: {
          d_fstat64 = other.d_fstat64;
      } break;
    }

    return *this;
}

                // --------------------------------------------
                // struct TestTransitionalUnixInterfaceResponse
                // --------------------------------------------

// CREATORS
TestTransitionalUnixInterfaceResponse::TestTransitionalUnixInterfaceResponse()
{
}

TestTransitionalUnixInterfaceResponse::TestTransitionalUnixInterfaceResponse(
                            const TestTransitionalUnixInterfaceResponse& other)
: d_functionId(other.d_functionId)
{
    switch (other.d_functionId) {
      case FunctionId::e_FSTAT64: {
          d_fstat64 = other.d_fstat64;
      } break;
    }
}

// MANIPULATORS
TestTransitionalUnixInterfaceResponse&
TestTransitionalUnixInterfaceResponse::operator=(
                            const TestTransitionalUnixInterfaceResponse& other)
{
    d_functionId = other.d_functionId;

    switch (other.d_functionId) {
      case FunctionId::e_FSTAT64: {
          d_fstat64 = other.d_fstat64;
      } break;
    }

    return *this;
}

                    // ------------------------------------
                    // struct TestTransitionalUnixInterface
                    // ------------------------------------

// CREATORS
TestTransitionalUnixInterface::TestTransitionalUnixInterface()
: d_calls()
, d_responses()
{
}

TestTransitionalUnixInterface::TestTransitionalUnixInterface(
                                               const allocator_type& allocator)
: d_calls(allocator.mechanism())
, d_responses(allocator.mechanism())
{
}

// MANIPULATORS
int TestTransitionalUnixInterface::fstat64(int fildes, stat64 *buf)
{
    Call call;
    call.d_functionId       = FunctionId::e_FSTAT64;
    call.d_fstat64.d_fildes = fildes;
    call.d_fstat64.d_buf    = buf;
    d_calls.push_back(call);

    BSLS_ASSERT(0 < d_responses.size());
    const Response& response = d_responses.front();
    BSLS_ASSERT(FunctionId::e_FSTAT64 == response.d_functionId);
    const int result = response.d_fstat64.d_result;
    *buf             = response.d_fstat64.d_buf;
    d_responses.pop_front();
    return result;
}

void TestTransitionalUnixInterface::popFrontCall(Call *call)
{
    BSLS_ASSERT(0 < d_calls.size());
    *call = d_calls.front();
    d_calls.pop_front();
}

void TestTransitionalUnixInterface::pushBackResponse(const Response& response)
{
    d_responses.push_back(response);
}

// ACCESSORS
int TestTransitionalUnixInterface::numCalls() const
{
    return static_cast<int>(d_calls.size());
}

int TestTransitionalUnixInterface::numResponses() const
{
    return static_cast<int>(d_responses.size());
}

                  // ----------------------------------------
                  // struct TestTransitionalUnixInterfaceUtil
                  // ----------------------------------------

// CLASS DATA
TestTransitionalUnixInterface
    *TestTransitionalUnixInterfaceUtil::s_interface_p = 0;

// CLASS METHODS
long
TestTransitionalUnixInterfaceUtil::get_st_mtim_nsec(const stat64& stat)
{
    (void) stat;
     return stat.BDLS_FILESYSTEMUTIL_UNIXPLATFORM_STAT_NS_MEMBER;
}

TestTransitionalUnixInterfaceUtil::time_t
TestTransitionalUnixInterfaceUtil::get_st_mtime(const stat64& stat)
{
    return stat.st_mtime;
}


TestTransitionalUnixInterfaceUtil::off64_t
TestTransitionalUnixInterfaceUtil::get_st_size(const stat64& stat)
{
    return stat.st_size;
}


int TestTransitionalUnixInterfaceUtil::fstat64(int fildes, stat64 *buf)
{
    return s_interface_p->fstat64(fildes, buf);
}

void TestTransitionalUnixInterfaceUtil::setInterface(
                                      TestTransitionalUnixInterface *interface)
{
    s_interface_p = interface;
}

TestTransitionalUnixInterface *TestTransitionalUnixInterfaceUtil::interface()
{
    return s_interface_p;
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

    typedef u::TestTransitionalUnixFunctionId        FunctionId;
    typedef u::TestTransitionalUnixInterfaceCall     Call;
    typedef u::TestTransitionalUnixInterfaceResponse Response;
    typedef u::TestTransitionalUnixInterface         Interface;
    typedef u::TestTransitionalUnixInterfaceUtil     InterfaceUtil;

    typedef FilesystemUtil_TransitionalUnixImpUtil<InterfaceUtil> Obj;
    typedef Obj::FileDescriptor FileDescriptor;
    typedef Obj::Offset         Offset;

    const FileDescriptor INVALID_FD = -1;

    typedef bsls::Types::Int64 Int64;

    switch(test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'getLastModificationTime'
        //   This case verifies, using a mock Unix interface, that
        //   'getLastModificationTime' correctly covnerts the values of its
        //   arguments into the values required by the underlying Unix system
        //   operation, converts the results of the operation to
        //   'getLastModificationTime's results, and invokes no undefined
        //   behavior when given or producing values near their limits.
        //
        // Concerns:
        //: o Invalid file descriptor values of 'descriptor' are forwarded to
        //:   the underlying 'fstat64' call.
        //:
        //: o Non-zero return codes from 'fstat64' calls result in a non-zero
        //:   return value.
        //:
        //: o Upon success, the value loaded to the 'st_mtime' field of the
        //:   'stat64' struct is converted to the 'bdlt::Datetime' result that
        //:   represents the same value as the 'st_mtime', if a
        //:   'bdlt::Datetime' is capable of representing the value, and a
        //:   non-zero value is returned otherwise.
        //
        // Plan:
        //: 1 Create a table that enumerates several combinations of values for
        //:   'descriptor', as well as return codes and 'stat64' struct values to
        //:   use as results from an underlying call to 'fstat64', where the
        //:   numeric limits of each are present, and values known to be near
        //:   the limits of the corresponding results for
        //:   'getLastModificationTime'.
        //:
        //: 2 For each combination 'C' of the set of combinations, do the
        //:   following:
        //:
        //:   1 Prime a mock Unix interface to expect a call to 'fstat64' with
        //:     the file descriptor in 'C' and to provide the return code and
        //:     'stat64' struct values from 'C' as results to the call to
        //:     'fstat64'.
        //:
        //:   2 Invoke 'getLastModificationTime' with the file descriptor from
        //:     'C'.
        //:
        //:   3 Verify that the results of the call to
        //:     'getLastModificationTime' satisfy the properties defined by each
        //:     concern relating to the particular result.
        //:
        //:   4 Verify that one and only one call to 'fstat64' was made.
        //
        // Testing:
        //   Offset getLastModificationTime(bdlt::Datetime *, FileDescriptor);
        // --------------------------------------------------------------------

        const u::time_t MIN_TIME = bsl::numeric_limits<u::time_t>::min();
        const u::time_t MAX_TIME = bsl::numeric_limits<u::time_t>::max();

        typedef bsls::Types::Int64 Int64;
        const Int64 MIN_TIME32 = -0x7FFFFFFF - 1;
        const Int64 MAX_TIME32 =  0x7FFFFFFF;
        const Int64 MIN_TIME64 = -0x7FFFFFFFFFFFFFFFLL - 1;
        const Int64 MAX_TIME64 =  0x7FFFFFFFFFFFFFFFLL;

        ASSERT(bsl::is_integral<u::time_t>::value);
        ASSERT(MIN_TIME32 == MIN_TIME || MIN_TIME64 == MIN_TIME);
        ASSERT(MAX_TIME32 == MAX_TIME || MAX_TIME64== MAX_TIME);

        const bool TIME_IS_LIMITED /* har har */ = (MAX_TIME32 == MAX_TIME);
        if (veryVerbose && TIME_IS_LIMITED) {
            bsl::cout << "This Unix platform cannot represent dates after "
                         "3:14:07AM on January 19th, 2038."
                      << bsl::endl;
        }

        enum { NA = 0 };

        typedef bdlt::Datetime DT;
        const DT               DT_NA;

        ASSERTV((bsl::is_same<FileDescriptor, int>::value));

        const FileDescriptor MIN_FD =
            bsl::numeric_limits<FileDescriptor>::min();
        const FileDescriptor MAX_FD =
            bsl::numeric_limits<FileDescriptor>::max();

        const int MIN = bsl::numeric_limits<int>::min();
        const int MAX = bsl::numeric_limits<int>::max();

        const struct {
            int                d_line;
                // the line number

            FileDescriptor     d_fileDescriptor;
                // the argument to 'getLastModificationTime'

            int                d_fildes;
                // the expected 'fildes' argument to 'fstat'

            int                d_fstat64Result;
                // what to return from the mock 'fstat' call

            bsls::Types::Int64 d_st_mtime64;
                // what to load into the 'st_mtime' field of the 'stat' from
                // the mock 'fstat' call

            bdlt::Datetime     d_time;
                // the expected value of 'time' after the call to
                // 'getLastModificationTime'

            int                d_result;
                // the expected 'getLastModificationTime' return value
          } DATA[] = {
 { L_, INVALID_FD,  -1, -1,             NA, DT_NA                       , -1 },
//{ L_, INVALID_FD,  -1,  0, MIN_TIME64    , DT_NA                       , -1 }
// Note that as of September 25th, 2020, there is a bug in 'bdlt_datetime' in
// which the behavior of invoking 'bdlt::Datetime::addSecondsIfValid' with an
// argument of 'bsl::numeric_limits<bsls::Types::Int64>::min()' is undefined.
 { L_, INVALID_FD,  -1,  0, MIN_TIME64 + 1, DT_NA                       , -1 },
 { L_, INVALID_FD,  -1,  0, -62135769601LL, DT_NA                       , -1 },
 { L_, INVALID_FD,  -1,  0, -62135769600LL, DT(   1,  1,  1,  0,  0,  0),  0 },
 { L_, INVALID_FD,  -1,  0, MIN_TIME32 - 1, DT(1901, 12, 13, 20, 45, 51),  0 },
 { L_, INVALID_FD,  -1,  0, MIN_TIME32    , DT(1901, 12, 13, 20, 45, 52),  0 },
 { L_, INVALID_FD,  -1,  0, MIN_TIME32 + 1, DT(1901, 12, 13, 20, 45, 53),  0 },
 { L_, INVALID_FD,  -1,  0,           -1LL, DT(1969, 12, 31, 23, 59, 59),  0 },
 { L_, INVALID_FD,  -1,  0,            0LL, DT(1970,  1,  1,  0,  0,  0),  0 },
 { L_, INVALID_FD,  -1,  0,            1LL, DT(1970,  1,  1,  0,  0,  1),  0 },
 { L_, INVALID_FD,  -1,  0,           59LL, DT(1970,  1,  1,  0,  0, 59),  0 },
 { L_, INVALID_FD,  -1,  0,           60LL, DT(1970,  1,  1,  0,  1,  0),  0 },
 { L_, INVALID_FD,  -1,  0,         3599LL, DT(1970,  1,  1,  0, 59, 59),  0 },
 { L_, INVALID_FD,  -1,  0,         3600LL, DT(1970,  1,  1,  1,  0,  0),  0 },
 { L_, INVALID_FD,  -1,  0,        86399LL, DT(1970,  1,  1, 23, 59, 59),  0 },
 { L_, INVALID_FD,  -1,  0,        86400LL, DT(1970,  1,  2,  0,  0,  0),  0 },
 { L_, INVALID_FD,  -1,  0, MAX_TIME32 - 1, DT(2038,  1, 19,  3, 14,  6),  0 },
 { L_, INVALID_FD,  -1,  0, MAX_TIME32    , DT(2038,  1, 19,  3, 14,  7),  0 },
 { L_, INVALID_FD,  -1,  0, MAX_TIME32 + 1, DT(2038,  1, 19,  3, 14,  8),  0 },
 { L_, INVALID_FD,  -1,  0, 253402300799LL, DT(9999, 12, 31, 23, 59, 59),  0 },
 { L_, INVALID_FD,  -1,  0, 253402300800LL, DT_NA                       , -1 },
 { L_, INVALID_FD,  -1,  0, MAX_TIME64 - 1, DT_NA                       , -1 },
 { L_, INVALID_FD,  -1,  0, MAX_TIME64    , DT_NA                       , -1 },

 { L_,     MIN_FD, MIN,  0, MIN_TIME64 + 1, DT_NA                       , -1 },
 { L_,     MIN_FD, MIN,  0, MAX_TIME32    , DT(2038,  1, 19,  3, 14,  7),  0 },
 { L_,     MIN_FD, MIN,  0, MAX_TIME64    , DT_NA                       , -1 },

 { L_,          0,   0,  0, MIN_TIME64 + 1, DT_NA                       , -1 },
 { L_,          0,   0,  0, MAX_TIME32    , DT(2038,  1, 19,  3, 14,  7),  0 },
 { L_,          0,   0,  0, MAX_TIME64    , DT_NA                       , -1 },

 { L_,          1,   1,  0, MIN_TIME64 + 1, DT_NA                       , -1 },
 { L_,          1,   1,  0, MAX_TIME32    , DT(2038,  1, 19,  3, 14,  7),  0 },
 { L_,          1,   1,  0, MAX_TIME64    , DT_NA                       , -1 },

 { L_,     MAX_FD, MAX,  0, MIN_TIME64 + 1, DT_NA                       , -1 },
 { L_,     MAX_FD, MAX,  0, MAX_TIME32    , DT(2038,  1, 19,  3, 14,  7),  0 },
 { L_,     MAX_FD, MAX,  0, MAX_TIME64    , DT_NA                       , -1 }
          };

          static const int NUM_DATA = sizeof DATA / sizeof *DATA;

          for (int i = 0; i != NUM_DATA; ++i) {
              const int            LINE            = DATA[i].d_line;
              const FileDescriptor FILE_DESCRIPTOR = DATA[i].d_fileDescriptor;
              const int            FILDES          = DATA[i].d_fildes;
              const int            FSTAT64_RESULT  = DATA[i].d_fstat64Result;
              const bsls::Types::Int64 ST_MTIME64  = DATA[i].d_st_mtime64;
                    bdlt::Datetime     TIME        = DATA[i].d_time;
              const int                RESULT      = DATA[i].d_result;

              if (ST_MTIME64 < MIN_TIME || ST_MTIME64 > MAX_TIME) {
                  continue;
              }

              const u::time_t ST_MTIME = static_cast<u::time_t>(ST_MTIME64);

              Response response;
              response.d_functionId             = FunctionId::e_FSTAT64;
              response.d_fstat64.d_result       = FSTAT64_RESULT;
              response.d_fstat64.d_buf.st_mtime = ST_MTIME;

              if (0 == RESULT) {
                  TIME.addMicrosecondsIfValid(99);
              }
              response.d_fstat64.d_buf
                  .BDLS_FILESYSTEMUTIL_UNIXPLATFORM_STAT_NS_MEMBER = 99 * 1000;

              Interface interface;
              interface.pushBackResponse(response);

              InterfaceUtil::setInterface(&interface);
              bdlt::Datetime time;
              const int      result =
                  Obj::getLastModificationTime(&time, FILE_DESCRIPTOR);
              InterfaceUtil::setInterface(0);

              Call call = Call();
              interface.popFrontCall(&call);
              LOOP_ASSERT_EQ(LINE, FunctionId::e_FSTAT64, call.d_functionId);
              if (FunctionId::e_FSTAT64 != call.d_functionId) {
                  continue;
              }

              LOOP_ASSERT_EQ(LINE, RESULT, result);
              LOOP_ASSERT_EQ(LINE, TIME, time);
              LOOP_ASSERT_EQ(LINE, FILDES, call.d_fstat64.d_fildes);
          }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'getFileSize'
        //   This case verifies, using a mock Unix interface, that
        //   'getFileSize' correctly converts the values of its arguments into
        //   the values required by the underlying Unix system operation,
        //   converts the results of the operation to 'getFileSize's results,
        //   and invokes no undefined behavior when given or producing values
        //   near their limits.
        //
        // Concerns:
        //: o Invalid file descriptor values of 'descriptor' are forwarded to
        //:   the underlying 'fstat' call.
        //:
        //: o File descriptors at the numeric limits of the type are forwarded
        //:   to the underlying 'fstat' call unmodified.
        //:
        //: o Non-zero return codes from 'fstat' calls result in a negative
        //:   return value.
        //:
        //: o Upon success, the numeric value loaded to the 'st_size' field of
        //:   the 'stat' struct is returned without modification, even at the
        //:   numeric limits of its type.
        //
        // Plan:
        //: 1 Create a table that enumerates several combinations of values for
        //:   'descriptor', as well as return codes and 'stat' struct values to
        //:   use as results from an underlying call to 'fstat', where the
        //:   numeric limits of each are present.
        //:
        //: 2 For each combination 'C' of the set of combinations, do the
        //:   following:
        //:
        //:   1 Prime a mock Unix interface to expect a call to 'fstat' with
        //:     the file descriptor in 'C', and to provide the return code and
        //:     'stat' struct values from 'C' as results to the call to
        //:     'fstat'.
        //:
        //:   2 Invoke 'getFileSize' with the file descriptor from 'C'.
        //:
        //:   3 Verify that the results of the call to 'getFileSize' satisfy
        //:     the properties defined by each concern relating to the
        //:     particular result.
        //:
        //:   4 Verify that one and only one call to 'fstat' was made.
        //
        // Testing:
        //   Offset getFileSize(FileDescriptor);
        // --------------------------------------------------------------------

        {
            // This block tets all code paths through the transitional Unix
            // implementation of 'getFileSize', as well as boundary values for
            // its input and output, using a mock transitional Unix interface.

            const int   MAX_INT   = bsl::numeric_limits<int>::max();
            const Int64 MAX_INT64 = bsl::numeric_limits<Int64>::max();

            const struct {
                int            d_line;
                    // the line number

                FileDescriptor d_fileDescriptor;
                    // the argument to 'getFileSize'

                int            d_fildes;
                    // the expected 'fildes' argument to 'fstat64'

                int            d_fstat64Result;
                    // what to return from the mock 'fstat64'

                Int64          d_st_size;
                    // what to load into 'stat64' from the mock 'fstat64'

                Offset         d_result;
                    // the expected 'getFileSize' result
            } DATA[] = {
                //   LINE                    FSTAT64 RESULT
                //  .----                   .--------------
                // /  FILE DESC.  FILDES   /   ST_SIZE     RESULT
                //-- ----------- -------- --- ---------- ----------
                { L_, INVALID_FD,      -1, -1,         0,        -1 },
                { L_, INVALID_FD,      -1, -1,         1,        -1 },
                { L_, INVALID_FD,      -1, -1, MAX_INT64,        -1 },
                { L_, INVALID_FD,      -1, -2,         0,        -1 },
                { L_, INVALID_FD,      -1, -2,         1,        -1 },
                { L_, INVALID_FD,      -1, -2, MAX_INT64,        -1 },
                { L_, INVALID_FD,      -1,  0,         0,         0 },
                { L_, INVALID_FD,      -1,  0,         1,         1 },
                { L_, INVALID_FD,      -1,  0, MAX_INT64, MAX_INT64 },

                { L_,          0,       0, -1,         0,        -1 },
                { L_,          0,       0, -1,         1,        -1 },
                { L_,          0,       0, -1, MAX_INT64,        -1 },
                { L_,          0,       0, -2,         0,        -1 },
                { L_,          0,       0, -2,         1,        -1 },
                { L_,          0,       0, -2, MAX_INT64,        -1 },
                { L_,          0,       0,  0,         0,         0 },
                { L_,          0,       0,  0,         1,         1 },
                { L_,          0,       0,  0, MAX_INT64, MAX_INT64 },

                { L_,          1,       1, -1,         0,        -1 },
                { L_,          1,       1, -1,         1,        -1 },
                { L_,          1,       1, -1, MAX_INT64,        -1 },
                { L_,          1,       1, -2,         0,        -1 },
                { L_,          1,       1, -2,         1,        -1 },
                { L_,          1,       1, -2, MAX_INT64,        -1 },
                { L_,          1,       1,  0,         0,         0 },
                { L_,          1,       1,  0,         1,         1 },
                { L_,          1,       1,  0, MAX_INT64, MAX_INT64 },

                { L_,    MAX_INT, MAX_INT, -1,         0,        -1 },
                { L_,    MAX_INT, MAX_INT, -1,         1,        -1 },
                { L_,    MAX_INT, MAX_INT, -1, MAX_INT64,        -1 },
                { L_,    MAX_INT, MAX_INT, -2,         0,        -1 },
                { L_,    MAX_INT, MAX_INT, -2,         1,        -1 },
                { L_,    MAX_INT, MAX_INT, -2, MAX_INT64,        -1 },
                { L_,    MAX_INT, MAX_INT,  0,         0,         0 },
                { L_,    MAX_INT, MAX_INT,  0,         1,         1 },
                { L_,    MAX_INT, MAX_INT,  0, MAX_INT64, MAX_INT64 },
            };

            static const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i != NUM_DATA; ++i) {
                const int            LINE            = DATA[i].d_line;
                const FileDescriptor FILE_DESCRIPTOR =
                    DATA[i].d_fileDescriptor;
                const int    FILDES         = DATA[i].d_fildes;
                const int    FSTAT64_RESULT = DATA[i].d_fstat64Result;
                const Int64  ST_SIZE        = DATA[i].d_st_size;
                const Offset RESULT         = DATA[i].d_result;

                Response response;
                response.d_functionId            = FunctionId::e_FSTAT64;
                response.d_fstat64.d_result      = FSTAT64_RESULT;
                response.d_fstat64.d_buf.st_size = ST_SIZE;

                Interface interface;
                interface.pushBackResponse(response);

                InterfaceUtil::setInterface(&interface);
                const Offset result = Obj::getFileSize(FILE_DESCRIPTOR);
                InterfaceUtil::setInterface(0);

                Call call = Call();
                interface.popFrontCall(&call);
                LOOP_ASSERT_EQ(LINE, FunctionId::e_FSTAT64, call.d_functionId);
                if (FunctionId::e_FSTAT64 != call.d_functionId) {
                    continue;                                       // CONTINUE
                }

                LOOP_ASSERT_EQ(LINE, RESULT, result);
                LOOP_ASSERT_EQ(LINE, FILDES, call.d_fstat64.d_fildes);
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
