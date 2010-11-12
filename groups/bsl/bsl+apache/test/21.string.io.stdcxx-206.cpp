/***********************************************************************
 *
 * 21.string.io.stdcxx-206.cpp - regression test for STDCXX-206
 *
 *   http://issues.apache.org/jira/browse/STDCXX-206
 *
 * $Id: 21.string.io.stdcxx-206.cpp 590132 2007-10-30 16:01:33Z faridz $
 *
 ***********************************************************************
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
 ***********************************************************************/

#include <strstream>
#include <string>
#include <cassert>

#include <rw/_defs.h>

void test (const std::string& str, const std::streamsize width)
{
    char buf[10];
    std::ostrstream os (buf, sizeof(buf));

    os.width (width);
    os.exceptions (std::ios_base::failbit | std::ios_base::badbit);

    try {
        os << str;
    }
    catch (std::ios_base::failure&) {
    }

#ifndef _RWSTD_NO_EXT_KEEP_WIDTH_ON_FAILURE
    assert (width == os.width ());
#else
    assert (0 == os.width ());
#endif
}

int main ()
{
    std::string str ("abcdefghijk");

    test (str, 2);
    test (str, str.size () + 2);

    return 0;
}
