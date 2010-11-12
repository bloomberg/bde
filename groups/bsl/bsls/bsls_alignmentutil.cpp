// bsls_alignmentutil.cpp                                             -*-C++-*-
#include <bsls_alignmentutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_types.h>  // for testing only

namespace BloombergLP {

template <int INTEGER>
struct bsls_AlignmentUtil_Assert;
    // This parameterized 'struct' is declared but not defined except for the
    // single specialization below.  It is used with the 'sizeof' operator to
    // verify the assumption that 'BSLS_MAX_ALIGNMENT' is a positive, integral
    // power of 2.  Note that 'bslmf_assert' cannot be used in 'bsls'.

template <>
struct bsls_AlignmentUtil_Assert<1> {
    // Applying 'sizeof' to this specialization will allow compilation to
    // succeed (i.e., the associated compile-time assert will succeed).

    enum { VALUE = 1 };
};

// Assert, at compile time, that 'BSLS_MAX_ALIGNMENT' is a positive, integral
// power of 2.

enum {

    assertion1 = sizeof(bsls_AlignmentUtil_Assert<
                                 1 <= bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT>),

    assertion2 = sizeof(bsls_AlignmentUtil_Assert<
                           0 <= (bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT
                              & (bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT - 1))>)

};

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
