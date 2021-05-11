// bdlb_caselessstringviewhash.cpp                                    -*-C++-*-

#include <bdlb_caselessstringviewhash.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_caselessstringviewhash_cpp,"$Id$ $CSID$")

#include <bdlb_chartype.h>

#include <bslh_spookyhashalgorithm.h>

#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>

namespace BloombergLP {
namespace bdlb {

                       // -----------------------------
                       // struct CaselessStringViewHash
                       // -----------------------------

// ACCESSORS
bsl::size_t CaselessStringViewHash::operator()(bsl::string_view argument) const
{
    char buffer[1024];

    bslh::SpookyHashAlgorithm hash;

    const bsl::size_t len = argument.length();

    for (bsl::size_t ii = 0, numBytesToCopy; ii < len; ii += numBytesToCopy) {
        numBytesToCopy = bsl::min(sizeof(buffer), len - ii);

        bsl::transform(argument.data() + ii,
                       argument.data() + ii + numBytesToCopy,
                       buffer,
                       &bdlb::CharType::toLower);

        hash(buffer, numBytesToCopy);
    }

#ifdef BSLS_PLATFORM_CPU_64_BIT
    return hash.computeHash();
#else
    const bsls::Types::Uint64 ret64 = hash.computeHash();
    return static_cast<bsl::size_t>(ret64 ^ (ret64 >> 32));
#endif
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
