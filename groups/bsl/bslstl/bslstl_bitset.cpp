// bslstl_bitset.cpp                                                  -*-C++-*-
#include <bslstl_bitset.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <bslmf_assert.h>

namespace BloombergLP {

BSLMF_ASSERT(4 == sizeof(int));

}  // close enterprise namespace

namespace bsl {

                        // --------------------
                        // class Bitset_ImpUtil
                        // --------------------

void Bitset_ImpUtil::defaultInit(unsigned int  *data,
                                 size_t         size,
                                 unsigned long  val)
{
    ::memset(data, 0, size * k_BYTES_PER_INT);
    if (val == 0) {
      return;                                                         // RETURN
    }
    if (1 == k_INTS_IN_LONG) {
        data[0] = static_cast<unsigned int>(val);
    }
    else {
        const unsigned int numInts = (unsigned int) k_INTS_IN_LONG
                                                    < (unsigned int) size
                                     ? (unsigned int) k_INTS_IN_LONG
                                     : (unsigned int) size;

        for (unsigned int i = 0; i < numInts; ++i) {
            data[i] = static_cast<unsigned int>(val >> (k_BITS_PER_INT * i));
        }
    }
}

}  // close namespace bsl

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
