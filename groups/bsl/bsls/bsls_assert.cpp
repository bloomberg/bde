// bsls_assert.cpp                                                    -*-C++-*-
#include <bsls_assert.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assertimputil.h>
#include <bsls_asserttestexception.h>
#include <bsls_log.h>
#include <bsls_logseverity.h>
#include <bsls_pointercastutil.h>

#include <exception>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef BSLS_ASSERT_NORETURN
#error BSLS_ASSERT_NORETURN must be a macro scoped locally to this file
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN)
#   define BSLS_ASSERT_NORETURN [[noreturn]]
#elif defined(BSLS_PLATFORM_CMP_MSVC)
#   define BSLS_ASSERT_NORETURN __declspec(noreturn)
#else
#   define BSLS_ASSERT_NORETURN
#endif

#ifdef BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER
#define BSLS_ASSERT_NORETURN_INVOKE_HANDLER  BSLS_ASSERT_NORETURN
#else
#define BSLS_ASSERT_NORETURN_INVOKE_HANDLER
#endif

namespace BloombergLP {

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// We want to print the error message to 'stderr', not 'stdout'.  The old
// documentation for 'printError' is:
//..
//  Print a formatted error message to standard output.  (Most Bloomberg
//  processes will send standard output to a log file.)
//..
// TBD: find out whether 'stderr' goes to 'act.log'.
#endif // BDE_OMIT_INTERNAL_DEPRECATED

static
void printError(const char *text, const char *file, int line)
    // Print a formatted error message to 'stderr' using the specified
    // expression 'text', 'file' name, and 'line' number.  If either 'text' or
    // 'file' is empty ("") or null (0), replace it with some informative,
    // "human-readable" text, before formatting.
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

// PUBLIC CONSTANTS
const char Assert::k_LEVEL_SAFE[]   = "SAF";
const char Assert::k_LEVEL_OPT[]    = "OPT";
const char Assert::k_LEVEL_ASSERT[] = "DBG";
const char Assert::k_LEVEL_INVOKE[] = "INV";

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
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -CC01 // AIX only allows this cast as a C-Style cast
    return (Handler) bsls::AtomicOperations::getPtrAcquire(&s_handler);
    // BDE_VERIFY pragma: pop
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

        // Do not use %p to print pointers.  On some platform it automatically
        // prefixes with '0x', on AIX it does not.

        BSLS_LOG_ERROR("Bad 'bsls_assert' configuration: "
                       "violation handler at 0x%llx must not return.",
                       reinterpret_cast<Types::Uint64>(failureHandlerPtr));
    }
}

                     // Standard Assertion-Failure Handlers

BSLS_ASSERT_NORETURN
void Assert::failAbort(const char *text, const char *file, int line)
{
    printError(text, file, line);

    AssertImpUtil::failAbort();
}

BSLS_ASSERT_NORETURN
void Assert::failSleep(const char *text, const char *file, int line)
{
    printError(text, file, line);

    AssertImpUtil::failSleep();
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
