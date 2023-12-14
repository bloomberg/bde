// bslstl_stdexceptutil.cpp                                           -*-C++-*-
#include <bslstl_stdexceptutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_atomicoperations.h>
#include <bsls_exceptionutil.h>
#include <bsls_log.h>
#include <bsls_platform.h>
#include <bsls_pointercastutil.h>
#include <bsls_stackaddressutil.h>

#include <stdexcept>
#include <string>

namespace {
namespace u {

using namespace BloombergLP;
using namespace bslstl;

// TYPES
typedef bsls::AtomicOperations        Atomics;
typedef Atomics::AtomicTypes::Pointer AtomicPtr;
typedef bsls::PointerCastUtil         CastUtil;

enum { k_MAX_BYTES_PER_PTR = sizeof(void *) * 2 };

// DATA
void * const null = 0;

u::AtomicPtr runtime_error_Hook    = { u::null };
u::AtomicPtr logic_error_Hook      = { u::null };
u::AtomicPtr domain_error_Hook     = { u::null };
u::AtomicPtr invalid_argument_Hook = { u::null };
u::AtomicPtr length_error_Hook     = { u::null };
u::AtomicPtr out_of_range_Hook     = { u::null };
u::AtomicPtr range_error_Hook      = { u::null };
u::AtomicPtr overflow_error_Hook   = { u::null };
u::AtomicPtr underflow_error_Hook  = { u::null };

#define U_THROW_EXCEPTION_BODY(exceptionName, message)                        \
    PreThrowHook preThrowHook = reinterpret_cast<PreThrowHook>(               \
                     u::Atomics::getPtrAcquire(&u::exceptionName ## _Hook));  \
    if (preThrowHook) {                                                       \
        (*preThrowHook)("std::" #exceptionName, message);                     \
    }                                                                         \
                                                                              \
    BSLS_THROW(std::exceptionName(message))

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

    BSLS_LOG_WARN("About to throw %s, %s %s",
                  exceptionName,
                  message,
                  cheapStackBuf);
}

void StdExceptUtil::setRuntimeErrorHook(PreThrowHook hook)
{
    u::Atomics::setPtrRelease(
                      &u::runtime_error_Hook, u::CastUtil::cast<void *>(hook));
}

void StdExceptUtil::setLogicErrorHook(PreThrowHook hook)
{
    u::Atomics::setPtrRelease(
                        &u::logic_error_Hook, u::CastUtil::cast<void *>(hook));
}
void StdExceptUtil::setDomainErrorHook(PreThrowHook hook)
{
    u::Atomics::setPtrRelease(
                       &u::domain_error_Hook, u::CastUtil::cast<void *>(hook));
}

void StdExceptUtil::setInvalidArgumentHook(PreThrowHook hook)
{
    u::Atomics::setPtrRelease(
                   &u::invalid_argument_Hook, u::CastUtil::cast<void *>(hook));
}

void StdExceptUtil::setLengthErrorHook(PreThrowHook hook)
{
    u::Atomics::setPtrRelease(
                       &u::length_error_Hook, u::CastUtil::cast<void *>(hook));
}

void StdExceptUtil::setOutOfRangeHook(PreThrowHook hook)
{
    u::Atomics::setPtrRelease(
                       &u::out_of_range_Hook, u::CastUtil::cast<void *>(hook));
}

void StdExceptUtil::setRangeErrorHook(PreThrowHook hook)
{
    u::Atomics::setPtrRelease(
                        &u::range_error_Hook, u::CastUtil::cast<void *>(hook));
}

void StdExceptUtil::setOverflowErrorHook(PreThrowHook hook)
{
    u::Atomics::setPtrRelease(
                     &u::overflow_error_Hook, u::CastUtil::cast<void *>(hook));
}

void StdExceptUtil::setUnderflowErrorHook(PreThrowHook hook)
{
    u::Atomics::setPtrRelease(
                    &u::underflow_error_Hook, u::CastUtil::cast<void *>(hook));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwRuntimeError(const char *message)
{
    U_THROW_EXCEPTION_BODY(runtime_error, message);
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwLogicError(const char *message)
{
    U_THROW_EXCEPTION_BODY(logic_error, message);
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwDomainError(const char *message)
{
    U_THROW_EXCEPTION_BODY(domain_error, message);
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwInvalidArgument(const char *message)
{
    U_THROW_EXCEPTION_BODY(invalid_argument, message);
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwLengthError(const char *message)
{
    U_THROW_EXCEPTION_BODY(length_error, message);
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwOutOfRange(const char *message)
{
    U_THROW_EXCEPTION_BODY(out_of_range, message);
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwRangeError(const char *message)
{
    U_THROW_EXCEPTION_BODY(range_error, message);
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwOverflowError(const char *message)
{
    U_THROW_EXCEPTION_BODY(overflow_error, message);
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwUnderflowError(const char *message)
{
    U_THROW_EXCEPTION_BODY(underflow_error, message);
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
