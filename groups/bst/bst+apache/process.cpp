/************************************************************************
 *
 * process.cpp - definitions of testsuite process helpers
 *
 * $Id: process.cpp 648752 2008-04-16 17:01:56Z faridz $
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

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <rw_process.h>

#include <ctype.h>        // for isspace()
#include <errno.h>        // for EACCES, errno
#include <signal.h>       // for SIGXXX, kill()
#include <stddef.h>       // for size_t
#include <stdarg.h>       // for va_copy, va_list, ...
#include <stdlib.h>       // for free(), exit()
#include <string.h>       // for strchr()

#include <sys/types.h>    // for pid_t

#include <driver.h>       // for rw_note(), ...
#include <rw_printf.h>    // for rw_fprintf()

#ifdef __CYGWIN__
// use the Windows API on Cygwin
#  define _WIN32
#endif

#ifndef E2BIG
#  define E2BIG   7   /* AIX, HP-UX, Linux, Solaris */
#endif

#ifndef EINTR
#  define EINTR   4
#endif

#ifndef SIGCHLD
#  if defined (_RWSTD_OS_AIX) || defined (_RWSTD_OS_OSF)
     // AIX, Tru64
#    define SIGCHLD   20
#  elif defined (_RWSTD_OS_LINUX)
#    define SIGCHLD   17
#  else
     // (System V-based) HP-UX, IRIX, and Solaris
#    define SIGCHLD   18
#  endif
#endif

// all known Unices
#ifndef SIGALRM
#  define SIGALRM  14
#endif

#ifndef SIGHUP
#  define SIGHUP   1
#endif

#ifndef SIGQUIT
#  define SIGQUIT  3
#endif

#ifndef SIGKILL
#  define SIGKILL  9
#endif


#ifdef _RWSTD_EDG_ECCP

extern "C" {

int kill (pid_t, int);

}   // extern "C"

#endif   // _RWSTD_EDG_ECCP

/**************************************************************************/

_TEST_EXPORT int
rw_vasnprintf (char**, size_t*, const char*, va_list);

/**************************************************************************/

#ifdef _WIN32

#  include <windows.h>      // for WaitForSingleObject(), ...

static int
_rw_map_errno (DWORD err)
{
    if (ERROR_WRITE_PROTECT <= err && ERROR_SHARING_BUFFER_EXCEEDED >= err)
        return EACCES;

    if (   ERROR_INVALID_STARTING_CODESEG <= err
        && ERROR_INFLOOP_IN_RELOC_CHAIN >= err) {

        return ENOEXEC;
    }

    switch (err)
    {
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
    case ERROR_INVALID_DRIVE:
    case ERROR_NO_MORE_FILES:
    case ERROR_BAD_NETPATH:
    case ERROR_BAD_NET_NAME:
    case ERROR_INVALID_NAME:
    case ERROR_BAD_PATHNAME:
    case ERROR_FILENAME_EXCED_RANGE:

        return ENOENT;

    case ERROR_TOO_MANY_OPEN_FILES:
        return EMFILE;

    case ERROR_ACCESS_DENIED:
    case ERROR_CURRENT_DIRECTORY:
    case ERROR_NETWORK_ACCESS_DENIED:
    case ERROR_CANNOT_MAKE:
    case ERROR_FAIL_I24:
    case ERROR_DRIVE_LOCKED:
    case ERROR_SEEK_ON_DEVICE:
    case ERROR_NOT_LOCKED:
    case ERROR_LOCK_FAILED:
        return EACCES;

    case ERROR_INVALID_HANDLE:
    case ERROR_INVALID_TARGET_HANDLE:
    case ERROR_DIRECT_ACCESS_HANDLE:
        return EBADF;

    case ERROR_ARENA_TRASHED:
    case ERROR_NOT_ENOUGH_MEMORY:
    case ERROR_INVALID_BLOCK:
    case ERROR_NOT_ENOUGH_QUOTA:
        return ENOMEM;

    case ERROR_BAD_ENVIRONMENT:
        return E2BIG;

    case ERROR_BAD_FORMAT:
        return ENOEXEC;

    case ERROR_NOT_SAME_DEVICE:
        return EXDEV;

    case ERROR_FILE_EXISTS:
        return EEXIST;

    case ERROR_NO_PROC_SLOTS:
    case ERROR_MAX_THRDS_REACHED:
    case ERROR_NESTING_NOT_ALLOWED:
        return EAGAIN;

    case ERROR_BROKEN_PIPE:
        return EPIPE;

    case ERROR_DISK_FULL:
        return ENOSPC;

    case ERROR_WAIT_NO_CHILDREN:
    case ERROR_CHILD_NOT_COMPLETE:
        return ECHILD;

    case ERROR_DIR_NOT_EMPTY:
        return ENOTEMPTY;

    case ERROR_ALREADY_EXISTS:
        return EEXIST;
    }

    return EINVAL;
}

#else   // #if !defined (_WIN32)

#  include <sys/types.h>
#  include <sys/wait.h>   // for waitpid()
#  include <unistd.h>     // for fork(), execv(), access(), sleep()
#  include <setjmp.h>     // for setjmp(), longjmp()
#  include <signal.h>     // for signal()
#  include <time.h>       // for time()

/**************************************************************************/

// splits command line to the array of parameters
// note: modifies the cmd string
// returns the number of parameters in cmd
// if argv != 0 fills argv up to size elements
static size_t
_rw_split_cmd (char* cmd, char** argv, size_t size)
{
    RW_ASSERT (0 != cmd);

    size_t ret = 0;

    for (char* end = cmd + strlen (cmd); cmd != end; /*do nothing*/) {
        // skip the leading spaces
        while (isspace (*cmd))
            ++cmd;

        if (end == cmd)
            break;

        if (argv && ret < size)
            argv [ret] = cmd;

        ++ret;

        if ('\'' == *cmd || '\"' == *cmd) {
            char* const cmd1 = cmd + 1;
            // search the closing quote
            char* const pos = strchr (cmd1, *cmd);
            if (pos) {
                // found, remove the quotes
                // remove the opening quote
                memmove (cmd, cmd1, pos - cmd1);
                // remove the closing quote
                cmd = pos - 1;
                memmove (cmd, pos + 1, end - pos);
                end -= 2;
            }
            else {
                // not found
                break;
            }
        }

        // search the space
        while (*cmd && !isspace (*cmd))
            ++cmd;

        if (cmd != end)
            // found, replace to '\0'
            *cmd++ = '\0';
    }

    return ret;
}

#endif   // #if defined (_WIN32)

/**************************************************************************/

static int
_rw_vsystem (const char *cmd, va_list va)
{
    RW_ASSERT (0 != cmd);

    char *buf = 0;

    rw_vasnprintf (&buf, 0, cmd, va);

    rw_note (0, "file:" __FILE__, __LINE__, "executing \"%s\"", buf);

    // avoid using const in order to prevent gcc warning on Linux
    // issued for WIFSIGNALED() et al: cast from `const int*' to
    // `int*' discards qualifiers from pointer target type:
    // see http://sourceware.org/bugzilla/show_bug.cgi?id=1392
    /* const */ int ret = system (buf);

    if (ret) {

#ifndef _WIN32

        if (-1 == ret) {
            // system() failed, e.g., because fork() failed
            rw_error (0, __FILE__, __LINE__,
                      "system (\"%s\") failed: errno = %{#m} (%{m})",
                      buf);
        }
        else if (WIFSIGNALED (ret)) {
            // command exited with a signal
            const int signo = WTERMSIG (ret);

            rw_error (0, __FILE__, __LINE__,
                      "the command \"%s\" exited with signal %d (%{K})",
                      buf, signo, signo);
        }
        else {
            // command exited with a non-zero status
            const int status = WEXITSTATUS (ret);

            rw_error (0, __FILE__, __LINE__,
                      "the command \"%s\" exited with status %d",
                      buf, status);
        }
#else   // if defined (_WIN32)

        // FIXME: make this more descriptive
        rw_error (0, __FILE__, __LINE__,
                  "the command \"%s\" failed with code %d",
                  buf, ret);

#endif   // _WIN32

    }

    free (buf);

    return ret;
}

/**************************************************************************/

_TEST_EXPORT int
rw_system (const char *cmd, ...)
{
    va_list va;
    va_start (va, cmd);

    const int ret = _rw_vsystem (cmd, va);

    va_end (va);
    return ret;
}

/**************************************************************************/

static rw_pid_t
_rw_vprocess_create (const char* cmd, va_list va)
{
    RW_ASSERT (0 != cmd);

    char *buf = 0;

    rw_vasnprintf (&buf, 0, cmd, va);

    rw_pid_t ret = -1;

#ifdef _WIN32

    STARTUPINFO si = { sizeof (si) };
    PROCESS_INFORMATION pi;

    if (CreateProcess (0, buf, 0, 0, FALSE,
                       CREATE_NEW_PROCESS_GROUP, 0, 0, &si, &pi)) {

        CloseHandle (pi.hThread);
        ret = rw_pid_t (pi.hProcess);
    }
    else {
        const DWORD err = GetLastError ();

        rw_error (0, __FILE__, __LINE__,
                  "CreateProcess () failed: GetLastError() = %zu",
                  size_t (err));

        errno = _rw_map_errno (err);
    }

#else   // #if !defined (_WIN32)

    const size_t MAX_PARAMS = 63;
    char* argv [MAX_PARAMS + 1] = { 0 };

    size_t argc = _rw_split_cmd (buf, argv, MAX_PARAMS);

    if (0 < argc && MAX_PARAMS >= argc)
        ret = rw_process_create (argv [0], argv);
    else
        errno = E2BIG;

#endif  // _WIN32

    free (buf);

    return ret;
}

/**************************************************************************/

_TEST_EXPORT rw_pid_t
rw_process_create (const char* cmd, ...)
{
    va_list va;
    va_start (va, cmd);

    const rw_pid_t ret = _rw_vprocess_create (cmd, va);

    va_end (va);
    return ret;
}

/**************************************************************************/

_TEST_EXPORT rw_pid_t
rw_process_create (const char* path, char* const argv[])
{
#if defined (_WIN32)

    return rw_process_create ("\"%s\" %{ As}", path, argv + 1);

#else   // #if !defined (_WIN32)

    if (0 == access (path, X_OK)) {

        const rw_pid_t child_pid = fork ();

        if (0 == child_pid) {
            // the child process
            execvp (path, argv);

            // the execvp returns only if an error occurs
            rw_fprintf (rw_stderr, "%s:%d execvp (%#s, %{As}) failed: "
                        "errno = %{#m} (%{m})\n",
                        __FILE__, __LINE__, path, argv);

            exit (1);
        }
        else if (-1 == child_pid)
            rw_error (0, __FILE__, __LINE__,
                      "fork () failed: errno = %{#m} (%{m})");

        return child_pid;
    }
    else
        rw_error (0, __FILE__, __LINE__,
                  "access (%#s, X_OK) failed: errno = %{#m} (%{m})",
                  path);

    return -1;

#endif  // #if defined (_WIN32)
}

/**************************************************************************/

#if defined (_WIN32)

_TEST_EXPORT rw_pid_t
rw_waitpid (rw_pid_t pid, int* result, int timeout/* = -1*/)
{
    /* Explicitly check for process_id being -1 or -2. In Windows NT,
    * -1 is a handle on the current process, -2 is a handle on the
    * current thread, and it is perfectly legal to to wait (forever)
    * on either */
    if (-1 == pid || -2 == pid) {
        errno = ECHILD;
        return -1;
    }

    const HANDLE handle = HANDLE (pid);

    const DWORD milliseconds =
        0 > timeout ? INFINITE : DWORD (timeout * 1000);

    const DWORD res = WaitForSingleObject (handle, milliseconds);

    DWORD err = ERROR_SUCCESS;

    if (WAIT_OBJECT_0 == res) {

        DWORD dwExitCode;
        if (GetExitCodeProcess (handle, &dwExitCode)) {

            CloseHandle (handle);

            if (dwExitCode)
                rw_error (0, __FILE__, __LINE__,
                          "the process (pid=%{P}) exited with return code %d",
                          pid, int (dwExitCode));

            if (result)
                *result = int (dwExitCode);

            return pid;
        }

        err = GetLastError ();
        rw_error (0, __FILE__, __LINE__,
                  "GetExitCodeProcess (%{P}, %#p) failed: GetLastError() = %zu",
                  pid, &dwExitCode, size_t (err));
    }
    else if (WAIT_FAILED == res) {
        err = GetLastError ();
        rw_error (0, __FILE__, __LINE__,
                  "WaitForSingleObject (%{P}, %{?}INFINITE%{:}%zu%{;}) failed: "
                  "GetLastError() = %zu",
                  pid, INFINITE == milliseconds,
                  size_t (milliseconds), size_t (err));
    }
    else {
        // time-out elapsed
        RW_ASSERT (WAIT_TIMEOUT == res);
        return 0;
    }

    if (ERROR_INVALID_HANDLE == err)
        errno = ECHILD;
    else
        errno = _rw_map_errno (err);

    return -1;
}

#else   // #if !defined (_WIN32)

extern "C" {

static int alarm_timeout;

static void handle_alarm_signal (int) {
    alarm_timeout = 1;
}

typedef void (*signal_handler_t) (int);

}

_TEST_EXPORT rw_pid_t
rw_waitpid (rw_pid_t pid, int* presult, int timeout/* = -1*/)
{
#ifdef _RWSTD_EDG_ECCP
#  define _RWSTD_NO_SIGACTION
#endif

#ifndef _RWSTD_NO_SIGACTION
    struct sigaction prev_alarm_action;
#else
    signal_handler_t prev_alarm_handler = 0;
#endif
    int              prev_alarm_timeout = 0;

    alarm_timeout = 0;
    if (0 < timeout) {

#ifndef _RWSTD_NO_SIGACTION
        struct sigaction alarm_action;
        memset (&alarm_action, 0, sizeof alarm_action);

        const signal_handler_t handler_fun = handle_alarm_signal;
        memcpy (&alarm_action.sa_handler, &handler_fun,
                sizeof alarm_action.sa_handler);

        sigaction (SIGALRM, &alarm_action, &prev_alarm_action);
#else
        prev_alarm_handler = signal (SIGALRM, handle_alarm_signal);
#endif
        prev_alarm_timeout = alarm (timeout);

    }

    int result = 0;
    if (!presult)
        presult = &result;

    const time_t start = time(0);

    int status   = 0;
    rw_pid_t ret = 0;
    do {

        ret = waitpid (pid, &status, 0);

        if (-1 == ret) {

            if (EINTR == errno && alarm_timeout) {

                // we are expected to return 0 on timeout
                ret = 0;
            }
            else if (EINTR == errno) {

                rw_warn (0, __FILE__, __LINE__,
                         "waitpid (%{P}, %#p, 0) interrupted: "
                         "errno = %{#m} (%{m})",
                         pid, &status);

                continue; // try again
            }
            else {

                rw_error (0, __FILE__, __LINE__,
                          "waitpid (%{P}, %#p, 0) failed: "
                          "errno = %{#m} (%{m})",
                          pid, &status);
            }

        }
        else if (ret == pid) {

            if (WIFSIGNALED (status)) {
                // process exited with a signal
                const int signo = WTERMSIG (status);

                rw_error (0, __FILE__, __LINE__,
                          "the process (pid=%{P}) exited with signal %d (%{K})",
                          pid, signo, signo);

                *presult = signo;
            }
            else if (WIFEXITED (status)) {
                // process exited with a status
                const int retcode = WEXITSTATUS (status);

                if (retcode)
                    rw_error (0, __FILE__, __LINE__,
                              "the process (pid=%{P}) exited with return code %d",
                              pid, retcode);

                *presult = retcode;
            }
            else {
                *presult = -1;
            }
        }
        else {
            *presult = -1;
        }

    } while(false);

    if (0 < timeout) {

        if (prev_alarm_timeout) {
            const int delta = time(0) - start;

            if (delta < prev_alarm_timeout)
                prev_alarm_timeout -= delta;
            else
                prev_alarm_timeout = 1;
        }

        alarm (prev_alarm_timeout);

#ifndef _RWSTD_NO_SIGACTION
        sigaction (SIGALRM, &prev_alarm_action, 0);
#else
        signal (SIGALRM, prev_alarm_handler);
#endif

    }

    return ret;
}

#endif  // #if defined (_WIN32)


_TEST_EXPORT int
rw_process_kill (rw_pid_t pid, int signo)
{
    // timeout for rw_wait_pid
    const int timeout = 1;

#if defined (_WIN32)

    // send signal
    if (!TerminateProcess (HANDLE (pid), DWORD (signo))) {

        const DWORD err = GetLastError ();
        rw_error (0, __FILE__, __LINE__,
                  "TerminateProcess (%{P}, %i) failed: GetLastError() = %zu",
                  pid, signo, size_t (err));

        if (ERROR_INVALID_HANDLE == err)
            errno = ESRCH;
        else if (ERROR_ACCESS_DENIED == err)
            errno = EPERM;
        else
            errno = _rw_map_errno (err);

        return -1;
    }

    // wait for process termination
    rw_pid_t res = rw_waitpid (pid, 0, timeout);
    if (pid == res)
        return 0;

    if (-1 == res)
        rw_error (0, __FILE__, __LINE__,
                  "rw_waitpid (%{P}, 0, %i) failed: errno = %{#m} (%{m})",
                  pid, timeout);

    return 1;

#else   // #if !defined (_WIN32)

    static const int signals_ [] = {
        SIGHUP, SIGINT, SIGQUIT, SIGTERM, SIGKILL
    };

    const int* const signals = (-1 == signo) ? signals_ : &signo;

    const unsigned sigcount =
        (-1 == signo) ? sizeof (signals_) / sizeof (*signals_) : 1;

    int ret = -1;

    for (unsigned i = 0; i < sigcount; ++i) {

        // send signal
        ret = kill (pid, signals [i]);

        if (-1 == ret) {
            rw_error (0, __FILE__, __LINE__,
                      "kill (%{P}, %{K}) failed: errno = %{#m} (%{m})",
                      pid, signals [i]);

            continue;
        }

        // wait for process termination
        rw_pid_t res = rw_waitpid (pid, 0, timeout);
        if (pid == res)
            return 0;

        if (-1 == res)
            rw_error (0, __FILE__, __LINE__,
                      "rw_waitpid (%{P}, 0, %i) failed: errno = %{#m} (%{m})",
                      pid, timeout);

        ret = 1;
    }

    return ret;

#endif  // #if defined (_WIN32)
}
