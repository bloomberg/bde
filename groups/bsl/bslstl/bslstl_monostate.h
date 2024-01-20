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


#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#include <variant>  // for 'std::monostate'
#include <bslh_hashvariant.h>
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
namespace bsl {
using std::monostate;
}  // close namespace bsl
#else
namespace bsl {
                              // ===============
                              // struct monostate
                              // ===============

struct monostate {

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

template <class t_HASHALG>
void hashAppend(t_HASHALG& hashAlg, const monostate&);
    // Pass a 'monostate' to the specified 'hashAlg', where 'hashAlg' is a
    // hashing algorithm.
}  // close namespace bsl
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
namespace bsl {


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

template <class t_HASHALG>
void hashAppend(t_HASHALG& hashAlg, const monostate&)
{
    hashAppend(hashAlg, -7777);
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
