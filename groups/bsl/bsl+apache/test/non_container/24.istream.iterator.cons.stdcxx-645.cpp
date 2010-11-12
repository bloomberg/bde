/***********************************************************************
 *
 * 24.istream.iterator.cons.stdcxx-645.cpp
 *     regression test for STDCXX-645:
 *     http://issues.apache.org/jira/browse/STDCXX-645
 *
 * $Id: 24.istream.iterator.cons.stdcxx-645.cpp 648752 2008-04-16 17:01:56Z faridz $
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

#include <assert.h>
#include <iterator>
#include <sstream>

int main ()
{
    std::istringstream a ("1");
    std::istream_iterator<int> i (a);

    std::istringstream b ("2");
    std::istream_iterator<int> j (b);

    assert (!(i == j));
}
