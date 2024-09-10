// bsltf_bitwisecopyabletesttype.h                                    -*-C++-*-
#ifndef INCLUDED_BSLTF_BITWISECOPYABLETESTTYPE
#define INCLUDED_BSLTF_BITWISECOPYABLETESTTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a bitwise-copyable test class.
//
//@CLASSES:
//   bsltf::BitwiseCopyableTestType: bitwise-copyable test class
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, `BitwiseCopyableTestType`, that is
// bitwise-copyable and defines the `bslmf::IsBitwiseCopyable` type trait.
//
///Attributes
///----------
// ```
// Name                Type         Default
// ------------------  -----------  -------
// data                int          0
// ```
// * `data`: representation of the class value
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Printing the Supported Traits
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to print the supported traits of this test type.
//
// First, we create a function template `printTypeTraits` with a parameterized
// `TYPE`:
// ```
// /// Prints the traits of the parameterized 'TYPE' to the console.
// template <class TYPE>
// void printTypeTraits()
// {
//     if (bslma::UsesBslmaAllocator<TYPE>::value) {
//         printf("Type defines bslma::UsesBslmaAllocator.\n");
//     }
//     else {
//         printf(
//              "Type does not define bslma::UsesBslmaAllocator.\n");
//     }
//
//     if (bls::is_trivially_copyable<TYPE>::value) {
//         printf("Type defines bsl::is_trivially_copyable.\n");
//     }
//     else {
//         printf("Type does not define bsl::is_trivially_copyable.\n");
//     }
// }
// ```
// Now, we invoke the `printTypeTraits` function template using
// `BitwiseCopyableTestType` as the parameterized `TYPE`:
// ```
// printTypeTraits<BitwiseCopyableTestType>();
// ```
// Finally, we observe the console output:
// ```
// Type does not define bslma::UsesBslmaAllocator.
// Type defines bsl::is_trivially_copyable.
// ```

#include <bslscm_version.h>

#include <bslma_usesbslmaallocator.h>

#include <bslmf_isbitwisecopyable.h>
#include <bslmf_nestedtraitdeclaration.h>

namespace BloombergLP {
namespace bsltf {

                        // =============================
                        // class BitwiseCopyableTestType
                        // =============================

/// This unconstrained (value-semantic) attribute class defines the
/// `bsl::is_trivially_copyable` trait and does not allocate memory.  See the
/// [](#Attributes) section for information on the class attributes.
class BitwiseCopyableTestType {

    // DATA
    int d_data;  // class value

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BitwiseCopyableTestType,
                                   bslmf::IsBitwiseCopyable);

    // CREATORS

    /// Create a `SimpleTestType` object having the (default) attribute values:
    /// ```
    /// data() == 0
    /// ```
    BitwiseCopyableTestType();

    /// Create a `BitwiseCopyableTestType` object having the specified `data`
    /// attribute value.
    explicit BitwiseCopyableTestType(int data);

    /// Create a 'BitwiseCopyableTestType' object having the same value as the
    /// specified 'original' object.
    //! BitwiseCopyableTestType(
    //!                     const BitwiseCopyableTestType& original) = default;

    /// Destroy this object.  Note that this destructor is neither declared nor
    /// defined because this type is trivially copyable.
    //! ~BitwiseCopyableTestType() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified 'rhs' object, and
    /// return a reference providing modifiable access to this object.
    //! BitwiseCopyableTestType& operator=(
    //!                          const BitwiseCopyableTestType& rhs) = default;

    /// Set the `data` attribute of this object to the specified `value`.
    void setData(int value);

    // ACCESSORS

    /// Return the value of the `data` attribute of this object.
    int data() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` objects have the same value,
/// and `false` otherwise.  Two `BitwiseCopyableTestType` objects have the same
/// if their `data` attributes are the same.
bool operator==(const BitwiseCopyableTestType& lhs,
                const BitwiseCopyableTestType& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the same
/// value, and `false` otherwise.  Two `BitwiseCopyableTestType` objects do not
/// have the same value if their `data` attributes are not the same.
bool operator!=(const BitwiseCopyableTestType& lhs,
                const BitwiseCopyableTestType& rhs);

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // -----------------------------
                        // class BitwiseCopyableTestType
                        // -----------------------------

// CREATORS
inline
BitwiseCopyableTestType::BitwiseCopyableTestType()
: d_data(0)
{
}

inline
BitwiseCopyableTestType::BitwiseCopyableTestType(int data)
: d_data(data)
{
}

// MANIPULATORS
inline
void BitwiseCopyableTestType::setData(int value)
{
    d_data = value;
}

// ACCESSORS
inline
int BitwiseCopyableTestType::data() const
{
    return d_data;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bsltf::operator==(const bsltf::BitwiseCopyableTestType& lhs,
                       const bsltf::BitwiseCopyableTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool bsltf::operator!=(const bsltf::BitwiseCopyableTestType& lhs,
                       const bsltf::BitwiseCopyableTestType& rhs)
{
    return lhs.data() != rhs.data();
}

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
