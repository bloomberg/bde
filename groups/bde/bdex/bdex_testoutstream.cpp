// bdex_testoutstream.cpp                                             -*-C++-*-
#include <bdex_testoutstream.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdex_testoutstream_cpp,"$Id$ $CSID$")


#include <bdex_bytestreamimputil.h>
#include <bdex_fieldcode.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_ostream.h>

// CONSTANTS

namespace BloombergLP {

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

bdex_TestOutStream::bdex_TestOutStream(bslma_Allocator *basicAllocator)
: d_streambuf(basicAllocator)
, d_formatter(&d_streambuf)
{
}

bdex_TestOutStream::~bdex_TestOutStream()
{
}

// MANIPULATORS

bdex_TestOutStream&
bdex_TestOutStream::putLength(int length)
{
    d_formatter.putLength(length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putVersion(int version)
{
    d_formatter.putVersion(version);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putInt64(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putInt64(value);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putUint64(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putUint64(value);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putInt56(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putInt56(value);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putUint56(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putUint56(value);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putInt48(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putInt48(value);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putUint48(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putUint48(value);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putInt40(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putInt40(value);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putUint40(bsls_PlatformUtil::Int64 value)
{
    d_formatter.putUint40(value);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putInt32(int value)
{
    d_formatter.putInt32(value);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putUint32(unsigned int value)
{
    d_formatter.putUint32(value);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putInt24(int value)
{
    d_formatter.putInt24(value);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putUint24(int value)
{
    d_formatter.putUint24(value);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putInt16(int value)
{
    d_formatter.putInt16(value);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putUint16(int value)
{
    d_formatter.putUint16(value);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putInt8(int value)
{
    d_formatter.putInt8(value);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putUint8(int value)
{
    d_formatter.putUint8(value);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putFloat64(double value)
{
    d_formatter.putFloat64(value);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putFloat32(float value)
{
    d_formatter.putFloat32(value);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayInt64(const bsls_PlatformUtil::Int64 *array,
                                  int                             length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayInt64(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayUint64(const bsls_PlatformUtil::Uint64 *array,
                                   int                              length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayUint64(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayInt56(const bsls_PlatformUtil::Int64 *array,
                                  int                             length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayInt56(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayUint56(const bsls_PlatformUtil::Uint64 *array,
                                   int                             length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayUint56(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayInt48(const bsls_PlatformUtil::Int64 *array,
                                  int                             length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayInt48(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayUint48(const bsls_PlatformUtil::Uint64 *array,
                                   int                              length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayUint48(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayInt40(const bsls_PlatformUtil::Int64 *array,
                                  int                             length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayInt40(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayUint40(const bsls_PlatformUtil::Uint64 *array,
                                   int                              length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayUint40(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayInt32(const int *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayInt32(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayUint32(const unsigned int *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayUint32(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayInt24(const int *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayInt24(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayUint24(const unsigned int *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayUint24(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayInt16(const short *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayInt16(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayUint16(const unsigned short *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayUint16(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayInt8(const char *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayInt8(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayInt8(const signed char *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayInt8(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayUint8(const char *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayUint8(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayUint8(const unsigned char *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayUint8(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayFloat64(const double *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayFloat64(array, length);
    return *this;
}

bdex_TestOutStream&
bdex_TestOutStream::putArrayFloat32(const float *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    d_formatter.putArrayFloat32(array, length);
    return *this;
}

// FREE OPERATORS

bsl::ostream& operator<<(bsl::ostream& stream, const bdex_TestOutStream& obj)
{
    const int   len  = obj.d_streambuf.length();
    const char *data = obj.d_streambuf.data();

    bsl::ios::fmtflags flags = stream.flags();
    stream << bsl::hex;
    for (int i = 0; i < len; i++) {
        if (0 < i && 0 != i % 8) {
            stream << ' ';
        }
        if (0 == i % 8) {  // print new line and address after 8 bytes
            stream << '\n' << bsl::setw(4) << bsl::setfill('0') << i << '\t';
        }

        char array[8];
        for (int j = 7; j >= 0; j--) {
            array[7-j] = '0' + ((data[i] >> j) & 0x01);
        }

        stream.write(array, sizeof array);
    }
    stream.flags(flags);  // reset stream format flags
    return stream;
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
