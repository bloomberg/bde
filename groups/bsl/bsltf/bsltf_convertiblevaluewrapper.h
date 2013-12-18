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
//   bsltf::ConvertibleValueWrapper: wrapper of a value semantic class
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
///Example 1: (TBD)
/// - - - - - - - - - - - - - - - -

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


// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ===========================================================================


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
// Copyright (C) 2013 Bloomberg Finance L.P.
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
