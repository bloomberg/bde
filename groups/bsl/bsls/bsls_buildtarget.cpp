// bsls_buildtarget.cpp                                               -*-C++-*-
#include <bsls_buildtarget.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {

extern const int BSLS_BUILDTARGET_IS_UNIQUE = 1;

#ifdef BDE_BUILD_TARGET_EXC

const int bsls::YesExcBuildTarget::d_isExcBuildTarget = 1;

#else

const int bsls::NoExcBuildTarget::d_isExcBuildTarget = 0;

#endif

#ifdef BDE_BUILD_TARGET_MT

const int bsls::YesMtBuildTarget::d_isMtBuildTarget = 1;

#else

const int bsls::NoMtBuildTarget::d_isMtBuildTarget = 0;

#endif

#ifdef BSLS_PLATFORM_CPU_64_BIT

const int bsls::Yes64BitBuildTarget::d_is64BitBuildTarget = 1;

#else

const int bsls::No64BitBuildTarget::d_is64BitBuildTarget = 0;

#endif

}  // close enterprise namespace

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
