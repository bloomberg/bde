// bslstp_iosfwd.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLSTP_IOSFWD
#define INCLUDED_BSLSTP_IOSFWD

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#if !defined(BSL_PUBLISHED) || 1 == BSL_PUBLISHED

namespace std {

template <typename CHAR_TYPE> struct char_traits;

}  // close namespace std

namespace bsl {

template <class TYPE> class allocator;

template <class _CharT, class _Traits = std::char_traits<_CharT>,
          class _Alloc = bsl::allocator<_CharT> >
class basic_stringbuf;

template <class _CharT, class _Traits = std::char_traits<_CharT>,
          class _Alloc = bsl::allocator<_CharT> >
class basic_istringstream;

template <class _CharT, class _Traits = std::char_traits<_CharT>,
          class _Alloc = bsl::allocator<_CharT> >
class basic_ostringstream;

template <class _CharT, class _Traits = std::char_traits<_CharT>,
          class _Alloc = bsl::allocator<_CharT> >
class basic_stringstream;

typedef basic_stringbuf    <char, std::char_traits<char>,
                                               allocator<char> >     stringbuf;
typedef basic_istringstream<char, std::char_traits<char>,
                                               allocator<char> > istringstream;
typedef basic_ostringstream<char, std::char_traits<char>,
                                               allocator<char> > ostringstream;
typedef basic_stringstream <char, std::char_traits<char>,
                                               allocator<char> >  stringstream;

typedef basic_stringbuf    <wchar_t, std::char_traits<wchar_t>,
                                           allocator<wchar_t> >     wstringbuf;
typedef basic_istringstream<wchar_t, std::char_traits<wchar_t>,
                                           allocator<wchar_t> > wistringstream;
typedef basic_ostringstream<wchar_t, std::char_traits<wchar_t>,
                                           allocator<wchar_t> > wostringstream;
typedef basic_stringstream <wchar_t, std::char_traits<wchar_t>,
                                           allocator<wchar_t> >  wstringstream;

}  // close namespace bsl

#endif // #if !defined(BSL_PUBLISHED) || 1 == BSL_PUBLISHED

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
