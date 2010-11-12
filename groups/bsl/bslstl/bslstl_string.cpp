// bslstl_string.cpp                                                  -*-C++-*-
#include <bslstl_string.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

// IMPLEMENTATION NOTES: We opt out of possible optimizations such as storing
// the allocator pointer in the string buffer, or short-string optimization
// (storing the string buffer within the string object footprint).  The reason
// is that such changes would non-monotonically affect the performances of some
// functions (some functions would benefit in performance, and some would be
// ever so slightly penalized) and we do not, and cannot, assert which way it
// would affect our clients.

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
