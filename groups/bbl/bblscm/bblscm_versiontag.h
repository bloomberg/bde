// bblscm_versiontag.h                                                -*-C++-*-
#ifndef INCLUDED_BBLSCM_VERSIONTAG
#define INCLUDED_BBLSCM_VERSIONTAG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide versioning information for the 'bbl' package group.
//
//@SEE_ALSO: bblscm_version
//
//@DESCRIPTION: This component provides versioning information for the 'bbl'
// package group.  The 'BBL_VERSION' macro that is supplied can be used for
// conditional-compilation based on 'bbl' version information.  The following
// usage example illustrates this basic capability.
//
///Usage
///-----
// At compile time, the version of BBL can be used to select an older or newer
// way to accomplish a task, to enable new functionality, or to accommodate an
// interface change.  For example, if the name of a function changes (a rare
// occurrence, but potentially disruptive when it does happen), the impact on
// affected code can be minimized by conditionally calling the function by its
// old or new name using conditional compilation.  In the following, the '#if'
// preprocessor directive compares 'BBL_VERSION' (i.e., the latest BBL version,
// excluding the patch version) to a specified major and minor version composed
// using the 'BBL_MAKE_VERSION' macro:
//..
//  #if BBL_VERSION > BBL_MAKE_VERSION(1, 3)
//      // Call 'newFunction' for BBL versions later than 1.3.
//      int result = newFunction();
//  #else
//      // Call 'oldFunction' for BBL version 1.3 or earlier.
//      int result = oldFunction();
//  #endif
//..

#include <bslscm_version.h>
#include <bslscm_versiontag.h>


#define BBL_VERSION_MAJOR     3
    // Provide the major version number of the current (latest) BBL release.

#define BBL_VERSION_MINOR     117
    // Provide the minor version number of the current (latest) BBL release.

#define BBL_VERSION BSL_MAKE_VERSION(BBL_VERSION_MAJOR, \
                                     BBL_VERSION_MINOR)
    // Construct an integer (unique to the specified 'BBL_VERSION_MAJOR' and
    // 'BBL_VERSION_MINOR' numbers) corresponding to the major and minor
    // version numbers, respectively, of the current (latest) BBL release.
    // Note that the patch version number is intentionally not included.  The
    // exact format of the result is an implementation detail of
    // 'bslscm_versiontag'.  The major and minor parts of the resulting version
    // can be extracted with 'BSL_GET_VERSION_MAJOR' and
    // 'BSL_GET_VERSION_MINOR' respectively.

#define BBL_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(3, 1)
    // Declare that deprecations for this UOR introduced in the specified
    // version or earlier will be "active" by default -- i.e.,
    // 'BSLS_DEPRECATE_IS_ACTIVE' will be 'true' (which typically enables
    // deprecation warnings).  Note that we define this macro in the version
    // component, so that it is available at the lowest level in the component
    // hierarchy of this UOR.

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
