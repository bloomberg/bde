/***************************************************************************
 *
 * 22.codecvt.out.cpp - test exercising the std::codecvt::out()
 *
 * $Id: 22.locale.codecvt.out.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2005-2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#ifdef __SUNPRO_CC
    // working around a SunPro/SunOS bug (PR #26255)
#  include <time.h>
#endif   // __SUNPRO_CC

#include <locale>    // for codecvt

#include <climits>   // for MB_LEN_MAX
#include <clocale>   // for LC_CTYPE, setlocale()
#include <cstdlib>   // for MB_CUR_MAX, free(), size_t
#include <cstring>   // for strcpy(), strlen()
#include <cwchar>    // for codecvt

#include <driver.h>      // for rw_test(), ...
#include <file.h>        // for rw_fwrite()
#include <rw_locale.h>   // for rw_locales(), rw_set_locale_root()
#include <rw_printf.h>   // for rw_sprintf()
#include <valcmp.h>      // for rw_strcmp()

/****************************************************************************/

// the root of the locale directory (RWSTD_LOCALE_ROOT)
// set in main() instead of here to avoid Solaris 7 putenv() bug (PR #30017)
const char* locale_root /* = set in main() */;


// creates a table-based multibyte locale
static const char*
create_locale ()
{
    char cm_fname [1024];
    if (rw_snprintf (cm_fname, sizeof cm_fname, "%s%c%s",
                     locale_root, _RWSTD_PATH_SEP, "charmap") < 0)
        return 0;

    static const char charmap[] = {
        "<code_set_name> test_charmap \n"
        "<comment_char>  % \n"
        "<escape_char>   / \n"
        "<mb_cur_min>    1 \n"
        "<mb_cur_max>    9 \n"
        "CHARMAP \n"
        "<U0000>   /x30                                   0 \n"
        "<U0001>   /x31                                   1 \n"
        "<U0002>   /x32/x32                               22 \n"
        "<U0003>   /x33/x33/x33                           333 \n"
        "<U0004>   /x34/x34/x34/x34                       4444 \n"
        "<U0005>   /x35/x35/x35/x35/x35                   55555 \n"
        "<U0006>   /x36/x36/x36/x36/x36/x36               666666 \n"
        "<U0007>   /x37/x37/x37/x37/x37/x37/x37           7777777 \n"
        "<U0008>   /x38/x38/x38/x38/x38/x38/x38/x38       88888888 \n"
        "<U0009>   /x39/x39/x39/x39/x39/x39/x39/x39/x39   999999999 \n"
        "<U0010>   /x41                                   A \n"
        "<U0011>   /x42                                   B \n"
        "<U0012>   /x43                                   C \n"
        "<U0013>   /x44                                   D \n"
        "<U0014>   /x45                                   E \n"
        "<U0015>   /x46                                   F \n"
        "END CHARMAP \n"
    };

    if (std::size_t (-1) == rw_fwrite (cm_fname, charmap))
        return 0;

    char src_fname [1024];
    if (rw_snprintf (src_fname, sizeof src_fname, "%s%c%s",
                     locale_root, _RWSTD_PATH_SEP, "source") < 0)
        return 0;

    if (std::size_t (-1) == rw_fwrite (src_fname, "LC_CTYPE\nEND LC_CTYPE\n"))
        return 0;

    // invoke localedef to create the named locale
    // silence the following warnings:
    // 701: no compatible locale found
    // 702: member of portable character set <x> not found
    //      in the character map
    // 706: iconv_open() failed
    const char* const locname =
        rw_localedef ("-w701 -w702 -w706",
                      src_fname, cm_fname, "mb_cur_max-9");

    return locname;
}

/****************************************************************************/

// finds a multibyte character that is `bytes' long if `bytes' is less
// than or equal to MB_CUR_MAX, or the longest multibyte sequence in
// the current locale, and sets `wchar' to the wide character that
// corresponds to it
// returns `mbchar' on success, 0 on failure to find a multibute
// character of the specified length
static const char*
get_mb_char (wchar_t *wchar, char *mbchar, std::size_t bytes)
{
    RW_ASSERT (0 != wchar);
    RW_ASSERT (0 != mbchar);

    *mbchar = '\0';

    if (0 == bytes)
        return mbchar;

    const bool exact = bytes <= MB_CUR_MAX;

    if (!exact)
        bytes = MB_CUR_MAX;

    wchar_t wc;

    // search the first 64K characters sequentially
    for (wc = wchar_t (1); wc != wchar_t (0xffff); ++wc) {

        if (   int (bytes) == std::wctomb (mbchar, wc)
            && int (bytes) == std::mblen (mbchar, bytes)) {
            // NUL-terminate the multibyte character of the requested length
            mbchar [bytes] = '\0';
            *wchar = wc;
            break;
        }

        *mbchar = '\0';
    }

#if 2 < _RWSTD_WCHAR_SIZE

    // if a multibyte character of the requested size is not found
    // in the low 64K range, try to find one using a random search
    if (wchar_t (0xffff) == wc) {

        // iterate only so many times to prevent an infinite loop
        // in case when MB_CUR_MAX is greater than the longest
        // multibyte character
        for (int i = 0; i != 0x100000; ++i) {

            wc = 0;

            typedef unsigned char UChar;

            // set wc to a random value (rand() returns a value
            // less than or equal to RAND_MAX so the loop makes
            // sure all bits are initialized)
            for (int j = 0; j < int (sizeof wc); ++j) {
                wc <<= 8;
                wc  |= wchar_t (UChar (std::rand ()));
            }

            if (   int (bytes) == std::wctomb (mbchar, wc)
                && int (bytes) == std::mblen (mbchar, bytes)) {
                // NUL-terminate the multibyte character
                mbchar [bytes] = '\0';
                *wchar = wc;
                break;
            }

            *mbchar = '\0';
        }
    }

#endif   // 2 < _RWSTD_WCHAR_SIZE

    // return 0 on failure to find a sequence exactly `bytes' long
    return !exact || bytes == std::strlen (mbchar) ? mbchar : 0;
}

/****************************************************************************/

struct WideCode
{
    wchar_t wchar;
    char    mbchar [MB_LEN_MAX];
};

typedef WideCode mb_char_array_t [MB_LEN_MAX];

// fills consecutive elemenets of the `mb_chars' array with wide
// and multibyte characters between 1 and MB_CUR_MAX bytes long
// for the given locale returns the number of elements populated
// (normally, MB_CUR_MAX)
static std::size_t
get_mb_chars (mb_char_array_t mb_chars)
{
    RW_ASSERT (0 != mb_chars);

    const char* mbc =
        get_mb_char (&mb_chars [0].wchar,
                     mb_chars [0].mbchar,
                     std::size_t (-1));

    if (!mbc) {
        rw_fprintf (rw_stderr, "*** unable to find any multibyte characters "
                    "in locale \"%s\" with MB_CUR_MAX = %u\n",
                    std::setlocale (LC_CTYPE, 0),
                    unsigned (MB_CUR_MAX));
        return 0;
    }

    std::size_t mb_cur_max = std::strlen (mbc);

    if (MB_LEN_MAX < mb_cur_max)
        mb_cur_max = MB_LEN_MAX;

    // fill each element of `mb_chars' a multibyte character
    // of the corresponding length
    for (std::size_t i = mb_cur_max; i; --i) {

        const std::size_t inx = i - 1;

        // try to generate a multibyte character `i' bytes long
        mbc = get_mb_char (&mb_chars [inx].wchar, mb_chars [inx].mbchar, i);

        if (0 == mbc) {
            if (i < mb_cur_max) {
                rw_fprintf (rw_stderr, "*** unable to find %zu-byte characters"
                            " in locale \"%s\" with MB_CUR_MAX = %u\n",
                            i + 1, std::setlocale (LC_CTYPE, 0),
                            unsigned (MB_CUR_MAX));
                mb_cur_max = 0;
                break;
            }
            --mb_cur_max;
        }
    }

    return mb_cur_max;
}

/****************************************************************************/

// finds the multibyte locale with the largest MB_CUR_MAX value and
// fills consecutive elemenets of the `mb_chars' array with multibyte
// characters between 1 and MB_CUR_MAX bytes long for such a locale
static const char*
find_mb_locale (std::size_t *mb_cur_max, mb_char_array_t mb_chars)
{
    RW_ASSERT (0 != mb_cur_max);
    RW_ASSERT (0 != mb_chars);

    if (2 > MB_LEN_MAX) {
        rw_fprintf (rw_stderr, "MB_LEN_MAX = %d, giving up\n", MB_LEN_MAX);
        return 0;
    }

    static const char *mb_locale_name;

    char saved_locale_name [1024];
    std::strcpy (saved_locale_name, std::setlocale (LC_CTYPE, 0));

    *mb_cur_max = 0;

    // iterate over all installed locales
    for (const char *name = rw_locales (); name && *name;
         name += std::strlen (name) + 1) {

        if (std::setlocale (LC_CTYPE, name)) {

            // try to generate a set of multibyte characters
            // with lengths from 1 and MB_CUR_MAX (or less)
            const std::size_t cur_max = get_mb_chars (mb_chars);

            if (*mb_cur_max < cur_max) {
                *mb_cur_max    = cur_max;
                mb_locale_name = name;

                // break when we've found a multibyte locale
                // with the longest possible encoding
                if (MB_LEN_MAX == *mb_cur_max)
                    break;
            }
        }
    }

    if (*mb_cur_max < 2) {
        rw_fprintf (rw_stderr, "*** unable to find a full set of multibyte "
                    "characters in locale \"%s\" with MB_CUR_MAX = %u "
                    "(computed)", mb_locale_name, *mb_cur_max);
        mb_locale_name = 0;
    }
    else {
        // (re)generate the multibyte characters for the saved locale
        // as they may have been overwritten in subsequent iterations
        // of the loop above (while searching for a locale with greater
        // value of MB_CUR_MAX)
        std::setlocale (LC_CTYPE, mb_locale_name);
        get_mb_chars (mb_chars);
    }

    std::setlocale (LC_CTYPE, saved_locale_name);

    return mb_locale_name;
}

/****************************************************************************/

inline const char* codecvt_result (std::codecvt_base::result res)
{
    return   std::codecvt_base::error == res ? "error"
           : std::codecvt_base::ok == res ? "ok"
           : std::codecvt_base::partial == res ? "partial"
           : "noconv";
}


template <class internT>
void test_out (int                                                line,
               const char*                                        tname,
               const std::mbstate_t                              *pstate,
               const std::codecvt<internT, char, std::mbstate_t> &cvt,
               const internT                                     *src,
               std::size_t                                        src_len,
               std::size_t                                        src_off,
               const char                                        *res,
               std::size_t                                        res_len,
               std::size_t                                        res_off,
               std::codecvt_base::result                          result)
{
    static const std::mbstate_t initial_state = std::mbstate_t ();

    std::mbstate_t state = pstate ? *pstate : initial_state;

    // create and invalidate a buffer for the destination sequence
    char buf [1024];
    std::memset (buf, -1, sizeof buf);

    // set up from, from_end, and from_next arguments
    const internT* const from      = src;
    const internT* const from_end  = from + src_len;
    const internT*       from_next = 0;

    // set up to, to_end, and to_next arguments
    char* const to      = buf;
    char* const to_end  = to + res_len;
    char*       to_next = 0;

    // call codecvt::out () with the arguments above
    const std::codecvt_base::result cvtres =
        cvt.out (state,
                 from, from_end, from_next,
                 to, to_end, to_next);

    // format a string describing the function call above
    static char fcall [4096];
    std::memset (fcall, 0, sizeof fcall);

    rw_sprintf (fcall,
                "codecvt<%s, char, mbstate_t>::out(state, "
                "from = %{*.*Ac}, from + %td, from + %td, "
                "to = %{#*s}, to + %d, to + %d)",
                tname, int (sizeof *from), int (src_len), from,
                from_end - from, from_next - from,
                int (to_next - to), to, to_end - to, to_next - to);

    RW_ASSERT (std::strlen (fcall) < sizeof fcall);

    // verify the expected result of the conversion
    rw_assert (cvtres == result, __FILE__, line,
               "line %d: %s == %s, got %s",
               __LINE__, fcall,
               codecvt_result (result),
               codecvt_result (cvtres));

    // verify that the from_next pointer is set just past the last
    // successfully converted character in the source sequence
    rw_assert (from_next == from + src_off, __FILE__, line,
               "line %d: %s: from_next == from + %d, got from + %d",
               __LINE__, fcall,
               src_off, from_next - from);

    // verify that the to_next pointer is set just past the last
    // external character in the converted (destination) sequence
    rw_assert (to_next == to + res_off, __FILE__, line,
               "line %d: %s: to_next == to + %d, got to + %d",
               __LINE__, fcall,
               res_off, to_next - to);

    // compare the converted sequence against the expected result
    rw_assert (0 == rw_strncmp (to, res, res_off), __FILE__, line,
               "line %d: %s: expected %{#*s}, got %{#*s}",
               __LINE__, fcall,
               int (res_off), res, int (to_next - to), to);

    // verify that the function didn't write past the end
    // of the destination buffer
    rw_assert (char (-1) == to [res_off], 0, line,
               "line %d: %s: expected %{#lc}, got %{#lc} "
               "at end of destination buffer (offset %zu)",
               __LINE__, fcall,
               -1, to [res_off], res_off);

    // verify that the conversion state is as expected
    rw_assert (!pstate || !std::memcmp (pstate, &state, sizeof state),
               __FILE__, line,
               "line %d: %s: unexpected conversion state",
               __LINE__, fcall);
}

/****************************************************************************/

#if TEST_RW_EXTENSIONS
typedef std::codecvt<char, char, std::mbstate_t>           Codecvt;
typedef std::codecvt_byname<char, char, std::mbstate_t>    CodecvtByname;
#else // if ! TEST_RW_EXTENSIONS

typedef std::codecvt<char, char, std::mbstate_t>           Codecvt;
struct CodecvtByname : std::codecvt_byname<char, char, std::mbstate_t>
{
    typedef std::codecvt_byname<char, char, std::mbstate_t> Base;
    CodecvtByname(const char* s) : Base(s) { }
    ~CodecvtByname() { }
};

#endif // TEST_RW_EXTENSIONS

static void
test_codecvt (const Codecvt *pcvt = 0)
{
    if (0 == pcvt)
        rw_info (0, 0, __LINE__,
                 "std::codecvt<char, char, mbstate_t>::out("
                 "state_type&, "
                 "const intern_type*, const intern_type*, "
                 "const intern_type*, "
                 "extern_type*, extern_type*, exterm_type*&)");

    const std::locale classic = std::locale::classic ();

    const Codecvt &cvt = pcvt ? *pcvt : std::use_facet<Codecvt>(classic);

#undef TEST
#define TEST(from, from_end_off, from_next_off,      \
             res, res_end_off, res_next_off, result) \
    test_out (__LINE__, "char", 0, cvt,              \
              from, from_end_off, from_next_off,     \
              res, res_end_off, res_next_off,        \
              std::codecvt_base::result)

    //    +---------------------------- source sequence before conversion
    //    |      +--------------------- (from_end - from) before conversion
    //    |      |  +------------------ (from_next - from) after conversion
    //    |      |  |  +--------------- destination sequence after conversion
    //    |      |  |  |      +-------- (to_end - to) before conversion
    //    |      |  |  |      |  +----- (to_next - to) after conversion
    //    |      |  |  |      |  |  +-- conversion result
    //    |      |  |  |      |  |  |
    //    V      V  V  V      V  V  V
    TEST ("",    0, 0, "",    0, 0, noconv);
    TEST ("a",   1, 0, "",    0, 0, noconv);
    TEST ("b",   1, 0, "",    1, 0, noconv);
    TEST ("ab",  2, 0, "",    0, 0, noconv);
    TEST ("bc",  2, 0, "",    1, 0, noconv);
    TEST ("cd",  2, 0, "",    2, 0, noconv);
    TEST ("abc", 3, 0, "",    0, 0, noconv);
    TEST ("\0",  1, 0, "",    0, 0, noconv);
}

/****************************************************************************/

static void
test_codecvt_byname ()
{
    rw_info (0, 0, __LINE__,
             "std::codecvt_byname<char, char, mbstate_t>::out("
             "state_type&, const intern_type*, const intern_type*, "
             "const intern_type*, "
             "extern_type*, extern_type*, exterm_type*&)");

    const CodecvtByname cvt ("");

    test_codecvt (&cvt);
}

/****************************************************************************/

#ifndef _RWSTD_NO_WCHAR_T

#if TEST_RW_EXTENSIONS
// protected destructors for codecvt and codecvt_byname are public in RW?
typedef std::codecvt<wchar_t, char, std::mbstate_t>        WCodecvt;
typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> WCodecvtByname;
#else // !TEST_RW_EXTENSIONS
struct WCodecvt : std::codecvt<wchar_t, char, std::mbstate_t>
{
    WCodecvt(std::size_t refs = 0)
    : std::codecvt<wchar_t, char, std::mbstate_t>(refs) { }
};
struct WCodecvtByname : std::codecvt_byname<wchar_t, char, std::mbstate_t>
{
    WCodecvtByname(const char* s, std::size_t refs = 0)
    : std::codecvt_byname<wchar_t, char, std::mbstate_t>(s,refs) { }
};
#endif // !TEST_RW_EXTENSIONS

static void
test_wcodecvt ()
{
    rw_info (0, 0, __LINE__,
             "std::codecvt<wchar_t, char, mbstate_t>::out("
             "state_type&, "
             "const intern_type*, const intern_type*, "
             "const intern_type*, "
             "extern_type*, extern_type*, exterm_type*&)");

    const std::locale classic = std::locale::classic ();

    const WCodecvt &cvt = std::use_facet<WCodecvt>(classic);

#undef TEST
#define TEST(from, from_end_off, from_next_off,      \
             res, res_end_off, res_next_off, result) \
    test_out (__LINE__, "wchar_t", 0, cvt,           \
              from, from_end_off, from_next_off,     \
              res, res_end_off, res_next_off,        \
              std::codecvt_base::result)

    //    +------------------------------- source sequence before conversion
    //    |        +---------------------- (from_end - from) before conversion
    //    |        |  +------------------- (from_next - from) after conversion
    //    |        |  |  +---------------- destination sequence
    //    |        |  |  |       +-------- (to_end - to) before conversion
    //    |        |  |  |       |  +----- (to_next - to) after conversion
    //    |        |  |  |       |  |  +-- conversion result
    //    |        |  |  |       |  |  |
    //    V        V  V  V       V  V  V
    TEST (L"",     0, 0, "",     0, 0, ok);
    TEST (L"a",    1, 1, "a",    1, 1, ok);
    TEST (L"ab",   2, 2, "ab",   2, 2, ok);
    TEST (L"abc",  3, 3, "abc",  3, 3, ok);
    TEST (L"\0",   1, 1, "\0",   1, 1, ok);
    TEST (L"\x80", 1, 1, "\x80", 1, 1, ok);
    TEST (L"\xff", 1, 1, "\xff", 1, 1, ok);
    TEST (L"a\0",  2, 2, "a\0",  2, 2, ok);
    TEST (L"a\0b", 3, 3, "a\0b", 3, 3, ok);
    TEST (L"\0\0", 2, 2, "\0\0", 2, 2, ok);

    TEST (L"123",  3, 0, "",     0, 0, partial);
    TEST (L"234",  3, 1, "2",    1, 1, partial);
    TEST (L"345",  3, 2, "34",   2, 2, partial);
    TEST (L"456",  3, 3, "456",  3, 3, ok);
    TEST (L"567",  3, 3, "567",  4, 3, ok);
    TEST (L"6789", 3, 3, "678",  4, 3, ok);
}

/****************************************************************************/

// exercises an algorithmic multibyte encoding
static void
test_wcodecvt_byname_algorithmic ()
{
    rw_info (0, 0, __LINE__, "locale (\"UTF-8@UCS\") [algorithmic encoding]");

    // lowercase utf ==> relaxed checking (i.e., some, but not all,
    // invalid UTF-8 sequence are accepted)
    const WCodecvtByname cvt_relaxd ("utf-8@UCS");

    // capital UTF ==> strict checking
    const WCodecvtByname cvt_strict ("UTF-8@UCS");

#undef STRICT
#define STRICT(from, from_end_off, from_next_off,    \
             res, res_end_off, res_next_off, result) \
    test_out (__LINE__, "wchar_t", 0, cvt_strict,    \
              from, from_end_off, from_next_off,     \
              res, res_end_off, res_next_off,        \
              std::codecvt_base::result)

#undef RELAXD
#define RELAXD(from, from_end_off, from_next_off,    \
             res, res_end_off, res_next_off, result) \
    test_out (__LINE__, "wchar_t", 0, cvt_relaxd,    \
              from, from_end_off, from_next_off,     \
              res, res_end_off, res_next_off,        \
              std::codecvt_base::result)

#undef TEST
#define TEST(from, from_end_off, from_next_off,      \
             res, res_end_off, res_next_off, result) \
    STRICT (from, from_end_off, from_next_off,       \
            res, res_end_off, res_next_off, result); \
    RELAXD (from, from_end_off, from_next_off,       \
            res, res_end_off, res_next_off, result)

    //    +------------------------------- source sequence before conversion
    //    |        +---------------------- (from_end - from) before conversion
    //    |        |  +------------------- (from_next - from) after conversion
    //    |        |  |  +---------------- destination sequence
    //    |        |  |  |       +-------- (to_end - to) before conversion
    //    |        |  |  |       |  +----- (to_next - to) after conversion
    //    |        |  |  |       |  |  +-- conversion result
    //    |        |  |  |       |  |  |
    //    V        V  V  V       V  V  V
    TEST (L"",     0, 0, "",     0, 0, ok);
    TEST (L"a",    1, 1, "a",    1, 1, ok);
    TEST (L"ab",   2, 2, "ab",   2, 2, ok);
    TEST (L"abc",  3, 3, "abc",  3, 3, ok);
    TEST (L"\0",   1, 1, "\0",   1, 1, ok);
    TEST (L"a\0",  2, 2, "a\0",  2, 2, ok);
    TEST (L"a\0b", 3, 3, "a\0b", 3, 3, ok);

    TEST (L"123",  3, 0, "",     0, 0, partial);
    TEST (L"234",  3, 1, "2",    1, 1, partial);
    TEST (L"345",  3, 2, "34",   2, 2, partial);
    TEST (L"456",  3, 3, "456",  3, 3, ok);
    TEST (L"567",  3, 3, "567",  4, 3, ok);

    // exercise multibyte sequences
    TEST (L"\x100",     1, 0, "\xc4\x80",  0, 0, partial);
    TEST (L"\x101",     1, 0, "\xc4\x81",  1, 0, partial);
    TEST (L"\x102",     1, 1, "\xc4\x82",  2, 2, ok);

    TEST (L"\x103x",    2, 0, "\xc4\x83",  0, 0, partial);
    TEST (L"\x104y",    2, 1, "\xc4\x84",  2, 2, partial);
    TEST (L"\x105z",    2, 2, "\xc4\x85z", 3, 3, ok);
    TEST (L"\x106zz",   2, 2, "\xc4\x86z", 4, 3, ok);

    TEST (L"\x0901",    1, 0, "\xe0\xa4\x81", 0, 0, partial);
    TEST (L"\x0902",    1, 0, "\xe0\xa4\x82", 1, 0, partial);
    TEST (L"\x0903",    1, 0, "\xe0\xa4\x83", 2, 0, partial);
    TEST (L"\x0904",    1, 1, "\xe0\xa4\x84", 3, 3, ok);

    TEST (L"\x0905\x0916", 2, 1, "\xe0\xa4\x85\xe0\xa4\x96", 4, 3, partial);
    TEST (L"\x0906\x0917", 2, 1, "\xe0\xa4\x86\xe0\xa4\x97", 5, 3, partial);
    TEST (L"\x0907\x0918", 2, 2, "\xe0\xa4\x87\xe0\xa4\x98", 6, 6, ok);

    // U+D800 to U+DFFF (UTF-16 surrogates) as well as U+FFFE and U+FFFF
    // must not occur in normal UCS-4 data and should be treated like
    // malformed or overlong sequences.

    STRICT (L"\xd800",       1, 0, "",    6, 0, error);
    STRICT (L"\xd801",       1, 0, "",    6, 0, error);
    STRICT (L"A\xd802",      2, 1, "A",   6, 1, error);
    STRICT (L"AB\xd803",     3, 2, "AB",  6, 2, error);
    STRICT (L"ABC\xd804",    4, 3, "ABC", 6, 3, error);
    STRICT (L"\xdffe",       1, 0, "",    6, 0, error);
    STRICT (L"\xdfff",       1, 0, "",    6, 0, error);

    TEST   (L"\xe000",       1, 1, "\xee\x80\x80", 6, 3, ok);
    TEST   (L"\xd7ff",       1, 1, "\xed\x9f\xbf", 6, 3, ok);

    STRICT (L"\xd800",       1, 0, "", 6, 0, error);
    STRICT (L"\xd801",       1, 0, "", 6, 0, error);
    STRICT (L"\xdffe",       1, 0, "", 6, 0, error);
    STRICT (L"\xdfff",       1, 0, "", 6, 0, error);

    TEST   (L"\xe000",       1, 1, "\xee\x80\x80", 6, 3, ok);

    // verify that surrogate pairs are accepted in relaxed mode
    RELAXD (L"\xd800",       1, 1, "\xed\xa0\x80", 6, 3, ok);
    RELAXD (L"\xd801",       1, 1, "\xed\xa0\x81", 6, 3, ok);
    RELAXD (L"\xdffe",       1, 1, "\xed\xbf\xbe", 6, 3, ok);
    RELAXD (L"\xdfff",       1, 1, "\xed\xbf\xbf", 6, 3, ok);
}

/****************************************************************************/

// exercises a table-based multibyte encoding
static void
test_wcodecvt_byname_table_based ()
{
    // create a locale from a generated character set description file
    // where L'\1' maps to "1", L'\2' to "22", ..., and L'\x10' to "A",
    // L'\x11' to "B", L'\x12' to "C", etc.
    const char* const locname = create_locale ();

    if (!locname) {
        rw_error (0, 0, __LINE__, "failed to create a locale database");
        return;
    }

    std::locale loc;

    _TRY {
        loc = std::locale (locname);
    }
    _CATCH (...) {
        rw_error (0, 0, __LINE__, "locale(\"%s\") unexpectedly threw", locname);
        return;
    }

    const WCodecvt &cvt_table = std::use_facet<WCodecvt>(loc);

    rw_info (0, 0, __LINE__, "locale (\"%s\") [table-based encoding]", locname);

#undef TEST
#define TEST(from, from_end_off, from_next_off,      \
             res, res_end_off, res_next_off, result) \
    test_out (__LINE__, "wchar_t", 0, cvt_table,     \
              from, from_end_off, from_next_off,     \
              res, res_end_off, res_next_off,        \
              std::codecvt_base::result)

    TEST (L"",              0, 0, "",     0, 0, ok);
    TEST (L"\x10",          1, 1, "A",    1, 1, ok);
    TEST (L"\x11\x12",      2, 2, "BC",   2, 2, ok);
    TEST (L"\x12\x13\x14",  3, 3, "CDE",  3, 3, ok);

    TEST (L"\1\2\3\4\5\6",  1, 1, "1",               21,  1, ok);
    TEST (L"\1\2\3\4\5\6",  2, 1, "1",                1,  1, partial);
    TEST (L"\1\2\3\4\5\6",  2, 1, "1",                2,  1, partial);
    TEST (L"\1\2\3\4\5\6",  2, 2, "122",              3,  3, ok);
    TEST (L"\1\2\3\4\5\6",  2, 2, "122",             21,  3, ok);
    TEST (L"\1\2\3\4\5\6",  3, 2, "122",              3,  3, partial);
    TEST (L"\1\2\3\4\5\6",  3, 2, "122",              4,  3, partial);
    TEST (L"\1\2\3\4\5\6",  3, 2, "122",              5,  3, partial);
    TEST (L"\1\2\3\4\5\6",  3, 3, "122333",           6,  6, ok);
    TEST (L"\1\2\3\4\5\6",  4, 3, "122333",           7,  6, partial);
    TEST (L"\1\2\3\4\5\6",  3, 3, "122333",          21,  6, ok);
    TEST (L"\1\2\3\4\5\6",  4, 4, "1223334444",      21, 10, ok);
    TEST (L"\1\2\3\4\5\6",  5, 5, "122333444455555", 21, 15, ok);
    TEST (L"\6\5\4\3\2\1",  6, 3, "666666555554444", 15, 15, partial);
    TEST (L"\6\5\4\3\2\1",  6, 3, "666666555554444", 16, 15, partial);

    // exercise the ability to detect invalid characters (e.g., '*')
    TEST (L"*\3\4\5\6\7",   6, 0, "",                27,  0, error);
    TEST (L"\2*\4\5\6\7",   6, 0, "",                 1,  0, partial);
    TEST (L"\2*\4\5\6\7",   6, 1, "22",               2,  2, partial);
    TEST (L"\2*\4\5\6\7",   6, 1, "22",              27,  2, error);
    TEST (L"\2*\4\5\6\7",   6, 1, "22",              27,  2, error);
    TEST (L"\2\3*\5\6\7",   6, 2, "22333",           27,  5, error);
    TEST (L"\2\3\4*\6\7",   6, 3, "223334444",       27,  9, error);
}

/****************************************************************************/

static void
make_strings (const char *pat, std::size_t patsize,
              wchar_t *wstr, char *mbstr,
              const mb_char_array_t mb_chars)
{
    RW_ASSERT (0 != pat);
    RW_ASSERT (0 != wstr);
    RW_ASSERT (0 != mbstr);
    RW_ASSERT (0 != mb_chars);

    typedef unsigned char UChar;

    *mbstr = '\0';

    for (const char *s = pat; s != pat + patsize; ++s, ++wstr) {

        if ('%' == *s) {

            ++s;

            RW_ASSERT ('0' <= *s && *s <= char ('0' + MB_LEN_MAX));

            std::size_t char_inx = *s - '0';

            *wstr = mb_chars [char_inx].wchar;

            std::strcpy (mbstr, mb_chars [char_inx].mbchar);
            mbstr += std::strlen (mbstr);
        }
        else {
            *wstr    = UChar (*s);
            *mbstr++ = *s;
            *mbstr   = '\0';
        }
    }

    *wstr = L'\0';
}

/****************************************************************************/

// exercises a libc-based multibyte encoding
static void
test_wcodecvt_byname_libc_based ()
{
    // find `mb_cur_max' multibyte characters in increasing length
    // from 1 to mb_cur_max bytes long

    // i.e., initialize the first (N + 1) elements of mb_chars as follows:
    // [0].mbstr = "0";         // where "0" is a single byte character
    // [1].mbstr = "11";        // where "11" is a two-byte character
    // [2].mbstr = "222";       // where "222" is a three-byte character
    // ...
    // [N].mbstr = "NNN...N";   // where "NNN...N" is an N-byte character

    std::size_t mb_cur_max = 0;

    mb_char_array_t mb_chars;

    const char* const locname = find_mb_locale (&mb_cur_max, mb_chars);

    if (0 == locname) {
        rw_warn (0, 0, __LINE__, "unable to find a multibyte locale");
        return;
    }

    std::locale loc;

    _TRY {
        loc = std::locale (locname);
    }
    _CATCH (...) {
        rw_error (0, 0, __LINE__, "locale(\"%s\") unexpectedly threw", locname);
        return;
    }

    const WCodecvt &cvt_libc = std::use_facet<WCodecvt>(loc);

    rw_info (0, 0, __LINE__, "locale (\"%s\") [libc-based encoding, "
             "single-byte characters]", locname);

    //////////////////////////////////////////////////////////////////
    // exercise sequences containing single-byte characters

#undef TEST
#define TEST(from, from_end_off, from_next_off,      \
             res, res_end_off, res_next_off, result) \
    test_out (__LINE__, "wchar_t", 0, cvt_libc,      \
              from, from_end_off, from_next_off,     \
              res, res_end_off, res_next_off,        \
              std::codecvt_base::result)

    //    +----------------------------------- source sequence (from)
    //    |          +------------------------ from_end offset from from
    //    |          |  +--------------------- expected from_next offset
    //    |          |  |  +------------------ expected destination sequence
    //    |          |  |  |         +-------- to_end offset from to
    //    |          |  |  |         |  +----- expected to_next offset
    //    |          |  |  |         |  |  +-- expected result (to)
    //    |          |  |  |         |  |  |
    //    V          V  V  V         V  V  V
    TEST (L"",       0, 0, "",       0, 0, ok);
    TEST (L"a",      1, 1, "a",      1, 1, ok);
    TEST (L"b",      1, 1, "b",      2, 1, ok);
    TEST (L"c",      1, 1, "c",      3, 1, ok);
    TEST (L"d",      1, 1, "d",      4, 1, ok);
    TEST (L"e",      1, 1, "e",      5, 1, ok);
    TEST (L"f",      1, 1, "f",      6, 1, ok);
    TEST (L"ab",     2, 2, "ab",     6, 2, ok);
    TEST (L"abc",    3, 3, "abc",    6, 3, ok);
    TEST (L"abcd",   4, 4, "abcd",   6, 4, ok);
    TEST (L"abcde",  5, 5, "abcde",  6, 5, ok);
    TEST (L"abcdef", 6, 6, "abcdef", 6, 6, ok);

    TEST (L"\n",     1, 1, "\n",     1, 1, ok);
    TEST (L"\n\377", 1, 1, "\n",     1, 1, ok);

    // exercise embedded NULs
    TEST (L"\0abcdef", 7, 7, "\0abcdef", 7, 7, ok);
    TEST (L"a\0bcdef", 7, 7, "a\0bcdef", 7, 7, ok);
    TEST (L"ab\0cdef", 7, 7, "ab\0cdef", 7, 7, ok);
    TEST (L"abc\0def", 7, 7, "abc\0def", 7, 7, ok);
    TEST (L"abcd\0ef", 7, 7, "abcd\0ef", 7, 7, ok);
    TEST (L"abcde\0f", 7, 7, "abcde\0f", 7, 7, ok);
    TEST (L"abcdef\0", 7, 7, "abcdef\0", 7, 7, ok);

    TEST (L"ab\0cd\0ef", 8, 8, "ab\0cd\0ef", 8, 8, ok);

    //////////////////////////////////////////////////////////////////
    // exercise sequences containing 2-byte characters

    if (mb_cur_max < 2) {
        rw_warn (0, 0, __LINE__, "no multibyte characters found");
        return;
    }

    size_t bufsize = 0;
    char *sequences = 0;

    // verify the length of each character
    for (std::size_t i = 0; i < mb_cur_max; ++i) {
        const std::size_t mb_len = std::strlen (mb_chars [i].mbchar);

        if (i + 1 != mb_len) {
            rw_assert (0, 0, __LINE__,
                       "unexpected multibyte character length: "
                       "%u, expected %u", mb_len, i + 1);
            return;
        }

        rw_asnprintf (&sequences, &bufsize,
                      "%{+}%s{ %{#lc}, %{#s} }",
                      i ? ", " : "",
                      mb_chars [i].wchar,
                      mb_chars [i].mbchar);
    }

    rw_info (0, 0, __LINE__,
             "locale (\"%s\") [libc-based encoding, "
             "MB_CUR_MAX = %u, multi-byte characters: %s]",
             locname, mb_cur_max, (const char*)sequences);

    std::free (sequences);

#ifdef _RWSTD_OS_SUNOS

    if (0 == std::strcmp ("5.7", _RWSTD_OS_RELEASE)) {

        rw_warn (0, 0, __LINE__, "skipping tests due to a SunOS 5.7 libc bug");
        return;
    }

#endif   // _RWSTD_OS_SUNOS

    wchar_t wstr [256];
    char    mbstr [256];

#undef TEST
#define TEST(pat, from_end_off, from_next_off,       \
             res_end_off, res_next_off, result)      \
    make_strings (pat, sizeof pat - 1,               \
                  wstr, mbstr, mb_chars);            \
    test_out (__LINE__, "wchar_t", 0, cvt_libc,      \
              wstr, from_end_off, from_next_off,     \
              mbstr, res_end_off, res_next_off,      \
              std::codecvt_base::result)

    // %N for N in [0, MB_CUR_MAX) represents a wide character
    // whose multibyte representation is (N + 1) bytes long
    // any other (narrow) character, including the NUL, is
    // widened to a wchar_t as if by an ordinary cast

    //    +------------------------------ source sequence (from)
    //    |               +-------------- initial (from_end - from)
    //    |               |  +----------- expected (from_next - from)
    //    |               |  |  +-------- initial (to_limit - to)
    //    |               |  |  |  +----- expected (to_next - to)
    //    |               |  |  |  |  +-- expected conversion result
    //    |               |  |  |  |  |
    //    V               V  V  V  V  V
    TEST ("%0",           0, 0, 0, 0, ok);
    TEST ("%0",           1, 0, 0, 0, partial);
    TEST ("%0",           1, 1, 1, 1, ok);
    TEST ("%1",           1, 0, 0, 0, partial);
    TEST ("%1",           1, 0, 1, 0, partial);
    TEST ("%1",           1, 1, 2, 2, ok);

    TEST ("a%1",          2, 1, 2, 1, partial);
    TEST ("b%1",          2, 2, 3, 3, ok);

    TEST ("%1%1",         2, 0, 1, 0, partial);
    TEST ("%1%1",         2, 1, 2, 2, partial);
    TEST ("%1%1",         2, 1, 3, 2, partial);
    TEST ("%1%1",         2, 2, 4, 4, ok);

    TEST ("%1X%1Y%1Z",    6, 0, 1, 0, partial);
    TEST ("%1X%1Y%1Z",    6, 1, 2, 2, partial);
    TEST ("%1X%1Y%1Z",    6, 2, 3, 3, partial);
    TEST ("%1X%1Y%1Z",    6, 2, 4, 3, partial);
    TEST ("%1X%1Y%1Z",    6, 3, 5, 5, partial);
    TEST ("%1X%1Y%1Z",    6, 4, 6, 6, partial);
    TEST ("%1X%1Y%1Z",    6, 4, 7, 6, partial);
    TEST ("%1X%1Y%1Z",    6, 5, 8, 8, partial);
    TEST ("%1X%1Y%1Z",    6, 6, 9, 9, ok);

    // exercise embedded NULs
    TEST ("\0",           1, 1, 1, 1, ok);
    TEST ("\0X",          2, 2, 2, 2, ok);
    TEST ("\0X\0",        3, 3, 3, 3, ok);
    TEST ("\0X\0Y",       4, 4, 4, 4, ok);
    TEST ("\0X\0Y\0",     5, 5, 5, 5, ok);
    TEST ("\0X\0Y\0Z",    6, 6, 6, 6, ok);
    TEST ("\0XYZ\0",      5, 5, 5, 5, ok);

    TEST ("%1\0",         2, 2, 3, 3, ok);
    TEST ("\0%1",         2, 2, 3, 3, ok);

    TEST ("%1\0%1",       3, 3, 5, 5, ok);
    TEST ("\0%1\0",       3, 3, 4, 4, ok);

    TEST ("%1\0%1\0%1\0", 6, 2, 3, 3, partial);
    TEST ("%1\0%1\0%1\0", 6, 3, 5, 5, partial);
    TEST ("%1\0%1\0%1\0", 6, 4, 6, 6, partial);
    TEST ("%1\0%1\0%1\0", 6, 5, 8, 8, partial);
    TEST ("%1\0%1\0%1\0", 6, 6, 9, 9, ok);

    // exercise consecutive embedded NULs
    TEST ("\0\0%1\0\0%1\0\0", 8, 8, 10, 10, ok);
    TEST ("\0\0\0\0\0\0\0XY", 9, 9,  9,  9, ok);

    //////////////////////////////////////////////////////////////////
    // exercise sequences containing 3-byte characters

    if (mb_cur_max < 3)
        return;

    TEST ("%2",   1, 0, 0, 0, partial);
    TEST ("%2",   1, 0, 1, 0, partial);
    TEST ("%2",   1, 0, 2, 0, partial);
    TEST ("%2",   1, 1, 3, 3, ok);

    TEST ("%2%1", 2, 0, 0, 0, partial);
    TEST ("%2%1", 2, 0, 1, 0, partial);
    TEST ("%2%1", 2, 0, 2, 0, partial);
    TEST ("%2%1", 2, 1, 3, 3, partial);
    TEST ("%2%1", 2, 1, 4, 3, partial);
    TEST ("%2%1", 2, 2, 5, 5, ok);

    TEST ("%2%2", 2, 0, 0, 0, partial);
    TEST ("%2%2", 2, 0, 1, 0, partial);
    TEST ("%2%2", 2, 0, 2, 0, partial);
    TEST ("%2%2", 2, 1, 3, 3, partial);
    TEST ("%2%2", 2, 1, 4, 3, partial);
    TEST ("%2%2", 2, 1, 5, 3, partial);
    TEST ("%2%2", 2, 2, 6, 6, ok);

    TEST ("%2%1%2", 3, 0, 0, 0, partial);
    TEST ("%2%1%2", 3, 0, 1, 0, partial);
    TEST ("%2%1%2", 3, 0, 2, 0, partial);
    TEST ("%2%1%2", 3, 1, 3, 3, partial);
    TEST ("%2%1%2", 3, 1, 4, 3, partial);
    TEST ("%2%1%2", 3, 2, 5, 5, partial);
    TEST ("%2%1%2", 3, 2, 6, 5, partial);
    TEST ("%2%1%2", 3, 2, 7, 5, partial);
    TEST ("%2%1%2", 3, 3, 8, 8, ok);

    TEST ("%2%1%2%0%1%2$", 6, 6, 14, 14, ok);

    // exercise embedded NULs
    TEST ("\0%2$",       2, 2, 4, 4, ok);
    TEST ("%2\0$",       2, 2, 4, 4, ok);
    TEST ("\0\0%2$",     3, 3, 5, 5, ok);
    TEST ("%2\0\0$",     3, 3, 5, 5, ok);
    TEST ("\0\0%2\0%2$", 5, 5, 9, 9, ok);
    TEST ("\0%2\0\0%2$", 5, 5, 9, 9, ok);
    TEST ("\0%2\0%2\0$", 5, 5, 9, 9, ok);
    TEST ("%2\0\0%2\0$", 5, 5, 9, 9, ok);
    TEST ("%2\0%2\0\0$", 5, 5, 9, 9, ok);

    //////////////////////////////////////////////////////////////////
    // exercise sequences containing 4-byte characters

    if (mb_cur_max < 4)
        return;

    TEST ("%3",     1, 0,  0,  0, partial);
    TEST ("%3",     1, 0,  1,  0, partial);
    TEST ("%3",     1, 0,  2,  0, partial);
    TEST ("%3",     1, 0,  3,  0, partial);
    TEST ("%3",     1, 1,  4,  4, ok);

    TEST ("%3%3",   2, 0,  3,  0, partial);
    TEST ("%3%3",   2, 1,  4,  4, partial);
    TEST ("%3%3",   2, 1,  5,  4, partial);
    TEST ("%3%3",   2, 1,  6,  4, partial);
    TEST ("%3%3",   2, 1,  7,  4, partial);
    TEST ("%3%3",   2, 2,  8,  8, ok);

    TEST ("%3G%3",  3, 2,  8,  5, partial);
    TEST ("%3G%3",  3, 3,  9,  9, ok);

    TEST ("%3%1%3", 3, 2,  9,  6, partial);
    TEST ("%3%1%3", 3, 3, 10, 10, ok);

    TEST ("%3%2%1%0%1%2%3", 7, 7, 19, 19, ok);

    //      4+1+3+1+2+1+1+1+2+1+3+1+4 = 25 bytes (13 wchar_t)
    TEST ("%3\0%2\0%1\0%0\0%1\0%2\0%3$", 13, 13, 25, 25, ok);
    //      4+1+1+3+2+1+1+1+2+1+3+4+1 = 25 bytes (13 wchar_t)
    TEST ("%3\0\0%2%1\0%0\0%1\0%2%3\0$", 13, 13, 25, 25, ok);
    //      1+4+3+1+2+1+1+1+2+1+3+4+1 = 25 bytes (13 wchar_t)
    TEST ("\0%3%2\0%1\0\0%0%1\0%2%3\0$", 13, 13, 25, 25, ok);
    //      1+4+3+1+2+1+2+1+3+4+1+1+1 = 25 bytes (13 wchar_t)
    TEST ("\0%3%2\0%1%0%1\0%2%3\0\0\0$", 13, 13, 25, 25, ok);
    //      1+1+1+1+1+1+4+3+2+1+2+3+4  = 25 bytes (13 wchar_t)
    TEST ("\0\0\0\0\0\0%3%2%1%0%1%2%3$", 13, 13, 25, 25, ok);
}

/****************************************************************************/

static void
test_wcodecvt_byname ()
{
    rw_info (0, 0, __LINE__,
             "std::codecvt_byname<wchar_t, char, mbstate_t>::out("
             "state_type&, "
             "const intern_type*, const intern_type*, "
             "const intern_type*, "
             "extern_type*, extern_type*, exterm_type*&)");

#if TEST_RW_EXTENSIONS
    // Only have classic C locale implemented.
    test_wcodecvt_byname_algorithmic ();
    // 'localedef' not working on these machines. Cannot create locale.
    test_wcodecvt_byname_table_based ();
#endif
    test_wcodecvt_byname_libc_based ();
}

#endif   // _RWSTD_NO_WCHAR_T

/****************************************************************************/

static int
run_test (int, char**)
{
    // set up RWSTD_LOCALE_ROOT and other environment variables
    // here as opposed to at program startup to work around a
    // SunOS 5.7 bug in putenv() (PR #30017)
    locale_root = rw_set_locale_root ();

    test_codecvt ();
    test_codecvt_byname ();

#ifndef _RWSTD_NO_WCHAR_T

    // exercise wchar_t specializations
    test_wcodecvt ();
    test_wcodecvt_byname ();

#else   // if defined (_RWSTD_NO_WCHAR_T)

    rw_warn (0, 0, __LINE__, "_RWSTD_NO_WCHAR_T #defined, cannot test");

#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}

/****************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.locale.codecvt.virtuals",
                    0 /* no comment */,
                    run_test,
                    "",
                    (void*)0   /* sentinel */);
}
