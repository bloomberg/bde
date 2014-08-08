// bdldfp_decimal.cpp                                                 -*-C++-*-

// TODO: Remove this #define and the generated bits...

#define BDLDFP_DISABLE_COMPILE \
typedef char Unsupported_Platform[ -1 ];

#ifndef BDLDFP_DECIMAL_SUN_WORKAROUND

// For better ways of binary-decimal FP conversion see:
// http://www.serpentine.com/blog/2011/06/29/...
//  ...here-be-dragons-advances-in-problems-you-didnt-even-know-you-had/
// http://tinyurl.com/3go5o7z

// Implementation notes:
//
// 48 buffer size for printing binary floats: according to the IEEE specs
// _Decimal128 would need a maximum 44 character buffer for scientific format
//  printing (34 digits, 4 chars for +/- of the mantissa and exponent, the 'e'
// for scientific, 1 more for the decimal point and then 1 more for the NUL
// terminator).  Since we would like to be cache friendly we use 48 (dividable
// by 8, so it is also alignment friendly).  For simplicity, we use the same
// size everywhere.  (see buff[48] in the code).
//
// 24 characters buffer is used for printing integers.  Unsigned long long (64
// bits) may print 20 digits at its max value.  The test driver contains an
// assert to check that 24 is indeed enough.  Why don't we use the
// numeric_limits<>::digits10?  Because it is "helpfully" broken on MS Visual
// C++ 2008, reports 18 & not 19.

#include <bdldfp_decimal.h>

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

#include <bsl_algorithm.h>
#include <bsl_functional.h>
#include <bsl_istream.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
#include <math.h>
#include <stdio.h>
#endif

#include <bdldfp_bufferbuf.h>

namespace BloombergLP {
namespace bdldfp {

namespace {
                    // ================
                    // class NotIsSpace
                    // ================

template <class CHARTYPE>
class NotIsSpace {
    // Helper function object type used to skip spaces in strings
    const bsl::ctype<CHARTYPE>& d_ctype;
  public:
    explicit NotIsSpace(const bsl::ctype<CHARTYPE>& ctype);
        // Construct a 'NotIsSpace' object, using the specified 'ctype'.
    bool operator()(CHARTYPE character) const;
        // Return true if the specified 'character' is a space (according to
        // the 'ctype' provided at construction), and false otherwise.
};

                    // ----------------
                    // class NotIsSpace
                    // ----------------

template <class CHARTYPE>
NotIsSpace<CHARTYPE>::NotIsSpace(const bsl::ctype<CHARTYPE>& ctype)
: d_ctype(ctype)
{
}

template <class CHARTYPE>
bool
NotIsSpace<CHARTYPE>::operator()(CHARTYPE character) const
{
    return !this->d_ctype.is(bsl::ctype_base::space, character);
}

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR

#  ifndef  __STDC_WANT_DEC_FP__
#    error __STDC_WANT_DEC_FP__ must be defined on the command line!
     char die[sizeof(Decimal_Assert)];     // if '#error' unsupported
#  endif


        // Implementation when we have C DecFP support only (no C++)

                   // C99 stream-output converters

static
char *format(const DecimalImpUtil::ValueType32 *value, char *buffer, int n)
{
    // TBD TODO - printf is locale dependent!!!

    BSLS_ASSERT(value);
    BSLS_ASSERT(buffer);

    snprintf(buffer, n, "%#.7HG", *value);
    return buffer;
}

static
char *format(const DecimalImpUtil::ValueType64 *value, char *buffer, int n)
{
    // TBD TODO - printf is locale dependent!!!

    BSLS_ASSERT(value);
    BSLS_ASSERT(buffer);

    snprintf(buffer, n, "%#.16DG", *value);
    return buffer;
}

static
char *format(const DecimalImpUtil::ValueType128 *value, char *buffer, int n)
{
    // TBD TODO - printf is locale dependent!!!

    BSLS_ASSERT(value);
    BSLS_ASSERT(buffer);

    snprintf(buffer, n, "%#.34DDG", *value);
    return buffer;
}

#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)

     // Implementation based on the decNumber library (no C or C++ support)

              // Implementation specific helper functions

static
char *format(const DecimalImpUtil::ValueType32 *value, char *buffer, int)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(buffer);

    return decSingleToString(value, buffer);
}

static
char *format(const DecimalImpUtil::ValueType64 *value, char *buffer, int)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(buffer);

    return decDoubleToString(value, buffer);
}

static
char *format(const DecimalImpUtil::ValueType128 *value, char *buffer, int)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(buffer);

    return decQuadToString(value, buffer);
}

static decContext* getContext()
    // Provides the decimal context required by the decNumber library functions
{
    static decContext context = { 0, 0, 0, DEC_ROUND_HALF_EVEN, 0, 0, 0 };
    return &context;
}

#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)

    // Implementation based upon the Intel DFP library (no C or C++ support)

              // Implementation specific helper functions

static char *format(const DecimalImpUtil::ValueType32  *value,
                    char                               *buffer,
                    int                                 size);
static char *format(const DecimalImpUtil::ValueType64  *value,
                    char                               *buffer,
                    int                                 size);
static char *format(const DecimalImpUtil::ValueType128 *value,
                    char                               *buffer,
                    int                                 size);
    // Produce a string representation of the specified decimal 'value', in the
    // specified 'buffer', which has at least as much space as specified by
    // 'size'.

static
char *format(const DecimalImpUtil::ValueType32 *value, char *buffer, int size)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(buffer);
    DecimalImpUtil::ValueType64 tmp;
    tmp.d_raw = __bid32_to_bid64(value->d_raw);

    return format(&tmp, buffer, size);
}

static
char *format(const DecimalImpUtil::ValueType64 *value, char *buffer, int)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(buffer);

    __bid64_to_string(buffer, value->d_raw);

    return buffer;
}

static
char *format(const DecimalImpUtil::ValueType128 *value, char *buffer, int)
    // Produce a string representation of the specified decimal 'value', in the
    // specified 'buffer', which has at least as much space as specified by
    // 'size'.
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(buffer);

    __bid128_to_string(buffer, value->d_raw);

    return buffer;
}

#else

BDLDFP_DISABLE_COMPILE; // Unsupported platform

#endif // elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)

                         // Print helper function

template <class CHARTYPE, class TRAITS, class DECIMAL>
bsl::basic_ostream<CHARTYPE, TRAITS>&
print(bsl::basic_ostream<CHARTYPE, TRAITS>& out,
      DECIMAL                               value)
{
    typename bsl::basic_ostream<CHARTYPE, TRAITS>::sentry kerberos(out);
    if (kerberos) {
        typedef BloombergLP::bdldfp::DecimalNumPut<CHARTYPE> Facet;
        const Facet& facet(bsl::has_facet<Facet>(out.getloc())
                           ? bsl::use_facet<Facet>(out.getloc())
                           : Facet::object());
        facet.put(bsl::ostreambuf_iterator<CHARTYPE, TRAITS>(out),
                  out,
                  out.fill(),
                  value);
    }
    return out;
}

                         // Read helper function

template <class CHARTYPE, class TRAITS, class DECIMAL>
bsl::basic_istream<CHARTYPE, TRAITS>&
read(bsl::basic_istream<CHARTYPE, TRAITS>& in,
     DECIMAL&                              value)
{
    typename bsl::basic_istream<CHARTYPE, TRAITS>::sentry kerberos(in);
    if (kerberos) {
        typedef BloombergLP::bdldfp::DecimalNumGet<CHARTYPE> Facet;
        const Facet& facet(bsl::has_facet<Facet>(in.getloc())
                           ? bsl::use_facet<Facet>(in.getloc())
                           : Facet::object());
        bsl::ios_base::iostate err = bsl::ios_base::iostate();
        facet.get(bsl::istreambuf_iterator<CHARTYPE, TRAITS>(in),
                  bsl::istreambuf_iterator<CHARTYPE, TRAITS>(),
                  in,
                  err,
                  value);
        if (err) {
            in.setstate(err);
        }
    }
    return in;
}


template <class ITER_TYPE, class CHAR_TYPE>
ITER_TYPE fillN(ITER_TYPE iter, int numCharacters, CHAR_TYPE character)
    // Assign to the specified output 'iter' the specified 'character' the
    // specified 'numCharacters' times, incrementing 'iter' between each
    // assignment, and then return the resulting incremented iterator.   Note
    // that this is an implementation of C++11 standard 'std::fill_n' that has
    // been specifialized slightly for filling characters; it is provided here
    // because the C++98 definition of 'fill_n' returns 'void'.
{
  while (numCharacters > 0) {
    *iter = character;
    ++iter;
    --numCharacters;
  }
  return iter;
}

template <class ITER_TYPE, class CHAR_TYPE>
ITER_TYPE
doPutCommon(ITER_TYPE       out,
            bsl::ios_base&  format,
            CHAR_TYPE       fillCharacter,
            char           *buffer)
    // Widen the specified 'buffer' into a string of the specified 'CHAR_TYPE',
    // and output the represented decimal number to the specified 'out',
    // adjusting for the formatting flags in the specified 'format' and using
    // the specified 'fillCharacter'.  Currently, formatting for the
    // formatting flags of justification, width, uppercase, and showpos are
    // supported.
{
    const int size = strlen(buffer);
    char *end = buffer + size;

    // Widen the buffer.
    CHAR_TYPE wbuffer[BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE];

    bsl::use_facet<std::ctype<CHAR_TYPE> >(
                                  format.getloc()).widen(buffer, end, wbuffer);

    const int  width   = format.width();
    const bool showPos = format.flags() & bsl::ios_base::showpos;
    const bool hasSign = wbuffer[0] == bsl::use_facet<bsl::ctype<CHAR_TYPE> >(
                                                 format.getloc()).widen('-') ||
                         wbuffer[0] == bsl::use_facet<bsl::ctype<CHAR_TYPE> >(
                                                 format.getloc()).widen('+');
    const bool addPlusSign = showPos & !hasSign;  // Do we need to add '+'?

    int surplus = bsl::max(0, width - size);  // Emit this many fillers.
    if (addPlusSign) {
        // Need to add a '+' character.
        --surplus;
    }

    CHAR_TYPE *wend       = wbuffer + size;
    CHAR_TYPE *wbufferPos = wbuffer;


    // Make use of the 'uppercase' flag to fix the capitalization of the
    // alphabets in the number.

    if (format.flags() & bsl::ios_base::uppercase) {
        bsl::use_facet<bsl::ctype<CHAR_TYPE> >(
                                       format.getloc()).toupper(wbuffer, wend);
    }
    else {
        bsl::use_facet<bsl::ctype<CHAR_TYPE> >(
                                       format.getloc()).tolower(wbuffer, wend);
    }

    switch (format.flags() & bsl::ios_base::adjustfield) {
      case bsl::ios_base::left: {

          // Left justify. Pad characters to the right.

          if (addPlusSign) {
              *out++ = '+';
          }

          out = bsl::copy(wbufferPos, wend, out);
          out = fillN(out, surplus, fillCharacter);
          break;
      }

      case bsl::ios_base::internal: {

          // Internal justify. Pad characters after sign.

          if (hasSign) {
              *out++ = *wbufferPos++;
          }
          else if (addPlusSign) {
              *out++ = '+';
          }

          out = fillN(out, surplus, fillCharacter);
          out = bsl::copy(wbufferPos, wend, out);
          break;
      }

      case bsl::ios_base::right:
      default: {

          // Right justify. Pad characters to the left.

          out = fillN(out, surplus, fillCharacter);

          if (addPlusSign) {
              *out++ = '+';
          }

          out = bsl::copy(wbufferPos, wend, out);
          break;
      }
    }

    return out;
}

} // end of anonymous namespace


                            // -------------------
                            // class DecimalNumGet
                            // -------------------

template <class CHARTYPE, class INPUTITERATOR>
bsl::locale::id DecimalNumGet<CHARTYPE, INPUTITERATOR>::id;

#ifdef BSLS_PLATFORM_CMP_SUN
template <class CHARTYPE, class INPUTITERATOR>
bsl::locale::id& DecimalNumGet<CHARTYPE, INPUTITERATOR>::__get_id() const
{
    return id;
}
#endif

template <class CHARTYPE, class INPUTITERATOR>
const DecimalNumGet<CHARTYPE, INPUTITERATOR>&
DecimalNumGet<CHARTYPE, INPUTITERATOR>::object()
{
    static DecimalNumGet<CHARTYPE, INPUTITERATOR> rc;
    return rc;
}

template <class CHARTYPE, class INPUTITERATOR>
DecimalNumGet<CHARTYPE, INPUTITERATOR>::DecimalNumGet(bsl::size_t refs)
    : bsl::locale::facet(refs)
{
}

template <class CHARTYPE, class INPUTITERATOR>
DecimalNumGet<CHARTYPE, INPUTITERATOR>::~DecimalNumGet()
{
}

template <class CHARTYPE, class INPUTITERATOR>
typename DecimalNumGet<CHARTYPE, INPUTITERATOR>::iter_type
DecimalNumGet<CHARTYPE, INPUTITERATOR>::get(
                                           iter_type               begin,
                                           iter_type               end,
                                           bsl::ios_base&          str,
                                           bsl::ios_base::iostate& err,
                                           Decimal32&              value) const
{
    return this->do_get(begin, end, str, err, value);
}
template <class CHARTYPE, class INPUTITERATOR>
typename DecimalNumGet<CHARTYPE, INPUTITERATOR>::iter_type
DecimalNumGet<CHARTYPE, INPUTITERATOR>::get(
                                           iter_type               begin,
                                           iter_type               end,
                                           bsl::ios_base&          str,
                                           bsl::ios_base::iostate& err,
                                           Decimal64&              value) const
{
    return this->do_get(begin, end, str, err, value);
}
template <class CHARTYPE, class INPUTITERATOR>
typename DecimalNumGet<CHARTYPE, INPUTITERATOR>::iter_type
DecimalNumGet<CHARTYPE, INPUTITERATOR>::get(
                                           iter_type               begin,
                                           iter_type               end,
                                           bsl::ios_base&          str,
                                           bsl::ios_base::iostate& err,
                                           Decimal128&             value) const
{
    return this->do_get(begin, end, str, err, value);
}

                        // do_get Functions

template <class CHARTYPE, class INPUTITERATOR>
typename DecimalNumGet<CHARTYPE, INPUTITERATOR>::iter_type
DecimalNumGet<CHARTYPE, INPUTITERATOR>::do_get(
                                           iter_type               begin,
                                           iter_type               end,
                                           bsl::ios_base&          str,
                                           bsl::ios_base::iostate& err,
                                           Decimal32&              value) const
{
    typedef bsl::ctype<CHARTYPE> Ctype;
    Ctype const& ctype(bsl::use_facet<Ctype>(str.getloc()));

    char        buffer[512];
    char*       to(buffer);
    char* const toEnd(buffer + (sizeof(buffer) - 1));
    CHARTYPE    separator(bsl::use_facet<bsl::numpunct<CHARTYPE> >(
                                                str.getloc()).thousands_sep());
    bool        hasDigit(false);

    // optional sign
    if (begin != end
        && (ctype.narrow(*begin, ' ') == '-'
            || ctype.narrow(*begin, ' ') == '+')) {
        *to = ctype.narrow(*begin, ' ');
        ++to;
        ++begin;
    }
    // spaces between sign and value
    begin = bsl::find_if(begin, end, NotIsSpace<CHARTYPE>(ctype));
    // non-fractional part
    for (; begin != end && to != toEnd
          && (ctype.is(bsl::ctype_base::digit, *begin) || *begin == separator);
         ++begin, ++to) {
        if (*begin != separator) {
            //-dk:TODO TBD store separators for later check
            hasDigit = true;
            *to = ctype.narrow(*begin, ' ');
        }
    }
    // fractional part
    if (begin != end && to != toEnd && ctype.narrow(*begin, ' ') == '.') {
        *to = '.';
        ++begin;
        ++to;
        for (; begin != end && to != toEnd
                 && ctype.is(bsl::ctype_base::digit, *begin);
             ++begin, ++to) {
            hasDigit = true;
            *to = ctype.narrow(*begin, ' ');
        }
    }
    // exponent (but not a stand-alone exponent
    if (hasDigit && begin != end && to != toEnd
        && ctype.narrow(ctype.tolower(*begin), ' ') == 'e') {
        *to = 'e';
        ++begin;
        ++to;
        // optional exponent sign
        if (begin != end
            && (ctype.narrow(*begin, ' ') == '-'
                || ctype.narrow(*begin, ' ') == '+')) {
            *to = ctype.narrow(*begin, ' ');
            ++to;
            ++begin;
        }
        char* start(to);
        for (; begin != end && to != toEnd
                 && ctype.is(bsl::ctype_base::digit, *begin);
             ++begin, ++to) {
            *to = ctype.narrow(*begin, ' ');
        }
        if (start == to) { // exponent needs to have at least one digit
            to = buffer;
        }
    }
    if (hasDigit) {
        *to = '\0';
        value = DecimalImpUtil::parse32(buffer);
    }
    else {
        err = bsl::ios_base::failbit;
    }
    return begin;
}
template <class CHARTYPE, class INPUTITERATOR>
typename DecimalNumGet<CHARTYPE, INPUTITERATOR>::iter_type
DecimalNumGet<CHARTYPE, INPUTITERATOR>::do_get(
                                           iter_type               begin,
                                           iter_type               end,
                                           bsl::ios_base&          str,
                                           bsl::ios_base::iostate& err,
                                           Decimal64&              value) const
{
    typedef bsl::ctype<CHARTYPE> Ctype;
    Ctype const& ctype(bsl::use_facet<Ctype>(str.getloc()));

    char        buffer[512];
    char*       to(buffer);
    char* const toEnd(buffer + (sizeof(buffer) - 1));
    CHARTYPE    separator(bsl::use_facet<bsl::numpunct<CHARTYPE> >(
                                                str.getloc()).thousands_sep());
    bool        hasDigit(false);

    // optional sign
    if (begin != end
        && (ctype.narrow(*begin, ' ') == '-'
            || ctype.narrow(*begin, ' ') == '+')) {
        *to = ctype.narrow(*begin, ' ');
        ++to;
        ++begin;
    }
    // spaces between sign and value
    begin = bsl::find_if(begin, end, NotIsSpace<CHARTYPE>(ctype));
    // non-fractional part
    for (; begin != end && to != toEnd
             && (ctype.is(bsl::ctype_base::digit, *begin)
             || *begin == separator);
         ++begin, ++to) {
        if (*begin != separator) {
            //-dk:TODO TBD store separators for later check
            hasDigit = true;
            *to = ctype.narrow(*begin, ' ');
        }
    }
    // fractional part
    if (begin != end && to != toEnd && ctype.narrow(*begin, ' ') == '.') {
        *to = '.';
        ++begin;
        ++to;
        for (; begin != end && to != toEnd
                 && ctype.is(bsl::ctype_base::digit, *begin);
             ++begin, ++to) {
            hasDigit = true;
            *to = ctype.narrow(*begin, ' ');
        }
    }
    // exponent (but not a stand-alone exponent
    if (hasDigit && begin != end && to != toEnd
        && ctype.narrow(ctype.tolower(*begin), ' ') == 'e') {
        *to = 'e';
        ++begin;
        ++to;
        // optional exponent sign
        if (begin != end
            && (ctype.narrow(*begin, ' ') == '-'
                || ctype.narrow(*begin, ' ') == '+')) {
            *to = ctype.narrow(*begin, ' ');
            ++to;
            ++begin;
        }
        char* start(to);
        for (; begin != end && to != toEnd
                 && ctype.is(bsl::ctype_base::digit, *begin);
             ++begin, ++to) {
            *to = ctype.narrow(*begin, ' ');
        }
        if (start == to) { // exponent needs to have at least one digit
            to = buffer;
        }
    }
    if (hasDigit) {
        *to = '\0';
        value = DecimalImpUtil::parse64(buffer);
    }
    else {
        err = bsl::ios_base::failbit;
    }
    return begin;
}
template <class CHARTYPE, class INPUTITERATOR>
typename DecimalNumGet<CHARTYPE, INPUTITERATOR>::iter_type
DecimalNumGet<CHARTYPE, INPUTITERATOR>::do_get(
                                           iter_type               begin,
                                           iter_type               end,
                                           bsl::ios_base&          str,
                                           bsl::ios_base::iostate& err,
                                           Decimal128&             value) const
{
    typedef bsl::ctype<CHARTYPE> Ctype;
    Ctype const& ctype(bsl::use_facet<Ctype>(str.getloc()));

    char        buffer[512];
    char*       to(buffer);
    char* const toEnd(buffer + (sizeof(buffer) - 1));
    CHARTYPE    separator(bsl::use_facet<bsl::numpunct<CHARTYPE> >(
                                                str.getloc()).thousands_sep());
    bool        hasDigit(false);

    // optional sign
    if (begin != end
        && (ctype.narrow(*begin, ' ') == '-'
            || ctype.narrow(*begin, ' ') == '+')) {
        *to = ctype.narrow(*begin, ' ');
        ++to;
        ++begin;
    }
    // spaces between sign and value
    begin = bsl::find_if(begin, end, NotIsSpace<CHARTYPE>(ctype));
    // non-fractional part
    for (; begin != end && to != toEnd
             && (ctype.is(bsl::ctype_base::digit, *begin)
             || *begin == separator);
         ++begin, ++to) {
        if (*begin != separator) {
            //-dk:TODO TBD store separators for later check
            hasDigit = true;
            *to = ctype.narrow(*begin, ' ');
        }
    }
    // fractional part
    if (begin != end && to != toEnd && ctype.narrow(*begin, ' ') == '.') {
        *to = '.';
        ++begin;
        ++to;
        for (; begin != end && to != toEnd
                 && ctype.is(bsl::ctype_base::digit, *begin);
             ++begin, ++to) {
            hasDigit = true;
            *to = ctype.narrow(*begin, ' ');
        }
    }
    // exponent (but not a stand-alone exponent)
    if (hasDigit && begin != end && to != toEnd
        && ctype.narrow(ctype.tolower(*begin), ' ') == 'e') {
        *to = 'e';
        ++begin;
        ++to;
        // optional exponent sign
        if (begin != end
            && (ctype.narrow(*begin, ' ') == '-'
                || ctype.narrow(*begin, ' ') == '+')) {
            *to = ctype.narrow(*begin, ' ');
            ++to;
            ++begin;
        }
        char* start(to);
        for (; begin != end && to != toEnd
                 && ctype.is(bsl::ctype_base::digit, *begin);
             ++begin, ++to) {
            *to = ctype.narrow(*begin, ' ');
        }
        if (start == to) { // exponent needs to have at least one digit
            to = buffer;
        }
    }
    if (hasDigit) {
        *to = '\0';
        value = DecimalImpUtil::parse128(buffer);
    }
    else {
        err = bsl::ios_base::failbit;
    }
    return begin;
}

                            // -------------------
                            // class DecimalNumPut
                            // -------------------

template <class CHARTYPE, class OUTPUTITERATOR>
bsl::locale::id DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::id;

#ifdef BSLS_PLATFORM_CMP_SUN
template <class CHARTYPE, class OUTPUTITERATOR>
bsl::locale::id& DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::__get_id() const
{
    return id;
}
#endif

template <class CHARTYPE, class OUTPUTITERATOR>
const DecimalNumPut<CHARTYPE, OUTPUTITERATOR>&
DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::object()
{
    static DecimalNumPut<CHARTYPE, OUTPUTITERATOR> rc;
    return rc;
}

template <class CHARTYPE, class OUTPUTITERATOR>
DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::DecimalNumPut(bsl::size_t refs)
    : bsl::locale::facet(refs)
{
}

template <class CHARTYPE, class OUTPUTITERATOR>
DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::~DecimalNumPut()
{
}

template <class CHARTYPE, class OUTPUTITERATOR>
typename DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::iter_type
DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::put(iter_type      out,
                                             bsl::ios_base& str,
                                             char_type      fill,
                                             Decimal32      value) const
{
    return this->do_put(out, str, fill, value);
}
template <class CHARTYPE, class OUTPUTITERATOR>
typename DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::iter_type
DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::put(iter_type      out,
                                             bsl::ios_base& str,
                                             char_type      fill,
                                             Decimal64      value) const
{
    return this->do_put(out, str, fill, value);
}
template <class CHARTYPE, class OUTPUTITERATOR>
typename DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::iter_type
DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::put(iter_type      out,
                                             bsl::ios_base& str,
                                             char_type      fill,
                                             Decimal128     value) const
{
    return this->do_put(out, str, fill, value);
}

template <class CHARTYPE, class OUTPUTITERATOR>
typename DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::iter_type
DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::do_put(iter_type      out,
                                                bsl::ios_base& ios_format,
                                                char_type      fill,
                                                Decimal32      value) const
{
    char  buffer[BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE];
    format(value.data(), buffer, sizeof(buffer));
    return doPutCommon(out, ios_format, fill, &buffer[0]);
}
template <class CHARTYPE, class OUTPUTITERATOR>
typename DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::iter_type
DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::do_put(iter_type      out,
                                                bsl::ios_base& ios_format,
                                                char_type      fill,
                                                Decimal64      value) const
{
    char  buffer[BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE];
    format(value.data(), buffer, sizeof(buffer));
    return doPutCommon(out, ios_format, fill, &buffer[0]);
}
template <class CHARTYPE, class OUTPUTITERATOR>
typename DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::iter_type
DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::do_put(iter_type      out,
                                                bsl::ios_base& ios_format,
                                                char_type      fill,
                                                Decimal128     value) const
{
    char  buffer[BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE];
    format(value.data(), buffer, sizeof(buffer));
    return doPutCommon(out, ios_format, fill, &buffer[0]);
}

                       // Explicit instantiations

template class DecimalNumPut<char, bsl::ostreambuf_iterator<char> >;
template class DecimalNumPut<wchar_t, bsl::ostreambuf_iterator<wchar_t> >;

}  // close package namespace

                     // Streaming operators implementations

                                  // Output

template <class CHARTYPE, class TRAITS>
bsl::basic_ostream<CHARTYPE, TRAITS>&
bdldfp::operator<<(bsl::basic_ostream<CHARTYPE, TRAITS>& stream,
                   Decimal32                             object)
{
    return print(stream, object);
}

template <class CHARTYPE, class TRAITS>
bsl::basic_ostream<CHARTYPE, TRAITS>&
bdldfp::operator<<(bsl::basic_ostream<CHARTYPE, TRAITS>& stream,
                   Decimal64                             object)
{
    return print(stream, object);
}

template <class CHARTYPE, class TRAITS>
bsl::basic_ostream<CHARTYPE, TRAITS>&
bdldfp::operator<<(bsl::basic_ostream<CHARTYPE, TRAITS>& stream,
                   Decimal128                            object)
{
    return print(stream, object);
}

                                  // Input

template <class CHARTYPE, class TRAITS>
bsl::basic_istream<CHARTYPE, TRAITS>&
bdldfp::operator>>(bsl::basic_istream<CHARTYPE, TRAITS>& stream,
                   Decimal32&                            object)
{
    return read(stream, object);
}

template <class CHARTYPE, class TRAITS>
bsl::basic_istream<CHARTYPE, TRAITS>&
bdldfp::operator>>(bsl::basic_istream<CHARTYPE, TRAITS>& stream,
                   Decimal64&                            object)
{
    return read(stream, object);
}

template <class CHARTYPE, class TRAITS>
bsl::basic_istream<CHARTYPE, TRAITS>&
bdldfp::operator>>(bsl::basic_istream<CHARTYPE, TRAITS>& stream,
                   Decimal128&                           object)
{
    return read(stream, object);
}

                // Streaming operators explicit instantiations

                                // Decimal32

template
bsl::basic_istream<char>&
bdldfp::operator>> <char, bsl::char_traits<char> >(
                                        bsl::basic_istream<char>& in,
                                        bdldfp::Decimal32&        value);
template
bsl::basic_istream<wchar_t>&
bdldfp::operator>> <wchar_t, bsl::char_traits<wchar_t> >(
                                        bsl::basic_istream<wchar_t>& in,
                                        bdldfp::Decimal32&           value);

template
bsl::basic_ostream<char>&
bdldfp::operator<< <char, bsl::char_traits<char> >(
                                         bsl::basic_ostream<char>& out,
                                         bdldfp::Decimal32         value);
template
bsl::basic_ostream<wchar_t>&
bdldfp::operator<< <wchar_t, bsl::char_traits<wchar_t> >(
                                         bsl::basic_ostream<wchar_t>& out,
                                         bdldfp::Decimal32            value);

                                // Decimal64

template
bsl::basic_istream<char>&
bdldfp::operator>> <char, bsl::char_traits<char> >(
                                        bsl::basic_istream<char>& in,
                                        bdldfp::Decimal64&        value);
template
bsl::basic_istream<wchar_t>&
bdldfp::operator>> <wchar_t, bsl::char_traits<wchar_t> >(
                                        bsl::basic_istream<wchar_t>& in,
                                        bdldfp::Decimal64&           value);

template
bsl::basic_ostream<char>&
bdldfp::operator<< <char, bsl::char_traits<char> >(
                                         bsl::basic_ostream<char>& out,
                                         bdldfp::Decimal64         value);
template
bsl::basic_ostream<wchar_t>&
bdldfp::operator<< <wchar_t, bsl::char_traits<wchar_t> >(
                                         bsl::basic_ostream<wchar_t>& out,
                                         bdldfp::Decimal64            value);

                                // Decimal128

template
bsl::basic_istream<char>&
bdldfp::operator>> <char, bsl::char_traits<char> >(
                                       bsl::basic_istream<char>& in,
                                       bdldfp::Decimal128&       value);
template
bsl::basic_istream<wchar_t>&
bdldfp::operator>> <wchar_t, bsl::char_traits<wchar_t> >(
                                       bsl::basic_istream<wchar_t>& in,
                                       bdldfp::Decimal128&          value);

template
bsl::basic_ostream<char>&
bdldfp::operator<< <char, bsl::char_traits<char> >(
                                        bsl::basic_ostream<char>& out,
                                        bdldfp::Decimal128        value);
template
bsl::basic_ostream<wchar_t>&
bdldfp::operator<< <wchar_t, bsl::char_traits<wchar_t> >(
                                        bsl::basic_ostream<wchar_t>& out,
                                        bdldfp::Decimal128           value);

}  // close enterprise namespace
                  // std::numeric_limits<> specializations

#if defined(BSL_OVERRIDES_STD) && defined(std)
#   undef std
#   define BDLDFP_DECIMAL_RESTORE_STD
#endif

            // --------------------------------------------------
            // std::numeric_limits<BloombergLP::bdldfp::Decimal32>
            // --------------------------------------------------

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::min()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 1e-95df;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv, "1e-95", BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::makeDecimalRaw32(1, -95);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::max()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 9.999999e96df;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv, "9.999999e96", BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::parse32("9.999999e96");
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::epsilon()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 1e-6df;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv, "1e-6", BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::makeDecimalRaw32(1, -6);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::round_error()
    BSLS_NOTHROW_SPEC
{ // TBD TODO - determine the real value from the round mode!
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 1.0df;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv, "1.0", BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::makeDecimalRaw32(1, 0);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::infinity()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return HUGE_VAL_D32;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv, "INF", BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::parse32("INF");
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::quiet_NaN()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return BDLDFP_DECIMALPLATFORM_C99_QNAN32;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv, "NaN", BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::parse32("NaN");
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal32
   std::numeric_limits<BloombergLP::bdldfp::Decimal32>::signaling_NaN()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return BDLDFP_DECIMALPLATFORM_C99_SNAN32;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv, "sNaN", BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::parse32("sNaN");
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::denorm_min()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 0.000001E-95df;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv,
                        "0.000001E-95",
                        BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::parse32("0.000001E-95");
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

            // --------------------------------------------------
            // std::numeric_limits<BloombergLP::bdldfp::Decimal64>
            // --------------------------------------------------

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::min()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 1e-383dd;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv, "1e-383", BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::makeDecimalRaw64(1, -383);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::max()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 9.999999999999999e384dd;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv,
                        "9.999999999999999e384",
                        BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::parse64(
                                                      "9.999999999999999e384");
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::epsilon()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 1e-15dd;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv, "1e-15", BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::parse64("1e-15");
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::round_error()
    BSLS_NOTHROW_SPEC
{  // TBD TODO - determine the real value from the round mode!
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 1.0dd;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv, "1.0", BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::makeDecimalRaw64(1, 0);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::infinity()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return HUGE_VAL_D64;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv, "INF", BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::parse64("INF");
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::quiet_NaN()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return BDLDFP_DECIMALPLATFORM_C99_QNAN64;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv, "qNaN", BloombergLP::bdldfp::getContext());
    BSLS_ASSERT(reinterpret_cast<const unsigned long long &>(rv) != 0);
    decDouble rv2 = rv;
    BSLS_ASSERT(reinterpret_cast<const unsigned long long &>(rv2) != 0);
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::parse64("NaN");
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal64
   std::numeric_limits<BloombergLP::bdldfp::Decimal64>::signaling_NaN()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return BDLDFP_DECIMALPLATFORM_C99_SNAN64;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv, "sNaN", BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::parse64("sNaN");
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::denorm_min()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 0.000000000000001e-383dd;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv,
                        "0.000000000000001e-383",
                        BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::parse64(
                                                     "0.000000000000001e-383");
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

            // ---------------------------------------------------
            // std::numeric_limits<BloombergLP::bdldfp::Decimal128>
            // ---------------------------------------------------

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::min()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 1e-6143dl;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv, "1e-6143", BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::makeDecimalRaw128(1, -6143);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::max()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 9.999999999999999999999999999999999e6144dl;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv,
                      "9.999999999999999999999999999999999e6144",
                      BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::parse128(
                                   "9.999999999999999999999999999999999e6144");
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::epsilon()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 1e-33dl;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv, "1e-33", BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::makeDecimalRaw128(1, -33);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::round_error()
    BSLS_NOTHROW_SPEC
{  // TBD TODO - determine the real value from the round mode setting!
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 1.0dl;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv, "1.0", BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::makeDecimalRaw128(1, 0);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::infinity()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return HUGE_VAL_D128;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv, "INF", BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::parse128("INF");
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::quiet_NaN()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return BDLDFP_DECIMALPLATFORM_C99_QNAN128;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv, "NaN", BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::parse128("NaN");
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal128
  std::numeric_limits<BloombergLP::bdldfp::Decimal128>::signaling_NaN()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return BDLDFP_DECIMALPLATFORM_C99_QNAN128;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv, "sNaN", BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::parse128("sNaN");
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::denorm_min()
    BSLS_NOTHROW_SPEC
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 0.000000000000000000000000000000001e-6143dl;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv,
                      "0.000000000000000000000000000000001e-6143",
                      BloombergLP::bdldfp::getContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return BloombergLP::bdldfp::DecimalImpUtil::parse128(
                                  "0.000000000000000000000000000000001e-6143");
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}


// Microsoft has non-standard behavior

#ifndef BSLS_PLATFORM_CMP_MSVC

      // Definitions of const static data memberss of numeric_limits<>

const bool
     std::numeric_limits<
        BloombergLP::bdldfp::Decimal_StandardNamespaceCanary>::is_specialized;

const bool std::numeric_limits<BloombergLP::bdldfp::Decimal32>::is_specialized;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal32>::digits;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal32>::digits10;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal32>::max_digits10;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal32>::is_signed;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal32>::is_integer;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal32>::is_exact;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal32>::radix;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal32>::min_exponent;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal32>::min_exponent10;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal32>::max_exponent;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal32>::max_exponent10;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal32>::has_infinity;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal32>::has_quiet_NaN;
const bool
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::has_signaling_NaN;
const std::float_denorm_style
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::has_denorm;
const bool
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::has_denorm_loss;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal32>::is_iec559;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal32>::is_bounded;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal32>::is_modulo;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal32>::traps;
const bool
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::tinyness_before;
const std::float_round_style
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::round_style;

const bool std::numeric_limits<BloombergLP::bdldfp::Decimal64>::is_specialized;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal64>::digits;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal64>::digits10;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal64>::max_digits10;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal64>::is_signed;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal64>::is_integer;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal64>::is_exact;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal64>::radix;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal64>::min_exponent;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal64>::min_exponent10;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal64>::max_exponent;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal64>::max_exponent10;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal64>::has_infinity;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal64>::has_quiet_NaN;
const bool
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::has_signaling_NaN;
const std::float_denorm_style
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::has_denorm;
const bool
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::has_denorm_loss;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal64>::is_iec559;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal64>::is_bounded;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal64>::is_modulo;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal64>::traps;
const bool
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::tinyness_before;
const std::float_round_style
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::round_style;

const bool
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::is_specialized;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal128>::digits;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal128>::digits10;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal128>::max_digits10;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal128>::is_signed;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal128>::is_integer;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal128>::is_exact;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal128>::radix;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal128>::min_exponent;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal128>::min_exponent10;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal128>::max_exponent;
const int std::numeric_limits<BloombergLP::bdldfp::Decimal128>::max_exponent10;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal128>::has_infinity;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal128>::has_quiet_NaN;
const bool
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::has_signaling_NaN;
const std::float_denorm_style
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::has_denorm;
const bool
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::has_denorm_loss;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal128>::is_iec559;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal128>::is_bounded;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal128>::is_modulo;
const bool std::numeric_limits<BloombergLP::bdldfp::Decimal128>::traps;
const bool
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::tinyness_before;
const std::float_round_style
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::round_style;

#endif // Microsoft is non-standard

#if defined(BDLDFP_DECIMAL_RESTORE_STD)
#   define std bsl
#   undef BDLDFP_DECIMAL_RESTORE_STD
#endif

// The endif is for the Sun compiler workaround
#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
