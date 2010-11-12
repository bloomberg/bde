/************************************************************************
 *
 * 27.istream.unformatted.get.cpp - test exercising overloads of
 *                                  basic_istream::get()
 *
 * $Id: 27.istream.unformatted.get.cpp 590052 2007-10-30 12:44:14Z faridz $
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
 * Copyright 2003-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <fstream>
#include <istream>
#include <sstream>
#include <cstddef>          // for size_t

#include <driver.h>         // for rw_test(), ...
#include <rw_char.h>        // for UserTraits
#include <rw_streambuf.h>   // for MyStreambuf

/***********************************************************************/

#define Bad   std::ios_base::badbit
#define Eof   std::ios_base::eofbit
#define Fail  std::ios_base::failbit
#define Good  std::ios_base::goodbit


template <class charT, class Traits>
void
check_failure (const MyStreambuf<charT, Traits>&  sb,
               const char*                        cname,
               int                                lineno,
               const char*                        fun,
               std::basic_istream<charT, Traits>& strm,
               int                                gcount,
               int                                nexpect,
               int                                rdstate,
               int                                exceptions,
               int                                caught)
{
    static const char* const exnames[] = {
        "none", "std::ios_base::failure",
        "streambuf_exception", "unknown exception"
    };

    const char* const caught_what = exnames [caught];

    // verify that stream is in the expected state
    rw_assert (strm.rdstate () == rdstate, 0, lineno,
               "line %d: basic_istream<%s>::%s; "
               "rdstate () == %{Is}, got %{Is}",
               __LINE__, cname, fun, rdstate, strm.rdstate ());


    if (sb.throw_set_ && sb.ncalls (sb.threw_) == sb.fail_when_) {

        // verify that the same exception (and not ios_base::failure)
        // as the one thrown from basic_filebuf has been propagated
        // and caught when badbit is set in exceptions, and that no
        // exception has been thrown if exceptions is clear
        if (exceptions & Bad && caught != 2) {

            rw_assert (false, 0, lineno,
                       "line %d: basic_istream<%s>::%s failed "
                       "to propagate an exception thrown "
                       "by basic_filebuf; caught %s instead",
                       __LINE__, cname, fun,
                       caught_what);
        }
        else if (!(exceptions & Bad) && caught) {

            rw_assert (false, 0, lineno,
                       "line %d: basic_istream<%s>::%s propagated "
                       "an exception thrown by basic_filebuf when "
                       "ios_base::badbit is clear in exceptions",
                       __LINE__, cname, fun);
        }

        // verify that the original gcount of 0 was unaffected
        // (for non-array forms of unformatted input functions)
        // or that it increased exactly `fail_when_'
        const int actual = strm.gcount () - gcount;

        rw_assert (!actual || actual == sb.fail_when_ - 1, 0, lineno,
                   "line %d: basic_istream<%s>::%s changed "
                   "gcount () from %d to %d after a failure; "
                   "either no change from 0 expected, or "
                   "an increase to %d from 0 expected",
                   __LINE__, cname, fun,
                   gcount, strm.gcount (), sb.fail_when_ - 1);
    }
    else {

        // verify that ios_base::failure has been thrown (and caught)
        // if badbit is set in exceptions
        rw_assert (!(exceptions & strm.rdstate ()) || caught == 1,
                   0, lineno,
                   "line %d: basic_istream<%s>::%s set %{Is} "
                   "but failed to throw ios_base::failure "
                   "when the same bit is set in exceptions",
                   __LINE__, cname, fun, rdstate);

        // verify that gcount has the expected value

        const int actual = strm.gcount () - gcount;

        rw_assert (nexpect == actual, 0, lineno,
                   "line %d: basic_istream<%s>::%s changed "
                   "gcount () from %d to %d after a successful "
                   "extraction; a change from 0 to %d expected",
                   __LINE__, cname, fun,
                   gcount, strm.gcount (), nexpect);
    }
}

/***********************************************************************/

#define TRY_GET(expr)                         \
    try {                                     \
        expr;                                 \
    }                                         \
    catch (const std::ios_base::failure&) {   \
        caught = 1;                           \
    }                                         \
    catch (Exception& e) {                    \
        caught = ex_stream == e.id_ ? 2 : 3;  \
    }                                         \
    catch (...) {                             \
        caught = 3;                           \
    } (void)0


/***********************************************************************/


// 27.6.1.3   Unformatted input functions   [lib.istream.unformatted]
//
// -1-  Each unformatted input function begins execution by constructing
//      an object of class sentry with the default argument noskipws
//      (second) argument true. If the sentry object returns true, when
//      converted to a value of type bool, the function endeavors to
//      obtain the requested input. If an exception is thrown during
//      input then ios::badbit is turned on284) in *this?s error state.
//      If (exception() & badbit) != 0 then the exception is rethrown.
//      It also counts the number of characters extracted. If no exception
//      has been thrown it ends by storing the count in a member object
//      and returning the value specified. In any event the sentry object
//      is destroyed before leaving the unformatted input function.

template <class charT, class Traits>
void
test_get_void (charT, const char *cname,
               const Traits*, int lineno,
               const char  *str,
               std::size_t  strsize,
               int          ival,
               int          rdstate,
               int          exceptions,
               int          failure,
               int          fail_when)
{
    // int_type get();
    // -3-  Effects: Extracts a character c, if one is available.
    //          Otherwise, the function calls setstate(failbit),
    //          which may throw ios_base::failure (27.4.4.3),
    // -4-  Returns: c if available, otherwise traits::eof().

    typedef MyStreambuf<charT, Traits>        Streambuf;
    typedef std::basic_istream<charT, Traits> Istream;
    typedef typename Istream::int_type        int_type;

    const int_type val = int_type (ival);

    // create a test streambuf object initialized with `str'
    Streambuf sb (str, strsize, failure, fail_when);

    Istream strm (&sb);

    strm.exceptions (std::ios_base::iostate (exceptions));

    const std::streamsize gcount = strm.gcount ();

    int_type got = Traits::eof ();

    int caught = 0;

    TRY_GET (got = strm.get ());

    check_failure (sb, cname, lineno, "get()", strm, gcount, !rdstate,
                   rdstate, exceptions, caught);

    // verify that the extracted value is the one expected

    if (!sb.throw_set_ && !caught && val != got) {

        const charT cval = Traits::to_char_type (val);
        const charT cgot = Traits::to_char_type (got);

        rw_assert (false, __FILE__, lineno,
                   "line %d: basic_istream<%s>::get () == %{#lc}, got %{#lc}",
                   __LINE__, cname, cval, cgot);
    }

    rw_assert (val == Traits::eof () || !(gcount || 1 != strm.gcount ()),
               0, lineno,
               "line %d: basic_istream<%s>::get () changed "
               "gcount () from %d to %d after a successful "
               "extraction; 0 to 1 expected",
               __LINE__, cname,
               gcount, strm.gcount ());
}

/***********************************************************************/

template <class charT>
void test_get_void (charT, const char *cname)
{
#define T      charT (), cname, (UserTraits<charT>*)0, __LINE__
#define TEST   test_get_void

    rw_info (0, 0, 0, "27.6.1.3, p3 - std::basic_istream<%s>::get ()",
             cname);

    typename UserTraits<charT>::int_type &eof =
        UserTraits<charT>::eof_;

    eof = ~0;

    TEST (T, "",      0, eof,  Eof | Fail, Good, 0, -1);
    TEST (T, "",      1, '\0', Good,       Good, 0, -1);
    TEST (T, "a",     1, 'a',  Good,       Good, 0, -1);
    TEST (T, " b",    2, ' ',  Good,       Good, 0, -1);
    TEST (T, "\x80",  1, 0x80, Good,       Good, 0, -1);
    TEST (T, "\xff ", 2, 0xff, Good,       Good, 0, -1);

    TEST (T, "c",     1, eof,  Eof | Fail, Good, Underflow, 1);
    TEST (T, "d",     1, eof,  Bad,        Good, Underflow | Throw, 1);

    eof = 256;

    TEST (T, "",  0, eof, Eof | Fail, Good, 0, 0);
    TEST (T, "f", 1, 'f', Good,       Good, 0, 0);
    TEST (T, "g", 1, eof, Eof | Fail, Good, Underflow, 1);
    TEST (T, "h", 1, eof, Bad,        Good, Underflow | Throw, 1);
    
    eof = '\0';

    TEST (T, "\0", 1, eof, Eof | Fail, Good, 0, -1);
    TEST (T, "\1", 1, eof, Eof | Fail, Good, Underflow, 1);
    TEST (T, "\2", 1, eof, Bad,        Good, Underflow | Throw, 1);
}

/***********************************************************************/

template <class charT, class Traits>
void test_get_char (charT, const char* cname,
                    const Traits*, int lineno,
                    const char  *str,
                    std::size_t  strsize,
                    char         initval,
                    char         expectval,
                    int          rdstate,
                    int          exceptions,
                    int          failure,
                    int          fail_when)
{
    // basic_istream<charT, Traits>& get (char_type&);
    // -5-  Effects: Extracts a character, if one is available,
    //          and assigns it to c.(285) Otherwise, the function calls
    //          setstate(failbit) (which may throw ios_base::failure
    //          (27.4.4.3)).
    // -6-  Returns: *this.
    // ---
    // 285) Note that this function is not overloaded on types signed
    //      charand unsigned char.

    typedef MyStreambuf<charT, Traits>        Streambuf;
    typedef std::basic_istream<charT, Traits> Istream;
    typedef typename Istream::int_type        int_type;

    // create a test streambuf object initialized with `str'
    Streambuf sb (str, strsize, failure, fail_when);

    Istream strm (&sb);

    strm.exceptions (std::ios_base::iostate (exceptions));

    const std::streamsize gcount = strm.gcount ();

    typedef unsigned char UChar;

          charT got = charT (UChar (initval));
    const charT val = charT (UChar (expectval));

    int caught = 0;

    TRY_GET (strm.get (got));

    check_failure (sb, cname, lineno, "get(char_type&)", strm,
                   gcount, !rdstate, rdstate, exceptions, caught);

    // verify that the extracted value is the one expected

    rw_assert (sb.throw_set_ || caught || Traits::eq (val, got), 0, lineno,
               "line %d: basic_istream<%s>::get (char_type& = %{#lc}),"
               " expected %{#lc}", __LINE__, cname, got, val);

    rw_assert (rdstate || !(gcount || 1 != strm.gcount ()), 0, lineno,
               "line %d: basic_istream<%s>::get (char_type&) "
               "changed gcount () from %d to %d after a successful "
               "extraction; 0 to 1 expected",
               __LINE__, cname,
               gcount, strm.gcount ());
}

/***********************************************************************/

template <class charT>
void test_get_char (charT, const char *cname)
{
#undef T
#undef TEST

#define T      charT (), cname, (UserTraits<charT>*)0, __LINE__
#define TEST   test_get_char

    rw_info (0, 0, 0,
             "27.6.1.3, p5 - std::basic_istream<%s>::get (char_type&)",
             cname);

    typename UserTraits<charT>::int_type &eof =
        UserTraits<charT>::eof_;

    eof = ~0;

    //       +- initial stream buffer contents (will be widened)
    //       |     +- the length of the buffer contents in chars
    //       |     |  +- initial value of get(char_type&) argument
    //       |     |  |     +- expected value of argument on return
    //       |     |  |     |     +- expected stream state on return
    //       |     |  |     |     |           +- stream exceptions
    //       |     |  |     |     |           |     +- force failure
    //       |     |  |     |     |           |     |  +- when to fail
    //       |     |  |     |     |           |     |  |
    //       v     v  v     v     v           v     v  v
    TEST (T, "",   0, '\0', '\0', Eof | Fail, Good, 0, -1);
    TEST (T, "",   1, '\1', '\0', Good,       Good, 0, -1);
    TEST (T, "1",  1, '\2', '1',  Good,       Good, 0, -1);
    TEST (T, "2 ", 2, '\3', '2',  Good,       Good, 0, -1);
    TEST (T, " 3", 2, '\4', ' ',  Good,       Good, 0, -1);

    TEST (T, "4",  1, '\5', '\5', Eof | Fail, Good, Underflow, 1);
    TEST (T, "5",  1, '\6', '\6', Bad,        Good, Underflow | Throw, 1);
    TEST (T, "6",  1, '\7', '\6', Eof | Fail, Eof,  Underflow, 1);
    TEST (T, "7",  1, 'x',   'x', Eof | Fail, Fail, Underflow, 1);
    TEST (T, "8",  1, 'y',   'y', Eof | Fail, Eof | Fail, Underflow, 1);
    TEST (T, "9",  1, 'z',   'z', Eof | Fail, Eof | Fail | Bad, Underflow, 1);

    TEST (T, "A",  1, 'a',   'a', Bad,        Bad, Underflow | Throw, 1);
}

/***********************************************************************/

#define MAXCHARS   (4096U * 4U)


template <class charT, class Traits>
void test_get_char_array (charT, const char* cname,
                          const Traits*,
                          int          lineno,
                          const char  *str,
                          std::size_t  strsize,
                          int          nread,
                          int          idelim,
                          int          nexpect,
                          int          rdstate,
                          int          exceptions,
                          int          failure,
                          int          fail_when)
{
    // basic_istream<charT, traits>&
    // get(char_type* s, streamsize n, char_type delim);
    //
    // -7-  Effects: Extracts characters and stores them into successive
    //      locations of an array whose first element is designated by s.
    //      286) Characters are extracted and stored until any of the
    //      following occurs:
    //      -  (n - 1) characters are stored;
    //      -  end-of-file occurs on the input sequence (in which case
    //         the function calls setstate(eofbit));
    //      -  (c == delim) for the next available input character c
    //         (in which case cis not extracted).
    //
    // -8-  If the function stores no characters, it calls setstate(failbit)
    //      (which may throw ios_base::failure (27.4.4.3)). In any case, it
    //      then stores a null character into the next successive location
    //      of the array.
    //
    // -9-  Returns: *this.

    // basic_istream<charT,traits>&
    // get(char_type* s, streamsize n);
    //
    // -10-  Effects: Calls get(s, n, widen('\n')).
    // -11-  Returns: Value returned by the call.

    typedef MyStreambuf<charT, Traits>        Streambuf;
    typedef std::basic_istream<charT, Traits> Istream;
    typedef typename Istream::int_type        int_type;

    const int_type delim = int_type (idelim);

    // create a test streambuf object initialized with `str'
    Streambuf sb (str, strsize, failure, fail_when);

    Istream strm (&sb);

    strm.exceptions (std::ios_base::iostate (exceptions));

    const std::streamsize gcount = strm.gcount ();

    typedef unsigned char UChar;

    int caught = 0;

    charT got [MAXCHARS + 1];
    Traits::assign (got, sizeof got / sizeof *got, charT ('\xfe'));

    const char *fun;

    if (delim != Traits::eof ()) {
        fun = "get(char_type*, streamsize, char_type)";
        TRY_GET (strm.get (got, std::streamsize (nread), delim));
    }
    else {
        fun = "get(char_type*, streamsize)";
        TRY_GET (strm.get (got, std::streamsize (nread)));
    }

    check_failure (sb, cname, lineno, fun, strm, gcount, nexpect,
                   rdstate, exceptions, caught);

    // verify that number and values of the extracted characters
    // matches the expected number and values

    rw_assert (strm.gcount () == nexpect, 0, lineno,
               "line %d: basic_istream<%s>::get (char_type*, "
               "streamsize = %d).gcount () == %d, got %d",
               __LINE__, cname, nread, nexpect, strm.gcount ());

    rw_assert (1 < nread || Traits::eq (got [0], charT ()), 0, lineno,
               "line %d: basic_istream<%s>::get (char_type *s "
               "= %{*Ac}, streamsize = %d); s[0] == '\\0', got %{#lc}",
               __LINE__, cname, int (sizeof *got), got, nread, *got);

    rw_assert (!Traits::compare (sb.buf_, got, std::size_t (nexpect)),
               0, lineno,
               "line %d: basic_istream<%s>::get (char_type* "
               "= %{*Ac}, streamsize = %d) data mismatch",
               __LINE__, cname, int (sizeof *got), got, nread);
}

/***********************************************************************/

template <class charT>
void test_get_char_array (charT, const char *cname)
{
#undef T
#undef TEST

#define T      charT (), cname, (UserTraits<charT>*)0, __LINE__
#define TEST   test_get_char_array

    rw_info (0, 0, 0, "27.6.1.3, p5 - std::basic_istream<%s>"
             "::get (char_type*, streamsize)", cname);

    typename UserTraits<charT>::int_type &eof =
        UserTraits<charT>::eof_;

    eof = ~0;

    //       +- initial stream buffer contents (will be widened)
    //       |        +- the length of the buffer contents in chars
    //       |        |  +- number of characters (2nd argument to get())
    //                |  |  +- delimiter (3rd argument to get() or eof)
    //       |        |  |  |    +- expected number of extracted characters
    //       |        |  |  |    |  +- expected stream state on return
    //       |        |  |  |    |  |           +- stream exceptions
    //       |        |  |  |    |  |           |     +- force failure
    //       |        |  |  |    |  |           |     |  +- when to fail
    //       |        |  |  |    |  |           |     |  |
    //       v        v  v  v    v  v           v     v  v
    TEST (T, "",      0, 0, eof, 0, Fail,       Good, 0, -1);
    TEST (T, "\0",    1, 1, eof, 0, Fail,       Good, 0, -1);
    TEST (T, "1",     1, 2, eof, 1, Good,       Good, 0, -1);
    TEST (T, "2",     1, 3, eof, 1, Eof,        Good, 0, -1);
    TEST (T, "23",    2, 3, eof, 2, Good,       Good, 0, -1);
    TEST (T, "34",    2, 4, eof, 2, Eof,        Good, 0, -1);
    TEST (T, "4\n",   2, 3, eof, 1, Good,       Good, 0, -1);
    TEST (T, "56\n",  3, 3, eof, 2, Good,       Good, 0, -1);
    TEST (T, "67\n",  3, 4, eof, 2, Good,       Good, 0, -1);

    // exercise the behavior on underflow() failure indicated
    // by returning traits_type::eof()
    TEST (T, "78\n9", 4, 4, eof, 0, Eof | Fail, Good,        Underflow, 1);
    TEST (T, "78901", 5, 5, eof, 0, Eof | Fail, Eof,         Underflow, 1);
    TEST (T, "78902", 5, 5, eof, 0, Eof | Fail, Fail,        Underflow, 1);
    TEST (T, "78803", 5, 5, eof, 0, Eof | Fail, Eof | Fail,  Underflow, 1);
    TEST (T, "89\na", 4, 4, eof, 1, Eof,        Good,        Underflow, 2);
    TEST (T, "9a\nb", 4, 4, eof, 2, Eof,        Good,        Underflow, 3);
    TEST (T, "ab\nc", 4, 4, eof, 2, Good,       Good,        Underflow, 4);

    // exercise the behavior on underflow() failure caused
    // by throwing an exception
    TEST (T, "bc\nd", 4, 4, eof, 0, Bad,        Good, Underflow | Throw, 1);
    TEST (T, "cd\ne", 4, 4, eof, 1, Bad,        Good, Underflow | Throw, 2);
    TEST (T, "def\n", 4, 4, eof, 2, Bad,        Good, Underflow | Throw, 3);
    TEST (T, "efg\n", 4, 4, eof, 2, Bad,        Bad,  Underflow | Throw, 3);
    TEST (T, "fgh\n", 4, 4, eof, 3, Good,       Bad,  Underflow | Throw, 4);
    TEST (T, "ghij",  4, 4, eof, 3, Good,       Bad,  Underflow | Throw, 4);
    TEST (T, "hijk",  4, 5, eof, 3, Bad,        Bad,  Underflow | Throw, 4);

    const std::streamsize N = std::streamsize (MAXCHARS);
    char *buf = new char [N];

    for (std::streamsize i = 0; i != N; ++i) {
        buf [i] = char (i);
    }

    // set the maximum number of characters in the pending input
    // sequence to be greater than the default 1 to allow get()
    // not to have to call underflow() on every iteration and
    // to exercise any/all branches in the function
    MyStreambuf<charT, UserTraits<charT> >::in_pending_ = N / 33;

    TEST (T, "12345", 5, 4, eof, 3, Good, Good, 0, -1);
    TEST (T, "12346", 5, 5, eof, 4, Good, Good, 0, -1);
    TEST (T, "12347", 5, 6, eof, 5, Good, Good, 0, -1);
    TEST (T, "12348", 5, 7, eof, 5, Eof,  Good, 0, -1);
    
    for (std::streamsize i = 0; i < N; i += 256) {
        TEST (T, buf, N, N, eof, '\n' + i, Good, Good, 0, -1);

        buf ['\n' + i] = '\0';
    }

    rw_info (0, 0, 0, "27.6.1.3, p5 - std::basic_istream<%s>"
             "::get (char_type*, streamsize, char_type)", cname);

    TEST (T, "",   0, 0, '\0', 0, Fail,       Good, 0, -1);
    TEST (T, "",   1, 1, '\0', 0, Fail,       Good, 0, -1);
    TEST (T, "a",  1, 2, '\0', 1, Good,       Good, 0, -1);
    TEST (T, "bc", 2, 2, 'c',  1, Good,       Good, 0, -1);
    TEST (T, "cd", 2, 3, 'd',  1, Good,       Good, 0, -1);
    TEST (T, "de", 2, 3, 'd',  0, Fail,       Good, 0, -1);
}

/***********************************************************************/

template <class charT, class Traits>
void test_get_streambuf (charT, const char* cname,
                         const Traits*,
                         int          lineno,
                         const char  *str,
                         std::size_t  strsize,
                         int          idelim,
                         int          nexpect,
                         int          rdstate,
                         int          exceptions,
                         int          failure,
                         int          fail_when)
{
    typedef MyStreambuf<charT, Traits>        Streambuf;
    typedef std::basic_istream<charT, Traits> Istream;
    typedef typename Istream::int_type        int_type;

    const int_type delim = int_type (idelim);

    int in_failure, in_fail_when;
    int out_failure, out_fail_when;

    switch (failure & ~Throw) {
    case Xsgetn: case Underflow: case Uflow:
        in_failure = failure;
        in_fail_when = fail_when;
        out_failure = 0;
        out_fail_when = 0;
        break;

    case Overflow: case Xsputn:
        in_failure = 0;
        in_fail_when = 0;
        out_failure = failure;
        out_fail_when = fail_when;
        break;

    default:
        in_failure = failure;
        in_fail_when = fail_when;
        out_failure = failure;
        out_fail_when = fail_when;
    }

    // create a test streambuf object initialized with `str'
    Streambuf inbuf (str, strsize, in_failure, in_fail_when);

    Istream strm (&inbuf);

    strm.exceptions (std::ios_base::iostate (exceptions));

    const std::streamsize gcount = strm.gcount ();

    typedef unsigned char UChar;

    int caught = 0;

    Streambuf outbuf (strsize, out_failure, out_fail_when);

    const char *fun;

    if (delim != Traits::eof ()) {
        fun = "get(basic_streambuf<char_type, traits_type>&, char_type)";
        TRY_GET (strm.get (outbuf, delim));
    }
    else {
        fun = "get(basic_streambuf<char_type, traits_type>&)";
        TRY_GET (strm.get (outbuf));
    }

    check_failure (inbuf, cname, lineno, fun, strm, gcount, nexpect,
                   rdstate, exceptions, caught);

    // verify that number and values of the extracted characters
    // matches the expected number and values

    rw_assert (strm.gcount () == nexpect, 0, lineno,
               "line %d: basic_istream<%s>::get (basic_streambuf"
               "<char_type, traits_type>*).gcount () == %d, got %d",
               __LINE__, cname, nexpect, strm.gcount ());

    rw_assert (!Traits::compare (inbuf.buf_, outbuf.buf_,
                                 std::size_t (nexpect)), 0, lineno,
               "line %d: basic_istream<%s>::get (basic_streambuf"
               "<char_type, traits_type>*) data mismatch",
               __LINE__, cname);
}

/***********************************************************************/

template <class charT>
void test_get_streambuf (charT, const char *cname)
{
#undef T
#undef TEST

#define T      charT (), cname, (UserTraits<charT>*)0, __LINE__
#define TEST   test_get_streambuf

    rw_info (0, 0, 0, "27.6.1.3, p12 - std::basic_istream<%s>"
             "::get (basic_streambuf<char_type, traits_type>*, char_type)",
             cname);

    typename UserTraits<charT>::int_type &eof =
        UserTraits<charT>::eof_;

    eof = ~0;

    // set the maximum number of characters in the pending input
    // sequence to 1 to force a call to underflow() for each
    // character
    MyStreambuf<charT, UserTraits<charT> >::in_pending_ = 1;

    // basic_istream<charT, traits>&
    // get (basic_streambuf<charT, traits> &sb, char_type delim);
    //
    // -12-  Effects: Extracts characters and inserts them in the output
    //       sequence controlled by sb. Characters are extracted and
    //       inserted until any of the following occurs:
    //       -  end-of-file occurs on the input sequence;
    //       -  inserting in the output sequence fails (in which case the
    //          character to be inserted is not extracted);
    //       -  (c == delim) for the next available input character c
    //          (in which case cis not extracted);
    //       -  an exception occurs (in which case, the exception is caught
    //          but not rethrown).
    // -13-  If the function inserts no characters, it calls
    //       setstate(failbit), which may throw ios_base::failure (27.4.4.3).
    // -14-  Returns: *this.

    TEST (T, "",      0, 'x',  0, Eof | Fail, Good, 0, 0);
    TEST (T, "\0",    1, 'y',  1, Eof,        Good, 0, 0);
    TEST (T, "ab",    2, 'z',  2, Eof,        Good, 0, 0);
    TEST (T, "abc",   3, '\0', 3, Eof,        Good, 0, 0);
    TEST (T, "b\0c",  3, '\0', 1, Good,       Good, 0, 0);
    TEST (T, "cd\0e", 4, '\0', 2, Good,       Good, 0, 0);
    TEST (T, "def\0", 4, '\0', 3, Good,       Good, 0, 0);

    TEST (T, "efgh",  4, 'f',  0, Eof | Fail, Good,       Underflow, 1);
    TEST (T, "fghi",  4, 'g',  0, Eof | Fail, Eof,        Underflow, 1);
    TEST (T, "ghij",  4, 'h',  0, Eof | Fail, Fail,       Underflow, 1);
    TEST (T, "hijk",  4, 'i',  0, Eof | Fail, Eof | Fail, Underflow, 1);

    TEST (T, "ijkl",  4, 'k',  1, Eof,        Good, Underflow, 2);
    TEST (T, "jklm",  4, 'm',  2, Eof,        Good, Underflow, 3);

    TEST (T, "klmn",  4, 'n',  2, Bad,        Good, Underflow | Throw, 3);
    TEST (T, "lmno",  4, 'o',  2, Bad,        Bad,  Underflow | Throw, 3);

    TEST (T, "EFGH",  4, 'F',  0, Fail,       Good,       Overflow, 1);
    TEST (T, "FGHI",  4, 'G',  0, Fail,       Eof,        Overflow, 1);
    TEST (T, "GHIJ",  4, 'H',  0, Fail,       Fail,       Overflow, 1);
    TEST (T, "HIJK",  4, 'I',  0, Fail,       Eof | Fail, Overflow, 1);

    TEST (T, "IJKL",  4, 'K',  1, Fail,       Good, Overflow, 2);
    TEST (T, "JKLM",  4, 'M',  2, Fail,       Good, Overflow, 3);

    TEST (T, "KLMN",  4, 'N',  2, Fail,       Good, Overflow | Throw, 3);
    TEST (T, "LMNO",  4, 'O',  2, Fail,       Bad,  Overflow | Throw, 3);

    rw_info (0, 0, 0, "27.6.1.3, p15 - std::basic_istream<%s>"
             "::get (basic_streambuf<char_type, traits_type>&)", cname);

    TEST (T, "",      0, eof, 0, Eof | Fail, Good, 0, 0);
    TEST (T, "\n",    1, eof, 0, Fail,       Good, 0, 0);
    TEST (T, "a\n",   2, eof, 1, Good,       Good, 0, 0);
    TEST (T, "ab\n",  3, eof, 2, Good,       Good, 0, 0);
}

/***********************************************************************/

static int rw_opt_no_get_void;
static int rw_opt_no_get_char;
static int rw_opt_no_get_char_array;
static int rw_opt_no_get_streambuf;


static int
run_test (int, char**)
{
#undef TEST
#define TEST(T, arg)                                            \
    if (rw_opt_no_get_ ## arg)                                  \
        rw_note (0, 0, 0,                                       \
                 "basic_istream::get(%s) test disabled", #arg); \
    else                                                        \
        test_get_ ## arg (T (), #T)


    TEST (char, void);
    TEST (char, char);
    TEST (char, char_array);
    TEST (char, streambuf);

#ifndef _RWSTD_NO_WCHAR_T

    TEST (wchar_t, void);
    TEST (wchar_t, char);
    TEST (wchar_t, char_array);
    TEST (wchar_t, streambuf);

#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}

/***********************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.istream.unformatted",
                    "overloads of get() member function",
                    run_test,
                    "|-no-get-void#"
                    "|-no-get-char#"
                    "|-no-get-char_array#"
                    "|-no-get-streambuf#",
                    &rw_opt_no_get_void,
                    &rw_opt_no_get_char,
                    &rw_opt_no_get_char_array,
                    &rw_opt_no_get_streambuf,
                    0);
}
