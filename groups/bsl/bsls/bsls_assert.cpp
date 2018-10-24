// bsls_assert.cpp                                                    -*-C++-*-
#include <bsls_assert.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assertimputil.h>
#include <bsls_asserttestexception.h>
#include <bsls_keyword.h>             // for testing only
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

namespace {

static
void printError(const char *comment, const char *file, int line)
    // Log a formatted message with the contents of the specified 'comment',
    // 'file', 'line' number, and a severity of 'e_ERROR'.
{
    if (!comment) {
        comment = "(* Unspecified Comment Text *)";
    }
    else if (!*comment) {
        comment = "(* Empty Comment Text *)";
    }

    if (!file) {
        file = "(* Unspecified File Name *)";
    }
    else if (!*file) {
        file = "(* Empty File Name *)";
    }

    bsls::Log::logFormattedMessage(bsls::LogSeverity::e_ERROR,
                                   file,
                                   line,
                                   "Assertion failed: %s",
                                   comment);
}

static
void printError(const bsls::AssertViolation& violation)
    // Log a formatted message with the contents of the specified 'violation'
    // and a severity of 'e_ERROR'.
{
    const char *comment = violation.comment();
    if (!comment) {
        comment = "(* Unspecified Comment Text *)";
    }
    else if (!*comment) {
        comment = "(* Empty Comment Text *)";
    }

    const char *file = violation.fileName();
    if (!file) {
        file = "(* Unspecified File Name *)";
    }
    else if (!*file) {
        file = "(* Empty File Name *)";
    }

    // Note: level not in output until we are ready to change what gets logged
    // by failed assertions
//    const char *level = violation.assertLevel();
//    if (!level) {
//        level = "(* Unspecified Level *)";
//    }
//    else if (!*level) {
//        level = "(* Empty Level *)";
//    }

    int line = violation.lineNumber();


    bsls::Log::logFormattedMessage(bsls::LogSeverity::e_ERROR,
                                   file,
                                   line,
                                   "Assertion failed: %s",
                                   comment);
}

// STATIC DATA

static const int k_KeyBufferSize = 29;
static char      g_KeyBuffer[k_KeyBufferSize] = "No";
    // Buffer for 'k_permitOutOfPolicyReturningAssertionBuildKey'.

static bool g_permitReturningHandlerRuntimeFlag = false;
    // Flag for method 'permitOutOfPolicyReturningFailureHandler'.

}  // close unnamed namespace

namespace bsls {

                          // ---------------------
                          // class AssertViolation
                          // ---------------------

// CREATORS
AssertViolation::AssertViolation(const char *comment,
                                 const char *fileName,
                                 int         lineNumber,
                                 const char *assertLevel)
: d_comment_p(comment)
, d_fileName_p(fileName)
, d_lineNumber(lineNumber)
, d_assertLevel_p(assertLevel)
{
}

                              // ------------
                              // class Assert
                              // ------------

// CLASS DATA
bsls::AtomicOperations::AtomicTypes::Pointer
    Assert::s_violationHandler = {(void *) &Assert::failByAbort};
bsls::AtomicOperations::AtomicTypes::Pointer
    Assert::s_handler = {(void *) NULL};

bsls::AtomicOperations::AtomicTypes::Int Assert::s_lockedFlag = {0};

// Install 'k_permitOutOfPolicyReturningAssertionBuildKey' in writable memory
// for testing.

// PUBLIC CLASS DATA
const char *Assert::k_permitOutOfPolicyReturningAssertionBuildKey =
                                                                   g_KeyBuffer;

// PUBLIC CONSTANTS
const char Assert::k_LEVEL_SAFE[]   = "SAF";
const char Assert::k_LEVEL_OPT[]    = "OPT";
const char Assert::k_LEVEL_ASSERT[] = "DBG";
const char Assert::k_LEVEL_INVOKE[] = "INV";

// CLASS METHODS
void Assert::failOnViolation(const bsls::AssertViolation& violation)
{
    Assert::Handler failureHandlerPtr = failureHandler();
    failureHandlerPtr(violation.comment(),
                      violation.fileName(),
                      violation.lineNumber());
}

void Assert::setFailureHandlerRaw(Assert::Handler function)
{
    bsls::AtomicOperations::setPtrRelease(
        &s_handler, PointerCastUtil::cast<void *>(function));
    bsls::AtomicOperations::setPtrRelease(
        &s_violationHandler, PointerCastUtil::cast<void*>(&failOnViolation));
}

void Assert::setViolationHandlerRaw(Assert::ViolationHandler function)
{
    // explicitly leave whatever might be there in legacyHandler to avoid
    // worrying about atomicity of the multiple pointer changes.
    bsls::AtomicOperations::setPtrRelease(
        &s_violationHandler, PointerCastUtil::cast<void *>(function));
}

void Assert::setFailureHandler(Assert::Handler function)
{
    if (!bsls::AtomicOperations::getIntRelaxed(&s_lockedFlag)) {
        setFailureHandlerRaw(function);
    }
}

void Assert::setViolationHandler(Assert::ViolationHandler function)
{
    if (!bsls::AtomicOperations::getIntRelaxed(&s_lockedFlag)) {
        setViolationHandlerRaw(function);
    }
}

void Assert::lockAssertAdministration()
{
    bsls::AtomicOperations::setIntRelaxed(&s_lockedFlag, 1);
}

Assert::ViolationHandler Assert::violationHandler()
{
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -CC01 // AIX only allows this cast as a C-Style cast
    return (ViolationHandler) bsls::AtomicOperations::getPtrAcquire(
                                                &s_violationHandler); // RETURN
    // BDE_VERIFY pragma: pop
}

Assert::Handler Assert::failureHandler()
{
    if (violationHandler() != &failOnViolation)
    {
        return NULL;                                                  // RETURN
    }
    else
    {
        // BDE_VERIFY pragma: push
        // BDE_VERIFY pragma: -CC01 // AIX only allows this as a C-Style cast
        return (Handler) bsls::AtomicOperations::getPtrAcquire(
                                                         &s_handler); // RETURN
        // BDE_VERIFY pragma: pop
    }
}

                       // Macro Dispatcher Method

#define IS_POWER_OF_TWO(X) (0 == ((X) & ((X) - 1)))

BSLS_ASSERT_NORETURN_INVOKE_HANDLER
void Assert::invokeHandler(const char *text, const char *file, int line)
{
    AssertViolation violation(text, file, line, k_LEVEL_INVOKE);
    invokeHandler(violation);
}

void Assert::invokeHandler(const bsls::AssertViolation& violation)
{
    static AtomicOperations::AtomicTypes::Int failureReturnCount = {0};

    Assert::ViolationHandler failureHandlerPtr = violationHandler();

    failureHandlerPtr(violation);

    if (abortUponReturningAssertionFailureHandler()) {
        failByAbort(violation);
    }

    // The failure handler should not return.  If a returning failure handler
    // has been installed and the out of policy override has been enabled,
    // alert the user that the program is continuing to run.

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
                                 violation.fileName(),
                                 violation.lineNumber(),
                                 "BSLS_ASSERT failure: '%s'",
                                 violation.comment());

        // Do not use %p to print pointers.  On some platform it automatically
        // prefixes with '0x', on AIX it does not.

        BSLS_LOG_ERROR("Bad 'bsls_assert' configuration: "
                       "violation handler at 0x%llx must not return.",
                       reinterpret_cast<Types::Uint64>(failureHandlerPtr));
    }
}

                    // Assertion Handler Policy Enforcement

bool Assert::abortUponReturningAssertionFailureHandler()
{
    if (!g_permitReturningHandlerRuntimeFlag) {
        return true;                                                  // RETURN
    }

    // Encoding the key string as integers below allows us to search for the
    // unique C-string text "bsls-PermitOutOfPolicyReturn" within a task.

    static const int permissionKey[k_KeyBufferSize] =
    { 'b', 's', 'l', 's', '-', 'P', 'e', 'r', 'm', 'i', 't', 'O', 'u', 't',
    'O', 'f', 'P', 'o', 'l', 'i', 'c', 'y', 'R', 'e', 't', 'u', 'r', 'n',
    0 };

    const int  *keyPtr = permissionKey;
    const char *permissionText = k_permitOutOfPolicyReturningAssertionBuildKey;
    while (*permissionText && (*permissionText == *keyPtr)) {
        ++permissionText;
        ++keyPtr;
    }
    return *permissionText != *keyPtr;
}

void Assert::permitOutOfPolicyReturningFailureHandler()
{
    g_permitReturningHandlerRuntimeFlag = true;
}


                     // Standard Assertion-Failure Handlers

BSLS_ASSERT_NORETURN
void Assert::failByAbort(const bsls::AssertViolation& violation)
{
    printError(violation);

    AssertImpUtil::failByAbort();
}

BSLS_ASSERT_NORETURN
void Assert::failBySleep(const bsls::AssertViolation& violation)
{
    printError(violation);

    AssertImpUtil::failBySleep();
}

BSLS_ASSERT_NORETURN
void Assert::failByThrow(const bsls::AssertViolation& violation)
{

#ifdef BDE_BUILD_TARGET_EXC
    if (!std::uncaught_exception()) {
        throw AssertTestException(violation.comment(),
                                  violation.fileName(),
                                  violation.lineNumber(),
                                  violation.assertLevel());
    }
    else {
        bsls::Log::logMessage(bsls::LogSeverity::e_ERROR,
                              violation.fileName(),
                              violation.lineNumber(),
                              "BSLS_ASSERT: An uncaught exception is pending;"
                              " cannot throw 'AssertTestException'.");
    }
#endif

    failByAbort(violation);
}

                  // Deprecated Assertion-Failure Handlers

BSLS_ASSERT_NORETURN
void Assert::failAbort(const char *comment, const char *file, int line)
{
    printError(comment,file,line);

    AssertImpUtil::failByAbort();
}

BSLS_ASSERT_NORETURN
void Assert::failSleep(const char *comment, const char *file, int line)
{
    printError(comment,file,line);

    AssertImpUtil::failBySleep();
}

BSLS_ASSERT_NORETURN
void Assert::failThrow(const char *comment, const char *file, int line)
{

#ifdef BDE_BUILD_TARGET_EXC
    if (!std::uncaught_exception()) {
        throw AssertTestException(comment,
                                  file,
                                  line,
                                  "LEGACY");
    }
    else {
        bsls::Log::logMessage(bsls::LogSeverity::e_ERROR,
                              file,
                              line,
                              "BSLS_ASSERT: An uncaught exception is pending;"
                              " cannot throw 'AssertTestException'.");
    }
#endif

    failAbort(comment,file,line);
}

}  // close package namespace

#undef BSLS_ASSERT_NORETURN

namespace bsls {

                     // -------------------------------
                     // class AssertFailureHandlerGuard
                     // -------------------------------

AssertFailureHandlerGuard::AssertFailureHandlerGuard(Assert::Handler temporary)
: d_original(Assert::violationHandler())
, d_legacyOriginal(Assert::failureHandler())
{
    Assert::setFailureHandlerRaw(temporary);
}

AssertFailureHandlerGuard::AssertFailureHandlerGuard(
                                            Assert::ViolationHandler temporary)
: d_original(Assert::violationHandler())
, d_legacyOriginal(Assert::failureHandler())
{
    Assert::setViolationHandlerRaw(temporary);
}

AssertFailureHandlerGuard::~AssertFailureHandlerGuard()
{
    if (d_legacyOriginal != NULL)
    {
        Assert::setFailureHandlerRaw(d_legacyOriginal);
    }
    else
    {
        Assert::setViolationHandlerRaw(d_original);
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
