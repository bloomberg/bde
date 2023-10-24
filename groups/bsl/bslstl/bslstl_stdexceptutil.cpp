// bslstl_stdexceptutil.cpp                                           -*-C++-*-
#include <bslstl_stdexceptutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_exceptionutil.h>
#include <bsls_log.h>
#include <bsls_platform.h>
#include <bsls_stackaddressutil.h>

#include <stdexcept>
#include <string>

namespace {
namespace u {

using namespace BloombergLP;
using namespace bslstl;

// TYPES
enum { k_MAX_BYTES_PER_PTR = sizeof(void *) * 2 };

// DATA
StdExceptUtil::PreThrowHook runtimeErrorHook;
StdExceptUtil::PreThrowHook logicErrorHook;
StdExceptUtil::PreThrowHook domainErrorHook;
StdExceptUtil::PreThrowHook invalidArgumentHook;
StdExceptUtil::PreThrowHook lengthErrorHook;
StdExceptUtil::PreThrowHook outOfRangeHook;
StdExceptUtil::PreThrowHook rangeErrorHook;
StdExceptUtil::PreThrowHook overflowErrorHook;
StdExceptUtil::PreThrowHook underflowErrorHook;

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace bslstl {

                        //====================
                        // class StdExceptUtil
                        //====================

// CLASS METHODS
void StdExceptUtil::logCheapStackTrace(const char *exceptionName,
                                       const char *message)
{
    enum { k_MAX_FRAMES = 128,
           k_CHEAP_STACK_BUF_SIZE = 1024 +
                                 k_MAX_FRAMES * (u::k_MAX_BYTES_PER_PTR + 1) };

    char cheapStackBuf[k_CHEAP_STACK_BUF_SIZE];
    bsls::StackAddressUtil::formatCheapStack(cheapStackBuf,
                                             k_CHEAP_STACK_BUF_SIZE,
                                             0);

    BSLS_LOG_FATAL("About to throw %s, %s %s",
                   exceptionName,
                   message,
                   cheapStackBuf);
}

void StdExceptUtil::setRuntimeErrorHook(PreThrowHook hook)
{
    u::runtimeErrorHook = hook;
}

void StdExceptUtil::setLogicErrorHook(PreThrowHook hook)
{
    u::logicErrorHook = hook;
}
void StdExceptUtil::setDomainErrorHook(PreThrowHook hook)
{
    u::domainErrorHook = hook;
}

void StdExceptUtil::setInvalidArgumentHook(PreThrowHook hook)
{
    u::invalidArgumentHook = hook;
}

void StdExceptUtil::setLengthErrorHook(PreThrowHook hook)
{
    u::lengthErrorHook = hook;
}

void StdExceptUtil::setOutOfRangeHook(PreThrowHook hook)
{
    u::outOfRangeHook = hook;
}

void StdExceptUtil::setRangeErrorHook(PreThrowHook hook)
{
    u::rangeErrorHook = hook;
}

void StdExceptUtil::setOverflowErrorHook(PreThrowHook hook)
{
    u::overflowErrorHook = hook;
}

void StdExceptUtil::setUnderflowErrorHook(PreThrowHook hook)
{
    u::underflowErrorHook = hook;
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwRuntimeError(const char *message)
{
    if (u::runtimeErrorHook) {
        (*u::runtimeErrorHook)("std::runtime_error", message);
    }

    BSLS_THROW(std::runtime_error(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwLogicError(const char *message)
{
    if (u::logicErrorHook) {
        (*u::logicErrorHook)("std::logic_error", message);
    }

    BSLS_THROW(std::logic_error(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwDomainError(const char *message)
{
    if (u::domainErrorHook) {
        (*u::domainErrorHook)("std::domain_error", message);
    }

    BSLS_THROW(std::domain_error(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwInvalidArgument(const char *message)
{
    if (u::invalidArgumentHook) {
        (*u::invalidArgumentHook)("std::invalid_argument", message);
    }

    BSLS_THROW(std::invalid_argument(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwLengthError(const char *message)
{
    if (u::lengthErrorHook) {
        (*u::lengthErrorHook)("std::length_error", message);
    }

    BSLS_THROW(std::length_error(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwOutOfRange(const char *message)
{
    if (u::outOfRangeHook) {
        (*u::outOfRangeHook)("std::out_of_range", message);
    }

    BSLS_THROW(std::out_of_range(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwRangeError(const char *message)
{
    if (u::rangeErrorHook) {
        (*u::rangeErrorHook)("std::range_error", message);
    }

    BSLS_THROW(std::range_error(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwOverflowError(const char *message)
{
    if (u::overflowErrorHook) {
        (*u::overflowErrorHook)("std::overflow_error", message);
    }

    BSLS_THROW(std::overflow_error(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwUnderflowError(const char *message)
{
    if (u::underflowErrorHook) {
        (*u::underflowErrorHook)("std::underflow_error", message);
    }

    BSLS_THROW(std::underflow_error(message));
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
