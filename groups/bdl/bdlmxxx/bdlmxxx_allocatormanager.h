// bdlmxxx_allocatormanager.h                                         -*-C++-*-
#ifndef INCLUDED_BDLMXXX_ALLOCATORMANAGER
#define INCLUDED_BDLMXXX_ALLOCATORMANAGER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an allocator manager based on a 'bdem' allocation strategy.
//
//@CLASSES:
//  bdlmxxx::AllocatorManager: manages an allocator based on an allocation strategy
//
//@AUTHOR: Rohan Bhindwale (rbhindwa)
//
//@SEE_ALSO: bdlmxxx_aggregateoption, bdlmxxx_choiceimp, bdlmxxx_listimp, bdlmxxx_tableimp
//
//@DESCRIPTION: This component provides a class, 'bdlmxxx::AllocatorManager', which
// is a mechanism that manages (internally) a memory allocator based on a
// 'bdem' allocation strategy (see 'bdlmxxx_aggregateoption').  If the allocation
// strategy supplied at construction of a 'bdlmxxx::AllocatorManager' is either
// 'BDEM_WRITE_ONCE' or 'BDEM_WRITE_MANY', an allocator that is appropriate for
// that strategy is constructed internally; the 'internalAllocator' method
// returns this internally-managed allocator.  If the allocation strategy
// supplied at construction is instead either 'BDEM_PASS_THROUGH' or
// 'BDEM_SUBORDINATE', no allocator is constructed internally and
// 'internalAllocator' returns the same allocator as is returned by
// 'originalAllocator'.  Note that all allocations from an allocator manager
// object generally should be made from the allocator that is returned by
// 'internalAllocator'.  When a 'bdlmxxx::AllocatorManager' object is destroyed,
// the internally-managed allocator (if any) is destroyed, and all memory that
// was allocated from it is released.
//
///Usage
///-----
// The bdlmxxx::AllocatorManager' class is meant to be used collaboratively by
// higher level components in the 'bdem' package and should not be used
// directly by client code.  Therefore, a usage example is not provided.  See
// 'bdlmxxx_choiceimp', 'bdlmxxx_choicearrayimp', 'bdlmxxx_listimp', and 'bdlmxxx_tableimp'
// for appropriate use of 'bdlmxxx::AllocatorManager'.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMXXX_AGGREGATEOPTION
#include <bdlmxxx_aggregateoption.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

namespace BloombergLP {

namespace bdlmxxx {
                        // ===========================
                        // class AllocatorManager
                        // ===========================

class AllocatorManager {
    // This class is a mechanism that manages (internally) a memory allocator
    // based on a 'bdem' allocation strategy (see 'bdlmxxx_aggregateoption').  If
    // the allocation strategy supplied at construction is either
    // 'BDEM_WRITE_ONCE' or 'BDEM_WRITE_MANY', an allocator that is appropriate
    // for that strategy is constructed internally; the 'internalAllocator'
    // method returns this internally-managed allocator.  No internally-managed
    // allocator is constructed for the other 'bdem' allocation strategies; in
    // that case, 'internalAllocator' returns the same allocator as is returned
    // by 'originalAllocator'.  Note that all allocations from an object of
    // this class generally should be made from the allocator that is returned
    // by 'internalAllocator'.  When an allocator manager object is destroyed,
    // the internally-managed allocator (if any) is destroyed, and all memory
    // that was allocated from it is released.
    //
    // This component is *not* meant for direct use by clients.  It is intended
    // to be used by higher level components in the 'bdem' package
    // (specifically, 'bdlmxxx_choiceimp', 'bdlmxxx_choicearrayimp', 'bdlmxxx_listimp',
    // and 'bdlmxxx_tableimp').

    // DATA
    AggregateOption::AllocationStrategy
                             d_allocationStrategy;   // allocation strategy

    bslma::Allocator        *d_internalAllocator_p;  // internally-managed
                                                     // allocator (owned)

    bslma::Allocator        *d_allocator_p;          // memory allocator (held,
                                                     // not owned)

  private:
    // NOT IMPLEMENTED
    AllocatorManager(const AllocatorManager&);
    AllocatorManager& operator=(const AllocatorManager&);

  public:
    // CREATORS
    AllocatorManager(
                 AggregateOption::AllocationStrategy  allocationStrategy,
                 bslma::Allocator                         *basicAllocator = 0);
    AllocatorManager(
                 AggregateOption::AllocationStrategy  allocationStrategy,
                 int                                       initialMemorySize,
                 bslma::Allocator                         *basicAllocator = 0);
        // Create an allocator manager based on the specified
        // 'allocationStrategy'.  Optionally specify an 'initialMemorySize' (in
        // bytes) that is preallocated in order to satisfy allocation requests
        // without replenishment (i.e., without internal allocation): it has no
        // effect unless 'allocationStrategy' is 'BDEM_WRITE_ONCE' or
        // 'BDEM_WRITE_MANY'.  If 'initialMemorySize' is not specified, an
        // implementation-dependent value is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~AllocatorManager();
        // Destroy this allocator manager.  If the allocation strategy supplied
        // at construction was either 'BDEM_WRITE_ONCE' or 'BDEM_WRITE_MANY',
        // all memory allocated from the internally-managed allocator is
        // released, and the allocator is destroyed; otherwise, this method has
        // no effect.

    // MANIPULATORS
    bslma::Allocator *internalAllocator();
        // Return the address of the internal allocator of this allocator
        // manager.  The address is distinct from that returned by
        // 'originalAllocator' if the allocation strategy supplied at
        // construction was either 'BDEM_WRITE_ONCE' or 'BDEM_WRITE_MANY', and
        // is the same address otherwise.  The allocator that is returned
        // remains valid for the lifetime of this allocator manager.  Note that
        // all memory allocations from this allocator manager generally should
        // be made from the allocator that is returned by this method.

    bslma::Allocator *originalAllocator();
        // Return the address of the allocator supplied at construction of this
        // allocator manager, or the address of the default allocator that was
        // in effect at the time of construction if no allocator was supplied.

    void reserveMemory(int numBytes);
        // Reserve sufficient memory to satisfy allocation requests for at
        // least the specified 'numBytes' without replenishment (i.e., without
        // internal allocation) if 'allocationStrategy' is 'BDEM_WRITE_ONCE',
        // or 'BDEM_WRITE_MANY'.  If 'allocationStrategy' is 'BDEM_WRITE_MANY'
        // then the reserved memory is supplied only for allocation requests
        // of 8 bytes or less (i.e., larger requests will issue new
        // allocations).  This method has no effect unless 'allocationStrategy'
        // is either 'BDEM_WRITE_ONCE' or 'BDEM_WRITE_MANY'.  The behavior is
        // undefined unless '0 < numBytes'.

    // ACCESSORS
    AggregateOption::AllocationStrategy allocationStrategy() const;
        // Return the allocation strategy that was supplied at construction of
        // this allocator manager.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------------
                        // class AllocatorManager
                        // ---------------------------

// MANIPULATORS
inline
bslma::Allocator *AllocatorManager::internalAllocator()
{
    return d_internalAllocator_p;
}

inline
bslma::Allocator *AllocatorManager::originalAllocator()
{
    return d_allocator_p;
}

// ACCESSORS
inline
AggregateOption::AllocationStrategy
AllocatorManager::allocationStrategy() const
{
    return d_allocationStrategy;
}
}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
