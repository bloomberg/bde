/************************************************************************
 *
 * 26.valarray.transcend.stdcxx-315.cpp - regression test for STDCXX-315
 *
 *   http://issues.apache.org/jira/browse/STDCXX-315
 *
 * $Id: 26.valarray.transcend.stdcxx-315.cpp 589882 2007-10-29 22:00:32Z sebor $
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
#include <cstdlib>
#include <cstring>
#include <new>
#include <valarray>

void* operator new (std::size_t s) throw (std::bad_alloc)
{
    return std::memset (std::malloc (s), 0x55, s);
}

bool pass = true;

struct S {
    const S* const self;
    S (): self (this) { }
    S (const S &s): self (this) { pass = pass && &s == s.self; }
    S (double): self (this) { }
    ~S () { pass = pass && this == self; }
    operator double () const { pass = pass && this == self; return 1.0; }

    void operator=(const S &s) {
        pass = pass && &s == s.self && this == self;
    }
    S operator- () const { pass = pass && this == self; return *this; }
    bool operator< (const S &s) const {
        pass = pass && &s == s.self && this == self;
        return false;
    }
};


int main ()
{
    const S s;
    const std::valarray<S> a (2);

    int nfails = 0;

#define TEST(fun, args) \
    pass = true; \
    fun args; \
    if (!pass) { \
        std::fprintf (stderr, "std::%s uses uninitialized storage\n", \
                      #fun); \
        ++nfails; \
    } (void)0

TEST (abs, (a));
    TEST (acos, (a));
    TEST (asin, (a));
    TEST (atan, (a));
    TEST (atan2, (a, a));
    TEST (atan2, (a, s));
    TEST (atan2, (s, a));
    TEST (cos, (a));
    TEST (cosh, (a));
    TEST (exp, (a));
    TEST (log, (a));
    TEST (log10, (a));
    TEST (pow, (a, a));
    TEST (pow, (a, s));
    TEST (pow, (s, a));
    TEST (sin, (a));
    TEST (sinh, (a));
    TEST (sqrt, (a));
    TEST (tan, (a));
    TEST (tanh, (a));

    assert (0 == nfails);

    return 0;
}
