// bslx_testoutstream.cpp                                             -*-C++-*-
#include <bslx_testoutstream.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslx_testoutstream_cpp,"$Id$ $CSID$")

#include <bslx_typecode.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace bslx {

                        // -------------------
                        // class TestOutStream
                        // -------------------

// CREATORS
TestOutStream::TestOutStream(int               versionSelector,
                             bslma::Allocator *basicAllocator)
: d_imp(versionSelector, basicAllocator)
, d_makeNextInvalidFlag(0)
{
}

TestOutStream::TestOutStream(int               versionSelector,
                             bsl::size_t       initialCapacity,
                             bslma::Allocator *basicAllocator)
: d_imp(versionSelector, basicAllocator)
, d_makeNextInvalidFlag(0)
{
    d_imp.reserveCapacity(initialCapacity);
}

TestOutStream::~TestOutStream()
{
}

// MANIPULATORS
TestOutStream& TestOutStream::putLength(int length)
{
    BSLS_ASSERT(0 <= length);

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        d_imp.putInt8(TypeCode::e_INVALID);
        d_imp.putInt32(length);
    } else if (length > 127) {
        d_imp.putInt8(TypeCode::e_INT32);
        d_imp.putInt32(length | (1 << 31));
    } else {
        d_imp.putInt8(TypeCode::e_INT8);
        d_imp.putInt8(length);
    }

    return *this;
}

TestOutStream& TestOutStream::putVersion(int version)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_UINT8;
    }

    d_imp.putInt8(code);
    d_imp.putUint8(static_cast<unsigned char>(version));

    return *this;
}

                      // *** scalar integer values ***

TestOutStream& TestOutStream::putInt64(bsls::Types::Int64 value)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_INT64;
    }

    d_imp.putInt8(code);
    d_imp.putInt64(value);

    return *this;
}

TestOutStream& TestOutStream::putUint64(bsls::Types::Uint64 value)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_UINT64;
    }

    d_imp.putInt8(code);
    d_imp.putUint64(value);

    return *this;
}

TestOutStream& TestOutStream::putInt56(bsls::Types::Int64 value)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_INT56;
    }

    d_imp.putInt8(code);
    d_imp.putInt56(value);

    return *this;
}

TestOutStream& TestOutStream::putUint56(bsls::Types::Uint64 value)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_UINT56;
    }

    d_imp.putInt8(code);
    d_imp.putUint56(value);

    return *this;
}

TestOutStream& TestOutStream::putInt48(bsls::Types::Int64 value)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_INT48;
    }

    d_imp.putInt8(code);
    d_imp.putInt48(value);

    return *this;
}

TestOutStream& TestOutStream::putUint48(bsls::Types::Uint64 value)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_UINT48;
    }

    d_imp.putInt8(code);
    d_imp.putUint48(value);

    return *this;
}

TestOutStream& TestOutStream::putInt40(bsls::Types::Int64 value)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_INT40;
    }

    d_imp.putInt8(code);
    d_imp.putInt40(value);

    return *this;
}

TestOutStream& TestOutStream::putUint40(bsls::Types::Uint64 value)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_UINT40;
    }

    d_imp.putInt8(code);
    d_imp.putUint40(value);

    return *this;
}

TestOutStream& TestOutStream::putInt32(int value)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_INT32;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(value);

    return *this;
}

TestOutStream& TestOutStream::putUint32(unsigned int value)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_UINT32;
    }

    d_imp.putInt8(code);
    d_imp.putUint32(value);

    return *this;
}

TestOutStream& TestOutStream::putInt24(int value)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_INT24;
    }

    d_imp.putInt8(code);
    d_imp.putInt24(value);

    return *this;
}

TestOutStream& TestOutStream::putUint24(unsigned int value)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_UINT24;
    }

    d_imp.putInt8(code);
    d_imp.putUint24(value);

    return *this;
}

TestOutStream& TestOutStream::putInt16(int value)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_INT16;
    }

    d_imp.putInt8(code);
    d_imp.putInt16(value);

    return *this;
}

TestOutStream& TestOutStream::putUint16(unsigned int value)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_UINT16;
    }

    d_imp.putInt8(code);
    d_imp.putUint16(value);

    return *this;
}

TestOutStream& TestOutStream::putInt8(int value)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_INT8;
    }

    d_imp.putInt8(code);
    d_imp.putInt8(value);

    return *this;
}

TestOutStream& TestOutStream::putUint8(unsigned int value)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_UINT8;
    }

    d_imp.putInt8(code);
    d_imp.putUint8(value);

    return *this;
}

                      // *** scalar floating-point values ***

TestOutStream& TestOutStream::putFloat64(double value)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_FLOAT64;
    }

    d_imp.putInt8(code);
    d_imp.putFloat64(value);

    return *this;
}

TestOutStream& TestOutStream::putFloat32(float value)
{
    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        code = TypeCode::e_INVALID;
        d_makeNextInvalidFlag = 0;
    } else {
        code = TypeCode::e_FLOAT32;
    }

    d_imp.putInt8(code);
    d_imp.putFloat32(value);

    return *this;
}

                      // *** arrays of integer values ***

TestOutStream& TestOutStream::putArrayInt64(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_INT64;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayInt64(values, numValues);

    return *this;
}

TestOutStream& TestOutStream::putArrayUint64(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_UINT64;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayUint64(values, numValues);

    return *this;
}

TestOutStream& TestOutStream::putArrayInt56(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_INT56;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayInt56(values, numValues);

    return *this;
}

TestOutStream& TestOutStream::putArrayUint56(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_UINT56;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayUint56(values, numValues);

    return *this;
}

TestOutStream& TestOutStream::putArrayInt48(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_INT48;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayInt48(values, numValues);

    return *this;
}

TestOutStream& TestOutStream::putArrayUint48(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_UINT48;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayUint48(values, numValues);

    return *this;
}

TestOutStream& TestOutStream::putArrayInt40(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_INT40;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayInt40(values, numValues);

    return *this;
}

TestOutStream& TestOutStream::putArrayUint40(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_UINT40;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayUint40(values, numValues);

    return *this;
}

TestOutStream& TestOutStream::putArrayInt32(const int *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_INT32;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayInt32(values, numValues);

    return *this;
}

TestOutStream& TestOutStream::putArrayUint32(const unsigned int *values,
                                             int                 numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_UINT32;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayUint32(values, numValues);

    return *this;
}

TestOutStream& TestOutStream::putArrayInt24(const int *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_INT24;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayInt24(values, numValues);

    return *this;
}

TestOutStream& TestOutStream::putArrayUint24(const unsigned int *values,
                                             int                 numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_UINT24;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayUint24(values, numValues);

    return *this;
}

TestOutStream& TestOutStream::putArrayInt16(const short *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_INT16;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayInt16(values, numValues);

    return *this;
}

TestOutStream& TestOutStream::putArrayUint16(const unsigned short *values,
                                             int                   numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_UINT16;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayUint16(values, numValues);

    return *this;
}

TestOutStream& TestOutStream::putArrayInt8(const char *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_INT8;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayInt8(values, numValues);

    return *this;
}

TestOutStream& TestOutStream::putArrayInt8(const signed char *values,
                                           int                numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_INT8;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayInt8(values, numValues);

    return *this;
}

TestOutStream& TestOutStream::putArrayUint8(const char *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_UINT8;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayUint8(values, numValues);

    return *this;
}

TestOutStream& TestOutStream::putArrayUint8(const unsigned char *values,
                                            int                  numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_UINT8;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayUint8(values, numValues);

    return *this;
}

                      // *** arrays of floating-point values ***

TestOutStream& TestOutStream::putArrayFloat64(const double *values,
                                              int           numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_FLOAT64;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayFloat64(values, numValues);

    return *this;
}

TestOutStream& TestOutStream::putArrayFloat32(const float *values,
                                              int          numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    TypeCode::Enum code;

    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = false;
        code = TypeCode::e_INVALID;
    } else {
        code = TypeCode::e_FLOAT32;
    }

    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    d_imp.putArrayFloat32(values, numValues);

    return *this;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const TestOutStream& object)
{
    return stream << object.d_imp;
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
