/***************************************************************************
 *
 * 21.string.push.back.mt.cpp - test exercising the therad safety
 *                              of the push_back member function
 *
 * $Id: 21.string.push_back.mt.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2005-2006 Rogue Wave Software.
 *
 **************************************************************************/

#include <string>       // for string
#include <cstddef>      // for size_t

#include <driver.h>     // for rw_test()
#include <rw_thread.h>  // for rw_thread_pool(), ...
#include <valcmp.h>     // for rw_strncmp()

/**************************************************************************/

#define MAX_THREADS      32
#define MAX_LOOPS    100000

const char to_append [] = {
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
};

const std::string shared0;
const std::string shared1 (64, '_');

/**************************************************************************/

#ifdef _RWSTD_REENTRANT
int rw_opt_nthreads = 4;
#else   // if !defined (_RWSTD_REENTRANT)
// in non-threaded builds use just one thread
int rw_opt_nthreads = 1;
#endif   // _RWSTD_REENTRANT

int rw_opt_nloops = MAX_LOOPS;

extern "C" {

static void*
thread_func (void *arg)
{
    const rw_thread_t* const pthread = (rw_thread_t*)arg;

    // compute an index unique to this thread
    const std::size_t thr_inx = std::size_t (pthread->threadno) % MAX_THREADS;

    RW_ASSERT (thr_inx < sizeof to_append);

    // compute the expected strings unique to this thread
    const std::string expect0 = shared0 + to_append [thr_inx];
    const std::string expect1 = shared1 + to_append [thr_inx];

    // get the data pointer and the length of each string
    // in order to avoid referencin the string objects below
    const char* const expect0_data = expect0.data ();
    const std::size_t expect0_len  = expect0.length ();

    const char* const expect1_data = expect1.data ();
    const std::size_t expect1_len  = expect1.length ();

    for (std::size_t i = 0; i != std::size_t (rw_opt_nloops); ++i) {

        // create copies of global strings
        std::string local0 (shared0);
        std::string local1 (shared1);

        // modify the local copies
        local0.push_back (to_append [thr_inx]);
        local1.push_back (to_append [thr_inx]);

        // verify that the local copies have the expected length
        RW_ASSERT (expect0_len == local0.length ());
        RW_ASSERT (expect1_len == local1.length ());

        // verify that the local copies have the expected data
        RW_ASSERT (0 == rw_strncmp (expect0_data, local0.data ()));
        RW_ASSERT (0 == rw_strncmp (expect1_data, local1.data ()));
    }

    return 0;
}

}   // extern "C"

/**************************************************************************/

static int
run_test (int, char**)
{
    rw_info (0, 0, 0, "running %d thread%{?}s%{;}, %d iteration%{?}s%{;} each",
             rw_opt_nthreads, 1 != rw_opt_nthreads,
             rw_opt_nloops, 1 != rw_opt_nloops);

    // create and start a pool of threads and wait for them to finish
    const int result = rw_thread_pool (0, std::size_t (rw_opt_nthreads),
                                       0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

    rw_assert (0 == shared0.size (), 0, 0,
               "shared empty string modifed: expected \"\", got %{#S}",
               &shared0);

    const std::string expect (64, '_');

    rw_assert (64 == shared1.size () && std::string (64, '_') == shared1,
               0, 0, "shared non-empty string modifed: "
               "expected %{#S}, got %{#S}",
               &expect, &shared1);

    return result;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.string.push_back",
                    "thread safety", run_test,
                    "|-nloops#0 "       // must be non-negative
                    "|-nthreads#0-*",   // must be in [0, MAX_THREADS]
                    &rw_opt_nloops,
                    int (MAX_THREADS),
                    &rw_opt_nthreads);
}
