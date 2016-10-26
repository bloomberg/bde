// bdlde_quotedprintableencoder.cpp                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlde_quotedprintableencoder.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlde_quotedprintableencoder_cpp,"$Id$ $CSID$")

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_cstring.h>   // 'strlen'

                          // ======================
                          // FILE-SCOPE STATIC DATA
                          // ======================

namespace {

typedef BloombergLP::bdlde::QuotedPrintableEncoder QuotedPrintableEncoder;

}  // close unnamed namespace

static const char PC = QuotedPrintableEncoder::e_PC;
static const char CC = QuotedPrintableEncoder::e_CC;
static const char CR = QuotedPrintableEncoder::e_CR;
static const char LF = QuotedPrintableEncoder::e_LF;
static const char WS = QuotedPrintableEncoder::e_WS;

static const char *bdeHex = "0123456789ABCDEF";

// The following table is a map of an 8-bit index value to the corresponding
// equivalence class.

static const char equivalenceClassMap[] = {
//  0   1   2   3   4   5   6   7
    CC, CC, CC, CC, CC, CC, CC, CC,  // 000
    CC, WS, LF, CC, CC, CR, CC, CC,  // 010  '\t', '\n', '\r'
    CC, CC, CC, CC, CC, CC, CC, CC,  // 020
    CC, CC, CC, CC, CC, CC, CC, CC,  // 030
    WS, PC, PC, PC, PC, PC, PC, PC,  // 040  ' ', '!' is first PC
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
    PC, PC, PC, PC, PC, PC, PC, CC,  // 170  '~' is last PC
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

namespace BloombergLP {
namespace bdlde {

const char* QuotedPrintableEncoder::s_lineBreakModeName[] = {
    "CRLF mode",
    "LF mode",
    "Mixed mode"
};

const char *QuotedPrintableEncoder::s_defaultEquivClass_p =
                                                           equivalenceClassMap;

// PRIVATE MANIPULATORS
void QuotedPrintableEncoder::appendSoftLineBreak(char *out) {
    *out = '=';
    ++d_outputLength;

    d_buffer[d_bufferLength++] = '\n';
    d_buffer[d_bufferLength++] = '\r';

    d_lineStart = d_outputLength + 2;

    d_lastWasWS = false;

    BSLS_ASSERT(5 >= d_bufferLength);
}

void QuotedPrintableEncoder::appendHardLineBreak(char *out) {
    if (d_lastWasWS) {
        *out = '=';
        ++d_outputLength;

        d_buffer[d_bufferLength++] = '\n';
        d_buffer[d_bufferLength++] = '\r';
        d_buffer[d_bufferLength++] = '\n';
        d_buffer[d_bufferLength++] = '\r';

        d_lineStart = d_outputLength + 4;
    }
    else {
        *out = '\r';
        ++d_outputLength;

        d_buffer[d_bufferLength++] = '\n';

        d_lineStart = d_outputLength + 1;
    }

    d_lastWasWS = false;

    BSLS_ASSERT(5 >= d_bufferLength);
}

void QuotedPrintableEncoder::appendPrintable(char *out, char ch) {
    BSLS_ASSERT(0 == d_bufferLength);

    if (d_outputLength - d_lineStart >= d_maxLineLength - 1) {
        d_buffer[d_bufferLength++] = ch;
        appendSoftLineBreak(out);
    }
    else {
        *out = ch;
        ++d_outputLength;
    }

    d_lastWasWS = (WS == d_equivClass_p[static_cast<unsigned char>(ch)]);

    BSLS_ASSERT(5 >= d_bufferLength);
}

void QuotedPrintableEncoder::appendAsHex(char *out, char ch, bool isFinal) {
    BSLS_ASSERT(0 == d_bufferLength);

    d_buffer[d_bufferLength++] = bdeHex[ch & 0xf];
    d_buffer[d_bufferLength++] = bdeHex[(ch >> 4) & 0xf];

    if (d_outputLength - d_lineStart >= d_maxLineLength - (isFinal ? 2 : 3)) {
        d_buffer[d_bufferLength++] = '=';
        appendSoftLineBreak(out);
    }
    else {
        *out = '=';
        ++d_outputLength;
    }

    d_lastWasWS = false;

    BSLS_ASSERT(5 >= d_bufferLength);
}

// CREATORS
QuotedPrintableEncoder::QuotedPrintableEncoder(
                         QuotedPrintableEncoder::LineBreakMode  lineBreakMode,
                         int                                    maxLineLength,
                         bslma::Allocator                      *basicAllocator)
: d_lineBreakMode(lineBreakMode)
, d_maxLineLength(maxLineLength)
, d_outputLength(0)
, d_lineLength(0)
, d_equivClass_p(const_cast<char *>(s_defaultEquivClass_p))
, d_state(e_INITIAL_STATE)
, d_bufferLength(0)
, d_lineStart(0)
, d_deffered(0)
, d_lastWasWS(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(4 <= d_maxLineLength);
    BSLS_ASSERT(     d_maxLineLength <= 76);

    if (e_CRLF_MODE != d_lineBreakMode && e_MIXED_MODE != d_lineBreakMode) {
        int len = sizeof(equivalenceClassMap);
        d_equivClass_p = static_cast<char *>(d_allocator_p->allocate(len));
        bsl::memcpy(d_equivClass_p, s_defaultEquivClass_p, len);
        d_equivClass_p['\r'] = CC;
        if (e_BINARY_MODE == d_lineBreakMode) {
            d_equivClass_p['\n'] = CC;
        }
    }
}

QuotedPrintableEncoder::QuotedPrintableEncoder(
                     const char                            *extraCharsToEncode,
                     QuotedPrintableEncoder::LineBreakMode  lineBreakMode,
                     int                                    maxLineLength,
                     bslma::Allocator                      *basicAllocator)
: d_lineBreakMode(lineBreakMode)
, d_maxLineLength(maxLineLength)
, d_outputLength(0)
, d_lineLength(0)
, d_equivClass_p(const_cast<char *>(s_defaultEquivClass_p))
, d_state(e_INITIAL_STATE)
, d_bufferLength(0)
, d_lineStart(0)
, d_deffered(0)
, d_lastWasWS(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(4 <= d_maxLineLength);
    BSLS_ASSERT(     d_maxLineLength <= 76);

    // First copy the default mapping table to data member.

    int len = sizeof(equivalenceClassMap);
    d_equivClass_p = static_cast<char *>(d_allocator_p->allocate(len));
    bsl::memcpy(d_equivClass_p, s_defaultEquivClass_p, len);

    // Now change the specified individual elements.

    if (extraCharsToEncode) {
        len = static_cast<int>(bsl::strlen(extraCharsToEncode));
        for (int i = 0; i < len; ++i) {
            int index = static_cast<unsigned char>(extraCharsToEncode[i]);
            if (PC == d_equivClass_p[index] || WS == d_equivClass_p[index]) {
                d_equivClass_p[index] = CC;
            }
        }
    }

    if (e_CRLF_MODE == d_lineBreakMode || e_MIXED_MODE == d_lineBreakMode) {
        d_equivClass_p['\r'] = CR;
        d_equivClass_p['\n'] = LF;
    }
    else if (e_LF_MODE == d_lineBreakMode) {
        d_equivClass_p['\r'] = CC;
        d_equivClass_p['\n'] = LF;
    }
    else {
        d_equivClass_p['\r'] = CC;
        d_equivClass_p['\n'] = CC;
    }
}

QuotedPrintableEncoder::~QuotedPrintableEncoder()
{
    if (d_equivClass_p != s_defaultEquivClass_p) {
        d_allocator_p->deallocate(d_equivClass_p);
    }
}

// MANIPULATORS
int QuotedPrintableEncoder::convert(char       *out,
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
        d_state = e_ERROR_STATE;
        *numOut = 0;
        *numIn = 0;
        return -1;                                                    // RETURN
    }

    if (0 == maxNumOut) {
        *numOut = 0;
        *numIn = 0;
        return numOutputPending();                                    // RETURN
    }

    if (begin < end) {
        d_state = e_INPUT_STATE;
    }

    const int   originalOutputLength = d_outputLength;
    const char *originalBegin        = begin;
    int         maxOutLen            = d_outputLength + maxNumOut;

    while (   d_outputLength != maxOutLen
           && (begin < end || d_bufferLength)) {

        while (d_bufferLength && d_outputLength != maxOutLen) {
            *out++ = d_buffer[--d_bufferLength];
            ++d_outputLength;
        }

        if (d_deffered && d_outputLength != maxOutLen && begin < end) {
            if (d_deffered == '\r') {
                if (LF == d_equivClass_p[static_cast<unsigned char>(*begin)]) {
                    appendHardLineBreak(out++);
                    ++begin;
                }
                else {
                    appendAsHex(out++, d_deffered);
                }
            }
            else {
                if (LF == d_equivClass_p[static_cast<unsigned char>(*begin)]
                 || CR == d_equivClass_p[static_cast<unsigned char>(*begin)]) {
                    appendAsHex(out++, d_deffered);
                }
                else {
                    appendPrintable(out++, d_deffered);
                }
            }
            d_deffered = 0;
        }

        while (   d_outputLength != maxOutLen
               && begin < end
               && 0 == d_deffered
               && 0 == d_bufferLength) {
            switch (d_equivClass_p[static_cast<unsigned char>(*begin)]) {
              case PC: {
                appendPrintable(out++, *begin++);
              } break;
              case CC: {
                appendAsHex(out++, *begin++);
              } break;
              case WS: {
                d_deffered = *begin++;
              } break;
              case CR: {
                d_deffered = *begin++;
              } break;
              case LF: {
                if (   e_LF_MODE    == d_lineBreakMode
                    || e_MIXED_MODE == d_lineBreakMode) {
                    appendHardLineBreak(out++);
                    ++begin;
                }
                else {
                    appendAsHex(out++, *begin++);
                }
              } break;
            }
        }
    }

    *numOut = d_outputLength - originalOutputLength;
    *numIn = static_cast<int>(begin - originalBegin);

    return numOutputPending();
}

int QuotedPrintableEncoder::endConvert(char *out, int *numOut, int maxNumOut)
{
    enum { e_ERROR = -1 };

    if (d_state == e_ERROR_STATE || isDone()) {
        d_state = e_ERROR_STATE;
        *numOut = 0;
        return e_ERROR;                                               // RETURN
    }

    d_state = e_DONE_STATE;

    const int originalOutputLength = d_outputLength;
    int maxOutLen = d_outputLength + maxNumOut;

    if (d_deffered && d_outputLength != maxOutLen) {
        appendAsHex(out++, d_deffered, true);
        d_deffered = 0;
    }

    while (d_bufferLength && d_outputLength != maxOutLen) {
        *out++ = d_buffer[--d_bufferLength];
        ++d_outputLength;
    }

    *numOut = d_outputLength - originalOutputLength;

    return numOutputPending();
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
