// bdlf_memfn.cpp                                                     -*-C++-*-
#include <bdlf_memfn.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlf_memfn_cpp,"$Id$ $CSID$")

///IMPLEMENTATION NOTES
///--------------------
// bdlf::MemFn: The 'operator()' methods of the 'bdlf::MemFn' wrapper need to
// be implemented within the class body for SUNPRO CC (even for version 5.5,
// a.k.a. Studio 8).  This is because that compiler accepts out-of-class-body
// definitions for template members, but not for template operators.
//
// bdlf::MemFnInstance: There are two ways to handle instance wrappers around
// class types that take a 'bslma' allocator.  One is to store the wrapper in a
// constructor proxy.  The other is to simply disallow it (asserting that
// 'INSTANCE_WRAPPER' does not have the 'bslalg::TypeTraitUsesBslmaAllocator'
// trait).  We probably could do the latter because nothing we would usually
// store in a 'bdlf::MemFnInstance' would require it (in all known uses the
// instance wrapper is a raw pointer, or some kind of smart pointer, to the
// 'ObjectType').  For the sake of correctness and generality, we do the
// former.

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
