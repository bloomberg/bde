/************************************************************************
 *
 * 22.locale.numpunct.mt.cpp
 *
 * test exercising the thread safety of the numpunct facet
 *
 * $Id: 22.locale.numpunct.mt.cpp 648752 2008-04-16 17:01:56Z faridz $
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

#include <locale>     // for locale, numpunct

#include <clocale>    // for lconv, localeconv()
#include <cstdlib>    // for mbstowcs()
#include <cstring>    // for size_t, strcpy()

#include <rw_locale.h>
#include <rw_thread.h>
#include <driver.h>
#include <valcmp.h>


// maximum number of threads allowed by the command line interface
#define MAX_THREADS   32

// default number of threads (will be adjusted to the number
// of processors/cores later)
int opt_nthreads = 1;

// the number of times each thread should iterate (unless specified
// otherwise on the command line)
int rw_opt_nloops = 200000;

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

struct NumPunctData
{
    const char* locale_name_;
    std::locale locale_;

    std::string grouping_;

    char decimal_point_;
    char thousands_sep_;
    std::string truename_;
    std::string falsename_;

#ifndef _RWSTD_NO_WCHAR_T

    wchar_t wdecimal_point_;
    wchar_t wthousands_sep_;
    std::wstring wtruename_;
    std::wstring wfalsename_;

#endif   // _RWSTD_NO_WCHAR_T

} punct_data [MAX_THREADS];


extern "C" {

bool test_char;    // exercise num_put<char>
bool test_wchar;   // exercise num_put<wchar_t>


static void*
thread_func (void*)
{
    for (int i = 0; i != rw_opt_nloops; ++i) {

        const std::size_t inx = std::size_t (i) % nlocales;

        const NumPunctData& data = punct_data[inx];

        // construct a named locale
        const std::locale loc =
            opt_shared_locale ? data.locale_
                                 : std::locale (data.locale_name_);

        if (test_char) {
            // exercise the narrow char specialization of the facet

            const std::numpunct<char> &np =
                std::use_facet<std::numpunct<char> >(loc);

            const char        dp  = np.decimal_point ();
            const char        ts  = np.thousands_sep ();
            const std::string grp = np.grouping ();
            const std::string tn  = np.truename ();
            const std::string fn  = np.falsename ();

            RW_ASSERT (dp == data.decimal_point_);
            RW_ASSERT (ts == data.thousands_sep_);
            RW_ASSERT (0 == rw_strncmp (grp.c_str (),
                                        data.grouping_.c_str ()));
            RW_ASSERT (0 == rw_strncmp (tn.c_str (),
                                        data.truename_.c_str ()));
            RW_ASSERT (0 == rw_strncmp (fn.c_str (),
                                        data.falsename_.c_str ()));
        }

        // both specializations may be tested at the same time

        if (test_wchar) {
            // exercise the wide char specialization of the facet

#ifndef _RWSTD_NO_WCHAR_T

            const std::numpunct<wchar_t> &wp =
                std::use_facet<std::numpunct<wchar_t> >(loc);

            const wchar_t      dp  = wp.decimal_point ();
            const wchar_t      ts  = wp.thousands_sep ();
            const std::string  grp = wp.grouping ();
            const std::wstring tn  = wp.truename ();
            const std::wstring fn  = wp.falsename ();

            RW_ASSERT (dp == data.wdecimal_point_);
            RW_ASSERT (ts == data.wthousands_sep_);
            RW_ASSERT (0 == rw_strncmp (grp.c_str (),
                                        data.grouping_.c_str ()));
            RW_ASSERT (0 == rw_strncmp (tn.c_str (),
                                        data.wtruename_.c_str ()));
            RW_ASSERT (0 == rw_strncmp (fn.c_str (),
                                        data.wfalsename_.c_str ()));

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

    const std::size_t maxinx = sizeof punct_data / sizeof *punct_data;

    // iterate over locales, initializing a global punct_data array
    for (const char *name = locale_list;
         *name;
         name += std::strlen (name) +1) {

        const std::size_t inx = nlocales;
        locales [inx] = name;

        NumPunctData& data = punct_data [inx];

        try {
            std::locale loc(name);
            data.locale_name_ = name;

            const std::numpunct<char> &np =
                std::use_facet<std::numpunct<char> >(loc);

            data.grouping_ = np.grouping ();

            data.decimal_point_ = np.decimal_point ();
            data.thousands_sep_ = np.thousands_sep ();

            data.truename_  = np.truename ();
            data.falsename_ = np.falsename ();

#ifndef _RWSTD_NO_WCHAR_T

            const std::numpunct<wchar_t> &wp =
                std::use_facet<std::numpunct<wchar_t> >(loc);

            data.wdecimal_point_ = wp.decimal_point ();
            data.wthousands_sep_ = wp.thousands_sep ();

            data.wtruename_  = wp.truename ();
            data.wfalsename_ = wp.falsename ();

#endif

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
             "testing std::numpunct<charT> with %d thread%{?}s%{;}, "
             "%d iteration%{?}s%{;} each, in %zu locales { %{ .*A@} }",
             opt_nthreads, 1 != opt_nthreads,
             rw_opt_nloops, 1 != rw_opt_nloops,
             nlocales, int (nlocales), "%#s", locales);

    rw_info (0, 0, 0, "exercising std::numpunct<char>");

    test_char  = true;
    test_wchar = false;

    // create and start a pool of threads and wait for them to finish
    int result =
        rw_thread_pool (0, std::size_t (opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              opt_nthreads, thread_func);

#ifndef _RWSTD_NO_WCHAR_T

    rw_info (0, 0, 0, "exercising std::numpunct<wchar_t>");

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
             "exercising both std::numpunct<char> and std::numpunct<wchar_t>");

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
                    "lib.locale.numpunct",
                    "thread safety", run_test,
                    "|-nloops#0 "       // must be non-negative
                    "|-nthreads#0-* "   // must be in [0, MAX_THREADS]
                    "|-nlocales#0 "     // arg must be non-negative
                    "|-locales= "       // must be provided
                    "|-shared-locale# ",
                    &rw_opt_nloops,
                    int (MAX_THREADS),
                    &opt_nthreads,
                    &opt_nlocales,
                    &rw_opt_setlocales,
                    &opt_shared_locale);
}
