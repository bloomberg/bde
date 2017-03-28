// bsltf_nonoptionalalloctesttype.h                                   -*-C++-*-
#ifndef INCLUDED_BSLTF_NONOPTIONALALLOCTESTTYPE
#define INCLUDED_BSLTF_NONOPTIONALALLOCTESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class for testing that allocates with 'bslma::Allocator'.
//
//@CLASSES:
//   bsltf::NonOptionalAllocTestType: test class with non-optional allocator.
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, 'NonOptionalAllocTestType', that uses a
// 'bslma::Allocator' to allocate memory and defines the type trait
// 'bslma::UsesBslmaAllocator'.  Furthermore, this class is not
// bitwise-moveable, and will assert on destruction if it has been moved.  This
// class does not meet the 'default-insertable' requirement on template
// parameter type for containers and is primarily provided to facilitate
// testing of extended quality of implementation of BDE containers.
//
///Attributes
///----------
//..
//  Name                Type         Default
//  ------------------  -----------  -------
//  data                int          0
//..
//: o 'data': representation of the object's value
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
//              "Type does not define bslma::UsesBslmaAllocator.\n");
//      }
//
//      if (bslmf::IsBitwiseMoveable<TYPE>::value) {
//          printf("Type defines bslmf::IsBitwiseMoveable.\n");
//      }
//      else {
//          printf("Type does not define bslmf::IsBitwiseMoveable.\n");
//      }
//  }
//..
// Now, we invoke the 'printTypeTraits' function template using
// 'NonOptionalAllocTestType' as the parameterized 'TYPE':
//..
//  printTypeTraits<NonOptionalAllocTestType>();
//..
// Finally, we observe the console output:
//..
//  Type defines bslma::UsesBslmaAllocator.
//  Type does not define bslmf::IsBitwiseMoveable.
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

namespace BloombergLP {

namespace bslma {
        class Allocator;
}

namespace bsltf {

                        // ==============================
                        // class NonOptionalAllocTestType
                        // ==============================

class NonOptionalAllocTestType {
    // This unconstrained (value-semantic) attribute class that uses a
    // 'bslma::Allocator' to allocate memory and defines the type trait
    // 'bslma::UsesBslmaAllocator'.  This class does not provide a default
    // constructor.

    // DATA
    int                      *d_data_p;      // pointer to the data value

    bslma::Allocator         *d_allocator_p; // allocator used to supply memory
                                             // (held, not owned)

    NonOptionalAllocTestType *d_self_p;      // pointer to self (to verify this
                                             // object is not bit-wise moved)

  public:
    // CREATORS
    explicit NonOptionalAllocTestType(bslma::Allocator *basicAllocator);
        // Create a 'NonOptionalAllocTestType' object having the (default)
        // attribute values using the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    explicit NonOptionalAllocTestType(int               data,
                                      bslma::Allocator *basicAllocator);
        // Create a 'NonOptionalAllocTestType' object having the specified
        // 'data' attribute value using the specified a 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    NonOptionalAllocTestType(
                          const NonOptionalAllocTestType&  original,
                          bslma::Allocator                *basicAllocator = 0);
        // Create a 'NonOptionalAllocTestType' object having the same value as
        // the specified 'original' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~NonOptionalAllocTestType();
        // Destroy this object.

    // MANIPULATORS
    NonOptionalAllocTestType& operator=(const NonOptionalAllocTestType& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

// FREE OPERATORS
bool operator==(const NonOptionalAllocTestType& lhs,
                const NonOptionalAllocTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'NonOptionalAllocTestType' objects
    // have the same if their 'data' attributes are the same.

bool operator!=(const NonOptionalAllocTestType& lhs,
                const NonOptionalAllocTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'NonOptionalAllocTestType'
    // objects do not have the same value if their 'data' attributes are not
    // the same.

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // ------------------------------
                        // class NonOptionalAllocTestType
                        // ------------------------------

// MANIPULATORS
inline
void NonOptionalAllocTestType::setData(int value)
{
    *d_data_p = value;
}

// ACCESSORS
inline
int NonOptionalAllocTestType::data() const
{
    return *d_data_p;
}

                                  // Aspects

inline
bslma::Allocator *NonOptionalAllocTestType::allocator() const
{
    return d_allocator_p;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bsltf::operator==(const NonOptionalAllocTestType& lhs,
                       const NonOptionalAllocTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool bsltf::operator!=(const NonOptionalAllocTestType& lhs,
                       const NonOptionalAllocTestType& rhs)
{
    return lhs.data() != rhs.data();
}

// TRAITS
namespace bslma {
template <>
struct UsesBslmaAllocator<bsltf::NonOptionalAllocTestType>
    : bsl::true_type {};
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
