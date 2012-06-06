/************************************************************************
 *
 * 26.valarray.unary.stdcxx-314.cpp - regression test for STDCXX-314
 *
 *   http://issues.apache.org/jira/browse/STDCXX-314
 *
 * $Id: 26.valarray.unary.stdcxx-314.cpp 590132 2007-10-30 16:01:33Z faridz $
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
#include <cstdlib>
#include <cstring>
#include <new>
#include <valarray>


void* operator new (std::size_t s) throw (std::bad_alloc)
{
    return std::memset (std::malloc (s), 0x55, s);
}

void operator delete (void *ptr) throw ()
{
    std::free (ptr);
}


struct S {
    const S* const self;
    S (): self (this) { }
    S (const S &s): self (this) { assert (&s == s.self); }
    ~S () { assert (this == self); }

    S operator+ () const { assert (this == self); return *this; }
    S operator- () const { assert (this == self); return *this; }
    S operator~ () const { assert (this == self); return *this; }
    bool operator! () const { assert (this == self); return false; }

void operator= (const S &s) { assert (this == self && &s == s.self); }
};

int main ()
{
    const std::valarray<S> a (2);

    +a;
    -a;
    ~a;
    !a;
}
