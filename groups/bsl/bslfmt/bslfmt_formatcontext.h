// bslfmt_formatcontext.h                                             -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATCONTEXT
#define INCLUDED_BSLFMT_FORMATCONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provides access to formatting state.
//
//@CLASSES:
//  bslfmt::basic_format_context: standard-compliant output context
//
//@DESCRIPTION: This component provides an implementation of the C++20 Standard
// Library's `std::basic_format_context`, which provides access to formatting
// state consisting of the formatting arguments and the output iterator.
//
// This type is designed to be constructed from within `bslfmt::format` and
// cannot be constructed directly from user code.
//
// As this type contains a `basic_format_args` type therefore all of the
// warnings around lifetime documented in `bslfmt_formatargs.h` also apply
// here.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example: 1 Testing a user defined formatter's `format` method.
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`. In addition, there
// are only a very limited number of public methods so this example is
// necessarily unrealistic.
//
// Suppose we have a user-defined formatter and want to test its `format`
// method:
//
//..
//  struct MyCharFormatter {
//      template <class t_FORMAT_CONTEXT>
//      typename t_FORMAT_CONTEXT::iterator format(
//          char              v,
//          t_FORMAT_CONTEXT& fc) const
//      {
//          typename t_FORMAT_CONTEXT::iterator out = fc.out();
//
//          *out++ = v;
//
//          return out;
//      }
//  };
//..
// 
// We can then write a test function. Note that it is not possible for users to
// construct a context directly, so we are forced to abuse the
// internal-use-only types `Format_OutputIteratorRef` and
// `Format_FormatContextFactory` in order to write this usage example.
//
//..
//  struct MyCharFormatterTestVisitor {
//      char value;
//
//      void operator()(char v) { value = v; }
//
//      template <class t_TYPE>
//      void operator()(const t_TYPE &) const
//      {
//          assert(false);  // Wrong Type
//      }
//  };
//
//  void testFormatter(bslfmt::format_args args)
//  {
//      MyCharFormatter                        f;
//      bsl::string                            result;
//      bsl::back_insert_iterator<bsl::string> backiter(result);
//      bslfmt::Format_OutputIteratorImpl<
//          char, bsl::back_insert_iterator<bsl::string> >
//                                             iter(backiter);
//      bslfmt::format_context fc = 
//          bslfmt::Format_FormatContextFactory::construct(
//                           bslfmt::Format_OutputIteratorRef<char>(&iter),
//                           args);
//      MyCharFormatterTestVisitor visitor;
//      bslfmt::visit_format_arg(visitor, fc.arg(0));
//      f.format(visitor.value, fc);
//      assert(result.size() == 1);
//      assert(result.front() == 'x');
//  }
//..
//
// Then we perform the test itself:
//
//..
//   char value = 'x';
//   testFormatter(bslfmt::make_format_args(value));
//..
//

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
                  // class Format_OutputIteratorBase<t_CHAR>
                  // ----------------------------------------

/// This component-private type is an abstract base required to type-erase
/// output iterators as required by the `format_context` and `wformat_context`
/// typedefs.
template <class t_CHAR>
class Format_OutputIteratorBase {
  public:
    // MANIPULATORS

    /// Write the specified `character` to the contained iterator then
    /// increment the contained iterator.
    virtual void put(t_CHAR character) = 0;
};

              // ------------------------------------------------
              // class Format_OutputIteratorImpl<t_CHAR, t_ITER>
              // ------------------------------------------------

/// This type holds a reference to an iterator, and is used to type-erase
/// output iterators as required by the `format_context` and `wformat_context`
/// typedefs. It is solely for private use by other components of the `bslfmt`
/// package and should not be used directly.
template <class t_CHAR, class t_ITER>
class Format_OutputIteratorImpl : public Format_OutputIteratorBase<t_CHAR> {
  private:
    // DATA
    t_ITER& d_iter;  // The contained iterator

  public:
    // CREATORS

    /// Create a instance containing a reference to the specified `iter` of
    /// parameter type `t_ITER`.
    Format_OutputIteratorImpl(t_ITER& iter);

    // MANIPULATORS

    /// Write the specified `character` to the contained iterator then
    /// increment the contained iterator.
    void put(t_CHAR character) BSLS_KEYWORD_OVERRIDE;
};

                  // --------------------------------------
                  // class Format_OutputIteratorRef<t_CHAR>
                  // --------------------------------------

/// This class provides a type-erased interface to an output iterator that it
/// holds indirectly via a reference to a `Format_OutputIteratorImpl` type. It
/// is solely for private use by other components of the `bslfmt` package and
/// should not be used directly.
template <class t_CHAR>
class Format_OutputIteratorRef {
  private:
    // DATA
    Format_OutputIteratorBase<t_CHAR> *d_base_p;
                                     // Pointer to Format_OutputIteratorImpl

  public:
    // TYPES
    typedef bsl::output_iterator_tag iterator_category;
    typedef void                     difference_type;
    typedef t_CHAR                   value_type;
    typedef void                     reference;
    typedef void                     pointer;

    // CREATORS

    /// Construct an instance of this type. The specified `base` should be a
    /// pointer to an `Format_OutputIteratorImpl` type whose lifetime must
    /// outlive that of the constructed instance.
    Format_OutputIteratorRef(Format_OutputIteratorBase<t_CHAR> *base);

    // MANIPULATORS

    /// Return a reference to `*this`. This method is provided to enable this
    /// type to satisfy the requirements of [output.iterators].
    Format_OutputIteratorRef& operator*();

    /// Call `put(x)` on the referenced `Format_OutputIteratorImpl` instance.
    /// This will result in the value being written to the underlying iterator
    /// referenced by the  `Format_OutputIteratorImpl` instance, and that
    /// underlying iterator then being incremented.
    void operator=(t_CHAR x);

    /// Do nothing. This method is provided to enable this type to satisfy the
    /// requirements of [output.iterators].
    Format_OutputIteratorRef& operator++();

    /// Do nothing. This method is provided to enable this type to satisfy the
    /// requirements of [output.iterators].
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
    basic_format_args<basic_format_context> d_args; // Format arguments
    t_OUT                                   d_out;  // Output iterator

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

    /// Create a context that contains a copies of the output iterator
    /// specified by `out` and of the arguments specified by `args`.
    basic_format_context(t_OUT                                          out,
                         const basic_format_args<basic_format_context>& args);

    // FRIENDS
    friend class Format_FormatContextFactory;

  public:

    // MANIPULATORS

    /// Update the contained iterator to that specified by `it`.
    void advance_to(iterator it);

    /// Return (by value) the contained output iterator. As this is returned by
    /// value, any changes will not be reflected in the contained iterator
    /// unless a subsequent call to `advance_to` is made.
    iterator out();

    // ACCESSORS

    /// Return a `basic_format_arg` type for the argument in the position
    /// specified by `id`, with indexing starting at zero. If `id` is not less
    /// than the number of contained arguments, a default-constructed
    /// `basic_format_arg` object is returned.
    Arg arg(size_t id) const BSLS_KEYWORD_NOEXCEPT;
};


                      // ---------------------------------
                      // class Format_FormatContextFactory
                      // ---------------------------------

/// This class provides utility functions to enable manipulation of types
/// declared by this component. It is solely for private use by other
/// components of the `bslfmt` package and should not be used directly.
class Format_FormatContextFactory {
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


              // -----------------------------------------------
              // class Format_OutputIteratorImpl<t_CHAR, t_ITER>
              // -----------------------------------------------


// CREATORS
template <class t_CHAR, class t_ITER>
Format_OutputIteratorImpl<t_CHAR, t_ITER>::Format_OutputIteratorImpl(
                                                                  t_ITER& iter)
: d_iter(iter)
{
    typedef typename bsl::iterator_traits<t_ITER>::value_type valuetype;

    // We allow void because the standard library
    // `back_insert_iterator` has a `value_type` of `void`.
    BSLMF_ASSERT((bsl::is_same<valuetype, void>::value ||
                  bsl::is_same<valuetype, t_CHAR>::value));
}

// MANIPULATORS
template <class t_CHAR, class t_ITER>
void Format_OutputIteratorImpl<t_CHAR, t_ITER>::put(t_CHAR character)
{
    // We cannot use postfix increment in case `d_iter` holds state.
    *d_iter = character;
    ++d_iter;
}


                  // --------------------------------------
                  // class Format_OutputIteratorRef<t_CHAR>
                  // --------------------------------------

// CREATORS
template <class t_CHAR>
Format_OutputIteratorRef<t_CHAR>::Format_OutputIteratorRef(
                                       Format_OutputIteratorBase<t_CHAR> *base)
: d_base_p(base)
{
}

// MANIPULATORS
template <class t_CHAR>
Format_OutputIteratorRef<t_CHAR>& Format_OutputIteratorRef<t_CHAR>::operator*()
{
    return *this;
}

template <class t_CHAR>
void Format_OutputIteratorRef<t_CHAR>::operator=(t_CHAR x)
{
    d_base_p->put(x);
}

template <class t_CHAR>
Format_OutputIteratorRef<t_CHAR>&
Format_OutputIteratorRef<t_CHAR>::operator++()
{
    return *this;
}

template <class t_CHAR>
Format_OutputIteratorRef<t_CHAR> Format_OutputIteratorRef<t_CHAR>::operator++(
                                                                           int)
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
