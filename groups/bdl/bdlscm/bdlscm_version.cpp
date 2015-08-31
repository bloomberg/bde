// bdlscm_version.cpp                                                 -*-C++-*-
#include <bdlscm_version.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlscm_version_cpp,"$Id$ $CSID$")

#include <bslscm_patchversion.h> // BSLSCM_PATCHVERSION_PATCH

#include <bslscm_versiontag.h>

namespace BloombergLP {

// 'bdl' is maintained in sync with 'bsl', so they share a patch version.

#define BDL_VERSION_PATCH BSLSCM_PATCHVERSION_PATCH

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

// Required patch version for dependency on 'bsl' is independent of whether or
// not 'bdl' versioning tracks 'bsl' versioning.

#define BSLSCM_PATCH_REQUIRED 0
#define BSLSCM_VERSION_STRING "BLP_LIB_BDE_BSL_" STRINGIFY(BSL_VERSION_MAJOR) \
                                           "." STRINGIFY(BSL_VERSION_MINOR)   \
                                           "." STRINGIFY(BSLSCM_PATCH_REQUIRED)

#define BDLSCM_VERSION_STRING "BLP_LIB_BDE_BDL_" STRINGIFY(BDL_VERSION_MAJOR) \
                                             "." STRINGIFY(BDL_VERSION_MINOR) \
                                             "." STRINGIFY(BDL_VERSION_PATCH)

const char *bdlscm::Version::s_ident = "$Id: " BDLSCM_VERSION_STRING " $";
const char *bdlscm::Version::s_what  = "@(#)" BDLSCM_VERSION_STRING;

const char *bdlscm::Version::BDLSCM_S_VERSION    = BDLSCM_VERSION_STRING;
const char *bdlscm::Version::s_dependencies      = BSLSCM_VERSION_STRING;
const char *bdlscm::Version::s_buildInfo         = "";
const char *bdlscm::Version::s_timestamp         = "";
const char *bdlscm::Version::s_sourceControlInfo = "";

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2012 Bloomberg Finance L.P.
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
