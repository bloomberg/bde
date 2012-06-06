/************************************************************************
 *
 * 22.locale.num.put.stdcxx-2.cpp
 *
 * the regression test for STDCXX-2 issue
 *
 * $Id: 22.locale.num.put.stdcxx-2.cpp 637239 2008-03-14 20:09:38Z faridz $
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
#include <ios>
#include <string>
#include <sstream>

 void test (std::streamsize prec)
{
    const std::size_t len = 0 > prec ? 7 : (1 < prec ? prec + 1 : 2);
    std::string exp (len, '0');
    exp [1] = '.';

    std::ostringstream strm;
    strm.setf (strm.showpoint);
    strm.precision (prec);

    strm << 0.0f;
    assert (exp == strm.str ());

    strm.str ("");
    strm << 0.0;
    assert (exp == strm.str ());

    strm.str ("");
    strm << 0.0L;
    assert (exp == strm.str ());
}

int main ()
{
#if DRQS
    // Negative precision is legal but should set precision to default.
    // stlport does not do this currently.
    test (-2);
#endif
    test (0);
    test (1);
    test (5);

    return 0;
}
