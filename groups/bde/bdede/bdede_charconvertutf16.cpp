// bdede_charconvertutf16.cpp                                         -*-C++-*-
#include <bdede_charconvertutf16.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslmf_assert.h>   // 'BSLMF_ASSERT'

#include <bsl_climits.h>    // 'CHAR_BIT'

///IMPLEMENTATION NOTES
///--------------------
// This UTF-8 documentation was copied verbatim from RFC 3629.  The original
// version was downloaded from:
//..
//     http://tools.ietf.org/html/rfc3629
//..
////////////////////////// BEGIN VERBATIM RFC TEXT //////////////////////////
// UTF-8 encodes UCS characters as a varying number of octets, where the
// number of octets, and the value of each, depend on the integer value
// assigned to the character in ISO/IEC 10646 (the character number,
// a.k.a. code position, code point or Unicode scalar value).  This
// encoding form has the following characteristics (all values are in
// hexadecimal):
//
// o  Character numbers from U+0000 to U+007F (US-ASCII repertoire)
//    correspond to octets 00 to 7F (7 bit US-ASCII values).  A direct
//    consequence is that a plain ASCII string is also a valid UTF-8
//    string.
//
// o  US-ASCII octet values do not appear otherwise in a UTF-8 encoded
//    character stream.  This provides compatibility with file systems
//    or other software (e.g., the printf() function in C libraries)
//    that parse based on US-ASCII values but are transparent to other
//    values.
//
// o  Round-trip conversion is easy between UTF-8 and other encoding
//    forms.
//
// o  The first octet of a multi-octet sequence indicates the number of
//    octets in the sequence.
//
// o  The octet values C0, C1, F5 to FF never appear.
//
// o  Character boundaries are easily found from anywhere in an octet
//    stream.
//
// o  The byte-value lexicographic sorting order of UTF-8 strings is the
//    same as if ordered by character numbers.  Of course this is of
//    limited interest since a sort order based on character numbers is
//    almost never culturally valid.
//
// o  The Boyer-Moore fast search algorithm can be used with UTF-8 data.
//
// o  UTF-8 strings can be fairly reliably recognized as such by a
//    simple algorithm, i.e., the probability that a string of
//    characters in any other encoding appears as valid UTF-8 is low,
//    diminishing with increasing string length.
//
// UTF-8 was devised in September 1992 by Ken Thompson, guided by design
// criteria specified by Rob Pike, with the objective of defining a UCS
// transformation format usable in the Plan9 operating system in a non-
// disruptive manner.  Thompson's design was stewarded through
// standardization by the X/Open Joint Internationalization Group XOJIG
// (see [FSS_UTF]), bearing the names FSS-UTF (variant FSS/UTF), UTF-2
// and finally UTF-8 along the way.
//
// ...
//
// UTF-8 is defined by the Unicode Standard [UNICODE].  Descriptions and
// formulae can also be found in Annex D of ISO/IEC 10646-1 [ISO.10646]
//
// In UTF-8, characters from the U+0000..U+10FFFF range (the UTF-16
// accessible range) are encoded using sequences of 1 to 4 octets.  The
// only octet of a "sequence" of one has the higher-order bit set to 0,
// the remaining 7 bits being used to encode the character number.  In a
// sequence of n octets, n>1, the initial octet has the n higher-order
// bits set to 1, followed by a bit set to 0.  The remaining bit(s) of
// that octet contain bits from the number of the character to be
// encoded.  The following octet(s) all have the higher-order bit set to
// 1 and the following bit set to 0, leaving 6 bits in each to contain
// bits from the character to be encoded.
//
// The table below summarizes the format of these different octet types.
// The letter x indicates bits available for encoding bits of the
// character number.
//
// Char number range   |        UTF-8 octet sequence
//    (hexadecimal)    |              (binary)
//  -------------------+---------------------------------------------
//  000 0000-0000 007F | 0xxxxxxx
//  000 0080-0000 07FF | 110xxxxx 10xxxxxx
//  000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
//  001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
/////////////////////////// END VERBATIM RFC TEXT ///////////////////////////
// This UTF-16 documentation was copied verbatim from RFC 2781.  The original
// version was downloaded from:
//..
//     http://tools.ietf.org/html/rfc2781
//..
////////////////////////// BEGIN VERBATIM RFC TEXT //////////////////////////
// 2. UTF-16 definition
//
//   UTF-16 is described in the Unicode Standard, version 3.0 [UNICODE].  The
//   definitive reference is Annex Q of ISO/IEC 10646-1 [ISO-10646].  The rest
//   of this section summarizes the definition is simple terms.
//
//   In ISO 10646, each character is assigned a number, which Unicode calls the
//   Unicode scalar value.  This number is the same as the UCS-4 value of the
//   character, and this document will refer to it as the "character value" for
//   brevity.  In the UTF-16 encoding, characters are represented using either
//   one or two unsigned 16-bit integers, depending on the character value.
//   Serialization of these integers for transmission as a byte stream is
//   discussed in Section 3.
//
//   The rules for how characters are encoded in UTF-16 are:
//
//   -  Characters with values less than 0x10000 are represented as a single
//      16-bit integer with a value equal to that of the character number.
//
//   -  Characters with values between 0x10000 and 0x10FFFF are represented by
//      a 16-bit integer with a value between 0xD800 and 0xDBFF (within the
//      so-called high-half zone or high surrogate area) followed by a 16-bit
//      integer with a value between 0xDC00 and 0xDFFF (within the so-called
//      low-half zone or low surrogate area).
//
//   -  Characters with values greater than 0x10FFFF cannot be encoded in
//      UTF-16.
//
//   Note: Values between 0xD800 and 0xDFFF are specifically reserved for use
//   with UTF-16, and don't have any characters assigned to them.
//
// 2.1 Encoding UTF-16
//
//   Encoding of a single character from an ISO 10646 character value to UTF-16
//   proceeds as follows.  Let U be the character number, no greater than
//   0x10FFFF.
//
//   1) If U < 0x10000, encode U as a 16-bit unsigned integer and terminate.
//
//   2) Let U' = U - 0x10000. Because U is less than or equal to 0x10FFFF,
//      U' must be less than or equal to 0xFFFFF.  That is, U' can be
//      represented in 20 bits.
//
//   3) Initialize two 16-bit unsigned integers, W1 and W2, to 0xD800 and
//      0xDC00, respectively.  These integers each have 10 bits free to encode
//      the character value, for a total of 20 bits.
//
//   4) Assign the 10 high-order bits of the 20-bit U' to the 10 low-order
//      bits of W1 and the 10 low-order bits of U' to the 10 low-order bits of
//      W2. Terminate.
//
//   Graphically, steps 2 through 4 look like:
//   U' = yyyyyyyyyyxxxxxxxxxx
//   W1 = 110110yyyyyyyyyy
//   W2 = 110111xxxxxxxxxx
//
// 2.2 Decoding UTF-16
//
//   Decoding of a single character from UTF-16 to an ISO 10646 character value
//   proceeds as follows.  Let W1 be the next 16-bit integer in the sequence of
//   integers representing the text.  Let W2 be the (eventual) next integer
//   following W1.
//
//   1) If W1 < 0xD800 or W1 > 0xDFFF, the character value U is the value of
//      W1. Terminate.
//
//   2) Determine if W1 is between 0xD800 and 0xDBFF.  If not, the sequence is
//      in error and no valid character can be obtained using W1.  Terminate.
//
//   3) If there is no W2 (that is, the sequence ends with W1), or if W2 is not
//      between 0xDC00 and 0xDFFF, the sequence is in error.  Terminate.
//
//   4) Construct a 20-bit unsigned integer U', taking the 10 low-order bits of
//      W1 as its 10 high-order bits and the 10 low-order bits of W2 as its 10
//      low-order bits.
//
//   5) Add 0x10000 to U' to obtain the character value U.  Terminate.
//
//   Note that steps 2 and 3 indicate errors.  Error recovery is not specified
//   by this document.  When terminating with an error in steps 2 and 3, it may
//   be wise to set U to the value of W1 to help the caller diagnose the error
//   and not lose information.  Also note that a string decoding algorithm, as
//   opposed to the single-character decoding described above, need not
//   terminate upon detection of an error, if proper error reporting and/or
//   recovery is provided.
//
/////////////////////////// END VERBATIM RFC TEXT ///////////////////////////

namespace {

// TYPES
// These return code values need to be kept synchronized with the header
// documentation.

enum {
    BDEDE_SUCCESS                 = 0,
    BDEDE_INVALID_INPUT_CHARACTER = 1,
    BDEDE_OUTPUT_BUFFER_TOO_SMALL = 2
};

typedef unsigned int Iso10646Char;
    // For storing uncompressed Unicode character (21 bit, 17 plane).

// Portability check -- data type sizes.
BSLMF_ASSERT(8 == CHAR_BIT);
BSLMF_ASSERT(2 == sizeof(unsigned short));
BSLMF_ASSERT(4 == sizeof(Iso10646Char));

// LOCAL HELPER STRUCT
struct Utf8 {
    // Utf8 is an 'expert' struct, embodying the rules for converting between
    // 21-bit (17-plane) Unicode and utf8

    // TYPES
    typedef unsigned char OctetType;
        // Treating the octets as a signed (or default signed) char has so
        // many pitfalls in widening that the code would become unreadable.
        // This typedef (used especially for pointer punning) gives a shorter
        // way to write all the conversions necessary.

    enum Utf8Bits {
        // Masks and shifts used to assemble and dismantle utf8 encodings.

        ONE_OCT_CONT_WID   = 7,               // Content in a one-octet coding
        ONE_OCTET_MASK     = 0xff & (~0 << ONE_OCT_CONT_WID),
        ONE_OCTET_TAG      = 0xff & 0,        // Compare this to mask'd bits

        CONTINUE_CONT_WID  = 6,               // Content in a continuation
                                              //                      octet
        CONTINUE_MASK      = 0xff & (~0 << CONTINUE_CONT_WID),
        CONTINUE_TAG       = ONE_OCTET_MASK,  // Compare this to mask'd bits

        TWO_OCT_CONT_WID   = 5,               // Content in a two-octet header
        TWO_OCTET_MASK     = 0xff & (~0 << TWO_OCT_CONT_WID),
        TWO_OCTET_TAG      = CONTINUE_MASK,   // Compare this to mask'd bits

        THREE_OCT_CONT_WID = 4,               // Content in a 3-octet header
        THREE_OCTET_MASK   = 0xff & (~0 << THREE_OCT_CONT_WID),
        THREE_OCTET_TAG    = TWO_OCTET_MASK,  // Compare this to mask'd bits

        FOUR_OCT_CONT_WID  = 3,               // Content in a four-octet header
        FOUR_OCTET_MASK    = 0xff & (~0 << FOUR_OCT_CONT_WID),
        FOUR_OCTET_TAG     = THREE_OCTET_MASK // Compare this to mask'd bits
    };

    // CLASS METHODS

    // Part 1: Determine how to decode a utf8 character.
    //   The functions 'isSingleOctet',
    //                 'isContinuation',
    //                 'isTwoOctetHeader',
    //                 'isThreeOctetHeader' and
    //                 'isFourOctetHeader' classify the argument octet
    //   according to what part it plays in the encoding.  Each function
    //   returns 'true' if the condition named is true and 'false' otherwise.
    //   Note that they are mutually exclusive: an octet will satisfy at most
    //   one of them.  Note also that none of these functions will return
    //   'true' for the octet '0xff' nor for the header octets for five-, six-,
    //   and seven-octet encodings.  (The higher-order headers are part of
    //   the original utf-8 scheme, but not utf-8 as it is used for encoding
    //   iso10646 characters.)

    static inline
    bool isSingleOctet(OctetType oct)
    {
        return ! (oct & ONE_OCTET_MASK);
    }

    static inline
    bool isContinuation(OctetType oct)
    {
        return (oct & CONTINUE_MASK) == CONTINUE_TAG;
    }

    static inline
    bool isTwoOctetHeader(OctetType oct)
    {
        return (oct & TWO_OCTET_MASK) == TWO_OCTET_TAG;
    }

    static inline
    bool isThreeOctetHeader(OctetType oct)
    {
        return (oct & THREE_OCTET_MASK) == THREE_OCTET_TAG;
    }

    static inline
    bool isFourOctetHeader(OctetType oct)
    {
        return (oct & FOUR_OCTET_MASK) == FOUR_OCTET_TAG;
    }

    // Part 2: the means to decode from utf8 to iso10646
    //    The functions 'decodeTwoOctets',
    //                  'decodeThreeOctets' and
    //                  'decodeFourOctets' convert a multi-octet utf8 character
    //    (addressed by the second argument) into the corresponding iso10646
    //    character.  The result is invalid unless the input is encoded
    //    according to the utf8 rules for that particular encoding.  Note
    //    that they do not check for misuse of the encoding (e.g., use of a
    //    four-byte coding for a character that could be encoded in fewer
    //    bytes); they implement just this one step of the decoding process.
    //    Note also that a single-octet character can be copied directly; no
    //    function is provided to cover this trivial computation.

    static inline
    void decodeTwoOctets(Iso10646Char *isoBuf, const OctetType *octBuf)
    {
        *isoBuf =  (octBuf[1] & ~CONTINUE_MASK)
                | ((octBuf[0] & ~TWO_OCTET_MASK) << CONTINUE_CONT_WID);
    }

    static inline
    void decodeThreeOctets(Iso10646Char *isoBuf, const OctetType *octBuf)
    {
        *isoBuf =  (octBuf[2] & ~CONTINUE_MASK)
                | ((octBuf[1] & ~CONTINUE_MASK) << CONTINUE_CONT_WID)
                | ((octBuf[0] & ~THREE_OCTET_MASK) << 2 * CONTINUE_CONT_WID);
    }

    static inline
    void decodeFourOctets(Iso10646Char *isoBuf, const OctetType *octBuf)
    {
        *isoBuf =  (octBuf[3] & ~CONTINUE_MASK)
                | ((octBuf[2] & ~CONTINUE_MASK) << CONTINUE_CONT_WID)
                | ((octBuf[1] & ~CONTINUE_MASK) << 2 * CONTINUE_CONT_WID)
                | ((octBuf[0] & ~FOUR_OCTET_MASK) << 3 * CONTINUE_CONT_WID);
    }

    static inline
    int lookaheadContinuations(const OctetType *octbuf, int n)
        // Return the number of continuation octets beginning at '*octBuf',
        // up to but not greater than 'n'.  Note that a null octet is not a
        // continuation and is taken to end the scan.
    {
        for (int i = 0; ; ++i) {
            if (i >= n || ! isContinuation(octbuf[i])) {
                return i;
            }
        }
    }

    // Part 3: Determine how to encode an iso10646 character as utf8.

    //    The functions 'fitsInSingleOctet',
    //                  'fitsInTwoOctets', and
    //                  'fitsInThreeOctets' each return 'true' if the iso10646
    //    character passed in the argument can be encoded in the specified
    //    number of octets, and 'false' otherwise.  Note that these functions
    //    do NOT check whether the specified encoding is the correct (i.e.,
    //    minimal) encoding or whether the character is a valid iso10646
    //    character (i.e., that it does not lie in the d800-to-dfff reserved
    //    range).

    static inline
    bool fitsInSingleOctet(Iso10646Char uc)
    {
        return 0 == (uc & ~Iso10646Char(0) << ONE_OCT_CONT_WID);
    }

    static inline
    bool fitsInTwoOctets(Iso10646Char uc)
    {
        return 0 == (uc & ~Iso10646Char(0) << (TWO_OCT_CONT_WID +
                                                          CONTINUE_CONT_WID));
    }

    static inline
    bool fitsInThreeOctets(Iso10646Char uc)
    {
        return 0 == (uc & ~Iso10646Char(0) << (THREE_OCT_CONT_WID +
                                                      2 * CONTINUE_CONT_WID));
    }

    // Part 4: The means to encode iso10646 into utf8

    //     The functions 'encodeTwoOctets',
    //                   'encodeThreeOctets' and
    //                   'encodeFourOctets' encode the iso10646 character
    //     passed as their second argument into the specified number of octets
    //     in the byte array addressed by their first argument.  The result
    //     is invalid if there is not enough room in the array, or if the
    //     specified encoding is not the correct coding for the character
    //     given.  Note that the single-byte encoding is accomplished by a
    //     direct copy, for which no function is provided here.

    static inline
    void encodeTwoOctets(char *octBuf, Iso10646Char isoBuf)
    {
        octBuf[1] = CONTINUE_TAG | (isoBuf & ~CONTINUE_MASK);
        octBuf[0] = TWO_OCTET_TAG |
                    ((isoBuf >> CONTINUE_CONT_WID) & ~TWO_OCTET_MASK);
    }

    static inline
    void encodeThreeOctets(char *octBuf, Iso10646Char isoBuf)
    {
        octBuf[2] = CONTINUE_TAG | (isoBuf & ~CONTINUE_MASK);
        octBuf[1] = CONTINUE_TAG |
                    ((isoBuf >> CONTINUE_CONT_WID) & ~CONTINUE_MASK);
        octBuf[0] = THREE_OCTET_TAG |
                    ((isoBuf >> 2 * CONTINUE_CONT_WID) & ~THREE_OCTET_MASK);
    }

    static inline
    void encodeFourOctets(char *octBuf, Iso10646Char isoBuf)
    {
        octBuf[3] = CONTINUE_TAG | (isoBuf & ~CONTINUE_MASK);
        octBuf[2] = CONTINUE_TAG |
                    ((isoBuf >> CONTINUE_CONT_WID) & ~CONTINUE_MASK);
        octBuf[1] = CONTINUE_TAG |
                    ((isoBuf >> 2 * CONTINUE_CONT_WID) & ~CONTINUE_MASK);
        octBuf[0] = FOUR_OCTET_TAG |
                    ((isoBuf >> 3 * CONTINUE_CONT_WID) & ~FOUR_OCTET_MASK);
    }
};

// LOCAL HELPER STRUCT
struct Utf16 {
    // utf16 embodies the rules for converting between 21-bit (17 plane)
    // Unicode and utf16

    // TYPES
    typedef unsigned short Utf16Char;
    typedef Utf8::OctetType OctetType;

    enum { RESERVED_MASK    = 0xF800,  // Common part of first/second header
           RESERVED_TAG     = 0xD800,  // Magic # in RESERVED_MASK
           HEADER_MASK      = 0xFC00,  // Includes 1st/2nd discriminant bit
           FIRST_TAG        = 0xD800,  // Header of first two-word part
           SECOND_TAG       = 0xDC00,  // Header of second two-word part
           CONTENT_CONT_WID = 10,      // Ten content bits
           CONTENT_MASK     = ~Iso10646Char(RESERVED_MASK),
           RESERVE_OFFSET   = 0x10000, // Subtract this before coding two-part
                                       // characters, per rfc 2871.
           UPPER_LIMIT      = 0x110000 // Valid characters lie below this, and
                                       // outside the reserved range.
    };

    // CLASS METHODS

    // Part 1: Determine how to decode an iso10646 character into utf16.

    //    The 'isSingleUtf8' static method returns true if the utf16 word
    //    passed as an argument will decode into a single utf8 character, false
    //    otherwise.
    //    The functions 'isSingleWord',
    //                  'isFirstWord' and
    //                   'isSecondWord' each return 'true' if the utf16 word
    //    passed as an argument is a single-word encoding, the first word of a
    //    two-word encoding, or the second word of a two-word encoding,
    //    respectively, and 'false' otherwise.

    static inline
    bool isSingleUtf8(Utf16Char uc)
    {
        return ! (uc & ~Utf16Char(0) << Utf8::ONE_OCT_CONT_WID);
    }

    static inline
    bool isSingleWord(Utf16Char uc)
    {
        return (uc & RESERVED_MASK) != RESERVED_TAG;
    }

    static inline
    bool isFirstWord(Utf16Char uc)
    {
        return (uc & HEADER_MASK) == FIRST_TAG;
    }

    static inline
    bool isSecondWord(Utf16Char uc)
    {
        return (uc & HEADER_MASK) == SECOND_TAG;
    }

    // Part 2: The means to decode from uf16

    //     The 'getUtf8Value' static method returns the utf8 character
    //     corresponding to the single-word utf16 character passed in as its
    //     argument.  The result is invalid unless the word passed in is a
    //     valid single-word utf16 character that can be encoded as a
    //     single-byte utf8 character.
    //
    //     The functions 'decodeSingleWord' and
    //                   'decodeTwoWords' store, in the iso10646 character
    //     buffer addressed by their first argument, the iso10646 character
    //     encoded by the single-word or two-word utf16 character in the
    //     buffer addressed by their second argument.  The result is invalid
    //     unless there is enough space to store one iso10646 character at the
    //     location addressed by the first argument, and unless the one or two
    //     words addressed by the second argument are a valid iso10646
    //     character in the specied encoding.

    static inline
    OctetType getUtf8Value(Utf16Char uc)
    {
        return uc;
    }

    static inline
    void decodeSingleWord(Iso10646Char *isoBuf, const Utf16Char *u16Buf)
    {
        *isoBuf = *u16Buf;
    }

    static inline
    void decodeTwoWords(Iso10646Char *isoBuf, const Utf16Char *u16Buf)
    {
        *isoBuf = RESERVE_OFFSET
                + ((u16Buf[0] & ~HEADER_MASK) << CONTENT_CONT_WID
                 | (u16Buf[1] & ~HEADER_MASK));
    }

    // Part 3: Determine how to encode a utf16 character.

    //     The functions 'fitsInOneWord',
    //                   'isValidOneWord' and
    //                   'isValidTwoWords' return 'true' if the iso10646
    //     character passed as their argument satisfies the specified
    //     condition, and 'false' otherwise.  'isValidTwoWords' yields an
    //     invalid result for a character that can be encoded in a single
    //     word.

    static inline
    bool fitsInOneWord(Iso10646Char uc)
    {
        return uc < RESERVE_OFFSET;
    }

    static inline
    bool isValidOneWord(Iso10646Char uc)
    {
        return (uc & RESERVED_MASK) != RESERVED_TAG;
    }

    static inline
    bool isValidTwoWords(Iso10646Char uc)
    {
        return uc < UPPER_LIMIT;
    }

    // Part 4: the means to encode a utf16 character

    //    The functions 'encodeSingleWord' and
    //                  'encodeTwoWords' encode the iso10646 character passed
    //    as their second argument into the utf16 word buffer addressed by
    //    their first argument.  The result is invalid if the buffer addressed
    //    by the first argument is not large enough, or if the character passed
    //    as the second argument is not a valid iso10646 character.

    static inline
    void encodeSingleWord(Utf16Char* u16Buf, Iso10646Char uc)
    {
        *u16Buf = uc;
    }

    static inline
    void encodeTwoWords(Utf16Char *u16Buf, Iso10646Char uc)
    {
        Iso10646Char v = uc - RESERVE_OFFSET;
        u16Buf[0] = FIRST_TAG  | (v >> CONTENT_CONT_WID);
        u16Buf[1] = SECOND_TAG |
                               (v & ~(~Iso10646Char(0) << CONTENT_CONT_WID));
    }
};

}  // close unnamed namespace

namespace BloombergLP {

                        // -----------------------------
                        // struct bdede_CharConvertUtf16
                        // -----------------------------

// CLASS METHODS
int bdede_CharConvertUtf16::utf8ToUtf16(unsigned short *dstBuffer,
                                        bsl::size_t       dstCapacity,
                                        const char       *srcString,
                                        bsl::size_t      *numCharsWritten,
                                        bsl::size_t      *numWordsWritten,
                                        unsigned short    errorCharacter)
{
    return utf8ToUtf16Partial(dstBuffer,
                              dstCapacity,
                              &srcString,
                              numCharsWritten,
                              numWordsWritten,
                              errorCharacter);
}

int bdede_CharConvertUtf16::utf8ToUtf16Partial(
                                             unsigned short   *dstBuffer,
                                             bsl::size_t       dstCapacity,
                                             const char      **srcStringArg,
                                             bsl::size_t      *numCharsWritten,
                                             bsl::size_t      *numWordsWritten,
                                             unsigned short    errorCharacter)
{
    // We need at least room for a null character in the output.
    if (dstCapacity < 1) {
        if (numCharsWritten) {
            *numCharsWritten = 0;
        }
        if (numWordsWritten) {
            *numWordsWritten = 0;
        }
        return BDEDE_OUTPUT_BUFFER_TOO_SMALL;
    }

    unsigned short *const dstStart = dstBuffer;
    bsl::size_t nChars = 0;
    int returnStatus = 0;

    // Working in unsigned makes bit manipulation with widening simpler;
    // changing the type here keeps the low-level routines short.

    const Utf8::OctetType *octets = static_cast<const Utf8::OctetType*>(
                                      static_cast<const void*>(*srcStringArg));
    const Utf8::OctetType *nextOctets = octets;
    for (; ; octets = nextOctets) {

        // Are we done?

        if (0 == *octets) {
            nextOctets = octets + 1;
            break;
        }

        // Checking for output space is tricky.  If we have an error case
        // and no replacement character, we may consume input octets without
        // using any space.

        // Single-octet case is simple and quick.

        if (Utf8::isSingleOctet(*octets)) {
            if (dstCapacity < 2) {
                // Are we out of output room, with only space for the null?
                returnStatus |= BDEDE_OUTPUT_BUFFER_TOO_SMALL;
                break;
            }

            Utf16::encodeSingleWord(dstBuffer, *octets);
            nextOctets = octets + 1;
            ++dstBuffer;
            --dstCapacity;
            ++nChars;
            continue;
        }

        // Two, three, or four octets needed.

        if (errorCharacter && dstCapacity < 2) {
            // If there is an error character, we'll
            // need at least one output slot.
            returnStatus |= BDEDE_OUTPUT_BUFFER_TOO_SMALL;
            break;
        }

        // The error cases have a lot of repetition.  With the optimizer on,
        // the repeated code should all get folded together.

        Iso10646Char convBuf;

        if (Utf8::isTwoOctetHeader(*octets)) {
            int nContins = Utf8::lookaheadContinuations(octets + 1, 1);
            if (nContins != 1) {
                returnStatus |= BDEDE_INVALID_INPUT_CHARACTER;
                nextOctets = octets + nContins + 1;  // Resynchronize past this
                                                     // group.
                if (errorCharacter) {
                    *dstBuffer++ = errorCharacter;
                    --dstCapacity;
                    ++nChars;
                }
                continue;
            }

            Utf8::decodeTwoOctets(&convBuf, octets);
            nextOctets = octets + 2;
            if (Utf8::fitsInSingleOctet(convBuf)) { // Miscoding!  Mischief?
                returnStatus |= BDEDE_INVALID_INPUT_CHARACTER;
                if (errorCharacter) {
                    *dstBuffer++ = errorCharacter;
                    --dstCapacity;
                    ++nChars;
                }
                continue;
            }
        }
        else if (Utf8::isThreeOctetHeader(*octets)) {
            int nContins = Utf8::lookaheadContinuations(octets + 1, 2);
            if (nContins != 2) {
                returnStatus |= BDEDE_INVALID_INPUT_CHARACTER;
                nextOctets = octets + nContins + 1;  // Resynchronize past this
                                                     // group.
                if (errorCharacter) {
                    *dstBuffer++ = errorCharacter;
                    --dstCapacity;
                    ++nChars;
                }
                continue;
            }

            Utf8::decodeThreeOctets(&convBuf, octets);
            nextOctets = octets + 3;
            if (Utf8::fitsInTwoOctets(convBuf)) { // Miscoding!  Mischief?
                returnStatus |= BDEDE_INVALID_INPUT_CHARACTER;
                if (errorCharacter) {
                    *dstBuffer++ = errorCharacter;
                    --dstCapacity;
                    ++nChars;
                }
                continue;
            }
        }
        else if (Utf8::isFourOctetHeader(*octets)) {
            int nContins = Utf8::lookaheadContinuations(octets + 1, 3);
            if (nContins != 3) {
                returnStatus |= BDEDE_INVALID_INPUT_CHARACTER;
                nextOctets = octets + nContins + 1;  // Resynchronize past this
                                                     // group.
                if (errorCharacter) {
                    *dstBuffer++ = errorCharacter;
                    --dstCapacity;
                    ++nChars;
                }
                continue;
            }

            Utf8::decodeFourOctets(&convBuf, octets);
            nextOctets = octets + 4;
            if (Utf8::fitsInThreeOctets(convBuf)) { // Miscoding!  Mischief?
                returnStatus |= BDEDE_INVALID_INPUT_CHARACTER;
                if (errorCharacter) {
                    *dstBuffer++ = errorCharacter;
                    --dstCapacity;
                    ++nChars;
                }
                continue;
            }
        }
        else {
            // Handle a five-octet character (or anything else) sanely.
            // (Continuations will have to be skipped in any case.)

            int nContins = Utf8::lookaheadContinuations(octets + 1, 4);
            nextOctets = octets + nContins + 1;
            returnStatus |= BDEDE_INVALID_INPUT_CHARACTER;
            if (errorCharacter) {
                *dstBuffer++ = errorCharacter;
                --dstCapacity;
                ++nChars;
            }
            continue;
        }

        // The utf8 character is decoded.  Re-encode it in utf16.
        // We could assume that a two- or three-octet utf8 character will have
        // to be a single-word utf16 character, and a four-octet utf8 character
        // will have to be a two-word utf16 character.  But the logic is less
        // tangled if the decode and encode, and all their cases, are kept
        // separate, especially since the validity tests must be made for
        // three- and four-octet utf8 encodings.

        if (Utf16::fitsInOneWord(convBuf)) {
            if (! Utf16::isValidOneWord(convBuf)) {
                returnStatus |= BDEDE_INVALID_INPUT_CHARACTER;
                if (errorCharacter) {
                    *dstBuffer++ = errorCharacter;
                    --dstCapacity;
                    ++nChars;
                }
                continue;
            }
            if (dstCapacity < 2) {  // Need room for at least one plus a null.
                returnStatus |= BDEDE_OUTPUT_BUFFER_TOO_SMALL;
                break;
            }
            Utf16::encodeSingleWord(dstBuffer, convBuf);
            ++dstBuffer;
            --dstCapacity;
            ++nChars;
        }
        else {
            // An invalid char can be coded in the one space we are guaranteed,
            // so test that first.

            if (! Utf16::isValidTwoWords(convBuf)) {
                returnStatus |= BDEDE_INVALID_INPUT_CHARACTER;
                if (errorCharacter) {
                    *dstBuffer++ = errorCharacter;
                    --dstCapacity;
                    ++nChars;
                }
                continue;
            }
            if (dstCapacity < 3) {  // Need room for at least two plus a null.
                returnStatus |= BDEDE_OUTPUT_BUFFER_TOO_SMALL;
                break;
            }
            Utf16::encodeTwoWords(dstBuffer, convBuf);
            dstBuffer += 2;
            dstCapacity -= 2;
            ++nChars;
        }
    }
    if (0 == (returnStatus & BDEDE_OUTPUT_BUFFER_TOO_SMALL)) {
        octets = nextOctets;
    }

    *dstBuffer++ = 0;
    ++nChars;

    if (numCharsWritten) {
        *numCharsWritten = nChars;
    }
    if (numWordsWritten) {
        *numWordsWritten = dstBuffer - dstStart;
    }

    *srcStringArg = static_cast<const char *>(
                                            static_cast<const void *>(octets));
    return returnStatus;
}

int bdede_CharConvertUtf16::utf16ToUtf8(char               *dstBuffer,
                                        bsl::size_t           dstCapacity,
                                        const unsigned short *srcString,
                                        bsl::size_t          *numCharsWritten,
                                        bsl::size_t          *numBytesWritten,
                                        char                  errorCharacter)
{
    return utf16ToUtf8Partial(dstBuffer,
                              dstCapacity,
                              &srcString,
                              numCharsWritten,
                              numBytesWritten,
                              errorCharacter);
}

int bdede_CharConvertUtf16::utf16ToUtf8Partial(
                                        char                  *dstBuffer,
                                        bsl::size_t            dstCapacity,
                                        const unsigned short **srcStringArg,
                                        bsl::size_t           *numCharsWritten,
                                        bsl::size_t           *numBytesWritten,
                                        char                   errorCharacter)
{
    const unsigned short *srcString = *srcStringArg;

    if (dstCapacity < 1) {
        if (numCharsWritten) {
            *numCharsWritten = 0;
        }
        if (numBytesWritten) {
            *numBytesWritten = 0;
        }
        return BDEDE_OUTPUT_BUFFER_TOO_SMALL;
    }

    char *const dstStart = dstBuffer;

    bsl::size_t nChars = 0;

    int returnStatus = 0;

    for (;;) {

        // Are we done?

        if (0 == *srcString) {
            ++srcString;
            break;
        }

        // We don't do the out-of-room tests until we know that we can
        // generate valid Iso10646Char characters from the utf16 string.

        // The single-byte case is the simplest.  It ought to be the fastest.

        if (Utf16::isSingleUtf8(*srcString)) {
            if (dstCapacity < 2) {
                // One for the char, one for the null.
                returnStatus |= BDEDE_OUTPUT_BUFFER_TOO_SMALL;
                break;
            }
            *dstBuffer = Utf16::getUtf8Value(*srcString);
            ++srcString;
            ++dstBuffer;
            --dstCapacity;
            ++nChars;
            continue;
        }

        Iso10646Char convBuf;

        // Is it a single-word character?

        if (Utf16::isSingleWord(*srcString)) {
            Utf16::decodeSingleWord(&convBuf, srcString);

            // One word encodes to one, two, or three utf8 octets.  We have
            // already handled the single-octet case.

            if (Utf8::fitsInTwoOctets(convBuf)) {
                // Need two spaces plus a null
                if (dstCapacity < 3) {
                    returnStatus |= BDEDE_OUTPUT_BUFFER_TOO_SMALL;
                    break;
                }
                Utf8::encodeTwoOctets(dstBuffer, convBuf);
                ++srcString;
                dstBuffer += 2;
                dstCapacity -= 2;
                ++nChars;
                continue;
            }

            // Need three spaces plus a null

            if (dstCapacity < 4) {
                returnStatus |= BDEDE_OUTPUT_BUFFER_TOO_SMALL;
                break;
            }

            Utf8::encodeThreeOctets(dstBuffer, convBuf);
            ++srcString;
            dstBuffer += 3;
            dstCapacity -= 3;
            ++nChars;
            continue;
        }

        // Not one word, must be two.

        if (! Utf16::isFirstWord(*srcString)
         || ! Utf16::isSecondWord(srcString[1])) {

            // ( ! isSecondWord() covers the case of null.  )

            returnStatus |= BDEDE_INVALID_INPUT_CHARACTER;
            ++srcString;
            if (0 != errorCharacter) {
                if (dstCapacity < 2) {
                    // One for the char, one for the null.

                    returnStatus |= BDEDE_OUTPUT_BUFFER_TOO_SMALL;
                    break;
                }
                *dstBuffer = errorCharacter;
                ++dstBuffer;
                --dstCapacity;
                ++nChars;
            }
            continue;
        }

        // Need four octets plus a null

        if (dstCapacity < 5) {
            returnStatus |= BDEDE_OUTPUT_BUFFER_TOO_SMALL;

            break;
        }

        Utf16::decodeTwoWords(&convBuf, srcString);
        Utf8::encodeFourOctets(dstBuffer, convBuf);
        srcString += 2;
        dstBuffer += 4;
        dstCapacity -= 4;
        ++nChars;
    }

    *dstBuffer++ = 0;
    ++nChars;

    if (numBytesWritten) {
        *numBytesWritten = dstBuffer - dstStart;
    }

    if (numCharsWritten) {
        *numCharsWritten = nChars;
    }

    *srcStringArg = srcString;
    return returnStatus;
}

}  // close namespace BloombergLP

// -------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE -------------------------------
