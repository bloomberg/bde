/************************************************************************
 *
 * 27.streambuf.imbue.stdcxx-307.cpp - test case from STDCXX-307 issue
 *
 * $Id: 27.streambuf.imbue.stdcxx-307.cpp 597670 2007-11-23 14:11:16Z faridz $
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
#include <rw/_defs.h>
#include <cassert>
#include <locale>
#include <streambuf>

struct MyFacet: std::locale::facet
{
    static std::locale::id id;
};

std::locale::id MyFacet::id;

int main ()
{
    struct MyBuf: std::streambuf {
        void imbue (const std::locale &loc) {
            std::streambuf::imbue (loc);
        }
    } buf;

    const std::locale loc (buf.getloc (), new MyFacet);

    assert (_STD_HAS_FACET (MyFacet, loc));
    assert (!_STD_HAS_FACET (MyFacet, buf.getloc ()));

    buf.imbue (loc);

    assert (!_STD_HAS_FACET (MyFacet, buf.getloc ()));

    buf.pubimbue (loc);

    assert (_STD_HAS_FACET (MyFacet, buf.getloc ()));

    return 0;
}
