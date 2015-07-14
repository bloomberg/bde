// bdlmca_xxxprotectablemultipool.cpp                                     -*-C++-*-
#include <bdlmca_xxxprotectablemultipool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmca_xxxprotectablemultipool_cpp,"$Id$ $CSID$")

#include <bdlmtt_lockguard.h>

#include <bdlma_memoryblockdescriptor.h>

#include <bsls_alignmentutil.h>

namespace BloombergLP {

namespace {

///IMPLEMENTATION NOTES
///--------------------
// The 'bdlmca::ProtectableMultipool' uses a 'bdlma::ProtectableBlockList' to
// manage memory chunks larger than the largest individual pool because the
// wrapped 'bdlmca::Multipool' is supplied a sequential allocator at construction
// and will not free non-pooled memory.  To facilitate this, the following
// 'struct Header' is a copy of the identical 'struct' that is 'private' to
// 'bdlmca::Multipool'.

// TYPES
struct Header {
    // Stores the pool number of this item.

    union {
        int                                 d_pool;   // pool for this item
        bsls::AlignmentUtil::MaxAlignedType d_dummy;  // force max. alignment
    } d_header;
};

}  // close unnamed namespace

namespace bdlmca {
                // --------------------------------
                // class ProtectableMultipool
                // --------------------------------

// CREATORS
ProtectableMultipool::
ProtectableMultipool(int                              numMemoryPools,
                           bdlma::ProtectableBlockDispenser *blockDispenser)
: d_blockList(blockDispenser)
, d_dispenserAdapter(&d_mutex, blockDispenser)
, d_allocator(&d_dispenserAdapter)
{
    d_pools_p = new (d_allocator) Multipool(numMemoryPools,
                                                  &d_allocator);
}

// MANIPULATORS
void *ProtectableMultipool::allocate(size_type size)
{
    if (size <= static_cast<size_type>(d_pools_p->maxPooledBlockSize())) {
        return d_pools_p->allocate(size);                             // RETURN
    }
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);

    // The requested size is large and will not be pooled.  Use our own block
    // list because the one in 'd_pools_p' is based on a sequential allocator,
    // and will not free memory.

    bdlma::MemoryBlockDescriptor block =
                                   d_blockList.allocate(size + sizeof(Header));
    if (block.isNull()) {
        return 0;                                                     // RETURN
    }
    Header *p = static_cast<Header *>(block.address());
    p->d_header.d_pool = -1;

    return p + 1;
}

void ProtectableMultipool::deallocate(void *address)
{
    Header *h = (Header *)address - 1;
    const int pool = h->d_header.d_pool;
    if (-1 == pool) {
        bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
        d_blockList.deallocate(h);
    }
    else {
        d_pools_p->deallocate(address);
    }
}

void ProtectableMultipool::release()
{
    const int numPools = d_pools_p->numPools();

    d_blockList.release();
    d_allocator.release();  // frees d_pools_p!

    d_pools_p = new (d_allocator) Multipool(numPools, &d_allocator);
}

void ProtectableMultipool::reserveCapacity(size_type size,
                                                 size_type numObjects)
{
    d_pools_p->reserveCapacity(size, numObjects);
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
