// bsls_review.cpp                                                    -*-C++-*-
#include <bsls_review.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_asserttestexception.h>
#include <bsls_bsltestutil.h>         // for testing only
#include <bsls_libraryfeatures.h>
#include <bsls_log.h>
#include <bsls_logseverity.h>
#include <bsls_pointercastutil.h>
#include <bsls_stackaddressutil.h>
#include <bsls_types.h>               // for testing only

#include <exception>

#ifdef BSLS_ASSERT_USE_CONTRACTS
#include <map>
#include <utility>
#include <string>
#include <mutex>
#endif

namespace BloombergLP {
namespace bsls {

namespace {

// STATIC HELPER FUNCTIONS
void printError(const bsls::ReviewViolation& violation)
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

    const char *level = violation.reviewLevel();
    if (!level) {
        level = "(* Unspecified Level *)";
    }
    else if (!*level) {
        level = "(* Empty Level *)";
    }

    int line = violation.lineNumber();

    bsls::Log::logFormattedMessage(bsls::LogSeverity::e_ERROR,
                                   file,
                                   line,
                                   "BSLS_REVIEW failure: (level:%s)"
                                   " '%s'",
                                   level,
                                   comment);
}

// STATIC DATA
bsls::AtomicOperations::AtomicTypes::Pointer
    g_violationHandler = {(void *) &Review::failByLog};
    // review-failure handler function

bsls::AtomicOperations::AtomicTypes::Int g_lockedFlag = {0};
    // lock to disable 'setViolationHandler'

}  // close unnamed namespace

                                // ------------
                                // class Review
                                // ------------

// PUBLIC CONSTANTS
const char Review::k_LEVEL_SAFE[]   = "R-SAF";
const char Review::k_LEVEL_OPT[]    = "R-OPT";
const char Review::k_LEVEL_REVIEW[] = "R-DBG";
const char Review::k_LEVEL_INVOKE[] = "R-INV";

// PRIVATE CLASS METHODS
void Review::setViolationHandlerRaw(Review::ViolationHandler function)
{
    bsls::AtomicOperations::setPtrRelease(
        &g_violationHandler, PointerCastUtil::cast<void *>(function));
}

// CLASS METHODS
                      // Administrative Methods

void Review::lockReviewAdministration()
{
    bsls::AtomicOperations::setIntRelaxed(&g_lockedFlag, 1);
}

void Review::setViolationHandler(Review::ViolationHandler function)
{
    if (!bsls::AtomicOperations::getIntRelaxed(&g_lockedFlag)) {
        setViolationHandlerRaw(function);
    }
}

Review::ViolationHandler Review::violationHandler()
{
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -CC01 // AIX only allows this cast as a C-style cast
    return (ViolationHandler) bsls::AtomicOperations::getPtrAcquire(
                                                &g_violationHandler); // RETURN
    // BDE_VERIFY pragma: pop
}

                      // Dispatcher Methods (called from within macros)

int Review::updateCount(Count *count)
{
    int lastCount = bsls::AtomicOperations::incrementIntNvAcqRel(count);
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY( (1 << 30) == lastCount) ) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        // avoid integer overflow (undefined behavior) and keep the period high
        // by dropping the count back down
        bsls::AtomicOperations::subtractIntNvAcqRel(count, 1 << 29);
    }
    return lastCount;
}

void Review::invokeHandler(const ReviewViolation& violation)
{
    Review::ViolationHandler violationHandlerPtr = violationHandler();

    violationHandlerPtr(violation);
}

#ifdef BSLS_ASSERT_USE_CONTRACTS
void Review::invokeLanguageContractHandler(
                                      const std::contract_violation &violation)
{
    static std::map<std::pair<std::string,int>,
                              Review::Count>              counts;
    static std::mutex                                     counts_mutex;

    Review::Count *myCount;
    {
        std::lock_guard<std::mutex> guard(counts_mutex);
        std::pair<const std::string,int> key(violation.file_name(),
                                             violation.line_number());
        myCount = &counts[key];
    }
    int count = Review::updateCount(myCount);

    BloombergLP::bsls::ReviewViolation bslsViolation(
        violation.comment().data(),
        violation.file_name().data(),
        violation.line_number(),
        violation.assertion_level().data(),
        count);
    BloombergLP::bsls::Review::invokeHandler(bslsViolation);
}
#endif

                      // Standard Review-Failure Handlers

void Review::failByAbort(const ReviewViolation& violation)
{
    printError(violation);
    AssertImpUtil::failByAbort();
}

#define IS_POWER_OF_TWO(X) (0 == ((X) & ((X) - 1)))
#define MAX_STACK_SIZE 1024

void Review::failByLog(const ReviewViolation& violation)
{
    int count = violation.count();
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(IS_POWER_OF_TWO(count))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        int skipped = count - (count >> 1) - 1;

#if defined(BSLS_PLATFORM_OS_WINDOWS)
        const char *stack = "";
#else
        char stack[MAX_STACK_SIZE + 1] = { ' ', 0 };
        bsls::StackAddressUtil::formatCheapStack(&stack[1],MAX_STACK_SIZE);
#endif

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

        const char *level = violation.reviewLevel();
        if (!level) {
            level = "(* Unspecified Level *)";
        }
        else if (!*level) {
            level = "(* Empty Level *)";
        }

        if (skipped > 0) {
            Log::logFormattedMessage(LogSeverity::e_ERROR,
                                     file,
                                     violation.lineNumber(),
                                     "BSLS_REVIEW failure: (level:%s"
                                     " skipped:%d) '%s'%s",
                                     level,
                                     skipped,
                                     comment,
                                     stack);
        }
        else {
            Log::logFormattedMessage(LogSeverity::e_ERROR,
                                     file,
                                     violation.lineNumber(),
                                     "BSLS_REVIEW failure: (level:%s) '%s'%s",
                                     level,
                                     comment,
                                     stack);
        }
    }
}

void Review::failBySleep(const ReviewViolation& violation)
{
    printError(violation);
    AssertImpUtil::failBySleep();
}

void Review::failByThrow(const ReviewViolation& violation)
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
                                  violation.reviewLevel());
    }
    else {
        bsls::Log::logMessage(bsls::LogSeverity::e_ERROR,
                              violation.fileName(),
                              violation.lineNumber(),
                              "BSLS_REVIEW: An uncaught exception is pending;"
                              " cannot throw 'AssertTestException'.");
    }
#endif

    failByAbort(violation);
}

                      // -------------------------------
                      // class ReviewFailureHandlerGuard
                      // -------------------------------

// CREATORS
ReviewFailureHandlerGuard::ReviewFailureHandlerGuard(
                                            Review::ViolationHandler temporary)
: d_original(Review::violationHandler())
{
    Review::setViolationHandlerRaw(temporary);
}

ReviewFailureHandlerGuard::~ReviewFailureHandlerGuard()
{
    Review::setViolationHandlerRaw(d_original);
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
