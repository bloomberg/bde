// bslstl_string.cpp                                                  -*-C++-*-
#include <bslstl_string.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

// IMPLEMENTATION NOTES:
// This string class implements a "short string optimization" which optimizes
// the handling of short strings (strings shorter than a certain length) by
// putting them into an internal short string buffer.  The short string buffer
// is a part of the 'string' object and it doesn't require any memory
// allocations.  This avoids memory allocations/deallocations on operations on
// short strings such as: construction, copy-construction, copy-assignment,
// destruction.  Those operations are much cheaper now than they used to be
// when they required memory allocations.
//
// There are also some side-effects of the short string optimization.  The
// footprint of the 'string' object is larger now than it used to be due to the
// short string buffer.  And the default-constructed object may now have a
// non-zero capacity.  But the default constructor of 'string' still doesn't
// require any memory and cannot fail.

#ifndef INCLUDED_BSLSTL_STRING_CPP
#define INCLUDED_BSLSTL_STRING_CPP

#ifdef BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
template class bsl::String_Imp<char, bsl::string::size_type>;
template class bsl::String_Imp<wchar_t, bsl::wstring::size_type>;
template class bsl::basic_string<char>;
template class bsl::basic_string<wchar_t>;
#endif


std::size_t bsl::hashBasicString(const string& str)
{
    return hashBasicString<char, char_traits<char>, allocator<char> >(str);
}

std::size_t bsl::hashBasicString(const wstring& str)
{
    return hashBasicString<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >(
                                                                          str);
}

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
