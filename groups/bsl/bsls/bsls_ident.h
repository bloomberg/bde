// bsls_ident.h                                                       -*-C++-*-
#ifndef INCLUDED_BSLS_IDENT
#define INCLUDED_BSLS_IDENT

//@PURPOSE: Provide macros for inserting SCM Ids into source files.
//
//@CLASSES:
//
//@MACROS:
//  BSLS_IDENT(identifier): inset `identifier` into `.comment` section
//  BSLS_IDENT_RCSID(tag, identifier): alternatively, use add `tag` as static
//  BSLS_IDENT_PRAGMA_ONCE: encapsulate `_Pragma("once")`
//
//@SEE_ALSO: bsls_buildtarget
//
//@DESCRIPTION: The purpose of this component is to provide macros for
// inserting SCM (Source Control Management) Ids into source files.  SCM Ids
// are useful for identifying source revisions in binaries.  Additional
// information about SCM Ids may be obtained from the following man pages:
// `man ident` `man strings` (`strings -a` produces more verbose output)
// `man mcs` (Solaris-only)
//
// Note that these SCM Ids are only present if the `BSLS_IDENT_ON` macro is
// defined at compilation time.  By default, this macro is *not* defined, and
// ident strings are *not* added to object files.
//
// SCM systems may replace Ids with their expanded forms.  Note that we will
// replace the key symbol `$` with `(DOLLAR)` to avoid any expansion within
// this header file's documentation.
//
// SCM Ids usually take the form "(DOLLAR)Id: (DOLLAR)" which is expanded,
// automatically, by the source control system into an identifier which maps to
// specific source revision:
// ```
// '(DOLLAR)Id: bsls_ident.h 141104 2010-09-17 00:30:47Z mgiroux (DOLLAR)'
// ```
// This specifies that the file was checked in on 2010-09-17 at the specified
// time by user `mgiroux`, and can be retrieved from the SCM system using
// revision `141104`.
//
// `BSLS_IDENT_PRAGMA_ONCE` can optionally be used in headers and encapsulates
// a non-standard pragma (_Pragma("once")) supported on a number of platforms
// and which indicates that a header should be included and parsed only once.
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
// * `BSLS_IDENT(identifier)`
//   > This macro inserts the specified `identifier` into the object's
//   > `.comment` section, if supported on the current platform.
//
// * `BSLS_IDENT_RCSID(tag, identifier)`
//   > This macro inserts the specified `identifier` into the object, using
//   > `BSLS_IDENT`, if possible on the current platform.  If `BSLS_IDENT` is
//   > not available, the specified `tag` may be used to declare a static char
//   > array containing the `tag`.
//
// * `BSLS_IDENT_PRAGMA_ONCE`
//   > This macro encapsulates the `_Pragma("once")` functionality if available
//   > on the current platform.  If available, this functions in the same way
//   > as redundant include guards, avoiding re-opening already-included header
//   > files.
//
///Usage
///-----
// The following example shows how we first include `bsls_ident.h` and then
// use the `BSLS_IDENT` macro.  For header (`.h`) files this macro should be
// placed directly after the include guards, e.g., `bsls_somefile.h`:
// ```
//  // bsls_somefile.h                                                -*-C++-*-
//  #ifndef INCLUDED_BSLS_SOMEFILE
//  #define INCLUDED_BSLS_SOMEFILE
//
//  #include <bsls_ident.h>
//  BSLS_IDENT("(DOLLAR)Id: (DOLLAR)")  // In real usage, replace '(DOLLAR)'
//                                      // with `$`.
//
//  // ...
//
//  #endif // INCLUDED_BSLS_SOMEFILE
// ```
// For implementation (`.cpp`) files the `BSLS_IDENT` macro should be placed
// directly after the comment line for the file name and the language, and
// the include for the component header e.g., `bsls_somefile.cpp`:
// ```
//  // bsls_somefile.cpp                                              -*-C++-*-
//  #include <bsls_somefile.h>
//
//  #include <bsls_ident.h>
//  BSLS_IDENT("(DOLLAR)Id: (DOLLAR)")  // In real usage, replace '(DOLLAR)'
//                                      // with `$`.
// ```

// ============================================================================
//                          COMMAND LINE BUILD ENVIRONMENT
// ============================================================================

/// Enforce a consistent build environment for command line build flags.  Note
/// that these configuration macros are defined in `bsls_buildtarget.h` while
/// the basic consistency rules are enforced in this component as the most
/// fundamental component header necessarily included before all other BDE
/// headers.

/// This macro contributes towards enforcing the principle that components
/// include the header for components that they depend on.  As component
/// headers evolve, sometimes dependencies are removed.  When that happens
/// the `#include` for the dependency could be safely removed, but might
/// break code above that was erroneously depending on that transitive
/// `#include`.  To avoid breaking code above, we retain the `#include`
/// directive, but guard it in group that is disabled we the test
/// `#ifdef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES`.  To confirm that we have
/// a clean build that does not rely on such known transitive `#includes`,
/// recompile with the `BDE_FORCE_DONT_ALLOW_TRANSITIVE_INCLUDES` macro
/// predefined.
#undef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#ifdef BDE_FORCE_DONT_ALLOW_TRANSITIVE_INCLUDES
  #define BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#endif

/// This macro controls whether we allow features which we must continue to
/// support for our clients but do not want to rely on in our own code base.
/// Clients who want to continue using these features need take no special
/// steps.  In order to make sure an entire code base does not rely on these
/// features, recompile with the `BDE_FORCE_OMIT_INTERNAL_DEPRECATED` macro
/// predefined.  Examples of such features are: including `<stdheader>` as
/// opposed to `<bsl_stdheader.h>`, or using `DEBUG` instead of `e_DEBUG`.
#undef BDE_OMIT_INTERNAL_DEPRECATED
#ifdef BDE_FORCE_OMIT_INTERNAL_DEPRECATED
  #define BDE_OMIT_INTERNAL_DEPRECATED
  #if !defined(BDE_DONT_ALLOW_TRANSITIVE_INCLUDES)
    #define BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
    // When we don't want to rely on legacy features, we also want to make sure
    // we are not picking up macros or type aliases via (direct or transitive)
    // includes of headers that have migrated from 'bde' to 'bsl' libraries.
  #endif
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

/// Ensure that either `BSLS_IDENT_ON` or `BSLS_IDENT_OFF` is defined according
/// to the command line configuration.  Enabling `BSLS_IDENT` by default causes
/// significant bloat - see internal ticket D29644737.
#if defined(BSLS_IDENT_ON)
  #if !defined(BSLS_IDENT_OFF)
    #define BSLS_IDENT_OFF
  #endif    // `ifndef BSLS_IDENT_OFF`
#elif defined(BSLS_IDENT_OFF)
  #error Command line cannot define both `BSLS_IDENT_ON` and `BSLS_IDENT_OFF`
#endif      // `ifndef BSLS_IDENT_ON`

// ============================================================================
//                  COMPONENT-SPECIFIC IMPLEMENTATION MACROS
// ============================================================================

// multi-level indirection to force macro expansion before concatenation
// (order of concatenation/macro expansion unspecified in ISO standard)
#define BSLS_IDENT_JOIN2(x,y) x ## y
#define BSLS_IDENT_JOIN(x,y) BSLS_IDENT_JOIN2(x,y)


// ============================================================================
//                           MACRO DEFINITIONS
// ============================================================================

/// `BSLS_IDENT()` - insert string into .comment binary segment (if supported)

#if defined(__GNUC__)
  #if !defined(_AIX)
    // does not work with AIX `as`; might work with GNU `as` (gas) (not tested)
    // gcc has no pragma equivalent for `#ident`
    #define _BSLS_IDENT(str) __asm__(#str);
    #define BSLS_IDENT(str) _BSLS_IDENT(.ident str)
  #endif
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
  #define _BSLS_IDENT(str) _Pragma(#str)
  #define BSLS_IDENT(str) _BSLS_IDENT(ident str)
#elif defined(__IBMC__) || defined(__IBMCPP__)
  #define _BSLS_IDENT(str) _Pragma(#str)
  #define BSLS_IDENT(str) _BSLS_IDENT(comment (user, str))
#elif defined(_MSC_VER) // Microsoft Visual Studio Compiler
  // Microsoft linker ignores `__pragma(comment (user, "str"))`
  // http://msdn.microsoft.com/en-us/library/7f0aews7.aspx
  #if 0 // disable `SYSUTIL_IDENT()` with Microsoft compiler
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


/// `BSLS_IDENT_RCSID()` - insert ident str (specific to platform/compiler)

#define BSLS_IDENT_RCSID(tag,str) BSLS_IDENT(str)

/// `BSLS_IDENT_PRAGMA_ONCE` - macro to avoid multiple inclusion

#if defined(__GNUC__)
  #define BSLS_IDENT_PRAGMA_ONCE _Pragma("once")
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
  // Sun Studio does not support `#pragma once`.  Instead, it natively detects
  // if the entire non-comment portion of a file is wrapped by `#ifndef`, and
  // if so, optimizes away reopens of the file if the `#ifndef` condition is
  // `false`.
  #define BSLS_IDENT_PRAGMA_ONCE
#elif defined(__IBMC__) || defined(__IBMCPP__)
  #define BSLS_IDENT_PRAGMA_ONCE _Pragma("once")
#elif defined(_MSC_VER)
  #define BSLS_IDENT_PRAGMA_ONCE __pragma(once)
#elif defined(__HP_cc) || defined(__HP_aCC)
  #define BSLS_IDENT_PRAGMA_ONCE _Pragma("once")
#else
  #define BSLS_IDENT_PRAGMA_ONCE
#endif


// ============================================================================
//                      COMPLETE THE BUILD ENVIRONMENT
// ============================================================================

// Timestamp compilation with the expansion of the `BUILDID` macro, if provided
// as a command-ine argument.  Such timestamping occurs once per object file.
// Use `BSLS_IDENT()` so the timestamp occurs in .comment section only if
// supported.  Timestamping occurs even if `BSLS_IDENT` functionality is
// otherwise disabled by the `BSLS_IDENT_OFF` command line flag.
// Example usage in a Makefile target:
//   $(CC) -c -DBUILDID="$(@F) `date +%Y%m%d_%H%M%S`" -o file.o file.
#ifdef BUILDID
  #define _BSLS_IDENT_BUILDID_IMP2(str) BSLS_IDENT(#str)
  #define _BSLS_IDENT_BUILDID_IMP(str) _BSLS_IDENT_BUILDID_IMP2(str)
  _BSLS_IDENT_BUILDID_IMP($cc: BUILDID $)
  #undef _BSLS_IDENT_BUILDID_IMP
  #undef _BSLS_IDENT_BUILDID_IMP2
#endif

// Enforce the empty defintion of the `BSLS_IDENT` and `BSLS_IDENT_RCSID`
// macros when `BSLS_IDENT_OFF` is supplied on the command line.
#ifdef BSLS_IDENT_OFF
  #undef BSLS_IDENT
  #undef BSLS_IDENT_RCSID
  #define BSLS_IDENT(str)
  #define BSLS_IDENT_RCSID(tag,str)
#endif // !BSLS_IDENT_OFF

BSLS_IDENT_PRAGMA_ONCE

// Clean up implementation detail macros defined for this component.
#undef BSLS_IDENT_JOIN
#undef BSLS_IDENT_JOIN2

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
