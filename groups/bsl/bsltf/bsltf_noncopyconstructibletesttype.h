// bsltf_noncopyconstructibletesttype.h                               -*-C++-*-
#ifndef INCLUDED_BSLTF_NONCOPYCONSTRUCTIBLETESTTYPE
#define INCLUDED_BSLTF_NONCOPYCONSTRUCTIBLETESTTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class that is not copy constructible.
//
//@CLASSES:
//   bsltf::NonCopyConstructibleTestType: non-copy-constructible test type
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, `NonCopyConstructibleTestType`, that does
// not provide a copy constructor.  This is particularly valuable when testing
// container operations that works with non-copy-constructible types.
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
///Example 1: Demonstrating The Type Cannot Be Copy Constructed
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to show `NonCopyConstructibleTestType` can't be copy
// constructed:
//
// First, we create a `NonCopyConstructibleTestType` object, `X`:
// ```
// NonCopyConstructibleTestType X;
// ```
// Now, we show that copy constructing another object from `X` will not
// compile:
// ```
// NonCopyConstructibleTestType Y(X); // This will not compile
// ```

#include <bslscm_version.h>

#include <bsls_keyword.h>

namespace BloombergLP {
namespace bsltf {

                        // ==================================
                        // class NonCopyConstructibleTestType
                        // ==================================

/// This unconstrained (value-semantic) attribute class does not
/// provide a copy constructor.
class NonCopyConstructibleTestType {

    // DATA
    int d_data;  // integer class value

  private:
    // NOT IMPLEMENTED
    NonCopyConstructibleTestType(const NonCopyConstructibleTestType&)
                                                          BSLS_KEYWORD_DELETED;

  public:

    // CREATORS

    /// Create a `NonCopyConstructibleTestType` object having the (default)
    /// attribute values:
    /// ```
    /// data() == 0
    /// ```
    NonCopyConstructibleTestType();

    /// Create a `NonCopyConstructibleTestType` object having the specified
    /// `data` attribute value.
    explicit NonCopyConstructibleTestType(int data);

    /// Destroy this object.
    ~NonCopyConstructibleTestType();

    // MANIPULATORS
    // NonCopyConstructibleTestType& operator=(
    //                      const NonCopyConstructibleTestType& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    /// Set the `data` attribute of this object to the specified `value`
    void setData(int value);

    // ACCESSORS

    /// Return the value of the `data` attribute of this object.
    int data() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  Two `NonCopyConstructibleTestType`
/// objects have the same if their `data` attributes are the same.
bool operator==(const NonCopyConstructibleTestType& lhs,
                const NonCopyConstructibleTestType& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the
/// same value, and `false` otherwise.  Two `NonCopyConstructibleTestType`
/// objects do not have the same value if their `data` attributes are not
/// the same.
bool operator!=(const NonCopyConstructibleTestType& lhs,
                const NonCopyConstructibleTestType& rhs);

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

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

inline
NonCopyConstructibleTestType::~NonCopyConstructibleTestType()
{
    d_data = ~d_data & 0xf0f0f0f0;
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
