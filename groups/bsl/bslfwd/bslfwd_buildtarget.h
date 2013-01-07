// bslfwd_buildtarget.h                                               -*-C++-*-
#ifndef INCLUDED_BSLFWD_BUILDTARGET
#define INCLUDED_BSLFWD_BUILDTARGET

//@PURPOSE: Supply a compile-time indicator of whether BSL uses C++ namespaces.
//
//@CLASSES:
//
//@MACROS:
// BSLFWD_BUILDTARGET_BSL_USING_NAMESPACES: set if two-level namespace in 'bsl'
//
//@AUTHOR: Alexei Zakharov (azakharov7)
//
//@DESCRIPTION: This header potentially provides a single macro,
// 'BSLFWD_BUILDTARGET_BSL_USING_NAMESPACES'.  If
// 'BSLFWD_BUILDTARGET_BSL_USING_NAMESPACES' is defined then the types in the
// 'bsl' library are declared in C++ style namespaces (e.g.,
// 'bslma::Allocator').  If 'BSLFWD_BUILDTARGET_BSL_USING_NAMESPACES' is not
// defined then the types in the 'bsl' library are declared using legacy-style
// namespaces (e.g., 'bslma_Allocator').

// Uncomment the following macro when switching to 'bsl' with namespaces to
// enable forward declarations provided by the 'bslfwd' package to be in the
// appropriate namespace.

#define BSLFWD_BUILDTARGET_BSL_USING_NAMESPACES

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
