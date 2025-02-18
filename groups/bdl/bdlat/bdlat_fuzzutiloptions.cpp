// bdlat_fuzzutiloptions.cpp                                          -*-C++-*-
#include <bdlat_fuzzutiloptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlat_fuzzutiloptions_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace bdlat {

                            // ---------------------
                            // class FuzzUtilOptions
                            // ---------------------

// CREATORS
FuzzUtilOptions::FuzzUtilOptions()
: d_maxArrayLength(k_MAX_ARRAY_LENGTH_DEFAULT)
, d_maxStringLength(k_MAX_STRING_LENGTH_DEFAULT)
{
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
