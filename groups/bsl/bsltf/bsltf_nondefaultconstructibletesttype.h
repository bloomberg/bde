// bsltf_nondefaultconstructibletesttype.h                            -*-C++-*-
#ifndef INCLUDED_BSLTF_NONDEFAULTCONSTRUCTIBLETESTTYPE
#define INCLUDED_BSLTF_NONDEFAULTCONSTRUCTIBLETESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class that is not default constructible.
//
//@CLASSES:
//   bsltf::NonDefaultConstructibleTestType: non-default-constructible class
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, 'NonDefaultConstructibleTestType', that is
// does not provide a default constructor.  This is particularly valuable when
// testing container operations that works with non-default-constructible
// types.
//
///Attributes
///----------
//..
//  Name                Type         Default
//  ------------------  -----------  -------
//  data                int          N/A
//..
//: o 'data': representation of the class value
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Demonstrating The Type Can't Be Default Constructed
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We show that create a 'NonDefaultConstructibleTestType' by using the default
// constructor will not compile:
//..
//  NonDefaultConstructibleTestType X; // This will not compile
//..

namespace BloombergLP {
namespace bsltf {

                        // =====================================
                        // class NonDefaultConstructibleTestType
                        // =====================================

class NonDefaultConstructibleTestType {
    // This unconstrained (value-semantic) attribute class does not provide a
    // default constructor.

    // DATA
    int d_data;  // integer class value

  public:

    // CREATORS
    explicit NonDefaultConstructibleTestType(int data);
        // Create a 'NonDefaultConstructibleTestType' object having the
        // specified 'data' attribute value.

    // NonDefaultConstructibleTestType(
    //              const NonDefaultConstructibleTestType& original) = default;
        // Create a 'NonDefaultConstructibleTestType' object having the same
        // value as the specified 'original' object.

    // ~NonDefaultConstructibleTestType() = default;
        // Destroy this object.

    // MANIPULATORS
    // NonDefaultConstructibleTestType& operator=(
    //                   const NonDefaultConstructibleTestType& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.
};

// FREE OPERATORS
bool operator==(const NonDefaultConstructibleTestType& lhs,
                const NonDefaultConstructibleTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'NonDefaultConstructibleTestType'
    // objects have the same if their 'data' attributes are the same.

bool operator!=(const NonDefaultConstructibleTestType& lhs,
                const NonDefaultConstructibleTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two
    // 'NonDefaultConstructibleTestType' objects do not have the same value if
    // 'data' attributes are not the same.

// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ===========================================================================

                        // -------------------------------------
                        // class NonDefaultConstructibleTestType
                        // -------------------------------------

// CREATORS
inline
NonDefaultConstructibleTestType::NonDefaultConstructibleTestType(int data)
: d_data(data)
{
}

// MANIPULATORS
inline
void NonDefaultConstructibleTestType::setData(int value)
{
    d_data = value;
}

// ACCESSORS
inline
int NonDefaultConstructibleTestType::data() const
{
    return d_data;
}

// FREE OPERATORS
inline
bool operator==(const bsltf::NonDefaultConstructibleTestType& lhs,
                const bsltf::NonDefaultConstructibleTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool operator!=(const bsltf::NonDefaultConstructibleTestType& lhs,
                const bsltf::NonDefaultConstructibleTestType& rhs)
{
    return lhs.data() != rhs.data();
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
