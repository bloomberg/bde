// bdescm_versiontag.h                                                -*-C++-*-
#ifndef INCLUDED_BDESCM_VERSIONTAG
#define INCLUDED_BDESCM_VERSIONTAG

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif

BDES_IDENT("$Id: $")

//@PURPOSE: Provide versioning information for the 'bde' package group.
//
//@SEE_ALSO: bdescm_version
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides versioning information for the 'bde'
// package group.  The 'BDE_VERSION' and 'BDE_MAKE_VERSION' macros that are
// supplied can be used for conditional-compilation based on 'bde' version
// information.  The following usage example illustrates this basic capability.
//
///Usage
///-----
// At compile time, the version of BDE can be used to select an older or newer
// way to accomplish a task, to enable new functionality, or to accommodate an
// interface change.  For example, if the name of a function changes (a rare
// occurrence, but potentially disruptive when it does happen), the impact on
// affected code can be minimized by conditionally calling the function by its
// old or new name using conditional compilation.  In the following, the '#if'
// preprocessor directive compares 'BDE_VERSION' (i.e., the latest BDE version,
// excluding the patch version) to a specified major and minor version composed
// using the 'BDE_MAKE_VERSION' macro:
//..
//  #if BDE_VERSION > BDE_MAKE_VERSION(1, 3)
//      // Call 'newFunction' for BDE versions later than 1.3.
//      int result = newFunction();
//  #else
//      // Call 'oldFunction' for BDE version 1.3 or earlier.
//      int result = oldFunction();
//  #endif
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_BUILDTARGET
#include <bsls_buildtarget.h>      // need to ensure consistent build options
#endif

#define BDE_VERSION_MAJOR     2
    // Provide the major version number of the current (latest) BDE release.

#define BDE_VERSION_MINOR     11
    // Provide the minor version number of the current (latest) BDE release.

#ifndef BDE_VERSION_RELEASETYPE
#define BDE_VERSION_RELEASETYPE        "_GIT_MASTER"
#define BDE_VERSION_RELEASETYPE_SYMBOL  _GIT_MASTER
#endif
    // Provide the "type" of the release, indicating whether the libs were
    // built for development, a particular beta, or for release.  A release
    // build is indicated by _PROD and "_PROD".

#define BDE_MAKE_VERSION(major, minor) ((major) * 10000 \
                                      + (minor) *   100)
    // Construct a composite version number in the range '[ 0 .. 999900 ]' from
    // the specified 'major' and 'minor' version numbers.  The resulting value,
    // when expressed as a 6-digit decimal string, has "00" as the two
    // lowest-order decimal digits, 'minor' as the next two digits, and 'major'
    // as the highest-order digits.  The result is unique for each combination
    // of 'major' and 'minor', and is sortable such that a value composed from
    // a given 'major' version number will compare larger than a value composed
    // from a smaller 'major' version number (and similarly for 'minor' version
    // numbers).  Note that if 'major' and 'minor' are both compile-time
    // integral constants, then the resulting expression is also a compile-time
    // integral constant.  Also note that the patch version number is
    // intentionally not included.  The behavior is undefined unless 'major'
    // and 'minor' are integral values in the range '[ 0 .. 99 ]'.

#define BDE_VERSION BDE_MAKE_VERSION(BDE_VERSION_MAJOR, \
                                     BDE_VERSION_MINOR)
    // Construct a composite version number in the range '[ 0 .. 999900 ]' from
    // the specified 'BDE_VERSION_MAJOR' and 'BDE_VERSION_MINOR' numbers
    // corresponding to the major and minor version numbers, respectively, of
    // the current (latest) BDE release.  Note that the patch version number is
    // intentionally not included.  For example, 'BDE_VERSION' produces 10300
    // (decimal) for BDE version 1.3.1.

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
