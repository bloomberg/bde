// bdes_platform.h                 -*-C++-*-
#ifndef INCLUDED_BDES_PLATFORM
#define INCLUDED_BDES_PLATFORM

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide preprocessor support for platform identification.
//
//@DEPRECATED: Use 'bsls_platform' instead.
//
//@CLASSES:
//   bdes_Platform: a namespace containing platform identification structs
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component defines preprocessor macros that identify and
// describe platform-specific attributes (at compile time).  These attributes
// consist of the types and versions of
//..
//   - the operating system
//   - the processor(s)
//   - the compiler
//..
// that comprise the platform.  By including this header file, most, but not
// all, of the following macros are configured automatically at compile time;
// the remainder can be set using compile-time switches.
//
// For the sake of brevity in the following table, the '*' character will be
// used to represent the characters 'BDES_PLATFORM'
//..
//      =============================================================
//                        OPERATING SYSTEM
//      -------------------------------------------------------------
//      Type                Subtype               Version
//      -----------------   -------------------   -------------------
//      *__OS_UNIX          *__OS_AIX             *__OS_VER_MAJOR
//                          *__OS_DGUX            *__OS_VER_MINOR
//                          *__OS_HPUX
//                          *__OS_LINUX
//                          *__OS_SOLARIS
//                          *__OS_SUNOS
//                          *__OS_CYGWIN
//
//      *__OS_WINDOWS       *__OS_WIN9X
//                          *__OS_WINNT
//                          *__OS_WIN2K
//                          *__OS_WINXP
//      =============================================================
//                          PROCESSOR
//      -------------------------------------------------------------
//      Instruction Set          Width                 Version
//      -----------------   -------------------   -------------------
//      *__CPU_88000        *__CPU_32_BIT         *__CPU_VER_MAJOR
//      *__CPU_ALPHA        *__CPU_64_BIT         *__CPU_VER_MINOR
//      *__CPU_HPPA
//      *__CPU_INTEL^
//      *__CPU_X86
//      *__CPU_IA64
//      *__CPU_X86_64
//      *__CPU_MIPS
//      *__CPU_POWERPC
//      *__CPU_SPARC
//        ^ - DEPRECATED
//      =============================================================
//                          COMPILER
//      -------------------------------------------------------------
//      Vendor                    Version
//      -----------------   -------------------
//      *__CMP_AIX          *__CMP_VER_MAJOR
//      *__CMP_EDG
//      *__CMP_EPC
//      *__CMP_GNU
//      *__CMP_HP
//      *__CMP_MSVC
//      *__CMP_SUN
//      ==========================================================
//..
// Where possible, the macros described above are configured automatically.
// At a minimum, the generic operating system type (i.e., either
// 'BDES_PLATFORM__OS_UNIX' or 'BDES_PLATFORM__OS_WINDOWS') is defined along
// with exactly one processor macro (e.g., 'BDES_PLATFORM__CPU_SPARC') and
// exactly one compiler macro (e.g., 'BDES_PLATFORM__CMP_SUN').  Clients may be
// required to supply user-defined macros (typically specified via the '-D'
// option of a compiler) if the client software needs to discriminate further
// based on operating system subtypes or individual versions of the specific
// operating system, processor, or compiler.  Note that supplying a minor
// version number implies that the major version is also defined.
//
///Usage
///-----
// Writing portable software at times involves specializing implementations to
// work with system-specific features.  For example, a socket-level
// communications framework would undoubtedly need to differentiate Windows
// Handle objects and Unix socket descriptors, but specialization of the
// different versions of Unix or the different versions of Windows is probably
// not needed.
//..
//    // my_socket.h
//    #include <bdes_platform.h>
//
//    #ifdef BDES_PLATFORM__OS_WINDOWS
//    #  ifndef INCLUDED_WINSOCK2
//    #  include <winsock2.h>
//    #  define INCLUDED_WINSOCK2
//    #  endif
//    #endif
//
//    class my_Socket {
//    #ifdef BDES_PLATFORM__OS_WINDOWS
//        SOCKET d_sockfd;   // Windows SOCKET handler
//    #else
//        int d_sockfd;      // UNIX socket descriptor
//    #endif
//
//    // ...
//
//    };
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>  // required by 'bdes'
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif


// Check using legacy macros only if 'BDE_OMIT_INTERNAL_DEPRECATED' is not 
// defined.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

    // No symbols or macros are defined here (see 'bsls_platform' component).
    // No aliases or macros for the 'bdes_Platform' types or 'BDES_PLATFORM_*'
    // macros are defined here, instead they are defined in 'bsls_platform' so
    // that clients that rely on these via transitive includes may still have
    // those aliases or macros defined.

                                 // ==========
                                 // Validation
                                 // ==========

// Unix flag must be set by the compiler if Unix detected (except for AIX).
#if defined(BDES_PLATFORM__OS_UNIX) && !defined(BDES_PLATFORM__OS_AIX)
    #if !defined(unix) && !defined(__unix__) && !defined(__unix)
        #error "Unix platform assumed, but unix flag not set by compiler"
        char assertion[0];                     // stop non-compliant compilers
    #endif
#endif

// Exactly one CMP type.
#if BDES_PLATFORM__CMP_AIX \
  + BDES_PLATFORM__CMP_EPC \
  + BDES_PLATFORM__CMP_GNU \
  + BDES_PLATFORM__CMP_EDG \
  + BDES_PLATFORM__CMP_HP \
  + BDES_PLATFORM__CMP_MSVC \
  + BDES_PLATFORM__CMP_SUN != 1
    #error "Exactly one compiler must be set."
    char assertion[0];                         // stop non-compliant compilers
#endif

// Exactly one OS type.
#if BDES_PLATFORM__OS_UNIX \
  + BDES_PLATFORM__OS_WINDOWS != 1
    #error "Exactly one operating system must be set."
    char assertion[0];                         // stop non-compliant compilers
#endif

// At most one OS subtype.
#define BDES_PLATFORM__OS_SUBTYPE_COUNT \
    BDES_PLATFORM__OS_AIX \
  + BDES_PLATFORM__OS_DGUX \
  + BDES_PLATFORM__OS_HPUX \
  + BDES_PLATFORM__OS_LINUX \
  + BDES_PLATFORM__OS_SOLARIS \
  + BDES_PLATFORM__OS_SUNOS \
  + BDES_PLATFORM__OS_CYGWIN \
  + BDES_PLATFORM__OS_WIN9X \
  + BDES_PLATFORM__OS_WINNT \
  + BDES_PLATFORM__OS_WIN2K \
  + BDES_PLATFORM__OS_WINXP
#if BDES_PLATFORM__OS_SUBTYPE_COUNT > 1
    #error "At most one operating system subtype must be set."
    char assertion[0];                         // stop non-compliant compilers
#endif

// Exactly one CPU type.
#if BDES_PLATFORM__CPU_88000 \
  + BDES_PLATFORM__CPU_ALPHA \
  + BDES_PLATFORM__CPU_HPPA \
  + BDES_PLATFORM__CPU_IA64 \
  + BDES_PLATFORM__CPU_X86 \
  + BDES_PLATFORM__CPU_X86_64 \
  + BDES_PLATFORM__CPU_MIPS \
  + BDES_PLATFORM__CPU_POWERPC \
  + BDES_PLATFORM__CPU_SPARC != 1
    #error "Exactly one processor must be set."
    char assertion[0];                         // stop non-compliant compilers
#endif


#if defined(BDES_PLATFORM__OS_VER_MAJOR) \
         && BDES_PLATFORM__OS_SUBTYPE_COUNT != 1
        // For OS, MAJOR VERSION implies SUBTYPE.
    #error "Operating system major version but not subtype defined."
    char assertion[0];                         // stop non-compliant compilers
#endif

#undef BDES_PLATFORM__OS_SUBTYPE_COUNT

#if defined(BDES_PLATFORM__OS_VER_MINOR) && \
   !defined(BDES_PLATFORM__OS_VER_MAJOR)
    #error "Operating System minor but not major version defined."
    char assertion[0];                         // stop non-compliant compilers
#endif

#if defined(BDES_PLATFORM__CPU_VER_MINOR) && \
   !defined(BDES_PLATFORM__CPU_VER_MAJOR)
    #error "Processor minor but not major version defined."
    char assertion[0];                         // stop non-compliant compilers
#endif

#endif // BDE_OMIT_INTERNAL_DEPRECATED

namespace BloombergLP {

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
