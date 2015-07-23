// bslma_rawdeleterguard.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMA_RAWDELETERGUARD
#define INCLUDED_BSLMA_RAWDELETERGUARD

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a guard to unconditionally manage an object.
//
//@CLASSES:
//  bslma::RawDeleterGuard: guard to unconditionally manage an object
//
//@SEE_ALSO: bslma_rawdeleterproctor, bslma_autorawdeleter
//
//@DESCRIPTION: This component provides a guard class template to
// unconditionally manage an (otherwise-unmanaged) object of parameterized
// 'TYPE' supplied at construction.  The managed object is deleted
// automatically when the guard object goes out of scope by first calling the
// (managed) object's destructor, and then freeing the memory using the
// parameterized 'ALLOCATOR' (allocator or pool) also supplied at construction.
//
///"Raw" Warning
///-------------
// Note that this component should be used only if we are sure that the
// supplied pointer is !not! of a type that is a secondary base class -- i.e.,
// the (managed) object's address is (numerically) the same as when it was
// originally dispensed by 'ALLOCATOR'.
//
///Requirement
///-----------
// The parameterized 'ALLOCATOR' type of the 'bslma::RawDeleterGuard' class
// template must provide a (possibly 'virtual') method:
//..
//  void deallocate(void *address);
//..
// to deallocate memory at the specified 'address' (originally supplied by the
// 'ALLOCATOR' object).
//
///Usage
///-----
// This example shows how one might use a 'bslma::RawDeleterGuard' to guard a
// dynamically-allocated object, deleting that object automatically when the
// guard goes out of scope.
//
// Suppose we have a simple queue class that stores object values using an
// "out-of-place" representation (i.e., an array of dynamically-allocated
// object pointers):
//..
//  // my_queue.h
//  // ...
//
//  template <class TYPE>
//  class my_Queue {
//      // This class is a container that uses an "out-of-place"
//      // representation to manage objects of parameterized 'TYPE'.  Note
//      // that this class is implemented with the native version of 'deque',
//      // instead of the version provided in 'bslstl_Deque'.  This is so that
//      // a circular dependency in the physical hierarchy will not be created.
//
//      // DATA
//      std::deque<TYPE *>   d_objects;      // objects stored in the queue
//      bslma::Allocator    *d_allocator_p;  // allocator (held, not owned)
//
//    public:
//      // CREATORS
//      my_Queue(bslma::Allocator *basicAllocator = 0);
//          // Create a 'my_Queue' object.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      // ...
//
//      ~my_Queue();
//          // Destroy this 'my_Queue' object and all elements currently
//          // stored.
//
//      // MANIPULATORS
//
//      // ...
//
//      void pushBack(const TYPE& object);
//          // Push the value of the specified 'object' of parameterized 'TYPE'
//          // onto the back of this queue.
//
//      TYPE popFront();
//          // Remove and return (by value) the object of parameterized 'TYPE'
//          // that is currently at the front of this queue.
//
//      // ...
//  };
//..
// Note that the 'popFront' method returns an object by value because (1) there
// may be no reasonable default object to pass in, (2) there may be no
// reasonable copy-assignment semantics, or (3) it is simply more syntactically
// convenient (e.g., if, say, the queued objects are themselves pointers):
//..
//  // CREATORS
//  template <class TYPE>
//  inline
//  my_Queue<TYPE>::my_Queue(bslma::Allocator *basicAllocator)
//  : d_objects(basicAllocator)
//  , d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//  }
//
//  template <class TYPE>
//  my_Queue<TYPE>::~my_Queue()
//  {
//      for (int i = 0; i < d_objects.size(); ++i) {
//          d_allocator_p->deleteObjectRaw(d_objects[i]);
//      }
//  }
//..
// Note that the 'pushBack' method should be implemented with a constructor
// proxy that determines whether 'TYPE' takes an allocator at construction (see
// 'bslalg_constructorproxy').  However, for the purpose of this example, the
// implementation is simplified by assuming 'TYPE' takes an allocator.
//..
//  // MANIPULATORS
//  template <class TYPE>
//  inline
//  void my_Queue<TYPE>::pushBack(const TYPE& object)
//  {
//      TYPE *tmp = (TYPE *)new(*d_allocator_p) TYPE(object);
//      d_objects.push_back(tmp);
//  }
//
//  template <class TYPE>
//  inline
//  TYPE my_Queue<TYPE>::popFront()
//  {
//      TYPE *tmp = d_objects.front();
//      d_objects.pop_front();
//
//      //***********************************************************
//      //* Note the use of the raw deleter guard on 'tmp' (below). *
//      //***********************************************************
//
//      bslma::RawDeleterGuard<TYPE, bslma::Allocator>
//                                                   guard(tmp, d_allocator_p);
//
//      return *tmp;
//  }
//..
// The 'pushBack' method defined above stores a copy of the provided object.
// The 'popFront' method returns the leading object by value, and the
// 'bslma::RawDeleterGuard' is used to automatically delete the copy the queue
// manages when the guard goes out of scope (i.e., when the function returns).

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_DELETERHELPER
#include <bslma_deleterhelper.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

namespace bslma {

                        // =====================
                        // class RawDeleterGuard
                        // =====================

template <class TYPE, class ALLOCATOR>
class RawDeleterGuard {
    // This class implements a guard that unconditionally deletes a managed
    // object upon destruction by first invoking the object's destructor, and
    // then invoking the 'deallocate' method of an allocator (or pool) of
    // parameterized 'ALLOCATOR' type supplied to it at construction.  The
    // managed object of parameterized 'TYPE' must have been created using
    // memory provided by this allocator (or pool), which must remain valid
    // throughout the lifetime of the guard object.

    // DATA
    TYPE      *d_object_p;     // managed object
    ALLOCATOR *d_allocator_p;  // allocator or pool (held, not owned)

    // NOT IMPLEMENTED
    RawDeleterGuard(const RawDeleterGuard&);
    RawDeleterGuard& operator=(const RawDeleterGuard&);

  public:
    // CREATORS
    RawDeleterGuard(TYPE *object, ALLOCATOR *allocator);
        // Create a raw deleter guard that unconditionally manages the
        // specified 'object', and that uses the specified 'allocator' to
        // delete 'object' upon the destruction of this guard.  The behavior is
        // undefined unless 'object' and 'allocator' are non-zero, and
        // 'allocator' supplied the memory for 'object'.  Note that 'allocator'
        // must remain valid throughout the lifetime of this guard.

    ~RawDeleterGuard();
        // Destroy this raw deleter guard and delete the object it manages by
        // first invoking the destructor of the (managed) object, and then
        // invoking the 'deallocate' method of the allocator (or pool) that was
        // supplied with the object at construction.
};

// ============================================================================
//                      TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class RawDeleterGuard
                        // ---------------------

// CREATORS
template <class TYPE, class ALLOCATOR>
inline
RawDeleterGuard<TYPE, ALLOCATOR>::
RawDeleterGuard(TYPE *object, ALLOCATOR *allocator)
: d_object_p(object)
, d_allocator_p(allocator)
{
    BSLS_ASSERT_SAFE(object);
    BSLS_ASSERT_SAFE(allocator);
}

template <class TYPE, class ALLOCATOR>
inline
RawDeleterGuard<TYPE, ALLOCATOR>::~RawDeleterGuard()
{
    BSLS_ASSERT_SAFE(d_object_p);
    BSLS_ASSERT_SAFE(d_allocator_p);

    DeleterHelper::deleteObjectRaw(d_object_p, d_allocator_p);
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
