// bslmt_testutil.cpp                                                 -*-C++-*-
#include <bslmt_testutil.h>

#include <bslmt_once.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assert.h>

char bloomberglp_bslmt_testutil_guard_object = 0;

namespace BloombergLP {
namespace bslmt {

                              // ---------------
                              // struct TestUtil
                              // ---------------

// CLASS DATA
TestUtil::Func TestUtil::s_func = 0;

// CLASS METHODS
void *TestUtil::callFunc(void *arg)
{
    BSLS_ASSERT(s_func);

    return (*s_func)(arg);
}

bslmt::Mutex& TestUtil::outputMutexSingleton_impl()
{
    static bslmt::Mutex *mutex_p;

    BSLMT_ONCE_DO {
        static bslmt::Mutex mutex;

        mutex_p = &mutex;
    }

    return *mutex_p;
}

void TestUtil::setFunc(TestUtil::Func func)
{
    s_func = func;
}

                          // ---------------------------
                          // class TestUtil::GuardObject
                          // ---------------------------

TestUtil::GuardObject::GuardObject()
: d_mutex_p(&TestUtil::outputMutexSingleton_impl())
{
    d_mutex_p->lock();
}

TestUtil::GuardObject::~GuardObject()
{
    d_mutex_p->unlock();
}

                          // ---------------------------
                          // class TestUtil::NestedGuard
                          // ---------------------------

// CREATORS
TestUtil::NestedGuard::NestedGuard(char *)
: d_mutex_p(&TestUtil::outputMutexSingleton_impl())
{
    d_mutex_p->lock();
}

TestUtil::NestedGuard::NestedGuard(GuardObject *)
: d_mutex_p(0)
{}

TestUtil::NestedGuard::~NestedGuard()
{
    if (d_mutex_p) {
        d_mutex_p->unlock();
    }
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
