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
// changed, it cannot change its referent data (the array of bytes).  The
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
//    areEqualCaseless(SR, SR) case-insensitive equality comparison
//        lowerCaseCmp(SR, SR) lexical comparison of lower-case conversion
//        upperCaseCmp(SR, SR) lexical comparison of upper-case conversion
//
//                   ltrim(SR) exclude whitespace from left  side  of string
//                   rtrim(SR) exclude whitespace from right side  of string
//                    trim(SR) exclude whitespace from both  sides of string
//
//  strstr         (SR, SUBSR) find first substring in string
//  strstrCaseless (SR, SUBSR) find first substring in string, case insensitive
//  strrstr        (SR, SUBSR) find last  substring in string
//  strrstrCaseless(SR, SUBSR) find last  substring in string, case insensitive
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
// This classification matches that of 'bsl::isspace'.
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

    static bool areEqualCaseless(const bslstl::StringRef& lhs,
                                 const bslstl::StringRef& rhs);
        // Compare the (referent data of) specified 'lhs' and 'rhs'.  Return
        // 'true' if 'lhs' and 'rhs' are equal up to a case conversion, and
        // 'false' otherwise.  See {Caseless Comparisons}.

    static int lowerCaseCmp(const bslstl::StringRef& lhs,
                            const bslstl::StringRef& rhs);
        // Compare the (referent data of) specified 'lhs' and 'rhs'.  Return 1
        // if, after a conversion to lower case, 'lhs' is greater than 'rhs', 0
        // if 'lhs' and 'rhs' are equal up to a case conversion, and -1
        // otherwise.  See {Caseless Comparisons}.

    static int upperCaseCmp(const bslstl::StringRef& lhs,
                            const bslstl::StringRef& rhs);
        // Compare the (referent data of) specified 'lhs' and 'rhs'.  Return 1
        // if, after a conversion to upper case, 'lhs' is greater than 'rhs', 0
        // if 'lhs' and 'rhs' are equal up to a case conversion, and -1
        // otherwise.  See {Caseless Comparisons}.

                        // Trim

    static bslstl::StringRef ltrim(const bslstl::StringRef& stringRef);
        // Return a 'bslstl::StringRef' object referring to the substring of
        // (the referent data of) 'stringRef' that excludes all leading
        // whitespace.  See {Whitespace Character Specification}.  If
        // 'stringRef' consists entirely of whitespace, return a zero-length
        // reference to the end of 'stringRef' (i.e.,
        // 'bslstl::StringRef(stringRef.end(), 0)').

    static bslstl::StringRef rtrim(const bslstl::StringRef& stringRef);
        // Return a 'bslstl::StringRef' object referring to the substring of
        // (the referent data of) 'stringRef' that excludes all trailing
        // whitespace.  See {Whitespace Character Specification}.  If
        // 'stringRef' consists entirely of whitespace, return a zero-length
        // reference to 'stringRef' (i.e.,
        // 'bslstl::StringRef(stringRef.data(), 0)').

    static bslstl::StringRef trim(const bslstl::StringRef& stringRef);
        // Return a 'bslstl::StringRef' object referring to the substring of
        // (the referent data of) 'stringRef' that excludes all leading and
        // trailing whitespace.  See {Whitespace Character Specification}.  If
        // 'stringRef' consists entirely of whitespace, return a zero-length
        // reference to 'stringRef' (i.e.,
        // 'bslstl::StringRef(stringRef.data(), 0)').

                        // Find 'subString'

    static bslstl::StringRef strstr(const bslstl::StringRef&    stringRef,
                                    const bslstl::StringRef& subStringRef);
        // Return a 'bslstl::StringRef' object referring to the first
        // occurrence in (the referent data of) the specified 'stringRef' at
        // which (the referent data of) the specified 'subStringRef' is found,
        // or 'bslstl::StringRef()' if there is no such occurrence.  If
        // 'subStringRef' has zero length then a zero-length reference to
        // the beginning of 'stringRef' is returned (i.e.,
        // 'bslstl::StringRef(stringRef.data(), 0)');

    static bslstl::StringRef strstrCaseless(
                                        const bslstl::StringRef&    stringRef,
                                        const bslstl::StringRef& subStringRef);
        // Return a 'bslstl::StringRef' object referring to the first
        // occurrence in (the referent data of) the specified 'stringRef' at
        // which (the referent data of) the specified 'subStringRef' is found
        // using case-insensitive comparisons, or 'bslstl::StringRef()' if
        // there is no such occurrence.  See {Caseless Comparisons}.  If
        // 'subStringRef' has zero length then a zero-length reference to
        // the beginning of 'stringRef' is returned (i.e.,
        // 'bslstl::StringRef(stringRef.data(), 0)');
     

    static bslstl::StringRef strrstr(const bslstl::StringRef&    stringRef,
                                     const bslstl::StringRef& subStringRef);
        // Return a 'bslstl::StringRef' object referring to the last
        // occurrence in (the referent data of) the specified 'stringRef' at
        // which (the referent data of) the specified 'subStringRef' is found,
        // or 'bslstl::StringRef()' if there is no such occurrence.  If
        // 'subStringRef' has zero length then a zero-length reference to
        // the end of 'stringRef' is returned (i.e.,
        // 'bslstl::StringRef(stringRef.end(), 0)');

    static bslstl::StringRef strrstrCaseless(const bslstl::StringRef& string,
                                             const bslstl::StringRef& subStr);
        // Return a 'bslstl::StringRef' object referring to the last
        // occurrence in (the referent data of) the specified 'stringRef' at
        // which (the referent data of) the specified 'subStringRef' is found
        // using case-insensitive comparisons, or 'bslstl::StringRef()' if
        // there is no such occurrence.  See {Caseless Comparisons}.  If
        // 'subStringRef' has zero length then a zero-length reference to
        // the end of 'stringRef' is returned (i.e.,
        // 'bslstl::StringRef(stringRef.end(), 0)');
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
bool StringRefUtil::areEqualCaseless(const bslstl::StringRef& lhs,
                                     const bslstl::StringRef& rhs)
{
    if (lhs.length() != rhs.length()) {
        return false;                                                 // RETURN
    }

    return 0 == lowerCaseCmp(lhs, rhs);
}

                        // Trim

inline
bslstl::StringRef StringRefUtil::trim(const bslstl::StringRef& stringRef);
{
    return ltrim(rtrim(stringRef));
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
