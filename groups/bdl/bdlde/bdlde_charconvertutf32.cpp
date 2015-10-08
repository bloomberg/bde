// bdlde_charconvertutf32.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlde_charconvertutf32.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlde_charconvertutf32_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>     // 'BSLMF_ASSERT'
#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_byteorderutil.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>    // 'bsl::find'

#include <bsl_climits.h>      // 'CHAR_BIT'
#include <bsl_cstring.h>

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
// '[0 .. 0xd7ff]' or in the range '[ 0xe000 .. 0x10ffff ]'.

namespace {

// TYPES
typedef unsigned char OctetType;
    // 'char' often defaults to a signed type, which becomes problematic in
    // widening to 32 bit values, so we use this unsigned type to store bytes
    // of UTF-8.

// Portability/sanity checks -- data type sizes.
BSLMF_ASSERT(8 == CHAR_BIT);
BSLMF_ASSERT(4 == sizeof(unsigned int));
BSLMF_ASSERT(sizeof(char) == sizeof(OctetType));

enum {
    // The return values of all the public functions defined by this component
    // return a bit-wise Or of these two flags.  When the output is to a
    // container rather than a fixed-length buffer, the 'k_OUT_OF_SPACE_BIT' is
    // never set.

    k_INVALID_CHARS_BIT =
                    BloombergLP::bdlde::CharConvertStatus::k_INVALID_CHARS_BIT,
    k_OUT_OF_SPACE_BIT  =
                    BloombergLP::bdlde::CharConvertStatus::k_OUT_OF_SPACE_BIT
};

enum Utf8Bits {
    // Masks and shifts used to assemble and dismantle UTF-8 encodings.

    k_ONE_OCT_CONT_WID   = 7,               // content in a one-octet coding
    k_ONE_OCTET_MASK     = 0xff & (~0 << k_ONE_OCT_CONT_WID),

    k_CONTINUE_CONT_WID  = 6,               // content in a continuation
                                          //                      octet
    k_CONTINUE_MASK      = 0xff & (~0 << k_CONTINUE_CONT_WID),
    k_CONTINUE_TAG       = k_ONE_OCTET_MASK,  // compare this to masked bits

    k_TWO_OCT_CONT_WID   = 5,               // content in a two-octet header
    k_TWO_OCTET_MASK     = 0xff & (~0 << k_TWO_OCT_CONT_WID),
    k_TWO_OCTET_TAG      = k_CONTINUE_MASK,   // compare this to masked bits

    k_THREE_OCT_CONT_WID = 4,               // content in a 3-octet header
    k_THREE_OCTET_MASK   = 0xff & (~0 << k_THREE_OCT_CONT_WID),
    k_THREE_OCTET_TAG    = k_TWO_OCTET_MASK,  // compare this to masked bits

    k_FOUR_OCT_CONT_WID  = 3,               // content in a four-octet header
    k_FOUR_OCTET_MASK    = 0xff & (~0 << k_FOUR_OCT_CONT_WID),
    k_FOUR_OCTET_TAG     = k_THREE_OCTET_MASK // compare this to masked bits
};

                           // =====================
                           // local struct Capacity
                           // =====================

struct Capacity {
    // Functor passed to 'Utf8ToUtf32Translator' and 'Utf32ToUtf8Translator' in
    // cases where we monitor capacity available in output.  Initialize in
    // c'tor with an integer 'capacity', then thereafter support operators
    // '--', '-=', and '<' for that value.

    bsl::size_t d_capacity;

    // CREATORS
    explicit
    Capacity(bsl::size_t capacity);
        // Create a 'Capacity' object with the specified 'capacity'.

    // MANIPULATORS
    void operator--();
        // Decrement 'd_capacity'.

    void operator-=(int delta);
        // Decrement 'd_capacity' by the specified 'delta'.

    // ACCESSORS
    bool operator<( bsl::size_t rhs) const;
        // Return 'true' if 'd_capacity' is less than the specified 'rhs', and
        // 'false' otherwise.

    bool operator>=(bsl::size_t rhs) const;
        // Return 'true' if 'd_capacity' is greater than or equal to the
        // specified 'rhs', and 'false' otherwise.
};

                           // ---------------------
                           // local struct Capacity
                           // ---------------------

// CREATOR
inline
Capacity::Capacity(bsl::size_t capacity)
: d_capacity(capacity)
{}

// MANIPULATORS
inline
void Capacity::operator--()
    // Decrement 'd_capacity'.
{
    --d_capacity;
}

inline
void Capacity::operator-=(int delta)
    // Decrement 'd_capacity' by 'delta'.
{
    d_capacity -= delta;
}

// ACCESSORS
inline
bool Capacity::operator<( bsl::size_t rhs) const
    // Return 'true' if 'd_capacity' is less than the specified 'rhs', and
    // 'false' otherwise.
{
    return d_capacity <  rhs;
}

inline
bool Capacity::operator>=(bsl::size_t rhs) const
    // Return 'true' if 'd_capacity' is greater than or equal to the specified
    // 'rhs', and 'false' otherwise.
{
    return d_capacity >= rhs;
}

                         // =========================
                         // local struct NoopCapacity
                         // =========================

struct NoopCapacity {
    // Functor passed to 'Utf8ToUtf32Translator' and 'Utf32ToUtf8Translator' in
    // cases where we don't want to monitor capacity available in output, all
    // operations on this object are to become no-ops, the '<' and '>='
    // operators return constant 'bool' values to indicate that adequate
    // capacity is available.

    // CREATORS
    explicit
    NoopCapacity(bsl::size_t);
        // Create empty object.

    // MANIPULATORS
    void operator--();
        // No-op.

    void operator-=(int);
        // No-op.

    // ACCESSORS
    bool operator<( bsl::size_t) const;
        // Return 'false'.

    bool operator>=(bsl::size_t) const;
        // Return 'true'.
};

                         // -------------------------
                         // local struct NoopCapacity
                         // -------------------------

// CREATOR
inline
NoopCapacity::NoopCapacity(bsl::size_t)
    // Create empty object.
{}

// MANIPULATORS
inline
void NoopCapacity::operator--()
    // No-op.
{}

inline
void NoopCapacity::operator-=(int)
    // No-op.
{}

// ACCESSORS
inline
bool NoopCapacity::operator<( bsl::size_t) const
    // Return 'false'.
{
    return false;
}

inline
bool NoopCapacity::operator>=(bsl::size_t) const
    // Return 'true'.
{
    return true;
}

                            // ====================
                            // local struct Swapper
                            // ====================

struct Swapper {
    // This 'struct' serves as a template argument.  The type is used for
    // reversing the byte order of 'unsigned int' values passed to 'swapBytes'.

    // CLASS METHODS
    static unsigned int swapBytes(unsigned int x);
        // Return the specified 'x' with its byte order reversed;
};

inline
unsigned int Swapper::swapBytes(unsigned int x)
{
    return BloombergLP::bsls::ByteOrderUtil::swapBytes(x);
}

                          // ========================
                          // local struct NoopSwapper
                          // ========================

struct NoopSwapper {
    // This 'struct' serves as a template argument, to be substituted for
    // 'Swapper' when swapping is not desired.  The type is used for returning
    // the value passed to 'swapBytes' without modification.  Note that the
    // function name and signature must match that of the function in
    // 'Swapper'.

    // CLASS METHODS
    static unsigned int swapBytes(unsigned int x);
        // Return the specified 'x' without modification.
};

inline
unsigned int NoopSwapper::swapBytes(unsigned int x)
{
    return x;
}

                        // ===========================
                        // local class Utf8PtrBasedEnd
                        // ===========================

class Utf8PtrBasedEnd {
    // This 'class' is initialized with a pointer to the end of input.  The
    // 'isFinished' function just determines whether input has reached that
    // end by comparing pointers.

    // DATA
    const OctetType * const d_end;

  public:
    // CREATORS
    explicit
    Utf8PtrBasedEnd(const char *end);
        // Create a 'Utf8PtrBasedEnd' object with the end at the specified
        // 'end'.

    // ACCESSORS
    bool isFinished(const OctetType *position) const;
        // Return 'true' if the specified 'position' is at the end of input and
        // 'false' otherwise.  The behavior is undefined unless
        // 'position <= d_end'.

    const OctetType *skipContinuations(const OctetType *octets,
                                       int              skipBy) const;
        // Return a pointer to after the specified 'skipBy' consecutive
        // continuation bytes following the specified 'octets' that are prior
        // to 'd_end'.  The behavior is undefined unless 'octets <= d_end'.

    bool verifyContinuations(const OctetType *octets, int n) const;
        // Return 'true' if there are at least the specified 'n' continuation
        // bytes beginning at the specified 'octets' and prior to 'd_end', and
        // 'false' otherwise.  The behavior is undefined if 'octets' is past
        // the end.  The behavior is undefined unless 'octets <= d_end'.
};

                        // ---------------------------
                        // local class Utf8PtrBasedEnd
                        // ---------------------------

// CREATORS
inline
Utf8PtrBasedEnd::Utf8PtrBasedEnd(const char *end)
: d_end(reinterpret_cast<const OctetType *>(end))
{}

// ACCESSORS
inline
bool Utf8PtrBasedEnd::isFinished(const OctetType *position) const
{
    if (position < d_end) {
        return false;                                                 // RETURN
    }
    else {
        BSLS_ASSERT_SAFE(d_end == position);
        return true;                                                  // RETURN
    }
}

inline
const OctetType *Utf8PtrBasedEnd::skipContinuations(
                                                 const OctetType *octets,
                                                 int              skipBy) const
{
    // This function is not called in the critical path.  It is called when it
    // is known that there are fewer continuation octets after 'octets' than
    // were expected, at least before 'd_end'.

    BSLS_ASSERT_SAFE(d_end >= octets);
    const OctetType *end = bsl::min(d_end, octets + skipBy);

    while (octets < end && (*octets & k_CONTINUE_MASK) == k_CONTINUE_TAG) {
        ++octets;
    }

    return octets;
}

inline
bool Utf8PtrBasedEnd::verifyContinuations(const OctetType *octets,
                                          int              n) const
{
    BSLS_ASSERT_SAFE(n >= 1);
    BSLS_ASSERT_SAFE(d_end >= octets);

    const OctetType *end = octets + n;
    if (end > d_end) {
        return false;                                                 // RETURN
    }

    do {
        if ((*octets & k_CONTINUE_MASK) != k_CONTINUE_TAG) {
            return false;                                             // RETURN
        }

        ++octets;
    } while (octets < end);

    return true;
}

                       // =============================
                       // local struct Utf8ZeroBasedEnd
                       // =============================

struct Utf8ZeroBasedEnd {
    // ACCESSORS
    bool isFinished(const OctetType *position) const;
        // Return 'true' if the specified 'position' is at the end of input,
        // and 'false' otherwise.

    const OctetType *skipContinuations(const OctetType *octets,
                                       int              skipBy) const;
        // Return a pointer to after up to the specified 'skipBy' consecutive
        // continuation bytes following the specified 'octets'.  The function
        // will skip over less than 'skipBy' octets if it encounters end of
        // input, or any non-continuation octets.  The behavior is undefined
        // unless 'octets' is before or at the end of input.

    bool verifyContinuations(const OctetType *octets, int n) const;
        // Return 'true' if there are at least the specified 'n' continuation
        // bytes beginning at the specified 'octets' and 'false' otherwise.
        // The behavior is undefined unless 'n >= 1'.
};

                       // -----------------------------
                       // local struct Utf8ZeroBasedEnd
                       // -----------------------------

// ACCESSORS
inline
bool Utf8ZeroBasedEnd::isFinished(const OctetType *position) const
{
    return 0 == *position;
}

inline
const OctetType *Utf8ZeroBasedEnd::skipContinuations(
                                                 const OctetType *octets,
                                                 int              skipBy) const
{
    // This function is not called in the critical path.  It is called when it
    // is known that there are not as many continuation octets after 'octets'
    // as were expected.  Note that if '0 == *octets' the loop will stop.

    const OctetType *end = octets + skipBy;

    while (octets < end && (*octets & k_CONTINUE_MASK) == k_CONTINUE_TAG) {
        ++octets;
    }

    return octets;
}

inline
bool Utf8ZeroBasedEnd::verifyContinuations(const OctetType *octets,
                                           int              n) const
{
    BSLS_ASSERT_SAFE(n >= 1);

    const OctetType *end = octets + n;
    do {
        if ((*octets & k_CONTINUE_MASK) != k_CONTINUE_TAG) {
            return false;                                             // RETURN
        }

        ++octets;
    } while (octets < end);

    return true;
}

}  // close unnamed namespace

static inline
const OctetType *constOctetCast(const char *ptr)
    // Return the specified 'ptr' cast to a 'const OctetType *'.  Note that
    // 'static_cast' does not work here, and the idea is to be sure in these
    // casts that one is never accidentally casting between pointers to 'char'
    // or 'OctetType' and pointers to 'unsigned int'.
{
    BSLMF_ASSERT(sizeof(*ptr) == sizeof(OctetType));
    return reinterpret_cast<const OctetType *>(ptr);
}

static inline
OctetType *octetCast(char *ptr)
    // Return the specified 'ptr' cast to an 'OctetType *'.  Note that
    // 'static_cast' does not work here, and the idea is to be sure in these
    // casts that one is never accidentally casting between pointers to 'char'
    // or 'OctetType' and pointers to 'unsigned int'.
{
    BSLMF_ASSERT(sizeof(*ptr) == sizeof(OctetType));
    return reinterpret_cast<OctetType *>(ptr);
}

static inline
bool isSingleOctet(OctetType oct)
    // Return 'true' if the specified 'oct' is a single-octet UTF-8 sequence.
{
    return ! (oct & k_ONE_OCTET_MASK);
}

static inline
bool isContinuation(OctetType oct)
    // Return 'true' if the specified 'oct' is a continuation octet and 'false'
    // otherwise.
{
    return (oct & k_CONTINUE_MASK) == k_CONTINUE_TAG;
}

static inline
bool isTwoOctetHeader(OctetType oct)
    // Return 'true' if the specified 'oct' is the first octet of a two-octet
    // UTF-8 sequence and 'false' otherwise.
{
    return (oct & k_TWO_OCTET_MASK) == k_TWO_OCTET_TAG;
}

static inline
bool isThreeOctetHeader(OctetType oct)
    // Return 'true' if the specified 'oct' is the first octet of a three-octet
    // UTF-8 sequence and 'false' otherwise.
{
    return (oct & k_THREE_OCTET_MASK) == k_THREE_OCTET_TAG;
}

static inline
bool isFourOctetHeader(OctetType oct)
    // Return 'true' if the specified 'oct' is the first octet of a four-octet
    // UTF-8 sequence and 'false' otherwise.
{
    return (oct & k_FOUR_OCTET_MASK) == k_FOUR_OCTET_TAG;
}

static inline
unsigned int decodeTwoOctets(const OctetType *octBuf)
    // Return the value of the two-octet sequence that begins with the octet
    // pointed to by the specified 'octBuf'.
{
    return     (octBuf[1] & ~k_CONTINUE_MASK)
            | ((octBuf[0] & ~k_TWO_OCTET_MASK)   <<     k_CONTINUE_CONT_WID);
}

static inline
unsigned int decodeThreeOctets(const OctetType *octBuf)
    // Return the value of the three-octet sequence that begins with the octet
    // pointed to by the specified 'octBuf'.
{
    return     (octBuf[2] & ~k_CONTINUE_MASK)
            | ((octBuf[1] & ~k_CONTINUE_MASK)    <<     k_CONTINUE_CONT_WID)
            | ((octBuf[0] & ~k_THREE_OCTET_MASK) << 2 * k_CONTINUE_CONT_WID);
}

static inline
unsigned int decodeFourOctets(const OctetType *octBuf)
    // Return the value of the four-octet sequence that begins with the octet
    // pointed to by the specified 'octBuf'.
{
    return     (octBuf[3] & ~k_CONTINUE_MASK)
            | ((octBuf[2] & ~k_CONTINUE_MASK)   <<     k_CONTINUE_CONT_WID)
            | ((octBuf[1] & ~k_CONTINUE_MASK)   << 2 * k_CONTINUE_CONT_WID)
            | ((octBuf[0] & ~k_FOUR_OCTET_MASK) << 3 * k_CONTINUE_CONT_WID);
}

static inline
int lookaheadContinuations(const OctetType * const octBuf, int n)
    // Return the number of continuation octets beginning at the specified
    // 'octBuf', up to but not greater than the specified 'n'.  Note that a
    // null octet is not a continuation and is taken to end the scan.
{
    const OctetType *       pc  = octBuf;
    const OctetType * const end = pc + n;
    while (pc < end && isContinuation(*pc)) {
        ++pc;
    }
    return pc - octBuf;
}

static inline
bool fitsInSingleOctet(unsigned int uc)
    // Return 'true' if the specified Unicode value 'uc' can be coded in a
    // single UTF-8 octet and 'false' otherwise.
{
    return 0 == (uc & (~ (unsigned int) 0 << k_ONE_OCT_CONT_WID));
}

static inline
bool fitsInTwoOctets(unsigned int uc)
    // Return 'true' if the specified Unicode value 'uc' can be coded in two
    // UTF-8 octets or less and 'false' otherwise.
{
    return 0 == (uc & (~ (unsigned int) 0 << (k_TWO_OCT_CONT_WID +
                                                      k_CONTINUE_CONT_WID)));
}

static inline
bool fitsInThreeOctets(unsigned int uc)
    // Return 'true' if the specified Unicode value 'uc' can be coded in three
    // UTF-8 octets or less and 'false' otherwise.
{
    return 0 == (uc & (~ (unsigned int) 0 << (k_THREE_OCT_CONT_WID +
                                                  2 * k_CONTINUE_CONT_WID)));
}

static inline
bool fitsInFourOctets(unsigned int uc)
    // Return 'true' if the specified Unicode value 'uc' can be coded in four
    // UTF-8 octets or less and 'false' otherwise.
{
    return 0 == (uc & (~ (unsigned int) 0 << (k_FOUR_OCT_CONT_WID +
                                                  3 * k_CONTINUE_CONT_WID)));
}

static inline
void encodeTwoOctets(OctetType *octBuf, unsigned int isoBuf)
    // Encode the specified Unicode value 'isoBuf' into the two bytes pointed
    // at by the specified 'octBuf'.
{
    octBuf[1] = (OctetType) (k_CONTINUE_TAG | (isoBuf & ~k_CONTINUE_MASK));
    octBuf[0] = (OctetType) (k_TWO_OCTET_TAG |
                 ((isoBuf >>     k_CONTINUE_CONT_WID) & ~k_TWO_OCTET_MASK));
}

static inline
void encodeThreeOctets(OctetType *octBuf, unsigned int isoBuf)
    // Encode the specified Unicode value 'isoBuf' into the three bytes pointed
    // at by the specified 'octBuf'.
{
    octBuf[2] = (OctetType) (k_CONTINUE_TAG | (isoBuf & ~k_CONTINUE_MASK));
    octBuf[1] = (OctetType) (k_CONTINUE_TAG |
                 ((isoBuf >>     k_CONTINUE_CONT_WID) & ~k_CONTINUE_MASK));
    octBuf[0] = (OctetType) (k_THREE_OCTET_TAG |
                 ((isoBuf >> 2 * k_CONTINUE_CONT_WID) & ~k_THREE_OCTET_MASK));
}

static inline
void encodeFourOctets(OctetType *octBuf, unsigned int isoBuf)
    // Encode the specified Unicode value 'isoBuf' into the four bytes pointed
    // at by the specified 'octBuf'.
{
    octBuf[3] = (OctetType) (k_CONTINUE_TAG | (isoBuf & ~k_CONTINUE_MASK));
    octBuf[2] = (OctetType) (k_CONTINUE_TAG |
                 ((isoBuf >>     k_CONTINUE_CONT_WID) & ~k_CONTINUE_MASK));
    octBuf[1] = (OctetType) (k_CONTINUE_TAG |
                 ((isoBuf >> 2 * k_CONTINUE_CONT_WID) & ~k_CONTINUE_MASK));
    octBuf[0] = (OctetType) (k_FOUR_OCTET_TAG |
                 ((isoBuf >> 3 * k_CONTINUE_CONT_WID) & ~k_FOUR_OCTET_MASK));
}

static inline
bool isIllegal16BitValue(unsigned int uc)
    // Return 'true' if the specified Unicode value 'uc' is a value reserved
    // for the encoding of double-word planes in UTF-16 (such values are
    // illegal in ANY Unicode format) and 'false' otherwise.
{
    return uc >= 0xd800 && uc < 0xe000;
}

static inline
bool isIllegalFourOctetValue(unsigned int uc)
    // Return 'true' if the specified 32-bit value 'uc' is too high to be
    // represented in Unicode and 'false' otherwise.
{
    return uc > 0x10ffff;
}

static inline
bool isLegalUtf32ErrorChar(unsigned int uc)
    // Return 'true' if the specified 32-bit value 'uc' is legal to be
    // represented in unicode and 'false' otherwise.
{
    return uc < 0xd800 || (uc >= 0xe000 && uc <= 0x10ffff);
}

static inline
const OctetType *skipUtf8Character(const OctetType *input)
    // Return a pointer to the next Unicode character after the character in
    // the sequence beginning at the specified 'input'.  Note that an
    // incomplete sequence is skipped as a single char, and that any first byte
    // that is neither a single byte nor a header of a valid UTF-8 sequence is
    // interpreted as a 5-byte header.
{
    const OctetType uc = *input;

    if (isSingleOctet(uc)) {
        return input + 1;                                             // RETURN
    }

    int expected = isTwoOctetHeader(uc)
                   ? 1
                   : isThreeOctetHeader(uc)
                     ? 2
                     : isFourOctetHeader(uc)
                       ? 3
                       : 4;
    ++input;
    return input + lookaheadContinuations(input, expected);
}

template <class END_FUNCTOR>
static
bsl::size_t utf32BufferLengthNeeded(const char  *input,
                                    END_FUNCTOR  endFunctor)
    // Return the number of 'unsigned int's sufficient to store the UTF-8
    // sequence beginning at the specified 'input', including the terminating 0
    // word of the output.  Use the specified 'endFunctor' to determine end of
    // input.  Note that if the translation occurs with 0 specified as the
    // error character and errors are present, this will be an over-estimate,
    // otherwise the result will be exact.  Also note that the end of 'input'
    // is determined by 'endFunctor', which will treat the input as either
    // null-terminated or fixed-length.
{
    // Note that to exactly calculate the size would require much more detailed
    // decoding and would hence be much slower.  Also, the default is for the
    // error character not to be 0, in which case this estimate will be exact.
    // Also, error sequences will usually range from extremely rare to totally
    // not present.  For these reasons, this faster, less exact algorithm was
    // chosen.

    BSLMF_ASSERT((bsl::is_same<END_FUNCTOR,  Utf8PtrBasedEnd>::value ||
                  bsl::is_same<END_FUNCTOR, Utf8ZeroBasedEnd>::value));

    const OctetType *octets = constOctetCast(input);

    bsl::size_t ret = 0;
    for (; ! endFunctor.isFinished(octets); ++ret) {
        octets = skipUtf8Character(octets);
    }

    return ret + 1;
}

template <class SWAPPER>
static
bsl::size_t utf8BufferLengthNeeded(const unsigned int *input,
                                   const OctetType     errorCharacter)
    // Return the length, in bytes, of the UTF-8 sequence required to store the
    // translation of the UTF-32 sequence pointed at by the specified 'input',
    // including the terminating 0, when using the specified 'errorCharacter'.
    // Use the specified 'SWAPPER' to perform swapping, or not perform
    // swapping, as desired (see detailed doc in 'Utf8ToUtf32Translator' and
    // 'Utf32ToUtf8Translator' below).  Note that this estimate will always be
    // exact.
{
    BSLMF_ASSERT((bsl::is_same<SWAPPER,     Swapper>::value ||
                  bsl::is_same<SWAPPER, NoopSwapper>::value));

    unsigned errorCharSize = !!errorCharacter;

    unsigned int uc;
    bsl::size_t ret = 0;
    for (; (uc = SWAPPER::swapBytes(*input)); ++input) {
        ret += fitsInSingleOctet(uc)
               ? 1
               : fitsInTwoOctets(uc)
                 ? 2
                 : fitsInThreeOctets(uc)
                   ? (isIllegal16BitValue(uc)
                      ? errorCharSize
                      : 3)
                   : fitsInFourOctets(uc)
                     ? (isIllegalFourOctetValue(uc)
                        ? errorCharSize
                        : 4)
                     : errorCharSize;
    }

    return ret + 1;
}

namespace {
    // Second unnamed namespace -- the static routines that precede this use
    // constants from the first unnamed namespace.  Now we are declaring two
    // local classes, 'Utf8ToUtf32Translator' and 'Utf32ToUtf8Translator', that
    // use those static routines.

                     // =================================
                     // local class Utf8ToUtf32Translator
                     // =================================

template <class CAPACITY, class END_FUNCTOR, class SWAPPER>
class Utf8ToUtf32Translator {
    // This 'class' is used to create an instance of an object that will be
    // used for translating a UTF-8 stream into UTF-32.  All functions in this
    // 'class' are private except for a single static method, 'translate',
    // which creates an object of this type and calls manipulators on it.
    //
    // The template argument 'CAPACITY' is to be either of the 'Capacity' or of
    // the 'NoopCapacity' type defined in the first unnamed namespace.  When
    // the output is to a buffer of fixed length that may not be adequate, the
    // 'Capacity' type is used and calculations are made incrementally to
    // constantly check if the end of the buffer has been reached.  If the
    // destination buffer is known ahead of time to be long enough, the
    // 'NoopCapacity' type should be used, and then all the incremental
    // capacity checking becomes inline calls to no-op functions that will
    // generate no code and waste no CPU time.
    //
    // The template argument 'END_FUNCTOR' is used for determining the end of
    // input.  If the input is null terminated, 'Utf8ZeroBasedEnd' is used for
    // 'END_FUNCTOR'; if the input is of fixed length, an instance of
    // 'Utf8PtrBasedEnd', which contains a pointer to the end of the string, is
    // used.
    //
    // The template argument 'SWAPPER' is use for handling the byte order of
    // the UTF-32 output.  If the output is in host byte order, the
    // 'NoopSwapper' type is used, which does no swapping.  If the output is in
    // the opposite of host byte order, the 'Swapper' type is used, which will
    // swap the byte order of words as they are output.

    BSLMF_ASSERT((bsl::is_same<CAPACITY,            Capacity>::value ||
                  bsl::is_same<CAPACITY,        NoopCapacity>::value));

    BSLMF_ASSERT((bsl::is_same<END_FUNCTOR,  Utf8PtrBasedEnd>::value ||
                  bsl::is_same<END_FUNCTOR, Utf8ZeroBasedEnd>::value));

    BSLMF_ASSERT((bsl::is_same<SWAPPER,              Swapper>::value ||
                  bsl::is_same<SWAPPER,          NoopSwapper>::value));

    // DATA
    unsigned int     *d_output;
    CAPACITY          d_capacity;
    END_FUNCTOR       d_endFunctor;
    const OctetType  *d_input;
    unsigned int      d_swappedErrorChar;   // 'SWAPPER::swap' is applied to
                                            // the 'errorChar' arg to the c'tor
                                            // and the result stored to this
                                            // variable.
    bool              d_invalidChars;

  private:
    // PRIVATE CREATORS
    Utf8ToUtf32Translator(unsigned int    *output,
                          bsl::size_t      capacity,
                          END_FUNCTOR      endFunctor,
                          const OctetType *input,
                          unsigned int     errorChar);
        // Create a 'Utf8ToUtf32Translator' object to translate UTF-8 from the
        // specified 'input' to be written as UTF-32 to the specified 'output',
        // that has the specified 'capacity' 'unsigned int's of room in it.
        // Use the specified 'endFunctor' for evaluating continuation octets
        // and determining end of input.  When an error is encountered in the
        // input, output the specified 'errorChar' to the output unless
        // 'errorChar' is 0, in which case no output corresponding to the error
        // sequence is generated.  Note that 'errorChar' is assumed to be in
        // host byte order.

    // PRIVATE MANIPULATORS
    void advanceOutput();
        // Update the output pointer and the capacity of this object to reflect
        // the fact that a word of output has been written.

    void handleInvalidChar();
        // Update the state of this object and possibly the output to reflect
        // that an error sequence was encountered in the input.  The behavior
        // is undefined unless 'd_capacity >= 2'.

    int decodeCharacter();
        // Read one Unicode character of UTF-8 from the input stream 'd_input',
        // and update the output and the state of this object accordingly.
        // Return a non-zero value if there was insufficient capacity for the
        // output, and 0 otherwise.  The behavior is undefined unless at least
        // 1 word of space is available in the output buffer.

  public:
    // CLASS METHODS
    static
    int translate(unsigned int      *output,
                  bsl::size_t        capacity,
                  END_FUNCTOR        endFunctor,
                  const char        *input,
                  bsl::size_t       *numCharsWritten,
                  unsigned int       errorChar);
        // Translate a UTF-8 stream from the specified null-terminated 'input'
        // to a UTF-32 stream written to the buffer at the specified 'output',
        // always null-terminating the result.  If the template argument is
        // type 'Capacity', the output buffer is the specified 'capacity' words
        // long, and encode as many Unicode characters as will fit, including
        // the terminating null character.  If the template argument is type
        // 'NoopCapacity', 'capacity' is ignored, the output buffer is assumed
        // to be long enough, and the entire UTF-32 sequence is to be
        // translated.  Use the specified 'endFunctor' to determine end of
        // input.  Write to the specified '*numCharsWritten' the number of
        // Unicode characters written, including the terminating null
        // character.  If the specified 'errorChar' is non-zero, write
        // 'errorChar' to the output every time an error character is
        // encountered in the input; otherwise write no output corresponding to
        // error characters in the input.  The behavior is undefined unless
        // 'CAPACITY' is 'NoopCapacity' or 'capacity > 0'.
};

                     // ---------------------------------
                     // local class Utf8ToUtf32Translator
                     // ---------------------------------

// PRIVATE CREATORS
template <class CAPACITY, class END_FUNCTOR, class SWAPPER>
inline
Utf8ToUtf32Translator<CAPACITY, END_FUNCTOR, SWAPPER>::Utf8ToUtf32Translator(
                                                   unsigned int    *output,
                                                   bsl::size_t      capacity,
                                                   END_FUNCTOR      endFunctor,
                                                   const OctetType *input,
                                                   unsigned int     errorChar)
: d_output(output)
, d_capacity(capacity)
, d_endFunctor(endFunctor)
, d_input(input)
, d_swappedErrorChar(SWAPPER::swapBytes(errorChar))
, d_invalidChars(false)
{}

// PRIVATE MANIPULATORS
template <class CAPACITY, class END_FUNCTOR, class SWAPPER>
inline
void Utf8ToUtf32Translator<CAPACITY, END_FUNCTOR, SWAPPER>::advanceOutput()
{
    ++d_output;
    --d_capacity;
}

template <class CAPACITY, class END_FUNCTOR, class SWAPPER>
int Utf8ToUtf32Translator<CAPACITY, END_FUNCTOR, SWAPPER>::decodeCharacter()
{
    BSLS_ASSERT_SAFE(d_capacity >= 1);

    OctetType    firstOctet = *d_input;
    unsigned int decodedChar;
    int          len;
    bool         good;

    if (d_capacity < 2) {
        return -1;                                                    // RETURN
    }

    if      (isSingleOctet(     firstOctet)) {
        len = 1;
        good = true;
        decodedChar = firstOctet;
    }
    else if (isTwoOctetHeader(  firstOctet)) {
        len = 2;
        good = d_endFunctor.verifyContinuations(d_input + 1, 1) &&
              ! fitsInSingleOctet(decodedChar = decodeTwoOctets(d_input));
    }
    else if (isThreeOctetHeader(firstOctet)) {
        len = 3;
        good = d_endFunctor.verifyContinuations(d_input + 1, 2) &&
              ! fitsInTwoOctets(  decodedChar = decodeThreeOctets(d_input)) &&
                                        ! isIllegal16BitValue(decodedChar);
    }
    else if (isFourOctetHeader( firstOctet)) {
        len = 4;
        good = d_endFunctor.verifyContinuations(d_input + 1, 3) &&
              ! fitsInThreeOctets(decodedChar = decodeFourOctets(d_input)) &&
                                        ! isIllegalFourOctetValue(decodedChar);
    }
    else {
        len = 5;
        good = false;
    }

    if (good) {
        d_input += len;

        *d_output = SWAPPER::swapBytes(decodedChar);
        advanceOutput();
    }
    else {
        d_input = d_endFunctor.skipContinuations(d_input + 1, len - 1);

        handleInvalidChar();
    }

    return 0;
}

template <class CAPACITY, class END_FUNCTOR, class SWAPPER>
inline
void Utf8ToUtf32Translator<CAPACITY, END_FUNCTOR, SWAPPER>::handleInvalidChar()
{
    BSLS_ASSERT_SAFE(d_capacity >= 2);

    d_invalidChars = true;

    if (d_swappedErrorChar) {
        *d_output = d_swappedErrorChar;
        advanceOutput();
    }
}

// CLASS METHODS
template <class CAPACITY, class END_FUNCTOR, class SWAPPER>
int Utf8ToUtf32Translator<CAPACITY, END_FUNCTOR, SWAPPER>::translate(
                                                 unsigned int *output,
                                                 bsl::size_t   capacity,
                                                 END_FUNCTOR   endFunctor,
                                                 const char   *input,
                                                 bsl::size_t  *numCharsWritten,
                                                 unsigned int  errorChar)
{
    Utf8ToUtf32Translator<CAPACITY, END_FUNCTOR, SWAPPER> translator(
                                                         output,
                                                         capacity,
                                                         endFunctor,
                                                         constOctetCast(input),
                                                         errorChar);
    BSLS_ASSERT(translator.d_capacity >= 1);

    int ret = 0;
    while (!endFunctor.isFinished(translator.d_input)) {
        if (0 != translator.decodeCharacter()) {
            BSLS_ASSERT((bsl::is_same<CAPACITY, Capacity>::value));
            ret = k_OUT_OF_SPACE_BIT;
            break;
        }
    }
    ++translator.d_input;
    BSLS_ASSERT_SAFE(translator.d_capacity >= 1);

    *translator.d_output = 0;
    translator.advanceOutput();

    bsl::size_t ncw = translator.d_output - output;
    *numCharsWritten = ncw;
    BSLS_ASSERT((bsl::is_same<CAPACITY, NoopCapacity>::value) ||
                                                              ncw <= capacity);
    BSLS_ASSERT(ncw <=
                   (bsl::size_t) (translator.d_input - constOctetCast(input)));

    if (translator.d_invalidChars) {
        ret |= k_INVALID_CHARS_BIT;
    }

    return ret;
}

                     // =================================
                     // local class Utf32ToUtf8Translator
                     // =================================

template <class CAPACITY, class SWAPPER>
class Utf32ToUtf8Translator {
    // This 'class' is used to create an instance of an object that will be
    // used for translating a UTF-32 stream into UTF-8.  All functions in this
    // 'class' are private except for a single static method, 'translate',
    // which creates an object of this type and calls manipulators on it.
    //
    // The template argument 'CAPACITY' is either 'Capacity' or 'NoopCapacity'
    // defined in the first unnamed namespace (above).  When the output is to a
    // buffer of fixed length that may not be of sufficient size, the
    // 'Capacity' type is used and calculations are made incrementally to
    // constantly check if the end of the buffer has been reached.  If the
    // destination buffer is known ahead of time to be long enough, the
    // 'NoopCapacity' type is used, and then all the incremental capacity
    // checking becomes inline calls to no-op functions that will generate no
    // code and waste no CPU time.
    //
    // The template argument 'SWAPPER' is used for dealing with the byte order
    // of the UTF-32 input.  If the input is in host byte order, a template
    // argument of 'NoopSwapper' is used and the input is not swapped.  If the
    // input is in the opposite of host byte order, a template argument of
    // 'Swapper' is used, which will swap the byte order of every 32-bit word
    // input to host byte order during the translator's internal processing.

    BSLMF_ASSERT((bsl::is_same<CAPACITY,     Capacity>::value ||
                  bsl::is_same<CAPACITY, NoopCapacity>::value));

    BSLMF_ASSERT((bsl::is_same<SWAPPER,       Swapper>::value ||
                  bsl::is_same<SWAPPER,   NoopSwapper>::value));

    // DATA
    OctetType          *d_output;
    CAPACITY            d_capacity;
    const unsigned int *d_input;
    const OctetType     d_errorChar;
    bsl::size_t         d_numCharsWritten;
    bool                d_invalidChars;

  private:
    // PRIVATE CREATOR
    Utf32ToUtf8Translator(OctetType          *output,
                          bsl::size_t         capacity,
                          const unsigned int *input,
                          const OctetType     errorChar);
        // Create a 'Utf32ToUtf8Translator' object to translate UTF-32 from the
        // specified 'input' to be written as UTF-8 to the specified 'output'
        // buffer that is the specified 'capacity' length in 'unsigned int's,
        // or is guaranteed to have adequate room for the translation if
        // 'CAPACITY' is 'NoopCapacity'.  Initialize 'd_errorChar' to the
        // specified 'errorChar'.

    // PRIVATE MANIPULATORS
    void advance(unsigned delta);
        // Update the state of this object to reflect the fact that a single
        // Unicode character of the specified 'delta' bytes of UTF-8 output has
        // been written.

    int handleInvalidChar();
        // Update this object and the output, if appropriate, to reflect the
        // fact that an error sequence has been encountered in the input.
        // Return a non-zero value if there was insufficient capacity for the
        // output, and 0 otherwise.

    int decodeCharacter(const unsigned int uc);
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
                  const unsigned int *input,
                  bsl::size_t        *numCharsWritten,
                  bsl::size_t        *numBytesWritten,
                  const char          errorChar);
        // Translate a UTF-32 stream from the specified null-terminated 'input'
        // to a UTF-8 stream written to the buffer at the specified 'output',
        // always null terminating the result.  If the template argument is
        // type 'Capacity', the output buffer is the specified 'capacity' bytes
        // long, and encode as many Unicode characters as will fit, including
        // the terminating null character.  If the template argument is type
        // 'NoopCapacity', 'capacity' is ignored, the output buffer is assumed
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

                     // ---------------------------------
                     // local class Utf32ToUtf8Translator
                     // ---------------------------------

// PRIVATE CREATORS
template <class CAPACITY, class SWAPPER>
inline
Utf32ToUtf8Translator<CAPACITY, SWAPPER>::Utf32ToUtf8Translator(
                                                 OctetType          *output,
                                                 bsl::size_t         capacity,
                                                 const unsigned int *input,
                                                 const OctetType     errorChar)
: d_output(output)
, d_capacity(capacity)
, d_input(input)
, d_errorChar(errorChar)
, d_numCharsWritten(0)
, d_invalidChars(false)
{}

// PRIVATE MANIPULATORS
template <class CAPACITY, class SWAPPER>
inline
void Utf32ToUtf8Translator<CAPACITY, SWAPPER>::advance(unsigned delta)
{
    d_capacity -= delta;
    d_output   += delta;
    ++d_numCharsWritten;
}

template <class CAPACITY, class SWAPPER>
inline
int Utf32ToUtf8Translator<CAPACITY, SWAPPER>::handleInvalidChar()
{
    d_invalidChars = true;

    if (0 == d_errorChar) {
        return 0;                                                     // RETURN
    }

    if (d_capacity >= 2) {
        *d_output = d_errorChar;
        advance(1);
        return 0;                                                     // RETURN
    }
    else {
        return -1;                                                    // RETURN
    }
}

template <class CAPACITY, class SWAPPER>
int Utf32ToUtf8Translator<CAPACITY, SWAPPER>::decodeCharacter(
                                                         const unsigned int uc)
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
            return handleInvalidChar();                               // RETURN
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
            return handleInvalidChar();                               // RETURN
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
        return handleInvalidChar();                                   // RETURN
    }

    return 0;
}

// CLASS METHODS
template <class CAPACITY, class SWAPPER>
int Utf32ToUtf8Translator<CAPACITY, SWAPPER>::translate(
                                           char               *output,
                                           bsl::size_t         capacity,
                                           const unsigned int *input,
                                           bsl::size_t        *numCharsWritten,
                                           bsl::size_t        *numBytesWritten,
                                           const char          errorChar)
{
    BSLS_ASSERT(0 == (0x80 & errorChar));

    Utf32ToUtf8Translator<CAPACITY, SWAPPER> translator(octetCast(output),
                                                        capacity,
                                                        input,
                                                        errorChar);
    BSLS_ASSERT_SAFE(translator.d_capacity >= 1);

    int          ret = 0;
    unsigned int uc;
    while ((uc = SWAPPER::swapBytes(*translator.d_input++))) {
        if (0 != translator.decodeCharacter(uc)) {
            BSLS_ASSERT((bsl::is_same<CAPACITY, Capacity>::value));
            ret |= k_OUT_OF_SPACE_BIT;
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
        ret |= k_INVALID_CHARS_BIT;
    }

    return ret;
}

}  // close unnamed namespace

namespace BloombergLP {
namespace bdlde {

                          // -----------------------
                          // struct CharConvertUtf32
                          // -----------------------

                                  // UTF8 to UTF32

// CLASS METHODS
int CharConvertUtf32::utf8ToUtf32(bsl::vector<unsigned int> *dstVector,
                                  const char                *srcString,
                                  unsigned int               errorCharacter,
                                  ByteOrder::Enum            byteOrder)
{
    BSLS_ASSERT(dstVector);
    BSLS_ASSERT(srcString);
    BSLS_ASSERT(isLegalUtf32ErrorChar(errorCharacter));

    typedef Utf8ToUtf32Translator<NoopCapacity,
                                  Utf8ZeroBasedEnd,
                                  Swapper> SwapTranslator;
    typedef Utf8ToUtf32Translator<NoopCapacity,
                                  Utf8ZeroBasedEnd,
                                  NoopSwapper> NoSwapTranslator;
    Utf8ZeroBasedEnd endFunctor;

    bsl::size_t bufferLen = utf32BufferLengthNeeded(srcString,
                                                    endFunctor);
    BSLS_ASSERT_SAFE(bufferLen > 0);
    dstVector->resize(bufferLen);

    bsl::size_t numWordsWritten;

    int ret = ByteOrder::e_HOST == byteOrder
              ? NoSwapTranslator::translate(&dstVector->front(),
                                            0,
                                            endFunctor,
                                            srcString,
                                            &numWordsWritten,
                                            errorCharacter)
              : SwapTranslator::translate(&dstVector->front(),
                                          0,
                                          endFunctor,
                                          srcString,
                                          &numWordsWritten,
                                          errorCharacter);

    // All asserts after this point are internal consistency checks that should
    // never fail in production, so they are for testing purposes only.

    BSLS_ASSERT_SAFE(!(ret & k_OUT_OF_SPACE_BIT));
    BSLS_ASSERT_SAFE(numWordsWritten >= 1);
    if (bufferLen > numWordsWritten) {
        // 'bufferLen' should have been an exactly accurate estimate unless
        // '0 == errorCharacter' and invalid characters occurred.

        BSLS_ASSERT_SAFE(0 == errorCharacter);
        BSLS_ASSERT_SAFE(ret & k_INVALID_CHARS_BIT);
        dstVector->resize(numWordsWritten);
    }
    else {
        // 'bufferLen' should never be an underestimate.

        BSLS_ASSERT_SAFE(bufferLen == numWordsWritten);

        // If the estimate was spot on, either 'errorCharacter' was non-zero,
        // or no invalid characters occurred.

        BSLS_ASSERT_SAFE(0 != errorCharacter ||
                                             0 == (ret & k_INVALID_CHARS_BIT));
    }

    BSLS_ASSERT_SAFE(0 == dstVector->back());

    // Verify that the translator embedded no null words in the output before
    // the end.

    BSLS_ASSERT_SAFE(bsl::find(&dstVector->front(),
                               &dstVector->front() + numWordsWritten,
                               0) == &dstVector->back());

    return ret;
}

int CharConvertUtf32::utf8ToUtf32(bsl::vector<unsigned int> *dstVector,
                                  const bslstl::StringRef&   srcString,
                                  unsigned int               errorCharacter,
                                  ByteOrder::Enum            byteOrder)
{
    BSLS_ASSERT(dstVector);
    BSLS_ASSERT(srcString.begin());
    BSLS_ASSERT(isLegalUtf32ErrorChar(errorCharacter));

    typedef Utf8ToUtf32Translator<NoopCapacity,
                                  Utf8PtrBasedEnd,
                                  Swapper> SwapTranslator;
    typedef Utf8ToUtf32Translator<NoopCapacity,
                                  Utf8PtrBasedEnd,
                                  NoopSwapper> NoSwapTranslator;
    Utf8PtrBasedEnd endFunctor(srcString.end());

    bsl::size_t bufferLen = utf32BufferLengthNeeded(srcString.begin(),
                                                    endFunctor);
    BSLS_ASSERT_SAFE(bufferLen > 0);
    dstVector->resize(bufferLen);

    bsl::size_t numWordsWritten;

    int ret = ByteOrder::e_HOST == byteOrder
              ? NoSwapTranslator::translate(&dstVector->front(),
                                            0,
                                            endFunctor,
                                            srcString.begin(),
                                            &numWordsWritten,
                                            errorCharacter)
              : SwapTranslator::translate(&dstVector->front(),
                                          0,
                                          endFunctor,
                                          srcString.begin(),
                                          &numWordsWritten,
                                          errorCharacter);

    // All asserts after this point are internal consistency checks that should
    // never fail in production, so they are for testing purposes only.

    BSLS_ASSERT_SAFE(!(ret & k_OUT_OF_SPACE_BIT));
    BSLS_ASSERT_SAFE(numWordsWritten >= 1);
    if (bufferLen > numWordsWritten) {
        // 'bufferLen' should have been an exactly accurate estimate unless
        // '0 == errorCharacter' and invalid characters occurred.

        BSLS_ASSERT_SAFE(0 == errorCharacter);
        BSLS_ASSERT_SAFE(ret & k_INVALID_CHARS_BIT);
        dstVector->resize(numWordsWritten);
    }
    else {
        // 'bufferLen' should never be an underestimate.

        BSLS_ASSERT_SAFE(bufferLen == numWordsWritten);

        // If the estimate was spot on, either 'errorCharacter' was non-zero,
        // or no invalid characters occurred.

        BSLS_ASSERT_SAFE(0 != errorCharacter ||
                                             0 == (ret & k_INVALID_CHARS_BIT));
    }

    BSLS_ASSERT_SAFE(0 == dstVector->back());

    return ret;
}

int CharConvertUtf32::utf8ToUtf32(unsigned int          *dstBuffer,
                                  bsl::size_t            dstCapacity,
                                  const char            *srcString,
                                  bsl::size_t           *numCharsWritten,
                                  unsigned int           errorCharacter,
                                  ByteOrder::Enum        byteOrder)
{
    BSLS_ASSERT(dstBuffer);
    BSLS_ASSERT(srcString);
    BSLS_ASSERT(isLegalUtf32ErrorChar(errorCharacter));

    typedef Utf8ToUtf32Translator<Capacity,
                                  Utf8ZeroBasedEnd,
                                  Swapper> SwapTranslator;
    typedef Utf8ToUtf32Translator<Capacity,
                                  Utf8ZeroBasedEnd,
                                  NoopSwapper> NoSwapTranslator;

    bsl::size_t localNumCharsWritten;
    if (0 == numCharsWritten) {
        numCharsWritten = &localNumCharsWritten;
    }
    if (0 == dstCapacity) {
        *numCharsWritten = 0;
        return k_OUT_OF_SPACE_BIT;                                    // RETURN
    }

    return ByteOrder::e_HOST == byteOrder
           ? NoSwapTranslator::translate(dstBuffer,
                                         dstCapacity,
                                         Utf8ZeroBasedEnd(),
                                         srcString,
                                         numCharsWritten,
                                         errorCharacter)
           : SwapTranslator::translate(dstBuffer,
                                       dstCapacity,
                                       Utf8ZeroBasedEnd(),
                                       srcString,
                                       numCharsWritten,
                                       errorCharacter);
}

int CharConvertUtf32::utf8ToUtf32(unsigned int               *dstBuffer,
                                  bsl::size_t                 dstCapacity,
                                  const bslstl::StringRef&    srcString,
                                  bsl::size_t                *numCharsWritten,
                                  unsigned int                errorCharacter,
                                  ByteOrder::Enum             byteOrder)
{
    BSLS_ASSERT(dstBuffer);
    BSLS_ASSERT(srcString.begin());
    BSLS_ASSERT(isLegalUtf32ErrorChar(errorCharacter));

    typedef Utf8ToUtf32Translator<Capacity,
                                  Utf8PtrBasedEnd,
                                  Swapper> SwapTranslator;
    typedef Utf8ToUtf32Translator<Capacity,
                                  Utf8PtrBasedEnd,
                                  NoopSwapper> NoSwapTranslator;
    Utf8PtrBasedEnd endFunctor(srcString.end());

    bsl::size_t localNumCharsWritten;
    if (0 == numCharsWritten) {
        numCharsWritten = &localNumCharsWritten;
    }
    if (0 == dstCapacity) {
        *numCharsWritten = 0;
        return k_OUT_OF_SPACE_BIT;                                    // RETURN
    }

    return ByteOrder::e_HOST == byteOrder
           ? NoSwapTranslator::translate(dstBuffer,
                                         dstCapacity,
                                         endFunctor,
                                         srcString.begin(),
                                         numCharsWritten,
                                         errorCharacter)
           : SwapTranslator::translate(dstBuffer,
                                       dstCapacity,
                                       endFunctor,
                                       srcString.begin(),
                                       numCharsWritten,
                                       errorCharacter);
}

                                // UTF32 to UTF8

int CharConvertUtf32::utf32ToUtf8(bsl::string           *dstString,
                                  const unsigned int    *srcString,
                                  bsl::size_t           *numCharsWritten,
                                  unsigned char          errorCharacter,
                                  ByteOrder::Enum        byteOrder)
{
    BSLS_ASSERT(dstString);
    BSLS_ASSERT(srcString);
    BSLS_ASSERT(errorCharacter < 0x80);

    typedef Utf32ToUtf8Translator<NoopCapacity, NoopSwapper> NoSwapTranslator;
    typedef Utf32ToUtf8Translator<NoopCapacity,     Swapper>   SwapTranslator;

    bsl::size_t bufferLen = ByteOrder::e_HOST == byteOrder
                            ? utf8BufferLengthNeeded<NoopSwapper>(
                                                                srcString,
                                                                errorCharacter)
                            : utf8BufferLengthNeeded<Swapper>(srcString,
                                                              errorCharacter);
    BSLS_ASSERT_SAFE(bufferLen > 0);
    dstString->resize(bufferLen);
    BSLS_ASSERT_SAFE(dstString->length() == bufferLen);

    bsl::size_t numBytesWritten, localNumCharsWritten;
    if (!numCharsWritten) {
        numCharsWritten = &localNumCharsWritten;
    }
    char *begin = &dstString->front();
    int   ret = ByteOrder::e_HOST == byteOrder
                ? NoSwapTranslator::translate(begin,
                                              0,
                                              srcString,
                                              numCharsWritten,
                                              &numBytesWritten,
                                              errorCharacter)
                : SwapTranslator::translate(begin,
                                            0,
                                            srcString,
                                            numCharsWritten,
                                            &numBytesWritten,
                                            errorCharacter);

    // The following are internal consistency checks that should never fail in
    // production, so they are for testing purposes only.

    BSLS_ASSERT_SAFE(*numCharsWritten > 0);
    BSLS_ASSERT_SAFE( numBytesWritten == bufferLen);
    BSLS_ASSERT_SAFE(!(ret & k_OUT_OF_SPACE_BIT));
    BSLS_ASSERT_SAFE(0 == (*dstString)[numBytesWritten - 1]);

    // There are two '\0's in 'dstString->c_str()' beginning at char
    // 'dstString->length() - 1' -- adjust 'length' to reflect the earlier
    // '\0'.

    dstString->resize(numBytesWritten - 1);

    return ret;
}

int CharConvertUtf32::utf32ToUtf8(bsl::vector<char>        *dstVector,
                                        const unsigned int *srcString,
                                        bsl::size_t        *numCharsWritten,
                                        unsigned char       errorCharacter,
                                        ByteOrder::Enum     byteOrder)
{
    BSLS_ASSERT(dstVector);
    BSLS_ASSERT(srcString);
    BSLS_ASSERT(errorCharacter < 0x80);

    typedef Utf32ToUtf8Translator<NoopCapacity,     Swapper>   SwapTranslator;
    typedef Utf32ToUtf8Translator<NoopCapacity, NoopSwapper> NoSwapTranslator;

    bsl::size_t bufferLen = ByteOrder::e_HOST == byteOrder
                            ? utf8BufferLengthNeeded<NoopSwapper>(
                                                                srcString,
                                                                errorCharacter)
                            : utf8BufferLengthNeeded<Swapper>(srcString,
                                                              errorCharacter);
    BSLS_ASSERT_SAFE(bufferLen > 0);
    dstVector->resize(bufferLen);

    bsl::size_t numBytesWritten, localNumCharsWritten;
    if (!numCharsWritten) {
        numCharsWritten = &localNumCharsWritten;
    }
    char *begin = &dstVector->front();
    int   ret   = ByteOrder::e_HOST == byteOrder
                  ? NoSwapTranslator::translate(begin,
                                                0,
                                                srcString,
                                                numCharsWritten,
                                                &numBytesWritten,
                                                errorCharacter)
                  : SwapTranslator::translate(begin,
                                              0,
                                              srcString,
                                              numCharsWritten,
                                              &numBytesWritten,
                                              errorCharacter);

    // The following are internal consistency checks that should never fail in
    // production, so they are for testing purposes only.

    BSLS_ASSERT_SAFE(*numCharsWritten > 0);
    BSLS_ASSERT_SAFE( numBytesWritten > 0);
    BSLS_ASSERT_SAFE(*numCharsWritten <= numBytesWritten);
    BSLS_ASSERT_SAFE(0 == (ret & k_OUT_OF_SPACE_BIT));
    BSLS_ASSERT_SAFE(numBytesWritten == bufferLen);
    BSLS_ASSERT_SAFE(0 == dstVector->back());

    return ret;
}

int CharConvertUtf32::utf32ToUtf8(char                  *dstBuffer,
                                  bsl::size_t            dstCapacity,
                                  const unsigned int    *srcString,
                                  bsl::size_t           *numCharsWritten,
                                  bsl::size_t           *numBytesWritten,
                                  unsigned char          errorCharacter,
                                  ByteOrder::Enum        byteOrder)
{
    BSLS_ASSERT(dstBuffer);
    BSLS_ASSERT(srcString);
    BSLS_ASSERT(errorCharacter < 0x80);

    bsl::size_t localNumCharsWritten, localNumBytesWritten;
    if (0 == numCharsWritten) {
        numCharsWritten = &localNumCharsWritten;
    }
    if (0 == numBytesWritten) {
        numBytesWritten = &localNumBytesWritten;
    }

    if (0 == dstCapacity) {
        *numCharsWritten = 0;
        *numBytesWritten = 0;
        return k_OUT_OF_SPACE_BIT;                                    // RETURN
    }

    typedef Utf32ToUtf8Translator<Capacity,     Swapper>   SwapTranslator;
    typedef Utf32ToUtf8Translator<Capacity, NoopSwapper> NoSwapTranslator;

    int ret = ByteOrder::e_HOST == byteOrder
              ? NoSwapTranslator::translate(dstBuffer,
                                            dstCapacity,
                                            srcString,
                                            numCharsWritten,
                                            numBytesWritten,
                                            errorCharacter)
              : SwapTranslator::translate(dstBuffer,
                                          dstCapacity,
                                          srcString,
                                          numCharsWritten,
                                          numBytesWritten,
                                          errorCharacter);

    // The following are internal consistency checks that should never fail in
    // production, so they are for testing purposes only.

    BSLS_ASSERT_SAFE(*numCharsWritten > 0);
    BSLS_ASSERT_SAFE(*numBytesWritten > 0);
    BSLS_ASSERT_SAFE(*numCharsWritten <= *numBytesWritten);
    BSLS_ASSERT_SAFE(*numBytesWritten <= dstCapacity);
    BSLS_ASSERT_SAFE(0 == dstBuffer[*numBytesWritten - 1]);

    return ret;
}

}  // close package namespace
}  // close enterprise namespace

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
