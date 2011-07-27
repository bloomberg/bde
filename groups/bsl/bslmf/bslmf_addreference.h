// bslmf_addreference.h                                               -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDREFERENCE
#define INCLUDED_BSLMF_ADDREFERENCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for adding "reference-ness" to a type.
//
//@CLASSES:
//  bslmf_AddReference: meta-function to form a reference to a type
//
//@AUTHOR: Alisdair Meredith (ameredit)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template 'struct',
// 'bslmf_AddReference', that is used to define a reference type from the type
// supplied as its single template type parameter.  Types that are 'void' or
// already reference types are unmodified.
//
////Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: A Simple Wrapper Class
///- - - - - - - - - - - - - - - - -
// First, let us write a simple class that can wrap any other type:
//..
//  template <class TYPE>
//  class Wrapper {
//    private:
//      // DATA
//      TYPE d_data;
//
//    public:
//      // TYPES
//      typedef typename bslmf_AddReference<TYPE>::Type WrappedType;
//
//      // CREATORS
//      Wrapper(TYPE value) : d_data(value) {}
//          // Create a 'Wrapper' object having the specified 'value'.
//
//      //! ~Wrapper() = default;
//          // Destroy this object.
//..
// Then, we would like to expose access to the wrapped element through a
// method that returns a reference to the data member 'd_data'.  However,
// there would be a problem if the user supplied a parameterized type 'TYPE'
// that is a reference type, as references-to-references were not permitted by
// the language (prior the C++11 standard).  We can resolve such problems
// using the meta-function 'bslmf_AddReference'.
//..
//      // MANIPULATORS
//      typename bslmf_AddReference<TYPE>::Type value()
//      {
//          return d_data;
//      }
//..
// Next, we supply an accessor function, 'value', that similarly wraps the
// parameterized type 'TYPE' with the 'bslmf_AddReference' meta-function.
// In this case we must remember to const-quality 'TYPE' before passing it
// on to the meta-function.
//..
//      // ACCESSORS
//      typename bslmf_AddReference<const TYPE>::Type value() const
//      {
//          return d_data;
//      }
//  };
//..
// Now, we write a test function, 'runTest', to verify our simple wrapper
// type.  We start by wrapping a simple 'int' value:
//..
//  void runTests()
//  {
//      int i = 42;
//
//      Wrapper<int> ti(i);  const Wrapper<int>& TI = ti;
//      assert(42 == i);
//      assert(42 == TI.value());
//
//      ti.value() = 13;
//      assert(42 == i);
//      assert(13 == TI.value());
//..
// Finally, we test 'Wrapper' with a reference type:
//..
//      Wrapper<int&> tr(i);  const Wrapper<int&>& TR = tr;
//      assert(42 == i);
//      assert(42 == TR.value());
//
//      tr.value() = 13;
//      assert(13 == i);
//      assert(13 == TR.value());
//
//      i = 42;
//      assert(42 == i);
//      assert(42 == TR.value());
//  }
//..
///Example 2: Expected Results
///- - - - - - - - - - - - - -
// For this example, the associated comments below indicate the expected type
// of 'bslmf_AddReference::Type' for a broad range of parameterized types:
//..
//  struct MyType {};
//  typedef MyType& MyTypeRef;
//
//  bslmf_AddReference<int             >::Type x1; // int&
//  bslmf_AddReference<int&            >::Type x2; // int&
//  bslmf_AddReference<int volatile    >::Type x3; // volatile int&
//  bslmf_AddReference<int volatile&   >::Type x4; // volatile int&
//
//  bslmf_AddReference<MyType          >::Type     // MyType&
//  bslmf_AddReference<MyType&         >::Type     // MyType&
//  bslmf_AddReference<MyTypeRef       >::Type     // MyType&
//  bslmf_AddReference<MyType const    >::Type     // const MyType&
//  bslmf_AddReference<MyType const&   >::Type     // const MyType&
//  bslmf_AddReference<const MyTypeRef >::Type     // MyType&
//  bslmf_AddReference<const MyTypeRef&>::Type     // MyType& (REQUIRES C++11)
//
//  bslmf_AddReference<void            >::Type     // void
//  bslmf_AddReference<void *          >::Type     // void *&
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

                        // =========================
                        // struct bslmf_AddReference
                        // =========================


template <class BSLMF_TYPE>
struct bslmf_AddReference {
    // This meta-function class defines a typedef, 'Type', that is an alias for
    // a reference to the parameterized 'BSLMF_TYPE'.  References to
    // cv-qualified 'void' will produce the original 'void' type and not a
    // reference (see specializations below).  References-to-references
    // "collapse" to produce an alias to the original reference type, which is
    // the revised rule according to the C++11 standard.  Note that there is no
    // requirement that the parameterized 'BSLMF_TYPE' be a complete type.

    typedef BSLMF_TYPE& Type;
        // Alias for a reference to the parameterized 'BSLMF_TYPE'
};

template <class BSLMF_TYPE>
struct bslmf_AddReference<BSLMF_TYPE &> {
    // Specialization to ensure reference-to-reference collapses to a plain
    // reference.

    typedef BSLMF_TYPE& Type;
};

template <>
struct bslmf_AddReference<void> {
    // This specialization of 'bslmf_AddReference' for type 'void' is needed as
    // the language does not support references to 'void'.

    typedef void Type;
};

template <>
struct bslmf_AddReference<const void> {
    // This specialization of 'bslmf_AddReference' for type 'const void' is
    // needed as the language does not support references to 'const void'.

    typedef const void Type;
};

template <>
struct bslmf_AddReference<volatile void> {
    // This specialization of 'bslmf_AddReference' for type 'volatile void' is
    // needed as the language does not support references to 'volatile void'.

    typedef volatile void Type;
};

template <>
struct bslmf_AddReference<const volatile void> {
    // This specialization of 'bslmf_AddReference' for type 'const volatile
    // void' is needed as the language does not support references to 'const
    // volatile void'.

    typedef const volatile void Type;
};

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
