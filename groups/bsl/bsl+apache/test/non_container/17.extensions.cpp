/************************************************************************
 *
 * extensions.cpp - compilation only test exercising the ability
 *                  to compile library headers with extensions diabled
 *
 * $Id$
 *
 ************************************************************************
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
 * Copyright 1994-2008 Rogue Wave Software.
 * 
 **************************************************************************/

#undef _RWSTD_NO_EXTENSIONS
#define _RWSTD_NO_EXTENSIONS

#include <rw/_defs.h>

// working around a SunOS/SunPro bug (PR #26255)
#if defined (__SUNPRO_CC) && (defined (__SunOS_5_8) || defined (__SunOS_5_9))
#  include <time.h>
#endif   // SunPro && SunOS 5.{8,9}

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cwchar>
#include <cwctype>

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <iso646.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>

#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>

/***********************************************************************/

int count_called = 0;
int count_pred_called = 0;
int distance_called = 0;


#if !defined (__SUNPRO_CC) || __SUNPRO_CC > 0x550

template <class T, class U>
void count (T, T, const U&, ...) { ++count_called; }


template <class T, class U>
void count_if (T, T, U, ...) { ++count_pred_called; }


template <class T>
void distance (T, T, ...) { ++distance_called; }

#else

// working around a SunPro bug (PR #28678)

template <class T, class U>
void count (T, T, const U&, std::size_t&) { ++count_called; }


template <class T, class U>
void count_if (T, T, U, std::size_t&) { ++count_pred_called; }


template <class T>
void distance (T, T, std::size_t&) { ++distance_called; }

#endif   // SunPro


#if !defined (__GNUG__) || __GNUG__ > 2

int ellipsis_called /* = 0 */;

struct NumGet: std::num_get<char>
{
    typedef std::num_get<char> Base;

#  ifndef _RWSTD_NO_USING_ACCESS_ADJUSTMENT

    _USING (Base::get);

#  else   // if defined (_RWSTD_NO_USING_ACCESS_ADJUSTMENT)

    Base::get;

#  endif   // _RWSTD_NO_USING_ACCESS_ADJUSTMENT

    iter_type get (iter_type, iter_type,
                   std::ios_base&, std::ios_base::iostate&, ...) const {
        ++ellipsis_called;
        return iter_type ();
    }
};

#endif   // gcc > 2.x


template <class T>
struct convertible
{
    operator T () const {
        static union {
            void *dummy;
            char buffer [sizeof (T)];
        } u;

        return (T)*u.buffer;
    }
};


struct base { /* empty */ };
struct badbit_set: base { /* empty */ };
struct eofbit_set: base { /* empty */ };
struct failbit_set: base { /* empty */ };

struct Bin: base { /* empty */ } *bin;
struct Nolock: base { /* empty */ } *nolock;
struct NolockBuf: base { /* empty */ } *nolockbuf;


struct IosBase: std::ios_base
{
    static bool check (int, base*) { return true; }
    static bool check (int, ...) { return false; }

    // members are preferred over non-members during unqualified lookup
    static bool check_badbit () { return check (0, (badbit_set*)0); }
    static bool check_eofbit () { return check (0, (eofbit_set*)0); }
    static bool check_failbit () { return check (0, (failbit_set*)0); }

    static bool check_bin () { return check (0, bin); }

    static bool check_nolock () { return check (0, nolock); }
    static bool check_nolockbuf () { return check (0, nolockbuf); }
};


_RWSTD_NAMESPACE (std) {


// define bogus primary templates to verify
// that they are not defined by the library

#ifdef _RWSTD_NO_EXT_CHAR_TRAITS_PRIMARY

template <class charT>
struct char_traits { /* empty */ };

#endif   // _RWSTD_NO_EXT_CHAR_TRAITS_PRIMARY


#ifdef _RWSTD_NO_EXT_CODECVT_PRIMARY

template <class internT, class externT, class stateT>
struct codecvt { /* empty */ };

#endif   // _RWSTD_NO_EXT_CODECVT_PRIMARY


#ifdef _RWSTD_NO_EXT_CTYPE_PRIMARY

template <class charT>
struct ctype { /* empty */ };

#endif   // _RWSTD_NO_EXT_CTYPE_PRIMARY


#ifdef _RWSTD_NO_EXT_COLLATE_PRIMARY

template <class charT>
struct collate { /* empty */ };

#endif   // _RWSTD_NO_EXT_COLLATE_PRIMARY


#ifdef _RWSTD_NO_EXT_MONEYPUNCT_PRIMARY

template <class charT, bool Intl>
struct moneypunct { /* empty */ };

#endif   // _RWSTD_NO_EXT_MONEYPUNCT_PRIMARY


#ifdef _RWSTD_NO_EXT_NUMPUNCT_PRIMARY

template <class charT>
struct numpunct { /* empty */ };

#endif   // _RWSTD_NO_EXT_NUMPUNCT_PRIMARY


#ifdef _RWSTD_NO_EXT_TIME_GET_PRIMARY

template <class charT, class InputIterator>
struct time_get { /* empty */ };

#endif   // _RWSTD_NO_EXT_TIME_GET_PRIMARY


#ifdef _RWSTD_NO_EXT_TIME_PUT_PRIMARY

template <class charT, class OutputIterator>
struct time_put { /* empty */ };

#endif   // _RWSTD_NO_EXT_TIME_PUT_PRIMARY


#ifdef _RWSTD_NO_EXT_MESSAGES_PRIMARY

template <class charT>
struct messages { /* empty */ };

#endif   // _RWSTD_NO_EXT_MESSAGES_PRIMARY

}   // namespace std

/***********************************************************************/

#include <driver.h>

bool predicate (int /* unused */) { return true; }

static int
run_test (int /* unused */, char* /* unused */ [])
{
    // exercise _RWSTD_NO_EXT_VOID_COUNT
    rw_info (0, 0, __LINE__, "_RWSTD_NO_EXT_VOID_COUNT");

    std::reverse_iterator<int*> it (0);

    std::size_t n = 0;

#ifndef _RWSTD_NO_EXT_VOID_COUNT
    rw_assert (0, __FILE__, __LINE__,
               "_RWSTD_NO_EXT_VOID_COUNT not #defined");
#endif

    // call the 4-argument overload of count() and expect Koenig lookup
    // to find ::count(); the std::count() extension will be preferred
    // if it exists and is a viable candidate, in which case the test
    // below will fail
    count (it, it, 0, n);

    rw_assert (1 == count_called, __FILE__, __LINE__,
               "_RWSTD_NO_EXTENSIONS #defined and an extended "
               "definition of std::count() found");

    // call the 4-argument overload of count_if() and expect Koenig lookup
    // to find ::count(); the std::count() extension will be preferred
    // if it exists and is a viable candidate
    count_if (it, it, predicate, n);

    rw_assert (1 == count_pred_called, __FILE__, __LINE__,
               "_RWSTD_NO_EXTENSIONS #defined and an extended "
               "definition of std::count_if() found");


    // exercise _RWSTD_NO_EXT_VOID_DISTANCE
    rw_info (0, 0, __LINE__, "_RWSTD_NO_EXT_VOID_DISTANCE");

#ifndef _RWSTD_NO_EXT_VOID_DISTANCE
    rw_assert (0, __FILE__, __LINE__,
               "_RWSTD_NO_EXT_VOID_DISTANCE not #defined");
#endif

    // call the 3-argument overload of distance() and expect Koenig
    // lookup to find ::distance(); std::distance() will be preferred
    // if it exists and is a viable candidate
    distance (it, it, n);

    rw_assert (1 == distance_called, __FILE__, __LINE__,
               "_RWSTD_NO_EXTENSIONS #defined and an extended "
               "definition of std::distance() found");


    // exercise _RWSTD_NO_EXT_NUM_GET
    rw_info (0, 0, __LINE__, "_RWSTD_NO_EXT_NUM_GET");

#ifndef _RWSTD_NO_EXT_NUM_GET
    rw_assert (0, __FILE__, __LINE__,
               "_RWSTD_NO_EXT_NUM_GET not #defined");
#endif

#if !defined (__GNUG__) || __GNUG__ > 2

#  define STRING(x) #x
#  define TEST_GET(T)                                               \
    do {                                                            \
        NumGet::iter_type in;                                       \
        std::ios_base::iostate err = std::ios_base::failbit;        \
        ellipsis_called = 0;                                        \
        NumGet ().get (in, in, std::cin, err, convertible<T&>());   \
        rw_assert (1 == ellipsis_called, __FILE__, __LINE__,        \
                   "_RWSTD_NO_EXTENSIONS #defined and std::"        \
                   "num_get<char>::get(..., " STRING(T) ") found"); \
    } while (0)

#if DRQS
// elipsis functions are not called properly.  'get' failed to override.
    TEST_GET (short);
    TEST_GET (int);

#  ifdef _RWSTD_LONG_LONG

    TEST_GET (_RWSTD_LONG_LONG);
    TEST_GET (unsigned _RWSTD_LONG_LONG);

#  endif   // _RWSTD_NO_LONG_LONG

#endif

#endif   // gcc > 2.x


    // exercise _RWSTD_NO_EXT_FAILURE
    rw_info (0, 0, __LINE__, "_RWSTD_NO_EXT_FAILURE");

#ifndef _RWSTD_NO_EXT_FAILURE
    rw_assert (0, __FILE__, __LINE__, "_RWSTD_NO_EXT_FAILURE not #defined");
#endif

    rw_assert (IosBase::check_badbit (), __FILE__, __LINE__,
               "_RWSTD_NO_EXTENSIONS #defined and "
               " class std::ios_base::badbit_set found");

    rw_assert (IosBase::check_eofbit (), __FILE__, __LINE__,
               "_RWSTD_NO_EXTENSIONS #defined and "
               " class std::ios_base::eofbit_set found");

    rw_assert (IosBase::check_eofbit (), __FILE__, __LINE__,
               "_RWSTD_NO_EXTENSIONS #defined and "
               " class std::ios_base::failbit_set found");


    // exercise _RWSTD_NO_EXT_BIN_IO
    rw_info (0, 0, __LINE__, "_RWSTD_NO_EXT_BIN_IO");

#ifndef _RWSTD_NO_EXT_BIN_IO
    rw_assert (0, __FILE__, __LINE__,
               "_RWSTD_NO_EXT_BIN_IO not #defined");
#endif

    rw_assert (IosBase::check_bin (), __FILE__, __LINE__,
               "_RWSTD_NO_EXTENSIONS #defined and "
               " std::ios_base::bin found");


    // exercise _RWSTD_NO_EXT_REENTRANT_IO
    rw_info (0, 0, __LINE__, "_RWSTD_NO_EXT_REENTRANT_IO");

#ifndef _RWSTD_NO_EXT_REENTRANT_IO
    rw_assert (0, __FILE__, __LINE__,
               "_RWSTD_NO_EXT_REENTRANT_IO not #defined");
#endif

    rw_assert (IosBase::check_nolock (), __FILE__, __LINE__,
               "_RWSTD_NO_EXTENSIONS #defined and "
               " std::ios_base::nolock found");

    rw_assert (IosBase::check_nolockbuf (), __FILE__, __LINE__,
               "_RWSTD_NO_EXTENSIONS #defined and "
               " std::ios_base::nolockbuf found");


    // exercise _RWSTD_NO_EXT_CHAR_TRAITS_PRIMARY
    rw_info (0, 0, __LINE__, "_RWSTD_NO_EXT_CHAR_TRAITS_PRIMARY");

#ifndef _RWSTD_NO_EXT_CHAR_TRAITS_PRIMARY
    rw_assert (0, __FILE__, __LINE__,
               "_RWSTD_NO_EXT_CHAR_TRAITS_PRIMARY not #defined");
#endif   // _RWSTD_NO_EXT_CHAR_TRAITS_PRIMARY


    // exercise _RWSTD_NO_EXT_CODECVT_PRIMARY
    rw_info (0, 0, __LINE__, "_RWSTD_NO_EXT_CODECVT_PRIMARY");

#ifndef _RWSTD_NO_EXT_CODECVT_PRIMARY
    rw_assert (0, __FILE__, __LINE__,
               "_RWSTD_NO_EXT_CODECVT_PRIMARY not #defined");
#endif   // _RWSTD_NO_EXT_CODECVT_PRIMARY


    // exercise _RWSTD_NO_EXT_COLLATE_PRIMARY
    rw_info (0, 0, __LINE__, "_RWSTD_NO_EXT_COLLATE_PRIMARY");

#ifndef _RWSTD_NO_EXT_COLLATE_PRIMARY
    rw_assert (0, __FILE__, __LINE__,
               "_RWSTD_NO_EXT_COLLATE_PRIMARY not #defined");
#endif   // _RWSTD_NO_EXT_COLLATE_PRIMARY


    // exercise _RWSTD_NO_EXT_CTYPE_PRIMARY
    rw_info (0, 0, __LINE__, "_RWSTD_NO_EXT_CTYPE_PRIMARY");

#ifndef _RWSTD_NO_EXT_CTYPE_PRIMARY
    rw_assert (0, __FILE__, __LINE__,
               "_RWSTD_NO_EXT_CTYPE_PRIMARY not #defined");
#endif   // _RWSTD_NO_EXT_CTYPE_PRIMARY


    // exercise _RWSTD_NO_EXT_MONEYPUNCT_PRIMARY
    rw_info (0, 0, __LINE__, "_RWSTD_NO_EXT_MONEYPUNCT_PRIMARY");

#ifndef _RWSTD_NO_EXT_MONEYPUNCT_PRIMARY
    rw_assert (0, __FILE__, __LINE__,
               "_RWSTD_NO_EXT_MONEYPUNCT_PRIMARY not #defined");
#endif   // _RWSTD_NO_EXT_MONEYPUNCT_PRIMARY


    // exercise _RWSTD_NO_EXT_NUMPUNCT_PRIMARY
    rw_info (0, 0, __LINE__, "_RWSTD_NO_EXT_NUMPUNCT_PRIMARY");

#ifndef _RWSTD_NO_EXT_NUMPUNCT_PRIMARY
    rw_assert (0, __FILE__, __LINE__,
               "_RWSTD_NO_EXT_NUMPUNCT_PRIMARY not #defined");
#endif   // _RWSTD_NO_EXT_NUMPUNCT_PRIMARY


    // exercise _RWSTD_NO_EXT_TIME_GET_PRIMARY
    rw_info (0, 0, __LINE__, "_RWSTD_NO_EXT_TIME_GET_PRIMARY");

#ifndef _RWSTD_NO_EXT_TIME_GET_PRIMARY
    rw_assert (0, __FILE__, __LINE__,
               "_RWSTD_NO_EXT_TIME_GET_PRIMARY not #defined");
#endif   // _RWSTD_NO_EXT_TIME_GET_PRIMARY


    // exercise _RWSTD_NO_EXT_TIME_PUT_PRIMARY
    rw_info (0, 0, __LINE__, "_RWSTD_NO_EXT_TIME_PUT_PRIMARY");

#ifndef _RWSTD_NO_EXT_TIME_PUT_PRIMARY
    rw_assert (0, __FILE__, __LINE__,
               "_RWSTD_NO_EXT_TIME_PUT_PRIMARY not #defined");
#endif   // _RWSTD_NO_EXT_TIME_PUT_PRIMARY


    // exercise _RWSTD_NO_EXT_MESSAGES_PRIMARY
    rw_info (0, 0, __LINE__, "_RWSTD_NO_EXT_MESSAGES_PRIMARY");

#ifndef _RWSTD_NO_EXT_MESSAGES_PRIMARY
    rw_assert (0, __FILE__, __LINE__,
               "_RWSTD_NO_EXT_MESSAGES_PRIMARY not #defined");
#endif   // _RWSTD_NO_EXT_MESSAGES_PRIMARY

    return 0;
}

/* extern */ int
main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "intro.compliance",
                    0,   // no comment
                    run_test, "", 0);
}

