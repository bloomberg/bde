/************************************************************************
 *
 * 27.istream.fmat.arith.cpp - test exercising basic_istream
 *                             arithmetic extractors
 *
 * $Id: 27.istream.fmat.arith.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 **************************************************************************/

#include <cstddef>
#include <istream>

#include <rw_char.h>
#include <rw_ctype.h>
#include <rw_exception.h>
#include <rw_printf.h>
#include <rw_streambuf.h>
#include <driver.h>

/***********************************************************************/

struct LocaleData {
    const char *whitespace;
    int         decimal_point;
    int         thousands_sep;
    const char *grouping;
    const char *truename;
    const char *falsename;
};


template <class CharT>
const std::locale
make_locale (const CharT*, const LocaleData &data)
{
#if 255U == _RWSTD_UCHAR_MAX

    // work around a bogus gcc error: ISO C++ forbids variable-size
    // array (see https://issues.apache.org/jira/browse/STDCXX-351)
    static const std::size_t N = 256;

#else   // if UCHAR_MAX != 255

    // determine the number of classic characters
    static const std::size_t N = std::ctype<char>::table_size;

#endif

    // derive a class so we can call the protected classic_table()
    struct ClassicTable: std::ctype<char> {
        static const std::ctype_base::mask* get () {
            return classic_table ();
        }
    };

    // get the classic C table of masks
    const std::ctype_base::mask* const classic = ClassicTable::get ();

    static int chars [N + 1];
    static int masks [N + 1];

    // -1 denotes end of characters (can't use 0 since it's a valid value)
    chars [N] = -1;

    // copy the classic table clearing the space bit of each mask
    for (std::size_t i = 0; i != N; ++i) {
        chars [i] = int (i);
        masks [i] = classic [i] & ~std::ctype_base::space;
    }

    typedef unsigned char UChar;

    // set the space bit for all characters in whitespace
    for (const char *ws = data.whitespace; *ws; ++ws)
        masks [UChar (*ws)] |= std::ctype_base::space;

    // create a user-defined ctype-derived facet and initialize
    // it with the character set and the specified masks
    FACET_CONST std::ctype<CharT>* const ctp =
        new UserCtype<CharT>(chars, masks);

    struct NumPunct: std::numpunct<CharT> {

        // CharType typedef works around a Sun C++ bug stdcxx-345
        typedef CharT                                          CharType;
        typedef std::numpunct<CharType>                        Base;
        typedef std::char_traits<CharType>                     Traits;
        typedef std::allocator<CharType>                       Allocator;
        typedef std::basic_string<CharType, Traits, Allocator> String;

        int             dp_;
        int             ts_;
        const char     *grp_;
        const CharType *fn_;
        const CharType *tn_;

        NumPunct (int dp, int ts, const char *grp,
                  const CharType *fn, const CharType *tn)
            : Base (), dp_ (dp), ts_ (ts), grp_ (grp ? grp : ""),
              fn_ (fn), tn_ (tn) { /* empty */ }

        CharType do_decimal_point () const {
            return -1 == dp_ ? Base::do_decimal_point ()
                              : make_char (char (dp_), (CharType*)0);
        }

        CharType do_thousands_sep () const {
            return -1 == ts_ ? Base::do_thousands_sep ()
                             : make_char (char (ts_), (CharType*)0);
        }

        std::string do_grouping () const {
            return 0 == grp_ ? Base::do_grouping ()
                             : std::string (grp_);
        }

        String do_truename () const {
            return 0 == tn_ ? Base::do_truename () : String (tn_);
        }

        String do_falsename () const {
            return 0 == fn_ ? Base::do_falsename () : String (fn_);
        }
    };

    static CharT names [2][64];

    names [false][0] = CharT ();
    names [true][0]  = CharT ();

    if (data.falsename) {
        for (std::size_t i = 0; data.falsename [i]; ++i) {
            names [false][i]     = data.falsename [i];
            names [false][i + 1] = CharT ();
        }
    }

    if (data.truename) {
        for (std::size_t i = 0; data.truename [i]; ++i) {
            names [true][i]     = data.truename [i];
            names [true][i + 1] = CharT ();
        }
    }

    // create a user-defined numpunct-derived facet and initialize
    // its members with the specified parameters
    FACET_CONST std::numpunct<CharT>* const np =
        new NumPunct (data.decimal_point,
                      data.thousands_sep,
                      data.grouping,
                      data.falsename ? names [false] : 0,
                      data.truename ? names [true] : 0);

    // install the new nupunct facet in a temporary locale,
    // replacing the original (if CharT=char or CharT=wchar_t)
    const std::locale tmp (std::locale::classic (), np);

    // install the new ctype facet in a locale, replacing
    // the original (if CharT=char or CharT=wchar_t)
    return std::locale (tmp, ctp);
}

/***********************************************************************/

template <class CharT, class Traits>
const std::locale
make_locale (const CharT*, const Traits*,  const LocaleData &data)
{
    // construct a locale with the appropriate ctype and numpunct
    // facets installed
    const std::locale tmp (make_locale ((CharT*)0, data));

    typedef std::istreambuf_iterator<CharT, Traits> Iterator;
    typedef std::num_get<CharT, Iterator>           NumGet;

    // check to see if num_get for this charater type and traits
    // is installed and install it if it isn't (i.e., when Traits
    // is other than std::char_traits<CharT>)
    if (std::has_facet<NumGet>(tmp))
        return tmp;

    return std::locale (tmp, new NumGet);
}

/***********************************************************************/

/* extern */ int opt_no_gcount;


template <class CharT, class Traits, class ArithmeticType>
void
test_extractor (CharT*, Traits*, ArithmeticType*,
                const char     *cname,
                const char     *tname,
                const char     *aname,
                int             line,
                // printf formatting directive for ArithmeticType
                const char *valfmt,
                // character buffer (input sequence)
                const char *cbuf,
                // number of characters in buffer:
                std::size_t cbuf_size,
                // ctype and numpunct data
                const LocaleData &locale_data,
                // stream flags():
                int flags,
                // initial stream rdstate():
                int init_state,
                // unmasked exceptions:
                int exceptions,
                // expected exception:
                int expect_exception,
                // expected stream state after extraction:
                int expect_state,
                // expected number of extracted characters:
                int expect_extract,
                // have streambuf fail (or throw) after so many calls
                // to underflow() (each call extracts a single chracter):
                int fail_when,
                // initial value of the argument to extractor:
                ArithmeticType init_value,
                // expected value of the argument after extraction:
                ArithmeticType expect_value)
{
    _RWSTD_UNUSED (cname);
    _RWSTD_UNUSED (tname);

    typedef std::basic_istream<CharT, Traits> Istream;
    typedef MyStreambuf<CharT, Traits>        Streambuf;

    const char *fail_desc = 0;

    int fail_how = 0;

    if (fail_when < 0) {
        // have the stream buffer object's underflow() fail (by throwing
        // an exception if possible) after `fail_when' characters have
        // been extracted (this object calls underflow() for every char)
        fail_how  = Underflow | Throw;
        fail_when = -fail_when;
        fail_desc = "threw";
    }
    else if (0 < fail_when) {
        // have the stream buffer object's underflow() fail by returning
        // eof after `fail_when' characters have been extracted (this
        // object calls underflow() for every char)
        fail_how  = Underflow;
        fail_desc = "returned EOF";
    }

    // construct a stream buffer object and initialize its read sequence
    // with the character buffer
    Streambuf sb (cbuf, cbuf_size, fail_how, fail_when);

    // construct an istream object and initialize it with the user
    // defined streambuf object
    Istream is (&sb);

    if (-1 == flags) {
        // get the initial stream object's format control flags
        flags = is.flags ();
    }
    else {
        // set the stream object's format control flags
        is.flags (std::ios::fmtflags (flags));
    }

    if (-1 == exceptions) {
        // get the initial stream object's exceptions
        exceptions = is.exceptions ();
    }
    else {
        // unmask the stream objects exceptions (must be done
        // before calling setstate() to prevent the latter from
        // throwing ios::failure)
        is.exceptions (std::ios::iostate (exceptions));
    }

    if (-1 == init_state) {
        // get the initial stream object's state
        init_state = is.rdstate ();
    }
    else {
        // set the stream object's initial state

#ifndef _RWSTD_NO_EXCEPTIONS
        try {
            is.setstate (std::ios::iostate (init_state));
        }
        catch (...) {
            // ignore exceptions
        }
#else   // if defined ( _RWSTD_NO_EXCEPTIONS)
        is.setstate (std::ios::iostate (init_state));
#endif   //  _RWSTD_NO_EXCEPTIONS
    }

    // construct a locale object that treats only the specified `white'
    // characters as whitespace (all others are treated normally)
    const std::locale loc =
        is.imbue (make_locale ((CharT*)0, (Traits*)0, locale_data));

    // imbue the previous locale into the stream buffer to verify that
    // the ws manipulator uses the locale imbued in the stream object
    // and not the one in the stream buffer
    sb.pubimbue (loc);

    // initialize the variable to the initial value to detect
    // the extractor setting it when it's not supposed to
    ArithmeticType value = init_value;

    // format the FUNCALL environment variable w/o writing out any output
    rw_fprintf (0,
                "%{$FUNCALL!:@}",
                "%{$CLASS}(%{*Ac}).operator>>(%s& = %{@}): "
                "initial flags() = %{If}, rdstate() = %{Is}, "
                "exceptions() = %{Is}, whitespace = %{#s}, numpunct = { "
                ".dp=%{#c}, .ts=%{#c}, .grp=%{#s}, .fn=%{#s}, .tn=%{#s} }",
                int (sizeof *cbuf), cbuf, aname, valfmt, init_value,
                flags, init_state,
                exceptions, locale_data.whitespace,
                locale_data.decimal_point, locale_data.thousands_sep,
                locale_data.grouping, locale_data.falsename,
                locale_data.truename);

#ifndef _RWSTD_NO_EXCEPTIONS

    int caught = 0;

    try {
        is >> value;
    }
    catch (Exception&) {
        caught = 1;
    }
    catch (std::ios_base::failure &ex) {
        caught = 2;
        rw_assert (caught == expect_exception, 0, line,
                   "line %d. %{$FUNCALL}: unexpectedly threw "
                   "ios_base::failure(%{#s})", __LINE__, ex.what ());
    }
    catch (...) {
        caught = -1;
        rw_assert (false, 0, line,
                   "line %d. %{$FUNCALL}: unexpectely threw an exception "
                   "of unknown type", __LINE__);
    }

    //////////////////////////////////////////////////////////////////
    // verify that the function propagates exceptions thrown from the
    // streambuf object only when badbit is set in the stream object's
    // exceptions()

    rw_assert (caught == expect_exception, 0, line,
               "line %d. %{$FUNCALL}: "
               "%{?}failed to throw"
               "%{:}unexpectedly propagated"
               "%{;} exception",
               __LINE__, expect_exception);

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    is >> value;

#endif   // _RWSTD_NO_EXCEPTIONS

    // clear the text describing the type of failure when streambuf
    // didn't actually fail (or throw)
    if (sb.failed_ == None && sb.threw_ == None)
        fail_desc = 0;

    //////////////////////////////////////////////////////////////////
    // verify that the expected number of characters have been
    // extracted from the stream

    const int extracted = int (sb.pubgptr () - sb.pubeback ());

    rw_assert (expect_extract == extracted, 0, line,
               "%d. %{$FUNCALL}: expected to extract %d characters, "
               "got %d%{?} (underflow() %s at extraction %u)%{;}",
               __LINE__, expect_extract,
               extracted, 0 != fail_desc, fail_desc, fail_when);

    //////////////////////////////////////////////////////////////////
    // verify that gcount() is not affected

    if (0 == opt_no_gcount)
        rw_assert (0 == is.gcount (), 0, line,
                   "%d. %{$FUNCALL}: gcount() == 0, got %d "
                   "%{?} (underflow() %s at extraction %u)%{;}",
                   __LINE__, is.gcount (),
                   0 != fail_desc, fail_desc, fail_when);

    //////////////////////////////////////////////////////////////////
    // verify the state of the stream object after the function call

    rw_assert (is.rdstate () == expect_state, 0, line,
               "line %d. %{$FUNCALL}: rdstate() == %{Is}, got %{Is}"
               "%{?} (underflow() %s at extraction %u)%{;}",
               __LINE__, expect_state, is.rdstate(),
               0 != fail_desc, fail_desc, fail_when);

    //////////////////////////////////////////////////////////////////
    // verify the extracted value matches the expected value

    rw_assert (expect_value == value, 0, line,
               "line %d. %{$FUNCALL}: expected value %{@}, got %{@}",
               __LINE__, valfmt, expect_value, valfmt, value);
}

/***********************************************************************/

// state abbreviations for brevity
const int ___ =                0;
const int __f =                0 |                0 | std::ios::failbit;
const int _e_ =                0 | std::ios::eofbit |                 0;
const int _ef =                0 | std::ios::eofbit | std::ios::failbit;
const int b__ = std::ios::badbit |                0 |                 0;
const int b_f = std::ios::badbit |                0 | std::ios::failbit;
const int be_ = std::ios::badbit | std::ios::eofbit |                 0;
const int bef = std::ios::badbit | std::ios::eofbit | std::ios::failbit;

const int DEC = std::ios::dec;
const int OCT = std::ios::oct;
const int HEX = std::ios::hex;


template <class CharT, class Traits, class ArithmeticType>
void
test_extractor (CharT*, Traits*, ArithmeticType*,
                const char *cname,
                const char *tname,
                const char *aname,
                const char *fmt)
{
    typedef ArithmeticType T;

    LocaleData locale_data = {
        0, -1, -1, 0, 0, 0
    };

    static const volatile T zero = T ();
    static const volatile T negative_one = T (-1);

    const bool is_signed   = negative_one < zero;
    const bool is_floating = zero > negative_one / 2;

    // avoid test cases designed to exercise the extraction of negative
    // values when ArithmeticType is an unsigned type
#undef TEST
#define TEST(str, fl, is, ex, ee, es, ne, fw, iv, ev)                        \
    if (is_signed || 0 <= ev)                                                \
        test_extractor ((CharT*)0, (Traits*)0, (T*)0, cname, tname, aname,   \
                        __LINE__, fmt, str, sizeof str - 1, locale_data,     \
                        fl, is, ex, ee, es, ne, fw, (T)(iv), (T)ev)

    locale_data.whitespace = "";

    //    +-- subject sequence
    //    |     +-- initial flags (-1 for default)
    //    |     |    +-- initial state (-1 for default)
    //    |     |    |   +-- exceptions (-1 for default)
    //    |     |    |   |  +-- expected exception
    //    |     |    |   |  |   +-- expected rdstate()
    //    |     |    |   |  |   |   +-- extracted characters
    //    |     |    |   |  |   |   |   +-- fail when? (0 to not)
    //    |     |    |   |  |   |   |   |  +-- initial value
    //    |     |    |   |  |   |   |   |  |  +-- expected value
    //    |     |    |   |  |   |   |   |  |  |
    //    V     V    V   V  V   V   V   V  V  V
    TEST ("",  -1,  -1, -1, 0, _ef, 0,  0, 1, 1);
    TEST (" ", -1,  -1, -1, 0, __f, 0,  0, 2, 2);
    TEST (" ", -1,  -1, -1, 0, _ef, 0,  1, 3, 3);   // fail
    TEST (" ", -1,  -1, -1, 0, b__, 0, -1, 4, 4);   // throw

    // exercise behavior when the initial state is other than good
    TEST (" ",   -1, __f, -1, 0, __f, 0, -1,  5,  5);
    TEST (" ",   -1, _e_, -1, 0, _ef, 0, -1,  6,  6);
    TEST (" ",   -1, _ef, -1, 0, _ef, 0, -1,  7,  7);
    TEST (" ",   -1, b__, -1, 0, b_f, 0, -1,  8,  8);
    TEST (" ",   -1, b_f, -1, 0, b_f, 0, -1,  9,  9);
    TEST (" ",   -1, be_, -1, 0, bef, 0, -1, 10, 10);
    TEST (" ",   -1, bef, -1, 0, bef, 0, -1, 11, 11);

    // verify that when the initial state isn't good the extractors
    // do not attempt to obtain the requested input but call setstate()
    // to set failbit (which causes failure to be thrown when failbit
    // or any other bit is set in exceptions) (see LWG issue 419)
    TEST (" ",   -1, __f, __f, 2, __f, 0, -1, 9, 9);
    TEST (" ",   -1, _e_, __f, 2, _ef, 0, -1, 9, 9);
    TEST (" ",   -1, _e_, _e_, 2, _ef, 0, -1, 9, 9);
    TEST (" ",   -1, _ef, _e_, 2, _ef, 0, -1, 9, 9);
    TEST (" ",   -1, b__, __f, 2, b_f, 0, -1, 9, 9);
    TEST (" ",   -1, b_f, __f, 2, b_f, 0, -1, 9, 9);
    TEST (" ",   -1, b_f, b__, 2, b_f, 0, -1, 9, 9);

    locale_data.whitespace = " ";
    TEST (" ",   -1,  -1, -1, 0, _ef, 1,  0, 9, 9);
    TEST (" ",   -1,  -1, -1, 0, _ef, 0,  1, 9, 9);   // fail
    TEST (" ",   -1,  -1, -1, 0, b__, 0, -1, 9, 9);   // throw
    TEST (" ",   -1,  -1, -1, 0, _ef, 1,  2, 9, 9);   // fail
    TEST (" ",   -1,  -1, -1, 0, b__, 1, -2, 9, 9);   // throw
    TEST ("_",   -1,  -1, -1, 0, __f, 0,  0, 9, 9);

    TEST (" ",    0,  -1, -1, 0, __f, 0,  0, 9, 9);

    locale_data.whitespace = "_";
    TEST (" ",   -1, -1, -1, 0, __f, 0, 0, 9, 9);
    TEST ("_",   -1, -1, -1, 0, _ef, 1, 0, 9, 9);

    locale_data.whitespace = "";
    TEST ("0",   -1, -1, -1, 0, _e_, 1, 0, 9,  0);
    TEST ("1",   -1, -1, -1, 0, _e_, 1, 0, 9,  1);
    TEST ("+0",  -1, -1, -1, 0, _e_, 2, 0, 9,  0);
    TEST ("-0",  -1, -1, -1, 0, _e_, 2, 0, 9,  0);
    TEST ("+1",  -1, -1, -1, 0, _e_, 2, 0, 9,  1);
    TEST ("-1",  -1, -1, -1, 0, _e_, 2, 0, 1, -1);
    TEST ("+2 ", -1, -1, -1, 0, ___, 2, 0, 9,  2);
    TEST ("-2 ", -1, -1, -1, 0, ___, 2, 0, 9, -2);

    if (is_floating) {
        TEST ("3.",  -1, -1, -1, 0, _e_, 2, 0, 9,  3);
        TEST ("+3.", -1, -1, -1, 0, _e_, 3, 0, 9,  3);
        TEST ("-3.", -1, -1, -1, 0, _e_, 3, 0, 9, -3);
    }
    else {
        TEST ("3.",  -1, -1, -1, 0, ___, 1, 0, 9,  3);
        TEST ("+3.", -1, -1, -1, 0, ___, 2, 0, 9,  3);
        TEST ("-3.", -1, -1, -1, 0, ___, 2, 0, 9, -3);
   }

    // set '#' as the only whitespace character
    locale_data.whitespace = "#";
    TEST ("#+3",  -1, -1, -1, 0, _e_, 3, 0,  9,  3);
    TEST ("#-4",  -1, -1, -1, 0, _e_, 3, 0,  9, -4);
    TEST ("#+5",  -1, -1, -1, 0, _e_, 3, 0,  9,  5);
    TEST ("#-6",  -1, -1, -1, 0, _e_, 3, 0,  9, -6);
    TEST ("#+7#", -1, -1, -1, 0, ___, 3, 0,  9,  7);
    TEST ("#-8#", -1, -1, -1, 0, ___, 3, 0,  9, -8);
    TEST (" -9",  -1, -1, -1, 0, __f, 0, 0, -9, -9);

    // set 'X', 'Y', and 'Z' as the only whitespace characters
    locale_data.whitespace = "XYZ";
    TEST ("X10",   -1, -1, -1, 0, _e_, 3, 0, 9, 10);
    TEST ("XY11",  -1, -1, -1, 0, _e_, 4, 0, 9, 11);
    TEST ("XYZ12", -1, -1, -1, 0, _e_, 5, 0, 9, 12);

    // set '9' as the whitespace character
    locale_data.whitespace = "9";
    TEST ("9",    -1, -1, -1, 0, _ef, 1, 0, 8, 8);
    TEST ("99",   -1, -1, -1, 0, _ef, 2, 0, 8, 8);
    TEST ("999",  -1, -1, -1, 0, _ef, 3, 0, 8, 8);
    TEST ("9991", -1, -1, -1, 0, _e_, 4, 0, 8, 1);

    // exercise grouping
    locale_data.whitespace    = "_";
    locale_data.thousands_sep = ';';
    locale_data.grouping      = "\1";

    TEST ("0;0",       -1, -1, -1, 0, _e_, 3, 0, 9,  0);
    TEST ("+0;0",      -1, -1, -1, 0, _e_, 4, 0, 9,  0);
    TEST ("-0;0",      -1, -1, -1, 0, _e_, 4, 0, 9,  0);
    TEST ("+0;1",      -1, -1, -1, 0, _e_, 4, 0, 9,  1);
    TEST ("-0;1",      -1, -1, -1, 0, _e_, 4, 0, 9, -1);
    TEST ("0;1",       -1, -1, -1, 0, _e_, 3, 0, 9,  1);
    TEST ("1;0",       -1, -1, -1, 0, _e_, 3, 0, 9,  10);
    TEST ("1;2",       -1, -1, -1, 0, _e_, 3, 0, 9,  12);
    TEST ("_1;2",      -1, -1, -1, 0, _e_, 4, 0, 9,  12);
    TEST ("_1;2_",     -1, -1, -1, 0, ___, 4, 0, 9,  12);
    TEST ("_1_;2",     -1, -1, -1, 0, ___, 2, 0, 9,  1);
    TEST ("12345",     -1, -1, -1, 0, _e_, 5, 0, 9,  12345);
    TEST ("1;2;3;4;5", -1, -1, -1, 0, _e_, 9, 0, 9,  12345);
    TEST ("12;3;4;5",  -1, -1, -1, 0, _ef, 8, 0, 9,  12345);

    // exercise thousands_sep that's a space
    locale_data.whitespace    = " ";
    locale_data.thousands_sep = ' ';
    locale_data.grouping      = "\2";
    TEST (" 0",        -1, -1, -1, 0, _e_, 2, 0, 9,  0);
    TEST ("  0",       -1, -1, -1, 0, _e_, 3, 0, 9,  0);
    TEST ("   0",      -1, -1, -1, 0, _e_, 4, 0, 9,  0);
    TEST (" 0 00",     -1, -1, -1, 0, _e_, 5, 0, 9,  0);
    TEST ("+0 00",     -1, -1, -1, 0, _e_, 5, 0, 9,  0);
    TEST ("-0 00",     -1, -1, -1, 0, _e_, 5, 0, 9,  0);
    TEST (" +0 00",    -1, -1, -1, 0, _e_, 6, 0, 9,  0);
    TEST (" -0 00",    -1, -1, -1, 0, _e_, 6, 0, 9,  0);
    TEST ("  +0 00",   -1, -1, -1, 0, _e_, 7, 0, 9,  0);
    TEST ("  -0 00",   -1, -1, -1, 0, _e_, 7, 0, 9,  0);
    TEST (" +00 00",   -1, -1, -1, 0, _e_, 7, 0, 9,  0);
    TEST (" -00 00",   -1, -1, -1, 0, _e_, 7, 0, 9,  0);
    TEST ("  00 01",   -1, -1, -1, 0, _e_, 7, 0, 9,  1);
    TEST (" +00 01",   -1, -1, -1, 0, _e_, 7, 0, 9,  1);
    TEST (" -00 01",   -1, -1, -1, 0, _e_, 7, 0, 1, -1);
    TEST ("0 01 02",   -1, -1, -1, 0, _e_, 7, 0, 9,  102);
    TEST ("1 02 03",   -1, -1, -1, 0, _e_, 7, 0, 9,  10203);
    TEST ("2 03 04",   -1, -1, -1, 0, _e_, 7, 0, 9,  20304);
    TEST ("304 05",    -1, -1, -1, 0, _ef, 6, 0, 9,  30405);
    TEST ("4 0 5 0",   -1, -1, -1, 0, _ef, 7, 0, 9,  4050);

    // according to the grammar in locale.numpunct, a valid number
    // cannot start or end with a thousands separator (and all pairs
    // of thousands separator must be separated by at least one
    // digit)
    locale_data.whitespace    = "_";
    locale_data.thousands_sep = ';';
    locale_data.grouping      = "\1";
    TEST (";",         -1, -1, -1, 0, __f, 0, 0, 9, 9);
    TEST (";;",        -1, -1, -1, 0, __f, 0, 0, 9, 9);
    TEST ("_;",        -1, -1, -1, 0, __f, 1, 0, 9, 9);
    TEST ("+;",        -1, -1, -1, 0, __f, 1, 0, 9, 9);
    TEST ("-;",        -1, -1, -1, 0, __f, 1, 0, 9, 9);
    TEST ("0;",        -1, -1, -1, 0, _ef, 2, 0, 9, 9);
    TEST ("+0;",       -1, -1, -1, 0, _ef, 3, 0, 9, 9);
    TEST ("-0;",       -1, -1, -1, 0, _ef, 3, 0, 9, 9);
    TEST ("+1;",       -1, -1, -1, 0, _ef, 3, 0, 9, 9);
    TEST ("-1;",       -1, -1, -1, 0, _ef, 3, 0, 9, 9);
    TEST ("1;",        -1, -1, -1, 0, _ef, 2, 0, 9, 9);
    TEST ("0;_",       -1, -1, -1, 0, __f, 2, 0, 9, 9);
    TEST ("1;_",       -1, -1, -1, 0, __f, 2, 0, 9, 9);
    TEST ("_0;",       -1, -1, -1, 0, _ef, 3, 0, 9, 9);
    TEST ("_1;",       -1, -1, -1, 0, _ef, 3, 0, 9, 9);
    TEST ("0;;",       -1, -1, -1, 0, __f, 2, 0, 9, 9);
    TEST ("1;;",       -1, -1, -1, 0, __f, 2, 0, 9, 9);
    TEST (";0",        -1, -1, -1, 0, __f, 0, 0, 9, 9);
    TEST (";1",        -1, -1, -1, 0, __f, 0, 0, 9, 9);
    TEST ("_;0",       -1, -1, -1, 0, __f, 1, 0, 9, 9);
    TEST ("_;1",       -1, -1, -1, 0, __f, 1, 0, 9, 9);

    // exercise errors when thousands_sep that's a space
    locale_data.whitespace    = " ";
    locale_data.thousands_sep = ' ';
    locale_data.grouping      = "\2";
    TEST (" ",         -1, -1, -1, 0, _ef, 1, 0, 9, 9);
    TEST (" +",        -1, -1, -1, 0, _ef, 2, 0, 9, 9);
    TEST (" -",        -1, -1, -1, 0, _ef, 2, 0, 9, 9);
    TEST (" + ",       -1, -1, -1, 0, __f, 2, 0, 9, 9);
    TEST (" - ",       -1, -1, -1, 0, __f, 2, 0, 9, 9);
    TEST (" 0 ",       -1, -1, -1, 0, _ef, 3, 0, 9, 9);
    TEST (" +0 ",      -1, -1, -1, 0, _ef, 4, 0, 9, 9);
    TEST (" -0 ",      -1, -1, -1, 0, _ef, 4, 0, 9, 9);
    TEST (" 123 ",     -1, -1, -1, 0, _ef, 5, 0, 9, 9);
    TEST (" +123 ",    -1, -1, -1, 0, _ef, 6, 0, 9, 9);
    TEST (" -123 ",    -1, -1, -1, 0, _ef, 6, 0, 9, 9);
}

/***********************************************************************/

void info (const char *cname, const char *tname, const char *type)
{
    // format the CLASS environment variable w/o writing out any output
    rw_fprintf (0,
                "%{$CLASS!:@}",
                "%{?}istream%{:}%{?}wistream"
                "%{:}basic_istream<%s, %s>%{;}%{;}",
                'c' == *cname && 's' == *tname,
                'w' == *cname && 's' == *tname,
                cname, tname);

    rw_info (0, 0, __LINE__, "std::%{$CLASS}::operator>>(%s&)", type);
}

/***********************************************************************/

#undef TEST
#define TEST(T, fmt, str, fl, is, ex, ee, es, ne, fw, iv, ev)              \
        test_extractor ((CharT*)0, (Traits*)0, (T*)0, cname, tname, #T,    \
                        __LINE__, fmt, str, sizeof str - 1, locale_data,   \
                        fl, is, ex, ee, es, ne, fw, (T)(iv), (T)ev)

/***********************************************************************/

template <class CharT, class Traits>
void
test_bool (CharT*, Traits*,
           const char *cname,
           const char *tname)
{
    info (cname, tname, "bool");

#define TEST_BOOL(ss, fl, is, ex, ee, es, ne, fw, iv, ev)   \
    TEST (bool, "%b", ss, fl, is, ex, ee, es, ne, fw, iv, ev)

    const int ba    = std::ios::boolalpha;
    const int ba_ws = ba | std::ios::skipws;

    LocaleData locale_data = { 0, -1, -1, 0, 0, 0 };
    locale_data.whitespace = "_";
    locale_data.falsename = "NO";
    locale_data.truename  = "YES";

    TEST_BOOL ("",      -1, -1, -1, 0, _ef, 0, 0, false, false);
    TEST_BOOL ("",      -1, -1, -1, 0, _ef, 0, 0, true,  true);
    TEST_BOOL ("0",     -1, -1, -1, 0, _e_, 1, 0, true,  false);
    TEST_BOOL ("1",     -1, -1, -1, 0, _e_, 1, 0, false, true);
    TEST_BOOL ("+0",    -1, -1, -1, 0, _e_, 2, 0, true,  false);
    TEST_BOOL ("-0",    -1, -1, -1, 0, _e_, 2, 0, true,  false);
    TEST_BOOL ("+1",    -1, -1, -1, 0, _e_, 2, 0, false, true);
    TEST_BOOL ("00",    -1, -1, -1, 0, _e_, 2, 0, true,  false);
    TEST_BOOL ("01",    -1, -1, -1, 0, _e_, 2, 0, false, true);
    TEST_BOOL ("_000",  -1, -1, -1, 0, _e_, 4, 0, true,  false);
    TEST_BOOL ("_001",  -1, -1, -1, 0, _e_, 4, 0, false, true);

    TEST_BOOL ("NO",    -1, -1, -1, 0, __f, 0, 0, true,  true);
    TEST_BOOL ("YES",   -1, -1, -1, 0, __f, 0, 0, false, false);
    TEST_BOOL ("_NO",   -1, -1, -1, 0, __f, 1, 0, true,  true);
    TEST_BOOL ("_YES",  -1, -1, -1, 0, __f, 1, 0, false, false);

    TEST_BOOL ("NO",    ba,    -1, -1, 0, ___, 2, 0, false, false);
    TEST_BOOL ("YES",   ba,    -1, -1, 0, ___, 3, 0, true,  true);
    TEST_BOOL ("_NO",   ba_ws, -1, -1, 0, ___, 3, 0, false, false);
    TEST_BOOL ("_YES",  ba_ws, -1, -1, 0, ___, 4, 0, true,  true);
    TEST_BOOL ("_NO_",  ba_ws, -1, -1, 0, ___, 3, 0, false, false);
    TEST_BOOL ("_YES_", ba_ws, -1, -1, 0, ___, 4, 0, true,  true);

    locale_data.falsename = "bool:0";
    locale_data.truename  = "bool:1";

    TEST_BOOL ("bool:0", ba, -1, -1, 0, ___, 6, 0, false, false);
    TEST_BOOL ("bool:1", ba, -1, -1, 0, ___, 6, 0, true,  true);
    TEST_BOOL ("bool:2", ba, -1, -1, 0, __f, 5, 0, true,  true);
}

/***********************************************************************/

template <class CharT, class Traits>
void
test_shrt (CharT*, Traits*,
           const char *cname,
           const char *tname)
{
    info (cname, tname, "short");

    test_extractor ((CharT*)0, (Traits*)0, (short*)0,
                    cname, tname, "short", "%hi");

#define TEST_SHRT(ss, fl, is, ex, ee, es, ne, fw, iv, ev)   \
    TEST (short, "%hi", ss, fl, is, ex, ee, es, ne, fw, iv, ev)

    LocaleData locale_data = { 0, -1, -1, 0, 0, 0 };
    locale_data.whitespace = "";

#if 2 == _RWSTD_SHRT_SIZE
    const short smax = +32767;
    const short smin = -smax - 1;

    TEST_SHRT ("-32768", -1, -1, -1, 0, _e_, 6, 0, -1, smin);
    TEST_SHRT ("-32769", -1, -1, -1, 0, _ef, 6, 0, -1, smin);
    TEST_SHRT ("-99999", -1, -1, -1, 0, _ef, 6, 0, -1, smin);

    TEST_SHRT ("+32767", -1, -1, -1, 0, _e_, 6, 0, -1, smax);
    TEST_SHRT ("+32768", -1, -1, -1, 0, _ef, 6, 0, -1, smax);
    TEST_SHRT ("+99999", -1, -1, -1, 0, _ef, 6, 0, -1, smax);
    TEST_SHRT ("999999", -1, -1, -1, 0, _ef, 6, 0, -1, smax);

#elif 4 == _RWSTD_SHRT_SIZE
    const short smax = +2147483647;
    const short smin = -smax - 1;

    TEST_SHRT ("-2147483648", -1, -1, -1, 0, _e_, 11, 0, -1, smin);
    TEST_SHRT ("-2147483649", -1, -1, -1, 0, _ef, 11, 0, -1, smin);
    TEST_SHRT ("-9999999999", -1, -1, -1, 0, _ef, 11, 0, -1, smin);

    TEST_SHRT ("+2147483647", -1, -1, -1, 0, _e_, 11, 0, -1, smax);
    TEST_SHRT ("+2147483648", -1, -1, -1, 0, _ef, 11, 0, -1, smax);
    TEST_SHRT ("+9999999999", -1, -1, -1, 0, _ef, 11, 0, -1, smax);
    TEST_SHRT ("99999999999", -1, -1, -1, 0, _ef, 11, 0, -1, smax);
#endif   // _RWSTD_SHRT_SIZE

}

/***********************************************************************/

template <class CharT, class Traits>
void
test_ushrt (CharT*, Traits*,
            const char *cname,
            const char *tname)
{
    info (cname, tname, "unsigned short");

    test_extractor ((CharT*)0, (Traits*)0, (unsigned short*)0,
                    cname, tname, "unsigned short", "%hu");

#define TEST_USHRT(ss, fl, is, ex, ee, es, ne, fw, iv, ev)   \
    TEST (unsigned short, "%hu", ss, fl, is, ex, ee, es, ne, fw, iv, ev)

    LocaleData locale_data = { 0, -1, -1, 0, 0, 0 };
    locale_data.whitespace = "";

#if 2 == _RWSTD_USHRT_SIZE
    const unsigned short usmax = 65535U;

    TEST_SHRT ("65535",  -1, -1, -1, 0, _e_, 5, 0,  1, usmax);
    TEST_SHRT ("65536",  -1, -1, -1, 0, _ef, 5, 0,  1, usmax);
    TEST_SHRT ("99999",  -1, -1, -1, 0, _ef, 5, 0,  1, usmax);
#elif 4 == _RWSTD_USHRT_SIZE
    const unsigned short usmax = 4294967295U;

    TEST_SHRT ("4294967295", -1, -1, -1, 0, _e_, 10, 0, -1, usmax);
    TEST_SHRT ("4294967296", -1, -1, -1, 0, _ef, 10, 0, -1, usmax);
    TEST_SHRT ("9999999999", -1, -1, -1, 0, _ef, 10, 0, -1, usmax);
#endif   // _RWSTD_USHRT_SIZE

}

/***********************************************************************/

template <class CharT, class Traits>
void
test_int (CharT*, Traits*,
          const char *cname,
          const char *tname)
{
    info (cname, tname, "int");

    test_extractor ((CharT*)0, (Traits*)0, (int*)0,
                    cname, tname, "int", "%i");

#define TEST_INT(ss, fl, is, ex, ee, es, ne, fw, iv, ev)   \
    TEST (int, "%i", ss, fl, is, ex, ee, es, ne, fw, iv, ev)

    LocaleData locale_data = { 0, -1, -1, 0, 0, 0 };
    locale_data.whitespace = "";

#if 2 == _RWSTD_INT_SIZE
    const int imax = +32767;
    const int imin = -imax - 1;

    TEST_INT ("-32768",  -1, -1, -1, 0, _e_, 6, 0,  1, imin);
    TEST_INT ("-32769",  -1, -1, -1, 0, _ef, 6, 0,  1, imin);
    TEST_INT ("-99999",  -1, -1, -1, 0, _ef, 6, 0,  1, imin);

    TEST_INT ("+32767",  -1, -1, -1, 0, _e_, 6, 0,  1, imax);
    TEST_INT ("+32768",  -1, -1, -1, 0, _ef, 6, 0,  1, imax);
    TEST_INT ("+99999",  -1, -1, -1, 0, _ef, 6, 0,  1, imax);
#elif 4 == _RWSTD_INT_SIZE
    const int imax = +2147483647;
    const int imin = -imax - 1;

    TEST_INT ("-2147483648", -1, -1, -1, 0, _e_, 11, 0, -1, imin);
    TEST_INT ("-2147483649", -1, -1, -1, 0, _ef, 11, 0, -1, imin);
    TEST_INT ("-9999999999", -1, -1, -1, 0, _ef, 11, 0, -1, imin);

    TEST_INT ("+2147483647", -1, -1, -1, 0, _e_, 11, 0, -1, imax);
    TEST_INT ("+2147483648", -1, -1, -1, 0, _ef, 11, 0, -1, imax);
    TEST_INT ("99999999999", -1, -1, -1, 0, _ef, 11, 0, -1, imax);
#elif 8 == _RWSTD_INT_SIZE
    const int imax = +9223372036854775807;
    const int imin = -imax - 1;

    TEST_INT ("-9223372036854775808", -1, -1, -1, 0, _e_, 20, 0, -1, imin);
    TEST_INT ("-9223372036854775809", -1, -1, -1, 0, _ef, 20, 0, -1, imin);
    TEST_INT ("-9999999999999999999", -1, -1, -1, 0, _ef, 20, 0, -1, imin);

    TEST_INT ("+9223372036854775807", -1, -1, -1, 0, _e_, 20, 0, -1, imax);
    TEST_INT ("+9223372036854775808", -1, -1, -1, 0, _ef, 20, 0, -1, imax);
    TEST_INT ("99999999999999999999", -1, -1, -1, 0, _ef, 20, 0, -1, imax);
#endif   // _RWSTD_INT_SIZE

}

/***********************************************************************/

template <class CharT, class Traits>
void
test_uint (CharT*, Traits*,
           const char *cname,
           const char *tname)
{
    info (cname, tname, "unsigned int");

    test_extractor ((CharT*)0, (Traits*)0, (unsigned int*)0,
                     cname, tname, "unsigned int", "%u");

#define TEST_UINT(ss, fl, is, ex, ee, es, ne, fw, iv, ev)   \
    TEST (int, "%u", ss, fl, is, ex, ee, es, ne, fw, iv, ev)

    LocaleData locale_data = { 0, -1, -1, 0, 0, 0 };
    locale_data.whitespace = "";

#if 2 == _RWSTD_UINT_SIZE
    const unsigned uimax = 65535U;

    TEST_INT ("65535",  -1, -1, -1, 0, _e_, 5, 0,  1, uimax);
    TEST_INT ("65536",  -1, -1, -1, 0, _ef, 5, 0,  1, uimax);
    TEST_INT ("99999",  -1, -1, -1, 0, _ef, 5, 0,  1, uimax);
#elif 4 == _RWSTD_UINT_SIZE
    const unsigned uimax = 4294967295U;

    TEST_INT ("4294967295", -1, -1, -1, 0, _e_, 10, 0, -1, uimax);
    TEST_INT ("4294967296", -1, -1, -1, 0, _ef, 10, 0, -1, uimax);
    TEST_INT ("9999999999", -1, -1, -1, 0, _ef, 10, 0, -1, uimax);
#elif 8 == _RWSTD_UINT_SIZE
    const unsigned uimax = 9223372036854775807U;

    TEST_INT ("9223372036854775807", -1, -1, -1, 0, _e_, 10, 0, -1, uimax);
    TEST_INT ("9223372036854775808", -1, -1, -1, 0, _ef, 10, 0, -1, uimax);
    TEST_INT ("9999999999999999999", -1, -1, -1, 0, _ef, 10, 0, -1, uimax);
#endif   // _RWSTD_UINT_SIZE

}

/***********************************************************************/

template <class CharT, class Traits>
void
test_long (CharT*, Traits*,
           const char *cname,
           const char *tname)
{
    info (cname, tname, "long");

    test_extractor ((CharT*)0, (Traits*)0, (long*)0,
                    cname, tname, "long", "%li");

#define TEST_LONG(ss, fl, is, ex, ee, es, ne, fw, iv, ev)   \
    TEST (long, "%li", ss, fl, is, ex, ee, es, ne, fw, iv, ev)

    LocaleData locale_data = { 0, -1, -1, 0, 0, 0 };
    locale_data.whitespace = "";

#if 2 == _RWSTD_LONG_SIZE
    const long lmax = +32767L;
    const long lmin = -lmax - 1;

    TEST_LONG ("-32768",  -1, -1, -1, 0, _e_, 6, 0,  1, lmin);
    TEST_LONG ("-32769",  -1, -1, -1, 0, _ef, 6, 0,  1, lmin);
    TEST_LONG ("-99999",  -1, -1, -1, 0, _ef, 6, 0,  1, lmin);

    TEST_LONG ("+32767",  -1, -1, -1, 0, _e_, 6, 0,  1, lmax);
    TEST_LONG ("+32768",  -1, -1, -1, 0, _ef, 6, 0,  1, lmax);
    TEST_LONG ("+99999",  -1, -1, -1, 0, _ef, 6, 0,  1, lmax);
#elif 4 == _RWSTD_LONG_SIZE
    const long lmax = +2147483647L;
    const long lmin = -lmax - 1;

    TEST_LONG ("-2147483648", -1, -1, -1, 0, _e_, 11, 0, -1, lmin);
    TEST_LONG ("-2147483649", -1, -1, -1, 0, _ef, 11, 0, -1, lmin);
    TEST_LONG ("-9999999999", -1, -1, -1, 0, _ef, 11, 0, -1, lmin);

    TEST_LONG ("+2147483647", -1, -1, -1, 0, _e_, 11, 0, -1, lmax);
    TEST_LONG ("+2147483648", -1, -1, -1, 0, _ef, 11, 0, -1, lmax);
    TEST_LONG ("99999999999", -1, -1, -1, 0, _ef, 11, 0, -1, lmax);
#elif 8 == _RWSTD_LONG_SIZE
    const long lmax = +9223372036854775807L;
    const long lmin = -lmax - 1;

    TEST_LONG ("-9223372036854775808", -1, -1, -1, 0, _e_, 20, 0, -1, lmin);
    TEST_LONG ("-9223372036854775809", -1, -1, -1, 0, _ef, 20, 0, -1, lmin);
    TEST_LONG ("-9999999999999999999", -1, -1, -1, 0, _ef, 20, 0, -1, lmin);

    TEST_LONG ("+9223372036854775807", -1, -1, -1, 0, _e_, 20, 0, -1, lmax);
    TEST_LONG ("+9223372036854775808", -1, -1, -1, 0, _ef, 20, 0, -1, lmax);
    TEST_LONG ("99999999999999999999", -1, -1, -1, 0, _ef, 20, 0, -1, lmax);
#endif   // _RWSTD_LONG_SIZE

}

/***********************************************************************/

template <class CharT, class Traits>
void
test_ulong (CharT*, Traits*,
            const char *cname,
            const char *tname)
{
    info (cname, tname, "unsigned long");

    test_extractor ((CharT*)0, (Traits*)0, (unsigned long*)0,
                     cname, tname, "unsigned long", "%lu");

#define TEST_ULONG(ss, fl, is, ex, ee, es, ne, fw, iv, ev)   \
    TEST (unsigned long, "%lu", ss, fl, is, ex, ee, es, ne, fw, iv, ev)

    LocaleData locale_data = { 0, -1, -1, 0, 0, 0 };
    locale_data.whitespace = "";

#if 2 == _RWSTD_ULONG_SIZE
    const unsigned ulmax = 65535UL;

    TEST_ULONG ("65535",  -1, -1, -1, 0, _e_, 5, 0,  1, ulmax);
    TEST_ULONG ("65536",  -1, -1, -1, 0, _ef, 5, 0,  1, ulmax);
    TEST_ULONG ("99999",  -1, -1, -1, 0, _ef, 5, 0,  1, ulmax);
#elif 4 == _RWSTD_ULONG_SIZE
    const unsigned ulmax = 4294967295UL;

    TEST_ULONG ("4294967295", -1, -1, -1, 0, _e_, 10, 0, -1, ulmax);
    TEST_ULONG ("4294967296", -1, -1, -1, 0, _ef, 10, 0, -1, ulmax);
    TEST_ULONG ("9999999999", -1, -1, -1, 0, _ef, 10, 0, -1, ulmax);
#elif 8 == _RWSTD_ULONG_SIZE
    const unsigned ulmax = 9223372036854775807UL;

    TEST_ULONG ("9223372036854775807", -1, -1, -1, 0, _e_, 10, 0, -1, ulmax);
    TEST_ULONG ("9223372036854775808", -1, -1, -1, 0, _ef, 10, 0, -1, ulmax);
    TEST_ULONG ("9999999999999999999", -1, -1, -1, 0, _ef, 10, 0, -1, ulmax);
#endif   // _RWSTD_ULONG_SIZE

}

/***********************************************************************/

template <class CharT, class Traits>
void
test_llong (CharT*, Traits*,
            const char *cname,
            const char *tname)
{
    info (cname, tname, "long long");

    test_extractor ((CharT*)0, (Traits*)0, (long long*)0,
                    cname, tname, "long long", "%lli");

#define TEST_LLONG(ss, fl, is, ex, ee, es, ne, fw, iv, ev)   \
    TEST (long long, "%lli", ss, fl, is, ex, ee, es, ne, fw, iv, ev)

    LocaleData locale_data = { 0, -1, -1, 0, 0, 0 };
    locale_data.whitespace = "";

#if 2 == _RWSTD_LLONG_SIZE
    const long long llmax = +32767LL;
    const long long llmin = -llmax - 1;

    TEST_LLONG ("-32768",  -1, -1, -1, 0, _e_, 6, 0,  1, llmin);
    TEST_LLONG ("-32769",  -1, -1, -1, 0, _ef, 6, 0,  1, llmin);
    TEST_LLONG ("-99999",  -1, -1, -1, 0, _ef, 6, 0,  1, llmin);

    TEST_LLONG ("+32767",  -1, -1, -1, 0, _e_, 6, 0,  1, llmax);
    TEST_LLONG ("+32768",  -1, -1, -1, 0, _ef, 6, 0,  1, llmax);
    TEST_LLONG ("+99999",  -1, -1, -1, 0, _ef, 6, 0,  1, llmax);
#elif 4 == _RWSTD_LLONG_SIZE
    const long long llmax = +2147483647LL;
    const long long llmin = -llmax - 1;

    TEST_LLONG ("-2147483648", -1, -1, -1, 0, _e_, 11, 0, -1, llmin);
    TEST_LLONG ("-2147483649", -1, -1, -1, 0, _ef, 11, 0, -1, llmin);
    TEST_LLONG ("-9999999999", -1, -1, -1, 0, _ef, 11, 0, -1, llmin);

    TEST_LLONG ("+2147483647", -1, -1, -1, 0, _e_, 11, 0, -1, llmax);
    TEST_LLONG ("+2147483648", -1, -1, -1, 0, _ef, 11, 0, -1, llmax);
    TEST_LLONG ("99999999999", -1, -1, -1, 0, _ef, 11, 0, -1, llmax);
#elif 8 == _RWSTD_LLONG_SIZE
    const long long llmax = +9223372036854775807LL;
    const long long llmin = -llmax - 1;

    TEST_LLONG ("-9223372036854775808", -1, -1, -1, 0, _e_, 20, 0, -1, llmin);
    TEST_LLONG ("-9223372036854775809", -1, -1, -1, 0, _ef, 20, 0, -1, llmin);
    TEST_LLONG ("-9999999999999999999", -1, -1, -1, 0, _ef, 20, 0, -1, llmin);

    TEST_LLONG ("+9223372036854775807", -1, -1, -1, 0, _e_, 20, 0, -1, llmax);
    TEST_LLONG ("+9223372036854775808", -1, -1, -1, 0, _ef, 20, 0, -1, llmax);
    TEST_LLONG ("99999999999999999999", -1, -1, -1, 0, _ef, 20, 0, -1, llmax);
#endif   // _RWSTD_LLONG_SIZE

}

/***********************************************************************/

template <class CharT, class Traits>
void
test_ullong (CharT*, Traits*,
             const char *cname,
             const char *tname)
{
    info (cname, tname, "unsigned long long");

    test_extractor ((CharT*)0, (Traits*)0, (unsigned long long*)0,
                    cname, tname, "unsigned long long", "%llu");

#define TEST_ULLONG(ss, fl, is, ex, ee, es, ne, fw, iv, ev)   \
    TEST (unsigned long long, "%llu", ss, fl, is, ex, ee, es, ne, fw, iv, ev)

    LocaleData locale_data = { 0, -1, -1, 0, 0, 0 };
    locale_data.whitespace = "";

#if 2 == _RWSTD_ULLONG_SIZE
    const unsigned long long ullmax = 65535ULL;

    TEST_ULLONG ("65535",  -1, -1, -1, 0, _e_, 5, 0,  1, ullmax);
    TEST_ULLONG ("65536",  -1, -1, -1, 0, _ef, 5, 0,  1, ullmax);
    TEST_ULLONG ("99999",  -1, -1, -1, 0, _ef, 5, 0,  1, ullmax);
#elif 4 == _RWSTD_ULLONG_SIZE
    const unsigned long long ullmax = 4294967295ULL;

    TEST_ULLONG ("4294967295", -1, -1, -1, 0, _e_, 10, 0, -1, ullmax);
    TEST_ULLONG ("4294967296", -1, -1, -1, 0, _ef, 10, 0, -1, ullmax);
    TEST_ULLONG ("9999999999", -1, -1, -1, 0, _ef, 10, 0, -1, ullmax);
#elif 8 == _RWSTD_ULLONG_SIZE
    const unsigned long long ullmax = 9223372036854775807ULL;

    TEST_ULLONG ("9223372036854775807", -1, -1, -1, 0, _e_, 10, 0, -1, ullmax);
    TEST_ULLONG ("9223372036854775808", -1, -1, -1, 0, _ef, 10, 0, -1, ullmax);
    TEST_ULLONG ("9999999999999999999", -1, -1, -1, 0, _ef, 10, 0, -1, ullmax);
#endif   // _RWSTD_ULLONG_SIZE

}

/***********************************************************************/

template <class CharT, class Traits, class FloatingType>
void
test_floating (CharT*, Traits*, FloatingType*,
               const char *cname,
               const char *tname,
               const char *fname,
               const char *fmt)
{
    typedef FloatingType T;

    test_extractor ((CharT*)0, (Traits*)0, (FloatingType*)0,
                    cname, tname, fname, fmt);

#define TEST_FLOATING(ss, fl, is, ex, ee, es, ne, fw, iv, ev)   \
    TEST (T, fmt, ss, fl, is, ex, ee, es, ne, fw, T (iv), T (ev))

    LocaleData locale_data = { 0, -1, -1, 0, 0, 0 };
    locale_data.whitespace = "";

    TEST_FLOATING ("", -1, -1, -1, 0, _ef, 0, 0, 1.0,  1.0);
}

/***********************************************************************/

template <class CharT, class Traits>
void
test_flt (CharT*, Traits*,
          const char *cname,
          const char *tname)
{
    const char fmt[]   = "%g";
    const char fname[] = "float";

    info (cname, tname, fname);

    test_floating ((CharT*)0, (Traits*)0, (float*)0,
                   cname, tname, fname, fmt);

#define TEST_FLT(ss, fl, is, ex, ee, es, ne, fw, iv, ev)   \
    TEST (float, fmt, ss, fl, is, ex, ee, es, ne, fw, iv, ev)

    LocaleData locale_data = { 0, -1, -1, 0, 0, 0 };
    locale_data.whitespace = "";

    TEST_FLT ("", -1, -1, -1, 0, _ef, 0, 0, 1.0f,  1.0f);

    rw_warn (0, 0, __LINE__,
             "%{$CLASS}::operator>>(%s&) insufficiently exercised", fname);
}

/***********************************************************************/

template <class CharT, class Traits>
void
test_dbl (CharT*, Traits*,
          const char *cname,
          const char *tname)
{
    const char fmt[]   = "%g";
    const char fname[] = "double";

    info (cname, tname, fname);

    test_floating ((CharT*)0, (Traits*)0, (double*)0,
                   cname, tname, fname, fmt);

#define TEST_DBL(ss, fl, is, ex, ee, es, ne, fw, iv, ev)   \
    TEST (double, fmt, ss, fl, is, ex, ee, es, ne, fw, iv, ev)

    LocaleData locale_data = { 0, -1, -1, 0, 0, 0 };
    locale_data.whitespace = "";

    TEST_DBL ("", -1, -1, -1, 0, _ef, 0, 0, 1.0,  1.0);

    rw_warn (0, 0, __LINE__,
             "%{$CLASS}::operator>>(%s&) insufficiently exercised", fname);
}

/***********************************************************************/

template <class CharT, class Traits>
void
test_ldbl (CharT*, Traits*,
           const char *cname,
           const char *tname)
{
    const char fmt[]   = "%Lg";
    const char fname[] = "long double";

    info (cname, tname, fname);

    test_floating ((CharT*)0, (Traits*)0, (long double*)0,
                   cname, tname, fname, fmt);

#define TEST_LDBL(ss, fl, is, ex, ee, es, ne, fw, iv, ev)   \
    TEST (long double, fmt, ss, fl, is, ex, ee, es, ne, fw, iv, ev)

    LocaleData locale_data = { 0, -1, -1, 0, 0, 0 };
    locale_data.whitespace = "";

    TEST_LDBL ("", -1, -1, -1, 0, _ef, 0, 0, 1.0L,  1.0L);

    rw_warn (0, 0, __LINE__,
             "%{$CLASS}::operator>>(%s&) insufficiently exercised", fname);
}

/***********************************************************************/

int opt_bool;
int opt_shrt;
int opt_ushrt;
int opt_int;
int opt_uint;
int opt_long;
int opt_ulong;
int opt_llong;
int opt_ullong;
int opt_flt;
int opt_dbl;
int opt_ldbl;


template <class CharT, class Traits>
void
test_extractor (CharT*, Traits*,
                const char *cname,
                const char *tname)
{
    // silence unused variable warnings
    (void)(___+__f+_e_+_ef+b__+b_f+be_+bef+DEC+OCT+HEX);

#undef TEST
#define TEST(T)                                                          \
    if (rw_note (0 == opt_ ## T, 0, __LINE__, "%s tests disabled", #T))   \
        test_ ## T ((CharT*)0, (Traits*)0, cname, tname)

    //////////////////////////////////////////////////////////////////
    // basic_istream::operator>>(bool&)
    TEST (bool);

    //////////////////////////////////////////////////////////////////
    // basic_istream::operator>>(short&)
    TEST (shrt);

    //////////////////////////////////////////////////////////////////
    // basic_istream::operator>>(unsigned short&)
    TEST (ushrt);

    //////////////////////////////////////////////////////////////////
    // basic_istream::operator>>(int&)
    TEST (int);

    //////////////////////////////////////////////////////////////////
    // basic_istream::operator>>(unsigned int&)
    TEST (uint);

    //////////////////////////////////////////////////////////////////
    // basic_istream::operator>>(long&)
    TEST (long);

    //////////////////////////////////////////////////////////////////
    // basic_istream::operator>>(unsigned long&)
    TEST (ulong);

    //////////////////////////////////////////////////////////////////
    // basic_istream::operator>>(long long&)
    TEST (llong);

    //////////////////////////////////////////////////////////////////
    // basic_istream::operator>>(unsigned long long&)
    TEST (ullong);

    //////////////////////////////////////////////////////////////////
    // basic_istream::operator>>(float&)
    TEST (flt);

    //////////////////////////////////////////////////////////////////
    // basic_istream::operator>>(double&)
    TEST (dbl);

    //////////////////////////////////////////////////////////////////
    // basic_istream::operator>>(long double&)
    TEST (ldbl);
}

/***********************************************************************/

static int opt_char;
static int opt_wchar;
static int opt_char_traits;
static int opt_user_traits;


static int
run_test (int, char**)
{
#undef TEST
#define TEST(CharT, Traits) \
    test_extractor ((CharT*)0, (Traits*)0, #CharT, #Traits)

    if (rw_note (0 <= opt_char && 0 <= opt_char_traits, 0, __LINE__,
                 "basic_istream<char, char_traits<char>>::operator>>() "
                 "tests disabled"))
        TEST (char, std::char_traits<char>);

    if (rw_note (0 <= opt_char && 0 <= opt_user_traits, 0, __LINE__,
                 "basic_istream<char, UserTraits<char>>::operator>>() "
                 "tests disabled"))
        TEST (char, UserTraits<char>);

#ifndef _RWSTD_NO_WCHAR_T

    if (rw_note (0 <= opt_wchar && 0 <= opt_char_traits, 0, __LINE__,
                 "basic_istream<wchar_t, char_traits<wchar_t>>::operator>>() "
                 "tests disabled"))
        TEST (wchar_t, std::char_traits<wchar_t>);

    if (rw_note (0 <= opt_wchar && 0 <= opt_user_traits, 0, __LINE__,
                 "basic_istream<wchar_t, UserTraits<wchar_t>>::operator>>() "
                 "tests disabled"))
        TEST (wchar_t, UserTraits<wchar_t>);

#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}

/***********************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "istream.formatted.arithmetic",
                    0 /* no comment */,
                    run_test,
                    "|-char~ "
                    "|-wchar_t~ "
                    "|-char_traits~ "
                    "|-UserTraits~ "
                    "|-bool~ "
                    "|-short~ "
                    "|-unsigned-short~ "
                    "|-int~ "
                    "|-unsigned-int~ "
                    "|-long~ "
                    "|-unsigned-long~ "
                    "|-long-long~ "
                    "|-unsigned-long-long~ "
                    "|-float~ "
                    "|-double~ "
                    "|-long-double~ "
                    "|-no-gcount# ",
                    &opt_char,
                    &opt_wchar,
                    &opt_char_traits,
                    &opt_user_traits,
                    &opt_bool,
                    &opt_shrt,
                    &opt_ushrt,
                    &opt_int,
                    &opt_uint,
                    &opt_long,
                    &opt_ulong,
                    &opt_llong,
                    &opt_ullong,
                    &opt_flt,
                    &opt_dbl,
                    &opt_ldbl,
                    &opt_no_gcount,
                    (void*)0   /* sentinel */);
}
