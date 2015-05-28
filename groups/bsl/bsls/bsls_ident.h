// bsls_ident.h                                                       -*-C++-*-
#ifndef INCLUDED_BSLS_IDENT
#define INCLUDED_BSLS_IDENT

//@PURPOSE: Provide macros for inserting SCM Ids into source files.
//
//@CLASSES:
//
//@MACROS:
//: BSLS_IDENT(identifier): inset 'identifier' into '.comment' section
//: BSLS_IDENT_RCSID(tag, identifier): alternatively, use add 'tag' as static
//: BSLS_IDENT_PRAGMA_ONCE: encapsulate '_Pragma("once")'
//
//@DESCRIPTION: The purpose of this component is to provide macros for
// inserting SCM (Source Control Management) Ids into source files.  SCM Ids
// are useful for identifying source revisions in binaries.  Additional
// information about SCM Ids may be obtained from the following man pages:
// 'man ident' 'man strings' ('strings -a' produces more verbose output)
// 'man mcs' (Solaris-only)
//
// Note that these SCM Ids are only present if the 'BSLS_IDENT_ON' macro is
// defined at compilation time.  By default, this macro is *not* defined, and
// ident strings are *not* added to object files.
//
// SCM systems may replace Ids with their expanded forms.  Note that we will
// replace the key symbol '$' with '(DOLLAR)' to avoid any expansion within
// this header file's documentation.
//
// SCM Ids usually take the form "(DOLLAR)Id: (DOLLAR)" which is expanded,
// automatically, by the source control system into an identifier which maps to
// specific source revision:
//..
//  '(DOLLAR)Id: bsls_ident.h 141104 2010-09-17 00:30:47Z mgiroux (DOLLAR)'
//..
// This specifies that the file was checked in on 2010-09-17 at the specified
// time by user 'mgiroux', and can be retrieved from the SCM system using
// revision '141104'.
//
// 'BSLS_IDENT_PRAGMA_ONCE' can optionally be used in headers and encapsulates
// a non-standard pragma (_Pragma("once")) supported on a number of platforms
// and which indicates that a header should only be included and parsed once.
// Use of this macro can help reduce compile times by eliminating extraneous
// I/O when headers are included more than once in the same translation unit.
// Note that this macro should *not* be used for any header that cannot use
// include guards: this is unusual, but can happen for certain low-level
// headers.
//
///Macro Summary
///-------------
// The following are the macros provided by this component.
//
//: 'BSLS_IDENT(identifier)'
//:   This macro inserts the specified 'identifier' into the object's
//:   '.comment' section, if supported on the current platform.
//:
//: 'BSLS_IDENT_RCSID(tag, identifier)'
//:   This macro inserts the specified 'identifier' into the object, using
//:   'BSLS_IDENT', if possible on the current platform.  If 'BSLS_IDENT' is
//:   not available, the specified 'tag' may be used to declare a static char
//:   array containing the 'tag'.
//:
//: 'BSLS_IDENT_PRAGMA_ONCE'
//:   This macro encapsulates the '_Pragma("once")' functionality if available
//:   on the current platform.  If available, this functions in the same way as
//:   redundant include guards, avoiding re-opening already-included header
//:   files.
//
///Usage
///-----
// Include 'bsls_ident.h' and use the BSLS_IDENT macro.  For header files this
// should be done directly after the include guards, e.g., bsls_somefile.h:
//..
//  // bsls_somefile.h                                                -*-C++-*-
//  #ifndef INCLUDED_BSLS_SOMEFILE
//  #define INCLUDED_BSLS_SOMEFILE
//
//  #include <bsls_ident.h>
//  BSLS_IDENT("(DOLLAR)Id: (DOLLAR)") // In real usage, replace '(DOLLAR)'
//                                     // with '$'
//
//  // ...
//
//  #endif // INCLUDED_BSLS_SOMEFILE
//..
// For cpp files it should be done directly after the comment for the file name
// and the language, e.g., bsls_somefile.cpp:
//..
//  // bsls_ident.t.cpp                                               -*-C++-*-
//
//  #include <bsls_ident.h>
//  BSLS_IDENT("(DOLLAR)Id: (DOLLAR)") // In real usage, replace '(DOLLAR)'
//                                     // with '$'
//..

/* ident string intentionally omitted for this header (do not add to binaries)
 * Its use is expected to be so extensive that the cost outweighs benefit
 * of including an ident string for every file that includes this header
 * (present here so that programs like update_rcsid do not accidentally add) */
#if 0
#define BSLS_IDENT_RCSID(tag,str)
BSLS_IDENT_RCSID(sysutil_ident_h,"$Id: $")
#endif

// Enabling BSLS_IDENT by default causes significant bloat - see internal
// ticket D29644737.
#ifndef BSLS_IDENT_ON
#ifndef BSLS_IDENT_OFF
#define BSLS_IDENT_OFF
#endif // ifndef BSLS_IDENT_OFF
#endif // ifndef BSLS_IDENT_ON


#ifdef BSLS_IDENT_OFF


#define BSLS_IDENT(str)
#define BSLS_IDENT_RCSID(tag,str)


#else /* !BSLS_IDENT_OFF */


/* BSLS_IDENT() - insert string into .comment binary segment (if supported)*/

#if defined(__GNUC__)
  #if !defined(_AIX)
    /* does not work with AIX as; might work with GNU as (gas) (not tested) */
    /* gcc has no pragma equivalent for #ident */
    #define _BSLS_IDENT(str) __asm__(#str);
    #define BSLS_IDENT(str) _BSLS_IDENT(.ident str)
  #endif
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
  /* Sun Studio CC does not support _Pragma() until Sun Studio 12 */
  #if (!defined(__cplusplus)) || (__SUNPRO_CC >= 0x590)
    #define _BSLS_IDENT(str) _Pragma(#str)
    #define BSLS_IDENT(str) _BSLS_IDENT(ident str)
  #endif
#elif defined(__IBMC__) || defined(__IBMCPP__)
  #define _BSLS_IDENT(str) _Pragma(#str)
  #define BSLS_IDENT(str) _BSLS_IDENT(comment (user, str))
#elif defined(_MSC_VER) /* Microsoft Visual Studio Compiler */
  /* Microsoft linker ignores __pragma(comment (user, "str"))
   * http://msdn.microsoft.com/en-us/library/7f0aews7.aspx */
 #if 0 /* disable SYSUTIL_IDENT() with Microsoft compiler */
  #define _BSLS_IDENT(str) __pragma(comment (user, #str))
  #define BSLS_IDENT(str) _BSLS_IDENT(str)
 #endif
#elif defined(__HP_cc) || defined(__HP_aCC)
  #define _BSLS_IDENT(str) _Pragma(#str)
  #define BSLS_IDENT(str) _BSLS_IDENT(versionid str)
#endif

#ifndef BSLS_IDENT
#define BSLS_IDENT(str)
#endif


/* multi-level indirection to force macro expansion before concatenation */
/* (order of concatenation/macro expansion unspecified in ANSI standard) */
#define BSLS_IDENT_JOIN2(x,y) x ## y
#define BSLS_IDENT_JOIN(x,y) BSLS_IDENT_JOIN2(x,y)


/* BSLS_IDENT_RCSID() - insert ident str (specific to platform/compiler) */

#if defined(__GNUC__)
  #if !defined(_AIX)
    #define BSLS_IDENT_RCSID(tag,str) BSLS_IDENT(str)
  #else /* _AIX */
    /* (XXX: look into further if we ever use gcc to build prod tasks on AIX)
     */
    #ifndef lint
    #define BSLS_IDENT_RCSID(tag,str)                                       \
      static char BSLS_IDENT_JOIN(ident_,tag)[] __attribute__((__unused__)) \
                                = str;
    #endif
  #endif
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
  #if (!defined(__cplusplus)) || (__SUNPRO_CC >= 0x590)
    #define BSLS_IDENT_RCSID(tag,str) BSLS_IDENT(str)
  #else /* Sun Studio CC does not support _Pragma() until Sun Studio 12 */
    #ifndef lint
    #define BSLS_IDENT_RCSID(tag,str) \
      static char BSLS_IDENT_JOIN(ident_,tag)[] = str;
    #endif
  #endif
#elif defined(__IBMC__) || defined(__IBMCPP__)
  #if ((defined(__IBMC__)   && __IBMC__   >= 1010) \
    || (defined(__IBMCPP__) && __IBMCPP__ >= 1010))
    #define BSLS_IDENT_RCSID(tag,str) BSLS_IDENT(str)
  #else
    /* Early versions of IBM xlc did not preserve .comment in binary
     * executables or pre-linked libraries.  Fixed by IBM in following
     * releases:
     * xlC v8.0 in May 2008 PTF (8.0.0.19) with -qxflag=new_pragma_comment_user
     * xlC v9.0 in July 2008 PTF (9.0.0.5)
     * xlC v10.1 GA
     * Enabled above only for xlC 10.1 or later, which can be detected reliably
     * (Use C compiler and printf("%s\n", __xlc__) to see x.x.x.x version,
     *  but strings are not comparable in macros)
     * Note that using updated linker (bind64) (circa late 2010) is also needed
     * for the ident strings to be present in pre-linked libs and executables.
     */
    /* various ways to create C string, including const, volatile, more */
    /* static char instead of static const char with xlC -qnoro -qnoroconst */
    /* xlC might end up optimizing this away, anyway */
    #ifndef lint
    #define BSLS_IDENT_RCSID(tag,str) \
      static char BSLS_IDENT_JOIN(ident_,tag)[] = str;
    #endif
  #endif
#elif defined(_MSC_VER)
  #define BSLS_IDENT_RCSID(tag,str) BSLS_IDENT(str)
#elif defined(__HP_cc) || defined(__HP_aCC)
  #define BSLS_IDENT_RCSID(tag,str) BSLS_IDENT(str)
#else
  /* others: add conditions above for compilers able to support BSLS_IDENT */
  #ifndef lint
  #define BSLS_IDENT_RCSID(tag,str) \
    static char BSLS_IDENT_JOIN(ident_,tag)[] = str;
  #endif
#endif

#ifndef BSLS_IDENT_RCSID
#define BSLS_IDENT_RCSID(tag,str)
#endif


/* timestamp compilation with BUILDID, if provided.  (occurs once per object)
 * Use BSLS_IDENT() so only occurs in .comment section if supported.
 * example usage in a Makefile target:
 *   $(CC) -c -DBUILDID="$(@F) `date +%Y%m%d_%H%M%S`" -o file.o file.c
 */
#ifdef BUILDID
  #define _BSLS_IDENT_BUILDID_IMP2(str) BSLS_IDENT(#str)
  #define _BSLS_IDENT_BUILDID_IMP(str) _BSLS_IDENT_BUILDID_IMP2(str)
  _BSLS_IDENT_BUILDID_IMP($cc: BUILDID $)
  #undef _BSLS_IDENT_BUILDID_IMP
  #undef _BSLS_IDENT_BUILDID_IMP2
#endif


#endif /* !BSLS_IDENT_OFF */


#if defined(__GNUC__)
  #define BSLS_IDENT_PRAGMA_ONCE _Pragma("once")
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
  /* Sun Studio does not support #pragma once.  Instead, it natively detects if
   * the entire non-comment portion of a file is wrapped by #ifndef, and if so,
   * optimizes away reopens of the file if the #ifndef condition is false */
  #define BSLS_IDENT_PRAGMA_ONCE
#elif defined(__IBMC__) || defined(__IBMCPP__)
  #define BSLS_IDENT_PRAGMA_ONCE _Pragma("once")
#elif defined(_MSC_VER)
  #define BSLS_IDENT_PRAGMA_ONCE __pragma(once)
#elif (defined(__HP_cc)  && __HP_cc-0  >= 62500) \
   || (defined(__HP_aCC) && __HP_aCC-0 >= 62500)
  /* supported in aCC A.06.25 (had not been fully supported in aCC A.06.20) */
  #define BSLS_IDENT_PRAGMA_ONCE _Pragma("once")
#else
  #define BSLS_IDENT_PRAGMA_ONCE
#endif
BSLS_IDENT_PRAGMA_ONCE


/* Technical notes:
 *
 * There is a cost to including ident extra information in the binary objects,
 * which may include disk space usage and runtime memory usage, as well as
 * side effects such as locality of strings in memory at runtime.
 *
 * Some vendors provide a mechanism to include ident strings in a non-loadable
 * .comment section of the binary so that strings are available for review in
 * the on-disk binary, but are not loaded into memory at runtime.
 * (Alternatively, this separate non-loadable section might selectively be
 * stripped from the binary, saved in a database with the checksum of the
 * stripped binary, and then the resulting smaller binary moved to production.)
 *
 * bsls_ident.h encapsulates ident mechanisms so that includers of this
 * header need not be concerned with the mechanism applied.  The mechanism may
 * be changed as better methods become available and the includer can obtain
 * the changes simply by recompiling (without needing to modify all other
 * source code).
 *
 * 'ident' shows only ident-style strings.  'man ident' for more info.
 * The tokens passed to the pragmas are not necessarily ident-style.
 * They are still visible with 'strings -a'.
 *
 * IBM xlC supports #pragma comment
 * IBM xlC warns about #pragma ident and errors for #ident
 * Sun CC supports #pragma ident and #ident
 * Sun CC silently ignores #pragma comment
 * GCC supports #ident
 * GCC warns about unrecognized #pragma ident and #pragma comment
 * While these can all be worked around by disabling specific warnings/errors,
 * doing so might mask other warnings/errors.  This header allows for
 * encapsulation of all the conditional logic to use the correct pragma with
 * each compiler without having to duplicate these conditions in every source
 * file.
 *
 * #ident, #pragma ident, #pragma comment each can take a -single- user-defined
 * token (no concatenation of string constants or preprocessor macro ##
 * concatenation)  (MS Visual Studio will do string concatenation)
 *
 * C99 _Pragma() can expanded in macros and can be used in place of #pragma.
 * GCC does not implement a #pragma version of gcc #ident preprocessor
 * directive
 *
 * AIX 'strip' removes strings inserted by #pragma comment
 * Solaris mcs -d removes strings inserted by #ident and #pragma ident
 *   (equivalent to compiling with cc -mr)
 *   mcs -c uniquifies the strings (equivalent to compiling with cc -mc)
 *   ('strip' does not remove these strings)
 * GNU strip: 'strip -R comment' will remove .comment section
 *
 * AIX multiple #pragma comment end up concatenated on a single line, allowing
 * for pasting together of individual tokens.  'ident' prints each ident-style
 * string on its own line.  Solaris #ident places each token on its own line.
 *
 * Sun Studio CC (C++ compiler) < Sun Studio 12 CC do not support _Pragma()
 * Sun Studio 12 CC supports _Pragma() but clips the first character of string
 *   (This leads to strings being present in .comment section, minus the first
 *    character, and then not showing up when 'ident' is run, but present with
 *    strings -a) (Bug filed with Sun and has been fixed in latest Studio 12).
 *
 * HP-UX supports _Pragma() only in ANSI (-Aa) and ANSI extended (-Ae) mode.
 */

#endif // INCLUDED_BSLS_IDENT

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
