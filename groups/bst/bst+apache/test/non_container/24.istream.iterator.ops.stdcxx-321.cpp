/***********************************************************************
 *
 * 24.istream.iterator.ops.stdcxx-321.cpp: regression test for STDCXX-321
 *
 *     http://issues.apache.org/jira/browse/STDCXX-321
 *
 * $Id: 24.istream.iterator.ops.stdcxx-321.cpp 648752 2008-04-16 17:01:56Z faridz $
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

#include <cassert>
#include <iterator>

// no iostream header should be #included in this test
#ifdef _RWSTD_ISTREAM_INCLUDED
#  error "<istream> unexpectedly #included"
#endif   // _RWSTD_ISTREAM_INCLUDED

int main ()
{
    const std::istream_iterator<char, char> it;
    const std::istream_iterator<char, char> eos;

    // istream_iterator equality operator should compile
    // when the <istream> header isn't #included (and
    // the iterator should compare equal to the end of
    // stream iterator)
    assert (it == eos);

    return 0;
}
