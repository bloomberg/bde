// bdldfp_decimalconvertutil_decnumber.cpp                            -*-C++-*-

#include <bdldfp_decimalconvertutil_decnumber.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdldfp_decimalconvertutil_decnumber_cpp,"$Id$ $CSID$")

#include <bdlscm_version.h>
#include <bdldfp_decimal.h>
#include <bdldfp_decimalplatform.h>
#include <bdldfp_decimalimputil.h>

#include <bsl_ostream.h>
#include <bsl_istream.h>

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

}  // close unnamed namespace


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
    float result;
    makeBinaryFloatingPoint(&result, decimal);
    return result;
}

float
DecimalConvertUtil_DecNumber::decimalToFloat(Decimal64 decimal)
{
    float result;
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
// Copyright 2014 Bloomberg Finance L.P.
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
