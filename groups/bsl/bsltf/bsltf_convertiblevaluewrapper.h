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
//@AUTHOR: Alisdair Meredith (ameredith1)
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
