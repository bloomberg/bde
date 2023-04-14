// bslmf_addreference.h                                               -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDREFERENCE
#define INCLUDED_BSLMF_ADDREFERENCE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for adding "reference-ness" to a type.
//
//@CLASSES:
//  bslmf::AddReference: meta-function to form a reference to a type
//
//@DESCRIPTION: This component defines a simple template 'struct',
// 'bslmf::AddReference', that is used to define a reference type from the type
// supplied as its single template type parameter.  Types that are 'void' or
// already reference types are unmodified.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Simple Wrapper Class
///- - - - - - - - - - - - - - - - -
// First, let us write a simple class that can wrap any other type:
//..
//  template <class t_TYPE>
//  class Wrapper {
//    private:
//      // DATA
//      t_TYPE d_data;
//
//    public:
//      // TYPES
//      typedef typename bslmf::AddReference<t_TYPE>::Type WrappedType;
//
//      // CREATORS
//      Wrapper(t_TYPE value) : d_data(value) {}                    // IMPLICIT
//          // Create a 'Wrapper' object having the specified 'value'.
//
//      //! ~Wrapper() = default;
//          // Destroy this object.
//..
// Then, we would like to expose access to the wrapped element through a method
// that returns a reference to the data member 'd_data'.  However, there would
// be a problem if the user supplied a parameterized type 't_TYPE' that is a
// reference type, as references-to-references were not permitted by the
// language (prior the C++11 standard).  We can resolve such problems using the
// meta-function 'bslmf::AddReference'.
//..
//  // MANIPULATORS
//  typename bslmf::AddReference<t_TYPE>::Type value()
//  {
//      return d_data;
//  }
//..
// Next, we supply an accessor function, 'value', that similarly wraps the
// parameterized type 't_TYPE' with the 'bslmf::AddReference' meta-function.
// In this case we must remember to const-quality 't_TYPE' before passing it on
// to the meta-function.
//..
//      // ACCESSORS
//      typename bslmf::AddReference<const t_TYPE>::Type value() const
//      {
//          return d_data;
//      }
//  };
//..
// Now, we write a test function, 'runTest', to verify our simple wrapper type.
// We start by wrapping a simple 'int' value:
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
//
///Example 2: Expected Results
///- - - - - - - - - - - - - -
// For this example, the associated comments below indicate the expected type
// of 'bslmf::AddReference::Type' for a broad range of parameterized types:
//..
//  struct MyType {};
//  typedef MyType& MyTypeRef;
//
//  bslmf::AddReference<int             >::Type x1; // int&
//  bslmf::AddReference<int&            >::Type x2; // int&
//  bslmf::AddReference<int volatile    >::Type x3; // volatile int&
//  bslmf::AddReference<int volatile&   >::Type x4; // volatile int&
//
//  bslmf::AddReference<MyType          >::Type     // MyType&
//  bslmf::AddReference<MyType&         >::Type     // MyType&
//  bslmf::AddReference<MyTypeRef       >::Type     // MyType&
//  bslmf::AddReference<MyType const    >::Type     // const MyType&
//  bslmf::AddReference<MyType const&   >::Type     // const MyType&
//  bslmf::AddReference<const MyTypeRef >::Type     // MyType&
//  bslmf::AddReference<const MyTypeRef&>::Type     // MyType& (REQUIRES C++11)
//
//  bslmf::AddReference<void            >::Type     // void
//  bslmf::AddReference<void *          >::Type     // void *&
//..

#include <bslscm_version.h>

#include <bslmf_addlvaluereference.h>

namespace BloombergLP {

namespace bslmf {

                        // ===================
                        // struct AddReference
                        // ===================

template <class t_TYPE>
struct AddReference {
    // This meta-function class defines a typedef, 'Type', that is an alias for
    // a reference to the parameterized 't_TYPE'.  References to cv-qualified
    // 'void' will produce the original 'void' type and not a reference (see
    // specializations below).  References-to-references "collapse" to produce
    // an alias to the original reference type, which is the revised rule
    // according to the C++11 standard.  Note that there is no requirement that
    // the parameterized 't_TYPE' be a complete type.

    typedef typename bsl::add_lvalue_reference<t_TYPE>::type Type;
};

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslmf_AddReference
#undef bslmf_AddReference
#endif
#define bslmf_AddReference bslmf::AddReference
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

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
