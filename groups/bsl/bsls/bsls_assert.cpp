// bsls_assert.cpp                                                    -*-C++-*-
#include <bsls_assert.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_asserttestexception.h>
#include <bsls_platform.h>

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
#include <crtdbg.h>    // '_CrtSetReportMode', to suppress pop-ups

typedef unsigned long DWORD;

extern "C" {
    __declspec(dllimport) void __stdcall Sleep(DWORD dwMilliseconds);
};
#endif

#ifdef BSLS_ASSERT_NORETURN
#error BSLS_ASSERT_NORETURN must be a macro scoped locally to this file
#endif

// Note that a portable syntax for 'noreturn' will be available once we have
// access to conforming C++0x compilers.
//# define BSLS_ASSERT_NORETURN [[noreturn]]

#ifdef BSLS_PLATFORM_CMP_MSVC
#   define BSLS_ASSERT_NORETURN __declspec(noreturn)
#else
#   define BSLS_ASSERT_NORETURN
#endif

namespace BloombergLP {


static
void printError(const char *text, const char *file, int line)
    // Print a formatted error message to 'stderr' using the specified
    // expression 'text', 'file' name, and 'line' number.  If either
    // 'text' or 'file' is empty ("") or null (0), replace it with some
    // informative, "human-readable" text, before formatting.
{

    // Note that we deliberately use 'stdio' rather than 'iostream' to
    // avoid issues pertaining to memory allocation for file-scope 'static'
    // objects such as 'std::cerr'.

    if (!text) {
        text = "(* Unspecified Expression Text *)";
    }
    else if (!*text) {
        text = "(* Empty Expression Text *)";
    }

    if (!file) {
        file = "(* Unspecified File Name *)";
    }
    else if (!*file) {
        file = "(* Empty File Name *)";
    }

    std::fprintf(stderr,
                 "Assertion failed: %s, file %s, line %d\n", text, file, line);

    std::fflush(stderr);  // Not necessary for the default 'stderr', but just
                          // in case it has been reopened as a buffered stream.
}

namespace bsls {

                                // ------------
                                // class Assert
                                // ------------

// CLASS DATA
bsls::AtomicOperations::AtomicTypes::Pointer
    Assert::s_handler = {(void *) &Assert::failAbort};
bsls::AtomicOperations::AtomicTypes::Int Assert::s_lockedFlag = {0};

// CLASS METHODS
void Assert::setFailureHandlerRaw(Assert::Handler function)
{
    bsls::AtomicOperations::setPtrRelease(&s_handler, (void *) function);
}

void Assert::setFailureHandler(Assert::Handler function)
{
    if (!bsls::AtomicOperations::getIntRelaxed(&s_lockedFlag)) {
        setFailureHandlerRaw(function);
    }
}

void Assert::lockAssertAdministration()
{
    bsls::AtomicOperations::setIntRelaxed(&s_lockedFlag, 1);
}

Assert::Handler Assert::failureHandler()
{
    return (Handler) bsls::AtomicOperations::getPtrAcquire(&s_handler);
}

                       // Macro Dispatcher Method

BSLS_ASSERT_NORETURN_INVOKE_HANDLER
void Assert::invokeHandler(const char *text, const char *file, int line)
{
    failureHandler()(text, file, line);
}

                     // Standard Assertion-Failure Handlers

BSLS_ASSERT_NORETURN
void Assert::failAbort(const char *text, const char *file, int line)
{
    printError(text, file, line);


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


#ifdef BSLS_PLATFORM_OS_WINDOWS
    // The following configures the runtime library on how to report asserts,
    // errors, and warnings in order to avoid pop-up windows when 'abort' is
    // called.

    _CrtSetReportMode(_CRT_ASSERT, 0);
    _CrtSetReportMode(_CRT_ERROR,  0);
    _CrtSetReportMode(_CRT_WARN,   0);
#endif

    std::abort();
}

BSLS_ASSERT_NORETURN
void Assert::failSleep(const char *text, const char *file, int line)
{
    printError(text, file, line);

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
}

BSLS_ASSERT_NORETURN
void Assert::failThrow(const char *text, const char *file, int line)
{

#ifdef BDE_BUILD_TARGET_EXC
    if (!std::uncaught_exception()) {
        throw AssertTestException(text, file, line);
    }
    else {
        std::fprintf(stderr,
                "BSLS_ASSERTION ERROR: An uncaught exception is pending;"
                " cannot throw 'AssertTestException'.\n");
    }
#endif

    failAbort(text, file, line);
}

}  // close package namespace

#undef BSLS_ASSERT_NORETURN

namespace bsls {

                    // -------------------------------
                    // class AssertFailureHandlerGuard
                    // -------------------------------

AssertFailureHandlerGuard::AssertFailureHandlerGuard(Assert::Handler temporary)
: d_original(Assert::failureHandler())
{
    Assert::setFailureHandlerRaw(temporary);
}

AssertFailureHandlerGuard::~AssertFailureHandlerGuard()
{
    Assert::setFailureHandlerRaw(d_original);
}

}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
