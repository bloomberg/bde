// bdlb_string.h                                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLB_STRING
#define INCLUDED_BDLB_STRING

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utility functions on C-style and 'STL' strings.
//
//@CLASSES:
//  bdlb::String: namespace for functions on C-style and 'STL' strings
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a utility 'struct', 'bdlb::String', that
// provides a suite of functions that operate on C-style strings and
// 'bsl::string'.  Note that the functions in this component will accept 0 as
// an *input* string, provided a length of 0 is supplied; however, 0 may not be
// be supplied for an *output* string parameter or for parameters expecting a
// 0-terminated string.
//
///Synopsis
///--------
// This component provides several kinds of utilities, including case-modified
// comparisons, case conversions, trimming functions, and length-related
// operations.  Most of the methods are overloaded to work on three styles of
// string: (1) a null-terminated C-style string, (2) a non-null-terminated
// 'const char *' with an 'int' length specifier, and (3) 'bsl::string'.  These
// overloaded methods are indicated schematically with the pseudo-argument
// 'STR', that represents any of the three possible string styles.  Note that
// the pseudo-signature '(STR, STR)' represents nine overloaded signatures, not
// three signatures.
//..
//  Method                        Purpose
//  --------------------------    --------------------------------------------
//  areEqualCaseless(STR, STR)    case-insensitive equality comparison
//  lowerCaseCmp(STR, STR)        lexical comparison of lower-case conversion
//  upperCaseCmp(STR, STR)        lexical comparison of upper-case conversion
//
//  ltrim(STR)                    remove whitespace from left side of string
//  rtrim(STR)                    remove whitespace from right side of string
//  trim(STR)                     remove whitespace from both sides of string
//
//  toLower(STR)                  convert to lower case
//  toUpper(STR)                  convert to upper case
//
//  strnlen(const char *, int)    null-terminated length, but bounded by 'int'
//  toFixedLength(...)            fixed-length copy with padding character
//  pad(...)                      append padding char.  up to specified length
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

namespace BloombergLP {
namespace bdlb {
                               // =============
                               // struct String
                               // =============

struct String {
    // This 'struct' provides a namespace for a suite of functions on C-style
    // strings and 'bsl::string'.

    // CLASS METHODS
    static bool areEqualCaseless(const char *lhsString, const char *rhsString);
    static bool areEqualCaseless(const char *lhsString,
                                 const char *rhsString,
                                 int         rhsLength);
    static bool areEqualCaseless(const char         *lhsString,
                                 const bsl::string&  rhsString);
    static bool areEqualCaseless(const char *lhsString,
                                 int         lhsLength,
                                 const char *rhsString);
    static bool areEqualCaseless(const char *lhsString,
                                 int         lhsLength,
                                 const char *rhsString,
                                 int         rhsLength);
    static bool areEqualCaseless(const char         *lhsString,
                                 int                 lhsLength,
                                 const bsl::string&  rhsString);
    static bool areEqualCaseless(const bsl::string&  lhsString,
                                 const char         *rhsString);
    static bool areEqualCaseless(const bsl::string&  lhsString,
                                 const char         *rhsString,
                                 int                 rhsLength);
    static bool areEqualCaseless(const bsl::string& lhsString,
                                 const bsl::string& rhsString);
        // Compare the specified 'lhsString' and 'rhsString', of the optionally
        // specified 'lhsLength' and 'rhsLength', respectively, indicating that
        // the corresponding C-style string is non-null-terminated.  Return
        // 'true' if 'lhsString' and 'rhsString' are equal up to a case
        // conversion, and 'false' otherwise.

    static char *copy(const char *string, bslma::Allocator *basicAllocator);
        // Create a null-terminated copy of the specified 'string', using the
        // specified 'basicAllocator' to supply memory, and return the address
        // of the newly-created modifiable string.  The string that is returned
        // is owned by the caller.  The behavior is undefined unless
        // 'basicAllocator' is non-null.

    static char *copy(const char       *string,
                      int               length,
                      bslma::Allocator *basicAllocator);
        // Create a null-terminated copy of the specified 'string' having the
        // specified 'length' (in bytes), using the specified 'basicAllocator'
        // to supply memory, and return the address of the newly-created
        // modifiable string.  The string that is returned is owned by the
        // caller.  The behavior is undefined unless '0 <= length' and
        // 'basicAllocator' is non-null.  Note that 'string' need not be
        // null-terminated and may contain null ('\0') characters.

    static char *copy(const bsl::string&  string,
                      bslma::Allocator   *basicAllocator);
        // Create a null-terminated copy of the specified 'string', using the
        // specified 'basicAllocator' to supply memory, and return the address
        // of the newly-created modifiable string.  The string that is returned
        // is owned by the caller.  The behavior is undefined unless
        // 'basicAllocator' is non-null.  Note that 'string' may contain null
        // ('\0') characters.

    static int lowerCaseCmp(const char *lhsString, const char *rhsString);
    static int lowerCaseCmp(const char *lhsString,
                            const char *rhsString,
                            int         rhsLength);
    static int lowerCaseCmp(const char         *lhsString,
                            const bsl::string&  rhsString);

    static int lowerCaseCmp(const char *lhsString,
                            int         lhsLength,
                            const char *rhsString);
    static int lowerCaseCmp(const char *lhsString,
                            int         lhsLength,
                            const char *rhsString,
                            int         rhsLength);
    static int lowerCaseCmp(const char         *lhsString,
                            int                 lhsLength,
                            const bsl::string&  rhsString);

    static int lowerCaseCmp(const bsl::string&  lhsString,
                            const char         *rhsString);
    static int lowerCaseCmp(const bsl::string&  lhsString,
                            const char         *rhsString,
                            int                 rhsLength);
    static int lowerCaseCmp(const bsl::string& lhsString,
                            const bsl::string& rhsString);
        // Compare the specified 'lhsString' and 'rhsString', of the optionally
        // specified 'lhsLength' and 'rhsLength', respectively, indicating that
        // the corresponding C-style string is non-null-terminated.  Return 1
        // if, after a conversion to lower case, 'lhsString' is lexically
        // greater than 'rhsString', 0 if are equal up to a case conversion,
        // and -1 otherwise.

    static void ltrim(char *string);
    static void ltrim(char *string, int *length);
    static void ltrim(bsl::string *string);
        // Remove from the specified 'string' (of the optionally specified
        // 'length') all leading whitespace to the left of the first
        // non-whitespace character in 'string'.  If 'string' has no
        // non-whitespace characters, 'string' will be empty (i.e., have zero
        // length) after this operation.

    static void pad(bsl::string *string, int numChars, char padChar = ' ');
        // Append repeatedly to the specified 'string' the optionally specified
        // 'padChar' until the length of 'string' is 'numChars'.  This
        // operation has no effect if 'string.size() >= numChars'.

    static void rtrim(char *string);
    static void rtrim(bsl::string *string);
        // Remove from the specified 'string' all trailing whitespace to the
        // right of the last non-whitespace character in 'string'.  If 'string'
        // has no non-whitespace characters, 'string' will be empty (i.e., have
        // zero length) after this operation.

    static void rtrim(const char *string, int *length);
        // Determine in the specified 'string' having the specified 'length'
        // the number of characters from the left-most character to the last
        // non-whitespace character.  Load the results into '*length'.  Note
        // that 'length' is both an input and output parameter.  Also note that
        // this method does not modify the contents of 'string'.

    static const char *strstr(const char *string,
                              int         stringLen,
                              const char *subString,
                              int         subStringLen);
        // Return a pointer offering non-modifiable access to the first
        // position in the specified 'string' of 'stringLen' characters at
        // which the specified 'subString' of 'subStringLen' characters is
        // found, or 0 if there is no such position.  If 'subStringLen' is 0,
        // 'subString' may be null and 'string' is returned.

    static const char *strstrCaseless(const char *string,
                                      int         stringLen,
                                      const char *subString,
                                      int         subStringLen);
        // Return a pointer offering non-modifiable access to the first
        // position in the specified 'string' of 'stringLen' characters at
        // which the specified 'subString' of 'subStringLen' characters is
        // found using case-insensitive comparison, or 0 if there is no such
        // position.  If 'subStringLen' is 0, 'subString' may be null and
        // 'string' is returned.

    static const char *strrstr(const char *string,
                               int         stringLen,
                               const char *subString,
                               int         subStringLen);
        // Return a pointer offering non-modifiable access to the last position
        // in the specified 'string' of 'stringLen' characters at which the
        // specified 'subString' of 'subStringLen' characters is found, or 0 if
        // there is no such position.  If 'subStringLen' is 0, 'subString' may
        // be null and 'string + stringLen' is returned.

    static const char *strrstrCaseless(const char *string,
                                       int         stringLen,
                                       const char *subString,
                                       int         subStringLen);
        // Return a pointer offering non-modifiable access to the last position
        // in the specified 'string' of 'stringLen' characters at which the
        // specified 'subString' of 'subStringLen' characters is found using
        // case-insensitive comparison, or 0 if there is no such position.  If
        // 'subStringLen' is 0, 'subString' may be null and
        // 'string + stringLen' is returned.

    static int strnlen(const char *string, int maximumLength);
        // Return the minimum of the length of the specified null-terminated
        // 'string' and the specified 'maximumLength'.

    static void toFixedLength(char       *dstString,
                              int         dstLength,
                              const char *srcString,
                              int         srcLength,
                              char        padChar = ' ');
        // Copy into the specified 'dstString' at most the specified first
        // 'dstLength' characters from the specified 'srcString' of the
        // specified 'srcLength'.  If 'srcLength < dstLength', repeatedly
        // append to 'dstString' the optionally specified 'padChar' until the
        // total number of characters written to 'dstString' is 'dstLength'.

    static void toLower(char *string);
    static void toLower(char *string, int length);
    static void toLower(bsl::string *string);
        // Replace all upper case characters in the specified 'string' (of the
        // optionally specified 'length') with their lowercase equivalent.

    static void toUpper(char *string);
    static void toUpper(char *string, int length);
    static void toUpper(bsl::string *string);
        // Replace all lower case characters in the specified 'string' (of the
        // optionally specified 'length') with their uppercase equivalent.

    static void trim(char *string);
    static void trim(char *string, int *length);
    static void trim(bsl::string *string);
        // Remove from the specified 'string' (of the optionally specified
        // 'length') all leading and trailing whitespace characters.  If
        // 'string' has no non-whitespace characters, 'string' will be empty
        // (i.e., have zero length) after this operation.

    static void skipLeadingTrailing(const char **begin, const char **end);
        // Skip leading and trailing whitespace characters in the string
        // indicated by the specified '*begin' and '*end' iterators by
        // appropriately advancing '*begin' and regressing '*end'.  If the
        // string indicated by '*begin' and '*end' is empty, or consists solely
        // of whitespace characters, '*begin' is unchanged and '*end' is
        // regressed to '*begin'.  Otherwise, advance '*begin' to the first
        // non-whitespace character whose position is greater than or equal to
        // '*begin'; regress '*end' to one past the position of the last
        // non-whitespace character whose position is less than '*end'.  The
        // behavior is undefined unless '*begin <= *end'.  Note that since
        // '*begin' and '*end' are iterators, '*end' refers the character one
        // past the end of the subject string.

    static int upperCaseCmp(const char *lhsString, const char *rhsString);
    static int upperCaseCmp(const char *lhsString,
                            const char *rhsString,
                            int         rhsLength);
    static int upperCaseCmp(const char         *lhsString,
                            const bsl::string&  rhsString);

    static int upperCaseCmp(const char *lhsString,
                            int         lhsLength,
                            const char *rhsString);
    static int upperCaseCmp(const char *lhsString,
                            int         lhsLength,
                            const char *rhsString,
                            int         rhsLength);
    static int upperCaseCmp(const char         *lhsString,
                            int                 lhsLength,
                            const bsl::string&  rhsString);

    static int upperCaseCmp(const bsl::string&  lhsString,
                            const char         *rhsString);
    static int upperCaseCmp(const bsl::string&  lhsString,
                            const char         *rhsString,
                            int                 rhsLength);
    static int upperCaseCmp(const bsl::string& lhsString,
                            const bsl::string& rhsString);
        // Compare the specified 'lhsString' and 'rhsString', of the optionally
        // specified 'lhsLength' and 'rhsLength', respectively, indicating that
        // the corresponding C-style string is non-null-terminated.  Return 1
        // if, after a conversion to upper case, 'lhsString' is lexically
        // greater than 'rhsString', 0 if, after a conversion to upper case,
        // the two strings are equal, and -1 otherwise.
};

// ============================================================================
//                        INLINE DEFINITIONS
// ============================================================================

                               // -------------
                               // struct String
                               // -------------

// CLASS METHODS
inline
bool String::areEqualCaseless(const char         *lhsString,
                              const bsl::string&  rhsString)
{
    BSLS_ASSERT_SAFE(lhsString);

    return areEqualCaseless(lhsString,
                            &rhsString[0],
                            static_cast<int>(rhsString.size()));
}

inline
bool String::areEqualCaseless(const char *lhsString,
                              int         lhsLength,
                              const char *rhsString)
{
    BSLS_ASSERT_SAFE(lhsString || 0 == lhsLength);
    BSLS_ASSERT_SAFE(0 <= lhsLength);
    BSLS_ASSERT_SAFE(rhsString);

    return areEqualCaseless(rhsString, lhsString, lhsLength);
}

inline
bool String::areEqualCaseless(const char         *lhsString,
                              int                 lhsLength,
                              const bsl::string&  rhsString)
{
    BSLS_ASSERT_SAFE(lhsString || 0 == lhsLength);
    BSLS_ASSERT_SAFE(0 <= lhsLength);

    return areEqualCaseless(lhsString,
                            lhsLength,
                            rhsString.data(),
                            static_cast<int>(rhsString.size()));
}

inline
bool String::areEqualCaseless(const bsl::string&  lhsString,
                              const char         *rhsString)
{
    BSLS_ASSERT_SAFE(rhsString);

    return areEqualCaseless(rhsString,
                            lhsString.data(),
                            static_cast<int>(lhsString.size()));
}

inline
bool String::areEqualCaseless(const bsl::string&  lhsString,
                              const char         *rhsString,
                              int                 rhsLength)
{
    BSLS_ASSERT_SAFE(rhsString || 0 == rhsLength);
    BSLS_ASSERT_SAFE(0 <= rhsLength);

    return areEqualCaseless(lhsString.data(),
                            static_cast<int>(lhsString.size()),
                            rhsString,
                            rhsLength);
}

inline
bool String::areEqualCaseless(const bsl::string& lhsString,
                              const bsl::string& rhsString)
{
    return areEqualCaseless(lhsString.data(),
                            static_cast<int>(lhsString.size()),
                            rhsString.data(),
                            static_cast<int>(rhsString.size()));
}

inline
char *String::copy(const char *string, bslma::Allocator *basicAllocator)
{
    BSLS_ASSERT_SAFE(string);
    BSLS_ASSERT_SAFE(basicAllocator);

    return copy(string,
                static_cast<int>(bsl::strlen(string)),
                basicAllocator);
}

inline
char *String::copy(const bsl::string&  string,
                   bslma::Allocator   *basicAllocator)
{
    BSLS_ASSERT_SAFE(basicAllocator);

    return copy(string.data(),
                static_cast<int>(string.length()),
                basicAllocator);
}

inline
int String::lowerCaseCmp(const char         *lhsString,
                         const bsl::string&  rhsString)
{
    BSLS_ASSERT_SAFE(lhsString);

    return lowerCaseCmp(lhsString,
                        &rhsString[0],
                        static_cast<int>(rhsString.size()));
}

inline
int String::lowerCaseCmp(const char *lhsString,
                         int         lhsLength,
                         const char *rhsString)
{
    BSLS_ASSERT_SAFE(lhsString || 0 == lhsLength);
    BSLS_ASSERT_SAFE(0 <= lhsLength);
    BSLS_ASSERT_SAFE(rhsString);

    return -lowerCaseCmp(rhsString, lhsString, lhsLength);
}

inline
int String::lowerCaseCmp(const char         *lhsString,
                         int                 lhsLength,
                         const bsl::string&  rhsString)
{
    BSLS_ASSERT_SAFE(lhsString || 0 == lhsLength);
    BSLS_ASSERT_SAFE(0 <= lhsLength);

    return lowerCaseCmp(lhsString,
                        lhsLength,
                        rhsString.data(),
                        static_cast<int>(rhsString.size()));
}

inline
int String::lowerCaseCmp(const bsl::string&  lhsString,
                         const char         *rhsString)
{
    BSLS_ASSERT_SAFE(rhsString);

    return -lowerCaseCmp(rhsString,
                         lhsString.data(),
                         static_cast<int>(lhsString.size()));
}

inline
int String::lowerCaseCmp(const bsl::string&  lhsString,
                         const char         *rhsString,
                         int                 rhsLength)
{
    BSLS_ASSERT_SAFE(rhsString || 0 == rhsLength);
    BSLS_ASSERT_SAFE(0 <= rhsLength);

    return lowerCaseCmp(lhsString.data(),
                        static_cast<int>(lhsString.size()),
                        rhsString,
                        rhsLength);
}

inline
int String::lowerCaseCmp(const bsl::string& lhsString,
                         const bsl::string& rhsString)
{
    return lowerCaseCmp(lhsString.data(),
                        static_cast<int>(lhsString.size()),
                        rhsString.data(),
                        static_cast<int>(rhsString.size()));
}

inline
void String::ltrim(bsl::string *string)
{
    BSLS_ASSERT_SAFE(string);

    int length = static_cast<int>(string->size());
    if (length > 0) {
        ltrim(&(*string)[0], &length);
        string->resize(length);
    }
}

inline
void String::pad(bsl::string *string, int numChars, char padChar)
{
    BSLS_ASSERT_SAFE(string);
    BSLS_ASSERT_SAFE(0 <= numChars);

    if (string->size() < static_cast<bsl::size_t>(numChars)) {
        string->resize(numChars, padChar);
    }
}

inline
void String::rtrim(bsl::string *string)
{
    BSLS_ASSERT_SAFE(string);

    int length = static_cast<int>(string->size());
    if (length > 0) {
        rtrim(&(*string)[0], &length);
        string->resize(length);
    }
}

inline
void String::toLower(bsl::string *string)
{
    BSLS_ASSERT_SAFE(string);

    int length = static_cast<int>(string->size());
    if (length > 0) {
        toLower(&(*string)[0], length);
    }
}

inline
void String::toUpper(bsl::string *string)
{
    BSLS_ASSERT_SAFE(string);

    int length = static_cast<int>(string->size());
    if (length > 0) {
        toUpper(&(*string)[0], length);
    }
}

inline
void String::trim(char *string, int *length)
{
    BSLS_ASSERT_SAFE(string);
    BSLS_ASSERT_SAFE(length);
    BSLS_ASSERT_SAFE(0 <= *length);

    rtrim(string, length);
    ltrim(string, length);
}

inline
void String::trim(bsl::string *string)
{
    BSLS_ASSERT_SAFE(string);

    int length = static_cast<int>(string->size());
    if (length > 0) {
        rtrim(&(*string)[0], &length);
        ltrim(&(*string)[0], &length);
        string->resize(length);
    }
}

inline
int String::upperCaseCmp(const char         *lhsString,
                         const bsl::string&  rhsString)
{
    BSLS_ASSERT_SAFE(lhsString);

    return upperCaseCmp(lhsString,
                        &rhsString[0],
                        static_cast<int>(rhsString.size()));
}

inline
int String::upperCaseCmp(const char *lhsString,
                         int         lhsLength,
                         const char *rhsString)
{
    BSLS_ASSERT_SAFE(lhsString || 0 == lhsLength);
    BSLS_ASSERT_SAFE(0 <= lhsLength);
    BSLS_ASSERT_SAFE(rhsString);

    return -upperCaseCmp(rhsString, lhsString, lhsLength);
}

inline
int String::upperCaseCmp(const char         *lhsString,
                         int                 lhsLength,
                         const bsl::string&  rhsString)
{
    BSLS_ASSERT_SAFE(lhsString || 0 == lhsLength);
    BSLS_ASSERT_SAFE(0 <= lhsLength);

    return upperCaseCmp(lhsString,
                        lhsLength,
                        rhsString.data(),
                        static_cast<int>(rhsString.size()));
}

inline
int String::upperCaseCmp(const bsl::string&  lhsString,
                         const char         *rhsString)
{
    BSLS_ASSERT_SAFE(rhsString);

    return -upperCaseCmp(rhsString,
                         lhsString.data(),
                         static_cast<int>(lhsString.size()));
}

inline
int String::upperCaseCmp(const bsl::string&  lhsString,
                         const char         *rhsString,
                         int                 rhsLength)
{
    BSLS_ASSERT_SAFE(rhsString || 0 == rhsLength);
    BSLS_ASSERT_SAFE(0 <= rhsLength);

    return upperCaseCmp(lhsString.data(),
                        static_cast<int>(lhsString.size()),
                        rhsString,
                        rhsLength);
}

inline
int String::upperCaseCmp(const bsl::string& lhsString,
                         const bsl::string& rhsString)
{
    return upperCaseCmp(lhsString.data(),
                        static_cast<int>(lhsString.size()),
                        rhsString.data(),
                        static_cast<int>(rhsString.size()));
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
