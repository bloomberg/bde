// bdlt_delegatingdateimputil.cpp                                     -*-C++-*-
#include <bdlt_delegatingdateimputil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_delegatingdateimputil_cpp,"$Id$ $CSID$")

#include <bsl_cstdio.h>    // 'fprintf'

namespace BloombergLP {
namespace bdlt {

                           // ----------------------------
                           // struct DelegatingDateImpUtil
                           // ----------------------------

// CLASS DATA
bool DelegatingDateImpUtil::s_prolepticGregorianModeFlag = false;

                        // Calendar Mode

// CLASS METHODS
void DelegatingDateImpUtil::disableProlepticGregorianMode()
{
    s_prolepticGregorianModeFlag = false;

    bsl::fprintf(stderr,
              "bdlt::DelegatingDateImpUtil: BDE calendar mode set to POSIX\n");
}

void DelegatingDateImpUtil::enableProlepticGregorianMode()
{
    s_prolepticGregorianModeFlag = true;

    bsl::fprintf(stderr,
"bdlt::DelegatingDateImpUtil: BDE calendar mode set to proleptic Gregorian\n");
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
