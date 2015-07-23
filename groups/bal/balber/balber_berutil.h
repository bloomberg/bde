// balber_berutil.h                                                     -*-C++-*-
#ifndef INCLUDED_BALBER_BERUTIL
#define INCLUDED_BALBER_BERUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functions to encode and decode simple types in BER format.
//
//@CLASSES:
//   balber::BerUtil: namespace of utility functions for BER
//
//@SEE_ALSO: balber_berencoder, balber_berdecoder
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
// primitive constructs.  Clients should use the 'balber_berencoder' and
// 'balber_berdecoder' components (which use this component in the
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
//  #include <balber_berconstants.h>
//  #include <balber_berutil.h>
//  #include <bdlsb_fixedmeminstreambuf.h>
//  #include <bdlsb_memoutstreambuf.h>
//
//  using namespace BloombergLP;
//
//  void usageExample()
//  {
//      bdlsb::MemOutStreamBuf osb;
//
//      balber::BerConstants::TagClass tagClass  =
//                                         balber::BerConstants::CONTEXT_SPECIFIC;
//      balber::BerConstants::TagType  tagType   = balber::BerConstants::PRIMITIVE;
//      int                         tagNumber = 31;
//
//      int retCode = balber::BerUtil::putIdentifierOctets(&osb,
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
//      bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());     // NO COPY
//
//      balber::BerConstants::TagClass tagClassIn;
//      balber::BerConstants::TagType  tagTypeIn;
//      int                         tagNumberIn;
//      int                         numBytesConsumed = 0;
//
//      retCode = balber::BerUtil::getIdentifierOctets(&isb,
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

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BALBER_BERCONSTANTS
#include <balber_berconstants.h>
#endif

#ifndef INCLUDED_BALBER_BERCONSTANTS
#include <balber_berconstants.h>
#endif

#ifndef INCLUDED_BALBER_BERENCODEROPTIONS
#include <balber_berencoderoptions.h>
#endif

#ifndef INCLUDED_BDLPUXXX_ISO8601
#include <bdlt_iso8601util.h>
#endif

#ifndef INCLUDED_BDLB_VARIANT
#include <bdlb_variant.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BDLB_XXXBITUTIL
#include <bdlb_xxxbitutil.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
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



namespace BloombergLP {



// Updated by 'bde-replace-bdet-forward-declares.py -m bdlt': 2015-02-03
// Updated declarations tagged with '// bdet -> bdlt'.

namespace bdlt { class Date; }                                  // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Date Date;                    // bdet -> bdlt
}  // close package namespace

namespace bdlt { class Datetime; }                              // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Datetime Datetime;            // bdet -> bdlt
}  // close package namespace

namespace bdlt { class DatetimeTz; }                            // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::DatetimeTz DatetimeTz;        // bdet -> bdlt
}  // close package namespace

namespace bdlt { class DateTz; }                                // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::DateTz DateTz;                // bdet -> bdlt
}  // close package namespace

namespace bdlt { class Time; }                                  // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Time Time;                    // bdet -> bdlt
}  // close package namespace

namespace bdlt { class TimeTz; }                                // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::TimeTz TimeTz;                // bdet -> bdlt
}  // close package namespace

namespace balber {
                            // ===================
                            // struct BerUtil
                            // ===================

struct BerUtil {
    // This utility contains functions to encode and decode primitive BER
    // constructs and simple value semantic types.  By convention, all
    // functions return 0 on success, and a non-zero value otherwise.  Also by
    // convention, all the "get" functions take an 'accumNumBytesConsumed';
    // each of the functions will add to this variable the number of bytes
    // consumed within the scope of the function.

    enum {
        BDEM_INDEFINITE_LENGTH = -1  // used to indicate that the length is
                                     // indefinite

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , INDEFINITE_LENGTH = BDEM_INDEFINITE_LENGTH
#endif // BDE_OMIT_INTERNAL_DEPRECATED
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
                           BerConstants::TagClass *tagClass,
                           BerConstants::TagType  *tagType,
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
                                   BerConstants::TagClass  tagClass,
                                   BerConstants::TagType   tagType,
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
                        const BerEncoderOptions *options = 0);
        // Encode the specified 'value' to the specified 'streamBuf'.  Return 0
        // on success, and a non-zero value otherwise.  Note that the value
        // consists of the length and contents primitives.  Also note that only
        // fundamental C++ types, 'bsl::string', 'bslstl::StringRef' and BDE
        // date/time types are supported.
};

// ---  Anything below this line is implementation specific.  Do not use.  ----

                        // =======================
                        // struct BerUtil_Imp
                        // =======================

struct BerUtil_Imp {
    // This 'struct' contains implementation functions used by the namespace
    // 'BerUtil'.

  private:
    // CONSTANTS
    enum {
        // This 'enum' lists constants that are used for encoding date and
        // time types in a binary encoding format.

        MAX_BINARY_DATE_LENGTH             = 3,
        MAX_BINARY_TIME_LENGTH             = 4,
        MAX_BINARY_DATETIME_LENGTH         = 6,

        MIN_BINARY_DATETZ_LENGTH           = MAX_BINARY_DATE_LENGTH + 1,
        MIN_BINARY_TIMETZ_LENGTH           = MAX_BINARY_TIME_LENGTH + 1,
        MIN_BINARY_DATETIMETZ_LENGTH       = MAX_BINARY_DATETIME_LENGTH + 1,

        MAX_BINARY_DATETZ_LENGTH           = 5,
        MAX_BINARY_TIMETZ_LENGTH           = 6,
        MAX_BINARY_DATETIMETZ_LENGTH       = 9
    };

    struct BinaryDateTimeFormat {
        // This 'struct' provides a function that specifies the maximum length
        // required to encode an object in the binary BER (octet string)
        // format.

        template <typename TYPE>
        static int maxLength();
            // Return the maximum length, in bytes, required to encode an
            // object of the templated 'TYPE' in the binary ber format.
    };

    struct StringDateTimeFormat {
        // This 'struct' provides a function that specifies the maximum length
        // required to encode an object in the ISO 8601 BER (visible string)
        // format.

        template <typename TYPE>
        static int maxLength();
            // Return the maximum length, in bytes, required to encode an
            // object of the templated 'TYPE' in the ISO 8601 format.
    };

  public:
    enum {
        INDEFINITE_LENGTH       = BerUtil::BDEM_INDEFINITE_LENGTH,
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
    static int getValue(bsl::streambuf    *streamBuf,
                        bslstl::StringRef *value,
                        int                length);
    static int getValue(bsl::streambuf *streamBuf,
                        bdlt::Date      *value,
                        int             length);
    static int getValue(bsl::streambuf *streamBuf,
                        bdlt::Datetime  *value,
                        int             length);
    static int getValue(bsl::streambuf  *streamBuf,
                        bdlt::DatetimeTz *value,
                        int              length);
    static int getValue(bsl::streambuf *streamBuf,
                        bdlt::DateTz    *value,
                        int             length);
    static int getValue(bsl::streambuf *streamBuf,
                        bdlt::Time      *value,
                        int             length);
    static int getValue(bsl::streambuf *streamBuf,
                        bdlt::TimeTz    *value,
                        int             length);
    template <typename TYPE, typename TYPETZ>
    static int getValue(bsl::streambuf               *streamBuf,
                        bdlb::Variant2<TYPE, TYPETZ> *value,
                        int                           length);

    static int numBytesToStream(short value);
    static int numBytesToStream(int value);
    static int numBytesToStream(long long value);
    template <typename TYPE>
    static int numBytesToStream(TYPE value);
        // The behavior is undefined unless 'sizeof(TYPE) > 1'.

    static int getBinaryDateValue(bsl::streambuf *streamBuf,
                                  bdlt::Date      *value,
                                  int             length);
    static int getBinaryTimeValue(bsl::streambuf *streamBuf,
                                  bdlt::Time      *value,
                                  int             length);
    static int getBinaryDatetimeValue(bsl::streambuf *streamBuf,
                                      bdlt::Datetime  *value,
                                      int             length);
    static int getBinaryDateTzValue(bsl::streambuf *streamBuf,
                                    bdlt::DateTz    *value,
                                    int             length);
    static int getBinaryTimeTzValue(bsl::streambuf *streamBuf,
                                    bdlt::TimeTz    *value,
                                    int             length);
    static int getBinaryDatetimeTzValue(bsl::streambuf  *streamBuf,
                                        bdlt::DatetimeTz *value,
                                        int              length);

    static int putBinaryDateValue(bsl::streambuf   *streamBuf,
                                  const bdlt::Date&  value);
    static int putBinaryTimeValue(bsl::streambuf   *streamBuf,
                                  const bdlt::Time&  value);
    static int putBinaryDatetimeValue(bsl::streambuf       *streamBuf,
                                      const bdlt::Datetime&  value);
    static int putBinaryDateTzValue(bsl::streambuf     *streamBuf,
                                    const bdlt::DateTz&  value);
    static int putBinaryTimeTzValue(bsl::streambuf     *streamBuf,
                                    const bdlt::TimeTz&  value);
    static int putBinaryDatetimeTzValue(bsl::streambuf         *streamBuf,
                                        const bdlt::DatetimeTz&  value);

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
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        bool                          value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        char                          value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        unsigned char                 value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        signed char                   value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        float                         value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        double                        value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        const bsl::string&            value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        const bslstl::StringRef&      value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        const bdlt::Date&              value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        const bdlt::Datetime&          value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        const bdlt::DatetimeTz&        value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        const bdlt::DateTz&            value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        const bdlt::Time&              value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf               *streamBuf,
                        const bdlt::TimeTz&            value,
                        const BerEncoderOptions *options);
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                            // -------------------
                            // struct balber::BerUtil
                            // -------------------

// PRIVATE CLASS METHODS
template <>
inline
int BerUtil_Imp::BinaryDateTimeFormat::maxLength<bdlt::Date>()
{
    return MAX_BINARY_DATE_LENGTH;
}

template <>
inline
int BerUtil_Imp::BinaryDateTimeFormat::maxLength<bdlt::Time>()
{
    return MAX_BINARY_TIME_LENGTH;
}

template <>
inline
int BerUtil_Imp::BinaryDateTimeFormat::maxLength<bdlt::Datetime>()
{
    return MAX_BINARY_DATETIME_LENGTH;
}

template <>
inline
int BerUtil_Imp::BinaryDateTimeFormat::maxLength<bdlt::DateTz>()
{
    return MAX_BINARY_DATETZ_LENGTH;
}

template <>
inline
int BerUtil_Imp::BinaryDateTimeFormat::maxLength<bdlt::TimeTz>()
{
    return MAX_BINARY_TIMETZ_LENGTH;
}

template <>
inline
int BerUtil_Imp::BinaryDateTimeFormat::maxLength<bdlt::DatetimeTz>()
{
    return MAX_BINARY_DATETIMETZ_LENGTH;
}

template <>
inline
int BerUtil_Imp::StringDateTimeFormat::maxLength<bdlt::Date>()
{
    return bdlt::Iso8601Util::BDEPU_DATE_STRLEN;
}

template <>
inline
int BerUtil_Imp::StringDateTimeFormat::maxLength<bdlt::Time>()
{
    return bdlt::Iso8601Util::BDEPU_TIME_STRLEN;
}

template <>
inline
int BerUtil_Imp::StringDateTimeFormat::maxLength<bdlt::Datetime>()
{
    return bdlt::Iso8601Util::BDEPU_DATETIME_STRLEN;
}

template <>
inline
int BerUtil_Imp::StringDateTimeFormat::maxLength<bdlt::DateTz>()
{
    return bdlt::Iso8601Util::BDEPU_DATETZ_STRLEN;
}

template <>
inline
int BerUtil_Imp::StringDateTimeFormat::maxLength<bdlt::TimeTz>()
{
    return bdlt::Iso8601Util::BDEPU_TIMETZ_STRLEN;
}

template <>
inline
int BerUtil_Imp::StringDateTimeFormat::maxLength<bdlt::DatetimeTz>()
{
    return bdlt::Iso8601Util::BDEPU_DATETIMETZ_STRLEN;
}

// CLASS METHODS
inline
int BerUtil::getEndOfContentOctets(bsl::streambuf *streamBuf,
                                        int            *accumNumBytesConsumed)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    *accumNumBytesConsumed += 2;

    return 0 == streamBuf->sbumpc() && 0 == streamBuf->sbumpc()
         ? BDEM_SUCCESS
         : BDEM_FAILURE;
}

inline
int BerUtil::getLength(bsl::streambuf *streamBuf,
                            int            *result,
                            int            *accumNumBytesConsumed)
{
    return BerUtil_Imp::getLength(streamBuf,
                                       result,
                                       accumNumBytesConsumed);
}

template <typename TYPE>
inline
int BerUtil::getValue(bsl::streambuf *streamBuf,
                           TYPE           *value,
                           int             length)
{
    return BerUtil_Imp::getValue(streamBuf, value, length);
}

template <typename TYPE>
inline
int BerUtil::getValue(bsl::streambuf *streamBuf,
                           TYPE           *value,
                           int            *accumNumBytesConsumed)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    int length;
    if (BerUtil_Imp::getLength(streamBuf, &length, accumNumBytesConsumed))
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
int BerUtil::putEndOfContentOctets(bsl::streambuf *streamBuf)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    return 0 == streamBuf->sputc(0) && 0 == streamBuf->sputc(0)
         ? BDEM_SUCCESS
         : BDEM_FAILURE;
}

inline
int BerUtil::putIndefiniteLengthOctet(bsl::streambuf *streamBuf)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    // "extra" unsigned char cast needed to suppress warning on Windows.

    return BerUtil_Imp::INDEFINITE_LENGTH_OCTET
               == streamBuf->sputc(static_cast<char>(
                  (unsigned char)(BerUtil_Imp::INDEFINITE_LENGTH_OCTET)))
               ? BDEM_SUCCESS
               : BDEM_FAILURE;
}

inline
int BerUtil::putLength(bsl::streambuf *streamBuf, int length)
{
    return BerUtil_Imp::putLength(streamBuf, length);
}

template <typename TYPE>
inline
int BerUtil::putValue(bsl::streambuf               *streamBuf,
                           const TYPE&                   value,
                           const BerEncoderOptions *options)
{
    return BerUtil_Imp::putValue(streamBuf, value, options);
}

                      // -----------------------
                      // struct BerUtil_Imp
                      // -----------------------

// CLASS METHODS
template <typename TYPE>
int BerUtil_Imp::getIntegerValue(bsl::streambuf *streamBuf,
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
int BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
                               TYPE           *value,
                               int             length)
{
    return BerUtil_Imp::getIntegerValue(streamBuf, value, length);
}

inline
int BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
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
int BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
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
int BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
                               unsigned char  *value,
                               int             length)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    short temp;
    if (BerUtil_Imp::getIntegerValue(streamBuf, &temp, length)) {
        return BDEM_FAILURE;                                          // RETURN
    }
    *value = (unsigned char) temp;
    return BDEM_SUCCESS;
}

inline
int BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
                               signed char    *value,
                               int             length)
{
    return getValue(streamBuf, (char *) value, length);
}

inline
int BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
                               float          *value,
                               int             length)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    double dvalue;
    if (BerUtil_Imp::getDoubleValue(streamBuf, &dvalue, length)) {
        return BDEM_FAILURE;                                          // RETURN
    }
    *value = (float) dvalue;
    return BDEM_SUCCESS;
}

inline
int BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
                               double         *value,
                               int             length)
{
    return BerUtil_Imp::getDoubleValue(streamBuf, value, length);
}

template <typename TYPE, typename TYPETZ>
int BerUtil_Imp::getValue(bsl::streambuf               *streamBuf,
                               bdlb::Variant2<TYPE, TYPETZ> *value,
                               int                           length)
{
    BSLMF_ASSERT((bslmf::IsSame<bdlt::Date, TYPE>::VALUE
               && bslmf::IsSame<bdlt::DateTz, TYPETZ>::VALUE)
              || (bslmf::IsSame<bdlt::Time, TYPE>::VALUE
               && bslmf::IsSame<bdlt::TimeTz, TYPETZ>::VALUE)
              || (bslmf::IsSame<bdlt::Datetime, TYPE>::VALUE
               && bslmf::IsSame<bdlt::DatetimeTz, TYPETZ>::VALUE));

    const int MAX_BINARY_TYPETZ_LENGTH =
                                     BinaryDateTimeFormat::maxLength<TYPETZ>();
    const int MAX_BINARY_TYPE_LENGTH = BinaryDateTimeFormat::maxLength<TYPE>();
    const int MAX_STRING_TYPE_LENGTH = StringDateTimeFormat::maxLength<TYPE>();

    bool doesNotHaveTzOffset = length > MAX_BINARY_TYPETZ_LENGTH
                             ? length <= MAX_STRING_TYPE_LENGTH
                             : length <= MAX_BINARY_TYPE_LENGTH;

    if (doesNotHaveTzOffset) {
        // Decode into TYPE

        value->template createInPlace<TYPE>();
        return getValue(streamBuf,
                        &value->template the<TYPE>(),
                        length);                                      // RETURN
    }

    // Decode into TYPETZ

    value->template createInPlace<TYPETZ>();
    return getValue(streamBuf, &value->template the<TYPETZ>(), length);
}

template <typename TYPE>
int BerUtil_Imp::numBytesToStream(TYPE value)
{
    int numBytes = sizeof(TYPE);

    BSLMF_ASSERT(sizeof(TYPE) > 1);

    // The 2 double casts to 'TYPE' in this function are necessary because if
    // the type is 64 bits the innermost cast is need to widen the constant
    // before the shift, and the outermost cast is needed if the type is
    // narrower than 'int'.

    static const TYPE NEG_MASK = static_cast<TYPE>(
             static_cast<TYPE>(0xff80) << ((sizeof(TYPE)-2) * BITS_PER_OCTET));
    if (0 == value) {
        numBytes = 1;
    }
    else if (value > 0) {
        static const TYPE SGN_BIT = static_cast<TYPE>(
                  static_cast<TYPE>(1) << (sizeof(TYPE) * BITS_PER_OCTET - 1));
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
int BerUtil_Imp::putIntegerGivenLength(bsl::streambuf *streamBuf,
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

#if BSLS_PLATFORM_IS_BIG_ENDIAN
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
int BerUtil_Imp::putIntegerValue(bsl::streambuf *streamBuf,
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
int BerUtil_Imp::putStringValue(bsl::streambuf *streamBuf,
                                     const char     *value,
                                     int             valueLength)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    return BerUtil_Imp::putLength(streamBuf, valueLength)
        || valueLength != streamBuf->sputn(value, valueLength)
         ? BDEM_FAILURE
         : BDEM_SUCCESS;
}

template <typename TYPE>
inline
int BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               const TYPE&                   value,
                               const BerEncoderOptions *)
{
    return BerUtil_Imp::putIntegerValue(streamBuf, value);
}

inline
int BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               bool                          value,
                               const BerEncoderOptions *)
{
    // It has been observed in practice that 'value' may refer to uninitialized
    // or overwritten memory, in which case its value may neither be 'true'
    // ('1') nor 'false' ('0').  We assert here to ensure that users get a
    // useful error message.  Note that we assert (rather than returning an
    // error code), as it is undefined behavior to examine the value of such an
    // uninitialized 'bool'.  Also note that gcc complains about this assert
    // when used with the '-Wlogical-op' flag.  Therefore, to silence this
    // warning/error we cast the 'bool' value to a 'char *' and check the value
    // referred to by the 'char *'.

    BSLMF_ASSERT(sizeof(bool) == sizeof(char));
    BSLS_ASSERT(0 == *(char *)&value || 1 == *(char *)&value);

    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    typedef bsl::streambuf::char_type char_type;

    return 1 == streamBuf->sputc(1)
        && (int)value == streamBuf->sputc((char_type)(value ? 1 : 0))
         ? BDEM_SUCCESS
         : BDEM_FAILURE;
}

inline
int BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               char                          value,
                               const BerEncoderOptions *)
{
    enum { BDEM_SUCCESS = 0, BDEM_FAILURE = -1 };

    return 1 == streamBuf->sputc(1)
        && (unsigned char) value == streamBuf->sputc(value)
         ? BDEM_SUCCESS
         : BDEM_FAILURE;
}

inline
int BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               unsigned char                 value,
                               const BerEncoderOptions *)
{
    return BerUtil_Imp::putIntegerValue(streamBuf, (unsigned short)value);
}

inline
int BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               signed char                   value,
                               const BerEncoderOptions *options)
{
    return putValue(streamBuf, (char) value, options);
}

inline
int BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               float                         value,
                               const BerEncoderOptions *)
{
    return BerUtil_Imp::putDoubleValue(streamBuf, (double) value);
}

inline
int BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               double                        value,
                               const BerEncoderOptions *)
{
    return BerUtil_Imp::putDoubleValue(streamBuf, value);
}

inline
int BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               const bsl::string&            value,
                               const BerEncoderOptions *)
{
    return putStringValue(streamBuf,
                          value.data(),
                          static_cast<int>(value.length()));
}

inline
int BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               const bslstl::StringRef&      value,
                               const BerEncoderOptions *)
{
    return putStringValue(streamBuf,
                          value.data(),
                          static_cast<int>(value.length()));
}
}  // close package namespace

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
