// bsls_assertimputil.cpp                                             -*-C++-*-
#include <bsls_assertimputil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <exception>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef BSLS_PLATFORM_OS_AIX
#include <signal.h>
#endif

#ifdef BSLS_PLATFORM_OS_UNIX
#include <unistd.h>    // 'sleep'
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>   // IsDebuggerPresent
#include <crtdbg.h>    // '_CrtSetReportMode', to suppress pop-ups

typedef unsigned long DWORD;

extern "C" {
    __declspec(dllimport) void __stdcall Sleep(DWORD dwMilliseconds);
};
#endif

#ifdef BSLS_ASSERTIMPUTIL_NORETURN
#error BSLS_ASSERTIMPUTIL_NORETURN must be a macro scoped locally to this file
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN)
#   define BSLS_ASSERTIMPUTIL_NORETURN [[noreturn]]
#elif defined(BSLS_PLATFORM_CMP_MSVC)
#   define BSLS_ASSERTIMPUTIL_NORETURN __declspec(noreturn)
#else
#   define BSLS_ASSERTIMPUTIL_NORETURN
#endif

namespace BloombergLP {
namespace bsls {

                          // --------------------
                          // struct AssertImpUtil
                          // --------------------

BSLS_ASSERTIMPUTIL_NORETURN
void AssertImpUtil::failByAbort()
{
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // See DRQS 8923441: The following is a work-around for a Fortran compiler
    // bug.
#endif // BDE_OMIT_INTERNAL_DEPRECATED

#ifdef BSLS_PLATFORM_OS_AIX
    sigset_t newset;
    sigemptyset(&newset);
    sigaddset(&newset, SIGABRT);
#if defined(BDE_BUILD_TARGET_MT)
    pthread_sigmask(SIG_UNBLOCK, &newset, 0);
#else
    sigprocmask(SIG_UNBLOCK, &newset, 0);
#endif
#endif

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // See DRQS 13882128: Note that (according to Oleg) the first line alone
    // may be sufficient.
#endif // BDE_OMIT_INTERNAL_DEPRECATED

#ifdef BSLS_PLATFORM_OS_WINDOWS
    // The following configures the runtime library on how to report asserts,
    // errors, and warnings in order to avoid pop-up windows when 'abort' is
    // called.
    if (!IsDebuggerPresent()) {
        _CrtSetReportMode(_CRT_ASSERT, 0);
        _CrtSetReportMode(_CRT_ERROR, 0);
        _CrtSetReportMode(_CRT_WARN, 0);
    }
#endif

    std::abort();
}

BSLS_ASSERTIMPUTIL_NORETURN
void AssertImpUtil::failBySleep()
{
    volatile int sleepDuration = 1;

    while (1 == sleepDuration) {

#if defined(BSLS_PLATFORM_OS_UNIX)
        sleep(sleepDuration);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
        Sleep(sleepDuration * 1000);  // milliseconds
#else
        #error "Do not know how to sleep on this platform."
#endif

    }

    // We will never reach this line, but it is needed to let the compiler know
    // that this function does not return.

    std::abort();
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
