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
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
