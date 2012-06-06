/************************************************************************
 *
 * 21.string.stdcxx-231.cpp - test case from STDCXX-231 issue
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
 **************************************************************************/

#include <cassert>
#include <string>
#include <sstream>

int main ()
{
    std::string s (100, 's');

    const std::string::size_type n = s.capacity ();
    const std::string::const_pointer p = s.c_str ();

    // verify getline(), clear(), erase() and replace() do not
    // unnecessarily resize or reallocate the data buffer
    std::istringstream is ("hello world");
    std::getline (is, s);
    assert (s.capacity () == n);
    assert (s.c_str ()    == p);

    s.clear ();
    assert (s.capacity () == n);
    assert (s.c_str ()    == p);

    s.erase ();
    assert (s.capacity () == n);
    assert (s.c_str ()    == p);

    s.replace (0, std::string::npos, "1", 0);
    assert (s.capacity () == n);
    assert (s.c_str ()    == p);

    return 0;
}

