/***************************************************************************
 *
 * 22.locale.moneypunct.cpp - tests for the moneypunct facet
 *
 * $Id: 22.locale.moneypunct.cpp 651095 2008-04-23 22:50:27Z sebor $
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
 * Copyright 1998-2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#include <locale>

#include <cassert>
#include <clocale>       // for LC_ALL, setlocale()
#include <cstdio>        // for fprintf(), stderr
#include <cstdlib>       // for getenv()
#include <cstring>       // for memcmp(), strerror()

#include <driver.h>      // for rw_test()
#include <environ.h>     // for rw_putenv()
#include <rw_locale.h>   // for rw_locales()
#include <rw_printf.h>   // for rw_fprintf()

/**************************************************************************/

typedef unsigned char UChar;

/**************************************************************************/

// does a deep copy of struct lconv
std::lconv* lconvdup (const std::lconv *plconv)
{
    if (!plconv)
        return 0;

    const std::size_t
        decimal_point_sz     = std::strlen (plconv->decimal_point) + 1,
        thousands_sep_sz     = std::strlen (plconv->thousands_sep) + 1,
        grouping_sz          = std::strlen (plconv->grouping) + 1,
        int_curr_symbol_sz   = std::strlen (plconv->int_curr_symbol) + 1,
        currency_symbol_sz   = std::strlen (plconv->currency_symbol) + 1,
        mon_decimal_point_sz = std::strlen (plconv->mon_decimal_point) + 1,
        mon_thousands_sep_sz = std::strlen (plconv->mon_thousands_sep) + 1,
        mon_grouping_sz      = std::strlen (plconv->mon_grouping) + 1,
        positive_sign_sz     = std::strlen (plconv->positive_sign) + 1,
        negative_sign_sz     = std::strlen (plconv->negative_sign) + 1;

    const std::size_t total_sz =
          decimal_point_sz
        + thousands_sep_sz
        + grouping_sz
        + int_curr_symbol_sz
        + currency_symbol_sz
        + mon_decimal_point_sz
        + mon_thousands_sep_sz
        + mon_grouping_sz
        + positive_sign_sz
        + negative_sign_sz;

    const std::size_t bufsize = sizeof (std::lconv) + total_sz;

    char *pbuf = _RWSTD_STATIC_CAST (char*, operator new (bufsize));

    std::lconv *plconv2 = _RWSTD_REINTERPRET_CAST (std::lconv*, pbuf);

    plconv2->decimal_point = (pbuf += sizeof (std::lconv));
    std::memcpy (pbuf, plconv->decimal_point, decimal_point_sz);

    plconv2->thousands_sep = (pbuf += decimal_point_sz);
    std::memcpy (pbuf, plconv->thousands_sep, thousands_sep_sz);

    plconv2->grouping = (pbuf += thousands_sep_sz);
    std::memcpy (pbuf, plconv->grouping, grouping_sz);

    plconv2->int_curr_symbol = (pbuf += grouping_sz);
    std::memcpy (pbuf, plconv->int_curr_symbol, int_curr_symbol_sz);

    plconv2->currency_symbol = (pbuf += int_curr_symbol_sz);
    std::memcpy (pbuf, plconv->currency_symbol, currency_symbol_sz);

    plconv2->mon_decimal_point = (pbuf += currency_symbol_sz);
    std::memcpy (pbuf, plconv->mon_decimal_point, mon_decimal_point_sz);

    plconv2->mon_thousands_sep = (pbuf += mon_decimal_point_sz);
    std::memcpy (pbuf, plconv->mon_thousands_sep, mon_thousands_sep_sz);

    plconv2->mon_grouping = (pbuf += mon_thousands_sep_sz);
    std::memcpy (pbuf, plconv->mon_grouping, mon_grouping_sz);

    plconv2->positive_sign = (pbuf += mon_grouping_sz);
    std::memcpy (pbuf, plconv->positive_sign, positive_sign_sz);

    plconv2->negative_sign = (pbuf += positive_sign_sz);
    std::memcpy (pbuf, plconv->negative_sign, negative_sign_sz);

    plconv2->int_frac_digits   = plconv->int_frac_digits;
    plconv2->frac_digits       = plconv->frac_digits;
    plconv2->p_cs_precedes     = plconv->p_cs_precedes;
    plconv2->p_sep_by_space    = plconv->p_sep_by_space;
    plconv2->n_cs_precedes     = plconv->n_cs_precedes;
    plconv2->n_sep_by_space    = plconv->n_sep_by_space;
    plconv2->p_sign_posn       = plconv->p_sign_posn;
    plconv2->n_sign_posn       = plconv->n_sign_posn;

#ifndef _RWSTD_NO_LCONV_INT_FMAT

    plconv2->int_p_cs_precedes   = plconv->int_p_cs_precedes;
    plconv2->int_p_sep_by_space  = plconv->int_p_sep_by_space;
    plconv2->int_n_cs_precedes   = plconv->int_n_cs_precedes;
    plconv2->int_n_sep_by_space  = plconv->int_n_sep_by_space;
    plconv2->int_p_sign_posn     = plconv->int_p_sign_posn;
    plconv2->int_n_sign_posn     = plconv->int_n_sign_posn;

#endif   // _RWSTD_NO_LCONV_INT_FMAT

    return plconv2;
}

/**************************************************************************/

template <class charT>
class Test
{
    std::locale        loc_;
    bool               intl_;        // international?
    const char        *char_name_;   // charT name ("char" or "wchar_t")
    const char        *locname_;
    const char        *lang_;
    const char        *lc_all_;
    const char        *lc_monetary_;

public:

    typedef std::moneypunct<charT, false> Punct;
    typedef std::moneypunct<charT, true>  IntlPunct;

    Test (const char *cname, bool intl)
        : loc_ (), intl_ (intl),
          char_name_ (cname),
          locname_ (0), lang_ (0), lc_all_ (0), lc_monetary_ (0) { }

    void runTest ();

    void check_decimal_point (charT);
    void check_thousands_sep (charT);
    void check_frac_digits (int);
    void check_grouping (const std::string&);

    typedef std::char_traits<charT>                     Traits;
    typedef std::allocator<charT>                       Allocator;
    typedef std::basic_string<charT, Traits, Allocator> String;

    void check_curr_symbol (const String&);
    void check_positive_sign (const String&);
    void check_negative_sign (const String&);

    void check_format (const std::lconv&);
    void check_format (bool, std::money_base::pattern, const UChar [3]);

    bool check_moneypunct (const char*);
};

/**************************************************************************/

template <class charT>
void Test<charT>::
runTest()
{
    // create a copy of the classic C locale
    loc_ = std::locale::classic ();

    // verify (at compile time) that _bynam facets can be used
    // to specialize use_facet() and has_facet()
    typedef std::moneypunct_byname<charT, false> MoneyPunctByname;
    typedef std::moneypunct_byname<charT, true>  MoneyPunctIntlByname;

    if (1) {
        if (_STD_HAS_FACET (MoneyPunctByname, loc_))
            _RWSTD_USE_FACET (MoneyPunctByname, loc_);

        if (_STD_HAS_FACET (MoneyPunctIntlByname, loc_))
            _RWSTD_USE_FACET (MoneyPunctIntlByname, loc_);
    }

    // note that values in 7.4 of C89, and 7.11, p2 of C99, respectively,
    // or '\0' and '\0' for decimal_point and thousands_sep, are specified
    // in 22.2.3.1.2, p1 and p2 of C++98 to be '.', and ',' in the C locale

#if DRQS
    // Virtual function should return '.' and ',' for 'C' locale's
    // 'decimal_point()' function and 'do_thousands_sep()' function.
    check_decimal_point (charT ('.'));
    check_thousands_sep (charT (','));
#endif
    check_frac_digits (0 /* i.e., not available, or same as CHAR_MAX in C */);
    check_grouping ("");

    const charT empty[] = { '\0' };
    check_curr_symbol (empty);
    check_positive_sign (empty);
    check_negative_sign (empty);

    // verify 22.2.6.3.2, p7:
    // moneypunct<char>, moneypunct<wchar_t>, moneypunct<char,true>, and
    // moneypunct<wchar_t,true>, return an object of type pattern initialized
    // to { symbol, sign, none, value }
    std::money_base::pattern fmat = {
        // initializer must be properly bracketed to prevent g++ warnings
        {
            std::money_base::symbol,
            std::money_base::sign,
            std::money_base::none,
            std::money_base::value
        }
    };

    const UChar cpat [4] = { '?', '?', '?', 0 };
    check_format (false /* negative */, fmat, cpat);
    check_format (true  /* positive */, fmat, cpat);

    // exercise the native locale (affected by the environment
    // variables LANG, LC_ALL, LC_MONETARY, etc.)
    check_moneypunct ("");   // native locale

    // the name of the first non-C (and non-POSIX) locale
    const char *first_non_c = 0;

    // exercise named locales (including "C" and "POSIX")
    for (const char* s = rw_locales (); s && *s; s += std::strlen (s) + 1) {
        if (check_moneypunct (s))
            if (   !first_non_c
                && std::strcmp ("C", s)
                && std::strcmp ("POSIX", s)) {
                first_non_c = s;
            }
    }

    if (!first_non_c)
        return;

    // verify that moneypunct behaves correctly when LC_ALL is set
    // to the name of the (non-C, non-POSIX) locale
    char envvar [80];

    // make sure buffer doesn't overflow
    assert (std::strlen(first_non_c) + sizeof "LC_ALL=" <= sizeof envvar);

    std::sprintf (envvar, "LC_ALL=%s", first_non_c);
    rw_putenv (envvar);
    check_moneypunct ("");

    // remove LC_ALL from the environment
    rw_putenv ("LC_ALL=");
}

/**************************************************************************/

std::string convert (const char*, const char *s, const char*)
{
    return std::string (s);
}


// convert a multibyte character string in an external representation
// to wstring object in an internal representation
std::wstring
convert (const char *locname, const char *s, const wchar_t*)
{
    // save the name of the original locale
    std::string saved_loc;

    if (const char *savename = std::setlocale (LC_ALL, 0))
        saved_loc = savename;

    // switch to (named) locale
    const char *loc = std::setlocale (LC_ALL, locname);
    if (!loc) {
        rw_fprintf (rw_stderr,
                    "%s:%d: setlocale (LC_ALL, %s) = 0: %m\n",
                    locname);
        return std::wstring ();
    }

    // use an extension: allocate but do not initialize
#if TEST_RW_EXTENSIONS
    std::wstring res ((wchar_t*)0, 64);
#else
    std::wstring res;
    res.reserve(64);
#endif

    for ( ; ; ) {
        // try to convert, resizing buffer if necessary
        std::size_t n =
            std::mbstowcs (&res [0], s, res.capacity ());

        if (res.capacity () == n)
            // increase capacity
            res.reserve (res.capacity () * 2);
        else if (std::size_t (-1) == n) {

            // restore the original locale before printing out
            // the error message (we don't want it localized)
            std::setlocale (LC_ALL, saved_loc.c_str ());

            rw_fprintf (rw_stderr,
                        "%s:%d: mbstowcs(..., %#s, %zu) "
                        "= -1: %m\n", __FILE__, __LINE__,
                        s, res.capacity ());
            res = std::wstring ();   // mbstowcs() error

            return res;
        }
        else {
            // shrink if necessary
            res.resize (n);
            break;
        }
    }

    // restore original locale
    std::setlocale (LC_ALL, saved_loc.c_str ());

    return res;
}

/**************************************************************************/

template <class charT>
bool Test<charT>::
check_moneypunct (const char *locname)
{
    // (try to) set the global C locale
    char locnamebuf [256];
    const char *loc = std::setlocale (LC_ALL, locname);
    if (!loc)
        return false;

    loc = std::strcpy (locnamebuf, loc);

    locname_ = loc;

    // the values of the environment variables LANG, LC_ALL,
    // and LC_MONETARY (and others) affect the native locale
    lang_        = std::getenv ("LANG");
    lc_all_      = std::getenv ("LC_ALL");
    lc_monetary_ = std::getenv ("LC_MONETARY");

    if (!lang_)
        lang_ = "(null)";
    if (!lc_all_)
        lc_all_ = "(null)";
    if (!lc_monetary_)
        lc_monetary_ = "(null)";

    _TRY {

        // get a pointer to lconv and copy data to a temporray buffer
        const std::lconv* const plconv = lconvdup (std::localeconv ());

        if (!plconv)
            return false;

        // reset to default locale given by LC_LANG
        std::setlocale (LC_ALL, "");

        // create a new C++ locale object
        loc_ = std::locale (locname);

        // check that newly constructed locale matches

        // the MBCS mon_decimal_point and mon_thousands_sep must
        // be properly converted (i.e., *mon_decimal_point may not
        // the same as s [0] after the conversion)
        String s;

        // `locname' may be the empty string, in which case `loc'
        // will be set to the actual name of the locale
        if (   std::strcmp ("C", locname)
            && std::strcmp ("C", loc)) {

#if DRQS
    // 'decimal_point()' and 'thousands_sep()' has bug.

            // named locale other than "C" or "POISX"
            s = ::convert (locname, plconv->mon_decimal_point, (charT*)0);
            check_decimal_point (s.size () ? s [0] : charT ());

            s = ::convert (locname, plconv->mon_thousands_sep, (charT*)0);
            check_thousands_sep (s.size () ? s [0] : charT ());
#endif
            check_frac_digits (plconv->int_frac_digits);
        }
        else {
            // note that values in 7.4 of C89, and 7.11, p2 of C99,
            // respectively, or '\0' and '\0' for decimal_point and
            // thousands_sep, are specified in 22.2.3.1.2, p1 and p2
            // of C++98 to be '.', and ',' in the C locale

#if DRQS
    // 'decimal_point()' and 'thousands_sep()' has bug.
            check_decimal_point (charT ('.'));
            check_thousands_sep (charT (','));
#endif

            // frac_digits is specified as CHAR_MAX (i.e., not available) by C99
            // verify that the C++ value is NOT CHAR_MAX, but rather 0 (it could
            // be negative with the same result)
            check_frac_digits (0);
        }

        check_grouping (plconv->mon_grouping);

        // convert a (possibly) multibyte string in external
        // representation to one in internal representation
        s = intl_ ? ::convert (locname, plconv->int_curr_symbol, (charT*)0)
                  : ::convert (locname, plconv->currency_symbol, (charT*)0);

        check_curr_symbol (s);

        s = ::convert (locname, plconv->positive_sign, (charT*)0);

        check_positive_sign (s);

        s = ::convert (locname, plconv->negative_sign, (charT*)0);

        check_negative_sign (s);

        check_format (*plconv);

        // cast away constness to work around compiler bugs (e.g., MSVC 6)
        operator delete (_RWSTD_CONST_CAST (std::lconv*, plconv));
    }
    _CATCH (...) {
        return false;
    }
    return true;
}

/**************************************************************************/

template <class charT>
void Test<charT>::
check_decimal_point (charT result)
{
    const charT c = intl_ ? _RWSTD_USE_FACET (IntlPunct, loc_).decimal_point ()
                          : _RWSTD_USE_FACET (Punct, loc_).decimal_point ();

    rw_assert (std::char_traits<charT>::eq (c, result), 0, __LINE__,
               "moneypunct<%s, %b>::decimal_point() == %#lc, got %#lc "
               "in locale (\"%s\") with LANG=%s, LC_ALL=%s, LC_MONETARY=%s",
               char_name_, intl_, result, c,
               locname_, lang_, lc_all_, lc_monetary_);
}

/**************************************************************************/

template <class charT>
void Test<charT>::
check_thousands_sep (charT result)
{
    const charT c = intl_ ? _RWSTD_USE_FACET (IntlPunct, loc_).thousands_sep ()
                          : _RWSTD_USE_FACET (Punct, loc_).thousands_sep ();

    rw_assert (std::char_traits<charT>::eq (c, result), 0, __LINE__,
               "moneypunct<%s, %b>::thousands_sep() == %#lc, got %#lc "
               "in locale (\"%s\") with LANG=%s, LC_ALL=%s, LC_MONETARY=%s",
               char_name_, intl_, result, c,
               locname_, lang_, lc_all_, lc_monetary_);
}

/**************************************************************************/

template <class charT>
void Test<charT>::
check_frac_digits (int result)
{
    const int i = intl_ ? _RWSTD_USE_FACET (IntlPunct, loc_).frac_digits ()
                        : _RWSTD_USE_FACET (Punct, loc_).frac_digits ();

    rw_assert (i == result, 0, __LINE__,
               "moneypunct<%s, %b>::frac_digits() == %d, got %d "
               "in locale (\"%s\") with LANG=%s, LC_ALL=%s, LC_MONETARY=%s",
               char_name_, intl_, result, i,
               locname_, lang_, lc_all_, lc_monetary_);
}

/**************************************************************************/

template <class charT>
void Test<charT>::
check_grouping (const std::string &result)
{
    const std::string s = intl_ ?
          _RWSTD_USE_FACET (IntlPunct, loc_).grouping ()
        : _RWSTD_USE_FACET (Punct, loc_).grouping ();

    if (   s != result
        && s.size () != result.size () && s.size () && result.size ()) {

        // if the grouping is not exactly the same as the expected result,
        // verify that the actual grouping is equivalent to the expected
        // one, e.g., that "\003\003" is equivalent to "\003"

        const std::string *lng  = s.size () > result.size () ? &result : &s;
        const std::string *shrt = s.size () < result.size () ? &result : &s;

        std::size_t i = shrt->size () - 1;

        for ( ; i != lng->size (); ++i)
            if ((*shrt) [shrt->size () - 1] != (*lng)[i])
                break;

        rw_assert (i == lng->size (), 0, __LINE__,
                   "numpunct<%s, %b>::grouping() equivalent to %#s, got %#s "
                   "in locale (%#s) with LANG=%s, LC_ALL=%s, "
                   "LC_MONETARY=%s",
                   char_name_, intl_, result.c_str (), s.c_str (),
                   locname_, lang_, lc_all_, lc_monetary_);
    }
    else
        rw_assert (s == result, 0, __LINE__,
                   "moneypunct<%s, %b>::grouping() == %#s, got %#s "
                   "in locale (%#s) with LANG=%s, LC_ALL=%s, "
                   "LC_MONETARY=%s",
                   char_name_, intl_, result.c_str (), s.c_str (),
                   locname_, lang_, lc_all_, lc_monetary_);

#if 0

    // Test disabled: what's meant here is that the pattern, NOT the value,
    // is defined identically as that of numpunct<charT>::do_grouping().
    // The values are often going to be different since one uses
    // lconv::grouping, and the other lconv::mon_grouping.

    // 22.2.6.3.2, p3: do_grouping() returns a pattern defined identically
    //                 as the result of numpunct<charT>::do_grouping().

    const std::string grp =
        _RWSTD_USE_FACET (std::numpunct<charT>, loc_).grouping ();

    rw_assert (s == grp, 0, __LINE__,
               "moneypunct<%s, %b>::grouping() == numpunct<>::grouping() == "
               "%{*Ac}, got %{*Ac} in locale (\"%s\") with LANG=%s, LC_ALL=%s, "
               "LC_MONETARY=%s",
               char_name_, intl_,
               int (sizeof (charT)), result.c_str (),
               int (sizeof (charT)), s.c_str (),
               locname_, lang_, lc_all_, lc_monetary_);

#endif   // 0

}

/**************************************************************************/

template <class charT>
void Test<charT>::
check_curr_symbol (const String &result)
{
    const String s = intl_ ? _RWSTD_USE_FACET (IntlPunct, loc_).curr_symbol ()
                           : _RWSTD_USE_FACET (Punct, loc_).curr_symbol ();

    rw_assert (s == result, 0, __LINE__,
               "moneypunct<%s, %b>::curr_symbol() == %{*Ac}, got %{*Ac} "
               "in locale (\"%s\") with LANG=%s, LC_ALL=%s, LC_MONETARY=%s",
               char_name_, intl_,
               int (sizeof (charT)), result.c_str (),
               int (sizeof (charT)), s.c_str (),
               locname_, lang_, lc_all_, lc_monetary_);
}

/**************************************************************************/

template <class charT>
void Test<charT>::
check_positive_sign (const String &result)
{
    const String s = intl_ ?
          _RWSTD_USE_FACET (IntlPunct, loc_).positive_sign ()
        : _RWSTD_USE_FACET (Punct, loc_).positive_sign ();

    rw_assert (s == result, 0, __LINE__,
               "moneypunct<%s, %b>::positive_sign() == %{*Ac}, got %{*Ac} "
               "in locale (\"%s\") with LANG=%s, LC_ALL=%s, LC_MONETARY=%s",
               char_name_, intl_,
               int (sizeof (charT)), result.c_str (),
               int (sizeof (charT)), s.c_str (),
               locname_, lang_, lc_all_, lc_monetary_);
}

/**************************************************************************/

template <class charT>
void Test<charT>::
check_negative_sign (const String &result)
{
    const String s = intl_ ?
          _RWSTD_USE_FACET (IntlPunct, loc_).negative_sign ()
        : _RWSTD_USE_FACET (Punct, loc_).negative_sign ();

    rw_assert (s == result, 0, __LINE__,
               "moneypunct<%s, %b>::negative_sign() == %{*Ac}, got %{*Ac} "
               "in locale (\"%s\") with LANG=%s, LC_ALL=%s, LC_MONETARY=%s",
               char_name_, intl_,
               int (sizeof (charT)), result.c_str (),
               int (sizeof (charT)), s.c_str (),
               locname_, lang_, lc_all_, lc_monetary_);
}

/**************************************************************************/

template <class charT>
void Test<charT>::
check_format (const std::lconv &lc)
{
    const UChar cpat [2][2][3] = {
        //            ^  ^  ^
        //            |  |  |
        //            |  |  +--- cs_precedes, sep_by_space, sign_posn
        //            |  +------ positive, negative
        //            +--------- local, international

        {
            // C90 positive local format
            { lc.p_cs_precedes, lc.p_sep_by_space, lc.p_sign_posn },

            // C90 negative local format
            { lc.n_cs_precedes, lc.n_sep_by_space, lc.n_sign_posn }
        },

#ifndef _RWSTD_NO_LCONV_INT_FMAT

        {   // C99 positive international format
            { lc.int_p_cs_precedes, lc.int_p_sep_by_space, lc.int_p_sign_posn},

            // C99 negative international format
            { lc.int_n_cs_precedes, lc.int_n_sep_by_space, lc.int_n_sign_posn }
        }

#else   // if defined (_RWSTD_NO_LCONV_INT_FMAT)

        {
            { lc.p_cs_precedes, lc.p_sep_by_space, lc.p_sign_posn },
            { lc.n_cs_precedes, lc.n_sep_by_space, lc.n_sign_posn }
        }

#endif   // _RWSTD_NO_LCONV_INT_FMAT
    };

    // cs_precedes [0..1]:
    //
    // An integer set to 1 if the currency_symbol precedes the value
    // for a monetary value, and set to 0 if the symbol succeeds
    // the value.

    // sep_by_space [0..2]:
    //
    // 0  No space separates the currency_symbol from the value for
    //    a monetary value.
    // 1  If the currency symbol and sign string are adjacent, a space
    //    separates them from the value; otherwise, a space separates
    //    the currency symbol from the value.
    // 2  If the currency symbol and sign string are adjacent, a space
    //    separates them; otherwise, a space separates the sign string
    //    from the value.

    // sign_posn [0..4]:
    //
    // An integer set to a value indicating the positioning of the
    // positive_sign for a monetary value. The following integer
    // values shall be recognized:
    //
    // 0  Parentheses enclose the value and the currency_symbol.
    // 1  The sign string precedes the value and the currency_symbol.
    // 2  The sign string succeeds the value and the currency_symbol.
    // 3  The sign string immediately precedes the currency_symbol.
    // 4  The sign string immediately succeeds the currency_symbol.

    enum {
        none   = std::money_base::none,
        space  = std::money_base::space,
        symbol = std::money_base::symbol,
        sign   = std::money_base::sign,
        value  = std::money_base::value
    };

    static const std::money_base::pattern patterns[] = {

        /* 000: -1$. */ { { sign, value, symbol, none } },   // "\3\4\2\0"
        /* 001: -1$. */ { { sign, value, symbol, none } },   // "\3\4\2\0"
        /* 002: 1$-. */ { { value, symbol, sign, none } },   // "\4\2\3\0"
        /* 003: 1-$. */ { { value, sign, symbol, none } },   // "\4\3\2\0"
        /* 004: 1$-. */ { { value, symbol, sign, none } },   // "\4\2\3\0"

        /* 010: -1 $ */ { { sign, value, space, symbol } },  // "\3\4\1\2"
        /* 011: -1 $ */ { { sign, value, space, symbol } },  // "\3\4\1\2"
        /* 012: 1 $- */ { { value, space, symbol, sign } },  // "\4\1\2\3"
        /* 013: 1 -$ */ { { value, space, sign, symbol } },  // "\4\3\3\2"
        /* 014: 1 $- */ { { value, space, symbol, sign } },  // "\4\1\2\3"

        /* 020: - 1$ */ { { sign, space, value, symbol } },  // "\3\1\4\2"
        /* 021: - 1$ */ { { sign, space, value, symbol } },  // "\3\\14\2"
        /* 022: 1$ - */ { { value, symbol, space, sign } },  // "\4\2\1\3"
        /* 023: 1- $ */ { { value, sign, space, symbol } },  // "\4\3\1\2"
        /* 024: 1$ - */ { { value, symbol, space, sign } },  // "\4\2\1\3"

        /* 100: -$1. */ { { sign, symbol, value, none } },   // "\3\2\4\0"
        /* 101: -$1. */ { { sign, symbol, value, none } },   // "\3\2\4\0"
        /* 102: $1-. */ { { symbol, value, sign, none } },   // "\2\4\3\0"
        /* 103: -$1. */ { { sign, symbol, value, none } },   // "\3\2\4\0"
        /* 104: $-1. */ { { symbol, sign, value, none } },   // "\2\3\4\0"

        /* 110: -$ 1 */ { { sign, symbol, space, value } },  // "\3\2\1\4"
        /* 111: -$ 1 */ { { sign, symbol, space, value } },  // "\3\2\1\4"
        /* 112: $ 1- */ { { symbol, space, value, sign } },  // "\2\1\4\3"
        /* 113: -$ 1 */ { { sign, symbol, space, value } },  // "\3\2\1\4"
        /* 114: $- 1 */ { { symbol, sign, space, value } },  // "\2\3\1\4"

        /* 120: - $1 */ { { sign, space, symbol, value } },  // "\3\1\2\4"
        /* 121: - $1 */ { { sign, space, symbol, value } },  // "\3\1\2\4"
        /* 122: $1 - */ { { symbol, value, space, sign } },  // "\2\4\1\3"
        /* 123: - $1 */ { { sign, space, symbol, value } },  // "\3\1\2\4"
        /* 124: $ -1 */ { { symbol, space, sign, value } }   // "\2\1\3\4"
    };

    for (int neg = 0; neg != 2; ++neg) {

        enum { cs_precedes, sep_by_space, sign_posn };

        // ignore unspecified formats (-1)
        if (   cpat [intl_][neg][cs_precedes] > 1
            || cpat [intl_][neg][sep_by_space] > 2
            || cpat [intl_][neg][sign_posn] > 4)
            continue;

        const int inx =
              cpat [intl_][neg][cs_precedes] * 3 * 5
            + cpat [intl_][neg][sep_by_space] * 5
            + cpat [intl_][neg][sign_posn];

        check_format (!neg, patterns [inx], cpat [intl_][neg]);
    }
}

/**************************************************************************/

template <class charT>
void Test<charT>::
check_format (bool pos, std::money_base::pattern result, const UChar cpat [3])
{
    const std::money_base::pattern pat =
        intl_ ? pos ? _RWSTD_USE_FACET (IntlPunct, loc_).pos_format ()
                    : _RWSTD_USE_FACET (IntlPunct, loc_).neg_format ()
              : pos ? _RWSTD_USE_FACET (Punct, loc_).pos_format ()
                    : _RWSTD_USE_FACET (Punct, loc_).neg_format ();

    // number of times each symbol appears in pattern
    int counts [5] = { 0 };

    for (unsigned i = 0; i != sizeof pat.field / sizeof *pat.field; i++) {

        if (UChar (pat.field [i] < char (sizeof counts / sizeof *counts)))
            ++counts [UChar (pat.field [i])];

        // verify 22.2.6.3, p1
        if (std::money_base::none == pat.field [i] && !i)
            rw_assert (false, 0, __LINE__,
                       "moneypunct<%s, %b>::%s_format() == %{LM}, "
                       "none must not appear first",
                       char_name_, intl_, pos ? "pos" : "neg",
                       pat.field);

        if (   std::money_base::space == pat.field [i]
            && !i && i != sizeof pat.field / sizeof *pat.field - 1)
            rw_assert (false, 0, __LINE__,
                       "moneypunct<%s, %b>::%s_format() == %{LM}, "
                       "space must not appear first or last",
                       char_name_, intl_, pos ? "pos" : "neg",
                       pat.field);
    }

    // verify that the actual pattern matches the expected one
    rw_assert (0 == std::memcmp (&pat, &result, sizeof pat), 0, __LINE__,
               "moneypunct<%s, %b>::%s_format() == %{LM}, got %{LM}; "
               "(cs_precedes = '\\%o', sep_by_space = '\\%o', "
               "sign_posn = '\\%o')",
               char_name_, intl_, pos ? "pos" : "neg",
               pat.field, result.field, cpat [0], cpat [1], cpat [2]);

    // verify 22.2.6.3, p1
    rw_assert (1 == counts [std::money_base::symbol], 0, __LINE__,
               "money_base::symbol must apear exactly once, did %d times",
               counts [std::money_base::symbol]);

    rw_assert (1 == counts [std::money_base::sign], 0, __LINE__,
               "money_base::sign must apear exactly once, did %d times",
               counts [std::money_base::sign]);

    rw_assert (1 == counts [std::money_base::value], 0, __LINE__,
               "money_base::value must apear exactly once, did %d times",
               counts [std::money_base::value]);

    rw_assert (1 ==   counts [std::money_base::space]
                    + counts [std::money_base::none], 0, __LINE__,
               "money_base::space or money_base::none must appear "
               "exactly once, did %d times",
                 counts [std::money_base::space]
               + counts [std::money_base::none]);
}

/****************************************************************************/

static int
run_test (int, char**)
{
    {
        Test<char> t ("char", false);
        t.runTest ();
    }

    {
        Test<char> t ("char", true);
        t.runTest ();
    }

#ifndef _RWSTD_NO_WCHAR_T

    {
        Test<wchar_t> t ("wchar_t", false);
        t.runTest ();
    }

    {
        Test<wchar_t> t ("wchar_t", true);
        t.runTest ();
    }

#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}


/****************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.locale.moneypunct",
                    0 /* no comment */,
                    run_test,
                    "",
                    (void*)0   /* sentinel */);
}
