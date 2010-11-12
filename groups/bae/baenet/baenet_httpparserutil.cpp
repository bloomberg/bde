// baenet_httpparserutil.cpp    -*-C++-*-
#include <baenet_httpparserutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httpparserutil_cpp,"$Id$ $CSID$")

#include <baenet_httpcontenttype.h>
#include <baenet_httphost.h>
#include <baenet_httprequestline.h>
#include <baenet_httprequestmethod.h>
#include <baenet_httpstatuscode.h>
#include <baenet_httpstatusline.h>
#include <baenet_httpviarecord.h>

#include <bdema_sequentialallocator.h>
#include <bdet_datetime.h>
#include <bdet_datetimetz.h>
#include <bdeu_chartype.h>
#include <bdeu_string.h>
#include <bdeut_stringref.h>

#include <bsls_platformutil.h>

#include <bsls_assert.h>

#include <bsl_streambuf.h>

// TBD: clean up this component!!!!

namespace BloombergLP {

namespace {

// CONSTANTS

enum {
    GROWTH_FACTOR = 2
};

enum {
    BAENET_SUCCESS            =  0
  , BAENET_FAILURE            = -1
  , BAENET_ELEMENT_NOT_FOUND  = -2
  , BAENET_REACHED_EOF        =  1
  , BAENET_END_OF_HEADER      =  2
  , BAENET_MAX_BYTES_EXCEEDED =  3
};

const bsl::char_traits<char>::int_type eof = bsl::char_traits<char>::eof();
    // End-of-file character.

static const char atomCharsTable[256]=
{
    1 , //   0   0
    1 , //   1   1
    1 , //   2   2
    1 , //   3   3
    1 , //   4   4
    1 , //   5   5
    1 , //   6   6
    1 , //   7   7
    1 , //   8   8 - BACKSPACE
    0 , //   9   9 - TAB
    0 , //  10   a - LF
    1 , //  11   b
    1 , //  12   c
    0 , //  13   d - CR
    1 , //  14   e
    1 , //  15   f
    1 , //  16  10
    1 , //  17  11
    1 , //  18  12
    1 , //  19  13
    1 , //  20  14
    1 , //  21  15
    1 , //  22  16
    1 , //  23  17
    1 , //  24  18
    1 , //  25  19
    1 , //  26  1a
    1 , //  27  1b
    1 , //  28  1c
    1 , //  29  1d
    1 , //  30  1e
    1 , //  31  1f
    0 , //  32  20 - SPACE
    1 , //  33  21 - !
    0 , //  34  22 - "
    1 , //  35  23 - #
    1 , //  36  24 - $
    1 , //  37  25 - %
    1 , //  38  26 - &
    1 , //  39  27 - '
    0 , //  40  28 - (
    0 , //  41  29 - )
    1 , //  42  2a - *
    1 , //  43  2b - +
    0 , //  44  2c - ,
    1 , //  45  2d - -
    0 , //  46  2e - .
    0 , //  47  2f - /
    1 , //  48  30 - 0
    1 , //  49  31 - 1
    1 , //  50  32 - 2
    1 , //  51  33 - 3
    1 , //  52  34 - 4
    1 , //  53  35 - 5
    1 , //  54  36 - 6
    1 , //  55  37 - 7
    1 , //  56  38 - 8
    1 , //  57  39 - 9
    0 , //  58  3a - :
    0 , //  59  3b - ;
    0 , //  60  3c - <
    0 , //  61  3d - =
    0 , //  62  3e - >
    1 , //  63  3f - ?
    0 , //  64  40 - @
    1 , //  65  41 - A
    1 , //  66  42 - B
    1 , //  67  43 - C
    1 , //  68  44 - D
    1 , //  69  45 - E
    1 , //  70  46 - F
    1 , //  71  47 - G
    1 , //  72  48 - H
    1 , //  73  49 - I
    1 , //  74  4a - J
    1 , //  75  4b - K
    1 , //  76  4c - L
    1 , //  77  4d - M
    1 , //  78  4e - N
    1 , //  79  4f - O
    1 , //  80  50 - P
    1 , //  81  51 - Q
    1 , //  82  52 - R
    1 , //  83  53 - S
    1 , //  84  54 - T
    1 , //  85  55 - U
    1 , //  86  56 - V
    1 , //  87  57 - W
    1 , //  88  58 - X
    1 , //  89  59 - Y
    1 , //  90  5a - Z
    0 , //  91  5b - [
    1 , //  92  5c - '\'
    0 , //  93  5d - ]
    1 , //  94  5e - ^
    1 , //  95  5f - _
    1 , //  96  60 - `
    1 , //  97  61 - a
    1 , //  98  62 - b
    1 , //  99  63 - c
    1 , // 100  64 - d
    1 , // 101  65 - e
    1 , // 102  66 - f
    1 , // 103  67 - g
    1 , // 104  68 - h
    1 , // 105  69 - i
    1 , // 106  6a - j
    1 , // 107  6b - k
    1 , // 108  6c - l
    1 , // 109  6d - m
    1 , // 110  6e - n
    1 , // 111  6f - o
    1 , // 112  70 - p
    1 , // 113  71 - q
    1 , // 114  72 - r
    1 , // 115  73 - s
    1 , // 116  74 - t
    1 , // 117  75 - u
    1 , // 118  76 - v
    1 , // 119  77 - w
    1 , // 120  78 - x
    1 , // 121  79 - y
    1 , // 122  7a - z
    1 , // 123  7b - {
    1 , // 124  7c - |
    1 , // 125  7d - }
    1 , // 126  7e - ~
    1 , // 127  7f - DEL
    1 , // 128  80
    1 , // 129  81
    1 , // 130  82
    1 , // 131  83
    1 , // 132  84
    1 , // 133  85
    1 , // 134  86
    1 , // 135  87
    1 , // 136  88
    1 , // 137  89
    1 , // 138  8a
    1 , // 139  8b
    1 , // 140  8c
    1 , // 141  8d
    1 , // 142  8e
    1 , // 143  8f
    1 , // 144  90
    1 , // 145  91
    1 , // 146  92
    1 , // 147  93
    1 , // 148  94
    1 , // 149  95
    1 , // 150  96
    1 , // 151  97
    1 , // 152  98
    1 , // 153  99
    1 , // 154  9a
    1 , // 155  9b
    1 , // 156  9c
    1 , // 157  9d
    1 , // 158  9e
    1 , // 159  9f
    1 , // 160  a0
    1 , // 161  a1
    1 , // 162  a2
    1 , // 163  a3
    1 , // 164  a4
    1 , // 165  a5
    1 , // 166  a6
    1 , // 167  a7
    1 , // 168  a8
    1 , // 169  a9
    1 , // 170  aa
    1 , // 171  ab
    1 , // 172  ac
    1 , // 173  ad
    1 , // 174  ae
    1 , // 175  af
    1 , // 176  b0
    1 , // 177  b1
    1 , // 178  b2
    1 , // 179  b3
    1 , // 180  b4
    1 , // 181  b5
    1 , // 182  b6
    1 , // 183  b7
    1 , // 184  b8
    1 , // 185  b9
    1 , // 186  ba
    1 , // 187  bb
    1 , // 188  bc
    1 , // 189  bd
    1 , // 190  be
    1 , // 191  bf
    1 , // 192  c0
    1 , // 193  c1
    1 , // 194  c2
    1 , // 195  c3
    1 , // 196  c4
    1 , // 197  c5
    1 , // 198  c6
    1 , // 199  c7
    1 , // 200  c8
    1 , // 201  c9
    1 , // 202  ca
    1 , // 203  cb
    1 , // 204  cc
    1 , // 205  cd
    1 , // 206  ce
    1 , // 207  cf
    1 , // 208  d0
    1 , // 209  d1
    1 , // 210  d2
    1 , // 211  d3
    1 , // 212  d4
    1 , // 213  d5
    1 , // 214  d6
    1 , // 215  d7
    1 , // 216  d8
    1 , // 217  d9
    1 , // 218  da
    1 , // 219  db
    1 , // 220  dc
    1 , // 221  dd
    1 , // 222  de
    1 , // 223  df
    1 , // 224  e0
    1 , // 225  e1
    1 , // 226  e2
    1 , // 227  e3
    1 , // 228  e4
    1 , // 229  e5
    1 , // 230  e6
    1 , // 231  e7
    1 , // 232  e8
    1 , // 233  e9
    1 , // 234  ea
    1 , // 235  eb
    1 , // 236  ec
    1 , // 237  ed
    1 , // 238  ee
    1 , // 239  ef
    1 , // 240  f0
    1 , // 241  f1
    1 , // 242  f2
    1 , // 243  f3
    1 , // 244  f4
    1 , // 245  f5
    1 , // 246  f6
    1 , // 247  f7
    1 , // 248  f8
    1 , // 249  f9
    1 , // 250  fa
    1 , // 251  fb
    1 , // 252  fc
    1 , // 253  fd
    1 , // 254  fe
    1   // 255  ff
};
    // Characters that can be used in an atom.

static const char headerCharsTable[256]=
{
    1 , //   0   0
    1 , //   1   1
    1 , //   2   2
    1 , //   3   3
    1 , //   4   4
    1 , //   5   5
    1 , //   6   6
    1 , //   7   7
    1 , //   8   8 - BACKSPACE
    0 , //   9   9 - TAB
    0 , //  10   a - LF
    1 , //  11   b
    1 , //  12   c
    0 , //  13   d - CR
    1 , //  14   e
    1 , //  15   f
    1 , //  16  10
    1 , //  17  11
    1 , //  18  12
    1 , //  19  13
    1 , //  20  14
    1 , //  21  15
    1 , //  22  16
    1 , //  23  17
    1 , //  24  18
    1 , //  25  19
    1 , //  26  1a
    1 , //  27  1b
    1 , //  28  1c
    1 , //  29  1d
    1 , //  30  1e
    1 , //  31  1f
    0 , //  32  20 - SPACE
    1 , //  33  21 - !
    1 , //  34  22 - "
    1 , //  35  23 - #
    1 , //  36  24 - $
    1 , //  37  25 - %
    1 , //  38  26 - &
    1 , //  39  27 - '
    0 , //  40  28 - (
    1 , //  41  29 - )
    1 , //  42  2a - *
    1 , //  43  2b - +
    1 , //  44  2c - ,
    1 , //  45  2d - -
    1 , //  46  2e - .
    1 , //  47  2f - /
    1 , //  48  30 - 0
    1 , //  49  31 - 1
    1 , //  50  32 - 2
    1 , //  51  33 - 3
    1 , //  52  34 - 4
    1 , //  53  35 - 5
    1 , //  54  36 - 6
    1 , //  55  37 - 7
    1 , //  56  38 - 8
    1 , //  57  39 - 9
    0 , //  58  3a - :
    1 , //  59  3b - ;
    1 , //  60  3c - <
    1 , //  61  3d - =
    1 , //  62  3e - >
    1 , //  63  3f - ?
    1 , //  64  40 - @
    1 , //  65  41 - A
    1 , //  66  42 - B
    1 , //  67  43 - C
    1 , //  68  44 - D
    1 , //  69  45 - E
    1 , //  70  46 - F
    1 , //  71  47 - G
    1 , //  72  48 - H
    1 , //  73  49 - I
    1 , //  74  4a - J
    1 , //  75  4b - K
    1 , //  76  4c - L
    1 , //  77  4d - M
    1 , //  78  4e - N
    1 , //  79  4f - O
    1 , //  80  50 - P
    1 , //  81  51 - Q
    1 , //  82  52 - R
    1 , //  83  53 - S
    1 , //  84  54 - T
    1 , //  85  55 - U
    1 , //  86  56 - V
    1 , //  87  57 - W
    1 , //  88  58 - X
    1 , //  89  59 - Y
    1 , //  90  5a - Z
    1 , //  91  5b - [
    1 , //  92  5c - '\'
    1 , //  93  5d - ]
    1 , //  94  5e - ^
    1 , //  95  5f - _
    1 , //  96  60 - `
    1 , //  97  61 - a
    1 , //  98  62 - b
    1 , //  99  63 - c
    1 , // 100  64 - d
    1 , // 101  65 - e
    1 , // 102  66 - f
    1 , // 103  67 - g
    1 , // 104  68 - h
    1 , // 105  69 - i
    1 , // 106  6a - j
    1 , // 107  6b - k
    1 , // 108  6c - l
    1 , // 109  6d - m
    1 , // 110  6e - n
    1 , // 111  6f - o
    1 , // 112  70 - p
    1 , // 113  71 - q
    1 , // 114  72 - r
    1 , // 115  73 - s
    1 , // 116  74 - t
    1 , // 117  75 - u
    1 , // 118  76 - v
    1 , // 119  77 - w
    1 , // 120  78 - x
    1 , // 121  79 - y
    1 , // 122  7a - z
    1 , // 123  7b - {
    1 , // 124  7c - |
    1 , // 125  7d - }
    1 , // 126  7e - ~
    1 , // 127  7f - DEL
    1 , // 128  80
    1 , // 129  81
    1 , // 130  82
    1 , // 131  83
    1 , // 132  84
    1 , // 133  85
    1 , // 134  86
    1 , // 135  87
    1 , // 136  88
    1 , // 137  89
    1 , // 138  8a
    1 , // 139  8b
    1 , // 140  8c
    1 , // 141  8d
    1 , // 142  8e
    1 , // 143  8f
    1 , // 144  90
    1 , // 145  91
    1 , // 146  92
    1 , // 147  93
    1 , // 148  94
    1 , // 149  95
    1 , // 150  96
    1 , // 151  97
    1 , // 152  98
    1 , // 153  99
    1 , // 154  9a
    1 , // 155  9b
    1 , // 156  9c
    1 , // 157  9d
    1 , // 158  9e
    1 , // 159  9f
    1 , // 160  a0
    1 , // 161  a1
    1 , // 162  a2
    1 , // 163  a3
    1 , // 164  a4
    1 , // 165  a5
    1 , // 166  a6
    1 , // 167  a7
    1 , // 168  a8
    1 , // 169  a9
    1 , // 170  aa
    1 , // 171  ab
    1 , // 172  ac
    1 , // 173  ad
    1 , // 174  ae
    1 , // 175  af
    1 , // 176  b0
    1 , // 177  b1
    1 , // 178  b2
    1 , // 179  b3
    1 , // 180  b4
    1 , // 181  b5
    1 , // 182  b6
    1 , // 183  b7
    1 , // 184  b8
    1 , // 185  b9
    1 , // 186  ba
    1 , // 187  bb
    1 , // 188  bc
    1 , // 189  bd
    1 , // 190  be
    1 , // 191  bf
    1 , // 192  c0
    1 , // 193  c1
    1 , // 194  c2
    1 , // 195  c3
    1 , // 196  c4
    1 , // 197  c5
    1 , // 198  c6
    1 , // 199  c7
    1 , // 200  c8
    1 , // 201  c9
    1 , // 202  ca
    1 , // 203  cb
    1 , // 204  cc
    1 , // 205  cd
    1 , // 206  ce
    1 , // 207  cf
    1 , // 208  d0
    1 , // 209  d1
    1 , // 210  d2
    1 , // 211  d3
    1 , // 212  d4
    1 , // 213  d5
    1 , // 214  d6
    1 , // 215  d7
    1 , // 216  d8
    1 , // 217  d9
    1 , // 218  da
    1 , // 219  db
    1 , // 220  dc
    1 , // 221  dd
    1 , // 222  de
    1 , // 223  df
    1 , // 224  e0
    1 , // 225  e1
    1 , // 226  e2
    1 , // 227  e3
    1 , // 228  e4
    1 , // 229  e5
    1 , // 230  e6
    1 , // 231  e7
    1 , // 232  e8
    1 , // 233  e9
    1 , // 234  ea
    1 , // 235  eb
    1 , // 236  ec
    1 , // 237  ed
    1 , // 238  ee
    1 , // 239  ef
    1 , // 240  f0
    1 , // 241  f1
    1 , // 242  f2
    1 , // 243  f3
    1 , // 244  f4
    1 , // 245  f5
    1 , // 246  f6
    1 , // 247  f7
    1 , // 248  f8
    1 , // 249  f9
    1 , // 250  fa
    1 , // 251  fb
    1 , // 252  fc
    1 , // 253  fd
    1 , // 254  fe
    1   // 255  ff
};
    // Characters that can be used in a header field.

static const char whitespaceCharsTable[256]=
{
    0 , //   0   0
    0 , //   1   1
    0 , //   2   2
    0 , //   3   3
    0 , //   4   4
    0 , //   5   5
    0 , //   6   6
    0 , //   7   7
    0 , //   8   8 - BACKSPACE
    1 , //   9   9 - TAB
    0 , //  10   a - LF
    0 , //  11   b
    0 , //  12   c
    0 , //  13   d - CR
    0 , //  14   e
    0 , //  15   f
    0 , //  16  10
    0 , //  17  11
    0 , //  18  12
    0 , //  19  13
    0 , //  20  14
    0 , //  21  15
    0 , //  22  16
    0 , //  23  17
    0 , //  24  18
    0 , //  25  19
    0 , //  26  1a
    0 , //  27  1b
    0 , //  28  1c
    0 , //  29  1d
    0 , //  30  1e
    0 , //  31  1f
    1 , //  32  20 - SPACE
    0 , //  33  21 - !
    0 , //  34  22 - "
    0 , //  35  23 - #
    0 , //  36  24 - $
    0 , //  37  25 - %
    0 , //  38  26 - &
    0 , //  39  27 - '
    0 , //  40  28 - (
    0 , //  41  29 - )
    0 , //  42  2a - *
    0 , //  43  2b - +
    0 , //  44  2c - ,
    0 , //  45  2d - -
    0 , //  46  2e - .
    0 , //  47  2f - /
    0 , //  48  30 - 0
    0 , //  49  31 - 1
    0 , //  50  32 - 2
    0 , //  51  33 - 3
    0 , //  52  34 - 4
    0 , //  53  35 - 5
    0 , //  54  36 - 6
    0 , //  55  37 - 7
    0 , //  56  38 - 8
    0 , //  57  39 - 9
    0 , //  58  3a - :
    0 , //  59  3b - ;
    0 , //  60  3c - <
    0 , //  61  3d - =
    0 , //  62  3e - >
    0 , //  63  3f - ?
    0 , //  64  40 - @
    0 , //  65  41 - A
    0 , //  66  42 - B
    0 , //  67  43 - C
    0 , //  68  44 - D
    0 , //  69  45 - E
    0 , //  70  46 - F
    0 , //  71  47 - G
    0 , //  72  48 - H
    0 , //  73  49 - I
    0 , //  74  4a - J
    0 , //  75  4b - K
    0 , //  76  4c - L
    0 , //  77  4d - M
    0 , //  78  4e - N
    0 , //  79  4f - O
    0 , //  80  50 - P
    0 , //  81  51 - Q
    0 , //  82  52 - R
    0 , //  83  53 - S
    0 , //  84  54 - T
    0 , //  85  55 - U
    0 , //  86  56 - V
    0 , //  87  57 - W
    0 , //  88  58 - X
    0 , //  89  59 - Y
    0 , //  90  5a - Z
    0 , //  91  5b - [
    0 , //  92  5c - '\'
    0 , //  93  5d - ]
    0 , //  94  5e - ^
    0 , //  95  5f - _
    0 , //  96  60 - `
    0 , //  97  61 - a
    0 , //  98  62 - b
    0 , //  99  63 - c
    0 , // 100  64 - d
    0 , // 101  65 - e
    0 , // 102  66 - f
    0 , // 103  67 - g
    0 , // 104  68 - h
    0 , // 105  69 - i
    0 , // 106  6a - j
    0 , // 107  6b - k
    0 , // 108  6c - l
    0 , // 109  6d - m
    0 , // 110  6e - n
    0 , // 111  6f - o
    0 , // 112  70 - p
    0 , // 113  71 - q
    0 , // 114  72 - r
    0 , // 115  73 - s
    0 , // 116  74 - t
    0 , // 117  75 - u
    0 , // 118  76 - v
    0 , // 119  77 - w
    0 , // 120  78 - x
    0 , // 121  79 - y
    0 , // 122  7a - z
    0 , // 123  7b - {
    0 , // 124  7c - |
    0 , // 125  7d - }
    0 , // 126  7e - ~
    0 , // 127  7f - DEL
    0 , // 128  80
    0 , // 129  81
    0 , // 130  82
    0 , // 131  83
    0 , // 132  84
    0 , // 133  85
    0 , // 134  86
    0 , // 135  87
    0 , // 136  88
    0 , // 137  89
    0 , // 138  8a
    0 , // 139  8b
    0 , // 140  8c
    0 , // 141  8d
    0 , // 142  8e
    0 , // 143  8f
    0 , // 144  90
    0 , // 145  91
    0 , // 146  92
    0 , // 147  93
    0 , // 148  94
    0 , // 149  95
    0 , // 150  96
    0 , // 151  97
    0 , // 152  98
    0 , // 153  99
    0 , // 154  9a
    0 , // 155  9b
    0 , // 156  9c
    0 , // 157  9d
    0 , // 158  9e
    0 , // 159  9f
    0 , // 160  a0
    0 , // 161  a1
    0 , // 162  a2
    0 , // 163  a3
    0 , // 164  a4
    0 , // 165  a5
    0 , // 166  a6
    0 , // 167  a7
    0 , // 168  a8
    0 , // 169  a9
    0 , // 170  aa
    0 , // 171  ab
    0 , // 172  ac
    0 , // 173  ad
    0 , // 174  ae
    0 , // 175  af
    0 , // 176  b0
    0 , // 177  b1
    0 , // 178  b2
    0 , // 179  b3
    0 , // 180  b4
    0 , // 181  b5
    0 , // 182  b6
    0 , // 183  b7
    0 , // 184  b8
    0 , // 185  b9
    0 , // 186  ba
    0 , // 187  bb
    0 , // 188  bc
    0 , // 189  bd
    0 , // 190  be
    0 , // 191  bf
    0 , // 192  c0
    0 , // 193  c1
    0 , // 194  c2
    0 , // 195  c3
    0 , // 196  c4
    0 , // 197  c5
    0 , // 198  c6
    0 , // 199  c7
    0 , // 200  c8
    0 , // 201  c9
    0 , // 202  ca
    0 , // 203  cb
    0 , // 204  cc
    0 , // 205  cd
    0 , // 206  ce
    0 , // 207  cf
    0 , // 208  d0
    0 , // 209  d1
    0 , // 210  d2
    0 , // 211  d3
    0 , // 212  d4
    0 , // 213  d5
    0 , // 214  d6
    0 , // 215  d7
    0 , // 216  d8
    0 , // 217  d9
    0 , // 218  da
    0 , // 219  db
    0 , // 220  dc
    0 , // 221  dd
    0 , // 222  de
    0 , // 223  df
    0 , // 224  e0
    0 , // 225  e1
    0 , // 226  e2
    0 , // 227  e3
    0 , // 228  e4
    0 , // 229  e5
    0 , // 230  e6
    0 , // 231  e7
    0 , // 232  e8
    0 , // 233  e9
    0 , // 234  ea
    0 , // 235  eb
    0 , // 236  ec
    0 , // 237  ed
    0 , // 238  ee
    0 , // 239  ef
    0 , // 240  f0
    0 , // 241  f1
    0 , // 242  f2
    0 , // 243  f3
    0 , // 244  f4
    0 , // 245  f5
    0 , // 246  f6
    0 , // 247  f7
    0 , // 248  f8
    0 , // 249  f9
    0 , // 250  fa
    0 , // 251  fb
    0 , // 252  fc
    0 , // 253  fd
    0 , // 254  fe
    0   // 255  ff
};
    // Whitespace characters ('\t' and ' ').

enum { NOT_HEX = 127 };

static const char hexCharTable[256]=
{
    NOT_HEX , //   0   0
    NOT_HEX , //   1   1
    NOT_HEX , //   2   2
    NOT_HEX , //   3   3
    NOT_HEX , //   4   4
    NOT_HEX , //   5   5
    NOT_HEX , //   6   6
    NOT_HEX , //   7   7
    NOT_HEX , //   8   8 - BACKSPACE
    NOT_HEX , //   9   9 - TAB
    NOT_HEX , //  10   a - LF
    NOT_HEX , //  11   b
    NOT_HEX , //  12   c
    NOT_HEX , //  13   d - CR
    NOT_HEX , //  14   e
    NOT_HEX , //  15   f
    NOT_HEX , //  16  10
    NOT_HEX , //  17  11
    NOT_HEX , //  18  12
    NOT_HEX , //  19  13
    NOT_HEX , //  20  14
    NOT_HEX , //  21  15
    NOT_HEX , //  22  16
    NOT_HEX , //  23  17
    NOT_HEX , //  24  18
    NOT_HEX , //  25  19
    NOT_HEX , //  26  1a
    NOT_HEX , //  27  1b
    NOT_HEX , //  28  1c
    NOT_HEX , //  29  1d
    NOT_HEX , //  30  1e
    NOT_HEX , //  31  1f
    NOT_HEX , //  32  20 - SPACE
    NOT_HEX , //  33  21 - !
    NOT_HEX , //  34  22 - "
    NOT_HEX , //  35  23 - #
    NOT_HEX , //  36  24 - $
    NOT_HEX , //  37  25 - %
    NOT_HEX , //  38  26 - &
    NOT_HEX , //  39  27 - '
    NOT_HEX , //  40  28 - (
    NOT_HEX , //  41  29 - )
    NOT_HEX , //  42  2a - *
    NOT_HEX , //  43  2b - +
    NOT_HEX , //  44  2c - ,
    NOT_HEX , //  45  2d - -
    NOT_HEX , //  46  2e - .
    NOT_HEX , //  47  2f - /
          0 , //  48  30 - 0
          1 , //  49  31 - 1
          2 , //  50  32 - 2
          3 , //  51  33 - 3
          4 , //  52  34 - 4
          5 , //  53  35 - 5
          6 , //  54  36 - 6
          7 , //  55  37 - 7
          8 , //  56  38 - 8
          9 , //  57  39 - 9
    NOT_HEX , //  58  3a - :
    NOT_HEX , //  59  3b - ;
    NOT_HEX , //  60  3c - <
    NOT_HEX , //  61  3d - =
    NOT_HEX , //  62  3e - >
    NOT_HEX , //  63  3f - ?
    NOT_HEX , //  64  40 - @
         10 , //  65  41 - A
         11 , //  66  42 - B
         12 , //  67  43 - C
         13 , //  68  44 - D
         14 , //  69  45 - E
         15 , //  70  46 - F
    NOT_HEX , //  71  47 - G
    NOT_HEX , //  72  48 - H
    NOT_HEX , //  73  49 - I
    NOT_HEX , //  74  4a - J
    NOT_HEX , //  75  4b - K
    NOT_HEX , //  76  4c - L
    NOT_HEX , //  77  4d - M
    NOT_HEX , //  78  4e - N
    NOT_HEX , //  79  4f - O
    NOT_HEX , //  80  50 - P
    NOT_HEX , //  81  51 - Q
    NOT_HEX , //  82  52 - R
    NOT_HEX , //  83  53 - S
    NOT_HEX , //  84  54 - T
    NOT_HEX , //  85  55 - U
    NOT_HEX , //  86  56 - V
    NOT_HEX , //  87  57 - W
    NOT_HEX , //  88  58 - X
    NOT_HEX , //  89  59 - Y
    NOT_HEX , //  90  5a - Z
    NOT_HEX , //  91  5b - [
    NOT_HEX , //  92  5c - '\'
    NOT_HEX , //  93  5d - ]
    NOT_HEX , //  94  5e - ^
    NOT_HEX , //  95  5f - _
    NOT_HEX , //  96  60 - `
         10 , //  97  61 - a
         11 , //  98  62 - b
         12 , //  99  63 - c
         13 , // 100  64 - d
         14 , // 101  65 - e
         15 , // 102  66 - f
    NOT_HEX , // 103  67 - g
    NOT_HEX , // 104  68 - h
    NOT_HEX , // 105  69 - i
    NOT_HEX , // 106  6a - j
    NOT_HEX , // 107  6b - k
    NOT_HEX , // 108  6c - l
    NOT_HEX , // 109  6d - m
    NOT_HEX , // 110  6e - n
    NOT_HEX , // 111  6f - o
    NOT_HEX , // 112  70 - p
    NOT_HEX , // 113  71 - q
    NOT_HEX , // 114  72 - r
    NOT_HEX , // 115  73 - s
    NOT_HEX , // 116  74 - t
    NOT_HEX , // 117  75 - u
    NOT_HEX , // 118  76 - v
    NOT_HEX , // 119  77 - w
    NOT_HEX , // 120  78 - x
    NOT_HEX , // 121  79 - y
    NOT_HEX , // 122  7a - z
    NOT_HEX , // 123  7b - {
    NOT_HEX , // 124  7c - |
    NOT_HEX , // 125  7d - }
    NOT_HEX , // 126  7e - ~
    NOT_HEX , // 127  7f - DEL
    NOT_HEX , // 128  80
    NOT_HEX , // 129  81
    NOT_HEX , // 130  82
    NOT_HEX , // 131  83
    NOT_HEX , // 132  84
    NOT_HEX , // 133  85
    NOT_HEX , // 134  86
    NOT_HEX , // 135  87
    NOT_HEX , // 136  88
    NOT_HEX , // 137  89
    NOT_HEX , // 138  8a
    NOT_HEX , // 139  8b
    NOT_HEX , // 140  8c
    NOT_HEX , // 141  8d
    NOT_HEX , // 142  8e
    NOT_HEX , // 143  8f
    NOT_HEX , // 144  90
    NOT_HEX , // 145  91
    NOT_HEX , // 146  92
    NOT_HEX , // 147  93
    NOT_HEX , // 148  94
    NOT_HEX , // 149  95
    NOT_HEX , // 150  96
    NOT_HEX , // 151  97
    NOT_HEX , // 152  98
    NOT_HEX , // 153  99
    NOT_HEX , // 154  9a
    NOT_HEX , // 155  9b
    NOT_HEX , // 156  9c
    NOT_HEX , // 157  9d
    NOT_HEX , // 158  9e
    NOT_HEX , // 159  9f
    NOT_HEX , // 160  a0
    NOT_HEX , // 161  a1
    NOT_HEX , // 162  a2
    NOT_HEX , // 163  a3
    NOT_HEX , // 164  a4
    NOT_HEX , // 165  a5
    NOT_HEX , // 166  a6
    NOT_HEX , // 167  a7
    NOT_HEX , // 168  a8
    NOT_HEX , // 169  a9
    NOT_HEX , // 170  aa
    NOT_HEX , // 171  ab
    NOT_HEX , // 172  ac
    NOT_HEX , // 173  ad
    NOT_HEX , // 174  ae
    NOT_HEX , // 175  af
    NOT_HEX , // 176  b0
    NOT_HEX , // 177  b1
    NOT_HEX , // 178  b2
    NOT_HEX , // 179  b3
    NOT_HEX , // 180  b4
    NOT_HEX , // 181  b5
    NOT_HEX , // 182  b6
    NOT_HEX , // 183  b7
    NOT_HEX , // 184  b8
    NOT_HEX , // 185  b9
    NOT_HEX , // 186  ba
    NOT_HEX , // 187  bb
    NOT_HEX , // 188  bc
    NOT_HEX , // 189  bd
    NOT_HEX , // 190  be
    NOT_HEX , // 191  bf
    NOT_HEX , // 192  c0
    NOT_HEX , // 193  c1
    NOT_HEX , // 194  c2
    NOT_HEX , // 195  c3
    NOT_HEX , // 196  c4
    NOT_HEX , // 197  c5
    NOT_HEX , // 198  c6
    NOT_HEX , // 199  c7
    NOT_HEX , // 200  c8
    NOT_HEX , // 201  c9
    NOT_HEX , // 202  ca
    NOT_HEX , // 203  cb
    NOT_HEX , // 204  cc
    NOT_HEX , // 205  cd
    NOT_HEX , // 206  ce
    NOT_HEX , // 207  cf
    NOT_HEX , // 208  d0
    NOT_HEX , // 209  d1
    NOT_HEX , // 210  d2
    NOT_HEX , // 211  d3
    NOT_HEX , // 212  d4
    NOT_HEX , // 213  d5
    NOT_HEX , // 214  d6
    NOT_HEX , // 215  d7
    NOT_HEX , // 216  d8
    NOT_HEX , // 217  d9
    NOT_HEX , // 218  da
    NOT_HEX , // 219  db
    NOT_HEX , // 220  dc
    NOT_HEX , // 221  dd
    NOT_HEX , // 222  de
    NOT_HEX , // 223  df
    NOT_HEX , // 224  e0
    NOT_HEX , // 225  e1
    NOT_HEX , // 226  e2
    NOT_HEX , // 227  e3
    NOT_HEX , // 228  e4
    NOT_HEX , // 229  e5
    NOT_HEX , // 230  e6
    NOT_HEX , // 231  e7
    NOT_HEX , // 232  e8
    NOT_HEX , // 233  e9
    NOT_HEX , // 234  ea
    NOT_HEX , // 235  eb
    NOT_HEX , // 236  ec
    NOT_HEX , // 237  ed
    NOT_HEX , // 238  ee
    NOT_HEX , // 239  ef
    NOT_HEX , // 240  f0
    NOT_HEX , // 241  f1
    NOT_HEX , // 242  f2
    NOT_HEX , // 243  f3
    NOT_HEX , // 244  f4
    NOT_HEX , // 245  f5
    NOT_HEX , // 246  f6
    NOT_HEX , // 247  f7
    NOT_HEX , // 248  f8
    NOT_HEX , // 249  f9
    NOT_HEX , // 250  fa
    NOT_HEX , // 251  fb
    NOT_HEX , // 252  fc
    NOT_HEX , // 253  fd
    NOT_HEX , // 254  fe
    NOT_HEX   // 255  ff
};
    // Hex character values

// HELPER FUNCTIONS

// === streambuf based parsing ======

void makeRoom(char                         **str,
              bsls_PlatformUtil::size_type  *size,
              int                            length,
              bdema_SequentialAllocator     *alloc)
{
    BSLS_ASSERT(alloc);
    BSLS_ASSERT(str);
    BSLS_ASSERT(*str);
    BSLS_ASSERT(size);
    BSLS_ASSERT(0 <= length);

    if (length >= *size) {
        // relocate
        bsls_PlatformUtil::size_type  newSize = length * GROWTH_FACTOR;
        char                         *newStr  = (char *)alloc->
                                                   allocateAndExpand(&newSize);

        BSLS_ASSERT(newStr);
        bsl::memcpy(newStr, *str, length);

        *str  = newStr;
        *size = newSize;
    }
}

int appendLine(char                         **str,
               int                           *strLen,
               bsls_PlatformUtil::size_type  *size,
               int                           *accumNumBytesConsumed,
               bdema_SequentialAllocator     *alloc,
               bsl::streambuf                *source,
               int                            maxNumBytesConsumed)
{
    BSLS_ASSERT(alloc);
    BSLS_ASSERT(str);
    BSLS_ASSERT(*str);
    BSLS_ASSERT(strLen);
    BSLS_ASSERT(size);
    BSLS_ASSERT(source);

    for (;;) {
        bsl::char_traits<char>::int_type c = source->sgetc();

        if (eof == c) {
            return BAENET_REACHED_EOF;
        }
        else if ('\r' == c) {
            c = source->snextc();
            ++*accumNumBytesConsumed;

            if (eof == c) {
                return BAENET_REACHED_EOF;
            }
            else if ('\n' == c) {
                source->snextc();
                ++*accumNumBytesConsumed;

                return BAENET_SUCCESS;
            }

            // Carriage return without line feed.

            makeRoom(str, size, *strLen, alloc);
            (*str)[(*strLen)++] = '\r';
        }
        else {

            // regular char
            makeRoom(str, size, *strLen, alloc);
            (*str)[(*strLen)++] = c;

            source->snextc();
            ++*accumNumBytesConsumed;
        }

        if ( *accumNumBytesConsumed > maxNumBytesConsumed) {
            return BAENET_MAX_BYTES_EXCEEDED;
        }

    }

    BSLS_ASSERT(0);
}

void skipChars(int            *accumNumBytesConsumed,
               bsl::streambuf *source,
               const char      charsTable[])
{
    BSLS_ASSERT(accumNumBytesConsumed);
    BSLS_ASSERT(source);
    BSLS_ASSERT(charsTable);

    bsl::char_traits<char>::int_type c = source->sgetc();

    while (eof != c && charsTable[c]) {
        c = source->snextc();
        ++*accumNumBytesConsumed;
    }
}

int skipWhitespace(int            *accumNumBytesConsumed,
                   bsl::streambuf *source)
{
    BSLS_ASSERT(accumNumBytesConsumed);
    BSLS_ASSERT(source);

    bsl::char_traits<char>::int_type c = source->sgetc();

    if (eof == c) {
        return BAENET_REACHED_EOF;
    }
    else if (!whitespaceCharsTable[c]) {
        return BAENET_ELEMENT_NOT_FOUND;
    }

    skipChars(accumNumBytesConsumed, source, whitespaceCharsTable);

    return BAENET_SUCCESS;
}

int parseChars(bdeut_StringRef           *result,
               int                       *accumNumBytesConsumed,
               bdema_SequentialAllocator *alloc,
               bsl::streambuf            *source,
               const char                 charsTable[],
               int                        maxNumBytesConsumed)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(accumNumBytesConsumed);
    BSLS_ASSERT(alloc);
    BSLS_ASSERT(source);
    BSLS_ASSERT(charsTable);

    bsl::char_traits<char>::int_type c = source->sgetc();

    if (eof == c) {
        return BAENET_REACHED_EOF;
    }

    if (!charsTable[c]) {
        return BAENET_ELEMENT_NOT_FOUND;
    }

    bsls_PlatformUtil::size_type  size = 1;
    int                           len  = 0;
    char                         *s    = (char *)alloc->
                                                      allocateAndExpand(&size);
    BSLS_ASSERT(s);

    do {
        makeRoom(&s, &size, len, alloc);
        s[len++] = c;

        c = source->snextc();
        ++*accumNumBytesConsumed;
        if ( *accumNumBytesConsumed > maxNumBytesConsumed) {
            return BAENET_MAX_BYTES_EXCEEDED;
        }
    } while (eof != c && charsTable[c]);

    alloc->truncate(s, size, len);
    result->assign(s, len);

    return BAENET_SUCCESS;
}

// ==== begin/end based parsing ======

void skipChars(const char **begin, const char *end, const char charsTable[])
{
    while (*begin != end && charsTable[static_cast<unsigned char>(**begin)]) {
        ++*begin;
    }
}

void skipWhitespace(const char **begin, const char *end)
{
    skipChars(begin, end, whitespaceCharsTable);
}

void skipEnclosedString(const char **begin,
                        const char  *end,
                        char         left,
                        char         right)
{
    BSLS_ASSERT(begin);
    BSLS_ASSERT(*begin);
    BSLS_ASSERT(end);

    if (*begin == end) {
        return;// RET_UNEXPECTED_EOF;
    }

    if (**begin != left) {
        return;// RET_ELEMENT_NOT_FOUND;
    }

    //int  ret = RET_SUCCESS;
    const char *p = *begin;

    ++p;
    for (;;) {
        if (p == end) {           // missing closing char
            //ret = RET_UNEXPECTED_EOF;
            break;
        }
        else if (*p == '\\') {  // found escape char
            ++p;
            if (p == end) {       // missing escaped char
                //ret = RET_UNEXPECTED_EOF;
                break;
                }
        }
        else if (*p == right) {   // found closing char
            ++p;
            break;
        }

        // regular char
        ++p;
    }

    *begin = p;

    //return ret;
}

int parseEnclosedString(bdeut_StringRef            *result,
                        bdema_SequentialAllocator  *alloc,
                        const char                **begin,
                        const char                 *end,
                        char                        left,
                        char                        right)
{
    enum {
        BAENET_SUCCESS        =  0,
        BAENET_UNEXPECTED_EOF = -1,
        BAENET_FAILURE        = -2
    };

    BSLS_ASSERT(result);
    BSLS_ASSERT(alloc);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(*begin);
    BSLS_ASSERT(end);

    if (*begin == end) {
        return BAENET_UNEXPECTED_EOF;
    }

    if (**begin != left) {
        return BAENET_FAILURE;
    }

    int  ret = BAENET_SUCCESS;
    const char *p = *begin;

    bsls_PlatformUtil::size_type  size = 1;
    int                           len  = 0;
    char                         *s    = (char *)alloc->
                                                      allocateAndExpand(&size);
    BSLS_ASSERT(s);

    ++p;
    for (;;) {
        if (p == end) {           // missing closing char
            ret = BAENET_UNEXPECTED_EOF;
            break;
        }
        else if (*p == '\\') {  // found escape char
            ++p;
            if (p == end) {       // missing escaped char
                ret = BAENET_UNEXPECTED_EOF;
                break;
            }
        }
        else if (*p == right) {   // found closing char
            ++p;
            break;
        }

        // regular char
        makeRoom(&s, &size, len, alloc);
        s[len++] = *p++;
    }

    alloc->truncate(s, size, len);
    result->assign(s, len);
    *begin = p;

    return ret;
}

int parseInt(int *object, const char **begin, const char *end)
{
    enum { BAENET_SUCCESS = 0, BAENET_FAILURE = -1 };

    const char *p = *begin;

    if (p == end) {
        return BAENET_FAILURE;
    }

    int sign = ('-' == *p) ? -1 : +1;

    if ('-' == *p || '+' == *p) {
        ++p;  // skip sign
    }

    if (p == end || '0' > *p || *p > '9') {
        return BAENET_FAILURE;
    }

    *object = 0;

    while (p != end && '0' <= *p && *p <= '9') {
        *object *= 10;
        *object += *p - '0';

        ++p;
    }

    *object *= sign;

    *begin = p;

    return BAENET_SUCCESS;
}

void parseAtom(bdeut_StringRef  *result,
               const char      **begin,
               const char       *end)
{
    const char *p = *begin;
    skipChars(&p, end, atomCharsTable);
    result->assign(*begin, p);
    *begin = p;
}

void skipDayOfWeek(const char **begin, const char *end)
{
    const char *p = *begin;

    bdeut_StringRef dayOfWeek;

    parseAtom(&dayOfWeek, &p, end);

    if (!bdeu_String::areEqualCaseless("Mon", 3,
                                       dayOfWeek.begin(),
                                       dayOfWeek.length())
     && !bdeu_String::areEqualCaseless("Tue", 3,
                                       dayOfWeek.begin(),
                                       dayOfWeek.length())
     && !bdeu_String::areEqualCaseless("Wed", 3,
                                       dayOfWeek.begin(),
                                       dayOfWeek.length())
     && !bdeu_String::areEqualCaseless("Thu", 3,
                                       dayOfWeek.begin(),
                                       dayOfWeek.length())
     && !bdeu_String::areEqualCaseless("Fri", 3,
                                       dayOfWeek.begin(),
                                       dayOfWeek.length())
     && !bdeu_String::areEqualCaseless("Sat", 3,
                                       dayOfWeek.begin(),
                                       dayOfWeek.length())
     && !bdeu_String::areEqualCaseless("Sun", 3,
                                       dayOfWeek.begin(),
                                       dayOfWeek.length())) {
        return;
    }

    baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(&p, end);

    if (p != end && ',' == *p) {
        ++p;
        *begin = p;
    }
}

int parseDate(int         *year,
              int         *month,
              int         *day,
              const char **begin,
              const char  *end)
{
    enum { BAENET_SUCCESS = 0, BAENET_FAILURE = -1 };

    const char *p = *begin;

    if (0 != parseInt(day, &p, end)) {
        return BAENET_FAILURE;
    }

    baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(&p, end);

    bdeut_StringRef monthRef;

    parseAtom(&monthRef, &p, end);

    if (bdeu_String::areEqualCaseless("Jan", 3,
                                      monthRef.begin(),
                                      monthRef.length())) {
        *month = 1;
    }
    else if (bdeu_String::areEqualCaseless("Feb", 3,
                                           monthRef.begin(),
                                           monthRef.length())) {
        *month = 2;
    }
    else if (bdeu_String::areEqualCaseless("Mar", 3,
                                           monthRef.begin(),
                                           monthRef.length())) {
        *month = 3;
    }
    else if (bdeu_String::areEqualCaseless("Apr", 3,
                                           monthRef.begin(),
                                           monthRef.length())) {
        *month = 4;
    }
    else if (bdeu_String::areEqualCaseless("May", 3,
                                           monthRef.begin(),
                                           monthRef.length())) {
        *month = 5;
    }
    else if (bdeu_String::areEqualCaseless("Jun", 3,
                                           monthRef.begin(),
                                           monthRef.length())) {
        *month = 6;
    }
    else if (bdeu_String::areEqualCaseless("Jul", 3,
                                           monthRef.begin(),
                                           monthRef.length())) {
        *month = 7;
    }
    else if (bdeu_String::areEqualCaseless("Aug", 3,
                                           monthRef.begin(),
                                           monthRef.length())) {
        *month = 8;
    }
    else if (bdeu_String::areEqualCaseless("Sep", 3,
                                           monthRef.begin(),
                                           monthRef.length())) {
        *month = 9;
    }
    else if (bdeu_String::areEqualCaseless("Oct", 3,
                                           monthRef.begin(),
                                           monthRef.length())) {
        *month = 10;
    }
    else if (bdeu_String::areEqualCaseless("Nov", 3,
                                           monthRef.begin(),
                                           monthRef.length())) {
        *month = 11;
    }
    else if (bdeu_String::areEqualCaseless("Dec", 3,
                                           monthRef.begin(),
                                           monthRef.length())) {
        *month = 12;
    }
    else {
        return BAENET_FAILURE;
    }

    baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(&p, end);

    if (0 != parseInt(year, &p, end)) {
        return BAENET_FAILURE;
    }

    *begin = p;

    return BAENET_SUCCESS;
}

int parseTime(int         *hour,
              int         *minute,
              int         *second,
              int         *hourOffset,
              int         *minuteOffset,
              const char **begin,
              const char  *end)
{
    enum { BAENET_SUCCESS = 0, BAENET_FAILURE = -1 };

    const char *p = *begin;

    if (0 != parseInt(hour, &p, end)) {
        return BAENET_FAILURE;
    }

    baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(&p, end);

    if (p == end || ':' != *p) {
        return BAENET_FAILURE;
    }

    ++p;  // skip ':'

    baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(&p, end);

    if (0 != parseInt(minute, &p, end)) {
        return BAENET_FAILURE;
    }

    baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(&p, end);

    if (p == end) {
        return BAENET_FAILURE;
    }

    if (':' == *p) {
        ++p;  // skip ':'

        baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(&p, end);

        if (0 != parseInt(second, &p, end)) {
            return BAENET_FAILURE;
        }

        baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(&p, end);
    }
    else {
        *second = 0;
    }

    if (p == end) {
        return BAENET_FAILURE;
    }
    else if ('-' == *p || '+' == *p) {
        int sign = ('-' == *p) ? -1 : +1;

        ++p;  // skip sign

        baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(&p, end);

        if (0 != parseInt(hourOffset, &p, p + 2)) {
            return BAENET_FAILURE;
        }

        baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(&p, end);

        if (0 != parseInt(minuteOffset, &p, p + 2)) {
            return BAENET_FAILURE;
        }

        *hourOffset   *= sign;
        *minuteOffset *= sign;
    }
    else {
        bdeut_StringRef timezone;

        parseAtom(&timezone, &p, end);

        if (bdeu_String::areEqualCaseless("UT", 2,
                                          timezone.begin(),
                                          timezone.length())
         || bdeu_String::areEqualCaseless("GMT", 3,
                                          timezone.begin(),
                                          timezone.length())) {
            *hourOffset   = 0;
            *minuteOffset = 0;
        }
        else if (bdeu_String::areEqualCaseless("EST", 3,
                                               timezone.begin(),
                                               timezone.length())) {
            *hourOffset   = -5;
            *minuteOffset = 0;
        }
        else if (bdeu_String::areEqualCaseless("EDT", 3,
                                               timezone.begin(),
                                               timezone.length())) {
            *hourOffset   = -4;
            *minuteOffset = 0;
        }
        else if (bdeu_String::areEqualCaseless("CST", 3,
                                               timezone.begin(),
                                               timezone.length())) {
            *hourOffset   = -5;
            *minuteOffset = 0;
        }
        else if (bdeu_String::areEqualCaseless("CDT", 3,
                                               timezone.begin(),
                                               timezone.length())) {
            *hourOffset   = -5;
            *minuteOffset = 0;
        }
        else if (bdeu_String::areEqualCaseless("MST", 3,
                                               timezone.begin(),
                                               timezone.length())) {
            *hourOffset   = -7;
            *minuteOffset = 0;
        }
        else if (bdeu_String::areEqualCaseless("MDT", 3,
                                               timezone.begin(),
                                               timezone.length())) {
            *hourOffset   = -6;
            *minuteOffset = 0;
        }
        else if (bdeu_String::areEqualCaseless("PST", 3,
                                               timezone.begin(),
                                               timezone.length())) {
            *hourOffset   = -8;
            *minuteOffset = 0;
        }
        else if (bdeu_String::areEqualCaseless("PDT", 3,
                                               timezone.begin(),
                                               timezone.length())) {
            *hourOffset   = -7;
            *minuteOffset = 0;
        }
        else {
            return BAENET_FAILURE;
        }
    }

    *begin = p;

    return BAENET_SUCCESS;
}

int parseContentParameter(bdeut_StringRef            *name,
                          bdeut_StringRef            *value,
                          bdema_SequentialAllocator  *alloc,
                          const char                **begin,
                          const char                 *end)
{
    enum {
        BAENET_SUCCESS        =  0,
        BAENET_UNEXPECTED_EOF = -1,
        BAENET_FAILURE        = -2
    };

    BSLS_ASSERT(name);
    BSLS_ASSERT(value);
    BSLS_ASSERT(alloc);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(*begin);
    BSLS_ASSERT(end);
    BSLS_ASSERT(*begin <= end);

    if (*begin == end) {
        return BAENET_UNEXPECTED_EOF;
    }

    bdeut_StringRef nameTemp;
    parseAtom(&nameTemp, begin, end);

    baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(begin, end);

    if (*begin == end) {
        return BAENET_UNEXPECTED_EOF;
    }
    else if ('=' != **begin) {
        return BAENET_FAILURE;
    }

    ++(*begin);  // skip '='

    baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(begin, end);

    bdeut_StringRef valueTemp;

    if (*begin == end) {
        return BAENET_UNEXPECTED_EOF;
    }
    else if ('"' == **begin) {   // value is quoted string
        int retCode = parseEnclosedString(&valueTemp, alloc, begin, end,
                                          '"', '"');

        if (BAENET_SUCCESS != retCode) {
            return retCode;
        }
    }
    else {                  // value is atom
        parseAtom(&valueTemp, begin, end);
    }

    *name  = nameTemp;
    *value = valueTemp;

    return BAENET_SUCCESS;
}

}  // close unnamed namespace

                        // ----------------------------
                        // struct baenet_HttpParserUtil
                        // ----------------------------

// FUNCTIONS

void baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(const char **begin,
                                                            const char  *end)
{
    BSLS_ASSERT(begin);
    BSLS_ASSERT(*begin);
    BSLS_ASSERT(end);
    BSLS_ASSERT(*begin <= end);

    if (*begin == end) {
        return;// BAENET_UNEXPECTED_EOF;
    }

    int commentLevel = 0;
        // Incremented at each '(', decremented at each ')'.

    const char *p   = *begin;
    //int         ret = BAENET_SUCCESS;

    for (;;) {
        if (p == end) {
            if (commentLevel) {   // comment closing char was not found
                //ret = BAENET_UNEXPECTED_EOF;
            }

            break;
        }
        else if ('\r' == *p) {  // found CR
            ++p;

            if (p == end) {         // missing LF
                //ret = BAENET_UNEXPECTED_EOF;

                break;
            }
            else if ('\n' == *p) {  // found LF
                ++p;

                if (p == end || !whitespaceCharsTable[(int)*p]) { // not folded
                                                                  // white
                                                                  // space
                    p -= 2;  // go back to CRLF

                    break;
                }
            }
        }
        else {
            if ('(' == *p) {          // found comment
                ++commentLevel;
            }
            else if (commentLevel) {  // we are inside a comment
                if (')' == *p) {          // found comment closing char
                    --commentLevel;
                }
                else if ('\\' == *p) {    // found comment escape char
                    ++p;

                    if (p == end) {           // missing escaped char
                        //ret = BAENET_UNEXPECTED_EOF;

                        break;
                    }
                }
            }
            else if (!whitespaceCharsTable[(int)*p] && '\n' != *p) {
                break;
            }

            ++p;
        }
    }

#if 0
    if (p == *begin) {
        return BAENET_ELEMENT_NOT_FOUND;
    }
    else {
        *begin = p;

        return ret;
    }
#endif

    *begin = p;
}

void baenet_HttpParserUtil::skipQuotedString(const char **begin,
                                             const char  *end)
{
    BSLS_ASSERT(begin);
    BSLS_ASSERT(*begin);
    BSLS_ASSERT(end);
    BSLS_ASSERT(*begin <= end);

    skipEnclosedString(begin, end, '"', '"');
}

void baenet_HttpParserUtil::skipDomainLiteral(const char **begin,
                                              const char  *end)
{
    BSLS_ASSERT(begin);
    BSLS_ASSERT(*begin);
    BSLS_ASSERT(end);
    BSLS_ASSERT(*begin <= end);

    skipEnclosedString(begin, end, '[', ']');
}

int baenet_HttpParserUtil::parseLine(
                              bdeut_StringRef           *result,
                              int                       *accumNumBytesConsumed,
                              bdema_SequentialAllocator *alloc,
                              bsl::streambuf            *source,
                              int                        maxNumBytesConsumed)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(accumNumBytesConsumed);
    BSLS_ASSERT(alloc);
    BSLS_ASSERT(source);

    bsls_PlatformUtil::size_type  size = 1;
    int                           len  = 0;
    char                         *s    = (char *)alloc->
                                                      allocateAndExpand(&size);
    BSLS_ASSERT(s);

    int ret = appendLine(&s, &len, &size, accumNumBytesConsumed,
                          alloc, source, maxNumBytesConsumed);

    alloc->truncate(s, size, len);
    result->assign(s, len);

    return ret;
}

int baenet_HttpParserUtil::parseFieldName(
                              bdeut_StringRef           *result,
                              int                       *accumNumBytesConsumed,
                              bdema_SequentialAllocator *alloc,
                              bsl::streambuf            *source,
                              int                        maxNumBytesConsumed)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(accumNumBytesConsumed);
    BSLS_ASSERT(alloc);
    BSLS_ASSERT(source);

    bsl::char_traits<char>::int_type c = source->sgetc();

    if (eof == c) {
        return BAENET_REACHED_EOF;
    }
    else if ('\r' != c && !headerCharsTable[c]) {
        return BAENET_ELEMENT_NOT_FOUND;
    }

    if ('\r' == c) {
        c = source->snextc();
        ++*accumNumBytesConsumed;

        if (eof == c) {
            return BAENET_REACHED_EOF;
        }
        else if ('\n' == c) {
            source->snextc();
            ++*accumNumBytesConsumed;

            return BAENET_END_OF_HEADER;
        }
        else {
            return BAENET_FAILURE;
        }
    }

    int retCode = parseChars(result,
                             accumNumBytesConsumed,
                             alloc,
                             source,
                             headerCharsTable,
                             maxNumBytesConsumed);

    if (BAENET_SUCCESS != retCode) {
        return retCode;
    }

    skipWhitespace(accumNumBytesConsumed, source);

    c = source->sgetc();

    if (eof == c) {
        return BAENET_REACHED_EOF;
    }
    else if (':' != c) {
        return BAENET_ELEMENT_NOT_FOUND;
    }

    source->snextc();  // skip ':'
    ++*accumNumBytesConsumed;

    return BAENET_SUCCESS;
}

int baenet_HttpParserUtil::parseFieldValueUnstructured(
                              bdeut_StringRef           *result,
                              int                       *accumNumBytesConsumed,
                              bdema_SequentialAllocator *alloc,
                              bsl::streambuf            *source,
                              int                        maxNumBytesConsumed)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(accumNumBytesConsumed);
    BSLS_ASSERT(alloc);
    BSLS_ASSERT(source);

    bsl::char_traits<char>::int_type c = source->sgetc();

    if (eof == c) {
        return BAENET_REACHED_EOF;
    }

    int                           ret  = BAENET_SUCCESS;
    bsls_PlatformUtil::size_type  size = 1;
    int                           len  = 0;
    char                         *s    = (char *)alloc->
                                                      allocateAndExpand(&size);
    BSLS_ASSERT(s);

    // Keep looping until we find CRLF ^WSP or CRLF <end>.

    for (;;) {
        ret = appendLine(&s, &len, &size, accumNumBytesConsumed,
                          alloc, source, maxNumBytesConsumed);

        c = source->sgetc();

        if (eof == c
         || !whitespaceCharsTable[c]
         || ret == BAENET_MAX_BYTES_EXCEEDED) {
            break;
        }

        makeRoom(&s, &size, len+1, alloc);
        s[len++] = '\r';
        s[len++] = '\n';
    }

    alloc->truncate(s, size, len);
    result->assign(s, len);

    return ret;
}

int baenet_HttpParserUtil::parseStartLine(baenet_HttpRequestLine *result,
                                          const bdeut_StringRef&  str)
{
    enum {
        BAENET_SUCCESS                =  0,
        BAENET_UNEXPECTED_EOF         = -1,
        BAENET_INVALID_REQUEST_METHOD = -2,
        BAENET_INVALID_URI            = -3
    };

    const char *begin = str.begin();
    const char *end   = str.end();
    const char *p     = begin;

    // get request method

    for (;;) {
        if (p == end) {
            return BAENET_UNEXPECTED_EOF;
        }
        else if (' ' == *p) {
            int methodLength = p - begin;

            if (0 != baenet_HttpRequestMethod::fromString(&result->method(),
                                                          begin,
                                                          methodLength)) {
                return BAENET_INVALID_REQUEST_METHOD;
            }

            break;
        }

        ++p;
    }

    // skip spaces

    skipWhitespace(&p, end);

    // get next space (to get request URI)

    if (p == end) {
        return BAENET_INVALID_URI;
    }

    begin = p;

    while (p != end && !whitespaceCharsTable[(int)*p]) {
        ++p;
    }

    result->requestUri().assign(begin, p);

    // get version (optional)

    skipWhitespace(&p, end);

    bdeut_StringRef httpToken;

    parseAtom(&httpToken, &p, end);

    if (4   != httpToken.length()
     || 'H' != bdeu_CharType::toUpper(httpToken[0])
     || 'T' != bdeu_CharType::toUpper(httpToken[1])
     || 'T' != bdeu_CharType::toUpper(httpToken[2])
     || 'P' != bdeu_CharType::toUpper(httpToken[3])) {
        return BAENET_FAILURE;
    }

    skipWhitespace(&p, end);

    if (p == end || '/' != *p) {
        return BAENET_FAILURE;
    }

    ++p;  // skip '/'

    skipWhitespace(&p, end);

    if (0 != parseInt(&result->majorVersion(), &p, end)) {
        return BAENET_FAILURE;
    }

    skipWhitespace(&p, end);

    if (p == end || '.' != *p) {
        return BAENET_FAILURE;
    }

    ++p;  // skip '.'

    skipWhitespace(&p, end);

    if (0 != parseInt(&result->minorVersion(), &p, end)) {
        return BAENET_FAILURE;
    }

    return BAENET_SUCCESS;
}

int baenet_HttpParserUtil::parseStartLine(baenet_HttpStatusLine  *result,
                                          const bdeut_StringRef&  str)
{
    const char *begin = str.begin();
    const char *end   = str.end();
    const char *p     = begin;

    bdeut_StringRef httpToken;

    parseAtom(&httpToken, &p, end);

    if (4   != httpToken.length()
     || 'H' != bdeu_CharType::toUpper(httpToken[0])
     || 'T' != bdeu_CharType::toUpper(httpToken[1])
     || 'T' != bdeu_CharType::toUpper(httpToken[2])
     || 'P' != bdeu_CharType::toUpper(httpToken[3])) {
        return BAENET_FAILURE;
    }

    skipWhitespace(&p, end);

    if (p == end || '/' != *p) {
        return BAENET_FAILURE;
    }

    ++p;  // skip '/'

    skipWhitespace(&p, end);

    if (0 != parseInt(&result->majorVersion(), &p, end)) {
        return BAENET_FAILURE;
    }

    skipWhitespace(&p, end);

    if (p == end || '.' != *p) {
        return BAENET_FAILURE;
    }

    ++p;  // skip '.'

    skipWhitespace(&p, end);

    if (0 != parseInt(&result->minorVersion(), &p, end)) {
        return BAENET_FAILURE;
    }

    skipWhitespace(&p, end);

    // Support non-standard HTTP status codes.
    int code;
    if (0 != parseInt(&code, &p, end)) {
        return BAENET_FAILURE;
    }

    result->statusCode() = (baenet_HttpStatusCode::Value) code;

    skipWhitespace(&p, end);

    result->reasonPhrase().assign(p, end);

    return BAENET_SUCCESS;
}

int baenet_HttpParserUtil::parseFieldValue(int                    *object,
                                           const bdeut_StringRef&  str)
{
    enum { BAENET_SUCCESS = 0, BAENET_FAILURE = -1 };

    const char *begin = str.begin();
    const char *end   = str.end();

    if (0 != parseInt(object, &begin, end)) {
        return BAENET_FAILURE;
    }

    skipWhitespace(&begin, end);

    if (begin != end) {
        return BAENET_FAILURE;
    }

    return BAENET_SUCCESS;
}

int baenet_HttpParserUtil::parseFieldValue(bsl::string            *result,
                                           const bdeut_StringRef&  str)
{
    enum { BAENET_SUCCESS = 0 };

    result->assign(str.begin(), str.end());

    return BAENET_SUCCESS;
}

int baenet_HttpParserUtil::parseFieldValue(bdet_Datetime          *object,
                                           const bdeut_StringRef&  str)
{
    enum { BAENET_SUCCESS = 0, BAENET_FAILURE = -1 };

    const char *begin = str.begin();
    const char *end   = str.end();
    const char *p     = begin;

    skipDayOfWeek(&p, end);
    baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(&p, end);

    int year, month, day;

    if (0 != parseDate(&year, &month, &day, &p, end)) {
        return BAENET_FAILURE;
    }

    baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(&p, end);

    int hour, minute, second, hourOffset, minuteOffset;

    if (0 != parseTime(&hour,
                       &minute,
                       &second,
                       &hourOffset,
                       &minuteOffset,
                       &p,
                       end)
     || !bdet_Datetime::isValid(year, month, day,
                                hour, minute, second)) {
        return BAENET_FAILURE;
    }

    bdet_DatetimeInterval timezoneOffset;

    timezoneOffset.addHours(hourOffset);
    timezoneOffset.addMinutes(minuteOffset);

    *object = bdet_Datetime(year, month, day,
                            hour, minute, second);

    *object -= timezoneOffset;  // convert to GMT

    return BAENET_SUCCESS;
}

int baenet_HttpParserUtil::parseFieldValue(bdet_DatetimeTz        *object,
                                           const bdeut_StringRef&  str)
{
    enum { BAENET_SUCCESS = 0, BAENET_FAILURE = -1 };

    const char *begin = str.begin();
    const char *end   = str.end();
    const char *p     = begin;

    skipDayOfWeek(&p, end);
    baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(&p, end);

    int year, month, day;

    if (0 != parseDate(&year, &month, &day, &p, end)) {
        return BAENET_FAILURE;
    }

    baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(&p, end);

    int hour, minute, second, hourOffset, minuteOffset;

    if (0 != parseTime(&hour,
                       &minute,
                       &second,
                       &hourOffset,
                       &minuteOffset,
                       &p,
                       end)
     || !bdet_Datetime::isValid(year, month, day,
                                hour, minute, second)) {
        return BAENET_FAILURE;
    }

    int timezoneOffset = (hourOffset * 60) + minuteOffset;

    bdet_Datetime localDatetime(year, month, day,
                                hour, minute, second);

    object->setDatetimeTz(localDatetime, timezoneOffset);

    return BAENET_SUCCESS;
}

int baenet_HttpParserUtil::parseFieldValue(baenet_HttpContentType *result,
                                           const bdeut_StringRef&  str)
{
    enum {
        BAENET_SUCCESS        =  0,
        BAENET_UNEXPECTED_EOF = -1,
        BAENET_FAILURE        = -2
    };

    BSLS_ASSERT(result);

    const char *begin = str.begin();
    const char *end   = str.end();

    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);
    BSLS_ASSERT(begin <= end);

    if (begin == end) {
        return BAENET_UNEXPECTED_EOF;
    }

    skipCommentsAndFoldedWhitespace(&begin, end);

    bdeut_StringRef type;
    parseAtom(&type, &begin, end);

    skipCommentsAndFoldedWhitespace(&begin, end);

    if (begin == end) {
        return BAENET_UNEXPECTED_EOF;
    }
    else if ('/' != *begin) {
        return BAENET_FAILURE;
    }

    ++begin;  // skip '/'

    skipCommentsAndFoldedWhitespace(&begin, end);

    bdeut_StringRef subType;
    parseAtom(&subType, &begin, end);

    skipCommentsAndFoldedWhitespace(&begin, end);

    result->type()    = type;
    result->subType() = subType;

    bdema_SequentialAllocator alloc;

    for (;;) {
        if (begin == end) {
            break;
        }
        else if (';' == *begin) {
            ++begin;

            skipCommentsAndFoldedWhitespace(&begin, end);

            bdeut_StringRef name, value;

            int retCode = parseContentParameter(&name,
                                                &value,
                                                &alloc,
                                                &begin,
                                                end);

            if (BAENET_SUCCESS == retCode) {
                skipCommentsAndFoldedWhitespace(&begin, end);

                const char CHARSET[]  = "CHARSET";
                const char BOUNDARY[] = "BOUNDARY";
                const char ID[]       = "ID";
                const char NAME[]     = "NAME";

                const int CHARSET_LEN  = sizeof(CHARSET)  - 1;
                const int BOUNDARY_LEN = sizeof(BOUNDARY) - 1;
                const int ID_LEN       = sizeof(ID)       - 1;
                const int NAME_LEN     = sizeof(NAME)     - 1;

                const char *nameBegin = name.begin();
                const int   nameLen   = name.length();

                if (bdeu_String::areEqualCaseless(CHARSET,
                                                  CHARSET_LEN,
                                                  nameBegin,
                                                  nameLen)) {
                    result->charset() = value;
                }
                else if (bdeu_String::areEqualCaseless(BOUNDARY,
                                                       BOUNDARY_LEN,
                                                       nameBegin,
                                                       nameLen)) {
                    result->boundary() = value;
                }
                else if (bdeu_String::areEqualCaseless(ID,
                                                       ID_LEN,
                                                       nameBegin,
                                                       nameLen)) {
                    result->id() = value;
                }
                else if (bdeu_String::areEqualCaseless(NAME,
                                                       NAME_LEN,
                                                       nameBegin,
                                                       nameLen)) {
                    result->name() = value;
                }
            }
            else {
                return BAENET_FAILURE;
#if 0
                // Failed to parse this parameter.  Skip it and move on to the
                // next parameter.  Implementation note: we want to skip to the
                // first CRLF ^WSP or ';' (whichever comes first).
                // 'contentParameterDelimiters' consists of CR and ';'.

                do {
                    ParserUtil::skipDelimitedString(&begin,
                                                    end,
                                                    contentParameterDelimiters,
                                                    0);
                } while (BAENET_SUCCESS == skipCommentsAndFoldedWhitespace(
                                                                        &begin,
                                                                        end));
#endif
            }
        }
        else {
            break;
        }
    }

    return BAENET_SUCCESS;
}

int baenet_HttpParserUtil::parseFieldValue(baenet_HttpViaRecord   *,
                                           const bdeut_StringRef&)
{
    enum { BAENET_NOT_IMPLEMENTED = 0 };
    return BAENET_NOT_IMPLEMENTED;
}

int baenet_HttpParserUtil::parseFieldValue(baenet_HttpHost        *result,
                                           const bdeut_StringRef&  str)
{
    enum { BAENET_SUCCESS = 0, BAENET_FAILURE = -1 };

    result->reset();

    const char *begin = str.begin();
    const char *end   = str.end();
    const char *p     = begin;

    while (p != end && !whitespaceCharsTable[(int)*p] && ':' != *p) {
        ++p;
    }

    result->name().assign(begin, p);

    skipWhitespace(&p, end);

    if (p == end) {
        return BAENET_SUCCESS;
    }

    if (':' != *p) {
        return BAENET_FAILURE;
    }

    ++p;  // skip ':'

    skipWhitespace(&p, end);

    int port;

    if (0 != parseInt(&port, &p, end)) {
        return BAENET_FAILURE;
    }

    result->port().makeValue(port);

    return BAENET_SUCCESS;
}

int baenet_HttpParserUtil::parseChunkHeader(int            *result,
                                            int            *numBytesConsumed,
                                            bsl::streambuf *buffer)
{
    enum { BAENET_SUCCESS = 0, BAENET_FAILURE = -1 };
    enum { CR = '\r', LF = '\n' };
    BSLS_ASSERT(result);
    BSLS_ASSERT(numBytesConsumed);
    BSLS_ASSERT(buffer);

    bsl::char_traits<char>::int_type c = buffer->sgetc();
    ++*numBytesConsumed;

    // ignore leading CRLF

    if (CR == c) {
        c = buffer->snextc();
        ++*numBytesConsumed;

        if (LF != c) {
            return eof == c ? (int)BAENET_REACHED_EOF : (int)BAENET_FAILURE;
        }
        c = buffer->snextc();
        ++*numBytesConsumed;
    }

    if (eof != c && NOT_HEX == hexCharTable[c]) {
        return BAENET_FAILURE;
    }
    int value = 0, bytesRead = 0;

    while (eof != c && NOT_HEX != hexCharTable[c]) {
        value <<= 4;
        value += hexCharTable[c];
        c = buffer->snextc();
        ++bytesRead;
    }
    *numBytesConsumed += bytesRead;

    if (2*sizeof(*result) < (unsigned) bytesRead) {
        return BAENET_FAILURE; // overflow
    }

    // Skip chunk extensions since baenet_MessageParser has no mechanism to
    // communicate them to the application, and the standard REQUIRES
    // implementations to ignore them if they are not going to be supported.

    while (eof != c && CR != c) {
        c = buffer->snextc();
        ++*numBytesConsumed;
    }

    if (eof == c) {
        return BAENET_REACHED_EOF;
    }
    c = buffer->snextc();
    ++*numBytesConsumed;

    if (eof == c) {
        return BAENET_REACHED_EOF;
    }

    if (LF != c) {
        return BAENET_FAILURE;
    }
    *result = value;
    return BAENET_SUCCESS;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
