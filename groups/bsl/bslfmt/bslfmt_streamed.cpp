// bslfmt_streamed.cpp                                                -*-C++-*-

#include <bslfmt_streamed.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslfmt_streamed_cpp, "$Id$ $CSID$")

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

BSLMF_ASSERT((!BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<const char *, char> >::value));

BSLMF_ASSERT((!BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<const wchar_t *, wchar_t> >::value));

BSLMF_ASSERT((!BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<std::string_view, char> >::value));

BSLMF_ASSERT((!BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<std::wstring_view, wchar_t> >::value));

#ifdef BSLSTL_STRING_VIEW_IS_ALIASED

BSLMF_ASSERT((!BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<bsl::string_view, char> >::value));

BSLMF_ASSERT((!BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<bsl::wstring_view, wchar_t> >::value));

#else // not aliased

BSLMF_ASSERT(!(BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<bsl::string_view, char> >::value));

BSLMF_ASSERT(!(BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<bsl::wstring_view, wchar_t> >::value));

#endif // BSLSTL_STRING_VIEW_IS_ALIASED

#endif // BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
