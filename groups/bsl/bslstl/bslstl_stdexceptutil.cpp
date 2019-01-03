// bslstl_stdexceptutil.cpp                                           -*-C++-*-
#include <bslstl_stdexceptutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_exceptionutil.h>
#include <bsls_platform.h>

#include <stdexcept>
#include <string>

namespace BloombergLP {
namespace bslstl {

                        //====================
                        // class StdExceptUtil
                        //====================

// CLASS METHODS
BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwRuntimeError(const char *message)
{
    BSLS_THROW(std::runtime_error(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwLogicError(const char *message)
{
    BSLS_THROW(std::logic_error(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwDomainError(const char *message)
{
    BSLS_THROW(std::domain_error(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwInvalidArgument(const char *message)
{
    BSLS_THROW(std::invalid_argument(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwLengthError(const char *message)
{
    BSLS_THROW(std::length_error(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwOutOfRange(const char *message)
{
    BSLS_THROW(std::out_of_range(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwRangeError(const char *message)
{
    BSLS_THROW(std::range_error(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwOverflowError(const char *message)
{
    BSLS_THROW(std::overflow_error(message));
}

BSLS_ANNOTATION_NORETURN
void StdExceptUtil::throwUnderflowError(const char *message)
{
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
