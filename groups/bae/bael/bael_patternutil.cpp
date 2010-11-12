// bael_patternutil.cpp                 -*-C++-*-
#include <bael_patternutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_patternutil_cpp,"$Id$ $CSID$")

namespace BloombergLP {

// ---------------------------------------------------------------------------
//                   State Transition Diagram for 'isValidPattern'
// ---------------------------------------------------------------------------
// S0: start state
// S1: saw '\'
// S2: saw '*'
// ---------------------------------------------------------------------------
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
// ---------------------------------------------------------------------------

bool bael_PatternUtil::isValidPattern(const char *pattern)
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

// ---------------------------------------------------------------------------
//                   State Transition Diagram for 'isMatch'
// ---------------------------------------------------------------------------
// Notation:
//  P means the current input character in the specified 'pattern'.  S means
//  the current input character in the specifying 'inputString'.  Note that the
//  action of advancing to the next input character is implied (i.e.,
//  automatically performed when the current input character is read to reach
//  a new state).
//----------------------------------------------------------------------------
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
// ---------------------------------------------------------------------------

bool bael_PatternUtil::isMatch(const char *inputString,
                               const char *pattern)
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

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
