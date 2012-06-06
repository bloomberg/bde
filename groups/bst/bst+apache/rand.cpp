/************************************************************************
 *
 * rand.cpp - definitions of testsuite random number generators
 *
 * $Id: rand.cpp 648752 2008-04-16 17:01:56Z faridz $
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

#include <time.h>   // for clock()

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC
#include <rw_rand.h>


enum { TABLE_SIZE = 55 };

static _RWSTD_UINT32_T table32 [TABLE_SIZE];
static _RWSTD_SIZE_T   inx32 [2];


// seeds the 32-bit random number generator
_TEST_EXPORT void
rw_seed32 (_RWSTD_UINT32_T seed)
{
    if (0xffffffffUL == seed)
        seed = clock ();

    table32 [TABLE_SIZE - 1] = seed;

    _RWSTD_UINT32_T k = 1;

    for (_RWSTD_SIZE_T i = 0; i != TABLE_SIZE - 1U; ++i) {

        const _RWSTD_SIZE_T ii = 21U * i % TABLE_SIZE;
        table32 [ii] =  k;
        k            = seed - k;
        seed         = table32 [ii];
    }

    for (_RWSTD_SIZE_T i = 0; i != 4U; ++i) {
        for (_RWSTD_SIZE_T ii = 0; ii != TABLE_SIZE; ++ii)
            table32 [i] = table32 [i] - table32 [(i + 31U) % TABLE_SIZE];
    }

    inx32 [0] = 0U;
    inx32 [1] = TABLE_SIZE * 2U / 3U - 1U;
}


// returns a 32-bit random number in the range [0, limit)
// if limit is non-zero, otherwise in the range [0, UINT32_MAX)
_TEST_EXPORT _RWSTD_UINT32_T
rw_rand32 (_RWSTD_UINT32_T limit /* = 0 */)
{
    if (0 == inx32 [0] && 0 == inx32 [1])
        rw_seed32 (161803398);

    table32 [++inx32 [0] %= TABLE_SIZE] -=
        table32 [++inx32 [1] %= TABLE_SIZE];

    return limit ? table32 [inx32 [0]] % limit : table32 [inx32 [0]];
}


#ifdef _RWSTD_UINT64_T

static _RWSTD_UINT64_T table64 [TABLE_SIZE];
static _RWSTD_SIZE_T   inx64 [2];


// seeds the 64-bit random number generator
_TEST_EXPORT void
rw_seed64 (_RWSTD_UINT64_T seed)
{
    if ((_RWSTD_UINT64_T)-1 == seed)
        seed = clock ();

    table64 [TABLE_SIZE - 1] = seed;

    _RWSTD_UINT64_T k = 1;

    for (_RWSTD_SIZE_T i = 0; i != TABLE_SIZE - 1U; ++i) {

        const _RWSTD_SIZE_T ii = 21U * i % TABLE_SIZE;
        table64 [ii] =  k;
        k            = seed - k;
        seed         = table64 [ii];
    }

    for (_RWSTD_SIZE_T i = 0; i != 4U; ++i) {
        for (_RWSTD_SIZE_T ii = 0; ii != TABLE_SIZE; ++ii)
            table64 [i] = table64 [i] - table64 [(i + 31U) % TABLE_SIZE];
    }

    inx64 [0] = 0U;
    inx64 [1] = TABLE_SIZE * 2U / 3U - 1U;
}


// returns a 64-bit random number in the range [0, limit)
// if limit is non-zero, otherwise in the range [0, UINT64_MAX)
_TEST_EXPORT _RWSTD_UINT64_T
rw_rand64 (_RWSTD_UINT64_T limit)
{
    if (0 == inx64 [0] && 0 == inx64 [1])
        rw_seed64 (161803398);

    table64 [++inx64 [0] %= TABLE_SIZE] -=
        table64 [++inx64 [1] %= TABLE_SIZE];

    return limit ? table64 [inx64 [0]] % limit : table64 [inx64 [0]];
}

#endif   // _RWSTD_UINT64_T
