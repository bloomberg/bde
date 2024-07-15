// bslfmt_formatterbase.h                                             -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERBASE
#define INCLUDED_BSLFMT_FORMATTERBASE

#include <bslscm_version.h>

#include <bslalg_numericformatterutil.h>

#include <bslmf_assert.h>
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
// On earlier C++ compilers we use a dummy typedef to avoid the compiler
// warning about extra semicolons.
#define BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20                      \
    typedef void FormatImp_DoNotPreventStdPromotion_DummyTypedef
#endif

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202302L
#define BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP23                      \
    typedef void FormatImp_PreventStdPromotion
#else
// On earlier C++ compilers we use a dummy typedef to avoid the compiler
// warning about extra semicolons.
#define BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP23                      \
    typedef void FormatImp_DoNotPreventStdPromotion_DummyTypedef
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

#if 0
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
    template <class t_ITERATOR>
    static typename t_ITERATOR outputFromChar(const char  *begin,
                                              const char  *end,
                                              t_ITERATOR   out)
    {
        typedef typename bsl::iterator_traits<t_ITERATOR>::value_type
            valuetype;
        BSLMF_ASSERT((bsl::is_same<valuetype, char>::value));

        return bsl::copy(begin, end, out);
    }

    template <class t_ITERATOR>
    static typename t_ITERATOR outputFromChar(const char v, t_ITERATOR out)
    {
        typedef typename bsl::iterator_traits<t_ITERATOR>::value_type
            valuetype;
        BSLMF_ASSERT((bsl::is_same<valuetype, char>::value));

        *out++ = v;

        return out;
    }
};


template <>
struct Formatter_CharUtils<wchar_t> {
  public:
    // CLASS METHODS
    template <class t_ITERATOR>
    static typename t_ITERATOR outputFromChar(const char  *begin,
                                              const char  *end,
                                              t_ITERATOR   out)
    {
        typedef typename bsl::iterator_traits<t_ITERATOR>::value_type
            valuetype;
        BSLMF_ASSERT((bsl::is_same<valuetype, wchar_t>::value));

        static const std::ctype<wchar_t>& ct =
                  std::use_facet<std::ctype<wchar_t> >(std::locale::classic());

        for (; begin != end; (void)++begin, (void)++out) {
            *out = ct.widen(*begin);
        }

        return out;
    }

    template <class t_ITERATOR>
    static typename t_ITERATOR outputFromChar(const char v, t_ITERATOR out)
    {
        typedef typename bsl::iterator_traits<t_ITERATOR>::value_type
            valuetype;
        BSLMF_ASSERT((bsl::is_same<valuetype, wchar_t>::value));

        static const std::ctype<wchar_t>& ct =
                  std::use_facet<std::ctype<wchar_t> >(std::locale::classic());

        *out++ = ct.widen(v);

        return out;
    }
};

}  // close namespace bslfmt
}  // close enterprise namespace
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
