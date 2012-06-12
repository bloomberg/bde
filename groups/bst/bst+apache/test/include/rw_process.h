/************************************************************************
 *
 * rw_process.h - declarations of testsuite process helpers
 *
 * $Id: rw_process.h 472469 2006-11-08 12:27:17Z faridz $
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
 **************************************************************************/

#ifndef RW_PROCESS_H_INCLUDED
#define RW_PROCESS_H_INCLUDED


#include <testdefs.h>

typedef _RWSTD_SSIZE_T rw_pid_t;

_TEST_EXPORT int
rw_system (const char*, ...);

// returns pid of the created process or -1 on error
// (in which case errno is set to an appropriate value)
_TEST_EXPORT rw_pid_t
rw_process_create (const char*, ...);

// note: argv[0] should be equal to path
// returns pid of the created process or -1 on error
// (in which case errno is set to an appropriate value)
_TEST_EXPORT rw_pid_t
rw_process_create (const char* /*path*/, char* const /*argv*/[]);

// result is a pointer to a buffer where the result code
//   of the specified process will be stored, or NULL
//
// the function suspends execution of the current process
// until a child has exited or specified timeout is reached
//
// returns:
//   pid of the specified process if it has exited
//   0 when process still active
//   -1 on error (in which case errno is set to an appropriate value)
//
// timeout is timeout interval in seconds.
//   if timeout > 0 the function returns if the interval elapses
//   if timeout == 0 the function returns immediately
//   if timeout < 0 the function's time-out interval never elapses
//
// errors:
//   ECHILD: no specified process exists
_TEST_EXPORT rw_pid_t
rw_waitpid (rw_pid_t /*pid*/, int* /*result*/, int /*timeout*/ = -1);

// returns:
//  0 when process terminated successfully
//  1 when signal was sent to the child process, but child process
//  not terminated within 1 second interval
//  -1 on error (in which case errno is set to an appropriate value)
// errors:
//   ESRCH: the pid does not exist
//   EPERM: the calling process does not have permission
//          to terminate the specified process
_TEST_EXPORT int
rw_process_kill (rw_pid_t, int = -1);

#endif   // RW_PROCESS_H_INCLUDED
