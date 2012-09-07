// fstream.h                                                          -*-C++-*-

//@PURPOSE: Provide backwards compatibility for STLPort header
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: This component provides backwards compatibility for Bloomberg
// libraries that depend on non-standard STLPort header files.  This header
// includes the corresponding C++ header and provides any additional symbols
// that the STLPort header would define via transitive includes.

#ifndef INCLUDED_BSLS_CODEBASE
#include <bsls_codebase.h>
#endif

#include <bsl_fstream.h>

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
    // Permit reliance on transitive includes within robo.
#include <iostream.h>
#endif

using std::basic_filebuf;
using std::basic_ifstream;
using std::basic_ofstream;
using std::basic_fstream;

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
