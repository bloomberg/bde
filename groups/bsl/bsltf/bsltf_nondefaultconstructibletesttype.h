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
///Example 1: Demonstrating The Type Cannot Be Default Constructed
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We show that create a 'NonDefaultConstructibleTestType' by using the default
// constructor will not compile:
//..
//  NonDefaultConstructibleTestType X; // This will not compile
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

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

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

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
