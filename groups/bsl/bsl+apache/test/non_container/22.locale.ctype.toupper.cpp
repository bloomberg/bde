/***************************************************************************
 *
 * 22.locale.ctype.toupper.cpp - Tests exercising the toupper() of ctype facet
 *
 * $Id: 22.locale.ctype.toupper.cpp 648752 2008-04-16 17:01:56Z faridz $
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
char libc_tolower (char ch)
{
    return std::tolower (UChar (ch));
}

char libc_toupper (char ch)
{
    return (std::toupper)(UChar (ch));
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

wchar_t libc_tolower (wchar_t ch)
{
    return (std::towlower)(ch);
}

wchar_t libc_toupper (wchar_t ch)
{
    return (std::towupper)(ch);
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

/**************************************************************************/

template <class charT>
void test_toupper (charT, const char *cname)
{
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
#if DRQS // Missing convenience functions from [lib.locale]
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
void test_libc (charT, const char *cname)
{
    test_toupper (charT (), cname);
}

/**************************************************************************/

template <class charT>
void test_libstd_toupper (charT, const char *cname,
                                  const std::ctype<charT> &ct,
                                  const char *locname)
{
    rw_info (0, 0, __LINE__,
             "std::ctype<%s>::toupper(%1$s) in locale(%#s)",
             cname, locname);

    int success;

#undef TEST
#define TEST(lower, upper)                                              \
    success = ct.widen (upper) == ct.toupper (ct.widen (lower));        \
    rw_assert (success, 0, __LINE__,                                    \
               "ctype<%s>::toupper(%d) == %d, got %d", cname,           \
               lower, upper, ct.toupper((charT)lower));                 

    TEST ('a', 'A');
    TEST ('b', 'B');
    TEST ('c', 'C');

    if (sizeof(charT) > 1)
        TEST ('\xa0', '\xa1');

#undef TEST
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

    test_libstd_toupper (charT (), cname, ct, locname);
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
    // variables being set.  Also 'localedef' not really working on our
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
