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

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
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

                 // ====================================
                 // class SharedPtrAllocateInplaceRep
                 // ====================================

template <class TYPE, class ALLOCATOR>
class SharedPtrAllocateInplaceRep : public BloombergLP::bslma::SharedPtrRep {
    // This class provides a concrete implementation of the 'SharedPtrRep'
    // protocol for out-of-place instances of the parameterized 'TYPE'.  Upon
    // destruction of this object, the parameterized 'DELETER' type is invoked
    // on the pointer to the shared object.

    typedef typename ALLOCATOR::template
                                     rebind<SharedPtrAllocateInplaceRep>::other
                                                              ReboundAllocator;

    // DATA
    ReboundAllocator d_allocator; // copy of the allocator for this object

    TYPE             d_instance;  // beginning of the in-place buffer
                                  // Note that this must be last in this layout
                                  // to allow for the possibility of creating
                                  // in-place uninitialized buffer, where it is
                                  // possible to access memory beyond the
                                  // 'd_instance' footprint (refer to
                                  // 'bsl::share_ptr::
                                  // createInplaceUninitializedBuffer' for
                                  // sample usage)


  private:
    // NOT IMPLEMENTED
    SharedPtrAllocateInplaceRep(const SharedPtrAllocateInplaceRep&);
    SharedPtrAllocateInplaceRep& operator=(const SharedPtrAllocateInplaceRep&);

    // PRIVATE CREATORS
    ~SharedPtrAllocateInplaceRep();
        // Destroy this representation object and if the shared object has not
        // been deleted, delete the shared object using the associated deleter.
        // Note that this destructor is never called explicitly.  Instead,
        // 'disposeObject' destroys the shared object object and 'disposeRep'
        // deallocates this representation object.

  public:
    // CREATORS
#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
# if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class... ARGS>
    explicit SharedPtrAllocateInplaceRep(const ReboundAllocator& basicAllocator,
                                 ARGS&&...  args);
        // Create a 'SharedPtrAllocateInplaceRep' object having an "in-place" instance
        // of the parameterized 'TYPE' using the 'TYPE' constructor that takes
        // the specified arguments, 'args...'.  Use the specified
        // 'basicAllocator' to supply memory and, upon a call to 'disposeRep',
        // to destroy this representation (and the "in-place" shared object).
# else
    template <class... ARGS>
    explicit SharedPtrAllocateInplaceRep(const ReboundAllocator& basicAllocator,
                                 const ARGS&...          args);
        // Create a 'SharedPtrAllocateInplaceRep' object having an "in-place" instance
        // of the parameterized 'TYPE' using the 'TYPE' constructor that takes
        // the specified arguments, 'args...'.  Use the specified
        // 'basicAllocator' to supply memory and, upon a call to 'disposeRep',
        // to destroy this representation (and the "in-place" shared object).

# endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#else
    explicit SharedPtrAllocateInplaceRep(const ReboundAllocator& basicAllocator);
        // Create a 'SharedPtrAllocateInplaceRep' object having an "in-place"
        // default-constructed instance of the parameterized 'TYPE'.  Use the
        // specified 'basicAllocator' to supply memory and, upon a call to
        // 'disposeRep', to destroy this representation (and the "in-place"
        // shared object) .

    template <class A1>
    SharedPtrAllocateInplaceRep(const ReboundAllocator& basicAllocator, const A1& a1);
    template <class A1, class A2>
    SharedPtrAllocateInplaceRep(const ReboundAllocator& basicAllocator,
                        const A1&         a1,
                        const A2&         a2);
    template <class A1, class A2, class A3>
    SharedPtrAllocateInplaceRep(const ReboundAllocator& basicAllocator,
                        const A1&         a1,
                        const A2&         a2,
                        const A3&         a3);
    template <class A1, class A2, class A3, class A4>
    SharedPtrAllocateInplaceRep(const ReboundAllocator& basicAllocator,
                        const A1&         a1,
                        const A2&         a2,
                        const A3&         a3,
                        const A4&         a4);
    template <class A1, class A2, class A3, class A4, class A5>
    SharedPtrAllocateInplaceRep(const ReboundAllocator& basicAllocator,
                        const A1&         a1,
                        const A2&         a2,
                        const A3&         a3,
                        const A4&         a4,
                        const A5&         a5);
    template <class A1, class A2, class A3, class A4, class A5, class A6>
    SharedPtrAllocateInplaceRep(const ReboundAllocator& basicAllocator,
                        const A1&         a1,
                        const A2&         a2,
                        const A3&         a3,
                        const A4&         a4,
                        const A5&         a5,
                        const A6&         a6);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
             class A7>
    SharedPtrAllocateInplaceRep(const ReboundAllocator& basicAllocator,
                        const A1&         a1,
                        const A2&         a2,
                        const A3&         a3,
                        const A4&         a4,
                        const A5&         a5,
                        const A6&         a6,
                        const A7&         a7);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8>
    SharedPtrAllocateInplaceRep(const ReboundAllocator& basicAllocator,
                        const A1&         a1,
                        const A2&         a2,
                        const A3&         a3,
                        const A4&         a4,
                        const A5&         a5,
                        const A6&         a6,
                        const A7&         a7,
                        const A8&         a8);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9>
    SharedPtrAllocateInplaceRep(const ReboundAllocator& basicAllocator,
                        const A1&         a1,
                        const A2&         a2,
                        const A3&         a3,
                        const A4&         a4,
                        const A5&         a5,
                        const A6&         a6,
                        const A7&         a7,
                        const A8&         a8,
                        const A9&         a9);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10>
    SharedPtrAllocateInplaceRep(const ReboundAllocator& basicAllocator,
                        const A1&         a1,
                        const A2&         a2,
                        const A3&         a3,
                        const A4&         a4,
                        const A5&         a5,
                        const A6&         a6,
                        const A7&         a7,
                        const A8&         a8,
                        const A9&         a9,
                        const A10&        a10);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11>
    SharedPtrAllocateInplaceRep(const ReboundAllocator& basicAllocator,
                        const A1&         a1,
                        const A2&         a2,
                        const A3&         a3,
                        const A4&         a4,
                        const A5&         a5,
                        const A6&         a6,
                        const A7&         a7,
                        const A8&         a8,
                        const A9&         a9,
                        const A10&        a10,
                        const A11&        a11);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12>
    SharedPtrAllocateInplaceRep(const ReboundAllocator& basicAllocator,
                        const A1&         a1,
                        const A2&         a2,
                        const A3&         a3,
                        const A4&         a4,
                        const A5&         a5,
                        const A6&         a6,
                        const A7&         a7,
                        const A8&         a8,
                        const A9&         a9,
                        const A10&        a10,
                        const A11&        a11,
                        const A12&        a12);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12,
              class A13>
    SharedPtrAllocateInplaceRep(const ReboundAllocator& basicAllocator,
                        const A1&         a1,
                        const A2&         a2,
                        const A3&         a3,
                        const A4&         a4,
                        const A5&         a5,
                        const A6&         a6,
                        const A7&         a7,
                        const A8&         a8,
                        const A9&         a9,
                        const A10&        a10,
                        const A11&        a11,
                        const A12&        a12,
                        const A13&        a13);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12,
              class A13, class A14>
    SharedPtrAllocateInplaceRep(const ReboundAllocator& basicAllocator,
                        const A1&         a1,
                        const A2&         a2,
                        const A3&         a3,
                        const A4&         a4,
                        const A5&         a5,
                        const A6&         a6,
                        const A7&         a7,
                        const A8&         a8,
                        const A9&         a9,
                        const A10&        a10,
                        const A11&        a11,
                        const A12&        a12,
                        const A13&        a13,
                        const A14&        a14);
        // Create a 'SharedPtrAllocateInplaceRep' object having an "in-place" instance
        // of the parameterized 'TYPE' using the 'TYPE' constructor that takes
        // the specified arguments, 'a1' up to 'aN', where 'N' (at most 14) is
        // the number of arguments passed to this method.  Use the specified
        // 'basicAllocator' to supply memory and, upon a call to 'disposeRep',
        // to destroy this representation (and the "in-place" shared object).
#endif


    // MANIPULATORS
    TYPE *ptr();
        // Return the address of the modifiable shared object to which this
        // object refers.

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

    void *getDeleter(const std::type_info& type);
        // Return a null pointer.  Note that an in-place representation for a
        // shared pointer can never store a user-supplied deleter, as there is
        // no function that might try to create one.

    // ACCESSORS
    virtual void *originalPtr() const;
        // Return the (untyped) address of the modifiable shared object to
        // which this object refers.
};


// ============================================================================
//                      INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                  // ------------------------------------
                  // class SharedPtrAllocateInplaceRep
                  // ------------------------------------

// CREATORS
#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
# if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE, class ALLOCATOR>
template <class... ARGS>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator,
                                        ARGS&&...               args)
: d_allocator(basicAllocator)
, d_instance(::std::forward<ARGS>(args)...)
{
}
# else
template <class TYPE, class ALLOCATOR>
template <class... ARGS>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator,
                                        const ARGS&...  args)
: d_allocator(basicAllocator)
, d_instance(args...)
{
}
# endif  
#else
template <class TYPE, class ALLOCATOR>
inline
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator)
: d_allocator(basicAllocator)
{
}

template <class TYPE, class ALLOCATOR>
template <class A1>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator,
                                        const A1&  a1)
: d_allocator(basicAllocator)
, d_instance(a1)
{
}

template <class TYPE, class ALLOCATOR>
template <class A1, class A2>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator,
                                        const A1&  a1,
                                        const A2&  a2)
: d_allocator(basicAllocator)
, d_instance(a1, a2)
{
}

template <class TYPE, class ALLOCATOR>
template <class A1, class A2, class A3>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator,
                                        const A1&  a1,
                                        const A2&  a2,
                                        const A3&  a3)
: d_allocator(basicAllocator)
, d_instance(a1, a2, a3)
{
}

template <class TYPE, class ALLOCATOR>
template <class A1, class A2, class A3, class A4>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator,
                                        const A1&  a1,
                                        const A2&  a2,
                                        const A3&  a3,
                                        const A4&  a4)
: d_allocator(basicAllocator)
, d_instance(a1, a2, a3, a4)
{
}

template <class TYPE, class ALLOCATOR>
template <class A1, class A2, class A3, class A4, class A5>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator,
                                        const A1&  a1,
                                        const A2&  a2,
                                        const A3&  a3,
                                        const A4&  a4,
                                        const A5&  a5)
: d_allocator(basicAllocator)
, d_instance(a1, a2, a3, a4, a5)
{
}

template <class TYPE, class ALLOCATOR>
template <class A1, class A2, class A3, class A4, class A5, class A6>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator,
                                        const A1&  a1,
                                        const A2&  a2,
                                        const A3&  a3,
                                        const A4&  a4,
                                        const A5&  a5,
                                        const A6&  a6)
: d_allocator(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6)
{
}

template <class TYPE, class ALLOCATOR>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator,
                                        const A1&  a1,
                                        const A2&  a2,
                                        const A3&  a3,
                                        const A4&  a4,
                                        const A5&  a5,
                                        const A6&  a6,
                                        const A7&  a7)
: d_allocator(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7)
{
}

template <class TYPE, class ALLOCATOR>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator,
                                        const A1&  a1,
                                        const A2&  a2,
                                        const A3&  a3,
                                        const A4&  a4,
                                        const A5&  a5,
                                        const A6&  a6,
                                        const A7&  a7,
                                        const A8&  a8)
: d_allocator(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8)
{
}

template <class TYPE, class ALLOCATOR>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator,
                                        const A1&  a1,
                                        const A2&  a2,
                                        const A3&  a3,
                                        const A4&  a4,
                                        const A5&  a5,
                                        const A6&  a6,
                                        const A7&  a7,
                                        const A8&  a8,
                                        const A9&  a9)
: d_allocator(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9)
{
}

template <class TYPE, class ALLOCATOR>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator,
                                        const A1&  a1,
                                        const A2&  a2,
                                        const A3&  a3,
                                        const A4&  a4,
                                        const A5&  a5,
                                        const A6&  a6,
                                        const A7&  a7,
                                        const A8&  a8,
                                        const A9&  a9,
                                        const A10& a10)
: d_allocator(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
{
}

template <class TYPE, class ALLOCATOR>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator,
                                        const A1&  a1,
                                        const A2&  a2,
                                        const A3&  a3,
                                        const A4&  a4,
                                        const A5&  a5,
                                        const A6&  a6,
                                        const A7&  a7,
                                        const A8&  a8,
                                        const A9&  a9,
                                        const A10& a10,
                                        const A11& a11)
: d_allocator(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)
{
}

template <class TYPE, class ALLOCATOR>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator,
                                        const A1&  a1,
                                        const A2&  a2,
                                        const A3&  a3,
                                        const A4&  a4,
                                        const A5&  a5,
                                        const A6&  a6,
                                        const A7&  a7,
                                        const A8&  a8,
                                        const A9&  a9,
                                        const A10& a10,
                                        const A11& a11,
                                        const A12& a12)
: d_allocator(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)
{
}

template <class TYPE, class ALLOCATOR>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator,
                                        const A1&  a1,
                                        const A2&  a2,
                                        const A3&  a3,
                                        const A4&  a4,
                                        const A5&  a5,
                                        const A6&  a6,
                                        const A7&  a7,
                                        const A8&  a8,
                                        const A9&  a9,
                                        const A10& a10,
                                        const A11& a11,
                                        const A12& a12,
                                        const A13& a13)
: d_allocator(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)
{
}

template <class TYPE, class ALLOCATOR>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::SharedPtrAllocateInplaceRep(
                                        const ReboundAllocator& basicAllocator,
                                        const A1&  a1,
                                        const A2&  a2,
                                        const A3&  a3,
                                        const A4&  a4,
                                        const A5&  a5,
                                        const A6&  a6,
                                        const A7&  a7,
                                        const A8&  a8,
                                        const A9&  a9,
                                        const A10& a10,
                                        const A11& a11,
                                        const A12& a12,
                                        const A13& a13,
                                        const A14& a14)
: d_allocator(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)
{
}
#endif

template <class TYPE, class ALLOCATOR>
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::
~SharedPtrAllocateInplaceRep()
{
    BSLS_ASSERT(0);
}


// MANIPULATORS
template <class TYPE, class ALLOCATOR>
inline
void SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::disposeRep()
{
    ReboundAllocator alloc(d_allocator);
    this->d_allocator.~ReboundAllocator();
    alloc.deallocate(this, 1);
}

template <class TYPE, class ALLOCATOR>
void SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::disposeObject()
{
    d_instance.~TYPE();
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
    return &d_instance;
}

// ACCESSORS
template <class TYPE, class ALLOCATOR>
inline
void *
SharedPtrAllocateInplaceRep<TYPE, ALLOCATOR>::originalPtr() const
{
    return const_cast<void *>(static_cast<const void *>(&d_instance));
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
