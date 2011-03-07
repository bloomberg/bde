// bdede_utf8util.cpp                                                 -*-C++-*-
#include <bdede_utf8util.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdede_utf8util_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_performancehint.h>

// LOCAL MACROS

#define UNLIKELY(EXPRESSION) BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(EXPRESSION)

// LOCAL CONSTANTS

enum {
    MIN_2_BYTE_VALUE = 0x80,     // min value that requires 2 bytes to encode
    MIN_3_BYTE_VALUE = 0x800,    // min value that requires 3 bytes to encode
    MIN_4_BYTE_VALUE = 0x10000,  // min value that requires 4 bytes to encode

    MAX_VALUE = 0x10ffff,        // max value that can be encoded in UTF-8

    CONT_VALUE_MASK = 0x3f       // part of a continuation byte that contains
                                 // the 6 bits of value info
};

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
    return (0xfffff800U & value) == 0xd800;
}

static inline
int get2ByteValue(const char *pc)
    // Return the integral value of the 2-byte UTF-8 character referred to by
    // the specified 'pc'.  The behavior is undefined unless 'pc' refers to a
    // valid 2-byte UTF-8 character.
{
    return ((*pc & 0x1f) << 6) | (pc[1] & CONT_VALUE_MASK);
}

static inline
int get3ByteValue(const char *pc)
    // Return the integral value of the 3-byte UTF-8 character referred to by
    // the specified 'pc'.  The behavior is undefined unless 'pc' refers to a
    // valid 3-byte UTF-8 character.
{
    return ((*pc & 0xf) << 12) | ((pc[1] & CONT_VALUE_MASK) << 6)
                               |  (pc[2] & CONT_VALUE_MASK);
}

static inline
int get4ByteValue(const char *pc)
    // Return the integral value of the 4-byte UTF-8 character referred to by
    // the specified 'pc'.  The behavior is undefined unless 'pc' refers to a
    // valid 4-byte UTF-8 character.
{
    return ((*pc & 0x7) << 18) | ((pc[1] & CONT_VALUE_MASK) << 12)
                               | ((pc[2] & CONT_VALUE_MASK) <<  6)
                               |  (pc[3] & CONT_VALUE_MASK);
}

static
int validateAndCountCharacters(const char **invalidString, const char *string)
    // Return the number of UTF-8 characters in the specified 'string' if it
    // contains valid UTF-8, with no effect on the specified 'invalidString'.
    // Otherwise, return a negative value and load into 'invalidString' the
    // address of the first character in 'string' that does not consitute the
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
            if (UNLIKELY(value < MIN_2_BYTE_VALUE)) {
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
            if (UNLIKELY((value < MIN_3_BYTE_VALUE)
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
                       | (value < MIN_4_BYTE_VALUE)
                       | (value > MAX_VALUE))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
                *invalidString = string;
                return -1;                                            // RETURN
            }
            string += 4;
          } break;
          default: {
            *invalidString = string;
            return -1;                                                // RETURN
          } break;
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
    // character in 'string' that does not consitute the start of a valid UTF-8
    // character encoding.  'string' need not be null-terminated and can
    // contain embedded null characters.  The behavior is undefined unless
    // '0 <= length'.  Note that 'string' may contain less than 'length' UTF-8
    // characters.
{
    // The following assertions are redundant with those in the CLASS METHODS.
    // Hence, 'BSLS_ASSERT_SAFE' is used.

    BSLS_ASSERT_SAFE(invalidString);
    BSLS_ASSERT_SAFE(string);
    BSLS_ASSERT_SAFE(0 <= length);

    const char *pc           = string;
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
                       | (value < MIN_2_BYTE_VALUE))) {
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
                       | (value < MIN_3_BYTE_VALUE)
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
                       | (value < MIN_4_BYTE_VALUE)
                       | (value > MAX_VALUE))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
                *invalidString = pc;
                return -1;                                            // RETURN
            }
            pc += 4;
          } break;
          default: {
            *invalidString = pc;
            return -1;                                                // RETURN
          } break;
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
                       | (value < MIN_2_BYTE_VALUE))) {
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
                       | (value < MIN_3_BYTE_VALUE)
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
          } break;
        }

        pc     += delta;
        length -= delta;
    }

    BSLS_ASSERT(0 == length);
    BSLS_ASSERT(pcEnd4 + 4 == pc);

    return count;
}

namespace BloombergLP {

                            // ---------------------
                            // struct bdede_Utf8Util
                            // ---------------------

// CLASS METHODS
bool bdede_Utf8Util::isValid(const char **invalidString, const char *string)
{
    BSLS_ASSERT(invalidString);
    BSLS_ASSERT(string);

    return validateAndCountCharacters(invalidString, string) >= 0;
}

bool bdede_Utf8Util::isValid(const char **invalidString,
                             const char  *string,
                             int          length)
{
    BSLS_ASSERT(invalidString);
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    return validateAndCountCharacters(invalidString, string, length) >= 0;
}

int bdede_Utf8Util::numCharactersIfValid(const char **invalidString,
                                         const char  *string)
{
    BSLS_ASSERT(invalidString);
    BSLS_ASSERT(string);

    return validateAndCountCharacters(invalidString, string);
}

int bdede_Utf8Util::numCharactersIfValid(const char **invalidString,
                                         const char  *string,
                                         int          length)
{
    BSLS_ASSERT(invalidString);
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    return validateAndCountCharacters(invalidString, string, length);
}

int bdede_Utf8Util::numCharactersRaw(const char *string)
{
    BSLS_ASSERT(string);

    int count = 0;

    // Note that since we are assuming the string already passed one of the
    // validation functions our work is very simple.

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

int bdede_Utf8Util::numCharactersRaw(const char *string, int length)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    int count = 0;

    // Note that since we are assuming the string already passed one of the
    // validation functions our work is very simple.

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

int bdede_Utf8Util::numCharacters(const char *string)
{
    return numCharactersRaw(string);
}

int bdede_Utf8Util::numCharacters(const char *string, int length)
{
    return numCharactersRaw(string, length);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
