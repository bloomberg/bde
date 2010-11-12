/***************************************************************************
 *
 * 21.string.cons.mt.cpp:
 *
 * Test exercising the thread safety of basic_string constructors
 * and assignment operators.
 *
 * $Id: 21.string.cons.mt.cpp 648752 2008-04-16 17:01:56Z faridz $
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

#include <string>       // for string

#include <cstddef>      // for size_t

#include <driver.h>     // for rw_test()
#include <rw_thread.h>  // for rw_thread_pool(), ...
#include <valcmp.h>     // for rw_strncmp()

/**************************************************************************/

#define MAX_THREADS      32
#define MAX_LOOPS    100000

const char* const
data [] = {
    /*  0 */ "",
    /*  1 */ "a",
    /*  2 */ "bc",
    /*  3 */ "cde",
    /*  4 */ "defg",
    /*  5 */ "efghi",
    /*  6 */ "fghijk",
    /*  7 */ "ghijklm"
};

const std::size_t
nstrings = sizeof data / sizeof *data;

const std::string
shared [nstrings] = {
    data [0], data [1], data [2], data [3],
    data [4], data [5], data [6], data [7]
};


#ifndef _RWSTD_NO_WCHAR_T

const wchar_t* const
wdata [] = {
    /*  0 */ L"",
    /*  1 */ L"a",
    /*  2 */ L"bc",
    /*  3 */ L"cde",
    /*  4 */ L"defg",
    /*  5 */ L"efghi",
    /*  6 */ L"fghijk",
    /*  7 */ L"ghijklm"
};

const std::wstring
wshared [nstrings] = {
    wdata [0], wdata [1], wdata [2], wdata [3],
    wdata [4], wdata [5], wdata [6], wdata [7]
};

#endif   // _RWSTD_NO_WCHAR_T

/**************************************************************************/

#ifdef _RWSTD_REENTRANT
int rw_opt_nthreads = 4;
#else   // if !defined (_RWSTD_REENTRANT)
// in non-threaded builds use just one thread
int rw_opt_nthreads = 1;
#endif   // _RWSTD_REENTRANT

// the number of times each thread should iterate
int rw_opt_nloops = MAX_LOOPS;


template <class T>
void
test_string_cons (const T* const             *pdata,
                  const std::basic_string<T> *pshared,
                  std::size_t                 threadno)
{
    typedef std::basic_string<T> String;

    for (std::size_t i = 0; i != std::size_t (rw_opt_nloops); ++i) {

        const std::size_t inx1 = (i + threadno) % nstrings;
        const std::size_t inx2 = (inx1 + 1) % nstrings;

        // create a copy of the global string
        const String copy (pshared [inx1]);

        String assigned (pshared [inx2]);

        {
            // create another copy of the same global string
            const String copy2 (pshared [inx1]);

            // verify that this copy has the expected length...
            RW_ASSERT (pshared [inx1].length () == copy2.length ());

            // ...and the expected data
            RW_ASSERT (0 == rw_strncmp (pdata [inx1], copy2.data ()));

            assigned = pshared [inx1];

            // copy2 gets destroyed...
        }

        // verify that local copy has the expected length...
        RW_ASSERT (pshared [inx1].length () == copy.length ());

        // ...and the expected data
        RW_ASSERT (0 == rw_strncmp (pdata [inx1], copy.data ()));

        // verify that assigned string has the expected length...
        RW_ASSERT (pshared [inx1].length () == assigned.length ());

        // ...and the expected data
        RW_ASSERT (0 == rw_strncmp (pdata [inx1], assigned.data ()));

        // reassign and verify again
        assigned = pdata [inx2];

        RW_ASSERT (pshared [inx2].length () == assigned.length ());
        RW_ASSERT (0 == rw_strncmp (pdata [inx2], assigned.data ()));
    }

}


extern "C" {

bool test_wstring;

static void*
thread_func (void *arg)
{
    const rw_thread_t* const pthread = (rw_thread_t*)arg;

    // get the 0-based thread number
    const std::size_t threadno = std::size_t (pthread->threadno);

    if (test_wstring) {

#ifndef _RWSTD_NO_WCHAR_T

        test_string_cons<wchar_t>(wdata, wshared, threadno);

#endif   // _RWSTD_NO_WCHAR_T

    }
    else {
        test_string_cons (data, shared, threadno);
    }

    return 0;
}

}   // extern "C"

/**************************************************************************/

static int
run_test (int, char**)
{
    rw_info (0, 0, 0,
             "testing std::string with %d thread%{?}s%{;}, "
             "%d iteration%{?}s%{;} each",
             rw_opt_nthreads, 1 != rw_opt_nthreads,
             rw_opt_nloops, 1 != rw_opt_nloops);

    test_wstring = false;

    // create and start a pool of threads and wait for them to finish
    int result =
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

    for (std::size_t i = 0; i != nstrings; ++i) {

        const std::size_t size =
            std::string::traits_type::length (data [i]);

        rw_assert (size == shared [i].size (), 0, 0,
                   "shared string modifed from %#s to %{#S}",
                   data [i], &shared [i]);
    }

#ifndef _RWSTD_NO_WCHAR_T

    rw_info (0, 0, 0,
             "testing std::wstring with %d thread%{?}s%{;}, "
             "%d iteration%{?}s%{;} each",
             rw_opt_nthreads, 1 != rw_opt_nthreads,
             rw_opt_nloops, 1 != rw_opt_nloops);

    test_wstring = true;

    // start a pool of threads to exercise wstring thread safety
    result =
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

    for (std::size_t i = 0; i != nstrings; ++i) {

        const std::size_t size =
            std::wstring::traits_type::length (wdata [i]);

        rw_assert (size == wshared [i].size (), 0, 0,
                   "shared string modifed from %#ls to %{#lS}",
                   wdata [i], &wshared [i]);
    }

#endif   // _RWSTD_NO_WCHAR_T

    return result;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.string.cons",
                    "thread safety", run_test,
                    "|-nloops#0 "       // must be non-negative
                    "|-nthreads#0-*",   // must be in [0, MAX_THREADS]
                    &rw_opt_nloops,
                    int (MAX_THREADS),
                    &rw_opt_nthreads);
}
