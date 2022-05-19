// bosscm_version.cpp                                                 -*-C++-*-
#include <bosscm_version.h>

#include <bsls_ident.h>

BSLS_IDENT_RCSID(bosscm_version_cpp,"$Id$ $CSID$")

namespace BloombergLP {

#define BOS_VERSION_PATCH 0

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

#define BOSSCM_VERSION_STRING "BLP_LIB_BDE_BOS_" STRINGIFY(BOS_VERSION_MAJOR) \
                                             "." STRINGIFY(BOS_VERSION_MINOR) \
                                             "." STRINGIFY(BOS_VERSION_PATCH)

const char *bosscm::Version::s_ident = "$Id: " BOSSCM_VERSION_STRING " $";
const char *bosscm::Version::s_what  = "@(#)"  BOSSCM_VERSION_STRING;

const char *bosscm::Version::BOSSCM_S_VERSION    = BOSSCM_VERSION_STRING;
const char *bosscm::Version::s_dependencies      = "";
const char *bosscm::Version::s_buildInfo         = "";
const char *bosscm::Version::s_timestamp         = "";
const char *bosscm::Version::s_sourceControlInfo = "";

}  // close enterprise namespace

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
