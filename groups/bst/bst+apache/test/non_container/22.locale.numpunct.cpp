/************************************************************************
 *
 * 22.locale.numpunct.cpp
 *
 * test exercising the std::numpunct facet
 *
 * $Id: 22.locale.numpunct.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 **************************************************************************/

#include <bsls_platform.h>

#include <rw/_defs.h>

#if defined (__IBMCPP__) && !defined (_RWSTD_NO_IMPLICIT_INCLUSION)
// Disable implicit inclusion to work around
// a limitation in IBM's VisualAge 5.0.2.0 (see PR#26959)

#  define _RWSTD_NO_IMPLICIT_INCLUSION
#endif


#include <clocale>   // for struct lconv, setlocale()
#include <cstdio>    // for sprintf()
#include <cstdlib>   // for getenv()
#include <cstring>   // for strcmp()
#include <locale>

#include <driver.h>
#include <environ.h>
#include <rw_locale.h>

/**************************************************************************/

template <class charT>
void run_test (charT*);

template <class charT>
void check_decimal_point (charT, const char*, const std::locale*);

template <class charT>
void check_thousands_sep (charT, const char*, const std::locale*);

template <class charT>
void check_grouping (charT, const char*, const std::string&, const std::locale*);

template <class charT>
void check_falsename (const std::basic_string<charT>&,
                      const char*, const std::locale*);

template <class charT>
void check_truename (const std::basic_string<charT>&,
                     const char*, const std::locale*);

template <class charT>
bool check_numpunct (charT, const char*, const char*, const std::locale*);


/**************************************************************************/

template <class charT>
void run_test (charT, const char *tname)
{
    rw_info (0, 0, __LINE__, "classic \"C\" locale");

    // create a copy of the classic ("C") locale
    std::locale *loc = new std::locale (std::locale::classic ());

    // verify (at compile time) that _byname facets can be used
    // to specialize use_facet() and has_facet()
    typedef std::numpunct_byname<charT> NumpunctByname;

    if (_STD_HAS_FACET (NumpunctByname, *loc))
        _STD_USE_FACET (NumpunctByname, *loc);

    // verify values as per 7.4 of C89, and 7.11, p2 of C99
    check_decimal_point (charT ('.'), tname, loc);

    // verify 22.2.3.1.2, p2
    check_thousands_sep (charT (','), tname, loc);
    check_grouping (charT (), tname, "", loc);

    const charT fn[] = { 'f', 'a', 'l', 's', 'e', '\0' };
    const charT tn[]  = { 't', 'r', 'u', 'e', '\0' };

    check_falsename (std::basic_string<charT>(fn), tname, loc);
    check_truename (std::basic_string<charT>(tn), tname, loc);

    delete loc;
    loc = 0;

    //////////////////////////////////////////////////////////////////
    // verify that the facet can be safely used on its own, without
    // first having to be installed in a locale and retrieved using
    // use_facet

    check_decimal_point (charT ('.'), tname, loc);
    check_thousands_sep (charT (','), tname, loc);
    check_grouping (charT (), tname, "", loc);

    check_falsename (std::basic_string<charT>(fn), tname, loc);
    check_truename (std::basic_string<charT>(tn), tname, loc);

    //////////////////////////////////////////////////////////////////
    // exercise the native locale (affected by the environment
    // variables LANG, LC_ALL, LC_NUMERIC, etc.)
    check_numpunct (charT (), tname, "", loc);   // native locale

    // the name of the first non-C (and non-POSIX) locale
    const char *first_non_c = 0;

    // exercise named locales (including "C" and "POSIX")
    for (const char* s = rw_locales (); *s; s += std::strlen (s) + 1) {
        if (check_numpunct (charT (), tname, s, loc))
            if (   !first_non_c
                && std::strcmp ("C", s)
                && std::strcmp ("POSIX", s))
            first_non_c = s;
    }

#if !defined(BSLS_PLATFORM_CMP_SUN) && !defined(BSLS_PLATFORM_CMP_AIX)
    // Note that there's no non-C locale implemented in stlport for SUN and AIX

    // verify that numpunct behaves correctly when LC_ALL is set
    // to the name of the (non-C, non-POSIX) locale
    char envvar [80];
    std::sprintf (envvar, "LC_ALL=%s", first_non_c);
    rw_putenv (envvar);
    check_numpunct (charT (), tname, "", loc);
#endif
}

/**************************************************************************/

char widen (char, const char *s)
{
    return *s;
}

#ifndef _RWSTD_NO_WCHAR_T

wchar_t widen (wchar_t, const char *s)
{
    wchar_t wc = 0;

#  ifndef _RWSTD_NO_MBTOWC

    const int n = s && *s ? std::mbtowc (&wc, s, std::strlen (s)) : 0;

#  else   // if defined (_RWSTD_NO_MBTOWC)

    const int n = 1;

    wc = s ? wchar_t (_RWSTD_STATIC_CAST (unsigned char, *s)) : 0;

#  endif   // _RWSTD_NO_MBTOWC

    return n > 0 ? wc : n ? -1 : 0;
}

#endif   // _RWSTD_NO_WCHAR_T


template <class charT>
bool check_numpunct (charT,
                     const char        *tname,
                     const char        *locname,
                     const std::locale *loc)
{
    // (try to) set the global C locale
    const char *tmploc = std::setlocale (LC_ALL, locname);
    if (!tmploc)
        return false;

    // future setlocale() calls may change tmploc's value, so make a copy
    // (fixed-size char buf is clunky but avoids memory-leak potential)
    char locnamebuf [1024];
    RW_ASSERT ((std::strlen (tmploc) + 1) < sizeof locnamebuf);
    std::strcpy (locnamebuf, tmploc);

    _TRY {

        rw_info (0, 0, __LINE__,
                 "locale (%s) -> \"%s\"; LANG=%s, "
                 "LC_ALL=%s, LC_NUMERIC=%s",
                 locname, loc,
                 std::getenv ("LANG"),
                 std::getenv ("LC_ALL"),
                 std::getenv ("LC_NUMERIC"));

        // get a pointer to lconv
        const std::lconv *plconv = std::localeconv ();

        if (!plconv)
            return false;

        // copy important data; the contents of *plconv may
        // be overwritten by the call sto setlocale() below
        // note that decimal_point at al may be multbyte character
        // strings that need to be widened according to the rules
        // of the same locale
        const charT decimal_point = widen (charT (), plconv->decimal_point);
        const charT thousands_sep = widen (charT (), plconv->thousands_sep);

        const std::string grouping =
            plconv->grouping ? plconv->grouping : "";

        // reset to default locale given by LC_LANG
        std::setlocale (LC_ALL, "");

        loc = new std::locale (locname);

        // check that newly constructed locale matches

        if (   'C' == locnamebuf [0] && '\0' == locnamebuf [1]
            || 'C' == locname [0] && '\0' == locname [1]
            || *loc == std::locale::classic ()) {
            // follow requirements in 22.2.3.1.2. p1, 2, and 3
            check_decimal_point (charT ('.'), tname, loc);
            check_thousands_sep (charT (','), tname, loc);

            check_grouping (charT (), tname, "", loc);
        }
        else {
            // cast to prevent sign extension between (signed) char and wchar_t
            // tmp guards against possibly passing along a "(nil)" pointer
            check_decimal_point (decimal_point, tname, loc);
            check_thousands_sep (thousands_sep, tname, loc);
            check_grouping (charT (), tname, grouping.c_str (), loc);
        }

        // FIXME: exercise falsename() and truename() in named locales

        delete loc;
        loc = 0;
    }
    _CATCH (...) {
        return false;
    }
    return true;
}

/**************************************************************************/

template <class charT>
void check_decimal_point (charT              expect,
                          const char        *tname,
                          const std::locale *loc)
{
#if TEST_RW_EXTENSIONS
// protected destructor for numpunct<> is public in RW?
    typedef std::numpunct<charT>    Punct;
#else
    struct Punct : std::numpunct<charT> { };
#endif
    typedef std::char_traits<charT> Traits;

    const charT c = loc ?
          _STD_USE_FACET (std::numpunct<charT>, *loc).decimal_point ()
        : Punct ().decimal_point ();


    rw_assert (Traits::eq (c, expect), 0, __LINE__,
               "numpunct<%s>::decimal_point() == %{#lc}, got %{#lc}",
               tname, expect, c);
}

/**************************************************************************/

template <class charT>
void check_thousands_sep (charT              expect,
                          const char        *tname,
                          const std::locale *loc)
{
#if TEST_RW_EXTENSIONS
// protected destructor for numpunct<> is public in RW?
    typedef std::numpunct<charT>    Punct;
#else
    struct Punct : std::numpunct<charT> { };
#endif
    typedef std::char_traits<charT> Traits;

    const charT c = loc ?
          _STD_USE_FACET (std::numpunct<charT>, *loc).thousands_sep ()
        : Punct ().thousands_sep ();

    rw_assert (Traits::eq (c, expect), 0, __LINE__,
               "numpunct<%s>::thousands_sep() == %{#lc}, got %{#lc}",
               tname, expect, c);
}

/**************************************************************************/

template <class charT>
void check_grouping (charT,
                     const char        *tname,
                     const std::string &expect,
                     const std::locale *loc)
{
#if TEST_RW_EXTENSIONS
// protected destructor for numpunct<> is public in RW?
    typedef std::numpunct<charT> Punct;
#else
    struct Punct : std::numpunct<charT> { };
#endif

    const std::string s = loc ?
          _STD_USE_FACET (std::numpunct<charT>, *loc).grouping ()
        : Punct ().grouping ();

    if (   s != expect
        && s.size () != expect.size () && s.size () && expect.size ()) {

        // if the grouping is not exactly the same as the expected result,
        // verify that the actual grouping is equivalent to the expected
        // one, e.g., that "\003\003" is equivalent to "\003"

        const std::string *lng  = s.size () > expect.size () ? &expect : &s;
        const std::string *shrt = s.size () < expect.size () ? &expect : &s;

        std::size_t i = shrt->size () - 1;

        for ( ; i != lng->size (); ++i)
            if ((*shrt) [shrt->size () - 1] != (*lng)[i])
                break;

        rw_assert (i == lng->size (), 0, __LINE__,
                   "numpunct<%s>::grouping() equivalent to %{#S}, got %{#S}",
                   tname, &expect, &s);
    }
    else
        rw_assert (s == expect, 0, __LINE__,
                   "numpunct<%s>::grouping() == %{#S}, got %{#S}",
                   tname, &expect, &s);
}

/**************************************************************************/

template <class charT>
void check_falsename (const std::basic_string<charT> &expect,
                      const char                     *tname,
                      const std::locale              *loc)
{
#if TEST_RW_EXTENSIONS
// protected destructor for numpunct<> is public in RW?
    typedef std::numpunct<charT> Punct;
#else
    struct Punct : std::numpunct<charT> { };
#endif

    const std::basic_string<charT> s = loc ?
          _STD_USE_FACET (std::numpunct<charT>, *loc).falsename ()
        : Punct ().falsename ();

    const int char_size = int (sizeof (charT));

    rw_assert (s == expect, 0, __LINE__,
               "numpunct<%s>::falsename() == %{#*S}, got %{#*S}",
               tname, char_size, &expect, char_size, &s);
}

/**************************************************************************/

template <class charT>
void check_truename (const std::basic_string<charT> &expect,
                     const char                     *tname,
                     const std::locale              *loc)
{
#if TEST_RW_EXTENSIONS
// protected destructor for numpunct<> is public in RW?
    typedef std::numpunct<charT> Punct;
#else
    struct Punct : std::numpunct<charT> { };
#endif

    const std::basic_string<charT> s = loc ?
          _STD_USE_FACET (std::numpunct<charT>, *loc).truename ()
        : Punct ().truename ();

    const int char_size = int (sizeof (charT));

    rw_assert (s == expect, 0, __LINE__,
               "numpunct<%s>::truename() == %{#*S}, got %{#*S}",
               tname, char_size, &expect, char_size, &s);
}

/**************************************************************************/

static int
run_test (int, char**)
{
    run_test (char (), "char");

#ifndef _RWSTD_WCHAR_T

    run_test (wchar_t (), "wchar_t");

#endif   // _RWSTD_WCHAR_T

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.locale.numpunct",
                    "", run_test,
                    "",
                    (void*)0);
}
