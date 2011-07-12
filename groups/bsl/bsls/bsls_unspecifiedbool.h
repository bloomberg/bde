// bsls_unspecifiedbool.h                                             -*-C++-*-
#ifndef INCLUDED_BSLS_UNSPECIFIEDBOOL
#define INCLUDED_BSLS_UNSPECIFIEDBOOL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility class supporting the unspecified bool idiom.
//
//@CLASSES:
//  bslsl_UnspecifiedBool: utility class template 
//
//@AUTHOR: Alisdair Meredith (ameredith1@bloomberg.net)
//
//@DESCRIPTION: This component provides a class template that can be used to
// manufacture an "unspecified boolean type".
//
////Usage
///------
//
///Example 1
///- - - - -
// A common requirement for "smart pointer" types is to emulate the native
// pointer types, and in particular support testing as a boolean value in 'if'
// and 'while' clauses.   Blah...:
//..
//  void explicitCastingExample()
//  {
//      bdema_ManagedPtr<A> a_mp1;
//      bdema_ManagedPtr<B> b_mp1(a_mp, static_cast<B*>(a_mp1.ptr()));
//..
// or even use the less safe "C"-style casts:
//..
//      bdema_ManagedPtr<A> a_mp2;
//      bdema_ManagedPtr<B> b_mp2(a_mp, (B*)(a_mp2.ptr()));
//
//  } // explicitCastingExample()
//..
// Note that when using dynamic cast, if the cast fails, the target managed
// pointer will be reset to an unset state, and the source will not be
// modified.  Consider for example the following snippet of code:
//..
//  void processPolymorphicObject(bdema_ManagedPtr<A> aPtr)
//  {
//      bdema_ManagedPtr<B> bPtr(aPtr, dynamic_cast<B*>(aPtr.ptr()));
//      if (bPtr) {
//          processBObject(bPtr);
//      }
//      else {
//          processAObject(aPtr);
//      }
//  }
//..
// If the value of 'aPtr' can be dynamically cast to 'B*' then ownership is
// transferred to 'bPtr', otherwise 'aPtr' is to be modified.  As previously
// stated, the managed instance will be destroyed correctly regardless of how
// it is cast.

namespace BloombergLP {


            // =====================================================
            // struct blsl_UnspecifiedBoolHelper
            // =====================================================

template<class BSLS_HOST_TYPE>
struct bsls_UnspecifiedBool {
    // This 'struct' provides a member, 'd_member', whose pointer-to-member may
    // be used as an "unspecified boolean type" for implicit conversion
    // operators.

    typedef int bsls_UnspecifiedBool::* BoolType;

    static BoolType falseValue();
    static BoolType trueValue();

    int d_member;
        // This data member is used solely for taking its address to return a
        // non-null pointer-to-member.  Note that the *value* of 'd_member' is
        // not used.
};


// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

template<class BSLS_HOST_TYPE>
inline int bsls_UnspecifiedBool<BSLS_HOST_TYPE>::* 
bsls_UnspecifiedBool<BSLS_HOST_TYPE>::falseValue()
{
    return 0;
}

template<class BSLS_HOST_TYPE>
inline int bsls_UnspecifiedBool<BSLS_HOST_TYPE>::* 
bsls_UnspecifiedBool<BSLS_HOST_TYPE>::trueValue()
{
    return &bsls_UnspecifiedBool::d_member;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
