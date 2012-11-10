// bsltf_nonassignabletesttype.h                                      -*-C++-*-
#ifndef INCLUDED_BSLTF_NONASSIGNABLETESTTYPE
#define INCLUDED_BSLTF_NONASSIGNABLETESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class to which can not be assigned.
//
//@CLASSES:
//   bsltf::NonAssignableTestType: non-assignable test type
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, 'NonAssignableTestType', that does not not
// support assignment.  This is particularly valuable when testing container
// operations that works with non-assignable types.
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
///Example 1: Demonstrating The Type Can't Be Assigned To
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to show 'NonAssignableTestType' can't be assigned to:
//
// First, we create two 'NonAssignableTestType' objects, 'X' and 'Y':
//..
//  NonAssignableTestType X(1);
//  NonAssignableTestType Y(2);
//..
// Now, we show that assigning 'X' from Y will not compile:
//..
//  X = Y; // This will not compile
//..

namespace BloombergLP {
namespace bsltf {

                        // ===========================
                        // class NonAssignableTestType
                        // ===========================

class NonAssignableTestType {
    // This unconstrained (value-semantic) attribute class does not
    // provide an assignment operator.

    // DATA
    int d_data;  // integer class value

  private:
    // NOT IMPLEMENTED
    NonAssignableTestType& operator=(const NonAssignableTestType& rhs);

  public:

    // CREATORS
    NonAssignableTestType();
        // Create a 'NonAssignableTestType' object having the (default)
        // attribute values:
        //..
        //  data() == 0
        //..

    explicit NonAssignableTestType(int data);
        // Create a 'NonAssignableTestType' object having the specified
        // 'data' attribute value.

    // NonAssignableTestType(const NonAssignableTestType& original) = default;
        // Create a 'SimpleTestType' object having the same value as the
        // specified 'original' object.

    // ~NonAssignableTestType() = default;
        // Destroy this object.

    // MANIPULATORS
    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.
};

// FREE OPERATORS
bool operator==(const NonAssignableTestType& lhs,
                const NonAssignableTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'NonAssignableTestType'
    // objects have the same if their 'data' attributes are the same.

bool operator!=(const NonAssignableTestType& lhs,
                const NonAssignableTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'NonAssignableTestType'
    // objects do not have the same value if their 'data' attributes are not
    // the same.

// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ===========================================================================

                        // ----------------------------------
                        // class NonAssignableTestType
                        // ----------------------------------

// CREATORS
inline
NonAssignableTestType::NonAssignableTestType()
: d_data(0)
{
}

inline
NonAssignableTestType::NonAssignableTestType(int data)
: d_data(data)
{
}

// MANIPULATORS
inline
void NonAssignableTestType::setData(int value)
{
    d_data = value;
}

// ACCESSORS
inline
int NonAssignableTestType::data() const
{
    return d_data;
}

// FREE OPERATORS
inline
bool operator==(const bsltf::NonAssignableTestType& lhs,
                const bsltf::NonAssignableTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool operator!=(const bsltf::NonAssignableTestType& lhs,
                const bsltf::NonAssignableTestType& rhs)
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
