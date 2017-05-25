// balscm_version.cpp                                                 -*-C++-*-
#include <balscm_version.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balscm_version_cpp,"$Id$ $CSID$")

#include <bslscm_patchversion.h> // BSLSCM_PATCHVERSION_PATCH

namespace BloombergLP {

// 'bal' is maintained in sync with 'bsl', so they share a patch version.

#define BAL_VERSION_PATCH BSLSCM_PATCHVERSION_PATCH

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

#define BALSCM_VERSION_STRING "BLP_LIB_BDE_BAL_" STRINGIFY(BAL_VERSION_MAJOR) \
                                             "." STRINGIFY(BAL_VERSION_MINOR) \
                                             "." STRINGIFY(BAL_VERSION_PATCH)

const char *balscm::Version::s_ident = "$Id: " BALSCM_VERSION_STRING " $";
const char *balscm::Version::s_what  = "@(#)"  BALSCM_VERSION_STRING;

const char *balscm::Version::BALSCM_S_VERSION    = BALSCM_VERSION_STRING;
const char *balscm::Version::s_dependencies      = "";
const char *balscm::Version::s_buildInfo         = "";
const char *balscm::Version::s_timestamp         = "";
const char *balscm::Version::s_sourceControlInfo = "";

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
