/************************************************************************
 *
 * 22.locale.ctype.mt.cpp
 *
 * test exercising the thread safety of the ctype facet
 *
 * $Id: 22.locale.ctype.mt.cpp 648752 2008-04-16 17:01:56Z faridz $
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

#include <locale>     // for locale, ctype

#include <clocale>    // for lconv, localeconv()
#include <cstdlib>    // for mbstowcs()
#include <cstring>    // for size_t, strcpy()

#include <rw_locale.h>
#include <rw_thread.h>
#include <driver.h>
#include <valcmp.h>


// maximum number of threads allowed by the command line interface
#define MAX_THREADS   16

// default number of threads (will be adjusted to the number
// of processors/cores later)
int opt_nthreads = 1;

// the number of times each thread should iterate (unless specified
// otherwise on the command line)
int opt_nloops = 200000;

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

struct CtypeData
{
    // the name of the locale the data corresponds to
    const char *locale_name_;

    // optionally set to the named locale for the threads to share
    std::locale locale_;

    // results of ctype<charT>::widen(char) for char and wchar_t
    char    widened_ [256];
    wchar_t wwidened_ [256];

    // array of wide characters to narrow and the number
    // of valid elements in the array
    wchar_t wide_ [1024];
    std::size_t nwide_;

    // results of ctype<charT>::narrow(charT, char) for char
    // and wchar_t values of wide_ [i]
    char narrowed_ [256];
    char wnarrowed_ [1024];

} ctype_data [MAX_THREADS];


extern "C" {

bool test_char;    // exercise num_put<char>
bool test_wchar;   // exercise num_put<wchar_t>


static void*
thread_func (void*)
{
    // number of narrow characters to test (should be 256)
    const std::size_t nchars =
        sizeof ctype_data->widened_ / sizeof *ctype_data->widened_;

    for (int i = 0; i != opt_nloops; ++i) {

        const std::size_t inx = std::size_t (i) % nlocales;

        // save the name of the locale
        const char* const locale_name = locales [inx];

        const CtypeData* const data = ctype_data + inx;

        // construct a named locale
        const std::locale loc =
            opt_shared_locale ? data->locale_ : std::locale (locale_name);

        // "random" index/character value
        const std::size_t cinx = std::size_t (i) % nchars;

        if (test_char) {
            // exercise the narrow char specialization of the facet

            const std::ctype<char> &ct =
                std::use_facet<std::ctype<char> >(loc);

            const char wc = ct.widen (char (cinx));

            RW_ASSERT (wc == data->widened_ [cinx]);

            const char nc = ct.narrow (char (cinx), '\0');

            RW_ASSERT (nc == data->narrowed_ [cinx]);
        }

        // both specializations may be tested at the same time

        if (test_wchar) {
            // exercise the wide char specialization of the facet

#ifndef _RWSTD_NO_WCHAR_T

            const std::ctype<wchar_t> &wct =
                std::use_facet<std::ctype<wchar_t> >(loc);

            // "random" index into the wide character array
            const std::size_t winx =
                std::size_t (i) % data->nwide_;

            const wchar_t wc = wct.widen (char (cinx));

            RW_ASSERT (wc == data->wwidened_ [cinx]);

            const char nc = wct.narrow (data->wide_ [winx], L'\0');

            RW_ASSERT (nc == data->wnarrowed_ [winx]);

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
    // find all installed locales for which setlocale(LC_ALL) succeeds
    const char* const locale_list =
        rw_opt_locales ? rw_opt_locales : rw_locales (_RWSTD_LC_ALL);

    const std::size_t maxinx = sizeof locales / sizeof *locales;

    // iterate over locales, initializing a global ctype_data array
    // with LC_CTYPE data obtained from each locale
    for (const char *name = locale_list; *name;
         name += std::strlen (name) + 1) {

        const std::size_t inx = nlocales;

        locales [inx] = name;

        CtypeData* const pdata = ctype_data + nlocales;

        // compute the number of wide characters to populate
        const std::size_t nwide =
            sizeof pdata->wide_ / sizeof pdata->wide_ [0];

        try {
            const std::locale loc (name);

            const std::ctype<char> &ct =
                std::use_facet<std::ctype<char> >(loc);

            const std::ctype<wchar_t> &wct =
                std::use_facet<std::ctype<wchar_t> >(loc);

            pdata->locale_name_ = name;

            const std::size_t nchars =
                sizeof pdata->widened_ / sizeof *pdata->widened_;

            for (std::size_t i = 0; i != nchars; ++i) {
                pdata->widened_  [i] = ct.widen (char (i));
                pdata->wwidened_ [i] = wct.widen (char (i));
                pdata->narrowed_ [i] = ct.narrow (char (i), '\0');
            }

            pdata->nwide_ = rw_get_wchars (pdata->wide_, nwide);
            for (std::size_t i = 0; i != pdata->nwide_; ++i)
                pdata->wnarrowed_ [i] = wct.narrow (pdata->wide_ [i], L'\0');

            if (opt_shared_locale)
                pdata->locale_ = loc;

            ++nlocales;
        }
        catch (...) {
            // skip over a bad locale
        }

        if (nlocales == maxinx || nlocales == std::size_t (opt_nlocales))
            break;
    }

    // avoid divide by zero in thread if there are no locales to test
    rw_fatal (nlocales != 0, 0, __LINE__,
              "failed to create one or more usable locales!");

    rw_info (0, 0, 0,
             "testing std::ctype<charT> with %d thread%{?}s%{;}, "
             "%d iteration%{?}s%{;} each, in %zu locales { %{ .*A@} }",
             opt_nthreads, 1 != opt_nthreads,
             opt_nloops, 1 != opt_nloops,
             nlocales, int (nlocales), "%#s", locales);

    rw_info (0, 0, 0, "exercising std::ctype<char>");

    test_char  = true;
    test_wchar = false;

    // create and start a pool of threads and wait for them to finish
    int result =
        rw_thread_pool (0, std::size_t (opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              opt_nthreads, thread_func);

#ifndef _RWSTD_NO_WCHAR_T

    rw_info (0, 0, 0, "exercising std::ctype<wchar_t>");

    test_char  = false;
    test_wchar = true;

    // start a pool of threads to exercise the thread safety
    // of the wchar_t specialization
    result =
        rw_thread_pool (0, std::size_t (opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              opt_nthreads, thread_func);

    // exercise both the char and the wchar_t specializations
    // at the same time

    rw_info (0, 0, 0,
             "exercising both std::ctype<char> and std::ctype<wchar_t>");

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
                    "lib.locale.ctype",
                    "thread safety", run_test,
                    "|-nloops#0 "        // arg must be non-negative
                    "|-nthreads#0-* "    // arg must be in [0, MAX_THREADS]
                    "|-nlocales#0 "      // arg must be non-negative
                    "|-locales= "        // arg must be provided
                    "|-shared-locale# ",
                    &opt_nloops,
                    int (MAX_THREADS),
                    &opt_nthreads,
                    &opt_nlocales,
                    &rw_opt_setlocales,
                    &opt_shared_locale);
}
