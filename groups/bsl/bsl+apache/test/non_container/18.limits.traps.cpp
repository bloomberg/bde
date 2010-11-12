/***************************************************************************
 *
 * 18.limits_traps.cpp - test exercising std::numeric_limits::traps
 *
 * $Id: 18.limits.traps.cpp 515262 2007-03-06 19:28:51Z sebor $
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

#include <limits>

#include <csignal>    // for SIGFPE, signal

#include <any.h>      // for rw_any_t
#include <cmdopt.h>   // for rw_enabled()
#include <driver.h>   // for rw_test(), ...

/**************************************************************************/

#ifdef _RWSTD_OS_LINUX

   // use siglongjmp() and sigsetjmp() on Linux to avoid
   // http://sourceware.org/bugzilla/show_bug.cgi?id=2351
#  include <setjmp.h>    // for siglongjmp, sigsetjmp

jmp_buf jmp_env;

extern "C" {

void handle_fpe (int)
{
    siglongjmp (jmp_env, 1);
}

}   // extern "C"

#  define RW_SIGSETJMP(env, signo)   sigsetjmp (env, signo)
#else   // if !defined (_RWSTD_OS_LINUX)

#  include <csetjmp>    // for longjmp, setjmp

std::jmp_buf jmp_env;

extern "C" {

void handle_fpe (int)
{
    std::longjmp (jmp_env, 1);
}

}   // extern "C"
#  define RW_SIGSETJMP(env, ignore)   setjmp (env)
#endif   // _RWSTD_OS_LINUX


/**************************************************************************/

#ifdef _MSC_VER
   // silence useless MSVC warnings:
   // 4800: 'int' : forcing value to bool 'true' or 'false'
   // 4804: '/' : unsafe use of type 'bool' in operation
#  pragma warning (disable: 4800 4804)

   // use Structured Exception Handling to detect arithmetic exceptions
#  define TRY           __try
#  define EXCEPT(arg)   __except (arg)
#else
#  define TRY              if (1)
#  define EXCEPT(ignore)   else if (0)
#endif   // _MSC_VER


template <class numT>
inline void
try_trap (const volatile numT &one, const volatile numT &zero,
           numT &result, bool &trapped)
{
    TRY {
        result = one / zero;
    }
    EXCEPT (1) {
        // Windows SEH hackery
        trapped = true;
    }
}


template <class numT>
numT test_traps (numT, int lineno, bool)
{
    static const char* const tname = rw_any_t (numT ()).type_name ();

    if (!rw_enabled (tname)) {
        rw_note (0, 0, 0, "numeric_limits<%s>::traps test disabled", tname);
        return numT ();
    }

    const bool traps = std::numeric_limits<numT>::traps;

    rw_info (0, 0, 0, "std::numeric_limits<%s>::traps = %b", tname, traps);

#ifdef SIGFPE
    std::signal (SIGFPE, handle_fpe);
#else   // if !defined (SIGFPE)
    if (!rw_warn (!traps, 0, lineno,
                  "SIGFPE not #defined and numeric_limits<%s>::traps == %b, "
                  "cannot test", tname, traps)) {
        return numT ();
    }
#endif   // SIGFPE

    numT result = numT ();

    // set the environment
    const int jumped = RW_SIGSETJMP (jmp_env, SIGFPE);

    volatile numT zero = numT (jumped);
    volatile numT one  = numT (1);

    bool trapped = false;

    if (jumped) {
        // setjmp() call above returned from the SIGFPE handler
        // as a result of a floating point exception triggered
        // by the division by zero in the else block below
        result = zero / one;

        trapped = true;
    }
    else {
        // setjmp() call above returned after setting up the jump
        // environment; see of division by zero traps (if so, it
        // will generate a SIGFPE which will be caught by the
        // signal hanlder above and execution will resume by
        // returning from setjmp() above again, but this time
        // with a non-zero value
        try_trap (one, zero, result, trapped);
    }

    rw_assert (trapped == traps, 0, lineno,
               "numeric_limits<%s>::traps == %b, got %b",
               tname,  trapped, traps);

    return result;
}

/**************************************************************************/

static int
run_test (int, char*[])
{
#define TEST(T, floating)   test_traps ((T)0, __LINE__, floating)

#ifndef _RWSTD_NO_NATIVE_BOOL
    TEST (bool, false);
#endif   // _RWSTD_NO_NATIVE_BOOL

    TEST (char, false);
    TEST (signed char, false);
    TEST (unsigned char, false);

    TEST (short, false);
    TEST (unsigned short, false);
    TEST (int, false);
    TEST (unsigned int, false);
    TEST (long, false);
    TEST (unsigned long, false);

#ifndef _RWSTD_NO_LONG_LONG
    TEST (_RWSTD_LONG_LONG, false);
    TEST (unsigned _RWSTD_LONG_LONG, false);
#endif   // _RWSTD_NO_LONG_LONG

#ifndef _RWSTD_NO_NATIVE_WCHAR_T
    TEST (wchar_t, false);
#endif   // _RWSTD_NO_NATIVE_WCHAR_T

    TEST (float, true);
    TEST (double, true);

#ifndef _RWSTD_NO_LONG_DOUBLE
    TEST (long double, true);
#endif   // _RWSTD_NO_LONG_DOUBLE

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.numeric.limits.members",
                    "traps data member",
                    run_test,
                    0, 0);
}
