// btlscm_versiontag.h                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSCM_VERSIONTAG
#define INCLUDED_BTLSCM_VERSIONTAG

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide versioning information for the 'btl' package group.
//
//@SEE_ALSO: btlscm_version
//
//@DESCRIPTION: This component provides versioning information for the 'btl'
// package group.  The 'BTL_VERSION' macro that is supplied can be used for
// conditional-compilation based on 'btl' version information.  The following
// usage example illustrates this basic capability.
//
// Note that since 'btl' is always released in lock-step with 'bsl', they
// share common versioning, and the 'BTL_VERSION' will always equal the
// 'BSL_VERSION'.
//
///Usage
///-----
// At compile time, the version of BTL can be used to select an older or newer
// way to accomplish a task, to enable new functionality, or to accommodate an
// interface change.  For example, if the name of a function changes (a rare
// occurrence, but potentially disruptive when it does happen), the impact on
// affected code can be minimized by conditionally calling the function by its
// old or new name using conditional compilation.  In the following, the '#if'
// preprocessor directive compares 'BTL_VERSION' (i.e., the latest BTL version,
// excluding the patch version) to a specified major and minor version composed
// using the 'BSL_MAKE_VERSION' macro:
//..
//  #if BTL_VERSION > BSL_MAKE_VERSION(1, 3)
//      // Call 'newFunction' for BTL versions later than 1.3.
//      int result = newFunction();
//  #else
//      // Call 'oldFunction' for BTL version 1.3 or earlier.
//      int result = oldFunction();
//  #endif
//..

#ifndef INCLUDED_BSLSCM_VERSIONTAG
#include <bslscm_versiontag.h>
#endif

#define BTL_VERSION_MAJOR BSL_VERSION_MAJOR
    // BTL release major version

#define BTL_VERSION_MINOR BSL_VERSION_MINOR
    // BTL release minor version

#define BTL_VERSION BSL_MAKE_VERSION(BTL_VERSION_MAJOR, \
                                     BTL_VERSION_MINOR)
    // Construct a composite version number in the range [ 0 .. 999900 ] from
    // the specified 'BTL_VERSION_MAJOR' and 'BTL_VERSION_MINOR' numbers
    // corresponding to the major and minor version numbers, respectively, of
    // the current (latest) BTL release.  Note that the patch version number is
    // intentionally not included.  For example, 'BTL_VERSION' produces 10300
    // (decimal) for BTL version 1.3.1.

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
