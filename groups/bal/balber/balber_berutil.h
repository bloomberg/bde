// balber_berutil.h                                                   -*-C++-*-
#ifndef INCLUDED_BALBER_BERUTIL
#define INCLUDED_BALBER_BERUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functions to encode and decode simple types in BER format.
//
//@CLASSES:
//   balber::BerUtil: namespace of utility functions for BER
//
//@SEE_ALSO: balber_berencoder, balber_berdecoder
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
//: o http://www.itu.int/ITU-T/studygroups/com17/languages/X.690-0207.pdf
//
// Note that this is a low-level component that only encodes and decodes
// primitive constructs.  Clients should use the 'balber_berencoder' and
// 'balber_berdecoder' components (which use this component in the
// implementation) to encode and decode well-formed BER messages.
//
///Terminology
///-----------
// The documentation of this component occasionally uses the following
// terminology as shorthand:
//
//: *date-and-time* *type*:
//:   A data type provided by BDE for the representation of a date and/or time
//:   value.  The date-and-time types are: 'bdlt::Date', 'bdlt::DateTz',
//:   'bdlt::Datetime', 'bdlt::DatetimeTz', 'bdlt::Time', and 'bdlt::TimeTz'.
//:   Note that under this definition, the time-zone-aware types provided by
//:   BDE, such as 'baltzo::LocalDatetime', are not date-and-time types.
//:
//: *date-and-time* *value*:
//:   The value associated with an object of a date-and-time type.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:  Reading and Writing Identifier Octets
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate the usage of this component.  Due
// to the low-level nature of this component, an extended usage example is not
// necessary.
//
// Suppose we wanted to write the identifier octets for a BER tag having the
// following properties:
//..
//    Tag Class:   Context-specific
//    Tag Type:    Primitive
//    Tag Number:  31
//..
// According to the BER specification, this should generate two octets
// containing the values 0x9F and 0x1F.  The following function demonstrates
// this:
//..
//  bdlsb::MemOutStreamBuf osb;
//
//  balber::BerConstants::TagClass tagClass  =
//                                    balber::BerConstants::e_CONTEXT_SPECIFIC;
//  balber::BerConstants::TagType  tagType   =
//                                           balber::BerConstants::e_PRIMITIVE;
//  int                            tagNumber = 31;
//
//  int retCode = balber::BerUtil::putIdentifierOctets(&osb,
//                                                     tagClass,
//                                                     tagType,
//                                                     tagNumber);
//  assert(0    == retCode);
//  assert(2    == osb.length());
//  assert(0x9F == (unsigned char)osb.data()[0]);
//  assert(0x1F == (unsigned char)osb.data()[1]);
//..
// The next part of the function will read the identifier octets from the
// stream and verify its contents:
//..
//  bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
//
//  balber::BerConstants::TagClass tagClassIn;
//  balber::BerConstants::TagType  tagTypeIn;
//  int                            tagNumberIn;
//  int                            numBytesConsumed = 0;
//
//  retCode = balber::BerUtil::getIdentifierOctets(&isb,
//                                                 &tagClassIn,
//                                                 &tagTypeIn,
//                                                 &tagNumberIn,
//                                                 &numBytesConsumed);
//  assert(0         == retCode);
//  assert(2         == numBytesConsumed);
//  assert(tagClass  == tagClassIn);
//  assert(tagType   == tagTypeIn);
//  assert(tagNumber == tagNumberIn);
//..

#include <balscm_version.h>

#include <balber_berconstants.h>
#include <balber_berdecoderoptions.h>
#include <balber_berencoderoptions.h>

#include <bdldfp_decimal.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_datetz.h>
#include <bdlt_iso8601util.h>
#include <bdlt_prolepticdateimputil.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bdlb_variant.h>

#include <bslmf_assert.h>

#include <bsla_nodiscard.h>
#include <bsla_unreachable.h>

#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_review.h>

#include <bsl_streambuf.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace balber {

                               // ==============
                               // struct BerUtil
                               // ==============

struct BerUtil {
    // This utility contains functions to encode and decode primitive BER
    // constructs and simple value semantic types.  By convention, all
    // functions return 0 on success, and a non-zero value otherwise.  Also by
    // convention, all the "get" functions take an 'accumNumBytesConsumed';
    // each of the functions will add to this variable the number of bytes
    // consumed within the scope of the function.

    enum {
        k_INDEFINITE_LENGTH = -1  // used to indicate that the length is
                                  // indefinite

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        ,
        BDEM_INDEFINITE_LENGTH = k_INDEFINITE_LENGTH,
        INDEFINITE_LENGTH      = k_INDEFINITE_LENGTH
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
    };

    // CLASS METHODS
    static int getEndOfContentOctets(bsl::streambuf *streamBuf,
                                     int            *accumNumBytesConsumed);
        // Decode the "end-of-content" octets (two consecutive zero-octets)
        // from the specified 'streamBuf' and add the number of bytes consumed
        // (which is always 2) to the specified 'accumNumBytesConsumed'.
        // Return 0 on success, and a non-zero value otherwise.

    static int getIdentifierOctets(
                                bsl::streambuf         *streamBuf,
                                BerConstants::TagClass *tagClass,
                                BerConstants::TagType  *tagType,
                                int                    *tagNumber,
                                int                    *accumNumBytesConsumed);
        // Decode the identifier octets from the specified 'streamBuf' and load
        // the tag class, tag type, and tag number to the specified 'tagClass',
        // 'tagType', and 'tagNumber' respectively.  Add the number of bytes
        // consumed to the specified 'accumNumBytesConsumed'.  Return
        // 0 on success, and a non-zero value otherwise.

    static int getLength(bsl::streambuf *streamBuf,
                         int            *result,
                         int            *accumNumBytesConsumed);
        // Decode the length octets from the specified 'streamBuf' and load the
        // result to the specified 'result'.  If the length is indefinite
        // (i.e., contents will be terminated by "end-of-content" octets) then
        // 'result' will be set to 'k_INDEFINITE_LENGTH'.  Add the number of
        // bytes consumed to the specified 'accumNumBytesConsumed'.  Return 0
        // on success, and a non-zero value otherwise.

    template <typename TYPE>
    static int getValue(
                      bsl::streambuf           *streamBuf,
                      TYPE                     *value,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
        // Decode the specified 'value' from the specified 'streamBuf',
        // consuming exactly the specified 'length' bytes.  Return 0 on
        // success, and a non-zero value otherwise.  Optionally specify
        // decoding 'options' to control aspects of the decoding.  Note that
        // the value consists of the contents bytes only (no length prefix).
        // Also note that only fundamental C++ types, 'bsl::string', and BDE
        // date/time types are supported.

    template <typename TYPE>
    static int getValue(
                      bsl::streambuf           *streamBuf,
                      TYPE                     *value,
                      int                      *accumNumBytesConsumed,
                      const BerDecoderOptions&  options = BerDecoderOptions());
        // Decode the specified 'value' from the specified 'streamBuf' and add
        // the number of bytes consumed to the specified
        // 'accumNumBytesConsumed'.  Return 0 on success, and a non-zero value
        // otherwise.  Optionally specify decoding 'options' to control aspects
        // of the decoding.  Note that the value consists of the length and
        // contents primitives.  Also note that only fundamental C++ types,
        // 'bsl::string', and BDE date/time types are supported.

    static int putEndOfContentOctets(bsl::streambuf *streamBuf);
        // Encode the "end-of-content" octets (two consecutive zero-octets) to
        // the specified 'streamBuf'.  The "end-of-content" octets act as the
        // termination bytes for objects that have indefinite length.  Return 0
        // on success, and a non-zero value otherwise.

    static int putIdentifierOctets(bsl::streambuf         *streamBuf,
                                   BerConstants::TagClass  tagClass,
                                   BerConstants::TagType   tagType,
                                   int                     tagNumber);
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
    static int putValue(bsl::streambuf          *streamBuf,
                        const TYPE&              value,
                        const BerEncoderOptions *options = 0);
        // Encode the specified 'value' to the specified 'streamBuf'.  Return 0
        // on success, and a non-zero value otherwise.  Note that the value
        // consists of the length and contents primitives.  Also note that only
        // fundamental C++ types, 'bsl::string', 'bslstl::StringRef' and BDE
        // date/time types are supported.
};

///Implementation Note
///-------------------
// The following utility structs used in the implementation of 'BerUtil' are
// provided in reverse dependency order.  This means that low-level utilities
// appear first, and higher-level utilities later.  No utility uses another
// that appears later.
//
// Each utility provides type aliases for the lower-level utilities used in its
// implementation.  This set of type aliases also serves as a manifest of the
// utility's dependencies.

                          // ========================
                          // struct BerUtil_Constants
                          // ========================

struct BerUtil_Constants {
    // This component-private utility 'struct' provides a namespace for a set
    // of constants used to calculate quantities needed by BER encoders and
    // decoders.  For example, this struct provides a named constant for the
    // number of bits in a byte, which is used in downstream calculations.

    // TYPES
    enum { k_NUM_BITS_PER_OCTET = 8 };
};

                        // ============================
                        // struct BerUtil_StreambufUtil
                        // ============================

struct BerUtil_StreambufUtil {
    // This component-private utility 'struct' provides a namespace for a suite
    // of functions used by 'BerUtil' to perform input and output operations on
    // 'bsl::streambuf' objects.  Note that these functions are intended to
    // adapt the standard stream-buffer operations to a BDE-style interface.

    // CLASS METHODS
    static int peekChar(char *value, bsl::streambuf *streamBuf);
        // Read the next byte from the specified 'streamBuf' without advancing
        // the read position and load that byte into the specified 'value'.
        // Return 0 on success, and a non-zero value otherwise.  If this
        // operation is not successful, the value of '*value' is unchanged.
        // This operation fails if the input sequence of 'streamBuf' is at its
        // end.

    static int getChars(char           *buffer,
                        bsl::streambuf *streamBuf,
                        int             bufferLength);
        // Read the specified 'bufferLength' number of bytes from the input
        // sequence of the specified 'streamBuf', as if by a call to
        // 'streamBuf->sgetn(buffer, bufferLength)', and load the bytes into
        // successive elements of the specified 'buffer', starting at the first
        // element.  Return 0 on success, and a non-zero value otherwise.  The
        // operation succeeds if 'length' bytes are successfully read from the
        // input sequence of the 'streamBuf' without the read position becoming
        // unavailable.  If less than 'bufferLength' bytes are read, the number
        // of bytes loaded into 'buffer' is not specified.  The behavior is
        // undefined unless '0 <= bufferLength' and 'buffer' is the address of
        // a sequence of at least 'bufferLength' bytes.

    static int putChars(bsl::streambuf *streamBuf,
                        const char     *buffer,
                        int             bufferLength);
        // Write the first specified 'bufferLength' number of bytes from the
        // specified 'buffer' to the specified 'streamBuf', as if by a call to
        // 'streamBuf->sputn(buffer, bufferLength)'.  Return 0 on success, and
        // a non-zero value otherwise.
};

                      // ================================
                      // struct BerUtil_IdentifierImpUtil
                      // ================================

struct BerUtil_IdentifierImpUtil {
    // This component-private utility 'struct' provides a namespace for a suite
    // of functions used by 'BerUtil' to implement BER identifier octet
    // encoding and decoding.

    // TYPES
    typedef BerUtil_Constants Constants;
        // 'Constants' is an alias to a namespace for a suite of
        // general-purpose constants that occur when encoding or decoding BER
        // data.

  private:
    // PRIVATE TYPES
    enum {
        k_TAG_CLASS_MASK  = 0xC0,  // 0xC0 = 0b1100'0000
        k_TAG_TYPE_MASK   = 0x20,  // 0x20 = 0b0010'0000
        k_TAG_NUMBER_MASK = 0x1F,  // 0x1F = 0b0001'1111
        // The first octet in a sequence of one or more BER identifier
        // octets encodes 3 quantities: the tag class, the tag type, and
        // the leading bits of the tag number.  These quantities are
        // encoded according to the packing suggested by the above 3 masks.

        k_MAX_TAG_NUMBER_IN_ONE_OCTET = 30,
        // The last 5 bits of the first octet in a sequence of one or more
        // BER identifier octets encodes one of 32 different values.
        // Values 0 through 30 indicate the tag number of the contents is
        // the corresponding value.  The value 31 indicates that the next
        // octet is the first byte in a possibly multi-byte encoding of an
        // 8-bit VLQ.

        k_NUM_VALUE_BITS_IN_TAG_OCTET = 7,
        // BER identifier octets (after the first octet) encode an 8-bit
        // VLQ unsigned integer value that indicates the tag number of the
        // contents.  The most significant bit of this octet indicates
        // whether or not the octet is the last one in the VLQ sequence, or
        // if another VLQ octet follows.

        k_MAX_TAG_NUMBER_OCTETS =
            (sizeof(int) * Constants::k_NUM_BITS_PER_OCTET) /
                k_NUM_VALUE_BITS_IN_TAG_OCTET +
            1,
        // This component restricts the maximum supported number of octets
        // used to represent the tag number to 4.  This means that there
        // are at most '4 * 7 = 28' bits used to encode such a tag number.

        k_CHAR_MSB_MASK = 0x80,  // 0x80 = 0b1000'0000
        // An 8-bit mask for the most significant bit of an octet.

        k_SEVEN_BITS_MASK = 0x7F  // 0x7F = 0b0111'1111
        // An 8-bit mask for all but the most significant bit of an octet.
    };

  public:
    // CLASS METHODS
    static int getIdentifierOctets(
                                 BerConstants::TagClass *tagClass,
                                 BerConstants::TagType  *tagType,
                                 int                    *tagNumber,
                                 int                    *accumNumBytesConsumed,
                                 bsl::streambuf         *streamBuf);
        // Decode the identifier octets from the specified 'streamBuf' and load
        // the tag class, tag type, and tag number to the specified 'tagClass',
        // 'tagType', and 'tagNumber', respectively.  Add the number of bytes
        // consumed to the specified 'accumNumBytesConsumed'.  Return 0 on
        // success, and a non-zero value otherwise.

    static int putIdentifierOctets(bsl::streambuf         *streamBuf,
                                   BerConstants::TagClass  tagClass,
                                   BerConstants::TagType   tagType,
                                   int                     tagNumber);
        // Encode the identifier octets for the specified 'tagClass', 'tagType'
        // and 'tagNumber', in that order, to the specified 'streamBuf'.
        // Return 0 on success, and a non-zero value otherwise.
};

                      // ================================
                      // struct BerUtil_RawIntegerImpUtil
                      // ================================

struct BerUtil_RawIntegerImpUtil {
    // This component-private utility 'struct' provides a namespace for a suite
    // of functions used by 'BerUtil' to implement BER integer encoding.  This
    // 'struct' is separate from 'BerUtil_IntegerImpUtil' to break a dependency
    // cycle between 'BerUtil_IntegerImpUtil' and 'BerUtil_LengthImpUtil'.

    // TYPES
    typedef BerUtil_Constants Constants;
        // 'Constants' is an alias to a namespace for a suite of
        // general-purpose constants that occur when encoding or decoding BER
        // data.

    // CLASS METHODS
    template <class INTEGRAL_TYPE>
    static int putIntegerGivenLength(bsl::streambuf *streamBuf,
                                     INTEGRAL_TYPE   value,
                                     int             length);
        // Encode the octets used in the BER encoding of the specified 'value'
        // of the specified 'INTEGRAL_TYPE' to the specified 'streamBuf', using
        // exactly the specified 'length' number of octets.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless 'INTEGRAL_TYPE' is fundamental integral type and exactly
        // 'length' number of octets is used in the BER encoding of the
        // specified 'value'.
};

                        // ============================
                        // struct BerUtil_LengthImpUtil
                        // ============================

struct BerUtil_LengthImpUtil {
    // This component-private utility 'struct' provides a namespace for a suite
    // of functions used by 'BerUtil' to implement BER length quantity encoding
    // and decoding.

    // TYPES
    typedef BerUtil_Constants Constants;
        // 'Constants' is an alias to a namespace for a suite of
        // general-purpose constants that occur when encoding or decoding BER
        // data.

    typedef BerUtil_RawIntegerImpUtil RawIntegerUtil;
        // 'RawIntegerUtil' is an alias to a namespace for a suite of functions
        // used to implement integer encoding.

  private:
    // PRIVATE TYPES
    enum {
        k_INDEFINITE_LENGTH = -1,
        // constant used to indicate that a calculated length quantity is
        // indefinite

        k_INDEFINITE_LENGTH_OCTET = 0x80,  // 0x80 = 0b1000'0000
        // value of the (singular) length octet used to indicate that
        // the length of a sequence of BER octets will be determined by
        // seeking forward until and end-of-contents pair of octets is
        // encountered

        k_LONG_FORM_LENGTH_FLAG_MASK = 0x80,  // 0x80 = 0b1000'0000
        // mask used to determine if the higher-order bit of a length
        // octet indicates that the next octet in the sequence is part of
        // the VLQ-encoding of the length or if the current octet is the
        // final octet of the length octets

        k_LONG_FORM_LENGTH_VALUE_MASK = 0x7F  // 0x7F = 0b0111'1111
        // mask used to retrieve the bits of a non-final length octet
        // that contribute to the BLQ-encoding of the length
    };

  public:
    // CLASS METHODS

    // Length Decoding Functions

    static int getLength(int            *result,
                         int            *accumNumBytesConsumed,
                         bsl::streambuf *streamBuf);
        // Decode the length octets from the specified 'streamBuf' and load the
        // result to the specified 'result'.  If the length is indefinite
        // (i.e., contents will be terminated by "end-of-content" octets) then
        // 'result' will be set to 'k_INDEFINITE_LENGTH'.  Add the number of
        // bytes consumed to the specified 'accumNumBytesConsumed'.  Return 0
        // on success, and a non-zero value otherwise.

    static int getEndOfContentOctets(int            *accumNumBytesConsumed,
                                     bsl::streambuf *streamBuf);
        // Decode the "end-of-content" octets (two consecutive zero-octets)
        // from the specified 'streamBuf' and add the number of bytes consumed
        // (which is always 2) to the specified 'accumNumBytesConsumed'.
        // Return 0 on success, and a non-zero value otherwise.

    // Length Encoding Functions

    static int putLength(bsl::streambuf *streamBuf, int length);
        // Encode the specified 'length' length octets to the specified
        // 'streamBuf'.  Return 0 on success, and a non-zero value otherwise.
        // The behavior is undefined unless '0 <= length'.

    static int putIndefiniteLengthOctet(bsl::streambuf *streamBuf);
        // Encode the "indefinite-length" octet onto the specified 'streamBuf'.
        // This octet signifies that the length of the contents is indefinite
        // (i.e., contents will be terminated by end of content octets).
        // Return 0 on success, and a non-zero value otherwise.

    static int putEndOfContentOctets(bsl::streambuf *streamBuf);
        // Encode the identifier octets for the specified 'tagClass', 'tagType'
        // and 'tagNumber' to the specified 'streamBuf'.  Return 0 on success,
        // and a non-zero value otherwise.
};

                       // =============================
                       // struct BerUtil_BooleanImpUtil
                       // =============================

struct BerUtil_BooleanImpUtil {
    // This component-private utility 'struct' provides a namespace for a suite
    // of functions used by 'BerUtil' to implement BER encoding and decoding
    // operations for boolean values.  Within the definition of this 'struct':
    //
    //: *the* *specification*:
    //:   Refers to the August 2015 revision of the ITU-T Recommendation X.690.

    // TYPES
    typedef BerUtil_LengthImpUtil LengthUtil;
        // 'LengthUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoding and decoding operations for length
        // quantities.

    // CLASS METHODS

    // Decoding

    static int getBoolValue(bool           *value,
                            bsl::streambuf *streamBuf,
                            int             length);
        // Decode to the specified 'value' from the specified 'streamBuf',
        // consuming exactly the specified 'length' bytes.  Return 0 on
        // success, and a non-zero value otherwise.  This operations succeeds
        // if 'length' bytes are successfully read from the 'streamBuf' and
        // they contain a valid representation of the contents octets for a
        // BER-encoded boolean value according to the specification.

    // Encoding

    static int putBoolValue(bsl::streambuf *streamBuf, bool value);
        // Encode the specified 'value' to the specified 'streamBuf'.  Return 0
        // on success and a non-zero value otherwise.  The 'value' is encoded
        // as the sequence of contents octets for a BER-encoded boolean value
        // according to the specification.  This operation succeeds if all of
        // the contents octets are successfully written to the 'streamBuf'.
};

                       // =============================
                       // struct BerUtil_IntegerImpUtil
                       // =============================

struct BerUtil_IntegerImpUtil {
    // This component-private utility 'struct' provides a namespace for a suite
    // of functions used by 'BerUtil' to implement BER encoding and decoding
    // operations for integer values.  Within the definition of this 'struct':
    //
    //: *the* *specification*:
    //:   Refers to the August 2015 revision of the ITU-T Recommendation X.690.

    // TYPES
    typedef BerUtil_Constants Constants;
        // 'Constants' is an alias to a namespace for a suite of
        // general-purpose constants that occur when encoding or decoding BER
        // data.

    typedef BerUtil_LengthImpUtil LengthImpUtil;
        // 'LengthUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoding and decoding operations for length
        // quantities.

    typedef BerUtil_RawIntegerImpUtil RawIntegerUtil;
        // 'RawIntegerUtil' is an alias to a namespace for a suite of low-level
        // functions used to implement BER encoding operations for integer
        // values.

    typedef BerUtil_StreambufUtil StreambufUtil;
        // 'StreambufUtil' is an alias to a namespace for a suite of functions
        // used to implement input and output operations on 'bsl::streambuf'
        // objects.

    enum {
        k_40_BIT_INTEGER_LENGTH = 5
        // the number of octets used to encode a signed integer value
        // in 40 bits
    };

    // CLASS METHODS
    static int getNumOctetsToStream(short value);
    static int getNumOctetsToStream(int value);
    static int getNumOctetsToStream(long long value);
        // Return the number of octets required to provide a BER encoding of
        // the specified 'value' according to the specification.

    template <class INTEGRAL_TYPE>
    static int getNumOctetsToStream(INTEGRAL_TYPE value);
        // Return the number of octets required to provide a BER encoding of
        // the specified 'value' according to the specification.  The program
        // is ill-formed unless the specified 'INTEGRAL_TYPE' is a fundamental
        // integral type.

    static int getIntegerValue(long long      *value,
                               bsl::streambuf *streamBuf,
                               int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // interpretation of those bytes as the contents octets of a
        // BER-encoded integer value according to the specification.  Return 0
        // if successful, and a non-zero value otherwise.

    template <class INTEGRAL_TYPE>
    static int getIntegerValue(INTEGRAL_TYPE  *value,
                               bsl::streambuf *streamBuf,
                               int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // interpretation of those bytes as the contents octets of BER-encoded
        // integer value according to the specification.  Return 0 if
        // successful, and a non-zero value otherwise.  The operation succeeds
        // if 'length' bytes are successfully read from the input sequence of
        // the 'streamBuf' without the read position becoming unavailable, and
        // the bytes read contain a valid representation of the contents octets
        // of an integer value according to the specification.  The program is
        // ill-formed unless the specified 'INTEGRAL_TYPE' is a fundamental
        // integral type.

    static int get40BitIntegerValue(bsls::Types::Int64 *value,
                                    bsl::streambuf     *streamBuf);
        // Read 5 bytes from the input sequence of the specified 'streamBuf'
        // and load to the specified 'value' the interpretation of those bytes
        // as a 40-bit, signed, 2's-complement, big-endian integer.  Return 0
        // if successful, and a non-zero value otherwise.  The operation
        // succeeds if all 5 bytes are successfully read from the input
        // sequence of the 'streamBuf' without the read position becoming
        // unavailable, and the bytes read contain a valid representation of a
        // 40-bit, signed, 2's-complement, big-endian integer.

    template <class INTEGRAL_TYPE>
    static int putIntegerValue(bsl::streambuf *streamBuf, INTEGRAL_TYPE value);
        // Write the length and contents octets of the BER encoding of the
        // specified integer 'value' (as defined in the specification) to the
        // output sequence of the specified 'streamBuf'.  Return 0 if
        // successful, and a non-zero value otherwise.  The operation succeeds
        // if all bytes corresponding to the length and contents octets are
        // written to the 'streamBuf' without the write position becoming
        // unavailable.  The program is ill-formed unless the specified
        // 'INTEGRAL_TYPE' is a fundamental integral type.

    static int put40BitIntegerValue(bsl::streambuf     *streamBuf,
                                    bsls::Types::Int64  value);
        // Write the 5 octets that comprise the 40-bit, signed, 2's-complement,
        // bit-endian representation of the specified integer 'value' to the
        // specified 'streamBuf'.  Return 0 if successful, and a non-zero value
        // otherwise.  The operation succeeds if all bytes corresponding to the
        // representation of the 'value' are written to the 'streamBuf' without
        // the write position becoming unavailable.  The behavior is undefined
        // unless the 'value' is in the half-open interval
        // '[-549755813888, 549755813888)'.

    template <class INTEGRAL_TYPE>
    static int putIntegerGivenLength(bsl::streambuf *streamBuf,
                                     INTEGRAL_TYPE   value,
                                     int             length);
        // Write exactly the specified 'length' number of contents octets of
        // the BER encoding of the specified integer 'value' (as defined in the
        // specification) to the output sequence of the specified 'streamBuf'.
        // Return 0 if successful, and a non-zero value otherwise.  The
        // operation succeeds if all bytes corresponding to the contents octets
        // are written to the 'streamBuf' without the write position becoming
        // unavailable.  The behavior is undefined unless there are exactly
        // 'length' number of contents octets used to encode the integer
        // 'value' according to the specification.  The program is ill-formed
        // unless the specified 'INTEGRAL_TYPE' is a fundamental integral type.
};

                      // ===============================
                      // struct BerUtil_CharacterImpUtil
                      // ===============================

struct BerUtil_CharacterImpUtil {
    // This component-private utility 'struct' provides a namespace for a suite
    // of functions used by 'BerUtil' to implement BER encoding and decoding
    // operations for byte values.  Within the definition of this 'struct':
    //
    //: *the* *specification*:
    //:   Refers to the August 2015 revision of the ITU-T Recommendation X.690.

    // TYPES
    typedef BerUtil_IntegerImpUtil IntegerUtil;
        // 'IntegerUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoding and decoding operations for integer
        // values.

    typedef BerUtil_LengthImpUtil LengthUtil;
        // 'LengthUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoding and decoding operations for length
        // quantities.

    // CLASS METHODS

    // Decoding

    static int getCharValue(char           *value,
                            bsl::streambuf *streamBuf,
                            int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // interpretation of those bytes as the value of the contents octets of
        // a BER-encoded integer according to the specification.  Return 0 if
        // successful, and a non-zero value otherwise.  The operation succeeds
        // if 'length' bytes are successfully read from the input sequence of
        // the 'streamBuf' without the read position becoming unavailable, and
        // the bytes read contain a valid representation of the contents octets
        // of an integer value according to the specification.  Note that the
        // signedness of the interpreted integer value is the same as the
        // signedness of 'char' according to the current platform.

    static int getSignedCharValue(signed char    *value,
                                  bsl::streambuf *streamBuf,
                                  int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // interpretation of those bytes as the value of the contents octets of
        // a BER-encoded integer according to the specification.  Return 0 if
        // successful, and a non-zero value otherwise.  The operation succeeds
        // if 'length' bytes are successfully read from the input sequence of
        // the 'streamBuf' without the read position becoming unavailable, and
        // the bytes read contain a valid representation of the contents octets
        // of an integer value according to the specification.

    static int getUnsignedCharValue(unsigned char  *value,
                                    bsl::streambuf *streamBuf,
                                    int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // interpretation of those bytes as the value of the contents octets of
        // a BER-encoded integer according to the specification.  Return 0 if
        // successful, and a non-zero value otherwise.  The operation succeeds
        // if 'length' bytes are successfully read from the input sequence of
        // the 'streamBuf' without the read position becoming unavailable, and
        // the bytes read contain a valid representation of the contents octets
        // of an integer value according to the specification.

    // Encoding

    static int putCharValue(bsl::streambuf *streamBuf, char value);
        // Write the length and contents octets of the BER encoding of the
        // specified integer 'value' (as defined in the specification) to the
        // output sequence of the specified 'streamBuf'.  Return 0 if
        // successful, and a non-zero value otherwise.  The operation succeeds
        // if all bytes corresponding to the length and contents octets are
        // written to the 'streamBuf' without the write position becoming
        // unavailable.

    static int putSignedCharValue(bsl::streambuf *streamBuf,
                                  signed char     value);
        // Write the length and contents octets of the BER encoding of the
        // specified integer 'value' (as defined in the specification) to the
        // output sequence of the specified 'streamBuf'.  Return 0 if
        // successful, and a non-zero value otherwise.  The operation succeeds
        // if all bytes corresponding to the length and contents octets are
        // written to the 'streamBuf' without the write position becoming
        // unavailable.

    static int putUnsignedCharValue(bsl::streambuf *streamBuf,
                                    unsigned char   value);
        // Write the length and contents octets of the BER encoding of the
        // specified integer 'value' (as defined in the specification) to the
        // output sequence of the specified 'streamBuf'.  Return 0 if
        // successful, and a non-zero value otherwise.  The operation succeeds
        // if all bytes corresponding to the length and contents octets are
        // written to the 'streamBuf' without the write position becoming
        // unavailable.
};

                    // ===================================
                    // struct BerUtil_FloatingPointImpUtil
                    // ===================================

struct BerUtil_FloatingPointImpUtil {
    // This component-private utility 'struct' provides a namespace for a suite
    // of functions used by 'BerUtil' to implement BER encoding and decoding
    // operations for floating point number values.  Within the definition of
    // this 'struct':
    //
    //: *the* *specification*:
    //:   Refers to the August 2015 revision of the ITU-T Recommendation X.690,
    //:   and
    //:
    //: *the* *floating* *point* *specification*:
    //:   Refers to the 2008 revision of the IEE 754 Standard for
    //:   Floating-Point Arithemtic.

    // TYPES
    typedef BerUtil_IntegerImpUtil IntegerUtil;
        // 'IntegerUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoding and decoding operations for integer
        // values.

    typedef BerUtil_LengthImpUtil LengthUtil;
        // 'LengthUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoding and decoding operations for length
        // quantities.

  private:
    // PRIVATE TYPES
    enum {
        k_MAX_MULTI_WIDTH_ENCODING_SIZE = 8,

        k_BINARY_NEGATIVE_NUMBER_ID = 0xC0,
        k_BINARY_POSITIVE_NUMBER_ID = 0x80,

        k_REAL_BINARY_ENCODING = 0x80,

        k_DOUBLE_EXPONENT_SHIFT              = 52,
        k_DOUBLE_OUTPUT_LENGTH               = 10,
        k_DOUBLE_EXPONENT_MASK_FOR_TWO_BYTES = 0x7FF,
        k_DOUBLE_NUM_EXPONENT_BITS           = 11,
        k_DOUBLE_NUM_MANTISSA_BITS           = 52,
        k_DOUBLE_NUM_EXPONENT_BYTES          = 2,
        k_DOUBLE_NUM_MANTISSA_BYTES          = 7,
        k_DOUBLE_BIAS                        = 1023,

        k_POSITIVE_INFINITY_ID = 0x40,
        k_NEGATIVE_INFINITY_ID = 0x41,
        k_NAN_ID               = 0x42,

        k_DOUBLE_INFINITY_EXPONENT_ID = 0x7FF,
        k_INFINITY_MANTISSA_ID        = 0,

        k_REAL_SIGN_MASK            = 0x40,  // 0x40 = 0b0100'0000
        k_REAL_BASE_MASK            = 0x20,  // 0x20 = 0b0010'0000
        k_REAL_SCALE_FACTOR_MASK    = 0x0C,  // 0x0C = 0b0000'1100
        k_REAL_EXPONENT_LENGTH_MASK = 0x03,  // 0x03 = 0b0000'0011

        k_BER_RESERVED_BASE       = 3,
        k_REAL_BASE_SHIFT         = 4,
        k_REAL_SCALE_FACTOR_SHIFT = 2,

        k_REAL_MULTIPLE_EXPONENT_OCTETS = 4
    };

    // PRIVATE CLASS METHODS

    // Utilities

    static void assembleDouble(double    *value,
                               long long  exponent,
                               long long  mantissa,
                               int        sign);
        // Load to the specified 'value' the value of the "binary64" object
        // having the specified 'exponent' value, the bits of the specified
        // 'mantissa' interpreted as the digits of the mantissa, and the value
        // of the specified 'sign' interpreted as the sign bit, according to
        // the floating point specification.  The behavior is undefined unless
        // 'exponent' is in the range '[-1023, 1023]', 'mantissa' is in the
        // range '[-9007199254740991, 9007199254740991]', and 'sign' is 0 or 1.
        // The program is ill-formed unless the platform uses the "binary64"
        // interchange format encoding defined in the floating point
        // specification as the object representation for 'double' values.

    static void normalizeMantissaAndAdjustExp(long long *mantissa,
                                              int       *exponent,
                                              bool       denormalized);
        // Normalize the specified '*mantissa' value by adjusting the implicit
        // decimal point to after the rightmost 1 bit in the mantissa.  If
        // 'false == denormalized' prepend the implicit 1 in the mantissa
        // before adjusting the implicit decimal point.  Multiply the
        // '*exponent' value by 2 to the power of the number of places the
        // implicit decimal point moves.

    static void parseDouble(int       *exponent,
                            long long *mantissa,
                            int       *sign,
                            double     value);
        // Parse the specified 'value' and populate the specified 'exponent',
        // 'mantissa', and 'sign' values from the exponent, mantissa, and sign
        // of the 'value', respectively.

  public:
    // CLASS METHODS

    // Decoding

    static int getFloatValue(float          *value,
                             bsl::streambuf *streamBuf,
                             int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // interpretation of those bytes as the contents octets of a
        // BER-encoded real value according to the specification.  Return 0 if
        // successful, and a non-zero value otherwise.  The operation succeeds
        // if 'length' bytes are successfully read from the input sequence of
        // the 'streamBuf' without the read position becoming unavailable, and
        // the bytes read contain a valid representation of the contents octets
        // of a real value according to the specification.

    static int getDoubleValue(double         *value,
                              bsl::streambuf *streamBuf,
                              int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // interpretation of those bytes as the contents octets of a
        // BER-encoded real value according to the specification.  Return 0 if
        // successful, and a non-zero value otherwise.  The operation succeeds
        // if 'length' bytes are successfully read from the input sequence of
        // the 'streamBuf' without the read position becoming unavailable, and
        // the bytes read contain a valid representation of the contents octets
        // of a real value according to the specification.

    static int getDecimal64Value(bdldfp::Decimal64 *value,
                                 bsl::streambuf    *streamBuf,
                                 int                length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // interpretation of those bytes as the contents octets of an encoded
        // 64-bit decimal value.  Return 0 if successful, and a non-zero value
        // otherwise.  The operation succeeds if 'length' bytes are
        // successfully read from the input sequence of the 'streamBuf' without
        // the read position becoming unavailable, and the bytes read contain a
        // valid representation of the contents octets of an encoded 64-bit
        // decimal value.  See the package-level documentation of {'balber'}
        // for the definition of the format used to encode 64-bit decimal
        // values.

    // Encoding

    static int putFloatValue(bsl::streambuf *streamBuf, float value);
        // Write the length and contents octets of the BER encoding of the
        // specified real 'value' (as defined in the specification) to the
        // output sequence of the specified 'streamBuf'.  Return 0 if
        // successful, and a non-zero value otherwise.  The operation succeeds
        // if all bytes corresponding to the length and contents octets are
        // written to the 'streamBuf' without the write position becoming
        // unavailable.

    static int putDoubleValue(bsl::streambuf *streamBuf, double value);
        // Write the length and contents octets of the BER encoding of the
        // specified real 'value' (as defined in the specification) to the
        // output sequence of the specified 'streamBuf'.  Return 0 if
        // successful, and a non-zero value otherwise.  The operation succeeds
        // if all bytes corresponding to the length and contents octets are
        // written to the 'streamBuf' without the write position becoming
        // unavailable.

    static int putDecimal64Value(bsl::streambuf    *streamBuf,
                                 bdldfp::Decimal64  value);
        // Write the length and contents octets of the encoding of the BER
        // encoding of the specified 'value' to the output sequence of the
        // specified 'streamBuf'.  Return 0 if successful, and a non-zero value
        // otherwise.  The operation succeeds if all bytes corresponding to the
        // length and contents octets are written to the 'streamBuf' without
        // the write position becoming unavailable.  See the package-level
        // documentation of {'balber'} for the definition of the format used to
        // encode 64-bit decimal values.
};

                        // ============================
                        // struct BerUtil_StringImpUtil
                        // ============================

struct BerUtil_StringImpUtil {
    // This component-private utility 'struct' provides a namespace for a suite
    // of functions used by 'BerUtil' to implement BER encoding and decoding
    // operations for string values.  Within the definition of this 'struct':
    //
    //: *the* *specification*:
    //:   Refers to the August 2015 revision of the ITU-T Recommendation X.690.

    // TYPES
    typedef BerUtil_LengthImpUtil LengthUtil;
        // 'LengthUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoding and decoding operations for length
        // quantities.

  public:
    // CLASS METHODS

    // Utilities

    static int putRawStringValue(bsl::streambuf *streamBuf,
                                 const char     *string,
                                 int             stringLength);
        // Write the length and contents octets of the BER encoding of the
        // specified byte 'string' having the specified 'stringLength' (as
        // defined in the specification) to the output sequence of the
        // specified 'streamBuf'.  Return 0 if successful, and a non-zero value
        // otherwise.  The operation succeeds if all bytes corresponding to the
        // length and contents octets are written to the 'streamBuf' without
        // the write position becoming unavailable.

    static int putChars(bsl::streambuf *streamBuf, char value, int numChars);
        // Write the specified 'numChars' number of bytes having the specified
        // 'value' to the output sequence of the specified 'streamBuf'.  Return
        // 0 if successful, and a non-zero value otherwise.  The operation
        // succeeds if all 'numChars' bytes are written to the 'streamBuf'
        // without the write position becoming unavailable.

    // 'bsl::string' Decoding

    static int getStringValue(bsl::string              *value,
                              bsl::streambuf           *streamBuf,
                              int                       length,
                              const BerDecoderOptions&  options);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // interpretation of those bytes as the value of the contents octets of
        // a BER-encoded character string (more specifically, an unrestricted
        // character string) according to the specification, unless an
        // alternate value is indicated by the specified 'options', in which
        // case, the alternate value is loaded.  If the 'DefaultEmptyStrings'
        // attribute of the 'options' is 'true' and the witnessed BER-encoded
        // character string represents the empty string value, the alternate
        // value is the current '*value', otherwise there is no alternate
        // value.  Return 0 if successful, and a non-zero value otherwise.  The
        // operation succeeds if 'length' bytes are successfully read from the
        // input sequence of the 'streamBuf' without the read position becoming
        // unavailable, and the bytes read contain a valid representation of
        // the contents octets of a character string value according to the
        // specification.

    // 'bsl::string' Encoding

    static int putStringValue(bsl::streambuf     *streamBuf,
                              const bsl::string&  value);
        // Write the length and contents octets of the BER encoding of the
        // specified character string 'value' (as defined in the specification)
        // to the output sequence of the specified 'streamBuf'.  Return 0 if
        // successful, and a non-zero value otherwise.  The operation succeeds
        // if all bytes corresponding to the length and contents octets are
        // written to the 'streamBuf' without the write position becoming
        // unavailable.

    // 'bslstl::StringRef' Encoding

    static int putStringRefValue(bsl::streambuf           *streamBuf,
                                 const bslstl::StringRef&  value);
        // Write the length and contents octets of the BER encoding of the
        // specified character string 'value' (as defined in the specification)
        // to the output sequence of the specified 'streamBuf'.  Return 0 if
        // successful, and a non-zero value otherwise.  The operation succeeds
        // if all bytes corresponding to the length and contents octets are
        // written to the 'streamBuf' without the write position becoming
        // unavailable.
};

                       // =============================
                       // struct BerUtil_Iso8601ImpUtil
                       // =============================

struct BerUtil_Iso8601ImpUtil {
    // This component-private utility 'struct' provides a namespace for a suite
    // of functions used by 'BerUtil' to implement BER encoding and decoding
    // operations for date and time values in the ISO 8601 format.  See the
    // component-level documentation of {'bdlt_iso8601util'} for a complete
    // description of the ISO 8601 format used by the functions provided by
    // this 'struct'.

    // TYPES
    typedef BerUtil_StringImpUtil StringUtil;
        // 'StringUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoder and decoding operations for string
        // values.

  private:
    // PRIVATE CLASS METHODS
    template <class TYPE>
    static int getValue(TYPE *value, bsl::streambuf *streamBuf, int length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // value represented by the interpretation of the bytes as an ISO 8601
        // date/time value.  The specified 'TYPE' defines the expected ISO 8601
        // date/time format, which is the format corresponding to the 'TYPE' as
        // specified in {'bdlt_iso8601util'}.  The operation succeeds if
        // 'length' bytes are successfully read from the input sequence of the
        // 'streamBuf' without the read position becoming unavailable, and the
        // bytes contain a valid representation of the expected ISO 8601
        // date/time format.  The program is ill-formed unless 'TYPE' is one
        // of: 'bdlt::Date', 'bdlt::DateTz', 'bdlt::Datetime',
        // 'bdlt::DatetimeTz', 'bdlt::Time', or 'bdlt::TimeTz'.

    template <class TYPE>
    static int putValue(bsl::streambuf          *streamBuf,
                        const TYPE&              value,
                        const BerEncoderOptions *options);
        // Write the ISO 8601 representation of the specified 'value' to the
        // output sequence of the specified 'streamBuf'.  If the specified
        // 'options' is 0, use 3 decimal places of fractional second precision,
        // otherwise use the number of decimal places specified by the
        // 'datetimeFractionalSecondPrecision' attribute of the 'options'.
        // Return 0 on success and a non-zero value otherwise.  The operation
        // succeeds if all bytes of the ISO 8601 representation of the 'value'
        // are written to the 'streamBuf' without the write position becoming
        // unavailable.  The program is ill-formed unless 'TYPE' is one of
        // 'bdlt::Date', 'bdlt::DateTz', 'bdlt::Datetime', 'bdlt::DatetimeTz',
        // 'bdlt::Time', or 'bdlt::TimeTz'.

  public:
    // CLASS METHODS

    // Decoding

    static int getDateValue(bdlt::Date     *value,
                            bsl::streambuf *streamBuf,
                            int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date value represented by the interpretation of the read bytes as an
        // ISO 8601 date.  Return 0 on success, and a non-zero value otherwise.
        // The operation succeeds if 'length' bytes are successfully read from
        // the input sequence of the 'streamBuf' without the read position
        // becoming unavailable, and the bytes contain a valid representation
        // of an ISO 8601 date.

    static int getDateTzValue(bdlt::DateTz   *value,
                              bsl::streambuf *streamBuf,
                              int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date value represented by the interpretation of the read bytes as an
        // ISO 8601 date and time zone.  Return 0 on success, and a non-zero
        // value otherwise.  The operation succeeds if 'length' bytes are
        // successfully read from the input sequence of the 'streamBuf' without
        // the read position becoming unavailable, and the bytes contain a
        // valid representation of an ISO 8601 date and time zone.

    static int getDatetimeValue(bdlt::Datetime *value,
                                bsl::streambuf *streamBuf,
                                int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date value represented by the interpretation of the read bytes as an
        // ISO 8601 date and time.  Return 0 on success, and a non-zero value
        // otherwise.  The operation succeeds if 'length' bytes are
        // successfully read from the input sequence of the 'streamBuf' without
        // the read position becoming unavailable, and the bytes contain a
        // valid representation of an ISO 8601 date and time.

    static int getDatetimeTzValue(bdlt::DatetimeTz *value,
                                  bsl::streambuf   *streamBuf,
                                  int               length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date value represented by the interpretation of the read bytes as an
        // ISO 8601 date, time, and time zone.  Return 0 on success, and a
        // non-zero value otherwise.  The operation succeeds if 'length' bytes
        // are successfully read from the input sequence of the 'streamBuf'
        // without the read position becoming unavailable, and the bytes
        // contain a valid representation of an ISO 8601 date, time, and time
        // zone.

    static int getTimeValue(bdlt::Time     *value,
                            bsl::streambuf *streamBuf,
                            int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date value represented by the interpretation of the read bytes as an
        // ISO 8601 time.  Return 0 on success, and a non-zero value otherwise.
        // The operation succeeds if 'length' bytes are successfully read from
        // the input sequence of the 'streamBuf' without the read position
        // becoming unavailable, and the bytes contain a valid representation
        // of an ISO 8601 time.

    static int getTimeTzValue(bdlt::TimeTz   *value,
                              bsl::streambuf *streamBuf,
                              int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date value represented by the interpretation of the read bytes as an
        // ISO 8601 time and time zone.  Return 0 on success, and a non-zero
        // value otherwise.  The operation succeeds if 'length' bytes are
        // successfully read from the input sequence of the 'streamBuf' without
        // the read position becoming unavailable, and the bytes contain a
        // valid representation of an ISO 8601 time and time zone.

    // Encoding

    static int putDateValue(bsl::streambuf          *streamBuf,
                            const bdlt::Date&        value,
                            const BerEncoderOptions *options);
        // Write the ISO 8601 representation of the specified 'value' to the
        // output sequence of the specified 'streamBuf'.  Return 0 on success
        // and a non-zero value otherwise.  The operation succeeds if all bytes
        // of the ISO 8601 representation of the 'value' are written to the
        // 'streamBuf' without the write position becoming unavailable.

    static int putDateTzValue(bsl::streambuf          *streamBuf,
                              const bdlt::DateTz&      value,
                              const BerEncoderOptions *options);
        // Write the ISO 8601 representation of the specified 'value' to the
        // output sequence of the specified 'streamBuf'.  Return 0 on success
        // and a non-zero value otherwise.  The operation succeeds if all bytes
        // of the ISO 8601 representation of the 'value' are written to the
        // 'streamBuf' without the write position becoming unavailable.

    static int putDatetimeValue(bsl::streambuf          *streamBuf,
                                const bdlt::Datetime&    value,
                                const BerEncoderOptions *options);
        // Write the ISO 8601 representation of the specified 'value' to the
        // output sequence of the specified 'streamBuf'.  If the specified
        // 'options' is 0, use 3 decimal places of fractional second precision,
        // otherwise use the number of decimal places specified by the
        // 'datetimeFractionalSecondPrecision' attribute of the 'options'.
        // Return 0 on success and a non-zero value otherwise.  The operation
        // succeeds if all bytes of the ISO 8601 representation of the 'value'
        // are written to the 'streamBuf' without the write position becoming
        // unavailable.

    static int putDatetimeTzValue(bsl::streambuf          *streamBuf,
                                  const bdlt::DatetimeTz&  value,
                                  const BerEncoderOptions *options);
        // Write the ISO 8601 representation of the specified 'value' to the
        // output sequence of the specified 'streamBuf'.  If the specified
        // 'options' is 0, use 3 decimal places of fractional second precision,
        // otherwise use the number of decimal places specified by the
        // 'datetimeFractionalSecondPrecision' attribute of the 'options'.
        // Return 0 on success and a non-zero value otherwise.  The operation
        // succeeds if all bytes of the ISO 8601 representation of the 'value'
        // are written to the 'streamBuf' without the write position becoming
        // unavailable.

    static int putTimeValue(bsl::streambuf          *streamBuf,
                            const bdlt::Time&        value,
                            const BerEncoderOptions *options);
        // Write the ISO 8601 representation of the specified 'value' to the
        // output sequence of the specified 'streamBuf'.  If the specified
        // 'options' is 0, use 3 decimal places of fractional second precision,
        // otherwise use the number of decimal places specified by the
        // 'datetimeFractionalSecondPrecision' attribute of the 'options'.
        // Return 0 on success and a non-zero value otherwise.  The operation
        // succeeds if all bytes of the ISO 8601 representation of the 'value'
        // are written to the 'streamBuf' without the write position becoming
        // unavailable.

    static int putTimeTzValue(bsl::streambuf          *streamBuf,
                              const bdlt::TimeTz&      value,
                              const BerEncoderOptions *options);
        // Write the ISO 8601 representation of the specified 'value' to the
        // output sequence of the specified 'streamBuf'.  If the specified
        // 'options' is 0, use 3 decimal places of fractional second precision,
        // otherwise use the number of decimal places specified by the
        // 'datetimeFractionalSecondPrecision' attribute of the 'options'.
        // Return 0 on success and a non-zero value otherwise.  The operation
        // succeeds if all bytes of the ISO 8601 representation of the 'value'
        // are written to the 'streamBuf' without the write position becoming
        // unavailable.
};

                    // ====================================
                    // struct BerUtil_TimezoneOffsetImpUtil
                    // ====================================

struct BerUtil_TimezoneOffsetImpUtil {
    // This component-private utility 'struct' provides a namespace for a suite
    // of functions and constants used by 'BerUtil' to encode and decode
    // time-zone values.

    // TYPES
    enum {
        k_MIN_OFFSET = -1439,
        // The minimum number of minutes in a valid time-zone offset

        k_MAX_OFFSET = 1439,
        // The maximum number of minutes in a valid time-zone offset

        k_TIMEZONE_LENGTH = 2
        // The number of octets used in the encoding of a time-zone offset
        // value.  This number is constant: all time-zone values are
        // encoded using 2 octets regardless of numeric value.
    };

    // CLASS METHODS
    static bool isValidTimezoneOffsetInMinutes(int value);
        // Return 'true' if the specified 'value' is a valid time-zone offset,
        // and return 'false' otherwise.  A time-zone offset is valid if it is
        // greater than or equal to 'k_MIN_OFFSET' and less than or equal to
        // 'k_MAX_OFFSET'.

    static int getTimezoneOffsetInMinutes(int            *value,
                                          bsl::streambuf *streamBuf);
        // Read from the specified 'streamBuf' and load to the specified
        // 'value' of the time-zone offset.

    static int getTimezoneOffsetInMinutesIfValid(int            *value,
                                                 bsl::streambuf *streamBuf);
        // Read a time zone offset value from the specified 'streamBuf'.  If
        // the offset is greater than or equal to 'k_MIN_OFFSET' and less than
        // or equal to 'k_MAX_OFFSET' then load the value of the offset to the
        // specified 'value' and return zero, otherwise do not modify the value
        // addressed by 'value' and return non-zero.

    static int putTimezoneOffsetInMinutes(bsl::streambuf *streamBuf,
                                          int             value);
        // Write to the specified 'streamBuf' the value of the specified
        // time-zone offset 'value'.  The behavior is undefined unless
        // 'k_MIN_OFFSET <= value' and 'value <= k_MAX_OFFSET'.
};

                     // ==================================
                     // struct BerUtil_DateAndTimeEncoding
                     // ==================================

struct BerUtil_DateAndTimeEncoding {
    // This component-private 'struct' provides a namespace for enumerating the
    // union of the sets of date and time formats used to encode and decode all
    // date and time types supported by 'BerUtil'.

    // TYPES
    enum Value {
        e_ISO8601_DATE,
        e_ISO8601_DATETZ,
        e_ISO8601_DATETIME,
        e_ISO8601_DATETIMETZ,
        e_ISO8601_TIME,
        e_ISO8601_TIMETZ,
        e_COMPACT_BINARY_DATE,
        e_COMPACT_BINARY_DATETZ,
        e_COMPACT_BINARY_DATETIME,
        e_COMPACT_BINARY_DATETIMETZ,
        e_COMPACT_BINARY_TIME,
        e_COMPACT_BINARY_TIMETZ,
        e_EXTENDED_BINARY_DATETIME,
        e_EXTENDED_BINARY_DATETIMETZ,
        e_EXTENDED_BINARY_TIME,
        e_EXTENDED_BINARY_TIMETZ
    };

    // TYPES
    enum {
        k_EXTENDED_BINARY_MIN_BDE_VERSION = 35500
        // the minimum BDE version number in which this component supports
        // encoding and decoding date and time types using their
        // respective extended-binary-encoding formats.
    };
};

                 // =========================================
                 // struct BerUtil_ExtendedBinaryEncodingUtil
                 // =========================================

struct BerUtil_ExtendedBinaryEncodingUtil {
    // This component-private utility 'struct' provides a namespace for a suite
    // of functions used by 'BerUtil' to determine if a particular date and/or
    // time value should be encoded using its corresponding
    // extended-binary-encoding format, its corresponding
    // compact-binary-encoding format, or neither format.

    // TYPES
    typedef BerUtil_DateAndTimeEncoding Encoding;
        // 'Encoding' is an alias to a namespace for enumerating the union of
        // the sets of date and time formats used to encode and decode all date
        // and time types supported by 'BerUtil'.

    // CLASS METHODS
    static bool useExtendedBinaryEncoding(const bdlt::Time&        value,
                                          const BerEncoderOptions *options);
    static bool useExtendedBinaryEncoding(const bdlt::TimeTz&      value,
                                          const BerEncoderOptions *options);
    static bool useExtendedBinaryEncoding(const bdlt::Datetime&    value,
                                          const BerEncoderOptions *options);
    static bool useExtendedBinaryEncoding(const bdlt::DatetimeTz&  value,
                                          const BerEncoderOptions *options);
        // Return 'true' if the specified 'value' must be encoded using its
        // corresponding extended-binary-encoding format according to the
        // specified 'options', and return 'false' otherwise.

    static bool useBinaryEncoding(const BerEncoderOptions *options);
        // Return 'true' if a date and/or time value must be encoded using
        // either its corresponding extended-binary-encoding format or its
        // corresponding compact-binary-encoding format according to the
        // specified 'options', and return 'false' otherwise.  Note that, for
        // any given 'value' and 'options', the 'value' must be encoded using
        // its corresponding compact-binary-encoding format if
        // 'useExtendedBinaryEncoding(value, options)' returns 'false' and
        // 'useBinaryEncoding(options)' returns 'true'.
};

                    // ====================================
                    // struct BerUtil_DateAndTimeHeaderType
                    // ====================================

struct BerUtil_DateAndTimeHeaderType {
    // This component-private 'struct' provides a namespace for enumerating the
    // set of "header type" values that may be encoded in the 2-byte header of
    // an extended-binary-encoding formatted date-and-time value.

    // TYPES
    enum Value {
        e_NOT_EXTENDED_BINARY,
        // header-type value that indicates the encoded value is in either
        // its corresponding compact-binary encoding or its corresponding
        // ISO 8601 encoding

        e_EXTENDED_BINARY_WITHOUT_TIMEZONE,
        // header-type value that indicates the encoded value is in its
        // corresponding extended-binary encoding and does not carry a
        // time-zone offset value

        e_EXTENDED_BINARY_WITH_TIMEZONE
        // header-type value that indicates the encoded value is in its
        // corresponding extended-binary encoding and carries a time-zone
        // offset value
    };
};

                      // ===============================
                      // class BerUtil_DateAndTimeHeader
                      // ===============================

class BerUtil_DateAndTimeHeader {
    // This component-private, in-core, value-semantic attribute class provides
    // a representation of the information available in the first two bytes of
    // any extended-binary-encoding formatted data.  All extended-binary
    // encoding schemes for date-and-time types contain a 2-byte header in the
    // same format, which can be unambiguously distinguished from the first 2
    // bytes of a date-and-time type in its corresponding
    // compact-binary-encoding format or its ISO 8601 format.

  public:
    // TYPES
    typedef BerUtil_DateAndTimeHeaderType Type;
        // 'Type' is an alias to a namespace for enumerating the set of "header
        // type" values that may be encoded in the 2-byte header of an
        // extended-binary-encoding formatted date-and-time value.

    typedef BerUtil_TimezoneOffsetImpUtil TimezoneUtil;
        // 'TimezoneUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoding and decoding operations for time-zone
        // offset values.

  private:
    // DATA
    Type::Value d_type;
        // date-and-time header type

    int         d_timezoneOffsetInMinutes;
        // offset in minutes from UTC indicated by the date-and-time header if
        // the header contains a time-zone offset, and 0 otherwise

  public:
    // CREATORS
    BerUtil_DateAndTimeHeader();
        // Create a 'BerUtil_DateAndTimeHeader' object having a 'type'
        // attribute with the 'Type::e_NOT_EXTENDED_BINARY' value and a
        // 'timezoneOffsetInMinutes' attribute with the 0 value.

    //! BerUtil_DateAndTimeHeader(
        //!               const BerUtil_DateAndTimeHeader& original) = default;
        // Create a 'BerUtil_DateAndTimeHeader' object having the same value as
        // the specified 'original' object.

    //! ~BerUtil_DateAndTimeHeader() = default;
        // Destroy this object.

    // MANIPULATORS
    //! BerUtil_DateAndTimeHeader&
        //!          operator=(const BerUtil_DateAndTimeHeader& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object.

    void makeNotExtendedBinary();
        // Set the 'type' attribute of this object to the
        // 'Type::e_NOT_EXTENDED_BINARY' value and the
        // 'timezoneOffsetInMinutes' attribute of this object to the 0 value.

    void makeExtendedBinaryWithoutTimezone();
        // Set the 'type' attribute of this object to the
        // 'Type::e_EXTENDED_BINARY_WITHOUT_TIMEZONE' value and the
        // 'timezoneOffsetInMinutes' attribute of this object to the 0 value.

    void makeExtendedBinaryWithTimezone(int offset);
        // Set the 'type' attribute of this object to the
        // 'Type::e_EXTENDED_BINARY_WITH_TIMEZONE' value and the
        // 'timezoneOffsetInMinutes' attribute of this object to the specified
        // 'offset'.  The behavior is undefined unless
        // 'TimezoneUtil::k_MIN_OFFSET <= offset' and
        // 'TimezoneUtil::k_MAX_OFFSET >= offset'.

    // ACCESSORS
    bool isExtendedBinary() const;
        // Return 'true' if the 'type' attribute of this object is
        // 'Type::e_EXTENDED_BINARY_WITH_TIMEZONE' or
        // 'Type::e_EXTENDED_BINARY_WITHOUT_TIMEZONE', and 'false' otherwise.

    bool isExtendedBinaryWithoutTimezone() const;
        // Return 'true' if the 'type' attribute of this object is
        // 'Type::e_EXTENDED_BINARY_WITHOUT_TIMEZONE', and 'false' otherwise.

    bool isExtendedBinaryWithTimezone() const;
        // Return 'true' if the 'type' attribute of this object is
        // 'Type::e_EXTENDED_BINARY_WITH_TIMEZONE', and 'false' otherwise.

    int timezoneOffsetInMinutes() const;
        // Return the value of the 'timezoneOffsetInMinutes' attribute of this
        // object.
};

                  // =======================================
                  // struct BerUtil_DateAndTimeHeaderImpUtil
                  // =======================================

struct BerUtil_DateAndTimeHeaderImpUtil {
    // This component-private utility 'struct' provides a namespace for a suite
    // of functions used by 'BerUtil' to implement encoding and decoding
    // operations for the 2-byte header of extended-binary-encoding formatted
    // date-and-time value.

    // TYPES
    typedef BerUtil_DateAndTimeHeader Header;
        // 'Header' is an alias to an in-core, value-semantic attribute class
        // that represents the range of valid values of the 2-byte header of
        // extended-binary-encoding formatted date-and-time values.

    typedef BerUtil_DateAndTimeHeaderType Type;
        // 'Type' is an alias to a namespace for enumerating the set of "header
        // type" values that may be encoded in the 2-byte header of an
        // extended-binary-encoding formatted date-and-time value.

    typedef BerUtil_StreambufUtil StreambufUtil;
        // 'StreambufUtil' is an alias to a namespace for a suite of functions
        // used to implement input and output operations on 'bsl::streambuf'
        // objects.

    typedef BerUtil_TimezoneOffsetImpUtil TimezoneUtil;
        // 'TimezoneUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoding and decoding operations for time-zone
        // offset values.

    // TYPES
    enum {
        k_HEADER_LENGTH = 2
        // number of octets used to encode an extended-binary-encoding
        // header
    };

    // CLASS METHODS
    static bool isReserved(unsigned char firstByte);
        // Return 'true' if the specified 'firstByte' of an encoded
        // date-and-time value indicates it is in a format reserved for future
        // use, and return 'false' otherwise.  Note that this may indicate the
        // value was encoded incorrectly or using a newer version of this
        // component.

    static bool isExtendedBinary(unsigned char firstByte);
        // Return 'true' if the specified 'firstByte' of an encoded
        // date-and-time value indicates it is in the extended-binary-encoding
        // format, and return 'false' otherwise.

    static bool isExtendedBinaryWithoutTimezone(unsigned char firstByte);
        // Return 'true' if the specified 'firstByte' of an encoded
        // date-and-time value indicates it is in the extended-binary-encoding
        // format and does not carry a time-zone offset value, and return
        // 'false' otherwise.

    static bool isExtendedBinaryWithTimezone(unsigned char firstByte);
        // Return 'true' if the specified 'firstByte' if an encoded
        // date-and-time value indicates is is in the extended-binary-encoding
        // format and carries a time-zone offset value, and return 'false'
        // otherwise.

    static void detectTypeIfNotReserved(bool          *reserved,
                                        Type::Value   *type,
                                        unsigned char  firstByte);
        // If the specified 'firstByte' of an encoded date-and-time value
        // indicates it is in a compact-binary-encoding format or an ISO 8601
        // format, load the value 'Type::e_NOT_EXTENDED_BINARY' to the
        // specified 'type' and 'false' to the specified 'reserved' flag.  If
        // it indicates it is in an extended-binary format that carries a
        // time-zone offset value, load the value
        // 'Type::e_EXTENDED_BINARY_WITH_TIMEZONE' to the 'type' and 'false' to
        // 'reserved'.  If it indicates it is in an extended-binary format that
        // does not carry a time-zone offset value, load the value
        // 'Type::e_EXTENDED_BINARY_WITHOUT_TIMEZONE' to the 'type' and 'false'
        // to 'reserved'.  Otherwise, load the value 'true' to 'reserved' and
        // leave the 'type' in a valid but unspecified state.  Note that this
        // operation has a wide contract because all possible values of
        // 'firstByte' can be interpreted to indicate one of the conditions
        // described above.

    static void detectType(Type::Value *type, unsigned char firstByte);
        // If the specified 'firstByte' of an encoded date-and-time value
        // indicates it is in a compact-binary-encoding format or an ISO 8601
        // format, load the value 'Type::e_NOT_EXTENDED_BINARY' to the
        // specified 'type'.  If it indicates it is in an extended-binary
        // format that carries a time-zone offset value, load the value
        // 'Type::e_EXTENDED_BINARY_WITH_TIMEZONE' to the 'type'.  If it
        // indicates it is in an extended-binary format that does not carry a
        // time-zone offset value, load the value
        // 'Type::e_EXTENDED_BINARY_WITHOUT_TIMEZONE' to the 'type'.  The
        // behavior is undefined unless 'isReserved(firstByte)' returns
        // 'false'.

    static int getValueIfNotReserved(Header *value, bsl::streambuf *streamBuf);
        // Read 2 bytes from the input sequence of the specified 'streamBuf'
        // and load to the specified 'value' the interpretation of those bytes
        // as an extended-binary header value, if that header indicates the
        // value is not in a format reserved for future use.  Return 0 on
        // success, and a non-zero value otherwise.

    static int getValueIfNotReserved(Header        *value,
                                     unsigned char  headerByte0,
                                     unsigned char  headerByte1);
        // Load to the specified 'value' the interpretation of the specified
        // 'headerByte0' and 'headerByte1' as the 2 bytes that comprise an
        // encoded extended-binary header value if that value indicates it is
        // not in a format reserved for future use.  Return 0 on success, and a
        // non-zero value otherwise.

    static int getValue(Header *value, bsl::streambuf *streamBuf);
        // Read 2 bytes from the input sequence of the specified 'streamBuf'
        // and load to the specified 'value' the interpretation of those bytes
        // as an extended-binary header value Return 0 on success, and a
        // non-zero value otherwise.  The behavior is undefined if 2 bytes are
        // successfully read from the 'streamBuf', but the interpretation of
        // those bytes as an extended-binary header indicates the value is in a
        // format reserved for future use.

    static int getValue(Header        *value,
                        unsigned char  headerByte0,
                        unsigned char  headerByte1);
        // Load to the specified 'value' the interpretation of the specified
        // 'headerByte0' and 'headerByte1' as the 2 bytes that comprise an
        // encoded extended-binary header value.  Return 0 on success, and a
        // non-zero value otherwise.  The behavior is undefined if the
        // interpretation of the 2 bytes as an extended-binary header indicates
        // the value is in a format reserved for future use.

    static int putExtendedBinaryWithoutTimezoneValue(
                                                    bsl::streambuf *streamBuf);
        // Write a representation of an extended-binary header value that does
        // not carry a time-zone offset value to the specified 'streamBuf'.
        // Return 0 on success, and a non-zero value otherwise.

    static int putExtendedBinaryWithTimezoneValue(
                                      bsl::streambuf *streamBuf,
                                      int             timezoneOffsetInMinutes);
        // Write a representation of an extended-binary header value that
        // carries the specified 'timezoneOffsetInMinutes' time-zone offset
        // value to the specified 'streamBuf'.  Return 0 on success, and a
        // non-zero value otherwise.
};

                        // ===========================
                        // struct BerUtil_DateEncoding
                        // ===========================

struct BerUtil_DateEncoding {
    // This component-private 'struct' provides a namespace for enumerating the
    // set of formats that may be used by 'BerUtil' to encode and decode values
    // of 'bdlt::Date' type.

    // TYPES
    typedef BerUtil_DateAndTimeEncoding Encoding;
        // 'Encoding' is an alias to a namespace for enumerating the union of
        // the sets of date and time formats used to encode and decode all date
        // and time types supported by 'BerUtil'.

    enum Value {
        e_ISO8601_DATE        = Encoding::e_ISO8601_DATE,
        e_COMPACT_BINARY_DATE = Encoding::e_COMPACT_BINARY_DATE
    };
};

                       // =============================
                       // struct BerUtil_DateTzEncoding
                       // =============================

struct BerUtil_DateTzEncoding {
    // This component-private 'struct' provides a namespace for enumerating the
    // set of formats that may be used by 'BerUtil' to encode and decode values
    // of 'bdlt::DateTz' type.

    // TYPES
    typedef BerUtil_DateAndTimeEncoding Encoding;
        // 'Encoding' is an alias to a namespace for enumerating the union of
        // the sets of date and time formats used to encode and decode all date
        // and time types supported by 'BerUtil'.

    enum Value {
        e_ISO8601_DATETZ        = Encoding::e_ISO8601_DATETZ,
        e_COMPACT_BINARY_DATE   = Encoding::e_COMPACT_BINARY_DATE,
        e_COMPACT_BINARY_DATETZ = Encoding::e_COMPACT_BINARY_DATETZ
    };
};

                    // ===================================
                    // struct BerUtil_DateOrDateTzEncoding
                    // ===================================

struct BerUtil_DateOrDateTzEncoding {
    // This component-private 'struct' provides a namespace for enumerating the
    // set of formats that may be used by 'BerUtil' to encode and decode values
    // of 'bdlb::Variant2<bdlt::Date, bdlt::DateTz>' type.

    // TYPES
    typedef BerUtil_DateAndTimeEncoding Encoding;
        // 'Encoding' is an alias to a namespace for enumerating the union of
        // the sets of date and time formats used to encode and decode all date
        // and time types supported by 'BerUtil'.

    enum Value {
        e_ISO8601_DATE          = Encoding::e_ISO8601_DATE,
        e_ISO8601_DATETZ        = Encoding::e_ISO8601_DATETZ,
        e_COMPACT_BINARY_DATE   = Encoding::e_COMPACT_BINARY_DATE,
        e_COMPACT_BINARY_DATETZ = Encoding::e_COMPACT_BINARY_DATETZ
    };
};

                         // ==========================
                         // struct BerUtil_DateImpUtil
                         // ==========================

struct BerUtil_DateImpUtil {
    // This component-private 'struct' provides a namespace for a suite of
    // functions used by 'BerUtil' to implement BER encoding and decoding
    // operations for date values.  Within the definition of this 'struct':
    //
    //: *the* *specification*:
    //:   Refers to the August 2015 revision of the ITU-T Recommendation X.690,
    //:   and
    //:
    //: *the* *default* *set* *of* *options*:
    //:   Refers to a 'balber::BerEncoderOptions' value having a
    //:   'datetimeFractionalSecondPrecision' attribute of 3 and a
    //:   'encodeDateAndTimeTypesAsBinary' attribute of 'false'.
    //
    // See the package level documentation of {'balber'} for a definition of
    // the compact and extended binary formats for date and time values.

    // TYPES
    typedef BerUtil_DateAndTimeHeaderImpUtil DateAndTimeHeaderUtil;
        // 'DateAndTimeHeaderUtil' is an alias to a namespace for a suite of
        // functions used to implement encoding and decoding operations for the
        // 2-byte header of an extended-binary-encoding formatted date-and-time
        // value.

    typedef BerUtil_DateEncoding DateEncoding;
        // 'DateEncoding' is an alias to a namespace for enumerating the set of
        // formats that may be used by 'BerUtil' to encode and decode values of
        // 'bdlt::Date' type.

    typedef BerUtil_DateTzEncoding DateTzEncoding;
        // 'DateEncoding' is an alias to a namespace for enumerating the set of
        // formats that may be used by 'BerUtil' to encode and decode values of
        // 'bdlt::DateTz' type.

    typedef BerUtil_DateOrDateTzEncoding DateOrDateTzEncoding;
        // 'DateEncoding' is an alias to a namespace for enumerating the set of
        // formats that may be used by 'BerUtil' to decode to values of
        // 'bdlb::Variant2<bdlt::Date, bdlt::DateTz>' type.

    typedef BerUtil_IntegerImpUtil IntegerUtil;
        // 'IntegerUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoding and decoding operations for integer
        // values.

    typedef BerUtil_Iso8601ImpUtil Iso8601Util;
        // 'Iso8601Util' is an alias to a namespace for a suite of functions
        // used to implementing the encoding and decoding of date and time
        // values using the ISO 8601 format.

    typedef BerUtil_LengthImpUtil LengthUtil;
        // 'LengthUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoding and decoding operations for length
        // quantities.

    typedef BerUtil_StreambufUtil StreambufUtil;
        // 'StreambufUtil' is an alias to a namespace for a suite of functions
        // used to implement input and output operations on 'bsl::streambuf'
        // objects.

    typedef BerUtil_StringImpUtil StringUtil;
        // 'StringUtil' is an alias to a namespace for a suite of functions
        // used by 'BerUtil' to implement BER encoding and decoding operations
        // for string values.

    typedef BerUtil_TimezoneOffsetImpUtil TimezoneUtil;
        // 'TimezoneUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoding and decoding operations for time-zone
        // offset values.

    typedef bdlb::Variant2<bdlt::Date, bdlt::DateTz> DateOrDateTz;
        // 'DateOrDateTz' is a convenient alias for
        // 'bdlb::Variant2<bdlt::Date, bdlt::DateTz>'.

    enum {
        k_COMPACT_BINARY_DATE_EPOCH = 737425
        // The serial date of January 1st, 2020.  Note that the serial date
        // of a date is defined as the number of days between that date and
        // January 1st year 1 in the Proleptic Gregorian calendar.
    };

  private:
    // PRIVATE TYPES
    enum {
        k_MAX_ISO8601_DATE_LENGTH = bdlt::Iso8601Util::k_DATE_STRLEN,
        // the maximum number of content octets used by 'BerUtil' to encode
        // a date value using the ISO 8601 format

        k_MAX_ISO8601_DATETZ_LENGTH = bdlt::Iso8601Util::k_DATETZ_STRLEN,
        // the maximum number of content octets used by 'BerUtil' to
        // encode a date and time zone value using the ISO 8601 format

        k_MAX_COMPACT_BINARY_DATE_LENGTH = 3,
        // the maximum number of content octets used by 'BerUtil' to
        // encode a date value using the compact-binary format

        k_MIN_COMPACT_BINARY_DATETZ_LENGTH =
            k_MAX_COMPACT_BINARY_DATE_LENGTH + 1,
        // the minimum number of content octets used by 'BerUtil' to
        // encode a date and time zone value using the compact-binary
        // format

        k_MAX_COMPACT_BINARY_DATETZ_LENGTH = 5
        // the maximum number of content octets used by 'BerUtil' to
        // encode a date and time zone value using the compact-binary
        // format
    };

    // PRIVATE CLASS METHODS

    // 'bdlt::Date' Decoding

    static int detectDateEncoding(DateEncoding::Value *encoding,
                                  int                  length,
                                  unsigned char        firstByte);
        // Load to the specified 'encoding' the enumerator that describes the
        // format used to encode a 'bdlt::Date' value given the specified
        // 'length' and 'firstByte' of the encoded representation.  Return 0 on
        // success, -1 if the format is reserved for future use, and some other
        // non-zero value otherwise.

    static int getIso8601DateValue(bdlt::Date     *value,
                                   bsl::streambuf *streamBuf,
                                   int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date value represented by the interpretation of the read bytes as an
        // ISO 8601 date.  Return 0 on success, and a non-zero value otherwise.
        // The operation succeeds if 'length' bytes are successfully read from
        // the input sequence of the 'streamBuf' without the read position
        // becoming unavailable, and the bytes contain a valid representation
        // of an ISO 8601 date.

    static int getCompactBinaryDateValue(bdlt::Date     *value,
                                         bsl::streambuf *streamBuf,
                                         int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date value represented by the interpretation of the read bytes as a
        // compact-binary date.  Return 0 on success, and a non-zero value
        // otherwise.  The operation succeeds if 'length' bytes are
        // successfully read from the input sequence of the 'streamBuf' without
        // the read position becoming unavailable, and the bytes contain a
        // valid representation of a compact-binary date.

    // 'bdlt::Date' Encoding

    static DateEncoding::Value selectDateEncoding(
                                             const bdlt::Date&        value,
                                             const BerEncoderOptions *options);
        // Determine the format that should be used to encode the specified
        // 'value' given the 'value' and the specified 'options'.  If 'options'
        // is 0, the default set of options is used.  Return an enumerator
        // identifying the selected format.

    static int putIso8601DateValue(bsl::streambuf          *streamBuf,
                                   const bdlt::Date&        value,
                                   const BerEncoderOptions *options);
        // Write the ISO 8601 representation of the specified 'value' to the
        // output sequence of the specified 'streamBuf' according to the
        // specified 'options'.  If 'options' is 0, the default set of options
        // is used.  Return 0 on success, and a non-zero value otherwise.  The
        // operation succeeds if all bytes of the ISO 8601 representation of
        // the 'value' are written to the 'streamBuf' without the write
        // position becoming unavailable.

    static int putCompactBinaryDateValue(bsl::streambuf          *streamBuf,
                                         const bdlt::Date&        value,
                                         const BerEncoderOptions *options);
        // Write the compact-binary date representation of the specified
        // 'value' to the output sequence of the specified 'streamBuf'
        // according to the specified 'options'.  If 'options' is 0, the
        // default set of options is used.  Return 0 on success, and a non-zero
        // value otherwise.  The operation succeeds if all bytes of the
        // compact-binary date representation of the 'value' are written to the
        // 'streamBuf' without the write position becoming unavailable.

    // 'bdlt::DateTz' Decoding

    static int detectDateTzEncoding(DateTzEncoding::Value *encoding,
                                    int                    length,
                                    unsigned char          firstByte);
        // Load to the specified 'encoding' the enumerator that describes the
        // format used to encode a 'bdlt::DateTz' value given the specified
        // 'length' and 'firstByte' of the encoded representation.  Return 0 on
        // success, -1 if the format is reserved for future use, and some other
        // non-zero value otherwise.

    static int getIso8601DateTzValue(bdlt::DateTz   *value,
                                     bsl::streambuf *streamBuf,
                                     int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date and time zone value represented by the interpretation of the
        // read bytes as an ISO 8601 date and time zone.  Return 0 on success,
        // and a non-zero value otherwise.  The operation succeeds if 'length'
        // bytes are successfully read from the input sequence of the
        // 'streamBuf' without the read position becoming unavailable, and the
        // bytes contain a valid representation of an ISO 8601 date and time
        // zone.

    static int getCompactBinaryDateValue(bdlt::DateTz   *value,
                                         bsl::streambuf *streamBuf,
                                         int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date and time zone value represented by the interpretation of the
        // read bytes as a compact-binary date.  Return 0 on success, and a
        // non-zero value otherwise.  The operation succeeds if 'length' bytes
        // are successfully read from the input sequence of the 'streamBuf'
        // without the read position becoming unavailable, and the bytes
        // contain a valid representation of a compact-binary date.

    static int getCompactBinaryDateTzValue(bdlt::DateTz   *value,
                                           bsl::streambuf *streamBuf,
                                           int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date and time zone value represented by the interpretation of the
        // read bytes as a compact-binary date and time zone.  Return 0 on
        // success, and a non-zero value otherwise.  The operation succeeds if
        // 'length' bytes are successfully read from the input sequence of the
        // 'streamBuf' without the read position becoming unavailable, and the
        // bytes contain a valid representation of a compact-binary date.

    // 'bdlt::DateTz' Encoding

    static DateTzEncoding::Value selectDateTzEncoding(
                                             const bdlt::DateTz&      value,
                                             const BerEncoderOptions *options);
        // Determine the format that should be used to encode the specified
        // 'value' given the 'value' and the specified 'options'.  If 'options'
        // is 0, the default set of options is used.  Return an enumerator
        // identifying the selected format.

    static int putIso8601DateTzValue(bsl::streambuf          *streamBuf,
                                     const bdlt::DateTz&      value,
                                     const BerEncoderOptions *options);
        // Write the ISO 8601 representation of the specified 'value' to the
        // output sequence of the specified 'streamBuf' according to the
        // specified 'options'.  If 'options' is 0, the default set of options
        // is used.  Return 0 on success, and a non-zero value otherwise.  The
        // operation succeeds if all bytes of the ISO 8601 representation of
        // the 'value' are written to the 'streamBuf' without the write
        // position becoming unavailable.

    static int putCompactBinaryDateValue(bsl::streambuf          *streamBuf,
                                         const bdlt::DateTz&      value,
                                         const BerEncoderOptions *options);
        // Write the compact-binary date representation of the specified
        // 'value' to the output sequence of the specified 'streamBuf'
        // according to the specified 'options'.  If 'options' is 0, the
        // default set of options is used.  Return 0 on success, and a non-zero
        // value otherwise.  The operation succeeds if all bytes of the
        // compact-binary date representation of the 'value' are written to the
        // 'streamBuf' without the write position becoming unavailable.  The
        // behavior is undefined unless the 'offset' of the 'value' is 0.

    static int putCompactBinaryDateTzValue(bsl::streambuf          *streamBuf,
                                           const bdlt::DateTz&      value,
                                           const BerEncoderOptions *options);
        // Write the compact-binary date and time zone representation of the
        // specified 'value' to the output sequence of the specified
        // 'streamBuf' according to the specified 'options'.  If 'options' is
        // 0, the default set of options is used.  Return 0 on success, and a
        // non-zero value otherwise.  The operation succeeds if all bytes of
        // the compact-binary date representation of the 'value' are written to
        // the 'streamBuf' without the write position becoming unavailable.

    // Variant Decoding

    static int detectDateOrDateTzEncoding(
                                 DateOrDateTzEncoding::Value *encoding,
                                 int                          length,
                                 unsigned char                firstByte);
        // Load to the specified 'encoding' the enumerator that describes the
        // format used to encode a 'bdlt::Date' or 'bdlt::DateTz' value given
        // the specified 'length' and 'firstByte' of the encoded
        // representation.  Return 0 on success, -1 if the format is reserved
        // for future use, and some other non-zero value otherwise.

    static int getIso8601DateValue(DateOrDateTz   *value,
                                   bsl::streambuf *streamBuf,
                                   int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date value represented by the interpretation of the read bytes as an
        // ISO 8601 date.  Return 0 on success, and a non-zero value otherwise.
        // The operation succeeds if 'length' bytes are successfully read from
        // the input sequence of the 'streamBuf' without the read position
        // becoming unavailable, and the bytes contain a valid representation
        // of an ISO 8601 date.

    static int getIso8601DateTzValue(DateOrDateTz   *value,
                                     bsl::streambuf *streamBuf,
                                     int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date value represented by the interpretation of the read bytes as an
        // ISO 8601 date and time zone.  Return 0 on success, and a non-zero
        // value otherwise.  The operation succeeds if 'length' bytes are
        // successfully read from the input sequence of the 'streamBuf' without
        // the read position becoming unavailable, and the bytes contain a
        // valid representation of an ISO 8601 date and time zone.

    static int getCompactBinaryDateValue(DateOrDateTz   *value,
                                         bsl::streambuf *streamBuf,
                                         int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date value represented by the interpretation of the read bytes as a
        // compact-binary date.  Return 0 on success, and a non-zero value
        // otherwise.  The operation succeeds if 'length' bytes are
        // successfully read from the input sequence of the 'streamBuf' without
        // the read position becoming unavailable, and the bytes contain a
        // valid representation of a compact-binary date.

    static int getCompactBinaryDateTzValue(DateOrDateTz   *value,
                                           bsl::streambuf *streamBuf,
                                           int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date and time zone value represented by the interpretation of the
        // read bytes as a compact-binary date and time zone.  Return 0 on
        // success, and a non-zero value otherwise.  The operation succeeds if
        // 'length' bytes are successfully read from the input sequence of the
        // 'streamBuf' without the read position becoming unavailable, and the
        // bytes contain a valid representation of a compact-binary date and
        // time zone.

  public:
    // CLASS METHODS

    // Utilities

    static void dateToDaysSinceEpoch(bsls::Types::Int64 *daysSinceEpoch,
                                     const bdlt::Date&   date);
        // Load to the specified 'daysSinceEpoch' the number of days between
        // the compact-binary date epoch and the specified 'date'. The
        // compact-binary date epoch is the date defined by the
        // 'k_COMPACT_BINARY_DATE_EPOCH' serial date.  Note that this quantity
        // may be negative if the specified 'date' occurs before the
        // compact-binary date epoch.

    static int daysSinceEpochToDate(bdlt::Date         *date,
                                    bsls::Types::Int64  daysSinceEpoch);
        // Load to the specified 'date' the date represented by the serial date
        // indicated by adding the specified 'daysSinceEpoch' to
        // 'k_COMPACT_BINARY_DATE_EPOCH'.  Return 0 on success, and a non-zero
        // value otherwise.  This operation succeeds if the resulting value
        // represents a date in the range '[0001JAN01 .. 9999DEC31]'.  Note
        // that 'daysSinceEpoch' may be negative to indicate a serial date that
        // occurs before 'k_COMPACT_BINARY_DATE_EPOCH'.

    // 'bdlt::Date' Decoding

    static int getDateValue(bdlt::Date     *date,
                            bsl::streambuf *streamBuf,
                            int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date value represented those bytes.  Return 0 on success, and a
        // non-zero value otherwise.  The operation succeeds if 'length' bytes
        // are successfully read from the input sequence of the 'streamBuf'
        // without the read position becoming unavailable, and the bytes
        // contain a valid representation of a date value.  See the
        // package-level documentation of {'balber'} for a description of the
        // decision procedure used to detect the encoding format for a
        // 'bdlt::Date' value.

    // 'bdlt::Date' Encoding

    static int putDateValue(bsl::streambuf          *streamBuf,
                            const bdlt::Date&        value,
                            const BerEncoderOptions *options);
        // Write a representation of the specified 'value' date to the output
        // sequence of the specified 'streamBuf' according to the specified
        // 'options'.  If 'options' is 0, the default set of options is used.
        // Return 0 on success, and a non-zero value otherwise.  This operation
        // succeeds if all bytes in the representation of the 'value' are
        // written to the output sequence of the 'streamBuf' without the write
        // position becoming unavailable.  See the class documentation for a
        // description of the default options.  See the package-level
        // documentation of {'balber'} for a description of the decision
        // procedure used to select an encoding format for the 'value'.

    // 'bdlt::DateTz' Decoding

    static int getDateTzValue(bdlt::DateTz   *value,
                              bsl::streambuf *streamBuf,
                              int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date and time zone value represented by those bytes.  Return 0 on
        // success, and a non-zero value otherwise.  The operation succeeds if
        // 'length' bytes are successfully read from the input sequence of the
        // 'streamBuf' without the read position becoming unavailable, and the
        // bytes contain a valid representation of a date and time zone value.
        // See the package-level documentation of {'balber'} for a description
        // of the decision procedure used to detect the encoding format for a
        // 'bdlt::DateTz' value.

    // 'bdlt::DateTz' Encoding

    static int putDateTzValue(bsl::streambuf          *streamBuf,
                              const bdlt::DateTz&      date,
                              const BerEncoderOptions *options);
        // Write a representation of the specified 'value' date and time zone
        // to the output sequence of the specified 'streamBuf' according to the
        // specified 'options'.  If 'options' is 0, the default set of options
        // is used.  Return 0 on success, and a non-zero value otherwise.  This
        // operation succeeds if all bytes in the representation of the 'value'
        // are written to the output sequence of the 'streamBuf' without the
        // write position becoming unavailable.  See the class documentation
        // for a description of the default options.  See the package-level
        // documentation of {'balber'} for a description of the decision
        // procedure used to select an encoding format for the 'value'.

    // Variant Decoding

    static int getDateOrDateTzValue(DateOrDateTz   *value,
                                    bsl::streambuf *streamBuf,
                                    int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date and optional time zone value represented by those bytes.
        // Return 0 on success, and a non-zero value otherwise.  The operation
        // succeeds if 'length' bytes are successfully read from the input
        // sequence of the 'streamBuf' without the read position becoming
        // unavailable, and the bytes contain a valid representation of a date
        // and optional time zone value.  See the package-level documentation
        // of {'balber'} for a description of the decision procedure used to
        // detect the encoding format for a 'DateOrDateTz' value.
};

                        // ===========================
                        // struct BerUtil_TimeEncoding
                        // ===========================

struct BerUtil_TimeEncoding {
    // This component-private utility 'struct' provides a namespace for
    // enumerating the set of formats that may be used by 'BerUtil' to encode
    // and decode values of 'bdlt::Time' type.

    // TYPES
    typedef BerUtil_DateAndTimeEncoding Encoding;
        // 'Encoding' is an alias to a namespace for enumerating the union of
        // the sets of date and time formats used to encode and decode all date
        // and time types supported by 'BerUtil'.

    enum Value {
        e_ISO8601_TIME         = Encoding::e_ISO8601_TIME,
        e_COMPACT_BINARY_TIME  = Encoding::e_COMPACT_BINARY_TIME,
        e_EXTENDED_BINARY_TIME = Encoding::e_EXTENDED_BINARY_TIME
    };

    enum {
        k_EXTENDED_BINARY_MIN_BDE_VERSION =
            Encoding::k_EXTENDED_BINARY_MIN_BDE_VERSION
    };
};

                       // =============================
                       // struct BerUtil_TimeTzEncoding
                       // =============================

struct BerUtil_TimeTzEncoding {
    // This component-private utility 'struct' provides a namespace for
    // enumerating the set of formats that may be used by 'BerUtil' to encode
    // and decode values of 'bdlt::TimeTz' type.

    // TYPES
    typedef BerUtil_DateAndTimeEncoding Encoding;
        // 'Encoding' is an alias to a namespace for enumerating the union of
        // the sets of date and time formats used to encode and decode all date
        // and time types supported by 'BerUtil'.

    enum Value {
        e_ISO8601_TIMETZ         = Encoding::e_ISO8601_TIMETZ,
        e_COMPACT_BINARY_TIME    = Encoding::e_COMPACT_BINARY_TIME,
        e_COMPACT_BINARY_TIMETZ  = Encoding::e_COMPACT_BINARY_TIMETZ,
        e_EXTENDED_BINARY_TIMETZ = Encoding::e_EXTENDED_BINARY_TIMETZ
    };

    enum {
        k_EXTENDED_BINARY_MIN_BDE_VERSION =
            Encoding::k_EXTENDED_BINARY_MIN_BDE_VERSION
    };
};

                    // ===================================
                    // struct BerUtil_TimeOrTimeTzEncoding
                    // ===================================

struct BerUtil_TimeOrTimeTzEncoding {
    // This component-private utility 'struct' provides a namespace for
    // enumerating the set of formats that may be used by 'BerUtil' to decode
    // values of 'bdlb::Variant2<bdlt::Time, bdlt::TimeTz>' type.

    // TYPES
    typedef BerUtil_DateAndTimeEncoding Encoding;
        // 'Encoding' is an alias to a namespace for enumerating the union of
        // the sets of date and time formats used to encode and decode all date
        // and time types supported by 'BerUtil'.

    enum Value {
        e_ISO8601_TIME           = Encoding::e_ISO8601_TIME,
        e_ISO8601_TIMETZ         = Encoding::e_ISO8601_TIMETZ,
        e_COMPACT_BINARY_TIME    = Encoding::e_COMPACT_BINARY_TIME,
        e_COMPACT_BINARY_TIMETZ  = Encoding::e_COMPACT_BINARY_TIMETZ,
        e_EXTENDED_BINARY_TIME   = Encoding::e_EXTENDED_BINARY_TIME,
        e_EXTENDED_BINARY_TIMETZ = Encoding::e_EXTENDED_BINARY_TIMETZ
    };
};

                         // ==========================
                         // struct BerUtil_TimeImpUtil
                         // ==========================

struct BerUtil_TimeImpUtil {
    // This component-private 'struct' provides a namespace for a suite of
    // functions used by 'BerUtil' to implement BER encoding and decoding
    // operations for time values.  Within the definition of this 'struct':
    //
    //: *the* *specification*:
    //:   Refers to the August 2015 revision of the ITU-T Recommendation X.690,
    //:   and
    //:
    //: *the* *default* *set* *of* *options*:
    //:   Refers to a 'balber::BerEncoderOptions' value having a
    //:   'datetimeFractionalSecondPrecision' attribute of 3 and a
    //:   'encodeDateAndTimeTypesAsBinary' attribute of 'false'.
    //
    // See the package level documentation of {'balber'} for a definition of
    // the compact and extended binary formats for date and time values.

    // TYPES
    typedef BerUtil_ExtendedBinaryEncodingUtil ExtendedBinaryEncodingUtil;
        // 'DateAndTimeHeaderUtil' is an alias to a namespace for a suite of
        // functions used to implement encoding and decoding operations for the
        // 2-byte header of an extended-binary-encoding formatted date-and-time
        // value.

    typedef BerUtil_DateAndTimeHeader DateAndTimeHeader;
        // 'Header' is an alias to an in-core, value-semantic attribute class
        // that represents the range of valid values of the 2-byte header of
        // extended-binary-encoding formatted date-and-time values.

    typedef BerUtil_DateAndTimeHeaderType DateAndTimeHeaderType;
        // 'Type' is an alias to a namespace for enumerating the set of "header
        // type" values that may be encoded in the 2-byte header of an
        // extended-binary-encoding formatted date-and-time value.

    typedef BerUtil_DateAndTimeHeaderImpUtil DateAndTimeHeaderUtil;
        // 'DateAndTimeHeaderUtil' is an alias to a namespace for a suite of
        // functions used to implement encoding and decoding operations for the
        // 2-byte header of an extended-binary-encoding formatted date-and-time
        // value.

    typedef BerUtil_IntegerImpUtil IntegerUtil;
        // 'IntegerUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoding and decoding operations for integer
        // values.

    typedef BerUtil_Iso8601ImpUtil Iso8601Util;
        // 'Iso8601Util' is an alias to a namespace for a suite of functions
        // used to implementing the encoding and decoding of date and time
        // values using the ISO 8601 format.

    typedef BerUtil_LengthImpUtil LengthUtil;
        // 'LengthUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoding and decoding operations for length
        // quantities.

    typedef BerUtil_StringImpUtil StringUtil;
        // 'StringUtil' is an alias to a namespace for a suite of functions
        // used by 'BerUtil' to implement BER encoding and decoding operations
        // for string values.

    typedef BerUtil_StreambufUtil StreambufUtil;
        // 'StreambufUtil' is an alias to a namespace for a suite of functions
        // used to implement input and output operations on 'bsl::streambuf'
        // objects.

    typedef BerUtil_TimeEncoding TimeEncoding;
        // 'TimeEncoding' is an alias to a namespace for enumerating the set of
        // formats that may be used by 'BerUtil' to encode and decode values of
        // 'bdlt::Time' type.

    typedef BerUtil_TimeTzEncoding TimeTzEncoding;
        // 'TimeTzEncoding' is an alias to a namespace for enumerating the set
        // of formats that may be used by 'BerUtil' to encode and decode values
        // of 'bdlt::TimeTz' type.

    typedef BerUtil_TimeOrTimeTzEncoding TimeOrTimeTzEncoding;
        // 'TimeOrTimeTzEncoding' is an alias to a namespace for enumerating
        // the set of formats that may be used by 'BerUtil' to decode to values
        // of 'bdlb::Variant2<bdlt::Time, bdlt::TimeTz>' type.

    typedef BerUtil_TimezoneOffsetImpUtil TimezoneUtil;
        // 'TimezoneUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoding and decoding operations for time-zone
        // offset values.

    typedef bdlb::Variant2<bdlt::Time, bdlt::TimeTz> TimeOrTimeTz;
        // 'TimeOrTimeTz' is a convenient alias for
        // 'bdlb::Variant2<bdlt::Time, bdlt::TimeTz>'.

  private:
    // PRIVATE TYPES
    enum {
        k_EXTENDED_BINARY_TIME_LENGTH =
            DateAndTimeHeaderUtil::k_HEADER_LENGTH +
            IntegerUtil::k_40_BIT_INTEGER_LENGTH,  // = 7
        // the number of content octets used by 'BerUtil' to encode
        // a time value using the extended-binary time and time zone format

        k_EXTENDED_BINARY_TIMETZ_LENGTH =
            DateAndTimeHeaderUtil::k_HEADER_LENGTH +
            IntegerUtil::k_40_BIT_INTEGER_LENGTH,  // = 7
        // the number of contents octets used by 'BerUtil' to encode
        // a time and time zone value using the extended-binary time and
        // time zone format

        k_MAX_ISO8601_TIME_LENGTH = bdlt::Iso8601Util::k_TIME_STRLEN,
        // the maximum number of content octets used by 'BerUtil' to encode
        // a time value using the ISO 8601 format

        k_MAX_ISO8601_TIMETZ_LENGTH = bdlt::Iso8601Util::k_TIMETZ_STRLEN,
        // the maximum number of content octets used by 'BerUtil to encode
        // a time and time zone value using the ISO 8601 format

        k_MIN_COMPACT_BINARY_TIME_LENGTH = 1,
        // the minimum number of content octets used by 'BerUtil' to encode
        // a time value using the compact-binary time format

        k_MAX_COMPACT_BINARY_TIME_LENGTH = 4,
        // the maximum number of content octets used by 'BerUtil' to encode
        // a time value using the compact-binary time format

        k_MIN_COMPACT_BINARY_TIMETZ_LENGTH =
            k_MAX_COMPACT_BINARY_TIME_LENGTH + 1,
        // the minimum number of content octets used by 'BerUtil' to encode
        // a time and time zone value using the compact-binary time and
        // time zone format

        k_MAX_COMPACT_BINARY_TIMETZ_LENGTH = 6
        // the maximum number of content octets used by 'BerUtil' to encode
        // a time and time zone value using the compact-binary time and
        // time zone format
    };

    // PRIVATE CLASS METHODS

    // 'bdlt::Time' Decoding

    static int detectTimeEncoding(TimeEncoding::Value *encoding,
                                  int                  length,
                                  unsigned char        firstByte);
        // Load to the specified 'encoding' the enumerator that describes the
        // format used to encode a 'bdlt::Time' value given the specified
        // 'length' and 'firstByte' of the encoded representation.  Return 0 on
        // success, -1 if the format is reserved for future use, and some other
        // non-zero value otherwise.

    static int getIso8601TimeValue(bdlt::Time     *value,
                                   bsl::streambuf *streamBuf,
                                   int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // time value represented by the interpretation of the read bytes as an
        // ISO 8601 time.  Return 0 on success, and a non-zero value otherwise.
        // The operation succeeds if 'length' bytes are successfully read from
        // the input sequence of the 'streamBuf' without the read position
        // becoming unavailable, and the bytes contain a valid representation
        // of an ISO 8601 time.

    static int getCompactBinaryTimeValue(bdlt::Time     *value,
                                         bsl::streambuf *streamBuf,
                                         int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // time value represented by the interpretation of the read bytes as a
        // compact-binary time.  Return 0 on success, and a non-zero value
        // otherwise.  The operation succeeds if 'length' bytes are
        // successfully read from the input sequence of the 'streamBuf' without
        // the read position becoming unavailable, and the bytes contain a
        // valid representation of a compact-binary time.

    static int getExtendedBinaryTimeValue(bdlt::Time     *value,
                                          bsl::streambuf *streamBuf,
                                          int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // time value represented by the interpretation of the read bytes as an
        // extended-binary time.  Return 0 on success, and a non-zero value
        // otherwise.  The operation succeeds if 'length' bytes are
        // successfully read from the input sequence of the 'streamBuf' without
        // the read position becoming unavailable, and the bytes contain a
        // valid representation of a extended-binary time.

    // 'bdlt::Time' Encoding

    static TimeEncoding::Value selectTimeEncoding(
                                             const bdlt::Time&        value,
                                             const BerEncoderOptions *options);
        // Determine the format that should be used to encode the specified
        // 'value' given the 'value' and the specified 'options'.  If 'options'
        // is 0, the default set of options is used.  Return an enumerator
        // identifying the selected format.

    static int putIso8601TimeValue(bsl::streambuf          *streamBuf,
                                   const bdlt::Time&        value,
                                   const BerEncoderOptions *options);
        // Write the ISO 8601 representation of the specified 'value' to the
        // output sequence of the specified 'streamBuf' according to the
        // specified 'options'.  If 'options' is 0, the default set of options
        // is used.  Return 0 on success, and a non-zero value otherwise.  The
        // operation succeeds if all bytes of the ISO 8601 representation of
        // the 'value' are written to the 'streamBuf' without the write
        // position becoming unavailable.

    static int putCompactBinaryTimeValue(bsl::streambuf          *streamBuf,
                                         const bdlt::Time&        value,
                                         const BerEncoderOptions *options);
        // Write the compact-binary time representation of the specified
        // 'value' to the output sequence of the specified 'streamBuf'
        // according to the specified 'options'.  If 'options' is 0, the
        // default set of options is used.  Return 0 on success, and a non-zero
        // value otherwise.  The operation succeeds if all bytes of the
        // compact-binary time representation of the 'value' are written to the
        // 'streamBuf' without the write position becoming unavailable.

    static int putExtendedBinaryTimeValue(bsl::streambuf          *streamBuf,
                                          const bdlt::Time&        value,
                                          const BerEncoderOptions *options);
        // Write the extended-binary time representation of the specified
        // 'value' to the output sequence of the specified 'streamBuf'
        // according to the specified 'options'.  If 'options' is 0, the
        // default set of options is used.  Return 0 on success, and a non-zero
        // value otherwise.  The operation succeeds if all bytes of the
        // extended-binary time representation of the 'value' are written to
        // the 'streamBuf' without the write position becoming unavailable.

    // 'bdlt::TimeTz' Decoding

    static int detectTimeTzEncoding(TimeTzEncoding::Value *encoding,
                                    int                    length,
                                    unsigned char          firstByte);
        // Load to the specified 'encoding' the enumerator that describes the
        // format used to encode a 'bdlt::Time' value given the specified
        // 'length' and 'firstByte' of the encoded representation.  Return 0 on
        // success, -1 if the format is reserved for future use, and some other
        // non-zero value otherwise.

    static int getIso8601TimeTzValue(bdlt::TimeTz   *value,
                                     bsl::streambuf *streamBuf,
                                     int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // time and time zone value represented by the interpretation of the
        // read bytes as an ISO 8601 time and time zone.  Return 0 on success,
        // and a non-zero value otherwise.  The operation succeeds if 'length'
        // bytes are successfully read from the input sequence of the
        // 'streamBuf' without the read position becoming unavailable, and the
        // bytes contain a valid representation of an ISO 8601 time and time
        // zone.

    static int getCompactBinaryTimeValue(bdlt::TimeTz   *value,
                                         bsl::streambuf *streamBuf,
                                         int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // time value represented by the interpretation of the read bytes as a
        // compact-binary time.  Return 0 on success, and a non-zero value
        // otherwise.  The operation succeeds if 'length' bytes are
        // successfully read from the input sequence of the 'streamBuf' without
        // the read position becoming unavailable, and the bytes contain a
        // valid representation of a compact-binary time.

    static int getCompactBinaryTimeTzValue(bdlt::TimeTz   *value,
                                           bsl::streambuf *streamBuf,
                                           int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // time and time zone value represented by the interpretation of the
        // read bytes as a compact-binary time and time zone.  Return 0 on
        // success, and a non-zero value otherwise.  The operation succeeds if
        // 'length' bytes are successfully read from the input sequence of the
        // 'streamBuf' without the read position becoming unavailable, and the
        // bytes contain a valid representation of a compact-binary time and
        // time zone.

    static int getExtendedBinaryTimeTzValue(bdlt::TimeTz   *value,
                                            bsl::streambuf *streamBuf,
                                            int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // time and time zone value represented by the interpretation of the
        // read bytes as an extended-binary time and time zone.  Return 0 on
        // success, and a non-zero value otherwise.  The operation succeeds if
        // 'length' bytes are successfully read from the input sequence of the
        // 'streamBuf' without the read position becoming unavailable, and the
        // bytes contain a valid representation of an extended-binary time and
        // time zone.

    // 'bdlt::TimeTz' Encoding

    static TimeTzEncoding::Value selectTimeTzEncoding(
                                             const bdlt::TimeTz&      value,
                                             const BerEncoderOptions *options);
        // Determine the format that should be used to encode the specified
        // 'value' given the 'value' and the specified 'options'.  If 'options'
        // is 0, the default set of options is used.  Return an enumerator
        // identifying the selected format.

    static int putIso8601TimeTzValue(bsl::streambuf          *streamBuf,
                                     const bdlt::TimeTz&      value,
                                     const BerEncoderOptions *options);
        // Write the ISO 8601 representation of the specified 'value' to the
        // output sequence of the specified 'streamBuf' according to the
        // specified 'options'.  If 'options' is 0, the default set of options
        // is used.  Return 0 on success, and a non-zero value otherwise.  The
        // operation succeeds if all bytes of the ISO 8601 representation of
        // the 'value' are written to the 'streamBuf' without the write
        // position becoming unavailable.

    static int putCompactBinaryTimeValue(bsl::streambuf          *streamBuf,
                                         const bdlt::TimeTz&      value,
                                         const BerEncoderOptions *options);
        // Write the compact-binary time representation of the specified
        // 'value' to the output sequence of the specified 'streamBuf'
        // according to the specified 'options'.  If 'options' is 0, the
        // default set of options is used.  Return 0 on success, and a non-zero
        // value otherwise.  The operation succeeds if all bytes of the
        // compact-binary time representation of the 'value' are written to the
        // 'streamBuf' without the write position becoming unavailable.  The
        // behavior is undefined unless the 'offset' of the 'value' is 0.

    static int putCompactBinaryTimeTzValue(bsl::streambuf          *streamBuf,
                                           const bdlt::TimeTz&      value,
                                           const BerEncoderOptions *options);
        // Write the compact-binary date and time zone representation of the
        // specified 'value' to the output sequence of the specified
        // 'streamBuf' according to the specified 'options'.  If 'options' is
        // 0, the default set of options is used.  Return 0 on success, and a
        // non-zero value otherwise.  The operation succeeds if all bytes of
        // the compact-binary date representation of the 'value' are written to
        // the 'streamBuf' without the write position becoming unavailable.

    static int putExtendedBinaryTimeTzValue(bsl::streambuf          *streamBuf,
                                            const bdlt::TimeTz&      value,
                                            const BerEncoderOptions *options);
        // Write the extended-binary date and time zone representation of the
        // specified 'value' to the output sequence of the specified
        // 'streamBuf' according to the specified 'options'.  If 'options' is
        // 0, the default set of options is used.  Return 0 on success, and a
        // non-zero value otherwise.  The operation succeeds if all bytes of
        // the extended-binary date representation of the 'value' are written
        // to the 'streamBuf' without the write position becoming unavailable.

    // Variant Decoding

    static int detectTimeOrTimeTzEncoding(
                                       TimeOrTimeTzEncoding::Value *encoding,
                                       int                          length,
                                       unsigned char                firstByte);
        // Load to the specified 'encoding' the enumerator that describes the
        // format used to encode a 'bdlt::Time' or 'bdlt::TimeTz' value given
        // the specified 'length' and 'firstByte' of the encoded
        // representation.  Return 0 on success, -1 if the format is reserved
        // for future use, and some other non-zero value otherwise.

    static int getIso8601TimeValue(TimeOrTimeTz   *value,
                                   bsl::streambuf *streamBuf,
                                   int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // time value represented by the interpretation of the read bytes as an
        // ISO 8601 time.  Return 0 on success, and a non-zero value otherwise.
        // The operation succeeds if 'length' bytes are successfully read from
        // the input sequence of the 'streamBuf' without the read position
        // becoming unavailable, and the bytes contain a valid representation
        // of an ISO 8601 time.

    static int getIso8601TimeTzValue(TimeOrTimeTz   *value,
                                     bsl::streambuf *streamBuf,
                                     int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // time and time zone value represented by the interpretation of the
        // read bytes as an ISO 8601 time and time zone.  Return 0 on success,
        // and a non-zero value otherwise.  The operation succeeds if 'length'
        // bytes are successfully read from the input sequence of the
        // 'streamBuf' without the read position becoming unavailable, and the
        // bytes contain a valid representation of an ISO 8601 time and time
        // zone.

    static int getCompactBinaryTimeValue(TimeOrTimeTz   *value,
                                         bsl::streambuf *streamBuf,
                                         int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // time value represented by the interpretation of the read bytes as a
        // compact-binary time.  Return 0 on success, and a non-zero value
        // otherwise.  The operation succeeds if 'length' bytes are
        // successfully read from the input sequence of the 'streamBuf' without
        // the read position becoming unavailable, and the bytes contain a
        // valid representation of a compact-binary time.

    static int getCompactBinaryTimeTzValue(TimeOrTimeTz   *value,
                                           bsl::streambuf *streamBuf,
                                           int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // time and time zone value represented by the interpretation of the
        // read bytes as a compact-binary time and time zone.  Return 0 on
        // success, and a non-zero value otherwise.  The operation succeeds if
        // 'length' bytes are successfully read from the input sequence of the
        // 'streamBuf' without the read position becoming unavailable, and the
        // bytes contain a valid representation of a compact-binary time and
        // time zone.

    static int getExtendedBinaryTimeValue(TimeOrTimeTz   *value,
                                          bsl::streambuf *streamBuf,
                                          int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // time value represented by the interpretation of the read bytes as a
        // extended-binary time.  Return 0 on success, and a non-zero value
        // otherwise.  The operation succeeds if 'length' bytes are
        // successfully read from the input sequence of the 'streamBuf' without
        // the read position becoming unavailable, and the bytes contain a
        // valid representation of an extended-binary time.

    static int getExtendedBinaryTimeTzValue(TimeOrTimeTz   *value,
                                            bsl::streambuf *streamBuf,
                                            int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // time and time zone value represented by the interpretation of the
        // read bytes as an extended-binary time and time zone.  Return 0 on
        // success, and a non-zero value otherwise.  The operation succeeds if
        // 'length' bytes are successfully read from the input sequence of the
        // 'streamBuf' without the read position becoming unavailable, and the
        // bytes contain a valid representation of an extended-binary time and
        // time zone.

  public:
    // CLASS METHODS

    // Utilities

    static void timeToMillisecondsSinceMidnight(
                                  int               *millisecondsSinceMidnight,
                                  const bdlt::Time&  time);
        // Load to the specified 'millisecondsSinceMidnight' the number of
        // milliseconds in the specified 'time' value.

    static void timeToMicrosecondsSinceMidnight(
                                 bsls::Types::Int64 *microsecondsSinceMidnight,
                                 const bdlt::Time&   time);
        // Load to the specified 'microsecondsSinceMidnight' the number of
        // microseconds in the specified 'time' value.

    static int millisecondsSinceMidnightToTime(
                                        bdlt::Time *time,
                                        int         millisecondsSinceMidnight);
        // Load to the specified 'time' the time value represented by the
        // specified 'millisecondsSinceMidnight'.

    static int microsecondsSinceMidnightToTime(
                                bdlt::Time         *time,
                                bsls::Types::Int64  microsecondsSinceMidnight);
        // Load to the specified 'time' the time value represented by the
        // specified 'microsecondsSinceMidnight'.

    // 'bdlt::Time' Decoding

    static int getTimeValue(bdlt::Time     *value,
                            bsl::streambuf *streamBuf,
                            int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // time value represented those bytes.  Return 0 on success, and a
        // non-zero value otherwise.  The operation succeeds if 'length' bytes
        // are successfully read from the input sequence of the 'streamBuf'
        // without the read position becoming unavailable, and the bytes
        // contain a valid representation of a time value.  See the
        // package-level documentation of {'balber'} for a description of the
        // decision procedure used to detect the encoding format for a
        // 'bdlt::Time' value.

    // 'bdlt::Time' Encoding

    static int putTimeValue(bsl::streambuf          *streamBuf,
                            const bdlt::Time&        value,
                            const BerEncoderOptions *options);
        // Write a representation of the specified time 'value' to the output
        // sequence of the specified 'streamBuf' according to the specified
        // 'options'.  If 'options' is 0, the default set of options is used.
        // Return 0 on success, and a non-zero value otherwise.  This operation
        // succeeds if all bytes in the representation of the 'value' are
        // written to the output sequence of the 'streamBuf' without the write
        // position becoming unavailable.  See the class documentation for a
        // description of the default options.  See the package-level
        // documentation of {'balber'} for a description of the decision
        // procedure used to select an encoding format for the 'value'.

    // 'bdlt::TimeTz' Decoding

    static int getTimeTzValue(bdlt::TimeTz   *value,
                              bsl::streambuf *streamBuf,
                              int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // time and time zone value represented by those bytes.  Return 0 on
        // success, and a non-zero value otherwise.  The operation succeeds if
        // 'length' bytes are successfully read from the input sequence of the
        // 'streamBuf' without the read position becoming unavailable, and the
        // bytes contain a valid representation of a time and time zone value.
        // See the package-level documentation of {'balber'} for a description
        // of the decision procedure used to detect the encoding format for a
        // 'bdlt::TimeTz' value.

    // 'bdlt::TimeTz' Encoding

    static int putTimeTzValue(bsl::streambuf          *streamBuf,
                              const bdlt::TimeTz&      value,
                              const BerEncoderOptions *options);
        // Write a representation of the specified time and time-zone 'value'
        // to the output sequence of the specified 'streamBuf' according to the
        // specified 'options'.  If 'options' is 0, the default set of options
        // is used.  Return 0 on success, and a non-zero value otherwise.  This
        // operation succeeds if all bytes in the representation of the 'value'
        // are written to the output sequence of the 'streamBuf' without the
        // write position becoming unavailable.  See the class documentation
        // for a description of the default options.  See the package-level
        // documentation of {'balber'} for a description of the decision
        // procedure used to select an encoding format for the 'value'.

    // Variant Decoding

    static int getTimeOrTimeTzValue(TimeOrTimeTz   *value,
                                    bsl::streambuf *streamBuf,
                                    int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // time and optional time zone value represented by those bytes.
        // Return 0 on success, and a non-zero value otherwise.  The operation
        // succeeds if 'length' bytes are successfully read from the input
        // sequence of the 'streamBuf' without the read position becoming
        // unavailable, and the bytes contain a valid representation of a time
        // and optional time zone value.  See the package-level documentation
        // of {'balber'} for a description of the decision procedure used to
        // detect the encoding format for a 'TimeOrTimeTz' value.
};

                      // ===============================
                      // struct BerUtil_DatetimeEncoding
                      // ===============================

struct BerUtil_DatetimeEncoding {
    // This component-private utility 'struct' provides a namespace for
    // enumerating the set of formats that may be used by 'BerUtil' to encode
    // and decode values of 'bdlt::Datetime' type.

    // TYPES
    typedef BerUtil_DateAndTimeEncoding Encoding;
        // 'Encoding' is an alias to a namespace for enumerating the union of
        // the sets of date and time formats used to encode and decode all date
        // and time types supported by 'BerUtil'.

    enum Value {
        e_ISO8601_DATETIME          = Encoding::e_ISO8601_DATETIME,
        e_COMPACT_BINARY_DATETIME   = Encoding::e_COMPACT_BINARY_DATETIME,
        e_COMPACT_BINARY_DATETIMETZ = Encoding::e_COMPACT_BINARY_DATETIMETZ,
        e_EXTENDED_BINARY_DATETIME  = Encoding::e_EXTENDED_BINARY_DATETIMETZ
    };

    enum {
        k_EXTENDED_BINARY_MIN_BDE_VERSION =
            Encoding::k_EXTENDED_BINARY_MIN_BDE_VERSION
    };
};

                     // =================================
                     // struct BerUtil_DatetimeTzEncoding
                     // =================================

struct BerUtil_DatetimeTzEncoding {
    // This component-private utility 'struct' provides a namespace for
    // enumerating the set of formats that may be used by 'BerUtil' to encode
    // and decode values of 'bdlt::DatetimeTz' type.

    // TYPES
    typedef BerUtil_DateAndTimeEncoding Encoding;
        // 'Encoding' is an alias to a namespace for enumerating the union of
        // the sets of date and time formats used to encode and decode all date
        // and time types supported by 'BerUtil'.

    enum Value {
        e_ISO8601_DATETIMETZ         = Encoding::e_ISO8601_DATETIMETZ,
        e_COMPACT_BINARY_DATETIME    = Encoding::e_COMPACT_BINARY_DATETIME,
        e_COMPACT_BINARY_DATETIMETZ  = Encoding::e_COMPACT_BINARY_DATETIMETZ,
        e_EXTENDED_BINARY_DATETIMETZ = Encoding::e_EXTENDED_BINARY_DATETIMETZ
    };

    enum {
        k_EXTENDED_BINARY_MIN_BDE_VERSION =
            Encoding::k_EXTENDED_BINARY_MIN_BDE_VERSION
    };
};

                // ===========================================
                // struct BerUtil_DatetimeOrDatetimeTzEncoding
                // ===========================================

struct BerUtil_DatetimeOrDatetimeTzEncoding {
    // This component-private utility 'struct' provides a namespace for
    // enumerating the set of formats that may be used by 'BerUtil' to decode
    // to values of 'bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>' type.

    // TYPES
    typedef BerUtil_DateAndTimeEncoding Encoding;
        // 'Encoding' is an alias to a namespace for enumerating the union of
        // the sets of date and time formats used to encode and decode all date
        // and time types supported by 'BerUtil'.

    enum Value {
        e_ISO8601_DATETIME           = Encoding::e_ISO8601_DATETIME,
        e_ISO8601_DATETIMETZ         = Encoding::e_ISO8601_DATETIMETZ,
        e_COMPACT_BINARY_DATETIME    = Encoding::e_COMPACT_BINARY_DATETIME,
        e_COMPACT_BINARY_DATETIMETZ  = Encoding::e_COMPACT_BINARY_DATETIMETZ,
        e_EXTENDED_BINARY_DATETIME   = Encoding::e_EXTENDED_BINARY_DATETIME,
        e_EXTENDED_BINARY_DATETIMETZ = Encoding::e_EXTENDED_BINARY_DATETIMETZ
    };
};

                       // ==============================
                       // struct BerUtil_DatetimeImpUtil
                       // ==============================

struct BerUtil_DatetimeImpUtil {
    // This component-private 'struct' provides a namespace for a suite of
    // functions used by 'BerUtil' to implement BER encoding and decoding
    // operations for date and time values.  Within the definition of this
    // 'struct':
    //
    //: *the* *specification*:
    //:   Refers to the August 2015 revision of the ITU-T Recommendation X.690,
    //:   and
    //:
    //: *the* *default* *set* *of* *options*:
    //:   Refers to a 'balber::BerEncoderOptions' value having a
    //:   'datetimeFractionalSecondPrecision' attribute of 3 and a
    //:   'encodeDateAndTimeTypesAsBinary' attribute of 'false'.
    //
    // See the package level documentation of {'balber'} for a definition of
    // the compact and extended binary formats for date and time values.

    // TYPES
    typedef BerUtil_Constants Constants;
        // 'Constants' is an alias to a namespace for a suite of
        // general-purpose constants that occur when encoding or decoding BER
        // data.

    typedef BerUtil_ExtendedBinaryEncodingUtil ExtendedBinaryEncodingUtil;
        // 'DateAndTimeHeaderUtil' is an alias to a namespace for a suite of
        // functions used to implement encoding and decoding operations for the
        // 2-byte header of an extended-binary-encoding formatted date-and-time
        // value.

    typedef BerUtil_DateAndTimeHeader DateAndTimeHeader;
        // 'Header' is an alias to an in-core, value-semantic attribute class
        // that represents the range of valid values of the 2-byte header of
        // extended-binary-encoding formatted date-and-time values.

    typedef BerUtil_DateAndTimeHeaderImpUtil DateAndTimeHeaderUtil;
        // 'DateAndTimeHeaderUtil' is an alias to a namespace for a suite of
        // functions used to implement encoding and decoding operations for the
        // 2-byte header of an extended-binary-encoding formatted date-and-time
        // value.

    typedef BerUtil_DateImpUtil DateUtil;
        // 'DateUtil' is an alias to a namespace for a suite of functions used
        // to implement BER encoding and decoding operations for date values.

    typedef BerUtil_DatetimeEncoding DatetimeEncoding;
        // 'DatetimeEncoding' is an alias to a namespace for enumerating the
        // set of formats that may be used by 'BerUtil' to encode and decode
        // values of 'bdlt::Datetime' type.

    typedef BerUtil_DatetimeTzEncoding DatetimeTzEncoding;
        // 'DatetimeTzEncoding' is an alias to a namespace for enumerating the
        // set of formats that may be used by 'BerUtil' to encode and decode
        // values of 'bdlt::DatetimeTz' type.

    typedef BerUtil_DatetimeOrDatetimeTzEncoding DatetimeOrDatetimeTzEncoding;
        // 'DatetimeOrDatetimeTzEncoding' is an alias to a namespace for
        // enumerating the set of formats that may be used by 'BerUtil' to
        // decode to values of
        // 'bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>' type.

    typedef BerUtil_IntegerImpUtil IntegerUtil;
        // 'IntegerUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoding and decoding operations for integer
        // values.

    typedef BerUtil_Iso8601ImpUtil Iso8601Util;
        // 'Iso8601Util' is an alias to a namespace for a suite of functions
        // used to implementing the encoding and decoding of date and time
        // values using the ISO 8601 format.

    typedef BerUtil_LengthImpUtil LengthUtil;
        // 'LengthUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoding and decoding operations for length
        // quantities.

    typedef BerUtil_StreambufUtil StreambufUtil;
        // 'StreambufUtil' is an alias to a namespace for a suite of functions
        // used to implement input and output operations on 'bsl::streambuf'
        // objects.

    typedef BerUtil_StringImpUtil StringUtil;
        // 'StringUtil' is an alias to a namespace for a suite of functions
        // used by 'BerUtil' to implement BER encoding and decoding operations
        // for string values.

    typedef BerUtil_TimeImpUtil TimeUtil;
        // 'DateUtil' is an alias to a namespace for a suite of functions used
        // to implement BER encoding and decoding operations for time values.

    typedef BerUtil_TimezoneOffsetImpUtil TimezoneUtil;
        // 'TimezoneUtil' is an alias to a namespace for a suite of functions
        // used to implement BER encoding and decoding operations for time-zone
        // offset values.

    typedef bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
        DatetimeOrDatetimeTz;
        // 'DatetimeOrDatetimeTz' is a convenient alias for
        // 'bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>'.

  private:
    // PRIVATE TYPES
    enum {
        k_EXTENDED_BINARY_SERIAL_DATE_LENGTH = 3,

        k_EXTENDED_BINARY_DATETIME_LENGTH =
            DateAndTimeHeaderUtil::k_HEADER_LENGTH +
            k_EXTENDED_BINARY_SERIAL_DATE_LENGTH +
            IntegerUtil::k_40_BIT_INTEGER_LENGTH,  // = 10
        // the number of content octets used by 'BerUtil' to encode a date
        // and time value using the extended-binary date and time format

        k_EXTENDED_BINARY_DATETIMETZ_LENGTH =
            DateAndTimeHeaderUtil::k_HEADER_LENGTH +
            k_EXTENDED_BINARY_SERIAL_DATE_LENGTH +
            IntegerUtil::k_40_BIT_INTEGER_LENGTH,  // = 10
        // the number of contents octets used by 'BerUtil' to encode a
        // date, time, and time zone value using the extended-binary date,
        // time, and time zone format

        k_MAX_ISO8601_DATETIME_LENGTH = bdlt::Iso8601Util::k_DATETIME_STRLEN,
        // the maximum number of content octets used by 'BerUtil' to
        // encode a date and time value using the ISO 8601 format

        k_MAX_ISO8601_DATETIMETZ_LENGTH =
            bdlt::Iso8601Util::k_DATETIMETZ_STRLEN,
        // the maximum number of content octets used by 'BerUtil' to
        // encode a date, time, and time zone value using the ISO 8601
        // format

        k_MAX_COMPACT_BINARY_DATETIME_LENGTH = 6,
        // the maximum number of content octets used by 'BerUtil' to encode
        // a date and time value using the compact-binary date and time
        // format

        k_MIN_COMPACT_BINARY_DATETIMETZ_LENGTH =
            k_MAX_COMPACT_BINARY_DATETIME_LENGTH + 1,
        // the minimum number of content octets used by 'BerUtil' to
        // encode a date, time, and time zone value using the
        // compact-binary date, time, and time zone format

        k_MAX_COMPACT_BINARY_DATETIMETZ_LENGTH = 9
        // the maximum number of content octets used by 'BerUtil' to
        // encode a date, time, and time zone value using the
        // compact-binary date, time, and time zone format
    };

    // PRIVATE CLASS METHODS

    // Utilities

    static void datetimeToMillisecondsSinceEpoch(
                                 bsls::Types::Int64    *millisecondsSinceEpoch,
                                 const bdlt::Datetime&  value);
        // Load to the specified 'millisecondsFromEpoch' the number of
        // milliseconds between the start of the day on the compact-binary date
        // epoch and the specified 'value'.  The compact-binary date epoch is
        // the date defined by the 'DateUtil::k_COMPACT_BINARY_DATE_EPOCH'
        // serial date.  Note that this quantity may be negative if the
        // specified 'value' occurs before the compact-binary date epoch.

    static int millisecondsSinceEpochToDatetime(
                                   bdlt::Datetime     *value,
                                   bsls::Types::Int64  millisecondsSinceEpoch);
        // Load to the specified 'value' the date and time represented by the
        // specified 'millisecondsSinceEpoch' number of milliseconds from the
        // compact-binary date epoch.  The compact-binary date epoch is the
        // date defined by the 'DateUtil::k_COMPACT_BINARY_DATE_EPOCH' serial
        // date.  Return 0 on success, and a non-zero value otherwise.  The
        // operation succeeds if the resulting date and time is a valid
        // 'bdlt::Datetime' value.  Note that 'millisecondsSinceEpoch' may be
        // negative to indicate a date and time that occurs before the
        // compact-binary date epoch.

    // 'bdlt::Datetime' Decoding

    static int detectDatetimeEncoding(DatetimeEncoding::Value *encoding,
                                      int                      length,
                                      unsigned char            firstByte);
        // Load to the specified 'encoding' the enumerator that describes the
        // format used to encode a 'bdlt::Datetime' value given the specified
        // 'length' and 'firstByte' of the encoded representation.  Return 0 on
        // success, -1 if the format is reserved for future use, and some other
        // non-zero value otherwise.

    static int getIso8601DatetimeValue(bdlt::Datetime *value,
                                       bsl::streambuf *streamBuf,
                                       int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date and time value represented by the interpretation of the read
        // bytes as an ISO 8601 date and time.  Return 0 on success, and a
        // non-zero value otherwise.  The operation succeeds if 'length' bytes
        // are successfully read from the input sequence of the 'streamBuf'
        // without the read position becoming unavailable, and the bytes
        // contain a valid representation of an ISO 8601 date and time.

    static int getCompactBinaryDatetimeValue(bdlt::Datetime *value,
                                             bsl::streambuf *streamBuf,
                                             int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date and time value represented by the interpretation of the read
        // bytes as a compact-binary date and time.  Return 0 on success, and a
        // non-zero value otherwise.  The operation succeeds if 'length' bytes
        // are successfully read from the input sequence of the 'streamBuf'
        // without the read position becoming unavailable, and the bytes
        // contain a valid representation of a compact-binary date and time.

    static int getCompactBinaryDatetimeTzValue(bdlt::Datetime *value,
                                               bsl::streambuf *streamBuf,
                                               int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date and time value represented by the interpretation of the read
        // bytes as a compact-binary date, time, and time zone.  Return 0 on
        // success, and a non-zero value otherwise.  The operation succeeds if
        // 'length' bytes are successfully read from the input sequence of the
        // 'streamBuf' without the read position becoming unavailable, and the
        // bytes contain a valid representation of a compact-binary date, time,
        // and time zone.

    static int getExtendedBinaryDatetimeValue(bdlt::Datetime *value,
                                              bsl::streambuf *streamBuf,
                                              int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date and time value represented by the interpretation of the read
        // bytes as an extended-binary date, time, and time zone.  Return 0 on
        // success, and a non-zero value otherwise.  The operation succeeds if
        // 'length' bytes are successfully read from the input sequence of the
        // 'streamBuf' without the read position becoming unavailable, and the
        // bytes contain a valid representation of an extended-binary date,
        // time, and time zone.

    // 'bdlt::Datetime' Encoding

    static DatetimeEncoding::Value selectDatetimeEncoding(
                                       bsls::Types::Int64      *serialDatetime,
                                       int                     *length,
                                       const bdlt::Datetime&    value,
                                       const BerEncoderOptions *options);
        // Determine the format that should be used to encode the specified
        // 'value' given the 'value' and the specified 'options'.  Load to the
        // specified 'serialDatetime' the number of milliseconds since the
        // start of the day on the compact-binary date epoch to the 'value',
        // and load to the specified 'length' the number of contents octets
        // that would be used by the BER encoding of 'serialDatetime' according
        // to the specification.  If 'options' is 0, the default set of options
        // is used.  Return an enumerator identifying the selected format.
        // Note that the 'serialDatetime' and 'length' of a date and time value
        // are frequently used as arguments to date and time encoding
        // operations defined in this 'struct'.

    static int putIso8601DatetimeValue(bsl::streambuf          *streamBuf,
                                       const bdlt::Datetime&    value,
                                       const BerEncoderOptions *options);
        // Write the ISO 8601 representation of the specified 'value' to the
        // output sequence of the specified 'streamBuf' according to the
        // specified 'options'.  If 'options' is 0, the default set of options
        // is used.  Return 0 on success, and a non-zero value otherwise.  The
        // operation succeeds if all bytes of the ISO 8601 representation of
        // the 'value' are written to the 'streamBuf' without the write
        // position becoming unavailable.

    static int putCompactBinaryDatetimeValue(
                                       bsl::streambuf          *streamBuf,
                                       bsls::Types::Int64       serialDatetime,
                                       int                      length,
                                       const BerEncoderOptions *options);
        // Write the specified 'length' number of octets of the compact-binary
        // date and time representation of the specified 'serialDatetime'
        // number of milliseconds from the start of the day on the
        // compact-binary serial epoch to the output sequence of the specified
        // 'streamBuf' according to the specified 'options'.  If 'options' is
        // 0, the default set of options is used.  Return 0 on success, and a
        // non-zero value otherwise.  The operation succeeds if all 'length'
        // bytes of the representation of the 'serialDatetime' are written to
        // the 'streamBuf' without the write position becoming unavailable.

    static int putCompactBinaryDatetimeValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::Datetime&    value,
                                            const BerEncoderOptions *options);
        // Write the compact-binary date and time representation of the
        // specified 'value' to the output sequence of the specified
        // 'streamBuf' according to the specified 'options'.  If 'options' is
        // 0, the default set of options is used.  Return 0 on success, and a
        // non-zero value otherwise.  The operation succeeds if all bytes of
        // the compact-binary date and time representation of the 'value' are
        // written to the 'streamBuf' without the write position becoming
        // unavailable.

    static int putCompactBinaryDatetimeTzValue(
                                       bsl::streambuf          *streamBuf,
                                       bsls::Types::Int64       serialDatetime,
                                       int                      length,
                                       const BerEncoderOptions *options);
        // Write the specified 'length' number of octets of the compact-binary
        // date, time, and time zone representation of the specified
        // 'serialDatetime' number of milliseconds from the start of the day on
        // the compact-binary serial epoch to the output sequence of the
        // specified 'streamBuf' according to the specified 'options'.  If
        // 'options' is 0, the default set of options is used.  Return 0 on
        // success, and a non-zero value otherwise.  The operation succeeds if
        // all 'length' bytes of the representation of the 'serialDatetime' are
        // written to the 'streamBuf' without the write position becoming
        // unavailable.

    static int putExtendedBinaryDatetimeValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::Datetime&    value,
                                            const BerEncoderOptions *options);
        // Write the extended-binary date and time representation of the
        // specified 'value' to the output sequence of the specified
        // 'streamBuf' according to the specified 'options'.  If 'options' is
        // 0, the default set of options is used.  Return 0 on success, and a
        // non-zero value otherwise.  The operation succeeds if all bytes of
        // the extended-binary date and time representation of the 'value' are
        // written to the 'streamBuf' without the write position becoming
        // unavailable.

    // 'bdlt::DatetimeTz' Decoding

    static int detectDatetimeTzEncoding(
                                   DatetimeTzEncoding::Value *encoding,
                                   int                        length,
                                   unsigned char              firstByte);
        // Load to the specified 'encoding' the enumerator that describes the
        // format used to encode a 'bdlt::DatetimeTz' value given the specified
        // 'length' and 'firstByte' of the encoded representation.  Return 0 on
        // success, -1 if the format is reserved for future use, and some other
        // non-zero value otherwise.

    static int getIso8601DatetimeTzValue(bdlt::DatetimeTz *value,
                                         bsl::streambuf   *streamBuf,
                                         int               length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date, time, and time zone value represented by the interpretation of
        // the read bytes as an ISO 8601 date, time, and time zone.  Return 0
        // on success, and a non-zero value otherwise.  The operation succeeds
        // if 'length' bytes are successfully read from the input sequence of
        // the 'streamBuf' without the read position becoming unavailable, and
        // the bytes contain a valid representation of an ISO 8601 date, time,
        // and time zone .

    static int getCompactBinaryDatetimeValue(bdlt::DatetimeTz *value,
                                             bsl::streambuf   *streamBuf,
                                             int               length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date value represented by the interpretation of the read bytes as a
        // compact-binary date and time.  Return 0 on success, and a non-zero
        // value otherwise.  The operation succeeds if 'length' bytes are
        // successfully read from the input sequence of the 'streamBuf' without
        // the read position becoming unavailable, and the bytes contain a
        // valid representation of a compact-binary date and time.

    static int getCompactBinaryDatetimeTzValue(bdlt::DatetimeTz *value,
                                               bsl::streambuf   *streamBuf,
                                               int               length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date, time, and time zone value represented by the interpretation of
        // the read bytes as a compact-binary date, time, and time zone.
        // Return 0 on success, and a non-zero value otherwise.  The operation
        // succeeds if 'length' bytes are successfully read from the input
        // sequence of the 'streamBuf' without the read position becoming
        // unavailable, and the bytes contain a valid representation of a
        // compact-binary date, time, and time zone.

    static int getExtendedBinaryDatetimeTzValue(bdlt::DatetimeTz *value,
                                                bsl::streambuf   *streamBuf,
                                                int               length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date and time zone value represented by the interpretation of the
        // read bytes as an extended-binary date, time, and time zone.  Return
        // 0 on success, and a non-zero value otherwise.  The operation
        // succeeds if 'length' bytes are successfully read from the input
        // sequence of the 'streamBuf' without the read position becoming
        // unavailable, and the bytes contain a valid representation of an
        // extended-binary date, time, and time zone.

    // 'bdlt::DatetimeTz' Encoding

    static DatetimeTzEncoding::Value selectDatetimeTzEncoding(
                                       bsls::Types::Int64      *serialDatetime,
                                       int                     *length,
                                       const bdlt::DatetimeTz&  value,
                                       const BerEncoderOptions *options);
        // Determine the format that should be used to encode the specified
        // 'value' given the 'value' and the specified 'options'.  If 'options'
        // is 0, the default set of options is used.  Return an enumerator
        // identifying the selected format.

    static int putIso8601DatetimeTzValue(bsl::streambuf          *streamBuf,
                                         const bdlt::DatetimeTz&  value,
                                         const BerEncoderOptions *options);
        // Write the ISO 8601 representation of the specified 'value' to the
        // output sequence of the specified 'streamBuf' according to the
        // specified 'options'.  If 'options' is 0, the default set of options
        // is used.  Return 0 on success, and a non-zero value otherwise.  The
        // operation succeeds if all bytes of the ISO 8601 representation of
        // the 'value' are written to the 'streamBuf' without the write
        // position becoming unavailable.

    static int putCompactBinaryDatetimeTzValue(
                              bsl::streambuf          *streamBuf,
                              int                      timezoneOffsetInMinutes,
                              bsls::Types::Int64       serialDatetime,
                              int                      serialDatetimeLength,
                              const BerEncoderOptions *options);
        // Write the specified 'serialDatetimeLength' number of octets of the
        // compact-binary date, time, and time zone representation using the
        // specified the date and time defined by the specified
        // 'serialDatetime' number of milliseconds from the start of the day on
        // the compact-binary serial epoch and the specified
        // 'timezoneOffsetInMinutes' time zone to the output sequence of the
        // specified 'streamBuf' according to the specified 'options'.  If
        // 'options' is 0, the default set of options is used.  Return 0 on
        // success, and a non-zero value otherwise.  The operation succeeds if
        // all 'length' bytes of the compact-binary date, time, and time zone
        // representation of the 'serialDatetime' and 'timezoneOffsetInMinutes'
        // are written to the 'streamBuf' without the write position becoming
        // unavailable.

    static int putExtendedBinaryDatetimeTzValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::DatetimeTz&  value,
                                            const BerEncoderOptions *options);
        // Write the extended-binary date, time, and time zone representation
        // of the specified 'value' to the output sequence of the specified
        // 'streamBuf' according to the specified 'options'.  If 'options' is
        // 0, the default set of options is used.  Return 0 on success, and a
        // non-zero value otherwise.  The operation succeeds if all bytes of
        // the extended-binary date, time, and time zone representation of the
        // 'value' are written to the 'streamBuf' without the write position
        // becoming unavailable.

    // Variant Decoding

    static int detectDatetimeOrDatetimeTzEncoding(
                               DatetimeOrDatetimeTzEncoding::Value *encoding,
                               int                                  length,
                               unsigned char                        firstByte);
        // Load to the specified 'encoding' the enumerator that describes the
        // format used to encode a 'bdlt::Datetime' or 'bdlt::DatetimeTz' value
        // given the specified 'length' and 'firstByte' of the encoded
        // representation.  Return 0 on success, -1 if the format is reserved
        // for future use, and some other non-zero value otherwise.

    static int getIso8601DatetimeValue(DatetimeOrDatetimeTz *value,
                                       bsl::streambuf       *streamBuf,
                                       int                   length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date, time, and optional time zone value represented by the
        // interpretation of the read bytes as an ISO 8601 date and time.
        // Return 0 on success, and a non-zero value otherwise.  The operation
        // succeeds if 'length' bytes are successfully read from the input
        // sequence of the 'streamBuf' without the read position becoming
        // unavailable, and the bytes contain a valid representation of an ISO
        // 8601 date and time.

    static int getIso8601DatetimeTzValue(DatetimeOrDatetimeTz *value,
                                         bsl::streambuf       *streamBuf,
                                         int                   length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date, time, and optional time zone value represented by the
        // interpretation of the read bytes as an ISO 8601 date, time, and time
        // zone.  Return 0 on success, and a non-zero value otherwise.  The
        // operation succeeds if 'length' bytes are successfully read from the
        // input sequence of the 'streamBuf' without the read position becoming
        // unavailable, and the bytes contain a valid representation of an ISO
        // 8601 date, time, and time zone.

    static int getCompactBinaryDatetimeValue(DatetimeOrDatetimeTz *value,
                                             bsl::streambuf       *streamBuf,
                                             int                   length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date, time, and optional time zone value represented by the
        // interpretation of the read bytes as a compact-binary date and time.
        // Return 0 on success, and a non-zero value otherwise.  The operation
        // succeeds if 'length' bytes are successfully read from the input
        // sequence of the 'streamBuf' without the read position becoming
        // unavailable, and the bytes contain a valid representation of a
        // compact-binary date and time.

    static int getCompactBinaryDatetimeTzValue(DatetimeOrDatetimeTz *value,
                                               bsl::streambuf       *streamBuf,
                                               int                   length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date, time, and optional time zone value represented by the
        // interpretation of the read bytes as a compact-binary date, time, and
        // time zone.  Return 0 on success, and a non-zero value otherwise.
        // The operation succeeds if 'length' bytes are successfully read from
        // the input sequence of the 'streamBuf' without the read position
        // becoming unavailable, and the bytes contain a valid representation
        // of a compact-binary date, time, and time zone.

    static int getExtendedBinaryDatetimeValue(DatetimeOrDatetimeTz *value,
                                              bsl::streambuf       *streamBuf,
                                              int                   length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // time value represented by the interpretation of the read bytes as a
        // extended-binary date and time.  Return 0 on success, and a non-zero
        // value otherwise.  The operation succeeds if 'length' bytes are
        // successfully read from the input sequence of the 'streamBuf' without
        // the read position becoming unavailable, and the bytes contain a
        // valid representation of an extended-binary date and time.

    static int getExtendedBinaryDatetimeTzValue(
                                               DatetimeOrDatetimeTz *value,
                                               bsl::streambuf       *streamBuf,
                                               int                   length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // time value represented by the interpretation of the read bytes as a
        // extended-binary date, time, and time zone.  Return 0 on success, and
        // a non-zero value otherwise.  The operation succeeds if 'length'
        // bytes are successfully read from the input sequence of the
        // 'streamBuf' without the read position becoming unavailable, and the
        // bytes contain a valid representation of an extended-binary date,
        // time, and time zone.

  public:
    // CLASS METHODS

    // 'bdlt::Datetime' Decoding

    static int getDatetimeValue(bdlt::Datetime *value,
                                bsl::streambuf *streamBuf,
                                int             length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date and time value represented those bytes.  Return 0 on success,
        // and a non-zero value otherwise.  The operation succeeds if 'length'
        // bytes are successfully read from the input sequence of the
        // 'streamBuf' without the read position becoming unavailable, and the
        // bytes contain a valid representation of a date and time value.  See
        // the package-level documentation of
        // {'balber'} for a description of the decision procedure used to
        // detect the encoding format for a 'bdlt::Datetime' value.

    // 'bdlt::Datetime' Encoding

    static int putDatetimeValue(bsl::streambuf          *streamBuf,
                                const bdlt::Datetime&    value,
                                const BerEncoderOptions *options);
        // Write a representation of the specified 'value' date and time to the
        // output sequence of the specified 'streamBuf' according to the
        // specified 'options'.  If 'options' is 0, the default set of options
        // is used.  Return 0 on success, and a non-zero value otherwise.  This
        // operation succeeds if all bytes in the representation of the 'value'
        // are written to the output sequence of the 'streamBuf' without the
        // write position becoming unavailable.  See the class documentation
        // for a description of the default options.  See the package-level
        // documentation of {'balber'} for a description of the decision
        // procedure used to select an encoding format for the 'value'.

    // 'bdlt::DatetimeTz' Decoding

    static int getDatetimeTzValue(bdlt::DatetimeTz *value,
                                  bsl::streambuf   *streamBuf,
                                  int               length);
        // Read the specified 'length' number of bytes from the input sequence
        // of the specified 'streamBuf' and load to the specified 'value' the
        // date, time, and time zone value represented those bytes.  Return 0
        // on success, and a non-zero value otherwise.  The operation succeeds
        // if 'length' bytes are successfully read from the input sequence of
        // the 'streamBuf' without the read position becoming unavailable, and
        // the bytes contain a valid representation of a date, time, and time
        // zone value.  See the package-level documentation of {'balber'} for a
        // description of the decision procedure used to detect the encoding
        // format for a 'bdlt::Datetime' value.

    // 'bdlt::DatetimeTz' Encoding

    static int putDatetimeTzValue(bsl::streambuf          *streamBuf,
                                  const bdlt::DatetimeTz&  value,
                                  const BerEncoderOptions *options);
        // Write a representation of the specified 'value' date, time, and time
        // zone to the output sequence of the specified 'streamBuf' according
        // to the specified 'options'.  If 'options' is 0, the default set of
        // options is used.  Return 0 on success, and a non-zero value
        // otherwise.  This operation succeeds if all bytes in the
        // representation of the 'value' are written to the output sequence of
        // the 'streamBuf' without the write position becoming unavailable.
        // See the class documentation for a description of the default
        // options.  See the package-level documentation of {'balber'} for a
        // description of the decision procedure used to select an encoding
        // format for the 'value'.

    // Variant Decoding

    static int getDatetimeOrDatetimeTzValue(DatetimeOrDatetimeTz *value,
                                            bsl::streambuf       *streamBuf,
                                            int                   length);
        // Write a representation of the specified 'value' date, time, and
        // optional time zone to the output sequence of the specified
        // 'streamBuf' according to the specified 'options'.  If 'options' is
        // 0, the default set of options is used.  Return 0 on success, and a
        // non-zero value otherwise.  This operation succeeds if all bytes in
        // the representation of the 'value' are written to the output sequence
        // of the 'streamBuf' without the write position becoming unavailable.
        // See the class documentation for a description of the default
        // options.  See the package-level documentation of {'balber'} for a
        // description of the decision procedure used to select an encoding
        // format for the 'value'.
};

                       // ==============================
                       // struct BerUtil_GetValueImpUtil
                       // ==============================

struct BerUtil_GetValueImpUtil {
    // This component-private utility 'struct' provides a namespace for a suite
    // of functions that define the overload set for the implementation of
    // 'balber::BerUtil::getValue'.  The set of types used for the 'value'
    // parameters in the overload set of 'getValue' in this 'struct' define the
    // set of types that 'balber::BerUtil::getValue' supports.

    // TYPES
    typedef BerUtil_BooleanImpUtil BooleanUtil;
        // 'BooleanUtil' is an alias to a namespace for a suite of functions
        // used by 'BerUtil' to implement BER encoding and decoding operations
        // for boolean values.

    typedef BerUtil_CharacterImpUtil CharacterUtil;
        // 'CharacterUtil' is an alias to a namespace for a suite of functions
        // used by 'BerUtil' to implement BER encoding and decoding operations
        // for byte values.

    typedef BerUtil_DateImpUtil DateUtil;
        // 'DateUtil' is an alias to a namespace for a suite of functions used
        // by 'BerUtil' to implement BER encoding and decoding operations for
        // date values.

    typedef BerUtil_DatetimeImpUtil DatetimeUtil;
        // 'DatetimeUtil' is an alias to a namespace for a suite of functions
        // used by 'BerUtil' to implement BER encoding and decoding operations
        // for date and time values.

    typedef BerUtil_FloatingPointImpUtil FloatingPointUtil;
        // 'FloatingPointUtil' is an alias to a namespace for a suite of
        // functions used by 'BerUtil' to implement BER encoding and decoding
        // operations for floating-point number values.

    typedef BerUtil_IntegerImpUtil IntegerUtil;
        // 'IntegerUtil' is an alias to a namespace for a suite of functions
        // used by 'BerUtil' to implement BER encoding and decoding operations
        // for integer values.

    typedef BerUtil_StringImpUtil StringUtil;
        // 'StringUtil' is an alias to a namespace for a suite of functions
        // used by 'BerUtil' to implement BER encoding and decoding operations
        // for string values.

    typedef BerUtil_TimeImpUtil TimeUtil;
        // 'TimeUtil' is an alias to a namespace for a suite of functions used
        // by 'BerUtil' to implement BER encoding and decoding operations for
        // time values.

    typedef bdlb::Variant2<bdlt::Date, bdlt::DateTz> DateOrDateTz;
        // 'DateOrDateTz' is a convenient alias for
        // 'bdlb::Variant2<bdlt::Date, bdlt::DateTz>'.

    typedef bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
        DatetimeOrDatetimeTz;
        // 'DatetimeOrDatetimeTz' is a convenient alias for
        // 'bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>'.

    typedef bdlb::Variant2<bdlt::Time, bdlt::TimeTz> TimeOrTimeTz;
        // 'TimeOrTimeTz' is a convenient alias for
        // 'bdlb::Variant2<bdlt::Time, bdlt::TimeTz>'.

    // CLASS METHODS
    template <typename TYPE>
    static int getValue(
                      TYPE                     *value,
                      bsl::streambuf           *streamBuf,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
    static int getValue(
                      bool                     *value,
                      bsl::streambuf           *streamBuf,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
    static int getValue(
                      char                     *value,
                      bsl::streambuf           *streamBuf,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
    static int getValue(
                      unsigned char            *value,
                      bsl::streambuf           *streamBuf,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
    static int getValue(
                      signed char              *value,
                      bsl::streambuf           *streamBuf,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
    static int getValue(
                      float                    *value,
                      bsl::streambuf           *streamBuf,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
    static int getValue(
                      double                   *value,
                      bsl::streambuf           *streamBuf,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
    static int getValue(
                      bdldfp::Decimal64        *value,
                      bsl::streambuf           *streamBuf,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
    static int getValue(
                      bsl::string              *value,
                      bsl::streambuf           *streamBuf,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
    static int getValue(
                      bdlt::Date               *value,
                      bsl::streambuf           *streamBuf,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
    static int getValue(
                      bdlt::DateTz             *value,
                      bsl::streambuf           *streamBuf,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
    static int getValue(
                      DateOrDateTz             *value,
                      bsl::streambuf           *streamBuf,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
    static int getValue(
                      bdlt::Datetime           *value,
                      bsl::streambuf           *streamBuf,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
    static int getValue(
                      bdlt::DatetimeTz         *value,
                      bsl::streambuf           *streamBuf,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
    static int getValue(
                      DatetimeOrDatetimeTz     *value,
                      bsl::streambuf           *streamBuf,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
    static int getValue(
                      bdlt::Time               *value,
                      bsl::streambuf           *streamBuf,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
    static int getValue(
                      bdlt::TimeTz             *value,
                      bsl::streambuf           *streamBuf,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
    static int getValue(
                      TimeOrTimeTz             *value,
                      bsl::streambuf           *streamBuf,
                      int                       length,
                      const BerDecoderOptions&  options = BerDecoderOptions());
        // Decode the specified 'value' from the specified 'streamBuf',
        // consuming exactly the specified 'length' bytes.  Return 0 on
        // success, and a non-zero value otherwise.  Optionally specify
        // decoding 'options' to control aspects of the decoding.  Note that
        // the value consists of the contents of the bytes only (no length
        // prefix).  Also note that only fundamental C++ types, 'bsl::string',
        // and BDE date/time types are supported.
};

                       // ==============================
                       // struct BerUtil_PutValueImpUtil
                       // ==============================

struct BerUtil_PutValueImpUtil {
    // This component-private utility 'struct' provides a namespace for a suite
    // of functions that define the overload set for the implementation of
    // 'balber::BerUtil::putValue'.  The set of types used for the 'value'
    // parameters in the overload set of 'putValue' in this 'struct' define the
    // set of types that 'balber::BerUtil::putValue' supports.

    // TYPES
    typedef BerUtil_BooleanImpUtil BooleanUtil;
        // 'BooleanUtil' is an alias to a namespace for a suite of functions
        // used by 'BerUtil' to implement BER encoding and decoding operations
        // for boolean values.

    typedef BerUtil_CharacterImpUtil CharacterUtil;
        // 'CharacterUtil' is an alias to a namespace for a suite of functions
        // used by 'BerUtil' to implement BER encoding and decoding operations
        // for byte values.

    typedef BerUtil_DateImpUtil DateUtil;
        // 'DateUtil' is an alias to a namespace for a suite of functions used
        // by 'BerUtil' to implement BER encoding and decoding operations for
        // date values.

    typedef BerUtil_DatetimeImpUtil DatetimeUtil;
        // 'DatetimeUtil' is an alias to a namespace for a suite of functions
        // used by 'BerUtil' to implement BER encoding and decoding operations
        // for date and time values.

    typedef BerUtil_FloatingPointImpUtil FloatingPointUtil;
        // 'FloatingPointUtil' is an alias to a namespace for a suite of
        // functions used by 'BerUtil' to implement BER encoding and decoding
        // operations for floating-point number values.

    typedef BerUtil_IntegerImpUtil IntegerUtil;
        // 'IntegerUtil' is an alias to a namespace for a suite of functions
        // used by 'BerUtil' to implement BER encoding and decoding operations
        // for integer values.

    typedef BerUtil_StringImpUtil StringUtil;
        // 'StringUtil' is an alias to a namespace for a suite of functions
        // used by 'BerUtil' to implement BER encoding and decoding operations
        // for string values.

    typedef BerUtil_TimeImpUtil TimeUtil;
        // 'TimeUtil' is an alias to a namespace for a suite of functions used
        // by 'BerUtil' to implement BER encoding and decoding operations for
        // time values.

    // CLASS METHODS
    template <typename TYPE>
    static int putValue(bsl::streambuf          *streamBuf,
                        const TYPE&              value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf          *streamBuf,
                        bool                     value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf          *streamBuf,
                        char                     value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf          *streamBuf,
                        unsigned char            value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf          *streamBuf,
                        signed char              value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf          *streamBuf,
                        float                    value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf          *streamBuf,
                        double                   value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf          *streamBuf,
                        bdldfp::Decimal64        value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf          *streamBuf,
                        const bsl::string&       value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf           *streamBuf,
                        const bslstl::StringRef&  value,
                        const BerEncoderOptions  *options);
    static int putValue(bsl::streambuf          *streamBuf,
                        const bdlt::Date&        value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf          *streamBuf,
                        const bdlt::DateTz&      value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf          *streamBuf,
                        const bdlt::Datetime&    value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf          *streamBuf,
                        const bdlt::DatetimeTz&  value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf          *streamBuf,
                        const bdlt::Time&        value,
                        const BerEncoderOptions *options);
    static int putValue(bsl::streambuf          *streamBuf,
                        const bdlt::TimeTz&      value,
                        const BerEncoderOptions *options);
        // Encode the specified 'value' to the specified 'streamBuf'.  Return 0
        // on success, and a non-zero value otherwise.  Note that the value
        // consists of the length and contents primitives.  Also note that only
        // fundamental C++ types, 'bsl::string', 'bslstl::StringRef' and BDE
        // date/time types are supported.
};

                             // ==================
                             // struct BerUtil_Imp
                             // ==================

struct BerUtil_Imp {
    // This component-private utility 'struct' exists to provide
    // backwards-compatability for external components that depend upon the
    // facilities provided by this 'struct'.

    // CLASS METHODS
    static int putStringValue(bsl::streambuf *streamBuf,
                              const char     *string,
                              int             stringLength);
        // Write the length and contents octets of the BER encoding of the
        // specified character 'string' having the specified 'stringLength' (as
        // defined in the specification) to the output sequence of the
        // specified 'streamBuf'.  Return 0 if successful, and a non-zero value
        // otherwise.  The operation succeeds if and only if all bytes
        // corresponding to the length and contents octets are written to the
        // 'streamBuf' without the write position becoming unavailable.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                               // --------------
                               // struct BerUtil
                               // --------------

// CLASS METHODS
inline
int BerUtil::getEndOfContentOctets(bsl::streambuf *streamBuf,
                                   int            *accumNumBytesConsumed)
{
    return BerUtil_LengthImpUtil::getEndOfContentOctets(accumNumBytesConsumed,
                                                        streamBuf);
}

inline
int BerUtil::getLength(bsl::streambuf *streamBuf,
                       int            *result,
                       int            *accumNumBytesConsumed)
{
    return BerUtil_LengthImpUtil::getLength(
        result, accumNumBytesConsumed, streamBuf);
}

template <class TYPE>
inline
int BerUtil::getValue(bsl::streambuf           *streamBuf,
                      TYPE                     *value,
                      int                       length,
                      const BerDecoderOptions&  options)
{
    return BerUtil_GetValueImpUtil::getValue(
        value, streamBuf, length, options);
}

template <typename TYPE>
inline
int BerUtil::getValue(bsl::streambuf           *streamBuf,
                      TYPE                     *value,
                      int                      *accumNumBytesConsumed,
                      const BerDecoderOptions&  options)
{
    int length;
    if (BerUtil_LengthImpUtil::getLength(
            &length, accumNumBytesConsumed, streamBuf)) {
        return -1;                                                    // RETURN
    }

    if (BerUtil::getValue(streamBuf, value, length, options)) {
        return -1;                                                    // RETURN
    }

    *accumNumBytesConsumed += length;
    return 0;
}

inline
int BerUtil::putEndOfContentOctets(bsl::streambuf *streamBuf)
{
    return BerUtil_LengthImpUtil::putEndOfContentOctets(streamBuf);
}

inline
int BerUtil::putIndefiniteLengthOctet(bsl::streambuf *streamBuf)
{
    return BerUtil_LengthImpUtil::putIndefiniteLengthOctet(streamBuf);
}

inline
int BerUtil::putLength(bsl::streambuf *streamBuf, int length)
{
    return BerUtil_LengthImpUtil::putLength(streamBuf, length);
}

template <typename TYPE>
inline
int BerUtil::putValue(bsl::streambuf          *streamBuf,
                      const TYPE&              value,
                      const BerEncoderOptions *options)
{
    return BerUtil_PutValueImpUtil::putValue(streamBuf, value, options);
}

                        // ----------------------------
                        // struct BerUtil_StreambufUtil
                        // ----------------------------

// CLASS METHODS
inline
int BerUtil_StreambufUtil::peekChar(char *value, bsl::streambuf *streamBuf)
{
    const bsl::streambuf::int_type byte = streamBuf->sgetc();
    if (bsl::streambuf::traits_type::eof() == byte) {
        return -1;                                                    // RETURN
    }

    *value = bsl::streambuf::traits_type::to_char_type(byte);
    return 0;
}

inline
int BerUtil_StreambufUtil::getChars(char           *buffer,
                                    bsl::streambuf *streamBuf,
                                    int             bufferLength)
{
    const bsl::streamsize numCharsRead =
        streamBuf->sgetn(buffer, static_cast<bsl::streamsize>(bufferLength));

    if (numCharsRead != static_cast<bsl::streamsize>(bufferLength)) {
        return -1;                                                    // RETURN
    }

    return 0;
}

inline
int BerUtil_StreambufUtil::putChars(bsl::streambuf *streamBuf,
                                    const char     *buffer,
                                    int             bufferLength)
{
    const bsl::streamsize numCharsWritten =
        streamBuf->sputn(buffer, static_cast<bsl::streamsize>(bufferLength));

    if (numCharsWritten != bufferLength) {
        return -1;                                                    // RETURN
    }

    return 0;
}

                      // --------------------------------
                      // struct BerUtil_RawIntegerImpUtil
                      // --------------------------------

// CLASS METHODS
template <typename TYPE>
int BerUtil_RawIntegerImpUtil::putIntegerGivenLength(bsl::streambuf *streamBuf,
                                                     TYPE            value,
                                                     int             length)
{
    enum { k_BDEM_SUCCESS = 0, k_BDEM_FAILURE = -1 };

    if (length <= 0) {
        return k_BDEM_FAILURE;                                        // RETURN
    }

    static const bool isUnsigned = (TYPE(-1) > TYPE(0));

    if (isUnsigned && (unsigned)length == sizeof(TYPE) + 1) {
        static const TYPE SGN_BIT = static_cast<TYPE>(
            static_cast<TYPE>(1)
            << (sizeof(TYPE) * Constants::k_NUM_BITS_PER_OCTET - 1));
        // Length may be one greater than 'sizeof(TYPE)' only if type is
        // unsigned and the high bit (normally the sign bit) is set.  In this
        // case, a leading zero octet is emitted.

        if (!(value & SGN_BIT)) {
            return k_BDEM_FAILURE;                                    // RETURN
        }

        if (0 != streamBuf->sputc(0)) {
            return k_BDEM_FAILURE;                                    // RETURN
        }

        --length;
    }

    if (static_cast<unsigned>(length) > sizeof(TYPE)) {
        return k_BDEM_FAILURE;                                        // RETURN
    }

#if BSLS_PLATFORM_IS_BIG_ENDIAN
    return length == streamBuf->sputn(
                         static_cast<char *>(static_cast<void *>(&value)) +
                             sizeof(TYPE) - length,
                         length)
               ? k_BDEM_SUCCESS
               : k_BDEM_FAILURE;
#else

    char *dst = static_cast<char *>(static_cast<void *>(&value)) + length;
    for (; length > 0; --length) {
        unsigned char c = *--dst;
        if (c != streamBuf->sputc(c)) {
            return k_BDEM_FAILURE;                                    // RETURN
        }
    }

    return k_BDEM_SUCCESS;

#endif
}

                       // -----------------------------
                       // struct BerUtil_BooleanImpUtil
                       // -----------------------------

// Decoding

inline
int BerUtil_BooleanImpUtil::getBoolValue(bool           *value,
                                         bsl::streambuf *streamBuf,
                                         int             length)
{
    if (1 != length) {
        return -1;                                                    // RETURN
    }

    int intValue = streamBuf->sbumpc();
    if (bsl::streambuf::traits_type::eof() == intValue) {
        return -1;                                                    // RETURN
    }

    *value = 0 != intValue;

    return 0;
}

// Encoding

inline
int BerUtil_BooleanImpUtil::putBoolValue(bsl::streambuf *streamBuf, bool value)
{
    // It has been observed in practice that 'value' may refer to uninitialized
    // or overwritten memory, in which case its value may neither be 'true' (1)
    // nor 'false' (0).  We assert here to ensure that users get a useful error
    // message.  Note that we assert (rather than returning an error code), as
    // it is undefined behavior to examine the value of such an uninitialized
    // 'bool'.  Also note that gcc complains about this assert when used with
    // the '-Wlogical-op' flag.  Therefore, to silence this warning/error we
    // cast the 'bool' value to a 'char *' and check the value referred to by
    // the 'char *'.

    BSLMF_ASSERT(sizeof(bool) == sizeof(char));
    BSLS_ASSERT(0 == *static_cast<char *>(static_cast<void *>(&value)) ||
                1 == *static_cast<char *>(static_cast<void *>(&value)));

    typedef bsl::streambuf::char_type char_type;

    if (0 != LengthUtil::putLength(streamBuf, 1)) {
        return -1;                                                    // RETURN
    }

    if (static_cast<int>(value) !=
        streamBuf->sputc(static_cast<char_type>(value ? 1 : 0))) {
        return -1;
    }

    return 0;
}

                       // -----------------------------
                       // struct BerUtil_IntegerImpUtil
                       // -----------------------------

// CLASS METHODS
template <typename TYPE>
int BerUtil_IntegerImpUtil::getNumOctetsToStream(TYPE value)
{
    int numBytes = sizeof(TYPE);

    BSLMF_ASSERT(sizeof(TYPE) > 1);

    // The 2 double casts to 'TYPE' in this function are necessary because if
    // the type is 64 bits the innermost cast is need to widen the constant
    // before the shift, and the outermost cast is needed if the type is
    // narrower than 'int'.

    static const TYPE NEG_MASK = static_cast<TYPE>(
        static_cast<TYPE>(0xff80)
        << ((sizeof(TYPE) - 2) * Constants::k_NUM_BITS_PER_OCTET));
    if (0 == value) {
        numBytes = 1;
    }
    else if (value > 0) {
        static const TYPE SGN_BIT = static_cast<TYPE>(
            static_cast<TYPE>(1)
            << (sizeof(TYPE) * Constants::k_NUM_BITS_PER_OCTET - 1));
        if (value & SGN_BIT) {
            // If 'value > 0' but the high bit (sign bit) is set, then this is
            // an unsigned value and a leading zero byte must be emitted to
            // prevent the value from looking like a negative value on the
            // wire.  The leading zero is followed by all of the bytes of the
            // unsigned value.

            return static_cast<int>(sizeof(TYPE) + 1);                // RETURN
        }

        // This mask zeroes out the most significant byte and the first bit of
        // the next byte.

        static const TYPE POS_MASK = TYPE(~NEG_MASK);
        while ((value & POS_MASK) == value) {
            value = static_cast<TYPE>(value << 8);
            // shift out redundant high-order 0x00
            --numBytes;
        }
    }
    else {  // 0 > value
        while ((value | NEG_MASK) == value) {
            value = static_cast<TYPE>(value << 8);
            // shift out redundant high-order 0xFF
            --numBytes;
        }
    }

    BSLS_ASSERT(numBytes > 0);
    return numBytes;
}

template <typename TYPE>
int BerUtil_IntegerImpUtil::getIntegerValue(TYPE           *value,
                                            bsl::streambuf *streamBuf,
                                            int             length)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    enum { k_SIGN_BIT_MASK = 0x80 };

    static const bool isUnsigned = (TYPE(-1) > TYPE(0));

    if (isUnsigned && static_cast<unsigned>(length) == sizeof(TYPE) + 1) {
        // Length of an unsigned is allowed to be one larger then
        // 'sizeof(TYPE)' only if first byte is zero.  (This is so that large
        // unsigned numbers do not appear as negative numbers in the BER
        // stream).  Remove the leading zero byte.

        if (0 != streamBuf->sbumpc()) {
            // First byte was not zero.  Fail.

            return k_FAILURE;                                         // RETURN
        }

        --length;
    }

    if (static_cast<unsigned>(length) > sizeof(TYPE)) {
        // Overflow.

        return k_FAILURE;                                             // RETURN
    }

    *value = static_cast<TYPE>(streamBuf->sgetc() & k_SIGN_BIT_MASK ? -1 : 0);

    for (int i = 0; i < length; ++i) {
        int nextOctet = streamBuf->sbumpc();
        if (bsl::streambuf::traits_type::eof() == nextOctet) {
            return k_FAILURE;                                         // RETURN
        }

        const unsigned long long mask =
            (1ull << ((sizeof(TYPE) - 1) * Constants::k_NUM_BITS_PER_OCTET)) -
            1;
        *value = static_cast<TYPE>((*value & mask)
                                   << Constants::k_NUM_BITS_PER_OCTET);
        *value =
            static_cast<TYPE>(*value | static_cast<unsigned char>(nextOctet));
    }

    return k_SUCCESS;
}

inline
int BerUtil_IntegerImpUtil::get40BitIntegerValue(bsls::Types::Int64 *value,
                                                 bsl::streambuf     *streamBuf)
{
    char bytes[5];
    if (0 != StreambufUtil::getChars(bytes, streamBuf, sizeof(bytes))) {
        return -1;                                                    // RETURN
    }

    const bsls::Types::Uint64 byte0 =
        static_cast<bsls::Types::Uint64>(static_cast<unsigned char>(bytes[0]))
        << (8 * 4);

    const bsls::Types::Uint64 byte1 =
        static_cast<bsls::Types::Uint64>(static_cast<unsigned char>(bytes[1]))
        << (8 * 3);

    const bsls::Types::Uint64 byte2 =
        static_cast<bsls::Types::Uint64>(static_cast<unsigned char>(bytes[2]))
        << (8 * 2);

    const bsls::Types::Uint64 byte3 =
        static_cast<bsls::Types::Uint64>(static_cast<unsigned char>(bytes[3]))
        << (8 * 1);

    const bsls::Types::Uint64 byte4 =
        static_cast<bsls::Types::Uint64>(static_cast<unsigned char>(bytes[4]))
        << (8 * 0);

    const bsls::Types::Uint64 unsignedValue =
        byte0 + byte1 + byte2 + byte3 + byte4;

    *value = static_cast<bsls::Types::Int64>(unsignedValue);

    return 0;
}

template <typename TYPE>
int BerUtil_IntegerImpUtil::putIntegerValue(bsl::streambuf *streamBuf,
                                            TYPE            value)
{
    typedef bsl::streambuf::char_type char_type;

    const int length = getNumOctetsToStream(value);
    if (length != streamBuf->sputc(static_cast<char_type>(length))) {
        return -1;                                                    // RETURN
    }

    return putIntegerGivenLength(streamBuf, value, length);
}

template <typename TYPE>
int BerUtil_IntegerImpUtil::putIntegerGivenLength(bsl::streambuf *streamBuf,
                                                  TYPE            value,
                                                  int             length)
{
    return RawIntegerUtil::putIntegerGivenLength(streamBuf, value, length);
}

inline
int BerUtil_IntegerImpUtil::put40BitIntegerValue(bsl::streambuf     *streamBuf,
                                                 bsls::Types::Int64  value)
    ///Implementation Note
    ///-------------------
    // This implementation requires the platform use a 2's complement
    // representation for signed integer values.
{
    BSLS_ASSERT(-549755813888ll <= value);
    BSLS_ASSERT(549755813888ll > value);

    const bsls::Types::Uint64 unsignedValue = value;

    const char bytes[5] = {
        static_cast<char>(
            static_cast<unsigned char>((unsignedValue >> (8 * 4)) & 0xFF)),
        static_cast<char>(
            static_cast<unsigned char>((unsignedValue >> (8 * 3)) & 0xFF)),
        static_cast<char>(
            static_cast<unsigned char>((unsignedValue >> (8 * 2)) & 0xFF)),
        static_cast<char>(
            static_cast<unsigned char>((unsignedValue >> (8 * 1)) & 0xFF)),
        static_cast<char>(
            static_cast<unsigned char>((unsignedValue >> (8 * 0)) & 0xFF))};

    return StreambufUtil::putChars(streamBuf, bytes, sizeof(bytes));
}

                      // -------------------------------
                      // struct BerUtil_CharacterImpUtil
                      // -------------------------------

// CLASS METHODS

// Decoding

inline
int BerUtil_CharacterImpUtil::getCharValue(char           *value,
                                           bsl::streambuf *streamBuf,
                                           int             length)
{
    switch (length) {
      case 1:
        break;
      case 2:
        if (0 != streamBuf->sbumpc()) {
            // see 'getIntegerValue', if this 'char' had been encoded as
            // 'unsigned' there might be a leading 0 which is acceptable, but
            // any other value for the first byte is invalid
            return -1;                                                // RETURN
        }
        break;
      default:
        return -1;                                                    // RETURN
    }

    int valueOctet = streamBuf->sbumpc();
    if (bsl::streambuf::traits_type::eof() == valueOctet) {
        return -1;                                                    // RETURN
    }

    *value = static_cast<char>(valueOctet);
    return 0;
}

inline
int BerUtil_CharacterImpUtil::getSignedCharValue(signed char    *value,
                                                 bsl::streambuf *streamBuf,
                                                 int             length)
{
    char temp;
    if (0 != getCharValue(&temp, streamBuf, length)) {
        return -1;                                                    // RETURN
    }

    *value = static_cast<signed char>(temp);
    return 0;
}

inline
int BerUtil_CharacterImpUtil::getUnsignedCharValue(unsigned char  *value,
                                                   bsl::streambuf *streamBuf,
                                                   int             length)
{
    short temp;
    if (IntegerUtil::getIntegerValue(&temp, streamBuf, length)) {
        return -1;                                                    // RETURN
    }

    *value = static_cast<unsigned char>(temp);
    return 0;
}

// Encoding

inline
int BerUtil_CharacterImpUtil::putCharValue(bsl::streambuf *streamBuf,
                                           char            value)
{
    if (0 != LengthUtil::putLength(streamBuf, 1)) {
        return -1;                                                    // RETURN
    }

    if (static_cast<unsigned char>(value) != streamBuf->sputc(value)) {
        return -1;                                                    // RETURN
    }

    return 0;
}

inline
int BerUtil_CharacterImpUtil::putSignedCharValue(bsl::streambuf *streamBuf,
                                                 signed char     value)
{
    return putCharValue(streamBuf, static_cast<char>(value));
}

inline
int BerUtil_CharacterImpUtil::putUnsignedCharValue(bsl::streambuf *streamBuf,
                                                   unsigned char   value)
{
    return IntegerUtil::putIntegerValue(streamBuf,
                                        static_cast<unsigned short>(value));
}

                    // -----------------------------------
                    // struct BerUtil_FloatingPointImpUtil
                    // -----------------------------------

// CLASS METHODS

// Decoding

inline
int BerUtil_FloatingPointImpUtil::getFloatValue(float          *value,
                                                bsl::streambuf *streamBuf,
                                                int             length)
{
    double tentativeValue;
    if (0 != getDoubleValue(&tentativeValue, streamBuf, length)) {
        return -1;                                                    // RETURN
    }

    *value = static_cast<float>(tentativeValue);

    return 0;
}

// Encoding

inline
int BerUtil_FloatingPointImpUtil::putFloatValue(bsl::streambuf *streamBuf,
                                                float           value)
{
    return putDoubleValue(streamBuf, static_cast<double>(value));
}

                        // ----------------------------
                        // struct BerUtil_StringImpUtil
                        // ----------------------------

// CLASS METHODS

// Encoding Utilities

inline
int BerUtil_StringImpUtil::putRawStringValue(bsl::streambuf *streamBuf,
                                             const char     *value,
                                             int             valueLength)
{
    if (0 != LengthUtil::putLength(streamBuf, valueLength)) {
        return -1;                                                    // RETURN
    }

    if (valueLength != streamBuf->sputn(value, valueLength)) {
        return -1;                                                    // RETURN
    }

    return 0;
}

// 'bsl::string' Encoding

inline
int BerUtil_StringImpUtil::putStringValue(bsl::streambuf     *streamBuf,
                                          const bsl::string&  value)
{
    return putRawStringValue(
        streamBuf, value.data(), static_cast<int>(value.length()));
}

// 'bslstl::StringRef' Encoding

inline
int BerUtil_StringImpUtil::putStringRefValue(
                                           bsl::streambuf           *streamBuf,
                                           const bslstl::StringRef&  value)
{
    return putRawStringValue(
        streamBuf, value.data(), static_cast<int>(value.length()));
}

                       // -----------------------------
                       // struct BerUtil_Iso8601ImpUtil
                       // -----------------------------

// PRIVATE CLASS METHODS
template <class TYPE>
int BerUtil_Iso8601ImpUtil::getValue(TYPE           *value,
                                     bsl::streambuf *streamBuf,
                                     int             length)
{
    if (length <= 0) {
        return -1;                                                    // RETURN
    }

    char               localBuf[32];  // for common case where length < 32
    bsl::vector<char>  vecBuf;        // for length >= 32
    char              *buf;

    if (length < 32) {
        buf = localBuf;
    }
    else {
        vecBuf.resize(length);
        buf = &vecBuf[0];  // First byte of contiguous string
    }

    const bsl::streamsize bytesConsumed = streamBuf->sgetn(buf, length);
    if (static_cast<int>(bytesConsumed) != length) {
        return -1;                                                    // RETURN
    }

    return bdlt::Iso8601Util::parse(value, buf, length);
}

template <class TYPE>
int BerUtil_Iso8601ImpUtil::putValue(bsl::streambuf          *streamBuf,
                                     const TYPE&              value,
                                     const BerEncoderOptions *options)
{
    char                           buf[bdlt::Iso8601Util::k_MAX_STRLEN];
    bdlt::Iso8601UtilConfiguration config;
    int                            datetimeFractionalSecondPrecision =
        options ? options->datetimeFractionalSecondPrecision() : 6;
    config.setFractionalSecondPrecision(datetimeFractionalSecondPrecision);
    int len = bdlt::Iso8601Util::generate(buf, sizeof(buf), value, config);

    return StringUtil::putStringRefValue(streamBuf,
                                         bslstl::StringRef(buf, len));
}

                 // --------------------------------------
                 // struct BerUtil_ExtendedBinaryEncodingUtil
                 // --------------------------------------

// CLASS METHODS
inline
bool BerUtil_ExtendedBinaryEncodingUtil::useExtendedBinaryEncoding(
                                              const bdlt::Time&        value,
                                              const BerEncoderOptions *options)
{
    if (!options) {
        // If encoding options are not specified, by default the ISO8601 format
        // is used.
        return false;                                                 // RETURN
    }

    const bool useBinaryEncoding = options->encodeDateAndTimeTypesAsBinary();

    const bool useMicrosecondPrecision =
        (6 == options->datetimeFractionalSecondPrecision());

    const bool compactBinaryEncodingOfValueIsBuggy = (bdlt::Time() == value);
    // The compact binary encoding format ambiguously encodes both the
    // '00:00:00' time value and the '24:00:00' time value to the same bit
    // pattern.  The decoder treats this bit pattern as the '00:00:00' time
    // value.  The extended binary encoding format does not have this
    // limitation.  Therefore, if the extended binary format is allowed, it
    // should be used to encode default time values even if the fractional
    // second precision is not 6.

    const bool useExtendedBinaryEncodingIfAllowed =
        useMicrosecondPrecision || compactBinaryEncodingOfValueIsBuggy;

    const bool extendedBinaryEncodingIsAllowed =
        (options->bdeVersionConformance() >=
         Encoding::k_EXTENDED_BINARY_MIN_BDE_VERSION);

    return useBinaryEncoding && useExtendedBinaryEncodingIfAllowed &&
           extendedBinaryEncodingIsAllowed;
}

inline
bool BerUtil_ExtendedBinaryEncodingUtil::useExtendedBinaryEncoding(
                                              const bdlt::TimeTz&      value,
                                              const BerEncoderOptions *options)
{
    return useExtendedBinaryEncoding(value.localTime(), options);
}

inline
bool BerUtil_ExtendedBinaryEncodingUtil::useExtendedBinaryEncoding(
                                              const bdlt::Datetime&    value,
                                              const BerEncoderOptions *options)
{
    return useExtendedBinaryEncoding(value.time(), options);
}

inline
bool BerUtil_ExtendedBinaryEncodingUtil::useExtendedBinaryEncoding(
                                              const bdlt::DatetimeTz&  value,
                                              const BerEncoderOptions *options)
{
    return useExtendedBinaryEncoding(value.localDatetime().time(), options);
}

inline
bool BerUtil_ExtendedBinaryEncodingUtil::useBinaryEncoding(
                                              const BerEncoderOptions *options)
{
    return options && options->encodeDateAndTimeTypesAsBinary();
}

                      // -------------------------------
                      // class BerUtil_DateAndTimeHeader
                      // -------------------------------

// CREATORS
inline
BerUtil_DateAndTimeHeader::BerUtil_DateAndTimeHeader()
: d_type(Type::e_NOT_EXTENDED_BINARY)
, d_timezoneOffsetInMinutes(0)
{
}

// MANIPULATORS
inline
void BerUtil_DateAndTimeHeader::makeNotExtendedBinary()
{
    d_type                    = Type::e_NOT_EXTENDED_BINARY;
    d_timezoneOffsetInMinutes = 0;
}

inline
void BerUtil_DateAndTimeHeader::makeExtendedBinaryWithoutTimezone()
{
    d_type                    = Type::e_EXTENDED_BINARY_WITHOUT_TIMEZONE;
    d_timezoneOffsetInMinutes = 0;
}

inline
void BerUtil_DateAndTimeHeader::makeExtendedBinaryWithTimezone(int offset)
{
    BSLS_ASSERT(TimezoneUtil::isValidTimezoneOffsetInMinutes(offset));

    d_type                    = Type::e_EXTENDED_BINARY_WITH_TIMEZONE;
    d_timezoneOffsetInMinutes = offset;
}

// ACCESSORS
inline
bool BerUtil_DateAndTimeHeader::isExtendedBinary() const
{
    return (Type::e_EXTENDED_BINARY_WITHOUT_TIMEZONE == d_type) ||
           (Type::e_EXTENDED_BINARY_WITH_TIMEZONE == d_type);
}

inline
bool BerUtil_DateAndTimeHeader::isExtendedBinaryWithoutTimezone() const
{
    return Type::e_EXTENDED_BINARY_WITHOUT_TIMEZONE == d_type;
}

inline
bool BerUtil_DateAndTimeHeader::isExtendedBinaryWithTimezone() const
{
    return Type::e_EXTENDED_BINARY_WITH_TIMEZONE == d_type;
}

inline
int BerUtil_DateAndTimeHeader::timezoneOffsetInMinutes() const
{
    return d_timezoneOffsetInMinutes;
}

                  // ---------------------------------------
                  // struct BerUtil_DateAndTimeHeaderImpUtil
                  // ---------------------------------------

// CLASS METHODS
inline
bool BerUtil_DateAndTimeHeaderImpUtil::isReserved(unsigned char firstByte)
{
    // A date-and-time header has a reserved bit pattern if:
    //..
    //  o the first bit is 1, and any of the following are true:
    //    o the second bit is 1
    //    o the third bit is 1
    //..

    const bool bit0 = firstByte & 0x80;
    const bool bit1 = firstByte & 0x40;
    const bool bit2 = firstByte & 0x20;

    return bit0 && (bit1 || bit2);
}

inline
bool BerUtil_DateAndTimeHeaderImpUtil::isExtendedBinary(
                                                       unsigned char firstByte)
{
    const bool bit0 = firstByte & 0x80;
    const bool bit1 = firstByte & 0x40;
    const bool bit2 = firstByte & 0x20;

    return bit0 && !(bit1 || bit2);
}

inline
bool BerUtil_DateAndTimeHeaderImpUtil::isExtendedBinaryWithoutTimezone(
                                                       unsigned char firstByte)
{
    const bool bit0 = firstByte & 0x80;
    const bool bit1 = firstByte & 0x40;
    const bool bit2 = firstByte & 0x20;
    const bool bit3 = firstByte & 0x10;

    return bit0 && !(bit1 || bit2) && !bit3;
}

inline
bool BerUtil_DateAndTimeHeaderImpUtil::isExtendedBinaryWithTimezone(
                                                       unsigned char firstByte)
{
    const bool bit0 = firstByte & 0x80;
    const bool bit1 = firstByte & 0x40;
    const bool bit2 = firstByte & 0x20;
    const bool bit3 = firstByte & 0x10;

    return bit0 && !(bit1 || bit2) && bit3;
}

inline
void BerUtil_DateAndTimeHeaderImpUtil::detectTypeIfNotReserved(
                                                      bool          *reserved,
                                                      Type::Value   *type,
                                                      unsigned char  firstByte)
{
    if (isReserved(firstByte)) {
        *reserved = true;
        return;                                                       // RETURN
    }

    *reserved = false;
    detectType(type, firstByte);
}

inline
void BerUtil_DateAndTimeHeaderImpUtil::detectType(Type::Value   *type,
                                                  unsigned char  firstByte)
{
    BSLS_ASSERT_OPT(!isReserved(firstByte));

    const bool bit0 = firstByte & 0x80;
    const bool bit3 = firstByte & 0x10;

    if (bit0 & bit3) {
        *type = Type::e_EXTENDED_BINARY_WITH_TIMEZONE;
        return;                                                       // RETURN
    }

    if (bit0 & !bit3) {
        *type = Type::e_EXTENDED_BINARY_WITHOUT_TIMEZONE;
        return;                                                       // RETURN
    }

    *type = Type::e_NOT_EXTENDED_BINARY;
}

inline
int BerUtil_DateAndTimeHeaderImpUtil::getValueIfNotReserved(
                                                     Header         *value,
                                                     bsl::streambuf *streamBuf)
{
    char headerBytes[2];
    if (0 !=
        StreambufUtil::getChars(headerBytes, streamBuf, sizeof(headerBytes))) {
        return -1;                                                    // RETURN
    }

    const unsigned char headerByte0 =
        static_cast<unsigned char>(headerBytes[0]);
    const unsigned char headerByte1 =
        static_cast<unsigned char>(headerBytes[1]);

    return getValueIfNotReserved(value, headerByte0, headerByte1);
}

inline
int BerUtil_DateAndTimeHeaderImpUtil::getValueIfNotReserved(
                                                    Header        *value,
                                                    unsigned char  headerByte0,
                                                    unsigned char  headerByte1)
{
    if (isReserved(headerByte0)) {
        return -1;                                                    // RETURN
    }

    return getValue(value, headerByte0, headerByte1);
}

inline
int BerUtil_DateAndTimeHeaderImpUtil::getValue(Header         *value,
                                               bsl::streambuf *streamBuf)
{
    char headerBytes[2];
    if (0 !=
        StreambufUtil::getChars(headerBytes, streamBuf, sizeof(headerBytes))) {
        return -1;                                                    // RETURN
    }

    const unsigned char headerByte0 =
        static_cast<unsigned char>(headerBytes[0]);
    const unsigned char headerByte1 =
        static_cast<unsigned char>(headerBytes[1]);

    return getValue(value, headerByte0, headerByte1);
}

inline
int BerUtil_DateAndTimeHeaderImpUtil::getValue(Header        *value,
                                               unsigned char  headerByte0,
                                               unsigned char  headerByte1)
{
    BSLS_ASSERT_OPT(!isReserved(headerByte0));

    Type::Value type;
    detectType(&type, headerByte0);

    switch (type) {
      case Type::e_NOT_EXTENDED_BINARY: {
        value->makeNotExtendedBinary();
        return 0;                                                     // RETURN
      } break;
      case Type::e_EXTENDED_BINARY_WITHOUT_TIMEZONE: {
        // If the header indicates that it does not carry time-zone
        // information, and the low 12 bits of the header are non-zero, then
        // the header is malformed.

        if ((headerByte0 & 0x0F) | (headerByte1 & 0xFF)) {
            return -1;                                                // RETURN
        }

        value->makeExtendedBinaryWithoutTimezone();
        return 0;                                                     // RETURN
      } break;
      case Type::e_EXTENDED_BINARY_WITH_TIMEZONE: {
        enum { k_TIMEZONE_SIGN_BIT = 0x08 };

        // If the time-zone offset's sign bit is 1, then sign-extend the low
        // nibble of the first byte, which encodes the 4 hi bits of the
        // 12-bit, 2's-complement number that represents the time-zone offset
        // in minutes.

        const unsigned char timezoneOffsetHi =
            (headerByte0 & k_TIMEZONE_SIGN_BIT)
                ? ((0x0F & headerByte0) | 0xF0)
                : ((0x0F & headerByte0) | 0x00);

        const signed char signedTimezoneOffsetHi =
            static_cast<signed char>(timezoneOffsetHi);

        const unsigned char timezoneOffsetLo = headerByte1;

        const int timezoneOffset =
            (static_cast<int>(signedTimezoneOffsetHi) << 8) |
            (static_cast<int>(timezoneOffsetLo) << 0);

        if (!TimezoneUtil::isValidTimezoneOffsetInMinutes(timezoneOffset)) {
            return -1;                                                // RETURN
        }

        value->makeExtendedBinaryWithTimezone(timezoneOffset);
        return 0;
      } break;
    }

    BSLS_ASSERT_OPT(!"Reachable");
#if BSLA_UNREACHABLE_IS_ACTIVE
    BSLA_UNREACHABLE;
#else
    return -1;                                                        // RETURN
#endif
}

inline
int BerUtil_DateAndTimeHeaderImpUtil::putExtendedBinaryWithoutTimezoneValue(
                                                     bsl::streambuf *streamBuf)
{
    static const char header[k_HEADER_LENGTH] = {'\x80', '\x00'};
    return StreambufUtil::putChars(streamBuf, header, k_HEADER_LENGTH);
}

inline
int BerUtil_DateAndTimeHeaderImpUtil::putExtendedBinaryWithTimezoneValue(
                                       bsl::streambuf *streamBuf,
                                       int             timezoneOffsetInMinutes)
{
    BSLS_ASSERT(
        TimezoneUtil::isValidTimezoneOffsetInMinutes(timezoneOffsetInMinutes));

    const unsigned short offsetWord =
        static_cast<unsigned short>(timezoneOffsetInMinutes);

    static const unsigned short k_OFFSET_MASK = 0x0FFF;

    const unsigned short headerWord = 0x9000 | (offsetWord & k_OFFSET_MASK);

    const char header[k_HEADER_LENGTH] = {
        static_cast<char>((headerWord >> 8) & 0xFF),
        static_cast<char>((headerWord >> 0) & 0xFF)};

    return StreambufUtil::putChars(streamBuf, header, k_HEADER_LENGTH);
}

                         // --------------------------
                         // struct BerUtil_DateImpUtil
                         // --------------------------

// PRIVATE CLASS METHODS

// 'bdlt::Date' Decoding

inline
int BerUtil_DateImpUtil::detectDateEncoding(DateEncoding::Value *encoding,
                                            int                  length,
                                            unsigned char        firstByte)
{
    if (k_MAX_COMPACT_BINARY_DATE_LENGTH >= length) {
        *encoding = DateEncoding::e_COMPACT_BINARY_DATE;
        return 0;                                                     // RETURN
    }

    BSLS_ASSERT_SAFE(k_MAX_COMPACT_BINARY_DATE_LENGTH < length);

    if (DateAndTimeHeaderUtil::isReserved(firstByte)) {
        return -1;                                                    // RETURN
    }

    *encoding = DateEncoding::e_ISO8601_DATE;
    return 0;
}

// 'bdlt::Date' Encoding

inline
BerUtil_DateEncoding::Value BerUtil_DateImpUtil::selectDateEncoding(
                                              const bdlt::Date&,
                                              const BerEncoderOptions *options)
{
    if (options && options->encodeDateAndTimeTypesAsBinary()) {
        return DateEncoding::e_COMPACT_BINARY_DATE;                   // RETURN
    }

    return DateEncoding::e_ISO8601_DATE;
}

// 'bdlt::DateTz' Decoding

inline
int BerUtil_DateImpUtil::detectDateTzEncoding(DateTzEncoding::Value *encoding,
                                              int                    length,
                                              unsigned char          firstByte)
{
    BSLMF_ASSERT(k_MAX_COMPACT_BINARY_DATE_LENGTH <
                 k_MIN_COMPACT_BINARY_DATETZ_LENGTH);

    if (k_MIN_COMPACT_BINARY_DATETZ_LENGTH > length) {
        *encoding = DateTzEncoding::e_COMPACT_BINARY_DATE;
        return 0;                                                     // RETURN
    }

    if (k_MAX_COMPACT_BINARY_DATETZ_LENGTH >= length) {
        *encoding = DateTzEncoding::e_COMPACT_BINARY_DATETZ;
        return 0;                                                     // RETURN
    }

    BSLS_ASSERT_SAFE(k_MAX_COMPACT_BINARY_DATETZ_LENGTH < length);

    if (DateAndTimeHeaderUtil::isReserved(firstByte)) {
        return -1;                                                    // RETURN
    }

    *encoding = DateTzEncoding::e_ISO8601_DATETZ;
    return 0;
}

// 'bdlt::DateTz' Encoding

inline
BerUtil_DateTzEncoding::Value BerUtil_DateImpUtil::selectDateTzEncoding(
                                              const bdlt::DateTz&      value,
                                              const BerEncoderOptions *options)
{
    if (options && options->encodeDateAndTimeTypesAsBinary() &&
        0 == value.offset()) {
        return DateTzEncoding::e_COMPACT_BINARY_DATE;                 // RETURN
    }

    if (options && options->encodeDateAndTimeTypesAsBinary()) {
        return DateTzEncoding::e_COMPACT_BINARY_DATETZ;               // RETURN
    }

    return DateTzEncoding::e_ISO8601_DATETZ;
}

// Variant Decoding

inline
int BerUtil_DateImpUtil::detectDateOrDateTzEncoding(
                                        DateOrDateTzEncoding::Value *encoding,
                                        int                          length,
                                        unsigned char                firstByte)
{
    if (k_MAX_COMPACT_BINARY_DATE_LENGTH >= length) {
        *encoding = DateOrDateTzEncoding::e_COMPACT_BINARY_DATE;
        return 0;                                                     // RETURN
    }

    BSLS_ASSERT_SAFE(k_MAX_COMPACT_BINARY_DATE_LENGTH < length);

    if (k_MAX_COMPACT_BINARY_DATETZ_LENGTH >= length) {
        *encoding = DateOrDateTzEncoding::e_COMPACT_BINARY_DATETZ;
        return 0;                                                     // RETURN
    }

    BSLS_ASSERT_SAFE(k_MAX_COMPACT_BINARY_DATETZ_LENGTH < length);

    if (DateAndTimeHeaderUtil::isReserved(firstByte)) {
        return -1;                                                    // RETURN
    }

    if (k_MAX_ISO8601_DATE_LENGTH >= length) {
        *encoding = DateOrDateTzEncoding::e_ISO8601_DATE;
        return 0;                                                     // RETURN
    }

    BSLS_ASSERT_SAFE(k_MAX_ISO8601_DATE_LENGTH < length);

    *encoding = DateOrDateTzEncoding::e_ISO8601_DATETZ;
    return 0;
}

inline
int BerUtil_DateImpUtil::getIso8601DateValue(DateOrDateTz   *value,
                                             bsl::streambuf *streamBuf,
                                             int             length)
{
    value->createInPlace<bdlt::Date>();
    return getIso8601DateValue(&value->the<bdlt::Date>(), streamBuf, length);
}

inline
int BerUtil_DateImpUtil::getIso8601DateTzValue(DateOrDateTz   *value,
                                               bsl::streambuf *streamBuf,
                                               int             length)
{
    value->createInPlace<bdlt::DateTz>();
    return getIso8601DateTzValue(
        &value->the<bdlt::DateTz>(), streamBuf, length);
}

inline
int BerUtil_DateImpUtil::getCompactBinaryDateValue(DateOrDateTz   *value,
                                                   bsl::streambuf *streamBuf,
                                                   int             length)
{
    value->createInPlace<bdlt::Date>();
    return getCompactBinaryDateValue(
        &value->the<bdlt::Date>(), streamBuf, length);
}

inline
int BerUtil_DateImpUtil::getCompactBinaryDateTzValue(DateOrDateTz   *value,
                                                     bsl::streambuf *streamBuf,
                                                     int             length)
{
    value->createInPlace<bdlt::DateTz>();
    return getCompactBinaryDateTzValue(
        &value->the<bdlt::DateTz>(), streamBuf, length);
}

// CLASS METHODS

// Variant Decoding

inline
int BerUtil_DateImpUtil::getDateOrDateTzValue(DateOrDateTz   *value,
                                              bsl::streambuf *streamBuf,
                                              int             length)
{
    char firstByte;
    if (0 != StreambufUtil::peekChar(&firstByte, streamBuf)) {
        return -1;                                                    // RETURN
    }

    DateOrDateTzEncoding::Value encoding;
    int rc = detectDateOrDateTzEncoding(&encoding, length, firstByte);

    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    switch (encoding) {
      case DateOrDateTzEncoding::e_ISO8601_DATE: {
        return getIso8601DateValue(value, streamBuf, length);         // RETURN
      } break;
      case DateOrDateTzEncoding::e_ISO8601_DATETZ: {
        return getIso8601DateTzValue(value, streamBuf, length);       // RETURN
      } break;
      case DateOrDateTzEncoding::e_COMPACT_BINARY_DATE: {
        return getCompactBinaryDateValue(value, streamBuf, length);   // RETURN
      } break;
      case DateOrDateTzEncoding::e_COMPACT_BINARY_DATETZ: {
        return getCompactBinaryDateTzValue(value, streamBuf, length);
                                                                      // RETURN
      } break;
    }

    BSLS_ASSERT_OPT(!"Reachable");
#if BSLA_UNREACHABLE_IS_ACTIVE
    BSLA_UNREACHABLE;
#else
    return -1;                                                        // RETURN
#endif
}

                         // --------------------------
                         // struct BerUtil_TimeImpUtil
                         // --------------------------

// PRIVATE CLASS METHODS

// Utilities

inline
void BerUtil_DateImpUtil::dateToDaysSinceEpoch(
                                            bsls::Types::Int64 *daysSinceEpoch,
                                            const bdlt::Date&   date)
{
    const int serialDate = bdlt::ProlepticDateImpUtil::ymdToSerial(
        date.year(), date.month(), date.day());

    *daysSinceEpoch = serialDate - k_COMPACT_BINARY_DATE_EPOCH;
}

inline
int BerUtil_DateImpUtil::daysSinceEpochToDate(
                                            bdlt::Date         *date,
                                            bsls::Types::Int64  daysSinceEpoch)
{
    const bsls::Types::Int64 serialDate =
        daysSinceEpoch + k_COMPACT_BINARY_DATE_EPOCH;

    if (!bdlt::ProlepticDateImpUtil::isValidSerial(
            static_cast<int>(serialDate))) {
        return -1;                                                    // RETURN
    }

    int year;
    int month;
    int day;
    bdlt::ProlepticDateImpUtil::serialToYmd(
        &year, &month, &day, static_cast<int>(serialDate));

    date->setYearMonthDay(year, month, day);
    return 0;
}

// 'bdlt::Time' Decoding

inline
int BerUtil_TimeImpUtil::detectTimeEncoding(TimeEncoding::Value *encoding,
                                            int                  length,
                                            unsigned char        firstByte)
{
    if (k_MAX_COMPACT_BINARY_TIME_LENGTH >= length) {
        *encoding = TimeEncoding::e_COMPACT_BINARY_TIME;
        return 0;                                                     // RETURN
    }

    BSLS_ASSERT_SAFE(k_MAX_COMPACT_BINARY_TIME_LENGTH < length);

    if (DateAndTimeHeaderUtil::isReserved(firstByte)) {
        return -1;                                                    // RETURN
    }

    if (DateAndTimeHeaderUtil::isExtendedBinary(firstByte)) {
        *encoding = TimeEncoding::e_EXTENDED_BINARY_TIME;
        return 0;                                                     // RETURN
    }

    *encoding = TimeEncoding::e_ISO8601_TIME;
    return 0;
}

inline
int BerUtil_TimeImpUtil::getIso8601TimeValue(bdlt::Time     *value,
                                             bsl::streambuf *streamBuf,
                                             int             length)
{
    return Iso8601Util::getTimeValue(value, streamBuf, length);
}

// 'bdlt::Time' Encoding

inline
BerUtil_TimeEncoding::Value BerUtil_TimeImpUtil::selectTimeEncoding(
                                              const bdlt::Time&        value,
                                              const BerEncoderOptions *options)
{
    if (ExtendedBinaryEncodingUtil::useExtendedBinaryEncoding(value,
                                                              options)) {
        return TimeEncoding::e_EXTENDED_BINARY_TIME;                  // RETURN
    }

    if (ExtendedBinaryEncodingUtil::useBinaryEncoding(options)) {
        return TimeEncoding::e_COMPACT_BINARY_TIME;                   // RETURN
    }

    return TimeEncoding::e_ISO8601_TIME;                              // RETURN
}

inline
int BerUtil_TimeImpUtil::putIso8601TimeValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::Time&        value,
                                            const BerEncoderOptions *options)
{
    return Iso8601Util::putTimeValue(streamBuf, value, options);
}

// 'bdlt::Time' Decoding

inline
int BerUtil_TimeImpUtil::detectTimeTzEncoding(TimeTzEncoding::Value *encoding,
                                              int                    length,
                                              unsigned char          firstByte)
{
    BSLMF_ASSERT(k_MAX_COMPACT_BINARY_TIME_LENGTH <
                 k_MIN_COMPACT_BINARY_TIMETZ_LENGTH);

    BSLMF_ASSERT(k_MIN_COMPACT_BINARY_TIMETZ_LENGTH <=
                 k_MAX_COMPACT_BINARY_TIMETZ_LENGTH);

    if (k_MAX_COMPACT_BINARY_TIME_LENGTH >= length) {
        *encoding = TimeTzEncoding::e_COMPACT_BINARY_TIME;
        return 0;                                                     // RETURN
    }

    if (k_MAX_COMPACT_BINARY_TIMETZ_LENGTH >= length) {
        *encoding = TimeTzEncoding::e_COMPACT_BINARY_TIMETZ;
        return 0;                                                     // RETURN
    }

    BSLS_ASSERT(k_MAX_COMPACT_BINARY_TIMETZ_LENGTH < length);

    if (DateAndTimeHeaderUtil::isReserved(firstByte)) {
        return -1;                                                    // RETURN
    }

    if (DateAndTimeHeaderUtil::isExtendedBinary(firstByte)) {
        *encoding = TimeTzEncoding::e_EXTENDED_BINARY_TIMETZ;
        return 0;                                                     // RETURN
    }

    *encoding = TimeTzEncoding::e_ISO8601_TIMETZ;
    return 0;
}

inline
int BerUtil_TimeImpUtil::getIso8601TimeTzValue(bdlt::TimeTz   *value,
                                               bsl::streambuf *streamBuf,
                                               int             length)
{
    return Iso8601Util::getTimeTzValue(value, streamBuf, length);
}

// 'bdlt::TimeTz' Encoding

inline
BerUtil_TimeTzEncoding::Value BerUtil_TimeImpUtil::selectTimeTzEncoding(
                                              const bdlt::TimeTz&      value,
                                              const BerEncoderOptions *options)
{
    if (ExtendedBinaryEncodingUtil::useExtendedBinaryEncoding(value,
                                                              options)) {
        return TimeTzEncoding::e_EXTENDED_BINARY_TIMETZ;              // RETURN
    }

    if (ExtendedBinaryEncodingUtil::useBinaryEncoding(options) &&
        (0 == value.offset())) {
        return TimeTzEncoding::e_COMPACT_BINARY_TIME;
    }

    if (ExtendedBinaryEncodingUtil::useBinaryEncoding(options)) {
        return TimeTzEncoding::e_COMPACT_BINARY_TIMETZ;
    }

    return TimeTzEncoding::e_ISO8601_TIMETZ;
}

inline
int BerUtil_TimeImpUtil::putIso8601TimeTzValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::TimeTz&      value,
                                            const BerEncoderOptions *options)
{
    return Iso8601Util::putTimeTzValue(streamBuf, value, options);
}

// Variant Decoding

inline
int BerUtil_TimeImpUtil::detectTimeOrTimeTzEncoding(
                                        TimeOrTimeTzEncoding::Value *encoding,
                                        int                          length,
                                        unsigned char                firstByte)
{
    if (k_MAX_COMPACT_BINARY_TIME_LENGTH >= length) {
        *encoding = TimeOrTimeTzEncoding::e_COMPACT_BINARY_TIME;
        return 0;                                                     // RETURN
    }

    if (k_MAX_COMPACT_BINARY_TIMETZ_LENGTH >= length) {
        *encoding = TimeOrTimeTzEncoding::e_COMPACT_BINARY_TIMETZ;
        return 0;                                                     // RETURN
    }

    if (DateAndTimeHeaderUtil::isReserved(firstByte)) {
        return -1;                                                    // RETURN
    }

    if (DateAndTimeHeaderUtil::isExtendedBinaryWithoutTimezone(firstByte)) {
        *encoding = TimeOrTimeTzEncoding::e_EXTENDED_BINARY_TIME;
        return 0;                                                     // RETURN
    }

    if (DateAndTimeHeaderUtil::isExtendedBinaryWithTimezone(firstByte)) {
        *encoding = TimeOrTimeTzEncoding::e_EXTENDED_BINARY_TIMETZ;
        return 0;                                                     // RETURN
    }

    if (k_MAX_ISO8601_TIME_LENGTH >= length) {
        *encoding = TimeOrTimeTzEncoding::e_ISO8601_TIME;
        return 0;                                                     // RETURN
    }

    *encoding = TimeOrTimeTzEncoding::e_ISO8601_TIMETZ;
    return 0;
}

inline
int BerUtil_TimeImpUtil::getIso8601TimeValue(TimeOrTimeTz   *value,
                                             bsl::streambuf *streamBuf,
                                             int             length)
{
    value->createInPlace<bdlt::Time>();
    return getIso8601TimeValue(&value->the<bdlt::Time>(), streamBuf, length);
}

inline
int BerUtil_TimeImpUtil::getIso8601TimeTzValue(TimeOrTimeTz   *value,
                                               bsl::streambuf *streamBuf,
                                               int             length)
{
    value->createInPlace<bdlt::TimeTz>();
    return getIso8601TimeTzValue(
        &value->the<bdlt::TimeTz>(), streamBuf, length);
}

inline
int BerUtil_TimeImpUtil::getCompactBinaryTimeValue(TimeOrTimeTz   *value,
                                                   bsl::streambuf *streamBuf,
                                                   int             length)
{
    value->createInPlace<bdlt::Time>();
    return getCompactBinaryTimeValue(
        &value->the<bdlt::Time>(), streamBuf, length);
}

inline
int BerUtil_TimeImpUtil::getCompactBinaryTimeTzValue(TimeOrTimeTz   *value,
                                                     bsl::streambuf *streamBuf,
                                                     int             length)
{
    value->createInPlace<bdlt::TimeTz>();
    return getCompactBinaryTimeTzValue(
        &value->the<bdlt::TimeTz>(), streamBuf, length);
}

inline
int BerUtil_TimeImpUtil::getExtendedBinaryTimeValue(TimeOrTimeTz   *value,
                                                    bsl::streambuf *streamBuf,
                                                    int             length)
{
    value->createInPlace<bdlt::Time>();
    return getExtendedBinaryTimeValue(
        &value->the<bdlt::Time>(), streamBuf, length);
}

inline
int BerUtil_TimeImpUtil::getExtendedBinaryTimeTzValue(
                                                     TimeOrTimeTz   *value,
                                                     bsl::streambuf *streamBuf,
                                                     int             length)
{
    value->createInPlace<bdlt::TimeTz>();
    return getExtendedBinaryTimeTzValue(
        &value->the<bdlt::TimeTz>(), streamBuf, length);
}

// CLASS METHODS

// Utilities

inline
void BerUtil_TimeImpUtil::timeToMillisecondsSinceMidnight(
                                  int               *millisecondsSinceMidnight,
                                  const bdlt::Time&  time)
{
    const bdlt::Time defaultTime;
    *millisecondsSinceMidnight =
        static_cast<int>((time - defaultTime).totalMilliseconds());
}

inline
void BerUtil_TimeImpUtil::timeToMicrosecondsSinceMidnight(
                                 bsls::Types::Int64 *millisecondsSinceMidnight,
                                 const bdlt::Time&   time)
{
    typedef bdlt::TimeUnitRatio Ratio;
    typedef bsls::Types::Int64  Int64;

    *millisecondsSinceMidnight =
        static_cast<Int64>(time.hour()) * Ratio::k_MICROSECONDS_PER_HOUR +
        static_cast<Int64>(time.minute()) * Ratio::k_MICROSECONDS_PER_MINUTE +
        static_cast<Int64>(time.second()) * Ratio::k_MICROSECONDS_PER_SECOND +
        static_cast<Int64>(time.millisecond()) *
            Ratio::k_MICROSECONDS_PER_MILLISECOND +
        static_cast<Int64>(time.microsecond());
}

inline
int BerUtil_TimeImpUtil::millisecondsSinceMidnightToTime(
                                         bdlt::Time *time,
                                         int         millisecondsSinceMidnight)
{
    typedef bdlt::TimeUnitRatio Ratio;

    static const int k_MIN_NUM_MILLISECONDS = 0;
    static const int k_MAX_NUM_MILLISECONDS =
        24 * Ratio::k_MILLISECONDS_PER_HOUR;

    if (k_MIN_NUM_MILLISECONDS > millisecondsSinceMidnight) {
        return -1;                                                    // RETURN
    }

    if (k_MAX_NUM_MILLISECONDS < millisecondsSinceMidnight) {
        return -1;                                                    // RETURN
    }

    const int serialTime = millisecondsSinceMidnight;

    const int hour = serialTime / Ratio::k_MILLISECONDS_PER_HOUR_32;

    const int minute =
        (serialTime - hour * Ratio::k_MILLISECONDS_PER_HOUR_32) /
        Ratio::k_MILLISECONDS_PER_MINUTE_32;

    const int second = (serialTime - hour * Ratio::k_MILLISECONDS_PER_HOUR_32 -
                        minute * Ratio::k_MILLISECONDS_PER_MINUTE_32) /
                       Ratio::k_MILLISECONDS_PER_SECOND_32;

    const int millisecond =
        (serialTime - hour * Ratio::k_MILLISECONDS_PER_HOUR_32 -
         minute * Ratio::k_MILLISECONDS_PER_MINUTE_32 -
         second * Ratio::k_MILLISECONDS_PER_SECOND_32);

    time->setTime(hour, minute, second, millisecond);
    return 0;
}

inline
int BerUtil_TimeImpUtil::microsecondsSinceMidnightToTime(
                                 bdlt::Time         *time,
                                 bsls::Types::Int64  microsecondsSinceMidnight)
{
    typedef bdlt::TimeUnitRatio Ratio;

    static const bsls::Types::Int64 k_MIN_NUM_MICROSECONDS = 0;
    static const bsls::Types::Int64 k_MAX_NUM_MICROSECONDS =
        24 * Ratio::k_MICROSECONDS_PER_HOUR;

    if (k_MIN_NUM_MICROSECONDS > microsecondsSinceMidnight) {
        return -1;                                                    // RETURN
    }

    if (k_MAX_NUM_MICROSECONDS < microsecondsSinceMidnight) {
        return -1;                                                    // RETURN
    }

    const bsls::Types::Int64 serialTime = microsecondsSinceMidnight;

    const int hour =
        static_cast<int>(serialTime / Ratio::k_MICROSECONDS_PER_HOUR);

    const int minute =
        static_cast<int>((serialTime - hour * Ratio::k_MICROSECONDS_PER_HOUR) /
                         Ratio::k_MICROSECONDS_PER_MINUTE);

    const int second =
        static_cast<int>((serialTime - hour * Ratio::k_MICROSECONDS_PER_HOUR -
                          minute * Ratio::k_MICROSECONDS_PER_MINUTE) /
                         Ratio::k_MICROSECONDS_PER_SECOND);

    const int millisecond =
        static_cast<int>((serialTime - hour * Ratio::k_MICROSECONDS_PER_HOUR -
                          minute * Ratio::k_MICROSECONDS_PER_MINUTE -
                          second * Ratio::k_MICROSECONDS_PER_SECOND) /
                         Ratio::k_MICROSECONDS_PER_MILLISECOND);

    const int microsecond = static_cast<int>(
        (serialTime - hour * Ratio::k_MICROSECONDS_PER_HOUR -
         minute * Ratio::k_MICROSECONDS_PER_MINUTE -
         second * Ratio::k_MICROSECONDS_PER_SECOND -
         millisecond * Ratio::k_MICROSECONDS_PER_MILLISECOND));

    time->setTime(hour, minute, second, millisecond, microsecond);
    return 0;
}

// 'bdlt::Time' Decoding

inline
int BerUtil_TimeImpUtil::getTimeValue(bdlt::Time     *value,
                                      bsl::streambuf *streamBuf,
                                      int             length)
{
    char firstByte;
    if (0 != StreambufUtil::peekChar(&firstByte, streamBuf)) {
        return -1;                                                    // RETURN
    }

    TimeEncoding::Value encoding;
    int rc = detectTimeEncoding(&encoding, length, firstByte);

    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    switch (encoding) {
      case TimeEncoding::e_ISO8601_TIME: {
        return getIso8601TimeValue(value, streamBuf, length);         // RETURN
      } break;
      case TimeEncoding::e_COMPACT_BINARY_TIME: {
        return getCompactBinaryTimeValue(value, streamBuf, length);   // RETURN
      } break;
      case TimeEncoding::e_EXTENDED_BINARY_TIME: {
        return getExtendedBinaryTimeValue(value, streamBuf, length);  // RETURN
      } break;
    }

    BSLS_ASSERT_OPT(!"Reachable");
#if BSLA_UNREACHABLE_IS_ACTIVE
    BSLA_UNREACHABLE;
#else
    return -1;                                                        // RETURN
#endif
}

// 'bdlt::Time' Encoding

inline
int BerUtil_TimeImpUtil::putTimeValue(bsl::streambuf          *streamBuf,
                                      const bdlt::Time&        value,
                                      const BerEncoderOptions *options)
{
    switch (selectTimeEncoding(value, options)) {
      case TimeEncoding::e_ISO8601_TIME: {
        return putIso8601TimeValue(streamBuf, value, options);        // RETURN
      } break;
      case TimeEncoding::e_COMPACT_BINARY_TIME: {
        return putCompactBinaryTimeValue(streamBuf, value, options);
                                                                      // RETURN
      } break;
      case TimeEncoding::e_EXTENDED_BINARY_TIME: {
        return putExtendedBinaryTimeValue(streamBuf, value, options);
                                                                      // RETURN
      } break;
    }

    BSLS_ASSERT_OPT(!"Reachable");
#if BSLA_UNREACHABLE_IS_ACTIVE
    BSLA_UNREACHABLE;
#else
    return -1;                                                        // RETURN
#endif
}

// 'bdlt::TimeTz' Decoding

inline
int BerUtil_TimeImpUtil::getTimeTzValue(bdlt::TimeTz   *value,
                                        bsl::streambuf *streamBuf,
                                        int             length)
{
    char firstByte;
    if (0 != StreambufUtil::peekChar(&firstByte, streamBuf)) {
        return -1;                                                    // RETURN
    }

    TimeTzEncoding::Value encoding;
    int rc = detectTimeTzEncoding(&encoding, length, firstByte);

    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    switch (encoding) {
      case TimeTzEncoding::e_ISO8601_TIMETZ: {
        return getIso8601TimeTzValue(value, streamBuf, length);       // RETURN
      } break;
      case TimeTzEncoding::e_COMPACT_BINARY_TIME: {
        return getCompactBinaryTimeValue(value, streamBuf, length);   // RETURN
      } break;
      case TimeTzEncoding::e_COMPACT_BINARY_TIMETZ: {
        return getCompactBinaryTimeTzValue(value, streamBuf, length);
                                                                      // RETURN
      } break;
      case TimeTzEncoding::e_EXTENDED_BINARY_TIMETZ: {
        return getExtendedBinaryTimeTzValue(value, streamBuf, length);
                                                                      // RETURN
      } break;
    }

    BSLS_ASSERT_OPT(!"Reachable");
#if BSLA_UNREACHABLE_IS_ACTIVE
    BSLA_UNREACHABLE;
#else
    return -1;                                                        // RETURN
#endif
}

// 'bdlt::TimeTz' Encoding

inline
int BerUtil_TimeImpUtil::putTimeTzValue(bsl::streambuf          *streamBuf,
                                        const bdlt::TimeTz&      value,
                                        const BerEncoderOptions *options)
{
    switch (selectTimeTzEncoding(value, options)) {
      case TimeTzEncoding::e_ISO8601_TIMETZ: {
        return putIso8601TimeTzValue(streamBuf, value, options);      // RETURN
      } break;
      case TimeTzEncoding::e_COMPACT_BINARY_TIME: {
        return putCompactBinaryTimeValue(streamBuf, value, options);
                                                                      // RETURN
      } break;
      case TimeTzEncoding::e_COMPACT_BINARY_TIMETZ: {
        return putCompactBinaryTimeTzValue(streamBuf, value, options);
                                                                      // RETURN
      } break;
      case TimeTzEncoding::e_EXTENDED_BINARY_TIMETZ: {
        return putExtendedBinaryTimeTzValue(streamBuf, value, options);
                                                                      // RETURN
      } break;
    }

    BSLS_ASSERT_OPT(!"Reachable");
#if BSLA_UNREACHABLE_IS_ACTIVE
    BSLA_UNREACHABLE;
#else
    return -1;                                                        // RETURN
#endif
}

// Variant Decoding

inline
int BerUtil_TimeImpUtil::getTimeOrTimeTzValue(TimeOrTimeTz   *value,
                                              bsl::streambuf *streamBuf,
                                              int             length)
{
    char firstByte;
    if (0 != StreambufUtil::peekChar(&firstByte, streamBuf)) {
        return -1;                                                    // RETURN
    }

    TimeOrTimeTzEncoding::Value encoding;
    int rc = detectTimeOrTimeTzEncoding(&encoding, length, firstByte);

    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    switch (encoding) {
      case TimeOrTimeTzEncoding::e_ISO8601_TIME: {
        return getIso8601TimeValue(value, streamBuf, length);         // RETURN
      } break;
      case TimeOrTimeTzEncoding::e_ISO8601_TIMETZ: {
        return getIso8601TimeTzValue(value, streamBuf, length);       // RETURN
      } break;
      case TimeOrTimeTzEncoding::e_COMPACT_BINARY_TIME: {
        return getCompactBinaryTimeValue(value, streamBuf, length);   // RETURN
      } break;
      case TimeOrTimeTzEncoding::e_COMPACT_BINARY_TIMETZ: {
        return getCompactBinaryTimeTzValue(value, streamBuf, length);
                                                                      // RETURN
      } break;
      case TimeOrTimeTzEncoding::e_EXTENDED_BINARY_TIME: {
        return getExtendedBinaryTimeValue(value, streamBuf, length);  // RETURN
      } break;
      case TimeOrTimeTzEncoding::e_EXTENDED_BINARY_TIMETZ: {
        return getExtendedBinaryTimeTzValue(value, streamBuf, length);
                                                                      // RETURN
      } break;
    }

    BSLS_ASSERT_OPT(!"Reachable");
#if BSLA_UNREACHABLE_IS_ACTIVE
    BSLA_UNREACHABLE;
#else
    return -1;                                                        // RETURN
#endif
}

                       // ------------------------------
                       // struct BerUtil_DatetimeImpUtil
                       // ------------------------------

// PRIVATE CLASS METHODS

// 'bdlt::Datetime' Decoding

inline
int BerUtil_DatetimeImpUtil::detectDatetimeEncoding(
                                            DatetimeEncoding::Value *encoding,
                                            int                      length,
                                            unsigned char            firstByte)
{
    if (k_MAX_COMPACT_BINARY_DATETIME_LENGTH >= length) {
        *encoding = DatetimeEncoding::e_COMPACT_BINARY_DATETIME;
        return 0;                                                     // RETURN
    }

    if (k_MAX_COMPACT_BINARY_DATETIMETZ_LENGTH >= length) {
        *encoding = DatetimeEncoding::e_COMPACT_BINARY_DATETIMETZ;
        return 0;                                                     // RETURN
    }

    if (DateAndTimeHeaderUtil::isReserved(firstByte)) {
        return -1;                                                    // RETURN
    }

    if (DateAndTimeHeaderUtil::isExtendedBinary(firstByte)) {
        *encoding = DatetimeEncoding::e_EXTENDED_BINARY_DATETIME;
        return 0;                                                     // RETURN
    }

    *encoding = DatetimeEncoding::e_ISO8601_DATETIME;
    return 0;
}

// 'bdlt::Datetime' Encoding

inline
BerUtil_DatetimeEncoding::Value
BerUtil_DatetimeImpUtil::selectDatetimeEncoding(
                                 bsls::Types::Int64      *serialDatetime,
                                 int                     *serialDatetimeLength,
                                 const bdlt::Datetime&    value,
                                 const BerEncoderOptions *options)
{
    if (ExtendedBinaryEncodingUtil::useExtendedBinaryEncoding(value,
                                                              options)) {
        return DatetimeEncoding::e_EXTENDED_BINARY_DATETIME;          // RETURN
    }

    if (ExtendedBinaryEncodingUtil::useBinaryEncoding(options)) {
        bsls::Types::Int64 serialDatetimeValue;
        datetimeToMillisecondsSinceEpoch(&serialDatetimeValue, value);

        const int serialDatetimeLengthValue =
            IntegerUtil::getNumOctetsToStream(serialDatetimeValue);

        if (k_MIN_COMPACT_BINARY_DATETIMETZ_LENGTH <=
            serialDatetimeLengthValue) {
            *serialDatetime       = serialDatetimeValue;
            *serialDatetimeLength = serialDatetimeLengthValue;
            return DatetimeEncoding::e_COMPACT_BINARY_DATETIMETZ;     // RETURN
        }

        *serialDatetime       = serialDatetimeValue;
        *serialDatetimeLength = serialDatetimeLengthValue;
        return DatetimeEncoding::e_COMPACT_BINARY_DATETIME;           // RETURN
    }

    return DatetimeEncoding::e_ISO8601_DATETIME;
}

// 'bdlt::DatetimeTz' Decoding

inline
int BerUtil_DatetimeImpUtil::detectDatetimeTzEncoding(
                                          DatetimeTzEncoding::Value *encoding,
                                          int                        length,
                                          unsigned char              firstByte)
{
    if (k_MAX_COMPACT_BINARY_DATETIME_LENGTH >= length) {
        *encoding = DatetimeTzEncoding::e_COMPACT_BINARY_DATETIME;
        return 0;                                                     // RETURN
    }

    if (k_MAX_COMPACT_BINARY_DATETIMETZ_LENGTH >= length) {
        *encoding = DatetimeTzEncoding::e_COMPACT_BINARY_DATETIMETZ;
        return 0;                                                     // RETURN
    }

    if (DateAndTimeHeaderUtil::isReserved(firstByte)) {
        return -1;                                                    // RETURN
    }

    if (DateAndTimeHeaderUtil::isExtendedBinary(firstByte)) {
        *encoding = DatetimeTzEncoding::e_EXTENDED_BINARY_DATETIMETZ;
        return 0;                                                     // RETURN
    }

    *encoding = DatetimeTzEncoding::e_ISO8601_DATETIMETZ;
    return 0;
}

// 'bdlt::DatetimeTz' Encoding

inline
BerUtil_DatetimeTzEncoding::Value
BerUtil_DatetimeImpUtil::selectDatetimeTzEncoding(
                                       bsls::Types::Int64      *serialDatetime,
                                       int                     *length,
                                       const bdlt::DatetimeTz&  value,
                                       const BerEncoderOptions *options)
{
    if (ExtendedBinaryEncodingUtil::useExtendedBinaryEncoding(value,
                                                              options)) {
        return DatetimeTzEncoding::e_EXTENDED_BINARY_DATETIMETZ;      // RETURN
    }

    if (ExtendedBinaryEncodingUtil::useBinaryEncoding(options)) {
        bsls::Types::Int64 serialDatetimeValue;
        datetimeToMillisecondsSinceEpoch(&serialDatetimeValue,
                                         value.localDatetime());

        const int lengthValue =
            IntegerUtil::getNumOctetsToStream(serialDatetimeValue);

        if (0 == value.offset() &&
            k_MIN_COMPACT_BINARY_DATETIMETZ_LENGTH > lengthValue) {
            *serialDatetime = serialDatetimeValue;
            *length         = lengthValue;
            return DatetimeTzEncoding::e_COMPACT_BINARY_DATETIME;     // RETURN
        }

        *serialDatetime = serialDatetimeValue;
        *length         = lengthValue;
        return DatetimeTzEncoding::e_COMPACT_BINARY_DATETIMETZ;       // RETURN
    }

    return DatetimeTzEncoding::e_ISO8601_DATETIMETZ;
}

// Variant Decoding

inline
int BerUtil_DatetimeImpUtil::detectDatetimeOrDatetimeTzEncoding(
                                DatetimeOrDatetimeTzEncoding::Value *encoding,
                                int                                  length,
                                unsigned char                        firstByte)
{
    BSLS_ASSERT(0 < length);

    if (k_MAX_COMPACT_BINARY_DATETIME_LENGTH >= length) {
        *encoding = DatetimeOrDatetimeTzEncoding::e_COMPACT_BINARY_DATETIME;
        return 0;                                                     // RETURN
    }

    if (k_MAX_COMPACT_BINARY_DATETIMETZ_LENGTH >= length) {
        *encoding = DatetimeOrDatetimeTzEncoding::e_COMPACT_BINARY_DATETIMETZ;
        return 0;                                                     // RETURN
    }

    if (DateAndTimeHeaderUtil::isReserved(firstByte)) {
        return -1;                                                    // RETURN
    }

    if (DateAndTimeHeaderUtil::isExtendedBinaryWithoutTimezone(firstByte)) {
        *encoding = DatetimeOrDatetimeTzEncoding::e_EXTENDED_BINARY_DATETIME;
        return 0;                                                       // RETURN
    }

    if (DateAndTimeHeaderUtil::isExtendedBinaryWithTimezone(firstByte)) {
        *encoding = DatetimeOrDatetimeTzEncoding::e_EXTENDED_BINARY_DATETIMETZ;
        return 0;                                                     // RETURN
    }

    if (k_MAX_ISO8601_DATETIME_LENGTH >= length) {
        *encoding = DatetimeOrDatetimeTzEncoding::e_ISO8601_DATETIME;
        return 0;                                                     // RETURN
    }

    *encoding = DatetimeOrDatetimeTzEncoding::e_ISO8601_DATETIMETZ;
    return 0;
}

inline
int BerUtil_DatetimeImpUtil::getIso8601DatetimeValue(
                                               DatetimeOrDatetimeTz *value,
                                               bsl::streambuf       *streamBuf,
                                               int                   length)
{
    value->createInPlace<bdlt::Datetime>();
    return getIso8601DatetimeValue(
        &value->the<bdlt::Datetime>(), streamBuf, length);
}

inline
int BerUtil_DatetimeImpUtil::getIso8601DatetimeTzValue(
                                               DatetimeOrDatetimeTz *value,
                                               bsl::streambuf       *streamBuf,
                                               int                   length)
{
    value->createInPlace<bdlt::DatetimeTz>();
    return getIso8601DatetimeTzValue(
        &value->the<bdlt::DatetimeTz>(), streamBuf, length);
}

inline
int BerUtil_DatetimeImpUtil::getCompactBinaryDatetimeValue(
                                               DatetimeOrDatetimeTz *value,
                                               bsl::streambuf       *streamBuf,
                                               int                   length)
{
    value->createInPlace<bdlt::Datetime>();
    return getCompactBinaryDatetimeValue(
        &value->the<bdlt::Datetime>(), streamBuf, length);
}

inline
int BerUtil_DatetimeImpUtil::getCompactBinaryDatetimeTzValue(
                                               DatetimeOrDatetimeTz *value,
                                               bsl::streambuf       *streamBuf,
                                               int                   length)
{
    value->createInPlace<bdlt::DatetimeTz>();
    return getCompactBinaryDatetimeTzValue(
        &value->the<bdlt::DatetimeTz>(), streamBuf, length);
}

inline
int BerUtil_DatetimeImpUtil::getExtendedBinaryDatetimeValue(
                                               DatetimeOrDatetimeTz *value,
                                               bsl::streambuf       *streamBuf,
                                               int                   length)
{
    value->createInPlace<bdlt::Datetime>();
    return getExtendedBinaryDatetimeValue(
        &value->the<bdlt::Datetime>(), streamBuf, length);
}

inline
int BerUtil_DatetimeImpUtil::getExtendedBinaryDatetimeTzValue(
                                               DatetimeOrDatetimeTz *value,
                                               bsl::streambuf       *streamBuf,
                                               int                   length)
{
    value->createInPlace<bdlt::DatetimeTz>();
    return getExtendedBinaryDatetimeTzValue(
        &value->the<bdlt::DatetimeTz>(), streamBuf, length);
}

// CLASS METHODS

// 'bdlt::Datetime' Encoding

inline
int BerUtil_DatetimeImpUtil::putDatetimeValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::Datetime&    value,
                                            const BerEncoderOptions *options)
{
    const bdlt::Time& time = value.time();
    bdlt::Date        date = value.date();

    if (0 != date.addDaysIfValid(0) ||
        !bdlt::Time::isValid(
            time.hour(), time.minute(), time.second(), time.millisecond())) {
        return -1;                                                    // RETURN
    }

    bsls::Types::Int64 serialDatetime;
    int                length;

    switch (selectDatetimeEncoding(&serialDatetime, &length, value, options)) {
      case DatetimeEncoding::e_ISO8601_DATETIME: {
        return putIso8601DatetimeValue(streamBuf, value, options);    // RETURN
      } break;
      case DatetimeEncoding::e_COMPACT_BINARY_DATETIME: {
        return putCompactBinaryDatetimeValue(
            streamBuf, serialDatetime, length, options);              // RETURN
      } break;
      case DatetimeEncoding::e_COMPACT_BINARY_DATETIMETZ: {
        return putCompactBinaryDatetimeTzValue(
            streamBuf, serialDatetime, length, options);              // RETURN
      } break;
      case DatetimeEncoding::e_EXTENDED_BINARY_DATETIME: {
        return putExtendedBinaryDatetimeValue(streamBuf, value, options);
                                                                      // RETURN
      } break;
    }

    BSLS_ASSERT_OPT(!"Reachable");
#if BSLA_UNREACHABLE_IS_ACTIVE
    BSLA_UNREACHABLE;
#else
    return -1;                                                        // RETURN
#endif
}

// 'bdlt::Datetime' Decoding

inline
int BerUtil_DatetimeImpUtil::getDatetimeValue(bdlt::Datetime *value,
                                              bsl::streambuf *streamBuf,
                                              int             length)
{
    char firstByte;
    if (0 != StreambufUtil::peekChar(&firstByte, streamBuf)) {
        return -1;                                                    // RETURN
    }

    DatetimeEncoding::Value encoding;
    int rc = detectDatetimeEncoding(&encoding, length, firstByte);

    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    switch (encoding) {
      case DatetimeEncoding::e_ISO8601_DATETIME: {
        return getIso8601DatetimeValue(value, streamBuf, length);     // RETURN
      } break;
      case DatetimeEncoding::e_COMPACT_BINARY_DATETIME: {
        return getCompactBinaryDatetimeValue(value, streamBuf, length);
                                                                      // RETURN
      } break;
      case DatetimeEncoding::e_COMPACT_BINARY_DATETIMETZ: {
        return getCompactBinaryDatetimeTzValue(value, streamBuf, length);
                                                                      // RETURN
      } break;
      case DatetimeEncoding::e_EXTENDED_BINARY_DATETIME: {
        return getExtendedBinaryDatetimeValue(value, streamBuf, length);
                                                                      // RETURN
      } break;
    }

    BSLS_ASSERT_OPT(!"Reachable");
#if BSLA_UNREACHABLE_IS_ACTIVE
    BSLA_UNREACHABLE;
#else
    return -1;                                                        // RETURN
#endif
}

// 'bdlt::DatetimeTz' Encoding

inline
int BerUtil_DatetimeImpUtil::putDatetimeTzValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::DatetimeTz&  value,
                                            const BerEncoderOptions *options)
{
    // Applications can create invalid 'bdlt::DatetimeTz' objects in optimized
    // build modes.  As this function assumes that 'value' is valid, it is
    // possible to encode an invalid 'bdlt::DatetimeTz' without returning an
    // error.  Decoding the corresponding output can result in hard-to-trace
    // decoding errors.  So to identify such errors early, we return an error
    // if 'value' is not valid.

    const bdlt::DateTz& dateTz = value.dateTz();
    const bdlt::TimeTz& timeTz = value.timeTz();
    if (0 != dateTz.localDate().addDaysIfValid(0) ||
        !bdlt::DateTz::isValid(dateTz.localDate(), dateTz.offset()) ||
        !bdlt::TimeTz::isValid(timeTz.utcTime(), timeTz.offset())) {
        return -1;                                                    // RETURN
    }

    bsls::Types::Int64 serialDatetime;
    int                serialDatetimeLength;

    switch (selectDatetimeTzEncoding(
        &serialDatetime, &serialDatetimeLength, value, options)) {
      case DatetimeTzEncoding::e_ISO8601_DATETIMETZ: {
        return putIso8601DatetimeTzValue(streamBuf, value, options);
                                                                      // RETURN
      } break;
      case DatetimeTzEncoding::e_COMPACT_BINARY_DATETIME: {
        return putCompactBinaryDatetimeValue(
            streamBuf, serialDatetime, serialDatetimeLength, options);
                                                                      // RETURN
      } break;
      case DatetimeTzEncoding::e_COMPACT_BINARY_DATETIMETZ: {
        return putCompactBinaryDatetimeTzValue(streamBuf,
                                               value.offset(),
                                               serialDatetime,
                                               serialDatetimeLength,
                                               options);
                                                                      // RETURN
      } break;
      case DatetimeTzEncoding::e_EXTENDED_BINARY_DATETIMETZ: {
        return putExtendedBinaryDatetimeTzValue(streamBuf, value, options);
                                                                      // RETURN
      } break;
    }

    BSLS_ASSERT_OPT(!"Reachable");
#if BSLA_UNREACHABLE_IS_ACTIVE
    BSLA_UNREACHABLE;
#else
    return -1;                                                        // RETURN
#endif
}

// 'bdlt::DatetimeTz' Decoding

inline
int BerUtil_DatetimeImpUtil::getDatetimeTzValue(bdlt::DatetimeTz *value,
                                                bsl::streambuf   *streamBuf,
                                                int               length)
{
    char firstByte;
    if (0 != StreambufUtil::peekChar(&firstByte, streamBuf)) {
        return -1;                                                    // RETURN
    }

    DatetimeTzEncoding::Value encoding;
    int rc = detectDatetimeTzEncoding(&encoding, length, firstByte);

    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    switch (encoding) {
      case DatetimeTzEncoding::e_ISO8601_DATETIMETZ: {
        return getIso8601DatetimeTzValue(value, streamBuf, length);   // RETURN
      } break;
      case DatetimeTzEncoding::e_COMPACT_BINARY_DATETIME: {
        return getCompactBinaryDatetimeValue(value, streamBuf, length);
                                                                      // RETURN
      } break;
      case DatetimeTzEncoding::e_COMPACT_BINARY_DATETIMETZ: {
        return getCompactBinaryDatetimeTzValue(value, streamBuf, length);
                                                                      // RETURN
      } break;
      case DatetimeTzEncoding::e_EXTENDED_BINARY_DATETIMETZ: {
        return getExtendedBinaryDatetimeTzValue(value, streamBuf, length);
                                                                      // RETURN
      } break;
    }

    BSLS_ASSERT_OPT(!"Reachable");
#if BSLA_UNREACHABLE_IS_ACTIVE
    BSLA_UNREACHABLE;
#else
    return -1;                                                        // RETURN
#endif
}

// Variant Decoding

inline
int BerUtil_DatetimeImpUtil::getDatetimeOrDatetimeTzValue(
                                               DatetimeOrDatetimeTz *value,
                                               bsl::streambuf       *streamBuf,
                                               int                   length)
{
    char firstByte;
    if (0 != StreambufUtil::peekChar(&firstByte, streamBuf)) {
        return -1;                                                    // RETURN
    }

    DatetimeOrDatetimeTzEncoding::Value encoding;
    int rc = detectDatetimeOrDatetimeTzEncoding(&encoding, length, firstByte);

    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    switch (encoding) {
      case DatetimeOrDatetimeTzEncoding::e_ISO8601_DATETIME: {
        return getIso8601DatetimeValue(value, streamBuf, length);
      } break;
      case DatetimeOrDatetimeTzEncoding::e_ISO8601_DATETIMETZ: {
        return getIso8601DatetimeTzValue(value, streamBuf, length);
      } break;
      case DatetimeOrDatetimeTzEncoding::e_COMPACT_BINARY_DATETIME: {
        return getCompactBinaryDatetimeValue(value, streamBuf, length);
      } break;
      case DatetimeOrDatetimeTzEncoding::e_COMPACT_BINARY_DATETIMETZ: {
        return getCompactBinaryDatetimeTzValue(value, streamBuf, length);
      } break;
      case DatetimeOrDatetimeTzEncoding::e_EXTENDED_BINARY_DATETIME: {
        return getExtendedBinaryDatetimeValue(value, streamBuf, length);
      } break;
      case DatetimeOrDatetimeTzEncoding::e_EXTENDED_BINARY_DATETIMETZ: {
        return getExtendedBinaryDatetimeTzValue(value, streamBuf, length);
      } break;
    }

    BSLS_ASSERT_OPT(!"Reachable");
#if BSLA_UNREACHABLE_IS_ACTIVE
    BSLA_UNREACHABLE;
#else
    return -1;                                                        // RETURN
#endif
}

                       // ------------------------------
                       // struct BerUtil_GetValueImpUtil
                       // ------------------------------

// CLASS METHODS
template <typename TYPE>
int BerUtil_GetValueImpUtil::getValue(TYPE                     *value,
                                      bsl::streambuf           *streamBuf,
                                      int                       length,
                                      const BerDecoderOptions&)
{
    return IntegerUtil::getIntegerValue(value, streamBuf, length);
}

inline
int BerUtil_GetValueImpUtil::getValue(bool                     *value,
                                      bsl::streambuf           *streamBuf,
                                      int                       length,
                                      const BerDecoderOptions&)
{
    return BooleanUtil::getBoolValue(value, streamBuf, length);
}

inline
int BerUtil_GetValueImpUtil::getValue(char                     *value,
                                      bsl::streambuf           *streamBuf,
                                      int                       length,
                                      const BerDecoderOptions&)
{
    return CharacterUtil::getCharValue(value, streamBuf, length);
}

inline
int BerUtil_GetValueImpUtil::getValue(unsigned char            *value,
                                      bsl::streambuf           *streamBuf,
                                      int                       length,
                                      const BerDecoderOptions&)
{
    return CharacterUtil::getUnsignedCharValue(value, streamBuf, length);
}

inline
int BerUtil_GetValueImpUtil::getValue(signed char              *value,
                                      bsl::streambuf           *streamBuf,
                                      int                       length,
                                      const BerDecoderOptions&)
{
    return CharacterUtil::getSignedCharValue(value, streamBuf, length);
}

inline
int BerUtil_GetValueImpUtil::getValue(float                    *value,
                                      bsl::streambuf           *streamBuf,
                                      int                       length,
                                      const BerDecoderOptions&)
{
    return FloatingPointUtil::getFloatValue(value, streamBuf, length);
}

inline
int BerUtil_GetValueImpUtil::getValue(double                   *value,
                                      bsl::streambuf           *streamBuf,
                                      int                       length,
                                      const BerDecoderOptions&)
{
    return FloatingPointUtil::getDoubleValue(value, streamBuf, length);
}

inline
int BerUtil_GetValueImpUtil::getValue(bdldfp::Decimal64        *value,
                                      bsl::streambuf           *streamBuf,
                                      int                       length,
                                      const BerDecoderOptions&)
{
    return FloatingPointUtil::getDecimal64Value(value, streamBuf, length);
}

inline
int BerUtil_GetValueImpUtil::getValue(bsl::string              *value,
                                      bsl::streambuf           *streamBuf,
                                      int                       length,
                                      const BerDecoderOptions&  options)
{
    return StringUtil::getStringValue(value, streamBuf, length, options);
}

inline
int BerUtil_GetValueImpUtil::getValue(bdlt::Date               *value,
                                      bsl::streambuf           *streamBuf,
                                      int                       length,
                                      const BerDecoderOptions&)
{
    return DateUtil::getDateValue(value, streamBuf, length);
}

inline
int BerUtil_GetValueImpUtil::getValue(bdlt::DateTz             *value,
                                      bsl::streambuf           *streamBuf,
                                      int                       length,
                                      const BerDecoderOptions&)
{
    return DateUtil::getDateTzValue(value, streamBuf, length);
}

inline
int BerUtil_GetValueImpUtil::getValue(DateOrDateTz             *value,
                                      bsl::streambuf           *streamBuf,
                                      int                       length,
                                      const BerDecoderOptions&)
{
    return DateUtil::getDateOrDateTzValue(value, streamBuf, length);
}

inline
int BerUtil_GetValueImpUtil::getValue(bdlt::Datetime           *value,
                                      bsl::streambuf           *streamBuf,
                                      int                       length,
                                      const BerDecoderOptions&)
{
    return DatetimeUtil::getDatetimeValue(value, streamBuf, length);
}

inline
int BerUtil_GetValueImpUtil::getValue(bdlt::DatetimeTz         *value,
                                      bsl::streambuf           *streamBuf,
                                      int                       length,
                                      const BerDecoderOptions&)
{
    return DatetimeUtil::getDatetimeTzValue(value, streamBuf, length);
}

inline
int BerUtil_GetValueImpUtil::getValue(DatetimeOrDatetimeTz     *value,
                                      bsl::streambuf           *streamBuf,
                                      int                       length,
                                      const BerDecoderOptions&)
{
    return DatetimeUtil::getDatetimeOrDatetimeTzValue(
        value, streamBuf, length);
}

inline
int BerUtil_GetValueImpUtil::getValue(bdlt::Time               *value,
                                      bsl::streambuf           *streamBuf,
                                      int                       length,
                                      const BerDecoderOptions&)
{
    return TimeUtil::getTimeValue(value, streamBuf, length);
}

inline
int BerUtil_GetValueImpUtil::getValue(bdlt::TimeTz             *value,
                                      bsl::streambuf           *streamBuf,
                                      int                       length,
                                      const BerDecoderOptions&)
{
    return TimeUtil::getTimeTzValue(value, streamBuf, length);
}

inline
int BerUtil_GetValueImpUtil::getValue(TimeOrTimeTz             *value,
                                      bsl::streambuf           *streamBuf,
                                      int                       length,
                                      const BerDecoderOptions&)
{
    return TimeUtil::getTimeOrTimeTzValue(value, streamBuf, length);
}

                       // ------------------------------
                       // struct BerUtil_PutValueImpUtil
                       // ------------------------------

// CLASS METHODS
template <class TYPE>
int BerUtil_PutValueImpUtil::putValue(bsl::streambuf          *streamBuf,
                                      const TYPE&              value,
                                      const BerEncoderOptions *)
{
    return IntegerUtil::putIntegerValue(streamBuf, value);
}

inline
int BerUtil_PutValueImpUtil::putValue(bsl::streambuf          *streamBuf,
                                      bool                     value,
                                      const BerEncoderOptions *)
{
    return BooleanUtil::putBoolValue(streamBuf, value);
}

inline
int BerUtil_PutValueImpUtil::putValue(bsl::streambuf          *streamBuf,
                                      char                     value,
                                      const BerEncoderOptions *)
{
    return CharacterUtil::putCharValue(streamBuf, value);
}

inline
int BerUtil_PutValueImpUtil::putValue(bsl::streambuf          *streamBuf,
                                      unsigned char            value,
                                      const BerEncoderOptions *)
{
    return CharacterUtil::putUnsignedCharValue(streamBuf, value);
}

inline
int BerUtil_PutValueImpUtil::putValue(bsl::streambuf          *streamBuf,
                                      signed char              value,
                                      const BerEncoderOptions *)
{
    return CharacterUtil::putSignedCharValue(streamBuf, value);
}

inline
int BerUtil_PutValueImpUtil::putValue(bsl::streambuf          *streamBuf,
                                      float                    value,
                                      const BerEncoderOptions *)
{
    return FloatingPointUtil::putFloatValue(streamBuf, value);
}

inline
int BerUtil_PutValueImpUtil::putValue(bsl::streambuf          *streamBuf,
                                      double                   value,
                                      const BerEncoderOptions *)
{
    return FloatingPointUtil::putDoubleValue(streamBuf, value);
}

inline
int BerUtil_PutValueImpUtil::putValue(bsl::streambuf          *streamBuf,
                                      bdldfp::Decimal64        value,
                                      const BerEncoderOptions *)
{
    return FloatingPointUtil::putDecimal64Value(streamBuf, value);
}

inline
int BerUtil_PutValueImpUtil::putValue(bsl::streambuf          *streamBuf,
                                      const bsl::string&       value,
                                      const BerEncoderOptions *)
{
    return StringUtil::putStringValue(streamBuf, value);
}

inline
int BerUtil_PutValueImpUtil::putValue(bsl::streambuf           *streamBuf,
                                      const bslstl::StringRef&  value,
                                      const BerEncoderOptions  *)
{
    return StringUtil::putStringRefValue(streamBuf, value);
}

inline
int BerUtil_PutValueImpUtil::putValue(bsl::streambuf          *streamBuf,
                                      const bdlt::Date&        value,
                                      const BerEncoderOptions *options)
{
    return DateUtil::putDateValue(streamBuf, value, options);
}

inline
int BerUtil_PutValueImpUtil::putValue(bsl::streambuf          *streamBuf,
                                      const bdlt::DateTz&      value,
                                      const BerEncoderOptions *options)
{
    return DateUtil::putDateTzValue(streamBuf, value, options);
}

inline
int BerUtil_PutValueImpUtil::putValue(bsl::streambuf          *streamBuf,
                                      const bdlt::Datetime&    value,
                                      const BerEncoderOptions *options)
{
    return DatetimeUtil::putDatetimeValue(streamBuf, value, options);
}

inline
int BerUtil_PutValueImpUtil::putValue(bsl::streambuf          *streamBuf,
                                      const bdlt::DatetimeTz&  value,
                                      const BerEncoderOptions *options)
{
    return DatetimeUtil::putDatetimeTzValue(streamBuf, value, options);
}

inline
int BerUtil_PutValueImpUtil::putValue(bsl::streambuf          *streamBuf,
                                      const bdlt::Time&        value,
                                      const BerEncoderOptions *options)
{
    return TimeUtil::putTimeValue(streamBuf, value, options);
}

inline
int BerUtil_PutValueImpUtil::putValue(bsl::streambuf          *streamBuf,
                                      const bdlt::TimeTz&      value,
                                      const BerEncoderOptions *options)
{
    return TimeUtil::putTimeTzValue(streamBuf, value, options);
}

                             // ------------------
                             // struct BerUtil_Imp
                             // ------------------

// CLASS METHODS
inline
int BerUtil_Imp::putStringValue(bsl::streambuf *streamBuf,
                                const char     *string,
                                int             stringLength)
{
    typedef BerUtil_StringImpUtil StringUtil;
    return StringUtil::putRawStringValue(streamBuf, string, stringLength);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
