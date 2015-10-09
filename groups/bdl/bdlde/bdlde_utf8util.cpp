// bdlde_utf8util.cpp                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlde_utf8util.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlde_utf8util_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_performancehint.h>

// LOCAL MACROS

#define UNLIKELY(EXPRESSION) BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(EXPRESSION)

// LOCAL CONSTANTS

namespace {

enum {
    k_MIN_2_BYTE_VALUE = 0x80,     // min value that requires 2 bytes to encode
    k_MIN_3_BYTE_VALUE = 0x800,    // min value that requires 3 bytes to encode
    k_MIN_4_BYTE_VALUE = 0x10000,  // min value that requires 4 bytes to encode

    k_MIN_SURROGATE    = 0xd800,   // min surrogate value

    k_MAX_VALID = 0x10ffff,        // max value that can be encoded in UTF-8

    k_CONT_VALUE_MASK = 0x3f       // part of a continuation byte that contains
                                   // the 6 bits of value info
};

}  // close unnamed namespace

// STATIC HELPER FUNCTIONS

static inline
bool isNotContinuation(char value)
    // Return 'true' if the specified 'value' is NOT a UTF-8 continuation byte,
    // and 'false' otherwise.
{
    return 0x80 != (value & 0xc0);
}

static inline
bool isSurrogateValue(int value)
    // Return 'true' if the specified 'value' is a surrogate value, and 'false'
    // otherwise.
{
    enum { k_SURROGATE_MASK = 0xfffff800U };

    return (k_SURROGATE_MASK & value) == k_MIN_SURROGATE;
}

static inline
int get2ByteValue(const char *pc)
    // Return the integral value of the 2-byte UTF-8 character referred to by
    // the specified 'pc'.  The behavior is undefined unless 'pc' refers to a
    // valid 2-byte UTF-8 character.
{
    return ((*pc & 0x1f) << 6) | (pc[1] & k_CONT_VALUE_MASK);
}

static inline
int get3ByteValue(const char *pc)
    // Return the integral value of the 3-byte UTF-8 character referred to by
    // the specified 'pc'.  The behavior is undefined unless 'pc' refers to a
    // valid 3-byte UTF-8 character.
{
    return ((*pc & 0xf) << 12) | ((pc[1] & k_CONT_VALUE_MASK) << 6)
                               |  (pc[2] & k_CONT_VALUE_MASK);
}

static inline
int get4ByteValue(const char *pc)
    // Return the integral value of the 4-byte UTF-8 character referred to by
    // the specified 'pc'.  The behavior is undefined unless 'pc' refers to a
    // valid 4-byte UTF-8 character.
{
    return ((*pc & 0x7) << 18) | ((pc[1] & k_CONT_VALUE_MASK) << 12)
                               | ((pc[2] & k_CONT_VALUE_MASK) <<  6)
                               |  (pc[3] & k_CONT_VALUE_MASK);
}

static
int validateAndCountCharacters(const char **invalidString, const char *string)
    // Return the number of UTF-8 characters in the specified 'string' if it
    // contains valid UTF-8, with no effect on the specified 'invalidString'.
    // Otherwise, return a negative value and load into 'invalidString' the
    // address of the first character in 'string' that does not constitute the
    // start of a valid UTF-8 character encoding.  'string' is necessarily
    // null-terminated, so it cannot contain embedded null characters.  Note
    // that 'string' may contain less than 'bsl::strlen(string)' UTF-8
    // characters.
{
    // The following assertions are redundant with those in the CLASS METHODS.
    // Hence, 'BSLS_ASSERT_SAFE' is used.

    BSLS_ASSERT_SAFE(invalidString);
    BSLS_ASSERT_SAFE(string);

    int count = 0;

    while (true) {
        switch ((*string >> 4) & 0xf) {
          case 0:
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7: {
            if (UNLIKELY(!*string)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                return count;                                         // RETURN
            }
            ++string;
          } break;
          case 0xc:
          case 0xd: {
            if (UNLIKELY(isNotContinuation(string[1]))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = string;
                return -1;                                            // RETURN
            }
            const int value = get2ByteValue(string);
            if (UNLIKELY(value < k_MIN_2_BYTE_VALUE)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = string;
                return -1;                                            // RETURN
            }
            string += 2;
          } break;
          case 0xe: {
            if (UNLIKELY(isNotContinuation(string[1]))
             || UNLIKELY(isNotContinuation(string[2]))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = string;
                return -1;                                            // RETURN
            }
            const int value = get3ByteValue(string);
            if (UNLIKELY((value < k_MIN_3_BYTE_VALUE)
                       | isSurrogateValue(value))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = string;
                return -1;                                            // RETURN
            }
            string += 3;
          } break;
          case 0xf: {
            if (UNLIKELY(isNotContinuation(string[1]))
             || UNLIKELY(isNotContinuation(string[2]))
             || UNLIKELY(isNotContinuation(string[3]))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = string;
                return -1;                                            // RETURN
            }
            const int value = get4ByteValue(string);
            if (UNLIKELY((8 & *string)
                       | (value < k_MIN_4_BYTE_VALUE)
                       | (value > k_MAX_VALID))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = string;
                return -1;                                            // RETURN
            }
            string += 4;
          } break;
          default: {
            *invalidString = string;
            return -1;                                                // RETURN
          }
        }

        ++count;
    }
}

static
int validateAndCountCharacters(const char **invalidString,
                               const char  *string,
                               int          length)
    // Return the number of UTF-8 characters in the specified 'string' having
    // the specified 'length' (in bytes) if 'string' contains valid UTF-8, with
    // no effect on the specified 'invalidString'.  Otherwise, return a
    // negative value and load into 'invalidString' the address of the first
    // character in 'string' that does not constitute the start of a valid
    // UTF-8 character encoding.  'string' need not be null-terminated and can
    // contain embedded null characters.  The behavior is undefined unless
    // '0 <= length'.  Note that 'string' may contain less than 'length' UTF-8
    // characters.
{
    // The following assertions are redundant with those in the CLASS METHODS.
    // Hence, 'BSLS_ASSERT_SAFE' is used.

    BSLS_ASSERT_SAFE(invalidString);
    BSLS_ASSERT_SAFE(string);
    BSLS_ASSERT_SAFE(0 <= length);

    const char       *pc     = string;
    const char *const pcEnd4 = string + length - 4;

    int count = 0;

    while (pc <= pcEnd4) {
        switch ((*pc >> 4) & 0xf) {
          case 0:
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7: {
            ++pc;
          } break;
          case 0xc:
          case 0xd: {
            const int value = get2ByteValue(pc);
            if (UNLIKELY(isNotContinuation(pc[1])
                       | (value < k_MIN_2_BYTE_VALUE))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = pc;
                return -1;                                            // RETURN
            }
            pc += 2;
          } break;
          case 0xe: {
            const int value = get3ByteValue(pc);
            if (UNLIKELY(isNotContinuation(pc[1])
                       | isNotContinuation(pc[2])
                       | (value < k_MIN_3_BYTE_VALUE)
                       | isSurrogateValue(value))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = pc;
                return -1;                                            // RETURN
            }
            pc += 3;
          } break;
          case 0xf: {
            const int value = get4ByteValue(pc);
            if (UNLIKELY((0x8 & *pc)
                       | isNotContinuation(pc[1])
                       | isNotContinuation(pc[2])
                       | isNotContinuation(pc[3])
                       | (value < k_MIN_4_BYTE_VALUE)
                       | (value > k_MAX_VALID))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = pc;
                return -1;                                            // RETURN
            }
            pc += 4;
          } break;
          default: {
            *invalidString = pc;
            return -1;                                                // RETURN
          }
        }

        ++count;
    }

    length -= pc - string;

    // 'length' is now < 4.

    while (length > 0) {
        int delta;

        switch ((*pc >> 4) & 0xf) {
          case 0:
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7: {
            delta = 1;
            ++count;
          } break;
          case 0xc:
          case 0xd: {
            if (UNLIKELY(length < 2)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = pc;
                return -1;                                            // RETURN
            }
            const int value = get2ByteValue(pc);
            if (UNLIKELY(isNotContinuation(pc[1])
                       | (value < k_MIN_2_BYTE_VALUE))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = pc;
                return -1;                                            // RETURN
            }
            delta = 2;
            ++count;
          } break;
          case 0xe: {
            if (UNLIKELY(length < 3)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = pc;
                return -1;                                            // RETURN
            }
            const int value = get3ByteValue(pc);
            if (UNLIKELY(isNotContinuation(pc[1])
                       | isNotContinuation(pc[2])
                       | (value < k_MIN_3_BYTE_VALUE)
                       | isSurrogateValue(value))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = pc;
                return -1;                                            // RETURN
            }
            delta = 3;
            ++count;
          } break;
          default: {
            *invalidString = pc;
            return -1;                                                // RETURN
          }
        }

        pc     += delta;
        length -= delta;
    }

    BSLS_ASSERT(0 == length);
    BSLS_ASSERT(pcEnd4 + 4 == pc);

    return count;
}

namespace BloombergLP {

namespace bdlde {
                              // ---------------
                              // struct Utf8Util
                              // ---------------

// CLASS METHODS
int Utf8Util::advanceIfValid(int         *status,
                             const char **result,
                             const char  *string,
                             int          numCharacters)
{
    BSLS_ASSERT(status);
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(numCharacters >= 0);

    int         ret = 0;      // return value -- number of characters advanced
    const char *next;         // 'next' is calculated during the loop as the
                              // starting position to start parsing the next
                              // character, and assigned to 'string' between
                              // iterations.

    // Note that we keep 'string' pointing to the beginning of the Unicode
    // character being processed, and only advance it to the next character
    // between iterations.

    for (; true; ++ret, string = next) {
        next = string + 1;

        if (UNLIKELY(ret >= numCharacters)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // This is the first of 2 ways we can exit this function
            // successfully.

            BSLS_ASSERT(ret == numCharacters);           // impossible to fail

            *status = 0;
            break;
        }

        // Note that if we leave this 'switch' without doing a 'continue' we'll
        // exit the loop at the bottom.

        switch (*reinterpret_cast<const unsigned char *>(string) >> 4) {
          case 0: {
            // binary: 0000xxxx: low ASCII and possibly '\0'

            if (!*string) {
                // '\0', end of input with no errors.  This is the second of
                // two ways we can successfully exit this function.

                *status = 0;
                break;
            }
          } continue;

          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7: {
            // binary: 0xxxxxxx: ASCII, but definitely not '\0'

          } continue;

          case 8:
          case 9:
          case 0xa:
          case 0xb: {
            // binary: 10xxxxxx: error: unexpected continuation octet

            *status = -1;

            // no 'continue'; exit the loop
          } break;

          case 0xc:
          case 0xd: {
            // binary: 110xxxxx: 2-octet sequence

            if (UNLIKELY(isNotContinuation(*next))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // invalid continuation

                *status = -1;
                break;
            }

            ++next;

            if (UNLIKELY(get2ByteValue(string) < k_MIN_2_BYTE_VALUE)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // non-minimal encoding

                *status = -1;
                break;
            }

          } continue;

          case 0xe: {
            // binary: 1110xxxx: 3 octet sequence

            if   (UNLIKELY(isNotContinuation(*next))
               || UNLIKELY(isNotContinuation(*++next))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // invalid continuations

                *status = -1;
                break;
            }

            ++next;

            int value = get3ByteValue(string);

            if (UNLIKELY(value < k_MIN_3_BYTE_VALUE)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // non-minimal encoding

                *status = -1;
                break;
            }

            if (UNLIKELY(isSurrogateValue(value))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // surrogate value

                *status = -1;
                break;
            }
          } continue;

          case 0xf: {
            // binary: 1111xxxx: 4 octet sequence (only legal if 11110xxx).

            if (UNLIKELY(*string & 8)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // binary: 11111xxx: illegal start of 5 (or more) octet
                // sequence

                *status = -1;
                break;
            }

            // binary: 11110xxx: legal start of 4 octet sequence

            if   (UNLIKELY(isNotContinuation(*next))
               || UNLIKELY(isNotContinuation(*++next))
               || UNLIKELY(isNotContinuation(*++next))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // invalid continuations

                *status = -1;
                break;
            }

            ++next;

            if (UNLIKELY(get4ByteValue(string) < k_MIN_4_BYTE_VALUE)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // non-minimal encoding

                *status = -1;
                break;
            }
          } continue;
        }

        break;
    }

    *result = string;
    return ret;
}

int Utf8Util::advanceIfValid(int         *status,
                             const char **result,
                             const char  *string,
                             int          length,
                             int          numCharacters)
{
    BSLS_ASSERT(status);
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(length        >= 0);
    BSLS_ASSERT(numCharacters >= 0);

    int         ret = 0;      // return value -- number of characters advanced
    const char *next;         // 'next' is advanced during the loop to the
                              // starting position to start parsing the next
                              // character, and assigned to 'string' between
                              // iterations.

    const char * const endOfInput = string + length;

    // Note that we keep 'string' pointing to the beginning of the Unicode
    // character being processed, and only advance it to the next character
    // between iterations.

    for (; true; ++ret, string = next) {
        next = string + 1;

        if (UNLIKELY(string >= endOfInput)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Success.  We have successfully parsed all the input.

            BSLS_ASSERT(string == endOfInput);           // impossible to fail

            *status = 0;
            break;
        }

        if (UNLIKELY(ret >= numCharacters)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Success.  We have successfully advanced 'numCharacters'
            // characters.

            BSLS_ASSERT(ret == numCharacters);           // impossible to fail

            *status = 0;
            break;
        }

        // Note that if we leave this 'switch' without doing a 'continue'
        // (which we only do if we encounter an error), we'll exit the loop at
        // the bottom.

        switch (*reinterpret_cast<const unsigned char *>(string) >> 4) {
          case 0:
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7: {
            // binary: 0xxxxxxx: ASCII and possible '\0'

          } continue;

          case 8:
          case 9:
          case 0xa:
          case 0xb: {
            // binary: 10xxxxxx: unexpected continuation octet

            *status = -1;
          } break;

          case 0xc:
          case 0xd: {
            // binary: 110xxxxx: 2-octet sequence

            if (UNLIKELY(string + 2 > endOfInput)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // truncation of character

                *status = -1;
                break;
            }

            if (UNLIKELY(isNotContinuation(*next))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // invalid continuation

                *status = -1;
                break;
            }

            ++next;

            if (UNLIKELY(get2ByteValue(string) < k_MIN_2_BYTE_VALUE)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // non-minimal encoding

                *status = -1;
                break;
            }
          } continue;

          case 0xe: {
            // binary: 1110xxxx: 3 octet sequence

            if (UNLIKELY(string + 3 > endOfInput)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // truncation of character

                *status = -1;
                break;
            }

            if   (UNLIKELY(isNotContinuation(*next))
               || UNLIKELY(isNotContinuation(*++next))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // invalid continuations

                *status = -1;
                break;
            }

            ++next;

            int value = get3ByteValue(string);

            if (UNLIKELY(value < k_MIN_3_BYTE_VALUE)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // non-minimal encoding

                *status = -1;
                break;
            }

            if (UNLIKELY(isSurrogateValue(value))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // surrogate value

                *status = -1;
                break;
            }
          } continue;

          case 0xf: {
            // binary: 1111xxxx: 4 octet sequence (only legal if 11110xxx).

            if (UNLIKELY(*string & 8)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // binary: 11111xxx: invalid character in all UTF-8 contexts.

                *status = -1;
                break;
            }

            // binary: 11110xxx: legal start to 4 octet sequence

            if (UNLIKELY(string + 4 > endOfInput)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // truncation of character

                *status = -1;
                break;
            }

            if   (UNLIKELY(isNotContinuation(*next))
               || UNLIKELY(isNotContinuation(*++next))
               || UNLIKELY(isNotContinuation(*++next))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // invalid continuations

                *status = -1;
                break;
            }

            ++next;

            if (UNLIKELY(get4ByteValue(string) < k_MIN_4_BYTE_VALUE)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // non-minimal encoding

                *status = -1;
                break;
            }
          } continue;
        }

        break;
    }

    *result = string;
    return ret;
}

int Utf8Util::advanceRaw(const char **result,
                         const char  *string,
                         int          numCharacters)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(numCharacters >= 0);

    int ret = 0;        // return value, # of characters advanced

    for (; ret < numCharacters; ++ret) {
        // There's a 'break' at the end of this loop, so any case in the switch
        // that leaves without doing a 'continue' will exit the loop.

        switch (*reinterpret_cast<const unsigned char *>(string) >> 4) {
          case 0: {
            // binary: 0000xxxx: low ASCII, possibly '\0'

            if (!*string) {
                // '\0': end of input

                break;
            }

            ++string;
          } continue;

          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7: {
            // binary: 0xxxxxxx: ASCII (definitely not '\0')

            ++string;
          } continue;

          case 8:
          case 9:
          case 0xa:
          case 0xb: {
            // binary: 10xxxxxx: We hit an unexpected continuation octet.

            BSLS_ASSERT(0 && "invalid UTF-8");
          } break;

          // The following are multi-octet sequences.  Since we're assuming
          // valid input, don't bother inspecting the continuation octets.

          case 0xc:
          case 0xd: {
            // binary: 110xxxxx: Start of two-octet sequence.

            string += 2;
          } continue;
          case 0xe: {
            // binary: 1110xxxx: Start of three-octet sequence.

            string += 3;
          } continue;
          case 0xf: {
            // binary: 1111xxxx: Start of four-octet sequence.

            BSLS_ASSERT(0 == (*string & 8));    // only legal if 11110xxx

            string += 4;
          } continue;
        }

        break;
    }

    BSLS_ASSERT(ret == numCharacters || '\0' == *string); // impossible to fail

    *result = string;
    return ret;
}

int Utf8Util::advanceRaw(const char **result,
                         const char  *string,
                         int          length,
                         int          numCharacters)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(length   >= 0);
    BSLS_ASSERT(numCharacters >= 0);

    int ret = 0;        // return value, # of characters advanced

    const char * const endOfInput = string + length;

    for (; true; ++ret) {
        // There's a 'break' at the end of this loop, so any case in the switch
        // that leaves without doing a 'continue' will exit the loop.

        if (UNLIKELY(string >= endOfInput)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Success

            BSLS_ASSERT(string == endOfInput);      // impossible to fail
                                                    // unless invalid UTF-8
            break;
        }

        if (UNLIKELY(ret >= numCharacters)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Success

            BSLS_ASSERT(ret == numCharacters);      // impossible to fail
            break;
        }

        switch (*reinterpret_cast<const unsigned char *>(string) >> 4) {
          case 0:
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7: {
            // binary: 0xxxxxxx: ASCII, possibly '\0'

            ++string;
          } continue;

          case 8:
          case 9:
          case 0xa:
          case 0xb: {
            // binary: 10xxxxxx: We hit an unexpected continuation octet.

            BSLS_ASSERT(0 && "invalid UTF-8");
          } break;

          // The following are multi-octet sequences.  Since we're assuming
          // valid input, don't bother inspecting the continuation octets.

          case 0xc:
          case 0xd: {
            // binary: 110xxxxx: Beginning of two-octet sequence.

            string += 2;
          } continue;

          case 0xe: {
            // binary: 1110xxxx: Beginning of three-octet sequence.

            string += 3;
          } continue;

          case 0xf: {
            // binary: 1111xxxx: Beginning of four-octet sequence.

            BSLS_ASSERT(0 == (*string & 8));    // only legal if 11110xxx

            // binary: 11110xxx: Legal beginning of four-octet sequence

            string += 4;
          } continue;
        }

        break;
    }

    *result = string;
    return ret;
}

bool Utf8Util::isValid(const char **invalidString, const char *string)
{
    BSLS_ASSERT(invalidString);
    BSLS_ASSERT(string);

    return validateAndCountCharacters(invalidString, string) >= 0;
}

bool Utf8Util::isValid(const char **invalidString,
                       const char  *string,
                       int          length)
{
    BSLS_ASSERT(invalidString);
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    return validateAndCountCharacters(invalidString, string, length) >= 0;
}

int Utf8Util::numCharactersIfValid(const char **invalidString,
                                   const char  *string)
{
    BSLS_ASSERT(invalidString);
    BSLS_ASSERT(string);

    return validateAndCountCharacters(invalidString, string);
}

int Utf8Util::numCharactersIfValid(const char **invalidString,
                                   const char  *string,
                                   int          length)
{
    BSLS_ASSERT(invalidString);
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    return validateAndCountCharacters(invalidString, string, length);
}

int Utf8Util::numCharactersRaw(const char *string)
{
    BSLS_ASSERT(string);

    int count = 0;

    // Note that since we assume the string contains valid UTF-8, our work is
    // very simple.

    while (true) {
        switch ((*string >> 4) & 0xf) {
          case 0:
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7: {
            if (!*string) {
                return count;                                         // RETURN
            }
            ++string;
          } break;
          case 0xc:
          case 0xd: {
            string += 2;
          } break;
          case 0xe: {
            string += 3;
          } break;
          default: {
            string += 4;
          } break;
        }

        ++count;
    }
}

int Utf8Util::numCharactersRaw(const char *string, int length)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    int count = 0;

    // Note that since we assume the string contains valid UTF-8, our work is
    // very simple.

    const char *const end = string + length;

    while (string < end) {
        switch ((*string >> 4) & 0xf) {
          case 0:
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7: {
            ++string;
          } break;
          case 0xc:
          case 0xd: {
            string += 2;
          } break;
          case 0xe: {
            string += 3;
          } break;
          default: {
            string += 4;
          } break;
        }

        ++count;
    }

    BSLS_ASSERT(end == string);

    return count;
}

int Utf8Util::numCharacters(const char *string)
{
    return numCharactersRaw(string);
}

int Utf8Util::numCharacters(const char *string, int length)
{
    return numCharactersRaw(string, length);
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
