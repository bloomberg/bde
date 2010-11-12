// bdem_aggregateimp.cpp     -*-C++-*-

#include <bdem_aggregateimp.h>

#include <bdem_descriptor.h>
#include <bdem_functiontemplates.h>

#include <bdet_date.h>
#include <bdet_time.h>
#include <bdet_datetime.h>
#include <bdetu_null.h>

#include <bdema_allocator.h>
#include <bdema_autodeleter.h>
#include <bdema_multipoolallocator.h>
#include <bdema_sequentialallocator.h>

#include <cassert>

namespace BloombergLP {

                        // =======================
                        // class bdem_AggregateImp
                        // =======================

                        // ---------
                        // CONSTANTS
                        // ---------

enum {
    INITIAL_SIZE = 2,            // initial size of idxoffsetmap
    INITIAL_SMALL_CAPACITY = 4,  // initial capacity of smallest pool
    MULTIPOOLALLOCATOR_BINS = 12 // The enum 'POOL_BINS' defines the number of
                                 // bins a 'bdema_MultipoolAllocator' should be
                                 // created with.
};

                        // ----------------
                        // STATIC FUNCTIONS
                        // ----------------


bdema_Allocator *bdem_AggregateImp::makeInternalAllocator(
                   bdem_AggregateOption::AllocationStrategy  allocMode,
                   int                                       initialMemory,
                   bdema_Allocator                          *originalAllocator)
{
    bdema_Allocator *internalAllocator;

    switch (allocMode) {
      case bdem_AggregateOption::WRITE_MANY: {

        bdema_MultipoolAllocator *multipoolAllocator =
            new (*originalAllocator)
                              bdema_MultipoolAllocator(MULTIPOOLALLOCATOR_BINS,
                                                       originalAllocator);

        bdema_AutoDeleter<bdema_MultipoolAllocator, bdema_Allocator>
                   allocatorAutoDel(&multipoolAllocator, 1, originalAllocator);

        multipoolAllocator->reserveCapacity(1, initialMemory);

        allocatorAutoDel.release();

        internalAllocator = multipoolAllocator;

      } break;
      case bdem_AggregateOption::WRITE_ONCE: {

        bdema_SequentialAllocator *sequentialAllocator =
            new (*originalAllocator)
                                  bdema_SequentialAllocator(originalAllocator);

        bdema_AutoDeleter<bdema_SequentialAllocator, bdema_Allocator>
                  allocatorAutoDel(&sequentialAllocator, 1, originalAllocator);

        sequentialAllocator->reserveCapacity(initialMemory);

        allocatorAutoDel.release();

        internalAllocator = sequentialAllocator;

      } break;
      default: {

        internalAllocator = originalAllocator;

      } break;
    }

    return internalAllocator;
}

                            // --------------------
                            // PRIVATE INITIALISERS
                            // --------------------

inline
void bdem_AggregateImp::init(
                      bdem_AggregateOption::AllocationStrategy  allocMode,
                      int                                       initialMemory,
                      bdema_Allocator                          *basicAllocator)
{
    d_allocMode = allocMode;
    d_originalAllocator = bdema_Default::allocator(basicAllocator);

    bdema_AutoDeleter<bdema_Allocator, bdema_Allocator>
                allocatorAutoDel(&d_internalAllocator, 0, d_originalAllocator);
    d_internalAllocator = makeInternalAllocator(allocMode,
                                                initialMemory,
                                                d_originalAllocator);

    if (d_originalAllocator != d_internalAllocator) {
        ++allocatorAutoDel;
    }

    d_rowDef = new (*d_internalAllocator)
      bdem_RowDef(INITIAL_SIZE, internalAllocationMode(), d_internalAllocator);

    allocatorAutoDel.release();
}

inline
void bdem_AggregateImp::init(
                     const bdem_ElemType::Type                 elementTypes[],
                     int                                       numElements,
                     const bdem_Descriptor *const              attrLookupTbl[],
                     bdem_AggregateOption::AllocationStrategy  allocMode,
                     int                                       initialMemory,
                     bdema_Allocator                          *basicAllocator)
{
    d_allocMode = allocMode;
    d_originalAllocator = bdema_Default::allocator(basicAllocator);

    bdema_AutoDeleter<bdema_Allocator, bdema_Allocator>
        allocatorAutoDel(&d_internalAllocator, 0, d_originalAllocator);
    d_internalAllocator = makeInternalAllocator(allocMode,
                                                initialMemory,
                                                d_originalAllocator);

    if (d_originalAllocator!=d_internalAllocator) {
        ++allocatorAutoDel;
    }

    d_rowDef = new (*d_internalAllocator)
       bdem_RowDef(elementTypes, numElements, attrLookupTbl,
                   internalAllocationMode(), d_internalAllocator);

    allocatorAutoDel.release();
}

inline
void bdem_AggregateImp::init(
                      const bdem_RowDef&                        def,
                      bdem_AggregateOption::AllocationStrategy  allocMode,
                      int                                       initialMemory,
                      bdema_Allocator                          *basicAllocator)
{
    d_allocMode = allocMode;
    d_originalAllocator = bdema_Default::allocator(basicAllocator);

    bdema_AutoDeleter<bdema_Allocator, bdema_Allocator>
                allocatorAutoDel(&d_internalAllocator, 0, d_originalAllocator);
    d_internalAllocator = makeInternalAllocator(allocMode,
                                                initialMemory,
                                                d_originalAllocator);

    if (d_originalAllocator!=d_internalAllocator) {
        ++allocatorAutoDel;
    }

    d_rowDef = new (*d_internalAllocator)
        bdem_RowDef(def, internalAllocationMode(), d_internalAllocator);

    allocatorAutoDel.release();
}

inline
void bdem_AggregateImp::init(
                      const bdem_RowHeader&                     row,
                      bdem_AggregateOption::AllocationStrategy  allocMode,
                      int                                       initialMemory,
                      bdema_Allocator                          *basicAllocator)
{
    d_allocMode = allocMode;
    d_originalAllocator = bdema_Default::allocator(basicAllocator);

    bdema_AutoDeleter<bdema_Allocator, bdema_Allocator>
                allocatorAutoDel(&d_internalAllocator, 0, d_originalAllocator);
    d_internalAllocator = makeInternalAllocator(allocMode,
                                                initialMemory,
                                                d_originalAllocator);

    if (d_originalAllocator!=d_internalAllocator) {
        ++allocatorAutoDel;
    }

    d_rowDef = new (*d_internalAllocator)
        bdem_RowDef(row, internalAllocationMode(), d_internalAllocator);

    allocatorAutoDel.release();
}

                        // --------
                        // CREATORS
                        // --------

bdem_AggregateImp::
    bdem_AggregateImp(bdem_AggregateOption::AllocationStrategy  allocMode,
                      bdema_Allocator                          *basicAllocator)
{
    init(allocMode, INITIAL_SMALL_CAPACITY, basicAllocator);
}

bdem_AggregateImp::bdem_AggregateImp(
                      bdem_AggregateOption::AllocationStrategy  allocMode,
                      const InitialMemory&                      initialMemory,
                      bdema_Allocator                          *basicAllocator)
{
    init(allocMode, initialMemory, basicAllocator);
}

bdem_AggregateImp::
   bdem_AggregateImp(const bdem_ElemType::Type                 elementTypes[],
                     int                                       numElements,
                     const bdem_Descriptor *const              attrLookupTbl[],
                     bdem_AggregateOption::AllocationStrategy  allocMode,
                     bdema_Allocator                          *basicAllocator)
{
    init(elementTypes,
         numElements,
         attrLookupTbl,
         allocMode,
         INITIAL_SMALL_CAPACITY,
         basicAllocator);
}

bdem_AggregateImp::
   bdem_AggregateImp(const bdem_ElemType::Type                 elementTypes[],
                     int                                       numElements,
                     const bdem_Descriptor *const              attrLookupTbl[],
                     bdem_AggregateOption::AllocationStrategy  allocMode,
                     const InitialMemory&                      initialMemory,
                     bdema_Allocator                          *basicAllocator)
{
    init(elementTypes,
         numElements,
         attrLookupTbl,
         allocMode,
         initialMemory,
         basicAllocator);
}

bdem_AggregateImp::
    bdem_AggregateImp(const bdem_RowDef&                        def,
                      bdem_AggregateOption::AllocationStrategy  allocMode,
                      bdema_Allocator                          *basicAllocator)
{
    init(def, allocMode, INITIAL_SMALL_CAPACITY, basicAllocator);
}

bdem_AggregateImp::
    bdem_AggregateImp(const bdem_RowDef&                        def,
                      bdem_AggregateOption::AllocationStrategy  allocMode,
                      const InitialMemory&                      initialMemory,
                      bdema_Allocator                          *basicAllocator)
{
    init(def, allocMode, initialMemory, basicAllocator);
}

bdem_AggregateImp::
    bdem_AggregateImp(const bdem_RowHeader&                     row,
                      bdem_AggregateOption::AllocationStrategy  allocMode,
                      bdema_Allocator                          *basicAllocator)
{
    init(row, allocMode, INITIAL_SMALL_CAPACITY, basicAllocator);
}

bdem_AggregateImp::
    bdem_AggregateImp(const bdem_RowHeader&                     row,
                      bdem_AggregateOption::AllocationStrategy  allocMode,
                      const InitialMemory&                      initialMemory,
                      bdema_Allocator                          *basicAllocator)
{
    init(row, allocMode, initialMemory, basicAllocator);
}

bdem_AggregateImp::~bdem_AggregateImp()
{
    // class invariants
    assert(d_rowDef);
    assert(d_originalAllocator);
    assert(d_internalAllocator);

    if ( d_allocMode & bdem_AggregateOption::OWN_ALLOCATOR_FLAG ) {
        // Pooled. Row and all of its elements will go away automagically
        // when the internal allocator is deleted.
        assert(d_originalAllocator != d_internalAllocator);
        d_internalAllocator->~bdema_Allocator();
        d_originalAllocator->deallocate(d_internalAllocator);
    }
    else if ( !(d_allocMode & bdem_AggregateOption::NODESTRUCT_FLAG) ) {
        // Explicitly free allocated memory
        assert(d_originalAllocator == d_internalAllocator);
        d_rowDef->~bdem_RowDef();
        d_internalAllocator->deallocate(d_rowDef);
    }
}

                        // ------------
                        // MANIPULATORS
                        // ------------

void bdem_AggregateImp::reserveMemory(int numBytes)
{
    assert(0 < numBytes);

    switch (d_allocMode) {
      case bdem_AggregateOption::WRITE_MANY: {
        bdema_MultipoolAllocator *multipoolAllocator
                 = static_cast<bdema_MultipoolAllocator*>(d_internalAllocator);

        multipoolAllocator->reserveCapacity(1, numBytes);
      } break;
      case bdem_AggregateOption::WRITE_ONCE: {
        bdema_SequentialAllocator *sequentialAllocator
                = static_cast<bdema_SequentialAllocator*>(d_internalAllocator);

        sequentialAllocator->reserveCapacity(numBytes);
      } break;
      default: {
        // No op.
      } break;
    }
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
