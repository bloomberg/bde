/************************************************************************
 *
 * fnmatch.cpp - definitions of testsuite helpers
 *
 * $Id: fnmatch.cpp 648752 2008-04-16 17:01:56Z faridz $
 *
 ************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC
#include <rw_fnmatch.h>


typedef unsigned char UChar;


// matches a character against a POSIX bracket expression
static const char*
_rw_bracketmatch (const char *pattern, const unsigned char ch, int)
{
    const char *pc = pattern;

    // negate the match?
    bool neg = false;

    // treat '!' and '^' the same (according to POSIX, the latter has
    // unspecified behavior)
    if ('!' == *pc || '^' == *pc) {
        ++pc;
        neg = true;
    }

    if ('-' == *pc || ']' == *pc) {
        // the '-' and ']' lose their special meaning when they occur
        // first in the bracket expression, after the initial '!', if
        // any
        if (ch == UChar (*pc)) {
            if (neg)
                return 0;

            // skip over the ']'
            if (']' == *pc)
                ++pc;

            // skip over any subsequent characters until the closing
            // (unescaped) ']'
            for ( ; *pc && (*pc != ']' || pc [-1] == '\\'); ++pc);
            return pc;
        }
        else if (!neg)
            return 0;
    }

    unsigned first = 0;
    bool     esc   = false;

    for ( ; *pc; ++pc) {
        if ('\\' == *pc) {
            if (esc) {
                if (ch == UChar (*pc)) {
                    if (neg)
                        return 0;

                    // skip over any subsequent characters until the closing
                    // (unescaped) ']'
                    for ( ; *pc && (*pc != ']' || pc [-1] == '\\'); ++pc);
                    return pc;
                }
            }
            esc = true;
            continue;
        }

        if (']' == *pc && !esc)
            break;

        if ('-' == *pc && !esc)
            first = UChar (pc [-1]);
        else if (first) {
            // check if character is in the current range
            if (first <= ch && ch <= UChar (*pc)) {
                if (neg)
                    return 0;

                // skip over any subsequent characters until the closing
                // (unescaped) ']'
                for ( ; *pc && (*pc != ']' || pc [-1] == '\\'); ++pc);
                return pc;
            }

            first = 0;
        }
        else if (ch == UChar (*pc)) {
            if (neg)
                return 0;

            // skip over any subsequent characters until the closing
            // (unescaped) ']'
            for ( ; *pc && (*pc != ']' || pc [-1] == '\\'); ++pc);
            return pc;
        }
    }

    return neg ? pc : 0;
}


_TEST_EXPORT int
rw_fnmatch (const char *pattern, const char *string, int arg)
{
    _RWSTD_ASSERT (pattern);
    _RWSTD_ASSERT (string);

    const char *next = string;

    bool esc = false;

    for (const char *pc = pattern; ; ++pc) {

        switch (const char c = *pc) {
        case '\0':
            return !!*next;

        case '\\':
            if (esc) {
                if (c != *next)
                    return 1;

                esc = false;
                ++next;
            }
            else
                esc = true;

            break;

        case '*':
            if (esc) {
                if (c != *next)
                    return 1;

                esc = false;

                ++next;
            }
            else {

                // skip any subsequent asterisks as an optimization
                for ( ; '*' == pc [1]; ++pc);

                for ( ; *next; ++next) {
                    // try to match the rest of the string against
                    // the rest of the pattern after the asterisk
                    if (0 == rw_fnmatch (pc + 1, next, arg))
                        return 0;
                }
            }

            break;

        case '?':
            if (esc) {
                if (c != *next)
                    return 1;

                esc = false;
            }

            if ('\0' == *next)
                return 1;

            ++next;
            break;

        case '[':
            if (esc) {
                if (c != *next)
                    return 1;

                esc = false;
            }
            else {
                if ('\0' == *next)
                    return 1;

                pc = _rw_bracketmatch (pc + 1, UChar (*next), arg);
                if (0 == pc)
                    return 1;
            }

            ++next;
            break;

        default:
            if (c != *next)
                return 1;

            esc = false;

            ++next;
        }

        if ('\0' == *next) {
            // reached the end of string: this is a match if the end
            // of the pattern has also been reached or if the rest
            // of the pattern in all asterisks
            if (!esc)
                while ('*' == *++pc);
            return !!*pc;
        }
    }

    return 1;
}
