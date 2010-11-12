/************************************************************************
 *
 * 22.locale.money.get.stdcxx-62.cpp - regression test for STDCXX-62
 *
 *   http://issues.apache.org/jira/browse/STDCXX-62
 *
 * $Id: 22.locale.money.get.stdcxx-62.cpp 650350 2008-04-22 01:35:17Z sebor $
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
#include <ios>
#include <locale>

int main ()
{
    typedef std::money_get<char> MoneyGet;

    const MoneyGet &mg = std::use_facet<MoneyGet>(std::locale::classic ());

    std::ios io (0);
    std::ios::iostate err = std::ios::goodbit;
    long double x = 0;

    // verify that the facet behavior is well-defined for empty stream

    mg.get (std::istreambuf_iterator<char>(),
            std::istreambuf_iterator<char>(),
            false, io, err, x);
#if DRQS
    // 'do_get' in stl/_monetary.c should set err properly.
    assert ((std::ios::eofbit | std::ios::failbit) == err);
#else
    assert (std::ios::eofbit == err);
#endif
}
