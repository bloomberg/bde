// bslfmt_formatterspecificationnumericvalue.h                        -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERSPECIFICATIONNUMERICVALUE
#define INCLUDED_BSLFMT_FORMATTERSPECIFICATIONNUMERICVALUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Integer value for use within `bsl::format` specification parsers
//
//@CLASSES:
//  FormatterSpecificationNumericValue: Category and its optional integer value
//
//@SEE_ALSO: bslfmt_format.h
//
//@DESCRIPTION: This component provides a a value semantic type to enable
// `bslfmt` formatters to return an optional integer value, typically
// representing a width or a precision, and a category that determines the
// meaning of the integer value (direct value or argument ID), or represents a
// category without an integer value (default value or next argument).
//
// This component is for use within `bslfmt` only.

#include <bslscm_version.h>

#include <bslfmt_formaterror.h>

#include <bslalg_numericformatterutil.h>

#include <bslmf_assert.h>
#include <bslmf_isintegral.h>
#include <bslmf_enableif.h>

#include <bsls_compilerfeatures.h>
#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#include <bslstl_iterator.h>
#include <bslstl_monostate.h>

#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'

#include <limits.h>   // `INT_MAX`
#include <stdio.h>    // for 'snprintf'

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
# include <format>     // for 'std::visit_format_arg'
#endif

namespace BloombergLP {
namespace bslfmt {

                // =========================================
                // struct FormatterSpecificationNumericValue
                // =========================================

/// Type holding a category plus an optional integral value.  This is a value
/// semantic type primarily used to represent width and precision of a format
/// specification.
struct FormatterSpecificationNumericValue {
  public:
    // TYPES
    enum Category {
        e_DEFAULT,   // Not specified value and category (in the format)
        e_VALUE,     // Contained integer is the value to use
        e_NEXT_ARG,  // Dynamic nested argument
        e_ARG_ID     // Nested argument whose id is the contained integer
    };

  private:
    // DATA
    Category d_category;  // category
    int      d_value;     // optionally specified integral value

    // FRIENDS

    friend class FormatterSpecificationNumericValue_ArgVisitor;

  public:
    // CREATORS

    /// Create a `FormatterSpecificationNumericValue` object with `e_DEFAULT`
    /// `category` and 0 `value`.
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationNumericValue();

    /// Create a `FormatterSpecificationNumericValue` object with the
    /// specified `value` and `category` attributes.
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationNumericValue(
                                                             Category category,
                                                             int      value);

    // MANIPULATORS

    /// Parse the string in the random-access-iterator range specified by
    /// `start` and `end` to extract either a hard-coded integer or a nested
    /// argument specification.  If the specified `needInitialDot` is true the
    /// string is only parsed if `*start == '.'` and a `format_error` exception
    /// is thrown if the string following the initial dot is empty.  If an
    /// error occurs throw an exception of type `format_error`, otherwise
    /// update the output `start` iterator to point to the first unparsed
    /// character of the string and store the parsed result into this object.
    template <class t_ITER>
    void BSLS_KEYWORD_CONSTEXPR_CPP20 parse(t_ITER *start,
                                            t_ITER  end,
                                            bool    needInitialDot);

    /// If this object holds a non-dynamic nested value (i.e., `d_type` is
    /// `e_ARG_ID`) update its value using the arguments stored in the
    /// specified `context` and update the type to `e_VALUE`.  If this object
    /// holds a dynamic nested value (i.e., `d_type` is `e_NEXT_ARG`) throw an
    /// exception of type `format_error`.  Otherwise do nothing.
    template <typename t_FORMAT_CONTEXT>
    void postprocess(const t_FORMAT_CONTEXT& context);

    // ACCESSORS

    /// Return `true` if this object is equal to the specified `other` object,
    /// otherwise return `false`.  Two `FormatterSpecificationNumericValue`
    /// objects are considered equal if their `category` and `value` attributes
    /// are both equal.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    bool operator==(const FormatterSpecificationNumericValue& other) const;

    /// Return the `category` attribute of this object.
    BSLS_KEYWORD_CONSTEXPR_CPP20 Category category() const;

    /// Return the `value` attribute of this object.  The behavior is undefined
    /// if `category` is either `e_DEFAULT` or `e_NEXT_ARG_ID`.
    BSLS_KEYWORD_CONSTEXPR_CPP20 int value() const;
};

          // ===================================================
          // class FormatterSpecificationNumericValue_ArgVisitor
          // ===================================================

/// Component-private type to enable extraction of values held by format
/// contexts during the postprocessing stage of determining the format
/// specification (typically performed by the `FormatSpecificationParser`).
/// This type exists so that `FormatSpecificationParser` can update
/// FormatterSpecificationNumericValue to the value contained by a Standard
/// `basic_format_arg` using the Standard `visit_format_arg` function.
class FormatterSpecificationNumericValue_ArgVisitor {
  private:
    // DATA

    /// The referenced `FormatterSpecificationNumericValue` to be updated
    FormatterSpecificationNumericValue *d_value_p;

  public:
    // CREATORS

    /// Create an instance of `FormatterSpecificationNumericValue_ArgVisitor`
    /// that refers to the object to which the specified `valuePtr` points.
    FormatterSpecificationNumericValue_ArgVisitor(
                                 FormatterSpecificationNumericValue *valuePtr);

    // ACCESSORS

    /// Throw an exception of type `format_error`.  This indicates visitation
    /// of an argument with no value, which would indicate a logic error.  Note
    /// that this would only be called when extracting a value from a
    /// `basic_format_arg` that holds no value, which would normally indicate
    /// an incorrect argument type.
    void operator()(bsl::monostate) const;

    /// Throw an exception of type `format_error`.  This indicates visitation
    /// of a `bsl::format` argument of boolean type, usually resulting from a
    /// user error such as calling `bsl::format("{:{}}", value, b)` where `b`
    /// is `bool`.
    void operator()(bool) const;

    /// Update the `FormatterSpecificationNumericValue` referenced by this
    /// visitor by setting its `category` attribute to `e_VALUE` and its
    /// `value` to the specified `value`.
    template <class t_TYPE>
    typename bsl::enable_if<bsl::is_integral<t_TYPE>::value>::type
    operator()(t_TYPE value) const;

    /// Throw an exception of type `format_error`.  This indicates visitation
    /// of a `bsl::format` argument of non-integral type, usually resulting
    /// from a user error such as calling `bsl::format("{:{}}", value, f)`
    /// where `f` has a floating point type.
    template <class t_TYPE>
    typename bsl::enable_if<!bsl::is_integral<t_TYPE>::value>::type
    operator()(t_TYPE value) const;
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
FormatterSpecificationNumericValue::FormatterSpecificationNumericValue()
: d_category(e_DEFAULT)
, d_value(0)
{
}
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
FormatterSpecificationNumericValue::FormatterSpecificationNumericValue(
                                                             Category category,
                                                             int      value)
: d_category(category)
, d_value(value)
{
}

// MANIPULATORS
template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20 void FormatterSpecificationNumericValue::parse(
                                                        t_ITER *start,
                                                        t_ITER  end,
                                                        bool    needInitialDot)
{
    // Handle empty string or empty specification.
    if (*start == end || **start == '}') {
        d_category = e_DEFAULT;
        return;                                                       // RETURN
    }

    t_ITER current = *start;

    if (needInitialDot) {
        // No dot therefore no precision: early successful exit
        if (*current != '.') {
            return;                                                   // RETURN
        }
        ++current;

        // Found a dot but nothing afterwards: it is an invalid precision spec.
        if (current == end) {
            BSLS_THROW(bsl::format_error(
                          "Invalid Precision (nothing after '.')"));   // THROW
        }
    }

    bool isArgId = false;

    if (*current == '{') {
        current++;
        if (current == end) {
            BSLS_THROW(bsl::format_error("Nested arg id closing '}' "
                                         "is missing"));               // THROW
        }

        // Early exit for a non-numbered replacement field
        if (*current == '}') {
            d_category = e_NEXT_ARG;
            d_value    = 0;
            *start     = current + 1;
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
            BSLS_THROW(bsl::format_error("Too large integer value"));  // THROW
        }
        ++digitCount;
        ++current;
        if (current == end) {
            break;                                                     // BREAK
        }
    }
    value = static_cast<int>(accumulator);

    // No digits
    if (digitCount == 0) {
        // If we have either specified the "precision dot" or if we know we
        // have a numbered replacement field then digits are non-optional.
        if (isArgId) {
            BSLS_THROW(bsl::format_error(
                               "Nested argument id is not numeric"));  // THROW
        }
        if (needInitialDot) {
            BSLS_THROW(bsl::format_error(
                     "Invalid precision (no digits following '.')"));  // THROW
        }
        d_category = e_DEFAULT;
    }
    // At least one digit
    else {
        // As we do not allow + or - the value must be non-negative.
        d_value = value;

        if (isArgId) {
            // Relative argument references must have a closing brace.
            if (current == end || *current != '}') {
                BSLS_THROW(bsl::format_error(
                               "Nested argument id '}' is missing"));  // THROW
            }
            ++current;
        }

        d_category = isArgId ? e_ARG_ID : e_VALUE;
    }

    *start = current;
}

template <typename t_FORMAT_CONTEXT>
inline
void FormatterSpecificationNumericValue::postprocess(
                                               const t_FORMAT_CONTEXT& context)
{
    // Non-nested argument id: value does not change
    if (d_category == FormatterSpecificationNumericValue::e_DEFAULT ||
        d_category == FormatterSpecificationNumericValue::e_VALUE) {
        return;                                                       // RETURN
    }

    // Parser parsing converts dynamic nested widths and precisions
    // (`e_NEXT_ARG`) into non-dynamic ones (`e_ARG_ID`).  As a result, if we
    // encounter a dynamic nested width at this stage it indicates a logic
    // error.
    if (d_category == FormatterSpecificationNumericValue::e_NEXT_ARG) {
        BSLS_THROW(bsl::format_error(
             "INTERNAL ERROR: Unconverted dynamic nested argument"));  // THROW
    }

    FormatterSpecificationNumericValue_ArgVisitor visitor(this);
    {
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        // Depending on the type of t_FORMAT_CONTEXT we may need to use
        // `visit_format_arg` from `bslfmt` or from `std`.
        using namespace std;
#endif
        visit_format_arg(visitor, context.arg(d_value));
    }
}

// ACCESSORS
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
bool FormatterSpecificationNumericValue::operator==(
                        const FormatterSpecificationNumericValue& other) const
{
    return d_category == other.d_category && d_value == other.d_value;
}

inline
BSLS_KEYWORD_CONSTEXPR_CPP20 int
FormatterSpecificationNumericValue::value() const
{
    if (d_category == e_DEFAULT) {
        BSLS_THROW(bsl::format_error("INTERNAL ERROR: Access to unspecified "
                                     "nested value in format spec.")); // THROW
    }
    if (d_category == e_NEXT_ARG) {
        BSLS_THROW(
            bsl::format_error("INTERNAL ERROR: Failed to identify arg-id for "
                              "nested value in format spec."));        // THROW
    }
    return d_value;
}

inline
BSLS_KEYWORD_CONSTEXPR_CPP20
FormatterSpecificationNumericValue::Category
FormatterSpecificationNumericValue::category() const
{
    return d_category;
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
        BSLS_THROW(
            bsl::format_error("Nested value argument out of range"));  // THROW
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

}  // close package namespace
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
