// bdepu_escapechar.h                  -*-C++-*-
#ifndef INCLUDED_BDEPU_ESCAPECHAR
#define INCLUDED_BDEPU_ESCAPECHAR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: This component identifies second characters of escape sequences.
//
//@CLASSES:
//    bdepu_EscapeChar: namespace for escape-char related table and codes
//
//@AUTHOR: Lea Fester (lfester)
//
//@DESCRIPTION: This component provides a lookup table and an enumeration with
// which to determine, for any character, whether it is a legitimate second
// character in an escape sequence, and if so which sequence.
//
///Usage
///-----
// This example demonstrates use of both the convenient enum codes for
// describing escape sequence information, and the provided table that
// identifies whether a given character is an escape literal (i.e., a valid
// character following a leading backslash ('\\')) in an escape sequence.
//
// Suppose we want to parse and immediately reformat a buffer possibly
// containing consecutive characters representing escape sequences (e.g.,
// '\\' followed by 'n' would denote '\n') for sending to a line-oriented
// device that cannot accept the screen-oriented characters Carriage Return,
// Line Feed, or New Line.  Neither can the device back up in a reasonable
// way, so Backspace characters are ill-advised.  Any occurrence of these four
// must be replaced with two consecutive colons ("::").
//
// The following utility does such a reformatting, using the translation table
// and "packed" ordering of the enum values to transform characters whose
// codes are not consecutive ('b','f','n','r') into contiguous-value codes,
// allowing the compiler to optimize the 'switch' as a dispatch table (also
// known as a "computed 'goto'") rather than a sequence of "if - then - else"
// statements.
//
//..
//      bsl::vector<char> theBuffer("abc\t\ndef", 8);
//      int bufLen = theBuffer.length();
//      for (int i = 0; i < bufLen; ++i) {
//          if ('\\' == theBuffer[i]) {
//            switch(bdepu_EscapeChar::escapeCodes()[theBuffer[i + 1]]) {
//              case bdepu_EscapeChar::BACKSPACE:
//              case bdepu_EscapeChar::FORMFEED:
//              case bdepu_EscapeChar::NEWLINE:
//              case bdepu_EscapeChar::CARRIAGE_RETURN:
//                  theBuffer[i] = theBuffer[i + 1] = ':'; // replace with "::"
//                  break;
//              default:      // for safety
//                  break;
//            } // end switch
//          } // end if
//      }  // end for
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

namespace BloombergLP {

                        // ======================
                        // class bdepu_escapechar
                        // ======================

class bdepu_EscapeChar {
    // This class provides a name space for data and an 'enum' characterizing
    // escape-sequence character codes.

  private:
    static const unsigned char *s_escapeCodes;

  public:
    // TYPES
    enum {
        // This 'enum' provides consecutive symbolic codes with which to
        // describe distinct simple escape sequences (e.g., differentiating
        // between tab and newline) as well as a single code denoting all octal
        // digit characters, a code denoting the start of a hex sequence, and a
        // code to identify that some character is !not! legitimately part of
        // any escape sequence.

        BDEPU_ERROR = 0,        // ERROR, not part of any escape characters
        BDEPU_ALERT,            // 'a', as in '\a'
        BDEPU_BACKSPACE,        // 'b', as in '\b'
        BDEPU_FORMFEED,         // 'f', as in '\f'
        BDEPU_NEWLINE,          // 'n', as in '\n'
        BDEPU_CARRIAGE_RETURN,  // 'r', as in '\r'
        BDEPU_TAB,              // 't', as in '\t'
        BDEPU_VERTICAL_TAB,     // 'v', as in '\v'
        BDEPU_QUESTION_MARK,    // '?', as in '\?'
        BDEPU_SINGLE_QUOTE,     // '\'', as in "\\\'"
        BDEPU_DOUBLE_QUOTE,     // '"', as in '\"'
        BDEPU_HEXADECIMAL,      // 'x', as in '\xaF'
        BDEPU_OCTAL,            // [0 .. 7], as in '\007'
        BDEPU_BACKSLASH,        // '\\', as in "\\\\"
        BDEPU_ENDLINE           // 0, as in '\0'

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , ERROR           = BDEPU_ERROR
      , ALERT           = BDEPU_ALERT
      , BACKSPACE       = BDEPU_BACKSPACE
      , FORMFEED        = BDEPU_FORMFEED
      , NEWLINE         = BDEPU_NEWLINE
      , CARRIAGE_RETURN = BDEPU_CARRIAGE_RETURN
      , TAB             = BDEPU_TAB
      , VERTICAL_TAB    = BDEPU_VERTICAL_TAB
      , QUESTION_MARK   = BDEPU_QUESTION_MARK
      , SINGLE_QUOTE    = BDEPU_SINGLE_QUOTE
      , DOUBLE_QUOTE    = BDEPU_DOUBLE_QUOTE
      , HEXADECIMAL     = BDEPU_HEXADECIMAL
      , OCTAL           = BDEPU_OCTAL
      , BACKSLASH       = BDEPU_BACKSLASH
      , ENDLINE         = BDEPU_ENDLINE
#endif
    };

    static const unsigned char *escapeCodes(void);
        // Return the address of a table classifying for each character
        // code [0 .. 255], to which escape sequence, if any, a character
        // belongs.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// CLASS METHODS
inline
const unsigned char *bdepu_EscapeChar::escapeCodes(void)
{
    return s_escapeCodes;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
