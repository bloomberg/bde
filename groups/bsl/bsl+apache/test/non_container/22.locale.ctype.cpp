/***************************************************************************
 *
 * 22.locale.ctype.cpp -  Tests exercising the ctype facet
 *
 * $Id: 22.locale.ctype.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2001-2006 Rogue Wave Software.
 *
 **************************************************************************/

// DESCRIPTION: test iterates over the locales installed on a machine,
//              calling the C character classification functions and
//              their C++ counterpart(s), comparing the results of
//              the calls against one another.

#include <bsls_platform.h>

#include <rw/_defs.h>

#if defined __linux__
   // on Linux define _XOPEN_SOURCE to get CODESET defined in <langinfo.h>
#  define _XOPEN_SOURCE   500   /* Single Unix conformance */
   // bring __int32_t into scope (otherwise <wctype.h> fails to compile)
#  include <sys/types.h>
#endif   // __linux__

// see Onyx PR #28150
#if defined (__SUNPRO_CC) && __SUNPRO_CC <= 0x540
#  include <wchar.h>
#endif // defined (__SUNPRO_CC) && __SUNPRO_CC <= 0x540

#include <locale>

#include <climits>
#include <clocale>
#include <cstring>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cwchar>     // for WEOF, btowc(), wctob()
#include <cwctype>    // for iswxxx()


#if !defined (_MSC_VER)
#  if !defined (LC_MESSAGES)
#    define LC_MESSAGES _RWSTD_LC_MESSAGES
#  endif   // LC_MESSAGES
#  include <langinfo.h>
#endif  // _MSC_VER

#include <driver.h>
#include <file.h>        // for SLASH
#include <rw_locale.h>   // for rw_locale_query()

/**************************************************************************/

// the root of the locale directory (RWSTD_LOCALE_ROOT)
// not set here to avoid Solaris 7 putenv() bug (PR #30017)
const char* locale_root;

#define NLOOPS         25
#define MAX_STR_SIZE   16

#define LOCALES "{{en-US,de-DE,fr-FR,es-ES}-*-{ISO-8859-*,UTF-8,CP125?},"  \
                 "{ja-JP-*-{EUC-JP,SHIFT_JIS,UTF-8,CP125?}},"              \
                 "{zh-CN-*-{GB*,UTF-8,CP125?}},"                           \
                 "{ru-RU-*-{KOI*,UTF-8,CP125?}}}"

#if TEST_RW_EXTENSIONS
// Only classic 'C' locale defined for stlport.
const char* locale_list = 0;
#else
const char* locale_list = "C\0";
#endif

#define BEGIN_LOCALE_LOOP(num, locname, loop_cntrl)                        \
   if (!locale_list) locale_list = rw_locale_query (LC_CTYPE, LOCALES);    \
   for (const char* locname = locale_list;                                 \
        locname && *locname; locname += std::strlen (locname) + 1) {       \
       _TRY {                                                              \
           const std::locale loc (locname);                                \
           const std::ctype<char> &ctc =                                   \
               _STD_USE_FACET (std::ctype<char>, loc);                     \
           _RWSTD_UNUSED (ctc);                                            \
           const std::ctype<charT> &ctp =                                  \
               _STD_USE_FACET (std::ctype<charT>, loc);                    \
           for (int loop_cntrl = 0; loop_cntrl < int (num); loop_cntrl++)

#define END_LOCALE_LOOP(locname)                                        \
       }                                                                \
       _CATCH (...) {                                                   \
           rw_assert (0, 0, __LINE__,                                   \
                      "locale (\"%s\") threw an exception", locname);   \
       }                                                                \
  }


// for notational convenience
typedef unsigned char UChar;

#define ALPHA   std::ctype_base::alpha
#define UPPER   std::ctype_base::upper
#define LOWER   std::ctype_base::lower
#define DIGIT   std::ctype_base::digit
#define SPACE   std::ctype_base::space
#define CNTRL   std::ctype_base::cntrl
#define PUNCT   std::ctype_base::punct
#define XDIGIT  std::ctype_base::xdigit
#define GRAPH   std::ctype_base::graph
#define PRINT   std::ctype_base::print


// wrapper functions for the c library char and wchar_t functions
int libc_isalpha (char ch)
{
    return (std::isalpha)(UChar (ch));
}

int libc_isspace (char ch)
{
    return (std::isspace)(UChar (ch));
}

int libc_isprint (char ch)
{
    return (std::isprint)(UChar (ch));
}

int libc_iscntrl (char ch)
{
    return (std::iscntrl)(UChar (ch));
}

int libc_isupper (char ch)
{
    return (std::isupper)(UChar (ch));
}

int libc_islower (char ch)
{
    return (std::islower)(UChar (ch));
}

int libc_isdigit (char ch)
{
    return (std::isdigit)(UChar (ch));
}

int libc_ispunct (char ch)
{
    return (std::ispunct)(UChar (ch));
}

int libc_isxdigit (char ch)
{
    return (std::isxdigit)(UChar (ch));
}

int libc_isalnum (char ch)
{
    return (std::isalnum)(UChar (ch));
}

int libc_isgraph (char ch)
{
    return (std::isgraph)(UChar (ch));
}

char libc_tolower (char ch)
{
    return std::tolower (UChar (ch));
}

char libc_toupper (char ch)
{
    return (std::toupper)(UChar (ch));
}

std::ctype_base::mask libc_mask (int mask, char ch, const char *locname)
{
    char curlocname [256];

    if (locname) {
        std::strcpy (curlocname, std::setlocale (LC_CTYPE, 0));

        if (0 == std::setlocale (LC_CTYPE, locname))
            return std::ctype_base::mask ();
    }

    const int c = UChar (ch);

    int result = 0;

    if (mask & ALPHA && (std::isalpha)(c))
        result |= ALPHA;
    if (mask & CNTRL && (std::iscntrl)(c))
        result |= CNTRL;
    if (mask & DIGIT && (std::isdigit)(c))
        result |= DIGIT;
    if (mask & GRAPH && (std::isgraph)(c))
        result |= GRAPH;
    if (mask & LOWER && (std::islower)(c))
        result |= LOWER;
    if (mask & PRINT && (std::isprint)(c))
        result |= PRINT;
    if (mask & PUNCT && (std::ispunct)(c))
        result |= PUNCT;
    if (mask & SPACE && (std::isspace)(c))
        result |= SPACE;
    if (mask & UPPER && (std::isupper)(c))
        result |= UPPER;
    if (mask & XDIGIT && (std::isxdigit)(c))
        result |= XDIGIT;

    if (locname)
        std::setlocale (LC_CTYPE, curlocname);

    return std::ctype_base::mask (result);
}

inline bool libc_is (std::ctype_base::mask mask, char ch, const char *locname)
{
    const std::ctype_base::mask m = libc_mask (mask, ch, locname);

    return 0 != (m & mask);
}

std::size_t c_strlen (const char *s1)
{
    return std::strlen (s1);
}

int c_strcmp (const char *s1, const char *s2)
{
    int ret = std::strcoll (s1, s2);
    return ret ? ret > 0 ? 1 : -1 : 0;
}

const char* narrow (char *dst, const char *src)
{
    if (src == dst || !src || !dst)
        return src;

    std::memcpy (dst, src, std::strlen (src) + 1);
    return dst;
}


const char* widen (char *dst, const char *src)
{
    if (src == dst || !src || !dst)
        return src;

    std::memcpy (dst, src, std::strlen (src) + 1);
    return dst;
}


char widen (char, char ch, const char*)
{
    return ch;
}

char narrow (char ch, const char*)
{
    return ch;
}

// cond1() verifies condition [1] in Test::test_narrow_widen()
// below using libc functions
bool cond1 (std::ctype_base::mask mask, char ch, const char *locname)
{
    char curlocname [256];
    std::strcpy (curlocname, std::setlocale (LC_CTYPE, 0));

    if (0 == std::setlocale (LC_CTYPE, locname))
        return false;

#ifdef __SUNPRO_CC

    // working around a SunPro bug (PR #28150)
    using std::wint_t;

#endif   // __SUNPRO_CC

#ifndef _RWSTD_NO_BTOWC

    const std::wint_t wc = std::btowc (UChar (ch));

#elif !defined (_RWSTD_NO_MBSTOWCS)

    wchar_t tmp;
    const std::wint_t wc = 1 == std::mbstowcs (&tmp, &ch, 1) ? tmp : WEOF;

#else

    const std::wint_t wc = WEOF;

#endif   // _RWSTD_NO_BTOWC, _RWSTD_NO_MBSTOWCS

    const bool result =
        WEOF == wc || libc_is (mask, ch, 0) || !libc_is (mask, wchar_t (wc), 0);

    std::setlocale (LC_CTYPE, curlocname);

    return result;
}


// cond3() overloads verify condition [3] in Test::test_narrow_widen()
// below using libc functions
bool cond3 (std::ctype_base::mask, char, const char*)
{
    return true;
}


#ifndef _RWSTD_NO_WCHAR_T

int libc_isalpha (wchar_t ch)
{
    return (std::iswalpha)(ch);
}

int libc_isspace (wchar_t ch)
{
    return (std::iswspace)(ch);
}

int libc_isprint (wchar_t ch)
{
    return (std::iswprint)(ch);
}

int libc_iscntrl (wchar_t ch)
{
    return (std::iswcntrl)(ch);
}

int libc_isupper (wchar_t ch)
{
    return (std::iswupper)(ch);
}

int libc_islower (wchar_t ch)
{
    return (std::iswlower)(ch);
}

int libc_isdigit (wchar_t ch)
{
    return (std::iswdigit)(ch);
}

int libc_ispunct (wchar_t ch)
{
    return (std::iswpunct)(ch);
}

int libc_isxdigit (wchar_t ch)
{
    return (std::iswxdigit)(ch);
}

int libc_isalnum (wchar_t ch)
{
    return (std::iswalnum)(ch);
}

int libc_isgraph (wchar_t ch)
{
    return (std::iswgraph)(ch);
}

wchar_t libc_tolower (wchar_t ch)
{
    return (std::towlower)(ch);
}

wchar_t libc_toupper (wchar_t ch)
{
    return (std::towupper)(ch);
}

std::size_t c_strlen (const wchar_t *s1)
{
    return std::wcslen (s1);
}

int c_strcmp (const wchar_t *s1, const wchar_t *s2)
{
    int ret = std::wcscmp (s1, s2);
    return ret ? ret > 0 ? 1 : -1 : 0;
}


const char* narrow (char *dst, const wchar_t *src)
{
    static char buf [4096];

    if (!src)
        return 0;

    if (!dst)
        dst = buf;

    std::size_t len = std::wcslen (src);

    _RWSTD_ASSERT (len < sizeof buf);

    len = std::wcstombs (dst, src, sizeof buf / sizeof *buf);

    if (std::size_t (-1) == len)
        *dst = 0;

    return dst;
}


const wchar_t* widen (wchar_t *dst, const char *src)
{
    static wchar_t buf [4096];

    if (!src)
        return 0;

    if (!dst)
        dst = buf;

    std::size_t len = std::strlen (src);

    _RWSTD_ASSERT (len < sizeof buf /sizeof *buf);

    len = std::mbstowcs (dst, src, sizeof buf / sizeof *buf);

    if (std::size_t (-1) == len)
        *dst = 0;

    return dst;
}

std::ctype_base::mask libc_mask (int mask, wchar_t ch, const char *locname)
{
    char curlocname [256];

    if (locname) {
        std::strcpy (curlocname, std::setlocale (LC_CTYPE, 0));

        if (0 == std::setlocale (LC_CTYPE, locname))
            return std::ctype_base::mask ();
    }

    int result = 0;

    if (mask & ALPHA && (std::iswalpha)(ch))
        result |= ALPHA;
    if (mask & CNTRL && (std::iswcntrl)(ch))
        result |= CNTRL;
    if (mask & DIGIT && (std::iswdigit)(ch))
        result |= DIGIT;
    if (mask & GRAPH && (std::iswgraph)(ch))
        result |= GRAPH;
    if (mask & LOWER && (std::iswlower)(ch))
        result |= LOWER;
    if (mask & PRINT && (std::iswprint)(ch))
        result |= PRINT;
    if (mask & PUNCT && (std::iswpunct)(ch))
        result |= PUNCT;
    if (mask & SPACE && (std::iswspace)(ch))
        result |= SPACE;
    if (mask & UPPER && (std::iswupper)(ch))
        result |= UPPER;
    if (mask & XDIGIT && (std::iswxdigit)(ch))
        result |= XDIGIT;

    if (locname)
        std::setlocale (LC_CTYPE, curlocname);

    return std::ctype_base::mask (result);
}

bool libc_is (std::ctype_base::mask mask, wchar_t ch, const char *locname)
{
    const std::ctype_base::mask m = libc_mask (mask, ch, locname);

    return 0 != (m & mask);
}

wchar_t widen (wchar_t, char ch, const char *locname)
{
    char curlocname [256];
    std::strcpy (curlocname, std::setlocale (LC_CTYPE, 0));

    if (0 == std::setlocale (LC_CTYPE, locname))
        return UChar (ch);

    wchar_t result;

#ifndef _RWSTD_NO_BTOWC

    result = std::btowc (UChar (ch));

#elif !defined (_RWSTD_NO_MBTOWC)

    if (1 != std::mbtowc (&result, &ch, 1))
        result = wchar_t (WEOF);

#else

    result = UChar (ch);

#endif   // _RWSTD_NO_BTOWC, _RWSTD_NO_MBTOWC

    if (locname)
        std::setlocale (LC_CTYPE, curlocname);

    return result;
}

char narrow (wchar_t ch, const char *locname)
{
    char curlocname [256];
    std::strcpy (curlocname, std::setlocale (LC_CTYPE, 0));

    if (0 == std::setlocale (LC_CTYPE, locname))
        return UChar (ch);

    char result [MB_LEN_MAX];

#ifndef _RWSTD_NO_WCTOB

    result [0] = std::wctob (ch);

#elif !defined (_RWSTD_NO_WCTOMB)

    if (1 != std::wctomb (result, ch))
        result [0] = '\377';

#else

    result [0] = char (ch);

#endif   // _RWSTD_NO_WCTOB, _RWSTD_NO_WCTOMB

    if (locname)
        std::setlocale (LC_CTYPE, curlocname);

    return result [0];
}

bool cond3 (std::ctype_base::mask mask, wchar_t ch, const char *locname)
{
    char curlocname [256];
    std::strcpy (curlocname, std::setlocale (LC_CTYPE, 0));

    if (0 == std::setlocale (LC_CTYPE, locname))
        return false;

#ifndef _RWSTD_NO_WCTOB

    const int byte = std::wctob (ch);

#elif !defined (_RWSTD_NO_WCTOMB)

    char buf [MB_LEN_MAX];
    const int byte = 1 == std::wctomb (buf, ch) ? buf [0] : EOF;

#else

    const int byte = EOF;

#endif   // _RWSTD_NO_WCTOB, _RWSTD_NO_WCTOMB

    const bool result =
        EOF == byte || !libc_is (mask, char (byte), 0) || libc_is (mask, ch, 0);

    std::setlocale (LC_CTYPE, curlocname);

    return result;
}

#endif   // _RWSTD_NO_WCHAR_T


template <class charT>
void gen_str (charT *str, std::size_t size)
{
    // generate a random string with the given size
    // we do not attempt to check that the size is within the
    // valid range for the string.

    for (std::size_t i = 0; i < size; i++){
        str [i] = UChar (std::rand () % UCHAR_MAX);
        // only increment if we are not going to roll over
        if (str [i] != charT (UCHAR_MAX - 1U))
            ++str [i];
    }

    str [size] = charT ();

    _RWSTD_ASSERT (c_strlen (str) == size);
}

/**************************************************************************/

template <class charT>
void test_is (charT, const char *cname)
{
    charT str [MAX_STR_SIZE + 1];

    BEGIN_LOCALE_LOOP (NLOOPS, locname, j) {

        std::size_t size = std::size_t (j % MAX_STR_SIZE);

        gen_str (str, size);

        // create a mask for each character in the string using the c library
        // to make sure that it is the same as the mask returned by is
        std::ctype_base::mask vec [MAX_STR_SIZE + 1];

        // set the global C locale to default to make sure
        // the C++ library does not asume a set value
        std::setlocale (LC_CTYPE, "");

        ctp.is (str, str + size, vec);

        // set the global C locale to the current locale for calls to the
        // C library
        std::setlocale (LC_CTYPE, locname);

        for (std::size_t i = 0; i < size ; i++) {

            int libc_result = 0;

            if (libc_isalpha (str [i]))
                libc_result |= ALPHA;
            if (libc_isprint (str [i]))
                libc_result |= PRINT;
            if (libc_isspace (str [i]))
                libc_result |= SPACE;
            if (libc_iscntrl (str [i]))
                libc_result |= CNTRL;
            if (libc_isupper (str [i]))
                libc_result |= UPPER;
            if (libc_islower (str [i]))
                libc_result |= LOWER;
            if (libc_isdigit (str [i]))
                libc_result |= DIGIT;
            if (libc_ispunct (str [i]))
                libc_result |= PUNCT;
            if (libc_isxdigit (str [i]))
                libc_result |= XDIGIT;
            if (libc_isgraph (str [i]))
                libc_result |= GRAPH;

            rw_assert (vec [i] == libc_result, 0, __LINE__,
                       "ctype<%s>::is(%{*Ac}, ..., v) in locale(%#s) "
                       "at offset %zu; got %{LC} expected %{LC}\n",
                       cname, int (sizeof *str), str,
                       locname, i, vec [i], libc_result);
        }
    } END_LOCALE_LOOP (locname);
}

/**************************************************************************/

template <class charT>
void test_toupper_tolower (charT, const char *cname)
{
    rw_info (0, 0, __LINE__, "std::ctype<%s>::tolower(%1$s)", cname);

    BEGIN_LOCALE_LOOP (UCHAR_MAX, locname, i) {

        const charT ch = charT (i);

        // set the global C locale to locname for the C library call
        std::setlocale (LC_CTYPE, locname);

        const charT lch = libc_tolower (ch);

        // set the global C locale to default to make sure
        // the C++ library does not asume a set value
        std::setlocale (LC_CTYPE, "");

        // exercise tolower using ctype<>::tolower ()
        rw_assert (lch == (ctp.tolower)(ch), 0, __LINE__,
                   "ctype<%s>::tolower(%{#lc}) == %{#lc}, got %{#lc} "
                   "in locale(%#s)",
                   cname, ch, lch, (ctp.tolower)(ch), locname);

        // exercise tolower using tolower (char, locale)
#if DRQS // Missing convenience functions [lib.locales]
        rw_assert (lch == (charT)(std::tolower)(ch, loc), 0, __LINE__,
                   "tolower<%s>(%{#lc}, locale(%#s)) == %{#lc}, got %{#lc}",
                   cname, ch, locname, lch, (std::tolower)(ch, loc));
#else
        rw_assert (lch == (charT)(std::tolower)(ch /*, loc */), 0, __LINE__,
                   "tolower<%s>(%{#lc}, locale(%#s)) == %{#lc}, got %{#lc}",
                   cname, ch, locname, lch, (std::tolower)(ch /*, loc */));
#endif
    } END_LOCALE_LOOP (locname);

    rw_info (0, 0, __LINE__, "std::ctype<%s>::toupper(%1$s)", cname);

    BEGIN_LOCALE_LOOP (UCHAR_MAX, locname2, i) {

        const charT ch = charT (i);

        // set the global C locale to the locale name for the C library call
        std::setlocale (LC_CTYPE, locname2);

        const charT uch = libc_toupper (ch);

        // set the global C locale to default to make sure
        // the C++ library does not asume a set value
        std::setlocale (LC_CTYPE, "");

        // exercise toupper using ctype<>::toupper ()
        rw_assert (uch == (ctp.toupper)(ch), 0, __LINE__,
                   "ctype<%s>::toupper(%{#lc}) == %{#lc}, got %{#lc} "
                   "in locale(%#s)",
                   cname, ch, uch, (ctp.toupper)(ch), locname2);

        // exercise toupper using toupper (char, locale)
#if DRQS // missing convenience functions [lib.locales]
        rw_assert (uch == (charT)(std::toupper)(ch, loc), 0, __LINE__,
                   "toupper<%s>(%{#lc}, locale(%#s)) == %{#lc}, got %{#lc}",
                   cname, ch, locname2, uch, (std::toupper)(ch, loc));
#else
        rw_assert (uch == (charT)(std::toupper)(ch /*, loc */), 0, __LINE__,
                   "toupper<%s>(%{#lc}, locale(%#s)) == %{#lc}, got %{#lc}",
                   cname, ch, locname2, uch, (std::toupper)(ch /*, loc */));
#endif
    } END_LOCALE_LOOP (locname2);
}

/**************************************************************************/

template <class charT>
void test_scan (charT, const char *cname)
{
    rw_info (0, 0, __LINE__, "std::ctype<%s>::scan_is(), scan_not()", cname);

    charT str [MAX_STR_SIZE + 1];

    BEGIN_LOCALE_LOOP (NLOOPS, locname, j) {

        const std::size_t size = std::size_t (j % MAX_STR_SIZE);

        // generate a random string
        gen_str (str, size);

        // set the global C locale to default to make sure
        // the C++ library does not asume a set value
        std::setlocale (LC_CTYPE, "");

        // call scan_is and scan_not using each mask and compare it to
        // the results found when using the c library

        const charT* alpha_is   = ctp.scan_is  (ALPHA,  str, str + size);
        const charT* alpha_not  = ctp.scan_not (ALPHA,  str, str + size);
        const charT* space_is   = ctp.scan_is  (SPACE,  str, str + size);
        const charT* space_not  = ctp.scan_not (SPACE,  str, str + size);
        const charT* print_is   = ctp.scan_is  (PRINT,  str, str + size);
        const charT* print_not  = ctp.scan_not (PRINT,  str, str + size);
        const charT* cntrl_is   = ctp.scan_is  (CNTRL,  str, str + size);
        const charT* cntrl_not  = ctp.scan_not (CNTRL,  str, str + size);
        const charT* upper_is   = ctp.scan_is  (UPPER,  str, str + size);
        const charT* upper_not  = ctp.scan_not (UPPER,  str, str + size);
        const charT* lower_is   = ctp.scan_is  (LOWER,  str, str + size);
        const charT* lower_not  = ctp.scan_not (LOWER,  str, str + size);
        const charT* digit_is   = ctp.scan_is  (DIGIT,  str, str + size);
        const charT* digit_not  = ctp.scan_not (DIGIT,  str, str + size);
        const charT* punct_is   = ctp.scan_is  (PUNCT,  str, str + size);
        const charT* punct_not  = ctp.scan_not (PUNCT,  str, str + size);
        const charT* xdigit_is  = ctp.scan_is  (XDIGIT, str, str + size);
        const charT* xdigit_not = ctp.scan_not (XDIGIT, str, str + size);

        // set the global C locale to locname for the C library call
        std::setlocale (LC_CTYPE, locname);

        // find the first character in the string that is of the specified
        // type
        charT first = 0;
        charT first_not = 0;
        _RWSTD_SIZE_T i;
        int success;

#define SCAN(what)                                                      \
        for (i = 0; i < size; i++) {                                    \
            if (libc_is##what (str [i])) {                              \
                first = (str [i]);                                      \
                break;                                                  \
            }                                                           \
        }                                                               \
        success =    what##_is != (str + size) || 0 == first            \
                  || *what##_is != first;                               \
        rw_assert (success, 0, __LINE__,                                \
                   "ctype<%s>::scan_is(" #what ", %{#lc}) "             \
                   "== %{*Ac}, got %{*Ac} in locale (%#s)",             \
                   cname, int (sizeof *str), first,                     \
                   int (sizeof *what##_is), what##_is,                  \
                   locname);                                            \
                                                                        \
        first = 0;                                                      \
        for (i = 0; i < size; i++) {                                    \
            if (!libc_is##what (str[i])) {                              \
                first_not = (str[i]);                                   \
                break;                                                  \
            }                                                           \
        }                                                               \
        success =    what##_not != (str + size) || 0 == first_not       \
                  || *what##_not == first_not;                          \
        rw_assert (success, 0, __LINE__,                                \
                   "ctype<%s>::scan_not(" #what ", %{#lc}) "            \
                   "== %{*Ac}, got %{*Ac} in locale (%#s)",             \
                   cname, int (sizeof *str), first,                     \
                   int (sizeof *what##_not), what##_not,                \
                   locname);                                            \
                                                                        \
        first_not = 0

        // test all classes of characters
        SCAN (alpha);
        SCAN (space);
        SCAN (print);
        SCAN (cntrl);
        SCAN (upper);
        SCAN (lower);
        SCAN (digit);
        SCAN (punct);
        SCAN (xdigit);

    } END_LOCALE_LOOP (locname);
}

/**************************************************************************/

template <class charT>
void test_narrow_widen (charT, const char *cname)
{
    // 22.2.1.1.2, p11 requires that the conditions below hold for all
    // facets ctc and ct whose types are ctype<char> and ctype<charT>,
    // respectively:

    // [1] (ctc.is (M, c) || !ct.is (M, ctc.do_widen (c))) holds for
    //     all narrow characters c
    //     i.e., narrow characters that are NOT members of a certain
    //     category may not belong to the same category when widened
    //     Note: this implies that some sort of code conversion may
    //     be necessary in order to implement a conforming do_widen()

    // 22.2.1.1.2, p13 requires that:
    // [2] (ct.do_widen (ct.do_narrow (c, dfault)) == c) holds unless
    //     (ct.do_narrow (c, dfault) == dfault) holds
    // [3] (ct.is (M, c) || !ctc.is (M, ct.do_narrow (c, dfault))) holds
    //     unless (ct.do_narrow(c, dfault) == dfault) holds
    //
    //     C99: each of the iswxxx() functions returns true for each
    //     wide character that corresponds (as if by a call to the
    //     wctob() function) to a single-byte character for which the
    //     corresponding character classification function from 7.4.1
    //     returns true, except that the iswgraph() and iswpunct()
    //     functions may differ with respect to wide characters other
    //     than L' ' that are both printing and white-space wide
    //     characters.
    //
    // [4] (ct.do_narrow (c, default) - '0') evaluates to the digit
    //     value of the character for all c for which ct.is(digit, c)
    //     returns true

    rw_info (0, 0, __LINE__,
             "std::ctype<%s>::narrow(%1$s), widen(char)",
             cname);

    rw_info (0, 0, __LINE__,
             "std::ctype<%s>::narrow(const %1$s*, const %1$s*, char*), "
             "widen(const char*, const char*, %1$s*)", cname);

#define STR(x) #x

    // verify condition [1] above; if it fails, verify that
    // the same condition also fails to hold when using the
    // corresponding libc functions
#define COND1(what)                                             \
  if (!(ctc.is (what, c) || !ctp.is (what, ctp.widen (c)))) {   \
      rw_assert (!cond1 (what, c, locname), 0, __LINE__,        \
                 "ctype<char>::is (" STR (what) ", %{#lc})"     \
                 " || !ctype<%1$s>::is (" STR (what) ", "       \
                 "ctype<%s>::widen (%{#lc}) = %{#lc})"          \
                 " returned false in locale(%#s)",              \
                 c, cname, c, ctp.widen (c), locname);          \
  } else (void)0

    // verify condition [3] above; if it fails, verify that
    // the same condition also fails to hold when using the
    // corresponding libc functions
#define COND3(what)                                                         \
  if (   ctp.narrow (ch, dfault) != dfault                                  \
      && !(ctp.is (what, ch) || !ctc.is (what, ctp.narrow (ch, dfault)))) { \
      rw_assert (!cond3 (what, ch, locname), 0, __LINE__,                   \
                 "ctype<%s>::is (" STR (what) ", %{#lc})"                   \
                 " || !ctype<char>::is (" STR (what) ", "                   \
                 "ctype<%1$s>::narrow (%{#lc}, %{#c}) = %{#lc})"            \
                 " returned false in locale(%#s)", cname,                   \
                 ch, cname, ch, dfault, ctp.narrow (ch, '\0'),              \
                 locname);                                                  \
  } else (void)0


    char c_locname [256];
    std::strcpy (c_locname, std::setlocale (LC_ALL, 0));

    BEGIN_LOCALE_LOOP (UCHAR_MAX, locname, i) {

#if defined (_RWSTD_OS_SUNOS) && _RWSTD_OS_MAJOR == 5 && _RWSTD_OS_MINOR <= 10

        // avoid a libc SIGSEGV in mbtowc() in zh_HK and zh_TW
        // locales encoded using the BIG5 codeset (see bug #603)
        if (   0 == std::strncmp ("zh_HK.BIG5", locname, 10)
            || 0 == std::strncmp ("zh_TW.BIG5", locname, 10))
            continue;

#endif   // SunOS < 5.10

        {
            // verify that the global C locale stays unchanged
            const char* const curlocname = std::setlocale (LC_ALL, 0);

            rw_assert (!std::strcmp (c_locname, curlocname), 0, __LINE__,
                       "setlocale(LC_ALL, 0) == \"%s\", got \"%s\"",
                       c_locname, curlocname);
        }

        const char  c  = char (i);
        const charT ch = charT (i);

        // verify that condition [1] holds
        COND1 (ALPHA);
        COND1 (CNTRL);
        COND1 (DIGIT);
        COND1 (GRAPH);
        COND1 (LOWER);
        COND1 (PRINT);
        COND1 (PUNCT);
        COND1 (SPACE);
        COND1 (UPPER);
        COND1 (XDIGIT);

        // verify that condition [2] holds
        char dfault = c ? '\0' : '\1';
        const charT ret = ctp.widen (ctp.narrow (ch, dfault));

        if (ret != charT (dfault) && ret != ch) {
            rw_assert (ch != widen (ch, narrow (ch, locname), locname),
                       0, __LINE__,
                       "ctype<%s>::widen (ctype<%1$s>::narrow "
                       "(%{#lc}, %{#c})) == %{#c}; got %{#c} "
                       "in locale (%#s)",
                       cname, ch, dfault, ch, ret, locname);
        }

        // finally verify that condition [3] holds
        COND3 (ALPHA);
        COND3 (CNTRL);
        COND3 (DIGIT);
        COND3 (GRAPH);
        COND3 (LOWER);
        COND3 (PRINT);
        COND3 (PUNCT);
        COND3 (SPACE);
        COND3 (UPPER);
        COND3 (XDIGIT);

        // now perform a relitively simple sanity check on the 3-argument
        // overloads of narrow() and widen(). Make sure that the 3-argument
        // overloads return the same value that the other overload produces
        // Only do this the first time through the locale list.

        if (i == 0) {
            // arrays of all tested narrow and wide characters
            charT wide_chars   [UCHAR_MAX + 1];
            char  narrow_chars [UCHAR_MAX + 1];
            charT narrow_in    [UCHAR_MAX + 1];
            char  widen_in     [UCHAR_MAX + 1];

            // zero out the last element to allow printing
            wide_chars   [UCHAR_MAX] = charT ();
            narrow_chars [UCHAR_MAX] = char ();
            narrow_in    [UCHAR_MAX] = charT ();
            widen_in     [UCHAR_MAX] = char ();

            // set the `dfault' character to something unlikely
            // but other than '\0'
            dfault = '\377';

            for (unsigned j = 0; j <= UCHAR_MAX; j++) {
                wide_chars [j]   = ctp.widen (char (j));
                narrow_chars [j] = ctp.narrow (wide_chars [j], dfault);
                narrow_in [j]    = ctp.widen (char (j));
                widen_in [j]     = char (j);
            }

            charT widen_out [UCHAR_MAX + 1];
            char narrow_out [UCHAR_MAX + 1];

            widen_out  [UCHAR_MAX] = charT ();
            narrow_out [UCHAR_MAX] = char ();

            // narrow source buffer into the destination
            // and compare with expected values
            ctp.narrow (narrow_in,
                        narrow_in + UCHAR_MAX + 1,
                        dfault,
                        narrow_out);

            bool success =
                !std::memcmp (narrow_chars, narrow_out, sizeof narrow_chars);

            rw_assert (success, 0, __LINE__,
                       "ctype<%s>::narrow (%{*.*Ac}\", ... , %#c) "
                       "== %{.*Ac}, got %{.Ac} in locale (%#s)", cname,
                       int (sizeof *narrow_in), UCHAR_MAX, narrow_in, dfault,
                       UCHAR_MAX, narrow_chars, UCHAR_MAX, narrow_out,
                       locname);

            // widen source buffer into the destination
            // and compare with expected values
            ctp.widen (widen_in,
                       widen_in + UCHAR_MAX + 1,
                       widen_out);

            success = !std::memcmp (wide_chars, widen_out, sizeof wide_chars);

            rw_assert (success, 0, __LINE__,
                       "ctype<%s>::widen (%{.*Ac}, ...) == "
                       "%{*.*Ac}, got %{*.*Ac} in locale (%#s)",
                       cname, UCHAR_MAX, widen_in,
                       int (sizeof *wide_chars), UCHAR_MAX, wide_chars,
                       int (sizeof *wide_chars), UCHAR_MAX, widen_out,
                       locname);
        }

    } END_LOCALE_LOOP (locname);
}

/**************************************************************************/

template <class charT>
void test_is_ch (charT, const char *cname)
{
    // buffers to hold the character classification
    // e.g., for `a' in the "C" locale the string will be
    //       "print lower alpha xdigit alnum graph "

    static char is_C    [80];  // C character classification
    static char is_CXX  [80];  // C++ classification using ctype<char> facet
    static char is_CXX2 [80];  // C++ classification using convenience funcs

    rw_info (0, 0, __LINE__, "std::ctype<%s>::is(mask, %1$s)", cname);

    BEGIN_LOCALE_LOOP (UCHAR_MAX, locname, i) {

        const charT ch = charT (i);
        const UChar uch = ch;

        *is_C    =
        *is_CXX  =
        *is_CXX2 = '\0';

        // set the global C locale to locname for the C library calls
        std::setlocale (LC_CTYPE, locname);

#define IS(what)                                                        \
        libc_is##what (ch)                                              \
             ? (void)std::strcat (is_C,    #what" ") : (void)0;

        // test all classes of characters
        IS (space); IS (print); IS (cntrl); IS (upper); IS (lower);
        IS (alpha); IS (digit); IS (punct); IS (xdigit); IS (alnum);
        IS (graph);

        // set the global C locale to default to make sure
        // the C++ library does not asume a set value
        std::setlocale (LC_CTYPE, "");

#undef IS
// convenience macro
#if DRQS // stdlib is supposed to provide convenience interfaces of the form:
         // template <class charT> std::isspace(CharT, const locale&);
	 // See section 22.1 [lib.locales] of the standard.
#define IS(what)                                                   \
    ctp.is (std::ctype_base::what, ch)                             \
          ? (void)std::strcat (is_CXX,  #what" ") : (void)0;       \
    (std::is##what)(ch, loc)                                       \
          ? (void)std::strcat (is_CXX2, #what" ") : (void)0;
#else
// Remove loc argument
#define IS(what)                                                   \
    ctp.is (std::ctype_base::what, ch)                             \
          ? (void)std::strcat (is_CXX,  #what" ") : (void)0;       \
    (std::is##what)(ch /*, loc */)                                       \
          ? (void)std::strcat (is_CXX2, #what" ") : (void)0;
#endif

        // test all classes of characters
        // (must be in the same order as the IS() calls above)
        IS (space); IS (print); IS (cntrl); IS (upper); IS (lower);
        IS (alpha); IS (digit); IS (punct); IS (xdigit); IS (alnum);
        IS (graph);

        // compare the two strings (should match)

        rw_assert (0 == std::strcmp (is_C, is_CXX), 0, __LINE__,
                   "ctype<%s>::is(..., %{#lc}) in locale(%#s) "
                   "expected to hold for { %s}, got { %s}",
                   cname, uch, locname, is_C, is_CXX);

        // convenience functions must produce the same results
        rw_assert (0 == std::strcmp (is_C, is_CXX2), 0, __LINE__,
                   "is* (%{#lc}, locale (%#s)) expected to hold for "
                   "{ %s}, got { %s}",
                   uch, locname, is_C, is_CXX2);

#if !defined (_WIN32) && !defined (_WIN64)

        // exercise POSIX requirements only on POSIX platforms

        static const std::ctype_base::mask masks[] = {
            ALPHA, std::ctype_base::mask (), CNTRL, DIGIT,
            GRAPH, LOWER, PRINT, PUNCT,
            SPACE, UPPER, XDIGIT
        };

        // see the POSIX description of LC_CTYPE in Locale for a table
        // of required and allowed combinations of character classes

        // a character in a given /* class */ below is also required
        // to be included in the following set of classes
        static const int required [sizeof masks / sizeof *masks] = {
            /* alpha  */ ALPHA | GRAPH | PRINT,
            /* blank  */ SPACE,
            /* cntrl  */ CNTRL,
            /* digit  */ DIGIT | GRAPH | PRINT | XDIGIT,
            /* graph  */ GRAPH | PRINT,
            /* lower  */ ALPHA | GRAPH | LOWER | PRINT,
            /* print  */ PRINT,
            /* punct  */ GRAPH | PRINT | PUNCT,
            /* space  */ SPACE,
            /* upper  */ ALPHA | GRAPH | PRINT | UPPER,
            /* xdigit */ GRAPH | PRINT | XDIGIT
        };

        // a character in a given /* class */ below may also
        // be included in the following set of classes
        static const int allowed [sizeof masks / sizeof *masks] = {
            /* alpha  */ UPPER | LOWER | XDIGIT,
            /* blank  */ CNTRL,
            /* cntrl  */ SPACE,
            /* digit  */ 0,
            /* graph  */ UPPER | LOWER | ALPHA | DIGIT | SPACE | PUNCT |XDIGIT,
            /* lower  */ UPPER | XDIGIT,
            /* print  */ UPPER | LOWER | ALPHA | DIGIT | SPACE | PUNCT |XDIGIT,
            /* punct  */ SPACE,
            /* space  */ CNTRL,
            /* upper  */ LOWER | XDIGIT,
            /* xdigit */ UPPER | LOWER | ALPHA | DIGIT
        };

        // a character in a given /* class */ below is explicitly
        // disallowed to be included in the following set of classes
        static const int disallowed [sizeof masks / sizeof *masks] = {
            /* alpha  */ DIGIT | SPACE | CNTRL | PUNCT,
            /* blank  */ UPPER | LOWER | ALPHA | DIGIT,
            /* cntrl  */ UPPER | LOWER | ALPHA | DIGIT | PUNCT | GRAPH | PRINT,
            /* digit  */ UPPER | LOWER | ALPHA | SPACE | SPACE | CNTRL,
            /* graph  */ CNTRL,
            /* lower  */ DIGIT | SPACE | CNTRL | PUNCT,
            /* print  */ CNTRL,
            /* punct  */ UPPER | LOWER | ALPHA | DIGIT | CNTRL | XDIGIT,
            /* space  */ UPPER | LOWER | ALPHA | DIGIT | XDIGIT,
            /* upper  */ DIGIT | SPACE | CNTRL | PUNCT,
            /* xdigit */ SPACE | CNTRL | PUNCT
        };

        // obtain the mask of a single character
        std::ctype_base::mask m;
        ctp.is (&ch, &ch + 1, &m);

        int missing        = 0;   // required bits missing in ch's mask
        int all_allowed    = 0;   // all bits allowed to be set in ch's mask
        int all_disallowed = 0;   // all bits disallowed to be set in the mask

        for (std::size_t k = 0; k != sizeof masks / sizeof *masks; ++k) {

            // assumes masks [k] has a single bit set
            const int bit = m & masks [k];
            if (!bit)
                continue;

            if ((bit & required [k]) && (m & required [k]) != required [k]) {
                // character in a class given by masks[k] is required
                // to also belong to all classes in required[k]

                missing |= ~(m & required [k]) & required [k];
            }

            // character in a class given by masks[k] is only allowed
            // to belong to classes in (allowed[k] | required[k]) and
            // is not allowed to belong to those in disallowed[k]
            all_allowed    |= required [k] | allowed [k];
            all_disallowed |= disallowed [k];
        }

        // the space character automatically belongs to the print class
        // but cannot belong to the punct or graph classes; other characters
        // that belong to the space class can belong to the punct and graph
        // classes
        if (' ' == ch) {
            all_disallowed |= PUNCT | GRAPH;
            if (!(m & PRINT))
                missing |= PRINT;
        }

        int cmask = -1;

        if (missing) {

            if (-1 == cmask)
                cmask = libc_mask (-1, ch, locname);
#if DRQS
    // 'libc_mask' returns a ctype mask that matches all possible
    // classifications, but '_S_classic_table' in ctype.cpp does not do this.
    // For example, '!' is missing the bits for 'graph' (graphical
    // representation), even though isgraph('!') returns non-zero.

            rw_assert (m == cmask, 0, __LINE__,
                       "mask of %{#lc} in locale(%#s) "
                       "is missing bits %{LC}: %{LC}",
                       ch, locname, missing, m);
#endif
        }

        if (m & ~all_allowed) {

            if (-1 == cmask)
                cmask = libc_mask (-1, ch, locname);

            rw_assert (m == cmask, 0, __LINE__,
                       "mask of %{#lc} in locale (%#s) "
                       "contains extra bits %{LC}: %{LC}",
                       ch, locname, m & ~all_allowed, m);
        }

        if (m & all_disallowed) {

            if (-1 == cmask)
                cmask = libc_mask (-1, ch, locname);

            rw_assert (m == cmask, 0, __LINE__,
                       "mask of %{#lc} in locale (%s#) "
                       "contains disallowed bits { %s }: { %s }",
                       ch, locname, m & all_disallowed, m);
        }

#endif   // !WIN32 && !WIN64

    } END_LOCALE_LOOP (locname);
}

/**************************************************************************/

template <class charT>
void test_libc (charT, const char *cname)
{
    test_is_ch (charT (), cname);
    test_toupper_tolower (charT (), cname);
    test_narrow_widen (charT (), cname);
    test_is (charT (), cname);
    test_scan (charT (), cname);
}

/**************************************************************************/

template <class charT>
void test_libstd_scan_is (charT, const char *cname,
                          const std::ctype<charT> &ct,
                          const char* str, std::ctype_base::mask mask,
                          int expected_idx, int fwiden = 0)
{
    // convert narrow string to a (possibly) wide representation
    charT  wstrbuf [256];
    charT* wstr = wstrbuf;

    // If instructed to use the facet widen method, do so, otherwise
    // use widen helper function that in turn uses C lib mbstowcs
    if (fwiden == 0)
        widen (wstrbuf, str);
    else
        ct.widen (str, str + std::strlen (str), wstrbuf);

    const int success =
        &wstr [expected_idx] == ct.scan_is (mask, wstr, wstr + c_strlen (wstr));

    rw_assert (success, 0, __LINE__,
               "ctype<%s>::scan_is() returned an unexpected value",
               cname);

}

/**************************************************************************/

template <class charT>
void test_libstd_scan_not (charT, const char *cname,
                           const std::ctype<charT> &ct,
                           const char* str, std::ctype_base::mask mask,
                           int expected_idx, int fwiden = 0)
{
    // convert narrow string to a (possibly) wide representation
    charT  wstrbuf [256];
    charT* wstr = wstrbuf;

    // If instructed to use the facet widen method, do so, otherwise
    // use widen helper function that in turn uses C lib mbstowcs
    if (fwiden == 0)
        widen (wstrbuf, str);
    else
        ct.widen (str, str + std::strlen (str), wstrbuf);

    const int success =
        &wstr[expected_idx] == ct.scan_not (mask, wstr, wstr + c_strlen (wstr));

    rw_assert (success, 0, __LINE__,
               "ctype<%s>::scan_not() returned an unexpected value",
               cname);

}

/**************************************************************************/

template <class charT>
void test_libstd_toupper_tolower (charT, const char *cname,
                                  const std::ctype<charT> &ct,
                                  const char *locname)
{
    rw_info (0, 0, __LINE__,
             "std::ctype<%s>::tolower(%1$s) toupper(%1$s) in locale(%#s)",
             cname, locname);

    int success;

#undef TEST
#define TEST(lower, upper)                                              \
    success = ct.widen (upper) == ct.toupper (ct.widen (lower));        \
    rw_assert (success, 0, __LINE__,                                    \
               "ctype<%s>::toupper(%d) == %d, got %d", cname,           \
               lower, upper, ct.toupper((charT)lower));                 \
    success = ct.widen (lower) == ct.tolower (ct.widen (upper));        \
    rw_assert (success, 0, __LINE__,                                    \
               "ctype<%s>::tolower(%d) == %d, got %d",                  \
               cname, upper, lower, ct.tolower((charT)upper))

    TEST ('a', 'A');
    TEST ('b', 'B');
    TEST ('c', 'C');

    if (sizeof(charT) > 1) {
        TEST ('\xa0', '\xa1');
    }

#undef TEST
}

/**************************************************************************/

template <class charT>
void test_libstd_mask (charT, const char *cname,
                       const std::ctype<charT> &ct, const char *locname)
{

    rw_info (0, 0, __LINE__, "std::ctype<%s>::is(mask, %1$s) in locale(%#s)",
             cname, locname);

#undef TEST
#if TEST_RW_EXTENTENSIONS
    // RW assumes that std::ctype_base::mask is int
#define TEST(ch, m)                                             \
    rw_assert (ct.is (m, ch), 0, __LINE__,                      \
               "ctype<%s>::is(%d, %d) failed", cname, m, ch)
#else
#define TEST(ch, m)                                             \
    rw_assert (ct.is (std::ctype_base::mask(m), ch), 0, __LINE__, \
               "ctype<%s>::is(%d, %d) failed", cname, m, ch)
#endif

    // make sure the characters have the correct masks
    TEST (charT ('a'), ALPHA);
    TEST (charT ('a'), LOWER);
    TEST (charT ('a'), XDIGIT);
    TEST (charT ('a'), GRAPH);
    TEST (charT ('a'), PRINT);
    TEST (charT ('a'), ALPHA | LOWER | XDIGIT | PRINT);

    TEST (charT ('b'), ALPHA);
    TEST (charT ('b'), LOWER);
    TEST (charT ('b'), XDIGIT);
    TEST (charT ('b'), GRAPH);
    TEST (charT ('b'), PRINT);
    TEST (charT ('b'), ALPHA | LOWER | XDIGIT | PRINT);

    TEST (charT ('c'), ALPHA);
    TEST (charT ('c'), LOWER);
    TEST (charT ('c'), XDIGIT);
    TEST (charT ('c'), GRAPH);
    TEST (charT ('c'), PRINT);
    TEST (charT ('c'), ALPHA | LOWER | XDIGIT | PRINT);

    TEST (charT ('A'), ALPHA);
    TEST (charT ('A'), UPPER);
    TEST (charT ('A'), XDIGIT);
    TEST (charT ('A'), GRAPH);
    TEST (charT ('A'), PRINT);
    TEST (charT ('A'), ALPHA | UPPER | XDIGIT | PRINT);

    TEST (charT ('B'), ALPHA);
    TEST (charT ('B'), UPPER);
    TEST (charT ('B'), XDIGIT);
    TEST (charT ('B'), GRAPH);
    TEST (charT ('B'), PRINT);
    TEST (charT ('B'), ALPHA | UPPER | XDIGIT | PRINT);

    TEST (charT ('C'), ALPHA);
    TEST (charT ('C'), UPPER);
    TEST (charT ('C'), XDIGIT);
    TEST (charT ('C'), GRAPH);
    TEST (charT ('C'), PRINT);
    TEST (charT ('C'), ALPHA | UPPER | XDIGIT | PRINT);

    TEST (charT ('1'), DIGIT);
    TEST (charT ('1'), XDIGIT);
    TEST (charT ('1'), GRAPH);
    TEST (charT ('1'), PRINT);
    TEST (charT ('1'), DIGIT | XDIGIT | GRAPH | PRINT);

    TEST (charT ('2'), DIGIT);
    TEST (charT ('2'), XDIGIT);
    TEST (charT ('2'), GRAPH);
    TEST (charT ('2'), PRINT);
    TEST (charT ('2'), DIGIT | XDIGIT | GRAPH | PRINT);

    TEST (charT ('3'), DIGIT);
    TEST (charT ('3'), XDIGIT);
    TEST (charT ('3'), GRAPH);
    TEST (charT ('3'), PRINT);
    TEST (charT ('3'), DIGIT | XDIGIT | GRAPH | PRINT);

    TEST (charT (' '), SPACE);
    TEST (charT (' '), GRAPH);
    TEST (charT (' '), PRINT);
    TEST (charT (' '), SPACE | GRAPH | PRINT);

    if (sizeof (charT) > 1) {
        TEST (ct.widen ('\xa0'), ALPHA);
        TEST (ct.widen ('\xa0'), LOWER);
        TEST (ct.widen ('\xa0'), GRAPH);
        TEST (ct.widen ('\xa0'), PRINT);
        TEST (ct.widen ('\xa0'), ALPHA | LOWER | GRAPH | PRINT);

        TEST (ct.widen ('\xa1'), ALPHA);
        TEST (ct.widen ('\xa1'), UPPER);
        TEST (ct.widen ('\xa1'), GRAPH);
        TEST (ct.widen ('\xa1'), PRINT);
        TEST (ct.widen ('\xa1'), ALPHA | UPPER | GRAPH | PRINT);

        TEST (ct.widen ('\xa2'), DIGIT);
        TEST (ct.widen ('\xa2'), GRAPH);
        TEST (ct.widen ('\xa2'), PRINT);
        TEST (ct.widen ('\xa2'), DIGIT | GRAPH | PRINT);
    }
}

/**************************************************************************/

template <class charT>
void test_libstd (charT, const char *cname)
{
    if (0) {
        // do a compile time only test on use_facet and has_facet
        _STD_HAS_FACET (std::ctype_byname<charT>, std::locale ());
        _STD_USE_FACET (std::ctype_byname<charT>, std::locale ());
    }

    const char cmap_1[] = {
        "<code_set_name> \"ANSI_X3.4-1968\"\n"
        "<mb_cur_max> 1\n"
        "<mb_cur_min> 1\n"
        "CHARMAP\n"
        "<U0000> \\x00\n"
        "<U0001> \\x01\n"
        "<U0002> \\x02\n"
        "<U0003> \\x03\n"
        "<U0004> \\x04\n"
        "<U0005> \\x05\n"
        "<U0006> \\x06\n"
        "<U0007> \\x07\n"
        "<U0008> \\x08\n"
        "<U0009> \\x09\n"
        "<U000a> \\x0a\n"
        "<U000b> \\x0b\n"
        "<U000c> \\x0c\n"
        "<U000d> \\x0d\n"
        "<U000e> \\x0e\n"
        "<U000f> \\x0f\n"
        "<U0010> \\x10\n"
        "<U0011> \\x11\n"
        "<U0012> \\x12\n"
        "<U0013> \\x13\n"
        "<U0014> \\x14\n"
        "<U0015> \\x15\n"
        "<U0016> \\x16\n"
        "<U0017> \\x17\n"
        "<U0018> \\x18\n"
        "<U0019> \\x19\n"
        "<U001a> \\x1a\n"
        "<U001b> \\x1b\n"
        "<U001c> \\x1c\n"
        "<U001d> \\x1d\n"
        "<U001e> \\x1e\n"
        "<U001f> \\x1f\n"
        "<U0020> \\x20\n"
        "<U0021> \\x21\n"
        "<U0022> \\x22\n"
        "<U0023> \\x23\n"
        "<U0024> \\x24\n"
        "<U0025> \\x25\n"
        "<U0026> \\x26\n"
        "<U0027> \\x27\n"
        "<U0028> \\x28\n"
        "<U0029> \\x29\n"
        "<U002a> \\x2a\n"
        "<U002b> \\x2b\n"
        "<U002c> \\x2c\n"
        "<U002d> \\x2d\n"
        "<U002e> \\x2e\n"
        "<U002f> \\x2f\n"
        "<U0030> \\x30\n"
        "<U0031> \\x31\n"
        "<U0032> \\x32\n"
        "<U0033> \\x33\n"
        "<U0034> \\x34\n"
        "<U0035> \\x35\n"
        "<U0036> \\x36\n"
        "<U0037> \\x37\n"
        "<U0038> \\x38\n"
        "<U0039> \\x39\n"
        "<U003a> \\x3a\n"
        "<U003b> \\x3b\n"
        "<U003c> \\x3c\n"
        "<U003d> \\x3d\n"
        "<U003e> \\x3e\n"
        "<U003f> \\x3f\n"
        "<U0040> \\x40\n"
        "<U0041> \\x41\n"
        "<U0042> \\x42\n"
        "<U0043> \\x43\n"
        "<U0044> \\x44\n"
        "<U0045> \\x45\n"
        "<U0046> \\x46\n"
        "<U0047> \\x47\n"
        "<U0048> \\x48\n"
        "<U0049> \\x49\n"
        "<U004a> \\x4a\n"
        "<U004b> \\x4b\n"
        "<U004c> \\x4c\n"
        "<U004d> \\x4d\n"
        "<U004e> \\x4e\n"
        "<U004f> \\x4f\n"
        "<U0050> \\x50\n"
        "<U0051> \\x51\n"
        "<U0052> \\x52\n"
        "<U0053> \\x53\n"
        "<U0054> \\x54\n"
        "<U0055> \\x55\n"
        "<U0056> \\x56\n"
        "<U0057> \\x57\n"
        "<U0058> \\x58\n"
        "<U0059> \\x59\n"
        "<U005a> \\x5a\n"
        "<U005b> \\x5b\n"
        "<U005c> \\x5c\n"
        "<U005d> \\x5d\n"
        "<U005e> \\x5e\n"
        "<U005f> \\x5f\n"
        "<U0060> \\x60\n"
        "<U0061> \\x61\n"
        "<U0062> \\x62\n"
        "<U0063> \\x63\n"
        "<U0064> \\x64\n"
        "<U0065> \\x65\n"
        "<U0066> \\x66\n"
        "<U0067> \\x67\n"
        "<U0068> \\x68\n"
        "<U0069> \\x69\n"
        "<U006a> \\x6a\n"
        "<U006b> \\x6b\n"
        "<U006c> \\x6c\n"
        "<U006d> \\x6d\n"
        "<U006e> \\x6e\n"
        "<U006f> \\x6f\n"
        "<U0070> \\x70\n"
        "<U0071> \\x71\n"
        "<U0072> \\x72\n"
        "<U0073> \\x73\n"
        "<U0074> \\x74\n"
        "<U0075> \\x75\n"
        "<U0076> \\x76\n"
        "<U0077> \\x77\n"
        "<U0078> \\x78\n"
        "<U0079> \\x79\n"
        "<U007a> \\x7a\n"
        "<U007b> \\x7b\n"
        "<U007c> \\x7c\n"
        "<U007d> \\x7d\n"
        "<U007e> \\x7e\n"
        "<U007f> \\x7f\n"
        "<U1000> \\xa0\n"
        "<U1001> \\xa1\n"
        "<U1002> \\xa2\n"
        "END CHARMAP\n"
    };

    const char loc_1[] = {
        "escape_char /\n"
        "LC_CTYPE\n"
        "#     <A>     <B>     <C>     MYANMAR LETTER KHA\n"
        "upper <U0041>;<U0042>;<U0043>;<U1001>\n"
        "      <a>     <b>     <c>     MYANMAR LETTER KA\n"
        "lower <U0061>;<U0062>;<U0063>;<U1000>\n"
        "alpha <U0061>;<U0062>;<U0063>;<U0041>;"
              "<U0042>;<U0043>;<U1000>;<U1001>\n"
        "digit <U0031>;<U0032>;<U0033>;<U1002>\n"
        "space <U0020>\n"
        "cntrl <U0000>\n"
        "      <!>      <\">\n"
        "punct <U0021>; <U0022>\n"
        "graph <U0041>;<U0042>;<U0043>;<U0061>;<U0062>;<U0063>;"
              "<U1000>;<U1001>;<U1002>;<U1003>;<U1004>;<U1005>;"
              "<U0031>;<U0032>;<U0033>;<U0020>;<U0021>;<U0022>\n"
        "print <U0041>;<U0042>;<U0043>;"
              "<U0061>;<U0062>;<U0063>;"
              "<U1000>;<U1001>;<U1002>;<U1003>;<U1004>;<U1005>;"
              "<U0031>;<U0032>;<U0033>;<U0020>;<U0021>;<U0022>\n"
        "xdigit <U0041>;<U0042>;<U0043>;<U0061>;<U0062>;"
               "<U0063>;<U0031>;<U0032>;<U0033>\n"
        "toupper (<U0061>,<U0041>);(<U0062>,<U0042>);"
                "(<U0063>,<U0043>);(<U1000>,<U1001>)\n"
        "tolower (<U0041>,<U0061>);(<U0042>,<U0062>);"
                "(<U0043>,<U0063>);(<U1001>,<U1000>)\n"
        "END LC_CTYPE\n"
    };

    //invoke rw_create_locale to build a locale to test with
    const char* const locname = rw_create_locale (cmap_1, loc_1);

    if (!rw_error (0 != locname, 0, __LINE__,
                   "failed to create a locale in %s", locale_root))
        return;

    const std::locale loc (locname);

    const std::ctype<charT> &ct =
        _STD_USE_FACET (std::ctype<charT>, loc);

#if TEST_RW_EXTENSIONS
    ct._C_opts |=  ct._C_use_libstd;
    ct._C_opts &= ~ct._C_use_libc;
#endif // TEST_RW_EXTENSIONS

    test_libstd_mask (charT (), cname, ct, locname);
    test_libstd_toupper_tolower (charT (), cname, ct, locname);

    // now check the scan functions
    rw_info (0, 0, __LINE__,
             "std::ctype<%s>::scan_is(const %1$s*, const %1$s, mask*) "
             "in locale(%#s)", cname, locname);

    test_libstd_scan_is (charT (), cname, ct, "abc1BC",   DIGIT, 3);
    test_libstd_scan_is (charT (), cname, ct, "abc123B ", SPACE, 7);
    test_libstd_scan_is (charT (), cname, ct, "abc123",   LOWER, 0);
    test_libstd_scan_is (charT (), cname, ct, "abc123",   UPPER, 6);
#if TEST_RW_EXTENSIONS
    // RW assumes that std::ctype_base::mask is the same as int
    test_libstd_scan_is (charT (), cname, ct, "abc1ABC",  DIGIT | UPPER, 3);
    test_libstd_scan_is (charT (), cname, ct, "abcA2BC",  DIGIT | UPPER, 3);
#else
    static const std::ctype_base::mask DIGIT_OR_UPPER =
	std::ctype_base::mask(DIGIT | UPPER);
    test_libstd_scan_is (charT (), cname, ct, "abc1ABC",  DIGIT_OR_UPPER, 3);
    test_libstd_scan_is (charT (), cname, ct, "abcA2BC",  DIGIT_OR_UPPER, 3);
#endif

    if (sizeof(charT) > 1) {
        test_libstd_scan_is (charT (), cname, ct, "ABC\xa0xyz", LOWER, 3, 1);
        test_libstd_scan_is (charT (), cname, ct, "abcx\xa1yz", UPPER, 4, 1);
    }

    rw_info (0, 0, __LINE__,
             "std::ctype<%s>::scan_not(const %1$s*, const %1$s, mask*) "
             "in locale(%#s)", cname, locname);

    test_libstd_scan_not (charT (), cname, ct, "abc1BC", ALPHA, 3);
    test_libstd_scan_not (charT (), cname, ct, "aaBBcc", LOWER, 2);
    test_libstd_scan_not (charT (), cname, ct, "abc1BC", DIGIT, 0);
    test_libstd_scan_not (charT (), cname, ct, "abc1BC", PRINT, 6);

    if (sizeof(charT) > 1) {
        test_libstd_scan_not (charT (), cname, ct, "abc\xa2xyz", ALPHA, 3, 1);
        test_libstd_scan_not (charT (), cname, ct, "123\xa1yz ", DIGIT, 3, 1);
    }
}

/**************************************************************************/

static int
run_test (int, char**)
{
    test_libc (char (), "char");

#ifndef _RWSTD_NO_WCHAR_T
    test_libc (wchar_t (), "wchar_t");
#endif   // _RWSTD_NO_WCHAR_T

#if TEST_RW_EXTENSIONS
    // rw_create_locale does not work as it depends on environment
    // variables being set.  'localedef' not really working too.
    test_libstd (char (), "char");

#ifndef _RWSTD_NO_WCHAR_T
    test_libstd (wchar_t (), "wchar_t");
#endif   // _RWSTD_NO_WCHAR_T

#endif // TEST_RW_EXTENSIONS
    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.category.ctype",
                    0 /* no comment */,
                    run_test,
                    "",
                    (void*)0   /* sentinel */);
}
