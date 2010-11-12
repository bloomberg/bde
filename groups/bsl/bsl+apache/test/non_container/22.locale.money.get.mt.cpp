/************************************************************************
 *
 * 22.locale.money.get.mt.cpp
 *
 * test exercising the thread safety of the money_get facet
 *
 * $Id: 22.locale.money.get.mt.cpp 648752 2008-04-16 17:01:56Z faridz $
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

#include <ios>        // for ios
#include <iterator>   // for ostreambuf_iterator
#include <locale>     // for locale, money_get

#include <cstring>    // for strlen()

#include <rw_locale.h>
#include <rw_thread.h>
#include <driver.h>
#include <valcmp.h>    // for rw_strncmp ()

#include <sstream>    // for converting

// maximum number of threads allowed by the command line interface
#define MAX_THREADS      32
#define MAX_LOOPS    100000

// the number of times each thread should iterate (unless specified
// otherwise on the command line)
int opt_nloops = 100000;

// default number of threads (will be adjusted to the number
// of processors/cores later)
int opt_nthreads = 1;

#if !defined (_RWSTD_OS_HP_UX) || defined (_ILP32)

// number of locales to use
int opt_nlocales = MAX_THREADS;

#else   // HP-UX in LP64 mode

// work around a small cache size on HP-UX in LP64 mode
// in LP64 mode (see STDCXX-812)
int opt_nlocales = 9;

#endif   // HP-UX 32/64 bit mode

// should all threads share the same set of locale objects instead
// of creating their own?
int opt_shared_locale;

/**************************************************************************/

// array of locale names to use for testing
static const char*
locales [MAX_THREADS];

// number of locale names in the array
static std::size_t
nlocales;

/**************************************************************************/

//
struct MyMoneyData
{
    enum { BufferSize = 16 };

    enum GetId {
        get_ldbl,
        get_string,
        get_max
    };

    // name of the locale the data corresponds to
    const char* locale_name_;

    // optionally set to the named locale for threads to share
    std::locale locale_;

    // international or domestic format flag
    bool intl_;

    //
    long double units_;

    // narrow locale specific representations of units_
    char ncs_ [BufferSize];
    std::string ncs_digits_;

#ifndef _RWSTD_NO_WCHAR_T

    // wide locale specific representations of units_
    wchar_t wcs_ [BufferSize];
    std::wstring wcs_digits_;

#endif // _RWSTD_NO_WCHAR_T

} my_money_data [MAX_THREADS];


template <class charT, class Traits>
struct MyIos: std::basic_ios<charT, Traits>
{
    MyIos () {
        this->init (0);
    }
};


template <class charT, class Traits>
struct MyStreambuf: std::basic_streambuf<charT, Traits>
{
    typedef std::basic_streambuf<charT, Traits> Base;

    MyStreambuf ()
        : Base () {
    }

    void pubsetg (const charT *gbeg, std::streamsize n) {
        this->setg (_RWSTD_CONST_CAST (charT*, gbeg),
                    _RWSTD_CONST_CAST (charT*, gbeg),
                    _RWSTD_CONST_CAST (charT*, gbeg) + n);
    }

    void pubsetp (charT *pbeg, std::streamsize n) {
        this->setp (pbeg, pbeg + n);
    }
};


extern "C" {

bool test_char;    // exercise money_get<char>
bool test_wchar;   // exercise money_get<wchar_t>


static void*
thread_func (void*)
{
    MyIos<char, std::char_traits<char> >       nio;
    MyStreambuf<char, std::char_traits<char> > nsb;
    std::basic_string<char, std::char_traits<char>  > str;
    nio.rdbuf (&nsb);

#ifndef _RWSTD_NO_WCHAR_T
    MyIos<wchar_t, std::char_traits<wchar_t> >       wio;
    MyStreambuf<wchar_t, std::char_traits<wchar_t> > wsb;
    std::basic_string<wchar_t, std::char_traits<wchar_t>  > wstr;
    wio.rdbuf (&wsb);
#endif // _RWSTD_NO_WCHAR_T

    std::ios_base::iostate state = std::ios_base::goodbit;

#ifndef _RWSTD_NO_LONG_DOUBLE
    long double ldbl = 0.;
#endif  // _RWSTD_NO_LONG_DOUBLE

    for (int i = 0; i != opt_nloops; ++i) {

        // save the name of the locale
        const MyMoneyData& data = my_money_data [i % nlocales];

        // construct a named locale, get a reference to the money_get
        // facet from it and use it to format a random money value
        const std::locale loc =
            opt_shared_locale ? data.locale_
                                 : std::locale (data.locale_name_);

        if (test_char) {
            // exercise the narrow char specialization of the facet

            const std::money_get<char> &ng =
                std::use_facet<std::money_get<char> >(loc);

            nio.imbue (loc);
            nsb.pubsetg (data.ncs_, RW_COUNT_OF (data.ncs_));

            if (i & 1) {
                ng.get (std::istreambuf_iterator<char>(&nsb),
                        std::istreambuf_iterator<char>(),
                        data.intl_, nio, state, ldbl);
                RW_ASSERT (! (state & std::ios_base::failbit));
                RW_ASSERT (! rw_ldblcmp (ldbl, data.units_));
            }
            else {
                ng.get (std::istreambuf_iterator<char>(&nsb),
                        std::istreambuf_iterator<char>(),
                        data.intl_, nio, state, str);
                RW_ASSERT (! (state & std::ios_base::failbit));
                RW_ASSERT (! rw_strncmp (str.c_str (),
                                         data.ncs_digits_.c_str ()));
            }
        }

        // both specializations may be tested at the same time

        if (test_wchar) {
            // exercise the wide char specialization of the facet

#ifndef _RWSTD_NO_WCHAR_T

            const std::money_get<wchar_t> &wg =
                std::use_facet<std::money_get<wchar_t> >(loc);

            wio.imbue (loc);
            wsb.pubsetg (data.wcs_, RW_COUNT_OF (data.wcs_));

            if (i & 1) {
                wg.get (std::istreambuf_iterator<wchar_t>(&wsb),
                        std::istreambuf_iterator<wchar_t>(),
                        data.intl_, wio, state, ldbl);
                RW_ASSERT (! (state & std::ios_base::failbit));
                RW_ASSERT (! rw_ldblcmp (ldbl, data.units_));
            }
            else {
                wg.get (std::istreambuf_iterator<wchar_t>(&wsb),
                        std::istreambuf_iterator<wchar_t>(),
                        data.intl_, wio, state, wstr);
                RW_ASSERT (! (state & std::ios_base::failbit));
                RW_ASSERT (! rw_strncmp(wstr.c_str (),
                                        data.wcs_digits_.c_str ()));
            }

#endif   // _RWSTD_NO_WCHAR_T

        }
    }

    return 0;
}

}   // extern "C"

/**************************************************************************/

static int
run_test (int, char**)
{
    MyIos<char, std::char_traits<char> >       nio;
    MyStreambuf<char, std::char_traits<char> > nsb;
    nio.rdbuf (&nsb);

#ifndef _RWSTD_NO_WCHAR_T
    MyIos<wchar_t, std::char_traits<wchar_t> >       wio;
    MyStreambuf<wchar_t, std::char_traits<wchar_t> > wsb;
    wio.rdbuf (&wsb);
#endif // _RWSTD_NO_WCHAR_T

    // find all installed locales for which setlocale (LC_ALL) succeeds
#if TEST_RW_EXTENSIONS
    // Only classic 'C' locale defined for stlport.
    const char* const locale_list =
        rw_opt_locales ? rw_opt_locales : rw_locales (_RWSTD_LC_ALL);
#else
    const char *const locale_list = "C\0";
#endif

    const std::size_t maxinx = RW_COUNT_OF (locales);

    for (const char* name = locale_list;
         *name;
         name += std::strlen (name) + 1) {

        const std::size_t inx = nlocales;
        locales [inx] = name;

        // fill in the money and results for this locale
        MyMoneyData& data = my_money_data [inx];
        data.locale_name_ = name;

        try {
            const std::locale loc (data.locale_name_);

            // exercise domestic formats every other iteration
            // and international formats the rest
            data.intl_ = 0 == (inx & 5);

            // initialize with random but valid values
            long double units = 1.f + inx;

            // exercise postive and negative values
            if (inx & 1)
                units += (units * 3.14159);
            else
                units -= (units * 2.71828);

            // local scope
            {
                const std::money_put<char> &np =
                    std::use_facet<std::money_put<char> >(loc);

                const std::money_get<char> &ng =
                    std::use_facet<std::money_get<char> >(loc);

                std::ios::iostate state = std::ios::goodbit;

                nio.imbue (loc);

                // write the value to buffer from long double
                nsb.pubsetp (data.ncs_, RW_COUNT_OF (data.ncs_));
#if DRQS
    // _monetary.h do_put for long double isn't using '__units' parameter.
    // TODO: Also need to fix up do_put to recognize decimals...
    // line 419 is checking only for digits, not decimals.
                *np.put (std::ostreambuf_iterator<char>(&nsb),
                         data.intl_, nio, ' ', units) = '\0';
#else
                std::stringstream ss;
                ss << units;
                *np.put (std::ostreambuf_iterator<char>(&nsb),
                         data.intl_, nio, ' ', ss.str()) = '\0';
#endif
                *np.put (std::ostreambuf_iterator<char>(&nsb),
                         data.intl_, nio, ' ', units) = '\0';


                rw_fatal (!nio.fail (), __FILE__, __LINE__,
                          "money_put<char>::put(...) "
                          "failed for locale(%#s)", data.locale_name_);

                // read the value back so threads know what to expect
                nsb.pubsetg (data.ncs_, RW_COUNT_OF (data.ncs_));
                ng.get (std::istreambuf_iterator<char>(&nsb),
                        std::istreambuf_iterator<char>(),
                        data.intl_, nio, state, data.units_);

                // read back as string, again for threads
                nsb.pubsetg (data.ncs_, RW_COUNT_OF (data.ncs_));
                ng.get (std::istreambuf_iterator<char>(&nsb),
                        std::istreambuf_iterator<char>(),
                        data.intl_, nio, state, data.ncs_digits_);

                if (state & std::ios_base::failbit)
                    continue;
            }

#ifndef _RWSTD_NO_WCHAR_T

            // local scope
            {
                const std::money_put<wchar_t> &wp =
                    std::use_facet<std::money_put<wchar_t> >(loc);

                const std::money_get<wchar_t> &wg =
                    std::use_facet<std::money_get<wchar_t> >(loc);

                std::ios::iostate state = std::ios::goodbit;

                wio.imbue (loc);

                // write the value to buffer from long double
                wsb.pubsetp (data.wcs_, RW_COUNT_OF (data.wcs_));

#if DRQS
    // _monetary.h do_put for long double isn't using '__units' parameter.
    // TODO: Also need to fix up do_put to recognize decimals...
    // line 419 is checking only for digits, not decimals.
                *wp.put (std::ostreambuf_iterator<wchar_t>(&wsb),
                         data.intl_, wio, L' ', units) = L'\0';
#else
                std::wstringstream wss;
                wss << units;
                *wp.put (std::ostreambuf_iterator<wchar_t>(&wsb),
                         data.intl_, wio, ' ', wss.str()) = '\0';
#endif

                rw_fatal (!nio.fail (), __FILE__, __LINE__,
                           "money_put<wchar_t>::put(...) "
                           "failed for locale(%#s)", data.locale_name_);

                // read back as string, again for threads
                wsb.pubsetg (data.wcs_, RW_COUNT_OF (data.wcs_));
                wg.get (std::istreambuf_iterator<wchar_t>(&wsb),
                        std::istreambuf_iterator<wchar_t>(),
                        data.intl_, wio, state, data.wcs_digits_);

                if (state & std::ios_base::failbit)
                    continue;
            }

#endif // _RWSTD_NO_WCHAR_T

            if (opt_shared_locale)
                data.locale_ = loc;

            nlocales += 1;

        }
        catch (...) {
            rw_warn (!rw_opt_locales, 0, __LINE__,
                     "failed to create locale(%#s)", name);
        }

        if (nlocales == maxinx || nlocales == std::size_t (opt_nlocales))
            break;
    }

    // avoid divide by zero in thread if there are no locales to test
    rw_fatal (nlocales != 0, 0, __LINE__,
              "failed to create one or more usable locales!");

    rw_info (0, 0, 0,
             "testing std::money_get<charT> with %d thread%{?}s%{;}, "
             "%d iteration%{?}s%{;} each, in %zu locales { %{ .*A@} }",
             opt_nthreads, 1 != opt_nthreads,
             opt_nloops, 1 != opt_nloops,
             nlocales, int (nlocales), "%#s", locales);

    rw_info (0, 0, 0, "exercising std::money_get<char>");

    test_char  = true;
    test_wchar = false;

    // create and start a pool of threads and wait for them to finish
    int result = 
        rw_thread_pool (0, std::size_t (opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              opt_nthreads, thread_func);

#ifndef _RWSTD_NO_WCHAR_T

    rw_info (0, 0, 0, "exercising std::money_get<wchar_t>");

    test_char  = false;
    test_wchar = true;

    // start a pool of threads to exercise wstring thread safety
    result =
        rw_thread_pool (0, std::size_t (opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              opt_nthreads, thread_func);

    // exercise both the char and the wchar_t specializations
    // at the same time

    rw_info (0, 0, 0,
             "exercising both std::money_get<char> "
             "and std::money_get<wchar_t>");

    test_char  = true;
    test_wchar = true;

    // start a pool of threads to exercise wstring thread safety
    result =
        rw_thread_pool (0, std::size_t (opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              opt_nthreads, thread_func);

#endif   // _RWSTD_NO_WCHAR_T

    return result;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
#ifdef _RWSTD_REENTRANT

    // set nthreads to the greater of the number of processors
    // and 2 (for uniprocessor systems) by default
    opt_nthreads = rw_get_cpus ();
    if (opt_nthreads < 2)
        opt_nthreads = 2;

#endif   // _RWSTD_REENTRANT

    return rw_test (argc, argv, __FILE__,
                    "lib.locale.money.get",
                    "thread safety", run_test,
                    "|-nloops#0 "        // must be non-negative
                    "|-nthreads#0-* "    // must be in [0, MAX_THREADS]
                    "|-nlocales#0 "      // arg must be non-negative
                    "|-locales= "        // must be provided
                    "|-shared-locale# ",
                    &opt_nloops,
                    int (MAX_THREADS),
                    &opt_nthreads,
                    &opt_nlocales,
                    &rw_opt_setlocales,
                    &opt_shared_locale);
}
