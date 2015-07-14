// balscm_versiontag.h -*-C++-*-
#ifndef INCLUDED_BALSCM_VERSIONTAG
#define INCLUDED_BALSCM_VERSIONTAG

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")



//@PURPOSE: Provide versioning information for the 'bae' package group.
//
//@SEE_ALSO: balscm_version
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides versioning information for the 'bae'
// package group.  The 'BAL_VERSION' macro that is supplied can be used for
// conditional-compilation based on 'bae' version information.  The following
// usage example illustrates this basic capability.
//
// Note that since 'bae' is always released in lock-step with 'bde', they
// share common versioning, and the 'BAL_VERSION' will always equal the
// 'BSL_VERSION'.
//
///Usage
///-----
// At compile time, the version of BAE can be used to select an older or newer
// way to accomplish a task, to enable new functionality, or to accommodate an
// interface change.  For example, if the name of a function changes (a rare
// occurrence, but potentially disruptive when it does happen), the impact on
// affected code can be minimized by conditionally calling the function by its
// old or new name using conditional compilation.  In the following, the '#if'
// preprocessor directive compares 'BAL_VERSION' (i.e., the latest BAE version,
// excluding the patch version) to a specified major and minor version composed
// using the 'BSL_MAKE_VERSION' macro:
//..
//  #if BAL_VERSION > BSL_MAKE_VERSION(1, 3)
//      // Call 'newFunction' for BAE versions later than 1.3.
//      int result = newFunction();
//  #else
//      // Call 'oldFunction' for BAE version 1.3 or earlier.
//      int result = oldFunction();
//  #endif
//..

#ifndef INCLUDED_BSLSCM_VERSIONTAG
#include <bslscm_versiontag.h>
#endif

#define BAL_VERSION_MAJOR BSL_VERSION_MAJOR
    // BAE release major version

#define BAL_VERSION_MINOR BSL_VERSION_MINOR
    // BAE release major version

#define BAL_VERSION BSL_MAKE_VERSION(BAL_VERSION_MAJOR, \
                                     BAL_VERSION_MINOR)
    // Construct a composite version number in the range [ 0 .. 999900 ] from
    // the specified 'BAL_VERSION_MAJOR' and 'BAL_VERSION_MINOR' numbers
    // corresponding to the major and minor version numbers, respectively, of
    // the current (latest) BAE release.  Note that the patch version number is
    // intentionally not included.  For example, 'BAL_VERSION' produces 10300
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
