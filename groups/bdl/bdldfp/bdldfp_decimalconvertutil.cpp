// bdldfp_decimalconvertutil.cpp                                      -*-C++-*-
#include <bdldfp_decimalconvertutil.h>

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

#include <bdldfp_decimalplatform.h>

#include <bsls_assert.h>

#if BDLDFP_DECIMALPLATFORM_C99_TR
#  ifndef  __STDC_WANT_DEC_FP__
#    error __STDC_WANT_DEC_FP__ must be defined on the command line!
     char die[-42];     // if '#error' unsupported
#  endif
#endif

#include <math.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>  // TODO TBD - remove this, it is just for debugging
#include <bsl_cstring.h>

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
    *this->pptr() = 0; return this->pbase();
}

                    // =================
                    // class PtrInputBuf
                    // =================

struct PtrInputBuf : bsl::streambuf {
    explicit PtrInputBuf(const char *s);
        // Create a 'PtrInputBuf' that reads from the specified 's'
        // NUL-terminated string.
};
                    // -----------------
                    // class PtrInputBuf
                    // -----------------

PtrInputBuf::PtrInputBuf(const char *s) {
    char *x = const_cast<char *>(s);
    this->setg(x, x, x + bsl::strlen(x));
}

template <class DECIMAL_TYPE, class BINARY_TYPE>
void makeBinaryFloatingPoint(BINARY_TYPE *bfp, DECIMAL_TYPE dfp)
    // Construct, in the specified 'bfp', a Binary Floating Point
    // representation of the value of the specified 'dfp'.
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    *bfp = dfp.value();
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    // Handle special values of +/-INF, and NaNs without use of streams

    // TODO TBD - set exception-flags here, but how do to know which one??? (E)
    // ::decClass fclass(decFloatClass(dfp)); -- to be used for NaNs

    if (dfp != dfp) {
        *bfp = bsl::numeric_limits<BINARY_TYPE>::quiet_NaN();
        if (dfp < DECIMAL_TYPE(0)) {
            *bfp = -*bfp;
        }
        return;                                                       // RETURN
    }

    if (dfp == bsl::numeric_limits<DECIMAL_TYPE>::infinity()) {
        *bfp = bsl::numeric_limits<BINARY_TYPE>::infinity();
        return;                                                       // RETURN
    }

    if (dfp == -bsl::numeric_limits<DECIMAL_TYPE>::infinity()) {
        *bfp = -bsl::numeric_limits<BINARY_TYPE>::infinity();
        return;                                                       // RETURN
    }

    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(std::numeric_limits<DECIMAL_TYPE>::digits10);
    out << dfp;
    BSLS_ASSERT(out);

    PtrInputBuf pb(bb.str());
    bsl::istream in(&pb);
    if (!(in >> *bfp)) {
        if (dfp > DECIMAL_TYPE(1) || dfp < DECIMAL_TYPE(-1)) { // overflow
            *bfp = bsl::numeric_limits<BINARY_TYPE>::infinity();
        } else { // underflow
            *bfp = 0.0;
        }
        if (dfp < DECIMAL_TYPE(0)) {
            *bfp = -*bfp;
        }
    }
#endif
}

                        // Reverse Memory

static void memrev(void *buffer, size_t count)
    // Reverse the order of the first specified 'count' bytes, at the beginning
    // of the specified 'buffer'.  'count % 2' must be zero.
{
    char *b = static_cast<char *>(buffer);
    for (unsigned int i = 0; i < count / 2; ++i) {
        bsl::swap(b[i], b[count - i - 1]);
    }
}

                        // Mem copy with reversal functions

unsigned char *memCpyFlip(void *out, const void *in, size_t count)
    // Copy the first specified 'count' bytes from the specified 'in' buffer to
    // the specified 'out' buffer and flip the byte order if needed.
{
    // Just stick the bytes into the buffer first
    bsl::memcpy(out, in, count);
#if BDLDFP_DECIMALPLATFORM_LITTLE_ENDIAN
    // little endian, needs to do some byte juggling
    memrev(out, count);
#endif
    return static_cast<unsigned char*>(out) + count;
}

                        // Decimal-network conversion functions

template <class DECIMAL_TYPE>
unsigned char *decimalFromNetworkT(DECIMAL_TYPE        *decimal,
                                   const unsigned char *buffer)
    // Construct into the specified 'decimal', the base-10 value represented by
    // the network-ordered bytes in the specified 'buffer', and return a raw
    // memory pointer, providing modifiable access, to one byte past the last
    // byte of 'decimal'.
{
    return memCpyFlip(decimal, buffer, sizeof(DECIMAL_TYPE));
}

template <class DECIMAL_TYPE>
unsigned char *decimalToNetworkT(unsigned char *buffer, DECIMAL_TYPE decimal)
    // Construct into the specified 'buffer', the network-ordered byte
    // representation of the base-10 value of the specified 'decimal', and,
    // return a raw memory pointer, providing modifiable access, to one byte
    // past the last written byte of the 'buffer'.
{
    return memCpyFlip(buffer, &decimal, sizeof(decimal));
}

                  // Helpers for Restoring Decimal from Binary

template <class DECIMAL_TYPE, class BINARY_TYPE>
void restoreDecimalFromBinary(DECIMAL_TYPE *dfp, BINARY_TYPE bfp)
    // Construct, in the specified 'dfp', a decimal floating point
    // representation of the value of the binary floating point value specified
    // by 'bfp'.
{
    // Handle special values of +/-INF, and NaNs without use of streams

    // TODO TBD - set exception-flags here, but how do to know which one??? (E)
    // ::decClass fclass(decFloatClass(dfp)); -- to be used for NaNs

    if (bfp != bfp) {
        *dfp = bsl::numeric_limits<DECIMAL_TYPE>::quiet_NaN();
        if (bfp < 0) {
            *dfp = -*dfp;
        }
        return;                                                       // RETURN
    }

    if (bfp == bsl::numeric_limits<BINARY_TYPE>::infinity()) {
        *dfp = bsl::numeric_limits<DECIMAL_TYPE>::infinity();
        return;                                                       // RETURN
    }

    if (bfp == -bsl::numeric_limits<BINARY_TYPE>::infinity()) {
        *dfp = -bsl::numeric_limits<DECIMAL_TYPE>::infinity();
        return;                                                       // RETURN
    }

    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(std::numeric_limits<BINARY_TYPE>::digits10);
    out << bfp;
    BSLS_ASSERT(out);

    PtrInputBuf pb(bb.str());
    bsl::istream in(&pb);
    if (!(in >> *dfp)) { // Try to be sensible in case of error
        if (bfp > 1 || bfp < -1) { // overflow
            *dfp = bsl::numeric_limits<DECIMAL_TYPE>::infinity();
        } else { // underflow
            *dfp = DECIMAL_TYPE(0);
        }
        if (bfp < 0) {
            *dfp = -*dfp;
        }
    }
}

} // end anonymous namespace

                        // decimalToLongDouble functions

long double DecimalConvertUtil::decimal32ToLongDouble(Decimal32 decimal)
{
    long double rv;
    makeBinaryFloatingPoint(&rv, decimal);
    return rv;
}
long double DecimalConvertUtil::decimal64ToLongDouble(Decimal64 decimal)
{
    long double rv;
    makeBinaryFloatingPoint(&rv, decimal);
    return rv;
}
long double DecimalConvertUtil::decimal128ToLongDouble(Decimal128 decimal)
{
    long double rv;
    makeBinaryFloatingPoint(&rv, decimal);
    return rv;
}
long double DecimalConvertUtil::decimalToLongDouble(Decimal32 decimal)
{
    return decimal32ToLongDouble(decimal);
}
long double DecimalConvertUtil::decimalToLongDouble(Decimal64 decimal)
{
    return decimal64ToLongDouble(decimal);
}
long double DecimalConvertUtil::decimalToLongDouble(Decimal128 decimal)
{
    return decimal128ToLongDouble(decimal);
}

                        // decimalToDouble functions

double DecimalConvertUtil::decimal32ToDouble(Decimal32 decimal)
{
    double rv;
    makeBinaryFloatingPoint(&rv, decimal);
    return rv;
}
double DecimalConvertUtil::decimal64ToDouble(Decimal64 decimal)
{
    double rv;
    makeBinaryFloatingPoint(&rv, decimal);
    return rv;
}
double DecimalConvertUtil::decimal128ToDouble(Decimal128 decimal)
{
    double rv;
    makeBinaryFloatingPoint(&rv, decimal);
    return rv;
}
double DecimalConvertUtil::decimalToDouble(Decimal32 decimal)
{
    return decimal32ToDouble(decimal);
}
double DecimalConvertUtil::decimalToDouble(Decimal64 decimal)
{
    return decimal64ToDouble(decimal);
}
double DecimalConvertUtil::decimalToDouble(Decimal128 decimal)
{
    return decimal128ToDouble(decimal);
}

                        // decimalToFloat functions

float DecimalConvertUtil::decimal32ToFloat (Decimal32 decimal)
{
    float rv;
    makeBinaryFloatingPoint(&rv, decimal);
    return rv;
}
float DecimalConvertUtil::decimal64ToFloat (Decimal64 decimal)
{
    float rv;
    makeBinaryFloatingPoint(&rv, decimal);
    return rv;
}
float DecimalConvertUtil::decimal128ToFloat (Decimal128 decimal)
{
    float rv;
    makeBinaryFloatingPoint(&rv, decimal);
    return rv;
}
float DecimalConvertUtil::decimalToFloat (Decimal32 decimal)
{
    return decimal32ToFloat (decimal);
}
float DecimalConvertUtil::decimalToFloat (Decimal64 decimal)
{
    return decimal64ToFloat (decimal);
}
float DecimalConvertUtil::decimalToFloat (Decimal128 decimal)
{
    return decimal128ToFloat (decimal);
}

                        // Network format converters

#if BDLDFP_DECIMALPLATFORM_BININT
#  error Binary Integer Significant format is not supported.
struct BinaryIntegerSignificant_FormatNotSupported;
BinaryIntegerSignificant_FormatNotSupported //
    forceError[sizeof(BinaryIntegerSignificant_FormatNotSupported)];
#endif

// Note that we do not use platform or bslsl supported converters because they
// work in terms of integers, so they would probably bleed out on the
// strict-aliasing rules.  We may solve that later on using the "union trick"
// and delegating to 'bsls_byteorder', but for now let's take it slow.

                        // Conversion to Network functions

unsigned char *DecimalConvertUtil::decimal32ToNetwork(unsigned char *buffer,
                                                      Decimal32      decimal)
{
    BSLS_ASSERT(buffer != 0);
    return decimalToNetworkT(buffer, decimal);
}

unsigned char *DecimalConvertUtil::decimal64ToNetwork(unsigned char *buffer,
                                                      Decimal64      decimal)
{
    BSLS_ASSERT(buffer != 0);
    return decimalToNetworkT(buffer, decimal);
}

unsigned char *DecimalConvertUtil::decimal128ToNetwork(unsigned char *buffer,
                                                       Decimal128     decimal)
{
    BSLS_ASSERT(buffer != 0);
    return decimalToNetworkT(buffer, decimal);
}

unsigned char *DecimalConvertUtil::decimalToNetwork(unsigned char *buffer,
                                                    Decimal32      decimal)
{
    BSLS_ASSERT(buffer != 0);
    return decimalToNetworkT(buffer, decimal);
}

unsigned char *DecimalConvertUtil::decimalToNetwork(unsigned char *buffer,
                                                    Decimal64      decimal)
{
    BSLS_ASSERT(buffer != 0);
    return decimalToNetworkT(buffer, decimal);
}

unsigned char *DecimalConvertUtil::decimalToNetwork(unsigned char *buffer,
                                                    Decimal128     decimal)
{
    BSLS_ASSERT(buffer != 0);
    return decimalToNetworkT(buffer, decimal);
}

                        // Conversion to Network functions

unsigned char *DecimalConvertUtil::decimal32FromNetwork(
                                                  Decimal32           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal != 0);
    return decimalFromNetworkT(decimal, buffer);
}

unsigned char *DecimalConvertUtil::decimal64FromNetwork(
                                                  Decimal64           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal != 0);
    return decimalFromNetworkT(decimal, buffer);
}

unsigned char *DecimalConvertUtil::decimal128FromNetwork(
                                                  Decimal128          *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal != 0);
    return decimalFromNetworkT(decimal, buffer);
}

unsigned char *DecimalConvertUtil::decimalFromNetwork(
                                                  Decimal32           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal != 0);
    return decimalFromNetworkT(decimal, buffer);
}

unsigned char *DecimalConvertUtil::decimalFromNetwork(
                                                  Decimal64           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal != 0);
    return decimalFromNetworkT(decimal, buffer);
}

unsigned char *DecimalConvertUtil::decimalFromNetwork(
                                                  Decimal128          *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal != 0);
    return decimalFromNetworkT(decimal, buffer);
}

            // Restore a Decimal Floating-Point from a Binary

                        // DecimalFromLongDouble functions

Decimal32 DecimalConvertUtil::decimal32FromLongDouble(long double binary)
{
    Decimal32 rv;
    restoreDecimalFromBinary(&rv, binary);
    return rv;
}
Decimal64 DecimalConvertUtil::decimal64FromLongDouble(long double binary)
{
    Decimal64 rv;
    restoreDecimalFromBinary(&rv, binary);
    return rv;
}
Decimal128 DecimalConvertUtil::decimal128FromLongDouble(long double binary)
{
    Decimal128 rv;
    restoreDecimalFromBinary(&rv, binary);
    return rv;
}

                        // DecimalFromDouble functions

Decimal32 DecimalConvertUtil::decimal32FromDouble(double binary)
{
    Decimal32 rv;
    restoreDecimalFromBinary(&rv, binary);
    return rv;
}
Decimal64 DecimalConvertUtil::decimal64FromDouble(double binary)
{
    Decimal64 rv;
    restoreDecimalFromBinary(&rv, binary);
    return rv;
}
Decimal128 DecimalConvertUtil::decimal128FromDouble(double binary)
{
    Decimal128 rv;
    restoreDecimalFromBinary(&rv, binary);
    return rv;
}

                        // DecimalFromFloat functions

Decimal32 DecimalConvertUtil::decimal32FromFloat(float binary)
{
    Decimal32 rv;
    restoreDecimalFromBinary(&rv, binary);
    return rv;
}
Decimal64 DecimalConvertUtil::decimal64FromFloat(float binary)
{
    Decimal64 rv;
    restoreDecimalFromBinary(&rv, binary);
    return rv;
}
Decimal128 DecimalConvertUtil::decimal128FromFloat(float binary)
{
    Decimal128 rv;
    restoreDecimalFromBinary(&rv, binary);
    return rv;
}

}  // close package namespace
}  // close enterprise namespace

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
