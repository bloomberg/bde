/************************************************************************
 *
 * rwthread.h - declarations for testsuite thread helpers
 *
 * $Id: rw_thread.h 580483 2007-09-28 20:55:52Z sebor $
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
 * Copyright 2005-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef RW_RWTHREAD_H_INCLUDED
#define RW_RWTHREAD_H_INCLUDED

#include <testdefs.h>


extern "C" {

struct rw_thread_attr_t;

struct rw_thread_t
{
    long  threadno;   // 0-based unique thread number
    long  id;         // thread id
    void* handle;     // thread handle
};


// same as POSIX pthread_create()
_TEST_EXPORT int
rw_thread_create (rw_thread_t*,
                  rw_thread_attr_t*,
                  void* (*)(void*),
                  void*);


// same as POSIX pthread_join()
_TEST_EXPORT int
rw_thread_join (rw_thread_t, void**);


// create a pool of nthreads, passing each a successive element
// of argarray as argument (if non-null) and filling the tidarray
// array with their id's; if (tidarray == 0), waits for all
// threads to join and fills the aragarray with the result
// returned from each thread
// if (nthreads == SIZE_MAX), sets nthreads to the positive result
// of rw_get_processors() plus 1, or to 2 otherwise
// returns 0 on success, or a non-zero value indicating the thread
// number that failed to create on error
_TEST_EXPORT int
rw_thread_pool (rw_thread_t*      /* tidarray */,
                _RWSTD_SIZE_T     /* nthreads */,
                rw_thread_attr_t* /* attr */,
                void* (*)(void*)  /* thr_proc */,
                void**            /* argarray */);

// returns the number of logical processors/cores on the system,
// or -1 on error
_TEST_EXPORT int
rw_get_cpus ();


}   // extern "C"

#endif   // RW_RWTHREAD_H_INCLUDED
