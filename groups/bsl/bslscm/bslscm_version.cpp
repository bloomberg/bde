// bslscm_version.cpp                                                 -*-C++-*-
#include <bslscm_version.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslscm_patchversion.h> // BSLSCM_PATCHVERSION_PATCH

namespace BloombergLP {

#define BSL_VERSION_PATCH BSLSCM_PATCHVERSION_PATCH

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

#define BSLSCM_VERSION_STRING "BLP_LIB_BSL_BSL_" STRINGIFY(BSL_VERSION_MAJOR) \
                                             "." STRINGIFY(BSL_VERSION_MINOR) \
                                             "." STRINGIFY(BSL_VERSION_PATCH)

const char *bslscm::Version::s_ident = "$Id: " BSLSCM_VERSION_STRING " $";
const char *bslscm::Version::s_what  = "@(#)"  BSLSCM_VERSION_STRING;

const char *bslscm::Version::BSLSCM_S_VERSION    = BSLSCM_VERSION_STRING;
const char *bslscm::Version::s_dependencies      = "";
const char *bslscm::Version::s_buildInfo         = "";
const char *bslscm::Version::s_timestamp         = "";
const char *bslscm::Version::s_sourceControlInfo = "";

}  // close enterprise namespace

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
