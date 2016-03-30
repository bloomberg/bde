// bdlb_stringrefutil.cpp                                             -*-C++-*-
#include <bdlb_stringrefutil.h>

#include <bsls_ident.h>

#include <bsl_c_string.h> // '::strncasecmp'
#include <bsl_cstring.h>  // 'bsl::memcmp'

namespace BloombergLP {
namespace bdlb {

static inline int u_upperToLower(int ch)
    // Return the lower case analog of the specified 'ch' if 'ch' is an upper
    // case character, and return 'ch' otherwise.  The sequence of characters
    // '[A .. Z]' is mapped to '[a .. z]', The behavior is undefined unless
    // characters are ASCII encoded.
{
    return 'A' <= ch && ch <= 'Z'
           ? ch |=  0x20  // upper -> lower
           : ch;
}

static inline int u_lowerToUpper(int ch)
    // Return the upper case analog of the specified 'ch' if 'ch' is a lower
    // case character, and return 'ch' otherwise.  The sequence of characters
    // '[a .. z]' is mapped to '[A .. Z]', The behavior is undefined unless
    // characters are ASCII encoded.
{
    return 'a' <= ch && ch <= 'z'
           ? ch &= ~0x20  // lower -> upper
           : ch;
}

static inline bool u_isspace(unsigned char ch)
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

int StringRefUtil::lowerCaseCmp(const bslstl::StringRef& lhsString,
                                const bslstl::StringRef& rhsString)
{
    const bsl::size_t lhsLength = lhsString.length();
    const bsl::size_t rhsLength = rhsString.length();
    const int         min       = lhsLength < rhsLength
                                ? lhsLength : rhsLength;

    for (int i = 0; i < min; ++i) {
        int lhs = u_upperToLower((static_cast<unsigned char>(lhsString[i])));
        int rhs = u_upperToLower((static_cast<unsigned char>(rhsString[i])));

        if (lhs != rhs) {
            return lhs < rhs ? -1 : 1;                                // RETURN
        }
    }
    return lhsLength <  rhsLength ? -1:
           lhsLength == rhsLength ?  0:
           /* else */                1;
}

int StringRefUtil::upperCaseCmp(const bslstl::StringRef& lhsString,
                                const bslstl::StringRef& rhsString)
{
    const bsl::size_t lhsLength = lhsString.length();
    const bsl::size_t rhsLength = rhsString.length();
    const bsl::size_t min       = lhsLength < rhsLength
                                ? lhsLength : rhsLength;

    for (bsl::size_t i = 0;  i < min; ++i) {
        int lhs = u_lowerToUpper((static_cast<unsigned char>(lhsString[i])));
        int rhs = u_lowerToUpper((static_cast<unsigned char>(rhsString[i])));

        if (lhs != rhs) {
            return lhs < rhs ? -1 : 1;                                // RETURN
        }
    }
    return lhsLength <  rhsLength ? -1:
           lhsLength == rhsLength ?  0:
           /* else */                1;
}

                        // Trim

void StringRefUtil::ltrim(bslstl::StringRef *string)
{
    BSLS_ASSERT_SAFE(string);

    bslstl::StringRef::const_iterator       itr = string->begin();
    bslstl::StringRef::const_iterator const end = string->end();

    while (end != itr && u_isspace(static_cast<unsigned char>(*itr))) {
        ++itr;
    }
    string->assign(itr, end);
}

void StringRefUtil::rtrim(bslstl::StringRef *string)
{
    const bsl::size_t length = string->length();

    if (length) {
        int index = length - 1;
        while(   0 <= index
              && u_isspace(static_cast<unsigned char>((*string)[index])))
        {
            --index;
        }
        string->assign(string->data(), index + 1);
    }
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

    BSLS_ASSERT(string.data()); BSLS_ASSERT(0 < string.length());
    BSLS_ASSERT(subStr.data()); BSLS_ASSERT(0 < subStr.length());

    const char * const end = string.end() - subStrLength + 1;
    BSLS_ASSERT(string.data() <= end);
    BSLS_ASSERT(                 end  <= string.end());

    for (const char *cur = string.data(); cur < end; ++cur) {
        if (0 == bsl::memcmp(cur, subStr.data(), subStrLength)) {
            return bslstl::StringRef(cur, subStrLength);              // RETURN
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

    BSLS_ASSERT(string.data()); BSLS_ASSERT(0 < string.length());
    BSLS_ASSERT(subStr.data()); BSLS_ASSERT(0 < subStr.length());

    const char * const end = string.end() - subStrLength + 1;
    BSLS_ASSERT(string.data() <= end);
    BSLS_ASSERT(                 end  <= string.end());

    for (const char *cur = string.data(); cur < end; ++cur) {
        if (0 == strncasecmp(cur, subStr.data(), subStrLength)) {
            return bslstl::StringRef(cur, subStrLength);              // RETURN
        }
    }

    return u_NOT_FOUND;
}

bslstl::StringRef StringRefUtil::strrstr(const bslstl::StringRef& string,
                                         const bslstl::StringRef& subStr)
{
    const bsl::size_t subStrLength = subStr.length();

    if (0 == subStrLength) {
        return bslstl::StringRef(string.data(), 0);                   // RETURN
    }

    if (string.length() < subStrLength) {
        return u_NOT_FOUND;                                           // RETURN
    }

    BSLS_ASSERT(string.data()); BSLS_ASSERT(0 < string.length());
    BSLS_ASSERT(subStr.data()); BSLS_ASSERT(0 < subStr.length());

    const bsl::size_t  count = string.length() - subStrLength + 1;
    const char        *cur   = string.end()    - subStrLength;
    BSLS_ASSERT(string.data() <= cur);
    BSLS_ASSERT(                 cur  < string.end());

    for (bsl::size_t i = 0; i < count; ++i, --cur) {
        BSLS_ASSERT(string.begin() <= cur);

        if (0 == bsl::memcmp(cur, subStr.data(), subStrLength)) {
            return bslstl::StringRef(cur, subStrLength);              // RETURN
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
        return bslstl::StringRef(string.data(), 0);                   // RETURN
    }

    if (string.length() < subStrLength) {
        return u_NOT_FOUND;                                           // RETURN
    }

    BSLS_ASSERT(string.data()); BSLS_ASSERT(0 < string.length());
    BSLS_ASSERT(subStr.data()); BSLS_ASSERT(0 < subStr.length());

    const bsl::size_t  count = string.length() - subStrLength + 1;
    const char        *cur   = string.end()    - subStrLength;
    BSLS_ASSERT(string.data() <= cur);
    BSLS_ASSERT(                 cur  < string.end());

    for (bsl::size_t i = 0; i < count; ++i, --cur) {
        BSLS_ASSERT(string.begin() <= cur);

        if (0 == strncasecmp(cur, subStr.data(), subStrLength)) {
            return bslstl::StringRef(cur, subStrLength);              // RETURN
        }
    }

    return u_NOT_FOUND;
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
