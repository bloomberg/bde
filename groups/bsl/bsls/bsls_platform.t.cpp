// bsls_platform.t.cpp                                                -*-C++-*-

#include <bsls_platform.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>     // 'strcmp', 'strlen'


using namespace BloombergLP;
using namespace std;

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

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define STRINGIFY2(...) "" #__VA_ARGS__
#define STRINGIFY(a) STRINGIFY2(a)

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
// will be affected by compile-time switches during the build process, any
// compile-time tests we come up with should probably reside directly in the
// header or implementation file.
// ----------------------------------------------------------------------------
// [ 1] Ensure that exactly one of each CMP type is set.
// [ 1] Ensure that exactly one of each OS type is set.
// [ 1] Ensure that exactly one of each CPU type is set.
// [ 1] Ensure that at most one of each CPU subtype is set.
// [ 1] For each category, ensure MINOR_NUMBER set -> MAJOR_NUMBER set.
// [ 1] For the OS, type ensure MAJOR_NUMBER set -> SUBTYPE set.
// [ 1] For the ENDIAN type, ensure one is set.
// [ 2] BSLS_PLATFORM_IS_LITTLE_ENDIAN
// [ 2] BSLS_PLATFORM_IS_BIG_ENDIAN
// [ 3] BSLS_PLATFORM_NO_64_BIT_LITERALS
// ============================================================================

// ============================================================================
//                              HELPER FUNCTIONS
// ----------------------------------------------------------------------------

static void printFlags()
    // Print a diagnostic message to standard output if any of the preprocessor
    // flags of interest are defined, and their value if a value had been set.
    // An "Enter" and "Leave" message is printed unconditionally so there is
    // some report even if all of the flags are undefined.
{
    printf("printFlags: Enter\n");

    printf("\n  printFlags: bsls_platform Macros\n");

    printf("\n  BSLS_PLATFORM_AGGRESSIVE_INLINE: ");
#ifdef BSLS_PLATFORM_AGGRESSIVE_INLINE
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_AGGRESSIVE_INLINE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_AIX: ");
#ifdef BSLS_PLATFORM_CMP_AIX
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_AIX) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_CLANG: ");
#ifdef BSLS_PLATFORM_CMP_CLANG
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_CLANG) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_EDG: ");
#ifdef BSLS_PLATFORM_CMP_EDG
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_EDG) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_GNU: ");
#ifdef BSLS_PLATFORM_CMP_GNU
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_GNU) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_HP: ");
#ifdef BSLS_PLATFORM_CMP_HP
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_HP) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_IBM: ");
#ifdef BSLS_PLATFORM_CMP_IBM
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_IBM) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_MSVC: ");
#ifdef BSLS_PLATFORM_CMP_MSVC
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_MSVC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_SUN: ");
#ifdef BSLS_PLATFORM_CMP_SUN
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_SUN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_VERSION: ");
#ifdef BSLS_PLATFORM_CMP_VERSION
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_VERSION) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_VER_MAJOR: ");
#ifdef BSLS_PLATFORM_CMP_VER_MAJOR
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_VER_MAJOR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_COMPILER_ERROR: ");
#ifdef BSLS_PLATFORM_COMPILER_ERROR
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_COMPILER_ERROR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_32_BIT: ");
#ifdef BSLS_PLATFORM_CPU_32_BIT
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_32_BIT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_64_BIT: ");
#ifdef BSLS_PLATFORM_CPU_64_BIT
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_64_BIT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_ALPHA: ");
#ifdef BSLS_PLATFORM_CPU_ALPHA
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_ALPHA) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_ARM: ");
#ifdef BSLS_PLATFORM_CPU_ARM
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_ARM) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_ARM_V5: ");
#ifdef BSLS_PLATFORM_CPU_ARM_V5
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_ARM_V5) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_ARM_V6: ");
#ifdef BSLS_PLATFORM_CPU_ARM_V6
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_ARM_V6) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_ARM_V7: ");
#ifdef BSLS_PLATFORM_CPU_ARM_V7
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_ARM_V7) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_ARM_V8: ");
#ifdef BSLS_PLATFORM_CPU_ARM_V8
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_ARM_V8) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_ARM_V9: ");
#ifdef BSLS_PLATFORM_CPU_ARM_V9
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_ARM_V9) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_HPPA: ");
#ifdef BSLS_PLATFORM_CPU_HPPA
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_HPPA) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_IA64: ");
#ifdef BSLS_PLATFORM_CPU_IA64
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_IA64) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_MIPS: ");
#ifdef BSLS_PLATFORM_CPU_MIPS
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_MIPS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_POWERPC: ");
#ifdef BSLS_PLATFORM_CPU_POWERPC
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_POWERPC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_SPARC: ");
#ifdef BSLS_PLATFORM_CPU_SPARC
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_SPARC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_SPARC_32: ");
#ifdef BSLS_PLATFORM_CPU_SPARC_32
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_SPARC_32) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_SPARC_V9: ");
#ifdef BSLS_PLATFORM_CPU_SPARC_V9
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_SPARC_V9) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_VER_MAJOR: ");
#ifdef BSLS_PLATFORM_CPU_VER_MAJOR
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_VER_MAJOR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_X86: ");
#ifdef BSLS_PLATFORM_CPU_X86
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_X86) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_X86_64: ");
#ifdef BSLS_PLATFORM_CPU_X86_64
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_X86_64) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_HAS_MACRO_PUSH_POP: ");
#ifdef BSLS_PLATFORM_HAS_MACRO_PUSH_POP
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_HAS_MACRO_PUSH_POP) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC: ");
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_IS_BIG_ENDIAN: ");
#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_IS_BIG_ENDIAN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_IS_LITTLE_ENDIAN: ");
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_IS_LITTLE_ENDIAN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_NO_64_BIT_LITERALS: ");
#ifdef BSLS_PLATFORM_NO_64_BIT_LITERALS
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_NO_64_BIT_LITERALS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_AIX: ");
#ifdef BSLS_PLATFORM_OS_AIX
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_AIX) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_CYGWIN: ");
#ifdef BSLS_PLATFORM_OS_CYGWIN
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_CYGWIN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_DARWIN: ");
#ifdef BSLS_PLATFORM_OS_DARWIN
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_DARWIN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_FREEBSD: ");
#ifdef BSLS_PLATFORM_OS_FREEBSD
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_FREEBSD) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_HPUX: ");
#ifdef BSLS_PLATFORM_OS_HPUX
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_HPUX) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_LINUX: ");
#ifdef BSLS_PLATFORM_OS_LINUX
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_LINUX) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_SOLARIS: ");
#ifdef BSLS_PLATFORM_OS_SOLARIS
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_SOLARIS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_SUBTYPE_COUNT: ");
#ifdef BSLS_PLATFORM_OS_SUBTYPE_COUNT
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_SUBTYPE_COUNT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_SUNOS: ");
#ifdef BSLS_PLATFORM_OS_SUNOS
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_SUNOS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_UNIX: ");
#ifdef BSLS_PLATFORM_OS_UNIX
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_UNIX) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_VER_MAJOR: ");
#ifdef BSLS_PLATFORM_OS_VER_MAJOR
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_VER_MAJOR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_VER_MINOR: ");
#ifdef BSLS_PLATFORM_OS_VER_MINOR
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_VER_MINOR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_WIN2K: ");
#ifdef BSLS_PLATFORM_OS_WIN2K
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_WIN2K) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_WIN9X: ");
#ifdef BSLS_PLATFORM_OS_WIN9X
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_WIN9X) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_WINDOWS: ");
#ifdef BSLS_PLATFORM_OS_WINDOWS
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_WINDOWS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_WINNT: ");
#ifdef BSLS_PLATFORM_OS_WINNT
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_WINNT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_WINXP: ");
#ifdef BSLS_PLATFORM_OS_WINXP
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_WINXP) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CMP_AIX: ");
#ifdef BSLS_PLATFORM__CMP_AIX
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CMP_AIX) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CMP_CLANG: ");
#ifdef BSLS_PLATFORM__CMP_CLANG
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CMP_CLANG) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CMP_EDG: ");
#ifdef BSLS_PLATFORM__CMP_EDG
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CMP_EDG) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CMP_GNU: ");
#ifdef BSLS_PLATFORM__CMP_GNU
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CMP_GNU) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CMP_HP: ");
#ifdef BSLS_PLATFORM__CMP_HP
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CMP_HP) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CMP_IBM: ");
#ifdef BSLS_PLATFORM__CMP_IBM
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CMP_IBM) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CMP_MSVC: ");
#ifdef BSLS_PLATFORM__CMP_MSVC
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CMP_MSVC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CMP_SUN: ");
#ifdef BSLS_PLATFORM__CMP_SUN
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CMP_SUN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CMP_VERSION: ");
#ifdef BSLS_PLATFORM__CMP_VERSION
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CMP_VERSION) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CMP_VER_MAJOR: ");
#ifdef BSLS_PLATFORM__CMP_VER_MAJOR
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CMP_VER_MAJOR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CPU_32_BIT: ");
#ifdef BSLS_PLATFORM__CPU_32_BIT
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CPU_32_BIT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CPU_64_BIT: ");
#ifdef BSLS_PLATFORM__CPU_64_BIT
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CPU_64_BIT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CPU_88000: ");
#ifdef BSLS_PLATFORM__CPU_88000
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CPU_88000) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CPU_ALPHA: ");
#ifdef BSLS_PLATFORM__CPU_ALPHA
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CPU_ALPHA) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CPU_HPPA: ");
#ifdef BSLS_PLATFORM__CPU_HPPA
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CPU_HPPA) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CPU_IA64: ");
#ifdef BSLS_PLATFORM__CPU_IA64
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CPU_IA64) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CPU_INTEL: ");
#ifdef BSLS_PLATFORM__CPU_INTEL
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CPU_INTEL) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CPU_MIPS: ");
#ifdef BSLS_PLATFORM__CPU_MIPS
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CPU_MIPS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CPU_POWERPC: ");
#ifdef BSLS_PLATFORM__CPU_POWERPC
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CPU_POWERPC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CPU_SPARC: ");
#ifdef BSLS_PLATFORM__CPU_SPARC
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CPU_SPARC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CPU_SPARC_32: ");
#ifdef BSLS_PLATFORM__CPU_SPARC_32
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CPU_SPARC_32) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CPU_SPARC_V9: ");
#ifdef BSLS_PLATFORM__CPU_SPARC_V9
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CPU_SPARC_V9) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CPU_VER_MAJOR: ");
#ifdef BSLS_PLATFORM__CPU_VER_MAJOR
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CPU_VER_MAJOR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CPU_VER_MINOR: ");
#ifdef BSLS_PLATFORM__CPU_VER_MINOR
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CPU_VER_MINOR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CPU_X86: ");
#ifdef BSLS_PLATFORM__CPU_X86
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CPU_X86) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__CPU_X86_64: ");
#ifdef BSLS_PLATFORM__CPU_X86_64
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__CPU_X86_64) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__IS_BIG_ENDIAN: ");
#ifdef BSLS_PLATFORM__IS_BIG_ENDIAN
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__IS_BIG_ENDIAN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__IS_LITTLE_ENDIAN: ");
#ifdef BSLS_PLATFORM__IS_LITTLE_ENDIAN
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__IS_LITTLE_ENDIAN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__NO_64_BIT_LITERALS: ");
#ifdef BSLS_PLATFORM__NO_64_BIT_LITERALS
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__NO_64_BIT_LITERALS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__OS_AIX: ");
#ifdef BSLS_PLATFORM__OS_AIX
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__OS_AIX) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__OS_CYGWIN: ");
#ifdef BSLS_PLATFORM__OS_CYGWIN
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__OS_CYGWIN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__OS_DARWIN: ");
#ifdef BSLS_PLATFORM__OS_DARWIN
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__OS_DARWIN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__OS_FREEBSD: ");
#ifdef BSLS_PLATFORM__OS_FREEBSD
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__OS_FREEBSD) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__OS_HPUX: ");
#ifdef BSLS_PLATFORM__OS_HPUX
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__OS_HPUX) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__OS_LINUX: ");
#ifdef BSLS_PLATFORM__OS_LINUX
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__OS_LINUX) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__OS_SOLARIS: ");
#ifdef BSLS_PLATFORM__OS_SOLARIS
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__OS_SOLARIS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__OS_SUBTYPE_COUNT: ");
#ifdef BSLS_PLATFORM__OS_SUBTYPE_COUNT
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__OS_SUBTYPE_COUNT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__OS_SUNOS: ");
#ifdef BSLS_PLATFORM__OS_SUNOS
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__OS_SUNOS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__OS_UNIX: ");
#ifdef BSLS_PLATFORM__OS_UNIX
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__OS_UNIX) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__OS_VER_MAJOR: ");
#ifdef BSLS_PLATFORM__OS_VER_MAJOR
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__OS_VER_MAJOR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__OS_VER_MINOR: ");
#ifdef BSLS_PLATFORM__OS_VER_MINOR
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__OS_VER_MINOR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__OS_WIN2K: ");
#ifdef BSLS_PLATFORM__OS_WIN2K
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__OS_WIN2K) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__OS_WIN9X: ");
#ifdef BSLS_PLATFORM__OS_WIN9X
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__OS_WIN9X) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__OS_WINDOWS: ");
#ifdef BSLS_PLATFORM__OS_WINDOWS
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__OS_WINDOWS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__OS_WINNT: ");
#ifdef BSLS_PLATFORM__OS_WINNT
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__OS_WINNT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM__OS_WINXP: ");
#ifdef BSLS_PLATFORM__OS_WINXP
    printf("%s\n", STRINGIFY(BSLS_PLATFORM__OS_WINXP) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\n  printFlags: bsls_platform Referenced Macros\n");

    printf("\n  BDE_BUILD_TARGET_AGGRESSIVE_INLINE: ");
#ifdef BDE_BUILD_TARGET_AGGRESSIVE_INLINE
    printf("%s\n", STRINGIFY(BDE_BUILD_TARGET_AGGRESSIVE_INLINE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BDE_DISABLE_COMPILER_VERSION_CHECK: ");
#ifdef BDE_DISABLE_COMPILER_VERSION_CHECK
    printf("%s\n", STRINGIFY(BDE_DISABLE_COMPILER_VERSION_CHECK) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BDE_HIDE_COMMON_WINDOWS_WARNINGS: ");
#ifdef BDE_HIDE_COMMON_WINDOWS_WARNINGS
    printf("%s\n", STRINGIFY(BDE_HIDE_COMMON_WINDOWS_WARNINGS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_AIX: ");
#ifdef BSLS_PLATFORM_CMP_AIX
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_AIX) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_CLANG: ");
#ifdef BSLS_PLATFORM_CMP_CLANG
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_CLANG) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_EDG: ");
#ifdef BSLS_PLATFORM_CMP_EDG
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_EDG) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_GNU: ");
#ifdef BSLS_PLATFORM_CMP_GNU
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_GNU) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_HP: ");
#ifdef BSLS_PLATFORM_CMP_HP
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_HP) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_IBM: ");
#ifdef BSLS_PLATFORM_CMP_IBM
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_IBM) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_MSVC: ");
#ifdef BSLS_PLATFORM_CMP_MSVC
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_MSVC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_SUN: ");
#ifdef BSLS_PLATFORM_CMP_SUN
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_SUN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_VERSION: ");
#ifdef BSLS_PLATFORM_CMP_VERSION
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_VERSION) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_VER_MAJOR: ");
#ifdef BSLS_PLATFORM_CMP_VER_MAJOR
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_VER_MAJOR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_32_BIT: ");
#ifdef BSLS_PLATFORM_CPU_32_BIT
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_32_BIT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_64_BIT: ");
#ifdef BSLS_PLATFORM_CPU_64_BIT
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_64_BIT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_88000: ");
#ifdef BSLS_PLATFORM_CPU_88000
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_88000) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_ALPHA: ");
#ifdef BSLS_PLATFORM_CPU_ALPHA
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_ALPHA) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_ARM_V5: ");
#ifdef BSLS_PLATFORM_CPU_ARM_V5
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_ARM_V5) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_ARM_V6: ");
#ifdef BSLS_PLATFORM_CPU_ARM_V6
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_ARM_V6) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_ARM_V7: ");
#ifdef BSLS_PLATFORM_CPU_ARM_V7
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_ARM_V7) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_HPPA: ");
#ifdef BSLS_PLATFORM_CPU_HPPA
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_HPPA) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_IA64: ");
#ifdef BSLS_PLATFORM_CPU_IA64
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_IA64) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_INTEL: ");
#ifdef BSLS_PLATFORM_CPU_INTEL
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_INTEL) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_MIPS: ");
#ifdef BSLS_PLATFORM_CPU_MIPS
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_MIPS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_POWERPC: ");
#ifdef BSLS_PLATFORM_CPU_POWERPC
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_POWERPC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_SPARC: ");
#ifdef BSLS_PLATFORM_CPU_SPARC
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_SPARC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_SPARC_32: ");
#ifdef BSLS_PLATFORM_CPU_SPARC_32
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_SPARC_32) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_SPARC_V9: ");
#ifdef BSLS_PLATFORM_CPU_SPARC_V9
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_SPARC_V9) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_VER_MAJOR: ");
#ifdef BSLS_PLATFORM_CPU_VER_MAJOR
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_VER_MAJOR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_VER_MINOR: ");
#ifdef BSLS_PLATFORM_CPU_VER_MINOR
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_VER_MINOR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_X86: ");
#ifdef BSLS_PLATFORM_CPU_X86
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_X86) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CPU_X86_64: ");
#ifdef BSLS_PLATFORM_CPU_X86_64
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CPU_X86_64) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_IS_BIG_ENDIAN: ");
#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_IS_BIG_ENDIAN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_IS_LITTLE_ENDIAN: ");
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_IS_LITTLE_ENDIAN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_NO_64_BIT_LITERALS: ");
#ifdef BSLS_PLATFORM_NO_64_BIT_LITERALS
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_NO_64_BIT_LITERALS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_AIX: ");
#ifdef BSLS_PLATFORM_OS_AIX
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_AIX) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_CYGWIN: ");
#ifdef BSLS_PLATFORM_OS_CYGWIN
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_CYGWIN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_DARWIN: ");
#ifdef BSLS_PLATFORM_OS_DARWIN
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_DARWIN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_FREEBSD: ");
#ifdef BSLS_PLATFORM_OS_FREEBSD
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_FREEBSD) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_HPUX: ");
#ifdef BSLS_PLATFORM_OS_HPUX
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_HPUX) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_LINUX: ");
#ifdef BSLS_PLATFORM_OS_LINUX
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_LINUX) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_SOLARIS: ");
#ifdef BSLS_PLATFORM_OS_SOLARIS
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_SOLARIS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_SUBTYPE_COUNT: ");
#ifdef BSLS_PLATFORM_OS_SUBTYPE_COUNT
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_SUBTYPE_COUNT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_SUNOS: ");
#ifdef BSLS_PLATFORM_OS_SUNOS
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_SUNOS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_UNIX: ");
#ifdef BSLS_PLATFORM_OS_UNIX
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_UNIX) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_VER_MAJOR: ");
#ifdef BSLS_PLATFORM_OS_VER_MAJOR
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_VER_MAJOR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_VER_MINOR: ");
#ifdef BSLS_PLATFORM_OS_VER_MINOR
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_VER_MINOR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_WIN2K: ");
#ifdef BSLS_PLATFORM_OS_WIN2K
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_WIN2K) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_WIN9X: ");
#ifdef BSLS_PLATFORM_OS_WIN9X
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_WIN9X) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_WINDOWS: ");
#ifdef BSLS_PLATFORM_OS_WINDOWS
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_WINDOWS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_WINNT: ");
#ifdef BSLS_PLATFORM_OS_WINNT
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_WINNT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_OS_WINXP: ");
#ifdef BSLS_PLATFORM_OS_WINXP
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_OS_WINXP) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSL_DOUBLE_UNDERSCORE_XLAT: ");
#ifdef BSL_DOUBLE_UNDERSCORE_XLAT
    printf("%s\n", STRINGIFY(BSL_DOUBLE_UNDERSCORE_XLAT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  WINVER: ");
#ifdef WINVER
    printf("%s\n", STRINGIFY(WINVER) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _AIX: ");
#ifdef _AIX
    printf("%s\n", STRINGIFY(_AIX) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _ARCH_601: ");
#ifdef _ARCH_601
    printf("%s\n", STRINGIFY(_ARCH_601) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _ARCH_COM: ");
#ifdef _ARCH_COM
    printf("%s\n", STRINGIFY(_ARCH_COM) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _ARCH_POWER: ");
#ifdef _ARCH_POWER
    printf("%s\n", STRINGIFY(_ARCH_POWER) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _ARCH_PPC: ");
#ifdef _ARCH_PPC
    printf("%s\n", STRINGIFY(_ARCH_PPC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _ARCH_PPC64: ");
#ifdef _ARCH_PPC64
    printf("%s\n", STRINGIFY(_ARCH_PPC64) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _ARCH_PWR: ");
#ifdef _ARCH_PWR
    printf("%s\n", STRINGIFY(_ARCH_PWR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _ARCH_PWR2: ");
#ifdef _ARCH_PWR2
    printf("%s\n", STRINGIFY(_ARCH_PWR2) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _BIG_ENDIAN: ");
#ifdef _BIG_ENDIAN
    printf("%s\n", STRINGIFY(_BIG_ENDIAN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _HPUX_SOURCE: ");
#ifdef _HPUX_SOURCE
    printf("%s\n", STRINGIFY(_HPUX_SOURCE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _IA64: ");
#ifdef _IA64
    printf("%s\n", STRINGIFY(_IA64) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _ILP32: ");
#ifdef _ILP32
    printf("%s\n", STRINGIFY(_ILP32) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _LITTLE_ENDIAN: ");
#ifdef _LITTLE_ENDIAN
    printf("%s\n", STRINGIFY(_LITTLE_ENDIAN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _LP64: ");
#ifdef _LP64
    printf("%s\n", STRINGIFY(_LP64) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _MSC_VER: ");
#ifdef _MSC_VER
    printf("%s\n", STRINGIFY(_MSC_VER) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _M_ALPHA: ");
#ifdef _M_ALPHA
    printf("%s\n", STRINGIFY(_M_ALPHA) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _M_AMD64: ");
#ifdef _M_AMD64
    printf("%s\n", STRINGIFY(_M_AMD64) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _M_IA64: ");
#ifdef _M_IA64
    printf("%s\n", STRINGIFY(_M_IA64) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _M_IX86: ");
#ifdef _M_IX86
    printf("%s\n", STRINGIFY(_M_IX86) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _M_MRX000: ");
#ifdef _M_MRX000
    printf("%s\n", STRINGIFY(_M_MRX000) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _M_PPC: ");
#ifdef _M_PPC
    printf("%s\n", STRINGIFY(_M_PPC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _POWER: ");
#ifdef _POWER
    printf("%s\n", STRINGIFY(_POWER) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _WIN16: ");
#ifdef _WIN16
    printf("%s\n", STRINGIFY(_WIN16) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _WIN32: ");
#ifdef _WIN32
    printf("%s\n", STRINGIFY(_WIN32) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _WIN32_WINDOWS: ");
#ifdef _WIN32_WINDOWS
    printf("%s\n", STRINGIFY(_WIN32_WINDOWS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _WIN32_WINNT: ");
#ifdef _WIN32_WINNT
    printf("%s\n", STRINGIFY(_WIN32_WINNT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _WIN64: ");
#ifdef _WIN64
    printf("%s\n", STRINGIFY(_WIN64) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __64BIT__: ");
#ifdef __64BIT__
    printf("%s\n", STRINGIFY(__64BIT__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __APPLE__: ");
#ifdef __APPLE__
    printf("%s\n", STRINGIFY(__APPLE__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __ARM_ARCH: ");
#ifdef __ARM_ARCH
    printf("%s\n", STRINGIFY(__ARM_ARCH) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __ARM_ARCH_5TEJ__: ");
#ifdef __ARM_ARCH_5TEJ__
    printf("%s\n", STRINGIFY(__ARM_ARCH_5TEJ__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __ARM_ARCH_5TE__: ");
#ifdef __ARM_ARCH_5TE__
    printf("%s\n", STRINGIFY(__ARM_ARCH_5TE__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __ARM_ARCH_5T__: ");
#ifdef __ARM_ARCH_5T__
    printf("%s\n", STRINGIFY(__ARM_ARCH_5T__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __ARM_ARCH_6__: ");
#ifdef __ARM_ARCH_6__
    printf("%s\n", STRINGIFY(__ARM_ARCH_6__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __ARM_ARCH_7A__: ");
#ifdef __ARM_ARCH_7A__
    printf("%s\n", STRINGIFY(__ARM_ARCH_7A__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __ARM_ARCH_7M__: ");
#ifdef __ARM_ARCH_7M__
    printf("%s\n", STRINGIFY(__ARM_ARCH_7M__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __ARM_ARCH_7R__: ");
#ifdef __ARM_ARCH_7R__
    printf("%s\n", STRINGIFY(__ARM_ARCH_7R__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __ARM_ARCH_7__: ");
#ifdef __ARM_ARCH_7__
    printf("%s\n", STRINGIFY(__ARM_ARCH_7__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __CYGWIN__: ");
#ifdef __CYGWIN__
    printf("%s\n", STRINGIFY(__CYGWIN__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __EDG__: ");
#ifdef __EDG__
    printf("%s\n", STRINGIFY(__EDG__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __FreeBSD__: ");
#ifdef __FreeBSD__
    printf("%s\n", STRINGIFY(__FreeBSD__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __GLIBC__: ");
#ifdef __GLIBC__
    printf("%s\n", STRINGIFY(__GLIBC__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __GNUC_PATCHLEVEL__: ");
#ifdef __GNUC_PATCHLEVEL__
    printf("%s\n", STRINGIFY(__GNUC_PATCHLEVEL__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __GNUC__: ");
#ifdef __GNUC__
    printf("%s\n", STRINGIFY(__GNUC__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __GXX_EXPERIMENTAL_CXX0X__: ");
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    printf("%s\n", STRINGIFY(__GXX_EXPERIMENTAL_CXX0X__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __HP_aCC: ");
#ifdef __HP_aCC
    printf("%s\n", STRINGIFY(__HP_aCC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __IA64__: ");
#ifdef __IA64__
    printf("%s\n", STRINGIFY(__IA64__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __IBMCPP__: ");
#ifdef __IBMCPP__
    printf("%s\n", STRINGIFY(__IBMCPP__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __IBMC__: ");
#ifdef __IBMC__
    printf("%s\n", STRINGIFY(__IBMC__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __LP64__: ");
#ifdef __LP64__
    printf("%s\n", STRINGIFY(__LP64__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __POWERPC__: ");
#ifdef __POWERPC__
    printf("%s\n", STRINGIFY(__POWERPC__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __SUNPRO_C: ");
#ifdef __SUNPRO_C
    printf("%s\n", STRINGIFY(__SUNPRO_C) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __SUNPRO_CC: ");
#ifdef __SUNPRO_CC
    printf("%s\n", STRINGIFY(__SUNPRO_CC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __SVR4: ");
#ifdef __SVR4
    printf("%s\n", STRINGIFY(__SVR4) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __SunOS_5_10: ");
#ifdef __SunOS_5_10
    printf("%s\n", STRINGIFY(__SunOS_5_10) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __SunOS_5_11: ");
#ifdef __SunOS_5_11
    printf("%s\n", STRINGIFY(__SunOS_5_11) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __SunOS_5_7: ");
#ifdef __SunOS_5_7
    printf("%s\n", STRINGIFY(__SunOS_5_7) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __SunOS_5_8: ");
#ifdef __SunOS_5_8
    printf("%s\n", STRINGIFY(__SunOS_5_8) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __SunOS_5_9: ");
#ifdef __SunOS_5_9
    printf("%s\n", STRINGIFY(__SunOS_5_9) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __WIN32__: ");
#ifdef __WIN32__
    printf("%s\n", STRINGIFY(__WIN32__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __alpha__: ");
#ifdef __alpha__
    printf("%s\n", STRINGIFY(__alpha__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __arch64__: ");
#ifdef __arch64__
    printf("%s\n", STRINGIFY(__arch64__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __arm__: ");
#ifdef __arm__
    printf("%s\n", STRINGIFY(__arm__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __clang__: ");
#ifdef __clang__
    printf("%s\n", STRINGIFY(__clang__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __cplusplus: ");
#ifdef __cplusplus
    printf("%s\n", STRINGIFY(__cplusplus) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __cygwin: ");
#ifdef __cygwin
    printf("%s\n", STRINGIFY(__cygwin) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __hppa: ");
#ifdef __hppa
    printf("%s\n", STRINGIFY(__hppa) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __hppa__: ");
#ifdef __hppa__
    printf("%s\n", STRINGIFY(__hppa__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __hpux: ");
#ifdef __hpux
    printf("%s\n", STRINGIFY(__hpux) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __i386: ");
#ifdef __i386
    printf("%s\n", STRINGIFY(__i386) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __i386__: ");
#ifdef __i386__
    printf("%s\n", STRINGIFY(__i386__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __ia64: ");
#ifdef __ia64
    printf("%s\n", STRINGIFY(__ia64) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __ia64__: ");
#ifdef __ia64__
    printf("%s\n", STRINGIFY(__ia64__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __ix86: ");
#ifdef __ix86
    printf("%s\n", STRINGIFY(__ix86) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __ix86__: ");
#ifdef __ix86__
    printf("%s\n", STRINGIFY(__ix86__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __linux: ");
#ifdef __linux
    printf("%s\n", STRINGIFY(__linux) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __linux__: ");
#ifdef __linux__
    printf("%s\n", STRINGIFY(__linux__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __mips__: ");
#ifdef __mips__
    printf("%s\n", STRINGIFY(__mips__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __powerpc: ");
#ifdef __powerpc
    printf("%s\n", STRINGIFY(__powerpc) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __powerpc__: ");
#ifdef __powerpc__
    printf("%s\n", STRINGIFY(__powerpc__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __ppc__: ");
#ifdef __ppc__
    printf("%s\n", STRINGIFY(__ppc__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __sparc: ");
#ifdef __sparc
    printf("%s\n", STRINGIFY(__sparc) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __sparc64: ");
#ifdef __sparc64
    printf("%s\n", STRINGIFY(__sparc64) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __sparc__: ");
#ifdef __sparc__
    printf("%s\n", STRINGIFY(__sparc__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __sparc_v9__: ");
#ifdef __sparc_v9__
    printf("%s\n", STRINGIFY(__sparc_v9__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __sparcv9: ");
#ifdef __sparcv9
    printf("%s\n", STRINGIFY(__sparcv9) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __sun: ");
#ifdef __sun
    printf("%s\n", STRINGIFY(__sun) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __svr4__: ");
#ifdef __svr4__
    printf("%s\n", STRINGIFY(__svr4__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __unix: ");
#ifdef __unix
    printf("%s\n", STRINGIFY(__unix) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __unix__: ");
#ifdef __unix__
    printf("%s\n", STRINGIFY(__unix__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __x86_64: ");
#ifdef __x86_64
    printf("%s\n", STRINGIFY(__x86_64) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __x86_64__: ");
#ifdef __x86_64__
    printf("%s\n", STRINGIFY(__x86_64__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __xlC__: ");
#ifdef __xlC__
    printf("%s\n", STRINGIFY(__xlC__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  bdes_Platform: ");
#ifdef bdes_Platform
    printf("%s\n", STRINGIFY(bdes_Platform) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  cygwin: ");
#ifdef cygwin
    printf("%s\n", STRINGIFY(cygwin) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  hpux: ");
#ifdef hpux
    printf("%s\n", STRINGIFY(hpux) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  i386: ");
#ifdef i386
    printf("%s\n", STRINGIFY(i386) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  linux: ");
#ifdef linux
    printf("%s\n", STRINGIFY(linux) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  macro: ");
#ifdef macro
    printf("%s\n", STRINGIFY(macro) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  sparc: ");
#ifdef sparc
    printf("%s\n", STRINGIFY(sparc) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  sun: ");
#ifdef sun
    printf("%s\n", STRINGIFY(sun) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  unix: ");
#ifdef unix
    printf("%s\n", STRINGIFY(unix) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\nprintFlags: Leave\n");
}

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    (void) veryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    if (veryVeryVerbose) {
        printFlags();
    }

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // DUMP OUT BSLS_PLATFORM_*
        //
        // Concerns:
        //: 1 It is hard to tell which old literals are enabled and which
        //:   aren't.
        //
        // Plan:
        //: 1 Dump them out.
        // --------------------------------------------------------------------

        if (verbose) printf("\nDUMP OUT BSLS_PLATFORM_*"
                            "\n========================\n");

        if (!verbose) break;

        printFlags();
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 64-BIT LITERALS
        //
        // Concerns:
        //: 1 Since the actual flag indicates the lack of support for
        //:   64-bit integer literals, the only way to test the flag is
        //:   for the compiler to fail.  Therefore the test will check for the
        //:   absence of the flag and attempt to assign 64-bit literals to a
        //:   variable, ensuring the compile-time macro for support of
        //:   64-bit integer literals agrees with the capability of the
        //:   compiler.
        //
        // Plan:
        //: 1 Assign both signed and unsigned 64-bit integer literals to
        //:   variables of each type.
        //:
        //: 2 Verify that the compiler does not truncate the assignment or the
        //:   constant by splitting the constant into 2 32-bit constants and
        //:   combining them using logical operations into another
        //:   64-bit value.
        //:
        //: 3 Verify the constructed value is equal to the 64-bit value
        //:   directly assigned.
        //:
        //: 4 Verify no truncation is occurring by logically masking and
        //:   shifting the 64-bit value with the 32-bit lo and hi words.
        //
        // Testing:
        //   BSLS_PLATFORM_NO_64_BIT_LITERALS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 64-BIT LITERALS"
                            "\n=======================\n");

        if (veryVerbose) printf("64-bit integer constants supported.\n");

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
        i = 9223372036854775807LL;    // 0x7FFFFFFFFFFFFFFF
        u = 9223372036854775809ULL;   // 0x8000000000000001

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
        // TESTING BIG ENDIAN AND LITTLE ENDIAN
        //
        // Concerns:
        //: 1 The macros BSLS_PLATFORM_IS_BIG_ENDIAN and
        //:   BSLS_PLATFORM_IS_LITTLE_ENDIAN must have boolean values.
        //:
        //: 2 The macros BSLS_PLATFORM_IS_BIG_ENDIAN and
        //:   BSLS_PLATFORM_IS_LITTLE_ENDIAN are assigned at compile time based
        //:   on the platform (see overview above).  If any one of the flags or
        //:   inferences is wrong, the "endian-ness" of a given platform could
        //:   be wrong.  Fortunately it is possible to make run-time
        //:   determination of a platform's "endian-ness" by using a union.
        //:   Unfortunately such a test is more expensive than checking a flag.
        //:   However, such a function is perfect for a test driver.
        //
        // Plan:
        //: 1 First ensure the values for the endian macros return boolean
        //:   values.  Next, ensure the compile-time macros and test functions
        //:   agree with the values calculated at runtime.
        //
        // Testing:
        //   BSLS_PLATFORM_IS_LITTLE_ENDIAN
        //   BSLS_PLATFORM_IS_BIG_ENDIAN
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BIG ENDIAN AND LITTLE ENDIAN"
                            "\n====================================\n");

        if (veryVerbose) {
            printf("This platform is %s.\n",
                   (::isBigEndian() ? "BIG ENDIAN" : "LITTLE ENDIAN"));
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
        // MINIMAL DEFINITION TEST
        //
        // Plan:
        //: 1 We want to make sure that exactly one each of OS, PROCESSOR,
        //:   COMPILER and ENDIAN type is set.  We also want to make sure that
        //:   at most one subtype of OS is set.  Furthermore, we want to make
        //:   sure that a minor version implies a major version, and that for
        //:   OS, a major version implies a subtype.  Finally, we want to make
        //:   sure that one endian-ness type is defined for the platform.
        //
        // Testing:
        //   Ensure that exactly one of each CMP type is set.
        //   Ensure that exactly one of each OS type is set.
        //   Ensure that exactly one of each CPU type is set.
        //   Ensure that at most one of each CPU subtype is set.
        //   For each category, ensure MINOR_NUMBER set -> MAJOR_NUMBER set.
        //   For the OS type, ensure MAJOR_NUMBER set -> SUBTYPE set.
        //   For the ENDIAN type, ensure one is set.
        // --------------------------------------------------------------------

        if (verbose) printf("\nMINIMAL DEFINITION TEST"
                            "\n=======================\n");

        // Compile-time tests for this case are located in the validation
        // section of this component's header file.

        struct OsCpu {
            bsls::Platform::Os  osType;
            bsls::Platform::Cpu cpuType;
        };

        if (veryVerbose) {
            printf("\nPrint Defined Symbols\n");
            printf("\nPrint CMP-related Symbols:\n");
        }

#define MACRO_TESTEQ(X, Y)                                                   \
        ASSERT(Y == X);                                                      \
        if (veryVerbose) printf("\t%s = %d (0x%X)\n", #X, (X), (X));

#define MACRO_TESTGT(X, Y)                                                   \
        ASSERT(Y <= X);                                                      \
        if (veryVerbose) printf("\t%s = %d (0x%X)\n", #X, (X), (X));

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

        if (veryVerbose) printf("Print OS-related Symbols:\n");

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

        if (veryVerbose) printf("\nPrint CPU-related Symbols:\n");

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

        if (veryVerbose) printf("\nPrint endian-ness symbols:\n");

        #if defined(BSLS_PLATFORM_IS_BIG_ENDIAN)
            MACRO_TESTEQ(BSLS_PLATFORM_IS_BIG_ENDIAN, 1);
        #endif
        #if defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
            MACRO_TESTEQ(BSLS_PLATFORM_IS_LITTLE_ENDIAN, 1);
        #endif

        if (veryVerbose) printf("\nPrint inlining symbol:\n");

        if (veryVerbose) printf("\t"
                                STRINGIFY2(BSLS_PLATFORM_AGGRESSIVE_INLINE)
                                " = \""
                                STRINGIFY(BSLS_PLATFORM_AGGRESSIVE_INLINE)
                                "\"\n");

        #if (defined(BSLS_PLATFORM_CMP_AIX) || defined(BSLS_PLATFORM_CMP_SUN))\
            || defined(BDE_BUILD_TARGET_AGGRESSIVE_INLINE)
        ASSERT(0 == strlen(STRINGIFY(BSLS_PLATFORM_AGGRESSIVE_INLINE)));
        #else
        ASSERT(0 == strcmp(STRINGIFY(BSLS_PLATFORM_AGGRESSIVE_INLINE),
                                     "inline"));
        #endif
      } break;
      default: {
        fprintf( stderr, "WARNING: CASE `%d` NOT FOUND.\n" , test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf( stderr, "Error, non-zero test status = %d.\n", testStatus );
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
