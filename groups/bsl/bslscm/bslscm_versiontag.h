// bslscm_versiontag.h                                                -*-C++-*-
#ifndef INCLUDED_BSLSCM_VERSIONTAG
#define INCLUDED_BSLSCM_VERSIONTAG

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif

BSLS_IDENT("$Id: $")

//@PURPOSE: Provide versioning information for the 'bsl' package group.
//
//@CLASSES:
//
//@MACROS:
//  BSL_VERSION_MAJOR: current release major version number
//  BSL_VERSION_MINOR: current release minor version number
//  BSL_MAKE_VERSION(MA, MI): create combined, 6-digit version number (MAMI00)
//  BSL_VERSION: combined, 6-digit version number for current release
//
//@SEE_ALSO: bslscm_version
//
//@DESCRIPTION: This component provides versioning information for the 'bsl'
// package group.  The 'BSL_VERSION' and 'BSL_MAKE_VERSION' macros that are
// supplied can be used for conditional-compilation based on 'bsl' version
// information.  The following usage example illustrates this basic capability.
//
///Usage
///-----
// At compile time, the version of BSL can be used to select an older or newer
// way to accomplish a task, to enable new functionality, or to accommodate an
// interface change.  For example, if the name of a function changes (a rare
// occurrence, but potentially disruptive when it does happen), the impact on
// affected code can be minimized by conditionally calling the function by its
// old or new name using conditional compilation.  In the following, the '#if'
// preprocessor directive compares 'BSL_VERSION' (i.e., the latest BSL version,
// excluding the patch version) to a specified major and minor version composed
// using the 'BSL_MAKE_VERSION' macro:
//..
//  #if BSL_VERSION > BSL_MAKE_VERSION(1, 3)
//      // Call 'newFunction' for BSL versions later than 1.3.
//      int result = newFunction();
//  #else
//      // Call 'oldFunction' for BSL version 1.3 or earlier.
//      int result = oldFunction();
//  #endif
//..

#define BSL_VERSION_MAJOR    3
    // Provide the major version number of the current (latest) BSL release.

#define BSL_VERSION_MINOR    0
    // Provide the minor version number of the current (latest) BSL release.

#define BSL_MAKE_VERSION(major, minor) ((major) * 10000 \
                                      + (minor) *   100)
    // Construct a composite version number in the range '[0 .. 999900]' from
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
    // and 'minor' are integral values in the range '[0 .. 99]'.

#define BSL_VERSION BSL_MAKE_VERSION(BSL_VERSION_MAJOR, \
                                     BSL_VERSION_MINOR)
    // Construct a composite version number in the range '[0 .. 999900]' from
    // the specified 'BSL_VERSION_MAJOR' and 'BSL_VERSION_MINOR' numbers
    // corresponding to the major and minor version numbers, respectively, of
    // the current (latest) BSL release.  Note that the patch version number is
    // intentionally not included.  For example, 'BSL_VERSION' produces 10300
    // (decimal) for BSL version 1.3.1.

#endif

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
