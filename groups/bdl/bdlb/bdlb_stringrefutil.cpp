// bdlb_stringrefutil.cpp                                             -*-C++-*-
#include <bdlb_stringrefutil.h>

#include <bsls_ident.h>

#include <bsl_algorithm.h>
#include <bsl_cstring.h>  // 'bsl::memcmp'

namespace BloombergLP {
namespace bdlb {

static inline int u_upperToLower(int ch)
    // Return the lower case analog of the specified 'ch' if 'ch' is an upper
    // case character, and return 'ch' otherwise; the sequence of characters
    // '[A .. Z]' is mapped to '[a .. z]'.  The behavior is undefined unless
    // characters are ASCII encoded.
{
    return 'A' <= ch && ch <= 'Z'
           ? ch |=  0x20  // upper -> lower
           : ch;
}

static inline int u_lowerToUpper(int ch)
    // Return the upper case analog of the specified 'ch' if 'ch' is a lower
    // case character, and return 'ch' otherwise; the sequence of characters
    // '[a .. z]' is mapped to '[A .. Z]'.  The behavior is undefined unless
    // characters are ASCII encoded.
{
    return 'a' <= ch && ch <= 'z'
           ? ch &= ~0x20  // lower -> upper
           : ch;
}

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


static const bslstl::StringRef u_NOT_FOUND;

                        // --------------------
                        // struct StringRefUtil
                        // --------------------

// CLASS METHODS
                        // Comparison

int StringRefUtil::lowerCaseCmp(const bslstl::StringRef& lhs,
                                const bslstl::StringRef& rhs)
{
    const bsl::size_t lhsLength = lhs.length();
    const bsl::size_t rhsLength = rhs.length();
    const bsl::size_t min       = lhsLength < rhsLength
                                ? lhsLength : rhsLength;

    for (bsl::size_t i = 0; i < min; ++i) {
        char lhsChar  = *(lhs.data() + i);
        char rhsChar  = *(rhs.data() + i);
        int  lhsUpper = u_upperToLower(static_cast<unsigned char>(lhsChar));
        int  rhsUpper = u_upperToLower(static_cast<unsigned char>(rhsChar));

        if (lhsUpper != rhsUpper) {
            return lhsUpper < rhsUpper ? -1 : 1;                      // RETURN
        }
    }
    return lhsLength <  rhsLength ? -1:
           lhsLength == rhsLength ?  0:
           /* else */                1;
}

int StringRefUtil::upperCaseCmp(const bslstl::StringRef& lhs,
                                const bslstl::StringRef& rhs)
{
    const bsl::size_t lhsLength = lhs.length();
    const bsl::size_t rhsLength = rhs.length();
    const bsl::size_t min       = lhsLength < rhsLength
                                ? lhsLength : rhsLength;

    for (bsl::size_t i = 0; i < min; ++i) {
        char lhsChar  = *(lhs.data() + i);
        char rhsChar  = *(rhs.data() + i);
        int  lhsUpper = u_lowerToUpper(static_cast<unsigned char>(lhsChar));
        int  rhsUpper = u_lowerToUpper(static_cast<unsigned char>(rhsChar));

        if (lhsUpper != rhsUpper) {
            return lhsUpper < rhsUpper ? -1 : 1;                      // RETURN
        }
    }

    return lhsLength <  rhsLength ? -1:
           lhsLength == rhsLength ?  0:
           /* else */                1;
}

                        // Trim

bslstl::StringRef StringRefUtil::ltrim(const bslstl::StringRef& string)
{
    bslstl::StringRef::const_iterator       itr = string.begin();
    bslstl::StringRef::const_iterator const end = string.end();

    while (end != itr && u_isWhitespace(static_cast<unsigned char>(*itr))) {
        ++itr;
    }

    return bslstl::StringRef(itr, end);
}

bslstl::StringRef StringRefUtil::rtrim(const bslstl::StringRef& string)
{

    int index = static_cast<int>(string.length()) - 1;
    while(   0 <= index
          && u_isWhitespace(static_cast<unsigned char>(string[index])))
    {
        --index;
    }

    return bslstl::StringRef(string.data(), index + 1);
}

                        // Find 'subString'

bslstl::StringRef StringRefUtil::strstr(const bslstl::StringRef& string,
                                        const bslstl::StringRef& subStr)
{
    const bsl::size_t subStrLength = subStr.length();

    if (0 == subStrLength) {
        return bslstl::StringRef(string.data(), 0);                   // RETURN
    }

    if (string.length() < subStrLength) {
        return u_NOT_FOUND;                                           // RETURN
    }

    const char * const end = string.end() - subStrLength + 1;

    for (const char *cur = string.data(); cur < end; ++cur) {
        if (0 == bsl::memcmp(cur, subStr.data(), subStrLength)) {
            return bslstl::StringRef(cur, cur + subStrLength);        // RETURN
        }
    }

    return u_NOT_FOUND;
}

bslstl::StringRef StringRefUtil::strstrCaseless(
                                               const bslstl::StringRef& string,
                                               const bslstl::StringRef& subStr)
{
    const bsl::size_t subStrLength = subStr.length();

    if (0 == subStrLength) {
        return bslstl::StringRef(string.data(), 0);                   // RETURN
    }

    if (string.length() < subStrLength) {
        return u_NOT_FOUND;                                           // RETURN
    }

    const char * const end = string.end() - subStrLength + 1;

    for (const char *cur = string.data(); cur < end; ++cur) {
         if (0 == lowerCaseCmp(
             bslstl::StringRef(cur,           cur           + subStrLength),
             bslstl::StringRef(subStr.data(), subStr.data() + subStrLength))) {
            return bslstl::StringRef(cur, cur + subStrLength);        // RETURN
        }
    }

    return u_NOT_FOUND;
}

bslstl::StringRef StringRefUtil::strrstr(const bslstl::StringRef& string,
                                         const bslstl::StringRef& subStr)
{
    const bsl::size_t subStrLength = subStr.length();

    if (0 == subStrLength) {
        return bslstl::StringRef(string.end(), 0);                    // RETURN
    }

    if (string.length() < subStrLength) {
        return u_NOT_FOUND;                                           // RETURN
    }

    const bsl::size_t  count = string.length() - subStrLength + 1;
    const char        *cur   = string.end()    - subStrLength;

    for (bsl::size_t i = 0; i < count; ++i, --cur) {
        if (0 == bsl::memcmp(cur, subStr.data(), subStrLength)) {
            return bslstl::StringRef(cur, cur + subStrLength);        // RETURN
        }
    }

    return u_NOT_FOUND;
}

bslstl::StringRef StringRefUtil::strrstrCaseless(
                                               const bslstl::StringRef& string,
                                               const bslstl::StringRef& subStr)
{
    const bsl::size_t subStrLength = subStr.length();

    if (0 == subStrLength) {
        return bslstl::StringRef(string.end(), 0);                    // RETURN
    }

    if (string.length() < subStrLength) {
        return u_NOT_FOUND;                                           // RETURN
    }

    const bsl::size_t  count = string.length() - subStrLength + 1;
    const char        *cur   = string.end()    - subStrLength;

    for (bsl::size_t i = 0; i < count; ++i, --cur) {
        if (0 == lowerCaseCmp(
             bslstl::StringRef(cur,           cur           + subStrLength),
             bslstl::StringRef(subStr.data(), subStr.data() + subStrLength))) {
            return bslstl::StringRef(cur, cur + subStrLength);        // RETURN
        }
    }

    return u_NOT_FOUND;
}

StringRefUtil::size_type
StringRefUtil::findFirstOf(const bslstl::StringRef& string,
                           const bslstl::StringRef& characters,
                           size_type                position)
{
    // This algorithm intentionally mimics the corresponding algorithm in
    // 'bsl::basic_string'.

    if (!characters.isEmpty() && position < string.length()) {
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

StringRefUtil::size_type
StringRefUtil::findLastOf(const bslstl::StringRef& string,
                          const bslstl::StringRef& characters,
                          size_type                position)
{
    // This algorithm intentionally mimics the corresponding algorithm in
    // 'bsl::basic_string'.

    if (!characters.isEmpty() && !string.isEmpty()) {
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

StringRefUtil::size_type
StringRefUtil::findFirstNotOf(const bslstl::StringRef& string,
                              const bslstl::StringRef& characters,
                              size_type                position)
{
    // This algorithm intentionally mimics the corresponding algorithm in
    // 'bsl::basic_string'.

    if (!characters.isEmpty() && position < string.length()) {
        for (const char *current = string.data() + position;
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

StringRefUtil::size_type
StringRefUtil::findLastNotOf(const bslstl::StringRef& string,
                             const bslstl::StringRef& characters,
                             size_type                position)
{
    // This algorithm intentionally mimics the corresponding algorithm in
    // 'bsl::basic_string'.5

    if (!characters.isEmpty() && !string.isEmpty()) {
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
// Copyright 2016 Bloomberg Finance L.P.
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
