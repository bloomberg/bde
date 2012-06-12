/************************************************************************
 *
 * rw_alloc.h - definitions of rw_alloc and rw_free
 *
 * $Id: rw_alloc.h
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
 **************************************************************************/

#ifndef RW_ALLOC_H_INCLUDED
#define RW_ALLOC_H_INCLUDED


#include <testdefs.h>   // for test config macros

enum {
    RW_PROT_NONE  = 0,
    RW_PROT_READ  = 1 << 0,
    RW_PROT_WRITE = 1 << 1,
    RW_PROT_RDWR  = RW_PROT_READ | RW_PROT_WRITE,
    RW_PROT_BELOW = 1 << 3
};

// if flags == -1 memory will be allocated by malloc();
// if flags != -1 memory will be allocated by system call and
// additional guard page with PAGE_NOACCESS protection will be allocated
// if RW_PROT_BELOW & flags != 0 then guard page will be located right
// before the user data, otherwise - right after the user data
// if RWSTD_ALLOC_FLAGS environment variable is defined and != 0
// and flags == -1, then rw_alloc will use value of RWSTD_ALLOC_FLAGS
// instead of flags variable
_TEST_EXPORT void*
rw_alloc(_RWSTD_SIZE_T, int /*flags*/ = -1);

// free the memory block, allocated by rw_alloc()
_TEST_EXPORT void
rw_free(void*);

#endif   // RW_ALLOC_H_INCLUDED
