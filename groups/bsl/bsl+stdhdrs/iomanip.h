// iomanip.h                                                          -*-C++-*-

//@PURPOSE: Provide backwards compatibility for STLPort header
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: This component provides backwards compatibility for Bloomberg
// libraries that depend on non-standard STLPort header files.  This header
// includes the corresponding C++ header and provides any additional
// symbols that the STLPort header would define via transitive includes.

#include <bsl_iomanip.h>

using std::setiosflags;
using std::resetiosflags;
using std::setbase;
using std::setfill;
using std::setprecision;
using std::setw;

// _lib.std.ios.manip_, manipulators:
using std::boolalpha;
using std::noboolalpha;
using std::showbase;
using std::noshowbase;
using std::showpoint;
using std::noshowpoint;
using std::showpos;
using std::noshowpos;
using std::skipws;
using std::noskipws;
using std::uppercase;
using std::nouppercase;

// _lib.adjustfield.manip_ adjustfield:
using std::internal;
using std::left;
using std::right;

// _lib.basefield.manip_ basefield:
using std::dec;
using std::hex;
using std::oct;

// _lib.floatfield.manip_ floatfield:
using std::fixed;
using std::scientific;

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
