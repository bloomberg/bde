/***************************************************************************
 *
 * 20.temp.buffer.mt.cpp - test exercising the thread safety
 *                         of std::get_temporary_buffer() and
 *                         std::return_temporary_buffer()
 *
 * $Id: 20.temp.buffer.mt.cpp 650350 2008-04-22 01:35:17Z sebor $
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
 * Copyright 2005-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <memory>    // for get_temporary_buffer
#include <utility>   // for pair

#include <cassert>   // for assert
#include <cstddef>   // for ptrdiff_t
#include <cstdio>    // for printf()
#include <cstring>   // for memset()

#include <driver.h>      // for rw_test(), ...
#include <rw_thread.h>   // for rw_thread_create(), ...

/**************************************************************************/

#ifndef _RWSTD_REENTRANT
#  define MAX_THREADS 0
#else
#  define MAX_THREADS 32
#endif

/* extern */ unsigned rw_opt_nthreads = 4 <= MAX_THREADS ? 4 : MAX_THREADS;
/* extern */ unsigned rw_opt_nloops   = 1024 * 1024;

/**************************************************************************/

struct thr_args
{
    unsigned threadno_;   // thread ordinal number
    unsigned ngets_;      // number of calls to get_temporary_buffer
    unsigned nreturns_;   // number of calls to return_temporary_buffer
};

/**************************************************************************/

template <class T>
std::pair<T*, std::ptrdiff_t>
test_get_buffer (thr_args *args, T*, unsigned nobjs)
{
    ++args->ngets_;

    std::pair<T*, std::ptrdiff_t> buf =
        std::get_temporary_buffer<T>(std::ptrdiff_t (nobjs));

//     std::printf ("T%u: get %s: { %p, %d } == %u\n",
//                  args->threadno_, type_name (T ()),
//                  buf.first, buf.second, buf.second * sizeof (T));

    if (buf.first && buf.second) {

        const std::size_t nbytes = buf.second * sizeof (T);

        std::memset (buf.first, args->threadno_ + 1, nbytes);

        *((char*)buf.first + nbytes - 1) = '\0';
    }

    return buf;
}

/**************************************************************************/

template <class T>
void
test_return_buffer (thr_args *args, std::pair<T*, std::ptrdiff_t> &buf)
{
    ++args->nreturns_;

    std::size_t len = 0;

    if (buf.first) {

        typedef unsigned char UChar;

        for (const UChar *pb = (const UChar*)buf.first; ; ++pb, ++len) {

            if (*pb != (args->threadno_ + 1)) {
                if (0 == *pb)
                    ++len;
                break;
            }
        }
    }

//     std::printf ("T%u: return %s: { %p, %d } == %u\n",
//                  args->threadno_, type_name (T ()),
//                  buf.first, buf.second, len);

    assert (len == buf.second * sizeof (T));

    std::return_temporary_buffer<T>(buf.first);
    buf.first = 0;
}

/**************************************************************************/

extern "C" void* thr_func (void *arg)
{
    thr_args* const targs = (thr_args*)arg;

    std::printf ("thread %u starting...\n", targs->threadno_);

#ifndef _RWSTD_INT64_T

    typedef char Type64 [8];

#else   // if defined (_RWSTD_INT64_T)

    typedef _RWSTD_INT64_T Type64;

#endif   // _RWSTD_INT64_T

    std::pair<_RWSTD_INT8_T*,  std::ptrdiff_t> buf0;
    std::pair<_RWSTD_INT16_T*, std::ptrdiff_t> buf1;
    std::pair<_RWSTD_INT32_T*, std::ptrdiff_t> buf2;
    std::pair<Type64*,         std::ptrdiff_t> buf3;

    for (unsigned i = 0; i != rw_opt_nloops; ++i) {

        const unsigned nelems = (i + targs->threadno_) % 32;

        switch ((i + targs->threadno_) % 8) {

        case 0:
            buf0 = test_get_buffer (targs, (_RWSTD_INT8_T*)0, nelems);
            break;

        case 1:
            buf1 = test_get_buffer (targs, (_RWSTD_INT16_T*)0, nelems);
            break;

        case 2:
            buf2 = test_get_buffer (targs, (_RWSTD_INT32_T*)0, nelems);
            break;

        case 3:
            buf3 = test_get_buffer (targs, (Type64*)0, nelems);
            break;

        case 4:
            test_return_buffer (targs, buf0);
            break;

        case 5:
            test_return_buffer (targs, buf2);
            break;

        case 6:
            test_return_buffer (targs, buf1);
            break;

        case 7:
            test_return_buffer (targs, buf3);
            break;
        }
    }

    // free any buffers that remain allocated
    if (buf0.first) test_return_buffer (targs, buf0);
    if (buf1.first) test_return_buffer (targs, buf1);
    if (buf2.first) test_return_buffer (targs, buf2);
    if (buf3.first) test_return_buffer (targs, buf3);

    return 0;
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    rw_info (0, 0, 0, "std::get_temporary_buffer(ptrdiff_t)");
    rw_info (0, 0, 0, "std::return_temporary_buffer(ptrdiff_t)");

    if (0 < rw_opt_nthreads) {

        rw_thread_t tid [MAX_THREADS + 1];

        thr_args targs [MAX_THREADS + 1];

        std::memset (targs, 0, sizeof targs);

        for (unsigned i = 0; i != rw_opt_nthreads; ++i) {

            targs [i].threadno_ = i;

            rw_fatal (0 == rw_thread_create (tid + i, 0, thr_func, targs + i),
                      0, 0, "thread_create() failed");
        }
            
        for (unsigned i = 0; i != rw_opt_nthreads; ++i) {

            rw_error (0 == rw_thread_join (tid [i], 0), 0, 0,
                      "thread_join() failed");
        }
    }
    else {

        thr_args targs;

        std::memset (&targs, 0, sizeof targs);

        thr_func (&targs);

    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.temporary.buffer",
                    0 /* no comment */,
                    run_test,
                    "|-nloops#0 "       // must be non-negative
                    "|-nthreads#0-*",   // must be in specified range
                    &rw_opt_nloops,
                    int (MAX_THREADS),
                    &rw_opt_nthreads);
}
