// bslstl_containerbase.h                                             -*-C++-*-
#ifndef INCLUDED_BSLSTL_CONTAINERBASE
#define INCLUDED_BSLSTL_CONTAINERBASE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Base class for STL-style containers.
//
//@CLASSES:
//  bslstl_ContainerBase: base class for STL-style containers
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component defines a class, 'bslstl_ContainerBase', that
// provides a common base class for all STL-style containers defined in the
// 'bslstl' package.
//
///Usage
///-----
// This component is for internal use only.  A usage example is not provided.

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_containerbase.h> can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATORPROXY
#include <bslstl_allocatorproxy.h>
#endif

namespace BloombergLP {

                        // ==========================
                        // class bslstl_ContainerBase
                        // ==========================

template <class ALLOCATOR>
class bslstl_ContainerBase : private bslstl_AllocatorProxy<ALLOCATOR> {
    // Base class for STL-style containers.  Provides access to the allocator.
    // The 'bslstl_AllocatorProxy base class has two possible implementations
    // depending on whether 'ALLOCATOR' is constructed from 'bslma_Allocator*'.
    // Private inheritance is used in order to take advantage of
    // empty-base-class optimization (implemented by most compilers) in the
    // case where the base class has zero size.

    // PRIVATE TYPES
    typedef bslstl_AllocatorProxy<ALLOCATOR> Base;
        // Base class type.

  private:
    // NOT IMPLEMENTED
    bslstl_ContainerBase& operator=(const bslstl_ContainerBase&);

  public:
    // TYPES
    typedef ALLOCATOR                          allocator_type;
    typedef typename allocator_type::size_type size_type;

    typedef typename Base::bslmaAllocatorPtr bslmaAllocatorPtr;
        // Pointer type returned by 'Base::bslmaAllocator()'.  This type is
        // 'bslma_Allocator*' for 'bslma'-based allocators and 'void*' for non
        // 'bslma'-based allocators.

    // CREATORS
    explicit
    bslstl_ContainerBase(const ALLOCATOR& alloc = ALLOCATOR());
        // Construct this object using the optionally specified 'alloc' of the
        // parameterized 'ALLOCATOR' type.  If 'alloc' is not specified, a
        // default-constructed 'ALLOCATOR' is used.

    bslstl_ContainerBase(const bslstl_ContainerBase& rhs);
        // Initialize this container base with 'rhs'.  NOTE: This is not a true
        // copy constructor.  The allocator do not get copied if the allocator
        // is 'bslma'-based.  Using BSL allocator semantics, the 'bslma'-style
        // allocator must be supplied explicitly (i.e., not copied from rhs) or
        // else it is given a default value.  Non-'bslma' allocators ARE copied
        // because that is the way the ISO standard is currently written.

    ~bslstl_ContainerBase();
        // Destroy this object.

    // MANIPULATORS
    template <class T>
    T *allocateN(T *p, size_type n);
        // Allocate (but do not initialize) 'n' objects of type 'T' using the
        // allocator returned by the 'allocator' method.  Return pointer to the
        // raw memory that was allocated.  The 'p' argument is used only to
        // determine the type of object being allocated; its value (usually
        // null) is not used.

    ALLOCATOR& allocator();
        // Return a reference to the modifiable allocator most recently
        // installed using 'setManagedAllocator'.  Returns the same object as
        // that returned by 'originalAllocator' if 'setManagedAllocator' has
        // not been called.

    template <class T>
    void deallocateN(T *p, size_type n);
        // Return 'n' objects of type 'T', starting at 'p', to the allocator
        // returned by the 'allocator' method.  Does not call destructors on
        // the deallocated objects.

    // ACCESSORS
    const ALLOCATOR& allocator() const;
        // Return a reference to the non-modifiable allocator most recently
        // installed using 'setManagedAllocator'.  Returns the same object as
        // that returned by 'originalAllocator' if 'setManagedAllocator' has
        // not been called.

    bslmaAllocatorPtr bslmaAllocator() const;
        // Return 'allocator().mechanism()' if 'ALLOCATOR' is constructed from
        // 'bslma_Allocator*'.  Otherwise returns '(void*) 0'.

    bool equalAllocator(bslstl_ContainerBase& rhs) const;
        // Returns 'this->originalAllocator() == rhs.originalAllocator()'.

    allocator_type get_allocator() const;
        // Return the allocator used to construct this object (i.e., the object
        // returned by 'originalAllocator').
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // --------------------------
                        // class bslstl_ContainerBase
                        // --------------------------

// CREATORS
template <class ALLOCATOR>
inline
bslstl_ContainerBase<ALLOCATOR>::
bslstl_ContainerBase(const ALLOCATOR& alloc)
: Base(alloc)
{
}

template <class ALLOCATOR>
inline
bslstl_ContainerBase<ALLOCATOR>::
bslstl_ContainerBase(const bslstl_ContainerBase<ALLOCATOR>& rhs)
: Base(rhs)
{
}

template <class ALLOCATOR>
inline
bslstl_ContainerBase<ALLOCATOR>::
~bslstl_ContainerBase()
{
}

// MANIPULATORS
template <class ALLOCATOR>
template <class T>
inline
T *bslstl_ContainerBase<ALLOCATOR>::allocateN(T *p, size_type n)
{
    return this->Base::allocateN(p, n);
}

template <class ALLOCATOR>
ALLOCATOR& bslstl_ContainerBase<ALLOCATOR>::allocator()
{
    return this->Base::allocator();
}

template <class ALLOCATOR>
template <class T>
inline
void bslstl_ContainerBase<ALLOCATOR>::deallocateN(T *p, size_type n)
{
    this->Base::deallocateN(p, n);
}

// ACCESSORS
template <class ALLOCATOR>
inline
const ALLOCATOR& bslstl_ContainerBase<ALLOCATOR>::allocator() const
{
    return this->Base::allocator();
}

template <class ALLOCATOR>
inline
typename bslstl_ContainerBase<ALLOCATOR>::bslmaAllocatorPtr
bslstl_ContainerBase<ALLOCATOR>::bslmaAllocator() const
{
    return this->Base::bslmaAllocator();
}

template <class ALLOCATOR>
inline
bool bslstl_ContainerBase<ALLOCATOR>::
equalAllocator(bslstl_ContainerBase& rhs) const
{
    return this->allocator() == rhs.allocator();
}

template <class ALLOCATOR>
inline
ALLOCATOR bslstl_ContainerBase<ALLOCATOR>::get_allocator() const
{
    return allocator();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
