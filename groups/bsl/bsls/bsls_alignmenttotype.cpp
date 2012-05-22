// bsls_alignmenttotype.cpp                                           -*-C++-*-
#include <bsls_alignmenttotype.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

///IMPLEMENTATION NOTES
///--------------------
// The core of the implementation logic to compute the most primitive type
// corresponding to a given 'ALIGNMENT' value is provided by the
// 'bsls_alignmentimp' component.  Ideally, we would like to create a simple
// template class that is specialized for each alignment value and that
// provides the corresponding fundamental type.  But since multiple types can
// have the same alignment, we need a mechanism to disambiguate between them.
// So we assign a priority value to each type matching a specified alignment.
// This priority value is computed by the 'bsls::AlignmentImpMatch::match'
// function, which is specialized on a given type's alignment and size.  Once
// the priority is calculated we use the 'bsls::AlignmentImpPriorityToType'
// meta-function to define the most primitive type for a specified 'ALIGNMENT'
// value.

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
