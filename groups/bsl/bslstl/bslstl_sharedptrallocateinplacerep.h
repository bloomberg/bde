// bslstl_sharedptrallocateinplacerep.h                               -*-C++-*-
#ifndef INCLUDED_BSLSTL_SHAREDPTRALLOCATEINPLACEREP
#define INCLUDED_BSLSTL_SHAREDPTRALLOCATEINPLACEREP

#include <bsls_ident.h>
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

#include <bslscm_version.h>

#include <bslma_allocatortraits.h>
#include <bslma_sharedptrrep.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_isarray.h>
#include <bslmf_assert.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsls_types.h>

#include <new>
#include <stddef.h>     // 'size_t'
#include <stdint.h>     // 'uintptr_t'
#include <typeinfo>

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
        // for this object.

    ~SharedPtrAllocateInplaceRep();  // = delete
        // The destructor for this object should never be called.  The in-place
        // 'TYPE' object will be destroyed by a call to 'disposeObject' and the
        // stored allocator for this object will be destroyed by a call to
        // 'disposeRep', which will then reclaim the storage occupied by this
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


                // ======================================
                // class SharedPtrArrayAllocateInplaceRep
                // ======================================

template <class TYPE, class ALLOCATOR>
class SharedPtrArrayAllocateInplaceRep
                                    : public BloombergLP::bslma::SharedPtrRep {
    // This class provides a concrete implementation of the 'SharedPtrRep'
    // protocol for in-place instances of the (template parameter) 'TYPE',
    // which must be an array.  Upon destruction of this object, each element
    // of the array is destructed.

  private:
    // PRIVATE TYPES
    typedef typename bsl::allocator_traits<ALLOCATOR>::
                template rebind_traits<SharedPtrArrayAllocateInplaceRep>
                                                                 ReboundTraits;

  public:
    // PUBLIC TYPES
    typedef typename bsl::remove_extent<TYPE>::type element_type;
    typedef typename ReboundTraits::allocator_type ReboundAllocator;

  private:
    // DATA
    ReboundAllocator  d_allocator; // copy of the allocator
    size_t            d_size;      // the number of elements
    char             *d_allocated_ptr_p; // the actual memory allocated
    // The elements of the array stored in this Rep are created after the
    // instance variables.

    // NOT IMPLEMENTED
    SharedPtrArrayAllocateInplaceRep(const SharedPtrArrayAllocateInplaceRep&);
    SharedPtrArrayAllocateInplaceRep& operator=(
                                      const SharedPtrArrayAllocateInplaceRep&);

    // PRIVATE CLASS METHODS
    static size_t offset_of_first_element();
        // Return the offset (in bytes) from the start of the object to the
        // first element of the array stored at the end of the object.

    // PRIVATE CREATORS
    explicit SharedPtrArrayAllocateInplaceRep(size_t            numElements,
                                       const ReboundAllocator&  basicAllocator,
                                       char                    *allocatedPtr);
        // Create a 'SharedPtrArrayAllocateInplaceRep' object having
        // uninitialized storage for an in-place instance of the parameterized
        // 'TYPE' containing the specified 'numElements' number of elements.
        // Use the specified 'basicAllocator' to supply memory and, upon a call
        // to 'disposeObject', to destroy the inplace object , and upon a call
        // to 'disposeRep' to destroy the copy the allocator and reclaim
        // storage for this object, pointed to by the specified 'allocatedPtr'.

    ~SharedPtrArrayAllocateInplaceRep();  // = delete
        // The destructor for this object should never be called.  The in-place
        // 'TYPE' object will be destroyed by a call to 'disposeObject' and the
        // stored allocator for this object will be destroyed by a call to
        // 'disposeRep', which will then reclaim the storage occupied by this
        // object.

    // PRIVATE ACCESSORS
    const element_type *get_pointer_to_first_element() const;
        // Return a const pointer to the first element of the array stored at
        // the end of the object.

    // PRIVATE MANIPULATORS
    element_type *get_pointer_to_first_element();
        // Return a pointer to the first element of the array stored at the end
        // of the object.

  public:
    // CLASS METHODS
    static size_t alloc_size(size_t numElements);
        // Return the size (in bytes) for an allocation to hold an object of
        // type 'SharedPtrArrayAllocateInplaceRep<T[]>' containing the
        // specified 'numElements' number of elements.

    static SharedPtrArrayAllocateInplaceRep *makeRep(
                                               ReboundAllocator basicAllocator,
                                               size_t           numElements);
        // Create a 'SharedPtrArrayAllocateInplaceRep' object having storage
        // for an in-place uninitialized sized array of (template parameter)
        // 'TYPE' containing space for the specified 'numElements' number of
        // elements, and return its address.  Use the specified
        // 'basicAllocator' to supply memory and, upon a call to 'disposeRep',
        // to destroy this representation (and the in-place shared object).
        // Note that the function members 'ptr' and 'originalPtr' will return
        // the address of an uninitialized array.  This object should be
        // explicitly initialized by the caller, and it is undefined behavior
        // to call 'disposeRep' until this object has been successfully
        // constructed.

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

    element_type *ptr();
        // Return the address of the modifiable shared object to which this
        // object refers.  Note that in order to return a pointer to a
        // modifiable object, this function cannot be 'const' qualified as the
        // referenced object is stored internally.

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
    BSLMF_ASSERT(!bsl::is_array<TYPE>::value);
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
    return d_instance.address();
}

// ACCESSORS
template <class TYPE, class ALLOCATOR>
inline
void *
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::originalPtr() const
{
    return const_cast<void *>(static_cast<const void *>(
                              const_cast<const TYPE *>(d_instance.address())));
}


                // --------------------------------------
                // class SharedPtrArrayAllocateInplaceRep
                // --------------------------------------

// PRIVATE CREATORS
template <class TYPE, class ALLOCATOR>
inline
SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR>::
SharedPtrArrayAllocateInplaceRep(size_t                   numElements,
                                 const ReboundAllocator&  basicAllocator,
                                 char                    *allocatedPtr)
: d_allocator(basicAllocator)
, d_size(numElements)
, d_allocated_ptr_p(allocatedPtr)
{
    BSLMF_ASSERT(bsl::is_array<TYPE>::value);
    BSLS_ASSERT_OPT(numElements > 0);
}

template <class TYPE, class ALLOCATOR>
SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR>::
~SharedPtrArrayAllocateInplaceRep()
{
    BSLS_ASSERT_OPT(0);
}

// PRIVATE CLASS METHODS
template <class TYPE, class ALLOCATOR>
inline
size_t
SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR>::offset_of_first_element()
{
    typedef SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR> ThisType;

    enum {
        k_ALIGNMENT      = bsls::AlignmentFromType<element_type>::VALUE,
        k_ALIGNMENT_MASK = ~(k_ALIGNMENT - 1)
    };

    // We want the first multiple of 'k_ALIGNMENT' that is >= sizeof(*this). We
    // assume that 'k_ALIGNMENT' is a power of two.
    BSLMF_ASSERT((k_ALIGNMENT & (k_ALIGNMENT - 1)) == 0);
    return (sizeof(ThisType) + k_ALIGNMENT - 1) & k_ALIGNMENT_MASK;
}

// CLASS METHODS
template <class TYPE, class ALLOCATOR>
inline
size_t
SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR>::alloc_size(
                                                            size_t numElements)
{
    BSLS_ASSERT_OPT(numElements > 0);

    typedef SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR> ThisType;
    return ThisType::offset_of_first_element() +
                                bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1 +
                                (sizeof(ThisType::element_type) * numElements);
}

template <class TYPE, class ALLOCATOR>
inline
SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR> *
SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR>::makeRep(
                                               ReboundAllocator basicAllocator,
                                               size_t           numElements)
{
    BSLS_ASSERT_OPT(numElements > 0);

    typedef typename bsl::allocator_traits<ReboundAllocator>::
                                  template rebind_traits<char> CharATraits;

    enum {
        k_ALIGNMENT      = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT,
        k_ALIGNMENT_MASK = ~(k_ALIGNMENT - 1)
    };

    // Rebind the allocator because we need to allocate in units of bytes.
    typename CharATraits::allocator_type  charAlloc(basicAllocator);
    char                                 *char_p =
                     CharATraits::allocate(charAlloc, alloc_size(numElements));

    // Align the pointer; convert it to an integer, add BSLS_MAX_ALIGNMENT -1,
    // then mask off the low bits.  We assume that 'BSLS_MAX_ALIGNMENT' is a
    // power of two.
    BSLMF_ASSERT((k_ALIGNMENT & (k_ALIGNMENT - 1)) == 0);
    bsls::Types::UintPtr              aligned =
           (reinterpret_cast<bsls::Types::UintPtr>(char_p) + k_ALIGNMENT - 1) &
                                                              k_ALIGNMENT_MASK;
    SharedPtrArrayAllocateInplaceRep *rep_p =
                 reinterpret_cast<SharedPtrArrayAllocateInplaceRep *>(aligned);

    new (rep_p)
         SharedPtrArrayAllocateInplaceRep(numElements, basicAllocator, char_p);
    return rep_p;
}

// ACCESSORS
template <class TYPE, class ALLOCATOR>
inline
typename
SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR>::element_type *
SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR>::
                                                 get_pointer_to_first_element()
{
    return reinterpret_cast<element_type *>(
                   reinterpret_cast<char *>(this) + offset_of_first_element());
}

// MANIPULATORS
template <class TYPE, class ALLOCATOR>
inline
void SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR>::disposeObject()
{
    typedef typename bsl::allocator_traits<ALLOCATOR>::
                template rebind_traits<element_type> ElementTraits;

    typename ElementTraits::allocator_type elementAlloc(d_allocator);

    //  Destruct the elements of the array in reverse order
    element_type *pN = static_cast<element_type *>(ptr()) + d_size - 1;
    for (size_t i = 0; i < d_size; ++i, --pN) {
        ElementTraits::destroy(elementAlloc, pN);
    }
}

template <class TYPE, class ALLOCATOR>
inline
void SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR>::disposeRep()
{
    typedef typename bsl::allocator_traits<ReboundAllocator>::
                                      template rebind_traits<char> CharATraits;
    typename CharATraits::allocator_type  charAlloc(d_allocator);

    this->d_allocator.~ReboundAllocator();
    CharATraits::deallocate(charAlloc, d_allocated_ptr_p, alloc_size(d_size));
}

template <class TYPE, class ALLOCATOR>
inline
void *
SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR>
::getDeleter(const std::type_info&)
{
    return 0;
}

template <class TYPE, class ALLOCATOR>
inline
const typename
SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR>::element_type *
SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR>::
                                           get_pointer_to_first_element() const
{
    return reinterpret_cast<const element_type *>(
             reinterpret_cast<const char *>(this) + offset_of_first_element());
}

template <class TYPE, class ALLOCATOR>
inline typename
SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR>::element_type *
SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR>::ptr()
{
    return get_pointer_to_first_element();
}

// ACCESSORS
template <class TYPE, class ALLOCATOR>
inline
void *
SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR>::originalPtr() const
{
    return const_cast<void *>(
               static_cast<const void *>(
                   get_pointer_to_first_element()));
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

template <class TYPE, class ALLOCATOR>
struct UsesBslmaAllocator<
                    bslstl::SharedPtrArrayAllocateInplaceRep<TYPE, ALLOCATOR> >
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
