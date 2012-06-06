/***************************************************************************
 *
 * 18.setjmp.cpp - test exercising [support.runtime], header <setjmp.h>
 *
 * $Id: 18.setjmp.cpp 593007 2007-11-08 04:30:45Z sebor $
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
 * Copyright 2007 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#include <setjmp.h>
#include <driver.h>

/**************************************************************************/

jmp_buf env;

void test_longjmp (int arg)
{
    longjmp (env, arg);

    rw_assert (0, 0, __LINE__,
               "call to longjmp(..., %d) returned", arg);
}

/**************************************************************************/

static int
run_test (int, char**)
{
#ifdef longjmp

    // longjmp must not be #defined as a macro
    rw_assert (0, 0, __LINE__, "longjmp #defined as a macro");

#endif   // longjmp

#ifdef jmp_buf

    //jmp_buf must not be #defined as a macro
    rw_assert (0, 0, __LINE__, "jmp_buf #defined as a macro");

#endif   // jmp_buf

#ifndef setjmp

    //setjmp must be #defined as a macro
    rw_assert (0, 0, __LINE__, "macro setjmp not #defined");

#endif   // setjmp

    // verify that setjmp works

    // using volatile to work around a gcc optimizer bug:
    //   http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34024
    volatile int arg = 1;

    int result;

    result = setjmp (env);

    if (0 == result) {

        test_longjmp (arg);

        rw_assert (0, 0, __LINE__,
                   "call to longjmp(..., %d) returned", arg);
    }
    else {
        rw_assert (arg == result, 0, __LINE__,
                   "longjmp(..., %d) returned %d from setjmp()",
                   arg, result);

        // repeat a couple more times
        if (arg < 3)
            test_longjmp (++arg);
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "support.runtime",
                    "header <setjmp.h>",
                    run_test,
                    "",
                    (void*)0);
}
