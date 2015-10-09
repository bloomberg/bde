// bdlcc_objectpool.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlcc_objectpool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlcc_objectpool_cpp,"$Id$ $CSID$")

#include <bdlcc_fixedqueue.h>     // for testing only
#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bsl_functional.h>

///IMPLEMENTATION NOTES
///--------------------
// The bdlcc::ObjectPool algorithm is mostly lock-free, except for operations
// that may grow the pool; these are serialized by d_mutex.  The remaining
// operations achieve lock-free performance by relying on an atomic reference
// count.  Threads seeking to acquire an object from the pool atomically
// increment the count of the object at the head of the free list; the one that
// incremented from 0 goes on to use the object, while the others wait a few
// cycles for the free list to change, decrement the count, and try again.
// Reference counts are normally incremented and decremented by 2, because the
// lowest-order bit of the count is reserved to indicate an object that is
// available for reuse but has yet to be added back to the free list.
//
// The reference count is thus unguarded and is crucial to the whole algorithm.
// Any change in the logic for using the reference count must be made with
// extreme caution and only after consulting with threading experts in our
// group (at this writing, Gino Rocha or Vlad Kliatchko).
//
// This picture describes a memory chunk returned by 'd_blockAllocator':
//
// padding-1 and padding-3 is necessary so that 'ObjectNode' is properly
// aligned.  (See BlockNode and ObjectNode, respectively)
//
// padding-2 is necessary so that objects are properly aligned.  Note that
// 'BlockNode' is always aligned because 'd_blockAllocator' always returns the
// maximally aligned address.
//
// padding-1 and padding-2 (and thus the proper alignments) are achieved by
// having 'd_dummy' field in 'BlockNode' and 'ObjectNode' respectively.
//..
//        padding-1    padding-2            padding-3    padding-2
// ----------------------------------------------------------------------------
// |        ||||        ||||                   ||||        ||||
// | Block  |||| Object ||||      Object       |||| Object |||| Object...
// | Node   |||| Node   ||||                   |||| Node   ||||
// |        ||||        ||||                   ||||        ||||
// ----------------------------------------------------------------------------
//                          <-ROUNDED_OBJECT_SIZE->
//              <--------OBJECT_FRAME_SIZE------->
//..

namespace BloombergLP {
namespace bdlcc {

                       // ---------------------------
                       // ObjectPool_CreatorConverter
                       // ---------------------------

// ACCESSORS
ObjectPoolFunctors::DefaultCreator
ObjectPool_CreatorConverter<ObjectPoolFunctors::DefaultCreator,
                            bsl::function<void(void*)> >::creator() const
{
    return bdlf::BindUtil::bind(d_creator, bdlf::PlaceHolders::_1);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
