// bslstl_exception.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_EXCEPTION
#define INCLUDED_BSLSTL_EXCEPTION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an implementation of `uncaught_exceptions`.
//
//@CLASSES:
//
//@CANONICAL_HEADER: bsl_exception.h
//
//@DESCRIPTION: This component defines a function `bsl::uncaught_exceptions`.
// For C++17 and later, this is an alias for `std::uncaught_exceptions`. Before
// C++17, we emulate the functionality.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Determining if the stack is being unwound.
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a class that does some processing in it's destructor, but
// we don't want to do this if an exception is "in flight".  `bslmt::OnceGuard`
// is an example of this kind of functionality.
//
// First, we create a class with an `int d_exception_count` member variable,
// and record the number of in-flight exceptions in the constructor.
// ```
// struct ExceptionAware {
//     ExceptionAware() : d_exception_count(bsl::uncaught_exceptions()) {}
//     ~ExceptionAware();
//     int d_exception_count;
//  };
// ```
// Then, we implement the destructor
// ```
// ExceptionAware::~ExceptionAware () {
//     if (bsl::uncaught_exceptions() > d_exception_count) {
//         // The stack is being unwound
//     }
//     else {
//         // The object is being destroyed normally
//     }
// }
// ```

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>

#include <exception>

namespace bsl {
#if defined (BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY) && \
   !(defined(BSLS_LIBRARYFEATURES_FORCE_ABI_ENABLED) &&          \
    (BSLS_LIBRARYFEATURES_FORCE_ABI_ENABLED < 17))

    #define BSLSTL_EXCEPTION_UNCAUGHT_EXCEPTIONS_IS_ALIAS
    using std::uncaught_exceptions;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY && not disabled

#ifndef BSLSTL_EXCEPTION_UNCAUGHT_EXCEPTIONS_IS_ALIAS
    /// Return the number of exceptions that have been thrown or rethrown in
    /// the current thread that have not been caught.  If the C++17 baseline
    /// library is not available, this function may return 1 even if more
    /// than 1 uncaught exception exists, but if guaranteed to return 0 if
    /// there are no uncaught exceptions.
    int uncaught_exceptions() throw();
#endif  // ndef BSLSTL_EXCEPTION_UNCAUGHT_EXCEPTIONS_IS_ALIAS
}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
