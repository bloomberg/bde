// bslstl_sharedptr.cpp                                               -*-C++-*-
#include <bslstl_sharedptr.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslstl_badweakptr.h>
#include <bslstl_deque.h>              // for testing only
#include <bslstl_list.h>               // for testing only
#include <bslstl_map.h>                // for testing only
#include <bslstl_string.h>             // for testing only
#include <bslstl_vector.h>             // for testing only

#include <bsls_alignmentutil.h>
#include <bsls_exceptionutil.h>

namespace BloombergLP {
namespace bslstl {

                             // -------------------
                             // class SharedPtrUtil
                             // -------------------

// MANIPULATORS
bsl::shared_ptr<char>
SharedPtrUtil::createInplaceUninitializedBuffer(
                                              size_t            bufferSize,
                                              bslma::Allocator *basicAllocator)
{
    basicAllocator = bslma::Default::allocator(basicAllocator);
                                                       // allocator is optional

    // We have alignment concerns here: there are no alignment issues with
    // 'bslstl::SharedPtrRep', but the buffer address (i.e., the address of
    // 'd_instance' in the 'bslstl::SharedPtrInplaceRep' object) must be at
    // least *naturally* *aligned* to 'bufferSize'.  See the 'bslma' package-
    // level documentation for a definition of natural alignment.)  We achieve
    // this in the simplest way by always maximally aligning the returned
    // pointer.

    typedef bslma::SharedPtrInplaceRep<bsls::AlignmentUtil::MaxAlignedType>
                                                                           Rep;

    enum {
        k_ALIGNMENT_MASK = ~(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1)
    };

    size_t repSize = (sizeof(Rep) + bufferSize - 1) & k_ALIGNMENT_MASK;

    Rep *rep = new (basicAllocator->allocate(repSize)) Rep(basicAllocator);

    return bsl::shared_ptr<char>(reinterpret_cast<char *>(rep->ptr()), rep);
}

void SharedPtr_ImpUtil::throwBadWeakPtr()
{
    BSLS_THROW(bsl::bad_weak_ptr());
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
