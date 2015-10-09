// ball_patternutil.h                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_PATTERNUTIL
#define INCLUDED_BALL_PATTERNUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")


//@PURPOSE: Provide a utility class for string pattern matching.
//
//@CLASSES:
//  ball::PatternUtil: utility class for string pattern matching
//
//@DESCRIPTION: This component defines a namespace, 'ball::PatternUtil', that
// provides utility functions for matching input strings to a given pattern
// based on wild-card and simple escape sequences.
//
///Usage
///-----
// The following code fragments illustrate basic usage of this component's
// utility functions.
//
// A string matches a pattern if they are identical:
//..
//     assert(ball::PatternUtil::isMatch("EQ",           "EQ"));
//..
// A string matches a pattern containing an (unescaped) trailing '*' if that
// pattern (without the trailing '*') is a prefix of the string:
//..
//     assert(ball::PatternUtil::isMatch("EQ.MARKET",    "EQ*"));
//     assert(ball::PatternUtil::isMatch("EQ",           "EQ*"));
//..
// An escaped '*' at the end loses its wild-card semantics and matches a single
// '*':
//..
//     assert(false == ball::PatternUtil::isMatch("EQ.MARKET", "EQ\\*"));
//     assert(ball::PatternUtil::isMatch("EQ*",          "EQ\\*"));
//..
// Escape sequences include '\\' and '\*' only and they can appear anywhere in
// the pattern:
//..
//     assert(ball::PatternUtil::isMatch("\\EQ",         "\\\\EQ"));
//     assert(ball::PatternUtil::isMatch("E*Q",          "E\\*Q"));
//..
// A pattern is invalid if it contains a non-trailing '*', or any '\' that is
// not followed by either '\' or '*'.  The 'isValidPattern' function can be
// used to determine whether or not a pattern is valid:
//..
//     assert(false == ball::PatternUtil::isValidPattern("E\\Q"));
//     assert(false == ball::PatternUtil::isValidPattern("E*Q"));
//     assert(true  == ball::PatternUtil::isValidPattern("E\\\\Q"));
//     assert(true  == ball::PatternUtil::isValidPattern("E\\*Q"));
//..
// The 'isMatch' function always returns 'false' on an invalid pattern:
//..
//     assert(false == ball::PatternUtil::isMatch("E\\Q","E\\Q"));
//     assert(false == ball::PatternUtil::isMatch("E*Q", "E*Q"));
//     assert(false == ball::PatternUtil::isMatch("ETQ", "E*Q"));
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

namespace BloombergLP {

namespace ball {
                        // =================
                        // class PatternUtil
                        // =================

struct PatternUtil {
    // This utility class provides functions relating to pattern matching for
    // strings.

    // CLASS METHODS
    static bool isMatch(const char *inputString, const char *pattern);
        // Return 'true' if the specified 'pattern' matches the specified
        // 'inputString', and 'false' if the pattern does not match or is
        // invalid.  There are two types of escape sequences that are allowed
        // in 'pattern'.  (See the function-level documentation of
        // 'PatternUtil::isValidPattern' for the definition of invalid
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
