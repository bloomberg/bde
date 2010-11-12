/************************************************************************
 *
 * 27.ostream.unformatted.stdcxx-626.cpp - regression test for STDCXX-626
 *
 * http://issues.apache.org/jira/browse/STDCXX-626
 *
 * $Id: 27.ostream.unformatted.stdcxx-626.cpp 612563 2008-01-16 20:31:30Z sebor $
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
 ************************************************************************/

#include <cassert>
#include <ostream>

int sync_calls;

struct MyBuf: std::streambuf
{
    virtual int sync () {
        ++sync_calls;
        return -1;
    }
};


int main ()
{
#if DRQS
    MyBuf buf;

    std::ostream os (&buf);

    // verify that ostream::flush() doesn't call streambuf::sync()
    // when the state of the stream isn't good

    os.setstate (os.badbit);

    os.flush ();

    assert (0 == sync_calls);

    os.setstate (os.eofbit);

    os.flush ();

    assert (0 == sync_calls);

    os.setstate (os.failbit);

    os.flush ();

    assert (0 == sync_calls);
#endif
    return 0;
}
