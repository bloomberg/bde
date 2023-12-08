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
void *null = 0;

u::AtomicPtr runtimeErrorHook    = { u::null };
u::AtomicPtr logicErrorHook      = { u::null };
u::AtomicPtr domainErrorHook     = { u::null };
u::AtomicPtr invalidArgumentHook = { u::null };
u::AtomicPtr lengthErrorHook     = { u::null };
u::AtomicPtr outOfRangeHook      = { u::null };
u::AtomicPtr rangeErrorHook      = { u::null };
u::AtomicPtr overflowErrorHook   = { u::null };
u::AtomicPtr underflowErrorHook  = { u::null };

StdExceptUtil::PreThrowHook readHook(u::AtomicPtr *hook)
    // Read the specified atomic pointer 'hook' and return it cast to a
    // 'PreThrowHook'.
{
    return reinterpret_cast<StdExceptUtil::PreThrowHook>(
                                              u::Atomics::getPtrAcquire(hook));
}

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
                        &u::runtimeErrorHook, u::CastUtil::cast<void *>(hook));
}

void StdExceptUtil::setLogicErrorHook(PreThrowHook hook)
{
    u::Atomics::setPtrRelease(
                          &u::logicErrorHook, u::CastUtil::cast<void *>(hook));
}
void StdExceptUtil::setDomainErrorHook(PreThrowHook hook)
{
    u::Atomics::setPtrRelease(
                         &u::domainErrorHook, u::CastUtil::cast<void *>(hook));
}

void StdExceptUtil::setInvalidArgumentHook(PreThrowHook hook)
{
    u::Atomics::setPtrRelease(
                     &u::invalidArgumentHook, u::CastUtil::cast<void *>(hook));
}

void StdExceptUtil::setLengthErrorHook(PreThrowHook hook)
{
    u::Atomics::setPtrRelease(
                         &u::lengthErrorHook, u::CastUtil::cast<void *>(hook));
}

void StdExceptUtil::setOutOfRangeHook(PreThrowHook hook)
{
    u::Atomics::setPtrRelease(
                          &u::outOfRangeHook, u::CastUtil::cast<void *>(hook));
}

void StdExceptUtil::setRangeErrorHook(PreThrowHook hook)
{
    u::Atomics::setPtrRelease(
                          &u::rangeErrorHook, u::CastUtil::cast<void *>(hook));
}

void StdExceptUtil::setOverflowErrorHook(PreThrowHook hook)
{
    u::Atomics::setPtrRelease(
                       &u::overflowErrorHook, u::CastUtil::cast<void *>(hook));
}

void StdExceptUtil::setUnderflowErrorHook(PreThrowHook hook)
{
    u::Atomics::setPtrRelease(
                      &u::underflowErrorHook, u::CastUtil::cast<void *>(hook));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwRuntimeError(const char *message)
{
    PreThrowHook hook = u::readHook(&u::runtimeErrorHook);
    if (hook) {
        (*hook)("std::runtime_error", message);
    }

    BSLS_THROW(std::runtime_error(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwLogicError(const char *message)
{
    PreThrowHook hook = u::readHook(&u::logicErrorHook);
    if (hook) {
        (*hook)("std::logic_error", message);
    }

    BSLS_THROW(std::logic_error(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwDomainError(const char *message)
{
    PreThrowHook hook = u::readHook(&u::domainErrorHook);
    if (hook) {
        (*hook)("std::domain_error", message);
    }

    BSLS_THROW(std::domain_error(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwInvalidArgument(const char *message)
{
    PreThrowHook hook = u::readHook(&u::invalidArgumentHook);
    if (hook) {
        (*hook)("std::invalid_argument", message);
    }

    BSLS_THROW(std::invalid_argument(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwLengthError(const char *message)
{
    PreThrowHook hook = u::readHook(&u::lengthErrorHook);
    if (hook) {
        (*hook)("std::length_error", message);
    }

    BSLS_THROW(std::length_error(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwOutOfRange(const char *message)
{
    PreThrowHook hook = u::readHook(&u::outOfRangeHook);
    if (hook) {
        (*hook)("std::out_of_range", message);
    }

    BSLS_THROW(std::out_of_range(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwRangeError(const char *message)
{
    PreThrowHook hook = u::readHook(&u::rangeErrorHook);
    if (hook) {
        (*hook)("std::range_error", message);
    }

    BSLS_THROW(std::range_error(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwOverflowError(const char *message)
{
    PreThrowHook hook = u::readHook(&u::overflowErrorHook);
    if (hook) {
        (*hook)("std::overflow_error", message);
    }

    BSLS_THROW(std::overflow_error(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwUnderflowError(const char *message)
{
    PreThrowHook hook = u::readHook(&u::underflowErrorHook);
    if (hook) {
        (*hook)("std::underflow_error", message);
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
