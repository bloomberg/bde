// bdede_charconvertutf32.cpp                                         -*-C++-*-
#include <bdede_charconvertutf32.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdede_charconvertutf32_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>     // 'BSLMF_ASSERT'
#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>    // 'bsl::find'

#include <bsl_climits.h>      // 'CHAR_BIT'
#include <bsl_cstring.h>      // 'CHAR_BIT'

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

// UTF-32 encoding is straightforward -- one 'unsigned int' *word* of UTF-32
// corresponds to one *character* of Unicode.  Values must be in the range
// '[ 0 .. 0xd7ff ]' or in the range '[ 0xe000 .. 0x10ffff ]'.

namespace {

// TYPES
typedef unsigned int Iso10646Char;
    // For storing uncompressed Unicode character (21 bits, 17 planes).

typedef unsigned char OctetType;
    // Treating the octets as a signed (or default, which is usally signed)
    // 'char' has so many pitfalls in widening that the code would become
    // unreadable.  This typedef (used especially for pointer punning) gives a
    // shorter way to write all the conversions necessary.

// Portability/sanity checks -- data type sizes.
BSLMF_ASSERT(8 == CHAR_BIT);
BSLMF_ASSERT(4 == sizeof(Iso10646Char));
BSLMF_ASSERT(sizeof(char) == sizeof(OctetType));

enum {
    // The return values of all the public functions defined by this component
    // return a bit-wise-or of these two flags.  When the output is to
    // container rather than a fixed-length buffer, the 'OUT_OF_SPACE_BIT'
    // is not set.

    INVALID_CHARS_BIT =
                 BloombergLP::bdede_CharConvertStatus::BDEDE_INVALID_CHARS_BIT,
    OUT_OF_SPACE_BIT  =
                 BloombergLP::bdede_CharConvertStatus::BDEDE_OUT_OF_SPACE_BIT
};

enum Utf8Bits {
    // Masks and shifts used to assemble and dismantle UTF-8 encodings.

    ONE_OCT_CONT_WID   = 7,               // Content in a one-octet coding
    ONE_OCTET_MASK     = 0xff & (~0 << ONE_OCT_CONT_WID),
    ONE_OCTET_TAG      = 0xff & 0,        // Compare this to masked bits

    CONTINUE_CONT_WID  = 6,               // Content in a continuation
                                          //                      octet
    CONTINUE_MASK      = 0xff & (~0 << CONTINUE_CONT_WID),
    CONTINUE_TAG       = ONE_OCTET_MASK,  // Compare this to masked bits

    TWO_OCT_CONT_WID   = 5,               // Content in a two-octet header
    TWO_OCTET_MASK     = 0xff & (~0 << TWO_OCT_CONT_WID),
    TWO_OCTET_TAG      = CONTINUE_MASK,   // Compare this to masked bits

    THREE_OCT_CONT_WID = 4,               // Content in a 3-octet header
    THREE_OCTET_MASK   = 0xff & (~0 << THREE_OCT_CONT_WID),
    THREE_OCTET_TAG    = TWO_OCTET_MASK,  // Compare this to masked bits

    FOUR_OCT_CONT_WID  = 3,               // Content in a four-octet header
    FOUR_OCTET_MASK    = 0xff & (~0 << FOUR_OCT_CONT_WID),
    FOUR_OCTET_TAG     = THREE_OCTET_MASK // Compare this to masked bits
};

struct Capacity {
    // Functor passed to 'Utf8ToUtf32Translator' and 'Utf32ToUtf8Translator' in
    // cases where we monitor capacity available in output.  Initialize in
    // c'tor with an integer 'capacity', then thereafter support operators
    // '--', '-=', and '<' for that value.

    bsl::size_t d_capacity;

    // CREATORS
    explicit
    Capacity(bsl::size_t capacity) : d_capacity(capacity) {}
        // Create a 'Capacity' object with the specified 'capacity'.

    // MANIPULATORS
    void operator--() { --d_capacity; }
        // Decrement 'd_capacity'

    void operator-=(int delta) { d_capacity -= delta; }
        // Decrement 'd_capacity' by 'delta'.

    // ACCESSORS
    bool operator<( bsl::size_t rhs) const { return d_capacity <  rhs; }
        // Return 'true' if 'd_capacity <  rhs', and 'false' otherwise.

    bool operator>=(bsl::size_t rhs) const { return d_capacity >= rhs; }
        // Return 'true' if 'd_capacity >= rhs', and 'false' otherwise.
};

struct NoopCapacity {
    // Functor passed to 'Utf8ToUtf32Translator' and 'Utf32ToUtf8Translator' in
    // cases where we don't want to monitor capacity available in output, all
    // operations on this object are to become no-ops, the '<' and '>='
    // operators return constant 'bool' values to indicate that adequate
    // capacity is available.

    // CREATORS
    explicit
    NoopCapacity(bsl::size_t) {}
        // Create empty object.

    // MANIPULATORS
    void operator--() { }
        // No-op.

    void operator-=(int) { }
        // No-op.

    // ACCESSORS
    bool operator<( bsl::size_t) const { return false; }
        // Return 'false'.

    bool operator>=(bsl::size_t) const { return true; }
        // Return 'true'.
};

}  // close unnamed namespace

static inline
const OctetType *constOctetCast(const char *ptr)
    // Return the specified 'ptr' cast to a 'const OctetType *'.  Note that
    // 'static_cast' does not work here, and the idea is to be sure in these
    // casts that one is never accidentally casting between pointers to 'char'
    // or 'OctetType' and pointers to 'Iso10646Char'.
{
    BSLMF_ASSERT(sizeof(*ptr) == sizeof(OctetType));
    return reinterpret_cast<const OctetType *>(ptr);
}

static inline
OctetType *octetCast(char *ptr)
    // Return the specified 'ptr' cast to an 'OctetType *'.  Note that
    // 'static_cast' does not work here, and the idea is to be sure in these
    // casts that one is never accidentally casting between pointers to 'char'
    // or 'OctetType' and pointers to 'Iso10646Char'.
{
    BSLMF_ASSERT(sizeof(*ptr) == sizeof(OctetType));
    return reinterpret_cast<OctetType *>(ptr);
}

static inline
bool isSingleOctet(OctetType oct)
    // Return 'true' if the specified 'oct' is a single-octet UTF-8 sequence.
{
    return ! (oct & ONE_OCTET_MASK);
}

static inline
bool isContinuation(OctetType oct)
    // Return 'true' if the specified 'oct' is a continuation octet.
{
    return (oct & CONTINUE_MASK) == CONTINUE_TAG;
}

static inline
bool isTwoOctetHeader(OctetType oct)
    // Return 'true' if the specified 'oct' is the first octet of a two-octet
    // UTF-8 sequence.
{
    return (oct & TWO_OCTET_MASK) == TWO_OCTET_TAG;
}

static inline
bool isThreeOctetHeader(OctetType oct)
    // Return 'true' if the specified 'oct' is the first octet of a three-octet
    // UTF-8 sequence.
{
    return (oct & THREE_OCTET_MASK) == THREE_OCTET_TAG;
}

static inline
bool isFourOctetHeader(OctetType oct)
    // Return 'true' if the specified 'oct' is the first octet of a four-octet
    // UTF-8 sequence.
{
    return (oct & FOUR_OCTET_MASK) == FOUR_OCTET_TAG;
}

static inline
Iso10646Char decodeTwoOctets(const OctetType *octBuf)
    // Return the value of the two-octet sequence that begins with the octet
    // pointed to by the specified 'octBuf'.
{
    return     (octBuf[1] & ~CONTINUE_MASK)
            | ((octBuf[0] & ~TWO_OCTET_MASK)   <<     CONTINUE_CONT_WID);
}

static inline
Iso10646Char decodeThreeOctets(const OctetType *octBuf)
    // Return the value of the three-octet sequence that begins with the octet
    // pointed to by the specified 'octBuf'.
{
    return     (octBuf[2] & ~CONTINUE_MASK)
            | ((octBuf[1] & ~CONTINUE_MASK)    <<     CONTINUE_CONT_WID)
            | ((octBuf[0] & ~THREE_OCTET_MASK) << 2 * CONTINUE_CONT_WID);
}

static inline
Iso10646Char decodeFourOctets(const OctetType *octBuf)
    // Return the value of the four-octet sequence that begins with the octet
    // pointed to by the specified 'octBuf'.
{
    return     (octBuf[3] & ~CONTINUE_MASK)
            | ((octBuf[2] & ~CONTINUE_MASK)   <<     CONTINUE_CONT_WID)
            | ((octBuf[1] & ~CONTINUE_MASK)   << 2 * CONTINUE_CONT_WID)
            | ((octBuf[0] & ~FOUR_OCTET_MASK) << 3 * CONTINUE_CONT_WID);
}

static inline
int lookaheadContinuations(const OctetType * const octbuf, int n)
    // Return the number of continuation octets beginning at the specified
    // '*octBuf', up to but not greater than the specified 'n'.  Note that a
    // null octet is not a continuation and is taken to end the scan.
{
    const OctetType *pc = octbuf;
    const OctetType * const end = pc + n;
    while (pc < end && isContinuation(*pc)) {
        ++pc;
    }
    return pc - octbuf;
}

static inline
bool fitsInSingleOctet(Iso10646Char uc)
    // Return 'true' if the specified Unicode value 'uc' can be coded in a
    // single UTF-8 octet.
{
    return 0 == (uc & (~Iso10646Char(0) << ONE_OCT_CONT_WID));
}

static inline
bool fitsInTwoOctets(Iso10646Char uc)
    // Return 'true' if the specified Unicode value 'uc' can be coded in two
    // UTF-8 octets or less.
{
    return 0 == (uc & (~Iso10646Char(0) << (TWO_OCT_CONT_WID +
                                                      CONTINUE_CONT_WID)));
}

static inline
bool fitsInThreeOctets(Iso10646Char uc)
    // Return 'true' if the specified Unicode value 'uc' can be coded in three
    // UTF-8 octets or less.
{
    return 0 == (uc & (~Iso10646Char(0) << (THREE_OCT_CONT_WID +
                                                  2 * CONTINUE_CONT_WID)));
}

static inline
bool fitsInFourOctets(Iso10646Char uc)
    // Return 'true' if the specified Unicode value 'uc' can be coded in four
    // UTF-8 octets or less.
{
    return 0 == (uc & (~Iso10646Char(0) << (FOUR_OCT_CONT_WID +
                                                  3 * CONTINUE_CONT_WID)));
}

static inline
void encodeTwoOctets(OctetType *octBuf, Iso10646Char isoBuf)
    // Encode the specified Unicode value 'isoBuf' into the two bytes pointed
    // at by the specified 'octBuf'.
{
    octBuf[1] = (OctetType) (CONTINUE_TAG | (isoBuf & ~CONTINUE_MASK));
    octBuf[0] = (OctetType) (TWO_OCTET_TAG |
                 ((isoBuf >>     CONTINUE_CONT_WID) & ~TWO_OCTET_MASK));
}

static inline
void encodeThreeOctets(OctetType *octBuf, Iso10646Char isoBuf)
    // Encode the specified Unicode value 'isoBuf' into the three bytes pointed
    // at by the specified 'octBuf'.
{
    octBuf[2] = (OctetType) (CONTINUE_TAG | (isoBuf & ~CONTINUE_MASK));
    octBuf[1] = (OctetType) (CONTINUE_TAG |
                 ((isoBuf >>     CONTINUE_CONT_WID) & ~CONTINUE_MASK));
    octBuf[0] = (OctetType) (THREE_OCTET_TAG |
                 ((isoBuf >> 2 * CONTINUE_CONT_WID) & ~THREE_OCTET_MASK));
}

static inline
void encodeFourOctets(OctetType *octBuf, Iso10646Char isoBuf)
    // Encode the specified Unicode value 'isoBuf' into the four bytes pointed
    // at by the specified 'octBuf'.
{
    octBuf[3] = (OctetType) (CONTINUE_TAG | (isoBuf & ~CONTINUE_MASK));
    octBuf[2] = (OctetType) (CONTINUE_TAG |
                 ((isoBuf >>     CONTINUE_CONT_WID) & ~CONTINUE_MASK));
    octBuf[1] = (OctetType) (CONTINUE_TAG |
                 ((isoBuf >> 2 * CONTINUE_CONT_WID) & ~CONTINUE_MASK));
    octBuf[0] = (OctetType) (FOUR_OCTET_TAG |
                 ((isoBuf >> 3 * CONTINUE_CONT_WID) & ~FOUR_OCTET_MASK));
}

static inline
bool isIllegal16BitValue(Iso10646Char uc)
    // Return 'true' if the specified Unicode value 'uc' is a value reserved
    // for the encoding of double-word planes in UTF-16 (such values are
    // illegal in ANY Unicode format).
{
    return uc >= 0xd800 && uc < 0xe000;
}

static inline
bool isIllegalFourOctetValue(Iso10646Char uc)
    // Return 'true' if the specified 32-bit value is too high to be
    // represented in Unicode.
{
    return uc > 0x10ffff;
}

static inline
bool isLegalUtf32ErrorChar(Iso10646Char uc)
    // Return 'true' if the specified 32-bit value is legal to be represented
    // in unicode.
{
    return uc < 0xd800 || (uc >= 0xe000 && uc <= 0x10ffff);
}

static inline
const OctetType *skipUtf8Character(const OctetType *input)
    // Return a pointer to the next Unicode character after the char in the
    // sequence beginning at 'input'.  Note that this routine is able to handle
    // illegal sequences.
{
    const OctetType uc = *input;

    if (isSingleOctet(uc)) {
        return input + 1;                                             // RETURN
    }

    int expected = isTwoOctetHeader(  uc) ? 1
                 : isThreeOctetHeader(uc) ? 2
                 : isFourOctetHeader( uc) ? 3
                 :                          4;
    ++input;
    return input + lookaheadContinuations(input, expected);
}

static
bsl::size_t utf32BufferLengthNeeded(const char *input)
    // Return the number of 'Iso10646Char's sufficient to store the
    // null-terminated utf-8 sequence beginning at the specified 'input',
    // including the terminating 0 word of the output.  Note that if the
    // translation occurs with 0 specified as the error character and errors
    // are present, this will be an over estimate, otherwise the result will be
    // completely precise.
{
    // Note that to exactly calculate the size would require much more detailed
    // decoding and would hence be much slower.  Also, the default is for the
    // error character not to be 0, in which case this estimate will be
    // completely precise.  Also, error sequences will usually range from
    // extremely rare to totally not present.  For these reasons, this faster,
    // less exact algorithm was chosen.

    const OctetType *octets = constOctetCast(input);

    bsl::size_t ret = 0;
    for (; *octets; ++ret) {
        octets = skipUtf8Character(octets);
    }

    return ret + 1;
}

static
bsl::size_t utf8BufferLengthNeeded(const Iso10646Char    *input,
                                   const OctetType        errorCharacter)
    // Return the length, in bytes, of the UTF-8 sequence needed to store the
    // translation of the utf32 sequence pointed at by 'input', including the
    // terminating 0, when using the specified 'errorCharacter'.  Note that
    // this estimate will always be completely precise.
{
    unsigned errorCharSize = !!errorCharacter;

    Iso10646Char uc;
    bsl::size_t ret = 0;
    for (; (uc = *input); ++input) {
        ret += fitsInSingleOctet(uc) ? 1
             : fitsInTwoOctets(  uc) ? 2
             : fitsInThreeOctets(uc) ? (isIllegal16BitValue(uc)
                                              ? errorCharSize
                                              : 3)
             : fitsInFourOctets( uc) ? (isIllegalFourOctetValue(uc)
                                              ? errorCharSize
                                              : 4)
             :                         errorCharSize;
    }

    return ret + 1;
}

namespace {
    // Second unnamed namespace -- the static routines that precede this use
    // constants from the first unnamed namespace.  Now we are declaring two
    // local classes, 'Utf8ToUtf32Translator' and 'Utf32ToUtf8Translator', that
    // use those static routines.  To keep the names of these local classes and
    // their functions private to this translation unit, we put them in the
    // unnamed namespace.

template <class CAPACITY>
class Utf8ToUtf32Translator {
    // This 'Class' is used to create an instance of an object that will be
    // used for translating a UTF-8 stream into UTF-32.  The template argument
    // 'CAPACITY' is to be either of the 'Capacity' or of the 'NoopCapacity'
    // type defined in the first unnamed namespace.  When the output is to be
    // to a buffer of fixed length that may not be adequate, the 'Capacity'
    // type is used and calculations are made incrementally to constantly check
    // if the end of the buffer has been reached.  If the destination buffer is
    // known ahead of time to be long enough, the 'NoopCapacity' type is to be
    // used, and then all the incremental capacity checking becomes inline
    // calls to no-op functions that will generate no code and waste no cpu
    // time.  All functions in this 'class' are private except for a single
    // static method, 'translate', which creates an object of this type and
    // calls manipulators on it.

    BSLMF_ASSERT((bsl::is_same<CAPACITY,     Capacity>::value ||
                  bsl::is_same<CAPACITY, NoopCapacity>::value));

    // DATA
    Iso10646Char     *d_output;
    CAPACITY          d_capacity;
    const OctetType  *d_input;
    Iso10646Char      d_errorChar;
    bool              d_invalidChars;

  private:
    // PRIVATE CREATORS
    Utf8ToUtf32Translator(Iso10646Char    *output,
                          bsl::size_t      capacity,
                          const OctetType *input,
                          Iso10646Char     errorChar);
        // Create a "Utf8ToUtf32Translator' object to translate UTF-8 from the
        // specified 'input' to be written as UTF-32 to the specified 'output',
        // which has the specified 'capacity' 'Iso10646Char's of room in it.
        // When an error is encountered in the input, output 'errorChar' to the
        // output unless 'errorChar' is 0, in which case no output
        // corresponding to the error sequence is generated.

    // PRIVATE MANIPULATORS
    void advance()
        // Update the output pointer and the capacity of this object to reflect
        // the fact that a word of output has been written.
    {
        ++d_output;
        --d_capacity;
    }

    int invalidChar()
        // Update the state of this object and possibly the output to reflect
        // that an error sequence was encountered in the input.  Return a
        // non-zero value if there was insufficient capacity for the output,
        // and 0 otherwise.
    {
        d_invalidChars = true;

        if (! d_errorChar) {
            return 0;                                                 // RETURN
        }
        else {
            if (d_capacity >= 2) {
                *d_output = d_errorChar;
                advance();
                return 0;                                             // RETURN
            }
            else {
                return -1;                                            // RETURN
            }
        }
    }

    int decodeCharacter(const OctetType uc);
        // Read one Unicode character of UTF-8 from the input stream 'd_input',
        // the first byte of which is the specified 'uc', and update the output
        // and the state of this object accordingly.  Return a non-zero value
        // if there was insufficient capacity for the output, and 0 otherwise.
        // The behavior is undefined unless 'uc' is non-zero, and unless at
        // least 1 word of space is available in the output buffer.

  public:
    // PUBLIC CLASS METHOD
    static
    int translate(Iso10646Char      *output,
                  bsl::size_t        capacity,
                  const char        *input,
                  bsl::size_t       *numCharsWritten,
                  Iso10646Char       errorChar);
        // Translate a UTF-8 stream from the specified null-terminated 'input'
        // to a UTF-32 stream written to the buffer at the specified 'output',
        // always null terminating the result.  If the template argument is
        // type 'Capacity', the output buffer is the specified 'capacity' words
        // long, and encode as many Unicode characters as will fit, including
        // the terminating null character.  If the template argument is type
        // 'NoopCapacity', "capacity' is ignored, the output buffer is assumed
        // to be long enough, and the entire UTF-32 sequence is to be
        // translated.  Write to the specified '*numCharsWritten' the number of
        // Unicode characters written, including the terminating null
        // character.  If the specified 'errorChar' is non-zero, write
        // 'errorChar' to the output every time an error character is
        // encountered in the input, otherwise write no output corresponding to
        // error characters in the input.  The behavior is undefined unless
        // 'CAPACITY' is 'NoopCapacity' or 'capacity > 0'.
};

template <class CAPACITY>
class Utf32ToUtf8Translator {
    // This 'class' is used to create an instance of an object that will be
    // used for translating a UTF-32 stream into UTF-8.  The template argument
    // 'CAPACITY' is to be either of the 'Capacity' or of the 'NoopCapacity'
    // types defined in the first unnamed namespace.  When the output is to be
    // to a buffer of fixed length that may not be of sufficient size, the
    // 'Capacity' type is used and calculations are made incrementally to
    // constantly check if the end of the buffer has been reached.  If the
    // destination buffer is known ahead of time to be long enough, the
    // 'NoopCapacity' type is to be used, and then all the incremental capacity
    // checking becomes inline calls to no-op functions that will generate no
    // code and waste no cpu time.  All functions in this 'class' are private
    // except for a single static method, 'translate', which creates an object
    // of this type and calls manipulators on it.

    BSLMF_ASSERT((bsl::is_same<CAPACITY,     Capacity>::value ||
                  bsl::is_same<CAPACITY, NoopCapacity>::value));

    // DATA
    OctetType          *d_output;
    CAPACITY            d_capacity;
    const Iso10646Char *d_input;
    const OctetType     d_errorChar;
    bsl::size_t         d_numCharsWritten;
    bool                d_invalidChars;

  private:
    // PRIVATE CREATOR
    Utf32ToUtf8Translator(OctetType          *output,
                          bsl::size_t         capacity,
                          const Iso10646Char *input,
                          const OctetType     errorChar);
        // Create a "Utf32ToUtf8Translator' object to translate UTF-32 from the
        // specified 'input' to be written as UTF-8 to the specified 'output'
        // buffer, which is the specified 'capacity' length in 'Iso10646Char's,
        // or is guaranteed to have adequate room for the translation if
        // "CAPACITY' is 'NoopCapacity'.  Initialize 'd_errorChar' to the
        // specified 'errorCar'.

    // PRIVATE MANIPULATORS
    void advance(unsigned delta)
        // Update the state of this object to reflect the fact that a single
        // Unicode character of the specified 'delta' bytes of UTF-8 output has
        // been written.
    {
        d_capacity -= delta;
        d_output   += delta;
        ++d_numCharsWritten;
    }

    int invalidChar()
        // Update this object and the output, if appropriate, to reflect the
        // fact that an error sequence has been encountered in the input.
        // Return a non-zero value if there was insufficient capacity for the
        // output, and 0 otherwise.
    {
        d_invalidChars = true;

        if (0 == d_errorChar) {
            return 0;                                                 // RETURN
        }

        if (d_capacity >= 2) {
            *d_output = d_errorChar;
            advance(1);
            return 0;                                                 // RETURN
        }
        else {
            return -1;                                                // RETURN
        }
    }

    int decodeCharacter(const Iso10646Char uc);
        // Translate the specified UTF-32 character 'uc' to UTF-8 in the output
        // stream, updating this object appropriately.  If insufficient space
        // exists in the output buffer for the character output plus a
        // terminating null character, fail without doing any output.  Return 0
        // if there was adequate space for the output, and a non-zero value
        // otherwise.  The behavior is undefined unless 'uc' is non-zero, and
        // there are at least 2 bytes of room in the output buffer.

  public:
    // PUBLIC CLASS METHOD
    static
    int translate(char               *output,
                  bsl::size_t         capacity,
                  const Iso10646Char *input,
                  bsl::size_t        *numCharsWritten,
                  bsl::size_t        *numBytesWritten,
                  const char          errorChar);
        // Translate a UTF-32 stream from the specified null-terminated 'input'
        // to a UTF-8 stream written to the buffer at the specified 'output',
        // always null terminating the result.  If the template argument is
        // type 'Capacity', the output buffer is the specified 'capacity' bytes
        // long, and encode as many Unicode characters as will fit, including
        // the terminating null character.  If the template argument is type
        // 'NoopCapacity', "capacity' is ignored, the output buffer is assumed
        // to be long enough, and the entire UTF-8 sequence is to be
        // translated.  Write to the specified '*numCharsWritten' the number of
        // Unicode characters written, including the terminating 0.  Write to
        // the specified '*numBytesWritten' the number of bytes of output
        // written, including the terminating null character.  If the specified
        // 'errorChar' is non-zero, write 'errorChar' to the output every time
        // an error sequence is encountered in the input, otherwise write no
        // output corresponding to error sequences in the input.  The behavior
        // is undefined unless 'CAPACITY' is 'NoopCapacity' or 'capacity > 0'.
};

                            // ---------------------
                            // Utf8ToUtf32Translator
                            // ---------------------

// PRIVATE CREATORS
template <class CAPACITY>
Utf8ToUtf32Translator<CAPACITY>::Utf8ToUtf32Translator(
    Iso10646Char       *output,
    bsl::size_t         capacity,
    const OctetType    *input,
    const Iso10646Char  errorChar)
: d_output(output)
, d_capacity(capacity)
, d_input(input)
, d_errorChar(errorChar)
, d_invalidChars(false)
{}

// PRIVATE MANIPULATORS
template <class CAPACITY>
int Utf8ToUtf32Translator<CAPACITY>::decodeCharacter(const OctetType uc)
{
    BSLS_ASSERT_SAFE(d_capacity >= 1);
    BSLS_ASSERT_SAFE(uc);

    int len;
    bool good;
    Iso10646Char newChar;
    if      (isSingleOctet(uc)) {
        BSLS_ASSERT_SAFE(uc);
        if (d_capacity >= 2) {
            *d_output = uc;
            advance();
            ++d_input;
            return 0;                                                 // RETURN
        }
        else {
            return -1;                                                // RETURN
        }
    }
    else if (isTwoOctetHeader(uc)) {
        len = lookaheadContinuations(d_input + 1, 1);
        good = 1 == len && ! fitsInSingleOctet(
                                           newChar = decodeTwoOctets(d_input));
    }
    else if (isThreeOctetHeader(uc)) {
        len = lookaheadContinuations(d_input + 1, 2);
        good = 2 == len && ! fitsInTwoOctets(
                                       newChar = decodeThreeOctets(d_input)) &&
                                               ! isIllegal16BitValue(newChar);
    }
    else if (isFourOctetHeader(uc)) {
        len = lookaheadContinuations(d_input + 1, 3);
        good = 3 == len && ! fitsInThreeOctets(
                                        newChar = decodeFourOctets(d_input)) &&
                                            ! isIllegalFourOctetValue(newChar);
    }
    else {
        ++d_input;
        d_input += lookaheadContinuations(d_input, 4);

        return invalidChar();                                         // RETURN
    }

    d_input += 1 + len;

    if (good) {
        if (d_capacity >= 2) {
            *d_output = newChar;
            advance();
            return 0;                                                 // RETURN
        }
        else {
            return -1;                                                // RETURN
        }
    }
    else {
        return invalidChar();                                         // RETURN
    }

    BSLS_ASSERT(0 && "reached unreachable statement");
}

template <class CAPACITY>
int Utf8ToUtf32Translator<CAPACITY>::translate(Iso10646Char *output,
                                               bsl::size_t   capacity,
                                               const char   *input,
                                               bsl::size_t  *numCharsWritten,
                                               Iso10646Char  errorChar)
{
    Utf8ToUtf32Translator<CAPACITY> translator(output,
                                               capacity,
                                               constOctetCast(input),
                                               errorChar);
    BSLS_ASSERT(translator.d_capacity >= 1);

    OctetType uc;
    int ret = 0;
    while ((uc = *translator.d_input)) {
        if (0 != translator.decodeCharacter(uc)) {
            BSLS_ASSERT((bsl::is_same<CAPACITY, Capacity>::value));
            ret = OUT_OF_SPACE_BIT;
            break;
        }
    }
    ++translator.d_input;
    BSLS_ASSERT_SAFE(translator.d_capacity >= 1);

    *translator.d_output = 0;
    translator.advance();

    bsl::size_t ncw = translator.d_output - output;
    *numCharsWritten = ncw;
    BSLS_ASSERT((bsl::is_same<CAPACITY, NoopCapacity>::value) ||
                                                              ncw <= capacity);
    BSLS_ASSERT(ncw <=
                   (bsl::size_t) (translator.d_input - constOctetCast(input)));

    if (translator.d_invalidChars) {
        ret |= INVALID_CHARS_BIT;
    }

    return ret;
}

                            // ---------------------
                            // Utf32ToUtf8Translator
                            // ---------------------

// PRIVATE CREATORS
template <class CAPACITY>
Utf32ToUtf8Translator<CAPACITY>::Utf32ToUtf8Translator(
    OctetType          *output,
    bsl::size_t         capacity,
    const Iso10646Char *input,
    const OctetType     errorChar)
: d_output(output)
, d_capacity(capacity)
, d_input(input)
, d_errorChar(errorChar)
, d_numCharsWritten(0)
, d_invalidChars(false)
{}

template <class CAPACITY>
int Utf32ToUtf8Translator<CAPACITY>::decodeCharacter(const Iso10646Char uc)
{
    BSLS_ASSERT_SAFE(d_capacity >= 1);
    BSLS_ASSERT_SAFE(uc);

    if      (fitsInSingleOctet(uc)) {
        if (d_capacity >= 2) {
            *d_output = (OctetType) uc;
            advance(1);
        }
        else {
            return -1;                                                // RETURN
        }
    }
    else if (fitsInTwoOctets(uc)) {
        if (d_capacity >= 3) {
            encodeTwoOctets(d_output, uc);
            advance(2);
        }
        else {
            return -1;                                                // RETURN
        }
    }
    else if (fitsInThreeOctets(uc)) {
        if (isIllegal16BitValue(uc)) {
            return invalidChar();                                     // RETURN
        }
        else {
            if (d_capacity >= 4) {
                encodeThreeOctets(d_output, uc);
                advance(3);
            }
            else {
                return -1;                                            // RETURN
            }
        }
    }
    else if (fitsInFourOctets(uc)) {
        if (isIllegalFourOctetValue(uc)) {
            return invalidChar();                                     // RETURN
        }
        else if (d_capacity >= 5) {
            encodeFourOctets(d_output, uc);
            advance(4);
        }
        else {
            return -1;                                                // RETURN
        }
    }
    else {
        return invalidChar();                                         // RETURN
    }

    return 0;
}

template <class CAPACITY>
int Utf32ToUtf8Translator<CAPACITY>::translate(
                                           char               *output,
                                           bsl::size_t         capacity,
                                           const Iso10646Char *input,
                                           bsl::size_t        *numCharsWritten,
                                           bsl::size_t        *numBytesWritten,
                                           const char          errorChar)
{
    BSLS_ASSERT(0 == (0x80 & errorChar));

    Utf32ToUtf8Translator<CAPACITY> translator(octetCast(output),
                                               capacity,
                                               input,
                                               errorChar);
    BSLS_ASSERT_SAFE(translator.d_capacity >= 1);

    int ret = 0;
    Iso10646Char uc;
    while ((uc = *translator.d_input++)) {
        if (0 != translator.decodeCharacter(uc)) {
            BSLS_ASSERT((bsl::is_same<CAPACITY, Capacity>::value));
            ret |= OUT_OF_SPACE_BIT;
            break;
        }
    }
    BSLS_ASSERT(translator.d_capacity >= 1);

    *translator.d_output = 0;
    translator.advance(1);

    *numCharsWritten = translator.d_numCharsWritten;

    *numBytesWritten = translator.d_output - octetCast(output);
    BSLS_ASSERT((bsl::is_same<CAPACITY, NoopCapacity>::value) ||
                                                 *numBytesWritten <= capacity);
    BSLS_ASSERT_SAFE((*numCharsWritten) * 4 - 3 >= *numBytesWritten);

    // Check for 0's in the middle of the output.

    BSLS_ASSERT_SAFE(bsl::strlen(output) == *numBytesWritten - 1);

    if (translator.d_invalidChars) {
        ret |= INVALID_CHARS_BIT;
    }

    return ret;
}

}  // close unnamed namespace

namespace BloombergLP {

                        // -----------------------------
                        // struct bdede_CharConvertUtf32
                        // -----------------------------

// CLASS METHODS
int bdede_CharConvertUtf32::utf8ToUtf32(
                                    bsl::vector<unsigned int> *dstVector,
                                    const char                *srcString,
                                    unsigned int               errorCharacter)
{
    BSLS_ASSERT(isLegalUtf32ErrorChar(errorCharacter));
    BSLS_ASSERT_SAFE(dstVector);
    BSLS_ASSERT_SAFE(srcString);

    typedef Utf8ToUtf32Translator<NoopCapacity> Translator;

    bsl::size_t bufferLen = utf32BufferLengthNeeded(srcString);
    BSLS_ASSERT(bufferLen > 0);
    dstVector->resize(bufferLen);

    bsl::size_t numWordsWritten;
    int ret = Translator::translate(&dstVector->front(),
                                    0,
                                    srcString,
                                    &numWordsWritten,
                                    errorCharacter);
    BSLS_ASSERT(!(ret & OUT_OF_SPACE_BIT));
    BSLS_ASSERT(numWordsWritten >= 1);
    if (bufferLen > numWordsWritten) {
        // 'bufferLen' should have been an exactly accurate estimate unless
        // '0 == errorCharacter' and invalid characters occurred.

        BSLS_ASSERT(0 == errorCharacter);
        BSLS_ASSERT(ret & INVALID_CHARS_BIT);
        dstVector->resize(numWordsWritten);
    }
    else {
        // 'bufferLen' should never be an underestimate.

        BSLS_ASSERT(bufferLen == numWordsWritten);

        // If the estimate was spot on, either 'errorCharacter' was non-zero,
        // or no invalid characters occurred.

        BSLS_ASSERT(0 != errorCharacter || 0 == (ret & INVALID_CHARS_BIT));
    }

    BSLS_ASSERT(0 == dstVector->back());

    // Verify that the translator embedded no null words in the output before
    // the end.

    BSLS_ASSERT_SAFE(bsl::find(&dstVector->front(),
                               &dstVector->front() + numWordsWritten,
                               0) == &dstVector->back());

    return ret;
}

int bdede_CharConvertUtf32::utf8ToUtf32(unsigned int *dstBuffer,
                                        bsl::size_t   dstCapacity,
                                        const char   *srcString,
                                        bsl::size_t  *numCharsWritten,
                                        unsigned int  errorCharacter)
{
    BSLS_ASSERT(isLegalUtf32ErrorChar(errorCharacter));
    BSLS_ASSERT_SAFE(dstBuffer);
    BSLS_ASSERT_SAFE(srcString);

    typedef Utf8ToUtf32Translator<Capacity> Translator;

    bsl::size_t ncw;
    if (0 == numCharsWritten) {
        numCharsWritten = &ncw;
    }
    if (0 == dstCapacity) {
        *numCharsWritten = 0;
        return OUT_OF_SPACE_BIT;                                      // RETURN
    }

    return Translator::translate(dstBuffer,
                                 dstCapacity,
                                 srcString,
                                 numCharsWritten,
                                 errorCharacter);
}

int bdede_CharConvertUtf32::utf32ToUtf8(bsl::string        *dstString,
                                        const unsigned int *srcString,
                                        bsl::size_t        *numCharsWritten,
                                        unsigned char       errorCharacter)
{
    BSLS_ASSERT(errorCharacter < 0x80);
    BSLS_ASSERT_SAFE(dstString);
    BSLS_ASSERT_SAFE(srcString);

    typedef Utf32ToUtf8Translator<NoopCapacity> Translator;

    bsl::size_t bufferLen = utf8BufferLengthNeeded(srcString,
                                                   errorCharacter);
    BSLS_ASSERT(bufferLen > 0);
    dstString->resize(bufferLen);
    BSLS_ASSERT_SAFE(dstString->length() == bufferLen);

    bsl::size_t numBytesWritten, ncw;
    if (!numCharsWritten) {
        numCharsWritten = &ncw;
    }
    char *begin = &dstString->front();
    int ret = Translator::translate(begin,
                                    0,
                                    srcString,
                                    numCharsWritten,
                                    &numBytesWritten,
                                    errorCharacter);
    BSLS_ASSERT(*numCharsWritten > 0);
    BSLS_ASSERT( numBytesWritten == bufferLen);
    BSLS_ASSERT(!(ret & OUT_OF_SPACE_BIT));
    BSLS_ASSERT(0 == (*dstString)[numBytesWritten - 1]);

    // The terminating '\0' is not strictly included in the 'length' of the
    // string, so resize down by 1 byte.

    dstString->resize(numBytesWritten - 1);

    return ret;
}

int bdede_CharConvertUtf32::utf32ToUtf8(bsl::vector<char>  *dstVector,
                                        const unsigned int *srcString,
                                        bsl::size_t        *numCharsWritten,
                                        unsigned char       errorCharacter)
{
    BSLS_ASSERT(errorCharacter < 0x80);
    BSLS_ASSERT_SAFE(dstVector);
    BSLS_ASSERT_SAFE(srcString);

    typedef Utf32ToUtf8Translator<NoopCapacity> Translator;

    bsl::size_t bufferLen = utf8BufferLengthNeeded(srcString,
                                                   errorCharacter);
    BSLS_ASSERT(bufferLen > 0);
    dstVector->resize(bufferLen);

    bsl::size_t numBytesWritten, ncw;
    if (!numCharsWritten) {
        numCharsWritten = &ncw;
    }
    char *begin = &dstVector->front();
    int ret = Translator::translate(begin,
                                    0,
                                    srcString,
                                    numCharsWritten,
                                    &numBytesWritten,
                                    errorCharacter);
    BSLS_ASSERT(*numCharsWritten > 0);
    BSLS_ASSERT( numBytesWritten > 0);
    BSLS_ASSERT(*numCharsWritten <= numBytesWritten);
    BSLS_ASSERT(0 == (ret & OUT_OF_SPACE_BIT));
    BSLS_ASSERT(numBytesWritten == bufferLen);
    BSLS_ASSERT(0 == dstVector->back());

    return ret;
}

int bdede_CharConvertUtf32::utf32ToUtf8(char               *dstBuffer,
                                        bsl::size_t         dstCapacity,
                                        const unsigned int *srcString,
                                        bsl::size_t        *numCharsWritten,
                                        bsl::size_t        *numBytesWritten,
                                        unsigned char       errorCharacter)
{
    BSLS_ASSERT(errorCharacter < 0x80);
    BSLS_ASSERT_SAFE(dstBuffer);
    BSLS_ASSERT_SAFE(srcString);

    bsl::size_t ncw, nbw;
    if (0 == numCharsWritten) {
        numCharsWritten = &ncw;
    }
    if (0 == numBytesWritten) {
        numBytesWritten = &nbw;
    }

    if (0 == dstCapacity) {
        *numCharsWritten = 0;
        *numBytesWritten = 0;
        return OUT_OF_SPACE_BIT;                                      // RETURN
    }

    typedef Utf32ToUtf8Translator<Capacity> Translator;

    int ret = Translator::translate(dstBuffer,
                                    dstCapacity,
                                    srcString,
                                    numCharsWritten,
                                    numBytesWritten,
                                    errorCharacter);
    ncw = *numCharsWritten;    // may be self-assign
    nbw = *numBytesWritten;    // may be self-assign

    BSLS_ASSERT_SAFE(ncw > 0);
    BSLS_ASSERT_SAFE(nbw > 0);
    BSLS_ASSERT_SAFE(ncw <= nbw);
    BSLS_ASSERT_SAFE(nbw <= dstCapacity);
    BSLS_ASSERT_SAFE(0 == dstBuffer[nbw - 1]);

    return ret;
}

}  // close enterprise namespace

// -------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE -------------------------------
