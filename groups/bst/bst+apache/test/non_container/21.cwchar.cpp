/***************************************************************************
 *
 * 21.cwchar.cpp - test exercising [lib.string.c.strings], Table 48
 *
 * $Id: 21.cwchar.cpp 597425 2007-11-22 15:20:29Z faridz $
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
 * Copyright 2004-2005 Rogue Wave Software.
 *
 **************************************************************************/

// used in the EVAL() macro below purely to make diagnostic messages
// more informative (i.e., to display which config macros are defined
// when a function isn't declared -- it may not make sense to declare
// some functions if they are not defined in the C library)
enum {
    _RWSTD_NO_BTOWC = 1, _RWSTD_NO_BTOWC_IN_LIBC = 1,
    _RWSTD_NO_FGETWC = 1, _RWSTD_NO_FGETWC_IN_LIBC = 1,
    _RWSTD_NO_FGETWS = 1, _RWSTD_NO_FGETWS_IN_LIBC = 1,
    _RWSTD_NO_FPUTWC = 1, _RWSTD_NO_FPUTWC_IN_LIBC = 1,
    _RWSTD_NO_FPUTWS = 1, _RWSTD_NO_FPUTWS_IN_LIBC = 1,
    _RWSTD_NO_FWIDE = 1, _RWSTD_NO_FWIDE_IN_LIBC = 1,
    _RWSTD_NO_FWPRINTF = 1, _RWSTD_NO_FWPRINTF_IN_LIBC = 1,
    _RWSTD_NO_FWSCANF = 1, _RWSTD_NO_FWSCANF_IN_LIBC = 1,
    _RWSTD_NO_GETWC = 1, _RWSTD_NO_GETWC_IN_LIBC = 1,
    _RWSTD_NO_GETWCHAR = 1, _RWSTD_NO_GETWCHAR_IN_LIBC = 1,
    _RWSTD_NO_MBRLEN = 1, _RWSTD_NO_MBRLEN_IN_LIBC = 1,
    _RWSTD_NO_MBRTOWC = 1, _RWSTD_NO_MBRTOWC_IN_LIBC = 1,
    _RWSTD_NO_MBSINIT = 1, _RWSTD_NO_MBSINIT_IN_LIBC = 1,
    _RWSTD_NO_MBSRTOWCS = 1, _RWSTD_NO_MBSRTOWCS_IN_LIBC = 1,
    _RWSTD_NO_PUTWC = 1, _RWSTD_NO_PUTWC_IN_LIBC = 1,
    _RWSTD_NO_PUTWCHAR = 1, _RWSTD_NO_PUTWCHAR_IN_LIBC = 1,
    _RWSTD_NO_SWPRINTF = 1, _RWSTD_NO_SWPRINTF_IN_LIBC = 1,
    _RWSTD_NO_SWSCANF = 1, _RWSTD_NO_SWSCANF_IN_LIBC = 1,
    _RWSTD_NO_UNGETWC = 1, _RWSTD_NO_UNGETWC_IN_LIBC = 1,
    _RWSTD_NO_VFWPRINTF = 1, _RWSTD_NO_VFWPRINTF_IN_LIBC = 1,
    _RWSTD_NO_VSWPRINTF = 1, _RWSTD_NO_VSWPRINTF_IN_LIBC = 1,
    _RWSTD_NO_VWPRINTF = 1, _RWSTD_NO_VWPRINTF_IN_LIBC = 1,
    _RWSTD_NO_WCRTOMB = 1, _RWSTD_NO_WCRTOMB_IN_LIBC = 1,
    _RWSTD_NO_WCSCAT = 1, _RWSTD_NO_WCSCAT_IN_LIBC = 1,
    _RWSTD_NO_WCSCHR = 1, _RWSTD_NO_WCSCHR_IN_LIBC = 1,
    _RWSTD_NO_WCSCMP = 1, _RWSTD_NO_WCSCMP_IN_LIBC = 1,
    _RWSTD_NO_WCSCOLL = 1, _RWSTD_NO_WCSCOLL_IN_LIBC = 1,
    _RWSTD_NO_WCSCPY = 1, _RWSTD_NO_WCSCPY_IN_LIBC = 1,
    _RWSTD_NO_WCSCSPN = 1, _RWSTD_NO_WCSCSPN_IN_LIBC = 1,
    _RWSTD_NO_WCSFTIME = 1, _RWSTD_NO_WCSFTIME_IN_LIBC = 1,
    _RWSTD_NO_WCSLEN = 1, _RWSTD_NO_WCSLEN_IN_LIBC = 1,
    _RWSTD_NO_WCSNCAT = 1, _RWSTD_NO_WCSNCAT_IN_LIBC = 1,
    _RWSTD_NO_WCSNCMP = 1, _RWSTD_NO_WCSNCMP_IN_LIBC = 1,
    _RWSTD_NO_WCSNCPY = 1, _RWSTD_NO_WCSNCPY_IN_LIBC = 1,
    _RWSTD_NO_WCSPBRK = 1, _RWSTD_NO_WCSPBRK_IN_LIBC = 1,
    _RWSTD_NO_WCSRCHR = 1, _RWSTD_NO_WCSRCHR_IN_LIBC = 1,
    _RWSTD_NO_WCSRTOMBS = 1, _RWSTD_NO_WCSRTOMBS_IN_LIBC = 1,
    _RWSTD_NO_WCSSPN = 1, _RWSTD_NO_WCSSPN_IN_LIBC = 1,
    _RWSTD_NO_WCSSTR = 1, _RWSTD_NO_WCSSTR_IN_LIBC = 1,
    _RWSTD_NO_WCSTOD = 1, _RWSTD_NO_WCSTOD_IN_LIBC = 1,
    _RWSTD_NO_WCSTOK = 1, _RWSTD_NO_WCSTOK_IN_LIBC = 1,
    _RWSTD_NO_WCSTOL = 1, _RWSTD_NO_WCSTOL_IN_LIBC = 1,
    _RWSTD_NO_WCSTOUL = 1, _RWSTD_NO_WCSTOUL_IN_LIBC = 1,
    _RWSTD_NO_WCSXFRM = 1, _RWSTD_NO_WCSXFRM_IN_LIBC = 1,
    _RWSTD_NO_WCTOB = 1, _RWSTD_NO_WCTOB_IN_LIBC = 1,
    _RWSTD_NO_WMEMCHR = 1, _RWSTD_NO_WMEMCHR_IN_LIBC = 1,
    _RWSTD_NO_WMEMCMP = 1, _RWSTD_NO_WMEMCMP_IN_LIBC = 1,
    _RWSTD_NO_WMEMCPY = 1, _RWSTD_NO_WMEMCPY_IN_LIBC = 1,
    _RWSTD_NO_WMEMMOVE = 1, _RWSTD_NO_WMEMMOVE_IN_LIBC = 1,
    _RWSTD_NO_WMEMSET = 1, _RWSTD_NO_WMEMSET_IN_LIBC = 1,
    _RWSTD_NO_WPRINTF = 1, _RWSTD_NO_WPRINTF_IN_LIBC = 1,
    _RWSTD_NO_WSCANF = 1, _RWSTD_NO_WSCANF_IN_LIBC = 1
};

/**************************************************************************/

#include <cwchar>
#include <any.h>      // for rw_any_t
#include <driver.h>   // for rw_test(), ...

/**************************************************************************/

// detect missing macros
const char* const missing_macros [] = {

#ifndef NULL
    "NULL",
#else   // if defined (NULL)
    "",
#endif   // NULL

#ifndef WCHAR_MIN
    "WCHAR_MIN",
#else   // if defined (WCHAR_MIN)
    "",
#endif   // WCHAR_MIN

#ifndef WCHAR_MAX
    "WCHAR_MAX",
#else   // if defined (WCHAR_MAX)
    "",
#endif   // WCHAR_MAX

#ifndef WEOF
    "WEOF",
#else   // if defined (WEOF)
    "",
#endif   // WEOF

    0
};

/**************************************************************************/

// detect masking macros and undefine them to avoid compilation errors
// they might cause otherwise
const char* const masking_macros [] = {

#ifdef fwprintf
    "fwprintf",
#  undef fwprintf
#else
    "",
#endif   // fwprintf

#ifdef fwscanf
    "fwscanf",
#  undef fwscanf
#else
    "",
#endif   // fwscanf

#ifdef wprintf
    "wprintf",
#  undef wprintf
#else
    "",
#endif   // wprintf

#ifdef wscanf
    "wscanf",
#  undef wscanf
#else
    "",
#endif   // wscanf

#ifdef swprintf
    "swprintf",
#  undef swprintf
#else
    "",
#endif   // swprintf

#ifdef swscanf
    "swscanf",
#  undef swscanf
#else
    "",
#endif   // swscanf

#ifdef vfwprintf
    "vfwprintf",
#  undef vfwprintf
#else
    "",
#endif   // vfwprintf

#ifdef vwprintf
    "vwprintf",
#  undef vwprintf
#else
    "",
#endif   // vwprintf

#ifdef vswprintf
    "vswprintf",
#  undef vswprintf
#else
    "",
#endif   // vswprintf

#ifdef fgetwc
    "fgetwc",
#  undef fgetwc
#else
    "",
#endif   // fgetwc

#ifdef fgetws
    "fgetws",
#  undef fgetws
#else
    "",
#endif   // fgetws

#ifdef fputwc
    "fputwc",
#  undef fputwc
#else
    "",
#endif   // fputwc

#ifdef fputws
    "fputws",
#  undef fputws
#else
    "",
#endif   // fputws

#ifdef getwc
    "getwc",
#  undef getwc
#else
    "",
#endif   // getwc

#ifdef getwchar
    "getwchar",
#  undef getwchar
#else
    "",
#endif   // getwchar

#ifdef putwc
    "putwc",
#  undef putwc
#else
    "",
#endif   // putwc

#ifdef putwchar
    "putwchar",
#  undef putwchar
#else
    "",
#endif   // putwchar

#ifdef ungetwc
    "ungetwc",
#  undef ungetwc
#else
    "",
#endif   // ungetwc

#ifdef fwide
    "fwide",
#  undef fwide
#else
    "",
#endif   // fwide

#ifdef wcstod
    "wcstod",
#  undef wcstod
#else
    "",
#endif   // wcstod

#ifdef wcstol
    "wcstol",
#  undef wcstol
#else
    "",
#endif   // wcstol

#ifdef wcstoul
    "wcstoul",
#  undef wcstoul
#else
    "",
#endif   // wcstoul

#ifdef wcscpy
    "wcscpy",
#  undef wcscpy
#else
    "",
#endif   // wcscpy

#ifdef wcsncpy
    "wcsncpy",
#  undef wcsncpy
#else
    "",
#endif   // wcsncpy

#ifdef wcscat
    "wcscat",
#  undef wcscat
#else
    "",
#endif   // wcscat

#ifdef wcsncat
    "wcsncat",
#  undef wcsncat
#else
    "",
#endif   // wcsncat

#ifdef wcscmp
    "wcscmp",
#  undef wcscmp
#else
    "",
#endif   // wcscmp

#ifdef wcscoll
    "wcscoll",
#  undef wcscoll
#else
    "",
#endif   // wcscoll

#ifdef wcsncmp
    "wcsncmp",
#  undef wcsncmp
#else
    "",
#endif   // wcsncmp

#ifdef wcsxfrm
    "wcsxfrm",
#  undef wcsxfrm
#else
    "",
#endif   // wcsxfrm

#ifdef wcschr
    "wcschr",
#  undef wcschr
#else
    "",
#endif   // wcschr

#ifdef wcscspn
    "wcscspn",
#  undef wcscspn
#else
    "",
#endif   // wcscspn

#ifdef wcspbrk
    "wcspbrk",
#  undef wcspbrk
#else
    "",
#endif   // wcspbrk

#ifdef wcsrchr
    "wcsrchr",
#  undef wcsrchr
#else
    "",
#endif   // wcsrchr

#ifdef wcsspn
    "wcsspn",
#  undef wcsspn
#else
    "",
#endif   // wcsspn

#ifdef wcsstr
    "wcsstr",
#  undef wcsstr
#else
    "",
#endif   // wcsstr

#ifdef wcstok
    "wcstok",
#  undef wcstok
#else
    "",
#endif   // wcstok

#ifdef wcslen
    "wcslen",
#  undef wcslen
#else
    "",
#endif   // wcslen

#ifdef wmemchr
    "wmemchr",
#  undef wmemchr
#else
    "",
#endif   // wmemchr

#ifdef wmemcmp
    "wmemcmp",
#  undef wmemcmp
#else
    "",
#endif   // wmemcmp

#ifdef wmemcpy
    "wmemcpy",
#  undef wmemcpy
#else
    "",
#endif   // wmemcpy

#ifdef wmemmove
    "wmemmove",
#  undef wmemmove
#else
    "",
#endif   // wmemmove

#ifdef wmemset
    "wmemset",
#  undef wmemset
#else
    "",
#endif   // wmemset

#ifdef wcsftime
    "wcsftime",
#  undef wcsftime
#else
    "",
#endif   // wcsftime

#ifdef btowc
    "btowc",
#  undef btowc
#else
    "",
#endif   // btowc

#ifdef wctob
    "wctob",
#  undef wctob
#else
    "",
#endif   // wctob

#ifdef mbsinit
    "mbsinit",
#  undef mbsinit
#else
    "",
#endif   // mbsinit

#ifdef mbrlen
    "mbrlen",
#  undef mbrlen
#else
    "",
#endif   // mbrlen

#ifdef mbrtowc
    "mbrtowc",
#  undef mbrtowc
#else
    "",
#endif   // mbrtowc

#ifdef wcrtomb
    "wcrtomb",
#  undef wcrtomb
#else
    "",
#endif   // wcrtomb

#ifdef mbsrtowcs
    "mbsrtowcs",
#  undef mbsrtowcs
#else
    "",
#endif   // mbsrtowcs

#ifdef wcsrtombs
    "wcsrtombs",
#  undef wcsrtombs
#else
    "",
#endif   // wcsrtombs

    0
};

/**************************************************************************/

static void
test_macros ()
{
    rw_info (0, 0, 0, "checking for missing and masking macros");

    for (unsigned i = 0; missing_macros [i]; ++i) {
        rw_assert ('\0' == missing_macros [i][0], 0, __LINE__,
                   "macro %s not defined", missing_macros [i]);
    }

    for (unsigned i = 0; masking_macros [i]; ++i) {
#ifdef _RWSTD_STRICT_ANSI
        rw_assert ('\0' == masking_macros [i][0], 0, __LINE__,
                   "masking macro %s unexpectedly defined",
                   masking_macros [i]);
#else
        rw_warn ('\0' == masking_macros [i][0], 0, __LINE__,
                 "masking macro %s unexpectedly defined",
                 masking_macros [i]);
#endif
    }
}

/**************************************************************************/

#ifndef _RWSTD_NO_NAMESPACE

// check types

namespace Fallback {

struct size_t {
    int i_;
    char dummy_ [256];   // make sure we're bigger than the real thing

    // this (fake) size_t emulates a scalar type
    size_t (int i): i_ (i) { }
    operator int () const { return i_; }
};

struct mbstate_t {
    char dummy_ [256];   // make sure we're bigger than the real thing
};

struct wint_t {
    int i_;
    char dummy_ [256];   // make sure we're bigger than the real thing

    // this (fake) wint_t emulates a scalar type
    wint_t (int i): i_ (i) { }
    operator int () const { return i_; }
};

struct FILE;

struct tm;

}    // namespace Fallback

namespace std {

// define test functions in namespace std to detect the presece
// or absence of the required types

namespace Nested {

using namespace Fallback;

// each test_xxx_t typedef aliases std::xxx_t if the corresponding
// type is defined in namespace std, or Fallback::xxx_t otherwise
typedef size_t    test_size_t;
typedef mbstate_t test_mbstate_t;
typedef wint_t    test_wint_t;
typedef tm        test_tm;

}   // namespace Nested

}   // namespace std

typedef std::Nested::test_size_t    test_size_t;
typedef std::Nested::test_mbstate_t test_mbstate_t;
typedef std::Nested::test_wint_t    test_wint_t;
typedef std::Nested::test_tm        test_tm;


template <class StructTm>
int tm_defined (const StructTm*) { return 1; }
int tm_defined (const Fallback::tm*) { return 0; }


const char std_name[] = "std";

static void
test_types ()
{
    rw_info (0, 0, 0,
             "types %s::size_t, %1$s::mbstate_t, %1$s::wint_t, and %1$s::tm",
             std_name);

    rw_assert (sizeof (test_size_t) != sizeof (Fallback::size_t), 0, 0,
               "%s::size_t not defined", std_name);
    rw_assert (sizeof (test_mbstate_t) != sizeof (Fallback::mbstate_t), 0, 0,
               "%s::mbstate_t not defined", std_name);
    rw_assert (sizeof (test_wint_t) != sizeof (Fallback::wint_t), 0, 0,
               "%s::wint_t not defined", std_name);

    rw_assert (tm_defined ((const test_tm*)0), 0, 0,
               "%s::tm not defined", std_name);
}

#else   // if defined (_RWSTD_NO_NAMESPACE)

const char std_name[] = "";

static void
test_types ()
{
    rw_info (0, 0, 0,
             "types %s::size_t, %1$s::mbstate_t, %1$s::wint_t, and %1$s::tm",
             std_name);

    rw_note (0, 0, 0, "_RWSTD_NO_NAMESPACE defined, cannot test");
}

#endif   // _RWSTD_NO_NAMESPACE

/**************************************************************************/

int ncalls;

_RWSTD_NAMESPACE (std) {

template <class FileT, class WCharT>
int fwprintf (FileT*, const WCharT*, ...) { return ncalls++; }

template <class FileT, class WCharT>
int fwscanf (FileT*, const WCharT*, ...) { return ncalls++; }

template <class WCharT>
int wprintf (const WCharT* dummy, ...) { return ncalls++; }

template <class WCharT>
int wscanf (const WCharT* dummy, ...) { return ncalls++; }

template <class WCharT, class SizeT>
int swprintf (WCharT*, SizeT, const WCharT* dummy, ...) { return ncalls++; }

template <class WCharT>
int swscanf (const WCharT*, const WCharT* dummy, ...) { return ncalls++; }

template <class FileT, class WCharT, class VAList>
int vfwprintf (FileT*, const WCharT*, VAList) { return ncalls++; }

template <class WCharT, class VAList>
int vwprintf (const WCharT*, VAList) { return ncalls++; }

template <class WCharT, class SizeT, class VAList>
int vswprintf (WCharT*, SizeT, const WCharT*, VAList) { return ncalls++; }

template <class FileT>
wint_t fgetwc (FileT*) { return ncalls++; }

template <class WCharT, class FileT>
WCharT* fgetws (WCharT*, int, FileT*) { ncalls++; return 0; }

template <class WCharT, class FileT>
test_wint_t fputwc (WCharT, FileT*) { return ncalls++; }

template <class WCharT, class FileT>
int fputws (const WCharT*, FileT*) { return ncalls++; }

template <class FileT>
test_wint_t getwc (FileT*) { return ncalls++; }

// cannot exercise
// test_wint_t getwchar();

template <class WCharT, class FileT>
test_wint_t putwc (WCharT, FileT*) { return ncalls++; }

template <class WCharT>
test_wint_t putwchar (WCharT) { return ncalls++; }

template <class WIntT, class FileT>
test_wint_t ungetwc (WIntT, FileT*) { return ncalls++; }

template <class FileT>
int fwide (FileT*, int) { return ncalls++; }

template <class WCharT>
double wcstod (const WCharT*, WCharT**) { return ncalls++; }

template <class WCharT>
long wcstol (const WCharT*, WCharT**, int) { return ncalls++; }

template <class WCharT>
unsigned long wcstoul (const WCharT*, WCharT**, int) { return ncalls++; }

template <class WCharT>
WCharT* wcscpy (WCharT*, const WCharT*) { ncalls++; return 0; }

template <class WCharT, class SizeT>
WCharT* wcsncpy (WCharT*, const WCharT*, SizeT) { ncalls++; return 0; }

template <class WCharT>
WCharT* wcscat (WCharT*, const WCharT*) { ncalls++; return 0; }

template <class WCharT, class SizeT>
WCharT* wcsncat (WCharT*, const WCharT*, SizeT) { ncalls++; return 0; }

template <class WCharT>
int wcscmp (const WCharT*, const WCharT*) { return ncalls++; }

template <class WCharT>
int wcscoll (const WCharT*, const WCharT*) { return ncalls++; }

template <class WCharT, class SizeT>
int wcsncmp (const WCharT*, const WCharT*, SizeT) { return ncalls++; }

template <class WCharT, class SizeT>
SizeT wcsxfrm (WCharT*, const WCharT*, SizeT) { return ncalls++; }

template <class WCharT>
WCharT* wcschr (WCharT*, WCharT) { ncalls++; return 0; }

template <class WCharT>
test_size_t wcscspn (const WCharT*, const WCharT*) { return ncalls++; }

template <class WCharT>
WCharT* wcspbrk (WCharT*, const WCharT*) { ncalls++; return 0; }

template <class WCharT>
WCharT* wcsrchr (WCharT*, WCharT) { ncalls++; return 0; }

template <class WCharT>
test_size_t wcsspn (const WCharT*, const WCharT*) { return ncalls++; }

template <class WCharT>
WCharT* wcsstr (WCharT*, const WCharT*) { ncalls++; return 0; }

template <class WCharT>
WCharT* wcstok (WCharT*, const WCharT*, WCharT**) { ncalls++; return 0; }

template <class WCharT>
test_size_t wcslen (const WCharT*) { return ncalls++; }

template <class WCharT, class SizeT>
WCharT* wmemchr (WCharT*, WCharT, SizeT) { ncalls++; return 0; }

template <class WCharT, class SizeT>
int wmemcmp (const WCharT*, const WCharT*, SizeT) { return ncalls++; }

template <class WCharT, class SizeT>
WCharT* wmemcpy (WCharT*, const WCharT*, SizeT) { ncalls++; return 0; }

template <class WCharT, class SizeT>
WCharT* wmemmove (WCharT*, const WCharT*, SizeT) { ncalls++; return 0; }

template <class WCharT, class SizeT>
WCharT* wmemset (WCharT*, WCharT, SizeT) { ncalls++; return 0; }

template <class WCharT, class SizeT, class StructTm>
SizeT wcsftime (WCharT*, SizeT, const WCharT*, const StructTm*)
{ return ncalls++; }

template <class IntT>
test_wint_t btowc (IntT) { return ncalls++; }

template <class WIntT>
int wctob (WIntT) { return ncalls++; }

template <class MBStateT>
int mbsinit (const MBStateT*) { return ncalls++; }

template <class SizeT, class MBStateT>
SizeT mbrlen (const char*, SizeT, MBStateT*) { return ncalls++; }

template <class WCharT, class SizeT, class MBStateT>
SizeT mbrtowc (WCharT*, const char*, SizeT, MBStateT*) { return ncalls++; }

template <class WCharT, class MBStateT>
test_size_t wcrtomb (char*, WCharT, MBStateT*) { return ncalls++; }

template <class WCharT, class SizeT, class MBStateT>
SizeT mbsrtowcs (WCharT*, const char**, SizeT, MBStateT*) { return ncalls++; }

template <class WCharT, class SizeT, class MBStateT>
SizeT wcsrtombs (char*, const WCharT**, SizeT, MBStateT*) { return ncalls++; }


template <class WCharT>
const WCharT* wcschr (const WCharT*, WCharT) { ncalls++; return 0; }

template <class WCharT>
const WCharT* wcspbrk (const WCharT*, const WCharT*) { ncalls++; return 0; }

template <class WCharT>
const WCharT* wcsrchr (const WCharT*, WCharT) { ncalls++; return 0; }

template <class WCharT>
const WCharT* wcsstr (const WCharT*, const WCharT*) { ncalls++; return 0; }

template <class WCharT, class SizeT>
const WCharT* wmemchr (const WCharT*, WCharT, SizeT) { ncalls++; return 0; }

}   // namespace std


struct UniqType;

#define GET_TYPE_NAME(T)                                                \
    template <class Type>                                               \
    const char* get_type_name (T, Type) {                               \
        return rw_any_t (Type ()).type_name ();                         \
    }                                                                   \
    const char* get_type_name (T, T) { return 0; }                      \
    typedef void unused_typedef /* allow a terminating semicolon */

GET_TYPE_NAME (short);
GET_TYPE_NAME (unsigned short);
GET_TYPE_NAME (int);
GET_TYPE_NAME (unsigned int);
GET_TYPE_NAME (long);
GET_TYPE_NAME (unsigned long);
GET_TYPE_NAME (double);

#ifndef _RWSTD_NO_LONG_LONG
GET_TYPE_NAME (_RWSTD_LONG_LONG);
GET_TYPE_NAME (unsigned _RWSTD_LONG_LONG);
#endif

#ifndef _RWSTD_NO_NATIVE_WCHAR_T
GET_TYPE_NAME (wchar_t);
#endif

GET_TYPE_NAME (wchar_t*);
GET_TYPE_NAME (const wchar_t*);


#define EVAL2(macro)         !!(macro - 1)
#define EVAL(name)           EVAL2 (_RWSTD_NO_ ## name + 0)
#define EVAL_IN_LIBC(name)   EVAL (name ## _IN_LIBC)


static int rw_opt_no_macros;     // for --no-macros
static int rw_opt_no_types;      // for --no-types
static int rw_opt_no_functions;  // for --no-functions
static int rw_opt_no_overloads;  // for --no-overloads


void test_functions ()
{
#define TEST(T, fun, args, macro, overload)                             \
    do {                                                                \
        cstr = (str = array);                                           \
        wcstr = (wstr = warray);                                        \
        *str = '\0'; *wstr = L'\0';                                     \
        ncalls = 0;                                                     \
        rw_info (0, 0, __LINE__, "%s::%s("                              \
                 "%{?}%{:}/* %{?}non-%{;}const overload */%{;})",       \
                 std_name, #fun, overload < 0, 0 == overload);          \
        const char* const return_type_name =                            \
            get_type_name ((T)0, std::fun args);                        \
        rw_warn (0 == ncalls, 0, __LINE__,                              \
                 "%s::%s("                                              \
                 "%{?}%{:}/* %{?}non-%{;}const overload */%{;}) "       \
                 "not declared (_RWSTD_NO_%s = %d, "                    \
                 "_RWSTD_NO_%s_IN_LIBC = %d)",                          \
                 std_name, #fun, overload < 0, 0 == overload,           \
                 #macro, EVAL (macro), #macro, EVAL_IN_LIBC (macro));   \
        if (0 == ncalls)                                                \
            rw_assert (0 == return_type_name, 0, __LINE__, "%s::%s("    \
                       "%{?}%{:}/* %{?}non-%{;}const overload */%{;}) " \
                       "expected return type %s, got %s",               \
                       std_name, #fun, overload < 0, 0 == overload,     \
                       #T, return_type_name);                           \
    } while (0)

    char array [4] = "";

    char* str        = array;
    const char* cstr = array;

    wchar_t warray [32] = L"";

    /* */ wchar_t* /* */ wstr  = warray;
    const wchar_t* /* */ wcstr = warray;

    test_size_t size = 0;

    const UniqType* const uniqptr = 0;

    const test_wint_t wi = 0;

    const int i = 0;
    TEST (int, wprintf, (L""), WPRINTF, -1);
    TEST (int, wprintf, (L"", uniqptr), WPRINTF, -1);

    TEST (int, wscanf, (L""), WSCANF, -1);
    TEST (int, wscanf, (L"", uniqptr), WSCANF, -1);

    TEST (int, swprintf, (wstr, size, L""), SWPRINTF, -1);
    TEST (int, swprintf, (wstr, size, L"", uniqptr), SWPRINTF, -1);

    TEST (int, swscanf, (wstr, L""), SWSCANF, -1);
    TEST (int, swscanf, (wstr, L"", uniqptr), SWSCANF, -1);

    TEST (double, wcstod, (L"", &wstr), WCSTOD, -1);
    TEST (long, wcstol, (L"", &wstr, i), WCSTOL, -1);
    TEST (unsigned long, wcstoul, (L"", &wstr, i), WCSTOUL, -1);

    TEST (wchar_t*, wcscpy, (wstr, L""), WCSCPY, -1);
    TEST (wchar_t*, wcsncpy, (wstr, L"", size), WCSNCPY, -1);
    TEST (wchar_t*, wcscat, (wstr, L""), WCSCAT, -1);
    TEST (int, wcscmp, (wstr, L""), WCSCMP, -1);
    TEST (int, wcsncmp, (wstr, L"", size), WCSNCMP, -1);
    TEST (test_size_t, wcsxfrm, (wstr, L"", size), WCSXFRM, -1);

    TEST (test_size_t, wcscspn, (L"", L""), WCSCSPN, -1);

    TEST (test_size_t, wcsspn, (L"", L""), WCSSPN, -1);
    TEST (wchar_t*, wcstok, (wstr, L"", &wstr), WCSTOK, -1);

    TEST (test_size_t, wcslen, (L""), WCSLEN, -1);
    TEST (int, wmemcmp, (L"", L"", size), WMEMCMP, -1);
    TEST (wchar_t*, wmemcpy, (wstr, L"", size), WMEMCPY, -1);
    TEST (wchar_t*, wmemmove, (wstr, L"", size), WMEMMOVE, -1);
    TEST (wchar_t*, wmemset, (wstr, L'\0', size), WMEMSET, -1);

    // const commented to prevent MSVC 7.0 error:
    // error C2147: 'tm_buf' : const automatic array must be fully initialized
    /* const */ int tm_buf [16] = { 0 };
    const test_tm* tmb = (const test_tm*)&tm_buf;

#ifdef _MSC_VER
    // prevent MSVC parameter validation error:
    // "Zero length output buffer passed to strftime"
    size = 1;
#endif

    TEST (test_size_t, wcsftime, (wstr, size, L"", tmb), WCSFTIME, -1);

#ifdef _MSC_VER
    // restore size
    size = 0;
#endif

    TEST (test_wint_t, btowc, (i), BTOWC, -1);
    TEST (int, wctob, (wi), WCTOB, -1);

    test_mbstate_t state = test_mbstate_t ();

    TEST (int, mbsinit, (&state), MBSINIT, -1);
    TEST (test_size_t, mbrlen, ("", size, &state), MBRLEN, -1);
    TEST (test_size_t, mbrtowc, (wstr, "", size, &state), MBRTOWC, -1);
    TEST (test_size_t, wcrtomb, (str, L'\0', &state), WCRTOMB, -1);

    TEST (test_size_t, mbsrtowcs, (wstr, &cstr, size, &state), MBSRTOWCS, -1);
    TEST (test_size_t, wcsrtombs, (str, &wcstr, size, &state), WCSRTOMBS, -1);

    if (rw_opt_no_overloads) {
        // exercise the traditional C (const-incorrect) functions
        TEST (/* const */ wchar_t*, wmemchr, (L"", L'\0', size), WMEMCHR, -1);
        TEST (/* const */ wchar_t*, wcspbrk, (wcstr, L""), WCSPBRK, -1);
        TEST (/* const */ wchar_t*, wcsrchr, (L"", L'\0'), WCSRCHR, -1);
        TEST (/* const */ wchar_t*, wcsstr, (L"", L""), WCSSTR, -1);
        TEST (/* const */ wchar_t*, wcschr, (L"", L'\0'), WCSCHR, -1);
    }
    else {
        // exercise const and non-const overloads that C++ replaces
        // the traditional C functions with
        TEST (const wchar_t*, wmemchr, (L"", L'\0', size), WMEMCHR, 1);
        TEST (wchar_t*, wmemchr, (wstr, L'\0', size), WMEMCHR, 0);

        TEST (const wchar_t*, wcspbrk, (wcstr, L""), WCSPBRK, 1);
        TEST (wchar_t*, wcspbrk, ((const wchar_t*)wstr,(const wchar_t*) L""), WCSPBRK, 0);

        TEST (const wchar_t*, wcsrchr, (L"", L'\0'), WCSRCHR, 1);
        TEST (wchar_t*, wcsrchr, (wstr, L'\0'), WCSRCHR, 0);

        TEST (const wchar_t*, wcsstr, (L"", L""), WCSSTR, 1);
        TEST (wchar_t*, wcsstr, (wstr, L""), WCSSTR, 0);

        TEST (const wchar_t*, wcschr, (L"", L'\0'), WCSCHR, 1);
        TEST (wchar_t*, wcschr, (wstr, L'\0'), WCSCHR, 0);
    }
}

/**************************************************************************/

// included here to avoid namespace pollution
#include <cstdarg>       // for va_list
#include <cstdio>        // for FILE, fopen()
#include <file.h>        // for DEV_NUL
#include <rw_printf.h>   // for rw_stdout

namespace std {

// define test types in namespace std to detect the presece
// or absence of the required types

namespace Nested {

using namespace Fallback;

typedef FILE    test_FILE;
typedef va_list test_va_list;

}   // namespace Nested

}   // namespace std

typedef std::Nested::test_FILE    test_FILE;
typedef std::Nested::test_va_list test_va_list;


void test_file_functions (int dummy, ...)
{
    char array [4] = "";

    /* */ char* str  = array;
    const char* cstr = array;

    wchar_t warray [32] = L"";

    /* */ wchar_t* /* */ wstr  = warray;
    const wchar_t* /* */ wcstr = warray;

    const int i = 0;

    const UniqType* const uniqptr = 0;

    const test_wint_t wi = 0;

    // cast rw_stdout to FILE* via void* since the latter may be
    // an incomplete type and casts between two non-void pointers
    // require that the types be complete (in case they are related
    // by inheritance and need to be adjusted)
    test_FILE* const fp = (test_FILE*)std::fopen (DEV_NULL, "w");
    test_va_list va;
    va_start (va, dummy);

    // call fwide() first before any prior output since 7.19.2, p5
    // of C99 prohibits wide character I/O functions from being called
    // on a byte-oriented stream
    TEST (int, fwide, (fp, i), FWIDE, -1);

    TEST (int, fwprintf, (fp, L""), FWPRINTF, -1);
    TEST (int, fwprintf, (fp, L"", uniqptr), FWPRINTF, -1);

    TEST (int, fwscanf, (fp, L""), FWSCANF, -1);
    TEST (int, fwscanf, (fp, L"", uniqptr), FWSCANF, -1);

    TEST (int, vfwprintf, (fp, L"", va), VFWPRINTF, -1);

    TEST (test_wint_t, fgetwc, (fp), FGETWC, -1);
    TEST (wchar_t*, fgetws, (wstr, i, fp), FGETWS, -1);
    TEST (test_wint_t, fputwc, (L'\0', fp), FPUTWC, -1);
    TEST (int, fputws, (L"", fp), FPUTWS, -1);
    TEST (test_wint_t, getwc, (fp), GETWC, -1);
    TEST (test_wint_t, putwc, (L'\0', fp), PUTWC, -1);
    TEST (test_wint_t, ungetwc, (wi, fp), UNGETWC, -1);

    _RWSTD_UNUSED (str);
    _RWSTD_UNUSED (cstr);
    _RWSTD_UNUSED (wcstr);
}

/**************************************************************************/

static int
run_test (int, char**)
{
    if (rw_opt_no_macros)
        rw_note (0, 0, 0, "test for macros disabled");
    else
        test_macros ();

    if (rw_opt_no_types)
        rw_note (0, 0, 0, "test for types disabled");
    else
        test_types ();

    if (rw_opt_no_functions)
        rw_note (0, 0, 0, "test for functions disabled");
    else {
        test_functions ();
        test_file_functions (0 /* dummy */);
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.c.strings",
                    "header <cwchar>",
                    run_test,
                    "|-no-macros#0-1 "
                    "|-no-types#0-1 "
                    "|-no-functions#0-1 "
                    "|-no-overloads#0-1 ",
                    &rw_opt_no_macros,
                    &rw_opt_no_types,
                    &rw_opt_no_functions,
                    &rw_opt_no_overloads);
}
