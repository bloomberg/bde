// bdlb_stringviewutil.cpp                                            -*-C++-*-
#include <bdlb_stringviewutil.h>

#include <bdlb_chartype.h>

#include <bsl_algorithm.h>
#include <bsl_cstring.h>  // 'bsl::memcmp'

namespace BloombergLP {
namespace bdlb {

static inline bool u_isWhitespace(unsigned char ch)
    // Return 'true' is the specified 'ch' is one of the ASCII whitespace
    // characters in the "C" and "POSIX" locales, and 'false' otherwise.
{
    if (' ' == ch) {
        return true;                                                  // RETURN
    }

    if (ch < '\t' || '\r' < ch ) {
        return false;                                                 // RETURN
    }

    return true;
}

static const bsl::string_view u_NOT_FOUND;

                        // ---------------------
                        // struct StringViewUtil
                        // ---------------------

// CLASS METHODS
                        // Comparison

int StringViewUtil::lowerCaseCmp(const bsl::string_view& lhs,
                                 const bsl::string_view& rhs)
{
    const bsl::size_t lhsLength = lhs.length();
    const bsl::size_t rhsLength = rhs.length();
    const bsl::size_t min       = lhsLength < rhsLength
                                ? lhsLength : rhsLength;

    for (bsl::size_t i = 0; i < min; ++i) {
        // Standard library comparison functions do comparisons between
        // unsigned 'char' values.

        const unsigned char lhsLower = CharType::toLower(*(lhs.data() + i));
        const unsigned char rhsLower = CharType::toLower(*(rhs.data() + i));

        if (lhsLower != rhsLower) {
            return lhsLower < rhsLower ? -1 : 1;                      // RETURN
        }
    }
    return lhsLength <  rhsLength ? -1:
           lhsLength == rhsLength ?  0:
           /* else */                1;
}

int StringViewUtil::upperCaseCmp(const bsl::string_view& lhs,
                                 const bsl::string_view& rhs)
{
    const bsl::size_t lhsLength = lhs.length();
    const bsl::size_t rhsLength = rhs.length();
    const bsl::size_t min       = lhsLength < rhsLength
                                ? lhsLength : rhsLength;

    for (bsl::size_t i = 0; i < min; ++i) {
        // Standard library comparison functions do comparisons between
        // unsigned 'char' values.

        const unsigned char lhsUpper = CharType::toUpper(*(lhs.data() + i));
        const unsigned char rhsUpper = CharType::toUpper(*(rhs.data() + i));

        if (lhsUpper != rhsUpper) {
            return lhsUpper < rhsUpper ? -1 : 1;                      // RETURN
        }
    }

    return lhsLength <  rhsLength ? -1:
           lhsLength == rhsLength ?  0:
           /* else */                1;
}

                        // Trim

bsl::string_view StringViewUtil::ltrim(const bsl::string_view& string)
{
    bsl::string_view::size_type       index  = 0;
    const bsl::string_view::size_type length = string.length();

    while (index < length &&
           u_isWhitespace(static_cast<unsigned char>(string[index]))) {
        ++index;
    }

    return bsl::string_view(string.data() + index, length - index);
}

bsl::string_view StringViewUtil::rtrim(const bsl::string_view& string)
{
    const char *begin = string.data();
    const char *end   = begin + string.length();

    while (begin < end &&
           u_isWhitespace(static_cast<unsigned char>(*(end - 1)))) {
        --end;
    }

    return bsl::string_view(
                        begin,
                        static_cast<bsl::string_view::size_type>(end - begin));
}

                        // Find 'subString'

bsl::string_view StringViewUtil::strstr(const bsl::string_view& string,
                                        const bsl::string_view& subStr)
{
    const bsl::size_t subStrLength = subStr.length();

    if (0 == subStrLength) {
        return bsl::string_view(string.data(), 0);                    // RETURN
    }

    if (string.length() < subStrLength) {
        return u_NOT_FOUND;                                           // RETURN
    }

    const char * const end =
                          string.data() + (string.length() - subStrLength + 1);

    for (const char *cur = string.data(); cur < end; ++cur) {
        if (0 == bsl::memcmp(cur, subStr.data(), subStrLength)) {
            return bsl::string_view(cur, subStrLength);               // RETURN
        }
    }

    return u_NOT_FOUND;
}

bsl::string_view StringViewUtil::strstrCaseless(const bsl::string_view& string,
                                                const bsl::string_view& subStr)
{
    const bsl::size_t subStrLength = subStr.length();

    if (0 == subStrLength) {
        return bsl::string_view(string.data(), 0);                    // RETURN
    }

    if (string.length() < subStrLength) {
        return u_NOT_FOUND;                                           // RETURN
    }

    const char * const end =
                          string.data() + (string.length() - subStrLength + 1);

    for (const char *cur = string.data(); cur < end; ++cur) {
        if (0 == lowerCaseCmp(
                              bsl::string_view(cur,           subStrLength),
                              bsl::string_view(subStr.data(), subStrLength))) {
            return bsl::string_view(cur, subStrLength);               // RETURN
        }
    }

    return u_NOT_FOUND;
}

bsl::string_view StringViewUtil::strrstr(const bsl::string_view& string,
                                         const bsl::string_view& subStr)
{
    const bsl::size_t strLength    = string.length();
    const bsl::size_t subStrLength = subStr.length();

    if (0 == subStrLength) {
        return bsl::string_view(string.data() + strLength, 0);        // RETURN
    }

    if (string.length() < subStrLength) {
        return u_NOT_FOUND;                                           // RETURN
    }

    const bsl::size_t  count = strLength - subStrLength + 1;
    const char        *cur   = string.data() + strLength - subStrLength;

    for (bsl::size_t i = 0; i < count; ++i, --cur) {
        if (0 == bsl::memcmp(cur, subStr.data(), subStrLength)) {
            return bsl::string_view(cur, subStrLength);               // RETURN
        }
    }

    return u_NOT_FOUND;
}

bsl::string_view StringViewUtil::strrstrCaseless(
                                                const bsl::string_view& string,
                                                const bsl::string_view& subStr)
{
    const bsl::size_t strLength    = string.length();
    const bsl::size_t subStrLength = subStr.length();

    if (0 == subStrLength) {
        return bsl::string_view(string.data() + strLength, 0);        // RETURN
    }

    if (string.length() < subStrLength) {
        return u_NOT_FOUND;                                           // RETURN
    }

    const bsl::size_t  count = strLength - subStrLength + 1;
    const char        *cur   = string.data() + strLength - subStrLength;

    for (bsl::size_t i = 0; i < count; ++i, --cur) {
        if (0 == lowerCaseCmp(
                              bsl::string_view(cur,           subStrLength),
                              bsl::string_view(subStr.data(), subStrLength))) {
            return bsl::string_view(cur, subStrLength);               // RETURN
        }
    }

    return u_NOT_FOUND;
}

StringViewUtil::size_type StringViewUtil::findFirstOf(
                                            const bsl::string_view& string,
                                            const bsl::string_view& characters,
                                            size_type               position)
{
    // This algorithm intentionally mimics the corresponding algorithm in
    // 'bsl::basic_string'.

    if (!characters.empty() && position < string.length()) {
        for (const char *current = string.data() + position;
                         current != string.data() + string.length();
                       ++current)
        {
            if (bsl::find(characters.begin(), characters.end(), *current)
                                                         != characters.end()) {
                return current - string.data();                       // RETURN
            }
        }
    }
    return k_NPOS;
}

StringViewUtil::size_type StringViewUtil::findLastOf(
                                            const bsl::string_view& string,
                                            const bsl::string_view& characters,
                                            size_type               position)
{
    // This algorithm intentionally mimics the corresponding algorithm in
    // 'bsl::basic_string'.

    if (!characters.empty() && !string.empty()) {
        const size_type remChars = position < string.length() ?
                                                        position :
                                                        string.length() - 1;
        for (const char *current = string.data() + remChars;; --current) {
            if (bsl::find(characters.begin(), characters.end(), *current)
                                                         != characters.end()) {
                return current - string.data();                       // RETURN
            }
            if (current == string.data()) {
                break;
            }
        }
    }
    return k_NPOS;
}

StringViewUtil::size_type StringViewUtil::findFirstNotOf(
                                            const bsl::string_view& string,
                                            const bsl::string_view& characters,
                                            size_type               position)
{
    // This algorithm intentionally mimics the corresponding algorithm in
    // 'bsl::basic_string'.

    if (!characters.empty() && position < string.length()) {
        for (const char *current  = string.data() + position;
                         current != string.data() + string.length();
                       ++current)
        {
            if (bsl::find(characters.begin(), characters.end(), *current)
                                                         == characters.end()) {
                return current - string.data();                       // RETURN
            }
        }
    }
    return k_NPOS;
}

StringViewUtil::size_type StringViewUtil::findLastNotOf(
                                            const bsl::string_view& string,
                                            const bsl::string_view& characters,
                                            size_type               position)
{
    // This algorithm intentionally mimics the corresponding algorithm in
    // 'bsl::basic_string'.5

    if (!characters.empty() && !string.empty()) {
        const size_type remChars = position < string.length() ?
                                                           position :
                                                           string.length() - 1;
        for (const char *current = string.data() + remChars;;--current) {
            if (bsl::find(characters.begin(), characters.end(), *current)
                                                         == characters.end()) {
                return current - string.data();                       // RETURN
            }
            if (current == string.data()) {
                break;
            }
        }
    }
    return k_NPOS;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
// ----------------------------- END-OF-FILE ----------------------------------
