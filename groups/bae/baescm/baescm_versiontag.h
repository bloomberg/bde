// baescm_versiontag.h -*-C++-*-
#ifndef INCLUDED_BAESCM_VERSIONTAG
#define INCLUDED_BAESCM_VERSIONTAG

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")



//@PURPOSE: Provide versioning information for the 'bae' package group.
//
//@SEE_ALSO: baescm_version
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides versioning information for the 'bae'
// package group.  The 'BAE_VERSION' macro that is supplied can be used for
// conditional-compilation based on 'bae' version information.  The following
// usage example illustrates this basic capability.
//
///Usage
///-----
// At compile time, the version of BAE can be used to select an older or newer
// way to accomplish a task, to enable new functionality, or to accommodate an
// interface change.  For example, if the name of a function changes (a rare
// occurrence, but potentially disruptive when it does happen), the impact on
// affected code can be minimized by conditionally calling the function by its
// old or new name using conditional compilation.  In the following, the '#if'
// preprocessor directive compares 'BAE_VERSION' (i.e., the latest BAE version,
// excluding the patch version) to a specified major and minor version composed
// using the 'BDE_MAKE_VERSION' macro:
//..
//  #if BAE_VERSION > BDE_MAKE_VERSION(1, 3)
//      // Call 'newFunction' for BAE versions later than 1.3.
//      int result = newFunction();
//  #else
//      // Call 'oldFunction' for BAE version 1.3 or earlier.
//      int result = oldFunction();
//  #endif
//..

#ifndef INCLUDED_BDESCM_VERSIONTAG
#include <bdescm_versiontag.h>
#endif

#define BAE_VERSION_MAJOR 2
    // BAE release major version

#define BAE_VERSION_MINOR 9
    // BAE release major version

#define BAE_VERSION BDE_MAKE_VERSION(BAE_VERSION_MAJOR, \
                                     BAE_VERSION_MINOR)
    // Construct a composite version number in the range [ 0 .. 999900 ] from
    // the specified 'BAE_VERSION_MAJOR' and 'BAE_VERSION_MINOR' numbers
    // corresponding to the major and minor version numbers, respectively, of
    // the current (latest) BAE release.  Note that the patch version number is
    // intentionally not included.  For example, 'BAE_VERSION' produces 10300
    // (decimal) for BAE version 1.3.1.

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
