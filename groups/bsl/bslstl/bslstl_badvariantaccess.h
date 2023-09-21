// bslstl_badvariantaccess.h                                          -*-C++-*-
#ifndef INCLUDED_BSLSTL_BADVARIANTACCESS
#define INCLUDED_BSLSTL_BADVARIANTACCESS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an exception class thrown by 'bsl::variant'.
//
//@CLASSES:
//  bsl::bad_variant_access: exception type thrown by 'bsl::variant'
//
//@SEE_ALSO: bslstl_variant, bslstl_stdexceptionutil
//
//@DESCRIPTION: This component provides the 'bsl::bad_variant_access' exception
// class.  This exception is thrown by 'bsl::get' when a 'bsl::variant' does
// not contain the requested alternative and 'bsl::visit' when a 'bsl::variant'
// does not contain a value.  If 'std::variant' is available,
// 'bsl::bad_variant_access' is an alias to 'std::bad_variant_access'.
//

#include <bslscm_version.h>

#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#include <exception>

#ifdef BDE_BUILD_TARGET_EXC

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#include <variant>  // for 'std::bad_variant_access'
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

namespace bsl {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
using std::bad_variant_access;
#else
                          // ========================
                          // class bad_variant_access
                          // ========================

class bad_variant_access : public std::exception {
  public:
    // CREATORS
    bad_variant_access() BSLS_KEYWORD_NOEXCEPT;
        // Create a 'bad_variant_access' object.  Note that this function is
        // explicitly user-declared, to make it simple to declare 'const'
        // objects of this type.

    // ACCESSORS
    const char *what() const BSLS_EXCEPTION_VIRTUAL_NOTHROW
                                                         BSLS_KEYWORD_OVERRIDE;
        // Return a pointer to the string literal "bad_variant_access", with a
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
                         // class bad_variant_access
                         // -------------------------

inline
bad_variant_access::bad_variant_access() BSLS_KEYWORD_NOEXCEPT
: std::exception()
{
}

inline
const char *bad_variant_access::what() const BSLS_EXCEPTION_VIRTUAL_NOTHROW
{
    return "bad_variant_access";
}
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

}  // close namespace bsl

#endif  // BDE_BUILD_TARGET_EXC
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
