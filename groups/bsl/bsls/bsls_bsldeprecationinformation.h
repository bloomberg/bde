// bsls_bsldeprecationinformation.h                                   -*-C++-*-
#ifndef INCLUDED_BSLS_BSLDEPRECATIONINFORMATION
#define INCLUDED_BSLS_BSLDEPRECATIONINFORMATION

//@PURPOSE: Provide a macro to control 'bsl' deprecations by release version.
//
//@MACROS:
//  BSL_VERSION_DEPRECATION_THRESHOLD: activate deprecations by 'bsl' version
//
//@SEE_ALSO: bsls_deprecate
//
//@DESCRIPTION: This component provides a macro,
// 'BSL_VERSION_DEPRECATION_THRESHOLD', that is used to control whether or not
// deprecations introduced in any given version of 'bsl' are active.
//
///NOT A MODEL; DO NOT EMULATE
///---------------------------
// This component exists only because the structure of 'bsl' is *NON-STANDARD*,
// which prevents us from defining the deprecation threshold macro in 'bslscm',
// where it normally would belong.  Other UORs should *NOT* use this component
// as a model.  Please see 'bsls_deprecate' and the version tag components in
// other, standard-conforming, BDE UORs such as 'bdlscm_versiontag'.
//
///Usage
///-----
// There is no usage example for this component since it is not meant for
// direct client use.

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -SLM01

#define BSL_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(3, 1)
    // Declare that deprecations for this UOR introduced in the specified
    // version or earlier will be "active" by default -- i.e.,
    // 'BSLS_DEPRECATE_IS_ACTIVE' will be 'true' (which typically enables
    // deprecation warnings).  Note that we define this macro in this special
    // component, so that it is available at the lowest level in the component
    // hierarchy of this UOR.

// BDE_VERIFY pragma: pop

#endif  // INCLUDED_BSLS_BSLDEPRECATIONINFORMATION

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
