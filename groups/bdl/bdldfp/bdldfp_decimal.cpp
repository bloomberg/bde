// bdldfp_decimal.cpp                                                 -*-C++-*-
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
#include <bsl_cstring.h>

#if BDLDFP_DECIMALPLATFORM_C99_TR
#include <math.h>
#include <stdio.h>
#endif

namespace BloombergLP {
namespace bdldfp {

namespace {
                    // ===============
                    // class BufferBuf
                    // ===============

template <int Size>
class BufferBuf : public bsl::streambuf {
    // A static (capacity) stream buffer helper

    char d_buf[Size + 1];  // Text plus closing NUL character

  public:
    // CREATORS
    BufferBuf();
        // Create an empty 'BufferBuf'.

    // MANIPULATORS
    void reset();
        // Clear this buffer (make it empty).

    const char *str();
        // Return a pointer to a non-modifiable, NUL-terminated string of
        // characters that is the content of this buffer.
};
                    // ---------------
                    // class BufferBuf
                    // ---------------

template <int Size>
BufferBuf<Size>::BufferBuf()
{
    reset();
}

template <int Size>
void BufferBuf<Size>::reset()
{
    this->setp(this->d_buf, this->d_buf + Size);
}

template <int Size>
const char *BufferBuf<Size>::str()
{
    *this->pptr() = 0;
    return this->pbase();
}

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

#if BDLDFP_DECIMALPLATFORM_C99_TR

#  ifndef  __STDC_WANT_DEC_FP__
#    error __STDC_WANT_DEC_FP__ must be defined on the command line!
     char die[sizeof(Decimal_Assert)];     // if '#error' unsupported
#  endif


        // Implementation when we have C DecFP support only (no C++)

                   // C99 stream-output converters

static
char *format(const DecimalImplUtil::ValueType32 *value, char *buffer, int n)
{
    // TBD TODO - printf is locale dependent!!!

    BSLS_ASSERT(value);
    BSLS_ASSERT(buffer);

    snprintf(buffer, n, "%#.7HG", *value);
    return buffer;
}

static
char *format(const DecimalImplUtil::ValueType64 *value, char *buffer, int n)
{
    // TBD TODO - printf is locale dependent!!!

    BSLS_ASSERT(value);
    BSLS_ASSERT(buffer);

    snprintf(buffer, n, "%#.16DG", *value);
    return buffer;
}

static
char *format(const DecimalImplUtil::ValueType128 *value, char *buffer, int n)
{
    // TBD TODO - printf is locale dependent!!!

    BSLS_ASSERT(value);
    BSLS_ASSERT(buffer);

    snprintf(buffer, n, "%#.34DDG", *value);
    return buffer;
}

#elif BDLDFP_DECIMALPLATFORM_DECNUMBER

     // Implementation based on the decNumber library (no C or C++ support)

              // Implementation specific helper functions

static
char *format(const DecimalImplUtil::ValueType32 *value, char *buffer, int)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(buffer);

    return decSingleToString(value, buffer);
}

static
char *format(const DecimalImplUtil::ValueType64 *value, char *buffer, int)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(buffer);

    return decDoubleToString(value, buffer);
}

static
char *format(const DecimalImplUtil::ValueType128 *value, char *buffer, int)
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

#endif // elif BDLDFP_DECIMALPLATFORM_DECNUMBER

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
    const int size = bsl::strlen(buffer);
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


                          // --------------------
                          // class Decimal_Type32
                          // --------------------

// CREATORS

Decimal_Type32::Decimal_Type32()
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = 0e0df;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    decSingleZero(&this->d_value);
#endif
}

Decimal_Type32::Decimal_Type32(DecimalImplUtil::ValueType32 value)
{
    this->d_value = value;
}

Decimal_Type32::Decimal_Type32(Decimal64 other)
{
    d_value = DecimalImplUtil::convertToDecimal32(*other.data());
}

Decimal_Type32::Decimal_Type32(float other)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = other;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(7);
    out << other;
    decSingleFromString(&this->d_value, bb.str(), getContext());
#endif
}

Decimal_Type32::Decimal_Type32(double other)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = other;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(7);
    out << other;
    decSingleFromString(&this->d_value, bb.str(), getContext());
#endif
}

Decimal_Type32::Decimal_Type32(long double other)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = other;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(7);
    out << other;
    decSingleFromString(&this->d_value, bb.str(), getContext());
#endif
}

Decimal_Type32::Decimal_Type32(int other)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = other;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << other;
    decSingleFromString(&this->d_value, bb.str(), getContext());
#endif
}

Decimal_Type32::Decimal_Type32(unsigned int other)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = other;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << other;
    decSingleFromString(&this->d_value, bb.str(), getContext());
#endif
}

Decimal_Type32::Decimal_Type32(long int other)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = other;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << other;
    decSingleFromString(&this->d_value, bb.str(), getContext());
#endif
}

Decimal_Type32::Decimal_Type32(unsigned long int other)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = other;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << other;
    decSingleFromString(&this->d_value, bb.str(), getContext());
#endif
}

Decimal_Type32::Decimal_Type32(long long int other)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = other;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << other;
    decSingleFromString(&this->d_value, bb.str(), getContext());
#endif
}

Decimal_Type32::Decimal_Type32(unsigned long long int other)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = other;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << other;
    decSingleFromString(&this->d_value, bb.str(), getContext());
#endif
}

// MANIPULATORS

DecimalImplUtil::ValueType32 *Decimal_Type32::data()
{
    return &d_value;
}

// ACCESSORS

const DecimalImplUtil::ValueType32 *Decimal_Type32::data() const
{
    return &d_value;
}

DecimalImplUtil::ValueType32 Decimal_Type32::value() const
{
    return d_value;
}

// FREE OPERATORS

                           // Unary operator+/-

Decimal32 operator+(Decimal32 value)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return +value.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    if (decSingleClass(value.data()) == DEC_CLASS_NEG_ZERO) {
        decSingle rv;
        decSingleCopyNegate(&rv, value.data());
        return rv;                                                    // RETURN
    }
    return value;
#endif
}

Decimal32 operator-(Decimal32 value)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return -value.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    Decimal32 rv;
    decSingleCopyNegate(rv.data(), value.data());
    return rv;
#endif
}

bool operator==(Decimal32 lhs, Decimal32 rhs)
{
    return DecimalImplUtil::equals(lhs.value(), rhs.value());
}

bool operator!=(Decimal32 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() != rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return !(lhs == rhs);
#endif
}

bool operator<(Decimal32 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() < rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) < Decimal64(rhs);
#endif
}

bool operator>(Decimal32 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() > rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return rhs < lhs;
#endif
}

bool operator<=(Decimal32 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() <= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return !(rhs < lhs);
#endif
}

bool operator>=(Decimal32 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() >= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return !(lhs < rhs);
#endif
}
                           // --------------------
                           // class Decimal_Type64
                           // --------------------

// CREATORS

Decimal_Type64::Decimal_Type64()
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = 0e0dd;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    decDoubleZero(&this->d_value);
#endif
}

Decimal_Type64::Decimal_Type64(DecimalImplUtil::ValueType64 value)
{
    this->d_value = value;
}

Decimal_Type64::Decimal_Type64(Decimal32 other)
{
    d_value = DecimalImplUtil::convertToDecimal64(*other.data());
}

Decimal_Type64::Decimal_Type64(Decimal128 other)
{
    d_value = DecimalImplUtil::convertToDecimal64(*other.data());
}

Decimal_Type64::Decimal_Type64(float other)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = other;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(16);
    out << other;
    decDoubleFromString(&this->d_value, bb.str(), getContext());
#endif
}
Decimal_Type64::Decimal_Type64(double other)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = other;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(16);
    out << other;
    decDoubleFromString(&this->d_value, bb.str(), getContext());
#endif
}
Decimal_Type64::Decimal_Type64(long double other)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = other;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(16);
    out << other;
    decDoubleFromString(&this->d_value, bb.str(), getContext());
#endif
}

Decimal_Type64::Decimal_Type64(int other)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = other;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << other;
    decDoubleFromString(&this->d_value, bb.str(), getContext());
#endif
}
Decimal_Type64::Decimal_Type64(unsigned int other)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = other;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << other;
    decDoubleFromString(&this->d_value, bb.str(), getContext());
#endif
}
Decimal_Type64::Decimal_Type64(long other)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = other;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << other;
    decDoubleFromString(&this->d_value, bb.str(), getContext());
#endif
}
Decimal_Type64::Decimal_Type64(unsigned long other)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = other;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << other;
    decDoubleFromString(&this->d_value, bb.str(), getContext());
#endif
}
Decimal_Type64::Decimal_Type64(long long other)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = other;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << other;
    decDoubleFromString(&this->d_value, bb.str(), getContext());
#endif
}
Decimal_Type64::Decimal_Type64(unsigned long long other)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = other;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << other;
    decDoubleFromString(&this->d_value, bb.str(), getContext());
#endif
}

// MANIPULATORS

DecimalImplUtil::ValueType64 *Decimal_Type64::data()
{
    return &d_value;
}


Decimal64& Decimal64::operator+=(Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value += rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    decDoubleAdd(&this->d_value, &this->d_value, rhs.data(), getContext());
#endif
    return *this;
}

Decimal64& Decimal64::operator-=(Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value -= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    decDoubleSubtract(&this->d_value, &this->d_value, rhs.data(),
                      getContext());
#endif
    return *this;
}

Decimal64& Decimal64::operator*=(Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value *= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    decDoubleMultiply(&this->d_value, &this->d_value, rhs.data(),
                      getContext());
#endif
    return *this;
}

Decimal64& Decimal64::operator/=(Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value /= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    decDoubleDivide(&this->d_value, &this->d_value, rhs.data(),
                    getContext());
#endif
    return *this;
}

Decimal64& Decimal64::operator+=(Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
     d_value += rhs.value();
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator+=(Decimal64(rhs));
#endif
}

Decimal64& Decimal64::operator+=(Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value += rhs.value();
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    Decimal128 me(*this);
    return *this = Decimal64(me+=rhs);
#endif
}

Decimal64& Decimal64::operator+=(int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value += rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator+=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator+=(unsigned int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value += rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator+=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator+=(long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value += rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator+=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator+=(unsigned long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value += rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator+=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator+=(long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value += rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator+=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator+=(unsigned long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value += rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator+=(Decimal64(rhs));
#endif
}

Decimal64& Decimal64::operator-=(Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value -= rhs.value();
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator-=(Decimal64(rhs));
#endif
}

Decimal64& Decimal64::operator-=(Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value -= rhs.value();
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    Decimal128 me(*this);
    return *this = Decimal64(me-=rhs);
#endif
}

Decimal64& Decimal64::operator-=(int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value -= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator-=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator-=(unsigned int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value -= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator-=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator-=(long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value -= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator-=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator-=(unsigned long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value -= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator-=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator-=(long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value -= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator-=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator-=(unsigned long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value -= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator-=(Decimal64(rhs));
#endif
}

Decimal64& Decimal64::operator*=(Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value *= rhs.value();
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator*=(Decimal64(rhs));
#endif
}

Decimal64& Decimal64::operator*=(Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value *= rhs.value();
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    Decimal128 me(*this);
    return *this = Decimal64(me*=rhs);
#endif
}

Decimal64& Decimal64::operator*=(int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value *= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator*=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator*=(unsigned int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value *= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator*=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator*=(long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value *= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator*=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator*=(unsigned long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value *= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator*=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator*=(long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value *= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator*=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator*=(unsigned long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value *= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator*=(Decimal64(rhs));
#endif
}

Decimal64& Decimal64::operator/=(Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value /= rhs.value();
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator/=(Decimal64(rhs));
#endif
}

Decimal64& Decimal64::operator/=(Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value /= rhs.value();
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    Decimal128 me(*this);
    return *this = Decimal64(me/=rhs);
#endif
}

Decimal64& Decimal64::operator/=(int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value /= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator/=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator/=(unsigned int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value /= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator/=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator/=(long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value /= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator/=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator/=(unsigned long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value /= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator/=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator/=(long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value /= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator/=(Decimal64(rhs));
#endif
}
Decimal64& Decimal64::operator/=(unsigned long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value /= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator/=(Decimal64(rhs));
#endif
}

#if BDLDFP_DECIMALPLATFORM_DECNUMBER
static Decimal64 one64(1);
#endif

Decimal64& Decimal64::operator++()
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    ++d_value;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator+=(one64);
#endif
}

Decimal64& Decimal64::operator--()
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    --d_value;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator-=(one64);
#endif
}

// ACCESSORS

const DecimalImplUtil::ValueType64 *Decimal_Type64::data() const
{
    return &d_value;
}

DecimalImplUtil::ValueType64 Decimal_Type64::value() const
{
    return d_value;
}

// FREE OPERATORS

                           // Unary operator+/-

Decimal64 operator+(Decimal64 value)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return +value.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    if (decDoubleClass(value.data()) == DEC_CLASS_NEG_ZERO) {
        decDouble rv;
        decDoubleCopyNegate(&rv, value.data());
        return rv;                                                    // RETURN
    }
    return value;
#endif
}

Decimal64 operator-(Decimal64 value)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return -value.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    decDouble rv;
    decDoubleCopyNegate(&rv, value.data());
    return rv;
#endif
}

                          // operator++/--(int)

Decimal64 operator++(Decimal64& value, int)
{
    Decimal64 rc(value);
    value.operator++();
    return rc;
}

Decimal64 operator--(Decimal64& value, int)
{
    Decimal64 rc(value);
    value.operator--();
    return rc;
}

                            // operator+

Decimal64 operator+(Decimal64 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs += rhs;
#endif
}
Decimal64 operator+(Decimal32 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) += rhs;
#endif
}
Decimal64 operator+(Decimal64 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs += Decimal64(rhs);
#endif
}
Decimal64 operator+(int lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) += rhs;
#endif
}
Decimal64 operator+(Decimal64 lhs, int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs += Decimal64(rhs);
#endif
}
Decimal64 operator+(unsigned int lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) += rhs;
#endif
}
Decimal64 operator+(Decimal64 lhs, unsigned int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs += Decimal64(rhs);
#endif
}
Decimal64 operator+(long lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) += rhs;
#endif
}
Decimal64 operator+(Decimal64 lhs, long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs += Decimal64(rhs);
#endif
}
Decimal64 operator+(unsigned long lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) += rhs;
#endif
}
Decimal64 operator+(Decimal64 lhs, unsigned long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs += Decimal64(rhs);
#endif
}
Decimal64 operator+(long long lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) += rhs;
#endif
}
Decimal64 operator+(Decimal64 lhs, long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs += Decimal64(rhs);
#endif
}
Decimal64 operator+(unsigned long long lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) += rhs;
#endif
}
Decimal64 operator+(Decimal64 lhs, unsigned long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs += Decimal64(rhs);
#endif
}

                            // operator-

Decimal64 operator-(Decimal64 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs -= rhs;
#endif
}
Decimal64 operator-(Decimal32 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) -= rhs;
#endif
}
Decimal64 operator-(Decimal64 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs -= Decimal64(rhs);
#endif
}
Decimal64 operator-(int lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) -= rhs;
#endif
}
Decimal64 operator-(Decimal64 lhs, int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs -= Decimal64(rhs);
#endif
}
Decimal64 operator-(unsigned int lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) -= rhs;
#endif
}
Decimal64 operator-(Decimal64 lhs, unsigned int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs -= Decimal64(rhs);
#endif
}
Decimal64 operator-(long lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) -= rhs;
#endif
}
Decimal64 operator-(Decimal64 lhs, long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs -= Decimal64(rhs);
#endif
}
Decimal64 operator-(unsigned long lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) -= rhs;
#endif
}
Decimal64 operator-(Decimal64 lhs, unsigned long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs -= Decimal64(rhs);
#endif
}
Decimal64 operator-(long long lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) -= rhs;
#endif
}
Decimal64 operator-(Decimal64 lhs, long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs -= Decimal64(rhs);
#endif
}
Decimal64 operator-(unsigned long long lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) -= rhs;
#endif
}
Decimal64 operator-(Decimal64 lhs, unsigned long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs -= Decimal64(rhs);
#endif
}

                            // operator*

Decimal64 operator*(Decimal64 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs *= rhs;
#endif
}
Decimal64 operator*(Decimal32 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) *= rhs;
#endif
}
Decimal64 operator*(Decimal64 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs *= Decimal64(rhs);
#endif
}
Decimal64 operator*(int lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) *= rhs;
#endif
}
Decimal64 operator*(Decimal64 lhs, int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs *= Decimal64(rhs);
#endif
}
Decimal64 operator*(unsigned int lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) *= rhs;
#endif
}
Decimal64 operator*(Decimal64 lhs, unsigned int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs *= Decimal64(rhs);
#endif
}
Decimal64 operator*(long lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) *= rhs;
#endif
}
Decimal64 operator*(Decimal64 lhs, long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs *= Decimal64(rhs);
#endif
}
Decimal64 operator*(unsigned long lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) *= rhs;
#endif
}
Decimal64 operator*(Decimal64 lhs, unsigned long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs *= Decimal64(rhs);
#endif
}
Decimal64 operator*(long long lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) *= rhs;
#endif
}
Decimal64 operator*(Decimal64 lhs, long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs *= Decimal64(rhs);
#endif
}
Decimal64 operator*(unsigned long long lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) *= rhs;
#endif
}
Decimal64 operator*(Decimal64 lhs, unsigned long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs *= Decimal64(rhs);
#endif
}

                            // operator/

Decimal64 operator/(Decimal64 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs /= rhs;
#endif
}
Decimal64 operator/(Decimal32 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) /= rhs;
#endif
}
Decimal64 operator/(Decimal64 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs /= Decimal64(rhs);
#endif
}
Decimal64 operator/(int lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) /= rhs;
#endif
}
Decimal64 operator/(Decimal64 lhs, int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs /= Decimal64(rhs);
#endif
}
Decimal64 operator/(unsigned int lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) /= rhs;
#endif
}
Decimal64 operator/(Decimal64 lhs, unsigned int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs /= Decimal64(rhs);
#endif
}
Decimal64 operator/(long lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) /= rhs;
#endif
}
Decimal64 operator/(Decimal64 lhs, long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs /= Decimal64(rhs);
#endif
}
Decimal64 operator/(unsigned long lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) /= rhs;
#endif
}
Decimal64 operator/(Decimal64 lhs, unsigned long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs /= Decimal64(rhs);
#endif
}
Decimal64 operator/(long long lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) /= rhs;
#endif
}
Decimal64 operator/(Decimal64 lhs, long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs /= Decimal64(rhs);
#endif
}
Decimal64 operator/(unsigned long long lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) /= rhs;
#endif
}
Decimal64 operator/(Decimal64 lhs, unsigned long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs /= Decimal64(rhs);
#endif
}

                         // Relational operators

                         // Homogeneous operators

bool operator==(Decimal64 lhs, Decimal64 rhs)
{
    return DecimalImplUtil::equals(lhs.value(), rhs.value());
}

bool operator!=(Decimal64 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() != rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return !(lhs == rhs);
#endif
}

bool operator<(Decimal64 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() < rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    decDouble result;
    decDoubleCompare(&result, lhs.data(), rhs.data(), getContext());
    return decDoubleIsNegative(&result);
#endif
}

bool operator>(Decimal64 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() > rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return rhs < lhs;
#endif
}

bool operator<=(Decimal64 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() <= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return !(rhs < lhs);
#endif
}

bool operator>=(Decimal64 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() >= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return !(lhs < rhs);
#endif
}

                             // Heterogeneous ==

bool operator==(Decimal32 lhs, Decimal64 rhs)
{
    return DecimalImplUtil::equals(lhs.value(), rhs.value());
}

bool operator==(Decimal64 lhs, Decimal32 rhs)
{
    return DecimalImplUtil::equals(lhs.value(), rhs.value());
}

                             // Heterogeneous !=

bool operator!=(Decimal32 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() != rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) != rhs;
#endif
}
bool operator!=(Decimal64 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() != rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs != Decimal64(rhs);
#endif
}

                             // Heterogeneous <

bool operator<(Decimal32 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() < rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) < rhs;
#endif
}
bool operator<(Decimal64 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() < rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs < Decimal64(rhs);
#endif
}

                             // Heterogeneous <=

bool operator<=(Decimal32 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() <= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) <= rhs;
#endif
}
bool operator<=(Decimal64 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() <= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs <= Decimal64(rhs);
#endif
}

                             // Heterogeneous >

bool operator>(Decimal32 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() > rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) > rhs;
#endif
}
bool operator>(Decimal64 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() > rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs > Decimal64(rhs);
#endif
}

                             // Heterogeneous >=

bool operator>=(Decimal32 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() >= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal64(lhs) >= rhs;
#endif
}
bool operator>=(Decimal64 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() >= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs >= Decimal64(rhs);
#endif
}
                          // ---------------------
                          // class Decimal_Type128
                          // ---------------------

// CREATORS

Decimal_Type128::Decimal_Type128()
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = 0e0dl;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    decQuadZero(&this->d_value);
#endif
}

Decimal_Type128::Decimal_Type128(DecimalImplUtil::ValueType128 value)
{
    this->d_value = value;
}

Decimal_Type128::Decimal_Type128(Decimal32 value)
{
    d_value = DecimalImplUtil::convertToDecimal128(*value.data());
}

Decimal_Type128::Decimal_Type128(Decimal64 value)
{
    d_value = DecimalImplUtil::convertToDecimal128(*value.data());
}

Decimal_Type128::Decimal_Type128(float value)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = value;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(34);
    out << value;
    decQuadFromString(&this->d_value, bb.str(), getContext());
#endif
}
Decimal_Type128::Decimal_Type128(double value)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = value;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(34);
    out << value;
    decQuadFromString(&this->d_value, bb.str(), getContext());
#endif
}
Decimal_Type128::Decimal_Type128(long double value)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = value;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(34);
    out << value;
    decQuadFromString(&this->d_value, bb.str(), getContext());
#endif
}

Decimal_Type128::Decimal_Type128(int value)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = value;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(34);
    out << value;
    decQuadFromString(&this->d_value, bb.str(), getContext());
#endif
}
Decimal_Type128::Decimal_Type128(unsigned int value)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = value;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(34);
    out << value;
    decQuadFromString(&this->d_value, bb.str(), getContext());
#endif
}
Decimal_Type128::Decimal_Type128(long value)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = value;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(34);
    out << value;
    decQuadFromString(&this->d_value, bb.str(), getContext());
#endif
}
Decimal_Type128::Decimal_Type128(unsigned long value)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = value;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(34);
    out << value;
    decQuadFromString(&this->d_value, bb.str(), getContext());
#endif
}
Decimal_Type128::Decimal_Type128(long long value)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = value;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(34);
    out << value;
    decQuadFromString(&this->d_value, bb.str(), getContext());
#endif
}
Decimal_Type128::Decimal_Type128(unsigned long long value)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value = value;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(34);
    out << value;
    decQuadFromString(&this->d_value, bb.str(), getContext());
#endif
}

// MANIPULATORS

                             // Unary operator+/-

Decimal128 operator+(Decimal128 value)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return +value.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    if (decQuadClass(value.data()) == DEC_CLASS_NEG_ZERO) {
        decQuad rv;
        decQuadCopyNegate(&rv, value.data());
        return rv;                                                    // RETURN
    }
    return value;
#endif
}

Decimal128 operator-(Decimal128 value)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return -value.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    decQuad rv;
    decQuadCopyNegate(&rv, value.data());
    return rv;
#endif
}

#if BDLDFP_DECIMALPLATFORM_DECNUMBER
static Decimal128 one128(1);
#endif

Decimal128& Decimal128::operator++()
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    ++d_value;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator+=(one128);
#endif
}

Decimal128& Decimal128::operator--()
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    --d_value;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator-=(one128);
#endif
}

Decimal128& Decimal128::operator+=(Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value += rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    decQuadAdd(&this->d_value, &this->d_value, rhs.data(), getContext());
#endif
    return *this;
}

Decimal128& Decimal128::operator-=(Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value -= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    decQuadSubtract(&this->d_value, &this->d_value, rhs.data(),
                    getContext());
#endif
    return *this;
}

Decimal128& Decimal128::operator*=(Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value *= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    decQuadMultiply(&this->d_value, &this->d_value, rhs.data(),
                    getContext());
#endif
    return *this;
}

Decimal128& Decimal128::operator/=(Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value /= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    decQuadDivide(&this->d_value, &this->d_value, rhs.data(), getContext());
#endif
    return *this;
}

                        // Heterogeneous operator+=

Decimal128& Decimal128::operator+=(Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value += rhs.value();
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator+=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator+=(Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value += rhs.value();
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator+=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator+=(int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value += rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator+=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator+=(unsigned int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value += rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator+=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator+=(long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value += rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator+=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator+=(unsigned long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value += rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator+=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator+=(long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value += rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator+=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator+=(unsigned long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value += rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator+=(Decimal128(rhs));
#endif
}

                        // Heterogeneous operator-=

Decimal128& Decimal128::operator-=(Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value -= rhs.value();
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator-=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator-=(Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value -= rhs.value();
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator-=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator-=(int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value -= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator-=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator-=(unsigned int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value -= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator-=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator-=(long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value -= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator-=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator-=(unsigned long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value -= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator-=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator-=(long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value -= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator-=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator-=(unsigned long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value -= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator-=(Decimal128(rhs));
#endif
}

                        // Heterogeneous operator*=

Decimal128& Decimal128::operator*=(Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value *= rhs.value();
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator*=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator*=(Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value *= rhs.value();
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator*=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator*=(int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value *= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator*=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator*=(unsigned int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value *= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator*=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator*=(long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value *= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator*=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator*=(unsigned long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value *= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator*=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator*=(long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value *= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator*=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator*=(unsigned long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value *= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator*=(Decimal128(rhs));
#endif
}

                        // Heterogeneous operator/=

Decimal128& Decimal128::operator/=(Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value /= rhs.value();
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator/=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator/=(Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value /= rhs.value();
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator/=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator/=(int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value /= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator/=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator/=(unsigned int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value /= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator/=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator/=(long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value /= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator/=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator/=(unsigned long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value /= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator/=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator/=(long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value /= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator/=(Decimal128(rhs));
#endif
}
Decimal128& Decimal128::operator/=(unsigned long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    d_value /= rhs;
    return *this;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return this->operator/=(Decimal128(rhs));
#endif
}

// MANIPULATORS

DecimalImplUtil::ValueType128 *Decimal_Type128::data()
{
    return &d_value;
}

const DecimalImplUtil::ValueType128 *Decimal_Type128::data() const
{
    return &d_value;
}

DecimalImplUtil::ValueType128 Decimal_Type128::value() const
{
    return d_value;
}

// FREE OPERATORS

                          // operator++/--(int)

Decimal128 operator++(Decimal128& value, int)
{
    Decimal128 rc(value);
    value.operator++();
    return rc;
}

Decimal128 operator--(Decimal128& value, int)
{
    Decimal128 rc(value);
    value.operator--();
    return rc;
}

                            // operator+

Decimal128 operator+(Decimal128 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs += rhs;
#endif
}
Decimal128 operator+(Decimal32 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) += rhs;
#endif
}
Decimal128 operator+(Decimal128 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs += Decimal128(rhs);
#endif
}
Decimal128 operator+(Decimal64 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) += rhs;
#endif
}
Decimal128 operator+(Decimal128 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs += Decimal128(rhs);
#endif
}
Decimal128 operator+(int lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) += rhs;
#endif
}
Decimal128 operator+(Decimal128 lhs, int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs += Decimal128(rhs);
#endif
}
Decimal128 operator+(unsigned int lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) += rhs;
#endif
}
Decimal128 operator+(Decimal128 lhs, unsigned int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs += Decimal128(rhs);
#endif
}
Decimal128 operator+(long lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) += rhs;
#endif
}
Decimal128 operator+(Decimal128 lhs, long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs += Decimal128(rhs);
#endif
}
Decimal128 operator+(unsigned long lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) += rhs;
#endif
}
Decimal128 operator+(Decimal128 lhs, unsigned long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs += Decimal128(rhs);
#endif
}
Decimal128 operator+(long long lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) += rhs;
#endif
}
Decimal128 operator+(Decimal128 lhs, long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs += Decimal128(rhs);
#endif
}
Decimal128 operator+(unsigned long long lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs + rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) += rhs;
#endif
}
Decimal128 operator+(Decimal128 lhs, unsigned long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() + rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs += Decimal128(rhs);
#endif
}

                            // operator-

Decimal128 operator-(Decimal128 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs -= rhs;
#endif
}
Decimal128 operator-(Decimal32 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) -= rhs;
#endif
}
Decimal128 operator-(Decimal128 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs -= Decimal128(rhs);
#endif
}
Decimal128 operator-(Decimal64 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) -= rhs;
#endif
}
Decimal128 operator-(Decimal128 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs -= Decimal128(rhs);
#endif
}
Decimal128 operator-(int lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) -= rhs;
#endif
}
Decimal128 operator-(Decimal128 lhs, int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs -= Decimal128(rhs);
#endif
}
Decimal128 operator-(unsigned int lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) -= rhs;
#endif
}
Decimal128 operator-(Decimal128 lhs, unsigned int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs -= Decimal128(rhs);
#endif
}
Decimal128 operator-(long lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) -= rhs;
#endif
}
Decimal128 operator-(Decimal128 lhs, long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs -= Decimal128(rhs);
#endif
}
Decimal128 operator-(unsigned long lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) -= rhs;
#endif
}
Decimal128 operator-(Decimal128 lhs, unsigned long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs -= Decimal128(rhs);
#endif
}
Decimal128 operator-(long long lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) -= rhs;
#endif
}
Decimal128 operator-(Decimal128 lhs, long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs -= Decimal128(rhs);
#endif
}
Decimal128 operator-(unsigned long long lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs - rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) -= rhs;
#endif
}
Decimal128 operator-(Decimal128 lhs, unsigned long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() - rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs -= Decimal128(rhs);
#endif
}

                            // operator*

Decimal128 operator*(Decimal128 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs *= rhs;
#endif
}
Decimal128 operator*(Decimal32 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) *= rhs;
#endif
}
Decimal128 operator*(Decimal128 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs *= Decimal128(rhs);
#endif
}
Decimal128 operator*(Decimal64 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) *= rhs;
#endif
}
Decimal128 operator*(Decimal128 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs *= Decimal128(rhs);
#endif
}
Decimal128 operator*(int lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) *= rhs;
#endif
}
Decimal128 operator*(Decimal128 lhs, int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs *= Decimal128(rhs);
#endif
}
Decimal128 operator*(unsigned int lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) *= rhs;
#endif
}
Decimal128 operator*(Decimal128 lhs, unsigned int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs *= Decimal128(rhs);
#endif
}
Decimal128 operator*(long lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) *= rhs;
#endif
}
Decimal128 operator*(Decimal128 lhs, long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs *= Decimal128(rhs);
#endif
}
Decimal128 operator*(unsigned long lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) *= rhs;
#endif
}
Decimal128 operator*(Decimal128 lhs, unsigned long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs *= Decimal128(rhs);
#endif
}
Decimal128 operator*(long long lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) *= rhs;
#endif
}
Decimal128 operator*(Decimal128 lhs, long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs *= Decimal128(rhs);
#endif
}
Decimal128 operator*(unsigned long long lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs * rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) *= rhs;
#endif
}
Decimal128 operator*(Decimal128 lhs, unsigned long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() * rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs *= Decimal128(rhs);
#endif
}

                            // operator/

Decimal128 operator/(Decimal128 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs /= rhs;
#endif
}
Decimal128 operator/(Decimal32 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) /= rhs;
#endif
}
Decimal128 operator/(Decimal128 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs /= Decimal128(rhs);
#endif
}
Decimal128 operator/(Decimal64 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) /= rhs;
#endif
}
Decimal128 operator/(Decimal128 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs /= Decimal128(rhs);
#endif
}
Decimal128 operator/(int lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) /= rhs;
#endif
}
Decimal128 operator/(Decimal128 lhs, int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs /= Decimal128(rhs);
#endif
}
Decimal128 operator/(unsigned int lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) /= rhs;
#endif
}
Decimal128 operator/(Decimal128 lhs, unsigned int rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs /= Decimal128(rhs);
#endif
}
Decimal128 operator/(long lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) /= rhs;
#endif
}
Decimal128 operator/(Decimal128 lhs, long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs /= Decimal128(rhs);
#endif
}
Decimal128 operator/(unsigned long lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) /= rhs;
#endif
}
Decimal128 operator/(Decimal128 lhs, unsigned long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs /= Decimal128(rhs);
#endif
}
Decimal128 operator/(long long lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) /= rhs;
#endif
}
Decimal128 operator/(Decimal128 lhs, long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs /= Decimal128(rhs);
#endif
}
Decimal128 operator/(unsigned long long lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs / rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) /= rhs;
#endif
}
Decimal128 operator/(Decimal128 lhs, unsigned long long rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() / rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs /= Decimal128(rhs);
#endif
}

                       // Homogeneous relational operators

bool operator==(Decimal128 lhs, Decimal128 rhs)
{
    return DecimalImplUtil::equals(lhs.value(), rhs.value());
}

bool operator!=(Decimal128 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() != rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return !(lhs == rhs);
#endif
}

bool operator<(Decimal128 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() < rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    decQuad result;
    decQuadCompare(&result, lhs.data(), rhs.data(), getContext());
    return decQuadIsNegative(&result);
#endif
}

bool operator>(Decimal128 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() > rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return rhs < lhs;
#endif
}

bool operator<=(Decimal128 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() <= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return !(rhs < lhs);
#endif
}

bool operator>=(Decimal128 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() >= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return !(lhs < rhs);
#endif
}

                        // Heterogeneous operator==

bool operator==(Decimal32 lhs, Decimal128 rhs)
{
    return DecimalImplUtil::equals(lhs.value(), rhs.value());
}
bool operator==(Decimal128 lhs, Decimal32 rhs)
{
    return DecimalImplUtil::equals(lhs.value(), rhs.value());
}
bool operator==(Decimal64 lhs, Decimal128 rhs)
{
    return DecimalImplUtil::equals(lhs.value(), rhs.value());
}
bool operator==(Decimal128 lhs, Decimal64 rhs)
{
    return DecimalImplUtil::equals(lhs.value(), rhs.value());
}

                        // Heterogeneous operator!=

bool operator!=(Decimal32 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() != rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) != rhs;
#endif
}
bool operator!=(Decimal128 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() != rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs != Decimal128(rhs);
#endif
}
bool operator!=(Decimal64 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() != rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) != rhs;
#endif
}
bool operator!=(Decimal128 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() != rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs != Decimal128(rhs);
#endif
}

                        // Heterogeneous operator<

bool operator<(Decimal32 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() < rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) < rhs;
#endif
}
bool operator<(Decimal128 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() < rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs < Decimal128(rhs);
#endif
}
bool operator<(Decimal64 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() < rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) < rhs;
#endif
}
bool operator<(Decimal128 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() < rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs < Decimal128(rhs);
#endif
}

                        // Heterogeneous operator<=

bool operator<=(Decimal32 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() <= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) <= rhs;
#endif
}
bool operator<=(Decimal128 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() <= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs <= Decimal128(rhs);
#endif
}
bool operator<=(Decimal64 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() <= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) <= rhs;
#endif
}
bool operator<=(Decimal128 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() <= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs <= Decimal128(rhs);
#endif
}

                        // Heterogeneous operator>

bool operator>(Decimal32 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() > rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) > rhs;
#endif
}
bool operator>(Decimal128 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() > rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs > Decimal128(rhs);
#endif
}
bool operator>(Decimal64 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() > rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) > rhs;
#endif
}
bool operator>(Decimal128 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() > rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs > Decimal128(rhs);
#endif
}

                        // Heterogeneous operator>=

bool operator>=(Decimal32 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() >= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) >= rhs;
#endif
}
bool operator>=(Decimal128 lhs, Decimal32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() >= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs >= Decimal128(rhs);
#endif
}
bool operator>=(Decimal64 lhs, Decimal128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() >= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return Decimal128(lhs) >= rhs;
#endif
}
bool operator>=(Decimal128 lhs, Decimal64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs.value() >= rhs.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return lhs >= Decimal128(rhs);
#endif
}

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
        value = DecimalImplUtil::parse32(buffer);
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
        value = DecimalImplUtil::parse64(buffer);
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
        value = DecimalImplUtil::parse128(buffer);
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
}  // close enterprise namespace

                     // Streaming operators implementations

                                  // Output

template <class CHARTYPE, class TRAITS>
bsl::basic_ostream<CHARTYPE, TRAITS>&
BloombergLP::bdldfp::operator<<(bsl::basic_ostream<CHARTYPE, TRAITS>& stream,
                               Decimal32                             object)
{
    return print(stream, object);
}

template <class CHARTYPE, class TRAITS>
bsl::basic_ostream<CHARTYPE, TRAITS>&
BloombergLP::bdldfp::operator<<(bsl::basic_ostream<CHARTYPE, TRAITS>& stream,
                               Decimal64                             object)
{
    return print(stream, object);
}

template <class CHARTYPE, class TRAITS>
bsl::basic_ostream<CHARTYPE, TRAITS>&
BloombergLP::bdldfp::operator<<(bsl::basic_ostream<CHARTYPE, TRAITS>& stream,
                               Decimal128                            object)
{
    return print(stream, object);
}

                                  // Input

template <class CHARTYPE, class TRAITS>
bsl::basic_istream<CHARTYPE, TRAITS>&
BloombergLP::bdldfp::operator>>(bsl::basic_istream<CHARTYPE, TRAITS>& stream,
                               Decimal32&                            object)
{
    return read(stream, object);
}

template <class CHARTYPE, class TRAITS>
bsl::basic_istream<CHARTYPE, TRAITS>&
BloombergLP::bdldfp::operator>>(bsl::basic_istream<CHARTYPE, TRAITS>& stream,
                               Decimal64&                            object)
{
    return read(stream, object);
}

template <class CHARTYPE, class TRAITS>
bsl::basic_istream<CHARTYPE, TRAITS>&
BloombergLP::bdldfp::operator>>(bsl::basic_istream<CHARTYPE, TRAITS>& stream,
                               Decimal128&                           object)
{
    return read(stream, object);
}

                // Streaming operators explicit instantiations

                                // Decimal32

template
bsl::basic_istream<char>&
BloombergLP::bdldfp::operator>> <char, bsl::char_traits<char> >(
                                        bsl::basic_istream<char>&      in,
                                        BloombergLP::bdldfp::Decimal32& value);
template
bsl::basic_istream<wchar_t>&
BloombergLP::bdldfp::operator>> <wchar_t, bsl::char_traits<wchar_t> >(
                                        bsl::basic_istream<wchar_t>&   in,
                                        BloombergLP::bdldfp::Decimal32& value);

template
bsl::basic_ostream<char>&
BloombergLP::bdldfp::operator<< <char, bsl::char_traits<char> >(
                                         bsl::basic_ostream<char>&     out,
                                         BloombergLP::bdldfp::Decimal32 value);
template
bsl::basic_ostream<wchar_t>&
BloombergLP::bdldfp::operator<< <wchar_t, bsl::char_traits<wchar_t> >(
                                         bsl::basic_ostream<wchar_t>&  out,
                                         BloombergLP::bdldfp::Decimal32 value);

                                // Decimal64

template
bsl::basic_istream<char>&
BloombergLP::bdldfp::operator>> <char, bsl::char_traits<char> >(
                                        bsl::basic_istream<char>&      in,
                                        BloombergLP::bdldfp::Decimal64& value);
template
bsl::basic_istream<wchar_t>&
BloombergLP::bdldfp::operator>> <wchar_t, bsl::char_traits<wchar_t> >(
                                        bsl::basic_istream<wchar_t>&   in,
                                        BloombergLP::bdldfp::Decimal64& value);

template
bsl::basic_ostream<char>&
BloombergLP::bdldfp::operator<< <char, bsl::char_traits<char> >(
                                         bsl::basic_ostream<char>&     out,
                                         BloombergLP::bdldfp::Decimal64 value);
template
bsl::basic_ostream<wchar_t>&
BloombergLP::bdldfp::operator<< <wchar_t, bsl::char_traits<wchar_t> >(
                                         bsl::basic_ostream<wchar_t>&  out,
                                         BloombergLP::bdldfp::Decimal64 value);

                                // Decimal128

template
bsl::basic_istream<char>&
BloombergLP::bdldfp::operator>> <char, bsl::char_traits<char> >(
                                       bsl::basic_istream<char>&       in,
                                       BloombergLP::bdldfp::Decimal128& value);
template
bsl::basic_istream<wchar_t>&
BloombergLP::bdldfp::operator>> <wchar_t, bsl::char_traits<wchar_t> >(
                                       bsl::basic_istream<wchar_t>&    in,
                                       BloombergLP::bdldfp::Decimal128& value);

template
bsl::basic_ostream<char>&
BloombergLP::bdldfp::operator<< <char, bsl::char_traits<char> >(
                                        bsl::basic_ostream<char>&      out,
                                        BloombergLP::bdldfp::Decimal128 value);
template
bsl::basic_ostream<wchar_t>&
BloombergLP::bdldfp::operator<< <wchar_t, bsl::char_traits<wchar_t> >(
                                        bsl::basic_ostream<wchar_t>&   out,
                                        BloombergLP::bdldfp::Decimal128 value);

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
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return 1e-95df;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv, "1e-95", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::max()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return 9.999999e96df;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv, "9.999999e96", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::epsilon()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return 1e-6df;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv, "1e-6", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::round_error()
    BSLS_NOTHROW_SPEC
{ // TBD TODO - determine the real value from the round mode!
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return 1.0df;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv, "1.0", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::infinity()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return HUGE_VAL_D32;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv, "INF", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::quiet_NaN()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return BDLDFP_DECIMALPLATFORM_C99_QNAN32;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv, "NaN", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal32
   std::numeric_limits<BloombergLP::bdldfp::Decimal32>::signaling_NaN()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return BDLDFP_DECIMALPLATFORM_C99_SNAN32;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv, "sNaN", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::denorm_min()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return 0.000001E-95df;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv,
                        "0.000001E-95",
                        BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

            // --------------------------------------------------
            // std::numeric_limits<BloombergLP::bdldfp::Decimal64>
            // --------------------------------------------------

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::min()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return 1e-383dd;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv, "1e-383", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::max()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return 9.999999999999999e384dd;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv,
                        "9.999999999999999e384",
                        BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::epsilon()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return 1e-15dd;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv, "1e-15", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::round_error()
    BSLS_NOTHROW_SPEC
{  // TBD TODO - determine the real value from the round mode!
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return 1.0dd;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv, "1.0", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::infinity()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return HUGE_VAL_D64;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv, "INF", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::quiet_NaN()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return BDLDFP_DECIMALPLATFORM_C99_QNAN64;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv, "NaN", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal64
   std::numeric_limits<BloombergLP::bdldfp::Decimal64>::signaling_NaN()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return BDLDFP_DECIMALPLATFORM_C99_SNAN64;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv, "sNaN", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::denorm_min()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return 0.000000000000001e-383dd;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv,
                        "0.000000000000001e-383",
                        BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

            // ---------------------------------------------------
            // std::numeric_limits<BloombergLP::bdldfp::Decimal128>
            // ---------------------------------------------------

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::min()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return 1e-6143dl;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv, "1e-6143", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::max()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return 9.999999999999999999999999999999999e6144dl;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv,
                      "9.999999999999999999999999999999999e6144",
                      BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::epsilon()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return 1e-33dl;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv, "1e-33", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::round_error()
    BSLS_NOTHROW_SPEC
{  // TBD TODO - determine the real value from the round mode setting!
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return 1.0dl;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv, "1.0", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::infinity()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return HUGE_VAL_D128;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv, "INF", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::quiet_NaN()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return BDLDFP_DECIMALPLATFORM_C99_QNAN128;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv, "NaN", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal128
  std::numeric_limits<BloombergLP::bdldfp::Decimal128>::signaling_NaN()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return BDLDFP_DECIMALPLATFORM_C99_QNAN128;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv, "sNaN", BloombergLP::bdldfp::getContext());
    return rv;
#endif
}

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::denorm_min()
    BSLS_NOTHROW_SPEC
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return 0.000000000000000000000000000000001e-6143dl;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv,
                      "0.000000000000000000000000000000001e-6143",
                      BloombergLP::bdldfp::getContext());
    return rv;
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
