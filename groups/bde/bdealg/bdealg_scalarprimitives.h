// bdealg_scalarprimitives.h                  -*-C++-*-
#ifndef INCLUDED_BDEALG_SCALARPRIMITIVES
#define INCLUDED_BDEALG_SCALARPRIMITIVES

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Primitive algorithms that operate on single elements.
//
//@DEPRECATED: Use 'bslalg_scalarprimitives' instead.
//
//@CLASSES:
//  bdealg_ScalarPrimitives: namespace for algorithms
//
//@SEE_ALSO: bdealg_typetraits, bdealg_constructorproxy
//
//@AUTHOR: Pablo Halpern (phalpern), Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides primitive algorithms that operate on
// single elements with a uniform interface but select a different
// implementation according to the various 'bdealg' type traits possessed by
// the underlying type.  Such primitives are exceptionally useful for
// implementing generic components such as containers.  There are six families
// of algorithms, each with a collection of overloads:
//..
//      Algorithm           Forwards to (depending on traits)
//      ----------------    ------------------------------------------------
//      defaultConstruct    Default constructor, with or without allocator
//
//      copyConstruct       Copy constructor, with or without allocator,
//                            or bitwise copy if appropriate
//
//      destructiveMove     Copy construction followed by destruction of the
//                            original, with or without allocator,
//                            or bitwise copy if appropriate
//
//      construct           In-place construction (using variadic template
//                            arguments, simulated by overloads), with
//                            or without allocator
//
//      destruct            Destructor, or no-op if can abandon memory
//
//      swap                Three way assignment, or bitwise swap
//..
// The traits under consideration by this component are:
//..
//  Trait                                         Description
//  --------------------------------------------  -----------------------------
//  bdealg_TypeTraitHasTrivialDefaultConstructor  "TYPE has the trivial default
//                                                constructor trait", or
//                                                "TYPE has a trivial default
//                                                constructor"
//
//  bdealg_TypeTraitUsesBdemaAllocator            "the 'TYPE' constructor takes
//                                                an allocator argument", or
//                                                "'TYPE' supports 'bdema'
//                                                allocators"
//
//  bdealg_TypeTraitBitwiseCopyable               "TYPE has the bit-wise
//                                                copyable trait", or
//                                                "TYPE is bit-wise copyable"
//                                                (implies that it has a
//                                                trivial destructor too)
//
//  bdealg_TypeTraitBitwiseMoveable               "TYPE has the bit-wise
//                                                moveable trait", or
//                                                "TYPE is bit-wise moveable"
//
//  bdealg_TypeTraitPair                          "TYPE has the pair trait"
//                                                 or "TYPE is a pair"
//..
///Usage
///-----
// This component is for use primarily by the 'bdestl' package.  Other clients
// should use the STL algorithms provided in '<algorithm>' and '<memory>'.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

namespace BloombergLP {

                       // ==============================
                       // struct bdealg_ScalarPrimitives
                       // ==============================

typedef bslalg_ScalarPrimitives bdealg_ScalarPrimitives;
    // This 'struct' provides a namespace for a suite of utility functions that
    // operate on elements of a parameterized type 'TARGET_TYPE'.  If any of
    // the '...Construct' methods throws, then its target 'address' is left
    // uninitialized and there are no effects, unless otherwise mentioned in
    // the documentation.

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
