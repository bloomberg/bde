// bdlde_quotedprintabledecoder.cpp                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlde_quotedprintabledecoder.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlde_quotedprintabledecoder_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsl_cstring.h>

namespace {

typedef BloombergLP::bdlde::QuotedPrintableDecoder QuotedPrintableDecoder;

}  // close unnamed namespace

                // ======================
                // FILE-SCOPE STATIC DATA
                // ======================

// Strict-mode equivalence class symbols
static const char RC_ = QuotedPrintableDecoder::e_RC_;
static const char HX_ = QuotedPrintableDecoder::e_HX_;
static const char EQ_ = QuotedPrintableDecoder::e_EQ_;
static const char WS_ = QuotedPrintableDecoder::e_WS_;
static const char CR_ = QuotedPrintableDecoder::e_CR_;
static const char LC_ = QuotedPrintableDecoder::e_LC_;
static const char LL_ = QuotedPrintableDecoder::e_LL_;
static const char UC_ = QuotedPrintableDecoder::e_UC_;

// Relaxed-mode equivalence class symbols
static const char RC = QuotedPrintableDecoder::e_RC;
static const char HX = QuotedPrintableDecoder::e_HX;
static const char EQ = QuotedPrintableDecoder::e_EQ;
static const char WS = QuotedPrintableDecoder::e_WS;
static const char CR = QuotedPrintableDecoder::e_CR;
static const char LC = QuotedPrintableDecoder::e_LC;
static const char LL = QuotedPrintableDecoder::e_LL;
static const char UC = QuotedPrintableDecoder::e_UC;

// The following table is a map of an 8-bit index value to the corresponding
// equivalence class for operation in the strict error- reporting mode (i.e.,
// unrecognizedIsErrorFlag == 'true').  Other modes of operation should
// use/modify the map below for the relaxed error-reporting and CRLF line break
// mode (i.e., EQUIVALENCE_CLASS_MAP_CRLF below).

static const char EQUIVALENCE_CLASS_MAP_STRICT[] = {
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
// equivalence class for operation in the relaxed error-reporting CRLF line
// break mode (line breaks are decoded to "\r\n").  The LF line break mode
// requires only moving element ['\n'] to the LL class.

static const char EQUIVALENCE_CLASS_MAP_CRLF[] = {
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
// check, the real value of 0x00 in decondingMap cannot be distinguished from
// the null values in the same table.

static const unsigned char decodingMap[] = {
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

namespace BloombergLP {
namespace bdlde {

const char* QuotedPrintableDecoder::s_lineBreakModeName[] = {
    "CRLF mode",
    "LF mode"
};

const char QuotedPrintableDecoder::s_componentName[] =
                                               "bdlde::QuotedPrintableDecoder";
const bool QuotedPrintableDecoder::s_defaultUnrecognizedIsErrorFlag = false;
const char *QuotedPrintableDecoder::s_defaultEquivClassStrict_p =
                                                  EQUIVALENCE_CLASS_MAP_STRICT;
const char *QuotedPrintableDecoder::s_defaultEquivClassCRLF_p =
                                                    EQUIVALENCE_CLASS_MAP_CRLF;
const unsigned char * const QuotedPrintableDecoder::s_decodingMap_p =
                                                                   decodingMap;
const int QuotedPrintableDecoder::s_defaultMaxLineLength = 76;

                           // ----------------------
                           // QuotedPrintableDecoder
                           // ----------------------

// CREATORS
QuotedPrintableDecoder::~QuotedPrintableDecoder()
{
    // Assert invariants:

    BSLS_ASSERT(e_ERROR_STATE <= d_state);
    BSLS_ASSERT(d_state <= e_DONE_STATE);
    BSLS_ASSERT(0 <= d_outputLength);
    BSLS_ASSERT(e_CRLF_MODE == d_lineBreakMode ||
                                                 e_LF_MODE == d_lineBreakMode);
    BSLS_ASSERT(d_lineBreakMode <= 1);

    if (d_equivClass_p != s_defaultEquivClassStrict_p &&
        d_equivClass_p != s_defaultEquivClassCRLF_p) {
        delete d_equivClass_p;
    }
}

// MANIPULATORS
int QuotedPrintableDecoder::convert(char       *out,
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

    if (e_ERROR_STATE == d_state || e_DONE_STATE == d_state) {
        int rv = e_DONE_STATE == d_state ? -2 : -1;
        d_state = e_ERROR_STATE;
        *numOut = 0;
        *numIn = 0;
        return rv;                                                    // RETURN
    }

    if (0 == maxNumOut) {
        *numOut = 0;
        *numIn = 0;
        return 0;                                                     // RETURN
    }

    const char *originalBegin = begin;
    int numEmitted = 0;

    while (numEmitted != maxNumOut && (begin < end ||
              ((e_INPUT_STATE == d_state || e_NEED_HEX_STATE == d_state) &&
                                                            d_bufferLength))) {
        if ((e_INPUT_STATE == d_state || e_NEED_HEX_STATE == d_state) &&
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
        else if (e_SAW_WS_STATE == d_state) {
            if (' ' == *begin || '\t' == *begin) {
                d_buffer[d_bufferLength++] = *begin++;
            }
            else if ('=' == *begin) {
                // set state to indicate the equal sign

                d_state = e_SAW_EQUAL_STATE;
                ++begin;
            }
            else if ('\r' == *begin) {
                d_bufferLength = 0;
                d_state = e_INPUT_STATE;
            }
            else {
                d_state = e_INPUT_STATE;
            }
        }
        else if (e_NEED_HEX_STATE == d_state) {
            const unsigned char ch = decodingMap[(int)*begin];
            if (ch == (unsigned char)0xFF) {
                *numOut = numEmitted;
                d_outputLength += numEmitted;
                *numIn = begin - originalBegin;
                d_state = e_ERROR_STATE;
                return -1;                                            // RETURN
            }
            *out++ = static_cast<char>((d_hexBuffer << 4) | ch);
            ++numEmitted;
            ++begin;
            d_state = e_INPUT_STATE;
        }
        else if (e_NEED_SOFT_LF_STATE == d_state) {
            if ('\n' != *begin) {
                *numOut = numEmitted;
                d_outputLength += numEmitted;
                *numIn = begin - originalBegin;
                d_state = e_ERROR_STATE;
                return -1;                                            // RETURN
            }
            ++begin;
            d_state = e_INPUT_STATE;
        }
        else if (e_NEED_HARD_LF_STATE == d_state) {
            if ('\n' != *begin) {
                *numOut = numEmitted;
                d_outputLength += numEmitted;
                *numIn = begin - originalBegin;
                d_state = e_ERROR_STATE;
                return -1;                                            // RETURN
            }
            if (e_CRLF_MODE == d_lineBreakMode) {
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
            d_state = e_INPUT_STATE;
        }
        else if (e_SAW_EQUAL_STATE == d_state) {
            if ('\r' == *begin) {
                // next character must be a '\n'

                d_state = e_NEED_SOFT_LF_STATE;
                ++begin;
            }
            else {
                d_hexBuffer = (char)decodingMap[(int)*begin];
                if (d_hexBuffer == (char)0xFF) {
                    *numOut = numEmitted;
                    d_outputLength += numEmitted;
                    *numIn = begin - originalBegin;
                    d_state = e_ERROR_STATE;
                    return -1;                                        // RETURN
                }
                d_state = e_NEED_HEX_STATE;
                ++begin;
            }
        }
        else {
            BSLS_ASSERT(e_INPUT_STATE == d_state);
            if ('=' == *begin) {
                // set state to indicate the equal sign

                d_state = e_SAW_EQUAL_STATE;
                ++begin;
            }
            else if ('\r' == *begin) {
                d_state = e_NEED_HARD_LF_STATE;
                ++begin;
            }
            else if (' ' == *begin || '\t' == *begin) {
                d_buffer[0] = *begin++;
                d_bufferLength = 1;
                d_state = e_SAW_WS_STATE;
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

int QuotedPrintableDecoder::endConvert(char *out,
                                       int  *numOut,
                                       int  )
{
    BSLS_ASSERT(out);
    BSLS_ASSERT(numOut);

    if (e_ERROR_STATE == d_state || e_DONE_STATE == d_state ||
                                                              d_bufferLength) {
        d_state = e_ERROR_STATE;
        *numOut = 0;
        return -1;                                                    // RETURN
    }

    *numOut = 0;
    return 0;
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
