// bdede_utf8util.cpp                                                 -*-C++-*-
#include <bdede_utf8util.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdede_utf8util_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_performancehint.h>

// LOCAL CONSTANTS

enum {
    MIN_2_BYTE_VALUE = 0x80,     // min value that requires 2 bytes to encode
    MIN_3_BYTE_VALUE = 0x800,    // min value that requires 3 bytes to encode
    MIN_4_BYTE_VALUE = 0x10000,  // min value that requires 4 bytes to encode

    MAX_VALUE = 0x10ffff,        // max value that can be encoded in UTF-8

    CONT_VALUE_MASK = 0x3f       // part of a continuation byte that contains
                                 // the 6 bits of value info
};

// STATIC FUNCTIONS

static inline
bool isNotContinuation(char c)
    // Return 'true' if the specified 'c' is NOT a UTF-8 continuation byte,
    // and 'false' otherwise.
{
    return 0x80 != (c & 0xc0);
}

static inline
bool isSurrogateValue(int val)
    // Return 'true' if the specified 'val' is a surrogate value, and 'false'
    // otherwise.
{
    return (0xfffff800U & val) == 0xd800;
}

static inline
int get2ByteVal(const char *pc)
    // Assuming the 2 bytes pointed at by the specified 'pc' are a valid single
    // 2 byte UTF-8 character, return the value as an int.
{
    return ((*pc & 0x1f) << 6) | (pc[1] & CONT_VALUE_MASK);
}

static inline
int get3ByteVal(const char *pc)
    // Assuming the 3 bytes pointed at by the specified 'pc' are a valid single
    // 3 byte UTF-8 character, return the value as an int.
{
    return ((*pc & 0xf) << 12) | ((pc[1] & CONT_VALUE_MASK) << 6)
                               |  (pc[2] & CONT_VALUE_MASK);
}

static inline
int get4ByteVal(const char *pc)
    // Assuming the 4 bytes pointed at by the specified 'pc' are a valid single
    // 4 byte UTF-8 character, return the value as an int.
{
    return ((*pc & 0x7) << 18) | ((pc[1] & CONT_VALUE_MASK) << 12)
                               | ((pc[2] & CONT_VALUE_MASK) <<  6)
                               |  (pc[3] & CONT_VALUE_MASK);
}

#define UNLIKELY(exp) BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(exp)

namespace BloombergLP {

                            // ---------------------
                            // struct bdede_Utf8Util
                            // ---------------------

// CLASS METHODS
bool bdede_Utf8Util::isValid(const char *string, int length)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    const char *pc     = string;
    const char *pcEnd4 = string + length - 4;

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
            const int val = get2ByteVal(pc);
            if (UNLIKELY(isNotContinuation(pc[1])
                       | (val < MIN_2_BYTE_VALUE))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
                return false;                                         // RETURN
            }
            pc += 2;
          } break;
          case 0xe: {
            const int val = get3ByteVal(pc);
            if (UNLIKELY(isNotContinuation(pc[1])
                       | isNotContinuation(pc[2])
                       | (val < MIN_3_BYTE_VALUE)
                       | isSurrogateValue(val))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
                return false;                                         // RETURN
            }
            pc += 3;
          } break;
          case 0xf: {
            const int val = get4ByteVal(pc);
            if (UNLIKELY((8 & *pc)
                       | isNotContinuation(pc[1])
                       | isNotContinuation(pc[2])
                       | isNotContinuation(pc[3])
                       | (val < MIN_4_BYTE_VALUE)
                       | (val > MAX_VALUE))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
                return false;                                         // RETURN
            }
            pc += 4;
          } break;
          default: {
            return false;                                             // RETURN
          } break;
        }
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
          } break;
          case 0xc:
          case 0xd: {
            if (UNLIKELY(length < 2)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
                return false;                                         // RETURN
            }
            const int val = get2ByteVal(pc);
            if (UNLIKELY(isNotContinuation(pc[1])
                       | (val < MIN_2_BYTE_VALUE))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
                return false;                                         // RETURN
            }
            delta = 2;
          } break;
          case 0xe: {
            if (UNLIKELY(length < 3)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
                return false;                                         // RETURN
            }
            const int val = get3ByteVal(pc);
            if (UNLIKELY(isNotContinuation(pc[1])
                       | isNotContinuation(pc[2])
                       | (val < MIN_3_BYTE_VALUE)
                       | isSurrogateValue(val))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
                return false;                                         // RETURN
            }
            delta = 3;
          } break;
          default: {
            return false;                                             // RETURN
          } break;
        }

        pc     += delta;
        length -= delta;
    }

    BSLS_ASSERT(0 == length);
    BSLS_ASSERT(pcEnd4 + 4 == pc);

    return true;
}

bool bdede_Utf8Util::isValid(const char *string)
{
    BSLS_ASSERT(string);

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
                return true;                                          // RETURN
            }
            ++string;
          } break;
          case 0xc:
          case 0xd: {
            if (UNLIKELY(isNotContinuation(string[1]))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
                return false;                                         // RETURN
            }
            const int val = get2ByteVal(string);
            if (UNLIKELY(val < MIN_2_BYTE_VALUE)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
                return false;                                         // RETURN
            }
            string += 2;
          } break;
          case 0xe: {
            if (UNLIKELY(isNotContinuation(string[1]))
             || UNLIKELY(isNotContinuation(string[2]))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
                return false;                                         // RETURN
            }
            const int val = get3ByteVal(string);
            if (UNLIKELY((val < MIN_3_BYTE_VALUE) | isSurrogateValue(val))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
                return false;                                         // RETURN
            }
            string += 3;
          } break;
          case 0xf: {
            if (UNLIKELY(isNotContinuation(string[1]))
             || UNLIKELY(isNotContinuation(string[2]))
             || UNLIKELY(isNotContinuation(string[3]))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
                return false;                                         // RETURN
            }
            const int val = get4ByteVal(string);
            if (UNLIKELY((8 & *string)
                       | (val < MIN_4_BYTE_VALUE)
                       | (val > MAX_VALUE))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
                return false;                                         // RETURN
            }
            string += 4;
          } break;
          default: {
            return false;                                             // RETURN
          } break;
        }
    }
}

int bdede_Utf8Util::numCharacters(const char *string, int length)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    int ret = 0;

    // Note that since we are assuming the string already passed one of the
    // validation functions our work is very simple.

    const char * const end = string + length;
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

        ++ret;
    }

    BSLS_ASSERT(end == string);

    return ret;
}

int bdede_Utf8Util::numCharacters(const char *string)
{
    BSLS_ASSERT(string);

    int ret = 0;

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
                return ret;                                           // RETURN
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

        ++ret;
    }
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
