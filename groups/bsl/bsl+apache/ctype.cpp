/**************************************************************************
 *
 * ctype.cpp - definitions of UserCtype facet members
 *
 * $Id: ctype.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC
#include <rw_ctype.h>

#include <stdarg.h>   // for va_arg(), va_list
#include <string.h>   // for memset()

/**************************************************************************/

typedef unsigned char UChar;


_TEST_EXPORT int
rw_vasnprintf (char**, size_t*, const char*, va_list);


static void
_rw_throw (const char *file, int line, const char *fmt, ...)
{
    _RWSTD_UNUSED (file);
    _RWSTD_UNUSED (line);

    struct Exception: UserCtypeBase::Exception {
        char what_ [256];

        /* virtual */ const char* what () const {
            return what_;
        }
    };

    Exception ex;

    va_list va;
    va_start (va, fmt);

    char *buf = ex.what_;
    size_t bufsize = sizeof ex.what_;
    rw_vasnprintf (&buf, &bufsize, fmt, va);

    va_end (va);

    throw ex;
}

/**************************************************************************/

static const char* const
_rw_func_names[] = {
    "is(mask, char_type)",
    "is(const char_type*, const char_type*, mask*)",
    "scan_is(mask, const char_type*, const char_type*)",
    "scan_not(mask, const char_type*, const char_type*)",
    "to_upper(char_type)",
    "to_upper(char_type*, const char_type*)",
    "to_lower(char_type)",
    "to_lower(char_type*, const char_type*)",
    "widen(char)",
    "widen(const char*, const char*, char_type*)",
    "narrow(char_type, char)",
    "narrow(const char_type*, const char_type*, char, char*)"
};

/**************************************************************************/

UserCtypeBase::Exception::
~Exception ()
{
    // no-op
}

/**************************************************************************/

static const int
_rw_char_masks [256] = {

#define ALPHA     std::ctype_base::alpha
#define ALNUM     std::ctype_base::alnum
#define CNTRL     std::ctype_base::cntrl
#define DIGIT     std::ctype_base::digit
#define GRAPH     std::ctype_base::graph
#define LOWER     std::ctype_base::lower
#define PRINT     std::ctype_base::print
#define PUNCT     std::ctype_base::punct
#define SPACE     std::ctype_base::space
#define UPPER     std::ctype_base::upper
#define XDIGIT    std::ctype_base::xdigit


#undef  LETTER
#define LETTER(m) (PRINT | ALPHA | GRAPH | m)

#if 'A' == 0x41   // ASCII

    /* 0x00 NUL */ CNTRL,
    /* 0x01 SOH */ CNTRL,
    /* 0x02 STX */ CNTRL,
    /* 0x03 ETX */ CNTRL,
    /* 0x04 EOT */ CNTRL,
    /* 0x05 ENQ */ CNTRL,
    /* 0x06 ACK */ CNTRL,
    /* '\a' BEL */ CNTRL,
    /* 0x08 BS  */ CNTRL,
    /* '\t' TAB */ SPACE | CNTRL,
    /* '\n' LF  */ SPACE | CNTRL,
    /* 0x0b VT  */ SPACE | CNTRL,
    /* '\f' FF  */ SPACE | CNTRL,
    /* '\r' CR  */ SPACE | CNTRL,
    /* 0x0e SO  */ CNTRL,
    /* 0x0f SI  */ CNTRL,
    /* 0x10 DLE */ CNTRL,
    /* 0x11 DC1 */ CNTRL,
    /* 0x12 DC2 */ CNTRL,
    /* 0x13 DC3 */ CNTRL,
    /* 0x14 DC4 */ CNTRL,
    /* 0x15 NAK */ CNTRL,
    /* 0x16 SYN */ CNTRL,
    /* 0x17 ETB */ CNTRL,
    /* 0x18 CAN */ CNTRL,
    /* 0x19 EM  */ CNTRL,
    /* 0x1a SUB */ CNTRL,
    /* 0x1b ESC */ CNTRL,
    /* 0x1c FS  */ CNTRL,
    /* 0x1d GS  */ CNTRL,
    /* 0x1e RS  */ CNTRL,
    /* 0x1f US  */ CNTRL,
    /* ' '      */ SPACE | PRINT,
    /* '!'      */ PRINT | PUNCT | GRAPH,
    /* '"'      */ PRINT | PUNCT | GRAPH,
    /* '#'      */ PRINT | PUNCT | GRAPH,
    /* '$'      */ PRINT | PUNCT | GRAPH,
    /* '%'      */ PRINT | PUNCT | GRAPH,
    /* '&'      */ PRINT | PUNCT | GRAPH,
    /* '\''     */ PRINT | PUNCT | GRAPH,
    /* '('      */ PRINT | PUNCT | GRAPH,
    /* ')'      */ PRINT | PUNCT | GRAPH,
    /* '*'      */ PRINT | PUNCT | GRAPH,
    /* '+'      */ PRINT | PUNCT | GRAPH,
    /* ','      */ PRINT | PUNCT | GRAPH,
    /* '-'      */ PRINT | PUNCT | GRAPH,
    /* '.'      */ PRINT | PUNCT | GRAPH,
    /* '/'      */ PRINT | PUNCT | GRAPH,
    /* '0'      */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* '1'      */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* '2'      */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* '3'      */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* '4'      */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* '5'      */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* '6'      */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* '7'      */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* '8'      */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* '9'      */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* ':'      */ PRINT | PUNCT | GRAPH,
    /* ';'      */ PRINT | PUNCT | GRAPH,
    /* '<'      */ PRINT | PUNCT | GRAPH,
    /* '='      */ PRINT | PUNCT | GRAPH,
    /* '>'      */ PRINT | PUNCT | GRAPH,
    /* '?'      */ PRINT | PUNCT | GRAPH,
    /* '@'      */ PRINT | PUNCT | GRAPH,

    /* 'A'      */ LETTER (UPPER | XDIGIT),
    /* 'B'      */ LETTER (UPPER | XDIGIT),
    /* 'C'      */ LETTER (UPPER | XDIGIT),
    /* 'D'      */ LETTER (UPPER | XDIGIT),
    /* 'E'      */ LETTER (UPPER | XDIGIT),
    /* 'F'      */ LETTER (UPPER | XDIGIT),
    /* 'G'      */ LETTER (UPPER),
    /* 'H'      */ LETTER (UPPER),
    /* 'I'      */ LETTER (UPPER),
    /* 'J'      */ LETTER (UPPER),
    /* 'K'      */ LETTER (UPPER),
    /* 'L'      */ LETTER (UPPER),
    /* 'M'      */ LETTER (UPPER),
    /* 'N'      */ LETTER (UPPER),
    /* 'O'      */ LETTER (UPPER),
    /* 'P'      */ LETTER (UPPER),
    /* 'Q'      */ LETTER (UPPER),
    /* 'R'      */ LETTER (UPPER),
    /* 'S'      */ LETTER (UPPER),
    /* 'T'      */ LETTER (UPPER),
    /* 'U'      */ LETTER (UPPER),
    /* 'V'      */ LETTER (UPPER),
    /* 'W'      */ LETTER (UPPER),
    /* 'X'      */ LETTER (UPPER),
    /* 'Y'      */ LETTER (UPPER),
    /* 'Z'      */ LETTER (UPPER),

    /* '['      */ PRINT | PUNCT | GRAPH,
    /* '\\'     */ PRINT | PUNCT | GRAPH,
    /* ']'      */ PRINT | PUNCT | GRAPH,
    /* '^'      */ PRINT | PUNCT | GRAPH,
    /* '_'      */ PRINT | PUNCT | GRAPH,
    /* '`'      */ PRINT | PUNCT | GRAPH,

    /* 'a'      */ LETTER (LOWER | XDIGIT),
    /* 'b'      */ LETTER (LOWER | XDIGIT),
    /* 'c'      */ LETTER (LOWER | XDIGIT),
    /* 'd'      */ LETTER (LOWER | XDIGIT),
    /* 'e'      */ LETTER (LOWER | XDIGIT),
    /* 'f'      */ LETTER (LOWER | XDIGIT),
    /* 'g'      */ LETTER (LOWER),
    /* 'h'      */ LETTER (LOWER),
    /* 'i'      */ LETTER (LOWER),
    /* 'j'      */ LETTER (LOWER),
    /* 'k'      */ LETTER (LOWER),
    /* 'l'      */ LETTER (LOWER),
    /* 'm'      */ LETTER (LOWER),
    /* 'n'      */ LETTER (LOWER),
    /* 'o'      */ LETTER (LOWER),
    /* 'p'      */ LETTER (LOWER),
    /* 'q'      */ LETTER (LOWER),
    /* 'r'      */ LETTER (LOWER),
    /* 's'      */ LETTER (LOWER),
    /* 't'      */ LETTER (LOWER),
    /* 'u'      */ LETTER (LOWER),
    /* 'v'      */ LETTER (LOWER),
    /* 'w'      */ LETTER (LOWER),
    /* 'x'      */ LETTER (LOWER),
    /* 'y'      */ LETTER (LOWER),
    /* 'z'      */ LETTER (LOWER),

    /* '{'      */ PRINT | PUNCT | GRAPH,
    /* '|'      */ PRINT | PUNCT | GRAPH,
    /* '}'      */ PRINT | PUNCT | GRAPH,
    /* '~'      */ PRINT | PUNCT | GRAPH,
    /* 0x7f DEL */ CNTRL

#elif 'A' == 0xc1   // EBCDIC

    /* NUL */ CNTRL,
    /* SOH */ CNTRL,
    /* STX */ CNTRL,
    /* ETX */ CNTRL,
    /* PF  */ CNTRL,
    /* HT  */ CNTRL,
    /* LC  */ CNTRL,
    /* DEL */ CNTRL,
    /*     */ 0,
    /*     */ 0,
    /* SMM */ CNTRL,
    /* VT  */ SPACE | CNTRL,
    /* FF  */ SPACE | CNTRL,
    /* CR  */ SPACE | CNTRL,
    /* SO  */ CNTRL,
    /* SI  */ CNTRL,
    /* DLE */ CNTRL,
    /* DC1 */ CNTRL,
    /* DC2 */ CNTRL,
    /* TM  */ CNTRL,
    /* RES */ CNTRL,
    /* NL  */ CNTRL,
    /* BS  */ CNTRL,
    /* IL  */ CNTRL,
    /* CAN */ CNTRL,
    /* EM  */ CNTRL,
    /* CC  */ CNTRL,
    /* CU1 */ CNTRL,
    /* IFS */ CNTRL,
    /* IGS */ CNTRL,
    /* IRS */ CNTRL,
    /* IUS */ CNTRL,
    /* DS  */ CNTRL,
    /* SOS */ CNTRL,
    /* FS  */ CNTRL,
    /*     */ 0,
    /* BYP */ CNTRL,
    /* LF  */ SPACE | CNTRL,
    /* ETB */ CNTRL,
    /* ESC */ CNTRL,
    /*     */ 0,
    /*     */ 0,
    /* SM  */ CNTRL,
    /* CU2 */ CNTRL,
    /*     */ 0,
    /* ENQ */ CNTRL,
    /* ACK */ CNTRL,
    /* BEL */ CNTRL,
    /*     */ 0,
    /*     */ 0,
    /* SYN */ CNTRL,
    /*     */ 0,
    /* PN  */ CNTRL,
    /* RS  */ CNTRL,
    /* UC  */ CNTRL,
    /* EOT */ CNTRL,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* CU3 */ CNTRL,
    /* DC4 */ CNTRL,
    /* NAK */ CNTRL,
    /*     */ 0,
    /* SUB */ CNTRL,
    /* ' ' */ SPACE | PRINT,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* ct. */ PRINT | PUNCT | GRAPH,
    /* '.' */ PRINT | PUNCT | GRAPH,
    /* '<' */ PRINT | PUNCT | GRAPH,
    /* '(' */ PRINT | PUNCT | GRAPH,
    /* '+' */ PRINT | PUNCT | GRAPH,
    /* '|' */ PRINT | PUNCT | GRAPH,
    /* '&' */ PRINT | PUNCT | GRAPH,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* '!' */ PRINT | PUNCT | GRAPH,
    /* '$' */ PRINT | PUNCT | GRAPH,
    /* '*' */ PRINT | PUNCT | GRAPH,
    /* ')' */ PRINT | PUNCT | GRAPH,
    /* ';' */ PRINT | PUNCT | GRAPH,
    /* '~' */ PRINT | PUNCT | GRAPH,
    /* '-' */ PRINT | PUNCT | GRAPH,
    /* '/' */ PRINT | PUNCT | GRAPH,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* ',' */ PRINT | PUNCT | GRAPH,
    /* '%' */ PRINT | PUNCT | GRAPH,
    /* '_' */ PRINT | PUNCT | GRAPH,
    /* '>' */ PRINT | PUNCT | GRAPH,
    /* '?' */ PRINT | PUNCT | GRAPH,
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
    /* ':' */ PRINT | PUNCT | GRAPH,
    /* '#' */ PRINT | PUNCT | GRAPH,
    /* '@' */ PRINT | PUNCT | GRAPH,
    /* ''' */ PRINT | PUNCT | GRAPH,
    /* '=' */ PRINT | PUNCT | GRAPH,
    /* '"' */ PRINT | PUNCT | GRAPH,
    /*     */ 0,
    /* 'a' */ LETTER (LOWER | XDIGIT),
    /* 'b' */ LETTER (LOWER | XDIGIT),
    /* 'c' */ LETTER (LOWER | XDIGIT),
    /* 'd' */ LETTER (LOWER | XDIGIT),
    /* 'e' */ LETTER (LOWER | XDIGIT),
    /* 'f' */ LETTER (LOWER | XDIGIT),
    /* 'g' */ LETTER (LOWER),
    /* 'h' */ LETTER (LOWER),
    /* 'i' */ LETTER (LOWER),
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* 'j' */ LETTER (LOWER),
    /* 'k' */ LETTER (LOWER),
    /* 'l' */ LETTER (LOWER),
    /* 'm' */ LETTER (LOWER),
    /* 'n' */ LETTER (LOWER),
    /* 'o' */ LETTER (LOWER),
    /* 'p' */ LETTER (LOWER),
    /* 'q' */ LETTER (LOWER),
    /* 'r' */ LETTER (LOWER),
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* 's' */ LETTER (LOWER),
    /* 't' */ LETTER (LOWER),
    /* 'u' */ LETTER (LOWER),
    /* 'v' */ LETTER (LOWER),
    /* 'w' */ LETTER (LOWER),
    /* 'x' */ LETTER (LOWER),
    /* 'y' */ LETTER (LOWER),
    /* 'z' */ LETTER (LOWER),
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
    /* 'A' */ LETTER (UPPER | XDIGIT),
    /* 'B' */ LETTER (UPPER | XDIGIT),
    /* 'C' */ LETTER (UPPER | XDIGIT),
    /* 'D' */ LETTER (UPPER | XDIGIT),
    /* 'E' */ LETTER (UPPER | XDIGIT),
    /* 'F' */ LETTER (UPPER | XDIGIT),
    /* 'G' */ LETTER (UPPER),
    /* 'H' */ LETTER (UPPER),
    /* 'I' */ LETTER (UPPER),
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* 'J' */ LETTER (UPPER),
    /* 'K' */ LETTER (UPPER),
    /* 'L' */ LETTER (UPPER),
    /* 'M' */ LETTER (UPPER),
    /* 'N' */ LETTER (UPPER),
    /* 'O' */ LETTER (UPPER),
    /* 'P' */ LETTER (UPPER),
    /* 'Q' */ LETTER (UPPER),
    /* 'R' */ LETTER (UPPER),
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* 'S' */ LETTER (UPPER),
    /* 'T' */ LETTER (UPPER),
    /* 'U' */ LETTER (UPPER),
    /* 'V' */ LETTER (UPPER),
    /* 'W' */ LETTER (UPPER),
    /* 'X' */ LETTER (UPPER),
    /* 'Y' */ LETTER (UPPER),
    /* 'Z' */ LETTER (UPPER),
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* '0' */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* '1' */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* '2' */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* '3' */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* '4' */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* '5' */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* '6' */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* '7' */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* '8' */ PRINT | DIGIT | GRAPH | XDIGIT,
    /* '9' */ PRINT | DIGIT | GRAPH | XDIGIT,
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


UserCtypeBase::
UserCtypeBase (const char *cname)
    : cname_ (cname), masks_ (_rw_char_masks), chars_ (0),
      upper_ (0), lower_ (0), narrow_ (0), wide_ (0),
      n_all_calls_ (0),
      throw_char_ (-1), throw_on_invalid_ (false)
{
    memset (n_calls_, 0, sizeof n_calls_);
    memset (n_throws_, 0, sizeof n_throws_);
    memset (throw_at_calls_, 0, sizeof throw_at_calls_);
}

/**************************************************************************/

// records a call to the object's member function fun and
// throws an exception if the number of calls to the member
// function reaches the limit specified by throw_at_calls_
static void
_rw_funcall (const UserCtypeBase    &ctp,
             UserCtypeBase::MemFunc  mf)
{
    UserCtypeBase* const pctp = _RWSTD_CONST_CAST (UserCtypeBase*, &ctp);

    // increment the number of calls regardless of success
    ++pctp->n_all_calls_;
    ++pctp->n_calls_ [mf];

    // check the number of calls and throw an exception
    // if the specified limit has been reached
    if (ctp.n_calls_ [mf] == ctp.throw_at_calls_ [mf]) {
        // increment the exception counter for this function
        ++pctp->n_throws_ [mf];

        _rw_throw (__FILE__, __LINE__,
                   "UserCtype<%s>::%s: reached call limit of %zu",
                   ctp.cname_, _rw_func_names [mf], ctp.throw_at_calls_);

        RW_ASSERT (!"logic error: should not reach");
    }
}

/**************************************************************************/

static void
_rw_check_char (const UserCtypeBase   &ctp,
                UserCtypeBase::MemFunc mf,
                size_t                 uch)
{
    if (0 <= ctp.throw_char_ && size_t (ctp.throw_char_) == uch) {

        UserCtypeBase* const pctp = _RWSTD_CONST_CAST (UserCtypeBase*, &ctp);
        ++pctp->n_throws_ [mf];

        _rw_throw (__FILE__, __LINE__,
                   "UserCtype<%s>::%s: invalid character: %{#lc}",
                   ctp.cname_, _rw_func_names [mf], int (uch));

        RW_ASSERT (!"logic error: should not reach");
    }
}

/**************************************************************************/

static UserCtype<char>::mask*
_rw_make_mask_vec (const int *chars, const int *masks)
{
    if (0 == chars) {
        RW_ASSERT (0 == masks);
        return 0;
    }

    RW_ASSERT (0 != masks);

    typedef UserCtype<char>::mask Mask;

    const size_t size = UserCtype<char>::table_size;
    Mask* const  vec  = new Mask [size];

    memset (vec, 0, size * sizeof *vec);

    for (size_t i = 0; 0 <= chars [i]; ++i)
        vec [UChar (chars [i])] = Mask (masks [i]);

    return vec;
}


UserCtype<char>::
UserCtype (size_t refs /* = 0 */)
    : Base (0, false, refs),
      UserCtypeBase ("char")
{
    chars_ = 0;
    masks_ = _rw_char_masks;
}


UserCtype<char>::
UserCtype (const int *chars, const int *masks, size_t refs /* = 0 */)
    : Base (_rw_make_mask_vec (chars, masks), true, refs),
      UserCtypeBase ("char")
{
    chars_ = chars;
    masks_ = masks;
}


UserCtype<char>::char_type
UserCtype<char>::
do_toupper (char_type ch) const
{
    _rw_funcall (*this, mf_toupper);

    const UChar uch = UChar (ch);

    _rw_check_char (*this, mf_toupper, uch);

    if (0 == upper_) {
        if (masks_ [uch] & LOWER) {
#if 'A' == 0x41   // ASCII
            return char_type (uch - 32);
#else   // EBCDIC
            return char_type (uch + 64);
#endif
        }
    }
    else {
        // FIXME: implement this
        RW_ASSERT (!"do_toupper() not implemented");
    }

    return ch;
}


const UserCtype<char>::char_type*
UserCtype<char>::
do_toupper (char_type *lo, const char_type *hi) const
{
    _rw_funcall (*this, mf_toupper_range);

    if (0 == upper_) {
        for ( ; lo < hi; ++lo) {

            const UChar uch = UChar (*lo);

            _rw_check_char (*this, mf_toupper, uch);

            if (masks_ [uch] & LOWER) {
#if 'A' == 0x41   // ASCII
                *lo = char_type (uch - 32);
#else   // EBCDIC
                *lo = char_type (uch + 64);
#endif
            }
        }
    }
    else {
        // FIXME: implement this
        RW_ASSERT (!"do_toupper() not implemented");
    }

    return lo;
}


UserCtype<char>::char_type
UserCtype<char>::
do_tolower (char_type ch) const
{
    _rw_funcall (*this, mf_tolower);

    _rw_check_char (*this, mf_toupper, UChar (ch));

    if (0 == chars_) {
        if (masks_ [UChar (ch)] & UPPER) {
#if 'A' == 0x41   // ASCII
            return char_type (UChar (ch + 32));
#else   // EBCDIC
            return char_type (UChar (ch - 64));
#endif
        }
    }
    else {
        // FIXME: implement this
        RW_ASSERT (!"do_toupper() not implemented");
    }

    return ch;
}


const UserCtype<char>::char_type*
UserCtype<char>::
do_tolower (char_type *lo, const char_type *hi) const
{
    _rw_funcall (*this, mf_tolower_range);

    if (0 == chars_) {
        for ( ; lo < hi; ++lo) {

            const UChar uch = UChar (*lo);

            _rw_check_char (*this, mf_toupper, uch);

            if (masks_ [uch] & UPPER) {
#if 'A' == 0x41   // ASCII
                *lo = char_type (uch - 32);
#else   // EBCDIC
                *lo = char_type (uch + 64);
#endif
            }
        }
    }
    else {
        // FIXME: implement this
        RW_ASSERT (!"do_toupper() not implemented");
    }

    return lo;
}


UserCtype<char>::char_type
UserCtype<char>::
do_widen (char ch) const
{
    _rw_funcall (*this, mf_widen);

    const UChar uch = UChar (ch);

    _rw_check_char (*this, mf_toupper, uch);

    if (narrow_ && wide_) {
        for (size_t i = 0; 0 <= narrow_ [i]; ++i) {
            if (uch == UChar (narrow_ [i])) {
                if (0 <= wide_ [i])
                    return UChar (wide_ [i]);

                if (throw_on_invalid_)
                    throw;

                break;
            }
        }
    }

    return ch;
}


const char*
UserCtype<char>::
do_widen (const char *lo, const char *hi, char_type *dst) const
{
    _rw_funcall (*this, mf_widen_range);

    if (narrow_ && wide_) {
        for ( ; lo < hi; ++lo, ++dst) {

            const UChar uch = *lo;

            _rw_check_char (*this, mf_toupper, uch);

            for (size_t i = 0; 0 <= narrow_ [i]; ++i) {
                if (uch == UChar (narrow_ [i])) {
                    if (0 <= wide_ [i])
                        *dst = UChar (wide_ [i]);

                    if (throw_on_invalid_)
                        throw;

                    break;
                }
            }
        }
    }
    else if (throw_char_ < 0 || int (_RWSTD_UCHAR_MAX) < throw_char_) {
        const size_t nelems = size_t (hi - lo);
        memcpy (dst, lo, nelems);
        lo += nelems;
    }
    else {
        for (; lo < hi; ++lo, ++dst) {

            const UChar uch = UChar (*lo);

            _rw_check_char (*this, mf_toupper, uch);

            *dst = char_type (uch);
        }
    }

    return lo;
}


char UserCtype<char>::
do_narrow (char_type ch, char dfault) const
{
    _rw_funcall (*this, mf_narrow);

    const UChar uch = UChar (ch);

    _rw_check_char (*this, mf_toupper, uch);

    if (narrow_ && wide_) {
        for (size_t i = 0; 0 <= wide_ [i]; ++i) {
            if (uch == UChar (wide_ [i])) {
                if (0 <= wide_ [i])
                    return UChar (wide_ [i]);

                if (throw_on_invalid_)
                    throw;

                break;
            }
        }

        return dfault;
    }

    return ch;
}


const UserCtype<char>::char_type*
UserCtype<char>::
do_narrow (const char_type *lo, const char_type *hi, char dflt, char *dst) const
{
    _rw_funcall (*this, mf_narrow_range);

    if (narrow_ && wide_) {
        for ( ; lo < hi; ++lo, ++dst) {

            const UChar uch = UChar (*lo);

            _rw_check_char (*this, mf_toupper, uch);

            for (size_t i = 0; 0 <= wide_ [i]; ++i) {
                if (uch == UChar (wide_ [i])) {
                    *dst = 0 <= narrow_ [i] ? UChar (wide_ [i]) : UChar (dflt);
                    break;
                }
            }
        }
    }
    else if (throw_char_ < 0 || int (_RWSTD_UCHAR_MAX) < throw_char_) {
        const size_t nelems = size_t (hi - lo);
        memcpy (dst, lo, nelems);
        lo += nelems;
    }
    else {
        for (; lo < hi; ++lo, ++dst) {

            const UChar uch = UChar (*lo);

            _rw_check_char (*this, mf_toupper, uch);

            *dst = char_type (uch);
        }
    }

    return lo;
}

/**************************************************************************/

#ifndef _RWSTD_NO_WCHAR_T

UserCtype<wchar_t>::
UserCtype (size_t refs /* = 0 */)
    : Base (refs), UserCtypeBase ("wchar_t")
{
    chars_ = 0;
    masks_ = _rw_char_masks;
}


UserCtype<wchar_t>::
UserCtype (const int *chars, const int *masks, size_t refs)
    : Base (refs), UserCtypeBase ("wchar_t")
{
    if (0 == masks) {
        // when masks is null so must chars
        RW_ASSERT (0 == chars);
        masks = _rw_char_masks;
    }

    chars_ = chars;
    masks_ = masks;
}


bool UserCtype<wchar_t>::
do_is (mask m, char_type ch) const
{
    _rw_funcall (*this, mf_is);

    const size_t uch = size_t (ch);

    _rw_check_char (*this, mf_toupper, uch);

    if (masks_ == _rw_char_masks) {
        return uch < 256 ? 0 != (masks_ [uch] & m) : false;
    }

    for (size_t i = 0; 0 <= chars_ [i]; ++i)
        if (chars_ [i] == int (ch))
            return 0 != (masks_ [i] & m);

    return false;
}


const UserCtype<wchar_t>::char_type*
UserCtype<wchar_t>::
do_is (const char_type *lo, const char_type *hi, mask *vec) const
{
    _rw_funcall (*this, mf_is_range);

    if (masks_ == _rw_char_masks) {
        for ( ; lo < hi; ++lo, ++vec) {

            const size_t uch = size_t (*lo);

            _rw_check_char (*this, mf_toupper, uch);

            *vec = mask (uch < 256 ? masks_ [uch] : 0);
        }
    }
    else {
        for ( ; lo < hi; ++lo, ++vec) {

            _rw_check_char (*this, mf_toupper, size_t (*lo));

            bool found = false;

            for (size_t i = 0; 0 <= chars_ [i]; ++i) {
                if (chars_ [i] == int (*lo)) {
                    *vec  = mask (masks_ [i]);
                    found = true;
                    break;
                }
            }

            if (!found && throw_on_invalid_)
                throw;
        }
    }

    return lo;
}


const UserCtype<wchar_t>::char_type*
UserCtype<wchar_t>::
do_scan_is (mask m, const char_type *lo, const char_type *hi) const
{
    _rw_funcall (*this, mf_scan_is);

    if (masks_ == _rw_char_masks) {
        for ( ; lo < hi; ++lo) {
            const size_t uch = size_t (*lo);

            _rw_check_char (*this, mf_toupper, uch);

            if (uch < 256 && masks_ [uch] & m)
                break;
        }
    }
    else {
        for ( ; lo < hi; ++lo) {

            _rw_check_char (*this, mf_toupper, size_t (*lo));

            for (size_t i = 0; 0 <= chars_ [i]; ++i) {
                if (chars_ [i] == int (*lo) && masks_ [i] & m)
                    return lo;
            }
        }
    }

    return lo;
}


const UserCtype<wchar_t>::char_type*
UserCtype<wchar_t>::
do_scan_not (mask m, const char_type *lo, const char_type *hi) const
{
    _rw_funcall (*this, mf_scan_not);

    if (masks_ == _rw_char_masks) {
        for ( ; lo < hi; ++lo) {

            const size_t uch = size_t (*lo);

            _rw_check_char (*this, mf_toupper, uch);

            if (uch < 256 && 0 == (masks_ [uch] & m))
                break;
        }
    }
    else {
        for ( ; lo < hi; ++lo) {

            _rw_check_char (*this, mf_toupper, size_t (*lo));

            for (size_t i = 0; 0 <= chars_ [i]; ++i) {
                if (chars_ [i] == int (*lo) && 0 == (masks_ [i] & m))
                    return lo;
            }
        }
    }

    return lo;
}


UserCtype<wchar_t>::char_type
UserCtype<wchar_t>::
do_toupper (char_type ch) const
{
    _rw_funcall (*this, mf_toupper);

    const size_t uch = size_t (ch);

    _rw_check_char (*this, mf_toupper, uch);

    if (0 == upper_) {

        if (uch < 256 && masks_ [uch] & LOWER) {
#if 'A' == 0x41   // ASCII
            return char_type (uch - 32);
#else   // EBCDIC
            return char_type (uch + 64);
#endif
        }
    }
    else {
        // FIXME: implement this
        RW_ASSERT (!"do_toupper() not implemented");
    }

    return ch;
}


const UserCtype<wchar_t>::char_type*
UserCtype<wchar_t>::
do_toupper (char_type *lo, const char_type *hi) const
{
    _rw_funcall (*this, mf_toupper_range);

    if (0 == upper_) {
        for ( ; lo < hi; ++lo) {
            const size_t uch = size_t (*lo);

            _rw_check_char (*this, mf_toupper, uch);

            if (uch < 256 && masks_ [uch] & LOWER) {
#if 'A' == 0x41   // ASCII
                *lo = char_type (uch - 32);
#else   // EBCDIC
                *lo = char_type (uch + 64);
#endif
            }
        }
    }
    else {
        // FIXME: implement this
        RW_ASSERT (!"do_toupper() not implemented");
    }

    return lo;
}


UserCtype<wchar_t>::char_type
UserCtype<wchar_t>::
do_tolower (char_type ch) const
{
    _rw_funcall (*this, mf_tolower);

    const size_t uch = size_t (ch);

    _rw_check_char (*this, mf_toupper, uch);

    if (masks_ == _rw_char_masks) {
        if (uch < 256 && masks_ [uch] & UPPER) {
#if 'A' == 0x41   // ASCII
            return char_type (uch + 32);
#else   // EBCDIC
            return char_type (uch - 64);
#endif
        }
    }
    else {
        // FIXME: implement this
        RW_ASSERT (!"do_toupper() not implemented");
    }

    return ch;
}


const UserCtype<wchar_t>::char_type*
UserCtype<wchar_t>::
do_tolower (char_type *lo, const char_type *hi) const
{
    _rw_funcall (*this, mf_tolower_range);

    if (masks_ == _rw_char_masks) {
        for ( ; lo < hi; ++lo) {

            const size_t uch = size_t (*lo);

            if (uch < 256 && masks_ [uch] & UPPER) {
#if 'A' == 0x41   // ASCII
                *lo = char_type (uch - 32);
#else   // EBCDIC
                *lo = char_type (uch + 64);
#endif
            }
        }
    }
    else {
        // FIXME: implement this
        RW_ASSERT (!"do_toupper() not implemented");
    }

    return lo;
}


UserCtype<wchar_t>::char_type
UserCtype<wchar_t>::
do_widen (char ch) const
{
    _rw_funcall (*this, mf_widen);

    const UChar uch = UChar (ch);

    _rw_check_char (*this, mf_toupper, uch);

    if (narrow_ && wide_) {
        for (size_t i = 0; 0 <= narrow_ [i]; ++i) {
            if (int (uch) == narrow_ [i]) {
                if (0 <= wide_ [i])
                    return char_type (wide_ [i]);

                if (throw_on_invalid_)
                    throw;

                break;
            }
        }
    }

    return ch;
}


const char*
UserCtype<wchar_t>::
do_widen (const char *lo, const char *hi, char_type *dst) const
{
    _rw_funcall (*this, mf_widen_range);

    if (narrow_ && wide_) {
        for ( ; lo < hi; ++lo, ++dst) {

            const size_t uch = size_t (*lo);

            _rw_check_char (*this, mf_toupper, uch);

            for (size_t i = 0; 0 <= narrow_ [i]; ++i) {
                if (int (uch) == narrow_ [i]) {
                    if (0 <= wide_ [i])
                        *dst = char_type (wide_ [i]);

                    if (throw_on_invalid_)
                        throw;

                    break;
                }
            }
        }
    }
    else {
        for (; lo < hi; ++lo, ++dst) {

            const UChar uch = UChar (*lo);

            _rw_check_char (*this, mf_toupper, uch);

            *dst = char_type (uch);
        }
    }

    return lo;
}


char UserCtype<wchar_t>::
do_narrow (char_type ch, char dfault) const
{
    _rw_funcall (*this, mf_narrow);

    const size_t uch = size_t (ch);

    _rw_check_char (*this, mf_toupper, uch);

    if (narrow_ && wide_) {
        for (size_t i = 0; 0 <= wide_ [i]; ++i) {
            if (int (uch) == wide_ [i]) {
                if (0 <= wide_ [i])
                    return char (wide_ [i]);

                if (throw_on_invalid_)
                    throw;

                break;
            }
        }

        return dfault;
    }

    return char (uch);
}


const UserCtype<wchar_t>::char_type*
UserCtype<wchar_t>::
do_narrow (const char_type *lo, const char_type *hi, char dflt, char *dst) const
{
    _rw_funcall (*this, mf_narrow_range);

    if (narrow_ && wide_) {
        for ( ; lo < hi; ++lo, ++dst) {

            const size_t uch = size_t (*lo);

            _rw_check_char (*this, mf_toupper, uch);

            for (size_t i = 0; 0 <= wide_ [i]; ++i) {
                if (int (uch) == wide_ [i]) {
                    *dst = 0 <= narrow_ [i] ? char (wide_ [i]) : dflt;
                    break;
                }
            }
        }
    }
    else {
        for (; lo < hi; ++lo, ++dst) {

            const size_t uch = size_t (*lo);

            _rw_check_char (*this, mf_toupper, uch);

            *dst = char (uch);
        }
    }

    return lo;
}

#endif   // _RWSTD_NO_WCHAR_T

/**************************************************************************/

//_RWSTD_NAMESPACE (std) {
#undef std
namespace std {

native_std::locale::id
ctype<UserChar>::
id;


ctype<UserChar>::
ctype (size_t refs /* = 0 */)
    : Base (refs), UserCtypeBase ("UserChar")
{
    masks_ = _rw_char_masks;
    chars_ = 0;
}


ctype<UserChar>::
ctype (const int *chars, const int *masks, size_t refs /* = 0 */)
    : Base (refs), UserCtypeBase ("UserChar")
{
    if (0 == masks) {
        // when masks is null so must chars
        RW_ASSERT (0 == chars);
        masks = _rw_char_masks;
    }

    chars_ = chars;
    masks_ = masks;
}


bool ctype<UserChar>::
do_is (mask m, char_type ch) const
{
    _rw_funcall (*this, mf_is);

    const size_t uch = size_t (ch.c);

    _rw_check_char (*this, mf_toupper, uch);

    if (masks_ == _rw_char_masks) {
        return uch < 256 ? 0 != (masks_ [uch] & m) : false;
    }

    for (size_t i = 0; 0 <= chars_ [i]; ++i)
        if (chars_ [i] == int (uch))
            return 0 != (masks_ [i] & m);

    return false;
}


const ctype<UserChar>::char_type*
ctype<UserChar>::
do_is (const char_type *lo, const char_type *hi, mask *vec) const
{
    _rw_funcall (*this, mf_is_range);

    if (masks_ == _rw_char_masks) {
        for ( ; lo < hi; ++lo, ++vec) {

            const size_t uch = size_t (lo->c);

            _rw_check_char (*this, mf_toupper, uch);

            *vec = mask (uch < 256 ? masks_ [uch] : 0);
        }
    }
    else {
        for ( ; lo < hi; ++lo, ++vec) {

            _rw_check_char (*this, mf_toupper, size_t (lo->c));

            bool found = false;

            for (size_t i = 0; 0 <= chars_ [i]; ++i) {
                if (chars_ [i] == int (UChar (lo->c))) {
                    *vec  = mask (masks_ [i]);
                    found = true;
                    break;
                }
            }

            if (!found && throw_on_invalid_)
                throw;
        }
    }

    return lo;
}


const ctype<UserChar>::char_type*
ctype<UserChar>::
do_scan_is (mask m, const char_type *lo, const char_type *hi) const
{
    _rw_funcall (*this, mf_scan_is);

    if (masks_ == _rw_char_masks) {
        for ( ; lo < hi; ++lo) {
            const size_t uch = size_t (lo->c);

            _rw_check_char (*this, mf_toupper, uch);

            if (uch < 256 && masks_ [uch] & m)
                break;
        }
    }
    else {
        for ( ; lo < hi; ++lo) {

            _rw_check_char (*this, mf_toupper, size_t (lo->c));

            for (size_t i = 0; 0 <= chars_ [i]; ++i) {
                if (chars_ [i] == int (UChar (lo->c)) && masks_ [i] & m)
                    return lo;
            }
        }
    }

    return lo;
}


const ctype<UserChar>::char_type*
ctype<UserChar>::
do_scan_not (mask m, const char_type *lo, const char_type *hi) const
{
    _rw_funcall (*this, mf_scan_not);

    if (masks_ == _rw_char_masks) {
        for ( ; lo < hi; ++lo) {

            const size_t uch = size_t (lo->c);

            _rw_check_char (*this, mf_toupper, uch);

            if (uch < 256 && 0 == (masks_ [uch] & m))
                break;
        }
    }
    else {
        for ( ; lo < hi; ++lo) {

            const size_t uch = size_t (lo->c);

            _rw_check_char (*this, mf_toupper, uch);

            for (size_t i = 0; 0 <= chars_ [i]; ++i) {
                if (chars_ [i] == int (uch) && 0 == (masks_ [i] & m))
                    return lo;
            }
        }
    }

    return lo;
}


ctype<UserChar>::char_type
ctype<UserChar>::
do_toupper (char_type ch) const
{
    _rw_funcall (*this, mf_toupper);

    const size_t uch = size_t (ch.c);

    _rw_check_char (*this, mf_toupper, uch);

    if (0 == upper_) {

        if (uch < 256 && masks_ [uch] & LOWER) {
#if 'A' == 0x41   // ASCII
            ch.c = UChar (uch - 32);
#else   // EBCDIC
            ch.c = UChar (uch + 64);
#endif
        }
    }
    else {
        // FIXME: implement this
        RW_ASSERT (!"do_toupper() not implemented");
    }

    return ch;
}


const ctype<UserChar>::char_type*
ctype<UserChar>::
do_toupper (char_type *lo, const char_type *hi) const
{
    _rw_funcall (*this, mf_toupper_range);

    if (0 == upper_) {
        for ( ; lo < hi; ++lo) {
            const size_t uch = size_t (lo->c);

            _rw_check_char (*this, mf_toupper, uch);

            if (uch < 256 && masks_ [uch] & LOWER) {
#if 'A' == 0x41   // ASCII
                lo->c = UChar (uch - 32);
#else   // EBCDIC
                lo->c = UChar (uch + 64);
#endif
            }
        }
    }
    else {
        // FIXME: implement this
        RW_ASSERT (!"do_toupper() not implemented");
    }

    return lo;
}


ctype<UserChar>::char_type
ctype<UserChar>::
do_tolower (char_type ch) const
{
    _rw_funcall (*this, mf_tolower);

    const size_t uch = size_t (ch.c);

    _rw_check_char (*this, mf_toupper, uch);

    if (masks_ == _rw_char_masks) {
        if (uch < 256 && masks_ [uch] & UPPER) {
#if 'A' == 0x41   // ASCII
            ch.c = UChar (uch + 32);
#else   // EBCDIC
            ch.c = UChar (uch - 64);
#endif
        }
    }
    else {
        // FIXME: implement this
        RW_ASSERT (!"do_toupper() not implemented");
    }

    return ch;
}


const ctype<UserChar>::char_type*
ctype<UserChar>::
do_tolower (char_type *lo, const char_type *hi) const
{
    _rw_funcall (*this, mf_tolower_range);

    if (masks_ == _rw_char_masks) {
        for ( ; lo < hi; ++lo) {

            const size_t uch = size_t (lo->c);

            if (uch < 256 && masks_ [uch] & UPPER) {
#if 'A' == 0x41   // ASCII
                lo->c = UChar (uch - 32);
#else   // EBCDIC
                lo->c = UChar (uch + 64);
#endif
            }
        }
    }
    else {
        // FIXME: implement this
        RW_ASSERT (!"do_toupper() not implemented");
    }

    return lo;
}


ctype<UserChar>::char_type
ctype<UserChar>::
do_widen (char ch) const
{
    _rw_funcall (*this, mf_widen);

    const UChar uch = UChar (ch);

    _rw_check_char (*this, mf_toupper, uch);

    if (narrow_ && wide_) {
        for (size_t i = 0; 0 <= narrow_ [i]; ++i) {
            if (int (uch) == narrow_ [i]) {
                if (0 <= wide_ [i])
                    return make_char (char (wide_ [i]), (UserChar*)0);

                if (throw_on_invalid_)
                    throw;

                break;
            }
        }
    }

    return make_char (ch, (UserChar*)0);
}


const char*
ctype<UserChar>::
do_widen (const char *lo, const char *hi, char_type *dst) const
{
    _rw_funcall (*this, mf_widen_range);

    if (narrow_ && wide_) {
        for ( ; lo < hi; ++lo, ++dst) {

            const size_t uch = size_t (*lo);

            _rw_check_char (*this, mf_toupper, uch);

            for (size_t i = 0; 0 <= narrow_ [i]; ++i) {
                if (int (uch) == narrow_ [i]) {
                    if (0 <= wide_ [i])
                        *dst = make_char (char (wide_ [i]), (UserChar*)0);

                    if (throw_on_invalid_)
                        throw;

                    break;
                }
            }
        }
    }
    else {
        for (; lo < hi; ++lo, ++dst) {

            const UChar uch = UChar (*lo);

            _rw_check_char (*this, mf_toupper, uch);

            *dst = make_char (*lo, (UserChar*)0);
        }
    }

    return lo;
}


char ctype<UserChar>::
do_narrow (char_type ch, char dfault) const
{
    _rw_funcall (*this, mf_narrow);

    const size_t uch = size_t (ch.c);

    _rw_check_char (*this, mf_toupper, uch);

    if (narrow_ && wide_) {
        for (size_t i = 0; 0 <= wide_ [i]; ++i) {
            if (int (uch) == wide_ [i]) {
                if (0 <= wide_ [i])
                    return char (wide_ [i]);

                if (throw_on_invalid_)
                    throw;

                break;
            }
        }

        return dfault;
    }

    return char (uch);
}


const ctype<UserChar>::char_type*
ctype<UserChar>::
do_narrow (const char_type *lo, const char_type *hi, char dflt, char *dst) const
{
    _rw_funcall (*this, mf_narrow_range);

    if (narrow_ && wide_) {
        for ( ; lo < hi; ++lo, ++dst) {

            const size_t uch = size_t (lo->c);

            _rw_check_char (*this, mf_toupper, uch);

            for (size_t i = 0; 0 <= wide_ [i]; ++i) {
                if (int (uch) == wide_ [i]) {
                    *dst = 0 <= narrow_ [i] ? char (wide_ [i]) : dflt;
                    break;
                }
            }
        }
    }
    else {
        for (; lo < hi; ++lo, ++dst) {

            const size_t uch = size_t (lo->c);

            _rw_check_char (*this, mf_toupper, uch);

            *dst = char (uch);
        }
    }

    return lo;
}

}   // namespace std
#define std bsl
/**************************************************************************/

UserCtype<UserChar>::
UserCtype (size_t refs /* = 0 */)
    : Base (refs)
{
    chars_ = 0;
    masks_ = _rw_char_masks;
}


UserCtype<UserChar>::
UserCtype (const int *chars, const int *masks, size_t refs)
    : Base (refs)
{
    if (0 == masks) {
        // when masks is null so must chars
        RW_ASSERT (0 == chars);
        masks = _rw_char_masks;
    }

    chars_ = chars;
    masks_ = masks;
}
