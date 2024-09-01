// bslstl_stdexceptutil.h                                             -*-C++-*-
#ifndef INCLUDED_BSLSTL_STDEXCEPTUTIL
#define INCLUDED_BSLSTL_STDEXCEPTUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility to throw standard exceptions.
//
//@CLASSES:
//  bslstl::StdExceptUtil: namespace for utilities to throw standard exceptions
//
//@CANONICAL_HEADER: bsl_stdexcept.h
//
//@SEE_ALSO: stdexcept
//
//@DESCRIPTION: This component provides a means to throw standard exceptions
// without introducing a compile-time dependency on the standard exception
// classes.  This valuable where header files define function templates or
// inline functions that may throw these types as exceptions.
//
///Pre-Throw Hooks
///---------------
// For each exception type supported by this component, there is a "pre throw
// hook", a function pointer that is normally null.  If that pointer is set to
// a function, that function is called prior to the throw.  This gives the
// client a chance to log a message.
//
// If the pre-throw hook is set to `StdExceptUtil::logCheapStackTrace`, a cheap
// stack trace will be logged, enabling the client to use
// `/bb/bin/showfunc.tsk` on the cheap stack trace to get a stack trace with
// symbols.  When running `showfunc.tsk`, pipe the output through `c++filt` to
// get demangled symbols.
//
// If the pre-throw hook is set to
// `balst::StackTracePrintUtil::logExceptionStackTrace`, a full multi-line
// stack trace with symbols will be logged, with, on some platforms, symbol
// demangling, line numbers, and source file names.  This alternative requires
// considerable disk access and is therefore orders of magnitude slower than
// the cheap stack trace.
//
///Usage
///-----
// First we declare a function template that wants to throw a standard
// exception.  Note that the `stdexcept` header is not included at this point.
// ```
// #include <bslstl_stdexceptutil.h>
//
// template<typename T>
// void testFunction(int selector)
//     //  Throw a standard exception according to the specified 'selector'.
// {
// switch(selector) {
//   case 1: {
//     bslstl::StdExceptUtil::throwRuntimeError("sample message 1");
//   } break;
//   case 2: {
//     bslstl::StdExceptUtil::throwLogicError("sample message 2");
//   } break;
//   default: {
//     bslstl::StdExceptUtil::throwInvalidArgument("ERROR");
//   } break;
// }
// ```
// However, if client code wishes to catch the exception, the `.cpp` file must
// `#include` the appropriate header.
// ```
// #include <stdexcept>
//
// void callTestFunction()
// {
//   try {
//     testFunction<int>(1);
//     assert(0 == "Should throw before reaching here.");
//   }
//   catch(const runtime_error& ex) {
//     assert(0 == std::strcmp(ex.what(), "sample message 1"));
//   }
//
//   try {
//     testFunction<double>(2);
//     assert(0 == "Should throw before reaching here.");
//   }
//   catch(const logic_error& ex) {
//     assert(0 == std::strcmp(ex.what(), "sample message 2"));
//   }
// }
// ```

#include <bslscm_version.h>

#include <bsla_noreturn.h>

#include <bsls_compilerfeatures.h>

#include <stddef.h>

namespace BloombergLP {

namespace bslstl {

                        //====================
                        // class StdExceptUtil
                        //====================

/// This `struct` provides a namespace for `static` utility functions that
/// throw standard library exceptions.
struct StdExceptUtil {

    // PUBLIC TYPES

    /// This is the type of function pointer that can be set.  One such
    /// static function pointer exists for each exception type supported by
    /// this component.  Functions called to throw exceptions examine their
    /// respective pointer, and if it's non-null, call it and then throw
    /// after it returns.  Note that it is recommended that the hook
    /// function log a greppable statement such as "About to throw
    /// <exceptionName>".
    typedef void (*PreThrowHook)(const char *exceptionName,
                                 const char *message);

    // CLASS METHODS

    /// Log "About to throw ", then the specified `exceptionName`, then the
    /// specified `message`, then log a cheap stack trace with warning
    /// severity.  This function is intended as a candidate for setting to
    /// the pre-throw hooks.  Note that a far slower alternative to this,
    /// which logs a full, multi-line stack trace with resolved symbols and,
    /// on many platforms, line numbers and source file names, is
    /// `balst::StackTracePrintUtil::logExceptionStackTrace`.
    static void logCheapStackTrace(const char *exceptionName,
                                   const char *message);

    static void setRuntimeErrorHook(   PreThrowHook hook);
    static void setLogicErrorHook(     PreThrowHook hook);
    static void setDomainErrorHook(    PreThrowHook hook);
    static void setInvalidArgumentHook(PreThrowHook hook);
    static void setLengthErrorHook(    PreThrowHook hook);
    static void setOutOfRangeHook(     PreThrowHook hook);
    static void setRangeErrorHook(     PreThrowHook hook);
    static void setOverflowErrorHook(  PreThrowHook hook);

    /// Set the pre throw hook for the specified exception type to the
    /// specified `hook`.  If `hook` is passed 0, or if the settor was never
    /// called, that means that no pre-throw function will be called.
    static void setUnderflowErrorHook( PreThrowHook hook);

    /// Throw a `std::runtime_error` exception supplying the specified
    /// `message` as the sole argument to its constructor.
    BSLA_NORETURN
    static void throwRuntimeError(const char *message);

    /// Throw a `std::logic_error` exception supplying the specified
    /// `message` as the sole argument to its constructor.
    BSLA_NORETURN
    static void throwLogicError(const char *message);

    /// Throw a `std::domain_error` exception supplying the specified
    /// `message` as the sole argument to its constructor.
    BSLA_NORETURN
    static void throwDomainError(const char *message);

    /// Throw a `std::invalid_argument` exception supplying the specified
    /// `message` as the sole argument to its constructor.
    BSLA_NORETURN
    static void throwInvalidArgument(const char *message);

    /// Throw a `std::length_error` exception supplying the specified
    /// `message` as the sole argument to its constructor.
    BSLA_NORETURN
    static void throwLengthError(const char *message);

    /// Throw a `std::out_of_range` exception supplying the specified
    /// `message` as the sole argument to its constructor.
    BSLA_NORETURN
    static void throwOutOfRange(const char *message);

    /// Throw a `std::range_error` exception supplying the specified
    /// `message` as the sole argument to its constructor.
    BSLA_NORETURN
    static void throwRangeError(const char *message);

    /// Throw a `std::overflow_error` exception supplying the specified
    /// `message` as the sole argument to its constructor.
    BSLA_NORETURN
    static void throwOverflowError(const char *message);

    /// Throw a `std::underflow_error` exception supplying the specified
    /// `message` as the sole argument to its constructor.
    BSLA_NORETURN
    static void throwUnderflowError(const char *message);
};

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

/// This alias is defined for backward compatibility.
typedef bslstl::StdExceptUtil bslstl_StdExceptUtil;
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

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
