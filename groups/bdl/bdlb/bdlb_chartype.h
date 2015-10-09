// bdlb_chartype.h                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLB_CHARTYPE
#define INCLUDED_BDLB_CHARTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Supply local-independent version of '<ctype.h>' functionality.
//
//@CLASSES:
//   bdlb::CharType: namespace for pure (read-only) procedures on characters
//
//@SEE_ALSO: bdlb_string
//
//@DESCRIPTION: This component provides an efficient, local-independent
// alternative for the standard functionality found in '<ctype.h>'.  The
// following character categories are supported (note that, 'IDENT', 'ALUND',
// 'ALL', and 'NONE' are new):
//..
//    ============================================================
//                          Category   Description
//    --------   -------------------------------------------------
//    UPPER      [A-Z]
//    LOWER      [a-z]
//    ALPHA      [A-Za-z]
//    DIGIT      [0-9]
//    XDIGIT     [0-9A-Fa-f]
//    ALNUM      [0-9A-Za-z]
//    SPACE      [space|tab|CR|NL|VT|FF]
//    PRINT      any printable character including SPACE
//    GRAPH      any printable character except SPACE
//    PUNCT      any printable character except SPACE or ALNUM
//    CNTRL      [\0-\37] and \177 (in standard ASCII, see below)
//    ASCII      [\0-\177]
//    IDENT      [ALNUM|_]
//    ALUND      [ALPHA|_]
//    ALL        any 8-bit value
//    NONE       []
//    ============================================================
//..
// Supported functionality includes determining whether a character is
// a member of a given 'bdlb::CharType' and also providing a null-terminated,
// contiguous sequence (and character count) for each character category.
// Additionally, the standard conversion methods 'toUpper' and 'toLower'
// are also provided.
//
// Note that this component assumes the ASCII character set *with* *standard*
// *encodings*, which is sufficient for all currently supported platforms.
//
///ASCII Character Set
///-------------------
// The following table provides a reference for the ASCII character set:
//..
//      Decimal      Hexadecimal    Key        Meaning
//      -------      -----------    ---        -------
//      0            0x00           ^@         NULL
//      1            0x01           ^A         Start Heading
//      2            0x02           ^B         Start Text
//      3            0x03           ^C         End Text
//      4            0x04           ^D         End of transmission
//      5            0x05           ^E         Enquiry
//      6            0x06           ^F         Acknowledge
//      7            0x07           ^G         Bell
//      8            0x08           ^H         Backspace
//      9            0x09           ^I         Horizontal Tab
//      10           0x0A           ^J         Newline (Linefeed)
//      11           0x0B           ^K         Vertical Tab
//      12           0x0C           ^L         Form Feed
//      13           0x0D           ^M         Carriage Return
//      14           0x0E           ^N         Shift Out
//      15           0x0F           ^O         Shift In
//      16           0x10           ^P         Data Link Escape
//      17           0x11           ^Q         Device Control 1
//      18           0x12           ^R         Device Control 2
//      19           0x13           ^S         Device Control 3
//      20           0x14           ^T         Device Control 4
//      21           0x15           ^U         Negative Acknowledgement
//      22           0x16           ^V         Synchronous Idle
//      23           0x17           ^W         End of transmission Block
//      24           0x18           ^X         Cancel
//      25           0x19            ^Y        End of Medium
//      26           0x1A           ^Z         Substitute
//      27           0x1B           ^[         Escape
//      28           0x1C           ^\         File Separator
//      29           0x1D           ^]         Group Separator
//      30           0x1E           ^^         Record Separator
//      31           0x1F           ^_         Unit Separator
//      32           0x20           (space)
//      33           0x21           !
//      34           0x22           "
//      35           0x23           #
//      36           0x24           $
//      37           0x25           %
//      38           0x26           &
//      39           0x27           '
//      40           0x28           (
//      41           0x29           )
//      42           0x2A           *
//      43           0x2B           +
//      44           0x2C           ,
//      45           0x2D           -
//      46           0x2E           .
//      47           0x2F           /
//      48-57        0x30-0x39      0-9
//      58           0x3A           :
//      59           0x3B           ;
//      60           0x3C           <
//      61           0x3D           =
//      62           0x3E           >
//      63           0x3F           ?
//      64           0x40           @
//      65-90        0x41-0x5A      A-Z
//      91           0x5B           [
//      92           0x5C           \          backslash
//      93           0x5D           ]
//      94           0x5E           ^
//      95           0x5F           _
//      96           0x60           `
//      97-122       0x61-0x7A      a-z
//      123          0x7B           {
//      124          0x7C           |
//      125          0x7D           }
//      126          0x7E           ~
//      127          0x75          ^?          Delete (Rubout)
//..
//
///Category Definitions
///--------------------
// The following table defines the members of each category:
//..
//                 UPPER
//                 :  LOWER
//                 :  :  ALPHA
//                 :  :  :  DIGIT
//                 :  :  :  :  XDIGIT
//                 :  :  :  :  :  ALNUM
//                 :  :  :  :  :  :  SPACE
//                 :  :  :  :  :  :  :  PRINT
//                 :  :  :  :  :  :  :  :  GRAPH
//                 :  :  :  :  :  :  :  :  :  PUNCT
//                 :  :  :  :  :  :  :  :  :  :  CNTRL
//                 :  :  :  :  :  :  :  :  :  :  :  ASCII
//                 :  :  :  :  :  :  :  :  :  :  :  :  IDENT
//                 :  :  :  :  :  :  :  :  :  :  :  :  :  ALUND
//                 :  :  :  :  :  :  :  :  :  :  :  :  :  :  ALL
//                 :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  NONE
//    Dec   Hex    :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :       Char
//    ---   ---    -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -       ----
//      0     0    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^@
//      1     1    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^A
//      2     2    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^B
//      3     3    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^C
//      4     4    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^D
//      5     5    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^E
//      6     6    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^F
//      7     7    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^G
//      8     8    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^H
//      9     9    _  _  _  _  _  _  S  _  _  _  C  A  _  _  A  _        ^I
//     10     A    _  _  _  _  _  _  S  _  _  _  C  A  _  _  A  _        ^J
//     11     B    _  _  _  _  _  _  S  _  _  _  C  A  _  _  A  _        ^K
//     12     C    _  _  _  _  _  _  S  _  _  _  C  A  _  _  A  _        ^L
//     13     D    _  _  _  _  _  _  S  _  _  _  C  A  _  _  A  _        ^M
//     14     E    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^N
//     15     F    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^O
//     16    10    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^P
//     17    11    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^Q
//     18    12    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^R
//     19    13    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^S
//     20    14    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^T
//     21    15    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^U
//     22    16    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^V
//     23    17    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^W
//     24    18    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^X
//     25    19    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^Y
//     26    1A    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^Z
//     27    1B    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^[
//     28    1C    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^/
//     29    1D    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^]
//     30    1E    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^^
//     31    1F    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^_
//     32    20    _  _  _  _  _  _  S  P  _  _  _  A  _  _  A  _
//     33    21    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        !
//     34    22    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        "
//     35    23    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        #
//     36    24    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        $
//     37    25    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        %
//     38    26    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        &
//     39    27    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        '
//     40    28    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        (
//     41    29    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        )
//     42    2A    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        *
//     43    2B    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        +
//     44    2C    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        ,
//     45    2D    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        -
//     46    2E    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        .
//     47    2F    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        /
//     48    30    _  _  _  D  X  A  _  P  G  _  _  A  I  _  A  _        0
//     49    31    _  _  _  D  X  A  _  P  G  _  _  A  I  _  A  _        1
//     50    32    _  _  _  D  X  A  _  P  G  _  _  A  I  _  A  _        2
//     51    33    _  _  _  D  X  A  _  P  G  _  _  A  I  _  A  _        3
//     52    34    _  _  _  D  X  A  _  P  G  _  _  A  I  _  A  _        4
//     53    35    _  _  _  D  X  A  _  P  G  _  _  A  I  _  A  _        5
//     54    36    _  _  _  D  X  A  _  P  G  _  _  A  I  _  A  _        6
//     55    37    _  _  _  D  X  A  _  P  G  _  _  A  I  _  A  _        7
//     56    38    _  _  _  D  X  A  _  P  G  _  _  A  I  _  A  _        8
//     57    39    _  _  _  D  X  A  _  P  G  _  _  A  I  _  A  _        9
//     58    3A    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        :
//     59    3B    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        ;
//     60    3C    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        <
//     61    3D    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        =
//     62    3E    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        >
//     63    3F    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        ?
//     64    40    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        @
//     65    41    U  _  A  _  X  A  _  P  G  _  _  A  I  A  A  _        A
//     66    42    U  _  A  _  X  A  _  P  G  _  _  A  I  A  A  _        B
//     67    43    U  _  A  _  X  A  _  P  G  _  _  A  I  A  A  _        C
//     68    44    U  _  A  _  X  A  _  P  G  _  _  A  I  A  A  _        D
//     69    45    U  _  A  _  X  A  _  P  G  _  _  A  I  A  A  _        E
//     70    46    U  _  A  _  X  A  _  P  G  _  _  A  I  A  A  _        F
//     71    47    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        G
//     72    48    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        H
//     73    49    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        I
//     74    4A    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        J
//     75    4B    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        K
//     76    4C    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        L
//     77    4D    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        M
//     78    4E    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        N
//     79    4F    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        O
//     80    50    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        P
//     81    51    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        Q
//     82    52    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        R
//     83    53    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        S
//     84    54    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        T
//     85    55    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        U
//     86    56    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        V
//     87    57    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        W
//     88    58    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        X
//     89    59    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        Y
//     90    5A    U  _  A  _  _  A  _  P  G  _  _  A  I  A  A  _        Z
//     91    5B    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        [
//     92    5C    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _       '\'
//     93    5D    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        ]
//     94    5E    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        ^
//     95    5F    _  _  _  _  _  _  _  P  G  P  _  A  I  A  A  _        _
//     96    60    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        `
//     97    61    _  L  A  _  X  A  _  P  G  _  _  A  I  A  A  _        a
//     98    62    _  L  A  _  X  A  _  P  G  _  _  A  I  A  A  _        b
//     99    63    _  L  A  _  X  A  _  P  G  _  _  A  I  A  A  _        c
//    100    64    _  L  A  _  X  A  _  P  G  _  _  A  I  A  A  _        d
//    101    65    _  L  A  _  X  A  _  P  G  _  _  A  I  A  A  _        e
//    102    66    _  L  A  _  X  A  _  P  G  _  _  A  I  A  A  _        f
//    103    67    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        g
//    104    68    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        h
//    105    69    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        i
//    106    6A    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        j
//    107    6B    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        k
//    108    6C    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        l
//    109    6D    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        m
//    110    6E    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        n
//    111    6F    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        o
//    112    70    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        p
//    113    71    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        q
//    114    72    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        r
//    115    73    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        s
//    116    74    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        t
//    117    75    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        u
//    118    76    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        v
//    119    77    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        w
//    120    78    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        x
//    121    79    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        y
//    122    7A    _  L  A  _  _  A  _  P  G  _  _  A  I  A  A  _        z
//    123    7B    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        {
//    124    7C    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        |
//    125    7D    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        }
//    126    7E    _  _  _  _  _  _  _  P  G  P  _  A  _  _  A  _        ~
//    127    7F    _  _  _  _  _  _  _  _  _  _  C  A  _  _  A  _        ^?
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Validating C-Style Identifiers
///- - - - - - - - - - - - - - - - - - - - -
// The character category extensions 'IDENT' and 'ALUND' are particularly
// useful for parsing C-style identifier names as described by the following
// regular expression:
//..
//  [A-Za-z_]([A-Za-z0-9_])*
//..
// The first character is required and must be in category 'ALUND'.  All
// subsequent characters are optional and must be in category 'IDENT':
//..
//  bool isIdentifier(const char *token)
//      // Return 'true' if the specified 'token' conforms to the requirements
//      // of a C-style identifier, and 'false' otherwise.
//  {
//      assert(token);
//
//      if (!bdlb::CharType::isAlund(*token)) {
//          return false; // bad required first character             // RETURN
//      }
//
//      for (const char *p = token + 1; *p; ++p) {
//          if (!bdlb::CharType::isIdent(*p)) {
//              return false; // bad optional subsequent character    // RETURN
//
//      return true;
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace bdlb {
struct CharType {
    // This 'struct' provides a namespace for a suite of pure procedures
    // operating on the fundamental type 'char'.

    // TYPES
    enum Category {
        // Set of character categories supported by this component.

        e_UPPER,          // [A-Z]
        e_LOWER,          // [a-z]
        e_ALPHA,          // [A-Za-z]
        e_DIGIT,          // [0-9]
        e_XDIGIT,         // [0-9A-Fa-f]
        e_ALNUM,          // [0-9A-Za-z]
        e_SPACE,          // [space|tab|CR|NL|VT|FF]
        e_PRINT,          // any printable character including SPACE
        e_GRAPH,          // any printable character except SPACE
        e_PUNCT,          // any printable character except SPACE or ALNUM
        e_CNTRL,          // [\1-\37] and \177
        e_ASCII,          // [\0-\177]
        e_IDENT,          // [ALNUM|_]
        e_ALUND,          // [ALPHA|_]
        e_ALL,            // [\0-\377]
        e_NONE            // []

    };

    enum {
        k_NUM_CATEGORIES = e_NONE + 1
    };
        // Current number of categories supported by this component.

  private:
    // CLASS DATA
    static const char *const s_upperString_p;      // 'char' array indexed by
    static const char *const s_lowerString_p;      // '[char]'
    static const char *const s_alphaString_p;
    static const char *const s_digitString_p;
    static const char *const s_xdigitString_p;
    static const char *const s_alnumString_p;
    static const char *const s_spaceString_p;
    static const char *const s_printString_p;
    static const char *const s_graphString_p;
    static const char *const s_punctString_p;
    static const char *const s_cntrlString_p;
    static const char *const s_asciiString_p;
    static const char *const s_identString_p;
    static const char *const s_alundString_p;
    static const char *const s_allString_p;
    static const char *const s_noneString_p;

    static const char *const *const s_categoryString_p;
                                                 // indexed by
                                                 // '[category, char]'

    static const char *const *const s_categoryName_p;
                                                 // 'enum' to string map
                                                 // indexed by '[category]'

    static const short int s_upperCount;         // 'int' category counts
    static const short int s_lowerCount;
    static const short int s_alphaCount;
    static const short int s_digitCount;
    static const short int s_xdigitCount;
    static const short int s_alnumCount;
    static const short int s_spaceCount;
    static const short int s_printCount;
    static const short int s_graphCount;
    static const short int s_punctCount;
    static const short int s_cntrlCount;
    static const short int s_asciiCount;
    static const short int s_identCount;
    static const short int s_alundCount;
    static const short int s_allCount;
    static const short int s_noneCount;

    static const short int *s_categoryCount_p;   // counts indexed by
                                                 // '[category]'

    static const bool *const s_upperArray_p;     // 'bool' arrays indexed by
    static const bool *const s_lowerArray_p;     // '[char]'
    static const bool *const s_alphaArray_p;
    static const bool *const s_digitArray_p;
    static const bool *const s_xdigitArray_p;
    static const bool *const s_alnumArray_p;
    static const bool *const s_spaceArray_p;
    static const bool *const s_printArray_p;
    static const bool *const s_graphArray_p;
    static const bool *const s_punctArray_p;
    static const bool *const s_cntrlArray_p;
    static const bool *const s_asciiArray_p;
    static const bool *const s_identArray_p;
    static const bool *const s_alundArray_p;
    static const bool *const s_allArray_p;
    static const bool *const s_noneArray_p;

    static const bool **s_categoryArray_p;       // indexed by
                                                 // '[category, char]'

    static const char *const s_toUpper_p;        // 'char' arrays index by
    static const char *const s_toLower_p;        // '[char]'

  public:
    // CLASS METHODS

                        // *** boolean tests ***

    static bool isUpper(char character);
        // Return 'true' if the specified 'character' is in the 'UPPER'
        // category (i.e., '[A-Z]'), and 'false' otherwise.

    static bool isLower(char character);
        // Return 'true' if the specified 'character' is in the 'LOWER'
        // category (i.e., '[a-z]'), and 'false' otherwise.

    static bool isAlpha(char character);
        // Return 'true' if the specified 'character' is in the 'ALPHA'
        // category (i.e., '[A-Za-z]'), and 'false' otherwise.

    static bool isDigit(char character);
        // Return 'true' if the specified 'character' is in the 'DIGIT'
        // category (i.e., '[0-9]'), and 'false' otherwise.

    static bool isXdigit(char character);
        // Return 'true' if the specified 'character' is in the 'XDIGIT'
        // category (i.e., '[0-9A-Fa-f]'), and 'false' otherwise.

    static bool isAlnum(char character);
        // Return 'true' if the specified 'character' is in the 'ALNUM'
        // category (i.e., '[0-9A-Za-Z]'), and 'false' otherwise.

    static bool isSpace(char character);
        // Return 'true' if the specified 'character' is in the 'SPACE'
        // category (i.e., '[space|tab|CR|NL|VT|FF]'), and 'false' otherwise.

    static bool isPrint(char character);
        // Return 'true' if the specified 'character' is in the 'PRINT'
        // category (i.e., any printable character including 'SPACE'),
        // and 'false' otherwise.

    static bool isGraph(char character);
        // Return 'true' if the specified 'character' is in the 'GRAPH'
        // category (i.e., any printable character except 'SPACE'), and
        // 'false' otherwise.

    static bool isPunct(char character);
        // Return 'true' if the specified 'character' is in the 'PUNCT'
        // category (i.e., any printable character other than 'SPACE'
        // or 'ALNUM'), and 'false' otherwise.

    static bool isCntrl(char character);
        // Return 'true' if the specified 'character' is in the 'CNTRL'
        // category (i.e., '[\1-\37]' and '\177'), and 'false' otherwise.

    static bool isAscii(char character);
        // Return 'true' if the specified 'character' is in the 'ASCII'
        // category (i.e., '[\0-\177]'), and 'false' otherwise.

    static bool isIdent(char character);
        // Return 'true' if the specified 'character' is in the 'IDENT'
        // category (i.e., '[ALNUM|_]'), and 'false' otherwise.

    static bool isAlund(char character);
        // Return 'true' if the specified 'character' is in the 'ALUND'
        // category (i.e., '[ALPHA|_]'), and 'false' otherwise.

    static bool isAll(char character);
        // Return 'true' if the specified 'character' is in the 'ALL'
        // category (i.e., '[\0-\377]'), and 'false' otherwise.  Note that
        // this function always returns 'true', but is provide for
        // completeness.

    static bool isNone(char character);
        // Return 'true' if the specified 'character' is in the 'NONE'
        // category (i.e., '[]'), and 'false' otherwise.  Note that this
        // function always returns 'false', but is provided for completeness.

    static bool isCategory(char character, CharType::Category category);
        // Return 'true' if the specified 'character' is in the specified
        // 'category', and 'false' otherwise.

                        // *** character sets ***

    static const char *stringUpper();
        // Return a null-terminated string consisting of all characters in the
        // category 'UPPER' (i.e., '[A-Z]'), ordered by increasing character
        // codes.

    static const char *stringLower();
        // Return a null-terminated string consisting of all characters in the
        // category 'LOWER' (i.e., '[a-z]'), ordered by increasing character
        // codes.

    static const char *stringAlpha();
        // Return a null-terminated string consisting of all characters in the
        // category 'ALPHA' (i.e., '[A-Za-z]'), ordered by increasing character
        // codes.

    static const char *stringDigit();
        // Return a null-terminated string consisting of all characters in the
        // category 'DIGIT' (i.e., '[0-9]'), ordered by increasing character
        // codes.

    static const char *stringXdigit();
        // Return a null-terminated string consisting of all characters in the
        // category 'XDIGIT' (i.e., '[0-9A-Fa-f]'), ordered by increasing
        // character codes.

    static const char *stringAlnum();
        // Return a null-terminated string consisting of all characters in the
        // category 'ALNUM' (i.e., '[0-9A-Za-Z]'), ordered by increasing
        // character codes.

    static const char *stringSpace();
        // Return a null-terminated string consisting of all characters in the
        // category 'SPACE' (i.e., '[space|tab|CR|NL|VT|FF]'), ordered by
        // increasing character codes.

    static const char *stringPrint();
        // Return a null-terminated string consisting of all characters in the
        // category 'PRINT' (i.e., any printable character including 'SPACE'),
        // ordered by increasing character codes.

    static const char *stringGraph();
        // Return a null-terminated string consisting of all characters in the
        // category 'GRAPH' (i.e., any printable character except 'SPACE'),
        // ordered by increasing character codes.

    static const char *stringPunct();
        // Return a null-terminated string consisting of all characters in the
        // category 'PUNCT' (i.e., any printable character other than 'SPACE'
        // or 'ALNUM'), ordered by increasing character codes.

    static const char *stringCntrl();
        // Return a null-terminated string consisting of all characters in the
        // category 'CNTRL' (i.e., '[\0-\37]' and '\177'), ordered by increasing
        // character codes.  Note that this string, if printed, may appear to
        // be of length 0 because the 'NULL' character is part of the set and
        // appears first.

    static const char *stringAscii();
        // Return a null-terminated string consisting of all characters in the
        // category 'ASCII' (i.e., '[\0-\177]'), ordered by increasing character
        // codes.  Note that this string, if printed, may appear to be of
        // length 0 because the 'NULL' character is part of the set and appears
        // first.

    static const char *stringIdent();
        // Return a null-terminated string consisting of all characters in the
        // category 'IDENT' (i.e., '[ALNUM|_]'), ordered by increasing character
        // codes.

    static const char *stringAlund();
        // Return a null-terminated string consisting of all characters in the
        // category 'ALUND' (i.e., '[ALPHA|_]'), ordered by increasing character
        // codes.

    static const char *stringAll();
        // Return a null-terminated string consisting of all characters in the
        // category 'ALL' (i.e., '[\0-\377]'), ordered by increasing character
        // codes.  Note that this string, if printed, may appear to be of
        // length 0 because the 'NULL' character is part of the set and appears
        // first.

    static const char *stringNone();
        // Return a null-terminated string consisting of all characters in
        // the category 'NONE'.  Note that this string is empty.

    static const char *stringCategory(CharType::Category category);
        // Return a null-terminated string consisting of all characters in
        // the specified 'category', ordered by increasing character codes.
        // Note that for category values of 'CNTRL', 'ASCII, and 'ALL',
        // this string, if printed, may appear to be of length 0 because the
        // 'NULL' character is part of the set and appears first.

                        // *** character counts ***

    static int numUpper();
        // Return the number of characters in the category 'UPPER' (i.e.,
        // '[A-Z]').

    static int numLower();
        // Return the number of characters in the category 'LOWER' (i.e.,
        // '[a-z]').

    static int numAlpha();
        // Return the number of characters in the category 'ALPHA' (i.e.,
        // '[A-Za-z]').

    static int numDigit();
        // Return the number of characters in the category 'DIGIT' (i.e.,
        // '[0-9]').

    static int numXdigit();
        // Return the number of characters in the category 'XDIGIT' (i.e.,
        // '[0-9A-Fa-f]').

    static int numAlnum();
        // Return the number of characters in the category 'ALNUM' (i.e.,
        // '[0-9A-Za-Z]').

    static int numSpace();
        // Return the number of characters in the category 'SPACE' (i.e.,
        // '[space|tab|CR|NL|VT|FF]').

    static int numPrint();
        // Return the number of characters in the category 'PRINT' (i.e.,
        // any printable character including 'SPACE').

    static int numGraph();
        // Return the number of characters in the category 'GRAPH (i.e.,
        // any printable character except 'SPACE'), and 'false' otherwise.

    static int numPunct();
        // Return the number of characters in the category 'PUNCT' (i.e.,
        // any printable character other than 'SPACE' or 'ALNUM').

    static int numCntrl();
        // Return the number of characters in the category 'CNTRL' (i.e.,
        // '[\1-\37]' and '\177').

    static int numAscii();
        // Return the number of characters in the category 'ASCII' (i.e.,
        // '[\0-\177]').

    static int numIdent();
        // Return the number of characters in the category 'IDENT' (i.e.,
        // '[ALNUM|_]').

    static int numAlund();
        // Return the number of characters in the category 'ALUND' (i.e.,
        // '[ALPHA|_]').

    static int numAll();
        // Return the number of characters in the category 'ALL'.  Note that
        // the character string returned by the 'stringAll' method is of
        // length 255, but the terminating null is part of the set.

    static int numNone();
        // Return the number of characters in the category 'NONE'.  Note that
        // the string returned by the 'stringNone' method is empty.

    static int numCategory(CharType::Category category);
        // Return the number of characters in the specified 'category'.

                        // *** miscellaneous ***

    static const char *toAscii(CharType::Category category);
        // Return a null-terminated string representation of the specified
        // character 'category' enumerator that is identical to the enumerator
        // name.

    static char toLower(char input);
        // Return the character in the 'LOWER' category corresponding to the
        // specified 'input' character from the 'UPPER' category or 'input'
        // itself if 'input' is not in category 'UPPER'.

    static char toUpper(char input);
        // Return the character in the 'UPPER' category corresponding to the
        // specified 'input' character from the 'LOWER' category or 'input'
        // itself if 'input' is not in category 'LOWER'.
};

bsl::ostream& operator<<(bsl::ostream& out, CharType::Category category);
    // Write the specified character 'category' enumerator to the specified
    // output stream as a string that is identical to the enumerator name.

// TBD add streaming functionality for character types.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// CLASS METHODS
                        // *** boolean tests ***

inline
bool CharType::isUpper(char character)
{
    return s_upperArray_p[static_cast<unsigned char>(character)];
}

inline
bool CharType::isLower(char character)
{
    return s_lowerArray_p[static_cast<unsigned char>(character)];
}

inline
bool CharType::isAlpha(char character)
{
    return s_alphaArray_p[static_cast<unsigned char>(character)];
}

inline
bool CharType::isDigit(char character)
{
    return s_digitArray_p[static_cast<unsigned char>(character)];
}

inline
bool CharType::isXdigit(char character)
{
    return s_xdigitArray_p[static_cast<unsigned char>(character)];
}

inline
bool CharType::isAlnum(char character)
{
    return s_alnumArray_p[static_cast<unsigned char>(character)];
}

inline
bool CharType::isSpace(char character)
{
    return s_spaceArray_p[static_cast<unsigned char>(character)];
}

inline
bool CharType::isPrint(char character)
{
    return s_printArray_p[static_cast<unsigned char>(character)];
}

inline
bool CharType::isGraph(char character)
{
    return s_graphArray_p[static_cast<unsigned char>(character)];
}

inline
bool CharType::isPunct(char character)
{
    return s_punctArray_p[static_cast<unsigned char>(character)];
}

inline
bool CharType::isCntrl(char character)
{
    return s_cntrlArray_p[static_cast<unsigned char>(character)];
}

inline
bool CharType::isAscii(char character)
{
    return s_asciiArray_p[static_cast<unsigned char>(character)];
}

inline
bool CharType::isIdent(char character)
{
    return s_identArray_p[static_cast<unsigned char>(character)];
}

inline
bool CharType::isAlund(char character)
{
    return s_alundArray_p[static_cast<unsigned char>(character)];
}

inline
bool CharType::isAll(char character)
{
    return s_allArray_p[static_cast<unsigned char>(character)];
}

inline
bool CharType::isNone(char character)
{
    return s_noneArray_p[static_cast<unsigned char>(character)];
}

inline
bool CharType::isCategory(char character,
                               CharType::Category category)
{
    return s_categoryArray_p[category][static_cast<unsigned char>(character)];
}

                        // *** character sets ***

inline
const char *CharType::stringUpper()
{
    return s_upperString_p;
}

inline
const char *CharType::stringLower()
{
    return s_lowerString_p;
}

inline
const char *CharType::stringAlpha()
{
    return s_alphaString_p;
}

inline
const char *CharType::stringDigit()
{
    return s_digitString_p;
}

inline
const char *CharType::stringXdigit()
{
    return s_xdigitString_p;
}

inline
const char *CharType::stringAlnum()
{
    return s_alnumString_p;
}

inline
const char *CharType::stringSpace()
{
    return s_spaceString_p;
}

inline
const char *CharType::stringPrint()
{
    return s_printString_p;
}

inline
const char *CharType::stringGraph()
{
    return s_graphString_p;
}

inline
const char *CharType::stringPunct()
{
    return s_punctString_p;
}

inline
const char *CharType::stringCntrl()
{
    return s_cntrlString_p;
}

inline
const char *CharType::stringAscii()
{
    return s_asciiString_p;
}

inline
const char *CharType::stringIdent()
{
    return s_identString_p;
}

inline
const char *CharType::stringAlund()
{
    return s_alundString_p;
}

inline
const char *CharType::stringAll()
{
    return s_allString_p;
}

inline
const char *CharType::stringNone()
{
    return s_noneString_p;
}

inline
const char *CharType::stringCategory(CharType::Category category)
{
    return s_categoryString_p[category];
}

                        // *** character counts ***

inline
int CharType::numUpper()
{
    return s_upperCount;
}

inline
int CharType::numLower()
{
    return s_lowerCount;
}

inline
int CharType::numAlpha()
{
    return s_alphaCount;
}

inline
int CharType::numDigit()
{
    return s_digitCount;
}

inline
int CharType::numXdigit()
{
    return s_xdigitCount;
}

inline
int CharType::numAlnum()
{
    return s_alnumCount;
}

inline
int CharType::numSpace()
{
    return s_spaceCount;
}

inline
int CharType::numPrint()
{
    return s_printCount;
}

inline
int CharType::numGraph()
{
    return s_graphCount;
}

inline
int CharType::numPunct()
{
    return s_punctCount;
}

inline
int CharType::numCntrl()
{
    return s_cntrlCount;
}

inline
int CharType::numAscii()
{
    return s_asciiCount;
}

inline
int CharType::numIdent()
{
    return s_identCount;
}

inline
int CharType::numAlund()
{
    return s_alundCount;
}

inline
int CharType::numAll()
{
    return s_allCount;
}

inline
int CharType::numNone()
{
    return s_noneCount;
}

inline
int CharType::numCategory(CharType::Category category)
{
    return s_categoryCount_p[category];
}

                        // *** miscellaneous ***

inline
const char *CharType::toAscii(CharType::Category category)
{
    return s_categoryName_p[category];
}

inline
char CharType::toLower(char input)
{
    return s_toLower_p[static_cast<unsigned char>(input)];
}

inline
char CharType::toUpper(char input)
{
    return s_toUpper_p[static_cast<unsigned char>(input)];
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
