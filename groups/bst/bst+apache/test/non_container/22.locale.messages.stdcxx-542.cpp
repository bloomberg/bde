/************************************************************************
 *
 * 22.locale.messages.stdcxx-508_542.cpp
 *
 * the regression test for STDCXX-508, STDCXX-542 issues
 *
 * $Id: 22.locale.messages.stdcxx-542.cpp 580483 2007-09-28 20:55:52Z sebor $
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

#include <cstdio>
#include <cassert>
#include <locale>

#include <rw_locale.h>
#include <driver.h>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#ifdef _WIN32
#  define CATALOG "test"
#  define MSG_EXT ".rc"
#  define CAT_EXT ".dll"
#else
#  define CATALOG "./test"
#  define MSG_EXT ".msg"
#  define CAT_EXT ".cat"
#endif


static int run_test (int, char**)
{
    const char * catalog =
        "First set, first message\0\0"
        "Second set, first message\0\0";

    rw_create_catalog (CATALOG MSG_EXT, catalog);

    typedef std::messages<char> messagesT;

    const std::locale loc;

    const messagesT& msgs =
        std::use_facet<messagesT>(loc);

    messagesT::catalog cats [4];

    int i;

    // test STDCXX-508
    for (i = 0; i < sizeof (cats) / sizeof (*cats); ++i) {
        cats [i] = msgs.open (CATALOG CAT_EXT, loc);
        assert (-1 != cats [i]);
    }

    // test STDCXX-542
    for (--i; i >= 0; --i)
        msgs.close (cats [i]);

    std::remove (CATALOG CAT_EXT);

    return 0;
}


int main (int argc, char* argv[])
{
#ifdef _MSC_VER
    // disable GUI window from abort()
    _CrtSetReportMode (_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif

#if TEST_RW_EXTENSIONS
    return rw_test (argc, argv, __FILE__,
                    "lib.locale.messages",
                    "STDCXX-508;STDCXX-542",
                    run_test,
                    "",
                    (void*)0);
#else
    return 0;
#endif
}
