// bdlxxxx_bytestreamimputil.cpp                                      -*-C++-*-
#include <bdlxxxx_bytestreamimputil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlxxxx_bytestreamimputil_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_platform.h>

namespace BloombergLP {

namespace bdlxxxx {
                        // -----------------------------
                        // struct ByteStreamImpUtil
                        // -----------------------------

// CLASS METHODS

                        // *** putting arrays of integral values ***

void ByteStreamImpUtil::putArrayInt64(char                     *buffer,
                                           const bsls::Types::Int64 *array,
                                           int                       count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const bsls::Types::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt64(buffer, *array);
        buffer += 8;
    }
}

void ByteStreamImpUtil::putArrayInt64(char                      *buffer,
                                           const bsls::Types::Uint64 *array,
                                           int                        count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const bsls::Types::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt64(buffer, *array);
        buffer += 8;
    }
}

void ByteStreamImpUtil::putArrayInt56(char                     *buffer,
                                           const bsls::Types::Int64 *array,
                                           int                       count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const bsls::Types::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt56(buffer, *array);
        buffer += 7;
    }
}

void ByteStreamImpUtil::putArrayInt56(char                      *buffer,
                                           const bsls::Types::Uint64 *array,
                                           int                        count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const bsls::Types::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt56(buffer, *array);
        buffer += 7;
    }
}

void ByteStreamImpUtil::putArrayInt48(char                     *buffer,
                                           const bsls::Types::Int64 *array,
                                           int                       count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const bsls::Types::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt48(buffer, *array);
        buffer += 6;
    }
}

void ByteStreamImpUtil::putArrayInt48(char                      *buffer,
                                           const bsls::Types::Uint64 *array,
                                           int                        count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const bsls::Types::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt48(buffer, *array);
        buffer += 6;
    }
}

void ByteStreamImpUtil::putArrayInt40(char                     *buffer,
                                           const bsls::Types::Int64 *array,
                                           int                       count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const bsls::Types::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt40(buffer, *array);
        buffer += 5;
    }
}

void ByteStreamImpUtil::putArrayInt40(char                      *buffer,
                                           const bsls::Types::Uint64 *array,
                                           int                        count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const bsls::Types::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt40(buffer, *array);
        buffer += 5;
    }
}

void ByteStreamImpUtil::putArrayInt32(char      *buffer,
                                           const int *array,
                                           int        count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const int *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt32(buffer, *array);
        buffer += 4;
    }
}

void ByteStreamImpUtil::putArrayInt32(char               *buffer,
                                           const unsigned int *array,
                                           int                 count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const unsigned int *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt32(buffer, *array);
        buffer += 4;
    }
}

void ByteStreamImpUtil::putArrayInt24(char      *buffer,
                                           const int *array,
                                           int        count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const int *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt24(buffer, *array);
        buffer += 3;
    }
}

void ByteStreamImpUtil::putArrayInt24(char               *buffer,
                                           const unsigned int *array,
                                           int                 count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const unsigned int *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt24(buffer, *array);
        buffer += 3;
    }
}

void ByteStreamImpUtil::putArrayInt16(char        *buffer,
                                           const short *array,
                                           int          count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const short *endArray = array + count;
    for (; array < endArray; ++array) {
        typedef const union Dummy {
            short d_variable;
            char  d_bytes[sizeof(short)];
        } T;

        const T *value = (const T *)array;

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
        buffer[0] = value->d_bytes[1];
        buffer[1] = value->d_bytes[0];
#else
        buffer[0] = value->d_bytes[sizeof(T) - 2];
        buffer[1] = value->d_bytes[sizeof(T) - 1];
#endif
        buffer += 2;
    }
}

                        // *** putting arrays of floating-point values ***

void ByteStreamImpUtil::putArrayFloat64(char         *buffer,
                                             const double *array,
                                             int           count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const double *endArray = array + count;
    for (; array < endArray; ++array) {
        putFloat64(buffer, *array);
        buffer += 8;
    }
}

void ByteStreamImpUtil::putArrayFloat32(char        *buffer,
                                             const float *array,
                                             int          count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const float *endArray = array + count;
    for (; array < endArray; ++array) {
        putFloat32(buffer, *array);
        buffer += 4;
    }
}

                        // *** getting arrays of integral values ***

void ByteStreamImpUtil::getArrayInt64(bsls::Types::Int64 *array,
                                           const char         *buffer,
                                           int                 count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const bsls::Types::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getInt64(array, buffer);
        buffer += 8;
    }
}

void ByteStreamImpUtil::getArrayUint64(bsls::Types::Uint64 *array,
                                            const char          *buffer,
                                            int                  count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const bsls::Types::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getUint64(array, buffer);
        buffer += 8;
    }
}

void ByteStreamImpUtil::getArrayInt56(bsls::Types::Int64 *array,
                                           const char         *buffer,
                                           int                 count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const bsls::Types::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getInt56(array, buffer);
        buffer += 7;
    }
}

void ByteStreamImpUtil::getArrayUint56(bsls::Types::Uint64 *array,
                                            const char          *buffer,
                                            int                  count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const bsls::Types::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getUint56(array, buffer);
        buffer += 7;
    }
}

void ByteStreamImpUtil::getArrayInt48(bsls::Types::Int64 *array,
                                           const char         *buffer,
                                           int                 count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const bsls::Types::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getInt48(array, buffer);
        buffer += 6;
    }
}

void ByteStreamImpUtil::getArrayUint48(bsls::Types::Uint64 *array,
                                            const char          *buffer,
                                            int                  count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const bsls::Types::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getUint48(array, buffer);
        buffer += 6;
    }
}

void ByteStreamImpUtil::getArrayInt40(bsls::Types::Int64 *array,
                                           const char         *buffer,
                                           int                 count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const bsls::Types::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getInt40(array, buffer);
        buffer += 5;
    }
}

void ByteStreamImpUtil::getArrayUint40(bsls::Types::Uint64 *array,
                                            const char          *buffer,
                                            int                  count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const bsls::Types::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getUint40(array, buffer);
        buffer += 5;
    }
}

void ByteStreamImpUtil::getArrayInt32(int        *array,
                                           const char *buffer,
                                           int         count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const int *endArray = array + count;
    for (; array < endArray; ++array) {
        getInt32(array, buffer);
        buffer += 4;
    }
}

void ByteStreamImpUtil::getArrayUint32(unsigned int *array,
                                            const char   *buffer,
                                            int           count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const unsigned int *endArray = array + count;
    for (; array < endArray; ++array) {
        getUint32(array, buffer);
        buffer += 4;
    }
}

void ByteStreamImpUtil::getArrayInt24(int        *array,
                                           const char *buffer,
                                           int         count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const int *endArray = array + count;
    for (; array < endArray; ++array) {
        getInt24(array, buffer);
        buffer += 3;
    }
}

void ByteStreamImpUtil::getArrayUint24(unsigned int *array,
                                            const char   *buffer,
                                            int           count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const unsigned int *endArray = array + count;
    for (; array < endArray; ++array) {
        getUint24(array, buffer);
        buffer += 3;
    }
}

void ByteStreamImpUtil::getArrayInt16(short      *array,
                                           const char *buffer,
                                           int         count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const short *endArray = array + count;
    for (; array < endArray; ++array) {
        getInt16(array, buffer);
        buffer += 2;
    }
}

void ByteStreamImpUtil::getArrayUint16(unsigned short *array,
                                            const char     *buffer,
                                            int             count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const unsigned short *endArray = array + count;
    for (; array < endArray; ++array) {
        getUint16(array, buffer);
        buffer += 2;
    }
}

                        // *** getting arrays of floating-point values ***

void ByteStreamImpUtil::getArrayFloat64(double     *array,
                                             const char *buffer,
                                             int         count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const double *endArray = array + count;
    for (; array < endArray; ++array) {
        getFloat64(array, buffer);
        buffer += 8;
    }
}

void ByteStreamImpUtil::getArrayFloat32(float      *array,
                                             const char *buffer,
                                             int         count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const float *endArray = array + count;
    for (; array < endArray; ++array) {
        getFloat32(array, buffer);
        buffer += 4;
    }
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
