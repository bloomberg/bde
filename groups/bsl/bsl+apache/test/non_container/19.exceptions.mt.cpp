/***************************************************************************
 *
 * 19.exceptions.mt.cpp - test exercising the thread safety
 *                        of C++ Standard Library exception classes
 *
 * $Id: 19.exceptions.mt.cpp 425530 2006-07-25 21:39:50Z sebor $
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

#include <stdexcept>   // for exceptions
#include <string>      // for string

#include <cassert>     // for assert
#include <cstdio>      // for printf()

#include <cmdopt.h>
#include <driver.h>
#include <rw_alarm.h>    // for rw_alarm()
#include <rw_thread.h>   // for rw_thread_pool()
#include <valcmp.h>

/**************************************************************************/

#ifndef NTHREADS
#  ifndef _RWSTD_REENTRANT
#    define MAX_THREADS    0
#    define NTHREADS       0
#  else
#    define MAX_THREADS   32
#    define NTHREADS       4
#  endif   // _RWSTD_REENTRANT
#endif   // NTHREADS

/**************************************************************************/

/* extern */ int rw_opt_nloops   = 256 * 1024;
/* extern */ int rw_opt_nthreads = NTHREADS;

/**************************************************************************/

volatile int alarm_expired;

extern "C" {

static void handle_alarm (int)
{
    alarm_expired = 1;
}

}   // extern "C"


// string to intialize exceptions from
static const char what_buf [] = {
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789ABCDEF"
};


enum {
    exception_tag,
    logic_error_tag,
    domain_error_tag,
    invalid_argument_tag,
    length_error_tag,
    out_of_range_tag,
    runtime_error_tag,
    range_error_tag,
    overflow_error_tag,
    underflow_error_tag,
    Derived_tag
};


struct DerivedException: std::invalid_argument
{
    DerivedException (const char *str)
        : std::invalid_argument (str) { }
};


static void
throw_exception (unsigned which, const char *what)
{
    switch (which) {
    case exception_tag:        throw std::exception ();
    case logic_error_tag:      throw std::logic_error (what);
    case domain_error_tag:     throw std::domain_error (what);
    case invalid_argument_tag: throw std::invalid_argument (what);
    case length_error_tag:     throw std::length_error (what);
    case out_of_range_tag:     throw std::out_of_range (what);
    case runtime_error_tag:    throw std::runtime_error (what);
    case range_error_tag:      throw std::range_error (what);
    case overflow_error_tag:   throw std::overflow_error (what);
    case underflow_error_tag:  throw std::underflow_error (what);
    case Derived_tag:          throw DerivedException (what);

    default: _RWSTD_ASSERT (!"logic error: bad exception tag");
    }
}

/**************************************************************************/

extern "C" void*
test_single_exception (void *arg)
{
    const rw_thread_t* const tid = (rw_thread_t*)arg;

    std::printf ("thread procedure %ld starting...\n", tid->threadno);

    for (unsigned i = 0; i < unsigned (rw_opt_nloops); ++i) {

        if (alarm_expired)
            break;

#if DRQS
        const std::size_t what_len = std::size_t (i % 1024);
#else
        const std::size_t what_len = std::size_t (i % 256);
#endif
        const char* const what = what_buf + sizeof what_buf - what_len - 1;

        const unsigned thrown = i % 11;

        unsigned caught = _RWSTD_UINT_MAX;

        try {
            // construct and throw an exception object of one
            // of the predefined standard exception classes
            // initialized with a distinct what string
            throw_exception (thrown, what);
        }
        catch (DerivedException ex) {
            // catch the exception object by value and verify
            // that the pointer returned by what() compares
            // equal to the string the thrown object was
            // constructed with
            caught = Derived_tag;
            assert (0 == rw_strncmp (ex.what (), what));
        }
        catch (std::domain_error ex) {
            caught = domain_error_tag;
            assert (0 == rw_strncmp (ex.what (), what));
        }
        catch (std::invalid_argument ex) {
            caught = invalid_argument_tag;
            assert (0 == rw_strncmp (ex.what (), what));
        }
        catch (std::length_error ex) {
            caught = length_error_tag;
            assert (0 == rw_strncmp (ex.what (), what));
        }
        catch (std::out_of_range ex) {
            caught = out_of_range_tag;
            assert (0 == rw_strncmp (ex.what (), what));
        }
        catch (std::range_error ex) {
            caught = range_error_tag;
            assert (0 == rw_strncmp (ex.what (), what));
        }
        catch (std::overflow_error ex) {
            caught = overflow_error_tag;
            assert (0 == rw_strncmp (ex.what (), what));
        }
        catch (std::underflow_error ex) {
            caught = underflow_error_tag;
            assert (0 == rw_strncmp (ex.what (), what));
        }
        catch (std::logic_error ex) {
            caught = logic_error_tag;
            assert (0 == rw_strncmp (ex.what (), what));
        }
        catch (std::runtime_error ex) {
            caught = runtime_error_tag;
            assert (0 == rw_strncmp (ex.what (), what));
        }
        catch (std::exception ex) {
            caught = exception_tag;
            rw_strncmp (ex.what (), what);
        }

        // verify that an object of the thrown type was caught
        assert (caught == thrown);
    }

    return 0;
}

/**************************************************************************/

static void
test_multi (unsigned i, unsigned nactive)
{
#if DRQS
    const std::size_t what_len = (i + nactive) % sizeof what_buf;
#else
    const std::size_t what_len = ((i + nactive) % sizeof what_buf) % 256;
#endif
    const char* const what = what_buf + sizeof what_buf - what_len - 1;
    const unsigned thrown = (i + nactive) % 10 + 1;

    try {
        // construct and throw an exception object of a distinct type
        // with a distinct what string at each level of recursion
        throw_exception (thrown, what);
    }
    catch (std::exception &ex) {

        // recursively throw another exception while the caught
        // exception object is still active
        if (nactive)
            test_multi (i, nactive - 1);

        // verify that the caught object's what string matches
        // the string the object was originally constructed with
        assert (0 == rw_strncmp (ex.what (), what));
    }
}

extern "C" void*
test_multi_exceptions (void *arg)
{
    const rw_thread_t* const tid = (rw_thread_t*)arg;

    std::printf ("thread procedure %ld starting...\n", tid->threadno);

    for (unsigned i = 0; i < unsigned (rw_opt_nloops); ++i) {

        if (alarm_expired)
            break;

        // exercise up to 4 simultaneously active exceptions
        test_multi (i, i % 4);
    }

    return 0;
}

/**************************************************************************/

static int
run_test (int, char**)
{
    // get the current alarm (if any) set for the test
    // on the command line without resetting it
    const unsigned max_sec = rw_alarm (0, rw_sig_hold);

    // compute a shorter timeout for each of the two subtests
    const unsigned nsec = 3 < max_sec ? max_sec / 2 : 0;

    rw_info (0, 0, 0,
             "single active exception per thread"
             "%{?}; timeout in %u seconds%{;}",
             0 != max_sec, nsec ? nsec : max_sec);

    // set a shorter alarm if possible
    if (nsec) {
        alarm_expired = 0;
        rw_alarm (nsec, handle_alarm);
    }

    const std::size_t nthreads = std::size_t (rw_opt_nthreads);

#if 0 < NTHREADS 

    rw_fatal (0 == rw_thread_pool (0, nthreads, 0, test_single_exception, 0),
              0, __LINE__, "rw_thread_pool() failed");

#else   // if !(0 < NTHREADS)

    rw_thread_t tid = rw_thread_t ();

    test_single_exception (&tid);

#endif   // NTHREADS

    rw_info (0, 0, 0,
             "multiple active exceptions per thread"
             "%{?}; timeout in %u seconds%{;}",
             0 != max_sec, nsec ? nsec : max_sec);

    // set another shorter alarm if possible
    if (nsec) {
        alarm_expired = 0;
        rw_alarm (nsec, handle_alarm);
    }

#if 0 < NTHREADS 

    rw_fatal (0 == rw_thread_pool (0, nthreads, 0, test_multi_exceptions, 0),
              0, __LINE__, "rw_thread_pool() failed");

#else   // if !(0 < NTHREADS)

    test_multi_exceptions (&tid);

#endif   // NTHREADS

    // restore the original alarm to go off approximately
    // when the original alar would have if it hadn't been
    // replaced above
    if (nsec)
        rw_alarm (2 * nsec, rw_sig_restore);

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.std.exceptions",
                    "thread safety", run_test,
                    "|-nloops#0 "        // must be non-negative
                    "|-nthreads#0-*",    // must be in [0, MAX_THREADS]
                    &rw_opt_nloops,
                    int (MAX_THREADS),
                    &rw_opt_nthreads);
}
