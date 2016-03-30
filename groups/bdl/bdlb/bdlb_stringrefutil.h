// bdlb_stringrefutil.h                                               -*-C++-*-
#ifndef INCLUDED_BDLB_STRINGREFUTIL
#define INCLUDED_BDLB_STRINGREFUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utility functions on 'bslstl::StringRef'-erenced strings.
//
//@CLASSES:
//  bdlb::StringRefUtil: namespace for functions on 'bslstl::StringRef' strings
//
//@SEE_ALSO: bdlb_String, bslstl_stringref
//
//@AUTHOR: Steven Breitstein (sbreitstein)
//
//@DESCRIPTION: This component defines a utility 'struct',
// 'bdlb::StringRefUtil', that provides a suite of functions that operate on
// 'bslstl::StringRef' references to string data.
//
///Synopsis of 'bslstl::StringRef'
///-------------------------------
// The 'bslstl::StringRef' class provides 'bsl::string'-like access to an array
// of bytes that need not be null terminated and that can have non-ASCII values
// (i.e., '[128 .. 255]').  Although a 'bslstl::StringRef' object can itself be
// changed, it cannot change its referent data (the array of bytes) The
// lifetime of the referent data must exceed that of all 'bslstl::StringRef'
// objects referring to it.  Equality comparison of 'bslstl::StringRef' objects
// compares the content of the referent data (not whether or not the object
// refer to the same array of bytes).  See {'bslstl_stringref'} for full
// details.
//
///Function Synopsis
///-----------------
// The table below provides an outline of the functions provided by this
// component.  Most functions take arguments of 'const bslstl::StringRef&';
// however, the '*trim' functions take 'bslstl::StringRef *', arguments that
// specify both input and output.
//..
//  Function                   Purpose
//  -------------------------- --------------------------------------------
//  areEqualCaseless(STR, STR) case-insensitive equality comparison
//      lowerCaseCmp(STR, STR) lexical comparison of lower-case conversion
//      upperCaseCmp(STR, STR) lexical comparison of upper-case conversion
//
//                 ltrim(*STR) exclude whitespace from left  side  of string
//                 rtrim(*STR) exclude whitespace from right side  of string
//                  trim(*STR) exclude whitespace from both  sides of string
//
//   strstr         (STR, SUB) find first substring in string
//   strstrCaseless (STR, SUB) find first substring in string, case insensitive
//   strrstr        (STR, SUB) find last  substring in string
//   strrstrCaseless(STR, SUB) find last  substring in string, case insensitive
//..
//
// Since 'bslstl::StringRef' objects know the length of the referent data these
// utility functions can make certain performance improvements over the
// classic, similarly named C language functions.  In particular, the 'ltrim'
// and 'trim' functions can change the state of their argument, rather than
// copying data over leading whitespace.  See {Example 1: Trimming Whitespace}.
//
///Character Encoding
///------------------
// These utilities assume ASCII encoding for character data when doing case
// conversions and when determining if a character is in the whitespace
// character set.
//
///Caseless Comparisons
/// - - - - - - - - - -
// Caseless (i.e., case-insensitive) comparisons treat characters in the
// sequence '[a .. z]' as equivalent to the respective characters in the
// sequence '[A .. Z]'.  This equivalence matches that of 'bsl::toupper'.
//
///Whitespace Character Specification
/// - - - - - - - - - - - - - - - - -
// The following characters are classified as "whitespace":
//..
//      Character  Description
//      ---------  ---------------
//      ' '        blank-space
//      '\f'       form-feed
//      '\n'       newline
//      '\r'       carriage return
//      '\t'       horizontal tab
//      '\v'       vertical   tab
//..
// This classification matches that of the 'bsl::isspace'.
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Trimming Whitespace
//- - - - - - - - - - - - - - - -
// Many applications must normalize user input by removing leading and trailing
// whitespace characters to obtain the essential text that is the intended
// input.  Naturally, one would prefer to do this as efficiently as possible.
//
// Suppose the response entered by a user is captured in 'rawInput' below:
//..
//  const char * const rawInput    = "    \t\r\n  Hello, world!    \r\n";
//..
// First, for this pedagogical example, we copy the contents at 'rawInput' for
// later reference:
//..
//  const bsl::string copyRawInput(rawInput);
//..
// Then, we create a 'bslstl::StringRef' object referring to the raw data.
// Given a single argument of 'const char *', the constructor assumes the data
// is a null-terminated string and implicitly calculates the length for the
// reference:
//..
//  bslstl::StringRef text(rawInput);
//
//  assert(rawInput              == text.data());
//  assert(bsl::strlen(rawInput) == text.length());
//..
// Now, we invoke the 'bdlb::StringRefUtil::trim' method to modify 'text' to
// refer to the "Hello, world!" sequence of 'rawInput'.
//..
//  bdlb::StringRefUtil::trim(&text);
//..
// Finally, we observe the results:
//..
//  assert("Hello, world!" == text);          // content compared
//  assert(13              == text.length());
//  assert(rawInput        <  text.data());
//  assert(rawInput        == copyRawInput);  // content compared
//..
// Notice that, as expected, the 'text' object now refers to the "Hello,
// world!" sequence in 'rawInput', while the data at 'rawIput remains
// unchanged.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>  // 'bslstl::StringRef'
#endif

namespace BloombergLP {
namespace bdlb {
                        // ====================
                        // struct StringRefUtil
                        // ====================

struct StringRefUtil {
    // This 'struct' provides a namespace for a suite of functions on
    // 'bslstl::StringRef' references to strings.

    // CLASS METHODS
                        // Comparison

    static bool areEqualCaseless(const bslstl::StringRef& lhsString,
                                 const bslstl::StringRef& rhsString);
        // Compare the specified 'lhsString' and 'rhsString' specified
        // 'lhsLength' and 'rhsLength'.  Return 'true' if 'lhsString' and
        // 'rhsString' are equal up to a case conversion, and 'false'
        // otherwise.  See {Caseless Comparisons}.

    static int lowerCaseCmp(const bslstl::StringRef& lhsString,
                            const bslstl::StringRef& rhsString);
        // Compare the specified 'lhsString' and 'rhsString'.  Return 1 if,
        // after a conversion to lower case, 'lhsString' is greater than
        // 'rhsString', 0 if 'lhsString' and 'rhsString' are equal up to a case
        // conversion, and -1 otherwise.  See {Caseless Comparisons}.

    static int upperCaseCmp(const bslstl::StringRef& lhsString,
                            const bslstl::StringRef& rhsString);
        // Compare the specified 'lhsString' and 'rhsString'.  Return 1 if,
        // after a conversion to upper case, 'lhsString' is greater than
        // 'rhsString', 0 if 'lhsString' and 'rhsString' are equal up to a case
        // conversion, and -1 otherwise.  See {Caseless Comparisons}.

                        // Trim

    static void ltrim(bslstl::StringRef *string);
        // Set the specified 'string' to exclude all leading whitespace
        // characters in 'string' on entry.  The the position of 'string'
        // (i.e., 'string.data()') is set to the first first non-whitespace
        // character in 'string' on entry and 'string' length is reduced by the
        // number of leading whitespace characters.  See {Whitespace Character
        // Specification}.  Note that if 'string' has no non-whitespace
        // characters, the position of 'string' is set to the value of
        // 'string.end()' on entry and its length is set to 0.  Also note that
        // 'string' is both an input and output parameter.

    static void rtrim(bslstl::StringRef *string);
        // Set the specified 'string' to exclude all trailing whitespace
        // characters in 'string' on entry.  The length of 'string' is reduced
        // by the number of trailing whitespace characters.  See {Whitespace
        // Character Specification}.  The position of 'string' (i.e.,
        // 'string.data()' is never changed by this function.  Note that
        // 'string' is both an input and output parameter.

    static void trim(bslstl::StringRef *string);
        // Set the specified 'string' to exclude all leading *and* trailing
        // whitespace characters in 'string' on entry.  The position of string
        // 'string' (i.e.,, 'string.data()') is set to the first non-whitespace
        // character in 'string' on entry and the length of 'string' is reduced
        // by the sum of leading and trailing whitespace characters.  See
        // {Whitespace Character Specification}.  If 'string' on entry consists
        // of all whitespace characters, the position is unchanged but its
        // length is set to zero.  Note that 'string' is both an input and
        // output parameter.

                        // Find 'subString'

    static bslstl::StringRef strstr(const bslstl::StringRef& string,
                                    const bslstl::StringRef& subStr);
        // Return a 'bslstl::StringRef' reference to the first occurence in the
        // specified 'string' at which the specified 'subStr' is found, or
        // 'bslstl::StringRef()' if there is no such occurence.  If 'subStr'
        // has zero length then a zero-length reference to 'string' is returned
        // (i.e., 'bslstl::StringRef(string.data(), 0)');

    static bslstl::StringRef strstrCaseless(const bslstl::StringRef& string,
                                            const bslstl::StringRef& subStr);
        // Return a 'bslstl::StringRef' reference to the first occurence in the
        // specified 'string' at which the specified 'subStr' is found using
        // case-insensitive comparison, or 'bslstl::StringRef()' if there is no
        // such occurence.  If 'subStr' has zero length then a zero-length
        // reference to 'string' is returned (i.e.,
        // 'bslstl::StringRef(string.data(), 0)').  See {Caseless Comparisons}.

    static bslstl::StringRef strrstr(const bslstl::StringRef& string,
                                     const bslstl::StringRef& subStr);
        // Return a 'bslstl::StringRef' reference to the last occurence in the
        // specified 'string' at which the specified 'subStr' is found, or
        // 'bslstl::StringRef()' if there is no such occurence.  If 'subStr'
        // has zero length then a zero-length reference to the end of 'string'
        // is returned (i.e., 'bslstl::StringRef(string.end(), 0)');

    static bslstl::StringRef strrstrCaseless(const bslstl::StringRef& string,
                                             const bslstl::StringRef& subStr);
        // Return a 'bslstl::StringRef' reference to the last occurence in the
        // specified 'string' at which the specified 'subStr' is found using
        // case-insenstive comparison, or 'bslstl::StringRef()' if there is no
        // such occurence.  If 'subStr' has zero length (i.e., then a
        // zero-length reference to the end of 'string' is returned (i.e.,
        // 'bslstl::StringRef(string.end(), 0)').  See {Caseless Comparisons}.
};

// ============================================================================
//                        INLINE DEFINITIONS
// ============================================================================

                        // --------------------
                        // struct StringRefUtil
                        // --------------------

// CLASS METHODS

                        // Comparison

inline
bool StringRefUtil::areEqualCaseless(const bslstl::StringRef& lhsString,
                                     const bslstl::StringRef& rhsString)
{
    if (lhsString.length() != rhsString.length()) {
        return false;                                                 // RETURN
    }

    return 0 == lowerCaseCmp(lhsString, rhsString);
}

                        // Trim

inline
void StringRefUtil::trim(bslstl::StringRef *string)
{
    BSLS_ASSERT_SAFE(string);

    rtrim(string);
    ltrim(string);
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
