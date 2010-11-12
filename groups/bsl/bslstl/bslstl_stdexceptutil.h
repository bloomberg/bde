// bslstl_stdexceptutil.h                                             -*-C++-*-
#ifndef INCLUDED_BSLSTL_STDEXCEPTUTIL
#define INCLUDED_BSLSTL_STDEXCEPTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility to throw standard exceptions.
//
//@CLASSES:
//   bslstl_StdExceptUtil: namespace for utilities to throw standard exceptions
//
//@SEE_ALSO: stdexcept
//
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides a means to throw standard exceptions
// without introducing a compile-time dependency on the standard exception
// classes.  This valuable where header files define function templates or
// inline functions that may throw these types as exceptions.
//
///Usage
///-----
// First we declare a function template that wants to throw a standard
// exception.  Note that the 'stdexcept' header is not included at this
// point.
//..
//  #include <bslstl_stdexceptutil.h>
//
//  template<typename T>
//  void testFunction(int selector)
//      //  Throw a standard exception according to the specified 'selector'.
//  {
//    switch(selector) {
//      case 1: bslstl_StdExceptUtil::throwRuntimeError("sample message 1");
//      case 2: bslstl_StdExceptUtil::throwLogicError("sample message 2");
//      default : bslstl_StdExceptUtil::throwInvalidArgument("ERROR");
//    }
//  }
//..
// However, if client code wishes to catch the exception, the '.cpp' file must
// '#include' the appropriate header.
//..
//  #include <stdexcept>
//
//  void callTestFunction()
//  {
//    try {
//      testFunction<int>(1);
//      assert(0 == "Should throw before reaching here.");
//    }
//    catch(const runtime_error& ex) {
//      assert(0 == std::strcmp(ex.what(), "sample message 1"));
//    }
//
//    try {
//      testFunction<double>(2);
//      assert(0 == "Should throw before reaching here.");
//    }
//    catch(const logic_error& ex) {
//      assert(0 == std::strcmp(ex.what(), "sample message 2"));
//    }
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifdef BSLSTL_STDEXCEPTUTIL_NORETURN
#error BSLSTL_STDEXCEPTUTIL_NORETURN \
                             must be a macro scoped locally to this header file
#endif

// Note that a portable syntax for 'noreturn' will be available once we have
// access to conforming C++0x compilers.
//# define BSLSTL_STDEXCEPTUTIL_NORETURN [[noreturn]]

#if defined(BSLS_PLATFORM__CMP_MSVC)
#   define BSLSTL_STDEXCEPTUTIL_NORETURN __declspec(noreturn)
#else
#   define BSLSTL_STDEXCEPTUTIL_NORETURN
#endif

namespace BloombergLP {

                        //===========================
                        // class bslstl_StdExceptUtil
                        //===========================

struct bslstl_StdExceptUtil {
    // This 'struct' provides a namespace for 'static' utility functions that
    // throw standard library exceptions.

    //  CLASS METHODS
    BSLSTL_STDEXCEPTUTIL_NORETURN
    static void throwRuntimeError(const char *message);
        // Throw a 'std::runtime_error' exception supplying the specified
        // 'message' as the sole argument to its constructor.

    BSLSTL_STDEXCEPTUTIL_NORETURN
    static void throwLogicError(const char *message);
        // Throw a 'std::logic_error' exception supplying the specified
        // 'message' as the sole argument to its constructor.

    BSLSTL_STDEXCEPTUTIL_NORETURN
    static void throwDomainError(const char *message);
        // Throw a 'std::domain_error' exception supplying the specified
        // 'message' as the sole argument to its constructor.

    BSLSTL_STDEXCEPTUTIL_NORETURN
    static void throwInvalidArgument(const char *message);
        // Throw a 'std::invalid_argument' exception supplying the specified
        // 'message' as the sole argument to its constructor.

    BSLSTL_STDEXCEPTUTIL_NORETURN
    static void throwLengthError(const char *message);
        // Throw a 'std::length_error' exception supplying the specified
        // 'message' as the sole argument to its constructor.

    BSLSTL_STDEXCEPTUTIL_NORETURN
    static void throwOutOfRange(const char *message);
        // Throw a 'std::out_of_range' exception supplying the specified
        // 'message' as the sole argument to its constructor.

    BSLSTL_STDEXCEPTUTIL_NORETURN
    static void throwRangeError(const char *message);
        // Throw a 'std::range_error' exception supplying the specified
        // 'message' as the sole argument to its constructor.

    BSLSTL_STDEXCEPTUTIL_NORETURN
    static void throwOverflowError(const char *message);
        // Throw a 'std::overflow_error' exception supplying the specified
        // 'message' as the sole argument to its constructor.

    BSLSTL_STDEXCEPTUTIL_NORETURN
    static void throwUnderflowError(const char *message);
        // Throw a 'std::underflow_error' exception supplying the specified
        // 'message' as the sole argument to its constructor.
};

#undef BSLSTL_STDEXCEPTUTIL_NORETURN

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
