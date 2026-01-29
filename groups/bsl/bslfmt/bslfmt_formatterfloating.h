// bslfmt_formatterfloating.h                                         -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERFLOATING
#define INCLUDED_BSLFMT_FORMATTERFLOATING

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a formatter customization for floating point types
//
//@CLASSES:
//  bsl::formatter<float, t_CHAR>:       formatter template for `float`
//  bsl::formatter<double, t_CHAR>:      formatter template for `double`
//  bsl::formatter<long double, t_CHAR>: disabled formatter template
//
//@CANONICAL_HEADER: bsl_format.h
//
//@DESCRIPTION: This component provides partial specializations of
// `bsl::formatter` catering for floating point types.  The component defines a
// component-private `bslfmt::FormatterFloating_Base` class that implements the
// formatters for the `float` and `double` types, for both supported output
// character types (`char` and `wchar_t`).  Partial specializations are then
// defined that use `bslfmt::FormatterFloating_Base` to specify the actual
// formatters in the `bsl` namespace.  Notice that `long double` is not one of
// the supported types (see [](#`long double`)).
//
// This header is not intended to be included directly.  Please include
// `<bsl_format.h>` to be able to use specializations of the `bsl::formatter`
// for floating point types.
//
///`long double`
///-------------
// The type `long double` is rarely used in code as it is not only not
// guaranteed to provide more precision than `double`, but in fact it is
// exactly the same type as `double` on MSVC (Microsoft Windows), which makes
// any code that uses it for more precision non-portable.
//
// We also do not have a ryu library implementation for `long double` so we
// could only use much slower means of "printing" it than the other two types.
//
// For the above reasons `long double` is now not supported and an attempt to
// print it will result in a runtime error.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example: Formatting a floating point number
///- - - - - - - - - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`, so this example is
// necessarily unrealistic.
//
// Suppose we want to test pointer formatter's ability to format a number in
// hexadecimal format with defined alignment and padding.
//
// ```
//  bslfmt::MockParseContext<char> mpc("*<8a", 1);
//
//  bsl::formatter<double, char> f;
//  mpc.advance_to(f.parse(mpc));
//
//  const double value = 42.24;
//
//  bslfmt::MockFormatContext<char> mfc(value, 0, 0);
//
//  mfc.advance_to(bsl::as_const(f).format(value, mfc));
//
//  assert("1.51eb851eb851fp+5" == mfc.finalString());
// ```

#include <bslscm_version.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>
#include <bslfmt_formattercharutil.h>
#include <bslfmt_padutil.h>
#include <bslfmt_standardformatspecification.h>

#include <bsla_annotations.h>

#include <bslalg_numericformatterutil.h>

#include <bslma_deallocatebytesproctor.h>

#include <bsls_assert.h>
#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>

namespace BloombergLP {
namespace bslfmt {

                 // =============================
                 // struct FormatterFloating_Base
                 // =============================

/// This class template provides the implementation for all possible floating
/// point formatting styles and the parsing of the format specification.  The
/// specified `t_VALUE` template type argument determines the type of floating
/// point value use, while the specified `t_CHAR` determines the output's
/// character type.  The behavior is undefined unless `t_VALUE` is one of
/// `float` or `double`, and `t_CHAR` is on of `char` or `wchar_t`.
template <class t_VALUE, class t_CHAR>
struct FormatterFloating_Base {
  private:
    // PRIVATE TYPES

    /// A type alias for the `StandardFormatSpecification<t_CHAR>`.
    typedef StandardFormatSpecification<t_CHAR> Specification;

    // DATA
    Specification d_spec;  /// Parsed specification.

    /// A constant representing the case when a position is not found in a
    /// string-search.
    static const size_t k_NO_POS = (size_t)(-1);

  private:
    // PRIVATE CLASS METHODS

    /// Convert the default format textual representation of a floating point
    /// number to its alternative format in the specified `buf` of size
    /// `numberLength` by adding a decimal point if necessary and return the
    /// new size of the buffer.  The behavior is undefined unless the buffer
    /// contains a floating point value in its default format, and it is large
    /// enough to store the new (dot) character added.
    static size_t applyDefaultAlternate(char* buf, size_t numberLength);

    /// Convert the fixed format textual representation of a floating point
    /// number to its alternative format in the specified `buf` of size
    /// `numberLength` and return the new size of the buffer.  The behavior is
    /// undefined unless the buffer contains a floating point value in its
    /// fixed format, and it is large enough to store the new (dot) character
    /// added.
    static size_t applyFixedAlternate(char *buf, size_t numberLength);

    /// Convert the general format textual representation of a floating point
    /// number to its alternative format in the specified `buf` of size
    /// `numberLength`, using the specified `precision` to add back removed
    /// trailing zeros and return the new size of the buffer.  The behavior is
    /// undefined unless the buffer contains a floating point value in its
    /// general format, and it is large enough to store the new characters
    /// added.  The behavior is also undefined unless `precision >= 0`.
    static
    size_t applyGeneralAlternate(char   *buf,
                                 size_t  numberLength,
                                 int     precision);

    /// In case there is a decimal point in the scientific or hexadecimal
    /// format textual representation of a number in the specified `buf` filled
    /// up to the specified `numberLength` do nothing and return
    /// `numberLength`. In case there is no decimal point in the `buf` insert
    /// one right before the specified `expChar` position and return
    /// `numberLength + 1`.  If no `expChar` is present in `buf` (assume that
    /// the value is not numeric but NaN or infinity) do nothing and return
    /// `numberLength`.  The behavior is undefined unless `expChar` is `e` when
    /// `buf` contains a number in scientific format, or `p` when it is a
    /// number in hexfloat format.  The behavior is also undefined unless the
    /// buffer is long enough to contain the additional (dot) character.
    static
    size_t applyScientificAlternate(char    expChar,
                                    char   *buf,
                                    size_t  numberLength);

    // PRIVATE MANIPULATORS

    /// Write the specified `numberBuffer` of size `numberLength` aligned with
    /// fills according to the specified `finalSpec` to the output iterator
    /// of the `formatContext` and return an iterator one-past the last
    /// written.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator alignAndCopy(
                                        const char           *numberBuffer,
                                        size_t                numberLength,
                                        t_FORMAT_CONTEXT&     formatContext,
                                        const Specification&  finalSpec) const;

    /// Create the fixed string representation of the specified `value`,
    /// customized in accordance with the requested format in the specified
    /// `finalSpec`, and write the result to the output iterator that the
    /// `formatContext` points to.  The behavior is undefined unless the
    /// requested format is fixed or uppercase fixed.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator formatFixedImpl(
                                         t_VALUE              value,
                                         t_FORMAT_CONTEXT&    formatContext,
                                         const Specification& finalSpec) const;

    /// Create the minimal-round-tripping string representation of the
    /// specified `value`, customized in accordance with the requested format
    /// in the specified `finalSpec`, and write the result to the output
    /// iterator that the `formatContext` points to.  The behavior is undefined
    /// unless the requested format type is default and no precision is
    /// specified.  Notice that when a precision is specified the format used
    /// changes to generic format (not "default").
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator formatDefaultImpl(
                                         t_VALUE              value,
                                         t_FORMAT_CONTEXT&    formatContext,
                                         const Specification& finalSpec) const;

    /// Create the general string representation of the specified `value`,
    /// customized in accordance with the requested format in the specified
    /// `finalSpec`, and write the result to the output iterator that the
    /// `formatContext` points to.  The behavior is undefined unless the
    /// requested format is general or uppercase general.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator formatGeneralImpl(
                                         t_VALUE              value,
                                         t_FORMAT_CONTEXT&    formatContext,
                                         const Specification& finalSpec) const;

    /// Create the minimal-round-tripping hexfloat string representation of the
    /// specified `value`, customized in accordance with the requested format
    /// in the specified `finalSpec`, and write the result to the output
    /// iterator that the `formatContext` points to.  The behavior is undefined
    /// unless the requested format is hexfloat or uppercase hexfloat and no
    /// precision is specified.  Notice that there is a separate implementation
    /// for hexfloats with a specified precision.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator formatHexImpl(
                                         t_VALUE              value,
                                         t_FORMAT_CONTEXT&    formatContext,
                                         const Specification& finalSpec) const;

    /// Create the hexfloat string representation of the specified `value`,
    /// customized in accordance with the requested format in the specified
    /// `finalSpec`, and write the result to the output iterator that the
    /// `formatContext` points to.  The behavior is undefined unless the
    /// requested format is hexfloat or uppercase hexfloat and the required
    /// precision is explicitly specified.  Notice that there is a separate
    /// implementation for hexfloats without a specified precision.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator formatHexPrecImpl(
                                         t_VALUE              value,
                                         t_FORMAT_CONTEXT&    formatContext,
                                         const Specification& finalSpec) const;

    /// Create the scientific string representation of the specified `value`,
    /// customized in accordance with the requested format in the specified
    /// `finalSpec`, and write the result to the output iterator that the
    /// `formatContext` points to.  The behavior is undefined unless the
    /// requested format is scientific or uppercase scientific.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator formatScientificImpl(
                                         t_VALUE              value,
                                         t_FORMAT_CONTEXT&    formatContext,
                                         const Specification& finalSpec) const;
  public:
    // MANIPULATORS

    /// Create string representation of the specified `value`, customized in
    /// accordance with the requested format and the specified `formatContext`,
    /// and write the result to the output iterator that the `formatContext`
    /// points to.  The behavior is undefined unless `t_FORMAT_CONTEXT` is
    /// either `std::format_context` if that is supported, or otherwise
    /// `bslfmt::format_context`.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                        t_VALUE           value,
                                        t_FORMAT_CONTEXT& formatContext) const;

    /// Parse the specified `parseContext`, store the resulting parsed
    /// specification and return an iterator, pointing to the beginning of the
    /// format string.  The behavior is undefined unless `t_PARSE_CONTEXT` is
    /// either `std::parse_context` if that is supported, or otherwise
    /// `bslfmt::parse_context`.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator
    parse(t_PARSE_CONTEXT& parseContext);
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                     // -----------------------------
                     // struct FormatterFloating_Base
                     // -----------------------------

// PRIVATE CLASS METHODS
template <class t_VALUE, class t_CHAR>
size_t
FormatterFloating_Base<t_VALUE, t_CHAR>::applyDefaultAlternate(
                                                          char   *buf,
                                                          size_t  numberLength)
{
    // Note that this method has to work with 3 substantially different-looking
    // formats: scientific, fixed, and the special values (NaN, infinity).

    {
        const char lastChar = buf[numberLength - 1];
        if ('f' == lastChar || 'n' == lastChar) {
            // infinity or NaN, nothing to do
            return numberLength;
        }
    }

    size_t dotPos = k_NO_POS;
    for (size_t i = 0; i < numberLength; ++i) {
        if ('.' == buf[i]) {
            dotPos = i;
            break;                                                     // BREAK
        }
    }

    if (dotPos == k_NO_POS) {
        // When here, we need to insert the decimal point before the `e` if
        // present, or at the end if we printed fixed-form.

        size_t ePos = k_NO_POS;
        for (size_t i = 0; i < numberLength; ++i) {
            if ('e' == buf[i]) {
                dotPos = i;
                break;                                                 // BREAK
            }
        }

        if (k_NO_POS == ePos) {
            buf[numberLength] = '.';
        }
        else {
            // Scientific form, we need to move the exponent part
            memmove(buf + ePos + 1, buf + ePos, numberLength - ePos);
            buf[ePos++] = '.';
        }
        ++numberLength;
    }

    return numberLength;
}

template <class t_VALUE, class t_CHAR>
size_t
FormatterFloating_Base<t_VALUE, t_CHAR>::applyFixedAlternate(
                                                          char   *buf,
                                                          size_t  numberLength)
{
    // Place in a decimal point if none is present

    for (size_t i = 0; i < numberLength; ++i) {
        if ('.' == buf[i]) {
            // Has a decimal point, nothing to do
            return numberLength;                                      // RETURN
        }
    }

    // When here, we need to insert the decimal point at the end
    buf[numberLength] = '.';

    return ++numberLength;
}

template <class t_VALUE, class t_CHAR>
size_t
FormatterFloating_Base<t_VALUE, t_CHAR>::applyGeneralAlternate(
                                                          char   *buf,
                                                          size_t  numberLength,
                                                          int     precision)
{
    // This method adds a decimal point if it is not present and re-adds the
    // removed trailing zeros up to the specified `precision` as required for
    // the alternate format of the generic (`g` and `G`) formats.  Note that
    // this method has to work with 3 substantially different-looking formats:
    // scientific, fixed, and the special value (NaN, infinity).

    {
        const char lastChar = buf[numberLength - 1];
        if ('f' == lastChar || 'n' == lastChar) {
            // infinity or NaN, nothing to do
            return numberLength;
        }
    }

    size_t dotPos = k_NO_POS;
    for (size_t i = 0; i < numberLength; ++i) {
        if ('.' == buf[i]) {
            dotPos = i;
            break;                                                     // BREAK
        }
    }
    size_t ePos = k_NO_POS;
    for (size_t i = 0; i < numberLength; ++i) {
        if ('e' == buf[i]) {
            ePos = i;
            break;                                                     // BREAK
        }
    }

    if (dotPos == k_NO_POS) {
        // When here, we need to insert the decimal point before the `e` if
        // present, or at the end if we printed fixed-form.

        if (k_NO_POS == ePos) {
            buf[numberLength] = '.';
        }
        else {
            // Scientific form, we need to move the exponent part
            memmove(buf + ePos + 1, buf + ePos, numberLength - ePos);
            buf[ePos++] = '.';
        }
        ++numberLength;
    }

    // Alternate form also has to "put back" all removed trailing zeros up to
    // the specified precision.
    const ptrdiff_t digits = (k_NO_POS == ePos)
                            ? numberLength - 1
                            : ePos - 1;
    if (precision > digits) {
        const size_t numZeros = precision - digits;

        // We need to put in some zeros
        if (k_NO_POS == ePos) {
            // Fixed format, zeros just go to the end
            memset(buf + numberLength, '0', numZeros);
        }
        else {
            // Scientific form, we need to move the exponent part
            memmove(buf + ePos + numZeros,
                    buf + ePos,
                    numberLength - ePos);
            memset(buf + ePos, '0', numZeros);
            ePos += numZeros;
        }
        numberLength += numZeros;
    }

    return numberLength;
}

template <class t_VALUE, class t_CHAR>
size_t
FormatterFloating_Base<t_VALUE, t_CHAR>::applyScientificAlternate(
                                                          char    expChar,
                                                          char   *buf,
                                                          size_t  numberLength)
{
    {
        const char lastChar = buf[numberLength - 1];
        if ('f' == lastChar || 'n' == lastChar) {
            // infinity or NaN, nothing to do
            return numberLength;                                      // RETURN
        }
    }

    for (size_t i = 0; i < numberLength; ++i) {
        if ('.' == buf[i]) {
            return numberLength;                                      // RETURN
        }
    }

    // When here, we need to insert the decimal point before the exponent
    // character.

    size_t expPos = k_NO_POS;
    for (size_t i = 0; i < numberLength; ++i) {
        if (expChar == buf[i]) {
            expPos = i;
            break;                                                     // BREAK
        }
    }

    if (k_NO_POS == expPos) {
        // This must not happen as we have a number in scientific format
        BSLS_THROW(bsl::format_error("Internal error, exponent not found"));
    }

    // Scientific form, we need to move the exponent part
    memmove(buf + expPos + 1, buf + expPos, numberLength - expPos);
    buf[expPos] = '.';

    return ++numberLength; // We added a '.'
}


// PRIVATE MANIPULATORS
template <class t_VALUE, class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
FormatterFloating_Base<t_VALUE, t_CHAR>::alignAndCopy(
                                         const char           *numberBuffer,
                                         size_t                numberLength,
                                         t_FORMAT_CONTEXT&     formatContext,
                                         const Specification&  finalSpec) const
{
    typedef FormatterSpecificationNumericValue NumericValue;
    typedef bsl::basic_string_view<t_CHAR>     StringView;
    typedef PadUtil<t_CHAR>                    PadUtil;

    NumericValue finalWidth(finalSpec.postprocessedWidth());

    std::ptrdiff_t leftPadFillerCopiesNum  = 0;
    std::ptrdiff_t rightPadFillerCopiesNum = 0;
    std::ptrdiff_t zeroPadFillerCopiesNum  = 0;

    const char addSignChar = *numberBuffer == '-'
                           ? '-'
                           : Specification::e_SIGN_POSITIVE == finalSpec.sign()
                           ? '+'
                           : Specification::e_SIGN_SPACE == finalSpec.sign()
                           ? ' '
                           : '\0';
    const bool hasSignChar = (addSignChar != 0);

    if ('-' == addSignChar) {
        // We are going to add the sign "by hand"
        ++numberBuffer;
        --numberLength;
    }

    // Check if we have a non-numerical value of "inf" or "nan", in which case
    // we must no use zero padding.
    const char lastChar = numberBuffer[numberLength - 1];
    const bool specialValue = 'f' == lastChar || 'n' == lastChar;

    if (finalWidth.category() != NumericValue::e_DEFAULT &&
        numberLength + hasSignChar < static_cast<size_t>(finalWidth.value())) {
        // We need to fill the remaining space.

        if (!specialValue &&
                     Specification::e_ALIGN_DEFAULT == finalSpec.alignment() &&
                                                 finalSpec.zeroPaddingFlag()) {
            // Space will be filled with zeros.

            zeroPadFillerCopiesNum = finalWidth.value() -
                                                  (numberLength + hasSignChar);
        }
        else {
            // Alignment with appropriate symbol is required.

            PadUtil::computePadding(&leftPadFillerCopiesNum,
                                    &rightPadFillerCopiesNum,
                                    finalWidth,
                                    numberLength + hasSignChar,
                                    d_spec.alignment(),
                                    Specification::e_ALIGN_RIGHT);
        }
    }

    // Assembling the final string.

    typename t_FORMAT_CONTEXT::iterator outIterator = formatContext.out();

    const StringView pad(finalSpec.filler(), finalSpec.numFillerCharacters());
    outIterator = PadUtil::pad(outIterator, leftPadFillerCopiesNum, pad);

    if (addSignChar) {
        outIterator = FormatterCharUtil<t_CHAR>::outputFromChar(
                                                     addSignChar, outIterator);
    }

    outIterator = PadUtil::pad(outIterator, zeroPadFillerCopiesNum, '0');

    outIterator = FormatterCharUtil<t_CHAR>::outputFromChar(
                                                   numberBuffer,
                                                   numberBuffer + numberLength,
                                                   outIterator);

    outIterator = PadUtil::pad(outIterator, rightPadFillerCopiesNum, pad);

    return outIterator;
}

template <class t_VALUE, class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
FormatterFloating_Base<t_VALUE, t_CHAR>::formatFixedImpl(
                                          t_VALUE              value,
                                          t_FORMAT_CONTEXT&    formatContext,
                                          const Specification& finalSpec) const
{
    typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;

    /// Must be decimal format
    BSLS_ASSERT(Specification::e_FLOATING_FIXED == finalSpec.formatType() ||
                Specification::e_FLOATING_FIXED_UC == finalSpec.formatType());

    // Determine the precision
    typedef FormatterSpecificationNumericValue NumericValue;
    const NumericValue& specPrec = finalSpec.postprocessedPrecision();
    const int precision = (NumericValue::e_DEFAULT == specPrec.category())
                        ? 6
                        : specPrec.value();

    // Converting to string
    const size_t k_STACK_BUF_LEN =   // Max useful precision 17 - dflt 6 => 11
              NFUtil::ToCharsMaxLength<t_VALUE, NFUtil::e_FIXED>::k_VALUE + 11;
    char sbuf[k_STACK_BUF_LEN];
    char *dbuf = 0;

    size_t  bufLen = k_STACK_BUF_LEN;
    char   *buf = sbuf;

    const size_t reqdBufLen =
        NFUtil::PrecisionMaxBufferLength<t_VALUE>::value(NFUtil::e_FIXED,
                                                         precision);
    bsl::polymorphic_allocator<char> allocator;
    if (reqdBufLen > k_STACK_BUF_LEN) {
        dbuf   = allocator.allocate(reqdBufLen);
        buf    = dbuf;
        bufLen = reqdBufLen;
    }
    bslma::DeallocateBytesProctor<bsl::polymorphic_allocator<char> > proctor(
                                                        allocator,
                                                        dbuf,
                                                        dbuf ? reqdBufLen : 0);
    char *end = NFUtil::toChars(buf,
                                buf + bufLen,
                                value,
                                NFUtil::e_FIXED,
                                precision);

    size_t numberLength = static_cast<size_t>(end - buf);

    if (finalSpec.alternativeFlag()) {
        numberLength = applyFixedAlternate(buf, numberLength);
    }

    return alignAndCopy(buf, numberLength, formatContext, finalSpec);
}

template <class t_VALUE, class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
FormatterFloating_Base<t_VALUE, t_CHAR>::formatDefaultImpl(
                                          t_VALUE              value,
                                          t_FORMAT_CONTEXT&    formatContext,
                                          const Specification& finalSpec) const
{
    typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;

    /// Must be default format
    BSLS_ASSERT(Specification::e_FLOATING_DEFAULT == finalSpec.formatType());

    /// Must not have precision
    BSLS_ASSERT(finalSpec.postprocessedPrecision().category() ==
                FormatterSpecificationNumericValue::e_DEFAULT);

    // Converting to string
    char buf[NFUtil::ToCharsMaxLength<t_VALUE>::k_VALUE];
    char *end = NFUtil::toChars(buf,
                                buf + sizeof(buf),
                                value);

    size_t numberLength = static_cast<size_t>(end - buf);

    if (finalSpec.alternativeFlag()) {
        numberLength = applyDefaultAlternate(buf, numberLength);
    }

    return alignAndCopy(buf, numberLength, formatContext, finalSpec);
}

template <class t_VALUE, class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
FormatterFloating_Base<t_VALUE, t_CHAR>::formatGeneralImpl(
                                          t_VALUE              value,
                                          t_FORMAT_CONTEXT&    formatContext,
                                          const Specification& finalSpec) const
{
    typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;

    /// Must be general format
    BSLS_ASSERT(
              Specification::e_FLOATING_GENERAL == finalSpec.formatType() ||
              Specification::e_FLOATING_GENERAL_UC == finalSpec.formatType() ||
              Specification::e_FLOATING_DEFAULT == finalSpec.formatType());

    // Determine the precision
    typedef FormatterSpecificationNumericValue FSNVAlue;
    const FSNVAlue& specPrec = finalSpec.postprocessedPrecision();
    const int precision = (FSNVAlue::e_DEFAULT == specPrec.category())
                        ? 6
                        : specPrec.value();

    // Converting to string
    const size_t k_STACK_BUF_LEN =       // max useful prec 17 - deflt 6 => 11
            NFUtil::ToCharsMaxLength<t_VALUE, NFUtil::e_GENERAL>::k_VALUE + 11;
    char  sbuf[k_STACK_BUF_LEN];
    char *dbuf = 0;

    size_t  bufLen = k_STACK_BUF_LEN;
    char   *buf = sbuf;

    const size_t reqdBufLen =
            NFUtil::PrecisionMaxBufferLength<t_VALUE>::value(NFUtil::e_GENERAL,
                                                             precision);
    bsl::polymorphic_allocator<char> allocator;
    if (reqdBufLen > k_STACK_BUF_LEN) {
        dbuf   = allocator.allocate(reqdBufLen);
        buf    = dbuf;
        bufLen = reqdBufLen;
    }
    bslma::DeallocateBytesProctor<bsl::polymorphic_allocator<char> > proctor(
                                                        allocator,
                                                        dbuf,
                                                        dbuf ? reqdBufLen : 0);
    char *end = NFUtil::toChars(buf,
                                buf + bufLen,
                                value,
                                NFUtil::e_GENERAL,
                                precision);

    size_t numberLength = static_cast<size_t>(end - buf);

    if (finalSpec.alternativeFlag()) {
        numberLength = applyGeneralAlternate(buf, numberLength, precision);
    }

    if (Specification::e_FLOATING_GENERAL_UC == finalSpec.formatType()) {
        BloombergLP::bslfmt::FormatterCharUtil<char>::toUpper(buf, end);
    }

    return alignAndCopy(buf, numberLength, formatContext, finalSpec);
}

template <class t_VALUE, class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
FormatterFloating_Base<t_VALUE, t_CHAR>::formatHexImpl(
                                          t_VALUE              value,
                                          t_FORMAT_CONTEXT&    formatContext,
                                          const Specification& finalSpec) const
{
    typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;

    /// Must be hexfloat format
    BSLS_ASSERT(Specification::e_FLOATING_HEX == finalSpec.formatType() ||
                Specification::e_FLOATING_HEX_UC == finalSpec.formatType());

    /// Must not have precision
    BSLS_ASSERT(finalSpec.postprocessedPrecision().category() ==
                FormatterSpecificationNumericValue::e_DEFAULT);

    // Converting to string
    char buf[NFUtil::ToCharsMaxLength<t_VALUE, NFUtil::e_HEX>::k_VALUE];
    char *end = NFUtil::toChars(buf,
                                buf + sizeof(buf),
                                value,
                                NFUtil::e_HEX);

    size_t numberLength = static_cast<size_t>(end - buf);

    if (finalSpec.alternativeFlag()) {
        numberLength = applyScientificAlternate('p', buf, numberLength);
    }

    if (Specification::e_FLOATING_HEX_UC == finalSpec.formatType()) {
        BloombergLP::bslfmt::FormatterCharUtil<char>::toUpper(buf, end);
    }

    return alignAndCopy(buf, numberLength, formatContext, finalSpec);
}

template <class t_VALUE, class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
FormatterFloating_Base<t_VALUE, t_CHAR>::formatHexPrecImpl(
                                          t_VALUE              value,
                                          t_FORMAT_CONTEXT&    formatContext,
                                          const Specification& finalSpec) const
{
    typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;

    /// Must be hexfloat format
    BSLS_ASSERT(Specification::e_FLOATING_HEX == finalSpec.formatType() ||
                Specification::e_FLOATING_HEX_UC == finalSpec.formatType());

    /// Must have precision
    BSLS_ASSERT(finalSpec.postprocessedPrecision().category() !=
                FormatterSpecificationNumericValue::e_DEFAULT);

    // Converting to string
    const size_t k_STACK_BUF_LEN =
                     NFUtil::ToCharsMaxLength<t_VALUE, NFUtil::e_HEX>::k_VALUE;
    char sbuf[k_STACK_BUF_LEN];
    char *dbuf = 0;

    size_t  bufLen = k_STACK_BUF_LEN;
    char   *buf = sbuf;

    const size_t reqdBufLen =
        NFUtil::PrecisionMaxBufferLength<t_VALUE>::value(
                                   NFUtil::e_HEX,
                                   finalSpec.postprocessedPrecision().value());
    bsl::polymorphic_allocator<char> allocator;
    if (reqdBufLen > k_STACK_BUF_LEN) {
        dbuf   = allocator.allocate(reqdBufLen);
        buf    = dbuf;
        bufLen = reqdBufLen;
    }
    bslma::DeallocateBytesProctor<bsl::polymorphic_allocator<char> > proctor(
                                                        allocator,
                                                        dbuf,
                                                        dbuf ? reqdBufLen : 0);
    char *end = NFUtil::toChars(buf,
                                buf + bufLen,
                                value,
                                NFUtil::e_HEX,
                                finalSpec.postprocessedPrecision().value());

    size_t numberLength = static_cast<size_t>(end - buf);

    if (finalSpec.alternativeFlag()) {
        numberLength = applyScientificAlternate('p', buf, numberLength);
    }

    if (Specification::e_FLOATING_HEX_UC == finalSpec.formatType()) {
        BloombergLP::bslfmt::FormatterCharUtil<char>::toUpper(buf, end);
    }

    return alignAndCopy(buf, numberLength, formatContext, finalSpec);
}

template <class t_VALUE, class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
FormatterFloating_Base<t_VALUE, t_CHAR>::formatScientificImpl(
                                          t_VALUE              value,
                                          t_FORMAT_CONTEXT&    formatContext,
                                          const Specification& finalSpec) const
{
    typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;

    /// Must be scientific format
    BSLS_ASSERT(
            Specification::e_FLOATING_SCIENTIFIC == finalSpec.formatType() ||
            Specification::e_FLOATING_SCIENTIFIC_UC == finalSpec.formatType());

    // Determine the precision
    typedef FormatterSpecificationNumericValue FSNVAlue;
    const FSNVAlue& specPrec = finalSpec.postprocessedPrecision();
    const int precision = (FSNVAlue::e_DEFAULT == specPrec.category())
                        ? 6
                        : specPrec.value();

    // Converting to string
    const size_t k_STACK_BUF_LEN =       // max useful prec 17 - deflt 6 => 11
         NFUtil::ToCharsMaxLength<t_VALUE, NFUtil::e_SCIENTIFIC>::k_VALUE + 11;
    char  sbuf[k_STACK_BUF_LEN];
    char *dbuf = 0;

    size_t  bufLen = k_STACK_BUF_LEN;
    char   *buf = sbuf;

    const size_t reqdBufLen =
        NFUtil::PrecisionMaxBufferLength<t_VALUE>::value(NFUtil::e_SCIENTIFIC,
                                                         precision);
    bsl::polymorphic_allocator<char> allocator;
    if (reqdBufLen > k_STACK_BUF_LEN) {
        dbuf   = allocator.allocate(reqdBufLen);
        buf    = dbuf;
        bufLen = reqdBufLen;
    }
    bslma::DeallocateBytesProctor<bsl::polymorphic_allocator<char> > proctor(
                                                        allocator,
                                                        dbuf,
                                                        dbuf ? reqdBufLen : 0);
    char *end = NFUtil::toChars(buf,
                                buf + bufLen,
                                value,
                                NFUtil::e_SCIENTIFIC,
                                precision);

    size_t numberLength = static_cast<size_t>(end - buf);

    if (finalSpec.alternativeFlag()) {
        numberLength = applyScientificAlternate('e', buf, numberLength);
    }

    if (Specification::e_FLOATING_SCIENTIFIC_UC == finalSpec.formatType()) {
        BloombergLP::bslfmt::FormatterCharUtil<char>::toUpper(buf, end);
    }

    return alignAndCopy(buf, numberLength, formatContext, finalSpec);
}

// MANIPULATORS
template <class t_VALUE, class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
FormatterFloating_Base<t_VALUE, t_CHAR>::format(
                                         t_VALUE           value,
                                         t_FORMAT_CONTEXT& formatContext) const
{
    typedef FormatterSpecificationNumericValue FSNVAlue;

    Specification finalSpec(d_spec);
    finalSpec.postprocess(formatContext);

    const bool isDefaultPrecision =
         FSNVAlue::e_DEFAULT == finalSpec.postprocessedPrecision().category();

    switch (finalSpec.formatType()) {
      case Specification::e_FLOATING_DEFAULT: {
        if (isDefaultPrecision) {
            return formatDefaultImpl(value, formatContext, finalSpec);
        }
        else {
            return formatGeneralImpl(value, formatContext, finalSpec);
        }
      } break;

      case Specification::e_FLOATING_HEX: BSLA_FALLTHROUGH;
      case Specification::e_FLOATING_HEX_UC: {
        if (isDefaultPrecision) {
            return formatHexImpl(value, formatContext, finalSpec);
        }
        else {
            return formatHexPrecImpl(value, formatContext, finalSpec);
        }
      } break;

      case Specification::e_FLOATING_SCIENTIFIC: BSLA_FALLTHROUGH;
      case Specification::e_FLOATING_SCIENTIFIC_UC: {
        return formatScientificImpl(value, formatContext, finalSpec);
      } break;

      case Specification::e_FLOATING_FIXED: BSLA_FALLTHROUGH;
      case Specification::e_FLOATING_FIXED_UC: {
        return formatFixedImpl(value, formatContext, finalSpec);
      } break;

      case Specification::e_FLOATING_GENERAL: BSLA_FALLTHROUGH;
      case Specification::e_FLOATING_GENERAL_UC: {
        return formatGeneralImpl(value, formatContext, finalSpec);
      } break;

      default: {
        BSLS_THROW(bsl::format_error("Unknown format"));
      }
    }
}

template <class t_VALUE, class t_CHAR>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator
FormatterFloating_Base<t_VALUE, t_CHAR>::parse(t_PARSE_CONTEXT& parseContext)
{
    d_spec.parse(&parseContext, Specification::e_CATEGORY_FLOATING);

    if (d_spec.localeSpecificFlag()) {
        BSLS_THROW(bsl::format_error(
                          "Formatting with the L specifier is not supported"));
    }

    return parseContext.begin();
}

}  // close package namespace
}  // close enterprise namespace

namespace bsl {
// FORMATTER SPECIALIZATIONS

/// This template partial specialization defines `bsl::formatter` for `float`
/// values for both (`char` and `wchar_t`) character types.
template <class t_CHAR>
struct formatter<float, t_CHAR>
: BloombergLP::bslfmt::FormatterFloating_Base<float, t_CHAR> {
};

/// This template partial specialization defines `bsl::formatter` for `float`
/// values for both (`char` and `wchar_t`) character types.
template <class t_CHAR>
struct formatter<double, t_CHAR>
: BloombergLP::bslfmt::FormatterFloating_Base<double, t_CHAR> {
};

/// This template partial specialization defines a disabled `bsl::formatter`
/// for `long double` values for both (`char` and `wchar_t`) character types.
/// Note that `long double` is disabled because we do not have a performant
/// implementation for this type, and it is also rarely used because it
/// essentially does not exist on MSVC because it is the same size and layout
/// as `double`.  Plan is to implement it only in case it is actually needed.
template <class t_CHAR>
struct formatter<long double, t_CHAR> {
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                 t_PARSE_CONTEXT& parseContext)
    {
        (void)parseContext;
        BSLS_THROW(bsl::format_error(
                           "Formatting `long double` is not implemented yet"));
    }

    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                         long double       value,
                                         t_FORMAT_CONTEXT& formatContext) const
    {
        (void)value;
        (void)formatContext;
        BSLS_THROW(bsl::format_error(
                           "Formatting `long double` is not implemented yet"));
    }
};

}  // close namespace bsl

#endif  // INCLUDED_BSLFMT_FORMATTERFLOATING

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
