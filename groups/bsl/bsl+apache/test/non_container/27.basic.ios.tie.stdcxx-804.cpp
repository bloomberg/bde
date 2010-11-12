/************************************************************************
 *
 * 27.basic.ios.tie.stdcxx-804 - regression test for STDCXX-804
 *
 * http://issues.apache.org/jira/browse/STDCXX-804
 *
 * $Id: 27.basic.ios.tie.stdcxx-804.cpp 648752 2008-04-16 17:01:56Z faridz $
 *
 ************************************************************************
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
 * Copyright 2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <sstream>

// tie the first n streams together in a circular list
void tie (std::ostringstream *ps, std::size_t n)
{
    for (std::size_t i = 0; i + 1 < n; ++i)
        ps [i].tie (ps + i + 1);

    if (n) {
        std::ostream *plast = ps + (n - 1);
        plast->tie (ps);
    }
}


// detect a cycle in a list of tied streams
bool is_cycle (const std::ostream *ps)
{
    if (0 == ps)
        return false;

    const std::ostream *p0, *p1;

    for (p0 = ps, p1 = ps->tie (); p0 && p1;
         p0 = p0->tie (), p1 = p1->tie () ? p1->tie ()->tie () : 0)
        if (p0 == p1 || p0->rdbuf () == p1->rdbuf ())
            return true;

    return false;
}

int main ()
{
    std::ostringstream strm [32];
    const std::size_t N = sizeof strm / sizeof *strm;

    for (std::size_t i = 0; i != N; ++i) {

        // tie the first i streams together in a circular list
        tie (strm, i);

        assert (0 == i || is_cycle (strm));

        for (std::size_t j = 0; j != i; ++j) {

            // make sure formatted and unformatted output for
            // each of the tied streams works without getting
            // into a deadlock or into infinite recursion
            strm [j] << j;
            strm [j].flush ();

            assert (strm [j].good ());
        }
    }

    return 0;
}
