// bsltf_convertiblevaluewrapper.h                                    -*-C++-*-
#ifndef INCLUDED_BSLTF_CONVERTIBLEVALUEWRAPPER
#define INCLUDED_BSLTF_CONVERTIBLEVALUEWRAPPER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a wrapper class, convertible to a supplied value.
//
//@CLASSES:
//   bsltf::ConvertibleValueWrapper: wrapper of a value-semantic class
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a 'struct' that holds an object of a
// template parameter type, and provides implicit conversions to, and from,
// that type.  A 'bsltf::ConvertibleValueWrapper' facilitates testing of
// function templates whose contract requires a type that is "convertible to
// the specified type".  It also ensures that this uses up the one user-defined
// conversion in permitted in the conversion sequence, rather than accidentally
// relying on a built-in conversion such as type promotion.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: TBD
/// - - - - - - -

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

namespace bsltf {

                       // =============================
                       // class ConvertibleValueWrapper
                       // =============================

template <class TYPE>
struct ConvertibleValueWrapper {
    // This class provides a wrapper around an object of the specified
    // (template parameter) 'TYPE'.  'TYPE' shall be CopyConstructible.  If
    // 'TYPE' is a value-semantic type, then this class will also be value
    // semantic.  Objects of this type are implicitly convertible to and from
    // objects of the specified 'TYPE'.

  private:
    // DATA
    TYPE d_value;

  public:
    // CREATORS
    ConvertibleValueWrapper(const TYPE& value);                     // IMPLICIT
        // Create an object wrapping the specified 'value'.

    // MANIPULATORS
    operator       TYPE&();
        // Return a reference to the (modifiable) wrapped value.

    // ACCESSORS
    operator const TYPE&() const;
        // Return a reference to the (non-modifiable) wrapped value.
};


// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================


                       // -----------------------------
                       // class ConvertibleValueWrapper
                       // -----------------------------

// CREATORS
template <class TYPE>
inline
ConvertibleValueWrapper<TYPE>::ConvertibleValueWrapper(const TYPE& value)
: d_value(value)
{
}

template <class TYPE>
inline
ConvertibleValueWrapper<TYPE>::operator TYPE&()
{
    return d_value;
}

// ACCESSORS
template <class TYPE>
inline
ConvertibleValueWrapper<TYPE>::operator const TYPE&() const
{
    return d_value;
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
