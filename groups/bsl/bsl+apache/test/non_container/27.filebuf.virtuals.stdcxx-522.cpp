/************************************************************************
*
* 27.filebuf.virtuals.stdcxx-522.cpp - regression test for STDCXX-522
*
* http://issues.apache.org/jira/browse/STDCXX-522
*
* $Id: 27.filebuf.virtuals.stdcxx-522.cpp 620105 2008-02-09 13:25:00Z faridz $
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
#include <fstream>
#include <ios>

int main ()
{
    struct MyBuf : std::filebuf {
        int_type overflow (int_type c) {
            return std::filebuf::overflow (c);
        }
    } buf;

    buf.open ("file", std::ios::out);
    buf.pubsetbuf (0, 0);
    buf.overflow (std::filebuf::traits_type::eof ());
    buf.close ();

    std::ifstream in ("file");
    assert (std::istream::traits_type::eof () == in.get ());
}
