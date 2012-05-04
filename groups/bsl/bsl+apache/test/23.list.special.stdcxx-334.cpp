/************************************************************************
 *
 * 23.list.special.stdcxx-334.cpp - test case from STDCXX-334 issue
 *
 * $Id: 23.list.special.stdcxx-334.cpp 550991 2007-06-26 23:58:07Z sebor $
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

#include <list>
#include <string>
#include <cassert>

class Alloc : public std::allocator <char>
{
};

bool operator == (Alloc a1, Alloc a2)
{
    return false;
}

bool operator != (Alloc a1, Alloc a2)
{
    return true;
}

int main(int argc, char* argv[])
{
    const char src [] = "source string";
    const char dst [] = "destination string";

    typedef std::list <char, Alloc> List;

    Alloc a1;
    Alloc a2;

    assert (!(a1 == a2));

    List src_lst (src, src + sizeof (src) - 1, a1);
    List dst_lst (dst, dst + sizeof (dst) - 1, a2);

    src_lst.swap (dst_lst);

    assert (std::string (src_lst.begin (), src_lst.end ()) == dst);
    assert (std::string (dst_lst.begin (), dst_lst.end ()) == src);

    return 0;
}
