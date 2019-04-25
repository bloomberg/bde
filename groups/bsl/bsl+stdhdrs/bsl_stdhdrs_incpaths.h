/* bsl_stdhdrs_incpaths.h                                            -*-C-*- */
#ifndef INCLUDED_BSL_STDHDRS_INCPATHS
#define INCLUDED_BSL_STDHDRS_INCPATHS

#ifdef __cplusplus
#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Define paths where native headers can be found.
//
//@MACROS:
// BSL_NATIVE_CPP_LIB_HEADER(header):  path to C++ standard header
// BSL_NATIVE_CPP_RUNTIME_HEADER(header): path to C++ run-time header
// BSL_NATIVE_CPP_C_HEADER(header): path to C++ versions of C standard header
// BSL_NATIVE_CPP_DEPRECATED_HEADER(header): path to deprecated C++ STD header
// BSL_NATIVE_C_LIB_HEADER(header): path to C standard header (end in '.h')
// BSL_NATIVE_OS_RTL_HEADER(header): path to a subset of C standard headers
// BSL_NATIVE_OS_STDDEF_HEADER(header): path to standard C stddef.h header
//
//@DESCRIPTION: [!PRIVATE!] This component is intended for use only by other
// components in the bsl package group.  The stability of this interface is not
// guaranteed.
//
// The macros in this component allow a source file compiled in bde-stl mode to
// include the compiler's native C++ library headers, even when the compiler's
// search path would otherwise find the headers with the same name in the
// 'bsl+stdhdrs' package.  The actual paths are hard coded to be platform
// specific.
//
// Each macro takes a 'filename' argument and expands to the path for the file
// in the native compiler's directory.  The expanded macros are suitable for
// use in '#include' directives.  In order to find the native file, the correct
// macro must be used for each header file, as follows:
//..
//  Macro                               Used for
//  =================================   ======================================
//  BSL_NATIVE_CPP_LIB_HEADER           Most C++ standard header files
//
//  BSL_NATIVE_CPP_RUNTIME_HEADER       Headers closely related to the
//                                      compiler ('<exception>', '<typeinfo>',
//                                      and '<new>')
//
//  BSL_NATIVE_CPP_C_HEADER             C++ standard versions of the C-language
//                                      library (e.g., '<cstdlib>', '<cctype>')
//
//  BSL_NATIVE_CPP_DEPRECATED_HEADER    Headers to C++ standard header files
//                                      that have been deprecated (<strstream>)
//
//  BSL_NATIVE_C_LIB_HEADER             Headers that are part of the C
//                                      standard library (e.g., '<stdio.h>').
//                                      Note that this will *not* work for
//                                      'errno.h'
//  BSL_NATIVE_OS_RTL_HEADER            Headers that are part of the C
//                                      standard library (e.g., '<stdio.h>')
//                                      but accessed through a special platform
//                                      directory on Windows, unrelated to the
//                                      compiler version, starting with Visual
//                                      C++ 2015.  Note that this will *not*
//                                      work for 'errno.h'
//  BSL_NATIVE_OS_STDDEF_HEADER         The specific location of the <stddef.h>
//                                      header, which needs to be tracked
//                                      separately for, at least, Solaris CC
//                                      C++11 builds, in order to support
//                                      chaining intercept headers with the
//                                      platform standard library.
//..
// Note that 'BSL_NATIVE_C_LIB_HEADER' cannot be used to include the native
// version of 'errno.h' because 'errno' is '#define'd as a macro by several
// standard headers (errno.h, stdlib.h, stddef.h) - hence 'errno' cannot be
// correctly passed as an argument to the macros defined within this component.
//
///Usage
///-----
// The 'bsl+stdhdrs' package provides low-level system facilities, including a
// replacement for some of the features of the C++ standard library.  Under
// bde-stl mode, including headers '<new>', '<vector>', '<strsteam>', and
// '<cstdlib>' as follows:
//..
//  #include <new>
//  #include <vector>
//  #include <strstream>
//  #include <cstdlib>
//..
// will usually find Bloomberg's implementation of standard headers provided in
// the 'bsl+stdhdrs' package.  Sometimes, however, it is necessary to force
// inclusion of the standard headers natively supplied with the compiler.  In
// these cases, one can use the macros in this component, as follows:
//..
//  #include <bsl_bslhdrs_incpaths.h>
//
//  #include BSL_NATIVE_CPP_RUNTIME_HEADER(new)
//  #include BSL_NATIVE_CPP_LIB_HEADER(vector)
//  #include BSL_NATIVE_CPP_DEPRECATED_HEADER(strstream)
//  #include BSL_NATIVE_CPP_C_HEADER(cstring)
//  #include BSL_NATIVE_C_LIB_HEADER(setjmp.h)
//  #include BSL_NATIVE_OS_RTL_HEADER(stdio.h)
//..
// Note that the macros used for '<new>' is different from that used for
// '<vector>' because '<new>' is one of the headers that is closely related to
// the compiler.  Note also that neither the arguments to the macros nor their
// invocations are quoted or enclosed in angle brackets.

// Note that 'bslscm_version.h' is not included here because it pulls in c++
// symbols.  When someone includes a C header file (such as stdio.h) inside an
// 'extern "C"' block, the symbol inside bslscm_version (which is hidden inside
// an unnamed namespace) will be viewed as a C symbol (with the file scope
// ignored), which causes link errors when multiple files defining the version
// symbol are linked together.

extern "C++" {
// Some third party libraries #include standard C headers inside an
// 'extern "C"' block, which can cause problems when our intercept headers
// transitively include code expected to be parsed as C++.  As we cannot fix
// the third party headers, we protect the file that is ubiquitously included
// when intercepting headers.

#include <bslmf_assert.h>

#include <bsls_nativestd.h>
#include <bsls_platform.h>
} // extern "C++"

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT)
    // If a compiler supports #include_next, then the following machinery is
    // unnecessary.

#elif defined(BSLS_PLATFORM_CMP_CLANG)                                        \
   || defined(BSLS_PLATFORM_CMP_GNU)                                          \
   || defined(BSLS_PLATFORM_CMP_IBM)
    // Clang, GCC, and xlC use 'include_next' so this branch should never be
    // activated.

    BSLMF_ASSERT(false);

#elif defined(BSLS_PLATFORM_CMP_SUN)
    // For Solaris, the C headers are provided by the operating system with the
    // typical include path.  The C++ header locations vary considerably with
    // compiler version and configuration.  The intercept strategy is defined,
    // in order, according to the configured library that is being intercepted:
    //..
    // 1 For C++11, some version of the gcc standard library is in use, with a
    //   path that varies by compiler version
    // 2 If the STLport command line switch is used for C++03 builds, set up
    //   the search paths accordingly
    // 3 Otherwise, assume the pre-standard C++98 configuration using the
    //   supplied Rogue Wave headers is in effect.
    //..
    // Note that while later version of this compiler provide an #include_next
    // preprocessor directive, it does not resume searching in the expected
    // manner, and so cannot be used to implement our intercept headers.

# define BSL_NATIVE_C_LIB_HEADER(filename) <../include/filename>
    // C standard library is provided by the OS, not the compiler, so is always
    // in the same location.

# if __cplusplus >= 201103  // C++11 libary is fixed as GCC, but path changes
                            // for each new version.
#   if BSLS_PLATFORM_CMP_VERSION < 0x5140
#     define BSL_NATIVE_CPP_LIB_HEADER(filename)                              \
                                      <../../CC-gcc/include/c++/4.8.2/filename>
#   elif BSLS_PLATFORM_CMP_VERSION < 0x5150
#     define BSL_NATIVE_CPP_LIB_HEADER(filename)                              \
                                      <../../CC-gcc/include/c++/5.1.0/filename>
#   else
#     define BSL_NATIVE_CPP_LIB_HEADER(filename)                              \
                                      <../../CC-gcc/include/c++/5.4.0/filename>
#   endif

#   define BSL_NATIVE_CPP_DEPRECATED_HEADER(filename)                         \
                                   BSL_NATIVE_CPP_LIB_HEADER(backward/filename)

#   if BSLS_PLATFORM_CMP_VERSION >= 0x5140
        // gnu standard library needs to provide its own intercept of the OS
        // <stddef.h> header, in order to add 'max_align_t'.
#     define BSL_NATIVE_OS_STDDEF_HEADER(filename) <../CC/gnu/filename>
#   endif

# elif defined(BDE_BUILD_TARGET_STLPORT)
    // The command line was configured to use the STLport library for C++03
#   if BSLS_PLATFORM_CMP_VERSION < 0x5130  // Sun CC 5.5 to 5.12.3
#     define BSL_NATIVE_CPP_LIB_HEADER(filename) <../CC/stlport4/filename>
#     define BSL_NATIVE_CISO646_HEADER(filename) <../CC/std/filename>
#   else  // Sun CC 5.12.4 and later use a different directory structure
#     define BSL_NATIVE_CPP_LIB_HEADER(filename)                              \
                                           <../../include/CC/stlport4/filename>
#   endif

# else  // (default) Rogue Wave library
#   if BSLS_PLATFORM_CMP_VERSION < 0x5130  // Sun CC 5.5 to 5.12.3
#     define BSL_NATIVE_CPP_LIB_HEADER(filename)     <../CC/Cstd/filename>
#     define BSL_NATIVE_CPP_C_HEADER(filename)       <../CC/std/filename>
#     define BSL_NATIVE_CPP_RUNTIME_HEADER(filename) <../CC/filename>
#   else  // Sun CC 5.12.4 and later use a different directory structure
#     define BSL_NATIVE_CPP_LIB_HEADER(filename)                              \
                                               <../../include/CC/Cstd/filename>
#     define BSL_NATIVE_CPP_C_HEADER(filename) <../../include/CC/filename>
#     define BSL_NATIVE_CPP_RUNTIME_HEADER(filename)                          \
                                               <../../include/CC/filename>
#   endif
# endif

#elif defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION >= 1900
  // Visual C++ 2015 and later splits the C library over two directories
# define BSL_NATIVE_C_LIB_HEADER(filename)     <../include/filename>
# define BSL_NATIVE_CPP_LIB_HEADER(filename)   <../include/filename>
# define BSL_NATIVE_OS_RTL_HEADER(filename)    <../ucrt/filename>
#else
  // Most other compilers
# define BSL_NATIVE_C_LIB_HEADER(filename)     <../include/filename>
# define BSL_NATIVE_CPP_LIB_HEADER(filename)   <../include/filename>
#endif

// Several compilers require special handling for a small subset of standard
// headers, and those special cases were handled above.  Here, we set the
// default for each of those macros to otherwise resolve to the same location
// as the rest of the similarly located C or C++ library headers, using the
// default macro we have defined to establish that search path.

#if !defined(BSL_NATIVE_CPP_C_HEADER)
# define BSL_NATIVE_CPP_C_HEADER(filename) BSL_NATIVE_CPP_LIB_HEADER(filename)
#endif

#if !defined(BSL_NATIVE_CISO646_HEADER)
# define BSL_NATIVE_CISO646_HEADER(filename) BSL_NATIVE_CPP_C_HEADER(filename)
    // This file is usually with the other <cstd*> headers, so picks up their
    // custom location by default.
#endif

#if !defined(BSL_NATIVE_CPP_DEPRECATED_HEADER)
# define BSL_NATIVE_CPP_DEPRECATED_HEADER(filename)                           \
                                            BSL_NATIVE_CPP_LIB_HEADER(filename)
#endif

#if !defined(BSL_NATIVE_CPP_RUNTIME_HEADER)
# define BSL_NATIVE_CPP_RUNTIME_HEADER(filename)                              \
                                            BSL_NATIVE_CPP_LIB_HEADER(filename)
#endif

#else /* ! __cplusplus */
# define BSL_NATIVE_C_LIB_HEADER(filename)     <../include/filename>

# if defined(_MSC_VER) && _MSC_VER >= 1900
#   define BSL_NATIVE_OS_RTL_HEADER(filename)  <../ucrt/filename>
# endif

#endif /* __cplusplus */

// The intercept headers required for some platforms C libraries require
// adjustment, as listed above, regardless of whether compiling as C or C++.
// Otherwise, fall back to the standard location.

#if !defined(BSL_NATIVE_OS_RTL_HEADER)
# define BSL_NATIVE_OS_RTL_HEADER(filename)   BSL_NATIVE_C_LIB_HEADER(filename)
#endif

#if !defined(BSL_NATIVE_OS_STDDEF_HEADER)
# define BSL_NATIVE_OS_STDDEF_HEADER(filename)                                \
                                             BSL_NATIVE_OS_RTL_HEADER(filename)
    // <stddef.h> is expected to be part of the platform RTL unless overriden
    // above.  If the RTL subset of files is relocated, then <stddef.h> will
    // generally be found there, rather than with the remaining C headers.
#endif


// Confirm all of the macros are defined.

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT)

# if defined __cplusplus

# ifndef BSL_NATIVE_CPP_LIB_HEADER
#   error BSL_NATIVE_CPP_LIB_HEADER not defined: cannot find the C++ library
# endif

# ifndef BSL_NATIVE_CPP_RUNTIME_HEADER
#   error BSL_NATIVE_CPP_RUNTIME_HEADER not defined: cannot find the C++ RTL
# endif

# ifndef BSL_NATIVE_CPP_C_HEADER
#   error BSL_NATIVE_CPP_C_HEADER not defined: cannot find the C wrappers
# endif

# ifndef BSL_NATIVE_CPP_DEPRECATED_HEADER
#   error BSL_NATIVE_CPP_DEPRECATED_HEADER not defined: cannot find old headers
# endif

# endif // __cplusplus

# ifndef BSL_NATIVE_C_LIB_HEADER
#   error BSL_NATIVE_C_LIB_HEADER not defined: cannot find the C library
# endif

# ifndef BSL_NATIVE_OS_RTL_HEADER
#   error BSL_NATIVE_OS_RTL_HEADER not defined: cannot find the C RTL headers
# endif

# ifndef BSL_NATIVE_OS_STDDEF_HEADER
#   error BSL_NATIVE_OS_STDDEF_HEADER not defined: cannot find <stddef.h>
# endif

#endif // sanity checks

#endif // ! defined(INCLUDED_BSL_STDHDRS_INCPATHS)

/*
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
*/
