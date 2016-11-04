// bsls_assert.cpp                                                    -*-C++-*-
#include <bsls_assert.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_asserttestexception.h>
#include <bsls_pointercastutil.h>
#include <bsls_types.h>
#include <bsls_log.h>
#include <bsls_logseverity.h>

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
#include <windows.h>   // IsDebbugerPresent
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

    bsls::Log::logFormattedMessage(
        bsls::LogSeverity::e_ERROR, file, line, "Assertion failed: %s", text);
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
    bsls::AtomicOperations::setPtrRelease(
        &s_handler, PointerCastUtil::cast<void *>(function));
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

#define IS_POWER_OF_TWO(X) (0 == ((X) & ((X) - 1)))

BSLS_ASSERT_NORETURN_INVOKE_HANDLER
void Assert::invokeHandler(const char *text, const char *file, int line)
{
    static AtomicOperations::AtomicTypes::Int failureReturnCount = {0};

    Assert::Handler failureHandlerPtr = failureHandler();

    failureHandlerPtr(text, file, line);

    // The failure handler should not return.  If a returning failure handler
    // has been installed, alert the user that the program is continuing to
    // run.

    unsigned count = static_cast<unsigned>(
                AtomicOperations::incrementIntNvAcqRel(&failureReturnCount));

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(IS_POWER_OF_TWO(count))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        // Log when 'count' is a power of 2.

        if (count == (1 << 30)) {
            // Avoid undefined behavior by resetting the counter.

            AtomicOperations::setInt(&failureReturnCount, 1 << 29);
        }

        Log::logFormattedMessage(LogSeverity::e_ERROR,
                                 file,
                                 line,
                                 "BSLS_ASSERT failure: '%s'",
                                 text);

        BSLS_LOG_ERROR("Bad 'bsls_assert' configuration: "
                       "violation handler at %p must not return.",
                       failureHandlerPtr);
    }
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
    if (!IsDebuggerPresent()) {
        _CrtSetReportMode(_CRT_ASSERT, 0);
        _CrtSetReportMode(_CRT_ERROR, 0);
        _CrtSetReportMode(_CRT_WARN, 0);
    }
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

    // We will never reach this line, but it is needed to let the compiler know
    // that this function does not return.

    std::abort();
}

BSLS_ASSERT_NORETURN
void Assert::failThrow(const char *text, const char *file, int line)
{

#ifdef BDE_BUILD_TARGET_EXC
    if (!std::uncaught_exception()) {
        throw AssertTestException(text, file, line);
    }
    else {
        bsls::Log::logMessage(bsls::LogSeverity::e_ERROR, file, line,
                "BSLS_ASSERT: An uncaught exception is pending;"
                " cannot throw 'AssertTestException'.");
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
// Copyright 2013 Bloomberg Finance L.P.
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
