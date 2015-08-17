// bdlxxxx_testoutstreamformatter.cpp                                 -*-C++-*-
#include <bdlxxxx_testoutstreamformatter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlxxxx_testoutstreamformatter_cpp,"$Id$ $CSID$")

#include <bdlxxxx_fieldcode.h>
#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                        // ========================
                        // class bdlxxxx::TestOutStream
                        // ========================

// CONSTANTS

// CPU-independent size of data types in wire format.  (Might not be the same
// as size of data types in memory.)
const int SIZEOF_INT64   = 8;
const int SIZEOF_INT56   = 7;
const int SIZEOF_INT48   = 6;
const int SIZEOF_INT40   = 5;
const int SIZEOF_INT32   = 4;
const int SIZEOF_INT24   = 3;
const int SIZEOF_INT16   = 2;
const int SIZEOF_INT8    = 1;
const int SIZEOF_FLOAT64 = 8;
const int SIZEOF_FLOAT32 = 4;
const int SIZEOF_CODE    = 1;
const int SIZEOF_ARRLEN  = SIZEOF_INT32;

namespace bdlxxxx {
// CREATORS
TestOutStreamFormatter::TestOutStreamFormatter(
                                                     bsl::streambuf *streamBuf)
: d_streamBuf(streamBuf)
, d_imp(d_streamBuf)
, d_makeNextInvalid(0)
, d_validFlag(1)
{
    BSLS_ASSERT(streamBuf);
}

TestOutStreamFormatter::~TestOutStreamFormatter()
{
}

// MANIPULATORS
TestOutStreamFormatter&
TestOutStreamFormatter::putLength(int length)
{
    BSLS_ASSERT(0 <= length);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
        d_imp.putInt8(code);
        d_imp.putInt32(length);
    }
    else if (length > 127) {
        code = FieldCode::BDEX_INT32;
        d_imp.putInt8(code);
        d_imp.putInt32(length | (1 << 31));
    }
    else {
        code = FieldCode::BDEX_INT8;
        d_imp.putInt8(code);
        d_imp.putInt8(length);
    }
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putVersion(int version)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_UINT8;
    }
    d_imp.putInt8(code);
    unsigned char tmp = static_cast<unsigned char>(version);
    d_imp.putInt8(tmp);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putInt64(bsls::Types::Int64 value)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_INT64;
    }
    d_imp.putInt8(code);
    d_imp.putInt64(value);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putUint64(bsls::Types::Int64 value)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_UINT64;
    }
    d_imp.putInt8(code);
    d_imp.putInt64(value);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putInt56(bsls::Types::Int64 value)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_INT56;
    }
    d_imp.putInt8(code);
    d_imp.putInt56(value);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putUint56(bsls::Types::Int64 value)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_UINT56;
    }
    d_imp.putInt8(code);
    d_imp.putInt56(value);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putInt48(bsls::Types::Int64 value)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_INT48;
    }
    d_imp.putInt8(code);
    d_imp.putInt48(value);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putUint48(bsls::Types::Int64 value)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_UINT48;
    }
    d_imp.putInt8(code);
    d_imp.putInt48(value);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putInt40(bsls::Types::Int64 value)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_INT40;
    }
    d_imp.putInt8(code);
    d_imp.putInt40(value);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putUint40(bsls::Types::Int64 value)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_UINT40;
    }
    d_imp.putInt8(code);
    d_imp.putInt40(value);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putInt32(int value)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_INT32;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(value);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putUint32(unsigned int value)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_UINT32;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(value);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putInt24(int value)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_INT24;
    }
    d_imp.putInt8(code);
    d_imp.putInt24(value);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putUint24(int value)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_UINT24;
    }
    d_imp.putInt8(code);
    d_imp.putInt24(value);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putInt16(int value)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_INT16;
    }
    d_imp.putInt8(code);
    d_imp.putInt16(value);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putUint16(int value)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_UINT16;
    }
    d_imp.putInt8(code);
    d_imp.putInt16(value);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putInt8(int value)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_INT8;
    }
    d_imp.putInt8(code);
    d_imp.putInt8(value);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putUint8(int value)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_UINT8;
    }
    d_imp.putInt8(code);
    d_imp.putInt8(value);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putFloat64(double value)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_FLOAT64;
    }
    d_imp.putInt8(code);
    d_imp.putFloat64(value);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putFloat32(float value)
{
    FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = FieldCode::BDEX_FLOAT32;
    }
    d_imp.putInt8(code);
    d_imp.putFloat32(value);
    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayInt64(const bsls::Types::Int64 *array,
                                           int                       count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_INT64;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayInt64(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayUint64(const bsls::Types::Uint64 *array,
                                            int                        count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_UINT64;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayUint64(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayInt56(const bsls::Types::Int64 *array,
                                           int                       count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_INT56;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayInt56(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayUint56(const bsls::Types::Uint64 *array,
                                            int                        count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_UINT56;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayUint56(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayInt48(const bsls::Types::Int64 *array,
                                           int                       count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_INT48;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayInt48(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayUint48(const bsls::Types::Uint64 *array,
                                            int                        count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_UINT48;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayUint48(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayInt40(const bsls::Types::Int64 *array,
                                           int                       count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_INT40;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayInt40(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayUint40(const bsls::Types::Uint64 *array,
                                            int                        count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_UINT40;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayUint40(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayInt32(const int *array, int count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

     FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_INT32;
    }
     d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayInt32(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayUint32(const unsigned int *array,
                                            int                 count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_UINT32;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayUint32(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayInt24(const int *array, int count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_INT24;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayInt24(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayUint24(const unsigned int *array,
                                            int                 count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_UINT24;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayUint24(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayInt16(const short *array, int count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_INT16;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayInt16(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayUint16(const unsigned short *array,
                                            int                   count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_UINT16;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayUint16(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayInt8(const char *array, int count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_INT8;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayInt8(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayInt8(const signed char *array, int count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_INT8;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayInt8(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayUint8(const char *array, int count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_UINT8;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayUint8(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayUint8(const unsigned char *array,
                                           int                  count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_UINT8;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayUint8(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayFloat64(const double *array, int count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_FLOAT64;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayFloat64(array, count);
    }

    return *this;
}

TestOutStreamFormatter&
TestOutStreamFormatter::putArrayFloat32(const float *array, int count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = FieldCode::BDEX_INVALID;
    }
    else {
        code = FieldCode::BDEX_FLOAT32;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count) {
        d_imp.putArrayFloat32(array, count);
    }

    return *this;
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
