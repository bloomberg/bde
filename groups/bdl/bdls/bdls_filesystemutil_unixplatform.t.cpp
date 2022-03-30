// bdls_filesystemutil_unixplatform.t.cpp                             -*-C++-*-
#include <bdls_filesystemutil_unixplatform.h>

#include <bslim_testutil.h>

#include <bslmf_assert.h>
#include <bslmf_isintegral.h>

#include <bsls_platform.h>

#include <bsl_deque.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <sys/stat.h>
#include <sys/types.h>
#endif

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test provides zero or one of a set of macros depending
// on the current platform configuration.  To test this component, we first
// record all aspects of the current platform that this component uses to
// determine which macros are defined, and then verify that the correct macro,
// if any, is defined given the specification in the header of this component.
// ----------------------------------------------------------------------------
// MACROS
// [ 2] BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF
// [ 2] BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF
// [ 2] BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF64
// ----------------------------------------------------------------------------
// [ 2] CONCERN: Exactly one of the '*_OFF*' macros is defined on any Unix.
// [ 2] CONCERN: None of the '*_OFF*' macros are defined on any non-Unix.
// [ 1] CONCERN: The correct macro is enabled given the current platform.

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

                               // ==============
                               // struct CpuBits
                               // ==============

struct CpuBits {
    // This 'struct' provides a namespace for enumerating the set of CPU word
    // width values that contribute to the determination of which macros this
    // component defines.

    // TYPES
    enum Enum {
        e_32,
        e_64
    };

    enum {
        k_NUM_ENUMERATORS = 2
    };

    BSLMF_ASSERT(k_NUM_ENUMERATORS == e_64 + 1);
};

                           // =====================
                           // struct FileOffsetBits
                           // =====================

struct FileOffsetBits {
    // This 'struct' provides a namespace for enumerating the set of values for
    // the '_FILE_OFFSET_BITS' macro that can contribute to the determination
    // of which macros this component defines.

    // TYPES
    enum Enum {
        e_NOT_DEFINED,
        e_64
    };

    enum {
        k_NUM_ENUMERATORS = 2
    };

    BSLMF_ASSERT(k_NUM_ENUMERATORS == e_64 + 1);
};

                            // ====================
                            // struct LargeFileMode
                            // ====================

struct LargeFileMode {
    // This 'struct' provides a namespace for enumerating the set of values for
    // the '_LARGE_FILE' or '_LARGEFILE64_SOURCE' macros that can contribute to
    // the determination of which macros this component defines.

    // TYPES
    enum Enum {
        e_NOT_DEFINED,
        e_DEFINED
    };

    enum {
        k_NUM_ENUMERATORS = 2
    };

    BSLMF_ASSERT(k_NUM_ENUMERATORS == e_DEFINED + 1);
};

                           // ======================
                           // struct OperatingSystem
                           // ======================

struct OperatingSystem {
    // This 'struct' provides a namespace for enumerating the set of operating
    // systems that this component supports in its determination of which
    // macros it defines.

    // TYPES
    enum Enum {
        e_AIX,
        e_CYGWIN,
        e_DARWIN,
        e_FREEBSD,
        e_LINUX,
        e_SOLARIS,
        e_SUNOS,
        e_WINDOWS
    };

    enum {
        k_NUM_ENUMERATORS = 8
    };

    BSLMF_ASSERT(k_NUM_ENUMERATORS == e_WINDOWS + 1);
};

                            // ===================
                            // struct PlatformUtil
                            // ===================

struct PlatformUtil {
    // This utility 'struct' provides a namespace for a suite of constants used
    // to identify all criteria of the current platform configuration that this
    // component uses to determine which macros to define.

    // CLASS DATA
    static const CpuBits::Enum k_CPU_BITS =
#if defined(BSLS_PLATFORM_CPU_32_BIT)
        CpuBits::e_32;
#elif defined(BSLS_PLATFORM_CPU_64_BIT)
        CpuBits::e_64;
#else
# error "'bdls_filesystemutil_unixplatform' does not support this platform."
#endif

    static const LargeFileMode::Enum k_LARGE_FILE_MODE =
#if defined(BSLS_PLATFORM_OS_AIX) && defined(_LARGE_FILE)
        LargeFileMode::e_DEFINED;
#elif defined(BSLS_PLATFORM_OS_AIX)
        LargeFileMode::e_NOT_DEFINED;
#elif defined(BSLS_PLATFORM_OS_CYGWIN)
        LargeFileMode::e_NOT_DEFINED;
#elif defined(BSLS_PLATFORM_OS_DARWIN)
        LargeFileMode::e_NOT_DEFINED;
#elif defined(BSLS_PLATFORM_OS_FREEBSD)
        LargeFileMode::e_NOT_DEFINED
#elif defined(BSLS_PLATFORM_OS_LINUX) && defined(_LARGEFILE64_SOURCE)
        LargeFileMode::e_DEFINED;
#elif defined(BSLS_PLATFORM_OS_LINUX)
        LargeFileMode::e_NOT_DEFINED;
#elif defined(BSLS_PLATFORM_OS_SOLARIS) && defined(_LARGEFILE64_SOURCE)
        LargeFileMode::e_DEFINED;
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
        LargeFileMode::e_NOT_DEFINED;
#elif defined(BSLS_PLATFORM_OS_SUNOS) && defined(_LARGEFILE64_SOURCE)
        LargeFileMode::e_DEFINED;
#elif defined(BSLS_PLATFORM_OS_SUNOS)
        LargeFileMode::e_NOT_DEFINED;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
        LargeFileMode::e_NOT_DEFINED;
#elif
#else
# error "'bdls_filesystemutil_unixplatform' does not support this platform."
#endif

    static const FileOffsetBits::Enum k_FILE_OFFSET_BITS =
#if defined(BSLS_PLATFORM_OS_AIX)
        FileOffsetBits::e_NOT_DEFINED;
#elif defined(BSLS_PLATFORM_OS_CYGWIN)
        FileOffsetBits::e_NOT_DEFINED;
#elif defined(BSLS_PLATFORM_OS_DARWIN)
        FileOffsetBits::e_NOT_DEFINED;
#elif defined(BSLS_PLATFORM_OS_LINUX) \
   && defined(_FILE_OFFSET_BITS)      \
   && _FILE_OFFSET_BITS == 64
        FileOffsetBits::e_64;
#elif defined(BSLS_PLATFORM_OS_LINUX)
        FileOffsetBits::e_NOT_DEFINED;
#elif defined(BSLS_PLATFORM_OS_SOLARIS) \
   && defined(_FILE_OFFSET_BITS)        \
   && _FILE_OFFSET_BITS == 64
        FileOffsetBits::e_64;
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
        FileOffsetBits::e_NOT_DEFINED;
#elif defined(BSLS_PLATFORM_OS_SUNOS) \
   && defined(_FILE_OFFSET_BITS)      \
   && _FILE_OFFSET_BITS == 64
        FileOffsetBits::e_64;
#elif defined(BSLS_PLATFORM_OS_SUNOS)
        FileOffsetBits::e_NOT_DEFINED;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
        FileOffsetBits::e_NOT_DEFINED;
#else
# error "'bdls_filesystemutil_unixplatform' does not support this platform."
#endif

    static const OperatingSystem::Enum k_OPERATING_SYSTEM =
#if defined(BSLS_PLATFORM_OS_AIX)
        OperatingSystem::e_AIX;
#elif defined(BSLS_PLATFORM_OS_CYGWIN)
        OperatingSystem::e_CYGWIN;
#elif defined(BSLS_PLATFORM_OS_DARWIN)
        OperatingSystem::e_DARWIN;
#elif defined(BSLS_PLATFORM_OS_FREEBSD)
        OperatingSystem::e_FREEBSD;
#elif defined(BSLS_PLATFORM_OS_LINUX)
        OperatingSystem::e_LINUX;
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
        OperatingSystem::e_SOLARIS;
#elif defined(BSLS_PLATFORM_OS_SUNOS)
        OperatingSystem::e_SUNOS;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
        OperatingSystem::e_WINDOWS;
#else
# error "'bdls_filesystemutil_unixplatform' does not support this platform."
#endif
};

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

    switch(test) { case 0:
      case 2: {
        // ------------------------------------------------------------------
        // TESTING MACRO EXCLUSIVITY
        //   This case verifies that this component provides exactly zero or
        //   one macro definition for the current platform configuration, and
        //   that the information conveyed in the macro: which offset type to
        //   use and the number of bits in that type, is accurate.
        //
        // Concerns:
        //: 1 Exactly one of the following macros is defined on Unix platforms:
        //:   'BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF',
        //:   'BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF', or
        //:   'BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF64'.
        //:
        //: 2 No macro is defined on Windows platforms.
        //:
        //: 3 if the '*_32_BIT_OFF' macro is defined, it expands to the literal
        //:   '1', the size of '::off_t' is 4 and its numeric range is
        //:   consistent with a 32-bit, 2's-complement, signed integral type.
        //:
        //: 4 If the '*_64_BIT_OFF' macro is defined, it expands to the literal
        //:   '1', the size of '::off_t' is 8 and its numeric range is
        //:   consistent with a 64-bit, 2's-complement, signed integral type.
        //:
        //: 5 If the '*_64_BIT_OFF64' macro is defined, it expands to the
        //:   literal '1', an '::off64_t' type is defined, its size is 8, and
        //:   its numeric range is consistent with a 64-bit, 2's-complement,
        //:   signed integral type.
        //
        // Plan:
        //: 1 For each macro 'M' under consideration:
        //:
        //:   1 Verify that if 'M' is defined, that no other macro under
        //:     consideration is defined, and that 'M' expands to '1'.
        //:
        //:   2 Verify that 'M's indicated offset type is an integral type.
        //:
        //:   3 Verify that 'M's indicated offset type has the exactly the
        //:     numeric range described in the corresponding concern.
        //:
        //:   4 Verify that 'M' is not defined if the current platform is a
        //:     Windows platform.
        //
        // Testing:
        //   BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF
        //   BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF
        //   BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF64
        // ------------------------------------------------------------------

        if (verbose) {
            bsl::cout << bsl::endl << "TESTING MACRO EXCLUSIVITY"
                      << bsl::endl << "=========================" << bsl::endl;
        }

#if defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF)
#define U_32_BIT_OFF 1
#else
#define U_32_BIT_OFF 0
#endif

#if defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF)
#define U_64_BIT_OFF 1
#else
#define U_64_BIT_OFF 0
#endif

#if defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF64)
#define U_64_BIT_OFF64 1
#else
#define U_64_BIT_OFF64 0
#endif

#if defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF)
        ASSERT(1 == BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF);
        ASSERT(1 == U_32_BIT_OFF);
        ASSERT(0 == U_64_BIT_OFF);
        ASSERT(0 == U_64_BIT_OFF64);

        ASSERT(bsl::is_integral< ::off_t>::value);
        ASSERT(4 == sizeof(::off_t));
        ASSERT(-0x7FFFFFFF - 1 == bsl::numeric_limits< ::off_t>::min());
        ASSERT( 0x7FFFFFFF     == bsl::numeric_limits< ::off_t>::max());
#endif

#if defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF)
        ASSERT(1 == BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF);
        ASSERT(0 == U_32_BIT_OFF);
        ASSERT(1 == U_64_BIT_OFF);
        ASSERT(0 == U_64_BIT_OFF64);

        ASSERT(bsl::is_integral< ::off_t>::value);
        ASSERT(8 == sizeof(::off_t));
        ASSERT(-0x7FFFFFFFFFFFFFFFLL - 1LL
                                      == bsl::numeric_limits< ::off_t>::min());
        ASSERT( 0x7FFFFFFFFFFFFFFFLL  == bsl::numeric_limits< ::off_t>::max());
#endif

#if defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF64)
        ASSERT(1 == BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF64);
        ASSERT(0 == U_32_BIT_OFF);
        ASSERT(0 == U_64_BIT_OFF);
        ASSERT(1 == U_64_BIT_OFF64);

        ASSERT(bsl::is_integral< ::off64_t>::value);
        ASSERT(8 == sizeof(::off64_t));
        ASSERT(-0x7FFFFFFFFFFFFFFFLL - 1LL
                                    == bsl::numeric_limits< ::off64_t>::min());
        ASSERT( 0x7FFFFFFFFFFFFFFFLL
                                    == bsl::numeric_limits< ::off64_t>::max());
#endif

#if defined(BSLS_PLATFORM_OS_UNIX)
        ASSERT(1 == U_32_BIT_OFF + U_64_BIT_OFF + U_64_BIT_OFF64);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
        ASSERT(0 == U_32_BIT_OFF + U_64_BIT_OFF + U_64_BIT_OFF64);
#else
# error "'bdls_filesystemutil_unixplatform' does not support this platform."
#endif

#undef U_32_BIT_OFF
#undef U_64_BIT_OFF
#undef U_64_BIT_OFF64
      } break;
      case 1: {
        // ------------------------------------------------------------------
        // TESTING MACRO SELECTION
        //   This case verifies that, of the 3 macros that this component
        //   conditionally defines, the correct macro is defined according to
        //   the current platform's configuration as specified by the component
        //   documentation.
        //
        // Concerns:
        //: t The decision procedure for which macro is defined for the current
        //:   platform is equivalent to the procedure specified by the table in
        //:   this component's documentation.
        //
        // Plan:
        //: 1 Construct a table 'T' that enumerates every supported combination
        //:   of operating system, cpu word width, "file-offset bits" macro
        //:   value, and "large file" macro value.
        //:
        //: 2 For each combination 'C' in 'T', also store booleans that
        //:   designate whether the '*_32_BIT_OFF', '*_64_BIT_OFF', or the
        //:   '*_64_BIT_OFF64' macros should be defined, according to the table
        //:   in the component documentation.
        //:
        //: 3 Using the platform configuration 'C' that 'u::PlatformUtil'
        //:   detects, look up the macro-definition booleans 'Bs' associated
        //:   with configuration 'C' in 'T'.
        //:
        //: 4 For each macro 'M' in the list of macros 'L', verify that 'M' is
        //:   defined if and only if its associated boolean in the set of
        //:   booleans 'Bs' is true; where 'L' is the list of macros:
        //:   '*_32_BIT_OFF', '*_64_BIT_OFF', and '*_64_BIT_OFF64'.
        //
        // Testing:
        //   BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF
        //   BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF
        //   BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF64
        // ------------------------------------------------------------------

        typedef u::CpuBits::Enum         CPU;
        typedef u::FileOffsetBits::Enum  FOB;
        typedef u::LargeFileMode::Enum   LFM;
        typedef u::OperatingSystem::Enum OPS;

        static const int NUM_CPU = u::CpuBits::k_NUM_ENUMERATORS;
        static const int NUM_FOB = u::FileOffsetBits::k_NUM_ENUMERATORS;
        static const int NUM_LFM = u::LargeFileMode::k_NUM_ENUMERATORS;
        static const int NUM_OPS = u::OperatingSystem::k_NUM_ENUMERATORS;

        static const CPU B32 = u::CpuBits::e_32;
        static const CPU B64 = u::CpuBits::e_64;

        static const FOB F__ = u::FileOffsetBits::e_NOT_DEFINED;
        static const FOB F64 = u::FileOffsetBits::e_64;

        static const LFM L__ = u::LargeFileMode::e_NOT_DEFINED;
        static const LFM LDF = u::LargeFileMode::e_DEFINED;

        static const OPS AIX = u::OperatingSystem::e_AIX;
        static const OPS CYG = u::OperatingSystem::e_CYGWIN;
        static const OPS DAR = u::OperatingSystem::e_DARWIN;
        static const OPS FRE = u::OperatingSystem::e_FREEBSD;
        static const OPS LIN = u::OperatingSystem::e_LINUX;
        static const OPS SOL = u::OperatingSystem::e_SOLARIS;
        static const OPS SUN = u::OperatingSystem::e_SUNOS;
        static const OPS WIN = u::OperatingSystem::e_WINDOWS;

        const struct {
            int  d_line;
            OPS  d_operatingSystem;
            CPU  d_cpuBits;
            FOB  d_fileOffsetBits;
            LFM  d_largeFileMode;
            bool d_defined64BitOff;
            bool d_defined64BitOff64;
            bool d_defined32BitOff;
          } DATA[] = {
              //                               '*_32_BIT_OFF'
              //                               --------------.
              //                            '*_64_BIT_OFF64'  \.
              //                            ----------------.  \.
              //                             '*_64_BIT_OFF'  \. \.
              //                             --------------.  \. \.
              // LINE  OS  CPU BITS OFFSET BITS LARGE FILES \. \. \.
              // ---- ---- -------- ----------- ----------- -- -- ---
              {  L_  , AIX,     B32,        F__,        L__, 0, 1, 0 },
              {  L_  , AIX,     B32,        F__,        LDF, 1, 0, 0 },
              {  L_  , AIX,     B32,        F64,        L__, 1, 0, 0 },
              {  L_  , AIX,     B32,        F64,        LDF, 1, 0, 0 },
              {  L_  , AIX,     B64,        F__,        L__, 1, 0, 0 },
              {  L_  , AIX,     B64,        F__,        LDF, 1, 0, 0 },
              {  L_  , AIX,     B64,        F64,        L__, 1, 0, 0 },
              {  L_  , AIX,     B64,        F64,        LDF, 1, 0, 0 },

              {  L_  , CYG,     B32,        F__,        L__, 1, 0, 0 },
              {  L_  , CYG,     B32,        F__,        LDF, 1, 0, 0 },
              {  L_  , CYG,     B32,        F64,        L__, 1, 0, 0 },
              {  L_  , CYG,     B32,        F64,        LDF, 1, 0, 0 },
              {  L_  , CYG,     B64,        F__,        L__, 1, 0, 0 },
              {  L_  , CYG,     B64,        F__,        LDF, 1, 0, 0 },
              {  L_  , CYG,     B64,        F64,        L__, 1, 0, 0 },
              {  L_  , CYG,     B64,        F64,        LDF, 1, 0, 0 },

              {  L_  , DAR,     B32,        F__,        L__, 1, 0, 0 },
              {  L_  , DAR,     B32,        F__,        LDF, 1, 0, 0 },
              {  L_  , DAR,     B32,        F64,        L__, 1, 0, 0 },
              {  L_  , DAR,     B32,        F64,        LDF, 1, 0, 0 },
              {  L_  , DAR,     B64,        F__,        L__, 1, 0, 0 },
              {  L_  , DAR,     B64,        F__,        LDF, 1, 0, 0 },
              {  L_  , DAR,     B64,        F64,        L__, 1, 0, 0 },
              {  L_  , DAR,     B64,        F64,        LDF, 1, 0, 0 },

              {  L_  , FRE,     B32,        F__,        L__, 1, 0, 0 },
              {  L_  , FRE,     B32,        F__,        LDF, 1, 0, 0 },
              {  L_  , FRE,     B32,        F64,        L__, 1, 0, 0 },
              {  L_  , FRE,     B32,        F64,        LDF, 1, 0, 0 },
              {  L_  , FRE,     B64,        F__,        L__, 1, 0, 0 },
              {  L_  , FRE,     B64,        F__,        LDF, 1, 0, 0 },
              {  L_  , FRE,     B64,        F64,        L__, 1, 0, 0 },
              {  L_  , FRE,     B64,        F64,        LDF, 1, 0, 0 },

              {  L_  , FRE,     B32,        F__,        L__, 1, 0, 0 },
              {  L_  , FRE,     B32,        F__,        LDF, 1, 0, 0 },
              {  L_  , FRE,     B32,        F64,        L__, 1, 0, 0 },
              {  L_  , FRE,     B32,        F64,        LDF, 1, 0, 0 },
              {  L_  , FRE,     B64,        F__,        L__, 1, 0, 0 },
              {  L_  , FRE,     B64,        F__,        LDF, 1, 0, 0 },
              {  L_  , FRE,     B64,        F64,        L__, 1, 0, 0 },
              {  L_  , FRE,     B64,        F64,        LDF, 1, 0, 0 },

              {  L_  , LIN,     B32,        F__,        L__, 0, 0, 1 },
              {  L_  , LIN,     B32,        F__,        LDF, 0, 1, 0 },
              {  L_  , LIN,     B32,        F64,        L__, 1, 0, 0 },
              {  L_  , LIN,     B32,        F64,        LDF, 1, 0, 0 },
              {  L_  , LIN,     B64,        F__,        L__, 1, 0, 0 },
              {  L_  , LIN,     B64,        F__,        LDF, 1, 0, 0 },
              {  L_  , LIN,     B64,        F64,        L__, 1, 0, 0 },
              {  L_  , LIN,     B64,        F64,        LDF, 1, 0, 0 },

              {  L_  , SOL,     B32,        F__,        L__, 0, 0, 1 },
              {  L_  , SOL,     B32,        F__,        LDF, 0, 1, 0 },
              {  L_  , SOL,     B32,        F64,        L__, 1, 0, 0 },
              {  L_  , SOL,     B32,        F64,        LDF, 1, 0, 0 },
              {  L_  , SOL,     B64,        F__,        L__, 1, 0, 0 },
              {  L_  , SOL,     B64,        F__,        LDF, 1, 0, 0 },
              {  L_  , SOL,     B64,        F64,        L__, 1, 0, 0 },
              {  L_  , SOL,     B64,        F64,        LDF, 1, 0, 0 },

              {  L_  , SUN,     B32,        F__,        L__, 0, 0, 1 },
              {  L_  , SUN,     B32,        F__,        LDF, 0, 1, 0 },
              {  L_  , SUN,     B32,        F64,        L__, 1, 0, 0 },
              {  L_  , SUN,     B32,        F64,        LDF, 1, 0, 0 },
              {  L_  , SUN,     B64,        F__,        L__, 1, 0, 0 },
              {  L_  , SUN,     B64,        F__,        LDF, 1, 0, 0 },
              {  L_  , SUN,     B64,        F64,        L__, 1, 0, 0 },
              {  L_  , SUN,     B64,        F64,        LDF, 1, 0, 0 },

              {  L_  , WIN,     B32,        F__,        L__, 0, 0, 0 },
              {  L_  , WIN,     B32,        F__,        LDF, 0, 0, 0 },
              {  L_  , WIN,     B32,        F64,        L__, 0, 0, 0 },
              {  L_  , WIN,     B32,        F64,        LDF, 0, 0, 0 },
              {  L_  , WIN,     B64,        F__,        L__, 0, 0, 0 },
              {  L_  , WIN,     B64,        F__,        LDF, 0, 0, 0 },
              {  L_  , WIN,     B64,        F64,        L__, 0, 0, 0 },
              {  L_  , WIN,     B64,        F64,        LDF, 0, 0, 0 }
          };

          const int NUM_DATA = sizeof DATA / sizeof *DATA;

          struct MacroDefinitions {
              bool d_defined64BitOff;
              bool d_defined64BitOff64;
              bool d_defined32BitOff;
          };

          MacroDefinitions MACRO_DEFINITIONS[NUM_OPS][NUM_CPU][NUM_FOB]
                                            [NUM_LFM];

          for (int i = 0; i != NUM_DATA; ++i) {
              const OPS  OPERATING_SYSTEM     = DATA[i].d_operatingSystem;
              const CPU  CPU_BITS             = DATA[i].d_cpuBits;
              const FOB  FILE_OFFSET_BITS     = DATA[i].d_fileOffsetBits;
              const LFM  LARGE_FILE_MODE      = DATA[i].d_largeFileMode;
              const bool DEFINED_64_BIT_OFF   = DATA[i].d_defined64BitOff;
              const bool DEFINED_64_BIT_OFF64 = DATA[i].d_defined64BitOff64;
              const bool DEFINED_32_BIT_OFF   = DATA[i].d_defined32BitOff;

              MacroDefinitions& macroDefinitions =
                  MACRO_DEFINITIONS[OPERATING_SYSTEM][CPU_BITS]
                                   [FILE_OFFSET_BITS][LARGE_FILE_MODE];
              macroDefinitions.d_defined64BitOff   = DEFINED_64_BIT_OFF;
              macroDefinitions.d_defined64BitOff64 = DEFINED_64_BIT_OFF64;
              macroDefinitions.d_defined32BitOff   = DEFINED_32_BIT_OFF;
          }

          const MacroDefinitions& EXPECTED_MACRO_DEFINITIONS =
              MACRO_DEFINITIONS[u::PlatformUtil::k_OPERATING_SYSTEM]
                               [u::PlatformUtil::k_CPU_BITS]
                               [u::PlatformUtil::k_FILE_OFFSET_BITS]
                               [u::PlatformUtil::k_LARGE_FILE_MODE];

          const bool EXPECT_32_BIT_OFF =
              EXPECTED_MACRO_DEFINITIONS.d_defined32BitOff;
          const bool EXPECT_64_BIT_OFF =
              EXPECTED_MACRO_DEFINITIONS.d_defined64BitOff;
          const bool EXPECT_64_BIT_OFF64 =
              EXPECTED_MACRO_DEFINITIONS.d_defined64BitOff64;

#if defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_32_BIT_OFF)
          ASSERT(1 == EXPECT_32_BIT_OFF);
          ASSERT(0 == EXPECT_64_BIT_OFF);
          ASSERT(0 == EXPECT_64_BIT_OFF64);
#elif defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF)
          ASSERT(0 == EXPECT_32_BIT_OFF);
          ASSERT(1 == EXPECT_64_BIT_OFF);
          ASSERT(0 == EXPECT_64_BIT_OFF64);
#elif defined(BDLS_FILESYSTEMUTIL_UNIXPLATFORM_64_BIT_OFF64)
          ASSERT(0 == EXPECT_32_BIT_OFF);
          ASSERT(0 == EXPECT_64_BIT_OFF);
          ASSERT(1 == EXPECT_64_BIT_OFF64);
#else
          ASSERT(0 == EXPECT_32_BIT_OFF);
          ASSERT(0 == EXPECT_64_BIT_OFF);
          ASSERT(0 == EXPECT_64_BIT_OFF64);
#endif
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
