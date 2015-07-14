// bdlmxxx_properties.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLMXXX_PROPERTIES
#define INCLUDED_BDLMXXX_PROPERTIES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide predefined attribute structures for 'bdem' types.
//
//@CLASSES:
//    bdlmxxx::Properties: namespace for 'bdlmxxx::Descriptor's
//
//@SEE_ALSO: bdlmxxx_descriptor, bdlmxxx_functiontemplates
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component creates and populates element attribute
// structures (i.e., 'bdlmxxx::Descriptor's), one for each scalar and array type
// enumerated in 'bdlmxxx::ElemType::Type' (but conspicuously omits 'CHOICE',
// 'CHOICE_ARRAY', 'LIST', and 'TABLE').  These prepopulated descriptors can be
// used by clients needing to indirectly invoke canonical operations for a
// given type (e.g., type-agnostic containers needing to copy-construct items).
//
// Note that because the structures created in this component are prepopulated
// with particular function addresses, the selected functions may not provide
// client-desired additional semantics for particular canonical operations.
// Clients needing specialized (additional) semantics for functions like copy
// construction, default construction, assignment, and so on should create
// and populate their own 'bdlmxxx::Descriptor' directly.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMXXX_DESCRIPTOR
#include <bdlmxxx_descriptor.h>
#endif

namespace BloombergLP {

namespace bdlmxxx {
                        // ======================
                        // struct Properties
                        // ======================

struct Properties {
    // This 'struct' provides a namespace for "global" element attribute
    // 'struct's for the 'bdem' types corresponding to enumerated scalar and
    // array types, as well as 'VOID'.

    // *** attribute structure for 'VOID' ***

    static const Descriptor s_voidAttr;

    // *** attribute structures for scalar types ***

    static const Descriptor s_boolAttr;
    static const Descriptor s_charAttr;
    static const Descriptor s_shortAttr;
    static const Descriptor s_intAttr;
    static const Descriptor s_int64Attr;
    static const Descriptor s_floatAttr;
    static const Descriptor s_doubleAttr;
    static const Descriptor s_stringAttr;
    static const Descriptor s_datetimeAttr;
    static const Descriptor s_datetimeTzAttr;
    static const Descriptor s_dateAttr;
    static const Descriptor s_dateTzAttr;
    static const Descriptor s_timeAttr;
    static const Descriptor s_timeTzAttr;

    // *** attribute structures for array types ***

    static const Descriptor s_boolArrayAttr;
    static const Descriptor s_charArrayAttr;
    static const Descriptor s_shortArrayAttr;
    static const Descriptor s_intArrayAttr;
    static const Descriptor s_int64ArrayAttr;
    static const Descriptor s_floatArrayAttr;
    static const Descriptor s_doubleArrayAttr;
    static const Descriptor s_stringArrayAttr;
    static const Descriptor s_datetimeArrayAttr;
    static const Descriptor s_datetimeTzArrayAttr;
    static const Descriptor s_dateArrayAttr;
    static const Descriptor s_dateTzArrayAttr;
    static const Descriptor s_timeArrayAttr;
    static const Descriptor s_timeTzArrayAttr;

    // The following names are provided for backward compatibility, they are
    // DEPRECATED as static variable names should begin with 's_' rather than
    // 'd_'.  They will be eliminated in a future release.

    // *** attribute structures for scalar types ***

    static const Descriptor& d_intAttr;
    static const Descriptor& d_doubleAttr;
    static const Descriptor& d_stringAttr;

    static const Descriptor& d_charArrayAttr;
};
}  // close package namespace

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
