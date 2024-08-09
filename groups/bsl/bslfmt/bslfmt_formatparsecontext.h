// bslfmt_formatparsecontext.h                                         -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATPARSECONTEXT
#define INCLUDED_BSLFMT_FORMATPARSECONTEXT

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


namespace BloombergLP {
namespace bslfmt {

// FORWARD DECLARATIONS

template <class t_CHAR>
class basic_format_parse_context;

class Format_ParseContext_Test_Querier;

// TYPEDEFS

typedef basic_format_parse_context<char> format_parse_context;

typedef basic_format_parse_context<wchar_t> wformat_parse_context;

                 // ----------------------------------------
                 // class basic_format_parse_context<t_CHAR>
                 // ----------------------------------------

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
                     bsl::basic_string_view<t_CHAR> fmt) BSLS_KEYWORD_NOEXCEPT;

    BSLS_KEYWORD_CONSTEXPR_CPP20 explicit basic_format_parse_context(
                 bsl::basic_string_view<t_CHAR> fmt,
                 size_t                         numArgs) BSLS_KEYWORD_NOEXCEPT;

    // MANIPULATORS
    BSLS_KEYWORD_CONSTEXPR_CPP20 void advance_to(const_iterator it);

    BSLS_KEYWORD_CONSTEXPR_CPP20 size_t next_arg_id();

    BSLS_KEYWORD_CONSTEXPR_CPP20 void check_arg_id(size_t id);

    // ACCESSORS
    BSLS_KEYWORD_CONSTEXPR_CPP20 const_iterator
    begin() const BSLS_KEYWORD_NOEXCEPT;

    BSLS_KEYWORD_CONSTEXPR_CPP20 const_iterator
    end() const BSLS_KEYWORD_NOEXCEPT;

  private:
    // NOT IMPLEMENTED
    basic_format_parse_context(
                       const basic_format_parse_context&) BSLS_KEYWORD_DELETED;
    basic_format_parse_context& operator=(
                       const basic_format_parse_context&) BSLS_KEYWORD_DELETED;

    // FRIENDS
    friend class Format_ParseContext_Test_Querier;
};

               // --------------------------------------
               // class Format_ParseContext_Test_Querier
               // --------------------------------------

struct Format_ParseContext_Test_Querier {
    // TYPES
    enum Indexing { e_UNKNOWN, e_MANUAL, e_AUTOMATIC };

    // CLASS METHODS
    template <class t_CHAR>
    static Indexing queryIndexing(
                       const basic_format_parse_context<t_CHAR>& parseContext);
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                 // ----------------------------------------
                  // class basic_format_parse_context<t_CHAR>
                  // ----------------------------------------


// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
basic_format_parse_context<t_CHAR>::basic_format_parse_context(
                      bsl::basic_string_view<t_CHAR> fmt) BSLS_KEYWORD_NOEXCEPT
: d_begin(fmt.begin())
, d_end(fmt.end())
, d_indexing(e_UNKNOWN)
, d_next_arg_id(0)
, d_num_args(0)
{
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
basic_format_parse_context<t_CHAR>::basic_format_parse_context(
                  bsl::basic_string_view<t_CHAR> fmt,
                  size_t                         numArgs) BSLS_KEYWORD_NOEXCEPT
: d_begin(fmt.begin())
, d_end(fmt.end())
, d_indexing(e_UNKNOWN)
, d_next_arg_id(0)
, d_num_args(numArgs)
{
}

// MANIPULATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void basic_format_parse_context<t_CHAR>::advance_to(const_iterator it)
{
    d_begin = it;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
size_t basic_format_parse_context<t_CHAR>::next_arg_id()
{
    if (d_indexing == e_MANUAL) {
        BSLS_THROW(format_error("mixing of automatic and manual indexing"));
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

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void basic_format_parse_context<t_CHAR>::check_arg_id(size_t id)
{
    if (d_indexing == e_AUTOMATIC) {
        BSLS_THROW(format_error("mixing of automatic and manual indexing"));
    }
    if (id >= d_num_args) {
        BSLS_THROW(format_error("invalid argument index"));
    }
    if (d_indexing == e_UNKNOWN) {
        d_indexing = e_MANUAL;
    }
}

// ACCESSORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename basic_format_parse_context<t_CHAR>::const_iterator
basic_format_parse_context<t_CHAR>::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_begin;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename basic_format_parse_context<t_CHAR>::const_iterator
basic_format_parse_context<t_CHAR>::end() const BSLS_KEYWORD_NOEXCEPT
{
    return d_end;
}

                   // --------------------------------------
                   // class Format_ParseContext_Test_Querier
                   // --------------------------------------

template <class t_CHAR>
Format_ParseContext_Test_Querier::Indexing
Format_ParseContext_Test_Querier::queryIndexing(
                        const basic_format_parse_context<t_CHAR>& parseContext)
{
    switch (parseContext.d_indexing) {
      case basic_format_parse_context<t_CHAR>::e_MANUAL: {
        return e_MANUAL;                                              // RETURN
      } break;
      case basic_format_parse_context<t_CHAR>::e_AUTOMATIC: {
        return e_AUTOMATIC;                                           // RETURN
      } break;
      default: {
        return e_UNKNOWN;                                             // RETURN
      } break;
    }
};


}  // close namespace bslfmt
} // close enterprise namespace


#endif  // INCLUDED_BSLFMT_FORMATPARSECONTEXT

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
