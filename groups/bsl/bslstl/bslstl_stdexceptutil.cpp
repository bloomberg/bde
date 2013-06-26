// bslstl_stdexceptutil.cpp                                           -*-C++-*-
#include <bslstl_stdexceptutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_exceptionutil.h>
#include <bsls_platform.h>

#include <stdexcept>
#include <string>

#ifdef BSLSTL_STDEXCEPTUTIL_NORETURN
#error BSLSTL_STDEXCEPTUTIL_NORETURN \
                                    must be a macro scoped locally to this file
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN)
#   define BSLSTL_STDEXCEPTUTIL_NORETURN [[noreturn]]
#elif defined(BSLS_PLATFORM_CMP_MSVC)
#   define BSLSTL_STDEXCEPTUTIL_NORETURN __declspec(noreturn)
#else
#   define BSLSTL_STDEXCEPTUTIL_NORETURN
#endif

namespace BloombergLP {

namespace bslstl {

                        //====================
                        // class StdExceptUtil
                        //====================

// CLASS METHODS
BSLSTL_STDEXCEPTUTIL_NORETURN
void StdExceptUtil::throwRuntimeError(const char *message)
{
    BSLS_THROW(std::runtime_error(message));
}

BSLSTL_STDEXCEPTUTIL_NORETURN
void StdExceptUtil::throwLogicError(const char *message)
{
    BSLS_THROW(std::logic_error(message));
}

BSLSTL_STDEXCEPTUTIL_NORETURN
void StdExceptUtil::throwDomainError(const char *message)
{
    BSLS_THROW(std::domain_error(message));
}

BSLSTL_STDEXCEPTUTIL_NORETURN
void StdExceptUtil::throwInvalidArgument(const char *message)
{
    BSLS_THROW(std::invalid_argument(message));
}

BSLSTL_STDEXCEPTUTIL_NORETURN
void StdExceptUtil::throwLengthError(const char *message)
{
    BSLS_THROW(std::length_error(message));
}

BSLSTL_STDEXCEPTUTIL_NORETURN
void StdExceptUtil::throwOutOfRange(const char *message)
{
    BSLS_THROW(std::out_of_range(message));
}

BSLSTL_STDEXCEPTUTIL_NORETURN
void StdExceptUtil::throwRangeError(const char *message)
{
    BSLS_THROW(std::range_error(message));
}

BSLSTL_STDEXCEPTUTIL_NORETURN
void StdExceptUtil::throwOverflowError(const char *message)
{
    BSLS_THROW(std::overflow_error(message));
}

BSLSTL_STDEXCEPTUTIL_NORETURN
void StdExceptUtil::throwUnderflowError(const char *message)
{
    BSLS_THROW(std::underflow_error(message));
}

}  // close package namespace

#undef BSLSTL_STDEXCEPTUTIL_NORETURN

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
