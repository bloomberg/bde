// bsls_alignmenttotype.cpp                                           -*-C++-*-
#include <bsls_alignmenttotype.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

///IMPLEMENTATION NOTES
///--------------------
// The core of the implementation logic to compute the most primitive type
// corresponding to a given 'ALIGNMENT' value is provided by the
// 'bsls_alignmentimp' component.  Ideally, we would like to create a simple
// template class that is specialized for each alignment value and that
// provides the corresponding fundamental type.  But since multiple types can
// have the same alignment, we need a mechanism to disambiguate between them.
// So we assign a priority value to each type matching a specified alignment.
// This priority value is computed by the 'bsls::AlignmentImpMatch::match'
// function, which is specialized on a given type's alignment and size.  Once
// the priority is calculated we use the 'bsls::AlignmentImpPriorityToType'
// meta-function to define the most primitive type for a specified 'ALIGNMENT'
// value.

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
