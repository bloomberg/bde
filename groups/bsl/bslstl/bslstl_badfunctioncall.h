// bslstl_badfunctioncall.h                                           -*-C++-*-
#ifndef INCLUDED_BSLSTL_BADFUNCTIONCALL
#define INCLUDED_BSLSTL_BADFUNCTIONCALL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an exception class thrown by 'bsl::function'.
//
//@CLASSES:
//  bsl::bad_function_call: exception type thrown by 'bsl::function'
//
//@SEE_ALSO: bslstl_function, bslstl_stdexceptionutil
//
//@DESCRIPTION: This component provides a 'bsl::bad_function_call' exception
// class.  This exception is thrown by 'bsl::function::operator()' when the
// function wrapper object has no target.  If 'std::function' implementation is
// available, 'bsl::bad_function_call' is an alias to 'std::bad_function_call'.
//

#include <bslscm_version.h>

#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#ifdef BDE_BUILD_TARGET_EXC

#include <exception>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#include <functional>  // for 'std::bad_function_call'
#endif  //BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

namespace bsl {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
typedef std::bad_function_call bad_function_call;
#else
                          // =======================
                          // class bad_function_call
                          // =======================

class bad_function_call : public std::exception {
  public:
    // CREATORS
    bad_function_call() BSLS_KEYWORD_NOEXCEPT;
        // Create a 'bad_function_call' object.  Note that this function is
        // explicitly user-declared, to make it simple to declare 'const'
        // objects of this type.

    // ACCESSORS
    const char *what() const BSLS_EXCEPTION_VIRTUAL_NOTHROW
                                                         BSLS_KEYWORD_OVERRIDE;
        // Return a pointer to the string literal "bad_function_call", with a
        // storage duration of the lifetime of the program.  Note that the
        // caller should *not* attempt to free this memory.  Note that the
        // 'bsls_exceptionutil' macro 'BSLS_NOTHROW_SPEC' is deliberately not
        // used here, as a number of standard libraries declare 'what' in the
        // base 'exception' class explicitly with the no-throw specification,
        // even in a build that may not recognize exceptions.
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                         // -------------------------
                         // class bad_function_call
                         // -------------------------

inline
bad_function_call::bad_function_call() BSLS_KEYWORD_NOEXCEPT
: std::exception()
{
}

inline
const char *bad_function_call::what() const BSLS_EXCEPTION_VIRTUAL_NOTHROW
{
    return "bad_function_call";
}
#endif  //BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

}  // close namespace bsl

#endif  //BDE_BUILD_TARGET_EXC
#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
