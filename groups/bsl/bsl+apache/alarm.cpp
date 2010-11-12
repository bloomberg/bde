/************************************************************************
 *
 * alarm.cpp - definitions of testsuite helpers
 *
 * $Id: alarm.cpp 590052 2007-10-30 12:44:14Z faridz $
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

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

// including <time.h> first works around a SunPro/SunOS bug (PR #26255)
#include <time.h>     // for time_t

#include <assert.h>   // for assert()
#include <stdio.h>    // for sprintf()

#include <rw_alarm.h>    // for rw_alarm()
#include <rw_printf.h>   // for rw_fprintf()


// avoid re-declaring the constants below extern exported to prevent
// MSVC error: object must have external linkage in order to be
// exported/imported

/* extern _TEST_EXPORT */ rw_signal_handler_t* const
rw_sig_dfl = (rw_signal_handler_t*)-1;

/* extern _TEST_EXPORT */ rw_signal_handler_t* const
rw_sig_ign = (rw_signal_handler_t*)-2;

/* extern _TEST_EXPORT */ rw_signal_handler_t* const
rw_sig_hold = (rw_signal_handler_t*)-3;

/* extern _TEST_EXPORT */ rw_signal_handler_t* const
rw_sig_restore = (rw_signal_handler_t*)-4;


// may point to a user-defined handler for the alarm
static rw_signal_handler_t*
_rw_alarm_handler;


#if defined (_WIN32) || defined (_WIN64)

#include <windows.h>

// CygWin doesn't seem to define alarm()...

// thread procedure
static DWORD WINAPI
_rw_alarm_proc (LPVOID arg /* seconds */)
{
    Sleep (1000U * (ULONG)arg);

    rw_fprintf (rw_stderr, "%s:%d alarm expired\n", __FILE__, __LINE__);

    if (_rw_alarm_handler)
        _rw_alarm_handler (0);
    else
        abort ();   // no SIGALRM on Win32

    return 0;
}


// rough equivalent of POSIX alarm
_TEST_EXPORT
unsigned int
rw_alarm (unsigned int nsec, rw_signal_handler_t* handler /* = 0 */)
{
    static HANDLE thread    = 0;   // thread handle (one alarm per process)
    static unsigned pending = 0;   // previous alarm() argument
    static time_t t0        = 0;   // start of previous alarm()
    time_t unslept          = 0;   // seconds until previous alarm expires

    if (thread) {
        // previous alarm is still pending, cancel it
        unslept = pending - (time (0) - t0);
        TerminateThread (thread, 0);
        CloseHandle (thread);
        thread = 0;
    }

    pending = nsec;

    if (nsec) {
        time (&t0);   // keep track of when countdown started

        if (handler) {
            // take care not to overwrite any previously set handler
            _rw_alarm_handler = handler;
        }

        DWORD tid;    // dummy (not used, required on Win95)
        thread = CreateThread (0, 0, _rw_alarm_proc, (LPVOID)nsec, 0, &tid);
        // thread handle will leak unless alarm (0) is called
    }

    return unsigned (unslept);
}


#else   // if !defined (_WIN{32,64}

#  include <signal.h>   // for SIGALRM, signal()
#  include <unistd.h>   // for alarm(), write()

   // define macros in case they aren't #defined by
   // the system headers e.g., when using pure libc headers
#  ifndef SIGALRM
#    define SIGALRM   14   /* e.g., Solaris */
#  endif

#  ifndef SIG_DFL
#    define SIG_IGN (rw_signal_handler_t*)0
#  endif   // SIG_DFL

#  ifndef SIG_IGN
#    define SIG_IGN (rw_signal_handler_t*)1
#  endif   // SIG_IGN

#  ifndef SIG_HOLD
#    define SIG_HOLD (rw_signal_handler_t*)2
#  endif   // SIG_HOLD


extern "C" {

static void
_rw_handle_sigalrm (int signo)
{
    assert (SIGALRM == signo);

    char buffer [1024];

    // fprintf() is not async-signal safe...
    const int len =
        sprintf (buffer, "%s:%d: alarm expired\n", __FILE__, __LINE__);

    // ...use write() instead
    write (STDERR_FILENO, buffer, size_t (len));

    if (_rw_alarm_handler)
        _rw_alarm_handler (signo);
}

}   // extern "C"


_TEST_EXPORT
unsigned
rw_alarm (unsigned int nsec, rw_signal_handler_t* handler /* = 0 */)
{
    static rw_signal_handler_t* saved_handler = 0;
    static unsigned             saved_nsec    = 0;

    if (rw_sig_hold == handler) {
        // hold and save the current SIGALRM handler
        // and just retrieve the current alarm
        saved_handler = signal (SIGALRM, (rw_signal_handler_t*)SIG_HOLD);
        saved_nsec    = alarm (0);

        alarm (saved_nsec);

        return saved_nsec;
    }

    if (rw_sig_restore == handler) {
        // restore the previously saved SIGALRM handler
        // and set a new alarm to go off approximately
        // after (saved_nsec - nsec) seconds

        if (saved_handler) {
            signal (SIGALRM, saved_handler);

            if (nsec < saved_nsec)
                nsec = saved_nsec - nsec;
            else
                nsec = 1;

            alarm (nsec);

            // return the new timeout
            return nsec;
        }

        // return 0 to indicate that no alarm was set
        return 0;
    }

    if (rw_sig_dfl == handler) {
        signal (SIGALRM, (rw_signal_handler_t*)SIG_DFL);
    }
    else if (rw_sig_ign == handler) {
        signal (SIGALRM, (rw_signal_handler_t*)SIG_IGN);
    }
    else if (handler) {
        // take care not to overwrite any previously set handler
        _rw_alarm_handler = handler;
        signal (SIGALRM, _rw_handle_sigalrm);
    }

    return alarm (nsec);
}


#endif   // _WIN{32,64}
