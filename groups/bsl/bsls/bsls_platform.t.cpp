// bsls_platform.t.cpp                                                -*-C++-*-

#include <bsls_platform.h>

#include <iostream>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>     // 'strcmp', 'strlen'


using namespace BloombergLP;
using namespace std;

// ============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static
void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                   SUPPORTING FUNCTIONS USED FOR TESTING
// ----------------------------------------------------------------------------

static
bool isBigEndian()
    // Return 'true' if this machine is observed to be big endian, and 'false'
    // otherwise.  Internally, assert that this machine is observed to be
    // either big endian or little endian.
{
    union U {
        int d_int;
        char d_char[sizeof(int)];
    } u;

    u.d_int = 0x1;

    if (u.d_char[0] == 0x1) {
        return false;                                                 // RETURN
    }
    else if (u.d_char[sizeof(int) - 1] == 0x1) {
        return true;                                                  // RETURN
    }
    else {
        ASSERT(0);      // Neither big endian nor little endian!
    }
    return false;
}

// ----------------------------------------------------------------------------

static
bool isLittleEndian()
    // Return 'true' if this machine is observed to be little endian, and
    // 'false' otherwise.  Internally, assert that this machine is observed to
    // be either big endian or little endian.
{
    union U {
        int d_int;
        char d_char[sizeof(int)];
    } u;

    u.d_int = 0x1;

    if (u.d_char[0] == 0x1) {
        return true;                                                  // RETURN
    }
    else if (u.d_char[sizeof(int) - 1] == 0x1) {
        return false;                                                 // RETURN
    }
    else {
        ASSERT(0);      // Neither big endian nor little endian!
    }
    return false;
}

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                            * Overview *
// Since this component implements CPP macro's, which may or may not be
// defined, there is not too much to test in this driver.  Since correctness
// will be affected by compile-time switches during the build process,
// any compile-time tests we come up with should probably reside directly in
// the header or implementation file.
// ----------------------------------------------------------------------------
// [ 1] Ensure that exactly one of each CMP type is set.
// [ 1] Ensure that exactly one of each OS type is set.
// [ 1] Ensure that exactly one of each CPU type is set.
// [ 1] Ensure that at most one of each CPU subtype is set.
// [ 1] For each category, ensure MINOR_NUMBER set -> MAJOR_NUMBER set.
// [ 1] For the OS, type ensure MAJOR_NUMBER set -> SUBTYPE set.
// [ 2] BSLS_PLATFORM_IS_LITTLE_ENDIAN
// [ 2] BSLS_PLATFORM_IS_BIG_ENDIAN
// [ 3] BSLS_PLATFORM_NO_64_BIT_LITERALS
// ============================================================================

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    (void) veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // DUMPING VALUES OF OLD '__' LITERALS
        //
        // Concerns:
        //   It is hard to tell which old literals are enabled and which
        //   aren't.
        //
        // Plan:
        //   Dump them out.
        // --------------------------------------------------------------------

        if (verbose) cout << "Dump out BSLS_PLATFORM_*\n"
                             "========================\n";

        if (!verbose) break;

        #ifdef BSLS_PLATFORM_CMP_AIX
          printf("BSLS_PLATFORM_CMP_AIX == 1\n");
        #else
          printf("BSLS_PLATFORM_CMP_AIX == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CMP_CLANG
          printf("BSLS_PLATFORM_CMP_CLANG == 1\n");
        #else
          printf("BSLS_PLATFORM_CMP_CLANG == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CMP_EDG
          printf("BSLS_PLATFORM_CMP_EDG == 1\n");
        #else
          printf("BSLS_PLATFORM_CMP_EDG == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CMP_GNU
          printf("BSLS_PLATFORM_CMP_GNU == 1\n");
        #else
          printf("BSLS_PLATFORM_CMP_GNU == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CMP_HP
          printf("BSLS_PLATFORM_CMP_HP == 1\n");
        #else
          printf("BSLS_PLATFORM_CMP_HP == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CMP_IBM
          printf("BSLS_PLATFORM_CMP_IBM == 1\n");
        #else
          printf("BSLS_PLATFORM_CMP_IBM == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CMP_MSVC
          printf("BSLS_PLATFORM_CMP_MSVC == 1\n");
        #else
          printf("BSLS_PLATFORM_CMP_MSVC == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CMP_SUN
          printf("BSLS_PLATFORM_CMP_SUN == 1\n");
        #else
          printf("BSLS_PLATFORM_CMP_SUN == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CMP_VERSION
          printf("BSLS_PLATFORM_CMP_VERSION == 1\n");
        #else
          printf("BSLS_PLATFORM_CMP_VERSION == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CMP_VER_MAJOR
          printf("BSLS_PLATFORM_CMP_VER_MAJOR == 1\n");
        #else
          printf("BSLS_PLATFORM_CMP_VER_MAJOR == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CPU_32_BIT
          printf("BSLS_PLATFORM_CPU_32_BIT == 1\n");
        #else
          printf("BSLS_PLATFORM_CPU_32_BIT == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CPU_64_BIT
          printf("BSLS_PLATFORM_CPU_64_BIT == 1\n");
        #else
          printf("BSLS_PLATFORM_CPU_64_BIT == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CPU_88000
          printf("BSLS_PLATFORM_CPU_88000 == 1\n");
        #else
          printf("BSLS_PLATFORM_CPU_88000 == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CPU_ALPHA
          printf("BSLS_PLATFORM_CPU_ALPHA == 1\n");
        #else
          printf("BSLS_PLATFORM_CPU_ALPHA == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CPU_HPPA
          printf("BSLS_PLATFORM_CPU_HPPA == 1\n");
        #else
          printf("BSLS_PLATFORM_CPU_HPPA == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CPU_IA64
          printf("BSLS_PLATFORM_CPU_IA64 == 1\n");
        #else
          printf("BSLS_PLATFORM_CPU_IA64 == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CPU_INTEL
          printf("BSLS_PLATFORM_CPU_INTEL == 1\n");
        #else
          printf("BSLS_PLATFORM_CPU_INTEL == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CPU_MIPS
          printf("BSLS_PLATFORM_CPU_MIPS == 1\n");
        #else
          printf("BSLS_PLATFORM_CPU_MIPS == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CPU_POWERPC
          printf("BSLS_PLATFORM_CPU_POWERPC == 1\n");
        #else
          printf("BSLS_PLATFORM_CPU_POWERPC == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CPU_SPARC
          printf("BSLS_PLATFORM_CPU_SPARC == 1\n");
        #else
          printf("BSLS_PLATFORM_CPU_SPARC == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CPU_SPARC_32
          printf("BSLS_PLATFORM_CPU_SPARC_32 == 1\n");
        #else
          printf("BSLS_PLATFORM_CPU_SPARC_32 == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CPU_SPARC_V9
          printf("BSLS_PLATFORM_CPU_SPARC_V9 == 1\n");
        #else
          printf("BSLS_PLATFORM_CPU_SPARC_V9 == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CPU_VER_MAJOR
          printf("BSLS_PLATFORM_CPU_VER_MAJOR == 1\n");
        #else
          printf("BSLS_PLATFORM_CPU_VER_MAJOR == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CPU_VER_MINOR
          printf("BSLS_PLATFORM_CPU_VER_MINOR == 1\n");
        #else
          printf("BSLS_PLATFORM_CPU_VER_MINOR == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CPU_X86
          printf("BSLS_PLATFORM_CPU_X86 == 1\n");
        #else
          printf("BSLS_PLATFORM_CPU_X86 == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_CPU_X86_64
          printf("BSLS_PLATFORM_CPU_X86_64 == 1\n");
        #else
          printf("BSLS_PLATFORM_CPU_X86_64 == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
          printf("BSLS_PLATFORM_IS_BIG_ENDIAN == 1\n");
        #else
          printf("BSLS_PLATFORM_IS_BIG_ENDIAN == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
          printf("BSLS_PLATFORM_IS_LITTLE_ENDIAN == 1\n");
        #else
          printf("BSLS_PLATFORM_IS_LITTLE_ENDIAN == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_NO_64_BIT_LITERALS
          printf("BSLS_PLATFORM_NO_64_BIT_LITERALS == 1\n");
        #else
          printf("BSLS_PLATFORM_NO_64_BIT_LITERALS == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_OS_AIX
          printf("BSLS_PLATFORM_OS_AIX == 1\n");
        #else
          printf("BSLS_PLATFORM_OS_AIX == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_OS_CYGWIN
          printf("BSLS_PLATFORM_OS_CYGWIN == 1\n");
        #else
          printf("BSLS_PLATFORM_OS_CYGWIN == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_OS_DARWIN
          printf("BSLS_PLATFORM_OS_DARWIN == 1\n");
        #else
          printf("BSLS_PLATFORM_OS_DARWIN == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_OS_FREEBSD
          printf("BSLS_PLATFORM_OS_FREEBSD == 1\n");
        #else
          printf("BSLS_PLATFORM_OS_FREEBSD == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_OS_HPUX
          printf("BSLS_PLATFORM_OS_HPUX == 1\n");
        #else
          printf("BSLS_PLATFORM_OS_HPUX == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_OS_LINUX
          printf("BSLS_PLATFORM_OS_LINUX == 1\n");
        #else
          printf("BSLS_PLATFORM_OS_LINUX == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_OS_SOLARIS
          printf("BSLS_PLATFORM_OS_SOLARIS == 1\n");
        #else
          printf("BSLS_PLATFORM_OS_SOLARIS == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_OS_SUBTYPE_COUNT
          printf("BSLS_PLATFORM_OS_SUBTYPE_COUNT == 1\n");
        #else
          printf("BSLS_PLATFORM_OS_SUBTYPE_COUNT == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_OS_SUNOS
          printf("BSLS_PLATFORM_OS_SUNOS == 1\n");
        #else
          printf("BSLS_PLATFORM_OS_SUNOS == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_OS_UNIX
          printf("BSLS_PLATFORM_OS_UNIX == 1\n");
        #else
          printf("BSLS_PLATFORM_OS_UNIX == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_OS_VER_MAJOR
          printf("BSLS_PLATFORM_OS_VER_MAJOR == 1\n");
        #else
          printf("BSLS_PLATFORM_OS_VER_MAJOR == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_OS_VER_MINOR
          printf("BSLS_PLATFORM_OS_VER_MINOR == 1\n");
        #else
          printf("BSLS_PLATFORM_OS_VER_MINOR == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_OS_WIN2K
          printf("BSLS_PLATFORM_OS_WIN2K == 1\n");
        #else
          printf("BSLS_PLATFORM_OS_WIN2K == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_OS_WIN9X
          printf("BSLS_PLATFORM_OS_WIN9X == 1\n");
        #else
          printf("BSLS_PLATFORM_OS_WIN9X == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_OS_WINDOWS
          printf("BSLS_PLATFORM_OS_WINDOWS == 1\n");
        #else
          printf("BSLS_PLATFORM_OS_WINDOWS == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_OS_WINNT
          printf("BSLS_PLATFORM_OS_WINNT == 1\n");
        #else
          printf("BSLS_PLATFORM_OS_WINNT == 0\n");
        #endif
        #ifdef BSLS_PLATFORM_OS_WINXP
          printf("BSLS_PLATFORM_OS_WINXP == 1\n");
        #else
          printf("BSLS_PLATFORM_OS_WINXP == 0\n");
        #endif
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 64-BIT LITERALS:
        //
        // Concerns:
        //   Since the actual flag indicates the lack of support for
        //   64-bit integer literals, the only way to test the flag is
        //   for the compiler to fail.  Therefore the test will check for
        //   the absence of the flag and attempt to assign 64-bit literals
        //   to a variable, ensuring the compile-time macro for support of
        //   64-bit integer literals agrees with the capability of the
        //   compiler.
        //
        // Plan:
        //   - Assign both signed and unsigned 64-bit integer literals
        //     to variables of each type.
        //   - Verify that the compiler does not truncate the assignment or
        //     the constant by splitting the constant into 2 32-bit constants
        //     and combining them using logical operations into another
        //     64-bit value.
        //   - Verify the constructed value is equal to the 64-bit value
        //     directly assigned.
        //   - Verify no truncation is occurring by logically masking
        //     and shifting the 64-bit value with the 32-bit lo and hi words.
        //
        // Testing:
        //   BSLS_PLATFORM_NO_64_BIT_LITERALS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 64-Bit Integer Constant Support" << endl
                          << "=======================================" << endl;

        if (veryVerbose) cout << "64-bit integer constants supported." << endl;

#ifdef BSLS_PLATFORM_OS_WINDOWS
        typedef          __int64   T;  // Int64;
        typedef unsigned __int64   U;  // Uint64;
#else
        typedef          long long T;  //Int64;
        typedef unsigned long long U;  //Uint64;
#endif

        T i, iHi, iLo, iTest;
        U u, uHi, uLo, uTest;

#if !defined(BSLS_PLATFORM_NO_64_BIT_LITERALS)
        i = 9223372036854775807;  // 0x7FFFFFFFFFFFFFFF
        u = 9223372036854775809;  // 0x8000000000000001

        ASSERT(i == 0x7FFFFFFFFFFFFFFF);
        ASSERT(u == 0x8000000000000001);
#else
        i = 9223372036854775807LL;   // 0x7FFFFFFFFFFFFFFF
        u = 9223372036854775809uLL;  // 0x8000000000000001

        ASSERT(i == 0x7FFFFFFFFFFFFFFFLL);
        ASSERT(u == 0x8000000000000001uLL);
#endif

        // Generate test values in 32-bit parts.

        iHi   = 0x7FFFFFFF; iLo = 0xFFFFFFFF;
        iTest = iHi << 32 | iLo;
        ASSERT(               i == iTest);
        ASSERT((i & 0xFFFFFFFF) == iLo);
        ASSERT(         i >> 32 == iHi);

        uHi   = 0x80000000; uLo = 0x00000001;
        uTest = uHi << 32 | uLo;
        ASSERT(                u == uTest);
        ASSERT((u & 0x0FFFFFFFF) == uLo);
        ASSERT(          u >> 32 == uHi);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BIG ENDIAN and LITTLE ENDIAN:
        //
        // Concerns:
        //   1. The macros BSLS_PLATFORM_IS_BIG_ENDIAN and
        //      BSLS_PLATFORM_IS_LITTLE_ENDIAN must have boolean values.
        //   2. The macros BSLS_PLATFORM_IS_BIG_ENDIAN and
        //      BSLS_PLATFORM_IS_LITTLE_ENDIAN are assigned at compile
        //      time based on the platform (see overview above).  If any
        //      one of the flags or inferences is wrong, the "endian-ness"
        //      of a given platform could be wrong.  Fortunately it is
        //      possible to make run-time determination of a platform's
        //      "endian-ness" by using a union.  Unfortunately such a test is
        //      more expensive than checking a flag.  However, such a
        //      function is perfect for a test driver.
        //
        // Plan:
        //   First ensure the values for the endian macros return boolean
        //   values.  Next, ensure the compile-time macros and test functions
        //   agree with the values calculated at runtime.
        //
        // Testing:
        //   BSLS_PLATFORM_IS_LITTLE_ENDIAN
        //   BSLS_PLATFORM_IS_BIG_ENDIAN
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Little-Endian & Big-Endian Macros" << endl
                 << "=========================================" << endl;

        if (veryVerbose) {
            cout << "This platform is "
                 << (::isBigEndian() ? "BIG ENDIAN" : "LITTLE ENDIAN")
                 << '.' << endl;
        }

        // Make sure that run-time and compile-time endian-ness match.

#if defined(BSLS_PLATFORM_IS_BIG_ENDIAN)
        ASSERT(::isBigEndian()    == BSLS_PLATFORM_IS_BIG_ENDIAN);
#endif
#if defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
        ASSERT(::isLittleEndian() ==  BSLS_PLATFORM_IS_LITTLE_ENDIAN);
#endif

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // MINIMAL DEFINITION TEST:
        //
        // Plan:
        //   We want to make sure that exactly one each of OS, PROCESSOR,
        //   COMPILER and ENDIAN type is set.  We also want to make sure that
        //   at most one subtype of OS is set.  Furthermore, we want to make
        //   sure that a minor version implies a major version, and that for
        //   OS, a major version implies a subtype.  Finally, we want to make
        //   sure that one endian-ness type is defined for the platform.
        //
        // Testing:
        //   Ensure that exactly one of each OS type is set.
        //   Ensure that exactly one of each CMP type is set.
        //   Ensure that exactly one of each CPU type is set.
        //   Ensure that at most one of each CPU subtype is set.
        //   For each category, ensure MINOR_NUMBER set -> MAJOR_NUMBER set.
        //   For the OS type, ensure MAJOR_NUMBER set -> SUBTYPE set.
        //   For the ENDIAN type, ensure one is set.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Minimal Definition Test" <<
                             endl << "=======================" << endl;

        // Compile-time tests for this case are located in the validation
        // section of this component's header file.

        struct OsCpu {
            bsls::Platform::Os  osType;
            bsls::Platform::Cpu cpuType;
        };

        if (veryVerbose) {
            cout << endl << "Print Defined Symbols"      << endl;
            cout << endl << "Print CMP-related Symbols:" << endl;
        }

#define MACRO_TESTEQ(X, Y)                                    \
        ASSERT(Y == X);                                       \
        if (veryVerbose) cout << "\t"#X" = " << X << endl;

#define MACRO_TESTGT(X, Y)                                    \
        ASSERT(Y <= X);                                       \
        if (veryVerbose) cout << "\t"#X" = " << X << endl;

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

        #if defined(BSLS_PLATFORM_CMP_IBM)
            MACRO_TESTGT(BSLS_PLATFORM_CMP_IBM, 0);
        #endif
        #if defined(BSLS_PLATFORM_CMP_IBM)
            MACRO_TESTGT(BSLS_PLATFORM_CMP_IBM, 0);
        #endif
        #if defined(BSLS_PLATFORM_CMP_BLP)
            MACRO_TESTGT(BSLS_PLATFORM_CMP_BLP, 0);
        #endif
        #if defined(BSLS_PLATFORM_CMP_CLANG)
            MACRO_TESTGT(BSLS_PLATFORM_CMP_CLANG, 0);
        #endif
        #if defined(BSLS_PLATFORM_CMP_GNU)
            MACRO_TESTGT(BSLS_PLATFORM_CMP_GNU, 0);
        #endif
        #if defined(BSLS_PLATFORM_CMP_HP)
            MACRO_TESTGT(BSLS_PLATFORM_CMP_HP, 0);
        #endif
        #if defined(BSLS_PLATFORM_CMP_MSVC)
            MACRO_TESTGT(BSLS_PLATFORM_CMP_MSVC, 0);
        #endif
        #if defined(BSLS_PLATFORM_CMP_SUN)
            MACRO_TESTGT(BSLS_PLATFORM_CMP_SUN, 0);
        #endif
        #if defined(BSLS_PLATFORM_CMP_VER_MAJOR)
            MACRO_TESTGT(BSLS_PLATFORM_CMP_VER_MAJOR, 0);
        #endif
        #if defined(BSLS_PLATFORM_CMP_VER_MINOR)
            MACRO_TESTGT(BSLS_PLATFORM_CMP_VER_MINOR, 0);
        #endif

        if (veryVerbose) cout << endl << "Print OS-related Symbols:" << endl;

        #if defined(BSLS_PLATFORM_OS_UNIX)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_UNIX, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_AIX)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_AIX, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_DGUX)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_DGUX, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_HPUX)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_HPUX, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_CYGWIN)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_CYGWIN, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_LINUX)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_LINUX, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_FREEBSD)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_FREEBSD, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_DARWIN)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_DARWIN, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_SOLARIS)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_SOLARIS, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_SUNOS)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_SUNOS, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_WINDOWS)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_WINDOWS, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_WIN9X)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_WIN9X, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_WINNT)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_WINNT, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_WIN2K)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_WIN2K, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_WINXP)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_WINXP, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_VER_MAJOR)
            MACRO_TESTGT(BSLS_PLATFORM_OS_VER_MAJOR, 0);
        #endif
        #if defined(BSLS_PLATFORM_OS_VER_MINOR)
            MACRO_TESTGT(BSLS_PLATFORM_OS_VER_MINOR, 0);
        #endif

        if (veryVerbose) cout << endl << "Print CPU-related Symbols:" << endl;

        #if defined(BSLS_PLATFORM_CPU_88000)
            MACRO_TESTEQ(BSLS_PLATFORM_CPU_88000, 1);
        #endif
        #if defined(BSLS_PLATFORM_CPU_ALPHA)
            MACRO_TESTEQ(BSLS_PLATFORM_CPU_ALPHA, 1);
        #endif
        #if defined(BSLS_PLATFORM_CPU_HPPA)
            MACRO_TESTEQ(BSLS_PLATFORM_CPU_HPPA, 1);
        #endif
        #if defined(BSLS_PLATFORM_CPU_X86)
            MACRO_TESTEQ(BSLS_PLATFORM_CPU_X86, 1);
        #endif
        #if defined(BSLS_PLATFORM_CPU_X86_64)
            MACRO_TESTEQ(BSLS_PLATFORM_CPU_X86_64, 1);
        #endif
        #if defined(BSLS_PLATFORM_CPU_IA64)
            MACRO_TESTEQ(BSLS_PLATFORM_CPU_IA64, 1);
        #endif
        #if defined(BSLS_PLATFORM_CPU_MIPS)
            MACRO_TESTEQ(BSLS_PLATFORM_CPU_MIPS, 1);
        #endif
        #if defined(BSLS_PLATFORM_CPU_POWERPC)
            MACRO_TESTEQ(BSLS_PLATFORM_CPU_POWERPC, 1);
        #endif
        #if defined(BSLS_PLATFORM_CPU_SPARC)
            MACRO_TESTEQ(BSLS_PLATFORM_CPU_SPARC, 1);
        #endif
        #if defined(BSLS_PLATFORM_CPU_32_BIT)
            MACRO_TESTEQ(BSLS_PLATFORM_CPU_32_BIT, 1);
        #endif
        #if defined(BSLS_PLATFORM_CPU_64_BIT)
            MACRO_TESTEQ(BSLS_PLATFORM_CPU_64_BIT, 1);
        #endif
        #if defined(BSLS_PLATFORM_CPU_VER_MAJOR)
            MACRO_TESTGT(BSLS_PLATFORM_CPU_VER_MAJOR, 0);
        #endif
        #if defined(BSLS_PLATFORM_CPU_VER_MINOR)
            MACRO_TESTGT(BSLS_PLATFORM_CPU_VER_MINOR, 0);
        #endif

        if (veryVerbose) cout << endl << "Print endian-ness symbols:" << endl;

        #if defined(BSLS_PLATFORM_IS_BIG_ENDIAN)
            MACRO_TESTEQ(BSLS_PLATFORM_IS_BIG_ENDIAN, 1);
        #endif
        #if defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
            MACRO_TESTEQ(BSLS_PLATFORM_IS_LITTLE_ENDIAN, 1);
        #endif

        if (veryVerbose) cout << endl << "Print inlining symbol:" << endl;

        if (veryVerbose) cout << "\t"
                              << STRINGIFY2(BSLS_PLATFORM_AGGRESSIVE_INLINE)
                              << " = \""
                              << STRINGIFY(BSLS_PLATFORM_AGGRESSIVE_INLINE)
                              << "\""
                              << endl;

        #if (defined(BSLS_PLATFORM_CMP_AIX) || defined(BSLS_PLATFORM_CMP_SUN))\
            && !defined(BDE_BUILD_TARGET_AGGRESSIVE_INLINE)
        ASSERT(0 == strlen(STRINGIFY(BSLS_PLATFORM_AGGRESSIVE_INLINE)));
        #else
        ASSERT(0 == strcmp(STRINGIFY(BSLS_PLATFORM_AGGRESSIVE_INLINE),
                                     "inline"));
        #endif
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
