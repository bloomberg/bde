/*
 * Copyright (c) 1999
 * Silicon Graphics Computer Systems, Inc.
 *
 * Copyright (c) 1999 
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted 
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */ 
//#include <bslstp_stlport_prefix.h>
#include <bslstp_sstream.h>

namespace bsl {

// Force instantiation of stringstream classes.
template class basic_stringbuf<char, ::std::char_traits<char>, allocator<char> >;
template class basic_ostringstream<char, ::std::char_traits<char>, allocator<char> >;
template class basic_istringstream<char, ::std::char_traits<char>, allocator<char> >;
template class basic_stringstream<char, ::std::char_traits<char>, allocator<char> >;

template class basic_stringbuf<wchar_t, ::std::char_traits<wchar_t>, allocator<wchar_t> >;
template class basic_ostringstream<wchar_t, ::std::char_traits<wchar_t>, allocator<wchar_t> >;
template class basic_istringstream<wchar_t, ::std::char_traits<wchar_t>, allocator<wchar_t> >;
template class basic_stringstream<wchar_t, ::std::char_traits<wchar_t>, allocator<wchar_t> >;

}  // close namespace bsl

// Local Variables:
// mode:C++
// End:
