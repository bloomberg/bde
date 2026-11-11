// bslfmt_bufferformatcontext.h                                       -*-C++-*-
#ifndef INCLUDED_BSLFMT_BUFFERFORMATCONTEXT
#define INCLUDED_BSLFMT_BUFFERFORMATCONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a format context that writes to a string buffer.
//
//@CLASSES:
//  bslfmt::BufferFormatContext: format context appending to a string
//
//@SEE_ALSO: bslfmt_formatterpair, bslfmt_rangeformatter
//
//@DESCRIPTION: This component provides a `BufferFormatContext` class template
// that is a format context which appends what is written to it to a string,
// and which obtains its formatting arguments from another format context.  A
// formatter that has to know the rendered size of its output before writing
// it -- for example, to pad that output to a width -- can render into a
// `BufferFormatContext` first, and then copy the buffered text, together with
// any padding, to its own format context.  Because argument lookups are
// forwarded, a nested replacement field in a specification that is used with
// the buffer context, as in `{:{}}`, finds the same argument that it would
// find in the original context.
//
// This component is for use within `bslfmt` only.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Buffering Formatted Output
///- - - - - - - - - - - - - - - - - - -
// Suppose we wish to render an integer whose width is supplied by a format
// argument, and examine the result before writing it to the actual output.
//
// First, we parse a specification that takes the width from the first
// argument:
// ```
//  bslfmt::MockParseContext<char> parseContext("{}}", 1);
//
//  bsl::formatter<int, char> formatter;
//  parseContext.advance_to(formatter.parse(parseContext));
// ```
// Then, we create the format context that holds the arguments, the buffer, and
// a buffer context that writes to the buffer and obtains its arguments from
// the format context:
// ```
//  typedef bslfmt::MockFormatContext<char>                  FormatContext;
//  typedef bslfmt::BufferFormatContext<char, FormatContext> Context;
//
//  FormatContext formatContext(5);
//  bsl::string   buffer;
//  Context       bufferContext(&buffer, &formatContext);
// ```
// Now, we format a value into the buffer context:
// ```
//  bufferContext.advance_to(
//                     bsl::as_const(formatter).format(42, bufferContext));
// ```
// Finally, we observe that the value has been written to the buffer, padded to
// the width that the format context supplied:
// ```
//  assert("   42" == buffer);
// ```

#include <bslscm_version.h>

#include <bsls_libraryfeatures.h>

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

#include <bslfmt_format_arg.h>
#include <bslfmt_format_context.h>
#include <bslfmt_formatterbase.h>

#include <bslmf_util.h>

#include <bslstl_iterator.h>
#include <bslstl_string.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>

#include <cstddef>  // `std::size_t`

namespace BloombergLP {
namespace bslfmt {

                    // ==================================
                    // struct BufferFormatContext_ArgType
                    // ==================================

/// [**PRIVATE**] This meta-function provides a `type` alias to the type that
/// the `arg` method of the (template parameter) `t_FORMAT_CONTEXT` returns.
template <class t_FORMAT_CONTEXT>
struct BufferFormatContext_ArgType {

    // TYPES
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    typedef decltype(BloombergLP::bslmf::Util::declval<
                          const t_FORMAT_CONTEXT&>().arg(0))            type;
#else
    typedef typename t_FORMAT_CONTEXT::Arg                              type;
#endif
};

#ifndef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
/// [**PRIVATE**] Specialization for the format context of this package, which
/// keeps the name of its argument type private.
template <class t_OUT, class t_CHAR>
struct BufferFormatContext_ArgType<basic_format_context<t_OUT, t_CHAR> > {

    // TYPES
    typedef basic_format_arg<basic_format_context<t_OUT, t_CHAR> >      type;
};
#endif

                         // =========================
                         // class BufferFormatContext
                         // =========================

/// This mechanism class template provides a format context that appends what
/// is written to it to a string, and that obtains its formatting arguments
/// from another format context, of the (template parameter)
/// `t_FORMAT_CONTEXT` type.  Neither the string nor the other format context
/// is owned, and both must outlive this object.
template <class t_CHAR, class t_FORMAT_CONTEXT>
class BufferFormatContext {
  private:
    // DATA
    bsl::basic_string<t_CHAR> *d_buffer_p;   // output buffer (held, not owned)
    const t_FORMAT_CONTEXT    *d_context_p;  // source of the formatting
                                             // arguments (held, not owned)

  public:
    // TYPES

    /// Alias for the character type written to this context.
    typedef t_CHAR char_type;

    /// Alias for the output iterator type of this context.
    typedef bsl::back_insert_iterator<bsl::basic_string<t_CHAR> > iterator;

    /// Alias for the type of a formatting argument of this context.
    typedef typename BufferFormatContext_ArgType<t_FORMAT_CONTEXT>::type Arg;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    /// Alias for the formatter type used to format a value of the (template
    /// parameter) `t_TYPE` type in this context.
    template <class t_TYPE>
    using formatter_type = bsl::formatter<t_TYPE, t_CHAR>;
#endif

    // CREATORS

    /// Create an object that appends to the specified `buffer` and obtains its
    /// formatting arguments from the specified `context`.  The behavior is
    /// undefined unless both pointers refer to valid objects and they remain
    /// valid for the lifetime of this object.
    BufferFormatContext(bsl::basic_string<t_CHAR> *buffer,
                        const t_FORMAT_CONTEXT    *context);

    // MANIPULATORS

    /// Ignore the specified `it` and do nothing.
    void advance_to(iterator it);

    /// Return an iterator that appends to the buffer supplied at
    /// construction.
    iterator out();

    // ACCESSORS

    /// Return the argument having the specified `id` held by the context
    /// supplied at construction.
    Arg arg(std::size_t id) const;
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                         // -------------------------
                         // class BufferFormatContext
                         // -------------------------

// CREATORS
template <class t_CHAR, class t_FORMAT_CONTEXT>
inline
BufferFormatContext<t_CHAR, t_FORMAT_CONTEXT>::BufferFormatContext(
                                      bsl::basic_string<t_CHAR> *buffer,
                                      const t_FORMAT_CONTEXT    *context)
: d_buffer_p(buffer)
, d_context_p(context)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(context);
}

// MANIPULATORS
template <class t_CHAR, class t_FORMAT_CONTEXT>
inline
void BufferFormatContext<t_CHAR, t_FORMAT_CONTEXT>::advance_to(iterator)
{
    // Every iterator returned by `out` appends to the end of the buffer, so
    // there is no position to update.
}

template <class t_CHAR, class t_FORMAT_CONTEXT>
inline
typename BufferFormatContext<t_CHAR, t_FORMAT_CONTEXT>::iterator
BufferFormatContext<t_CHAR, t_FORMAT_CONTEXT>::out()
{
    return bsl::back_inserter(*d_buffer_p);
}

// ACCESSORS
template <class t_CHAR, class t_FORMAT_CONTEXT>
inline
typename BufferFormatContext<t_CHAR, t_FORMAT_CONTEXT>::Arg
BufferFormatContext<t_CHAR, t_FORMAT_CONTEXT>::arg(std::size_t id) const
{
    return d_context_p->arg(id);
}

}  // close package namespace
}  // close enterprise namespace

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

#endif

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
