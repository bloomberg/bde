// bael_patternutil.h           -*-C++-*-
#ifndef INCLUDED_BAEL_PATTERNUTIL
#define INCLUDED_BAEL_PATTERNUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")


//@PURPOSE: Provide a utility class for string pattern matching.
//
//@CLASSES:
//        bael_PatternUtil: utility class for string pattern matching
//
//@AUTHOR: Gang Chen (gchen20)
//
//@DESCRIPTION: This component provides utility functions for matching input
// strings to a given pattern based on wild-card and simple escape sequences.
//
///Usage
///-----
// The following code fragments illustrate basic usage of this component's
// utility functions.
//
// A string matches a pattern if they are identical:
//..
//     assert(bael_PatternUtil::isMatch("EQ",           "EQ"));
//..
// A string matches a pattern containing an (unescaped) trailing '*' if that
// pattern (without the trailing '*') is a prefix of the string:
//..
//     assert(bael_PatternUtil::isMatch("EQ.MARKET",    "EQ*"));
//     assert(bael_PatternUtil::isMatch("EQ",           "EQ*"));
//..
// An escaped '*' at the end loses its wild-card semantics and matches a single
// '*':
//..
//     assert(false == bael_PatternUtil::isMatch("EQ.MARKET", "EQ\\*"));
//     assert(bael_PatternUtil::isMatch("EQ*",          "EQ\\*"));
//..
// Escape sequences include '\\' and '\*' only and they can appear anywhere in
// the pattern:
//..
//     assert(bael_PatternUtil::isMatch("\\EQ",         "\\\\EQ"));
//     assert(bael_PatternUtil::isMatch("E*Q",          "E\\*Q"));
//..
// A pattern is invalid if it contains a non-trailing '*', or any '\' that is
// not followed by either '\' or '*'.  The 'isValidPattern' function can be
// used to determine whether or not a pattern is valid:
//..
//     assert(false == bael_PatternUtil::isValidPattern("E\\Q"));
//     assert(false == bael_PatternUtil::isValidPattern("E*Q"));
//     assert(true  == bael_PatternUtil::isValidPattern("E\\\\Q"));
//     assert(true  == bael_PatternUtil::isValidPattern("E\\*Q"));
//..
// The 'isMatch' function always returns 'false' on an invalid pattern:
//..
//     assert(false == bael_PatternUtil::isMatch("E\\Q","E\\Q"));
//     assert(false == bael_PatternUtil::isMatch("E*Q", "E*Q"));
//     assert(false == bael_PatternUtil::isMatch("ETQ", "E*Q"));
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

namespace BloombergLP {

                        // ======================
                        // class bael_PatternUtil
                        // ======================

struct bael_PatternUtil {
    // This utility class provides functions relating to pattern matching for
    // strings.

    // CLASS METHODS
    static bool isMatch(const char *inputString,
                        const char *pattern);
        // Return 'true' if the specified 'pattern' matches the specified
        // 'inputString', and 'false' if the pattern does not match or is
        // invalid.  There are two types of escape sequences that are allowed
        // in 'pattern'.  (See the function-level documentation of
        // 'bael_PatternUtil::isValidPattern' for the definition of invalid
        // patterns.)  A '\*' escape sequence in 'pattern' matches a single
        // '*' in 'inputString'.  A '\\' escape sequence in 'pattern' matches
        // a single '\' in 'inputString'.  If 'pattern' ends with an unescaped
        // '*', then 'pattern' matches 'inputString' if the string indicated
        // by 'pattern' (after escape sequence processing) with the final '*'
        // removed is a prefix of 'inputString'.  Otherwise 'pattern' matches
        // 'inputString' only if the string indicated by 'pattern' (after
        // escape sequence processing) and 'inputString' are the same.  The
        // behavior is undefined unless both 'inputString' and 'pattern' are
        // null-terminated c-style strings.

    static bool isValidPattern(const char *pattern);
        // Return 'true' if the specified 'pattern' does not contain a '\' not
        // followed by either '\' or '*', or an unescaped '*' at locations
        // other than at the end, and 'false' otherwise.  Note that an
        // unescaped '*' not at the end may someday be considered a valid
        // pattern.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
