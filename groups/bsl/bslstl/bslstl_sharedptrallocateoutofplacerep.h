// bslstl_sharedptrallocateoutofplacerep.h                            -*-C++-*-
#ifndef INCLUDED_BSLSTL_SHAREDPTRALLOCATEOUTOFPLACEREP
#define INCLUDED_BSLSTL_SHAREDPTRALLOCATEOUTOFPLACEREP

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide an out-of-place implementation of 'bslma::SharedPtrRep'.
//
//@CLASSES:
//  bslstl::SharedPtrAllocateOutofplaceRep: out-of-place 'shared_ptr' imp.
//
//@SEE_ALSO: bslma_sharedptrrep, bslma_sharedptroutofplacerep, bslstl_sharedptr
//
//@DESCRIPTION: This component provides a class template,
// 'bslstl::SharedPtrAllocateOutofplaceRep' , which is a concrete
// implementation of 'bslma::SharedPtrRep' for managing objects of the
// parameterized 'TYPE' that are stored outside of the representation.  When
// all references to the out-of-place object are released using 'releaseRef',
// the deleter of the parameterized 'DELETER' type is invoked to delete the
// shared object.  Memory is supplied and reclaimed by an allocator of the
// parameterized 'ALLOCATOR' type.
//
///Thread Safety
///-------------
// 'bslstl::SharedPtrAllocateOutofplaceRep' is thread-safe provided that
// 'disposeObject' and 'disposeRep' are not called explicitly, meaning that all
// non-creator operations other than 'disposeObject' and 'disposeRep' on a
// given instance can be safely invoked simultaneously from multiple threads
// ('disposeObject' and 'disposeRep' are meant to be invoked only by
// 'releaseRef' and 'releaseWeakRef').  Note that there is no thread safety
// guarantees for operations on the managed object.
//
///Deleters
///--------
// When the last shared reference to a shared object is released, the object is
// destroyed using the "deleter" provided when the associated shared pointer
// representation was created.  'bslstl::SharedPtrAllocateOutofplaceRep'
// supports two kinds of "deleter" objects, which vary in how they are invoked.
// A "function-like" deleter is any language entity that can be invoked such
// that the expression 'deleterInstance(objectPtr)' is a valid expression, and
// a "factory" deleter is any language entity that can be invoked such that the
// expression 'deleterInstance.deleteObject(objectPtr)' is a valid expression,
// where 'deleterInstance' is an instance of the "deleter" object, and
// 'objectPtr' is a pointer to the shared object.  In summary:
//..
//  Deleter                     Expression used to destroy 'objectPtr'
//  - - - - - - - -             - - - - - - - - - - - - - - - - - - -
//  "function-like"             deleterInstance(objectPtr);
//  "factory"                   deleterInstance.deleteObject(objectPtr);
//..
// The following are examples of function-like deleters that delete an object
// of 'MyType':
//..
//  void deleteObject(MyType *object);
//      // Delete the specified 'object'.
//
//  void releaseObject(MyType *object);
//      // Release the specified 'object'.
//
//  struct FunctionLikeDeleterObject {
//      // This 'struct' provides an 'operator()' that can be used to delete a
//      // 'MyType' object.
//
//      void operator()(MyType *object);
//          // Destroy the specified 'object'.
//  };
//..
// The following on the other hand is an example of a factory deleter:
//..
//  class MyFactory {
//
//     // . . .
//
//     // MANIPULATORS
//     MyType *createObject(bslma::Allocator *basicAllocator = 0);
//         // Create a 'MyType' object.  Optionally specify a 'basicAllocator'
//         // used to supply memory.  If 'basicAllocator' is 0, the currently
//         // installed default allocator is used.
//
//     void deleteObject(MyType *object);
//         // Delete the specified 'object'.
//  };
//..
// Note that 'deleteObject' is provided by all 'bslma' allocators and by any
// object that implements the 'bdlma::Deleter' protocol.  Thus, any of these
// objects can be used as a factory deleter.  The purpose of this design is to
// allow 'bslma' allocators and factories to be used seamlessly as deleters.
//
// The selection of which expression is used by
// 'bslstl::SharedPtrAllocateOutofplaceRep' to destroy a shared object is based
// on how the deleter is passed to the shared pointer object: Deleters that are
// passed by *address* are assumed to be factory deleters, while those that are
// passed by *value* are assumed to be function-like.  Note that if the wrong
// interface is used for a deleter, i.e., if a function-like deleter is passed
// by pointer, or a factory deleter is passed by value, and the expression used
// to delete the object is invalid, a compiler diagnostic will be emitted
// indicating the error.
//
///Usage
///-----
// The following example demonstrates how to implement a shared
// 'bdlt::Datetime' object using 'bslstl::SharedPtrAllocateOutofplaceRep':
//..
//  class MySharedDatetimePtr {
//      // This class provide a reference counted smart pointer to support
//      // shared ownership of a 'bdlt::Datetime' object.
//
//    private:
//      bdlt::Datetime      *d_ptr_p;  // pointer to the managed object
//      bslma::SharedPtrRep *d_rep_p;  // pointer to the representation object
//
//    private:
//      // NOT IMPLEMENTED
//      MySharedDatetimePtr& operator=(const MySharedDatetimePtr&);
//
//    public:
//      // CREATORS
//      MySharedDatetimePtr(bdlt::Datetime   *ptr,
//                          bslma::Allocator *basicAllocator = 0);
//          // Create a 'MySharedDatetimePtr' object to managed the specified
//          // 'ptr'.  Optionally specify an 'basicAllocator' to allocate and
//          // deallocate the internal representation and to destroy 'ptr' when
//          // all references have been released.  The behavior is undefined
//          // unless 'ptr' was allocated using memory supplied by
//          // 'basicAllocator'.
//
//      MySharedDatetimePtr(const MySharedDatetimePtr& original);
//          // Create a shared datetime that refers to the same object managed
//          // by the specified 'original'
//
//      ~MySharedDatetimePtr();
//          // Destroy this shared datetime and release the reference to the
//          // 'bdlt::Datetime' object to which it might be referring.  If this
//          // is the last shared reference, deleted the managed object.
//
//      // MANIPULATORS
//      bdlt::Datetime& operator*() const;
//          // Return a reference offering modifiable access to the shared
//          // datetime.
//
//      bdlt::Datetime *operator->() const;
//          // Return the address of the modifiable 'bdlt::Datetime' to which
//          // this object refers.
//
//      bdlt::Datetime *ptr() const;
//          // Return the address of the modifiable 'bdlt::Datetime' to which
//          // this object refers.
//  };
//..
// Finally, we define the implementation.
//..
//  MySharedDatetimePtr::MySharedDatetimePtr(bdlt::Datetime   *ptr,
//                                           bslma::Allocator *basicAllocator)
//  {
//      d_ptr_p = ptr;
//      d_rep_p = bslstl::SharedPtrAllocateOutofplaceRep<bdlt::Datetime,
//                                                       bslma::Allocator *>::
//                      makeOutofplaceRep(ptr, basicAllocator, basicAllocator);
//  }
//
//  MySharedDatetimePtr::MySharedDatetimePtr(
//                                         const MySharedDatetimePtr& original)
//  : d_ptr_p(original.d_ptr_p)
//  , d_rep_p(original.d_rep_p)
//  {
//      if (d_ptr_p) {
//          d_rep_p->acquireRef();
//      } else {
//          d_rep_p = 0;
//      }
//  }
//
//  MySharedDatetimePtr::~MySharedDatetimePtr()
//  {
//      if (d_rep_p) {
//          d_rep_p->releaseRef();
//      }
//  }
//
//  bdlt::Datetime& MySharedDatetimePtr::operator*() const {
//      return *d_ptr_p;
//  }
//
//  bdlt::Datetime *MySharedDatetimePtr::operator->() const {
//      return d_ptr_p;
//  }
//
//  bdlt::Datetime *MySharedDatetimePtr::ptr() const {
//      return d_ptr_p;
//  }
//..

#include <bslscm_version.h>

#include <bslma_allocatortraits.h>
#include <bslma_sharedptrrep.h>

#include <bsls_util.h>

#include <typeinfo>

namespace BloombergLP {
namespace bslstl {

                 // ====================================
                 // class SharedPtrAllocateOutofplaceRep
                 // ====================================

template <class TYPE, class DELETER, class ALLOCATOR>
class SharedPtrAllocateOutofplaceRep : public BloombergLP::bslma::SharedPtrRep
                                                                              {
    // This class provides a concrete implementation of the 'SharedPtrRep'
    // protocol for out-of-place instances of the parameterized 'TYPE'.  Upon
    // destruction of this object, the parameterized 'DELETER' type is invoked
    // on the pointer to the shared object.

    // PRIVATE TYPES
    typedef bsl::allocator_traits<ALLOCATOR> OriginalTraits;

    typedef typename
         OriginalTraits::template rebind_traits<SharedPtrAllocateOutofplaceRep>
                                                               AllocatorTraits;
    typedef typename AllocatorTraits::allocator_type Allocator;

    // DATA
    TYPE      *d_ptr_p;     // pointer to out-of-place object (held, not owned)
    DELETER    d_deleter;   // deleter for this out-of-place instance
    Allocator  d_allocator; // copy of the allocator for this instance

  private:
    // NOT IMPLEMENTED
    SharedPtrAllocateOutofplaceRep(const SharedPtrAllocateOutofplaceRep&);
    SharedPtrAllocateOutofplaceRep& operator=(
                                        const SharedPtrAllocateOutofplaceRep&);

    // PRIVATE CREATORS
    SharedPtrAllocateOutofplaceRep(TYPE             *ptr,
                                   const DELETER&    deleter,
                                   const ALLOCATOR&  basicAllocator);
        // Create a 'SharedPtrAllocateOutofplaceRep' that manages the lifetime
        // of the specified 'ptr', using the specified 'deleter' to destroy
        // 'ptr', and using the specified 'basicAllocator' to supply memory.
        // Note that 'basicAllocator' will be used to destroy this
        // representation object, but not necessarily to destroy 'ptr'.  Also
        // note that a 'SharedPtrAllocateOutofplaceRep' must be created using
        // 'makeOutofplaceRep', which will call the private constructor.

    ~SharedPtrAllocateOutofplaceRep();
        // Destroy this representation object and if the shared object has not
        // been deleted, delete the shared object using the associated deleter.
        // Note that this destructor is never called explicitly.  Instead,
        // 'disposeObject' destroys the shared object and 'disposeRep'
        // deallocates this representation object.

  public:
    // CLASS METHODS
    static SharedPtrAllocateOutofplaceRep *makeOutofplaceRep(
                                             TYPE             *ptr,
                                             const DELETER&    deleter,
                                             const ALLOCATOR&  basicAllocator);
        // Return the address of a newly created
        // 'SharedPtrAllocateOutofplaceRep' object that manages the lifetime of
        // the specified 'ptr', uses the specified 'deleter' to destroy 'ptr',
        // and uses the specified 'basicAllocator' to supply memory.  Note that
        // the parameterized 'DELETER' type will be used to deallocate the
        // memory pointed to by 'ptr'.

    // MANIPULATORS
    virtual void disposeObject();
        // Destroy the object referred to by this representation.  This method
        // is invoked by 'releaseRef' when the number of shared references
        // reaches zero and should not be explicitly invoked otherwise.

    virtual void disposeRep();
        // Destroy this representation object and deallocate the associated
        // memory.  This method is invoked by 'releaseRef' and 'releaseWeakRef'
        // when the number of weak references and the number of shared
        // references both reach zero and should not be explicitly invoked
        // otherwise.  The behavior is undefined unless 'disposeObject' has
        // already been called for this representation.  Note that this method
        // effectively serves as the representation object's destructor.

    virtual void *getDeleter(const std::type_info& type);
        // Return a pointer to the deleter stored by the derived representation
        // if the deleter has the same type as that described by the specified
        // 'type', and a null pointer otherwise.

    // ACCESSORS
    virtual void *originalPtr() const;
        // Return the (untyped) address of the modifiable shared object to
        // which this object refers.

    TYPE *ptr() const;
        // Return the address of the modifiable shared object to which this
        // object refers.
};

               // =================================================
               // struct SharedPtrAllocateOutofplaceRep_InitProctor
               // =================================================

template <class TYPE, class DELETER>
class SharedPtrAllocateOutofplaceRep_InitProctor {
    // This proctor is used for out-of-place shared pointer instantiations.
    // Generally, a proctor is created prior to constructing a
    // 'SharedPtrAllocateOutofplaceRep' and released after successful
    // construction.  In the event that an exception is thrown during
    // construction of the representation, the proctor will delete the provided
    // pointer using the provided deleter.  Note that the provided deleter is
    // held by reference and must remain valid for the lifetime of the proctor.
    // If the proctor is not released before it's destruction, a copy of the
    // deleter is instantiated to delete the pointer (in case 'operator()' is
    // non-'const').  Also note that if the deleter throws during
    // copy construction, the provided pointer will not be destroyed.

    // DATA
    TYPE           *d_ptr_p;    // address of the managed object (held, not
                                // owned)

    const DELETER&  d_deleter;  // deleter used to destroy managed object

  private:
    // NOT IMPLEMENTED
    SharedPtrAllocateOutofplaceRep_InitProctor(
                const SharedPtrAllocateOutofplaceRep_InitProctor&); // = delete
    SharedPtrAllocateOutofplaceRep_InitProctor& operator=(
                const SharedPtrAllocateOutofplaceRep_InitProctor&); // = delete

  public:
    // CREATORS
    SharedPtrAllocateOutofplaceRep_InitProctor(TYPE           *ptr,
                                               const DELETER&  deleter);
        // Create a proctor managing the specified 'ptr' and using the
        // specified 'deleter' to destroy 'ptr' when the proctor is destroyed,
        // unless it has been released from management by a call to 'release'.

    ~SharedPtrAllocateOutofplaceRep_InitProctor();
        // Destroy this proctor and the object (if any) managed by this
        // proctor.

    // MANIPULATORS
    void release();
        // Release from management the object referred to by this proctor.
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                  // ------------------------------------
                  // class SharedPtrAllocateOutofplaceRep
                  // ------------------------------------

// PRIVATE CREATORS
template <class TYPE, class DELETER, class ALLOCATOR>
inline
SharedPtrAllocateOutofplaceRep<TYPE, DELETER, ALLOCATOR>::
SharedPtrAllocateOutofplaceRep(TYPE             *ptr,
                               const DELETER&    deleter,
                               const ALLOCATOR&  basicAllocator)
: d_ptr_p(ptr)
, d_deleter(deleter)
, d_allocator(basicAllocator)
{
}

template <class TYPE, class DELETER, class ALLOCATOR>
inline
SharedPtrAllocateOutofplaceRep<TYPE, DELETER, ALLOCATOR>::
~SharedPtrAllocateOutofplaceRep()
{
}

// CLASS METHODS
template <class TYPE, class DELETER, class ALLOCATOR>
SharedPtrAllocateOutofplaceRep<TYPE, DELETER, ALLOCATOR> *
SharedPtrAllocateOutofplaceRep<TYPE, DELETER, ALLOCATOR>::makeOutofplaceRep(
                                              TYPE             *ptr,
                                              const DELETER&    deleter,
                                              const ALLOCATOR&  basicAllocator)
{
    SharedPtrAllocateOutofplaceRep_InitProctor<TYPE, DELETER> proctor(ptr,
                                                                      deleter);

    Allocator alloc(basicAllocator);

    SharedPtrAllocateOutofplaceRep *rep = AllocatorTraits::allocate(alloc, 1);
    new (rep) SharedPtrAllocateOutofplaceRep(ptr, deleter, alloc);

    proctor.release();

    return rep;
}

// MANIPULATORS
template <class TYPE, class DELETER, class ALLOCATOR>
inline
void SharedPtrAllocateOutofplaceRep<TYPE, DELETER, ALLOCATOR>::disposeObject()
{
    d_deleter(d_ptr_p);
    d_ptr_p = 0;
}

template <class TYPE, class DELETER, class ALLOCATOR>
inline
void SharedPtrAllocateOutofplaceRep<TYPE, DELETER, ALLOCATOR>::disposeRep()
{
    Allocator alloc(d_allocator);
    this->~SharedPtrAllocateOutofplaceRep();
    AllocatorTraits::deallocate(alloc, this, 1);
}

template <class TYPE, class DELETER, class ALLOCATOR>
inline
void *
SharedPtrAllocateOutofplaceRep<TYPE, DELETER, ALLOCATOR>::getDeleter(
                                                    const std::type_info& type)
{
    return typeid(d_deleter) == type
         ? bsls::Util::addressOf(d_deleter)
         : 0;
}

// ACCESSORS
template <class TYPE, class DELETER, class ALLOCATOR>
inline
void *
SharedPtrAllocateOutofplaceRep<TYPE, DELETER, ALLOCATOR>::originalPtr() const
{
    return const_cast<void *>(static_cast<const void *>(d_ptr_p));
}

template <class TYPE, class DELETER, class ALLOCATOR>
inline
TYPE *SharedPtrAllocateOutofplaceRep<TYPE, DELETER, ALLOCATOR>::ptr() const
{
    return d_ptr_p;
}

               // ------------------------------------------
               // SharedPtrAllocateOutofplaceRep_InitProctor
               // ------------------------------------------

// CREATORS
template <class TYPE, class DELETER>
inline
SharedPtrAllocateOutofplaceRep_InitProctor<TYPE, DELETER>::
SharedPtrAllocateOutofplaceRep_InitProctor(TYPE           *ptr,
                                           const DELETER&  deleter)
: d_ptr_p(ptr)
, d_deleter(deleter)
{
}

template <class TYPE, class DELETER>
inline
SharedPtrAllocateOutofplaceRep_InitProctor<TYPE, DELETER>::
~SharedPtrAllocateOutofplaceRep_InitProctor()
{
    // The proctor must destroy 'd_ptr_p' to avoid a leak, but is not subject
    // to the reference-counting of null pointers principle, where the deleter
    // is called on destroying the last reference, even if 'd_ptr_p' is null.

    if (d_ptr_p) {
        d_deleter(d_ptr_p);
    }
}

// MANIPULATORS
template <class TYPE, class DELETER>
inline
void SharedPtrAllocateOutofplaceRep_InitProctor<TYPE, DELETER>::release()
{
    d_ptr_p = 0;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
