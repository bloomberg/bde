/***************************************************************************
 *
 * 18.atomic.xchg.cpp - test exercising the __rw_atomic_exchange() function
 *                      template
 *
 * $Id: atomic_xchg.cpp 580483 2007-09-28 20:55:52Z sebor $
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
 * Copyright 2003-2006 Rogue Wave Software.
 *
 **************************************************************************/

#if TEST_RW_EXTENSIONS // atomic xchange
#include <stdio.h>
#include <string.h>

#include <rw/_mutex.h>

#include <any.h>
#include <cmdopt.h>
#include <driver.h>
#include <rw_thread.h>   // for rw_thread_create()

/**************************************************************************/

struct thr_args_base
{
    static enum tag_t {
        Char, SChar, UChar,
        Short, UShort, Int, UInt, Long, ULong,
        LLong, ULLong
    } type_tag_;

    unsigned long threadno_;     // thread ordinal number
    unsigned long niter_;        // number of iterations
    unsigned long nxchg_;        // number of exchanges
};

thr_args_base::tag_t thr_args_base::type_tag_;

template <class intT>
struct thr_args: thr_args_base
{
    static unsigned long  nincr_;        // number of increments
    static intT          *shared_;       // shared variables
    static unsigned long  nthreads_;     // number of threads

    static intT* get_array ();
};

template <class intT>
unsigned long thr_args<intT>::nincr_;

template <class intT>
intT* thr_args<intT>::shared_ = thr_args<intT>::get_array ();

template <class intT>
unsigned long thr_args<intT>::nthreads_;

// working around compiler bugs that prevent us from defining
// a static array data member of a class template (PR #30009)
template <class intT>
/* static */ intT* thr_args<intT>::get_array ()
{
    static intT array [2];
    return array;
}

/**************************************************************************/

template <class intT>
intT exchange (intT &x, intT y)
{
#ifndef _RWSTD_REENTRANT

    intT save (x);

    x = y;

    return save;

#else   // if defined (_RWSTD_REENTRANT)

    return _RW::__rw_atomic_exchange (x, y, false);

#endif   // _RWSTD_REENTRANT
}

/**************************************************************************/

template <class intT>
void* thread_routine (thr_args<intT> *args)
{
    // each thread operates on one of two shared values to exercise
    // problems due to operating on adjacent bytes or half-words
    const unsigned long inx = args->threadno_ % 2;

    static volatile int failed;

    // exercise atomic_exchange() in a tight loop

    // perform the requested number increments, or until the
    // shared `failed' variable is set to a non-zero value

    for (unsigned long i = 0; i != args->nincr_ && !failed; ++i) {

        for (unsigned long j = 0; !failed; ++j) {

            // increment the number of iterations of this thread
            ++args->niter_;

            // use intT() as a special "lock" value
            const intT old = exchange (args->shared_ [inx], intT ());

            // increment the number of exchanges performed by this thread
            ++args->nxchg_;

            if (intT () != old) {

                // shared variable was not locked by any other thread

                // increment the value of the shared variable, taking
                // care to avoid the special "lock" value of intT()
                intT newval = intT (old + 1);

                if (intT () == newval)
                    ++newval;

                const intT lock = exchange (args->shared_ [inx], newval);

                // increment the number of exchanges
                ++args->nxchg_;

                // the returned value must be the special "lock" value
                if (intT () == lock)
                    break;

                // fail by setting the shared failed variable (to
                // prevent deadlock) if the returned value is not
                // the special "lock" value

                printf ("*** line %d: error: thread %lu failed "
                        "at increment %lu after %lu iterations\n",
                        __LINE__, args->threadno_, i, args->niter_);
                failed = 1;
                return 0;
            }

            if (100UL * args->nincr_ == j) {

                // fail by setting the shared failed variable (to
                // prevent deadlock) if the number of failed attempts
                // to lock the shared variable reaches the requested
                // number of increments * 100 (an arbitrary number)

                printf ("*** line %d: error thread %lu \"timed out\" after "
                        "%lu increments and %lu iterations\n",
                        __LINE__, args->threadno_, i, args->niter_);
                failed = 1;
                return 0;
            }
        }
    }

    return 0;
}

/**************************************************************************/

extern "C" void* thread_routine (void *arg)
{
    thr_args_base* const args = (thr_args_base*)arg;

    printf ("thread %lu starting\n", args->threadno_);

    switch (args->type_tag_) {

    case thr_args_base::Char:
        return thread_routine ((thr_args<char>*)(arg));
    case thr_args_base::SChar:
        return thread_routine ((thr_args<signed char>*)(arg));
    case thr_args_base::UChar:
        return thread_routine ((thr_args<unsigned char>*)(arg));

    case thr_args_base::Short:
        return thread_routine ((thr_args<short>*)(arg));
    case thr_args_base::UShort:
        return thread_routine ((thr_args<unsigned short>*)(arg));

    case thr_args_base::Int:
        return thread_routine ((thr_args<int>*)(arg));
    case thr_args_base::UInt:
        return thread_routine ((thr_args<unsigned int>*)(arg));

    case thr_args_base::Long:
        return thread_routine ((thr_args<long>*)(arg));
    case thr_args_base::ULong:
        return thread_routine ((thr_args<unsigned long>*)(arg));

#ifdef _RWSTD_LONG_LONG

    case thr_args_base::LLong:
        return thread_routine ((thr_args<_RWSTD_LONG_LONG>*)(arg));
    case thr_args_base::ULLong:
        return thread_routine ((thr_args<unsigned _RWSTD_LONG_LONG>*)(arg));

#endif   // _RWSTD_LONG_LONG

    };

    return 0;
}

/**************************************************************************/

/* extern */ int rw_opt_nloops   = 1024 * 1024;
/* extern */ int rw_opt_nthreads = 4;

#define MAX_THREADS   32


template <class intT>
void run_test (intT, thr_args_base::tag_t tag)
{
    static const char* const tname = rw_any_t (intT ()).type_name ();

    if (!rw_enabled (tname)) {
        rw_note (0, 0, 0, "%s test disabled", tname);
        return;
    }

#ifdef _RWSTD_REENTRANT

    static const char* const fun = "__rw_atomic_exchange";

    rw_info (0, 0, 0, "__rw::%s (%s&, %2$s): %d iterations in %d threads",
             fun, tname, rw_opt_nloops, rw_opt_nthreads);

    rw_thread_t tid [MAX_THREADS];

    typedef thr_args<intT> Args;

    Args::nthreads_   = unsigned (rw_opt_nthreads);
    Args::type_tag_   = tag;
    Args::nincr_      = unsigned (rw_opt_nloops);
    Args::shared_ [0] = intT (1);
    Args::shared_ [1] = intT (1);

    _RWSTD_ASSERT (Args::nthreads_ < sizeof tid / sizeof *tid);

    Args args [sizeof tid / sizeof *tid];

    for (unsigned long i = 0; i != Args::nthreads_; ++i) {

        args [i].threadno_ = i;
        args [i].niter_    = 0;
        args [i].nxchg_    = 0;

        rw_fatal (0 == rw_thread_create (tid + i, 0, thread_routine, args + i),
                  0, __LINE__, "thread_create() failed");
    }
            
    for (unsigned long i = 0; i != Args::nthreads_; ++i) {

        rw_error (0 == rw_thread_join (tid [i], 0), 0, __LINE__,
                  "thread_join() failed");

        if (args [i].niter_) {
            // compute the percantage of thread iterations that resulted
            // in increments of one of the shared variables
            const unsigned long incrpcnt =
                (100U * Args::nincr_) / args [i].niter_;

            printf ("thread %lu performed %lu exchanges in %lu iterations "
                    "(%lu%% increments)\n",
                    args [i].threadno_, args [i].nxchg_,
                    args [i].niter_, incrpcnt);
        }
    }

    // compute the expected result, "skipping" zeros by incrementing
    // expect twice when it overflows and wraps around to 0 (zero is
    // used as the lock variable in thread_routine() above)
    intT expect = intT (1);

    const unsigned long nincr = (Args::nthreads_ * Args::nincr_) / 2U;
        
    for (unsigned long i = 0; i != nincr; ++i) {
        if (intT () == ++expect)
            ++expect;
    }

    // verify that the final value of the variables shared among all
    // threads equals the number of increments performed by the threads
    rw_assert (Args::shared_ [0] == expect, 0, __LINE__,
               "1. %s (%s&, %2$s); %s == %s failed",
               fun, tname, TOSTR (Args::shared_ [0]), TOSTR (expect));

    rw_assert (Args::shared_ [1] == expect, 0, __LINE__,
               "2. %s (%s&, %2$s); %s == %s failed",
               fun, tname, TOSTR (Args::shared_ [1]), TOSTR (expect));

#else   // if !defined (_RWSTD_REENTRANT)

    _RWSTD_UNUSED (tag);

#endif   // _RWSTD_REENTRANT
}

/**************************************************************************/

static int
run_test (int, char**)
{
    // exercise atomic exchange
    run_test ((char)0, thr_args_base::Char);
    run_test ((signed char)0, thr_args_base::SChar);
    run_test ((unsigned char)0, thr_args_base::UChar);

    run_test ((short)0, thr_args_base::Short);
    run_test ((unsigned short)0, thr_args_base::UShort);

    run_test ((int)0, thr_args_base::Int);
    run_test ((unsigned int)0, thr_args_base::UInt);

    run_test ((long)0, thr_args_base::Long);
    run_test ((unsigned long)0, thr_args_base::ULong);

#ifdef _RWSTD_LONG_LONG

    run_test ((_RWSTD_LONG_LONG)0, thr_args_base::LLong);
    run_test ((unsigned _RWSTD_LONG_LONG)0, thr_args_base::ULLong);

#endif    // _RWSTD_LONG_LONG

    return 0;

}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    0 /* no clause */,
                    0 /* no comment */, run_test,
                    "|-nloops#0 "        // must be non-negative
                    "|-nthreads#0-*",    // must be in [0, MAX_THREADS]
                    &rw_opt_nloops,
                    int (MAX_THREADS),
                    &rw_opt_nthreads);
}
#else // !TEST_RW_EXTENSIONS
int main()
{
    return 0;
}
#endif // !TEST_RW_EXTENSIONS
