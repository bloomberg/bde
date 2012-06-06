/************************************************************************
 *
 * 22.locale.cons.stdcxx-485.cpp
 *
 * test exercising the thread safety of locale ctors
 *
 * $Id: 22.locale.cons.stdcxx-485.cpp 580483 2007-09-28 20:55:52Z sebor $
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

#include <bsls_platform.h>

#include <cassert>
#include <cstring>
#include <locale>

#include <rw_locale.h>
#include <driver.h>


static int
run_test (int, char**)
{
    // obtain a NUL-separated list of installed locales
#if TEST_RW_EXTENSIONS
    // Only classic 'C' locale implemented on stlport.
    const char* const names = rw_locales ();
#else
    const char *const names = "C\0";
#endif

    if (0 == names || '\0' == *names)
        return 0;

    // repeatedly create a large set of unique named locale objects
    for (int j = 0; j != 2; ++j) {
        std::locale locales [64];

        std::size_t i = 0;

        for (const char *pn = names; *pn; ++i, pn += std::strlen (pn) + 1) {
            if (i == sizeof locales / sizeof *locales)
                break;

            locales [i] = std::locale (pn);
        }

        i = 0;

        for (const char *pn = names; *pn; ++i, pn += std::strlen (pn) + 1) {
            if (i == sizeof locales / sizeof *locales)
                break;

            rw_assert (std::has_facet<std::collate<char> >(locales [i]),
                       0, __LINE__,
                       "std::has_facet<std::collate<char> >(locales(#s))",
                       pn);
        }
    }

    return 0;
}


int main (int argc, char* argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.locale.cons",
                    "STDCXX-485",
                    run_test,
                    "",
                    (void*)0);
}
