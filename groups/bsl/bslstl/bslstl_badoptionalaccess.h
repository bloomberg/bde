// bslstl_badoptionalaccess.h                                         -*-C++-*-
#ifndef INCLUDED_BSLSTL_BADOPTIONALACCESS
#define INCLUDED_BSLSTL_BADOPTIONALACCESS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an exception class thrown by 'bsl::optional'.
//
//@CLASSES:
//  bsl::bad_optional_access: exception type thrown by 'bsl::optional'
//
//@SEE_ALSO: bslstl_optional, bslstl_stdexceptionutil
//
//@DESCRIPTION: This component provides a 'bsl::bad_optional_access' exception
// class.  This exception is thrown by 'bsl::optional::value' when accessing a
// 'bsl::optional' object that does not contain a value.  If 'std::optional'
// implementation is available, 'bsl::bad_optional_access' is an alias to
// 'std::bad_optional_access'.
//

#include <bslscm_version.h>

#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#include <exception>

#ifdef BDE_BUILD_TARGET_EXC

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#include <optional> // for 'std::bad_optional_access'
#endif // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

namespace bsl {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
typedef std::bad_optional_access bad_optional_access;
#else
                       // =========================
                       // class bad_optional_access
                       // =========================

class bad_optional_access : public std::exception {
  public:
    // CREATORS
    bad_optional_access() BSLS_KEYWORD_NOEXCEPT;
        // Create a 'bad_optional_access' object.  Note that this function is
        // explicitly user-declared, to make it simple to declare 'const'
        // objects of this type.

    // ACCESSORS
    const char *what() const BSLS_EXCEPTION_WHAT_NOTHROW BSLS_KEYWORD_OVERRIDE;
        // Return a pointer to the string literal "bad_optional_access", with a
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
                       // class bad_optional_access
                       // -------------------------

inline
bad_optional_access::bad_optional_access() BSLS_KEYWORD_NOEXCEPT
: std::exception()
{
}

inline
const char *bad_optional_access::what() const BSLS_EXCEPTION_WHAT_NOTHROW
{
    return "bad_optional_access";
}
#endif // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

}  // close namespace bsl

#endif // BDE_BUILD_TARGET_EXC
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
