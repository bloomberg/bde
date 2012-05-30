/***************************************************************************
 *
 * 0.new.cpp - test exercising replacement operator new and delete
 *
 * $Id: 0.new.cpp 641117 2008-03-26 02:32:31Z sebor $
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#include <new>        // for bad_alloc

#include <cstddef>    // for size_t

#include <setjmp.h>   // for longjmp(), setjmp()
#include <signal.h>   // for SIGABRT, signal()

#include <rw_new.h>
#include <driver.h>

/**************************************************************************/

// program exit status
int exit_status /* = 0 */;

extern "C" {

volatile int line;   // currently executed line
volatile int fail;   // non-zero when line failed

jmp_buf env;

static void
handle_ABRT (int)
{
    fail = 0;

    longjmp (env, 1);
}

}

#define FAIL(code)                                              \
    fail = line = __LINE__;                                     \
    signal (SIGABRT, handle_ABRT);                              \
    if (0 == setjmp (env)) {                                    \
        code;                                                   \
        exit_status = 1;                                        \
        rw_assert (0, __FILE__, line, "expected assertion");    \
    }                                                           \
    else                                                        \
        (void)0

#define PASS(code)                                              \
    fail = -1; line = __LINE__;                                 \
    signal (SIGABRT, handle_ABRT);                              \
    if (0 == setjmp (env)) {                                    \
        code;                                                   \
    }                                                           \
    else if (fail != line) {                                    \
        exit_status = 1;                                        \
        rw_assert (0, __FILE__, line, "unexpected assertion");  \
    } (void)0

/**************************************************************************/

void test_new_delete ()
{
    rw_info (0, __FILE__, __LINE__,
             "exercising successful allocation and deallocation");

    {
        void *p = 0;
        PASS (p = operator new (0));
        rw_assert (p != 0, __FILE__, __LINE__, "operator new(0) != 0");
        operator delete (p);
    }

    {
        void *p = 0;
        PASS (p = operator new (1));
        rw_assert (p != 0, __FILE__, __LINE__, "operator new(1) != 0");
        operator delete (p);
    }

    {
        void *p = 0;
        PASS (p = operator new (2));
        rw_assert (p != 0, __FILE__, __LINE__, "operator new(2) != 0");
        operator delete (p);
    }

    {
        void *p = 0;
        PASS (p = operator new (1024));
        rw_assert (p != 0, __FILE__, __LINE__, "operator new(1024) != 0");
        operator delete (p);
    }
}

/**************************************************************************/

#define CATCH(code)                                     \
    try { code; fail = 1; }                             \
    catch (_RWSTD_BAD_ALLOC) { fail = 0; }              \
    catch (...) { fail = 2; }                           \
    rw_assert (!fail, __FILE__, __LINE__,               \
               "%s", 1 == fail ? "failed to throw" :    \
               "threw an unknown exception")

#define NOTHROW(code)                           \
    try { fail = 0; code; }                     \
    catch (...) { fail = 1; }                   \
    rw_assert (!fail, __FILE__, __LINE__,       \
               "unexpected exception")

void test_bad_alloc ()
{
    rw_info (0, __FILE__, __LINE__,
             "exercising the ability of ordinary "
             "operator new to throw std::bad_alloc");

    rwt_free_store* const pst = rwt_get_free_store (0);

    *pst->throw_at_blocks_ [0] = pst->blocks_ [0];

    CATCH (operator new (0));
    CATCH (operator new (1));
    CATCH (operator new (2));
    CATCH (operator new (1024));

    {
        void *p = 0;
        NOTHROW (p = operator new[](0));
        operator delete[](p);
    }

    {
        void *p = 0;
        NOTHROW (p = operator new[](1));
        operator delete[](p);
    }

    {
        void *p = 0;
        NOTHROW (p = operator new[](2));
        operator delete[](p);
    }

    {
        void *p = 0;
        NOTHROW (p = operator new[](1024));
        operator delete[](p);
    }

    rw_info (0, __FILE__, __LINE__,
             "exercising the ability of the array form "
             "of operator new to throw std::bad_alloc");

    *pst->throw_at_blocks_ [0] = std::size_t (-1);
    *pst->throw_at_blocks_ [1] = pst->blocks_ [1];

    CATCH (operator new[](0));
    CATCH (operator new[](1));
    CATCH (operator new[](2));
    CATCH (operator new[](1024));

    {
        void *p = 0;
        NOTHROW (p = operator new (0));
        rw_assert (p != 0, __FILE__, __LINE__, "operator new[](0) != 0");
        operator delete (p);
    }

    {
        void *p = 0;
        NOTHROW (p = operator new (1));
        rw_assert (p != 0, __FILE__, __LINE__, "operator new[](1) != 0");
        operator delete (p);
    }

    {
        void *p = 0;
        NOTHROW (p = operator new (32));
        rw_assert (p != 0, __FILE__, __LINE__, "operator new[](32) != 0");
        operator delete (p);
    }

    {
        void *p = 0;
        NOTHROW (p = operator new (4096));
        rw_assert (p != 0, __FILE__, __LINE__, "operator new[](4096) != 0");
        operator delete (p);
    }

    *pst->throw_at_blocks_ [1] = std::size_t (-1);
}

/**************************************************************************/

void test_mismatch ()
{
    rw_info (0, __FILE__, __LINE__,
             "exercising the ability to detect "
             "allocation/deallocation mismatches");

    {
        // detect allocations by operator new() deallocated
        // using (the array form of) operator delete[]
        void *p = 0;
        PASS (p = operator new (0));
        FAIL (operator delete[](p));
        PASS (operator delete (p));
    }

    {
        void *p = 0;
        PASS (p = operator new (1));
        FAIL (operator delete[](p));
        PASS (operator delete (p));
    }

    {
        void *p = 0;
        PASS (p = operator new[](33));
        FAIL (operator delete (p));
        PASS (operator delete[] (p));
    }
}

/**************************************************************************/

void test_bad_delete ()
{
    rw_info (0, __FILE__, __LINE__,
             "exercising the ability to detect "
             "deletion of unallocated storage");

    {
        char *p = 0;
        PASS (p = new char);

        FAIL (delete (p - 1));
        FAIL (delete (p + 1));

        PASS (delete (p));
    }

    {
        char *p = 0;
        PASS (p = new char [4]);

        FAIL (delete (p - 1));
        FAIL (delete (p + 1));
        FAIL (delete (p + 2));
        FAIL (delete (p + 3));
        FAIL (delete (p + 4));

        FAIL (delete[] (p - 1));
        FAIL (delete[] (p + 1));
        FAIL (delete[] (p + 2));
        FAIL (delete[] (p + 3));
        FAIL (delete[] (p + 4));

        PASS (delete[] p);
    }
}

/**************************************************************************/

void test_double_delete ()
{
    rw_info (0, __FILE__, __LINE__,
             "exercising the ability to detect double deletion");

    {
        char *p = 0;
        PASS (p = new char);

        PASS (delete (p));
        FAIL (delete (p));
    }

    {
        char *p = 0;
        PASS (p = new char [32]);

        PASS (delete[] p);
        FAIL (delete[] p);
        FAIL (delete p);
    }
}

/**************************************************************************/

void test_corruption ()
{
    rw_info (0, __FILE__, __LINE__,
             "exercising the ability to detect memory corruption");

    // corrupt (and restore) memory past the end of the allocated block
    for (std::size_t i = 1; i != 8; ++i) {
        char *p = 0;
        PASS (p = new char);

        // save the value of the byte past the end of the block
        // and temporarily overwrite it with another value
        const char save = p [i];
        p [i] = ~p [i];

        // expect operator delete to diagnose the corruption
        // and call abort() without actually freeing the block
        FAIL (delete p);

        // restore the corrupted byte to its original value
        p [i] = save;

        // expect operator delete not to complain
        PASS (delete p);
    }
}

/**************************************************************************/

#define LEAK(code, bytes, blks)                                         \
    do {                                                                \
        /* establish a checkpoint for memory leaks */                   \
        rwt_check_leaks (0, 0);                                         \
                                                                        \
        code;                                                           \
                                                                        \
        /* find memory leaks since the last checkpoint */               \
        std::size_t nbytes;                                             \
        const std::size_t nblocks = rwt_check_leaks (&nbytes, 0);       \
                                                                        \
        rw_assert (blks == nblocks && bytes == nbytes,                  \
                   __FILE__, __LINE__,                                  \
                   "failed to detect a leak of %d bytes in "            \
                   "%d blocks: got %zu bytes in %zu blocks",            \
                   bytes, blks, nbytes, nblocks);                       \
    } while (0)


void test_leaks ()
{
    rw_info (0, __FILE__, __LINE__,
             "exercising the ability to detect memory leaks");

    {
        void *p = 0;
        LEAK (p = operator new (0), 0, 1);
        PASS (operator delete (p));
    }

    {
        void *p = 0;
        LEAK (p = operator new (1), 1, 1);
        PASS (operator delete (p));
    }

    {
        void *p = 0;
        LEAK (p = operator new (1234), 1234, 1);
        PASS (operator delete (p));
    }

    {
        void *p0 = 0;
        void *p1 = 0;
        LEAK (p0 = operator new (32);
              p1 = operator new (64), 96, 2);
        PASS (operator delete (p0));
        PASS (operator delete (p1));
    }

    {
        void *p = 0;
        LEAK (p = operator new[] (12345), 12345, 1);
        PASS (operator delete[] (p));
    }
}

/**************************************************************************/

void test_stress ()
{
    rw_info (0, __FILE__, __LINE__,
             "stress-testing replacement operators new and delete");

    rwt_free_store* const pst = rwt_get_free_store (0);

    std::size_t nblocks = pst->blocks_ [0] + pst->blocks_ [1];
    std::size_t nbytes  = pst->bytes_ [0] + pst->bytes_ [1];

    void* ptrs [1000];

    const std::size_t N = sizeof ptrs / sizeof *ptrs;

    for (std::size_t i = 0; i != N; ++i) {
        if (i % 2) {
            PASS (ptrs [i] = operator new[](i));
        }
        else {
            PASS (ptrs [i] = operator new (i));
        }
    }

    for (std::size_t i = 0; i < N; ++i) {

        const std::size_t j = (i * (i + 17)) % N;

        if (j % 2) {
            PASS (operator delete[](ptrs [j]));
        }
        else {
            PASS (operator delete (ptrs [j]));
        }

        ptrs [j] = 0;
    }

    for (std::size_t i = 0; i < N; ++i) {
        if (i % 2) {
            PASS (operator delete[](ptrs [i]));
        }
        else {
            PASS (operator delete (ptrs [i]));
        }
    }

    nblocks = pst->blocks_ [0] + pst->blocks_ [1] - nblocks;
    nbytes  = pst->bytes_ [0] + pst->bytes_ [1] - nbytes;

    rw_assert (0 == nblocks && 0 == nbytes, __FILE__, __LINE__,
               "false leaks detected: %zu bytes in %zu blocks",
               nbytes, nblocks);
}

/**************************************************************************/

static int rw_opt_no_new_delete;      // for --no-new-delete
static int rw_opt_no_bad_alloc;       // for --no-bad_alloc
static int rw_opt_no_mismatch;        // for --no-mismatch
static int rw_opt_no_bad_delete;      // for --no-bad-delete
static int rw_opt_no_double_delete;   // for --no-double-delete
static int rw_opt_no_corruption;      // for --no-corruption
static int rw_opt_no_leaks;           // for --no-leaks
static int rw_opt_no_stress;          // for --no-stress

int run_test (int, char**)
{
#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    // disable diagnostics issued by the replacement operator new
    // and delete defined by the test driver in response to deliberate
    // errors caused by this test
    rw_enable (rw_error, false);

#  define TEST(name)                                            \
    if (rw_opt_no_ ## name)                                     \
        rw_note (0, 0, __LINE__, "%s test disabled", #name);    \
    else                                                        \
        test_ ## name ()

    TEST (bad_alloc);
    TEST (mismatch);
    TEST (double_delete);
    TEST (bad_delete);
    TEST (corruption);
    TEST (leaks);
    TEST (stress);

#else    // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    rw_note (0, 0, __LINE__, "Test disabled");

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    return 0;
}

/**************************************************************************/

int main (int argc, char** argv)
{
    return rw_test (argc, argv, __FILE__,
                    0 /* no clause */,
                    0 /* no comment */,
                    run_test,
                    "|-no-new-delete# "
                    "|-no-bad_alloc# "
                    "|-no-mismatch# "
                    "|-no-bad-delete# "
                    "|-no-double-delete# "
                    "|-no-corruption# "
                    "|-no-leaks# "
                    "|-no-stress-test#",
                    &rw_opt_no_new_delete,
                    &rw_opt_no_bad_alloc,
                    &rw_opt_no_mismatch,
                    &rw_opt_no_bad_delete,
                    &rw_opt_no_double_delete,
                    &rw_opt_no_corruption,
                    &rw_opt_no_leaks,
                    &rw_opt_no_stress);
}
