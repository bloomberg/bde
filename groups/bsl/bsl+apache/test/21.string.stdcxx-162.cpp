/************************************************************************
*
* 21.string.stdcxx-162.cpp - regression test for STDCXX-162
*
* https://issues.apache.org/jira/browse/STDCXX-162
*
* $Id: 21.string.stdcxx-162.cpp 648752 2008-04-16 17:01:56Z faridz $
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
#include <string>

#ifdef _RWSTD_POSIX_THREADS
#  include <pthread.h>
#  define MUTEX   pthread_mutex_t
#elif _RWSTD_SOLARIS_THREADS
#  include <thread.h>
#  define MUTEX   mutex_t
#endif   // _RWSTD_POSIX_THREADS

int main ()
{
#if TEST_RW_EXTENSIONS
    typedef std::char_traits<char> Traits;
    typedef std::allocator<char> Allocator;
    typedef __rw::__string_ref<char, Traits, Allocator> strref;

    struct SmallRef {
        int ref_count;
        std::string::size_type capacity;
        std::string::size_type size;
    };

    // verify that the reference-counted string body isn't bigger
    // than the struct above
    assert (sizeof (strref) <= sizeof (SmallRef));

#ifdef MUTEX

    struct LargeRef {
        MUTEX                  mutex;
        int                    ref_count;
        std::string::size_type capacity;
        std::string::size_type size;
    };

    // verify that the reference-counted string body is smaller
    // than the struct containing a full-blown mutex object above
    assert (sizeof (strref) < sizeof (LargeRef));

#endif   // MUTEX

#endif // TEST_RW_EXTENSIONS
    return 0;
}
