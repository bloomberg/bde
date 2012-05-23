/***************************************************************************
 *
 * 22.locale.ctype.scan.cpp -  Tests exercising the scan() of ctype facet
 *
 * $Id: 22.locale.ctype.scan.cpp 648752 2008-04-16 17:01:56Z faridz $
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
void test_libc (charT, const char *cname)
{
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
void test_libstd (charT, const char *cname)
{
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

template <class charT>
void run_test (charT, const char *cname)
{
    if (0) {
        // do a compile time only test on use_facet and has_facet
        _STD_HAS_FACET (std::ctype_byname<charT>, std::locale ());
        _STD_USE_FACET (std::ctype_byname<charT>, std::locale ());
    }

    test_libc (charT (), cname);

#if TEST_RW_EXTENSIONS
    // rw_create_locale does not work as it depends on environment
    // variables being set.  Also 'localedef' not really working in our
    // machines.
    test_libstd (charT (), cname);
#endif
}

/**************************************************************************/

static int
run_test (int, char**)
{
    run_test (char (), "char");
    run_test (wchar_t (), "wchar_t");

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
