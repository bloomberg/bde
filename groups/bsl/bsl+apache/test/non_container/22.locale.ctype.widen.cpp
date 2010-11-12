/***************************************************************************
 *
 * 22.locale.ctype.widen.cpp - tests exercising std::ctype<charT>::widen()
 *
 * $Id$
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
 * Copyright 1994-2008 Rogue Wave Software.
 * 
 **************************************************************************/

#include <locale>

#include <driver.h>

typedef unsigned char UChar;

#ifndef _RWSTD_NO_WCHAR_T

typedef std::ctype<wchar_t> WCtype;

#endif   // _RWSTD_NO_WCHAR_T

/**************************************************************************/

static void
test_char ()
{
    rw_info (0, __FILE__, __LINE__, "std::ctype<char>::widen(char)");

    const std::locale loc;

    const WCtype& ctp = std::use_facet<WCtype> (loc);

    for (int i = 0; i != int (_RWSTD_UCHAR_MAX) + 1; ++i) {

        // compute the argument, and the expected and actual results
        const char narrow = char (i);
        const char expect = narrow;
        const char wide   = ctp.widen (narrow);

        rw_assert (expect == wide, __FILE__, __LINE__,
                   "std::ctype<char>::widen(%#c) == %#c, got %#c",
                   narrow, expect, wide);
    }
}

/**************************************************************************/

// will leave this here for now since it appears to be a placeholder
// for further testing

static void
test_byname_char ()
{
    rw_info (0, __FILE__, __LINE__,
             "std::ctype_byname<char>::widen(char)");

    rw_warn (false, __FILE__, __LINE__,
             "ctype_byname<char>::widen() not exercised");
}

/**************************************************************************/

static void
test_wchar_t ()
{
    rw_info (0, __FILE__, __LINE__,
             "std::ctype<wchar_t>::widen(char)");

#ifndef _RWSTD_NO_WCHAR_T

    const std::locale loc;

    const WCtype& ctp = std::use_facet<WCtype> (loc);

    for (int i = 0; i != int (_RWSTD_UCHAR_MAX) + 1; ++i) {

        // compute the argument, and the expected and actual results
        const char    narrow = char (i);
        const wchar_t expect = wchar_t (UChar (narrow));
        const wchar_t wide   = ctp.widen (narrow);

        rw_assert (expect == wide, __FILE__, __LINE__,
                   "std::ctype<wchar_t>::widen(%#c) == %#Lc, got %#Lc",
                   narrow, expect, wide);
    }

#else   // if defined (_RWSTD_NO_WCHAR_T)

    rw_warn (false, __FILE__, __LINE__,
             "ctype<wchar_t>::widen() not exercised: "
             "macro _RWSTD_NO_WCHAR_T defined");

#endif   // _RWSTD_NO_WCHAR_T

}

/**************************************************************************/

// another placeholder for further testing?

static void
test_byname_wchar_t ()
{
    rw_info (0, __FILE__, __LINE__,
             "std::ctype_byname<wchar_t>::widen(char)");

    rw_warn (false, __FILE__, __LINE__,
             "ctype_byname<wchar_t>::widen() not exercised");
}

/**************************************************************************/

static int
run_test (int /*unused*/, char* /*unused*/ [])
{
    test_char ();
    test_byname_char ();

    test_wchar_t ();
    test_byname_wchar_t ();

    return 0;
}

/*extern*/ int
main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.locale.ctype.widen",
                    0,   // no comment
                    run_test, "", 0);
}

