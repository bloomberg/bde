// bsltf_allocemplacabletesttype.h                                    -*-C++-*-
#ifndef INCLUDED_BSLTF_ALLOCEMPLACABLETESTTYPE
#define INCLUDED_BSLTF_ALLOCEMPLACABLETESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an allocating test class used to test 'emplace' methods.
//
//@CLASSES:
//  bsltf::AllocEmplacableTestType: allocating test class with 0..14 arguments
//
//@SEE_ALSO: bsltf_allocargumenttype, bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a (value-semantic) attribute class,
// 'bsltf::AllocEmplacableTestType', that is used to ensure that arguments are
// forwarded correctly to a type's constructor.  This component is similar to
// 'bsltf_emplacabletesttype', but provides a type that allocates on
// construction.
//
///Attributes
///----------
//..
//  Name     Type                           Default
//  -------  -----------------------------  -------
//  arg01    bsltf::AllocArgumentType< 1>   -1
//  arg02    bsltf::AllocArgumentType< 2>   -1
//  arg03    bsltf::AllocArgumentType< 3>   -1
//  arg04    bsltf::AllocArgumentType< 4>   -1
//  arg05    bsltf::AllocArgumentType< 5>   -1
//  arg06    bsltf::AllocArgumentType< 6>   -1
//  arg07    bsltf::AllocArgumentType< 7>   -1
//  arg08    bsltf::AllocArgumentType< 8>   -1
//  arg09    bsltf::AllocArgumentType< 9>   -1
//  arg10    bsltf::AllocArgumentType<10>   -1
//  arg11    bsltf::AllocArgumentType<11>   -1
//  arg12    bsltf::AllocArgumentType<12>   -1
//  arg13    bsltf::AllocArgumentType<13>   -1
//  arg14    bsltf::AllocArgumentType<14>   -1
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Testing Methods With Argument Forwarding
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we will utilize 'bsltf::AllocEmplacableTestType' to
// test the implementation of a container's 'emplace' method.
//
// First, we create an elided definition of a container class, 'MyContainer',
// and show the signature of the 'emplace' method we intend to test:
//..
//  // container.h
//  // -----------
//  template <class TYPE>
//  class Container {
//      // This class template implements a value-semantic container type
//      // holding elements of the (template parameter) type 'TYPE'.  This
//      // class provides an 'emplace' method that constructs the element by
//      // forwarding a variable number of arguments to the 'TYPE' constructor.
//
//      // ...
//
//      // MANIPULATORS
//      template <class... Args>
//      void emplace(Args&&... arguments);
//          // Insert into this container a newly created 'TYPE' object,
//          // constructed by forwarding the specified (variable number of)
//          // 'arguments' to the corresponding constructor of 'TYPE'.
//
//      // ...
//  };
//..
// Then, we provide test machinery that will invoke the 'emplace' method with
// variable number of arguments:
//..
//  // container.t.cpp
//  // ---------------
//
//  template <class T>
//  bslmf::MovableRef<T> forwardCtorArg(T& argument, bsl::true_type);
//      // Return 'bslmf::MovableRef' to the specified 'argument'.
//
//  template <class T>
//  const T& forwardCtorArg(T& argument, bsl::false_type)
//      // Return a reference providing non-modifiable access to the
//      // specified 'argument'.
//
//  template <int N_ARGS, bool MOVE_ARG_01, bool MOVE_ARG_02>
//  void testCaseHelper()
//      // Call 'emplace' on the container and verify that value was correctly
//      // constructed and inserted into the container.  Forward (template
//      // parameter) 'N_ARGS' arguments to the 'emplace' method and ensure 1)
//      // that values are properly passed to the constructor of
//      // 'bsltf::AllocEmplacableTestType', 2) that the allocator is correctly
//      // configured for each argument in the newly inserted element in
//      // 'target', and 3) that the arguments are forwarded using copy
//      // ('false') or move semantics ('true') based on bool template
//      // parameters 'MOVE_ARG_01' ...  'MOVE_ARG_02'.
//  {
//      bslma::TestAllocator ta;
//..
// Here, we use 'AllocEmplacableTestType' as the contained type to ensure the
// arguments to the 'emplace' method are correctly forwarded to the contained
// type's constructor:
//..
//      Container<bsltf::AllocEmplacableTestType>        mX(&ta);
//      const Container<bsltf::AllocEmplacableTestType>& X = mX;
//
//      // Prepare the arguments
//      bslma::TestAllocator aa("args", veryVeryVeryVerbose);
//
//      bsltf::AllocArgumentType<1> A01(18, &aa);
//      bsltf::AllocArgumentType<2> A02(33, &aa);
//..
// Then, we call 'emplace' supplying test arguments, which should call the
// correct constructor of 'AllocEmplacableTestType' (which we will later
// verify):
//..
//      const bsl::integral_constant<bool, MOVE_ARG_01> MOVE_01 = {};
//      const bsl::integral_constant<bool, MOVE_ARG_02> MOVE_02 = {};
//      switch (N_ARGS) {
//        case 0: {
//          mX.emplace();
//        } break;
//        case 1: {
//          mX.emplace(forwardCtorArg(A01, MOVE_01));
//        } break;
//        case 2: {
//          mX.emplace(forwardCtorArg(A01, MOVE_01),
//                     forwardCtorArg(A02, MOVE_02));
//        } break;
//        default: {
//          assert(0);
//        } break;
//      }
//..
// We verify the correct arguments were forwarded to the
// 'AllocEmplcableTestType':
//..
//      // Verify that, depending on the corresponding template parameters,
//      // arguments were copied or moved.
//      assert(MOVE_ARG_01 == (bsltf::MoveState::e_MOVED == A01.movedFrom()));
//      assert(MOVE_ARG_02 == (bsltf::MoveState::e_MOVED == A02.movedFrom()));
//
//      // Verify that the element was constructed correctly.
//      const bsltf::AllocEmplacableTestType& V = X.front();
//
//      assert(18 == V.arg01() || N_ARGS < 1);
//      assert(33 == V.arg02() || N_ARGS < 2);
//  }
//..
// Finally, we call our templatized test case helper with a variety of template
// arguments:
//..
//  void testCase()
//  {
//      // Testing 'emplace' with 0 arguments.
//      testCaseHelper<0, false, false>();
//
//      // Testing 'emplace' with 1 argument.
//      testCaseHelper<1, false, false>();
//      testCaseHelper<1, true,  false>();
//
//      // Testing 'emplace' with 2 arguments.
//      testCaseHelper<2, false, false>();
//      testCaseHelper<2, true,  false>();
//      testCaseHelper<2, false, true >();
//      testCaseHelper<2, true,  true >();
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLTF_ALLOCARGUMENTTYPE
#include <bsltf_allocargumenttype.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace bsltf {

                         // =============================
                         // class AllocEmplacableTestType
                         // =============================

class AllocEmplacableTestType {
    // This class provides a test object used to check that the arguments
    // passed for creating an object with an in-place representation are of the
    // correct types and values.
  public:
    // TYPEDEFS
    typedef bsltf::AllocArgumentType< 1> ArgType01;
    typedef bsltf::AllocArgumentType< 2> ArgType02;
    typedef bsltf::AllocArgumentType< 3> ArgType03;
    typedef bsltf::AllocArgumentType< 4> ArgType04;
    typedef bsltf::AllocArgumentType< 5> ArgType05;
    typedef bsltf::AllocArgumentType< 6> ArgType06;
    typedef bsltf::AllocArgumentType< 7> ArgType07;
    typedef bsltf::AllocArgumentType< 8> ArgType08;
    typedef bsltf::AllocArgumentType< 9> ArgType09;
    typedef bsltf::AllocArgumentType<10> ArgType10;
    typedef bsltf::AllocArgumentType<11> ArgType11;
    typedef bsltf::AllocArgumentType<12> ArgType12;
    typedef bsltf::AllocArgumentType<13> ArgType13;
    typedef bsltf::AllocArgumentType<14> ArgType14;

  private:
    // DATA
    ArgType01 d_arg01;
    ArgType02 d_arg02;
    ArgType03 d_arg03;
    ArgType04 d_arg04;
    ArgType05 d_arg05;
    ArgType06 d_arg06;
    ArgType07 d_arg07;
    ArgType08 d_arg08;
    ArgType09 d_arg09;
    ArgType10 d_arg10;
    ArgType11 d_arg11;
    ArgType12 d_arg12;
    ArgType13 d_arg13;
    ArgType14 d_arg14;

    // CLASS DATA
    static int s_numDeletes;
        // Track number of times the destructor is called.

  public:
    // CLASS METHODS
    static int getNumDeletes();
        // Return the number of times an object of this type has been
        // destroyed.

    // CREATORS
    AllocEmplacableTestType(bslma::Allocator *basicAllocator = 0);
    explicit AllocEmplacableTestType(ArgType01         arg01,
                                     bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            ArgType06         arg06,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            ArgType06         arg06,
                            ArgType07         arg07,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            ArgType06         arg06,
                            ArgType07         arg07,
                            ArgType08         arg08,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            ArgType06         arg06,
                            ArgType07         arg07,
                            ArgType08         arg08,
                            ArgType09         arg09,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            ArgType06         arg06,
                            ArgType07         arg07,
                            ArgType08         arg08,
                            ArgType09         arg09,
                            ArgType10         arg10,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            ArgType06         arg06,
                            ArgType07         arg07,
                            ArgType08         arg08,
                            ArgType09         arg09,
                            ArgType10         arg10,
                            ArgType11         arg11,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            ArgType06         arg06,
                            ArgType07         arg07,
                            ArgType08         arg08,
                            ArgType09         arg09,
                            ArgType10         arg10,
                            ArgType11         arg11,
                            ArgType12         arg12,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            ArgType06         arg06,
                            ArgType07         arg07,
                            ArgType08         arg08,
                            ArgType09         arg09,
                            ArgType10         arg10,
                            ArgType11         arg11,
                            ArgType12         arg12,
                            ArgType13         arg13,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            ArgType06         arg06,
                            ArgType07         arg07,
                            ArgType08         arg08,
                            ArgType09         arg09,
                            ArgType10         arg10,
                            ArgType11         arg11,
                            ArgType12         arg12,
                            ArgType13         arg13,
                            ArgType14         arg14,
                            bslma::Allocator *basicAllocator = 0);
        // Create an 'AllocEmplacableTestType' by initializing corresponding
        // attributes with the specified 'arg01'..'arg14', and initializing any
        // remaining attributes with their default value (-1).  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    AllocEmplacableTestType(
                           const AllocEmplacableTestType&  original,
                           bslma::Allocator               *basicAllocator = 0);
        // Create an allocating, in-place test object having the same value as
        // the specified 'original'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~AllocEmplacableTestType();
        // Increment the count of calls to this destructor, and destroy this
        // object.

    // MANIPULATORS
    //! AllocEmplacableTestType& operator=(
    //                           const AllocEmplacableTestType& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the allocator used to supply memory for this object.

    const ArgType01& arg01() const;
    const ArgType02& arg02() const;
    const ArgType03& arg03() const;
    const ArgType04& arg04() const;
    const ArgType05& arg05() const;
    const ArgType06& arg06() const;
    const ArgType07& arg07() const;
    const ArgType08& arg08() const;
    const ArgType09& arg09() const;
    const ArgType10& arg10() const;
    const ArgType11& arg11() const;
    const ArgType12& arg12() const;
    const ArgType13& arg13() const;
    const ArgType14& arg14() const;
        // Return the value of the correspondingly numbered argument that was
        // passed to the constructor of this object.

    bool isEqual(const AllocEmplacableTestType& rhs) const;
        // Return 'true' if the specified 'rhs' has the same value as this
        // object, and 'false' otherwise.  Two 'AllocEmplacableTestType'
        // objects have the same value if each of their corresponding
        // attributes have the same value.
};

// FREE OPERATORS
bool operator==(const AllocEmplacableTestType& lhs,
                const AllocEmplacableTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'AllocEmplacableTestType' objects
    // have the same value if each of their corresponding attributes have the
    // same value.

bool operator!=(const AllocEmplacableTestType& lhs,
                const AllocEmplacableTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'AllocEmplacableTestType'
    // objects do not have the same value if any of their corresponding
    // attributes do not have the same value.

// ============================================================================
//                      INLINE DEFINITIONS
// ============================================================================

                        // -----------------------------
                        // class AllocEmplacableTestType
                        // -----------------------------

// ACCESSORS
inline
const AllocEmplacableTestType::ArgType01&
AllocEmplacableTestType::arg01() const
{
    return d_arg01;
}

inline
const AllocEmplacableTestType::ArgType02&
AllocEmplacableTestType::arg02() const
{
    return d_arg02;
}

inline
const AllocEmplacableTestType::ArgType03&
AllocEmplacableTestType::arg03() const
{
    return d_arg03;
}

inline
const AllocEmplacableTestType::ArgType04&
AllocEmplacableTestType::arg04() const
{
    return d_arg04;
}

inline
const AllocEmplacableTestType::ArgType05&
AllocEmplacableTestType::arg05() const
{
    return d_arg05;
}

inline
const AllocEmplacableTestType::ArgType06&
AllocEmplacableTestType::arg06() const
{
    return d_arg06;
}

inline
const AllocEmplacableTestType::ArgType07&
AllocEmplacableTestType::arg07() const
{
    return d_arg07;
}

inline
const AllocEmplacableTestType::ArgType08&
AllocEmplacableTestType::arg08() const
{
    return d_arg08;
}

inline
const AllocEmplacableTestType::ArgType09&
AllocEmplacableTestType::arg09() const
{
    return d_arg09;
}

inline
const AllocEmplacableTestType::ArgType10&
AllocEmplacableTestType::arg10() const
{
    return d_arg10;
}

inline
const AllocEmplacableTestType::ArgType11&
AllocEmplacableTestType::arg11() const
{
    return d_arg11;
}

inline
const AllocEmplacableTestType::ArgType12&
AllocEmplacableTestType::arg12() const
{
    return d_arg12;
}

inline
const AllocEmplacableTestType::ArgType13&
AllocEmplacableTestType::arg13() const
{
    return d_arg13;
}

inline
const AllocEmplacableTestType::ArgType14&
AllocEmplacableTestType::arg14() const
{
    return d_arg14;
}

inline
bslma::Allocator *AllocEmplacableTestType::allocator() const
{
    return d_arg01.allocator();
}

inline
bool AllocEmplacableTestType::isEqual(const AllocEmplacableTestType& rhs) const
{
    return d_arg01 == rhs.d_arg01
        && d_arg02 == rhs.d_arg02
        && d_arg03 == rhs.d_arg03
        && d_arg04 == rhs.d_arg04
        && d_arg05 == rhs.d_arg05
        && d_arg06 == rhs.d_arg06
        && d_arg07 == rhs.d_arg07
        && d_arg08 == rhs.d_arg08
        && d_arg09 == rhs.d_arg09
        && d_arg10 == rhs.d_arg10
        && d_arg11 == rhs.d_arg11
        && d_arg12 == rhs.d_arg12
        && d_arg13 == rhs.d_arg13
        && d_arg14 == rhs.d_arg14;
}

// FREE OPERATORS
inline
bool operator==(const bsltf::AllocEmplacableTestType& lhs,
                const bsltf::AllocEmplacableTestType& rhs)
{
    return lhs.isEqual(rhs);
}

inline
bool operator!=(const bsltf::AllocEmplacableTestType& lhs,
                const bsltf::AllocEmplacableTestType& rhs)
{
    return !lhs.isEqual(rhs);
}

}  // close package namespace

// TRAITS
namespace bslma {
template <>
struct UsesBslmaAllocator<bsltf::AllocEmplacableTestType> : bsl::true_type
{
};

}  // close namespace bslma

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
