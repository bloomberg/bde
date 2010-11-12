// bcema_protectablemultipool.cpp                                     -*-C++-*-
#include <bcema_protectablemultipool.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcema_protectablemultipool_cpp,"$Id$ $CSID$")

#include <bcemt_lockguard.h>

#include <bdema_memoryblockdescriptor.h>

#include <bsls_alignmentutil.h>

namespace BloombergLP {

namespace {

///IMPLEMENTATION NOTES
///--------------------
// The 'bcema_ProtectableMultipool' uses a 'bdema_ProtectableBlockList' to
// manage memory chunks larger than the largest individual pool because the
// wrapped 'bcema_Multipool' is supplied a sequential allocator at construction
// and will not free non-pooled memory.  To facilitate this, the following
// 'struct Header' is a copy of the identical 'struct' that is 'private' to
// 'bcema_Multipool'.

// TYPES
struct Header {
    // Stores the pool number of this item.

    union {
        int                                d_pool;   // pool for this item
        bsls_AlignmentUtil::MaxAlignedType d_dummy;  // force maximum alignment
    } d_header;
};

}  // close unnamed namespace

                // --------------------------------
                // class bcema_ProtectableMultipool
                // --------------------------------

// CREATORS
bcema_ProtectableMultipool::
bcema_ProtectableMultipool(int                              numMemoryPools,
                           bdema_ProtectableBlockDispenser *blockDispenser)
: d_blockList(blockDispenser)
, d_dispenserAdapter(&d_mutex, blockDispenser)
, d_allocator(&d_dispenserAdapter)
{
    d_pools_p = new (d_allocator) bcema_Multipool(numMemoryPools,
                                                  &d_allocator);
}

// MANIPULATORS
void *bcema_ProtectableMultipool::allocate(size_type size)
{
    if (size <= d_pools_p->maxPooledBlockSize()) {
        return d_pools_p->allocate(size);                             // RETURN
    }
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    // The requested size is large and will not be pooled.  Use our own block
    // list because the one in 'd_pools_p' is based on a sequential allocator,
    // and will not free memory.

    bdema_MemoryBlockDescriptor block =
                                   d_blockList.allocate(size + sizeof(Header));
    if (block.isNull()) {
        return 0;                                                     // RETURN
    }
    Header *p = static_cast<Header *>(block.address());
    p->d_header.d_pool = -1;

    return p + 1;
}

void bcema_ProtectableMultipool::deallocate(void *address)
{
    Header *h = (Header *)address - 1;
    const int pool = h->d_header.d_pool;
    if (-1 == pool) {
        bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
        d_blockList.deallocate(h);
    }
    else {
        d_pools_p->deallocate(address);
    }
}

void bcema_ProtectableMultipool::release()
{
    const int numPools = d_pools_p->numPools();

    d_blockList.release();
    d_allocator.release();  // frees d_pools_p!

    d_pools_p = new (d_allocator) bcema_Multipool(numPools, &d_allocator);
}

void bcema_ProtectableMultipool::reserveCapacity(size_type size,
                                                 size_type numObjects)
{
    d_pools_p->reserveCapacity(size, numObjects);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
