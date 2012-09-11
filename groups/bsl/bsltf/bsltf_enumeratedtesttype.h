// bsltf_enumeratedtesttype.h                                         -*-C++-*-
#ifndef INCLUDED_BSLTF_ENUMERATEDTESTTYPE
#define INCLUDED_BSLTF_ENUMERATEDTESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an enumerated test type.
//
//@CLASSES:
//   bsltf::EnumeratedTestType: enumerated test type
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@AUTHOR: Chen He (che2)
//
//@DESCRIPTION: This component provides an enumeration type,
//'EnumeratedTestType', to facilitate the testing of templates.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using The Enumeration
/// - - - - - - - - - - - - - - - -
// First, we create an 'EnumeratedTestType::Enum' value and initialize it to
// the first possible value:
//..
// EnumeratedTestType::Enum first = EnumeratedTestType::FIRST;
// assert(static_cast<int>(first) == 0);
//..
// Finally, we create an 'EnumeratedTestType::Enum' value and initilize it to
// the last possible value:
//..
// EnumeratedTestType::Enum last = EnumeratedTestType::LAST;
// assert(static_cast<int>(last) == 127);
//..

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

namespace BloombergLP {

namespace bsltf {

                        // =========================
                        // struct EnumeratedTestType
                        // =========================

struct EnumeratedTestType {
    // This 'struct' provides a namespace for defining an 'enum' type that
    // supports explict conversion to and from an integeral type for values
    // from 0 to 127 (according to 7.2.7 of the C++11 standard).

  public:
    // TYPES
    enum Enum {
        FIRST = 0,
        LAST = 127
    };
};

// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ===========================================================================


}  // close package namespace

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
