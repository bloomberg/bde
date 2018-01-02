// bsltf_nonequalcomparabletesttype.h                                 -*-C++-*-
#ifndef INCLUDED_BSLTF_NONEQUALCOMPARABLETESTTYPE
#define INCLUDED_BSLTF_NONEQUALCOMPARABLETESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class that is not equal comparable.
//
//@CLASSES:
//   bsltf::NonEqualComparableTestType: non-equal-comparable class
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, 'NonEqualComparableTestType', that does
// not provide equality-comparison operators.  This is particularly valuable
// when test containers that should work with non-equal-comparable types.
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
///Example 1: Demonstrating the Type Cannot be Equality Compared
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to show 'NonEqualComparableTestType' can't be equal
// compared.
//
// First, we create two 'NonEqualComparableTestType' objects, 'X' and 'Y':
//..
//  NonEqualComparableTestType X(1);
//  NonEqualComparableTestType Y(2);
//..
// Now, we show that equal comparing 'X' and 'Y' will not compile:
//..
//  assert(X != Y); // This will not compile
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {
namespace bsltf {

                        // ================================
                        // class NonEqualComparableTestType
                        // ================================

class NonEqualComparableTestType {
    // This unconstrained (value-semantic) attribute class does not
    // provide equality-comparison operators.

    // DATA
    int d_data;  // integer class value

  public:

    // CREATORS
    NonEqualComparableTestType();
        // Create a 'NonEqualComparableTestType' object having the (default)
        // attribute values:
        //..
        //  data() == 0
        //..

    explicit NonEqualComparableTestType(int data);
        // Create a 'NonEqualComparableTestType' object having the specified
        // 'data' attribute value.

    // NonEqualComparableTestType(
    //                   const NonEqualComparableTestType& original) = default;
        // Create a 'NonEqualComparableTestType' object having the same value
        // as the specified 'original' object.

    // ~NonEqualComparableTestType() = default;
        // Destroy this object.

    // MANIPULATORS
    // NonEqualComparableTestType& operator=(
    //                        const NonEqualComparableTestType& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.
};

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // --------------------------------
                        // class NonEqualComparableTestType
                        // --------------------------------

// CREATORS
inline
NonEqualComparableTestType::NonEqualComparableTestType()
: d_data(0)
{
}

inline
NonEqualComparableTestType::NonEqualComparableTestType(int data)
: d_data(data)
{
}

// MANIPULATORS
inline
void NonEqualComparableTestType::setData(int value)
{
    d_data = value;
}

// ACCESSORS
inline
int NonEqualComparableTestType::data() const
{
    return d_data;
}

}  // close package namespace
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
