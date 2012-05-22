/************************************************************************
 *
 * alg_test.cpp - definitions of testsuite helpers
 *
 * $Id: alg_test.cpp 509950 2007-02-21 09:36:50Z faridz $
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

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <stdlib.h>     // for rand()


#include <alg_test.h>


// generate a unique sequential number starting from 0
_TEST_EXPORT int gen_seq ()
{
    static int val;

    return ++val;
}


// generate numbers in the sequence 0, 0, 1, 1, 2, 2, 3, 3, etc...
_TEST_EXPORT int gen_seq_2lists ()
{
    static int vals [2];

    return vals [0] += ++vals [1] % 2;
}


// generate a sequence of subsequences (i.e., 0, 1, 2, 3, 4, 0, 1, 2, etc...)
_TEST_EXPORT int gen_subseq ()
{
    static int val;

    return val++ % 5;
}


// wrapper around a (possibly) extern "C" int rand()
// extern "C++"
_TEST_EXPORT int gen_rnd ()
{
    return rand ();
}


_TEST_EXPORT unsigned
ilog2 (size_t n)
{
    unsigned result = 0;

    while (n >>= 1)
        ++result;

    return result;
}


_TEST_EXPORT unsigned
ilog10 (size_t n)
{
    unsigned result = 0;

    while (n /= 10)
        ++result;

    return result;
}
