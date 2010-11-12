/************************************************************************
 *
 * environ.h - declarations of testsuite helpers
 *
 * $Id: environ.h 550991 2007-06-26 23:58:07Z sebor $
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

#ifndef RW_ENVIRON_H_INCLUDED
#define RW_ENVIRON_H_INCLUDED


#include <testdefs.h>


// rw_putenv is a wrapper for the C standard library putenv function
// when called with the first argument of 0, rw_putenv defines any
// environment variables specified in the RW_PUTENV environment
// variable (e.g., RW_PUTENV=:foo=1:bar=2 will cause the variables
// foo=1 and bar=2 to be defined in the environment)
_TEST_EXPORT int
rw_putenv (const char*, int = -1);


#endif   // RW_ENVIRON_H_INCLUDED
