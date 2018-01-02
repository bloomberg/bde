// bsltf_bitwisemoveabletesttype.h                                    -*-C++-*-
#ifndef INCLUDED_BSLTF_BITWISEMOVEABLETESTTYPE
#define INCLUDED_BSLTF_BITWISEMOVEABLETESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a bitwise-moveable test class.
//
//@CLASSES:
//   bsltf::BitwiseMoveableTestType: bitwise-moveable test class
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, 'BitwiseMoveableTestType', that is
// bitwise-moveable and defines the 'bslmf::IsBitwiseMoveable' type
// trait.
//
///Attributes
///----------
//..
//  Name                Type         Default
//  ------------------  -----------  -------
//  data                int          0
//..
//: o 'data': representation of the class value
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Printing the Supported Traits
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to print the supported traits of this test type.
//
// First, we create a function template 'printTypeTraits' with a parameterized
// 'TYPE':
//..
//  template <class TYPE>
//  void printTypeTraits()
//      // Prints the traits of the parameterized 'TYPE' to the console.
//  {
//      if (bslma::UsesBslmaAllocator<TYPE>::value) {
//          printf("Type defines bslma::UsesBslmaAllocator.\n");
//      }
//      else {
//          printf(
//               "Type does not define bslma::UsesBslmaAllocator.\n");
//      }
//
//      if (blsmf::IsBitwiseMoveable<TYPE>::value) {
//          printf("Type defines bslmf::IsBitwiseMoveable.\n");
//      }
//      else {
//          printf("Type does not define bslmf::IsBitwiseMoveable.\n");
//      }
//  }
//..
// Now, we invoke the 'printTypeTraits' function template using
// 'BitwiseMoveableTestType' as the parameterized 'TYPE':
//..
//  printTypeTraits<BitwiseMoveableTestType>();
//..
// Finally, we observe the console output:
//..
//  Type does not define bslma::UsesBslmaAllocator.
//  Type defines bslmf::IsBitwiseMoveable.
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

namespace BloombergLP {
namespace bsltf {

                        // =============================
                        // class BitwiseMoveableTestType
                        // =============================

class BitwiseMoveableTestType {
    // This unconstrained (value-semantic) attribute class defines the
    // 'bslmf::IsBitwiseMoveable' and does not allocate memory.  See the
    // Attributes section under @DESCRIPTION in the component-level
    // documentation for information on the class attributes.

    // DATA
    int d_data;  // class value

  public:
    // CREATORS
    BitwiseMoveableTestType();
        // Create a 'SimpleTestType' object having the (default) attribute
        // values:
        //..
        //  data() == 0
        //..

    explicit BitwiseMoveableTestType(int data);
        // Create a 'BitwiseMoveableTestType' object having the specified
        // 'data' attribute value.

    // BitwiseMoveableTestType(
    //                      const BitwiseMoveableTestType& original) = default;
        // Create a 'BitwiseMoveableTestType' object having the same value
        // as the specified 'original' object.

    // ~BitwiseMoveableTestType() = default;
        // Destroy this object.

    // MANIPULATORS
    // BitwiseMoveableTestType& operator=(
    //                           const BitwiseMoveableTestType& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.
};

// FREE OPERATORS
bool operator==(const BitwiseMoveableTestType& lhs,
                const BitwiseMoveableTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'BitwiseMoveableTestType' objects
    // have the same if their 'data' attributes are the same.

bool operator!=(const BitwiseMoveableTestType& lhs,
                const BitwiseMoveableTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'BitwiseMoveableTestType'
    // objects do not have the same value if their 'data' attributes are not
    // the same.

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // -----------------------------
                        // class BitwiseMoveableTestType
                        // -----------------------------

// CREATORS
inline
BitwiseMoveableTestType::BitwiseMoveableTestType()
: d_data(0)
{
}

inline
BitwiseMoveableTestType::BitwiseMoveableTestType(int data)
: d_data(data)
{
}

// MANIPULATORS

inline
void BitwiseMoveableTestType::setData(int value)
{
    d_data = value;
}

// ACCESSORS
inline
int BitwiseMoveableTestType::data() const
{
    return d_data;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bsltf::operator==(const bsltf::BitwiseMoveableTestType& lhs,
                       const bsltf::BitwiseMoveableTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool bsltf::operator!=(const bsltf::BitwiseMoveableTestType& lhs,
                       const bsltf::BitwiseMoveableTestType& rhs)
{
    return lhs.data() != rhs.data();
}

// TRAITS
namespace bslmf {
template <>
struct IsBitwiseMoveable<bsltf::BitwiseMoveableTestType>
    : bsl::true_type {};
}  // close namespace bslmf

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
