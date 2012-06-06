/************************************************************************
*
* 0.process.cpp - test exercising the rw_process_create(),
*                 rw_process_kill() and rw_waitpid() functions
*
* $Id: 0.process.cpp 580483 2007-09-28 20:55:52Z sebor $
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

// tell Compaq C++ we need POSIX errno constants that are otherwise
// guarded (not defined) in the compiler's pure C++ libc headers
#undef __PURE_CNAME
#include <errno.h>          // for ENOENT, ECHILD, ESRCH, errno
#include <string.h>         // for strcmp()

#include <rw_process.h>     // for rw_process_create(), rw_waitpid()
#include <driver.h>         // for rw_test()

static int _rw_child = 0;
static int _rw_timeout = 5;

static char arg1[] = "--child=1";
static char arg2[] = "--no-stdout";

static char* args [] = { 0, arg1, arg2, 0 };
static const int nargs = sizeof (args) / sizeof (*args) - 1;

static rw_pid_t _rw_pid = -1;

static int join_test (rw_pid_t pid, bool should_hang)
{
    int result = 0;
    const rw_pid_t ret = rw_waitpid (pid, &result, _rw_timeout);

    rw_assert (-1 != ret, __FILE__, __LINE__,
               "rw_waitpid() failed, errno = %{#m}");

    if (-1 == ret)
        return 1;

    if (0 == ret) {
        // set to ignore rw_error diagnostic
        rw_enable (rw_error, false);

        // time_out elapsed, kill the process
        if (1 == rw_process_kill (pid)) {
            // the process not yet terminated
            // wait for process termination and remove the zombie process
            rw_waitpid (pid, 0);
        }

        // enable rw_error diagnostic
        rw_enable (rw_error);

        rw_assert (should_hang, __FILE__, __LINE__,
                   "The child process unexpectedly deadlocked");

        return should_hang ? 0 : 1;
    }

    rw_assert (!should_hang, __FILE__, __LINE__,
               "Expected the deadlocked process, but process exited "
               "with code: %d",
               result);

    if (!should_hang)
        rw_assert (0 == result, __FILE__, __LINE__,
                   "Process exit code: expected 0, got %d", result);

    return result;
}

static int test_process_create1 ()
{
    rw_info (0, 0, 0,
             "Exercising the rw_process_create "
             "(const char*, char* const []) overload");

    const rw_pid_t pid = rw_process_create (args [0], args);

    rw_assert (-1 != pid, __FILE__, __LINE__,
               "rw_process_create() failed, errno = %{#m}");

    if (-1 == pid)
        return 1;

    // save the pid for test exercising rw_waitpid() fail
    if (-1 == _rw_pid)
        _rw_pid = pid;

    return join_test (pid, false);
}

static int test_process_create2 ()
{
    rw_info (0, 0, 0,
             "Exercising the rw_process_create (const char*, ...) overload");

    const rw_pid_t pid = rw_process_create ("\"%s\" %s %s",
                                            args[0], args[1], args[2]);

    rw_assert (-1 != pid, __FILE__, __LINE__,
               "rw_process_create() failed, errno = %{#m}");

    if (-1 == pid)
        return 1;

    // save the pid for test exercising rw_waitpid() fail
    if (-1 == _rw_pid)
        _rw_pid = pid;

    return join_test (pid, false);
}

static int test_process_deadlocked ()
{
    rw_info (0, 0, 0,
             "Exercising the rw_waitpid() with timeout and deadlocked process");

    const rw_pid_t pid = rw_process_create ("\"%s\" --child=2 %s",
                                            args[0], args[2]);

    rw_assert (-1 != pid, __FILE__, __LINE__,
               "rw_process_create() failed, errno = %{#m}");

    return -1 == pid ? 1 : join_test (pid, true);
}

static int test_process_create_fail ()
{
    rw_info (0, 0, 0,
             "Exercising the rw_process_create() behavior "
             "when invalid path specified");

    // set to ignore rw_error diagnostic
    rw_enable (rw_error, false);

    const rw_pid_t pid = rw_process_create ("/\\/\\/\\", args);

    // enable rw_error diagnostic
    rw_enable (rw_error);

    rw_assert (-1 == pid, __FILE__, __LINE__,
               "rw_process_create returns %ld, expected -1",
               long (pid));

    if (-1 != pid) {
        rw_waitpid (pid, 0);
        return 1;
    }

    rw_assert (ENOENT == errno, __FILE__, __LINE__,
               "errno: expected ENOENT, got %{#m}");

    return ENOENT == errno ? 0 : 1;
}

static int test_waitpid_fail ()
{
    if (-1 == _rw_pid) {
        rw_info (0, 0, 0,
                 "The test, exercising the rw_waitpid() behavior "
                 "when invalid pid specified is disabled");

        return 0;
    }

    rw_info (0, 0, 0,
             "Exercising the rw_waitpid() behavior "
             "when invalid pid specified");

    // set to ignore rw_error diagnostic
    rw_enable (rw_error, false);

    const rw_pid_t pid = rw_waitpid (_rw_pid, 0);

    // enable rw_error diagnostic
    rw_enable (rw_error);

    rw_assert (-1 == pid, __FILE__, __LINE__,
               "rw_waitpid returns %ld, expected -1",
               long (pid));

    if (-1 != pid)
        return 1;

    rw_assert (ECHILD == errno, __FILE__, __LINE__,
               "errno: expected ECHILD, got %{#m}");

    return ECHILD == errno ? 0 : 1;
}

static int test_process_kill_fail ()
{
    if (-1 == _rw_pid) {
        rw_info (0, 0, 0,
                 "The test, exercising the rw_process_kill() behavior "
                 "when invalid pid specified is disabled");

        return 0;
    }

    rw_info (0, 0, 0,
             "Exercising the rw_process_kill() behavior "
             "when invalid pid specified");

    // set to ignore rw_error diagnostic
    rw_enable (rw_error, false);

    const int res = rw_process_kill (_rw_pid);

    // enable rw_error diagnostic
    rw_enable (rw_error);

    rw_assert (-1 == res, __FILE__, __LINE__,
               "rw_process_kill returns %ld, expected -1",
               long (res));

    if (-1 != res)
        return 1;

    rw_assert (ESRCH == errno, __FILE__, __LINE__,
               "errno: expected ESRCH, got %{#m}");

    return ESRCH == errno ? 0 : 1;
}

static int
run_test (int argc, char** argv)
{
    if (_rw_child) {

        rw_info (0, 0, 0,
            "The child process: _rw_child = %i", _rw_child);

        if (2 == _rw_child) {
            // simulate the deadlock
            while (true) ;
        }

        // compare number of parameters with expected
        if (nargs != argc)
            return nargs;

        // compare the parameters with expected
        for (int i = 1; i < argc; ++i) {
            if (0 != strcmp (argv [i], args [i]))
                return i;
        }

        return 0;
    }

    args [0] = argv [0];

    int fails = 0;

    if (test_process_create1 ())
        ++fails;

    if (test_process_create2 ())
        ++fails;

    if (test_process_deadlocked ())
        ++fails;

    if (test_process_create_fail ())
        ++fails;

    if (test_waitpid_fail ())
        ++fails;

    if (test_process_kill_fail ())
        ++fails;

    return fails;
}

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "0.process",
                    "", run_test,
                    "|-child#0 "
                    "|-timeout#",
                    &_rw_child,
                    &_rw_timeout,
                    0 /*sentinel*/);
}
