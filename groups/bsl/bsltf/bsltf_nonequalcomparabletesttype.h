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
// not provide equality comparison operators.  This is particularly valuable
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
///Example 1: Demonstrating The Type Can't Be Equal Compared
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

namespace BloombergLP {
namespace bsltf {

                        // ================================
                        // class NonEqualComparableTestType
                        // ================================

class NonEqualComparableTestType {
    // This unconstrained (value-semantic) attribute class does not
    // provide a equality comparison operators.

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

// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ===========================================================================

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
// Copyright (C) 2012 Bloomberg L.P.
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
