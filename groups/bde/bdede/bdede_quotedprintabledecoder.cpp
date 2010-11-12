// bdede_quotedprintabledecoder.cpp              -*-C++-*-
#include <bdede_quotedprintabledecoder.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdede_quotedprintabledecoder_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsl_cstring.h>

namespace BloombergLP {

                // ======================
                // FILE-SCOPE STATIC DATA
                // ======================

// Strict-mode equivalence class symbols
const char RC_ = bdede_QuotedPrintableDecoder::BDEDE_RC_;
const char HX_ = bdede_QuotedPrintableDecoder::BDEDE_HX_;
const char EQ_ = bdede_QuotedPrintableDecoder::BDEDE_EQ_;
const char WS_ = bdede_QuotedPrintableDecoder::BDEDE_WS_;
const char CR_ = bdede_QuotedPrintableDecoder::BDEDE_CR_;
const char LC_ = bdede_QuotedPrintableDecoder::BDEDE_LC_;
const char LL_ = bdede_QuotedPrintableDecoder::BDEDE_LL_;
const char UC_ = bdede_QuotedPrintableDecoder::BDEDE_UC_;

// Relaxed-mode equivalence class symbols
const char RC = bdede_QuotedPrintableDecoder::BDEDE_RC;
const char HX = bdede_QuotedPrintableDecoder::BDEDE_HX;
const char EQ = bdede_QuotedPrintableDecoder::BDEDE_EQ;
const char WS = bdede_QuotedPrintableDecoder::BDEDE_WS;
const char CR = bdede_QuotedPrintableDecoder::BDEDE_CR;
const char LC = bdede_QuotedPrintableDecoder::BDEDE_LC;
const char LL = bdede_QuotedPrintableDecoder::BDEDE_LL;
const char UC = bdede_QuotedPrintableDecoder::BDEDE_UC;

// The following table is a map of an 8-bit index value to the corresponding
// equivalence class for operation in the strict error- reporting mode (i.e.,
// unrecognizedIsErrorFlag == 'true').  Other modes of operation should
// use/modify the map below for the relaxed error-reporting and CRLF line break
// mode (i.e., EQUIVALENCE_CLASS_MAP_CRLF below).

const char EQUIVALENCE_CLASS_MAP_STRICT[] = {
//  0   1   2   3   4   5   6   7
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_,  // 000
    UC_, WS_, LC_, UC_, UC_, CR_, UC_, UC_,  // 010  '\t'_, '\n'_, '\r'
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_,  // 020
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_,  // 030
    WS_, RC_, RC_, RC_, RC_, RC_, RC_, RC_,  // 040  ' '_, '!' (decimal 33)
    RC_, RC_, RC_, RC_, RC_, RC_, RC_, RC_,  // 050
    HX_, HX_, HX_, HX_, HX_, HX_, HX_, HX_,  // 060  '0' - '7'
    HX_, HX_, RC_, RC_, RC_, EQ_, RC_, RC_,  // 070  '8'-'9'_, '='
    RC_, HX_, HX_, HX_, HX_, HX_, HX_, RC_,  // 100  'A'-'F'
    RC_, RC_, RC_, RC_, RC_, RC_, RC_, RC_,  // 110
    RC_, RC_, RC_, RC_, RC_, RC_, RC_, RC_,  // 120
    RC_, RC_, RC_, RC_, RC_, RC_, RC_, RC_,  // 130
    RC_, RC_, RC_, RC_, RC_, RC_, RC_, RC_,  // 140  'a'-'f' (\141-\146)
    RC_, RC_, RC_, RC_, RC_, RC_, RC_, RC_,  // 150
    RC_, RC_, RC_, RC_, RC_, RC_, RC_, RC_,  // 160
    RC_, RC_, RC_, RC_, RC_, RC_, RC_, UC_,  // 170  '~' (decimal 126)
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_,  // 200
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_,  // 210
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_,  // 220
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_,  // 230
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_,  // 240
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_,  // 250
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_,  // 260
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_,  // 270
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_,  // 300
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_,  // 310
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_,  // 320
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_,  // 330
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_,  // 340
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_,  // 350
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_,  // 360
    UC_, UC_, UC_, UC_, UC_, UC_, UC_, UC_   // 370
};

// The following table is a map of an 8-bit index value to the corresponding
// equivalence class for operation in the relaxed error-reporting CRLF
// line break mode (line breaks are decoded to "\r\n").  The LF line break mode
// requires only moving element ['\n'] to the LL class.

const char EQUIVALENCE_CLASS_MAP_CRLF[] = {
//  0   1   2   3   4   5   6   7
    RC, RC, RC, RC, RC, RC, RC, RC,  // 000
    RC, WS, LC, RC, RC, CR, RC, RC,  // 010  '\t', '\n', '\r'
    RC, RC, RC, RC, RC, RC, RC, RC,  // 020
    RC, RC, RC, RC, RC, RC, RC, RC,  // 030
    WS, RC, RC, RC, RC, RC, RC, RC,  // 040  ' ', '!' (decimal 33)
    RC, RC, RC, RC, RC, RC, RC, RC,  // 050
    HX, HX, HX, HX, HX, HX, HX, HX,  // 060  '0'-'7'
    HX, HX, RC, RC, RC, EQ, RC, RC,  // 070  '8'-'9', '='
    RC, HX, HX, HX, HX, HX, HX, RC,  // 100  'A'-'F'
    RC, RC, RC, RC, RC, RC, RC, RC,  // 110
    RC, RC, RC, RC, RC, RC, RC, RC,  // 120
    RC, RC, RC, RC, RC, RC, RC, RC,  // 130
    RC, HX, HX, HX, HX, HX, HX, UC,  // 140  'a'-'f' (\141-\146)
    UC, UC, UC, UC, UC, UC, UC, UC,  // 200
    UC, UC, UC, UC, UC, UC, UC, UC,  // 210
    UC, UC, UC, UC, UC, UC, UC, UC,  // 220
    UC, UC, UC, UC, UC, UC, UC, UC,  // 230
    UC, UC, UC, UC, UC, UC, UC, UC,  // 240
    UC, UC, UC, UC, UC, UC, UC, UC,  // 250
    UC, UC, UC, UC, UC, UC, UC, UC,  // 260
    UC, UC, UC, UC, UC, UC, UC, UC,  // 270
    UC, UC, UC, UC, UC, UC, UC, UC,  // 300
    UC, UC, UC, UC, UC, UC, UC, UC,  // 310
    UC, UC, UC, UC, UC, UC, UC, UC,  // 320
    UC, UC, UC, UC, UC, UC, UC, UC,  // 330
    UC, UC, UC, UC, UC, UC, UC, UC,  // 340
    UC, UC, UC, UC, UC, UC, UC, UC,  // 350
    UC, UC, UC, UC, UC, UC, UC, UC,  // 360
    UC, UC, UC, UC, UC, UC, UC, UC   // 370
};

// The following table maps an ASCII character to the hexadecimal value it is
// representing.  When used consecutively to convert 2 contiguous hexadecimal
// digits (X), this table is useful for decoding a sequence of the form "=XX"
// from its Quoted-Printable representation back to a single character whose
// value is equal to the number formed by the 2 digits.
//
// Note that the mapped value for characters outside the hexadecimal range is
// intentionally set to 0 to ease the table construction.  This design choice
// is correct only *if* the indexing character is first verified to be a
// hexadecimal digit (which is done in the decoder anyway).  Without this
// check, the real value of 0x00 in DEC cannot be distinguished from the null
// values in the same table.

const unsigned char DEC[] = {
//  0     1     2     3     4     5     6     7
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 000
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 010
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 020
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 030
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 040
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 050
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,  // 060  '0'-'7'
    0x08, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 070  '8'-'9'
    0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF,  // 100  'A'-'F'
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 110
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 120
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 130
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 140  'a'-'f'
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 150
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 160
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 170
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 200
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 210
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 220
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 230
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 240
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 250
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 260
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 270
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 300
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 310
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 320
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 330
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 340
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 350
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 360
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 370
};

const char* bdede_QuotedPrintableDecoder::s_lineBreakModeName[] = {
    "CRLF mode",
    "LF mode"
};

const char bdede_QuotedPrintableDecoder::s_componentName[] =
                                                "bdede_QuotedPrintableDecoder";

const bool
bdede_QuotedPrintableDecoder::s_defaultUnrecognizedIsErrorFlag = false;
const char*bdede_QuotedPrintableDecoder::s_defaultEquivClassStrict_p =
                                                  EQUIVALENCE_CLASS_MAP_STRICT;
const char*bdede_QuotedPrintableDecoder::s_defaultEquivClassCRLF_p =
                                                    EQUIVALENCE_CLASS_MAP_CRLF;
const unsigned char *const bdede_QuotedPrintableDecoder::s_decodingMap_p = DEC;
const int bdede_QuotedPrintableDecoder::s_defaultMaxLineLength = 76;

// CREATORS
bdede_QuotedPrintableDecoder::~bdede_QuotedPrintableDecoder()
{
    // Assert invariants:
    BSLS_ASSERT(BDEDE_ERROR_STATE <= d_state);
    BSLS_ASSERT(d_state <= BDEDE_DONE_STATE);
    BSLS_ASSERT(0 <= d_outputLength);
    BSLS_ASSERT(BDEDE_CRLF_MODE == d_lineBreakMode || BDEDE_LF_MODE == d_lineBreakMode);
    BSLS_ASSERT(d_lineBreakMode <= 1);

    if (d_equivClass_p != s_defaultEquivClassStrict_p &&
        d_equivClass_p != s_defaultEquivClassCRLF_p) {
        delete d_equivClass_p;
    }
}



// MANIPULATORS
int bdede_QuotedPrintableDecoder::convert(char       *out,
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

    const char *originalBegin = begin;
    int numEmitted = 0;

    while (numEmitted != maxNumOut && (begin < end ||
              ((BDEDE_INPUT_STATE == d_state || BDEDE_NEED_HEX_STATE == d_state) &&
                                                            d_bufferLength))) {
        if ((BDEDE_INPUT_STATE == d_state || BDEDE_NEED_HEX_STATE == d_state) &&
                                                              d_bufferLength) {
            // flush buffer
            int i = 0;
            while (i < d_bufferLength && numEmitted != maxNumOut) {
                *out++ = d_buffer[i++];
                ++numEmitted;
            }
            d_bufferLength -= i;
            bsl::memcpy(d_buffer, &d_buffer[i], d_bufferLength);
        }
        else if (BDEDE_SAW_WS_STATE == d_state) {
            if (' ' == *begin || '\t' == *begin) {
                d_buffer[d_bufferLength++] = *begin++;
            }
            else if ('=' == *begin) {
                // set state to indicate the equal sign
                d_state = BDEDE_SAW_EQUAL_STATE;
                ++begin;
            }
            else if ('\r' == *begin) {
                d_bufferLength = 0;
                d_state = BDEDE_INPUT_STATE;
            }
            else {
                d_state = BDEDE_INPUT_STATE;
            }
        }
        else if (BDEDE_NEED_HEX_STATE == d_state) {
            const unsigned char ch = DEC[(int)*begin];
            if (ch == (unsigned char)0xFF) {
                *numOut = numEmitted;
                d_outputLength += numEmitted;
                *numIn = begin - originalBegin;
                d_state = BDEDE_ERROR_STATE;
                return -1;
            }
            *out++ = (d_hexBuffer << 4) | ch;
            ++numEmitted;
            ++begin;
            d_state = BDEDE_INPUT_STATE;
        }
        else if (BDEDE_NEED_SOFT_LF_STATE == d_state) {
            if ('\n' != *begin) {
                *numOut = numEmitted;
                d_outputLength += numEmitted;
                *numIn = begin - originalBegin;
                d_state = BDEDE_ERROR_STATE;
                return -1;
            }
            ++begin;
            d_state = BDEDE_INPUT_STATE;
        }
        else if (BDEDE_NEED_HARD_LF_STATE == d_state) {
            if ('\n' != *begin) {
                *numOut = numEmitted;
                d_outputLength += numEmitted;
                *numIn = begin - originalBegin;
                d_state = BDEDE_ERROR_STATE;
                return -1;
            }
            if (BDEDE_CRLF_MODE == d_lineBreakMode) {
                *out++ = '\r';
                ++numEmitted;
                d_buffer[0] = '\n';
                d_bufferLength = 1;
            }
            else {
                *out++ = '\n';
                ++numEmitted;
            }
            ++begin;
            d_state = BDEDE_INPUT_STATE;
        }
        else if (BDEDE_SAW_EQUAL_STATE == d_state) {
            if ('\r' == *begin) {
                // next character must be a '\n'
                d_state = BDEDE_NEED_SOFT_LF_STATE;
                ++begin;
            }
            else {
                d_hexBuffer = (char)DEC[(int)*begin];
                if (d_hexBuffer == (char)0xFF) {
                    *numOut = numEmitted;
                    d_outputLength += numEmitted;
                    *numIn = begin - originalBegin;
                    d_state = BDEDE_ERROR_STATE;
                    return -1;
                }
                d_state = BDEDE_NEED_HEX_STATE;
                ++begin;
            }
        }
        else {
            BSLS_ASSERT(BDEDE_INPUT_STATE == d_state);
            if ('=' == *begin) {
                // set state to indicate the equal sign
                d_state = BDEDE_SAW_EQUAL_STATE;
                ++begin;
            }
            else if ('\r' == *begin) {
                d_state = BDEDE_NEED_HARD_LF_STATE;
                ++begin;
            }
            else if (' ' == *begin || '\t' == *begin) {
                d_buffer[0] = *begin++;
                d_bufferLength = 1;
                d_state = BDEDE_SAW_WS_STATE;
            }
            else {
                // forward the character
                *out++ = *begin++;
                ++numEmitted;
            }
        }
    }

    *numOut = numEmitted;
    d_outputLength += numEmitted;
    *numIn = begin - originalBegin;
    return 0;
    // TBD return value
}

int bdede_QuotedPrintableDecoder::endConvert(char *out,
                                             int  *numOut,
                                             int  )
{
    BSLS_ASSERT(out);
    BSLS_ASSERT(numOut);

    if (BDEDE_ERROR_STATE == d_state || BDEDE_DONE_STATE == d_state ||
                                                              d_bufferLength) {
        d_state = BDEDE_ERROR_STATE;
        *numOut = 0;
        return -1;
    }

    *numOut = 0;
    return 0;
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
