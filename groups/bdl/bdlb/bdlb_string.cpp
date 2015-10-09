// bdlb_string.cpp                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlb_string.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_string_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_cctype.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace bdlb {
                               // -------------
                               // struct String
                               // -------------

// CLASS METHODS
bool String::areEqualCaseless(const char *lhsString,
                              const char *rhsString)
{
    BSLS_ASSERT(lhsString);
    BSLS_ASSERT(rhsString);

    int i = 0;
    while (lhsString[i]) {
        unsigned char lhs = static_cast<unsigned char>(
                       bsl::tolower(static_cast<unsigned char>(lhsString[i])));
        unsigned char rhs = static_cast<unsigned char>(
                       bsl::tolower(static_cast<unsigned char>(rhsString[i])));
        if (lhs != rhs) {
            return false;                                             // RETURN
        }
        ++i;
    }
    return 0 == rhsString[i];
}

bool String::areEqualCaseless(const char *lhsString,
                              const char *rhsString,
                              int         rhsLength)
{
    BSLS_ASSERT(lhsString);
    BSLS_ASSERT(rhsString || 0 == rhsLength);
    BSLS_ASSERT(             0 <= rhsLength);

    for (int i = 0; i < rhsLength; ++i) {
        unsigned char lhs = static_cast<unsigned char>(
                       bsl::tolower(static_cast<unsigned char>(lhsString[i])));
        unsigned char rhs = static_cast<unsigned char>(
                       bsl::tolower(static_cast<unsigned char>(rhsString[i])));
        if (lhs != rhs || !lhs) {
            return false;                                             // RETURN
        }
    }
    return 0 == lhsString[rhsLength] ? true : false;
}

bool String::areEqualCaseless(const char *lhsString,
                              int         lhsLength,
                              const char *rhsString,
                              int         rhsLength)
{
    BSLS_ASSERT(lhsString || 0 == lhsLength);
    BSLS_ASSERT(             0 <= lhsLength);
    BSLS_ASSERT(rhsString || 0 == rhsLength);
    BSLS_ASSERT(             0 <= rhsLength);

    if (lhsLength != rhsLength) {
        return false;                                                 // RETURN
    }
    int i = 0;
    while (i < lhsLength) {
        unsigned char lhs = static_cast<unsigned char>(
                       bsl::tolower(static_cast<unsigned char>(lhsString[i])));
        unsigned char rhs = static_cast<unsigned char>(
                       bsl::tolower(static_cast<unsigned char>(rhsString[i])));
        if (lhs != rhs) {
            return false;                                             // RETURN
        }
        ++i;
    }
    return true;
}

char *String::copy(const char       *string,
                   int               length,
                   bslma::Allocator *basicAllocator)
{
    BSLS_ASSERT(string || 0 == length);
    BSLS_ASSERT(          0 <= length);
    BSLS_ASSERT(basicAllocator);

    char *newString =
                reinterpret_cast<char *>(basicAllocator->allocate(length + 1));
    if (string) {
        bsl::memcpy(newString, string, length);
    }
    newString[length] = 0;

    return newString;
}

int String::lowerCaseCmp(const char *lhsString, const char *rhsString)
{
    BSLS_ASSERT(lhsString);
    BSLS_ASSERT(rhsString);

    int i = 0;
    while (lhsString[i]) {
        unsigned char lhs = static_cast<unsigned char>(
                       bsl::tolower(static_cast<unsigned char>(lhsString[i])));
        unsigned char rhs = static_cast<unsigned char>(
                       bsl::tolower(static_cast<unsigned char>(rhsString[i])));
        if (lhs != rhs) {
            return lhs < rhs ? -1 : 1;                                // RETURN
        }
        ++i;
    }
    return 0 == rhsString[i] ? 0 : -1;
}

int String::lowerCaseCmp(const char *lhsString,
                         const char *rhsString,
                         int         rhsLength)
{
    BSLS_ASSERT(lhsString);
    BSLS_ASSERT(rhsString || 0 == rhsLength);
    BSLS_ASSERT(             0 <= rhsLength);

    for (int i = 0; i < rhsLength; ++i) {
        unsigned char lhs = static_cast<unsigned char>(
                       bsl::tolower(static_cast<unsigned char>(lhsString[i])));
        unsigned char rhs = static_cast<unsigned char>(
                       bsl::tolower(static_cast<unsigned char>(rhsString[i])));
        if (lhs != rhs || !lhs) {
            return lhs < rhs ? -1 : 1;                                // RETURN
        }
    }
    return 0 == lhsString[rhsLength] ? 0 : 1;
}

int String::lowerCaseCmp(const char *lhsString,
                         int         lhsLength,
                         const char *rhsString,
                         int         rhsLength)
{
    BSLS_ASSERT(lhsString || 0 == lhsLength);
    BSLS_ASSERT(             0 <= lhsLength);
    BSLS_ASSERT(rhsString || 0 == rhsLength);
    BSLS_ASSERT(             0 <= rhsLength);

    int min = lhsLength < rhsLength ? lhsLength : rhsLength;
    int i = 0;
    while (i < min) {
        unsigned char lhs = static_cast<unsigned char>(
                       bsl::tolower(static_cast<unsigned char>(lhsString[i])));
        unsigned char rhs = static_cast<unsigned char>(
                       bsl::tolower(static_cast<unsigned char>(rhsString[i])));
        if (lhs != rhs) {
            return lhs < rhs ? -1 : 1;                                // RETURN
        }
        ++i;
    }
    return lhsLength < rhsLength ? -1 : lhsLength == rhsLength ? 0 : 1;
}

void String::ltrim(char *string)
{
    BSLS_ASSERT(string);

    int index = 0;
    while (bsl::isspace(static_cast<unsigned char>(string[index]))) {
        ++index;
    }
    bsl::size_t len = bsl::strlen(string + index);
    bsl::memmove(string, &string[index], len + 1);
}

void String::ltrim(char *string, int *length)
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

void String::rtrim(char *string)
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

void String::rtrim(const char *string, int *length)
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

const char *String::strstr(const char *string,
                           int         stringLen,
                           const char *subString,
                           int         subStringLen)
{
    BSLS_ASSERT(string    || 0 == stringLen);
    BSLS_ASSERT(             0 <= stringLen);
    BSLS_ASSERT(subString || 0 == subStringLen);
    BSLS_ASSERT(             0 <= subStringLen);

    if (0 == subStringLen) {
        return string;                                                // RETURN
    }

    if (stringLen < subStringLen) {
        return 0;                                                     // RETURN
    }

    BSLS_ASSERT_SAFE(string);    // impossible to fail

    const char *end = string + stringLen - subStringLen;

    for (const char *p = string; p <= end; ++p) {
        if (0 == bsl::memcmp(p, subString, subStringLen)) {
            return p;                                                 // RETURN
        }
    }

    return 0;
}

const char *String::strstrCaseless(const char *string,
                                   int         stringLen,
                                   const char *subString,
                                   int         subStringLen)
{
    BSLS_ASSERT(string    || 0 == stringLen);
    BSLS_ASSERT(             0 <= stringLen);
    BSLS_ASSERT(subString || 0 == subStringLen);
    BSLS_ASSERT(             0 <= subStringLen);

    if (0 == subStringLen) {
        return string;                                                // RETURN
    }

    if (stringLen < subStringLen) {
        return 0;                                                     // RETURN
    }

    BSLS_ASSERT_SAFE(string);    // impossible to fail

    const char *end = string + stringLen - subStringLen;

    for (const char *p = string; p <= end; ++p) {
        if (areEqualCaseless(p, subStringLen, subString, subStringLen)) {
            return p;                                                 // RETURN
        }
    }

    return 0;
}

const char *String::strrstr(const char *string,
                            int         stringLen,
                            const char *subString,
                            int         subStringLen)
{
    BSLS_ASSERT(string    || 0 == stringLen);
    BSLS_ASSERT(             0 <= stringLen);
    BSLS_ASSERT(subString || 0 == subStringLen);
    BSLS_ASSERT(             0 <= subStringLen);

    if (0 == subStringLen) {
        return string + stringLen;                                    // RETURN
    }

    if (stringLen < subStringLen) {
        return 0;                                                     // RETURN
    }

    BSLS_ASSERT_SAFE(string);    // impossible to fail

    for (int i = stringLen - subStringLen; i >= 0; --i) {
        const char *p = string + i;

        if (0 == bsl::memcmp(p, subString, subStringLen)) {
            return p;                                                 // RETURN
        }
    }

    return 0;
}

const char *String::strrstrCaseless(const char *string,
                                    int         stringLen,
                                    const char *subString,
                                    int         subStringLen)
{
    BSLS_ASSERT(string    || 0 == stringLen);
    BSLS_ASSERT(             0 <= stringLen);
    BSLS_ASSERT(subString || 0 == subStringLen);
    BSLS_ASSERT(             0 <= subStringLen);

    if (0 == subStringLen) {
        return string + stringLen;                                    // RETURN
    }

    if (stringLen < subStringLen) {
        return 0;                                                     // RETURN
    }

    BSLS_ASSERT_SAFE(string);    // impossible to fail

    for (int i = stringLen - subStringLen; i >= 0; --i) {
        const char *p = string + i;

        if (areEqualCaseless(p, subStringLen, subString, subStringLen)) {
            return p;                                                 // RETURN
        }
    }

    return 0;
}

void String::toFixedLength(char       *dstString,
                           int         dstLength,
                           const char *srcString,
                           int         srcLength,
                           char        padChar)
{
    BSLS_ASSERT(dstString);
    BSLS_ASSERT(             0 <= dstLength);
    BSLS_ASSERT(srcString || 0 == srcLength);
    BSLS_ASSERT(             0 <= srcLength);

    if (dstLength < srcLength) {
        // We know 'srcLength > 0', therefore '0 != srcString'.

        if (dstString != srcString) {
            bsl::memmove(dstString, srcString, dstLength);
        }
    }
    else {
        if (srcString && dstString != srcString) {
            bsl::memmove(dstString, srcString, srcLength);
        }
        for (int i = srcLength; i < dstLength; ++i) {
            dstString[i] = padChar;
        }
    }
}

int String::strnlen(const char *string, int maximumLength)
{
    BSLS_ASSERT(string || 0 == maximumLength);
    BSLS_ASSERT(          0 <= maximumLength);

    int i = 0;
    while (i < maximumLength && string[i]) {
        ++i;
    }
    return i;
}

void String::toLower(char *string)
{
    BSLS_ASSERT(string);

    for (int i = 0; string[i]; ++i) {
        string[i] = static_cast<char>(
                          bsl::tolower(static_cast<unsigned char>(string[i])));
    }
}

void String::toLower(char *string, int length)
{
    BSLS_ASSERT(string || 0 == length);
    BSLS_ASSERT(          0 <= length);

    for (int i = 0; i < length; ++i) {
        string[i] = static_cast<char>(
                          bsl::tolower(static_cast<unsigned char>(string[i])));
    }
}

void String::toUpper(char *string)
{
    BSLS_ASSERT(string);

    for (int i = 0; string[i]; ++i) {
        string[i] = static_cast<char>(
                          bsl::toupper(static_cast<unsigned char>(string[i])));
    }
}

void String::toUpper(char *string, int length)
{
    BSLS_ASSERT(string || 0 == length);
    BSLS_ASSERT(          0 <= length);

    for (int i = 0; i < length; ++i) {
        string[i] = static_cast<char>(
                          bsl::toupper(static_cast<unsigned char>(string[i])));
    }
}

void String::trim(char *string)
{
    BSLS_ASSERT(string);

    // This is organized to minimize passes through the string.  One pass to
    // set 'start', 'end', and 'trailing':

    char *start = string;    // will point to after initial spaces
    while (bsl::isspace(static_cast<unsigned char>(*start))) {
        ++start;
    }

    char *end      = start;  // will traverse to trailing '\0'
    char *trailing = 0;      // will point to start of trailing spaces, or 0
                             // if there are none
    for (; *end; ++end) {
        if (bsl::isspace(static_cast<unsigned char>(*end))) {
            if (!trailing) {
                trailing = end;
            }
        }
        else {
            trailing = 0;
        }
    }

    // Then the second pass to change the string given that information,
    // moving it only if necessary.

    if (trailing) {
        *trailing = '\0';
    }

    if (start == string) {
        return;                                                       // RETURN
    }

    while ((*string++ = *start++)) {
        ;
    }
}

void String::skipLeadingTrailing(const char **begin, const char **end)
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

int String::upperCaseCmp(const char *lhsString, const char *rhsString)
{
    BSLS_ASSERT(lhsString);
    BSLS_ASSERT(rhsString);

    int i = 0;
    while (lhsString[i]) {
        unsigned char lhs = static_cast<unsigned char>(
                       bsl::toupper(static_cast<unsigned char>(lhsString[i])));
        unsigned char rhs = static_cast<unsigned char>(
                       bsl::toupper(static_cast<unsigned char>(rhsString[i])));
        if (lhs != rhs) {
            return lhs < rhs ? -1 : 1;                                // RETURN
        }
        ++i;
    }
    return 0 == rhsString[i] ? 0 : -1;
}

int String::upperCaseCmp(const char *lhsString,
                         const char *rhsString,
                         int         rhsLength)
{
    BSLS_ASSERT(lhsString);
    BSLS_ASSERT(rhsString || 0 == rhsLength);
    BSLS_ASSERT(             0 <= rhsLength);

    for (int i = 0; i < rhsLength; ++i) {
        unsigned char lhs = static_cast<unsigned char>(
                       bsl::toupper(static_cast<unsigned char>(lhsString[i])));
        unsigned char rhs = static_cast<unsigned char>(
                       bsl::toupper(static_cast<unsigned char>(rhsString[i])));
        if (lhs != rhs || !lhs) {
            return lhs < rhs ? -1 : 1;                                // RETURN
        }
    }
    return 0 == lhsString[rhsLength] ? 0 : 1;
}

int String::upperCaseCmp(const char *lhsString,
                         int         lhsLength,
                         const char *rhsString,
                         int         rhsLength)
{
    BSLS_ASSERT(lhsString || 0 == lhsLength);
    BSLS_ASSERT(             0 <= lhsLength);
    BSLS_ASSERT(rhsString || 0 == rhsLength);
    BSLS_ASSERT(             0 <= rhsLength);

    int min = lhsLength < rhsLength ? lhsLength : rhsLength;
    int i = 0;
    while (i < min) {
        unsigned char lhs = static_cast<unsigned char>(
                       bsl::toupper(static_cast<unsigned char>(lhsString[i])));
        unsigned char rhs = static_cast<unsigned char>(
                       bsl::toupper(static_cast<unsigned char>(rhsString[i])));
        if (lhs != rhs) {
            return lhs < rhs ? -1 : 1;                                // RETURN
        }
        ++i;
    }
    return lhsLength < rhsLength ? -1 : lhsLength == rhsLength ? 0 : 1;
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
