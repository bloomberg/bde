/************************************************************************
*
* 23.vector.bool.stdcxx-235.cpp - regression test for STDCXX-235
*
* https://issues.apache.org/jira/browse/STDCXX-235
*
* $Id: 23.vector.bool.stdcxx-235.cpp 588739 2007-10-26 18:41:14Z faridz $
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
#include <vector>

int main ()
{
    std::vector<bool> v;

    typedef std::vector<bool>::const_iterator const_iterator;
    typedef std::vector<bool>::iterator       iterator;

    const const_iterator cbegin = v.begin ();
    const iterator       begin  = v.begin ();

    assert (cbegin == begin);
    assert (begin  == cbegin);

    assert (!(cbegin != begin));
    assert (!(begin  != cbegin));

    assert (cbegin <= begin);
    assert (begin  <= cbegin);

    assert (cbegin >= begin);
    assert (begin  >= cbegin);

    assert (!(cbegin < begin));
    assert (!(begin  < cbegin));

    assert (!(cbegin > begin));
    assert (!(begin  > cbegin));
}
