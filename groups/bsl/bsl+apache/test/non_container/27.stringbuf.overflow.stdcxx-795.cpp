/************************************************************************
 *
 * 27.stringbuf.overflow.stdcxx-795.cpp - test case from STDCXX-795 issue
 *
 * $Id: 27.stringbuf.overflow.stdcxx-795.cpp 648752 2008-04-16 17:01:56Z faridz $
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

#include <sstream>  // for ostringstream
#include <string>   // for string
#include <cassert>  // for assert()
#include <cstddef>  // for size_t

int main ()
{
    std::ostringstream strm;
    std::string s;

    for (std::size_t i = 1; i <= 1024; ++i) {
        const char c = char (i);
        strm << c;
        s.push_back (c);
        assert (strm.str () == s);
    }

    return 0;
}
