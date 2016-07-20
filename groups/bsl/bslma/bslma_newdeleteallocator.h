// bslma_newdeleteallocator.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMA_NEWDELETEALLOCATOR
#define INCLUDED_BSLMA_NEWDELETEALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide singleton new/delete adaptor to 'bslma::Allocator' protocol.
//
//@CLASSES:
//  bslma::NewDeleteAllocator: support new/delete-style allocation/deallocation
//
//@SEE_ALSO: bslma_default, bslma_testallocator
//
//@DESCRIPTION: This component provides a concrete allocation mechanism,
// 'bslma::NewDeleteAllocator', that implements the 'bslma::Allocator' protocol
// to provide direct access to the system-supplied (native) global
// 'operator new' and 'operator delete' functions via that pure abstract
// interface.
//..
//   ,-------------------------.
//  ( bslma::NewDeleteAllocator )
//   `-------------------------'
//               |         allocator
//               |         singleton
//               |         ctor/dtor
//               V
//       ,----------------.
//      ( bslma::Allocator )
//       `----------------'
//                       allocate
//                       deallocate
//..
// The essential purpose of this component is to facilitate the default use of
// global 'new' and 'delete' in all components that accept a user-supplied
// allocator derived from 'bslma::Allocator' (see 'bslma_default').  Hence, the
// global 'operator new' and 'operator delete' functions are wrapped within
// concrete methods of a derived 'bslma::NewDeleteAllocator' class.  A 'static'
// (factory) method of 'bslma::NewDeleteAllocator' can be used to obtain a
// unique (singleton) 'bslma::NewDeleteAllocator' object for the given process,
// and whose lifetime is guaranteed to exceed any possibility of its use.  Note
// that the standard also requires the global 'operator new' to return
// maximally-aligned memory, which is a stricter post condition than the
// natural-alignment requirement imposed by the base-class contract, or than is
// provided by many other concrete implementations.
//
///Thread Safety
///-------------
// This class is fully thread-safe, which means that all non-creator object
// methods can be safely accessed concurrently (from multiple treads).  The
// singleton 'bslma::NewDeleteAllocator' can also be safely created/accessed
// concurrently (from multiple threads) via either the 'static' 'singleton' or
// 'allocator' (factory) methods.  Moreover, the underlying (native)
// implementation of 'new' and 'delete' are required by the C++ standard to
// ensure that concurrent access to either the virtual 'allocate' and/or
// 'deallocate' are also safe (i.e., will not not result in heap corruption).
// Note that this allocator therefore has stronger thread-safety guarantees
// than is required by the base-class contract or than is provided by many
// other derived concrete allocators.
//
///Usage
///-----
// The most common and proper use of 'bslma::NewDeleteAllocator' is both
// *indirect* and *by* *default* (see 'bslma_default').  For example, consider
// (along with its destructor) the default and copy constructors for, say, a
// simple container, such as 'my_ShortArray', each of which take as its final
// optional argument the address of a 'bslma::Allocator' protocol:
//..
//  // my_shortarray.h:
//  // ...
//  namespace bslma { class Allocator; }
//
//  class my_ShortArray {
//      short            *d_array_p;     // dynamically-allocated array
//      int               d_capacity;    // physical capacity (in elements)
//      int               d_length;      // logical length (in elements)
//      bslma::Allocator *d_allocator_p; // memory allocator (not owned)
//
//    public:
//      my_ShortArray(bslma::Allocator *basicAllocator = 0);
//          // Create an empty 'my_shortArray'.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator'
//          // is 0, the currently installed default allocator is used.
//
//      my_ShortArray(const my_ShortArray&  other,
//                    bslma::Allocator     *basicAllocator = 0);
//          // Create a 'bslma::ShortArray' having the same value as the
//          // specified 'other' array.  Optionally specify a 'basicAllocator'
//          // used to supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      ~my_ShortArray();
//          // Destroy this object.
//
//     // ...
//  };
//
//  // ...
//..
// In order to satisfy this contract, we will need a globally accessible
// utility (see 'bslma_default'), which by default returns the singleton
// 'bslma::NewDeleteAllocator', but which could be configured to return some
// other allocator, say a *test* allocator (see 'bslma_testallocator'):
//..
//  // my_default.h:
//  // ...
//  namespace bslma { class Allocator; }
//
//  struct my_Default {
//      // This class maintains a process-wide 'bslma::Allocator' object
//      // to be used when an allocator is needed, and not suppled explicitly.
//      // By default, the currently installed default allocator is the unique
//      // 'bslma::NewDeleteAllocator' object returned by the 'static' method,
//      // 'bslma::NewDeleteAllocator::singleton()'.  Note that the default
//      // allocator will exist longer than any possibility of its use.
//
//      static bslma::Allocator *allocator(bslma::Allocator *basicAllocator);
//          // Return the address of the specified modifiable
//          // 'basicAllocator' or, if 'basicAllocator' is 0, an instance of
//          // the currently installed default 'bslma::Allocator' object, which
//          // will exist longer than any possibility of its use.  Note
//          // that this function can safely be called concurrently (from
//          // multiple threads).
//
//      static bslma::Allocator *replace(bslma::Allocator *basicAllocator);
//          // Replace the address of the currently installed allocator with
//          // that of the specified modifiable 'basicAllocator' (or if 0,
//          // with the "factory" default, 'bslma::NewDeleteAllocator'), and
//          // return the address of the previous allocator.  The behavior is
//          // undefined unless 'basicAllocator' will exist longer than any
//          // possibility of its use.  Note that this function is *not* *at*
//          // *all* thread-safe, and should *never* be called when multiple
//          // threads are active.
//  };
//
//  // my_default.cpp:
//  // ...
//
//  #include <my_default.h>
//
//  static bslma::Allocator *s_default_p = 0; // load-time initialized
//
//  bslma::Allocator *my_Default::allocator(bslma::Allocator *basicAllocator)
//  {
//      return bslma::NewDeleteAllocator::allocator(s_default_p);
//  }
//
//  bslma::Allocator *my_Default::replace(bslma::Allocator *basicAllocator)
//  {
//      bslma::Allocator *tmp =
//                           bslma::NewDeleteAllocator::allocator(s_default_p);
//      s_default_p = bslma::NewDeleteAllocator::allocator(basicAllocator);
//      return tmp;
//  }
//..
// Notice that the only part of the 'bslma::NewDeleteAllocator' class we used
// directly was its static 'allocator' method, which -- in addition to safely
// constructing the singleton 'bslma::NewDeleteAllocator' object on first
// access -- also automatically replaces a 0 address value with that of
// singleton 'bslma::NewDeleteAllocator' object.  From now on, we will never
// again need to invoke the 'bslma_newdeleteallocator' component's interface
// directly, but instead use it through 'my_Default' (see 'bslma::Default' for
// what is actually used in practice).
//
// Turning back to our 'my_shortarray' example, let's now implement the two
// constructors using the 'bslma_newdeleteallocator' component indirectly via
// the 'my_default' component:
//..
//  // my_shortarray.cpp:
//  #include <my_shortarray.h>
//  #include <my_default.h>
//  #include <bsls_assert.h>
//
//  // ...
//
//  enum {
//      INITIAL_CAPACITY = 0, // recommended to avoid unnecessary allocations
//                            // possibly resulting in locking and extra thread
//                            // contention for the 'bslma::NewDeleteAllocator'
//
//      GROW_FACTOR = 2       // typical value for geometric growth
//  };
//
//  // ...
//
//  my_ShortArray::my_ShortArray(bslma::Allocator *basicAllocator)
//  : d_capacity(INITIAL_CAPACITY)
//  , d_length(0)
//  , d_allocator_p(my_Default::allocator(basicAllocator))
//  {
//      assert(d_allocator_p);
//      d_array_p = (short *)  // no thread contention if 'd_capacity' is 0
//                  d_allocator_p->allocate(d_capacity * sizeof *d_array_p);
//      assert(0 == d_array_p);
//  }
//
//  my_ShortArray::my_ShortArray(const my_ShortArray&   other,
//                               bslma::Allocator      *basicAllocator)
//  : d_capacity(other.d_capacity)
//  , d_length(other.d_length)
//  , d_allocator_p(my_Default::allocator(basicAllocator))
//  {
//      assert(d_allocator_p);
//      d_array_p = (short *)
//                  d_allocator_p->allocate(d_capacity * sizeof *d_array_p);
//      assert(!d_capacity == !d_array_p);
//      memcpy(d_array_p, other.d_array_p, d_length * sizeof *d_array_p);
//  }
//
//  my_ShortArray::~my_ShortArray()
//  {
//      d_allocator_p->deallocate(d_array_p); // no locking if 'd_array_p' is 0
//  }
//
// // ...
//..
// When the default constructor is called, the default capacity and length are
// recorded in data members via the initialization list.  The static function
// 'allocator' (provided in 'my_Default') is used to assign the value of the
// 'basicAllocator' address passed in, or if that is 0, the address of the
// currently installed default allocator, which by default is the singleton
// object of type 'bslma::NewDeleteAllocator', defined in this component.  Note
// that since 'INITIAL_CAPACITY' is 0, a default constructed object that is
// created using a 'bslma::NewDeleteAllocator' will *not* invoke the
// 'operator new' function, which on some platforms may needlessly acquire a
// lock, causing unnecessary overhead (the same potential overhead is avoided
// for 'operator delete' whenever a 0 'd_array_p' value is deallocated in the
// destructor) and 'd_allocator_p' refers to a 'bslma::NewDeleteAllocator'.
// Note also that, for the copy constructor, the currently installed default
// allocator, and not the 'other' array's allocator is used whenever
// 'basicAllocator' is 0 or not explicitly supplied.
//
// Finally note that this entire component is *not* intended for direct use by
// typical clients: See 'bslma_default' for more information or proper usage.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

namespace BloombergLP {

namespace bslma {

                        // ========================
                        // class NewDeleteAllocator
                        // ========================

class NewDeleteAllocator : public Allocator {
    // This class defines a concrete mechanism that adapts the system-supplied
    // (native) global 'operator new' and 'operator delete' to the 'Allocator'
    // protocol.  The class method 'singleton' returns a process-wide unique
    // object of this class whose lifetime is guaranteed to extend from the
    // first call to 'singleton' until the program terminates.  A second class
    // method, 'allocator', allows for conveniently replacing a "null"
    // allocator with this singleton object.  Note that this entire class
    // should generally not be used directly by typical clients (see
    // 'bslma_default' for more information).

  private:
    // NOT IMPLEMENTED
    NewDeleteAllocator(const NewDeleteAllocator&);
    NewDeleteAllocator& operator=(const NewDeleteAllocator&);

  public:
    // CLASS METHODS
    static NewDeleteAllocator& singleton();
        // Return a reference to a process-wide unique object of this class.
        // The lifetime of this object is guaranteed to extend from the first
        // call of this method until the program terminates.  Note that this
        // method should generally not be used directly by typical clients (see
        // 'bslma_default' for more information).

    static Allocator *allocator(Allocator *basicAllocator);
        // Return the address of the specified modifiable 'basicAllocator' or,
        // if 'basicAllocator' is 0, the process-wide unique (see 'singleton')
        // object of this class.  Note that the behavior of this function is
        // equivalent to the following expression:
        //..
        //  basicAllocator
        //  ? basicAllocator
        //  : &NewDeleteAllocator::singleton()
        //..
        // Also note that if a 'NewDeleteAllocator' object is supplied, it is
        // owned by the class and must NOT be deleted.  Finally note that this
        // method should generally not be called directly by typical clients
        // (see 'bslma_default' for more information).

    // CREATORS
    NewDeleteAllocator();
        // Create a ("stateless") new-delete-allocator object that wraps the
        // global native 'operator new' and 'operator delete' functions in
        // order to supply memory via the 'Allocator' protocol.  Note that all
        // objects of this class share the same underlying resource; hence,
        // this constructor should generally not be invoked directly by
        // clients; instead, consider using the 'static' 'singleton' or
        // 'allocator' (factory) methods, or -- better -- the appropriate ones
        // in 'Default' (see 'bslma_default' for more information).

    virtual ~NewDeleteAllocator();
        // Destroy this allocator object.  Note that destroying this allocator
        // has no effect on any outstanding allocated memory.

    // MANIPULATORS
    virtual void *allocate(size_type size);
        // Return a newly allocated block of memory of (at least) the specified
        // positive 'size' (in bytes).  If 'size' is 0, a null pointer is
        // returned with no other effect.  The alignment of the address
        // returned is the maximum alignment for any fundamental, pointer, or
        // enumerated type defined for this platform.  The behavior is
        // undefined unless '0 <= size'.  Note that global 'operator new' is
        // *not* called when 'size' is 0 (in order to avoid having to acquire a
        // lock, and potential contention in multi-threaded programs).

    virtual void deallocate(void *address);
        // Return the memory block at the specified 'address' back to this
        // allocator.  If 'address' is 0, this function has no effect.  The
        // behavior is undefined unless 'address' was allocated using this
        // allocator object and has not already been deallocated.  Note that
        // global 'operator delete' is *not* called when 'address' is 0 (in
        // order to avoid having to acquire a lock, and potential contention in
        // multi-threaded programs).
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                        // ------------------------
                        // class NewDeleteAllocator
                        // ------------------------

// CLASS METHODS
inline
Allocator *NewDeleteAllocator::allocator(Allocator *basicAllocator)
{
    return basicAllocator ? basicAllocator : &singleton();
}

// CREATORS
inline
NewDeleteAllocator::NewDeleteAllocator()
{
}

// MANIPULATORS
inline
void NewDeleteAllocator::deallocate(void *address)
{
    // While the C++ standard guarantees that calling delete(0) is safe
    // (3.7.3.2 paragraph 3), some libc implementations take out a lock to deal
    // with the free(0) case, so this check can improve efficiency of threaded
    // programs.

    if (address) {
        ::operator delete(address);
    }
}

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
