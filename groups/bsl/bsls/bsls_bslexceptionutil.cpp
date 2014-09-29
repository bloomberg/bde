// bsls_bslexceptionutil.cpp                                          -*-C++-*-
#include <bsls_bslexceptionutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_exceptionutil.h>

#include <exception>
#include <new>
#include <typeinfo>

#ifdef BSLS_BSLEXCEPTIONUTIL_NORETURN
#error BSLS_BSLEXCEPTIONUTIL_NORETURN \
                                    must be a macro scoped locally to this file
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN)
#   define BSLS_BSLEXCEPTIONUTIL_NORETURN [[noreturn]]
#elif defined(BSLS_PLATFORM_CMP_MSVC)
#   define BSLS_BSLEXCEPTIONUTIL_NORETURN __declspec(noreturn)
#else
#   define BSLS_BSLEXCEPTIONUTIL_NORETURN
#endif

namespace BloombergLP {

namespace bsls {

                      //-----------------------
                      // class BslExceptionUtil
                      //-----------------------

// CLASS METHODS
BSLS_BSLEXCEPTIONUTIL_NORETURN
void BslExceptionUtil::throwBadAlloc()
{
    BSLS_THROW(std::bad_alloc());
}

BSLS_BSLEXCEPTIONUTIL_NORETURN
void BslExceptionUtil::throwBadCast()
{
    BSLS_THROW(std::bad_cast());
}

BSLS_BSLEXCEPTIONUTIL_NORETURN
void BslExceptionUtil::throwBadException()
{
    BSLS_THROW(std::bad_exception());
}

BSLS_BSLEXCEPTIONUTIL_NORETURN
void BslExceptionUtil::throwBadTypeid()
{
    BSLS_THROW(std::bad_typeid());
}

BSLS_BSLEXCEPTIONUTIL_NORETURN
void BslExceptionUtil::throwException()
{
    BSLS_THROW(std::exception());
}

}  // close package namespace

#undef BSLS_BSLEXCEPTIONUTIL_NORETURN

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
