// bdldfp_decimalconvertutil_decnumber.cpp                            -*-C++-*-

#include <bdldfp_decimalconvertutil_decnumber.h>


#include <bsls_ident.h>
BSLS_IDENT("$Id$")

#include <bdlscm_version.h>
#include <bdldfp_decimal.h>
#include <bdldfp_decimalplatform.h>
#include <bdldfp_decimalimputil.h>

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
inline
BufferBuf<Size>::BufferBuf()
{
    reset();
}

template <int Size>
inline
void BufferBuf<Size>::reset()
{
    this->setp(this->d_buf, this->d_buf + Size);
}

template <int Size>
inline
const char *BufferBuf<Size>::str()
{
    *this->pptr() = 0;
    return this->pbase();
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
}

}  // close anonymous namespace


                          // ----------------------------------
                          // class DecimalConvertUtil_DecNumber
                          // ----------------------------------
// CLASS METHODS

                        // decimalToDouble functions

double
DecimalConvertUtil_DecNumber::decimalToDouble(Decimal32 decimal)
{
    double result;
    makeBinaryFloatingPoint(&result, decimal);
    return result;
}

double
DecimalConvertUtil_DecNumber::decimalToDouble(Decimal64 decimal)
{
    double result;
    makeBinaryFloatingPoint(&result, decimal);
    return result;
}

double
DecimalConvertUtil_DecNumber::decimalToDouble(Decimal128 decimal)
{
    double result;
    makeBinaryFloatingPoint(&result, decimal);
    return result;
}

                        // decimalToFloat functions

float
DecimalConvertUtil_DecNumber::decimalToFloat(Decimal32 decimal)
{
    double result;
    makeBinaryFloatingPoint(&result, decimal);
    return result;
}

float
DecimalConvertUtil_DecNumber::decimalToFloat(Decimal64 decimal)
{
    double result;
    makeBinaryFloatingPoint(&result, decimal);
    return result;
}

float
DecimalConvertUtil_DecNumber::decimalToFloat(Decimal128 decimal)
{
    float result;
    makeBinaryFloatingPoint(&result, decimal);
    return result;
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
