// bdede_quotedprintableencoder.cpp              -*-C++-*-
#include <bdede_quotedprintableencoder.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdede_quotedprintableencoder_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_cstring.h>   // strlen()

namespace BloombergLP {

                // ======================
                // FILE-SCOPE STATIC DATA
                // ======================

const char PC = bdede_QuotedPrintableEncoder::BDEDE_PC;
const char CC = bdede_QuotedPrintableEncoder::BDEDE_CC;
const char CR = bdede_QuotedPrintableEncoder::BDEDE_CR;
const char LF = bdede_QuotedPrintableEncoder::BDEDE_LF;
const char WS = bdede_QuotedPrintableEncoder::BDEDE_WS;

// The following table is a map of an 8-bit index value to the
// corresponding equivalence class.

const char EQUIVALENCE_CLASS_MAP[] = {
//  0   1   2   3   4   5   6   7
    CC, CC, CC, CC, CC, CC, CC, CC,  // 000
    CC, WS, CC, CC, CC, CR, CC, CC,  // 010  '\t', '\n', '\r'
    CC, CC, CC, CC, CC, CC, CC, CC,  // 020
    CC, CC, CC, CC, CC, CC, CC, CC,  // 030
    WS, PC, PC, PC, PC, PC, PC, PC,  // 040  ' ', '!' (decimal 33)
    PC, PC, PC, PC, PC, PC, PC, PC,  // 050
    PC, PC, PC, PC, PC, PC, PC, PC,  // 060
    PC, PC, PC, PC, PC, CC, PC, PC,  // 070  '='
    PC, PC, PC, PC, PC, PC, PC, PC,  // 100
    PC, PC, PC, PC, PC, PC, PC, PC,  // 110
    PC, PC, PC, PC, PC, PC, PC, PC,  // 120
    PC, PC, PC, PC, PC, PC, PC, PC,  // 130
    PC, PC, PC, PC, PC, PC, PC, PC,  // 140
    PC, PC, PC, PC, PC, PC, PC, PC,  // 150
    PC, PC, PC, PC, PC, PC, PC, PC,  // 160
    PC, PC, PC, PC, PC, PC, PC, CC,  // 170  '~' (decimal 126)
    CC, CC, CC, CC, CC, CC, CC, CC,  // 200
    CC, CC, CC, CC, CC, CC, CC, CC,  // 210
    CC, CC, CC, CC, CC, CC, CC, CC,  // 220
    CC, CC, CC, CC, CC, CC, CC, CC,  // 230
    CC, CC, CC, CC, CC, CC, CC, CC,  // 240
    CC, CC, CC, CC, CC, CC, CC, CC,  // 250
    CC, CC, CC, CC, CC, CC, CC, CC,  // 260
    CC, CC, CC, CC, CC, CC, CC, CC,  // 270
    CC, CC, CC, CC, CC, CC, CC, CC,  // 300
    CC, CC, CC, CC, CC, CC, CC, CC,  // 310
    CC, CC, CC, CC, CC, CC, CC, CC,  // 320
    CC, CC, CC, CC, CC, CC, CC, CC,  // 330
    CC, CC, CC, CC, CC, CC, CC, CC,  // 340
    CC, CC, CC, CC, CC, CC, CC, CC,  // 350
    CC, CC, CC, CC, CC, CC, CC, CC,  // 360
    CC, CC, CC, CC, CC, CC, CC, CC   // 370
};

// The following table stores the names for the line break modes so that on
// query, users can tell the mode of the encoder without having to search the
// header file for the corresponding enum value.

const char* bdede_QuotedPrintableEncoder::s_lineBreakModeName[] = {
    "CRLF mode",
    "LF mode",
    "Mixed mode"
};

const char*
bdede_QuotedPrintableEncoder::s_defaultEquivClass_p = EQUIVALENCE_CLASS_MAP;
const int bdede_QuotedPrintableEncoder::s_defaultMaxLineLength = 76;

static const char *BDE_HEX = "0123456789ABCDEF";

// CREATORS
bdede_QuotedPrintableEncoder::bdede_QuotedPrintableEncoder(
                     bdede_QuotedPrintableEncoder::LineBreakMode mode,
                     int                                         maxLineLength)
: d_lineBreakMode(mode)
, d_maxLineLength(maxLineLength)
, d_outputLength(0)
, d_lineLength(0)
, d_state(BDEDE_INITIAL_STATE)
, d_bufferLength(0)
, d_lineStart(0)
{
    if (d_maxLineLength == DEFAULT_MAX_LINELEN) {
        d_maxLineLength = s_defaultMaxLineLength;
    }
    else if (d_maxLineLength == 0) {
        d_maxLineLength = INT_MAX;
    }

    BSLS_ASSERT(4 <= d_maxLineLength);

    if (mode == BDEDE_CRLF_MODE) {
        // Use the default opcode table.

       d_equivClass_p = const_cast<char*>(s_defaultEquivClass_p);
            // Encoder will not change d_equivClass_p outside constructor.
    }
    else {
        // Copy the table and change the opcodes for '\r' for both
        // LF_MODE and BDEDE_MIXED_MODE.

        int len = sizeof(EQUIVALENCE_CLASS_MAP);
        d_equivClass_p = new char[len];
        bsl::memcpy(d_equivClass_p, s_defaultEquivClass_p, len);
        d_equivClass_p['\n'] = LF;
    }

    if (mode == BDEDE_LF_MODE) {
        // In addition, change the opcode for '\n' for BDEDE_LF_MODE.

        d_equivClass_p['\r'] = CC;
    }
}

bdede_QuotedPrintableEncoder::bdede_QuotedPrintableEncoder(
               const char                                  *extraCharsToEncode,
               bdede_QuotedPrintableEncoder::LineBreakMode  mode,
               int                                          maxLineLength)
: d_lineBreakMode(mode)
, d_maxLineLength(maxLineLength)
, d_outputLength(0)
, d_lineLength(0)
, d_state(BDEDE_INITIAL_STATE)
, d_bufferLength(0)
, d_lineStart(0)
{
    if (d_maxLineLength == DEFAULT_MAX_LINELEN) {
        d_maxLineLength = s_defaultMaxLineLength;
    }
    else if (d_maxLineLength == 0) {
        d_maxLineLength = INT_MAX;
    }

    BSLS_ASSERT(4 <= d_maxLineLength);

    // First copy the default mapping table to data member.
    int len = sizeof(EQUIVALENCE_CLASS_MAP);
    d_equivClass_p = new char[len];
    bsl::memcpy(d_equivClass_p, s_defaultEquivClass_p, len);

    // Now change the specified individual elements.
    if (extraCharsToEncode) {
        len = bsl::strlen(extraCharsToEncode);
        for (int i = 0; i < len; ++i) {
            d_equivClass_p[(unsigned char) (extraCharsToEncode[i])] = CC;
        }
    }

    // Put CRLF mapping last as it will override the caller's mistake of
    // including '\r' or '\n' in the 'extraCharsToEncode' array.
    switch (mode) {
      case BDEDE_CRLF_MODE: {
        d_equivClass_p['\r'] = CR;
        d_equivClass_p['\n'] = CC;  // Stand-alone '\n' is encoded.
      } break;
      case BDEDE_LF_MODE: {
        d_equivClass_p['\r'] = CC;  // Stand-alone '\r' is encoded.
        d_equivClass_p['\n'] = LF;
      } break;
      case BDEDE_MIXED_MODE: {
        d_equivClass_p['\r'] = CR;
        d_equivClass_p['\n'] = LF;
      } break;
      default: {
      }
    }
}

bdede_QuotedPrintableEncoder::~bdede_QuotedPrintableEncoder()
{
    // Assert invariants:
    BSLS_ASSERT(BDEDE_ERROR_STATE <= d_state);
    BSLS_ASSERT(d_state <= BDEDE_SAW_CR_STATE);
    BSLS_ASSERT(4 <= d_maxLineLength);
    BSLS_ASSERT(0 <= d_outputLength);
    BSLS_ASSERT(0 <= d_lineLength);
    BSLS_ASSERT(BDEDE_CRLF_MODE  == d_lineBreakMode
             || BDEDE_LF_MODE    == d_lineBreakMode
             || BDEDE_MIXED_MODE == d_lineBreakMode);
    BSLS_ASSERT(d_lineBreakMode <= 2);

    if (d_equivClass_p != s_defaultEquivClass_p) {
        delete[] d_equivClass_p;
    }

    while (!d_outBuf.empty()) {
        d_outBuf.pop();
    }
}



// MANIPULATORS
int bdede_QuotedPrintableEncoder::convert(char       *out,
                                          int        *numOut,
                                          int        *numIn,
                                          const char *begin,
                                          const char *end,
                                          int         maxNumOut)
{
    BSLS_ASSERT(out);
    BSLS_ASSERT(numOut);
    BSLS_ASSERT(numIn);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);

    if (BDEDE_ERROR_STATE == d_state || BDEDE_DONE_STATE == d_state) {
        int rv = BDEDE_DONE_STATE == d_state ? -2 : -1;
        d_state = BDEDE_ERROR_STATE;
        *numOut = 0;
        *numIn = 0;
        return rv;
    }

    if (0 == maxNumOut) {
        *numOut = 0;
        *numIn = 0;
        return 0;
    }

    const int originalOutputLength = d_outputLength;
    const char *originalBegin = begin;
    int maxOutLen = d_outputLength + maxNumOut;

    while (begin < end && d_outputLength != maxOutLen) {
        if (d_outputLength - d_lineStart == 75) {
            // insert soft newline
            BSLS_ASSERT(0 == d_bufferLength);
            *out++ = '=';
            ++d_outputLength;
            d_lineStart = d_outputLength + 2;
            d_buffer[0] = '\r';
            d_buffer[1] = '\n';
            d_bufferLength = 2;
        }
        else if (d_bufferLength) {
            // flush buffer
            BSLS_ASSERT(d_bufferLength <= 3);
            if (('\r' == *begin || '\n' == *begin) &&
                                 (' ' == d_buffer[0] || '\t' == d_buffer[0])) {
                BSLS_ASSERT(1 == d_bufferLength);
                if (d_outputLength - d_lineStart > 72) {
                    // insert soft newline
                    *out++ = '=';
                    ++d_outputLength;
                    d_lineStart = d_outputLength + 2;
                    d_buffer[2] = d_buffer[0];
                    d_buffer[0] = '\r';
                    d_buffer[1] = '\n';
                    d_bufferLength = 3;
                }
                else {
                    *out++ = '=';
                    ++d_outputLength;
                    const char ch = d_buffer[0];
                    d_buffer[0] = BDE_HEX[ch >> 4];
                    d_buffer[1] = BDE_HEX[ch & 0xf];
                    d_bufferLength = 2;
                }
            }
            else {
                *out++ = d_buffer[0];
                ++d_outputLength;
                d_buffer[0] = d_buffer[1];
                d_buffer[1] = d_buffer[2];
                --d_bufferLength;
            }
        }
        else if (BDEDE_SAW_CR_STATE == d_state) {
            const char ch = *begin;
            if ('\n' == ch) {
                // TBD fix
                *out++ = '\r';
                ++d_outputLength;
                d_lineStart = d_outputLength + 1;
                d_buffer[0] = '\n';
                d_bufferLength = 1;
                d_state = BDEDE_INPUT_STATE;
                ++begin;
            }
            else if (d_outputLength - d_lineStart > 72) {
                // insert soft newline
                BSLS_ASSERT(0 == d_bufferLength);
                *out++ = '=';
                ++d_outputLength;
                d_lineStart = d_outputLength + 2;
                d_buffer[0] = '\r';
                d_buffer[1] = '\n';
                d_bufferLength = 2;
            }
            else {
                // insert BDE_HEX representation
                BSLS_ASSERT(0 == d_bufferLength);
                *out++ = '=';
                ++d_outputLength;
                // buffer two hex characters
                d_buffer[0] = '0';
                d_buffer[1] = 'D';
                d_bufferLength = 2;
                d_state = BDEDE_INPUT_STATE;
            }
        }
        else {
            const char ch = *begin;
            if (' ' == ch || '\t' == ch) {
                d_buffer[0] = ch;
                d_bufferLength = 1;
                ++begin;
            }
            else if ('=' == ch || ch < 33 || ch > 126) {
                if ('\r' == ch && bdede_QuotedPrintableEncoder::BDEDE_LF_MODE !=
                                                             d_lineBreakMode) {
                    d_state = BDEDE_SAW_CR_STATE;
                    ++begin;
                }
                else if ('\n' == ch &&
                                     bdede_QuotedPrintableEncoder::BDEDE_CRLF_MODE !=
                                                             d_lineBreakMode) {
                    d_state = BDEDE_SAW_CR_STATE;
                    ++begin;
                }
                else if (d_outputLength - d_lineStart > 72) {
                    *out++ = '=';
                    ++d_outputLength;
                    d_lineStart = d_outputLength + 2;
                    d_buffer[0] = '\r';
                    d_buffer[1] = '\n';
                    d_bufferLength = 2;
                }
                else {
                    *out++ = '=';
                    ++d_outputLength;
                    // buffer two hex characters
                    d_buffer[0] = BDE_HEX[ch >> 4];
                    d_buffer[1] = BDE_HEX[ch & 0xf];
                    d_bufferLength = 2;
                    ++begin;
                }
            }
            else {
                *out++ = ch;
                ++d_outputLength;
                ++begin;
            }
        }
    }

    *numOut = d_outputLength - originalOutputLength;
    *numIn = begin - originalBegin;
    return d_bufferLength;
}

int bdede_QuotedPrintableEncoder::endConvert(char *out,
                                             int  *numOut,
                                             int   maxNumOut)
{
    enum { BDEDE_ERR = -1 };

    if (d_state == BDEDE_ERROR_STATE || isDone()) {
        d_state = BDEDE_ERROR_STATE;
        *numOut = 0;
        return BDEDE_ERR;
    }

    BSLS_ASSERT(d_bufferLength <= 3);

    const int originalOutputLength = d_outputLength;
    int maxOutLen = d_outputLength + maxNumOut;
    while (d_bufferLength && d_outputLength != maxOutLen) {
        if (' ' == d_buffer[0] || '\t' == d_buffer[0]) {
            if (d_outputLength - d_lineStart > 72) {
                // insert soft newline
                *out++ = '=';
                ++d_outputLength;
                d_lineStart = d_outputLength + 2;
                d_buffer[2] = d_buffer[0];
                d_buffer[0] = '\r';
                d_buffer[1] = '\n';
                d_bufferLength = 3;
            }
            else {
                *out++ = '=';
                ++d_outputLength;
                const char ch = d_buffer[0];
                d_buffer[0] = BDE_HEX[ch >> 4];
                d_buffer[1] = BDE_HEX[ch & 0xf];
                d_bufferLength = 2;
            }
        }
        else {
            *out++ = d_buffer[0];
            ++d_outputLength;
            d_buffer[0] = d_buffer[1];
            d_buffer[1] = d_buffer[2];
            --d_bufferLength;
        }
    }
    *numOut = d_outputLength - originalOutputLength;
    return d_bufferLength;
}



}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
