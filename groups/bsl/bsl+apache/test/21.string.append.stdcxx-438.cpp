/************************************************************************
*
* 21.string.append.stdcxx-438.cpp - regression test for STDCXX-438
*
* https://issues.apache.org/jira/browse/STDCXX-438
*
* $Id: 21.string.append.stdcxx-438.cpp 641433 2008-03-26 17:39:28Z faridz $
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
#include <iterator>
#include <new>
#include <string>

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE
   // disabled for compilers that can't reliably replace the operators

void* operator new (std::size_t n) throw (std::bad_alloc)
{
    void* const ptr = std::malloc (n + sizeof n);
    std::memset (ptr, -1, n);
    *(std::size_t*)ptr = n;
    return (std::size_t*)ptr + 1;
}

void operator delete (void *ptr) throw ()
{
    if (ptr) {
        std::memset (ptr, -1, *((std::size_t*)ptr - 1));
        std::free ((std::size_t*)ptr - 1);
    }
}

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

struct InputIterator: std::iterator<std::input_iterator_tag, char>
{
    const char *p_;
    InputIterator (const char *p): p_ (p) { }

    char operator* () const { return *p_; }
    InputIterator& operator++ () { return ++p_, *this; }
    InputIterator operator++ (int) {
        return ++p_, InputIterator (p_ - 1);
    }

    bool operator== (const InputIterator &rhs) const { return p_ == rhs.p_; }
#if !TEST_RW_EXTENSION
    // The RW imp relaxes the requirements on input iterators in that
    // it does not require that InputIterators have operator!=.
    // This "extension" is probably silly.  Every iterator (except
    // output iterators) should have operator!=, as per the standard.
    // Note that C++0x automatically provides operator!= for
    // EqualityComparible types that don't supply their own.
    bool operator!= (const InputIterator &rhs) const { return p_ != rhs.p_; }
#endif
};


int main ()
{
    const char s[] = "abc";

    {
        std::string str (s);

        const char* p0 = s + 1;
        const char* p1 = p0 + 1;

        const InputIterator first (p0);
        const InputIterator last (p1);

        str.append (first, last);

        assert ("abcb" == str);
    }

    {
        std::string str (s);

        const char* p0 = str.data () + 1;
        const char* p1 = p0 + 1;

        const InputIterator first (p0);
        const InputIterator last (p1);

        str.append (first, last);

        assert ("abcb" == str);
    }

    return 0;
}
