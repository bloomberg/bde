/************************************************************************
 *
 * 27.readsome.cpp - test exercising istream::readsome()
 *
 * $Id: 27.istream.readsome.cpp 503954 2007-02-06 01:59:05Z sebor $
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
 * Copyright 2003-2007 Rogue Wave Software.
 * 
 **************************************************************************/

#include <istream>
#include <sstream>

#include <driver.h>
#include <rw_char.h>     // for UserChar, UserTraits
#include <rw_printf.h>   // for rw_printf()

/***********************************************************************/

template <class charT, class Traits>
struct Streambuf: std::basic_streambuf<charT, Traits>
{
    typedef std::basic_streambuf<charT, Traits> Base;
    typedef typename Base::traits_type          traits_type;
    typedef typename Base::int_type             int_type;

    // value returned from showmanyc()
    int showmanyc_;

    // underflow() will return eof () after extracting this many chars
    int fail_after_;

    // underflow() will throw am exception after extracting this many chars
    int throw_after_;

    const charT *end_;          // the end of the buffer (future eback())

    Streambuf (const charT *gbeg, const charT *gend)
        : Base (),
          showmanyc_ (0),
          fail_after_ (-1),    // never fail
          throw_after_ (-1),   // never throw
          end_ (gend) {
        this->setg (_RWSTD_CONST_CAST (charT*, gbeg),
                    _RWSTD_CONST_CAST (charT*, gbeg),
                    _RWSTD_CONST_CAST (charT*, gbeg));
    }

    const charT* pubeback () const {
        return this->eback ();
    }

    const charT* pubgptr () const {
        return this->gptr ();
    }

    const charT* pubegptr () const {
        return this->egptr ();
    }

    /* virtual */ std::streamsize showmanyc () {
#ifndef _RWSTD_NO_EXCEPTIONS

        if (-2 == showmanyc_)
            throw (const char*)"showmanyc";

#endif   // _RWSTD_NO_EXCEPTIONS

        return showmanyc_;
    }

    /* virtual */ int_type underflow () {
        if (this->gptr () == end_)
            return traits_type::eof ();

        if (throw_after_ == this->gptr () - this->eback ()) {

#ifndef _RWSTD_NO_EXCEPTIONS

            throw (const char*)"underflow";

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

            return traits_type::eof ();

#endif   // _RWSTD_NO_EXCEPTIONS

        }

        if (fail_after_ == this->gptr () - this->eback ())
            return traits_type::eof ();

        this->setg (this->eback (), this->gptr (), this->gptr () + 1);
        return traits_type::to_int_type (*this->gptr ());
    }
};

/***********************************************************************

    27.6.1.3, p30 and 31

    streamsize readsome (char_type* s, streamsize n);

    -30- Effects: If !good() calls setstate(failbit) which may throw an
         exception, and return. Otherwise extracts characters and stores
         them into successive locations of an array whose first element
         is designated by s.
         --  If rdbuf()­>in_avail() == ­1, calls setstate(eofbit) (which
             may throw ios_base::failure (27.4.4.3)), and extracts no
             characters;
         --  If rdbuf()­>in_avail() == 0, extracts no characters
         --  If rdbuf()­>in_avail() > 0, extracts min(rdbuf()­>in_avail(),
             n)).
    -31- Returns: The number of characters extracted.

************************************************************************/

void
memfun_info (int line, const char *cname, const char *tname)
{
    // format the ISTREAM environment variable w/o writing
    // out any output
    rw_fprintf (0,
                "%{$ISTREAM!:@}",
                "%{?}istream%{:}%{?}wistream"
                "%{:}basic_istream<%s, %s>%{;}%{;}",
                'c' == *cname && 'c' == *tname, 
                'w' == *cname && 'c' == *tname,
                cname, tname);

    rw_info (0, 0, line,
             "std::%{$ISTREAM}::readsome (char_type*, streamsize)");
}

/************************************************************************/

extern const std::ios_base::iostate
states[] = {
    std::ios_base::badbit,
    std::ios_base::eofbit,
    std::ios_base::failbit,
    std::ios_base::goodbit,
    std::ios_base::badbit | std::ios_base::eofbit,
    std::ios_base::badbit | std::ios_base::failbit,
    std::ios_base::eofbit | std::ios_base::failbit,
    std::ios_base::badbit | std::ios_base::eofbit | std::ios_base::failbit
};

extern const unsigned nstates = sizeof states / sizeof *states;


template <class charT, class Traits>
void test_readsome (const charT *cbuf, const Traits*,
                    unsigned cbuf_size,
                    unsigned i,   // index into states
                    unsigned j,   // number of chars to read
                    unsigned k,   // when underflow() will fail
                    int      l,   // value returned from showmanyc()
                    unsigned m)   // how underflow should fail()
{
    typedef std::basic_istream<charT, Traits> Istream;

    static unsigned iter = 0;   // iteration counter

    // construct a stream buffer object and initialize its read
    // sequence with the character buffer
    Streambuf<charT, Traits> sb (cbuf, cbuf + cbuf_size - 1);

    sb.showmanyc_ = l;

    const char* err_type = 0;

    if (m < 1) {
        // have the stream buffer object's underflow() fail (by
        // throwing an exception if possible) after k characters
        // have been extracted (this object calls underflow() for
        // every char)
        sb.throw_after_ = k;
        err_type        = "threw";
    }
    else {
        // have the stream buffer object's underflow() fail by
        // returning eof after k characters have been extracted
        // (this object calls underflow() for every char)
        sb.fail_after_ = k;
        err_type       = "returned EOF";
    }

    // construct an istream object and initialize it with the user
    // defined streambuf object
    Istream is (&sb);

    // set the stream object's initial state
    is.setstate (states [i]);

    // the number of extracted whitespace characters expected to
    // be reported by gcount() must equal the number of successful
    // extractions computed by the the stream buffer overridden
    // underflow member function
    const int extract =
        is.good () ? j < k ? int (j) < l ? j
                                         : l < 0 ? 0 : l
                           : int (k) < l ? k
                                         : l < 0 ? 0 : l
                   : 0;

    charT buf [256] = { 0 };

    std::streamsize nread = -1;

    // format the FCALL environment variable so that it can be
    // conveniently used in diagnostics below
    rw_fprintf (0, "%u. %{$FCALL!:@}",
                iter,
                "%{$ISTREAM}(%{*Ac}).readsome(%p, %d)",
                int (sizeof *cbuf), cbuf, buf, j);

#ifndef _RWSTD_NO_EXCEPTIONS

    const char *caught = 0;

    // on every other iteration set badbit in exceptions to check
    // that the thrown object is propagated by the function
    if (k % 2 && !is.bad ())
        is.exceptions (std::ios_base::badbit);

    try {
        nread = is.readsome (buf, j);
    }
    catch (const char *s) {
        caught = s;
    }
    catch (...) {

        caught = "...";

        rw_assert (0, 0, __LINE__,
                   "%{$FCALL} threw an exception of unexpected type");
    }

    //////////////////////////////////////////////////////////////////
    // verify that the function propagates exceptions thrown from the
    // streambuf object only when badbit is set in the stream object's
    // exceptions()

    rw_assert (!caught || (k % 2), 0, __LINE__,
               "%{$FCALL} unexpectedly propagated an exception");

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    nread = is.readsome (buf, j);

#endif   // _RWSTD_NO_EXCEPTIONS

    //////////////////////////////////////////////////////////////////
    // verify that the function returned the expected number of
    // extracted characters

    const std::streamsize extracted = sb.pubgptr () - sb.pubeback ();

    rw_assert (extract == extracted, 0, __LINE__,
               "%{$FCALL} expected to extract %d chars, got %u; "
               "initial state = %{Is}, underflow %s at extraction %u",
               extract, extracted, states [i],
               err_type, k);

    //////////////////////////////////////////////////////////////////
    // verify that the expected number of characters have been
    // extracted from the stream

    rw_assert (cbuf + extract == sb.pubgptr (), 0, __LINE__,
               "%{$FCALL} expected to extract %d chars, got %u; "
               "initial state = %{Is}, underflow %s at extraction %u",
               extract, extracted, states [i],
               err_type, k);

    //////////////////////////////////////////////////////////////////
    // verify that the extracted characters match those in the buffer

    rw_assert (0 == std::char_traits<charT>::compare (buf, cbuf, extract),
               0, __LINE__,
               "%{$FCALL} expected to extract the first %d chars, got %{*Ac}",
               extract, int (sizeof *buf), buf);

    //////////////////////////////////////////////////////////////////
    // verify that gcount() correctly reflects the number of
    // characters successfully extracted from the stream

    rw_assert (extract == is.gcount (), 0, __LINE__,
               "%{$FCALL}: gcount() == %d, got %d; initial state = %{Is}, "
               "underflow %s at extraction %u",
               extract, is.gcount (), states [i],
               err_type, k);

    //////////////////////////////////////////////////////////////////
    // verify the state of the stream object after the function call

    // expected stream state after the function call is unchanged
    // (i.e., the initial stream state), except...
    std::ios_base::iostate expect_state = states [i];

    if (!states [i]) {

#ifndef _RWSTD_NO_EXCEPTIONS

        // ...if an extraction is attempted, or even if the first
        // character on the stream is peeked at, and an exception
        // is thrown during input, badbit should be set, otherwise
        // if in_avail() returned -1, eofbit should be set, else
        // the state should be good
        if (-2 == l)
            expect_state = std::ios_base::badbit;
        else if (l < 0)
            expect_state = std::ios_base::eofbit;
        else
            expect_state = std::ios_base::goodbit;

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

        if (l < 0)
            expect_state = std::ios_base::eofbit;
        else
            expect_state = std::ios_base::goodbit;

#endif   // _RWSTD_NO_EXCEPTIONS

    }
    else {

        // ...if the initial stream state is not good, failbit
        // must be set
        expect_state = states [i] | std::ios_base::failbit;
    }

    rw_assert (is.rdstate () == expect_state, 0, __LINE__,
               "%{$FCALL}: rdstate() == %{Is}, got %{Is}; "
               "extracted %u characters; "
               "initial state = %{Is}, underflow %s at extraction %u",
               expect_state, is.rdstate (), extracted,
               states [i], err_type, k);

    ++iter;
}

/***********************************************************************/


template <class charT, class Traits>
void test_readsome (const charT*, const Traits*,
                    const char *cname, const char *tname)
{
    memfun_info (__LINE__, cname, tname);

    //////////////////////////////////////////////////////////////
    // exercise 27.6.1.3, p1

    const charT cbuf[] = { ' ', 'a', ' ', 'b', 'c', '\0' };
    const unsigned cbuf_size = sizeof cbuf / sizeof *cbuf;

    unsigned iter   = 0;   // iteration counter

    // iterate over all possible settings of the stream state
    for (unsigned i = 0; i != nstates; ++i) {

        // call readsome(..., j) to extract j characters 
        for (unsigned j = 0; j != cbuf_size - 1; ++j) {

            // have underflow() fail after the `k-th' extraction
            for (unsigned k = 0; k != j + 1; ++k) {

                // return `l' from showmanyc(); -2 will throw
                for (int l = -4; l != int (k); ++l) {

                    // if (0 == m), underflow() will throw an exception
                    // at `k-th' extraction, otherwise the function will
                    // return EOF
                    for (unsigned m = 0; m != 2; ++m, ++iter) {

                        // prevent causing underflow() to fail by
                        // returning EOF when in_avail() promised
                        // it wouldn't (by returning a value
                        // greater than the number of extractions
                        // underflow() will allow to succeed)
                        if (1U == m && int (k) < l)
                            continue;

                        test_readsome (cbuf, (Traits*)0,
                                       cbuf_size, i, j, k, l, m);
                    }
                }
            }
        }
    }
}

/***********************************************************************/

static int opt_char;
static int opt_wchar;
static int opt_char_traits;
static int opt_user_traits;


static int
run_test (int, char**)
{
    // introduce char_traits into scope
    using namespace std;

#undef TEST
#define TEST(CharT, Traits) \
    test_readsome ((CharT*)0, (Traits*)0, #CharT, #Traits)

    if (rw_note (0 <= opt_char && 0 <= opt_char_traits, 0, __LINE__,
                 "basic_istream<char, char_traits<char>>::readsome() "
                 "tests disabled"))
        TEST (char, char_traits<char>);

    if (rw_note (0 <= opt_char && 0 <= opt_user_traits, 0, __LINE__,
                 "basic_istream<char, UserTraits<char>>::readsome() "
                 "tests disabled"))
        TEST (char, UserTraits<char>);

#ifndef _RWSTD_NO_WCHAR_T

    if (rw_note (0 <= opt_wchar && 0 <= opt_char_traits, 0, __LINE__,
                 "basic_istream<wchar_t, char_traits<wchar_t>>::readsome() "
                 "tests disabled"))
        TEST (wchar_t, char_traits<wchar_t>);

    if (rw_note (0 <= opt_wchar && 0 <= opt_user_traits, 0, __LINE__,
                 "basic_istream<wchar_t, UserTraits<wchar_t>>::readsome() "
                 "tests disabled"))
        TEST (wchar_t, UserTraits<wchar_t>);

#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}

/***********************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "istream.unformatted",
                    "readsome",
                    run_test,
                    "|-char~ "
                    "|-wchar_t~ "
                    "|-char_traits~ "
                    "|-UserTraits~ ",
                    &opt_char,
                    &opt_wchar,
                    &opt_char_traits,
                    &opt_user_traits,
                    (void*)0   /* sentinel */);
}
