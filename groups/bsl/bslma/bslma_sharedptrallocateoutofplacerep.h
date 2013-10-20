// bslma_sharedptrallocateoutofplacerep.h                             -*-C++-*-
#ifndef INCLUDED_BSLMA_SHAREDPTRALLOCATEOUTOFPLACEREP
#define INCLUDED_BSLMA_SHAREDPTRALLOCATEOUTOFPLACEREP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide an out-of-place implementation of 'bslma::SharedPtrRep'.
//
//@CLASSES:
//  bslma::SharedPtrAllocateOutofplaceRep: out-of-place shared ptr implementation
//
//@AUTHOR: Alisdair Meredith (ameredit) 
//
//@SEE_ALSO: bslma_sharedptr, bslma_sharedptrrep, bslma_sharedptrinplacerep
//
//@DESCRIPTION: This component provides a concrete implementation of
// 'bslma::SharedPtrRep' for managing objects of the parameterized 'TYPE' that
// are stored outside of the representation.  When all references to the
// out-of-place object are released using 'releaseRef', the deleter of the
// parameterized 'DELETER' type is invoked to delete the shared object.
//
///Thread-Safety
///-------------
// 'bslma::SharedPtrAllocateOutofplaceRep' is thread-safe provided that 'disposeObject'
// and 'disposeRep' are not called explicitly, meaning that all non-creator
// operations other than 'disposeObject' and 'disposeRep' on a given instance
// can be safely invoked simultaneously from multiple threads ('disposeObject'
// and 'disposeRep' are meant to be invoked only by 'releaseRef' and
// 'releaseWeakRef').  Note that there is no thread safety guarantees for
// operations on the managed object.
//
///Deleters
///--------
// When the last shared reference to a shared object is released, the object is
// destroyed using the "deleter" provided when the associated shared pointer
// representation was created.  'bslma::SharedPtrAllocateOutofplaceRep' supports two
// kinds of "deleter" objects, which vary in how they are invoked.  A
// "function-like" deleter is any language entity that can be invoked such that
// the expression 'deleterInstance(objectPtr)' is a valid expression, and a
// "factory" deleter is any language entity that can be invoked such that the
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
//         // Create a 'MyType' object.  Optionally specify a
//         // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//         // 0, the currently installed default allocator is used.
//
//     void deleteObject(MyType *object);
//         // Delete the specified 'object'.
//  };
//..
// Note that 'deleteObject' is provided by all 'bslma' allocators and by any
// object that implements the 'bcema_Deleter' protocol.  Thus, any of these
// objects can be used as a factory deleter.  The purpose of this design is to
// allow 'bslma' allocators and factories to be used seamlessly as deleters.
//
// The selection of which expression is used by 'bslma::SharedPtrAllocateOutofplaceRep'
// to destroy a shared object is based on how the deleter is passed to the
// shared pointer object: Deleters that are passed by *address* are assumed to
// be factory deleters, while those that are passed by *value* are assumed to
// be function-like.  Note that if the wrong interface is used for a deleter,
// i.e., if a function-like deleter is passed by pointer, or a factory deleter
// is passed by value, and the expression used to delete the object is invalid,
// a compiler diagnostic will be emitted indicating the error.
//
///Usage
///-----
// The following example demonstrates how to implement a shared 'bdet_Datetime'
// object using 'bslma::SharedPtrAllocateOutofplaceRep':
//..
//  class MySharedDatetimePtr {
//      // This class provide a reference counted smart pointer to support
//      // shared ownership of a 'bdet_Datetime' object.
//
//    private:
//      bdet_Datetime      *d_ptr_p;  // pointer to the managed object
//      bslma::SharedPtrRep *d_rep_p;  // pointer to the representation object
//
//    private:
//      // NOT IMPLEMENTED
//      MySharedDatetimePtr& operator=(const MySharedDatetimePtr&);
//
//    public:
//      // CREATORS
//      MySharedDatetimePtr(bdet_Datetime    *ptr,
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
//          // 'bdet_Datetime' object to which it might be referring.  If this
//          // is the last shared reference, deleted the managed object.
//
//      // MANIPULATORS
//      bdet_Datetime& operator*() const;
//          // Return a modifiable reference to the shared datetime.
//
//      bdet_Datetime *operator->() const;
//          // Return the address of the modifiable 'bdet_Datetime' to which
//          // this object refers.
//
//      bdet_Datetime *ptr() const;
//          // Return the address of the modifiable 'bdet_Datetime' to which
//          // this object refers.
//  };
//..
// Finally, we define the implementation.
//..
//  MySharedDatetimePtr::MySharedDatetimePtr(bdet_Datetime    *ptr,
//                                           bslma::Allocator *basicAllocator)
//  {
//      d_ptr_p = ptr;
//      d_rep_p =
//           bslma::SharedPtrAllocateOutofplaceRep<bdet_Datetime, bslma::Allocator *>::
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
//  bdet_Datetime& MySharedDatetimePtr::operator*() const {
//      return *d_ptr_p;
//  }
//
//  bdet_Datetime *MySharedDatetimePtr::operator->() const {
//      return d_ptr_p;
//  }
//
//  bdet_Datetime *MySharedDatetimePtr::ptr() const {
//      return d_ptr_p;
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_SHAREDPTRREP
#include <bslma_sharedptrrep.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

#ifndef INCLUDED_TYPEINFO
#include <typeinfo>
#define INCLUDED_TYPEINFO
#endif

namespace BloombergLP {
namespace bslma {

                 // ====================================
                 // class SharedPtrAllocateOutofplaceRep
                 // ====================================

template <class TYPE, class DELETER, class ALLOCATOR>
class SharedPtrAllocateOutofplaceRep : public SharedPtrRep {
    // This class provides a concrete implementation of the 'SharedPtrRep'
    // protocol for out-of-place instances of the parameterized 'TYPE'.  Upon
    // destruction of this object, the parameterized 'DELETER' type is invoked
    // on the pointer to the shared object.

    // DATA
    TYPE      *d_ptr_p;     // pointer to out-of-place instance
                            // (held, not owned)
    DELETER    d_deleter;   // deleter for this out-of-place instance
    ALLOCATOR  d_allocator; // copy of the allocator for this instance

  private:
    // NOT IMPLEMENTED
    SharedPtrAllocateOutofplaceRep(const SharedPtrAllocateOutofplaceRep&);
    SharedPtrAllocateOutofplaceRep& operator=(
                                        const SharedPtrAllocateOutofplaceRep&);

    // PRIVATE CREATORS
    SharedPtrAllocateOutofplaceRep(TYPE             *ptr,
                                   const DELETER&    deleter,
                                   const ALLOCATOR&  basicAllocator);
        // Create a 'SharedPtrAllocateOutofplaceRep' that manages the lifetime of the
        // specified 'ptr', using the specified 'deleter' to destroy 'ptr', and
        // using the specified 'basicAllocator' to supply memory.  Note that
        // 'basicAllocator' will be used to destroy this representation object,
        // but not necessarily to destroy 'ptr'.  Also note that
        // 'SharedPtrAllocateOutofplaceRep' should be created using
        // 'makeOutofplaceRep', which will call the appropriate private
        // constructor depending on the parameterized 'DELETER' type.

    ~SharedPtrAllocateOutofplaceRep();
        // Destroy this representation object and if the shared object has not
        // been deleted, delete the shared object using the associated deleter.
        // Note that this destructor is never called explicitly.  Instead,
        // 'disposeObject' destroys the shared object object and 'disposeRep'
        // deallocates this representation object.

  public:
    // CLASS METHODS
    static SharedPtrAllocateOutofplaceRep *makeOutofplaceRep(
                                             TYPE             *ptr,
                                             const DELETER&    deleter,
                                             const ALLOCATOR&  basicAllocator);
        // Return the address of a newly created 'SharedPtrAllocateOutofplaceRep'
        // object that manages the lifetime of the specified 'ptr', using the
        // specified 'deleter' to destroy 'ptr'.  Optionally, specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  Note that the
        // parameterized 'DELETER' type will be used to deallocate the memory
        // pointed to by 'ptr'.

    // MANIPULATORS
    virtual void disposeRep();
        // Destroy this representation object and deallocate the associated
        // memory.  This method is automatically invoked by 'releaseRef' and
        // 'releaseWeakRef' when the number of weak references and the number
        // of shared references both reach zero and should not be explicitly
        // invoked otherwise.  The behavior is undefined unless 'disposeObject'
        // has already been called for this representation.  Note that this
        // 'disposeRep' method effectively serves as the representation
        // object's destructor.

    virtual void disposeObject();
        // Destroy the object being referred to by this representation.  This
        // method is automatically invoked by 'releaseRef' when the number of
        // shared references reaches zero and should not be explicitly invoked
        // otherwise.

    virtual void *getDeleter(const std::type_info& type);
        // Return a pointer to the deleter stored by the derived representation
        // (if any) if the deleter has the same type as that described by the
        // specified 'type', and a null pointer otherwise.

    // ACCESSORS
    virtual void *originalPtr() const;
        // Return the (untyped) address of the modifiable shared object to
        // which this object refers.

    TYPE *ptr() const;
        // Return the address of the modifiable shared object to which this
        // object refers.
};


               // ===============================================
               // struct SharedPtrAllocateOutofplaceRep_InitGuard
               // ===============================================

template <class TYPE, class DELETER>
struct SharedPtrAllocateOutofplaceRep_InitGuard {
    // This guard is used for out-of-place shared pointer instantiations.
    // Generally, a guard is created prior to constructing a
    // 'SharedPtrAllocateOutofplaceRep' and released after successful construction.  In
    // the event that an exception is thrown during construction of the
    // representation, the guard will delete the provided pointer using the
    // provided deleter.  Note that the provided deleter is held by reference
    // and must remain valid for the lifetime of the guard.  If the guard is
    // not released before it's destruction, a copy of the deleter is
    // instantiated to delete the pointer (in case 'operator()' is
    // non-'const').  Also note that if the deleter throws during
    // copy-construction, the provided pointer will not be destroyed.

    // DATA
    TYPE           *d_ptr_p;    // address of the managed object (held, not
                                // owned)

    const DELETER&  d_deleter;  // deleter used to destroy managed object

  public:
    // CREATORS
    SharedPtrAllocateOutofplaceRep_InitGuard(TYPE           *ptr,
                                             const DELETER&  deleter);
        // Create a guard referring to the specified 'ptr' and using the
        // specified 'deleter' to destroy 'ptr' when the guard is destroyed.

    ~SharedPtrAllocateOutofplaceRep_InitGuard();
        // Destroy this guard and the object (if any) referred to by this
        // guard.

    // MANIPULATORS
    void release();
        // Release from management the object referred to by this guard.
};

// ============================================================================
//                      INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                  // ------------------------------------
                  // class SharedPtrAllocateOutofplaceRep
                  // ------------------------------------

// CREATORS
template <class TYPE, class DELETER, class ALLOCATOR>
SharedPtrAllocateOutofplaceRep<TYPE, DELETER, ALLOCATOR>::
SharedPtrAllocateOutofplaceRep(
                              TYPE             *ptr,
                              const DELETER&    deleter,
                              const ALLOCATOR&  basicAllocator)
: d_ptr_p(ptr)
, d_deleter(deleter)
, d_allocator(basicAllocator)
{
}


template <class TYPE, class DELETER, class ALLOCATOR>
SharedPtrAllocateOutofplaceRep<TYPE, DELETER, ALLOCATOR>::
~SharedPtrAllocateOutofplaceRep()
{
}


// MANIPULATORS
template <class TYPE, class DELETER, class ALLOCATOR>
SharedPtrAllocateOutofplaceRep<TYPE, DELETER, ALLOCATOR> *
SharedPtrAllocateOutofplaceRep<TYPE, DELETER, ALLOCATOR>::makeOutofplaceRep(
                                              TYPE             *ptr,
                                              const DELETER&    deleter,
                                              const ALLOCATOR&  basicAllocator)
{
    SharedPtrAllocateOutofplaceRep_InitGuard<TYPE, DELETER> guard(ptr, deleter);

    typedef
     typename ALLOCATOR::template rebind<SharedPtrAllocateOutofplaceRep>::other
                                                              ReboundAllocator;

    ReboundAllocator alloc(basicAllocator);
    SharedPtrAllocateOutofplaceRep *rep = alloc.allocate(1);
    new (rep) SharedPtrAllocateOutofplaceRep(ptr, deleter, basicAllocator);

    guard.release();

    return rep;
}

template <class TYPE, class DELETER, class ALLOCATOR>
inline
void SharedPtrAllocateOutofplaceRep<TYPE, DELETER, ALLOCATOR>::disposeRep()
{
    // If 'd_allocator_p->deleteObject' is used to destroy the
    // 'SharedPtrAllocateOutofplaceRep' object, a virtual function call will be used
    // and a 'dynamic_cast' is required to obtain the address of the most
    // derived object to deallocate it.  Knowing 'SharedPtrAllocateOutofplaceRep' is
    // the most derived class, this unnecessary overhead of a virtual function
    // call can be avoided by explicitly calling the destructor.  This behavior
    // is guaranteed by the standard ([class.virtual] 13: "Explicit
    // qualification with the scope operator (5.1) suppresses the virtual call
    // mechanism.", page 224 of Working Draft 2007-10).

    typedef
     typename ALLOCATOR::template rebind<SharedPtrAllocateOutofplaceRep>::other
                                                              ReboundAllocator;

    ReboundAllocator alloc(d_allocator);
    this->SharedPtrAllocateOutofplaceRep<TYPE, DELETER, ALLOCATOR>::
                                             ~SharedPtrAllocateOutofplaceRep();
    alloc.deallocate(this, 1);
}

template <class TYPE, class DELETER, class ALLOCATOR>
void SharedPtrAllocateOutofplaceRep<TYPE, DELETER, ALLOCATOR>::disposeObject()
{
    d_deleter(d_ptr_p);
    d_ptr_p = 0;
}

template <class TYPE, class DELETER, class ALLOCATOR>
inline
void *
SharedPtrAllocateOutofplaceRep<TYPE, DELETER, ALLOCATOR>::getDeleter(
                                                    const std::type_info& type)
{
    return (typeid(d_deleter) == type)
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


               // -----------------------------------------------
               // struct SharedPtrAllocateOutofplaceRep_InitGuard
               // -----------------------------------------------

// CREATORS
template <class TYPE, class DELETER>
inline
SharedPtrAllocateOutofplaceRep_InitGuard<TYPE, DELETER>::
SharedPtrAllocateOutofplaceRep_InitGuard(TYPE           *ptr,
                                         const DELETER&  deleter)
: d_ptr_p(ptr)
, d_deleter(deleter)
{
}

template <class TYPE, class DELETER>
inline
SharedPtrAllocateOutofplaceRep_InitGuard<TYPE, DELETER>::
~SharedPtrAllocateOutofplaceRep_InitGuard()
{
    // The guard must destroy 'ptr' to avoid a leak, but is not subject to the
    // reference-counting null pointers principle, where the deleter is called
    // on destroying the last reference, even if 'd_ptr' is null.

    if (d_ptr_p) {
        d_deleter(d_ptr_p);
    }
}

// MANIPULATORS
template <class TYPE, class DELETER>
inline
void SharedPtrAllocateOutofplaceRep_InitGuard<TYPE, DELETER>::release()
{
    d_ptr_p = 0;
}

}  // close namespace bslma
}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
