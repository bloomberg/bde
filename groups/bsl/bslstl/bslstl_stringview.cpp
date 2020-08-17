// bslstl_stringview.cpp                                              -*-C++-*-
#include <bslstl_stringview.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")


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

namespace BloombergLP {
#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

const char *BSLSTL_STRING_VIEW_LINKER_CHECK_NAME =
                               "BSLSTL_STRINGVIEW_ABI_CHECK: "
                               STRINGIFY(BSLSTL_STRING_VIEW_LINKER_CHECK_NAME);
}  // close enterprise namespace

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
