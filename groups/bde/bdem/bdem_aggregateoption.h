// bdem_aggregateoption.h                                             -*-C++-*-
#ifndef INCLUDED_BDEM_AGGREGATEOPTION
#define INCLUDED_BDEM_AGGREGATEOPTION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enumerate memory management strategies used by 'bdem'.
//
//@CLASSES:
//  bdem_AggregateOption: namespace for 'enum' of memory management strategies
//
//@SEE_ALSO
//
//@AUTHOR Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides an 'enum' type, 'AllocationStrategy',
// within the namespace of the 'struct' 'bdem_AggregateOption'.  The
// enumerated values are expressly intended for use as constructor arguments
// for other 'bdem' components to guide those components in selecting
// efficient memory management strategies.
//
// 'AllocationStrategy' values are built by OR-ing together bit patterns in a
// separate (unnamed) enumeration.  These enumerated flag values can also be
// used to mask out certain strategy modes when passing strategy options to
// nested containers.
//
///Usage
///-----
// The following snippets of code illustrate basic usage:
//..
//  bdem_AggregateOption::AllocationStrategy strategy;
//
//  strategy = bdem_AggregateOption::BDEM_PASS_THROUGH;
//         assert(0 == strategy);
//         assert(!(strategy & bdem_AggregateOption::BDEM_OWN_ALLOCATOR_FLAG));
//         assert(!(strategy & bdem_AggregateOption::BDEM_NODESTRUCT_FLAG));
//
//  strategy = bdem_AggregateOption::BDEM_WRITE_MANY;
//         assert(1 == strategy);
//         assert(  strategy & bdem_AggregateOption::BDEM_OWN_ALLOCATOR_FLAG);
//         assert(!(strategy & bdem_AggregateOption::BDEM_NODESTRUCT_FLAG));
//
//  strategy = bdem_AggregateOption::BDEM_WRITE_ONCE;
//         assert(3 == strategy);
//         assert(  strategy & bdem_AggregateOption::BDEM_OWN_ALLOCATOR_FLAG);
//         assert(  strategy & bdem_AggregateOption::BDEM_NODESTRUCT_FLAG);
//
//  strategy = bdem_AggregateOption::BDEM_SUBORDINATE;
//         assert(2 == strategy);
//         assert(!(strategy & bdem_AggregateOption::BDEM_OWN_ALLOCATOR_FLAG));
//         assert(  strategy & bdem_AggregateOption::BDEM_NODESTRUCT_FLAG);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

namespace BloombergLP {

                     // ===========================
                     // struct bdem_AggregateOption
                     // ===========================

struct bdem_AggregateOption {
    // This 'struct' provides a common namespace for an enumerated type.
    // The enumerated values defined here can be supplied to other 'bdem'
    // components to guide those components in selecting efficient memory
    // management strategies.

    // TYPES
    enum {
        // This 'enum' provides bit masks that are used to define the
        // 'AllocationStrategy' enumeration type.  The defined flag values are
        // also directly useful for discovering allocation strategies for
        // existing objects.

        BDEM_OWN_ALLOCATOR_FLAG = 1,
            // This bit, when set, indicates that the aggregate object should
            // construct its own private managed allocator.

        BDEM_NODESTRUCT_FLAG    = 2
            // This bit, when set, indicates that destruction and deallocation
            // for an aggregate object are suppressed (e.g., because a
            // sequential allocator is used).

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , OWN_ALLOCATOR_FLAG = BDEM_OWN_ALLOCATOR_FLAG
      , NODESTRUCT_FLAG    = BDEM_NODESTRUCT_FLAG
#endif
    };

    enum AllocationStrategy {
        // This 'enum' indicate the memory management strategy to be used by an
        // aggregate object and its contained objects.  The individual values
        // are documented with their intended directives to the aggregate
        // object that will be constructed in accord with this strategy.

        BDEM_PASS_THROUGH = 0,
            // Perform no local pooling or special allocation.  Use the
            // passed-in allocator for each allocation and deallocate in the
            // normal way during element destruction.

        BDEM_WRITE_MANY = BDEM_OWN_ALLOCATOR_FLAG,
            // Create a local allocator for the container that optimizes
            // run-time performance assuming that contained elements will be
            // modified many times over their lifetimes; this optimization
            // comes at the expense of a (typically modest) increase in memory
            // usage.  Destroying the top-level container releases all
            // associated memory at once.

        BDEM_WRITE_ONCE = BDEM_OWN_ALLOCATOR_FLAG | BDEM_NODESTRUCT_FLAG,
            // Create a very fast local allocator that optimizes both memory
            // and run-time performance for contained objects that are seldom
            // modified; individual memory deallocations are ignored.
            // Destroying the top-level container releases all associated
            // memory at once.

        BDEM_SUBORDINATE = BDEM_NODESTRUCT_FLAG
            // Collaborate with a parent container using a local allocator by
            // suppressing destruction and deallocation of individual objects.
            // This behavior *temporarily* leaks memory, but all memory will be
            // reclaimed when the highest-level parent aggregate's allocator is
            // destroyed.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , PASS_THROUGH = BDEM_PASS_THROUGH
      , WRITE_MANY   = BDEM_WRITE_MANY
      , WRITE_ONCE   = BDEM_WRITE_ONCE
      , SUBORDINATE  = BDEM_SUBORDINATE
#endif
    };
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
