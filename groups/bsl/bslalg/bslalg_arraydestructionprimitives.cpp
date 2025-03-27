// bslalg_arraydestructionprimitives.cpp                              -*-C++-*-
#include <bslalg_arraydestructionprimitives.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {

                  // ---------------------------------
                  // struct ArrayDestructionPrimitives
                  // ---------------------------------

// PRIVATE CLASS METHODS
void
bslalg::ArrayDestructionPrimitives::scribbleOverMemory(void   *ptr,
                                                       size_t  numBytes)
{
    // GCC 11 has a bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=101854
    // that mistakenly generates a -Wstringop-overflow warning for this code.
    // For that compiler, we replace the call to 'memset' with a loop.  This
    // bug is fixed in GCC 12.  At '-O2' and '-O3', (on GCC 11) the loop
    // generates identical code as the call to 'memset'.
#if defined(BSLS_PLATFORM_CMP_GNU) &&                                        \
           (BSLS_PLATFORM_CMP_VERSION >= 110000) &&                          \
           (BSLS_PLATFORM_CMP_VERSION < 120000)
    unsigned char *pBegin = reinterpret_cast<unsigned char *>(ptr);
    unsigned char *pEnd   = pBegin + numBytes;
    for (unsigned char *p = pBegin; p < pEnd; ++p) {
        *p = 0xa5;
    }
#else
    if (numBytes) {
        memset(ptr, 0xa5, numBytes);
    }
#endif
}

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
