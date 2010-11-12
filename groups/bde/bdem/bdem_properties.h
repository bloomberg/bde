// bdem_properties.h                                                  -*-C++-*-
#ifndef INCLUDED_BDEM_PROPERTIES
#define INCLUDED_BDEM_PROPERTIES

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide predefined attribute structures for 'bdem' types.
//
//@CLASSES:
//    bdem_Properties: namespace for 'bdem_Descriptor's
//
//@SEE_ALSO: bdem_descriptor, bdem_functiontemplates
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component creates and populates element attribute
// structures (i.e., 'bdem_Descriptor's), one for each scalar and array type
// enumerated in 'bdem_ElemType::Type' (but conspicuously omits 'CHOICE',
// 'CHOICE_ARRAY', 'LIST', and 'TABLE').  These prepopulated descriptors can be
// used by clients needing to indirectly invoke canonical operations for a
// given type (e.g., type-agnostic containers needing to copy-construct items).
//
// Note that because the structures created in this component are prepopulated
// with particular function addresses, the selected functions may not provide
// client-desired additional semantics for particular canonical operations.
// Clients needing specialized (additional) semantics for functions like copy
// construction, default construction, assignment, and so on should create
// and populate their own 'bdem_Descriptor' directly.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_DESCRIPTOR
#include <bdem_descriptor.h>
#endif

namespace BloombergLP {

                        // ======================
                        // struct bdem_Properties
                        // ======================

struct bdem_Properties {
    // This 'struct' provides a namespace for "global" element attribute
    // 'struct's for the 'bdem' types corresponding to enumerated scalar and
    // array types, as well as 'VOID'.

    // *** attribute structure for 'VOID' ***

    static const bdem_Descriptor d_voidAttr;

    // *** attribute structures for scalar types ***

    static const bdem_Descriptor d_boolAttr;
    static const bdem_Descriptor d_charAttr;
    static const bdem_Descriptor d_shortAttr;
    static const bdem_Descriptor d_intAttr;
    static const bdem_Descriptor d_int64Attr;
    static const bdem_Descriptor d_floatAttr;
    static const bdem_Descriptor d_doubleAttr;
    static const bdem_Descriptor d_stringAttr;
    static const bdem_Descriptor d_datetimeAttr;
    static const bdem_Descriptor d_datetimeTzAttr;
    static const bdem_Descriptor d_dateAttr;
    static const bdem_Descriptor d_dateTzAttr;
    static const bdem_Descriptor d_timeAttr;
    static const bdem_Descriptor d_timeTzAttr;

    // *** attribute structures for array types ***

    static const bdem_Descriptor d_boolArrayAttr;
    static const bdem_Descriptor d_charArrayAttr;
    static const bdem_Descriptor d_shortArrayAttr;
    static const bdem_Descriptor d_intArrayAttr;
    static const bdem_Descriptor d_int64ArrayAttr;
    static const bdem_Descriptor d_floatArrayAttr;
    static const bdem_Descriptor d_doubleArrayAttr;
    static const bdem_Descriptor d_stringArrayAttr;
    static const bdem_Descriptor d_datetimeArrayAttr;
    static const bdem_Descriptor d_datetimeTzArrayAttr;
    static const bdem_Descriptor d_dateArrayAttr;
    static const bdem_Descriptor d_dateTzArrayAttr;
    static const bdem_Descriptor d_timeArrayAttr;
    static const bdem_Descriptor d_timeTzArrayAttr;
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
