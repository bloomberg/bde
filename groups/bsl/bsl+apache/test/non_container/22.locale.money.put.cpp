/***************************************************************************
 *
 * 22.locale.money.put.cpp - tests exercising the std::money_put facet
 *
 * $Id: 22.locale.money.put.cpp 651095 2008-04-23 22:50:27Z sebor $
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

#include <ios>
#include <locale>
#include <cstdio>      // for sprintf()

#include <cmdopt.h>    // for rw_enabled()
#include <driver.h>    // for rw_assert(), rw_test(), ...
#include <valcmp.h>    // for rw_strncmp()

/**************************************************************************/

// set to non-zero value when grouping shouldn't be exercised
static int no_grouping;

// set to non-zero value when the basic_string overloads
// of moneY-put::put() shouldn't be exercised
static int no_basic_string;

/**************************************************************************/

template <class charT, bool Intl>
struct Punct: std::moneypunct<charT, Intl>
{
    typedef typename std::moneypunct<charT>::char_type   char_type;
    typedef typename std::moneypunct<charT>::string_type string_type;

    static char_type                 decimal_point_;
    static char_type                 thousands_sep_;
    static const char               *grouping_;
    static const char_type          *curr_symbol_;
    static const char_type          *positive_sign_;
    static const char_type          *negative_sign_;
    static std::money_base::pattern  pos_format_;
    static std::money_base::pattern  neg_format_;
    static int                       frac_digits_;

    static int n_thousands_sep_;   // number of calls to do_thousands_sep()

    Punct (): std::moneypunct<charT>(1) { }

    char_type do_decimal_point () const {
        return decimal_point_;
    }

    int do_frac_digits () const {
        return frac_digits_;
    }

    virtual string_type do_curr_symbol () const {
        return curr_symbol_ ? curr_symbol_ : string_type ();
    }

    virtual std::string do_grouping () const {
        return grouping_;
    }

    virtual char_type do_thousands_sep () const {
        ++n_thousands_sep_;
        return thousands_sep_;
    }

    virtual string_type do_positive_sign () const {
        return positive_sign_ ? positive_sign_ : string_type ();
    }

    virtual string_type do_negative_sign () const {
        return negative_sign_ ? negative_sign_ : string_type ();
    }

    virtual std::money_base::pattern do_pos_format () const {
        return pos_format_;
    }

    virtual std::money_base::pattern do_neg_format () const {
        return neg_format_;
    }
};

template <class charT, bool Intl>
const char*
Punct<charT, Intl>::grouping_ = "";

template <class charT, bool Intl>
typename Punct<charT, Intl>::char_type
Punct<charT, Intl>::decimal_point_ = ':';

template <class charT, bool Intl>
typename Punct<charT, Intl>::char_type
Punct<charT, Intl>::thousands_sep_ = ';';

template <class charT, bool Intl>
const typename Punct<charT, Intl>::char_type*
Punct<charT, Intl>::curr_symbol_;

template <class charT, bool Intl>
const typename Punct<charT, Intl>::char_type*
Punct<charT, Intl>::positive_sign_;

template <class charT, bool Intl>
const typename Punct<charT, Intl>::char_type*
Punct<charT, Intl>::negative_sign_;

template <class charT, bool Intl>
std::money_base::pattern
Punct<charT, Intl>::pos_format_ = { {
    std::money_base::symbol,
    std::money_base::sign,
    std::money_base::none,
    std::money_base::value
} };

template <class charT, bool Intl>
std::money_base::pattern
Punct<charT, Intl>::neg_format_ = { {
    std::money_base::symbol,
    std::money_base::sign,
    std::money_base::none,
    std::money_base::value
} };

template <class charT, bool Intl>
int
Punct<charT, Intl>::n_thousands_sep_;

template <class charT, bool Intl>
int
Punct<charT, Intl>::frac_digits_;

/**************************************************************************/

template <class charT>
struct Ios: std::basic_ios<charT>
{
    Ios () { this->init (0); }
};

template <class charT>
struct MoneyPut: std::money_put<charT, charT*>
{
    // default ctor defined in order to make it possible
    // to define const objects of the type w/o explictly
    // initializing them
    typedef std::money_put<charT, charT*> Base;
    MoneyPut (): Base () { /* no-op */ }
};

/**************************************************************************/

bool test_neg_zero;

/**************************************************************************/

std::money_base::pattern
set_pattern (const char *format)
{
    std::money_base::pattern pat;

    for (unsigned i = 0; i != sizeof pat.field / sizeof *pat.field; ++i) {
        switch (format [i]) {
        case '\0': case '@': pat.field [i] = std::money_base::none; break;
        case '\1': case ' ': pat.field [i] = std::money_base::space; break;
        case '\2': case '$': pat.field [i] = std::money_base::symbol; break;
        case '\3': case '-': pat.field [i] = std::money_base::sign; break;
        case '\4': case '1': pat.field [i] = std::money_base::value; break;
        default:
            _RWSTD_ASSERT (!!"bad format specifier");
        }
    }

    return pat;
}


template <class charT>
int type_test (int         lineno,
               charT       /* unused */,
               long double val /* value to format */,
               const char *str /* expected result */,
               char        fill        = ' ',
               int         flags       = 0,
               int         frac_digits = 0,
               int         width       = 0,
               const char *format      = 0 /* pattern or printf() format */,
               const char *grouping    = 0)
{
    if (!rw_enabled (lineno)) {
        rw_note (0, __FILE__, __LINE__, "test on line %d disabled", lineno);
        return 0;
    }

    static const char* const cname = 1 < sizeof (charT) ? "wchar_t" : "char";

    static int i = 0;   // assertion number per specialization of type_test()

    if (!i) {
        rw_info (0, 0, 0, "std::money_put<%s>::put(ostreambuf_iterator"
                 "<%s>, bool, ios_base, %s, long double)",
                 cname, cname, cname);

        rw_info (0, 0, 0, "std::money_put<%s>::put(ostreambuf_iterator"
                 "<%s>, bool, ios_base, %s, "
                 "const basic_string<%s>&))",
                 cname, cname, cname, cname);
    }

    //////////////////////////////////////////////////////////////////
    // exercise put (..., long double) overload

    int nfail = 0;   // number of failed assertions

    // unless 0, or unless it starts with '%', the `format' argument
    // is treated as a money_base::pattern string; otherwise the
    // default pattern is used

    // default format as in locale ("C")
    const char *fmt_pat = !format || *format == '%' ? "\2\3\4\0" : format;

    if (!grouping) {
        // default grouping as in locale ("C")
        grouping = "";
    }

    const Punct<charT, false> pun;

    // take care to initialize Punct static data before installing
    // the facet in case locale or the base facet calls the overridden
    // virtuals early to cache the results
    pun.grouping_    = grouping;
    pun.frac_digits_ = frac_digits;

    if (val < 0.0) {
        pun.neg_format_ = set_pattern (fmt_pat);
        pun.pos_format_ = std::money_base::pattern ();
    }
    else if (val > 0.0) {
        pun.neg_format_ = std::money_base::pattern ();
        pun.pos_format_ = set_pattern (fmt_pat);
    }
    else {
        // 0.0 and -0.0
        pun.neg_format_ = set_pattern (fmt_pat);
        pun.pos_format_ = pun.neg_format_;
    }

    // ios-derived object to pass to the facet
    Ios<charT> ios;

    // money_put-derived object
    FACET_CONST MoneyPut<charT> mp;

    // install the moneypunct facet in a locale and imbue the latter
    // in the ios-derived object used by money_put
    std::locale loc (ios.getloc (), (FACET_CONST std::moneypunct<charT>*)&pun);
    ios.imbue (loc);

    // set the formatting flags and the width
    ios.flags (std::ios_base::fmtflags (flags));
    ios.width (std::streamsize (width));

    bool success;

    charT buf [256] = { 0 };

    if (rw_enabled ("long double")) {

        // exercise 22.2.6.2.1, long double overload
        *mp.put (buf, false, ios, charT (fill), val) = charT ();

        rw_assert (0 == ios.width (), __FILE__, lineno,
                   "money_put<%s>::put (%#p, false, const ios_base&, "
                   "%{#c}, %LfL) reset width from %d",
                   cname, buf, fill, val, width);

        success = 0 == rw_strncmp (buf, str);

        if (!success)
            ++nfail;

        rw_assert (success, __FILE__, lineno,
                   "money_put<%s>::put (%#p, false, "
                   "const ios_base&, %{#c}, %LfL) == %{#s} got %{#s}; "
                   "flags = %{If}, grouping = %{#s}",
                   cname, buf,
                   fill, val, str, buf,
                   flags, grouping);
    }
    else {
        static int msg_issued;

        rw_note (msg_issued++, __FILE__, 0, "long double test disabled");
    }

    //////////////////////////////////////////////////////////////////
    // exercise put (..., const basic_string&) overload

    if (frac_digits < 0) {
        // do not perform string test(s) for negative values of frac_digits
        // (extension implemented only for the long double overload of put())
        ++i;
        return nfail;
    }

    if (no_basic_string) {
        rw_note (0, __FILE__, __LINE__,
                 "basic_string<%s> test on line %d disabled", lineno, cname);
        return nfail;
    }

    // zero out buffer
    std::char_traits<charT>::assign (buf, sizeof buf / sizeof *buf, charT ());

    // if the format string starts with a '%',
    // use it to format the floating point value
    if (!format || *format != '%')
        format = "%.0" _RWSTD_LDBL_PRINTF_PREFIX "f";

    char valbuf [256] = "";
    const int valbuflen = std::sprintf (valbuf, format, val);

    RW_ASSERT (0 < valbuflen && valbuflen < int (sizeof valbuf));

    typedef std::char_traits<charT> Traits;
    typedef std::allocator<charT>   Allocator;

    const std::basic_string<charT, Traits, Allocator>
        valstr (valbuf, valbuf + valbuflen);

    // set width (reset by a previous call)
    ios.width (std::streamsize (width));

    // exercise 22.2.6.2.1, basic_string overload
    *mp.put (buf, false, ios, charT (fill), valstr) = charT ();
    
    success = 0 == rw_strncmp (buf, str);

    if (!success)
        ++nfail;

    rw_assert (success, __FILE__, lineno,
               "money_put<%s>::put (%#p, false, "
               "const ios_base&, %{#S}) == %{#s} got %{#s}; "
               "flags = %{If}, grouping = %{#s}",
               cname, fill, &valstr, buf, str,
               flags, grouping);

    ++i;

    return nfail;
}

/**************************************************************************/

// for convenience
#define Boolalpha   std::ios_base::boolalpha
#define Dec         std::ios_base::dec
#define Fixed       std::ios_base::fixed
#define Hex         std::ios_base::hex
#define Internal    std::ios_base::internal
#define Left        std::ios_base::left
#define Oct         std::ios_base::oct
#define Right       std::ios_base::right
#define Scientific  std::ios_base::scientific
#define Showbase    std::ios_base::showbase
#define Showpoint   std::ios_base::showpoint
#define Showpos     std::ios_base::showpos
#define Skipws      std::ios_base::skipws
#define Unitbuf     std::ios_base::unitbuf
#define Uppercase   std::ios_base::uppercase
#define Bin         std::ios_base::bin
#define Adjustfield std::ios_base::adjustfield
#define Basefield   std::ios_base::basefield
#define Floatfield  std::ios_base::floatfield
#define Nolock      std::ios_base::nolock
#define Nolockbuf   std::ios_base::nolockbuf

#define Eofbit      std::ios_base::eofbit
#define Failbit     std::ios_base::failbit
#define Goodbit     std::ios_base::goodbit


template <class charT>
void ldbl_test (charT)
{
    static const charT curr_symbol1 [8] = { '$', '\0' };
    Punct<charT, false>::curr_symbol_   = curr_symbol1;

    Punct<charT, false>::decimal_point_ = ':';
    Punct<charT, false>::thousands_sep_ = ';';

    static const charT signs1[][4] = {
        // negative            positive
        { '~', '\0' }, { '\0', },
    };

    Punct<charT, false>::negative_sign_ = signs1 [0];
    Punct<charT, false>::positive_sign_ = signs1 [1];

    int ntried  = 0;
    int nfailed = 0;

#define T      __LINE__, charT ()
#define TEST   ++ntried, nfailed += type_test

    // enum part { none, space, symbol, sign, value };

    // no symbol, empty sign, default format
    TEST (T,         0.,         "0", ' ');
    TEST (T,         1.,         "1", ' ');
    TEST (T,        12.,        "12", ' ');
    TEST (T,       123.,       "123", ' ');
    TEST (T,      1234.,      "1234", ' ');
    TEST (T,     12345.,     "12345", ' ');
    TEST (T,    123456.,    "123456", ' ');
    TEST (T,   1234567.,   "1234567", ' ');
    TEST (T,  12345678.,  "12345678", ' ');
    TEST (T, 123456789., "123456789", ' ');

    TEST (T, 12345678.9,  "12345679", ' ');
    TEST (T, 1234567.89,   "1234568", ' ');
    TEST (T, 123456.789,    "123457", ' ');
    TEST (T, 12345.6789,     "12346", ' ');
    TEST (T, 1234.56789,      "1235", ' ');
    TEST (T, 123.456789,       "123", ' ');
    TEST (T, 12.3456789,        "12", ' ');
    TEST (T, 1.23456789,         "1", ' ');
    TEST (T, .123456789,         "0", ' ');

    // exercise correct handling of frac_digits
    TEST (T, 9876543210.,  "987654321:0", ' ', 0, 1);
    TEST (T,  876543210.,   "8765432:10", ' ', 0, 2);
    TEST (T,   76543210.,    "76543:210", ' ', 0, 3);
    TEST (T,    6543210.,     "654:3210", ' ', 0, 4);
    TEST (T,     543210.,      "5:43210", ' ', 0, 5);
    TEST (T,      43210.,     "0:043210", ' ', 0, 6);
    TEST (T,       3210.,    "0:0003210", ' ', 0, 7);
    TEST (T,        210.,   "0:00000210", ' ', 0, 8);
    TEST (T,         10.,  "0:000000010", ' ', 0, 9);
    TEST (T,          1.,  "0:000000001", ' ', 0, 9);
    TEST (T,          0.,  "0:000000000", ' ', 0, 9);
    TEST (T,        200.,  "0:000000200", ' ', 0, 9);
    TEST (T,       3000.,  "0:000003000", ' ', 0, 9);
    TEST (T,      40000.,  "0:000040000", ' ', 0, 9);
    TEST (T,     500000.,  "0:000500000", ' ', 0, 9);
    TEST (T,    6000000.,  "0:006000000", ' ', 0, 9);
    TEST (T,   70000000.,  "0:070000000", ' ', 0, 9);
    TEST (T,  800000000.,  "0:800000000", ' ', 0, 9);
    TEST (T, -900000000., "~0:900000000", ' ', 0, 9);
    TEST (T,  -80000000., "~0:080000000", ' ', 0, 9);
    TEST (T,   -7000000., "~0:007000000", ' ', 0, 9);
    TEST (T,    -600000., "~0:000600000", ' ', 0, 9);
    TEST (T,     -50000., "~0:000050000", ' ', 0, 9);
    TEST (T,      -4000., "~0:000004000", ' ', 0, 9);
    TEST (T,       -300., "~0:000000300", ' ', 0, 9);
    TEST (T,        -20., "~0:000000020", ' ', 0, 9);
    TEST (T,         -1., "~0:000000001", ' ', 0, 9);

    if (test_neg_zero)
        TEST (T, -0., "~0:000000000", ' ', 0, 9);

    // extension: fixed and negative frac_digits
    TEST (T,   1.0,       "1:0",      ' ', Fixed, -1);
    TEST (T,   2.0,       "2:00",     ' ', Fixed, -2);
    TEST (T,   3.0,       "3:000",    ' ', Fixed, -3);
    TEST (T,   4.1,       "4:1000",   ' ', Fixed, -4);
    TEST (T,  52.34,     "52:34000",  ' ', Fixed, -5);
    TEST (T, 634.56789, "634:5679",   ' ', Fixed, -4);


    // exhaustively exercise valid permutations of format patterns
    // (`none' allowed in all but the first position by 22.2.6.3, p1)
    TEST (T,   12,   "12", '\0', 0, 0, 0, "\3\0\4\2", "");
    TEST (T,  123,  "123", '\0', 0, 0, 0, "\3\0\2\4", "");
    TEST (T, 1234, "1234", '\0', 0, 0, 0, "\3\2\4\0", "");
    TEST (T, 2345, "2345", '\0', 0, 0, 0, "\3\4\2\0", "");
    TEST (T, 3456, "3456", '\0', 0, 0, 0, "\3\4\0\2", "");
    TEST (T, 4567, "4567", '\0', 0, 0, 0, "\4\0\2\3", "");
    TEST (T, 5678, "5678", '\0', 0, 0, 0, "\4\2\0\3", "");
    TEST (T, 6789, "6789", '\0', 0, 0, 0, "\4\2\3\0", "");
    TEST (T, 7890, "7890", '\0', 0, 0, 0, "\4\0\3\2", "");
    TEST (T, 8901, "8901", '\0', 0, 0, 0, "\2\4\0\3", "");
    TEST (T, 9012, "9012", '\0', 0, 0, 0, "\2\0\4\3", "");
    TEST (T,  123,  "123", '\0', 0, 0, 0, "\2\0\3\4", "");

    // format using `space' in valid positions (all but
    // the first and last as specified by 22.2.6.3, p1)
    // the actual space character (not the fill char)
    // is required
    TEST (T,   9,   " 9", '*', 0, 0, 0, "\3\1\4\2", "");
    TEST (T,  98,  " 98", '*', 0, 0, 0, "\3\1\2\4", "");
    TEST (T, 987, "987 ", '*', 0, 0, 0, "\3\4\1\2", "");
    TEST (T, 876, "876 ", '*', 0, 0, 0, "\4\1\2\3", "");
    TEST (T, 765, "765 ", '*', 0, 0, 0, "\4\2\1\3", "");
    TEST (T, 654, "654 ", '*', 0, 0, 0, "\4\1\3\2", "");
    TEST (T, 543, "543 ", '*', 0, 0, 0, "\2\4\1\3", "");
    TEST (T, 432, " 432", '*', 0, 0, 0, "\2\1\4\3", "");
    TEST (T, 321, " 321", '*', 0, 0, 0, "\2\1\3\4", "");

    TEST (T,        0,        "$0", '\0', Showbase, 0, 0, "\2\3\4\0", "");
    TEST (T,        1,        "$1", '\0', Showbase, 0, 0, "\2\3\4\0", "");
    TEST (T,       12,       "$12", '\0', Showbase, 0, 0, "\2\3\4\0", "");
    TEST (T,      123,      "$123", '\0', Showbase, 0, 0, "\2\3\4\0", "");
    TEST (T,     1234,     "$1234", '\0', Showbase, 0, 0, "\2\3\4\0", "");
    TEST (T,    12345,    "$12345", '\0', Showbase, 0, 0, "\2\3\4\0", "");
    TEST (T,   123456,   "$123456", '\0', Showbase, 0, 0, "\2\3\4\0", "");
    TEST (T,  1234567,  "$1234567", '\0', Showbase, 0, 0, "\2\3\4\0", "");
    TEST (T, 12345678, "$12345678", '\0', Showbase, 0, 0, "\2\3\4\0", "");

    TEST (T,         0,             "$0", '\0', Showbase, 0, 0, 0, "\2");
    TEST (T,         1,             "$1", '\0', Showbase, 0, 0, 0, "\2\2");
    TEST (T,        12,            "$12", '\0', Showbase, 0, 0, 0, "\2");
    TEST (T,       123,          "$1;23", '\0', Showbase, 0, 0, 0, "\2\2");
    TEST (T,      1234,         "$12;34", '\0', Showbase, 0, 0, 0, "\2");
    TEST (T,     12345,       "$1;23;45", '\0', Showbase, 0, 0, 0, "\2\2");
    TEST (T,    123456,      "$12;34;56", '\0', Showbase, 0, 0, 0, "\2");
    TEST (T,   1234567,    "$1;23;45;67", '\0', Showbase, 0, 0, 0, "\2\2");
    TEST (T,  12345678,   "$12;34;56;78", '\0', Showbase, 0, 0, 0, "\2");
    TEST (T, 123456789, "$1;23;45;67;89", '\0', Showbase, 0, 0, 0, "\2\2");

    if (test_neg_zero)
        TEST (T, -0.0, "$~0", '\0', Showbase, 0, 0, 0, "\3\3");

    TEST (T,         -1,           "$~1", '\0', Showbase, 0, 0, 0, "\3");
    TEST (T,        -12,          "$~12", '\0', Showbase, 0, 0, 0, "\3\3");
    TEST (T,       -123,         "$~123", '\0', Showbase, 0, 0, 0, "\3");
    TEST (T,      -1234,       "$~1;234", '\0', Showbase, 0, 0, 0, "\3\3");
    TEST (T,     -12345,      "$~12;345", '\0', Showbase, 0, 0, 0, "\3");
    TEST (T,    -123456,     "$~123;456", '\0', Showbase, 0, 0, 0, "\3\3");
    TEST (T,   -1234567,   "$~1;234;567", '\0', Showbase, 0, 0, 0, "\3");
    TEST (T,  -12345678,  "$~12;345;678", '\0', Showbase, 0, 0, 0, "\3\3");
    TEST (T, -123456789, "$~123;456;789", '\0', Showbase, 0, 0, 0, "\3");

    // for convenience
    const char *f = "\3\2\4\0";   // { sign, symbol, value, none }

    TEST (T,       -321,           "~$32;1", '\0', Showbase, 0, 0, f, "\1\2");
    TEST (T,      -4322,          "~$432;2", '\0', Showbase, 0, 0, f, "\1\3");
    TEST (T,     -54323,      "~$5;4;3;2;3", '\0', Showbase, 0, 0, f, "\1\1");
    TEST (T,    -654324,    "~$6;5;4;3;2;4", '\0', Showbase, 0, 0, f, "\1\1");
    TEST (T,   -7654325,      "~$765;432;5", '\0', Showbase, 0, 0, f, "\1\3");
    TEST (T,  -87654326, "~$8;7;6;5;4;3;26", '\0', Showbase, 0, 0, f, "\2\1");
    TEST (T, -987654327,  "~$98;76;54;32;7", '\0', Showbase, 0, 0, f, "\1\2");

    static const charT curr_symbol2 [8] = { 'X', 'Y', '\0' };
    Punct<charT, false>::curr_symbol_   = curr_symbol2;

    static const charT signs2[][8] = {
        // negative            positive
        { '(', ')', '\0' }, { '{', '}', '\0' }
    };

    Punct<charT, false>::negative_sign_ = signs2 [0];
    Punct<charT, false>::positive_sign_ = signs2 [1];
    Punct<charT, false>::thousands_sep_ = '.';
    Punct<charT, false>::decimal_point_ = '.';

    f = "\3\4\1\2";   // { sign, value, space, symbol }

    TEST (T, -1357911, "(1.3.5.7.9.1.1 XY)", '*', Showbase, 0, 0, f, "\1");
    TEST (T, -2468012,    "(2.46.80.12 XY)", '-', Showbase, 0, 0, f, "\2");
    TEST (T, -3692513,     "(3.692.513 XY)", ' ', Showbase, 0, 0, f, "\3");
    TEST (T, -9999914,      "(999.9914 XY)", '@', Showbase, 0, 0, f, "\4");
    TEST (T, -1000015,      "(10.00015 XY)", '#', Showbase, 0, 0, f, "\5");
    TEST (T,      -16,            "(16 XY)", ')', Showbase, 0, 0, f, "\6");

    TEST (T, +1357917, "{1.3.5.7.9.1.7 XY}", ',',  Showbase, 0, 0, f, "\1");
    TEST (T, +2468018,    "{2.46.80.18 XY}", '+',  Showbase, 0, 0, f, "\2");
    TEST (T, +3692519,     "{3.692.519 XY}", '0',  Showbase, 0, 0, f, "\3");
    TEST (T, +9999920,      "{999.9920 XY}", '1',  Showbase, 0, 0, f, "\4");
    TEST (T, +1000021,      "{10.00021 XY}", '\0', Showbase, 0, 0, f, "\5");
    TEST (T,      +22,            "{22 XY}", '{',  Showbase, 0, 0, f, "\6");
    TEST (T,     +123,             "{123 }", '_',  0,        0, 0, f, "\6");
    TEST (T,     -224,             "(224 )", '_',  0,        0, 0, f, "\7");

    TEST (T,  +325,  "{ XY325}", ' ', Showbase, 0, 0, "\3\1\2\4", "\xa");
    TEST (T,  -426,  "( XY426)", ' ', Showbase, 0, 0, "\3\1\2\4", "\xb");
    TEST (T,  +527,  "{XY 527}", ' ', Showbase, 0, 0, "\3\2\1\4", "\xc");
    TEST (T,  -628,  "(XY 628)", ' ', Showbase, 0, 0, "\3\2\1\4", "\xd");
    TEST (T,  +729,   "{XY729}", ' ', Showbase, 0, 0, "\3\2\0\4", "\xe");
    TEST (T,  -830,   "(XY830)", ' ', Showbase, 0, 0, "\3\2\0\4", "\xf");
    TEST (T,  +931,   "{XY931}", ' ', Showbase, 0, 0, "\3\0\2\4", "\x10");
    TEST (T, -1032,  "(XY1032)", ' ', Showbase, 0, 0, "\3\0\2\4");
    TEST (T, +1133,  "1133{XY}", ' ', Showbase, 0, 0, "\4\3\2\0");
    TEST (T, -1234,  "1234XY()", ' ', Showbase, 0, 0, "\4\2\3\0");
    TEST (T, +1335,  "1335XY{}", ' ', Showbase, 0, 0, "\4\2\0\3");
    TEST (T, -1436, "1436XY ()", ' ', Showbase, 0, 0, "\4\2\1\3");
    TEST (T, +1537,  "1537XY{}", ' ', Showbase, 0, 0, "\4\0\2\3");
    TEST (T, -1638, "1638 XY()", ' ', Showbase, 0, 0, "\4\1\2\3");
    TEST (T, +1739,  "XY1739{}", ' ', Showbase, 0, 0, "\2\4\0\3");
    TEST (T, -1840, "XY1840 ()", ' ', Showbase, 0, 0, "\2\4\1\3");
    TEST (T, +1941,  "XY1941{}", ' ', Showbase, 0, 0, "\2\0\4\3");
    TEST (T, -2042, "XY 2042()", ' ', Showbase, 0, 0, "\2\1\4\3");
    TEST (T, +2143,  "XY{2143}", ' ', Showbase, 0, 0, "\2\3\4\0");
    TEST (T, -2244,  "XY(2244)", ' ', Showbase, 0, 0, "\2\3\0\4");
    TEST (T, +2345, "XY{ 2345}", ' ', Showbase, 0, 0, "\2\3\1\4");

    // 22.2.6.2, p2:
    //   ...copies of `fill' are inserted as necessary to pad to the
    //   specified width. For the value `af' equal to (str.flags()
    //   & str.adjustfield), if (af == str.internal) is true, the fill
    //   characters are placed where `none' or `space' appears in the
    //   formatting pattern; otherwise if (af == str.left) is true, they
    //   are placed after the other characters; otherwise, they are
    //   placed before the other characters.

    TEST (T, -2446, "___XY( 2446)", '_', Showbase, 0, 12, "\2\3\1\4");
    TEST (T, +2547, "____XY{2547}", '_', Showbase, 0, 12, "\2\3\0\4");

    TEST (T, -2648, "___XY( 2648)", '_', Showbase | Right, 0, 12, "\2\3\1\4");
    TEST (T, +2749, "____XY{2749}", '_', Showbase | Right, 0, 12, "\2\3\0\4");

    TEST (T, -2850, "XY( 2850)___", '_', Showbase | Left, 0, 12, "\2\3\1\4");
    TEST (T, +2951, "XY{2951}____", '_', Showbase | Left, 0, 12, "\2\3\0\4");

    TEST (T, -3052, "___XY( 3052)", '_',
          Showbase | Left | Right, 0, 12, "\2\3\1\4");

    TEST (T, +3153, "____XY{3153}", '_',
          Showbase | Left | Right, 0, 12, "\2\3\0\4");

    TEST (T, -3254, "___XY( 3254)", '_',
          Showbase | Left | Right | Internal, 0, 12, "\2\3\1\4");

    TEST (T, +3355, "____XY{3355}", '_',
          Showbase | Left | Right | Internal, 0, 12, "\2\3\0\4");

    TEST (T, -3456, "XY( ___3456)", '_',
          Showbase | Internal, 0, 12, "\2\3\1\4");

    TEST (T, +3557, "XY{____3557}", '_',
          Showbase | Internal, 0, 12, "\2\3\0\4");

    TEST (T, -3658, "XY____(3658)", '_',
          Showbase | Internal, 0, 12, "\2\0\3\4");

    TEST (T, +3759, "XY{3759____}", '_',
          Showbase | Internal, 0, 12, "\2\3\4\0");

    TEST (T, 3759, "XY{37.59}", '_',
          Showbase | Internal, 0, 8, "\2\3\4\0", "\2");

    TEST (T, 3760, "XY{ 37.60}", '_',
          Showbase | Internal, 0, 8, "\2\3\1\4", "\2");

    TEST (T, 12345678900000000000.0, "XY{ ........1.23.45678.9000.00.000.0.00}",
          '.', Showbase | Internal, 0, 40, "\2\3\1\4", "\2\1\3\2\4\5\2");

    TEST (T, 1234567890000000000.0, "{ ...........1234.56789.0000.000.00.0XY}",
          '.', Showbase | Internal, 0, 40, "\3\1\4\2", "\1\2\3\4\5\6");

    // exercise justification with non-zero frac_digits
    TEST (T,    1,  "_____{0.1}", '_', 0,        1, 10, "-@1$");
    TEST (T,   12,  "_____{1.2}", '_', 0,        1, 10, "-@1$");
    TEST (T,  123,  "____{1.23}", '_', 0,        2, 10, "-@1$");
    TEST (T, 1234,  "_{12.34XY}", '_', Showbase, 2, 10, "-@1$");
    TEST (T, 1235, "_{ 12.35XY}", '_', Showbase, 2, 11, "- 1$");

    TEST (T,    2,  "******{0.2}", '*', Right,            1, 11, "-@1$");
    TEST (T,   23,  "******{2.3}", '*', Right,            1, 11, "-@1$");
    TEST (T,  234,  "*****{2.34}", '*', Right,            2, 11, "-@1$");
    TEST (T, 2345,  "**{23.45XY}", '*', Right | Showbase, 2, 11, "-@1$");
    TEST (T, 2346, "**{ 23.46XY}", '*', Right | Showbase, 2, 12, "- 1$");

    TEST (T,    3,  "{0.3}#######", '#', Left,             1, 12, "-@1$");
    TEST (T,   34,  "{3.4}#######", '#', Left,             1, 12, "-@1$");
    TEST (T,  345,  "{3.45}######", '#', Left,             2, 12, "-@1$");
    TEST (T, 3456,  "{34.56XY}###", '#', Left | Showbase,  2, 12, "-@1$");
    TEST (T, 3457, "{ 34.57XY}###", '#', Left | Showbase,  2, 13, "- 1$");

    TEST (T,    4,  "{=====0.4}", '=', Internal,            1, 10, "-@1$");
    TEST (T,   45,  "{=====4.5}", '=', Internal,            1, 10, "-@1$");
    TEST (T,  456,  "{====4.56}", '=', Internal,            2, 10, "-@1$");
    TEST (T, 4567,  "{=45.67XY}", '=', Internal | Showbase, 2, 10, "-@1$");
    TEST (T, 4568, "{ =45.68XY}", '=', Internal | Showbase, 2, 11, "- 1$");

    Punct<charT, false>::thousands_sep_ = ',';
    Punct<charT, false>::decimal_point_ = '.';

    // justification with grouping
    TEST (T,    105,   "====={1,0.5}", '=', 0,        1, 12, "-@1$", "\1");
    TEST (T,   1056,   "===={1,05.6}", '=', 0,        1, 12, "-@1$", "\2");
    TEST (T,  10567,   "==={1,05.67}", '=', 0,        2, 12, "-@1$", "\2");
    TEST (T, 105678, "=={10,56.78XY}", '=', Showbase, 2, 14, "-@1$", "\2");
    TEST (T, 105679, "={ 10,56.79XY}", '=', Showbase, 2, 14, "- 1$", "\2");
    TEST (T, 105680, "={ 1,056,80XY}", '=', Showbase, 0, 14, "- 1$", "\2\3");

    int flags = Right | Showbase;
    TEST (T,    106,   ">>>>>{1,0.6}", '>', Right, 1, 12, "-@1$", "\1");
    TEST (T,   1057,   ">>>>{1,05.7}", '>', Right, 1, 12, "-@1$", "\2");
    TEST (T,  10568,   ">>>{1,05.68}", '>', Right, 2, 12, "-@1$", "\2");
    TEST (T, 105679, ">>{10,56.79XY}", '>', flags, 2, 14, "-@1$", "\2");
    TEST (T, 105680, ">{ 10,56.80XY}", '>', flags, 2, 14, "- 1$", "\2");
    TEST (T, 105681, ">{ 1,056,81XY}", '>', flags, 0, 14, "- 1$", "\2\3");

    flags = Left | Showbase;
    TEST (T,    107,   "{1,0.7}<<<<<", '<', Left,  1, 12, "-@1$", "\1");
    TEST (T,   1058,   "{1,05.8}<<<<", '<', Left,  1, 12, "-@1$", "\2");
    TEST (T,  10569,   "{1,05.69}<<<", '<', Left,  2, 12, "-@1$", "\2");
    TEST (T, 105680, "{10,56.80XY}<<", '<', flags, 2, 14, "-@1$", "\2");
    TEST (T, 105681, "{ 10,56.81XY}<", '<', flags, 2, 14, "- 1$", "\2");
    TEST (T, 105682, "{ 1,056,82XY}<", '<', flags, 0, 14, "- 1$", "\2\3");

    flags = Internal | Showbase;
    TEST (T,    108,   "{^^^^^1,0.8}", '^', Internal, 1, 12, "-@1$", "\1");
    TEST (T,   1059,   "{^^^^1,05.9}", '^', Internal, 1, 12, "-@1$", "\2");
    TEST (T,  10570,   "{^^^1,05.70}", '^', Internal, 2, 12, "-@1$", "\2");
    TEST (T, 105681, "{^^10,56.81XY}", '^', flags,    2, 14, "-@1$", "\2");
    TEST (T, 105682, "{ ^10,56.82XY}", '^', flags,    2, 14, "- 1$", "\2");
    TEST (T, 105683, "{ ^1,056,83XY}", '^', flags,    0, 14, "- 1$", "\2\3");

    flags = Left | Right | Showbase;
    TEST (T,    109,   "#####{1,0.9}", '#', Left | Right, 1, 12, "-@1$", "\1");
    TEST (T,   1060,   "####{1,06.0}", '#', Left | Right, 1, 12, "-@1$", "\2");
    TEST (T,  10571,   "###{1,05.71}", '#', Left | Right, 2, 12, "-@1$", "\2");
    TEST (T, 105682, "##{10,56.82XY}", '#', flags,        2, 14, "-@1$", "\2");
    TEST (T, 105683, "#{ 10,56.83XY}", '#', flags,        2, 14, "- 1$", "\2");
    TEST (T, 105684, "#{ 10,56,84XY}", '#', flags,        0, 14, "- 1$", "\2");


    // verify that mon_put (..., const basic_string&) ignores the part
    // of the string starting with the first non-numeric character
#undef FMT
#define FMT(fmt) "%.0" _RWSTD_LDBL_PRINTF_PREFIX "f" fmt

    // using default format "\2\3\4\0": {symbol, sign, value, none}
#if TEST_RW_EXTENSIONS
    // negative width not supported
    TEST (T,   10, "{1.0}", ' ', 0, 1, -1, FMT (" "));
    TEST (T,   10, "{1.0}", ' ', 0, 1, -1, FMT ("."));

    TEST (T,  -11, "(1.1)", ' ', 0, 1, -1, FMT (" "));
    TEST (T,  -11, "(1.1)", ' ', 0, 1, -1, FMT ("."));

    TEST (T,  101, "{1.01}", ' ', 0, 2, -1, FMT (" "));
    TEST (T,  101, "{1.01}", ' ', 0, 2, -1, FMT ("."));

    Punct<charT, false>::negative_sign_ = signs1 [0];
    Punct<charT, false>::positive_sign_ = signs1 [1];

    TEST (T,  1012, "1,0.12", ' ', 0, 2, -1, FMT (" 0"), "\1");
    TEST (T,  1013, "1,0.13", ' ', 0, 2, -1, FMT (".1"), "\1");

    TEST (T, -1014, "~1,01.4", ' ', 0, 1, -1, FMT ("~2"), "\2");
    TEST (T, -1015, "~1,01.5", ' ', 0, 1, -1, FMT (",3"), "\2");
#else
    TEST (T,   10, "{1.0}", ' ', 0, 1, 0, FMT (" "));
    TEST (T,   10, "{1.0}", ' ', 0, 1, 0, FMT ("."));

    TEST (T,  -11, "(1.1)", ' ', 0, 1, 0, FMT (" "));
    TEST (T,  -11, "(1.1)", ' ', 0, 1, 0, FMT ("."));

    TEST (T,  101, "{1.01}", ' ', 0, 2, 0, FMT (" "));
    TEST (T,  101, "{1.01}", ' ', 0, 2, 0, FMT ("."));

    Punct<charT, false>::negative_sign_ = signs1 [0];
    Punct<charT, false>::positive_sign_ = signs1 [1];

    TEST (T,  1012, "1,0.12", ' ', 0, 2, 0, FMT (" 0"), "\1");
    TEST (T,  1013, "1,0.13", ' ', 0, 2, 0, FMT (".1"), "\1");

    TEST (T, -1014, "~1,01.4", ' ', 0, 1, 0, FMT ("~2"), "\2");
    TEST (T, -1015, "~1,01.5", ' ', 0, 1, 0, FMT (",3"), "\2");
#endif
}

/**************************************************************************/

void grouping_test ()
{
    if (!rw_enabled ("char")) {
        rw_note (0, __FILE__, __LINE__, "char test disabled");
        return;
    }
    
    rw_info (0, 0, 0, "std::money_put<%s>::put(ostreambuf_iterator"
             "<%s>, bool, ios_base, %s, long double); correct use of "
             "moneypunct<%s, false>::grouping() and thousands_sep()",
             "char", "char", "char", "char");

    typedef Punct<char, false> PunctT;

    // construct a "replacement" moneypunct-derived facet
    FACET_CONST PunctT pun;

    // group after every digit
    PunctT::grouping_ = "\1";

    // reset the do_thousands_sep()-call counter
    PunctT::thousands_sep_   = ';';
    PunctT::decimal_point_   = ':';
    PunctT::n_thousands_sep_ = 0;

    // construct and initialize a basic_ios-derived object
    struct Ios: std::basic_ios<char> { Ios () { this->init (0); } } ios;

    // construct a money_put-derived facet to exercise
    struct MoneyPut: std::money_put<char, char*> { } mp;

    // imbue a stream object with a custom locale
    // containing the replacement punctuation facet
    std::locale l;
    std::locale loc (l, (FACET_CONST std::moneypunct<char>*)&pun);
    ios.imbue (loc);

    // decimal output, no special formatting
    ios.setf (std::ios::fmtflags ());

    char buf [40] = "";

    *mp.put (buf, false, ios, '\0', 123456789.0L) = '\0';

    // verify that the number was formatted correctly
    rw_assert (0 == rw_strncmp (buf, "1;2;3;4;5;6;7;8;9"), __FILE__, __LINE__,
               "money_put<char, char*>::do_put(..., 123456789.0L) "
               "== %#s, got %#s", "1;2;3;4;5;6;7;8;9", buf);

    // verify that do_thousands_sep() was called at least once
    // (but not necessarily for each thousands separator inserted
    // into the output, since the value is allowed to be cached)
    rw_assert (0 != PunctT::n_thousands_sep_, __FILE__, __LINE__,
               "money_put<char, char*>::do_put(..., 123456789.0L) "
               "called moneypunct<char>::do_thousands_sep()");
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    // check to see if we can test -0.0
    static volatile double d = -0.0;
    test_neg_zero = 1.0 / d < d;
    
    if (no_grouping)
        rw_note (0, __FILE__, __LINE__, "grouping test disabled");
    else
        grouping_test ();

    if (rw_enabled ("char"))
        ldbl_test (char ());
    else
        rw_note (0, __FILE__, __LINE__, "char test disabled");

#ifndef _RWSTD_NO_WCHAR_T

    if (rw_enabled ("wchar_t"))
        ldbl_test (wchar_t ());
    else
        rw_note (0, __FILE__, __LINE__, "wchar_t test disabled");

#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.locale.money.put",
                    0 /* no comment */, run_test,
                    "|-no-grouping# |-no-basic_string#",
                    &no_grouping, &no_basic_string);
}
