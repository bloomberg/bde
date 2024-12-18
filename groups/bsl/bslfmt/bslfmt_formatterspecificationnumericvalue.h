// bslfmt_formatterspecificationnumericvalue.h                        -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERSPECIFICATIONNUMERICVALUE
#define INCLUDED_BSLFMT_FORMATTERSPECIFICATIONNUMERICVALUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Integer value for use within `bsl::format` spec parsers
//
//@CLASSES:
//  FormatterSpecificationNumericValue: Category and its integer value
//
//@SEE_ALSO: bslfmt_format.h
//
//@DESCRIPTION: This component provides a a value semantic type to enable
// `bslfmt` formatters to return an integer, typically representing a width or
// a precision,  and a category that determines the meaning of the integer
// value (direct value or argument ID), or represents a category without an
// integer value (default value or next argument).
//
// This component is for use by formatters in BDE only (primarily in `bslfmt`)

#include <bslscm_version.h>

#include <bslfmt_formaterror.h>

#include <bslalg_numericformatterutil.h>

#include <bslmf_assert.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isarithmetic.h>
#include <bslmf_issame.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_keyword.h>

#include <bslstl_iterator.h>
#include <bslstl_monostate.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'

#include <stdio.h>    // for 'snprintf'

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <format>     // for 'std::visit_format_arg'
#endif

namespace BloombergLP {
namespace bslfmt {

                   // -----------------------------------------
                   // struct FormatterSpecificationNumericValue
                   // -----------------------------------------

/// Type holding an category plus an integral value.  This is a value semantic
/// type primarily used to hold width and precision entries in a format
/// specification.
struct FormatterSpecificationNumericValue {
  public:
    // TYPES
    enum ValueType {
        e_DEFAULT,   // Unspecified value and category
        e_VALUE,     // Contained integer is the value to use
        e_NEXT_ARG,  // Dynamic nested argument
        e_ARG_ID     // Nested argument whose id is the contained integer
    };

  private:
    // DATA
    ValueType d_category;  // category
    int       d_value;     // integral value

    // FRIENDS

    friend struct FormatterSpecificationNumericValue_ArgVisitor;

  public:
    // CREATORS

    /// Create a `FormatterSpecificationNumericValue` object holding the
    /// specified `value` and `type`.
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationNumericValue(
                                                               int       value,
                                                               ValueType type);

    // ACCESSORS

    /// Return true if the value and type of this object match those of the
    /// specified `object`, otherwise return false.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    bool operator==(const FormatterSpecificationNumericValue& other) const;

    /// Return the held value. Behavior is undefined if the held category is
    /// either `e_DEFAULT` or `e_NEXT_ARG_ID`.
    BSLS_KEYWORD_CONSTEXPR_CPP20 int       value() const;

    /// Return the held category.
    BSLS_KEYWORD_CONSTEXPR_CPP20 ValueType category() const;

    // CLASS METHODS

    /// Parse the string in the random-access-iterator range specified by
    /// `start` and `end` to extract either a hard-coded integer or a nested
    /// argument specification.  If the specified `needInitialDot` is true the
    /// string is only parsed if `*begin == '.'` and a `format_error` exception
    /// is thrown if the string following the initial dot is empty. If an error
    /// occurs throw an exception of type `format_error`, otherwise pdate the
    /// output `start` iterator to point to the first unparsed character of the
    /// string and store the result into the specified `outValue`.
    template <class t_ITER>
    static void BSLS_KEYWORD_CONSTEXPR_CPP20 parse(
                           FormatterSpecificationNumericValue *outValue,
                           t_ITER                             *start,
                           t_ITER                              end,
                           bool                                needInitialDot);

    /// If the specified output `out` holds a non-dynamic nested value (ie
    /// `d_type` is `e_ARG_ID`) update its value using the arguments stored in
    /// the specified `context` and update the type to `e_VALUE`.  If `out`
    /// holds a dynamic nested value (ie `d_type` is `e_NEXT_ARG`) throw an
    /// exception of type `format_error`.  Otherwise do nothing.
    template <typename t_FORMAT_CONTEXT>
    static void postprocess(FormatterSpecificationNumericValue *out,
                            const t_FORMAT_CONTEXT&             context);
};

                 // -------------------------------------------------
                 // struct FormatterSpecificationNumericValue_Visitor
                 // -------------------------------------------------

/// Component-private type to enable extraction of values held by format
/// contexts during the postprocessing stage of determining the format
/// specification (typically performed by the FormatSpecificationSplitter).
/// This type exists so that FormatterSpecificationSplitter can update
/// FormatterSpecificationNumericValue to the value contained by a Standard
/// `basic_format_arg` using the Standard `visit_format_arg` function.
struct FormatterSpecificationNumericValue_ArgVisitor {
  private:
    // DATA
    FormatterSpecificationNumericValue *d_value_p;
        // referenced FormatterSpecificationNumericValue

  public:
    // CREATORS

    /// Create an instance of `FormatterSpecificationNumericValue_Visitor` that
    /// refers to the object to which the specified `valuePtr` points.
    FormatterSpecificationNumericValue_ArgVisitor(
                                 FormatterSpecificationNumericValue *valuePtr);

    // MANIPULATORS

    /// Throw an exception of type `format_error`.  This indicates visitation
    /// of an argument with no value, which would indicate a logic error. Note
    /// this would only be called when extracting a value from a
    /// `basic_format_arg` that holds no value, which would normally indicate
    /// an incorrect argument type.
    void operator()(bsl::monostate) const;

    /// Throw an exception of type `format_error`.  This indicates visitation
    /// of an `bsl::format` argument of boolean type, usually resulting from a
    /// user error such as calling `bsl::format("{:{}}",value,b)` where `b` is
    /// bool.
    void operator()(bool) const;

    /// Update the value of the `FormatterSpecificationNumericValue` referenced
    /// by this instance to the specified `x` and update its type to `e_VALUE`.
    template <class t_TYPE>
    typename bsl::enable_if<bsl::is_integral<t_TYPE>::value>::type operator()(
                                                               t_TYPE x) const;

    /// Throw an exception of type `format_error`.  This indicates visitation
    /// of an `bsl::format` argument of non-integral type, usually resulting
    /// from a user error such as calling `bsl::format("{:{}}",value,f)` where
    /// `f` is floating point.
    template <class t_TYPE>
    typename bsl::enable_if<!bsl::is_integral<t_TYPE>::value>::type operator()(
                                                               t_TYPE x) const;
};


// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                 // -----------------------------------------
                 // struct FormatterSpecificationNumericValue
                 // -----------------------------------------

// CREATORS

inline
BSLS_KEYWORD_CONSTEXPR_CPP20
FormatterSpecificationNumericValue::FormatterSpecificationNumericValue(
                                                               int       value,
                                                               ValueType type)
: d_category(type)
, d_value(value)
{
}

// ACCESSORS

inline
BSLS_KEYWORD_CONSTEXPR_CPP20
bool FormatterSpecificationNumericValue::operator==(
                        const FormatterSpecificationNumericValue& other) const
{
    return d_value == other.d_value && d_category == other.d_category;
}

inline
BSLS_KEYWORD_CONSTEXPR_CPP20 int
FormatterSpecificationNumericValue::value() const
{
    if (d_category == e_DEFAULT) {
        BSLS_THROW(
              bsl::format_error("INTERNAL ERROR: Access to unspecified nested "
                                "value in format spec."));            // RETURN
    }
    if (d_category == e_NEXT_ARG) {
        BSLS_THROW(
             bsl::format_error("INTERNAL ERROR: Failed to identify arg-id for "
                               "nested value in format spec."));      // RETURN
    }
    return d_value;
}

inline
BSLS_KEYWORD_CONSTEXPR_CPP20
FormatterSpecificationNumericValue::ValueType
FormatterSpecificationNumericValue::category() const
{
    return d_category;
}

// CLASS METHODS

template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20
void FormatterSpecificationNumericValue::parse(
                           FormatterSpecificationNumericValue *outValue,
                           t_ITER                             *start,
                           t_ITER                              end,
                           bool                                needInitialDot)
{
    // Handle empty string or empty specification.
    if (*start == end || **start == '}') {
        return;                                                       // RETURN
    }

    t_ITER current = *start;

    if (needInitialDot) {
        // No dot therefore no precision - early successful exit.
        if (*current != '.')
            return;                                                   // RETURN
        ++current;
        // A dot but nothing afterwards is an invalid precision string.
        if (current == end)
            BSLS_THROW(bsl::format_error(
                          "Invalid Precision (nothing after '.')"));  // RETURN
    }

    bool isArgId = false;

    if (*current == '{') {
        current++;
            // Missing matching closing brace.
        if (current == end)
            BSLS_THROW(
                    bsl::format_error("Nested arg id missing '}'"));  // RETURN
        // Early exit for a non-numbered replacement field.
        if (*current == '}') {
            outValue->d_category = e_NEXT_ARG;
            outValue->d_value    = 0;
            *start               = current + 1;
            return;                                                   // RETURN
        }
        isArgId = true;
    }

    int digitCount = 0;
    int value      = 0;

    long long accumulator = 0;
    while (*current >= '0' && *current <= '9') {
        accumulator = (accumulator * 10) + static_cast<int>(*current - '0');
        if (accumulator > INT_MAX) {
            BSLS_THROW(
                 bsl::format_error("Parsed FormatterSpecificationNumericValue "
                                   "overflow"));                      // RETURN
        }
        ++digitCount;
        ++current;
    }
    value = static_cast<int>(accumulator);

    // No digits
    if (digitCount == 0) {
        // If we have either specified the "precision dot" or if we know we
        // have a numbered replacement field then digits are non-optional.
        if (needInitialDot) {
            BSLS_THROW(bsl::format_error(
                    "Invalid Precision (no digits following '.')"));  // RETURN
        }
        if (isArgId) {
            BSLS_THROW(
                    bsl::format_error("Nested arg id non-numeric"));  // RETURN
        }
        outValue->d_value    = 0;
        outValue->d_category = e_DEFAULT;
    }
    // At least one digit
    else {
        // As we do not allow + or - the value must be non-negative.
        outValue->d_value = value;

        if (isArgId) {
            // Relative argument references must have a closing brace.
            if (current == end || *current != '}') {
                BSLS_THROW(
                    bsl::format_error("Nested arg id missing '}'"));  // RETURN
            }
            ++current;
        }

        outValue->d_category = isArgId ? e_ARG_ID : e_VALUE;
    }

    *start = current;

    return;
}

template <typename t_FORMAT_CONTEXT>
inline
void FormatterSpecificationNumericValue::postprocess(
                                 FormatterSpecificationNumericValue *out,
                                 const t_FORMAT_CONTEXT&             context)
{
    // Non-nested argument id - value does not change.
    if (out->d_category == FormatterSpecificationNumericValue::e_DEFAULT ||
        out->d_category == FormatterSpecificationNumericValue::e_VALUE) {
        return;                                                       // RETURN
    }

    // Splitter parsing converts dynamic nested widths and precisions
    // (`e_NEXT_ARG`) into non-dynamic ones (`e_ARG_ID`).  As a result, if we
    // encounder a dynamic nested width at this stage it indicates a logic
    // error.
    if (out->d_category == FormatterSpecificationNumericValue::e_NEXT_ARG) {
        BSLS_THROW(bsl::format_error(
            "INTERNAL ERROR: Unconverted dynamic nested argument"));  // RETURN
    }

    FormatterSpecificationNumericValue_ArgVisitor visitor(out);
    {
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        // Depending on the type of t_FORMAT_CONTEXT we may need to use
        // `visit_format_arg` from `bslfmt` or from `std`.
        using namespace std;
#endif
        visit_format_arg(visitor, context.arg(out->d_value));
    }
}

            // ----------------------------------------------------
            // struct FormatterSpecificationNumericValue_ArgVisitor
            // ----------------------------------------------------

inline
FormatterSpecificationNumericValue_ArgVisitor::
    FormatterSpecificationNumericValue_ArgVisitor(
                                  FormatterSpecificationNumericValue *valuePtr)
: d_value_p(valuePtr)
{
}

inline
void FormatterSpecificationNumericValue_ArgVisitor::operator()(
                                                          bsl::monostate) const
{
    BSLS_THROW(bsl::format_error("Nested argument id out of range"));
}

inline
void FormatterSpecificationNumericValue_ArgVisitor::operator()(bool) const
{
    BSLS_THROW(bsl::format_error("Nested value argument must be integral"));
}

template <class t_TYPE>
typename bsl::enable_if<bsl::is_integral<t_TYPE>::value>::type
FormatterSpecificationNumericValue_ArgVisitor::operator()(t_TYPE x) const
{
    if (x < 0 || x > INT_MAX) {
        BSLS_THROW(bsl::format_error("Nested value argument out of range"));
    }
    d_value_p->d_value    = static_cast<int>(x);
    d_value_p->d_category = FormatterSpecificationNumericValue::e_VALUE;
}

template <class t_TYPE>
typename bsl::enable_if<!bsl::is_integral<t_TYPE>::value>::type
FormatterSpecificationNumericValue_ArgVisitor::operator()(t_TYPE) const
{
    BSLS_THROW(bsl::format_error("Nested value argument must be integral"));
}

}  // close namespace bslfmt
}  // close enterprise namespace

#endif  // INCLUDED_BSLFMT_FORMATTERSPECIFICATIONNUMERICVALUE

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
