/************************************************************************
 *
 * opt_trace.h - declarations of tracing option handlers
 *
 * $Id: opt_trace.h 465641 2006-10-19 14:15:56Z faridz $
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

#ifndef RW_OPT_TRACE_H_INCLUDED
#define RW_OPT_TRACE_H_INCLUDED


#include "opt_diags.h"


// diagnostics enabled by defaults
extern int
_rw_diag_mask;

// ignored diagnostics bitmap
extern int
_rw_diag_ignore;


int
_rw_setopt_trace_mask (int argc, char *argv[]);


int
_rw_setopt_trace (int argc, char *argv[]);


#endif   // RW_OPT_TRACE_H_INCLUDED
