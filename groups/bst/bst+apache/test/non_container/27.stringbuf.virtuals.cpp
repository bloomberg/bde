/***************************************************************************
 *
 * 27.stringbuf.virtuals.cpp - test exercising lib.stringbuf.virtuals
 *
 * $Id: 27.stringbuf.virtuals.cpp 640856 2008-03-25 15:23:51Z sebor $
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

#include <sstream>     // for stringbuf
#include <cstring>     // for size_t, strchr()

#include <cmdopt.h>    // for rw_enabled()
#include <driver.h>    // for rw_assert(), ...
#include <rw_char.h>   // for rw_expand(), rw_match()

/**************************************************************************/

#undef EOF
#define EOF   (_RWSTD_INT_MIN + 2)

#undef NOT_EOF
#define NOT_EOF   (_RWSTD_INT_MIN + 1)

#undef IGN
#define IGN   _RWSTD_INT_MIN

#undef NPOS
#define NPOS       -1   // invalid position (post_type(off_type(-1))

#define MAYBE_1     0   // 0 or more write positions available
#define AT_LEAST_1  1   // at least one write position available

/**************************************************************************/

template <class charT, class Traits>
struct PubBuf: std::basic_stringbuf<charT, Traits>
{
    typedef std::basic_stringbuf<charT> Base;
    typedef typename Base::char_type    char_type;
    typedef typename Base::int_type     int_type;
    typedef typename Base::off_type     off_type;
    typedef typename Base::pos_type     pos_type;

    char_type* Eback () const { return this->eback (); }
    char_type* Gptr () const { return this->gptr (); }
    char_type* Egptr () const { return this->egptr (); }

    void Gbump (int n) { this->gbump (n); }

    char_type* Pbase () const { return this->pbase (); }
    char_type* Pptr () const { return this->pptr (); }
    char_type* Epptr () const { return this->epptr (); }

    int_type Pbackfail () { return this->pbackfail (); }
    int_type Pbackfail (int_type c) { return this->pbackfail (c); }

    int_type Overflow () { return this->overflow (); }
    int_type Overflow (int_type c) { return this->overflow (c); }

    int_type Underflow () { return this->underflow (); }

    pos_type Seekoff (off_type off, std::ios_base::seekdir way) {
        return this->seekoff (off, way);
     }

    pos_type Seekoff (off_type off, std::ios_base::seekdir way,
                      std::ios_base::openmode which) {
        return this->seekoff (off, way, which);
     }

    pos_type Seekpos (pos_type pos) {
        return this->seekpos (pos);
    }

    pos_type Seekpos (pos_type pos, std::ios_base::openmode which) {
        return this->seekpos (pos, which);
    }

    static int capacity (int n) {
        Base buf (std::ios::out);
        while (n--)
            buf.sputc ('c');
        return ((PubBuf&)buf).Epptr () - ((PubBuf&)buf).Pbase ();
    }
};


int traits_eof = -1;

template <class charT>
struct CharTraits: std::char_traits<charT>
{
    typedef std::char_traits<charT> Base;
    typedef typename Base::int_type int_type;

    // override eof() to detect bad assumptions
    static int_type eof () { return traits_eof; }
    static int_type not_eof (int_type c) {
        return c == eof () ? int_type (!c) : c;
    }
};

/**************************************************************************/

struct VFun
{
    enum charT { Char, WChar };
    enum Traits { DefaultTraits, UserTraits };
    enum VirtualTag {
        // which virtual function to exercise
        xsputn, pbackfail, overflow, underflow, seekoff, seekpos
    };

    VFun (charT cid, const char *cname,
          Traits tid, const char *tname)
        : cid_ (cid), tid_ (tid), vfun_ (),
          cname_ (cname), tname_ (tname), fname_ (0),
          strarg_ (0), sequence_ (0) { /* empty */ }

    charT       cid_;
    Traits      tid_;
    VirtualTag  vfun_;
    const char *cname_;      // character type name
    const char *tname_;      // traits name
    const char *fname_;      // function name

    const char *strarg_;     // string argument
    const char *sequence_;   // final sequence
};


static int  stringbuf_capacity;
static char long_string [4096];

/**************************************************************************/

template <class charT, class Traits>
void test_virtual (charT, Traits, const VFun *pfid,
                   int         line,           // line number
                   const char *str,            // ctor string argument
                   std::size_t,                // length of string
                   int         mode,           // ctor mode argument
                   int         gbump,          // initial gptr offset
                   int         arg0,           // first argument
                   int         arg1,           // second argument
                   int         arg2,           // third argument
                   int         ret_expect,     // expected return value
                   int         pback_expect,   // ... size of putback area
                   int         read_expect,    // ... size of read area
                   int         write_expect)   // ... size of write area
{
    typedef std::allocator<charT>                          Allocator;
    typedef std::basic_stringbuf<charT, Traits, Allocator> Stringbuf;

    typedef typename Stringbuf::int_type                   int_type;
    typedef typename Stringbuf::off_type                   off_type;
    typedef typename Stringbuf::pos_type                   pos_type;

    if (!rw_enabled (line)) {
        rw_note (0, 0, 0, "test on line %d disabled", line);
        return;
    }

    if (EOF == arg0)
        arg0 = Traits::eof ();

    if (EOF == ret_expect)
        ret_expect = Traits::eof ();

    // widen the source sequence into the (possibly wide) character buffer
    static charT wstr [4096];
    static charT warg [4096];

    std::size_t wstr_len = sizeof wstr / sizeof *wstr;
    std::size_t warg_len = sizeof warg / sizeof *warg;

    wstr [0] = charT ();
    rw_expand (wstr, str, _RWSTD_SIZE_MAX, &wstr_len);

    const std::ios_base::openmode openmode = std::ios_base::openmode (mode);

    // construct three stringbuf objects but use only the one that
    // matches the provided arguments
    Stringbuf sb_0;
    Stringbuf sb_m (openmode);
    Stringbuf sb_s (wstr);
    Stringbuf sb_s_m (wstr, openmode);

    PubBuf<charT, Traits>* const pbuf = (PubBuf<charT, Traits>*)
        (str ? -1 < mode ? &sb_s_m : &sb_s : -1 < mode ? &sb_m : &sb_0);

    if (stringbuf_capacity < 0)
        stringbuf_capacity =
            PubBuf<charT, Traits>::capacity (-stringbuf_capacity);

    if (gbump && IGN != gbump) {
        // make sure gbump is valid
        RW_ASSERT (gbump <= pbuf->Egptr () - pbuf->Eback ());

        // advance gptr() as requested
        pbuf->Gbump (gbump);
    }

    // create the argument to overflow
    const int_type arg_int = int_type (arg0);
    const off_type arg_off = off_type (arg0);
    const pos_type arg_pos = arg_off;

    const std::ios_base::seekdir  arg_way   = std::ios_base::seekdir (arg1);
    const std::ios_base::openmode arg_which = std::ios_base::openmode (arg2);

    int ret = EOF;

    // invoke the virtual function with the expected argument (if any)
    switch (pfid->vfun_) {
    case VFun::xsputn: {

        if (pfid->strarg_) {
            rw_expand (warg, pfid->strarg_, _RWSTD_SIZE_MAX, &warg_len);
            if (std::strchr (pfid->strarg_, '@'))
                arg0 = warg_len;

            RW_ASSERT (std::size_t (arg0) < sizeof wstr / sizeof *wstr);
            ret = pbuf->sputn (warg, arg0);
        }
        else {
            // invoke sputn() with pbase as an argument
            ret = pbuf->sputn (pbuf->Pbase (), arg0);
        }

        break;
    }

    case VFun::pbackfail:
        ret = IGN == arg0 ? pbuf->Pbackfail ()
                          : pbuf->Pbackfail (arg_int);
        break;

    case VFun::overflow:
        ret = IGN == arg0 ? pbuf->Overflow ()
                          : pbuf->Overflow (arg_int);
        break;

    case VFun::underflow:
        ret = pbuf->Underflow ();
        break;

    case VFun::seekoff:
        ret = IGN == arg2 ? pbuf->Seekoff (arg_off, arg_way)
                          : pbuf->Seekoff (arg_off, arg_way, arg_which);
        break;

    case VFun::seekpos:
        ret = IGN == arg2 ? pbuf->Seekpos (arg_pos)
                          : pbuf->Seekpos (arg_pos, arg_which);
        break;
    }

    // compute the number of putback positions available
    // see 27.5.1, p3, bullet 3
    const int pback_pos = int (pbuf->Gptr () - pbuf->Eback ());

    // compute the number of read positions available
    // see 27.5.1, p3, bullet 4
    const int read_pos = int (pbuf->Egptr () - pbuf->Gptr ());

    // compute the number of write positions available
    // see 27.5.1, p3, bullet 2
    const int write_pos = int (pbuf->Epptr () - pbuf->Pptr ());

    // set up a format string with the name of the class and member
    // function being called and describing the function arguments
    // basic_stringbuf<char> is formatted as stringbuf
    // basic_stringbuf<wchar_t> is formatted as wstringbuf
    // all other specializations are fully spelled out
#define CALLFMT                                                         \
    "line %d. "                                                         \
    "%{?}basic_%{:}%{?}w%{;}%{;}stringbuf%{?}<%s%{?}, %s%{;}>%{;}"      \
    "(%{?}%{*Ac}%{?}, %{Io}%{;}%{:}%{?}%{Io}%{;}%{;})"                  \
    ".%s (%{?}%{?}%{*Ac}%{:}pptr()%{;}, %d"   /* xsputn */              \
         "%{:}%{?}" /* pbackfail, over/underflow argument */            \
              "%{?}%{#lc}%{;}"                                          \
         "%{:}" /* seekoff and seekpos */                               \
              "%d%{?}, %{Iw}%{?}, %{Io}%{;}"                            \
                "%{:}%{?}, %{Io}%{;}%{;}"                               \
         "%{;}%{;})"

    // is the tested virtual function seekoff or seekpos?
    const bool is_seek = VFun::seekoff <= pfid->vfun_;

    // arguments corresponding to CALLFMT
#define CALLARGS                                                          \
    __LINE__,                                                             \
    0 != pfid->tname_, 'w' == *pfid->cname_, 0 != pfid->tname_,           \
    pfid->cname_, 0 != pfid->tname_, pfid->tname_,                        \
    0 != str, int (sizeof *wstr), wstr, -1 < mode, mode, -1 < mode, mode, \
    pfid->fname_, pfid->vfun_ == VFun::xsputn, 0 != pfid->strarg_,        \
        int (sizeof *warg), warg, arg0,                                   \
    !is_seek,                                                             \
    IGN != arg0, arg_int,                                                 \
    arg0, VFun::seekoff == pfid->vfun_, arg1, IGN != arg2, arg2,          \
    IGN != arg1, arg1

    const int_type not_eof = Traits::not_eof (arg_int);

    int success = ret == (NOT_EOF == ret_expect ? int (not_eof) : ret_expect);

    // verify the expected return value
    if (pfid->vfun_ == VFun::xsputn || is_seek) {
        rw_assert (success, 0, line,
                   CALLFMT " == %d, got %d",
                   CALLARGS, ret_expect, ret);
    }
    else {
        rw_assert (success, 0, line,
                   CALLFMT " == %{?}not EOF%{:}%{#lc}%{;}, got %{#lc}",
                   CALLARGS, NOT_EOF == ret_expect, ret_expect, ret);
    }

    // verify the expected size of the putback area
    if (IGN != pback_expect)
        rw_assert (pback_pos == pback_expect, 0, line,
                   CALLFMT ": putback positions (gptr - eback) == %d, got %d"
                   "%{?}: gptr = NULL%{;}",
                   CALLARGS, pback_expect, pback_pos,
                   0 == pbuf->Gptr ());

    // verify the expected size of the read area
    if (IGN != read_expect)
        rw_assert (read_pos == read_expect, 0, line,
                   CALLFMT ": read positions (egptr - gptr) == %d, got %d: "
                   "gptr = %{?}NULL%{:}eback + %td%{;}",
                   CALLARGS, read_expect, read_pos,
                   0 == pbuf->Gptr (), pbuf->Gptr () - pbuf->Eback ());

    // verify the expected size of the write area
    if (IGN != write_expect) {

        // at least as many write positions as expected
        success = -1 < write_expect ?
            write_expect <= write_pos : 0 == write_pos;

        rw_assert (success, 0, line,
                   CALLFMT ": write positions (epptr - pptr) "
                   "%{?}==%{:}>=%{;} %d, got %d: "
                   "pptr = %{?}NULL%{:}pbase + %td%{;}",
                   CALLARGS, 1 < write_expect, write_expect, write_pos,
                   0 == pbuf->Pptr (), pbuf->Pptr () - pbuf->Pbase ());
    }

    if (pfid->sequence_) {

        // verify that the put area matches the expected sequence
        const std::size_t len    = pbuf->Pptr () - pbuf->Pbase ();
        const std::size_t xmatch =
            rw_match (pfid->sequence_, pbuf->Pbase (), len);

        rw_assert (xmatch == len, 0, line,
                   CALLFMT ": put area mismatch (pbase + %zu)",
                   CALLARGS, xmatch);
    }
}

/**************************************************************************/

// dispatches to the appropriate specialization of the function template
void test_virtual (VFun         *pfid,
                   int           line,
                   const char   *str,
                   std::size_t   str_len,
                   int           mode,
                   int           gbump,
                   int           arg0,
                   int           arg1,
                   int           arg2,
                   int           ret_expect,
                   int           pback_expect,
                   int           read_expect,
                   int           write_expect)
{
#undef TEST
#define TEST(charT, Traits)                                     \
    test_virtual (charT (), Traits (), pfid, line,              \
                  str, str_len, mode, gbump,                    \
                  arg0, arg1, arg2, ret_expect,                 \
                  pback_expect, read_expect, write_expect)

    static const char* const fnames[] = {
        "xsputn", "pbackfail", "overflow", "underflow", "seekoff", "seekpos"
    };

    if (!rw_enabled (line)) {
        rw_note (0, 0, __LINE__, "test on line %d disabled", line);
        return;
    }

    pfid->fname_ = fnames [pfid->vfun_];

    if (VFun:: DefaultTraits == pfid->tid_) {
        if (VFun::Char == pfid->cid_)
            TEST (char, std::char_traits<char>);
#ifndef _RWSTD_NO_WCHAR_T
        else
            TEST (wchar_t, std::char_traits<wchar_t>);
#endif   // _RWSTD_NO_WCHAR_T
    }
    else {
        if (VFun::Char == pfid->cid_)
            TEST (char, CharTraits<char>);

#ifndef _RWSTD_NO_WCHAR_T
        else
            TEST (wchar_t, CharTraits<wchar_t>);
#endif   // _RWSTD_NO_WCHAR_T
    }

    pfid->strarg_   = 0;
    pfid->sequence_ = 0;
}

/**************************************************************************/

// convenience constants
const int in  = std::ios::in;
const int out = std::ios::out;
const int ate = std::ios::ate;

const int cur = std::ios::cur;
const int end = std::ios::end;

static int rw_opt_max_size = 1024;

/**************************************************************************/

static void
test_xsputn (VFun *pfid)
{
    rw_info (0, 0, 0, "basic_stringbuf<%s%{?}, %s%{;}>::xsputn"
             "(const char_type*, streamsize)",
             pfid->cname_, 0 != pfid->tname_, pfid->tname_);

    pfid->vfun_ = VFun::xsputn;

#undef TEST
#define TEST(str, mode, gbump, sarg, result, pback, read, write)        \
    pfid->strarg_ = sarg;                                                \
    test_virtual (pfid, __LINE__, str, sizeof str - 1, mode,            \
                  gbump, sizeof sarg - 1, IGN, IGN, result,             \
                  pback, read, write)

    //    +-------------------------------------------- initial sequence
    //    |      +------------------------------------- open mode
    //    |      |               +--------------------- gbump (gptr offset)
    //    |      |               |   +----------------- xsputn argument
    //    |      |               |   |     +----------- expected return value
    //    |      |               |   |     |  +-------- putback positions
    //    |      |               |   |     |  |  +----- read positions
    //    |      |               |   |     |  |  |  +-- write positions
    //    |      |               |   |     |  |  |  |
    //    V      V               V   V     V  V  V  V
    TEST (0,     0,              0, "",    0, 0, 0, -1);
    TEST (0,     0,              0, "a",   0, 0, 0, -1);
    TEST (0,     in,             0, "a",   0, 0, 0, -1);
    TEST (0,     out,            0, "a",   1, 0, 0, MAYBE_1);
    TEST (0,     out,            0, "ab",  2, 0, 0, MAYBE_1);
    TEST (0,     out,            0, "abc", 3, 0, 0, MAYBE_1);
    TEST (0,     in | out,       0, "abc", 3, 0, 3, MAYBE_1);
    TEST (0,     in | out | ate, 0, "abc", 3, 0, 3, MAYBE_1);

    TEST ("abc", in | out,       0, "def", 3, 0, 6, MAYBE_1);
    TEST ("abc", in | out | ate, 0, "def", 3, 0, 6, MAYBE_1);

    TEST (       0, in | out,       0, "a@1000", 1000,    0, 1000, MAYBE_1);
    TEST (       0, in | out,       0, "a@1000", 1000,    0, 1000, MAYBE_1);

    TEST (       0, in | out,       0, "a@1001", 1001,    0, 1001, MAYBE_1);
    TEST (   "a@1", in | out,       0, "a@1002", 1002,    0, 1003, MAYBE_1);
    TEST (   "a@2", in | out,       0, "a@1003", 1003,    0, 1005, MAYBE_1);
    TEST (   "a@3", in | out,       0, "a@1004", 1004,    0, 1007, MAYBE_1);

    TEST (       0, in | out | ate, 0, "a@1000", 1000,    0, 1000, MAYBE_1);
    TEST (       0, in | out | ate, 0, "a@1001", 1001,    0, 1001, MAYBE_1);
    TEST (   "a@1", in | out | ate, 0, "a@1002", 1002,    0, 1003, MAYBE_1);
    TEST (   "a@2", in | out | ate, 0, "a@1003", 1003,    0, 1005, MAYBE_1);
    TEST (   "a@3", in | out | ate, 0, "a@1004", 1004,    0, 1007, MAYBE_1);
    TEST ( "a@127", in | out | ate, 0, "a@1005", 1005,    0, 1132, MAYBE_1);

    TEST ("a@1000", in | out | ate, 0, "a@1006", 1006,    0, 2006, MAYBE_1);
    TEST ("a@2000", in | out | ate, 0, "a@1007", 1007,    0, 3007, MAYBE_1);
    TEST ("a@2001", in | out | ate, 0, "a@1008", 1008,    0, 3009, MAYBE_1);

    //////////////////////////////////////////////////////////////////
    // exercise calling sputn() with pptr() as the first argument
#undef TEST
#define TEST(str, mode, len, result, seq, read)                 \
    pfid->strarg_ = 0; pfid->sequence_ = seq;                   \
    test_virtual (pfid, __LINE__, str, sizeof str - 1, mode,    \
                  0, len, IGN, IGN, result,                     \
                  0, read, MAYBE_1)

    //     +---------------------------------------------- initial sequence
    //     |         +------------------------------------ open mode
    //     |         |               +-------------------- xsputn 2nd argument
    //     |         |               |     +-------------- return value
    //     |         |               |     |     +-------- sequence at pbase()
    //     |         |               |     |     |     +-- read positions
    //     |         |               |     |     |     |
    //     |         |               |     |     |     +---------+
    //     |         |               |     |     |               |
    //     V         V               V     V     V               V
    TEST ( "a@500b", in | out | ate,   11,   11, "a@500ba@11",   512);
    TEST ( "a@500b", in | out | ate,   12,   12, "a@500ba@12",   513);
    TEST ( "a@500b", in | out | ate,   13,   13, "a@500ba@13",   514);
    TEST ( "a@500b", in | out | ate,  500,  500, "a@500ba@500",  1001);
    TEST ( "a@500b", in | out | ate,  501,  501, "a@500ba@500b", 1002);
    TEST ("a@1022b", in | out | ate,    1,    1, "a@1022ba",     1024);
    TEST ("a@1023b", in | out | ate,    1,    1, "a@1023ba",     1025);

    //////////////////////////////////////////////////////////////////
    // exercise multiples of stringbuf capacity
#undef TEST
#define TEST(len, mode, gbump, arg_len, result, pback, read, write)  \
    pfid->strarg_ = long_string + (sizeof long_string - 1 - len),    \
    test_virtual (pfid, __LINE__, pfid->strarg_,                     \
                  len, mode, gbump, arg_len, IGN, IGN,               \
                  result, pback, read, write)

// for convenience
#define CAP stringbuf_capacity

    // compute the minimum buffer capacity
    stringbuf_capacity = -1;
    TEST (   0, in | out,       0,    0,    0,    0,    0, -1);

    //    +----------------------------------------------- length of sequence
    //    |    +------------------------------------------ open mode
    //    |    |               +-------------------------- gbump (gptr offset)
    //    |    |               |     +-------------------- xsputn 2nd argument
    //    |    |               |     |     +-------------- expected return value
    //    |    |               |     |     |     +-------- putback positions
    //    |    |               |     |     |     |  +----- read positions
    //    |    |               |     |     |     |  |  +-- write positions
    //    |    |               |     |     |     |  |  |
    //    |    |               |     |     |     |  |  +--------+
    //    |    |               |     |     |     |  |           |
    //    V    V               V     V     V     V  V           V
    TEST (CAP, in | out,       0,    1,    1,    0, CAP +    1, MAYBE_1);
    TEST (CAP, in | out,       0,    2,    2,    0, CAP +    2, MAYBE_1);
    TEST (CAP, in | out,       0,    3,    3,    0, CAP +    3, MAYBE_1);
    TEST (CAP, in | out,       0,  127,  127,    0, CAP +  127, MAYBE_1);
    TEST (CAP, in | out,       0, 1023, 1023,    0, CAP + 1023, MAYBE_1);
    TEST (CAP, in | out,       0, 1024, 1024,    0, CAP + 1024, MAYBE_1);
// The following test case causes an ASSERT to fail because the internal size
// of the buffer of 'stringbuf' is a lot bigger than the limit imposed in the
// test case.  The point of failure is at line 241:
//..
//  RW_ASSERT (std::size_t (arg0) < sizeof wstr / sizeof *wstr);
//..
// Where 'sizeof wstr' is 4096.
#if TEST_RW_EXTENSIONS
    TEST (CAP, in | out,       0,  CAP,  CAP,    0, CAP +  CAP, MAYBE_1);
#endif

    TEST (CAP, in | out | ate, 0,    1,    1,    0, CAP +    1, MAYBE_1);
    TEST (CAP, in | out | ate, 0,    2,    2,    0, CAP +    2, MAYBE_1);
    TEST (CAP, in | out | ate, 0,    3,    3,    0, CAP +    3, MAYBE_1);
#if TEST_RW_EXTENSIONS
    TEST (CAP, in | out | ate, 0,  CAP,  CAP,    0, CAP +  CAP, MAYBE_1);
#endif
    TEST (CAP, in | out | ate, 0,  127,  127,    0, CAP +  127, MAYBE_1);
    TEST (CAP, in | out | ate, 0, 1023, 1023,    0, CAP + 1023, MAYBE_1);
    TEST (CAP, in | out | ate, 0, 1024, 1024,    0, CAP + 1024, MAYBE_1);
    TEST (CAP, in | out | ate, 0, 1025, 1025,    0, CAP + 1025, MAYBE_1);

    for (int i = 0; i != rw_opt_max_size; ++i) {
        TEST (  0, in | out, 0, i, i, 0, i +   0, MAYBE_1);
        TEST (  1, in | out, 0, i, i, 0, i +   1, MAYBE_1);
        TEST ( 32, in | out, 0, i, i, 0, i +  32, MAYBE_1);
        TEST (127, in | out, 0, i, i, 0, i + 127, MAYBE_1);
        TEST (CAP, in | out, 0, i, i, 0, i + CAP, MAYBE_1);
    }
}

/**************************************************************************/

static void
test_pbackfail (VFun *pfid)
{
    rw_info (0, 0, 0, "basic_stringbuf<%s%{?}, %s%{;}>::pbackfail(int_type)",
             pfid->cname_, 0 != pfid->tname_, pfid->tname_);

    pfid->vfun_ = VFun::pbackfail;

#undef TEST
#define TEST(str, mode, gbump, arg, result, pback, read, write)         \
    test_virtual (pfid, __LINE__, str, sizeof str - 1, mode,            \
                  gbump, arg, IGN, IGN, result, pback, read, write)

    ///////////////////////////////////////////////////////////////////////
    // 27.7.1.3
    //
    // -2- Effects: Puts back the character designated by c to the input
    //     sequence, if possible, in one of three ways:
    //     --  If traits::eq_int_type(c,traits::eof()) returns false and
    //         if the input sequence has a putback position available,
    //         and if traits::eq(to_char_type(c),gptr()[-1]) returns true,
    //         assigns gptr() - 1 to gptr().
    //         Returns: c.
    //     --  If traits::eq_int_type(c,traits::eof()) returns false and
    //         if the input sequence has a putback position available, and
    //         if mode & ios_base::out is nonzero, assigns c to *--gptr().
    //         Returns: c.
    //     --  If traits::eq_int_type(c,traits::eof()) returns true and if
    //         the input sequence has a putback position available, assigns
    //         gptr() - 1 to gptr().
    //         Returns: traits::not_eof(c).
    // -3- Returns: traits::eof() to indicate failure.
    // -4- Notes: If the function can succeed in more than one of these ways,
    //     it is unspecified which way is chosen.

    //    +----------------------------------------- initial sequence (if any)
    //    |      +---------------------------------- open mode
    //    |      |         +------------------------ gbump (gptr offset)
    //    |      |         |   +-------------------- pbackfail argument
    //    |      |         |   |   +---------------- expected return value
    //    |      |         |   |   |    +----------- number of putback positions
    //    |      |         |   |   |    |  +-------- number of read positions
    //    |      |         |   |   |    |  |  +----- number of write positions
    //    |      |         |   |   |    |  |  |
    //    V      V         V   V   V    V  V  V
    TEST (0,     0,        0, 'c', EOF, 0, 0, -1);
    TEST (0,     0,        0, EOF, EOF, 0, 0, -1);

    TEST (0,     in,       0, 'c', EOF, 0, 0, -1);
    TEST (0,     out,      0, EOF, EOF, 0, 0, -1);
    TEST (0,     ate,      0, EOF, EOF, 0, 0, -1);
    TEST (0,     in | out, 0, 'c', EOF, 0, 0, -1);
    TEST (0,     in | out, 0, EOF, EOF, 0, 0, -1);

    TEST ("a",   0,        0, 'c', EOF, 0, 0, -1);
    TEST ("a",   in,       0, 'c', EOF, 0, 1, -1);
    TEST ("a",   in,       1, 'c', EOF, 1, 0, -1);
    TEST ("a",   in,       1, 'a', 'a', 0, 1, -1);
    TEST ("a",   out,      0, 'c', EOF, 0, 0, AT_LEAST_1);
    TEST ("a",   ate,      0, 'c', EOF, 0, 0, -1);

    TEST ("abc", in,       1, 'c', EOF, 1, 2, -1);
    TEST ("abc", in,       2, 'c', EOF, 2, 1, -1);
    TEST ("abc", in,       3, 'c', 'c', 2, 1, -1);

    TEST ("abc", in | out, 0, 'c', EOF, 0, 3, AT_LEAST_1);
    TEST ("abc", in | out, 1, 'c', 'c', 0, 3, AT_LEAST_1);
    TEST ("abc", in | out, 2, 'c', 'c', 1, 2, AT_LEAST_1);
    TEST ("abc", in | out, 3, 'c', 'c', 2, 1, AT_LEAST_1);
}

/**************************************************************************/

static void
test_overflow (VFun *pfid)
{
    rw_info (0, 0, 0, "basic_stringbuf<%s%{?}, %s%{;}>::overflow(int_type)",
             pfid->cname_, 0 != pfid->tname_, pfid->tname_);

    pfid->vfun_ = VFun::overflow;

#undef TEST
#define TEST(str, mode, gbump, arg, result, pback, read, write)         \
    test_virtual (pfid, __LINE__, str, sizeof str - 1, mode, gbump,     \
                  arg, IGN, IGN, result, pback, read, write);

    ///////////////////////////////////////////////////////////////////////
    // 27.7.1.3 + LWG issues 169 and 432
    //
    // -5- Effects: Appends the character designated by c to the output
    //     sequence, if possible, in one of two ways:
    //     --  If traits::eq_int_type(c,traits::eof()) returns false and
    //         if either the output sequence has a write position available
    //         or the function makes a write position available (as described
    //         below), the function calls sputc(c).
    //         Signals success by returning c.
    //     --  If traits::eq_int_type(c,traits::eof()) returns true, there
    //         is no character to append.
    //         Signals success by returning a value other than traits::eof().
    // -6- Notes: The function can alter the number of write positions
    //     available as a result of any call.
    // -7- Returns: traits::eof() to indicate failure.
    // -8- (blank)
    // -9- Notes: The function can make a write position available only if
    //     (mode & ios_base::out) != 0. To make a write position available,
    //     the function reallocates (or initially allocates) an array object
    //     with a sufficient number of elements to hold the current array
    //     object (if any), plus one additional write position. If
    //     (mode & ios_base::in) != 0, the function alters the read end
    //     pointer egptr() to point just past the new write position.
    ///////////////////////////////////////////////////////////////////////

    //    +-------------------------------------- initial sequence (if any)
    //    |   +---------------------------------- open mode
    //    |   |         +------------------------ gbump
    //    |   |         |   +-------------------- overflow argument
    //    |   |         |   |   +---------------- result of call
    //    |   |         |   |   |     +---------- putback positions after call
    //    |   |         |   |   |     |   +------ read positions after call
    //    |   |         |   |   |     |   |   +-- write positions after call
    //    |   |         |   |   |     |   |   |
    //    |   |         |   |   |     |   |   |
    //    V   V         V   V   V     V   V   V
    TEST (0,  0,        0, 'c', EOF, 0, 0, -1);
    TEST (0,  in,       0, 'c', EOF, 0, 0, -1);
    TEST (0,  out,      0, 'c', 'c', 0, 0, AT_LEAST_1);
    TEST (0,  in | out, 0, 'c', 'c', 0, 1, AT_LEAST_1);

    TEST (0,  in | ate,         0, 'c', EOF, 0, 0, -1);
    TEST (0,  out | ate,        0, 'c', 'c', 0, 0, AT_LEAST_1);
    TEST (0,  in | out | ate,   0, 'c', 'c', 0, 1, AT_LEAST_1);

    TEST ("", 0,                0, 'c', EOF, 0, 0, -1);
    TEST ("", in,               0, 'c', EOF, 0, 0, -1);
    TEST ("", out,              0, 'c', 'c', 0, 0, AT_LEAST_1);
    TEST ("", in | out,         0, 'c', 'c', 0, 1, AT_LEAST_1);

    TEST ("a", 0,               0, 'c', EOF, 0, 0, -1);
    TEST ("a", in,              0, 'c', EOF, 0, 1, -1);
    TEST ("a", out,             0, 'c', 'c', 0, 0, AT_LEAST_1);
    TEST ("a", in | out,        0, 'c', 'c', 0, 2, AT_LEAST_1);

    TEST ("a", in | ate,        0, 'c', EOF, 0, 1, -1);
    TEST ("a", out | ate,       0, 'c', 'c', 0, 0, MAYBE_1);
    TEST ("a", in | out | ate,  0, 'c', 'c', 0, 2, MAYBE_1);

    TEST ("ab", 0,              0, 'c', EOF, 0, 0, -1);
    TEST ("ab", in,             0, 'c', EOF, 0, 2, -1);
    TEST ("ab", out,            0, 'c', 'c', 0, 0, AT_LEAST_1);
    TEST ("ab", in | out,       0, 'c', 'c', 0, 3, AT_LEAST_1);

    TEST ("ab", in | ate,       0, 'c', EOF, 0, 2, -1);
    TEST ("ab", out | ate,      0, 'c', 'c', 0, 0, AT_LEAST_1);
    TEST ("ab", in | out | ate, 0, 'c', 'c', 0, 3, AT_LEAST_1);

    // verify that when the argument is EOF the function returns a value
    // other than traits::eof() regardless of the streambuf open mode
    TEST (0, 0,                   0, EOF, NOT_EOF, 0, 0, -1);
    TEST (0, in,                  0, EOF, NOT_EOF, 0, 0, -1);
    TEST (0, out,                 0, EOF, NOT_EOF, 0, 0, -1);
    TEST (0, in | out,            0, EOF, NOT_EOF, 0, 0, -1);
    TEST (0, in | ate,            0, EOF, NOT_EOF, 0, 0, -1);
    TEST (0, out | ate,           0, EOF, NOT_EOF, 0, 0, -1);
    TEST (0, in | out | ate,      0, EOF, NOT_EOF, 0, 0, -1);

    TEST ("abc",  0,              0, EOF, NOT_EOF, 0, 0, -1);
    TEST ("abc",  in,             0, EOF, NOT_EOF, 0, 3, -1);
    TEST ("abc",  out,            0, EOF, NOT_EOF, 0, 0, AT_LEAST_1);
    TEST ("abc",  in | out,       0, EOF, NOT_EOF, 0, 3, AT_LEAST_1);
    TEST ("abc",  in | ate,       0, EOF, NOT_EOF, 0, 3, -1);
    TEST ("abc",  out | ate,      0, EOF, NOT_EOF, 0, 0, MAYBE_1);
    TEST ("abc",  in | out | ate, 0, EOF, NOT_EOF, 0, 3, MAYBE_1);
}

/**************************************************************************/

static void
test_underflow (VFun *pfid)
{
    rw_info (0, 0, 0, "basic_stringbuf<%s%{?}, %s%{;}>::underflow()",
             pfid->cname_, 0 != pfid->tname_, pfid->tname_);

    pfid->vfun_ = VFun::underflow;

#undef TEST
#define TEST(str, mode, gbump, result, pback, read, write)              \
    test_virtual (pfid, __LINE__, str, sizeof str - 1, mode, gbump,     \
                  IGN, IGN, IGN, result, pback, read, write);


    ///////////////////////////////////////////////////////////////////////
    // 27.7.1.3 + LWG issue 432
    //
    // int_type undeflow ()
    //
    // -1- If the input sequence has a read position available, returns
    //     traits::to_int_type(*gptr()). Otherwise, returns traits::eof().
    //     Any character in the underlying buffer which has been initialized
    //     is considered to be part of the input sequence.

    //    +------------------------------------------ initial sequence
    //    |      +----------------------------------- open mode
    //    |      |               +------------------- gbump
    //    |      |               |  +---------------- underflow result
    //    |      |               |  |    +----------- putback positions
    //    |      |               |  |    |  +-------- read positions
    //    |      |               |  |    |  |   +---- write positions
    //    |      |               |  |    |  |   |
    //    |      |               |  |    |  |   |
    //    |      |               |  |    |  |   |
    //    V      V               V  V    V  V   V
    TEST (0,     0,              0, EOF, 0, 0, -1);
    TEST (0,     in,             0, EOF, 0, 0, -1);
    TEST (0,     out,            0, EOF, 0, 0, -1);
    TEST (0,     in | out,       0, EOF, 0, 0, -1);
    TEST (0,     in | out | ate, 0, EOF, 0, 0, -1);

    TEST ("",    0,              0, EOF, 0, 0, -1);
    TEST ("",    in,             0, EOF, 0, 0, -1);
    TEST ("",    out,            0, EOF, 0, 0, -1);
    TEST ("",    in | out,       0, EOF, 0, 0, -1);
    TEST ("",    in | out | ate, 0, EOF, 0, 0, -1);

    TEST ("abc", 0,              0, EOF, 0, 0, -1);
    TEST ("abc", in,             0, 'a', 0, 3, -1);
    TEST ("abc", in,             1, 'b', 1, 2, -1);
    TEST ("abc", in,             2, 'c', 2, 1, -1);
    TEST ("abc", in,             3, EOF, 3, 0, -1);
    TEST ("abc", out,            0, EOF, 0, 0, AT_LEAST_1);
    TEST ("abc", in | out,       0, 'a', 0, 3, AT_LEAST_1);
    TEST ("abc", in | out,       1, 'b', 1, 2, AT_LEAST_1);
    TEST ("abc", in | out,       2, 'c', 2, 1, AT_LEAST_1);
    TEST ("abc", in | out,       3, EOF, 3, 0, AT_LEAST_1);
    TEST ("abc", in | out | ate, 0, 'a', 0, 3, MAYBE_1);
    TEST ("abc", in | out | ate, 1, 'b', 1, 2, MAYBE_1);
    TEST ("abc", in | out | ate, 2, 'c', 2, 1, MAYBE_1);
    TEST ("abc", in | out | ate, 3, EOF, 3, 0, MAYBE_1);

    //////////////////////////////////////////////////////////////////
    // exercise UserTraits with an unusual eof

    if (0 == pfid->tname_)
        return;

    TEST ("a$c", in,             0, 'a',  0,  3, -1);
    TEST ("a$c", in,             1, EOF,  1,  2, -1);
}

/**************************************************************************/

// exercises both seekoff (seeking from the beginning) and seekpos
static void
test_seek (VFun *pfid)
{
    RW_ASSERT (   VFun::seekoff == pfid->vfun_
               || VFun::seekpos == pfid->vfun_);

#undef TEST
#define TEST(str, mode, gbump, off, which, res, pback, read, write)     \
    test_virtual (pfid, __LINE__, str, sizeof str - 1, mode, gbump,     \
                  off, std::ios::beg, which, res, pback, read, write)

    //////////////////////////////////////////////////////////////////
    //
    // 27.7.1.3, p10 + LWG issue 432
    //
    // pos_type seekoff (off_type off, ios_base::seekdir way,
    //                   ios_base::openmode which = in | out);
    //
    // -10- Effects: Alters the stream position within one of the controlled
    // sequences, if possible, as indicated in Table 90:
    //
    // Table 90 -- seekoff positioning
    // -------------------------------
    // Conditions      Result
    // --------------- ---------------
    // (which & in)    positions the input sequence
    // (which & out)   positions the output sequence
    // (which & (in | out)) == (in & out) && (way == beg || way == end)
    //                 positions both sequences
    // otherwise       fails
    // -------------------------------
    //
    // -11- For a sequence to be positioned, if its next pointer (either
    //      gptr() or pptr()) is a null pointer, the positioning operation
    //      fails. Otherwise, the function determines newoff as indicated
    //      in Table 91:
    //
    // Table 91 -- newoff values
    // ----------------------------
    //
    // Condition       Newoff value
    // --------------- ------------
    // (way == beg)    0
    // (way == cur)    xnext - xbeg
    // (way == end)    xend - xbeg
    // ----------------------------
    //
    // -12- If (newoff + off) < 0, or if (newoff + off) refers to an
    //      uninitialized character (as defined in 27.7.1.2
    //      [lib.stringbuf.members] paragraph 1), the positioning operation
    //      fails. Otherwise, the function assigns (xbeg + newoff + off) to
    //      the next pointer xnext.

    // -13- Returns: pos_type(newoff), constructed from the resultant offset
    //      newoff (of type off_type), that stores the resultant stream
    //      position, if possible. If the positioning operation fails, or
    //      if the constructed object cannot represent the resultant stream
    //      position, the return value is pos_type(off_type(-1)).
    //
    //////////////////////////////////////////////////////////////////

    //    +-- initial sequence
    //    |      +-- open mode (in, out, ate)
    //    |      |               +-- gbump
    //    |      |               |   +-- offset
    //    |      |               |   |  +-- which (in, out)
    //    |      |               |   |  |  +--- resulting position
    //    |      |               |   |  |  |     +-- putback positions
    //    |      |               |   |  |  |     |  +-- read positions
    //    |      |               |   |  |  |     |  |  +-- write pos.
    //    |      |               |   |  |  |     |  |  |
    //    |      |               |   |  |  |     |  |  |
    //    |      |               |   |  |  |     |  |  |
    //    V      V               V   V  V  V     V  V  V
    TEST (0,     0,              0,  0, 0, NPOS, 0, 0, -1);
    TEST (0,     in,             0,  0, 0, NPOS, 0, 0, -1);
    TEST (0,     out,            0,  0, 0, NPOS, 0, 0, -1);
    TEST (0,     ate,            0,  0, 0, NPOS, 0, 0, -1);
    TEST (0,     in | out,       0,  0, 0, NPOS, 0, 0, -1);
    TEST (0,     in | out | ate, 0,  0, 0, NPOS, 0, 0, -1);

    TEST (0,     0,              0,  0, 0, NPOS, 0, 0, -1);

    TEST ("abc", 0,              0,  0, 0, NPOS, 0, 0, -1);
    TEST ("abc", 0,              0,  1, 0, NPOS, 0, 0, -1);
    TEST ("abc", 0,              0, -1, 0, NPOS, 0, 0, -1);

    // seek within the input sequence from the beginning
    TEST ("abc", in,             0,  0, in,    0, 0, 3, -1);
    TEST ("abc", in,             0,  1, in,    1, 1, 2, -1);
    TEST ("abc", in,             0,  2, in,    2, 2, 1, -1);
    TEST ("abc", in,             0,  3, in,    3, 3, 0, -1);
    TEST ("abc", in,             0,  4, in, NPOS, 0, 3, -1);

    // advance gptr by one and seek within the input sequence
    TEST ("abc", in,             1,  0, in,    0, 0, 3, -1);
    TEST ("abc", in,             1,  1, in,    1, 1, 2, -1);
    TEST ("abc", in,             1,  2, in,    2, 2, 1, -1);
    TEST ("abc", in,             1,  3, in,    3, 3, 0, -1);
    TEST ("abc", in,             1,  4, in, NPOS, 1, 2, -1);

    // advance gptr by two and seek within the input sequence
    TEST ("abc", in,             2,  0, in,    0, 0, 3, -1);
    TEST ("abc", in,             2,  1, in,    1, 1, 2, -1);
    TEST ("abc", in,             2,  2, in,    2, 2, 1, -1);
    TEST ("abc", in,             2,  3, in,    3, 3, 0, -1);
    TEST ("abc", in,             2,  4, in, NPOS, 2, 1, -1);

    // exercise seeking within the output sequence
    TEST ("abc", out,            0,  0, out,    0, 0, 0, MAYBE_1);
    TEST ("abc", out,            0,  1, out,    1, 0, 0, MAYBE_1);
    TEST ("abc", out,            0, -1, out, NPOS, 0, 0, MAYBE_1);
    TEST ("abc", out,            0,  2, out,    2, 0, 0, MAYBE_1);
    TEST ("abc", out,            0,  3, out,    3, 0, 0, MAYBE_1);
    TEST ("abc", out,            0,  4, out, NPOS, 0, 0, MAYBE_1);

    // seeking withing both input and output
    TEST ("abc", in | out,       0,  0, in | out,    0, 0, 3, MAYBE_1);
    TEST ("abc", in | out,       0,  1, in | out,    1, 1, 2, MAYBE_1);
    TEST ("abc", in | out,       0, -1, in | out, NPOS, 0, 3, MAYBE_1);
    TEST ("abc", in | out,       0,  2, in | out,    2, 2, 1, MAYBE_1);
    TEST ("abc", in | out,       0,  3, in | out,    3, 3, 0, MAYBE_1);
    TEST ("abc", in | out,       0,  4, in | out, NPOS, 0, 3, MAYBE_1);
}

/**************************************************************************/

static void
test_seekoff (VFun *pfid)
{
    rw_info (0, 0, 0, "basic_stringbuf<%s%{?}, %s%{;}>::seekoff(off_type, "
             "ios_base::seekdir, ios_base::openmode)",
             pfid->cname_, 0 != pfid->tname_, pfid->tname_);

    pfid->vfun_ = VFun::seekoff;

    test_seek (pfid);

#undef TEST
#define TEST(str, mode, gbump, off, way, which, res, pback, read, write)  \
    test_virtual (pfid, __LINE__, str, sizeof str - 1, mode, gbump,       \
                  off, way, which, res, pback, read, write)

    // exercise seeking from the current position and from end

    //    +-- initial sequence
    //    |      +-- open mode (in, out, ate)
    //    |      |               +-- gbump
    //    |      |               |   +-- offset
    //    |      |               |   |  +-- way (cur or end)
    //    |      |               |   |  |    +-- which (in, out)
    //    |      |               |   |  |    |  +--- resulting position
    //    |      |               |   |  |    |  |     +-- putback positions
    //    |      |               |   |  |    |  |     |  +-- read positions
    //    |      |               |   |  |    |  |     |  |  +-- write pos.
    //    |      |               |   |  |    |  |     |  |  |
    //    |      |               |   |  |    |  |     |  |  |
    //    |      |               |   |  |    |  |     |  |  |
    //    V      V               V   V  V    V  V     V  V  V
    TEST (0,     0,              0,  0, cur, 0, NPOS, 0, 0, -1);
    TEST (0,     in,             0,  0, cur, 0, NPOS, 0, 0, -1);
    TEST (0,     out,            0,  0, cur, 0, NPOS, 0, 0, -1);
    TEST (0,     ate,            0,  0, cur, 0, NPOS, 0, 0, -1);
    TEST (0,     in | out,       0,  0, cur, 0, NPOS, 0, 0, -1);
    TEST (0,     in | out | ate, 0,  0, cur, 0, NPOS, 0, 0, -1);

    TEST (0,     0,              0,  0, end, 0, NPOS, 0, 0, -1);
    TEST (0,     in,             0,  0, end, 0, NPOS, 0, 0, -1);
    TEST (0,     out,            0,  0, end, 0, NPOS, 0, 0, -1);
    TEST (0,     ate,            0,  0, end, 0, NPOS, 0, 0, -1);
    TEST (0,     in | out,       0,  0, end, 0, NPOS, 0, 0, -1);
    TEST (0,     in | out | ate, 0,  0, end, 0, NPOS, 0, 0, -1);

    // exercise seeking within the output sequence
    // (if (mode & in) == 0 then (pptr() == pbase()) is a postcondition
    // of the constructor -- see also lwg issue 562)
    TEST ("abc", out,            0,  0, cur, out,    0, 0, 0, MAYBE_1);
    TEST ("abc", out,            0,  1, cur, out,    1, 0, 0, MAYBE_1);
    TEST ("abc", out,            0, -1, cur, out, NPOS, 0, 0, MAYBE_1);
    TEST ("abc", out,            0,  2, cur, out,    2, 0, 0, MAYBE_1);
    TEST ("abc", out,            0,  3, cur, out,    3, 0, 0, MAYBE_1);
    TEST ("abc", out,            0,  4, cur, out, NPOS, 0, 0, MAYBE_1);

    // seek within the input sequence from the current position
    TEST ("abc", in,             0,  0, cur, in,    0, 0, 3, -1);
    TEST ("abc", in,             0,  1, cur, in,    1, 1, 2, -1);
    TEST ("abc", in,             0,  2, cur, in,    2, 2, 1, -1);
    TEST ("abc", in,             0,  3, cur, in,    3, 3, 0, -1);
    TEST ("abc", in,             0,  4, cur, in, NPOS, 0, 3, -1);

    // seek within the input sequence from the end
    TEST ("abc", in,             0,  0, end, in,    3, 3, 0, -1);
    TEST ("abc", in,             0, -1, end, in,    2, 2, 1, -1);
    TEST ("abc", in,             0, -2, end, in,    1, 1, 2, -1);
    TEST ("abc", in,             0, -3, end, in,    0, 0, 3, -1);
    TEST ("abc", in,             0, -4, end, in, NPOS, 0, 3, -1);

    // exercise positioning from the end: this behavior is not
    // correctly specified even in LWG issue 432 (see the post
    // in c++std-lib-16390)

    // the implemented behavior considers end to be the "high
    // mark" mentioned in DR 432 rather than epptr()

    // see LWG issue 563:
    // http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#563
    TEST ("abc", out,            0,  0, end, out,    3, 0, 0, MAYBE_1);
    TEST ("abc", out,            0,  1, end, out, NPOS, 0, 0, MAYBE_1);
    TEST ("abc", out,            0, -1, end, out,    2, 0, 0, MAYBE_1);
    TEST ("abc", out,            0, -2, end, out,    1, 0, 0, MAYBE_1);
    TEST ("abc", out,            0, -3, end, out,    0, 0, 0, MAYBE_1);
    TEST ("abc", out,            0, -4, end, out, NPOS, 0, 0, MAYBE_1);

    TEST ("abc", out | ate,      0,  0, cur, out,    3, 0, 0, MAYBE_1);
    TEST ("abc", out | ate,      0,  1, cur, out, NPOS, 0, 0, MAYBE_1);
    TEST ("abc", out | ate,      0, -1, cur, out,    2, 0, 0, MAYBE_1);
    TEST ("abc", out | ate,      0, -2, cur, out,    1, 0, 0, MAYBE_1);
    TEST ("abc", out | ate,      0, -3, cur, out,    0, 0, 0, MAYBE_1);
    TEST ("abc", out | ate,      0, -4, cur, out, NPOS, 0, 0, MAYBE_1);

    // seeking within both input and output from the end
    TEST ("abc", in | out,       0,  0, end, in | out,    3, 3, 0, MAYBE_1);
    TEST ("abc", in | out,       0,  1, end, in | out, NPOS, 0, 3, MAYBE_1);
    TEST ("abc", in | out,       0, -1, end, in | out,    2, 2, 1, MAYBE_1);
    TEST ("abc", in | out,       0, -2, end, in | out,    1, 1, 2, MAYBE_1);
    TEST ("abc", in | out,       0, -3, end, in | out,    0, 0, 3, MAYBE_1);
    TEST ("abc", in | out,       0, -4, end, in | out, NPOS, 0, 3, MAYBE_1);

    TEST ("abc", in,             0,  0, cur, in | out, NPOS, 0, 3, MAYBE_1);
    TEST ("abc", out,            0,  0, cur, in | out, NPOS, 0, 0, MAYBE_1);
    TEST ("abc", in | out,       0,  0, cur, in | out, NPOS, 0, 3, MAYBE_1);
    TEST ("abc", in | out | ate, 0,  0, cur, in | out, NPOS, 0, 3, MAYBE_1);

    TEST ("abc", in | out | ate, 1, -1, end, in,          2, 2, 1, MAYBE_1);
    TEST ("abc", in | out | ate, 1, -1, end, out,         2, 1, 2, MAYBE_1);

    TEST ("abc", in | out | ate, 1, -1, end, in | out,    2, 2, 1, MAYBE_1);
    TEST ("abc", in | out | ate, 2, -1, end, in | out,    2, 2, 1, MAYBE_1);
    TEST ("abc", in | out | ate, 3, -1, end, in | out,    2, 2, 1, MAYBE_1);
}

/**************************************************************************/

static void
test_seekpos (VFun *pfid)
{
    rw_info (0, 0, 0, "basic_stringbuf<%s%{?}, %s%{;}>::seekpos(pos_type, "
             "ios_base::openmode)",
             pfid->cname_, 0 != pfid->tname_, pfid->tname_);

    pfid->vfun_ = VFun::seekpos;

    test_seek (pfid);
}

/**************************************************************************/

static int rw_opt_no_xsputn;        // for --no-xsputn
static int rw_opt_no_pbackfail;     // for --no-pbackfail
static int rw_opt_no_overflow;      // for --no-overflow
static int rw_opt_no_underflow;     // for --no-underflow
static int rw_opt_no_seekoff;       // for --no-seekoff
static int rw_opt_no_seekpos;       // for --no-seekpos
static int rw_opt_no_seek;          // for --no-seek
static int rw_opt_no_char_traits;   // for --no-char_traits
static int rw_opt_no_user_traits;   // for --no-user_traits

static void
run_test (VFun *pfid)
{
#undef TEST
#define TEST(function)                                          \
    if (rw_opt_no_ ## function)                                 \
        rw_note (1 < rw_opt_no_ ## function++, 0, 0,            \
                 "%s test disabled", #function);                \
    else                                                        \
        test_ ## function (pfid)

    if (pfid->tname_ && rw_opt_no_user_traits) {
        rw_note (1 < rw_opt_no_user_traits++, 0, 0,
                 "user defined traits test disabled");
    }
    else if (!pfid->tname_ && rw_opt_no_char_traits) {
        rw_note (1 < rw_opt_no_char_traits++, 0, 0,
                 "char_traits test disabled");
    }
    else {
        TEST (xsputn);
        TEST (pbackfail);
        TEST (overflow);
        TEST (underflow);
        TEST (seekoff);
        TEST (seekpos);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    if ('\0' == long_string [0]) {
        // initialize long_string
        for (std::size_t i = 0; i != sizeof long_string - 1; ++i)
            long_string [i] = 'x';
    }

    if (rw_opt_no_seek) {
        rw_opt_no_seekoff = 1;
        rw_opt_no_seekpos = 1;
    }

    if (rw_enabled ("char")) {

        VFun fid (VFun::Char, "char", VFun::DefaultTraits, 0);

        traits_eof = -1;

        run_test (&fid);

        fid.tid_    = VFun::UserTraits;
        fid.tname_  = "UserTraits";
        traits_eof = '$';

        run_test (&fid);
    }
    else
        rw_note (0, 0, 0, "char tests disabled");

    if (rw_enabled ("wchar_t")) {
        VFun fid (VFun::WChar, "wchar_t", VFun::DefaultTraits, 0);

        traits_eof = -1;

        run_test (&fid);

        fid.tid_    = VFun::UserTraits;
        fid.tname_  = "UserTraits";
        traits_eof = '$';

        run_test (&fid);
    }
    else
        rw_note (0, 0, 0, "char tests disabled");

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.stringbuf.virtuals",
                    0,   // no comment
                    run_test,
                    "|-no-xsputn# "
                    "|-no-pbackfail# "
                    "|-no-overflow# "
                    "|-no-underflow# "
                    "|-no-seekoff# "
                    "|-no-seekpos# "
                    "|-no-seek# "
                    "|-no-char_traits# "
                    "|-no-user_traits# "
                    "|-max_buf_size#0-*",
                    &rw_opt_no_xsputn,
                    &rw_opt_no_pbackfail,
                    &rw_opt_no_overflow,
                    &rw_opt_no_underflow,
                    &rw_opt_no_seekoff,
                    &rw_opt_no_seekpos,
                    &rw_opt_no_seek,
                    &rw_opt_no_char_traits,
                    &rw_opt_no_user_traits,
                    int (sizeof long_string) - 1,
                    &rw_opt_max_size);
}
