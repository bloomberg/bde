// iostream.h                                                         -*-C++-*-

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

#include <bsl_iostream.h>

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
    // Permit reliance on transitive includes within robo.
#include <ios.h>
#include <ostream.h>

// Transitive using - <ios.h> includes <ios>, which includes <iosfwd>, which
// then contains definition of 'ostream', 'istream', 'ifstream' and 'ofstream'.
using std::ifstream;
using std::istream;
using std::ofstream;
using std::ostream;
#endif
#endif // BDE_OMIT_INTERNAL_DEPRECATED

using std::cin;
using std::cout;
using std::cerr;
using std::clog;
using std::wcin;
using std::wcout;
using std::wcerr;
using std::wclog;

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
