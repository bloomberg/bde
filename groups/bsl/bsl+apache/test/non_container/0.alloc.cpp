/************************************************************************
 *
 * 0.alloc.cpp - test exercising the rw_alloc() and rw_free() functions
 *
 * $Id: 0.alloc.cpp 590052 2007-10-30 12:44:14Z faridz $
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

#include <setjmp.h>      // for setjmp, longjmp
#include <signal.h>      // for signal
#include <stddef.h>      // for size_t

#include <rw_alloc.h>    // for rw_alloc, rw_free
#include <rw_printf.h>   // for rw_printf
#include <driver.h>

static jmp_buf mark;

extern "C" {

static void
sig_handler (int)
{
    longjmp (mark, -1);
}

}

// on Cygwin use setjmp
#if !defined (__CYGWIN__) && defined (sigsetjmp)
#define SETJMP(env) sigsetjmp (env, 1)
#else
#define SETJMP(env) setjmp (env)
#endif

#define BEGIN_TEST_OVERRUN()        \
    (signal (SIGSEGV, sig_handler), \
    (0 == SETJMP (mark)))

struct TestCase
{
    size_t       size_;
    int          index_;
    int          prot_;
    const char*  file_;
    int          line_;
};

void test (const TestCase& tcase)
{
    if (void* buf = rw_alloc (tcase.size_, tcase.prot_)) {

        char* str = _RWSTD_STATIC_CAST (char*, buf);

        bool protbelow = 0 != (tcase.prot_ & RW_PROT_BELOW);

        bool isguardpage = (protbelow && 0 > tcase.index_)
            || (!protbelow && tcase.index_ >= int (tcase.size_));

        bool canread = (tcase.prot_ & RW_PROT_READ) && !isguardpage;

        bool canwrite = (tcase.prot_ & RW_PROT_WRITE) && !isguardpage;

        char c = 'a';

        // check read access only if:
        // tcase.prot_ & RW_PROT_READ != 0
        // tcase.prot_ == RW_PROT_NONE
        // tcase.prot_ & RW_PROT_WRITE != 0 and the
        // accessed memory is placed in the guard page
        if (tcase.prot_ & RW_PROT_READ || !canwrite) {
            if (BEGIN_TEST_OVERRUN ()) {
                c = str[tcase.index_];
                rw_assert (canread, tcase.file_, tcase.line_,
                    "expected no read access, got read access");
            } else
                rw_assert (!canread, tcase.file_, tcase.line_,
                "expected read access, got no read access");
        }

        if (BEGIN_TEST_OVERRUN ()) {
            str[tcase.index_] = c;
            rw_assert (canwrite, tcase.file_, tcase.line_,
                       "expected no write access, got write access");
        } else
            rw_assert (!canwrite, tcase.file_, tcase.line_,
                       "expected write access, got no write access");

        rw_free (buf);
    }
}

static int
run_test (int, char**)
{
    const size_t BUF_SIZE = 10;

    static const TestCase tcases[] = {

#ifdef TEST
#undef TEST
#endif
#define TEST(size, index, prot) \
    { size, index, prot, __FILE__, __LINE__ }

        TEST(BUF_SIZE, -1, RW_PROT_NONE),
        TEST(BUF_SIZE, -1, RW_PROT_READ),
        TEST(BUF_SIZE, -1, RW_PROT_WRITE),
        TEST(BUF_SIZE, -1, RW_PROT_RDWR),

        TEST(BUF_SIZE, -1, RW_PROT_NONE | RW_PROT_BELOW),
        TEST(BUF_SIZE, -1, RW_PROT_READ | RW_PROT_BELOW),
        TEST(BUF_SIZE, -1, RW_PROT_WRITE | RW_PROT_BELOW),
        TEST(BUF_SIZE, -1, RW_PROT_RDWR | RW_PROT_BELOW),

        TEST(BUF_SIZE, 0, RW_PROT_NONE),
        TEST(BUF_SIZE, 0, RW_PROT_READ),
        TEST(BUF_SIZE, 0, RW_PROT_WRITE),
        TEST(BUF_SIZE, 0, RW_PROT_RDWR),

        TEST(BUF_SIZE, 0, RW_PROT_NONE | RW_PROT_BELOW),
        TEST(BUF_SIZE, 0, RW_PROT_READ | RW_PROT_BELOW),
        TEST(BUF_SIZE, 0, RW_PROT_WRITE | RW_PROT_BELOW),
        TEST(BUF_SIZE, 0, RW_PROT_RDWR | RW_PROT_BELOW),

        TEST(BUF_SIZE, BUF_SIZE - 1, RW_PROT_NONE),
        TEST(BUF_SIZE, BUF_SIZE - 1, RW_PROT_READ),
        TEST(BUF_SIZE, BUF_SIZE - 1, RW_PROT_WRITE),
        TEST(BUF_SIZE, BUF_SIZE - 1, RW_PROT_RDWR),

        TEST(BUF_SIZE, BUF_SIZE - 1, RW_PROT_NONE | RW_PROT_BELOW),
        TEST(BUF_SIZE, BUF_SIZE - 1, RW_PROT_READ | RW_PROT_BELOW),
        TEST(BUF_SIZE, BUF_SIZE - 1, RW_PROT_WRITE | RW_PROT_BELOW),
        TEST(BUF_SIZE, BUF_SIZE - 1, RW_PROT_RDWR | RW_PROT_BELOW),

        TEST(BUF_SIZE, BUF_SIZE, RW_PROT_NONE),
        TEST(BUF_SIZE, BUF_SIZE, RW_PROT_READ),
        TEST(BUF_SIZE, BUF_SIZE, RW_PROT_WRITE),
        TEST(BUF_SIZE, BUF_SIZE, RW_PROT_RDWR),

        TEST(BUF_SIZE, BUF_SIZE, RW_PROT_NONE | RW_PROT_BELOW),
        TEST(BUF_SIZE, BUF_SIZE, RW_PROT_READ | RW_PROT_BELOW),
        TEST(BUF_SIZE, BUF_SIZE, RW_PROT_WRITE | RW_PROT_BELOW),
        TEST(BUF_SIZE, BUF_SIZE, RW_PROT_RDWR | RW_PROT_BELOW)
    };

    for (size_t i = 0; i < sizeof (tcases) / sizeof (tcases[0]); ++i)
        test (tcases [i]);

    return 0;
}

/***********************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
        "",
        0,
        run_test,
        "",
        (void*)0 /* sentinel */);
}
