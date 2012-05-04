/************************************************************************
 *
 * 22.locale.num.put.mt.cpp
 *
 * test exercising the thread safety of the num_put facet
 *
 * $Id: 22.locale.num.put.mt.cpp 648752 2008-04-16 17:01:56Z faridz $
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
#include <locale>     // for locale, num_put

#include <cstring>    // for strlen()

#include <rw_locale.h>
#include <rw_thread.h>   // for rw_get_processors (), rw_thread_pool()
#include <driver.h>
#include <valcmp.h>

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


struct MyNumData {

    enum { BufferSize = 32 };

    enum PutId {
        put_bool,
        put_long,
        put_ulong,

#ifndef _RWSTD_NO_LONG_LONG

        put_llong,
        put_ullong,

#endif   // _RWSTD_NO_LONG_LONG

        put_dbl,

#ifndef _RWSTD_NO_LONG_DOUBLE

        put_ldbl,

#endif   // _RWSTD_NO_LONG_DOUBLE

        put_ptr,
        put_max
    };

    // name of the locale the data corresponds to
    const char* locale_name_;

    // optionally set to the named locale for threads to share
    std::locale locale_;

    // the value that we will be formatting
    double value_;

    // the type of the data we put
    PutId type_;

    // holds the narrow/wide character representation of value_ and
    // the number of used 'charT' in each buffer.
    char    ncs_ [BufferSize];

#ifndef _RWSTD_NO_WCHAR_T

    wchar_t wcs_ [BufferSize];

#endif  // _RWSTD_NO_WCHAR_T

} my_num_data [MAX_THREADS];

/**************************************************************************/

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

template <class charT, class Traits>
void
put_data (const MyNumData                               &data,
          const std::num_put<charT>                     &np,
          const std::ostreambuf_iterator<charT, Traits> &iter,
          std::basic_ios<charT, Traits>                 &io,
          charT                                          fill,
          charT                                          term)
{
    switch (data.type_) {
    case MyNumData::put_bool:
        *np.put (iter, io, fill,
                 data.value_ < 1.f) = term;
        break;
    case MyNumData::put_long:
        *np.put (iter, io, fill,
                 (long)data.value_) = term;
        break;
    case MyNumData::put_ulong:
        *np.put (iter, io, fill,
                 (unsigned long)data.value_) = term;
        break;

#ifndef _RWSTD_NO_LONG_LONG

    case MyNumData::put_llong:
        *np.put (iter, io, fill,
                 (_RWSTD_LONG_LONG)data.value_) = term;
        break;
    case MyNumData::put_ullong:
        *np.put (iter, io, fill,
                 (unsigned _RWSTD_LONG_LONG)data.value_) = term;
        break;

#endif   // _RWSTD_NO_LONG_LONG

    case MyNumData::put_dbl:
        *np.put (iter, io, fill,
                 (double)data.value_) = term;

        break;

#ifndef _RWSTD_NO_LONG_DOUBLE

    case MyNumData::put_ldbl:
        *np.put (iter, io, fill,
                 (long double)data.value_) = term;
        break;

#endif   // _RWSTD_NO_LONG_DOUBLE

    case MyNumData::put_ptr:
        *np.put (iter, io, fill,
                 (const void*)&data.value_) = term;
        break;

    case MyNumData::put_max:
        // avoid enumeration value `put_max' not handled in switch
        // this case should never happen
        break;
    }

}

extern "C" {

bool test_char;    // exercise num_put<char>
bool test_wchar;   // exercise num_put<wchar_t>


static void*
thread_func (void*)
{
    char               ncs [MyNumData::BufferSize];
    MyIos<char, std::char_traits<char> >       nio;
    MyStreambuf<char, std::char_traits<char> > nsb;
    nio.rdbuf (&nsb);

#ifndef _RWSTD_NO_WCHAR_T
    wchar_t                  wcs [MyNumData::BufferSize];
    MyIos<wchar_t, std::char_traits<wchar_t> >       wio;
    MyStreambuf<wchar_t, std::char_traits<wchar_t> > wsb;
    wio.rdbuf (&wsb);
#endif // _RWSTD_NO_WCHAR_T

    for (int i = 0; i != opt_nloops; ++i) {

        // fill in the value and results for this locale
        const MyNumData& data = my_num_data [i % nlocales];

        // construct a named locale and imbue it in the ios object
        // so that the locale is used not only by the num_put facet
        const std::locale loc =
            opt_shared_locale ? data.locale_
                                 : std::locale (data.locale_name_);

        if (test_char) {
            // exercise the narrow char specialization of the facet

            const std::num_put<char> &np =
                std::use_facet<std::num_put<char> >(loc);

            nio.imbue (loc);
            nsb.pubsetp (ncs, RW_COUNT_OF (ncs));

            put_data (data, np, std::ostreambuf_iterator<char>(&nsb),
                      nio, ' ', '\0');

            RW_ASSERT (!nio.fail ());
            RW_ASSERT (!rw_strncmp (ncs, data.ncs_));
        }

        // both specializations may be tested at the same time

        if (test_wchar) {
            // exercise the wide char specialization of the facet

#ifndef _RWSTD_NO_WCHAR_T

            const std::num_put<wchar_t> &wp =
                std::use_facet<std::num_put<wchar_t> >(loc);

            wio.imbue (loc);
            wsb.pubsetp (wcs, RW_COUNT_OF (wcs));

            put_data (data, wp, std::ostreambuf_iterator<wchar_t>(&wsb),
                      wio, L' ', L'\0');

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

    // find all installed locales for which setlocale(LC_ALL) succeeds
    const char* const locale_list =
        rw_opt_locales ? rw_opt_locales : rw_locales (_RWSTD_LC_ALL);

    const std::size_t maxinx = RW_COUNT_OF (locales);

    for (const char *name = locale_list;
         *name;
         name += std::strlen (name) + 1) {

        const std::size_t inx = nlocales;
        locales [inx] = name;

        // fill in the value and results for this locale
        MyNumData& data = my_num_data [nlocales];
        data.locale_name_ = name;

        try {
            const std::locale loc (data.locale_name_);

            data.value_ = nlocales & 1 ? -1 * nlocales : nlocales;
            data.type_ = MyNumData::PutId (nlocales % MyNumData::put_max);

            // format data into buffers
            const std::num_put<char> &np =
                std::use_facet<std::num_put<char> >(loc);

            nio.imbue (loc);
            nsb.pubsetp (data.ncs_, RW_COUNT_OF (data.ncs_));

            put_data (data, np, std::ostreambuf_iterator<char>(&nsb),
                      nio, ' ', '\0');

            rw_fatal (!nio.fail (), __FILE__, __LINE__,
                      "num_put<char>::put(...) failed for locale(%#s)",
                      data.locale_name_);

#ifndef _RWSTD_NO_WCHAR_T

            const std::num_put<wchar_t> &wp =
                std::use_facet<std::num_put<wchar_t> >(loc);

            wio.imbue (loc);
            wsb.pubsetp (data.wcs_, RW_COUNT_OF (data.wcs_));

            put_data (data, wp, std::ostreambuf_iterator<wchar_t>(&wsb),
                      wio, L' ', L'\0');

            rw_fatal (!wio.fail (), __FILE__, __LINE__,
                      "num_put<wchar_t>::put(...) failed for locale(%#s)",
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
             "testing std::num_put<charT> with %d thread%{?}s%{;}, "
             "%d iteration%{?}s%{;} each, in %zu locales { %{ .*A@} }",
             opt_nthreads, 1 != opt_nthreads,
             opt_nloops, 1 != opt_nloops,
             nlocales, int (nlocales), "%#s", locales);

    rw_info (0, 0, 0, "exercising std::num_put<char>");

    test_char  = true;
    test_wchar = false;

    // create and start a pool of threads and wait for them to finish
    int result =
        rw_thread_pool (0, std::size_t (opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              opt_nthreads, thread_func);

#ifndef _RWSTD_NO_WCHAR_T

    rw_info (0, 0, 0, "exercising std::num_put<wchar_t>");

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
             "exercising both std::num_put<char> and std::num_put<wchar_t>");

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
                    "lib.locale.num.put",
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
