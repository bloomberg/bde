// bslalg_scalarprimitives.cpp                                        -*-C++-*-
#include <bslalg_scalarprimitives.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// IMPLEMENTATION NOTES: The 'bslmf::IsPair' trait is slated to
// disappear, as a pair is like any other type w.r.t. the constructors and
// 'bslma::Allocator'.  Nevertheless, it is used for different purposes in
// libraries that depend on 'bslalg'.  For instance, 'bdepu' uses it for
// printing (a pair does not have a printing method, but it is possible to use
// the trait to forward the call to 'print' to its two members if they both
// have the printable trait).  More importantly, it is used in 'mgustd_pair' to
// forward allocators if needed (this relies on an older version of 'std::pair'
// which did not support allocators).  'mgustd_Pair' should be redesigned, but
// for now we have to support it or else risk breaking the Robo build.  For
// these reasons, we have retained the trait and support it in the
// implementations of 'bslalg::ScalarPrimitives::construct' and
// 'copyConstruct'.
#endif // BDE_OMIT_INTERNAL_DEPRECATED

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
