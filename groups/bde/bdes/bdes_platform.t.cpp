// bdes_platform.t.cpp    -*-C++-*-

#include <bdes_platform.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------

//==========================================================================
//                             TEST PLAN
//--------------------------------------------------------------------------
//                            * Overview *
// Since this component implements CPP macro's, which may or may not be
// defined, there is not too much to test in this driver.  Since correctness
// will be affected by compile-time switches during the build process,
// any compile-time tests we come up with should probably reside directly in
// the header or implementation file.
//--------------------------------------------------------------------------
// [ 1] Ensure that exactly one of each CMP type is set.
// [ 1] Ensure that exactly one of each OS type is set.
// [ 1] Ensure that exactly one of each CPU type is set.
// [ 1] Ensure that at most one of each CPU subtype is set.
// [ 1] For each category, ensure MINOR_NUMBER set -> MAJOR_NUMBER set.
// [ 1] For the OS, type ensure MAJOR_NUMBER set -> SUBTYPE set.
//==========================================================================

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------
        // MINIMAL DEFINITION TEST:
        //   We want to make sure that exactly one each of OS, PROCESSOR, and
        //   COMPILER type is set.  We also want to make sure that at most one
        //   subtype of OS is set.  Finally we want to make sure that a minor
        //   version implies a major version, and that for OS, a major version
        //   implies a subtype.
        // Testing:
        //   Ensure that exactly one of each OS type is set.
        //   Ensure that exactly one of each CMP type is set.
        //   Ensure that exactly one of each CPU type is set.
        //   Ensure that at most one of each CPU subtype is set.
        //   For each category, ensure MINOR_NUMBER set -> MAJOR_NUMBER set.
        //   For the OS type, ensure MAJOR_NUMBER set -> SUBTYPE set.
        // --------------------------------------------------------

        if (verbose) cout << endl << "Minimal Definition Test" <<
                             endl << "-----------------------" << endl;

        // Compile-time tests for this case are located in the validation
        // section of this component's header file.

        struct OsCpu {
            bsls_Platform::Os  osType;
            bsls_Platform::Cpu cpuType;
#if BDE_ALLOW_DEPRECATED_bdes_Platform_ALLCAPS_CPU
            bsls_Platform::CPU CPUType;
#endif
        };

        if (verbose) {

            cout << endl << "Print Defined Symbols:" << endl;

            cout << endl << "Print CMP-related Symbols:" << endl;

#define MACRO_TESTEQ(X, Y) \
        ASSERT(Y == X);\
        cout << "\t"#X" = " << X << endl;

#define MACRO_TESTGT(X, Y) \
        ASSERT(Y <= X);\
        cout << "\t"#X" = " << X << endl;

            #if defined(BSLS_PLATFORM_CMP_IBM)
                MACRO_TESTGT(BSLS_PLATFORM_CMP_IBM, 0);
            #endif
            #if defined(BDES_PLATFORM_CMP_BLP)
                MACRO_TESTGT(BDES_PLATFORM_CMP_BLP, 0);
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

            cout << endl << "Print OS-related Symbols:" << endl;

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
            #if defined(BDES_PLATFORM_OS_FREEBSD)
                MACRO_TESTEQ(BDES_PLATFORM_OS_FREEBSD, 1);
            #endif
            #if defined(BDES_PLATFORM_OS_DARWIN)
                MACRO_TESTEQ(BDES_PLATFORM_OS_DARWIN, 1);
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


            cout << endl << "Print CPU-related Symbols:" << endl;
            #if defined(BSLS_PLATFORM_CPU_88000)
                MACRO_TESTEQ(BSLS_PLATFORM_CPU_88000, 1);
            #endif
            #if defined(BSLS_PLATFORM_CPU_ALPHA)
                MACRO_TESTEQ(BSLS_PLATFORM_CPU_ALPHA, 1);
            #endif
            #if defined(BSLS_PLATFORM_CPU_HPPA)
                MACRO_TESTEQ(BSLS_PLATFORM_CPU_HPPA, 1);
            #endif
            // Deprecated but still exposed, and used by some
            // components.  Thus, of interest.
            #if defined(BSLS_PLATFORM_CPU_INTEL)
                MACRO_TESTEQ(BSLS_PLATFORM_CPU_INTEL, 1);
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
        }

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
