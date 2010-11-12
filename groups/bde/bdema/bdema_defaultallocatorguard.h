// bdema_defaultallocatorguard.h    -*-C++-*-
#ifndef INCLUDED_BDEMA_DEFAULTALLOCATORGUARD
#define INCLUDED_BDEMA_DEFAULTALLOCATORGUARD

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide scoped guard to temporarily change the default allocator.
//
//@DEPRECATED: Use 'bslma_defaultallocatorguard' instead.
//
//@CLASSES:
//  bdema_DefaultAllocatorGuard: default-allocator scoped guard
//
//@SEE_ALSO: bdema_allocator, bdema_default
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@DESCRIPTION: This component provides an object,
// 'bdema_DefaultAllocatorGuard', that serves as a "scoped guard" to enable the
// temporary replacement of the process-wide default allocator.  This
// functionality is intended for *testing* only, and in no event should this
// component be used except in 'main'.
//
// The guard object takes as its constructor
// argument the address of an object of a class derived from 'bdema_Allocator'.
// The default allocator at the time of guard construction is held by the
// guard, and the constructor-argument allocator is installed as the new
// process-wide default allocator (via a call to
// 'bdema_Default::setDefaultAllocator').  Upon destruction of the guard
// object, its held allocator is restored as the process-wide default
// allocator (via another call to 'bdema_Default::setDefaultAllocator').
//
///Usage
///-----
// The 'bdema_default' component ensures that, unless the owner of 'main' or
// some intervening code explicitly installs a default allocator, the
// 'bdema_NewDeleteAllocator::singleton()' will be the default allocator for
// that process  (i.e., calls to 'bdema_Default::defaultAllocator()' will
// return the new-delete allocator unless someone has set the default allocator
// to a different allocator intentionally).  Consider for purposes of this
// illustrative example the case where we, as owners of a test driver 'main',
// can count on the 'bdema_NewDeleteAllocator' singleton being the default.
//
// Consider now that, for testing purposes, we want a simple counting allocator
// that uses 'new' and 'delete', and that also keeps count of the number of
// memory blocks that have been allocated but never deallocated.  (Note that,
// in testing real production code, 'bdema_TestAllocator' serves this purpose.)
//..
//    class my_CountingAllocator : public bdema_Allocator
//    {
//        int d_blocksOutstanding;
//      public:
//        my_CountingAllocator();
//        ~my_CountingAllocator();
//
//        virtual void *allocate(int size);
//        virtual void deallocate(void *address);
//
//        int blocksOutstanding() const { return d_blocksOutstanding; }
//    };
//
//    inline
///   my_CountingAllocator::my_CountingAllocator()
//    : d_blocksOutstanding(0)
//    {
//    }
//
//    inline
///   my_CountingAllocator::~my_CountingAllocator()
//    {
//        if (0 < d_blocksOutstanding) {
//            bsl::cout << "***ERROR: Memory Leak***" << bsl::endl
//                      << d_blocksOutstanding << " block(s) leaked.  "
//                      << "Program aborting." << bsl::endl;
//            assert(0);
//        }
//    }
//
//    inline
//    void *my_CountingAllocator::allocate(int size)
//    {
//        ++d_blocksOutstanding;
//        return operator new(size);
//    }
//
//    inline
//    void my_CountingAllocator::deallocate(void *address)
//    {
//        --d_blocksOutstanding;
//        operator delete(address);
//    }
//..
// We may now write a test driver for some component that uses a
// 'bdema_Allocator' and the 'bdema_Default' mechanism.  First, we confirm that
// the 'bdema_NewDeleteAllocator' singleton is indeed installed.
//..
//    //my_component.t.cpp
//
//    // ...
//
//   int main()
//   {
//       // ...
//       bdema_NewDeleteAllocator *na = &bdema_NewDeleteAllocator::singleton();
//       assert(na == bdema_Default::defaultAllocator());
//..
// Now, we can go into some inner scope and use a guard object to install a
// test allocator as the default for any newly created objects.  When the inner
// scope ends, the guard will be destroyed, automatically restoring the
// original default allocator.
//..
//       {
//           my_CountingAllocator        testAllocator;
//           bdema_DefaultAllocatorGuard guard(&testAllocator);
//           assert(&testAllocator == bdema_Default::defaultAllocator());
//
//           // Create and test the object under test, which will use the
//           // test allocator *by* *default*.
//
//           // ...
//       }
//       assert(na == bdema_Default::defaultAllocator());
//..
// If the test block above has a memory leak, the program will print an error
// to 'stdout' and abort.  Otherwise, the new-delete allocator will be
// re-installed as the default allocator, and the program will proceed to the
// next block of code.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULTALLOCATORGUARD
#include <bslma_defaultallocatorguard.h>
#endif

namespace BloombergLP {

typedef bslma_DefaultAllocatorGuard bdema_DefaultAllocatorGuard;
    // Upon construction, an object of this class saves the current default
    // allocator and installs the user-specified allocator as the default
    // allocator.  On destruction, the original default allocator is restored.
    // Objects of this class are intended for testing purposes *only* and are
    // *not* guaranteed to be safe in production environments.

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
