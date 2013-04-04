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
