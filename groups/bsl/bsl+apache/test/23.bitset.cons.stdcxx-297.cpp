/************************************************************************
 *
 * 23.bitset.cons.stdcxx-297.cpp - test case from STDCXX-297 issue
 *
 * $Id: 23.bitset.cons.stdcxx-297.cpp 601042 2007-12-04 19:07:45Z sebor $
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
#include <new>
#include <bitset>
#include <string>
#include <cstddef>
#include <string.h>

int main ()
{
    typedef std::bitset<128> BitSet;
    const std::size_t size = sizeof (BitSet);

    char buf [size + 4];
    memset (buf, '\xff', sizeof (buf));

    const std::basic_string<int> s;
    BitSet* btest = new (buf) BitSet (s);

    for (std::size_t i = size; i < sizeof (buf); ++i)
        assert ('\xff' == buf [i]);

    btest->~BitSet ();

    return 0;
}
