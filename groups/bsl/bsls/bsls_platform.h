// bsls_platform.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLS_PLATFORM
#define INCLUDED_BSLS_PLATFORM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide compile-time support for platform/attribute identification.
//
//@CLASSES:
//  bsls::Platform: namespace for platform traits
//
//@MACROS:
//  BSLS_PLATFORM_OS_*: operating system type, sub-type, and version
//  BSLS_PLATFORM_CPU_*: instruction set, instruction width, and version
//  BSLS_PLATFORM_CMP_*: compiler vendor, and version
//  BSLS_PLATFORM_COMPILER_ERROR: trigger a compiler error
//  BSLS_PLATFORM_AGGRESSIVE_INLINE: inline code for speed over text size
//
//@SEE_ALSO: bsls_compilerfeatures, bsls_libraryfeatures
//
//@DESCRIPTION: This component implements a suite of preprocessor macros and
// traits that identify and define platform-specific compile-time attributes.
// These attributes consist of the types and versions of (1) the operating
// system, (2) the processor(s), and (3) the compiler that together make up the
// platform.  Many of the macros defined in this component are configured
// automatically at compile-time; compile-time switches are used to configure
// the rest.  Note that compiler capabilities are better determined through the
// 'bsls_compilerfeatures' and 'bsls_libraryfeatures' components; this
// component focuses on just identifying the tool chain.
//
// Note that, for brevity, the '@' character in the following (alphabetically
// organized) tables is used to represent the characters 'BSLS_PLATFORM' --
// e.g., '@_OS_UNIX' represents 'BSLS_PLATFORM_OS_UNIX':
//..
//  =============================================================
//                           OPERATING SYSTEM
//  -------------------------------------------------------------
//  Type                Subtype                 Version
//  -----------------   -------------------     -----------------
//  @_OS_UNIX           @_OS_AIX                @_OS_VER_MAJOR
//                      @_OS_LINUX              @_OS_VER_MINOR
//                      @_OS_FREEBSD
//                      @_OS_SOLARIS
//                      @_OS_SUNOS
//                      @_OS_CYGWIN
//                      @_OS_DARWIN
//
//  @_OS_WINDOWS        @_OS_WINNT
//                      @_OS_WIN9X
//                      @_OS_WIN2K
//                      @_OS_WINXP
//                      @_OS_WINS03
//                      @_OS_WINS08 -- alias to Vista
//                      @_OS_WINVISTA
//                      @_OS_WIN7
//                      @_OS_WIN8
//                      @_OS_WINBLUE
//                      @_OS_WIN10
//
// Please see also {Windows Subtypes Explained}.
//
//  ============================================================
//                              PROCESSOR
//  ------------------------------------------------------------
//  Instruction Set     Width                   Version
//  -----------------   ------------------      ----------------
//  @_CPU_88000         @_CPU_32_BIT            @_CPU_VER_MAJOR
//  @_CPU_ALPHA         @_CPU_64_BIT
//  @_CPU_HPPA
//  @_CPU_X86
//  @_CPU_IA64
//  @_CPU_X86_64
//  @_CPU_MIPS
//  @_CPU_POWERPC
//  @_CPU_SPARC
//  @_CPU_ARM
//
//  =============================================================
//                              COMPILER
//  -------------------------------------------------------------
//  Vendor              Version
//  -----------------   ------------------
//  @_CMP_CLANG         @_CMP_VERSION
//  @_CMP_EDG           @_CMP_VER_MAJOR (deprecated)
//  @_CMP_GNU
//  @_CMP_HP
//  @_CMP_IBM
//  @_CMP_MSVC
//  @_CMP_SUN
//
//  =============================================================
//
//  =============================================================
//        ENSURE A COMPILER ERROR FOR UNSUPPORTED PLATFORMS
// --------------------------------------------------------------
//  Macro
//  ---------------
// @_COMPILER_ERROR
//
//  =============================================================
//
//  =============================================================
//                              INLINING
//  -------------------------------------------------------------
//  Macro
//  -----------------
//  @_AGGRESSIVE_INLINE
//
//  =============================================================
//..
// These macros are configured automatically, where possible.  At a minimum,
// the generic operating system type (i.e., either 'BSLS_PLATFORM_OS_UNIX' or
// 'BSLS_PLATFORM_OS_WINDOWS') is defined along with exactly one processor
// macro (e.g., 'BSLS_PLATFORM_CPU_SPARC') and exactly one compiler macro
// (e.g., 'BSLS_PLATFORM_CMP_SUN').  Clients may need to supply additional
// macros (controlled via the '-D' option of a compiler) if further
// discrimination is required (e.g., based on sub-type or version of a specific
// operating system, processor, or compiler).  Note that supplying a minor
// version number implies that the major version is also defined.
//
///Aggressive Inlining
///-------------------
// The aggressive inlining macro 'BSLS_PLATFORM_AGGRESSIVE_INLINE' is defined
// as the 'inline' keyword on all compilers except 'BSLS_PLATFORM_CMP_IBM' and
// 'BSLS_PLATFORM_CMP_SUN', where it is left empty.  This is required for some
// of our legacy applications where substantially growing the text size is not
// possible.  Even on those platforms, the symbol will be defined as 'inline'
// if 'BDE_BUILD_TARGET_AGGRESSIVE_INLINE' is passed in via the '-D' option of
// the compiler.
//
///Forcing a Compiler Error
///------------------------
// The compiler-error triggering macro 'BSLS_PLATFORM_COMPILER_ERROR' is
// defined to a sequence of tokens that guarantees a compilation error on every
// supported compiler.  This is necessary because not all compilers will
// respect the fairly obvious goal of the '#error' preprocessor directive even
// though they support it, and just issue a warning (claiming that the C++
// standard does not make a difference between a warning and an error, it knows
// only about diagnostic messages).  Code using 'bsls_platform' macros will
// often want to prevent compilation for unsupported platforms, hence this
// component provides a macro that can be used to ensure that.
//
///Windows Subtypes Explained
//---------------------------
// Windows Subtypes describe what API variation the code is built to target,
// not the actual operating system it will run on, with the following mapping
// between the macro names and Windows API versions:
//..
//  MACRO           Targeted API Version & Notes   Officially supported?
//  -------------   ----------------------------   ---------------------
//  @_OS_WINNT      NT 4.0                         NO
//  @_OS_WIN9X      95, 98, ME                     NO
//  @_OS_WIN2K      2000                           NO
//  @_OS_WINXP      XP                             NO
//  @_OS_WINS03     Server 2003                    NO
//  @_OS_WINS08     Server 2008, same as Vista     yes
//  @_OS_WINVISTA   Vista                          yes
//  @_OS_WIN7       7                              yes
//  @_OS_WIN8       8                              yes
//  @_OS_WINBLUE    8.1                            yes
//  @_OS_WIN10      10                             yes
//..
// Note that Vista is the default targeted API.
//
///Usage
///-----
// Writing portable software sometimes involves specializing implementations to
// work with platform-specific interfaces.  For example, a socket-level
// communications framework would need to operate differently on a platform
// having a Windows operating system than on one having a Unix one (but it is
// probably unnecessary to distinguish between their respective versions):
//..
//  // my_socket.h
//  #include <bsls_platform.h>
//
//  #ifdef BSLS_PLATFORM_OS_WINDOWS
//      #ifndef INCLUDED_WINSOCK2
//      #include <winsock2.h>
//      #define INCLUDED_WINSOCK2
//      #endif
//  #endif
//
//  class my_Socket {
//
//  #ifdef BSLS_PLATFORM_OS_WINDOWS
//      SOCKET d_socketObject;  // Windows SOCKET handle
//  #else
//      int d_socketObject;     // Unix socket descriptor
//  #endif
//
//  // ...
//
//  };
//..
// Certain compile-time constants are also provided as preprocessor macros that
// encapsulate the capability of determining whether a machine is big-endian or
// little-endian across all supported platforms:
//..
//  BSLS_PLATFORM_IS_BIG_ENDIAN
//  BSLS_PLATFORM_IS_LITTLE_ENDIAN
//..
// These macros are useful for writing platform-independent code, such as a
// function that converts the bytes in a 'short' to network byte order (which
// is consistent with big-endian):
//..
//  short convertToNetworkByteOrder(short input)
//      // Return the specified 'input' in network byte order.
//  {
//  #ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
//      return input;
//  #else
//      return ((input >> 8) & 0xFF) | ((input & 0xFF) << 8);
//  #endif
//  }
//..

#ifdef __cplusplus
namespace BloombergLP {
#endif

                       // ===========================
                       // struct bsls_Platform_Assert
                       // ===========================

struct bsls_Platform_Assert;
    // This 'struct' is declared but not defined.  It is used with the 'sizeof'
    // operator to force a compile-time error on platforms that do not support
    // '#error'.  For example:
    //..
    //  char die[sizeof(bsls_Platform_Assert)];  // if '#error' unsupported
    //..

#ifdef __cplusplus
}  // close enterprise namespace
#endif

// Use this macro to trigger a compile-time error if '#error' is not supported.
#ifdef __cplusplus
    #define BSLS_PLATFORM_COMPILER_ERROR                                      \
                          char die[sizeof(::BloombergLP::bsls_Platform_Assert)]
#else
    #define BSLS_PLATFORM_COMPILER_ERROR char die[sizeof(bsls_Platform_Assert)]
#endif

                        // Automatic Configuration

// IMPLEMENTATION NOTE: The following attempts to configure the system
// automatically, setting as many of the macros listed in the table above as
// possible.  Since the automatic configuration is based upon the macros set by
// the compiler, the outer-most level of detection will be driven by whatever
// compiler is in use.  Once the compiler has been determined, the
// automatic-configuration process will attempt to determine the OS and finally
// the CPU.  At the end of the section for each compiler, the macros that have
// been defined automatically will be "sanity-checked" (for consistency)
// against other macros set by the compiler.  The order of the statements in
// the process is the same as that in the table above, which is also
// alphabetical.  Any changes to the list of supported compilers should
// preserve the alphabetical order of the table, inserting the necessary
// configuration logic in the preprocessor statements below.

// ----------------------------------------------------------------------------
#if defined(__xlC__) || defined(__IBMC__) || defined(__IBMCPP__)
    #define BSLS_PLATFORM_CMP_IBM                                             1
    #define BSLS_PLATFORM_CMP_VERSION __xlC__

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    #define BSLS_PLATFORM_CMP_AIX                                             1
         // DEPRECATED: use 'BSLS_PLATFORM_CMP_IBM' instead.
#endif // BDE_OMIT_INTERNAL_DEPRECATED

    // which OS -- this compiler should only be used on AIX
    #define BSLS_PLATFORM_OS_UNIX                                             1
    #if defined(_AIX)                          // must be defined
        #define BSLS_PLATFORM_OS_AIX                                          1
        #define BSLS_PLATFORM_OS_VER_MAJOR _AIX
    #elif defined(__linux__)
        #define BSLS_PLATFORM_OS_LINUX                                        1
    #else
        #error "AIX compiler appears to be in use on non-AIX OS."
        BSLS_PLATFORM_COMPILER_ERROR;
    #endif

    // which CPU -- should always be POWERPC
    #if defined(_ARCH_PWR2)
        #define BSLS_PLATFORM_CPU_VER_MAJOR _ARCH_PWR2
    #elif defined(_ARCH_PWR)
        #define BSLS_PLATFORM_CPU_VER_MAJOR _ARCH_PWR
    #elif defined(_ARCH_POWER)
        #define BSLS_PLATFORM_CPU_VER_MAJOR _ARCH_POWER
    #elif defined(_POWER)
        #define BSLS_PLATFORM_CPU_VER_MAJOR _POWER
    #elif defined(_ARCH_COM)
        #define BSLS_PLATFORM_CPU_VER_MAJOR _ARCH_COM
    #elif defined(_ARCH_601)
        #define BSLS_PLATFORM_CPU_VER_MAJOR _ARCH_601
    #elif defined(_ARCH_PPC)
        #define BSLS_PLATFORM_CPU_VER_MAJOR _ARCH_PPC
    #elif defined(_ARCH_PPC64)
        #define BSLS_PLATFORM_CPU_VER_MAJOR _ARCH_PPC64
    #else
        #error "Unable to identify the AIX CPU."
        BSLS_PLATFORM_COMPILER_ERROR;
    #endif

    #define BSLS_PLATFORM_CPU_POWERPC                                         1
    #if defined (__64BIT__)
        #define BSLS_PLATFORM_CPU_64_BIT                                      1
    #else
        #define BSLS_PLATFORM_CPU_32_BIT                                      1
    #endif
// ----------------------------------------------------------------------------
#elif defined(_MSC_VER)
    #define BSLS_PLATFORM_CMP_MSVC                                            1
    #define BSLS_PLATFORM_CMP_VERSION _MSC_VER

    // which OS -- should be some flavor of Windows
    //  there is currently no support for:
    //  - 16-bit versions of Windows (3.x)
    //  - Windows CE
    #if defined(_WIN64) || defined(_WIN32)
        #define BSLS_PLATFORM_OS_WINDOWS                                      1
    #elif defined(_WIN16)
        #error "16-bit Windows platform not supported."
        BSLS_PLATFORM_COMPILER_ERROR;
    #else
        #error "Microsoft OS is running on an unknown platform."
        BSLS_PLATFORM_COMPILER_ERROR;
    #endif

    // which version of Windows, source sampled 2022.Dec.26 18:00EST:
    // https://learn.microsoft.com/en-us/cpp/porting/ ...
    //                                     ... modifying-winver-and-win32-winnt
    #if _WIN32_WINNT >= 0x0A00
        #define BSLS_PLATFORM_OS_WIN10                                        1
    #elif _WIN32_WINNT >= 0x0603
        #define BSLS_PLATFORM_OS_WINBLUE                                      1
    #elif _WIN32_WINNT >= 0x0602
        #define BSLS_PLATFORM_OS_WIN8                                         1
    #elif _WIN32_WINNT >= 0x0601
        #define BSLS_PLATFORM_OS_WIN7                                         1
    #elif _WIN32_WINNT >= 0x0600
        #define BSLS_PLATFORM_OS_WINVISTA                                     1
    #elif _WIN32_WINNT >= 0x0502
        #define BSLS_PLATFORM_OS_WINS03                                       1
    #elif _WIN32_WINNT >= 0x0501
        #define BSLS_PLATFORM_OS_WINXP                                        1
    #elif _WIN32_WINNT >= 0x0500
        #define BSLS_PLATFORM_OS_WIN2K                                        1
    #elif _WIN32_WINNT >= 0x0410
        #define BSLS_PLATFORM_OS_WIN9X                                        1
    #elif _WIN32_WINNT >= 0x0400
        #define BSLS_PLATFORM_OS_WINNT                                        1
    #elif defined(WINVER) && WINVER >= 0x0400                                 \
        || defined(_WIN32_WINDOWS) && _WIN32_WINDOWS >= 0x401
        #define BSLS_PLATFORM_OS_WIN9X                                        1
    #else  // default when detection fails
        #define BSLS_PLATFORM_OS_WINVISTA                                     1
    #endif

    // set API Version synonyms
    #if defined(BSLS_PLATFORM_OS_WINVISTA) && !defined(BSLS_PLATFORM_OS_WINS08)
        #define BSLS_PLATFORM_OS_WINS08                                       1
            // Windows Server 2008 is the same API as Windows Vista
    #endif
    #if !defined(BSLS_PLATFORM_OS_WINVISTA) && defined(BSLS_PLATFORM_OS_WINS08)
        #define BSLS_PLATFORM_OS_WINVISTA                                     1
            // Windows Server 2008 is the same API as Windows Vista
    #endif

    // set Version flags
    #if defined(_WIN32_WINNT)
        #define BSLS_PLATFORM_OS_VER_MAJOR _WIN32_WINNT / 0x100
        #define BSLS_PLATFORM_OS_VER_MINOR _WIN32_WINNT % 0x100
    #elif defined(WINVER)
        #define BSLS_PLATFORM_OS_VER_MAJOR WINVER / 0x100
        #define BSLS_PLATFORM_OS_VER_MINOR WINVER % 0x100
    #elif defined(_WIN32_WINDOWS)
        #define BSLS_PLATFORM_OS_VER_MAJOR _WIN32_WINDOWS / 0x100
        #define BSLS_PLATFORM_OS_VER_MINOR _WIN32_WINDOWS % 0x100
    #else                                      // default
        #define BSLS_PLATFORM_OS_VER_MAJOR 4
        #define BSLS_PLATFORM_OS_VER_MINOR 0
    #endif

    // which CPU
    //  since WinCE is not supported, neither is the HITACHI CPU
    #if defined(_M_ALPHA)
        #define BSLS_PLATFORM_CPU_ALPHA                                       1
    #elif defined(_M_IX86)
        #define BSLS_PLATFORM_CPU_X86                                         1
        #define BSLS_PLATFORM_CPU_32_BIT                                      1
        #define BSLS_PLATFORM_CPU_VER_MAJOR _M_IX86
    #elif defined(_M_IA64)
        #if defined(_WIN64)  // native mode
            #define BSLS_PLATFORM_CPU_IA64                                    1
            #define BSLS_PLATFORM_CPU_64_BIT                                  1
        #else                // emulated
            #define BSLS_PLATFORM_CPU_X86                                     1
            #define BSLS_PLATFORM_CPU_32_BIT                                  1
        #endif
        #define BSLS_PLATFORM_CPU_VER_MAJOR _M_IA64
    #elif defined(_M_AMD64)
        #if defined(_WIN64)  // native mode
            #define BSLS_PLATFORM_CPU_X86_64                                  1
            #define BSLS_PLATFORM_CPU_64_BIT                                  1
        #else                // emulated
            #define BSLS_PLATFORM_CPU_X86                                     1
            #define BSLS_PLATFORM_CPU_32_BIT                                  1
        #endif
        #define BSLS_PLATFORM_CPU_VER_MAJOR _M_AMD64
    #elif defined(_M_PPC)
        #define BSLS_PLATFORM_CPU_POWERPC                                     1
        #define BSLS_PLATFORM_CPU_VER_MAJOR _M_PPC
    #elif defined(_M_MRX000)
        #define BSLS_PLATFORM_CPU_MIPS                                        1
        #define BSLS_PLATFORM_CPU_VER_MAJOR _M_MRX000
    #else
        #error "Unable to identify CPU on which the MSVC compiler is running."
        BSLS_PLATFORM_COMPILER_ERROR;
    #endif

// ---------------------------------------------------------------------------
#elif defined(__clang__) || defined(__GNUC__) || defined(__EDG__)

    #if defined(__clang__)
        // Clang presents itself as GCC compatible, but sets the pre-defined
        // GCC version macros ('__GNUC__', '__GNUC_MINOR__', and
        // '__GNUC_PATCHLEVEL__') to version 4.2.1 no matter the version of
        // Clang being used.  In order to differentiate between Clang and GCC,
        // the compilers are identified by independent 'BSLS_PLATFORM' macros.
        // Apple Xcode is based upon LLVM (Clang), but Apple changes the
        // reported Clang versioning ('__clang_major__', '__clang_minor__',
        // '__clang_patchlevel__') to report the Xcode version rather than the
        // actual version of Clang the Xcode release includes.  A table of
        // Xcode/Clang version information is maintained here:
        // https://trac.macports.org/wiki/XcodeVersionInfo To avoid this extra
        // dimension, the Clang intrinsics '__has_builtin', '__has_feature',
        // and '__has_extension' should be used in preference to
        // 'BSLS_PLATFORM_CMP_VERSION' when checking for compiler features.  If
        // 'BSLS_PLATFORM_CMP_VERSION' must be used, then '__APPLE_CC__' can be
        // tested to determine if 'BSLS_PLATFORM_CMP_VERSION' represents Clang
        // LLVM or Apple Xcode version.

        #define BSLS_PLATFORM_CMP_CLANG                                       1
        #define BSLS_PLATFORM_CMP_VERSION ( __clang_major__ * 10000           \
                                          + __clang_minor__ * 100             \
                                          + __clang_patchlevel__ )
    #elif defined (__GNUC__)
        #define BSLS_PLATFORM_CMP_GNU                                         1
        #if defined(__GNUC_PATCHLEVEL__)
            #define BSLS_PLATFORM_CMP_VERSION (__GNUC__ * 10000               \
                        + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
        #else
            #define BSLS_PLATFORM_CMP_VERSION (__GNUC__ * 10000               \
                        + __GNUC_MINOR__ * 100)
        #endif
    #else
        #define BSLS_PLATFORM_CMP_EDG                                         1
        #define BSLS_PLATFORM_CMP_VERSION __EDG_VERSION__
    #endif

    // which OS -- GNU and EDG/Como are implemented almost everywhere
    #if defined(_AIX)
        #define BSLS_PLATFORM_OS_AIX                                          1
    #elif defined(__CYGWIN__) || defined(cygwin) || defined(__cygwin)
        #define BSLS_PLATFORM_OS_CYGWIN                                       1
    #elif defined(linux) || defined(__linux)
        #define BSLS_PLATFORM_OS_LINUX                                        1
    #elif defined(__FreeBSD__)
        #define BSLS_PLATFORM_OS_FREEBSD                                      1
    #elif defined(sun) || defined(__sun)
        #if defined(__SVR4) || defined(__svr4__)
            #define BSLS_PLATFORM_OS_SOLARIS                                  1
        #else
            #define BSLS_PLATFORM_OS_SUNOS                                    1
        #endif
    #elif defined(_WIN32) || defined(__WIN32__) &&                            \
          !(defined(cygwin) || defined(__cygwin))
        #define BSLS_PLATFORM_OS_WINDOWS                                      1
    #elif defined(__APPLE__)
        #define BSLS_PLATFORM_OS_DARWIN                                       1
    #else
        #if defined(__GNUC__)
            #error "Unable to determine on which OS the compiler is running."
        #else
            #error "Unable to determine on which OS EDG compiler is running."
        #endif
        BSLS_PLATFORM_COMPILER_ERROR;
    #endif

    #if !defined(BSLS_PLATFORM_OS_WINDOWS)
        #define BSLS_PLATFORM_OS_UNIX                                         1
    #endif

    // which CPU -- GNU and EDG/Como are implemented almost everywhere
    #if defined(__alpha__)
        #define BSLS_PLATFORM_CPU_ALPHA                                       1
    #elif defined(__x86_64) || defined(__x86_64__)
        #define BSLS_PLATFORM_CPU_X86_64                                      1
        #define BSLS_PLATFORM_CPU_64_BIT                                      1
    #elif defined(__i386) || defined(__i386__)                                \
        || defined(__ix86) || defined(__ix86__)
        #define BSLS_PLATFORM_CPU_X86                                         1
        #define BSLS_PLATFORM_CPU_32_BIT                                      1
    #elif defined(__ia64) || defined(__ia64__) || defined(_IA64)              \
          || defined(__IA64__)
        #define BSLS_PLATFORM_CPU_IA64                                        1
        #if defined(_LP64) || defined(__LP64__)
            #define BSLS_PLATFORM_CPU_64_BIT                                  1
        #else  // defined(_ILP32)
            #define BSLS_PLATFORM_CPU_32_BIT                                  1
        #endif
    #elif defined(__mips__)
        #define BSLS_PLATFORM_CPU_MIPS                                        1
    #elif defined(__hppa__) || defined(__hppa)
        #define BSLS_PLATFORM_CPU_HPPA                                        1
    #elif defined(__powerpc) || defined(__powerpc__)                          \
          || defined(__POWERPC__) || defined(__ppc__) || defined(_POWER)
        #define BSLS_PLATFORM_CPU_POWERPC                                     1
        #if defined(__64BIT__) || defined(_LP64) || defined(__LP64__)
            #define BSLS_PLATFORM_CPU_64_BIT                                  1
        #else  // defined(_ILP32)
            #define BSLS_PLATFORM_CPU_32_BIT                                  1
        #endif
    #elif defined(__sparc__) || defined(__sparc_v9__) || defined(__sparcv9)
        #define BSLS_PLATFORM_CPU_SPARC                                       1
        #if defined(__sparc_v9__) || defined(__sparcv9) || defined(__arch64__)
            #define BSLS_PLATFORM_CPU_SPARC_V9                                1
            #define BSLS_PLATFORM_CPU_64_BIT                                  1
        #else
            #define BSLS_PLATFORM_CPU_SPARC_32                                1
        #endif
    #elif defined(__arm__) || defined(__arm64__)
        #define BSLS_PLATFORM_CPU_ARM                                         1
        #if defined(__arm64__)
            #define BSLS_PLATFORM_CPU_64_BIT                                  1
        #endif
        #if defined(__ARM_ARCH)
            #if __ARM_ARCH == 6
                #define BSLS_PLATFORM_CPU_ARM_V6                              1
            #elif __ARM_ARCH == 7
                #define BSLS_PLATFORM_CPU_ARM_V7                              1
            #elif __ARM_ARCH == 8
                #define BSLS_PLATFORM_CPU_ARM_V8                              1
            #endif
        #elif defined(__ARM_ARCH_5T__)                                        \
            || defined(__ARM_ARCH_5TE__)                                      \
            || defined(__ARM_ARCH_5TEJ__)
            #define BSLS_PLATFORM_CPU_ARM_V5
        #elif defined(__ARM_ARCH_6__) || defined (__ARM_ARCH_6ZK__)
            #define BSLS_PLATFORM_CPU_ARM_V6
        #elif defined(__ARM_ARCH_7__)                                         \
            || defined(__ARM_ARCH_7A__)                                       \
            || defined(__ARM_ARCH_7M__)                                       \
            || defined(__ARM_ARCH_7R__)
            #define BSLS_PLATFORM_CPU_ARM_V7
        #elif defined(__ARM64_ARCH_8__)                                       \
            || defined(__ARM_ARCH_8_3__)                                      \
            || defined(__ARM_ARCH_8_4__)                                      \
            || defined(__ARM_ARCH_8_5__)
            #define BSLS_PLATFORM_CPU_ARM_V8
        #else
            #error "Unsupported ARM platform."
        #endif
    #else
        #if defined(__GNUC__)
            #error "Unable to determine on which CPU the compiler is running."
        #else
            #error "Unable to determine on which CPU EDG compiler is running."
        #endif
        BSLS_PLATFORM_COMPILER_ERROR;
    #endif

    #if !defined(BSLS_PLATFORM_CPU_64_BIT)
        #define BSLS_PLATFORM_CPU_32_BIT                                      1
    #endif
// ----------------------------------------------------------------------------
#elif defined(__SUNPRO_CC) || defined(__SUNPRO_C)

    #define BSLS_PLATFORM_CMP_SUN                                             1
    #if defined(__cplusplus)
        #define BSLS_PLATFORM_CMP_VERSION __SUNPRO_CC
    #else
        #define BSLS_PLATFORM_CMP_VERSION __SUNPRO_C
    #endif

    // which OS
    #define BSLS_PLATFORM_OS_UNIX                                             1
    #if defined(sun) || defined(__sun)
        #define BSLS_PLATFORM_OS_SOLARIS                                      1
        #if defined(__SVR4) || defined(__svr4__)
            #if defined(__SunOS_5_7)
                #define BSLS_PLATFORM_OS_VER_MAJOR 7
                #define BSLS_PLATFORM_OS_VER_MINOR 0
            #elif defined(__SunOS_5_8)
                #define BSLS_PLATFORM_OS_VER_MAJOR 8
                #define BSLS_PLATFORM_OS_VER_MINOR 0
            #elif defined(__SunOS_5_9)
                #define BSLS_PLATFORM_OS_VER_MAJOR 9
                #define BSLS_PLATFORM_OS_VER_MINOR 0
            #elif defined(__SunOS_5_10)
                #define BSLS_PLATFORM_OS_VER_MAJOR 10
                #define BSLS_PLATFORM_OS_VER_MINOR 0
            #elif defined(__SunOS_5_11)
                #define BSLS_PLATFORM_OS_VER_MAJOR 11
                #define BSLS_PLATFORM_OS_VER_MINOR 0
            #else
                #define BSLS_PLATFORM_OS_VER_MAJOR 1
                #define BSLS_PLATFORM_OS_VER_MINOR 0
            #endif
        #else
            #error "Unable to determine SUN OS version."
            BSLS_PLATFORM_COMPILER_ERROR;
        #endif
    #elif defined(__SVR4) || defined(__svr4__)
        #define BSLS_PLATFORM_OS_SUNOS                                        1
        #if defined(__SunOS_5_7)
            #define BSLS_PLATFORM_OS_VER_MAJOR 7
            #define BSLS_PLATFORM_OS_VER_MINOR 0
        #elif defined(__SunOS_5_8)
            #define BSLS_PLATFORM_OS_VER_MAJOR 8
            #define BSLS_PLATFORM_OS_VER_MINOR 0
        #elif defined(__SunOS_5_9)
            #define BSLS_PLATFORM_OS_VER_MAJOR 9
            #define BSLS_PLATFORM_OS_VER_MINOR 0
        #else
            #define BSLS_PLATFORM_OS_VER_MAJOR 1
        #endif
    #else
        #error "Unable to determine SUN OS version."
        BSLS_PLATFORM_COMPILER_ERROR;
    #endif

    // determine which CPU
    #if defined(__x86_64) || defined(__x86_64__)
        #define BSLS_PLATFORM_CPU_X86_64                                      1
        #define BSLS_PLATFORM_CPU_64_BIT                                      1
    #elif defined(i386) || defined(__i386)
        #define BSLS_PLATFORM_CPU_X86                                         1
        #define BSLS_PLATFORM_CPU_32_BIT                                      1
    #elif defined(__sparc64) || defined(__sparcv9)
        #define BSLS_PLATFORM_CPU_SPARC                                       1
        #define BSLS_PLATFORM_CPU_SPARC_V9                                    1
        #define BSLS_PLATFORM_CPU_64_BIT                                      1
    #elif defined(sparc) || defined(__sparc)
        #define BSLS_PLATFORM_CPU_SPARC                                       1
        #define BSLS_PLATFORM_CPU_SPARC_32                                    1
        #define BSLS_PLATFORM_CPU_32_BIT                                      1
    #else
        #error "Cannot determine CPU on which the SUN compiler is running."
        BSLS_PLATFORM_COMPILER_ERROR;
    #endif
// ---------------------------------------------------------------------------
#else
    #error "Could not identify the compiler."
    BSLS_PLATFORM_COMPILER_ERROR;
#endif
// ----------------------------------------------------------------------------

// Determine endianness.

// GNU libc or Linux or Cygwin
#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_CYGWIN)       \
    || defined(__GLIBC__)

    #include <endian.h>
    #if (__BYTE_ORDER == __LITTLE_ENDIAN)
        #define BSLS_PLATFORM_IS_LITTLE_ENDIAN                                1
    #elif (__BYTE_ORDER == __BIG_ENDIAN)
        #define BSLS_PLATFORM_IS_BIG_ENDIAN                                   1
    #endif

// AIX
#elif defined(BSLS_PLATFORM_OS_AIX)
    #include <sys/machine.h>
    #if BYTE_ORDER == LITTLE_ENDIAN
        #define BSLS_PLATFORM_IS_LITTLE_ENDIAN                                1
    #elif BYTE_ORDER == BIG_ENDIAN
        #define BSLS_PLATFORM_IS_BIG_ENDIAN                                   1
    #endif

// Sun/Solaris
#elif defined(BSLS_PLATFORM_OS_SUNOS) || defined(BSLS_PLATFORM_OS_SOLARIS)
    #include <sys/isa_defs.h>
    #if defined(_LITTLE_ENDIAN)
        #define BSLS_PLATFORM_IS_LITTLE_ENDIAN                                1
    #elif defined(_BIG_ENDIAN)
        #define BSLS_PLATFORM_IS_BIG_ENDIAN                                   1
    #endif

// Darwin
#elif defined(BSLS_PLATFORM_OS_DARWIN)
    #include <machine/endian.h>
    #if BYTE_ORDER == LITTLE_ENDIAN
        #define BSLS_PLATFORM_IS_LITTLE_ENDIAN                                1
    #elif BYTE_ORDER == BIG_ENDIAN
        #define BSLS_PLATFORM_IS_BIG_ENDIAN                                   1
    #endif

// MSVC and Windows
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    #define BSLS_PLATFORM_IS_LITTLE_ENDIAN                                    1
#endif

// Endianness sanity check is done further in this header.

// ----------------------------------------------------------------------------

                         // Detect Supported Platform

#if !defined(BDE_DISABLE_COMPILER_VERSION_CHECK)

#if defined(BSLS_PLATFORM_CMP_CLANG)
    // No minimum supported compiler version has been identified yet.
#elif defined(BSLS_PLATFORM_CMP_EDG)
    // No minimum supported compiler version has been identified yet.
#elif defined(BSLS_PLATFORM_CMP_HP)
    #if BSLS_PLATFORM_CMP_VERSION < 62500
        #error This early compiler is not supported by BDE
    #endif
#elif defined(BSLS_PLATFORM_CMP_IBM)
    // Require at least xlC 12 for AIX.
    #if BSLS_PLATFORM_CMP_VERSION < 0x0c01
        #error This early compiler is not supported by BDE
    #endif
#elif defined(BSLS_PLATFORM_CMP_SUN)
    #if BSLS_PLATFORM_CMP_VERSION < 0x5120
        #error This early compiler is not supported by BDE
    #endif
#elif defined(BSLS_PLATFORM_CMP_GNU)
    // Test GNU late, as so many compilers offer a GNU compatibility mode.
    #if BSLS_PLATFORM_CMP_VERSION < 40102
        #error This early compiler is not supported by BDE
    #endif
#elif defined(BSLS_PLATFORM_CMP_MSVC)
    // Test MSVC last, as many compilers targeting Windows offer a Microsoft
    // compatibility mode.
    #if BSLS_PLATFORM_CMP_VERSION < 1800
        #error This early compiler is not supported by BDE
    #endif
#else
    #error This compiler is not recognized by BDE
#endif

#endif

#ifdef BSLS_PLATFORM_CMP_VERSION
#define BSLS_PLATFORM_CMP_VER_MAJOR  BSLS_PLATFORM_CMP_VERSION
    // This deprecated macro is defined for backwards compatibility only.
#endif
// ----------------------------------------------------------------------------

                        // Miscellaneous Platform Macros

#if defined(BSLS_PLATFORM_CMP_GNU) && !defined(__GXX_EXPERIMENTAL_CXX0X__)
    #define BSLS_PLATFORM_NO_64_BIT_LITERALS                                  1
#endif

#if defined(BSLS_PLATFORM_CMP_IBM) && !defined(BSLS_PLATFORM_CPU_64_BIT)
    #define BSLS_PLATFORM_NO_64_BIT_LITERALS                                  1
#endif

#if (defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VER_MAJOR >= 40600)  \
                                    || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC                           1
    #if defined(BSLS_PLATFORM_CMP_CLANG)
        #define BSLS_PLATFORM_PRAGMA_GCC_DIAGNOSTIC_CLANG                     1
    #else
        #define BSLS_PLATFORM_PRAGMA_GCC_DIAGNOSTIC_GCC                       1
    #endif
#endif

#if !(defined(BSLS_PLATFORM_CMP_AIX) || defined(BSLS_PLATFORM_CMP_SUN))       \
                                || defined(BDE_BUILD_TARGET_AGGRESSIVE_INLINE)
    #define BSLS_PLATFORM_AGGRESSIVE_INLINE inline
#else
    #define BSLS_PLATFORM_AGGRESSIVE_INLINE
#endif

#if defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 40406 ||   \
    defined(BSLS_PLATFORM_CMP_CLANG) || defined(BSLS_PLATFORM_CMP_MSVC)
    // Support for preserving macro values through the following pragmas:
    //     #pragma push_macro("NAME")   // Save macro definition of NAME
    //     #pragma pop_macro("NAME")    // Restore macro definition of NAME
    // Note that if NAME is undefined, the sequence push/define NAME/pop may
    // leave NAME defined at the end, depending on compiler version.
    #define BSLS_PLATFORM_HAS_MACRO_PUSH_POP                                  1
#endif

///Implementation Note
///- - - - - - - - - -
// When compiling with Microsoft Visual Studio, as indicated by the macro
// '_MSC_VER' being defined, this component assumes that the SSE, SSE2, and
// SSE3 instruction set extensions are available on x86 platforms.  Therefore,
// x86 processors lacking these extensions are not supported when compiling
// with Visual Studio.  Later extensions, such as SSE4.1, SSE4.2, and AVX, are
// not assumed.  As of the writing of this note, Visual Studio does not provide
// a reliable way to detect whether x86 instruction set extensions are enabled.
//
// Note that it is not necessarily safe to change this component to assume
// later instruction set extensions are available when compiling with Visual
// Studio.  Doing so may lead to dependent components using instructions that
// are not available on some x86 platforms.
//
// Clang and GCC (as well as compilers with EDG front-ends) provide macros to
// detect whether most x86 instruction set extensions are enabled.

#if defined(_MSC_VER)
    #define BSLS_PLATFORM_CPU_SSE                                             1
    #define BSLS_PLATFORM_CPU_SSE2                                            1
    #define BSLS_PLATFORM_CPU_SSE3                                            1
#elif defined(__clang__) || defined(__GNUC__) || defined(__EDG__)
    #if defined(__SSE__)
        #define BSLS_PLATFORM_CPU_SSE                                         1
    #endif
    #if defined(__SSE2__)
        #define BSLS_PLATFORM_CPU_SSE2                                        1
    #endif
    #if defined(__SSE3__)
        #define BSLS_PLATFORM_CPU_SSE3                                        1
    #endif
    #if defined(__SSE4_1__)
        #define BSLS_PLATFORM_CPU_SSE4_1                                      1
    #endif
    #if defined(__SSE4_2__)
        #define BSLS_PLATFORM_CPU_SSE4_2                                      1
    #endif
#endif

// ----------------------------------------------------------------------------

                           // Self Validation

// Unix flag must be set by the compiler if Unix detected (except for AIX).
#if defined(BSLS_PLATFORM_OS_UNIX) && !defined(BSLS_PLATFORM_OS_AIX) &&       \
                                      !defined(BSLS_PLATFORM_OS_DARWIN)
    #if !defined(unix) && !defined(__unix__) && !defined(__unix)
        #error "Unix platform assumed, but unix flag not set by compiler"
        BSLS_PLATFORM_COMPILER_ERROR;
    #endif
#endif

// Exactly one 'CMP' type.
#if BSLS_PLATFORM_CMP_EDG                                                     \
  + BSLS_PLATFORM_CMP_CLANG                                                   \
  + BSLS_PLATFORM_CMP_GNU                                                     \
  + BSLS_PLATFORM_CMP_HP                                                      \
  + BSLS_PLATFORM_CMP_IBM                                                     \
  + BSLS_PLATFORM_CMP_MSVC                                                    \
  + BSLS_PLATFORM_CMP_SUN != 1
    #error "Exactly one compiler must be set."
    BSLS_PLATFORM_COMPILER_ERROR;
#endif

// Exactly one 'OS' type.
#if BSLS_PLATFORM_OS_UNIX                                                     \
  + BSLS_PLATFORM_OS_WINDOWS != 1
    #error "Exactly one operating system must be set."
    BSLS_PLATFORM_COMPILER_ERROR;
#endif

// At most one 'OS' subtype.
#define BSLS_PLATFORM_OS_SUBTYPE_COUNT                                        \
    BSLS_PLATFORM_OS_AIX                                                      \
  + BSLS_PLATFORM_OS_DARWIN                                                   \
  + BSLS_PLATFORM_OS_LINUX                                                    \
  + BSLS_PLATFORM_OS_FREEBSD                                                  \
  + BSLS_PLATFORM_OS_SOLARIS                                                  \
  + BSLS_PLATFORM_OS_SUNOS                                                    \
  + BSLS_PLATFORM_OS_CYGWIN                                                   \
  + BSLS_PLATFORM_OS_WIN9X                                                    \
  + BSLS_PLATFORM_OS_WINNT                                                    \
  + BSLS_PLATFORM_OS_WIN2K                                                    \
  + BSLS_PLATFORM_OS_WINXP                                                    \
  + BSLS_PLATFORM_OS_WINS03                                                   \
  + BSLS_PLATFORM_OS_WINVISTA                                                 \
  + BSLS_PLATFORM_OS_WIN7                                                     \
  + BSLS_PLATFORM_OS_WIN8                                                     \
  + BSLS_PLATFORM_OS_WINBLUE                                                  \
  + BSLS_PLATFORM_OS_WIN10
#if BSLS_PLATFORM_OS_SUBTYPE_COUNT > 1
    #error "At most one operating system subtype must be set."
    BSLS_PLATFORM_COMPILER_ERROR;
#endif

// Windows Server 2008 API is the same as Vista
#if defined(BSLS_PLATFORM_OS_WINVISTA) != defined(BSLS_PLATFORM_OS_WINS08)
    #error "Windows Vista and Server 2008 are the same API."
    BSLS_PLATFORM_COMPILER_ERROR;
#endif

// Exactly one 'CPU' type.
#if BSLS_PLATFORM_CPU_88000                                                   \
  + BSLS_PLATFORM_CPU_ALPHA                                                   \
  + BSLS_PLATFORM_CPU_HPPA                                                    \
  + BSLS_PLATFORM_CPU_IA64                                                    \
  + BSLS_PLATFORM_CPU_X86                                                     \
  + BSLS_PLATFORM_CPU_X86_64                                                  \
  + BSLS_PLATFORM_CPU_MIPS                                                    \
  + BSLS_PLATFORM_CPU_POWERPC                                                 \
  + BSLS_PLATFORM_CPU_SPARC                                                   \
  + BSLS_PLATFORM_CPU_ARM != 1
    #error "Exactly one processor must be set."
    BSLS_PLATFORM_COMPILER_ERROR;
#endif

// Exactly one kind of "endian-ness".
#if BSLS_PLATFORM_IS_BIG_ENDIAN                                               \
  + BSLS_PLATFORM_IS_LITTLE_ENDIAN != 1
    #error "Exactly one kind of endian-ness must be set."
    BSLS_PLATFORM_COMPILER_ERROR;
#endif

#if defined(BSLS_PLATFORM_OS_VER_MAJOR) && BSLS_PLATFORM_OS_SUBTYPE_COUNT != 1
        // For OS, MAJOR VERSION implies SUBTYPE.
    #error "Operating system major version but not subtype defined."
    BSLS_PLATFORM_COMPILER_ERROR;
#endif

#undef BSLS_PLATFORM_OS_SUBTYPE_COUNT

#if defined(BSLS_PLATFORM_OS_VER_MINOR) && !defined(BSLS_PLATFORM_OS_VER_MAJOR)
    #error "Operating System minor but not major version defined."
    BSLS_PLATFORM_COMPILER_ERROR;
#endif

#ifdef __cplusplus
namespace BloombergLP {
namespace bsls {

// ----------------------------------------------------------------------------

                                // ========
                                // Platform
                                // ========

struct Platform {
    // Namespace for platform-trait definitions.

                                  // OS TYPES

    struct OsAny {};

    struct OsUnix    : OsAny {};
    struct OsWindows : OsAny {};

    struct OsAix     : OsUnix {};
    struct OsHpux    : OsUnix {};
    struct OsLinux   : OsUnix {};
    struct OsFreeBsd : OsUnix {};
    struct OsSolaris : OsUnix {};
    struct OsCygwin  : OsUnix {};
    struct OsDarwin  : OsUnix {};

    struct OsSolaris_V8  : OsSolaris {};
    struct OsSolaris_V9  : OsSolaris {};
    struct OsSolaris_V10 : OsSolaris {};

    struct OsAix_V5 : OsAix {};

    struct OsWinNT : OsWindows {};

                              // PROCESSOR TYPES

    struct CpuAny {};

    struct Cpu88000   : CpuAny {};
    struct CpuAlpha   : CpuAny {};
    struct CpuHppa    : CpuAny {};
    struct CpuX86     : CpuAny {};
    struct CpuIa64    : CpuAny {};
    struct CpuX86_64  : CpuAny {};
    struct CpuMips    : CpuAny {};

    struct CpuPowerpc : CpuAny {};

    struct CpuSparc   : CpuAny {};
    struct CpuSparc_32 : CpuSparc {}; // 32-bit
    struct CpuSparc_V9 : CpuSparc {}; // 64-bit

    typedef CpuPowerpc     CPU_POWERPC;
    typedef CpuSparc       CPU_SPARC;
    typedef CpuSparc_32    CPU_SPARC_32;
    typedef CpuSparc_V9    CPU_SPARC_V9;

    struct CpuPowerpc_601 : CpuPowerpc {};
    struct CpuArch_Pwr    : CpuPowerpc {};
    struct CpuArch_Pwr2   : CpuPowerpc {};
    struct CpuArch_Pwr2s  : CpuPowerpc {};

    struct CpuArm   : CpuAny {};
    struct CpuArmv5 : CpuArm {};
    struct CpuArmv6 : CpuArm {};
    struct CpuArmv7 : CpuArm {};
    struct CpuArmv8 : CpuArm {};

                              // PLATFORM TRAITS

    //OS TRAIT
    // Will fail to compile if more than one OS type is set.

    #if defined(BSLS_PLATFORM_OS_AIX)
        #if (BSLS_PLATFORM_OS_VER_MAJOR == _AIX)
            typedef OsAix_V5         Os;
        #else
            typedef OsAix            Os;
        #endif
    #endif
    #if defined BSLS_PLATFORM_OS_SOLARIS
        #if   (BSLS_PLATFORM_OS_VER_MAJOR == 9)
            typedef OsSolaris_V9     Os;
        #elif (BSLS_PLATFORM_OS_VER_MAJOR == 10)
            typedef OsSolaris_V10    Os;
        #else
            typedef OsSolaris_V8     Os;
        #endif
    #endif
    #if defined(BSLS_PLATFORM_OS_CYGWIN)
        typedef OsCygwin            Os;
    #endif
    #if defined(BSLS_PLATFORM_OS_LINUX)
        typedef OsLinux             Os;
    #endif
    #if defined(BSLS_PLATFORM_OS_FREEBSD)
        typedef OsFreeBsd           Os;
    #endif
    #if defined(BSLS_PLATFORM_OS_WINDOWS)
        typedef OsWinNT             Os;
    #endif
    #if defined(BSLS_PLATFORM_OS_DARWIN)
        typedef OsDarwin            Os;
    #endif

    //CPU TRAIT
    // Will fail to compile if more than one CPU type is set.

    #if defined(BSLS_PLATFORM_CPU_X86)
        typedef CpuX86  Cpu;
    #endif
    #if defined(BSLS_PLATFORM_CPU_IA64)
        typedef CpuIa64 Cpu;
    #endif
    #if defined(BSLS_PLATFORM_CPU_X86_64)
        typedef CpuX86_64  Cpu;
    #endif
    #if defined(BSLS_PLATFORM_CPU_POWERPC)
        #if defined(BSLS_PLATFORM_CPU_VER_MAJOR) &&                           \
                                    (BSLS_PLATFORM_CPU_VER_MAJOR == _ARCH_601)
            typedef CpuPowerpc_601 Cpu;
        #else
            typedef CpuPowerpc     Cpu;
        #endif
    #endif
    #if defined(BSLS_PLATFORM_CPU_SPARC_32)
        typedef CpuSparc_32   Cpu;
    #endif
    #if defined(BSLS_PLATFORM_CPU_SPARC_V9)
        typedef CpuSparc_V9   Cpu;
    #endif
    #if defined(BSLS_PLATFORM_CPU_ARM_V5)
        typedef CpuArmv5 Cpu;
    #endif
    #if defined(BSLS_PLATFORM_CPU_ARM_V6)
        typedef CpuArmv6 Cpu;
    #endif
    #if defined(BSLS_PLATFORM_CPU_ARM_V7)
        typedef CpuArmv7 Cpu;
    #endif
    #if defined(BSLS_PLATFORM_CPU_ARM_V8)
        typedef CpuArmv8 Cpu;
    #endif
};

}  // close package namespace
#endif  // __cplusplus

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
#ifndef BDE_OMIT_INTERNAL_DEPRECATED

                         // ======================
                         // BACKWARD COMPATIBILITY
                         // ======================

#ifdef __cplusplus
typedef bsls::Platform bdes_Platform;
    // This alias is defined for backward compatibility.
#endif  // __cplusplus

#endif // BDE_OMIT_INTERNAL_DEPRECATED

#ifdef __cplusplus
typedef bsls::Platform bsls_Platform;
    // This alias is defined for backward compatibility.
#endif

#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

#ifdef __cplusplus
}  // close enterprise namespace
#endif

#if !defined(BSL_DOUBLE_UNDERSCORE_XLAT) || 1 == BSL_DOUBLE_UNDERSCORE_XLAT

// 'BDES' aliases

#ifdef BSLS_PLATFORM_CMP_AIX
# define BDES_PLATFORM__CMP_AIX BSLS_PLATFORM_CMP_AIX
#endif
#ifdef BSLS_PLATFORM_CMP_GNU
# define BDES_PLATFORM__CMP_GNU BSLS_PLATFORM_CMP_GNU
#endif
#ifdef BSLS_PLATFORM_CMP_HP
# define BDES_PLATFORM__CMP_HP BSLS_PLATFORM_CMP_HP
#endif
#ifdef BSLS_PLATFORM_CMP_MSVC
# define BDES_PLATFORM__CMP_MSVC BSLS_PLATFORM_CMP_MSVC
#endif
#ifdef BSLS_PLATFORM_CMP_SUN
# define BDES_PLATFORM__CMP_SUN BSLS_PLATFORM_CMP_SUN
#endif
#ifdef BSLS_PLATFORM_CMP_VER_MAJOR
# define BDES_PLATFORM__CMP_VER_MAJOR BSLS_PLATFORM_CMP_VER_MAJOR
#endif
#ifdef BSLS_PLATFORM_CPU_64_BIT
# define BDES_PLATFORM__CPU_64_BIT BSLS_PLATFORM_CPU_64_BIT
#endif
#ifdef BSLS_PLATFORM_OS_AIX
# define BDES_PLATFORM__OS_AIX BSLS_PLATFORM_OS_AIX
#endif
#ifdef BSLS_PLATFORM_OS_CYGWIN
# define BDES_PLATFORM__OS_CYGWIN BSLS_PLATFORM_OS_CYGWIN
#endif
#ifdef BSLS_PLATFORM_OS_DARWIN
# define BDES_PLATFORM__OS_DARWIN BSLS_PLATFORM_OS_DARWIN
#endif
#ifdef BSLS_PLATFORM_OS_FREEBSD
# define BDES_PLATFORM__OS_FREEBSD BSLS_PLATFORM_OS_FREEBSD
#endif
#ifdef BSLS_PLATFORM_OS_LINUX
# define BDES_PLATFORM__OS_LINUX BSLS_PLATFORM_OS_LINUX
#endif
#ifdef BSLS_PLATFORM_OS_SOLARIS
# define BDES_PLATFORM__OS_SOLARIS BSLS_PLATFORM_OS_SOLARIS
#endif
#ifdef BSLS_PLATFORM_OS_SUNOS
# define BDES_PLATFORM__OS_SUNOS BSLS_PLATFORM_OS_SUNOS
#endif
#ifdef BSLS_PLATFORM_OS_UNIX
# define BDES_PLATFORM__OS_UNIX BSLS_PLATFORM_OS_UNIX
#endif
#ifdef BSLS_PLATFORM_OS_VER_MAJOR
# define BDES_PLATFORM__OS_VER_MAJOR BSLS_PLATFORM_OS_VER_MAJOR
#endif
#ifdef BSLS_PLATFORM_OS_VER_MINOR
# define BDES_PLATFORM__OS_VER_MINOR BSLS_PLATFORM_OS_VER_MINOR
#endif
#ifdef BSLS_PLATFORM_OS_WIN2K
# define BDES_PLATFORM__OS_WIN2K BSLS_PLATFORM_OS_WIN2K
#endif
#ifdef BSLS_PLATFORM_OS_WIN9X
# define BDES_PLATFORM__OS_WIN9X BSLS_PLATFORM_OS_WIN9X
#endif
#ifdef BSLS_PLATFORM_OS_WINDOWS
# define BDES_PLATFORM__OS_WINDOWS BSLS_PLATFORM_OS_WINDOWS
#endif
#ifdef BSLS_PLATFORM_OS_WINNT
# define BDES_PLATFORM__OS_WINNT BSLS_PLATFORM_OS_WINNT
#endif
#ifdef BSLS_PLATFORM_OS_WINXP
# define BDES_PLATFORM__OS_WINXP BSLS_PLATFORM_OS_WINXP
#endif

// 'BSLS' aliases

#ifdef BSLS_PLATFORM_CMP_AIX
# define BSLS_PLATFORM__CMP_AIX BSLS_PLATFORM_CMP_AIX
#endif
#ifdef BSLS_PLATFORM_CMP_CLANG
# define BSLS_PLATFORM__CMP_CLANG BSLS_PLATFORM_CMP_CLANG
#endif
#ifdef BSLS_PLATFORM_CMP_EDG
# define BSLS_PLATFORM__CMP_EDG BSLS_PLATFORM_CMP_EDG
#endif
#ifdef BSLS_PLATFORM_CMP_GNU
# define BSLS_PLATFORM__CMP_GNU BSLS_PLATFORM_CMP_GNU
#endif
#ifdef BSLS_PLATFORM_CMP_HP
# define BSLS_PLATFORM__CMP_HP BSLS_PLATFORM_CMP_HP
#endif
#ifdef BSLS_PLATFORM_CMP_IBM
# define BSLS_PLATFORM__CMP_IBM BSLS_PLATFORM_CMP_IBM
#endif
#ifdef BSLS_PLATFORM_CMP_MSVC
# define BSLS_PLATFORM__CMP_MSVC BSLS_PLATFORM_CMP_MSVC
#endif
#ifdef BSLS_PLATFORM_CMP_SUN
# define BSLS_PLATFORM__CMP_SUN BSLS_PLATFORM_CMP_SUN
#endif
#ifdef BSLS_PLATFORM_CMP_VERSION
# define BSLS_PLATFORM__CMP_VERSION BSLS_PLATFORM_CMP_VERSION
#endif
#ifdef BSLS_PLATFORM_CMP_VER_MAJOR
# define BSLS_PLATFORM__CMP_VER_MAJOR BSLS_PLATFORM_CMP_VER_MAJOR
#endif
#ifdef BSLS_PLATFORM_CPU_32_BIT
# define BSLS_PLATFORM__CPU_32_BIT BSLS_PLATFORM_CPU_32_BIT
#endif
#ifdef BSLS_PLATFORM_CPU_64_BIT
# define BSLS_PLATFORM__CPU_64_BIT BSLS_PLATFORM_CPU_64_BIT
#endif
#ifdef BSLS_PLATFORM_CPU_POWERPC
# define BSLS_PLATFORM__CPU_POWERPC BSLS_PLATFORM_CPU_POWERPC
#endif
#ifdef BSLS_PLATFORM_CPU_X86
# define BSLS_PLATFORM__CPU_X86 BSLS_PLATFORM_CPU_X86
#endif
#ifdef BSLS_PLATFORM_CPU_X86_64
# define BSLS_PLATFORM__CPU_X86_64 BSLS_PLATFORM_CPU_X86_64
#endif
#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
# define BSLS_PLATFORM__IS_BIG_ENDIAN BSLS_PLATFORM_IS_BIG_ENDIAN
#endif
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
# define BSLS_PLATFORM__IS_LITTLE_ENDIAN BSLS_PLATFORM_IS_LITTLE_ENDIAN
#endif
#ifdef BSLS_PLATFORM_OS_AIX
# define BSLS_PLATFORM__OS_AIX BSLS_PLATFORM_OS_AIX
#endif
#ifdef BSLS_PLATFORM_OS_CYGWIN
# define BSLS_PLATFORM__OS_CYGWIN BSLS_PLATFORM_OS_CYGWIN
#endif
#ifdef BSLS_PLATFORM_OS_DARWIN
# define BSLS_PLATFORM__OS_DARWIN BSLS_PLATFORM_OS_DARWIN
#endif
#ifdef BSLS_PLATFORM_OS_LINUX
# define BSLS_PLATFORM__OS_LINUX BSLS_PLATFORM_OS_LINUX
#endif
#ifdef BSLS_PLATFORM_OS_SOLARIS
# define BSLS_PLATFORM__OS_SOLARIS BSLS_PLATFORM_OS_SOLARIS
#endif
#ifdef BSLS_PLATFORM_OS_SUNOS
# define BSLS_PLATFORM__OS_SUNOS BSLS_PLATFORM_OS_SUNOS
#endif
#ifdef BSLS_PLATFORM_OS_UNIX
# define BSLS_PLATFORM__OS_UNIX BSLS_PLATFORM_OS_UNIX
#endif
#ifdef BSLS_PLATFORM_OS_WINDOWS
# define BSLS_PLATFORM__OS_WINDOWS BSLS_PLATFORM_OS_WINDOWS
#endif

#endif

#endif

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
