// bdepu_parserimputil.cpp                                            -*-C++-*-
#include <bdepu_parserimputil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdepu_parserimputil_cpp,"$Id$ $CSID$")

#include <bdepu_escapechar.h>

#include <bsls_assert.h>
#include <bsls_platformutil.h>

#include <bsl_cctype.h>   // isspace(), isdigit(), isalpha(), isalnum()
#include <bsl_cstring.h>  // strlen()

namespace BloombergLP {

enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = 1 };

typedef bsls_PlatformUtil::Int64 Int64;

                    // --------------------------
                    // struct bdepu_ParserImpUtil
                    // --------------------------

// CLASS METHODS
int bdepu_ParserImpUtil::characterToDigit(char character, int base)
{
// Some libraries still define this as a macro.
#ifdef isdigit
    using bsl::isdigit;
#endif

    int digit = isdigit(static_cast<unsigned char>(character))
                ? int(character - '0')
                : 'A' <= character && character <= 'Z'
                  ? int(character - 'A' + 10)
                  : 'a' <= character && character <= 'z'
                    ? int(character - 'a' + 10)
                    : -1;

    return digit < base ? digit : -1;
}

void bdepu_ParserImpUtil::compactMultibyteChars(bsl::string *inOutString)
{
    BSLS_ASSERT(inOutString);

    const char *nextReadChar = inOutString->data();
    char *nextWriteChar = const_cast<char *>(nextReadChar);
    char c;

    while (0 != (c = *nextReadChar)) {
        if ('\\' == c) {
            const char *next;
            if (0 == parseEscapeSequence(&next, &c, nextReadChar + 1)) {
                nextReadChar = next;
            }
            else {
                // This is not a valid escape sequence; parse as text.
                ++nextReadChar;
            }
        }
        else {
            ++nextReadChar;
        }
        *nextWriteChar++ = c;
    }

    inOutString->resize(nextWriteChar - inOutString->data());
}

bool bdepu_ParserImpUtil::isIdentifier(const bsl::string& inString)
{
// Some libraries still define these as macros.
#ifndef isalnum
    using bsl::isalnum;
    using bsl::isdigit;
#endif

    // Check the first character (no numbers allowed).
    if (inString.empty() || isdigit(static_cast<unsigned char>(inString[0]))) {
        return false;                                                 // RETURN
    }

    bsl::string::const_iterator endStr = inString.end();
    for (bsl::string::const_iterator i = inString.begin(); i != endStr; ++i) {
        char nextChar = *i;
        if (!(isalnum(static_cast<unsigned char>(nextChar))
         || '_' == nextChar)) {
            return false;                                             // RETURN
        }
    }

    return true;
}

                        // *** PARSE FUNCTIONS ***

int bdepu_ParserImpUtil::parseEscapeSequence(const char **endPos,
                                             char        *result,
                                             const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    int rv = BDEPU_SUCCESS;

    const int charType =
                 bdepu_EscapeChar::escapeCodes()[(unsigned char) *inputString];
    switch (charType) {
      case bdepu_EscapeChar::BDEPU_ERROR: {
        // Not a valid escape sequence.
        *endPos = inputString;
        rv = BDEPU_FAILURE;
      } break;
      case bdepu_EscapeChar::BDEPU_ALERT: {
        *result = '\a';
        *endPos = inputString + 1;
      } break;
      case bdepu_EscapeChar::BDEPU_BACKSPACE: {
        *result = '\b';
        *endPos = inputString + 1;
      } break;
      case bdepu_EscapeChar::BDEPU_FORMFEED: {
        *result = '\f';
        *endPos = inputString + 1;
      } break;
      case bdepu_EscapeChar::BDEPU_NEWLINE: {
        *result = '\n';
        *endPos = inputString + 1;
      } break;
      case  bdepu_EscapeChar::BDEPU_CARRIAGE_RETURN: {
        *result = '\r';
        *endPos = inputString + 1;
      } break;
      case bdepu_EscapeChar::BDEPU_TAB: {
        *result = '\t';
        *endPos = inputString + 1;
      } break;
      case bdepu_EscapeChar::BDEPU_VERTICAL_TAB: {
        *result = '\v';
        *endPos = inputString + 1;
      } break;
      case bdepu_EscapeChar::BDEPU_QUESTION_MARK: {
        *result = '?';
        *endPos = inputString + 1;
      } break;
      case bdepu_EscapeChar::BDEPU_SINGLE_QUOTE: {
        *result = '\'';
        *endPos = inputString + 1;
      } break;
      case bdepu_EscapeChar::BDEPU_DOUBLE_QUOTE: {
        *result = '"';
        *endPos = inputString + 1;
      } break;
      case bdepu_EscapeChar::BDEPU_HEXADECIMAL: {
        // hex escape sequence
        ++inputString;
        bsls_PlatformUtil::Uint64 hexRes = 0;
        if (parseUnsignedInteger(endPos, &hexRes, inputString,
                                                       16, 0xFFFFFFFFuLL, 2)) {
            rv = BDEPU_FAILURE;
            break;
        }
        *result = static_cast<char>(hexRes);
      } break;
      case bdepu_EscapeChar::BDEPU_OCTAL: {
        // octal escape sequence
        bsls_PlatformUtil::Uint64 intRes = 0;
        // Note that the next line cannot fail since there is already an octal
        // digit present.
        parseUnsignedInteger(endPos, &intRes, inputString, 8, 255, 3);
        *result = static_cast<char>(intRes);
      } break;
      case bdepu_EscapeChar::BDEPU_BACKSLASH: {
        *result = '\\';
        *endPos = inputString + 1;
      } break;
      case bdepu_EscapeChar::BDEPU_ENDLINE: {
        // Not a valid escape sequence.
        *endPos = inputString;
        rv = BDEPU_FAILURE;
      } break;

    }
    return rv;
}

int bdepu_ParserImpUtil::parseIdentifier(const char  **endPos,
                                         bsl::string  *result,
                                         const char   *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    const char *startPos = inputString;
    const char *curPos = inputString;

    // Parse first character.
    if (!(isalpha(static_cast<unsigned char>(*curPos)) || *curPos == '_'))
    {
        *endPos = curPos;
        return BDEPU_FAILURE;
    }

    // Parse the rest of the identifier.
    ++curPos;
    while (isalnum(static_cast<unsigned char>(*curPos)) || *curPos == '_') {
        ++curPos;
    }

    // Parsing successful, prepare 'result'.
    result->erase();
    result->append(startPos, curPos - startPos);

    *endPos = curPos;
    return BDEPU_SUCCESS;
}

int bdepu_ParserImpUtil::parseNull(const char **endPos,
                                   const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(inputString);

    if ('N' != bdeu_CharType::toUpper(*(*endPos = inputString++))) {
        return BDEPU_FAILURE;
    }
    if ('U' != bdeu_CharType::toUpper(*(*endPos = inputString++))) {
        return BDEPU_FAILURE;
    }
    if ('L' != bdeu_CharType::toUpper(*(*endPos = inputString++))) {
        return BDEPU_FAILURE;
    }
    if ('L' != bdeu_CharType::toUpper(*(*endPos = inputString++))) {
        return BDEPU_FAILURE;
    }
    *endPos = inputString;
    return BDEPU_SUCCESS;
}

int bdepu_ParserImpUtil::parseSignedInteger(
                                  const char                     **endPos,
                                  bsls_PlatformUtil::Int64        *result,
                                  const char                      *inputString,
                                  int                              base,
                                  const bsls_PlatformUtil::Int64   minValue,
                                  const bsls_PlatformUtil::Int64   maxValue)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);
    BSLS_ASSERT(minValue <= 0);
    BSLS_ASSERT(maxValue >= 0);

    bsls_PlatformUtil::Uint64 res = *result;
    int rv;

    if ('-' == *inputString) {
        rv = parseUnsignedInteger(endPos,
                                  &res,
                                  inputString + 1,
                                  base,
                                  (bsls_PlatformUtil::Uint64)~minValue + 1);
        if (!rv) {
            res = -res;
        }

    }
    else {
        rv = parseUnsignedInteger(endPos,
                                  &res,
                                  inputString + ('+' == *inputString),
                                  base,
                                  maxValue);
    }

    *result = res;
    return rv;
}

int bdepu_ParserImpUtil::parseUnsignedInteger(
                                 const char                      **endPos,
                                 bsls_PlatformUtil::Uint64        *result,
                                 const char                       *inputString,
                                 int                               base,
                                 const bsls_PlatformUtil::Uint64   maxValue)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    const bsls_PlatformUtil::Uint64 maxCheck = maxValue / base;
    const char *start = inputString;

    bsls_PlatformUtil::Uint64 res = 0;
    int digit = characterToDigit(*inputString, base);
    while (-1 != digit) {
        if (res < maxCheck) {
            res = res * base + digit;
            ++inputString;
            digit = characterToDigit(*inputString, base);
        }
        else if (res == maxCheck && digit <= (int)(maxValue % base)) {
            res = res * base + digit;
            ++inputString;
            break;
        }
        else {
            break;
        }
    }

    *endPos = inputString;
    if (start == inputString) {
        // ERROR: The number must have at least one digit.
        return BDEPU_FAILURE;
    }

    *result = res;
    return BDEPU_SUCCESS;
}

int bdepu_ParserImpUtil::parseUnsignedInteger(
                                const char                      **endPos,
                                bsls_PlatformUtil::Uint64        *result,
                                const char                       *inputString,
                                int                               base,
                                const bsls_PlatformUtil::Uint64   maxValue,
                                int                               maxNumDigits)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);
    BSLS_ASSERT(0 <= maxNumDigits);

    const bsls_PlatformUtil::Uint64 maxCheck = maxValue / base;
    const char *start = inputString;

    bsls_PlatformUtil::Uint64 res = 0;
    int digit = characterToDigit(*inputString, base);
    while (-1 != digit && maxNumDigits--) {
        if (res < maxCheck) {
            res = res * base + digit;
            ++inputString;
            digit = characterToDigit(*inputString, base);
        }
        else if (res == maxCheck && digit <= (int)(maxValue % base)) {
            res = res * base + digit;
            ++inputString;
            break;
        }
        else {
            break;
        }
    }

    *endPos = inputString;
    if (start == inputString) {
        // ERROR: The number must have at least one digit.
        return BDEPU_FAILURE;
    }

    *result = res;
    return BDEPU_SUCCESS;
}

                        // *** SKIP FUNCTIONS ***

int bdepu_ParserImpUtil::skipRequiredCaselessToken(const char **endPos,
                                                   const char  *inputString,
                                                   const char  *token)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(token);

    *endPos = inputString;
    while ('\0' != *token
        && bdeu_CharType::toUpper(*token) == bdeu_CharType::toUpper(**endPos))
                                                                              {
        ++*endPos;
        ++token;
    }

    return '\0' != *token;  // return "is null char"
}

int bdepu_ParserImpUtil::skipRequiredToken(const char **endPos,
                                           const char  *inputString,
                                           const char  *token)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(token);

    *endPos = inputString;
    while ('\0' != *token && *token == **endPos) {
        ++*endPos;
        ++token;
    }

    return '\0' != *token;  // return "is null char"
}

int bdepu_ParserImpUtil::skipWhiteSpace(const char **endPos,
                                        const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(inputString);

    while (1) {
        while (isspace(static_cast<unsigned char>(*inputString))) {
            ++inputString;
        }
        if (*inputString != '/') {
            *endPos = inputString;
            return BDEPU_SUCCESS;
        }
        else {
            ++inputString;
            if ('*' == *inputString) {
                // This is a C comment.
                ++inputString;
                while (1) {
                    if ('\0' == *inputString) {
                        // Comment is erroneous.
                        *endPos = inputString;
                        return BDEPU_FAILURE;
                    }
                    if ('*' == *inputString && '/' == *(inputString + 1)) {
                        // Found end of comment.
                        inputString += 2;
                        break;
                    }
                    ++inputString;
                }
            } else if ('/' == *inputString) {
                // This is a C++ comment.
                ++inputString;
                while (1) {
                    if ('\n' == *inputString) {
                        // End of string means end of comment.
                        ++inputString;
                        break;
                    }
                    if ('\0' == *inputString) {
                        // Reached end of string.
                        *endPos = inputString;
                        return BDEPU_SUCCESS;
                    }
                    ++inputString;
                }
            } else {
                *endPos = inputString - 1;
                return BDEPU_SUCCESS;
            }
        }
    }
}

                        // *** GENERATE FUNCTIONS

void bdepu_ParserImpUtil::generateIdentifier(bsl::vector<char> *buffer,
                                             const char        *value,
                                             int                level,
                                             int                spacesPerLevel)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(value);

    if (level > 0) {
        generateIndentation(buffer, level, spacesPerLevel);
    }
    int len = bsl::strlen(value);
    buffer->reserve(buffer->size() + len);
    for (int i = 0; i < len; ++i) {
        buffer->push_back(value[i]);
    }
}

void bdepu_ParserImpUtil::generateNull(bsl::vector<char> *buffer)
{
    BSLS_ASSERT(buffer);

    buffer->reserve(buffer->size() + 4);
    buffer->push_back('N');
    buffer->push_back('U');
    buffer->push_back('L');
    buffer->push_back('L');
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
