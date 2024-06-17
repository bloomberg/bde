// bslfmt_formatimp.h                                                    -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATCONTEXT
#define INCLUDED_BSLFMT_FORMATCONTEXT

#include <bslscm_version.h>

#include <bslalg_numericformatterutil.h>

#include <bslmf_conditional.h>
#include <bslmf_enableif.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isintegral.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>

#include <bsls_compilerfeatures.h>
#include <bsls_exceptionutil.h>
#include <bsls_libraryfeatures.h>
#include <bsls_unspecifiedbool.h>
#include <bsls_util.h>

#include <bslstl_iterator.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>
#include <bslstl_array.h>
#include <bslstl_monostate.h>
#include <bslstl_utility.h>
#include <bslstl_variant.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Tue Jun 11 07:56:24 2024
// Command line: sim_cpp11_features.pl bslfmt_formatimp.h
# define COMPILING_BSLFMT_FORMATIMP_H
# include <bslfmt_formatimp_cpp03.h>
# undef COMPILING_BSLFMT_FORMATIMP_H
#else


namespace BloombergLP {
namespace bslfmt {

template <class t_VALUE>
class Format_OutputIteratorBase {
  public:
    // MANIPULATORS
    virtual void put(t_VALUE) = 0;
};

template <class t_VALUE, class t_ITER>
class Format_OutputIteratorImpl
: public Format_OutputIteratorBase<t_VALUE> {
  private:
    // DATA
    t_ITER& d_iter;

  public:
    // CREATORS
    Format_OutputIteratorImpl(t_ITER& iter)
    : d_iter(iter)
    {
    }

    // MANIPULATORS
    void put(t_VALUE x) BSLS_KEYWORD_OVERRIDE
    {
        *d_iter++ = x;
    }
};

template <class t_VALUE>
class Format_OutputIteratorRef {
  private:
    // DATA
    Format_OutputIteratorBase<t_VALUE> *d_base_p;

  public:
    // TYPES
    typedef bsl::output_iterator_tag iterator_category;
    typedef void                     difference_type;
    typedef void                     value_type;
    typedef void                     reference;
    typedef void                     pointer;

    // CREATORS
    Format_OutputIteratorRef(
                               Format_OutputIteratorBase<t_VALUE> *base)
    : d_base_p(base)
    {
    }

    // MANIPULATORS
    Format_OutputIteratorRef& operator*()
    {
        return *this;
    }

    void operator=(t_VALUE x)
    {
        d_base_p->put(x);
    }

    Format_OutputIteratorRef& operator++()
    {
        return *this;
    }

    Format_OutputIteratorRef operator++(int)
    {
        return *this;
    }
};

template <class t_OUT, class t_CHAR>
class basic_format_context;

template <class t_CHAR>
class basic_format_parse_context {
  public:
    // TYPES
    typedef t_CHAR char_type;
    typedef
        typename bsl::basic_string_view<t_CHAR>::const_iterator const_iterator;
    typedef const_iterator                                      iterator;

  private:
    // TYPES
    enum Indexing { e_UNKNOWN, e_MANUAL, e_AUTOMATIC };

    // DATA
    iterator d_begin;
    iterator d_end;
    Indexing d_indexing;
    size_t   d_next_arg_id;
    size_t   d_num_args;

  public:
    // CREATORS
    BSLS_KEYWORD_CONSTEXPR_CPP20 explicit basic_format_parse_context(
                      bsl::basic_string_view<t_CHAR> fmt) BSLS_KEYWORD_NOEXCEPT
    : d_begin(fmt.begin())
    , d_end(fmt.end())
    , d_indexing(e_UNKNOWN)
    , d_next_arg_id(0)
    , d_num_args(0)
    {
    }

    BSLS_KEYWORD_CONSTEXPR_CPP20 explicit basic_format_parse_context(
                  bsl::basic_string_view<t_CHAR> fmt,
                  size_t                         numArgs) BSLS_KEYWORD_NOEXCEPT
    : d_begin(fmt.begin())
    , d_end(fmt.end())
    , d_indexing(e_UNKNOWN)
    , d_next_arg_id(0)
    , d_num_args(numArgs)
    {
    }

  public:

    // MANIPULATORS
    BSLS_KEYWORD_CONSTEXPR_CPP20 void advance_to(const_iterator it)
    {
        d_begin = it;
    }

    BSLS_KEYWORD_CONSTEXPR_CPP20 size_t next_arg_id()
    {
        if (d_indexing == e_MANUAL) {
            BSLS_THROW(
                      format_error("mixing of automatic and manual indexing"));
        }
        if (d_next_arg_id >= d_num_args) {
            BSLS_THROW(format_error("number of conversion specifiers exceeds "
            "number of arguments"));
        }
        if (d_indexing == e_UNKNOWN) {
            d_indexing = e_AUTOMATIC;
        }
        return d_next_arg_id++;
    }

    BSLS_KEYWORD_CONSTEXPR_CPP20 void check_arg_id(size_t id)
    {
        if (d_indexing == e_AUTOMATIC) {
            BSLS_THROW(
                      format_error("mixing of automatic and manual indexing"));
        }
        if (id >= d_num_args) {
            BSLS_THROW(format_error("invalid argument index"));
        }
        if (d_indexing == e_UNKNOWN) {
            d_indexing = e_MANUAL;
        }
    }

    // ACCESSORS
    BSLS_KEYWORD_CONSTEXPR_CPP20 const_iterator
    begin() const BSLS_KEYWORD_NOEXCEPT
    {
        return d_begin;
    }

    BSLS_KEYWORD_CONSTEXPR_CPP20 const_iterator
    end() const BSLS_KEYWORD_NOEXCEPT
    {
        return d_end;
    }

  private:
    // NOT IMPLEMENTED
    basic_format_parse_context(
                       const basic_format_parse_context&) BSLS_KEYWORD_DELETED;
    basic_format_parse_context& operator=(
                       const basic_format_parse_context&) BSLS_KEYWORD_DELETED;
};

typedef basic_format_parse_context<char> format_parse_context;

typedef basic_format_parse_context<wchar_t> wformat_parse_context;

typedef basic_format_context<Format_OutputIteratorRef<char>, char>
    format_context;

typedef basic_format_context<Format_OutputIteratorRef<wchar_t>, wchar_t>
    wformat_context;


template <class t_CONTEXT>
class basic_format_arg;

template <class t_CONTEXT>
class basic_format_args;

template <class t_OUT, class t_CHAR>
class basic_format_context {
  private:
    // TYPES
    typedef basic_format_arg<basic_format_context> Arg;

    // DATA
    basic_format_args<basic_format_context> d_args;
    t_OUT                                               d_out;

  public:
    // TYPES
    typedef t_OUT  iterator;
    typedef t_CHAR char_type;

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
    template <class t_TYPE>
    using formatter_type = bsl::formatter<t_TYPE, t_CHAR>;
#endif

  private:
    // PRIVATE CREATORS

    basic_format_context(
                      t_OUT                                               out,
                      const basic_format_args<basic_format_context> &args)
    : d_args(args)
    , d_out(out)
    {
    }

    // FRIENDS
    template <class t_OUT, class t_CHAR>
    friend basic_format_context<t_OUT, t_CHAR> Format_FormatContextFactory(
          t_OUT                                                          out,
          const basic_format_args<basic_format_context<t_OUT, t_CHAR> >& args);

  public:

    // MANIPULATORS
    iterator out()
    {
        return BloombergLP::bslmf::MovableRefUtil::move(d_out);
    }

    void advance_to(iterator it)
    {
        d_out = BloombergLP::bslmf::MovableRefUtil::move(it);
    }

    // ACCESSORS
    Arg arg(size_t id) const BSLS_KEYWORD_NOEXCEPT
    {
        return d_args.get(id);
    }
};

template <class t_OUT, class t_CHAR>
basic_format_context<t_OUT, t_CHAR> Format_FormatContextFactory(
           t_OUT                                                          out,
           const basic_format_args<basic_format_context<t_OUT, t_CHAR> >& args)
{
    return basic_format_context<t_OUT, t_CHAR>(out, args);
}

}  // close namespace bslfmt
} // close enterprise namespace


#endif // End C++11 code

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
