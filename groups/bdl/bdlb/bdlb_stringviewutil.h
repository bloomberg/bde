// bdlb_stringviewutil.h                                              -*-C++-*-
#ifndef INCLUDED_BDLB_STRINGVIEWUTIL
#define INCLUDED_BDLB_STRINGVIEWUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utility functions on 'bsl::string_view' containers.
//
//@CLASSES:
//  bdlb::StringViewUtil: namespace for functions on 'string_view' containers
//
//@SEE_ALSO: bslstl_stringview
//
//@DESCRIPTION: This component defines a utility 'struct',
// 'bdlb::StringViewUtil', that provides a suite of functions that operate on
// 'bsl::string_view' containers.
//
///Synopsis of 'bsl::string_view'
///------------------------------
// The 'bsl::string_view' class provides 'bsl::string'-like access to an array
// of bytes that need not be null terminated and that can have non-ASCII values
// (i.e., '[128 .. 255]').  Although a 'bsl::string_view' object can itself be
// changed, it cannot change its referent data (the array of bytes).  The
// lifetime of the referent data must exceed that of all 'bsl::string_view'
// objects referring to it.  Equality comparison of 'bsl::string_view' objects
// compares the content of the referent data (not whether or not the object
// refer to the same array of bytes).  See {'bslstl_stringview'} for full
// details.
//
///Function Synopsis
///-----------------
// The table below provides an outline of the functions provided by this
// component.
//..
//  Function                   Purpose
//  -------------------------- ------------------------------------------------
//  areEqualCaseless(SV, SV)   case-insensitive equality comparison
//     lowerCaseCmp (SV, SV)   lexical comparison of lower-case conversion
//     upperCaseCmp (SV, SV)   lexical comparison of upper-case conversion
//
//  ltrim(SV)                  exclude whitespace from left  side  of string
//  rtrim(SV)                  exclude whitespace from right side  of string
//   trim(SV)                  exclude whitespace from both  sides of string
//
//  substr(SV, pos, num)       substring, 'num' characters from 'pos'
//
//  strstr         (SV, SUBSV) find first substring in string
//  strstrCaseless (SV, SUBSV) find first substring in string, case insensitive
//  strrstr        (SV, SUBSV) find last  substring in string
//  strrstrCaseless(SV, SUBSV) find last  substring in string, case insensitive
//
//  findFirstOf   (SV, ch, p)  find first occurrence of any character from 'ch'
//  findLastOf    (SV, ch, p)  find last  occurrence of any character from 'ch'
//  findFirstNotOf(SV, ch, p)  find first occurrence of any char  not from 'ch'
//  findLastNotOf (SV, ch, p)  find last  occurrence of any char  not from 'ch'
//
//  startsWith(SV, ch)         find out if string starts with 'ch'
//    endsWith(SV, ch)         find out if string ends   with 'ch'
//..
// Since 'bsl::string_view' objects know the length of the referent data these
// utility functions can make certain performance improvements over the
// classic, similarly named C language functions.
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
/// - - - - - - - - - - - - - - -
// Many applications must normalize user input by removing leading and trailing
// whitespace characters to obtain the essential text that is the intended
// input.  Naturally, one would prefer to do this as efficiently as possible.
//
// Suppose the response entered by a user is captured in 'rawInput' below:
//..
//  const char * const rawInput    = "    \t\r\n  Hello, world!    \r\n";
//                                  //1234 5 6 789             1234 5 6
//                                  //            123456789ABCD
//                                  // Note lengths of whitespace and
//                                  // non-whitespace substrings for later.
//..
// First, for this pedagogical example, we copy the contents at 'rawInput' for
// later reference:
//..
//  const bsl::string copyRawInput(rawInput);
//..
// Then, we create a 'bsl::string_view' object referring to the raw data.
// Given a single argument of 'const char *', the constructor assumes the data
// is a null-terminated string and implicitly calculates the length for the
// reference:
//..
//  bsl::string_view text(rawInput);
//
//  assert(rawInput   == text.data());
//  assert(9 + 13 + 6 == text.length());
//..
// Now, we invoke the 'bdlb::StringViewUtil::trim' method to find the "Hello,
// world!" sequence in 'rawInput'.
//..
//  bsl::string_view textOfInterest = bdlb::StringViewUtil::trim(text);
//..
// Finally, we observe the results:
//..
//  assert(bsl::string_view("Hello, world!") == textOfInterest);
//  assert(13                                == textOfInterest.length());
//
//  assert(text.data()   + 9                 == textOfInterest.data());
//  assert(text.length() - 9 - 6             == textOfInterest.length());
//
//  assert(rawInput                          == copyRawInput);
//..
// Notice that, as expected, the 'textOfInterest' object refers to the "Hello,
// world!" sub-sequence within the 'rawInput' byte array while the data at
// 'rawInput' remains *unchanged*.

#include <bdlscm_version.h>

#include <bsls_review.h>

#include <bsl_algorithm.h>    // bsl::min
#include <bsl_string_view.h>

namespace BloombergLP {
namespace bdlb {
                        // =====================
                        // struct StringViewUtil
                        // =====================

struct StringViewUtil {
    // This 'struct' provides a namespace for a suite of functions on
    // 'bsl::string_view' containers.

    // PUBLIC TYPES
    typedef bsl::string_view::size_type size_type;
        // Size type of string_view containers.

    // PUBLIC CLASS DATA
    static const size_type k_NPOS = bsl::string_view::npos;
        // Value used to denote "not-a-position", guaranteed to be outside the
        // 'range[0 .. bsl::string_view::max_size()]'.

    // CLASS METHODS
                        // Comparison

    static bool areEqualCaseless(const bsl::string_view& lhs,
                                 const bsl::string_view& rhs);
        // Compare (the referent data of) the specified 'lhs' and 'rhs'.
        // Return 'true' if 'lhs' and 'rhs' are equal up to a case conversion,
        // and 'false' otherwise.  See {Caseless Comparisons}.

    static int lowerCaseCmp(const bsl::string_view& lhs,
                            const bsl::string_view& rhs);
        // Compare (the referent data of) the specified 'lhs' and 'rhs'.
        // Return 1 if, after a conversion to lower case, 'lhs' is greater than
        // 'rhs', 0 if 'lhs' and 'rhs' are equal up to a case conversion, and
        // -1 otherwise.  See {Caseless Comparisons}.

    static int upperCaseCmp(const bsl::string_view& lhs,
                            const bsl::string_view& rhs);
        // Compare (the referent data of) the specified 'lhs' and 'rhs'.
        // Return 1 if, after a conversion to upper case, 'lhs' is greater than
        // 'rhs', 0 if 'lhs' and 'rhs' are equal up to a case conversion, and
        // -1 otherwise.  See {Caseless Comparisons}.

                        // Trim

    static bsl::string_view ltrim(const bsl::string_view& string);
        // Return a 'bsl::string_view' object referring to the substring of
        // (the referent data of) the specified 'string' that excludes all
        // leading whitespace.  See {Whitespace Character Specification}.  If
        // 'string' consists entirely of whitespace, return a zero-length
        // reference to the end of 'string' (i.e.,
        // 'bsl::string_view(string.end(), 0)').

    static bsl::string_view rtrim(const bsl::string_view& string);
        // Return a 'bsl::string_view' object referring to the substring of
        // (the referent data of) the specified 'string' that excludes all
        // trailing whitespace.  See {Whitespace Character Specification}.  If
        // 'string' consists entirely of whitespace, return a zero-length
        // reference to the beginning of (the referent data of) 'string'
        // (i.e., 'bsl::string_view(string.data(), 0)').

    static bsl::string_view trim(const bsl::string_view& string);
        // Return a 'bsl::string_view' object referring to the substring of
        // (the referent data of) the specified 'string' that excludes all
        // leading and trailing whitespace.  See {Whitespace Character
        // Specification}.  If 'string' consists entirely of whitespace, return
        // a zero-length reference to the beginning of (the referent data of)
        // 'string' (i.e., 'bsl::string_view(string.data(), 0)').

                        // Create 'subString'

    static bsl::string_view substr(const bsl::string_view& string,
                                   size_type               position = 0,
                                   size_type               numChars = k_NPOS);
        // Return a string whose value is the substring starting at the
        // optionally specified 'position' in the specified 'string', of length
        // the optionally specified 'numChars' or 'length() - position',
        // whichever is smaller.  If 'position' is not specified, 0 is used
        // (i.e., the substring is from the beginning of this string).  If
        // 'numChars' is not specified, 'k_NPOS' is used (i.e., the entire
        // suffix from 'position' to the end of the string is returned).  The
        // behavior is undefined unless 'position' is within the string
        // boundaries ('0 <= position <= string.length()').

                         // Find 'subString'

    static bsl::string_view strstr(const bsl::string_view& string,
                                   const bsl::string_view& subString);
        // Return a 'bsl::string_view' object referring to the first occurrence
        // in (the referent data of) the specified 'string' at which (the
        // referent data of) the specified 'subString' is found, or
        // 'bsl::string_view()' if there is no such occurrence.  If 'subString'
        // has zero length then a zero-length reference to the beginning of
        // 'string' is returned (i.e., 'bsl::string_view(string.data(), 0)');

    static bsl::string_view strstrCaseless(const bsl::string_view& string,
                                           const bsl::string_view& subString);
        // Return a 'bsl::string_view' object referring to the first occurrence
        // in (the referent data of) the specified 'string' at which (the
        // referent data of) the specified 'subString' is found using
        // case-insensitive comparisons, or 'bsl::string_view()' if there is no
        // such occurrence.  See {Caseless Comparisons}.  If 'subString' has
        // zero length then a zero-length reference to the beginning of
        // 'string' is returned (i.e., 'bsl::string_view(string.data(), 0)');

    static bsl::string_view strrstr(const bsl::string_view& string,
                                    const bsl::string_view& subString);
        // Return a 'bsl::string_view' object referring to the last occurrence
        // in (the referent data of) the specified 'string' at which (the
        // referent data of) the specified 'subString' is found, or
        // 'bsl::string_view()' if there is no such occurrence.  If 'subString'
        // has zero length then a zero-length reference to the end of 'string'
        // is returned (i.e., 'bsl::string_view(string.end(), 0)');

    static bsl::string_view strrstrCaseless(const bsl::string_view& string,
                                            const bsl::string_view& subString);
        // Return a 'bsl::string_view' object referring to the last occurrence
        // in (the referent data of) the specified 'string' at which (the
        // referent data of) the specified 'subString' is found using
        // case-insensitive comparisons, or 'bsl::string_view()' if there is no
        // such occurrence.  See {Caseless Comparisons}.  If 'subString' has
        // zero length then a zero-length reference to the end of 'string' is
        // returned (i.e., 'bsl::string_view(string.end(), 0)');

                      // Find first/last of/not of

    static size_type findFirstOf(const bsl::string_view& string,
                                 const bsl::string_view& characters,
                                 size_type               position = 0);
        // Return the position of the *first* occurrence of a character
        // belonging to the specified 'characters', if such an occurrence can
        // can be found in the specified 'string' (on or *after* the optionally
        // specified 'position' if such a 'position' is specified), and return
        // 'k_NPOS' otherwise.

    static size_type findLastOf(const bsl::string_view& string,
                                const bsl::string_view& characters,
                                size_type               position = k_NPOS);
        // Return the position of the *last* occurrence of a character
        // belonging to the specified 'characters', if such an occurrence can
        // can be found in the specified 'string' (on or *before* the
        // optionally specified 'position' if such a 'position' is specified),
        // and return 'k_NPOS' otherwise.

    static size_type findFirstNotOf(const bsl::string_view& string,
                                    const bsl::string_view& characters,
                                    size_type               position = 0);
        // Return the position of the *first* occurrence of a character *not*
        // belonging to the specified 'characters', if such an occurrence can
        // be found in the specified 'string' (on or *after* the optionally
        // specified 'position' if such a 'position' is specified), and return
        // 'k_NPOS' otherwise.

    static size_type findLastNotOf(const bsl::string_view& string,
                                   const bsl::string_view& characters,
                                   size_type               position = k_NPOS);
        // Return the position of the *last* occurrence of a character *not*
        // belonging to the specified 'characters', if such an occurrence can
        // be found in the specified 'string' (on or *before* the optionally
        // specified 'position' if such a 'position' is specified), and return
        // 'k_NPOS' otherwise.

                        // Starts/ends with

    static bool startsWith(const bsl::string_view&  string,
                           const bsl::string_view&  characters);
    static bool startsWith(const bsl::string_view&  string,
                           const char              *characters);
        // Return 'true' if the specified 'string' begins with the specified
        // 'characters', and 'false' otherwise.

    static bool startsWith(const bsl::string_view& string, char character);
        // Return 'true' if the specified 'string' begins with the specified
        // 'character', and 'false' otherwise.

    static bool endsWith(const bsl::string_view&  string,
                         const bsl::string_view&  characters);
    static bool endsWith(const bsl::string_view&  string,
                         const char              *characters);
        // Return 'true' if the specified 'string' ends with the specified
        // 'characters', and 'false' otherwise.

    static bool endsWith(const bsl::string_view& string, char character);
        // Return 'true' if the specified 'string' ends with the specified
        // 'character', and 'false' otherwise.
};

// ============================================================================
//                        INLINE DEFINITIONS
// ============================================================================

                        // ---------------------
                        // struct StringViewUtil
                        // ---------------------

// CLASS METHODS

                        // Comparison

inline
bool StringViewUtil::areEqualCaseless(const bsl::string_view& lhs,
                                      const bsl::string_view& rhs)
{
    if (lhs.length() != rhs.length()) {
        return false;                                                 // RETURN
    }

    return 0 == lowerCaseCmp(lhs, rhs);
}

                        // Trim

inline
bsl::string_view StringViewUtil::trim(const bsl::string_view& string)
{
    return ltrim(rtrim(string));
}

                        // Substring

inline
bsl::string_view StringViewUtil::substr(const bsl::string_view& string,
                                        size_type               position,
                                        size_type               numChars)
{
    BSLS_ASSERT(position <= string.length());

    return bsl::string_view(string.data() + position,
                            bsl::min(numChars, string.length() - position));
}

                        // Starts/ends with

inline
bool StringViewUtil::startsWith(const bsl::string_view& string,
                                const bsl::string_view& characters)
{
    if (characters.length() > string.length()) {
        return false;                                                 // RETURN
    }
    return characters == bsl::string_view(string.data(), characters.length());
}

inline
bool StringViewUtil::startsWith(const bsl::string_view&  string,
                                const char              *characters)
{
    BSLS_ASSERT_SAFE(characters);

    const char                       *nextChar = characters;
    bsl::string_view::const_iterator  stringIt = string.begin();
    while (stringIt != string.end()) {
        if (0 == *nextChar) {
            return true;                                              // RETURN
        }
        if (*nextChar != *stringIt) {
            return false;                                             // RETURN
        }
        ++stringIt;
        ++nextChar;
    }
    return 0 == *nextChar;
}

inline
bool StringViewUtil::startsWith(const bsl::string_view& string, char character)
{
    return (!string.empty() && character == string.front());
}

inline
bool StringViewUtil::endsWith(const bsl::string_view& string,
                              const bsl::string_view& characters)
{
    if (string.length() < characters.length()) {
        return false;                                                 // RETURN
    }

    bsl::string_view::size_type pos = string.length() - characters.length();
    return 0 == bsl::string_view::traits_type::compare(string.data() + pos,
                                                       characters.data(),
                                                       characters.length());
}

inline
bool StringViewUtil::endsWith(const bsl::string_view&  string,
                              const char              *characters)
{
    BSLS_ASSERT_SAFE(characters);

    return endsWith(string, bsl::string_view(characters));
}

inline
bool StringViewUtil::endsWith(const bsl::string_view& string, char character)
{
    return (!string.empty() && character == string.back());
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
