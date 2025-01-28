// bsls_fuzztest.cpp                                                  -*-C++-*-
#include <bsls_fuzztest.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_bslsourcenameparserutil.h>
#include <bsls_exceptionutil.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace BloombergLP {
namespace bsls {

const char *FuzzTestPreconditionTracker::s_file_p                     = "";
bool        FuzzTestPreconditionTracker::s_isInFirstPreconditionBlock = false;
int         FuzzTestPreconditionTracker::s_level                      = 0;

FuzzTestHandlerGuard
*FuzzTestHandlerGuard::s_currentFuzzTestHandlerGuard_p = NULL;

                     // ---------------------------------
                     // class FuzzTestPreconditionTracker
                     // ---------------------------------

// CLASS METHODS
void FuzzTestPreconditionTracker::handleAssertViolation(
                                              const AssertViolation& violation)
{
    if (s_isInFirstPreconditionBlock && (1 == s_level)) {
        // Prior to `END`, an assertion was triggered.  We still must verify
        // that this assertion did not come from a different component.  That
        // check is done in `handleException`.
        FuzzTestPreconditionException exception(violation.comment(),
                                                violation.fileName(),
                                                violation.lineNumber(),
                                                violation.assertLevel(),
                                                false);
        BSLS_THROW(exception);
    }
    else {
        Assert::ViolationHandler vh =
               FuzzTestHandlerGuard::instance()->getOriginalAssertionHandler();
        vh(violation);
    }
}

void FuzzTestPreconditionTracker::handleException(
                                const FuzzTestPreconditionException& exception)
{
    const char *exceptionComponent  = NULL;
    std::size_t exceptionNameLength = 0;
    const char *fileName            = exception.filename();

    // Check that the assertion/review failure comes from a component with a
    // valid BDE-style name.
    if (0 == bsls::BslSourceNameParserUtil::getComponentName(
                 &exceptionComponent,
                 &exceptionNameLength,
                 fileName)) {
        const char *currentComponent  = NULL;
        std::size_t currentNameLength = 0;
        // Check that the current component has a valid BDE-style name.
        if (0 == bsls::BslSourceNameParserUtil::getComponentName(
                     &currentComponent, &currentNameLength, s_file_p)) {
            // Finally, check if the two components are the same.
            if ((exceptionNameLength == currentNameLength) &&
                0 == std::strncmp(exceptionComponent,
                                  currentComponent,
                                  exceptionNameLength)) {
                return;                                               // RETURN
            }
        }
    }
    // If we reach here, the component names are different or not BDE-style.
    if (exception.isReview()) {
        Review::ViolationHandler vh =
                  FuzzTestHandlerGuard::instance()->getOriginalReviewHandler();
        vh(ReviewViolation(exception.expression(),
                           exception.filename(),
                           exception.lineNumber(),
                           exception.level(),
                           0));
    }
    else {
        Assert::ViolationHandler vh =
               FuzzTestHandlerGuard::instance()->getOriginalAssertionHandler();
        vh(AssertViolation(exception.expression(),
                           exception.filename(),
                           exception.lineNumber(),
                           exception.level()));
    }
}

void FuzzTestPreconditionTracker::handlePreconditionsBegin()
{
    ++s_level;
}

void FuzzTestPreconditionTracker::handlePreconditionsEnd()
{
    BSLS_ASSERT(0 < s_level);
    if (0 == --s_level) {
        s_isInFirstPreconditionBlock = false;
    }
}

void FuzzTestPreconditionTracker::handleReviewViolation(
                                              const ReviewViolation& violation)
{
    if (s_isInFirstPreconditionBlock && (1 == s_level)) {
        // Prior to `END`, a review was triggered.  We still must verify that
        // this review did not come from a different component.  That check is
        // done in `handleException`.
        FuzzTestPreconditionException exception(violation.comment(),
                                                violation.fileName(),
                                                violation.lineNumber(),
                                                violation.reviewLevel(),
                                                true);
        BSLS_THROW(exception);
    }
    else {
        // We want the execution to end here, so we call the original handler
        // for assertion violations (not review violations).  This is because
        // the default review violation handler logs the violation and
        // continues.  Note that this is acceptable because this function is
        // intended to be invoked in test drivers only.
        Assert::ViolationHandler vh =
               FuzzTestHandlerGuard::instance()->getOriginalAssertionHandler();
        vh(AssertViolation(violation.comment(),
                           violation.fileName(),
                           violation.lineNumber(),
                           violation.reviewLevel()));
    }
}

void FuzzTestPreconditionTracker::initStaticState(const char *fileName)
{
    s_file_p                     = fileName;
    s_isInFirstPreconditionBlock = true;
    s_level                      = 0;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
