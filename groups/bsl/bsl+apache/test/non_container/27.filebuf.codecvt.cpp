/************************************************************************
 *
 * codecvt.cpp - test exercising file streams and code conversion
 *
 * $Id: 27.filebuf.codecvt.cpp 590052 2007-10-30 12:44:14Z faridz $
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
 * Copyright 2001-2006 Rogue Wave Software.
 * 
 **************************************************************************/

// PROBLEM DESCRIPTION:
//   basic_ofstream<>::overflow() mangles a multibyte sequence when using
//   code conversion. This functionality of the class isn't currently (as
//   of the date of the creation of the test) being exercised by our
//   testsuite due to the lack of a suitable locale (e.g., ja_JP).

// TEST DESCRIPTION:
//   test creates a temporary file and fills it with series of pairs
//   <offset, seq>, where `offset' is the offset from the beginning of
//   the file to the first (decimal) digit of offset, and `seq' is
//   a character in the range [1, CHAR_MAX] possibly converted to an
//   escape or trigraph sequence according to the rules described
//   in 2.3 and 2.13.2
//
//   the program then exercises the stream's (actually, the stream
//   file buffer's) ability to extract and seek within such a file
//   thus testing the stream's ability to crrectly interact with
//   the codecvt facet installed in the imbued locale


#include <rw/_defs.h> 
#if defined (__IBMCPP__) && !defined (_RWSTD_NO_IMPLICIT_INCLUSION)
   // disable implicit inclusion to work around a limitation
   // in IBM VisualAge
#  define _RWSTD_NO_IMPLICIT_INCLUSION 
#endif 
 

#include <fstream>
#include <sstream>

#include <climits>   // for UCHAR_MAX
#include <cstdio>    // for fclose(), fopen(), fseek(), size_t
#include <cstdlib>   // for abort()
#include <cstring>   // for memset(), strcmp(), strlen()
#include <cwchar>    // for mbstate_t

#include <driver.h>
#include <file.h>

/**************************************************************************/

// code conversion facet suitable for replacement of the default
// codecvt<char, char, mbstate_t> facet
// cformat::do_out() converts printable ASCII characters into themselves,
// control characters are converted to standard C escape sequences
// cformat::do_in() reverses the effect of do_out()
class cformat: public std::codecvt <char, char, std::mbstate_t>
{
    enum {
        new_line         = 0x0001,  // convert '\n' to "\n"
        horizontal_tab   = 0x0002,  // convert '\t' to "\t"
        vertical_tab     = 0x0004,  // convert '\v' to "\v"
        backspace        = 0x0008,  // convert '\b' to "\b"
        carriage_return  = 0x0010,  // convert '\r' to "\r"
        form_feed        = 0x0020,  // convert '\f' to "\f"
        alert            = 0x0040,  // convert '\a' to "\a"
        backslash        = 0x0080,  // convert '\\' to "\\"
        question_mark    = 0x0100,  // convert '?' to "\?"
        single_quote     = 0x0200,  // convert '\\'' to "\'"
        double_quote     = 0x0400,  // convert '"' to "\""
        trigraphs        = 0x8000,  // convert to/from trigrap sequences
        hex              = 0x1000   // hex notation in external representation
    };

    const int mask;   // bitmaps of flags above

public:
    explicit
    cformat (std::size_t ref = 0, int m = 0)
        : std::codecvt<char, char, std::mbstate_t> (ref),
          mask (m) { /* empty */ }

protected:

    virtual result
    do_out (state_type&, 
            const intern_type*, const intern_type*, 
            const intern_type*&, 
            extern_type*, extern_type*, extern_type*&) const;

    virtual result
    do_in (state_type&, const extern_type*, 
           const extern_type*, const extern_type*&, 
           intern_type*, intern_type*, intern_type*&) const;

    virtual result
    do_unshift (state_type&, extern_type*, 
                extern_type*, extern_type*&) const {
        // stateless encoding, no conversion necessary
        return noconv;
    }

    virtual int
    do_encoding () const _THROWS (()) {
        return 0;   // variable number of external chars per single internal
    }

    virtual bool
    do_always_noconv () const _THROWS (()) {
        return false;   // conversion always necessary
    }

    // returns the maximum `N' of extern chars in the range [from, from_end)
    // such that N represents max or fewer internal chars
    virtual int
    do_length (state_type&, const extern_type*, 
               const extern_type*, std::size_t) const;

    // returns the max value do_length (s, from, from_end, 1) can return
    // for any valid range [from, from_end) - see LWG issue 74 (a DR)
    virtual int
    do_max_length () const _THROWS (()) {
        // assume that an internal char occupies at most 4 external chars
        // this won't hold for e.g. '\x00001' etc., but will hold for all
        // chars in the hex notation of up to two digits and all chars in
        // octal notation (which are required to fit in 4 by the standard)
        return 4;
    }
};


std::codecvt_base::result
cformat::do_out (      state_type& /* unused */,
                 const intern_type *from,
                 const intern_type *from_end,
                 const intern_type *&from_next, 
                       extern_type *to,
                       extern_type *to_end,
                       extern_type *&to_next) const
{
    // assert 22.2.1.5.2, p1 preconditions
    rw_assert (from <= from_end, __FILE__, __LINE__,
               "codecvt::do_out (..., from = %#p, from + %d, %#p, "
               "to = %#p, to + %d, %#p): from <= from_end",
               from, from_end - from, from_next, to, to_end - to, to_next);

    rw_assert (to <= to_end, __FILE__, __LINE__,
               "codecvt::do_out (..., from = %#p, from + %d, %#p, "
               "to = %#p, to + %d, %#p): to <= to_end",
               from, from_end - from, from_next, to, to_end - to, to_next);

    // assume no conversion will be performed
    result res = noconv;

    for (from_next = from, to_next = to; from_next != from_end; ++from_next) {

        // out of space
        if (to_next == to_end) {
            res = partial;
            break;
        }

        // convert to unsigned to make sure comparison works
        unsigned char ch = *from_next;
        extern_type esc  = extern_type ();

        if (ch < ' ') {

            // convert to a C escape sequence
            switch (ch) {

            case '\a':
                if (!(mask & alert)) {
                    ch  = 'a';
                    esc = '\\';
                }
                break;

            case '\b':
                if (!(mask & backspace)) {
                    ch  = 'b';
                    esc = '\\';
                }
                break;

            case '\t':
                if (!(mask & horizontal_tab)) {
                    ch  = 't';
                    esc = '\\';
                }    
                break;

            case '\n':
                if (!(mask & new_line)) {
                    ch  = 'n';
                    esc = '\\';
                }
                break;

            case '\v':
                if (!(mask & vertical_tab)) {
                    ch  = 'v';
                    esc = '\\';
                }
                break;

            case '\f':
                if (!(mask & form_feed)) {
                    ch  = 'f';
                    esc = '\\';
                }
                break;

            case '\r':
                if (!(mask & carriage_return)) {
                    ch = 'r';
                    esc = '\\';
                }
                break;

            case '\\':
                if (!(mask & backslash)) {
                    ch  = '\\';
                    esc = '\\';
                }
                break;

            default:
                esc = '\\';
                break;
            }
        }
        else if (ch > '~') {
            // convert to a C escape sequence (octal)
            esc = '\\';
        }
        else {
            // escape special characters
            switch (ch) {
            case '?':
                if (!(mask & question_mark))
                    esc = '\\';
                break;

            case '\'':
                if (!(mask & single_quote))
                    esc = '\\';
                break;

            case '"':
                if (!(mask & double_quote))
                    esc = '\\';
                break;

            case '\\':
                if (!(mask & backslash))
                    esc = '\\';
                break;
            }

            if (!(mask & trigraphs)) {

                // convert to a trigraph sequence
                switch (ch) {
                case '#':  ch  = '=';  esc = '?'; break;
                case '\\': ch  = '/';  esc = '?'; break;
                case '^':  ch  = '\''; esc = '?'; break;
                case '[':  ch  = '(';  esc = '?'; break;
                case ']':  ch  = ')';  esc = '?'; break;
                case '|':  ch  = '!';  esc = '?'; break;
                case '{':  ch  = '<';  esc = '?'; break;
                case '}':  ch  = '>';  esc = '?'; break;
                case '~':  ch  = '-';  esc = '?'; break;
                }
            }
        }

        // process `ch' and `esc'

        if ('\\' == esc) {

            // conversion was performed
            res = ok;

            if (ch < ' ' || ch > '~') {

                // need room for an escape followed by three ocal digits
                if (4 > to_end - to_next) {
                    res = partial;
                    break;
                }

                static const char digits[] = "0123456789abcdef";

                // add an escape character
                *to_next++ = esc;

                if (mask & hex) {
                    // add hex representation (exactly three chars)
                    *to_next++ = 'x';
                    *to_next++ = digits [(ch & 0xf0) >> 4];
                    *to_next++ = digits [ch & 0xf];
                }
                else {
                    // add octal representation (exactly three digits)
                    *to_next++ = digits [(ch & (7 << 6)) >> 6];
                    *to_next++ = digits [(ch & (7 << 3)) >> 3];
                    *to_next++ = digits [ch & 7];
                }
            }
            else {
                // need room for an escape followed by a single char 
                if (2 > to_end - to_next) {
                    res = partial;
                    break;
                }

                // add an escape char followed by the escaped char
                *to_next++ = esc;
                *to_next++ = ch;
            }
        }
        else if ('?' == esc) {
            // need room for a trigraph sequence
            if (3 > to_end - to_next) {
                res = partial;
                break;
            }

            // conversion was performed
            res = ok;

            // add a trigraph sequence
            *to_next++ = '?';
            *to_next++ = '?';
            *to_next++ = ch;
        }
        else {
            // not escaped
            *to_next++ = ch;
        }
    }

    if (noconv == res) {
        // 22.2.1.5.2, p2, Note: no conversion was necessary
        from_next = from;
        to_next   = to;
    }

    rw_fatal (from_next >= from && from_next <= from_end, 0, __LINE__,
              "user-defined codecvt: internal inconsistency");

    rw_fatal (to_next >= to && to_next <= to_end, 0, __LINE__,
              "user-defined codecvt: internal inconsistency");

    return res;
}


std::codecvt_base::result
cformat::do_in (      state_type& /* unused */,
                const extern_type *from,
                const extern_type *from_end,
                const extern_type *&from_next, 
                      intern_type *to,
                      intern_type *to_end,
                      intern_type *&to_next) const
{
    // assert 22.2.1.5.2, p1 preconditions
    rw_assert (from <= from_end, __FILE__, __LINE__,
               "codecvt::do_in (..., from = %#p, from + %d, %#p, "
               "to = %#p, to + %d, %#p): from <= from_end",
               from, from_end - from, from_next, to, to_end - to, to_next);

    rw_assert (to <= to_end, __FILE__, __LINE__,
               "codecvt::do_in (..., from = %#p, from + %d, %#p, "
               "to = %#p, to + %d, %#p) to <= to_end",
               from, from_end - from, from_next, to, to_end - to, to_next);

    result res = ok;

    for (from_next = from, to_next = to; from_next != from_end; ++from_next) {

        unsigned char ch = *from_next;
        intern_type c    = intern_type ();

        if ('\\' == ch) {

            if (2 > from_end - from_next) {
                // ok is the correct value to return in this case,
                // but partial should be handled as well for robustness
                res = (from_end - (extern_type*)0) % 2 ? ok : partial;
                break;
            }

            ch = from_next [1];

            if ('x' == ch) {
                // interpret a hex escape sequence

                // advance past '\x'
                const extern_type *next = from_next + 2;

                // parse hex digits until a non-hex digits is encountered
                for (; ; ++next) {

                    if (next == from_end) {
                        // do not advance to the end since there may be 
                        // more digits following it (e.g., '\x012' with
                        // from_end pointing at '1' or '2')
                        return partial;
                    }

                    ch = *next;
                    if (ch >= '0' && ch <= '9')
                        c = (c << 4) | (ch - '0');
                    else if (ch >= 'a' && ch <= 'f')
                        c = (c << 4) | (ch - 'a' + 10);
                    else if (ch >= 'A' && ch <= 'F')
                        c = (c << 4) | (ch - 'A' + 10);
                    else if (next - from_next > 2)
                        break;
                    else {
                        return error;   // non-hex digit immediately after '\x'
                    }
                }

                // advance to the end of parsed number
                from_next = next - 1;
            }
            else if ('0' <= ch && '7' >= ch) {
                // interpret a oct escape sequence

                // (tentatively) advance past '\'
                const extern_type *next = from_next + 1;

                // parse at most three oct digits
                for (; next - from_next < 4; ++next) {

                    if (next == from_end) {
                        // do not advance to the end since there may be 
                        // more digits following it (e.g., '\x012' with
                        // from_end pointing at '1' or '2')
                        return partial;
                    }

                    ch = *next;
                    if (ch >= '0' && ch <= '7')
                        c = (c << 3) | (ch - '0');
                    else if (next - from_next)
                        break;
                    else {
                        // advance to the offending char
                        from_next = next;
                        return error;   // non-oct digit immediately after '\'
                    }
                }

                // advance to the end of parsed number
                from_next = next - 1;
            }
            else {
                // interpret standard C escape sequence
                switch (ch) {
                case 'a': c = '\a'; break;
                case 'b': c = '\b'; break;
                case 't': c = '\t'; break;
                case 'n': c = '\n'; break;
                case 'v': c = '\v'; break;
                case 'f': c = '\f'; break;
                case 'r': c = '\r'; break;

                // optional but allowed and escaped backslash
                case '?': case '"': case '\'': case '\\': c = ch ; break;

                // bad escape sequence                    
                default: return error;
                }

                // advance past the initial '\'
                ++from_next;
            }
        }
        else if ('?' == ch && !(mask & trigraphs)) {
            // (try to) convert a trigraph sequence
            if (   2 > from_end - from_next
                || '?' == from_next [1] && 3 > from_end - from_next) {
                res = partial;
                break;
            }

            if ('?' == from_next [1]) {

                // "??" (potentilly) introduces a trigraph sequence
                switch (from_next [2]) {

                case '=':  c = '#';  break;
                case '/':  c = '\\'; break;
                case '\'': c = '^';  break;
                case '(':  c = '[';  break;
                case ')':  c = ']';  break;
                case '!':  c = '|';  break;
                case '<':  c = '{';  break;
                case '>':  c = '}';  break;
                case '-':  c = '~';  break;

                default:
                    // no a trigraph sequence, won't convert
                    c = from_next [0];   // i.e., '?'
                }

                // skip the leading "??" of a trigraph sequence
                if (c != from_next [0])
                    from_next += 2;
            }
            else
                // ordinary (not escaped) character
                c = ch;
        }
        else
            // ordinary (not escaped) character
            c = ch;

        // to_next may be 0 (when called from do_length())
        // doing pointer math on invalid pointers (null) has undefined behavior
        // but will probably work in most cases
        if (to_next)
            *to_next = c;

        ++to_next;

        // in case of of the inner loops has reached end
        if (from_next == from_end)
            break;
    }
    
    rw_fatal (from_next >= from && from_next <= from_end, 0, __LINE__,
              "user-defined codecvt: internal inconsistency");

    rw_fatal (to_next >= to && (to_next <= to_end || !to_end), 0, __LINE__,
              "user-defined codecvt: internal inconsistency");

    return res;
}


int
cformat::do_length (state_type&,
                    const extern_type *from, 
                    const extern_type *from_end,
                    std::size_t        max) const
{
    const extern_type *from_next;
    intern_type *to_next = 0;

    std::mbstate_t st;
    std::memset (&st, 0, sizeof st);

    // use do_in() with `to' of 0 to do the computation
    // doing pointer math on invalid pointers (null) has undefined behavior
    // but will probably work in most cases
    do_in (st, from, from_end, from_next,
           to_next, to_next + max, to_next);

    return to_next - (intern_type*)0;
}

/***********************************************************************/

// determines file size in bytes
static std::streamsize
fsize (const char *fname)
{
    std::FILE* const f = std::fopen (fname, "r");
    if (!f || std::fseek (f, 0, SEEK_END))
        return -1;

    const std::streamsize size = std::ftell (f);

    std::fclose (f);

    return size;
}

/***********************************************************************/

static void
self_test ()
{
    rw_info (0,  __FILE__, __LINE__,
             "user-defined codecvt facet -- self test");

    static const char* const result[] = {
        "ok", "partial", "error", "noconv"
    };

    // user-defined code conversion facet
    cformat fmt (1);

    // original array of internal characters and one to which to convert
    // an external representation back to (for comparison)
    cformat::intern_type intrn [2][256] = { { '\0' } };

    // array of external chars large enough to hold the internal array
    // each internal char converts to at most 4 external chars
    cformat::extern_type extrn [1024] = { '\0' };

    // fill internal array with chars from '\1' to '\377'
    for (std::size_t i = 0; i != sizeof intrn [0] - 1; ++i)
        intrn [0][i] = cformat::intern_type (i + 1);

    const cformat::intern_type *intrn_next_0 = 0;
          cformat::intern_type *intrn_next_1 = 0;

          cformat::extern_type *extrn_next   = 0;
    

    // dummy (state not used, conversion is stateless)
    std::mbstate_t st;
    std::memset (&st, 0, sizeof st);

    // convert internal to external representation, substituting
    // escape sequences for non-printable characters
    std::codecvt_base::result res;

    // convert array in internal representation to external representation
    res = fmt.out (st,
                   intrn [0], intrn [0] + sizeof intrn [0], intrn_next_0,
                   extrn, extrn + sizeof extrn, extrn_next);

    rw_assert (std::codecvt_base::ok == res, 0, __LINE__,
               "codecvt::out (); result == codecvt_base::ok, "
               "got codecvt_base::%s", result [res]);

    // assert that the external sequence is longer than the internal one
    rw_assert (extrn_next - extrn > intrn_next_0 - intrn [0], 0, __LINE__,
               "codecvt::out (); converted size %d, expected > %d",
               extrn_next - extrn, intrn_next_0 - intrn [0]);

    // convert external to internal representation, parsing
    // multi-char escape sequences into single chars

    const cformat::intern_type *next = extrn;

    intrn_next_1 = intrn [1];

    for (; next != extrn_next; ) {

        // allow only a small buffer space to exercise partial conversion
        std::size_t step = std::size_t (extrn_next - next);
        if (step > 12)
            step = 5 + step % 8;

        res = fmt.in (st,
                      next, next + step, next,
                      intrn_next_1, intrn [1] + sizeof intrn [1], intrn_next_1);

        if (std::codecvt_base::error == res)
            break;
    }

    // assert that entrire sequence converted ok
    rw_assert (std::codecvt_base::ok == res, 0, __LINE__,
               "codecvt::in (); result == codecvt_base::ok, got "
               "codecvt_base::%s at offset %d", result [res], next - extrn);

    rw_assert (intrn_next_1 == intrn [1] + sizeof intrn [1], 0, __LINE__,
               "codecvt::in (); to_next == %#p, got %#p",
               intrn [1] + sizeof intrn [1], intrn_next_1);

    rw_assert (0 == std::strcmp (intrn [0], intrn [1]), 0, __LINE__,
               "codecvt<>::out/in ()");
}

/***********************************************************************/

#define RW_ASSERT_STATE(strm, state)                            \
        rw_assert ((strm).rdstate () == (state), 0, __LINE__,   \
                   "rdstate () == %{Is}, got %{Is}",            \
                   (state), (strm).rdstate ())

static void
test_noconv (const char *fname)
{
    rw_info (0, 0, __LINE__, "ifstream extraction without conversion");

    std::ifstream f (fname);

    // make sure file stream has been successfully opened
    RW_ASSERT_STATE (f, std::ios::goodbit);

    // gain public access to protected members
    struct pubbuf: std::streambuf {

        // working around an MSVC 6.0 bug (PR #26330)
        typedef std::streambuf Base;

        virtual std::streamsize showmanyc () {
            return Base::showmanyc ();
        }
        char* pubgptr () {
            return Base::gptr ();
        }
        char* pubegptr () {
            return Base::egptr ();
        }
    };

    // use static cast through void* to avoid using reinterpret_cast
    pubbuf *rdbuf = _RWSTD_STATIC_CAST (pubbuf*, (void*)f.rdbuf ());

    std::streamsize filesize = rdbuf->showmanyc ();

    // exercise 27.5.2.4.3, p1: showmanyc() returns the "estimated"
    // size of the sequence (i.e., the file size in this case)
    rw_assert (filesize == fsize (fname), 0, __LINE__,
               "streambuf::showmanyc () == %ld, got %ld",
               fsize (fname), filesize);
    
    // exercise 27.5.2.2.3, p1
    filesize = f.rdbuf ()->in_avail ();

    const char *gptr  = rdbuf->pubgptr ();
    const char *egptr = rdbuf->pubgptr ();

    rw_assert (filesize == (gptr < egptr ? egptr - gptr : fsize (fname)),
               0, __LINE__,
               "streambuf::in_avail () == %ld, got %ld",
               (gptr < egptr ? egptr - gptr : fsize (fname)), filesize);

    // allocate buffer large enough to accomodate the converted
    // (i.e. internal) sequence
    const std::size_t bufsize = 0x10000;     // 64k should do it
    char *tmpbuf = new char [bufsize];

    // fill with non-0 value to check for writes past the end
    // (see also Onyx incident 14033)
    std::memset (tmpbuf, '\x7f', bufsize);

    // ecercise putback area
    std::streamsize i;

#ifndef _RWSTD_PBACK_SIZE
#  define _RWSTD_PBACK_SIZE 1
#endif   // _RWSTD_PBACK_SIZE

    // _RWSTD_PBACK_SIZE is the size of the putback area the library
    // was configured with; the macro expands to streamsize (N)
    for (i = 0; i != _RWSTD_PBACK_SIZE + 1; ++i) {
        // read a few characters, read must not append a '\0'
        std::streamsize n = f.read (tmpbuf, i).gcount ();

        // assert that read exactly `i' chars, buffer not null-terminared
        rw_assert (i == n && '\x7f' == tmpbuf [i], 0, __LINE__,
                   "ifstream::read (%#p, %ld) read %ld,"
                   "buffer terminated with '\\%03o'",
                   tmpbuf, i, n, tmpbuf [i]);

        // put back read characters; assert that they are the same
        // as those in the corresponding positions in the buffer
        for (std::streamsize j = 0; j != i; ++j) {
            std::ifstream::int_type c = f.rdbuf ()->sungetc ();

            typedef std::ifstream::traits_type Traits;

            rw_assert (Traits::to_int_type (tmpbuf [n - j - 1]) == c,
                       0, __LINE__,
                       "filebuf::sungetc() == '\\%03o', got '\\%03o",
                       tmpbuf [n - j - 1], c);
        }

        // re-read characters just put back
        char buf [_RWSTD_PBACK_SIZE + 1];
        std::memset (buf, '\x7f', sizeof buf);

        std::streamsize n2 = f.read (buf, i).gcount ();

        // assert that the requested number of chars were read in
        rw_assert (i == n2 && '\x7f' == buf [i], 0, __LINE__,
                   "ifstream::read (%#p, %ld) read %ld,"
                   "buffer terminated with '\\%03o'",
                   buf, i, n2, buf [i]);

        // assert that the read chars are those that were put back
        for (std::streamsize k = 0; k != i; ++k) {
            rw_assert (buf [k] == tmpbuf [k], 0, __LINE__,
                       "buffer mismatch at offset %ld: got '\\%03o', "
                       "expected '\\%03o'", k, buf [k], tmpbuf [k]);
            // put character back again so that it can be read back in
            f.rdbuf ()->sungetc ();
        }
    }

    // read file contents into buffer (apply no conversion)
    const std::streamsize n = f.read (tmpbuf, bufsize).gcount ();

    // 27.6.1.3, p28 - read() sets eofbit | failbit
    // if end-of-file occurs on the input sequence
    RW_ASSERT_STATE (f, std::ios::eofbit | std::ios::failbit);

    // assert that the entiire file has been read
    rw_assert (n == filesize && '\x7f' == tmpbuf [n], 0, __LINE__,
               "ifstream::read (%#p, %ld) read %ld, expected %ld; "
               "buffer terminated with '\\%03o' [%s]",
               tmpbuf, bufsize, n, filesize, tmpbuf [n], fname);

    tmpbuf [n] = '\0';

    // assert that file contains no control characters
    bool b = true;
    for (i = 0; b && i != UCHAR_MAX + 1; ++i) {
        if (i >= ' ' || i <= '~')
            continue;
        b = 0 == std::strchr (tmpbuf, char (i));
    }

    rw_assert (b, 0, __LINE__,
               "unescaped non-printable character '\\#03o' at offset %ld",
                tmpbuf [i], i);

    delete[] tmpbuf;
}

/***********************************************************************/

static void
test_error (const char *fname)
{
    rw_info (0, 0, __LINE__, "ifstream extraction with a conversion error");

    const char outbuf[] = {
        "abcdefghijklmnopqrstuvwxyz\\x20\\xzzABCDEFGHIJKLMNOPQRSTUVWXYZ"
        //                                 ^
        //                                 |
        // error (invalid hex sequence) ---+
    };

    // write out a text file containing a conversion error
    std::ofstream ostrm (fname);

    ostrm << outbuf;

    ostrm.close ();

    // read the file back in using the conversion facet
    std::ifstream istrm (fname);

    // user-defined code conversion facet
    FACET_CONST cformat fmt (1 /* prevent locale from deleting */);

    // create a locale by combining the classic locale and our UD facet
    // cformat; the facet will replace std::codecvt<char, char, mbstate_t>
    std::locale l (std::locale::classic (), &fmt);

    // imbue locale with formatting facet into streams and save previous
    l = istrm.imbue (l);

    char inbuf [sizeof outbuf * 4] = { 0 };

    // try to read partial contents of the file
    // including the conversion error into the buffer
    istrm.read (inbuf, 26L + 4L /* "a..z" <space> <error> "AB" */);

    // verify that the operation failed, eofbit is set since
    // less than the requested number of characters have been read
    RW_ASSERT_STATE (istrm, std::ios::eofbit | std::ios::failbit);

    // verify that the 26 characters 'a' through 'z' plus
    // the space (i.e., 27 chars) have been extracted
    rw_assert (27 == istrm.gcount (), 0, __LINE__,
               "ifstream::read () extracted %d, expected 27",
               istrm.gcount ());

    rw_assert (   0 == std::ifstream::traits_type::compare (inbuf, outbuf, 26)
               && ' ' == inbuf [26], 0, __LINE__,
               "ifstream::read () got \"%s\", expected \"%.26s \"",
               inbuf, outbuf);

    istrm.clear ();

    // try to read again, and verify that the operation fails
    istrm.read (inbuf, sizeof inbuf);

    RW_ASSERT_STATE (istrm, std::ios::eofbit | std::ios::failbit);

    rw_assert (0 == istrm.gcount (), 0, __LINE__,
               "ifstream::read () extracted %d, expected 0",
               istrm.gcount ());

    // replace the imbued locale
    istrm.imbue (l);
}

/***********************************************************************/

static void
test_seek (const char *fname,
           std::size_t bufsize = std::size_t (-1))
{
    std::ifstream f (fname);

    // make sure stream has been successfully opened
    RW_ASSERT_STATE (f, std::ios::goodbit);

    // set buffer size if specified
    if (std::size_t (-1) != bufsize) {

        rw_info (0, 0, __LINE__,
                 "ifstream::seekg()/tellg() - %zu byte buffer", bufsize);

        RW_ASSERT_STATE (f, std::ios::goodbit);
        f.rdbuf ()->pubsetbuf (0, bufsize);
    }
    else {
        rw_info (0, 0, __LINE__,
                 "ifstream::seekg()/tellg() - default buffer size");
    }

    // user-defined code conversion facet
    FACET_CONST cformat fmt (1 /* prevent locale from deleting */);

    // create a locale by combining the classic locale and our UD facet
    // cformat; the facet will replace std::codecvt<char, char, mbstate_t>
    std::locale l (std::locale::classic (), &fmt);

    // imbue locale with formatting facet into streams and save previous
    l = f.imbue (l);

    // seek to the beginning of stream (safe)
    f.seekg (0);
    RW_ASSERT_STATE (f, std::ios::goodbit);

    const unsigned char max = UCHAR_MAX - '~';

    for (std::size_t n = 0; n != std::size_t (max - 1); ++n) {

        const char delim = char ('~' + n);

        // skip over chars until the terminating delim (and extract it)
        f.ignore (0x10000, std::fstream::traits_type::to_int_type (delim));
        rw_assert (f.good (), 0, __LINE__,
                   "istream::ignore (0x10000, '\\%03o'); "
                   "rdstate() = %{Is}, gcount() = %ld",
                   delim, f.rdstate (), f.gcount ());

        // alternate between exercising seekg() and pubsync()
        if (n % 2) {
            // seek inplace (offset must be 0 for MB encodings)
            f.seekg (0, std::ios::cur);
            RW_ASSERT_STATE (f, std::ios::goodbit);
        }
        else {
            // filebuf::pubsync() must return 0
            int syn = f.rdbuf ()->pubsync ();
            rw_assert (0 == syn, 0, __LINE__,
                       "filebuf::pubsync () == 0, got %d", syn);
        }

        // skip exactly one char forward (retrieve a space)
        char c = char ();
        f.get (c);
        RW_ASSERT_STATE (f, std::ios::goodbit);
        rw_assert (' ' == c, 0, __LINE__,
                   "istream::get(char_type) got '\\%03o', expected ' '", c);

        // get current file position
        const std::ifstream::pos_type pos = f.tellg ();
        RW_ASSERT_STATE (f, std::ios::goodbit);

        // extract offset - should be the same as pos
        long offset = 0;
        f >> offset;

        RW_ASSERT_STATE (f, std::ios::goodbit);

        rw_assert (long (pos) == offset, 0, __LINE__,
                   "ifstream::operator>>() expected %ld, got %ld",
                   long (pos), offset);

        // in_avail() must return a value > 0
        std::streamsize avail = f.rdbuf ()->in_avail ();
        rw_assert (avail > 0, 0, __LINE__,
                   "filebuf::in_avail() expected > 0, got %ld", avail);

        // "rewind" stream to the beginning
        f.seekg (0);
        RW_ASSERT_STATE (f, std::ios::goodbit);

        // try seeking to the previous position
        f.seekg (pos);
        RW_ASSERT_STATE (f, std::ios::goodbit);
        rw_assert (f.tellg () == pos, 0, __LINE__,
                   "istream::seekg (%ld); tellg () returns %ld",
                   long (pos), long (f.tellg ()));

        // re-read offset - should be the same as file pos
        f >> offset;

        RW_ASSERT_STATE (f, std::ios::goodbit);

        rw_assert (long (pos) == offset, 0, __LINE__,
                   "ifstream::operator>>() expected %ld, got %ld",
                   long (pos), offset);
    }

    // ignore the rest of file, eofbit must be set
    f.ignore (0x10000);
    RW_ASSERT_STATE (f, std::ios::eofbit);

    // in_avail() must return 0
    const std::streamsize avail = f.rdbuf ()->in_avail ();
    rw_assert (0 == avail, 0, __LINE__,
               "filebuf::in_avail() expected 0, got %ld", avail);

    // imbue original locale (currently imbued locale
    // will be destroyed prior to the destruction of `f')
    f.imbue (l);
}

/***********************************************************************/

static int
run_test (int, char*[])
{
    // self-test make sure facet works
    self_test ();

    // user-defined code conversion facet
    FACET_CONST cformat fmt (1 /* prevent locale from deleting */);

    // create a locale by combining the classic locale and our UD facet
    // cformat; the facet will replace std::codecvt<char, char, mbstate_t>
    std::locale l (std::locale::classic (), &fmt);

    const char *fname = rw_tmpnam (0);

    if (!fname)
        return 1;

    // will be populated with file offsets and escape sequences
    char buffer [4096] = { '\0' };

    int buflen = 0;

    // generate file contents using UD conversion
    if (1) {
        rw_info (0, 0, __LINE__,
                 "ofstream insertion with multibyte conversion");

        std::ofstream f (fname);

        // make sure file stream has been successfully opened
        RW_ASSERT_STATE (f, std::ios::goodbit);

        // imbue locale with formatting facet into stream
        f.imbue (l);

        for (std::size_t i = 1; i != UCHAR_MAX + 1U; ++i) {

            const std::ofstream::pos_type pos = f.tellp ();

            RW_ASSERT_STATE (f, std::ios::goodbit);

            buflen = std::strlen (buffer);

            // append the file offset followed by a (possibly escaped) char
            std::sprintf (buffer + buflen, "%ld %c ", long (pos), char (i));

            // write out the just appended portion of the buffer
            f << (buffer + buflen);

            RW_ASSERT_STATE (f, std::ios::goodbit);
        }

        buflen = std::strlen (buffer);

        // file contains the contents of buffer with non-printable
        // chars replaced with escape sequences (e.g., tabs with '\t', etc.)
    }


    // read contents of file w/o conversion
    test_noconv (fname);

    // read contents of file, apply conversion
    if (1) {
        rw_info (0, 0, __LINE__,
                 "ifstream extraction with multibyte conversion");

        std::ifstream f (fname);

        // make sure file stream has been successfully opened
        RW_ASSERT_STATE (f, std::ios::goodbit);

        // imbue locale with formatting facet into stream
        f.imbue (l);

        // allocate buffer large enough to accomodate the converted
        // (i.e. internal) sequence
        char tmpbuf [sizeof buffer];

        // read file contents into buffer, convert escape sequences
        // into the corresponding (perhaps unprintable) characters
        const std::streamsize n = f.read (tmpbuf, sizeof tmpbuf).gcount ();

        // 27.6.1.3, p28 - read() sets eofbit | failbit
        // if end-of-file occurs on the input sequence
        RW_ASSERT_STATE (f, std::ios::eofbit | std::ios::failbit);

        rw_assert (long (n) == buflen, 0, __LINE__,
                   "ifstream::read (%#p, %d); read %ld, expected %d",
                   tmpbuf, sizeof tmpbuf, long (n), buflen);

        // assert that converted file contents are the same
        // as the originally generated buffer
        const long len = long (n) < buflen ? long (n) : buflen;
        for (long i = 0; i != len; ++i) {
            if (tmpbuf [i] != buffer [i]) {
                rw_assert (0, 0, __LINE__,
                           "'\\%03o' == '\\%03o'; offset %d",
                           (unsigned char)buffer [i],
                           (unsigned char)tmpbuf [i], i);
                break;
            }
        }
    }


    // test with default buffer
    test_seek (fname);

    // retest with buffer of user-defined size
    for (std::size_t n = 4096; n != std::size_t (-1);
         n -= 1024 < n ? 1024 : 256 < n ? 256 : 16 < n ? 16 : 1)
        test_seek (fname, n);


    // test with errors during conversion
    test_error (fname);

    // remove a temporary file
    std::remove (fname);

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.filebuf",
                    0 /* no comment */,
                    run_test,
                    "", 0);
}
