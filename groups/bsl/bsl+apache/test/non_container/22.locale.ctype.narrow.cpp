/***************************************************************************
 *
 * 22.locale.ctype.narrow.cpp - tests exercising the narrow() and widen()
 *                              member functions of the ctype facet
 *
 * $Id: 22.locale.ctype.narrow.cpp 650350 2008-04-22 01:35:17Z sebor $
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
 * Copyright 2001-2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

// DESCRIPTION: The test iterates over a subset of locales installed
// on a machine, calling the C character classification functions and
// their C++ counterpart(s), comparing the results of the calls against
// one another.

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

// cond1() verifies condition [1] in test_narrow_widen()
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


// cond3() overloads verify condition [3] in test_narrow_widen()
// below using libc functions
bool cond3 (std::ctype_base::mask, char, const char*)
{
    return true;
}


#ifndef _RWSTD_NO_WCHAR_T

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
                 " returned false in locale(%#s)", cname, ch,               \
                 ch, dfault, ctp.narrow (ch, '\0'),                         \
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

        if (0 == i)
            rw_info (0, 0, __LINE__, "std::ctype<%s> in locale(%#s)",
                     cname, locname);

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
                       "ctype<%s>::narrow (%{*.*Ac}\", ... , %{#c}) "
                       "== %{.*Ac}, got %{.*Ac} in locale (%#s)", cname,
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

// exercise the behavior of the libc-based C++ locale implementation
template <class charT>
void test_libc (charT, const char *cname)
{
    test_narrow_widen (charT (), cname);
}

/**************************************************************************/

// exercise the behavior of our own C++ locale implementation
template <class charT>
void test_libstd (charT, const char *cname)
{
    rw_warn (0, 0, __LINE__,
             "stdcxx implementation of std::ctype<%s> not exercised",
             cname);
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

    // exercise the behavior of the libc-based C++ locale implementation
    test_libc (charT (), cname);

    // exercise the behavior of our own C++ locale implementation
#if TEST_RW_EXTENSIONS
    // rw_create_locale does not work on SUN as it depends on environment
    // variables being set.  Also 'localedef' is not really working on our
    // machines.
    test_libstd (charT (), cname);
#endif
}

/**************************************************************************/

static int
run_test (int, char**)
{
    // set the global locale_list pointer to point to the array
    // of NUL-separated locale names set on the command line via
    // the --locales=... option, if specified, or to 0 (in which
    // case we'll generate our own list)
#if TEST_RW_EXTENSIONS
    // Only classic 'C' locale defined for stlport.
    locale_list = rw_opt_locales;
#endif

    run_test (char (), "char");

#ifndef _RWSTD_NO_WCHAR_T

    run_test (wchar_t (), "wchar_t");

#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.category.ctype",
                    "narrow and widen",
                    run_test,
                    "|-locales= ",
                    &rw_opt_setlocales,
                    (void*)0   /* sentinel */);
}
