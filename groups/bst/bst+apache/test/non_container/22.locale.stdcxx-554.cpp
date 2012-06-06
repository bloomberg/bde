/************************************************************************
 *
 * 22.locale.stdcxx-554.cpp - the regression test for STDCXX-554 issue
 *
 * $Id: 22.locale.stdcxx-554.cpp 648752 2008-04-16 17:01:56Z faridz $
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

#include <cstring>   // for memset()
#include <cassert>   // for assert()

#include <locale>    // for std::moneypunct, std::messages

enum { fill = '\xdc' };

template <class charT>
void test_moneypunct (charT)
{
    typedef std::moneypunct <charT> PunctT;

    // Use a pointer to properly align buffer for placment new.
    union {
        void* ptr;
        char buf [sizeof (PunctT) + 1];
    } u;

    std::memset (u.buf, fill, sizeof (u.buf));

    PunctT* p = new (u.buf) PunctT ();

    assert (fill == u.buf [sizeof (PunctT)]);

#if TEST_RW_EXTENSIONS
    p->~PunctT ();
#endif // TEST_RW_EXTENSIONS
}

template <class charT>
void test_messages (charT)
{
    typedef std::messages <charT> MessagesT;

    union {
        void* ptr;
        char buf [sizeof (MessagesT) + 1];
    } u;

    std::memset (u.buf, fill, sizeof (u.buf));

    MessagesT* p = new (u.buf) MessagesT ();

    assert (fill == u.buf [sizeof (MessagesT)]);

#if TEST_RW_EXTENSIONS
    p->~MessagesT ();
#endif // TEST_RW_EXTENSIONS
}

int main (int, char* [])
{
    test_moneypunct (char ());
    test_messages (char ());

#ifndef _RWSTD_NO_WCHAR_T
    test_moneypunct (wchar_t ());
    test_messages (wchar_t ());
#endif

    return 0;
}
