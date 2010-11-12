// bdex_testoutstreamformatter.cpp               -*-C++-*-
#include <bdex_testoutstreamformatter.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdex_testoutstreamformatter_cpp,"$Id$ $CSID$")

#include <bdex_fieldcode.h>
#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                        // ========================
                        // class bdex_TestOutStream
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

// CREATORS
bdex_TestOutStreamFormatter::bdex_TestOutStreamFormatter(
                                                     bsl::streambuf *streamBuf)
: d_streamBuf(streamBuf)
, d_imp(d_streamBuf)
, d_makeNextInvalid(0)
, d_validFlag(1)
{
}

bdex_TestOutStreamFormatter::~bdex_TestOutStreamFormatter()
{
}

// MANIPULATORS
bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putLength(int length)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
        d_imp.putInt8(code);
        d_imp.putInt32(length);
    }
    else if (length > 127) {
        code = bdex_FieldCode::BDEX_INT32;
        d_imp.putInt8(code);
        d_imp.putInt32(length | (1 << 31));
    }
    else {
        code = bdex_FieldCode::BDEX_INT8;
        d_imp.putInt8(code);
        d_imp.putInt8(length);
    }
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putVersion(int version)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_UINT8;
    }
    d_imp.putInt8(code);
    unsigned char tmp = version;
    d_imp.putInt8(tmp);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putInt64(bsls_PlatformUtil::Int64 value)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_INT64;
    }
    d_imp.putInt8(code);
    d_imp.putInt64(value);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putUint64(bsls_PlatformUtil::Int64 value)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_UINT64;
    }
    d_imp.putInt8(code);
    d_imp.putInt64(value);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putInt56(bsls_PlatformUtil::Int64 value)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_INT56;
    }
    d_imp.putInt8(code);
    d_imp.putInt56(value);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putUint56(bsls_PlatformUtil::Int64 value)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_UINT56;
    }
    d_imp.putInt8(code);
    d_imp.putInt56(value);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putInt48(bsls_PlatformUtil::Int64 value)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_INT48;
    }
    d_imp.putInt8(code);
    d_imp.putInt48(value);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putUint48(bsls_PlatformUtil::Int64 value)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_UINT48;
    }
    d_imp.putInt8(code);
    d_imp.putInt48(value);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putInt40(bsls_PlatformUtil::Int64 value)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_INT40;
    }
    d_imp.putInt8(code);
    d_imp.putInt40(value);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putUint40(bsls_PlatformUtil::Int64 value)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_UINT40;
    }
    d_imp.putInt8(code);
    d_imp.putInt40(value);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putInt32(int value)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_INT32;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(value);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putUint32(unsigned int value)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_UINT32;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(value);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putInt24(int value)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_INT24;
    }
    d_imp.putInt8(code);
    d_imp.putInt24(value);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putUint24(int value)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_UINT24;
    }
    d_imp.putInt8(code);
    d_imp.putInt24(value);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putInt16(int value)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_INT16;
    }
    d_imp.putInt8(code);
    d_imp.putInt16(value);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putUint16(int value)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_UINT16;
    }
    d_imp.putInt8(code);
    d_imp.putInt16(value);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putInt8(int value)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_INT8;
    }
    d_imp.putInt8(code);
    d_imp.putInt8(value);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putUint8(int value)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_UINT8;
    }
    d_imp.putInt8(code);
    d_imp.putInt8(value);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putFloat64(double value)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_FLOAT64;
    }
    d_imp.putInt8(code);
    d_imp.putFloat64(value);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putFloat32(float value)
{
    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        code = bdex_FieldCode::BDEX_INVALID;
        d_makeNextInvalid = 0;
    }
    else {
        code = bdex_FieldCode::BDEX_FLOAT32;
    }
    d_imp.putInt8(code);
    d_imp.putFloat32(value);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayInt64(
                                        const bsls_PlatformUtil::Int64 *array,
                                        int                             count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_INT64;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayInt64(array, count);

    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayUint64(
                                       const bsls_PlatformUtil::Uint64 *array,
                                       int                              count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_UINT64;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayUint64(array, count);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayInt56(
                                        const bsls_PlatformUtil::Int64 *array,
                                        int                             count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_INT56;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayInt56(array, count);

    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayUint56(
                                       const bsls_PlatformUtil::Uint64 *array,
                                       int                              count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_UINT56;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayUint56(array, count);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayInt48(
                                        const bsls_PlatformUtil::Int64 *array,
                                        int                             count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_INT48;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayInt48(array, count);

    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayUint48(
                                       const bsls_PlatformUtil::Uint64 *array,
                                       int                              count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_UINT48;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayUint48(array, count);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayInt40(
                                        const bsls_PlatformUtil::Int64 *array,
                                        int                             count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_INT40;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayInt40(array, count);

    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayUint40(
                                      const bsls_PlatformUtil::Uint64 *array,
                                      int                              count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_UINT40;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayUint40(array, count);
    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayInt32(const int *array,
                                           int        count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

     bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_INT32;
    }
     d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayInt32(array, count);

    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayUint32(const unsigned int *array,
                                            int                 count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_UINT32;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayUint32(array, count);

        return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayInt24(const int *array,
                                           int        count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_INT24;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayInt24(array, count);

    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayUint24(const unsigned int *array,
                                            int                 count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_UINT24;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayUint24(array, count);

        return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayInt16(const short *array,
                                           int          count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_INT16;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayInt16(array, count);

    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayUint16(const unsigned short *array,
                                            int                   count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_UINT16;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayUint16(array, count);

    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayInt8(const char *array,
                                          int         count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_INT8;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayInt8(array, count);

    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayInt8(const signed char *array, int count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_INT8;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayInt8(array, count);

    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayUint8(const char *array,
                                           int         count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_UINT8;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayUint8(array, count);

    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayUint8(const unsigned char *array,
                                           int                  count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_UINT8;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayUint8(array, count);

    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayFloat64(const double *array,
                                             int           count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_FLOAT64;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayFloat64(array, count);

    return *this;
}

bdex_TestOutStreamFormatter&
bdex_TestOutStreamFormatter::putArrayFloat32(const float *array,
                                             int          count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    bdex_FieldCode::Type code;
    if (d_makeNextInvalid) {
        d_makeNextInvalid = 0;
        code = bdex_FieldCode::BDEX_INVALID;
    }
    else {
        code = bdex_FieldCode::BDEX_FLOAT32;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(count);
    if (0 < count)
        d_imp.putArrayFloat32(array, count);

    return *this;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
