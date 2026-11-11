// bslfmt_debugformatutil.h                                           -*-C++-*-

#ifndef INCLUDED_BSLFMT_DEBUGFORMATUTIL
#define INCLUDED_BSLFMT_DEBUGFORMATUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a way to request debug formatting where it is supported.
//
//@CLASSES:
//  bslfmt::SupportsDebugFormat: trait for formatters having debug formatting
//  bslfmt::DebugFormatUtil: switch a formatter into debug mode, if it can
//
//@SEE_ALSO: bslmf_nestedtraitdeclaration
//
//@DESCRIPTION: This component provides a trait, `SupportsDebugFormat`, that
// identifies formatters having a debug presentation, and a utility `struct`,
// `DebugFormatUtil`, whose `setDebugFormat` function selects that presentation
// on a formatter that supports it and does nothing otherwise.
//
// A formatter type supports debug formatting if it is associated with the
// `SupportsDebugFormat` trait using the `BSLMF_NESTED_TRAIT_DECLARATION` macro
// or, on platforms that support `decltype`, if it has a callable
// `set_debug_format()` member.
//
// This component is private to this package and should not be used by clients.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Requesting Debug Formatting Of An Element
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are writing a formatter for a container and wish its elements to
// be rendered in the debug presentation, as the standard requires.
//
// First, we have two element formatters: one that has a debug presentation and
// advertises it and one that has none.
// ```
//  struct StringLikeFormatter {
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(StringLikeFormatter,
//                                     bslfmt::SupportsDebugFormat);
//
//      // PUBLIC DATA
//      bool d_debug;
//
//      // CREATORS
//      StringLikeFormatter() : d_debug(false) {}
//
//      // MANIPULATORS
//      void set_debug_format() { d_debug = true; }
//  };
//
//  struct IntLikeFormatter {
//      // PUBLIC DATA
//      bool d_debug;
//
//      // CREATORS
//      IntLikeFormatter() : d_debug(false) {}
//  };
// ```
// Now, we ask each of them to switch to debug formatting:
// ```
//  StringLikeFormatter stringFormatter;
//  IntLikeFormatter    intFormatter;
//
//  bslfmt::DebugFormatUtil::setDebugFormat(&stringFormatter);
//  bslfmt::DebugFormatUtil::setDebugFormat(&intFormatter);
// ```
// Finally, we observe that the string-like formatter has switched and the
// integer-like formatter has been left alone:
// ```
//  assert(true  == stringFormatter.d_debug);
//  assert(false == intFormatter.d_debug);
// ```

#include <bslscm_version.h>

#include <bslmf_detectnestedtrait.h>
#include <bslmf_integralconstant.h>
#include <bslmf_util.h>
#include <bslmf_voidtype.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

namespace BloombergLP {
namespace bslfmt {

// FORWARD DECLARATION
template <class t_FORMATTER>
struct SupportsDebugFormat_Imp;

                         // ==========================
                         // struct SupportsDebugFormat
                         // ==========================

/// This metafunction is derived from `bsl::true_type` if the (template
/// parameter) `t_FORMATTER` has a debug presentation that is selected by
/// calling its `set_debug_format()` member, and from `bsl::false_type`
/// otherwise.
template <class t_FORMATTER>
struct SupportsDebugFormat
: bsl::integral_constant<bool, SupportsDebugFormat_Imp<t_FORMATTER>::value> {
};

                          // ======================
                          // struct DebugFormatUtil
                          // ======================

/// This struct provides a namespace for a utility function that requests debug
/// formatting of a formatter that supports it.
struct DebugFormatUtil {

  private:
    // PRIVATE CLASS METHODS

    /// Call `set_debug_format()` on the specified `formatter`.
    template <class t_FORMATTER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20
    void setDebugFormatImp(t_FORMATTER *formatter, bsl::true_type);

    /// Do nothing.
    template <class t_FORMATTER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20
    void setDebugFormatImp(t_FORMATTER *, bsl::false_type);

  public:
    // CLASS METHODS

    /// Call `set_debug_format()` on the specified `formatter` if
    /// `SupportsDebugFormat<t_FORMATTER>` is `true`, and do nothing otherwise.
    template <class t_FORMATTER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20
    void setDebugFormat(t_FORMATTER *formatter);
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                     // ====================================
                     // struct SupportsDebugFormat_HasMember
                     // ====================================

/// [**PRIVATE**] This component-private metafunction is derived from
/// `bsl::true_type` if the (template parameter) `t_FORMATTER` has a callable
/// `set_debug_format()` member and this can be detected on the current
/// platform, and from `bsl::false_type` otherwise.
template <class t_FORMATTER, class = void>
struct SupportsDebugFormat_HasMember : bsl::false_type {
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
/// [**PRIVATE**] Partial specialization for a type having a callable
/// `set_debug_format()` member.
template <class t_FORMATTER>
struct SupportsDebugFormat_HasMember<
             t_FORMATTER,
             BSLMF_VOIDTYPE(decltype(
                 bslmf::Util::declval<t_FORMATTER&>().set_debug_format()))>
: bsl::true_type {
};
#endif

                        // ==============================
                        // struct SupportsDebugFormat_Imp
                        // ==============================

/// [**PRIVATE**] This component-private metafunction computes the value of
/// `SupportsDebugFormat` for the (template parameter) `t_FORMATTER`.
template <class t_FORMATTER>
struct SupportsDebugFormat_Imp {
    // PUBLIC CLASS DATA
    enum {
        value = bslmf::DetectNestedTrait<t_FORMATTER,
                                         SupportsDebugFormat>::value
             || SupportsDebugFormat_HasMember<t_FORMATTER>::value
    };
};

                          // ----------------------
                          // struct DebugFormatUtil
                          // ----------------------

// PRIVATE CLASS METHODS
template <class t_FORMATTER>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
void DebugFormatUtil::setDebugFormatImp(t_FORMATTER *formatter, bsl::true_type)
{
    formatter->set_debug_format();
}

template <class t_FORMATTER>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
void DebugFormatUtil::setDebugFormatImp(t_FORMATTER *, bsl::false_type)
{
}

// CLASS METHODS
template <class t_FORMATTER>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
void DebugFormatUtil::setDebugFormat(t_FORMATTER *formatter)
{
    setDebugFormatImp(formatter, SupportsDebugFormat<t_FORMATTER>());
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BSLFMT_DEBUGFORMATUTIL

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
