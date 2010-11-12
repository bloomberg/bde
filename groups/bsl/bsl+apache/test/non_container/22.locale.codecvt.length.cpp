/***************************************************************************
 * 
 * codecvt_length.cpp - test exercising the std::codecvt::length()
 *
 * $Id: 22.locale.codecvt.length.cpp 604041 2007-12-13 21:43:43Z sebor $
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
 * Copyright 2004-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <bsls_platform.h>

#ifdef __SUNPRO_CC
    // working around a SunPro/SunOS 5.8 bug (PR #26255)
#  include <time.h>
#endif   // __SUNPRO_CC

#include <locale>    // for codecvt

#include <climits>   // for MB_LEN_MAX
#include <clocale>   // for LC_CTYPE, setlocale()
#include <cstdlib>   // for MB_CUR_MAX, free(), size_t
#include <cstring>   // for strcpy(), strlen()
#include <cwchar>    // for mbstate_t

#include <any.h>         // for rw_any
#include <cmdopt.h>      // for rw_enabled()
#include <driver.h>      // for rw_test()
#include <file.h>        // for rw_fwrite()
#include <rw_locale.h>   // for rw_localedef(), rw_find_mb_locale()
#include <rw_printf.h>   // for rw_printf()

/****************************************************************************/

#if TEST_RW_EXTENSIONS
typedef std::codecvt<char, char, std::mbstate_t>           Codecvt;
typedef std::codecvt_byname<char, char, std::mbstate_t>    CodecvtByname;

#ifndef _RWSTD_NO_WCHAR_T

typedef std::codecvt<wchar_t, char, std::mbstate_t>        WCodecvt;
typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> WCodecvtByname;

#endif   // _RWSTD_NO_WCHAR_T

#else // if ! TEST_RW_EXTENSIONS

typedef std::codecvt<char, char, std::mbstate_t>           Codecvt;

struct CodecvtByname : std::codecvt_byname<char, char, std::mbstate_t>
{
    typedef std::codecvt_byname<char, char, std::mbstate_t> Base;
    CodecvtByname(const char* s) : Base(s) { }
    ~CodecvtByname() { }
};

#ifndef _RWSTD_NO_WCHAR_T

typedef std::codecvt<wchar_t, char, std::mbstate_t>        WCodecvt;

struct WCodecvtByname : std::codecvt_byname<wchar_t, char, std::mbstate_t>
{
    typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> Base;
    WCodecvtByname(const char* s) : Base(s) { }
    ~WCodecvtByname() { }
};
#endif   // _RWSTD_NO_WCHAR_T

#endif // TEST_RW_EXTENSIONS

/****************************************************************************/

// the root of the locale directory (RWSTD_LOCALE_ROOT)
// set in main() instead of here to avoid Solaris 7 putenv() bug (PR #30017)
const char* locale_root /* = set in main() */;


// creates a table-based multibyte locale
const char* create_locale ()
{
    char cm_fname [1024];
    if (rw_snprintf (cm_fname, sizeof cm_fname, "%s%c%s",
                     locale_root, _RWSTD_PATH_SEP, "charmap") < 0)
        return 0;

    static const char charmap[] = {
        "<code_set_name> test_charmap\n"
        "<comment_char> %\n"
        "<escape_char> /\n"
        "<mb_cur_min> 1\n"
        "<mb_cur_max> 9\n"
        "CHARMAP\n"
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
        "<U0010>   /x41 A \n"
        "<U0011>   /x42 B \n"
        "<U0012>   /x43 C \n"
        "END CHARMAP\n"
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

template <class internT>
void test_length (internT                                         /* dummy */,
                  int                                                line,
                  const std::mbstate_t                              *pstate,
                  const std::codecvt<internT, char, std::mbstate_t> &cvt,
                  const char                                        *from,
                  std::size_t                                        nchars,
                  int                                                maxi,
                  int                                                result)
{
    static const std::mbstate_t initial_state = std::mbstate_t ();

    const char* const tname = rw_any_t (internT ()).type_name ();

    std::mbstate_t state = pstate ? *pstate : initial_state;

    if (std::size_t (-1) == nchars)
        nchars = std::strlen (from);

    const int res = cvt.length (state, from, from + nchars, maxi);

    rw_assert (res == result, 0, line,
               "line %d: codecvt<%s, char, mbstate_t>::length("
               "state, from=%{*s}, from + %zu, %d) == %d, got %d",
               __LINE__, tname, sizeof *from, from, nchars, maxi, result, res);

    rw_assert (!pstate || 0 == std::memcmp (pstate, &state, sizeof state),
               0, line,
               "line %d: codecvt<%s, char, mbstate_t>::length("
               "state, from=%{*s}, from + %zu, %d) unexpected state",
               __LINE__, tname, from, nchars, maxi);
}

/****************************************************************************/

void test_codecvt (const Codecvt *pcvt = 0)
{
    if (0 == pcvt)
        rw_info (0, 0, 0,
                 "std::codecvt<char, char, mbstate_t>::length "
                 "(state_type&, const extern_type*, const "
                 "extern_type*, size_t)");

    const std::locale classic = std::locale::classic ();

    const Codecvt &cvt = pcvt ? *pcvt : std::use_facet<Codecvt>(classic);

#undef TEST
#define TEST(from, nchars, maxi, result) \
    test_length (char (), __LINE__, 0, cvt, from, nchars, maxi, result)

    //    +--------------- source sequence of externT characters
    //    |      +-------- size of sequence in externT characters
    //    |      |  +----- maximum number of internT characters
    //    |      |  |  +-- expected result
    //    |      |  |  |
    //    V      V  V  V
    TEST (0,     0, 0, 0);
    TEST ("",    0, 0, 0);
    TEST ("a",   1, 0, 0);
    TEST ("ab",  2, 1, 1);
    TEST ("ab",  2, 2, 2);
    TEST ("ab",  2, 3, 2);
    TEST ("abc", 3, 0, 0);
    TEST ("abc", 3, 1, 1);
    TEST ("abc", 3, 2, 2);
    TEST ("abc", 3, 3, 3);
    TEST ("abc", 3, 4, 3);
}

/****************************************************************************/

static void
test_codecvt_byname ()
{
    rw_info (0, 0, 0,
             "std::codecvt_byname<char, char, mbstate_t>::length "
             "(state_type&, const extern_type*, const extern_type*, "
             "size_t)");

    const CodecvtByname cvt ("");

    test_codecvt (&cvt);

}

/****************************************************************************/

static void
test_wcodecvt ()
{
    rw_info (0, 0, 0,
             "std::codecvt<wchar_t, char, mbstate_t>::length "
             "(state_type&, const extern_type*, const extern_type*, "
             "size_t)");

#ifndef _RWSTD_NO_WCHAR_T

    const std::locale classic = std::locale::classic ();

    const WCodecvt &cvt = std::use_facet<WCodecvt>(classic);

#undef TEST
#define TEST(from, nchars, maxi, result) \
    test_length (wchar_t (), __LINE__, 0, cvt, from, nchars, maxi, result)

    //    +--------------- source sequence of externT characters
    //    |      +-------- size of sequence in externT characters
    //    |      |  +----- maximum number of internT characters
    //    |      |  |  +-- expected result
    //    |      |  |  |
    //    V      V  V  V
    TEST (0,     0, 0, 0);
    TEST ("",    0, 0, 0);
    TEST ("a",   1, 0, 0);
    TEST ("ab",  2, 1, 1);
    TEST ("ab",  2, 2, 2);
    TEST ("ab",  2, 3, 2);
    TEST ("abc", 3, 0, 0);
    TEST ("abc", 3, 1, 1);
    TEST ("abc", 3, 2, 2);
    TEST ("abc", 3, 3, 3);
    TEST ("abc", 3, 4, 3);

#else   // if defined (_RWSTD_NO_WCHAR_T)

    rw_warn (0, 0, __LINE__, "_RWSTD_NO_WCHAR_T #defined, cannot test");

#endif   // _RWSTD_NO_WCHAR_T

}

/****************************************************************************/

#ifndef _RWSTD_NO_WCHAR_T

// exercises an algorithmic multibyte encoding
static void
test_wcodecvt_byname_algorithmic ()
{
    rw_info (0, 0, 0, "locale (\"UTF-8@UCS\") [algorithmic encoding]");

    // lowercase utf ==> relaxed checking (i.e., some, but not all,
    // invalid UTF-8 sequence are accepted)
    const WCodecvtByname cvt_relaxd ("utf-8@UCS");

    // capital UTF ==> strict checking
    const WCodecvtByname cvt_strict ("UTF-8@UCS");

#undef STRICT
#define STRICT(from, nc, maxi, res) \
    test_length (wchar_t (), __LINE__, 0, cvt_strict, from, nc, maxi, res)

#undef RELAXD
#define RELAXD(from, nc, maxi, res) \
    test_length (wchar_t (), __LINE__, 0, cvt_relaxd, from, nc, maxi, res)

#undef TEST
#define TEST(from, nc, maxi, res) \
    STRICT (from, nc, maxi, res); \
    RELAXD (from, nc, maxi, res)

    // 22.2.1.5.2  [lib.locale.codecvt.virtuals]
    // including the resolution of lwg issue 305
    //
    // -9-  Preconditions: (from<=from_end) well-defined and true; state
    //      initialized, if at the beginning of a sequence, or else equal
    //      to the result of converting the preceding characters in the
    //      sequence.
    //
    // -9a- Effects: The effect on the state argument is "as if" it called
    //      do_in(state, from, from_end, from, to, to+max, to) for to pointing
    //      to a buffer of at least max elements.
    //
    // -10- Returns: (from_next-from) where from_next is the largest value
    //      in the range [from,from_end] such that the sequence of values
    //      in the range [from,from_next) represents max or fewer valid
    //      complete characters of type internT. The instantiation
    //      codecvt<char, char, mbstate_t> returns the lesser of max
    //      and (from_end-from).

    // Note that the function returns the number of externT characters
    // (i.e., those of type char for the required instantiations)

    //    +--------------- source sequence of externT characters
    //    |      +-------- size of sequence in externT characters
    //    |      |  +----- maximum number of internT characters
    //    |      |  |  +-- expected result in externT characters
    //    |      |  |  |
    //    V      V  V  V
    TEST (0,     0, 0, 0);
    TEST ("",    0, 0, 0);
    TEST ("a",   1, 0, 0);
    TEST ("ab",  2, 1, 1);
    TEST ("ab",  2, 2, 2);
    TEST ("ab",  2, 3, 2);
    TEST ("abc", 3, 0, 0);
    TEST ("abc", 3, 1, 1);
    TEST ("abc", 3, 2, 2);
    TEST ("abc", 3, 3, 3);
    TEST ("abc", 3, 4, 3);

    // invalid sequences rejected in both the strict and relaxed mode
    TEST ("\x80",     1, 0, 0);
    TEST ("\xc0",     1, 0, 0);
    TEST ("\x80\x00", 2, 0, 0);
    TEST ("\xc0\x00", 2, 0, 0);

    // valid 2-byte UTF-8 sequences (except for overlong sequences)
    // 110x xxxx 10xx xxxx
    // i.e., first byte:  c0-df
    //       second byte: 80-bf
    TEST ("\xc2",     1, 0, 0);
    TEST ("\xc2\x81", 1, 1, 0);
    TEST ("\xc2\x82", 2, 1, 2);
    TEST ("\xc2\x83", 2, 2, 2);

    // the second byte doesn't follow the correct pattern
    // and will be rejected in strict mode (but will be
    // accepted in relaxed mode)
    STRICT ("\xc2\x01", 2, 1, 0);
    RELAXD ("\xc2\x01", 2, 1, 2);

    TEST ("\xc2\x80\xc0",     3, 0, 0);
    TEST ("\xc2\x80\xc0\x81", 3, 1, 2);
    TEST ("\xc2\x80\xc0\x82", 3, 2, 2);
    
    STRICT ("\xc2\x80\xc2\x01", 4, 2, 2);
    RELAXD ("\xc2\x80\xc2\x01", 4, 2, 4);

    TEST ("\xc2\x80\xc2\x81", 4, 0, 0);
    TEST ("\xc2\x80\xc2\x82", 4, 1, 2);
    TEST ("\xc2\x80\xc2\x83", 4, 2, 4);
    TEST ("\xc2\x80\xc2\x84", 4, 3, 4);
}


// exercises a table-based multibyte encoding
static void
test_wcodecvt_byname_table_based ()
{
    const char* const locname = create_locale ();

    if (!rw_error (0 != locname, 0, __LINE__,
                   "failed to create a locale database")) {
        return;
    }

    std::locale loc;

    _TRY {
        loc = std::locale (locname);
    }
    _CATCH (...) {
        rw_error (0, 0, __LINE__,
                  "locale(\"%s\") unexpectedly threw an exception", locname);
        return;
    }

    const WCodecvt &cvt_table = std::use_facet<WCodecvt>(loc);

    rw_info (0, 0, 0, "locale (\"%s\") [table-based encoding]", locname);

#undef TEST
#define TEST(from, nc, maxi, res)                       \
    test_length (wchar_t (), __LINE__, 0, cvt_table,    \
                 from, std::size_t (nc), maxi, res)

    TEST (0,     0, 0, 0);
    TEST ("",    0, 0, 0);
    TEST ("A",   1, 0, 0);
    TEST ("AB",  2, 1, 1);
    TEST ("AB",  2, 2, 2);
    TEST ("AB",  2, 3, 2);
    TEST ("ABC", 3, 0, 0);
    TEST ("ABC", 3, 1, 1);
    TEST ("ABC", 3, 2, 2);
    TEST ("ABC", 3, 3, 3);
    TEST ("ABC", 3, 4, 3);

    TEST ("22", 1, 1, 0);   // "22" --> L'2'
    TEST ("22", 2, 1, 2);
    TEST ("22", 2, 2, 2);

    TEST ("333", 1, 1, 0);   // "333" --> L'3'
    TEST ("333", 2, 1, 0);
    TEST ("333", 3, 1, 3);
    TEST ("333", 3, 2, 3);
 
    TEST ("4444", 1, 1, 0);   // "4444" --> L'4'
    TEST ("4444", 2, 1, 0);
    TEST ("4444", 3, 1, 0);
    TEST ("4444", 4, 1, 4);
    TEST ("4444", 4, 2, 4);

    TEST ("122", 1, 0, 0);   // "122" --> L"12"
    TEST ("122", 1, 1, 1);
    TEST ("122", 1, 2, 1);
    TEST ("122", 2, 2, 1);
    TEST ("122", 3, 2, 3);
    TEST ("122", 3, 3, 3);

    TEST ("122333", 1, 0, 0);   // "122333" --> L"123"
    TEST ("122333", 1, 1, 1);
    TEST ("122333", 1, 2, 1);
    TEST ("122333", 4, 1, 1);
    TEST ("122333", 4, 2, 3);
    TEST ("122333", 4, 3, 3);
    TEST ("122333", 5, 1, 1);
    TEST ("122333", 5, 2, 3);
    TEST ("122333", 5, 3, 3);
    TEST ("122333", 5, 1, 1);
    TEST ("122333", 5, 2, 3);
    TEST ("122333", 5, 3, 3);
    TEST ("122333", 6, 1, 1);
    TEST ("122333", 6, 2, 3);
    TEST ("122333", 6, 3, 6);
    TEST ("122333", 6, 4, 6);

    //     0   12    3      45      67  89
    // I:  +---++----+------++------++--++- (intern_type characters)
    // E:  0....:....1....:....2....:....3. (extern_type characters)
    TEST ("4444A55555B666666C77777770333122", -1,  1,  4);
    TEST ("4444A55555B666666C77777770333122", -1,  2,  5);
    TEST ("4444A55555B666666C77777770333122", -1,  3, 10);
    TEST ("4444A55555B666666C77777770333122", -1,  4, 11);
    TEST ("4444A55555B666666C77777770333122", -1,  5, 17);
    TEST ("4444A55555B666666C77777770333122", -1,  6, 18);
    TEST ("4444A55555B666666C77777770333122", -1,  7, 25);
    TEST ("4444A55555B666666C77777770333122", -1,  8, 26);
    TEST ("4444A55555B666666C77777770333122", -1,  9, 29);
    TEST ("4444A55555B666666C77777770333122", -1, 10, 30);
    TEST ("4444A55555B666666C77777770333122", -1, 11, 32);

    TEST ("4444.55555B666666C77777770333122", -1, 11,  4);
    TEST ("4444A55555.666666C77777770333122", -1, 11, 10);
    TEST ("4444A55555B666666.77777770333122", -1, 11, 17);
    TEST ("4444A55555B666666C7777777.333122", -1, 11, 25);
    TEST ("4444A55555B666666C77777770333.22", -1, 11, 29);
}


// exercises a libc-based multibyte encoding
static void
test_wcodecvt_byname_libc_based ()
{
    // compute `mb_cur_max' multibyte characters in increasing
    // length from 1 to mb_cur_max bytes long

    // i.e., initialize the first (N + 1) elements of mb_chars as follows:
    // mb_chars [0] = "0";         // where "0" is a single byte character
    // mb_chars [1] = "11";        // where "11" is a two-byte character
    // mb_chars [2] = "222";       // where "222" is a three-byte character
    // mb_chars [N] = "NNN...N";   // where "NNN...N" is an N-byte character
    
    std::size_t mb_cur_max = 0;

    rw_mbchar_array_t mb_chars;

#if TEST_RW_EXTENSIONS
    // Only classic 'C' locale defined in stlport.
    const char* const locname = rw_find_mb_locale (&mb_cur_max, mb_chars);
#else
    const char* const locname = "C\0";
#endif

    if (!rw_warn (0 != locname, 0, __LINE__,
                  "failed to find a multibyte locale")) {
        return;
    }

    std::locale loc;

    _TRY {
        loc = std::locale (locname);
    }
    _CATCH (...) {
        rw_error (0, 0, __LINE__,
                  "locale(\"%s\") unexpectedly threw an exception", locname);
        return;
    }

    const WCodecvt &cvt_libc = std::use_facet<WCodecvt>(loc);

    rw_info (0, 0, 0, "locale (\"%s\") [libc-based encoding, "
             "single-byte characters]", locname);

    //////////////////////////////////////////////////////////////////
    // exercise sequences containing single-byte characters

#undef TEST
#define TEST(from, nc, maxi, res)                       \
    test_length (wchar_t (), __LINE__, 0, cvt_libc,     \
                 from, std::size_t (nc), std::size_t (maxi), res)

    TEST (0,      0, 0, 0);
    TEST ("",     0, 0, 0);
    TEST ("A",    1, 0, 0);
    TEST ("AB",   2, 1, 1);
    TEST ("AB",   2, 2, 2);
    TEST ("AB",   2, 3, 2);
    TEST ("ABC",  3, 0, 0);
    TEST ("ABC",  3, 1, 1);
    TEST ("ABC",  3, 2, 2);
    TEST ("ABC",  3, 3, 3);
    TEST ("ABC",  3, 4, 3);

    // exercise embedded NULs
    TEST ("\0BC",   3, 3, 3);
    TEST ("A\0C",   3, 3, 3);
    TEST ("AB\0",   3, 3, 3);
    TEST ("\0\0C",  3, 3, 3);
    TEST ("A\0\0",  3, 3, 3);
    TEST ("\0B\0",  3, 3, 3);
    TEST ("\0\0\0", 3, 3, 3);

    //////////////////////////////////////////////////////////////////
    // exercise sequences containing 2-byte characters

    if (!rw_warn (2 <= mb_cur_max, 0, __LINE__,
                  "no multibyte characters found, skipping test")) {
        return;
    }

    char* sequences = 0;

    // verify the length of each character
    for (std::size_t i = 0; i < mb_cur_max; ++i) {
        const std::size_t mb_len = std::strlen (mb_chars [i]);

        if (!rw_error (i + 1 == mb_len, 0, __LINE__,
                       "unexpected multibyte character length: "
                       "%zu, expected %zu", mb_len, i + 1)) {
            return;
        }

        sequences = rw_sprintfa ("%s%s%#s",
                                 sequences ? sequences : "",
                                 i ? ", " : "", mb_chars [i]);
    }

    rw_info (0, 0, 0, "locale (\"%s\") [libc-based encoding, "
             "MB_CUR_MAX = %zu, multi-byte characters: %s]",
             locname, mb_cur_max, sequences);

    std::free (sequences);

#ifdef _RWSTD_OS_SUNOS

    if (!rw_warn (std::strcmp ("5.7", _RWSTD_OS_RELEASE), 0, __LINE__,
                  "skipping tests due to a SunOS 5.7 libc bug")) {
        return;
    }

#endif   // _RWSTD_OS_SUNOS

    char mb_string [256];

    // create a sequence of two multibyte characters
    rw_sprintf (mb_string, "%s%s", mb_chars [0], mb_chars [1]);

    //    +------------------- source sequence of multibyte externT characters
    //    |          +-------- lenght of externT sequence in chars (bytes)
    //    |          |  +----- maximum number of internT characters
    //    |          |  |  +-- expected result in externT characters (bytes)
    //    |          |  |  |
    //    V          V  V  V
    TEST (mb_string, 0, 0, 0);
    TEST (mb_string, 1, 1, 1);
    TEST (mb_string, 1, 1, 1);
    TEST (mb_string, 2, 1, 1);
    TEST (mb_string, 2, 2, 1);
    TEST (mb_string, 3, 1, 1);
    TEST (mb_string, 3, 2, 3);
    TEST (mb_string, 3, 3, 3);

    TEST (mb_string, 3, -1, 3);

    // exercise embedded NULs
    rw_sprintf (mb_string, "%c%s%s", '\0', mb_chars [0], mb_chars [1]);
    TEST (mb_string, 4, 3, 4);

    rw_sprintf (mb_string, "%s%c%s", mb_chars [0], '\0', mb_chars [1]);
    TEST (mb_string, 4, 3, 4);

    rw_sprintf (mb_string, "%s%s%c", mb_chars [0], mb_chars [1], '\0');
    TEST (mb_string, 4, 3, 4);

    rw_sprintf (mb_string, "%c%c%s", '\0', '\0', mb_chars [0]);
    TEST (mb_string, 3, 3, 3);

    rw_sprintf (mb_string, "%c%c%s", '\0', '\0', mb_chars [1]);
    TEST (mb_string, 4, 3, 4);

    //////////////////////////////////////////////////////////////////
    // exercise sequences containing 3-byte characters

    if (mb_cur_max < 3)
        return;

    // create a sequence of three multibyte characters, 3, 2,
    // and 1 byte long (and total length of (3+2+1)=6 bytes)
    // with the following pattern: "<333><22><1>"
    rw_sprintf (mb_string, "%s%s%s",
                mb_chars [2], mb_chars [1], mb_chars [0]);

    TEST (mb_string, 0,  3, 0);   // ""
    TEST (mb_string, 1,  3, 0);   // "3"
    TEST (mb_string, 2,  3, 0);   // "33"
    TEST (mb_string, 3,  3, 3);   // "333" -> 1 complete internT
    TEST (mb_string, 4,  3, 3);   // "3332"
    TEST (mb_string, 5,  3, 5);   // "33322" -> 2 complete internT's
    TEST (mb_string, 6,  3, 6);   // "333221" -> 3 complete internT's
    TEST (mb_string, 6,  4, 6);
    TEST (mb_string, 6, -1, 6);

    //////////////////////////////////////////////////////////////////
    // exercise sequences containing 4-byte characters

    if (mb_cur_max < 4)
        return;

    // create a sequence of four multibyte characters, 4, 3, 2,
    // and 1 byte long (and total length of (4+3+2+1)=10 bytes)
    // with the following pattern: "<4444><333><22><1>"
    rw_sprintf (mb_string, "%s%s%s%s",
                mb_chars [3], mb_chars [2], mb_chars [1], mb_chars [0]);

    TEST (mb_string, 0,   4,  0);   // ""
    TEST (mb_string, 1,   4,  0);   // "4"
    TEST (mb_string, 2,   4,  0);   // "44"
    TEST (mb_string, 3,   4,  0);   // "444"
    TEST (mb_string, 4,   4,  4);   // "4444" -> 1 complete internT
    TEST (mb_string, 5,   4,  4);   // "44443"
    TEST (mb_string, 6,   4,  4);   // "444433"
    TEST (mb_string, 7,   4,  7);   // "4444333" -> 2 complete internT's
    TEST (mb_string, 8,   4,  7);   // "44443332"
    TEST (mb_string, 9,   4,  9);   // "444433322" -> 3 complete internT's
    TEST (mb_string, 10,  4, 10);   // "4444333221" -> 4 complete internT's
    TEST (mb_string, 10,  5, 10);
    TEST (mb_string, 10, -1, 10);

    //////////////////////////////////////////////////////////////////
    // exercise sequences containing 5-byte characters

    if (mb_cur_max < 5)
        return;

    // create a sequence of five multibyte characters, 5, 4, 3, 2,
    // and 1 byte long (and total length of (5+4+3+2+1)=15 bytes)
    // with the following pattern: "<55555><4444><333><22><1>"
    rw_sprintf (mb_string, "%s%s%s%s%s",
                mb_chars [4], mb_chars [3], mb_chars [2], mb_chars [1],
                mb_chars [0]);

    TEST (mb_string,  0,  5,  0);   // ""
    TEST (mb_string,  1,  5,  0);   // "5"
    TEST (mb_string,  2,  5,  0);   // "55"
    TEST (mb_string,  3,  5,  0);   // "555"
    TEST (mb_string,  4,  5,  0);   // "5555"
    TEST (mb_string,  5,  5,  5);   // "55555"
    TEST (mb_string,  6,  5,  5);   // "555554"
    TEST (mb_string,  7,  5,  5);   // "5555544"
    TEST (mb_string,  8,  5,  5);   // "55555444"
    TEST (mb_string,  9,  5,  9);   // "555554444"
    TEST (mb_string, 10,  5,  9);   // "5555544443"
    TEST (mb_string, 11,  5,  9);   // "55555444433"
    TEST (mb_string, 12,  5, 12);   // "555554444333"
    TEST (mb_string, 13,  5, 12);   // "5555544443332"
    TEST (mb_string, 14,  5, 14);   // "55555444433322"
    TEST (mb_string, 15,  5, 15);   // "555554444333221"
    TEST (mb_string, 15,  6, 15);
    TEST (mb_string, 15, -1, 15);

    // create a sequence of five multibyte characters, each 5 bytes long
    // with the following pattern: "<55555><55555><55555><55555><55555>"
    rw_sprintf (mb_string, "%s%s%s%s%s",
                mb_chars [4], mb_chars [4], mb_chars [4], mb_chars [4],
                mb_chars [4]);

    TEST (mb_string,  5,  5,  5);   // "<55555>"
    TEST (mb_string,  6,  5,  5);   // "<55555><5"
    TEST (mb_string,  9,  5,  5);   // "<55555><5555"
    TEST (mb_string, 10,  1,  5);   // "<55555><55555>"
    TEST (mb_string, 10,  2, 10);
    TEST (mb_string, 10,  5, 10);
    TEST (mb_string, 11,  5, 10);   // "<55555><55555><5"
    TEST (mb_string, 14,  5, 10);   // "<55555><55555><5555"
    TEST (mb_string, 15,  5, 15);   // "<55555><55555><55555>"

    rw_sprintf (mb_string, "%s%s%s%s%s",
                mb_chars [4], mb_chars [0], mb_chars [4], mb_chars [4],
                mb_chars [4]);
    
    //                        internT: 0      1  2      3      4      5
    //                        externT: <-----><-><-----><-----><-----><
    //                                "<55555><1><55555><55555><55555>"
    TEST (mb_string,  5, 5,  5);   //  |----->  >      >      >      >
    TEST (mb_string,  6, 5,  6);   //  |-------->      >      >      >
    TEST (mb_string,  7, 5,  6);   //  |-----------    >      >      >
    TEST (mb_string,  8, 5,  6);   //  |------------   >      >      >
    TEST (mb_string,  9, 5,  6);   //  |-------------  >      >      >
    TEST (mb_string, 10, 5,  6);   //  |-------------- >      >      >
    TEST (mb_string, 11, 5, 11);   //  |--------------->      >      >
    TEST (mb_string, 12, 5, 11);   //  |----------------      >      >
    TEST (mb_string, 15, 5, 11);   //  |--------------------- >      >
    TEST (mb_string, 16, 5, 16);   //  |---------------------->      >
    TEST (mb_string, 21, 5, 21);   //  |----------------------------->
}

#endif   // _RWSTD_NO_WCHAR_T


static void
test_wcodecvt_byname ()
{
    rw_info (0, 0, 0,
             "std::codecvt_byname<wchar_t, char, mbstate_t>::length "
             "(state_type&, const extern_type*, const extern_type*, "
             "size_t)");

#ifndef _RWSTD_NO_WCHAR_T

#if TEST_RW_EXTENSIONS
    // Only classic 'C' locale defined for stlport.
    test_wcodecvt_byname_algorithmic ();
    // 'localedef' not working on these machines. Cannot create locale.
    test_wcodecvt_byname_table_based ();
#endif
    test_wcodecvt_byname_libc_based ();

#else   // if defined (_RWSTD_NO_WCHAR_T)

    rw_warn (0, 0, __LINE__, "_RWSTD_NO_WCHAR_T #defined, cannot test");

#endif   // _RWSTD_NO_WCHAR_T

}

/****************************************************************************/

static int no_codecvt;
static int no_codecvt_byname;
static int no_wcodecvt;
static int no_wcodecvt_byname;

static int
run_test (int, char*[])
{
    // set up RWSTD_LOCALE_ROOT and other environment variables
    // here as opposed to at program startup to work around a
    // SunOS 5.7 bug in putenv() (PR #30017)
    locale_root = rw_set_locale_root ();

#undef TEST
#define TEST(what)                                              \
    if (no_ ## what) {                                          \
        rw_note (0, 0, __LINE__, "%s test disabled", #what);    \
    }                                                           \
    else {                                                      \
        test_ ## what ();                                       \
    } typedef void unused_typedef


    if (rw_enabled ("char")) {
        TEST (codecvt);
        TEST (codecvt_byname);
    }
    else {
        rw_note (0, 0, 0, "char tests disabled");
    }

    if (rw_enabled ("wchar_t")) {
        TEST (wcodecvt);
        TEST (wcodecvt_byname);
    }
    else {
        rw_note (0, 0, 0, "wchar_t tests disabled");
    }

    return 0;
}

/****************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.locale.codecvt.length",
                    0 /* no comment */, run_test,
                    "|-no-codecvt# "
                    "|-no-codecvt_byname# "
                    "|-no-wcodecvt# "
                    "|-no-wcodecvt_byname# ",
                    &no_codecvt,
                    &no_codecvt_byname,
                    &no_wcodecvt,
                    &no_wcodecvt_byname);
}
