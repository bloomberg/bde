// bslh_hashthreadid.cpp                                              -*-C++-*-
#include <bslh_hashthreadid.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslh_defaulthashalgorithm.h>  // for testing only

namespace BloombergLP {
namespace bslh {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
std::size_t HashThreadId_ImpUtil::getHashInput(const std::thread::id& tid)
{
    // We must double-hash `tid` because it's an opaque type and we can't
    // guarantee that it doesn't contain padding.
    return std::hash<std::thread::id>()(tid);
}
#endif  // C++11
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
