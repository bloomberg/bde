// bdem_berutil.h                                                     -*-C++-*-
#ifndef INCLUDED_BDEM_BERUTIL
#define INCLUDED_BDEM_BERUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide functions to encode and decode simple types in BER format.
//
//@CLASSES:
//   bdem_BerUtil: namespace of utility functions for BER
//
//@SEE_ALSO: bdem_berencoder, bdem_berdecoder
//
//@AUTHOR: Rohan Bhindwale (rbhindwa), Shezan Baig (sbaig)
//
//@DESCRIPTION: This component provides utility functions for encoding and
// decoding of primitive BER constructs, such as tag identifier octets, length
// octets, fundamental C++ types.  The encoding and decoding of 'bsl::string'
// and BDE date/time types is also implemented.
//
// These utility functions operate on 'bsl::streambuf' for buffer management.
//
// More information about BER constructs can be found in the BER specification
// (X.690).  A copy of the specification can be found at the URL:
//: o  http://www.itu.int/ITU-T/studygroups/com17/languages/X.690-0207.pdf
//
// Note that this is a low-level component that only encodes and decodes
// primitive constructs.  Clients should use the 'bdem_berencoder' and
// 'bdem_berdecoder' components (which use this component in the
// implementation) to encode and decode well-formed BER messages.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose we want to write the identifier octets for a BER tag having the
// following properties:
//..
//    Tag Class:   Context-specific
//    Tag Type:    Primitive
//    Tag Number:  31
//..
// According to the BER specification, this should generate two octets
// containing the values 0x9F and 0x1F.  Note that we will use 'bdesb' stream
// buffers for in-core buffer management:
//..
//  #include <bdem_berconstants.h>
//  #include <bdem_berutil.h>
//  #include <bdesb_fixedmeminstreambuf.h>
//  #include <bdesb_memoutstreambuf.h>
//
//  using namespace BloombergLP;
//
//  void usageExample()
//  {
//      bdesb_MemOutStreamBuf osb;
//
//      bdem_BerConstants::TagClass tagClass  =
//                                         bdem_BerConstants::CONTEXT_SPECIFIC;
//      bdem_BerConstants::TagType  tagType   = bdem_BerConstants::PRIMITIVE;
//      int                         tagNumber = 31;
//
//      int retCode = bdem_BerUtil::putIdentifierOctets(&osb,
//                                                      tagClass,
//                                                      tagType,
//                                                      tagNumber);
//      assert(0    == retCode);
//      assert(2    == osb.length());
//      assert(0x9F == (unsigned char)osb.data()[0]);
//      assert(0x1F == (unsigned char)osb.data()[1]);
//..
// The next part of the function will read the identifier octets from the
// stream and verify its contents:
//..
//      bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());     // NO COPY
//
//      bdem_BerConstants::TagClass tagClassIn;
//      bdem_BerConstants::TagType  tagTypeIn;
//      int                         tagNumberIn;
//      int                         numBytesConsumed = 0;
//
//      retCode = bdem_BerUtil::getIdentifierOctets(&isb,
//                                                  &tagClassIn,
//                                                  &tagTypeIn,
//                                                  &tagNumberIn,
//                                                  &numBytesConsumed);
//      assert(0         == retCode);
//      assert(2         == numBytesConsumed);
//      assert(tagClass  == tagClassIn);
//      assert(tagType   == tagTypeIn);
//      assert(tagNumber == tagNumberIn);
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_BERCONSTANTS
#include <bdem_berconstants.h>
#endif

#ifndef INCLUDED_BDEM_BERCONSTANTS
#include <bdem_berconstants.h>
#endif

#ifndef INCLUDED_BDEM_BERENCODEROPTIONS
#include <bdem_berencoderoptions.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BDES_BITUTIL
#include <bdes_bitutil.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BDES_ASSERT
#include <bdes_assert.h>
#endif

#ifndef INCLUDED_BDES_PLATFORMUTIL
#include <bdes_platformutil.h>
#endif

#ifndef INCLUDED_STREAMBUF
#include <streambuf>
#define INCLUDED_STREAMBUF
#endif

#endif

namespace BloombergLP {

class bdet_Date;
class bdet_Datetime;
class bdet_DatetimeTz;
class bdet_DateTz;
class bdet_Time;
class bdet_TimeTz;

                            // ===================
                            // struct bdem_BerUtil
                            // ===================

struct bdem_BerUtil {
    // This utility contains functions to encode and decode primitive BER
    // constructs and simple value semantic types.  By convention, all
    // functions return 0 on success, and a non-zero value otherwise.  Also by
    // convention, all the "get" functions take an 'accumNumBytesConsumed';
    // each of the functions will add to this variable the number of bytes
    // consumed within the scope of the function.

    enum {
        BDEM_INDEFINITE_LENGTH = -1  // used to indicate that the length is
                                     // indefinite

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , INDEFINITE_LENGTH = BDEM_INDEFINITE_LENGTH
#endif
    };

    // CLASS METHODS
    static int getEndOfContentOctets(bsl::streambuf *streamBuf,
                                     int            *accumNumBytesConsumed);
        // Decode the "end-of-content" octets (two consecutive zero-octets)
        // from the specified 'streamBuf' and add the number of bytes consumed
        // (which is always 2) to the specified 'accumNumBytesConsumed'.
        // Return 0 on success, and a non-zero value otherwise.

    static int getIdentifierOctets(
                           bsl::streambuf              *streamBuf,
                           bdem_BerConstants::TagClass *tagClass,
                           bdem_BerConstants::TagType  *tagType,
                           int                         *tagNumber,
                           int                         *accumNumBytesConsumed);
        // Decode the identifier octets from the specified 'streamBuf' and load
        // the tag class, tag type, and tag number into the specified
        // 'tagClass', 'tagType', and 'tagNumber' respectively.  Add the number
        // of bytes consumed to the specified 'accumNumBytesConsumed'.  Return
        // 0 on success, and a non-zero value otherwise.

    static int getLength(bsl::streambuf *streamBuf,
                         int            *result,
                         int            *accumNumBytesConsumed);
        // Decode the length octets from the specified 'streamBuf' and load the
        // result into the specified 'length'.  If the length is indefinite
        // (i.e., contents will be terminated by "end-of-content" octets) then
        // 'result' will be set to 'BDEM_INDEFINITE_LENGTH'.  Add the number of
        // bytes consumed to the specified 'accumNumBytesConsumed'.  Return 0
        // on success, and a non-zero value otherwise.

    template <typename TYPE>
    static int getValue(bsl::streambuf *streamBuf,
                        TYPE           *value,
                        int             length);
        // Decode the specified 'value' from the specified 'streamBuf',
        // consuming exactly 'length' bytes.  Return 0 on success, and a
        // non-zero value otherwise.  Note that the value consists of the
        // contents bytes only (no length prefix).  Also note that only
        // fundamental C++ types, 'bsl::string', and BDE date/time types are
        // supported.

    template <typename TYPE>
    static int getValue(bsl::streambuf *streamBuf,
                        TYPE           *value,
                        int            *accumNumBytesConsumed);
        // Decode the specified 'value' from the specified 'streamBuf' and add
        // the number of bytes consumed to the specified
        // 'accumNumBytesConsumed'.  Return 0 on success, and a non-zero value
        // otherwise.  Note that the value consists of the length and contents
        // primitives.  Also note that only fundamental C++ types,
        // 'bsl::string', and BDE date/time types are supported.

    static int putEndOfContentOctets(bsl::streambuf *streamBuf);
        // Encode the "end-of-content" octets (two consecutive zero-octets) to
        // the specified 'streamBuf'.  The "end-of-content" octets act as the
        // termination bytes for objects that have indefinite length.  Return 0
        // on success, and a non-zero value otherwise.

    static int putIdentifierOctets(bsl::streambuf              *streamBuf,
                                   bdem_BerConstants::TagClass  tagClass,
                                   bdem_BerConstants::TagType   tagType,
                                   int                          tagNumber);
        // Encode the identifier octets for the specified 'tagClass', 'tagType'
        // and 'tagNumber' to the specified 'streamBuf'.  Return 0 on success,
        // and a non-zero value otherwise.

    static int putIndefiniteLengthOctet(bsl::streambuf *streamBuf);
        // Encode the "indefinite-length" octet onto the specified 'streamBuf'.
        // This octet signifies that the length of the contents is indefinite
        // (i.e., contents will be terminated by end of content octets).
        // Return 0 on success, and a non-zero value otherwise.

    static int putLength(bsl::streambuf *streamBuf, int length);
        // Encode the specified 'length' to the specified 'streamBuf'.  Return
        // 0 on success, and a non-zero value otherwise.  The behavior is
        // undefined unless '0 <= length'.

    template <typename TYPE>
    static int putValue(bsl::streambuf               *streamBuf,
                        const TYPE&                   value,
                        const bdem_BerEncoderOptions *options = 0);
        // Encode the specified 'value' to the specified 'streamBuf'.  Return 0
        // on success, and a non-zero value otherwise.  Note that the value
        // consists of the length and contents primitives.  Also note that only
        // fundamental C++ types, 'bsl::string', 'bslstl_StringRef' and BDE
        // date/time types are supported.
};

// ---  Anything below this line is implementation specific.  Do not use.  ----

                        // =======================
                        // struct bdem_BerUtil_Imp
                        // =======================

struct bdem_BerUtil_Imp {
    // This 'struct' contains implementation functions used by the namespace
    // 'bdem_BerUtil'.

    enum {
        INDEFINITE_LENGTH       = bdem_BerUtil::BDEM_INDEFINITE_LENGTH,
        BITS_PER_OCTET          = 8,
        MAX_INTEGER_LENGTH      = 9,
        INDEFINITE_LENGTH_OCTET = 0x80  // value that indicates an
                                        // indefinite length
    };

    // CLASS METHODS
    static int getDoubleValue(bsl::streambuf *streamBuf,
                              double         *value,
                              int             length);

    static int getIntegerValue(bsl::streambuf *streamBuf,
                               long long      *value,
                               int             length);
    template <typename TYPE>
    static int getIntegerValue(bsl::streambuf *streamBuf,
                               TYPE           *value,
                               int             length);

    static int getLength(bsl::streambuf *streamBuf,
                         int            *result,
                         int            *accumNumBytesConsumed);

    template <typename TYPE>
    static int getValue(bsl::streambuf *streamBuf,
                        TYPE           *value,
                        int             length);
    static int getValue(bsl::streambuf *streamBuf,
                        bool           *value,
                        int             length);
    static int getValue(bsl::streambuf *streamBuf,
                        char           *value,
                        int             length);
    static int getValue(bsl::streambuf *streamBuf,
                        unsigned char  *value,
                        int             length);
    static int getValue(bsl::streambuf *streamBuf,
                        signed char    *value,
                        int             length);
    static int getValue(bsl::streambuf *streamBuf,
                        float          *value,
                        int             length);
    static int getValue(bsl::streambuf *streamBuf,
                        double         *value,
                        int             length);
    static int getValue(bsl::streambuf *streamBuf,
                        bsl::string    *value,
                        int             length);
    static int getValue(bsl::streambuf   *streamBuf,
                        bslstl_StringRef *value,
                        int               length);
    static int getValue(bsl::streambuf *streamBuf,
                        bdet_Date      *value,
                        int             length);
    static int getValue(bsl::streambuf *streamBuf,
                        bdet_Datetime  *value,
                        int             length);
    static int getValue(bsl::streambuf  *streamBuf,
                        bdet_DatetimeTz *value,
                        int              length);
    static int getValue(bsl::streambuf *streamBuf,
                        bdet_DateTz    *value,
                        int             length);
    static int getValue(bsl::streambuf *streamBuf,
                        bdet_Time      *value,
                        int             length);
    static int getValue(bsl::streambuf *streamBuf,
                        bdet_TimeTz    *value,
                        int             length);

    static int numBytesToStream(short value);
    static int numBytesToStream(int value);
    static int numBytesToStream(long long value);
    template <typename TYPE>
    static int numBytesToStream(TYPE value);

    static int getBinaryDateValue(bsl::streambuf *streamBuf,
                                  bdet_Date      *value,
                                  int             length);
    static int getBinaryTimeValue(bsl::streambuf *streamBuf,
                                  bdet_Time      *value,
                                  int             length);
    static int getBinaryDatetimeValue(bsl::streambuf *streamBuf,
                                      bdet_Datetime  *value,
                                      int             length);
    static int getBinaryDateTzValue(bsl::streambuf *streamBuf,
                                    bdet_DateTz    *value,
                                    int             length);
    static int getBinaryTimeTzValue(bsl::streambuf *streamBuf,
                                    bdet_TimeTz    *value,
                                    int             length);
    static int getBinaryDatetimeTzValue(bsl::streambuf  *streamBuf,
                                        bdet_DatetimeTz *value,
                                        int              length);

    static int putBinaryDateValue(bsl::streambuf   *streamBuf,
                                  const bdet_Date&  value);
    static int putBinaryTimeValue(bsl::streambuf   *streamBuf,
                                  const bdet_Time&  value);
    static int putBinaryDatetimeValue(bsl::streambuf       *streamBuf,
                                      const bdet_Datetime&  value);
    static int putBinaryDateTzValue(bsl::streambuf     *streamBuf,
                                    const bdet_DateTz&  value);
    static int putBinaryTimeTzValue(bsl::streambuf     *streamBuf,
                                    const bdet_TimeTz&  value);
    static int putBinaryDatetimeTzValue(bsl::streambuf         *streamBuf,
                                        const bdet_DatetimeTz&  value);

    static int putDoubleValue(bsl::streambuf *streamBuf, double value);

    template <typename TYPE>
    static int putIntegerGivenLength(bsl::streambuf *streamBuf,
                                     TYPE            value,
                                     int             length);

    template <typename TYPE>
    static int putIntegerValue(bsl::streambuf *streamBuf, TYPE value);

    static int putLength(bsl::streambuf *streamBuf, int length);

    static int putStringValue(bsl::streambuf *streamBuf,
                              const char     *value,
                              int             valueLength);

    template <typename TYPE>
    static int putValue(bsl::streambuf               *streamBuf,
                        const TYPE&                   value,
                        const bdem_BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        bool                          value,
                        const bdem_BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        char                          value,
                        const bdem_BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        unsigned char                 value,
                        const bdem_BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        signed char                   value,
                        const bdem_BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        float                         value,
                        const bdem_BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        double                        value,
                        const bdem_BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        const bsl::string&            value,
                        const bdem_BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        const bslstl_StringRef&       value,
                        const bdem_BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        const bdet_Date&              value,
                        const bdem_BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        const bdet_Datetime&          value,
                        const bdem_BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        const bdet_DatetimeTz&        value,
                        const bdem_BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        const bdet_DateTz&            value,
                        const bdem_BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        const bdet_Time&              value,
                        const bdem_BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        const bdet_TimeTz&            value,
                        const bdem_BerEncoderOptions *options);
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                            // -------------------
                            // struct bdem_BerUtil
                            // -------------------

// CLASS METHODS
inline
int bdem_BerUtil::getEndOfContentOctets(bsl::streambuf *streamBuf,
                                        int            *accumNumBytesConsumed)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    *accumNumBytesConsumed += 2;

    return 0 == streamBuf->sbumpc() && 0 == streamBuf->sbumpc()
         ? BDEM_SUCCESS
         : BDEM_FAILURE;
}

inline
int bdem_BerUtil::getLength(bsl::streambuf *streamBuf,
                            int            *result,
                            int            *accumNumBytesConsumed)
{
    return bdem_BerUtil_Imp::getLength(streamBuf,
                                       result,
                                       accumNumBytesConsumed);
}

template <typename TYPE>
inline
int bdem_BerUtil::getValue(bsl::streambuf *streamBuf,
                           TYPE           *value,
                           int             length)
{
    return bdem_BerUtil_Imp::getValue(streamBuf, value, length);
}

template <typename TYPE>
inline
int bdem_BerUtil::getValue(bsl::streambuf *streamBuf,
                           TYPE           *value,
                           int            *accumNumBytesConsumed)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    int length;
    if (bdem_BerUtil_Imp::getLength(streamBuf, &length, accumNumBytesConsumed))
    {
        return BDEM_FAILURE;                                          // RETURN
    }

    if (getValue(streamBuf, value, length)) {
        return BDEM_FAILURE;                                          // RETURN
    }

    *accumNumBytesConsumed += length;
    return BDEM_SUCCESS;
}

inline
int bdem_BerUtil::putEndOfContentOctets(bsl::streambuf *streamBuf)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    return 0 == streamBuf->sputc(0) && 0 == streamBuf->sputc(0)
         ? BDEM_SUCCESS
         : BDEM_FAILURE;
}

inline
int bdem_BerUtil::putIndefiniteLengthOctet(bsl::streambuf *streamBuf)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    // "extra" unsigned char cast needed to suppress warning on Windows.

    return bdem_BerUtil_Imp::INDEFINITE_LENGTH_OCTET
               == streamBuf->sputc(static_cast<char>(
                  (unsigned char)(bdem_BerUtil_Imp::INDEFINITE_LENGTH_OCTET)))
               ? BDEM_SUCCESS
               : BDEM_FAILURE;
}

inline
int bdem_BerUtil::putLength(bsl::streambuf *streamBuf, int length)
{
    return bdem_BerUtil_Imp::putLength(streamBuf, length);
}

template <typename TYPE>
inline
int bdem_BerUtil::putValue(bsl::streambuf               *streamBuf,
                           const TYPE&                   value,
                           const bdem_BerEncoderOptions *options)
{
    return bdem_BerUtil_Imp::putValue(streamBuf, value, options);
}

                      // -----------------------
                      // struct bdem_BerUtil_Imp
                      // -----------------------

// CLASS METHODS
template <typename TYPE>
int bdem_BerUtil_Imp::getIntegerValue(bsl::streambuf *streamBuf,
                                      TYPE           *value,
                                      int             length)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    enum { SIGN_BIT_MASK = 0x80 };

    static const bool isUnsigned = (TYPE(-1) > TYPE(0));

    if (isUnsigned && (unsigned) length == sizeof(TYPE) + 1) {
        // Length of an unsigned is allowed to be one larger then sizeof(TYPE)
        // only if first byte is zero.  (This is so that large unsigned
        // numbers don't appear as negative numbers in the BER stream).
        // Remove the leading zero byte.

        if (0 != streamBuf->sbumpc()) {
            // First byte was not zero.  Fail.

            return BDEM_FAILURE;                                      // RETURN
        }

        --length;
    }

    if ((unsigned) length > sizeof(TYPE)) {
        // Overflow.

        return BDEM_FAILURE;                                          // RETURN
    }

    *value = (TYPE)(streamBuf->sgetc() & SIGN_BIT_MASK ? -1 : 0);

    for (int i = 0; i < length; ++i) {
        int nextOctet = streamBuf->sbumpc();
        if (bsl::streambuf::traits_type::eof() == nextOctet) {
            return BDEM_FAILURE;                                      // RETURN
        }

        *value = (TYPE)(*value << BITS_PER_OCTET);
        *value = (TYPE)(*value | (unsigned char)nextOctet);
    }

    return BDEM_SUCCESS;
}

template <typename TYPE>
inline
int bdem_BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
                               TYPE           *value,
                               int             length)
{
    return bdem_BerUtil_Imp::getIntegerValue(streamBuf, value, length);
}

inline
int bdem_BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
                               bool           *value,
                               int             length)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    if (1 != length) {
        return BDEM_FAILURE;                                          // RETURN
    }

    int intValue = streamBuf->sbumpc();
    if (bsl::streambuf::traits_type::eof() == intValue) {
        return BDEM_FAILURE;                                          // RETURN
    }

    *value = 0 != intValue;

    return BDEM_SUCCESS;
}

inline
int bdem_BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
                               char           *value,
                               int             length)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    if (1 != length) {
        return BDEM_FAILURE;                                          // RETURN
    }

    int valueOctet = streamBuf->sbumpc();
    if (bsl::streambuf::traits_type::eof() == valueOctet) {
        return BDEM_FAILURE;                                          // RETURN
    }

    *value = (char) valueOctet;
    return BDEM_SUCCESS;
}

inline
int bdem_BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
                               unsigned char  *value,
                               int             length)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    short temp;
    if (bdem_BerUtil_Imp::getIntegerValue(streamBuf, &temp, length)) {
        return BDEM_FAILURE;                                          // RETURN
    }
    *value = (unsigned char) temp;
    return BDEM_SUCCESS;
}

inline
int bdem_BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
                               signed char    *value,
                               int             length)
{
    return getValue(streamBuf, (char *) value, length);
}

inline
int bdem_BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
                               float          *value,
                               int             length)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    double dvalue;
    if (bdem_BerUtil_Imp::getDoubleValue(streamBuf, &dvalue, length)) {
        return BDEM_FAILURE;                                          // RETURN
    }
    *value = (float) dvalue;
    return BDEM_SUCCESS;
}

inline
int bdem_BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
                               double         *value,
                               int             length)
{
    return bdem_BerUtil_Imp::getDoubleValue(streamBuf, value, length);
}

template <typename TYPE>
int bdem_BerUtil_Imp::numBytesToStream(TYPE value)
{
    int numBytes = sizeof(TYPE);
    static const TYPE NEG_MASK =
        (TYPE) 0xff80 << (sizeof(TYPE) - 2) * BITS_PER_OCTET;
    if (0 == value) {
        numBytes = 1;
    }
    else if (value > 0) {
        static const TYPE SGN_BIT =
            TYPE(TYPE(1) << (sizeof(TYPE) * BITS_PER_OCTET - 1));
        if (value & SGN_BIT) {
            // If value is > 0 but the high bit (sign bit) is set, then this
            // is an unsigned value and a leading zero byte must be emitted to
            // prevent the value from looking like a negative value on the
            // wire.  The leading zero is followed by all of the bytes of the
            // unsigned value.

            return sizeof(TYPE) + 1;                                  // RETURN
        }

        // mask that zeroes out the most significant byte and the first bit
        // of the next byte.

        static const TYPE POS_MASK = TYPE(~NEG_MASK);
        while ((value & POS_MASK) == value) {
            value = (TYPE)(value << 8);  // shift out redundant high-order 0x00
            --numBytes;
        }
    }
    else {  // 0 > value
        while ((value | NEG_MASK) == value) {
            value = (TYPE)(value << 8);  // shift out redundant high-order 0xFF
            --numBytes;
        }
    }

    BSLS_ASSERT_SAFE(numBytes > 0);
    return numBytes;
}

template <typename TYPE>
int bdem_BerUtil_Imp::putIntegerGivenLength(bsl::streambuf *streamBuf,
                                            TYPE            value,
                                            int             length)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    if (length <= 0) {
        return BDEM_FAILURE;                                          // RETURN
    }

    static const bool isUnsigned = (TYPE(-1) > TYPE(0));

    if (isUnsigned && (unsigned) length == sizeof(TYPE) + 1) {
        static const TYPE SGN_BIT =
            TYPE(TYPE(1) << (sizeof(TYPE) * BITS_PER_OCTET - 1));
        // Length may be one greater than sizeof(TYPE) only if type is
        // unsigned and the high bit (normally the sign bit) is set.  In this
        // case, a leading zero octet is emitted.

        if (! (value & SGN_BIT)) {
            return BDEM_FAILURE;                                      // RETURN
        }

        if (0 != streamBuf->sputc(0)) {
            return BDEM_FAILURE;                                      // RETURN
        }

        --length;
    }

    if ((unsigned) length > sizeof(TYPE)) {
        return BDEM_FAILURE;                                          // RETURN
    }

#if BSLS_PLATFORMUTIL__IS_BIG_ENDIAN
    return length == streamBuf->sputn((char *) &value + sizeof(TYPE) - length,
                                      length)
         ? BDEM_SUCCESS
         : BDEM_FAILURE;

#else

    char *dst = (char *) &value + length;
    for ( ; length > 0; --length) {
        unsigned char c = *--dst;
        if (c != streamBuf->sputc(c)) {
            return BDEM_FAILURE;                                      // RETURN
        }
    }

    return BDEM_SUCCESS;

#endif
}

template <typename TYPE>
inline
int bdem_BerUtil_Imp::putIntegerValue(bsl::streambuf *streamBuf,
                                      TYPE            value)
{
    enum { BDEM_FAILURE = -1 };

    typedef bsl::streambuf::char_type char_type;
    const int length = numBytesToStream(value);
    if (length != streamBuf->sputc((char_type)length)) {
        return BDEM_FAILURE;                                          // RETURN
    }

    return putIntegerGivenLength(streamBuf, value, length);
}

inline
int bdem_BerUtil_Imp::putStringValue(bsl::streambuf *streamBuf,
                                     const char     *value,
                                     int             valueLength)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    return bdem_BerUtil_Imp::putLength(streamBuf, valueLength)
        || valueLength != streamBuf->sputn(value, valueLength)
         ? BDEM_FAILURE
         : BDEM_SUCCESS;
}

template <typename TYPE>
inline
int bdem_BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               const TYPE&                   value,
                               const bdem_BerEncoderOptions *)
{
    return bdem_BerUtil_Imp::putIntegerValue(streamBuf, value);
}

inline
int bdem_BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               bool                          value,
                               const bdem_BerEncoderOptions *)
{
    // It has been observed in practice that 'value' may refer to
    // uninitialized or overwritten memory, in which case its value may
    // neither be 'true' ('1') nor 'false' ('0').  We assert here to ensure
    // that users get a useful error message.  Note that we assert 
    // (rather than returning an error code), as it is undefined behavior to
    // examine the value of such an uninitialized 'bool'.

    BSLS_ASSERT(0 == value || 1 == value);

    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    typedef bsl::streambuf::char_type char_type;

    return 1 == streamBuf->sputc(1)
        && (int)value == streamBuf->sputc((char_type)(value ? 1 : 0))
         ? BDEM_SUCCESS
         : BDEM_FAILURE;
}

inline
int bdem_BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               char                          value,
                               const bdem_BerEncoderOptions *)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    return 1 == streamBuf->sputc(1)
        && (unsigned char) value == streamBuf->sputc(value)
         ? BDEM_SUCCESS
         : BDEM_FAILURE;
}

inline
int bdem_BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               unsigned char                 value,
                               const bdem_BerEncoderOptions *)
{
    return bdem_BerUtil_Imp::putIntegerValue(streamBuf, (unsigned short)value);
}

inline
int bdem_BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               signed char                   value,
                               const bdem_BerEncoderOptions *options)
{
    return putValue(streamBuf, (char) value, options);
}

inline
int bdem_BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               float                         value,
                               const bdem_BerEncoderOptions *)
{
    return bdem_BerUtil_Imp::putDoubleValue(streamBuf, (double) value);
}

inline
int bdem_BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               double                        value,
                               const bdem_BerEncoderOptions *)
{
    return bdem_BerUtil_Imp::putDoubleValue(streamBuf, value);
}

inline
int bdem_BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               const bsl::string&            value,
                               const bdem_BerEncoderOptions *)
{
    return putStringValue(streamBuf,
                          value.data(),
                          static_cast<int>(value.length()));
}

inline
int bdem_BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               const bslstl_StringRef&       value,
                               const bdem_BerEncoderOptions *)
{
    return putStringValue(streamBuf,
                          value.data(),
                          static_cast<int>(value.length()));
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
