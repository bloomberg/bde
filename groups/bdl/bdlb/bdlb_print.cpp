// bdlb_print.cpp                                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlb_print.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_print_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_cassert.h>
#include <bsl_cstdio.h>       // 'bsl::sprintf'
#include <bsl_iomanip.h>
#include <bsl_ostream.h>

namespace BloombergLP {

// COMPILE-TIME ASSERTIONS
BSLMF_ASSERT(4 == sizeof(void *) || 8 == sizeof(void *));

// STATIC DATA
static const char hexDumpChars[256] = {
    '.'   , //   0   0
    '.'   , //   1   1
    '.'   , //   2   2
    '.'   , //   3   3
    '.'   , //   4   4
    '.'   , //   5   5
    '.'   , //   6   6
    '.'   , //   7   7
    '.'   , //   8   8 - BACKSPACE
    '.'   , //   9   9 - TAB
    '.'   , //  10   a - LF
    '.'   , //  11   b
    '.'   , //  12   c
    '.'   , //  13   d - CR
    '.'   , //  14   e
    '.'   , //  15   f
    '.'   , //  16  10
    '.'   , //  17  11
    '.'   , //  18  12
    '.'   , //  19  13
    '.'   , //  20  14
    '.'   , //  21  15
    '.'   , //  22  16
    '.'   , //  23  17
    '.'   , //  24  18
    '.'   , //  25  19
    '.'   , //  26  1a
    '.'   , //  27  1b
    '.'   , //  28  1c
    '.'   , //  29  1d
    '.'   , //  30  1e
    '.'   , //  31  1f
    ' '   , //  32  20 - SPACE
    '!'   , //  33  21 - !
    '\"'  , //  34  22 - "
    '#'   , //  35  23 - #
    '$'   , //  36  24 - $
    '%'   , //  37  25 - %
    '&'   , //  38  26 - &
    '\''  , //  39  27 - '
    '('   , //  40  28 - (
    ')'   , //  41  29 - )
    '*'   , //  42  2a - *
    '+'   , //  43  2b - +
    ','   , //  44  2c - ,
    '-'   , //  45  2d - -
    '.'   , //  46  2e - .
    '/'   , //  47  2f - /
    '0'   , //  48  30 - 0
    '1'   , //  49  31 - 1
    '2'   , //  50  32 - 2
    '3'   , //  51  33 - 3
    '4'   , //  52  34 - 4
    '5'   , //  53  35 - 5
    '6'   , //  54  36 - 6
    '7'   , //  55  37 - 7
    '8'   , //  56  38 - 8
    '9'   , //  57  39 - 9
    ':'   , //  58  3a - :
    ';'   , //  59  3b - ;
    '<'   , //  60  3c - <
    '='   , //  61  3d - =
    '>'   , //  62  3e - >
    '?'   , //  63  3f - ?
    '@'   , //  64  40 - @
    'A'   , //  65  41 - A
    'B'   , //  66  42 - B
    'C'   , //  67  43 - C
    'D'   , //  68  44 - D
    'E'   , //  69  45 - E
    'F'   , //  70  46 - F
    'G'   , //  71  47 - G
    'H'   , //  72  48 - H
    'I'   , //  73  49 - I
    'J'   , //  74  4a - J
    'K'   , //  75  4b - K
    'L'   , //  76  4c - L
    'M'   , //  77  4d - M
    'N'   , //  78  4e - N
    'O'   , //  79  4f - O
    'P'   , //  80  50 - P
    'Q'   , //  81  51 - Q
    'R'   , //  82  52 - R
    'S'   , //  83  53 - S
    'T'   , //  84  54 - T
    'U'   , //  85  55 - U
    'V'   , //  86  56 - V
    'W'   , //  87  57 - W
    'X'   , //  88  58 - X
    'Y'   , //  89  59 - Y
    'Z'   , //  90  5a - Z
    '['   , //  91  5b - [
    '\\'  , //  92  5c - '\'
    ']'   , //  93  5d - ]
    '^'   , //  94  5e - ^
    '_'   , //  95  5f - _
    '`'   , //  96  60 - `
    'a'   , //  97  61 - a
    'b'   , //  98  62 - b
    'c'   , //  99  63 - c
    'd'   , // 100  64 - d
    'e'   , // 101  65 - e
    'f'   , // 102  66 - f
    'g'   , // 103  67 - g
    'h'   , // 104  68 - h
    'i'   , // 105  69 - i
    'j'   , // 106  6a - j
    'k'   , // 107  6b - k
    'l'   , // 108  6c - l
    'm'   , // 109  6d - m
    'n'   , // 110  6e - n
    'o'   , // 111  6f - o
    'p'   , // 112  70 - p
    'q'   , // 113  71 - q
    'r'   , // 114  72 - r
    's'   , // 115  73 - s
    't'   , // 116  74 - t
    'u'   , // 117  75 - u
    'v'   , // 118  76 - v
    'w'   , // 119  77 - w
    'x'   , // 120  78 - x
    'y'   , // 121  79 - y
    'z'   , // 122  7a - z
    '{'   , // 123  7b - {
    '|'   , // 124  7c - |
    '}'   , // 125  7d - }
    '~'   , // 126  7e - ~
    '.'   , // 127  7f - DEL
    '.'   , // 128  80
    '.'   , // 129  81
    '.'   , // 130  82
    '.'   , // 131  83
    '.'   , // 132  84
    '.'   , // 133  85
    '.'   , // 134  86
    '.'   , // 135  87
    '.'   , // 136  88
    '.'   , // 137  89
    '.'   , // 138  8a
    '.'   , // 139  8b
    '.'   , // 140  8c
    '.'   , // 141  8d
    '.'   , // 142  8e
    '.'   , // 143  8f
    '.'   , // 144  90
    '.'   , // 145  91
    '.'   , // 146  92
    '.'   , // 147  93
    '.'   , // 148  94
    '.'   , // 149  95
    '.'   , // 150  96
    '.'   , // 151  97
    '.'   , // 152  98
    '.'   , // 153  99
    '.'   , // 154  9a
    '.'   , // 155  9b
    '.'   , // 156  9c
    '.'   , // 157  9d
    '.'   , // 158  9e
    '.'   , // 159  9f
    '.'   , // 160  a0
    '.'   , // 161  a1
    '.'   , // 162  a2
    '.'   , // 163  a3
    '.'   , // 164  a4
    '.'   , // 165  a5
    '.'   , // 166  a6
    '.'   , // 167  a7
    '.'   , // 168  a8
    '.'   , // 169  a9
    '.'   , // 170  aa
    '.'   , // 171  ab
    '.'   , // 172  ac
    '.'   , // 173  ad
    '.'   , // 174  ae
    '.'   , // 175  af
    '.'   , // 176  b0
    '.'   , // 177  b1
    '.'   , // 178  b2
    '.'   , // 179  b3
    '.'   , // 180  b4
    '.'   , // 181  b5
    '.'   , // 182  b6
    '.'   , // 183  b7
    '.'   , // 184  b8
    '.'   , // 185  b9
    '.'   , // 186  ba
    '.'   , // 187  bb
    '.'   , // 188  bc
    '.'   , // 189  bd
    '.'   , // 190  be
    '.'   , // 191  bf
    '.'   , // 192  c0
    '.'   , // 193  c1
    '.'   , // 194  c2
    '.'   , // 195  c3
    '.'   , // 196  c4
    '.'   , // 197  c5
    '.'   , // 198  c6
    '.'   , // 199  c7
    '.'   , // 200  c8
    '.'   , // 201  c9
    '.'   , // 202  ca
    '.'   , // 203  cb
    '.'   , // 204  cc
    '.'   , // 205  cd
    '.'   , // 206  ce
    '.'   , // 207  cf
    '.'   , // 208  d0
    '.'   , // 209  d1
    '.'   , // 210  d2
    '.'   , // 211  d3
    '.'   , // 212  d4
    '.'   , // 213  d5
    '.'   , // 214  d6
    '.'   , // 215  d7
    '.'   , // 216  d8
    '.'   , // 217  d9
    '.'   , // 218  da
    '.'   , // 219  db
    '.'   , // 220  dc
    '.'   , // 221  dd
    '.'   , // 222  de
    '.'   , // 223  df
    '.'   , // 224  e0
    '.'   , // 225  e1
    '.'   , // 226  e2
    '.'   , // 227  e3
    '.'   , // 228  e4
    '.'   , // 229  e5
    '.'   , // 230  e6
    '.'   , // 231  e7
    '.'   , // 232  e8
    '.'   , // 233  e9
    '.'   , // 234  ea
    '.'   , // 235  eb
    '.'   , // 236  ec
    '.'   , // 237  ed
    '.'   , // 238  ee
    '.'   , // 239  ef
    '.'   , // 240  f0
    '.'   , // 241  f1
    '.'   , // 242  f2
    '.'   , // 243  f3
    '.'   , // 244  f4
    '.'   , // 245  f5
    '.'   , // 246  f6
    '.'   , // 247  f7
    '.'   , // 248  f8
    '.'   , // 249  f9
    '.'   , // 250  fa
    '.'   , // 251  fb
    '.'   , // 252  fc
    '.'   , // 253  fd
    '.'   , // 254  fe
    '.'     // 255  ff
};

// STATIC HELPER FUNCTIONS
static
void putSpaces(bsl::ostream& stream, int numSpaces)
    // Efficiently insert the specified 'numSpaces' spaces into the specified
    // 'stream'.  This function has no effect on 'stream' if 'numSpaces < 0'.
{
    // Algorithm: Write spaces in chunks.  The chunk size is large enough so
    // that most times only a single call to the 'write' method is needed.

    // Define the largest chunk of spaces:
    static const char SPACES[]    = "                                        ";
    const int         SPACES_SIZE = sizeof(SPACES) - 1;

    while (SPACES_SIZE < numSpaces) {
        stream.write(SPACES, SPACES_SIZE);
        numSpaces -= SPACES_SIZE;
    }

    if (0 < numSpaces) {
        stream.write(SPACES, numSpaces);
    }
}

static
void hexDumpFullLine(char *dst, const char *src, int length)
{
    BSLS_ASSERT(dst);
    BSLS_ASSERT(src);
    BSLS_ASSERT(0 <= length);

    const int ret = bsl::sprintf(
                            dst,
                            "%.2X%.2X%.2X%.2X %.2X%.2X%.2X%.2X "
                            "%.2X%.2X%.2X%.2X %.2X%.2X%.2X%.2X"
                            "     |%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c|\n",
                            static_cast<unsigned char>(src[ 0]),
                            static_cast<unsigned char>(src[ 1]),
                            static_cast<unsigned char>(src[ 2]),
                            static_cast<unsigned char>(src[ 3]),
                            static_cast<unsigned char>(src[ 4]),
                            static_cast<unsigned char>(src[ 5]),
                            static_cast<unsigned char>(src[ 6]),
                            static_cast<unsigned char>(src[ 7]),
                            static_cast<unsigned char>(src[ 8]),
                            static_cast<unsigned char>(src[ 9]),
                            static_cast<unsigned char>(src[10]),
                            static_cast<unsigned char>(src[11]),
                            static_cast<unsigned char>(src[12]),
                            static_cast<unsigned char>(src[13]),
                            static_cast<unsigned char>(src[14]),
                            static_cast<unsigned char>(src[15]),
                            hexDumpChars[static_cast<unsigned char>(src[ 0])],
                            hexDumpChars[static_cast<unsigned char>(src[ 1])],
                            hexDumpChars[static_cast<unsigned char>(src[ 2])],
                            hexDumpChars[static_cast<unsigned char>(src[ 3])],
                            hexDumpChars[static_cast<unsigned char>(src[ 4])],
                            hexDumpChars[static_cast<unsigned char>(src[ 5])],
                            hexDumpChars[static_cast<unsigned char>(src[ 6])],
                            hexDumpChars[static_cast<unsigned char>(src[ 7])],
                            hexDumpChars[static_cast<unsigned char>(src[ 8])],
                            hexDumpChars[static_cast<unsigned char>(src[ 9])],
                            hexDumpChars[static_cast<unsigned char>(src[10])],
                            hexDumpChars[static_cast<unsigned char>(src[11])],
                            hexDumpChars[static_cast<unsigned char>(src[12])],
                            hexDumpChars[static_cast<unsigned char>(src[13])],
                            hexDumpChars[static_cast<unsigned char>(src[14])],
                            hexDumpChars[static_cast<unsigned char>(src[15])]);
    BSLS_ASSERT(ret == length - 1);
}

namespace bdlb {
                             // ------------
                             // struct Print
                             // ------------

// CLASS METHODS
bsl::ostream& Print::indent(bsl::ostream& stream,
                            int           level,
                            int           spacesPerLevel)
{
    if (spacesPerLevel < 0) {
        spacesPerLevel = -spacesPerLevel;
    }

    putSpaces(stream, level * spacesPerLevel);
    return stream;
}

bsl::ostream& Print::newlineAndIndent(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel)
{
    if (spacesPerLevel < 0) {
        return stream << ' ';                                         // RETURN
    }

    if (level < 0) {
        level = -level;
    }

    stream << '\n';
    putSpaces(stream, level * spacesPerLevel);
    return stream;
}

void Print::printPtr(bsl::ostream& stream, const void *value)
{
    const int  k_BDLB_PRINT_BUF_SIZE = sizeof(value) * 2 + 1;
    char       buf[k_BDLB_PRINT_BUF_SIZE];
    char      *bufp = buf;

    #if defined(BSLS_PLATFORM_CPU_32_BIT)
        // 32 bit pointer
        bsl::sprintf(buf, "%x", reinterpret_cast<unsigned int>(value));
    #else
        // 64 bit pointer
    #if defined(BSLS_PLATFORM_CMP_MSVC)
        bsl::sprintf(buf, "%I64x", value);
    #else
        bsl::sprintf(buf, "%llx", reinterpret_cast<unsigned long long>(value));
    #endif
    #endif

    // Some platforms print leading zeros, so strip them.

    while ('0' == *bufp) {
        ++bufp;
    }

    if ('\0' == *bufp) {
        --bufp;
    }

    stream << bufp;
}

bsl::ostream& Print::printString(bsl::ostream&  stream,
                                 const char    *string,
                                 int            length,
                                 bool           escapeBackSlash)
{
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(string || 0 == length);

    const char *p   = string;
    const char *q   = p;
    const char *end = p + length;

    while (q != end) {
        if (*q < 0x20 || *q > 0x7E) {  // not printable
            stream.write(p, q - p);

            const char HEX[] = "0123456789ABCDEF";
            const char value = *q;

            stream << "\\x"
                   << HEX[(value >> 4) & 0xF]
                   << HEX[value        & 0xF];

            ++q;
            p = q;
        }
        else if ('\\' == *q && escapeBackSlash) {
            stream.write(p, q - p);
            stream << "\\\\";
            ++q;
            p = q;
        }
        else {
            ++q;
        }
    }

    stream.write(p, q - p);
    return stream;
}

bsl::ostream& Print::hexDump(bsl::ostream&                stream,
                             bsl::pair<const char *,int> *buffers,
                             int                          numBuffers)
{
    BSLS_ASSERT(0 <= numBuffers);
    BSLS_ASSERT(buffers || 0 == numBuffers);

    enum {
        k_BLOCK_SIZE         =  4,
        k_CHAR_PER_LINE      = 16,
        k_SCRATCH_BUFFER_LEN = 60  // line size  + '\0'
    };

    char scratch[k_SCRATCH_BUFFER_LEN];

    int bufferIndex    = 0;  // Current buffer.
    int charIndex      = 0;  // Index in buffer of next byte to be read.
    int preBufferIndex = 0;  // Saves the previous buffer index.  Used on
                             // transitions between buffers.
    int preCharIndex   = 0;  // Saves the last write position in the previous
                             // buffer.  Used on transitions between buffers.

    const char *buffer = buffers[bufferIndex].first;  // Data.
    int         length = buffers[bufferIndex].second; // Size of buffer.

    int lineIndex = 0;
    while (bufferIndex < numBuffers) {
        int ret = bsl::sprintf(scratch,
                               "%6d:   ",
                               lineIndex * k_CHAR_PER_LINE);
        stream.write(scratch, ret);
        if (charIndex + k_CHAR_PER_LINE <= length) {
            hexDumpFullLine(scratch, buffer + charIndex, k_SCRATCH_BUFFER_LEN);
            stream.write(scratch, k_SCRATCH_BUFFER_LEN - 1);
            charIndex += k_CHAR_PER_LINE;
            if (charIndex == length) {
                bufferIndex++;
                if (bufferIndex < numBuffers) {
                    buffer = buffers[bufferIndex].first;
                    length = buffers[bufferIndex].second;
                    charIndex = 0;
                }
            }
        }
        else {
            preBufferIndex = bufferIndex;  // Save indices so we can get back
            preCharIndex   = charIndex;    // to them when printing out ASCII.

            for (int j = 0; j < k_CHAR_PER_LINE; ++j) {
                if (charIndex < length) {
                    ret = bsl::sprintf(
                                scratch,
                                "%.2X",
                                static_cast<unsigned char>(buffer[charIndex]));
                    stream.write(scratch, ret);
                    charIndex++;
                }
                else if (bufferIndex >= numBuffers) {
                    stream.write("  ", sizeof("  ") - 1);
                }

                if (charIndex == length) {
                    bufferIndex++;
                    if (bufferIndex < numBuffers) {
                        buffer = buffers[bufferIndex].first;
                        length = buffers[bufferIndex].second;
                        charIndex = 0;
                    }
                }

                if (j % k_BLOCK_SIZE == 3) {
                    stream.put(' ');  // Spacing.
                }
            }

            stream.write("    |", sizeof("    |") - 1);

            bufferIndex = preBufferIndex;  // Reset to previous position,
            charIndex   = preCharIndex;    // we are printing out the same
                                           // stuff just in a different format.

            buffer = buffers[preBufferIndex].first;
            length = buffers[preBufferIndex].second;

            for (int j = 0; j < k_CHAR_PER_LINE; ++j) {
                if (charIndex < length) {
                    stream.put(hexDumpChars[static_cast<unsigned char>(
                                                          buffer[charIndex])]);
                    charIndex++;
                }
                else if (bufferIndex >= numBuffers) {
                    stream.put(' ');
                }

                if (charIndex == length) {
                    bufferIndex++;
                    if (bufferIndex < numBuffers) {
                        buffer    = buffers[bufferIndex].first;
                        length    = buffers[bufferIndex].second;
                        charIndex = 0;
                    }
                }
            }
            stream.write("|\n",sizeof("|\n") - 1);
        }
        lineIndex++;
    }

    return stream;
}

bsl::ostream& Print::hexDump(bsl::ostream&  stream,
                             const char    *buffer,
                             int            length)
{
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(buffer || 0 == length);

    enum {
        k_BLOCK_SIZE         =  4,
        k_CHAR_PER_LINE      = 16,
        k_SCRATCH_BUFFER_LEN = 60  // line size  + '\0'
    };

    char scratch[k_SCRATCH_BUFFER_LEN];
    for (int i = 0; i < length; i += k_CHAR_PER_LINE) {
        int j;

        int ret = bsl::sprintf(scratch, "%6d:   ", i);
        stream.write(scratch, ret);
        if (i + k_CHAR_PER_LINE <= length) {
            hexDumpFullLine(scratch, buffer + i, k_SCRATCH_BUFFER_LEN);
            stream.write(scratch, k_SCRATCH_BUFFER_LEN - 1);
        }
        else {
            for (j = 0; j < k_CHAR_PER_LINE; ++j) {
                if ((i + j) < length) {
                    ret = bsl::sprintf(
                                    scratch,
                                    "%.2X",
                                    static_cast<unsigned char>(buffer[i + j]));
                    stream.write(scratch, ret);
                }
                else {
                    stream.write("  ", sizeof("  ") -1);
                }
                if (j % k_BLOCK_SIZE == 3) {
                    stream.put(' ');
                }
            }
            stream << "    |";
            for (j = 0; j < k_CHAR_PER_LINE; ++j) {
                if ((i + j) < length) {
                    stream.put(hexDumpChars[static_cast<unsigned char>(
                                                              buffer[i + j])]);
                }
                else {
                    stream.put(' ');
                }
            }
            stream.write("|\n", sizeof("|\n") - 1);
        }
    }

    return stream;
}

bsl::ostream& Print::singleLineHexDump(bsl::ostream&  stream,
                                       const char    *begin,
                                       const char    *end)
{
    BSLS_ASSERT(begin <= end);

    return singleLineHexDump<const char *>(stream, begin, end);
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
