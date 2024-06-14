// bslfmt_formatimp.h                                                    -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATIMP
#define INCLUDED_BSLFMT_FORMATIMP

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
#include <bslfmt_formatarg.h>
#include <bslfmt_formatcontext.h>

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

template <class t_ITERATOR>
class bslstl_format_TruncatingIterator {
  private:
    // TYPES
    typedef typename bsl::iterator_traits<t_ITERATOR>::difference_type DT;

    // DATA
    t_ITERATOR d_iterator;
    DT         d_limit;
    DT         d_count;

  public:
    // TYPES
    typedef bsl::output_iterator_tag iterator_category;
    typedef void                     difference_type;
    typedef void                     value_type;
    typedef void                     reference;
    typedef void                     pointer;

    // CREATORS
    bslstl_format_TruncatingIterator(t_ITERATOR iterator, DT limit)
    : d_iterator(iterator)
    , d_limit(limit)
    , d_count(0)
    {
    }

    // MANIPULATORS
    bslstl_format_TruncatingIterator& operator*()
    {
        return *this;
    }

    void operator=(typename bsl::iterator_traits<t_ITERATOR>::value_type x)
    {
        if (d_count++ < d_limit) {
            *d_iterator++ = x;
        }
    }

    bslstl_format_TruncatingIterator& operator++()
    {
        return *this;
    }

    bslstl_format_TruncatingIterator operator++(int)
    {
        return *this;
    }

    // ACCESSORS
    DT count() const
    {
        return d_count;
    }

    t_ITERATOR underlying() const
    {
        return d_iterator;
    }
};

template <class t_OUT>
struct format_to_n_result {
    t_OUT                                                 out;
    typename bsl::iterator_traits<t_OUT>::difference_type size;
};

template <class t_OUT, class t_CHAR>
struct bslstl_format_FormatVisitor {
    basic_format_parse_context<t_CHAR>  *d_parseContext_p;
    basic_format_context<t_OUT, t_CHAR> *d_formatContext_p;

    bslstl_format_FormatVisitor(basic_format_parse_context<t_CHAR>&  pc,
                                basic_format_context<t_OUT, t_CHAR>& fc)
    : d_parseContext_p(&pc)
    , d_formatContext_p(&fc)
    {
    }

    typedef
        typename basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::handle
            handle;

    //template <class t_TYPE>
    //void operator()(t_TYPE) const
    //{
    //    BSLS_THROW(format_error("this argument type isn't supported yet"));
    //}

    void operator()(bsl::monostate) const
    {
        BSLMF_ASSERT(false);
        //BSLS_THROW(format_error("this argument type isn't supported yet"));
    }

    void operator()(bool) const
    {
        BSLS_THROW(format_error("this argument type isn't supported yet"));
    }

    void operator()(t_CHAR) const
    {
        BSLS_THROW(format_error("this argument type isn't supported yet"));
    }

    void operator()(unsigned) const
    {
        BSLS_THROW(format_error("this argument type isn't supported yet"));
    }

    void operator()(long long) const
    {
        BSLS_THROW(format_error("this argument type isn't supported yet"));
    }

    void operator()(unsigned long long) const
    {
        BSLS_THROW(format_error("this argument type isn't supported yet"));
    }

    void operator()(float) const
    {
        BSLS_THROW(format_error("this argument type isn't supported yet"));
    }

    void operator()(double) const
    {
        BSLS_THROW(format_error("this argument type isn't supported yet"));
    }

    void operator()(long double) const
    {
        BSLS_THROW(format_error("this argument type isn't supported yet"));
    }

    void operator()(const t_CHAR *) const
    {
        BSLS_THROW(format_error("this argument type isn't supported yet"));
    }

    void operator()(const void *) const
    {
        BSLS_THROW(format_error("this argument type isn't supported yet"));
    }

    void operator()(int x) const
    {
        bsl::formatter<int, t_CHAR> f;
        d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
        d_formatContext_p->advance_to(
                               bsl::as_const(f).format(x, *d_formatContext_p));
    }

    void operator()(bsl::basic_string_view<t_CHAR> sv) const
    {
        bsl::formatter<bsl::basic_string_view<t_CHAR>, t_CHAR> f;
        d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
        d_formatContext_p->advance_to(
                              bsl::as_const(f).format(sv, *d_formatContext_p));
    }

    void operator()(const handle& h) const
    {
        h.format(*d_parseContext_p, *d_formatContext_p);
    }
};

template <class t_OUT, class t_CHAR>
t_OUT bslstl_format_VFormatImpl(
    t_OUT                                                               out,
    bsl::basic_string_view<t_CHAR>                                      fmtstr,
    bslstl_format_BasicFormatArgs<basic_format_context<t_OUT, t_CHAR> > args)
    // The actual meat of the implementation.  This overload is used when the
    // iterator type 't_OUT' matches the iterator type that 'args' is able to
    // format to.  In all other cases the iterator must be wrapped.
{
    typedef basic_format_context<t_OUT, t_CHAR> FC;

    basic_format_parse_context<t_CHAR>                pc(fmtstr, args.size());
    FC                                                fc(out, args);
    typename bsl::basic_string_view<t_CHAR>::iterator it = pc.begin();
    bslstl_format_FormatVisitor<t_OUT, t_CHAR>        visitor(pc, fc);

    while (it != pc.end()) {
        if (*it == '{') {
            ++it;
            if (it == pc.end()) {
                BSLS_THROW(format_error("unmatched {"));
            }
            else if (*it == '{') {
                // literal {
                ++it;
                out    = fc.out();
                *out++ = '{';
                fc.advance_to(out);
                continue;
            }
            size_t id = -1;
            if (*it >= '0' && *it <= '9') {
                // numeric ID
                id = 0;
                while (it != pc.end() && *it >= '0' && *it <= '9') {
                    id = 10 * id + (*it++ - '0');
                    if (id >= args.size()) {
                        BSLS_THROW(format_error("arg id too large"));
                    }
                }
                if (it == pc.end()) {
                    BSLS_THROW(format_error("unmatched {"));
                }
            }
            if (id == size_t(-1)) {
                id = pc.next_arg_id();
            }
            else {
                pc.check_arg_id(id);
            }
            if (*it == ':') {
                ++it;
            }
            pc.advance_to(it);
            visit_format_arg(visitor, args.get(id));
            it = pc.begin();
            if (it != pc.end()) {
                // advance past the terminating }
                ++it;
            }
        }
        else if (*it == '}') {
            // must be escaped
            ++it;
            if (it == pc.end() || *it != '}') {
                BSLS_THROW(format_error("} must be escaped"));
            }
            ++it;
            out    = fc.out();
            *out++ = '}';
            fc.advance_to(out);
        }
        else {
            // just copy it
            out    = fc.out();
            *out++ = *it++;
            fc.advance_to(out);
        }
    }
    return fc.out();
}

template <class t_OUT, class t_CHAR, class t_CONTEXT>
t_OUT bslstl_format_VFormatImpl(
                               t_OUT                                    out,
                               bsl::basic_string_view<t_CHAR>           fmtstr,
                               bslstl_format_BasicFormatArgs<t_CONTEXT> args)
{
    bslstl_format_OutputIteratorImpl<char, t_OUT> wrappedOut(out);
    bslstl_format_VFormatImpl(
                            bslstl_format_OutputIteratorRef<char>(&wrappedOut),
                            fmtstr,
                            args);
    return out;
}

template <class t_OUT>
t_OUT vformat_to(t_OUT out, bsl::string_view fmtstr, format_args args)
{
    return bslstl_format_VFormatImpl(out, fmtstr, args);
}


template <class t_OUT>
t_OUT vformat_to(t_OUT out, bsl::wstring_view fmtstr, wformat_args args)
{
    return bslstl_format_VFormatImpl(out, fmtstr, args);
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_OUT, class... t_ARGS>
t_OUT format_to(t_OUT out, bsl::string_view fmtstr, const t_ARGS&... args)
{
    typedef basic_format_context<t_OUT, char>      Context;
    typedef bslstl_format_BasicFormatArgs<Context> Args;
    return bslstl_format_VFormatImpl(
                         out,
                         fmtstr,
                         Args(bslstl_format_MakeFormatArgs<Context>(args...)));
}

template <class t_OUT, class... t_ARGS>
t_OUT format_to(t_OUT out, bsl::wstring_view fmtstr, const t_ARGS&... args)
{
    typedef basic_format_context<t_OUT, wchar_t>      Context;
    typedef bslstl_format_BasicFormatArgs<Context> Args;
    return bslstl_format_VFormatImpl(
                         out,
                         fmtstr,
                         Args(bslstl_format_MakeFormatArgs<Context>(args...)));
}

template <class... t_ARGS>
void format_to(bsl::string      *out,
               bsl::string_view  fmtstr,
               const t_ARGS&...  args)
{
    format_to(bsl::back_inserter(*out), fmtstr, args...);
}

template <class... t_ARGS>
void format_to(bsl::wstring      *out,
               bsl::wstring_view  fmtstr,
               const t_ARGS&...   args)
{
    format_to(bsl::back_inserter(*out), fmtstr, args...);
}

inline
void vformat_to(bsl::string *out, bsl::string_view fmtstr, format_args args)
{
    vformat_to(bsl::back_inserter(*out), fmtstr, args);
}

template <class... t_ARGS>
bsl::string format(bsl::string_view fmtstr, const t_ARGS&... args)
{
    bsl::string result;
    format_to(&result, fmtstr, args...);
    return result;
}

template <class... t_ARGS>
bsl::wstring format(bsl::wstring_view fmtstr, const t_ARGS&... args)
{
    bsl::wstring result;
    format_to(&result, fmtstr, args...);
    return result;
}

template <class... t_ARGS>
bsl::string format(bsl::allocator<char> alloc,
                   bsl::string_view     fmtstr,
                   const t_ARGS&...     args)
{
    bsl::string result(alloc);
    format_to(&result, fmtstr, args...);
    return result;
}


template <class... t_ARGS>
bsl::wstring format(bsl::allocator<wchar_t> alloc,
                    bsl::wstring_view       fmtstr,
                    const t_ARGS&...       args)
{
    bsl::wstring result(alloc);
    format_to(&result, fmtstr, args...);
    return result;
}

inline
bsl::string vformat(bsl::string_view fmt, format_args args)
{
    bsl::string result;
    vformat_to(&result, fmt, args);
    return result;
}

inline
bsl::string
vformat(bsl::allocator<char> alloc, bsl::string_view fmt, format_args args)
{
    bsl::string result(alloc);
    vformat_to(&result, fmt, args);
    return result;
}

template <class... t_ARGS>
std::size_t formatted_size(bsl::string_view fmtstr, const t_ARGS&... args)
{
    bslstl_format_TruncatingIterator<char *> it(0, 0);
    format_to(it, fmtstr, args...);
    return it.count();
}

template <class t_OUT, class... t_ARGS>
format_to_n_result<t_OUT> format_to_n(
                  t_OUT                                                 out,
                  typename bsl::iterator_traits<t_OUT>::difference_type n,
                  bsl::string_view                                      fmtstr,
                  const t_ARGS&...                                      args)
{
    if (n < 0)
        n = 0;
    bslstl_format_TruncatingIterator<t_OUT> it(out, n);
    format_to(it, fmtstr, args...);
    format_to_n_result<t_OUT> result;
    result.out  = it.underlying();
    result.size = it.count();
    return result;
}
#endif // end C++11 code
}  // close namespace bslfmt
} // close enterprise namespace

namespace bsl {
// TEMPORARY HACKS TO MAKE THE FORMATTER BIT WORK
    using BloombergLP::bslfmt::basic_format_context;
    using BloombergLP::bslfmt::format_context;
    using BloombergLP::bslfmt::wformat_context;
    using BloombergLP::bslfmt::basic_format_parse_context;
    using BloombergLP::bslfmt::format_parse_context;
    using BloombergLP::bslfmt::wformat_parse_context;
    using BloombergLP::bslfmt::format_error;
}


//#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
//namespace std {
//// FORMATTER SPECIALIZATIONS
//template <>
//struct formatter<bsl::string, char> : formatter<bsl::string_view, char> {
//};
//}  // close namespace bsl
//#endif

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
