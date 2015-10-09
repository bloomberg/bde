// btlscm_version.cpp                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlscm_version.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlscm_version_cpp,"$Id$ $CSID$")

#include <btlscm_versiontag.h>
#include <bdlscm_versiontag.h>

#include <bslscm_patchversion.h> // BSLSCM_PATCHVERSION_PATCH

namespace BloombergLP {

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

#define REQUIRED_BDL_PATCH 0
#define BDLSCM_VERSION_STRING "BLP_LIB_BDE_BDL_" STRINGIFY(BDL_VERSION_MAJOR) \
                                             "." STRINGIFY(BDL_VERSION_MINOR) \
                                             "." STRINGIFY(REQUIRED_BDL_PATCH)

#define BTL_VERSION_PATCH BSLSCM_PATCHVERSION_PATCH
#define BTLSCM_VERSION_STRING "BLP_LIB_BDE_BTL_" STRINGIFY(BTL_VERSION_MAJOR) \
                                             "." STRINGIFY(BTL_VERSION_MINOR) \
                                             "." STRINGIFY(BTL_VERSION_PATCH)

const char *btlscm::Version::s_ident = "$Id: " BTLSCM_VERSION_STRING " $";
const char *btlscm::Version::s_what  = "@(#)" BTLSCM_VERSION_STRING;

const char *btlscm::Version::s_version           = BTLSCM_VERSION_STRING;
const char *btlscm::Version::s_dependencies      = BDLSCM_VERSION_STRING;
const char *btlscm::Version::s_buildInfo         = "";
const char *btlscm::Version::s_timestamp         = "";
const char *btlscm::Version::s_sourceControlInfo = "";

}  // close enterprise namespace

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
