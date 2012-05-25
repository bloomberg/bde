/************************************************************************
 *
 * 22.locale.statics.mt.cpp
 *
 * test exercising the thread safety of [lib.locale.statics]
 *
 * $Id: 22.locale.statics.mt.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2007 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#include <bsls_platform.h>

#include <locale>        // for locale
#include <cstring>       // for strlen()

#include <rw_locale.h>   // for rw_locales()
#include <rw_thread.h>   // for rw_get_processors(), rw_thread_pool()
#include <driver.h>      // for rw_test()


// maximum number of threads allowed by the command line interface
#define MAX_THREADS   16

// default number of threads (will be adjusted to the number
// of processors/cores later)
int opt_nthreads = 1;

// the number of times each thread should iterate (unless specified
// otherwise on the command line)
int opt_nloops = 20000;

int opt_classic;
int opt_global;

/**************************************************************************/

// array of locale objects to use for testing
static std::locale
locales [MAX_THREADS];

// number of locale names in the array
static std::size_t
nlocales;

/**************************************************************************/

extern "C" {

static void*
test_classic (void*)
{
    static volatile int nthreads;

    // cast nthreads to int& (see STDCXX-792)
    // casting should be removed after fixing STDCXX-794
    _RWSTD_ATOMIC_PREINCREMENT (_RWSTD_CONST_CAST (int&, nthreads), false);

    // spin until all threads have been created in order to icrease
    // the odds that at least two of them will hit the tested function
    // (and the lazy one-time initialization done by it) at the same
    // time
    while (nthreads < opt_nthreads);

    const std::locale classic (std::locale::classic ());

    _RWSTD_UNUSED (classic);

    return 0;
}


static void*
test_global (void*)
{
    for (std::size_t i = 0; i != opt_nloops; ++i) {

        const std::size_t inx = i % nlocales;

        const std::locale last (std::locale::global (locales [inx]));

        _RWSTD_UNUSED (last);

        // FIXME: verify the consistency of the returned locale
        // by making sure that it matches the locale passed to
        // global() made by the last call to the function by
        // the last thread
    }

    return 0;
}

}   // extern "C"

/**************************************************************************/

static int
run_test (int, char**)
{
    int result = 0;

    if (rw_note (0 <= opt_classic, 0, __LINE__,
                 "std::locale::classic() test disabled")) {

        rw_info (0, 0, 0,
                 "testing std::locale::classic() with %d thread%{?}s%{;}",
                 opt_nthreads, 1 != opt_nthreads);

        // create and start a pool of threads and wait for them to finish
        result = rw_thread_pool (0, std::size_t (opt_nthreads), 0,
                                 test_classic, 0);
    }

    if (rw_note (0 <= opt_global, 0, __LINE__,
                 "std::locale::global(const std::locale&) test disabled")) {


        // find all installed locales for which setlocale(LC_ALL) succeeds
#if TEST_RW_EXTENSIONS
        // Only classic 'C' locale implemented by stlport.
        const char* const locale_list =
            rw_opt_locales ? rw_opt_locales : rw_locales (_RWSTD_LC_ALL);
#else
        const char* const locale_list = "C\0";
#endif

        const std::size_t maxinx = sizeof locales / sizeof *locales;

        const char* locale_names [MAX_THREADS];

        for (const char *name = locale_list;
             *name;
             name += std::strlen (name) + 1) {

            locale_names [nlocales] = name;
            locales [nlocales++]    = std::locale (name);

            if (nlocales == maxinx)
                break;
        }

        rw_info (0, 0, 0,
                 "testing std::locale::global(const std::locale&) with "
                 "%d thread%{?}s%{;}, %d iteration%{?}s%{;} each, in "
                 "%zu locales { %{ .*A@} }",
                 opt_nthreads, 1 != opt_nthreads,
                 opt_nloops, 1 != opt_nloops,
                 nlocales, int (nlocales), "%#s", locale_names);

        // create and start a pool of threads and wait for them to finish
        result = rw_thread_pool (0, std::size_t (opt_nthreads), 0,
                                 test_global, 0);
    }

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
                    "lib.locale.statics",
                    "thread safety",
                    run_test,
                    "|-classic~ "
                    "|-global~ "
                    "|-nloops#0 "        // arg must be non-negative
                    "|-nthreads#0-* "    // arg must be in [0, MAX_THREADS]
                    "|-locales= ",       // argument must be provided
                    &opt_classic,
                    &opt_global,
                    &opt_nloops,
                    int (MAX_THREADS),
                    &opt_nthreads,
                    &rw_opt_setlocales);
}
