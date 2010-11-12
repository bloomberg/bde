// bdex_testinstream.cpp                                              -*-C++-*-
#include <bdex_testinstream.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdex_testinstream_cpp,"$Id$ $CSID$")


#include <bdex_bytestreamimputil.h>
#include <bdex_fieldcode.h>
#include <bdex_testinstreamexception.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_ostream.h>

#ifdef FOR_TESTING_ONLY
#include <bdex_testoutstream.h>
#endif

namespace BloombergLP {

// CREATORS

bdex_TestInStream::bdex_TestInStream(bslma_Allocator *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_length(0)
, d_buffer_p(0)
, d_streambuf(d_buffer_p, 0)
, d_formatter(&d_streambuf)
{
    // IMPORTANT: d_allocator_p, d_buffer_p, d_streambuf and d_formatter
    // must be all be initialized in the order shown here.  Don't
    // re-arrange their declarations in the class definition.
    BSLS_ASSERT(d_allocator_p);
}

bdex_TestInStream::bdex_TestInStream(const char      *buffer,
                                     int              numBytes,
                                     bslma_Allocator *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_length(0)
, d_buffer_p(0)
, d_streambuf(0, 0)
, d_formatter(&d_streambuf)
{
    reload(buffer, numBytes);
}

bdex_TestInStream::~bdex_TestInStream()
{
    BSLS_ASSERT(d_allocator_p);

    if (d_buffer_p) {
        // Must disconnect 'd_buffer_p' from stream before deallocating.
        d_streambuf.pubsetbuf((char *)0, 0);
        d_allocator_p->deallocate(d_buffer_p);
    }
}

// MANIPULATORS

void bdex_TestInStream::reload(const char *buffer, int numBytes)
{
    char *newBuffer = (char *)d_allocator_p->allocate(numBytes);
    if (d_buffer_p) {
        d_allocator_p->deallocate(d_buffer_p);
    }
    d_buffer_p = newBuffer;
    bsl::memcpy(d_buffer_p, buffer, numBytes);
    d_formatter.reload(d_buffer_p, numBytes);
    d_length = numBytes;
}

bdex_TestInStream&
bdex_TestInStream::getLength(int& variable)
{
    d_formatter.getLength(variable);
    return *this;
}

bdex_TestInStream& bdex_TestInStream::getString(bsl::string& str)
{
    d_formatter.getString(str);
    return *this;
}

bdex_TestInStream& bdex_TestInStream::getVersion(int& variable)
{
    d_formatter.getVersion(variable);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getInt64(bsls_PlatformUtil::Int64& variable)
{
    d_formatter.getInt64(variable);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getUint64(bsls_PlatformUtil::Uint64& variable)
{
    d_formatter.getUint64(variable);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getInt56(bsls_PlatformUtil::Int64& variable)
{
    d_formatter.getInt56(variable);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getUint56(bsls_PlatformUtil::Uint64& variable)
{
    d_formatter.getUint56(variable);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getInt48(bsls_PlatformUtil::Int64& variable)
{
    d_formatter.getInt48(variable);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getUint48(bsls_PlatformUtil::Uint64& variable)
{
    d_formatter.getUint48(variable);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getInt40(bsls_PlatformUtil::Int64& variable)
{
    d_formatter.getInt40(variable);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getUint40(bsls_PlatformUtil::Uint64& variable)
{
    d_formatter.getUint40(variable);
    return *this;
}

bdex_TestInStream& bdex_TestInStream::getInt32(int& variable)
{
    d_formatter.getInt32(variable);
    return *this;
}

bdex_TestInStream& bdex_TestInStream::getUint32(unsigned int& variable)
{
    d_formatter.getUint32(variable);
    return *this;
}

bdex_TestInStream& bdex_TestInStream::getInt24(int& variable)
{
    d_formatter.getInt24(variable);
    return *this;
}

bdex_TestInStream& bdex_TestInStream::getUint24(unsigned int& variable)
{
    d_formatter.getUint24(variable);
    return *this;
}

bdex_TestInStream& bdex_TestInStream::getInt16(short& variable)
{
    d_formatter.getInt16(variable);
    return *this;
}

bdex_TestInStream& bdex_TestInStream::getUint16(unsigned short& variable)
{
    d_formatter.getUint16(variable);
    return *this;
}

bdex_TestInStream& bdex_TestInStream::getInt8(char& variable)
{
    d_formatter.getInt8(variable);
    return *this;
}

bdex_TestInStream& bdex_TestInStream::getInt8(signed char& variable)
{
    d_formatter.getInt8(variable);
    return *this;
}

bdex_TestInStream& bdex_TestInStream::getUint8(char& variable)
{
    d_formatter.getUint8(variable);
    return *this;
}

bdex_TestInStream& bdex_TestInStream::getUint8(unsigned char& variable)
{
    d_formatter.getUint8(variable);
    return *this;
}

bdex_TestInStream& bdex_TestInStream::getFloat64(double& variable)
{
    d_formatter.getFloat64(variable);
    return *this;
}

bdex_TestInStream& bdex_TestInStream::getFloat32(float& variable)
{
    d_formatter.getFloat32(variable);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayInt64(bsls_PlatformUtil::Int64 *array,
                                 int                       length)
{
    d_formatter.getArrayInt64(array, length);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayUint64(bsls_PlatformUtil::Uint64 *array,
                                  int                        length)
{
    d_formatter.getArrayUint64(array, length);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayInt56(bsls_PlatformUtil::Int64 *array,
                                 int                       length)
{
    d_formatter.getArrayInt56(array, length);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayUint56(bsls_PlatformUtil::Uint64 *array,
                                  int                        length)
{
    d_formatter.getArrayUint56(array, length);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayInt48(bsls_PlatformUtil::Int64 *array,
                                 int                       length)
{
    d_formatter.getArrayInt48(array, length);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayUint48(bsls_PlatformUtil::Uint64 *array,
                                  int                        length)
{
    d_formatter.getArrayUint48(array, length);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayInt40(bsls_PlatformUtil::Int64 *array,
                                 int                       length)
{
    d_formatter.getArrayInt40(array, length);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayUint40(bsls_PlatformUtil::Uint64 *array,
                                  int                        length)
{
    d_formatter.getArrayUint40(array, length);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayInt32(int *array, int length)
{
    d_formatter.getArrayInt32(array, length);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayUint32(unsigned int *array, int length)
{
    d_formatter.getArrayUint32(array, length);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayInt24(int *array, int length)
{
    d_formatter.getArrayInt24(array, length);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayUint24(unsigned int *array, int length)
{
    d_formatter.getArrayUint24(array, length);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayInt16(short *array, int length)
{
    d_formatter.getArrayInt16(array, length);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayUint16(unsigned short *array, int length)
{
    d_formatter.getArrayUint16(array, length);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayInt8(char *array, int length)
{
    d_formatter.getArrayInt8(array, length);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayInt8(signed char *array, int length)
{
    d_formatter.getArrayInt8(array, length);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayUint8(char *array, int length)
{
    d_formatter.getArrayUint8(array, length);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayUint8(unsigned char *array, int length)
{
    d_formatter.getArrayUint8(array, length);
    return *this;
}

bdex_TestInStream&
bdex_TestInStream::getArrayFloat64(double *array, int length)
{
    d_formatter.getArrayFloat64(array, length);
    return *this;
}

bdex_TestInStream& bdex_TestInStream::getArrayFloat32(float *array,
                                                      int    length)
{
    d_formatter.getArrayFloat32(array, length);
    return *this;
}

// FREE OPERATORS

bsl::ostream& operator<<(bsl::ostream& stream, const bdex_TestInStream& obj)
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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
