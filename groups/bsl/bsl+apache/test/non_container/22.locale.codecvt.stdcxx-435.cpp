/************************************************************************
 *
 * 22.locale.codecvt.stdcxx-435.cpp - regression test for STDCXX-435
 *
 *   http://issues.apache.org/jira/browse/STDCXX-435
 *
 * $Id: 22.locale.codecvt.stdcxx-435.cpp 648752 2008-04-16 17:01:56Z faridz $
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

#include <cassert>
#include <clocale>
#include <cwchar>
#include <locale>

int main ()
{
#if TEST_RW_EXTENSIONS
    // No locale implemented from stlport for SUN and AIX.

    // multibyte locale
    std::locale mbloc;

    // names of some well-known multibyte locales
    const char* const names[] = {
        "EN_US.UTF-8",   // AIX
        "en_US.UTF-8",   // AIX, Linux, Solaris
        "en_US.utf8",    // HP-UX

        // starting with Vista, Windows follows RFC 4646 name format
        // http://msdn2.microsoft.com/en-us/library/ms776260(VS.85).aspx
        "hy-AM",  // Win 2000
        "as-IN",  // Win Vista
        "bn-IN",  // Win XP SP2
        "zh-HK",
        "zh-CN",

        0
    };

    // try to find the first multibyte locale on this system
    for (unsigned i = 0; names [i]; ++i) {
        if (std::setlocale (LC_ALL, names [i])) {
            mbloc = std::locale (names [i]);
            break;
        }
    }

    // fall back on the "C" locale...

    typedef std::codecvt<wchar_t, char, std::mbstate_t> MbCvt;

    const MbCvt &cvt = std::use_facet<MbCvt>(mbloc);

    // source and destination buffers
    const char src[] = "abc";
    wchar_t dst [2] = { L'\0', L'\1' };

    const char* from_next;

    wchar_t* to_next;

    std::mbstate_t state = std::mbstate_t ();

    // convert exactly one source character, expect exactly
    // one wide character in the destination buffer
    const std::codecvt_base::result res =
        cvt.in (state,
                src, src + 1, from_next,
                dst, dst + 2, to_next);

    assert (cvt.ok == res);
    assert (1 == from_next - src);
    assert (1 == to_next - dst);
    assert ('a' == dst [0] && '\1' == dst [1]);
#endif  // TEST_RW_EXTENSIONS
}
