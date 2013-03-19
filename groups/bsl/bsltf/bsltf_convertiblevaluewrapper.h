// bsltf_convertiblevaluewrapper.h                                    -*-C++-*-
#ifndef INCLUDED_BSLTF_CONVERTIBLEVALUEWRAPPER
#define INCLUDED_BSLTF_CONVERTIBLEVALUEWRAPPER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an enumerated test type.
//
//@CLASSES:
//   bsltf::ConvertibleValueWrapper: wrapper of a value semantic class
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides ... (TBD)
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: (TBD)
/// - - - - - - - - - - - - - - - -
// First, we ...

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
  private:
    // DATA
    TYPE d_value;

  public:
    // CREATORS
    ConvertibleValueWrapper(const TYPE& value);                     // IMPLICIT

    // MANIPULATORS
    operator       TYPE&();

    // ACCESSORS
    operator const TYPE&() const;
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
// Copyright (C) 2013 Bloomberg L.P.
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
