// bslstl_allocatorproxy.h                                            -*-C++-*-
#ifndef INCLUDED_BSLSTL_ALLOCATORPROXY
#define INCLUDED_BSLSTL_ALLOCATORPROXY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Wrapper for STL-style allocators to encapsulate 'bslma' semantics.
//
//@CLASSES:
//  bslstl::AllocatorProxy: proxy class for STL-style containers
//  bslstl::AllocatorProxyBslmaBase: base class for 'bslstl::AllocatorProxy'
//  bslstl::AllocatorProxyNonBslmaBase: base class for 'bslstl::AllocatorProxy'
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION:
//
///Usage
///-----
// This component is for use by 'bslstl_containerbase' only.

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_allocatorproxy.h> can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

namespace BloombergLP {

namespace bslstl {

                        // =============================
                        // class AllocatorProxyBslmaBase
                        // =============================

template <class ALLOCATOR>
class AllocatorProxyBslmaBase {
    // One of two possible base classes for 'AllocatorProxy'.  This class
    // should only be used for allocators that are based on 'bslma::Allocator'.
    // Provides access to the allocator.  Since 'ALLOCATOR' always has state
    // (at least a 'bslma::Allocator *'), there is no empty-base initialization
    // opportunity, so we don't inherit from 'ALLOCATOR' the way
    // 'AllocatorProxyNonBslmaBase' does, below.  (Inheritance of this type can
    // cause ambiguous conversions and should be avoided or insulated.)

    ALLOCATOR d_allocator;

    // NOT IMPLEMENTED
    AllocatorProxyBslmaBase& operator=(const AllocatorProxyBslmaBase&);

  public:
    // TYPES
    typedef ALLOCATOR AllocatorType;

    typedef bslma::Allocator *bslmaAllocatorPtr;
        // Pointer type returned by 'ALLOCATOR::mechanism'.  This type differs
        // in specializations of this class for non 'bslma'-based allocators.

    // CREATORS
    explicit
    AllocatorProxyBslmaBase(const ALLOCATOR& allocator);
        // Construct this object using the specified 'allocator' of the
        // 'ALLOCATOR' parameterized type.

    AllocatorProxyBslmaBase(const AllocatorProxyBslmaBase& original);
        // Construct this object using the default allocator.  The 'original'
        // argument is ignored.  NOTE: This is obviously not a copy constructor
        // as is does not do any copying.  It does implement BSL-style
        // allocator semantics, whereby a newly created object must either have
        // an explicitly-specified allocator or else it uses the default
        // allocator object.  Under no circumstances is a BSL-style allocator
        // copied during copy construction or assignment.

    ~AllocatorProxyBslmaBase();
        // Destroy this object.

    // MANIPULATORS
    ALLOCATOR& allocator();
        // Return a reference to the modifiable allocator used to construct
        // this object.

    // ACCESSORS
    const ALLOCATOR& allocator() const;
        // Return a reference to the non-modifiable allocator used to construct
        // this object.

    bslmaAllocatorPtr bslmaAllocator() const;
        // Return 'allocator().mechanism()'.
};

                        // ================================
                        // class AllocatorProxyNonBslmaBase
                        // ================================

template <class ALLOCATOR>
class AllocatorProxyNonBslmaBase : public ALLOCATOR {
    // One of two possible base classes for 'AllocatorProxy'.  This class is
    // for allocators that are not based on 'bslma::Allocator'.  Provides
    // access to the allocator.
    //
    // Because this class inherits from 'ALLOCATOR' it can take advantage of
    // empty-base optimization.  In other words, if 'ALLOCATOR' has no state
    // then it will not contribute to the footprint of the
    // 'AllocatorProxyNonBslmaBase' object.  'AllocatorProxyNonBslmaBase'
    // itself adds no state and will not increase the footprint of subsequently
    // derived classes.

    // NOT IMPLEMENTED
    AllocatorProxyNonBslmaBase& operator=(const AllocatorProxyNonBslmaBase&);

  public:
    // TYPES
    typedef ALLOCATOR AllocatorType;

    typedef void *bslmaAllocatorPtr;
        // Generically, a pointer to the 'bslma::Allocator' mechanism type for
        // 'ALLOCATOR'.  Since the 'ALLOCATOR' type specified in this template
        // does not use the 'bslma::Allocator' mechanism, 'bslmaAllocatorPtr'
        // is simply 'void*'.  This can be used for overloading functions based
        // on whether a container uses a 'bslma'-based allocator or not.  (See
        // the 'bslalg_scalarprimitives' and 'bslalg_arrayprimitives'
        // components.)

    // CREATORS
    AllocatorProxyNonBslmaBase(const ALLOCATOR& allocator);
        // Construct this object using the specified 'allocator' or the
        // parameterized 'ALLOCATOR' type.

    AllocatorProxyNonBslmaBase(const AllocatorProxyNonBslmaBase& rhs);
        // Construct this object by copying the allocator from rhs.  NOTE:
        // Although this constructor does copy the allocator, the copy
        // constructor in the 'bslma'-specific 'AllocatorProxyBslmaBase' class
        // (above) does not.

    ~AllocatorProxyNonBslmaBase();
        // Destroy this object.

    // MANIPULATORS
    ALLOCATOR& allocator();
        // Return a reference to the modifiable allocator used to construct
        // this object.

    // ACCESSORS
    const ALLOCATOR& allocator() const;
        // Return a reference to the non-modifiable allocator used to construct
        // this object.

    bslmaAllocatorPtr bslmaAllocator() const;
        // Return a null pointer.  (This container's allocator does not use
        // 'bslma').  Note that the return type for this function differs from
        // the (typedef'ed) return type for the same function in the
        // 'AllocatorProxyBslmaBase' class and can thus be used to choose one
        // overloaded function over another.
};

                        // ====================
                        // class AllocatorProxy
                        // ====================

template <class ALLOCATOR>
class AllocatorProxy : public
    bslmf::If<bslmf::IsConvertible<bslma::Allocator*, ALLOCATOR>::VALUE,
              AllocatorProxyBslmaBase<ALLOCATOR>,
              AllocatorProxyNonBslmaBase<ALLOCATOR> >::Type {
    // Allocator proxy class for STL-style containers.  Provides access to the
    // allocator.  Implements the entire STL allocator interface, redirecting
    // allocation and deallocation calls to the proxied allocator.  One of two
    // possible base classes is chosen depending on whether 'ALLOCATOR' is
    // constructed from 'bslma::Allocator*'.

    // PRIVATE TYPES
    typedef typename
        bslmf::If<bslmf::IsConvertible<bslma::Allocator*, ALLOCATOR>::VALUE,
                  AllocatorProxyBslmaBase<ALLOCATOR>,
                  AllocatorProxyNonBslmaBase<ALLOCATOR> >::Type Base;

    // NOT IMPLEMENTED
    AllocatorProxy& operator=(const AllocatorProxy&);

  private:
    // PRIVATE MANIPULATORS
    template <class T>
    typename ALLOCATOR::template rebind<T>::other
    rebindAllocator(T*)
        // Return 'this->allocator()' rebound for type 'T'.  The 'T*' argument
        // is used only for template parameter deduction and is ignored.
    {
        typedef typename ALLOCATOR::template rebind<T>::other Rebound;
        return Rebound(this->allocator());
    }

  public:
    // PUBLIC TYPES
    typedef typename Base::AllocatorType   AllocatorType;

    typedef typename AllocatorType::size_type       size_type;
    typedef typename AllocatorType::difference_type difference_type;
    typedef typename AllocatorType::pointer         pointer;
    typedef typename AllocatorType::const_pointer   const_pointer;
    typedef typename AllocatorType::reference       reference;
    typedef typename AllocatorType::const_reference const_reference;
    typedef typename AllocatorType::value_type      value_type;
        // Restate required allocator types.  (Reduces use of 'typename' in
        // interface.)

    // CREATORS
    AllocatorProxy(const ALLOCATOR& allocator);
        // Construct this object using the specified 'allocator' of the
        // parameterized 'ALLOCATOR' type.

    AllocatorProxy(const AllocatorProxy& rhs);
        // Initialize this container base with rhs.  NOTE: This is not a true
        // copy constructor.  The allocator does not get copied if the
        // allocator is 'bslma'-based.  Using BSL allocator semantics, the
        // 'bslma'-style allocator must be supplied explicitly (i.e., not
        // copied from rhs) or else it is given a default value.  Non-'bslma'
        // allocators ARE copied because that is the way the ISO standard is
        // currently written.

    ~AllocatorProxy();
        // Destroy this object.

    // MANIPULATORS
    pointer allocate(size_type n, const void *hint = 0);
        // Allocate enough (properly aligned) space for 'n' objects of type 'T'
        // by calling 'allocate' on the mechanism object.  The 'hint' argument
        // is ignored by this allocator type.

    template <class T>
    T *allocateN(T* p, size_type n)
        // Allocate (but do not initialize) 'n' objects of type 'T' using the
        // allocator returned by 'allocator'.  Return a pointer to the raw
        // memory that was allocated.  The 'T*' argument is used only to
        // determine the type of object being allocated; its value (usually
        // null) is not used.
    {
        return rebindAllocator(p).allocate(n);
    }

    void construct(pointer p, const value_type& val);
        // Copy-construct a 'T' object at the memory address specified by 'p'.
        // Do not directly allocate memory.  The behavior is undefined if 'p'
        // is not properly aligned for 'T'.

    void deallocate(pointer p, size_type n = 1);
        // Return memory previously allocated with 'allocate' to the underlying
        // mechanism object by calling 'deallocate' on the mechanism object.
        // The 'n' argument is ignored by this allocator type.

    template <class T>
    void deallocateN(T *p, size_type n)
        // Return 'n' objects of type 'T', starting at 'p' to the allocator
        // returned by 'allocator'.  Does not call destructors on the
        // deallocated objects.
    {
        rebindAllocator(p).deallocate(p, n);
    }

    void destroy(pointer p);
        // Call the 'T' destructor for the object pointed to by 'p'.  Do not
        // directly deallocate any memory.

    // ACCESSORS
    bool equalAllocator(const AllocatorProxy& rhs) const;
        // Returns 'this->allocator() == rhs.allocator()'.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                  // -----------------------------
                  // class AllocatorProxyBslmaBase
                  // -----------------------------

// CREATORS
template <class ALLOCATOR>
inline
AllocatorProxyBslmaBase<ALLOCATOR>::
AllocatorProxyBslmaBase(const ALLOCATOR& allocator)
: d_allocator(allocator)
{
}

template <class ALLOCATOR>
inline
AllocatorProxyBslmaBase<ALLOCATOR>::
AllocatorProxyBslmaBase(const AllocatorProxyBslmaBase&)
: d_allocator()
{
}

template <class ALLOCATOR>
inline
AllocatorProxyBslmaBase<ALLOCATOR>::~AllocatorProxyBslmaBase()
{
}

// MANIPULATORS
template <class ALLOCATOR>
inline
ALLOCATOR& AllocatorProxyBslmaBase<ALLOCATOR>::allocator()
{
    return d_allocator;
}

// ACCESSORS
template <class ALLOCATOR>
inline
const ALLOCATOR& AllocatorProxyBslmaBase<ALLOCATOR>::allocator() const
{
    return d_allocator;
}

template <class ALLOCATOR>
inline
typename AllocatorProxyBslmaBase<ALLOCATOR>::bslmaAllocatorPtr
AllocatorProxyBslmaBase<ALLOCATOR>::bslmaAllocator() const
{
    return d_allocator.mechanism();
}

                        // --------------------------------
                        // class AllocatorProxyNonBslmaBase
                        // --------------------------------

// CREATORS
template <class ALLOCATOR>
inline
AllocatorProxyNonBslmaBase<ALLOCATOR>::
AllocatorProxyNonBslmaBase(const ALLOCATOR& allocator)
: ALLOCATOR(allocator)
{
}

template <class ALLOCATOR>
inline
AllocatorProxyNonBslmaBase<ALLOCATOR>::
AllocatorProxyNonBslmaBase(const AllocatorProxyNonBslmaBase& rhs)
: ALLOCATOR(rhs.allocator())
{
}

template <class ALLOCATOR>
inline
AllocatorProxyNonBslmaBase<ALLOCATOR>::~AllocatorProxyNonBslmaBase()
{
}

// MANIPULATORS
template <class ALLOCATOR>
inline
ALLOCATOR& AllocatorProxyNonBslmaBase<ALLOCATOR>::allocator()
{
    return *this;
}

// ACCESSORS
template <class ALLOCATOR>
inline
const ALLOCATOR&
AllocatorProxyNonBslmaBase<ALLOCATOR>::allocator() const
{
    return *this;
}

template <class ALLOCATOR>
inline
typename AllocatorProxyNonBslmaBase<ALLOCATOR>::bslmaAllocatorPtr
AllocatorProxyNonBslmaBase<ALLOCATOR>::bslmaAllocator() const
{
    return 0;
}

                        // --------------------
                        // class AllocatorProxy
                        // --------------------

// CREATORS
template <class ALLOCATOR>
inline
AllocatorProxy<ALLOCATOR>::
AllocatorProxy(const ALLOCATOR& allocator)
: Base(allocator)
{
}

template <class ALLOCATOR>
inline
AllocatorProxy<ALLOCATOR>::
AllocatorProxy(const AllocatorProxy<ALLOCATOR>& rhs)
: Base(rhs)
{
}

template <class ALLOCATOR>
inline
AllocatorProxy<ALLOCATOR>::~AllocatorProxy()
{
}

// MANIPULATORS
template <class ALLOCATOR>
inline
typename AllocatorProxy<ALLOCATOR>::pointer
AllocatorProxy<ALLOCATOR>::allocate(size_type n, const void *hint)
{
    return this->allocator().allocate(n, hint);
}

template <class ALLOCATOR>
inline
void AllocatorProxy<ALLOCATOR>::deallocate(pointer p, size_type n)
{
    this->allocator().deallocate(p, n);
}

template <class ALLOCATOR>
inline
void AllocatorProxy<ALLOCATOR>::construct(pointer           p,
                                          const value_type& val)
{
    this->allocator().construct(p, val);
}

template <class ALLOCATOR>
inline
void AllocatorProxy<ALLOCATOR>::destroy(pointer p)
{
    this->allocator().destroy(p);
}

// ACCESSORS
template <class ALLOCATOR>
inline
bool AllocatorProxy<ALLOCATOR>::equalAllocator(const AllocatorProxy& rhs) const
{
    return this->allocator() == rhs.allocator();
}

}  // close package namespace

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslstl_AllocatorProxy
#undef bslstl_AllocatorProxy
#endif
#define bslstl_AllocatorProxy bslstl::AllocatorProxy
    // This alias is defined for backward compatibility.

#ifdef bslstl_AllocatorProxyBslmaBase
#undef bslstl_AllocatorProxyBslmaBase
#endif
#define bslstl_AllocatorProxyBslmaBase bslstl::AllocatorProxyBslmaBase
    // This alias is defined for backward compatibility.

#ifdef bslstl_AllocatorProxyNonBslmaBase
#undef bslstl_AllocatorProxyNonBslmaBase
#endif
#define bslstl_AllocatorProxyNonBslmaBase bslstl::AllocatorProxyNonBslmaBase
    // This alias is defined for backward compatibility.

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
