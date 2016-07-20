// bslma_defaultallocatorguard.h                                      -*-C++-*-
#ifndef INCLUDED_BSLMA_DEFAULTALLOCATORGUARD
#define INCLUDED_BSLMA_DEFAULTALLOCATORGUARD

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide scoped guard to temporarily change the default allocator.
//
//@CLASSES:
//  bslma::DefaultAllocatorGuard: default-allocator scoped guard
//
//@SEE_ALSO: bslma_allocator, bslma_default
//
//@DESCRIPTION: This component provides an object,
// 'bslma::DefaultAllocatorGuard', that serves as a "scoped guard" to enable
// the temporary replacement of the process-wide default allocator.  This
// functionality is intended for *testing* only, and in no event should this
// component be used except in 'main'.
//
// The guard object takes as its constructor argument the address of an object
// of a class derived from 'bslma::Allocator'.  The default allocator at the
// time of guard construction is held by the guard, and the
// constructor-argument allocator is installed as the new process-wide default
// allocator (via a call to 'bslma::Default::setDefaultAllocator').  Upon
// destruction of the guard object, its held allocator is restored as the
// process-wide default allocator (via another call to
// 'bslma::Default::setDefaultAllocator').
//
///Usage
///-----
// The 'bslma_default' component ensures that, unless the owner of 'main' or
// some intervening code explicitly installs a default allocator, the
// 'bslma::NewDeleteAllocator::singleton()' will be the default allocator for
// that process (i.e., calls to 'bslma::Default::defaultAllocator()' will
// return the new-delete allocator unless someone has set the default allocator
// to a different allocator intentionally).  Consider for purposes of this
// illustrative example the case where we, as owners of a test driver 'main',
// can count on the 'bslma::NewDeleteAllocator' singleton being the default.
//
// Consider now that, for testing purposes, we want a simple counting allocator
// that uses 'new' and 'delete', and that also keeps count of the number of
// memory blocks that have been allocated but never deallocated.  (Note that,
// in testing real production code, 'bslma::TestAllocator' serves this
// purpose.)
//..
//  class my_CountingAllocator : public bslma::Allocator
//  {
//      int d_blocksOutstanding;
//    public:
//      my_CountingAllocator();
//      ~my_CountingAllocator();
//
//      virtual void *allocate(int size);
//      virtual void deallocate(void *address);
//
//      int blocksOutstanding() const { return d_blocksOutstanding; }
//  };
//
//  inline
//  my_CountingAllocator::my_CountingAllocator()
//  : d_blocksOutstanding(0)
//  {
//  }
//
//  inline
//  my_CountingAllocator::~my_CountingAllocator()
//  {
//      if (0 < d_blocksOutstanding) {
//          std::cout << "***ERROR: Memory Leak***" << std::endl
//                    << d_blocksOutstanding << " block(s) leaked.  "
//                    << "Program aborting." << std::endl;
//          assert(0);
//      }
//  }
//
//  inline
//  void *my_CountingAllocator::allocate(int size)
//  {
//      ++d_blocksOutstanding;
//      return operator new(size);
//  }
//
//  inline
//  void my_CountingAllocator::deallocate(void *address)
//  {
//      --d_blocksOutstanding;
//      operator delete(address);
//  }
//..
// We may now write a test driver for some component that uses a
// 'bslma::Allocator' and the 'bslma::Default' mechanism.  First, we confirm
// that the 'bslma::NewDeleteAllocator' singleton is indeed installed.
//..
//   //my_component.t.cpp
//
//   // ...
//
//  int main()
//  {
//      // ...
//      bslma::NewDeleteAllocator *na =
//                                     &bslma::NewDeleteAllocator::singleton();
//      assert(na == bslma::Default::defaultAllocator());
//..
// Now, we can go into some inner scope and use a guard object to install a
// test allocator as the default for any newly created objects.  When the inner
// scope ends, the guard will be destroyed, automatically restoring the
// original default allocator.
//..
//  {
//      my_CountingAllocator        testAllocator;
//      bslma::DefaultAllocatorGuard guard(&testAllocator);
//      assert(&testAllocator == bslma::Default::defaultAllocator());
//
//      // Create and test the object under test, which will use the
//      // test allocator *by* *default*.
//
//      // ...
//  }
//  assert(na == bslma::Default::defaultAllocator());
//..
// If the test block above has a memory leak, the program will print an error
// to 'stdout' and abort.  Otherwise, the new-delete allocator will be
// re-installed as the default allocator, and the program will proceed to the
// next block of code.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {


namespace bslma {

class Allocator;

                        // ===========================
                        // class DefaultAllocatorGuard
                        // ===========================

class DefaultAllocatorGuard {
    // Upon construction, an object of this class saves the current default
    // allocator and installs the user-specified allocator as the default
    // allocator.  On destruction, the original default allocator is restored.
    // Objects of this class are intended for testing purposes *only* and are
    // *not* guaranteed to be safe in production environments.

    Allocator *d_original_p;  // original (to be restored at destruction)

    // NOT IMPLEMENTED
    DefaultAllocatorGuard(const DefaultAllocatorGuard&);
    DefaultAllocatorGuard& operator=(const DefaultAllocatorGuard&);

  public:
    // CREATORS
    explicit
    DefaultAllocatorGuard(Allocator *temporary);
        // Create a scoped guard that installs the specified 'temporary'
        // allocator as the default allocator.  Note that the default allocator
        // is automatically restored to the original allocator on destruction.

    ~DefaultAllocatorGuard();
        // Restore the default allocator that was in place when this scoped
        // guard was created and destroy this guard.
};

}  // close package namespace


}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
