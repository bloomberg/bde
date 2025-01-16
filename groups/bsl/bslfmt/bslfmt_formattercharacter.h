// bslfmt_formattercharacter.h                                        -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERCHARACTER
#define INCLUDED_BSLFMT_FORMATTERCHARACTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a formatter customization for character types
//
//@CLASSES:
//  formatter<char, wchar_t>    : specialization for `char` and `wchar_t`
//  formatter<char, char>       : specialization for `char`s
//  formatter<wchar_t, wchar_t> : specialization for `wchar_t`s
//
//@DESCRIPTION: This component provides partial specializations of
// `bsl::formatter` catering for character types.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example: Formatting a character
///- - - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`, so this example is
// necessarily unrealistic.
//
// Suppose we want to test this formatter's ability to present a character with
// defined alignment and padding.
// ```
//  bslfmt::MockParseContext<char> mpc("*<6c", 1);
//
//  bsl::formatter<char, char> formatter;
//  mpc.advance_to(formatter.parse(mpc));
//
//  char value = 'a';
//
//  bslfmt::MockFormatContext<char> mfc(value, 0, 0);
//
//  mfc.advance_to(bsl::as_const(formatter).format(value, mfc));
//
//  assert("a*****" == mfc.finalString());
// ```

#include <bslscm_version.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>
#include <bslfmt_formatterintegral.h>

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

#include <locale>     // for `std::ctype`, `locale`
#include <string>     // for `std::char_traits`

#include <stdio.h>    // for `snprintf`

namespace BloombergLP {
namespace bslfmt {

                         // ======================================
                         // struct FormatterCharacter_MakeUnsigned
                         // ======================================

/// This `struct` template implements the `make_unsigned` meta-function defined
/// in the C++11 standard [meta.trans.sign], providing an alias, `type`, that
/// returns the result.  The `type` is the unsigned analogue of the (template
/// parameter) `t_TYPE`.
template <class t_TYPE, int t_SIZE>
struct FormatterCharacter_MakeUnsigned {
};

/// Partial specialization of the `FormatterCharacter_MakeUnsigned` template
/// for the type `char`.
template <int t_SIZE>
struct FormatterCharacter_MakeUnsigned<char, t_SIZE> {
    // TYPES
    typedef unsigned char type;  // unsigned analogue for `char`
};

/// This is a specialization of the `FormatterCharacter_MakeUnsigned` template
/// for the type `wchar_t` having 1-byte length.
template <>
struct FormatterCharacter_MakeUnsigned<wchar_t, 1> {
    // TYPES
    typedef unsigned char type;  // unsigned analogue for 1-byte `wchar_t`
};

/// This is a specialization of the `FormatterCharacter_MakeUnsigned` template
/// for the type `wchar_t` having 2-byte length.
template <>
struct FormatterCharacter_MakeUnsigned<wchar_t, 2> {
    // TYPES
    typedef unsigned short int type;  // unsigned analogue for 2-byte `wchar_t`
};

/// This is a specialization of the `FormatterCharacter_MakeUnsigned` template
/// for the type `wchar_t` having 4-byte length.
template <>
struct FormatterCharacter_MakeUnsigned<wchar_t, 4> {
    // TYPES
    typedef unsigned long int type;  // unsigned analogue for 4-byte `wchar_t`
};

/// This is a specialization of the `FormatterCharacter_MakeUnsigned` template
/// for the type `wchar_t` having 8-byte length.
template <>
struct FormatterCharacter_MakeUnsigned<wchar_t, 8> {
    // TYPES
    typedef unsigned long long int type;  // unsigned analogue for 8-byte
                                          // `wchar_t`
};

                // =========================================
                // struct FormatterCharacter_CharacterOutput
                // =========================================

/// This struct provides a function that outputs the specified `value` to the
/// specified `out`.
template <class t_VALUE_TYPE, class t_CHAR>
struct FormatterCharacter_CharacterOutput {
    // CLASS METHODS

    /// Output the specified `value` to the specified `out` and return `out`.
    /// incremented by the number of characters written.
    template <class t_ITERATOR>
    static t_ITERATOR output(const t_VALUE_TYPE value, t_ITERATOR out)
    {
        typedef
            typename bsl::iterator_traits<t_ITERATOR>::value_type OutputType;
        BSLMF_ASSERT((bsl::is_same<OutputType, t_VALUE_TYPE>::value) &&
                     (bsl::is_same<t_CHAR, t_VALUE_TYPE>::value));

        *out = value;
        ++out;
        return out;
    }
};

/// This is a specialization of `FormatterCharacter_CharacterOutput` template
/// for `char` and `wchar_t`.
template <>
struct FormatterCharacter_CharacterOutput<char, wchar_t> {
    // CLASS METHODS

    /// Output the specified `value` to the specified `out` and return `out`.
    /// incremented by the number of characters written.
    template <class t_ITERATOR>
    static t_ITERATOR output(const char value, t_ITERATOR out)
    {
        typedef
            typename bsl::iterator_traits<t_ITERATOR>::value_type OutputType;
        BSLMF_ASSERT((bsl::is_same<OutputType, wchar_t>::value));

        // Unfortunately, it is not clearly defined in the current version of
        // the standard (Working draft #4993 from 10/16/2024) what should
        // happen with non-printable character values (i.e. the negative ones)
        // during their output to the `wchar_t` sink:
        //
        // 28.5.2.2 Standard format specifiers
        // Table 103 - Meaning of type options for charT [tab:format.type.char]
        // +---------+-------------------------------------+
        // | Type    | Meaning                             |
        // +---------+-------------------------------------+
        // | none, c | Copies the character to the output. |
        // +---------+-------------------------------------+
        //
        // This implementation adopts behavior consistent with the current
        // standard library implementations (gcc (14.2), clang (19.1) and msvc
        // (19.40)).

        *out = static_cast<wchar_t>(static_cast<unsigned char>(value));
        ++out;
        return out;
    }
};

                         // =============================
                         // struct FormatterCharacter_Imp
                         // =============================

/// This type implements the formatter logic specific to the character types
/// (`char` and `wchar_t`).
template <class t_VALUE, class t_CHAR>
struct FormatterCharacter_Imp : public FormatterIntegralBase<t_VALUE, t_CHAR> {
  public:
    // TRAITS
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // ACCESSORS

    /// Create string representation of the specified `value`, customized in
    /// accordance with the requested format and the specified `formatContext`,
    /// and copy it to the output that the output iterator of the
    /// `formatContext` points to.  Return the output iterator incremented by
    /// the number of characters written.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                        t_VALUE           value,
                                        t_FORMAT_CONTEXT& formatContext) const;
};
}  // close package namespace
}  // close enterprise namespace

namespace bsl {

/// This is a specialization of the `bsl::formatter` template for the types
/// `char` and `wchar_t`.
template <>
struct formatter<char, wchar_t>
: BloombergLP::bslfmt::FormatterCharacter_Imp<char, wchar_t> {
};

/// This is a specialization of the `bsl::formatter` template for the type
/// `char`.
template <>
struct formatter<char, char>
: BloombergLP::bslfmt::FormatterCharacter_Imp<char, char> {
};

/// This is a specialization of the `bsl::formatter` template for the type
/// `wchar_t`.
template <>
struct formatter<wchar_t, wchar_t>
: BloombergLP::bslfmt::FormatterCharacter_Imp<wchar_t, wchar_t> {
};

}  // close namespace bsl

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bslfmt {

                         // -----------------------------
                         // struct FormatterCharacter_Imp
                         // -----------------------------

template <class t_VALUE, class t_CHAR>
template <class t_FORMAT_CONTEXT>
inline
typename t_FORMAT_CONTEXT::iterator
FormatterCharacter_Imp<t_VALUE, t_CHAR>::format(
                                         t_VALUE           value,
                                         t_FORMAT_CONTEXT& formatContext) const
{
    typedef FormatterSpecificationStandard<t_CHAR> FSS;
    typedef bslalg::NumericFormatterUtil           NFUtil;

    const FSS& parsedSpec = this->specification();

    if (FSS::e_CHARACTER_CHARACTER == parsedSpec.formatType()) {
        char    dummyPrefix = 0;
        t_CHAR  valueBuf;
        t_CHAR *valueBegin = &valueBuf;
        t_CHAR *valueEnd =
                   FormatterCharacter_CharacterOutput<t_VALUE, t_CHAR>::output(
                       value,
                       valueBegin);

        return this->outputValue(&dummyPrefix,
                                 &dummyPrefix,
                                 valueBegin,
                                 valueEnd,
                                 formatContext);                      // RETURN
    }
    else {
        typedef typename FormatterCharacter_MakeUnsigned<t_VALUE,
                                                         sizeof(wchar_t)>::type
            UintType;

        const int  maxPrefixSize = 4;
        char       prefixBuf[maxPrefixSize];
        char      *prefixBegin = prefixBuf;
        char      *prefixEnd   = prefixBuf;

        UintType   intValue = static_cast<UintType>(value);

        // We want to make sure that we have enough space to accommodate any
        // representation of the `value`.  Binary representation takes up the
        // most space.

        const int  maxValueSize =
                                NFUtil::ToCharsMaxLength<UintType, 2>::k_VALUE;
        t_CHAR     valueBuf[maxValueSize];
        t_CHAR    *valueBegin = valueBuf;
        t_CHAR    *valueEnd   = valueBuf;

        prefixEnd = this->formatPrefix(prefixBuf, maxPrefixSize, intValue);
        valueEnd  = this->formatValue(valueBuf, maxValueSize, intValue);
        return this->outputValue(prefixBegin,
                                 prefixEnd,
                                 valueBegin,
                                 valueEnd,
                                 formatContext);
    }
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BSLFMT_FORMATTERCHARACTER

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
