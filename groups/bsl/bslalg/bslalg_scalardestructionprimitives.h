// bslalg_scalardestructionprimitives.h                               -*-C++-*-
#ifndef INCLUDED_BSLALG_SCALARDESTRUCTIONPRIMITIVES
#define INCLUDED_BSLALG_SCALARDESTRUCTIONPRIMITIVES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide primitive algorithms that destroy scalars.
//
//@CLASSES:
//  bslalg::ScalarDestructionPrimitives: namespace for scalar algorithms
//
//@SEE_ALSO: bslalg_scalarprimitives, bslalg_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides utilities to destroy scalars with a
// uniform interface, but select a different implementation according to the
// traits possessed by the underlying type.
//
// The trait under consideration by this component is:
//..
//  Trait                             Note
//  -------------------------------   -------------------------------------
//  bslalg::TypeTraitBitwiseCopyable  Expressed in English as "TYPE has the
//                                    bit-wise copyable trait", or "TYPE is
//                                    bit-wise copyable", this trait also
//                                    implies that destructor calls can be
//                                    elided with no effect on observable
//                                    behavior.
//
//..
//
///Usage
///-----
// In this section we show intended use of this component.  Note that this
// component is for use by the 'bslstl' package.  Other clients should use the
// STL algorithms (in header '<algorithm>' and '<memory>').
//
///Example 1: Destroy 'int' and an Integer Wrapper
///- - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we will use 'bslalg::ScalarDestructionPrimitives' to
// destroy both a scalar integer and a 'MyInteger' type object.  Calling the
// 'destory' method on a scalar integer is a no-op while calling the 'destroy'
// method on an object of 'MyInteger' class invokes the destructor of the
// object.
//
// First, we define a 'MyInteger' class that represents an integer value:
//..
//  class MyInteger {
//      // This class represents an integer value.
//
//      // DATA
//      int d_intValue;  // integer value
//
//    public:
//      // CREATORS
//      MyInteger();
//          // Create a 'MyInteger' object having integer value '0'.
//
//      explicit MyInteger(int value);
//          // Create a 'MyInteger' object having the specified 'value'.
//
//      ~MyInteger();
//          // Destroy this object.
//
//      // ACCESSORS
//      int getValue() const;
//  };
//..
// Then, we create an object, 'myInteger', of type 'MyInteger':
//..
//  bsls::ObjectBuffer<MyInteger> buffer;
//  MyInteger *myInteger = &buffer.object();
//  new (myInteger) MyInteger(1);
//..
// Notice that we use an 'ObjectBuffer' to allow us to safely invoke the
// destructor explicitly.
//
// Now, we define a primitive integer:
//..
//  int scalarInteger = 2;
//..
// Finally, we use the uniform 'bslalg::ScalarDestructionPrimitives:destroy'
// method to destroy both 'myInteger' and 'scalarInteger':
//..
//  bslalg::ScalarDestructionPrimitives::destroy(myInteger);
//  bslalg::ScalarDestructionPrimitives::destroy(&scalarInteger);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>  // 'memset', 'memcpy', 'memmove'
#define INCLUDED_CSTRING
#endif

namespace BloombergLP {

namespace bslalg {

                  // ==================================
                  // struct ScalarDestructionPrimitives
                  // ==================================

struct ScalarDestructionPrimitives {
    // This 'struct' provides a namespace for a suite of utility functions that
    // destroy elements of the parameterized type 'TARGET_TYPE'.  Depending on
    // the traits of 'TARGET_TYPE', the destructor may be invoked or not (i.e.,
    // optimized away as a no-op).

  private:
    // PRIVATE CLASS METHODS
    template <typename TARGET_TYPE>
    static void destroy(TARGET_TYPE *address, bslmf::MetaInt<1>);
    template <typename TARGET_TYPE>
    static void destroy(TARGET_TYPE *address, bslmf::MetaInt<0>);
        // Destroy the object of the parameterized 'TARGET_TYPE' at the
        // specified 'address' if the second argument is of type
        // 'bslmf::MetaInt<0>', and do nothing otherwise.  This method is a
        // no-op if the second argument is of type 'bslmf::MetaInt<1>',
        // indicating that the object at 'address' is bit-wise copyable.  Note
        // that the second argument is for overload resolution only and its
        // value is ignored.

  public:
    // CLASS METHODS
    template <typename TARGET_TYPE>
    static void destroy(TARGET_TYPE *object);
        // Destroy the specified 'object' of the parameterized 'TARGET_TYPE',
        // as if by calling the 'TARGET_TYPE' destructor, but do not deallocate
        // the memory occupied by 'object'.  Note that the destructor may
        // deallocate other memory owned by 'object'.  Also note that this
        // function is a no-op if the 'TARGET_TYPE' has the trivial destructor
        // trait.
};

// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ===========================================================================

                  // ----------------------------------
                  // struct ScalarDestructionPrimitives
                  // ----------------------------------

// PRIVATE CLASS METHODS
template <typename TARGET_TYPE>
inline
void ScalarDestructionPrimitives::destroy(TARGET_TYPE *address,
                                          bslmf::MetaInt<1>)
{
    // No-op.

#ifdef BDE_BUILD_TARGET_SAFE
    std::memset(address, 0xa5, sizeof *address);
#else
    (void) address;
#endif
}

}  // close package namespace

#ifdef BSLS_PLATFORM_CMP_MSVC
#pragma warning( push )           //  For some reason, VC2008 does not detect
#pragma warning( disable : 4100 ) //  that 'address' is used.
#endif

namespace bslalg {

template <typename TARGET_TYPE>
inline
void ScalarDestructionPrimitives::destroy(TARGET_TYPE *address,
                                          bslmf::MetaInt<0>)
{
#ifndef BSLS_PLATFORM_CMP_SUN
    address->~TARGET_TYPE();
#else
    // Workaround for a bug in Sun's CC whereby destructors cannot be called on
    // 'const' objects of polymorphic types.

    typedef bslmf::RemoveCvq<TARGET_TYPE>::Type NoCvqType;
    const_cast<NoCvqType *>(address)->~NoCvqType();
#endif
}

}  // close package namespace

#ifdef BSLS_PLATFORM_CMP_MSVC
#pragma warning( pop )
#endif

namespace bslalg {

// CLASS METHODS
template <typename TARGET_TYPE>
inline
void ScalarDestructionPrimitives::destroy(TARGET_TYPE *object)
{
    BSLS_ASSERT_SAFE(object);

    typedef typename HasTrait<TARGET_TYPE,
                              TypeTraitBitwiseCopyable>::Type Trait;
    destroy(object, Trait());
}

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bslalg::ScalarDestructionPrimitives bslalg_ScalarDestructionPrimitives;
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD_COMPATIBILITY

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
