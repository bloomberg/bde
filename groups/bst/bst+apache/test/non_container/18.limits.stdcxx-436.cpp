/************************************************************************
 *
 * 18.limits.stdcxx-436.cpp - test case from STDCXX-436 issue
 *
 * $Id: 18.limits.stdcxx-436.cpp 580483 2007-09-28 20:55:52Z sebor $
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

#include <assert.h>  // for assert()
#include <limits.h>  // for MB_LEN_MAX

// tell Compaq C++ we need the declaration of the POSIX popen() function
// that's guarded (not declared) in the compiler's pure C++ libc headers
#undef __PURE_CNAME
#include <stdio.h>   // for popen(), fscanf(), pclose(), ...

#if defined(_WIN32) || defined(_WIN64)
#  define popen  _popen
#  define pclose _pclose
#endif  // defined(_WIN32) || defined(_WIN64)

int main ()
{
    // assumes getconf is in the path. on most unix systems it is in
    // '/usr/bin' and it doesn't exist on most win32 configurations,
    // yet somehow this appears to work on windows, but not sure how.

    FILE* f = popen ("getconf MB_LEN_MAX", "r");
    if (f) {

        int mb_len_max = -1;

        const bool failed = fscanf (f, "%d", &mb_len_max) != 1;

        pclose (f);

        // if getconf isn't available, or getconf doesn't know the limit
        // the scan will fail, so we must handle both situations.
        assert (failed || MB_LEN_MAX == mb_len_max);
    }

    return 0;
}
