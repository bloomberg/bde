// bslstl_typeindex.cpp                                               -*-C++-*-
#include <bslstl_typeindex.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslstl_function.h>            // for testing only
#include <bslstl_sharedptr.h>           // for testing only
#include <bslstl_unorderedmap.h>        // for testing only

#include <bslh_spookyhashalgorithm.h>

#include <bsls_libraryfeatures.h>

#include <string.h>

namespace bsl {

                        // ----------------
                        // class type_index
                        // ----------------

size_t type_index::hash_code() const BSLS_KEYWORD_NOEXCEPT
{
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
    return d_targetType_p->hash_code();
#else
    const char *contents = d_targetType_p->name();

    BloombergLP::bslh::SpookyHashAlgorithm hasher;
    hasher(contents, strlen(contents));
    return static_cast<size_t>(hasher.computeHash());
#endif
}

}  // close namespace bsl

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
