// bsls_platform.cpp                                                  -*-C++-*-
#include <bsls_platform.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

                        // --------------------
                        // class bsls::Platform
                        // --------------------

// This class has no implementation; it is merely a place-holder for this
// component's logical name space.

// The compiler flags set up by the 'bsls_platform' header file will be sanity
// checked against those those passed in by the build tool.

// Ensure compiler flags are set when they have to be.
#if defined(BSLS_PLATFORM_CMP_MSVC)
    #if defined(BDE_BUILD_TARGET_EXC) && !defined(_CPPUNWIND)
        #error "Exception build without exception handling enabled (/GX)."
    #endif
#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
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
