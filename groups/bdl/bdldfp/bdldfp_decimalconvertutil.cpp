// bdldfp_decimalconvertutil.cpp                                      -*-C++-*-
#include <bdldfp_decimalconvertutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdldfp_decimalconvertutil_cpp,"$Id$ $CSID$")

#include <bdldfp_decimalplatform.h>

#include <bsls_assert.h>

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
#  ifndef  __STDC_WANT_DEC_FP__
#    error __STDC_WANT_DEC_FP__ must be defined on the command line!
     char die[-42];     // if '#error' unsupported
#  endif
#endif

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace bdldfp {

namespace {
                        // Reverse Memory

static void memrev(void *buffer, size_t count)
    // Reverse the order of the first specified 'count' bytes, at the beginning
    // of the specified 'buffer'.  'count % 2' must be zero.
{
    unsigned char *b = static_cast<unsigned char *>(buffer);
    bsl::reverse(b, b + count);
}

                        // Mem copy with reversal functions

unsigned char *memReverseIfNeeded(void *buffer, size_t count)
    // Reverse the first specified 'count' bytes from the specified 'buffer`,
    // if the host endian is different from network endian, and return the
    // address computed from 'static_cast<unsigned char *>(buffer) + count'.
{
#ifdef BDLDFP_DECIMALPLATFORM_LITTLE_ENDIAN
    // little endian, needs to do some byte juggling
    memrev(buffer, count);
#endif
    return static_cast<unsigned char*>(buffer) + count;
}

                        // Decimal-network conversion functions

template <class DECIMAL_TYPE>
const unsigned char *decimalFromNetworkT(DECIMAL_TYPE        *decimal,
                                         const unsigned char *buffer)
    // Construct into the specified 'decimal', the base-10 value represented by
    // the network-ordered bytes in the specified 'buffer', and return a raw
    // memory pointer, providing modifiable access, to one byte past the last
    // byte read from 'buffer'.
{
    bsl::memcpy(decimal, buffer, sizeof(DECIMAL_TYPE));
    memReverseIfNeeded(decimal, sizeof(DECIMAL_TYPE));

    DecimalConvertUtil::decimalFromDPD(
                                   decimal,
                                   reinterpret_cast<unsigned char *>(decimal));

    return buffer + sizeof(DECIMAL_TYPE);
}


template <class DECIMAL_TYPE>
unsigned char *decimalToNetworkT(unsigned char *buffer, DECIMAL_TYPE decimal)
    // Construct into the specified 'buffer', the network-ordered byte
    // representation of the base-10 value of the specified 'decimal', and,
    // return a raw memory pointer, providing modifiable access, to one byte
    // past the last written byte of the 'buffer'.
{
    DecimalConvertUtil::decimalToDPD(buffer, decimal);
    return memReverseIfNeeded(buffer, sizeof(DECIMAL_TYPE));
}

                        // =================
                        // class StdioFormat
                        // =================

template <class FORMATTED_TYPE> struct StdioFormat;
    // This 'struct' template provides a method, 'format', that returns a
    // 'printf'-style format string to format values of the template parameter
    // type 'FORMATTED_TYPE' that can be used to restore a decimal value that
    // was previously converted to this type.

template <>
struct StdioFormat<float> {
    // This template specialization of 'StdioFormat' provides a function that
    // returns a 'printf'-style format string for 'float' values.

    static const char* format();
        // Return a 'printf'-style format string that can be used to restore a
        // decimal value that was previously converted to a 'float' value.
        // Refer to the documentation of 'decimalFromFloat' for the conversion
        // rules.
};

template <>
struct StdioFormat<double> {
    // This template specialization of 'StdioFormat' provides a function that
    // returns a 'printf'-style format string for 'double' values.

    static char const* format();
        // Return a 'printf'-style format string that can be used to restore a
        // decimal value that was previously converted to a 'double' value.
        // Refer to the documentation of 'decimalFromDouble' for the conversion
        // rules.
};

                        // -----------------
                        // class StdioFormat
                        // -----------------

const char* StdioFormat<float>::format()
{
    return "%.6g";
}

const char* StdioFormat<double>::format()
{
    return "%.15g";
}

                        // ===================
                        // class DecimalTraits
                        // ===================

template <class DECIMAL_TYPE> struct DecimalTraits;
    // This 'struct' template provides a way to create an object of the
    // template parameter type 'DECIMAL_TYPE' though a consistent interface.


template <>
struct DecimalTraits<Decimal32> {
    // This template specialization of 'DecimalTraits' provides functions to
    // create 'Decimal32' values.

    typedef int SignificandType;
        // This 'typedef' defines a type that is large enough to hold the
        // significant of 'Decimal32'.

    static Decimal32 make(int significand, int exponent);
        // Return a 'Decimal32' value having the specified 'significand' and
        // the specified 'exponent'.
};

template <>
struct DecimalTraits<Decimal64> {
    // This template specialization of 'DecimalTraits' provides utilities to
    // create 'Decimal64' values.

    typedef long long SignificandType;
        // This 'typedef' defines a type that is large enough to hold the
        // significant of 'Decimal64'.

    static Decimal64 make(long long significand, int exponent);
        // Return a 'Decimal64' value having the specified 'significand' and
        // the specified 'exponent'.

};

template <>
struct DecimalTraits<bdldfp::Decimal128> {
    // This template specialization of 'DecimalTraits' provides utilities to
    // create 'Decimal128' values.

    typedef long long SignificandType;
        // This 'typedef' defines a type that is large enough to hold the
        // significant of 'Decimal128' if it's small enough to be convertible
        // to a double.

    static bdldfp::Decimal128 make(long long significand, int exponent);
        // Return a 'Decimal128' value having the specified 'significand' and
        // the specified 'exponent'.
};

                        // ===================
                        // class DecimalTraits
                        // ===================

Decimal32 DecimalTraits<Decimal32>::make(int significand, int exponent)
{
    return bdldfp::DecimalUtil::makeDecimalRaw32(significand, exponent);
}

Decimal64 DecimalTraits<Decimal64>::make(long long significand, int exponent)
{
    return bdldfp::DecimalUtil::makeDecimalRaw64(significand, exponent);
}

Decimal128 DecimalTraits<Decimal128>::make(long long significand, int exponent)
{
    return bdldfp::DecimalUtil::makeDecimalRaw128(significand, exponent);
}

                  // Helpers for Restoring Decimal from Binary

template <class DECIMAL_TYPE, class BINARY_TYPE>
void restoreDecimalFromBinary(DECIMAL_TYPE *dfp, BINARY_TYPE bfp)
    // Construct, in the specified 'dfp', a decimal floating point
    // representation of the value of the binary floating point value specified
    // by 'bfp'.
{

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

    char buffer[48];
    snprintf(buffer, sizeof(buffer), StdioFormat<BINARY_TYPE>::format(), bfp);

    typename DecimalTraits<DECIMAL_TYPE>::SignificandType significand(0);
    int  exponent(0);
    bool negative(false);

    char const* it(buffer);
    if (*it == '-') {
        negative = true;
        ++it;
    }
    for (; bsl::isdigit(static_cast<unsigned char>(*it)); ++it) {
        significand = significand * 10 + (*it - '0');
    }
    if (*it == '.') {
        ++it;
        for (; bsl::isdigit(static_cast<unsigned char>(*it)); ++it) {
            significand = significand * 10 + (*it - '0');
            --exponent;
        }
    }
    if (*it == 'e' || *it == 'E') {
        ++it;
        exponent += bsl::atoi(it);
    }

    *dfp = DecimalTraits<DECIMAL_TYPE>::make(significand, exponent);

    // Because the significand is a signed integer, it can not represent the
    // value -0, which distinct from +0 in decimal floating point. So instead
    // of converting the significand to a signed value, we change the decimal
    // value based on the sign appropriately after the decimal value is
    // created.

    if (negative) {
        *dfp = -(*dfp);
    }
}

}  // close unnamed namespace

                        // Network format converters

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

const unsigned char *DecimalConvertUtil::decimal32FromNetwork(
                                                  Decimal32           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal != 0);
    return decimalFromNetworkT(decimal, buffer);
}

const unsigned char *DecimalConvertUtil::decimal64FromNetwork(
                                                  Decimal64           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal != 0);
    return decimalFromNetworkT(decimal, buffer);
}

const unsigned char *DecimalConvertUtil::decimal128FromNetwork(
                                                  Decimal128          *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal != 0);
    return decimalFromNetworkT(decimal, buffer);
}

const unsigned char *DecimalConvertUtil::decimalFromNetwork(
                                                  Decimal32           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal != 0);
    return decimalFromNetworkT(decimal, buffer);
}

const unsigned char *DecimalConvertUtil::decimalFromNetwork(
                                                  Decimal64           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal != 0);
    return decimalFromNetworkT(decimal, buffer);
}

const unsigned char *DecimalConvertUtil::decimalFromNetwork(
                                                  Decimal128          *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal != 0);
    return decimalFromNetworkT(decimal, buffer);
}

            // Restore a Decimal Floating-Point from a Binary


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
