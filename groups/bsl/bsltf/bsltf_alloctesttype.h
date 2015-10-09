// bsltf_alloctesttype.h                                              -*-C++-*-
#ifndef INCLUDED_BSLTF_ALLOCTESTTYPE
#define INCLUDED_BSLTF_ALLOCTESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class for testing that allocates with 'bslma::Allocator'.
//
//@CLASSES:
//   bsltf::AllocTestType: allocating test class
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, 'AllocTestType', that uses a
// 'bslma::Allocator' to allocate memory and defines the type trait
// 'bslma::UsesBslmaAllocator'.  Furthermore, this class is not
// bitwise-moveable, and will assert on destruction if it has been moved.  This
// class is primarily provided to facilitate testing of templates by defining a
// simple type representative of user-defined types having an allocator.
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
// 'AllocTestType' as the parameterized 'TYPE':
//..
//  printTypeTraits<AllocTestType>();
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

namespace BloombergLP
{

namespace bslma { class Allocator; }

namespace bsltf
{

                        // ===================
                        // class AllocTestType
                        // ===================

class AllocTestType {
    // This unconstrained (value-semantic) attribute class that uses a
    // 'bslma::Allocator' to allocate memory and defines the type trait
    // 'bslma::UsesBslmaAllocator'.  This class is primarily provided
    // to facilitate testing of templates by defining a simple type
    // representative of user-defined types having an allocator.  See the
    // Attributes section under @DESCRIPTION in the component-level
    // documentation for information on the class attributes.

    // DATA
    int             *d_data_p;       // pointer to the data value

    bslma::Allocator *d_allocator_p;  // allocator used to supply memory (held,
                                     // not owned)

    AllocTestType   *d_self_p;       // pointer to self (to verify this object
                                     // is not bit-wise moved)

  public:
    // CREATORS
    explicit AllocTestType(bslma::Allocator *basicAllocator = 0);
        // Create a 'AllocTestType' object having the (default) attribute
        // values:
        //..
        //  data() == 0
        //..
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    explicit AllocTestType(int data, bslma::Allocator *basicAllocator = 0);
        // Create a 'AllocTestType' object having the specified 'data'
        // attribute value.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    AllocTestType(const AllocTestType&  original,
                  bslma::Allocator      *basicAllocator = 0);
        // Create a 'AllocTestType' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~AllocTestType();
        // Destroy this object.

    // MANIPULATORS
    AllocTestType& operator=(const AllocTestType& rhs);
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
bool operator==(const AllocTestType& lhs, const AllocTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'AllocTestType' objects have the same
    // if their 'data' attributes are the same.

bool operator!=(const AllocTestType& lhs, const AllocTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'AllocTestType' objects do not
    // have the same value if their 'data' attributes are not the same.

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // -------------------
                        // class AllocTestType
                        // -------------------

// MANIPULATORS
inline
void AllocTestType::setData(int value)
{
    *d_data_p = value;
}

// ACCESSORS
inline
int AllocTestType::data() const
{
    return *d_data_p;
}

                                  // Aspects

inline
bslma::Allocator *AllocTestType::allocator() const
{
    return d_allocator_p;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bsltf::operator==(const AllocTestType& lhs, const AllocTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool bsltf::operator!=(const AllocTestType& lhs, const AllocTestType& rhs)
{
    return lhs.data() != rhs.data();
}

// TRAITS
namespace bslma {
template <>
struct UsesBslmaAllocator<bsltf::AllocTestType>
    : bsl::true_type {};
}  // close namespace bslma

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
