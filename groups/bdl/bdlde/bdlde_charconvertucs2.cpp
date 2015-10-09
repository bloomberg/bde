// bdlde_charconvertucs2.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlde_charconvertucs2.h>

#include <bdlde_charconvertstatus.h>

#include <bsls_performancehint.h>  // 'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY'

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

///IMPLEMENTATION NOTES
///--------------------
// This UTF-8 documentation was copied verbatim from RFC 3629.  The original
// version was downloaded from:
//..
//     http://tools.ietf.org/html/rfc3629
//..
///////////////////////////// VERBATIM RFC TEXT /////////////////////////////
// UTF-8 encodes UCS characters as a varying number of octets, where the number
// of octets, and the value of each, depend on the integer value assigned to
// the character in ISO/IEC 10646 (the character number, a.k.a. code position,
// code point or Unicode scalar value).  This encoding form has the following
// characteristics (all values are in hexadecimal):
//
// o  Character numbers from U+0000 to U+007F (US-ASCII repertoire) correspond
//    to octets 00 to 7F (7 bit US-ASCII values).  A direct consequence is that
//    a plain ASCII string is also a valid UTF-8 string.
//
// o  US-ASCII octet values do not appear otherwise in a UTF-8 encoded
//    character stream.  This provides compatibility with file systems or other
//    software (e.g., the printf() function in C libraries) that parse based on
//    US-ASCII values but are transparent to other values.
//
// o  Round-trip conversion is easy between UTF-8 and other encoding forms.
//
// o  The first octet of a multi-octet sequence indicates the number of octets
//    in the sequence.
//
// o  The octet values C0, C1, F5 to FF never appear.
//
// o  Character boundaries are easily found from anywhere in an octet stream.
//
// o  The byte-value lexicographic sorting order of UTF-8 strings is the same
//    as if ordered by character numbers.  Of course this is of limited
//    interest since a sort order based on character numbers is almost never
//    culturally valid.
//
// o  The Boyer-Moore fast search algorithm can be used with UTF-8 data.
//
// o  UTF-8 strings can be fairly reliably recognized as such by a simple
//    algorithm, i.e., the probability that a string of characters in any other
//    encoding appears as valid UTF-8 is low, diminishing with increasing
//    string length.
//
// UTF-8 was devised in September 1992 by Ken Thompson, guided by design
// criteria specified by Rob Pike, with the objective of defining a UCS
// transformation format usable in the Plan9 operating system in a non-
// disruptive manner.  Thompson's design was stewarded through standardization
// by the X/Open Joint Internationalization Group XOJIG (see [FSS_UTF]),
// bearing the names FSS-UTF (variant FSS/UTF), UTF-2 and finally UTF-8 along
// the way.
//
// .....
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
//..
// --------------------+---------------------------------------------
// 0000 0000-0000 007F | 0xxxxxxx
// 0000 0080-0000 07FF | 110xxxxx 10xxxxxx
// 0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
// 0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
//..
///////////////////////////// VERBATIM RFC TEXT /////////////////////////////

// Utility bitmasks for decoding UTF-8 octets.
enum {
    RANGE1_BITS =  0xC0,
    RANGE1_MASK = ~0x1F,

    RANGE2_BITS =  0xE0,
    RANGE2_MASK = ~0x0F,

    // We can't deal with range 3 - it's past the UCS-2 domain.  The following
    // masks and shifts are included to correctly process range 3 characters as
    // single errors.

    RANGE3_BITS =  0xF0,
    RANGE3_MASK = ~0x07,

    EXTRA_BITS  =  0x80, // 0b10xxxxxx
    EXTRA_MASK  =  0x3F  // low 6 bits
};

// Utility bitmasks for encoding UTF-8 octets.
enum {
    // 1-octet characters.
    ONE_OCTET_CHARACTER_UPPER_BOUND = 0x7f,

    // 2-octet characters.
    TWO_OCTET_CHARACTER_UPPER_BOUND = 0x7ff,
    RANGE1_BYTE1_MASK      =  0x1F,
    RANGE1_BYTE1_SHIFT     =  6,

    // 3-octet characters.
    THREE_OCTET_CHARACTER_UPPER_BOUND = 0xffff,
    RANGE2_BYTE1_MASK      =  0x0F,
    RANGE2_BYTE1_SHIFT     =  12,
    RANGE2_BYTE2_SHIFT     =  6,

    // 4-octet characters - we can't deal with range 3 - it's past the UCS-2
    // domain.  The following masks and shifts are included to correctly
    // process range 3 characters as single errors.
    RANGE3_BYTE1_MASK      =  0x07,
    RANGE3_BYTE1_SHIFT     =  18,
    RANGE3_BYTE2_SHIFT     =  12,
    RANGE3_BYTE3_SHIFT     =  6,

    // Each multi-octet character uses this mask and marker for the trailing
    // octets.
    EXTRA_BYTE_MASK        =  0x3F,
    EXTRA_BYTE_MARKER_MASK =  0xC0 // 0b11000000
};

// These return code values need to be kept synchronized with the header
// documentation.

enum {
    k_SUCCESS                 = 0,
    k_INVALID_INPUT_CHARACTER =
                    BloombergLP::bdlde::CharConvertStatus::k_INVALID_CHARS_BIT,
    k_OUTPUT_BUFFER_TOO_SMALL =
                    BloombergLP::bdlde::CharConvertStatus::k_OUT_OF_SPACE_BIT
};

namespace {

// LOCAL HELPER FUNCTION
void insertUcs2Character(unsigned short **dstBufferPtr,
                         bsl::size_t     *dstCapacityPtr,
                         bsl::size_t     *charsWrittenPtr,
                         unsigned short   character)
    // If 'character' is non-0, insert the UCS-2 (16-bit) 'character' into the
    // location pointed to by the specified '**dstBufferPtr', and bump
    // '*dstBufferPtr' forward, increment the specified '*charsWrittenPtr', and
    // decrement the specified '*dstCapacityPtr'.  If 'character' is 0, this
    // function has no effect (in order to support the "skipping" behavior of
    // utf8ToUcs2 for 0 'errorCharacter' arguments).
{
    // If the invalid character placeholder is '\0', we "skip" bad characters.
    // This routine is not used for null termination.

    if (!character) {
        return;                                                       // RETURN
    }

    **dstBufferPtr = character;
    ++*dstBufferPtr;
    --*dstCapacityPtr;
    ++*charsWrittenPtr;
}

// LOCAL HELPER FUNCTION
void skipBadUtf8Data(unsigned short      **dstBufferPtr,
                     bsl::size_t          *dstCapacityPtr,
                     const unsigned char **srcPtr,
                     bsl::size_t          *charsWrittenPtr,
                     unsigned short        errorCharacter,
                     int                  *retCode,
                     int                   octetsToSkip)
    // Skip past the specified 'octetsToSkip' invalid characters in the
    // specified '*srcPtr', setting the 'k_INVALID_INPUT_CHARACTER' bit in the
    // specified '*retCode', and calling 'insertUcs2Character' to update the
    // specified '*dstBufferPtr' with the specified 'errorCharacter', and
    // updating '*dstCapacityPtr' and the specified 'charsWrittenPtr'.
{
    *srcPtr  += octetsToSkip;

    *retCode |= k_INVALID_INPUT_CHARACTER;

    insertUcs2Character(dstBufferPtr,
                        dstCapacityPtr,
                        charsWrittenPtr,
                        errorCharacter);
}

// LOCAL HELPER FUNCTION
void convertUtf8ToUcs2(unsigned short      **dstBufferPtr,
                       bsl::size_t          *dstCapacityPtr,
                       const unsigned char **srcPtr,
                       bsl::size_t          *charsWrittenPtr,
                       unsigned short        errorCharacter,
                       int                  *retCode)
    // Convert one multi-octet UTF-8 character to one UCS-2 character, if the
    // multi-octet sequence is valid.  If the sequence is invalid, insert
    // the specified 'errorCharacter' instead, and turn on the
    // 'k_INVALID_INPUT_CHARACTER' bit in the specified '*retCode'.
{
    const unsigned char *src       = *srcPtr;
    unsigned char        firstByte = *src;

    // Check whether 'firstByte' indicates a 2-octet character.

    if ((firstByte & RANGE1_MASK) == RANGE1_BITS) {
        // Check to make sure that the next byte is indeed part of an extended
        // character

        if ((EXTRA_BYTE_MARKER_MASK & src[1]) != EXTRA_BITS) {
            // This is not a valid character.  Skip one octet forward and
            // output 'errorCharacter'

            skipBadUtf8Data(dstBufferPtr,
                        dstCapacityPtr,
                        srcPtr,
                        charsWrittenPtr,
                        errorCharacter,
                        retCode,
                        1);

            return;                                                   // RETURN
        }

        // We have confirmed that these 2 octets do make up a valid UCS-2
        // character - compute the character taking the higher bits from
        // firstByte, and the lower bits from src[1].

        insertUcs2Character(dstBufferPtr,
                            dstCapacityPtr,
                            charsWrittenPtr,
                            static_cast<unsigned short>(
                        ((firstByte & RANGE1_BYTE1_MASK) << RANGE1_BYTE1_SHIFT)
                 | (src[1] & EXTRA_BYTE_MASK)));

        // Skip forward 2 octets.

        *srcPtr+=2;

        return;                                                       // RETURN
    }

    // 'firstByte' does not indicate a 2-octet character.  Check whether
    // 'firstByte' indicates a 3-octet character

    if ((firstByte & RANGE2_MASK) == RANGE2_BITS) {
        // Check to make sure that the second byte is indeed part of an
        // extended character.

        if ((EXTRA_BYTE_MARKER_MASK & src[1]) != EXTRA_BITS) {
            // This is not a valid character.  Skip one octet forward and
            // output 'errorCharacter'

            skipBadUtf8Data(dstBufferPtr,
                            dstCapacityPtr,
                            srcPtr,
                            charsWrittenPtr,
                            errorCharacter,
                            retCode,
                            1);

            return;                                                   // RETURN
        }

        // Check to make sure that the third byte is indeed part of an extended
        // character.

        if ((EXTRA_BYTE_MARKER_MASK & src[2]) != EXTRA_BITS) {
            // This is not a valid character.  Skip over the first 2 octets and
            // output 'errorCharacter'

            skipBadUtf8Data(dstBufferPtr,
                            dstCapacityPtr,
                            srcPtr,
                            charsWrittenPtr,
                            errorCharacter,
                            retCode,
                            2);

            return;                                                   // RETURN
        }

        // We have got a valid 3-octet character.  Get the highest bits from
        // byte 0, the middle bits from byte 1, and the lowest bits from byte
        // 2.

        insertUcs2Character(dstBufferPtr,
                            dstCapacityPtr,
                            charsWrittenPtr,
                            static_cast<unsigned short>(
                   ((firstByte & RANGE2_BYTE1_MASK) << RANGE2_BYTE1_SHIFT)
                 | ((src[1]    & EXTRA_BYTE_MASK)   << RANGE2_BYTE2_SHIFT)
                 |  (src[2]    & EXTRA_BYTE_MASK)));

        // Skip forward 3 octets.

        *srcPtr += 3;

        return;                                                       // RETURN
    }

    // 'firstByte' does not indicate a 2 or 3-octet character.  Check whether
    // 'firstByte' indicates a 4-octet character (which will be handled as a
    // single error.

    if ((firstByte & RANGE3_MASK) == RANGE3_BITS) {
        // This is either a complete or partial 4-octet character.  We can't
        // convert such characters (their values exceed 0xFFFF), but we should
        // skip them as single characters.

        int octetsToSkip = 1;

        // This might be 1, 2, 3, or 4 bytes of a 4-octet character.  Handle
        // all 4 possibilities.

        if ((EXTRA_BYTE_MARKER_MASK & src[1]) == EXTRA_BITS) {
            ++octetsToSkip;

            if ((EXTRA_BYTE_MARKER_MASK & src[2]) == EXTRA_BITS) {
                ++octetsToSkip;

                if ((EXTRA_BYTE_MARKER_MASK & src[3]) == EXTRA_BITS) {
                    ++octetsToSkip;
                }
            }
        }

        // This is not a UCS-2-convertible character.  Skip over the first
        // 'octetsToSkip' and output 'errorCharacter'

        skipBadUtf8Data(dstBufferPtr,
                        dstCapacityPtr,
                        srcPtr,
                        charsWrittenPtr,
                        errorCharacter,
                        retCode,
                        octetsToSkip);

        return;                                                       // RETURN
    }

    // If we get here, we have got something we can't handle - 'firstByte' is
    // not correct for a 2, 3, or 4-octet character.

    *retCode |= k_INVALID_INPUT_CHARACTER;

    insertUcs2Character(dstBufferPtr,
                        dstCapacityPtr,
                        charsWrittenPtr,
                        errorCharacter);

    // Since we got something we can't handle, we have to skip forward 1 octet.

    ++*srcPtr;
}

// LOCAL HELPER FUNCTION
void nullFillRemainingBuffer(unsigned char **dstBufferPtr,
                             bsl::size_t    *dstCapacityPtr,
                             bsl::size_t    *charsWrittenPtr,
                             bsl::size_t    *bytesWrittenPtr)
    // Called when a multi-octet character would not allow enough space in the
    // buffer for a null terminator.  We fill the remaining capacity with 0
    // bytes to make sure the buffer will be correctly terminated.  If any
    // space is available, increment '*bytesWrittenPtr' and '*charsWrittenPtr'
    // by 1.
{
    if (*dstCapacityPtr) {
        ++*bytesWrittenPtr;
        ++*charsWrittenPtr;
    }

    while (*dstCapacityPtr) {
        --*dstCapacityPtr;
        **dstBufferPtr = 0;
        ++*dstBufferPtr;
    }
}

// LOCAL HELPER FUNCTION
void convertUcs2ToUtf8(unsigned char        **dstBufferPtr,
                       bsl::size_t           *dstCapacityPtr,
                       const unsigned short  *srcString,
                       bsl::size_t           *charsWrittenPtr,
                       bsl::size_t           *bytesWrittenPtr)
    // Convert one 16-bit UCS-2 character '*srcString' to one multi-octet
    // UTF-8 character at the location pointed to by '*dstBufferPtr'.  This
    // routine is only called in the case where '*srcString' maps to a 2 or 3
    // octet output sequence (the one octet case is handled in the parent
    // "fast" loop).  This routine also updates bumps '*dstBufferPtr' forward,
    // and updates '*bytesWrittenPtr', '*charsWrittenPtr', and
    // '*dstCapacityPtr'.  This will always succeed as long as sufficient space
    // is available for the multi-octet character AND a possible following 0
    // byte to terminate the output string.  If there is not enough space, the
    // remaining space in the output buffer is filled with 0 bytes.  Note that
    // in an implementation of a 'convertUtf16ToUtf8' routine, an additional
    // 'retCode' argument would be required.
{
    unsigned char *dstBuffer = *dstBufferPtr;

    if (*srcString <= TWO_OCTET_CHARACTER_UPPER_BOUND) {

        // This will be a 2-octet character.  We can't write a 2 byte character
        // unless there's space for 3 bytes (2 bytes + the following null
        // terminator).

        if (*dstCapacityPtr < 3) {
            // There is not enough space -- fill any remaining space with 0
            // bytes.

            nullFillRemainingBuffer(dstBufferPtr,
                                    dstCapacityPtr,
                                    charsWrittenPtr,
                                    bytesWrittenPtr);

            return;                                                   // RETURN
        }

        // There is enough space: populate the 2-octet output, using this
        // bitmap:
        //..
        // 0080-07FF | 110xxxxx 10xxxxxx
        //..

        dstBuffer[0] = (unsigned char)(RANGE1_BITS
                                         | (*srcString >> RANGE1_BYTE1_SHIFT));
        dstBuffer[1] = (unsigned char)(EXTRA_BITS | (*srcString & EXTRA_MASK));

        // Account for the 2 bytes and 1 character written.

        *dstBufferPtr    += 2;
        *dstCapacityPtr  -= 2;
        ++*charsWrittenPtr;
        *bytesWrittenPtr += 2;

        return;                                                       // RETURN
    }

    // If we get here, this will be a 3-octet character.  We can't write a 3
    // byte character unless there's space for 4 bytes (3 bytes + the following
    // 0 terminator).

    if (*dstCapacityPtr < 4){
        // There is not enough space -- fill remaining space with 0 bytes.

        nullFillRemainingBuffer(dstBufferPtr,
                                dstCapacityPtr,
                                charsWrittenPtr,
                                bytesWrittenPtr);

        return;                                                       // RETURN
    }

    // There is enough space: populate the 2-octet output, using this bitmap:
    //..
    // 0800-FFFF | 1110xxxx 10xxxxxx 10xxxxxx
    //..

    dstBuffer[0] = (unsigned char)(
                            RANGE2_BITS | (*srcString >> RANGE2_BYTE1_SHIFT));
    dstBuffer[1] = (unsigned char)(
              EXTRA_BITS | (EXTRA_MASK  & (*srcString >> RANGE2_BYTE2_SHIFT)));
    dstBuffer[2] = (unsigned char)(EXTRA_BITS | (*srcString & EXTRA_MASK));

    // Account for the 3 bytes and 1 character written.

    *dstBufferPtr    += 3;
    *dstCapacityPtr  -= 3;
    ++*charsWrittenPtr;
    *bytesWrittenPtr += 3;
}

}  // close unnamed namespace

namespace BloombergLP {
namespace bdlde {

                           // ----------------------
                           // struct CharConvertUcs2
                           // ----------------------

int CharConvertUcs2::utf8ToUcs2(unsigned short *dstBuffer,
                                bsl::size_t     dstCapacity,
                                const char     *srcString,
                                bsl::size_t    *numCharsWritten,
                                unsigned short  errorCharacter)
{
    // Theory: We're mainly dealing with 7-bit ASCII, so we're going to try for
    // an "as tight as possible" inner loop for characters that fit in 7 bits,
    // and call 'convertUtf8ToUcs2' to deal with the harder cases.

    if (dstCapacity <= 1) {
        if (numCharsWritten) {
            *numCharsWritten = dstCapacity;
        }

        if (dstCapacity) {
            *dstBuffer = 0;
        }

        if (srcString && *srcString) {
            return k_OUTPUT_BUFFER_TOO_SMALL;                         // RETURN
        }
        else {
            if (dstCapacity) {
                return k_SUCCESS;                                     // RETURN
            }

            return k_OUTPUT_BUFFER_TOO_SMALL;                         // RETURN
        }
    }

    // 'dstCapacity' is greater than 1 (we have room for more than just a
    // null-terminator).

    bsl::size_t charsWritten = 0;
    const unsigned char *src
                         = reinterpret_cast<const unsigned char *>(srcString);

    int retCode = k_SUCCESS;

    while (*src && dstCapacity) {
        if (*src <= ONE_OCTET_CHARACTER_UPPER_BOUND) {
            // This is a single-octet character; copy it and account for it.

            *dstBuffer = *src;

            ++dstBuffer;
            --dstCapacity;
            ++src;
            ++charsWritten;
        }
        else {
            // This is a multi-octet character.  Call helper routine.

            convertUtf8ToUcs2(&dstBuffer,
                              &dstCapacity,
                              &src,
                              &charsWritten,
                              errorCharacter,
                              &retCode);
        }
    }

    // We have either processed all the input or run out of space.

    if (dstCapacity) {
        ++charsWritten;
    }
    else {
        // If not enough space is available, back up, and write the terminating
        // null over the last character, which was already counted.

        --dstBuffer;

        retCode |= k_OUTPUT_BUFFER_TOO_SMALL;
    }

    *dstBuffer = 0;

    if (numCharsWritten) {
        *numCharsWritten = charsWritten;
    }

    return retCode;
}

int CharConvertUcs2::utf8ToUcs2(bsl::vector<unsigned short> *result,
                                const char                  *srcString,
                                unsigned short               errorCharacter)
{
    // Theory: We're mainly dealing with 7-bit ASCII, so we're going to try for
    // an "as tight as possible" inner loop for characters that fit in 7 bits,
    // and call 'convertUtf8ToUcs2' to deal with the harder cases.

    bsl::size_t charsWritten = 0;
    const unsigned char *src
                         = reinterpret_cast<const unsigned char *>(srcString);
    int retCode = k_SUCCESS;

    // We're going to work in 'buffer' and only update '*result' when 'buffer'
    // fills up.

    unsigned short        buffer[512];
    unsigned short       *dstBuffer   = buffer;
    bsl::size_t           dstCapacity = sizeof buffer / sizeof *buffer;
    const unsigned short *bufEnd      = buffer + dstCapacity - 2;

    result->resize(0);

    while (*src) {
        // This branch is unlikely - this only happens every 510 output
        // characters.

        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(dstBuffer >= bufEnd)) {
            result->insert(result->end(), buffer, dstBuffer);
            dstBuffer   = buffer;
            dstCapacity = sizeof buffer / sizeof *buffer;
        }

        if (*src <= ONE_OCTET_CHARACTER_UPPER_BOUND) {
            // This is a single-octet character; copy it and account for it.

            *dstBuffer = *src;

            ++dstBuffer;
            --dstCapacity;
            ++src;
            ++charsWritten;  // not strictly necessary for C++-style interface
        }
        else {
            // This is a multi-octet character.  Call helper routine.

            convertUtf8ToUcs2(&dstBuffer,
                              &dstCapacity,
                              &src,
                              &charsWritten,
                              errorCharacter,
                              &retCode);
        }
    }

    // Explicitly null-terminate '*result'.

    *dstBuffer++ = 0;

    result->insert(result->end(), buffer, dstBuffer);

    return retCode;
}

int CharConvertUcs2::ucs2ToUtf8(char                 *dstBuffer,
                                bsl::size_t           dstCapacity,
                                const unsigned short *srcString,
                                bsl::size_t          *numCharsWritten,
                                bsl::size_t          *numBytesWritten)
{
    // Theory: We're mainly dealing with 7-bit ASCII, so we're going to try for
    // an "as tight as possible" inner loop for characters that fit in 7 bits,
    // and call 'convertUcs2ToUtf8' to deal with the harder cases.

    if (dstCapacity <= 1) {
        if (numCharsWritten) {
            *numCharsWritten = dstCapacity;
        }

        if (numBytesWritten) {
            *numBytesWritten = dstCapacity;
        }

        if (dstCapacity) {
            *dstBuffer = 0;
        }

        if (srcString && *srcString) {
            return k_OUTPUT_BUFFER_TOO_SMALL;                         // RETURN
        }
        else {
            if (dstCapacity) {
                return k_SUCCESS;                                     // RETURN
            }

            return k_OUTPUT_BUFFER_TOO_SMALL;                         // RETURN
        }
    }

    // 'dstCapacity' is greater than 1 (we have room for more than just a
    // null-terminator).

    unsigned char *dst = reinterpret_cast<unsigned char *>(dstBuffer);
    int retCode = k_SUCCESS;

    bsl::size_t charsWritten = 0;
    bsl::size_t bytesWritten = 0;

    while (*srcString && dstCapacity) {
        if (*srcString <= ONE_OCTET_CHARACTER_UPPER_BOUND) {
            // This is a single-octet character.  Copy it and account for it.

            *dst = static_cast<unsigned char>(*srcString);
            ++dst;
            --dstCapacity;
            ++charsWritten;
            ++bytesWritten;
        }
        else {
            // This is a multi-octet character.  Call helper routine.

            convertUcs2ToUtf8(&dst,
                              &dstCapacity,
                              srcString,
                              &charsWritten,
                              &bytesWritten);
            // A UTF-16 conversion routine would need a '&retCode' argument
            // passed in here.
        }

        ++srcString;
    }

    if (dstCapacity) {
        // If enough space is available, we need to count the 0 byte we're
        // about to terminate the string with.

        ++charsWritten;
        ++bytesWritten;
    }
    else {
        // If not enough space is available, back up and write the terminating
        // null over the last character, which was already counted.  Note that
        // convertUcs2ToUtf8 does NOT write out multi-octet characters unless
        // there's space for the following null byte, so we don't have to worry
        // about backing up over the last octet of a multi-octet character.

        --dst;

        retCode |= k_OUTPUT_BUFFER_TOO_SMALL;
    }

    *dst = 0;

    if (numCharsWritten) {
        *numCharsWritten = charsWritten;
    }

    if (numBytesWritten) {
        *numBytesWritten = bytesWritten;
    }

    return retCode;
}

int CharConvertUcs2::ucs2ToUtf8(bsl::string          *result,
                                const unsigned short *srcString,
                                bsl::size_t          *numCharsWritten)
{
    // Theory: We're mainly dealing with 7-bit ASCII, so we're going to try for
    // an "as tight as possible" inner loop for characters that fit in 7 bits,
    // and call 'convertUcs2ToUtf8' to deal with the harder cases.

    // We're going to work in 'buffer' and only update '*result' when 'buffer'
    // fills up.

    unsigned char        buffer[1024];
    unsigned char       *dstBuffer   = buffer;
    bsl::size_t          dstCapacity = sizeof buffer / sizeof *buffer;
    const unsigned char *bufEnd      = buffer + dstCapacity - 5;

    result->resize(0);

    int retCode = k_SUCCESS;

    bsl::size_t charsWritten = 0;
    bsl::size_t bytesWritten = 0;

    while (*srcString) {
        // This branch is unlikely - this only happens about every 1020 output
        // octets.

        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(dstBuffer >= bufEnd)) {
            result->append(buffer, dstBuffer);
            dstBuffer   = buffer;
            dstCapacity = sizeof buffer / sizeof *buffer;
        }

        if (*srcString <= ONE_OCTET_CHARACTER_UPPER_BOUND) {
            // This is a single-octet character.  Copy it and account for it.

            *dstBuffer = static_cast<unsigned char>(*srcString);
            ++dstBuffer;
            --dstCapacity;
            ++charsWritten;
            ++bytesWritten;  // not strictly necessary for C++-style interface
        }
        else {
            // This is a multi-octet character.  Call helper routine.

            convertUcs2ToUtf8(&dstBuffer,
                              &dstCapacity,
                              srcString,
                              &charsWritten,
                              &bytesWritten);
            // A UTF-16 conversion routine would need a '&retCode' argument
            // passed in here.
        }

        ++srcString;
    }

    if (dstBuffer != buffer) {
        ++charsWritten;  // Account for the null-terminator even though none is
                         // explicitly appended to '*result'.

        result->append(buffer, dstBuffer);
    }

    if (numCharsWritten) {
        // If nothing was written, pretend a null-terminator was output.

        if (!charsWritten) {
            ++charsWritten;
        }
        *numCharsWritten = charsWritten;
    }

    return retCode;
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
