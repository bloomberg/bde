/************************************************************************
 *
 * 21.string.stdcxx-466.cpp - test case from STDCXX-466 issue
 *
 * $Id: 21.string.stdcxx-466.cpp 580483 2007-09-28 20:55:52Z sebor $
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
#include <string>

int main (int argc, char* argv[])
{
    std::string s ("efgh");

    const std::string::size_type len = s.max_size () + 1;

#define TEST(expr)                                      \
    try {                                               \
        assert (expr);                                  \
    }                                                   \
    catch (...) {                                       \
        assert (!"Unexpected exception was thrown");    \
    } (void)0

    const std::string::size_type npos = std::string::npos;

    TEST (npos == s.find ("fg", 0, len));
    TEST (npos == s.rfind ("fg", 0, len));
    TEST (0 == s.find_first_of ("eh", 0, len));
    TEST (3 == s.find_last_of ("eh", npos, len));
    TEST (0 > s.compare (0, npos, "efgh", len));
    TEST (0 > s.compare (0, npos, "ijkl", len));
    TEST (0 < s.compare (0, npos, "abcd", len));

    s.clear ();

    TEST (npos == s.find_first_not_of ("eh", 0, len));
    TEST (npos == s.find_last_not_of ("eh", npos, len));

    return 0;
}
