/************************************************************************
 *
 * 22.locale.time.put.mt.cpp
 *
 * test exercising the thread safety of the time_put facet
 *
 * $Id: 22.locale.time.put.mt.cpp 648752 2008-04-16 17:01:56Z faridz $
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
int opt_nthreads = 1;

// the number of times each thread should iterate
int opt_nloops = MAX_LOOPS;

#if !defined (_RWSTD_OS_HP_UX) || defined (_ILP32)

// number of locales to use
int opt_nlocales = MAX_THREADS;

#else   // HP-UX in LP64 mode

// work around an inefficiency (small cache size?) on HP-UX
// in LP64 mode (see STDCXX-812)
int opt_nlocales = 10;

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
struct MyTimeData
{
    enum { BufferSize = 64 };

    // name of the locale the data corresponds to
    const char* locale_name_;

    // optionally set to the named locale for threads to share
    std::locale locale_;

    // the time struct used to generate strings below
    std::tm time_;

    // the format specifier
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

    void pubsetp (charT *pbeg, std::streamsize n) {
        this->setp (pbeg, pbeg + n);
    }
};


extern "C" {

bool test_char;    // exercise time_put<char>
bool test_wchar;   // exercise time_put<wchar_t>


static void*
thread_func (void*)
{
    char              ncs [MyTimeData::BufferSize];
    MyIos<char, std::char_traits<char> >       nio;
    MyStreambuf<char, std::char_traits<char> > nsb;
    nio.rdbuf (&nsb);

#ifndef _RWSTD_NO_WCHAR_T
    wchar_t                 wcs [MyTimeData::BufferSize];
    MyIos<wchar_t, std::char_traits<wchar_t> >       wio;
    MyStreambuf<wchar_t, std::char_traits<wchar_t> > wsb;
    wio.rdbuf (&wsb);
#endif // _RWSTD_NO_WCHAR_T

    for (int i = 0; i != opt_nloops; ++i) {

        // save the name of the locale
        const MyTimeData& data = my_time_data [i % nlocales];

        // construct a named locale, get a reference to the time_put
        // facet from it and use it to format a random time value
        // using a random conversion specifier
        const std::locale loc =
            opt_shared_locale ? data.locale_
                                 : std::locale (data.locale_name_);

        if (test_char) {
            // exercise the narrow char specialization of the facet

            const std::time_put<char> &tp =
                std::use_facet<std::time_put<char> >(loc);

            nio.imbue (loc);
            nsb.pubsetp (ncs, RW_COUNT_OF (ncs));

            // format time using provided format specifier
            *tp.put (std::ostreambuf_iterator<char>(&nsb),
                     nio, ' ', &data.time_, data.format_) = '\0';

            RW_ASSERT (!nio.fail ());
            RW_ASSERT (!rw_strncmp(ncs, data.ncs_));

        }

        // both specializations may be tested at the same time

        if (test_wchar) {
            // exercise the wide char specialization of the facet

#ifndef _RWSTD_NO_WCHAR_T

            const std::time_put<wchar_t> &wp =
                std::use_facet<std::time_put<wchar_t> >(loc);

            wio.imbue (loc);
            wsb.pubsetp (wcs, RW_COUNT_OF (wcs));

            *wp.put (std::ostreambuf_iterator<wchar_t>(&wsb),
                     wio, L' ', &data.time_, data.format_) = L'\0';

            RW_ASSERT (!wio.fail ());
            RW_ASSERT (!rw_strncmp(wcs, data.wcs_));

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
        data.time_.tm_sec  = ++j % 61;
        data.time_.tm_min  = ++j % 60;
        data.time_.tm_hour = ++j % 12;
        data.time_.tm_wday = ++j % 7;
        data.time_.tm_mon  = ++j % 12;
        data.time_.tm_mday = ++j % 31;
        data.time_.tm_yday = ++j % 366;
        data.time_.tm_year = ++j;

        const char cvtspecs[] = "aAbBcCdDeFgGhHIjmMnprRStTuUVwWxXyYzZ%";

        // get the "random" conversion specifier used to generate
        // the result string
        data.format_ = cvtspecs [nlocales % (sizeof cvtspecs - 1)];

        try {
            const std::locale loc (data.locale_name_);

            const std::time_put<char> &np =
                std::use_facet<std::time_put<char> >(loc);

            nio.imbue (loc);
            nsb.pubsetp (data.ncs_, RW_COUNT_OF (data.ncs_));
            
            *np.put (std::ostreambuf_iterator<char>(&nsb),
                     nio, ' ', &data.time_, data.format_) = '\0';

            rw_fatal (!nio.fail (), __FILE__, __LINE__,
                      "time_put<char>::put(..., %c) "
                      "failed for locale(%#s)",
                      data.format_, data.locale_name_);
            
#ifndef _RWSTD_NO_WCHAR_T

            const std::time_put<wchar_t> &wp =
                std::use_facet<std::time_put<wchar_t> >(loc);

            wio.imbue (loc);
            wsb.pubsetp (data.wcs_, RW_COUNT_OF (data.wcs_));
            
            *wp.put (std::ostreambuf_iterator<wchar_t>(&wsb),
                     wio, L' ', &data.time_, data.format_) = L'\0';
            
            rw_fatal (!wio.fail (), __FILE__, __LINE__,
                      "time_put<wchar_t>::put(..., %c) "
                      "failed for locale(%#s)",
                      data.format_, data.locale_name_);

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
             "testing std::time_put<charT> with %d thread%{?}s%{;}, "
             "%d iteration%{?}s%{;} each, in %zu locales { %{ .*A@} }",
             opt_nthreads, 1 != opt_nthreads,
             opt_nloops, 1 != opt_nloops,
             nlocales, int (nlocales), "%#s", locales);

    rw_info (0, 0, 0, "exercising std::time_put<char>");

    test_char  = true;
    test_wchar = false;

    // create and start a pool of threads and wait for them to finish
    int result =
        rw_thread_pool (0, std::size_t (opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              opt_nthreads, thread_func);

#ifndef _RWSTD_NO_WCHAR_T

    rw_info (0, 0, 0, "exercising std::time_put<wchar_t>");

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
             "exercising both std::time_put<char> and std::time_put<wchar_t>");

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
                    "lib.locale.time.put",
                    "thread safety", run_test,
                    "|-nloops#0 "       // must be non-negative
                    "|-nthreads#0-* "   // must be in [0, MAX_THREADS]
                    "|-nlocales#0 "     // arg must be non-negative
                    "|-locales= "       // must be provided
                    "|-shared-locale# ",
                    &opt_nloops,
                    int (MAX_THREADS),
                    &opt_nthreads,
                    &opt_nlocales,
                    &rw_opt_setlocales,
                    &opt_shared_locale);
}
