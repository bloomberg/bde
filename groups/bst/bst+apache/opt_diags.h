/************************************************************************
 *
 * opt_diags.h - declarations of diagnostic option handlers
 *
 * $Id: opt_diags.h 550991 2007-06-26 23:58:07Z sebor $
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
 * Copyright 1994-2006 Rogue Wave Software.
 *
 **************************************************************************/

#ifndef RW_OPT_DIAGS_H_INCLUDED
#define RW_OPT_DIAGS_H_INCLUDED


#include <testdefs.h>


enum diag_t {
    diag_trace   =  0,   // trace statements of inactive diagnostics
    diag_info    =  1,   // informational messages (e.g., indicating progress)
    diag_note    =  2,   // noteworthy events
    diag_expect  =  3,   // unexpected diagnostic inactive
    diag_xwarn   =  4,   // expected warning
    diag_warn    =  5,   // warning
    diag_xassert =  6,   // expected assertion failure
    diag_assert  =  7,   // assertion failure
    diag_error   =  8,   // test logic errors (do not terminate the test)
    diag_fatal   =  9,   // fatal errors (the first will terminate the test)

    N_DIAG_TYPES = diag_fatal + 1
};


struct esc_text_t
{
    char esc_pfx [40];   // escape prefix
    char esc_sfx [40];   // escape suffix
    char code    [16];   // short code
    char desc    [80];   // verbose description
};


extern esc_text_t
diag_msgs [N_DIAG_TYPES];


int
_rw_setopt_diags (int argc, char *argv[]);


#endif   // RW_OPT_DIAGS_H_INCLUDED
