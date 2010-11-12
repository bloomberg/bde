/************************************************************************
 *
 * 23.vector.stdcxx-611.cpp - regression test for STDCXX-611
 *
 * https://issues.apache.org/jira/browse/STDCXX-611
 *
 * $Id: 23.vector.stdcxx-611.cpp 592996 2007-11-08 01:55:18Z sebor $
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

#include <vector> // for vector

struct S
{
   void operator& () const {};
};

void test ()
{
    const S s [3] = { S (), S (), S () };

    std::vector<S> v;

#if DRQS // Fails for overloaded operator&
    v.assign (1, s [0]);
    v.assign (s, s+3);
    v.at (0);
    v.back ();
    v.begin ();
    v.capacity ();
    v.empty ();
    v.end ();
    v.front ();
    v.insert (v.begin (), s [0]);
    v.insert (v.begin (), s, s+3);
    v.insert (v.begin (), 2, s [0]);
    v.erase (v.begin ());
    v.erase (v.begin (), v.end ());
    v.max_size ();
    v.pop_back ();
    v.push_back (s[0]);
    v.rbegin ();
    v.rend ();
    v.reserve (10);
    v.resize (10);
#endif
    v.size ();
    std::vector<S>().swap (v);
}


int main ()
{
    // compile-time only test
    return 0;
}
