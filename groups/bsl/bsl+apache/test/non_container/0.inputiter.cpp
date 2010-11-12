/************************************************************************
 *
 * 0.inputiter.cpp - test exercising the InputIter class template
 *
 * $Id: 0.inputiter.cpp 550991 2007-06-26 23:58:07Z sebor $
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

#include <alg_test.h>   // for InputIter
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


typedef InputIter<UserClass> Iterator;

// FIXME: implement an easy way to print out an array of UserClass
#define X2STR(ign1, ign2)   "***NOT IMPLEMENTED ***"

/***********************************************************************/

static void
test_0 ()
{
    UserClass *x = UserClass::from_char ("abc");

    Iterator end0 = make_iter (x + 3, x + 0, x + 3, end0);
    Iterator end1 = make_iter (x + 3, x + 0, x + 3, end1);

    bool equal;

    // end iterator must compare equal to itself
    PASS (equal = end0 == end0);

    rw_assert (equal, 0, __LINE__,
               "InputIter<UserClass> end iterator unexpectedly "
               "not equal to self: %p == %p", end0.cur_, end0.cur_);

    PASS (equal = end0 != end0);

    rw_assert (!equal, 0, __LINE__,
               "InputIter<UserClass> end iterator unexpectedly "
               "not equal to self: %p == %p", end0.cur_, end0.cur_);

    // end iterator must compare equal to another
    PASS (equal = end0 == end1);

    rw_assert (equal, 0, __LINE__,
               "InputIter<UserClass> end iterator unexpectedly "
               "not equal to another: %p == %p", end0.cur_, end1.cur_);

    PASS (equal = end0 != end1);

    rw_assert (!equal, 0, __LINE__,
               "InputIter<UserClass> end iterator unexpectedly "
               "not equal to another: %p == %p", end0.cur_, end1.cur_);

    // cannot increment the end iterator
    FAIL (++end0);
    FAIL (end0++);
    FAIL (++end1);
    FAIL (end1++);

    // cannot dereference the end iterator
    FAIL (x [0] = *end0);
    FAIL (x [0] = *end1);

    FAIL (equal = int ('a') == end0->data_.val_);
    FAIL (equal = int ('a') == end1->data_.val_);

    delete[] x;
}

/***********************************************************************/

static void
test_1 ()
{
    UserClass *x = UserClass::from_char ("abcdef");
    UserClass *y = UserClass::from_char ("ABCDEF");

    const Iterator end = make_iter (x + 6, x + 0, x + 6, end);
          Iterator it  = make_iter (x + 0, x + 0, x + 6, it);

    bool equal;

    // non-end iterator must compare unequal to the end iterator
    PASS (equal = it == end);

    rw_assert (!equal, 0, __LINE__,
               "InputIter<UserClass> unexpectedly equal to end: "
               "%p == %p", it.cur_, end.cur_);
    
    PASS (y [0] = *it);
    PASS (++it);
    PASS (equal = it == end);

    rw_assert (!equal, 0, __LINE__,
               "InputIter<UserClass> unexpectedly equal to end: "
               "%p == %p", it.cur_, end.cur_);

    PASS (y [1] = *it);
    PASS (++it);

    PASS (equal = it == it);

    rw_assert (equal, 0, __LINE__,
               "InputIter<UserClass> unexpectedly equal to self: "
               "%p == %p", it.cur_, it.cur_);

    PASS (y [2] = *it);
    PASS (++it);
    PASS (y [3] = *it);
    PASS (++it);
    PASS (y [4] = *it);
    PASS (++it);
    PASS (y [5] = *it);
    PASS (++it);

    PASS (equal = it == end);

    rw_assert (equal, 0, __LINE__,
               "InputIter<UserClass> unexpectedly not qual to end: "
               "%p != %p (diff = %d)",
               it.cur_, end.cur_, end.cur_ - it.cur_);

    rw_assert (0 == UserClass::compare (x, y, 6), 0, __LINE__,
               "InputIter<UserClass> data mismatch: %s != %s",
               X2STR (x, 6), X2STR (y, 6));
    
    delete[] x;
    delete[] y;
}

/***********************************************************************/

static void
test_2 ()
{
    UserClass *x = UserClass::from_char ("abcdef");
    UserClass *y = UserClass::from_char ("ABCDEF");

    const Iterator end = make_iter (x + 6, x + 0, x + 6, end);
          Iterator it  = make_iter (x + 0, x + 0, x + 6, it);

    PASS (y [0] = *it);
    PASS (y [1] = *++it);
    PASS (y [2] = *it++);   // it++ is the same as ++it
    PASS (y [3] = *it++);
    PASS (++it);
    PASS (y [4] = *it);
    PASS (y [5] = *it++);

    bool equal;

    PASS (equal = it == end);

    rw_assert (!equal, 0, __LINE__,
               "InputIter<UserClass> unexpectedly equal to end: "
               "%p != %p (diff = %d)",
               it.cur_, end.cur_, end.cur_ - it.cur_);

    PASS (++it);

    PASS (equal = it == end);

    rw_assert (equal, 0, __LINE__,
               "InputIter<UserClass> unexpectedly not equal to end: "
               "%p != %p (diff = %d)",
               it.cur_, end.cur_, end.cur_ - it.cur_);

    rw_assert (0 == UserClass::compare (x, y, 6), 0, __LINE__,
               "InputIter<UserClass> data mismatch: %s != %s",
               X2STR (x, 6), X2STR (y, 6));
    
    delete[] x;
    delete[] y;
}

/***********************************************************************/

static void
test_3 ()
{
    UserClass *x = UserClass::from_char ("abcdef");
    UserClass *y = UserClass::from_char ("ABCDEF");

    const Iterator end = make_iter (x + 6, x + 6, x + 6, end);
          Iterator it0 = make_iter (x + 0, x + 0, x + 6, it0);
          Iterator it1 = it0;

    bool equal;
    
    PASS (equal = it0 == it1);

    rw_assert (equal, 0, __LINE__,
               "InputIter<UserClass> unexpectedly not equal: "
               "%p != %p (diff = %d)",
               it0.cur_, it1.cur_, it0.cur_ - it1.cur_);

    PASS (equal = it0 == end);

    rw_assert (!equal, 0, __LINE__,
               "InputIter<UserClass> unexpectedly equal to end: "
               "%p == %p", it1.cur_, end.cur_);

    PASS (y [0] = *it0);
    PASS (y [1] = *it1);

    rw_assert (y [0].data_.val_ == y [1].data_.val_, 0, __LINE__,
               "two copies of InputIter<UserClass> unexpectedly yield "
               "different values: %d != %d",
               y [0].data_.val_, y [1].data_.val_);

    PASS (it0++);
    FAIL (it1++);        // can't pass through the same iterator twice
    FAIL (it0 == it1);   // it1 not in the domain of operator==()

    PASS (equal = it0 == end);

    rw_assert (!equal, 0, __LINE__,
               "InputIter<UserClass> unexpectedly equal to end: "
               "%p == %p", it1.cur_, end.cur_);

    PASS (it1 = it0);

    PASS (equal = it0 == it1);

    rw_assert (equal, 0, __LINE__,
               "InputIter<UserClass> unexpectedly not equal: "
               "%p == %p", it0.cur_, it1.cur_);

    PASS (y [0] = *it0);
    PASS (y [1] = *it1);

    rw_assert (y [0].data_.val_ == y [1].data_.val_, 0, __LINE__,
               "two copies of InputIter<UserClass> unexpectedly yield "
               "different values: %d != %d",
               y [0].data_.val_, y [1].data_.val_);

    rw_assert (y [0].data_.val_ == 'b', 0, __LINE__,
               "InputIter<UserClass>::operator*() == %d, got %d",
               y [0].data_.val_, 'b');

    PASS (it1++);
    FAIL (it0++);        // can't pass through the same iterator twice
    FAIL (it0 == it1);   // it0 not in the domain of operator==()

    PASS (equal = it1 == end);

    rw_assert (!equal, 0, __LINE__,
               "InputIter<UserClass> unexpectedly equal to end: "
               "%p == %p", it1.cur_, end.cur_);

    FAIL (x [0] = *it0);   // cannot dereference

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

    test_0 ();
    test_1 ();
    test_2 ();
    test_3 ();

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
