/************************************************************************
 *
 * 0.outputiter.cpp - test exercising the OutputIter class template
 *
 * $Id: 0.outputiter.cpp 550991 2007-06-26 23:58:07Z sebor $
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
 * Copyright 1994-2005 Rogue Wave Software.
 *
 **************************************************************************/

#include <setjmp.h>
#include <signal.h>
#include <stdio.h>

#ifdef _MSC_VER
#include <crtdbg.h>     // for _CrtSetReportMode()
#endif

#include <alg_test.h>   // for OutputIter
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test(), ...

/***********************************************************************/

int exit_status /* = 0 */;

extern "C" {

int line;
int fail;

jmp_buf env;

void handle_ABRT (int)
{
    fail = 0;

    longjmp (env, 1);
}

}

#define FAIL(code)                                      \
    fail = line = __LINE__;                             \
    signal (SIGABRT, handle_ABRT);                      \
    if (0 == setjmp (env)) {                            \
        code;                                           \
        exit_status = 1;                                \
        rw_assert (0, 0, line, "expected assertion");   \
    }                                                   \
    else                                                \
        (void)0

#define PASS(code)                                      \
    fail = -1; line = __LINE__;                         \
    signal (SIGABRT, handle_ABRT);                      \
    if (0 == setjmp (env))                              \
        code;                                           \
    else if (fail != line) {                            \
        exit_status = 1;                                \
        rw_assert (0, 0, line, "unexpected assertion"); \
    } (void)0

/***********************************************************************/

static void
test_1 ()
{
    UserClass *x = UserClass::from_char ("abcdef");
    UserClass *y = UserClass::from_char ("ABCDEF");

    // use `out' as the last argument to make_iter() (as opposed
    // to, say, OutputIter<UserClass>()) since Output Iterators
    // are not required to be copy-constructible
    OutputIter<UserClass> out = make_iter (x + 0, x + 0, x + 6, out);

    PASS (*out++ = y [0]);
    PASS (*out++ = y [1]);
    PASS (*out++ = y [2]);
    PASS (*out++ = y [3]);
    PASS (*out++ = y [4]);
    PASS (*out++ = y [5]);

    delete[] x;
    delete[] y;
}

/***********************************************************************/

static void
test_2 ()
{
    UserClass *x = UserClass::from_char ("abcdef");
    UserClass *y = UserClass::from_char ("ABCDEF");

    OutputIter<UserClass> out = make_iter (x + 0, x + 0, x + 6, out);

    PASS (*out = y [0]);
    PASS (++out);
    PASS (out = y [1]);
    PASS (++out);
    PASS (*out = y [2]);
    PASS (++out);
    PASS (out = y [3]);
    PASS (++out);
    PASS (*out = y [4]);
    PASS (++out);
    PASS (out = y [5]);
    PASS (++out);

    delete[] x;
    delete[] y;
}

/***********************************************************************/

static void
test_3 ()
{
    UserClass *x = UserClass::from_char ("abcdef");
    UserClass *y = UserClass::from_char ("ABCDEF");

    OutputIter<UserClass> out = make_iter (x + 0, x + 0, x + 6, out);

    PASS (*out = y [0]);
    PASS (out++);
    PASS (out = y [1]);
    PASS (out++);
    PASS (*out = y [2]);
    PASS (out++);
    PASS (out = y [3]);
    PASS (out++);
    PASS (*out = y [4]);
    PASS (out++);
    PASS (out = y [5]);
    PASS (out++);

    delete[] x;
    delete[] y;
}

/***********************************************************************/

static void
test_4 ()
{
    UserClass *x = UserClass::from_char ("abcdef");
    UserClass *y = UserClass::from_char ("ABCDEF");

    OutputIter<UserClass> out = make_iter (x + 0, x + 0, x + 6, out);

    PASS (*out++ = y [0]);
    PASS (*out = y [1]);
    PASS (out++);

    PASS (out++ = y [2]);
    PASS (out = y [3]);
    PASS (out++);

    PASS (*out++ = y [4]);
    PASS (*out = y [5]);
    PASS (out++);

    delete[] x;
    delete[] y;
}

/***********************************************************************/

static void
test_5 ()
{
    UserClass *x = UserClass::from_char ("abcdef");
    UserClass *y = UserClass::from_char ("ABCDEF");

    OutputIter<UserClass> out = make_iter (x + 0, x + 0, x + 6, out);

    PASS (*out = y [0]);
    PASS (++out);
    PASS (*out++ = y [1]);

    PASS (out = y [2]);
    PASS (++out);
    PASS (out++ = y [3]);

    PASS (*out = y [4]);
    PASS (++out);
    PASS (*out++ = y [5]);

    delete[] x;
    delete[] y;
}

/***********************************************************************/

static void
test_6 ()
{
    UserClass *x = UserClass::from_char ("abcdef");
    UserClass *y = UserClass::from_char ("ABCDEF");

    OutputIter<UserClass> out = make_iter (x + 0, x + 0, x + 6, out);

    PASS (*out   = y [0]);
    PASS (*++out = y [1]);
    PASS (++out  = y [2]);
    PASS (*++out = y [3]);
    PASS (++out  = y [4]);
    PASS (*++out = y [5]);
    PASS (out++);

    delete[] x;
    delete[] y;
}

/***********************************************************************/

static void
test_7 ()
{
    UserClass x;

    OutputIter<UserClass> out = make_iter (&x, &x, &x, out);

    FAIL (++out);
    FAIL (out++);
    FAIL (*out);
    FAIL (*out++);
    FAIL (*out   = x);
    FAIL (*out++ = x);
    FAIL (*++out = x);
    FAIL (out    = x);
    FAIL (out++  = x);
    FAIL (++out  = x);

    out = make_iter (&x, &x, &x + 1, out);
}

/***********************************************************************/

static void
test_8 ()
{
    UserClass *x = UserClass::from_char ("abcdef");
    UserClass *y = UserClass::from_char ("ABCDEF");

    OutputIter<UserClass> out = make_iter (x + 0, x + 0, x + 6, out);

    PASS (*out++ = y [0]);
    PASS (*out++ = y [1]);
    PASS (*out++ = y [2]);
    PASS (*out++ = y [3]);
    PASS (*out++ = y [4]);
    PASS (*out++ = y [5]);
    FAIL (out++);

    delete[] x;
    delete[] y;
}

/***********************************************************************/

static void
test_9 ()
{
    UserClass *x = UserClass::from_char ("abcdef");
    UserClass *y = UserClass::from_char ("ABCDEF");

    OutputIter<UserClass> out = make_iter (x + 0, x + 0, x + 6, out);

    PASS (*out++ = y [0]);
    PASS (out++);
    FAIL (*out++ = y [1]);

    delete[] x;
    delete[] y;
}

/***********************************************************************/

static int
run_test (int, char*[])
{
#ifndef NDEBUG

    // silence stderr output from invocations of the RW_ASSERT() macro
    // that are expected to fail by design (i.e., that's what this test
    // exercises)
    fclose (stderr);

    test_1 ();
    test_2 ();
    test_3 ();
    test_4 ();
    test_5 ();
    test_6 ();
    test_7 ();
    test_8 ();
    test_9 ();

#else   // if defined (NDEBUG)

    rw_assert (0, 0, __LINE__, "macro NDEBUG #defined, cannot test");

#endif   // NDEBUG

    return exit_status;

}

/***********************************************************************/

int main (int argc, char *argv[])
{
#ifdef _MSC_VER
    // disable GUI window from abort()
    _CrtSetReportMode (_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif

    return rw_test (argc, argv, __FILE__,
                    0 /* no clause */,
                    0 /* no comment */, run_test,
                    0 /* co command line options */);
}
