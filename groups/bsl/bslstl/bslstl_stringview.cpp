// bslstl_stringview.cpp                                              -*-C++-*-
#include <bslstl_stringview.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslmf_isbitwisecopyable.h>

#include <bslmf_assert.h>

//=============================================================================
//                            // Implementation Note
//
//                           // ---------------------
//                           // 'StringView_Identity'
//                           // ---------------------
//
// The identity metafunction is intended to inhibit type deduction from a
// specific template argument.  Using identity on one of the arguments of a
// function template allows the compiler to deduce template arguments from the
// other function arguments thereby avoiding potential ambiguities and allowing
// implicit conversions to be considered for the argument for which the
// identity metafunction is used.
//
// Using this approach provides a convenient way to implement comparison
// operations for 'string_view', which is required to allow comparisons between
// 'string_view' objects and between one 'string_view' and one object that is
// implictly convertible to a 'string_view'.  Unfortunately, some compilers are
// not able to correctly hande the three overloads, two of which employ the
// identity metafunction for one of their arguments.  Specifically, the MSVC
// compiler can't handle partial ordering of function templates w.r.t.
// non-deduced context which the use of identity metafunction introduces,
// whereas some Sun compilers (pre-5.12.4 and 5.12.6, see DRQS 169697089)
// mangle the overloads in the same way, and compilation fails because multiple
// functions are defined with the same symbol.
//
// As a workaround, we provide two implementations of 'StringView_Identity'
// metafunction.  For compilers that do not have the deficiencies mentioned
// above, we use the straigtforward identity transformation.  For MSVC and Sun
// compilers, however, instead of aliasing the type itself, we alias a
// lightweight wrapper which is implicitly constructible from any type
// convertible to a 'string_view' and is itself implicitly convertible to
// 'string_view'.  This allows function implementation to remain the same on
// all compilers.  In addition, since we know that this wrapper is only used
// for 'string_view' objects which are very cheap to copy, this does not
// introduce a significant performance penalty.
//
// The comparison operators '==', '!=', '<', '>', '<=', and '>=' are all
// defined with 3 overloads -- one taking two string views, and two others each
// taking a string view on one side and a 'StringView_Identity<string_view>' on
// the other.
//=============================================================================

namespace bsl {
    BSLMF_ASSERT(bsl::is_trivially_copyable<string_view>::value);
    BSLMF_ASSERT(BloombergLP::bslmf::IsTriviallyCopyableCheck<
                                                          string_view>::value);
    BSLMF_ASSERT(BloombergLP::bslmf::IsBitwiseMoveable<string_view>::value);
}  // close namespace bsl

#if defined (BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY) && \
    defined (BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)
namespace bsl {
inline namespace literals {
inline namespace string_view_literals {

bsl::string_view operator ""_sv(const char  *characterString,
                                std::size_t  length)
{
    BSLS_ASSERT_SAFE(characterString || 0 == length);
    return bsl::string_view(characterString, length);
}

bsl::wstring_view operator ""_sv(const wchar_t *characterString,
                                 std::size_t    length)
{
    BSLS_ASSERT_SAFE(characterString || 0 == length);
    return bsl::wstring_view(characterString, length);
}

}  // close namespace string_view_literals
}  // close namespace literals

}  // close namespace bsl
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY &&
        // BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
