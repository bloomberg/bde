// bdeu_string.h                                                      -*-C++-*-
#ifndef INCLUDED_BDEU_STRING
#define INCLUDED_BDEU_STRING

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide utility functions on C-style and 'STL' strings.
//
//@CLASSES:
//  bdeu_String: namespace for pure procedures on C-style and 'STL' strings
//
//@AUTHOR: Jeff Mendelsohn (jmendels)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a suite of pure procedures that operate
// on C-style strings and 'bsl::string'.
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

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
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

                         // ==================
                         // struct bdeu_String
                         // ==================

struct bdeu_String {
    // This 'struct' provides a namespace for a suite of pure procedures on
    // C-style strings and 'bsl::string'.

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

    static char *copy(const char *string, bslma_Allocator *basicAllocator);
        // Create a null-terminated copy of the specified 'string', using the
        // specified 'basicAllocator' to supply memory, and return the address
        // of the newly-created modifiable string.  The string that is returned
        // is owned by the caller.  The behavior is undefined unless
        // 'basicAllocator' is non-null.

    static char *copy(const char      *string,
                      int              length,
                      bslma_Allocator *basicAllocator);
        // Create a null-terminated copy of the specified 'string' having the
        // specified 'length' (in bytes), using the specified 'basicAllocator'
        // to supply memory, and return the address of the newly-created
        // modifiable string.  The string that is returned is owned by the
        // caller.  The behavior is undefined unless '0 <= length' and
        // 'basicAllocator' is non-null.  Note that 'string' need not be
        // null-terminated and may contain null ('\0') characters.

    static char *copy(const bsl::string&  string,
                      bslma_Allocator    *basicAllocator);
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
        // Append repeatedly to the specified 'string' the specified 'padChar'
        // until the length of 'string' is 'numChars'.  This operation has no
        // effect if 'string.size() >= numChars'.

    static void rtrim(char *string);
    static void rtrim(bsl::string *string);
        // Remove from the specified 'string' all trailing whitespace to the
        // right of the last non-whitespace character in 'string'.  If 'string'
        // has no non-whitespace characters, 'string' will be empty (i.e., have
        // zero length) after this operation.

    static void rtrim(const char *string, int *length);
        // Load, into the specified 'length', the number of characters from the
        // left-most character to the right-most character that is not a
        // whitespace in the specified 'string'.  Note that 'length' will be
        // set to the length of 'string' if all whitespace characters at the
        // right edge of 'string' have been removed.

    static const char *strstr(const char *string,
                              int         stringLen,
                              const char *subString,
                              int         subStringLen);
        // Return the address of the non-modifiable first occurrence of the
        // specified 'subString' having the specified 'subStringLen' in the
        // specified 'string' having the specified 'stringLen'; return 0 if
        // 'subString' is not found in 'string'.  The comparison is
        // case-sensitive.  Note that a 'subString' of length 0 matches the
        // beginning of 'string'.

    static const char *strstrCaseless(const char *string,
                                      int         stringLen,
                                      const char *subString,
                                      int         subStringLen);
        // Return the address of the non-modifiable first occurrence of the
        // specified 'subString' having the specified 'subStringLen' in the
        // specified 'string' having the specified 'stringLen'; return 0 if
        // 'subString' is not found in 'string'.  The comparison is
        // case-insensitive.  Note that a 'subString' of length 0 matches the
        // beginning of 'string'.

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
        // append to 'dstString' the specified 'padChar' until the total number
        // of characters written to 'dstString' is 'dstLength'.

    static void toLower(char *string);
    static void toLower(char *string, int length);
    static void toLower(bsl::string *string);
        // Replace all upper case characters in 'string' with their lowercase
        // equivalent.

    static void toUpper(char *string);
    static void toUpper(char *string, int length);
    static void toUpper(bsl::string *string);
        // Replace all lower case characters in 'string' with their uppercase
        // equivalent.

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
        // of whitespace characters, '*begin' is uneffected and '*end' is
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

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                         // ------------------
                         // struct bdeu_String
                         // ------------------

// CLASS METHODS
inline
bool bdeu_String::areEqualCaseless(const char         *lhsString,
                                   const bsl::string&  rhsString)
{
    BSLS_ASSERT_SAFE(lhsString);

    return areEqualCaseless(lhsString, &rhsString[0], (int)rhsString.size());
}

inline
bool bdeu_String::areEqualCaseless(const char *lhsString,
                                   int         lhsLength,
                                   const char *rhsString)
{
    BSLS_ASSERT_SAFE(lhsString);
    BSLS_ASSERT_SAFE(0 <= lhsLength);
    BSLS_ASSERT_SAFE(rhsString);

    return areEqualCaseless(rhsString, lhsString, lhsLength);
}

inline
bool bdeu_String::areEqualCaseless(const char         *lhsString,
                                   int                 lhsLength,
                                   const bsl::string&  rhsString)
{
    BSLS_ASSERT_SAFE(lhsString);
    BSLS_ASSERT_SAFE(0 <= lhsLength);

    return areEqualCaseless(lhsString,
                            lhsLength,
                            rhsString.data(),
                            (int)rhsString.size());
}

inline
bool bdeu_String::areEqualCaseless(const bsl::string&  lhsString,
                                   const char         *rhsString)
{
    BSLS_ASSERT_SAFE(rhsString);

    return areEqualCaseless(rhsString,
                            lhsString.data(),
                            (int)lhsString.size());
}

inline
bool bdeu_String::areEqualCaseless(const bsl::string&  lhsString,
                                   const char         *rhsString,
                                   int                 rhsLength)
{
    BSLS_ASSERT_SAFE(rhsString);
    BSLS_ASSERT_SAFE(0 <= rhsLength);

    return areEqualCaseless(lhsString.data(),
                            (int)lhsString.size(),
                            rhsString,
                            rhsLength);
}

inline
bool bdeu_String::areEqualCaseless(const bsl::string& lhsString,
                                   const bsl::string& rhsString)
{
    return areEqualCaseless(lhsString.data(),
                            (int)lhsString.size(),
                            rhsString.data(),
                            (int)rhsString.size());
}

inline
char *bdeu_String::copy(const char *string, bslma_Allocator *basicAllocator)
{
    BSLS_ASSERT_SAFE(string);
    BSLS_ASSERT_SAFE(basicAllocator);

    return copy(string,
                static_cast<int>(bsl::strlen(string)),
                basicAllocator);
}

inline
char *bdeu_String::copy(const bsl::string&  string,
                        bslma_Allocator    *basicAllocator)
{
    BSLS_ASSERT_SAFE(basicAllocator);

    return copy(string.data(),
                static_cast<int>(string.length()),
                basicAllocator);
}

inline
int bdeu_String::lowerCaseCmp(const char         *lhsString,
                              const bsl::string&  rhsString)
{
    BSLS_ASSERT_SAFE(lhsString);

    return lowerCaseCmp(lhsString, &rhsString[0], (int)rhsString.size());
}

inline
int bdeu_String::lowerCaseCmp(const char *lhsString,
                              int         lhsLength,
                              const char *rhsString)
{
    BSLS_ASSERT_SAFE(lhsString);
    BSLS_ASSERT_SAFE(0 <= lhsLength);
    BSLS_ASSERT_SAFE(rhsString);

    return -lowerCaseCmp(rhsString, lhsString, lhsLength);
}

inline
int bdeu_String::lowerCaseCmp(const char         *lhsString,
                              int                 lhsLength,
                              const bsl::string&  rhsString)
{
    BSLS_ASSERT_SAFE(lhsString);
    BSLS_ASSERT_SAFE(0 <= lhsLength);

    return lowerCaseCmp(lhsString,
                        lhsLength,
                        rhsString.data(),
                        (int)rhsString.size());
}

inline
int bdeu_String::lowerCaseCmp(const bsl::string&  lhsString,
                              const char         *rhsString)
{
    BSLS_ASSERT_SAFE(rhsString);

    return -lowerCaseCmp(rhsString, lhsString.data(), (int)lhsString.size());
}

inline
int bdeu_String::lowerCaseCmp(const bsl::string&  lhsString,
                              const char         *rhsString,
                              int                 rhsLength)
{
    BSLS_ASSERT_SAFE(rhsString);
    BSLS_ASSERT_SAFE(0 <= rhsLength);

    return lowerCaseCmp(lhsString.data(),
                        (int)lhsString.size(),
                        rhsString,
                        rhsLength);
}

inline
int bdeu_String::lowerCaseCmp(const bsl::string& lhsString,
                              const bsl::string& rhsString)
{
    return lowerCaseCmp(lhsString.data(),
                        (int)lhsString.size(),
                        rhsString.data(),
                        (int)rhsString.size());
}

inline
void bdeu_String::ltrim(bsl::string *string)
{
    BSLS_ASSERT_SAFE(string);

    int length = (int)string->size();
    if (length > 0) {
        ltrim(&(*string)[0], &length);
        string->resize(length);
    }
}

inline
void bdeu_String::pad(bsl::string *string, int numChars, char padChar)
{
    BSLS_ASSERT_SAFE(string);
    BSLS_ASSERT_SAFE(0 <= numChars);

    if (string->size() < (bsl::size_t)numChars) {
        string->resize(numChars, padChar);
    }
}

inline
void bdeu_String::rtrim(bsl::string *string)
{
    BSLS_ASSERT_SAFE(string);

    int length = (int)string->size();
    if (length > 0) {
        rtrim(&(*string)[0], &length);
        string->resize(length);
    }
}

inline
void bdeu_String::toLower(bsl::string *string)
{
    BSLS_ASSERT_SAFE(string);

    int length = (int)string->size();
    if (length > 0) {
        toLower(&(*string)[0], length);
    }
}

inline
void bdeu_String::toUpper(bsl::string *string)
{
    BSLS_ASSERT_SAFE(string);

    int length = (int)string->size();
    if (length > 0) {
        toUpper(&(*string)[0], length);
    }
}

inline
void bdeu_String::trim(char *string)
{
    BSLS_ASSERT_SAFE(string);

    // TBD efficiency
    rtrim(string);
    ltrim(string);
}

inline
void bdeu_String::trim(char *string, int *length)
{
    BSLS_ASSERT_SAFE(string);
    BSLS_ASSERT_SAFE(length);
    BSLS_ASSERT_SAFE(0 <= *length);

    rtrim(string, length);
    ltrim(string, length);
}

inline
void bdeu_String::trim(bsl::string *string)
{
    BSLS_ASSERT_SAFE(string);

    int length = (int)string->size();
    if (length > 0) {
        rtrim(&(*string)[0], &length);
        ltrim(&(*string)[0], &length);
        string->resize(length);
    }
}

inline
int bdeu_String::upperCaseCmp(const char         *lhsString,
                              const bsl::string&  rhsString)
{
    BSLS_ASSERT_SAFE(lhsString);

    return upperCaseCmp(lhsString, &rhsString[0], (int)rhsString.size());
}

inline
int bdeu_String::upperCaseCmp(const char *lhsString,
                              int         lhsLength,
                              const char *rhsString)
{
    BSLS_ASSERT_SAFE(lhsString);
    BSLS_ASSERT_SAFE(0 <= lhsLength);
    BSLS_ASSERT_SAFE(rhsString);

    return -upperCaseCmp(rhsString, lhsString, lhsLength);
}

inline
int bdeu_String::upperCaseCmp(const char         *lhsString,
                              int                 lhsLength,
                              const bsl::string&  rhsString)
{
    BSLS_ASSERT_SAFE(lhsString);
    BSLS_ASSERT_SAFE(0 <= lhsLength);

    return upperCaseCmp(lhsString,
                        lhsLength,
                        rhsString.data(),
                        (int)rhsString.size());
}

inline
int bdeu_String::upperCaseCmp(const bsl::string&  lhsString,
                              const char         *rhsString)
{
    BSLS_ASSERT_SAFE(rhsString);

    return -upperCaseCmp(rhsString, lhsString.data(), (int)lhsString.size());
}

inline
int bdeu_String::upperCaseCmp(const bsl::string&  lhsString,
                              const char         *rhsString,
                              int                 rhsLength)
{
    BSLS_ASSERT_SAFE(rhsString);
    BSLS_ASSERT_SAFE(0 <= rhsLength);

    return upperCaseCmp(lhsString.data(),
                        (int)lhsString.size(),
                        rhsString,
                        rhsLength);
}

inline
int bdeu_String::upperCaseCmp(const bsl::string& lhsString,
                              const bsl::string& rhsString)
{
    return upperCaseCmp(lhsString.data(),
                        (int)lhsString.size(),
                        rhsString.data(),
                        (int)rhsString.size());
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
