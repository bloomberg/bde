// bdem_allocatormanager.cpp                                          -*-C++-*-
#include <bdem_allocatormanager.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_allocatormanager_cpp,"$Id$ $CSID$")

#include <bdema_multipoolallocator.h>
#include <bdema_sequentialallocator.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_rawdeleterproctor.h>

#include <bsls_assert.h>

namespace BloombergLP {

// LOCAL CONSTANTS
enum {
    INITIAL_SIZE =  4,  // if 'BDEM_WRITE_MANY', then initial capacity of
                        // smallest pool; if 'BDEM_WRITE_ONCE', then size
                        // (in bytes) of the initial memory reservation

    NUM_POOLS    = 12   // number of 'bdema_MultipoolAllocator' pools
                        // ('BDEM_WRITE_MANY' only)
};

// STATIC HELPER FUNCTIONS
static
bslma_Allocator *init(
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  int                                       initialMemorySize,
                  bslma_Allocator                          *originalAllocator)
    // TBD doc
{
    bslma_Allocator *internalAllocator;

    switch (allocationStrategy) {
      case bdem_AggregateOption::BDEM_WRITE_MANY: {
        bdema_MultipoolAllocator *multipoolAllocator =
            new (*originalAllocator) bdema_MultipoolAllocator(
                                                            NUM_POOLS,
                                                            originalAllocator);

        bslma_RawDeleterProctor<bdema_MultipoolAllocator, bslma_Allocator>
                         deleterProctor(multipoolAllocator, originalAllocator);

        multipoolAllocator->reserveCapacity(1, initialMemorySize);

        deleterProctor.release();

        internalAllocator = multipoolAllocator;
      } break;
      case bdem_AggregateOption::BDEM_WRITE_ONCE: {
        bdema_SequentialAllocator *sequentialAllocator =
            new (*originalAllocator) bdema_SequentialAllocator(
                                                            originalAllocator);

        bslma_RawDeleterProctor<bdema_SequentialAllocator, bslma_Allocator>
                        deleterProctor(sequentialAllocator, originalAllocator);

        sequentialAllocator->reserveCapacity(initialMemorySize);

        deleterProctor.release();

        internalAllocator = sequentialAllocator;
      } break;
      default: {
        internalAllocator = originalAllocator;
      } break;
    }

    return internalAllocator;
}

                        // ---------------------------
                        // class bdem_AllocatorManager
                        // ---------------------------

// CREATORS
bdem_AllocatorManager::bdem_AllocatorManager(
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_allocationStrategy(allocationStrategy)
, d_internalAllocator_p(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    d_internalAllocator_p = init(allocationStrategy,
                                 INITIAL_SIZE,
                                 d_allocator_p);
    BSLS_ASSERT(d_internalAllocator_p);
}

bdem_AllocatorManager::bdem_AllocatorManager(
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  int                                       initialMemorySize,
                  bslma_Allocator                          *basicAllocator)
: d_allocationStrategy(allocationStrategy)
, d_internalAllocator_p(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    d_internalAllocator_p = init(allocationStrategy,
                                 initialMemorySize,
                                 d_allocator_p);
    BSLS_ASSERT(d_internalAllocator_p);
}

bdem_AllocatorManager::~bdem_AllocatorManager()
{
    BSLS_ASSERT(d_internalAllocator_p);
    BSLS_ASSERT(d_allocator_p);

    if (static_cast<int>(d_allocationStrategy)
                             & bdem_AggregateOption::BDEM_OWN_ALLOCATOR_FLAG) {
        BSLS_ASSERT(d_allocator_p != d_internalAllocator_p);

        d_internalAllocator_p->~bslma_Allocator();
        d_allocator_p->deallocate(d_internalAllocator_p);
    }
}

// MANIPULATORS
void bdem_AllocatorManager::reserveMemory(int numBytes)
{
    BSLS_ASSERT(0 < numBytes);

    switch (d_allocationStrategy) {
      case bdem_AggregateOption::BDEM_WRITE_MANY: {
        bdema_MultipoolAllocator *multipoolAllocator =
                static_cast<bdema_MultipoolAllocator *>(d_internalAllocator_p);

        multipoolAllocator->reserveCapacity(1, numBytes);
      } break;
      case bdem_AggregateOption::BDEM_WRITE_ONCE: {
        bdema_SequentialAllocator *sequentialAllocator =
               static_cast<bdema_SequentialAllocator *>(d_internalAllocator_p);

        sequentialAllocator->reserveCapacity(numBytes);
      } break;
      default: {
        // Nothing to do for 'BDEM_PASS_THROUGH' or 'BDEM_SUBORDINATE'.
      } break;
    }
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
