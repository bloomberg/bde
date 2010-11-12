/************************************************************************
 *
 * istream_sentry.cpp - test exercising basic_istream<charT>::sentry
 *
 * $Id: 27.istream.sentry.cpp 503938 2007-02-06 00:41:34Z sebor $
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

#include <istream>
#include <locale>        // for ctype
#include <streambuf>     // for streambuf

#include <rw_char.h>     // for UserChar, UserTraits
#include <rw_printf.h>   // for rw_printf()
#include <driver.h>

/************************************************************************

    27.6.1.1.2   Class basic_istream::sentry   [lib.istream::sentry]

    namespace std {
        template <class charT, class traits = char_traits<charT> >
        class basic_istream<charT, traits>::sentry {
            typedef traits traits_type;
            bool ok_;                            // exposition only
        public:
            explicit
            sentry (basic_istream<charT, traits>& is, bool noskipws = false);
            ~sentry ();
            operator bool() const { return ok_; }
        private:
            sentry (const sentry&);              // not defined
            sentry& operator= (const sentry&);   // not defined
        };
    }

    -1- The class sentry defines a class that is responsible for doing
        exception safe prefix and suffix operations.

    explicit sentry(basic_istream<charT, traits>& is, bool noskipws = false);
    -2- Effects: If is.good() is true, prepares for formatted or unformatted
        input. First, if is.tie() is not a null pointer, the function calls
        is.tie()­>flush() to synchronize the output sequence with any
        associated external C stream. Except that this call can be suppressed
        if the put area of is.tie() is empty. Further an implementation is
        allowed to defer the call to flush until a call of
        is->rdbuf()­>underflow occurs. If no such call occurs before the
        sentry object is destroyed, the call to flush may be eliminated
        entirely(279). If noskipws is zero and is.flags() & ios_base::skipws
        is nonzero, the function extracts and discards each character as long
        as the next available input character c is a whitespace character.
    -3- Notes: The constructor explicit sentry(basic_istream<charT, traits>&
        is, bool noskipws = false) uses the currently imbued locale in is,
        to determine whether the next input character is whitespace or not.
    -4- To decide if the character cis a whitespace character, the constructor
        performs "as if" it executes the following code fragment:
        const ctype<charT>& ctype = use_facet<ctype<charT> >(is.getloc());
        if (ctype.is (ctype.space,c) != 0)
            // c is a whitespace character.
    -5- If, after any preparation is completed, is.good() is true,
        ok_ != false otherwise, ok_ == false. During preparation, the
        constructor may call setstate(failbit) (which may throw
        ios_base::failure (27.4.4.3))(280)
    __________________
    279) This will be possible only in functions that are part of the
         library. The semantics of the constructor used in user code is
         as specified.
    280) The sentry constructor and destructor can also perform additional
         implementation-dependent operations.

    -6- [Example: A typical implementation of the sentry constructor might
        include code such as:

        template <class charT, class traits = char_traits<charT> >
        basic_istream<charT,traits>::sentry(
            basic_istream<charT,traits>& is, bool noskipws = false) {
            ...
            int_type c;
            typedef ctype<charT> ctype_type;
            const ctype_type& ctype = use_facet<ctype_type>(is.getloc());
            while ((c = is.rdbuf()­>snextc()) != traits::eof()) {
                if (ctype.is(ctype.space,c)==0) {
                    is.rdbuf()­>sputbackc (c);
                    break;
                }
            }
            ...
        }
        --end example]

    ~sentry();
    -7- Effects: None.

    operator bool() const;
    -8- Effects: Returns ok_.

************************************************************************/

void
memfun_info (int line, const char *cname, const char *tname, const char *fname)
{
    // format the ISTREAM and SENTRY environment variables w/o writing
    // out any output
    rw_fprintf (0,
                "%{$ISTREAM!:@}",
                "%{?}istream%{:}%{?}wistream"
                "%{:}basic_istream<%s, %s>%{;}%{;}",
                'c' == *cname && 'c' == *tname, 
                'w' == *cname && 'c' == *tname,
                cname, tname);

    rw_fprintf (0,
                "%{$SENTRY!:@}",
                "%{$ISTREAM}::sentry");

    // pass fname through the "%{@}" directive to expand any embedded
    // %{$XYZ} directives
    rw_info (0, 0, line, "std::%{$SENTRY}::%{@}", fname);
}

/***********************************************************************/

template <class charT>
struct Ctype: std::ctype<charT>
{
    typedef std::ctype<charT> Base;

    const charT ws_;   // whitespace character

    std::ctype_base::mask table_ [256];

    Ctype (unsigned ref, charT white)
        : Base (ref), ws_ (white) { }

    // virtuals overridden below used by ctype<wchar_t>
    // ctype<char> uses the table() member instead

    /* virtual */ bool
    do_is (std::ctype_base::mask m, charT c) const {
        if (m & std::ctype_base::space)
            return ws_ == c;
        return Base::is (m, c);
    }

    /* virtual */ const charT*
    do_is (const charT *from, const charT *to,
           std::ctype_base::mask *m) const {
        return Base::is (from, to, m);
    }

    /* virtual */ const charT*
    do_scan_is (std::ctype_base::mask m,
                 const charT *from, const charT *to) const {
        for (; from != to && !do_is (m, *from); ++from);
        return from;
    }

    /* virtual */ const charT*
    do_scan_not (std::ctype_base::mask m,
                 const charT *from, const charT *to) const {
        for (; from != to && do_is (m, *from); ++from);
        return from;
    }
};


// specialized for ctype<char>
_RWSTD_SPECIALIZED_CLASS
Ctype<char>::Ctype (unsigned ref, char white)
    : Base (table_, false, ref), ws_ (white)
{
    for (unsigned i = 0; i != sizeof table_ / sizeof *table_; ++i)
        table_ [i] = std::ctype_base::mask ();

    typedef unsigned char UChar;

    table_ [UChar (ws_)] = std::ctype_base::space;
}


template <class charT, class Traits>
struct Streambuf: std::basic_streambuf<charT, Traits>
{
    typedef std::basic_streambuf<charT, Traits> Base;

    int nsyncs_;

    Streambuf (const charT *gbeg, const charT *gend)
        : Base (), nsyncs_ (0) {
        this->setg (_RWSTD_CONST_CAST (charT*, gbeg),
                    _RWSTD_CONST_CAST (charT*, gbeg),
                    _RWSTD_CONST_CAST (charT*, gend));
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
};


template <class charT, class Traits>
void test_ctor (const charT*, const Traits*,
                const char *cname, const char *tname)
{
    typedef std::basic_istream<charT, Traits>   Istream;
    typedef typename Istream::sentry            Sentry;

    memfun_info (__LINE__, cname, tname, "sentry (%{$ISTREAM}&, bool)");

    const charT cbuf[] = { 'a', 'b', 'c', 'd', 'e', ' ', 'f', '\0' };

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

    //////////////////////////////////////////////////////////////
    // exercise 27.6.1.1.2, p1:
    //     -  is.good() is true
    //     -  is.tie() is not null
    //     =  the function calls is.tie().flush()

    unsigned iter = 0;     // iteration counter

    for (unsigned i = 0; i != sizeof states / sizeof *states; ++i) {
        for (unsigned j = 0; j != 2; ++j /* noskipws */) {
            Streambuf<charT, Traits>
                sb (cbuf, cbuf + sizeof cbuf / sizeof *cbuf);

            Istream is (&sb);

            // flush() is called iff
            // all of the following conditions hold
            const bool flush_called = is.good () && 0 != is.tie ();

            const Sentry guard (is, 0 != j);

            _RWSTD_UNUSED (guard);

            rw_assert (flush_called == sb.nsyncs_, 0, __LINE__,
                       "%u. basic_istream<%s, %s>::sentry::sentry"
                       "(basic_istream &is, bool noskipws = %d); "
                       "expected to call is.flush () %d times, got %d"
                       "initial is.state () = %{Is}, is.flags() & "
                       "ios::skipws = %d",
                       iter, cname, tname, 0 != j, flush_called, sb.nsyncs_,
                       states [i], is.flags () & std::ios_base::skipws);

            ++iter;
        }
    }

    //////////////////////////////////////////////////////////////
    // exercise 27.6.1.1.2, p1:
    //     -  is.good() is true
    //     -  noskipws is zero
    //     -  is.flags() & ios_base::skipws
    //     =  the function extracts and discards each character as long
    //        as the next available input character c is a whitespace
    //        character.

    for (unsigned i = 0; i != sizeof states / sizeof *states; ++i) {
        for (unsigned j = 0; j != 2; ++j /* noskipws */) {
            for (unsigned k = 0; k != 2; ++k /* ios_base::skipws */) {
                for (charT wc = charT ('a'); wc != charT ('c'); ++wc) {

                    FACET_CONST Ctype<charT> ctp (1, wc);

                    Streambuf<charT, Traits>
                        sb (cbuf, cbuf + sizeof cbuf / sizeof *cbuf);

                    Istream is (&sb);

                    is.setstate (states [i]);

                    if (k)
                        is.setf (std::ios_base::skipws);
                    else
                        is.unsetf (std::ios_base::skipws);

                    const std::locale loc = 
                        is.imbue (std::locale (is.getloc (), &ctp));

                    // imbue the previous locale into the stream
                    // buffer to verify that the sentry ctor uses
                    // the locale imbued in the stream object and
                    // not the one in the stream buffer
                    sb.pubimbue (loc);

                    // a whitespace character is extracted iff
                    // all of the following conditions hold
                    const bool extract =
                           is.good ()
                        && 0 == j
                        && is.flags () & std::ios_base::skipws
                        && cbuf [0] == wc;

                    const Sentry guard (is, 0 != j);

                    _RWSTD_UNUSED (guard);

                    rw_assert (cbuf + extract == sb.pubgptr (), 0, __LINE__, 
                               "%u. %{$SENTRY}::sentry"
                               "(%{$ISTREAM} &is, bool noskipws "
                               "= %b); expected to extract %d "
                               "whitespace chars ('%c') from %{*Ac}, "
                               "extracted %u; initial is.state () = "
                               "%{Is}, is.flags() & ios::skipws = %d",
                               iter, j, extract + 0, char (wc),
                               int (sizeof (*cbuf)), cbuf,
                               sb.pubgptr () - sb.pubeback (),
                               states [i], k);

                    // verify that the ctor doesn't affect gcount()
                    rw_assert (0 == is.gcount (), 0, __LINE__, 
                               "%u. %{$SENTRY}::sentry"
                               "(%{$ISTREAM} &is = %{*Ac}, bool noskipws "
                               "= %b); changed is.gcount() from 0 to %i",
                               iter, int (sizeof (*cbuf)), cbuf, j,
                               is.gcount ());

                    ++iter;
                }
            }
        }
    }
}

/***********************************************************************/

template <class charT, class Traits>
void test_ok (const charT*, const Traits*,
              const char *cname, const char *tname)
{
    typedef std::basic_istream<charT, Traits>   Istream;
    typedef typename Istream::sentry            Sentry;

    memfun_info (__LINE__, cname, tname, "operator bool () const");

    const charT cbuf[] = { 'a', 'b', 'c', 'd', 'e', ' ', 'f', '\0' };

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

    //////////////////////////////////////////////////////////////
    // exercise 27.6.1.1.2, p5:
    //     -  is.good() is true
    //     -  noskipws is zero
    //     -  is.flags() & ios_base::skipws
    //     -  the function extracts and discards each character as long
    //        as the next available input character c is a whitespace
    //        character
    //     =  if, after any preparation is completed, is.good() is true,
    //        ok_ != false otherwise, ok_ == false.

    unsigned iter = 0;     // iteration counter

    for (unsigned i = 0; i != sizeof states / sizeof *states; ++i) {
        for (unsigned j = 0; j != 2; ++j /* noskipws */) {
            for (unsigned k = 0; k != 2; ++k /* ios_base::skipws */) {
                for (charT wc = charT ('a'); wc != charT ('c'); ++wc) {

                    FACET_CONST Ctype<charT> ctp (1, wc);

                    Streambuf<charT, Traits>
                        sb (cbuf, cbuf + sizeof cbuf / sizeof *cbuf);

                    Istream is (&sb);

                    is.setstate (states [i]);

                    if (k)
                        is.setf (std::ios_base::skipws);
                    else
                        is.unsetf (std::ios_base::skipws);

                    const std::locale loc = 
                        is.imbue (std::locale (is.getloc (), &ctp));

                    // imbue the previous locale into the stream
                    // buffer to verify that the sentry ctor uses
                    // the locale imbued in the stream object and
                    // not the one in the stream buffer
                    sb.pubimbue (loc);

                    const Sentry guard (is, 0 != j);

                    _RWSTD_UNUSED (guard);

                    const bool success =
                           is.good () && guard
                        || !is.good () && !guard;

                    rw_assert (success, 0, __LINE__,
                               "%u. %{$SENTRY}"
                               "(%{$ISTREAM} &is, bool noskipws "
                               "= %d).operator bool() == %d; initial "
                               "is.state() = %{Is}, is.flags() & "
                               "ios::skipws = %d",
                               iter, j, is.good (),
                               states [i], k);

                    ++iter;
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
#define TEST(what, charT, Traits) \
    test_ ## what ((charT*)0, (Traits*)0, #charT, #Traits)

    using namespace std;

    if (rw_note (0 <= opt_char && 0 <= opt_char_traits, 0, __LINE__,
                 "istream::sentry tests disabled")) {
        TEST (ctor, char, char_traits<char>);
        TEST (ok, char, char_traits<char>);
    }

    if (rw_note (0 <= opt_char && 0 <= opt_user_traits, 0, __LINE__,
                 "basic_istream<char, UserTraits<char>::sentry "
                 "tests disabled")) {
        TEST (ctor, char, UserTraits<char>);
        TEST (ok, char, UserTraits<char>);
    }

#ifndef _RWSTD_NO_WCHAR_T

    if (rw_note (0 <= opt_wchar && 0 <= opt_char_traits, 0, __LINE__,
                 "wistream::sentry tests disabled")) {
        TEST (ctor, wchar_t, char_traits<wchar_t>);
        TEST (ok, wchar_t, char_traits<wchar_t>);
    }

    if (rw_note (0 <= opt_wchar && 0 <= opt_user_traits, 0, __LINE__,
                 "basic_istream<wchar_t, UserTraits<wchar_t>::sentry "
                 "tests disabled")) {
        TEST (ctor, wchar_t, UserTraits<wchar_t>);
        TEST (ok, wchar_t, UserTraits<wchar_t>);
    }

#endif   // _RWSTD_NO_WCHAR_T

    return 0;

}

/***********************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "istream.sentry",
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
