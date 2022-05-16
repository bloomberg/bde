// bsls_assert.cpp                                                    -*-C++-*-
#include <bsls_assert.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_asserttestexception.h>
#include <bsls_bsltestutil.h>          // for testing only
#include <bsls_libraryfeatures.h>
#include <bsls_log.h>
#include <bsls_logseverity.h>
#include <bsls_pointercastutil.h>
#include <bsls_unspecifiedbool.h>      // for testing only
#include <bsls_types.h>                // for testing only

#include <exception>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER
#define BSLS_ASSERT_NORETURN_INVOKE_HANDLER BSLS_ANNOTATION_NORETURN
#else
#define BSLS_ASSERT_NORETURN_INVOKE_HANDLER
#endif

                         // -------------------------
                         // Language Contract Support
                         // -------------------------

#ifdef BSLS_ASSERT_USE_CONTRACTS

void handle_contract_violation(const std::contract_violation &violation)
    // Call 'bsls::Review::invokeLanguageContractHandler' or
    // 'bsls::Assert::invokeLanguageContractHandler' based on the semantic of
    // the specified 'violation'.  Note that this is the replacable function
    // that the Lock3 GCC compiler will look for when handling a contract
    // violation (see 'https://github.com/lock3/gcc/wiki/contracts').  Also
    // note that any component above 'bsls_assert' cannot define its own
    // violation handler, and components below 'bsls_assert' should not do so
    // outside of their test drivers (see 'bsls_review.t').
{
    if (violation.continuation_mode() ==
                   std::contract_violation_continuation_mode::MAYBE_CONTINUE) {
        BloombergLP::bsls::Review::invokeLanguageContractHandler(violation);
    }
    else {
        BloombergLP::bsls::Assert::invokeLanguageContractHandler(violation);
    }
}

#endif

namespace BloombergLP {
namespace bsls {

                              // ---------------
                              // Local Utilities
                              // ---------------
namespace {

void printError(const char *comment, const char *file, int line)
    // Log a formatted message with the contents of the specified 'comment',
    // 'file', 'line' number, and a severity of 'e_FATAL'.
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

    bsls::Log::logFormattedMessage(bsls::LogSeverity::e_FATAL,
                                   file,
                                   line,
                                   "Assertion failed: %s",
                                   comment);
}

void printError(const bsls::AssertViolation& violation)
    // Log a formatted message with the contents of the specified 'violation'
    // and a severity of 'e_FATAL'.
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


    bsls::Log::logFormattedMessage(bsls::LogSeverity::e_FATAL,
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

bsls::AtomicOperations::AtomicTypes::Pointer
    g_violationHandler = {(void *) &Assert::failByAbort};
    // assertion-failure handler function

bsls::AtomicOperations::AtomicTypes::Pointer
    g_handler = {(void *) NULL};
    // legacy assertion-failure handler

bsls::AtomicOperations::AtomicTypes::Int g_lockedFlag = {0};
    // lock to disable 'setFailureHandler'

}  // close unnamed namespace

                                // ------------
                                // class Assert
                                // ------------

// PUBLIC CLASS DATA

// Install 'k_permitOutOfPolicyReturningAssertionBuildKey' in writable memory
// for testing.
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
        &g_handler, PointerCastUtil::cast<void *>(function));
    bsls::AtomicOperations::setPtrRelease(
        &g_violationHandler, PointerCastUtil::cast<void*>(&failOnViolation));
}

void Assert::setViolationHandlerRaw(Assert::ViolationHandler function)
{
    // explicitly leave whatever might be there in legacyHandler to avoid
    // worrying about atomicity of the multiple pointer changes.
    bsls::AtomicOperations::setPtrRelease(
        &g_violationHandler, PointerCastUtil::cast<void *>(function));
}

void Assert::setFailureHandler(Assert::Handler function)
{
    if (!bsls::AtomicOperations::getIntRelaxed(&g_lockedFlag)) {
        setFailureHandlerRaw(function);
    }
}

void Assert::setViolationHandler(Assert::ViolationHandler function)
{
    if (!bsls::AtomicOperations::getIntRelaxed(&g_lockedFlag)) {
        setViolationHandlerRaw(function);
    }
}

void Assert::lockAssertAdministration()
{
    bsls::AtomicOperations::setIntRelaxed(&g_lockedFlag, 1);
}

Assert::ViolationHandler Assert::violationHandler()
{
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -CC01 // AIX only allows this cast as a C-Style cast
    return (ViolationHandler) bsls::AtomicOperations::getPtrAcquire(
                                                &g_violationHandler); // RETURN
    // BDE_VERIFY pragma: pop
}

Assert::Handler Assert::failureHandler()
{
    if (violationHandler() != &failOnViolation) {
        return NULL;                                                  // RETURN
    }
    else {
        // BDE_VERIFY pragma: push
        // BDE_VERIFY pragma: -CC01 // AIX only allows this as a C-Style cast
        return (Handler) bsls::AtomicOperations::getPtrAcquire(
                                                         &g_handler); // RETURN
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

        Log::logFormattedMessage(LogSeverity::e_FATAL,
                                 violation.fileName(),
                                 violation.lineNumber(),
                                 "BSLS_ASSERT failure: '%s'",
                                 violation.comment());

        // Do not use %p to print pointers.  On some platform it automatically
        // prefixes with '0x', on AIX it does not.

        BSLS_LOG_FATAL("Bad 'bsls_assert' configuration: "
                       "violation handler at 0x%llx must not return.",
                       reinterpret_cast<Types::Uint64>(failureHandlerPtr));
    }
}

BSLS_ANNOTATION_NORETURN
void Assert::invokeHandlerNoReturn(const bsls::AssertViolation &violation)
{
    Assert::ViolationHandler failureHandlerPtr = violationHandler();

    failureHandlerPtr(violation);

    // In order to be sure we don't return, abort immediately if the installed
    // handler does return.
    failByAbort(violation);
}

#ifdef BSLS_ASSERT_USE_CONTRACTS
void Assert::invokeLanguageContractHandler(
                                      const std::contract_violation &violation)
{
    BloombergLP::bsls::AssertViolation bslsViolation(
        violation.comment().data(),
        violation.file_name().data(),
        violation.line_number(),
        violation.assertion_level().data());
    BloombergLP::bsls::Assert::invokeHandler(bslsViolation);
}
#endif


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
#if defined(BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER)
    BSLS_ASSERT_INVOKE("BSLS_ASSERT return policy cannot be overridden with "
                       "BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER "
                       "defined");
#endif
    g_permitReturningHandlerRuntimeFlag = true;
}


                     // Standard Assertion-Failure Handlers

BSLS_ANNOTATION_NORETURN
void Assert::failByAbort(const bsls::AssertViolation& violation)
{
    printError(violation);

    AssertImpUtil::failByAbort();
}

BSLS_ANNOTATION_NORETURN
void Assert::failBySleep(const bsls::AssertViolation& violation)
{
    printError(violation);

    AssertImpUtil::failBySleep();
}

BSLS_ANNOTATION_NORETURN
void Assert::failByThrow(const bsls::AssertViolation& violation)
{

#ifdef BDE_BUILD_TARGET_EXC
# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    if (0 == std::uncaught_exceptions()) {
# else
    if (!std::uncaught_exception()) {
# endif
        throw AssertTestException(violation.comment(),
                                  violation.fileName(),
                                  violation.lineNumber(),
                                  violation.assertLevel());
    }
    else {
        bsls::Log::logMessage(bsls::LogSeverity::e_FATAL,
                              violation.fileName(),
                              violation.lineNumber(),
                              "BSLS_ASSERT: An uncaught exception is pending;"
                              " cannot throw 'AssertTestException'.");
    }
#endif

    failByAbort(violation);
}

                  // Deprecated Assertion-Failure Handlers

BSLS_ANNOTATION_NORETURN
void Assert::failAbort(const char *comment, const char *file, int line)
{
    printError(comment,file,line);

    AssertImpUtil::failByAbort();
}

BSLS_ANNOTATION_NORETURN
void Assert::failSleep(const char *comment, const char *file, int line)
{
    printError(comment,file,line);

    AssertImpUtil::failBySleep();
}

BSLS_ANNOTATION_NORETURN
void Assert::failThrow(const char *comment, const char *file, int line)
{

#ifdef BDE_BUILD_TARGET_EXC
# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    if (0 == std::uncaught_exceptions()) {
# else
    if (!std::uncaught_exception()) {
# endif
        throw AssertTestException(comment,
                                  file,
                                  line,
                                  "LEGACY");
    }
    else {
        bsls::Log::logMessage(bsls::LogSeverity::e_FATAL,
                              file,
                              line,
                              "BSLS_ASSERT: An uncaught exception is pending;"
                              " cannot throw 'AssertTestException'.");
    }
#endif

    failAbort(comment,file,line);
}

}  // close package namespace

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
    if (d_legacyOriginal != NULL) {
        Assert::setFailureHandlerRaw(d_legacyOriginal);
    }
    else {
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
