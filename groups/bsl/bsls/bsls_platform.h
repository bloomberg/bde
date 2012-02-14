// bsls_platform.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLS_PLATFORM
#define INCLUDED_BSLS_PLATFORM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide compile-time support for platform/attribute identification.
//
//@CLASSES:
//   bsls_Platform: namespace for platform traits
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component implements a suite of preprocessor macros
// and traits that identify and define platform-specific compile-time
// attributes.  These attributes consist of the types and versions of (1) the
// operating system, (2) the processor(s), and (3) the compiler that together
// make up the platform.  Many of macros defined in this component are
// configured automatically at compile-time; compile-time switches are used
// to configure the rest.
//
// Note that, for brevity, the '@' character in the following (alphabetically
// organized) tables is used to represent the characters 'BSLS_PLATFORM' --
// e.g., '@__OS_UNIX' represent 'BSLS_PLATFORM__OS_UNIX':
//..
//  =============================================================
//                           OPERATING SYSTEM
//  -------------------------------------------------------------
//       Type                Subtype               Version
//  -----------------   -------------------   -------------------
//   @__OS_UNIX          @__OS_AIX             @__OS_VER_MAJOR
//                       @__OS_HPUX            @__OS_VER_MINOR
//                       @__OS_LINUX
//                       @__OS_FREEBSD
//                       @__OS_SOLARIS
//                       @__OS_SUNOS
//                       @__OS_CYGWIN
//
//   @__OS_WINDOWS       @__OS_WIN9X
//                       @__OS_WINNT
//                       @__OS_WIN2K
//                       @__OS_WINXP
//
//  ============================================================
//                              PROCESSOR
//  ------------------------------------------------------------
//  Instruction Set          Width                 Version
//  ---------------    -------------------   -------------------
//   @__CPU_88000       @__CPU_32_BIT         @__CPU_VER_MAJOR
//   @__CPU_ALPHA       @__CPU_64_BIT         @__CPU_VER_MINOR
//   @__CPU_HPPA
//   @__CPU_X86
//   @__CPU_IA64
//   @__CPU_X86_64
//   @__CPU_MIPS
//   @__CPU_POWERPC
//   @__CPU_SPARC
//
//  =============================================================
//                              COMPILER
//  -------------------------------------------------------------
//     Vendor                Version
//  -----------------   -------------------
//   @__CMP_EDG          @__CMP_VER_MAJOR
//   @__CMP_GNU          @__CMP_VER_MINOR
//   @__CMP_HP
//   @__CMP_IBM
//   @__CMP_MSVC
//   @__CMP_SUN
//
//  =============================================================
//..
// These macros are configured automatically, where possible.  At a minimum,
// the generic operating system type (i.e., either 'BSLS_PLATFORM__OS_UNIX' or
// 'BSLS_PLATFORM__OS_WINDOWS') is defined along with exactly one processor
// macro (e.g., 'BSLS_PLATFORM__CPU_SPARC') and exactly one compiler macro
// (e.g., 'BSLS_PLATFORM__CMP_SUN').  Clients may need to supply additional
// macros (controlled via the '-D' option of a compiler) if further
// discrimination is required (e.g., based on sub-type or version of a specific
// operating system, processor, or compiler).  Note that supplying a minor
// version number implies that the major version is also defined.
//
///Usage
///-----
// Writing portable software sometimes involves specializing implementations
// to work with platform-specific interfaces.  For example, a socket-level
// communications framework would need to operate differently on a platform
// having a Windows operating system than one having a Unix one (but it is
// probably unnecessary to distinguish between their respective versions):
//..
//  // my_socket.h
//  #include <bsls_platform.h>
//
//  #ifdef BSLS_PLATFORM__OS_WINDOWS
//      #ifndef INCLUDED_WINSOCK2
//      #include <winsock2.h>
//      #define INCLUDED_WINSOCK2
//      #endif
//  #endif
//
//  class my_Socket {
//
//  #ifdef BSLS_PLATFORM__OS_WINDOWS
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
// little-endian across all supported platforms.
//..
//  BSLS_PLATFORM__IS_BIG_ENDIAN
//  BSLS_PLATFORM__IS_LITTLE_ENDIAN
//..
// These macros are useful for writing platform-independent code, such as a
// function that converts the bytes in a 'short' to network byte order (which
// is consistent with big-endian):
//..
//  short convertToNetworkByteOrder(short input)
//      // Return the specified 'input' in network byte order.
//  {
//  #ifdef BSLS_PLATFORM__IS_BIG_ENDIAN
//      return input;
//  #else
//      return ((input >> 8) & 0xFF) | ((input & 0xFF) << 8);
//  #endif
//  }
//..

#ifndef __cplusplus
#error This header should not be included in non-C++ compilation units.
#endif

namespace BloombergLP {

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

// ---------------------------------------------------------------------------
#if defined(__xlC__) || defined(__IBMC__) || defined(__IBMCPP__)
    #define BSLS_PLATFORM__CMP_IBM 1
    #define BSLS_PLATFORM__CMP_VER_MAJOR __xlC__

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
    #define BSLS_PLATFORM__CMP_AIX 1
         // DEPRECATED: use 'BSLS_PLATFORM__CMP_IBM' instead.
#endif

    // which OS -- this compiler should only be used on AIX
    #define BSLS_PLATFORM__OS_UNIX 1
    #if defined(_AIX)                          // must be defined
        #define BSLS_PLATFORM__OS_AIX 1
        #define BSLS_PLATFORM__OS_VER_MAJOR _AIX
    #else
        #error "AIX compiler appears to be in use on non-AIX OS."
        char die[sizeof(bsls_Platform_Assert)];      // if '#error' unsupported
    #endif

    // which CPU -- should always be POWERPC
    #if defined(_ARCH_PWR2)
        #define BSLS_PLATFORM__CPU_VER_MAJOR _ARCH_PWR2
    #elif defined(_ARCH_PWR)
        #define BSLS_PLATFORM__CPU_VER_MAJOR _ARCH_PWR
    #elif defined(_ARCH_POWER)
        #define BSLS_PLATFORM__CPU_VER_MAJOR _ARCH_POWER
    #elif defined(_POWER)
        #define BSLS_PLATFORM__CPU_VER_MAJOR _POWER
    #elif defined(_ARCH_COM)
        #define BSLS_PLATFORM__CPU_VER_MAJOR _ARCH_COM
    #elif defined(_ARCH_601)
        #define BSLS_PLATFORM__CPU_VER_MAJOR _ARCH_601
    #else
        #error "Unable to identify the AIX CPU."
        char die[sizeof(bsls_Platform_Assert)];      // if '#error' unsupported
    #endif

    #define BSLS_PLATFORM__CPU_POWERPC 1
    #if defined (__64BIT__)
        #define BSLS_PLATFORM__CPU_64_BIT 1
    #else
        #define BSLS_PLATFORM__CPU_32_BIT 1
    #endif
// ---------------------------------------------------------------------------
#elif defined(__HP_aCC)
    #define BSLS_PLATFORM__CMP_HP 1
    #define BSLS_PLATFORM__CMP_VER_MAJOR __HP_aCC

    // which OS -- should always be HPUX
    #if defined(hpux) || defined(__hpux) || defined(_HPUX_SOURCE)
        #define BSLS_PLATFORM__OS_UNIX 1
        #define BSLS_PLATFORM__OS_HPUX 1
    #else
        #error "Unable to determine on which OS the HP compiler is running."
        char die[sizeof(bsls_Platform_Assert)];      // if '#error' unsupported
    #endif

    // which CPU -- should always be HPPA
    #if defined(__hppa__) || defined(__hppa)
        #define BSLS_PLATFORM__CPU_HPPA 1
    #elif defined(__ia64)                      // Itanium
        #define BSLS_PLATFORM__CPU_IA64 1

        #if defined(_LP64) || defined(__LP64__)
            #define BSLS_PLATFORM__CPU_64_BIT 1
        #else  // defined(_ILP32)
            #define BSLS_PLATFORM__CPU_32_BIT 1
        #endif
    #else
        #error "Unable to determine on which CPU the HP compiler is running."
        char die[sizeof(bsls_Platform_Assert)];      // if '#error' unsupported
    #endif
// ---------------------------------------------------------------------------
#elif defined(_MSC_VER)
    #define BSLS_PLATFORM__CMP_MSVC 1
    #define BSLS_PLATFORM__CMP_VER_MAJOR _MSC_VER

    // which OS -- should be some flavor of Windows
    // there is currently no support for:
    // - 16-bit versions of Windows (3.x)
    // - Windows CE
    #if defined(_WIN64) || defined(_WIN32)
        #define BSLS_PLATFORM__OS_WINDOWS 1
    #elif defined(_WIN16)
        #error "16-bit Windows platform not supported."
        char die[sizeof(bsls_Platform_Assert)];      // if '#error' unsupported
    #else
        #error "Microsoft OS is running on an unknown platform."
        char die[sizeof(bsls_Platform_Assert)];      // if '#error' unsupported
    #endif

    // which version of Windows
    #if _WIN32_WINNT >= 0x0501
        #define BSLS_PLATFORM__OS_WINXP 1
    #elif _WIN32_WINNT >= 0x0500
        #define BSLS_PLATFORM__OS_WIN2K 1
    #elif _WIN32_WINNT >= 0x0410
        #define BSLS_PLATFORM__OS_WIN9X 1
    #elif _WIN32_WINNT >= 0x0400
        #define BSLS_PLATFORM__OS_WINNT 1
    #elif defined(WINVER) && WINVER >= 0x0400 \
        || defined(_WIN32_WINDOWS) && _WIN32_WINDOWS >= 0x401
        #define BSLS_PLATFORM__OS_WIN9X 1
    #else
        #define BSLS_PLATFORM__OS_WINNT 1      // default
    #endif

    // set Version flags
    #if defined(_WIN32_WINNT)
        #define BSLS_PLATFORM__OS_VER_MAJOR _WIN32_WINNT / 0x100
        #define BSLS_PLATFORM__OS_VER_MINOR _WIN32_WINNT % 0x100
    #elif defined(WINVER)
        #define BSLS_PLATFORM__OS_VER_MAJOR WINVER / 0x100
        #define BSLS_PLATFORM__OS_VER_MINOR WINVER % 0x100
    #elif defined(_WIN32_WINDOWS)
        #define BSLS_PLATFORM__OS_VER_MAJOR _WIN32_WINDOWS / 0x100
        #define BSLS_PLATFORM__OS_VER_MINOR _WIN32_WINDOWS % 0x100
    #else                                      // default
        #define BSLS_PLATFORM__OS_VER_MAJOR 4
        #define BSLS_PLATFORM__OS_VER_MINOR 0
    #endif

    // which CPU
    // since WinCE is not supported, neither is the HITACHI CPU
    #if defined(_M_ALPHA)
        #define BSLS_PLATFORM__CPU_ALPHA 1
    #elif defined(_M_IX86)
        #define BSLS_PLATFORM__CPU_X86 1
        #define BSLS_PLATFORM__CPU_32_BIT 1
        #define BSLS_PLATFORM__CPU_VER_MAJOR _M_IX86
    #elif defined(_M_IA64)
        #if defined(_WIN64)
            #define BSLS_PLATFORM__CPU_IA64 1
            #define BSLS_PLATFORM__CPU_64_BIT 1 // native mode
        #else
            #define BSLS_PLATFORM__CPU_X86 1
            #define BSLS_PLATFORM__CPU_32_BIT 1 // emulated
        #endif
        #define BSLS_PLATFORM__CPU_VER_MAJOR _M_IA64
    #elif defined(_M_AMD64)
        #if defined(_WIN64)
            #define BSLS_PLATFORM__CPU_X86_64 1
            #define BSLS_PLATFORM__CPU_64_BIT 1 // native mode
        #else
            #define BSLS_PLATFORM__CPU_X86 1
            #define BSLS_PLATFORM__CPU_32_BIT 1 // emulated
        #endif
        #define BSLS_PLATFORM__CPU_VER_MAJOR _M_AMD64
    #elif defined(_M_PPC)
        #define BSLS_PLATFORM__CPU_POWERPC 1
        #define BSLS_PLATFORM__CPU_VER_MAJOR _M_PPC
    #elif defined(_M_MRX000)
        #define BSLS_PLATFORM__CPU_MIPS 1
        #define BSLS_PLATFORM__CPU_VER_MAJOR _M_MRX000
    #else
        #error "Unable to identify CPU on which the MSVC compiler is running."
        char die[sizeof(bsls_Platform_Assert)];      // if '#error' unsupported
    #endif

    // Simplify Windows development by providing defaults for the following
    // frequently-specified macros.  Note that the presence of other package
    // group names in this list in no way implies a dependency of BSL on these
    // other package groups.

    #ifndef BSL_DCL
    #define BSL_DCL
    #endif

    #ifndef BDE_DCL
    #define BDE_DCL
    #endif

    #ifndef BCE_DCL
    #define BCE_DCL
    #endif

    #ifndef BAE_DCL
    #define BAE_DCL
    #endif

    #ifndef BTE_DCL
    #define BTE_DCL
    #endif

    #ifndef BSI_DCL
    #define BSI_DCL
    #endif

    #ifndef BSC_DCL
    #define BSC_DCL
    #endif

    #ifndef BAS_DCL
    #define BAS_DCL
    #endif

    #ifndef USE_STATIC_XERCES
    #define USE_STATIC_XERCES
    #endif

    // Macro to suppress deprecation warnings about functions in POSIX version
    // of the C headers, but not specified in the C or C++ standards.
    // (CRT == C Runtime Libraries)

    #ifndef _CRT_NONSTDC_NO_DEPRECATE
    #define _CRT_NONSTDC_NO_DEPRECATE
    #endif

    // Macro to suppress deprecation warnings for functions in the CRT that are
    // deemed insecure.  For example, use of 'strcpy' would elicit a warning
    // unless the following macro is defined; use of 'strcpy_s' would not
    // elicit a warning regardless.

    #ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
    #endif

    // Macro to suppress warning C4996 for calling any one of the potentially
    // unsafe methods in the Standard C++ Library (this macro was necessitated
    // by VC2010).

    #ifndef _SCL_SECURE_NO_WARNINGS
    #define _SCL_SECURE_NO_WARNINGS
    #endif

    // Macro to disable "Checked Iterators".  If '_SECURE_SCL' is defined as 1,
    // unsafe iterator use causes a runtime error.  If defined as 0, checked
    // iterators are disabled.

    #ifndef _SECURE_SCL
    #define _SECURE_SCL 0
    #endif

    // Do not define 'min' and 'max' as macros in the Windows headers.

    #ifndef NOMINMAX
    #define NOMINMAX
    #endif

    #ifndef BDE_DO_NOT_HIDE_PEDANTIC_WINDOWS_WARNINGS // config macro name
    // Disable common warnings that are unreasonable to work around.  Most of
    // these warnings complain about deliberate and correct use of idioms
    // common in library code, but less so in application code.  As such, more
    // localized fixes would be preferred, restoring the warnings for
    // application developers.  Given the frequency of these warnings and the
    // lack of a portable and clear workaround, it is likely that these
    // 'pragma's will remain for the foreseeable future.
    #pragma warning(disable : 4347)  // Compiler fixed func-templates in 2003
    #pragma warning(disable : 4350)  // Compiler fixed call & with temp in 2003
    #pragma warning(disable : 4345)  // Compiler fixed 0-initialization in 2003
    #pragma warning(disable : 4351)  // Compiler fixed 0-initialization in 2005
    #pragma warning(disable : 4686)  // Compiler fixed template thing in 2003
    #pragma warning(disable : 4800)  // forcing value to bool 'true' or 'false'
    #pragma warning(disable : 4396)  // odd inline/friend conflict
    #pragma warning(disable : 4913)  // using built-in , rather than user-def
    #pragma warning(disable : 4127)  // conditional expression is constant
    #pragma warning(disable : 4610)  // cannot make objects of type 'class'
    #pragma warning(disable : 4512)  // could not generate assignment operator
    #pragma warning(disable : 4510)  // could not generate default constructor
    #pragma warning(disable : 4503)  // decorated name too long, truncated
    #pragma warning(disable : 4290)  // MSVC ignores exception specifications
    #pragma warning(disable : 4673)  // warns that warning 4670 follows
    #pragma warning(disable : 4670)  // thrown exception has inaccessible base

    #pragma warning(disable : 4514)  // unused inline function
    #pragma warning(disable : 4571)  // catch(...) no longer handles SEH
    #pragma warning(disable : 4574)  // #ifdef on macro defined to be 0
    #pragma warning(disable : 4668)  // undefined macros #if as 0
    #pragma warning(disable : 4640)  // fn-local static init not threadsafe

    // These nags could be hints to document private ops in derived class
    #pragma warning(disable : 4625)  // base class copy ctor is private
    #pragma warning(disable : 4626)  // base class operator= is private

    // These informational hints might be useful on rare occasions
    #pragma warning(disable : 4710)  // function was not inlined
    #pragma warning(disable : 4820)  // padding bytes added to data structure
    #endif

    #ifdef BDE_HIDE_COMMON_WINDOWS_WARNINGS // config macro name
    // Short-term noise reduction: These warnings are noisy but should be
    // eliminated at source.
    #pragma warning(disable : 4018)  // signed/unsigned mismatch operator <
    #pragma warning(disable : 4389)  // signed/unsigned mismatch operator==
    #pragma warning(disable : 4245)  // signed/unsigned mismatch in conversion
    #pragma warning(disable : 4244)  // conversion may lose data
    #pragma warning(disable : 4310)  // cast truncates constant value
    #pragma warning(disable : 4309)  // initialization truncates constant value
    #pragma warning(disable : 4305)  // initialization truncates in conversion
    #pragma warning(disable : 4189)  // unused local variable is initialized
    #pragma warning(disable : 4101)  // local variable is not used
    #pragma warning(disable : 4100)  // unused function parameter
    #pragma warning(disable : 4805)  // unsafe mix of bool
    #pragma warning(disable : 4702)  // unreachable code (likely in templates)
    #pragma warning(disable : 4505)  // unreferenced local function removed
    #pragma warning(disable : 4661)  // type traits abuses operator ,

    // Be very careful with the following warnings.  A large number are
    // generated from test drivers containing correct code, but a small number
    // of genuine bugs have also been highlighted.
    #pragma warning(disable : 4701)  // potentially uninitialized variable
    #pragma warning(disable : 4706)  // assignment in conditional expression

    // TBD
    // Warning #4267 swamps the signal/noise by 2-3 orders of magnitude when
    // building for a 64-bit target.  Many of these warnings should really
    // be dealt with, but for now we are silencing them.  This pragma should be
    // removed and the warnings addressed in a future release.
    #pragma warning(disable : 4267)  // conversion from 'size_t' to int
    #endif
// ---------------------------------------------------------------------------
#elif defined(__GNUC__) || defined(__EDG__)

    #if defined (__GNUC__)
        #define BSLS_PLATFORM__CMP_GNU 1
        #if defined(__GNU_PATCHLEVEL__)
            #define BSLS_PLATFORM__CMP_VER_MAJOR (__GNUC__ * 10000 \
                        + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
        #else
            #define BSLS_PLATFORM__CMP_VER_MAJOR (__GNUC__ * 10000 \
                        + __GNUC_MINOR__ * 100)
        #endif
    #else
        #define BSLS_PLATFORM__CMP_EDG 1
        #define BSLS_PLATFORM__CMP_VER_MAJOR __EDG_VERSION__
    #endif

    // which OS -- GNU and EDG/Como are implemented almost everywhere
    #if defined(_AIX)
        #define BSLS_PLATFORM__OS_AIX 1
    #elif defined(hpux) || defined(__hpux)
        #define BSLS_PLATFORM__OS_HPUX 1
    #elif defined(__CYGWIN__) || defined(cygwin) || defined(__cygwin)
        #define BSLS_PLATFORM__OS_CYGWIN 1
    #elif defined(linux) || defined(__linux)
        #define BSLS_PLATFORM__OS_LINUX 1
    #elif defined(__FreeBSD__)
        #define BSLS_PLATFORM__OS_FREEBSD 1
    #elif defined(sun) || defined(__sun)
        #if defined(__SVR4) || defined(__svr4__)
            #define BSLS_PLATFORM__OS_SOLARIS 1
        #else
            #define BSLS_PLATFORM__OS_SUNOS 1
        #endif
    #elif defined(_WIN32) || defined(__WIN32__) && \
          ! (defined(cygwin) || defined(__cygwin))
        #define BSLS_PLATFORM__OS_WINDOWS 1
    #elif defined(__APPLE__) 
        #define BSLS_PLATFORM__OS_DARWIN 1
    #else
        #if defined(__GNUC__)
            #error "Unable to determine on which OS GNU compiler is running."
        #else
            #error "Unable to determine on which OS EDG compiler is running."
        #endif
        char die[sizeof(bsls_Platform_Assert)];      // if '#error' unsupported
    #endif

    #if !defined(BSLS_PLATFORM__OS_WINDOWS)
        #define BSLS_PLATFORM__OS_UNIX 1
    #endif

    // which CPU -- GNU and EDG/Como implemented almost everywhere
    #if defined(__alpha__)
        #define BSLS_PLATFORM___CPU_ALPHA 1
    #elif defined(__x86_64) || defined(__x86_64__)
        #define BSLS_PLATFORM__CPU_X86_64 1
        #define BSLS_PLATFORM__CPU_64_BIT 1
    #elif defined(__i386) || defined(__i386__) \
        || defined(__ix86) || defined(__ix86__)
        #define BSLS_PLATFORM__CPU_X86 1
        #define BSLS_PLATFORM__CPU_32_BIT 1
    #elif defined(__ia64) || defined(__ia64__) || defined(_IA64) \
          || defined(__IA64__)
        #define BSLS_PLATFORM__CPU_IA64 1
        #if defined(_LP64) || defined(__LP64__)
            #define BSLS_PLATFORM__CPU_64_BIT 1
        #else  // defined(_ILP32)
            #define BSLS_PLATFORM__CPU_32_BIT 1
        #endif
    #elif defined(__mips__)
        #define BSLS_PLATFORM__CPU_MIPS 1
    #elif defined(__hppa__) || defined(__hppa)
        #define BSLS_PLATFORM__CPU_HPPA 1
    #elif defined(__powerpc) || defined(__powerpc__) \
          || defined(__POWERPC__) || defined(__ppc__) || defined(_POWER)
        #define BSLS_PLATFORM__CPU_POWERPC 1
        #if defined(__64BIT__)
            #define BSLS_PLATFORM__CPU_64_BIT 1
        #endif
    #elif defined(__sparc__) || defined(__sparc_v9__) || defined(__sparcv9)
        #define BSLS_PLATFORM__CPU_SPARC 1
        #if defined(__sparc_v9__) || defined(__sparcv9) || defined(__arch64__)
            #define BSLS_PLATFORM__CPU_SPARC_V9 1
            #define BSLS_PLATFORM__CPU_64_BIT 1
        #else
            #define BSLS_PLATFORM__CPU_SPARC_32 1
        #endif
    #else
        #if defined(__GNUC__)
            #error "Unable to determine on which CPU GNU compiler is running."
        #else
            #error "Unable to determine on which CPU EDG compiler is running."
        #endif
        char die[sizeof(bsls_Platform_Assert)];      // if '#error' unsupported
    #endif

    #if !defined(BSLS_PLATFORM__CPU_64_BIT)
        #define BSLS_PLATFORM__CPU_32_BIT 1
    #endif
// ---------------------------------------------------------------------------
#elif defined(__SUNPRO_CC) || defined(__SUNPRO_C)

    #define BSLS_PLATFORM__CMP_SUN 1
    #if defined(__cplusplus)
        #define BSLS_PLATFORM__CMP_VER_MAJOR __SUNPRO_CC
    #else
        #define BSLS_PLATFORM__CMP_VER_MAJOR __SUNPRO_C
    #endif

    // which OS
    #define BSLS_PLATFORM__OS_UNIX 1
    #if defined(sun) || defined(__sun)
        #define BSLS_PLATFORM__OS_SOLARIS 1
        #if defined(__SVR4) || defined(__svr4__)
            #if defined(__SunOS_5_7)
                #define BSLS_PLATFORM__OS_VER_MAJOR 7
                #define BSLS_PLATFORM__OS_VER_MINOR 0
            #elif defined(__SunOS_5_8)
                #define BSLS_PLATFORM__OS_VER_MAJOR 8
                #define BSLS_PLATFORM__OS_VER_MINOR 0
            #elif defined(__SunOS_5_9)
                #define BSLS_PLATFORM__OS_VER_MAJOR 9
                #define BSLS_PLATFORM__OS_VER_MINOR 0
            #elif defined(__SunOS_5_10)
                #define BSLS_PLATFORM__OS_VER_MAJOR 10
                #define BSLS_PLATFORM__OS_VER_MINOR 0
            #else
                #define BSLS_PLATFORM__OS_VER_MAJOR 1
                #define BSLS_PLATFORM__OS_VER_MINOR 0
            #endif
        #else
            #error "Unable to determine SUN OS version."
            char die[sizeof(bsls_Platform_Assert)];  // if '#error' unsupported
        #endif
    #elif defined(__SVR4) || defined(__svr4__)
        #define BSLS_PLATFORM__OS_SUNOS 1
        #if defined(__SunOS_5_7)
            #define BSLS_PLATFORM__OS_VER_MAJOR 7
            #define BSLS_PLATFORM__OS_VER_MINOR 0
        #elif defined(__SunOS_5_8)
            #define BSLS_PLATFORM__OS_VER_MAJOR 8
            #define BSLS_PLATFORM__OS_VER_MINOR 0
        #elif defined(__SunOS_5_9)
            #define BSLS_PLATFORM__OS_VER_MAJOR 9
            #define BSLS_PLATFORM__OS_VER_MINOR 0
        #else
            #define BSLS_PLATFORM__OS_VER_MAJOR 1
        #endif
    #else
        #error "Unable to determine SUN OS version."
        char die[sizeof(bsls_Platform_Assert)];      // if '#error' unsupported
    #endif

    // determine which CPU
    #if defined(__x86_64) || defined(__x86_64__)
        #define BSLS_PLATFORM__CPU_X86_64 1
        #define BSLS_PLATFORM__CPU_64_BIT 1
    #elif defined(i386) || defined(__i386)
        #define BSLS_PLATFORM__CPU_X86 1
        #define BSLS_PLATFORM__CPU_32_BIT 1
    #elif defined(__sparc64) || defined(__sparcv9)
        #define BSLS_PLATFORM__CPU_SPARC 1
        #define BSLS_PLATFORM__CPU_SPARC_V9 1
        #define BSLS_PLATFORM__CPU_64_BIT 1
    #elif defined(sparc) || defined(__sparc)
        #define BSLS_PLATFORM__CPU_SPARC 1
        #define BSLS_PLATFORM__CPU_SPARC_32 1
        #define BSLS_PLATFORM__CPU_32_BIT 1
    #else
        #error "Cannot determine CPU on which the SUN compiler is running."
        char die[sizeof(bsls_Platform_Assert)];      // if '#error' unsupported
    #endif
// ---------------------------------------------------------------------------
#else
    #error "Could not identify the compiler."
    char die[sizeof(bsls_Platform_Assert)];          // if '#error' unsupported
#endif
// ---------------------------------------------------------------------------

#if defined(BSLS_PLATFORM__CPU_X86_64) || defined(BSLS_PLATFORM__CPU_X86)
    #define BSLS_PLATFORM__IS_LITTLE_ENDIAN 1
#endif

#if !defined(BSLS_PLATFORM__IS_LITTLE_ENDIAN)
   #define BSLS_PLATFORM__IS_BIG_ENDIAN 1
#endif
// ----------------------------------------------------------------------------

                        // Miscellaneous Platform Macros

#ifdef BSLS_PLATFORM__CMP_MSVC
    #if BSLS_PLATFORM__CMP_VER_MAJOR <= 1200
        #define BSLS_PLATFORM__NO_LONG_HEADER_NAMES 1
    #endif
#endif

#if defined(BSLS_PLATFORM__CMP_GNU)
    #define BSLS_PLATFORM__NO_64_BIT_CONSTANTS 1
#endif

#if defined(BSLS_PLATFORM__CMP_IBM) && !defined(BSLS_PLATFORM__CPU_64_BIT)
    #define BSLS_PLATFORM__NO_64_BIT_CONSTANTS 1
#endif
// ----------------------------------------------------------------------------

                                 // Validation

// Unix flag must be set by the compiler if Unix detected (except for AIX).
#if defined(BSLS_PLATFORM__OS_UNIX) && !defined(BSLS_PLATFORM__OS_AIX)
    #if !defined(unix) && !defined(__unix__) && !defined(__unix)
        #error "Unix platform assumed, but unix flag not set by compiler"
        char die[sizeof(bsls_Platform_Assert)];      // if '#error' unsupported
    #endif
#endif

// Exactly one CMP type.
#if BSLS_PLATFORM__CMP_EDG  \
  + BSLS_PLATFORM__CMP_GNU  \
  + BSLS_PLATFORM__CMP_HP   \
  + BSLS_PLATFORM__CMP_IBM  \
  + BSLS_PLATFORM__CMP_MSVC \
  + BSLS_PLATFORM__CMP_SUN != 1
    #error "Exactly one compiler must be set."
    char die[sizeof(bsls_Platform_Assert)];          // if '#error' unsupported
#endif

// Exactly one OS type.
#if BSLS_PLATFORM__OS_UNIX \
  + BSLS_PLATFORM__OS_WINDOWS != 1
    #error "Exactly one operating system must be set."
    char die[sizeof(bsls_Platform_Assert)];          // if '#error' unsupported
#endif

// At most one OS subtype.

#define BSLS_PLATFORM__OS_SUBTYPE_COUNT \
    BSLS_PLATFORM__OS_AIX     \
  + BSLS_PLATFORM__OS_DARWIN  \
  + BSLS_PLATFORM__OS_HPUX    \
  + BSLS_PLATFORM__OS_LINUX   \
  + BSLS_PLATFORM__OS_FREEBSD \
  + BSLS_PLATFORM__OS_SOLARIS \
  + BSLS_PLATFORM__OS_SUNOS   \
  + BSLS_PLATFORM__OS_CYGWIN  \
  + BSLS_PLATFORM__OS_WIN9X   \
  + BSLS_PLATFORM__OS_WINNT   \
  + BSLS_PLATFORM__OS_WIN2K   \
  + BSLS_PLATFORM__OS_WINXP
#if BSLS_PLATFORM__OS_SUBTYPE_COUNT > 1
    #error "At most one operating system subtype must be set."
    char die[sizeof(bsls_Platform_Assert)];          // if '#error' unsupported
#endif

// Exactly one CPU type.
#if BSLS_PLATFORM__CPU_88000   \
  + BSLS_PLATFORM__CPU_ALPHA   \
  + BSLS_PLATFORM__CPU_HPPA    \
  + BSLS_PLATFORM__CPU_IA64    \
  + BSLS_PLATFORM__CPU_X86     \
  + BSLS_PLATFORM__CPU_X86_64  \
  + BSLS_PLATFORM__CPU_MIPS    \
  + BSLS_PLATFORM__CPU_POWERPC \
  + BSLS_PLATFORM__CPU_SPARC != 1
    #error "Exactly one processor must be set."
    char die[sizeof(bsls_Platform_Assert)];          // if '#error' unsupported
#endif

// Exactly one kind of "endian-ness".
#if BSLS_PLATFORM__IS_BIG_ENDIAN \
  + BSLS_PLATFORM__IS_LITTLE_ENDIAN != 1
    #error "Exactly one kind of endian-ness must be set."
    char die[sizeof(bsls_Platform_Assert)];          // if '#error' unsupported
#endif

#if defined(BSLS_PLATFORM__OS_VER_MAJOR) \
         && BSLS_PLATFORM__OS_SUBTYPE_COUNT != 1
        // For OS, MAJOR VERSION implies SUBTYPE.
    #error "Operating system major version by not subtype defined."
    char die[sizeof(bsls_Platform_Assert)];          // if '#error' unsupported
#endif

#undef BSLS_PLATFORM__OS_SUBTYPE_COUNT

#if defined(BSLS_PLATFORM__CMP_VER_MINOR) && \
   !defined(BSLS_PLATFORM__CMP_VER_MAJOR)
        // For each category, MINOR VERSION implies MAJOR VERSION
    #error "Compiler minor but not major version defined."
    char die[sizeof(bsls_Platform_Assert)];          // if '#error' unsupported
#endif

#if defined(BSLS_PLATFORM__OS_VER_MINOR) && \
   !defined(BSLS_PLATFORM__OS_VER_MAJOR)
    #error "Operating System minor but not major version defined."
    char die[sizeof(bsls_Platform_Assert)];          // if '#error' unsupported
#endif

#if defined(BSLS_PLATFORM__CPU_VER_MINOR) && \
   !defined(BSLS_PLATFORM__CPU_VER_MAJOR)
    #error "Processor minor but not major version defined."
    char die[sizeof(bsls_Platform_Assert)];          // if '#error' unsupported
#endif
// ----------------------------------------------------------------------------

                               // =============
                               // bsls_Platform
                               // =============

struct bsls_Platform {
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

                              // PLATFORM TRAITS

    // OS TRAIT
    // Will fail to compile if more than one OS type is set.

    #if defined(BSLS_PLATFORM__OS_AIX)
        #if (BSLS_PLATFORM__OS_VER_MAJOR == _AIX)
            typedef OsAix_V5         Os;
        #else
            typedef OsAix            Os;
        #endif
    #endif
    #if defined BSLS_PLATFORM__OS_SOLARIS
        #if   (BSLS_PLATFORM__OS_VER_MAJOR == 9)
            typedef OsSolaris_V9     Os;
        #elif (BSLS_PLATFORM__OS_VER_MAJOR == 10)
            typedef OsSolaris_V10    Os;
        #else
            typedef OsSolaris_V8     Os;
        #endif
    #endif
    #if defined(BSLS_PLATFORM__OS_HPUX)
        typedef OsHpux              Os;
    #endif
    #if defined(BSLS_PLATFORM__OS_CYGWIN)
        typedef OsCygwin            Os;
    #endif
    #if defined(BSLS_PLATFORM__OS_LINUX)
        typedef OsLinux             Os;
    #endif
    #if defined(BSLS_PLATFORM__OS_FREEBSD)
        typedef OsFreeBsd           Os;
    #endif
    #if defined(BSLS_PLATFORM__OS_WINDOWS)
        typedef OsWinNT             Os;
    #endif
    #if defined(BSLS_PLATFORM__OS_DARWIN)
        typedef OsDarwin            Os;
    #endif

    // CPU TRAIT
    // Will fail to compile if more than one CPU type is set.

    #if defined(BSLS_PLATFORM__CPU_X86)
        typedef CpuX86  Cpu;
    #endif
    #if defined(BSLS_PLATFORM__CPU_IA64)
        typedef CpuIa64 Cpu;
    #endif
    #if defined(BSLS_PLATFORM__CPU_X86_64)
        typedef CpuX86_64  Cpu;
    #endif
    #if defined(BSLS_PLATFORM__CPU_POWERPC)
        #if defined(BSLS_PLATFORM__CPU_VER_MAJOR) &&  \
                                    (BSLS_PLATFORM__CPU_VER_MAJOR == _ARCH_601)
            typedef CpuPowerpc_601 Cpu;
        #else
            typedef CpuPowerpc     Cpu;
        #endif
    #endif
    #if defined BSLS_PLATFORM__CPU_SPARC_32
        typedef CpuSparc_32   Cpu;
    #endif
    #if defined BSLS_PLATFORM__CPU_SPARC_V9
        typedef CpuSparc_V9   Cpu;
    #endif

};

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

                      // ======================
                      // BACKWARD COMPATIBILITY
                      // ======================

#ifdef BSLS_PLATFORM__OS_UNIX
#define BDES_PLATFORM__OS_UNIX        BSLS_PLATFORM__OS_UNIX
#endif

#ifdef BSLS_PLATFORM__OS_WINDOWS
#define BDES_PLATFORM__OS_WINDOWS     BSLS_PLATFORM__OS_WINDOWS
#endif

#ifdef BSLS_PLATFORM__OS_VER_MAJOR
#define BDES_PLATFORM__OS_VER_MAJOR   BSLS_PLATFORM__OS_VER_MAJOR
#endif

#ifdef BSLS_PLATFORM__OS_VER_MINOR
#define BDES_PLATFORM__OS_VER_MINOR   BSLS_PLATFORM__OS_VER_MINOR
#endif

#ifdef BSLS_PLATFORM__OS_AIX
#define BDES_PLATFORM__OS_AIX         BSLS_PLATFORM__OS_AIX
#endif

#ifdef BSLS_PLATFORM__OS_CYGWIN
#define BDES_PLATFORM__OS_CYGWIN      BSLS_PLATFORM__OS_CYGWIN
#endif

#ifdef BSLS_PLATFORM__OS_HPUX
#define BDES_PLATFORM__OS_HPUX        BSLS_PLATFORM__OS_HPUX
#endif

#ifdef BSLS_PLATFORM__OS_LINUX
#define BDES_PLATFORM__OS_LINUX       BSLS_PLATFORM__OS_LINUX
#endif

#ifdef BSLS_PLATFORM__OS_DARWIN
#define BDES_PLATFORM__OS_DARWIN      BSLS_PLATFORM__OS_DARWIN
#endif

#ifdef BSLS_PLATFORM__OS_FREEBSD
#define BDES_PLATFORM__OS_FREEBSD     BSLS_PLATFORM__OS_FREEBSD
#endif

#ifdef BSLS_PLATFORM__OS_SOLARIS
#define BDES_PLATFORM__OS_SOLARIS     BSLS_PLATFORM__OS_SOLARIS
#endif

#ifdef BSLS_PLATFORM__OS_SUNOS
#define BDES_PLATFORM__OS_SUNOS       BSLS_PLATFORM__OS_SUNOS
#endif

#ifdef BSLS_PLATFORM__OS_WIN2K
#define BDES_PLATFORM__OS_WIN2K       BSLS_PLATFORM__OS_WIN2K
#endif

#ifdef BSLS_PLATFORM__OS_WIN9X
#define BDES_PLATFORM__OS_WIN9X       BSLS_PLATFORM__OS_WIN9X
#endif

#ifdef BSLS_PLATFORM__OS_WINNT
#define BDES_PLATFORM__OS_WINNT       BSLS_PLATFORM__OS_WINNT
#endif

#ifdef BSLS_PLATFORM__OS_WINXP
#define BDES_PLATFORM__OS_WINXP       BSLS_PLATFORM__OS_WINXP
#endif

#ifdef BSLS_PLATFORM__CMP_VER_MAJOR
#define BDES_PLATFORM__CMP_VER_MAJOR  BSLS_PLATFORM__CMP_VER_MAJOR
#endif

#ifdef BSLS_PLATFORM__CMP_VER_MINOR
#define BDES_PLATFORM__CMP_VER_MINOR  BSLS_PLATFORM__CMP_VER_MINOR
#endif

#ifdef BSLS_PLATFORM__CMP_IBM
#define BDES_PLATFORM__CMP_AIX        BSLS_PLATFORM__CMP_IBM
#endif

#ifdef BSLS_PLATFORM__CMP_EDG
#define BDES_PLATFORM__CMP_EDG        BSLS_PLATFORM__CMP_EDG
#endif

#ifdef BSLS_PLATFORM__CMP_GNU
#define BDES_PLATFORM__CMP_GNU        BSLS_PLATFORM__CMP_GNU
#endif

#ifdef BSLS_PLATFORM__CMP_HP
#define BDES_PLATFORM__CMP_HP         BSLS_PLATFORM__CMP_HP
#endif

#ifdef BSLS_PLATFORM__CMP_MSVC
#define BDES_PLATFORM__CMP_MSVC       BSLS_PLATFORM__CMP_MSVC
#endif

#ifdef BSLS_PLATFORM__CMP_SUN
#define BDES_PLATFORM__CMP_SUN        BSLS_PLATFORM__CMP_SUN
#endif

#ifdef BSLS_PLATFORM__CPU_VER_MAJOR
#define BDES_PLATFORM__CPU_VER_MAJOR  BSLS_PLATFORM__CPU_VER_MAJOR
#endif

#ifdef BSLS_PLATFORM__CPU_VER_MINOR
#define BDES_PLATFORM__CPU_VER_MINOR  BSLS_PLATFORM__CPU_VER_MINOR
#endif

#ifdef BSLS_PLATFORM__CPU_64_BIT
#define BDES_PLATFORM__CPU_64_BIT     BSLS_PLATFORM__CPU_64_BIT
#endif

#ifdef BSLS_PLATFORM__CPU_32_BIT
#define BDES_PLATFORM__CPU_32_BIT     BSLS_PLATFORM__CPU_32_BIT
#endif

#ifdef BSLS_PLATFORM__CPU_88000
#define BDES_PLATFORM__CPU_88000      BSLS_PLATFORM__CPU_88000
#endif

#ifdef BSLS_PLATFORM__CPU_ALPHA
#define BDES_PLATFORM__CPU_ALPHA      BSLS_PLATFORM__CPU_ALPHA
#endif

#ifdef BSLS_PLATFORM__CPU_HPPA
#define BDES_PLATFORM__CPU_HPPA       BSLS_PLATFORM__CPU_HPPA
#endif

#ifdef BSLS_PLATFORM__CPU_IA64
#define BDES_PLATFORM__CPU_IA64       BSLS_PLATFORM__CPU_IA64
#endif

#ifdef BSLS_PLATFORM__CPU_INTEL
#define BDES_PLATFORM__CPU_INTEL      BSLS_PLATFORM__CPU_INTEL
#endif

#ifdef BSLS_PLATFORM__CPU_MIPS
#define BDES_PLATFORM__CPU_MIPS       BSLS_PLATFORM__CPU_MIPS
#endif

#ifdef BSLS_PLATFORM__CPU_POWERPC
#define BDES_PLATFORM__CPU_POWERPC    BSLS_PLATFORM__CPU_POWERPC
#endif

#ifdef BSLS_PLATFORM__CPU_SPARC
#define BDES_PLATFORM__CPU_SPARC      BSLS_PLATFORM__CPU_SPARC
#endif

#ifdef BSLS_PLATFORM__CPU_SPARC_32
#define BDES_PLATFORM__CPU_SPARC_32   BSLS_PLATFORM__CPU_SPARC_32
#endif

#ifdef BSLS_PLATFORM__CPU_SPARC_V9
#define BDES_PLATFORM__CPU_SPARC_V9   BSLS_PLATFORM__CPU_SPARC_V9
#endif

#ifdef BSLS_PLATFORM__CPU_X86
#define BDES_PLATFORM__CPU_X86        BSLS_PLATFORM__CPU_X86
#endif

#ifdef BSLS_PLATFORM__CPU_X86_64
#define BDES_PLATFORM__CPU_X86_64     BSLS_PLATFORM__CPU_X86_64
#endif

typedef bsls_Platform bdes_Platform;
    // This alias is defined for backward compatibility.

#endif

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
