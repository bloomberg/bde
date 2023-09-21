// bslstl_monostate.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_MONOSTATE
#define INCLUDED_BSLSTL_MONOSTATE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a default constructible alternative for 'bsl::variant'.
//
//@CLASSES:
//  bsl::monostate: default constructible 'bsl::variant' first alternative
//
//@SEE_ALSO: bslstl_variant
//
//@DESCRIPTION: This component provides a 'bsl::monostate' class.  This class
// can serve as a first alternative type for a 'bsl::variant' to make the
// 'bsl::variant' type default constructible.
//

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#include <variant>  // for 'std::monostate'
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
namespace bsl {
using std::monostate;
}  // close namespace bsl
namespace BloombergLP {
namespace bslh {
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const std::monostate&);
    // Pass a 'monostate' to the specified 'hashAlg', where 'hashAlg' is a
    // hashing algorithm.
}  // close namespace bslh
}  // close enterprise namespace
#else
namespace bsl {
                              // ===============
                              // struct monostate
                              // ===============

struct monostate {
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)
    // CREATORS
    BSLS_KEYWORD_CONSTEXPR monostate() BSLS_KEYWORD_NOEXCEPT;
        // Create a 'monostate' object.  Note that this constructor is
        // implicitly defined and trivial in C++11 and later; a user-declared
        // constructor is provided in C++03 in order to make 'const monostate'
        // default-constructible.
#endif
};

// FREE OPERATORS

BSLS_KEYWORD_CONSTEXPR bool operator==(monostate,
                                       monostate) BSLS_KEYWORD_NOEXCEPT;
BSLS_KEYWORD_CONSTEXPR bool operator!=(monostate,
                                       monostate) BSLS_KEYWORD_NOEXCEPT;
BSLS_KEYWORD_CONSTEXPR bool operator<(monostate,
                                      monostate) BSLS_KEYWORD_NOEXCEPT;
BSLS_KEYWORD_CONSTEXPR bool operator>(monostate,
                                      monostate) BSLS_KEYWORD_NOEXCEPT;
BSLS_KEYWORD_CONSTEXPR bool operator<=(monostate,
                                       monostate) BSLS_KEYWORD_NOEXCEPT;
BSLS_KEYWORD_CONSTEXPR bool operator>=(monostate,
                                       monostate) BSLS_KEYWORD_NOEXCEPT;
    // Compare two 'monostate' objects.  Since 'monostate' only has one value,
    // the result is 'true' for the '==', '<=', and '>=' operators, and 'false'
    // for the '!=', '<', and '>' operators.

template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const monostate&);
    // Pass a 'monostate' to the specified 'hashAlg', where 'hashAlg' is a
    // hashing algorithm.
}  // close namespace bsl
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
namespace BloombergLP {
namespace bslh {
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const std::monostate&)
{
    hashAppend(hashAlg, true);
}
}  // close namespace bslh
}  // close enterprise namespace
#else
namespace bsl {
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)
BSLS_KEYWORD_CONSTEXPR monostate::monostate() BSLS_KEYWORD_NOEXCEPT
{
}
#endif

BSLS_KEYWORD_CONSTEXPR bool operator==(monostate,
                                       monostate) BSLS_KEYWORD_NOEXCEPT
{
    return true;
}
BSLS_KEYWORD_CONSTEXPR bool operator!=(monostate,
                                       monostate) BSLS_KEYWORD_NOEXCEPT
{
    return false;
}
BSLS_KEYWORD_CONSTEXPR bool operator<(monostate,
                                      monostate) BSLS_KEYWORD_NOEXCEPT
{
    return false;
}
BSLS_KEYWORD_CONSTEXPR bool operator>(monostate,
                                      monostate) BSLS_KEYWORD_NOEXCEPT
{
    return false;
}
BSLS_KEYWORD_CONSTEXPR bool operator<=(monostate,
                                       monostate) BSLS_KEYWORD_NOEXCEPT
{
    return true;
}
BSLS_KEYWORD_CONSTEXPR bool operator>=(monostate,
                                       monostate) BSLS_KEYWORD_NOEXCEPT
{
    return true;
}

template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const monostate&)
{
    hashAppend(hashAlg, true);
}
}  // close namespace bsl
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#endif  // INCLUDED_BSLSTL_MONOSTATE

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
