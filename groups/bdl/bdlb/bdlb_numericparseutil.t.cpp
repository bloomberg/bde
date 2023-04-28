// bdlb_numericparseutil.t.cpp                                        -*-C++-*-
#include <bdlb_numericparseutil.h>

#include <bdlb_chartype.h>
#include <bdlb_float.h>
#include <bdlb_stringviewutil.h>

#include <bslim_testutil.h>

#include <bslma_defaultallocatorguard.h>   // for testing only
#include <bslma_sequentialallocator.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_log.h>
#include <bsls_platform.h>
#include <bsls_review.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_iomanip.h>
#include <bsl_iterator.h>
#include <bsl_limits.h>
#include <bsl_map.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_string_view.h>
#include <bsl_vector.h>

#include <bsl_cerrno.h>    // 'errno', 'ERANGE', 'EDOM'
#include <bsl_climits.h>
#include <bsl_cmath.h>     // 'bsl::fabs'
#include <bsl_cstdio.h>    // 'bsl::sscanf'
#include <bsl_cstring.h>   // 'bsl::memset', 'bsl::memcpy'
#include <bsl_cstddef.h>   // 'bsl::ptrdiff_t', 'bsl::size_t'
#include <bsl_cstdlib.h>

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV) || \
    !defined(BSLS_LIBRARYFEATURES_STDCPP_MSVC)
    #define u_PARSEDOUBLE_USES_STRTOD                                         1
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
    #define u_PARSEDOUBLE_USES_FROM_CHARS                                     1

    #ifndef BSLS_LIBRARYFEATURES_STDCPP_MSVC
        #define u_PARSEDOUBLE_USES_STRTOD_ON_RANGE_ERRORS_ONLY                1
    #endif
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
    // For 'parseDouble' implementation assumptions verification & reporting
    #include <bsl_charconv.h>
    #include <bsl_system_error.h>
#endif

#if defined(__GLIBC__) && defined(u_PARSEDOUBLE_USES_STRTOD)
    #include <gnu/libc-version.h>
    // Using 'gnu_get_libc_version()' and 'gnu_get_libc_release()' to give a
    // full picture of which libc.so we are running against (when we detect
    // changes in the expected 'strtod' anomalies).
#endif

//=============================================================================
//                   PLATFORM RELATED CONDITIONAL BEHAVIORS
//-----------------------------------------------------------------------------

#if defined(__GLIBC__) && __GLIBC__ <= 2
    // 'strtod' of GNU libc  consistently parses any hexfloat value that is
    // half of 'bsl::numeric_limits<double>::denorm_min()' into the value of
    // 'bsl::numeric_limits<double>::denorm_min()'.  This off-by-one-helf bug
    // does not seem to appear for any other value or format.  The anomaly is
    // fixed/patched up in the 'parseDouble' implementation.  As 'strtod' is
    // used with GNU libstdc++ even when 'from_chars' is present (to determine
    // the exact under/overflow condition) the bug fix has to be tested even
    // when we use 'from_chars'.
    #define u_GLIBC2_STRTOD_HEX_HALF_DENORM_MIN_HEX_BUG                       1
#endif

#if !defined(BSLS_PLATFORM_OS_SUNOS) && !defined(BSLS_PLATFORM_OS_SOLARIS)
    // SunOS/Solaris 'strtod' does not support hexfloats, even though its C99.
    #define u_BDLB_NUMERICPARSEUTIL_SUPPORT_PARSING_HEXFLOAT                  1
#endif

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We use standard table-based approach to testing where we put both input and
// expected output in the same table row and verify that the actual result
// matches the expected value.
//
// For each test case the test vectors include both cases where parsable
// string is (a) the only string in the input and (b) followed by a suffix
// string.
//
//-----------------------------------------------------------------------------
// [ 1] characterToDigit(char character, int base)
// [ 2] parseUnsignedInteger(result, rest, in, base, maxVal)
// [ 2] parseUnsignedInteger(result, rest, in, base, maxVal, maxDigit)
// [ 2] parseUnsignedInteger(result, in, base, maxVal)
// [ 2] parseUnsignedInteger(result, in, base, maxVal, maxDigit)
// [ 3] parseSignedInteger(result, rest, input, base, minVal, maxVal)
// [ 3] parseSignedInteger(result, input, base, minVal, maxVal)
// [ 4] size_t calcRestPos(parsedChars, length)
// [ 4] class BslsReviewMonitor
// [ 4] class TestAssertCounter
// [ 4] double parseExpected(const bsl::string_view&)
// [ 5] parseDouble(double *res, string_view *rest, string_view in)
// [ 5] parseDouble(double *res, string_view in)
// [ 6] parseInt(result, rest, input, base = 10)
// [ 6] parseInt(result, input, base = 10)
// [ 7] parseInt64(result, rest, input, base = 10)
// [ 7] parseInt64(result, input, base = 10)
// [ 8] parseUint(result, rest, input, base = 10)
// [ 8] parseUint(result, input, base = 10)
// [ 9] parseUint64(result, rest, input, base = 10)
// [ 9] parseUint64(result, input, base = 10)
// [10] parseShort(result, rest, input, base = 10)
// [10] parseShort(result, input, base = 10)
// [11] parseUshort(result, rest, input, base = 10)
// [11] parseUshort(result, input, base = 10)
//-----------------------------------------------------------------------------
// [ 4] PARSE DOUBLE TEST MACHINERY
// [12] USAGE EXAMPLE

// ============================================================================
//                        STANDARD ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        bsl::cout << "Error " __FILE__ "(" << line << "): " << message
                  << "    (failed)" << bsl::endl;

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

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

//=============================================================================
//                        TEST-DRIVER LOCAL WORKAROUNDS
//-----------------------------------------------------------------------------

#ifdef BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD
    // SunOS/Solaris iostreams are broken as in they can crash in various
    // fashion if used with a null stream buffer pointer, especially
    // bsl::cout', that just crashes on any output after a 'bsl::cout.rdbuf(0)'
    // call if using in any 'operator<<' from it.
    #define u_NULL_STREAMBUFPTR_CRASHES_IOSTREAMS                             1
#endif

//=============================================================================
//                       VARIADIC MACRO HELPERS
//-----------------------------------------------------------------------------

#define u_LASTARG0(X)                 (X)
#define u_LASTARG1(I,X)               (X)
#define u_LASTARG2(I,J,X)             (X)
#define u_LASTARG3(I,J,K,X)           (X)
#define u_LASTARG4(I,J,K,L,X)         (X)
#define u_LASTARG5(I,J,K,L,M,X)       (X)
#define u_LASTARG6(I,J,K,L,M,N,X)     (X)
#define u_LASTARG7(I,J,K,L,M,N,O,X)   (X)
#define u_LASTARG8(I,J,K,L,M,N,O,V,X) (X)

#define u_LASTARGN_IMPL(N, ...) BSLIM_TESTUTIL_EXPAND(                        \
                                                   u_LASTARG ## N(__VA_ARGS__))

#define u_LASTARGN(N, ...) u_LASTARGN_IMPL(N, __VA_ARGS__)

#define u_LASTARG(...) u_LASTARGN(BSLIM_TESTUTIL_NUM_ARGS(__VA_ARGS__),       \
                                  __VA_ARGS__)
    // To get the condition out of the arguments of an 'ASSERTV'.  The macro
    // evaluates to the last argument given to it, unless there are more than 8
    // arguments, in which case the behavior is undefined.  Same as 'ASSERTV'.

// ============================================================================
//                                MACROS
// ----------------------------------------------------------------------------

#define ASPTR(ptrexpr) static_cast<const void *>(ptrexpr)
    // Convert the specified 'ptrexp' (an expression that results in a pointer)
    // to 'const void *'.  Used in printing the pointer value and not a C
    // string for 'char *'/'const char *' values.

//=============================================================================
//                        GLOBAL TYPEDEFS/CONSTANTS
//-----------------------------------------------------------------------------

typedef bdlb::NumericParseUtil Util;
    // De-facto standard abbreviation for utilities in testing

typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

//=============================================================================
//                             GLOBAL SETTINGS
//-----------------------------------------------------------------------------

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

//=============================================================================
//                     GLOBAL PRINT HELPER FUNCTIONS
//-----------------------------------------------------------------------------

                              // =========
                              // class Opt
                              // =========
class Opt {
    // Thin wrapper to safely print optional C-string (that may be a null
    // pointer) to an 'ostream'.

    // DATA
    const char *d_optCstr_p;

  public:
    // CREATORS
    explicit Opt(const char *optCstr) : d_optCstr_p(optCstr) {}

    // ACCESSORS
    const char *pointer() { return d_optCstr_p; }
};

bsl::ostream& operator<<(bsl::ostream& os, Opt o)
{
    return os << (o.pointer() ? o.pointer() : "(null)");
}

                         // ==================
                         // template class Qs_
                         // ==================

template <class StringType>
class Qs_ {
    // Thin wrapper to print strings in quotes to an 'ostream'.  Don't use it
    // directly, use the 'Qs' function.

    // DATA
    const StringType& d_string;

  public:
    // CREATORS
    explicit Qs_(const StringType& string) : d_string(string) {}

    // ACCESSORS
    const StringType& string() { return d_string; }
};

template <class StringType>
bsl::ostream& operator<<(bsl::ostream& os, Qs_<StringType> qs)
{
    return os << '"' << qs.string() << '"';
}

                          // template Qs(string)

template <class StringType>
Qs_<StringType> Qs(const StringType& string)
    // Return a 'Qs_' object with the specified 'string'.
{
    return Qs_<StringType>(string);
}

                         // ===================
                         // template class Qas_
                         // ===================

template <class StringType>
class Qas_ {
    // Thin wrapper to print abbreviated strings in quotes to an 'ostream'.
    // Don't use it directly, use the 'Qas' function.

    // DATA
    const StringType& d_string;

  public:
    // CREATORS
    explicit Qas_(const StringType& string) : d_string(string) {}

    // ACCESSORS
    const StringType& string() { return d_string; }
};

template <class StringType>
bsl::ostream& operator<<(bsl::ostream& os, Qas_<StringType> qas)
{
    const StringType& s = qas.string();

    if (s.empty()) {
        return os << "\"\"";                                          // RETURN
    }

    os << '"';

    typedef typename StringType::size_type Size;

    const Size k_NPOS = StringType::npos;
    for (Size i = 0; i < s.length();) {
        const char c = s[i];

        const Size pos     = s.find_first_not_of(c, i + 1);
        const Size nextPos = (k_NPOS == pos) ? s.size() : pos;
        const Size repLen  = nextPos - i;
        if (repLen > 10) {
            os << c << ".." << c << "{ " << repLen << " }";
        }
        else {
            os << bsl::string_view(&s[i], repLen);
        }
        i = nextPos;
    }

    return os << '"';
}

                          // template Qas(string)

template <class StringType>
Qas_<StringType> Qas(const StringType& string)
    // Return a 'Qas_' object with the specified 'string'.
{
    return Qas_<StringType>(string);
}

//=============================================================================
//                          GLOBAL HELPER TYPES
//-----------------------------------------------------------------------------

struct OpMode {
    // Orthogonal Perturbation Mode enumeration wrapper.

    enum Enum {
        e_NONE              = 0,  // Original input string from test table
        e_TRAILING          = 1,  // Trailing whitespace has no effect
        e_LEADING           = 2,  // Leading whitespace, always fails parsing
        e_EXPLICIT_POSITIVE = 3,  // Adding leading + if no sign in input
        e_NEGATIVE_SIGN     = 4,  // Adding leading - if no sign in input
        e_END
    };
};

bsl::ostream& operator<<(bsl::ostream& os, OpMode::Enum opMode)
    // Print the specified 'opMode' as human-readable text to the specified
    // 'os' output stream, then return 'os'.
 {
    switch (opMode) {
      case OpMode::e_NONE:                return os << "[unchanged-input]";
      case OpMode::e_TRAILING:            return os << "[trailing-whitespace]";
      case OpMode::e_LEADING:             return os << "[leading-whitespace]";
      case OpMode::e_EXPLICIT_POSITIVE:   return os << "[explicit-positive]";
      case OpMode::e_NEGATIVE_SIGN:       return os << "[negative-variant]";
      case OpMode::e_END:                 break; // clang/gcc enum warning off
    }
    return os << "[unknown-perturbation-mode]";
}

//=============================================================================
//                   TEST CASE FUNCTIONS & SUPPORT CODE
//-----------------------------------------------------------------------------

namespace testDouble {

// ============================================================================
// C GLOBAL 'errno' VERIFICATION SUPPORT CODE

                          // =================
                          // class StrtodErrno
                          // =================

class StrtodErrno {
    // This attribute type stores 'strtod' error values from the global 'errno'
    // of '<errno.h>'.  It also provides its own stream insert operator that
    // prints the macro name for expected errors, including also the anomalous
    // ones we know about.

  private:
    // DATA
    int d_errno;

  public:
    // CREATORS
    explicit StrtodErrno(int errnoValue) : d_errno(errnoValue) {}

    // ACCESSORS
    int value() const { return d_errno; }
};

bool operator==(int errnoValue, StrtodErrno obj)
{
    return errnoValue == obj.value();
}
bool operator==(StrtodErrno obj, int errnoValue)
{
    return errnoValue == obj;
}

bool operator!=(int errnoValue, StrtodErrno obj)
{
    return errnoValue != obj.value();
}
bool operator!=(StrtodErrno obj, int errnoValue)
{
    return errnoValue != obj;
}

bsl::ostream& operator<<(bsl::ostream& os, StrtodErrno obj)
{
    switch (obj.value()) {
      case 0: return os << "Success(0)";                              // RETURN

      case EINVAL: os << "'EINVAL'";  break;
      case ERANGE: os << "'ERANGE'";  break;
#if defined(__GLIBC__) && __GLIBC__ <= 2
      case EDOM:   os << "'EDOM'";    break;
#endif

      default:     os << "**unexpected-strtod-errno**";
    }

    return os << '(' << obj.value() << "): \"" << strerror(obj.value()) << '"';
}
                       // ========================
                       // class StrtodErrnoSampler
                       // ========================

class StrtodErrnoSampler {
    // This mechanism provides functionality to test 'strtod' that reports
    // errors via the global 'errno' of '<errno.h>'.  This type sets 'errno' to
    // zero on construction (to clear earlier errors).  The value of 'errno' is
    // then sampled by calling 'grab()' immediately after 'strtod'.  Equality
    // and non-equality operators to 'int' are provided that directly compare
    // the grabbed 'errno' value without the need to use an accessor.  This
    // allows direct comparison to '<errno.h>'-defined error number that are
    // 'int' literals.  'ErrnoSampler' has a stream insert operator that prints
    // the '<errno.h>' error (macro) name for expected values (including also
    // the possible anomalous ones that we know about).

  private:
    // DATA
    int d_errno;

  public:
    // CREATORS
    StrtodErrnoSampler() : d_errno(0) { errno = 0; }

    // MANIPULATORS
    void grab() { d_errno = errno; }

    // ACCESSORS
    int value() const { return d_errno; }
};

inline
bool operator==(int errnoValue, StrtodErrnoSampler obj)
{
    return errnoValue == obj.value();
}
inline
bool operator==(StrtodErrnoSampler obj, int errnoValue)
{
    return errnoValue == obj;
}

inline
bool operator!=(int errnoValue, StrtodErrnoSampler obj)
{
    return errnoValue != obj.value();
}
inline
bool operator!=(StrtodErrnoSampler obj, int errnoValue)
{
    return errnoValue != obj;
}

bsl::ostream& operator<<(bsl::ostream& os, StrtodErrnoSampler obj)
{
    return os << StrtodErrno(obj.value());
}

// ============================================================================
// GUARDS AND MONITORS

#ifdef u_NULL_STREAMBUFPTR_CRASHES_IOSTREAMS
    // In case a simple 'cout.rdbuf(0)' won't work we need an actual null
    // stream buffer object and its address.  We need only one as it has no
    // state whatsoever (only vtable of its do-nothing protected virtuals).
static struct NullBuffer : bsl::streambuf {} s_nullBuf;
#endif

                          // ======================
                          // class StreamStateGuard
                          // ======================

class StreamStateGuard {
    // This guard saves and restores the state of a standard iostream,
    // including "sticky" and user defined states.

  private:
    // DATA
    bsl::ios *d_stream_p;
#ifndef BSLS_PLATFORM_CMP_SUN
    bsl::ios  d_oldState;
#else
    // Solaris, of course, has non-standard iostreams.
    bsl::iostream d_oldState;
#endif

  public:
    // CREATORS
    explicit StreamStateGuard(bsl::ios& stream)
        // Create an 'StreamStateGuard' object that restores the complete state
        // os the specified 'stream' upon its destruction.
    : d_stream_p(&stream)
    , d_oldState(0)
    {
        d_oldState.copyfmt(stream);
    }

    ~StreamStateGuard()
        // Restore the state of the guarded stream then destroy this object.
    {
        d_stream_p->copyfmt(d_oldState);
    }
};
                       // ==========================
                       // class OstreamSilencerGuard
                       // ==========================

class OstreamSilencerGuard {
    // This guard type work using an 'ostream' and a 'verbosity' flag.  If the
    // flag is 'false' the guard object silences the output stream during
    // construction, and restores it during destruction (by replacing the
    // stream buffer).  If the verbosity flag is 'true' the guard will do
    // nothing.  The purpose of this type is to silence expected assertions
    // while testing test support code that asserts.

  private:
    // DATA
    bsl::ostream   *d_ostream_p;
    bsl::streambuf *d_streambuf_p;

  public:
    // CREATORS
    OstreamSilencerGuard(bsl::ostream& ostream, bool verbose)
        // Create an 'OstreamSilencerGuard' object that silences the specified
        // 'ostream' when the specified 'verbose' flag is 'false'.
    {
        if (verbose) {
            d_ostream_p = 0;
            return;                                                   // RETURN
        }

        d_ostream_p   = &ostream;
#ifndef u_NULL_STREAMBUFPTR_CRASHES_IOSTREAMS
        d_streambuf_p = d_ostream_p->rdbuf(0);
#else
        d_streambuf_p = d_ostream_p->rdbuf(&s_nullBuf);
#endif
    }

    ~OstreamSilencerGuard()
        // Restore the guarded 'ostream' if it was silenced upon construction.
    {
        if (d_ostream_p) {
            d_ostream_p->rdbuf(d_streambuf_p);
        }
    }
};
                           // =======================
                           // class TestAssertCounter
                           // =======================
class TestAssertCounter {
    // This mechanism enables counting test assertion failures that occur in a
    // in a block/part of code by setting 'testStatus' to 0 at construction and
    // properly updating it with the observed changes of 'testStatus' on
    // destruction, or when requested.

  public:
    // PUBLIC CONSTANTS
    static const int k_MAX_TESTSTATUS = 100;

  private:
    // DATA
    int  d_testStatus;
    bool d_active;

  private:
    // NOT IMPLEMENTED
    TestAssertCounter&operator=(const TestAssertCounter&) BSLS_KEYWORD_DELETED;
    TestAssertCounter(          const TestAssertCounter&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    TestAssertCounter();
        // Create an activated test status counter object and save the current
        // value of the global 'testStatus' then set that 'testStatus' to 0.

    ~TestAssertCounter();
        // If this object is active call 'release()'.  Destroy this object.

    // ACCESSORS
    bool isActive() const;
        // Return 'true' if the object is active, otherwise return 'false'.

    // MANIPULATORS
    void release();
        // Set the value of the global 'testStatus' to the value it have had if
        // this object had never existed, then deactivate this object.  The
        // behavior is undefined unless the object is active.

    void reset();
        // Update this object as if it had just been created at the point of
        // the call and reactivate the object if it was inactive.
};
                         // -----------------------
                         // class TestAssertCounter
                         // -----------------------

#ifdef BSLS_PLATFORM_CMP_SUN
// PUBLIC CONSTANTS
const int TestAssertCounter::k_MAX_TESTSTATUS;
    // Oracle Developer Studio insists on having an object even though we never
    // take the address of this.
#endif

// CREATORS
TestAssertCounter::TestAssertCounter()
: d_testStatus(testStatus)
, d_active(true)
{
    testStatus = 0;
}

TestAssertCounter::~TestAssertCounter()
{
    if (d_active) {
        release();
    }
}

// ACCESSORS
bool TestAssertCounter::isActive() const
{
    return d_active;
}

// MANIPULATORS
void TestAssertCounter::release()
{
    ASSERT(d_active);

    d_active = false;

    if (testStatus < 0) {
        return;                                                       // RETURN
    }

    if (d_testStatus < 0 || d_testStatus >= k_MAX_TESTSTATUS) {
        testStatus = d_testStatus;
    }
    else if (testStatus + d_testStatus > k_MAX_TESTSTATUS) {
        testStatus = k_MAX_TESTSTATUS;
    }
    else {
        testStatus += d_testStatus;
    }
}

void TestAssertCounter::reset()
{
    if (d_active) {
        release();
    }

    d_testStatus = testStatus;
    testStatus   = 0;
    d_active     = true;
}
                          // =========================
                          // class BslsLogCounterGuard
                          // =========================

class BslsLogCounterGuard {
    // Depending on a verbosity flag install either a counting 'bsls' log
    // handler or a counting and printing log handler on construction and
    // restore the original handler on destruction.  It counts the log messages
    // using a class data member.

  private:
    // TYPES
    typedef bsls::Log::LogMessageHandler LogMessageHandler;

  private:
    // DATA

  private:
    // CLASS DATA
    static LogMessageHandler s_originalHandler;
    static int               s_logMessageCounter;

  private:
    // PRIVATE CLASS METHODS
    static void countingCallingHandler(bsls::LogSeverity::Enum  severity,
                                        const char             *file,
                                        int                     line,
                                        const char             *message);
        // Count log by incrementing 's_logMessageCounter' then call
        // 's_originalHandler' using the specified 'severity', 'file', 'line',
        // and 'message'.

    static void countingHandler(bsls::LogSeverity::Enum,
                                const char *,
                                int,
                                const char *);
        // Count calls by incrementing 's_logMessageCounter'.

  public:
    // CREATORS
    explicit BslsLogCounterGuard(bool verbose);
        // Create a 'BslsLogCounterGuard' object.  Set the call counter to
        // zero.  If the specified 'verbose' is 'false' install a call-counting
        // log message handler, otherwise install a handler that counts as well
        // as calls the original log message handler to print the message.  The
        // behavior is undefined unless no other object of this type exist.

    ~BslsLogCounterGuard();
        // Restore the original log message handler from before the creation of
        // this object, then destroy this object.

    // CLASS "MANIPULATORS"
    static void resetCounter();
        // Set the log message counter to zero.

    // CLASS "ACCESSORS"
    static int counter();
        // Return the current value of the log message counter.
};
                       // -------------------------
                       // class BslsLogCounterGuard
                       // -------------------------

// PRIVATE CLASS DATA
bsls::Log::LogMessageHandler BslsLogCounterGuard::s_originalHandler   = 0;
int                          BslsLogCounterGuard::s_logMessageCounter = 0;

// PRIVATE CLASS METHODS
void BslsLogCounterGuard::countingCallingHandler(
                                             bsls::LogSeverity::Enum  severity,
                                             const char              *file,
                                             int                      line,
                                             const char              *message)
{
    ++s_logMessageCounter;
    s_originalHandler(severity, file, line, message);
}

void BslsLogCounterGuard::countingHandler(bsls::LogSeverity::Enum,
                                          const char *,
                                          int,
                                          const char *)
{
    ++s_logMessageCounter;
}

// CREATORS
BslsLogCounterGuard::BslsLogCounterGuard(bool verbose)
{
    ASSERT(0 == s_originalHandler);

    s_logMessageCounter = 0;

    s_originalHandler = bsls::Log::logMessageHandler();

    bsls::Log::setLogMessageHandler(verbose
                                  ? countingCallingHandler
                                  : countingHandler);
}

BslsLogCounterGuard::~BslsLogCounterGuard()
{
    bsls::Log::setLogMessageHandler(s_originalHandler);
    s_originalHandler = 0;
}

// CLASS "MANIPULATORS"
void BslsLogCounterGuard::resetCounter()
{
    s_logMessageCounter = 0;
}

// CLASS "ACCESSORS"
int BslsLogCounterGuard::counter()
{
    return s_logMessageCounter;
}

                       // ===========================
                       // class BslsReviewMonitorUtil
                       // ===========================

class BslsReviewMonitorUtil {
    // A simple stateful utility that monitors 'BSLS_REVIEW' failures and
    // verifies that only expected messages from the tested component occur.
    // Do not use this utility directly, use 'BslsReviewMonitor'.

  private:
    // CLASS DATA
    static bsls::Review::ViolationHandler s_originalReviewHandler;

    static bool s_verbose;

    static int  s_numHexfloatReviews;
    static int  s_numOverflowReviews;
    static int  s_numUnderflowReviews;

  private:
    // PRIVATE CLASS METHODS

                       // VIOLATION PREDICATES RELATED

    static bool containsCaseless(const bsl::string_view& string,
                                 const bsl::string_view& subString);
        // Return 'true' if the specified 'subString' is present in the
        // specified 'string' disregarding case of alphabet characters
        // '[a-zA-Z]', otherwise return 'false'.

    static bool isInComment(const char *comment, const char *subString);
        // Return 'true' if the specified 'comment' is not a null pointer and
        // contains the specified 'subString', case insensitive, otherwise
        // return 'false'.

    static bool maybeRightFilename(const char *fileName);
        // Return 'true' if the specified 'fileName' is a null pointer, or an
        // empty null-terminated string, or contains the name of this
        // component, case insensitive, immediately followed by a dot,
        // otherwise return 'false'.

                     // 'bsls::Review::ViolationHandler'

    static void monitoringHandler(const bsls::ReviewViolation& violation);
        // If 'd_verbose' is 'true' "print" the specified 'violation' using the
        // 'bsls::Review::failByLog' handler.  Determine if the 'violation' is
        // one of the valid review violations from the tested method
        // ('bsls::NumericParseutil::parseDouble').  In case  'violation' is
        // unexpected 'ASSERT' and call the original violation handler stored
        // by the constructor, unless 'd_verbose' is 'true' and the original
        // handler is 'bsls::Review::failByLog', then return.  In case
        // 'violation' is an expected one update the corresponding counter,
        // then return.

                               // Handler Helpers

    static bool handleIfExpected(const bsls::ReviewViolation& violation);
        // Determine if the specified 'violation' is expected and update the
        // corresponding counter if it is, and return 'true', otherwise return
        // 'false'.

    static bool handleIfHexfloat(const char *comment);
        // If the specified 'comment' is not from a hexfloat related review
        // violation do nothing and return 'false', otherwise return 'true'.

    static bool handleIfOverflow(const char *comment);
        // If the specified 'comment' is not from an overflow related review
        // violation do nothing and return 'false', otherwise return 'true'.

    static bool handleIfUnderflow(const char *comment);
        // If the specified 'comment' is not from an underflow related review
        // violation do nothing and return 'false', otherwise return 'true'.

  public:
    // CLASS METHODS            // Setup/Tear-down
    static void start();
        // Save the original handler, install 'monitoringHandler' as the bsls
        // review violation handler.  The behavior is undefined unless
        // 'false == wasStarted()'.

    static void stop();
        // Restore the saved original bsls review violation handler stored by
        // 'installHandler' and set the original handler to null.  The behavior
        // is undefined unless 'true == isActive()'.

                            // Monitoring Status Helpers

    static bool isActive();
        // Return 'true' if 'true == wasStarted()' and also the installed
        // review violation handler is 'monitoringHandler', otherwise return
        // 'false'.

    static bool wasStarted();
        // Return 'true' if there is a saved original bsls review violation
        // handler, otherwise return 'false'.

    static bool isVerbose();
        // Return 'd_verbose'.

                                // Counter Related
    static int numHexfloat();
        // Return the hexfloat review violation counter.

    static int numOverflow();
        // Return the overflow review violation counter.

    static int numUnderflow();
        // Return the underflow review violation counter.

    static void resetCounters();
        // Set all counters to 0.

                               // Verbosity Related

    static void setVerbose(bool verbosity);
        // Set verbosity to the specified 'verbosity'.
};
                         // =======================
                         // class BslsReviewMonitor
                         // =======================

class BslsReviewMonitor {
    // A mechanism that monitors 'BSLS_REVIEW' failures and verifies that only
    // expected messages from the tested component occur.  Note that no more
    // than one instance of this type is allowed to exist.

  public:
    // CREATORS
    explicit BslsReviewMonitor(bool verbose);
        // Create a 'BslsReviewMonitor' object, set the monitoring verbosity
        // to the specified 'verbose', reset all counters to zero, and finally
        // start monitoring bsls review failures.  The behavior is undefined
        // unless no other 'BslsReviewMonitor' objects exists or in other words
        // 'false == BslsReviewMonitorUtil::wasStarted()'.

    ~BslsReviewMonitor();
        // Call 'BslsReviewMonitorUtil::stop()' to stop monitoring and destroy
        // this object.

    // MANIPULATORS
    void resetCounters();
        // Set all expected review violation counters to zero.

    // ACCESSORS
    bool isVerbose() const;
        // Return 'true' if this object has been instantiated in 'verbose'
        // mode, otherwise return 'false'.

    int numHexfloat() const;
    int numOverflow() const;
    int numUnderflow() const;
        // Return the number of observed review failures since construction or
        // the last call to 'resetCounters'.
};
                        // ---------------------------
                        // class BslsReviewMonitorUtil
                        // ---------------------------
// CLASS DATA
bsls::Review::ViolationHandler BslsReviewMonitorUtil::s_originalReviewHandler;

bool BslsReviewMonitorUtil::s_verbose = false;

int BslsReviewMonitorUtil::s_numHexfloatReviews  = 0;
int BslsReviewMonitorUtil::s_numOverflowReviews  = 0;
int BslsReviewMonitorUtil::s_numUnderflowReviews = 0;

// PRIVATE CLASS METHODS

                     // VIOLATION PREDICATES RELATED
inline
bool BslsReviewMonitorUtil::containsCaseless(const bsl::string_view& string,
                                             const bsl::string_view& subString)
{
    if (subString.empty()) {
        return true;                                                  // RETURN
    }

    typedef bdlb::StringViewUtil SVU;
    const bsl::string_view rsv = SVU::strstrCaseless(string, subString);

    return !rsv.empty();
}

inline
bool BslsReviewMonitorUtil::isInComment(const char *comment,
                                        const char *subString)
{
    return comment && containsCaseless(comment, subString);
}

inline
bool BslsReviewMonitorUtil::maybeRightFilename(const char *fileName)
{
    if (0 == fileName || '\0' == *fileName) {
        // It could have been "bsls_numericparseutil.cpp"
        return true;                                                  // RETURN
    }

    return containsCaseless(fileName, "bdlb_numericparseutil.");
}

                    // 'bsls::Review::ViolationHandler'

void BslsReviewMonitorUtil::monitoringHandler(
                                        const bsls::ReviewViolation& violation)
{
    if (s_verbose) {
        bsls::Review::failByLog(violation);
    }

    const bool expectedReviewViolation = handleIfExpected(violation);

    ASSERTV(violation.reviewLevel(),
            Opt(violation.fileName()),
            violation.lineNumber(),
            Opt(violation.comment()),
            violation.count(),
            expectedReviewViolation);

    if (!expectedReviewViolation &&
        (!s_verbose || &bsls::Review::failByLog != s_originalReviewHandler)) {
        // Unexpected violations get the original handler called unless we have
        // called it already, because it is 'failByLog' and we are verbose.
        s_originalReviewHandler(violation);
    }
}
                             // Handler Helpers
inline
bool BslsReviewMonitorUtil::handleIfExpected(
                                        const bsls::ReviewViolation& violation)
{
    return maybeRightFilename(violation.fileName())
        && (handleIfHexfloat(violation.comment())  ||
            handleIfUnderflow(violation.comment()) ||
            handleIfOverflow(violation.comment()));
}

inline
bool BslsReviewMonitorUtil::handleIfHexfloat(const char *comment)
{
    const bool isMyThing = isInComment(comment, "hexfloat");
    if (isMyThing) {
        ++s_numHexfloatReviews;
    }

    return isMyThing;  // Chain of Responsibilities Pattern
}

inline
bool BslsReviewMonitorUtil::handleIfOverflow(const char *comment)
{
    const bool isMyThing = isInComment(comment, "overflow");
    if (isMyThing) {
        ++s_numOverflowReviews;
    }

    return isMyThing;  // Chain of Responsibilities Pattern
}

inline
bool BslsReviewMonitorUtil::handleIfUnderflow(const char *comment)
{
    const bool isMyThing = isInComment(comment, "underflow");
    if (isMyThing) {
        ++s_numUnderflowReviews;
    }

    return isMyThing;  // Chain of Responsibilities Pattern
}

// CLASS METHODS
                            // Setup/Tear-down
inline
void BslsReviewMonitorUtil::start()
{
    ASSERT(!wasStarted());

    s_originalReviewHandler = bsls::Review::violationHandler();

    bsls::Review::setViolationHandler(&monitoringHandler);
}

inline
void BslsReviewMonitorUtil::stop()
{
    ASSERT(isActive());

    bsls::Review::setViolationHandler(s_originalReviewHandler);
    s_originalReviewHandler = 0;
}
                        // Monitoring Status Helpers
inline
bool BslsReviewMonitorUtil::isActive()
{
    return wasStarted() &&
                        &monitoringHandler == bsls::Review::violationHandler();
}

inline
bool BslsReviewMonitorUtil::wasStarted()
{
    return s_originalReviewHandler != 0;
}

inline bool BslsReviewMonitorUtil::isVerbose()
{
    return s_verbose;
}

                             // Counter Related
inline
int BslsReviewMonitorUtil::numHexfloat()
{
    return s_numHexfloatReviews;
}

inline
int BslsReviewMonitorUtil::numOverflow()
{
    return s_numOverflowReviews;
}

inline
int BslsReviewMonitorUtil::numUnderflow()
{
    return s_numUnderflowReviews;
}

inline
void BslsReviewMonitorUtil::resetCounters()
{
    s_numHexfloatReviews  = 0;
    s_numOverflowReviews  = 0;
    s_numUnderflowReviews = 0;
}
                            // Verbosity Related
inline
void BslsReviewMonitorUtil::setVerbose(bool verbosity)
{
    s_verbose = verbosity;
}
                         // -----------------------
                         // class BslsReviewMonitor
                         // -----------------------
// CREATORS
inline
BslsReviewMonitor::BslsReviewMonitor(bool verbose)
{
    ASSERT(!BslsReviewMonitorUtil::wasStarted());

    BslsReviewMonitorUtil::setVerbose(verbose);
    BslsReviewMonitorUtil::resetCounters();
    BslsReviewMonitorUtil::start();
}

inline
BslsReviewMonitor::~BslsReviewMonitor()
{
    BslsReviewMonitorUtil::stop();
}

// MANIPULATORS
inline
void BslsReviewMonitor::resetCounters()
{
    BslsReviewMonitorUtil::resetCounters();
}

// ACCESSORS
inline
bool BslsReviewMonitor::isVerbose() const
{
    return BslsReviewMonitorUtil::isVerbose();
}

inline
int BslsReviewMonitor::numHexfloat() const
{
    return BslsReviewMonitorUtil::numHexfloat();
}

inline
int BslsReviewMonitor::numOverflow() const
{
    return BslsReviewMonitorUtil::numOverflow();
}

inline
int BslsReviewMonitor::numUnderflow() const
{
    return BslsReviewMonitorUtil::numUnderflow();
}
                        // Null Review Handler

void nullReviewHandler(const bsls::ReviewViolation&)
    // This 'BSLS_REVIEW' handler does nothing.  Used in benchmarking.
{
    // This function is intentionally empty.
}

// ============================================================================
// FREQUENTLY USED TYPE ABBREVIATIONS

typedef bsl::numeric_limits<double> Limits;

// ============================================================================
// FLOAT CLASSIFICATION FUNCTION "ABBREVIATIONS"

bool isInf(     double d) { return bdlb::Float::isInfinite(d); }
bool isNan(     double d) { return bdlb::Float::isNan(     d); }
bool isNegative(double d) { return bdlb::Float::signBit(   d); }

// ============================================================================
// PARSE DATA TEST DATA SPECIFICATION ALIASES, CONSTANTS, AND FUNCTIONS

typedef bsl::ptrdiff_t ParsedChars;
    // For human-readable "what was parsed" column.  Positive numbers count
    // from the start of string, larger in absolute value (than the string
    // length) negative numbers (see 'ALL' below) mean the whole string was
    // parsed (successfully), smaller negative numbers describe the number of
    // *not* parsed characters, counting from the beginning of the string.
    // Zero means failure (no characters were successfully parsed).

static const ParsedChars All = bsl::numeric_limits<ParsedChars>::min();

bsl::size_t calcRestPos(ParsedChars parsedChars, bsl::size_t length)
    // Convert the specified 'parsedChars' that is a possibly negative number
    // of characters to an index into an input string with the specified
    //  'length' and return it as follows:
    //:
    //: o Return a non-negative 'parsedChars' value as is.
    //:
    //: o If '-parsedChars >= length' return 'length',
    //:
    //: o otherwise return 'length + parsedChars'.
    //
    // Essentially positive 'parsedChars' values indicate exact positions,
    // large negative values (see 'All' earlier) represent the whole string
    // parsed successfully, and smaller negative values indicate the number of
    // characters from the end of the string that were not parsed.
{
    if (0 <= parsedChars) {  // Positive or zero
        return parsedChars;                                           // RETURN
    }

    if (static_cast<bsl::size_t>(-parsedChars) > length) { // large negative
        return length;                                                // RETURN
    }

    // Small negative position counts from the end of the string (of 'length')
    return length + parsedChars;
}

bsl::size_t calcRestPosForNoHexfloat(bool                    isHex,
                                     const bsl::string_view& input,
                                     ParsedChars             parsedChars)
    // Calculate the position of the first not-parsed character for platforms
    // that do not support parsing hexadecimal floating point.  Such platforms
    // will parse the '0' of the hex prefix into a 0, regardless of the rest of
    // the input after the 'x' or 'X'.  Non hexfloat input (the specified
    // 'isHex' is 'false') is not different, so we delegate to'calcRestpos'
    // using the specified 'parsedChars' value, and the length of the specified
    // 'input'.  When 'isHex' is 'true' the function returns the position of
    // the first 'x' or 'X' character of 'input'.  The behavior is undefined
    // if 'isHex' is 'true', but the 'input' does not start with a hex prefix
    // ("0x" or "0X") optionally preceded by a sign character ('+'/'-').
    // Notice that the 'parsedChars' argument value is deliberately ignored in
    // case of a hexfloat 'input'.
{
    ASSERTV(input,
            !isHex || input.find_first_of("xX") != bsl::string_view::npos);

    return isHex
        ? input.find_first_of("xX")
        : calcRestPos(parsedChars, input.length());
}

static const double Inf = Limits::infinity();
static const double NaN = Limits::quiet_NaN();

static const double NtA = -42e42;
    // [N]o[t] [A]pplicable, expected result for failure (readability)

double parseExpected(const bsl::string_view& sv)
    // This function exists only because lack of proper hexfloat literal
    // support and support for hexfloat in 'strtod' on Solaris.  We want to
    // *precisely* specify the expected 'parseDouble' result, and in a way that
    // is still readable for humans.  That needs hexfloat.
    //
    // Convert the specified 'sv' string view to a 'double'.  The behavior is
    // undefined unless 's' is:
    //: o a simple, normalized hexfloat
    //: o a small (less than 2^54) decimal unsigned integer
    //: o the letters "NtA"  -- [No]o[t] [A]pplicable
    //: o the letters "NaN"  -- Not a Number
    //: o the letters "Inf"  -- Infinity
    //
    // A simple hexfloat starts with "0x", followed by a single integer digit
    // of '0' or '1'. It has an optional fractional part that immediately
    // follows the integer digit and starts with a '.' followed by minimum 0
    // and maximum 13 hexadecimal digits.  It has a mandatory binary exponent
    // that follows the optional fraction or the integer part if a fraction is
    // not present.  The exponent part consists of the letter 'p' followed by
    // an optional '+' or '-' sign followed by a decimal number in the range
    // -1022 and +1023.  If the integer part is 0 the exponent value must be 0
    // or -1022.  No value that would over or underflow (not fit an IEEE
    // binary64) is allowed.
    //
    // Notice that no sign before the value is allowed.  This is because we
    // verify the explicit positive and the negative variant of every input
    // string automatically by adding the sign before the string, so there is
    // no reason to use sign in the input.  (The domain of the signed and
    // unsigned values of 'double' is the same in absolute values.)
    //
    // "NtA" 'input' returns 'NtA', "NaN" and "Inf" 'input' return 'NaN' and
    // 'Inf' respectively.  Other allowed 'input' returns the exact
    // corresponding positive integer, normal, or subnormal 'double' value.
{
    typedef bsl::string_view::size_type Position;
    typedef bsl::string_view::size_type Size;

    static const Position k_NPOS = bsl::string_view::npos;

    if ("0" == sv) {
        return 0;                                                     // RETURN
    }
    else if ("NtA" == sv) {
        return NtA;                                                   // RETURN
    }
    else if ("NaN" == sv) {
        return NaN;                                                   // RETURN
    }
    else if ("Inf" == sv) {
        return Inf;                                                   // RETURN
    }

    // A small positive integer less than 2^54
    if (sv.substr(0, 2) != "0x") {
        unsigned long long uint = 0;
        const int rc = Util::parseUnsignedInteger(&uint,
                                                  sv,
                                                  10,
                                                  0x1FFFFFFFFFFFFFFull);
        ASSERTV(rc, 0 == rc);

        return static_cast<double>(uint);                             // RETURN
    }

    ASSERTV(sv[2], sv[2] == '1' || sv[2] == '0');
    ASSERTV(sv.length(), sv.substr(3, 1),
            sv.length() < 4 || sv[3] == '.' || sv[3] == 'p');

    const unsigned intPart = sv[2] - '0';

    const Position dotPos    = sv.find('.');
    const Position expMrkPos = sv.find('p');
    ASSERT(k_NPOS != expMrkPos);
    ASSERTV(dotPos, expMrkPos,
           k_NPOS == dotPos || dotPos < expMrkPos);

    unsigned long long bits = 0;

    if (k_NPOS != dotPos) {  // There are fractional digits
        const Position fracPos = dotPos + 1;
        const Size     fracLen = (k_NPOS ==  expMrkPos)
                                ? sv.size() - fracPos
                                : expMrkPos - fracPos;
        ASSERTV(fracLen, 13 >= fracLen);
        if (fracLen > 0) {
            const bsl::string_view fraction = sv.substr(fracPos, fracLen);
            const int rc = Util::parseUnsignedInteger(&bits,
                                                      fraction,
                                                      16,
                                                      0xFFFFFFFFFFFFFFull);
            ASSERTV(rc, 0 == rc);

            // Apply the missing hex trailing zeros
            bits <<= 4 * (13 - fraction.length());
        }
    }

    if (0 != intPart) {  // Zero or subnormal has all-0 exponent bits
        int exponent = 0;
        const bsl::string_view expChars = sv.substr(expMrkPos + 1);
        ASSERTV(expChars.length(), expChars.length() > 0);

        // the exponent is expressed in base 10 but it is a binary exponent
        const int rc = Util::parseInt(&exponent, expChars, 10);
        ASSERTV(rc, 0 == rc);
        ASSERTV(intPart, exponent,
                intPart != 0 || -1022 == exponent || 0 == exponent);

        exponent += 1023;                  // Apply IEEE exponent bias
        ASSERTV(exponent, exponent >= 0);  // Biased must be positive
        unsigned long long expBits = static_cast<Uint64>(exponent) << 52;
        bits |= expBits;
    }

    double rv;
    memcpy(&rv, &bits, sizeof rv);

    return rv;
}

// ============================================================================
// PARSE DOUBLE TEST DATA TYPES

struct SpecialFlags {
    // This complex constrained attribute type stores directly publicly
    // accessible flags that describe certain special input values in
    // 'parseDouble' test data.
    //
    ///Attributes
    ///----------
    //..
    //  Name                Type  Default  Simple Constraints
    //  ------------------  ----  -------  ------------------
    //  isHex               bool  none     none
    //  uflwInSignificand   bool  none     none
    //  isUnderflow         bool  none     none
    //  isOverflow          bool  none     none
    //  isSubnormal         bool  none     none
    //..
    //
    ///Complex Constrains
    ///------------------
    ///
    //: +-------------------+-----------------------------------------+
    //: | Flags             | Constraint one when can be set ('true') |
    //: +===================+=========================================+
    //: | isHex             | none                                    |
    //: +-------------------+-----------------------------------------+
    //: | isUnderflow       | these 3 flags are mutually exclusive,   |
    //: | isOverflow        | only one of them may be 'true'.         |
    //: | isSubnormal       |                                         |
    //: +-------------------+-----------------------------------------+
    //: | uflwInSignificand | true == isOverflow, in other words this |
    //: |                   | is a special flavor of underflow        |
    //: +-------------------+-----------------------------------------+
    //: | subnormalIsMin    | true == isSubnormal, in other words     |
    //: |                   | this is a special subnormal             |
    //: +-------------------+-----------------------------------------+
    //
    // 'uflwInSignificand': GNU libc 'strtod' behave differently when it
    //                      converts a number that has smallest representable
    //                      effective decimal exponent -324 but its binary
    //                      significand rounds to zero when trying to fit it
    //                      into the available 52 bits with binary exponent
    //                      -1022.  This is a range of very small numbers that
    //                      have the largest negative subnormal
    //                      decimal exponent -324 but the significand rounds to
    //                      less than that of the smallest subnormal
    //                      'bsl::numeric_limits<double>denorm_min()'.
    //
    // 'subnormalIsMin'    - GNU libc has an anomaly when parsing the exact
    //                       value of 'numeric_limits<>::denorm_min()'
    //
    ///String Literal Format
    ///---------------------
    //
    // For ease of readability the flags are set using a string literal that we
    // then translate to these boolean values.  See also 'stringToFlags'.  The
    // above constraints translate to the following possible literals:
    //
    //: +------+---+-----+---+-----+
    //: | str  |   | u   | o | d i |
    //: | ing  |   | n t | v | e s |
    //: |      | h | d i | e | n m |
    //: | lit  | e | e n | r | r i | ,----------------------------------------+
    //: | eral | x | r y | f | m n |/               Description               |
    //: +======+===+=====+===+=====+==========================================+
    //: | ""   | f | f f | f | f f | normal number in fixed/scientific format |
    //: |      |   |     |   |     | or parsing error(**)                     |
    //: +------+---+-----+---+-----+------------------------------------------+
    //: | "H"  | T | f f | f | f f | normal number in hexfloat format         |
    //: +------+---+-----+---+-----+------------------------------------------+
    //: | "U"  | f | T f | f | f f | underflow in fixed or scientific format  |
    //: +------+---+-----+---+-----+------------------------------------------+
    //: | "u"  | f | T T | f | f T | significand-only underflow in            |
    //: |      |   |     |   |     | fixed/scientific format                  |
    //: +------+---+-----+---+-----+------------------------------------------+
    //: | "O"  | f | f f | T | f T | overflow in fixed or scientific format   |
    //: +------+---+-----+---+-----+------------------------------------------+
    //: | "HU" | T | T f | f | f f | underflow in hexfloat format             |
    //: +------+---+-----+---+-----+------------------------------------------+
    //: | "Hu" | T | T T | f | f T | significand-only underflow in hexadecimal|
    //: |      |   |     |   |     | format                                   |
    //: +------+---+-----+---+-----+------------------------------------------+
    //: | "HO" | T | f f | T | f f | overflow in hexfloat format              |
    //: +------+---+-----+---+-----+------------------------------------------+
    //: | "D"  | f | f f | f | f f | subnormal in fixed or scientific format  |
    //: +------+---+-----+---+-----+------------------------------------------+
    //: | "d"  | f | f f | f | T T | 'denorm_min()' in fixed/scientific fmt   |
    //: +------+---+-----+---+-----+------------------------------------------+
    //: | "HD" | T | f f | f | T f | subnormal in hexfloat format             |
    //: +------+---+-----+---+-----+------------------------------------------+
    //: | "Hd" | T | f f | f | T T | 'denorm_min()' in hexfloat format        |
    //: +------+---+-----+---+-----+------------------------------------------+
    //: | "hd" | T | f f | f | F F | hexfloat 'denorm_min()/2' GNU bug(***)   |
    //: +------+---+-----+---+-----+------------------------------------------+
    //: | str  | h | u t | o | d i |\              Description                |
    //: | ing  | e | n i | v | e s | `----------------------------------------+
    //: | eral | x | d n | e | n M |
    //: | lit  |   | e y | r | r i |
    //: |      |   | r   | f | m n |
    //: +------+---+-----+---+-----+
    //:
    //:(**) Every other string literal must accompany an input that is, or
    //:     begins-with a parsable 'double' that fulfills to the description.
    //:
    //: (***) See 'u_GLIBC2_STRTOD_HEX_HALF_DENORM_MIN_HEX_BUG'.  This sets
    //:       'd_isHexHalfSubnormMin' to 'true', while all other flags
    //:       combinations set 'd_isHexHalfSubnormMin' to 'false'.  That flag
    //:       is used only in the expectations-verifier when the 'strtod' bug
    //:       is expected to be present.

    // PUBLIC DATA
    bool d_isHex;
    bool d_isUnderflow;
    bool d_significandUnderflow;
    bool d_isOverflow;
    bool d_isSubnormal;
    bool d_subnormIsMin;
    bool d_isHexHalfSubnormMin;
};

bsl::ostream& operator<<(bsl::ostream& os, const SpecialFlags& flags)
{
    os << '"';

    const char hexFlag = (flags.d_isHexHalfSubnormMin ? 'h' : 'H');

    if (flags.d_isHex)                os << hexFlag;
    if (flags.d_significandUnderflow) os << 'u';
    else if (flags.d_isUnderflow)     os << 'U';
    else if (flags.d_isOverflow)      os << 'O';
    else if (flags.d_subnormIsMin)    os << 'd';
    else if (flags.d_isSubnormal)     os << 'D';

    return os << '"';
}

SpecialFlags stringToFlags(const char *string)
    // Return 'SpecialFlags' filled out according to the specified 'string',
    // properly ignoring hexfloat flags for platforms that do not support
    // parsing that format.
    //
    //: "H"  | hexfloat
    //: "U"  | underflow
    //: "u"  | Significand-only underflow
    //: "O"  | overflow
    //: "HU" | hex and underflow
    //: "Hu" | hex and decimal significand-only underflow
    //: "HO" | hex and overflow
    //: "D"  | subnormal
    //: "HD" | hexfloat subnormal
    //: "d"  | sci/fixed 'denorm_min()'
    //: "Hd" | hexfloat 'denorm_min()'
    //: "hd" | hexfloat 'denorm_min() / 2'
    //
    // See 'SpecialFlags' for a detailed description.
{
    using bdlb::CharType;

    const bool k_IS_HEX      = ('H' == CharType::toUpper(string[0]));

#ifndef u_BDLB_NUMERICPARSEUTIL_SUPPORT_PARSING_HEXFLOAT
    // On platforms not supporting hexfloat parsing the hex prefix will be
    // parsed as zero ("0x"/"0X"), so no overflow, underflow, or subnormals are
    // possible.  Therefore all flags should be 'false' except for 'isHex', as
    // we still get our own 'BSLS_REVIEW_OPT'.  Note that all platforms that
    // support 'from_chars' do support 'chars_format::hex'.
    if (k_IS_HEX) {
        static const SpecialFlags k_JUST_HEX = { true };
        return k_JUST_HEX;                                            // RETURN
    }
#endif

    const bsl::size_t POS = k_IS_HEX ? 1 : 0;

    SpecialFlags rv = {
        k_IS_HEX,                                 // isHex
        ('U' == CharType::toUpper(string[POS])),  // isUnderflow
        ('u' == string[POS]),                     // significandUnderflow
        ('O' == string[POS]),                     // isOverflow
        ('D' == CharType::toUpper(string[POS])),  // isSubnormal
        ('d' == string[POS]),                     // subnormIsMin
        (k_IS_HEX && 'h' == string[0])            // isHexHalfSubnormMin
    };

    return rv;
}
                        // ==================
                        // struct TestDataRow
                        // ==================
struct TestDataRow {
    // One line of test data for 'parseDouble' tests (and benchmarking).

    typedef bsl::string_view string_view;

    int          d_line;        // source line number
    string_view  d_input;       // base input string to parse
    double       d_expected;    // expected value if parsed successfully
    bsl::size_t  d_restPos;     // position of the first non-parsed character
    SpecialFlags d_flags;       // See 'SpecialFlags' and 'stringToFlags()'.

    string_view  d_expLiteral;  // literal form of 'expected' used to overcome
};                              // no-support for hexfloat literal in compiler

// ============================================================================
// PARSE DOUBLE TEST DATA DEFINITION AND ITS PORTABILITY MACROS

#ifdef BSLS_COMPILERFEATURES_SUPPORT_HEXFLOAT_LITERALS
    #define ROW(input, expected, offset, sflags)                              \
        { L_, input,                                                          \
              expected,                                                       \
              calcRestPos(offset, strlen(input)),                             \
              stringToFlags(sflags),                                          \
              #expected                                                       \
        }
        // Define a table row using the specified 'input', 'expected' value,
        // expected 'offset' specification, and 'sflags' special flags.
        //
        // Add the string-form of the 'expected' value in order to verify the
        // parser function for the platforms that do not support hexadecimal
        // floating point literals, and to allow better for experience by
        // asserting *exactly* what the expected value was in the table
        // regardless of stream format settings.
#elif defined(u_BDLB_NUMERICPARSEUTIL_SUPPORT_PARSING_HEXFLOAT)
    // This is AIX, has trouble with some literals but its 'strtod' has working
    // hexfloat support.
    #define ROW(input, expected, offset, sflags)                              \
        { L_, input,                                                          \
              parseExpected(#expected),                                       \
              calcRestPos(offset, strlen(input)),                             \
              stringToFlags(sflags),                                          \
              #expected                                                       \
        }
#else
    // This is Solaris, no hexfloat support of any kind so we need to fake that
    // by "parsing" everything hex into 0, and parsing the expected literals by
    // a function into a 'double'.
    #define ROW(input, expected, offset, sflags)                              \
        { L_, input,                                                          \
              ('H' != *sflags) ? parseExpected(#expected) : 0.0,              \
              calcRestPosForNoHexfloat('H' == *sflags || 'h' == *sflags,      \
                                       input, offset),                        \
              stringToFlags(sflags),                                          \
              #expected                                                       \
        }
#endif

#define PROW(input, expected) ROW(input, expected, All, "")
    // Abbreviation for long rows that are expected to pass and have no special
    // flags (not subnormal, not hexfloat format, no any under/overflow).

#define HPROW(input, expected) ROW(input, expected, All, "H")
    // Abbreviation for long hexfloat rows that are expected to pass and have
    // no other special flags (not subnormal, no any under/overflow).

#define HDROW(input, expected) ROW(input, expected, All, "HD")
    // Abbreviation for long hexfloat subnormal rows.

static const TestDataRow TEST_DATA[] = {
    //  input text                         expected                 offs sflags
    //  ---------------------------------- -----------------------  ---- ------
    ROW("",                                NtA,                       0, ""  ),

    ROW("+",                               NtA,                       0, ""  ),
    ROW("-",                               NtA,                       0, ""  ),
    ROW(".",                               NtA,                       0, ""  ),
    ROW("a",                               NtA,                       0, ""  ),
    ROW("0",                               0,                       All, ""  ),
    ROW("1",                               1,                       All, ""  ),
    ROW("9",                               9,                       All, ""  ),
    ROW("e",                               NtA,                       0, ""  ),
    ROW("E",                               NtA,                       0, ""  ),

    // We treat 'x' and 'X' at the end differently due to MS and AIX issues
    ROW("x",                               NtA,                       0, ""  ),
    ROW("X",                               NtA,                       0, ""  ),

    // Partial hex prefix
    ROW("0x",                              0,                        1,  ""  ),
    ROW("0X",                              0,                        1,  ""  ),

    // Non-hex-looking two character variations
    ROW("++",                              NtA,                       0, ""  ),
    ROW("+-",                              NtA,                       0, ""  ),
    ROW("-+",                              NtA,                       0, ""  ),
    ROW("--",                              NtA,                       0, ""  ),
    ROW("10",                              10,                      All, ""  ),
    ROW("90",                              90,                      All, ""  ),
    ROW(".0",                              0,                       All, ""  ),
    ROW(".1",                              0x1.999999999999ap-4,    All, ""  ),
    ROW(".9",                              0x1.ccccccccccccdp-1,    All, ""  ),
    ROW("0e",                              0,                         1, ""  ),
    ROW("0E",                              0,                         1, ""  ),

    // Too many signs
    ROW("++1",                             NtA,                       0, ""  ),
    ROW("--1",                             NtA,                       0, ""  ),
    ROW("+-1",                             NtA,                       0, ""  ),
    ROW("-+1",                             NtA,                       0, ""  ),

    // Hex-looking 3 char, but not really valid hexfloat
    ROW("0xx",                             0,                         1, ""  ),
    ROW("0XX",                             0,                         1, ""  ),
    ROW("0x.",                             0,                         1, ""  ),
    ROW("0X.",                             0,                         1, ""  ),

    // Valid 3 char hexfloats
    ROW("0x0",                              0,                      All, "H" ),
    ROW("0x1",                              1,                      All, "H" ),
    ROW("0x2",                              2,                      All, "H" ),
    ROW("0x3",                              3,                      All, "H" ),
    ROW("0x4",                              4,                      All, "H" ),
    ROW("0x5",                              5,                      All, "H" ),
    ROW("0x6",                              6,                      All, "H" ),
    ROW("0x7",                              7,                      All, "H" ),
    ROW("0x8",                              8,                      All, "H" ),
    ROW("0x9",                              9,                      All, "H" ),
    ROW("0xa",                             10,                      All, "H" ),
    ROW("0xb",                             11,                      All, "H" ),
    ROW("0xc",                             12,                      All, "H" ),
    ROW("0xd",                             13,                      All, "H" ),
    ROW("0xe",                             14,                      All, "H" ),
    ROW("0xf",                             15,                      All, "H" ),
    ROW("0x10",                            16,                      All, "H" ),
    ROW("0x11",                            17,                      All, "H" ),
    ROW("0x12",                            18,                      All, "H" ),
    ROW("0x13",                            19,                      All, "H" ),
    ROW("0x14",                            20,                      All, "H" ),
    ROW("0x15",                            21,                      All, "H" ),
    ROW("0x16",                            22,                      All, "H" ),
    ROW("0x17",                            23,                      All, "H" ),
    ROW("0x18",                            24,                      All, "H" ),
    ROW("0x19",                            25,                      All, "H" ),
    ROW("0x1a",                            26,                      All, "H" ),
    ROW("0x1b",                            27,                      All, "H" ),
    ROW("0x1c",                            28,                      All, "H" ),
    ROW("0x1d",                            29,                      All, "H" ),
    ROW("0x1e",                            30,                      All, "H" ),
    ROW("0x1f",                            31,                      All, "H" ),

    // '[eE]+$' had 'scanf' anomalies, we treat those as possible anomalies
    ROW("0ee",                               0,                       1, ""  ),
    ROW("0eE",                               0,                       1, ""  ),
    ROW("0Ee",                               0,                       1, ""  ),
    ROW("0EE",                               0,                       1, ""  ),
    ROW("0e0",                               0,                     All, ""  ),
    ROW("0e1",                               0,                     All, ""  ),
    ROW("1e0",                               1,                     All, ""  ),
    ROW("1e1",                              10,                     All, ""  ),
    ROW("1e2",                             100,                     All, ""  ),
    ROW("0E0",                               0,                     All, ""  ),
    ROW("0E1",                               0,                     All, ""  ),
    ROW("1E0",                               1,                     All, ""  ),
    ROW("1E1",                              10,                     All, ""  ),
    ROW("1E2",                             100,                     All, ""  ),

    // Too many signs mixed with whitespace
    ROW(" ++1",                            NtA,                       0, ""  ),
    ROW("+ +1",                            NtA,                       0, ""  ),
    ROW("++\t1",                           NtA,                       0, ""  ),

    ROW(" + +t1",                          NtA,                       0, ""  ),
    ROW(" ++ t1",                          NtA,                       0, ""  ),
    ROW("+ + t1",                          NtA,                       0, ""  ),
    ROW(" + + t1",                         NtA,                       0, ""  ),

    ROW("\t--1",                           NtA,                       0, ""  ),
    ROW("- -1",                            NtA,                       0, ""  ),
    ROW("-- 1",                            NtA,                       0, ""  ),
    ROW(" - -1",                           NtA,                       0, ""  ),
    ROW(" -- 1",                           NtA,                       0, ""  ),
    ROW("- - 1",                           NtA,                       0, ""  ),
    ROW(" - - 1",                          NtA,                       0, ""  ),

    ROW(" +-1",                            NtA,                       0, ""  ),
    ROW("+\t-1",                           NtA,                       0, ""  ),
    ROW("+- 1",                            NtA,                       0, ""  ),
    ROW(" + -1",                           NtA,                       0, ""  ),
    ROW(" +- 1",                           NtA,                       0, ""  ),
    ROW("+ - 1",                           NtA,                       0, ""  ),
    ROW(" + - 1",                          NtA,                       0, ""  ),

    ROW(" -+1",                            NtA,                       0, ""  ),
    ROW("- +1",                            NtA,                       0, ""  ),
    ROW("-+ 1",                            NtA,                       0, ""  ),
    ROW(" - +1",                           NtA,                       0, ""  ),
    ROW(" -+ 1",                           NtA,                       0, ""  ),
    ROW("- + 1",                           NtA,                       0, ""  ),
    ROW(" - + 1",                          NtA,                       0, ""  ),

    // Valid 4 char hexfloats
    ROW("0x42",                             66,                     All, "H" ),
    ROW("0X0f",                             15,                     All, "H" ),
    ROW("0Xa5",                            165,                     All, "H" ),

    ROW("0x.0",                            0,                       All, "H" ),
    ROW("0x.1",                            0x1.0000000000000p-4,    All, "H" ),
    ROW("0x.2",                            0x1.0000000000000p-3,    All, "H" ),
    ROW("0x.3",                            0x1.8000000000000p-3,    All, "H" ),
    ROW("0x.4",                            0x1.0000000000000p-2,    All, "H" ),
    ROW("0x.5",                            0x1.4000000000000p-2,    All, "H" ),
    ROW("0x.6",                            0x1.8000000000000p-2,    All, "H" ),
    ROW("0x.7",                            0x1.c000000000000p-2,    All, "H" ),
    ROW("0x.8",                            0x1.0000000000000p-1,    All, "H" ),
    ROW("0x.9",                            0x1.2000000000000p-1,    All, "H" ),
    ROW("0x.a",                            0x1.4000000000000p-1,    All, "H" ),
    ROW("0x.b",                            0x1.6000000000000p-1,    All, "H" ),
    ROW("0x.c",                            0x1.8000000000000p-1,    All, "H" ),
    ROW("0x.d",                            0x1.a000000000000p-1,    All, "H" ),
    ROW("0x.e",                            0x1.c000000000000p-1,    All, "H" ),
    ROW("0x.f",                            0x1.e000000000000p-1,    All, "H" ),

    // Too many signs for hexfloat
    ROW("++0Xa",                           NtA,                       0, ""  ),
    ROW("--0xa",                           NtA,                       0, ""  ),
    ROW("+-0Xa",                           NtA,                       0, ""  ),
    ROW("-+0xa",                           NtA,                       0, ""  ),

    // Eclectic set of string that look hex or scientific but aren't, yet a
    // part of them can be parsed as a 'double'
    ROW("1.23E2E",                         123,                      -1, ""  ),
    ROW("123x",                            123,                      -1, ""  ),
    ROW("1x",                                1,                      -1, ""  ),
    ROW("123X",                            123,                      -1, ""  ),
    ROW("1X",                                1,                      -1, ""  ),

    ROW("486xe",                           486,                      -2, ""  ),
    ROW("914Xe",                           914,                      -2, ""  ),
    ROW("597xE",                           597,                      -2, ""  ),
    ROW("302XE",                           302,                      -2, ""  ),

    ROW("486ex",                           486,                      -2, ""  ),
    ROW("914eX",                           914,                      -2, ""  ),
    ROW("597Ex",                           597,                      -2, ""  ),
    ROW("302EX",                           302,                      -2, ""  ),

    // Variations on fixed numbers
    ROW(  ".25",                           0x1.0000000000000p-2,    All, ""  ),
    ROW( "0.25",                           0x1.0000000000000p-2,    All, ""  ),
    ROW("00.25",                           0x1.0000000000000p-2,    All, ""  ),
    ROW( "0.125",                          0x1.0000000000000p-3,    All, ""  ),
    ROW( "0.0625",                         0x1.0000000000000p-4,    All, ""  ),
    ROW( "0.3125",                         0x1.4000000000000p-2,    All, ""  ),

    // Decimal powers of ten, all expected to pass (pos: All, sflags: "")
    //
    //   Input                                    Expected
    //   ---------------------------------------- -----------------------
    PROW("1",                                                     1      ),
    PROW("10",                                                   10      ),
    PROW("100",                                                 100      ),
    PROW("1000",                                               1000      ),
    PROW("10000",                                             10000      ),
    PROW("100000",                                           100000      ),
    PROW("1000000",                                         1000000      ),
    PROW("10000000",                                       10000000      ),
    PROW("100000000",                                     100000000      ),
    PROW("1000000000",                                   1000000000      ),
    PROW("10000000000",                                 10000000000      ),
    PROW("100000000000",                               100000000000      ),
    PROW("1000000000000",                             1000000000000      ),
    PROW("10000000000000",                           10000000000000      ),
    PROW("100000000000000",                         100000000000000      ),
    PROW("1000000000000000",                       1000000000000000      ),
    PROW("10000000000000000",                     0x1.1c37937e08000p+53  ),
    PROW("100000000000000000",                    0x1.6345785d8a000p+56  ),
    PROW("1000000000000000000",                   0x1.bc16d674ec800p+59  ),
    PROW("10000000000000000000",                  0x1.158e460913d00p+63  ),
    PROW("100000000000000000000",                 0x1.5af1d78b58c40p+66  ),
    PROW("1000000000000000000000",                0x1.b1ae4d6e2ef50p+69  ),
    PROW("10000000000000000000000",               0x1.0f0cf064dd592p+73  ),
    PROW("100000000000000000000000",              0x1.52d02c7e14af6p+76  ),
    PROW("1000000000000000000000000",             0x1.a784379d99db4p+79  ),
    PROW("10000000000000000000000000",            0x1.08b2a2c280291p+83  ),
    PROW("100000000000000000000000000",           0x1.4adf4b7320335p+86  ),
    PROW("1000000000000000000000000000",          0x1.9d971e4fe8402p+89  ),
    PROW("10000000000000000000000000000",         0x1.027e72f1f1281p+93  ),
    PROW("100000000000000000000000000000",        0x1.431e0fae6d721p+96  ),
    PROW("1000000000000000000000000000000",       0x1.93e5939a08ceap+99  ),
    PROW("10000000000000000000000000000000",      0x1.f8def8808b024p+102 ),
    PROW("100000000000000000000000000000000",     0x1.3b8b5b5056e17p+106 ),
    PROW("1000000000000000000000000000000000",    0x1.8a6e32246c99cp+109 ),
    PROW("10000000000000000000000000000000000",   0x1.ed09bead87c03p+112 ),
    PROW("100000000000000000000000000000000000",  0x1.3426172c74d82p+116 ),
    PROW("1000000000000000000000000000000000000", 0x1.812f9cf7920e3p+119 ),

                // "Exact" tests in scientific format

    // Due to the mapping of decimal scientific values to binary IEEE values
    // the "exact" values do not represent an exact decimal boundary condition.
    // Of course we have an infinite number of possible decimal values between
    // any given 2 values if we allow any number of decimal digits.  But even
    // if we limit the number of decimal digits there are decimal digits
    // sequences that still map to the maximum/minimum value.  So just
    // increasing (maximum) or decreasing (minimum) the last decimal digit of
    // the mantissa will *not* cross into the overflow/underflow condition.  So
    // do not be surprised by the numbers below.

    // "Exact" scientific maximum 'double', and the first that overflows
    ROW("1.7976931348623157e308",          0x1.fffffffffffffp+1023, All, ""  ),
    ROW("1.7976931348623157e+308",         0x1.fffffffffffffp+1023, All, ""  ),
    ROW("1.7976931348623160e308",          Inf,                     All, "O" ),
    ROW("1.7976931348623160e+308",         Inf,                     All, "O" ),

    // Minimum normal, max/min subnormal, first underflow 'double'
    ROW("2.2250738585072014e-308",         0x1.0000000000000p-1022, All, ""  ),
    ROW("2.2250738585072009e-308",         0x0.fffffffffffffp-1022, All, "D" ),
    ROW("4.9406564584124654e-324",         0x0.0000000000001p-1022, All, "d" ),
    ROW("2.4703282292062327e-324",         0,                       All, "u" ),
    ROW("2.4703282292062327e-324",         0,                       All, "u" ),
    ROW("1.2351641146031166e-324",         0,                       All, "u" ),
    ROW("6.1758205730155818e-325",         0,                       All, "U" ),

        // Brute-force (not exactly edge case) scientific under/overflow
    ROW("1e-325",                          0,                       All, "U" ),
    ROW("1e309",                           Inf,                     All, "O" ),
    ROW("1e+309",                          Inf,                     All, "O" ),

    // Hexfloat under/overflow
    ROW("0x1p-1074",                       0x0.0000000000001p-1022, All, "Hd"),
    ROW("0x2p-1075",                       0x0.0000000000001p-1022, All, "Hd"),
    ROW("0x2p-1076",                       0,                       All, "hd"),
    ROW("0x1p-1075",                       0,                       All, "hd"),

    // 'strtod' of GNU libc  consistently parses any hexfloat value that is
    // half of 'bsl::numeric_limits<double>::denorm_min()' into the value of
    // 'bsl::numeric_limits<double>::denorm_min()'.  This off-by-one-helf bug
    // does not seem to appear for any other value or format.  The anomaly is
    // fixed/patched up in the 'parseDouble' implementation.  These test values
    // are the reasonable verification of the fix on this GNU libc bug.  A more
    // exhaustive verification step is performed separately with much longer
    // strings to verify pathological scenarios.  These tests are enabled for
    // all platforms deliberately as we use the non-buggy platforms as oracles.
    // See also 'u_GLIBC2_STRTOD_HEX_HALF_DENORM_MIN_HEX_BUG'.
    ROW( "0x1p-1075",                      0,                       All, "hd"),
    ROW( "0x2p-1076",                      0,                       All, "hd"),
    ROW( "0x4p-1077",                      0,                       All, "hd"),
    ROW( "0x8p-1078",                      0,                       All, "hd"),
    ROW("0x10p-1079",                      0,                       All, "hd"),
    ROW("0x20p-1080",                      0,                       All, "hd"),
    ROW("0x40p-1081",                      0,                       All, "hd"),
    ROW("0x80p-1082",                      0,                       All, "hd"),
    ROW("0x100p-1083",                     0,                       All, "hd"),
    ROW("0x200p-1084",                     0,                       All, "hd"),
    ROW("0x400p-1085",                     0,                       All, "hd"),
    ROW("0x800p-1086",                     0,                       All, "hd"),
    ROW("0x1000p-1087",                    0,                       All, "hd"),
    ROW("0x2000p-1088",                    0,                       All, "hd"),
    ROW("0x4000p-1089",                    0,                       All, "hd"),
    ROW("0x8000p-1090",                    0,                       All, "hd"),
    ROW("0x10000p-1091",                   0,                       All, "hd"),
    ROW("0x20000p-1092",                   0,                       All, "hd"),
    ROW("0x40000p-1093",                   0,                       All, "hd"),

    ROW("0x0.8p-1074",                     0,                       All, "hd"),
    ROW("0x0.4p-1073",                     0,                       All, "hd"),
    ROW("0x0.2p-1072",                     0,                       All, "hd"),
    ROW("0x0.1p-1071",                     0,                       All, "hd"),
    ROW("0x0.08p-1070",                    0,                       All, "hd"),
    ROW("0x0.04p-1069",                    0,                       All, "hd"),
    ROW("0x0.02p-1068",                    0,                       All, "hd"),
    ROW("0x0.01p-1067",                    0,                       All, "hd"),
    ROW("0x0.008p-1066",                   0,                       All, "hd"),
    ROW("0x0.004p-1065",                   0,                       All, "hd"),
    ROW("0x0.002p-1064",                   0,                       All, "hd"),
    ROW("0x0.001p-1063",                   0,                       All, "hd"),
    ROW("0x0.0008p-1062",                  0,                       All, "hd"),
    ROW("0x0.0004p-1061",                  0,                       All, "hd"),
    ROW("0x0.0002p-1060",                  0,                       All, "hd"),
    ROW("0x0.0001p-1059",                  0,                       All, "hd"),
    ROW("0x0.00008p-1058",                 0,                       All, "hd"),
    ROW("0x0.00004p-1057",                 0,                       All, "hd"),
    ROW("0x0.00002p-1056",                 0,                       All, "hd"),
    ROW("0x0.00001p-1055",                 0,                       All, "hd"),
    ROW("0x0.000001p-1051",                0,                       All, "hd"),

    ROW("0x.8p-1074",                      0,                       All, "hd"),
    ROW("0x.4p-1073",                      0,                       All, "hd"),
    ROW("0x.2p-1072",                      0,                       All, "hd"),
    ROW("0x.1p-1071",                      0,                       All, "hd"),
    ROW("0x.08p-1070",                     0,                       All, "hd"),
    ROW("0x.04p-1069",                     0,                       All, "hd"),
    ROW("0x.02p-1068",                     0,                       All, "hd"),
    ROW("0x.01p-1067",                     0,                       All, "hd"),
    ROW("0x.008p-1066",                    0,                       All, "hd"),
    ROW("0x.004p-1065",                    0,                       All, "hd"),
    ROW("0x.002p-1064",                    0,                       All, "hd"),
    ROW("0x.001p-1063",                    0,                       All, "hd"),
    ROW("0x.0008p-1062",                   0,                       All, "hd"),
    ROW("0x.0004p-1061",                   0,                       All, "hd"),
    ROW("0x.0002p-1060",                   0,                       All, "hd"),
    ROW("0x.0001p-1059",                   0,                       All, "hd"),
    ROW("0x.00008p-1058",                  0,                       All, "hd"),
    ROW("0x.00004p-1057",                  0,                       All, "hd"),
    ROW("0x.00002p-1056",                  0,                       All, "hd"),
    ROW("0x.00001p-1055",                  0,                       All, "hd"),
    ROW("0x.000001p-1051",                 0,                       All, "hd"),

    // 'u_GLIBC2_STRTOD_HEX_HALF_DENORM_MIN_HEX_BUG' unlikely but valid inputs

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "00000000000000000000000000002", // 268 '0's then '2'
                                           0,                       All, "hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "00000000000000000000000000004", // 268 '0's then '4'
                                           0x0.0000000000001p-1022, All, "Hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "00000000000000000000000000002P0", // 268 '0's then '2'
                                           0,                       All, "hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "00000000000000000000000000004P0", // 268 '0's then '4'
                                           0x0.0000000000001p-1022, All, "Hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "00000000000000000000000000001P1",
                                           0,                       All, "hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "00000000000000000000000000002P1",
                                           0x0.0000000000001p-1022, All, "Hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "000000000000000000000000000008P2",
                                           0,                       All, "hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "00000000000000000000000000001P2",
                                           0x0.0000000000001p-1022, All, "Hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "000000000000000000000000000004P3",
                                           0,                       All, "hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "000000000000000000000000000008P3",
                                           0x0.0000000000001p-1022, All, "Hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "000000000000000000000000000002P4",
                                           0,                       All, "hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "000000000000000000000000000004P4",
                                           0x0.0000000000001p-1022, All, "Hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "0000000000000000000000000000002P8",
                                           0,                       All, "hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "0000000000000000000000000000004P8",
                                           0x0.0000000000001p-1022, All, "Hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "00000000000000000000000000004P-1",
                                           0,                       All, "hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "00000000000000000000000000008P-1",
                                           0x0.0000000000001p-1022, All, "Hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "00000000000000000000000000008P-2",
                                           0,                       All, "hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "0000000000000000000000000001P-2",
                                           0x0.0000000000001p-1022, All, "Hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "0000000000000000000000000001P-3",
                                           0,                       All, "hd"),

    ROW("0x.000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "0000000000000000000000000002P-3",
                                           0x0.0000000000001p-1022, All, "Hd"),

    ROW("0x0"
          ".000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "00000000000000000000000000002", // 268 '0's then '2'
                                           0,                       All, "hd"),

    ROW("0x0"
          ".000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "00000000000000000000000000004", // 268 '0's then '4'
                                           0x0.0000000000001p-1022, All, "Hd"),

    ROW("0x00000"
          ".000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "00000000000000000000000000002", // 268 '0's then '2'
                                           0,                       All, "hd"),

    ROW("0x00000000"
          ".000000000000000000000000000000000000000000000000000000000000" // 60
           "000000000000000000000000000000000000000000000000000000000000" //120
           "000000000000000000000000000000000000000000000000000000000000" //180
           "000000000000000000000000000000000000000000000000000000000000" //240
           "00000000000000000000000000004", // 268 '0's then '4'
                                           0x0.0000000000001p-1022, All, "Hd"),

    ROW("0x2p-1077",                       0,                       All, "Hu"),
    ROW("0x1p-1076",                       0,                       All, "Hu"),
    ROW("0x1p-1077",                       0,                       All, "Hu"),
    ROW("0x1p-1078",                       0,                       All, "Hu"),
    ROW("0x1p-1079",                       0,                       All, "HU"),
    ROW("0x1p-1080",                       0,                       All, "HU"),
    ROW("0x0.8p1024",                      0x1.0000000000000p+1023, All, "H" ),
    ROW("0x1p1024",                        Inf,                     All, "HO"),
    ROW("0x1p+1024",                       Inf,                     All, "HO"),

    // Hexfloat exact maximum, then first overflow NA,
    ROW("0x1.FFFFFFFFFFFFFp1023",          0x1.fffffffffffffp+1023, All, "H" ),
    ROW("0x1.FFFFFFFFFFFFFp+1023",         0x1.fffffffffffffp+1023, All, "H" ),

    ROW("1e1000000000",                    Inf,                     All, "O" ),
    ROW("1e+1000000000",                   Inf,                     All, "O" ),
    ROW("1e2147483647",                    Inf,                     All, "O" ),
    ROW("1e+2147483647",                   Inf,                     All, "O" ),
    ROW("1e-2147483647",                     0,                     All, "U" ),
    ROW("1e-2147483648",                     0,                     All, "U" ),

    ROW("12345678000000000000",            0x1.56a951762ab5cp+63,   All, ""  ),

    // Boundary-value cases for 64-bit double.
    ROW("1e308",                           0x1.1ccf385ebc8a0p+1023, All, ""  ),
    ROW("1e+308",                          0x1.1ccf385ebc8a0p+1023, All, ""  ),
    ROW("1.7e308",                         0x1.e42d130773b76p+1023, All, ""  ),
    ROW("1.7e+308",                        0x1.e42d130773b76p+1023, All, ""  ),
    ROW("1.8e308",                         Inf,                     All, "O" ),
    ROW("1.8e+308",                        Inf,                     All, "O" ),

    // Stress tests for converting decimal to 64-bit double.  See
    //: "A Program for Testing IEEE Decimal-Binary Conversions"
    //: Vern Paxson, ICIR 1991.
    //
    //   Input                                    Expected
    //   ---------------------------------------- -----------------------
    PROW("5e125",                                 0x1.7a2ecc414a03fp+417 ),
    PROW("5e+125",                                0x1.7a2ecc414a03fp+417 ),
    PROW("6.9e268",                               0x1.0b7cb60c994dap+893 ),
    PROW("6.9e+268",                              0x1.0b7cb60c994dap+893 ),
    PROW("9.99e-24",                              0x1.82782afe1869ep-77  ),
    PROW("7.861e-31",                             0x1.fe3544145e9d8p-101 ),
    PROW("7.5569e-250",                           0x1.5a462d91c6ab3p-828 ),
    PROW("9.28609e-256",                          0x1.be2dd66200befp-848 ),
    PROW("9.210917e86",                           0x1.da232347e6032p+288 ),
    PROW("9.210917e+86",                          0x1.da232347e6032p+288 ),
    PROW("8.4863171e121",                         0x1.06e98f5ec8f37p+405 ),
    PROW("8.4863171e+121",                        0x1.06e98f5ec8f37p+405 ),
    PROW("6.53777767e281",                        0x1.20223f2b3a881p+936 ),
    PROW("6.53777767e+281",                       0x1.20223f2b3a881p+936 ),
    PROW("5.232604057e-289",                      0x1.465b896c24520p-958 ),
    PROW("2.7235667517e-99",                      0x1.7d41824d64fb2p-328 ),
    PROW("6.53532977297e-112",                    0x1.925a0aabcdc68p-370 ),
    PROW("3.142213164987e-282",                   0x1.d3409dfbca26fp-936 ),
    PROW("4.6202199371337e-59",                   0x1.28f9edfbd341fp-194 ),
    PROW("2.31010996856685e-59",                  0x1.28f9edfbd341fp-195 ),
    PROW("9.324754620109615e227",                 0x1.3ae60753af6cap+757 ),
    PROW("9.324754620109615e+227",                0x1.3ae60753af6cap+757 ),
    PROW("7.8459735791271921e65",                 0x1.dcd0089c1314ep+218 ),
    PROW("7.8459735791271921e+65",                0x1.dcd0089c1314ep+218 ),
    PROW("2.72104041512242479e217",               0x1.3bbb4bf05f087p+722 ),
    PROW("2.72104041512242479e+217",              0x1.3bbb4bf05f087p+722 ),
    PROW("6.802601037806061975e216",              0x1.3bbb4bf05f087p+720 ),
    PROW("6.802601037806061975e+216",             0x1.3bbb4bf05f087p+720 ),
    PROW("2.0505426358836677347e-202",            0x1.012954b6aabbap-670 ),
    PROW("8.36168422905420598437e-214",           0x1.20403a628a9cap-708 ),
    PROW("4.891559871276714924261e243",           0x1.6ecaf7694a3c7p+809 ),
    PROW("4.891559871276714924261e+243",          0x1.6ecaf7694a3c7p+809 ),

    // Test of the special values NaN and Infinity
    //
    //   Input                      Expected
    //   ------------------------   --------
    PROW("inf",                     Inf ),
    PROW("INF",                     Inf ),
    PROW("Inf",                     Inf ),
    PROW("InF",                     Inf ),

    PROW("infinity",                Inf ),
    PROW("INFINITY",                Inf ),
    PROW("Infinity",                Inf ),
    PROW("InFiNiTy",                Inf ),

    PROW("nan",                     NaN ),
    PROW("NAN",                     NaN ),
    PROW("Nan",                     NaN ),
    PROW("NaN",                     NaN ),

    PROW("nan()",                   NaN ),
    PROW("NAN()",                   NaN ),
    PROW("Nan()",                   NaN ),
    PROW("NaN()",                   NaN ),

    PROW("nan(ananana_batmaaan)",   NaN ),
    PROW("NAN(ananana_batmaaan)",   NaN ),
    PROW("Nan(ananana_batmaaan)",   NaN ),
    PROW("NaN(ananana_batmaaan)",   NaN ),

    // Tests for hexfloat parsing
    //
    // Hexfloats are verified more thoroughly as they are a relatively recent
    // addition to the languages and also one that may not be used as widely as
    // the scientific and fixed format.
    //
    // Some values are from https://observablehq.com/@jrus/hexfloat
    //..
    // number             hexfloat                 decimal
    //  1                 +0x1.0000000000000p+00   +1.0000000000000000e+00
    // +0                 +0x0.0000000000000p+00   +0.0000000000000000e+00
    // -0                 -0x0.0000000000000p+00   -0.0000000000000000e+00
    //  1/3               +0x1.5555555555555p-02   +3.3333333333333331e-01
    // -4/7               -0x1.2492492492492p-01   -5.7142857142857140e-01
    //  0.1               +0x1.999999999999ap-04   +1.0000000000000001e-01
    // (sqrt(5) + 1) / 2  +0x1.9e3779b97f4a8p+00   +1.6180339887498949e+00
    // sqrt(.5)           +0x1.6a09e667f3bcdp-01   +7.0710678118654757e-01
    // pi                 +0x1.921fb54442d18p+01   +3.1415926535897931e+00
    // e                  +0x1.5bf0a8b145769p+01   +2.7182818284590451e+00
    // ln(2)              +0x1.62e42fefa39efp-01   +6.9314718055994529e-01
    // log2(e)            +0x1.71547652b82fep+00   +1.4426950408889634e+00
    // DBL_EPSILON        +0x1.0000000000000p-52   +2.2204460492503131e-16
    // DBL_MIN            +0x1.0000000000000p-1074 +4.9406564584124654e-324
    // DBL_MAX            +0x1.fffffffffffffp+1023 +1.7976931348623157e+308
    // min safe int64     -0x1.fffffffffffffp+052  -9.0071992547409910e+15
    // max safe int64     +0x1.fffffffffffffp+052  +9.0071992547409910e+15
    //..
    //
    // More values (corner cases, subnormals, etc.) will have to be added only
    // if we decide to implement hexfloat support on Solaris.
    //
    //     Input                         Expected
    //     ---------------------------   ------------------------
    HPROW( "0x1.0000000000000p0",        1                       ),
    HPROW( "0x1.0000000000000p+0",       1                       ),
    HPROW( "0x1.0000000000000p00",       1                       ),
    HPROW( "0x1.0000000000000p+00",      1                       ),
    HPROW( "0x0.0000000000000p0",        0                       ),
    HPROW( "0x0.0000000000000p+0",       0                       ),
    HPROW( "0x0.0000000000000p00",       0                       ),
    HPROW( "0x0.0000000000000p+00",      0                       ),
    HPROW( "0x1.5555555555555p-2",       0x1.5555555555555p-2    ),
    HPROW( "0x1.5555555555555p-02",      0x1.5555555555555p-2    ),
    HPROW( "0x1.999999999999ap-4",       0x1.999999999999ap-4    ),
    HPROW( "0x1.999999999999ap-04",      0x1.999999999999ap-4    ),
    HPROW( "0x1.9e3779b97f4a8p0",        0x1.9e3779b97f4a8p+0    ),
    HPROW( "0x1.9e3779b97f4a8p00",       0x1.9e3779b97f4a8p+0    ),
    HPROW( "0x1.9e3779b97f4a8p+0",       0x1.9e3779b97f4a8p+0    ),
    HPROW( "0x1.9e3779b97f4a8p+00",      0x1.9e3779b97f4a8p+0    ),
    HPROW( "0x1.6a09e667f3bcdp-1",       0x1.6a09e667f3bcdp-1    ),
    HPROW( "0x1.6a09e667f3bcdp-01",      0x1.6a09e667f3bcdp-1    ),
    HPROW( "0x1.921fb54442d18p1",        0x1.921fb54442d18p+1    ),
    HPROW( "0x1.921fb54442d18p01",       0x1.921fb54442d18p+1    ),
    HPROW( "0x1.921fb54442d18p+1",       0x1.921fb54442d18p+1    ),
    HPROW( "0x1.921fb54442d18p+01",      0x1.921fb54442d18p+1    ),
    HPROW( "0x1.5bf0a8b145769p1",        0x1.5bf0a8b145769p+1    ),
    HPROW( "0x1.5bf0a8b145769p01",       0x1.5bf0a8b145769p+1    ),
    HPROW( "0x1.5bf0a8b145769p+1",       0x1.5bf0a8b145769p+1    ),
    HPROW( "0x1.5bf0a8b145769p+01",      0x1.5bf0a8b145769p+1    ),
    HPROW( "0x1.62e42fefa39efp-1",       0x1.62e42fefa39efp-1    ),
    HPROW( "0x1.62e42fefa39efp-01",      0x1.62e42fefa39efp-1    ),
    HPROW( "0x1.71547652b82fep0",        0x1.71547652b82fep+0    ),
    HPROW( "0x1.71547652b82fep00",       0x1.71547652b82fep+0    ),
    HPROW( "0x1.71547652b82fep+0",       0x1.71547652b82fep+0    ),
    HPROW( "0x1.71547652b82fep+00",      0x1.71547652b82fep+0    ),
    HPROW( "0x1.0000000000000p-52",      0x1.0000000000000p-52   ),
    HPROW( "0x1.0000000000000p-052",     0x1.0000000000000p-52   ),
    HPROW( "0x1.0000000000000p-0052",    0x1.0000000000000p-52   ),
    HDROW( "0x1.0000000000000p-1074",    0x0.0000000000001p-1022 ),
    HDROW( "0x1.0000000000000p-01074",   0x0.0000000000001p-1022 ),
    HPROW( "0x1.fffffffffffffp1023",     0x1.fffffffffffffp+1023 ),
    HPROW( "0x1.fffffffffffffp01023",    0x1.fffffffffffffp+1023 ),
    HPROW( "0x1.fffffffffffffp+1023",    0x1.fffffffffffffp+1023 ),
    HPROW( "0x1.fffffffffffffp+01023",   0x1.fffffffffffffp+1023 ),
    HPROW( "0x1.fffffffffffffp52",       9007199254740991        ),
    HPROW( "0x1.fffffffffffffp052",      9007199254740991        ),
    HPROW( "0x1.fffffffffffffp0052",     9007199254740991        ),
    HPROW( "0x1.fffffffffffffp+52",      9007199254740991        ),
    HPROW( "0x1.fffffffffffffp+052",     9007199254740991        ),
    HPROW( "0x1.fffffffffffffp+0052",    9007199254740991        ),
};
#undef HDROW
#undef HPROW
#undef PROW
#undef ROW

static bsl::size_t NUM_TEST_DATA = sizeof TEST_DATA / sizeof *TEST_DATA;

#ifdef u_PARSEDOUBLE_USES_STRTOD
namespace StrtodAssumptions {
void verifyForRow(const TestDataRow& ROW);
    // Print a report 'strtod' unexpected behavior about the input in the
    // specified test data 'ROW'.
}  // close namespace StrtodAssumptions
#endif

#ifdef u_PARSEDOUBLE_USES_FROM_CHARS
namespace FromCharsAssumptions {
void verify();
    // Verify assumptions about 'from_chars' behavior on range errors.
}  // close namespace FromCharsAssumptions
#endif

void verifyParseDouble(const TestDataRow& ROW, OpMode::Enum opMode)
    // Verify 'bdlb::NumericParseUtil::parseDouble' using the test data from
    // the specified 'ROW'.  The specified 'opMode' (orthogonal perturbation
    // mode) is used only in test assertions to denote what exact sub-test
    // failed (e.g., adding a redundant '+' sign, or adding trailing
    // whitespace, or a combination etc.).
{
    // Pull in long names
    using bsl::string_view;

    // Pull in TEST DATA
    const int          LINE     = ROW.d_line;
    const string_view  INPUT    = ROW.d_input;
    const double       EXPECTED = ROW.d_expected;
    const bsl::size_t  REST_POS = ROW.d_restPos;
    const SpecialFlags FLAGS    = ROW.d_flags;

    // Derived test data
    const bool         SUCCESS = (REST_POS != 0);
    const bool         IS_NAN  = SUCCESS && isNan(EXPECTED);
    const string_view  REST    = INPUT.substr(REST_POS);

    // Flags describing the input value
    const bool IS_HEX       = FLAGS.d_isHex;
    const bool IS_OVERFLOW  = FLAGS.d_isOverflow;
    //const bool SIGD_UFLOW   = FLAGS.d_significandUnderflow;
    const bool IS_UNDERFLOW = FLAGS.d_isUnderflow;
    const bool IS_SUBNORMAL = FLAGS.d_isSubnormal;
    //const bool SUBNORM_MIN  = FLAGS.d_subnormIsMin;

    if (veryVerbose) {
        P_(LINE) P_(INPUT)
        if (SUCCESS) {
            P_(FLAGS) P_(REST_POS)
        }
        P(SUCCESS);
    }

    // For platforms that do not support parsing hexfloat we modified the flags
    // while filling the row of the test table.
    const bool EXPECT_HEX_REVIEW       = (SUCCESS && IS_HEX      );
    const bool EXPECT_OVERFLOW_REVIEW  = (SUCCESS && IS_OVERFLOW );
    const bool EXPECT_UNDERFLOW_REVIEW = (SUCCESS && IS_UNDERFLOW);
    const bool MAYBE_UNDERFLOW_REVIEW  = (SUCCESS && IS_SUBNORMAL);

    const double INIT1 = 42.42e42 * (isNegative(EXPECTED) ? 1 : -1);
    const double INIT2 = 24.24e24 * (isNegative(EXPECTED) ? 1 : -1);

    const double INIT_VALUE = (INIT1 == EXPECTED) ? INIT2 : INIT1;

    // For under/overflow and hexfloat 'BSLS_REVIEW' tests.
    BslsReviewMonitor reviewMon(veryVerbose);

    double      result = INIT_VALUE;
    string_view rest;

    // ==========================================
    // Verification of the all-arguments function

    reviewMon.resetCounters();
    const int rc = Util::parseDouble(&result, &rest, INPUT);
    if (veryVerbose) {
        P(rc);
    }

#define ASSERTL(...) do {                                                     \
    if (!u_LASTARG(__VA_ARGS__)) {                                            \
         bsl::cout << "LINE: "   <<    LINE                     << ", "       \
                   << "INPUT: "  << Qs(INPUT)                   << ", "       \
                   << "EXPECT: " << (SUCCESS ? "pass" : "FAIL") << ", "       \
                   << "mode: "   << opMode                      << ".  "      \
                   << "rc: "     << rc                          << ", "       \
                   << "result: " << result << '\n';                           \
    }                                                                         \
    ASSERTV(__VA_ARGS__) } while (false)
    // Local 'ASSERT' that adds extra value printing we want to see in every
    // failure message to be able to quickly identify the issue reported.

    // Return value is as expected
    ASSERTL(SUCCESS == (0 == rc));

    // The unparsed 'rest' is at the expected input-string-offset
    ASSERTL(REST_POS,
            Qs(REST),
            ASPTR(REST.data()), ASPTR(rest.data()),
            REST.data() == rest.data());

    // The unparsed 'rest' ends at the end of the input
    ASSERTL(Qs(REST),
            REST.length(),   rest.length(),
            REST.length() == rest.length());

    if (SUCCESS) {
        if (IS_NAN) {
            ASSERTL(INIT_VALUE, result, isNan(result));

            const bool resultSign   = isNegative(result);
            const bool expectedSign = isNegative(EXPECTED);

            ASSERTL(resultSign, expectedSign, resultSign == expectedSign);
        }
        else { // Not NaN or expected failure
            ASSERTL(result, EXPECTED, result == EXPECTED);
        }
    }
    else {  // In case of failure
        ASSERTL(result, INIT_VALUE, result == INIT_VALUE);
    }

    // Verify expected 'BSLS_REVIEW' violation numbers
    ASSERTL(reviewMon.numHexfloat(),
            reviewMon.numHexfloat() == (EXPECT_HEX_REVIEW ? 1 : 0));
    ASSERTL(reviewMon.numOverflow(),
            reviewMon.numOverflow() == (EXPECT_OVERFLOW_REVIEW ? 1 : 0));
    if (EXPECT_UNDERFLOW_REVIEW) {
        ASSERTL(reviewMon.numUnderflow(), reviewMon.numUnderflow() == 1);
    }
    else if (MAYBE_UNDERFLOW_REVIEW) {
        ASSERTL(reviewMon.numUnderflow(), reviewMon.numUnderflow() <= 1);
    }
    else {
        ASSERTL(reviewMon.numUnderflow(), reviewMon.numUnderflow() == 0);
    }

    // ===========================================
    // Verification of the two-parameters function

    reviewMon.resetCounters();
    double    result2 = INIT_VALUE;
    const int rc2     = Util::parseDouble(&result2, INPUT);
    if (veryVerbose) {
        P(rc2);
    }

    // Should return the exact same value as the "full" function
    ASSERTL(rc, rc2, rc == rc2);

    if (IS_NAN && SUCCESS) {
        // This should also parse a NaN
        ASSERTV(LINE, result2, isNan(result2));

        // With the same sign
        const bool resultSign  = isNegative(result);
        const bool result2Sign = isNegative(result2);

        ASSERTL(resultSign,   result2Sign,
                resultSign == result2Sign);
    }
    else {
        // The parsed 'double' value should be the same in case of numbers *or*
        // failure as we use the same 'INIT_VALUE'
        ASSERTL(rc2, result == result2);
    }

    // Verify expected 'BSLS_REVIEW' violation numbers
    ASSERTL(reviewMon.numHexfloat(),
            reviewMon.numHexfloat() == (EXPECT_HEX_REVIEW ? 1 : 0));
    ASSERTL(reviewMon.numOverflow(),
            reviewMon.numOverflow() == (EXPECT_OVERFLOW_REVIEW ? 1 : 0));
    if (EXPECT_UNDERFLOW_REVIEW) {
        ASSERTL(reviewMon.numUnderflow(), reviewMon.numUnderflow() == 1);
    }
    else if (MAYBE_UNDERFLOW_REVIEW) {
        ASSERTL(reviewMon.numUnderflow(), reviewMon.numUnderflow() <= 1);
    }
    else {
        ASSERTL(reviewMon.numUnderflow(), reviewMon.numUnderflow() == 0);
    }

#undef ASSERTL
}

void orthogonallyPerturbateOn(const TestDataRow& ROW)
    // One by one prepare all relevant orthogonal perturbations on the
    // 'parseDouble' test described by the specified 'ROW' and delegate actual
    // verification of each to 'verifyParseDouble'.
{
    // The buffer used to prepare input during orthogonal perturbation
    char opBuff[1024];

    using bsl::string_view;
    using bsl::size_t;

    const int          LINE        = ROW.d_line;
    const string_view  INPUT       = ROW.d_input;
    const double       EXPECTED    = ROW.d_expected;
    const size_t       REST_POS    = ROW.d_restPos;
    const SpecialFlags FLAGS       = ROW.d_flags;
    const string_view  EXP_LITERAL = ROW.d_expLiteral;

    const bool         SUCCESS = (REST_POS != 0);
    const bool         IS_NAN  = (SUCCESS && EXPECTED != EXPECTED);

    //const bool         IS_HEX       = FLAGS.d_isHex;
    //const bool         IS_OVERFLOW  = FLAGS.d_isOverflow;
    //const bool         SIGD_UFLOW   = FLAGS.d_significandUnderflow;
    //const bool         IS_UNDERFLOW = FLAGS.d_isUnderflow;
    //const bool         IS_SUBNORMAL = FLAGS.d_isSubnormal;
    //const bool         SUBNORM_MIN  = FLAGS.d_subnormIsMin;

    if (veryVerbose) {
        P_(LINE) P_(INPUT)
        if (SUCCESS) {
            P_(FLAGS) P_(REST_POS)
        }
        P(SUCCESS);
    }

    const TestAssertCounter tsg; (void)tsg;

    static const int OPI_MAX_EXTRA_CHARS = 4;
        // Allows really *orthogonal* perturbation on 4 axis:
        //
        //: o leading whitespace
        //: o trailing whitespace
        //: o optional sign
        //: o optional exponent sign
        //
        // We do *not* yet do these 4 axis parallel, just ensure that 'opBuff'
        // is larger enough for it.

    // Ensure that the any orthogonal perturbation will fit in 'opBuff'
    ASSERTV(LINE,
            INPUT.length() + OPI_MAX_EXTRA_CHARS,
            INPUT.length() + OPI_MAX_EXTRA_CHARS <= sizeof opBuff);

    // orthogonal perturbation over whitespace and +/- sign
    for (int opi = 0; opi < OpMode::e_END; ++opi) {
        const OpMode::Enum opMode = static_cast<OpMode::Enum>(opi);

        if (veryVerbose) bsl::cout << "testing with " << opMode << '\n';

        // We use 'opBuff' not only to build up the input string variations
        // (based on the 'opi' Orthogonal Perturbation Iterator), but also to
        // make the input text not to be null terminated.  We do that by making
        // sure that 'opBuff' is always longer than our final input string, and
        // we pre-fill 'opBuff' (C-style) with a character that should cause an
        // error when read, and is not the 0 character.

        const size_t MAX_OPI_LENGTH = INPUT.length() + OPI_MAX_EXTRA_CHARS;
        const size_t OPI_LENGTH = (0 == opi) ? INPUT.length() : MAX_OPI_LENGTH;
        if (OPI_LENGTH > sizeof opBuff) {
            if (veryVerbose) bsl::cout << "Skipping due to too long INPUT\n";
            // To avoid possible buffer overrun
            continue;                                               // CONTINUE
        }

        // Pre-fill 'opBuff' with a character that would be invalid in any
        // parsed number.
        bsl::memset(&opBuff, '\xff', sizeof opBuff);

        int extraChars       = 0;  // These get into 'opBuff'
        int extraParsedChars = 0;  // These get into parsed characters

        switch (opMode) {
          case OpMode::e_NONE: {     // Unchanged input
            memcpy(opBuff, INPUT.data(), INPUT.length());
          } break;

          case OpMode::e_LEADING: {  // Leading whitespace added
            memcpy(opBuff + 1, INPUT.data(), INPUT.length());
            opBuff[0] = '\t';  // TBD: use all possible WS characters
            ++extraChars;
          } break;

          case OpMode::e_TRAILING: {
            memcpy(opBuff, INPUT.data(), INPUT.length());
            opBuff[INPUT.length()] = ' ';
            ++extraChars;
          } break;

          case OpMode::e_EXPLICIT_POSITIVE: {
            if (INPUT.empty() || '-' == INPUT[0] || '+' == INPUT[0]) {
                // Input already has a sign or too short
                continue;                                           // CONTINUE
            }
            memcpy(opBuff + 1, INPUT.data(), INPUT.length());
            opBuff[0] = '+';
            ++extraChars;
            ++extraParsedChars;
          } break;

          case OpMode::e_NEGATIVE_SIGN: {
            if (INPUT.empty() || '-' == INPUT[0] || '+' == INPUT[0]) {
                // Input already has a sign or too short
                continue;                                           // CONTINUE
            }
            memcpy(opBuff + 1, INPUT.data(), INPUT.length());
            opBuff[0] = '-';
            ++extraChars;
            ++extraParsedChars;
          } break;

          case OpMode::e_END: break;  // silence clang/gcc warning
        }

        // Test data and expectations that depend on the 'opMode'

        const size_t OP_LENGTH = INPUT.length() + extraChars;
            // The actual number of characters we need to parse

        const bsl::string_view OP_INPUT(opBuff, OP_LENGTH);
            // The input string view.

        const bool OP_SUCCESS = (SUCCESS && OpMode::e_LEADING != opMode);

        // Not 'size_t' because we compare it to the difference
        // 'rest.data() - OP_INPUT.data()' and compilers warn about
        // signed/unsigned mismatch
        const size_t OP_REST_POS =
                                 !OP_SUCCESS ? 0 : REST_POS + extraParsedChars;

        const double OP_EXPECTED = IS_NAN ? OpMode::e_NEGATIVE_SIGN == opMode
                                            ? -Limits::quiet_NaN()
                                            : Limits::quiet_NaN()
                                          : OpMode::e_NEGATIVE_SIGN == opMode
                                            ? -EXPECTED
                                            : EXPECTED;

        const TestDataRow OP_ROW = {
            LINE, OP_INPUT, OP_EXPECTED, OP_REST_POS, FLAGS, EXP_LITERAL
        };
        verifyParseDouble(OP_ROW, opMode);
    }  // close opMode loop
}

void testParseDouble()
    // Verify the 'parseDouble' function using orthogonal perturbation methods
    // over the 'TEST_DATA' table.
{
    TestAssertCounter testStatusGuard;
    for (bsl::size_t ti = 0; ti < NUM_TEST_DATA; ++ti) {
        orthogonallyPerturbateOn(TEST_DATA[ti]);

        // Check our assumptions in the implementation, did 'strtod' change?
#ifdef u_PARSEDOUBLE_USES_STRTOD
        if (testStatus > 0) {
#ifdef u_PARSEDOUBLE_USES_STRTOD_ON_RANGE_ERRORS_ONLY
            const SpecialFlags& f = TEST_DATA[ti].d_flags;
            if (f.d_isOverflow || f.d_isUnderflow)
#endif
            {
                StrtodAssumptions::verifyForRow(TEST_DATA[ti]);
                testStatusGuard.reset();
            }
        }
#endif
    }

#ifdef u_PARSEDOUBLE_USES_FROM_CHARS
    // Checking for our LWG 3081 assumption is fast so we always do it.
    FromCharsAssumptions::verify();
#endif
}

// ============================================================================
// IMPLEMENTATION ASSUMPTIONS VERIFICATION AND REPORTING

#ifdef u_PARSEDOUBLE_USES_STRTOD
namespace StrtodAssumptions {

                                // ============
                                // class Result
                                // ============
class Result {
    // This simply constrained attribute type-like class is an abstraction of
    // the easily O(1) observable results of an 'strtod' call.  It has dual
    // purpose.  It provides easy verification of assumptions such as POSIX
    // compliance, expected parsing results, as well as observed and assumed
    // anomalies.  But it also represent "what the parsing code can see".
    // Attaching observed anomalies to their observed 'Result' quickly shows if
    // there are bugs in 'strtod' (on a given platform) that we cannot possibly
    // fix without writing our 'double' parser parser.
    //
    // The main purpose of this type is to compute and store the (useful)
    // categories in which the observable effects of an 'strtod' call fall,
    // such as the category of an 'errno' value that was possibly set by
    // 'strtod', the category of the parsed number of characters reported via
    // the 'endPtr' output argument, and of course the category of the returned
    // 'double' value itself.  We also store the precise values to display them
    // in messages since they may be needed by humans analyzing unexpected
    // 'strtod' behavior.
    //
    // This class essentially reduces the large range of theoretically possible
    // 'double', 'endPtr', and 'errno' results into few categories for each,
    // greatly simplifying the definition of what is expected or not.
    //
    ///The Categories
    ///--------------
    // This 'strtod' result type has three "dimensions": the 'double' return
    // value, the 'errno' value right after the call, and the number of parsed
    // characters reported via the 'endPtr' output parameter.
    ///
    ///Categories of 'errno'
    ///- - - - - - - - - - -
    //..
    // e_OK     - zero 'errno' value
    // e_EINVAL - (22) redundantly reports failed parsing, "syntax error"
    // e_ERANGE - (34) result is over/underflow
    // e_EDOM   - (33) GNU libc anomaly for certain underflows
    // e_EOTHER - any other non-zero 'errno'
    //..
    // 'e_OK', 'e_EINVAL', and 'e_RANGE' are POSIX compliant.  'e_EDOM' is a
    // known 'errno' anomaly on Linux that indicates underflow.  'e_OTHER'
    // indicates an error value that POSIX does not allow, and we have not seen
    // any 'strtod' implementation set.
    //
    // Each 'errno' category limits the possible category the other values may,
    // meaningfully, take.  Those constraints however are not part of the
    // invariant of this type.  They only describe if the particular
    // combination is an expected/valid one.  This type should be able to store
    // whatever 'strtod' result we observe.  A separate assumption verification
    // algorithm decides if the result is something that the tested
    // 'parseDouble' implementation should be prepared to deal with, or if we
    // are seeing some new bug in an 'strtod' implementation.
    //
    ///Categorization of The Number of Parsed Characters
    ///- - - - - - - - - - - - - - - - - - - - - - - - -
    // We care about failure and success only.  No anomaly found in the exact
    // number of parsed characters reported on any platform: 'isFailure'.
    //
    ///Categorization of The Returned 'double' Value
    ///- - - - - - - - - - - - - - - - - - - - - - -
    // In order to be able to handle expected anomalies on every platform the
    // return value has the most categories:
    //..
    // e_ZERO      - indicates 'isFailure()', a zero parsed, or underflow
    // e_SUBNORMAL - should indicate that value parsed, or an underflow
    // e_NORMAL    - should indicate that value parsed
    // e_INF       - should indicate Infinity parsed or an overflow
    // a_NAN       - should indicate Not-a-Number parsed
    //..
    // A numeric value parsed is indicated by 'e_OK' and '!isFailure()', in
    // addition of the return value being the number.
    //
    // Underflow is indicated by 'ERANGE', '!isFailure()', and an absolute
    // return value not greater than 'std::numeric_limits<double>::min()'.  Due
    // to a GNU libc bug, on Linux 'errno' may also be 'EDOM' on occasion.
    //
    // Not-a-Number and Infinity values are represented in the input by
    // letters, not numbers.  See {Grammar Production Rules}.
    //
    // Overflow is indicated by 'e_ERANGE', '!isFailure()', and an absolute
    // return value of Infinity, or 'bsl::numeric_limits<double>::max()'.  The
    // 'max()' return value has not been observed with default (C, global)
    // rounding setting.
    //
    ///Special in this Type
    ///--------------------
    // This type looks much like an attribute type, but it does not aim to
    // implement all value-type abilities, only those needed for the tests.

  public:
    // PUBLIC TYPES
    enum ValueCat {
        e_ZERO      = 0,       // These values must be in increasing order
        e_SUBNORMAL = 1,
        e_NORMAL    = 2,
        e_INF       = INT_MAX,

        e_NAN       = INT_MIN  // A distinct value
    };

  private:
    // DATA
    double      d_value;
    bsl::size_t d_parsedChars;
    int         d_errno;

    ValueCat    d_valueCat;

  public:
    // CREATORS
    Result(const char *input,
           double      value,
           const char *endPtr,
           int         errorNumber);
        // Create an 'Result' object with the specified 'errorNumber' and
        // 'value'.  Compute 'parsedChars' from the specified 'input' and
        // 'endPtr'.  Compute the value category from the absolute of 'value'.

    // ACCESSORS                // Values
    int         errorNumber() const;
    bsl::size_t parsedChars() const;
    double      value()       const;
        // Return the attribute's value.

    bool isFailure() const;
        // Return '0 == parsedChars()'.

    bool isInf() const;
        // Return 'abs(value()) == bsl::numeric_limits<double>::infinity()'.

    bool isNan() const;
        // Return 'true' if the value is positive or negative NaN, otherwise
        // return 'false'.

    bool isOverflow() const;
        // Return 'true' if the values correspond to a POSIX overflow,
        // otherwise return 'false'.

    bool isUnderflow() const;
        // Return 'true' if the values correspond to a POSIX underflow, or a
        // "GNU underflow" where 'errno' is 'ERANGE', otherwise return 'false'.

                              // Value Category

    ValueCat valueCategory() const;
        // Return the computed category (enumeration).

    friend bool hasExpectedErrno(const Result& res)
    {
        switch (res.errorNumber()) {
          case      0: return true;                                   // RETURN

          case ERANGE: return res.valueCategory() < e_NORMAL
                           || res.valueCategory() == e_INF;           // RETURN

#if defined(__GLIBC__) && __GLIBC__ <= 2
          case EDOM:                                            // FALL-THROUGH
#endif
          case EINVAL: return res.valueCategory() == e_ZERO;          // RETURN
        }

        return false;
    }
};
                               // ------------
                               // class Result
                               // ------------
// CREATORS
inline
Result::Result(const char *input,
               double      value,
               const char *endPtr,
               int         errorNumber)
: d_value(value)
, d_parsedChars(static_cast<bsl::size_t>(endPtr - input))
, d_errno(errorNumber)
{
    BSLS_ASSERT_OPT(input );
    BSLS_ASSERT_OPT(endPtr);

    const double absValue = abs(value);

    d_valueCat =
          absValue == 0                  ? e_ZERO
        : absValue < Limits::min()       ? e_SUBNORMAL
        : absValue < Limits::infinity()  ? e_NORMAL
        : absValue == Limits::infinity() ? e_INF
        : e_NAN;
}

// ACCESSORS                // Values
inline
int Result::errorNumber() const         { return d_errno;                    }

inline
bsl::size_t Result::parsedChars() const { return d_parsedChars;              }

inline
double Result::value() const            { return d_value;                    }

inline
bool Result::isFailure() const          { return (0 == d_parsedChars);       }

inline
bool Result::isNan() const              { return testDouble::isNan(d_value); }

inline
bool Result::isInf() const              { return testDouble::isInf(d_value); }

inline
bool Result::isOverflow() const
{
    return d_errno == ERANGE && d_valueCat == e_INF;
}

inline
bool Result::isUnderflow() const
{
    return (d_errno == ERANGE && d_valueCat < e_NORMAL)
#if defined(__GLIBC__) && __GLIBC__ <= 2
        || (d_errno == EDOM   && d_valueCat == e_ZERO)
#endif
        ;
    // POSIX definition: 'errno' is 'ERANGE', value is less than the smallest
    // normal.  Linux anomaly: 'errno' can be 'EDOM' on significand-only
    // underflow, with the returned value being '0.0'.
}
                            // Categories
inline
Result::ValueCat Result::valueCategory() const
{
    return d_valueCat;
}

bsl::ostream& operator<<(bsl::ostream& os, Result::ValueCat vc)
{
#define u_CASE(label, text) case Result::label: return os << text
    switch (vc) {
      u_CASE(e_ZERO,      "zero     ");
      u_CASE(e_SUBNORMAL, "subnormal");
      u_CASE(e_NORMAL,    "normal   ");

      u_CASE(e_INF,       "infinity ");

      u_CASE(e_NAN,       "nan      ");
    }
#undef u_CASE

    return os << "***unknown-strtod-value-category**";
}

bsl::ostream& operator<<(bsl::ostream& os, Result sr)
{
    if (sr.isFailure()) {
        return os << "FAIL";                                          // RETURN
    }

    return os << "PASS, "
              << sr.valueCategory() << ", "
              << StrtodErrno(sr.errorNumber());
}
                           // ==================
                           // class ResultCatKey
                           // ==================
class ResultCatKey {
    // Represents a so-called category key of an 'Result' that can be used as a
    // key for lookup or sorting.  Note that the value used for lookup-key or
    // sorting is not the same than what is displayed for human consumption.
    // The class provides hashing and sorting support by implicitly converting
    // to an unspecified 'unsigned' value.  It provides stream output support
    // for human consumption.

  private:
    // DATA
    bool          d_failure;
    unsigned char d_valueCategory;
    unsigned char d_errnoCategory;

  private:
    // PRIVATE CREATORS
    ResultCatKey(bool isFailure, unsigned char valCat, unsigned char errCat);

  public:
    // CLASS METHODS
    static unsigned char categorize(int errorNumber);
        // Return an 'unsigned' with its lowest 3 bits filled according to the
        // specified 'errorNumber' ('errno').

    static unsigned char convertToBits(Result::ValueCat valueCategory);
        // Return an 'unsigned' with its lowest 3 bits filled according to the
        // specified 'valueCategory'.

    static Result::ValueCat convertToValueCategory(unsigned bits);
        // Return a value category filled according to the 3 bits of the
        // specified 'bits' value.  The behavior is undefined unless 'bits' is
        // a value obtained from 'convertToBits' called on a valid enumerator.

    static const char *errnoCategoryText(unsigned char errnoCategory);
        // Return a human readable description of the specified
        // 'errnoCategory'.

    static ResultCatKey fromBits(unsigned bits);
        // Convert the specified 'bits' into a 'ResultCatKey' and return that.
        // Opposite action of 'operator unsigned'.

  public:
    // CREATORS
    explicit ResultCatKey(const Result& strtodResult);
        // Create a category key object from the specified 'strtodResult' that
        // allows to look up or sort information based on the major
        // characteristics of the result.

    // ACCESSORS
    operator unsigned() const;
        // Return an unsigned value that is a key that may be used for sorting
        // or lookup based on the major categories.

    // ASPECTS
    friend
    bsl::ostream& operator<<(bsl::ostream& os, ResultCatKey k)
    {
        return os << (k.d_failure ? "FAIL: <" : "PASS: <")
                  << errnoCategoryText(k.d_errnoCategory) << "> "
                  << convertToValueCategory(k.d_valueCategory);
    }
};

                           // ------------------
                           // class ResultCatKey
                           // ------------------
// PRIVATE CREATORS
ResultCatKey::ResultCatKey(bool          isFailure,
                           unsigned char valCat,
                           unsigned char errCat)
: d_failure(isFailure)
, d_valueCategory(valCat)
, d_errnoCategory(errCat)
{}

// CLASS METHODS
inline
unsigned char ResultCatKey::categorize(int errorNumber)
{
    switch (errorNumber) {
      case      0:  return 0;                                         // RETURN
      case EINVAL:  return 1;                                         // RETURN
      case ERANGE:  return 2;                                         // RETURN
#if defined(__GLIBC__) && __GLIBC__ <= 2
      case   EDOM:  return 3;                                         // RETURN
#endif
    }

    // Unexpected 'errno' value (not in POSIX or seen in an anomaly)
    return 7;
}

inline
const char *ResultCatKey::errnoCategoryText(unsigned char errnoCategory)
{
    switch (errnoCategory) {
      case 0:  return "success";                                      // RETURN
      case 1:  return "EINVAL ";                                      // RETURN
      case 2:  return "ERANGE ";                                      // RETURN
#if defined(__GLIBC__) && __GLIBC__ <= 2
      case 3:  return "EDOM   ";                                      // RETURN
#endif
    }

    return "EOTHER?";
}

inline
unsigned char ResultCatKey::convertToBits(Result::ValueCat valueCategory)
{
    switch (valueCategory) {
      case Result::e_ZERO:      return 0;                             // RETURN
      case Result::e_SUBNORMAL: return 1;                             // RETURN
      case Result::e_NORMAL:    return 2;                             // RETURN
      case Result::e_INF:       return 3;                             // RETURN
      case Result::e_NAN:       return 4;                             // RETURN
    }

    // Not in 'default' to not silence 'enum' warnings
    ASSERT(!"unknown-value-category");
    return 0;
}

inline
Result::ValueCat ResultCatKey::convertToValueCategory(unsigned bits)
{
    switch (bits) {
      case 0: return Result::e_ZERO;                                  // RETURN
      case 1: return Result::e_SUBNORMAL;                             // RETURN
      case 2: return Result::e_NORMAL;                                // RETURN
      case 3: return Result::e_INF;                                   // RETURN
      case 4: return Result::e_NAN;                                   // RETURN
    }

    ASSERT(!"unknown-value-category-bits");
    return Result::e_ZERO;
}

ResultCatKey ResultCatKey::fromBits(unsigned bits)
{
    return ResultCatKey(!!(bits & 0x40), (bits & 0x38) >> 3, bits & 0x7);
}

// CREATORS
ResultCatKey::ResultCatKey(const Result& strtodResult)
{
    d_failure = strtodResult.isFailure();

    d_valueCategory = convertToBits(strtodResult.valueCategory());

    switch (strtodResult.errorNumber()) {
      case      0:  d_errnoCategory = 0;  break;
      case EINVAL:  d_errnoCategory = 1;  break;
      case ERANGE:  d_errnoCategory = 2;  break;
#if defined(__GLIBC__) && __GLIBC__ <= 2
      case   EDOM:  d_errnoCategory = 3;  break;
#endif

      default:      d_errnoCategory = 7;
    }
}

// ACCESSORS
ResultCatKey::operator unsigned() const
{
    return (d_failure << 6) | (d_valueCategory << 3) | d_errnoCategory;
}

                       // ==================
                       // class Expectations
                       // ==================

class Expectations {
    // Flags and the expected 'double' result of the test data row.  Used to
    // determine if the 'Result' is correct and may help a human reader to
    // determine what the anomaly is when 'Result' is not correct.

  private:
    // DATA
    double           d_expected;
    bsl::string_view d_expLiteral;
    bsl::size_t      d_parsedChars;
    bool             d_isInf;
    bool             d_isNan;
    SpecialFlags     d_flags;

  public:
    // CREATORS
    Expectations(const TestDataRow& row);                           // IMPLICIT

    // ACCESSORS       // Result Expectations
    double expected() const;
        // Return the value 'strod' is expected to return.  Meaningless in case
        // any of 'true == isFail()'.

    const bsl::string_view& expAsLiteral() const;
        // Return the value 'strod' is expected to return as the literal used
        // to define it in the test data table row.  Meaningless in case of
        // 'true == isFail()'.

    bool isFailure() const;
        // Return 'true' if 'strtod' was supposed to parse 0 characters,
        // otherwise return 'false'.

    bool isInUnderflowGap() const;
        // Return 'true' if the input was too small in absolute value to be
        // represented as a normal number in a 'double' before rounding or
        // after rounding to binary, otherwise return 'false'.  Notice that in
        // case of away-from-zero, towards-positive, or towards-negative
        // rounding applied the absolute return value of 'strtod' may still be
        // the 'bsl::numeric_limits<double>::min()' value, which is a normal
        // number, not a subnormal one.  See also 'isUnderflow()' here.

    bool isOverflow() const;
        // Return 'true' if the input was too large in absolute value to be
        // represented in a 'double' so 'strtod' was supposed to set 'errno' to
        // 'ERANGE' and return +/-Infinity.  (Technically +/-'HUGE_VAL', but
        // in case of IEEE 754 binary formats that is Infinity.), otherwise
        // return 'false'.

    bool isUnderflow() const;
        // Return 'true' if the input was too small in absolute value to be
        // represented in a 'double' so the resulting value after rounding is
        // zero, otherwise return 'false'.

    bsl::size_t parsedChars() const;
        // Return the number of characters 'strtod' was supposed to convert
        // into a 'double'.  Note that this value can be zero and that means
        // that the parsing has failed completely.

                        // Useful Input Categorization

    bool isSubnormal() const;
        // Return 'true' if the input was a subnormal number, a number that
        // can only be represented by less than 53 significant binary digits in
        // an IEEE 754 binary64, otherwise return 'false'.

    bool isSubnormMin() const;
        // Return 'true' if the absolute value of the input is the smallest
        // representable subnormal number (that is
        // 'bsl::numeric_limits<double>::denorm_min()') in any accepted format,
        // otherwise return 'false'.  Note that this flags is additional to
        // 'isSubnormal()', hence when this flag is 'true' so is
        // 'isSubnormal()'.  In other words this flag is always 'false' unless
        // 'true == isSubnormal()'.

    bool isHexHalfSubnormMin() const;
        // Return 'true' if the input is in hexfloat format, and its absolute
        // value is the half smallest representable subnormal number (that is
        // 'bsl::numeric_limits<double>::denorm_min() / 2'), otherwise, return
        // 'false'.  Note that this flags is additional to 'isSubnormal()',
        // hence when this flag is 'true' so is 'isSubnormal()'.  In other
        // words this flag is always 'false' unless 'true == isSubnormal()'.
        // See also 'u_GLIBC2_STRTOD_HEX_HALF_DENORM_MIN_HEX_BUG'.

    bool isInf() const;
        // Return 'true' if the input should convert to positive or negative
        // Infinity, otherwise return 'false'.

    bool isNan() const;
        // Return 'true' if the input should convert to positive or negative
        // Not-a-Number, otherwise return 'false'.

    bool isSignificandUnderflow() const;
        // Return 'true' if the absolute value of the input converts to a
        // number that, when represented in scientific format has its decimal
        // exponent value at the same as the smallest subnormal value (-324 for
        // 'double), but its binary significand is smaller than what can be
        // represented using the available 52 bits, otherwise return 'false'.

                             // Input Format Flag
    bool isHex() const;
        // Return 'true' if the input text was in hexadecimal format,
        // otherwise return 'false'.
};
                       // ------------------
                       // class Expectations
                       // ------------------
// CREATORS
inline
Expectations::Expectations(const TestDataRow& row)
: d_expected(row.d_expected)
, d_expLiteral(row.d_expLiteral)
, d_parsedChars(row.d_restPos)
, d_isInf(abs(row.d_expected) == Limits::infinity())
, d_isNan(row.d_expected != row.d_expected)
, d_flags(row.d_flags)
{
#if u_GLIBC2_STRTOD_HEX_HALF_DENORM_MIN_HEX_BUG
    if (row.d_flags.d_isHexHalfSubnormMin) {
        // We expect this bug.
        d_expected = bsl::numeric_limits<double>::denorm_min() *
                                     (isNegative(row.d_expected) ? -1.0 : 1.0);
    }
#endif  // u_GLIBC2_STRTOD_HEX_HALF_DENORM_MIN_HEX_BUG
}

// ACCESSORS       // Result Expectations
inline
double Expectations::expected() const
{
    return d_expected;
}

inline
const bsl::string_view& Expectations::expAsLiteral() const
{
    return d_expLiteral;
}

inline
bool Expectations::isFailure() const
{
    return 0 == d_parsedChars;
}

inline
bool Expectations::isInUnderflowGap() const
{
    return d_flags.d_isUnderflow || d_flags.d_isSubnormal;
}

inline
bool Expectations::isOverflow() const
{
    return d_flags.d_isOverflow;
}

inline
bool Expectations::isUnderflow() const
{
    return d_flags.d_isUnderflow;
}

inline
bsl::size_t Expectations::parsedChars() const
{
    return d_parsedChars;
}
              // Useful Input Categorization
inline
bool Expectations::isSubnormal() const
{
    return d_flags.d_isSubnormal;
}

inline
bool Expectations::isSubnormMin() const
{
    return d_flags.d_subnormIsMin;
}

inline
bool Expectations::isHexHalfSubnormMin() const
{
    return d_flags.d_isHexHalfSubnormMin;
}

inline
bool Expectations::isInf() const
{
    return d_isInf;
}

inline
bool Expectations::isNan() const
{
    return d_isNan;
}

inline
bool Expectations::isSignificandUnderflow() const
{
    return d_flags.d_significandUnderflow;
}
                  // Input Format Flag
inline
bool Expectations::isHex() const
{
    return d_flags.d_isHex;
}

                            // ================
                            // class Conclusion
                            // ================
struct Conclusion {
    // This enumerator describes what an 'Result' is when compared to its
    // 'Expectations'.  The following conclusions are possible:
    //..
    // e_OK:  'strtod' worked as expected (by POSIX)
    //
    // e_BOGUS_SUCCESS: Was supposed to fail but parsed something.
    // e_BOGUS_FAILURE: Was supposed to parse something but parsed nothing.
    //
    // e_WRONG_LENGTH: non-zero number of parsed characters do not match.
    //
    // e_WRONG_VALUE: returned the wrong value.
    //
    // e_BOGUS_OVERFLOW:  Overflow reported but not expected.
    // e_MISSED_OVERFLOW: Expected overflow not reported.
    //
    // e_BOGUS_UNDERFLOW:  Underflow reported but not expected.
    // e_MISSED_UNDERFLOW: Expected underflow not reported.
    //..

  public:
    // PUBIC TYPES
    enum Enum {
        e_NUMBER_OK        =  1,  // All as expected for a number parsed
        e_NAN_OK           =  2,  // All as expected for a Not-a-Number parsed
        e_INF_OK           =  3,  // All as expected for an Infinity parsed

        e_FAIL_OK          =  4,  // Failed as expected

        e_OVERFLOW_OK      =  5,  // All as expected for an overflowing input
        e_UNDERFLOW_OK     =  6,  // All as expected for an underflowing input
        e_UNDERFLOW_SUBN   =  7,  // Underflow on subnormal

        e_WEIRD_ERRNO      =  8,  // Not 0, ERANGE, or EINVAL

        e_BOGUS_SUCCESS    =  9,  // parsed more than 0 chars
        e_BOGUS_FAILURE    = 10,  // parsed 0 chars

        e_BOGUS_OVERFLOW   = 11,  // Unexpected overflow
        e_MISSED_OVERFLOW  = 12,  // Overflow not reported

        e_BOGUS_UNDERFLOW  = 13,  // Unexpected underflow
        e_MISSED_UNDERFLOW = 14,  // Underflow not reported

        e_WRONG_LENGTH     = 15,  // wrong 'double' returned (number/Inf/NaN)
        e_WRONG_VALUE      = 16   // wrong 'endPtr' returned
    };

    enum {
        // These enumerators we do not need to handle in 'switch' statements.

        k_LAST_GOOD = e_UNDERFLOW_SUBN,  // as-expected high-boundary
        k_FIRST_BAD = e_WEIRD_ERRNO,     // unexpected low-boundary

        k_BEGIN  = 1,              // for array access offset & loop start
        k_MAX    = e_WRONG_VALUE,  // for max value
        k_END    = k_MAX + 1,      // for loop end with less than

        k_SIZEOF = k_END - k_BEGIN + 1  // for array size
    };

    friend bool isAsExpected(Enum e)
        // Return 'true' if the specified 'e' enumerator value represents an
        // 'strtod' result that was the expected one, otherwise return 'false'.
    {
        // Unary + avoids the "comparing different enum types" warnings.
        return +e < +k_FIRST_BAD;
    }
};

bsl::ostream& operator<<(bsl::ostream& os, Conclusion::Enum sde)
{
#define u_CASE(label, text) case Conclusion::label: return os << text
    switch (sde) {
      u_CASE(e_NUMBER_OK,        "number-as-expected"    );

      u_CASE(e_NAN_OK,           "nan-as-expected"       );
      u_CASE(e_INF_OK,           "infinity-as-expected"  );

      u_CASE(e_FAIL_OK,          "failed-as-expected"    );

      u_CASE(e_OVERFLOW_OK,      "overflow-as-expected"  );
      u_CASE(e_UNDERFLOW_OK,     "underflow-as-expected" );

      u_CASE(e_WEIRD_ERRNO,      "unexpected-'errno'"    );

      u_CASE(e_UNDERFLOW_SUBN,   "underflow-on-subnormal");

      u_CASE(e_BOGUS_SUCCESS,    "unexpected-success"    );
      u_CASE(e_BOGUS_FAILURE,    "unexpected-failure"    );

      u_CASE(e_MISSED_OVERFLOW,  "missed-overflow"       );
      u_CASE(e_MISSED_UNDERFLOW, "missed-underflow"      );

      u_CASE(e_BOGUS_OVERFLOW,   "unexpected-overflow"   );
      u_CASE(e_BOGUS_UNDERFLOW,  "unexpected-underflow"  );

      u_CASE(e_WRONG_LENGTH,     "wrong-length"          );
      u_CASE(e_WRONG_VALUE,      "wrong-value"           );
    }
#undef u_CASE

    return os << "***unknown-strtod-conclusion**";
}

//..
// .------------.
// | May not be |       .-----------.
// | observed   |       | PASS/FAIL |                     .-------.
// |   +--------^-------^---.   .---^------+--------+-----+ Linux |
// |   | Conclusion         |   | VALUE    | errno  |  #  | extra |
// +---+--------------------+---+----------+--------+-----+-------+
// |   | e_NUMBER_OK        | P | 0        | 0      |   1 |       |
// |   |                    |   +----------+--------+-----+-------+
// |   |                    |   | Subnorm. | 0      |   2 |       |
// |   |                    |   +----------+--------+-----+-------+
// |   |                    |   | Normal   | 0      |   3 |       |
// +---+--------------------+---+----------+--------+-----+-------+
// |   | e_INF_OK           | P | Inf      | 0      |   4 |       |
// +---+--------------------+---+----------+--------+-----+-------+
// |   | e_NAN_OK           | P | NaN      | 0      |   5 |       |
// +---+--------------------+---+----------+--------+-----+-------+
// |   | e_FAIL_OK          | F | 0        | 0      |   6 |       |
// |   |                    |   |          +--------+-----+-------+
// | ? |                    |   |          | EINVAL |   7 |       |
// +---+--------------------+---+----------+--------+-----+-------+
// |   | e_OVERFLOW_OK      | P | Infinity | ERANGE |   8 |       |
// +---+--------------------+---+----------+--------+-----+-------+
// |   | e_UNDERFLOW_OK     | P | 0        | ERANGE |   9 |       |
// |   |                    |   |          +--------+-----+-------+
// | L |                    |   |          | EDOM   |  10 | L:  1 |
// |   |                    |   +----------+--------+-----+-------+
// | ? |                    |   | Subnorm. | ERANGE |  11 |     1 |
// +---+--------------------+---+----------+--------+-----+-------+
// | ? | e_WEIRD_ERRNO      | P | 0        | other  |  12 |     1 |
// |   |                    |   +----------+--------+-----+-------+
// | L |                    |   | Subnorm. | EDOM   |  13 |     2 |
// |   |                    |   |          +--------+-----+-------+
// | ? |                    |   |          | other  |  14 |     2 |
// |   |                    |   +----------+--------+-----+-------+
// | L |                    |   | Normal   | EDOM   |  15 |     3 |
// |   |                    |   |          +--------+-----+-------+
// | ? |                    |   |          | other  |  16 |     3 |
// |   |                    |   +----------+--------+-----+-------+
// | L |                    |   | Infinity | EDOM   |  17 |     4 |
// |   |                    |   |          +--------+-----+-------+
// | ? |                    |   |          | other  |  18 |     4 |
// |   |                    |   +----------+--------+-----+-------+
// | L |                    |   | NaN      | EDOM   |  19 |     5 |
// |   |                    |   |          +--------+-----+-------+
// | ? |                    |   |          | other  |  20 |     5 |
// |   |                    +---+----------+--------+-----+-------+
// | ? | e_WEIRD_ERRNO      | F | 0        | other  |  21 |     5 |
// |   |                    |   +----------+--------+-----+-------+
// | L |                    |   | Subnorm. | EDOM   |  22 |     6 |
// |   |                    |   |          +--------+-----+-------+
// | ? |                    |   |          | other  |  23 |     6 |
// |   |                    |   +----------+--------+-----+-------+
// | L |                    |   | Normal   | EDOM   |  24 |     7 |
// |   |                    |   |          +--------+-----+-------+
// | ? |                    |   |          | other  |  25 |     7 |
// |   |                    |   +----------+--------+-----+-------+
// | L |                    |   | Infinity | EDOM   |  26 |     8 |
// |   |                    |   |          +--------+-----+-------+
// | ? |                    |   |          | other  |  27 |     8 |
// |   |                    |   +----------+--------+-----+-------+
// | L |                    |   | NaN      | EDOM   |  28 |     9 |
// |   |                    |   |          +--------+-----+-------+
// | ? |                    |   |          | other  |  29 |     9 |
// +---+--------------------+---+----------+--------+-----+-------+
// | ? | e_BOGUS_SUCCESS    | F | 0        | 0      |  30 |     9 |
// |   |                    |   +----------+--------+-----+-------+
// | ? |                    |   | Subnorm. | 0      |  31 |     9 |
// |   |                    |   +----------+--------+-----+-------+
// | ? |                    |   | Normal   | 0      |  32 |     9 |
// |   |                    |   +----------+--------+-----+-------+
// | ? |                    |   | Infinity | 0      |  33 |     9 |
// |   |                    |   +----------+--------+-----+-------+
// | ? |                    |   | NaN      | 0      |  34 |     9 |
// +---+--------------------+---+----------+--------+-----+-------+
// | ? | e_BOGUS_FAILURE    | P | 0        | 0      |     |       |
// |   |                    |   |          +--------+-----+-------+
// | ? |                    |   |          | EINVAL |  26 |       |
// +---+--------------------+---+----------+--------+-----+-------+
// | ? | e_MISSED_OVERFLOW  | P | 0        | 0      |     |       |
// | ? |                    |   |          | ERANGE |     |       |
// | ? |                    |   | Subnorm. | 0      |     |       |
// | ? |                    |   |          | ERANGE |     |       |
// | ? |                    |   |          | EINVAL |     |       |
// | ? |                    |   | Normal   | 0      |     |       |
// | ? |                    |   |          | ERANGE |     |       |
// | ? |                    |   |          | EINVAL |     |       |
// | ? |                    |   | Infinity | 0      |     |       |
// | ? |                    |   |          | EINVAL |     |       |
// | ? |                    |   | NaN      | 0      |     |       |
// | ? |                    |   |          | ERANGE |     |       |
// | ? |                    |   |          | EINVAL |     |       |
// +---+--------------------+---+----------+--------+-----+-------+
// | ? | e_MISSED_UNDERFLOW | P | 0        | 0      |     |       |
// | ? |                    |   |          | EINVAL |     |       |
// | ? |                    |   | Subnorm. | 0      |     |       |
// | ? |                    |   |          | EINVAL |     |       |
// | ? |                    |   | Normal   | 0      |     |       |
// | ? |                    |   |          | ERANGE |     |       |
// | ? |                    |   |          | EINVAL |     |       |
// | ? |                    |   | Infinity | 0      |     |       |
// | ? |                    |   |          | ERANGE |     |       |
// | ? |                    |   |          | EINVAL |     |       |
// | ? |                    |   | NaN      | 0      |     |       |
// | ? |                    |   |          | ERANGE |     |       |
// | ? |                    |   |          | EINVAL |     |       |
// +---+--------------------+---+----------+--------+-----+-------+
// | ? | e_BOGUS_OVERFLOW   | P | Infinity | ERANGE |  65 |       |
// +---+--------------------+---+----------+--------+-----+-------+
// | ? | e_BOGUS_UNDERFLOW  | P | 0        | ERANGE |     |       |
// | L |                    |   |          | EDOM   |     |       |
// | ? |                    |   | Subnorm. | ERANGE |  68 |       |
// +---+--------------------+---+----------+--------+-----+-------+
// | ? | e_WRONG_LENGTH     | P | 0        | 0      |     |       |
// | ? |                    |   | Subnorm. | 0      |     |       |
// | ? |                    |   |          | EINVAL |     |       |
// | ? |                    |   | Normal   | 0      |     |       |
// | ? |                    |   |          | ERANGE |     |       |
// | ? |                    |   |          | EINVAL |     |       |
// | ? |                    |   | Infinity | 0      |     |       |
// | ? |                    |   |          | EINVAL |     |       |
// | ? |                    |   | NaN      | 0      |     |       |
// | ? |                    |   |          | ERANGE |     |       |
// | ? |                    |   |          | EINVAL |     |       |
// +---+--------------------+---+----------+--------+-----+-------+
// | ? | e_WRONG_VALUE      | P | 0        | 0      |     |       |
// | ? |                    |   | Subnorm. | 0      |     |       |
// | ? |                    |   |          | EINVAL |     |       |
// | ? |                    |   | Normal   | 0      |     |       |
// | ? |                    |   |          | ERANGE |     |       |
// | ? |                    |   |          | EINVAL |     |       |
// | ? |                    |   | Infinity | 0      |     |       |
// | ? |                    |   |          | EINVAL |     |       |
// | ? |                    |   | NaN      | 0      |     |       |
// | ? |                    |   |          | ERANGE |     |       |
// | ? |                    |   |          | EINVAL |     |       |
// +---+--------------------+---+----------+--------+-----+-------+
//..
//
//: LEGEND: 'L' - only on Linux
//:         '?' - need a flag to show if it was seen


Conclusion::Enum
evaluateResult(const Result&       result,
               const Expectations& expectations)
    // Return the conclusion about the specified 'strtod' 'result' when
    // evaluated in regards to the specified 'expectations'.
{
    // ERRNO
    if (!hasExpectedErrno(result)) {
        return Conclusion::e_WEIRD_ERRNO;                             // RETURN
    }

    // FAIL/PASS
    if (expectations.isFailure() && !result.isFailure()) {
        return Conclusion::e_BOGUS_SUCCESS;                           // RETURN
    }
    if (!expectations.isFailure() && result.isFailure()) {
        return Conclusion::e_BOGUS_FAILURE;                           // RETURN
    }
    if (expectations.isFailure() && result.isFailure()) {
        // Nothing else to check if it was failure
        return Conclusion::e_FAIL_OK;                                 // RETURN
    }

    // OVERFLOW
    if (expectations.isOverflow()) {
        return !result.isOverflow()
             ? Conclusion::e_MISSED_OVERFLOW
               : expectations.expected() != result.value()  // +/-Inf
               ? Conclusion::e_WRONG_VALUE
               : Conclusion::e_OVERFLOW_OK;                           // RETURN
    }

    // UNDERFLOW
    if (expectations.isUnderflow()) {
        return !result.isUnderflow()
             ? Conclusion::e_MISSED_UNDERFLOW
             : expectations.expected() != result.value()  // +/-0.0
               ? Conclusion::e_WRONG_VALUE
               : Conclusion::e_UNDERFLOW_OK;                          // RETURN
    }

    // Underflow is allowed on subnormals
    if (expectations.isInUnderflowGap() && result.isUnderflow()) {
        return expectations.expected() != result.value() // Some subnormal
             ? Conclusion::e_WRONG_VALUE
             : Conclusion::e_UNDERFLOW_SUBN;                          // RETURN
    }

    // Bogus over/underflow
    if (!expectations.isOverflow() && result.isOverflow()) {
        return Conclusion::e_BOGUS_OVERFLOW;                          // RETURN
    }
    if (!expectations.isUnderflow() && result.isUnderflow()) {
        return Conclusion::e_BOGUS_UNDERFLOW;                         // RETURN
    }

    // LENGTH
    if (expectations.parsedChars() != result.parsedChars()) {
        return Conclusion::e_WRONG_LENGTH;                            // RETURN
    }

    // NaN
    if (expectations.isNan() || result.isNan()) {
        if (expectations.isNan() != result.isNan()) {
            return Conclusion::e_WRONG_VALUE;                         // RETURN
        }
        return Conclusion::e_NAN_OK;                                  // RETURN
    }

    // Infinity
    if (expectations.isInf() || result.isInf()) {
        if (expectations.expected() != result.value()) {
            return Conclusion::e_WRONG_VALUE;                         // RETURN
        }
        return Conclusion::e_INF_OK;                                  // RETURN
    }

    // Number
    if (expectations.expected() != result.value()) {
        return Conclusion::e_WRONG_VALUE;                             // RETURN
    }

    return Conclusion::e_NUMBER_OK;
}

                           // ===================
                           // class BriefCallData
                           // ===================

class BriefCallData {
    // A read-only attribute type that stores all information about an 'strtod'
    // call and its results that are relevant in determining "the least faulty"
    // 'parseDouble' implementation we may be able to provide based on a given
    // (anomalous) 'strtod' implementation.  In other words: the information
    // needed to see if we can differentiate between faulty and correct
    // behavior just seeing the results of an 'strtod' call but not parsing
    // the input string itself heavily again.

  private:
    // DATA
    int              d_line;
    bsl::string      d_input;
    Result           d_result;
    Conclusion::Enum d_conclusion;

  public:
    // CREATORS
    BriefCallData(int                     line,
                  const bsl::string_view& input,
                  const Result&           result,
                  Conclusion::Enum        conclusion);
        // Create an object that describes an 'strtod' call made according to
        // the test data at the specified 'line', to parse the specified,
        // 'input', resulting in the specified 'result', and finally classified
        // according to the specified 'conclusion'.

    // ACCESSORS
    Conclusion::Enum conclusion() const;
    bsl::string_view input()      const;
    int              line()       const;
    const Result&    result()     const;
        // Return the requested data from this object.

    // ASPECTS
    friend
    bsl::ostream& operator<<(bsl::ostream& os, const BriefCallData& call)
    {
        return os << call.d_line << ": " << '"' << call.d_input << "\" => "
                  << call.d_result << " => "
                  << call.d_conclusion;
    }
};
                           // -------------------
                           // class BriefCallData
                           // -------------------
// CREATORS
inline
BriefCallData::BriefCallData(int                     line,
                             const bsl::string_view& input,
                             const Result&           result,
                             Conclusion::Enum        conclusion)
: d_line(line)
, d_input(input)
, d_result(result)
, d_conclusion(conclusion)
{}

// ACCESSORS
inline
Conclusion::Enum BriefCallData::conclusion() const { return d_conclusion; }

inline
bsl::string_view BriefCallData::input() const      { return d_input;      }

inline
int BriefCallData::line() const                    { return d_line;       }

inline
const Result& BriefCallData::result() const        { return d_result;     }

                        // =======================
                        // class ConclusionsMatrix
                        // =======================

class ConclusionsMatrix {
    // This complex, database-like mechanism stores all 'strtod' call inputs,
    // results and their evaluations in different views to help determine the
    // "least faulty" 'parseDouble' implementation we are able to provide given
    // an anomalous 'strtod' implementation.

  public:
    // TYPES
    typedef bsl::vector<BriefCallData>    Calls;
    typedef bsl::vector<Calls::size_type> CallRefs;

    class CatMapData {
      private:
        // DATA
        CallRefs d_asExpected;
        CallRefs d_unexpected;

      public:
        // CREATORS
        CatMapData() {}

        // MANIPULATORS
        void addCall(bool isExpectedResult, bsl::size_t index)
            // Add the specified 'index' of a call to the proper list based on
            // the specified 'isExpectedResult'.  The behavior is undefined
            // unless 'callIndex < d_calls.size()' of the calling
            // 'ConclusionsMatrix'.
        {
            (isExpectedResult ? d_asExpected : d_unexpected).push_back(index);
        }

        // ACCESSORS
        const CallRefs &asExpectedCalls() const { return d_asExpected; }
        const CallRefs &unexpectedCalls() const { return d_unexpected; }
    };

    class CatMapDataBoundRef {
        // Bind the call references to calls just for printing.

      private:
        // DATA
        const Calls&      d_calls;
        const CatMapData& d_catMapData;

      public:
        // CREATORS
        CatMapDataBoundRef(const Calls& calls, const CatMapData& catMapData)
        : d_calls(calls) , d_catMapData(catMapData) {}

        // ACCESSORS
        const Calls::value_type& operator[](bsl::size_t idx) const
        {
            return d_calls[idx];
        }

        // ASPECTS
        friend
        bsl::ostream& operator<<(bsl::ostream& os, const CatMapDataBoundRef& r)
        {
            const CallRefs& okidx = r.d_catMapData.asExpectedCalls();
            const CallRefs& noidx = r.d_catMapData.unexpectedCalls();

            ASSERT(!okidx.empty() || !noidx.empty());

            static const bsl::size_t k_OFFSET = Conclusion::k_BEGIN;
            bool conclSet[Conclusion::k_SIZEOF] = { };
            for (bsl::size_t i = 0; i < okidx.size(); ++i) {
                conclSet[r[okidx[i]].conclusion() - k_OFFSET] = true;
            }

            for (bsl::size_t i = 0; i < noidx.size(); ++i) {
                conclSet[r[noidx[i]].conclusion() - k_OFFSET] = true;
            }

            bool first = true;
            static const char dlm[4] = {
                '<', '>',  // 0,1 - unexpected
                '{', '}'   // 2,3 - asExpected
            };
            for (int i = Conclusion::k_BEGIN; i < Conclusion::k_END; ++i) {
                if (!conclSet[i - k_OFFSET]) {
                    continue;                                       // CONTINUE
                }

                if (!first) { os << ", "; } else { first = false; }

                const Conclusion::Enum c = Conclusion::Enum(i);
                const bool isGood = isAsExpected(c);
                const int dlo = 2 * isGood;
                os << dlm[dlo] << c << dlm[dlo + 1];
                if (!isGood) {
                    // Let's print out the line numbers
                    bool j1st = true;
                    os << "[L#: ";
                    for (bsl::size_t j = 0; j < noidx.size(); ++j) {
                        if (r[noidx[j]].conclusion() == c) {
                            if (!j1st) { os << ", "; } else { j1st = false; }
                            os << r[noidx[j]].line();
                        }
                    }
                    os << " ]";
                }
            }

            return os;
        }
    };

    typedef bsl::map<unsigned, CatMapData> CatMap;
    typedef CatMap::value_type             CatMapEntry;

  private:
    // DATA
    Calls  d_calls;
    CatMap d_catMap;

  public:
    // CREATORS
      ConclusionsMatrix() {}
          // Create an empty matrix.

    // MANIPULATORS
    void pushBack(const BriefCallData& call);
        // Add the specified 'call' into the database.

    // ASPECTS
    friend
    bsl::ostream& operator<<(bsl::ostream& os, const ConclusionsMatrix& m)
    {
        for (CatMap::const_iterator i = m.d_catMap.begin();
                                    i != m.d_catMap.end();
                                  ++i) {
            os << ResultCatKey::fromBits(i->first) << ": "
               << CatMapDataBoundRef(m.d_calls, i->second) << '\n';
        }
        return os;
    }
};
                        // -----------------------
                        // class ConclusionsMatrix
                        // -----------------------
// MANIPULATORS
void ConclusionsMatrix::pushBack(const BriefCallData& call)
{
    d_calls.push_back(call);
    const bsl::size_t callIndex = d_calls.size() - 1;

    const ResultCatKey key = ResultCatKey(call.result());

    typedef bsl::pair<CatMap::iterator, bool> InsertResult;

    const InsertResult ir = d_catMap.insert(CatMapEntry(key, CatMapData()));

    CatMapData& data = ir.first->second;

    data.addCall(isAsExpected(call.conclusion()), callIndex);
}

                         // callStrtod(input)

Result callStrtod(const char *input)
    // The specified 'input' must be null terminated.
{
    char               *endPtr = 0;
    StrtodErrnoSampler  ERRNO;
    const double        result = bsl::strtod(input, &endPtr);
    ERRNO.grab();

    return Result(input, result, endPtr, ERRNO.value());
}

                      // verifyAssumptions(ROW)

BriefCallData verifyAssumptions(const TestDataRow& ROW)
    // Report 'strtod' unexpected behavior for the specified test data table
    // 'ROW'.
{
    // Prepare null terminated input and do the call
    const bsl::string  str(ROW.d_input);
    const Result       result = callStrtod(str.c_str());

    // Verify results against expectations
    const Expectations      expectations(ROW);
    const Conclusion::Enum  conclusion = evaluateResult(result, expectations);

    if (!isAsExpected(conclusion)) {
        bsl::cout << ROW.d_line << ": " << result << ": " << conclusion;
        switch (conclusion) {
          case Conclusion::e_WEIRD_ERRNO: {
            bsl::cout << ", " << result.errorNumber()
                      << ", " << result.valueCategory();
          } break;
          case Conclusion::e_WRONG_LENGTH: {
            bsl::cout << ", " << expectations.parsedChars()
                      << " != " << result.parsedChars();
          } break;
          case Conclusion::e_WRONG_VALUE: {
            StreamStateGuard ssg(bsl::cout); (void)ssg;
            bsl::cout.precision(17);
            bsl::cout << ", " << expectations.expected()
                      << " != " << result.value();
          } break;
          case Conclusion::e_BOGUS_OVERFLOW:// FALLTHROUGH
          case Conclusion::e_BOGUS_UNDERFLOW: {
            StreamStateGuard ssg(bsl::cout); (void)ssg;
            bsl::cout.precision(17);
            bsl::cout << ", \"" << str
                      << "\", " << result.value();
          } break;
        default:
            break;
        }
        bsl::cout << '\n';

        ASSERT(isAsExpected(conclusion));
    }

    return BriefCallData(ROW.d_line, ROW.d_input, result, conclusion);
}

void reportForWholeTestTable()
    // Print a report 'strtod' unexpected behavior using the global test data.
{
    ConclusionsMatrix matrix;

    for (bsl::size_t ti = 0; ti < NUM_TEST_DATA; ++ti) {
        if (veryVerbose) {
            P(TEST_DATA[ti].d_line);
        }
        matrix.pushBack(verifyAssumptions(TEST_DATA[ti]));
    }

    bsl::cout << matrix << '\n';
}

void verifyForRow(const TestDataRow& ROW)
    // Print a report 'strtod' unexpected behavior about the input in the
    // specified test data 'ROW'.
{
    const BriefCallData& bcd = verifyAssumptions(ROW);
    if (!isAsExpected(bcd.conclusion())) {
        bsl::cout << "Unexpected 'strtod' behavior: " << bcd << '\n';
    }
}

}  // close namespace StrtodAssumptions
#endif  // u_PARSEDOUBLE_USES_STRTOD

#ifdef u_PARSEDOUBLE_USES_FROM_CHARS
namespace FromCharsAssumptions {

#ifdef u_PARSEDOUBLE_USES_STRTOD_ON_RANGE_ERRORS_ONLY
    #define u_HAS_LWG_3081  false;
#else
    #define u_HAS_LWG_3081  true;
#endif

const bool k_OVER  = true;
const bool k_UNDER = false;

void verifyLwg3081(const std::string_view& toParse,
                   bool                    isOverflow,
                   bsl::chars_format       fmt)
{
    const bool hasLwg3081 = u_HAS_LWG_3081;

    bsl::string_view        s{ toParse };
    bsl::from_chars_result  r{ };
    bsl::error_code        ec{ };
    double                  d{ };

    using bsl::from_chars;
    using bsl::chars_format;

    d = 42;
    r  = from_chars(s.data(), s.data() + s.size(), d, fmt);
    ec = bsl::make_error_code(r.ec);
    ASSERTV(ec, r.ec == bsl::errc::result_out_of_range);
    ASSERTV(d,  hasLwg3081 || 42 == d);

    const bool k_NEGATIVE = ('-' == toParse.substr(0, 1)[0]);
    if (k_NEGATIVE) {
        if (isOverflow) {
            ASSERTV(d, !hasLwg3081 || -Limits::infinity() == d);
        }
        else {
            ASSERTV(d, !hasLwg3081 || (0.0 == d && isNegative(d)));
        }

        d = 42;
        s.remove_prefix(1);  // Drop the negative sign
        r = from_chars(s.data(), s.data() + s.size(), d, fmt);
        ec = bsl::make_error_code(r.ec);
        ASSERTV(ec, r.ec == bsl::errc::result_out_of_range);
        ASSERTV(d, hasLwg3081 || 42 == d);
    }
    if (isOverflow) {
        ASSERTV(d, !hasLwg3081 || Limits::infinity() == d);
    }
    else {
        ASSERTV(d, !hasLwg3081 || (0.0 == d && !isNegative(d)));
    }
}

void verify()
    // Verify assumptions about 'std::from_chars' behavior.  Currently verifies
    // only issue LWG 3081 related behavior: does it set the 'double' output
    // value when parsing out-of-range input.
{
    const auto hexFmt = bsl::chars_format::hex;
    const auto genFmt = bsl::chars_format::general;

    TestAssertCounter testStatusGuard;

    verifyLwg3081("-1p-1075",                 k_UNDER, hexFmt);
    verifyLwg3081("-2.4703282292062327e-324", k_UNDER, genFmt);

    verifyLwg3081("-1p1024",  k_OVER, hexFmt);
    verifyLwg3081("-1.8e308", k_OVER, genFmt);
}

void report()
    // Verify and report assumptions about 'std::from_chars' behavior.
    // See also 'verify()' above.
{
    TestAssertCounter testStatusGuard;

    verify();

    if (testStatus == 0) {
        bsl::cout << "'std::from_chars' assumption in implementation hold.\n";
    }
}

}  // close namespace FromCharsAssumptions
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV

// ============================================================================
// PARSE DOUBLE BENCHMARKING

volatile double sink = 0.0;
    // Attempting to avoid optimization in the benchmark code

void benchmarkParseDouble(int numofWarmupRuns, int numofMeasuredRuns)
    // Call 'parseDouble' on each line of the test data table the specified
    // 'numofWarmupRuns' times, then call 'parseDouble' on each line of the
    // test data table the specified 'numofMeasuredRuns' times while collecting
    // CPU use and wall time data.  Then report the collected data to standard
    // output and return.
{
    // Silence 'BSLS_REVIEW', its handling is irrelevant to the benchmark.
    bsls::Review::setViolationHandler(nullReviewHandler);

    // Not measured, to get the tight loop "recognized"
    for (int wi = 0; wi < numofWarmupRuns; ++wi) {
        for (bsl::size_t ti = 0; ti < NUM_TEST_DATA; ++ti) {
            double d;
            if (0 == Util::parseDouble(&d, TEST_DATA[ti].d_input)) {
                // Pretend the result is used
                sink = sink + d;
            }
        }
    }

    bsls::Stopwatch sw;
    sw.start(bsls::Stopwatch::k_COLLECT_WALL_AND_CPU_TIMES);
    for (int mi = 0; mi < numofMeasuredRuns; ++mi) {
        for (bsl::size_t ti = 0; ti < NUM_TEST_DATA; ++ti) {
            double d;
            if (0 == Util::parseDouble(&d, TEST_DATA[ti].d_input)) {
                // Pretend the result is used
                sink = sink + d;
            }
        }
    }
    sw.stop();

    // All times spent running
    const double allUserSec = sw.accumulatedUserTime();
    const double allSystSec = sw.accumulatedSystemTime();
    const double allWallSec = sw.accumulatedWallTime();

    // The average time to run the whole test data table
    const double tblUserMilli = allUserSec * 1000 / numofMeasuredRuns;
    const double tblSystMilli = allSystSec * 1000 / numofMeasuredRuns;
    const double tblWallMilli = allWallSec * 1000 / numofMeasuredRuns;

    // The average time per table line, which is a call
    const double dblNumRows = static_cast<double>(NUM_TEST_DATA);
        // Avoids long lines and a conversion warning.

    const double callUserNano = tblUserMilli * 1000000 / dblNumRows;
    const double callSystNano = tblSystMilli * 1000000 / dblNumRows;
    const double callWallNano = tblWallMilli * 1000000 / dblNumRows;

    StreamStateGuard ssg(bsl::cout); (void)ssg;
    bsl::cout.precision(2);
    bsl::cout << bsl::fixed;

    bsl::cout << "Full run\n"
                  "    User:   " << allUserSec << " s\n"
                  "    System: " << allSystSec << " s\n"
                  "    Wall:   " << allWallSec << " s\n\n";

    bsl::cout << "Per call\n"
                 "    User:   " << callUserNano << " ns\n"
                 "    System: " << callSystNano << " ns\n"
                 "    Wall:   " << callWallNano << " ns\n\n";
}

// ============================================================================
// PARSE DOUBLE TEST MACHINERY VERIFICATION

void testCalcRestPos()
    // Verifies the 'calcRestPos' support function.  See the calling test
    // 'case' description for concerns and plan.
{
    // 'ParsedChars' constants
    static const ParsedChars k_ALL = bsl::numeric_limits<ParsedChars>::min();

    static const struct {
        int         d_line;
        bsl::size_t d_length;
        ParsedChars d_restPosSpec;
        bsl::size_t d_expected;
    } DATA[] = {
        //     Length   PosSpec    Expected
        //--   ------   --------   --------
        {L_,   0,       0,         0       },
        {L_,   0,       k_ALL,     0       },

        {L_,   1,        0,        0       },
        {L_,   1,        1,        1       },
        {L_,   1,       -1,        0       },
        {L_,   1,       k_ALL,     1       },

        {L_,   2,        0,        0       },
        {L_,   2,        1,        1       },
        {L_,   2,        2,        2       },
        {L_,   2,       -1,        1       },
        {L_,   2,       -2,        0       },
        {L_,   2,       k_ALL,     2       },

        {L_,   7,        0,        0       },
        {L_,   7,        1,        1       },
        {L_,   7,        2,        2       },
        {L_,   7,        3,        3       },
        {L_,   7,        4,        4       },
        {L_,   7,        5,        5       },
        {L_,   7,        6,        6       },
        {L_,   7,        7,        7       },
        {L_,   7,       -1,        6       },
        {L_,   7,       -2,        5       },
        {L_,   7,       -3,        4       },
        {L_,   7,       -4,        3       },
        {L_,   7,       -5,        2       },
        {L_,   7,       -6,        1       },
        {L_,   7,       -7,        0       },
        {L_,   7,       k_ALL,     7       },
    };

    const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE     = DATA[ti].d_line;
        const size_t      LENGTH   = DATA[ti].d_length;
        const ParsedChars POS_SPEC = DATA[ti].d_restPosSpec;
        const size_t      EXPECTED = DATA[ti].d_expected;

        const size_t RESULT = calcRestPos(POS_SPEC, LENGTH);

        ASSERTV(LINE, LENGTH, POS_SPEC, EXPECTED, RESULT, EXPECTED == RESULT);
    }
}

void testParseExpected()
    // Verifies the 'testParseExpected' support function.  See the calling test
    // 'case' description for concerns and plan.
{

    struct SolarisTestData {
        int         d_line;
        const char *d_literal_p;
        double      d_expected;
        double      d_manual;
    };

#define IROW(literal) {L_, #literal, literal##., 0}
    // Non-hex row: integers

#define CROW(constantname) {L_, #constantname, constantname, 0}
    // Constant row: Symbolic constants (NtA, NaN, Inf)

#ifdef BSLS_COMPILERFEATURES_SUPPORT_HEXFLOAT_LITERALS
    #define HROW(literal, manual) {L_, #literal, literal, manual}
        // Hexfloat row for modern platforms
#else
    #define HROW(literal, manual) {L_, #literal, manual, manual}
        // Solaris has no hexfloat literals, AIX parses them wrong
#endif

    // Multipliers that to add/subtract from a binary exponent
    static const double k_M2  = 0x4ull;
    static const double k_M6  = 0x40ull;
    static const double k_M10 = 0x400ull;
    static const double k_M14 = 0x4000ull;
    static const double k_M18 = 0x40000ull;
    static const double k_M22 = 0x400000ull;
    static const double k_M26 = 0x4000000ull;
    static const double k_M30 = 0x40000000ull;
    static const double k_M34 = 0x400000000ull;
    static const double k_M38 = 0x4000000000ull;
    static const double k_M42 = 0x40000000000ull;
    static const double k_M46 = 0x400000000000ull;

    static const double k_M33 = 0x0000000200000000ull;
    static const double k_M63 = 0x8000000000000000ull;

    static const double k_M222 = k_M63 * k_M63 * k_M63 * k_M33;

    static const SolarisTestData DATA[] = {

        // First 64 integers
        IROW( 0),IROW( 1),IROW( 2),IROW( 3),
        IROW( 4),IROW( 5),IROW( 6),IROW( 7),
        IROW( 8),IROW( 9),IROW(10),IROW(11),
        IROW(12),IROW(13),IROW(14),IROW(15),
        IROW(16),IROW(17),IROW(18),IROW(19),
        IROW(20),IROW(21),IROW(22),IROW(23),
        IROW(24),IROW(25),IROW(26),IROW(27),
        IROW(28),IROW(29),IROW(30),IROW(31),
        IROW(32),IROW(33),IROW(34),IROW(35),
        IROW(36),IROW(37),IROW(38),IROW(39),
        IROW(40),IROW(11),IROW(42),IROW(43),
        IROW(44),IROW(45),IROW(46),IROW(47),
        IROW(48),IROW(49),IROW(50),IROW(11),
        IROW(52),IROW(53),IROW(54),IROW(55),
        IROW(56),IROW(57),IROW(58),IROW(59),
        IROW(60),IROW(11),IROW(62),IROW(63),

        // Random integers that fit
        IROW(459876),
        IROW(4359876035),

        // Maximum integer we support
        IROW(9007199254740991),  // 1F FFFF FFFF FFFF => 53 set bits

        // Special values
        CROW(NtA),
        CROW(NaN),
        CROW(Inf),

        // Smallest possible (subnormal) number as hexfloat
        HROW(0x0.0000000000001p-1022, Limits::denorm_min()),

        // Smallest normal number as hexfloat
        HROW(0x1.0000000000000p-1022, Limits::min()),
        HROW(0x1.000000000000p-1022,  Limits::min()),
        HROW(0x1.00000000000p-1022,   Limits::min()),
        HROW(0x1.0000000000p-1022,    Limits::min()),
        HROW(0x1.000000000p-1022,     Limits::min()),
        HROW(0x1.00000000p-1022,      Limits::min()),
        HROW(0x1.0000000p-1022,       Limits::min()),
        HROW(0x1.000000p-1022,        Limits::min()),
        HROW(0x1.00000p-1022,         Limits::min()),
        HROW(0x1.0000p-1022,          Limits::min()),
        HROW(0x1.000p-1022,           Limits::min()),
        HROW(0x1.00p-1022,            Limits::min()),
        HROW(0x1.0p-1022,             Limits::min()),
        HROW(0x1.p-1022,              Limits::min()),
        HROW(0x1p-1022,               Limits::min()),

        // Smallest normal number with all significand bits set
        HROW(0x1.FFFFFFFFFFFFFp-1022, Limits::min() +
                            Limits::denorm_min() * 0xFFFFFFFFFFFFFull),

        // Just havin' fun
        HROW(0x1.0000000000001p-1021,
                Limits::min() * 2 + Limits::denorm_min() * 2),

        // Another trailing zero test set
        HROW(0x1.0000000000000p-800, Limits::min() * k_M222),
        HROW(0x1.000000000000p-800,  Limits::min() * k_M222),
        HROW(0x1.00000000000p-800,   Limits::min() * k_M222),
        HROW(0x1.0000000000p-800,    Limits::min() * k_M222),
        HROW(0x1.000000000p-800,     Limits::min() * k_M222),
        HROW(0x1.00000000p-800,      Limits::min() * k_M222),
        HROW(0x1.0000000p-800,       Limits::min() * k_M222),
        HROW(0x1.000000p-800,        Limits::min() * k_M222),
        HROW(0x1.00000p-800,         Limits::min() * k_M222),
        HROW(0x1.0000p-800,          Limits::min() * k_M222),
        HROW(0x1.000p-800,           Limits::min() * k_M222),
        HROW(0x1.00p-800,            Limits::min() * k_M222),
        HROW(0x1.0p-800,             Limits::min() * k_M222),
        HROW(0x1.p-800,              Limits::min() * k_M222),
        HROW(0x1p-800,               Limits::min() * k_M222),

        // Trailing 0 tests with non-zero fraction variations
        HROW(0x1.0000000000001p50,  0x10000000000001ull / k_M2),

        HROW(0x1.0000000000010p50,  0x10000000000010ull / k_M2),
        HROW(0x1.000000000001p50,   0x10000000000010ull / k_M2),

        HROW(0x1.0000000000040p50,  0x10000000000040ull / k_M2),
        HROW(0x1.000000000004p50,   0x10000000000040ull / k_M2),

        HROW(0x1.00032000000F0p50,  0x100032000000F0ull / k_M2),
        HROW(0x1.00032000000Fp50,   0x100032000000F0ull / k_M2),

        HROW(0x1.0000000000100p50,  0x100000000001ull * k_M6),
        HROW(0x1.000000000010p50,   0x100000000001ull * k_M6),
        HROW(0x1.00000000001p50,    0x100000000001ull * k_M6),

        HROW(0x1.0000000001000p50,  0x10000000001ull * k_M10),
        HROW(0x1.000000000100p50,   0x10000000001ull * k_M10),
        HROW(0x1.00000000010p50,    0x10000000001ull * k_M10),
        HROW(0x1.0000000001p50,     0x10000000001ull * k_M10),

        HROW(0x1.0000000010000p50,  0x1000000001ull * k_M14),
        HROW(0x1.000000001000p50,   0x1000000001ull * k_M14),
        HROW(0x1.00000000100p50,    0x1000000001ull * k_M14),
        HROW(0x1.0000000010p50,     0x1000000001ull * k_M14),
        HROW(0x1.000000001p50,      0x1000000001ull * k_M14),

        HROW(0x1.0000000100000p50,  0x100000001ull * k_M18),
        HROW(0x1.000000010000p50,   0x100000001ull * k_M18),
        HROW(0x1.00000001000p50,    0x100000001ull * k_M18),
        HROW(0x1.0000000100p50,     0x100000001ull * k_M18),
        HROW(0x1.000000010p50,      0x100000001ull * k_M18),
        HROW(0x1.00000001p50,       0x100000001ull * k_M18),

        HROW(0x1.0000001000000p50,  0x10000001ull * k_M22),
        HROW(0x1.000000100000p50,   0x10000001ull * k_M22),
        HROW(0x1.00000010000p50,    0x10000001ull * k_M22),
        HROW(0x1.0000001000p50,     0x10000001ull * k_M22),
        HROW(0x1.000000100p50,      0x10000001ull * k_M22),
        HROW(0x1.00000010p50,       0x10000001ull * k_M22),
        HROW(0x1.0000001p50,        0x10000001ull * k_M22),

        HROW(0x1.0000010000000p50,  0x1000001ull * k_M26),
        HROW(0x1.000001000000p50,   0x1000001ull * k_M26),
        HROW(0x1.00000100000p50,    0x1000001ull * k_M26),
        HROW(0x1.0000010000p50,     0x1000001ull * k_M26),
        HROW(0x1.000001000p50,      0x1000001ull * k_M26),
        HROW(0x1.00000100p50,       0x1000001ull * k_M26),
        HROW(0x1.0000010p50,        0x1000001ull * k_M26),
        HROW(0x1.000001p50,         0x1000001ull * k_M26),

        HROW(0x1.0000100000000p50,  0x100001ull * k_M30),
        HROW(0x1.000010000000p50,   0x100001ull * k_M30),
        HROW(0x1.00001000000p50,    0x100001ull * k_M30),
        HROW(0x1.0000100000p50,     0x100001ull * k_M30),
        HROW(0x1.000010000p50,      0x100001ull * k_M30),
        HROW(0x1.00001000p50,       0x100001ull * k_M30),
        HROW(0x1.0000100p50,        0x100001ull * k_M30),
        HROW(0x1.000010p50,         0x100001ull * k_M30),
        HROW(0x1.00001p50,          0x100001ull * k_M30),

        HROW(0x1.0001000000000p50,  0x10001ull * k_M34),
        HROW(0x1.000100000000p50,   0x10001ull * k_M34),
        HROW(0x1.00010000000p50,    0x10001ull * k_M34),
        HROW(0x1.0001000000p50,     0x10001ull * k_M34),
        HROW(0x1.000100000p50,      0x10001ull * k_M34),
        HROW(0x1.00010000p50,       0x10001ull * k_M34),
        HROW(0x1.0001000p50,        0x10001ull * k_M34),
        HROW(0x1.000100p50,         0x10001ull * k_M34),
        HROW(0x1.00010p50,          0x10001ull * k_M34),
        HROW(0x1.0001p50,           0x10001ull * k_M34),

        HROW(0x1.0010000000000p50,  0x1001ull * k_M38),
        HROW(0x1.001000000000p50,   0x1001ull * k_M38),
        HROW(0x1.00100000000p50,    0x1001ull * k_M38),
        HROW(0x1.0010000000p50,     0x1001ull * k_M38),
        HROW(0x1.001000000p50,      0x1001ull * k_M38),
        HROW(0x1.00100000p50,       0x1001ull * k_M38),
        HROW(0x1.0010000p50,        0x1001ull * k_M38),
        HROW(0x1.001000p50,         0x1001ull * k_M38),
        HROW(0x1.00100p50,          0x1001ull * k_M38),
        HROW(0x1.0010p50,           0x1001ull * k_M38),
        HROW(0x1.001p50,            0x1001ull * k_M38),

        HROW(0x1.0100000000000p50,  0x101ull * k_M42),
        HROW(0x1.010000000000p50,   0x101ull * k_M42),
        HROW(0x1.01000000000p50,    0x101ull * k_M42),
        HROW(0x1.0100000000p50,     0x101ull * k_M42),
        HROW(0x1.010000000p50,      0x101ull * k_M42),
        HROW(0x1.01000000p50,       0x101ull * k_M42),
        HROW(0x1.0100000p50,        0x101ull * k_M42),
        HROW(0x1.010000p50,         0x101ull * k_M42),
        HROW(0x1.01000p50,          0x101ull * k_M42),
        HROW(0x1.0100p50,           0x101ull * k_M42),
        HROW(0x1.010p50,            0x101ull * k_M42),
        HROW(0x1.01p50,             0x101ull * k_M42),

        HROW(0x1.1000000000000p50,  0x11ull * k_M46),
        HROW(0x1.100000000000p50,   0x11ull * k_M46),
        HROW(0x1.10000000000p50,    0x11ull * k_M46),
        HROW(0x1.1000000000p50,     0x11ull * k_M46),
        HROW(0x1.100000000p50,      0x11ull * k_M46),
        HROW(0x1.10000000p50,       0x11ull * k_M46),
        HROW(0x1.1000000p50,        0x11ull * k_M46),
        HROW(0x1.100000p50,         0x11ull * k_M46),
        HROW(0x1.10000p50,          0x11ull * k_M46),
        HROW(0x1.1000p50,           0x11ull * k_M46),
        HROW(0x1.100p50,            0x11ull * k_M46),
        HROW(0x1.10p50,             0x11ull * k_M46),
        HROW(0x1.1p50,              0x11ull * k_M46),

        // Hexfloats with no fraction digits
        HROW(0x1.p0, 1),
        HROW(0x1p0,  1),
        HROW(0x1p2,  4),
        HROW(0x1p+3, 8),  // optional exponent '+' sign
    };

    const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int              LINE     = DATA[ti].d_line;
        const bsl::string_view LITERAL  = DATA[ti].d_literal_p;
        const double           EXPECTED = DATA[ti].d_expected;

        const double result = parseExpected(LITERAL);
            // For readability of the test assertion below

        ASSERTV(LINE, LITERAL, EXPECTED, result,
                (EXPECTED == result) || (isNan(EXPECTED) && isNan(result)));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_HEXFLOAT_LITERALS
        // When hexfloat literals are supported we compare the manual value to
        // the value created by the compiler to ensure the correctness of this
        // local test table.
        const bool IS_HEX = LITERAL.substr(0, 2) == "0x";
        if (IS_HEX) {
            const double MANUAL_EXPECTED = DATA[ti].d_manual;
                // Meaningfully filled for hexfloats only

            ASSERTV(LINE, LITERAL, EXPECTED, MANUAL_EXPECTED,
                    EXPECTED == MANUAL_EXPECTED);
        }
#endif
    }
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_HEXFLOAT_LITERALS
void testParseExpectedOnMainDataWithCompilerAsOracle()
    // Extra verification for the 'testParseExpected' function on platforms
    // that properly support hexadecimal floating point literals for 'double'.
    // See the calling test 'case' description for concerns and plan.
{
    for (bsl::size_t ti = 0; ti < NUM_TEST_DATA; ++ti) {
        const int               LINE     = TEST_DATA[ti].d_line;
        const bsl::string_view& LITERAL  = TEST_DATA[ti].d_expLiteral;
        const double            EXPECTED = TEST_DATA[ti].d_expected;

        if (veryVerbose) {
            P_(LINE) P_(LITERAL) P(EXPECTED);
        }

        const double result = parseExpected(LITERAL);

        ASSERTV(LINE, LITERAL, EXPECTED, result,
                EXPECTED == result || (isNan(EXPECTED) && isNan(result)));
    }
}
#endif  // BSLS_COMPILERFEATURES_SUPPORT_HEXFLOAT_LITERALS

void testTestAssertCounter()
    // Verifies the 'TestAssertCounter' mechanism.  See the calling test 'case'
    // description for concerns and plan.
{
    ASSERTV(TestAssertCounter::k_MAX_TESTSTATUS,
            100 == TestAssertCounter::k_MAX_TESTSTATUS);

    const int savedTestStatus = testStatus;
        // No exceptions here, simple save/restore suffices

    int assertionCount = 0;
        // As we keep overwriting 'testStatus' we have to maintain our own
        // assertion count.

#define ASSERTL(...) do { if (!u_LASTARG(__VA_ARGS__)) ++assertionCount;      \
                          ASSERTV(__VA_ARGS__) } while (false)
    // Local 'ASSERT' that increases 'assertionCount' in case of failure.

    // Verify that construction sets 'testStatus' to zero and activates.
    {

        testStatus = 42;
        TestAssertCounter tsg;

        ASSERTL(testStatus, 0 == testStatus);
        ASSERTL(tsg.isActive());
    }
#undef ASSERTL

    static const struct {
        int d_line;
        int d_original;
        int d_monitored;
        int d_final;
    } DATA[] = {
        //    orig   moni  final
        //--  ----   ----  -----
        {L_,    0,     0,     0  },
        {L_,    0,     1,     1  },
        {L_,    0,     2,     2  },
        {L_,    0,    50,    50  },
        {L_,    0,    99,    99  },
        {L_,    0,   100,   100  },

        {L_,    1,     0,     1  },
        {L_,    1,     1,     2  },
        {L_,    1,     2,     3  },
        {L_,    1,    50,    51  },
        {L_,    1,    99,   100  },
        {L_,    1,   100,   100  },

        {L_,    2,     0,     2  },
        {L_,    2,     1,     3  },
        {L_,    2,     2,     4  },
        {L_,    2,    50,    52  },
        {L_,    2,    99,   100  },
        {L_,    2,   100,   100  },

        {L_,   50,     0,    50  },
        {L_,   50,     1,    51  },
        {L_,   50,     2,    52  },
        {L_,   50,    50,   100  },
        {L_,   50,    99,   100  },
        {L_,   50,   100,   100  },

        {L_,   99,     0,    99  },
        {L_,   99,     1,   100  },
        {L_,   99,     2,   100  },
        {L_,   99,    50,   100  },
        {L_,   99,    99,   100  },
        {L_,   99,   100,   100  },

        {L_,  100,     0,   100  },
        {L_,  100,     1,   100  },
        {L_,  100,     2,   100  },
        {L_,  100,    50,   100  },
        {L_,  100,    99,   100  },
        {L_,  100,   100,   100  },
    };

    const bsl::size_t NUM_DATA = sizeof DATA / sizeof * DATA;

    for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int LINE      = DATA[ti].d_line;
        const int ORIGINAL  = DATA[ti].d_original;
        const int MONITORED = DATA[ti].d_monitored;
        const int FINAL     = DATA[ti].d_final;

        if (veryVerbose) {
            P_(LINE) P_(ORIGINAL) P_(MONITORED) P(FINAL);
        }

#define ASSERTL(...) do { if (!u_LASTARG(__VA_ARGS__)) ++assertionCount;      \
                          ASSERTV(LINE, __VA_ARGS__) } while (false)
        // Local 'ASSERT' that increases 'assertionCount' in case of failure,
        // and also adds LINE information.

        // Verifying simplest case
        testStatus = ORIGINAL;
        {
            TestAssertCounter tsg; (void)tsg;
            testStatus = MONITORED;
        }
        ASSERTL(FINAL, testStatus, FINAL == testStatus);

        // Verifying 'release()', and 'reset()'
        testStatus = ORIGINAL;
        {
            TestAssertCounter tsg;
            testStatus = MONITORED;

            tsg.release();
            ASSERTL(false == tsg.isActive());
            ASSERTL(FINAL, testStatus, FINAL == testStatus);

            tsg.reset();
            ASSERTL( true == tsg.isActive());
            ASSERTL(testStatus, 0 == testStatus);

            tsg.release();
            ASSERTL(false == tsg.isActive());
            ASSERTL(FINAL, testStatus, FINAL == testStatus);

            for (bsl::size_t tj = 0; tj < NUM_DATA; ++tj) {
                const int LINE2      = DATA[tj].d_line;
                const int ORIGINAL2  = DATA[tj].d_original;
                const int MONITORED2 = DATA[tj].d_monitored;
                const int FINAL2     = DATA[tj].d_final;

                if (veryVerbose) {
                    P_(LINE2) P_(ORIGINAL2) P_(MONITORED2) P(FINAL2);
                }

                testStatus = ORIGINAL2;
                tsg.reset();
                ASSERTL(LINE2, true == tsg.isActive());
                ASSERTL(LINE2, testStatus, 0 == testStatus);

                testStatus = MONITORED2;
                tsg.release();
                ASSERTL(LINE2, false == tsg.isActive());
                ASSERTL(LINE2, FINAL2, testStatus, FINAL2 == testStatus);
            }

            testStatus = ORIGINAL;
            tsg.reset();
            ASSERTL(true == tsg.isActive());
            testStatus = MONITORED;
        }
        ASSERTL(FINAL, testStatus, FINAL == testStatus);
    }
#undef ASSERTL

    testStatus = savedTestStatus + assertionCount;
    if (testStatus > 100) {
        testStatus = 100;
    }
}

void testBslsLogCounterGuard()
    // Verifies the 'BslsLogCounterGuard' mechanism.  See the calling test
    // 'case' description for concerns and plan.
{
    // Abbreviations for more readable code
    using   bsls::Log;
    using   bsls::LogSeverity;
    typedef Log::LogMessageHandler LogMessageHandler;

    Log::setSeverityThreshold(LogSeverity::e_TRACE);

    const LogMessageHandler originalLogHandler = Log::logMessageHandler();
    {
        BslsLogCounterGuard guard(veryVerbose);

        ASSERT(originalLogHandler != Log::logMessageHandler());

        BSLS_LOG_FATAL("'BslsLogCounterGuard' test log message FATAL");
        ASSERTV(BslsLogCounterGuard::counter(),
                1 == BslsLogCounterGuard::counter());

        BSLS_LOG_INFO("TESTING 'BslsLogCounterGuard' test log message INFO");
        ASSERTV(BslsLogCounterGuard::counter(),
                2 == BslsLogCounterGuard::counter());
    }
    ASSERT(originalLogHandler == Log::logMessageHandler());
}

void testBslsReviewMonitorCounters(const BslsReviewMonitor&  monitor,
                                   int                       fileLine,
                                   const char               *file,
                                   int                       commentLine,
                                   const char               *comment,
                                   char                      category)
    // Verifies the working of the counters in the 'BslsReviewMonitor'
    // mechanism on expected review violations.  Use the specified 'monitor',
    // with test data from the specified 'file', 'comment', and 'category'.
    // Use the specified 'fileLine' and 'commentLine' in assertions to identify
    // the location of the test data in this file.  The behavior is undefined
    // unless 'file' is a file name that matches this component(*), and
    // category is one of 'H', 'O', or 'U'.
{
    const bool IS_VERBOSE    = monitor.isVerbose();

    const bool IS_HEXFLOAT   = ('H' == category);
    const bool IS_OVERFLOW   = ('O' == category);
    const bool IS_UNDERFLOW  = ('U' == category);

#define ASSERTL(...)                                                          \
    ASSERTV(fileLine, commentLine, Opt(file), category, Opt(comment),         \
            __VA_ARGS__)
        // Local assert that prints all values to understand what failed

    const int origNumHexf  = monitor.numHexfloat();
    const int origNumOver  = monitor.numOverflow();
    const int origNumUnder = monitor.numUnderflow();

    const int EXP_NUM_HEXFLOAT  = origNumHexf  + IS_HEXFLOAT;
    const int EXP_NUM_OVERFLOW  = origNumOver  + IS_OVERFLOW;
    const int EXP_NUM_UNDERFLOW = origNumUnder + IS_UNDERFLOW;

    using bsls::Review;
    using bsls::ReviewViolation;
    typedef Review::ViolationHandler ReviewHandler;

    const ReviewHandler monitoringHandler = Review::violationHandler();

    const ReviewViolation reviewViolation(comment, file, L_, "11", 0);
                                                    // line, level, count
    {
        // This guard counts log messages.
        const BslsLogCounterGuard lcg(false); (void)lcg;

        // If we get an assertion failure for an expected review message we
        // want to tell the test data line numbers that had the issue.  This
        // guard allows us to monitor those unexpected assertions even if
        // 'testStatus' is at the maximum 100.
        TestAssertCounter tsg; (void)tsg;

        monitoringHandler(reviewViolation);

        // Save the number of test assert failures we counted.
        const int numAsserts = testStatus;
        tsg.release();  // Stop the counter.

        ASSERTL(numAsserts, 0 == numAsserts);
        // There should be no assertion failures

        if (IS_VERBOSE) {
            ASSERTL(lcg.counter(), 1 == lcg.counter());
            // There should be exactly one log message if monitor is verbose
        }
        else {
            ASSERTL(lcg.counter(), 0 == lcg.counter());
            // There should be no log messages if the monitor isn't verbose
        }
    }

    ASSERTL(origNumHexf, EXP_NUM_HEXFLOAT, monitor.numHexfloat(),
            EXP_NUM_HEXFLOAT == monitor.numHexfloat());

    ASSERTL(origNumOver, EXP_NUM_OVERFLOW, monitor.numOverflow(),
            EXP_NUM_OVERFLOW == monitor.numOverflow());

    ASSERTL(origNumUnder, EXP_NUM_UNDERFLOW, monitor.numUnderflow(),
            EXP_NUM_UNDERFLOW == monitor.numUnderflow());
#undef ASSERTL
}

void testBslsReviewMonitorAsserts(const BslsReviewMonitor&  monitor,
                                  int                       fileLine,
                                  const char               *file,
                                  int                       commentLine,
                                  const char               *comment)
    // Verifies the existence of assertions in the 'BslsReviewMonitor'
    // mechanism for unexpected review violations as well as that counters are
    // unchanged by such unexpected review message.  Use the specified
    // 'monitor' with test data from the specified 'file', 'comment'.  Use the
    // specified 'fileLine' and 'commentLine' in assertions to identify the
    // location of the test data in this file.  The behavior is undefined
    // unless either 'file' does not matches this component(*), or comment does
    // not match the underflow, overflow, or hexfloat messages.
{
    // Abbreviations for readable code

#define ASSERTL(...)                                                          \
    ASSERTV(fileLine, commentLine, Opt(file), Opt(comment), __VA_ARGS__)
        // Local assert that prints all values to understand what failed

    const int origNumHexf  = monitor.numHexfloat();
    const int origNumOver  = monitor.numOverflow();
    const int origNumUnder = monitor.numUnderflow();

    using bsls::Review;
    using bsls::ReviewViolation;
    typedef Review::ViolationHandler ReviewHandler;

    const ReviewHandler monitoringHandler = Review::violationHandler();

    const ReviewViolation reviewViolation(comment, file, L_, "11", 0);
                                                    // line, level, count
    {
        // This guard silences and counts the 'failByLog' messages of the
        // unexpected review violations unless the user requested to see them.
        const BslsLogCounterGuard lcg(veryVerbose); (void)lcg;

        // This guard allows us to count the number of assertions that occur
        // even if 'testStatus' is already at its maximum 100.
        TestAssertCounter tsg;
        {
            // This guard silences the test assertion failures ('cout') unless
            // the user of the test driver requested otherwise.
            const OstreamSilencerGuard osg(bsl::cout, veryVerbose); (void)osg;

            monitoringHandler(reviewViolation);
        }

        // Save the number of test asserts that had occurred
        const int numAsserts = testStatus;

        // "Hide" the count of the test assert that we expected to happen.
        testStatus = (testStatus > 0) ? testStatus - 1 : testStatus;

        // Stop the counter and restore the global 'testStatus'
        tsg.release();

        // Verify that only the expected number of test asserts fired.
        ASSERTL(numAsserts, 1 == numAsserts);

        ASSERTL(lcg.counter(), 1 == lcg.counter());
        // There should be exactly 1 log message observed.  Note that even if
        // 'monitor' is in verbose mode we don't want it logged it twice.
    }

    // Counters must not change for unexpected review messages
    ASSERTL(origNumHexf,    monitor.numHexfloat(),
            origNumHexf  == monitor.numHexfloat());
    ASSERTL(origNumOver,    monitor.numOverflow(),
            origNumOver  == monitor.numOverflow());
    ASSERTL(origNumUnder,   monitor.numUnderflow(),
            origNumUnder == monitor.numUnderflow());
#undef ASSERTL
}

void testBslsReviewMonitor()
    // Verifies the 'BslsReviewMonitor' mechanism.  See the calling test 'case'
    // description for concerns and plan.
{
    // Abbreviations for readable code
    using bsls::Review;
    using bsls::ReviewViolation;

    typedef Review::ViolationHandler ReviewHandler;

    // Creating a monitor replaces the review violation handler
    const ReviewHandler originalHandler = Review::violationHandler();
    {
        BslsReviewMonitor brm(veryVerbose); (void)brm;

        ASSERT(originalHandler != Review::violationHandler());
    }
    // Destroying a monitor restores the original violation handler
    ASSERT(originalHandler == Review::violationHandler());

    static const struct FilenameTestData {
        int          d_line;
        const char * d_filename_p;
        bool         d_match;
    } FILENAMES[] = {
        { L_, "/usr/local/Cellar/BBLP-BDE/src/groups/bdl/bdlb/"
                                     "BDLB_NumericParseUtil.CPP",   true  },
        { L_, "X:\\devel\\workarea\\bde\\groups\bdl\\bdlb\\"
                                         "bdlb_numericparseutil.h", true  },

        { L_, "BDLB_NUMERICPARSEUTIL.",                             true  },

        { L_, "",                                                   true  },

        { L_, 0,                                                    true  },

        { L_, "bdlb_numericparseRutil.",                            false },
        { L_, "bdlb_numericparseutil",                              false },
        { L_, "dlb_numericparseutil.",                              false },
        { L_, "bdlbnumericparseutil",                               false },
    };
    const bsl::size_t NUM_FILENAMES = sizeof FILENAMES / sizeof *FILENAMES;

    static const struct CommentTestData {
        int         d_line;
        const char *d_comment_p;
        char        d_category;  // '?' - unexpected
                                 // 'H' - hexfloat
                                 // 'O' - overflow
                                 // 'U' - underflow
    } COMMENTS[] = {
        { L_, "An hexfloat has been detected.",  'H' },
        { L_, "Hexfloat parsed by 'stR2D2'.",    'H' },
        { L_, "I think I taw an hexfloat",       'H' },
        { L_, "HeXfLoAt baby, yeah!",            'H' },
        { L_, "hExFlOaT",                        'H' },

        { L_, "An overflow has been detected.",  'O' },
        { L_, "Overflow reported by 'stR2D'.",   'O' },
        { L_, "I think I taw an overflow",       'O' },
        { L_, "oVeRfLoW baby, yeah!",            'O' },
        { L_, "OvErFlOw",                        'O' },

        { L_, "An underflow has been detected.", 'U' },
        { L_, "Underflow reported by 'stR2D'.",  'U' },
        { L_, "I think I taw an underflow",      'U' },
        { L_, "uNdErFlOw baby, yeah!",           'U' },
        { L_, "UnDeRfLoW",                       'U' },

        { L_, "Hexgroot parsed by 'stR2D2'.",    '?' },
        { L_, "An overgrow has been detected.",  '?' },
        { L_, "I think I taw an unterflow",      '?' },

        { L_, 0, '?' }
    };
    const bsl::size_t NUM_COMMENTS = sizeof COMMENTS / sizeof *COMMENTS;

    // Let's ensure we are using a log severity threshold that will show all
    // log messages so we can count them properly.
    bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_TRACE);

    // Let's ensure we are using the proper review violation handler for these
    // test so that we can count the log messages.
    Review::setViolationHandler(&Review::failByLog);

    for (int vi = 0; vi <= 1; ++vi) {
        const bool VERBOSE = (0 < vi);

        BslsReviewMonitor monitor(VERBOSE);

        // Verifying counting of expected review violations, as well as lack of
        // test assertions using the already verified test status monitor.
        // This loop skips failure cases on purpose.
        for (bsl::size_t fi = 0; fi < NUM_FILENAMES; ++fi) {
            const FilenameTestData& FILENAME = FILENAMES[fi];

            const bool MATCHING_FILENAME = FILENAME.d_match;

            for (bsl::size_t ci = 0; ci < NUM_COMMENTS; ++ci) {
                const CommentTestData& COMMENT = COMMENTS[ci];

                const bool EXPECTED_COMMENT = ('?' != COMMENT.d_category);
                const bool EXPECTED_REVIEW  =
                                         MATCHING_FILENAME && EXPECTED_COMMENT;

                if (veryVerbose) {
                    P_(FILENAME.d_line) P_(COMMENT.d_line)
                    P_(Opt(FILENAME.d_filename_p)) P_(MATCHING_FILENAME)
                    P_(Opt(COMMENT.d_comment_p))  P_(COMMENT.d_category)
                    P(EXPECTED_REVIEW);
                }

                if (EXPECTED_REVIEW) {
                    testBslsReviewMonitorCounters(monitor,
                                                  FILENAME.d_line,
                                                  FILENAME.d_filename_p,
                                                  COMMENT.d_line,
                                                  COMMENT.d_comment_p,
                                                  COMMENT.d_category);
                }
                else {
                    testBslsReviewMonitorAsserts(monitor,
                                                 FILENAME.d_line,
                                                 FILENAME.d_filename_p,
                                                 COMMENT.d_line,
                                                 COMMENT.d_comment_p);
                }
            }  // Close comments (messages) loop
        }  // Close filenames loop

        // Verifying 'resetCounters'
        {
            ASSERT(monitor.numHexfloat()  > 0);
            ASSERT(monitor.numOverflow()  > 0);
            ASSERT(monitor.numUnderflow() > 0);

            monitor.resetCounters();

            ASSERTV(monitor.numHexfloat(),  0 == monitor.numHexfloat());
            ASSERTV(monitor.numOverflow(),  0 == monitor.numOverflow());
            ASSERTV(monitor.numUnderflow(), 0 == monitor.numUnderflow());
        }
    }  // Close monitor-verbosity setting (on/off) loop
}

}  // close namespace testDouble

bool isHexHalfSubnormMin(const bsl::string_view& input)
    // Return 'true' if the specified 'input' is a signless hexfloat
    // representation of 'denorm_min()', otherwise return 'false'.
{
    typedef bsl::string_view::size_type Size;

    if (input.size() < 7) {
        // Too short
        return false;                                                 // RETURN
    }
    if (input[0] != '0' || bdlb::CharType::toUpper(input[1]) != 'X') {
        // Not hex
        return false;                                                 // RETURN
    }

    // Does it have a binary exponent?
    const bsl::string_view::size_type k_NPOS = bsl::string_view::npos;
    bsl::string_view::size_type pos = input.find_last_of("pP");
    if (k_NPOS == pos) {  // There is no exponent
        // Must have "0x."(3)"0...0"(268)"2" minimum
        if (input.size() < 272) {  // Too short
            return false;                                             // RETURN
        }

        pos = input.find('.');
        if (k_NPOS == pos) {  // There is no dot
            return false;                                             // RETURN
        }

        if (input.size() - pos < 270) {  // Too short
            return false;                                             // RETURN
        }
        if (input[++pos] != '0') {  // Nah
            return false;                                             // RETURN
        }
        const Size fracPos = pos;


        pos = input.find_last_not_of('0');
        if (k_NPOS == pos) {  // There is no significant digit
            return false;                                             // RETURN
        }

        if (input[pos] != 2) {  // Not the right number
            return false;                                             // RETURN
        }

        const Size twoPos = pos;
        if (twoPos - fracPos != 268) {  // Wrong length
            return false;                                             // RETURN
        }

        if (input.find_first_not_of('0', fracPos) != twoPos) {  // Wrong digits
            return false;                                             // RETURN
        }
        return true;                                                  // RETURN
    }

    // Has an exponent at 'pos'
    if (input.size() < pos + 6) {  // Not enough characters after 'p'
        return false;                                                 // RETURN
    }

    const Size pPos = pos++;

    const bool expNeg = ('-' ==  input[pos]);
    if (expNeg || '+' == input[pos]) {
        ++pos;  // Skip sign
    }

    pos = input.find_first_not_of('0', pos);
    if (k_NPOS == pos) {  // There is no significant exponent digit
        return false;                                                 // RETURN
    }

    if (input.size() - pos > 20) { // This won't fit into an unit64
        return false;                                                 // RETURN
    }

    Uint64 exponent;
    if (0 != Util::parseUint64(&exponent, input.substr(pos), 10)) {
        // Not a number or too large
        return false;                                                 // RETURN
    }

    // Cannot be 'npos', we have the exponent starting with 'p'
    pos = input.find_first_not_of("0.", 2);

    if ('P' == bdlb::CharType::toUpper(input[pos])) {  // No significant digits
        return false;                                                 // RETURN
    }
    if (!bdlb::CharType::isDigit(input[pos])) {  // Bad char
        return false;                                                 // RETURN
    }
    static const bsl::string_view bitXD("1248");  // 1 bit set hex digits
    if (k_NPOS == bitXD.find(input[pos])) {  // Bad digit for us
        return false;                                                 // RETURN
    }
    static const int xdigitToBitOffset[] = {
        -1, // '0' X
         0, // '1'
         1, // '2'
        -1, // '3' X
         2, // '4'
        -1, // '5' X
        -1, // '6' X
        -1, // '7' X
         3  // '8'
    };
    const unsigned xdigitOffset = xdigitToBitOffset[input[pos] - '0'];

    const Size xdigitPos = pos;

    pos = input.find('.', 2);

    const Size hasFrac = (pos != k_NPOS);

    const Size dotPos  = hasFrac ? pos        : pPos;
    const Size fracPos = hasFrac ? dotPos + 1 : pPos;

    const bool hasIntPart = !hasFrac || dotPos > xdigitPos;

    if (hasIntPart && !expNeg) {
        // Fraction has bad character or it is non-zero (only 1 bit can be set)
        return false;                                                 // RETURN
    }
    if (hasIntPart && input.find_first_not_of('0', fracPos) != pPos) {
        // Fraction has bad character or it is non-zero (only 1 bit can be set)
        return false;                                                 // RETURN
    }

    static const Size sizeMax = bsl::numeric_limits<Size>::max();

    if (hasIntPart) {
        const Size bigxShift = (dotPos - xdigitPos - 1);
        if (bigxShift > sizeMax / 4) {
            // Waaaay too large integer value
            return false;                                             // RETURN
        }

        if (exponent < 4 * bigxShift) {
            return false;                                             // RETURN
        }
        exponent -= 4 * bigxShift;
        return (exponent == xdigitOffset + 1075);                     // RETURN
    }

    // Fraction has the significant bit...
    if (pPos != input.find_first_not_of('0', xdigitPos + 1)) {
        // Garbage after the significant digit
        return false;                                                 // RETURN
    }

    const Size bigxShift = xdigitPos - 1 - dotPos;

    if (bigxShift > bsl::numeric_limits<Size>::max() / 4) {
        // Waaaay too small fraction value
        return false;                                                 // RETURN
    }

    if (expNeg) {
        if (exponent > sizeMax - 4 * bigxShift) {
            return false;                                             // RETURN
        }
        exponent += 4 * bigxShift;
    }
    else {
        // Positive exponent
        if (exponent >= 4 * bigxShift) {  // It must turn negative
            return false;                                             // RETURN
        }
        exponent = 4 * bigxShift - exponent;
        // Now it is negative
    }

    return (exponent == 1075 - (4 - xdigitOffset));
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? atoi(argv[1]) : 0;
        verbose         = argc > 2;  // These variables are global
        veryVerbose     = argc > 3;  // to be reachable from
        veryVeryVerbose = argc > 4;  // test case functions

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << '\n';

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                                  "\n=============\n";

///Example 1: Parsing an Integer Value from a 'string_view'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we have a 'string_view' that presumably contains a (not
// necessarily NUL terminated) string representing a 32-bit integer value and
// we want to convert that string into an 'int' (32-bit integer).
//
// First, we create the string:
//..
    const bsl::string_view input("2017");
//..
// Then we create the output variables for the parser:
//..
    int              year;
    bsl::string_view rest;
//..
// Next we call the parser function:
//..
    const int rv = bdlb::NumericParseUtil::parseInt(&year, &rest, input);
//..
// Then we verify the results:
//..
    ASSERT(0    == rv);
    ASSERT(2017 == year);
    ASSERT(rest.empty());
//..
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // PARSE USHORT
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //:
        //:  3 Corner cases work as expected
        //:
        //:    1 The string ends unexpectedly
        //:    2 The value is as large as representable
        //:    3 The value is just larger than representable
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   parseUshort(result, rest, input, base = 10)
        //   parseUshort(result, input, base = 10)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nPARSE USHORT"
                                  "\n============\n";

        const unsigned short INITIAL_VALUE_1 = 3;  // first initial value
        const unsigned short INITIAL_VALUE_2 = 9;  // second initial value

        static const struct {
            int            d_line;      // source line number
            const char    *d_input_p;   // specification string
            int            d_base;      // specification base
            bsl::size_t    d_offset;    // expected number of parsed chars
            bool           d_fail;      // parsing expected to fail flag
            unsigned short d_value;     // expected return value
        } DATA[] = {
            //line  input text      base offset  fail  value
            //----  --------------  ---- ------  ----  -------
            { L_,   "",               10,   0,    1,       0 },

            { L_,   "a",              10,   0,    1,       0 },
            { L_,   "+",              10,   1,    1,       0 },
            { L_,   "-",              10,   0,    1,       0 },
            { L_,   "0",              10,   1,    0,       0 },
            { L_,   "a",              16,   1,    0,      10 },
            { L_,   "f",              16,   1,    0,      15 },
            { L_,   "A",              16,   1,    0,      10 },
            { L_,   "F",              16,   1,    0,      15 },
            { L_,   "g",              16,   0,    1,       0 },
            { L_,   "0",               2,   1,    0,       0 },
            { L_,   "1",               2,   1,    0,       1 },
            { L_,   "a",              11,   1,    0,      10 },
            { L_,   "A",              11,   1,    0,      10 },
            { L_,   "z",              36,   1,    0,      35 },
            { L_,   "Z",              36,   1,    0,      35 },

            { L_,   "++",             10,   1,    1,       0 },
            { L_,   "+-",             10,   1,    1,       0 },
            { L_,   "-+",             10,   0,    1,       0 },
            { L_,   "--",             10,   0,    1,       0 },
            { L_,   "+a",             10,   1,    1,       0 },
            { L_,   "-a",             10,   0,    1,       0 },
            { L_,   "+0",             10,   2,    0,       0 },
            { L_,   "+9",             10,   2,    0,       9 },
            { L_,   "-0",             10,   0,    1,       0 },
            { L_,   "-9",             10,   0,    1,       0 },
            { L_,   "0a",             10,   1,    0,       0 },
            { L_,   "9a",             10,   1,    0,       9 },
            { L_,   "00",             10,   2,    0,       0 },
            { L_,   "01",             10,   2,    0,       1 },
            { L_,   "19",             10,   2,    0,      19 },
            { L_,   "99",             10,   2,    0,      99 },
            { L_,   "+g",             16,   1,    1,       0 },
            { L_,   "+a",             16,   2,    0,      10 },
            { L_,   "+f",             16,   2,    0,      15 },
            { L_,   "ff",             16,   2,    0,     255 },
            { L_,   "FF",             16,   2,    0,     255 },
            { L_,   "+0",              2,   2,    0,       0 },
            { L_,   "+1",              2,   2,    0,       1 },
            { L_,   "00",              2,   2,    0,       0 },
            { L_,   "01",              2,   2,    0,       1 },
            { L_,   "10",              2,   2,    0,       2 },
            { L_,   "11",              2,   2,    0,       3 },
            { L_,   "+z",             36,   2,    0,      35 },
            { L_,   "0z",             36,   2,    0,      35 },
            { L_,   "0Z",             36,   2,    0,      35 },
            { L_,   "10",             36,   2,    0,      36 },
            { L_,   "z0",             36,   2,    0,    1260 },
            { L_,   "Z0",             36,   2,    0,    1260 },

            { L_,   "+0a",            10,   2,    0,       0 },
            { L_,   "+9a",            10,   2,    0,       9 },
            { L_,   "+12",            10,   3,    0,      12 },
            { L_,   "+89",            10,   3,    0,      89 },
            { L_,   "123",            10,   3,    0,     123 },
            { L_,   "789",            10,   3,    0,     789 },
            { L_,   "+fg",            16,   2,    0,      15 },
            { L_,   "+ff",            16,   3,    0,     255 },
            { L_,   "+FF",            16,   3,    0,     255 },
            { L_,   "fff",            16,   3,    0,    4095 },
            { L_,   "fFf",            16,   3,    0,    4095 },
            { L_,   "FfF",            16,   3,    0,    4095 },
            { L_,   "FFF",            16,   3,    0,    4095 },

            { L_,   "1234",           10,   4,    0,    1234 },
            { L_,   "7FFF",           16,   4,    0,   32767 },

            { L_,   "12345",          10,   5,    0,   12345 },
            { L_,   "32766",          10,   5,    0,   32766 },
            { L_,   "32767",          10,   5,    0,   32767 },
            { L_,   "32768",          10,   5,    0,   32768 },
            { L_,   "32769",          10,   5,    0,   32769 },

            { L_,   "65535",          10,   5,    0,   65535 },
            { L_,   "65536",          10,   4,    0,    6553 },
            { L_,   "123456",         10,   5,    0,   12345 },
        };
        const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (bsl::size_t ti = 0; ti < NUM_DATA ; ++ti) {
            const int            LINE  = DATA[ti].d_line;
            const char *const    INPUT = DATA[ti].d_input_p;
            const int            BASE  = DATA[ti].d_base;
            const bsl::ptrdiff_t NUM   = DATA[ti].d_offset;
            const bool           FAIL  = DATA[ti].d_fail;
            const unsigned short VALUE = DATA[ti].d_value;

            if (veryVerbose) {
                P_(LINE) P_(INPUT) P_(BASE) P_(NUM) P_(FAIL) P(VALUE);
            }

            {  // test with first initial value
                unsigned short   result = INITIAL_VALUE_1;
                bsl::string_view rest;
                const int        rv = Util::parseUshort(&result,
                                                        &rest,
                                                        INPUT,
                                                        BASE);
                ASSERTV(LINE, NUM == rest.data() - INPUT);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_1));
            }

            {  // test with second initial value
                unsigned short   result = INITIAL_VALUE_2;
                bsl::string_view rest;
                const int        rv = Util::parseUshort(&result,
                                                        &rest,
                                                        INPUT,
                                                        BASE);
                ASSERTV(LINE, NUM == rest.data() - INPUT);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_2));
            }

            // Testing without 'remainder' argument.

            {  // test with first initial value
                unsigned short result = INITIAL_VALUE_1;
                const int      rv = Util::parseUshort(&result, INPUT, BASE);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_1));
            }

            {  // test with second initial value
                unsigned short result = INITIAL_VALUE_2;
                const int      rv = Util::parseUshort(&result, INPUT, BASE);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_2));
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // PARSE SHORT
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //:
        //:  3 Corner cases work as expected
        //:
        //:    1 The string ends unexpectedly
        //:    2 The value is as large or small as representable
        //:    3 The value is just large/small than representable
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   parseShort(result, rest, input, base = 10)
        //   parseShort(result, input, base = 10)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nPARSE SHORT"
                                  "\n===========\n";

        const short INITIAL_VALUE_1 = -3;  // first initial value
        const short INITIAL_VALUE_2 =  9;  // second initial value

        static const struct {
            int          d_line;      // source line number
            const char  *d_input_p;   // specification string
            int          d_base;      // specification base
            bsl::size_t  d_offset;    // expected number of parsed characters
            bool         d_fail;      // parsing expected to fail indicator
            short        d_value;     // expected return value
        } DATA[] = {
            //line  input text      base offset  fail  value
            //----  --------------  ---- ------  ----  -------
            { L_,   "",               10,   0,    1,       0 },

            { L_,   "a",              10,   0,    1,       0 },
            { L_,   "+",              10,   1,    1,       0 },
            { L_,   "-",              10,   1,    1,       0 },
            { L_,   "0",              10,   1,    0,       0 },
            { L_,   "a",              16,   1,    0,      10 },
            { L_,   "f",              16,   1,    0,      15 },
            { L_,   "A",              16,   1,    0,      10 },
            { L_,   "F",              16,   1,    0,      15 },
            { L_,   "g",              16,   0,    1,       0 },
            { L_,   "0",               2,   1,    0,       0 },
            { L_,   "1",               2,   1,    0,       1 },
            { L_,   "a",              11,   1,    0,      10 },
            { L_,   "A",              11,   1,    0,      10 },
            { L_,   "z",              36,   1,    0,      35 },
            { L_,   "Z",              36,   1,    0,      35 },

            { L_,   "++",             10,   1,    1,       0 },
            { L_,   "+-",             10,   1,    1,       0 },
            { L_,   "-+",             10,   1,    1,       0 },
            { L_,   "--",             10,   1,    1,       0 },
            { L_,   "+a",             10,   1,    1,       0 },
            { L_,   "-a",             10,   1,    1,       0 },
            { L_,   "+0",             10,   2,    0,       0 },
            { L_,   "+9",             10,   2,    0,       9 },
            { L_,   "-0",             10,   2,    0,       0 },
            { L_,   "-9",             10,   2,    0,      -9 },
            { L_,   "0a",             10,   1,    0,       0 },
            { L_,   "9a",             10,   1,    0,       9 },
            { L_,   "00",             10,   2,    0,       0 },
            { L_,   "01",             10,   2,    0,       1 },
            { L_,   "19",             10,   2,    0,      19 },
            { L_,   "99",             10,   2,    0,      99 },
            { L_,   "+g",             16,   1,    1,       0 },
            { L_,   "-g",             16,   1,    1,       0 },
            { L_,   "+a",             16,   2,    0,      10 },
            { L_,   "+f",             16,   2,    0,      15 },
            { L_,   "-a",             16,   2,    0,     -10 },
            { L_,   "-f",             16,   2,    0,     -15 },
            { L_,   "ff",             16,   2,    0,     255 },
            { L_,   "FF",             16,   2,    0,     255 },
            { L_,   "+0",              2,   2,    0,       0 },
            { L_,   "-0",              2,   2,    0,       0 },
            { L_,   "+1",              2,   2,    0,       1 },
            { L_,   "-1",              2,   2,    0,      -1 },
            { L_,   "00",              2,   2,    0,       0 },
            { L_,   "01",              2,   2,    0,       1 },
            { L_,   "10",              2,   2,    0,       2 },
            { L_,   "11",              2,   2,    0,       3 },
            { L_,   "+z",             36,   2,    0,      35 },
            { L_,   "-z",             36,   2,    0,     -35 },
            { L_,   "0z",             36,   2,    0,      35 },
            { L_,   "0Z",             36,   2,    0,      35 },
            { L_,   "10",             36,   2,    0,      36 },
            { L_,   "z0",             36,   2,    0,    1260 },
            { L_,   "Z0",             36,   2,    0,    1260 },

            { L_,   "+0a",            10,   2,    0,       0 },
            { L_,   "+9a",            10,   2,    0,       9 },
            { L_,   "-0a",            10,   2,    0,       0 },
            { L_,   "-9a",            10,   2,    0,      -9 },
            { L_,   "+12",            10,   3,    0,      12 },
            { L_,   "+89",            10,   3,    0,      89 },
            { L_,   "-12",            10,   3,    0,     -12 },
            { L_,   "123",            10,   3,    0,     123 },
            { L_,   "789",            10,   3,    0,     789 },
            { L_,   "+fg",            16,   2,    0,      15 },
            { L_,   "-fg",            16,   2,    0,     -15 },
            { L_,   "+ff",            16,   3,    0,     255 },
            { L_,   "+FF",            16,   3,    0,     255 },
            { L_,   "-ff",            16,   3,    0,    -255 },
            { L_,   "-FF",            16,   3,    0,    -255 },
            { L_,   "fff",            16,   3,    0,    4095 },
            { L_,   "fFf",            16,   3,    0,    4095 },
            { L_,   "FfF",            16,   3,    0,    4095 },
            { L_,   "FFF",            16,   3,    0,    4095 },

            { L_,   "1234",           10,   4,    0,    1234 },
            { L_,   "-123",           10,   4,    0,    -123 },
            { L_,   "7FFF",           16,   4,    0,   32767 },

            { L_,   "12345",          10,   5,    0,   12345 },
            { L_,   "-1234",          10,   5,    0,   -1234 },
            { L_,   "-7FFF",          16,   5,    0,  -32767 },
            { L_,   "32766",          10,   5,    0,   32766 },
            { L_,   "32767",          10,   5,    0,   32767 },
            { L_,   "32768",          10,   4,    0,    3276 },
            { L_,   "32769",          10,   4,    0,    3276 },
            { L_,   "-8000",          16,   5,    0,  -32768 },

            { L_,   "123456",         10,   5,    0,   12345 },
            { L_,   "-32766",         10,   6,    0,  -32766 },
            { L_,   "-32767",         10,   6,    0,  -32767 },
            { L_,   "-32768",         10,   6,    0,  -32768 },
            { L_,   "-32769",         10,   5,    0,   -3276 },

            { L_,   "-123456",        10,   6,    0,  -12345 }
        };
        const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (bsl::size_t ti = 0; ti < NUM_DATA ; ++ti) {
            const int            LINE  = DATA[ti].d_line;
            const char *const    INPUT = DATA[ti].d_input_p;
            const int            BASE  = DATA[ti].d_base;
            const bsl::ptrdiff_t NUM   = DATA[ti].d_offset;
            const bool           FAIL  = DATA[ti].d_fail;
            const short          VALUE = DATA[ti].d_value;

            if (veryVerbose) {
                P_(LINE) P_(INPUT) P_(BASE) P_(NUM) P_(FAIL) P(VALUE);
            }

            {  // test with first initial value
                short       result = INITIAL_VALUE_1;
                bsl::string_view rest;
                const int   rv = Util::parseShort(&result,
                                                  &rest,
                                                  INPUT,
                                                  BASE);
                ASSERTV(LINE, NUM == rest.data() - INPUT);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_1));
            }

            {  // test with second initial value
                short       result = INITIAL_VALUE_2;
                bsl::string_view rest;
                const int   rv = Util::parseShort(&result,
                                                  &rest,
                                                  INPUT,
                                                  BASE);
                ASSERTV(LINE, NUM == rest.data() - INPUT);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_2));
            }

            // Test without the 'remainder' argument

            {  // test with first initial value
                short     result = INITIAL_VALUE_1;
                const int rv     = Util::parseShort(&result, INPUT, BASE);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_1));
            }

            {  // test with second initial value
                short     result = INITIAL_VALUE_2;
                const int rv     = Util::parseShort(&result, INPUT, BASE);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_2));
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // PARSE UINT64
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //:
        //:  3 Corner cases work as expected
        //:
        //:    1 The string ends unexpectedly
        //:    2 The value is as large or small as representable
        //:    3 The value is just large/small than representable
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   parseUint64(result, rest, input, base = 10)
        //   parseUint64(result, input, base = 10)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nPARSE UINT64"
                                  "\n============\n";

        const Uint64 INITIAL_VALUE_1 = static_cast<Uint64>(-3);
        const Uint64 INITIAL_VALUE_2 = static_cast<Uint64>( 9);

        static const struct {
            int          d_line;      // source line number
            const char  *d_input_p;   // specification string
            int          d_base;      // specification base
            bsl::size_t  d_offset;    // expected number of parsed characters
            bool         d_fail;      // parsing expected to fail indicator
            Uint64       d_value;     // expected return value
        } DATA[] = {
            //line  input text      base offset  fail  value
            //----  --------------  ---- ------  ----  -------
            { L_,   "",               10,   0,    1,       0 },

            { L_,   "a",              10,   0,    1,       0 },
            { L_,   "+",              10,   1,    1,       0 },
            { L_,   "-",              10,   0,    1,       0 },
            { L_,   "0",              10,   1,    0,       0 },
            { L_,   "a",              16,   1,    0,      10 },
            { L_,   "f",              16,   1,    0,      15 },
            { L_,   "A",              16,   1,    0,      10 },
            { L_,   "F",              16,   1,    0,      15 },
            { L_,   "g",              16,   0,    1,       0 },
            { L_,   "0",               2,   1,    0,       0 },
            { L_,   "1",               2,   1,    0,       1 },
            { L_,   "a",              11,   1,    0,      10 },
            { L_,   "A",              11,   1,    0,      10 },
            { L_,   "z",              36,   1,    0,      35 },
            { L_,   "Z",              36,   1,    0,      35 },

            { L_,   "++",             10,   1,    1,       0 },
            { L_,   "+-",             10,   1,    1,       0 },
            { L_,   "-+",             10,   0,    1,       0 },
            { L_,   "--",             10,   0,    1,       0 },
            { L_,   "+a",             10,   1,    1,       0 },
            { L_,   "+0",             10,   2,    0,       0 },
            { L_,   "+9",             10,   2,    0,       9 },
            { L_,   "-0",             10,   0,    1,       0 },
            { L_,   "-9",             10,   0,    1,       0 },
            { L_,   "0a",             10,   1,    0,       0 },
            { L_,   "9a",             10,   1,    0,       9 },
            { L_,   "00",             10,   2,    0,       0 },
            { L_,   "01",             10,   2,    0,       1 },
            { L_,   "19",             10,   2,    0,      19 },
            { L_,   "99",             10,   2,    0,      99 },
            { L_,   "+g",             16,   1,    1,       0 },
            { L_,   "-g",             16,   0,    1,       0 },
            { L_,   "+a",             16,   2,    0,      10 },
            { L_,   "+f",             16,   2,    0,      15 },
            { L_,   "ff",             16,   2,    0,     255 },
            { L_,   "FF",             16,   2,    0,     255 },
            { L_,   "+0",              2,   2,    0,       0 },
            { L_,   "+1",              2,   2,    0,       1 },
            { L_,   "00",              2,   2,    0,       0 },
            { L_,   "01",              2,   2,    0,       1 },
            { L_,   "10",              2,   2,    0,       2 },
            { L_,   "11",              2,   2,    0,       3 },
            { L_,   "+z",             36,   2,    0,      35 },
            { L_,   "0z",             36,   2,    0,      35 },
            { L_,   "0Z",             36,   2,    0,      35 },
            { L_,   "10",             36,   2,    0,      36 },
            { L_,   "z0",             36,   2,    0,    1260 },
            { L_,   "Z0",             36,   2,    0,    1260 },

            { L_,   "+0a",            10,   2,    0,       0 },
            { L_,   "+9a",            10,   2,    0,       9 },
            { L_,   "+12",            10,   3,    0,      12 },
            { L_,   "+89",            10,   3,    0,      89 },
            { L_,   "123",            10,   3,    0,     123 },
            { L_,   "789",            10,   3,    0,     789 },
            { L_,   "+fg",            16,   2,    0,      15 },
            { L_,   "+ff",            16,   3,    0,     255 },
            { L_,   "+FF",            16,   3,    0,     255 },
            { L_,   "fff",            16,   3,    0,    4095 },
            { L_,   "fFf",            16,   3,    0,    4095 },
            { L_,   "FfF",            16,   3,    0,    4095 },
            { L_,   "FFF",            16,   3,    0,    4095 },

            { L_,   "1234",           10,   4,    0,    1234 },
            { L_,   "7FFF",           16,   4,    0,   32767 },

            { L_,   "12345",          10,   5,    0,   12345 },
            { L_,   "32766",          10,   5,    0,   32766 },
            { L_,   "32767",          10,   5,    0,   32767 },
            { L_,   "32768",          10,   5,    0,   32768 },
            { L_,   "32769",          10,   5,    0,   32769 },

            { L_,   "123456",         10,   6,    0,  123456 },

            { L_,   "2147483647",     10,  10,    0,  2147483647 },
            { L_,   "2147483648",     10,  10,    0,  2147483648LL },
            { L_,   "2147483649",     10,  10,    0,  2147483649LL },

            { L_,   "ffffffffffffffff",  16,  16,    0,
                                                0xFFFFFFFFFFFFFFFFuLL },
            { L_,   "8000000000000000",  16,  16,    0,
                                                0x8000000000000000uLL },

            { L_,   "18446744073709551615", 10,  20,    0,
                                            18446744073709551615ull },
        };
        const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (bsl::size_t ti = 0; ti < NUM_DATA ; ++ti) {
            const int            LINE  = DATA[ti].d_line;
            const char *const    INPUT = DATA[ti].d_input_p;
            const int            BASE  = DATA[ti].d_base;
            const bsl::ptrdiff_t NUM   = DATA[ti].d_offset;
            const bool           FAIL  = DATA[ti].d_fail;
            const Uint64         VALUE = DATA[ti].d_value;

            if (veryVerbose) {
                P_(LINE) P_(INPUT) P_(BASE) P_(NUM) P_(FAIL) P(VALUE);
            }

            {  // test with first initial value
                Uint64      result = INITIAL_VALUE_1;
                bsl::string_view rest;
                const int   rv = Util::parseUint64(&result,
                                                   &rest,
                                                   INPUT,
                                                   BASE);
                ASSERTV(LINE, NUM == rest.data() - INPUT);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_1));
            }

            {  // test with second initial value
                Uint64      result = INITIAL_VALUE_2;
                bsl::string_view rest;
                const int   rv = Util::parseUint64(&result,
                                                   &rest,
                                                   INPUT,
                                                   BASE);
                ASSERTV(LINE, NUM == rest.data() - INPUT);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_2));
            }

            // Test without the 'remainder' argument

            {  // test with first initial value
                Uint64    result = INITIAL_VALUE_1;
                const int rv     = Util::parseUint64(&result, INPUT, BASE);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_1));
            }

            {  // test with second initial value
                Uint64    result = INITIAL_VALUE_2;
                const int rv     = Util::parseUint64(&result, INPUT, BASE);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_2));
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // PARSE UINT
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //:
        //:  3 Corner cases work as expected
        //:
        //:    1 The string ends unexpectedly
        //:    2 The value is as large or small as representable
        //:    3 The value is just large/small than representable
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   parseUint(result, rest, input, base = 10)
        //   parseUint(result, input, base = 10)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nPARSE UINT"
                                  "\n==========\n";

        const unsigned INITIAL_VALUE_1 = static_cast<unsigned>(-3);
        const unsigned INITIAL_VALUE_2 = static_cast<unsigned>( 9);

        static const struct {
            int          d_line;      // source line number
            const char  *d_input_p;   // specification string
            int          d_base;      // specification base
            bsl::size_t  d_offset;    // expected number of parsed characters
            bool         d_fail;      // parsing expected to fail indicator
            unsigned     d_value;     // expected return value
        } DATA[] = {
            //line  input text      base offset  fail  value
            //----  --------------  ---- ------  ----  ---------
            { L_,   "",               10,   0,    1,          0U },

            { L_,   "a",              10,   0,    1,          0U },
            { L_,   "+",              10,   1,    1,          0U },
            { L_,   "-",              10,   0,    1,          0U },
            { L_,   "0",              10,   1,    0,          0U },
            { L_,   "a",              16,   1,    0,         10U },
            { L_,   "f",              16,   1,    0,         15U },
            { L_,   "A",              16,   1,    0,         10U },
            { L_,   "F",              16,   1,    0,         15U },
            { L_,   "g",              16,   0,    1,          0U },
            { L_,   "0",               2,   1,    0,          0U },
            { L_,   "1",               2,   1,    0,          1U },
            { L_,   "a",              11,   1,    0,         10U },
            { L_,   "A",              11,   1,    0,         10U },
            { L_,   "z",              36,   1,    0,         35U },
            { L_,   "Z",              36,   1,    0,         35U },

            { L_,   "++",             10,   1,    1,          0U },
            { L_,   "+-",             10,   1,    1,          0U },
            { L_,   "-+",             10,   0,    1,          0U },
            { L_,   "--",             10,   0,    1,          0U },
            { L_,   "+a",             10,   1,    1,          0U },
            { L_,   "-a",             10,   0,    1,          0U },
            { L_,   "+0",             10,   2,    0,          0U },
            { L_,   "+9",             10,   2,    0,          9U },
            { L_,   "-0",             10,   0,    1,          0U },
            { L_,   "0a",             10,   1,    0,          0U },
            { L_,   "9a",             10,   1,    0,          9U },
            { L_,   "00",             10,   2,    0,          0U },
            { L_,   "01",             10,   2,    0,          1U },
            { L_,   "19",             10,   2,    0,         19U },
            { L_,   "99",             10,   2,    0,         99U },
            { L_,   "+g",             16,   1,    1,          0U },
            { L_,   "+a",             16,   2,    0,         10U },
            { L_,   "+f",             16,   2,    0,         15U },
            { L_,   "ff",             16,   2,    0,        255U },
            { L_,   "FF",             16,   2,    0,        255U },
            { L_,   "+0",              2,   2,    0,          0U },
            { L_,   "+1",              2,   2,    0,          1U },
            { L_,   "00",              2,   2,    0,          0U },
            { L_,   "01",              2,   2,    0,          1U },
            { L_,   "10",              2,   2,    0,          2U },
            { L_,   "11",              2,   2,    0,          3U },
            { L_,   "+z",             36,   2,    0,         35U },
            { L_,   "0z",             36,   2,    0,         35U },
            { L_,   "0Z",             36,   2,    0,         35U },
            { L_,   "10",             36,   2,    0,         36U },
            { L_,   "z0",             36,   2,    0,       1260U },
            { L_,   "Z0",             36,   2,    0,       1260U },

            { L_,   "+0a",            10,   2,    0,          0U },
            { L_,   "+9a",            10,   2,    0,          9U },
            { L_,   "+12",            10,   3,    0,         12U },
            { L_,   "+89",            10,   3,    0,         89U },
            { L_,   "-12",            10,   0,    1,          0U },
            { L_,   "123",            10,   3,    0,        123U },
            { L_,   "789",            10,   3,    0,        789U },
            { L_,   "+fg",            16,   2,    0,         15U },
            { L_,   "+ff",            16,   3,    0,        255U },
            { L_,   "+FF",            16,   3,    0,        255U },
            { L_,   "fff",            16,   3,    0,       4095U },
            { L_,   "fFf",            16,   3,    0,       4095U },
            { L_,   "FfF",            16,   3,    0,       4095U },
            { L_,   "FFF",            16,   3,    0,       4095U },

            { L_,   "1234",           10,   4,    0,       1234U },
            { L_,   "7FFF",           16,   4,    0,      32767U },

            { L_,   "12345",          10,   5,    0,      12345U },
            { L_,   "32766",          10,   5,    0,      32766U },
            { L_,   "32767",          10,   5,    0,      32767U },
            { L_,   "32768",          10,   5,    0,      32768U },
            { L_,   "32769",          10,   5,    0,      32769U },

            { L_,   "123456",         10,   6,    0,     123456U },

            { L_,   "2147483647",     10,  10,    0,  2147483647U },
            { L_,   "2147483648",     10,  10,    0,  2147483648U },
            { L_,   "2147483649",     10,  10,    0,  2147483649U },

            { L_,   "4294967295",     10,  10,    0,  4294967295U },
            { L_,   "4294967296",     10,   9,    0,   429496729U },
        };
        const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (bsl::size_t ti = 0; ti < NUM_DATA ; ++ti) {
            const int            LINE  = DATA[ti].d_line;
            const char *const    INPUT = DATA[ti].d_input_p;
            const int            BASE  = DATA[ti].d_base;
            const bsl::ptrdiff_t NUM   = DATA[ti].d_offset;
            const bool           FAIL  = DATA[ti].d_fail;
            const unsigned       VALUE = DATA[ti].d_value;

            if (veryVerbose) {
                P_(LINE) P_(INPUT) P_(BASE) P_(NUM) P_(FAIL) P(VALUE);
            }

            {  // test with first initial value
                unsigned    result = INITIAL_VALUE_1;
                bsl::string_view rest;
                const int   rv = Util::parseUint(&result,
                                                 &rest,
                                                 INPUT,
                                                 BASE);
                ASSERTV(LINE, NUM == rest.data() - INPUT);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_1));
            }

            {  // test with second initial value
                unsigned    result = INITIAL_VALUE_2;
                bsl::string_view rest;
                const int   rv = Util::parseUint(&result,
                                                 &rest,
                                                 INPUT,
                                                 BASE);
                ASSERTV(LINE, NUM == rest.data() - INPUT);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_2));
            }

            // Test without the 'remainder' argument

            {  // test with first initial value
                unsigned  result = INITIAL_VALUE_1;
                const int rv     = Util::parseUint(&result, INPUT, BASE);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_1));
            }

            {  // test with second initial value
                unsigned  result = INITIAL_VALUE_2;
                const int rv     = Util::parseUint(&result, INPUT, BASE);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_2));
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // PARSE INT64
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //:
        //:  3 Corner cases work as expected
        //:
        //:    1 The string ends unexpectedly
        //:    2 The value is as large or small as representable
        //:    3 The value is just large/small than representable
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   parseInt64(result, rest, input, base = 10)
        //   parseInt64(result, input, base = 10)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nPARSE INT64"
                                  "\n===========\n";

        const Int64 INITIAL_VALUE_1 = -3;  // first initial value
        const Int64 INITIAL_VALUE_2 =  9;  // second initial value

        static const struct {
            int          d_line;      // source line number
            const char  *d_input_p;   // specification string
            int          d_base;      // specification base
            bsl::size_t  d_offset;    // expected number of parsed characters
            bool         d_fail;      // parsing expected to fail indicator
            Int64        d_value;     // expected return value
        } DATA[] = {
            //line  input text      base offset  fail  value
            //----  --------------  ---- ------  ----  -------
            { L_,   "",               10,   0,    1,       0 },

            { L_,   "a",              10,   0,    1,       0 },
            { L_,   "+",              10,   1,    1,       0 },
            { L_,   "-",              10,   1,    1,       0 },
            { L_,   "0",              10,   1,    0,       0 },
            { L_,   "a",              16,   1,    0,      10 },
            { L_,   "f",              16,   1,    0,      15 },
            { L_,   "A",              16,   1,    0,      10 },
            { L_,   "F",              16,   1,    0,      15 },
            { L_,   "g",              16,   0,    1,       0 },
            { L_,   "0",               2,   1,    0,       0 },
            { L_,   "1",               2,   1,    0,       1 },
            { L_,   "a",              11,   1,    0,      10 },
            { L_,   "A",              11,   1,    0,      10 },
            { L_,   "z",              36,   1,    0,      35 },
            { L_,   "Z",              36,   1,    0,      35 },

            { L_,   "++",             10,   1,    1,       0 },
            { L_,   "+-",             10,   1,    1,       0 },
            { L_,   "-+",             10,   1,    1,       0 },
            { L_,   "--",             10,   1,    1,       0 },
            { L_,   "+a",             10,   1,    1,       0 },
            { L_,   "-a",             10,   1,    1,       0 },
            { L_,   "+0",             10,   2,    0,       0 },
            { L_,   "+9",             10,   2,    0,       9 },
            { L_,   "-0",             10,   2,    0,       0 },
            { L_,   "-9",             10,   2,    0,      -9 },
            { L_,   "0a",             10,   1,    0,       0 },
            { L_,   "9a",             10,   1,    0,       9 },
            { L_,   "00",             10,   2,    0,       0 },
            { L_,   "01",             10,   2,    0,       1 },
            { L_,   "19",             10,   2,    0,      19 },
            { L_,   "99",             10,   2,    0,      99 },
            { L_,   "+g",             16,   1,    1,       0 },
            { L_,   "-g",             16,   1,    1,       0 },
            { L_,   "+a",             16,   2,    0,      10 },
            { L_,   "+f",             16,   2,    0,      15 },
            { L_,   "-a",             16,   2,    0,     -10 },
            { L_,   "-f",             16,   2,    0,     -15 },
            { L_,   "ff",             16,   2,    0,     255 },
            { L_,   "FF",             16,   2,    0,     255 },
            { L_,   "+0",              2,   2,    0,       0 },
            { L_,   "-0",              2,   2,    0,       0 },
            { L_,   "+1",              2,   2,    0,       1 },
            { L_,   "-1",              2,   2,    0,      -1 },
            { L_,   "00",              2,   2,    0,       0 },
            { L_,   "01",              2,   2,    0,       1 },
            { L_,   "10",              2,   2,    0,       2 },
            { L_,   "11",              2,   2,    0,       3 },
            { L_,   "+z",             36,   2,    0,      35 },
            { L_,   "-z",             36,   2,    0,     -35 },
            { L_,   "0z",             36,   2,    0,      35 },
            { L_,   "0Z",             36,   2,    0,      35 },
            { L_,   "10",             36,   2,    0,      36 },
            { L_,   "z0",             36,   2,    0,    1260 },
            { L_,   "Z0",             36,   2,    0,    1260 },

            { L_,   "+0a",            10,   2,    0,       0 },
            { L_,   "+9a",            10,   2,    0,       9 },
            { L_,   "-0a",            10,   2,    0,       0 },
            { L_,   "-9a",            10,   2,    0,      -9 },
            { L_,   "+12",            10,   3,    0,      12 },
            { L_,   "+89",            10,   3,    0,      89 },
            { L_,   "-12",            10,   3,    0,     -12 },
            { L_,   "123",            10,   3,    0,     123 },
            { L_,   "789",            10,   3,    0,     789 },
            { L_,   "+fg",            16,   2,    0,      15 },
            { L_,   "-fg",            16,   2,    0,     -15 },
            { L_,   "+ff",            16,   3,    0,     255 },
            { L_,   "+FF",            16,   3,    0,     255 },
            { L_,   "-ff",            16,   3,    0,    -255 },
            { L_,   "-FF",            16,   3,    0,    -255 },
            { L_,   "fff",            16,   3,    0,    4095 },
            { L_,   "fFf",            16,   3,    0,    4095 },
            { L_,   "FfF",            16,   3,    0,    4095 },
            { L_,   "FFF",            16,   3,    0,    4095 },

            { L_,   "1234",           10,   4,    0,    1234 },
            { L_,   "-123",           10,   4,    0,    -123 },
            { L_,   "7FFF",           16,   4,    0,   32767 },

            { L_,   "12345",          10,   5,    0,   12345 },
            { L_,   "-1234",          10,   5,    0,   -1234 },
            { L_,   "-7FFF",          16,   5,    0,  -32767 },
            { L_,   "32766",          10,   5,    0,   32766 },
            { L_,   "32767",          10,   5,    0,   32767 },
            { L_,   "32768",          10,   5,    0,   32768 },
            { L_,   "32769",          10,   5,    0,   32769 },
            { L_,   "-8000",          16,   5,    0,  -32768 },

            { L_,   "123456",         10,   6,    0,  123456 },
            { L_,   "-32766",         10,   6,    0,  -32766 },
            { L_,   "-32767",         10,   6,    0,  -32767 },
            { L_,   "-32768",         10,   6,    0,  -32768 },
            { L_,   "-32769",         10,   6,    0,  -32769 },

            { L_,   "-123456",        10,   7,    0, -123456 },

            { L_,   "2147483647",     10,  10,    0,  2147483647 },
            { L_,   "2147483648",     10,  10,    0,  2147483648LL },
            { L_,   "2147483649",     10,  10,    0,  2147483649LL },

            { L_,   "-2147483647",    10,  11,    0, -2147483647LL },
            { L_,   "-2147483648",    10,  11,    0, -2147483648LL },
            { L_,   "-2147483649",    10,  11,    0, -2147483649LL },

            { L_,   "7fffffffffffffff",  16,  16,    0,
                                                    0x7FFFFFFFFFFFFFFFLL },
            { L_,   "8000000000000000",  16,  15,    0,
                                                        0x800000000000000LL },

            { L_,   "-7FFFFFFFFFFFFFFF", 16,  17,    0,
                                                    -0x7FFFFFFFFFFFFFFFLL },

            { L_,   "9223372036854775807", 10, 19, 0,
                                                    9223372036854775807ll },

            { L_,   "-9223372036854775807", 10, 20, 0,
                                                    -9223372036854775807ll },

            { L_,   "-9223372036854775809", 10, 19, 0,
                                                    -922337203685477580ll },
        };
        const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (bsl::size_t ti = 0; ti < NUM_DATA ; ++ti) {
            const int            LINE  = DATA[ti].d_line;
            const char *const    INPUT = DATA[ti].d_input_p;
            const int            BASE  = DATA[ti].d_base;
            const bsl::ptrdiff_t NUM   = DATA[ti].d_offset;
            const bool           FAIL  = DATA[ti].d_fail;
            const Int64          VALUE = DATA[ti].d_value;

            if (veryVerbose) {
                P_(LINE) P_(INPUT) P_(BASE) P_(NUM) P_(FAIL) P(VALUE);
            }

            {  // test with first initial value
                Int64       result = INITIAL_VALUE_1;
                bsl::string_view rest;
                const int   rv = Util::parseInt64(&result,
                                                  &rest,
                                                  INPUT,
                                                  BASE);
                ASSERTV(LINE, NUM == rest.data() - INPUT);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_1));
            }

            {  // test with second initial value
                Int64       result = INITIAL_VALUE_2;
                bsl::string_view rest;
                const int   rv = Util::parseInt64(&result,
                                                  &rest,
                                                  INPUT,
                                                  BASE);
                ASSERTV(LINE, NUM == rest.data() - INPUT);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_2));
            }

            // Test without the 'remainder' argument

            {  // test with first initial value
                Int64     result = INITIAL_VALUE_1;
                const int rv     = Util::parseInt64(&result, INPUT, BASE);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_1));
            }

            {  // test with second initial value
                Int64     result = INITIAL_VALUE_2;
                const int rv     = Util::parseInt64(&result, INPUT, BASE);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_2));
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // PARSE INT
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //:
        //:  3 Corner cases work as expected
        //:
        //:    1 The string ends unexpectedly
        //:    2 The value is as large or small as representable
        //:    3 The value is just large/small than representable
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   parseInt(result, rest, input, base = 10)
        //   parseInt(result, input, base = 10)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nPARSE INT"
                                  "\n=========\n";

        const int INITIAL_VALUE_1 = -3;  // first initial value
        const int INITIAL_VALUE_2 =  9;  // second initial value

        static const struct {
            int          d_line;      // source line number
            const char  *d_input_p;   // specification string
            int          d_base;      // specification base
            bsl::size_t  d_offset;    // expected number of parsed characters
            bool         d_fail;      // parsing expected to fail indicator
            int          d_value;     // expected return value
        } DATA[] = {
            //line  input text      base offset  fail  value
            //----  --------------  ---- ------  ----  -------
            { L_,   "",               10,   0,    1,       0 },

            { L_,   "a",              10,   0,    1,       0 },
            { L_,   "+",              10,   1,    1,       0 },
            { L_,   "-",              10,   1,    1,       0 },
            { L_,   "0",              10,   1,    0,       0 },
            { L_,   "a",              16,   1,    0,      10 },
            { L_,   "f",              16,   1,    0,      15 },
            { L_,   "A",              16,   1,    0,      10 },
            { L_,   "F",              16,   1,    0,      15 },
            { L_,   "g",              16,   0,    1,       0 },
            { L_,   "0",               2,   1,    0,       0 },
            { L_,   "1",               2,   1,    0,       1 },
            { L_,   "a",              11,   1,    0,      10 },
            { L_,   "A",              11,   1,    0,      10 },
            { L_,   "z",              36,   1,    0,      35 },
            { L_,   "Z",              36,   1,    0,      35 },

            { L_,   "++",             10,   1,    1,       0 },
            { L_,   "+-",             10,   1,    1,       0 },
            { L_,   "-+",             10,   1,    1,       0 },
            { L_,   "--",             10,   1,    1,       0 },
            { L_,   "+a",             10,   1,    1,       0 },
            { L_,   "-a",             10,   1,    1,       0 },
            { L_,   "+0",             10,   2,    0,       0 },
            { L_,   "+9",             10,   2,    0,       9 },
            { L_,   "-0",             10,   2,    0,       0 },
            { L_,   "-9",             10,   2,    0,      -9 },
            { L_,   "0a",             10,   1,    0,       0 },
            { L_,   "9a",             10,   1,    0,       9 },
            { L_,   "00",             10,   2,    0,       0 },
            { L_,   "01",             10,   2,    0,       1 },
            { L_,   "19",             10,   2,    0,      19 },
            { L_,   "99",             10,   2,    0,      99 },
            { L_,   "+g",             16,   1,    1,       0 },
            { L_,   "-g",             16,   1,    1,       0 },
            { L_,   "+a",             16,   2,    0,      10 },
            { L_,   "+f",             16,   2,    0,      15 },
            { L_,   "-a",             16,   2,    0,     -10 },
            { L_,   "-f",             16,   2,    0,     -15 },
            { L_,   "ff",             16,   2,    0,     255 },
            { L_,   "FF",             16,   2,    0,     255 },
            { L_,   "+0",              2,   2,    0,       0 },
            { L_,   "-0",              2,   2,    0,       0 },
            { L_,   "+1",              2,   2,    0,       1 },
            { L_,   "-1",              2,   2,    0,      -1 },
            { L_,   "00",              2,   2,    0,       0 },
            { L_,   "01",              2,   2,    0,       1 },
            { L_,   "10",              2,   2,    0,       2 },
            { L_,   "11",              2,   2,    0,       3 },
            { L_,   "+z",             36,   2,    0,      35 },
            { L_,   "-z",             36,   2,    0,     -35 },
            { L_,   "0z",             36,   2,    0,      35 },
            { L_,   "0Z",             36,   2,    0,      35 },
            { L_,   "10",             36,   2,    0,      36 },
            { L_,   "z0",             36,   2,    0,    1260 },
            { L_,   "Z0",             36,   2,    0,    1260 },

            { L_,   "+0a",            10,   2,    0,       0 },
            { L_,   "+9a",            10,   2,    0,       9 },
            { L_,   "-0a",            10,   2,    0,       0 },
            { L_,   "-9a",            10,   2,    0,      -9 },
            { L_,   "+12",            10,   3,    0,      12 },
            { L_,   "+89",            10,   3,    0,      89 },
            { L_,   "-12",            10,   3,    0,     -12 },
            { L_,   "123",            10,   3,    0,     123 },
            { L_,   "789",            10,   3,    0,     789 },
            { L_,   "+fg",            16,   2,    0,      15 },
            { L_,   "-fg",            16,   2,    0,     -15 },
            { L_,   "+ff",            16,   3,    0,     255 },
            { L_,   "+FF",            16,   3,    0,     255 },
            { L_,   "-ff",            16,   3,    0,    -255 },
            { L_,   "-FF",            16,   3,    0,    -255 },
            { L_,   "fff",            16,   3,    0,    4095 },
            { L_,   "fFf",            16,   3,    0,    4095 },
            { L_,   "FfF",            16,   3,    0,    4095 },
            { L_,   "FFF",            16,   3,    0,    4095 },

            { L_,   "1234",           10,   4,    0,    1234 },
            { L_,   "-123",           10,   4,    0,    -123 },
            { L_,   "7FFF",           16,   4,    0,   32767 },

            { L_,   "12345",          10,   5,    0,   12345 },
            { L_,   "-1234",          10,   5,    0,   -1234 },
            { L_,   "-7FFF",          16,   5,    0,  -32767 },
            { L_,   "32766",          10,   5,    0,   32766 },
            { L_,   "32767",          10,   5,    0,   32767 },
            { L_,   "32768",          10,   5,    0,   32768 },
            { L_,   "32769",          10,   5,    0,   32769 },
            { L_,   "-8000",          16,   5,    0,  -32768 },

            { L_,   "123456",         10,   6,    0,  123456 },
            { L_,   "-32766",         10,   6,    0,  -32766 },
            { L_,   "-32767",         10,   6,    0,  -32767 },
            { L_,   "-32768",         10,   6,    0,  -32768 },
            { L_,   "-32769",         10,   6,    0,  -32769 },

            { L_,   "-123456",        10,   7,    0, -123456 },

            { L_,   "2147483647",     10,  10,    0,  2147483647 },
            { L_,   "2147483648",     10,   9,    0,   214748364 },
            { L_,   "2147483649",     10,   9,    0,   214748364 },

            { L_,   "-2147483647",    10,  11,    0, -2147483647 },
            { L_,   "-2147483648",    10,  11,    0, -2147483648LL },
            { L_,   "-2147483649",    10,  10,    0,  -214748364 },
        };
        const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (bsl::size_t ti = 0; ti < NUM_DATA ; ++ti) {
            const int            LINE  = DATA[ti].d_line;
            const char *const    INPUT = DATA[ti].d_input_p;
            const int            BASE  = DATA[ti].d_base;
            const bsl::ptrdiff_t NUM   = DATA[ti].d_offset;
            const bool           FAIL  = DATA[ti].d_fail;
            const int            VALUE = DATA[ti].d_value;

            if (veryVerbose) {
                P_(LINE) P_(INPUT) P_(BASE) P_(NUM) P_(FAIL) P(VALUE);
            }

            {  // test with first initial value
                int         result = INITIAL_VALUE_1;
                bsl::string_view rest;
                const int   rv = Util::parseInt(&result,
                                                &rest,
                                                INPUT,
                                                BASE);
                ASSERTV(LINE, NUM == rest.data() - INPUT);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_1));
            }

            {  // test with second initial value
                int         result = INITIAL_VALUE_2;
                bsl::string_view rest;
                const int   rv = Util::parseInt(&result,
                                                &rest,
                                                INPUT,
                                                BASE);
                ASSERTV(LINE, NUM == rest.data() - INPUT);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_2));
            }

            // Test without the 'remainder' argument

            {  // test with first initial value
                int       result = INITIAL_VALUE_1;
                const int rv     = Util::parseInt(&result, INPUT, BASE);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_1));
            }

            {  // test with second initial value
                int       result = INITIAL_VALUE_2;
                const int rv     = Util::parseInt(&result, INPUT, BASE);
                ASSERTV(LINE, FAIL == (rv != 0));
                ASSERTV(LINE, rv, result,
                        result == (0 == rv ? VALUE : INITIAL_VALUE_2));
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PARSE DOUBLE
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //:
        //:  3 Corner cases work as expected
        //:
        //:    1 The string ends unexpectedly
        //:    2 The value is as large or small as representable
        //:    3 The value is just large/small than representable
        //:    4 "Interesting" values from "A Program for Testing IEEE
        //:      Decimal-Binary Conversions", Vern Paxson, ICIR 1991.
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   parseDouble(double *res, string_view *rest, string_view in)
        //   parseDouble(double *res, string_view in)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nPARSE DOUBLE"
                                  "\n============\n";

        testDouble::testParseDouble();

#ifdef u_PARSEDOUBLE_USES_STRTOD
        if (verbose) puts("u_PARSEDOUBLE_USES_STRTOD");
#endif
#ifdef u_PARSEDOUBLE_USES_STRTOD_ON_RANGE_ERRORS_ONLY
        if (verbose) puts("u_PARSEDOUBLE_USES_STRTOD_ON_RANGE_ERRORS_ONLY");
    #ifndef u_PARSEDOUBLE_USES_FROM_CHARS
        ASSERT("u_PARSEDOUBLE_USES_STRTOD_ON_RANGE_ERRORS_ONLY without using"
               " 'bsl::from_chars'?" == 0);
    #endif
#endif

#ifdef u_PARSEDOUBLE_USES_FROM_CHARS
        if (verbose) puts("u_PARSEDOUBLE_USES_FROM_CHARS");
#elif !defined(u_PARSEDOUBLE_USES_STRTOD)
        ASSERT("'parseDouble' uses neither 'strtod' nor 'from_chars'?" == 0);
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PARSE DOUBLE TEST MACHINERY
        //
        // Concerns:
        //:  1 'calRestPos' returns the expected values for meaningful
        //:    combinations of 'parsaedChars' specification and 'length'.
        //:
        //:  2 'parseExpected' parses properly:
        //:    1 0..0x1fffffffffffff *decimal* integer strings
        //:    2 hexfloats 0x0.0000000000001p-1022..0x1.fffffffffffffp+1023
        //:    3 hexfloats with less than 13 fractional digits
        //:    4 optional '+' sign for hexfloat binary exponent
        //:    5 "NtA", "Inf", "Nan"
        //:
        //:  3 'TestAssertCounter':
        //:     1 sets 'testStatus' to '0' after construction
        //:     2 recreates 'testStatus' properly when destroyed
        //:
        //:  4 'BslsLogCounterGuard' replaces and restores the handler.
        //:
        //:  5 'BslsReviewMonitor':
        //:     1 on expected events:
        //:         1 counts the events properly
        //:         2 calls the original handler only when 'verbose == true'
        //          3 does not assert
        //:     2 on unexpected events:
        //:         1 asserts with the full content
        //:         2 calls the original handler with all the same arguments
        //:     3 empty file name in an event is ignored
        //
        // Plan:
        //: 1 Use the table-driven approach with combinations of zero, and
        //:   positive length, as well as zero, positive, less-than length
        //:   negative, and very large negative parsed chars values.  Compare
        //:   to manually provided expected values. (C-2)
        //:
        //: 2 Use the table-driven approach with a twist that the expected
        //:   results are created by using the compiler on platforms supporting
        //:   hexfloat for all numeric values, and for decimal integers on
        //:   AIX/Solaris, the platforms that do not support hexfloat literals.
        //:   (AIX thinks it does but it fails on certain subnormals.)
        //:   Hexfloat values on  are tested against manually-provided decimal
        //:   floating point constants on AIX/Solaris.  The distinction in the
        //:   row definitions is made by the preprocessor.  Special,
        //:   non-numeric "NtA", "NaN", and "Inf" are tested separately,
        //:   directly, not in the table-driven test.  On platforms that do
        //:   support hexfloat literals we also run the verification on all
        //:   rows of the "main" 'TEST_DATA' table for 'parseDouble'. (C-3)
        //:
        //: 3 Use the table-driven approach with combinations of an original
        //:   'testStatus' value, value at the end of scope, and the expected
        //:    final 'testStatus' value restored by the destructor. (C-4)
        //:
        //: 4 'BslsLogCounterGuard' is tested by comparing the active handler
        //:   during and after its lifetime.
        //:
        //: 5 'BslsReviewMonitor' is tested by comparing the active handler
        //:   during and after its lifetime, as well as calling the handler
        //:   directly with fake log messages and observing the counts.
        //
        // Testing:
        //   PARSE DOUBLE TEST MACHINERY
        //   size_t calcRestPos(parsedChars, length)
        //   double parseExpected(const bsl::string_view&)
        //   class BslsReviewMonitor
        //   class TestAssertCounter
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nPARSE DOUBLE TEST MACHINERY"
                                  "\n===========================\n";

        if (verbose) bsl::cout << "'calcRestPos'\n";
        testDouble::testCalcRestPos();

        if (verbose) bsl::cout << "'parseExpected'\n";
        testDouble::testParseExpected();

#ifdef BSLS_COMPILERFEATURES_SUPPORT_HEXFLOAT_LITERALS
        if (verbose) bsl::cout << "'parseExpected' on main test data table\n";
        testDouble::testParseExpectedOnMainDataWithCompilerAsOracle();
#endif

        if (verbose) bsl::cout << "'TestAssertCounter'\n";
        testDouble::testTestAssertCounter();

        if (verbose) bsl::cout << "'BslsLogCounterGuard'\n";
        testDouble::testBslsLogCounterGuard();

        if (verbose) bsl::cout << "'BslsReviewMonitor'\n";
        testDouble::testBslsReviewMonitor();

     } break;
      case 3: {
        // --------------------------------------------------------------------
        // PARSE SIGNED INTEGER
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //:
        //:  3 Corner cases work as expected
        //:
        //:    1 The string ends unexpectedly
        //:    2 The value is as large or small as representable
        //:    3 The value is just large/small than representable
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   parseSignedInteger(result, rest, input, base, minVal, maxVal)
        //   parseSignedInteger(result, input, base, minVal, maxVal)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nPARSE SIGNED INTEGER"
                                  "\n====================\n";

        typedef bsls::Types::Int64 Int64;

        const Int64 INITIAL_VALUE_1 =  37;  // first initial value
        const Int64 INITIAL_VALUE_2 = -58;  // second initial value

        const Int64 int64Min = bsl::numeric_limits<Int64>::min();
        const Int64 int64Max = bsl::numeric_limits<Int64>::max();

        static const struct {
            int          d_line;      // source line number
            const char  *d_input_p;   // specification string
            int          d_base;      // specification base
            Int64        d_min;       // specification min
            Int64        d_max;       // specification max
            bsl::size_t  d_offset;    // expected number of parsed characters
            bool         d_fail;      // parsing expected to fail indicator
            Int64        d_value;     // expected return value
        } DATA[] = {
            //line  input text base   min      max    off  fail  value
            //----  ---------- ---- -------- -------- ---  ----  -------
            { L_,   "",         10,  -32768,   32767,  0,    1,        0 },

            { L_,   "a",        10,  -32768,   32767,  0,    1,        0 },
            { L_,   "+",        10,  -32768,   32767,  1,    1,        0 },
            { L_,   "-",        10,  -32768,   32767,  1,    1,        0 },
            { L_,   "0",        10,  -32768,   32767,  1,    0,        0 },
            { L_,   "a",        16,  -32768,   32767,  1,    0,       10 },
            { L_,   "f",        16,  -32768,   32767,  1,    0,       15 },
            { L_,   "A",        16,  -32768,   32767,  1,    0,       10 },
            { L_,   "F",        16,  -32768,   32767,  1,    0,       15 },
            { L_,   "g",        16,  -32768,   32767,  0,    1,        0 },
            { L_,   "0",         2,  -32768,   32767,  1,    0,        0 },
            { L_,   "1",         2,  -32768,   32767,  1,    0,        1 },
            { L_,   "a",        11,  -32768,   32767,  1,    0,       10 },
            { L_,   "A",        11,  -32768,   32767,  1,    0,       10 },
            { L_,   "z",        36,  -32768,   32767,  1,    0,       35 },
            { L_,   "Z",        36,  -32768,   32767,  1,    0,       35 },

            { L_,   "++",       10,  -32768,   32767,  1,    1,        0 },
            { L_,   "+-",       10,  -32768,   32767,  1,    1,        0 },
            { L_,   "-+",       10,  -32768,   32767,  1,    1,        0 },
            { L_,   "--",       10,  -32768,   32767,  1,    1,        0 },
            { L_,   "+a",       10,  -32768,   32767,  1,    1,        0 },
            { L_,   "-a",       10,  -32768,   32767,  1,    1,        0 },
            { L_,   "+0",       10,  -32768,   32767,  2,    0,        0 },
            { L_,   "+9",       10,  -32768,   32767,  2,    0,        9 },
            { L_,   "-0",       10,  -32768,   32767,  2,    0,        0 },
            { L_,   "-9",       10,  -32768,   32767,  2,    0,       -9 },
            { L_,   "0a",       10,  -32768,   32767,  1,    0,        0 },
            { L_,   "9a",       10,  -32768,   32767,  1,    0,        9 },
            { L_,   "00",       10,  -32768,   32767,  2,    0,        0 },
            { L_,   "01",       10,  -32768,   32767,  2,    0,        1 },
            { L_,   "19",       10,  -32768,   32767,  2,    0,       19 },
            { L_,   "99",       10,  -32768,   32767,  2,    0,       99 },
            { L_,   "+g",       16,  -32768,   32767,  1,    1,        0 },
            { L_,   "-g",       16,  -32768,   32767,  1,    1,        0 },
            { L_,   "+a",       16,  -32768,   32767,  2,    0,       10 },
            { L_,   "+f",       16,  -32768,   32767,  2,    0,       15 },
            { L_,   "-a",       16,  -32768,   32767,  2,    0,      -10 },
            { L_,   "-f",       16,  -32768,   32767,  2,    0,      -15 },
            { L_,   "ff",       16,  -32768,   32767,  2,    0,      255 },
            { L_,   "FF",       16,  -32768,   32767,  2,    0,      255 },
            { L_,   "+0",        2,  -32768,   32767,  2,    0,        0 },
            { L_,   "-0",        2,  -32768,   32767,  2,    0,        0 },
            { L_,   "+1",        2,  -32768,   32767,  2,    0,        1 },
            { L_,   "-1",        2,  -32768,   32767,  2,    0,       -1 },
            { L_,   "00",        2,  -32768,   32767,  2,    0,        0 },
            { L_,   "01",        2,  -32768,   32767,  2,    0,        1 },
            { L_,   "10",        2,  -32768,   32767,  2,    0,        2 },
            { L_,   "11",        2,  -32768,   32767,  2,    0,        3 },
            { L_,   "+z",       36,  -32768,   32767,  2,    0,       35 },
            { L_,   "-z",       36,  -32768,   32767,  2,    0,      -35 },
            { L_,   "0z",       36,  -32768,   32767,  2,    0,       35 },
            { L_,   "0Z",       36,  -32768,   32767,  2,    0,       35 },
            { L_,   "10",       36,  -32768,   32767,  2,    0,       36 },
            { L_,   "z0",       36,  -32768,   32767,  2,    0,     1260 },
            { L_,   "Z0",       36,  -32768,   32767,  2,    0,     1260 },

            { L_,   "+0a",      10,  -32768,   32767,  2,    0,        0 },
            { L_,   "+9a",      10,  -32768,   32767,  2,    0,        9 },
            { L_,   "-0a",      10,  -32768,   32767,  2,    0,        0 },
            { L_,   "-9a",      10,  -32768,   32767,  2,    0,       -9 },
            { L_,   "+12",      10,  -32768,   32767,  3,    0,       12 },
            { L_,   "+89",      10,  -32768,   32767,  3,    0,       89 },
            { L_,   "-12",      10,  -32768,   32767,  3,    0,      -12 },
            { L_,   "123",      10,  -32768,   32767,  3,    0,      123 },
            { L_,   "789",      10,  -32768,   32767,  3,    0,      789 },
            { L_,   "+fg",      16,  -32768,   32767,  2,    0,       15 },
            { L_,   "-fg",      16,  -32768,   32767,  2,    0,      -15 },
            { L_,   "+ff",      16,  -32768,   32767,  3,    0,      255 },
            { L_,   "+FF",      16,  -32768,   32767,  3,    0,      255 },
            { L_,   "-ff",      16,  -32768,   32767,  3,    0,     -255 },
            { L_,   "-FF",      16,  -32768,   32767,  3,    0,     -255 },
            { L_,   "fff",      16,  -32768,   32767,  3,    0,     4095 },
            { L_,   "fFf",      16,  -32768,   32767,  3,    0,     4095 },
            { L_,   "FfF",      16,  -32768,   32767,  3,    0,     4095 },
            { L_,   "FFF",      16,  -32768,   32767,  3,    0,     4095 },

            { L_,   "1234",     10,  -32768,   32767,  4,    0,     1234 },
            { L_,   "-123",     10,  -32768,   32767,  4,    0,     -123 },
            { L_,   "7FFF",     16,  -32768,   32767,  4,    0,    32767 },

            { L_,   "12345",    10,  -32768,   32767,  5,    0,    12345 },
            { L_,   "-1234",    10,  -32768,   32767,  5,    0,    -1234 },
            { L_,   "-7FFF",    16,  -32768,   32767,  5,    0,   -32767 },
            { L_,   "32766",    10,  -32768,   32767,  5,    0,    32766 },
            { L_,   "32767",    10,  -32768,   32767,  5,    0,    32767 },
            { L_,   "32768",    10,  -32768,   32767,  4,    0,     3276 },
            { L_,   "32769",    10,  -32768,   32767,  4,    0,     3276 },
            { L_,   "-8000",    16,  -32768,   32767,  5,    0,   -32768 },
            { L_,   "32768",    10,  -32768,   32767,  4,    0,     3276 },
            { L_,   "32768",    10,
                            -2147483648LL, 2147483647LL,  5,    0,    32768 },
            { L_,   "32769",    10,
                            -2147483648LL, 2147483647LL,  5,    0,    32769 },

            { L_,   "123456",   10,  -32768,   32767,  5,    0,    12345 },
            { L_,   "-32766",   10,  -32768,   32767,  6,    0,   -32766 },
            { L_,   "-32767",   10,  -32768,   32767,  6,    0,   -32767 },
            { L_,   "-32768",   10,  -32768,   32767,  6,    0,   -32768 },
            { L_,   "-32769",   10,  -32768,   32767,  5,    0,    -3276 },

            { L_,   "-123456",  10,  -32768,   32767,  6,    0,   -12345 },

            { L_,   "2147483647", 10,
                            -2147483648LL, 2147483647LL, 10,    0,
                                                                2147483647 },
            { L_,   "2147483648", 10,
                            -2147483648LL, 2147483647LL,  9,    0,
                                                                214748364 },
            { L_,   "2147483649", 10,
                            -2147483648LL, 2147483647LL,  9,    0,
                                                                214748364 },
            { L_,   "2147483648", 10,
                                int64Min,     int64Max, 10,    0,
                                                            2147483648LL },
            { L_,   "2147483649", 10,
                bsl::numeric_limits<Int64>::min(), 0x7FFFFFFFFFFFFFFFLL,
                                                    10,    0, 2147483649LL },

            { L_,   "-2147483647", 10,
                            -2147483648LL, 2147483647LL, 11,    0,
                                                                -2147483647 },
            { L_,   "-2147483648", 10,
                            -2147483648LL, 2147483647LL, 11,    0,
                                                            -2147483648LL },
            { L_,   "-2147483649", 10,
                            -2147483648LL, 2147483647LL, 10,    0,
                                                                -214748364 },
            { L_,   "-2147483648", 10,
                                int64Min, int64Max,     11,    0,
                                                            -2147483648LL },
            { L_,   "-2147483649", 10,
                                int64Min, int64Max,     11,    0,
                                                            -2147483649LL },

            { L_,   "7fffffffffffffff", 16,
                                int64Min, int64Max,     16,   0,
                                                    0x7FFFFFFFFFFFFFFFLL },
            { L_,   "-8000000000000000", 16,
                                int64Min, int64Max,     17,   0,  int64Min },
            { L_,   "-8000000000000001", 16,
                                int64Min, int64Max,     16,   0,
                                                        -0x800000000000000LL},
        };
        const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        char opBuff[32];  // maximum length of input strings + 2

        for (bsl::size_t ti = 0; ti < NUM_DATA ; ++ti) {
            const int            LINE   = DATA[ti].d_line;
            const char *const    INPUT  = DATA[ti].d_input_p;
            const int            BASE   = DATA[ti].d_base;
            const Int64          MIN    = DATA[ti].d_min;
            const Int64          MAX    = DATA[ti].d_max;
            const bsl::ptrdiff_t NUM    = DATA[ti].d_offset;
            const bool           FAIL   = DATA[ti].d_fail;
            const Int64          VALUE  = DATA[ti].d_value;
            const bsl::size_t    LENGTH = bsl::strlen(INPUT);

            if (veryVerbose) {
                P_(LINE) P_(INPUT) P_(LENGTH) P_(BASE) P_(MIN) P_(MAX) P_(NUM)
                P_(FAIL) P(VALUE);
            }

            // orthogonal perturbation over terminating whitespace
            for (int opi = 0; opi < 2; ++opi) {

                // Prepare 'opBuff' with the input text and the unset bytes
                bsl::memset(&opBuff, '\xff', sizeof opBuff);
                bsl::memcpy(opBuff, INPUT, LENGTH);

                if (veryVerbose) {
                    bsl::cout << (opi
                                ? "original input"
                                : "trailing whitespace") << '\n';
                }

                if (opi) {
                    if (LENGTH + 2 >= sizeof opBuff) {
                        // to avoid buffer overflow
                        continue;                                   // CONTINUE
                    }

                    opBuff[LENGTH] = ' ';
                }

                const bsl::string_view IN_TEXT(opBuff, LENGTH + opi);

                {  // test with first initial value
                    Int64            result = INITIAL_VALUE_1;
                    bsl::string_view rest;
                    const int   rv = Util::parseSignedInteger(&result,
                                                              &rest,
                                                              IN_TEXT,
                                                              BASE,
                                                              MIN,
                                                              MAX);
                    ASSERTV(LINE, opi, NUM, rest.data() - opBuff,
                            NUM == rest.data() - opBuff);
                    ASSERTV(LINE, opi, FAIL, rv, FAIL == (rv != 0));

                    const Int64 expectedResult =
                                           (0 == rv ? VALUE : INITIAL_VALUE_1);

                    if (veryVeryVerbose) {
                        P_(result) P_(rv) P(expectedResult);
                    }
                    ASSERTV(LINE, opi, result, rv, expectedResult,
                            result == expectedResult);
                }

                {  // test with second initial value
                    Int64       result = INITIAL_VALUE_2;
                    bsl::string_view rest;
                    const int   rv = Util::parseSignedInteger(&result,
                                                              &rest,
                                                              IN_TEXT,
                                                              BASE,
                                                              MIN,
                                                              MAX);
                    ASSERTV(LINE, opi, NUM, rest.data() - opBuff,
                            NUM == rest.data() - opBuff);
                    ASSERTV(LINE, opi, FAIL, rv, FAIL == (rv != 0));

                    const Int64 expectedResult =
                                        (0 == rv ? VALUE : INITIAL_VALUE_2);

                    if (veryVeryVerbose) {
                        P_(result) P_(rv) P(expectedResult);
                    }
                    ASSERTV(LINE, opi, result, rv, expectedResult,
                            result == expectedResult);
                }

                // Test without the 'remainder' argument

                {  // test with first initial value
                    Int64     result = INITIAL_VALUE_1;
                    const int rv = Util::parseSignedInteger(&result,
                                                            IN_TEXT,
                                                            BASE,
                                                            MIN,
                                                            MAX);
                    ASSERTV(LINE, opi, FAIL, rv, FAIL == (rv != 0));

                    const Int64 expectedResult =
                                           (0 == rv ? VALUE : INITIAL_VALUE_1);

                    if (veryVeryVerbose) {
                        P_(result) P_(rv) P(expectedResult);
                    }
                    ASSERTV(LINE, opi, result, rv, expectedResult,
                            result == expectedResult);
                }

                {  // test with second initial value
                    Int64     result = INITIAL_VALUE_2;
                    const int rv = Util::parseSignedInteger(&result,
                                                            IN_TEXT,
                                                            BASE,
                                                            MIN,
                                                            MAX);
                    ASSERTV(LINE, opi, FAIL, rv, FAIL == (rv != 0));

                    const Int64 expectedResult =
                                        (0 == rv ? VALUE : INITIAL_VALUE_2);

                    if (veryVeryVerbose) {
                        P_(result) P_(rv) P(expectedResult);
                    }
                    ASSERTV(LINE, opi, result, rv, expectedResult,
                            result == expectedResult);
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PARSE UNSIGNED INTEGER
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //:
        //:  3 Corner cases work as expected
        //:
        //:    1 The string ends unexpectedly
        //:    2 The value is as large or small as representable
        //:    3 The value is just large/small than representable
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   parseUnsignedInteger(result, rest, in, base, maxVal)
        //   parseUnsignedInteger(result, rest, in, base, maxVal, maxDigit)
        //   parseUnsignedInteger(result, in, base, maxVal)
        //   parseUnsignedInteger(result, in, base, maxVal, maxDigit)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nPARSE UNSIGNED INTEGER"
                                  "\n======================\n";

        typedef bsls::Types::Uint64 Uint64;

        if (verbose) bsl::cout << "\nDefault maximum number of digits.\n";
        {
            const Uint64 INITIAL_VALUE_1 = 37;  // first initial value
            const Uint64 INITIAL_VALUE_2 = 58;  // second initial value

            static const struct {
                int          d_line;      // source line number
                const char  *d_input_p;   // specification string
                int          d_base;      // specification base
                Uint64       d_max;       // specification max
                bsl::size_t  d_offset;    // expected num. of parsed characters
                bool         d_fail;      // parsing expected to fail indicator
                Uint64       d_value;     // expected return value
            } DATA[] = {
                //line  input text base   max    off  fail  value
                //----  ---------- ---- -------- ---  ----  -------
                { L_,   "",         10,   32767,  0,    1,        0 },

                { L_,   "a",        10,   32767,  0,    1,        0 },
                { L_,   "+",        10,   32767,  0,    1,        0 },
                { L_,   "-",        10,   32767,  0,    1,        0 },
                { L_,   "0",        10,   32767,  1,    0,        0 },
                { L_,   "a",        16,   32767,  1,    0,       10 },
                { L_,   "f",        16,   32767,  1,    0,       15 },
                { L_,   "A",        16,   32767,  1,    0,       10 },
                { L_,   "F",        16,   32767,  1,    0,       15 },
                { L_,   "g",        16,   32767,  0,    1,        0 },
                { L_,   "0",         2,   32767,  1,    0,        0 },
                { L_,   "1",         2,   32767,  1,    0,        1 },
                { L_,   "a",        11,   32767,  1,    0,       10 },
                { L_,   "A",        11,   32767,  1,    0,       10 },
                { L_,   "z",        36,   32767,  1,    0,       35 },
                { L_,   "Z",        36,   32767,  1,    0,       35 },

                { L_,   "+a",       10,   32767,  0,    1,        0 },
                { L_,   "-a",       10,   32767,  0,    1,        0 },
                { L_,   "+0",       10,   32767,  0,    1,        0 },
                { L_,   "+9",       10,   32767,  0,    1,        0 },
                { L_,   "-0",       10,   32767,  0,    1,        0 },
                { L_,   "-9",       10,   32767,  0,    1,        0 },
                { L_,   "0a",       10,   32767,  1,    0,        0 },
                { L_,   "9a",       10,   32767,  1,    0,        9 },
                { L_,   "00",       10,   32767,  2,    0,        0 },
                { L_,   "01",       10,   32767,  2,    0,        1 },
                { L_,   "19",       10,   32767,  2,    0,       19 },
                { L_,   "99",       10,   32767,  2,    0,       99 },
                { L_,   "ff",       16,   32767,  2,    0,      255 },
                { L_,   "FF",       16,   32767,  2,    0,      255 },
                { L_,   "00",        2,   32767,  2,    0,        0 },
                { L_,   "01",        2,   32767,  2,    0,        1 },
                { L_,   "10",        2,   32767,  2,    0,        2 },
                { L_,   "11",        2,   32767,  2,    0,        3 },
                { L_,   "0z",       36,   32767,  2,    0,       35 },
                { L_,   "0Z",       36,   32767,  2,    0,       35 },
                { L_,   "10",       36,   32767,  2,    0,       36 },
                { L_,   "z0",       36,   32767,  2,    0,     1260 },
                { L_,   "Z0",       36,   32767,  2,    0,     1260 },

                { L_,   "123",      10,   32767,  3,    0,      123 },
                { L_,   "789",      10,   32767,  3,    0,      789 },
                { L_,   "fff",      16,   32767,  3,    0,     4095 },
                { L_,   "fFf",      16,   32767,  3,    0,     4095 },
                { L_,   "FfF",      16,   32767,  3,    0,     4095 },
                { L_,   "FFF",      16,   32767,  3,    0,     4095 },

                { L_,   "1234",     10,   32767,  4,    0,     1234 },
                { L_,   "7FFF",     16,   32767,  4,    0,    32767 },

                { L_,   "12345",    10,   32767,  5,    0,    12345 },
                { L_,   "32766",    10,   32767,  5,    0,    32766 },
                { L_,   "32767",    10,   32767,  5,    0,    32767 },
                { L_,   "32768",    10,   32767,  4,    0,     3276 },
                { L_,   "32769",    10,   32767,  4,    0,     3276 },
                { L_,   "32768",    10,   32767,  4,    0,     3276 },
                { L_,   "32768",    10,
                                     2147483647,  5,    0,    32768 },
                { L_,   "32769",    10,
                                     2147483647,  5,    0,    32769 },

                { L_,   "123456",   10,   32767,  5,    0,    12345 },

                { L_,   "2147483647", 10,
                                     2147483647, 10,    0, 2147483647 },
                { L_,   "2147483648", 10,
                                     2147483647,  9,    0, 214748364 },
                { L_,   "2147483649", 10,
                                     2147483647,  9,    0, 214748364 },
                { L_,   "2147483648", 10,
                          0x7FFFFFFFFFFFFFFFuLL, 10,    0, 2147483648uLL },
                { L_,   "2147483649", 10,
                          0x7FFFFFFFFFFFFFFFuLL, 10,    0, 2147483649uLL },

                { L_,   "7fffffffffffffff", 16,
                          0x7FFFFFFFFFFFFFFFuLL, 16,    0,
                                                0x7FFFFFFFFFFFFFFFuLL },
                { L_,   "8000000000000000", 16,
                          0x7FFFFFFFFFFFFFFFuLL, 15,    0,
                                                 0x800000000000000uLL },
                { L_,   "8000000000000000", 16,
                          0x8000000000000000uLL, 16,    0,
                                                0x8000000000000000uLL },
            };
            const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            char opBuff[32];  // max length of input strings + 2

            for (bsl::size_t ti = 0; ti < NUM_DATA ; ++ti) {
                const int            LINE   = DATA[ti].d_line;
                const char *const    INPUT  = DATA[ti].d_input_p;
                const int            BASE   = DATA[ti].d_base;
                const Uint64         MAX    = DATA[ti].d_max;
                const bsl::ptrdiff_t NUM    = DATA[ti].d_offset;
                const bool           FAIL   = DATA[ti].d_fail;
                const Uint64         VALUE  = DATA[ti].d_value;
                const bsl::size_t    LENGTH = bsl::strlen(INPUT);

                ASSERTV(LINE, LENGTH + 2 < sizeof opBuff);
                               // orthogonal perturbation will fit in 'opBuff'

                // orthogonal perturbation over terminating whitespace
                for (int opi = 0; opi < 2; ++opi) {

                    // Prepare the opBuff
                    bsl::memset(&opBuff, '\xff', sizeof opBuff);
                    bsl::memcpy(opBuff, INPUT, LENGTH);

                    if (veryVerbose) {
                        bsl::cout << (opi
                                    ? "original input"
                                    : "trailing whitespace") << '\n';
                    }

                    if (opi) {
                        if (LENGTH + 2 >= sizeof opBuff) {
                            // to avoid buffer overflow
                            continue;                               // CONTINUE
                        }

                        opBuff[LENGTH] = ' ';
                    }

                    const bsl::string_view IN_TEXT(opBuff, LENGTH + opi);

                    {  // test with first initial value
                        Uint64           result = INITIAL_VALUE_1;
                        bsl::string_view rest;
                        const int   rv = Util::parseUnsignedInteger(&result,
                                                                    &rest,
                                                                    IN_TEXT,
                                                                    BASE,
                                                                    MAX);
                        ASSERTV(LINE, opi, NUM, rest.data() - opBuff,
                                NUM == rest.data() - opBuff);
                        ASSERTV(LINE, opi, FAIL, rv, FAIL == (rv != 0));

                        const Uint64 expectedResult =
                                           (0 == rv ? VALUE : INITIAL_VALUE_1);

                        if (veryVeryVerbose) {
                            P_(result) P_(rv) P(expectedResult);
                        }
                        ASSERTV(LINE, opi, result, rv, expectedResult,
                                result == expectedResult);
                    }

                    {  // test with second initial value
                        Uint64      result = INITIAL_VALUE_2;
                        bsl::string_view rest;
                        const int   rv = Util::parseUnsignedInteger(&result,
                                                                    &rest,
                                                                    IN_TEXT,
                                                                    BASE,
                                                                    MAX);
                        ASSERTV(LINE, opi, NUM, rest.data() - opBuff,
                                NUM == rest.data() - opBuff);
                        ASSERTV(LINE, opi, FAIL, rv, FAIL == (rv != 0));

                        const Uint64 expectedResult =
                                           (0 == rv ? VALUE : INITIAL_VALUE_2);

                        if (veryVeryVerbose) {
                            P_(result) P_(rv) P(expectedResult);
                        }
                        ASSERTV(LINE, opi, result, rv, expectedResult,
                                result == expectedResult);
                    }

                    // Test without the 'remainder' argument

                    {  // test with first initial value
                        Uint64    result = INITIAL_VALUE_1;
                        const int rv     = Util::parseUnsignedInteger(&result,
                                                                      IN_TEXT,
                                                                      BASE,
                                                                      MAX);
                        ASSERTV(LINE, opi, FAIL, rv, FAIL == (rv != 0));

                        const Uint64 expectedResult =
                                           (0 == rv ? VALUE : INITIAL_VALUE_1);

                        if (veryVeryVerbose) {
                            P_(result) P_(rv) P(expectedResult);
                        }
                        ASSERTV(LINE, opi, result, rv, expectedResult,
                                result == expectedResult);
                    }

                    {  // test with second initial value
                        Uint64    result = INITIAL_VALUE_2;
                        const int rv     = Util::parseUnsignedInteger(&result,
                                                                      IN_TEXT,
                                                                      BASE,
                                                                      MAX);
                        ASSERTV(LINE, opi, FAIL, rv, FAIL == (rv != 0));

                        const Uint64 expectedResult =
                                           (0 == rv ? VALUE : INITIAL_VALUE_2);

                        if (veryVeryVerbose) {
                            P_(result) P_(rv) P(expectedResult);
                        }
                        ASSERTV(LINE, opi, result, rv, expectedResult,
                                result == expectedResult);
                    }
                }
            }
        }

        if (verbose) bsl::cout << "\nSpecified maximum number of digits.\n";
        {
            const Uint64 INITIAL_VALUE_1 = 37;  // first initial value
            const Uint64 INITIAL_VALUE_2 = 58;  // second initial value

            static const struct {
                int          d_line;      // source line number
                const char  *d_input_p;   // specification string
                int          d_base;      // specification base
                Uint64       d_max;       // specification max
                int          d_digit;     // specification number of digits
                bsl::size_t  d_offset;    // expected num. of parsed characters
                bool         d_fail;      // parsing expected to fail indicator
                Uint64       d_value;     // expected return value
            } DATA[] = {
                //line  input text base   max    dig off  fail  value
                //----  ---------- ---- -------- --- ---  ----  -------
                { L_,   "a",        10,   32767,  1,  0,    1,        0 },
                { L_,   "+",        10,   32767,  1,  0,    1,        0 },
                { L_,   "-",        10,   32767,  1,  0,    1,        0 },
                { L_,   "0",        10,   32767,  1,  1,    0,        0 },
                { L_,   "a",        16,   32767,  1,  1,    0,       10 },
                { L_,   "f",        16,   32767,  1,  1,    0,       15 },
                { L_,   "A",        16,   32767,  1,  1,    0,       10 },
                { L_,   "F",        16,   32767,  1,  1,    0,       15 },
                { L_,   "g",        16,   32767,  1,  0,    1,        0 },
                { L_,   "0",         2,   32767,  1,  1,    0,        0 },
                { L_,   "1",         2,   32767,  1,  1,    0,        1 },
                { L_,   "a",        11,   32767,  1,  1,    0,       10 },
                { L_,   "A",        11,   32767,  1,  1,    0,       10 },
                { L_,   "z",        36,   32767,  1,  1,    0,       35 },
                { L_,   "Z",        36,   32767,  1,  1,    0,       35 },

                { L_,   "+a",       10,   32767,  1,  0,    1,        0 },
                { L_,   "-a",       10,   32767,  1,  0,    1,        0 },
                { L_,   "+0",       10,   32767,  1,  0,    1,        0 },
                { L_,   "+9",       10,   32767,  1,  0,    1,        0 },
                { L_,   "-0",       10,   32767,  1,  0,    1,        0 },
                { L_,   "-9",       10,   32767,  1,  0,    1,        0 },
                { L_,   "0a",       10,   32767,  1,  1,    0,        0 },
                { L_,   "9a",       10,   32767,  1,  1,    0,        9 },
                { L_,   "00",       10,   32767,  1,  1,    0,        0 },
                { L_,   "01",       10,   32767,  1,  1,    0,        0 },
                { L_,   "19",       10,   32767,  1,  1,    0,        1 },
                { L_,   "99",       10,   32767,  1,  1,    0,        9 },
                { L_,   "ff",       16,   32767,  1,  1,    0,       15 },
                { L_,   "FF",       16,   32767,  1,  1,    0,       15 },
                { L_,   "00",        2,   32767,  1,  1,    0,        0 },
                { L_,   "01",        2,   32767,  1,  1,    0,        0 },
                { L_,   "10",        2,   32767,  1,  1,    0,        1 },
                { L_,   "11",        2,   32767,  1,  1,    0,        1 },
                { L_,   "0z",       36,   32767,  1,  1,    0,        0 },
                { L_,   "0Z",       36,   32767,  1,  1,    0,        0 },
                { L_,   "10",       36,   32767,  1,  1,    0,        1 },
                { L_,   "z0",       36,   32767,  1,  1,    0,       35 },
                { L_,   "Z0",       36,   32767,  1,  1,    0,       35 },

                { L_,   "+a",       10,   32767,  2,  0,    1,        0 },
                { L_,   "-a",       10,   32767,  2,  0,    1,        0 },
                { L_,   "+0",       10,   32767,  2,  0,    1,        0 },
                { L_,   "+9",       10,   32767,  2,  0,    1,        0 },
                { L_,   "-0",       10,   32767,  2,  0,    1,        0 },
                { L_,   "-9",       10,   32767,  2,  0,    1,        0 },
                { L_,   "0a",       10,   32767,  2,  1,    0,        0 },
                { L_,   "9a",       10,   32767,  2,  1,    0,        9 },
                { L_,   "00",       10,   32767,  2,  2,    0,        0 },
                { L_,   "01",       10,   32767,  2,  2,    0,        1 },
                { L_,   "19",       10,   32767,  2,  2,    0,       19 },
                { L_,   "99",       10,   32767,  2,  2,    0,       99 },
                { L_,   "ff",       16,   32767,  2,  2,    0,      255 },
                { L_,   "FF",       16,   32767,  2,  2,    0,      255 },
                { L_,   "00",        2,   32767,  2,  2,    0,        0 },
                { L_,   "01",        2,   32767,  2,  2,    0,        1 },
                { L_,   "10",        2,   32767,  2,  2,    0,        2 },
                { L_,   "11",        2,   32767,  2,  2,    0,        3 },
                { L_,   "0z",       36,   32767,  2,  2,    0,       35 },
                { L_,   "0Z",       36,   32767,  2,  2,    0,       35 },
                { L_,   "10",       36,   32767,  2,  2,    0,       36 },
                { L_,   "z0",       36,   32767,  2,  2,    0,     1260 },
                { L_,   "Z0",       36,   32767,  2,  2,    0,     1260 },
            };
            const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            char opBuff[32];  // max length of input strings + 2

            for (bsl::size_t ti = 0; ti < NUM_DATA ; ++ti) {
                const int            LINE   = DATA[ti].d_line;
                const char *const    INPUT  = DATA[ti].d_input_p;
                const int            BASE   = DATA[ti].d_base;
                const Uint64         MAX    = DATA[ti].d_max;
                const int            DIGIT  = DATA[ti].d_digit;
                const bsl::ptrdiff_t NUM    = DATA[ti].d_offset;
                const bool           FAIL   = DATA[ti].d_fail;
                const Uint64         VALUE  = DATA[ti].d_value;
                const bsl::size_t    LENGTH = bsl::strlen(INPUT);

                if (veryVerbose) {
                    P(LINE);
                    P(INPUT);
                    P(BASE);
                }

                ASSERTV(LINE, LENGTH + 2 < sizeof opBuff);
                                // orthogonal perturbation will fit in 'opBuff'

                // orthogonal perturbation over terminating whitespace
                for (int opi = 0; opi < 2; ++opi) {

                    // Prepare 'opBuff'
                    bsl::memset(&opBuff, '\xff', sizeof opBuff);
                    bsl::memcpy(opBuff, INPUT, LENGTH);

                    if (veryVerbose) {
                        bsl::cout << (opi
                                    ? "original input"
                                    : "trailing whitespace") << '\n';
                    }

                    if (opi) {
                        if (LENGTH + 2 >= sizeof opBuff) {
                            // to avoid buffer overflow
                            continue;                               // CONTINUE
                        }

                        opBuff[LENGTH] = ' ';
                    }

                    const bsl::string_view IN_TEXT(opBuff, LENGTH + opi);

                    {  // test with first initial value
                        Uint64      result = INITIAL_VALUE_1;
                        bsl::string_view rest;
                        const int   rv = Util::parseUnsignedInteger(&result,
                                                                    &rest,
                                                                    IN_TEXT,
                                                                    BASE,
                                                                    MAX,
                                                                    DIGIT);
                        ASSERTV(LINE, opi, NUM, rest.data() - opBuff,
                                NUM == rest.data() - opBuff);
                        ASSERTV(LINE, opi, FAIL, rv, FAIL == (rv != 0));

                        const Uint64 expectedResult =
                                           (0 == rv ? VALUE : INITIAL_VALUE_1);

                        if (veryVeryVerbose) {
                            P_(result) P_(rv) P(expectedResult);
                        }
                        ASSERTV(LINE, opi, result, rv, expectedResult,
                                result == expectedResult);
                    }

                    {  // test with second initial value
                        Uint64      result = INITIAL_VALUE_2;
                        bsl::string_view rest;
                        const int   rv = Util::parseUnsignedInteger(&result,
                                                                    &rest,
                                                                    IN_TEXT,
                                                                    BASE,
                                                                    MAX,
                                                                    DIGIT);
                        ASSERTV(LINE, opi, NUM, rest.data() - opBuff,
                                NUM == rest.data() - opBuff);
                        ASSERTV(LINE, opi, FAIL, rv, FAIL == (rv != 0));

                        const Uint64 expectedResult =
                                           (0 == rv ? VALUE : INITIAL_VALUE_2);

                        if (veryVeryVerbose) {
                            P_(result) P_(rv) P(expectedResult);
                        }
                        ASSERTV(LINE, opi, result, rv, expectedResult,
                                result == expectedResult);
                    }

                    // Test without the 'remainder' argument

                    {  // test with first initial value
                        Uint64      result = INITIAL_VALUE_1;
                        const int   rv = Util::parseUnsignedInteger(&result,
                                                                    IN_TEXT,
                                                                    BASE,
                                                                    MAX,
                                                                    DIGIT);
                        ASSERTV(LINE, opi, FAIL, rv, FAIL == (rv != 0));

                        const Uint64 expectedResult =
                                           (0 == rv ? VALUE : INITIAL_VALUE_1);

                        if (veryVeryVerbose) {
                            P_(result) P_(rv) P(expectedResult);
                        }
                        ASSERTV(LINE, opi, result, rv, expectedResult,
                                result == expectedResult);
                    }

                    {  // test with second initial value
                        Uint64    result = INITIAL_VALUE_2;
                        const int rv     = Util::parseUnsignedInteger(&result,
                                                                      IN_TEXT,
                                                                      BASE,
                                                                      MAX,
                                                                      DIGIT);
                        ASSERTV(LINE, opi, FAIL, rv, FAIL == (rv != 0));

                        const Uint64 expectedResult =
                                           (0 == rv ? VALUE : INITIAL_VALUE_2);

                        if (veryVeryVerbose) {
                            P_(result) P_(rv) P(expectedResult);
                        }
                        ASSERTV(LINE, opi, result, rv, expectedResult,
                                result == expectedResult);
                    }
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // CHARACTER TO DIGIT
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   characterToDigit(char character, int base)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nCHARACTER TO DIGIT"
                                  "\n==================\n";

        static const struct {
            int   d_line;  // source line number
            char  d_inChar;   // input character
            int   d_base;     // the numeric base
            short d_value;    // expected return value
        } DATA[] = {
            //line  inchar  base  value
            //----  ----    ----  -----
            { L_,    '0',    2,     0 },
            { L_,    '1',    2,     1 },
            { L_,    '2',    2,    -1 },
            { L_,    '7',    2,    -1 },
            { L_,    '8',    2,    -1 },
            { L_,    '9',    2,    -1 },
            { L_,    'a',    2,    -1 },
            { L_,    'f',    2,    -1 },
            { L_,    'z',    2,    -1 },
            { L_,    'A',    2,    -1 },
            { L_,    'F',    2,    -1 },
            { L_,    'Z',    2,    -1 },
            { L_,    ' ',    2,    -1 },
            { L_,    '+',    2,    -1 },
            { L_,    '-',    2,    -1 },

            { L_,    '0',    8,     0 },
            { L_,    '1',    8,     1 },
            { L_,    '2',    8,     2 },
            { L_,    '7',    8,     7 },
            { L_,    '8',    8,    -1 },
            { L_,    '9',    8,    -1 },
            { L_,    'a',    8,    -1 },
            { L_,    'f',    8,    -1 },
            { L_,    'z',    8,    -1 },
            { L_,    'A',    8,    -1 },
            { L_,    'F',    8,    -1 },
            { L_,    'Z',    8,    -1 },
            { L_,    ' ',    8,    -1 },
            { L_,    '+',    8,    -1 },
            { L_,    '-',    8,    -1 },

            { L_,    '0',   10,     0 },
            { L_,    '1',   10,     1 },
            { L_,    '2',   10,     2 },
            { L_,    '7',   10,     7 },
            { L_,    '8',   10,     8 },
            { L_,    '9',   10,     9 },
            { L_,    'a',   10,    -1 },
            { L_,    'f',   10,    -1 },
            { L_,    'z',   10,    -1 },
            { L_,    'A',   10,    -1 },
            { L_,    'F',   10,    -1 },
            { L_,    'Z',   10,    -1 },
            { L_,    ' ',   10,    -1 },
            { L_,    '+',   10,    -1 },
            { L_,    '-',   10,    -1 },

            { L_,    '0',   16,     0 },
            { L_,    '1',   16,     1 },
            { L_,    '2',   16,     2 },
            { L_,    '7',   16,     7 },
            { L_,    '8',   16,     8 },
            { L_,    '9',   16,     9 },
            { L_,    'a',   16,    10 },
            { L_,    'f',   16,    15 },
            { L_,    'z',   16,    -1 },
            { L_,    'A',   16,    10 },
            { L_,    'F',   16,    15 },
            { L_,    'Z',   16,    -1 },
            { L_,    ' ',   16,    -1 },
            { L_,    '+',   16,    -1 },
            { L_,    '-',   16,    -1 },

            { L_,    '0',   36,     0 },
            { L_,    '1',   36,     1 },
            { L_,    '2',   36,     2 },
            { L_,    '7',   36,     7 },
            { L_,    '8',   36,     8 },
            { L_,    '9',   36,     9 },
            { L_,    'a',   36,    10 },
            { L_,    'f',   36,    15 },
            { L_,    'z',   36,    35 },
            { L_,    'A',   36,    10 },
            { L_,    'F',   36,    15 },
            { L_,    'Z',   36,    35 },
            { L_,    ' ',   36,    -1 },
            { L_,    '+',   36,    -1 },
            { L_,    '-',   36,    -1 },
        };
        const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (bsl::size_t ti = 0; ti < NUM_DATA ; ++ti) {
            const int   LINE       = DATA[ti].d_line;
            const char  INPUT_CHAR = DATA[ti].d_inChar;
            const int   BASE       = DATA[ti].d_base;
            const short VALUE      = DATA[ti].d_value;

            if (veryVerbose) {
                P_(LINE) P_(INPUT_CHAR) P_(BASE) P(VALUE);
            }

            const int rv = Util::characterToDigit(INPUT_CHAR, BASE);
            ASSERTV(LINE, VALUE, rv, VALUE == rv);
        }
        // TODO: failure modes (in char is out of range, 'base' out of range)
      } break;

      // ======================================================================
      // INTERACTIVE TEST CASES

      case -1: {
        // --------------------------------------------------------------------
        // BENCHMARKING PARSE DOUBLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBENCHMARKING PARSE DOUBLE"
                                  "\n=========================\n";

        const int numofMeasuredRuns = argc > 2 ? atoi(argv[2]) : 500000;
        const int numofWarmupRuns   = argc > 3 ? atoi(argv[3]) : 50;

        bsl::cout << '\n';

        if (0 == numofMeasuredRuns || 0 == numofWarmupRuns) {
            bsl::cout
                << "Usage: " << argv[0] << "[#measured-runs [#warm-up-runs]]"
                   "\n\tmeasured-runs default: 10000"
                   "\n\twarm-up-runs default : 50\n\n";
            exit(1);
        }

        P_(numofWarmupRuns); P(numofMeasuredRuns); puts("");

        testDouble::benchmarkParseDouble(numofWarmupRuns, numofMeasuredRuns);
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // REPORTING 'parseDouble' IMPLEMENTATION ASSUMPTIONS VALIDITY
        // --------------------------------------------------------------------

        if (verbose) bsl::cout <<
            "\nREPORTING 'parseDouble' IMPLEMENTATION ASSUMPTIONS VALIDITY"
            "\n===========================================================\n";

#ifdef u_PARSEDOUBLE_USES_STRTOD
        bsl::cout << "\nAssumptions about 'strtod' behavior:\n";
        testDouble::StrtodAssumptions::reportForWholeTestTable();
#endif

#ifdef u_PARSEDOUBLE_USES_FROM_CHARS
        bsl::cout << "\nAssumptions about 'std::from_chars' behavior\n";
        testDouble::FromCharsAssumptions::report();
#endif
      } break;
      default: {
        bsl::cerr << "WARNING: CASE '" << test << "' NOT FOUND.\n";
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << ".\n";
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
