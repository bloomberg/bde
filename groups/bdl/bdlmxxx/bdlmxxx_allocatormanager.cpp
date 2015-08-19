// bdlmxxx_allocatormanager.cpp                                       -*-C++-*-
#include <bdlmxxx_allocatormanager.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_allocatormanager_cpp,"$Id$ $CSID$")

#include <bdlma_multipoolallocator.h>
#include <bdlma_sequentialallocator.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_deleterhelper.h>
#include <bslma_rawdeleterproctor.h>

#include <bsls_assert.h>

namespace BloombergLP {

// LOCAL CONSTANTS
enum {
    INITIAL_SIZE =  4,  // if 'BDEM_WRITE_MANY', then initial capacity of
                        // smallest pool; if 'BDEM_WRITE_ONCE', then size
                        // (in bytes) of the initial memory reservation

    NUM_POOLS    = 12   // number of 'bdlma::MultipoolAllocator' pools
                        // ('BDEM_WRITE_MANY' only)
};

// STATIC HELPER FUNCTIONS
static
bslma::Allocator *init(
                  bdlmxxx::AggregateOption::AllocationStrategy  allocationStrategy,
                  int                                       initialMemorySize,
                  bslma::Allocator                         *originalAllocator)
    // TBD doc
{
    bslma::Allocator *internalAllocator;

    switch (allocationStrategy) {
      case bdlmxxx::AggregateOption::BDEM_WRITE_MANY: {
        bdlma::MultipoolAllocator *multipoolAllocator =
            new (*originalAllocator) bdlma::MultipoolAllocator(
                                                            NUM_POOLS,
                                                            originalAllocator);

        bslma::RawDeleterProctor<bdlma::MultipoolAllocator, bslma::Allocator>
                         deleterProctor(multipoolAllocator, originalAllocator);

        multipoolAllocator->reserveCapacity(1, initialMemorySize);

        deleterProctor.release();

        internalAllocator = multipoolAllocator;
      } break;
      case bdlmxxx::AggregateOption::BDEM_WRITE_ONCE: {
        bdlma::SequentialAllocator *sequentialAllocator =
            new (*originalAllocator) bdlma::SequentialAllocator(
                                                            originalAllocator);

        bslma::RawDeleterProctor<bdlma::SequentialAllocator, bslma::Allocator>
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

namespace bdlmxxx {
                        // ---------------------------
                        // class AllocatorManager
                        // ---------------------------

// CREATORS
AllocatorManager::AllocatorManager(
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_allocationStrategy(allocationStrategy)
, d_internalAllocator_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_internalAllocator_p = init(allocationStrategy,
                                 INITIAL_SIZE,
                                 d_allocator_p);
    BSLS_ASSERT(d_internalAllocator_p);
}

AllocatorManager::AllocatorManager(
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  int                                       initialMemorySize,
                  bslma::Allocator                         *basicAllocator)
: d_allocationStrategy(allocationStrategy)
, d_internalAllocator_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_internalAllocator_p = init(allocationStrategy,
                                 initialMemorySize,
                                 d_allocator_p);
    BSLS_ASSERT(d_internalAllocator_p);
}

AllocatorManager::~AllocatorManager()
{
    BSLS_ASSERT(d_internalAllocator_p);
    BSLS_ASSERT(d_allocator_p);

    if (static_cast<int>(d_allocationStrategy)
                             & AggregateOption::BDEM_OWN_ALLOCATOR_FLAG) {
        BSLS_ASSERT(d_allocator_p != d_internalAllocator_p);

        bslma::DeleterHelper::deleteObject(d_internalAllocator_p,
                                           d_allocator_p);
    }
}

// MANIPULATORS
void AllocatorManager::reserveMemory(int numBytes)
{
    BSLS_ASSERT(0 < numBytes);

    switch (d_allocationStrategy) {
      case AggregateOption::BDEM_WRITE_MANY: {
        bdlma::MultipoolAllocator *multipoolAllocator =
                static_cast<bdlma::MultipoolAllocator *>(d_internalAllocator_p);

        multipoolAllocator->reserveCapacity(1, numBytes);
      } break;
      case AggregateOption::BDEM_WRITE_ONCE: {
        bdlma::SequentialAllocator *sequentialAllocator =
               static_cast<bdlma::SequentialAllocator *>(d_internalAllocator_p);

        sequentialAllocator->reserveCapacity(numBytes);
      } break;
      default: {
        // Nothing to do for 'BDEM_PASS_THROUGH' or 'BDEM_SUBORDINATE'.
      } break;
    }
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
