// bslfmt_formatcontext.h                                             -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATCONTEXT
#define INCLUDED_BSLFMT_FORMATCONTEXT

#include <bslscm_version.h>

#include <bslalg_numericformatterutil.h>

#include <bslmf_assert.h>
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

namespace BloombergLP {
namespace bslfmt {

// FORWARD DECLARATIONS

template <class t_OUT, class t_CHAR>
class basic_format_context;

template <class t_VALUE>
class Format_OutputIteratorRef;

template <class t_CONTEXT>
class basic_format_arg;

template <class t_CONTEXT>
class basic_format_args;

// TYPEDEFS

typedef basic_format_context<Format_OutputIteratorRef<char>, char>
    format_context;

typedef basic_format_context<Format_OutputIteratorRef<wchar_t>, wchar_t>
    wformat_context;

                 // ----------------------------------------
                  // class Format_OutputIteratorBase<t_VALUE>
                  // ----------------------------------------

template <class t_VALUE>
class Format_OutputIteratorBase {
  public:
    // MANIPULATORS
    virtual void put(t_VALUE) = 0;
};

              // ------------------------------------------------
              // class Format_OutputIteratorImpl<t_VALUE, t_ITER>
              // ------------------------------------------------

template <class t_VALUE, class t_ITER>
class Format_OutputIteratorImpl
: public Format_OutputIteratorBase<t_VALUE> {
  private:
    // DATA
    t_ITER& d_iter;

  public:
    // CREATORS
    Format_OutputIteratorImpl(t_ITER& iter);

    // MANIPULATORS
    void put(t_VALUE x) BSLS_KEYWORD_OVERRIDE;
};

                  // ---------------------------------------
                  // class Format_OutputIteratorRef<t_VALUE>
                  // ---------------------------------------

template <class t_VALUE>
class Format_OutputIteratorRef {
  private:
    // DATA
    Format_OutputIteratorBase<t_VALUE> *d_base_p;

  public:
    // TYPES
    typedef bsl::output_iterator_tag iterator_category;
    typedef void                     difference_type;
    typedef t_VALUE                  value_type;
    typedef void                     reference;
    typedef void                     pointer;

    // CREATORS
    Format_OutputIteratorRef(Format_OutputIteratorBase<t_VALUE> *base);

    // MANIPULATORS
    Format_OutputIteratorRef& operator*();

    void operator=(t_VALUE x);

    Format_OutputIteratorRef& operator++();

    Format_OutputIteratorRef operator++(int);
};

                 // -----------------------------------------
                 // class basic_format_context<t_OUT, t_CHAR>
                 // -----------------------------------------

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
    basic_format_context(t_OUT                                          out,
                         const basic_format_args<basic_format_context>& args);

    // FRIENDS
    friend class Format_FormatContextFactory;

  public:

    // MANIPULATORS
    iterator out();

    void advance_to(iterator it);

    // ACCESSORS
    Arg arg(size_t id) const BSLS_KEYWORD_NOEXCEPT;
};


                      // ---------------------------------
                      // class Format_FormatContextFactory
                      // ---------------------------------

/// This class provides utility functions to enable manipulation of types
/// declared by this component. It is solely for private use by other
/// components of the `bslfmt` package and should not be used directly.
struct Format_FormatContextFactory {
  public:
    // CLASS METHODS

    /// Construct a `basic_format_context` object holding the specified `out`
    /// output iterator and a `basic_format_args` object containing the
    /// specified `fmt_args` values.
    template <class t_OUT, class t_CHAR>
    static basic_format_context<t_OUT, t_CHAR> construct(
          t_OUT                                                          out,
          const basic_format_args<basic_format_context<t_OUT, t_CHAR> >& args);
};


// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================


              // ------------------------------------------------
              // class Format_OutputIteratorImpl<t_VALUE, t_ITER>
              // ------------------------------------------------


// CREATORS
template <class t_VALUE, class t_ITER>
Format_OutputIteratorImpl<t_VALUE, t_ITER>::Format_OutputIteratorImpl(
                                                                  t_ITER& iter)
: d_iter(iter)
{
    typedef typename bsl::iterator_traits<t_ITER>::value_type valuetype;

    // We allow void because the standard library
    // `back_insert_iterator` has a `value_type` of `void`.
    BSLMF_ASSERT((bsl::is_same<valuetype, void>::value ||
                  bsl::is_same<valuetype, t_VALUE>::value));
}

// MANIPULATORS
template <class t_VALUE, class t_ITER>
void Format_OutputIteratorImpl<t_VALUE, t_ITER>::put(
                                               t_VALUE x)
{
    // We cannot use postfix increment in case `d_iter` holds state.
    *d_iter = x;
    ++d_iter;
}


                  // ---------------------------------------
                  // class Format_OutputIteratorRef<t_VALUE>
                  // ---------------------------------------

// CREATORS
template <class t_VALUE>
Format_OutputIteratorRef<t_VALUE>::Format_OutputIteratorRef(
                                      Format_OutputIteratorBase<t_VALUE> *base)
: d_base_p(base)
{
}

// MANIPULATORS
template <class t_VALUE>
Format_OutputIteratorRef<t_VALUE>&
Format_OutputIteratorRef<t_VALUE>::operator*()
{
    return *this;
}

template <class t_VALUE>
void Format_OutputIteratorRef<t_VALUE>::operator=(t_VALUE x)
{
    d_base_p->put(x);
}

template <class t_VALUE>
Format_OutputIteratorRef<t_VALUE>&
Format_OutputIteratorRef<t_VALUE>::operator++()
{
    return *this;
}

template <class t_VALUE>
Format_OutputIteratorRef<t_VALUE>
Format_OutputIteratorRef<t_VALUE>::operator++(int)
{
    return *this;
}

                 // -----------------------------------------
                 // class basic_format_context<t_OUT, t_CHAR>
                 // -----------------------------------------

// PRIVATE CREATORS

template <class t_OUT, class t_CHAR>
basic_format_context<t_OUT, t_CHAR>::basic_format_context(
                           t_OUT                                          out,
                           const basic_format_args<basic_format_context>& args)
: d_args(args)
, d_out(out)
{
}

// MANIPULATORS

template <class t_OUT, class t_CHAR>
typename basic_format_context<t_OUT, t_CHAR>::iterator
basic_format_context<t_OUT, t_CHAR>::out()
{
    return BloombergLP::bslmf::MovableRefUtil::move(d_out);
}

template <class t_OUT, class t_CHAR>
void basic_format_context<t_OUT, t_CHAR>::advance_to(iterator it)
{
    d_out = BloombergLP::bslmf::MovableRefUtil::move(it);
}

// ACCESSORS

template <class t_OUT, class t_CHAR>
typename basic_format_context<t_OUT, t_CHAR>::Arg
basic_format_context<t_OUT, t_CHAR>::arg(size_t id) const BSLS_KEYWORD_NOEXCEPT
{
    return d_args.get(id);
}


                     // ---------------------------------
                     // class Format_FormatContextFactory
                     // ---------------------------------


// CLASS METHODS

template <class t_OUT, class t_CHAR>
basic_format_context<t_OUT, t_CHAR> Format_FormatContextFactory::construct(
           t_OUT                                                          out,
           const basic_format_args<basic_format_context<t_OUT, t_CHAR> >& args)
{
    return basic_format_context<t_OUT, t_CHAR>(out, args);
}


}  // close namespace bslfmt
} // close enterprise namespace

#endif  // INCLUDED_BSLFMT_FORMATCONTEXT

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
