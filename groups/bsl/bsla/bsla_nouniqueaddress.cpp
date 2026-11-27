// bsla_nouniqueaddress.cpp                                           -*-C++-*-
#include <bsla_nouniqueaddress.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {

#if defined(BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE)
const int bsla::NoUniqueAddress_Active::s_value = 1;
#else
const int bsla::NoUniqueAddress_Inactive::s_value = 0;
#endif

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
