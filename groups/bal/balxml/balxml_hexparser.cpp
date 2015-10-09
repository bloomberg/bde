// balxml_hexparser.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_hexparser.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_hexparser_cpp,"$Id$ $CSID$")

namespace BloombergLP {

const char balxml::HexParser_Helper::s_hexValueTable[128] =
{
    0  , //   0   0
    0  , //   1   1
    0  , //   2   2
    0  , //   3   3
    0  , //   4   4
    0  , //   5   5
    0  , //   6   6
    0  , //   7   7
    0  , //   8   8 - BACKSPACE
    0  , //   9   9 - TAB
    0  , //  10   a - LF
    0  , //  11   b
    0  , //  12   c
    0  , //  13   d - CR
    0  , //  14   e
    0  , //  15   f
    0  , //  16  10
    0  , //  17  11
    0  , //  18  12
    0  , //  19  13
    0  , //  20  14
    0  , //  21  15
    0  , //  22  16
    0  , //  23  17
    0  , //  24  18
    0  , //  25  19
    0  , //  26  1a
    0  , //  27  1b
    0  , //  28  1c
    0  , //  29  1d
    0  , //  30  1e
    0  , //  31  1f
    0  , //  32  20 - SPACE
    0  , //  33  21 - !
    0  , //  34  22 - "
    0  , //  35  23 - #
    0  , //  36  24 - $
    0  , //  37  25 - %
    0  , //  38  26 - &
    0  , //  39  27 - '
    0  , //  40  28 - (
    0  , //  41  29 - )
    0  , //  42  2a - *
    0  , //  43  2b - +
    0  , //  44  2c - ,
    0  , //  45  2d - -
    0  , //  46  2e - .
    0  , //  47  2f - /
    0  , //  48  30 - 0
    1  , //  49  31 - 1
    2  , //  50  32 - 2
    3  , //  51  33 - 3
    4  , //  52  34 - 4
    5  , //  53  35 - 5
    6  , //  54  36 - 6
    7  , //  55  37 - 7
    8  , //  56  38 - 8
    9  , //  57  39 - 9
    0  , //  58  3a - :
    0  , //  59  3b - ;
    0  , //  60  3c - <
    0  , //  61  3d - =
    0  , //  62  3e - >
    0  , //  63  3f - ?
    0  , //  64  40 - @
    10 , //  65  41 - A
    11 , //  66  42 - B
    12 , //  67  43 - C
    13 , //  68  44 - D
    14 , //  69  45 - E
    15 , //  70  46 - F
    0  , //  71  47 - G
    0  , //  72  48 - H
    0  , //  73  49 - I
    0  , //  74  4a - J
    0  , //  75  4b - K
    0  , //  76  4c - L
    0  , //  77  4d - M
    0  , //  78  4e - N
    0  , //  79  4f - O
    0  , //  80  50 - P
    0  , //  81  51 - Q
    0  , //  82  52 - R
    0  , //  83  53 - S
    0  , //  84  54 - T
    0  , //  85  55 - U
    0  , //  86  56 - V
    0  , //  87  57 - W
    0  , //  88  58 - X
    0  , //  89  59 - Y
    0  , //  90  5a - Z
    0  , //  91  5b - [
    0  , //  92  5c - '\'
    0  , //  93  5d - ]
    0  , //  94  5e - ^
    0  , //  95  5f - _
    0  , //  96  60 - `
    10 , //  97  61 - a
    11 , //  98  62 - b
    12 , //  99  63 - c
    13 , // 100  64 - d
    14 , // 101  65 - e
    15 , // 102  66 - f
    0  , // 103  67 - g
    0  , // 104  68 - h
    0  , // 105  69 - i
    0  , // 106  6a - j
    0  , // 107  6b - k
    0  , // 108  6c - l
    0  , // 109  6d - m
    0  , // 110  6e - n
    0  , // 111  6f - o
    0  , // 112  70 - p
    0  , // 113  71 - q
    0  , // 114  72 - r
    0  , // 115  73 - s
    0  , // 116  74 - t
    0  , // 117  75 - u
    0  , // 118  76 - v
    0  , // 119  77 - w
    0  , // 120  78 - x
    0  , // 121  79 - y
    0  , // 122  7a - z
    0  , // 123  7b - {
    0  , // 124  7c - |
    0  , // 125  7d - }
    0  , // 126  7e - ~
    0    // 127  7f - DEL
};

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
