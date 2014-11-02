// bsls_buildtarget.cpp                                               -*-C++-*-
#include <bsls_buildtarget.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_bsltestutil.h>  // for testing only

namespace BloombergLP {

extern const int BSLS_BUILDTARGET_IS_UNIQUE = 1;

#ifdef BDE_BUILD_TARGET_EXC

const int bsls::BuildTargetExcYes::s_isBuildTargetExc = 1;

#else

const int bsls::BuildTargetExcNo::s_isBuildTargetExc = 0;

#endif

#ifdef BDE_BUILD_TARGET_MT

const int bsls::BuildTargetMtYes::s_isBuildTargetMt = 1;

#else

const int bsls::BuildTargetMtNo::s_isBuildTargetMt = 0;

#endif

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
