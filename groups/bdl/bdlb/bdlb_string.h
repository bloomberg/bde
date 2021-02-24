// bdlb_string.h                                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLB_STRING
#define INCLUDED_BDLB_STRING

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utility functions on STL-style and C-style strings.
//
//@CLASSES:
//  bdlb::String: namespace for functions on STL-style and C-style strings
//
//@SEE_ALSO: bdlb_stringviewutil
//
//@DESCRIPTION: This component defines a utility 'struct', 'bdlb::String', that
// provides a suite of functions operating on STL-style strings ('bsl::string',
// 'std::string', 'std::pmr::string'), C-style strings (necessarily
// null-terminated), and strings specified by a '(const char *, int)' or
// '(char *, int)' pair.
//
// Unlike C-style strings, strings specified via '(const char *, int)' or
// '(char *, int)' need not be null-terminated and may contain embedded null
// ('\0') characters.  Note that if a string is specified by a 'const char *'
// and a length, the string address may be null only if 0 is supplied for the
// length; this is the only case where a null string argument is accepted by
// the functions of this utility.  In particular, for strings specified by a
// 'char *' (to non-'const') and a length, the string address must not be null
// even if 0 is supplied for the length.
//
///Synopsis
///--------
// This component provides several kinds of utility functions, including both
// case-sensitive and case-insensitive comparisons, case conversions, trimming
// functions, and other length-related operations.  Most of the methods are
// overloaded to work on three string styles: (1) STL-style ('bsl::string',
// 'std::string', 'std::pmr::string'), (2) C-style, and (3) a
// '(const char *, int)' or '(char *, int)' pair.  These overloaded methods are
// indicated schematically with a pseudo-argument 'STR' representing any of the
// three possible string styles.  Note that the pseudo-signature
// 'foo(STR, STR)' represents nine overloaded signatures, not three.
//..
//  Method                        Purpose
//  --------------------------    --------------------------------------------
//  areEqualCaseless(STR, STR)    case-insensitive equality comparison
//  lowerCaseCmp(STR, STR)        lexical comparison of lower-cased strings
//  upperCaseCmp(STR, STR)        lexical comparison of upper-cased strings
//
//  ltrim(STR)                    remove leading whitespace
//  rtrim(STR)                    remove trailing whitespace
//  trim(STR)                     remove both leading and trailing whitespace
//
//  toLower(STR)                  convert to lower case
//  toUpper(STR)                  convert to upper case
//
//  strnlen(const char *, int)    null-terminated length, but bounded by 'int'
//  toFixedLength(...)            fixed-length copy with padding character
//  pad(...)                      append padding 'char' up to specified length
//..
//
///UTF-8
///------
// Note that functions involving the case and classification of characters deal
// with base ASCII *only*.  For example, for strings encoded in UTF-8, the
// functions behave as expected for the ASCII subset of UTF-8 but do *not*
// provide full Unicode support.

#include <bdlscm_version.h>

#include <bslma_allocator.h>

#include <bsls_assert.h>
#include <bsls_libraryfeatures.h>
#include <bsls_review.h>

#include <bsl_climits.h>    // 'INT_MAX'
#include <bsl_cstddef.h>
#include <bsl_cstring.h>
#include <bsl_string.h>

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
#include <memory_resource>  // 'std::pmr::polymorphic_allocator'
#endif

#include <string>           // 'std::string', 'std::pmr::string'

namespace BloombergLP {
namespace bdlb {

                               // =============
                               // struct String
                               // =============

struct String {
    // This 'struct' provides a namespace for a suite of functions on STL-style
    // strings ('bsl::string', 'std::string', 'std::pmr::string'), C-style
    // strings, and strings specified by a '(const char *, int)' or
    // '(char *, int)' pair.

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
        // Compare the specified 'lhsString' and 'rhsString' having the
        // optionally specified 'lhsLength' and 'rhsLength', respectively, *as*
        // *if* the strings were converted to lower case before the comparison.
        // Return 1 if 'lhsString' is lexically greater than 'rhsString', 0 if
        // they are equal, and -1 otherwise.  The behavior is undefined unless
        // '0 <= lhsLength' and '0 <= rhsLength' (if specified), and
        // 'lhsString.size() <= INT_MAX' and 'rhsString.size() <= INT_MAX' (if
        // applicable).  See {'bdlb_stringviewutil'} for an identically named
        // method having the same semantics taking 'bsl::string_view'.

    static char *copy(const char *string, bslma::Allocator *basicAllocator);
        // Create a null-terminated copy of the specified 'string', using the
        // specified 'basicAllocator' to supply memory, and return the address
        // of the newly-created modifiable string.  The string that is returned
        // is owned by the caller.  The behavior is undefined unless
        // 'bsl::strlen(string) <= INT_MAX' and 'basicAllocator' is non-null.

    static char *copy(const char       *string,
                      int               length,
                      bslma::Allocator *basicAllocator);
        // Create a null-terminated copy of the specified 'string' having the
        // specified 'length' (in bytes), using the specified 'basicAllocator'
        // to supply memory, and return the address of the newly-created
        // modifiable string.  The string that is returned is owned by the
        // caller.  The behavior is undefined unless '0 <= length' and
        // 'basicAllocator' is non-null.  Note that if 'string' contains any
        // embedded null ('\0') characters they will be propagated to the copy.

    static char *copy(const bsl::string&  string,
                      bslma::Allocator   *basicAllocator);
        // Create a null-terminated copy of the specified 'string', using the
        // specified 'basicAllocator' to supply memory, and return the address
        // of the newly-created modifiable string.  The string that is returned
        // is owned by the caller.  The behavior is undefined unless
        // 'string.size() <= INT_MAX' and 'basicAllocator' is non-null.  Note
        // that if 'string' contains any embedded null ('\0') characters they
        // will be propagated to the copy.

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
        // Compare the specified 'lhsString' and 'rhsString' having the
        // optionally specified 'lhsLength' and 'rhsLength', respectively.
        // Return 1 if, after conversion to lower case, 'lhsString' is
        // lexically greater than 'rhsString', 0 if they are equal up to a case
        // conversion, and -1 otherwise.  The behavior is undefined unless
        // '0 <= lhsLength' and '0 <= rhsLength' (if specified), and
        // 'lhsString.size() <= INT_MAX' and 'rhsString.size() <= INT_MAX' (if
        // applicable).  See {'bdlb_stringviewutil'} for an identically named
        // method having the same semantics taking 'bsl::string_view'.

    static void ltrim(char *string);
    static void ltrim(bsl::string *string);
    static void ltrim(std::string *string);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
    static void ltrim(std::pmr::string *string);
#endif
        // Remove all leading whitespace characters from the specified
        // 'string'.  If 'string' consists of only whitespace then it will be
        // empty (i.e., have 0 length) after this operation.  The behavior is
        // undefined unless 'string->size() <= INT_MAX' (if applicable).  See
        // {'bdlb_stringviewutil'} for an identically named method having
        // similar semantics taking (and returning) 'bsl::string_view'.

    static void ltrim(char *string, int *length);
        // Remove all leading whitespace characters from the specified 'string'
        // having the specified 'length', and load into 'length' the number of
        // characters in the resulting (trimmed) string.  If 'string' consists
        // of only whitespace then it will be empty (i.e., have 0 length) after
        // this operation.  The behavior is undefined unless '0 <= *length'.
        // Note that 'length' is both an input and output parameter.  See
        // {'bdlb_stringviewutil'} for an identically named method having
        // similar semantics taking (and returning) 'bsl::string_view'.

    static void pad(bsl::string *string, int length, char padChar = ' ');
    static void pad(std::string *string, int length, char padChar = ' ');
        // Append repeatedly to the specified 'string' the optionally specified
        // 'padChar' until 'string' has the specified 'length'.  If 'padChar'
        // is not specified the space (' ') character is appended.  This
        // operation has no effect if 'string.size() >= length'.  The behavior
        // is undefined unless '0 <= length'.

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
    static void pad(std::pmr::string            *string,
                    std::pmr::string::size_type  length,
                    char                         padChar = ' ');
        // Append repeatedly to the specified 'string' the optionally specified
        // 'padChar' until 'string' has the specified 'length'.  If 'padChar'
        // is not specified the space (' ') character is appended.  This
        // operation has no effect if 'string.size() >= length'.
#endif

    static void rtrim(char *string);
    static void rtrim(bsl::string *string);
    static void rtrim(std::string *string);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
    static void rtrim(std::pmr::string *string);
#endif
        // Remove all trailing whitespace characters from the specified
        // 'string'.  If 'string' consists of only whitespace then it will be
        // empty (i.e., have 0 length) after this operation.  The behavior is
        // undefined unless 'string->size() <= INT_MAX' (if applicable).  See
        // {'bdlb_stringviewutil'} for an identically named method having
        // similar semantics taking (and returning) 'bsl::string_view'.

    static void rtrim(const char *string, int *length);
        // Determine the number of characters that the specified 'string'
        // having the specified 'length' would have if all trailing whitespace
        // characters were removed and load the result into '*length'.
        // 'string' is *not* modified.  The behavior is undefined unless
        // '0 <= *length'.  Note that 'length' is both an input and output
        // parameter.

    static const char *strstr(const char *string,
                              int         stringLen,
                              const char *subString,
                              int         subStringLen);
        // Return the address providing non-modifiable access to the first
        // position in the specified 'string' having 'stringLen' characters at
        // which the specified 'subString' having 'subStringLen' characters is
        // found, or 0 if there is no such position.  If 'subStringLen' is 0,
        // 'subString' may be null and 'string' is returned.  The behavior is
        // undefined unless '0 <= stringLen' and '0 <= subStringLen'.  See
        // {'bdlb_stringviewutil'} for an identically named method having
        // similar semantics taking (and returning) 'bsl::string_view'.

    static const char *strstrCaseless(const char *string,
                                      int         stringLen,
                                      const char *subString,
                                      int         subStringLen);
        // Return the address providing non-modifiable access to the first
        // position in the specified 'string' having 'stringLen' characters at
        // which the specified 'subString' having 'subStringLen' characters is
        // found using case-insensitive comparison, or 0 if there is no such
        // position.  If 'subStringLen' is 0, 'subString' may be null and
        // 'string' is returned.  The behavior is undefined unless
        // '0 <= stringLen' and '0 <= subStringLen'.  See
        // {'bdlb_stringviewutil'} for an identically named method having
        // similar semantics taking (and returning) 'bsl::string_view'.

    static const char *strrstr(const char *string,
                               int         stringLen,
                               const char *subString,
                               int         subStringLen);
        // Return the address providing non-modifiable access to the last
        // position in the specified 'string' having 'stringLen' characters at
        // which the specified 'subString' having 'subStringLen' characters is
        // found, or 0 if there is no such position.  If 'subStringLen' is 0,
        // 'subString' may be null and 'string + stringLen' is returned.  The
        // behavior is undefined unless '0 <= stringLen' and
        // '0 <= subStringLen'.  See {'bdlb_stringviewutil'} for an identically
        // named method having similar semantics taking (and returning)
        // 'bsl::string_view'.

    static const char *strrstrCaseless(const char *string,
                                       int         stringLen,
                                       const char *subString,
                                       int         subStringLen);
        // Return the address providing non-modifiable access to the last
        // position in the specified 'string' having 'stringLen' characters at
        // which the specified 'subString' having 'subStringLen' characters is
        // found using case-insensitive comparison, or 0 if there is no such
        // position.  If 'subStringLen' is 0, 'subString' may be null and
        // 'string + stringLen' is returned.  The behavior is undefined unless
        // '0 <= stringLen' and '0 <= subStringLen'.  See
        // {'bdlb_stringviewutil'} for an identically named method having
        // similar semantics taking (and returning) 'bsl::string_view'.

    static int strnlen(const char *string, int maximumLength);
        // Return the minimum of the length of the specified 'string' and the
        // specified 'maximumLength'.  If 'maximumLength' is 0, 'string' may be
        // null and 0 is returned.  The behavior is undefined unless
        // '0 <= maximumLength'.

    static void toFixedLength(char       *dstString,
                              int         dstLength,
                              const char *srcString,
                              int         srcLength,
                              char        padChar = ' ');
        // Copy into the specified 'dstString' at most the specified leading
        // 'dstLength' characters from the specified 'srcString' having the
        // specified 'srcLength'.  If 'srcLength < dstLength', after
        // 'srcString' is copied to 'dstString' repeatedly append to
        // 'dstString' the optionally specified 'padChar' until the total
        // number of characters written to 'dstString' is 'dstLength'.  If
        // 'padChar' is not specified the space (' ') character is appended.
        // The behavior is undefined unless '0 <= dstLength' and
        // '0 <= srcLength'.

    static void toLower(char *string);
    static void toLower(char *string, int length);
    static void toLower(bsl::string *string);
    static void toLower(std::string *string);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
    static void toLower(std::pmr::string *string);
#endif
        // Replace all upper case characters in the specified 'string' having
        // the optionally specified 'length' with their lower-case equivalent.
        // The behavior is undefined unless '0 <= length' (if specified) and
        // 'string->size() <= INT_MAX' (if applicable).

    static void toUpper(char *string);
    static void toUpper(char *string, int length);
    static void toUpper(bsl::string *string);
    static void toUpper(std::string *string);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
    static void toUpper(std::pmr::string *string);
#endif
        // Replace all lower case characters in the specified 'string' having
        // the optionally specified 'length' with their upper-case equivalent.
        // The behavior is undefined unless '0 <= length' (if specified) and
        // 'string->size() <= INT_MAX' (if applicable).

    static void trim(char *string);
    static void trim(bsl::string *string);
    static void trim(std::string *string);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
    static void trim(std::pmr::string *string);
#endif
        // Remove all leading and trailing whitespace characters from the
        // specified 'string'.  If 'string' consists of only whitespace then it
        // will be empty (i.e., have 0 length) after this operation.  The
        // behavior is undefined unless 'string->size() <= INT_MAX' (if
        // applicable).  See {'bdlb_stringviewutil'} for an identically named
        // method having similar semantics taking (and returning)
        // 'bsl::string_view'.

    static void trim(char *string, int *length);
        // Remove all leading and trailing whitespace characters from the
        // specified 'string' having the specified 'length', and load into
        // 'length' the number of characters in the resulting (trimmed) string.
        // If 'string' consists of only whitespace then it will be empty (i.e.,
        // have 0 length) after this operation.  The behavior is undefined
        // unless '0 <= *length'.  Note that 'length' is both an input and
        // output parameter.  See {'bdlb_stringviewutil'} for an identically
        // named method having similar semantics taking (and returning)
        // 'bsl::string_view'.

    static void skipLeadingTrailing(const char **begin, const char **end);
        // Skip leading and trailing whitespace characters in the string
        // indicated by the specified '*begin' and '*end' iterators by
        // appropriately advancing '*begin' and regressing '*end'.  If the
        // indicated string is empty, or consists solely of whitespace
        // characters, '*begin' is unchanged and '*end' is regressed to
        // '*begin'.  Otherwise, advance '*begin' to the first non-whitespace
        // character whose position is greater than or equal to '*begin' and
        // regress '*end' to one past the position of the last non-whitespace
        // character whose position is less than '*end'.  The indicated string
        // need not be null-terminated and may contain embedded null ('\0')
        // characters.  The behavior is undefined unless '*begin <= *end'.
        // Note that since '*begin' and '*end' are iterators, '*end' refers to
        // the character one past the end of the subject string.

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
        // Compare the specified 'lhsString' and 'rhsString' having the
        // optionally specified 'lhsLength' and 'rhsLength', respectively, *as*
        // *if* the strings were converted to upper case before the comparison.
        // Return 1 if 'lhsString' is lexically greater than 'rhsString', 0 if
        // they are equal, and -1 otherwise.  The behavior is undefined unless
        // '0 <= lhsLength' and '0 <= rhsLength' (if specified), and
        // 'lhsString.size() <= INT_MAX' and 'rhsString.size() <= INT_MAX' (if
        // applicable).  See {'bdlb_stringviewutil'} for an identically named
        // method having the same semantics taking 'bsl::string_view'.
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
    BSLS_ASSERT(lhsString);
    BSLS_ASSERT(rhsString.size() <= INT_MAX);

    return areEqualCaseless(lhsString,
                            &rhsString[0],
                            static_cast<int>(rhsString.size()));
}

inline
bool String::areEqualCaseless(const char *lhsString,
                              int         lhsLength,
                              const char *rhsString)
{
    BSLS_ASSERT(lhsString || 0 == lhsLength);
    BSLS_ASSERT(0 <= lhsLength);
    BSLS_ASSERT(rhsString);

    return areEqualCaseless(rhsString, lhsString, lhsLength);
}

inline
bool String::areEqualCaseless(const char         *lhsString,
                              int                 lhsLength,
                              const bsl::string&  rhsString)
{
    BSLS_ASSERT(lhsString || 0 == lhsLength);
    BSLS_ASSERT(0 <= lhsLength);
    BSLS_ASSERT(rhsString.size() <= INT_MAX);

    return areEqualCaseless(lhsString,
                            lhsLength,
                            rhsString.data(),
                            static_cast<int>(rhsString.size()));
}

inline
bool String::areEqualCaseless(const bsl::string&  lhsString,
                              const char         *rhsString)
{
    BSLS_ASSERT(lhsString.size() <= INT_MAX);
    BSLS_ASSERT(rhsString);

    return areEqualCaseless(rhsString,
                            lhsString.data(),
                            static_cast<int>(lhsString.size()));
}

inline
bool String::areEqualCaseless(const bsl::string&  lhsString,
                              const char         *rhsString,
                              int                 rhsLength)
{
    BSLS_ASSERT(lhsString.size() <= INT_MAX);
    BSLS_ASSERT(rhsString || 0 == rhsLength);
    BSLS_ASSERT(0 <= rhsLength);

    return areEqualCaseless(lhsString.data(),
                            static_cast<int>(lhsString.size()),
                            rhsString,
                            rhsLength);
}

inline
bool String::areEqualCaseless(const bsl::string& lhsString,
                              const bsl::string& rhsString)
{
    BSLS_ASSERT(lhsString.size() <= INT_MAX);
    BSLS_ASSERT(rhsString.size() <= INT_MAX);

    return areEqualCaseless(lhsString.data(),
                            static_cast<int>(lhsString.size()),
                            rhsString.data(),
                            static_cast<int>(rhsString.size()));
}

inline
char *String::copy(const char *string, bslma::Allocator *basicAllocator)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(bsl::strlen(string) <= INT_MAX);
    BSLS_ASSERT(basicAllocator);

    return copy(string,
                static_cast<int>(bsl::strlen(string)),
                basicAllocator);
}

inline
char *String::copy(const bsl::string&  string,
                   bslma::Allocator   *basicAllocator)
{
    BSLS_ASSERT(string.size() <= INT_MAX);
    BSLS_ASSERT(basicAllocator);

    return copy(string.data(),
                static_cast<int>(string.size()),
                basicAllocator);
}

inline
int String::lowerCaseCmp(const char         *lhsString,
                         const bsl::string&  rhsString)
{
    BSLS_ASSERT(lhsString);
    BSLS_ASSERT(rhsString.size() <= INT_MAX);

    return lowerCaseCmp(lhsString,
                        &rhsString[0],
                        static_cast<int>(rhsString.size()));
}

inline
int String::lowerCaseCmp(const char *lhsString,
                         int         lhsLength,
                         const char *rhsString)
{
    BSLS_ASSERT(lhsString || 0 == lhsLength);
    BSLS_ASSERT(0 <= lhsLength);
    BSLS_ASSERT(rhsString);

    return -lowerCaseCmp(rhsString, lhsString, lhsLength);
}

inline
int String::lowerCaseCmp(const char         *lhsString,
                         int                 lhsLength,
                         const bsl::string&  rhsString)
{
    BSLS_ASSERT(lhsString || 0 == lhsLength);
    BSLS_ASSERT(0 <= lhsLength);
    BSLS_ASSERT(rhsString.size() <= INT_MAX);

    return lowerCaseCmp(lhsString,
                        lhsLength,
                        rhsString.data(),
                        static_cast<int>(rhsString.size()));
}

inline
int String::lowerCaseCmp(const bsl::string&  lhsString,
                         const char         *rhsString)
{
    BSLS_ASSERT(lhsString.size() <= INT_MAX);
    BSLS_ASSERT(rhsString);

    return -lowerCaseCmp(rhsString,
                         lhsString.data(),
                         static_cast<int>(lhsString.size()));
}

inline
int String::lowerCaseCmp(const bsl::string&  lhsString,
                         const char         *rhsString,
                         int                 rhsLength)
{
    BSLS_ASSERT(lhsString.size() <= INT_MAX);
    BSLS_ASSERT(rhsString || 0 == rhsLength);
    BSLS_ASSERT(0 <= rhsLength);

    return lowerCaseCmp(lhsString.data(),
                        static_cast<int>(lhsString.size()),
                        rhsString,
                        rhsLength);
}

inline
int String::lowerCaseCmp(const bsl::string& lhsString,
                         const bsl::string& rhsString)
{
    BSLS_ASSERT(lhsString.size() <= INT_MAX);
    BSLS_ASSERT(rhsString.size() <= INT_MAX);

    return lowerCaseCmp(lhsString.data(),
                        static_cast<int>(lhsString.size()),
                        rhsString.data(),
                        static_cast<int>(rhsString.size()));
}

inline
void String::ltrim(bsl::string *string)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(string->size() <= INT_MAX);

    int length = static_cast<int>(string->size());

    ltrim(&(*string)[0], &length);
    string->resize(length);
}

inline
void String::ltrim(std::string *string)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(string->size() <= INT_MAX);

    int length = static_cast<int>(string->size());

    ltrim(&(*string)[0], &length);
    string->resize(length);
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
inline
void String::ltrim(std::pmr::string *string)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(string->size() <= INT_MAX);

    int length = static_cast<int>(string->size());

    ltrim(&(*string)[0], &length);
    string->resize(length);
}
#endif

inline
void String::pad(bsl::string *string, int length, char padChar)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    if (string->size() < static_cast<bsl::size_t>(length)) {
        string->resize(length, padChar);
    }
}

inline
void String::pad(std::string *string, int length, char padChar)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    if (string->size() < static_cast<bsl::size_t>(length)) {
        string->resize(length, padChar);
    }
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
inline
void String::pad(std::pmr::string            *string,
                 std::pmr::string::size_type  length,
                 char                         padChar)
{
    BSLS_ASSERT(string);

    if (string->size() < length) {
        string->resize(length, padChar);
    }
}
#endif

inline
void String::rtrim(bsl::string *string)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(string->size() <= INT_MAX);

    int length = static_cast<int>(string->size());

    rtrim(&(*string)[0], &length);
    string->resize(length);
}

inline
void String::rtrim(std::string *string)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(string->size() <= INT_MAX);

    int length = static_cast<int>(string->size());

    rtrim(&(*string)[0], &length);
    string->resize(length);
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
inline
void String::rtrim(std::pmr::string *string)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(string->size() <= INT_MAX);

    int length = static_cast<int>(string->size());

    rtrim(&(*string)[0], &length);
    string->resize(length);
}
#endif

inline
void String::toLower(bsl::string *string)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(string->size() <= INT_MAX);

    toLower(&(*string)[0], static_cast<int>(string->size()));
}

inline
void String::toLower(std::string *string)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(string->size() <= INT_MAX);

    toLower(&(*string)[0], static_cast<int>(string->size()));
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
inline
void String::toLower(std::pmr::string *string)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(string->size() <= INT_MAX);

    toLower(&(*string)[0], static_cast<int>(string->size()));
}
#endif

inline
void String::toUpper(bsl::string *string)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(string->size() <= INT_MAX);

    toUpper(&(*string)[0], static_cast<int>(string->size()));
}

inline
void String::toUpper(std::string *string)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(string->size() <= INT_MAX);

    toUpper(&(*string)[0], static_cast<int>(string->size()));
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
inline
void String::toUpper(std::pmr::string *string)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(string->size() <= INT_MAX);

    toUpper(&(*string)[0], static_cast<int>(string->size()));
}
#endif

inline
void String::trim(char *string, int *length)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(length);
    BSLS_ASSERT(0 <= *length);

    rtrim(string, length);
    ltrim(string, length);
}

inline
void String::trim(bsl::string *string)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(string->size() <= INT_MAX);

    int length = static_cast<int>(string->size());

    rtrim(&(*string)[0], &length);
    ltrim(&(*string)[0], &length);
    string->resize(length);
}

inline
void String::trim(std::string *string)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(string->size() <= INT_MAX);

    int length = static_cast<int>(string->size());

    rtrim(&(*string)[0], &length);
    ltrim(&(*string)[0], &length);
    string->resize(length);
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
inline
void String::trim(std::pmr::string *string)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(string->size() <= INT_MAX);

    int length = static_cast<int>(string->size());

    rtrim(&(*string)[0], &length);
    ltrim(&(*string)[0], &length);
    string->resize(length);
}
#endif

inline
int String::upperCaseCmp(const char         *lhsString,
                         const bsl::string&  rhsString)
{
    BSLS_ASSERT(lhsString);
    BSLS_ASSERT(rhsString.size() <= INT_MAX);

    return upperCaseCmp(lhsString,
                        &rhsString[0],
                        static_cast<int>(rhsString.size()));
}

inline
int String::upperCaseCmp(const char *lhsString,
                         int         lhsLength,
                         const char *rhsString)
{
    BSLS_ASSERT(lhsString || 0 == lhsLength);
    BSLS_ASSERT(0 <= lhsLength);
    BSLS_ASSERT(rhsString);

    return -upperCaseCmp(rhsString, lhsString, lhsLength);
}

inline
int String::upperCaseCmp(const char         *lhsString,
                         int                 lhsLength,
                         const bsl::string&  rhsString)
{
    BSLS_ASSERT(lhsString || 0 == lhsLength);
    BSLS_ASSERT(0 <= lhsLength);
    BSLS_ASSERT(rhsString.size() <= INT_MAX);

    return upperCaseCmp(lhsString,
                        lhsLength,
                        rhsString.data(),
                        static_cast<int>(rhsString.size()));
}

inline
int String::upperCaseCmp(const bsl::string&  lhsString,
                         const char         *rhsString)
{
    BSLS_ASSERT(lhsString.size() <= INT_MAX);
    BSLS_ASSERT(rhsString);

    return -upperCaseCmp(rhsString,
                         lhsString.data(),
                         static_cast<int>(lhsString.size()));
}

inline
int String::upperCaseCmp(const bsl::string&  lhsString,
                         const char         *rhsString,
                         int                 rhsLength)
{
    BSLS_ASSERT(lhsString.size() <= INT_MAX);
    BSLS_ASSERT(rhsString || 0 == rhsLength);
    BSLS_ASSERT(0 <= rhsLength);

    return upperCaseCmp(lhsString.data(),
                        static_cast<int>(lhsString.size()),
                        rhsString,
                        rhsLength);
}

inline
int String::upperCaseCmp(const bsl::string& lhsString,
                         const bsl::string& rhsString)
{
    BSLS_ASSERT(lhsString.size() <= INT_MAX);
    BSLS_ASSERT(rhsString.size() <= INT_MAX);

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
