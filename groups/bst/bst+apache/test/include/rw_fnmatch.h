/************************************************************************
 *
 * rw_fnmatch.h - declarations of testsuite helpers
 *
 * $Id: rw_fnmatch.h 648752 2008-04-16 17:01:56Z faridz $
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

#ifndef RW_FNMATCH_H_INCLUDED
#define RW_FNMATCH_H_INCLUDED


#include <testdefs.h>


// rw_fnmatch() matches patterns as described in the Shell and Utilities
// volume of IEEE Std 1003.1-2001, Section 2.13.1, Patterns Matching
// a Single Character, and Section 2.13.2, Patterns Matching Multiple
// Characters. It checks the string specified by the string argument
// to see if it matches the pattern specified by the pattern argument.

_TEST_EXPORT int
rw_fnmatch (const char*, const char*, int);


#endif   // RW_FNMATCH_H_INCLUDED
