// bsls_platform.t.cpp                                                -*-C++-*-

#include <bsls_platform.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>     // 'strcmp', 'strlen'

#if defined(_MSC_VER)
    #include <intrin.h>
#elif (defined(__clang__) || defined(__GNUC__) || defined(__EDG__)) \
        && (BSLS_PLATFORM_CPU_X86 || BSLS_PLATFORM_CPU_X86_64)
    #include <cpuid.h>
#endif

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                            * Overview *
// Since this component implements C++ macros, which may or may not be defined,
// there is not too much to test in this driver.  Since correctness will be
// affected by compile-time switches during the build process, any compile-time
// tests we come up with should probably reside directly in the header or
// implementation file.
// ----------------------------------------------------------------------------
// [ 1] CONCERN: exactly one compiler vendor macro is defined.
// [ 1] CONCERN: the compiler version macro is defined.
// [ 1] CONCERN: exactly one OS type macro is defined.
// [ 1] CONCERN: exactly one OS subtype macro is defined.
// [ 1] CONCERN: exactly one of each CPU macro is defined.
// [ 1] CONCERN: exactly one CPU instruction set macro is defined.
// [ 1] CONCERN: exactly one CPU register width macro is defined.
// [ 1] CONCERN: at most one CPU version macro is defined.
// [ 1] CONCERN: exactly one ENDIAN macro is set.
// [ 4] CONCERN: report definition of all platform macros.
// [ 2] BSLS_PLATFORM_IS_LITTLE_ENDIAN
// [ 2] BSLS_PLATFORM_IS_BIG_ENDIAN
// [ 3] BSLS_PLATFORM_NO_64_BIT_LITERALS
// [ 5] BSLS_PLATFORM_CPU_SSE*
// ============================================================================

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

// BDE_VERIFY pragma: -TP19
//  This component is levelized below 'bsls_bsltestutil', so cannot directly
//  alias the standard test macros.

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define STRINGIFY2(...) "" #__VA_ARGS__
#define STRINGIFY(...) STRINGIFY2(__VA_ARGS__)

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
        int  d_int;
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
        int  d_int;
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
//                              HELPER FUNCTIONS
// ----------------------------------------------------------------------------

namespace {

template <size_t t_MAX_SIZE>
class StaticStringsTable {
    // A statically-sized table for pointers to C strings for collecting
    // information about macros.

  public:
    // PUBLIC CONSTANTS
    static const size_t k_MAX_SIZE = t_MAX_SIZE;

  private:
    // DATA
    const char *d_name_p;          // Name for this table

    const char *d_table[k_MAX_SIZE];
    size_t      d_index;

  public:
    // CREATORS
    explicit StaticStringsTable(const char *name)
        // Create a new 'StaticStringsTable' with the specified 'name'.  The
        // behavior is undefined unless 'name' outlives the created object.
    : d_name_p(name)
    , d_index(0)
    {
    }

    // MANIPULATORS
    void pushBack(const char *string)
        // If there are no more free locations in the table report that error
        // to 'stdout'.  Otherwise store the specified 'string' to the next
        // free location then increment the next free location.
    {
        if (k_MAX_SIZE == d_index) {
            fputs(d_name_p, stdout);
            fputs(" table is full, could not store ", stdout);
            puts(string);
            aSsErT(true, "Could not store string.", __LINE__);
            return;                                                   // RETURN
        }

        d_table[d_index++] = string;
    }

    // ACCESSORS
    const char *operator[](size_t idx) const
        // Return the string pointer at the specified 'idx' position or return
        // an error if 'idx' is out of bounds.
    {
        ASSERT(idx < count());
        if (idx >= count()) {
            return "-<[* Out of Bound Access to String Table *]>-";   // RETURN
        }
        return d_table[idx];
    }

    size_t count() const
        // Return the number of string pointers store in this object.
    {
        return d_index;
    }
};

}  // close unnamed namespace

static void printFlags()
    // Print a diagnostic message to standard output if any of the preprocessor
    // flags of interest are defined, and their value if a value had been set.
    // An "Enter" and "Leave" message is printed unconditionally so there is
    // some report even if all of the flags are undefined.
{
    StaticStringsTable<512> undefinedMacros("undefinedMacros");

    puts("printFlags: Enter");

    puts("\n  printFlags: Configuration Macros");
    puts(  "  --------------------------------");

#define D_MACRO(X) undefinedMacros.pushBack(#X);
    // Add the specified macro named by 'X' to the list of macros to report as
    // not defined.

#define P_MACRO(X) printf("\t  %s:\t%s\n", #X, STRINGIFY(X));
    // Print the name of the specified object-like macro named by 'X', and the
    // source it expands to.

#if defined(BDE_BUILD_TARGET_AGGRESSIVE_INLINE)
    P_MACRO(BDE_BUILD_TARGET_AGGRESSIVE_INLINE);
#else
    D_MACRO(BDE_BUILD_TARGET_AGGRESSIVE_INLINE);
#endif

#if defined(BDE_DISABLE_COMPILER_VERSION_CHECK)
    P_MACRO(BDE_DISABLE_COMPILER_VERSION_CHECK);
#else
    D_MACRO(BDE_DISABLE_COMPILER_VERSION_CHECK);
#endif

#if defined(BSL_DOUBLE_UNDERSCORE_XLAT)
    P_MACRO(BSL_DOUBLE_UNDERSCORE_XLAT);
#else
    D_MACRO(BSL_DOUBLE_UNDERSCORE_XLAT);
#endif

    if (4 == undefinedMacros.count()) {
        puts("\n      None defined");
    }

    puts("\n  printFlags: Consistency Check Macros");
    puts(  "  ------------------------------------");

#if defined(BSLS_PLATFORM_COMPILER_ERROR)
    P_MACRO(BSLS_PLATFORM_COMPILER_ERROR);
#else
    D_MACRO(BSLS_PLATFORM_COMPILER_ERROR);
#endif

#if defined(BSLS_PLATFORM_OS_SUBTYPE_COUNT)
    P_MACRO(BSLS_PLATFORM_OS_SUBTYPE_COUNT);
#else
    D_MACRO(BSLS_PLATFORM_OS_SUBTYPE_COUNT);
#endif


    puts("\n  printFlags: standard macros");
    puts(  "  ---------------------------");

#if defined(__cplusplus)
    P_MACRO(__cplusplus);
#else
    D_MACRO(__cplusplus);
#endif

#if defined(__DATE__)
    P_MACRO(__DATE__);
#else
    D_MACRO(__DATE__);
#endif

#if defined(__TIME__)
    P_MACRO(__TIME__);
#else
    D_MACRO(__TIME__);
#endif

#if defined(__FILE__)
    P_MACRO(__FILE__);
#else
    D_MACRO(__FILE__);
#endif

#if defined(__LINE__)
    P_MACRO(__LINE__);
#else
    D_MACRO(__LINE__);
#endif

#if defined(__STDC__)
    P_MACRO(__STDC__);
#else
    D_MACRO(__STDC__);
#endif

#if defined(__STDC_HOSTED__)
    P_MACRO(__STDC_HOSTED__);
#else
    D_MACRO(__STDC_HOSTED__);
#endif

#if defined(__STDC_MB_MIGHT_NEQ_WC__)
    P_MACRO(__STDC_MB_MIGHT_NEQ_WC__);
#else
    D_MACRO(__STDC_MB_MIGHT_NEQ_WC__);
#endif

#if defined(__STDC_VERSION__)
    P_MACRO(__STDC_VERSION__);
#else
    D_MACRO(__STDC_VERSION__);
#endif

#if defined(__STDC_ISO_10646__)
    P_MACRO(__STDC_ISO_10646__);
#else
    D_MACRO(__STDC_ISO_10646__);
#endif

#if defined(__STDCPP_DEFAULT_NEW_ALIGNMENT__)
    P_MACRO(__STDCPP_DEFAULT_NEW_ALIGNMENT__);
#else
    D_MACRO(__STDCPP_DEFAULT_NEW_ALIGNMENT__);
#endif

#if defined(__STDCPP_STRICT_POINTER_SAFETY__)
    P_MACRO(__STDCPP_STRICT_POINTER_SAFETY__);
#else
    D_MACRO(__STDCPP_STRICT_POINTER_SAFETY__);
#endif

#if defined(__STDCPP_THREADS__)
    P_MACRO(__STDCPP_THREADS__);
#else
    D_MACRO(__STDCPP_THREADS__);
#endif


    puts("\n  printFlags: bsls_platform Macros");
    puts(  "  --------------------------------");

#if defined(BSLS_PLATFORM_CMP_AIX)
    P_MACRO(BSLS_PLATFORM_CMP_AIX);
#else
    D_MACRO(BSLS_PLATFORM_CMP_AIX);
#endif

#if defined(BSLS_PLATFORM_CMP_CLANG)
    P_MACRO(BSLS_PLATFORM_CMP_CLANG);
#else
    D_MACRO(BSLS_PLATFORM_CMP_CLANG);
#endif

#if defined(BSLS_PLATFORM_CMP_EDG)
    P_MACRO(BSLS_PLATFORM_CMP_EDG);
#else
    D_MACRO(BSLS_PLATFORM_CMP_EDG);
#endif

#if defined(BSLS_PLATFORM_CMP_GNU)
    P_MACRO(BSLS_PLATFORM_CMP_GNU);
#else
    D_MACRO(BSLS_PLATFORM_CMP_GNU);
#endif

#if defined(BSLS_PLATFORM_CMP_HP)
    P_MACRO(BSLS_PLATFORM_CMP_HP);
#else
    D_MACRO(BSLS_PLATFORM_CMP_HP);
#endif

#if defined(BSLS_PLATFORM_CMP_IBM)
    P_MACRO(BSLS_PLATFORM_CMP_IBM);
#else
    D_MACRO(BSLS_PLATFORM_CMP_IBM);
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)
    P_MACRO(BSLS_PLATFORM_CMP_MSVC);
#else
    D_MACRO(BSLS_PLATFORM_CMP_MSVC);
#endif

#if defined(BSLS_PLATFORM_CMP_SUN)
    P_MACRO(BSLS_PLATFORM_CMP_SUN);
#else
    D_MACRO(BSLS_PLATFORM_CMP_SUN);
#endif

#if defined(BSLS_PLATFORM_CMP_VERSION)
    P_MACRO(BSLS_PLATFORM_CMP_VERSION);
#else
    D_MACRO(BSLS_PLATFORM_CMP_VERSION);
#endif

#if defined(BSLS_PLATFORM_CMP_VER_MAJOR)
    P_MACRO(BSLS_PLATFORM_CMP_VER_MAJOR);
#else
    D_MACRO(BSLS_PLATFORM_CMP_VER_MAJOR);
#endif

#if defined(BSLS_PLATFORM_CPU_32_BIT)
    P_MACRO(BSLS_PLATFORM_CPU_32_BIT);
#else
    D_MACRO(BSLS_PLATFORM_CPU_32_BIT);
#endif

#if defined(BSLS_PLATFORM_CPU_64_BIT)
    P_MACRO(BSLS_PLATFORM_CPU_64_BIT);
#else
    D_MACRO(BSLS_PLATFORM_CPU_64_BIT);
#endif

#if defined(BSLS_PLATFORM_CPU_88000)
    P_MACRO(BSLS_PLATFORM_CPU_88000);
#else
    D_MACRO(BSLS_PLATFORM_CPU_88000);
#endif

#if defined(BSLS_PLATFORM_CPU_ALPHA)
    P_MACRO(BSLS_PLATFORM_CPU_ALPHA);
#else
    D_MACRO(BSLS_PLATFORM_CPU_ALPHA);
#endif

#if defined(BSLS_PLATFORM_CPU_ARM)
    P_MACRO(BSLS_PLATFORM_CPU_ARM);
#else
    D_MACRO(BSLS_PLATFORM_CPU_ARM);
#endif

#if defined(BSLS_PLATFORM_CPU_ARM_V5)
    P_MACRO(BSLS_PLATFORM_CPU_ARM_V5);
#else
    D_MACRO(BSLS_PLATFORM_CPU_ARM_V5);
#endif

#if defined(BSLS_PLATFORM_CPU_ARM_V6)
    P_MACRO(BSLS_PLATFORM_CPU_ARM_V6);
#else
    D_MACRO(BSLS_PLATFORM_CPU_ARM_V6);
#endif

#if defined(BSLS_PLATFORM_CPU_ARM_V7)
    P_MACRO(BSLS_PLATFORM_CPU_ARM_V7);
#else
    D_MACRO(BSLS_PLATFORM_CPU_ARM_V7);
#endif

#if defined(BSLS_PLATFORM_CPU_ARM_V8)
    P_MACRO(BSLS_PLATFORM_CPU_ARM_V8);
#else
    D_MACRO(BSLS_PLATFORM_CPU_ARM_V8);
#endif

#if defined(BSLS_PLATFORM_CPU_ARM_V9)
    P_MACRO(BSLS_PLATFORM_CPU_ARM_V9);
#else
    D_MACRO(BSLS_PLATFORM_CPU_ARM_V9);
#endif

#if defined(BSLS_PLATFORM_CPU_HPPA)
    P_MACRO(BSLS_PLATFORM_CPU_HPPA);
#else
    D_MACRO(BSLS_PLATFORM_CPU_HPPA);
#endif

#if defined(BSLS_PLATFORM_CPU_IA64)
    P_MACRO(BSLS_PLATFORM_CPU_IA64);
#else
    D_MACRO(BSLS_PLATFORM_CPU_IA64);
#endif

#if defined(BSLS_PLATFORM_CPU_INTEL)
    P_MACRO(BSLS_PLATFORM_CPU_INTEL);
#else
    D_MACRO(BSLS_PLATFORM_CPU_INTEL);
#endif

#if defined(BSLS_PLATFORM_CPU_MIPS)
    P_MACRO(BSLS_PLATFORM_CPU_MIPS);
#else
    D_MACRO(BSLS_PLATFORM_CPU_MIPS);
#endif

#if defined(BSLS_PLATFORM_CPU_POWERPC)
    P_MACRO(BSLS_PLATFORM_CPU_POWERPC);
#else
    D_MACRO(BSLS_PLATFORM_CPU_POWERPC);
#endif

#if defined(BSLS_PLATFORM_CPU_SPARC)
    P_MACRO(BSLS_PLATFORM_CPU_SPARC);
#else
    D_MACRO(BSLS_PLATFORM_CPU_SPARC);
#endif

#if defined(BSLS_PLATFORM_CPU_SPARC_32)
    P_MACRO(BSLS_PLATFORM_CPU_SPARC_32);
#else
    D_MACRO(BSLS_PLATFORM_CPU_SPARC_32);
#endif

#if defined(BSLS_PLATFORM_CPU_SPARC_V9)
    P_MACRO(BSLS_PLATFORM_CPU_SPARC_V9);
#else
    D_MACRO(BSLS_PLATFORM_CPU_SPARC_V9);
#endif

#if defined(BSLS_PLATFORM_CPU_VER_MAJOR)
    P_MACRO(BSLS_PLATFORM_CPU_VER_MAJOR);
#else
    D_MACRO(BSLS_PLATFORM_CPU_VER_MAJOR);
#endif

#if defined(BSLS_PLATFORM_CPU_X86)
    P_MACRO(BSLS_PLATFORM_CPU_X86);
#else
    D_MACRO(BSLS_PLATFORM_CPU_X86);
#endif

#if defined(BSLS_PLATFORM_CPU_X86_64)
    P_MACRO(BSLS_PLATFORM_CPU_X86_64);
#else
    D_MACRO(BSLS_PLATFORM_CPU_X86_64);
#endif

#if defined(BSLS_PLATFORM_IS_BIG_ENDIAN)
    P_MACRO(BSLS_PLATFORM_IS_BIG_ENDIAN);
#else
    D_MACRO(BSLS_PLATFORM_IS_BIG_ENDIAN);
#endif

#if defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
    P_MACRO(BSLS_PLATFORM_IS_LITTLE_ENDIAN);
#else
    D_MACRO(BSLS_PLATFORM_IS_LITTLE_ENDIAN);
#endif

#if defined(BSLS_PLATFORM_OS_AIX)
    P_MACRO(BSLS_PLATFORM_OS_AIX);
#else
    D_MACRO(BSLS_PLATFORM_OS_AIX);
#endif

#if defined(BSLS_PLATFORM_OS_CYGWIN)
    P_MACRO(BSLS_PLATFORM_OS_CYGWIN);
#else
    D_MACRO(BSLS_PLATFORM_OS_CYGWIN);
#endif

#if defined(BSLS_PLATFORM_OS_DARWIN)
    P_MACRO(BSLS_PLATFORM_OS_DARWIN);
#else
    D_MACRO(BSLS_PLATFORM_OS_DARWIN);
#endif

#if defined(BSLS_PLATFORM_OS_FREEBSD)
    P_MACRO(BSLS_PLATFORM_OS_FREEBSD);
#else
    D_MACRO(BSLS_PLATFORM_OS_FREEBSD);
#endif

#if defined(BSLS_PLATFORM_OS_LINUX)
    P_MACRO(BSLS_PLATFORM_OS_LINUX);
#else
    D_MACRO(BSLS_PLATFORM_OS_LINUX);
#endif

#if defined(BSLS_PLATFORM_OS_SOLARIS)
    P_MACRO(BSLS_PLATFORM_OS_SOLARIS);
#else
    D_MACRO(BSLS_PLATFORM_OS_SOLARIS);
#endif

#if defined(BSLS_PLATFORM_OS_SUNOS)
    P_MACRO(BSLS_PLATFORM_OS_SUNOS);
#else
    D_MACRO(BSLS_PLATFORM_OS_SUNOS);
#endif

#if defined(BSLS_PLATFORM_OS_UNIX)
    P_MACRO(BSLS_PLATFORM_OS_UNIX);
#else
    D_MACRO(BSLS_PLATFORM_OS_UNIX);
#endif

#if defined(BSLS_PLATFORM_OS_VER_MAJOR)
    P_MACRO(BSLS_PLATFORM_OS_VER_MAJOR);
#else
    D_MACRO(BSLS_PLATFORM_OS_VER_MAJOR);
#endif

#if defined(BSLS_PLATFORM_OS_VER_MINOR)
    P_MACRO(BSLS_PLATFORM_OS_VER_MINOR);
#else
    D_MACRO(BSLS_PLATFORM_OS_VER_MINOR);
#endif

#if defined(BSLS_PLATFORM_OS_WIN2K)
    P_MACRO(BSLS_PLATFORM_OS_WIN2K);
#else
    D_MACRO(BSLS_PLATFORM_OS_WIN2K);
#endif

#if defined(BSLS_PLATFORM_OS_WIN9X)
    P_MACRO(BSLS_PLATFORM_OS_WIN9X);
#else
    D_MACRO(BSLS_PLATFORM_OS_WIN9X);
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
    P_MACRO(BSLS_PLATFORM_OS_WINDOWS);
#else
    D_MACRO(BSLS_PLATFORM_OS_WINDOWS);
#endif

#if defined(BSLS_PLATFORM_OS_WINNT)
    P_MACRO(BSLS_PLATFORM_OS_WINNT);
#else
    D_MACRO(BSLS_PLATFORM_OS_WINNT);
#endif

#if defined(BSLS_PLATFORM_OS_WINXP)
    P_MACRO(BSLS_PLATFORM_OS_WINXP);
#else
    D_MACRO(BSLS_PLATFORM_OS_WINXP);
#endif

#if defined(BSLS_PLATFORM_OS_WINS03)
    P_MACRO(BSLS_PLATFORM_OS_WINS03);
#else
    D_MACRO(BSLS_PLATFORM_OS_WINS03);
#endif

#if defined(BSLS_PLATFORM_OS_WINS08)
    P_MACRO(BSLS_PLATFORM_OS_WINS08);
#else
    D_MACRO(BSLS_PLATFORM_OS_WINS08);
#endif

#if defined(BSLS_PLATFORM_OS_WINVISTA)
    P_MACRO(BSLS_PLATFORM_OS_WINVISTA);
#else
    D_MACRO(BSLS_PLATFORM_OS_WINVISTA);
#endif

#if defined(BSLS_PLATFORM_OS_WIN7)
    P_MACRO(BSLS_PLATFORM_OS_WIN7);
#else
    D_MACRO(BSLS_PLATFORM_OS_WIN7);
#endif

#if defined(BSLS_PLATFORM_OS_WIN8)
    P_MACRO(BSLS_PLATFORM_OS_WIN8);
#else
    D_MACRO(BSLS_PLATFORM_OS_WIN8);
#endif

#if defined(BSLS_PLATFORM_OS_WINBLUE)
    P_MACRO(BSLS_PLATFORM_OS_WINBLUE);
#else
    D_MACRO(BSLS_PLATFORM_OS_WINBLUE);
#endif

#if defined(BSLS_PLATFORM_OS_WIN10)
    P_MACRO(BSLS_PLATFORM_OS_WIN10);
#else
    D_MACRO(BSLS_PLATFORM_OS_WIN10);
#endif


    puts("\n  printFlags: Feature Detection Macros");
    puts(  "  ------------------------------------");

#if defined(BSLS_PLATFORM_AGGRESSIVE_INLINE)
    P_MACRO(BSLS_PLATFORM_AGGRESSIVE_INLINE);
#else
    D_MACRO(BSLS_PLATFORM_AGGRESSIVE_INLINE);
#endif

#if defined(BSLS_PLATFORM_HAS_MACRO_PUSH_POP)
    P_MACRO(BSLS_PLATFORM_HAS_MACRO_PUSH_POP);
#else
    D_MACRO(BSLS_PLATFORM_HAS_MACRO_PUSH_POP);
#endif

#if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
    P_MACRO(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC);
#else
    D_MACRO(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC);
#endif

#if defined(BSLS_PLATFORM_NO_64_BIT_LITERALS)
    P_MACRO(BSLS_PLATFORM_NO_64_BIT_LITERALS);
#else
    D_MACRO(BSLS_PLATFORM_NO_64_BIT_LITERALS);
#endif


    puts("\n  printFlags: Deprecated macros for legacy support");
    puts(  "  ------------------------------------------------");

#if defined(BSLS_PLATFORM__CMP_AIX)
    P_MACRO(BSLS_PLATFORM__CMP_AIX);
#else
    D_MACRO(BSLS_PLATFORM__CMP_AIX);
#endif

#if defined(BSLS_PLATFORM__CMP_CLANG)
    P_MACRO(BSLS_PLATFORM__CMP_CLANG);
#else
    D_MACRO(BSLS_PLATFORM__CMP_CLANG);
#endif

#if defined(BSLS_PLATFORM__CMP_EDG)
    P_MACRO(BSLS_PLATFORM__CMP_EDG);
#else
    D_MACRO(BSLS_PLATFORM__CMP_EDG);
#endif

#if defined(BSLS_PLATFORM__CMP_GNU)
    P_MACRO(BSLS_PLATFORM__CMP_GNU);
#else
    D_MACRO(BSLS_PLATFORM__CMP_GNU);
#endif

#if defined(BSLS_PLATFORM__CMP_HP)
    P_MACRO(BSLS_PLATFORM__CMP_HP);
#else
    D_MACRO(BSLS_PLATFORM__CMP_HP);
#endif

#if defined(BSLS_PLATFORM__CMP_IBM)
    P_MACRO(BSLS_PLATFORM__CMP_IBM);
#else
    D_MACRO(BSLS_PLATFORM__CMP_IBM);
#endif

#if defined(BSLS_PLATFORM__CMP_MSVC)
    P_MACRO(BSLS_PLATFORM__CMP_MSVC);
#else
    D_MACRO(BSLS_PLATFORM__CMP_MSVC);
#endif

#if defined(BSLS_PLATFORM__CMP_SUN)
    P_MACRO(BSLS_PLATFORM__CMP_SUN);
#else
    D_MACRO(BSLS_PLATFORM__CMP_SUN);
#endif

#if defined(BSLS_PLATFORM__CMP_VERSION)
    P_MACRO(BSLS_PLATFORM__CMP_VERSION);
#else
    D_MACRO(BSLS_PLATFORM__CMP_VERSION);
#endif

#if defined(BSLS_PLATFORM__CMP_VER_MAJOR)
    P_MACRO(BSLS_PLATFORM__CMP_VER_MAJOR);
#else
    D_MACRO(BSLS_PLATFORM__CMP_VER_MAJOR);
#endif

#if defined(BSLS_PLATFORM__CPU_32_BIT)
    P_MACRO(BSLS_PLATFORM__CPU_32_BIT);
#else
    D_MACRO(BSLS_PLATFORM__CPU_32_BIT);
#endif

#if defined(BSLS_PLATFORM__CPU_64_BIT)
    P_MACRO(BSLS_PLATFORM__CPU_64_BIT);
#else
    D_MACRO(BSLS_PLATFORM__CPU_64_BIT);
#endif

#if defined(BSLS_PLATFORM__CPU_POWERPC)
    P_MACRO(BSLS_PLATFORM__CPU_POWERPC);
#else
    D_MACRO(BSLS_PLATFORM__CPU_POWERPC);
#endif

#if defined(BSLS_PLATFORM__CPU_X86)
    P_MACRO(BSLS_PLATFORM__CPU_X86);
#else
    D_MACRO(BSLS_PLATFORM__CPU_X86);
#endif

#if defined(BSLS_PLATFORM__CPU_X86_64)
    P_MACRO(BSLS_PLATFORM__CPU_X86_64);
#else
    D_MACRO(BSLS_PLATFORM__CPU_X86_64);
#endif

#if defined(BSLS_PLATFORM__IS_BIG_ENDIAN)
    P_MACRO(BSLS_PLATFORM__IS_BIG_ENDIAN);
#else
    D_MACRO(BSLS_PLATFORM__IS_BIG_ENDIAN);
#endif

#if defined(BSLS_PLATFORM__IS_LITTLE_ENDIAN)
    P_MACRO(BSLS_PLATFORM__IS_LITTLE_ENDIAN);
#else
    D_MACRO(BSLS_PLATFORM__IS_LITTLE_ENDIAN);
#endif

#if defined(BSLS_PLATFORM__OS_AIX)
    P_MACRO(BSLS_PLATFORM__OS_AIX);
#else
    D_MACRO(BSLS_PLATFORM__OS_AIX);
#endif

#if defined(BSLS_PLATFORM__OS_CYGWIN)
    P_MACRO(BSLS_PLATFORM__OS_CYGWIN);
#else
    D_MACRO(BSLS_PLATFORM__OS_CYGWIN);
#endif

#if defined(BSLS_PLATFORM__OS_DARWIN)
    P_MACRO(BSLS_PLATFORM__OS_DARWIN);
#else
    D_MACRO(BSLS_PLATFORM__OS_DARWIN);
#endif

#if defined(BSLS_PLATFORM__OS_LINUX)
    P_MACRO(BSLS_PLATFORM__OS_LINUX);
#else
    D_MACRO(BSLS_PLATFORM__OS_LINUX);
#endif

#if defined(BSLS_PLATFORM__OS_SOLARIS)
    P_MACRO(BSLS_PLATFORM__OS_SOLARIS);
#else
    D_MACRO(BSLS_PLATFORM__OS_SOLARIS);
#endif

#if defined(BSLS_PLATFORM__OS_SUNOS)
    P_MACRO(BSLS_PLATFORM__OS_SUNOS);
#else
    D_MACRO(BSLS_PLATFORM__OS_SUNOS);
#endif

#if defined(BSLS_PLATFORM__OS_UNIX)
    P_MACRO(BSLS_PLATFORM__OS_UNIX);
#else
    D_MACRO(BSLS_PLATFORM__OS_UNIX);
#endif

#if defined(BSLS_PLATFORM__OS_WINDOWS)
    P_MACRO(BSLS_PLATFORM__OS_WINDOWS);
#else
    D_MACRO(BSLS_PLATFORM__OS_WINDOWS);
#endif

#if defined(BDES_PLATFORM__CMP_AIX)
    P_MACRO(BDES_PLATFORM__CMP_AIX);
#else
    D_MACRO(BDES_PLATFORM__CMP_AIX);
#endif

#if defined(BDES_PLATFORM__CMP_GNU)
    P_MACRO(BDES_PLATFORM__CMP_GNU);
#else
    D_MACRO(BDES_PLATFORM__CMP_GNU);
#endif

#if defined(BDES_PLATFORM__CMP_HP)
    P_MACRO(BDES_PLATFORM__CMP_HP);
#else
    D_MACRO(BDES_PLATFORM__CMP_HP);
#endif

#if defined(BDES_PLATFORM__CMP_MSVC)
    P_MACRO(BDES_PLATFORM__CMP_MSVC);
#else
    D_MACRO(BDES_PLATFORM__CMP_MSVC);
#endif

#if defined(BDES_PLATFORM__CMP_SUN)
    P_MACRO(BDES_PLATFORM__CMP_SUN);
#else
    D_MACRO(BDES_PLATFORM__CMP_SUN);
#endif

#if defined(BDES_PLATFORM__CMP_VER_MAJOR)
    P_MACRO(BDES_PLATFORM__CMP_VER_MAJOR);
#else
    D_MACRO(BDES_PLATFORM__CMP_VER_MAJOR);
#endif

#if defined(BDES_PLATFORM__CPU_64_BIT)
    P_MACRO(BDES_PLATFORM__CPU_64_BIT);
#else
    D_MACRO(BDES_PLATFORM__CPU_64_BIT);
#endif

#if defined(BDES_PLATFORM__OS_AIX)
    P_MACRO(BDES_PLATFORM__OS_AIX);
#else
    D_MACRO(BDES_PLATFORM__OS_AIX);
#endif

#if defined(BDES_PLATFORM__OS_CYGWIN)
    P_MACRO(BDES_PLATFORM__OS_CYGWIN);
#else
    D_MACRO(BDES_PLATFORM__OS_CYGWIN);
#endif

#if defined(BDES_PLATFORM__OS_DARWIN)
    P_MACRO(BDES_PLATFORM__OS_DARWIN);
#else
    D_MACRO(BDES_PLATFORM__OS_DARWIN);
#endif

#if defined(BDES_PLATFORM__OS_FREEBSD)
    P_MACRO(BDES_PLATFORM__OS_FREEBSD);
#else
    D_MACRO(BDES_PLATFORM__OS_FREEBSD);
#endif

#if defined(BDES_PLATFORM__OS_LINUX)
    P_MACRO(BDES_PLATFORM__OS_LINUX);
#else
    D_MACRO(BDES_PLATFORM__OS_LINUX);
#endif

#if defined(BDES_PLATFORM__OS_SOLARIS)
    P_MACRO(BDES_PLATFORM__OS_SOLARIS);
#else
    D_MACRO(BDES_PLATFORM__OS_SOLARIS);
#endif

#if defined(BDES_PLATFORM__OS_SUNOS)
    P_MACRO(BDES_PLATFORM__OS_SUNOS);
#else
    D_MACRO(BDES_PLATFORM__OS_SUNOS);
#endif

#if defined(BDES_PLATFORM__OS_UNIX)
    P_MACRO(BDES_PLATFORM__OS_UNIX);
#else
    D_MACRO(BDES_PLATFORM__OS_UNIX);
#endif

#if defined(BDES_PLATFORM__OS_VER_MAJOR)
    P_MACRO(BDES_PLATFORM__OS_VER_MAJOR);
#else
    D_MACRO(BDES_PLATFORM__OS_VER_MAJOR);
#endif

#if defined(BDES_PLATFORM__OS_VER_MINOR)
    P_MACRO(BDES_PLATFORM__OS_VER_MINOR);
#else
    D_MACRO(BDES_PLATFORM__OS_VER_MINOR);
#endif

#if defined(BDES_PLATFORM__OS_WIN2K)
    P_MACRO(BDES_PLATFORM__OS_WIN2K);
#else
    D_MACRO(BDES_PLATFORM__OS_WIN2K);
#endif

#if defined(BDES_PLATFORM__OS_WIN9X)
    P_MACRO(BDES_PLATFORM__OS_WIN9X);
#else
    D_MACRO(BDES_PLATFORM__OS_WIN9X);
#endif

#if defined(BDES_PLATFORM__OS_WINDOWS)
    P_MACRO(BDES_PLATFORM__OS_WINDOWS);
#else
    D_MACRO(BDES_PLATFORM__OS_WINDOWS);
#endif

#if defined(BDES_PLATFORM__OS_WINNT)
    P_MACRO(BDES_PLATFORM__OS_WINNT);
#else
    D_MACRO(BDES_PLATFORM__OS_WINNT);
#endif

#if defined(BDES_PLATFORM__OS_WINXP)
    P_MACRO(BDES_PLATFORM__OS_WINXP);
#else
    D_MACRO(BDES_PLATFORM__OS_WINXP);
#endif


    puts("\n  printFlags: platform macros of interest to our configuration");
    puts(  "  ------------------------------------------------------------");

                      // AIX platform specific macros
#if defined(_AIX)
    P_MACRO(_AIX);
#else
    D_MACRO(_AIX);
#endif

#if defined(_ARCH_601)
    P_MACRO(_ARCH_601);
#else
    D_MACRO(_ARCH_601);
#endif

#if defined(_ARCH_COM)
    P_MACRO(_ARCH_COM);
#else
    D_MACRO(_ARCH_COM);
#endif

#if defined(_ARCH_POWER)
    P_MACRO(_ARCH_POWER);
#else
    D_MACRO(_ARCH_POWER);
#endif

#if defined(_ARCH_PPC)
    P_MACRO(_ARCH_PPC);
#else
    D_MACRO(_ARCH_PPC);
#endif

#if defined(_ARCH_PPC64)
    P_MACRO(_ARCH_PPC64);
#else
    D_MACRO(_ARCH_PPC64);
#endif

#if defined(_ARCH_PWR)
    P_MACRO(_ARCH_PWR);
#else
    D_MACRO(_ARCH_PWR);
#endif

#if defined(_ARCH_PWR2)
    P_MACRO(_ARCH_PWR2);
#else
    D_MACRO(_ARCH_PWR2);
#endif

#if defined(_BIG_ENDIAN)
    P_MACRO(_BIG_ENDIAN);
#else
    D_MACRO(_BIG_ENDIAN);
#endif

#if defined(_IA64)
    P_MACRO(_IA64);
#else
    D_MACRO(_IA64);
#endif

#if defined(_ILP32)
    P_MACRO(_ILP32);
#else
    D_MACRO(_ILP32);
#endif

#if defined(_LITTLE_ENDIAN)
    P_MACRO(_LITTLE_ENDIAN);
#else
    D_MACRO(_LITTLE_ENDIAN);
#endif

#if defined(_LP64)
    P_MACRO(_LP64);
#else
    D_MACRO(_LP64);
#endif

             // Microsoft Visual Studio compiler specific macros

    // Source for compiler specific predefined macros was
    //: https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros
    // at 2020-12-13 03:15-05:00 (EST/New York).  Please visit that page for
    // interpretation of the Microsoft specific macros/values.

#if defined(_MSC_VER)
    P_MACRO(_MSC_VER);
#else
    D_MACRO(_MSC_VER);
#endif

#if defined(_MSC_FULL_VER)
    P_MACRO(_MSC_FULL_VER);
#else
    D_MACRO(_MSC_FULL_VER);
#endif

#if defined(_MSC_BUILD)
    P_MACRO(_MSC_BUILD);
#else
    D_MACRO(_MSC_BUILD);
#endif

#if defined(_MSVC_LANG)
    P_MACRO(_MSVC_LANG);
#else
    D_MACRO(_MSVC_LANG);
#endif

#if defined(__INTELLISENSE__)
    P_MACRO(__INTELLISENSE__);
#else
    D_MACRO(__INTELLISENSE__);
#endif

#if defined(_INTEGRAL_MAX_BITS)
    P_MACRO(_INTEGRAL_MAX_BITS);
#else
    D_MACRO(_INTEGRAL_MAX_BITS);
#endif

    // Microsoft Visual Studio implementation-defined-feature detection

#if defined(_CHAR_UNSIGNED)
    P_MACRO(_CHAR_UNSIGNED);
#else
    D_MACRO(_CHAR_UNSIGNED);
#endif

    // Microsoft Visual Studio floating point mode feature detection

#if defined(_M_FP_CONTRACT)
    P_MACRO(_M_FP_CONTRACT);
#else
    D_MACRO(_M_FP_CONTRACT);
#endif

#if defined(_M_FP_EXCEPT)
    P_MACRO(_M_FP_EXCEPT);
#else
    D_MACRO(_M_FP_EXCEPT);
#endif

#if defined(_M_FP_FAST)
    P_MACRO(_M_FP_FAST);
#else
    D_MACRO(_M_FP_FAST);
#endif

#if defined(_M_FP_PRECISE)
    P_MACRO(_M_FP_PRECISE);
#else
    D_MACRO(_M_FP_PRECISE);
#endif

#if defined(_M_FP_STRICT)
    P_MACRO(_M_FP_STRICT);
#else
    D_MACRO(_M_FP_STRICT);
#endif

    // Microsoft Visual Studio optional-feature detection

#if defined(_MT)
    P_MACRO(_MT);
#else
    D_MACRO(_MT);
#endif

#if defined(_OPENMP)
    P_MACRO(_OPENMP);
#else
    D_MACRO(_OPENMP);
#endif

#if defined(_MSC_EXTENSIONS)
    P_MACRO(_MSC_EXTENSIONS);
#else
    D_MACRO(_MSC_EXTENSIONS);
#endif

#if defined(_MSVC_TRADITIONAL)
    P_MACRO(_MSVC_TRADITIONAL);
#else
    D_MACRO(_MSVC_TRADITIONAL);
#endif

#if defined(_NATIVE_WCHAR_T_DEFINED)
    P_MACRO(_NATIVE_WCHAR_T_DEFINED);
#else
    D_MACRO(_NATIVE_WCHAR_T_DEFINED);
#endif

#if defined(_WCHAR_T_DEFINED)
    P_MACRO(_WCHAR_T_DEFINED);
#else
    D_MACRO(_WCHAR_T_DEFINED);
#endif

#if defined(_CPPRTTI)
    P_MACRO(_CPPRTTI);
#else
    D_MACRO(_CPPRTTI);
#endif

#if defined(_CPPUNWIND)
    P_MACRO(_CPPUNWIND);
#else
    D_MACRO(_CPPUNWIND);
#endif

#if defined(_ISO_VOLATILE)
    P_MACRO(_ISO_VOLATILE);
#else
    D_MACRO(_ISO_VOLATILE);
#endif

#if defined(_KERNEL_MODE)
    P_MACRO(_KERNEL_MODE);
#else
    D_MACRO(_KERNEL_MODE);
#endif

    // Microsoft Visual Studio security features detection

#if defined(_CONTROL_FLOW_GUARD)
    P_MACRO(_CONTROL_FLOW_GUARD);
#else
    D_MACRO(_CONTROL_FLOW_GUARD);
#endif

#if defined(_MSVC_RUNTIME_CHECKS)
    P_MACRO(_MSVC_RUNTIME_CHECKS);
#else
    D_MACRO(_MSVC_RUNTIME_CHECKS);
#endif

    // Microsoft Visual Studio sanitizer related

#if defined(__SANITIZE_ADDRESS__)
    P_MACRO(__SANITIZE_ADDRESS__);
#else
    D_MACRO(__SANITIZE_ADDRESS__);
#endif

    // Microsoft Visual Studio build configuration specific

#if defined(_DLL)
    P_MACRO(_DLL);
#else
    D_MACRO(_DLL);
#endif

#if defined(_PREFAST_)
    P_MACRO(_PREFAST_);
#else
    D_MACRO(_PREFAST_);
#endif

#if defined(_VC_NODEFAULTLIB)
    P_MACRO(_VC_NODEFAULTLIB);
#else
    D_MACRO(_VC_NODEFAULTLIB);
#endif

    // Microsoft CLR, CLI, and .NET specific macros

#if defined(__CLR_VER)
    P_MACRO(__CLR_VER);
#else
    D_MACRO(__CLR_VER);
#endif

#if defined(__cplusplus_cli)
    P_MACRO(__cplusplus_cli);
#else
    D_MACRO(__cplusplus_cli);
#endif

#if defined(_MANAGED)
    P_MACRO(_MANAGED);
#else
    D_MACRO(_MANAGED);
#endif

#if defined(_M_CEE)
    P_MACRO(_M_CEE);
#else
    D_MACRO(_M_CEE);
#endif

#if defined(_M_CEE_PURE)
    P_MACRO(_M_CEE_PURE);
#else
    D_MACRO(_M_CEE_PURE);
#endif

#if defined(_M_CEE_SAFE)
    P_MACRO(_M_CEE_SAFE);
#else
    D_MACRO(_M_CEE_SAFE);
#endif

    // Microsoft Visual Studio hardware platform detection

#if defined(_M_ALPHA)
    P_MACRO(_M_ALPHA);
#else
    D_MACRO(_M_ALPHA);
#endif

#if defined(_M_AMD64)
    P_MACRO(_M_AMD64);
#else
    D_MACRO(_M_AMD64);
#endif

#if defined(_M_IA64)
    P_MACRO(_M_IA64);
#else
    D_MACRO(_M_IA64);
#endif

#if defined(_M_IX86)
    P_MACRO(_M_IX86);
#else
    D_MACRO(_M_IX86);
#endif

#if defined(_M_X64)
    P_MACRO(_M_X64);
#else
    D_MACRO(_M_X64);
#endif

    // Macros predefined for x86 or x64 MSVC targets only -- BEGIN vvv

#if defined(_M_IX86_FP)
    P_MACRO(_M_IX86_FP);
#else
    D_MACRO(_M_IX86_FP);
#endif

#if defined(__ATOM__)
    P_MACRO(__ATOM__);
#else
    D_MACRO(__ATOM__);
#endif

#if defined(__AVX__)
    P_MACRO(__AVX__);
#else
    D_MACRO(__AVX__);
#endif

#if defined(__AVX2__)
    P_MACRO(__AVX2__);
#else
    D_MACRO(__AVX2__);
#endif

#if defined(__AVX__)
    P_MACRO(__AVX__);
#else
    D_MACRO(__AVX__);
#endif

#if defined(__AVX512BW__)
    P_MACRO(__AVX512BW__);
#else
    D_MACRO(__AVX512BW__);
#endif

#if defined(__AVX512CD__)
    P_MACRO(__AVX512CD__);
#else
    D_MACRO(__AVX512CD__);
#endif

#if defined(__AVX512DQ__)
    P_MACRO(__AVX512DQ__);
#else
    D_MACRO(__AVX512DQ__);
#endif

#if defined(__AVX512F__)
    P_MACRO(__AVX512F__);
#else
    D_MACRO(__AVX512F__);
#endif

#if defined(__AVX512VL__)
    P_MACRO(__AVX512VL__);
#else
    D_MACRO(__AVX512VL__);
#endif

    // Macros predefined for x86 or x64 MSVC targets only -- END ^^^

    // Microsoft Visual Studio ARM related hardware detection

#if defined(_ARM)
    P_MACRO(_ARM);
#else
    D_MACRO(_ARM);
#endif

#if defined(_ARM_ARMV7VE)
    P_MACRO(_ARM_ARMV7VE);
#else
    D_MACRO(_ARM_ARMV7VE);
#endif

#if defined(_ARM_FP)
    P_MACRO(_ARM_FP);
#else
    D_MACRO(_ARM_FP);
#endif

#if defined(_ARM64)
    P_MACRO(_ARM64);
#else
    D_MACRO(_ARM64);
#endif

#if defined(_ARM64EC)
    P_MACRO(_ARM64EC);
#else
    D_MACRO(_ARM64EC);
#endif

    // Microsoft Visual Studio miscellaneous hardware detection

#if defined(_POWER)
    P_MACRO(_POWER);
#else
    D_MACRO(_POWER);
#endif

#if defined(_M_MRX000)
    P_MACRO(_M_MRX000);
#else
    D_MACRO(_M_MRX000);
#endif

#if defined(_M_PPC)
    P_MACRO(_M_PPC);
#else
    D_MACRO(_M_PPC);
#endif

#if defined(_POWER)
    P_MACRO(_POWER);
#else
    D_MACRO(_POWER);
#endif

    // Microsoft Windows platform specific macros

#if defined(__cplusplus_winrt)
    P_MACRO(__cplusplus_winrt);
#else
    D_MACRO(__cplusplus_winrt);
#endif

#if defined(_WINRT_DLL)
    P_MACRO(_WINRT_DLL);
#else
    D_MACRO(_WINRT_DLL);
#endif

#if defined(_WIN16)
    P_MACRO(_WIN16);
#else
    D_MACRO(_WIN16);
#endif

#if defined(_WIN32)
    P_MACRO(_WIN32);
#else
    D_MACRO(_WIN32);
#endif

#if defined(_WIN32_WINDOWS)
    P_MACRO(_WIN32_WINDOWS);
#else
    D_MACRO(_WIN32_WINDOWS);
#endif

#if defined(_WIN32_WINNT)
    P_MACRO(_WIN32_WINNT);
#else
    D_MACRO(_WIN32_WINNT);
#endif

#if defined(_WIN64)
    P_MACRO(_WIN64);
#else
    D_MACRO(_WIN64);
#endif

#if defined(__64BIT__)
    P_MACRO(__64BIT__);
#else
    D_MACRO(__64BIT__);
#endif

                // clang compiler platform specific macros

#if defined(__APPLE__)
    P_MACRO(__APPLE__);
#else
    D_MACRO(__APPLE__);
#endif

#if defined(__ARM_ARCH)
    P_MACRO(__ARM_ARCH);
#else
    D_MACRO(__ARM_ARCH);
#endif

#if defined(__ARM_ARCH_5TEJ__)
    P_MACRO(__ARM_ARCH_5TEJ__);
#else
    D_MACRO(__ARM_ARCH_5TEJ__);
#endif

#if defined(__ARM_ARCH_5TE__)
    P_MACRO(__ARM_ARCH_5TE__);
#else
    D_MACRO(__ARM_ARCH_5TE__);
#endif

#if defined(__ARM_ARCH_5T__)
    P_MACRO(__ARM_ARCH_5T__);
#else
    D_MACRO(__ARM_ARCH_5T__);
#endif

#if defined(__ARM_ARCH_6__)
    P_MACRO(__ARM_ARCH_6__);
#else
    D_MACRO(__ARM_ARCH_6__);
#endif

#if defined(__ARM_ARCH_7A__)
    P_MACRO(__ARM_ARCH_7A__);
#else
    D_MACRO(__ARM_ARCH_7A__);
#endif

#if defined(__ARM_ARCH_7M__)
    P_MACRO(__ARM_ARCH_7M__);
#else
    D_MACRO(__ARM_ARCH_7M__);
#endif

#if defined(__ARM_ARCH_7R__)
    P_MACRO(__ARM_ARCH_7R__);
#else
    D_MACRO(__ARM_ARCH_7R__);
#endif

#if defined(__ARM_ARCH_7__)
    P_MACRO(__ARM_ARCH_7__);
#else
    D_MACRO(__ARM_ARCH_7__);
#endif

#if defined(__APPLE_CC__)
    P_MACRO(__APPLE_CC__);
#else
    D_MACRO(__APPLE_CC__);
#endif

#if defined(__CHAR_UNSIGNED__)
    P_MACRO(__CHAR_UNSIGNED__);
#else
    D_MACRO(__CHAR_UNSIGNED__);
#endif

                 // EDG compiler platform specific macros

#if defined(__EDG__)
    P_MACRO(__EDG__);
#else
    D_MACRO(__EDG__);
#endif

                    // CygWin platform specific macros

#if defined(__CYGWIN__)
    P_MACRO(__CYGWIN__);
#else
    D_MACRO(__CYGWIN__);
#endif

                     // OSS platform specific macros


#if defined(__FreeBSD__)
    P_MACRO(__FreeBSD__);
#else
    D_MACRO(__FreeBSD__);
#endif

#if defined(__GLIBC__)
    P_MACRO(__GLIBC__);
#else
    D_MACRO(__GLIBC__);
#endif

#if defined(__GNUC_PATCHLEVEL__)
    P_MACRO(__GNUC_PATCHLEVEL__);
#else
    D_MACRO(__GNUC_PATCHLEVEL__);
#endif

#if defined(__GNUC__)
    P_MACRO(__GNUC__);
#else
    D_MACRO(__GNUC__);
#endif

#if defined(__GNUC_GNU_INLINE__)
    P_MACRO(__GNUC_GNU_INLINE__);
#else
    D_MACRO(__GNUC_GNU_INLINE__);
#endif

#if defined(__GNUC_STDC_INLINE__)
    P_MACRO(__GNUC_STDC_INLINE__);
#else
    D_MACRO(__GNUC_STDC_INLINE__);
#endif

#if defined(__GXX_EXPERIMENTAL_CXX0X__)
    P_MACRO(__GXX_EXPERIMENTAL_CXX0X__);
#else
    D_MACRO(__GXX_EXPERIMENTAL_CXX0X__);
#endif

#if defined(__HP_cc)
    P_MACRO(__HP_cc);
#else
    D_MACRO(__HP_cc);
#endif

#if defined(__HP_aCC)
    P_MACRO(__HP_aCC);
#else
    D_MACRO(__HP_aCC);
#endif

#if defined(__IA64__)
    P_MACRO(__IA64__);
#else
    D_MACRO(__IA64__);
#endif

#if defined(__IBMC__)
    P_MACRO(__IBMC__);
#else
    D_MACRO(__IBMC__);
#endif

#if defined(__IBMCPP__)
    P_MACRO(__IBMCPP__);
#else
    D_MACRO(__IBMCPP__);
#endif

#if defined(__LP64__)
    P_MACRO(__LP64__);
#else
    D_MACRO(__LP64__);
#endif

#if defined(__PIC__)
    P_MACRO(__PIC__);
#else
    D_MACRO(__PIC__);
#endif

#if defined(__POWERPC__)
    P_MACRO(__POWERPC__);
#else
    D_MACRO(__POWERPC__);
#endif

#if defined(__SUNPRO_C)
    P_MACRO(__SUNPRO_C);
#else
    D_MACRO(__SUNPRO_C);
#endif

#if defined(__SUNPRO_CC)
    P_MACRO(__SUNPRO_CC);
#else
    D_MACRO(__SUNPRO_CC);
#endif

#if defined(__SVR4)
    P_MACRO(__SVR4);
#else
    D_MACRO(__SVR4);
#endif

#if defined(__SunOS_5_10)
    P_MACRO(__SunOS_5_10);
#else
    D_MACRO(__SunOS_5_10);
#endif

#if defined(__SunOS_5_11)
    P_MACRO(__SunOS_5_11);
#else
    D_MACRO(__SunOS_5_11);
#endif

#if defined(__SunOS_5_7)
    P_MACRO(__SunOS_5_7);
#else
    D_MACRO(__SunOS_5_7);
#endif

#if defined(__SunOS_5_8)
    P_MACRO(__SunOS_5_8);
#else
    D_MACRO(__SunOS_5_8);
#endif

#if defined(__SunOS_5_9)
    P_MACRO(__SunOS_5_9);
#else
    D_MACRO(__SunOS_5_9);
#endif

#if defined(__WCHAR_UNSIGNED__)
    P_MACRO(__WCHAR_UNSIGNED__);
#else
    D_MACRO(__WCHAR_UNSIGNED__);
#endif

#if defined(__WIN32__)
    P_MACRO(__WIN32__);
#else
    D_MACRO(__WIN32__);
#endif

#if defined(__alpha__)
    P_MACRO(__alpha__);
#else
    D_MACRO(__alpha__);
#endif

#if defined(__arch64__)
    P_MACRO(__arch64__);
#else
    D_MACRO(__arch64__);
#endif

#if defined(__arm__)
    P_MACRO(__arm__);
#else
    D_MACRO(__arm__);
#endif

#if defined(__clang__)
    P_MACRO(__clang__);
#else
    D_MACRO(__clang__);
#endif

#if defined(__cygwin)
    P_MACRO(__cygwin);
#else
    D_MACRO(__cygwin);
#endif

#if defined(__hppa)
    P_MACRO(__hppa);
#else
    D_MACRO(__hppa);
#endif

#if defined(__hppa__)
    P_MACRO(__hppa__);
#else
    D_MACRO(__hppa__);
#endif

#if defined(__hpux)
    P_MACRO(__hpux);
#else
    D_MACRO(__hpux);
#endif

#if defined(__i386)
    P_MACRO(__i386);
#else
    D_MACRO(__i386);
#endif

#if defined(__i386__)
    P_MACRO(__i386__);
#else
    D_MACRO(__i386__);
#endif

#if defined(__ia64)
    P_MACRO(__ia64);
#else
    D_MACRO(__ia64);
#endif

#if defined(__ia64__)
    P_MACRO(__ia64__);
#else
    D_MACRO(__ia64__);
#endif

#if defined(__ix86)
    P_MACRO(__ix86);
#else
    D_MACRO(__ix86);
#endif

#if defined(__ix86__)
    P_MACRO(__ix86__);
#else
    D_MACRO(__ix86__);
#endif

#if defined(__linux)
    P_MACRO(__linux);
#else
    D_MACRO(__linux);
#endif

#if defined(__linux__)
    P_MACRO(__linux__);
#else
    D_MACRO(__linux__);
#endif

#if defined(__mips__)
    P_MACRO(__mips__);
#else
    D_MACRO(__mips__);
#endif

#if defined(__powerpc)
    P_MACRO(__powerpc);
#else
    D_MACRO(__powerpc);
#endif

#if defined(__powerpc__)
    P_MACRO(__powerpc__);
#else
    D_MACRO(__powerpc__);
#endif

#if defined(__ppc__)
    P_MACRO(__ppc__);
#else
    D_MACRO(__ppc__);
#endif

#if defined(__sparc)
    P_MACRO(__sparc);
#else
    D_MACRO(__sparc);
#endif

#if defined(__sparc64)
    P_MACRO(__sparc64);
#else
    D_MACRO(__sparc64);
#endif

#if defined(__sparc__)
    P_MACRO(__sparc__);
#else
    D_MACRO(__sparc__);
#endif

#if defined(__sparc_v9__)
    P_MACRO(__sparc_v9__);
#else
    D_MACRO(__sparc_v9__);
#endif

#if defined(__sparcv9)
    P_MACRO(__sparcv9);
#else
    D_MACRO(__sparcv9);
#endif

#if defined(__sun)
    P_MACRO(__sun);
#else
    D_MACRO(__sun);
#endif

#if defined(__svr4__)
    P_MACRO(__svr4__);
#else
    D_MACRO(__svr4__);
#endif

#if defined(__unix)
    P_MACRO(__unix);
#else
    D_MACRO(__unix);
#endif

#if defined(__unix__)
    P_MACRO(__unix__);
#else
    D_MACRO(__unix__);
#endif

#if defined(__x86_64)
    P_MACRO(__x86_64);
#else
    D_MACRO(__x86_64);
#endif

#if defined(__x86_64__)
    P_MACRO(__x86_64__);
#else
    D_MACRO(__x86_64__);
#endif

#if defined(__xlC__)
    P_MACRO(__xlC__);
#else
    D_MACRO(__xlC__);
#endif

#if defined(cygwin)
    P_MACRO(cygwin);
#else
    D_MACRO(cygwin);
#endif

#if defined(hpux)
    P_MACRO(hpux);
#else
    D_MACRO(hpux);
#endif

#if defined(i386)
    P_MACRO(i386);
#else
    D_MACRO(i386);
#endif

#if defined(linux)
    P_MACRO(linux);
#else
    D_MACRO(linux);
#endif

#if defined(macro)
    P_MACRO(macro);
#else
    D_MACRO(macro);
#endif

#if defined(sparc)
    P_MACRO(sparc);
#else
    D_MACRO(sparc);
#endif

#if defined(sun)
    P_MACRO(sun);
#else
    D_MACRO(sun);
#endif

#if defined(unix)
    P_MACRO(unix);
#else
    D_MACRO(unix);
#endif

#if defined(WINVER)
    P_MACRO(WINVER);
#else
    D_MACRO(WINVER);
#endif

    puts("\n\n  printFlags: UNDEFINED MACROS:");
    puts(    "  -----------------------------");

    for (size_t i = 0; i != undefinedMacros.count(); ++i) {
        printf("\t  %s\n", undefinedMacros[i]);
    }

    puts("\n\nprintFlags: Leave");

        // Clean up locally scoped macros
#undef D_MACRO
#undef P_MACRO
}

#ifdef _WIN32
    #define cpuid(info, x) __cpuidex(info, x, 0)
#elif (defined(__clang__) || defined(__GNUC__) || defined(__EDG__)) \
        && (BSLS_PLATFORM_CPU_X86 || BSLS_PLATFORM_CPU_X86_64)
    void cpuid(int info[4], int infoType)
        // Load into the specified 'info' the results of the intrinsic
        // '__cpuid_count' command invoked with the specified 'infoType' for
        // the 'level' parameter and zero for 'count' the parameter.  Note
        // that this intrinsic command provides information on the instruction
        // sets supported by the processor.
    {
        __cpuid_count(infoType, 0, info[0], info[1], info[2], info[3]);
    }
#else
    void cpuid(int info[4], int)
        // Load zero into each element of the specified 'info'.
    {
        info[0] = 0;
        info[1] = 0;
        info[2] = 0;
        info[3] = 0;
    }
#endif

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
      case 5: {
        // --------------------------------------------------------------------
        // TESTING SSE MACROS
        //
        // Concerns:
        //: 1 If an SSE-detection macro is defined, 'cpuid' indicates that the
        //:   associated instruction set is supported.  Note that the converse
        //:   need not be true because the compiler can be configured to not
        //:   emit SSE instructions even if the target supports them.
        //:
        //: 2 If any SSE-detection macro is defined, then either or both of
        //:   the x86-family-detection macros are also defined.
        //
        // Plan:
        //: 1 Use 'cpuinfo' to verify macro settings.
        //:
        //: 2 Ensure that either the x86- or x86_64-detection macro is defined
        //:   if any SSE-detection macro is defined.
        //
        // Testing
        //   BSLS_PLATFORM_CPU_SSE*
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING SSE MACROS"
                            "\n==================\n");

        int info[4];

        cpuid(info, 0);

        if (info[0] >= 0x00000001) {
            cpuid(info, 0x00000001);
        }

        #ifdef BSLS_PLATFORM_CPU_SSE
            ASSERT(1 == ((info[3] >> 25) & 0x1));
        #endif

        #ifdef BSLS_PLATFORM_CPU_SSE2
            ASSERT(1 == ((info[3] >> 26) & 0x1));
        #endif

        #ifdef BSLS_PLATFORM_CPU_SSE3
            ASSERT(1 == ((info[2] >>  0) & 0x1));
        #endif

        #ifdef BSLS_PLATFORM_CPU_SSE4_1
            ASSERT(1 == ((info[2] >> 19) & 0x1));
        #endif

        #ifdef BSLS_PLATFORM_CPU_SSE4_2
            ASSERT(1 == ((info[2] >> 20) & 0x1));
        #endif

        #if  defined(BSLS_PLATFORM_CPU_SSE)    \
         && !defined(BSLS_PLATFORM_CPU_X86)    \
         && !defined(BSLS_PLATFORM_CPU_X86_64)
            ASSERT(false);
        #endif

        #if  defined(BSLS_PLATFORM_CPU_SSE2)   \
         && !defined(BSLS_PLATFORM_CPU_X86)    \
         && !defined(BSLS_PLATFORM_CPU_X86_64)
            ASSERT(false);
        #endif

        #if  defined(BSLS_PLATFORM_CPU_SSE3)   \
         && !defined(BSLS_PLATFORM_CPU_X86)    \
         && !defined(BSLS_PLATFORM_CPU_X86_64)
            ASSERT(false);
        #endif

        #if  defined(BSLS_PLATFORM_CPU_SSE4_1) \
         && !defined(BSLS_PLATFORM_CPU_X86)    \
         && !defined(BSLS_PLATFORM_CPU_X86_64)
            ASSERT(false);
        #endif

        #if  defined(BSLS_PLATFORM_CPU_SSE4_2) \
         && !defined(BSLS_PLATFORM_CPU_X86)    \
         && !defined(BSLS_PLATFORM_CPU_X86_64)
            ASSERT(false);
        #endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: REPORT DEFINITION OF ALL PLATFORM MACROS
        //
        // Concerns:
        //: 1 Audit the set of macros of concern to this component.
        //
        // Plan:
        //: 1 In 'verbose' mode, iterate, in alphanumerical order within themed
        //:   groupings, over all macros that are either defined by this
        //:   component, or are platform supplied macros of interest when
        //:   defining the macros of this component.
        //:
        //:   1 If a macro is defined, write its name and value to the console.
        //:
        //:   2 If a macro is not defined, append its name to a global list of
        //:     macro names that are not defined.
        //:
        //: 2 If in 'verbose' mode, write out the list of macro names that are
        //:   not defined.
        //
        // Testing
        //   CONCERN: report definition of all platform macros.
        // --------------------------------------------------------------------

        if (verbose) printf(
              "\nTESTING CONCERN: REPORT DEFINITION OF ALL PLATFORM MACROS"
              "\n=========================================================\n");

        if (!verbose) break;

        printFlags();
      } break;
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
        // Concerns:
        //: 1 The platform is correctly detected.
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
        //   CONCERN: exactly one compiler vendor macro is defined.
        //   CONCERN: the compiler version macro is defined.
        //   CONCERN: exactly one OS type macro is defined.
        //   CONCERN: exactly one OS subtype macro is defined.
        //   CONCERN: exactly one of each CPU macro is defined.
        //   CONCERN: exactly one CPU instruction set macro is defined.
        //   CONCERN: exactly one CPU register width macro is defined.
        //   CONCERN: at most one CPU version macro is defined.
        //   CONCERN: exactly one ENDIAN macro is set.
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

        #if defined(BSLS_PLATFORM_CMP_AIX)
            MACRO_TESTGT(BSLS_PLATFORM_CMP_AIX, 0);
        #endif
        #if defined(BSLS_PLATFORM_CMP_CLANG)
            MACRO_TESTGT(BSLS_PLATFORM_CMP_CLANG, 0);
        #endif
        #if defined(BSLS_PLATFORM_CMP_EDG)
            MACRO_TESTGT(BSLS_PLATFORM_CMP_EDG, 0);
        #endif
        #if defined(BSLS_PLATFORM_CMP_GNU)
            MACRO_TESTGT(BSLS_PLATFORM_CMP_GNU, 0);
        #endif
        #if defined(BSLS_PLATFORM_CMP_HP)
            MACRO_TESTGT(BSLS_PLATFORM_CMP_HP, 0);
        #endif
        #if defined(BSLS_PLATFORM_CMP_IBM)
            MACRO_TESTGT(BSLS_PLATFORM_CMP_IBM, 0);
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

        if (veryVerbose) printf("Print OS-related Symbols:\n");

        #if defined(BSLS_PLATFORM_OS_UNIX)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_UNIX, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_AIX)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_AIX, 1);
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
        #if defined(BSLS_PLATFORM_OS_WINS03)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_WINS03, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_WINS08)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_WINS08, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_WINVISTA)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_WINVISTA, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_WIN7)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_WIN7, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_WIN8)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_WIN8, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_WINBLUE)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_WINBLUE, 1);
        #endif
        #if defined(BSLS_PLATFORM_OS_WIN10)
            MACRO_TESTEQ(BSLS_PLATFORM_OS_WIN10, 1);
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

        // Clean up locally scoped macros
#undef MACRO_TESTEQ
#undef MACRO_TESTGT
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
// Copyright 2019 Bloomberg Finance L.P.
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
