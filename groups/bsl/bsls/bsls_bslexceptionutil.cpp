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

                      //------------------------
                      // class BslExceptiontUtil
                      //------------------------

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
