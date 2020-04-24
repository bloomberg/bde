// bosscm_versiontag.h                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BOSSCM_VERSIONTAG
#define INCLUDED_BOSSCM_VERSIONTAG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide versioning information for the 'bos' package group.
//
//@SEE_ALSO: bosscm_version
//
//@DESCRIPTION: This component provides versioning information for the 'bos'
// package group.  The 'BOS_VERSION' macro that is supplied can be used for
// conditional-compilation based on 'bos' version information.  The following
// usage example illustrates this basic capability.
//
// Note that since 'bae' is always released in lock-step with 'bde', they
// share common versioning, and the 'BOS_VERSION' will always equal the
// 'BSL_VERSION'.
//
///Usage
///-----
// At compile time, the version of BOS can be used to select an older or newer
// way to accomplish a task, to enable new functionality, or to accommodate an
// interface change.  For example, if the name of a function changes (a rare
// occurrence, but potentially disruptive when it does happen), the impact on
// affected code can be minimized by conditionally calling the function by its
// old or new name using conditional compilation.  In the following, the '#if'
// preprocessor directive compares 'BOS_VERSION' (i.e., the latest BOS version,
// excluding the patch version) to a specified major and minor version composed
// using the 'BSL_MAKE_VERSION' macro:
//..
//  #if BOS_VERSION > BSL_MAKE_VERSION(1, 3)
//      // Call 'newFunction' for BOS versions later than 1.3.
//      int result = newFunction();
//  #else
//      // Call 'oldFunction' for BOS version 1.3 or earlier.
//      int result = oldFunction();
//  #endif
//..

#include <bslscm_versiontag.h>

#define BOS_VERSION_MAJOR 3
    // BOS release major version

#define BOS_VERSION_MINOR 53
    // BOS release major version

#define BOS_VERSION BSL_MAKE_VERSION(BOS_VERSION_MAJOR, \
                                     BOS_VERSION_MINOR)
    // Construct a composite version number in the range [ 0 .. 999900 ] from
    // the specified 'BOS_VERSION_MAJOR' and 'BOS_VERSION_MINOR' numbers
    // corresponding to the major and minor version numbers, respectively, of
    // the current (latest) BOS release.  Note that the patch version number is
    // intentionally not included.  For example, 'BOS_VERSION' produces 10300
    // (decimal) for BOS version 1.3.1.

#define BOS_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(3, 1)
    // Declare that deprecations for this UOR introduced in the specified
    // version or earlier will be "active" by default -- i.e.,
    // 'BSLS_DEPRECATE_IS_ACTIVE' will be 'true' (which typically enables
    // deprecation warnings).  Note that we define this macro in the version
    // component, so that it is available at the lowest level in the component
    // hierarchy of this UOR.

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
