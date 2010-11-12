/************************************************************************
 *
 * 0.cmdopts.cpp - test exercising the rw_runopts() and rw_setopts()
 *                 utility functions
 *
 * $Id: 0.cmdopts.cpp 590052 2007-10-30 12:44:14Z faridz $
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

#include <cmdopt.h>

#include <stdio.h>
#include <string.h>

// global buffer containing the names of all callbacks (along with
// their arguments) called in response to each invocation to rw_runopts()
static char argstr [4096];

// the maximum number of callbacks to be invoked by a single call
// to rw_runopts()
#define MAXCALLBACKS 32

// the values to be returned by each callback
static int retvals [MAXCALLBACKS];

// the cumulative number of callback invocations
static size_t ncalls;

// the exit status of the whole test
static int exit_status;

// the current line number
static int current_line;

/**************************************************************************/

// prints its arguments in a human-readable form to buf
static int
pargs (char *buf, const char *funcname, int argc, char *argv [])
{
    _RWSTD_ASSERT (0 != buf);

    char* next = buf + strlen (buf);

    if ('\0' != *buf) {
        next [0] = ';';
        next [1] = '\0';
        ++next;
    }

    if (funcname)
        next += sprintf (next, "%s(%d,{", funcname, argc);
    else
        next += sprintf (next, "%d,{", argc);

    for (int i = 0; i < argc; ++i) {

        if (argv [i])
            next += sprintf (next, "\"%s\"", argv [i]);
        else {
            strcpy (next, "(null)");
            next += strlen (next);
        }

        if (i + 1 < argc) {
            next [0] = ',';
            next [1] = '\0';
            ++next;
        }
    }

    next [0] = '}';
    next [1] = ')';
    next [2] = '\0';

    // verify that the number of calls hasn't exceeded the size
    // of the array of return values
    _RWSTD_ASSERT (ncalls < MAXCALLBACKS);

    return retvals [ncalls];
}

/**************************************************************************/

static int
callback_imp (const char *cbname, int argc, char *argv[])
{
    const int status = pargs (argstr, cbname, argc, argv);

    ++ncalls;

    return status;
}


static int
foo (int argc, char *argv[])
{
    return callback_imp ("foo", argc, argv);
}


static int
bar (int argc, char *argv[])
{
    return callback_imp ("bar", argc, argv);
}

static int
err (int argc, char *argv[])
{
    return callback_imp ("ERR", argc, argv);
}

/**************************************************************************/

typedef int (cbfun_t)(int, char*[]);

static int opt_counts [4];

// hackery to allow passing int* and cbfun_t* in the same argument
static const union {
    int     *pint;
    cbfun_t *pfun;
    int      ival;
} cntptrs [] = {
    { opt_counts + 0 },
    { opt_counts + 1 },
    { opt_counts + 2 },
    { opt_counts + 3 }
};

/**************************************************************************/

static void
test_opts (const char *expect,
           int         get_exp,
           char       *argv [],
           int         set_exp,
           const char *argspec,
           cbfun_t    *f0,
           cbfun_t    *f1 = 0,
           cbfun_t    *f2 = 0,
           cbfun_t    *f3 = 0)
{
    argstr [0] = '\0';

    // reset all previously set options
    rw_setopts (0, 0);

    // set new options
    const int set_res = rw_setopts (argspec, f0, f1, f2, f3);

    if (set_res != set_exp) {

        // convert function pointers to void pointers
        // to avoid compiler error and warnings
        union {
            cbfun_t *pfun;
            void    *pvoid;
        } uptr[] = {
            { f0 }, { f1 }, { f2 }, { f3 }
        };

        fprintf (stderr,
                 "line %d: rw_setopts (\"%s\", %p, %p, %p, %p) == %d, got %d\n",
                 current_line, argspec,
                 uptr [0].pvoid, uptr [1].pvoid, uptr [2].pvoid, uptr [3].pvoid,
                 set_exp, set_res);

        exit_status = 1;
    }

    argstr [0] = '\0';

    // compute the value of argc from argv
    int argc = 0;
    for (; argv [argc]; ++argc);

    // reset the number of callback invocations
    ncalls = 0;

    // reset the option counters
    memset (opt_counts, 0, sizeof opt_counts);

    const int get_res = rw_runopts (argc, argv);

    if (get_res != get_exp) {

        static char tmp [4096];

        tmp [0] = '\0';
        pargs (tmp, 0, argc, argv);

        fprintf (stderr,
                 "line %d: rw_runopts (%s) == %d, got %d\n",
                 current_line, tmp, get_exp, get_res);

        exit_status = 1;
    }

    if (strchr (expect, '#')) {
        // when the expected result string contains the pound sign,
        // treat the arguments as pointers to integers as opposed
        // to pointers to callback functions and format the actual
        // result as a sequence of integers at the given addresses

        if (f0 == cntptrs [0].pfun) {
            sprintf (argstr + strlen (argstr),
                     "%s%d", *argstr ? "; #" : "#", opt_counts [0]);

            if (f1 == cntptrs [1].pfun) {
                sprintf (argstr + strlen (argstr),
                         ",%d", opt_counts [1]);

                if (f2 == cntptrs [2].pfun) {
                    sprintf (argstr + strlen (argstr),
                             ",%d", opt_counts [2]);

                    if (f3 == cntptrs [3].pfun) {
                        sprintf (argstr + strlen (argstr),
                                 ",%d", opt_counts [3]);
                    }
                }
            }
        }
    }

    if (strcmp (argstr, expect)) {
        fprintf (stderr,
                 "line %d: \"%s\" != \"%s\"\n",
                 current_line, argstr, expect);

        exit_status = 1;
    }
}

/**************************************************************************/

static char**
mkargv (const char *s0,
        const char *s1 = 0,
        const char *s2 = 0,
        const char *s3 = 0,
        const char *s4 = 0,
        const char *s5 = 0,
        const char *s6 = 0,
        const char *s7 = 0,
        const char *s8 = 0,
        const char *s9 = 0)
{
    static char argbuf [10][1024];
    static char* argv [10];

    argv [0] = s0 ? strcpy (argbuf [0], s0) : 0;
    argv [1] = s1 ? strcpy (argbuf [1], s1) : 0;
    argv [2] = s2 ? strcpy (argbuf [2], s2) : 0;
    argv [3] = s3 ? strcpy (argbuf [3], s3) : 0;
    argv [4] = s4 ? strcpy (argbuf [4], s4) : 0;
    argv [5] = s5 ? strcpy (argbuf [5], s5) : 0;
    argv [6] = s6 ? strcpy (argbuf [6], s6) : 0;
    argv [7] = s7 ? strcpy (argbuf [7], s7) : 0;
    argv [8] = s8 ? strcpy (argbuf [8], s8) : 0;
    argv [9] = s9 ? strcpy (argbuf [9], s9) : 0;

    return argv;
}
                              
/**************************************************************************/

// convenience macros for brevity

#define A mkargv
#define B bar
#define F foo
#define E err
#define C0 cntptrs [0].pfun
#define C1 cntptrs [1].pfun
#define C2 cntptrs [2].pfun
#define C3 cntptrs [3].pfun
#define N(n)  make_arg (n)
#define T (current_line = __LINE__), test_opts

/**************************************************************************/

static void
test_unknown_options ()
{
    // +--------- expected result string (formatted by callbacks)
    // |   +----- expected value returned from rw_runopts()
    // |   |  +-- second argument to rw_runopts() (argv)
    // |   |  |
    // |   |  |       +---------- expected rw_setopts() result
    // |   |  |       |  +------- rw_setopts() first argument
    // |   |  |       |  |    +-- rw_setopts() callbacks...
    // |   |  |       |  |    |
    // V   V  V       V  V    V
    T ("", 0, A (""), 0, "",  0);

    // exercise setting up the "unknown option" handler
    T ("",  0, A (""),  1, "-",     0);
    T ("",  0, A (""),  1, "-",     E);

    // exercise invoking the "unknown option" handler
    T ("ERR(1,{\"-x\"})",  0, A ("-x"),     2, "- f", E, F);
    T ("ERR(2,{\"-x\",\"-y\"});"
       "foo(1,{\"-y\"})",  0, A ("-x","-y"),  2, "- y", E, F);

    retvals [0] = 1;
    T ("ERR(2,{\"-x\",\"-y\"})", 1, A ("-x","-y"),  2, "- y", E, F);

    retvals [0] = 0;
    retvals [1] = 2;
    T ("foo(2,{\"-x\",\"-y\"});"
       "ERR(1,{\"-y\"})",  2, A ("-x","-y"),  2, "- x", E, F);
    T ("foo(3,{\"-x\",\"-y\",\"-x\"});"
       "ERR(2,{\"-y\",\"-x\"})",  2, A ("-x","-y", "-x"),  2, "- x", E, F);
    retvals [1] = 0;
}

/**************************************************************************/

static void
test_counted_options ()
{
    // exercise options with a counter instead of a callback
    T ("#1",   0, A ("-b"),              1, "b#",     C0);
    T ("#1",   0, A ("--cc"),            1, "|-cc#",  C0);
    T ("#2",   0, A ("-d", "-d"),        1, "d#",     C0);
    T ("#2",   0, A ("-e", "--ee"),      1, "e|-ee#", C0);
    T ("#3",   0, A ("-e", "-e", "-e"),  1, "e#",     C0);
    T ("#1,2", 0, A ("-f", "-g", "-g"),  2, "f# g#",  C0, C1);

    // exercise counted options with a numerical argument
    T ("#0",      0, A ("--n=0"),           1, "|-n#",     C0);
    T ("#1",      0, A ("--n=1"),           1, "|-n#",     C0);
    T ("#1",      0, A ("--n=+1"),          1, "|-n#",     C0);
    T ("#-1",     0, A ("--n=-1"),          1, "|-n#",     C0);
    T ("#2",      0, A ("--n=+2"),          1, "|-n#",     C0);
    T ("#-2",     0, A ("--n=-2"),          1, "|-n#",     C0);
    T ("#12345",  0, A ("--n=+12345"),      1, "|-n#",     C0);
    T ("#-12346", 0, A ("--n=-12346"),      1, "|-n#",     C0);

    // exercise counted options with a restricted numerical argument
    T ("#0",      0, A ("--n=0"),           1, "|-n#0",     C0);
    T ("#1",      0, A ("--n=1"),           1, "|-n#0",     C0);
    T ("#1",      0, A ("--n=1"),           1, "|-n#1",     C0);
    T ("#2",      0, A ("--n=2"),           1, "|-n#1",     C0);
    T ("#1",      0, A ("--n=+1"),          1, "|-n#+1",    C0);
    T ("#-1",     0, A ("--n=-1"),          1, "|-n#-1",    C0);
    T ("#0",      0, A ("--n=0"),           1, "|-n#-1",    C0);
    T ("#1",      0, A ("--n=1"),           1, "|-n#-1",    C0);
    T ("#123",    0, A ("--n=+123"),        1, "|-n#+123",  C0);
    T ("#124",    0, A ("--n=+124"),        1, "|-n#+123",  C0);
    T ("#-125",   0, A ("--n=-125"),        1, "|-n#-125",  C0);
    T ("#-126",   0, A ("--n=-126"),        1, "|-n#-127",  C0);

    T ("#0",      0, A ("--n=0"),           1, "|-n#0-1",   C0);
    T ("#1",      0, A ("--n=1"),           1, "|-n#0-1",   C0);

    T ("#0",      0, A ("--n=0"),           1, "|-n#-1-0",  C0);

    // same as above but with an out of range argument
    T ("",        1, A ("--n=1"),           1, "|-n#-1-0",  C0);
    T ("",        1, A ("--n=-1"),          1, "|-n#0",     C0);
    T ("",        1, A ("--n=0"),           1, "|-n#1",     C0);
    T ("",        1, A ("--n=1"),           1, "|-n#2",     C0);
    T ("",        1, A ("--n=+2"),          1, "|-n#+3",    C0);

    T ("",        1, A ("--n=-1"),          1, "|-n#0-1",    C0);
    T ("",        1, A ("--n=+2"),          1, "|-n#0-1",    C0);
    T ("",        1, A ("--n=-11"),         1, "|-n#-10--5", C0);
    T ("",        1, A ("--n=-4"),          1, "|-n#-10--5", C0);
    T ("",        1, A ("--n=-11"),         1, "|-n#-1-1",   C0);
    T ("",        1, A ("--n=-1"),          1, "|-n#0-32",   C0);
    T ("",        1, A ("--n=33"),          1, "|-n#0-32",   C0);
}

/**************************************************************************/

static void
test_tristate ()
{
    //  +-- expected result string
    //  |     +-- expected return value from rw_getopts()
    //  |     |     +-- command line arguments
    //  |     |     |                      +-- number of directives
    //  |     |     |                      |  +-- cmdopt specification
    //  |     |     |                      |  |         +-- counter
    //  |     |     |                      |  |         |
    //  V     V     V                      V  V         V
    T ("#1",  0, A ("--enable-foo"),       1, "|-foo~", C0);
    T ("#1",  0, A ("--use-foo"),          1, "|-foo~", C0);
    T ("#1",  0, A ("--with-foo"),         1, "|-foo~", C0);
    T ("#-1", 0, A ("--disable-foo"),      1, "|-foo~", C0);
    T ("#-1", 0, A ("--no-foo"),           1, "|-foo~", C0);
    T ("#-1", 0, A ("--without-foo"),      1, "|-foo~", C0);

    // the same tristate can be repeated any number of times
    T ("#1",  0, A ("--enable-foo", "--use-foo"),  1, "|-foo~", C0);
    T ("#-1", 0, A ("--no-foo", "--without-foo"),  1, "|-foo~", C0);

    // the last tristate wins
    T ("#-1", 0, A ("--use-foo", "--no-foo"), 1, "|-foo~", C0);
    T ("#1",  0, A ("--no-foo", "--use-foo"), 1, "|-foo~", C0);

    //////////////////////////////////////////////////////////////////

    // set bits using a bitmap
    T ("#1",             0, A ("--enable-foo"),   1, "|-foo~:0",  C0);
    T ("#1",             0, A ("--enable-foo"),   1, "|-foo~:1",  C0);
    T ("#2",             0, A ("--enable-foo"),   1, "|-foo~:2",  C0);
    T ("#3",             0, A ("--enable-foo"),   1, "|-foo~:3",  C0);
    T ("#4",             0, A ("--enable-foo"),   1, "|-foo~:4",  C0);

    // unset bits using a bitmap
    T ("#-2",            0, A ("--disable-foo"),  1, "|-foo~:1",  C0);
    T ("#-3",            0, A ("--disable-foo"),  1, "|-foo~:2",  C0);
    T ("#-4",            0, A ("--disable-foo"),  1, "|-foo~:3",  C0);
    T ("#-5",            0, A ("--disable-foo"),  1, "|-foo~:4",  C0);

    // set bits in word 2
    T ("#0,1",           0, A ("--enable-bar"),   1, "|-bar~32:0", C0, C1);
    T ("#0,1",           0, A ("--enable-bar"),   1, "|-bar~32:1", C0, C1);
    T ("#0,2",           0, A ("--enable-bar"),   1, "|-bar~32:2", C0, C1);
    T ("#0,3",           0, A ("--enable-bar"),   1, "|-bar~32:3", C0, C1);
    T ("#0,4",           0, A ("--enable-bar"),   1, "|-bar~32:4", C0, C1);
 
    // enable bits 0 through 4 in C0 one at a time
    T ("#1", 0, A ("--with-0"), 4, "|-0~0 |-1~1 |-2~2 |-3~3", C0, C0, C0, C0);
    T ("#2", 0, A ("--with-1"), 4, "|-0~0 |-1~1 |-2~2 |-3~3", C0, C0, C0, C0);
    T ("#4", 0, A ("--with-2"), 4, "|-0~0 |-1~1 |-2~2 |-3~3", C0, C0, C0, C0);
    T ("#8", 0, A ("--with-3"), 4, "|-0~0 |-1~1 |-2~2 |-3~3", C0, C0, C0, C0);

    // enable multiple bits 0 through 4 in C0 simultaneously
    T ("#3", 0, A ("--with-0", "--with-1"),
       4, "|-0~0 |-1~1 |-2~2 |-3~3", C0, C0, C0, C0);

    T ("#7", 0, A ("--with-0", "--with-1", "--with-2"),
       4, "|-0~0 |-1~1 |-2~2 |-3~3", C0, C0, C0, C0);

    T ("#15", 0, A ("--with-0", "--with-1", "--with-2", "--with-3"),
       4, "|-0~0 |-1~1 |-2~2 |-3~3", C0, C0, C0, C0);

    // specify bit value
    T ("#1", 0, A ("--with-0"),
       4, "|-0~0:1 |-1~1:1 |-2~2:1 |-3~3:1", C0, C0, C0, C0);

    T ("#2", 0, A ("--with-1"),
       4, "|-0~0:1 |-1~1:1 |-2~2:1 |-3~3:1", C0, C0, C0, C0);

    T ("#4", 0, A ("--with-2"),
       4, "|-0~0:1 |-1~1:1 |-2~2:1 |-3~3:1", C0, C0, C0, C0);

    T ("#8", 0, A ("--with-3"),
       4, "|-0~0:1 |-1~1:1 |-2~2:1 |-3~3:1", C0, C0, C0, C0);

    T ("#3", 0, A ("--with-0", "--with-1"),
       4, "|-0~0:1 |-1~1:1 |-2~2:1 |-3~3:1", C0, C0, C0, C0);

    T ("#7", 0, A ("--with-0", "--with-1", "--with-2"),
       4, "|-0~0:1 |-1~1:1 |-2~2:1 |-3~3:1", C0, C0, C0, C0);

    T ("#15", 0, A ("--with-0", "--with-1", "--with-2", "--with-3"),
       4, "|-0~0:1 |-1~1:1 |-2~2:1 |-3~3:1", C0, C0, C0, C0);

    T ("#-1073741824",
       0, A ("--enable-f30"),
       3, "|-f30~30:3 |-f28~28:2 |-f26~26:2", C0, C0, C0);

    T ("#-2147483648",
       0, A ("--enable-f30"),
       3, "|-f30~30:2 |-f28~28:2 |-f26~26:2", C0, C0, C0);

    T ("#-1610612736",
       0, A ("--enable-f30","--enable-f28"),
       3, "|-f30~30:2 |-f28~28:2 |-f26~26:2", C0, C0, C0);

    T ("#-1476395008",
       0, A ("--enable-f30","--enable-f28","--enable-f26"),
       3, "|-f30~30:2 |-f28~28:2 |-f26~26:2", C0, C0, C0);
}

/**************************************************************************/

static void
test_optional_argument ()
{
    // exercise an option with an optional argument
    T ("foo(1,{\"-a\"})",       0, A ("-a"),      1, "a:", F);
    T ("foo(1,{\"-a\"})",       0, A ("-a"),      1, "a:", F);
    T ("foo(2,{\"-a\",\"x\"})", 0, A ("-a", "x"), 1, "a:", F);
    T ("foo(1,{\"-ay\"})",      0, A ("-ay"),     1, "a:", F);
    T ("foo(1,{\"-axyz\"})",    0, A ("-axyz"),   1, "a:", F);
    T ("foo(1,{\"--a\"})",      0, A ("--a"),     1, "|-a:", F);
    T ("ERR(1,{\"--ab\"})",     0, A ("--ab"),    2, "- |-a:", E, F);

    // exercise the processing of two optional command line options
    T ("foo(2,{\"-a\",\"-b\"});"
       "bar(1,{\"-b\"})",
       0, A ("-a", "-b"), 2, "a: b", F, B);

    T ("foo(3,{\"-a\",\"x\",\"-b\"});"
       "bar(1,{\"-b\"})",
       0, A ("-a", "x", "-b"), 2, "a: b", F, B);

    // exercise optional restricted numeric argument
    T ("foo(2,{\"-n\",\"0\"})",    0, A ("-n", "0"),    1, "n:0",    F);
    T ("foo(2,{\"-n\",\"+1\"})",   0, A ("-n", "+1"),   1, "n:1",    F);
    T ("foo(2,{\"-n\",\"+2\"})",   0, A ("-n", "+2"),   1, "n:+2",   F);
    T ("foo(2,{\"-n\",\"\\-2\"})", 0, A ("-n", "\\-2"), 1, "n:-3",   F);
    T ("foo(2,{\"-n\",\"\\-0\"})", 0, A ("-n", "\\-0"), 1, "n:-3-0", F);
}

/**************************************************************************/

static void
test_required_argument ()
{
    // exercise the processing of an option with a required argument

    // the equals sign missing
    T ("ERR(1,{\"--a\"})",       0, A ("--a"),       2, "- |-a=", E, F);

    // required argument empty
    T ("foo(1,{\"--a=\"})",      0, A ("--a="),      1, "|-a=",   F);

    // required argument contains funky characters
    T ("foo(1,{\"--a=1\"})",     0, A ("--a=1"),     1, "|-a=",   F);
    T ("foo(1,{\"--b=1-\"})",    0, A ("--b=1-"),    1, "|-b=",   F);
    T ("foo(1,{\"--c=-2\"})",    0, A ("--c=-2"),    1, "|-c=",   F);
    T ("foo(1,{\"--d=1-2\"})",   0, A ("--d=1-2"),   1, "|-d=",   F);
    T ("foo(1,{\"--e=-1=2\"})",  0, A ("--e=-1=2"),  1, "|-e=",   F);
    T ("foo(1,{\"--f=-1=-2\"})", 0, A ("--f=-1=-2"), 1, "|-f=",   F);
    T ("foo(1,{\"--g=2,3\"})",   0, A ("--g=2,3"),   1, "|-g=",   F);
    T ("foo(1,{\"--h=3:4\"})",   0, A ("--h=3:4"),   1, "|-h=",   F);
    T ("foo(1,{\"--i=\"j\"\"})", 0, A ("--i=\"j\""), 1, "|-i=",   F);

    // exercise restricted numeric argument
    T ("foo(1,{\"--a=1\"})",     0, A ("--a=1"),     1, "|-a=0",   F);
    T ("foo(1,{\"--a=1\"})",     0, A ("--a=1"),     1, "|-a=1",   F);
    T ("foo(1,{\"--a=2\"})",     0, A ("--a=2"),     1, "|-a=2",   F);
    T ("foo(1,{\"--a=3\"})",     0, A ("--a=3"),     1, "|-a=3-4", F);
    T ("foo(1,{\"--a=5\"})",     0, A ("--a=5"),     1, "|-a=4-5", F);

    T ("",                       1, A ("--a=1"),     1, "|-a=2",    F);
    T ("",                       1, A ("--a=2"),     1, "|-a=0-1",  F);
    T ("",                       1, A ("--a=-1"),    1, "|-a=1-2",  F);
    T ("",                       1, A ("--a=+123"),  1, "|-a=+2-3", F);

}

/**************************************************************************/

static void
test_repeated_options ()
{
    // exercise repeated options
    // only the first occurrence of each command line option
    // causes an invocation of the callback, all subsequent
    // ones will be ignored by default
    T ("foo(2,{\"-a\",\"-a\"})", 0, A ("-a", "-a"), 1, "a", F);

    // unlimited number of invocations
    T ("foo(2,{\"-a\",\"-a\"});"
       "foo(1,{\"-a\"})",
       0, A ("-a", "-a"), 1, "a@*", F);

    // no invocation (option is disabled)
    T ("", 0, A ("-a"),       1, "a@0", F);
    T ("", 0, A ("-a", "-a"), 1, "a@0", F);

    T ("bar(1,{\"-b\"})",        0, A ("-a", "-a", "-b"), 2, "a@0 b", F, B);
    T ("bar(2,{\"-b\",\"-a\"})", 0, A ("-a", "-b", "-a"), 2, "a@0 b", F, B);

    T ("bar(3,{\"-b\",\"-a\",\"-b\"})",
       0, A ("-b", "-a", "-b"), 2, "b a@0", B, F);

    T ("bar(4,{\"-b\",\"-a\",\"-b\",\"-a\"})",
       0, A ("-b", "-a", "-b", "-a"), 2, "b a@0", B, F);

    // at most one invocation (default)
    T ("foo(2,{\"-a\",\"-a\"})", 0, A ("-a", "-a"), 1, "a@1", F);
    T ("foo(2,{\"-a\",\"-a\"})", 0, A ("-a", "-a"), 1, "a@1", F);

    // at most two invocations
    T ("foo(2,{\"-a\",\"-a\"});"
       "foo(1,{\"-a\"})",
       0, A ("-a", "-a"), 1, "a@2", F);

    T ("foo(3,{\"-a\",\"-a\",\"-a\"});"
       "foo(2,{\"-a\",\"-a\"})",
       0, A ("-a", "-a", "-a"), 1, "a@2", F);

    // inverted option (callback invoked iff option is not specified)
    T ("",          0, A ("-i"),       1, "i!",    F);
    T ("foo(0,{})", 0, A (""),         1, "i!",    F);
    T ("",          0, A ("--j"),      1, "i|-j!", F);

    T ("foo(1,{\"-k\"});"
       "foo(0,{})", 0, A ("-k"),       2, "k l!",  F, F);
}

/**************************************************************************/

int main ()
{
    // +--------- expected result string (formatted by callbacks)
    // |   +----- expected value returned from rw_runopts()
    // |   |  +-- second argument to rw_runopts() (argv)
    // |   |  |
    // |   |  |             +---------- expected rw_setopts() result
    // |   |  |             |  +------- rw_setopts() first argument
    // |   |  |             |  |    +-- rw_setopts() callbacks...
    // |   |  |             |  |    |
    // V   V  V             V  V    V
    T ("", 0, A (""),       0, "",  0);
    T ("", 0, A ("a"),      0, "",  0);
    T ("", 0, A ("a", "b"), 0, "",  0);
    T ("", 0, A ("a", "b"), 0, "",  0);

    T ("", 0, A (""),       1, "f", F);
    T ("", 0, A ("a"),      1, "f", F);
    T ("", 0, A ("a", "b"), 1, "f", F);
    T ("", 0, A ("a", "b"), 1, "f", F);
    T ("", 0, A ("a", "f"), 1, "f", F);
    T ("", 0, A ("f", "f"), 1, "f", F);

    // exercise short and/or long options
    T ("foo(1,{\"-a\"})",  0, A ("-a"),  1, "a",    F);
    T ("foo(1,{\"--a\"})", 0, A ("--a"), 1, "|-a",  F);
    T ("foo(1,{\"-a\"})",  0, A ("-a"),  1, "a|-a", F);
    T ("foo(1,{\"--a\"})", 0, A ("--a"), 1, "a|-a", F);

    // exercise the handling of unknown options
    test_unknown_options ();

    // exercise the handling of options with a counter
    // instead of a callback handler
    test_counted_options ();

    // exercise the handling of options with an optional argument
    test_optional_argument ();

    // exercise the handling of tristate options 
    test_tristate ();

    // exercise the handling of options with a required argument
    test_required_argument ();

    // exercise the handling of repeated occurrences of the same option
    test_repeated_options ();

    // exercise callback errors
    retvals [0] = 1;
    T ("foo(2,{\"-a\",\"-b\"})",
       retvals [0], A ("-a", "-b"), 2, "a b", F, B);

    retvals [0] = 0;
    retvals [1] = 2;

    T ("foo(3,{\"-a\",\"-b\",\"-c\"});"
       "bar(2,{\"-b\",\"-c\"})",
       retvals [1], A ("-a", "-b", "-c"), 3, "a b c", F, B, E);

    retvals [1] = 0;

    return exit_status;
}
