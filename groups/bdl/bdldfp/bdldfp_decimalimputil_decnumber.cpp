// bdldfp_decimalimputil_decnumber.cpp                                -*-C++-*-
#include <bdldfp_decimalimputil_decnumber.h>

#include <bsls_ident.h>

#include <bsl_iostream.h>


BSLS_IDENT("$Id$")

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

template <typename TYPE>
bool isFinite(TYPE value)
{
    return value == value
        && value != std::numeric_limits<TYPE>::infinity()
        && value != -std::numeric_limits<TYPE>::infinity();
}

template <typename TYPE>
const char *nonFiniteToString(TYPE value)
    // Return a string for the non finite specified 'value'.  The behavior
    // is undefined unless 'value' not finite.
{
    BSLS_ASSERT(!isFinite(value));
    if (bsl::numeric_limits<TYPE>::infinity() == value) {
        return "inf";
    }
    if (-bsl::numeric_limits<TYPE>::infinity() == value) {
        return "-inf";
    }
    BSLS_ASSERT(value != value);  // isnan
    return "nan";
}

}  // close unnamed namespace


                          // ------------------------------
                          // class DecimalImpUtil_DecNumber
                          // ------------------------------

// PRIVATE HELPERS
DecimalImpUtil_DecNumber::ValueType32
DecimalImpUtil_DecNumber::roundToDecimal32(long long int value)
{
    BSLS_ASSERT(value < -9999999 || value > 9999999);

    return convertToDecimal32(int64ToDecimal64(value));
}

DecimalImpUtil_DecNumber::ValueType32
DecimalImpUtil_DecNumber::roundToDecimal32(unsigned long long int value)
{
    BSLS_ASSERT(value > 9999999);

    return convertToDecimal32(uint64ToDecimal64(value));

}

DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::roundToDecimal64(long long int value)
{
    BSLS_ASSERT(value < -9999999999999999LL || value > 9999999999999999LL);

    return convertToDecimal64(int64ToDecimal128(value));
}

DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::roundToDecimal64(unsigned long long int value)
{
    BSLS_ASSERT(value > 9999999999999999LL);

    return convertToDecimal64(uint64ToDecimal128(value));
}


DecimalImpUtil_DecNumber::ValueType32
DecimalImpUtil_DecNumber::binaryToDecimal32(float value)
{
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly

    ValueType32 result;
    bdldfp::BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(7);

    // On some platforms (Visual Studio) 'operator<<' renders something
    // un-parsable for non-finite values.

    if (!isFinite(value)) {
        out << nonFiniteToString(value);
    }
    else {
        out << value;
    }
    decSingleFromString(&result, bb.str(), getDecNumberContext());
    return result;
}

DecimalImpUtil_DecNumber::ValueType32
DecimalImpUtil_DecNumber::binaryToDecimal32(double value)
{
    ValueType32 result;
    bdldfp::BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(7);

    // On some platforms (Visual Studio) 'operator<<' renders something
    // un-parsable for non-finite values.

    if (!isFinite(value)) {
        out << nonFiniteToString(value);
    }
    else {
        out << value;
    }
    decSingleFromString(&result, bb.str(), getDecNumberContext());
    return result;
}

DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::binaryToDecimal64(float value)
{
    ValueType64 result;
    bdldfp::BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(16);
    out << value;
    decDoubleFromString(&result, bb.str(), getDecNumberContext());
    return result;
}

DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::binaryToDecimal64(double value)
{
    ValueType64 result;
    bdldfp::BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(16);
    out << value;
    decDoubleFromString(&result, bb.str(), getDecNumberContext());
    return result;
}

DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::binaryToDecimal128(float value)
{
    ValueType128 result;
    bdldfp::BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(34);
    out << value;
    decQuadFromString(&result, bb.str(), getDecNumberContext());
    return result;
}

DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::binaryToDecimal128(double value)
{
    ValueType128 result;
    bdldfp::BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(34);
    out << value;
    decQuadFromString(&result, bb.str(), getDecNumberContext());
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
