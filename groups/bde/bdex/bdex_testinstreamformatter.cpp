// bdex_testinstreamformatter.cpp                                     -*-C++-*-
#include <bdex_testinstreamformatter.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdex_testinstreamformatter_cpp,"$Id$ $CSID$")

#include <bdex_testinstreamexception.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_cstdio.h>

#ifdef FOR_TESTING_ONLY
#include <bdex_testoutstreamformatter.h>
#endif

namespace BloombergLP {

                        // ---------
                        // CONSTANTS
                        // ---------

// CPU-independent size of data types in wire format.  (Might not be the same
// as size of data types in memory.)
enum {
    SIZEOF_INT64   = 8,
    SIZEOF_INT56   = 7,
    SIZEOF_INT48   = 6,
    SIZEOF_INT40   = 5,
    SIZEOF_INT32   = 4,
    SIZEOF_INT24   = 3,
    SIZEOF_INT16   = 2,
    SIZEOF_INT8    = 1,
    SIZEOF_FLOAT64 = 8,
    SIZEOF_FLOAT32 = 4,
    SIZEOF_CODE    = SIZEOF_INT8,
    SIZEOF_VERSION = SIZEOF_INT8,
    SIZEOF_ARRLEN  = SIZEOF_INT32
};

namespace {

class bdex_TestInstreamFormatter_StreamBuf : public bsl::streambuf {
    // This class is used to expose the protected members 'egptr', 'eback' and
    // 'gptr' of 'streambuf'.

    // PRIVATE TYPES
    typedef bsl::streambuf Base;

  public:
    // ACCESSORS
    char *eback() const;
        // Return a pointer to the first element of the accessible part of the
        // controlled input sequence.

    char *egptr() const;
        // Return a pointer to the element that would follow the last element
        // of the accessible part of the controlled input sequence.

    char *gptr() const;
        // Return a pointer to the current element of the controlled input
        // sequence.
};

char *bdex_TestInstreamFormatter_StreamBuf::eback() const
{
    return Base::eback();
}

char *bdex_TestInstreamFormatter_StreamBuf::egptr() const
{
    return Base::egptr();
}

char *bdex_TestInstreamFormatter_StreamBuf::gptr() const
{
    return Base::gptr();
}

}  // close unnamed namespace

                        // --------------
                        // STATIC METHODS
                        // --------------

void bdex_TestInStreamFormatter::checkScalar(int                  *validFlag_p,
                                             bdex_FieldCode::Type  code,
                                             int                   scalarSize,
                                             int                   quietFlag)
{
    BSLS_ASSERT(validFlag_p);
    BSLS_ASSERT(0 < scalarSize);

    if (!*validFlag_p) {
        return;                                                     // RETURN
    }

    if (length() - cursor() >= SIZEOF_CODE) {
        unsigned char codeFound;  // 'unsigned' to support type values >= 128
        d_imp.getUint8(codeFound);
        if (codeFound != code) {
            *validFlag_p = 0;     // Wrong type.  Invalidate stream.
            if (!quietFlag) {
                bsl::fprintf(stderr,
                             "*** bdex_TestInStream: Expecting type %d"
                             ", but found type %d ***\n",
                             code,
                             (bdex_FieldCode::Type) codeFound);
            }
            return;                                                 // RETURN
        }
    } else {
        *validFlag_p = 0; // Incomplete stream.  Invalidate silently.
        return;                                                     // RETURN
    }

    if (length() - cursor() < scalarSize) {
        *validFlag_p = 0; // Incomplete stream.  Invalidate silently.
        return;                                                     // RETURN
    }
}

void bdex_TestInStreamFormatter::checkArray(int                  *validFlag_p,
                                            bdex_FieldCode::Type  code,
                                            int                   scalarSize,
                                            int                   numElements,
                                            int                   quietFlag)
{
    BSLS_ASSERT(validFlag_p);
    BSLS_ASSERT(0 < scalarSize);
    BSLS_ASSERT(0 <= numElements);

    if (!*validFlag_p) {
        return;                                                     // RETURN
    }

    if (length() - cursor() >= SIZEOF_CODE) {
        unsigned char codeFound;  // 'unsigned' to support type values >= 128
        d_imp.getUint8(codeFound);
        if (codeFound != code) {
            *validFlag_p = 0;     // Wrong type.  Invalidate stream.
            if (!quietFlag) {
                bsl::fprintf(stderr,
                             "*** bdex_TestInStream: Expecting type %d, "
                             "but found type %d ***\n",
                             code,
                             (bdex_FieldCode::Type) codeFound);
            }
            return;                                                 // RETURN
        }
    } else {
        *validFlag_p = 0; // Incomplete stream.  Invalidate silently.
        return;                                                     // RETURN
    }

    if (length() - cursor() >= SIZEOF_ARRLEN) {
        int numElemFound;
        d_imp.getInt32(numElemFound);
        if (numElemFound != numElements) {
            *validFlag_p = 0;
            if (!quietFlag) {
                bsl::fprintf(stderr,
                             "*** bdex_TestInStream: Expecting array length %d"
                             ", but found length %d ***\n",
                             numElements,
                             numElemFound);
            }
            return;                                                 // RETURN
        }
    } else {
        *validFlag_p = 0; // Incomplete stream.  Invalidate silently.
        return;                                                     // RETURN
    }

    if (length() - cursor() < scalarSize * numElements) {
        *validFlag_p = 0; // Incomplete stream.  Invalidate silently.
        return;                                                     // RETURN
    }
}

void bdex_TestInStreamFormatter::checkVersion(int *validFlag_p,
                                              int  suppressVersionCheckFlag,
                                              int  quietFlag)
{
    BSLS_ASSERT(validFlag_p);

    if (!*validFlag_p || suppressVersionCheckFlag) {
        return;                                                     // RETURN
    }

    unsigned char codeFound;  // 'unsigned' to support type values >= 128
    if (length() >= SIZEOF_CODE) {
        codeFound = d_streamBuf->sgetc();

        if (codeFound != bdex_FieldCode::BDEX_INT8 &&
            codeFound != bdex_FieldCode::BDEX_UINT8) {
            *validFlag_p = 0;     // Wrong type.  Invalidate stream.
            if (!quietFlag) {
                bsl::fprintf(stderr,
                             "*** bdex_TestInStream: Expecting version of type"
                             " %d, but found type %d ***\n",
                             bdex_FieldCode::BDEX_INT8,
                             (bdex_FieldCode::Type) codeFound);
            }
            return;                                                 // RETURN
        }
    } else {
        *validFlag_p = 0; // Incomplete stream.  Invalidate silently.
        return;                                                     // RETURN
    }

    if (length() - SIZEOF_CODE >= SIZEOF_VERSION) {
        char version = 0;
        char temp = d_streamBuf->sbumpc();
        version = d_streamBuf->sgetc();
        d_streamBuf->sputbackc(temp);
        if (version <= 0) {
            *validFlag_p = 0;     // Invalid version.  Invalidate stream.
            if (!quietFlag)
                bsl::fprintf(stderr,
                             "*** [bdex_TestInStream] Invalid version:"
                             " %d ***\n", (int) version);
        }
    } else {
        *validFlag_p = 0; // Incomplete stream.  Invalidate silently.
        return;                                                     // RETURN
    }
}

                        // =======================
                        // class bdex_TestInStream
                        // =======================

// CREATORS
bdex_TestInStreamFormatter::bdex_TestInStreamFormatter(
                                                  bsl::streambuf *streamBuffer)

: d_validFlag(1)
, d_quietFlag(0)
, d_suppressVersionCheckFlag(0)
, d_inputLimit(-1)
, d_imp(streamBuffer)
, d_streamBuf(streamBuffer)
{
    BSLS_ASSERT(streamBuffer);
}

bdex_TestInStreamFormatter::~bdex_TestInStreamFormatter()
{
}

// MANIPULATORS
bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getLength(int& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT32;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif

    if (length() - cursor() < SIZEOF_CODE + SIZEOF_INT8) {
        d_validFlag = 0; // Incomplete stream.  Invalidate silently.
    }
    else {
      char temp = d_streamBuf->sbumpc();
      char temp2 = d_streamBuf->sgetc();
      d_streamBuf->sputbackc(temp);
      if (127 < (unsigned char)temp2 ) {
            if (d_validFlag) {
                getInt32(variable);
                variable ^= (1 << 31);
            }
        }
        else {
            code = bdex_FieldCode::BDEX_INT8;
            if (d_validFlag) {
                char tmp;
                getInt8(tmp);
                variable = tmp;
            }
        }
    }
    return *this;
}

bdex_TestInStreamFormatter& bdex_TestInStreamFormatter::getString(
                                                              bsl::string& str)
{
    int length;
    getLength(length);
    if (d_validFlag) {
        str.resize(length);
        getArrayUint8((char *)(void *)str.data(), length);
    }
    return *this;
}

bdex_TestInStreamFormatter& bdex_TestInStreamFormatter::getVersion(
                                                                int& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT8;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT8, d_quietFlag);
    if (d_validFlag) {
        unsigned char tmp;
        tmp = d_streamBuf->sbumpc();
        variable = tmp;
    }

    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getInt64(bsls_PlatformUtil::Int64& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT64;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT64, d_quietFlag);
    if (d_validFlag) {
        d_imp.getInt64(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getUint64(bsls_PlatformUtil::Uint64& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT64;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT64, d_quietFlag);
    if (d_validFlag) {
        d_imp.getUint64(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getInt56(bsls_PlatformUtil::Int64& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT56;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT56, d_quietFlag);
    if (d_validFlag) {
        d_imp.getInt56(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getUint56(bsls_PlatformUtil::Uint64& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT56;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT56, d_quietFlag);
    if (d_validFlag) {
        d_imp.getUint56(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getInt48(bsls_PlatformUtil::Int64& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT48;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT48, d_quietFlag);
    if (d_validFlag) {
        d_imp.getInt48(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getUint48(bsls_PlatformUtil::Uint64& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT48;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT48, d_quietFlag);
    if (d_validFlag) {
        d_imp.getUint48(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getInt40(bsls_PlatformUtil::Int64& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT40;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT40, d_quietFlag);
    if (d_validFlag) {
        d_imp.getInt40(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getUint40(bsls_PlatformUtil::Uint64& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT40;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT40, d_quietFlag);
    if (d_validFlag) {
        d_imp.getUint40(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getInt32(int& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT32;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT32, d_quietFlag);
    if (d_validFlag) {
        d_imp.getInt32(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getUint32(unsigned int& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT32;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT32, d_quietFlag);
    if (d_validFlag) {
        d_imp.getUint32(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getInt24(int& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT24;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT24, d_quietFlag);
    if (d_validFlag) {
        d_imp.getInt24(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getUint24(unsigned int& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT24;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT24, d_quietFlag);
    if (d_validFlag) {
        d_imp.getUint24(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getInt16(short& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT16;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT16, d_quietFlag);
    if (d_validFlag) {
        d_imp.getInt16(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getUint16(unsigned short& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT16;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT16, d_quietFlag);
    if (d_validFlag) {
        d_imp.getUint16(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getInt8(char& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT8;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT8, d_quietFlag);
    if (d_validFlag) {
        d_imp.getInt8(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getInt8(signed char& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT8;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT8, d_quietFlag);
    if (d_validFlag) {
        d_imp.getInt8(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getUint8(char& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT8;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT8, d_quietFlag);
    if (d_validFlag) {
        d_imp.getInt8(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getUint8(unsigned char& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT8;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_INT8, d_quietFlag);
    if (d_validFlag) {
        d_imp.getUint8(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getFloat64(double& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_FLOAT64;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkScalar(&d_validFlag, code, SIZEOF_FLOAT64, d_quietFlag);
    if (d_validFlag) {
        d_imp.getFloat64(variable);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getFloat32(float& variable)
{
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_FLOAT32;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif

    checkScalar(&d_validFlag, code, SIZEOF_FLOAT32, d_quietFlag);
    if (d_validFlag) {
        d_imp.getFloat32(variable);
    }

    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayInt64(bsls_PlatformUtil::Int64 *array,
                                          int                       length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT64;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_INT64, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayInt64(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayUint64(bsls_PlatformUtil::Uint64 *array,
                                           int                        length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT64;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_INT64, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayUint64(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayInt56(bsls_PlatformUtil::Int64 *array,
                                          int                       length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT56;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_INT56, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayInt56(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayUint56(bsls_PlatformUtil::Uint64 *array,
                                           int                        length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT56;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_INT56, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayUint56(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayInt48(bsls_PlatformUtil::Int64 *array,
                                          int                       length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT48;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_INT48, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayInt48(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayUint48(bsls_PlatformUtil::Uint64 *array,
                                           int                        length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT48;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_INT48, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayUint48(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayInt40(bsls_PlatformUtil::Int64 *array,
                                          int                       length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT40;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_INT40, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayInt40(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayUint40(bsls_PlatformUtil::Uint64 *array,
                                           int                        length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT40;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_INT40, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayUint40(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayInt32(int *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);
    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT32;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_INT32, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayInt32(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayUint32(unsigned int *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT32;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_INT32, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayUint32(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayInt24(int *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT24;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_INT24, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayInt24(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayUint24(unsigned int *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT24;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_INT24, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayUint24(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayInt16(short *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT16;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_INT16, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayInt16(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayUint16(unsigned short *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT16;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_INT16, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayUint16(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayInt8(char *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT8;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_INT8, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayInt8(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayInt8(signed char *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_INT8;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_INT8, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayInt8(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayUint8(char *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT8;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_INT8, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayInt8(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayUint8(unsigned char *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_UINT8;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_INT8, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayUint8(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayFloat64(double *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }
    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_FLOAT64;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_FLOAT64, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayFloat64(array, length);
    }
    return *this;
}

bdex_TestInStreamFormatter&
bdex_TestInStreamFormatter::getArrayFloat32(float *array, int length)
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    if (0 == cursor()) {
        checkVersion(&d_validFlag, d_suppressVersionCheckFlag, d_quietFlag);
    }

    bdex_FieldCode::Type code = bdex_FieldCode::BDEX_FLOAT32;
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(code);
        }
    }
#endif
    checkArray(&d_validFlag, code, SIZEOF_FLOAT32, length, d_quietFlag);
    if (d_validFlag) {
        d_imp.getArrayFloat32(array, length);
    }
    return *this;
}

// ACCESSORS
int bdex_TestInStreamFormatter::length() const
{
    return (int)(
        (static_cast<bdex_TestInstreamFormatter_StreamBuf *>(
                                                       d_streamBuf))->egptr() -
        (static_cast<bdex_TestInstreamFormatter_StreamBuf *>(
                                                       d_streamBuf)->eback()));
}

int bdex_TestInStreamFormatter::cursor() const
{
    return (int)(
        (static_cast<bdex_TestInstreamFormatter_StreamBuf *>(
                                                       d_streamBuf))->gptr() -
        (static_cast<bdex_TestInstreamFormatter_StreamBuf *>(
                                                       d_streamBuf)->eback()));
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
