// bslfmt_formattercharacter.h                                        -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERCHARACTER
#define INCLUDED_BSLFMT_FORMATTERCHARACTER

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
#include <bslfmt_formatterbase.h>

#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'

#include <stdio.h>    // for 'snprintf'

namespace BloombergLP {
namespace bslfmt {

template <class t_VALUE, class t_CHAR, class t_DERIVED>
struct Formatter_CharacterBase {
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
        typename t_FORMAT_CONTEXT::iterator o = fc.out();

        t_DERIVED::doCharOutput(&o, x);

        return o;
    }
};

}  // close namespace bslfmt
}  // close enterprise namespace

namespace bsl {
// FORMATTER SPECIALIZATIONS

template <class t_CHAR>
struct formatter<t_CHAR, t_CHAR> 
: public BloombergLP::bslfmt::
      Formatter_CharacterBase<t_CHAR, t_CHAR, formatter<t_CHAR, t_CHAR> > {
  private:
    // FRIENDS
    template <class t_INNER_VALUE, class t_INNER_CHAR, class t_INNER_DERIVED>
    friend struct BloombergLP::bslfmt::Formatter_CharacterBase;

    // PRIVATE MANIPULATORS
    template <class t_ITERATOR>
    static void doCharOutput(t_ITERATOR *o, t_CHAR x)
    {
        *(*o)++ = x;
    }
};

template<>
struct formatter<char, wchar_t>
: public BloombergLP::bslfmt::
      Formatter_CharacterBase<char, wchar_t, formatter<char, wchar_t> > {
  private:
    // FRIENDS
    template <class t_INNER_VALUE, class t_INNER_CHAR, class t_INNER_DERIVED>
    friend struct BloombergLP::bslfmt::Formatter_CharacterBase;

    // PRIVATE MANIPULATORS
    template <class t_ITERATOR>
    static void doCharOutput(t_ITERATOR *o, char x)
    {
        *(*o)++ = x;
    }
};

#if 0
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
#endif


}

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
