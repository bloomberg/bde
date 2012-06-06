/************************************************************************
 *
 * rw_rand.h - declarations of testsuite random number generators
 *
 * $Id: rw_rand.h 453816 2006-10-07 00:12:43Z sebor $
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

#ifndef RW_RAND_H_INCLUDED
#define RW_RAND_H_INCLUDED

#include <rw/_defs.h>   // for libstd config macros
#include <testdefs.h>   // for test config macros

// seeds the 32-bit random number generator
// if the seed is equal UINT32_MAX picks a random value to seed
// the generator with
_TEST_EXPORT void
rw_seed32 (_RWSTD_UINT32_T);

// returns a 32-bit random number in the range [0, limit)
// if limit is non-zero, otherwise in the range [0, UINT32_MAX)
_TEST_EXPORT _RWSTD_UINT32_T
rw_rand32 (_RWSTD_UINT32_T = 0);

#ifdef _RWSTD_UINT64_T

// seeds the 64-bit random number generator
// if the seed is equal UINT64_MAX picks a random value to seed
// the generator with
_TEST_EXPORT void
rw_seed64 (_RWSTD_UINT64_T);

// returns a 64-bit random number in the range [0, limit)
// if limit is non-zero, otherwise in the range [0, UINT64_MAX)
_TEST_EXPORT _RWSTD_UINT64_T
rw_rand64 (_RWSTD_UINT64_T = 0);

#  if 32 == _RWSTD_INT_SIZE
#    define rw_seed(limit)   rw_seed32 (limit)
#    define rw_rand(limit)   rw_rand32 (limit)
#  else   // if 32 != _RWSTD_INT_SIZE
#    define rw_seed(limit)   rw_seed64 (limit)
#    define rw_rand(limit)   rw_rand64 (limit)
#  endif   // 32 == _RWSTD_INT_SIZE
#else   // if !defined (_RWSTD_UINT64_T)
#  define rw_seed(limit)   rw_seed32 (limit)
#  define rw_rand(limit)   rw_rand32 (limit)
#endif   // _RWSTD_UINT64_T

#endif   // RW_RAND_H_INCLUDED
