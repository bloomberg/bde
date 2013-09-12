// bsls_bslexceptionutil.h                                            -*-C++-*-
#ifndef INCLUDED_BSLS_BSLEXCEPTIONUTIL
#define INCLUDED_BSLS_BSLEXCEPTIONUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functions for use in 'bsl' that throw standard exceptions.
//
//@CLASSES:
//  bsls::BslExceptionUtil: namespace for utilities to throw exceptions
//
//@SEE_ALSO: bsl_exception, bsl_new, bsl_typeinfo
//
//@DESCRIPTION: This component provides a means to throw standard exceptions
// without introducing a compile-time dependency on the standard exception
// classes.  This is valuable where header files define function templates or
// inline functions that may throw these types as exceptions.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Throwing a standard exception
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we are implementing a class that must conform to the requirements of
// the C++ Standard.  There are several clauses that dictate throwing an
// exception of a standard type to indicate failure.  However, we do not want
// to expose the standard exception header to our clients, which would be
// typical when implementing function templates inline, and we want to have a
// consistent behavior when building with a compiler in a non-standard mode
// that does not support exceptions.
//
// First we declare a function template that wants to throw a standard
// exception.  Note that the 'exception' header is not included at this point.
//..
//  #include <bsls_bslexceptionutil.h>
//
//  template<typename T>
//  void testFunction(int selector)
//      //  Throw a standard exception according to the specified 'selector'.
//  {
//    switch (selector) {
//..
//  Now we can use the utilities in this component to throw the desired
//  exception, even though the standard exception classes are not visible to
//  this code.
//..
//      case  1: bsls::BslExceptionUtil::throwBadAlloc();
//      case  2: bsls::BslExceptionUtil::throwBadCast();
//      default: bsls::BslExceptionUtil::throwException();
//    }
//  }
//..
// Finally, we can write some client code that calls our function, and wishes
// to catch the thrown exception.  Observe that this file must #include the
// corresponding standard header in order to catch the exception.
//..
//  #include <exception>
//  #include <new>
//  #include <typeinfo>
//
//  void callTestFunction()
//  {
//      try {
//          testFunction<int>(1);
//          assert(0 == "Should throw before reaching here.");
//      }
//      catch (const std::bad_alloc& ex) {
//      }
//
//      try {
//          testFunction<double>(2);
//          assert(0 == "Should throw before reaching here.");
//      }
//      catch (const std::bad_cast& ex) {
//      }
//  }
//..

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifdef BSLS_BSLEXCEPTIONUTIL_NORETURN
#error BSLS_BSLEXCEPTIONUTIL_NORETURN \
                             must be a macro scoped locally to this header file
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

                        //=======================
                        // class BslExceptionUtil
                        //=======================

struct BslExceptionUtil {
    // This 'struct' provides a namespace for 'static' utility functions that
    // throw standard library exceptions.

    // CLASS METHODS
    BSLS_BSLEXCEPTIONUTIL_NORETURN
    static void throwBadAlloc();
        // Throw a 'bsl::bad_alloc' exception if exceptions are enabled in the
        // current build mode, otherwise abort the program.  'bsl::bad_alloc'
        // will be an alias for the platform's 'std::bad_alloc' class, to
        // ensure both ABI and API compatibility with non-BDE code relying on
        // the standard exception hierarchy.

    BSLS_BSLEXCEPTIONUTIL_NORETURN
    static void throwBadCast();
        // Throw a 'bsl::bad_cast' exception if exceptions are enabled in the
        // current build mode, otherwise abort the program.  'bsl::bad_cast'
        // will be an alias for the platform's 'std::bad_cast' class, to
        // ensure both ABI and API compatibility with non-BDE code relying on
        // the standard exception hierarchy.

    BSLS_BSLEXCEPTIONUTIL_NORETURN
    static void throwBadException();
        // Throw a 'bsl::bad_exception' exception if exceptions are enabled in
        // the current build mode, otherwise abort the program.
        // 'bsl::bad_exception' will be an alias for the platform's
        // 'std::bad_exception' class, to ensure both ABI and API compatibility
        // with non-BDE code relying on the standard exception hierarchy.

    BSLS_BSLEXCEPTIONUTIL_NORETURN
    static void throwBadTypeid();
        // Throw a 'bsl::bad_typeid' exception if exceptions are enabled in the
        // current build mode, otherwise abort the program.  'bsl::bad_typeid'
        // will be an alias for the platform's 'std::bad_typeid' class, to
        // ensure both ABI and API compatibility with non-BDE code relying on
        // the standard exception hierarchy.

    BSLS_BSLEXCEPTIONUTIL_NORETURN
    static void throwException();
        // Throw a 'bsl::exception' exception if exceptions are enabled in the
        // current build mode, otherwise abort the program.  'bsl::exception'
        // will be an alias for the platform's 'std::exception' class, to
        // ensure both ABI and API compatibility with non-BDE code relying on
        // the standard exception hierarchy.
};

}  // close package namespace

#undef BSLS_BSLEXCEPTIONUTIL_NORETURN


}  // close enterprise namespace

#endif

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
