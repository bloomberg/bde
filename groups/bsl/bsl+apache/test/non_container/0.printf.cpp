/************************************************************************
 *
 * 0.printf.cpp - test exercising the rw_snprinfa() utility functions
 *
 * $Id: 0.printf.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2005-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

// tell Compaq C++ we need ENOMEM (defined by POSIX
// but not in the compiler's pure C++ libc headers)
#undef __PURE_CNAME

#include <rw_printf.h>
#include <rw_process.h> // for rw_pid_t
#include <environ.h>    // for rw_putenv()

#include <bitset>       // for bitset
#include <ios>          // for ios::iostate, ios::openmode, ios::seekdir
#include <string>       // for string
#include <locale>       // for locale::all, ...

#include <assert.h>     // for assert()
#include <ctype.h>      // for isdigit()
#include <errno.h>      // for EXXX, errno
#include <limits.h>     // for INT_MAX, ...
#include <signal.h>     // for SIGABRT, ...
#include <stdio.h>      // for printf(), ...
#include <stdlib.h>     // for free(), size_t
#include <string.h>     // for strcpy()
#include <stdarg.h>     // for va_arg, ...
#include <time.h>       // for struct tm
#include <locale.h>     // for LC_ALL, ...


// disable tests for function name in "%{lF}"
#define _RWSTD_NO_SPRINTFA_FUNNAME

/***********************************************************************/

int ntests;      // number of tests performed
int nfailures;   // number of failures


static void
do_test (int         line,     // line number of the test case
         const char *fmt,      // format string
         const char *expect,   // expected output or 0 on error
         char       *result)   // actual result (0 on error)
{
    static char nullstr[] = "null";
    ++ntests;

    const char* q_fmt = "\"\"";

    if (0 == fmt) {
        fmt   = nullstr;
        q_fmt = "()";
    }

    if (result && expect) {

        const int cmp = memcmp (expect, result, strlen (expect) + 1);

        if (cmp) {
            ++nfailures;
            fprintf (stderr,
                     "# Assertion failed on line %d: "
                     "rw_sprintf(%c%s%c, ...) == \"%s\", got \"%s\"\n",
                     line, q_fmt [0], fmt, q_fmt [1], expect, result);
        }
    }
    else if (result || expect) {
        ++nfailures;

        const char* q_expect = "\"\"";
        const char* q_result = "\"\"";

        if (0 == expect) {
            expect   = nullstr;
            q_expect = "()";
        }

        if (0 == result) {
            result   = nullstr;
            q_result = "()";
        }

        fprintf (stderr, "# Assertion failed on line %d: "
                 "rw_printf(%c%s%c, ...) == %c%s%c got %c%s%c\n",
                 line, q_fmt [0], fmt, q_fmt [1],
                 q_expect [0], expect, q_expect [1],
                 q_result [0], result, q_result [1]);
    }
    else /* if (!result && !expect) */ {
        _RWSTD_ASSERT (!result && !expect);
    }

    if (result && result != nullstr)
        free (result);
}


#undef TEST
#define TEST(fmt, a1, a2, a3, expect)                               \
    do_test (__LINE__, fmt, expect, rw_sprintfa (fmt, a1, a2, a3))

#undef TEST_SPEC
#define TEST_SPEC(pfx, a1, a2, a3, expect)                              \
    {                                                                   \
        ++ntests;                                                       \
        char fmt [64];                                                  \
        sprintf (fmt, "%s%c", pfx, spec);                               \
        char* const s0 = rw_sprintfa (fmt, a1, a2, a3);                 \
        char buf [256];                                                 \
        /* non-const variable below avoids warnings about */            \
        /* controlling expression being constant */                     \
        const char* /* const */ expect_var = (expect);                  \
        if (expect_var)                                                 \
            strcpy (buf, expect_var ? expect_var : "");                 \
        else                                                            \
            sprintf (buf, fmt, a1, a2, a3);                             \
        const int result = memcmp (buf, s0, strlen (buf) + 1);          \
        if (result) {                                                   \
           ++nfailures;                                                 \
           fprintf (stderr,                                             \
                    "# Assertion failed on line %d: "                   \
                    "rw_printf(\"%s\", %ld, %ld, %ld) "                 \
                    "== \"%s\", got \"%s\"\n",                          \
                    __LINE__, fmt,                                      \
                    (long)a1, (long)a2, (long)a3, buf, s0);             \
        }                                                               \
        free (s0);                                                      \
    } (void)0 /* require semicolon after macro invocation */

/***********************************************************************/

// returns an invalid or misaligned address (when 1 < size)
const void* bad_address (size_t size)
{
    const char *addr;

    if (1 < size) {
        static const char buf [] = "0123456789abcdef";

        addr = buf;
        while (0 == ((size_t)addr & (size - 1)))
            ++addr;
    }
    else {

#ifndef _RWSTD_OS_HP_UX
        // the first page is usually unmapped
        addr = (char*)32;
#else
        // the first page on HP-UX is readable, this should give
        // an invalid (inaccessible) address both on IPF and PA
        addr = (char*)(0 - size_t (32));
#endif   // _RWSTD_OS_HP_UX

    }

    return addr;
}

// returns the expected string corresponding to an invalid
// or misaligned address
const char* format_bad_address (const void *ptr, bool valid)
{
    static char buf [80];

#if 4 == _RWSTD_PTR_SIZE
    sprintf (buf, "(%s address %#010" _RWSTD_PRIz "x)",
             valid ? "misaligned" : "invalid", (size_t)ptr);
#elif 8 == _RWSTD_PTR_SIZE
    sprintf (buf, "(%s address %#018" _RWSTD_PRIz "x)",
             valid ? "misaligned" : "invalid", (size_t)ptr);
#else
    sprintf (buf, "(%s address %#0" _RWSTD_PRIz "x)",
             valid ? "misaligned" : "invalid", (size_t)ptr);
#endif

    return buf;
}

/***********************************************************************/

static void
test_percent ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "\"%\": percent sign");

    TEST ("%",        0, 0, 0, "%");
    TEST ("%%",       0, 0, 0, "%");
    TEST ("%% ",      0, 0, 0, "% ");
    TEST (" %% ",     0, 0, 0, " % ");
    TEST ("%%%",      0, 0, 0, "%%");
    TEST ("%% %",     0, 0, 0, "% %");
    TEST (" %",       0, 0, 0, " %");
    TEST ("  %",      0, 0, 0, "  %");
    TEST ("%%%%",     0, 0, 0, "%%");
    TEST ("%% %%",    0, 0, 0, "% %");
    TEST ("%% %% %",  0, 0, 0, "% % %");
    TEST ("%%  %% ",  0, 0, 0, "%  % ");
    TEST (" %%%% ",   0, 0, 0, " %% ");
    TEST ("%%%%%%%%", 0, 0, 0, "%%%%");
}

/***********************************************************************/

static void
test_character ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "\"%c\": character formatting");

    const char spec = 'c';

    TEST_SPEC ("%", '\0',   0, 0, "\0");
    TEST_SPEC ("%", '\a',   0, 0, "\a");
    TEST_SPEC ("%", '\n',   0, 0, "\n");
    TEST_SPEC ("%", '\r',   0, 0, "\r");
    TEST_SPEC ("%", '\t',   0, 0, "\t");
    TEST_SPEC ("%", '0',    0, 0, "0");
    TEST_SPEC ("%", '1',    0, 0, "1");
    TEST_SPEC ("%", 'A',    0, 0, "A");
    TEST_SPEC ("%", 'Z',    0, 0, "Z");
    TEST_SPEC ("%", '\xff', 0, 0, "\xff");

    // exercise right justification
    TEST_SPEC ("%0", 'v',   0, 0, "v");
    TEST_SPEC ("%1", 'w',   0, 0, "w");
    TEST_SPEC ("%2", 'x',   0, 0, " x");
    TEST_SPEC ("%3", 'y',   0, 0, "  y");
    TEST_SPEC ("%4", 'z',   0, 0, "   z");

    TEST_SPEC ("%*", 0,     '0', 0, "0");
    TEST_SPEC ("%*", 1,     '1', 0, "1");
    TEST_SPEC ("%*", 2,     '2', 0, " 2");
    TEST_SPEC ("%*", 3,     '3', 0, "  3");
    TEST_SPEC ("%*", 4,     '4', 0, "   4");

    // exercise left justification
    TEST_SPEC ("%-0", 'V', 0,  0, "V");
    TEST_SPEC ("%-1", 'W', 0,  0, "W");
    TEST_SPEC ("%-2", 'X', 0,  0, "X ");
    TEST_SPEC ("%-3", 'Y', 0,  0, "Y  ");
    TEST_SPEC ("%-4", 'Z', 0,  0, "Z   ");

    TEST_SPEC ("%-*", 0, '0', 0, "0");
    TEST_SPEC ("%-*", 1, '1', 0, "1");
    TEST_SPEC ("%-*", 2, '2', 0, "2 ");
    TEST_SPEC ("%-*", 3, '3', 0, "3  ");
    TEST_SPEC ("%-*", 4, '4', 0, "4   ");

    // 7.19.6.1, p5 of ISO/IEC 9899:1999:
    //   A negative field width argument is taken as a - flag
    //   followed by a positive field width.

    TEST_SPEC ("%*",  -1, '1', 0, "1");
    TEST_SPEC ("%*",  -2, '2', 0, "2 ");
    TEST_SPEC ("%*",  -3, '3', 0, "3  ");
    TEST_SPEC ("%*",  -4, '4', 0, "4   ");
    TEST_SPEC ("%-*", -5, '5', 0, "5    ");

    TEST_SPEC ("%#",  'a', 0,  0, "'a'");

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{c}\": escaped character");

    TEST ("%{c}", '\0',   0, 0, "\\0");
    TEST ("%{c}", '\2',   0, 0, "\\x02");
    TEST ("%{c}", '\a',   0, 0, "\\a");
    TEST ("%{c}", '\n',   0, 0, "\\n");
    TEST ("%{c}", '\r',   0, 0, "\\r");
    TEST ("%{c}", '\t',   0, 0, "\\t");
    TEST ("%{c}", '0',    0, 0, "0");
    TEST ("%{c}", '2',    0, 0, "2");
    TEST ("%{c}", 'A',    0, 0, "A");
    TEST ("%{c}", 'Z',    0, 0, "Z");
    TEST ("%{c}", '\xff', 0, 0, "\\xff");

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{#c}\": quoted escaped character");
    TEST ("%{#c}", '\0',   0, 0, "'\\0'");
    TEST ("%{#c}", '\3',   0, 0, "'\\x03'");
    TEST ("%{#c}", '\a',   0, 0, "'\\a'");
    TEST ("%{#c}", '\n',   0, 0, "'\\n'");
    TEST ("%{#c}", '\r',   0, 0, "'\\r'");
    TEST ("%{#c}", '\t',   0, 0, "'\\t'");
    TEST ("%{#c}", '0',    0, 0, "'0'");
    TEST ("%{#c}", '3',    0, 0, "'3'");
    TEST ("%{#c}", 'A',    0, 0, "'A'");
    TEST ("%{#c}", 'Z',    0, 0, "'Z'");
    TEST ("%{#c}", '\xff', 0, 0, "'\\xff'");

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "\"%lc\": wide character");

    TEST_SPEC ("%", L'\0',   0, 0, "\0");
    TEST_SPEC ("%", L'\a',   0, 0, "\a");
    TEST_SPEC ("%", L'\n',   0, 0, "\n");
    TEST_SPEC ("%", L'\r',   0, 0, "\r");
    TEST_SPEC ("%", L'\t',   0, 0, "\t");
    TEST_SPEC ("%", L'0',    0, 0, "0");
    TEST_SPEC ("%", L'1',    0, 0, "1");
    TEST_SPEC ("%", L'A',    0, 0, "A");
    TEST_SPEC ("%", L'Z',    0, 0, "Z");

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{lc}\": escaped wide character");

    TEST ("%{lc}", L'\0',   0, 0, "\\0");
    TEST ("%{lc}", L'\4',   0, 0, "\\x04");
    TEST ("%{lc}", L'\a',   0, 0, "\\a");
    TEST ("%{lc}", L'\n',   0, 0, "\\n");
    TEST ("%{lc}", L'\r',   0, 0, "\\r");
    TEST ("%{lc}", L'\t',   0, 0, "\\t");
    TEST ("%{lc}", L'0',    0, 0, "0");
    TEST ("%{lc}", L'1',    0, 0, "1");
    TEST ("%{lc}", L'A',    0, 0, "A");
    TEST ("%{lc}", L'Z',    0, 0, "Z");
    TEST ("%{lc}", L'\xff', 0, 0, "\\xff");
    TEST ("%{lc}", -1,      0, 0, "EOF");

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{#lc}\": quoted escaped wide character");

    TEST ("%{#lc}", L'\0',   0, 0, "'\\0'");
    TEST ("%{#lc}", L'\5',   0, 0, "'\\x05'");
    TEST ("%{#lc}", L'\a',   0, 0, "'\\a'");
    TEST ("%{#lc}", L'\n',   0, 0, "'\\n'");
    TEST ("%{#lc}", L'\r',   0, 0, "'\\r'");
    TEST ("%{#lc}", L'\t',   0, 0, "'\\t'");
    TEST ("%{#lc}", L'0',    0, 0, "'0'");
    TEST ("%{#lc}", L'1',    0, 0, "'1'");
    TEST ("%{#lc}", L'A',    0, 0, "'A'");
    TEST ("%{#lc}", L'Z',    0, 0, "'Z'");
    TEST ("%{#lc}", L'\xff', 0, 0, "'\\xff'");
    TEST ("%{#lc}", -1,      0, 0, "EOF");
}

/***********************************************************************/

static void
test_string ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "\"%s\": character string");

    TEST ("",   "",        0, 0, "");
    TEST ("%s", "",        0, 0, "");
    TEST ("%s", "a",       0, 0, "a");
    TEST ("%s", "ab",      0, 0, "ab");
    TEST ("%s", "abc",     0, 0, "abc");
    TEST ("%s", "abcd",    0, 0, "abcd");
    TEST ("%s", "abcde",   0, 0, "abcde");
    TEST ("%s", "abcdef",  0, 0, "abcdef");

    TEST ("|%1s|", "xyz",  0, 0, "|xyz|");
    TEST ("|%2s|", "xyz",  0, 0, "|xyz|");
    TEST ("|%3s|", "xyz",  0, 0, "|xyz|");
    TEST ("|%4s|", "xyz",  0, 0, "| xyz|");
    TEST ("|%5s|", "xyz",  0, 0, "|  xyz|");
    TEST ("|%6s|", "xyz",  0, 0, "|   xyz|");

    TEST ("|%-1s|", "xyz", 0, 0, "|xyz|");
    TEST ("|%-2s|", "xyz", 0, 0, "|xyz|");
    TEST ("|%-3s|", "xyz", 0, 0, "|xyz|");
    TEST ("|%-4s|", "xyz", 0, 0, "|xyz |");
    TEST ("|%-5s|", "xyz", 0, 0, "|xyz  |");
    TEST ("|%-6s|", "xyz", 0, 0, "|xyz   |");

    TEST ("|%+1s|", "xyz", 0, 0, "|xyz|");
    TEST ("|%+2s|", "xyz", 0, 0, "|xyz|");
    TEST ("|%+3s|", "xyz", 0, 0, "|xyz|");
    TEST ("|%+4s|", "xyz", 0, 0, "| xyz|");
    TEST ("|%+5s|", "xyz", 0, 0, "|  xyz|");
    TEST ("|%+6s|", "xyz", 0, 0, "|   xyz|");

    TEST ("|%1.0s|", "xyz", 0, 0, "| |");
    TEST ("|%2.1s|", "xyz", 0, 0, "| x|");
    TEST ("|%3.2s|", "xyz", 0, 0, "| xy|");
    TEST ("|%4.3s|", "xyz", 0, 0, "| xyz|");
    TEST ("|%5.4s|", "xyz", 0, 0, "|  xyz|");
    TEST ("|%6.5s|", "xyz", 0, 0, "|   xyz|");

    TEST ("|%*.*s|",  7, 2, "xyz", "|     xy|");
    TEST ("|%*.*s|", -8, 1, "xyz", "|x       |");
 
    TEST ("%s%s",       "A", "BC", 0,     "ABC");
    TEST ("1%s2%s3",    "A", "BC", 0,     "1A2BC3");
    TEST ("%s%s%s",     "A", "BC", "DEF", "ABCDEF");
    TEST ("1%s2%s3%s4", "A", "BC", "DEF", "1A2BC3DEF4");

    TEST ("%s", 0, 0, 0, "(null)");

#if TEST_RW_EXTENSIONS
    const void* addr = bad_address (0);
    TEST ("%s", addr, 0, 0, format_bad_address (addr, false));
#endif

#ifndef _RWSTD_NO_WCHAR_T

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "\"%ls\": wide character string");

    TEST ("%ls", 0, 0, 0, "(null)");

#if TEST_RW_EXTENSIONS
    addr = bad_address (0);
    TEST ("%ls", addr, 0, 0, format_bad_address (addr, false));

    addr = bad_address (sizeof (wchar_t));
    TEST ("%ls", addr, 0, 0, format_bad_address (addr, true));
#endif

    fprintf (stderr, "Warning: %s\n", "\"%ls\" not exercised");

#endif   //  _RWSTD_NO_WCHAR_T

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{#s}\": quoted character string");

    TEST ("%{#s}", "",     0, 0, "\"\"");
    TEST ("%{#s}", "\0",   0, 0, "\"\"");
    TEST ("%{#s}", "\1",   0, 0, "\"\\x01\"");
    TEST ("%{#s}", "\a",   0, 0, "\"\\a\"");
    TEST ("%{#s}", "\n",   0, 0, "\"\\n\"");
    TEST ("%{#s}", "\r",   0, 0, "\"\\r\"");
    TEST ("%{#s}", "\t",   0, 0, "\"\\t\"");
    TEST ("%{#s}", "\v",   0, 0, "\"\\v\"");
    TEST ("%{#s}", "a",    0, 0, "\"a\"");
    TEST ("%{#s}", "ab",   0, 0, "\"ab\"");
    TEST ("%{#s}", "abc",  0, 0, "\"abc\"");
    TEST ("%{#s}", "a\ac", 0, 0, "\"a\\ac\"");
    TEST ("%{#s}", "a\"c", 0, 0, "\"a\\\"c\"");

    // embedded NULs
    TEST ("%{#1s}", "\0",     0, 0, "\"\\0\"");
    TEST ("%{#2s}", "\0",     0, 0, "\"\\0\\0\"");
    TEST ("%{#2s}", "a\0",    0, 0, "\"a\\0\"");
    TEST ("%{#2s}", "\0a",    0, 0, "\"\\0a\"");
    TEST ("%{#3s}", "\0\0\0", 0, 0, "\"\\0\\0\\0\"");
    TEST ("%{#3s}", "\0a\0",  0, 0, "\"\\0a\\0\"");
    TEST ("%{#3s}", "\0\0a",  0, 0, "\"\\0\\0a\"");

    TEST ("%{#*s}", 0, "\0\0\0v", 0, "\"\"");
    TEST ("%{#*s}", 1, "\0\0\0w", 0, "\"\\0\"");
    TEST ("%{#*s}", 2, "\0\0\0x", 0, "\"\\0\\0\"");
    TEST ("%{#*s}", 3, "\0\0\0y", 0, "\"\\0\\0\\0\"");
    TEST ("%{#*s}", 4, "\0\0\0z", 0, "\"\\0\\0\\0z\"");
    TEST ("%{#*s}", 5, "\0\0\0z", 0, "\"\\0\\0\\0z\\0\"");

#ifndef _RWSTD_NO_WCHAR_T

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{#ls}\": quoted wide character string");

    TEST ("%{#ls}", L"",      0, 0, "\"\"");
    TEST ("%{#ls}", L"\1",    0, 0, "\"\\x01\"");
    TEST ("%{#ls}", L"\a",    0, 0, "\"\\a\"");
    TEST ("%{#ls}", L"\n",    0, 0, "\"\\n\"");
    TEST ("%{#ls}", L"\r",    0, 0, "\"\\r\"");
    TEST ("%{#ls}", L"\t",    0, 0, "\"\\t\"");
    TEST ("%{#ls}", L"\v",    0, 0, "\"\\v\"");
    TEST ("%{#ls}", L"a",     0, 0, "\"a\"");
    TEST ("%{#ls}", L"ab",    0, 0, "\"ab\"");
    TEST ("%{#ls}", L"abc",   0, 0, "\"abc\"");
    TEST ("%{#ls}", L"a\ac",  0, 0, "\"a\\ac\"");
    TEST ("%{#ls}", L"a\"c",  0, 0, "\"a\\\"c\"");
    TEST ("%{#ls}", L"\x100", 0, 0, "\"\\x100\"");

    // embedded NULs
    TEST ("%{#1ls}", L"\0",     0, 0, "\"\\0\"");
    TEST ("%{#2ls}", L"\0",     0, 0, "\"\\0\\0\"");
    TEST ("%{#2ls}", L"a\0",    0, 0, "\"a\\0\"");
    TEST ("%{#2ls}", L"\0a",    0, 0, "\"\\0a\"");
    TEST ("%{#3ls}", L"\0\0\0", 0, 0, "\"\\0\\0\\0\"");
    TEST ("%{#3ls}", L"\0a\0",  0, 0, "\"\\0a\\0\"");
    TEST ("%{#3ls}", L"\0\0a",  0, 0, "\"\\0\\0a\"");

    TEST ("%{#*ls}", 0, L"\0\0\0v", 0, "\"\"");
    TEST ("%{#*ls}", 1, L"\0\0\0w", 0, "\"\\0\"");
    TEST ("%{#*ls}", 2, L"\0\0\0x", 0, "\"\\0\\0\"");
    TEST ("%{#*ls}", 3, L"\0\0\0y", 0, "\"\\0\\0\\0\"");
    TEST ("%{#*ls}", 4, L"\0\0\0z", 0, "\"\\0\\0\\0z\"");
    TEST ("%{#*ls}", 5, L"\0\0\0z", 0, "\"\\0\\0\\0z\\0\"");

#if TEST_RW_EXTENSIONS
    addr = bad_address (0);
    TEST ("%{#ls}", addr, 0, 0, format_bad_address (addr, false));

    addr = bad_address (sizeof (wchar_t));
    TEST ("%{#ls}", addr,  0, 0, format_bad_address (addr, true));
#endif

#endif   // _RWSTD_NO_WCHAR_T

}

/***********************************************************************/

static void
test_chararray ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{Ac}\": quoted character array");

    // exercise the formatting of arrays of characters of arbitrary width
    // (i.e., single-byte narrow, 2-byte, 4-byte (usually wchar_t's), and
    // 8-byte characters
    TEST ("%{Ac}", 0,     0, 0, "(null)");
    TEST ("%{Ac}", "",    0, 0, "\"\"");
    TEST ("%{Ac}", "a",   0, 0, "\"a\"");
    TEST ("%{Ac}", "ab",  0, 0, "\"ab\"");
    TEST ("%{Ac}", "abc", 0, 0, "\"abc\"");

    TEST ("%{1Ac}", 0,     0, 0, "(null)");
    TEST ("%{1Ac}", "",    0, 0, "\"\"");
    TEST ("%{1Ac}", "a",   0, 0, "\"a\"");
    TEST ("%{1Ac}", "ab",  0, 0, "\"ab\"");
    TEST ("%{1Ac}", "abc", 0, 0, "\"abc\"");

    //       +---- width: character width in bytes
    //       | +-- precision: number of elements in array
    //       | |
    //       v v
    TEST ("%{1.0Ac}", 0,     0, 0, "(null)");
    TEST ("%{1.0Ac}", "",    0, 0, "\"\"");
    TEST ("%{1.1Ac}", "",    0, 0, "\"\\0\"");

    TEST ("%{1.0Ac}", "a",   0, 0, "\"\"");
    TEST ("%{1.1Ac}", "a",   0, 0, "\"a\"");
    TEST ("%{1.2Ac}", "a",   0, 0, "\"a\\0\"");

    TEST ("%{1.0Ac}", "ab",  0, 0, "\"\"");
    TEST ("%{1.1Ac}", "ab",  0, 0, "\"a\"");
    TEST ("%{1.2Ac}", "ab",  0, 0, "\"ab\"");
    TEST ("%{1.3Ac}", "ab",  0, 0, "\"ab\\0\"");

    TEST ("%{1.*Ac}", 7,    "ab\0cdef", 0, "\"ab\\0cdef\"");
    TEST ("%{*.7Ac}", 1,    "abc\0def", 0, "\"abc\\0def\"");
    TEST ("%{*.*Ac}", 1, 7, "abcd\0ef",    "\"abcd\\0ef\"");

#ifndef _RWSTD_NO_WCHAR_T
    const unsigned wchar_size = sizeof (wchar_t);
#else   // if defined (_RWSTD_NO_WCHAR_T)
    const unsigned wchar_size = 0;
#endif   // _RWSTD_NO_WCHAR_T

    if (2 == wchar_size) {
        TEST ("%{2Ac}", 0,      0, 0, "(null)");
        TEST ("%{2Ac}", L"",    0, 0, "L\"\"");
        TEST ("%{2Ac}", L"a",   0, 0, "L\"a\"");
        TEST ("%{2Ac}", L"ab",  0, 0, "L\"ab\"");
        TEST ("%{2Ac}", L"abc", 0, 0, "L\"abc\"");

        TEST ("%{2.0Ac}", L"",    0, 0, "L\"\"");
        TEST ("%{2.1Ac}", L"",    0, 0, "L\"\\0\"");

        TEST ("%{2.0Ac}", L"a",   0, 0, "L\"\"");
        TEST ("%{2.1Ac}", L"a",   0, 0, "L\"a\"");
        TEST ("%{2.2Ac}", L"a",   0, 0, "L\"a\\0\"");

        TEST ("%{2.0Ac}", L"ab",  0, 0, "L\"\"");
        TEST ("%{2.1Ac}", L"ab",  0, 0, "L\"a\"");
        TEST ("%{2.2Ac}", L"ab",  0, 0, "L\"ab\"");
        TEST ("%{2.3Ac}", L"ab",  0, 0, "L\"ab\\0\"");

        TEST ("%{2.0Ac}", L"abc", 0, 0, "L\"\"");
        TEST ("%{2.1Ac}", L"abc", 0, 0, "L\"a\"");
        TEST ("%{2.2Ac}", L"abc", 0, 0, "L\"ab\"");
        TEST ("%{2.3Ac}", L"abc", 0, 0, "L\"abc\"");
        TEST ("%{2.4Ac}", L"abc", 0, 0, "L\"abc\\0\"");

        TEST ("%{2.*Ac}", 7,    L"ab\0cdef", 0, "L\"ab\\0cdef\"");
        TEST ("%{*.7Ac}", 2,    L"abc\0def", 0, "L\"abc\\0def\"");
        TEST ("%{*.*Ac}", 2, 7, L"abcd\0ef",    "L\"abcd\\0ef\"");
    }

    if (sizeof (short) != wchar_size) {
        const short s_ []    = { '\0' };
        const short s_a []   = { 'a', '\0' };
        const short s_ab []  = { 'a', 'b', '\0' };
        const short s_abc [] = { 'a', 'b', 'c', '\0' };
            
        TEST ("%{2Ac}", 0,     0, 0, "(null)");
        TEST ("%{2Ac}", s_,    0, 0, "\"\"");
        TEST ("%{2Ac}", s_a,   0, 0, "\"a\"");
        TEST ("%{2Ac}", s_ab,  0, 0, "\"ab\"");
        TEST ("%{2Ac}", s_abc, 0, 0, "\"abc\"");
    }

    if (4 == wchar_size) {
        TEST ("%{4Ac}", 0,      0, 0, "(null)");
        TEST ("%{4Ac}", L"",    0, 0, "L\"\"");
        TEST ("%{4Ac}", L"a",   0, 0, "L\"a\"");
        TEST ("%{4Ac}", L"ab",  0, 0, "L\"ab\"");
        TEST ("%{4Ac}", L"abc", 0, 0, "L\"abc\"");

        TEST ("%{4.0Ac}", L"",    0, 0, "L\"\"");
        TEST ("%{4.1Ac}", L"",    0, 0, "L\"\\0\"");

        TEST ("%{4.0Ac}", L"a",   0, 0, "L\"\"");
        TEST ("%{4.1Ac}", L"a",   0, 0, "L\"a\"");
        TEST ("%{4.2Ac}", L"a",   0, 0, "L\"a\\0\"");

        TEST ("%{4.0Ac}", L"ab",  0, 0, "L\"\"");
        TEST ("%{4.1Ac}", L"ab",  0, 0, "L\"a\"");
        TEST ("%{4.2Ac}", L"ab",  0, 0, "L\"ab\"");
        TEST ("%{4.3Ac}", L"ab",  0, 0, "L\"ab\\0\"");

        TEST ("%{4.0Ac}", L"abc", 0, 0, "L\"\"");
        TEST ("%{4.1Ac}", L"abc", 0, 0, "L\"a\"");
        TEST ("%{4.2Ac}", L"abc", 0, 0, "L\"ab\"");
        TEST ("%{4.3Ac}", L"abc", 0, 0, "L\"abc\"");
        TEST ("%{4.4Ac}", L"abc", 0, 0, "L\"abc\\0\"");

        TEST ("%{4.*Ac}", 7,    L"ab\0cdef", 0, "L\"ab\\0cdef\"");
        TEST ("%{*.7Ac}", 4,    L"abc\0def", 0, "L\"abc\\0def\"");
        TEST ("%{*.*Ac}", 4, 7, L"abcd\0ef",    "L\"abcd\\0ef\"");
    }
}

/***********************************************************************/

static const char**
mkargv (const char *arg0 = 0,
        const char *arg1 = 0,
        const char *arg2 = 0,
        const char *arg3 = 0,
        const char *arg4 = 0,
        const char *arg5 = 0,
        const char *arg6 = 0,
        const char *arg7 = 0,
        const char *arg8 = 0,
        const char *arg9 = 0)
{
    static const char* argv [10];

    argv [0] = arg0;
    argv [1] = arg1;
    argv [2] = arg2;
    argv [3] = arg3;
    argv [4] = arg4;
    argv [5] = arg5;
    argv [6] = arg6;
    argv [7] = arg7;
    argv [8] = arg8;
    argv [9] = arg9;

    return argv;
}


static void
test_stringarray ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{As}\": array of character strings");

#undef ARG
#define ARG   mkargv

    TEST ("%{As}", 0,                              0, 0, "(null)");
    TEST ("%{As}", ARG ("a"),                      0, 0, "a");
    TEST ("%{As}", ARG ("a", "bc"),                0, 0, "a,bc");
    TEST ("%{As}", ARG ("a", "bc", "def"),         0, 0, "a,bc,def");
    TEST ("%{As}", ARG ("a", "bc", "def", "ghij"), 0, 0, "a,bc,def,ghij");

    TEST ("%{#As}", 0,                         0, 0, "(null)");
    TEST ("%{#As}", ARG ("abcd"),              0, 0, "\"abcd\"");
    TEST ("%{#As}", ARG ("abcd", "efg"),       0, 0, "\"abcd\",\"efg\"");
    TEST ("%{#As}", ARG ("abcd", "efg", "hi"), 0, 0, "\"abcd\",\"efg\",\"hi\"");

    TEST ("%{ As}", 0,                              0, 0, "(null)");
    TEST ("%{ As}", ARG ("a"),                      0, 0, "a");
    TEST ("%{ As}", ARG ("a", "bc"),                0, 0, "a bc");
    TEST ("%{ As}", ARG ("a", "bc", "def"),         0, 0, "a bc def");
    TEST ("%{ As}", ARG ("a", "bc", "def", "ghij"), 0, 0, "a bc def ghij");

    TEST ("%{ #As}", 0,                        0, 0, "(null)");
    TEST ("%{ #As}", ARG ("abc"),              0, 0, "\"abc\"");
    TEST ("%{ #As}", ARG ("abc", "efg"),       0, 0, "\"abc\" \"efg\"");
    TEST ("%{ #As}", ARG ("abc", "efg", "hi"), 0, 0, "\"abc\" \"efg\" \"hi\"");
}

/***********************************************************************/

static void
test_basic_string ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{S}\": std::string");

    std::string str;

#undef S
#define S(s)   &(str = std::string (s, sizeof s - 1))

    TEST ("%{S}",  0,                0, 0, "(null)");
    TEST ("%{S}",  S (""),           0, 0, "");
    TEST ("%{S}",  S ("a"),          0, 0, "a");
    TEST ("%{S}",  S ("ab"),         0, 0, "ab");
    TEST ("%{S}",  S ("abc"),        0, 0, "abc");

    TEST ("%{#S}", S ("\a\n\r\t\v"), 0, 0, "\"\\a\\n\\r\\t\\v\"");

    TEST ("%{#S}", S ("\0bc"),       0, 0, "\"\\0bc\"");
    TEST ("%{#S}", S ("a\0c"),       0, 0, "\"a\\0c\"");
    TEST ("%{#S}", S ("ab\0"),       0, 0, "\"ab\\0\"");
    TEST ("%{#S}", S ("a\0\0"),      0, 0, "\"a\\0\\0\"");
    TEST ("%{#S}", S ("\0\0\0"),     0, 0, "\"\\0\\0\\0\"");

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{lS}\": std::wstring");

#ifndef _RWSTD_NO_WCHAR_T

    std::wstring wstr;

#  undef WS
#  define WS(ws)   \
      &(wstr = std::wstring (L ## ws, sizeof L ## ws / sizeof (wchar_t) - 1))

    TEST ("%{lS}",  0,                 0, 0, "(null)");
    TEST ("%{lS}",  WS (""),           0, 0, "");
    TEST ("%{lS}",  WS ("a"),          0, 0, "a");
    TEST ("%{lS}",  WS ("ab"),         0, 0, "ab");
    TEST ("%{lS}",  WS ("abc"),        0, 0, "abc");

    TEST ("%{#lS}", WS ("\a\n\r\t\v"), 0, 0, "\"\\a\\n\\r\\t\\v\"");

    TEST ("%{#lS}", WS ("\0bc"),       0, 0, "\"\\0bc\"");
    TEST ("%{#lS}", WS ("a\0c"),       0, 0, "\"a\\0c\"");
    TEST ("%{#lS}", WS ("ab\0"),       0, 0, "\"ab\\0\"");
    TEST ("%{#lS}", WS ("a\0\0"),      0, 0, "\"a\\0\\0\"");
    TEST ("%{#lS}", WS ("\0\0\0"),     0, 0, "\"\\0\\0\\0\"");

#else   // if defined (_RWSTD_NO_WCHAR_T)

    fprintf (stderr, "Warning: %s\n", "\"%{lS}\" not exercised: "
             "_RWSTD_NO_WCHAR_T #defined");

#endif   // _RWSTD_NO_WCHAR_T

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{#*S}\": std::basic_string<charT> with "
            "sizeof (charT)");

    TEST ("%{#1S}", 0,            0, 0, "(null)");
    TEST ("%{#1S}", S ("\0bc"),   0, 0, "\"\\0bc\"");
    TEST ("%{#1S}", S ("a\0c"),   0, 0, "\"a\\0c\"");
    TEST ("%{#1S}", S ("ab\0"),   0, 0, "\"ab\\0\"");
    TEST ("%{#1S}", S ("a\0\0"),  0, 0, "\"a\\0\\0\"");
    TEST ("%{#1S}", S ("\0\0\0"), 0, 0, "\"\\0\\0\\0\"");

#if 2 == _RWSTD_WCHAR_SIZE

    TEST ("%{#2S}",  0,             0, 0, "(null)");
    TEST ("%{#2S}",  WS (""),       0, 0, "L\"\"");
    TEST ("%{#2S}",  WS ("a"),      0, 0, "L\"a\"");
    TEST ("%{#2S}",  WS ("ab"),     0, 0, "L\"ab\"");
    TEST ("%{#2S}",  WS ("abc"),    0, 0, "L\"abc\"");

    TEST ("%{#2S}",  WS ("\0bc"),   0, 0, "L\"\\0bc\"");
    TEST ("%{#2S}",  WS ("a\0c"),   0, 0, "L\"a\\0c\"");
    TEST ("%{#2S}",  WS ("ab\0"),   0, 0, "L\"ab\\0\"");
    TEST ("%{#2S}",  WS ("a\0\0"),  0, 0, "L\"a\\0\\0\"");
    TEST ("%{#2S}",  WS ("\0\0\0"), 0, 0, "L\"\\0\\0\\0\"");

#elif 4 == _RWSTD_WCHAR_SIZE

    TEST ("%{#4S}",  0,             0, 0, "(null)");
    TEST ("%{#4S}",  WS (""),       0, 0, "L\"\"");
    TEST ("%{#4S}",  WS ("a"),      0, 0, "L\"a\"");
    TEST ("%{#4S}",  WS ("ab"),     0, 0, "L\"ab\"");
    TEST ("%{#4S}",  WS ("abc"),    0, 0, "L\"abc\"");

    TEST ("%{#4S}",  WS ("\0bc"),   0, 0, "L\"\\0bc\"");
    TEST ("%{#4S}",  WS ("a\0c"),   0, 0, "L\"a\\0c\"");
    TEST ("%{#4S}",  WS ("ab\0"),   0, 0, "L\"ab\\0\"");
    TEST ("%{#4S}",  WS ("a\0\0"),  0, 0, "L\"a\\0\\0\"");
    TEST ("%{#4S}",  WS ("\0\0\0"), 0, 0, "L\"\\0\\0\\0\"");

#endif   // _RWSTD_WCHAR_SIZE

}

/***********************************************************************/

static void
test_ios_bitmasks ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{Is}\": std::ios_base::iostate");

    const int bad  = std::ios_base::badbit;
    const int eof  = std::ios_base::eofbit;
    const int fail = std::ios_base::failbit;
//    const int good = std::ios_base::goodbit;

    TEST ("[%{Is}]", 0,                0, 0, "[goodbit]");
    TEST ("[%{Is}]", bad,              0, 0, "[badbit]");
    TEST ("[%{Is}]", eof,              0, 0, "[eofbit]");
    TEST ("[%{Is}]", fail,             0, 0, "[failbit]");

    TEST ("[%{#Is}]", 0,               0, 0, "[std::ios::goodbit]");
    TEST ("[%{#Is}]", bad,             0, 0, "[std::ios::badbit]");
    TEST ("[%{#Is}]", eof,             0, 0, "[std::ios::eofbit]");
    TEST ("[%{#Is}]", fail,            0, 0, "[std::ios::failbit]");

    TEST ("[%{Is}]", bad | eof,        0, 0, "[badbit | eofbit]");
    TEST ("[%{Is}]", bad | fail,       0, 0, "[badbit | failbit]");
    TEST ("[%{Is}]", eof | fail,       0, 0, "[eofbit | failbit]");
    TEST ("[%{Is}]", bad | eof | fail, 0, 0, "[badbit | eofbit | failbit]");

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{Io}\": std::ios_base::opemode");

    const int in  = std::ios_base::in;
    const int out = std::ios_base::out;
    const int ate = std::ios_base::ate;

    TEST ("[%{Io}]", 0,               0, 0, "[openmode(0)]");
    TEST ("[%{Io}]", in,              0, 0, "[in]");
    TEST ("[%{Io}]", out,             0, 0, "[out]");
    TEST ("[%{Io}]", ate,             0, 0, "[ate]");
    TEST ("[%{Io}]", in | out,        0, 0, "[in | out]");
    TEST ("[%{Io}]", in | ate,        0, 0, "[in | ate]");
    TEST ("[%{Io}]", in | out | ate,  0, 0, "[in | out | ate]");
    TEST ("[%{Io}]", out | ate,       0, 0, "[out | ate]");

    TEST ("[%{#Io}]", 0,              0, 0, "[std::ios::openmode(0)]");
    TEST ("[%{#Io}]", in,             0, 0, "[std::ios::in]");
    TEST ("[%{#Io}]", out,            0, 0, "[std::ios::out]");
    TEST ("[%{#Io}]", ate,            0, 0, "[std::ios::ate]");
    TEST ("[%{#Io}]", in | out,       0, 0, "[std::ios::in | std::ios::out]");
    TEST ("[%{#Io}]", in | ate,       0, 0, "[std::ios::in | std::ios::ate]");
    TEST ("[%{#Io}]", in | out | ate, 0, 0,
          "[std::ios::in | std::ios::out | std::ios::ate]");
    TEST ("[%{#Io}]", out | ate,      0, 0, "[std::ios::out | std::ios::ate]");

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{Iw}\": std::ios_base::seekdir");

    TEST ("[%{Iw}]",  std::ios::beg, 0, 0, "[beg]");
    TEST ("[%{Iw}]",  std::ios::cur, 0, 0, "[cur]");
    TEST ("[%{Iw}]",  std::ios::end, 0, 0, "[end]");

    TEST ("[%{#Iw}]", std::ios::beg, 0, 0, "[std::ios::beg]");
    TEST ("[%{#Iw}]", std::ios::cur, 0, 0, "[std::ios::cur]");
    TEST ("[%{#Iw}]", std::ios::end, 0, 0, "[std::ios::end]");

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{If}\": std::ios_base::fmtflags");

#define BASE(n)   ((n)  << _RWSTD_IOS_BASEOFF)

    TEST ("[%{If}]",  0,                     0, 0, "[fmtflags(0)]");
    TEST ("[%{If}]",  std::ios::adjustfield, 0, 0, "[adjustfield]");
    TEST ("[%{If}]",  std::ios::basefield,   0, 0, "[basefield]");
    TEST ("[%{If}]",  std::ios::boolalpha,   0, 0, "[boolalpha]");
    TEST ("[%{If}]",  std::ios::dec,         0, 0, "[dec]");
    TEST ("[%{If}]",  std::ios::fixed,       0, 0, "[fixed]");
    TEST ("[%{If}]",  std::ios::hex,         0, 0, "[hex]");
    TEST ("[%{If}]",  std::ios::internal,    0, 0, "[internal]");
    TEST ("[%{If}]",  std::ios::left,        0, 0, "[left]");
    TEST ("[%{If}]",  std::ios::oct,         0, 0, "[oct]");
    TEST ("[%{If}]",  std::ios::right,       0, 0, "[right]");
    TEST ("[%{If}]",  std::ios::scientific,  0, 0, "[scientific]");
    TEST ("[%{If}]",  std::ios::showbase,    0, 0, "[showbase]");
    TEST ("[%{If}]",  std::ios::showpoint,   0, 0, "[showpoint]");
    TEST ("[%{If}]",  std::ios::showpos,     0, 0, "[showpos]");
    TEST ("[%{If}]",  std::ios::skipws,      0, 0, "[skipws]");
    TEST ("[%{If}]",  std::ios::unitbuf,     0, 0, "[unitbuf]");
    TEST ("[%{If}]",  std::ios::uppercase,   0, 0, "[uppercase]");
#ifndef _RWSTD_NO_EXT_BIN_IO
    TEST ("[%{If}]",  std::ios::bin,         0, 0, "[bin]");
#endif   // _RWSTD_NO_EXT_BIN_IO
#ifndef _RWSTD_NO_EXT_REENTRANT_IO
    TEST ("[%{If}]",  std::ios::nolock,      0, 0, "[nolock]");
    TEST ("[%{If}]",  std::ios::nolockbuf,   0, 0, "[nolockbuf]");
#endif   // _RWSTD_NO_EXT_REENTRANT_IO

#if TEST_RW_EXTENSIONS
    TEST ("[%{If}]",  BASE (1),              0, 0, "[fmtflags(0) | base(1)]");
    TEST ("[%{If}]",  BASE (2),              0, 0, "[fmtflags(0) | base(2)]");
    TEST ("[%{If}]",  BASE (3),              0, 0, "[fmtflags(0) | base(3)]");
    TEST ("[%{If}]",  BASE (4),              0, 0, "[fmtflags(0) | base(4)]");
    TEST ("[%{If}]",  BASE (5),              0, 0, "[fmtflags(0) | base(5)]");
    TEST ("[%{If}]",  BASE (6),              0, 0, "[fmtflags(0) | base(6)]");
    TEST ("[%{If}]",  BASE (7),              0, 0, "[fmtflags(0) | base(7)]");
    TEST ("[%{If}]",  BASE (8),              0, 0, "[fmtflags(0)]");
    TEST ("[%{If}]",  BASE (9),              0, 0, "[fmtflags(0) | base(9)]");
    TEST ("[%{If}]",  BASE (10),             0, 0, "[fmtflags(0)]");
    TEST ("[%{If}]",  BASE (11),             0, 0, "[fmtflags(0) | base(11)]");
    TEST ("[%{If}]",  BASE (12),             0, 0, "[fmtflags(0) | base(12)]");
    TEST ("[%{If}]",  BASE (13),             0, 0, "[fmtflags(0) | base(13)]");
    TEST ("[%{If}]",  BASE (14),             0, 0, "[fmtflags(0) | base(14)]");
    TEST ("[%{If}]",  BASE (15),             0, 0, "[fmtflags(0) | base(15)]");
    TEST ("[%{If}]",  BASE (16),             0, 0, "[fmtflags(0)]");
    TEST ("[%{If}]",  BASE (17),             0, 0, "[fmtflags(0) | base(17)]");
    TEST ("[%{If}]",  BASE (18),             0, 0, "[fmtflags(0) | base(18)]");
    TEST ("[%{If}]",  BASE (19),             0, 0, "[fmtflags(0) | base(19)]");
    TEST ("[%{If}]",  BASE (20),             0, 0, "[fmtflags(0) | base(20)]");
    TEST ("[%{If}]",  BASE (21),             0, 0, "[fmtflags(0) | base(21)]");
    TEST ("[%{If}]",  BASE (22),             0, 0, "[fmtflags(0) | base(22)]");
    TEST ("[%{If}]",  BASE (23),             0, 0, "[fmtflags(0) | base(23)]");
    TEST ("[%{If}]",  BASE (24),             0, 0, "[fmtflags(0) | base(24)]");
    TEST ("[%{If}]",  BASE (25),             0, 0, "[fmtflags(0) | base(25)]");
    TEST ("[%{If}]",  BASE (26),             0, 0, "[fmtflags(0) | base(26)]");
    TEST ("[%{If}]",  BASE (27),             0, 0, "[fmtflags(0) | base(27)]");
    TEST ("[%{If}]",  BASE (28),             0, 0, "[fmtflags(0) | base(28)]");
    TEST ("[%{If}]",  BASE (29),             0, 0, "[fmtflags(0) | base(29)]");
    TEST ("[%{If}]",  BASE (30),             0, 0, "[fmtflags(0) | base(30)]");
    TEST ("[%{If}]",  BASE (31),             0, 0, "[fmtflags(0) | base(31)]");
    TEST ("[%{If}]",  BASE (32),             0, 0, "[fmtflags(0) | base(32)]");
    TEST ("[%{If}]",  BASE (33),             0, 0, "[fmtflags(0) | base(33)]");
    TEST ("[%{If}]",  BASE (34),             0, 0, "[fmtflags(0) | base(34)]");
    TEST ("[%{If}]",  BASE (35),             0, 0, "[fmtflags(0) | base(35)]");
    TEST ("[%{If}]",  BASE (36),             0, 0, "[fmtflags(0) | base(36)]");
#endif // TEST_RW_EXTENSIONS

    TEST ("[%{#If}]",  0,                     0, 0, "[std::ios::fmtflags(0)]");
    TEST ("[%{#If}]",  std::ios::adjustfield, 0, 0, "[std::ios::adjustfield]");
    TEST ("[%{#If}]",  std::ios::basefield,   0, 0, "[std::ios::basefield]");
    TEST ("[%{#If}]",  std::ios::boolalpha,   0, 0, "[std::ios::boolalpha]");
    TEST ("[%{#If}]",  std::ios::dec,         0, 0, "[std::ios::dec]");
    TEST ("[%{#If}]",  std::ios::fixed,       0, 0, "[std::ios::fixed]");
    TEST ("[%{#If}]",  std::ios::hex,         0, 0, "[std::ios::hex]");
    TEST ("[%{#If}]",  std::ios::internal,    0, 0, "[std::ios::internal]");
    TEST ("[%{#If}]",  std::ios::left,        0, 0, "[std::ios::left]");
    TEST ("[%{#If}]",  std::ios::oct,         0, 0, "[std::ios::oct]");
    TEST ("[%{#If}]",  std::ios::right,       0, 0, "[std::ios::right]");
    TEST ("[%{#If}]",  std::ios::scientific,  0, 0, "[std::ios::scientific]");
    TEST ("[%{#If}]",  std::ios::showbase,    0, 0, "[std::ios::showbase]");
    TEST ("[%{#If}]",  std::ios::showpoint,   0, 0, "[std::ios::showpoint]");
    TEST ("[%{#If}]",  std::ios::showpos,     0, 0, "[std::ios::showpos]");
    TEST ("[%{#If}]",  std::ios::skipws,      0, 0, "[std::ios::skipws]");
    TEST ("[%{#If}]",  std::ios::unitbuf,     0, 0, "[std::ios::unitbuf]");
    TEST ("[%{#If}]",  std::ios::uppercase,   0, 0, "[std::ios::uppercase]");
#ifndef _RWSTD_NO_EXT_BIN_IO
    TEST ("[%{#If}]",  std::ios::bin,         0, 0, "[std::ios::bin]");
#endif   // _RWSTD_NO_EXT_BIN_IO
#ifndef _RWSTD_NO_EXT_REENTRANT_IO
    TEST ("[%{#If}]",  std::ios::nolock,      0, 0, "[std::ios::nolock]");
    TEST ("[%{#If}]",  std::ios::nolockbuf,   0, 0, "[std::ios::nolockbuf]");
#endif   // _RWSTD_NO_EXT_REENTRANT_IO

#if TEST_RW_EXTENSIONS
    TEST ("[%{#If}]",  BASE (1),              0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(1)]");
    TEST ("[%{#If}]",  BASE (2),              0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(2)]");
    TEST ("[%{#If}]",  BASE (3),              0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(3)]");
    TEST ("[%{#If}]",  BASE (4),              0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(4)]");
    TEST ("[%{#If}]",  BASE (5),              0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(5)]");
    TEST ("[%{#If}]",  BASE (6),              0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(6)]");
    TEST ("[%{#If}]",  BASE (7),              0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(7)]");
    TEST ("[%{#If}]",  BASE (8),              0, 0,
          "[std::ios::fmtflags(0)]");
    TEST ("[%{#If}]",  BASE (9),              0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(9)]");
    TEST ("[%{#If}]",  BASE (10),             0, 0,
          "[std::ios::fmtflags(0)]");
    TEST ("[%{#If}]",  BASE (11),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(11)]");
    TEST ("[%{#If}]",  BASE (12),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(12)]");
    TEST ("[%{#If}]",  BASE (13),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(13)]");
    TEST ("[%{#If}]",  BASE (14),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(14)]");
    TEST ("[%{#If}]",  BASE (15),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(15)]");
    TEST ("[%{#If}]",  BASE (16),             0, 0,
          "[std::ios::fmtflags(0)]");
    TEST ("[%{#If}]",  BASE (17),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(17)]");
    TEST ("[%{#If}]",  BASE (18),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(18)]");
    TEST ("[%{#If}]",  BASE (19),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(19)]");
    TEST ("[%{#If}]",  BASE (20),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(20)]");
    TEST ("[%{#If}]",  BASE (21),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(21)]");
    TEST ("[%{#If}]",  BASE (22),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(22)]");
    TEST ("[%{#If}]",  BASE (23),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(23)]");
    TEST ("[%{#If}]",  BASE (24),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(24)]");
    TEST ("[%{#If}]",  BASE (25),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(25)]");
    TEST ("[%{#If}]",  BASE (26),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(26)]");
    TEST ("[%{#If}]",  BASE (27),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(27)]");
    TEST ("[%{#If}]",  BASE (28),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(28)]");
    TEST ("[%{#If}]",  BASE (29),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(29)]");
    TEST ("[%{#If}]",  BASE (30),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(30)]");
    TEST ("[%{#If}]",  BASE (31),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(31)]");
    TEST ("[%{#If}]",  BASE (32),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(32)]");
    TEST ("[%{#If}]",  BASE (33),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(33)]");
    TEST ("[%{#If}]",  BASE (34),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(34)]");
    TEST ("[%{#If}]",  BASE (35),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(35)]");
    TEST ("[%{#If}]",  BASE (36),             0, 0,
          "[std::ios::fmtflags(0) | std::ios::base(36)]");
#endif // TEST_RW_EXTENSIONS

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{Ie}\": std::ios_base::event");

    TEST ("[%{Ie}]", std::ios::erase_event,   0, 0, "[erase_event]");
    TEST ("[%{Ie}]", std::ios::imbue_event,   0, 0, "[imbue_event]");
    TEST ("[%{Ie}]", std::ios::copyfmt_event, 0, 0, "[copyfmt_event]");
    TEST ("[%{Ie}]", 3,                       0, 0, "[event(3)]");
    TEST ("[%{Ie}]", 10,                      0, 0, "[event(10)]");

    TEST ("[%{#Ie}]", std::ios::erase_event,   0, 0,
          "[std::ios::erase_event]");
    TEST ("[%{#Ie}]", std::ios::imbue_event,   0, 0,
          "[std::ios::imbue_event]");
    TEST ("[%{#Ie}]", std::ios::copyfmt_event, 0, 0,
          "[std::ios::copyfmt_event]");
    TEST ("[%{#Ie}]", 3,                       0, 0,
          "[std::ios::event(3)]");
    TEST ("[%{#Ie}]", 10,                      0, 0,
          "[std::ios::event(10)]");
}

/***********************************************************************/

static void
test_locale_category ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{Lc}\": locale category");

    TEST ("[%{Lc}]", LC_ALL,      0, 0, "[LC_ALL]");
    TEST ("[%{Lc}]", LC_COLLATE,  0, 0, "[LC_COLLATE]");
    TEST ("[%{Lc}]", LC_CTYPE,    0, 0, "[LC_CTYPE]");
    TEST ("[%{Lc}]", LC_MONETARY, 0, 0, "[LC_MONETARY]");
    TEST ("[%{Lc}]", LC_NUMERIC,  0, 0, "[LC_NUMERIC]");
    TEST ("[%{Lc}]", LC_TIME,     0, 0, "[LC_TIME]");
#ifdef LC_MESSAGES
    TEST ("[%{Lc}]", LC_MESSAGES, 0, 0, "[LC_MESSAGES]");
#endif   // LC_MESSAGES

    TEST ("[%{Lc}]", std::locale::all,      0, 0, "[all]");
    TEST ("[%{Lc}]", std::locale::none,     0, 0, "[none]");
    TEST ("[%{Lc}]", std::locale::collate,  0, 0, "[collate]");
    TEST ("[%{Lc}]", std::locale::ctype,    0, 0, "[ctype]");
    TEST ("[%{Lc}]", std::locale::monetary, 0, 0, "[monetary]");
    TEST ("[%{Lc}]", std::locale::numeric,  0, 0, "[numeric]");
    TEST ("[%{Lc}]", std::locale::messages, 0, 0, "[messages]");
    TEST ("[%{Lc}]", std::locale::time,     0, 0, "[time]");

    TEST ("[%{#Lc}]", LC_ALL,      0, 0, "[LC_ALL]");
    TEST ("[%{#Lc}]", LC_COLLATE,  0, 0, "[LC_COLLATE]");
    TEST ("[%{#Lc}]", LC_CTYPE,    0, 0, "[LC_CTYPE]");
    TEST ("[%{#Lc}]", LC_MONETARY, 0, 0, "[LC_MONETARY]");
    TEST ("[%{#Lc}]", LC_NUMERIC,  0, 0, "[LC_NUMERIC]");
    TEST ("[%{#Lc}]", LC_TIME,     0, 0, "[LC_TIME]");
#ifdef LC_MESSAGES
    TEST ("[%{#Lc}]", LC_MESSAGES, 0, 0, "[LC_MESSAGES]");
#endif   // LC_MESSAGES

    TEST ("[%{#Lc}]", std::locale::all,      0, 0, "[std::locale::all]");
    TEST ("[%{#Lc}]", std::locale::none,     0, 0, "[std::locale::none]");
    TEST ("[%{#Lc}]", std::locale::collate,  0, 0, "[std::locale::collate]");
    TEST ("[%{#Lc}]", std::locale::ctype,    0, 0, "[std::locale::ctype]");
    TEST ("[%{#Lc}]", std::locale::monetary, 0, 0, "[std::locale::monetary]");
    TEST ("[%{#Lc}]", std::locale::numeric,  0, 0, "[std::locale::numeric]");
    TEST ("[%{#Lc}]", std::locale::messages, 0, 0, "[std::locale::messages]");
    TEST ("[%{#Lc}]", std::locale::time,     0, 0, "[std::locale::time]");
}

/***********************************************************************/

static void
test_ctype_mask ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{LC}\": std::ctype_base::mask");

    const int Alpha  = std::ctype_base::alpha;
    const int Alnum  = std::ctype_base::alnum;
    const int Cntrl  = std::ctype_base::cntrl;
    const int Digit  = std::ctype_base::digit;
    const int Graph  = std::ctype_base::graph;
    const int Lower  = std::ctype_base::lower;
    const int Print  = std::ctype_base::print;
    const int Punct  = std::ctype_base::punct;
    const int Space  = std::ctype_base::space;
    const int Xdigit = std::ctype_base::xdigit;

    TEST ("[%{LC}]", Alpha,  0, 0, "[alpha]");
    TEST ("[%{LC}]", Alnum,  0, 0, "[alnum]");
    TEST ("[%{LC}]", Cntrl,  0, 0, "[cntrl]");
    TEST ("[%{LC}]", Digit,  0, 0, "[digit]");
    TEST ("[%{LC}]", Graph,  0, 0, "[graph]");
    TEST ("[%{LC}]", Lower,  0, 0, "[lower]");
    TEST ("[%{LC}]", Print,  0, 0, "[print]");
    TEST ("[%{LC}]", Punct,  0, 0, "[punct]");
    TEST ("[%{LC}]", Space,  0, 0, "[space]");
    TEST ("[%{LC}]", Xdigit, 0, 0, "[xdigit]");

    TEST ("[%{#LC}]", Alpha,  0, 0, "[std::ctype_base::alpha]");
    TEST ("[%{#LC}]", Alnum,  0, 0, "[std::ctype_base::alnum]");
    TEST ("[%{#LC}]", Cntrl,  0, 0, "[std::ctype_base::cntrl]");
    TEST ("[%{#LC}]", Digit,  0, 0, "[std::ctype_base::digit]");
    TEST ("[%{#LC}]", Graph,  0, 0, "[std::ctype_base::graph]");
    TEST ("[%{#LC}]", Lower,  0, 0, "[std::ctype_base::lower]");
    TEST ("[%{#LC}]", Print,  0, 0, "[std::ctype_base::print]");
    TEST ("[%{#LC}]", Punct,  0, 0, "[std::ctype_base::punct]");
    TEST ("[%{#LC}]", Space,  0, 0, "[std::ctype_base::space]");
    TEST ("[%{#LC}]", Xdigit, 0, 0, "[std::ctype_base::xdigit]");

    TEST ("[%{LC}]", Alpha | Cntrl, 0, 0, "[alpha | cntrl]");
    TEST ("[%{LC}]", Cntrl | Digit, 0, 0, "[cntrl | digit]");
    TEST ("[%{LC}]", Digit | Graph, 0, 0, "[digit | graph]");
    TEST ("[%{LC}]", Graph | Lower, 0, 0, "[graph | lower]");
    TEST ("[%{LC}]", Lower | Print, 0, 0, "[lower | print]");
    TEST ("[%{LC}]", Print | Punct, 0, 0, "[print | punct]");
    TEST ("[%{LC}]", Punct | Space, 0, 0, "[punct | space]");
}

/***********************************************************************/


static const char*
mkbitset (const char *str)
{
    static char bitset [32];

    memset (bitset, 0, sizeof bitset);

    char *pbyte = bitset;

    for (const char *pc = str; *pc; ++pc) {

        const size_t bitno = size_t (pc - str);

        if ((bitno & 15) == 8)
            ++pbyte;

        const size_t binx = bitno & 7;

        if ('0' == *pc)
            *pbyte &= ~(1 << binx);
        else if ('1' == *pc)
            *pbyte |= 1 << binx;
        else
            RW_ASSERT (!"logic error: bit must be '0' or '1'");

        RW_ASSERT (size_t (pbyte - bitset) < sizeof bitset);
    }

    return bitset;
}


static void
test_bitset ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{b}\": bitset");

#define BS(str)   mkbitset (str)

    TEST ("%{b}",    0,                       0, 0, "(null)");
    TEST ("%{.1b}",  BS ("1"),                0, 0, "1");
    TEST ("%{.2b}",  BS ("1"),                0, 0, "10");
    TEST ("%{.3b}",  BS ("1"),                0, 0, "100");
    TEST ("%{.3b}",  BS ("101"),              0, 0, "101");
    TEST ("%{.8b}",  BS ("11111111"),         0, 0, "11111111");
    TEST ("%{.16b}", BS ("0000000000000000"), 0, 0, "0000000000000000");
    TEST ("%{.16b}", BS ("0000000000000001"), 0, 0, "0000000000000001");
    TEST ("%{.16b}", BS ("0000000000000010"), 0, 0, "0000000000000010");
    TEST ("%{.16b}", BS ("0000000000000100"), 0, 0, "0000000000000100");
    TEST ("%{.16b}", BS ("0000000000001000"), 0, 0, "0000000000001000");
    TEST ("%{.16b}", BS ("0000000000010000"), 0, 0, "0000000000010000");
    TEST ("%{.16b}", BS ("0000000000100000"), 0, 0, "0000000000100000");
    TEST ("%{.16b}", BS ("0000000001000000"), 0, 0, "0000000001000000");
    TEST ("%{.16b}", BS ("0000000010000000"), 0, 0, "0000000010000000");
    TEST ("%{.16b}", BS ("0000000100000000"), 0, 0, "0000000100000000");
    TEST ("%{.16b}", BS ("0000001000000000"), 0, 0, "0000001000000000");
    TEST ("%{.16b}", BS ("0000010000000000"), 0, 0, "0000010000000000");
    TEST ("%{.16b}", BS ("0000100000000000"), 0, 0, "0000100000000000");
    TEST ("%{.16b}", BS ("0001000000000000"), 0, 0, "0001000000000000");
    TEST ("%{.16b}", BS ("0010000000000000"), 0, 0, "0010000000000000");
    TEST ("%{.16b}", BS ("0100000000000000"), 0, 0, "0100000000000000");
    TEST ("%{.16b}", BS ("1000000000000000"), 0, 0, "1000000000000000");
}

/***********************************************************************/

static void
test_dec (char spec)
{
    const bool sgn = 'u' != spec;

    // "%d", "%i" ////////////////////////////////////////////////////
    printf ("\"%%%c\": %ssigned integer\n", spec, sgn ? "" : "un");

    TEST_SPEC ("%",   0, 0, 0, "0");
    TEST_SPEC ("%",   1, 0, 0, "1");
    TEST_SPEC ("%",   2, 0, 0, "2");
    TEST_SPEC ("%",   3, 0, 0, "3");

    TEST_SPEC ("%",  -1, 0, 0, sgn ? "-1" : "4294967295");
    TEST_SPEC ("%",  -2, 0, 0, sgn ? "-2" : "4294967294");
    TEST_SPEC ("%",  -3, 0, 0, sgn ? "-3" : "4294967293");

    TEST_SPEC ("%+",  4, 0, 0, sgn ? "+4" : "4");
    TEST_SPEC ("%+",  5, 0, 0, sgn ? "+5" : "5");
    TEST_SPEC ("%+",  6, 0, 0, sgn ? "+6" : "6");
    TEST_SPEC ("%+", -4, 0, 0, sgn ? "-4" : "4294967292");
    TEST_SPEC ("%+", -5, 0, 0, sgn ? "-5" : "4294967291");
    TEST_SPEC ("%+", -6, 0, 0, sgn ? "-6" : "4294967290");

    TEST_SPEC ("%",    123, 0, 0, "123");
    TEST_SPEC ("%",   2345, 0, 0, "2345");
    TEST_SPEC ("%",  34567, 0, 0, "34567");
   
    TEST_SPEC ("%",   -124, 0, 0, sgn ? "-124" : "4294967172");
    TEST_SPEC ("%",  -2346, 0, 0, sgn ? "-2346" : "4294964950");
    TEST_SPEC ("%", -34568, 0, 0, sgn ? "-34568" : "4294932728");

    TEST_SPEC ("%", INT_MIN, 0, 0, 0);
    TEST_SPEC ("%", INT_MAX, 0, 0, 0);

    // exercise right justification
    TEST_SPEC ("%0",   3140, 0, 0, "3140");
    TEST_SPEC ("%1",   3141, 0, 0, "3141");
    TEST_SPEC ("%2",   3142, 0, 0, "3142");
    TEST_SPEC ("%3",   3143, 0, 0, "3143");
    TEST_SPEC ("%4",   3144, 0, 0, "3144");
    TEST_SPEC ("%5",   3145, 0, 0, " 3145");
    TEST_SPEC ("%6",   3146, 0, 0, "  3146");
    TEST_SPEC ("%7",   3147, 0, 0, "   3147");
    TEST_SPEC ("%+8",  3148, 0, 0, sgn ? "   +3148"  : "    3148");
    TEST_SPEC ("%9",  -3149, 0, 0, sgn ? "    -3149" : "4294964147");

    // exercise left justification
    TEST_SPEC ("%-0",   4140, 0, 0, "4140");
    TEST_SPEC ("%-1",   4141, 0, 0, "4141");
    TEST_SPEC ("%-2",   4142, 0, 0, "4142");
    TEST_SPEC ("%-3",   4143, 0, 0, "4143");
    TEST_SPEC ("%-4",   4144, 0, 0, "4144");
    TEST_SPEC ("%-5",   4145, 0, 0, "4145 ");
    TEST_SPEC ("%-6",   4146, 0, 0, "4146  ");
    TEST_SPEC ("%-7",   4147, 0, 0, "4147   ");
    TEST_SPEC ("%-8",  -4148, 0, 0, sgn ? "-4148   "  : "4294963148");
    TEST_SPEC ("%+-9",  4149, 0, 0, sgn ? "+4149    " : "4149     ");

    // exercise precision
    TEST_SPEC ("%.0",   5670, 0, 0, "5670");
    TEST_SPEC ("%.1",   5671, 0, 0, "5671");
    TEST_SPEC ("%.2",   5672, 0, 0, "5672");
    TEST_SPEC ("%.3",   5673, 0, 0, "5673");
    TEST_SPEC ("%.4",   5674, 0, 0, "5674");
    TEST_SPEC ("%.5",   5675, 0, 0, "05675");
    TEST_SPEC ("%.6",   5676, 0, 0, "005676");
    TEST_SPEC ("%.7",   5677, 0, 0, "0005677");
    TEST_SPEC ("%.8",  -5678, 0, 0, sgn ? "-00005678"  : "4294961618");
    TEST_SPEC ("%+.9",  5679, 0, 0, sgn ? "+000005679" : "000005679");

    // exercise justification with precision
    TEST_SPEC ("%3.0",   30, 0, 0, " 30");
    TEST_SPEC ("%3.1",   31, 0, 0, " 31");
    TEST_SPEC ("%3.2",   32, 0, 0, " 32");
    TEST_SPEC ("%3.3",   33, 0, 0, "033");
    TEST_SPEC ("%3.4",   34, 0, 0, "0034");
    TEST_SPEC ("%3.5",   35, 0, 0, "00035");
    TEST_SPEC ("%4.5",   45, 0, 0, "00045");
    TEST_SPEC ("%5.5",   55, 0, 0, "00055");
    TEST_SPEC ("%6.5",   65, 0, 0, " 00065");
    TEST_SPEC ("%7.5",   75, 0, 0, "  00075");
    TEST_SPEC ("%8.5",   85, 0, 0, "   00085");
    TEST_SPEC ("%9.5",   95, 0, 0, "    00095");
    TEST_SPEC ("%9.6",  -96, 0, 0, sgn ? "  -000096"  : "4294967200");
    TEST_SPEC ("%+9.7",  97, 0, 0, sgn ? " +0000097"  : "  0000097");
    TEST_SPEC ("%+-9.8", 98, 0, 0, sgn ? "+00000098"  : "00000098 ");
    TEST_SPEC ("%-+9.9", 99, 0, 0, sgn ? "+000000099" : "000000099");

    // exercise edge cases

    // 7.19.6.1 of ISO/IEC 9899:1999:
    //   The result of converting a zero value with a precision
    //   of zero is no characters.
    TEST_SPEC ("%.0",    0, 0, 0, "");
    TEST_SPEC ("%1.0",   0, 0, 0, " ");
    TEST_SPEC ("%2.0",   0, 0, 0, "  ");
    TEST_SPEC ("%+3.0",  0, 0, 0, "   ");
    TEST_SPEC ("%-4.0",  0, 0, 0, "    ");

    // 7.19.6.1, p5 of ISO/IEC 9899:1999:
    //   A negative field width argument is taken as
    //   a - flag followed by a positive field width.
    //   A negative precision argument is taken as
    //   if the precision were omitted.

    TEST_SPEC ("%*",    0,  0, 0, "0");
    TEST_SPEC ("%*",    1,  1, 0, "1");
    TEST_SPEC ("%*",    2,  2, 0, " 2");
    TEST_SPEC ("%*",    3,  3, 0, "  3");
    TEST_SPEC ("%*",   -4, -4, 0, sgn ? "-4  " : "4294967292");
    TEST_SPEC ("%-*",   5,  5, 0, "5    ");
    TEST_SPEC ("%-*",  -6, -6, 0, sgn ? "-6    " : "4294967290");

    TEST_SPEC ("%*.*",   0,  0,  0, "");
    TEST_SPEC ("%*.*",   1,  0,  0, " ");
    TEST_SPEC ("%*.*",   2,  0,  0, "  ");
    TEST_SPEC ("%*.*",   2,  0,  1, " 1");
    TEST_SPEC ("%*.*",   2,  1,  2, " 2");
    TEST_SPEC ("%*.*",   2,  2,  2, "02");
    TEST_SPEC ("%*.*",  -3,  2,  3, "03 ");
    TEST_SPEC ("%-*.*", -4,  2, -4, sgn ? "-04 " : "4294967292");
    TEST_SPEC ("%-*.*", -4, -2, -4, sgn ? "-4  " : "4294967292");

    // "%hhd", "%hhi", ///////////////////////////////////////////////
    printf ("\"%%hh%c\": %ssigned char\n", spec, sgn ? "" : "un");

    TEST_SPEC ("%hh", '\0',   0, 0, "0");
    TEST_SPEC ("%hh", '\1',   0, 0, "1");
    TEST_SPEC ("%hh", '\2',   0, 0, "2");
    TEST_SPEC ("%hh", '\x7f', 0, 0, "127");

    TEST_SPEC ("%hh", '\x80', 0, 0, sgn ? "-128" : "128");
    TEST_SPEC ("%hh", '\xff', 0, 0, sgn ? "-1"   : "255");

    // "%hd", "%hi" //////////////////////////////////////////////////
    printf ("\"%%h%c\": %ssigned short\n", spec, sgn ? "" : "un");

    TEST_SPEC ("%h", short (0), 0, 0, "0");
    TEST_SPEC ("%h", short (1), 0, 0, "1");
    TEST_SPEC ("%h", short (2), 0, 0, "2");

    TEST_SPEC ("%h", SHRT_MIN, 0, 0, 0);
    TEST_SPEC ("%h", SHRT_MAX, 0, 0, 0);

    // "%ld", "%li" //////////////////////////////////////////////////
    printf ("\"%%l%c\": signed long\n", spec);

    TEST_SPEC ("%l", 0L,       0, 0, "0");
    TEST_SPEC ("%l", 1L,       0, 0, "1");
    TEST_SPEC ("%l", 2L,       0, 0, "2");
    TEST_SPEC ("%l", LONG_MAX, 0, 0, 0);

    TEST_SPEC ("%l", -1L,      0, 0, 0);
    TEST_SPEC ("%l", -2L,      0, 0, 0);
    TEST_SPEC ("%l", LONG_MIN, 0, 0, 0);

#ifndef _RWSTD_NO_LONG_LONG

    // "%lld", "%lli" ////////////////////////////////////////////////
    printf ("\"%%ll%c\": signed long long\n", spec);

    const _RWSTD_LONG_LONG llong_min = _RWSTD_LLONG_MIN;
    const _RWSTD_LONG_LONG llong_max = _RWSTD_LLONG_MAX;

    TEST_SPEC ("%ll", 0LL,           0, 0, "0");
    TEST_SPEC ("%ll", 1LL,           0, 0, "1");
    TEST_SPEC ("%ll", 12LL,          0, 0, "12");
    TEST_SPEC ("%ll", 123LL,         0, 0, "123");
    TEST_SPEC ("%ll", 1234LL,        0, 0, "1234");
    TEST_SPEC ("%ll", 12345LL,       0, 0, "12345");
    TEST_SPEC ("%ll", 123456LL,      0, 0, "123456");
    TEST_SPEC ("%ll", 1234567LL,     0, 0, "1234567");
    TEST_SPEC ("%ll", 12345678LL,    0, 0, "12345678");
    TEST_SPEC ("%ll", 123456789LL,   0, 0, "123456789");

#  if 4 == _RWSTD_LLONG_SIZE

    // FIXME: exercise 32-bit negative long long

    fprintf (stderr, "Warning: %s\n", "\"%lld\" not exercised "
             "with negative values for %u-bit long long",
             sizeof (RWSTD_LONG_LONG) * CHAR_BIT);

#  elif 8 == _RWSTD_LLONG_SIZE

    TEST_SPEC ("%ll", -1LL, 0, 0, sgn ? "-1" : "18446744073709551615");
    TEST_SPEC ("%ll", -2LL, 0, 0, sgn ? "-2" : "18446744073709551614");
    TEST_SPEC ("%ll", -3LL, 0, 0, sgn ? "-3" : "18446744073709551613");

    TEST_SPEC ("%ll", llong_min,     0, 0,
               sgn ? "-9223372036854775808" : "9223372036854775808");
    TEST_SPEC ("%ll", llong_min + 1, 0, 0,
               sgn ? "-9223372036854775807" : "9223372036854775809");
    TEST_SPEC ("%ll", llong_min + 2, 0, 0,
               sgn ? "-9223372036854775806" : "9223372036854775810");

    TEST_SPEC ("%ll", llong_max,     0, 0, "9223372036854775807");
    TEST_SPEC ("%ll", llong_max - 1, 0, 0, "9223372036854775806");

#  endif

#else   // if defined (_RWSTD_NO_LONG_LONG)

    fprintf (stderr, "Warning: %s\n", "\"%lld\" not exercised");

#endif   // _RWSTD_NO_LONG_LONG
}

/***********************************************************************/

static void
test_oct ()
{
    printf ("%s\n", "\"%o\": octal integer");

    TEST ("%o",  0, 0, 0, "0");
    TEST ("%o",  1, 0, 0, "1");
    TEST ("%o",  2, 0, 0, "2");
    TEST ("%o",  3, 0, 0, "3");
    TEST ("%o",  4, 0, 0, "4");
    TEST ("%o",  5, 0, 0, "5");
    TEST ("%o",  6, 0, 0, "6");
    TEST ("%o",  7, 0, 0, "7");
    TEST ("%o",  8, 0, 0, "10");
    TEST ("%o",  9, 0, 0, "11");
    TEST ("%o", 10, 0, 0, "12");

    TEST ("%#o", 11, 0, 0, "013");
}

/***********************************************************************/

static void
test_hex (char spec)
{
    printf ("\"%%%c\": hexadecimal integer\n", spec);

    // exercise by comparing against libc sprintf()
    TEST_SPEC ("%",    0, 0, 0, 0);
    TEST_SPEC ("%",    1, 0, 0, 0);
    TEST_SPEC ("%",    2, 0, 0, 0);
    TEST_SPEC ("%",    3, 0, 0, 0);
    TEST_SPEC ("%",    9, 0, 0, 0);
    TEST_SPEC ("%",   10, 0, 0, 0);
    TEST_SPEC ("%",   11, 0, 0, 0);
    TEST_SPEC ("%",   12, 0, 0, 0);
    TEST_SPEC ("%",   13, 0, 0, 0);
    TEST_SPEC ("%",   14, 0, 0, 0);
    TEST_SPEC ("%",   15, 0, 0, 0);
    TEST_SPEC ("%",  123, 0, 0, 0);
    TEST_SPEC ("%",  234, 0, 0, 0);
    TEST_SPEC ("%",  345, 0, 0, 0);
    TEST_SPEC ("%",   -1, 0, 0, 0);
    TEST_SPEC ("%",   -2, 0, 0, 0);
    TEST_SPEC ("%",   -3, 0, 0, 0);
    TEST_SPEC ("%",   -4, 0, 0, 0);

    TEST_SPEC ("%+",   0, 0, 0, 0);
    TEST_SPEC ("%+",   1, 0, 0, 0);
    TEST_SPEC ("%+",   2, 0, 0, 0);
    TEST_SPEC ("%+",   3, 0, 0, 0);
    TEST_SPEC ("%+",   4, 0, 0, 0);
    TEST_SPEC ("%+",   5, 0, 0, 0);
    TEST_SPEC ("%+",   6, 0, 0, 0);
    TEST_SPEC ("%+",  15, 0, 0, 0);
    TEST_SPEC ("%+",  16, 0, 0, 0);

    TEST_SPEC ("%+",  -1, 0, 0, 0);
    TEST_SPEC ("%+",  -2, 0, 0, 0);
    TEST_SPEC ("%+",  -3, 0, 0, 0);
    TEST_SPEC ("%+",  -4, 0, 0, 0);
    TEST_SPEC ("%+",  -5, 0, 0, 0);
    TEST_SPEC ("%+",  -6, 0, 0, 0);
    TEST_SPEC ("%+", -15, 0, 0, 0);
    TEST_SPEC ("%+", -16, 0, 0, 0);

    TEST_SPEC ("%#",   0, 0, 0, "0");
    TEST_SPEC ("%#",   1, 0, 0, 0);
    TEST_SPEC ("%#",  20, 0, 0, 0);
    TEST_SPEC ("%#", -30, 0, 0, 0);

    TEST_SPEC ("%0",   0, 0, 0, "0");
    TEST_SPEC ("%0",   2, 0, 0, 0);
    TEST_SPEC ("%0",  21, 0, 0, 0);
    TEST_SPEC ("%0", -32, 0, 0, 0);

    // exercise right justification
    TEST_SPEC ("%0",    1000, 0, 0, 0);
    TEST_SPEC ("%1",    1001, 0, 0, 0);
    TEST_SPEC ("%2",    1002, 0, 0, 0);
    TEST_SPEC ("%3",    1003, 0, 0, 0);
    TEST_SPEC ("%4",    1004, 0, 0, 0);
    TEST_SPEC ("%5",    1005, 0, 0, 0);
    TEST_SPEC ("%6",    1006, 0, 0, 0);
    TEST_SPEC ("%7",    1007, 0, 0, 0);
    TEST_SPEC ("%8",    1008, 0, 0, 0);
    TEST_SPEC ("%9",    1009, 0, 0, 0);
    TEST_SPEC ("%10",   1010, 0, 0, 0);

    TEST_SPEC ("%11",  -1011, 0, 0, 0);
    TEST_SPEC ("%12",  -1012, 0, 0, 0);
    TEST_SPEC ("%13",  -1013, 0, 0, 0);
    TEST_SPEC ("%14",  -1014, 0, 0, 0);
    TEST_SPEC ("%15",  -1015, 0, 0, 0);
    TEST_SPEC ("%16",  -1016, 0, 0, 0);
    TEST_SPEC ("%17",  -1017, 0, 0, 0);
    TEST_SPEC ("%18",  -1018, 0, 0, 0);
    TEST_SPEC ("%19",  -1019, 0, 0, 0);
    TEST_SPEC ("%20",  -1020, 0, 0, 0);

    // exercise left justification
    TEST_SPEC ("%-0",   2000, 0, 0, 0);
    TEST_SPEC ("%-1",   2001, 0, 0, 0);
    TEST_SPEC ("%-2",   2002, 0, 0, 0);
    TEST_SPEC ("%-3",   2003, 0, 0, 0);
    TEST_SPEC ("%-4",   2004, 0, 0, 0);
    TEST_SPEC ("%-5",   2005, 0, 0, 0);
    TEST_SPEC ("%-6",   2006, 0, 0, 0);
    TEST_SPEC ("%-7",   2007, 0, 0, 0);
    TEST_SPEC ("%-8",   2008, 0, 0, 0);
    TEST_SPEC ("%-9",   2009, 0, 0, 0);
    TEST_SPEC ("%-10",  2010, 0, 0, 0);

    TEST_SPEC ("%-11", -2011, 0, 0, 0);
    TEST_SPEC ("%-12", -2012, 0, 0, 0);
    TEST_SPEC ("%-13", -2013, 0, 0, 0);
    TEST_SPEC ("%-14", -2014, 0, 0, 0);
    TEST_SPEC ("%-15", -2015, 0, 0, 0);
    TEST_SPEC ("%-16", -2016, 0, 0, 0);
    TEST_SPEC ("%-17", -2017, 0, 0, 0);
    TEST_SPEC ("%-18", -2018, 0, 0, 0);
    TEST_SPEC ("%-19", -2019, 0, 0, 0);
    TEST_SPEC ("%-20", -2020, 0, 0, 0);

    // exercise precision
    TEST_SPEC ("%.0",   3000, 0, 0, 0);
    TEST_SPEC ("%.1",   3001, 0, 0, 0);
    TEST_SPEC ("%.2",   3002, 0, 0, 0);
    TEST_SPEC ("%.3",   3003, 0, 0, 0);
    TEST_SPEC ("%.4",   3004, 0, 0, 0);
    TEST_SPEC ("%.5",   3005, 0, 0, 0);
    TEST_SPEC ("%.6",   3006, 0, 0, 0);
    TEST_SPEC ("%.7",   3007, 0, 0, 0);
    TEST_SPEC ("%.8",   3008, 0, 0, 0);
    TEST_SPEC ("%.9",   3009, 0, 0, 0);
    TEST_SPEC ("%.10",  3010, 0, 0, 0);

    TEST_SPEC ("%+.0",  4000, 0, 0, 0);
    TEST_SPEC ("%+.1",  4001, 0, 0, 0);
    TEST_SPEC ("%+.2",  4002, 0, 0, 0);
    TEST_SPEC ("%+.3",  4003, 0, 0, 0);
    TEST_SPEC ("%+.4",  4004, 0, 0, 0);
    TEST_SPEC ("%+.5",  4005, 0, 0, 0);
    TEST_SPEC ("%+.6",  4006, 0, 0, 0);
    TEST_SPEC ("%+.7",  4007, 0, 0, 0);
    TEST_SPEC ("%+.8",  4008, 0, 0, 0);
    TEST_SPEC ("%+.9",  4009, 0, 0, 0);
    TEST_SPEC ("%+.10", 4010, 0, 0, 0);

    // exercise justification with precision
    TEST_SPEC ("%+-.0",  5000, 0, 0, 0);
    TEST_SPEC ("%+-.1",  5001, 0, 0, 0);
    TEST_SPEC ("%+-.2",  5002, 0, 0, 0);
    TEST_SPEC ("%+-.3",  5003, 0, 0, 0);
    TEST_SPEC ("%+-.4",  5004, 0, 0, 0);
    TEST_SPEC ("%+-.5",  5005, 0, 0, 0);
    TEST_SPEC ("%+-.6",  5006, 0, 0, 0);
    TEST_SPEC ("%+-.7",  5007, 0, 0, 0);
    TEST_SPEC ("%+-.8",  5008, 0, 0, 0);
    TEST_SPEC ("%+-.9",  5009, 0, 0, 0);
    TEST_SPEC ("%+-.10", 5010, 0, 0, 0);

    TEST_SPEC ("%-+.0",  5020, 0, 0, 0);
    TEST_SPEC ("%-+.1",  5021, 0, 0, 0);
    TEST_SPEC ("%-+.2",  5022, 0, 0, 0);
    TEST_SPEC ("%-+.3",  5023, 0, 0, 0);
    TEST_SPEC ("%-+.4",  5024, 0, 0, 0);
    TEST_SPEC ("%-+.5",  5025, 0, 0, 0);
    TEST_SPEC ("%-+.6",  5026, 0, 0, 0);
    TEST_SPEC ("%-+.7",  5027, 0, 0, 0);
    TEST_SPEC ("%-+.8",  5028, 0, 0, 0);
    TEST_SPEC ("%-+.9",  5029, 0, 0, 0);
    TEST_SPEC ("%-+.10", 5020, 0, 0, 0);

    // exercise edge cases

    // 7.19.6.1 of ISO/IEC 9899:1999:
    //   The result of converting a zero value with a precision
    //   of zero is no characters.
    TEST_SPEC ("%.0",   0, 0, 0, "");
    TEST_SPEC ("%1.0",  0, 0, 0, " ");
    TEST_SPEC ("%2.0",  0, 0, 0, "  ");
    TEST_SPEC ("%+3.0", 0, 0, 0, "   ");
    TEST_SPEC ("%-4.0", 0, 0, 0, "    ");

    // 7.19.6.1, p5 of ISO/IEC 9899:1999:
    //   A negative field width argument is taken as
    //   a - flag followed by a positive field width.
    //   A negative precision argument is taken as
    //   if the precision were omitted.

    TEST_SPEC ("%*",     0,  0, 0, 0);
    TEST_SPEC ("%*",     1,  1, 0, 0);
    TEST_SPEC ("%*",     2,  2, 0, 0);
    TEST_SPEC ("%*",     3,  3, 0, 0);
    TEST_SPEC ("%*",    -4, -4, 0, 0);
    TEST_SPEC ("%-*",    5,  5, 0, 0);
    TEST_SPEC ("%-*",   -6, -6, 0, 0);

    TEST_SPEC ("%*.*",   0,  0,  0, 0);
    TEST_SPEC ("%*.*",   1,  0,  0, 0);
    TEST_SPEC ("%*.*",   2,  0,  0, 0);
    TEST_SPEC ("%*.*",   2,  0,  1, 0);
    TEST_SPEC ("%*.*",   2,  1,  2, 0);
    TEST_SPEC ("%*.*",   2,  2,  2, 0);
    TEST_SPEC ("%*.*",  -3,  2,  3, 0);
    TEST_SPEC ("%-*.*", -4,  2, -4, 0);
    TEST_SPEC ("%-*.*", -4, -2, -4, 0);

    // "%hhx", "%hhx", ///////////////////////////////////////////////
    printf ("\"%%hh%c\": hexadecimal char\n", spec);

    TEST_SPEC ("%hh", '\0',   0, 0, "0");
    TEST_SPEC ("%hh", '\1',   0, 0, "1");
    TEST_SPEC ("%hh", '\2',   0, 0, "2");
    TEST_SPEC ("%hh", '\x7f', 0, 0, ('x' == spec ? "7f" : "7F"));
    TEST_SPEC ("%hh", '\x80', 0, 0, "80");
    TEST_SPEC ("%hh", '\xff', 0, 0, ('x' == spec ? "ff" : "FF"));

    TEST_SPEC ("%#hh", '\0',   0, 0, "0");
    TEST_SPEC ("%#hh", '\1',   0, 0, ('x' == spec ? "0x1" : "0X1"));
    TEST_SPEC ("%#hh", '\2',   0, 0, ('x' == spec ? "0x2" : "0X2"));
    TEST_SPEC ("%#hh", '\x7f', 0, 0, ('x' == spec ? "0x7f" : "0X7F"));
    TEST_SPEC ("%#hh", '\x80', 0, 0, ('x' == spec ? "0x80" : "0X80"));
    TEST_SPEC ("%#hh", '\xff', 0, 0, ('x' == spec ? "0xff" : "0XFF"));

    // "%hx", "%hhX" /////////////////////////////////////////////////
    printf ("\"%%h%c\": hexadecimal short\n", spec);

    TEST_SPEC ("%h", short (0), 0, 0, "0");
    TEST_SPEC ("%h", short (1), 0, 0, "1");
    TEST_SPEC ("%h", short (2), 0, 0, "2");

    TEST_SPEC ("%h", SHRT_MIN, 0, 0, 0);
    TEST_SPEC ("%h", SHRT_MAX, 0, 0, 0);
}

/***********************************************************************/

static void
test_bool ()
{
    printf ("%s\n", "extension: \"%b\": bool");

    TEST ("%b", false, 0, 0, "false");
    TEST ("%b", true,  0, 0, "true");

    TEST ("%b", '\0',    0, 0, "false");
    TEST ("%b", '\x01',  0, 0, "true");
    TEST ("%b", '\x80',  0, 0, "true");
    TEST ("%b", '\xff',  0, 0, "true");

    TEST ("%b",  0, 0, 0, "false");
    TEST ("%b", -1, 0, 0, "true");
    TEST ("%b", +1, 0, 0, "true");
    TEST ("%b", -2, 0, 0, "true");
    TEST ("%b", +2, 0, 0, "true");
}

/***********************************************************************/

static void
test_integer ()
{
    test_dec ('d');
    test_dec ('i');
    test_dec ('u');

    test_oct ();

    test_hex ('x');
    test_hex ('X');

    test_bool ();
}

/***********************************************************************/

void* make_array (int width,   // element width in bytes
                  int a0 = 0, int a1 = 0, int a2 = 0, int a3 = 0,
                  int a4 = 0, int a5 = 0, int a6 = 0, int a7 = 0,
                  int a8 = 0, int a9 = 0, int a10 = 0, int a11 = 0,
                  int a12 = 0, int a13 = 0, int a14 = 0, int a15 = 0)
{
    RW_ASSERT (8 == width || 4 == width || 2 == width || 1 == width);

#ifdef _RWSTD_INT64_T
    typedef _RWSTD_UINT64_T ui64_t;
    typedef _RWSTD_INT64_T  i64_t;
#else
    typedef _RWSTD_UINT32_T ui64_t;
    typedef _RWSTD_INT32_T  i64_t;
#endif
    typedef _RWSTD_UINT32_T ui32_t;
    typedef _RWSTD_INT32_T  i32_t;
    typedef _RWSTD_INT16_T  ui16_t;
    typedef _RWSTD_INT16_T  i16_t;
    typedef _RWSTD_INT8_T   ui8_t;
    typedef _RWSTD_INT8_T   i8_t;

    static union {
        i64_t i64;
        i32_t i32;
        i16_t i16;
        i8_t  i8;
    } array [17];

    union {
        i64_t* pi64;
        i32_t* pi32;
        i16_t* pi16;
        i8_t*  pi8;
    } ptr = { &array [0].i64 };

#define ADD_ELEMENT(n)                                          \
    switch (width) {                                            \
    case 8 /* bytes */: *ptr.pi64++ = i64_t (a##n); break;      \
    case 4 /* bytes */: *ptr.pi32++ = i32_t (a##n); break;      \
    case 2 /* bytes */: *ptr.pi16++ = i16_t (a##n); break;      \
    case 1 /* byte  */: *ptr.pi8++  = i8_t (a##n); break;       \
    } (void)0

    ADD_ELEMENT ( 0); ADD_ELEMENT ( 1); ADD_ELEMENT ( 2); ADD_ELEMENT ( 3);
    ADD_ELEMENT ( 4); ADD_ELEMENT ( 5); ADD_ELEMENT ( 6); ADD_ELEMENT ( 7);
    ADD_ELEMENT ( 8); ADD_ELEMENT ( 9); ADD_ELEMENT (10); ADD_ELEMENT (11);
    ADD_ELEMENT (12); ADD_ELEMENT (13); ADD_ELEMENT (14); ADD_ELEMENT (15);

    // zero-terminate
    const int a16 = 0;
    ADD_ELEMENT (16);

    return array;
}

static void
test_intarray ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{Ao}\": array of octal integers");

#define AR   make_array

    // null 1, 2, 4, and 8-byte integer arrays
    TEST ("%{1Ao}", 0, 0, 0, "(null)");
    TEST ("%{2Ao}", 0, 0, 0, "(null)");
    TEST ("%{4Ao}", 0, 0, 0, "(null)");

#ifdef _RWSTD_INT64_T
    TEST ("%{8Ao}", 0, 0, 0, "(null)");
#endif   // _RWSTD_INT64_T

    // 2-byte integer arrays
    TEST ("%{2Ao}",   AR (2, 0),             0, 0, "");
    TEST ("%{2Ao}",   AR (2, 1, 2),          0, 0, "1,2");
    TEST ("%{2Ao}",   AR (2, 2, 3, 4),       0, 0, "2,3,4");
    TEST ("%{2Ao}",   AR (2, 3, 4, 5, 6),    0, 0, "3,4,5,6");
    TEST ("%{2Ao}",   AR (2, 4, 5, 6, 7, 8), 0, 0, "4,5,6,7,10");

    TEST ("%{*Ao}",   2, AR (2, 4, 5, 6, 7, 8), 0, "4,5,6,7,10");
    TEST ("%{*.*Ao}", 2, 2, AR (2, 4, 0, 6, 0, 8), "4,0");
    TEST ("%{*.*Ao}", 2, 3, AR (2, 4, 0, 6, 0, 8), "4,0,6");
    TEST ("%{*.*Ao}", 2, 4, AR (2, 4, 0, 6, 0, 8), "4,0,6,0");
    TEST ("%{*.*Ao}", 2, 5, AR (2, 4, 0, 6, 0, 8), "4,0,6,0,10");

    // the pound flag alone has no affect on the '0' prefix
    TEST ("%{#2Ao}",  AR (2, 5, 6, 7, 8, 9), 0, 0, "5,6,7,10,11");
    // zero and pound flags add the '0' prefix
    TEST ("%{0#2Ao}", AR (2, 6, 7, 8, 9, 10), 0, 0, "06,07,010,011,012");

    _RWSTD_INT8_T array8 [] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 /* terminate */
    };

    _RWSTD_INT16_T array16 [] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 /* terminate */
    };

    _RWSTD_INT32_T array32 [] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 /* terminate */
    };

#ifdef _RWSTD_INT64_T

    _RWSTD_INT64_T array64 [] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 /* terminate */
    };

   // set array element at index inx to value val
#  define SET_ELEM(inx, val)                    \
      RW_ASSERT (unsigned (inx) < array_size);  \
      array8  [inx] = val; array16 [inx] = val; \
      array32 [inx] = val; array64 [inx] = val; \
      array_str [inx * 2] = '0' + val

#else   // if !defined (_RWSTD_INT64_T)

   // set array element at index inx to value val
#  define SET_ELEM(inx, val)                    \
      RW_ASSERT (unsigned (inx) < array_size);  \
      array8  [inx] = val; array16 [inx] = val; \
      array32 [inx] = val;                      \
      array_str [inx * 2] = '0' + val

#endif   // _RWSTD_INT64_T

    const unsigned array_size = sizeof array16 / sizeof *array16;

    char array_str [2 * array_size] = {
        "1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,"
        "1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1"
    };

    TEST ("%{1Ao}",  array8, 0, 0, array_str);
    TEST ("%{#1Ao}", array8, 0, 0, "{ 1 <repeats 32 times> }");

    TEST ("%{2Ao}",  array16, 0, 0, array_str);
    TEST ("%{#2Ao}", array16, 0, 0, "{ 1 <repeats 32 times> }");

    TEST ("%{4Ao}",  array32, 0, 0, array_str);
    TEST ("%{#4Ao}", array32, 0, 0, "{ 1 <repeats 32 times> }");

    SET_ELEM (1, 2);

    TEST ("%{2Ao}",  array16, 0, 0, array_str);
    TEST ("%{#2Ao}", array16, 0, 0, "{ 1,2,1 <repeats 30 times> }");

    SET_ELEM ( 1, 1);
    SET_ELEM (30, 2);

    TEST ("%{2Ao}",  array16, 0, 0, array_str);
    TEST ("%{#2Ao}", array16, 0, 0, "{ 1 <repeats 30 times>,2,1 }");

    SET_ELEM (30, 1);
    SET_ELEM (31, 2);

    TEST ("%{2Ao}",  array16, 0, 0, array_str);
    TEST ("%{#2Ao}", array16, 0, 0, "{ 1 <repeats 31 times>,2 }");

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{Ad}\": array of decimal integers");

    TEST ("%{4Ad}",   AR (4, 0),                  0, 0, "");
    TEST ("%{4Ad}",   AR (4, 20, 31),             0, 0, "20,31");
    TEST ("%{4Ad}",   AR (4, 21, 32, 43),         0, 0, "21,32,43");
    TEST ("%{4Ad}",   AR (4, 22, 33, 44, 55),     0, 0, "22,33,44,55");
    TEST ("%{4Ad}",   AR (4, 23, 34, 45, 56, 67), 0, 0, "23,34,45,56,67");

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{Ax}\": array of hexadecimal integers");

    TEST ("%{4Ax}",   AR (4, 0),                      0, 0, "");
    TEST ("%{4Ax}",   AR (4, 0xa, 0xb),               0, 0, "a,b");
    TEST ("%{4Ax}",   AR (4, 0xb, 0xc, 0xd),          0, 0, "b,c,d");
    TEST ("%{4Ax}",   AR (4, 0xc, 0xd, 0xe, 0xf),     0, 0, "c,d,e,f");
    TEST ("%{4Ax}",   AR (4, 0xc9, 0xda, 0xeb, 0xfc), 0, 0, "c9,da,eb,fc");

    TEST ("%{#4Ax}",  AR (4, 0xd, 0xe, 0xa, 0xd), 0, 0, "d,e,a,d");
    TEST ("%{0#4Ax}", AR (4, 0xb, 0xe, 0xe, 0xf), 0, 0, "0xb,0xe,0xe,0xf");
}

/***********************************************************************/

static void
test_floating ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "\"%e\": scientific floating point notation");

    // double formatting
    TEST ("%e",   0.0,  0, 0, "0.000000e+00");
    TEST ("%e",   1.0,  0, 0, "1.000000e+00");
    TEST ("%e",  -1.0,  0, 0, "-1.000000e+00");
    TEST ("%e",  10.0,  0, 0, "1.000000e+01");
    TEST ("%e", -10.0,  0, 0, "-1.000000e+01");
    TEST ("%e",  10.1,  0, 0, "1.010000e+01");
    TEST ("%e", -10.1,  0, 0, "-1.010000e+01");

    // long double formatting
    TEST ("%Le",   0.0L,  0, 0, "0.000000e+00");
    TEST ("%Le",   1.0L,  0, 0, "1.000000e+00");
    TEST ("%Le",  -1.0L,  0, 0, "-1.000000e+00");
    TEST ("%Le",  10.0L,  0, 0, "1.000000e+01");
    TEST ("%Le", -10.0L,  0, 0, "-1.000000e+01");
    TEST ("%Le",  10.1L,  0, 0, "1.010000e+01");
    TEST ("%Le", -10.1L,  0, 0, "-1.010000e+01");

    TEST ("%Le",  1.1e+01L,  0, 0, "1.100000e+01");
    TEST ("%Le",  1.2e+10L,  0, 0, "1.200000e+10");
    TEST ("%Le",  1.3e+12L,  0, 0, "1.300000e+12");

#if 100 < _RWSTD_LDBL_MAX_10_EXP

    // especially exercise the correct number of zeros in the exponent
    // to verify that the function corrects MSVC's screwed up formatting
    // without messing it up even more than it is (see PR #27946)
    TEST ("%Le",  1.4e+100L, 0, 0, "1.400000e+100");
    TEST ("%Le",  1.5e+120L, 0, 0, "1.500000e+120");
    TEST ("%Le",  1.6e+123L, 0, 0, "1.600000e+123");
#endif

#if 1000 < _RWSTD_LDBL_MAX_10_EXP
    TEST ("%Le",  1.7e+1000L,  0, 0, "1.700000e+1000");
    TEST ("%Le",  1.8e+1200L,  0, 0, "1.800000e+1200");
    TEST ("%Le",  1.9e+1230L,  0, 0, "1.900000e+1230");
    TEST ("%Le",  2.0e+1234L,  0, 0, "2.000000e+1234");
#endif

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "\"%E\": scientific floating point notation");

    fprintf (stderr, "Warning: %s\n", "\"%E\" not exercised");

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "\"%f\": fixed floating point notation");

    fprintf (stderr, "Warning: %s\n", "\"%f\" not exercised");

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "\"%F\": fixed floating point notation");

    fprintf (stderr, "Warning: %s\n", "\"%F\" not exercised");

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "\"%g\": value-dependent floating point notation");

    fprintf (stderr, "Warning: %s\n", "\"%g\" not exercised");

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "\"%G\": value-dependent floating point notation");

    fprintf (stderr, "Warning: %s\n", "\"%G\" not exercised");
}

/***********************************************************************/

static void
test_pointer ()
{
    printf ("%s\n", "\"%p\": void pointer");

    const char spec = 'p';

#if 4 == _RWSTD_PTR_SIZE

    TEST_SPEC ("%",  (void*)0,          0, 0, "00000000");
    TEST_SPEC ("%",  (void*)1,          0, 0, "00000001");
    TEST_SPEC ("%",  (void*)0xffffffff, 0, 0, "ffffffff");

    TEST_SPEC ("%#", (void*)0,          0, 0, "0x00000000");
    TEST_SPEC ("%#", (void*)0x123,      0, 0, "0x00000123");
    TEST_SPEC ("%#", (void*)0xffffffff, 0, 0, "0xffffffff");

#elif 8 == _RWSTD_PTR_SIZE

    TEST_SPEC ("%", (void*)0,           0, 0, "0000000000000000");
    TEST_SPEC ("%", (void*)1,           0, 0, "0000000000000001");
    TEST_SPEC ("%", (void*)0xffffffff,  0, 0, "00000000ffffffff");

    TEST_SPEC ("%#", (void*)0,          0, 0, "0x0000000000000000");
    TEST_SPEC ("%#", (void*)0x123,      0, 0, "0x0000000000000123");
    TEST_SPEC ("%#", (void*)0xffffffff, 0, 0, "0x00000000ffffffff");

#endif
}

/***********************************************************************/

extern "C" int test_function (int i)
{
    return i ? i + test_function (-1) : 0;
}


static void
test_funptr ()
{
    printf ("%s\n", "extension: \"%{f}\": function pointer");

    typedef void (*funptr_t)();

#if 4 == _RWSTD_PTR_SIZE

    TEST ("%{f}",  (funptr_t)0,          0, 0, "00000000");
    TEST ("%{f}",  (funptr_t)1,          0, 0, "00000001");
    TEST ("%{f}",  (funptr_t)0xffffffff, 0, 0, "ffffffff");

    TEST ("%{#f}", (funptr_t)0,          0, 0, "0x00000000");
    TEST ("%{#f}", (funptr_t)0x123,      0, 0, "0x00000123");
    TEST ("%{#f}", (funptr_t)0xffffffff, 0, 0, "0xffffffff");

#  ifndef _RWSTD_NO_SPRINTFA_FUNNAME

    char output [64];
    void* funaddr = (void*)&test_function;
    sprintf (output, "%#x=test_function+0", funaddr);

    TEST ("%{lf}", (funptr_t)&test_function, 0, 0, output);

    funaddr = (char*)funaddr + 4;
    sprintf (output, "%#x=test_function+4", funaddr);

    TEST ("%{lf}", (funptr_t)funaddr, 0, 0, output);

#  endif   // _RWSTD_NO_SPRINTFA_FUNNAME

#elif 8 == _RWSTD_PTR_SIZE

    TEST ("%{f}", (funptr_t)0,           0, 0, "0000000000000000");
    TEST ("%{f}", (funptr_t)1,           0, 0, "0000000000000001");
    TEST ("%{f}", (funptr_t)0xffffffff,  0, 0, "00000000ffffffff");

    TEST ("%{#f}", (funptr_t)0,          0, 0, "0x0000000000000000");
    TEST ("%{#f}", (funptr_t)0x123,      0, 0, "0x0000000000000123");
    TEST ("%{#f}", (funptr_t)0xffffffff, 0, 0, "0x00000000ffffffff");

#  ifndef _RWSTD_NO_SPRINTFA_FUNNAME

    char output [64];
    sprintf (output, "%#x=test_function+0", (void*)&test_function);

    TEST ("%{lf}", (funptr_t)&test_function, 0, 0, output);

    sprintf (output, "%#x=test_function+0", (char*)&test_function + 32);
    TEST ("%{lf}", (funptr_t)((char*)&test_funptr + 32), 0, 0, output);

#  endif   // _RWSTD_NO_SPRINTFA_FUNNAME

#endif
}

/***********************************************************************/

static void
test_memptr ()
{
    printf ("%s\n", "extension: \"%{M}\": member pointer");

    struct MyClass { };
    typedef void (MyClass::*memptr_t)();

    union {
        memptr_t mptr;
        long lval [sizeof (memptr_t) / sizeof (long) + 1];
    } uval;

    if (sizeof (memptr_t) <= sizeof (long)) {

#if 4 == _RWSTD_LONG_SIZE

        uval.lval [0] = 0UL;
        TEST ("%{M}",  uval.mptr, 0, 0,   "00000000");
        TEST ("%{#M}", uval.mptr, 0, 0, "0x00000000");

        uval.lval [0] = 1UL;
        TEST ("%{M}",  uval.mptr, 0, 0,   "00000001");
        TEST ("%{#M}", uval.mptr, 0, 0, "0x00000001");

        uval.lval [0] = 0xffffffffUL;
        TEST ("%{M}",  uval.mptr, 0, 0,   "ffffffff");
        TEST ("%{#M}", uval.mptr, 0, 0, "0xffffffff");

#elif 8 == _RWSTD_LONG_SIZE

        uval.lval [0] = 0UL;
        TEST ("%{M}",  uval.mptr, 0, 0,   "0000000000000000");
        TEST ("%{#M}", uval.mptr, 0, 0, "0x0000000000000000");

        uval.lval [0] = 1UL;
        TEST ("%{M}",  uval.mptr, 0, 0,   "0000000000000001");
        TEST ("%{#M}", uval.mptr, 0, 0, "0x0000000000000001");

        uval.lval [0] = 0xffffffffUL;
        TEST ("%{M}",  uval.mptr, 0, 0,   "00000000ffffffff");
        TEST ("%{#M}", uval.mptr, 0, 0, "0x00000000ffffffff");

#else

    fprintf (stderr, "Warning: %s\n", "\"%{M}\" not exercised");

#endif

    }
    else if (sizeof (memptr_t) == 2 * sizeof (long)) {

        static const union {
            unsigned int  ival;
            unsigned char bytes [sizeof (int)];
        } u = { 1U };

        static size_t big_endian = size_t (0 == u.bytes [0]);

        const size_t lo_inx = size_t (big_endian);
        const size_t hi_inx = size_t (1 - big_endian);

#if 4 == _RWSTD_LONG_SIZE

        uval.lval [hi_inx] = 0UL;

        uval.lval [lo_inx] = 0UL;
        TEST ("%{M}",  uval.mptr, 0, 0,   "0000000000000000");
        TEST ("%{#M}", uval.mptr, 0, 0, "0x0000000000000000");

        uval.lval [lo_inx] = 1UL;
        TEST ("%{M}",  uval.mptr, 0, 0,   "0000000000000001");
        TEST ("%{#M}", uval.mptr, 0, 0, "0x0000000000000001");

        uval.lval [lo_inx] = 0xffffffffUL;
        TEST ("%{M}",  uval.mptr, 0, 0,   "00000000ffffffff");
        TEST ("%{#M}", uval.mptr, 0, 0, "0x00000000ffffffff");

        uval.lval [hi_inx] = 0xdeadbeefUL;
        
        uval.lval [lo_inx] = 0UL;
        TEST ("%{M}",  uval.mptr, 0, 0,   "deadbeef00000000");
        TEST ("%{#M}", uval.mptr, 0, 0, "0xdeadbeef00000000");

        uval.lval [lo_inx] = 0x1aUL;
        TEST ("%{M}",  uval.mptr, 0, 0,   "deadbeef0000001a");
        TEST ("%{#M}", uval.mptr, 0, 0, "0xdeadbeef0000001a");

        uval.lval [lo_inx] = 0x0fff1fffUL;
        TEST ("%{M}",  uval.mptr, 0, 0,   "deadbeef0fff1fff");
        TEST ("%{#M}", uval.mptr, 0, 0, "0xdeadbeef0fff1fff");

#elif 8 == _RWSTD_LONG_SIZE

        uval.lval [hi_inx] = 0UL;

        uval.lval [lo_inx] = 0UL;
        TEST ("%{M}",  uval.mptr, 0, 0,   "00000000000000000000000000000000");
        TEST ("%{#M}", uval.mptr, 0, 0, "0x00000000000000000000000000000000");

        uval.lval [lo_inx] = 1UL;
        TEST ("%{M}",  uval.mptr, 0, 0,   "00000000000000000000000000000001");
        TEST ("%{#M}", uval.mptr, 0, 0, "0x00000000000000000000000000000001");

        uval.lval [lo_inx] = 0xffffffffUL;
        TEST ("%{M}",  uval.mptr, 0, 0,   "000000000000000000000000ffffffff");
        TEST ("%{#M}", uval.mptr, 0, 0, "0x000000000000000000000000ffffffff");

        uval.lval [hi_inx] = 0x0123456789abcdefUL;
        
        uval.lval [lo_inx] = 0UL;
        TEST ("%{M}",  uval.mptr, 0, 0,   "0123456789abcdef0000000000000000");
        TEST ("%{#M}", uval.mptr, 0, 0, "0x0123456789abcdef0000000000000000");

        uval.lval [lo_inx] = 0x1aUL;
        TEST ("%{M}",  uval.mptr, 0, 0,   "0123456789abcdef000000000000001a");
        TEST ("%{#M}", uval.mptr, 0, 0, "0x0123456789abcdef000000000000001a");

        uval.lval [lo_inx] = 0x0fff1fffUL;
        TEST ("%{M}",  uval.mptr, 0, 0,   "0123456789abcdef000000000fff1fff");
        TEST ("%{#M}", uval.mptr, 0, 0, "0x0123456789abcdef000000000fff1fff");

#else

    fprintf (stderr, "Warning: %s\n", "\"%{M}\" not exercised");

#endif

    }
}

/***********************************************************************/

static void
test_width_specific_int ()
{
    printf ("%s\n", "extension: \"%{I8d}\": 8-bit decimal integers");

    TEST ("%{I8d}",    0, 0, 0, "0");
    TEST ("%{I8d}",    1, 0, 0, "1");
    TEST ("%{I8d}",    2, 0, 0, "2");
    TEST ("%{I8d}",    3, 0, 0, "3");
    TEST ("%{I8d}",  127, 0, 0, "127");
    TEST ("%{I8d}",  128, 0, 0, "-128");
    TEST ("%{I8d}",  255, 0, 0, "-1");
    TEST ("%{I8d}",  256, 0, 0, "0");
    TEST ("%{I8d}",   -1, 0, 0, "-1");
    TEST ("%{I8d}", -128, 0, 0, "-128");

    printf ("%s\n", "extension: \"%{I8o}\": 8-bit octal integers");

    TEST ("%{I8o}",    0, 0, 0, "0");
    TEST ("%{I8o}",    1, 0, 0, "1");
    TEST ("%{I8o}",    2, 0, 0, "2");
    TEST ("%{I8o}",    3, 0, 0, "3");
    TEST ("%{I8o}",    4, 0, 0, "4");
    TEST ("%{I8o}",    5, 0, 0, "5");
    TEST ("%{I8o}",    6, 0, 0, "6");
    TEST ("%{I8o}",    7, 0, 0, "7");
    TEST ("%{I8o}",    8, 0, 0, "10");
    TEST ("%{I8o}",  127, 0, 0, "177");
    TEST ("%{I8o}",  128, 0, 0, "200");
    TEST ("%{I8o}",  255, 0, 0, "377");
    TEST ("%{I8o}",  256, 0, 0, "0");

    printf ("%s\n", "extension: \"%{I8x}\": 8-bit hexadecimal integers");

    TEST ("%{I8x}",    0, 0, 0, "0");
    TEST ("%{I8x}",    1, 0, 0, "1");
    TEST ("%{I8x}",    2, 0, 0, "2");
    TEST ("%{I8x}",    9, 0, 0, "9");
    TEST ("%{I8x}",   10, 0, 0, "a");
    TEST ("%{I8x}",   15, 0, 0, "f");
    TEST ("%{I8x}",   16, 0, 0, "10");
    TEST ("%{I8x}",  127, 0, 0, "7f");
    TEST ("%{I8x}",  128, 0, 0, "80");
    TEST ("%{I8x}",  255, 0, 0, "ff");
    TEST ("%{I8x}",  256, 0, 0, "0");

    printf ("%s\n", "extension: \"%{I16d}\": 16-bit decimal integers");

    TEST ("%{I16d}",      0, 0, 0, "0");
    TEST ("%{I16d}",      1, 0, 0, "1");
    TEST ("%{I16d}",      2, 0, 0, "2");
    TEST ("%{I16d}",      3, 0, 0, "3");
    TEST ("%{I16d}",    127, 0, 0, "127");
    TEST ("%{I16d}",    128, 0, 0, "128");
    TEST ("%{I16d}",  32767, 0, 0, "32767");
    TEST ("%{I16d}",  32768, 0, 0, "-32768");
    TEST ("%{I16d}",  65535, 0, 0, "-1");
    TEST ("%{I16d}",  65536, 0, 0, "0");
    TEST ("%{I16d}",     -1, 0, 0, "-1");
    TEST ("%{I16d}", -32768, 0, 0, "-32768");

    printf ("%s\n", "extension: \"%{I32d}\": 32-bit decimal integers");

    TEST ("%{I32d}",           0,   0, 0, "0");
    TEST ("%{I32d}",           1,   0, 0, "1");
    TEST ("%{I32d}",           2,   0, 0, "2");
    TEST ("%{I32d}",           3,   0, 0, "3");
    TEST ("%{I32d}",       32767,   0, 0, "32767");
    TEST ("%{I32d}",       32768,   0, 0, "32768");
    TEST ("%{I32d}",  2147483647,   0, 0, "2147483647");
    TEST ("%{I32d}",  2147483648UL, 0, 0, "-2147483648");

    printf ("%s\n", "extension: \"%{I64d}\": 64-bit decimal integers");

#ifndef _RWSTD_NO_LONG_LONG

    TEST ("%{I64d}",           0LL, 0, 0, "0");
    TEST ("%{I64d}",           1LL, 0, 0, "1");
    TEST ("%{I64d}",           2LL, 0, 0, "2");
    TEST ("%{I64d}",           3LL, 0, 0, "3");
    TEST ("%{I64d}",       32767LL, 0, 0, "32767");
    TEST ("%{I64d}",       32768LL, 0, 0, "32768");
    TEST ("%{I64d}",  2147483647LL, 0, 0, "2147483647");
    TEST ("%{I64d}",  2147483648LL, 0, 0, "2147483648");

#else   // if defined (_RWSTD_NO_LONG_LONG)

    fprintf (stderr, "Warning: %s\n", "\"%{I64d}\" not exercised "
             "(no long long support)");

#endif   // _RWSTD_NO_LONG_LONG
}

/***********************************************************************/

static void
test_envvar ()
{
    printf ("%s\n", "extension: \"%{$string}\": environment variable");

    rw_putenv ("FOO=bar");
    TEST ("[%{$FOO}]",     0,     0, 0, "[bar]");
    TEST ("[%{$*}]",       "FOO", 0, 0, "[bar]");
    TEST ("[%{$*}][%1$s]", "FOO", 0, 0, "[bar][bar]");

    // +--------------------+-------------+-------------+-------------+
    // |                    |  parameter  |  parameter  |  parameter  |
    // |                    +-------------+-------------+-------------+
    // |                    |Set, Not Null|  Set, Null  |   Unset     |
    // +--------------------+-------------+-------------+-------------+
    // | ${parameter:-word} |  parameter  |    word     |    word     |
    // | ${parameter-word}  |  parameter  |    null     |    word     |
    // | ${parameter:=word} |  parameter  | assign word | assign word |
    // | ${parameter=word}  |  parameter  |    null     | assign word |
    // | ${parameter:?word} |  parameter  |    error    |    error    |
    // | ${parameter?word}  |  parameter  |    null     |    error    |
    // | ${parameter:+word} |     word    |    null     |    null     |
    // | ${parameter+word}  |     word    |    word     |    null     |
    // +--------------------+-------------+-------------+-------------+

    rw_putenv ("NOT_NULL=FOO");
    rw_putenv ("NULL=");   // define to null (empty string)
    rw_putenv ("UNSET=");   // undefine if defined

    // ":-" use parameter if not null, otherwise word
    TEST ("[%{$NOT_NULL:-word}]", 0, 0, 0, "[FOO]");
    TEST ("[%{$NULL:-word}]",     0, 0, 0, "[word]");
    TEST ("[%{$UNSET:-word}]",    0, 0, 0, "[word]");

    // "-" use parameter if not null, word when unset, otherwise null
    TEST ("[%{$NOT_NULL-word}]",  0, 0, 0, "[FOO]");
    TEST ("[%{$NULL-word}]",      0, 0, 0, "[]");
    TEST ("[%{$UNSET-word}]",     0, 0, 0, "[word]");

    // ":=" use parameter if not null, otherwise assign word
    TEST ("[%{$NOT_NULL:=word}]", 0, 0, 0, "[FOO]");
    TEST ("[%{$NULL:=word}]",     0, 0, 0, "[word]");
    TEST ("[%{$NULL}]",           0, 0, 0, "[word]");
    TEST ("[%{$UNSET:=word}]",    0, 0, 0, "[word]");
    TEST ("[%{$UNSET}]",          0, 0, 0, "[word]");

    // restore variables assigned above
    rw_putenv ("NULL=");
    rw_putenv ("UNSET=");

    // "=" use parameter if not null, assign word when unset, otherwise null
    TEST ("[%{$NOT_NULL=word}]",  0, 0, 0, "[FOO]");
    TEST ("[%{$NULL=word}]",      0, 0, 0, "[]");
    TEST ("[%{$UNSET=word}]",     0, 0, 0, "[word]");
    TEST ("[%{$UNSET}]",          0, 0, 0, "[word]");

    // restore variables assigned above
    rw_putenv ("NULL=");
    rw_putenv ("UNSET=");

    // ":?" use parameter if not null, otherwise error
    TEST ("[%{$NOT_NULL:?word}]", 0, 0, 0, "[FOO]");
    TEST ("[%{$NULL:?word}]",     0, 0, 0, "[%{$NULL:?word}]");
    TEST ("[%{$UNSET:?word}]",    0, 0, 0, "[%{$UNSET:?word}]");

    // "?" use parameter if not null, null when unset, otherwise error
    TEST ("[%{$NOT_NULL?word}]",  0, 0, 0, "[FOO]");
    TEST ("[%{$NULL?word}]",      0, 0, 0, "[]");
    TEST ("[%{$UNSET?word}]",     0, 0, 0, "[%{$UNSET?word}]");

    // ":+" use word if parameter is not null, otherwise null
    TEST ("[%{$NOT_NULL:+word}]", 0, 0, 0, "[word]");
    TEST ("[%{$NULL:+word}]",     0, 0, 0, "[]");
    TEST ("[%{$UNSET:+word}]",    0, 0, 0, "[]");

    // "+" use word if parameter is set, otherwise null
    TEST ("[%{$NOT_NULL+word}]",  0, 0, 0, "[word]");
    TEST ("[%{$NULL+word}]",      0, 0, 0, "[word]");
    TEST ("[%{$UNSET+word}]",     0, 0, 0, "[]");

    //////////////////////////////////////////////////////////////////

    rw_putenv ("NOT_NULL=bar");
    rw_putenv ("NULL=");   // define to null (empty string)
    rw_putenv ("UNSET=");   // undefine if defined

    TEST ("[%{$*:-WORD}]", "NOT_NULL", 0, 0, "[bar]");
    TEST ("[%{$*:-WORD}]", "NULL",     0, 0, "[WORD]");
    TEST ("[%{$*:-WORD}]", "UNSET",    0, 0, "[WORD]");

    TEST ("[%{$*-WORD}]",  "NOT_NULL", 0, 0, "[bar]");
    TEST ("[%{$*-WORD}]",  "NULL",     0, 0, "[]");
    TEST ("[%{$*-WORD}]",  "UNSET",    0, 0, "[WORD]");

    TEST ("[%{$*:=WORD}]", "NOT_NULL", 0, 0, "[bar]");
    TEST ("[%{$*:=WORD}]", "NULL",     0, 0, "[WORD]");
    TEST ("[%{$*}]",       "NULL",     0, 0, "[WORD]");
    TEST ("[%{$*:=WORD}]", "UNSET",    0, 0, "[WORD]");
    TEST ("[%{$*}]",       "UNSET",    0, 0, "[WORD]");

    // restore variables assigned above
    rw_putenv ("NULL=");
    rw_putenv ("UNSET=");

    TEST ("[%{$*=WORD}]",  "NOT_NULL", 0, 0, "[bar]");
    TEST ("[%{$*=WORD}]",  "NULL",     0, 0, "[]");
    TEST ("[%{$*=WORD}]",  "UNSET",    0, 0, "[WORD]");
    TEST ("[%{$*}]",       "UNSET",    0, 0, "[WORD]");

    // restore variables assigned above
    rw_putenv ("NULL=");
    rw_putenv ("UNSET=");

    TEST ("[%{$*:?WORD}]", "NOT_NULL", 0, 0, "[bar]");
    TEST ("[%{$*:?WORD}]", "NULL",     0, 0, "[%{$*:?WORD}]");
    TEST ("[%{$*:?WORD}]", "UNSET",    0, 0, "[%{$*:?WORD}]");

    TEST ("[%{$*?WORD}]",  "NOT_NULL", 0, 0, "[bar]");
    TEST ("[%{$*?WORD}]",  "NULL",     0, 0, "[]");
    TEST ("[%{$*?WORD}]",  "UNSET",    0, 0, "[%{$*?WORD}]");

    TEST ("[%{$*:+WORD}]", "NOT_NULL", 0, 0, "[WORD]");
    TEST ("[%{$*:+WORD}]", "NULL",     0, 0, "[]");
    TEST ("[%{$*:+WORD}]", "UNSET",    0, 0, "[]");

    TEST ("[%{$*+WORD}]",  "NOT_NULL", 0, 0, "[WORD]");
    TEST ("[%{$*+WORD}]",  "NULL",     0, 0, "[WORD]");
    TEST ("[%{$*+WORD}]",  "UNSET",    0, 0, "[]");

    //////////////////////////////////////////////////////////////////

    TEST ("[%{$*:-*}]", "NOT_NULL", "WORD", 0, "[bar]");
    TEST ("[%{$*:-*}]", "NULL",     "WORD", 0, "[WORD]");
    TEST ("[%{$*:-*}]", "UNSET",    "WORD", 0, "[WORD]");

    TEST ("[%{$*-*}]",  "NOT_NULL", "WORD", 0, "[bar]");
    TEST ("[%{$*-*}]",  "NULL",     "WORD", 0, "[]");
    TEST ("[%{$*-*}]",  "UNSET",    "WORD", 0, "[WORD]");

    TEST ("[%{$*:=*}]", "NOT_NULL", "WORD", 0, "[bar]");
    TEST ("[%{$*:=*}]", "NULL",     "WORD", 0, "[WORD]");
    TEST ("[%{$*}]",    "NULL",     0,      0, "[WORD]");
    TEST ("[%{$*:=*}]", "UNSET",    "WORD", 0, "[WORD]");
    TEST ("[%{$*}]",    "UNSET",    0,      0, "[WORD]");

    // restore variables assigned above
    rw_putenv ("NULL=");
    rw_putenv ("UNSET=");

    TEST ("[%{$*=*}]",  "NOT_NULL", "WORD", 0, "[bar]");
    TEST ("[%{$*=*}]",  "NULL",     "WORD", 0, "[]");
    TEST ("[%{$*=*}]",  "UNSET",    "WORD", 0, "[WORD]");
    TEST ("[%{$*}]",    "UNSET",    0,      0, "[WORD]");

    // restore variables assigned above
    rw_putenv ("NULL=");
    rw_putenv ("UNSET=");

    TEST ("[%{$*:?*}]", "NOT_NULL", "WORD", 0, "[bar]");
    TEST ("[%{$*:?*}]", "NULL",     "WORD", 0, "[%{$*:?*}]");
    TEST ("[%{$*:?*}]", "UNSET",    "WORD", 0, "[%{$*:?*}]");

    TEST ("[%{$*?*}]",  "NOT_NULL", "WORD", 0, "[bar]");
    TEST ("[%{$*?*}]",  "NULL",     "WORD", 0, "[]");
    TEST ("[%{$*?*}]",  "UNSET",    "WORD", 0, "[%{$*?*}]");

    TEST ("[%{$*:+*}]", "NOT_NULL", "WORD", 0, "[WORD]");
    TEST ("[%{$*:+*}]", "NULL",     "WORD", 0, "[]");
    TEST ("[%{$*:+*}]", "UNSET",    "WORD", 0, "[]");

    TEST ("[%{$*+*}]",  "NOT_NULL", "WORD", 0, "[WORD]");
    TEST ("[%{$*+*}]",  "NULL",     "WORD", 0, "[WORD]");
    TEST ("[%{$*+*}]",  "UNSET",    "WORD", 0, "[]");

    //////////////////////////////////////////////////////////////////

    // exercise unconditional assignment
    TEST ("[%{$*!:*}]", "NOT_NULL", "WORD1", 0, "[WORD1]");
    TEST ("[%{$*!:*}]", "NULL",     "WORD2", 0, "[WORD2]");
    TEST ("[%{$*!:*}]", "UNSET",    "WORD3", 0, "[WORD3]");

    TEST ("[%{$*!:*}]", "NOT_NULL", "WORD4", 0, "[WORD4]");
    TEST ("[%{$*!:*}]", "NULL",     "WORD5", 0, "[WORD5]");
    TEST ("[%{$*!:*}]", "UNSET",    "WORD6", 0, "[WORD6]");

    //////////////////////////////////////////////////////////////////

    // exercise assignment of a formatted string
    TEST ("[%{$FOO!:@}, %{$FOO}]", "%s", "bar", 0, "[bar, bar]");
}

/***********************************************************************/

static void
test_errno ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%m\": strerror(errno)");

    for (int i = 0; i != 256; ++i) {

        char expect [256];

        // be prepared to deal with non-conforming implementations
        // of strerror() (such as IRIX) that return a null pointer
        const char* const str = strerror (i);
        strcpy (expect, str ? str : "(null)");

        errno = i;

        char *result = rw_sprintfa ("%m");

        if (strcmp (result, expect)) {
            fprintf (stderr,
                     "rw_sprintfa(\"%%m\") == \"%s\", got \"%s\" for "
                     "errno=%d", expect, result, i);
        }

        free (result);
    }

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{#m}\": errno");

    int count = 0;

    errno = 0;
    TEST ("%{#m}", 0, 0, 0, "E#0");

    errno = 0;
    TEST ("%{#*m}", 0, 0, 0, "E#0");

#ifdef EDOM

    ++count;

    errno = EDOM;
    TEST ("%{#m}", 0, 0, 0, "EDOM");

    errno = 0;
    TEST ("%{#*m}", EDOM, 0, 0, "EDOM");

#endif   // EDOM

#ifdef ERANGE

    ++count;

    errno = ERANGE;
    TEST ("%{#m}", 0, 0, 0, "ERANGE");

    errno = 0;
    TEST ("%{#*m}", ERANGE, 0, 0, "ERANGE");

#endif   // ERANGE

#ifdef EILSEQ

    ++count;

    errno = EILSEQ;
    TEST ("%{#m}", 0, 0, 0, "EILSEQ");

    errno = 0;
    TEST ("%{#*m}", EILSEQ, 0, 0, "EILSEQ");

#endif   // EILSEQ

    if (0 == count)
        fprintf (stderr, "%s\n", "%{#m}: could not test");

    errno = 0;
}

/***********************************************************************/

static void
test_signal ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%K\": signal name");

    TEST ("%{K}", SIGABRT, 0, 0, "SIGABRT");
    TEST ("%{K}", SIGFPE,  0, 0, "SIGFPE");
    TEST ("%{K}", SIGILL,  0, 0, "SIGILL");
    TEST ("%{K}", SIGINT,  0, 0, "SIGINT");
    TEST ("%{K}", SIGSEGV, 0, 0, "SIGSEGV");
    TEST ("%{K}", SIGTERM, 0, 0, "SIGTERM");
    TEST ("%{K}", 12345,   0, 0, "SIG#12345");

    TEST ("[%{10K}]",    SIGABRT, 0, 0, "[   SIGABRT]");
    TEST ("[%{+10K}]",   SIGFPE,  0, 0, "[    SIGFPE]");
    TEST ("[%{-10K}]",   SIGILL,  0, 0, "[SIGILL    ]");
    TEST ("[%{.3K}]",    SIGINT,  0, 0, "[SIG]");
    TEST ("[%{10.3K}]",  SIGSEGV, 0, 0, "[       SIG]");
    TEST ("[%{+10.3K}]", SIGTERM, 0, 0, "[       SIG]");
    TEST ("[%{-10.3K}]", 12345,   0, 0, "[SIG       ]");
}

/***********************************************************************/

static void
test_pid ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%P\": rw_pid_t");

    const rw_pid_t pid = 12345;

    TEST ("%{P}", pid, 0, 0, "12345");

    TEST ("[%{10P}]",    pid, 0, 0, "[     12345]");
    TEST ("[%{+10P}]",   pid, 0, 0, "[     12345]");
    TEST ("[%{-10P}]",   pid, 0, 0, "[12345     ]");
    TEST ("[%{.3P}]",    pid, 0, 0, "[12345]");
    TEST ("[%{10.3P}]",  pid, 0, 0, "[     12345]");
    TEST ("[%{+10.3P}]", pid, 0, 0, "[     12345]");
    TEST ("[%{-10.3P}]", pid, 0, 0, "[12345     ]");
}

/***********************************************************************/

static const tm*
make_tm (int sec = 0,            // [0,60]
         int min = 0,            // [0,59]
         int hour = 0,           // [0,23]
         int mday = 1,           // [1,31]
         int mon = 0,            // [0,11]
         int year = 0,           // Gregorian year - 1900
         int wday = 0,           // [0,6]; 0 = Sunday
         int yday = 0,           // [0,365]
         int isdst = 0,          // < 0, 0, > 0
         long gmtoff = 0,        // offset from GMT in seconds,
         const char *zone = 0)   // timezone name
{
    static tm tmp = tm ();

    if (sec < 0) {
        // get the current local time
        time_t t = time (0);
        tm *tmb = localtime (&t);
        return tmb ? tmb : &tmp;
    }
    else if (INT_MAX == sec) {
        // return 0 to exercise extensions
        return 0;
    }

    // use arguments to initialize struct
    tmp.tm_sec   = sec;
    tmp.tm_min   = min;
    tmp.tm_hour  = hour;
    tmp.tm_mday  = mday;
    tmp.tm_mon   = mon;
    tmp.tm_year  = year;
    tmp.tm_wday  = wday;
    tmp.tm_yday  = yday;
    tmp.tm_isdst = isdst;

#if defined (__linux__) && defined (_RWSTD_NO_PURE_C_HEADERS)

    // support for glibc extension:

    // GNU glibc uses gmtoff and zone instead of timezone and
    // tzname when computing/formatting time zone information
    // 
    // http://www.gnu.org/manual/glibc-2.2.3/html_node/libc_425.html#SEC434

#  ifndef __USE_BSD

    tmp.__tm_gmtoff = gmtoff;
    tmp.__tm_zone   = zone;

#  else   // if defined (__USE_BSD)

    tmp.tm_gmtoff = gmtoff;
    tmp.tm_zone   = zone;

#  endif   // __USE_BSD

#else

    _RWSTD_UNUSED (gmtoff);
    _RWSTD_UNUSED (zone);

#endif   // __linux__ && _RWSTD_NO_PURE_C_HEADERS

    return &tmp;
}


static void
test_tm ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{t}\": struct tm");

#define TM   make_tm

    TEST ("%{t}",        0, 0, 0, "(null)");

#if TEST_RW_EXTENSIONS
    const void* addr = bad_address (0);
    TEST ("%{t}", addr, 0, 0, format_bad_address (addr, false));

    addr = bad_address (sizeof (int));
    TEST ("%{t}", addr, 0, 0, format_bad_address (addr, true));
#endif

    // exercise human readable format
    TEST ("%{t}", TM (),                      0, 0, "Sun Jan  1 00:00:00 1900");
    TEST ("%{t}", TM (1),                     0, 0, "Sun Jan  1 00:00:01 1900");
    TEST ("%{t}", TM (2, 3),                  0, 0, "Sun Jan  1 00:03:02 1900");
    TEST ("%{t}", TM (3, 4, 5),               0, 0, "Sun Jan  1 05:04:03 1900");
    TEST ("%{t}", TM (4, 5, 6, 7),            0, 0, "Sun Jan  7 06:05:04 1900");
    TEST ("%{t}", TM (5, 6, 7, 8,  9),        0, 0, "Sun Oct  8 07:06:05 1900");
    TEST ("%{t}", TM (6, 7, 8, 9, 10, 11),    0, 0, "Sun Nov  9 08:07:06 1911");

    TEST ("%{t}", TM (7, 7, 8, 1,  1, 12, 1), 0, 0, "Mon Feb  1 08:07:07 1912");
    TEST ("%{t}", TM (8, 7, 8, 2,  2, 23, 2), 0, 0, "Tue Mar  2 08:07:08 1923");
    TEST ("%{t}", TM (9, 7, 8, 3,  3, 34, 3), 0, 0, "Wed Apr  3 08:07:09 1934");
    TEST ("%{t}", TM (0, 7, 8, 4,  4, 45, 4), 0, 0, "Thu May  4 08:07:00 1945");
    TEST ("%{t}", TM (1, 7, 8, 5,  5, 56, 5), 0, 0, "Fri Jun  5 08:07:01 1956");
    TEST ("%{t}", TM (2, 7, 8, 6,  6, -1, 6), 0, 0, "Sat Jul  6 08:07:02 1899");
    TEST ("%{t}", TM (3, 7, 8, 7,  7, -1899), 0, 0, "Sun Aug  7 08:07:03 1");
    TEST ("%{t}", TM (3, 7, 8, 7,  7, -1900), 0, 0, "Sun Aug  7 08:07:03 1 BC");
    TEST ("%{t}", TM (3, 7, 8, 7,  7, -1901), 0, 0, "Sun Aug  7 08:07:03 2 BC");

    TEST ("%{t}", TM (56, 34, 12, 1, 0, 0, 0, 0, 1), 0, 0,
          "Sun Jan  1 12:34:56 DST 1900");

    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{#t}\": verbose struct tm");

    // exercise verbose struct tm format (members with 0 value omitted)
    TEST ("%{#t}", TM (),                 0, 0, "{ tm_mday=1 }");
    TEST ("%{#t}", TM (60),               0, 0, "{ tm_sec=60, tm_mday=1 }");
    TEST ("%{#t}", TM ( 0, 59),           0, 0, "{ tm_min=59, tm_mday=1 }");
    TEST ("%{#t}", TM ( 0,  0, 23),       0, 0, "{ tm_hour=23, tm_mday=1 }");
    TEST ("%{#t}", TM ( 0,  0,  0, 2),    0, 0, "{ tm_mday=2 }");
    TEST ("%{#t}", TM ( 0,  0,  0, 3, 1), 0, 0, "{ tm_mday=3, tm_mon=1 Feb }");

    // exercise invalid values of tm members (verbose format implied)
    TEST ("%{t}", TM (61),            0, 0, "{ tm_sec=61 [0,60], tm_mday=1 }");
    TEST ("%{t}", TM ( 0, 60),        0, 0, "{ tm_min=60 [0,59], tm_mday=1 }");
    TEST ("%{t}", TM ( 0,  0, 24),    0, 0, "{ tm_hour=24 [0,23], tm_mday=1 }");

    TEST ("%{t}", TM ( 0, 0, 0,  0), 0, 0, "{ tm_mday=0 [1,31] }");
    TEST ("%{t}", TM ( 0, 0, 0, 32), 0, 0, "{ tm_mday=32 [1,31] }");

    TEST ("%{t}", TM ( 0, 0, 0, 1, 12), 0, 0,
          "{ tm_mday=1, tm_mon=12 [0,11] }");
    TEST ("%{t}", TM (0, 0, 0, 1, 0, 0, -1), 0, 0,
          "{ tm_mday=1, tm_wday=-1 [0,6] }");
    TEST ("%{t}", TM (0, 0, 0, 1, 0, 0, 0, 366), 0, 0,
          "{ tm_mday=1, tm_yday=366 [0,365] }");

#if 4 == _RWSTD_INT_SIZE

    // exercise integer overflow in tm_year arithmetic
    TEST ("%{t}", TM (0, 0, 0, 1, 0, INT_MAX), 0, 0,
          "{ tm_mday=1, tm_year=2147483647 [-2147483648,2147481747] }");

#elif 8 == _RWSTD_INT_SIZE

    TEST ("%{t}", TM (0, 0, 0, 1, 0, INT_MAX), 0, 0,
          "{ tm_mday=1, tm_year=9223372036854775807 "
          "[-9223372036854775808,9223372036854773907] }");

#endif   // _RWSTD_INT_SIZE
}

/***********************************************************************/

static void
test_paramno ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%<paramno>$\": positional parameters");

    TEST ("%c %1$c",         'a', 0,   0, "a a");
    TEST ("%c %1$c %1$c",    'b', 0,   0, "b b b");
    TEST ("%c %1$c %c %2$c", 'c', 'd', 0, "c c d d");
    TEST ("%c %c %1$c %1$c", 'e', 'f', 0, "e f e e");

    TEST ("%s %1$s",    "b",   0, 0, "b b");
    TEST ("%s %1$s",    "cd",  0, 0, "cd cd");

    TEST ("[%s|%1$1s]", "xyz", 0, 0, "[xyz|xyz]");
    TEST ("[%s|%1$2s]", "xyz", 0, 0, "[xyz|xyz]");
    TEST ("[%s|%1$3s]", "xyz", 0, 0, "[xyz|xyz]");
    TEST ("[%s|%1$4s]", "xyz", 0, 0, "[xyz| xyz]");
    TEST ("[%s|%1$5s]", "xyz", 0, 0, "[xyz|  xyz]");
    TEST ("[%s|%1$6s]", "xyz", 0, 0, "[xyz|   xyz]");

    TEST ("[%s|%1$-1s]", "xyz", 0, 0, "[xyz|xyz]");
    TEST ("[%s|%1$-2s]", "xyz", 0, 0, "[xyz|xyz]");
    TEST ("[%s|%1$-3s]", "xyz", 0, 0, "[xyz|xyz]");
    TEST ("[%s|%1$-4s]", "xyz", 0, 0, "[xyz|xyz ]");
    TEST ("[%s|%1$-5s]", "xyz", 0, 0, "[xyz|xyz  ]");
    TEST ("[%s|%1$-6s]", "xyz", 0, 0, "[xyz|xyz   ]");

    TEST ("[%s|%1$+1s]", "xyz", 0, 0, "[xyz|xyz]");
    TEST ("[%s|%1$+2s]", "xyz", 0, 0, "[xyz|xyz]");
    TEST ("[%s|%1$+3s]", "xyz", 0, 0, "[xyz|xyz]");
    TEST ("[%s|%1$+4s]", "xyz", 0, 0, "[xyz| xyz]");
    TEST ("[%s|%1$+5s]", "xyz", 0, 0, "[xyz|  xyz]");
    TEST ("[%s|%1$+6s]", "xyz", 0, 0, "[xyz|   xyz]");

    TEST ("[%s|%1$1.1s]", "xyz", 0, 0, "[xyz|x]");
    TEST ("[%s|%1$2.2s]", "xyz", 0, 0, "[xyz|xy]");
    TEST ("[%s|%1$3.3s]", "xyz", 0, 0, "[xyz|xyz]");
    TEST ("[%s|%1$4.1s]", "xyz", 0, 0, "[xyz|   x]");
    TEST ("[%s|%1$5.2s]", "xyz", 0, 0, "[xyz|   xy]");
    TEST ("[%s|%1$6.3s]", "xyz", 0, 0, "[xyz|   xyz]");

    TEST ("[%s|%1$6.3s]", "xyz", 0, 0, "[xyz|   xyz]");

    TEST ("[foo=%s, bar=%s|foo=%1$s, bar=%2$s]",
          "abc", "def", 0,
          "[foo=abc, bar=def|foo=abc, bar=def]");

    TEST ("%i %1$i", 1, 0, 0, "1 1");
    TEST ("%i %1$i", 1, 0, 0, "1 1");
}

/***********************************************************************/

static void
test_conditional ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{?}\", \"%{:}\", \"%{;}\": conditional");

    TEST ("%{?}%{;}",           0, 0, 0, "");
    TEST ("%{?}%{;}",           1, 0, 0, "");

    TEST ("%{?}a%{;}",          0, 0, 0, "");
    TEST ("%{?}b%{;}",          1, 0, 0, "b");

    TEST ("a%{?}b%{;}",         0, 0, 0, "a");
    TEST ("a%{?}b%{;}",         1, 0, 0, "ab");

    TEST ("a%{?}%{;}b",         0, 0, 0, "ab");
    TEST ("a%{?}%{;}b",         1, 0, 0, "ab");

    TEST ("a%{?}b%{;}c",        0, 0, 0, "ac");
    TEST ("a%{?}b%{;}c",        1, 0, 0, "abc");

    TEST ("%{?}%{:}%{;}",       0, 0, 0, "");
    TEST ("%{?}%{:}%{;}",       1, 0, 0, "");

    TEST ("%{?}a%{:}b%{;}",     0, 0, 0, "b");
    TEST ("%{?}a%{:}b%{;}",     1, 0, 0, "a");

    TEST ("a%{?}b%{:}c%{;}",    0, 0, 0, "ac");
    TEST ("a%{?}b%{:}c%{;}",    1, 0, 0, "ab");

    TEST ("a%{?}b%{:}c%{;}d",   0, 0, 0, "acd");
    TEST ("a%{?}b%{:}c%{;}d",   1, 0, 0, "abd");

    TEST ("a%{?}bc%{:}d%{;}",   0, 0, 0, "ad");
    TEST ("a%{?}bc%{:}d%{;}",   1, 0, 0, "abc");

    TEST ("a%{?}bc%{:}d%{;}e",  0, 0, 0, "ade");
    TEST ("a%{?}bc%{:}d%{;}e",  1, 0, 0, "abce");

    TEST ("a%{?}bc%{:}de%{;}",  0, 0, 0, "ade");
    TEST ("a%{?}bc%{:}de%{;}",  1, 0, 0, "abc");

    TEST ("a%{?}bc%{:}de%{;}f", 0, 0, 0, "adef");
    TEST ("a%{?}bc%{:}de%{;}f", 1, 0, 0, "abcf");

    // exercise nested confitionals
    TEST ("x%{?}0%{?}1%{;}2%{;}y", 0, 0, 0, "xy");
    TEST ("x%{?}0%{?}1%{;}2%{;}y", 0, 1, 0, "xy");
    TEST ("x%{?}0%{?}1%{;}2%{;}y", 1, 0, 0, "x02y");
    TEST ("x%{?}0%{?}1%{;}2%{;}y", 1, 1, 0, "x012y");

    TEST ("x%{?}0%{?}1%{:}2%{;}3%{;}y", 0, 0, 0, "xy");
    TEST ("x%{?}0%{?}1%{:}2%{;}3%{;}y", 0, 1, 0, "xy");
    TEST ("x%{?}0%{?}1%{:}2%{;}3%{;}y", 1, 0, 0, "x023y");
    TEST ("x%{?}0%{?}1%{:}2%{;}3%{;}y", 1, 1, 0, "x013y");

    TEST ("x%{?}0%{?}1%{:}2%{;}3%{:}4%{;}y", 0, 0, 0, "x4y");
    TEST ("x%{?}0%{?}1%{:}2%{;}3%{:}4%{;}y", 0, 1, 0, "x4y");
    TEST ("x%{?}0%{?}1%{:}2%{;}3%{:}4%{;}y", 1, 0, 0, "x023y");
    TEST ("x%{?}0%{?}1%{:}2%{;}3%{:}4%{;}y", 1, 1, 0, "x013y");

    const char cond3[] = {
        "0"
        "%{?}"   // if ($1) {
          "1"
          "%{?}"   // if ($2) {
            "2"
            "%{?}"   // if  ($3) {
              "3"
            "%{:}"   // } else /* if (!$3) */ {
              "4"
            "%{;}"   // }   // $3
            "5"
          "%{:}"   // } else /* if (!$2) */{
            "6"
          "%{;}"   // }   // $2
          "7"
        "%{:}"   // } else /* if (!$1) */ {
          "8"
        "%{;}"   // }   // $1
        "9"
    };

    TEST (cond3, 0, 0, 0, "089");
    TEST (cond3, 0, 0, 1, "089");
    TEST (cond3, 0, 1, 0, "089");
    TEST (cond3, 0, 1, 1, "089");
    TEST (cond3, 1, 0, 0, "01679");
    TEST (cond3, 1, 0, 1, "01679");
    TEST (cond3, 1, 1, 0, "0124579");
    TEST (cond3, 1, 1, 1, "0123579");

    // exercise conditionals with positional parameters
    TEST ("A%{?}B%{:}C%{;}%1$d",  0, 0, 0, "AC0");
    TEST ("A%{?}B%{:}C%{;}%1$d",  1, 0, 0, "AB1");
    TEST ("A%{?}B%{:}C%{;}%1$d",  2, 0, 0, "AB2");
    TEST ("A%{?}B%{:}C%{;}%1$d", -1, 0, 0, "AB-1");
}

/***********************************************************************/

static int
user_fun_va (const char *fun_name,   // name of calling function
             char      **pbuf,       // pointer to a buffer
             size_t     *pbufsize,   // pointer to buffer size
             const char *fmt,        // format string
             va_list     va)         // argument list
{
    if (0 == pbuf) {
        fprintf (stderr, "pbuf: unexpected null argument #1");
        ++nfailures;
        return -1;
    }

    if (0 == pbufsize) {
        fprintf (stderr, "pbufsize: unexpected null argument #2");
        ++nfailures;
        return -1;
    }

    if (0 == fmt) {
        fprintf (stderr, "fmt: unexpected null argument #3");
        ++nfailures;
        return -1;
    }

    if (   0 == strcmp ("!", fmt)
        || 0 == strcmp ("+!", fmt)
        || 0 == strcmp ("-!", fmt)) {
        // special value indicating to the caller that we're
        // returning control to it and letting it to handle
        // the directive to set/push/pop a user-defined
        // formatting function 
        return _RWSTD_INT_MIN;
    }

    const size_t funlen = strlen (fun_name);

    int arg = -1;

    if (isdigit (*fmt)) {
        // process positional parameter
        char* end = 0;
        arg = strtol (fmt, &end, 10);
        if ('$' != *end)
            arg = -1;
        else if (memcmp (fun_name, end + 1, funlen) || ':' != end [funlen + 1])
            arg = _RWSTD_INT_MIN;
    }
    else if (memcmp (fun_name, fmt, funlen) || ':' != fmt [funlen])
        arg = _RWSTD_INT_MIN;

    if (_RWSTD_INT_MIN == arg) {
        // if the format directive (beyond the positional parameter, if
        // present) doesn't match the name of the user-defined function,
        // return to the caller for further processing
        return arg;
    }

    if (-1 < arg) {
        // extract the address of the positional argument from the list
        // (assume its type is int for simplicity -- there is no easy way
        // to verify compatibility with the actual type of the argument
        // without having access to the argument's original  directive)
        const int* const parg = va_arg (va, int*);

        RW_ASSERT (0 != parg);

        arg = *parg;
    }
    else {
        // extract the address of the caller's variable argument list
        va_list *pva = va_arg (va, va_list*);

        RW_ASSERT (0 != pva);

        // extract an integer value from rw_snprintfa's variable argument
        // list pass through to us by the caller 
        arg = va_arg (*pva, int);

        // extract the address where to store the extracted argument
        int* const pparam = va_arg (va, int*);

        RW_ASSERT (0 != pparam);

        // store the extracted argument
        *pparam = arg;
    }

    // compute the length of the buffer formatted so far
    const size_t buflen_0 = *pbuf ? strlen (*pbuf) : 0;

    // invoke rw_asnprintf() recursively to format our arguments
    // and append the result to the end of the buffer; pass the
    // value returned from rw_asnprintf() (i.e., the number of
    // bytes appended) back to the caller
    const int n = rw_asnprintf (pbuf, pbufsize,
                                "%{+}<%s:'%s',%d>", fun_name, fmt, arg);

    // compute the new length of the buffer
    const size_t buflen_1 = *pbuf ? strlen (*pbuf) : 0;

    // assert that the function really appended as many characters
    // as it said it did (assumes no NULs embedded in the output)
    // and that it didn't write past the end of the buffer
    RW_ASSERT (n < 0 || buflen_1 == buflen_0 + n);
    RW_ASSERT (buflen_1 < *pbufsize);

    return n;
}


static int
user_fun_f (char **pbuf, size_t *pbufsize, const char *fmt, ...)
{
    va_list va;
    va_start (va, fmt);

    const int result = user_fun_va ("f", pbuf, pbufsize, fmt, va);

    va_end (va);

    return result;
}

static int
user_fun_g (char **pbuf, size_t *pbufsize, const char *fmt, ...)
{
    va_list va;
    va_start (va, fmt);

    const int result = user_fun_va ("g", pbuf, pbufsize, fmt, va);

    va_end (va);

    return result;
}


static void
test_user_defined_formatting ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{!}\" user-defined formatting function");

    // set the user-defined function that overrides the default
    TEST ("%{!}",   &user_fun_f, 0, 0, "");
    TEST ("%{d}",      0, 0, 0, "0");
    TEST ("%{f:d}",    0, 0, 0, "<f:'f:d',0>");
    TEST ("%{x}",     10, 0, 0, "a");
    TEST ("%{f:foo}",  1, 0, 0, "<f:'f:foo',1>");
    TEST ("%{f:bar}", -1, 0, 0, "<f:'f:bar',-1>");

    // exercise positional parameters
    TEST ("%d%{1$f:foo}",           11, 0, 0, "11<f:'1$f:foo',11>");
    TEST ("%{f:d}%{1$f:bar}",       12, 0, 0, "<f:'f:d',12><f:'1$f:bar',12>");
    TEST ("%i%{1$f:foo}%{1$f:bar}", 13, 0, 0,
          "13<f:'1$f:foo',13><f:'1$f:bar',13>");

    // push another user-defined function on the stack
    TEST ("%{+!}",          &user_fun_g, 0, 0, "");
    TEST ("%{f:x}",          0, 0, 0, "<f:'f:x',0>");
    TEST ("%{g:y}",          1, 0, 0, "<g:'g:y',1>");
    TEST ("%{i}",            2, 0, 0, "2");
    TEST ("%{f:a}%b%{g:c}",  3, 4, 5, "<f:'f:a',3>true<g:'g:c',5>");

    // disable all user-defined processing
    TEST ("%{!}",  0, 0, 0, "");
    TEST ("%{d}",  3, 0, 0, "3");
    TEST ("%{i}", -5, 0, 0, "-5");

    // pop the top of the stack and re-enable user-defined processing
    TEST ("%{-!}",  0, 0, 0, "");
    TEST ("%{f:x}", 1, 0, 0, "<f:'f:x',1>");
    TEST ("%{f:y}", 2, 0, 0, "<f:'f:y',2>");

    // reset the user-defined formatting function
    TEST ("%{!}",      0, 0, 0, "");
    TEST ("%{d}",    123, 0, 0, "123");
}

/***********************************************************************/

static void
test_bufsize ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{N}\" buffer size");

    TEST ("%{0}foo",     0, 0, 0, 0);
    TEST ("%{1}foo",     0, 0, 0, 0);
    TEST ("%{2}foo",     0, 0, 0, 0);
    TEST ("%{3}foo",     0, 0, 0, 0);
    TEST ("%{4}foo",     0, 0, 0, "foo");
    TEST ("%{10}foobar", 0, 0, 0, "foobar");

    TEST ("%{*}bar",     0, 0, 0, 0);
    TEST ("%{*}bar",     1, 0, 0, 0);
    TEST ("%{*}bar",     2, 0, 0, 0);
    TEST ("%{*}bar",     3, 0, 0, 0);
    TEST ("%{*}bar",     4, 0, 0, "bar");
}

/***********************************************************************/

static void
test_nested_format ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "extension: \"%{@}\" nested format directive");

    TEST ("%{@}",        "",     0,    0, "");
    TEST ("%{@}",        "a",    0,    0, "a");
    TEST ("%{@}",        "ab",   0,    0, "ab");
    TEST ("%{@}",        "abc",  0,    0, "abc");
    TEST ("%{@}",        "%d",   0,    0, "0");
    TEST ("%{@}",        "%d",   1,    0, "1");
    TEST ("%{@}",        "%d",  12,    0, "12");
    TEST ("%{@}",        "%s",  "x",   0, "x");
    TEST ("%{@}",        "%s",  "xy",  0, "xy");
    TEST ("%{@}",        "%s",  "xyz", 0, "xyz");
    TEST ("x%{@}",       "%s",  "yz",  0, "xyz");
    TEST ("%{@}z",       "%s",  "xy",  0, "xyz");
    TEST ("x%{@}z",      "%s",  "y",   0, "xyz");

    TEST ("ABC%{@}F",    "D%{@}E", "",    0, "ABCDEF");
    TEST ("ABC%{@}GH",   "D%{@}F", "e",   0, "ABCDeFGH");
    TEST ("ABC%{@}HIJ",  "D%{@}G", "ef",  0, "ABCDefGHIJ");
    TEST ("ABC%{@}IJKL", "D%{@}H", "efg", 0, "ABCDefgHIJKL");

    TEST ("AB%{@}MN", "CD%{@}KL", "EF%{@}IJ", "gh", "ABCDEFghIJKLMN");

    TEST ("A%{@}C%{@}E%{@}G", "B",  "D",  "F", "ABCDEFG");
    TEST ("A%{@}C%{@}E",      "%s", "B",  "D", "ABCDE");
    TEST ("A%{@}C%{@}E",      "B",  "%s", "D", "ABCDE");

    TEST ("ABC%sGHI%{@}XYZ", "DEF", "%s%1$s", "JKL", "ABCDEFGHIJKLJKLXYZ");

    // exercise arrays

    TEST ("%{.0@}", "%i", 0, 0, "");
    TEST ("%{.1@}", "%i", 1, 0, "1");
    TEST ("%{.2@}", "%i", 2, 3, "23");

    TEST ("%{ .0@}",    "%i",  3, 0, "");
    TEST ("%{ .1@}",    "%i",  4, 5, "4");
    TEST ("%{ .2@}",    "%i",  5, 6, "5 6");
    TEST ("[ %{.2@}]",  "%i ", 6, 7, "[ 6 7 ]");

    const int ia[] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

    //
    //       +---- (width: ???)
    //       | +-- precision: number of elements in array
    //       | |
    //       v v
    TEST ("%{.0A@}", "%i", ia, 0, "");
    TEST ("%{.1A@}", "%i", ia, 0, "9");
    TEST ("%{.2A@}", "%i", ia, 0, "98");
    TEST ("%{.3A@}", "%i", ia, 0, "987");
    TEST ("%{.4A@}", "%i", ia, 0, "9876");
    TEST ("%{.5A@}", "%i", ia, 0, "98765");
    TEST ("%{.6A@}", "%i", ia, 0, "987654");
    TEST ("%{.7A@}", "%i", ia, 0, "9876543");
    TEST ("%{.8A@}", "%i", ia, 0, "98765432");
    TEST ("%{.9A@}", "%i", ia, 0, "987654321");
    TEST ("%{.*A@}", 10, "%i", ia, "9876543210");
    TEST ("[%{ .*A@}]", 9, "%i", ia, "[9 8 7 6 5 4 3 2 1]");
    TEST ("[%{ .*A@}]", 8, "%i", ia, "[9 8 7 6 5 4 3 2]");
    TEST ("[%{ .*A@}]", 7, "%i", ia, "[9 8 7 6 5 4 3]");
    TEST ("[%{ .*A@}]", 6, "%i", ia, "[9 8 7 6 5 4]");
    TEST ("[%{ .*A@}]", 5, "%i", ia, "[9 8 7 6 5]");
    TEST ("[%{ .*A@}]", 4, "%i", ia, "[9 8 7 6]");
    TEST ("[%{ .*A@}]", 3, "%i", ia, "[9 8 7]");
    TEST ("[%{ .*A@}]", 2, "%i", ia, "[9 8]");
    TEST ("[%{ .*A@}]", 1, "%i", ia, "[9]");
    TEST ("[%{ .*A@}]", 0, "%i", ia, "[]");

    static const struct {
        size_t i; const char *s;
    } sa[] = {
        { 1, "foo" }, { 2, "bar" }, { 3, "foobar" }
    };

    // members must be packed with no padding
    RW_ASSERT (sizeof *sa == sizeof sa->i + sizeof sa->s);

    TEST ("{%{ .*A@}}",   // array of nested directives
          3,              // three elements
          "(%zu, %#s)",   // descrcibes structure
          sa,             // array argument
          "{(1, \"foo\") (2, \"bar\") (3, \"foobar\")}");
}

/***********************************************************************/

static void
test_malformed_directives ()
{
    //////////////////////////////////////////////////////////////////
    printf ("%s\n", "malformed directives");

    TEST ((char*)0, 0, 0, 0, "(null)");
    TEST ("%{",     0, 0, 0, "%{");
    TEST ("%{%",    0, 0, 0, "%{%");
    TEST ("%{%{",   0, 0, 0, "%{%{");
    TEST ("%{}",    0, 0, 0, "%{}");
}

/***********************************************************************/

static void
stress_bufsize ()
{
    // exercise the ability to format into a fixed size buffer
    // and correctly report buffer overlow errors via ENOMEM

    //////////////////////////////////////////////////////////////////
    static const char str[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    char buf [sizeof str] = "";

    for (unsigned size = 0; size != sizeof str - 1; ++size) {
        for (unsigned len = 0; len != sizeof str - 1; ++len) {

            errno = 0;

            const char* const s = str + sizeof str - 1 - len;

            // format a substring of s len characters long
            // into a buffer size characters wide
            const char* const res =
                rw_snprintfa (buf, size, "%{*}%s", size, s);

            const int error = errno;

            if (size <= len) {
                // verify that the function returns 0 and sets errno
                // to ENOMEM when the provided buffer is not big enough
                assert (0 == res);
                assert (ENOMEM == error);
            }
            else {
                // verify that the function returns the address
                // of the provided buffer on success and that
                // the contents of the buffer match the argument
                assert (res == buf);
                assert (0 == error);
                assert (0 == strcmp (s, buf));
            }
        }
    }
}

/***********************************************************************/

int main ()
{
    test_percent ();

    test_character ();
    test_string ();
    test_chararray ();
    test_stringarray ();

    test_integer ();
    test_intarray ();

    test_floating ();
    test_pointer ();

    test_envvar ();
    test_funptr ();
    test_memptr ();
    test_errno ();
    test_signal ();
    test_pid ();

    test_basic_string ();

    test_ios_bitmasks ();

    test_locale_category ();

    test_ctype_mask ();

    test_bitset ();

    test_tm ();

    test_paramno ();

    test_width_specific_int ();

    test_conditional ();

    test_user_defined_formatting ();

    test_bufsize ();

    test_nested_format ();

    test_malformed_directives ();

    //////////////////////////////////////////////////////////////////
    if (nfailures) {
        fprintf (stderr, "\nFailed %d out of %d assertions.\n",
                 nfailures, ntests);
        return 1;
    }

    //////////////////////////////////////////////////////////////////
    // close stderr to prevent the tested function from spitting out
    // thousands of error messages in the stress test below
    fclose (stderr);

    stress_bufsize ();

    printf ("\nPassed all %d assertions.\n", ntests);

    return 0;
}
