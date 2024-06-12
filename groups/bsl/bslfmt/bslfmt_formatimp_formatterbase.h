// bslfmt_formatimp.h                                                    -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATIMP_FORMATTERBASE
#define INCLUDED_BSLFMT_FORMATIMP_FORMATTERBASE

#include <bslscm_version.h>

#include <bslalg_numericformatterutil.h>

#include <bslmf_integralconstant.h>
#include <bslmf_isarithmetic.h>
#include <bslmf_issame.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_keyword.h>

#include <bslstl_iterator.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <bslfmt_formatimp_error.h>

//#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
//// Include version that can be compiled with C++03
//// Generated on Tue Jun 11 07:56:24 2024
//// Command line: sim_cpp11_features.pl bslfmt_formatimp.h
//# define COMPILING_BSLFMT_FORMATIMP_H
//# include <bslfmt_formatimp_cpp03.h>
//# undef COMPILING_BSLFMT_FORMATIMP_H
//#else

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202002L
#define BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20                      \
    typedef void FormatImp_PreventStdPromotion
#else
#define BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20
#endif

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202302L
#define BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP23                      \
    typedef void FormatImp_PreventStdPromotion
#else
#define BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP23
#endif


namespace bsl {
template <class t_ARG, class t_CHAR = char>
struct formatter {
  public:
    // TRAITS
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

  private:
    // NOT IMPLEMENTED
    formatter(const formatter&) BSLS_KEYWORD_DELETED;
    formatter& operator=(const formatter&) BSLS_KEYWORD_DELETED;
};
}  // close namespace bsl

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

namespace BloombergLP {
namespace bslfmt {
template <class t_FORMATTER, class = void>
struct FormatImp_formatterIsStdAliasingEnabled : bsl::true_type {
};

template <class t_FORMATTER>
struct FormatImp_formatterIsStdAliasingEnabled<
    t_FORMATTER,
    typename t_FORMATTER::FormatImp_PreventStdPromotion> : bsl::false_type {
};
}  // close namespace bslfmt
}  // close enterprise namespace

//namespace BloombergLP {
//namespace bslfmt {
//template <class t_TYPE>
//constexpr bool bslfmt_format_IsStdBasicString = false;
//
//template <class charT, class traits, class Allocator>
//constexpr bool bslfmt_format_IsStdBasicString<
//    std::basic_string<charT, traits, Allocator> > = true;
//
//template <class t_TYPE>
//constexpr bool bslfmt_format_IsStdBasicStringView = false;
//
//template <class charT, class traits>
//constexpr bool bslfmt_format_IsStdBasicStringView<
//    ::std::basic_string_view<charT, traits> > = true;
//}  // close namespace bslfmt
//}  // close enterprise namespace

namespace std {
template <class t_ARG, class t_CHAR>
struct formatter;

template <class t_ARG, class t_CHAR>
requires(
    BloombergLP::bslfmt::FormatImp_formatterIsStdAliasingEnabled<
        bsl::formatter<t_ARG, t_CHAR> >::value
)
struct formatter<t_ARG, t_CHAR>
: bsl::formatter<t_ARG, t_CHAR> {};

//template <class t_ARG, class t_CHAR>
//requires(
//    !bsl::is_arithmetic_v<t_ARG> && !bsl::is_same_v<t_ARG, bsl::nullptr_t> &&
//    !bsl::is_same_v<t_ARG, void *> && !bsl::is_same_v<t_ARG, const void *> &&
//    !bsl::is_same_v<t_ARG, t_CHAR *> &&
//    !bsl::is_same_v<t_ARG, const t_CHAR *> &&
//    !bsl::is_same_v<bsl::remove_extent_t<t_ARG>, const t_CHAR> &&
//    !BloombergLP::bslfmt::bslfmt_format_IsStdBasicString<t_ARG> &&
//    !BloombergLP::bslfmt::bslfmt_format_IsStdBasicStringView<t_ARG> &&
//    BloombergLP::bslfmt::FormatImp_formatterIsStdAliasingEnabled<
//        bsl::formatter<t_ARG, t_CHAR> >::value) struct formatter<t_ARG, t_CHAR>
//: bsl::formatter<t_ARG, t_CHAR> {
//};
}  // close namespace std
#endif

namespace bsl {
// FORMATTER SPECIALIZATIONS

template <> struct formatter<int, char> {
  public:
    // TRAITS
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // MANIPULATORS
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                           t_PARSE_CONTEXT&
                                                      pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(bsl::format_error("not implemented"));
        }
        return pc.begin();
    }

    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                       int                            x,
                                       t_FORMAT_CONTEXT& fc) const
    {
        typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;
        char  buf[NFUtil::ToCharsMaxLength<int>::k_VALUE];
        char *result = NFUtil::toChars(buf, buf + sizeof(buf), x);
        return bsl::copy(buf, result, fc.out());
    }
};

template <class t_CHAR>
struct formatter<basic_string_view<t_CHAR>, t_CHAR> {
  public:
    // TRAITS
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // MANIPULATORS
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                           t_PARSE_CONTEXT& pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(bsl::format_error("not implemented"));
        }
        return pc.begin();
    }

    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                            basic_string_view<t_CHAR> sv,
                                            t_FORMAT_CONTEXT&         fc) const
    {
        return bsl::copy(sv.begin(), sv.end(), fc.out());
    }
};

}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
namespace std {
// FORMATTER SPECIALIZATIONS
template <class t_CHAR>
struct formatter<bsl::basic_string<t_CHAR>, t_CHAR>
: bsl::formatter<bsl::basic_string_view<t_CHAR>, t_CHAR> {
};
}  // close namespace bsl
#endif

//#endif // End C++11 code

#endif  // INCLUDED_BSLSTL_FORMATIMP

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
