// bslfmt_formatterbase.h                                             -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERBASE
#define INCLUDED_BSLFMT_FORMATTERBASE

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

#include <bslfmt_formaterror.h>

#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'

#include <stdio.h>    // for 'snprintf'

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
struct Formatter_IsStdAliasingEnabled : bsl::true_type {
};

template <class t_FORMATTER>
struct Formatter_IsStdAliasingEnabled<
    t_FORMATTER,
    typename t_FORMATTER::Formatter_PreventStdPromotion> : bsl::false_type {
};

}  // close namespace bslfmt
}  // close enterprise namespace

namespace std {

template <class t_ARG, class t_CHAR>
struct formatter;

template <class t_ARG, class t_CHAR>
requires(
    BloombergLP::bslfmt::Formatter_IsStdAliasingEnabled<
        bsl::formatter<t_ARG, t_CHAR> >::value
)
struct formatter<t_ARG, t_CHAR>
: bsl::formatter<t_ARG, t_CHAR> {};

}  // close namespace std
#endif


namespace BloombergLP {
namespace bslfmt {

template <class t_CHAR>
struct Formatter_CharUtils {
  public:
    // CLASS METHODS
};


template <>
struct Formatter_CharUtils<char> {
  public:
    // CLASS METHODS
    template <class t_FORMAT_CONTEXT>
    static typename t_FORMAT_CONTEXT::iterator outputFromChar(
                                                       const char       *begin,
                                                       const char       *end,
                                                       t_FORMAT_CONTEXT& fc)
    {
        return bsl::copy(begin, end, fc.out());
    }
};


template <>
struct Formatter_CharUtils<wchar_t> {
  public:
    // CLASS METHODS
    template <class t_FORMAT_CONTEXT>
    static typename t_FORMAT_CONTEXT::iterator outputFromChar(
                                                      const char        *begin,
                                                      const char        *end,
                                                      t_FORMAT_CONTEXT&  fc)
    {
        static const std::ctype<wchar_t>& ct =
                  std::use_facet<std::ctype<wchar_t> >(std::locale::classic());
        typename t_FORMAT_CONTEXT::iterator out = fc.out();

        for (; begin != end; (void)++begin, (void)++out) {
            *out = ct.widen(*begin);
        }

        return out;
    }
};

template <class t_VALUE, class t_CHAR>
struct Formatter_IntegerBase {
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
    typename t_FORMAT_CONTEXT::iterator format(t_VALUE           x,
                                               t_FORMAT_CONTEXT& fc) const
    {
        typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;
        char  buf[NFUtil::ToCharsMaxLength<t_VALUE>::k_VALUE];
        char *result = NFUtil::toChars(buf, buf + sizeof(buf), x);
        return BloombergLP::bslfmt::Formatter_CharUtils<
            t_CHAR>::outputFromChar(buf, result, fc);
    }
};

template <class t_VALUE, class t_CHAR>
struct Formatter_FloatingBase {
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
    typename t_FORMAT_CONTEXT::iterator format(t_VALUE           x,
                                               t_FORMAT_CONTEXT& fc) const
    {
        typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;
        char  buf[NFUtil::ToCharsMaxLength<double>::k_VALUE];
        char *result = NFUtil::toChars(buf, buf + sizeof(buf), (double)x);
        return BloombergLP::bslfmt::Formatter_CharUtils<
            t_CHAR>::outputFromChar(buf, result, fc);
    }
};

}  // close namespace bslfmt
}  // close enterprise namespace

namespace bsl {
// FORMATTER SPECIALIZATIONS

template <class t_CHAR>
struct formatter<bool, t_CHAR> {
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
    typename t_FORMAT_CONTEXT::iterator format(bool            v,
                                               t_FORMAT_CONTEXT& fc) const
    {
        const char *buf = v ? "1" : "0";

        return BloombergLP::bslfmt::Formatter_CharUtils<
            t_CHAR>::outputFromChar(buf, buf + 1, fc);
    }
};

template <class t_CHAR>
struct formatter<int, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<int, t_CHAR> {
};

template <class t_CHAR>
struct formatter<unsigned, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<unsigned, t_CHAR> {
};

template <class t_CHAR>
struct formatter<long long, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<long long, t_CHAR> {
};

template <class t_CHAR>
struct formatter<unsigned long long, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<unsigned long long, t_CHAR> {
};

template <class t_CHAR>
struct formatter<float, t_CHAR>
: BloombergLP::bslfmt::Formatter_FloatingBase<float, t_CHAR> {
};

template <class t_CHAR>
struct formatter<double, t_CHAR>
: BloombergLP::bslfmt::Formatter_FloatingBase<double, t_CHAR> {
};

template <class t_CHAR>
struct formatter<long double, t_CHAR>
: BloombergLP::bslfmt::Formatter_FloatingBase<long double, t_CHAR> {
};


template <class t_CHAR>
struct formatter<const void *, t_CHAR> {
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
    typename t_FORMAT_CONTEXT::iterator format(const void *              v,
                                               t_FORMAT_CONTEXT& fc) const
    {
        char buf[64];
        size_t req = snprintf(buf, sizeof(buf), "%p", v);

        if (req >= sizeof(buf))
            BSLS_THROW(bsl::format_error("buffer overflow"));

        return BloombergLP::bslfmt::Formatter_CharUtils<
            t_CHAR>::outputFromChar(buf, buf + req, fc);
    }
};


template <class t_CHAR>
struct formatter<t_CHAR, t_CHAR> {
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
    typename t_FORMAT_CONTEXT::iterator format(t_CHAR            c,
                                               t_FORMAT_CONTEXT& fc) const
    {
        typename t_FORMAT_CONTEXT::iterator o = fc.out();

        *o++ = c;
        return o;
    }
};

template <class t_CHAR>
struct formatter<const t_CHAR *, t_CHAR> {
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
    typename t_FORMAT_CONTEXT::iterator format(const t_CHAR      *c,
                                               t_FORMAT_CONTEXT&  fc) const
    {
        basic_string_view<t_CHAR> sv(c);
        return bsl::copy(sv.begin(), sv.end(), fc.out());
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

#endif  // INCLUDED_BSLFMT_FORMATTERBASE

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
