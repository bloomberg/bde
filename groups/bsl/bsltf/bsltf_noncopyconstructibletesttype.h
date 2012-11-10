// bsltf_noncopyconstructibletesttype.h                               -*-C++-*-
#ifndef INCLUDED_BSLTF_NONCOPYCONSTRUCTIBLETESTTYPE
#define INCLUDED_BSLTF_NONCOPYCONSTRUCTIBLETESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class that is not copy constructable.
//
//@CLASSES:
//   bsltf::NonCopyConstructibleTestType: non-copy-constructible test type
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, 'NonCopyConstructibleTestType', that does
// not provide a copy constructor.  This is particularly valuable when testing
// container operations that works with non-copy-constructible types.
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
///Example 1: Demonstrating The Type Can't Be Copy Constructed
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to show 'NonCopyConstructibleTestType' can't be copy
// constructed:
//
// First, we create a 'NonCopyConstructibleTestType' object, 'X':
//..
//  NonCopyConstructibleTestType X;
//..
// Now, we show that copy constructing another object from 'X' will not
// compile:
//..
//  NonCopyConstructibleTestType Y(X); // This will not compile
//..

namespace BloombergLP {
namespace bsltf {

                        // ==================================
                        // class NonCopyConstructibleTestType
                        // ==================================

class NonCopyConstructibleTestType {
    // This unconstrained (value-semantic) attribute class does not
    // provide a copy constructor.

    // DATA
    int d_data;  // integer class value

  private:
    // NOT IMPLEMENTED
    NonCopyConstructibleTestType(const NonCopyConstructibleTestType&);

  public:

    // CREATORS
    NonCopyConstructibleTestType();
        // Create a 'NonCopyConstructibleTestType' object having the (default)
        // attribute values:
        //..
        //  data() == 0
        //..

    explicit NonCopyConstructibleTestType(int data);
        // Create a 'NonCopyConstructibleTestType' object having the specified
        // 'data' attribute value.

    // ~NonCopyConstructibleTestType() = default;
        // Destroy this object.

    // MANIPULATORS
    // NonCopyConstructibleTestType& operator=(
    //                      const NonCopyConstructibleTestType& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.
};

// FREE OPERATORS
bool operator==(const NonCopyConstructibleTestType& lhs,
                const NonCopyConstructibleTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'NonCopyConstructibleTestType'
    // objects have the same if their 'data' attributes are the same.

bool operator!=(const NonCopyConstructibleTestType& lhs,
                const NonCopyConstructibleTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'NonCopyConstructibleTestType'
    // objects do not have the same value if their 'data' attributes are not
    // the same.

// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ===========================================================================

                        // ----------------------------------
                        // class NonCopyConstructibleTestType
                        // ----------------------------------

// CREATORS
inline
NonCopyConstructibleTestType::NonCopyConstructibleTestType()
: d_data(0)
{
}

inline
NonCopyConstructibleTestType::NonCopyConstructibleTestType(int data)
: d_data(data)
{
}

// MANIPULATORS
inline
void NonCopyConstructibleTestType::setData(int value)
{
    d_data = value;
}

// ACCESSORS
inline
int NonCopyConstructibleTestType::data() const
{
    return d_data;
}

// FREE OPERATORS
inline
bool operator==(const bsltf::NonCopyConstructibleTestType& lhs,
                const bsltf::NonCopyConstructibleTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool operator!=(const bsltf::NonCopyConstructibleTestType& lhs,
                const bsltf::NonCopyConstructibleTestType& rhs)
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
