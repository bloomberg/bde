/************************************************************************
 *
 * 27.istream.manip.cpp - test exercising the basic_istream ws manipulator
 *
 * $Id: 27.istream.manip.cpp 580483 2007-09-28 20:55:52Z sebor $
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
 * Copyright 2003-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <cstddef>
#include <istream>

#include <driver.h>

/************************************************************************

    namespace std {
        template <class charT, class traits>
        basic_istream<charT,traits>& ws(basic_istream<charT,traits>& is);
    }

    -1- Effects: Extracts characters as long as the next available
        character c is whitespace or until there are no more characters
        in the sequence. Whitespace characters are distinguished with
        the same criterion as used by sentry::sentry (27.6.1.1.2). If
        ws stops extracting characters because there are no more
        available it sets eofbit, but not failbit.
    -2- Returns: is

************************************************************************/

template <class charT>
struct Ctype: std::ctype<charT>
{
    typedef std::ctype<charT> Base;

    const charT ws_;   // whitespace character

    std::ctype_base::mask table_ [256];

    Ctype (std::size_t ref, charT white)
        : Base (ref), ws_ (white) { }

    // used by ctype<wchar_t>
    /* virtual */ bool
    do_is (std::ctype_base::mask m, charT c) const {
        if (m & std::ctype_base::space)
            return ws_ == c;
        return Base::is (m, c);
    }

    // not used by the test, defined only to silence compiler warnings
    // about the overload above hiding the base function overloaded below
    /* virtual */ const charT*
    do_is (const charT *lo, const charT *hi, std::ctype_base::mask *m) const {
        return Base::do_is (lo, hi, m);
    }
};


// specialized for ctype<char>
_RWSTD_SPECIALIZED_CLASS
Ctype<char>::Ctype (std::size_t ref, char white)
    : Base (table_, false, ref), ws_ (white)
{
    for (std::size_t i = 0; i != sizeof table_ / sizeof *table_; ++i)
        table_ [i] = std::ctype_base::mask ();

    typedef unsigned char UChar;

    table_ [UChar (ws_)] = std::ctype_base::space;
}


// user-defined Traits type exercises the ability to instantiate
// std::ws and any supporting templates on types other than the
// default std::char_traits<charT>
template <class charT>
struct UserTraits: std::char_traits<charT> { };


template <class charT, class Traits>
struct Streambuf: std::basic_streambuf<charT, Traits>
{
    typedef std::basic_streambuf<charT, Traits> Base;

    typedef typename Base::traits_type          traits_type;
    typedef typename Base::int_type             int_type;

    int nsyncs_;       // number of calls to sync()

    // overflow() will return eof () after extracting this many chars
    int fail_after_;

    // overflow() will throw am exception after extracting this many chars
    int throw_after_;

    const charT *end_;          // the end of the buffer (future eback())

    Streambuf (const charT *gbeg, const charT *gend)
        : Base (),
          nsyncs_ (0),
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

    /* virtual */ int sync () {
        ++nsyncs_;
        return Base::sync ();
    }

    /* virtual */ int_type underflow () {
        if (this->gptr () == end_)
            return traits_type::eof ();

        if (throw_after_ == this->gptr () - this->eback ()) {

#ifndef _RWSTD_NO_EXCEPTIONS

            throw throw_after_;

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

/***********************************************************************/

template <class charT, class Traits>
void
test_ws (const Traits*           /* dummy */,
         const char             *cname,
         const char             *tname,
         std::size_t             iter,
         const charT            *cbuf,
         std::size_t             cbuf_size,
         std::ios_base::iostate  state,
         std::size_t             j,
         bool                    skipws,
         charT                   white)
{
    typedef std::basic_istream<charT, Traits>   Istream;
    typedef typename Istream::sentry            Sentry;

    // construct a ctype object that treats `wc' as the only
    // whitespace character
    FACET_CONST Ctype<charT> ctp (1, white);

    // construct a stream buffer object and initialize its read sequence
    // with the character buffer
    Streambuf<charT, Traits> sb (cbuf, cbuf + cbuf_size - 1);

    // set err_after to the number of successful extractions from
    // the stream; subsequent extraction will cause an error
    const std::size_t err_after = j % (cbuf_size - 1);
    const char *err_type;

    if (j < cbuf_size - 1) {
        // have the stream buffer object's underflow() fail (by throwing
        // an exception if possible) after j characters have been extracted
        // (this object calls underflow() for every char)
        sb.throw_after_ = err_after;
        err_type        = "threw";
    }
    else {
        // have the stream buffer object's underflow() fail by returning
        // eof after j characters have been extracted (this object calls
        // underflow() for every char)
        sb.fail_after_ = err_after;
        err_type       = "returned EOF";
    }

    // construct an istream object and initialize it with the user
    // defined streambuf object
    Istream is (&sb);

    // set the stream object's initial state
    is.setstate (state);

    // set or clear the skipws bit
    if (skipws)
        is.setf (std::ios_base::skipws);
    else
        is.unsetf (std::ios_base::skipws);

    // construct a locale object containing the user-defined ctype object
    const std::locale loc = is.imbue (std::locale (is.getloc (), &ctp));

    // imbue the previous locale into the stream buffer to verify that
    // the ws manipulator uses the locale imbued in the stream object
    // and not the one in the stream buffer
    sb.pubimbue (loc);

    // the number of extracted whitespace characters expected to
    // be reported by gcount() must equal the number of successful
    // extractions computed by the the stream buffer overridden
    // underflow member function
    const int extract = is.good () && white == cbuf [0] ? int (err_after) : 0;

#ifndef _RWSTD_NO_EXCEPTIONS

    bool caught = false;

    // on every other iteration set badbit in exceptions to check
    // that the thrown object is propagated by the function
    if (err_after % 2 && !is.bad ())
        is.exceptions (std::ios_base::badbit);

    try {
        std::ws (is);
    }
    catch (int) {
        caught = true;
    }
    catch (...) {

        caught = true;

        rw_assert (false, 0, __LINE__,
                   "%u. std::ws (basic_istream<%s, %s >&);"
                   " threw exception of unexpected type",
                   iter, cname, tname);
    }

    //////////////////////////////////////////////////////////////////
    // verify that the function propagates exceptions thrown from the
    // streambuf object only when badbit is set in the stream object's
    // exceptions()

    rw_assert (!(caught && !(err_after % 2)), 0, __LINE__,
               "%u. std::ws (basic_istream<%s, %s>&);"
               " unexpectedly propagated exception",
               iter, cname, tname);

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    std::ws (is);

#endif   // _RWSTD_NO_EXCEPTIONS

    //////////////////////////////////////////////////////////////////
    // verify that the expected number of whitespace characters have
    // been extracted from the stream

    rw_assert (cbuf + extract == sb.pubgptr (), 0, __LINE__,
               "%u. std::ws (basic_istream<%s, %s>&);"
               " expected to extract %d "
               "whitespace chars ('%c') from %{*Ac}, "
               "extracted %u; initial rdstate() = %{Is}, "
               "is.flags() & ios::skipws = %d, "
               "underflow %s at extraction %u",
               iter, cname, tname, extract, (char)(white),
               int (sizeof *cbuf), cbuf,
               sb.pubgptr () - sb.pubeback (), state,
               skipws, err_type, err_after);

    //////////////////////////////////////////////////////////////////
    // verify that gcount() value not affected (27.6.1.4 p1)

    rw_assert (0 == is.gcount (), 0, __LINE__,
               "%u. std::ws (basic_istream<%s, %s >&)."
               "gcount() == %d, got %d; whitespace is"
               " '%c', input is %{*Ac}, initial rdstate() = %{Is}, "
               "flags() & ios::skipws = %d,"
               " underflow %s at extraction %u",
               iter, cname, tname, extract, is.gcount (),
               (char)(white), int (sizeof *cbuf), cbuf, state,
               skipws, err_type, err_after);

    //////////////////////////////////////////////////////////////////
    // verify the state of the stream object after the function call

    // expected stream state after the function call is unchanged
    // (i.e., the initial stream state), // except...
    std::ios_base::iostate expect_state = state;

    if (state) {
        // lwg 419
        expect_state |= std::ios_base::failbit;
    }
    else if (white == cbuf [0] || !err_after) {

#ifndef _RWSTD_NO_EXCEPTIONS

        // ...when any whitespace characters are extracted, or the
        // first character on the stream is peeked at, even if it's
        // not a space, and an exception is thrown during input,
        // badbit should be set, otherwise eofbit (bot not failbit)
        // should be set
        if (j < cbuf_size - 1)
            expect_state = std::ios_base::badbit;
        else
            expect_state = std::ios_base::eofbit;

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

        expect_state = std::ios_base::eofbit;

#endif   // _RWSTD_NO_EXCEPTIONS
    }

    rw_assert (is.rdstate () == expect_state, 0, __LINE__,
               "%u. std::ws (basic_istream<%s, %s >&)."
               "rdstate() == %{Is}, got %{Is}; "
               "extracting whitespace ('%c') from %{*Ac}, "
               "extracted %u; initial is.rdstate() = %{Is}, "
               "is.flags() & ios::skipws = %d, underflow %s "
               "at extraction %u",
               iter, cname, tname,
               expect_state, is.rdstate(),
               (char)(white), int (sizeof *cbuf), cbuf,
               sb.pubgptr () - sb.pubeback (), state,
               skipws, err_type, err_after);
}

/***********************************************************************/


template <class charT, class Traits>
void test_ws (charT*, Traits*, const char *cname, const char *tname)
{
    rw_info (0, 0, __LINE__, "std::basic_istream<%s, %s>& "
             "std::ws (std::basic_istream<%1$s, %2$s>&)",
             cname, tname);

    const charT cbuf[] = { 'a', 'a', 'a', '\0' };
    const std::size_t cbuf_size = sizeof cbuf / sizeof *cbuf;

    const std::ios_base::iostate states[] = {
        std::ios_base::badbit,
        std::ios_base::eofbit,
        std::ios_base::failbit,
        std::ios_base::goodbit,
        std::ios_base::badbit | std::ios_base::eofbit,
        std::ios_base::badbit | std::ios_base::failbit,
        std::ios_base::eofbit | std::ios_base::failbit,
        std::ios_base::badbit | std::ios_base::eofbit | std::ios_base::failbit
    };

    const std::size_t nstates = sizeof states / sizeof *states;

    {   //////////////////////////////////////////////////////////////
        // exercise 27.6.1.4, p1

        std::size_t iter = 0;     // iteration counter

        // iterate over all possible settings of the stream state
        for (std::size_t i = 0; i != nstates; ++i) {

            // iterate twice over all characters in the character buffer,
            // once to trigger an exception at each iteration (except
            // the last one), and then again to have underflow() fail
            // by returning EOF at each iteration (except the last one)
            for (std::size_t j = 0; j != 2 * cbuf_size - 1; ++j) {

                // iterate over the ios_base::skipws setting
                for (std::size_t k = 0; k != 2; ++k /* skipws */) {

                    // iterate over two whitespace characters
                    for (charT wc = charT ('a'); wc != charT ('c'); ++wc) {

                        test_ws ((Traits*)0, cname, tname, iter,
                                 cbuf, cbuf_size, states [i], j, !!k, wc);

                        ++iter;
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
#define TEST(charT, Traits) \
    test_ws ((charT*)0, (Traits*)0, #charT, #Traits)

    if (rw_note (0 <= opt_char && 0 <= opt_char_traits, 0, __LINE__,
                 "ws (basic_istream<char, char_traits<char>>) disabled"))
        TEST (char, std::char_traits<char>);

    if (rw_note (0 <= opt_char && 0 <= opt_user_traits, 0, __LINE__,
                 "ws (basic_istream<char, UserTraits<char>>) disabled"))
        TEST (char, UserTraits<char>);

#ifndef _RWSTD_NO_WCHAR_T

    if (rw_note (0 <= opt_wchar && 0 <= opt_char_traits, 0, __LINE__,
                 "ws (basic_istream<wchar_t, char_traits<wchar_t>>) disabled"))
        TEST (wchar_t, std::char_traits<wchar_t>);

    if (rw_note (0 <= opt_wchar && 0 <= opt_user_traits, 0, __LINE__,
                 "ws (basic_istream<wchar_t, UserTraits<wchar_t>>) disabled"))
        TEST (wchar_t, UserTraits<wchar_t>);

#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}

/***********************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "istream.manip",
                    0 /* no comment */,
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
