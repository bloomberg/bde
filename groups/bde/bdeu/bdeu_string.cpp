// bdeu_string.cpp                                                    -*-C++-*-
#include <bdeu_string.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdeu_string_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_cctype.h>
#include <bsl_cstring.h>

namespace BloombergLP {

                        // ------------------
                        // struct bdeu_String
                        // ------------------

// CLASS METHODS
bool bdeu_String::areEqualCaseless(const char *lhsString,
                                   const char *rhsString)
{
    BSLS_ASSERT(lhsString);
    BSLS_ASSERT(rhsString);

    int i = 0;
    while (lhsString[i]) {
        unsigned char lhs =
                        bsl::tolower(static_cast<unsigned char>(lhsString[i]));
        unsigned char rhs =
                        bsl::tolower(static_cast<unsigned char>(rhsString[i]));
        if (lhs != rhs) {
            return false;                                             // RETURN
        }
        ++i;
    }
    return 0 == rhsString[i] ? true : false;
}

bool bdeu_String::areEqualCaseless(const char *lhsString,
                                   const char *rhsString,
                                   int         rhsLength)
{
    BSLS_ASSERT(lhsString);
    BSLS_ASSERT(rhsString);
    BSLS_ASSERT(0 <= rhsLength);

    for (int i = 0; i < rhsLength; ++i) {
        unsigned char lhs =
                        bsl::tolower(static_cast<unsigned char>(lhsString[i]));
        unsigned char rhs =
                        bsl::tolower(static_cast<unsigned char>(rhsString[i]));
        if (lhs != rhs || !lhs) {
            return false;                                             // RETURN
        }
    }
    return 0 == lhsString[rhsLength] ? true : false;
}

bool bdeu_String::areEqualCaseless(const char *lhsString,
                                   int         lhsLength,
                                   const char *rhsString,
                                   int         rhsLength)
{
    BSLS_ASSERT(lhsString);
    BSLS_ASSERT(0 <= lhsLength);
    BSLS_ASSERT(rhsString);
    BSLS_ASSERT(0 <= rhsLength);

    if (lhsLength != rhsLength) {
        return false;                                                 // RETURN
    }
    int i = 0;
    while (i < lhsLength) {
        unsigned char lhs =
                        bsl::tolower(static_cast<unsigned char>(lhsString[i]));
        unsigned char rhs =
                        bsl::tolower(static_cast<unsigned char>(rhsString[i]));
        if (lhs != rhs) {
            return false;                                             // RETURN
        }
        ++i;
    }
    return true;
}

char *bdeu_String::copy(const char      *string,
                        int              length,
                        bslma_Allocator *basicAllocator)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(basicAllocator);

    char *newString =
                reinterpret_cast<char *>(basicAllocator->allocate(length + 1));
    bsl::memcpy(newString, string, length);
    newString[length] = 0;

    return newString;
}

int bdeu_String::lowerCaseCmp(const char *lhsString, const char *rhsString)
{
    BSLS_ASSERT(lhsString);
    BSLS_ASSERT(rhsString);

    int i = 0;
    while (lhsString[i]) {
        unsigned char lhs =
                        bsl::tolower(static_cast<unsigned char>(lhsString[i]));
        unsigned char rhs =
                        bsl::tolower(static_cast<unsigned char>(rhsString[i]));
        if (lhs != rhs) {
            return lhs < rhs ? -1 : 1;                                // RETURN
        }
        ++i;
    }
    return 0 == rhsString[i] ? 0 : -1;
}

int bdeu_String::lowerCaseCmp(const char *lhsString,
                              const char *rhsString,
                              int         rhsLength)
{
    BSLS_ASSERT(lhsString);
    BSLS_ASSERT(rhsString);
    BSLS_ASSERT(0 <= rhsLength);

    for (int i = 0; i < rhsLength; ++i) {
        unsigned char lhs =
                        bsl::tolower(static_cast<unsigned char>(lhsString[i]));
        unsigned char rhs =
                        bsl::tolower(static_cast<unsigned char>(rhsString[i]));
        if (lhs != rhs || !lhs) {
            return lhs < rhs ? -1 : 1;                                // RETURN
        }
    }
    return 0 == lhsString[rhsLength] ? 0 : 1;
}

int bdeu_String::lowerCaseCmp(const char *lhsString,
                              int         lhsLength,
                              const char *rhsString,
                              int         rhsLength)
{
    BSLS_ASSERT(lhsString);
    BSLS_ASSERT(0 <= lhsLength);
    BSLS_ASSERT(rhsString);
    BSLS_ASSERT(0 <= rhsLength);

    int min = lhsLength < rhsLength ? lhsLength : rhsLength;
    int i = 0;
    while (i < min) {
        unsigned char lhs =
                        bsl::tolower(static_cast<unsigned char>(lhsString[i]));
        unsigned char rhs =
                        bsl::tolower(static_cast<unsigned char>(rhsString[i]));
        if (lhs != rhs) {
            return lhs < rhs ? -1 : 1;                                // RETURN
        }
        ++i;
    }
    return lhsLength < rhsLength ? -1 : lhsLength == rhsLength ? 0 : 1;
}

void bdeu_String::ltrim(char *string)
{
    BSLS_ASSERT(string);

    int index = 0;
    while (bsl::isspace(static_cast<unsigned char>(string[index]))) {
        ++index;
    }
    bsl::size_t len = bsl::strlen(string + index);
    bsl::memmove(string, &string[index], len + 1);
}

void bdeu_String::ltrim(char *string, int *length)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(length);
    BSLS_ASSERT(0 <= *length);

    int index = 0;
    while (index < *length
        && bsl::isspace(static_cast<unsigned char>(string[index]))) {
        ++index;
    }
    *length -= index;
    bsl::memmove(string, &string[index], *length);
}

void bdeu_String::rtrim(char *string)
{
    BSLS_ASSERT(string);

    int index = 0;
    while (string[index]) {
        ++index;
    }
    --index;
    while (index >= 0
                  && bsl::isspace(static_cast<unsigned char>(string[index]))) {
        string[index] = '\0';
        --index;
    }
}

void bdeu_String::rtrim(const char *string, int *length)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(length);
    BSLS_ASSERT(0 <= *length);

    if (*length) {
        int index = *length - 1;
        while (index >= 0
                  && bsl::isspace(static_cast<unsigned char>(string[index]))) {
            --index;
        }
        *length = index + 1;
    }
}

const char *bdeu_String::strstr(const char *string,
                                int         stringLen,
                                const char *subString,
                                int         subStringLen)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= stringLen);
    BSLS_ASSERT(subString);
    BSLS_ASSERT(0 <= subStringLen);

    if (0 == subStringLen) {
        return string;                                                // RETURN
    }

    if (stringLen < subStringLen) {
        return 0;                                                     // RETURN
    }

    const char *end = string + stringLen - subStringLen;

    for (const char *p = string; *p && p <= end; ++p) {
        if (0 == bsl::memcmp(p, subString, subStringLen)) {
            return p;                                                 // RETURN
        }
    }

    return 0;
}

const char *bdeu_String::strstrCaseless(const char *string,
                                        int         stringLen,
                                        const char *subString,
                                        int         subStringLen)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= stringLen);
    BSLS_ASSERT(subString);
    BSLS_ASSERT(0 <= subStringLen);

    if (0 == subStringLen) {
        return string;                                                // RETURN
    }

    if (stringLen < subStringLen) {
        return 0;                                                     // RETURN
    }

    const char *end = string + stringLen - subStringLen;

    for (const char *p = string; *p && p <= end; ++p) {
        int i;

        for (i = 0; i < subStringLen; ++i) {
            if (bsl::toupper(static_cast<unsigned char>(p[i]))
                   != bsl::toupper(static_cast<unsigned char>(subString[i]))) {
                break;
            }
        }

        if (i == subStringLen) {
            return p;                                                 // RETURN
        }
    }

    return 0;
}

void bdeu_String::toFixedLength(char       *dstString,
                                int         dstLength,
                                const char *srcString,
                                int         srcLength,
                                char        padChar)
{
    BSLS_ASSERT(dstString);
    BSLS_ASSERT(0 <= dstLength);
    BSLS_ASSERT(srcString);
    BSLS_ASSERT(0 <= srcLength);

    // TBD make alias safe
    if (dstLength < srcLength) {
        bsl::memcpy(dstString, srcString, dstLength);
    }
    else {
        bsl::memcpy(dstString, srcString, srcLength);
        for (int i = srcLength; i < dstLength; ++i) {
            dstString[i] = padChar;
        }
    }
}

int bdeu_String::strnlen(const char *string, int maximumLength)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= maximumLength);

    int i = 0;
    while (i < maximumLength && string[i]) {
        ++i;
    }
    return i;
}

void bdeu_String::toLower(char *string)
{
    BSLS_ASSERT(string);

    for (int i = 0; string[i]; ++i) {
        string[i] = bsl::tolower(static_cast<unsigned char>(string[i]));
    }
}

void bdeu_String::toLower(char *string, int length)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    for (int i = 0; i < length; ++i) {
        string[i] = bsl::tolower(static_cast<unsigned char>(string[i]));
    }
}

void bdeu_String::toUpper(char *string)
{
    BSLS_ASSERT(string);

    for (int i = 0; string[i]; ++i) {
        string[i] = bsl::toupper(static_cast<unsigned char>(string[i]));
    }
}

void bdeu_String::toUpper(char *string, int length)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    for (int i = 0; i < length; ++i) {
        string[i] = bsl::toupper(static_cast<unsigned char>(string[i]));
    }
}

void bdeu_String::skipLeadingTrailing(const char **begin, const char **end)
{
    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);
    BSLS_ASSERT(*begin <= *end);

    const char *p = *begin;
    const char *q = *end;

    --q;

    while (q >= p && bsl::isspace(static_cast<unsigned char>(*q))) {
        --q;
    }

    while (p < q && bsl::isspace(static_cast<unsigned char>(*p))) {
        ++p;
    }

    ++q;

    *begin = p;
    *end   = q;
}

int bdeu_String::upperCaseCmp(const char *lhsString, const char *rhsString)
{
    BSLS_ASSERT(lhsString);
    BSLS_ASSERT(rhsString);

    int i = 0;
    while (lhsString[i]) {
        unsigned char lhs =
                        bsl::toupper(static_cast<unsigned char>(lhsString[i]));
        unsigned char rhs =
                        bsl::toupper(static_cast<unsigned char>(rhsString[i]));
        if (lhs != rhs) {
            return lhs < rhs ? -1 : 1;                                // RETURN
        }
        ++i;
    }
    return 0 == rhsString[i] ? 0 : -1;
}

int bdeu_String::upperCaseCmp(const char *lhsString,
                              const char *rhsString,
                              int         rhsLength)
{
    BSLS_ASSERT(lhsString);
    BSLS_ASSERT(rhsString);
    BSLS_ASSERT(0 <= rhsLength);

    for (int i = 0; i < rhsLength; ++i) {
        unsigned char lhs =
                        bsl::toupper(static_cast<unsigned char>(lhsString[i]));
        unsigned char rhs =
                        bsl::toupper(static_cast<unsigned char>(rhsString[i]));
        if (lhs != rhs || !lhs) {
            return lhs < rhs ? -1 : 1;                                // RETURN
        }
    }
    return 0 == lhsString[rhsLength] ? 0 : 1;
}

int bdeu_String::upperCaseCmp(const char *lhsString,
                              int         lhsLength,
                              const char *rhsString,
                              int         rhsLength)
{
    BSLS_ASSERT(lhsString);
    BSLS_ASSERT(0 <= lhsLength);
    BSLS_ASSERT(rhsString);
    BSLS_ASSERT(0 <= rhsLength);

    int min = lhsLength < rhsLength ? lhsLength : rhsLength;
    int i = 0;
    while (i < min) {
        unsigned char lhs =
                        bsl::toupper(static_cast<unsigned char>(lhsString[i]));
        unsigned char rhs =
                        bsl::toupper(static_cast<unsigned char>(rhsString[i]));
        if (lhs != rhs) {
            return lhs < rhs ? -1 : 1;                                // RETURN
        }
        ++i;
    }
    return lhsLength < rhsLength ? -1 : lhsLength == rhsLength ? 0 : 1;
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
