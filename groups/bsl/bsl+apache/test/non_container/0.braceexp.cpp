/************************************************************************
 *
 * 0.braceexp.cpp - tests exercising the rw_brace_expand()
 *                  and rw_shell_expand() helper functions
 *
 * $Id: 0.braceexp.cpp 648752 2008-04-16 17:01:56Z faridz $
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

#include <rw_braceexp.h>
#include <environ.h>      // for rw_putenv()

#include <stdio.h>        // for fprintf(), size_t, stderr
#include <stdlib.h>       // for free()
#include <string.h>       // for strcmp()

// the number of failed tests
static int nerrors;

static void
test (int line, const char* brace_expr, size_t n, const char* expect,
      const char* fname,
      char* (fn)(const char*, size_t, char*, size_t, char))
{
    char buf [128];

    char* result = fn (brace_expr, n, buf, sizeof (buf), ' ');

    const bool equal =   (expect && result)
                       ? !strcmp (expect, result)
                       : expect == result;

    if (!equal) {

        ++nerrors;

        fprintf (stderr,
                 "%d. %s(\"%s\", ...) failed. "
                 "expected \"%s\" got \"%s\"\n",
                 line, fname, brace_expr,
                 expect ? expect : "(null)",
                 result ? result : "(null)");
    }

    if (result != buf)
        free (result);
}


#define DO_TEST(s,e,f) test (__LINE__, s, strlen (s), e, #f, f)
#define TEST_COMMON(fn) run_tests (#fn, fn)

static void
run_tests (const char* fname,
           char* (fn)(const char*, size_t, char*, size_t, char))
{
#undef TEST
#define TEST(s,e) test (__LINE__, s, strlen (s), e, fname, fn)

    // run our tests
    TEST ("", "");

    // test plain and escaped whitespace
    TEST ("\\ ",             " ");
    TEST ("\\ \\ ",          "  ");
    TEST ("\\ \\\t\\ ",      " \t ");
    TEST ("a\\ b",           "a b");
    TEST ("a b",             "a b");

    TEST ("a", "a");
    TEST ("a\\b", "ab");

    TEST ("{",     0);
    TEST ("}",     "}");
    TEST ("{}",    "");

    TEST ("}{",    0);
    TEST ("}{}",   "}");
    TEST ("}{}{",  0);
    TEST ("{{",    0);
    TEST ("}}",    "}}");

    TEST ("{0}"  , "0");
    TEST ("\\{0}", "{0}");
    TEST ("{\\0}", "0");
    TEST ("{0\\}", 0);

    TEST ("{\\{}", "{");
    TEST ("{\\}}", "}");

    TEST ("a{0}",            "a0");
    TEST ("a{0}b",           "a0b");
    TEST ("a\\{0\\}b",       "a{0}b");
    TEST ("a\\{0,123,4\\}b", "a{0,123,4}b");

    // extension: bash sequence expansion
    TEST ("{0..a}", "0..a");
    TEST ("{a..0}", "a..0");

    TEST ("{0..0}",   "0");
    TEST ("{0..5}",   "0 1 2 3 4 5");
    TEST ("{4..2}",   "4 3 2");
    TEST ("{+5..6}",  "5 6");
    TEST ("{6..+7}",  "6 7");
    TEST ("{+7..+8}", "7 8");
    TEST ("{11..21}", "11 12 13 14 15 16 17 18 19 20 21");
    TEST ("{0001..0002}", "1 2");
    TEST ("{-0001..0002}", "-1 0 1 2");

    TEST ("{-3..-1}", "-3 -2 -1");
    TEST ("{+3..-2}", "3 2 1 0 -1 -2");
    TEST ("{-3..+2}", "-3 -2 -1 0 1 2");

    TEST ("{a..g}",   "a b c d e f g");
    TEST ("{g..c}",   "g f e d c");
    TEST ("{A..G}",   "A B C D E F G");
    TEST ("{G..C}",   "G F E D C");
    TEST ("{AB..CD}", "AB..CD");

    TEST ("{0..1}",   "0 1");
    TEST ("\\{0..1}", "{0..1}");
    TEST ("{\\0..1}", "0..1");
    TEST ("{0\\..1}", "0..1");
    TEST ("{0.\\.1}", "0..1");
    TEST ("{0..\\1}", "0..1");
    TEST ("{0..1\\}", 0);

    // list expansion
    TEST ("{,}",   "");
    TEST ("{,",    0);
    TEST ("a{,}",  "a a");
    TEST ("b{,",   0);
    TEST ("{c,}",  "c");
    TEST ("{d,",   0);
    TEST ("{,e}",  "e"); // for 100% compatibility this should be " e"
    TEST ("{,f",   0);
    TEST ("{,}g",  "g g");
    TEST ("a{,}b", "ab ab");

    TEST ("{,,}",   "");
    TEST ("a{,,}",  "a a a");
    TEST ("{b,,}",  "b");
    TEST ("{,c,}",  "c");
    TEST ("{,,d}",  "d");
    TEST ("{,,}e",  "e e e");
    TEST ("a{,,}b", "ab ab ab");

    TEST ("{abc,def}",     "abc def");
    TEST ("{ab\\c,d\\ef}", "abc def");
    TEST ("abc{d,e,f}",    "abcd abce abcf");
    
    TEST ("z{c,a{d..f}a,c}z",  "zcz zadaz zaeaz zafaz zcz");
    TEST ("z{c,a{d,e,f}a,c}z", "zcz zadaz zaeaz zafaz zcz");
    
    TEST ("{abc,{,d,e,f,}}",      "abc d e f");
    TEST ("{abc,{,d,e,f,}}{x,y}", "abcx abcy x y dx dy ex ey fx fy x y");
    TEST ("{abc,{,d\\,e\\,f,}}",  "abc d,e,f");

    // list expansion with embedded whitespace
    TEST ("a{b\\ ,c}",       "ab  ac");
    TEST ("a{b\\ \\ ,c\\ }", "ab   ac ");

    // series of list and sequence expansions
    TEST ("A{0..3}",         "A0 A1 A2 A3");
    TEST ("A{0..2}{6..7}",   "A06 A07 A16 A17 A26 A27");
    TEST ("A{A}{0..3}",      "AA0 AA1 AA2 AA3");
    TEST ("A{0..3}{A}",      "A0A A1A A2A A3A");
    TEST ("{A,a}{I,i}{X,x}", "AIX AIx AiX Aix aIX aIx aiX aix");

    // list expansion with nested sequence expansions
    TEST ("A{0,{4..7}{,}}", "A0 A4 A4 A5 A5 A6 A6 A7 A7");
    TEST ("a{1,3,x{5..9}y}b", "a1b a3b ax5yb ax6yb ax7yb ax8yb ax9yb");

    // make absolutely sure that we don't treat ${ as an open brace.
    // we must expand its contents if appropriate.
    TEST ("a${b}c",            "a${b}c");
    TEST ("a{${b},c,d}e",      "a${b}e ace ade");
    TEST ("a{b,${c},d}e",      "abe a${c}e ade");
    TEST ("a{b,${c{1,2}},d}e", "abe a${c1}e a${c2}e ade");

    // csh specific behavior, lists with single items are valid and
    // expanded
    TEST ("{s}",         "s");
    TEST ("{{s}}",       "s");
    TEST ("{{{s}}",      0);
    TEST ("{s,t}",       "s t");
    TEST ("{{s,t}}",     "s t");
    TEST ("{{{{s,t}}}}", "s t");

    // extension: bash sequence expansion with csh behavior
    TEST ("{{-10..10}}",
        "-10 -9 -8 -7 -6 -5 -4 -3 -2 -1 0 1 2 3 4 5 6 7 8 9 10");
    TEST ("{{{-10..10}}}",
        "-10 -9 -8 -7 -6 -5 -4 -3 -2 -1 0 1 2 3 4 5 6 7 8 9 10");

    // escaping nested braces
    TEST ("\\{{{-3..3}}", "{-3 {-2 {-1 {0 {1 {2 {3");
    TEST ("{\\{{-3..3}}", "{-3 {-2 {-1 {0 {1 {2 {3");
    TEST ("{{\\{-3..3}}", "{-3..3");

    TEST ("{{{-3..3\\}}}}", "-3..3}");
    TEST ("{{{-3..3}\\}}}", "-3} -2} -1} 0} 1} 2} 3}");
    TEST ("{{{-3..3}}\\}}", "-3} -2} -1} 0} 1} 2} 3}");
    TEST ("{{{-3..3}}}\\}", "-3} -2} -1} 0} 1} 2} 3}");


    // lifted from the bash-3.2 testsuite and modified to be consistent
    // with expectations of the csh shell and our extensions.

    TEST ("ff{c,b,a}",   "ffc ffb ffa");
    TEST ("f{d,e,f}g",   "fdg feg ffg");
    TEST ("{l,n,m}xyz",  "lxyz nxyz mxyz");
    TEST ("{abc\\,def}", "abc,def");
    TEST ("{abc}",       "abc");

    TEST ("\\{a,b,c,d,e}",          "{a,b,c,d,e}");
    TEST ("{x,y,\\{a,b,c}}",        "x} y} {a} b} c}");
    TEST ("{x\\,y,\\{abc\\},trie}", "x,y {abc} trie");

    TEST ("/usr/{ucb/{ex,edit},lib/{ex,how_ex}}",
          "/usr/ucb/ex /usr/ucb/edit /usr/lib/ex /usr/lib/how_ex");

    TEST ("{}", "");
    TEST ("}",  "}");
    TEST ("{",  0);

    TEST ("abcd{efgh", 0);

    TEST ("{1..10}", "1 2 3 4 5 6 7 8 9 10");

    TEST ("{{0..10},braces}", "0 1 2 3 4 5 6 7 8 9 10 braces");
    TEST ("x{{0..10},braces}y",
          "x0y x1y x2y x3y x4y x5y x6y x7y x8y x9y x10y xbracesy");

    TEST ("{3..3}",    "3");
    TEST ("x{3..3}y",  "x3y");
    TEST ("{10..1}",   "10 9 8 7 6 5 4 3 2 1");
    TEST ("{10..1}y",  "10y 9y 8y 7y 6y 5y 4y 3y 2y 1y");
    TEST ("x{10..1}y", "x10y x9y x8y x7y x6y x5y x4y x3y x2y x1y");

    TEST ("{a..f}", "a b c d e f");
    TEST ("{f..a}", "f e d c b a");
    TEST ("{f..f}", "f");

    // mixes are incorrectly-formed brace expansions
    TEST ("{1..f}", "1..f");
    TEST ("{f..1}", "f..1");

    // do negative numbers work?
    TEST ("{-1..-10}", "-1 -2 -3 -4 -5 -6 -7 -8 -9 -10");
    TEST ("{-20..0}",
          "-20 -19 -18 -17 -16 -15 -14 -13 -12 -11 -10 "
          "-9 -8 -7 -6 -5 -4 -3 -2 -1 0");

    TEST ("a-{b{d,e}}-c",    "a-bd-c a-be-c");
    TEST ("a-{bdef-{g,i}-c", 0);
}

static void
run_brace_expand_tests ()
{
#undef TEST
#define TEST(s,e) DO_TEST (s,e,rw_brace_expand)

    TEST_COMMON (rw_brace_expand);

    // rw_brace_expand does not do whitespace collapse.
    TEST ("a {1,2} b",       "a 1 b a 2 b");
    TEST ("a\t\t{1,2}\t\tb", "a\t\t1\t\tb a\t\t2\t\tb");

    // test whitespace
    TEST (" ",               " ");
    TEST ("  ",              "  ");
    TEST ("  \t",            "  \t");
    TEST ("a  b",            "a  b");
    TEST (" a   b ",         " a   b ");
    TEST (" a{b,c}",        " ab  ac");
    TEST ("a {b,c}",        "a b a c");
    TEST ("a{ b,c}",        "a b ac");
    TEST ("a{b ,c}",         "ab  ac");
    TEST ("a{b, c}",         "ab a c");
    TEST ("a{b,c }",         "ab ac ");
    TEST ("a{b,c} ",         "ab  ac ");

    TEST ("{ }",   " ");
    TEST ("{{ }}", " ");
    TEST ("{{ }",  0); // brace mismatch
}

static void
run_shell_expand_tests ()
{
#undef TEST
#define TEST(s,e) DO_TEST (s,e,rw_shell_expand)

    TEST_COMMON (rw_shell_expand);

    // rw_shell_expand does whitespace collapse
    TEST ("a {1,2} b",       "a 1 2 b");
    TEST ("a\t\t{1,2}\t\tb", "a 1 2 b");

    // test whitespace
    TEST (" ",               "");
    TEST ("  ",              "");
    TEST ("  \t",            "");
    TEST ("a  b",            "a b");
    TEST (" a   b ",         "a b");
    TEST (" a{b,c}",         "ab ac");
    TEST ("a {b,c}",         "a b c");
    TEST ("a{ b,c}",         0);
    TEST ("a{b ,c}",         0);
    TEST ("a{b, c}",         0);
    TEST ("a{b,c }",         0);
    TEST ("a{b,c} ",         "ab ac");

    TEST ("{ }",   0); // brace mismatch
    TEST ("{{ }}", 0); // brace mismatch
    TEST ("{{ }",  0); // brace mismatch

#if 0   // not implemented yet

    // set the three variables
    rw_putenv ("var=baz:varx=vx:vary=vy");

    TEST ("foo{bar,${var}.}", "foobar foobaz.");
    TEST ("foo{bar,${var}}",  "foobar foobaz");

    TEST ("${var}\"{x,y}",    "bazx bazy");
    TEST ("$var{x,y}",        "vx vy");
    TEST ("${var}{x,y}",      "bazx bazy");

    // unset all three variables
    rw_putenv ("var=:varx=:vary=");

#endif   // 0
}

int main ()
{
    run_brace_expand_tests ();

    run_shell_expand_tests ();

    // return 0 on success, 1 on failure
    return !(0 == nerrors);
}
