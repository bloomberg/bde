/**************************************************************************
 *
 * 27.basic.ios.copyfmt.stdcxx-766.cpp - regression test for STDCXX-766
 *
 * http://issues.apache.org/jira/browse/STDCXX-766
 *
 * $Id: 27.basic.ios.copyfmt.stdcxx-766.cpp 648752 2008-04-16 17:01:56Z faridz $
 *
 **************************************************************************
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
#include <iosfwd>
#include <locale>
#include <ostream>


int main ()
{
    struct: std::streambuf { } xsb, ysb;
    std::ostream xstrm (0);
    std::ostream ystrm (0);

    std::ios x (&xsb);
    std::ios y (&ysb);

    x.tie (&xstrm);

    const std::ios::iostate     x_rdstate = x.rdstate ();
    const std::streambuf* const x_rdbuf   = x.rdbuf ();

    const std::locale loc =
        std::locale (std::locale::classic (), new std::numpunct<char>());

    // set up y to be different from x
    y.tie (&ystrm);
    y.exceptions (y.eofbit);
    y.setf (y.boolalpha);
    y.precision (x.precision () + 1);
    y.width (x.width () + 1);
    y.fill (x.fill () + 1);
    y.imbue (loc);

    // verify test preconditions
    assert (x.tie ()    != y.tie ());
    assert (x.getloc () != y.getloc ());

    x.copyfmt (y);

    // verify copyfmt() postconditions
    assert (x.rdbuf ()      == x_rdbuf);
    assert (x.tie ()        == y.tie ());
    assert (x.rdstate ()    == x_rdstate);
    assert (x.exceptions () == y.exceptions ());
    assert (x.flags ()      == y.flags ());
    assert (x.precision ()  == y.precision ());
    assert (x.width ()      == y.width ());
    assert (x.fill ()       == y.fill ());
    assert (x.getloc ()     == y.getloc ());
}
