/************************************************************************
 *
 * 22.locale.money.put.mt.cpp
 *
 * test exercising the thread safety of the money_put facet
 *
 * $Id: 22.locale.money.put.mt.cpp 648752 2008-04-16 17:01:56Z faridz $
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

#include <ios>        // for ios
#include <iterator>   // for ostreambuf_iterator
#include <locale>     // for locale, money_put

#include <cstring>    // for strlen()

#include <rw_locale.h>
#include <rw_thread.h>
#include <driver.h>
#include <valcmp.h>    // for rw_strncmp ()


// maximum number of threads allowed by the command line interface
#define MAX_THREADS      32
#define MAX_LOOPS    100000

// default number of threads (will be adjusted to the number
// of processors/cores later)
int opt_nthreads = 1;

// the number of times each thread should iterate (unless specified
// otherwise on the command line)
int opt_nloops = 100000;

#if !defined (_RWSTD_OS_HP_UX) || defined (_ILP32)

// number of locales to use
int opt_nlocales = MAX_THREADS;

#else   // HP-UX in LP64 mode

// work around a small locale cache on HP-UX in LP64 mode
// (see STDCXX-812)
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

static const char n_money_vals[][20] = {
    "1", "12", "123", "1234", "12345", "123456", "1234567", "12345678",
    "-9", "-98", "-987", "-9876", "-98765", "-987654", "-9876543",
    "1.9", "-12.89", "123.789", "-1234.6789", "-12345.56789"
};

#ifndef _RWSTD_NO_WCHAR_T

static const wchar_t w_money_vals[][20] = {
    L"1", L"12", L"123", L"1234", L"12345", L"123456", L"1234567",
    L"-9", L"-98", L"-987", L"-9876", L"-98765", L"-987654", L"-9876543",
    L"1.9", L"-12.89", L"123.789", L"-1234.6789", L"-12345.56789"
};

#endif  // _RWSTD_NO_WCHAR_T

//
struct MyMoneyData
{
    enum { BufferSize = 16 };

    enum PutId {
        put_ldbl,
        put_string,
        put_max
    };

    // name of the locale the data corresponds to
    const char* locale_name_;

    // optionally set to the named locale for threads to share
    std::locale locale_;

    // international or domestic format flag
    bool intl_;

    // the time struct used to generate strings below
    double money_value_;

    // type of the data we created string from
    PutId type_;

    // index into string array [n,w]_money_vals
    unsigned money_index_;

    // narrow representations of money_
    char ncs_ [BufferSize];

#ifndef _RWSTD_NO_WCHAR_T

    // wide representations of money_
    wchar_t wcs_ [BufferSize];

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

    void pubsetp (charT *pbeg, std::streamsize n) {
        this->setp (pbeg, pbeg + n);
    }
};


extern "C" {

bool test_char;    // exercise money_put<char>
bool test_wchar;   // exercise money_put<wchar_t>


static void*
thread_func (void*)
{
    char             ncs [MyMoneyData::BufferSize];
    MyIos<char, std::char_traits<char> >       nio;
    MyStreambuf<char, std::char_traits<char> > nsb;
    nio.rdbuf (&nsb);

#ifndef _RWSTD_NO_WCHAR_T
    wchar_t                wcs [MyMoneyData::BufferSize];
    MyIos<wchar_t, std::char_traits<wchar_t> >       wio;
    MyStreambuf<wchar_t, std::char_traits<wchar_t> > wsb;
    wio.rdbuf (&wsb);
#endif // _RWSTD_NO_WCHAR_T

    for (int i = 0; i != opt_nloops; ++i) {

        // save the name of the locale
        const MyMoneyData& data = my_money_data [i % nlocales];

        // construct a named locale, get a reference to the money_put
        // facet from it and use it to format a random money value
        const std::locale loc =
            opt_shared_locale ? data.locale_
                                 : std::locale (data.locale_name_);

        if (test_char) {
            // exercise the narrow char specialization of the facet

            const std::money_put<char> &np =
                std::use_facet<std::money_put<char> >(loc);

            nio.imbue (loc);
            nsb.pubsetp (ncs, RW_COUNT_OF (ncs));

            switch (data.type_) {
            case MyMoneyData::put_ldbl:
                *np.put (std::ostreambuf_iterator<char>(&nsb),
                         data.intl_, nio, ' ', data.money_value_) = '\0';
                break;
            case MyMoneyData::put_string:
                *np.put (std::ostreambuf_iterator<char>(&nsb),
                         data.intl_, nio, ' ',
                         n_money_vals [data.money_index_]) = '\0';
                break;
            case MyMoneyData::put_max:
                // avoid enumeration value `put_max' not handled in switch
                // this case should never happen
                break;
            }

            RW_ASSERT (!nio.fail ());
            RW_ASSERT (!rw_strncmp (ncs, data.ncs_));

        }

        // both specializations may be tested at the same time

        if (test_wchar) {
            // exercise the wide char specialization of the facet

#ifndef _RWSTD_NO_WCHAR_T

            const std::money_put<wchar_t> &wp =
                std::use_facet<std::money_put<wchar_t> >(loc);

            wio.imbue (loc);
            wsb.pubsetp (wcs, RW_COUNT_OF (wcs));

            switch (data.type_) {
            case MyMoneyData::put_ldbl:
                *wp.put (std::ostreambuf_iterator<wchar_t>(&wsb),
                         data.intl_, wio, ' ', data.money_value_) = L'\0';
                break;
            case MyMoneyData::put_string:
                *wp.put (std::ostreambuf_iterator<wchar_t>(&wsb),
                         data.intl_, wio, ' ',
                         w_money_vals [data.money_index_]) = L'\0';
                break;
            case MyMoneyData::put_max:
                // avoid enumeration value `put_max' not handled in switch
                // this case should never happen
                break;
            }

            RW_ASSERT (!wio.fail ());
            RW_ASSERT (!rw_strncmp (wcs, data.wcs_));

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
    const char* const locale_list =
        rw_opt_locales ? rw_opt_locales : rw_locales (_RWSTD_LC_ALL);

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

            // initialize with random but valid values

            data.money_value_ = inx;
            data.type_ = MyMoneyData::PutId (nlocales % MyMoneyData::put_max);
            data.money_index_ = inx % RW_COUNT_OF (n_money_vals);

            // exercise domestic formats every other iteration
            // and international formats the rest
            data.intl_ = 0 == (inx & 1);

            // exercise postive and negative values
            if (inx & 1)
                data.money_value_ *= -1.;

            // add some random fractional digits
            if (inx & 2)
                data.money_value_ += data.money_value_ / 3.14;

            const std::money_put<char> &np =
                std::use_facet<std::money_put<char> >(loc);

            nio.imbue (loc);
            nsb.pubsetp (data.ncs_, RW_COUNT_OF (data.ncs_));
            
            switch (data.type_) {
            case MyMoneyData::put_ldbl:
                *np.put (std::ostreambuf_iterator<char>(&nsb),
                         data.intl_, nio, ' ', data.money_value_) = '\0';
                break;
            case MyMoneyData::put_string:
                *np.put (std::ostreambuf_iterator<char>(&nsb),
                         data.intl_, nio, ' ',
                         n_money_vals [data.money_index_]) = '\0';
                break;
            case MyMoneyData::put_max:
                // avoid enumeration value `put_max' not handled in switch
                // this case should never happen
                break;
            }

            rw_assert (!nio.fail (), __FILE__, __LINE__,
                       "money_put<char>::put(...) "
                       "failed for locale(%#s)",
                       data.locale_name_);

#ifndef _RWSTD_NO_WCHAR_T

            const std::money_put<wchar_t> &wp =
                std::use_facet<std::money_put<wchar_t> >(loc);

            wio.imbue (loc);
            wsb.pubsetp (data.wcs_, RW_COUNT_OF (data.wcs_));

            switch (data.type_) {
            case MyMoneyData::put_ldbl:
                *wp.put (std::ostreambuf_iterator<wchar_t>(&wsb),
                         data.intl_, wio, L' ', data.money_value_) = '\0';
                break;
            case MyMoneyData::put_string:
                *wp.put (std::ostreambuf_iterator<wchar_t>(&wsb),
                         data.intl_, wio, L' ',
                         w_money_vals [data.money_index_]) = L'\0';
                break;
            case MyMoneyData::put_max:
                // avoid enumeration value `put_max' not handled in switch
                // this case should never happen
                break;
            }

            rw_assert (!nio.fail (), __FILE__, __LINE__,
                       "money_put<wchar_t>::put(...) "
                       "failed for locale(%#s)",
                       data.locale_name_);

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
             "testing std::money_put<charT> with %d thread%{?}s%{;}, "
             "%d iteration%{?}s%{;} each, in %zu locales { %{ .*A@} }",
             opt_nthreads, 1 != opt_nthreads,
             opt_nloops, 1 != opt_nloops,
             nlocales, int (nlocales), "%#s", locales);

    rw_info (0, 0, 0, "exercising std::money_put<char>");

    test_char  = true;
    test_wchar = false;

    // create and start a pool of threads and wait for them to finish
    int result = 
        rw_thread_pool (0, std::size_t (opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              opt_nthreads, thread_func);

#ifndef _RWSTD_NO_WCHAR_T

    rw_info (0, 0, 0, "exercising std::money_put<wchar_t>");

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
             "exercising both std::money_put<char> "
             "and std::money_put<wchar_t>");

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
                    "lib.locale.money.put",
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
