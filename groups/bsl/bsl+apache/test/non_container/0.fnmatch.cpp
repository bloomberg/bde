/************************************************************************
 *
 * 0.fnmatch.cpp - tests exercising the rw_fnmatch() helper function
 *
 * $Id: 0.fnmatch.cpp 648752 2008-04-16 17:01:56Z faridz $
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

#include <rw_fnmatch.h>   // for rw_fnmatch()
#include <stdio.h>        // for fprintf(), stderr


#ifdef _RWSTD_OS_LINUX
#  include <fnmatch.h>    // for fnmatch()
#  define FNMATCH(pat, str, arg) fnmatch (pat, str, arg)
#endif   // _RWSTD_OS_LINUX


// the number of failed tests
static int nerrors;

static void
test (int line, int exp, const char *pat, const char *str)
{
    const int result = rw_fnmatch (pat, str, 0);

#ifdef FNMATCH
    const int native = FNMATCH (pat, str, 0);
#else   // if !defined (FNMATCH)
    const int native = exp;
#endif   // FNMATCH

    if (result != native || -1 < exp && result != exp) {

        ++nerrors;

        fprintf (stderr,
                 "%d. rw_fnmatch(\"%s\", \"%s\", 0) ==> %d, "
                 "expected (native:own) %d:%d\n",
                 line, pat, str, result, native, exp);
    }
}


int main ()
{
#define TEST(exp, pat, str) test (__LINE__, exp, pat, str)

    TEST (0, "",  "");
    TEST (1, "",  "a");
    TEST (1, "a", "");

    TEST (0, "a", "a");
    TEST (1, "a", "b");

    TEST (0, "ab", "ab");
    TEST (1, "ab", "ac");

    TEST (0, "abc", "abc");
    TEST (1, "xbc", "abc");
    TEST (1, "ayc", "abc");
    TEST (1, "abz", "abc");

    // exercise escaped characters
    TEST (1, "\\a", "");
    TEST (0, "\\a", "a");
    TEST (1, "\\a", "\\a");
    TEST (1, "\\\\", "");
    TEST (0, "\\\\", "\\");
    TEST (1, "\\?", "\\a");
    TEST (0, "\\?", "?");
    TEST (1, "\\?", "\\?");
    TEST (0, "\\[", "[");
    TEST (1, "\\[", "\\[");
    TEST (0, "\\[a", "[a");
    TEST (0, "\\[a]", "[a]");
    TEST (0, "\\[a\\]", "[a]");
    TEST (0, "\\[\\a\\]", "[a]");

    // exercise the question mark pattern
    TEST (1, "?", "");
    TEST (0, "?", "a");
    TEST (1, "?", "ab");

    TEST (1, "\\?", "");
    TEST (1, "\\?", "a");
    TEST (0, "\\?", "?");

    TEST (0, "a?", "ad");
    TEST (0, "?b", "ab");
    TEST (1, "?b", "ac");
    TEST (0, "??", "ae");
    TEST (1, "??", "aef");

    TEST (0, "?bc", "abc");
    TEST (0, "a?c", "abc");
    TEST (0, "ab?", "abc");

    TEST (0, "??c", "abc");
    TEST (0, "a??", "abc");
    TEST (0, "?b?", "abc");

    TEST (1, "??c", "abz");
    TEST (1, "a??", "xbc");
    TEST (1, "?b?", "ayc");

    TEST (0, "???", "abc");
    TEST (1, "???", "ab");
    TEST (1, "???", "abcd");

    // exercise the asterisk pattern
    TEST (0, "*", "");
    TEST (0, "*", "a");
    TEST (0, "*", "ab");
    TEST (0, "*", "abc");

    TEST (1, "\\*", "");
    TEST (1, "\\*", "a");
    TEST (0, "\\*", "*");
    TEST (1, "\\*", "ab");
    TEST (1, "\\*", "abc");

    TEST (0, "**", "");
    TEST (0, "**", "a");
    TEST (0, "**", "ab");
    TEST (0, "**", "abc");

    TEST (1, "*\\*", "");
    TEST (1, "*\\*", "a");
    TEST (0, "*\\*", "a*");
    TEST (0, "*\\*", "ab*");

    TEST (1, "a*", "");
    TEST (0, "a*", "a");
    TEST (0, "a*", "ab");
    TEST (0, "a*", "abc");

    TEST (1, "a**", "");
    TEST (0, "a**", "a");
    TEST (0, "a**", "ab");
    TEST (0, "a**", "abc");

    TEST (1, "**c", "");
    TEST (1, "**c", "a");
    TEST (1, "**c", "ab");
    TEST (0, "**c", "abc");
    TEST (1, "**c", "abcd");

    TEST (1, "a*z", "");
    TEST (1, "a*z", "a");
    TEST (1, "a*z", "ab");
    TEST (1, "a*z", "abc");
    TEST (0, "a*z", "abcz");
    TEST (0, "a*z", "abcdz");
    TEST (0, "a*z", "abcdez");

    TEST (1, "ab*z", "");
    TEST (1, "ab*z", "a");
    TEST (1, "ab*z", "ab");
    TEST (1, "ab*z", "abc");
    TEST (0, "ab*z", "abcz");
    TEST (0, "ab*z", "abcdz");
    TEST (0, "ab*z", "abcdez");

    TEST (1, "a*yz", "");
    TEST (1, "a*yz", "a");
    TEST (1, "a*yz", "ab");
    TEST (1, "a*yz", "abc");
    TEST (1, "a*yz", "abcz");
    TEST (0, "a*yz", "abcyz");
    TEST (0, "a*yz", "abcdyz");

    TEST (1, "ab*yz", "");
    TEST (1, "ab*yz", "a");
    TEST (1, "ab*yz", "ab");
    TEST (1, "ab*yz", "abc");
    TEST (1, "ab*yz", "abcz");
    TEST (0, "ab*yz", "abcyz");
    TEST (0, "ab*yz", "abcdyz");
    TEST (0, "ab*yz", "abcdeyz");

    TEST (1, "ab*x*z", "");
    TEST (1, "ab*x*z", "a");
    TEST (1, "ab*x*z", "ab");
    TEST (1, "ab*x*z", "abc");
    TEST (1, "ab*x*z", "abcz");
    TEST (1, "ab*x*z", "abcyz");
    TEST (0, "ab*x*z", "abcxyz");
    TEST (0, "ab*x*z", "abcxxyz");
    TEST (0, "ab*x*z", "abcxyyz");

    TEST (1, "*b*x*z", "");
    TEST (1, "*b*x*z", "a");
    TEST (1, "*b*x*z", "ab");
    TEST (1, "*b*x*z", "abc");
    TEST (1, "*b*x*z", "abcz");
    TEST (1, "*b*x*z", "abcyz");
    TEST (0, "*b*x*z", "bcxyz");
    TEST (0, "*b*x*z", "abcxyz");
    TEST (0, "*b*x*z", "aabcxyz");
    TEST (0, "*b*x*z", "abcxxyz");
    TEST (0, "*b*x*z", "abcxyyz");

    TEST (1, "abc*klm*xyz", "");
    TEST (1, "abc*klm*xyz", "a");
    TEST (1, "abc*klm*xyz", "ak");
    TEST (1, "abc*klm*xyz", "akx");

    TEST (1, "abc*klm*xyz", "abkx");
    TEST (1, "abc*klm*xyz", "abklx");
    TEST (1, "abc*klm*xyz", "abklxy");
    TEST (1, "abc*klm*xyz", "abcklxy");
    TEST (1, "abc*klm*xyz", "abcklmxy");
    TEST (0, "abc*klm*xyz", "abcklmxyz");
    TEST (0, "abc*klm*xyz", "abcdklmxyz");
    TEST (0, "abc*klm*xyz", "abcdklmnxyz");

    // exercise simple bracket expressions
    TEST (1, "[a]", "");
    TEST (0, "[a]", "a");
    TEST (1, "[a]", "b");
    TEST (1, "[?]", "c");
    TEST (0, "[?]", "?");
    TEST (1, "[-]", "d");
    TEST (0, "[-]", "-");
    TEST (1, "[]]", "e");
    TEST (0, "[]]", "]");
    TEST (1, "[\\]]", "f");
    TEST (1, "[\\]]", "\\");
    TEST (0, "[\\]]", "]");

    TEST (1, "[\\!a]", "");
    TEST (0, "[\\!a]", "a");
    TEST (1, "[\\!a]", "b");
    TEST (0, "[\\!?]", "!");
    TEST (0, "[\\!?]", "?");
    TEST (0, "[\\!-]", "-");
    TEST (0, "[\\!!]", "!");
    TEST (0, "[\\\\!]", "\\");

    TEST (1, "[ab]", "");
    TEST (0, "[ab]", "a");
    TEST (0, "[ab]", "b");
    TEST (1, "[ab]", "c");

    TEST (1, "[a]b", "");
    TEST (1, "[a]b", "a");
    TEST (0, "[a]b", "ab");
    TEST (1, "[a]b", "b");

    TEST (1, "a[b]", "");
    TEST (1, "a[b]", "a");
    TEST (0, "a[b]", "ab");
    TEST (1, "a[b]", "b");

    TEST (1, "a[bc]", "");
    TEST (1, "a[bc]", "a");
    TEST (0, "a[bc]", "ab");
    TEST (0, "a[bc]", "ac");
    TEST (1, "a[bc]", "abc");
    TEST (1, "a[\\]]", "ab");
    TEST (0, "a[\\]]", "a]");

    TEST (1, "[abc]", "");
    TEST (0, "[abc]", "a");
    TEST (0, "[abc]", "b");
    TEST (0, "[abc]", "c");
    TEST (1, "[abc]", "ab");
    TEST (1, "[abc]", "ac");
    TEST (1, "[abc]", "abc");

    // exercise negated bracket expressions
    TEST (1, "[!a]", "");
    TEST (1, "[!a]", "a");
    TEST (0, "[!a]", "b");
    TEST (1, "[!?]", "?");
    TEST (0, "[!?]", "c");
    TEST (1, "[!-]", "-");
    TEST (0, "[!-]", "d");
    TEST (1, "[!!]", "!");
    TEST (0, "[!!]", "e");

    TEST (1, "[!ab]", "");
    TEST (1, "[!ab]", "a");
    TEST (1, "[!ab]", "b");
    TEST (0, "[!ab]", "c");

    TEST (1, "[!a]b", "");
    TEST (1, "[!a]b", "a");
    TEST (1, "[!a]b", "ab");
    TEST (1, "[!a]b", "b");
    TEST (0, "[!a]b", "bb");

    TEST (1, "a[!b]", "");
    TEST (1, "a[!b]", "a");
    TEST (1, "a[!b]", "ab");
    TEST (1, "a[!b]", "b");
    TEST (0, "a[!b]", "ac");

    TEST (1, "a[!bc]", "");
    TEST (1, "a[!bc]", "a");
    TEST (1, "a[!bc]", "ab");
    TEST (1, "a[!bc]", "ac");
    TEST (0, "a[!bc]", "ad");
    TEST (0, "a[!!?]", "ae");
    TEST (0, "a[!?!]", "af");
    TEST (0, "a[!?-]", "ag");
    TEST (0, "a[!-?]", "ah");
    TEST (1, "a[!bc]", "aae");
    TEST (1, "a[!bc]", "abe");
    TEST (1, "a[!bc]", "ace");

    TEST (1, "[!abc]", "");
    TEST (1, "[!abc]", "a");
    TEST (1, "[!abc]", "b");
    TEST (1, "[!abc]", "c");
    TEST (0, "[!abc]", "d");
    TEST (1, "[!abc]", "ab");
    TEST (1, "[!abc]", "ac");
    TEST (1, "[!abc]", "ad");
    TEST (1, "[!abc]", "abc");
    TEST (1, "[!abc]", "abcd");

    TEST (1, "[!abc]d", "");
    TEST (1, "[!abc]d", "a");
    TEST (1, "[!abc]d", "b");
    TEST (1, "[!abc]d", "c");
    TEST (1, "[!abc]d", "d");
    TEST (0, "[!abc]d", "dd");
    TEST (1, "[!abc]d", "ab");
    TEST (1, "[!abc]d", "ac");
    TEST (1, "[!abc]d", "ad");
    TEST (1, "[!abc]d", "abc");
    TEST (1, "[!abc]d", "abcd");

    // exercise bracket expressions with ranges
    TEST (1, "[a-a]", "");
    TEST (0, "[a-a]", "a");
    TEST (1, "[a-a]", "b");
    TEST (1, "[a-a]", "ab");
    TEST (1, "[a-a]", "ba");

    TEST (1, "[a-b]", "");
    TEST (0, "[a-b]", "a");
    TEST (0, "[a-b]", "b");
    TEST (1, "[a-b]", "c");

    TEST (1, "[a-c]", "");
    TEST (0, "[a-c]", "a");
    TEST (0, "[a-c]", "b");
    TEST (0, "[a-c]", "c");
    TEST (1, "[a-c]", "d");

    TEST (1, "[a-ce]", "");
    TEST (0, "[a-ce]", "a");
    TEST (0, "[a-ce]", "b");
    TEST (0, "[a-ce]", "c");
    TEST (1, "[a-ce]", "d");
    TEST (0, "[a-ce]", "e");
    TEST (1, "[a-ce]", "f");

    TEST (1, "[ac-e]", "");
    TEST (0, "[ac-e]", "a");
    TEST (1, "[ac-e]", "b");
    TEST (0, "[ac-e]", "c");
    TEST (0, "[ac-e]", "d");
    TEST (0, "[ac-e]", "e");
    TEST (1, "[ac-e]", "f");

    TEST (1, "[ac-eg]", "");
    TEST (0, "[ac-eg]", "a");
    TEST (1, "[ac-eg]", "b");
    TEST (0, "[ac-eg]", "c");
    TEST (0, "[ac-eg]", "d");
    TEST (0, "[ac-eg]", "e");
    TEST (1, "[ac-eg]", "f");
    TEST (0, "[ac-eg]", "g");

    TEST (1, "[ac-egk-m]", "");
    TEST (0, "[ac-egk-m]", "a");
    TEST (1, "[ac-egk-m]", "b");
    TEST (0, "[ac-egk-m]", "c");
    TEST (0, "[ac-egk-m]", "d");
    TEST (0, "[ac-egk-m]", "e");
    TEST (1, "[ac-egk-m]", "f");
    TEST (0, "[ac-egk-m]", "g");
    TEST (1, "[ac-egk-m]", "h");
    TEST (1, "[ac-egk-m]", "i");
    TEST (1, "[ac-egk-m]", "j");
    TEST (0, "[ac-egk-m]", "k");
    TEST (0, "[ac-egk-m]", "l");
    TEST (0, "[ac-egk-m]", "m");
    TEST (1, "[ac-egk-m]", "n");

    TEST (1, "[1-9][0-9]", "");
    TEST (1, "[1-9][0-9]", "0");
    TEST (1, "[1-9][0-9]", "1");
    TEST (1, "[1-9][0-9]", "2");
    TEST (1, "[1-9][0-9]", "00");
    TEST (1, "[1-9][0-9]", "01");
    TEST (0, "[1-9][0-9]", "10");
    TEST (0, "[1-9][0-9]", "11");
    TEST (0, "[1-9][0-9]", "12");
    TEST (0, "[1-9][0-9]", "90");
    TEST (0, "[1-9][0-9]", "99");
    TEST (1, "[1-9][0-9]", "999");

    TEST (1, "[1-9].[0-9]", "0.1");
    TEST (0, "[1-9].[0-9]", "1.0");
    TEST (1, "[1-9].[0-9]", "1.0.0");

    // exercise mixed patterns
    TEST (1, "[1-9].[0-9]*", "1.");
    TEST (0, "[1-9].[0-9]*", "1.2");
    TEST (0, "[1-9].[0-9]*", "1.23");
    TEST (0, "[1-9].[0-9]*", "1.2x");
    TEST (0, "[1-9]?[0-9]*", "1-2y");

    TEST (1, "*-aix[1-9].[0-9]-xlc7.0.0.[0-9]", "ppc-aix4.3-xlc6.0");
    TEST (1, "*-aix[1-9].[0-9]-xlc7.0.0.[0-9]", "ppc-aix5.2-xlc6.0");
    TEST (0, "*-aix[1-9].[0-9]-xlc7.0.0.[0-9]", "ppc-aix4.3-xlc7.0.0.0");

    // exercise incomplete patterns
    TEST (1, "\\", "");
    TEST (1, "\\", "a");
    TEST (1, "\\", "\\");
    TEST (1, "[",  "");
    TEST (1, "[",  "a");
    TEST (1, "[",  "[");
    TEST (1, "[a", "");
    TEST (1, "[a", "[a");

    // return 0 on success, 1 on failure
    return !(0 == nerrors);
}
