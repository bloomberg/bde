// dummy.cpp                                                          -*-C++-*-

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Workaround for build tool.
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: This is a workaround for the build tool since the tool requires
// at least one implementation file in a package.

// This symbol exists to avoid 'getarsym' errors when linking tests against the
// 'bsl+stdhdrs' package library on SunOS with gcc.
char stdhdrs_dummy_cpp_this_symbol_avoids_an_empty_package_library;

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
