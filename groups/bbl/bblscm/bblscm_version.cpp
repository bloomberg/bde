// bblscm_version.cpp                                                 -*-C++-*-
#include <bblscm_version.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bblscm_version_cpp,"$Id$ $CSID$")

#include <bslscm_patchversion.h> // BSLSCM_PATCHVERSION_PATCH

namespace BloombergLP {

#define BBL_VERSION_PATCH BSLSCM_PATCHVERSION_PATCH

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

#define BBLSCM_VERSION_STRING "BLP_LIB_BBL_BBL_" STRINGIFY(BBL_VERSION_MAJOR) \
                                             "." STRINGIFY(BBL_VERSION_MINOR) \
                                             "." STRINGIFY(BBL_VERSION_PATCH)

const char *bblscm::Version::s_ident = "$Id: " BBLSCM_VERSION_STRING " $";
const char *bblscm::Version::s_what  = "@(#)" BBLSCM_VERSION_STRING;

const char *bblscm::Version::BBLSCM_S_VERSION    = BBLSCM_VERSION_STRING;
const char *bblscm::Version::s_dependencies      = "";
const char *bblscm::Version::s_buildInfo         = "";
const char *bblscm::Version::s_timestamp         = "";
const char *bblscm::Version::s_sourceControlInfo = "";

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
