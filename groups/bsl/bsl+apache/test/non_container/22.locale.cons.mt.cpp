/************************************************************************
 *
 * 22.locale.cons.mt.cpp
 *
 * test exercising the thread safety of locale ctors
 *
 * $Id: 22.locale.cons.mt.cpp 648752 2008-04-16 17:01:56Z faridz $
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

#include <locale>     // for locale

#include <cstring>    // for strlen()

#include <rw_locale.h>
#include <rw_thread.h>   // for rw_get_processors (), rw_thread_pool()
#include <driver.h>


// maximum number of threads allowed by the command line interface
#define MAX_THREADS   32

// default number of threads (will be adjusted to the number
// of processors/cores later)
int opt_nthreads = 1;

// the number of times each thread should iterate (unless specified
// otherwise on the command line)
int opt_nloops = 20000;

// tristate flag set in response to the --no-combine/--enable-combine
// command line option
int opt_combine;

/**************************************************************************/

// array of locale names to use for testing
static const char*
locales [MAX_THREADS];

// number of locale names in the array
static std::size_t
nlocales;

/**************************************************************************/

extern "C" {

static void*
test_ctors (void*)
{
    static const std::locale::category cats[] = {
        std::locale::all,
        std::locale::collate,
        std::locale::ctype,
        std::locale::messages,
        std::locale::monetary,
        std::locale::numeric,
        std::locale::time,
        std::locale::none
    };

    static const std::size_t ncats = sizeof cats / sizeof *cats;

    // the next locale "advanced" in each iteration of the loop
    std::locale next (locales [0]);

    for (int i = 0; i != opt_nloops; ++i) {

        // compute an index into the array of locales
        const std::size_t linx = i % nlocales;

        // create two locale objects from the same name and verify
        // they are equal
        const std::locale first (next.name ().c_str ());
        const std::locale second (next.name ().c_str ());

        RW_ASSERT (first == second);

        // create another locale object as a copy of one of the two
        // above and verify it's equal to the other
        const std::locale third (first);

        RW_ASSERT (second == third);

        // compute the next index into the array of locales
        const std::size_t ninx = (i + 1) % nlocales;
        const char* const next_name = locales [ninx];

        // create the next locale from a name
        next = std::locale (next_name);

        if (0 <= opt_combine) {
            // compute an index into the array of categories
            const std::size_t cinx = i % ncats;
            const std::locale::category cat = cats [cinx];

            // create a locale from another object and the name of yet
            // another locale, combining some (none, one, or all) of their
            // categories
            const std::locale combined (first, next_name, cat);

            // verify that the locales were created correctly
            if (   std::locale::none == cat
#ifdef _MSC_VER
                || std::locale::messages == cat
#endif
                || first == next) {
                RW_ASSERT (combined == first);
            }
            else if (std::locale::all == cat)
                RW_ASSERT (combined == next);
            else
                RW_ASSERT (combined != first && combined != next);

            // repeat the step above but with a locale object
            const std::locale combined_2 (first, next, cat);

            if (   std::locale::none == cat
#ifdef _MSC_VER
                || std::locale::messages == cat
#endif
                || first == next) {
                RW_ASSERT (combined_2 == first);
            }
            else if (std::locale::all == cat)
                RW_ASSERT (combined_2 == next);
            else
                RW_ASSERT (combined_2 != first && combined_2 != next);
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
#if TEST_RW_EXTENSIONS
    // Only classic 'C' locale implemented for stlport.
    const char* const locale_list =
        rw_opt_locales ? rw_opt_locales : rw_locales (_RWSTD_LC_ALL);

    const std::size_t maxinx = sizeof locales / sizeof *locales;

    for (const char *name = locale_list; *name; name += std::strlen (name) +1) {

        locales [nlocales++] = name;

        if (nlocales == maxinx)
            break;
    }
#else
    locales[nlocales++] = "C";
#endif
    int result;

    rw_info (0, 0, 0,
             "testing std::locale ctors with %d thread%{?}s%{;}, "
             "%d iteration%{?}s%{;} each, in %zu locales { %{ .*A@} }",
             opt_nthreads, 1 != opt_nthreads,
             opt_nloops, 1 != opt_nloops,
             nlocales, int (nlocales), "%#s", locales);

    // create and start a pool of threads and wait for them to finish
    result = rw_thread_pool (0, std::size_t (opt_nthreads), 0,
                                 test_ctors, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              opt_nthreads, test_ctors);

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
                    "lib.locale.cons",
                    "thread safety", run_test,
                    "|-combine~ "
                    "|-nloops#0 "        // must be non-negative
                    "|-nthreads#0-* "    // must be in [0, MAX_THREADS]
                    "|-locales=",        // must be provided
                    &opt_combine,
                    &opt_nloops,
                    int (MAX_THREADS),
                    &opt_nthreads,
                    &rw_opt_setlocales);
}
