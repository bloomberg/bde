// ball_patternutil.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_patternutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_patternutil_cpp,"$Id$ $CSID$")

namespace BloombergLP {

namespace ball {

// ----------------------------------------------------------------------------
//                   State Transition Diagram for 'isValidPattern'
// ----------------------------------------------------------------------------
// S0: start state
// S1: saw '\'
// S2: saw '*'
// ----------------------------------------------------------------------------
//
//                          '\' or '*'
//                      ____________________
//                     |                    |
//                    _V__                 _|__
//              -----|    |     '\'       |    |
//      others |     | S0 |-------------->| S1 |
//              ---->|____|               |____|
//                     | |                   |
//      _______________| |                   | not '\' and
//     |    NULL         | '*'               | not '*'
//     |                 |                   |
//  ___V____             |               ____V____
// | ______ |           _V__            | _______ |
// ||      ||  NULL    |    | not NULL  ||       ||
// || true ||<---------| S2 |---------->|| false ||
// ||______||          |____|           ||_______||
// |________|                           |_________|
//
//
// ----------------------------------------------------------------------------

bool PatternUtil::isValidPattern(const char *pattern)
{
    while (*pattern) {
        if ('\\' == *pattern) {
            ++pattern;
            if ('\\' != *pattern && '*' != *pattern) {
                return false;                                         // RETURN
            }
        } else if ('*' == *pattern) {
            return 0 == pattern[1];                                   // RETURN
        }
        ++pattern;
    }
    return true;
}

// ----------------------------------------------------------------------------
//                   State Transition Diagram for 'isMatch'
// ----------------------------------------------------------------------------
// Notation:
//  P means the current input character in the specified 'pattern'.  S means
//  the current input character in the specifying 'inputString'.  Note that the
//  action of advancing to the next input character is implied (i.e.,
//  automatically performed when the current input character is read to reach
//  a new state).
//-----------------------------------------------------------------------------
//..
//
//                            P != '\' && P != '*"
//                           && P != '\0' && P == S
//                                __
//                               |  |   (P == '*' && S == '*') ||
//                               |  |   (P == '\\' && S == '\\')
//     ____                     _V__|_                 ____
//    |    |    P == '*'       |      |<--------------|    |
//    | S2 |<------------------|  S0  |               | S1 |
//    |____|                   |______|-------------->|____|
//     |  |  P == '\0'           |  |    P == '\\'     |
//     |  |_______               |  |                  | (P != '*' && P != '\')
//     |          |   P == '\0'  |  | P != '\'         | || P != S
//     |          | && S == '\0' |  | && P != '*'      |
//     |          |              |  | && P != S        |
//     | P != '0' |   __________ |  |___________     __|
//     |          |  |                          |   |
//     |        __V__V__                      __V___V__
//     |       | ______ |                    | _______ |
//     |       ||      ||                    ||       ||
//     |       || true ||                    || false ||
//     |       ||______||                    ||_______||
//     |       |________|                    |_________|
//     |                                         ^
//     |_________________________________________|
//
//..
// ----------------------------------------------------------------------------

bool PatternUtil::isMatch(const char *inputString, const char *pattern)
{
    char c;
    while (*pattern && *inputString) {
        if ('\\' == *pattern) {
            ++pattern;
            if ('\\' == *pattern) {
                c = '\\';
            }
            else if ('*' == *pattern) {
                c = '*';
            }
            else {
                return false;                                         // RETURN
            }
        }
        else if ('*' == *pattern) {
            return 0 == pattern[1];                                   // RETURN
        }
        else {
            c = *pattern;
        }
        if (c != *inputString) {
            return false;                                             // RETURN
        }
        ++inputString;
        ++pattern;
    }

    return ( 0  == *pattern && 0 == *inputString)
        || ('*' == *pattern && 0 == pattern[1]);
}

}  // close package namespace

}  // close enterprise namespace

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
