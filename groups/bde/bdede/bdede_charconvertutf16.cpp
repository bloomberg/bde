// bdede_charconvertutf16.cpp                                         -*-C++-*-
#include <bdede_charconvertutf16.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bdede_charconvertstatus.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>  // 'min'
#include <bsl_climits.h>    // 'CHAR_BIT'

///IMPLEMENTATION NOTES
///--------------------
// This UTF-8 documentation was copied verbatim from RFC 3629.  The original
// version was downloaded from:
//..
//     http://tools.ietf.org/html/rfc3629
//..
////////////////////////// BEGIN VERBATIM RFC TEXT //////////////////////////
// UTF-8 encodes UCS characters as a varying number of octets, where the number
// of octets, and the value of each, depend on the integer value assigned to
// the character in ISO/IEC 10646 (the character number, a.k.a. code position,
// code point or Unicode scalar value).  This encoding form has the following
// characteristics (all values are in hexadecimal):
//
//: o Character numbers from U+0000 to U+007F (US-ASCII repertoire) correspond
//:   to octets 00 to 7F (7 bit US-ASCII values).  A direct consequence is that
//:   a plain ASCII string is also a valid UTF-8 string.
//:
//: o US-ASCII octet values do not appear otherwise in a UTF-8 encoded
//:   character stream.  This provides compatibility with file systems or other
//:   software (e.g., the printf() function in C libraries) that parse based on
//:   US-ASCII values but are transparent to other values.
//:
//: o Round-trip conversion is easy between UTF-8 and other encoding forms.
//:
//: o The first octet of a multi-octet sequence indicates the number of octets
//:   in the sequence.
//:
//: o The octet values C0, C1, F5 to FF never appear.
//:
//: o Character boundaries are easily found from anywhere in an octet stream.
//:
//: o The byte-value lexicographic sorting order of UTF-8 strings is the same
//:   as if ordered by character numbers.  Of course this is of limited
//:   interest since a sort order based on character numbers is almost never
//:   culturally valid.
//:
//: o The Boyer-Moore fast search algorithm can be used with UTF-8 data.
//:
//: o UTF-8 strings can be fairly reliably recognized as such by a simple
//:   algorithm, i.e., the probability that a string of characters in any other
//:   encoding appears as valid UTF-8 is low, diminishing with increasing
//:   string length.
//
// UTF-8 was devised in September 1992 by Ken Thompson, guided by design
// criteria specified by Rob Pike, with the objective of defining a UCS
// transformation format usable in the Plan9 operating system in a non-
// disruptive manner.  Thompson's design was stewarded through standardization
// by the X/Open Joint Internationalization Group XOJIG (see [FSS_UTF]),
// bearing the names FSS-UTF (variant FSS/UTF), UTF-2 and finally UTF-8 along
// the way.
//
// ...
//
// UTF-8 is defined by the Unicode Standard [UNICODE].  Descriptions and
// formulae can also be found in Annex D of ISO/IEC 10646-1 [ISO.10646]
//
// In UTF-8, characters from the U+0000..U+10FFFF range (the UTF-16 accessible
// range) are encoded using sequences of 1 to 4 octets.  The only octet of a
// "sequence" of one has the higher-order bit set to 0, the remaining 7 bits
// being used to encode the character number.  In a sequence of n octets, n>1,
// the initial octet has the n higher-order bits set to 1, followed by a bit
// set to 0.  The remaining bit(s) of that octet contain bits from the number
// of the character to be encoded.  The following octet(s) all have the
// higher-order bit set to 1 and the following bit set to 0, leaving 6 bits in
// each to contain bits from the character to be encoded.
//
// The table below summarizes the format of these different octet types.  The
// letter x indicates bits available for encoding bits of the character number.
//..
// Char number range   |        UTF-8 octet sequence
//    (hexadecimal)    |              (binary)
//  -------------------+---------------------------------------------
//  000 0000-0000 007F | 0xxxxxxx
//  000 0080-0000 07FF | 110xxxxx 10xxxxxx
//  000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
//  001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
//..
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

typedef BloombergLP::bdede_CharConvertUtf16 Util;
typedef BloombergLP::bslstl::StringRef      StringRef;

enum {
    INVALID_CHARS_BIT =
                 BloombergLP::bdede_CharConvertStatus::BDEDE_INVALID_CHARS_BIT,
    OUT_OF_SPACE_BIT  =
                 BloombergLP::bdede_CharConvertStatus::BDEDE_OUT_OF_SPACE_BIT
};

typedef unsigned int Iso10646Char;
    // For storing uncompressed Unicode character (21 bit, 17 plane).

// Portability check -- data type sizes.
BSLMF_ASSERT(8 == CHAR_BIT);
BSLMF_ASSERT(2 == sizeof(unsigned short));
BSLMF_ASSERT(4 == sizeof(Iso10646Char));

struct Capacity {
    // Functor passed to 'localUtf8ToUtf16' and 'localUtf16ToUtf8' in cases
    // where we monitor capacity available in output.  Initialize in c'tor with
    // an integer 'capacity', then thereafter support operators '--', '-=', and
    // '<' for that value.

    bsl::size_t d_capacity;

    // CREATORS
    explicit
    Capacity(bsl::size_t capacity) : d_capacity(capacity) {}
        // Create a 'Capacity' object with the specified 'capacity'.

    // MANIPULATORS
    void operator--() { --d_capacity; }
        // Decrement 'd_capacity'.

    void operator-=(int delta) { d_capacity -= delta; }
        // Decrement 'd_capacity' by 'delta'.

    // ACCESSORS
    bool operator<(bsl::size_t rhs) const { return d_capacity < rhs; }
        // Return 'true' if 'd_capacity < rhs' and 'false' otherwise.
};

struct NoOpCapacity {
    // Functor passed to 'localUtf8ToUtf16' and 'localUtf16ToUtf8' in cases
    // where we don't want to monitor capacity available in output, all
    // operations on this object are to become no-ops, all if-blocks controlled
    // by '<' are 'if '<capacity> < <value>' with no 'else' and will expand to
    // no code.

    // MANIPULATORS
    void operator--() {}
        // No-op.

    void operator-=(int) {}
        // No-op.

    // ACCESSORS
    bool operator<(bsl::size_t) const { return false; }
        // Return 'false'.
};

// LOCAL HELPER STRUCT
struct Utf8 {
    // 'Utf8' is an 'expert' struct, embodying the rules for converting between
    // 21-bit (17-plane) Unicode and UTF-8.

    // TYPES
    typedef unsigned char OctetType;
        // Treating the octets as a signed (or default signed) char has so
        // many pitfalls in widening that the code would become unreadable.
        // This typedef (used especially for pointer punning) gives a shorter
        // way to write all the conversions necessary.

    enum Utf8Bits {
        // Masks and shifts used to assemble and dismantle UTF-8 encodings.

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

    class PtrBasedEnd {
        // DATA
        const OctetType * const d_end;

      public:
        // CREATORS
        explicit
        PtrBasedEnd(const char *end)
            // Create a 'PtrBasedEnd' object with the end at the specified
            // 'end'.
        : d_end(reinterpret_cast<const OctetType *>(end))
        {}

        // ACCESSORS
        bool isFinished(const OctetType *position) const
            // Return 'true' if 'position' is at the end of input and 'false'
            // otherwise.  The behavior is undefined unless
            // 'position <= d_end'.
        {
            if (position < d_end) {
                return false;                                         // RETURN
            }
            else {
                BSLS_ASSERT_SAFE(d_end == position);
                return true;                                          // RETURN
            }
        }

        const OctetType *skipContinuations(const OctetType *octets) const
            // Return a pointer to after all the consequetive continuation
            // bytes following the specified 'octets' that are prior to
            // 'd_end'.  The behavior is undefined unless 'octets <= d_end'.
        {
            // This function is not called in the critical path.  It is called
            // when it is known that there are fewer continuation octets
            // after 'octets' than were expected, at least before 'd_end'.

            BSLS_ASSERT_SAFE(d_end >= octets);

            while (octets < d_end &&
                                   (*octets & CONTINUE_MASK) == CONTINUE_TAG) {
                ++octets;
            }

            return octets;
        }

        bool verifyContinuations(const OctetType *octets, int n) const
            // Return 'true' if there are at least the specified 'n'
            // continuation bytes beginning at the specified 'octets' and prior
            // to 'd_end', and 'false' otherwise.  The behavior is undefined if
            // 'octets' is past the end.  The behavior is undefined unless
            // 'octets <= d_end'.
        {
            BSLS_ASSERT_SAFE(n >= 1);
            BSLS_ASSERT_SAFE(d_end >= octets);

            const OctetType *end = octets + n;
            if (end > d_end) {
                return false;                                         // RETURN
            }

            do {
                if ((*octets & CONTINUE_MASK) != CONTINUE_TAG) {
                    return false;                                     // RETURN
                }

                ++octets;
            } while (octets < end);

            return true;
        }
    };

    struct ZeroBasedEnd {
        // ACCESSORS
        bool isFinished(const OctetType *position) const
            // Return 'true' if 'position' is at the end of input, and 'false'
            // otherwise.
        {
            return 0 == *position;
        }

        const OctetType *skipContinuations(const OctetType *octets) const
            // Return a pointer to after all the consequetive continuation
            // bytes following the specified 'octets'.  The behavior is
            // undefined unless 'octets <= d_end'.
        {
            // This function is not called in the critical path.  It is called
            // when it is known that there are not as many continuation octets
            // after 'octets' as were expected.

            while ((*octets & CONTINUE_MASK) == CONTINUE_TAG) {
                ++octets;
            }

            return octets;
        }

        bool verifyContinuations(const OctetType *octets, int n) const
            // Return 'true' if there are at least the specified 'n'
            // continuation bytes beginning at the specified 'octets' and
            // 'false' otherwise.  The behavior is undefined unless 'n >= 1'.
        {
            BSLS_ASSERT_SAFE(n >= 1);

            const OctetType *end = octets + n;
            do {
                if ((*octets & CONTINUE_MASK) != CONTINUE_TAG) {
                    return false;                                     // RETURN
                }

                ++octets;
            } while (octets < end);

            return true;
        }
    };

    // CLASS METHODS

    // Part 1: Determine how to decode a UTF-8 character.
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

    static
    bool isSingleOctet(OctetType oct)
    {
        return 0 == (oct & ONE_OCTET_MASK);
    }


    static
    bool isTwoOctetHeader(OctetType oct)
    {
        return (oct & TWO_OCTET_MASK) == TWO_OCTET_TAG;
    }

    static
    bool isThreeOctetHeader(OctetType oct)
    {
        return (oct & THREE_OCTET_MASK) == THREE_OCTET_TAG;
    }

    static
    bool isFourOctetHeader(OctetType oct)
    {
        return (oct & FOUR_OCTET_MASK) == FOUR_OCTET_TAG;
    }

    // Part 2: the means to decode from UTF-8 to iso10646
    //: The functions
    //: o 'decodeTwoOctets',
    //: o 'decodeThreeOctets', and
    //: o 'decodeFourOctets' convert a multi-octet UTF-8 character (addressed
    //:   by the second argument) into the corresponding iso10646 character.
    //:   The result is invalid unless the input is encoded according to the
    //:   UTF-8 rules for that particular encoding.  Note that they do not
    //:   check for misuse of the encoding (e.g., use of a four-byte coding for
    //:   a character that could be encoded in fewer bytes); they implement
    //:   just this one step of the decoding process.  Note also that a
    //:   single-octet character can be copied directly; no function is
    //:   provided to cover this trivial computation.

    static
    void decodeTwoOctets(Iso10646Char *isoBuf, const OctetType *octBuf)
    {
        *isoBuf =  (octBuf[1] & ~CONTINUE_MASK)
                | ((octBuf[0] & ~TWO_OCTET_MASK) << CONTINUE_CONT_WID);
    }

    static
    void decodeThreeOctets(Iso10646Char *isoBuf, const OctetType *octBuf)
    {
        *isoBuf =  (octBuf[2] & ~CONTINUE_MASK)
                | ((octBuf[1] & ~CONTINUE_MASK) << CONTINUE_CONT_WID)
                | ((octBuf[0] & ~THREE_OCTET_MASK) << 2 * CONTINUE_CONT_WID);
    }

    static
    void decodeFourOctets(Iso10646Char *isoBuf, const OctetType *octBuf)
    {
        *isoBuf =  (octBuf[3] & ~CONTINUE_MASK)
                | ((octBuf[2] & ~CONTINUE_MASK) << CONTINUE_CONT_WID)
                | ((octBuf[1] & ~CONTINUE_MASK) << 2 * CONTINUE_CONT_WID)
                | ((octBuf[0] & ~FOUR_OCTET_MASK) << 3 * CONTINUE_CONT_WID);
    }

    // Part 3: Determine how to encode an iso10646 character as UTF-8.

    //    The functions 'fitsInSingleOctet',
    //                  'fitsInTwoOctets', and
    //                  'fitsInThreeOctets' each return 'true' if the iso10646
    //    character passed in the argument can be encoded in the specified
    //    number of octets, and 'false' otherwise.  Note that these functions
    //    do NOT check whether the specified encoding is the correct (i.e.,
    //    minimal) encoding or whether the character is a valid iso10646
    //    character (i.e., that it does not lie in the d800-to-dfff reserved
    //    range).

    static
    bool fitsInSingleOctet(Iso10646Char uc)
    {
        return 0 == (uc & ~Iso10646Char(0) << ONE_OCT_CONT_WID);
    }

    static
    bool fitsInTwoOctets(Iso10646Char uc)
    {
        return 0 == (uc & ~Iso10646Char(0) << (TWO_OCT_CONT_WID +
                                                          CONTINUE_CONT_WID));
    }

    static
    bool fitsInThreeOctets(Iso10646Char uc)
    {
        return 0 == (uc & ~Iso10646Char(0) << (THREE_OCT_CONT_WID +
                                                      2 * CONTINUE_CONT_WID));
    }

    // Part 4: The means to encode iso10646 into UTF-8

    //     The functions 'encodeTwoOctets',
    //                   'encodeThreeOctets' and
    //                   'encodeFourOctets' encode the iso10646 character
    //     passed as their second argument into the specified number of octets
    //     in the byte array addressed by their first argument.  The result
    //     is invalid if there is not enough room in the array, or if the
    //     specified encoding is not the correct coding for the character
    //     given.  Note that the single-byte encoding is accomplished by a
    //     direct copy, for which no function is provided here.

    static
    void encodeTwoOctets(char *octBuf, Iso10646Char isoBuf)
    {
        octBuf[1] = static_cast<char>(CONTINUE_TAG |
                             (isoBuf                       & ~CONTINUE_MASK));
        octBuf[0] = static_cast<char>(TWO_OCTET_TAG |
                            ((isoBuf >> CONTINUE_CONT_WID) & ~TWO_OCTET_MASK));
    }

    static
    void encodeThreeOctets(char *octBuf, Iso10646Char isoBuf)
    {
        octBuf[2] = static_cast<char>(CONTINUE_TAG |
                     (isoBuf                           & ~CONTINUE_MASK));
        octBuf[1] = static_cast<char>(CONTINUE_TAG |
                    ((isoBuf >>     CONTINUE_CONT_WID) & ~CONTINUE_MASK));
        octBuf[0] = static_cast<char>(THREE_OCTET_TAG |
                    ((isoBuf >> 2 * CONTINUE_CONT_WID) & ~THREE_OCTET_MASK));
    }

    static
    void encodeFourOctets(char *octBuf, Iso10646Char isoBuf)
    {
        octBuf[3] = static_cast<char>(CONTINUE_TAG |
                        (isoBuf                           & ~CONTINUE_MASK));
        octBuf[2] = static_cast<char>(CONTINUE_TAG |
                       ((isoBuf >>     CONTINUE_CONT_WID) & ~CONTINUE_MASK));
        octBuf[1] = static_cast<char>(CONTINUE_TAG |
                       ((isoBuf >> 2 * CONTINUE_CONT_WID) & ~CONTINUE_MASK));
        octBuf[0] = static_cast<char>(FOUR_OCTET_TAG |
                       ((isoBuf >> 3 * CONTINUE_CONT_WID) & ~FOUR_OCTET_MASK));
    }
};

// LOCAL HELPER STRUCT
struct Utf16 {
    // 'Utf16' embodies the rules for converting between 21-bit (17 plane)
    // Unicode and UTF-16

    // TYPES
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

    template <class UTF16_CHAR>
    class PtrBasedEnd {
        // The 'class' determines whether translation is at the end of input by
        // compaarisons with an end pointer.

        // DATA
        const UTF16_CHAR * const d_end;

      public:
        // CREATORS
        explicit
        PtrBasedEnd(const UTF16_CHAR *end) : d_end(end) {}
            // Create a 'PtrBasedEnd' object with the end at the specified
            // 'end'.

        // ACCESSORS
        bool isFinished(const UTF16_CHAR *utf16Buf) const
            // Return 'true' if the specified 'utf16Buf' is at the end of
            // input and 'false' otherwise.
        {
            if (utf16Buf < d_end) {
                return false;                                         // RETURN
            }
            else {
                BSLS_ASSERT_SAFE(d_end == utf16Buf);

                return true;                                          // RETURN
            }
        }
    };

    template <class UTF16_CHAR>
    struct ZeroBasedEnd {
        // The 'class' determines whether translation is at the end of input by
        // evaluating whether the next word of input is 0.

        // ACCESSORS    
        bool isFinished(const UTF16_CHAR *u16Buf) const
            // Return 'true' if the specified 'utf16Buf' is at the end of
            // input and 'false' otherwise.
        {
            return !*u16Buf;
        }
    };

    // CLASS METHODS

    // Part 1: Determine how to decode an iso10646 character into UTF-16.
    //    The 'isSingleUtf8' static method returns true if the UTF-16 word
    //    passed as an argument will decode into a single UTF-8 character,
    //    false otherwise.
    //    The functions 'isSingleWord',
    //                  'isFirstWord' and
    //                   'isSecondWord' each return 'true' if the UTF-16 word
    //    passed as an argument is a single-word encoding, the first word of a
    //    two-word encoding, or the second word of a two-word encoding,
    //    respectively, and 'false' otherwise.

    template <class UTF16_CHAR>
    static
    bool isSingleUtf8(UTF16_CHAR uc)
    {
        return 0 == (uc & (~UTF16_CHAR(0) << Utf8::ONE_OCT_CONT_WID));
    }

    template <class UTF16_CHAR>
    static
    bool isSingleWord(UTF16_CHAR uc)
    {
        return (uc & RESERVED_MASK) != RESERVED_TAG;
    }

    template <class UTF16_CHAR>
    static
    bool isFirstWord(UTF16_CHAR uc)
    {
        return (uc & HEADER_MASK) == FIRST_TAG;
    }

    template <class UTF16_CHAR>
    static
    bool isSecondWord(UTF16_CHAR uc)
    {
        return (uc & HEADER_MASK) == SECOND_TAG;
    }

    // Part 2: The means to decode from UTF-16

    //     The 'getUtf8Value' static method returns the UTF-8 character
    //     corresponding to the single-word UTF-16 character passed in as its
    //     argument.  The result is invalid unless the word passed in is a
    //     valid single-word UTF-16 character that can be encoded as a
    //     single-byte UTF-8 character.
    //
    //     The functions 'decodeSingleWord' and
    //                   'decodeTwoWords' store, in the iso10646 character
    //     buffer addressed by their first argument, the iso10646 character
    //     encoded by the single-word or two-word UTF-16 character in the
    //     buffer addressed by their second argument.  The result is invalid
    //     unless there is enough space to store one iso10646 character at the
    //     location addressed by the first argument, and unless the one or two
    //     words addressed by the second argument are a valid iso10646
    //     character in the specified encoding.

    template <class UTF16_CHAR>
    static
    OctetType getUtf8Value(UTF16_CHAR uc)
    {
        return static_cast<OctetType>(uc);
    }

    template <class UTF16_CHAR>
    static
    void decodeSingleWord(Iso10646Char *isoBuf, const UTF16_CHAR *u16Buf)
    {
        *isoBuf = *u16Buf;
    }

    template <class UTF16_CHAR>
    static
    void decodeTwoWords(Iso10646Char *isoBuf, const UTF16_CHAR *u16Buf)
    {
        *isoBuf = RESERVE_OFFSET
                + ((u16Buf[0] & ~HEADER_MASK) << CONTENT_CONT_WID
                 | (u16Buf[1] & ~HEADER_MASK));
    }

    // Part 3: Determine how to encode a UTF-16 character.

    //     The functions 'fitsInOneWord',
    //                   'isValidOneWord' and
    //                   'isValidTwoWords' return 'true' if the iso10646
    //     character passed as their argument satisfies the specified
    //     condition, and 'false' otherwise.  'isValidTwoWords' yields an
    //     invalid result for a character that can be encoded in a single
    //     word.

    static
    bool fitsInOneWord(Iso10646Char uc)
    {
        return uc < RESERVE_OFFSET;
    }

    static
    bool isValidOneWord(Iso10646Char uc)
    {
        return (uc & RESERVED_MASK) != RESERVED_TAG;
    }

    static
    bool isValidTwoWords(Iso10646Char uc)
    {
        return uc < UPPER_LIMIT;
    }

    // Part 4: the means to encode a UTF-16 character

    //    The functions 'encodeSingleWord' and
    //                  'encodeTwoWords' encode the iso10646 character passed
    //    as their second argument into the UTF-16 word buffer addressed by
    //    their first argument.  The result is invalid if the buffer addressed
    //    by the first argument is not large enough, or if the character passed
    //    as the second argument is not a valid iso10646 character.

    template <class UTF16_CHAR>
    static
    void encodeSingleWord(UTF16_CHAR* u16Buf, Iso10646Char uc)
    {
        *u16Buf = static_cast<UTF16_CHAR>(uc);
    }

    template <class UTF16_CHAR>
    static
    void encodeTwoWords(UTF16_CHAR *u16Buf, Iso10646Char uc)
    {
        Iso10646Char v = uc - RESERVE_OFFSET;
        u16Buf[0] = static_cast<UTF16_CHAR>(FIRST_TAG  |
                                                      (v >> CONTENT_CONT_WID));
        u16Buf[1] = static_cast<UTF16_CHAR>(SECOND_TAG |
                                (v & ~(~Iso10646Char(0) << CONTENT_CONT_WID)));
    }

    template <class UTF16_CHAR>
    static
    bsl::size_t wslen(const UTF16_CHAR *str)
    {
        const UTF16_CHAR *pwc = str;
        while (*pwc) {
            ++pwc;
        }
        return pwc - str;
    }
};

// These compile-time asserts aren't strictly necessary, but we may plan to
// expand this component to support UTF-16 wstrings someday, which won't work
// if the size of a 'wchar_t' is less than that of a 'short' on any platform
// we port to.

BSLMF_ASSERT(sizeof(wchar_t)                  >= sizeof(unsigned short));
BSLMF_ASSERT(sizeof(bsl::wstring::value_type) >= sizeof(unsigned short));

// These template functions should be in the unnamed namespace, because if they
// are declared static, you have to fully specialize them every time you call
// them.

template <class END_FUNCTOR>
bsl::size_t utf16BufferLength(const char  *srcBuffer,
                              END_FUNCTOR  endFunctor)
    // Return the number of shorts required to store the translation of the
    // specified UTF-8 string 'srcBuffer', using the specified 'endFunctor' to
    // evaluate end of input and explore continuation bytes.  Note that this
    // routine will exactly estimate the right size except in two cases, in
    // which case it will either still return exactly the right value, or a
    // slight over estimation.  The two cases where this routine will
    // overestimate the size required are:
    //: o There are errors and the 'errorCharacter' is 0.  This routine assumes
    //:   that 'errorCharacter' is non-zero.
    //: o if a four byte sequence is a non-minimal encoding.  This would be
    //:   translated as a single error character, while we don't decode it so
    //:   we just assume it will result in 2 words output.
{
    bsl::size_t wordsNeeded = 0;

    // Working in unsigned makes bit manipulation with widening simpler;
    // changing the type here keeps the low-level routines short.

    const Utf8::OctetType *octets = static_cast<const Utf8::OctetType*>(
                                          static_cast<const void*>(srcBuffer));
    while (!endFunctor.isFinished(octets)) {
        if      (Utf8::isSingleOctet(     *octets)) {
            ++octets;
            ++wordsNeeded;
        }
        else if (Utf8::isTwoOctetHeader(  *octets)) {
            octets += endFunctor.verifyContinuations(octets + 1, 1) ? 2 : 1;
            ++wordsNeeded;
        }
        else if (Utf8::isThreeOctetHeader(*octets)) {
            if (endFunctor.verifyContinuations(octets + 1, 2)) {
                octets += 3;
            }
            else {
                octets = endFunctor.skipContinuations(octets + 1);
            }
            ++wordsNeeded;
        }
        else if (Utf8::isFourOctetHeader( *octets)) {
            if (endFunctor.verifyContinuations(octets + 1, 3)) {
                octets += 4;
            }
            else {
                octets = endFunctor.skipContinuations(octets + 1);
            }
            wordsNeeded += 2;      // will be overestimate if error
        }
        else {
            // Handle a five-octet character (or anything else) sanely.

            ++octets;
            if (endFunctor.verifyContinuations(octets, 4)) {
                octets += 4;
            }
            else {
                octets = endFunctor.skipContinuations(octets);
            }

            ++wordsNeeded;
        }
    }

    return wordsNeeded + 1;
}

template <class UTF16_CHAR, class CAPACITY_FUNCTOR, class END_FUNCTOR>
int localUtf8ToUtf16(UTF16_CHAR       *dstBuffer,
                     CAPACITY_FUNCTOR  dstCapacity,
                     const char       *srcBuffer,
                     END_FUNCTOR       endFunctor,
                     bsl::size_t      *numCharsWritten,
                     bsl::size_t      *numWordsWritten,
                     UTF16_CHAR        errorCharacter)
    // Translate from the specified null-terminated UTF-8 buffer 'srcBuffer' to
    // the specified null-terminated UTF-16 buffer 'dstBuffer' whose capacity
    // is evaluated by the specified 'dstCapacity', using the specified
    // 'endFunctor' to evaluate end of input and continuation bytes.  Return
    // the number of Unicode characters in the specified '*numCharsWritten' and
    // the number of 'UTF16_CHAR's written in '*numWordsWritten'.  The
    // specified 'errorCharacter' is output in place of any error sequences
    // encountered, or nothing is output in their place if
    // '0 == errorCharacter'.  Return a bit-wise or of the flags specified by
    // 'bdede_CharConvertStatus::Enum' to indicate whether error sequences were
    // encountered and/or whether the translation ran out of space.  Use type
    // 'CAPACITY_FUNCTOR' to check if there is enough room.  If the caller
    // isn't certain the output buffer will be big enough, 'CAPACITY_FUNCTOR'
    // should be 'Capacity' defined in this file, and the routine will
    // constantly check adequate room for output exists.  If the caller is
    // certain adequate room exists, 'CAPACITY_FUNCTOR' should be
    // 'NoOpCapacity', which translates all the checking to no-ops and always
    // returns that room is adequate, for faster execution.
{
    // Note 'dstCapacity' and 'endFunctor' are passed by value, not by
    // reference -- they're at most one pointer in size, so copying it is just
    // as fast as passing them by reference, then they are faster to access if
    // it is by value on the stack.

    // We need at least room for a null character in the output.

    if (dstCapacity < 1) {
        if (numCharsWritten) {
            *numCharsWritten = 0;
        }
        if (numWordsWritten) {
            *numWordsWritten = 0;
        }
        return OUT_OF_SPACE_BIT;                                      // RETURN
    }

    UTF16_CHAR *const dstStart = dstBuffer;
    bsl::size_t nChars = 0;
    int returnStatus = 0;

    // Working in unsigned makes bit manipulation with widening simpler;
    // changing the type here keeps the low-level routines short.

    const Utf8::OctetType *octets = static_cast<const Utf8::OctetType*>(
                                          static_cast<const void*>(srcBuffer));
    while (!endFunctor.isFinished(octets)) {
        // Checking for output space is tricky.  If we have an error case
        // and no replacement character, we may consume input octets without
        // using any space.

        if (errorCharacter && dstCapacity < 2) {
            // If there is an error character, we'll
            // need at least one output slot.

            returnStatus |= OUT_OF_SPACE_BIT;
            break;
        }

        // Single-octet case is simple and quick.

        if (Utf8::isSingleOctet(*octets)) {
            if (dstCapacity < 2) {
                // Are we out of output room, with only space for the null?

                returnStatus |= OUT_OF_SPACE_BIT;
                break;
            }

            Utf16::encodeSingleWord(dstBuffer, *octets);
            ++octets;
            ++dstBuffer;
            --dstCapacity;
            ++nChars;
            continue;
        }

        // Two, three, or four octets needed.

        // The error cases have a lot of repetition.  With the optimizer on,
        // the repeated code should all get folded together.

        Iso10646Char convBuf;

        if (Utf8::isTwoOctetHeader(*octets)) {
            if (!endFunctor.verifyContinuations(octets + 1, 1)) {
                returnStatus |= INVALID_CHARS_BIT;
                octets = endFunctor.skipContinuations(octets + 1);
                if (errorCharacter) {
                    *dstBuffer++ = errorCharacter;
                    --dstCapacity;
                    ++nChars;
                }
                continue;
            }

            Utf8::decodeTwoOctets(&convBuf, octets);
            octets += 2;
            if (Utf8::fitsInSingleOctet(convBuf)) { // Miscoding!  Mischief?
                returnStatus |= INVALID_CHARS_BIT;
                if (errorCharacter) {
                    *dstBuffer++ = errorCharacter;
                    --dstCapacity;
                    ++nChars;
                }
                continue;
            }
        }
        else if (Utf8::isThreeOctetHeader(*octets)) {
            if (!endFunctor.verifyContinuations(octets + 1, 2)) {
                returnStatus |= INVALID_CHARS_BIT;
                octets = endFunctor.skipContinuations(octets + 1);
                if (errorCharacter) {
                    *dstBuffer++ = errorCharacter;
                    --dstCapacity;
                    ++nChars;
                }
                continue;
            }

            Utf8::decodeThreeOctets(&convBuf, octets);
            octets += 3;
            if (Utf8::fitsInTwoOctets(convBuf)) { // Miscoding!  Mischief?
                returnStatus |= INVALID_CHARS_BIT;
                if (errorCharacter) {
                    *dstBuffer++ = errorCharacter;
                    --dstCapacity;
                    ++nChars;
                }
                continue;
            }
        }
        else if (Utf8::isFourOctetHeader(*octets)) {
            if (!endFunctor.verifyContinuations(octets + 1, 3)) {
                returnStatus |= INVALID_CHARS_BIT;
                octets = endFunctor.skipContinuations(octets + 1);
                if (errorCharacter) {
                    *dstBuffer++ = errorCharacter;
                    --dstCapacity;
                    ++nChars;
                }
                continue;
            }

            Utf8::decodeFourOctets(&convBuf, octets);
            octets += 4;
            if (Utf8::fitsInThreeOctets(convBuf)) { // Miscoding!  Mischief?
                returnStatus |= INVALID_CHARS_BIT;
                if (errorCharacter) {
                    *dstBuffer++ = errorCharacter;
                    --dstCapacity;
                    ++nChars;
                }
                continue;
            }
        }
        else {
            // Handle possibility of a five-octet character sanely.
            // (Continuations will have to be skipped in any case.)  6 and 7
            // character sequences (which are totally illegal) are handlesd
            // as skipping the first 5 chars then encountering unexpected
            // continuations, for multiple errors.

            ++octets;
            if (endFunctor.verifyContinuations(octets, 4)) {
                octets += 4;
            }
            else {
                octets = endFunctor.skipContinuations(octets);
            }

            returnStatus |= INVALID_CHARS_BIT;
            if (errorCharacter) {
                *dstBuffer++ = errorCharacter;
                --dstCapacity;
                ++nChars;
            }
            continue;
        }

        // The UTF-8 character is decoded.  Re-encode it in UTF-16.
        // We could assume that a two- or three-octet UTF-8 character will have
        // to be a single-word UTF-16 character, and a four-octet UTF-8
        // character will have to be a two-word UTF-16 character.  But the
        // logic is less tangled if the decode and encode, and all their cases,
        // are kept separate, especially since the validity tests must be made
        // for three- and four-octet UTF-8 encodings.

        if (Utf16::fitsInOneWord(convBuf)) {
            if (!Utf16::isValidOneWord(convBuf)) {
                returnStatus |= INVALID_CHARS_BIT;
                if (errorCharacter) {
                    *dstBuffer++ = errorCharacter;
                    --dstCapacity;
                    ++nChars;
                }
                continue;
            }
            if (dstCapacity < 2) {  // Need room for at least one plus a null.
                returnStatus |= OUT_OF_SPACE_BIT;
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

            if (!Utf16::isValidTwoWords(convBuf)) {
                returnStatus |= INVALID_CHARS_BIT;
                if (errorCharacter) {
                    *dstBuffer++ = errorCharacter;
                    --dstCapacity;
                    ++nChars;
                }
                continue;
            }
            if (dstCapacity < 3) {  // Need room for at least two plus a null.
                returnStatus |= OUT_OF_SPACE_BIT;
                break;
            }
            Utf16::encodeTwoWords(dstBuffer, convBuf);
            dstBuffer += 2;
            dstCapacity -= 2;
            ++nChars;
        }
    }
    BSLS_ASSERT(endFunctor.isFinished(octets) ||
                                            (returnStatus & OUT_OF_SPACE_BIT));

    *dstBuffer++ = 0;
    ++nChars;

    if (numCharsWritten) {
        *numCharsWritten = nChars;
    }
    if (numWordsWritten) {
        *numWordsWritten = dstBuffer - dstStart;
    }

#ifdef BDE_BUILD_TARGET_SAFE
    if (sizeof(UTF16_CHAR) > sizeof(unsigned short)) {
        UTF16_CHAR forbidden_mask = (UTF16_CHAR)-1;
        forbidden_mask &= static_cast<UTF16_CHAR>(~0xffff);
        for (UTF16_CHAR *pwc = dstStart; pwc < dstBuffer; ++pwc) {
            BSLS_ASSERT(0 == (forbidden_mask & *pwc));
        }
    }
#endif

    return returnStatus;
}

template <class UTF16_CHAR, class END_FUNCTOR>
bsl::size_t utf8BufferLength(const UTF16_CHAR *srcBuffer,
                             END_FUNCTOR       endFunctor)
    // Return the length needed in bytes, for a buffer to hold the
    // null-terminated UTF-8 string translated from the specified
    // null-terminated UTF-16 string 'srcBuffer', using 'endFunctor' to
    // evaluate end of input.  Note that the method will get the length exactly
    // right unless there are errors and the 'errorCharacter' is 0, in which
    // case it will slightly over estimate the necessary length.
{
    bsl::size_t bytesNeeded = 0;
    while (!endFunctor.isFinished(srcBuffer)) {
        if      (Utf16::isSingleUtf8(*srcBuffer)) {
            ++srcBuffer;
            ++bytesNeeded;
        }
        else if (Utf16::isSingleWord(*srcBuffer)) {
            Iso10646Char convBuf;

            Utf16::decodeSingleWord(&convBuf, srcBuffer);
            ++srcBuffer;
            bytesNeeded += Utf8::fitsInTwoOctets(convBuf) ? 2 : 3;
        }
        else if (!endFunctor.isFinished(srcBuffer + 1)
              && Utf16::isFirstWord( srcBuffer[0])
              && Utf16::isSecondWord(srcBuffer[1])) {
            srcBuffer += 2;
            bytesNeeded += 4;
        }
        else {
            ++srcBuffer;
            ++bytesNeeded;    // error char
        }
    }
    BSLS_ASSERT(endFunctor.isFinished(srcBuffer));

    return bytesNeeded + 1;
}

template <class UTF16_CHAR, class CAPACITY_FUNCTOR, class END_FUNCTOR>
int localUtf16ToUtf8(char             *dstBuffer,
                     CAPACITY_FUNCTOR  dstCapacity,
                     const UTF16_CHAR *srcBuffer,
                     END_FUNCTOR       endFunctor,
                     bsl::size_t      *numCharsWritten,
                     bsl::size_t      *numBytesWritten,
                     char              errorCharacter)
    // Translate from the specified null-terminated UTF-16 buffer 'srcBuffer',
    // using the specified 'endFunctor' to determine end of input, to the
    // specified null-terminated UTF-8 buffer 'dstBuffer' using the specified
    // 'dstCapacity'; to evaluate the size of 'dstBuffer'.  Return the number
    // of Unicode characters translated in the specified '*numCharsWritten' and
    // the number of bytes written in the specified '*numBytesWritten'.  Return
    // a bit-wise or of the flags specified by 'bdede_CharConvertStatus::Enum'
    // to indicate whether error sequences were encountered and/or whether the
    // translation ran out of space.  Use type 'CAPACITY_FUNCTOR' to check if
    // there is enough room.  If the caller isn't certain the output buffer
    // will be big enough, 'CAPACITY_FUNCTOR' should be 'Capacity' defined in
    // this file, and the routine will constantly check adequate room for
    // output exists.  If the caller is certain adequate room exists,
    // 'CAPACITY_FUNCTOR' should be 'NoOpCapacity', which translates all the
    // checking to no-ops and always returns that room is adequate, for faster
    // execution.
{
    if (dstCapacity < 1) {
        if (numCharsWritten) {
            *numCharsWritten = 0;
        }
        if (numBytesWritten) {
            *numBytesWritten = 0;
        }
        return OUT_OF_SPACE_BIT;                                      // RETURN
    }

    char *const dstStart = dstBuffer;

    bsl::size_t nChars = 0;

    int returnStatus = 0;
    while (!endFunctor.isFinished(srcBuffer)) {
        // We don't do the out-of-room tests until we know that we can
        // generate valid Iso10646Char characters from the UTF-16 string.

        // The single-byte case is the simplest.  It ought to be the fastest.

        if (Utf16::isSingleUtf8(*srcBuffer)) {
            if (dstCapacity < 2) {
                // One for the char, one for the null.

                returnStatus |= OUT_OF_SPACE_BIT;
                break;
            }
            *dstBuffer = Utf16::getUtf8Value(*srcBuffer);
            ++srcBuffer;
            ++dstBuffer;
            --dstCapacity;
            ++nChars;
            continue;
        }

        Iso10646Char convBuf;

        // Is it a single-word character?

        if (Utf16::isSingleWord(*srcBuffer)) {
            Utf16::decodeSingleWord(&convBuf, srcBuffer);
            ++srcBuffer;

            // One word encodes to one, two, or three UTF-8 octets.  We have
            // already handled the single-octet case.

            if (Utf8::fitsInTwoOctets(convBuf)) {
                // Need two bytes plus a null.

                if (dstCapacity < 3) {
                    returnStatus |= OUT_OF_SPACE_BIT;
                    break;
                }
                Utf8::encodeTwoOctets(dstBuffer, convBuf);
                dstBuffer += 2;
                dstCapacity -= 2;
                ++nChars;
                continue;
            }

            // Need three bytes plus a null.

            if (dstCapacity < 4) {
                returnStatus |= OUT_OF_SPACE_BIT;
                break;
            }

            Utf8::encodeThreeOctets(dstBuffer, convBuf);
            dstBuffer += 3;
            dstCapacity -= 3;
            ++nChars;
            continue;
        }

        // Not one word, must be two.

        if (endFunctor.isFinished(srcBuffer + 1)
           || !Utf16::isFirstWord(*srcBuffer)
           || !Utf16::isSecondWord(srcBuffer[1])) {

            // ( !isSecondWord() covers the case of null.  )

            srcBuffer++;
            if (0 != errorCharacter) {
                if (dstCapacity < 2) {
                    // One for the char, one for the null.

                    returnStatus |= OUT_OF_SPACE_BIT;
                    break;
                }
                *dstBuffer = errorCharacter;
                ++dstBuffer;
                --dstCapacity;
                ++nChars;
            }
            returnStatus |= INVALID_CHARS_BIT;
            continue;
        }

        // Need four octets plus a null

        if (dstCapacity < 5) {
            returnStatus |= OUT_OF_SPACE_BIT;

            break;
        }

        Utf16::decodeTwoWords(&convBuf, srcBuffer);
        srcBuffer += 2;

        Utf8::encodeFourOctets(dstBuffer, convBuf);
        dstBuffer += 4;
        dstCapacity -= 4;
        ++nChars;
    }

    BSLS_ASSERT(endFunctor.isFinished(srcBuffer) ||
                                            (returnStatus & OUT_OF_SPACE_BIT));

    *dstBuffer++ = 0;
    ++nChars;

    if (numBytesWritten) {
        *numBytesWritten = dstBuffer - dstStart;
    }

    if (numCharsWritten) {
        *numCharsWritten = nChars;
    }

    return returnStatus;
}

template <class UTF16_CHAR, class END_FUNCTOR>
int localUtf16ToUtf8String(bsl::string      *dstString,
                           const UTF16_CHAR *srcBuffer,
                           END_FUNCTOR       endFunctor,
                           bsl::size_t      *numCharsWritten,
                           char              errorCharacter)
    // Translate the UTF-16 encoded sequence in the specified 'srcBuffer' to
    // the specified 'dstString', using the specified 'endFunctor' to evaluate
    // end of input.  Any pre-existing contents of 'dstString' are discarded.
    // Return the number of characters (not words or bytes) translated to the
    // specified '*numCharsWritten'.  If error sequences are encountered,
    // substitute the specified 'errorCharacter', or eliminate the sequence
    // entirely if '0 == errorCharacter'.  The behavior is undefined if
    // 'srcBuffer' is not null-terminated, or if 'errorCharacter >= 0x80'.
{
    bsl::size_t estLength = utf8BufferLength(srcBuffer,
                                             endFunctor);
    BSLS_ASSERT(estLength > 0);

    // Set the length big enough to include the '\0' at the end.  There's no
    // way to stop the routine we call from writing it, and we don't have
    // permission to write past 'dstString->begin() + dstString->length()'.  So
    // we'll have to chop the '\0' off after the translation.

    if (estLength > dstString->length()) {
        dstString->resize(estLength);
    }

    bsl::size_t numBytesWritten;

    int rc = localUtf16ToUtf8(dstString->begin(),
                              NoOpCapacity(),
                              srcBuffer,
                              endFunctor,
                              numCharsWritten,
                              &numBytesWritten,
                              errorCharacter);
    BSLS_ASSERT_SAFE(0 == (OUT_OF_SPACE_BIT & rc));
    if (numBytesWritten != estLength) {
        BSLS_ASSERT(numBytesWritten < estLength);
        BSLS_ASSERT(INVALID_CHARS_BIT & rc);
        BSLS_ASSERT(0 == errorCharacter);
    }
    BSLS_ASSERT(numBytesWritten <= dstString->length());
    BSLS_ASSERT(0 == (*dstString)[numBytesWritten - 1]);

    // 'numBytesWritten' may be much smaller than 'length()', plus we always
    // have to get rid of the '\0'.

    dstString->resize(numBytesWritten - 1);

    return rc;
}

template <class UTF16_CHAR, class END_FUNCTOR>
int localUtf16ToUtf8Vector(bsl::vector<char> *dstVector,
                           const UTF16_CHAR  *srcBuffer,
                           END_FUNCTOR        endFunctor,
                           bsl::size_t       *numCharsWritten,
                           char               errorCharacter)
    // Translate the UTF-16 encoded sequence in the specified 'srcBuffer' to
    // the specified 'dstString', using the specified 'endFunctor' to evaluate
    // end of input and null-terminating the result.  Any pre-existing contents
    // of 'dstString' are discarded.  Return the number of characters (not
    // words or bytes) translated to the specified '*numCharsWritten'.  If
    // error sequences are encountered, substitute the specified
    // 'errorCharacter', or eliminate the sequence entirely if
    // '0 == errorCharacter'.  The behavior is undefined if 'srcBuffer' is not
    // null-terminated, or if 'errorCharacter >= 0x80'.
{
    bsl::size_t estLength = utf8BufferLength(srcBuffer,
                                             endFunctor);
    BSLS_ASSERT(estLength > 0);

    if (estLength > dstVector->size()) {
        dstVector->resize(estLength);
    }

    bsl::size_t numBytesWritten;

    int rc = localUtf16ToUtf8(dstVector->begin(),
                              NoOpCapacity(),
                              srcBuffer,
                              endFunctor,
                              numCharsWritten,
                              &numBytesWritten,
                              errorCharacter);
    BSLS_ASSERT_SAFE(0 == (OUT_OF_SPACE_BIT & rc));
    if (numBytesWritten != estLength) {
        BSLS_ASSERT(numBytesWritten < estLength);
        BSLS_ASSERT(INVALID_CHARS_BIT & rc);
        BSLS_ASSERT(0 == errorCharacter);
    }
    BSLS_ASSERT(0 == (*dstVector)[numBytesWritten - 1]);

    if (numBytesWritten != dstVector->size()) {
        BSLS_ASSERT(numBytesWritten < dstVector->size());

        dstVector->resize(numBytesWritten);
    }

    return rc;
}

}  // close unnamed namespace

namespace BloombergLP {

// This component overloads single routine names where the only difference
// between args is that one is an 'unsigned short *' while the same arg in the
// other is 'wchar_t *'.  If we ever port to a platform where 'wchar_t' is just
// a typedef to 'unsigned short' this will result in the compiler rightfully
// complaining about routines multiply defined and we will need to '#ifdef'
// away all the routines taking a 'const wchar_t *' pointer as input on that
// platform (at which point everything will then work).  'wchar_t' is 4 bytes
// on Linux, Solaris (CC and gcc), HP-UX, and Darwin.  On AIX and Windows,
// 'wchar_t' is unsigned and the size of short, but it is a compiler-defined
// type considered distinct from 'unsigned short'.

BSLMF_ASSERT(!(bslmf::IsSame<wchar_t, unsigned short>::VALUE));

                        // -----------------------------
                        // struct bdede_CharConvertUtf16
                        // -----------------------------

// CLASS METHODS

                        // -- UTF-8 to UTF-16 Methods

int bdede_CharConvertUtf16::utf8ToUtf16(
                                     bsl::wstring             *dstWstring,
                                     const bslstl::StringRef&  srcString,
                                     bsl::size_t              *numCharsWritten,
                                     wchar_t                   errorCharacter)
{
    Utf8::PtrBasedEnd endFunctor(srcString.end());

    bsl::size_t estLength = utf16BufferLength(srcString.data(),
                                              endFunctor);
    BSLS_ASSERT(estLength > 0);

    // Set the length big enough to include the '\0' at the end.  There's no
    // way to stop the routine we call from writing it, and we don't have
    // permission to write past 'dstWstring->begin() + dstWstring->length()'.
    // So we'll have to chop the '\0' off after the translation.

    if (estLength > dstWstring->length()) {
        dstWstring->resize(estLength);
    }

    bsl::size_t numWordsWritten;

    int rc = localUtf8ToUtf16(dstWstring->begin(),
                              NoOpCapacity(),
                              srcString.data(),
                              endFunctor,
                              numCharsWritten,
                              &numWordsWritten,
                              errorCharacter);
    BSLS_ASSERT_SAFE(0 == (OUT_OF_SPACE_BIT & rc));
    if (numWordsWritten != estLength) {
        BSLS_ASSERT(numWordsWritten < estLength);
        BSLS_ASSERT(INVALID_CHARS_BIT & rc);
    }
    BSLS_ASSERT(0 == (*dstWstring)[numWordsWritten - 1]);

    BSLS_ASSERT(numWordsWritten <= dstWstring->length());

    // 'dstWstring' may be much longer than necessary, plus we always want to
    // get rid of the terminating 0.

    dstWstring->resize(numWordsWritten - 1);

    return rc;
}

int bdede_CharConvertUtf16::utf8ToUtf16(
                                     bsl::wstring             *dstWstring,
                                     const char               *srcString,
                                     bsl::size_t              *numCharsWritten,
                                     wchar_t                   errorCharacter)
{
    Utf8::ZeroBasedEnd endFunctor;

    bsl::size_t estLength = utf16BufferLength(srcString,
                                              endFunctor);
    BSLS_ASSERT(estLength > 0);

    // Set the length big enough to include the '\0' at the end.  There's no
    // way to stop the routine we call from writing it, and we don't have
    // permission to write past 'dstWstring->begin() + dstWstring->length()'.
    // So we'll have to chop the '\0' off after the translation.

    if (estLength > dstWstring->length()) {
        dstWstring->resize(estLength);
    }

    bsl::size_t numWordsWritten;

    int rc = localUtf8ToUtf16(dstWstring->begin(),
                              NoOpCapacity(),
                              srcString,
                              endFunctor,
                              numCharsWritten,
                              &numWordsWritten,
                              errorCharacter);
    BSLS_ASSERT_SAFE(0 == (OUT_OF_SPACE_BIT & rc));
    if (numWordsWritten != estLength) {
        BSLS_ASSERT(numWordsWritten < estLength);
        BSLS_ASSERT(INVALID_CHARS_BIT & rc);
    }
    BSLS_ASSERT(0 == (*dstWstring)[numWordsWritten - 1]);

    BSLS_ASSERT(numWordsWritten <= dstWstring->length());

    // 'dstWstring' may be much longer than necessary, plus we always want to
    // get rid of the terminating 0.

    dstWstring->resize(numWordsWritten - 1);

    return rc;
}

int bdede_CharConvertUtf16::utf8ToUtf16(
                                  bsl::vector<unsigned short> *dstVector,
                                  const bslstl::StringRef&     srcString,
                                  bsl::size_t                 *numCharsWritten,
                                  unsigned short               errorCharacter)
{
    Utf8::PtrBasedEnd endFunctor(srcString.end());

    bsl::size_t estLength = utf16BufferLength(srcString.data(),
                                              endFunctor);
    BSLS_ASSERT(estLength > 0);

    if (estLength > dstVector->size()) {
        dstVector->resize(estLength);
    }

    bsl::size_t numWordsWritten;

    int rc = localUtf8ToUtf16(dstVector->begin(),
                              NoOpCapacity(),
                              srcString.data(),
                              endFunctor,
                              numCharsWritten,
                              &numWordsWritten,
                              errorCharacter);
    BSLS_ASSERT_SAFE(0 == (OUT_OF_SPACE_BIT & rc));
    if (numWordsWritten != estLength) {
        BSLS_ASSERT(numWordsWritten < estLength);
        BSLS_ASSERT(INVALID_CHARS_BIT & rc);
    }
    BSLS_ASSERT(0 == (*dstVector)[numWordsWritten - 1]);

    if (numWordsWritten != dstVector->size()) {
        BSLS_ASSERT(numWordsWritten < dstVector->size());

        dstVector->resize(numWordsWritten);
    }

    return rc;
}

int bdede_CharConvertUtf16::utf8ToUtf16(
                         bsl::vector<unsigned short> *dstVector,
                         const char                  *srcString,
                         bsl::size_t                 *numCharsWritten,
                         unsigned short               errorCharacter)
{
    Utf8::ZeroBasedEnd endFunctor;

    bsl::size_t estLength = utf16BufferLength(srcString,
                                              endFunctor);
    BSLS_ASSERT(estLength > 0);

    if (estLength > dstVector->size()) {
        dstVector->resize(estLength);
    }

    bsl::size_t numWordsWritten;

    int rc = localUtf8ToUtf16(dstVector->begin(),
                              NoOpCapacity(),
                              srcString,
                              endFunctor,
                              numCharsWritten,
                              &numWordsWritten,
                              errorCharacter);
    BSLS_ASSERT_SAFE(0 == (OUT_OF_SPACE_BIT & rc));
    if (numWordsWritten != estLength) {
        BSLS_ASSERT(numWordsWritten < estLength);
        BSLS_ASSERT(INVALID_CHARS_BIT & rc);
    }
    BSLS_ASSERT(0 == (*dstVector)[numWordsWritten - 1]);

    if (numWordsWritten != dstVector->size()) {
        BSLS_ASSERT(numWordsWritten < dstVector->size());

        dstVector->resize(numWordsWritten);
    }

    return rc;
}

int bdede_CharConvertUtf16::utf8ToUtf16(
                                     unsigned short           *dstBuffer,
                                     bsl::size_t               dstCapacity,
                                     const bslstl::StringRef&  srcString,
                                     bsl::size_t              *numCharsWritten,
                                     bsl::size_t              *numWordsWritten,
                                     unsigned short            errorCharacter)
{
    return localUtf8ToUtf16(dstBuffer,
                            Capacity(dstCapacity),
                            srcString.data(),
                            Utf8::PtrBasedEnd(srcString.end()),
                            numCharsWritten,
                            numWordsWritten,
                            errorCharacter);
}

int bdede_CharConvertUtf16::utf8ToUtf16(
                                     unsigned short           *dstBuffer,
                                     bsl::size_t               dstCapacity,
                                     const char               *srcString,
                                     bsl::size_t              *numCharsWritten,
                                     bsl::size_t              *numWordsWritten,
                                     unsigned short            errorCharacter)
{
    return localUtf8ToUtf16(dstBuffer,
                            Capacity(dstCapacity),
                            srcString,
                            Utf8::ZeroBasedEnd(),
                            numCharsWritten,
                            numWordsWritten,
                            errorCharacter);
}

int bdede_CharConvertUtf16::utf8ToUtf16(
                                    wchar_t                  *dstBuffer,
                                    bsl::size_t               dstCapacity,
                                    const bslstl::StringRef&  srcString,
                                    bsl::size_t              *numCharsWritten,
                                    bsl::size_t              *numWordsWritten,
                                    wchar_t                   errorCharacter)
{
    return localUtf8ToUtf16(dstBuffer,
                            Capacity(dstCapacity),
                            srcString.data(),
                            Utf8::PtrBasedEnd(srcString.end()),
                            numCharsWritten,
                            numWordsWritten,
                            errorCharacter);
}

int bdede_CharConvertUtf16::utf8ToUtf16(
                                    wchar_t                  *dstBuffer,
                                    bsl::size_t               dstCapacity,
                                    const char               *srcString,
                                    bsl::size_t              *numCharsWritten,
                                    bsl::size_t              *numWordsWritten,
                                    wchar_t                   errorCharacter)
{
    return localUtf8ToUtf16(dstBuffer,
                            Capacity(dstCapacity),
                            srcString,
                            Utf8::ZeroBasedEnd(),
                            numCharsWritten,
                            numWordsWritten,
                            errorCharacter);
}

                        // -- UTF-16 to UTF-8 Methods

int bdede_CharConvertUtf16::utf16ToUtf8(bsl::string          *dstString,
                                        const unsigned short *srcString,
                                        bsl::size_t          *numCharsWritten,
                                        char                  errorCharacter)
{
    return localUtf16ToUtf8String(dstString,
                                  srcString,
                                  Utf16::ZeroBasedEnd<unsigned short>(),
                                  numCharsWritten,
                                  errorCharacter);
}

int bdede_CharConvertUtf16::utf16ToUtf8(
                                 bsl::string                  *dstString,
                                 const bslstl::StringRefWide&  srcString,
                                 bsl::size_t                  *numCharsWritten,
                                 char                          errorCharacter)
{
    return localUtf16ToUtf8String(dstString,
                                  srcString.data(),
                                  Utf16::PtrBasedEnd<wchar_t>(srcString.end()),
                                  numCharsWritten,
                                  errorCharacter);
}

int bdede_CharConvertUtf16::utf16ToUtf8(
                                 bsl::string                  *dstString,
                                 const wchar_t                *srcString,
                                 bsl::size_t                  *numCharsWritten,
                                 char                          errorCharacter)
{
    return localUtf16ToUtf8String(dstString,
                                  srcString,
                                  Utf16::ZeroBasedEnd<wchar_t>(),
                                  numCharsWritten,
                                  errorCharacter);
}

int bdede_CharConvertUtf16::utf16ToUtf8(bsl::vector<char>    *dstVector,
                                        const unsigned short *srcString,
                                        bsl::size_t          *numCharsWritten,
                                        char                  errorCharacter)
{
    return localUtf16ToUtf8Vector(dstVector,
                                  srcString,
                                  Utf16::ZeroBasedEnd<unsigned short>(),
                                  numCharsWritten,
                                  errorCharacter);
}

int bdede_CharConvertUtf16::utf16ToUtf8(
                                 bsl::vector<char>            *dstVector,
                                 const bslstl::StringRefWide&  srcString,
                                 bsl::size_t                  *numCharsWritten,
                                 char                          errorCharacter)
{
    return localUtf16ToUtf8Vector(dstVector,
                                  srcString.data(),
                                  Utf16::PtrBasedEnd<wchar_t>(srcString.end()),
                                  numCharsWritten,
                                  errorCharacter);
}

int bdede_CharConvertUtf16::utf16ToUtf8(
                                 bsl::vector<char>            *dstVector,
                                 const wchar_t                *srcString,
                                 bsl::size_t                  *numCharsWritten,
                                 char                          errorCharacter)
{
    return localUtf16ToUtf8Vector(dstVector,
                                  srcString,
                                  Utf16::ZeroBasedEnd<wchar_t>(),
                                  numCharsWritten,
                                  errorCharacter);
}

int bdede_CharConvertUtf16::utf16ToUtf8(char                 *dstBuffer,
                                        bsl::size_t           dstCapacity,
                                        const unsigned short *srcString,
                                        bsl::size_t          *numCharsWritten,
                                        bsl::size_t          *numBytesWritten,
                                        char                  errorCharacter)
{
    return localUtf16ToUtf8(dstBuffer,
                            Capacity(dstCapacity),
                            srcString,
                            Utf16::ZeroBasedEnd<unsigned short>(),
                            numCharsWritten,
                            numBytesWritten,
                            errorCharacter);
}

int bdede_CharConvertUtf16::utf16ToUtf8(
                                 char                         *dstBuffer,
                                 bsl::size_t                   dstCapacity,
                                 const bslstl::StringRefWide&  srcString,
                                 bsl::size_t                  *numCharsWritten,
                                 bsl::size_t                  *numBytesWritten,
                                 char                          errorCharacter)
{
    return localUtf16ToUtf8(dstBuffer,
                            Capacity(dstCapacity),
                            srcString.data(),
                            Utf16::PtrBasedEnd<wchar_t>(srcString.end()),
                            numCharsWritten,
                            numBytesWritten,
                            errorCharacter);
}

int bdede_CharConvertUtf16::utf16ToUtf8(
                                 char                         *dstBuffer,
                                 bsl::size_t                   dstCapacity,
                                 const wchar_t                *srcString,
                                 bsl::size_t                  *numCharsWritten,
                                 bsl::size_t                  *numBytesWritten,
                                 char                          errorCharacter)
{
    return localUtf16ToUtf8(dstBuffer,
                            Capacity(dstCapacity),
                            srcString,
                            Utf16::ZeroBasedEnd<wchar_t>(),
                            numCharsWritten,
                            numBytesWritten,
                            errorCharacter);
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
