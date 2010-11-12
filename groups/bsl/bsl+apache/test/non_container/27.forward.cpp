/***************************************************************************
 *
 * 27.forward.cpp - exercises lib.iostream.forward
 *
 * $Id: 27.forward.cpp 451936 2006-10-02 09:51:24Z faridz $
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 * Copyright 1994-2005 Rogue Wave Software.
 * 
 **************************************************************************/

#include <iosfwd>

// IMPORTANT: do not #include any other header before the declarations
//            below in order not to accidentally bring in any missing
//            definitions from the #included header

#ifndef _RWSTD_NO_NAMESPACE
#  define std std
#else
#  define std
#endif   // _RWSTD_NO_NAMESPACE

typedef char C;

typedef std::char_traits<C>                            C_traits;
typedef std::allocator<C>                              C_alloc;

typedef std::basic_ios<C, C_traits>                    C_basic_ios;
typedef std::basic_streambuf<C, C_traits>              C_basic_streambuf;
typedef std::basic_istream<C, C_traits>                C_basic_istream;
typedef std::basic_ostream<C, C_traits>                C_basic_ostream;
typedef std::basic_iostream<C, C_traits>               C_basic_iostream;
typedef std::basic_stringbuf<C, C_traits, C_alloc>     C_basic_stringbuf;
typedef std::basic_istringstream<C, C_traits, C_alloc> C_basic_istringstream;
typedef std::basic_ostringstream<C, C_traits, C_alloc> C_basic_ostringstream;
typedef std::basic_stringstream<C, C_traits, C_alloc>  C_basic_stringstream;

typedef std::basic_filebuf<C, C_traits>                C_basic_filebuf;
typedef std::basic_ifstream<C, C_traits>               C_basic_ifstream;
typedef std::basic_ofstream<C, C_traits>               C_basic_ofstream;
typedef std::basic_fstream<C, C_traits>                C_basic_fstream;

typedef std::istreambuf_iterator<C, C_traits>          C_istreambuf_iterator;
typedef std::ostreambuf_iterator<C, C_traits>          C_ostreambuf_iterator;

#ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES

// make sure that default template arguments are specified

typedef std::basic_ios<C>           C_X_basic_ios;
typedef std::basic_streambuf<C>     C_X_basic_streambuf;
typedef std::basic_istream<C>       C_X_basic_istream;
typedef std::basic_ostream<C>       C_X_basic_ostream;
typedef std::basic_iostream<C>      C_X_basic_iostream;
typedef std::basic_stringbuf<C>     C_X_basic_stringbuf;
typedef std::basic_istringstream<C> C_X_basic_istringstream;
typedef std::basic_ostringstream<C> C_X_basic_ostringstream;
typedef std::basic_stringstream<C>  C_X_basic_stringstream;

typedef std::basic_filebuf<C>       C_X_basic_filebuf;
typedef std::basic_ifstream<C>      C_X_basic_ifstream;
typedef std::basic_ofstream<C>      C_X_basic_ofstream;
typedef std::basic_fstream<C>       C_X_basic_fstream;

typedef std::istreambuf_iterator<C> C_X_istreambuf_iterator;
typedef std::ostreambuf_iterator<C> C_X_ostreambuf_iterator;

#endif   // _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES

typedef std::fpos<C>       C_fpos;

// test whether all required typedefs are defined
typedef std::ios           C_ios;
typedef std::streambuf     C_streambuf;
typedef std::istream       C_istream;
typedef std::ostream       C_ostream;
typedef std::iostream      C_iostream;
typedef std::stringbuf     C_stringbuf;
typedef std::istringstream C_istringstream;
typedef std::ostringstream C_ostringstream;
typedef std::stringstream  C_stringstream;
typedef std::filebuf       C_filebuf;
typedef std::ifstream      C_ifstream;
typedef std::ofstream      C_ofstream;
typedef std::fstream       C_fstream;

typedef std::streampos     C_streampos;


#ifndef _RWSTD_NO_WCHAR_T

typedef std::wios           W_wios;
typedef std::wstreambuf     W_wstreambuf;
typedef std::wistream       W_wistream;
typedef std::wostream       W_wostream;
typedef std::wiostream      W_wiostream;
typedef std::wstringbuf     W_wstringbuf;
typedef std::wistringstream W_wistringstream;
typedef std::wostringstream W_wostringstream;
typedef std::wstringstream  W_wstringstream;
typedef std::wfilebuf       W_wfilebuf;
typedef std::wifstream      W_wifstream;
typedef std::wofstream      W_wofstream;
typedef std::wfstream       W_wfstream;

typedef std::wstreampos     W_wstreampos;

#endif   // _RWSTD_NO_WCHAR_T

/**************************************************************************/

#include <driver.h>

/**************************************************************************/

#ifndef _RWSTD_NO_PART_SPEC_OVERLOAD

template <class T, class U>
bool is_same_type (const T*, const U*) { return false; }

template <class T>
bool is_same_type (const T*, const T*) { return true; }

#else   // ifdef _RWSTD_NO_PART_SPEC_OVERLOAD

template <class T, class U>
struct is_same
{
    struct yes {};
    struct no { yes no_ [2]; };
    template <class T>
    struct Type {};

    static yes test (Type<T>, Type<T>);
    static no test (...);

    enum { value = sizeof (test (Type<T> (), Type<U> ())) == sizeof (yes) };
};

template <class T, class U>
bool is_same_type (const T*, const U*) { return is_same<T, U>::value; }

#endif  // _RWSTD_NO_PART_SPEC_OVERLOAD

/**************************************************************************/

static int
run_test (int, char*[])
{
#define TEST_TYPE(T)                                                    \
    rw_assert (is_same_type ((C_X_ ## T*)0, (C_ ## T*)0), 0, __LINE__,  \
               "%s", #T);

    // check that default arguments are correct
    TEST_TYPE (basic_ios);
    TEST_TYPE (basic_streambuf);
    TEST_TYPE (basic_istream);
    TEST_TYPE (basic_ostream);
    TEST_TYPE (basic_iostream);
    TEST_TYPE (basic_stringbuf);
    TEST_TYPE (basic_istringstream);
    TEST_TYPE (basic_ostringstream);
    TEST_TYPE (basic_stringstream);

    TEST_TYPE (basic_filebuf);
    TEST_TYPE (basic_ifstream);
    TEST_TYPE (basic_ofstream);
    TEST_TYPE (basic_fstream);

    TEST_TYPE (istreambuf_iterator);
    TEST_TYPE (ostreambuf_iterator);


#define TEST_TYPEDEF(T, U)                                      \
    rw_assert (is_same_type ((T*)0, (U*)0), 0, __LINE__,        \
               "%s == %s", #T, #U);

    // check that typedefs are correctly defined
    TEST_TYPEDEF (std::ios,           C_ios);
    TEST_TYPEDEF (std::streambuf,     C_streambuf);
    TEST_TYPEDEF (std::istream,       C_istream);
    TEST_TYPEDEF (std::ostream,       C_ostream);
    TEST_TYPEDEF (std::iostream,      C_iostream);
    TEST_TYPEDEF (std::stringbuf,     C_stringbuf);
    TEST_TYPEDEF (std::istringstream, C_istringstream);
    TEST_TYPEDEF (std::ostringstream, C_ostringstream);
    TEST_TYPEDEF (std::stringstream,  C_stringstream);
    TEST_TYPEDEF (std::filebuf,       C_filebuf);
    TEST_TYPEDEF (std::ifstream,      C_ifstream);
    TEST_TYPEDEF (std::ofstream,      C_ofstream);
    TEST_TYPEDEF (std::fstream,       C_fstream);

#ifndef _RWSTD_NO_WCHAR_T

    TEST_TYPEDEF (std::wios,           W_wios);
    TEST_TYPEDEF (std::wstreambuf,     W_wstreambuf);
    TEST_TYPEDEF (std::wistream,       W_wistream);
    TEST_TYPEDEF (std::wostream,       W_wostream);
    TEST_TYPEDEF (std::wiostream,      W_wiostream);
    TEST_TYPEDEF (std::wstringbuf,     W_wstringbuf);
    TEST_TYPEDEF (std::wistringstream, W_wistringstream);
    TEST_TYPEDEF (std::wostringstream, W_wostringstream);
    TEST_TYPEDEF (std::wstringstream,  W_wstringstream);
    TEST_TYPEDEF (std::wfilebuf,       W_wfilebuf);
    TEST_TYPEDEF (std::wifstream,      W_wifstream);
    TEST_TYPEDEF (std::wofstream,      W_wofstream);
    TEST_TYPEDEF (std::wfstream,       W_wfstream);
    TEST_TYPEDEF (std::wstreampos,     std::streampos);

#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.iostream.forward",
                    0,   // no comment
                    run_test,
                    "",
                    0);
}
