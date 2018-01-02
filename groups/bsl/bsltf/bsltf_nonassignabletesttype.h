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
///Example 1: Demonstrating The Type Cannot Be Assigned To
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

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
    NonAssignableTestType& operator=(const NonAssignableTestType&);

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

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

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
