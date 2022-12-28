// bsltf_evilbooleantype.h                                            -*-C++-*-
#ifndef INCLUDED_BSLTF_EVILBOOLEANTYPE
#define INCLUDED_BSLTF_EVILBOOLEANTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide the most awkward type that is convertible to 'bool'.
//
//@CLASSES:
//  bsltf::EvilBooleanType: most awkward type that is convertible to 'bool'
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a type that is convertible-to-'bool',
// and so may be used to satisfy many C++ standard library requirements, but is
// perversely implemented to provide the most awkward interface that meets the
// requirements.  This type is not intended for use in production code, but is
// most useful when implementing test drivers for generic components that must
// accept predicates, or other expressions, that yield a type that is merely
// convertible to 'bool'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of using
// 'bsltf::EvilBooleanType'.
//
// First, we create an object 'trueValue' and initialize it with the 'true'
// value:
//..
//  bsltf::EvilBooleanType trueValue(true);
//..
// Now, we can use it for if-else conditions or another constructions, that
// require boolen value:
//..
//  if (trueValue) {
//      assert(trueValue);
//  }
//..
// Finally we create another object, having the opposite value, and verify it:
//..
//  bsltf::EvilBooleanType falseValue = !trueValue;
//  assert(false == (bool)falseValue);
//..

#include <bslscm_version.h>

#include <bsls_keyword.h>

namespace BloombergLP {

namespace bsltf {

                            // =====================
                            // class EvilBooleanType
                            // =====================

struct EvilBooleanType {
    // This class provides a test type for predicates returning a type that is
    // convertible-to-bool.  It makes life reasonably difficult by disabling
    // the address-of and comma operators, but deliberately does not overload
    // the '&&' and '||' operators, as we hope the standard will be updated to
    // no longer require such support.  Once C++11 becomes available, this
    // class would use an 'explicit operator bool()' conversion operator, and
    // explicitly supply the '==' and '!=' operators, but we use the
    // convertible-to-pointer-to-member idiom in the meantime.  Implicitly
    // defined operations fill out the API as needed.

  private:
    // PRIVATE TYPES
    struct ImpDetail
        // Struct, containing value to be pointed to.
    {
        int d_member;
    };


    typedef int ImpDetail::* BoolResult;
        // Typedef for pointer to the nested class field, that can be converted
        // to boolean.

    // DATA
    BoolResult d_value;  // pointer being converted to boolean value

  private:
    // NOT IMPLEMENTED
    void operator=(const EvilBooleanType&) BSLS_KEYWORD_DELETED;

    void operator&() const BSLS_KEYWORD_DELETED;

    template<class T>
    void operator,(T&) const BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    EvilBooleanType(bool value);                                    // IMPLICIT
        // Create a 'EvilBooleanType' object having the attribute value defined
        // by the specified 'value'.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    // To avoid warnings about future incompatibility due to the deleted copy
    // assignment operator we declare the copy constructor as implicitly
    // generated.  For consistency the destructor was also placed here and
    // declared to be explicitly generated.

    EvilBooleanType(const EvilBooleanType& original) = default;
        // Create a 'EvilBooleanType' object having the same value as the
        // specified 'original' object.

    ~EvilBooleanType() = default;
        // Destroy this object.
#endif

    // ACCESSORS
    operator BoolResult() const;
        // Return the value of this object.

    EvilBooleanType operator!() const;
        // Return the newly created object having the logically negated value
        // of this object.
};

EvilBooleanType operator==(const EvilBooleanType& lhs,
                           const EvilBooleanType& rhs);
    // Return an 'EvilBoolanType' value that converts to 'true' if the
    // specified 'lhs' and 'rhs' have the same value, and a value that converts
    // to 'false' otherwise. Two 'EvilBooleanType' objects have the same value
    // if the values resulting from converting each to 'bool' have the same
    // value.

EvilBooleanType operator!=(const EvilBooleanType& lhs,
                           const EvilBooleanType& rhs);
    // Return an 'EvilBoolanType' value that converts to 'true' if the
    // specified 'lhs' and 'rhs' do not have the same value, and a value that
    // converts to 'false' otherwise. Two 'EvilBooleanType' objects do not have
    // the same value if the values resulting from converting each to 'bool' do
    // not have the same value.

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                       // ---------------------
                       // class EvilBooleanType
                       // ---------------------

// CREATORS
inline
EvilBooleanType::EvilBooleanType(bool value)
: d_value(!value ? 0 : &ImpDetail::d_member)
{
}

// ACCESSORS
inline
EvilBooleanType::operator BoolResult() const
{
    return d_value;
}

inline
EvilBooleanType EvilBooleanType::operator!() const
{
    return !d_value;
}

}  // close package namespace

inline
bsltf::EvilBooleanType bsltf::operator==(const EvilBooleanType& lhs,
                                         const EvilBooleanType& rhs)
{
    return static_cast<bool>(lhs) == static_cast<bool>(rhs);
}

inline
bsltf::EvilBooleanType bsltf::operator!=(const EvilBooleanType& lhs,
                                         const EvilBooleanType& rhs)
{
    return static_cast<bool>(lhs) != static_cast<bool>(rhs);
}

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
