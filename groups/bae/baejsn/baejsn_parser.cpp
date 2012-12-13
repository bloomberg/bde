// baejsn_parser.cpp                                                  -*-C++-*-
#include <baejsn_parser.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_parser_cpp,"$Id$ $CSID$")

#include <bsl_streambuf.h>

// TBD: Remove
#include <bsl_iostream.h>

// IMPLEMENTATION NOTES
// --------------------

// This section will provide a little background on how this component is
// implemented.  This component presents a parser that has the following
// states:
//..
//                                   +---------+
//         +-------------------------| 'ERROR' |
//         |                         +---------+
//         V
//      +-----+ <--------------------------------------------------- +-----+
//      | '{' |--------------------------+                           | '[' |
//      +-----+                          |                           +-----+
//       ^ | ^                           |                            ^ ^ |
//       | | |                           V                            | | |
//       | | |                   +--------+                         | | |
//       | | +------------------ | 'NAME' |-------------------------+ | |
//       | |                     +--------+                           | |
//       | |                             |                            | |
//       | |                 +-----+     |                            | |
//       | |                 |     V     V                            | |
//       | |                 |   +---------+                          | |
//       | |                 |   | 'VALUE' |<-------------------------+ |
//       | |                 |   +---------+                            |
//       | |                 |     | | |                                |
//   +-+ | |                 +-----+ | |                                |
//   | V V +-------------------------+ |                                V
//   |  +-----+                       / \                            +-----+
//   |  | '}' | <--------------------+   +-------------------------> | ']' |
//   |  +-----+ <--------------------------------------------------> +-----+
//   |   |
//   +---+
//..
// For clarity only the trailing words of tokens is used below.
//
//   Current Token             Next Character             Following Token
//   -------------             --------------             ---------------
//   ERROR, START_ARRAY             '{'                   START_OBJECT
//   NAME, END_OBJECT
//
//   VALUE, START_OBJECT            '}'                   END_OBJECT
//   END_ARRAY, END_OBJECT
//
//   NAME, VALUE                    '['                   START_ARRAY
//
//   VALUE, START_ARRAY             ']'                   END_ARRAY
//
//   NAME                           ':'                   VALUE, START_OBJECT
//                                                        START_ARRAY
//
//   START_OBJECT                   '"'                   NAME
//
//   VALUE, START_ARRAY             '"'                   END_ARRAY
//
//   START_ARRAY, NAME, VAlUE       Non '"' character     VALUE
//..

namespace BloombergLP {

namespace {
    const char *WHITESPACE = " \n\t\v\f\r";
    const char *TOKENS     = "{}[]:,";
}

                            // --------------------
                            // struct baejsn_Parser
                            // --------------------

// PRIVATE MANIPULATORS
int baejsn_Parser::reloadStringBuffer()
{
    d_stringBuffer.resize(BAEJSN_MAX_STRING_SIZE);
    const int numRead = d_streamBuf_p->sgetn(&d_stringBuffer[0],
                                             BAEJSN_MAX_STRING_SIZE);
    d_cursor = 0;
    d_stringBuffer.resize(numRead);
    return numRead;
}

int baejsn_Parser::skipWhitespace()
{
    while (true) {
        bsl::size_t pos = d_stringBuffer.find_first_not_of(WHITESPACE,
                                                           d_cursor);
        if (bsl::string::npos != pos) {
            d_cursor = pos;
            break;
        }

        const int numRead = reloadStringBuffer();
        if (0 == numRead) {
            return -1;                                                // RETURN
        }
    }
    return 0;
}

int baejsn_Parser::extractStringValue()
{
    d_valueBegin          = d_cursor;
    bsl::size_t iter      = d_cursor + 1;
    bool        firstTime = true;

    while (true) {
        while (iter < d_stringBuffer.length()
            && '"' != d_stringBuffer[iter]) {
            ++iter;
        }

        if (iter == d_stringBuffer.length()) {
            // TBD: Refactor
            if (!firstTime) {
                d_stringBuffer.resize(d_stringBuffer.length()
                                                     + BAEJSN_MAX_STRING_SIZE);
                int numRead = d_streamBuf_p->sgetn(&d_stringBuffer[iter],
                                                   BAEJSN_MAX_STRING_SIZE);
                if (0 == numRead) {
                    return -1;                                        // RETURN
                }
                continue;
            }

            d_stringBuffer.erase(d_stringBuffer.begin(),
                                 d_stringBuffer.begin() + d_cursor);
            d_stringBuffer.resize(BAEJSN_MAX_STRING_SIZE);
            iter = BAEJSN_MAX_STRING_SIZE - d_cursor;
            int numRead = d_streamBuf_p->sgetn(&d_stringBuffer[iter],
                                               d_cursor);
            if (0 == numRead) {
                return -1;                                            // RETURN
            }

            d_stringBuffer.resize(iter + numRead);
            d_valueBegin = 0;
            firstTime = false;
        }
        else {
            d_valueEnd = iter;
            return 0;                                                 // RETURN
        }
    }
    return 0;
}

int baejsn_Parser::skipNonWhitespaceOrTillToken()
{
    d_valueBegin          = d_cursor;
    bsl::size_t iter      = d_cursor + 1;
    bool        firstTime = true;

    while (true) {
        while (iter < d_stringBuffer.length()
            && !bsl::isspace(d_stringBuffer[iter])
            && !bsl::strchr(TOKENS, d_stringBuffer[iter])) {
            ++iter;
        }

        if (iter == d_stringBuffer.length()) {
            if (!firstTime) {
                d_stringBuffer.resize(d_stringBuffer.length()
                                                     + BAEJSN_MAX_STRING_SIZE);
                int numRead = d_streamBuf_p->sgetn(&d_stringBuffer[iter],
                                                   BAEJSN_MAX_STRING_SIZE);
                if (0 == numRead) {
                    return -1;                                        // RETURN
                }
                continue;
            }

            d_stringBuffer.erase(d_stringBuffer.begin(),
                                 d_stringBuffer.begin() + d_cursor);
            d_stringBuffer.resize(BAEJSN_MAX_STRING_SIZE);
            iter = BAEJSN_MAX_STRING_SIZE - d_cursor;
            int numRead = d_streamBuf_p->sgetn(&d_stringBuffer[iter],
                                               d_cursor);
            if (0 == numRead) {
                return -1;                                            // RETURN
            }

            d_stringBuffer.resize(iter + numRead);
            d_valueBegin = 0;
            firstTime = false;
        }
        else {
            d_valueEnd = iter;
            return 0;                                                 // RETURN
        }
    }
    return 0;
}

// MANIPULATORS
int baejsn_Parser::advanceToNextToken()
{
    if (d_cursor >= d_stringBuffer.size()) {
        const int numRead = reloadStringBuffer();
        if (0 == numRead) {
            return -1;                                                // RETURN
        }
    }

    bool continueFlag;
    char previousChar = 0;
    do {
        continueFlag = false;

        const int rc = skipWhitespace();
        if (rc) {
            return -1;                                                // RETURN
        }

//         bsl::cout << d_stringBuffer[d_cursor] << bsl::endl;

        switch (d_stringBuffer[d_cursor]) {
          case '{': {
            if (BAEJSN_ELEMENT_NAME == d_tokenType
             || BAEJSN_START_ARRAY  == d_tokenType
             || (BAEJSN_END_OBJECT  == d_tokenType && ',' == previousChar)
             || BAEJSN_ERROR        == d_tokenType) {

                d_tokenType  = BAEJSN_START_OBJECT;
                d_context    = BAEJSN_OBJECT_CONTEXT;
                previousChar = '{';

                ++d_cursor;
            }
            else {
                return -1;                                            // RETURN
            }
          } break;

          case '}': {
            if (BAEJSN_ELEMENT_VALUE == d_tokenType
             || BAEJSN_START_OBJECT  == d_tokenType
             || BAEJSN_END_OBJECT    == d_tokenType
             || BAEJSN_END_ARRAY     == d_tokenType) {

                d_tokenType  = BAEJSN_END_OBJECT;
                d_context    = BAEJSN_OBJECT_CONTEXT;
                previousChar = '}';

                ++d_cursor;
            }
            else {
                return -1;                                            // RETURN
            }
          } break;

          case '[': {
            if (BAEJSN_ELEMENT_NAME   == d_tokenType
             || BAEJSN_START_ARRAY    == d_tokenType
             || (BAEJSN_ELEMENT_VALUE == d_tokenType && ',' == previousChar)) {

                d_tokenType  = BAEJSN_START_ARRAY;
                d_context    = BAEJSN_ARRAY_CONTEXT;
                previousChar = '[';

                ++d_cursor;
            }
            else {
                return -1;                                            // RETURN
            }
          } break;

          case ']': {
            if (BAEJSN_ELEMENT_VALUE == d_tokenType
             || BAEJSN_START_ARRAY   == d_tokenType
             || BAEJSN_END_OBJECT    == d_tokenType) {

                d_tokenType = BAEJSN_END_ARRAY;
                d_context   = BAEJSN_OBJECT_CONTEXT;
                previousChar = ']';

                ++d_cursor;
            }
            else {
                return -1;                                            // RETURN
            }
          } break;

          case ',': {
            if (BAEJSN_ELEMENT_VALUE == d_tokenType
             || BAEJSN_END_OBJECT    == d_tokenType
             || BAEJSN_END_ARRAY     == d_tokenType) {

                previousChar = ',';
                continueFlag = true;

                ++d_cursor;
            }
            else {
                return -1;                                            // RETURN
            }
          } break;

          case ':': {
            if (BAEJSN_ELEMENT_NAME == d_tokenType) {

                previousChar = ':';
                continueFlag = true;

                ++d_cursor;
            }
            else {
                return -1;                                            // RETURN
            }
          } break;

          case '"': {

            // Here are the scenarios for a '"':
            //
            // PREVIOUS TOKEN          CONTEXT           RESULT TOKEN
            // --------------          -------           ------------
            // START_OBJECT  ('{')                       ELEMENT_NAME
            // END_OBJECT    ('}')                       ELEMENT_NAME
            // START_ARRAY   ('[')                       ELEMENT_VALUE
            // END_ARRAY     (']')                       ELEMENT_VALUE
            // ELEMENT_NAME  (':')                       ELEMENT_VALUE
            // ELEMENT_VALUE (   )     OBJECT_CONTEXT    ELEMENT_NAME
            // ELEMENT_VALUE (   )     ARRAY_CONTEXT     ELEMENT_VALUE

            if (BAEJSN_START_OBJECT   == d_tokenType
             || BAEJSN_END_OBJECT     == d_tokenType
             || BAEJSN_END_ARRAY      == d_tokenType
             || (BAEJSN_ELEMENT_VALUE == d_tokenType
                                      && BAEJSN_OBJECT_CONTEXT == d_context)) {
                d_tokenType = BAEJSN_ELEMENT_NAME;
                ++d_cursor;
            }
            else if (BAEJSN_START_ARRAY    == d_tokenType
                  || BAEJSN_ELEMENT_NAME   == d_tokenType
                  || (BAEJSN_ELEMENT_VALUE == d_tokenType
                                       && BAEJSN_ARRAY_CONTEXT == d_context)) {
                d_tokenType = BAEJSN_ELEMENT_VALUE;
            }
            else {
                return -1;                                            // RETURN
            }

            d_valueBegin = 0;
            d_valueEnd   = 0;
            int rc = extractStringValue();
            if (rc) {
                return -1;                                            // RETURN
            }

            if (BAEJSN_ELEMENT_NAME == d_tokenType) {
                d_cursor = d_valueEnd + 1;
            }
            else {
                // Advance past the end '"'.

                ++d_valueEnd;
                d_cursor = d_valueEnd;
            }

            previousChar = '"';
          } break;

          default: {
            if (BAEJSN_START_ARRAY   == d_tokenType 
             || BAEJSN_ELEMENT_NAME  == d_tokenType 
             || BAEJSN_ELEMENT_VALUE == d_tokenType) {

                d_tokenType = BAEJSN_ELEMENT_VALUE;

                d_valueBegin = 0;
                d_valueEnd   = 0;
                const int rc = skipNonWhitespaceOrTillToken();
                if (rc) {
                    return -1;                                        // RETURN
                }
                d_cursor     = d_valueEnd;
                previousChar = 0;
            }
            else {
                return -1;                                            // RETURN
            }
          } break;
        }
    } while (continueFlag);

    return 0;
}

// ACCESSORS
int baejsn_Parser::value(bslstl::StringRef *data) const
{
    if ((BAEJSN_ELEMENT_NAME == d_tokenType
                                        || BAEJSN_ELEMENT_VALUE == d_tokenType)
     && d_valueBegin != d_valueEnd) {
        data->assign(&d_stringBuffer[d_valueBegin],
                     &d_stringBuffer[d_valueEnd]);
//         bsl::cout << *data  << bsl::endl;
        return 0;                                                     // RETURN
    }
    return -1;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
