/***************************************************************************
 *
 * 18.atomic.add.cpp - test exercising the __rw_atomic_preincrement()
 *                     and __rw_atomic_predecrement function templates
 *
 * $Id: atomic_add.cpp 425530 2006-07-25 21:39:50Z sebor $
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

#if TEST_RW_EXTENSIONS // atomic add
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
};

thr_args_base::tag_t thr_args_base::type_tag_;

template <class intT>
struct thr_args: thr_args_base
{
    unsigned         threadno_;     // thread ordinal number
    static int       inc_;          // increment, decrement, or both
    static unsigned  nincr_;        // number of increments
    static intT     *shared_;       // shared variables
    static unsigned  nthreads_;     // number of threads

    static intT* get_array ();
};

template <class intT>
int thr_args<intT>::inc_;

template <class intT>
unsigned thr_args<intT>::nincr_;

template <class intT>
intT* thr_args<intT>::shared_ = thr_args<intT>::get_array ();

template <class intT>
unsigned thr_args<intT>::nthreads_;

// working around compiler bugs that prevent us from defining
// a static array data member of a class template (PR #30009)
template <class intT>
intT* thr_args<intT>::get_array ()
{
    static intT array [2];
    return array;
}

/**************************************************************************/


template <class T>
T preincrement (T &x)
{
#ifndef _RWSTD_REENTRANT

    return ++x;

#else   // if defined (_RWSTD_REENTRANT)

    return _RW::__rw_atomic_preincrement (x, false);

#endif   // _RWSTD_REENTRANT
}


template <class T>
T predecrement (T &x)
{
#ifndef _RWSTD_REENTRANT

    return --x;

#else   // if defined (_RWSTD_REENTRANT)

    return _RW::__rw_atomic_predecrement (x, false);

#endif   // _RWSTD_REENTRANT
}

/**************************************************************************/


template <class intT>
void* thread_routine (thr_args<intT> *args)
{
    printf ("thread %u starting to %screment\n",
            args->threadno_,
              args->inc_ < 0 ? "de"
            : args->inc_ > 0 ? "in"
            : "decrement and in");

    if (args->inc_ > 0) {

        // exercise atomic_preincrement() in a tight loop

        for (unsigned i = 0; i != args->nincr_; ++i) {
            preincrement (args->shared_ [0]);
            preincrement (args->shared_ [1]);
        }
    }
    else if (args->inc_ < 0) {

        // exercise atomic_predecrement() in a tight loop

        for (unsigned i = 0; i != args->nincr_; ++i) {
            predecrement (args->shared_ [0]);
            predecrement (args->shared_ [1]);
        }
    }
    else {

        // exercise both atomic_preincrement() and atomic_predecrement()

        for (unsigned i = 0; i != args->nincr_; ++i) {
            preincrement (args->shared_ [0]);
            predecrement (args->shared_ [0]);
            preincrement (args->shared_ [1]);
            predecrement (args->shared_ [1]);
        }
    }

    return 0;
}

/**************************************************************************/

extern "C" void* thread_routine (void *arg)
{
    thr_args_base* const args = (thr_args_base*)arg;

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
void run_test (intT, thr_args_base::tag_t tag, int inc)
{
    static const char* const tname = rw_any_t (intT ()).type_name ();

    if (!rw_enabled (tname)) {
        rw_note (0, 0, 0, "%s test disabled", tname);
        return;
    }

#if defined (_RWSTD_REENTRANT)

    static const char* const fun =
          inc < 0 ? "__rw_atomic_predecrement"
        : inc > 0 ? ":__rw_atomic_preincrement"
        : "__rw_atomic_pre{in,de}crement";

    rw_info (0, 0, 0, "__rw::%s (%s&): %d iterations in %d threads",
             fun, tname, rw_opt_nloops, rw_opt_nthreads);

    rw_thread_t tid [MAX_THREADS];

    typedef thr_args<intT> Args;

    Args::nthreads_   = rw_opt_nthreads;
    Args::type_tag_   = tag;
    Args::inc_        = inc;
    Args::nincr_      = unsigned (rw_opt_nloops);
    Args::shared_ [0] = intT ();
    Args::shared_ [1] = intT ();

    _RWSTD_ASSERT (Args::nthreads_ < sizeof tid / sizeof *tid);

    Args args [sizeof tid / sizeof *tid];

    memset (args, 0, sizeof args);

    for (unsigned i = 0; i != Args::nthreads_; ++i) {

        args [i].threadno_ = i;

        rw_fatal (0 == rw_thread_create (tid + i, 0, thread_routine, args + i),
                  0, __LINE__, "thread_create() failed");
    }

    for (unsigned i = 0; i != Args::nthreads_; ++i) {

        rw_error (0 == rw_thread_join (tid [i], 0), 0, __LINE__,
                  "thread_join() failed");
    }

    // compute the expected result
    intT expect = intT ();

    if (inc < 0) {
        for (unsigned i = 0; i != Args::nthreads_ * Args::nincr_; ++i)
            --expect;
    }
    else if (inc > 0) {
        for (unsigned i = 0; i != Args::nthreads_ * Args::nincr_; ++i)
            ++expect;
    }

    // verify that the final value of the variable shared among all
    // threads equals the number of increments or decrements performed
    // by all threads
    rw_assert (Args::shared_ [0] == expect, 0, __LINE__,
               "1. %s (%s&); %s == %s failed",
               fun, tname, TOSTR (Args::shared_ [0]), TOSTR (expect));

    rw_assert (Args::shared_ [1] == expect, 0, __LINE__,
               "2. %s (%s&); %s == %s failed",
               fun, tname, TOSTR (Args::shared_ [1]), TOSTR (expect));

#else   // if !defined (_RWSTD_REENTRANT)

    _RWSTD_UNUSED (tag);
    _RWSTD_UNUSED (inc);

#endif   // _RWSTD_REENTRANT
}

/**************************************************************************/

static int
run_test (int, char**)
{
    // exercise atomic subtract
    run_test ((char)0, thr_args_base::Char, -1);
    run_test ((signed char)0, thr_args_base::SChar, -1);
    run_test ((unsigned char)0, thr_args_base::UChar, -1);

    run_test ((short)0, thr_args_base::Short, -1);
    run_test ((unsigned short)0, thr_args_base::UShort, -1);

    run_test ((int)0, thr_args_base::Int, -1);
    run_test ((unsigned int)0, thr_args_base::UInt, -1);

    run_test ((long)0, thr_args_base::Long, -1);
    run_test ((unsigned long)0, thr_args_base::ULong, -1);

#ifdef _RWSTD_LONG_LONG

    run_test ((_RWSTD_LONG_LONG)0, thr_args_base::LLong, -1);
    run_test ((unsigned _RWSTD_LONG_LONG)0, thr_args_base::ULLong, -1);

#endif    // _RWSTD_LONG_LONG

    // exercise atomic add
    run_test ((char)0, thr_args_base::Char, +1);
    run_test ((signed char)0, thr_args_base::SChar, +1);
    run_test ((unsigned char)0, thr_args_base::UChar, +1);

    run_test ((short)0, thr_args_base::Short, +1);
    run_test ((unsigned short)0, thr_args_base::UShort, +1);

    run_test ((int)0, thr_args_base::Int, +1);
    run_test ((unsigned int)0, thr_args_base::UInt, +1);

    run_test ((long)0, thr_args_base::Long, +1);
    run_test ((unsigned long)0, thr_args_base::ULong, +1);

#ifdef _RWSTD_LONG_LONG

    run_test ((_RWSTD_LONG_LONG)0, thr_args_base::LLong, +1);
    run_test ((unsigned _RWSTD_LONG_LONG)0, thr_args_base::ULLong, +1);

#endif    // _RWSTD_LONG_LONG


    // exercise both atomic add and subtract
    run_test ((char)0, thr_args_base::Char, 0);
    run_test ((signed char)0, thr_args_base::SChar, 0);
    run_test ((unsigned char)0, thr_args_base::UChar, 0);

    run_test ((short)0, thr_args_base::Short, 0);
    run_test ((unsigned short)0, thr_args_base::UShort, 0);

    run_test ((int)0, thr_args_base::Int, 0);
    run_test ((unsigned int)0, thr_args_base::UInt, 0);

    run_test ((long)0, thr_args_base::Long, 0);
    run_test ((unsigned long)0, thr_args_base::ULong, 0);

#ifdef _RWSTD_LONG_LONG

    run_test ((_RWSTD_LONG_LONG)0, thr_args_base::LLong, 0);
    run_test ((unsigned _RWSTD_LONG_LONG)0, thr_args_base::ULLong, 0);

#endif    // _RWSTD_LONG_LONG

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    0 /* no clause */,
                    0 /* no comment */,
                    run_test,
                    "|-nloops#0 "        // must be non-negative
                    "|-nthreads#0-*" ,   // must be in [0, MAX_THREADS]
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
