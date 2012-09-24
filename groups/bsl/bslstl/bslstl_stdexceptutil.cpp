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

// Note that a portable syntax for 'noreturn' will be available once we have
// access to conforming C++0x compilers.
//# define BSLSTL_STDEXCEPTUTIL_NORETURN [[noreturn]]

#if defined(BSLS_PLATFORM_CMP_MSVC)
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
