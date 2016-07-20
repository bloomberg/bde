// bslma_rawdeleterproctor.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMA_RAWDELETERPROCTOR
#define INCLUDED_BSLMA_RAWDELETERPROCTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a proctor to conditionally manage an object.
//
//@CLASSES:
//  bslma::RawDeleterProctor: proctor to conditionally manage an object
//
//@SEE_ALSO: bslma_rawdeleterguard, bslma_autorawdeleter
//
//@DESCRIPTION: This component provides a proctor class template,
// 'bslma::RawDeleterProctor', to conditionally manage an (otherwise-unmanaged)
// object of parameterized 'TYPE' supplied at construction.  If not explicitly
// released, the managed object is deleted automatically when the proctor
// object goes out of scope by first calling the (managed) object's destructor,
// and then freeing the memory using the parameterized 'ALLOCATOR' (allocator
// or pool) also supplied at construction.  Note that after a proctor object
// releases its managed object, the same proctor can be reused to conditionally
// manage another object (allocated from the same allocator or pool that was
// supplied at construction) by invoking the 'reset' method.
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
// The parameterized 'ALLOCATOR' type of the 'bslma::RawDeleterProctor' class
// template must provide a (possibly 'virtual') method:
//..
//  void deallocate(void *address);
//..
// to deallocate memory at the specified 'address' (originally supplied by the
// 'ALLOCATOR' object).
//
///Usage
///-----
// 'bslma::RawDeleterProctor' is normally used to achieve *exception* *safety*
// in an *exception* *neutral* way by managing objects that are created
// temporarily on the heap, but not yet committed to a container object's
// management.  This (somewhat contrived) example illustrates the use of a
// 'bslma::RawDeleterProctor' to manage a dynamically-allocated object,
// deleting the object automatically should an exception occur.
//
// Suppose we have a simple linked list class that manages objects of
// parameterized 'TYPE', but which are (for the purpose of this example)
// allocated separately from the links that hold them (thereby requiring two
// separate allocations for each 'append' operation):
//..
//  // my_list.h
//  // ...
//
//  template <class TYPE>
//  class my_List {
//      // This class is a container that uses a linked list data structure to
//      // manage objects of parameterized 'TYPE'.
//
//      // PRIVATE TYPES
//      struct Link {
//          TYPE *d_object_p;  // object held by the link
//          Link *d_next_p;    // next link
//      };
//
//      // DATA
//      Link             *d_head_p;       // head of list
//      Link             *d_tail_p;       // tail of list
//      int               d_length;       // number of objects
//      bslma::Allocator *d_allocator_p;  // allocator (held, not owned)
//
//    public:
//      // CREATORS
//      my_List(bslma::Allocator *basicAllocator = 0);
//          // Create a 'my_List' object having an initial length of 0.
//          // Optionally specify a 'basicAllocator' used to supply memory.  If
//          // 'basicAllocator' is 0, the currently installed default allocator
//          // is used.
//
//      // ...
//
//      ~my_List();
//          // Destroy this 'my_List' object and all elements currently stored.
//
//      // MANIPULATORS
//      // ...
//
//      void append(const TYPE& object);
//          // Append (a copy of) the specified 'object' of parameterized
//          // 'TYPE' to (the end of) this list.
//
//      // ...
//  };
//..
// Note that the rest of the 'my_List' interface (above) and implementation
// (below) are omitted as the portion shown is sufficient to demonstrate the
// use of 'bslma::RawDeleterProctor'.
//..
//  // CREATORS
//  template <class TYPE>
//  inline
//  my_List<TYPE>::my_List(bslma::Allocator *basicAllocator)
//  : d_head_p(0)
//  , d_tail_p(0)
//  , d_length(0)
//  , d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//  }
//
//  template <class TYPE>
//  my_List<TYPE>::~my_List()
//  {
//      while (d_head_p) {
//          Link *tmp = d_head_p;
//          d_head_p  = d_head_p->d_next_p;
//          d_allocator_p->deleteObject(tmp->d_object_p);
//          d_allocator_p->deallocate(tmp);
//      }
//  }
//
//  // MANIPULATORS
//  template <class TYPE>
//  void my_List<TYPE>::append(const TYPE& object)
//  {
//      TYPE *tmp = (TYPE *)new(*d_allocator_p) TYPE(object, d_allocator_p);
//                                                          // possibly throw
//
//      //************************************************************
//      // Note the use of the raw deleter proctor on 'tmp' (below). *
//      //************************************************************
//
//      bslma::RawDeleterProctor<TYPE, bslma::Allocator> proctor(tmp,
//                                                             d_allocator_p);
//
//      if (!d_head_p) {
//          d_head_p           = new(*d_allocator_p) Link;  // possibly throw
//          d_tail_p           = d_head_p;
//      }
//      else {
//          d_tail_p->d_next_p = new(*d_allocator_p) Link;  // possibly throw
//          d_tail_p           = d_tail_p->d_next_p;
//      }
//      d_tail_p->d_object_p   = tmp;
//      d_tail_p->d_next_p     = 0;
//
//      //*********************************************************
//      // Note that the raw deleter proctor is released (below). *
//      //*********************************************************
//
//      proctor.release();
//  }
//..
// The 'append' method defined above potentially throws in three places.  If
// the memory allocator held in 'd_allocator_p' were to throw while attempting
// to create the object of parameterized 'TYPE', no memory would be leaked.
// But without subsequent use of the 'bslma::RawDeleterProctor', if the
// allocator subsequently throws while creating the link, all memory (and any
// other resources) acquired as a result of copying the (not-yet-managed)
// object would be leaked.  Using the 'bslma::RawDeleterProctor' prevents the
// leaks by deleting the proctored object automatically should the proctor go
// out of scope before the 'release' method of the proctor is called (such as
// when the function exits prematurely due to an exception).
//
// Note that the 'append' method assumes the copy constructor of 'TYPE' takes
// an allocator as a second argument.  In production code, a constructor proxy
// that checks the traits of 'TYPE' (to determine whether 'TYPE' uses
// 'bslma::Allocator') should be used (see 'bslalg_constructorproxy').

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_DELETERHELPER
#include <bslma_deleterhelper.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

namespace BloombergLP {

namespace bslma {

                        // =======================
                        // class RawDeleterProctor
                        // =======================

template <class TYPE, class ALLOCATOR>
class RawDeleterProctor {
    // This class implements a proctor that, unless its 'release' method has
    // previously been invoked, automatically deletes a managed object upon
    // destruction by first invoking the object's destructor, and then invoking
    // the 'deallocate' method of an allocator (or pool) of parameterized
    // 'ALLOCATOR' type supplied to it at construction.  The managed object of
    // parameterized 'TYPE' must have been created using memory provided by
    // this allocator (or pool), which must remain valid throughout the
    // lifetime of the proctor object.

    // DATA
    TYPE      *d_object_p;     // managed object
    ALLOCATOR *d_allocator_p;  // allocator or pool (held, not owned)

    // NOT IMPLEMENTED
    RawDeleterProctor(const RawDeleterProctor&);
    RawDeleterProctor& operator=(const RawDeleterProctor&);

  public:
    // CREATORS
    RawDeleterProctor(TYPE *object, ALLOCATOR *allocator);
        // Create a raw deleter proctor that conditionally manages the
        // specified 'object' (if non-zero), and that uses the specified
        // 'allocator' to delete the object managed by this proctor (if not
        // released -- see 'release') upon destruction.  The behavior is
        // undefined unless 'allocator' is non-zero and supplied the memory for
        // 'object' (if non-zero).  Note that 'allocator' must remain valid
        // throughout the lifetime of this proctor.

    ~RawDeleterProctor();
        // Destroy this raw deleter proctor, and delete the object it manages
        // (if any) by first invoking the destructor of the (managed) object,
        // and then invoking the 'deallocate' method of the allocator (or pool)
        // that was supplied at the construction of this proctor.  If no object
        // is currently being managed, this method has no effect.

    // MANIPULATORS
    void release();
        // Release from management the object currently managed by this
        // proctor.  If no object is currently being managed, this method has
        // no effect.

    void reset(TYPE *object);
        // Set the specified 'object' as the object to be managed by this
        // proctor.  The behavior is undefined unless 'object' is non-zero and
        // was allocated from the allocator (or pool) supplied at construction.
        // Note that this method releases any previously-managed object from
        // management (without deleting it), and so may be invoked with or
        // without having called 'release' when reusing this object.
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                        // -----------------------
                        // class RawDeleterProctor
                        // -----------------------

// CREATORS
template <class TYPE, class ALLOCATOR>
inline
RawDeleterProctor<TYPE, ALLOCATOR>::
RawDeleterProctor(TYPE *object, ALLOCATOR *allocator)
: d_object_p(object)
, d_allocator_p(allocator)
{
    BSLS_ASSERT_SAFE(allocator);
}

template <class TYPE, class ALLOCATOR>
inline
RawDeleterProctor<TYPE, ALLOCATOR>::~RawDeleterProctor()
{
    BSLS_ASSERT_SAFE(d_allocator_p);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 != d_object_p)) {
        DeleterHelper::deleteObjectRaw(d_object_p, d_allocator_p);
    }
}

// MANIPULATORS
template <class TYPE, class ALLOCATOR>
inline
void RawDeleterProctor<TYPE, ALLOCATOR>::release()
{
    d_object_p = 0;
}

template <class TYPE, class ALLOCATOR>
inline
void RawDeleterProctor<TYPE, ALLOCATOR>::reset(TYPE *object)
{
    BSLS_ASSERT_SAFE(object);

    d_object_p = object;
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
