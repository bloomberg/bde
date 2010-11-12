/***************************************************************************
 *
 * 21.cctype.cpp - test exercising clause lib.string.c.strings,
 *                 header <cctype>
 *
 * $Id: 21.cctype.cpp 550991 2007-06-26 23:58:07Z sebor $
 *
 ***************************************************************************
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <cctype>
#include <driver.h>

/**************************************************************************/

// detect masking macros
const char* const cctype_macros [] = {

#ifdef isalnum
    "isalnum",
#else
    "",
#endif

#ifdef isalpha
    "isalpha",
#else
    "",
#endif

#ifdef iscntrl
    "iscntrl",
#else
    "",
#endif

#ifdef isdigit
    "isdigit",
#else
    "",
#endif

#ifdef isgraph
    "isgraph",
#else
    "",
#endif

#ifdef islower
    "islower",
#else
    "",
#endif

#ifdef isprint
    "isprint",
#else
    "",
#endif

#ifdef ispunct
    "ispunct",
#else
    "",
#endif

#ifdef isspace
    "isspace",
#else
    "",
#endif

#ifdef isupper
    "isupper",
#else
    "",
#endif

#ifdef isxdigit
    "isxdigit",
#else
    "",
#endif

#ifdef tolower
    "tolower",
#else
    "",
#endif

#ifdef toupper
    "toupper",
#else
    "",
#endif

    0
};

/**************************************************************************/

// define function template overloads for the <cctype> functions with
// the same name to detect whether the C versions are declared or not
#define TEST_FUNCTION(name)                                             \
    template <class T> int name (T) { function_called = 0; return 0; }  \
    typedef void rw_unused_typedef

int function_called;

_RWSTD_NAMESPACE (std) {

TEST_FUNCTION (isalnum);
TEST_FUNCTION (isalpha);
TEST_FUNCTION (iscntrl);
TEST_FUNCTION (isdigit);
TEST_FUNCTION (isprint);
TEST_FUNCTION (isupper);
TEST_FUNCTION (tolower);
TEST_FUNCTION (isgraph);
TEST_FUNCTION (ispunct);
TEST_FUNCTION (isxdigit);
TEST_FUNCTION (toupper);

}   // namespace std

/**************************************************************************/

enum {
    bit_alnum  = 1,
    bit_alpha  = 1 << 1,
    bit_cntrl  = 1 << 2,
    bit_digit  = 1 << 3,
    bit_graph  = 1 << 4,
    bit_lower  = 1 << 5,
    bit_print  = 1 << 6,
    bit_punct  = 1 << 7,
    bit_space  = 1 << 8,
    bit_upper  = 1 << 9,
    bit_xdigit = 1 << 10
};


static char*
get_bitmask (int mask, char *str)
{
    char *str_save = str;

#define APPEND(bit)                                     \
    if (mask & bit_ ## bit) {                           \
        for (const char *s = #bit; *s; ++s, ++str)      \
            *str = *s;                                  \
        *str++ = '|';                                   \
    } else (void)0

    APPEND (alnum);
    APPEND (alpha);
    APPEND (cntrl);
    APPEND (digit);
    APPEND (graph);
    APPEND (lower);
    APPEND (print);
    APPEND (punct);
    APPEND (space);
    APPEND (upper);
    APPEND (xdigit);

    if (str == str_save)
        *str = '\0';
    else
        str [-1] = '\0';

    return str_save;
}


static const int
char_mask [256] = {

#undef  LETTER
#define LETTER(bits) bit_alpha | bit_alnum | bit_print | bit_graph | bits

#if 'A' == 0x41

    /* 0x00 */ bit_cntrl,
    /* 0x01 */ bit_cntrl,
    /* 0x02 */ bit_cntrl,
    /* 0x03 */ bit_cntrl,
    /* 0x04 */ bit_cntrl,
    /* 0x05 */ bit_cntrl,
    /* 0x06 */ bit_cntrl,
    /* 0x07 */ bit_cntrl,
    /* 0x08 */ bit_cntrl,
    /* 0x09 */ bit_space | bit_cntrl,
    /* 0x0a */ bit_space | bit_cntrl,
    /* 0x0b */ bit_space | bit_cntrl,
    /* 0x0c */ bit_space | bit_cntrl,
    /* 0x0d */ bit_space | bit_cntrl,
    /* 0x0e */ bit_cntrl,
    /* 0x0f */ bit_cntrl,
    /* 0x10 */ bit_cntrl,
    /* 0x11 */ bit_cntrl,
    /* 0x12 */ bit_cntrl,
    /* 0x13 */ bit_cntrl,
    /* 0x14 */ bit_cntrl,
    /* 0x15 */ bit_cntrl,
    /* 0x16 */ bit_cntrl,
    /* 0x17 */ bit_cntrl,
    /* 0x18 */ bit_cntrl,
    /* 0x19 */ bit_cntrl,
    /* 0x1a */ bit_cntrl,
    /* 0x1b */ bit_cntrl,
    /* 0x1c */ bit_cntrl,
    /* 0x1d */ bit_cntrl,
    /* 0x1e */ bit_cntrl,
    /* 0x1f */ bit_cntrl,
    /* ' '  */ bit_space | bit_print,
    /* '!'  */ bit_print | bit_punct | bit_graph,
    /* '"'  */ bit_print | bit_punct | bit_graph,
    /* '#'  */ bit_print | bit_punct | bit_graph,
    /* '$'  */ bit_print | bit_punct | bit_graph,
    /* '%'  */ bit_print | bit_punct | bit_graph,
    /* '&'  */ bit_print | bit_punct | bit_graph,
    /* '\'' */ bit_print | bit_punct | bit_graph,
    /* '('  */ bit_print | bit_punct | bit_graph,
    /* ')'  */ bit_print | bit_punct | bit_graph,
    /* '*'  */ bit_print | bit_punct | bit_graph,
    /* '+'  */ bit_print | bit_punct | bit_graph,
    /* ','  */ bit_print | bit_punct | bit_graph,
    /* '-'  */ bit_print | bit_punct | bit_graph,
    /* '.'  */ bit_print | bit_punct | bit_graph,
    /* '/'  */ bit_print | bit_punct | bit_graph,
    /* '0'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '1'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '2'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '3'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '4'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '5'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '6'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '7'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '8'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '9'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* ':'  */ bit_print | bit_punct | bit_graph,
    /* ';'  */ bit_print | bit_punct | bit_graph,
    /* '<'  */ bit_print | bit_punct | bit_graph,
    /* '='  */ bit_print | bit_punct | bit_graph,
    /* '>'  */ bit_print | bit_punct | bit_graph,
    /* '?'  */ bit_print | bit_punct | bit_graph,
    /* '@'  */ bit_print | bit_punct | bit_graph,

    /* 'A'  */ LETTER (bit_upper | bit_xdigit),
    /* 'B'  */ LETTER (bit_upper | bit_xdigit),
    /* 'C'  */ LETTER (bit_upper | bit_xdigit),
    /* 'D'  */ LETTER (bit_upper | bit_xdigit),
    /* 'E'  */ LETTER (bit_upper | bit_xdigit),
    /* 'F'  */ LETTER (bit_upper | bit_xdigit),
    /* 'G'  */ LETTER (bit_upper),
    /* 'H'  */ LETTER (bit_upper),
    /* 'I'  */ LETTER (bit_upper),
    /* 'J'  */ LETTER (bit_upper),
    /* 'K'  */ LETTER (bit_upper),
    /* 'L'  */ LETTER (bit_upper),
    /* 'M'  */ LETTER (bit_upper),
    /* 'N'  */ LETTER (bit_upper),
    /* 'O'  */ LETTER (bit_upper),
    /* 'P'  */ LETTER (bit_upper),
    /* 'Q'  */ LETTER (bit_upper),
    /* 'R'  */ LETTER (bit_upper),
    /* 'S'  */ LETTER (bit_upper),
    /* 'T'  */ LETTER (bit_upper),
    /* 'U'  */ LETTER (bit_upper),
    /* 'V'  */ LETTER (bit_upper),
    /* 'W'  */ LETTER (bit_upper),
    /* 'X'  */ LETTER (bit_upper),
    /* 'Y'  */ LETTER (bit_upper),
    /* 'Z'  */ LETTER (bit_upper),
    /* '['  */ bit_print | bit_punct | bit_graph,
    /* '\\' */ bit_print | bit_punct | bit_graph,
    /* ']'  */ bit_print | bit_punct | bit_graph,
    /* '^'  */ bit_print | bit_punct | bit_graph,
    /* '_'  */ bit_print | bit_punct | bit_graph,
    /* '`'  */ bit_print | bit_punct | bit_graph,
    /* 'a'  */ LETTER (bit_lower | bit_xdigit),
    /* 'b'  */ LETTER (bit_lower | bit_xdigit),
    /* 'c'  */ LETTER (bit_lower | bit_xdigit),
    /* 'd'  */ LETTER (bit_lower | bit_xdigit),
    /* 'e'  */ LETTER (bit_lower | bit_xdigit),
    /* 'f'  */ LETTER (bit_lower | bit_xdigit),
    /* 'g'  */ LETTER (bit_lower),
    /* 'h'  */ LETTER (bit_lower),
    /* 'i'  */ LETTER (bit_lower),
    /* 'j'  */ LETTER (bit_lower),
    /* 'k'  */ LETTER (bit_lower),
    /* 'l'  */ LETTER (bit_lower),
    /* 'm'  */ LETTER (bit_lower),
    /* 'n'  */ LETTER (bit_lower),
    /* 'o'  */ LETTER (bit_lower),
    /* 'p'  */ LETTER (bit_lower),
    /* 'q'  */ LETTER (bit_lower),
    /* 'r'  */ LETTER (bit_lower),
    /* 's'  */ LETTER (bit_lower),
    /* 't'  */ LETTER (bit_lower),
    /* 'u'  */ LETTER (bit_lower),
    /* 'v'  */ LETTER (bit_lower),
    /* 'w'  */ LETTER (bit_lower),
    /* 'x'  */ LETTER (bit_lower),
    /* 'y'  */ LETTER (bit_lower),
    /* 'z'  */ LETTER (bit_lower),
    /* '{'  */ bit_print | bit_punct | bit_graph,
    /* '|'  */ bit_print | bit_punct | bit_graph,
    /* '}'  */ bit_print | bit_punct | bit_graph,
    /* '~'  */ bit_print | bit_punct | bit_graph,
    /* 0x7f */ bit_cntrl

#elif 'A' == 0xc1

    /* NUL */ bit_cntrl,
    /* SOH */ bit_cntrl,
    /* STX */ bit_cntrl,
    /* ETX */ bit_cntrl,
    /* PF  */ bit_cntrl,
    /* HT  */ bit_cntrl,
    /* LC  */ bit_cntrl,
    /* DEL */ bit_cntrl,
    /*     */ 0,
    /*     */ 0,
    /* SMM */ bit_cntrl,
    /* VT  */ bit_cntrl,
    /* FF  */ bit_cntrl,
    /* CR  */ bit_cntrl,
    /* SO  */ bit_cntrl,
    /* SI  */ bit_cntrl,
    /* DLE */ bit_cntrl,
    /* DC1 */ bit_cntrl,
    /* DC2 */ bit_cntrl,
    /* TM  */ bit_cntrl,
    /* RES */ bit_cntrl,
    /* NL  */ bit_cntrl,
    /* BS  */ bit_cntrl,
    /* IL  */ bit_cntrl,
    /* CAN */ bit_cntrl,
    /* EM  */ bit_cntrl,
    /* CC  */ bit_cntrl,
    /* CU1 */ bit_cntrl,
    /* IFS */ bit_cntrl,
    /* IGS */ bit_cntrl,
    /* IRS */ bit_cntrl,
    /* IUS */ bit_cntrl,
    /* DS  */ bit_cntrl,
    /* SOS */ bit_cntrl,
    /* FS  */ bit_cntrl,
    /*     */ 0,
    /* BYP */ bit_cntrl,
    /* LF  */ bit_cntrl,
    /* ETB */ bit_cntrl,
    /* ESC */ bit_cntrl,
    /*     */ 0,
    /*     */ 0,
    /* SM  */ bit_cntrl,
    /* CU2 */ bit_cntrl,
    /*     */ 0,
    /* ENQ */ bit_cntrl,
    /* ACK */ bit_cntrl,
    /* BEL */ bit_cntrl,
    /*     */ 0,
    /*     */ 0,
    /* SYN */ bit_cntrl,
    /*     */ 0,
    /* PN  */ bit_cntrl,
    /* RS  */ bit_cntrl,
    /* UC  */ bit_cntrl,
    /* EOT */ bit_cntrl,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* CU3 */ bit_cntrl,
    /* DC4 */ bit_cntrl,
    /* NAK */ bit_cntrl,
    /*     */ 0,
    /* SUB */ bit_cntrl,
    /* SP  */ bit_cntrl,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* ct. */ bit_print | bit_punct | bit_graph,
    /* '.' */ bit_print | bit_punct | bit_graph,
    /* '<' */ bit_print | bit_punct | bit_graph,
    /* '(' */ bit_print | bit_punct | bit_graph,
    /* '+' */ bit_print | bit_punct | bit_graph,
    /* '|' */ bit_print | bit_punct | bit_graph,
    /* '&' */ bit_print | bit_punct | bit_graph,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* '!' */ bit_print | bit_punct | bit_graph,
    /* '$' */ bit_print | bit_punct | bit_graph,
    /* '*' */ bit_print | bit_punct | bit_graph,
    /* ')' */ bit_print | bit_punct | bit_graph,
    /* ';' */ bit_print | bit_punct | bit_graph,
    /* '~' */ bit_print | bit_punct | bit_graph,
    /* '-' */ bit_print | bit_punct | bit_graph,
    /* '/' */ bit_print | bit_punct | bit_graph,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* ',' */ bit_print | bit_punct | bit_graph,
    /* '%' */ bit_print | bit_punct | bit_graph,
    /* '_' */ bit_print | bit_punct | bit_graph,
    /* '>' */ bit_print | bit_punct | bit_graph,
    /* '?' */ bit_print | bit_punct | bit_graph,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* ':' */ bit_print | bit_punct | bit_graph,
    /* '#' */ bit_print | bit_punct | bit_graph,
    /* '@' */ bit_print | bit_punct | bit_graph,
    /* ''' */ bit_print | bit_punct | bit_graph,
    /* '=' */ bit_print | bit_punct | bit_graph,
    /* '"' */ bit_print | bit_punct | bit_graph,
    /*     */ 0,
    /* 'a' */ LETTER (bit_lower | bit_xdigit),
    /* 'b' */ LETTER (bit_lower | bit_xdigit),
    /* 'c' */ LETTER (bit_lower | bit_xdigit),
    /* 'd' */ LETTER (bit_lower | bit_xdigit),
    /* 'e' */ LETTER (bit_lower | bit_xdigit),
    /* 'f' */ LETTER (bit_lower | bit_xdigit),
    /* 'g' */ LETTER (bit_lower),
    /* 'h' */ LETTER (bit_lower),
    /* 'i' */ LETTER (bit_lower),
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* 'j' */ LETTER (bit_lower),
    /* 'k' */ LETTER (bit_lower),
    /* 'l' */ LETTER (bit_lower),
    /* 'm' */ LETTER (bit_lower),
    /* 'n' */ LETTER (bit_lower),
    /* 'o' */ LETTER (bit_lower),
    /* 'p' */ LETTER (bit_lower),
    /* 'q' */ LETTER (bit_lower),
    /* 'r' */ LETTER (bit_lower),
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* 's' */ LETTER (bit_lower),
    /* 't' */ LETTER (bit_lower),
    /* 'u' */ LETTER (bit_lower),
    /* 'v' */ LETTER (bit_lower),
    /* 'w' */ LETTER (bit_lower),
    /* 'x' */ LETTER (bit_lower),
    /* 'y' */ LETTER (bit_lower),
    /* 'z' */ LETTER (bit_lower),
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* '`' */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* 'A' */ LETTER (bit_upper | bit_xdigit),
    /* 'B' */ LETTER (bit_upper | bit_xdigit),
    /* 'C' */ LETTER (bit_upper | bit_xdigit),
    /* 'D' */ LETTER (bit_upper | bit_xdigit),
    /* 'E' */ LETTER (bit_upper | bit_xdigit),
    /* 'F' */ LETTER (bit_upper | bit_xdigit),
    /* 'G' */ LETTER (bit_upper),
    /* 'H' */ LETTER (bit_upper),
    /* 'I' */ LETTER (bit_upper),
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* 'J' */ LETTER (bit_upper),
    /* 'K' */ LETTER (bit_upper),
    /* 'L' */ LETTER (bit_upper),
    /* 'M' */ LETTER (bit_upper),
    /* 'N' */ LETTER (bit_upper),
    /* 'O' */ LETTER (bit_upper),
    /* 'P' */ LETTER (bit_upper),
    /* 'Q' */ LETTER (bit_upper),
    /* 'R' */ LETTER (bit_upper),
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* 'S' */ LETTER (bit_upper),
    /* 'T' */ LETTER (bit_upper),
    /* 'U' */ LETTER (bit_upper),
    /* 'V' */ LETTER (bit_upper),
    /* 'W' */ LETTER (bit_upper),
    /* 'X' */ LETTER (bit_upper),
    /* 'Y' */ LETTER (bit_upper),
    /* 'Z' */ LETTER (bit_upper),
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* '0' */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '1' */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '2' */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '3' */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '4' */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '5' */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '6' */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '7' */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '8' */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '9' */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0

#else   // 'A' != 0x41 && 'A' != 0xc1
#  error unknown character set (neither ASCII nor EBCDIC)
#endif   // ASCII or EBCDIC

};


static void
test_behavior ()
{
    rw_info (0, 0, 0, "behavior of <cctype> functions in the \"C\" locale");

    char extra_str [80];
    char missing_str [80];

    for (int i = 0; i != 256; ++i) {

        typedef unsigned char UChar;

        const int c = UChar (i);

        int mask = 0;

        mask |= std::isalnum (c) ? bit_alnum : 0;
        mask |= std::isalpha (c) ? bit_alpha : 0;
        mask |= std::iscntrl (c) ? bit_cntrl : 0;
        mask |= std::isdigit (c) ? bit_digit : 0;
        mask |= std::isgraph (c) ? bit_graph : 0;
        mask |= std::islower (c) ? bit_lower : 0;
        mask |= std::isprint (c) ? bit_print : 0;
        mask |= std::ispunct (c) ? bit_punct : 0;
        mask |= std::isspace (c) ? bit_space : 0;
        mask |= std::isupper (c) ? bit_upper : 0;
        mask |= std::isxdigit (c) ? bit_xdigit : 0;

        const int extra_bits   = mask & ~char_mask [i];
        const int missing_bits = ~mask & char_mask [i];

        rw_assert (mask == char_mask [i], 0, 0,
                   "%#c mask%{?} missing bits %s (%#x)%{;}"
                   "%{?} extra bits %s (%#x)%{;}",
                   c,
                   missing_bits,
                   get_bitmask (missing_bits, missing_str), missing_bits,
                   extra_bits,
                   get_bitmask (extra_bits, extra_str), extra_bits);
    }
}

/**************************************************************************/

static int rw_opt_no_behavior;   // for --no-behavior

static int
run_test (int, char**)
{
    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, 0, "checking for the absence of masking macros");

    // verify the shadow macros are not #defined (explicitly
    // disallowed by 17.4.1.2, p6 and Footnote 159 of C++ '03)
    for (unsigned i = 0; cctype_macros [i]; ++i) {
        rw_assert ('\0' == cctype_macros [i][0], 0, 0,
                   "macro %s unexpectedly #defined", cctype_macros [i]);
    }

    //////////////////////////////////////////////////////////////////
    // verify that each function is defined
#define TEST(function)                                          \
    do {                                                        \
        rw_info (0, 0, 0, "%s(int) definition", #function);     \
        function (function_called = 1);                         \
        rw_assert (1 == function_called, 0, __LINE__,           \
                   "%s (int) not defined", #function);          \
    } while (0)

    TEST (std::isalnum);
    TEST (std::isalpha);
    TEST (std::iscntrl);
    TEST (std::isdigit);
    TEST (std::isgraph);
    TEST (std::islower);
    TEST (std::isprint);
    TEST (std::ispunct);
    TEST (std::isupper);
    TEST (std::isxdigit);
    TEST (std::tolower);
    TEST (std::toupper);

    //////////////////////////////////////////////////////////////////
    if (rw_opt_no_behavior)
        rw_note (0, 0, 0, "behavior test disabled (use --test-behavior)");
    else
        test_behavior ();

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.c.strings",
                    "header <cctype>",
                    run_test,
                    "|-no-behavior#0-1 ",
                    &rw_opt_no_behavior);
}
