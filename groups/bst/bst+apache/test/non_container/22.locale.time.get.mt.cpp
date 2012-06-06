/************************************************************************
 *
 * 22.locale.time.get.mt.cpp
 *
 * test exercising the thread safety of the time_get facet
 *
 * $Id: 22.locale.time.get.mt.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2002-2007 Rogue Wave Software.
 *
 **************************************************************************/
#include <ios>        // for ios
#include <iterator>   // for ostreambuf_iterator
#include <locale>     // for locale, time_put
#include <cstring>    // for strlen ()
#include <ctime>      // for tm

#include <rw_locale.h>
#include <rw_thread.h>
#include <driver.h>    // for rw_assert ()
#include <valcmp.h>    // for rw_strncmp ()

#define MAX_THREADS      32
#define MAX_LOOPS    100000

// default number of threads (will be adjusted to the number
// of processors/cores later)
int rw_opt_nthreads = 1;

// the number of times each thread should iterate
int rw_opt_nloops = 50000;

// number of locales to use
int rw_opt_nlocales = MAX_THREADS;

// should all threads share the same set of locale objects instead
// of creating their own?
int rw_opt_shared_locale;

/**************************************************************************/

// array of locale names to use for testing
static const char*
locales [MAX_THREADS];

// number of locale names in the array
static std::size_t
nlocales;

/**************************************************************************/

//
struct MyTimeData
{
    enum { BufferSize = 64 };

    // name of the locale the data corresponds to
    const char* locale_name_;

    // optionally set to the named locale for threads to share
    std::locale locale_;

    // the time struct used to generate strings below
    std::tm time_;

    // the type specifier
    char format_;

    // narrow representations of time_ given the
    // locale_name_ and the format_
    char ncs_ [BufferSize];

#ifndef _RWSTD_NO_WCHAR_T

    // wide representations of time_
    wchar_t wcs_ [BufferSize];

#endif // _RWSTD_NO_WCHAR_T

} my_time_data [MAX_THREADS];


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

void verify_fields (const std::tm& a, const std::tm& b)
{
    RW_ASSERT (a.tm_sec  == b.tm_sec);
    RW_ASSERT (a.tm_min  == b.tm_min);
    RW_ASSERT (a.tm_hour == b.tm_hour);
    RW_ASSERT (a.tm_mday == b.tm_mday);
    RW_ASSERT (a.tm_mon  == b.tm_mon);
    RW_ASSERT (a.tm_year == b.tm_year);
    RW_ASSERT (a.tm_wday == b.tm_wday);
    RW_ASSERT (a.tm_yday == b.tm_yday);
}
#define RW_ASSERT_FIELDS(a,b) verify_fields(a,b)

extern "C" {

bool test_char;    // exercise time_get<char>
bool test_wchar;   // exercise time_get<wchar_t>


static void*
thread_func (void*)
{
    MyIos<char, std::char_traits<char> >       nio;
    MyStreambuf<char, std::char_traits<char> > nsb;
    nio.rdbuf (&nsb);

#ifndef _RWSTD_NO_WCHAR_T
    MyIos<wchar_t, std::char_traits<wchar_t> >       wio;
    MyStreambuf<wchar_t, std::char_traits<wchar_t> > wsb;
    wio.rdbuf (&wsb);
#endif // _RWSTD_NO_WCHAR_T

    std::ios_base::iostate state = std::ios_base::goodbit;

    std::tm local;
    for (int i = 0; i != rw_opt_nloops; ++i) {

        // save the name of the locale
        const MyTimeData& data = my_time_data [i % nlocales];

        // construct a named locale, get a reference to the time_put
        // facet from it and use it to format a random time value
        // using a random conversion specifier
        const std::locale loc =
            rw_opt_shared_locale ? data.locale_
                                 : std::locale (data.locale_name_);

        if (test_char) {
            // exercise the narrow char specialization of the facet

            const std::time_get<char> &ng =
                std::use_facet<std::time_get<char> >(loc);

            nio.imbue (loc);
            nsb.pubsetg (data.ncs_, RW_COUNT_OF (data.ncs_));

            const std::istreambuf_iterator<char> iter (&nsb);
            const std::istreambuf_iterator<char> end;

            std::memset (&local, 0, sizeof local);

            switch (data.format_) {
            case 'X':
                ng.get_time (iter, end, nio, state, &local);
                break;
            case 'x':
                ng.get_date (iter, end, nio, state, &local);
                break;
            case 'a':
            case 'A':
                ng.get_weekday (iter, end, nio, state, &local);
                break;
            case 'b':
            case 'B':
                ng.get_monthname (iter, end, nio, state, &local);
                break;
            case 'y':
            case 'Y':
                ng.get_year (iter, end, nio, state, &local);
                break;
            default:

#ifndef _RWSTD_NO_EXT_TIME_GET
                ng.get (iter, end, nio, state, &local, data.format_);
#endif  // _RWSTD_NO_EXT_TIME_GET

                break;
            }

            RW_ASSERT (! (state & std::ios_base::failbit));
            RW_ASSERT_FIELDS (local, data.time_);
        }

        // both specializations may be tested at the same time

        if (test_wchar) {
            // exercise the wide char specialization of the facet

#ifndef _RWSTD_NO_WCHAR_T

            const std::time_get<wchar_t> &wg =
                std::use_facet<std::time_get<wchar_t> >(loc);

            wio.imbue (loc);
            wsb.pubsetg (data.wcs_, RW_COUNT_OF (data.wcs_));

            const std::istreambuf_iterator<wchar_t> iter (&wsb);
            const std::istreambuf_iterator<wchar_t> end;

            std::memset (&local, 0, sizeof local);

            switch (data.format_) {
            case 'X':
                wg.get_time (iter, end, wio, state, &local);
                break;
            case 'x':
                wg.get_date (iter, end, wio, state, &local);
                break;
            case 'a':
            case 'A':
                wg.get_weekday (iter, end, wio, state, &local);
                break;
            case 'b':
            case 'B':
                wg.get_monthname (iter, end, wio, state, &local);
                break;
            case 'y':
            case 'Y':
                wg.get_year (iter, end, wio, state, &local);
                break;
            default:

#ifndef _RWSTD_NO_EXT_TIME_GET
                wg.get (iter, end, wio, state, &local, data.format_);
#endif  // _RWSTD_NO_EXT_TIME_GET

                break;
            }

            RW_ASSERT (! (state & std::ios_base::failbit));
            RW_ASSERT_FIELDS (local, data.time_);

#endif // _RWSTD_NO_WCHAR_T

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

    int j = 0;
    for (const char* name = locale_list;
         *name;
         name += std::strlen (name) + 1) {

        const std::size_t inx = nlocales;
        locales [inx] = name;

        // fill in the time and results for this locale
        MyTimeData& data = my_time_data [inx];
        data.locale_name_ = name;

        // initialize tm with random but valid values
        std::memset (&data.time_, 0, sizeof data.time_);
        data.time_.tm_sec  = ++j % 61;
        data.time_.tm_min  = ++j % 60;
        data.time_.tm_hour = ++j % 12;
        data.time_.tm_wday = ++j % 7;
        data.time_.tm_mon  = ++j % 12;
        data.time_.tm_mday = ++j % 31;
        data.time_.tm_yday = ++j % 366;
        data.time_.tm_year = ++j;

#ifndef _RWSTD_NO_EXT_TIME_GET
        // format specifiers that we can reliably parse
        // get_weekday, get_monthname, get_date, get_time,
        // get_year and the extension get.
        const char formats [] = "aAbBCdhHjmMnRStTwxXyY";
#else
        // formats that generate strings compatible with
        // get_weekday, get_monthname, get_date, get_time
        // and get_year.
        const char formats [] = "aAbBxXyY";
#endif

        // get the "random" data type used to generate representation
        data.format_ = formats [nlocales % (sizeof formats - 1)];

        try {
            const std::locale loc (data.locale_name_);

            const std::time_put<char> &np =
                std::use_facet<std::time_put<char> >(loc);

            const std::time_get<char> &ng =
                std::use_facet<std::time_get<char> >(loc);

            std::ios::iostate state = std::ios::goodbit;

            nio.imbue (loc);

            // put the time representation on the stream
            nsb.pubsetp (data.ncs_, RW_COUNT_OF (data.ncs_));
            *np.put (std::ostreambuf_iterator<char>(&nsb), nio, ' ',
                     &data.time_, data.format_) = '\0';

            rw_fatal (!nio.fail (), __FILE__, __LINE__,
                      "time_put<char>::put(...) failed for locale(%#s)",
                      data.locale_name_);

            // read the data back out. this is what the thread
            // is expected to do, and the result it should get.

            nsb.pubsetg (data.ncs_, RW_COUNT_OF (data.ncs_));

            const std::istreambuf_iterator<char> iter (&nsb);
            const std::istreambuf_iterator<char> end;

            std::memset (&data.time_, 0, sizeof data.time_);

            switch (data.format_) {
            case 'a':
            case 'A':
                ng.get_weekday (iter, end, nio, state, &data.time_);
                break;
            case 'b':
            case 'B':
            case 'h':
                ng.get_monthname (iter, end, nio, state, &data.time_);
                break;
            case 'y':
            case 'Y':
                ng.get_year (iter, end, nio, state, &data.time_);
                break;
            case 'x':
                ng.get_date (iter, end, nio, state, &data.time_);
                break;
            case 'X':
                ng.get_time (iter, end, nio, state, &data.time_);
                break;
            default:

#ifndef _RWSTD_NO_EXT_TIME_GET
                ng.get (iter, end, nio, state, &data.time_, data.format_);
#endif  // _RWSTD_NO_EXT_TIME_GET

                break;
            }

            // we couldn't read the data back, so don't use this locale
            if (state & std::ios_base::failbit)
                continue;

#ifndef _RWSTD_NO_WCHAR_T

            const std::time_put<wchar_t> &wp =
                std::use_facet<std::time_put<wchar_t> >(loc);

            wio.imbue (loc);
            wsb.pubsetp (data.wcs_, RW_COUNT_OF (data.wcs_));

            *wp.put (std::ostreambuf_iterator<wchar_t>(&wsb),
                     wio, L' ', &data.time_, data.format_) = L'\0';

            rw_fatal (!wio.fail (), __FILE__, __LINE__,
                      "time_put<wchar_t>::put(...) failed for locale(%#s)",
                      data.locale_name_);

#endif // _RWSTD_NO_WCHAR_T

            if (rw_opt_shared_locale)
                data.locale_ = loc;

            nlocales += 1;

        }
        catch (...) {
            rw_warn (!rw_opt_locales, 0, __LINE__,
                     "failed to create locale(%#s)", name);
        }

        if (nlocales == maxinx || nlocales == std::size_t (rw_opt_nlocales))
            break;
    }

    // avoid divide by zero in thread if there are no locales to test
    rw_fatal (nlocales != 0, 0, __LINE__,
              "failed to create one or more usable locales!");

    rw_info (0, 0, 0,
             "testing std::time_get<charT> with %d thread%{?}s%{;}, "
             "%d iteration%{?}s%{;} each, in %zu locales { %{ .*A@} }",
             rw_opt_nthreads, 1 != rw_opt_nthreads,
             rw_opt_nloops, 1 != rw_opt_nloops,
             nlocales, int (nlocales), "%#s", locales);

    rw_info (0, 0, 0, "exercising std::time_get<char>");

    test_char  = true;
    test_wchar = false;

    // create and start a pool of threads and wait for them to finish
    int result =
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

#ifndef _RWSTD_NO_WCHAR_T

    rw_info (0, 0, 0, "exercising std::time_get<wchar_t>");

    test_char  = false;
    test_wchar = true;

    // start a pool of threads to exercise wstring thread safety
    result =
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

    // exercise both the char and the wchar_t specializations
    // at the same time

    rw_info (0, 0, 0,
             "exercising both std::time_get<char> and std::time_get<wchar_t>");

    test_char  = true;
    test_wchar = true;

    // start a pool of threads to exercise wstring thread safety
    result =
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

#endif   // _RWSTD_NO_WCHAR_T

    return result;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
#ifdef _RWSTD_REENTRANT

    // set nthreads to the greater of the number of processors
    // and 2 (for uniprocessor systems) by default
    rw_opt_nthreads = rw_get_cpus ();
    if (rw_opt_nthreads < 2)
        rw_opt_nthreads = 2;

#endif   // _RWSTD_REENTRANT

    return rw_test (argc, argv, __FILE__,
                    "lib.locale.time.get",
                    "thread safety", run_test,
                    "|-nloops#0 "       // must be non-negative
                    "|-nthreads#0-* "   // must be in [0, MAX_THREADS]
                    "|-nlocales#0 "     // arg must be non-negative
                    "|-locales= "       // must be provided
                    "|-shared-locale# ",
                    &rw_opt_nloops,
                    int (MAX_THREADS),
                    &rw_opt_nthreads,
                    &rw_opt_nlocales,
                    &rw_opt_setlocales,
                    &rw_opt_shared_locale);
}
