// bslx_testinstream.cpp                                              -*-C++-*-
#include <bslx_testinstream.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslx_testinstream_cpp,"$Id$ $CSID$")

#include <bslx_marshallingutil.h>
#include <bslx_testoutstream.h>                 // for testing only

#include <bsls_assert.h>

#include <bsl_cstdio.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace bslx {

namespace {

typedef MarshallingUtil Util;

enum {
    // Enumerate the sizes (in bytes) of auxiliary data elements streamed in
    // from a 'TestInStream'.
    k_SIZEOF_CODE    = Util::k_SIZEOF_INT8,   // size of type code value
    k_SIZEOF_VERSION = Util::k_SIZEOF_INT8,   // size of version value
    k_SIZEOF_ARRLEN  = Util::k_SIZEOF_INT32   // size of array length value
};

}  // close unnamed namespace

                        // ------------------
                        // class TestInStream
                        // ------------------

// PRIVATE MANIPULATORS
void TestInStream::checkArray(TypeCode::Enum code,
                              int            elementSize,
                              int            numElements)
{
    BSLS_ASSERT(0 < elementSize);
    BSLS_ASSERT(0 <= numElements);

    checkTypeCodeAndAvailableLength(
                            code, k_SIZEOF_ARRLEN + elementSize * numElements);

    if (!isValid()) {
        return;                                                       // RETURN
    }

    int numElemFound = 0;
    Util::getInt32(&numElemFound, d_buffer + cursor());
    d_cursor += k_SIZEOF_ARRLEN;
    if (numElemFound != numElements) {
        if (!d_quietFlag) {
            bsl::fprintf(stderr,
                         "*** TestInStream: Expecting array length %d"
                         ", but found length %d ***\n",
                         numElements,
                         numElemFound);
        }
        invalidate();
        return;                                                       // RETURN
    }
}

void TestInStream::checkTypeCodeAndAvailableLength(
                                               TypeCode::Enum code,
                                               bsl::size_t    numExpectedBytes)
{
    BSLS_ASSERT(0 < numExpectedBytes);

    if (!isValid()) {
        return;                                                       // RETURN
    }

    if (length() - cursor() >= k_SIZEOF_CODE + numExpectedBytes) {
        unsigned char codeFound;  // 'unsigned' to support type values >= 128
        Util::getInt8(&codeFound, d_buffer + cursor());
        d_cursor += k_SIZEOF_CODE;
        if (codeFound != code) {
            if (!d_quietFlag) {
                bsl::fprintf(stderr,
                             "*** TestInStream: Expecting type %s"
                             ", but found type %s ***\n",
                             TypeCode::toAscii(code),
                             TypeCode::toAscii(
                                      static_cast<TypeCode::Enum>(codeFound)));
            }
            invalidate();
            return;                                                   // RETURN
        }
    }
    else {
        // Incomplete stream.
        invalidate();
        return;                                                       // RETURN
    }
}

// CREATORS
TestInStream::TestInStream()
: d_buffer()
, d_numBytes(0)
, d_validFlag(1)
, d_quietFlag(0)
, d_inputLimit(-1)
, d_cursor(0)
{
}

TestInStream::TestInStream(const char *buffer, bsl::size_t numBytes)
: d_buffer(buffer)
, d_numBytes(numBytes)
, d_validFlag(1)
, d_quietFlag(0)
, d_inputLimit(-1)
, d_cursor(0)
{
    BSLS_ASSERT(buffer || 0 == numBytes);
}

TestInStream::TestInStream(const bslstl::StringRef& srcData)
: d_buffer(srcData.data())
, d_numBytes(static_cast<int>(srcData.length()))
, d_validFlag(1)
, d_quietFlag(0)
, d_inputLimit(-1)
, d_cursor(0)
{
}

TestInStream::~TestInStream()
{
}

// MANIPULATORS
TestInStream& TestInStream::getLength(int& variable)
{
    if (length() - cursor() < k_SIZEOF_CODE + Util::k_SIZEOF_INT8) {
        invalidate();  // Incomplete stream.  Invalidate silently.
    }
    else {
        if (127 <
            static_cast<unsigned char>(d_buffer[cursor() + k_SIZEOF_CODE])) {
            // If 'length > 127', 'length' is stored as 4 bytes with top-bit
            // set.

            if (isValid()) {
                getInt32(variable);
                variable &= 0x7fffff;  // Clear top bit.
            }
        }
        else {
            // If 'length <= 127', 'length' is stored as one byte.

            if (isValid()) {
                char tmp;
                getInt8(tmp);
                variable = tmp;
            }
        }
    }

    return *this;
}

TestInStream& TestInStream::getVersion(int& variable)
{
    if (isValid()) {
        unsigned char tmp;
        getUint8(tmp);
        if (isValid()) {
            variable = tmp;
        }
    }

    return *this;
}

                      // *** scalar integer values ***

TestInStream& TestInStream::getInt64(bsls::Types::Int64& variable)
{
    TypeCode::Enum code = TypeCode::e_INT64;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_INT64);

    if (isValid()) {
        Util::getInt64(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_INT64;
    }

    return *this;
}

TestInStream& TestInStream::getUint64(bsls::Types::Uint64& variable)
{
    TypeCode::Enum code = TypeCode::e_UINT64;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_INT64);

    if (isValid()) {
        Util::getUint64(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_INT64;
    }

    return *this;
}

TestInStream& TestInStream::getInt56(bsls::Types::Int64& variable)
{
    TypeCode::Enum code = TypeCode::e_INT56;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_INT56);

    if (isValid()) {
        Util::getInt56(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_INT56;
    }

    return *this;
}

TestInStream& TestInStream::getUint56(bsls::Types::Uint64& variable)
{
    TypeCode::Enum code = TypeCode::e_UINT56;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_INT56);

    if (isValid()) {
        Util::getUint56(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_INT56;
    }

    return *this;
}

TestInStream& TestInStream::getInt48(bsls::Types::Int64& variable)
{
    TypeCode::Enum code = TypeCode::e_INT48;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_INT48);

    if (isValid()) {
        Util::getInt48(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_INT48;
    }

    return *this;
}

TestInStream& TestInStream::getUint48(bsls::Types::Uint64& variable)
{
    TypeCode::Enum code = TypeCode::e_UINT48;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_INT48);

    if (isValid()) {
        Util::getUint48(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_INT48;
    }

    return *this;
}

TestInStream& TestInStream::getInt40(bsls::Types::Int64& variable)
{
    TypeCode::Enum code = TypeCode::e_INT40;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_INT40);

    if (isValid()) {
        Util::getInt40(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_INT40;
    }

    return *this;
}

TestInStream& TestInStream::getUint40(bsls::Types::Uint64& variable)
{
    TypeCode::Enum code = TypeCode::e_UINT40;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_INT40);

    if (isValid()) {
        Util::getUint40(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_INT40;
    }

    return *this;
}

TestInStream& TestInStream::getInt32(int& variable)
{
    TypeCode::Enum code = TypeCode::e_INT32;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_INT32);

    if (isValid()) {
        Util::getInt32(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_INT32;
    }

    return *this;
}

TestInStream& TestInStream::getUint32(unsigned int& variable)
{
    TypeCode::Enum code = TypeCode::e_UINT32;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_INT32);

    if (isValid()) {
        Util::getUint32(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_INT32;
    }

    return *this;
}

TestInStream& TestInStream::getInt24(int& variable)
{
    TypeCode::Enum code = TypeCode::e_INT24;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_INT24);

    if (isValid()) {
        Util::getInt24(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_INT24;
    }

    return *this;
}

TestInStream& TestInStream::getUint24(unsigned int& variable)
{
    TypeCode::Enum code = TypeCode::e_UINT24;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_INT24);

    if (isValid()) {
        Util::getUint24(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_INT24;
    }

    return *this;
}

TestInStream& TestInStream::getInt16(short& variable)
{
    TypeCode::Enum code = TypeCode::e_INT16;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_INT16);

    if (isValid()) {
        Util::getInt16(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_INT16;
    }

    return *this;
}

TestInStream& TestInStream::getUint16(unsigned short& variable)
{
    TypeCode::Enum code = TypeCode::e_UINT16;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_INT16);

    if (isValid()) {
        Util::getUint16(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_INT16;
    }

    return *this;
}

TestInStream& TestInStream::getInt8(char& variable)
{
    TypeCode::Enum code = TypeCode::e_INT8;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_INT8);

    if (isValid()) {
        Util::getInt8(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_INT8;
    }

    return *this;
}

TestInStream& TestInStream::getInt8(signed char& variable)
{
    TypeCode::Enum code = TypeCode::e_INT8;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_INT8);

    if (isValid()) {
        Util::getInt8(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_INT8;
    }

    return *this;
}

TestInStream& TestInStream::getUint8(char& variable)
{
    TypeCode::Enum code = TypeCode::e_UINT8;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_INT8);

    if (isValid()) {
        Util::getInt8(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_INT8;
    }

    return *this;
}

TestInStream& TestInStream::getUint8(unsigned char& variable)
{
    TypeCode::Enum code = TypeCode::e_UINT8;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_INT8);

    if (isValid()) {
        Util::getInt8(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_INT8;
    }

    return *this;
}

                      // *** scalar floating-point values ***

TestInStream& TestInStream::getFloat64(double& variable)
{
    TypeCode::Enum code = TypeCode::e_FLOAT64;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_FLOAT64);

    if (isValid()) {
        Util::getFloat64(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_FLOAT64;
    }

    return *this;
}

TestInStream& TestInStream::getFloat32(float& variable)
{
    TypeCode::Enum code = TypeCode::e_FLOAT32;
    throwExceptionIfInputLimitExhausted(code);
    checkTypeCodeAndAvailableLength(code, Util::k_SIZEOF_FLOAT32);

    if (isValid()) {
        Util::getFloat32(&variable, d_buffer + cursor());
        d_cursor += Util::k_SIZEOF_FLOAT32;
    }

    return *this;
}

                      // *** string values ***

TestInStream& TestInStream::getString(bsl::string& variable)
{
    int length;
    getLength(length);

    if (isValid()) {
        variable.resize(length);

        // The implementation cannot call 'getArrayUint8' since the input limit
        // has already been modified.

        checkArray(TypeCode::e_UINT8, Util::k_SIZEOF_INT8, length);

        if (length > 0 && isValid()) {
            Util::getArrayInt8(&variable.front(), d_buffer + cursor(), length);
            d_cursor += Util::k_SIZEOF_INT8 * length;
        }
    }

    return *this;
}

                      // *** arrays of integer values ***

TestInStream& TestInStream::getArrayInt64(bsls::Types::Int64 *variables,
                                          int                 numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_INT64;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_INT64, numVariables);

    if (isValid()) {
        Util::getArrayInt64(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_INT64 * numVariables;
    }

    return *this;
}

TestInStream& TestInStream::getArrayUint64(bsls::Types::Uint64 *variables,
                                           int                  numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_UINT64;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_INT64, numVariables);

    if (isValid()) {
        Util::getArrayUint64(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_INT64 * numVariables;
    }

    return *this;
}

TestInStream& TestInStream::getArrayInt56(bsls::Types::Int64 *variables,
                                          int                 numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_INT56;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_INT56, numVariables);

    if (isValid()) {
        Util::getArrayInt56(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_INT56 * numVariables;
    }

    return *this;
}

TestInStream& TestInStream::getArrayUint56(bsls::Types::Uint64 *variables,
                                           int                  numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_UINT56;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_INT56, numVariables);

    if (isValid()) {
        Util::getArrayUint56(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_INT56 * numVariables;
    }

    return *this;
}

TestInStream& TestInStream::getArrayInt48(bsls::Types::Int64 *variables,
                                          int                 numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_INT48;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_INT48, numVariables);

    if (isValid()) {
        Util::getArrayInt48(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_INT48 * numVariables;
    }

    return *this;
}

TestInStream& TestInStream::getArrayUint48(bsls::Types::Uint64 *variables,
                                           int                  numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_UINT48;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_INT48, numVariables);

    if (isValid()) {
        Util::getArrayUint48(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_INT48 * numVariables;
    }

    return *this;
}

TestInStream& TestInStream::getArrayInt40(bsls::Types::Int64 *variables,
                                          int                 numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_INT40;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_INT40, numVariables);

    if (isValid()) {
        Util::getArrayInt40(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_INT40 * numVariables;
    }

    return *this;
}

TestInStream& TestInStream::getArrayUint40(bsls::Types::Uint64 *variables,
                                           int                  numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_UINT40;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_INT40, numVariables);

    if (isValid()) {
        Util::getArrayUint40(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_INT40 * numVariables;
    }

    return *this;
}

TestInStream& TestInStream::getArrayInt32(int *variables, int numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_INT32;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_INT32, numVariables);

    if (isValid()) {
        Util::getArrayInt32(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_INT32 * numVariables;
    }

    return *this;
}

TestInStream& TestInStream::getArrayUint32(unsigned int *variables,
                                           int           numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_UINT32;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_INT32, numVariables);

    if (isValid()) {
        Util::getArrayUint32(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_INT32 * numVariables;
    }

    return *this;
}

TestInStream& TestInStream::getArrayInt24(int *variables, int numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_INT24;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_INT24, numVariables);

    if (isValid()) {
        Util::getArrayInt24(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_INT24 * numVariables;
    }

    return *this;
}

TestInStream& TestInStream::getArrayUint24(unsigned int *variables,
                                           int           numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_UINT24;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_INT24, numVariables);

    if (isValid()) {
        Util::getArrayUint24(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_INT24 * numVariables;
    }

    return *this;
}

TestInStream& TestInStream::getArrayInt16(short *variables, int numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_INT16;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_INT16, numVariables);

    if (isValid()) {
        Util::getArrayInt16(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_INT16 * numVariables;
    }

    return *this;
}

TestInStream& TestInStream::getArrayUint16(unsigned short *variables,
                                           int             numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_UINT16;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_INT16, numVariables);

    if (isValid()) {
        Util::getArrayUint16(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_INT16 * numVariables;
    }

    return *this;
}

TestInStream& TestInStream::getArrayInt8(char *variables, int numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_INT8;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_INT8, numVariables);

    if (isValid()) {
        Util::getArrayInt8(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_INT8 * numVariables;
    }

    return *this;
}

TestInStream& TestInStream::getArrayInt8(signed char *variables,
                                         int          numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_INT8;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_INT8, numVariables);

    if (isValid()) {
        Util::getArrayInt8(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_INT8 * numVariables;
    }

    return *this;
}

TestInStream&
TestInStream::getArrayUint8(char *variables, int numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_UINT8;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_INT8, numVariables);

    if (isValid()) {
        Util::getArrayInt8(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_INT8 * numVariables;
    }

    return *this;
}

TestInStream& TestInStream::getArrayUint8(unsigned char *variables,
                                          int            numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_UINT8;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_INT8, numVariables);

    if (isValid()) {
        Util::getArrayInt8(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_INT8 * numVariables;
    }

    return *this;
}

                      // *** arrays of floating-point values ***

TestInStream& TestInStream::getArrayFloat64(double *variables,
                                            int     numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_FLOAT64;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_FLOAT64, numVariables);

    if (isValid()) {
        Util::getArrayFloat64(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_FLOAT64 * numVariables;
    }

    return *this;
}

TestInStream& TestInStream::getArrayFloat32(float *variables, int numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(0 <= numVariables);

    TypeCode::Enum code = TypeCode::e_FLOAT32;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::k_SIZEOF_FLOAT32, numVariables);

    if (isValid()) {
        Util::getArrayFloat32(variables, d_buffer + cursor(), numVariables);
        d_cursor += Util::k_SIZEOF_FLOAT32 * numVariables;
    }

    return *this;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const TestInStream& object)
{
    const bsl::size_t   len   = object.length();
    const char         *data  = object.data();
    bsl::ios::fmtflags  flags = stream.flags();

    stream << bsl::hex;

    for (bsl::size_t i = 0; i < len; ++i) {
        if (0 < i && 0 != i % 8) {
            stream << ' ';
        }
        if (0 == i % 8) { // output newline character and address every 8 bytes
            stream << '\n' << bsl::setw(4) << bsl::setfill('0') << i << '\t';
        }

        stream << bsl::setw(2)
               << bsl::setfill('0')
               << static_cast<int>(static_cast<unsigned char>(data[i]));
    }

    stream.flags(flags);  // reset stream format flags

    return stream;
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
