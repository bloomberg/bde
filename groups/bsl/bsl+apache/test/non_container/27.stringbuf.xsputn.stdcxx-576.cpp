/************************************************************************
 *
 * 27.stringbuf.xsputn.stdcxx-576.cpp - test case from STDCXX-576 issue
 *
 * $Id: 27.stringbuf.xsputn.stdcxx-576.cpp 590132 2007-10-30 16:01:33Z faridz $
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

#include <sstream>  // for stringbuf
#include <string>   // for string
#include <cassert>  // for assert()

struct PubBuf: std::stringbuf
{
    char* Pbase () const { return this->pbase (); }
    pos_type Seekoff(off_type off,
                     std::ios_base::seekdir way,
                     std::ios_base::openmode which) {
        return this->seekoff(off, way, which);
    }
};

int main ()
{
    // 512 is the default buffer size of the basic_stringbuf
    std::string s (512, 'a');

    std::stringbuf sbuf (s);
    PubBuf& buf = (PubBuf&)sbuf;

    buf.Seekoff(512, std::ios_base::beg, std::ios_base::out);
    std::streamsize res = buf.sputn (buf.Pbase (), 128);
    s.append (s.begin (), s.begin () + 128);
    buf.Seekoff(0, std::ios_base::beg, std::ios_base::out);

    const std::string& str = buf.str ();

    assert (res == 128);
    assert (str.size () == s.size ());
    assert (str == s);

    return 0;
} 
