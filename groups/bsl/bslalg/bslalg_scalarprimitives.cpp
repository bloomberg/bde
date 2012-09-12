// bslalg_scalarprimitives.cpp                                        -*-C++-*-
#include <bslalg_scalarprimitives.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// IMPLEMENTATION NOTES: The 'bslalg::TypeTraitPair' trait is slated to
// disappear, as a pair is like any other type w.r.t. the constructors and
// 'bslma::Allocator'.  Nevertheless, it is used for different purposes in
// libraries that depend on 'bslalg'.  For instance, 'bdepu' uses it for
// printing (a pair does not have a printing method, but it is possible to use
// the trait to forward the call to 'print' to its two members if they both
// have the printable trait).  More importantly, it is used in 'mgustd_pair' to
// forward allocators if needed (this relies on an older version of 'std::pair'
// which did not support allocators).  'mgustd_Pair' should be redesigned, but
// for now we have to support it or else risk breaking the Robo build.  For
// these reasons, we have retained the trait and support it in the
// implementations of 'bslalg::ScalarPrimitives::construct' and
// 'copyConstruct'.
#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
