/************************************************************************
 *
 * 26.valarray.sub.stdcxx-224.cpp - regression test for STDCXX-224
 *   http://issues.apache.org/jira/browse/STDCXX-224
 *
 * $Id: 26.valarray.sub.stdcxx-224.cpp 590132 2007-10-30 16:01:33Z faridz $
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
#include <cstddef>
#include <valarray>

int main ()
{
    const int vals[] = { 1, 0 };
    const std::size_t N = sizeof vals / sizeof *vals;

    const std::valarray<int> p (vals, N);

    assert (N == p.size ());

    const std::valarray<int> r = p [std::slice (0, 0, 1)];

    assert (0 == r.size ());
}
