// bslma_sharedptrinplacerep.h                                        -*-C++-*-
#ifndef INCLUDED_BSLMA_SHAREDPTRINPLACEREP
#define INCLUDED_BSLMA_SHAREDPTRINPLACEREP

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide an in-place implementation of 'bslma::SharedPtrRep'.
//
//@CLASSES:
//  bslma::SharedPtrInplaceRep: in-place 'bslma::SharedPtrRep' implementation
//
//@SEE_ALSO: bslma_sharedptr, bslma_sharedptr_rep, bslma_sharedptroutofplacerep
//
//@DESCRIPTION: This component provides a concrete implementation of
// 'bslma::SharedPtrRep' for managing objects of the parameterized 'TYPE' that
// are stored in-place in the representation .  Thus, only one memory
// allocation is required to create both the representation and the managed
// object.  When all references to the in-place object are released (using
// 'releaseRef'), the destructor of 'TYPE' is invoked.
//
///Thread Safety
///-------------
// 'bslma::SharedPtrInplaceRep' is thread-safe provided that 'disposeObject'
// and 'disposeRep' are not called explicitly, meaning that all non-creator
// operations other than 'disposeObject' and 'disposeRep' on a given instance
// can be safely invoked simultaneously from multiple threads ('disposeObject'
// and 'disposeRep' are meant to be invoked only by 'releaseRef' and
// 'releaseWeakRef').  Note that there is no thread safety guarantees for
// operations on the managed object contained in 'bslma::SharedPtrInplaceRep'.
//
///Usage
///-----
// The following example demonstrates how to implement a shared
// 'bdlt::Datetime' using 'bslma::SharedPtrInplaceRep':
//..
//  class MySharedDatetimePtr {
//      // This class provide a reference counted smart pointer to support
//      // shared ownership of a 'bdlt::Datetime' object.
//
//      bdlt::Datetime      *d_ptr_p;  // pointer to the managed object
//      bslma::SharedPtrRep *d_rep_p;  // pointer to the representation object
//
//    private:
//      // NOT IMPLEMENTED
//      MySharedDatetimePtr& operator=(const MySharedDatetimePtr&);
//
//    public:
//      // CREATORS
//      MySharedDatetimePtr();
//          // Create an empty shared datetime.
//
//      MySharedDatetimePtr(bdlt::Datetime* ptr, bslma::SharedPtrRep* rep);
//          // Create a shared datetime that adopts ownership of the specified
//          // 'ptr' and the specified 'rep.
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
//      void createInplace(bslma::Allocator *basicAllocator,
//                         int               year,
//                         int               month,
//                         int               day);
//          // Create a new 'bslma::SharedPtrInplaceRep', using the specified
//          // 'basicAllocator' to supply memory, using the specified 'year',
//          // 'month' and 'day' to initialize the 'bdlt::Datetime' within the
//          // newly created 'bslma::SharedPtrInplaceRep', and make this
//          // object refer to the newly created 'bdlt::Datetime' object.
//
//      bdlt::Datetime& operator*() const;
//          // Return a reference offering modifiable access to the shared
//          // 'bdlt::Datetime' object.
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
//  MySharedDatetimePtr::MySharedDatetimePtr()
//  : d_ptr_p(0)
//  , d_rep_p(0)
//  {
//  }
//
//  MySharedDatetimePtr::MySharedDatetimePtr(bdlt::Datetime      *ptr,
//                                           bslma::SharedPtrRep *rep)
//  : d_ptr_p(ptr)
//  , d_rep_p(rep)
//  {
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
//  void MySharedDatetimePtr::createInplace(bslma::Allocator *basicAllocator,
//                                          int               year,
//                                          int               month,
//                                          int               day)
//  {
//      basicAllocator = bslma::Default::allocator(basicAllocator);
//      bslma::SharedPtrInplaceRep<bdlt::Datetime> *rep = new (*basicAllocator)
//                   bslma::SharedPtrInplaceRep<bdlt::Datetime>(basicAllocator,
//                                                              year,
//                                                              month,
//                                                              day);
//      MySharedDatetimePtr temp(rep->ptr(), rep);
//      bsl::swap(d_ptr_p, temp.d_ptr_p);
//      bsl::swap(d_rep_p, temp.d_rep_p);
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

#include <bslma_allocator.h>
#include <bslma_sharedptrrep.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_movableref.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_util.h>

#include <stddef.h>
#include <typeinfo>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Thu Oct 21 10:11:37 2021
// Command line: sim_cpp11_features.pl bslma_sharedptrinplacerep.h
# define COMPILING_BSLMA_SHAREDPTRINPLACEREP_H
# include <bslma_sharedptrinplacerep_cpp03.h>
# undef COMPILING_BSLMA_SHAREDPTRINPLACEREP_H
#else

namespace BloombergLP {
namespace bslma {

                      // =========================
                      // class SharedPtrInplaceRep
                      // =========================

template <class TYPE>
class SharedPtrInplaceRep : public SharedPtrRep {
    // This class provides a concrete implementation of the 'SharedPtrRep'
    // protocol for "in-place" instances of the parameterized 'TYPE'.  Upon
    // destruction of this object, the destructor of 'TYPE' is invoked.

    // DATA
    Allocator *d_allocator_p; // memory allocator (held, not owned)

    TYPE       d_instance;    // Beginning of the in-place buffer.  Note that
                              // this must be last in this layout to allow for
                              // the possibility of creating in-place
                              // uninitialized buffer, where it is possible to
                              // access memory beyond the 'd_instance'
                              // footprint (refer to 'bsl::shared_ptr::
                              // createInplaceUninitializedBuffer' for sample
                              // usage)

  private:
    // NOT IMPLEMENTED
    SharedPtrInplaceRep(const SharedPtrInplaceRep&);
    SharedPtrInplaceRep& operator=(const SharedPtrInplaceRep&);

    // PRIVATE CREATORS
    ~SharedPtrInplaceRep();
        // Destroy this representation object and the embedded instance of
        // parameterized 'TYPE'.  Note that this destructor is never called.
        // Instead, 'disposeObject' destroys the in-place object and
        // 'disposeRep' deallocates this representation object (including the
        // shared object's footprint).

  public:
    // CREATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
    template <class... ARGS>
    explicit SharedPtrInplaceRep(Allocator *basicAllocator,
                                 ARGS&&...  args);
        // Create a 'SharedPtrInplaceRep' object having an "in-place" instance
        // of the parameterized 'TYPE' using the 'TYPE' constructor that takes
        // the specified arguments, 'args...'.  Use the specified
        // 'basicAllocator' to supply memory and, upon a call to 'disposeRep',
        // to destroy this representation (and the "in-place" shared object).
#endif

    // MANIPULATORS
    virtual void disposeObject();
        // Destroy the object being referred to by this representation.  This
        // method is automatically invoked by 'releaseRef' when the number of
        // shared references reaches zero and should not be explicitly invoked
        // otherwise.  Note that this function calls the destructor for the
        // shared object, but does not deallocate its footprint.

    virtual void disposeRep();
        // Deallocate the memory associated with this representation object
        // (including the shared object's footprint).  This method is
        // automatically invoked by 'releaseRef' and 'releaseWeakRef' when the
        // number of weak references and the number of shared references both
        // reach zero and should not be explicitly invoked otherwise.  The
        // behavior is undefined unless 'disposeObject' has already been called
        // for this representation.  Note that this 'disposeRep' method
        // effectively serves as the representation object's destructor.

    void *getDeleter(const std::type_info& type);
        // Return a null pointer.  Note that the specified 'type' is not used
        // as an in-place representation for a shared pointer can never store a
        // user-supplied deleter (there is no function that might try to create
        // one).

    TYPE *ptr();
        // Return the address of the modifiable (in-place) object referred to
        // by this representation object.

    // ACCESSORS
    virtual void *originalPtr() const;
        // Return the (untyped) address of the modifiable (in-place) object
        // referred to by this representation object.
};

                        //============================
                        // SharedPtrInplaceRep_ImpUtil
                        //============================

struct SharedPtrInplaceRep_ImpUtil {
    // This struct provides a namespace for several static methods that ease
    // the implementation of many methods of the 'SharedPtrInplaceRep' class.

    // CLASS METHODS
    template <class TYPE>
    static const TYPE& forward(const TYPE& reference);
    template <class TYPE>
    static BloombergLP::bslmf::MovableRef<TYPE> forward(
                        const BloombergLP::bslmf::MovableRef<TYPE>& reference);
        // Return the specified 'reference'.  Note that this pair of overloaded
        // functions is necessary to correctly forward movable references when
        // providing explicit move-semantics for C++03; otherwise the
        // 'MovableRef' is likely to be wrapped in multiple layers of reference
        // wrappers, and not be recognized as the movable vocabulary type.

    template <class TYPE>
    static void *voidify(TYPE *address);
        // Return the specified 'address' cast as a pointer to 'void', even if
        // (the template parameter) 'TYPE' is cv-qualified.

    template <class TYPE>
    static void dispose(const TYPE& object);
        // Destroy the specified 'object'.

    template <class TYPE, size_t SIZE>
    static void dispose(const TYPE (&object)[SIZE]);
        // Destroy each element of the specified 'object'.
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================


                        // ---------------------------
                        // SharedPtrInplaceRep_ImpUtil
                        // ---------------------------

template <class TYPE>
inline
const TYPE& SharedPtrInplaceRep_ImpUtil::forward(const TYPE& reference)
{
    return reference;
}

template <class TYPE>
inline
BloombergLP::bslmf::MovableRef<TYPE> SharedPtrInplaceRep_ImpUtil::forward(
                         const BloombergLP::bslmf::MovableRef<TYPE>& reference)
{
    return reference;
}

template <class TYPE>
inline
void *SharedPtrInplaceRep_ImpUtil::voidify(TYPE *address) {
    return static_cast<void *>(
            const_cast<typename bsl::remove_cv<TYPE>::type *>(address));
}

template <class TYPE>
inline
void SharedPtrInplaceRep_ImpUtil::dispose(const TYPE& object)
{
    object.~TYPE();
}

template <class TYPE, size_t SIZE>
inline
void SharedPtrInplaceRep_ImpUtil::dispose(const TYPE (&object)[SIZE])
{
    for (size_t i = 0; i < SIZE; ++i) {
        dispose(object[i]);
    }
}

                        // -------------------------
                        // class SharedPtrInplaceRep
                        // -------------------------

// CREATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE>
template <class... ARGS>
SharedPtrInplaceRep<TYPE>::SharedPtrInplaceRep(Allocator *basicAllocator,
                                               ARGS&&...  args)
: d_allocator_p(basicAllocator)
, d_instance(BSLS_COMPILERFEATURES_FORWARD(ARGS,args)...)
{
}
#endif

template <class TYPE>
SharedPtrInplaceRep<TYPE>::~SharedPtrInplaceRep()
{
    BSLS_ASSERT(0);
}

// MANIPULATORS
template <class TYPE>
inline
void SharedPtrInplaceRep<TYPE>::disposeObject()
{
    SharedPtrInplaceRep_ImpUtil::dispose(d_instance);
}

template <class TYPE>
inline
void SharedPtrInplaceRep<TYPE>::disposeRep()
{
    d_allocator_p->deallocate(this);
}

template <class TYPE>
inline
void *SharedPtrInplaceRep<TYPE>::getDeleter(const std::type_info&)
{
    return 0;
}

template <class TYPE>
inline
TYPE *SharedPtrInplaceRep<TYPE>::ptr()
{
    return bsls::Util::addressOf(d_instance);
}

// ACCESSORS
template <class TYPE>
inline
void *SharedPtrInplaceRep<TYPE>::originalPtr() const
{
    return const_cast<void *>(static_cast<const void *>(
                                           bsls::Util::addressOf(d_instance)));
}

// ============================================================================
//                              TYPE TRAITS
// ============================================================================

template <class ELEMENT_TYPE>
struct UsesBslmaAllocator<SharedPtrInplaceRep<ELEMENT_TYPE> >
    : bsl::false_type {
    // The class template 'SharedPtrInplaceRep' appears to use allocators, but
    // passes its allocator argument in the first position, rather than in the
    // last position, so is not compatible with BDE APIs that use this trait.
};

}  // close package namespace
}  // close enterprise namespace

#endif // End C++11 code

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
