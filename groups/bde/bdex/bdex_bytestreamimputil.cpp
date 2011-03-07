// bdex_bytestreamimputil.cpp                                         -*-C++-*-
#include <bdex_bytestreamimputil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdex_bytestreamimputil_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

namespace BloombergLP {

                        // -----------------------------
                        // struct bdex_ByteStreamImpUtil
                        // -----------------------------

// CLASS METHODS

                        // *** putting arrays of integral values ***

void bdex_ByteStreamImpUtil::putArrayInt64(
                                        char                           *buffer,
                                        const bsls_PlatformUtil::Int64 *array,
                                        int                             count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const bsls_PlatformUtil::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt64(buffer, *array);
        buffer += 8;
    }
}

void bdex_ByteStreamImpUtil::putArrayInt64(
                                       char                            *buffer,
                                       const bsls_PlatformUtil::Uint64 *array,
                                       int                              count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const bsls_PlatformUtil::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt64(buffer, *array);
        buffer += 8;
    }
}

void bdex_ByteStreamImpUtil::putArrayInt56(
                                        char                           *buffer,
                                        const bsls_PlatformUtil::Int64 *array,
                                        int                             count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const bsls_PlatformUtil::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt56(buffer, *array);
        buffer += 7;
    }
}

void bdex_ByteStreamImpUtil::putArrayInt56(
                                       char                            *buffer,
                                       const bsls_PlatformUtil::Uint64 *array,
                                       int                              count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const bsls_PlatformUtil::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt56(buffer, *array);
        buffer += 7;
    }
}

void bdex_ByteStreamImpUtil::putArrayInt48(
                                        char                           *buffer,
                                        const bsls_PlatformUtil::Int64 *array,
                                        int                             count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const bsls_PlatformUtil::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt48(buffer, *array);
        buffer += 6;
    }
}

void bdex_ByteStreamImpUtil::putArrayInt48(
                                       char                            *buffer,
                                       const bsls_PlatformUtil::Uint64 *array,
                                       int                              count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const bsls_PlatformUtil::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt48(buffer, *array);
        buffer += 6;
    }
}

void bdex_ByteStreamImpUtil::putArrayInt40(
                                        char                           *buffer,
                                        const bsls_PlatformUtil::Int64 *array,
                                        int                             count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const bsls_PlatformUtil::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt40(buffer, *array);
        buffer += 5;
    }
}

void bdex_ByteStreamImpUtil::putArrayInt40(
                                       char                            *buffer,
                                       const bsls_PlatformUtil::Uint64 *array,
                                       int                              count)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= count);

    const bsls_PlatformUtil::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        putInt40(buffer, *array);
        buffer += 5;
    }
}

void bdex_ByteStreamImpUtil::putArrayInt32(char      *buffer,
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

void bdex_ByteStreamImpUtil::putArrayInt32(char               *buffer,
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

void bdex_ByteStreamImpUtil::putArrayInt24(char      *buffer,
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

void bdex_ByteStreamImpUtil::putArrayInt24(char               *buffer,
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

void bdex_ByteStreamImpUtil::putArrayInt16(char        *buffer,
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
            char d_bytes[1];
        } T;

        const T *value = (const T *)array;

#if BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
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

void bdex_ByteStreamImpUtil::putArrayFloat64(char         *buffer,
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

void bdex_ByteStreamImpUtil::putArrayFloat32(char        *buffer,
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

void bdex_ByteStreamImpUtil::getArrayInt64(bsls_PlatformUtil::Int64 *array,
                                           const char               *buffer,
                                           int                       count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const bsls_PlatformUtil::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getInt64(array, buffer);
        buffer += 8;
    }
}

void bdex_ByteStreamImpUtil::getArrayUint64(bsls_PlatformUtil::Uint64 *array,
                                            const char                *buffer,
                                            int                        count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const bsls_PlatformUtil::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getUint64(array, buffer);
        buffer += 8;
    }
}

void bdex_ByteStreamImpUtil::getArrayInt56(bsls_PlatformUtil::Int64 *array,
                                           const char               *buffer,
                                           int                       count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const bsls_PlatformUtil::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getInt56(array, buffer);
        buffer += 7;
    }
}

void bdex_ByteStreamImpUtil::getArrayUint56(bsls_PlatformUtil::Uint64 *array,
                                            const char                *buffer,
                                            int                        count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const bsls_PlatformUtil::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getUint56(array, buffer);
        buffer += 7;
    }
}

void bdex_ByteStreamImpUtil::getArrayInt48(bsls_PlatformUtil::Int64 *array,
                                           const char               *buffer,
                                           int                       count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const bsls_PlatformUtil::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getInt48(array, buffer);
        buffer += 6;
    }
}

void bdex_ByteStreamImpUtil::getArrayUint48(bsls_PlatformUtil::Uint64 *array,
                                            const char                *buffer,
                                            int                        count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const bsls_PlatformUtil::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getUint48(array, buffer);
        buffer += 6;
    }
}

void bdex_ByteStreamImpUtil::getArrayInt40(bsls_PlatformUtil::Int64 *array,
                                           const char               *buffer,
                                           int                       count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const bsls_PlatformUtil::Int64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getInt40(array, buffer);
        buffer += 5;
    }
}

void bdex_ByteStreamImpUtil::getArrayUint40(bsls_PlatformUtil::Uint64 *array,
                                            const char                *buffer,
                                            int                        count)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= count);

    const bsls_PlatformUtil::Uint64 *endArray = array + count;
    for (; array < endArray; ++array) {
        getUint40(array, buffer);
        buffer += 5;
    }
}

void bdex_ByteStreamImpUtil::getArrayInt32(int        *array,
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

void bdex_ByteStreamImpUtil::getArrayUint32(unsigned int *array,
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

void bdex_ByteStreamImpUtil::getArrayInt24(int        *array,
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

void bdex_ByteStreamImpUtil::getArrayUint24(unsigned int *array,
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

void bdex_ByteStreamImpUtil::getArrayInt16(short      *array,
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

void bdex_ByteStreamImpUtil::getArrayUint16(unsigned short *array,
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

void bdex_ByteStreamImpUtil::getArrayFloat64(double     *array,
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

void bdex_ByteStreamImpUtil::getArrayFloat32(float      *array,
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

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
