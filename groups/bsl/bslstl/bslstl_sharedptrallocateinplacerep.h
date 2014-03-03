// bslstl_sharedptrallocateinplacerep.h                               -*-C++-*-
#ifndef INCLUDED_BSLSTL_SHAREDPTRALLOCATEINPLACEREP
#define INCLUDED_BSLSTL_SHAREDPTRALLOCATEINPLACEREP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide an out-of-place implementation of 'bslma::SharedPtrRep'.
//
//@CLASSES:
//  bslstl::SharedPtrAllocateInplaceRep: in-place shared ptr implementation
//
//@AUTHOR: Alisdair Meredith (ameredit)
//
//@SEE_ALSO: bslma_sharedptr, bslma_sharedptrrep, bslma_SharedPtrAllocateInplaceRep
//
//@DESCRIPTION: This component provides ...

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

#ifndef INCLUDED_UTILITY
#include <utility>  // for 'std::forward'
#define INCLUDED_UTILITY
#endif

namespace BloombergLP {
namespace bslstl {

                 // =================================
                 // class SharedPtrAllocateInplaceRep
                 // =================================

template <class TYPE, class ALLOCATOR>
class SharedPtrAllocateInplaceRep : public BloombergLP::bslma::SharedPtrRep {
    // This class provides a concrete implementation of the 'SharedPtrRep'
    // protocol for out-of-place instances of the parameterized 'TYPE'.  Upon
    // destruction of this object, the parameterized 'DELETER' type is invoked
    // on the pointer to the shared object.

    // PRIVATE TYPES
    typedef typename bsl::allocator_traits<ALLOCATOR>::
                            template rebind_traits<SharedPtrAllocateInplaceRep>
                                                                 ReboundTraits;

  public:
    typedef typename ReboundTraits::allocator_type ReboundAllocator;

  private:
    // DATA
    ReboundAllocator d_allocator; // copy of the allocator for this object

    bsls::ObjectBuffer<TYPE> d_instance;
                                  // beginning of the in-place buffer
                                  // Note that this must be last in this layout
                                  // to allow for the possibility of creating
                                  // in-place uninitialized buffer, where it is
                                  // possible to access memory beyond the
                                  // 'd_instance' footprint (refer to
                                  // 'bsl::share_ptr::
                                  // createInplaceUninitializedBuffer' for
                                  // sample usage)

    // PRIVATE CREATORS
    explicit SharedPtrAllocateInplaceRep(
                                       const ReboundAllocator& basicAllocator);
        // Create a 'SharedPtrAllocateInplaceRep' object having an "in-place"
        // default-constructed instance of the parameterized 'TYPE'.  Use the
        // specified 'basicAllocator' to supply memory and, upon a call to
        // 'disposeRep', to destroy this representation (and the "in-place"
        // shared object) .

    ~SharedPtrAllocateInplaceRep();
        // Destroy this representation object and if the shared object has not
        // been deleted, delete the shared object using the associated deleter.
        // Note that this destructor is never called explicitly.  Instead,
        // 'disposeObject' destroys the shared object object and 'disposeRep'
        // deallocates this representation object.

  private:
    // NOT IMPLEMENTED
    SharedPtrAllocateInplaceRep(const SharedPtrAllocateInplaceRep&);
    SharedPtrAllocateInplaceRep& operator=(const SharedPtrAllocateInplaceRep&);

  public:
    static SharedPtrAllocateInplaceRep *makeRep(
                                              ReboundAllocator basicAllocator);
        // Create a 'SharedPtrAllocateInplaceRep' object having an "in-place"
        // default-constructed instance of the parameterized 'TYPE', and return
        // its address.  Use the specified 'basicAllocator' to supply memory
        // and, upon a call to 'disposeRep', to destroy this representation
        // (and the "in-place" shared object).  Note that the function members
        // 'ptr' and 'originalPtr' will return the address of an uninitialized
        // object.  This object should be explicitly initialized by the caller,
        // and it is undefined behavior to call 'disposeRep' until this object
        // has been successfully constructed.

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
        // 'disposeRep' method effectively serves as the representation
        // object's destructor.

    void *getDeleter(const std::type_info& type);
        // Return a null pointer.  The specified 'type' is not used.  Note that
        // an in-place representation for a shared pointer can never store a
        // user-supplied deleter, as there is no function that might try to
        // create one.

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
//              INLINE FUNCTION AND FUNCTION TEMPLATE DEFINITIONS
// ============================================================================

                  // ---------------------------------
                  // class SharedPtrAllocateInplaceRep
                  // ---------------------------------

// CLASS METHODS
template <class TYPE, class ALLOCATOR>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR> *
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::makeRep(
                                               ReboundAllocator basicAllocator)
{
    SharedPtrAllocateInplaceRep *rep_p =
                                    ReboundTraits::allocate(basicAllocator, 1);
    new(rep_p) SharedPtrAllocateInplaceRep(basicAllocator);
    return rep_p;
}

// CREATORS
template <class TYPE, class ALLOCATOR>
inline
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator)
: d_allocator(basicAllocator)
// do NOT waste cycles zero-initializing the object buffer
{
}


template <class TYPE, class ALLOCATOR>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::
~SharedPtrAllocateInplaceRep()
{
    BSLS_ASSERT(0);
}


// MANIPULATORS
template <class TYPE, class ALLOCATOR>
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
    alloc.deallocate(this, 1);
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

}  // close namespace bslstl
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
