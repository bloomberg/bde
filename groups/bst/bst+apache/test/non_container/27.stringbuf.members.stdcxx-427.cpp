/************************************************************************
 *
 * 27.stringbuf.members.stdcxx-427.cpp - test case from STDCXX-427 issue
 *
 * $Id: 27.stringbuf.members.stdcxx-427.cpp 550991 2007-06-26 23:58:07Z sebor $
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
#include <cstdio>
#include <sstream>

int main ()
{
    struct Buf: std::stringbuf {
        Buf (std::string s, std::ios::openmode m)
            : std::stringbuf (s, m) { }

        void setget (int beg, int cur, int end) {
            setg (eback () + beg, eback () + cur, eback () + end);
        }

        void setput (int beg, int cur, int end) {
            setp (pbase () + beg, pbase () + end);
            pbump (cur);
        }
    };
#if DRQS
    {
        Buf buf ("abcde", std::ios::in);
        buf.setget (1, 2, 4);
        std::printf ("%s\n", buf.str ().c_str ());
        assert ("bcd" == buf.str ());
    }
    {
        Buf buf ("abcde", std::ios::out);
        buf.setput (1, 2, 4);
        std::printf ("%s\n", buf.str ().c_str ());
        assert ("bcde" == buf.str ());
    }
    {
        Buf buf ("abcde", std::ios::openmode ());
        std::printf ("%s\n", buf.str ().c_str ());
        assert ("" == buf.str ());
    }
#endif
}
