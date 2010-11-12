// bcec_objectpool.cpp                                                -*-C++-*-
#include <bcec_objectpool.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcec_objectpool_cpp,"$Id$ $CSID$")

#include <bcec_fixedqueue.h>     // for testing only
#include <bdef_bind.h>
#include <bdef_placeholder.h>

///IMPLEMENTATION NOTES
///--------------------
// The bcec_ObjectPool algorithm is mostly lock-free, except for operations
// that may grow the pool; these are serialized by d_mutex.   The remaining
// operations achieve lock-free performance by relying on an atomic reference
// count.  Threads seeking to acquire an object from the pool atomically
// increment the count of the object at the head of the free list; the one
// that incremented from 0 goes on to use the object, while the others wait a
// few cycles for the free list to change, decrement the count, and try again.
// Reference counts are normally incremented and decremented by 2, because the
// lowest-order bit of the count is reserved to indicate an object that is
// available for reuse but has yet to be added back to the free list.
//
// The reference count is thus unguarded and is crucial to the whole
// algorithm.  Any change in the logic for using the reference count must
// be made with extreme caution and only after consulting with threading
// experts in our group (at this writing, Gino Rocha or Vlad Kliatchko).
//
// This picture describes a memory chunk returned by 'd_blockAllocator':
//
// padding-1 and padding-3 is necessary so that 'ObjectNode' is
// properly aligned.  (See BlockNode and ObjectNode, respectively)
//
// padding-2 is necessary so that objects are properly aligned.
// Note that 'BlockNode' is always aligned because
// 'd_blockAllocator' always returns the maximally aligned address.
//
// padding-1 and padding-2 (and thus the proper alignments) are
// achieved by having 'd_dummy' field in 'BlockNode' and 'ObjectNode'
// respectively.
//
//        padding-1    padding-2            padding-3    padding-2
// -------------------------------------------------------------------
// |        ||||        ||||                   ||||        ||||
// | Block  |||| Object ||||      Object       |||| Object |||| Object..
// | Node   |||| Node   ||||                   |||| Node   ||||
// |        ||||        ||||                   ||||        ||||
// --------------------------------------------------------------------
//                          <-ROUNDED_OBJECT_SIZE->
//              <--------OBJECT_FRAME_SIZE------->

namespace BloombergLP {

                         // --------------------------------
                         // bcec_ObjectPool_CreatorConverter
                         // --------------------------------

// ACCESSORS
bcec_ObjectPoolFunctors::DefaultCreator
bcec_ObjectPool_CreatorConverter<bcec_ObjectPoolFunctors::DefaultCreator,
                               bdef_Function<void(*)(void*)> >::creator() const
{
    return bdef_BindUtil::bind(d_creator, bdef_PlaceHolders::_1);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
