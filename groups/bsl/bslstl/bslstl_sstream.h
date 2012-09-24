// bslstl_sstream.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLSTL_SSTREAM
#define INCLUDED_BSLSTL_SSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide the C++03 compatible stringstream classes.
//
//@SEE_ALSO: bslstl_stringbuf, bslstl_istringstream, bslstl_ostringstream,
//           bslstl_stringstream
//
//@AUTHOR: Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_sstream.h>' instead.
//
///Usage
///-----
// This component is for use by the 'bsl+stdhdrs' package.  Use
// 'std::stringstream' directly.

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_sstream.h> instead of <bslstl_sstream.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSTL_STREAMBUF
#include <bslstl_stringbuf.h>
#endif

#ifndef INCLUDED_BSLSTL_ISTREAMSTREAM
#include <bslstl_istringstream.h>
#endif

#ifndef INCLUDED_BSLSTL_OSTREAMSTREAM
#include <bslstl_ostringstream.h>
#endif

#ifndef INCLUDED_BSLSTL_STRINGSTREAM
#include <bslstl_stringstream.h>
#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
