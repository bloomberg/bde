/************************************************************************
 *
 * thread.cpp - definitions of testsuite thread helpers
 *
 * $Id: thread.cpp 587713 2007-10-24 00:23:56Z sebor $
 *
 ************************************************************************
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
 * Copyright 2005-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <rw_thread.h>
#include <stddef.h>     // for size_t
#include <string.h>     // for memset()

#ifndef _WIN32
#  include <stdio.h>      // for FILE, fscanf(), popen()
#  include <unistd.h>     // for sysconf(), _SC_NPROCESSORS_{CONF,ONLN}
#else
#  include <windows.h>    // for GetSystemInfo()
#endif   // _WIN32

#ifndef _RWSTD_NO_PURE_C_HEADERS

extern "C" {
FILE* popen (const char*, const char*);
}   // extern "C"

#endif   // _RWSTD_NO_PURE_C_HEADERS

/**************************************************************************/

static long maxthreads;


#if defined (_RWSTD_POSIX_THREADS)
#  include <pthread.h>

extern "C" {

_TEST_EXPORT int
rw_thread_create (rw_thread_t *thr_id,
                  rw_thread_attr_t*,
                  void* (*thr_proc)(void*),
                  void *thr_arg)
{
#ifdef _RWSTD_OS_SUNOS

    static int concurrency_set;

    if (0 == concurrency_set) {
        pthread_setconcurrency (4);
        concurrency_set = 1;
    }

#endif   // _RWSTD_OS_SUNOS


    rw_thread_t tmpid;

    if (0 == thr_id) {
        thr_id = &tmpid;
    }

    pthread_t tid;

    // set the thread number *before* creating the thread
    // so that it's visible in thr_proc when it starts to
    // run even before pthread_create returns
    thr_id->threadno = maxthreads;

    const int result = pthread_create (&tid, 0, thr_proc, thr_arg);

    if (0 == result) {
        thr_id->id     = (long)tid;
        thr_id->handle = 0;
        ++maxthreads;
    }

    return result;
}


_TEST_EXPORT int
rw_thread_join (rw_thread_t thr_id, void **parg)
{
    const int result = pthread_join ((pthread_t)thr_id.id, parg);

    return result;
}

}   // extern "C"

/**************************************************************************/

#elif defined (_RWSTD_SOLARIS_THREADS)
#  include <thread.h>

extern "C" {

_TEST_EXPORT int
rw_thread_create (rw_thread_t *thr_id,
                  rw_thread_attr_t*,
                  void* (*thr_proc)(void*),
                  void *thr_arg)
{
    static int concurrency_set;

    if (0 == concurrency_set) {
        thr_setconcurrency (4);
        concurrency_set = 1;
    }

    rw_thread_t tmpid;

    if (0 == thr_id) {
        thr_id = &tmpid;
    }

    thread_t tid;

    // set the thread number *before* creating the thread
    // so that it's visible in thr_proc when it starts to
    // run even before thr_create returns
    thr_id->threadno = maxthreads;

    const int result =
        thr_create (0,          // stack_base
                    0,          // stack_size
                    thr_proc,   // start_func
                    thr_arg,    // arg
                    0,          // flags
                    &tid);      // new_thread_ID

    if (0 == result) {
        thr_id->id     = (long)tid;
        thr_id->handle = 0;
        ++maxthreads;
    }

    return result;
}


_TEST_EXPORT int
rw_thread_join (rw_thread_t thr_id, void **parg)
{
    const int result = thr_join ((thread_t)thr_id.id, 0, parg);

    return result;
}

}   // extern "C"

/**************************************************************************/

#elif defined (_RWSTD_DEC_THREADS)

#  include <setjmp.h>
#  include <cma.h>


extern "C" {

_TEST_EXPORT int
rw_thread_create (rw_thread_t *thr_id,
                  rw_thread_attr_t*,
                  void* (*thr_proc)(void*),
                  void *thr_arg)
{
    rw_thread_t tmpid;

    if (0 == thr_id) {
        thr_id = &tmpid;
    }

    int result = 0;

    cma_t_thread tid;

    // set the thread number *before* creating the thread
    // so that it's visible in thr_proc when it starts to
    // run even before cma_thread_create returns
    thr_id->threadno = maxthreads;

    TRY {
        // cma_thread_create() returns void but throws an exception on error
        cma_thread_create (&tid,        // new_thread
                           0,           // attr
                           thr_proc,    // start_routine
                           &thr_arg);   // arg

        thr_id->id     = tid.field1;
        thr_id->handle = (void*)tid.field2;
        ++maxthreads;
    }
    CATCH_ALL {
        result = -1;
    }
    ENDTRY

    return result;
}


_TEST_EXPORT int
rw_thread_join (rw_thread_t thr_id, void **parg)
{
    int status = 0;

    cma_t_thread tid = {
        thr_id.id, (long)thr_id.handle
    };

    TRY {
        // cma_thread_join() returns void but throws an exception on error
        cma_thread_join (&tid, 0, parg);
    }
    CATCH_ALL {
        status = -1;
    }
    ENDTRY

    return status;
}

}   // extern "C"

/**************************************************************************/

#elif defined (_WIN32) && defined (_MT)
#  include <process.h>    // for _beginthreadex()

extern "C" {

_TEST_EXPORT int
rw_thread_create (rw_thread_t *thr_id,
                  rw_thread_attr_t*,
                  void* (*thr_proc)(void*),
                  void *thr_arg)
{
    int result = 0;

    rw_thread_t tmpid;

    if (0 == thr_id)
        thr_id = &tmpid;

    unsigned nid;   // numerical id

    typedef unsigned int (__stdcall *win32_thr_proc_t)(void *);
    win32_thr_proc_t win32_thr_proc =
        _RWSTD_REINTERPRET_CAST (win32_thr_proc_t, thr_proc);

    // set the thread number *before* creating the thread
    // so that it's visible in thr_proc when it starts to
    // run even before CreateThread returns
    thr_id->threadno = maxthreads;

    const uintptr_t hthread =
        _beginthreadex (0,                // lpThreadAttributes
                        0,                // dwStackSize
                        win32_thr_proc,   // lpStartAddress
                        thr_arg,          // lpParameter
                        0,                // dwCreationFlags
                        &nid);            // lpThreadId

    if (!hthread) {
        thr_id->id     = -1;
        thr_id->handle = 0;
        result         = -1;
    }
    else {
        thr_id->id     = nid;
        thr_id->handle = _RWSTD_REINTERPRET_CAST (void*, hthread);
        ++maxthreads;
    }

    return result;
}


_TEST_EXPORT int
rw_thread_join (rw_thread_t thr_id, void **parg)
{
    int result = 0;

    const DWORD retcode = WaitForSingleObject (thr_id.handle, INFINITE);

    if (WAIT_OBJECT_0 == retcode) {
        if (parg) {
            DWORD exit_code;

            if (GetExitCodeThread (thr_id.handle, &exit_code))
                *parg = (void*)exit_code;
            else
                result = -1;
        }
    }
    else {
        result = -1;
    }

    return result;
}

}   // extern "C"

/**************************************************************************/

#else   // unknown/missing threads environment

#  include <errno.h>

#  ifndef ENOTSUP
#    if defined (_RWSTD_OS_AIX)
#      define ENOTSUP    124
#    elif defined (_RWSTD_OS_HP_UX)
#      define ENOTSUP    252
#    elif defined (_RWSTD_OS_IRIX64)
#      define ENOTSUP   1008
#    elif defined (_RWSTD_OS_LINUX)
#      define ENOTSUP    524
#    elif defined (_RWSTD_OS_OSF1)
#      define ENOTSUP     99
#    elif defined (_RWSTD_OS_SUNOS)
#      define ENOTSUP     48
#    elif defined (_WIN32) || defined (_WIN64)
#      define ENOTSUP ENOSYS
#    else
#      define ENOTSUP   9999
#    endif
#  endif   // ENOTSUP

extern "C" {

_TEST_EXPORT int
rw_thread_create (rw_thread_t*,
                  rw_thread_attr_t*,
                  void* (*)(void*),
                  void*)
{
    _RWSTD_UNUSED (maxthreads);

    return ENOTSUP;
}


_TEST_EXPORT int
rw_thread_join (rw_thread_t, void**)
{
    return ENOTSUP;
}

}   // extern "C"

#endif   // threads environment

/**************************************************************************/

// retrieves the number of processors/cores on the system
_TEST_EXPORT int
rw_get_cpus ()
{
#ifndef _WIN32

    const char* const cmd = {
        // shell command(s) to obtain the number of processors

#  ifdef _RWSTD_OS_AIX
        // AIX: /etc/lsdev -Cc processor | wc -l
        "/etc/lsdev -Cc processor | /usr/bin/wc -l"
#  elif defined (_RWSTD_OS_LINUX)
        // Linux: cat /proc/cpuinfo | grep processor | wc -l
        "cat /proc/cpuinfo "
        "  | grep processor "
        "  | wc -l"
#  elif defined (_RWSTD_OS_FREEBSD)
        // FreeBSD: /sbin/sysctl -n hw.ncpu
        "/sbin/sysctl -n hw.ncpu"
#  elif defined (_RWSTD_OS_HP_UX)
        // HP-UX: /etc/ioscan -k -C processor | grep processor | wc -l
        "/etc/ioscan -k -C processor "
        "  | /usr/bin/grep processor "
        "  | /usr/bin/wc -l"
#  elif defined (_RWSTD_OS_IRIX64)
        // IRIX: hinv | /usr/bin/grep "^[1-9][0-9]* .* Processor"
        "/sbin/hinv "
        "  | /usr/bin/grep \"^[1-9][0-9]* .* Processor\""
#  elif defined (_RWSTD_OS_OSF1)
        // Tru64 UNIX: /usr/sbin/psrinfo | grep online | wc -l
        "/usr/sbin/psrinfo "
        "  | /usr/bin/grep on[-]*line "
        "  | /usr/bin wc -l"
#  elif defined (_RWSTD_OS_SUNOS)
        // Solaris: /usr/bin/mpstat | wc -l
        "/usr/bin/mpstat "
        "  | /usr/bin/grep -v \"^CPU\" "
        "  | /usr/bin/wc -l"
#  else
        0
#  endif

    };

    int ncpus = -1;

#  ifdef _SC_NPROCESSORS_ONLN
    // try to obtain the number of processors that are currently online
    // programmatically and fall back on the shell script above if it
    // fails
    ncpus = int (sysconf (_SC_NPROCESSORS_ONLN));

#  elif defined (_SC_NPROCESSORS_CONF)

    // try to obtain the number of processors the system is configured
    // with (not all of them are necessarily online) programmatically
    // and fall back on the shell script above if it fails
    ncpus = int (sysconf (_SC_NPROCESSORS_CONF));

#  endif   // _SC_NPROCESSORS_CONF

    if (ncpus < 1 && cmd) {
        // if the number of processors couldn't be determined using
        // sysconf() above,  open and read the output of the command
        // from a pipe
        FILE* const fp = popen (cmd, "r");

        if (fp) {
            int tmp = 0;
        
            int n = fscanf (fp, "%d", &tmp);

            if (1 == n)
                ncpus = tmp;

            fclose (fp);
        }
    }

    return ncpus;

#else    // _WIN32

    SYSTEM_INFO info;
    GetSystemInfo (&info);
    return int (info.dwNumberOfProcessors);

#endif   // _WIN32
}

/**************************************************************************/

extern "C" {


_TEST_EXPORT int
rw_thread_pool (rw_thread_t        *thr_id,
                size_t              nthrs,
                rw_thread_attr_t*,
                void*             (*thr_proc)(void*),
                void*              *thr_arg)
{
    // small buffer for thread ids when invoked with (thr_id == 0)
    rw_thread_t id_buf [16];

    const bool join = 0 == thr_id;

#ifdef _RWSTD_REENTRANT

    if (_RWSTD_SIZE_MAX == nthrs) {
        // when the number of threads is -1 use the number
        // of processors plus 1 (in case it's 1 to begin
        // with)

        const int ncpus = rw_get_cpus ();

        if (0 < ncpus)
            nthrs = size_t (ncpus) + 1;
        else
            nthrs = 2;
    }

#else

    // when not reentrant/thread safe emulate the creation
    // of a single thread and then waiting for it to finish
    // by simply calling the thread procedure

    if (1 == nthrs && join) {

        if (0 == thr_id) {
            thr_id = id_buf;
            memset (thr_id, 0, sizeof *thr_id);
        }

        // when the thr_arg pointer is 0 pass the address
        // of each thread's id as the argument to thr_proc
        void* const arg = thr_arg ? thr_arg [0] : (void*)(thr_id);

        void* const thr_result = thr_proc (arg);

        if (thr_arg)
            thr_arg [0] = thr_result;

        return 0;
    }
#endif   // !_RWSTD_REENTRANT

    bool delete_ids = false;

    if (0 == thr_id) {
        // save thread idsso that they (and no other threads)
        // can be joined later
        if (sizeof id_buf / sizeof *id_buf < nthrs) {
            delete_ids = true;
            thr_id     = new rw_thread_t [nthrs];
        }
        else
            thr_id = id_buf;
    }

    // create a pool of threads storing their id's
    // in consecutive elements of the thr_id array
    for (size_t i = 0; i != nthrs; ++i) {

        // when the thr_arg pointer is 0 pass the address
        // of each thread's id as the argument to thr_proc
        void* const next_arg = thr_arg ? thr_arg [i] : (void*)(thr_id + i);

        if (rw_thread_create (thr_id + i, 0, thr_proc, next_arg)) {
            if (delete_ids)
                delete[] thr_id;

            return int (i + 1);
        }
    }

    // invoking the function with a 0 thr_id pointer
    // is a request to join all threads in the pool
    if (join) {
        for (size_t i = 0; i != nthrs; ++i) {

            // avoid advancing through the thr_arg array
            // when it's 0 (and have rw_thread_join() simply
            // ignore the thread's return value)
            void** next_arg = thr_arg ? thr_arg + i : 0;

            rw_thread_join (thr_id [i], next_arg);
        }

        if (delete_ids)
            delete[] thr_id;
    }

    return 0;
}

}   // extern "C"
