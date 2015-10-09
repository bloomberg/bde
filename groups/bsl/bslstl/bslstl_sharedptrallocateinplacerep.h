// bslstl_sharedptrallocateinplacerep.h                               -*-C++-*-
#ifndef INCLUDED_BSLSTL_SHAREDPTRALLOCATEINPLACEREP
#define INCLUDED_BSLSTL_SHAREDPTRALLOCATEINPLACEREP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide an in-place implementation of 'bslma::SharedPtrRep'.
//
//@CLASSES:
//  bslstl::SharedPtrAllocateInplaceRep: in-place 'shared_ptr' implementation
//
//@SEE_ALSO: bslma_sharedptrrep, bslma_sharedptrinplacerep, bslstl_sharedptr
//
//@DESCRIPTION: This component provides a class template,
// 'bslstl::SharedPtrAllocateInplaceRep', proving a concrete implementation of
// 'bslma::SharedPtrRep' for managing objects of the parameterized 'TYPE' that
// are stored in-place in the representation, and are allocated by an allocator
// that satisfies the requirements of an Allocator type in the C++11 standard.
// Thus, only one memory allocation is required to create both the
// representation and the managed object.  When all references to the in-place
// object are released (using 'releaseRef'), the destructor of 'TYPE' is
// invoked.
//
///Thread Safety
///-------------
// 'bslstl::SharedPtrAllocateInplaceRep' is thread-safe provided that
// 'disposeObject' and 'disposeRep' are not called explicitly, meaning that all
// non-creator operations other than 'disposeObject' and 'disposeRep' on a
// given instance can be safely invoked simultaneously from multiple threads
// ('disposeObject' and 'disposeRep' are meant to be invoked only by
// 'releaseRef' and 'releaseWeakRef').  Note that there is no thread safety
// guarantees for operations on the managed object contained in
// 'bslstl::SharedPtrAllocateInplaceRep'.
//
#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATORTRAITS
#include <bslstl_allocatortraits.h>
#endif

#ifndef INCLUDED_BSLMA_SHAREDPTRREP
#include <bslma_sharedptrrep.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_TYPEINFO
#include <typeinfo>
#define INCLUDED_TYPEINFO
#endif

namespace BloombergLP {
namespace bslstl {

                 // =================================
                 // class SharedPtrAllocateInplaceRep
                 // =================================

template <class TYPE, class ALLOCATOR>
class SharedPtrAllocateInplaceRep : public BloombergLP::bslma::SharedPtrRep {
    // This class provides a concrete implementation of the 'SharedPtrRep'
    // protocol for in-place instances of the (template parameter) 'TYPE'.
    // Upon destruction of this object, the destructor of 'TYPE' is invoked on
    // the shared object.

    // PRIVATE TYPES
    typedef typename bsl::allocator_traits<ALLOCATOR>::
                            template rebind_traits<SharedPtrAllocateInplaceRep>
                                                                 ReboundTraits;

  public:
    // PUBLIC TYPES
    typedef typename ReboundTraits::allocator_type ReboundAllocator;

  private:
    // DATA
    ReboundAllocator         d_allocator; // copy of this object's allocator
    bsls::ObjectBuffer<TYPE> d_instance;  // beginning of the in-place buffer

    // Note that 'd_instance' must be last in this layout to allow for the
    // possibility of creating an in-place uninitialized buffer, where it is
    // possible to access memory beyond the 'd_instance' footprint (refer to
    // 'bsl::shared_ptr::createInplaceUninitializedBuffer' for sample usage).

  private:
    // NOT IMPLEMENTED
    SharedPtrAllocateInplaceRep(const SharedPtrAllocateInplaceRep&);
    SharedPtrAllocateInplaceRep& operator=(const SharedPtrAllocateInplaceRep&);

    // PRIVATE CREATORS
    explicit SharedPtrAllocateInplaceRep(
                                       const ReboundAllocator& basicAllocator);
        // Create a 'SharedPtrAllocateInplaceRep' object having uninitialized
        // storage for an in-place instance of the parameterized 'TYPE'.  Use
        // the specified 'basicAllocator' to supply memory and, upon a call to
        // 'disposeObject', to destroy the inplace object, and upon a call to
        // 'disposeRep' to destroy the copy the allocator and reclaim storage
        // for this object,

    ~SharedPtrAllocateInplaceRep();  // = delete
        // The destructor for this object should never be called.  The in-place
        // 'TYPE' object will be destroyed by a call to 'destroyObject' and the
        // stored allocator for this object will be destroyed by a call to
        // 'destroyRep', which will then reclaim the storage occupied by this
        // object.

  public:
    // CLASS METHODS
    static SharedPtrAllocateInplaceRep *makeRep(
                                              ReboundAllocator basicAllocator);
        // Create a 'SharedPtrAllocateInplaceRep' object having storage for an
        // in-place uninitialized object of (template parameter) 'TYPE', and
        // return its address.  Use the specified 'basicAllocator' to supply
        // memory and, upon a call to 'disposeRep', to destroy this
        // representation (and the in-place shared object).  Note that the
        // function members 'ptr' and 'originalPtr' will return the address of
        // an uninitialized object.  This object should be explicitly
        // initialized by the caller, and it is undefined behavior to call
        // 'disposeRep' until this object has been successfully constructed.

    // MANIPULATORS
    virtual void disposeObject();
        // Destroy the object being referred to by this representation.  This
        // method is automatically invoked by 'releaseRef' when the number of
        // shared references reaches zero and should not be explicitly invoked
        // otherwise.

    virtual void disposeRep();
        // Destroy this representation object and deallocate the associated
        // memory.  This method is automatically invoked by 'releaseRef' and
        // 'releaseWeakRef' when the number of weak references and the number
        // of shared references both reach zero and should not be explicitly
        // invoked otherwise.  The behavior is undefined unless 'disposeObject'
        // has already been called for this representation.  Note that this
        // method effectively serves as the representation object's destructor.

    void *getDeleter(const std::type_info& type);
        // Ignore the specified 'type' and return a null pointer.  Note that
        // there is no facility for the user to supply a deleter for an
        // in-place representation for a shared pointer.

    TYPE *ptr();
        // Return the address of the modifiable shared object to which this
        // object refers.  Note that in order to return a pointer to a
        // modifiable object, this function cannot be 'const' qualified as the
        // referenced object is stored internally as a data member.

    // ACCESSORS
    virtual void *originalPtr() const;
        // Return the (untyped) address of the modifiable shared object to
        // which this object refers.
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                  // ---------------------------------
                  // class SharedPtrAllocateInplaceRep
                  // ---------------------------------

// PRIVATE CREATORS
template <class TYPE, class ALLOCATOR>
inline
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator)
: d_allocator(basicAllocator)
// 'd_instance' is intentionally left uninitialized.
{
}

template <class TYPE, class ALLOCATOR>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::
~SharedPtrAllocateInplaceRep()
{
    BSLS_ASSERT_OPT(0);
}

// CLASS METHODS
template <class TYPE, class ALLOCATOR>
inline
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR> *
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::makeRep(
                                               ReboundAllocator basicAllocator)
{
    SharedPtrAllocateInplaceRep *rep_p =
                                    ReboundTraits::allocate(basicAllocator, 1);
    new(rep_p) SharedPtrAllocateInplaceRep(basicAllocator);
    return rep_p;
}

// MANIPULATORS
template <class TYPE, class ALLOCATOR>
inline
void SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::disposeObject()
{
    ReboundTraits::destroy(d_allocator, ptr());
}

template <class TYPE, class ALLOCATOR>
inline
void SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::disposeRep()
{
    ReboundAllocator alloc(d_allocator);
    this->d_allocator.~ReboundAllocator();
    ReboundTraits::deallocate(alloc, this, 1);
}

template <class TYPE, class ALLOCATOR>
inline
void *
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::getDeleter(const std::type_info&)
{
    return 0;
}

template <class TYPE, class ALLOCATOR>
inline
TYPE *SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::ptr()
{
    return reinterpret_cast<TYPE *>(d_instance.buffer());
}

// ACCESSORS
template <class TYPE, class ALLOCATOR>
inline
void *
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::originalPtr() const
{
    return const_cast<void *>(static_cast<const void *>(
                         reinterpret_cast<const TYPE *>(d_instance.buffer())));
}

}  // close package namespace

// ============================================================================
//                              TYPE TRAITS
// ============================================================================

namespace bslma {

template <class TYPE, class ALLOCATOR>
struct UsesBslmaAllocator< bslstl::SharedPtrAllocateInplaceRep<TYPE,
                                                               ALLOCATOR> >
    : bsl::false_type
{};

}  // close namespace bslma
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
