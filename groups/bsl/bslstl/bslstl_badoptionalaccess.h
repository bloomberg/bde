// bslstl_badoptionalaccess.h                                                -*-C++-*-
#ifndef INCLUDED_BSLSTL_BADOPTIONALACCESS
#define INCLUDED_BSLSTL_BADOPTIONALACCESS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an exception class to indicate a weak_ptr has expired.
//
//@CLASSES:
//  bslstl::BadOptionalAccess: exception class derived from 'native_std' classes
//  bsl::bad_optional_access: alias to an exception type thrown by the 'bsl' library
//
//@SEE_ALSO: bslstl_optional, bslstl_stdexceptionutil
//
//@DESCRIPTION: This component provides ...
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: TBD
/// - - - - - - -

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BOS_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_memory.h> instead of <bslstl_badoptionalaccess.h> in \
BSL_OVERRIDES_STD mode"
#endif
#include <bslscm_version.h>

#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_nativestd.h>

#include <exception>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#include <optional> // 'std::bad_optional_access' if defined
#endif // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

namespace BloombergLP {
namespace bslstl { class BadOptionalAccess; }
}  // close enterprise namespace

namespace bsl {
#ifdef __cpp_lib_optional
typedef std::bad_optional_access bad_optional_access;
#else
typedef ::BloombergLP::bslstl::BadOptionalAccess bad_optional_access;
#endif //__cpp_lib_optional

}  // close namespace bsl

#ifdef __cpp_lib_optional
#else
namespace BloombergLP {
namespace bslstl {

                       // ================
                       // class BadOptionalAccess
                       // ================

class BadOptionalAccess : public native_std::exception {
  public:
    BadOptionalAccess() BSLS_KEYWORD_NOEXCEPT;
        // Create a 'BadOptionalAccess' object.  Note that this function is explicitly
        // user-declared, to make it simple to declare 'const' objects of this
        // type.

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

                       // ----------------
                       // class BadOptionalAccess
                       // ----------------

inline
BadOptionalAccess::BadOptionalAccess() BSLS_KEYWORD_NOEXCEPT
: native_std::exception()
{
}

inline
const char *BadOptionalAccess::what() const BSLS_EXCEPTION_WHAT_NOTHROW
{
    return "bad_optional_access";
}

}  // close package namespace
}  // close enterprise namespace
#endif //__cpp_lib_optional
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
