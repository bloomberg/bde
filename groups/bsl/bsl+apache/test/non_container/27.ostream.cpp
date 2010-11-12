/***************************************************************************
 *
 * 27.ostream.cpp - test exercising class template basic_ostream
 *
 * $Id: 27.ostream.cpp 580483 2007-09-28 20:55:52Z sebor $
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
 * Copyright 2002-2006 Rogue Wave Software.
 * 
 **************************************************************************/


#include <exception>   // for uncaught_exception()
#include <ostream>     // for basic_ostream
#include <climits>     // for XXX_MAX and XXX_MIN
#include <cstddef>     // for ptrdiff_t
#include <cstring>     // for memset()

#include <any.h>       // for rw_any_t()
#include <driver.h>    // for rw_test()

/***************************************************************************/

int rw_opt_sentry;          // for --enable/disable-sentry
int rw_opt_formatted;       // for --enable/disable-formatted
int rw_opt_unformatted;     // for --enable/disable-unformatted
int rw_opt_flush;           // for --enable/disable-flush
int rw_opt_exceptions;      // for --enable/disable-exceptions

/***************************************************************************/

enum streambuf_failure {
    Setbuf = 1, Seekoff, Seekpos, Shwomanyc, Xsgetn,
    Underflow, Uflow, Overflow, Pbackfail, Xsputn, Sync
};


template <class charT>
struct test_streambuf: std::basic_streambuf<charT, std::char_traits<charT> >
{
    typedef charT                                        char_type;
    typedef std::char_traits<charT>                      traits_type;
    typedef std::basic_streambuf<char_type, traits_type> Streambuf;
    typedef typename Streambuf::int_type                 int_type;
    typedef typename Streambuf::off_type                 off_type;
    typedef typename Streambuf::pos_type                 pos_type;

    test_streambuf (std::basic_ios<char_type, traits_type> *pstrm)
        : Streambuf (), buf_ (0), bufsize_ (0),
          throws_ (streambuf_failure ()), fails_ (streambuf_failure ()),
          pstrm_ (pstrm) {
        std::memset (ncalls_, 0, sizeof ncalls_);
        this->setp (buf_, buf_);
    }

    test_streambuf (charT *buf, std::streamsize bufsize)
        : Streambuf (), buf_ (buf), bufsize_ (bufsize),
          throws_ (streambuf_failure ()), fails_ (streambuf_failure ()) {
        std::memset (ncalls_, 0, sizeof ncalls_);
        this->setp (buf_, buf_);
    }

    bool test (streambuf_failure) const;

    virtual Streambuf* setbuf (char_type*, std::streamsize) {
        return test (Setbuf) ? this : 0;
    }

    virtual pos_type
    seekoff (off_type, std::ios_base::seekdir, std::ios_base::openmode) {
        test (Seekoff);
        return pos_type (off_type (-1));
    }

    virtual pos_type
    seekpos (pos_type, std::ios_base::openmode) {
        test (Seekpos);
        return pos_type (off_type (-1));
    }

    virtual std::streamsize showmanyc () {
        test (Shwomanyc);
        return 0;
    }

    virtual std::streamsize xsgetn (char_type*, std::streamsize) {
        test (Xsgetn);
        return 0;
    }

    virtual int_type underflow () {
        test (Underflow);
        return traits_type::eof ();
    }

    virtual int_type uflow () {
        test (Uflow);
        return traits_type::eof ();
    }

    virtual int_type
    overflow (int_type = traits_type::eof ());

    virtual int_type
    pbackfail (int_type = traits_type::eof ()) {
        test (Pbackfail);
        return traits_type::eof ();
    }

    virtual std::streamsize
    xsputn (const char_type *buf, std::streamsize bufsize) {
        if (!test (Xsputn))
            return 0;
        return Streambuf::xsputn (buf, bufsize);
    }

    virtual int sync () {
        if (!test (Sync))
            return -1;

        if (pstrm_)
            pstrm_->setstate (std::ios_base::failbit);

        return 0;
    }

    char_type       *buf_;
    std::streamsize  bufsize_;

    int ncalls_ [Sync + 1];     // number of calls made to each function

    streambuf_failure throws_;  // exception value to throw
    streambuf_failure fails_;   // which function to fail

    std::basic_ios<char_type, traits_type> *pstrm_;

    static int fail_when_;      // call number on which to fail
};


template <class charT>
int test_streambuf<charT>::fail_when_ = 1;


template <class charT>
bool
test_streambuf<charT>::test (streambuf_failure which) const
{
    ++_RWSTD_CONST_CAST (test_streambuf*, this)->ncalls_ [which];

    if (ncalls_ [which] == fail_when_ && throws_ == which)
        throw which;

    return fails_ != which || ncalls_ [which] != fail_when_;
}


template <class charT>
typename test_streambuf<charT>::int_type
test_streambuf<charT>::overflow (int_type c /* = traits_type::eof () */)
{
    if (!test (Overflow))
        return traits_type::eof ();

    if (traits_type::eq_int_type (c, traits_type::eof ()))
        return traits_type::not_eof (c);

    const std::ptrdiff_t pptr_off = this->pptr () - this->pbase ();

    if (   this->pptr () == this->epptr ()
        && this->epptr () < buf_ + bufsize_) {
        this->setp (buf_, this->epptr () + 1);
        this->pbump (pptr_off + 1);
        *(this->pptr () - 1) = traits_type::to_char_type (c);
        return c;
    }

    return traits_type::eof ();
}



enum num_put_overload {
    Bool = Sync + 1, Long, ULong, Dbl, LDbl, PVoid, LLong, ULLong
};


#ifndef _RWSTD_NO_NATIVE_BOOL

num_put_overload select_overload (bool)           { return Bool; }

#endif   // _RWSTD_NO_NATIVE_BOOL

template <class T>
num_put_overload select_overload (T) { return num_put_overload (); }

num_put_overload select_overload (short)          { return Long; }
num_put_overload select_overload (unsigned short) { return ULong; }
num_put_overload select_overload (int)            { return Long; }
num_put_overload select_overload (unsigned int)   { return ULong; }
num_put_overload select_overload (long)           { return Long; }
num_put_overload select_overload (unsigned long)  { return ULong; }

#ifdef _RWSTD_LONG_LONG

num_put_overload select_overload (_RWSTD_LONG_LONG)          { return Long; }
num_put_overload select_overload (unsigned _RWSTD_LONG_LONG) { return ULong; }

#endif

num_put_overload select_overload (float)          { return Dbl; }
num_put_overload select_overload (double)         { return Dbl; }

#ifndef _RWSTD_NO_LONG_DOUBLE

num_put_overload select_overload (long double)    { return LDbl; }

#endif   // _RWSTD_NO_LONG_DOUBLE

num_put_overload select_overload (const void*)    { return PVoid; }


template <class charT>
struct test_num_put: std::num_put<charT, std::ostreambuf_iterator<charT> >
{
    typedef charT                           char_type;
    typedef std::ostreambuf_iterator<charT> iter_type;
    typedef std::num_put<charT, iter_type>  NumPut;
    typedef std::ios_base                   Ios;

    explicit test_num_put (std::size_t refs = 0)
        : NumPut (refs),
          throws_ (num_put_overload ()),
          fails_ (num_put_overload ()) {
        std::memset (ncalls_, 0, sizeof ncalls_);
    }

    iter_type test (iter_type, num_put_overload) const;

#define TEST(it, val)   test (it, select_overload (val))

#ifndef _RWSTD_NO_NATIVE_BOOL

    virtual iter_type
    do_put (iter_type it, Ios &fl, char_type fill, bool val) const {
        return NumPut::do_put (TEST (it, val), fl, fill, val);
    }

#endif   // _RWSTD_NO_NATIVE_BOOL

    virtual iter_type
    do_put (iter_type it, Ios &fl, char_type fill, long val) const {
        return NumPut::do_put (TEST (it, val), fl, fill, val);
    }

    virtual iter_type
    do_put (iter_type it, Ios &fl, char_type fill, unsigned long val) const {
        return NumPut::do_put (TEST (it, val), fl, fill, val);
    }

    virtual iter_type
    do_put (iter_type it, Ios &fl, char_type fill, double val) const {
        return NumPut::do_put (TEST (it, val), fl, fill, val);
    }

    virtual iter_type
    do_put (iter_type it, Ios &fl, char_type fill, long double val) const {
        return NumPut::do_put (TEST (it, val), fl, fill, val);
    }

    virtual iter_type
    do_put (iter_type it, Ios &fl, char_type fill, const void *val) const {
        return NumPut::do_put (TEST (it, val), fl, fill, val);
    }

#ifdef _RWSTD_LONG_LONG

    // extensions
    virtual iter_type
    do_put (iter_type it, Ios &fl, char_type fill,
            _RWSTD_LONG_LONG val) const {
        return NumPut::do_put (TEST (it, val), fl, fill, val);
    }

    virtual iter_type
    do_put (iter_type it, Ios &fl, char_type fill,
            unsigned _RWSTD_LONG_LONG val) const {
        return NumPut::do_put (TEST (it, val), fl, fill, val);
    }

#endif   // _RWSTD_LONG_LONG

#undef TEST

    int ncalls_ [ULLong + 1];
    num_put_overload throws_;
    num_put_overload fails_;
};


template <class charT>
typename test_num_put<charT>::iter_type
test_num_put<charT>::test (iter_type it, num_put_overload which) const
{
    ++_RWSTD_CONST_CAST (test_num_put*, this)->ncalls_ [which];

    if (throws_ == which)
        throw which;

    if (fails_ == which) {

        // construct an iter_type object in a failed state
        test_streambuf<charT> tsb (0, 0);
        iter_type failed (&tsb);
        failed = char_type ();

        _RWSTD_ASSERT (failed.failed ());

        return failed;
    }

    return it;
}

/***************************************************************************/

bool uncaught = false;


template <class charT>
struct DtorChek
{
    typedef std::basic_ostream<charT, std::char_traits<charT> > Ostream;
    typedef typename Ostream::sentry                            Sentry;

    Ostream *pstrm_;

    ~DtorChek () {

#ifndef _RWSTD_UNCAUGHT_EXCEPTION

        uncaught = std::uncaught_exception ();

#endif   // _RWSTD_UNCAUGHT_EXCEPTION

        const Sentry guard (*pstrm_);
    }
};


template <class charT>
void test_sentry (charT)
{
    static const char* const cname = rw_any_t (charT ()).type_name ();

    typedef std::basic_ostream<charT, std::char_traits<charT> > Ostream;
    typedef typename Ostream::sentry                            Sentry;

    const Sentry *pguard = 0;

    {
        rw_info (0, 0, __LINE__,
                 "std::basic_ostream<%s>::sentry::sentry "
                 "(basic_ostream&)", cname);

        // verify that sentry ctor handles streams with rdbuf() == 0
        static Ostream strm (0);

        strm.setf (std::ios_base::unitbuf);

        // pguard will be deleted in sentry dtor block
        pguard = new Sentry (strm);
    }

    {
        // verify 27.6.2.3, p2 and 3:
        //
        // explicit sentry(basic_ostream<charT,traits>& os);
        //
        // -2-  If os.good() is nonzero, prepares for formatted or
        //      unformatted output. If os.tie() is not a null pointer,
        //      calls os.tie()->flush().
        // -3-  If, after any preparation is completed, os.good() is
        //      true, ok_ == true otherwise, ok_ == false.

        // verify that sentry ctor calls stream.tie()->flush() once
        // and that the object returns true when converted to bool
        test_streambuf<charT> tsb_tied (0);
        test_streambuf<charT> tsb_strm (0);

        Ostream tied (&tsb_tied);
        Ostream strm (&tsb_strm);

        strm.tie (&tied);

        int nsyncs = tsb_tied.ncalls_ [Sync];

        const Sentry guard (strm);

        rw_assert (1 + nsyncs == tsb_tied.ncalls_ [Sync], 0, __LINE__,
                   "std::basic_ostream<%s>::sentry::sentry "
                   "(basic_ostream &s) called s.tie ()->flush() once; "
                   "got %d times",
                   cname, tsb_tied.ncalls_ [Sync] - nsyncs);

        rw_info (0, 0, __LINE__, "std::basic_ostream<%s>::sentry::operator "
                 "bool () const", cname);

        rw_assert (bool (guard), 0, __LINE__,
                   "std::basic_ostream<%s>::sentry::operator bool() == true "
                   "after a successful construction", cname);
    }

    {
        // verify 27.6.2.3, p3: i.e., same as above, but check that
        // the ctor sets ok_ to false after the preparation fails
        // (which is done by having stream.tie()->flush() call 
        // stream.setf (failbit))

        test_streambuf<charT> tsb_strm (0);
        Ostream strm (&tsb_strm);

        // create a use-defined streambuf object and associate it
        // with the stream object, strm, constructed above so that
        // when the latter calls strm.tie()->flush() the former
        // can set failbit in strm
        test_streambuf<charT> tsb_tied (&strm);
        Ostream tied (&tsb_tied);

        // tie the two streams together
        strm.tie (&tied);

        // sentry ctor calls strm.tie()->flush() which sets failbit
        // in strm's; the sentry ctor should detect this condition
        const Sentry guard (strm);

        rw_assert (!guard, 0, __LINE__,
                   "std::basic_ostream<%s>::sentry::operator bool() == "
                   "false after a failed construction (stream.rdstate() "
                   "== %{Is})", cname, strm.rdstate ());
    }

    {
        // verify 27.6.2.3, p3: i.e., same as above, but check that
        // stream.tie()->flush() is not called when stream.good() is
        // false

        test_streambuf<charT> tsb_strm (0);
        Ostream strm (&tsb_strm);

        // create a use-defined streambuf object and associate it
        // with the stream object, strm, constructed above to keep
        // track of the number of any calls to sync() from flush()
        test_streambuf<charT> tsb_tied (&strm);
        Ostream tied (&tsb_tied);

        // tie the two streams together
        strm.tie (&tied);

        strm.setstate (std::ios_base::failbit);

        // sentry ctor should not call strm.tie()->flush() if
        // strm.good() returns false
        const Sentry guard (strm);

        rw_assert (!tsb_tied.ncalls_ [Sync], 0, __LINE__,
                   "std::basic_ostream<%s>::sentry::sentry(basic_ostream&) "
                   "unexpectedly called stream.tie()->flush() when "
                   "stream.good() == false", cname);

        rw_assert (!guard, 0, __LINE__,
                   "std::basic_ostream<%s>::sentry::operator bool() == "
                   "false for a stream whose stream.rdstate() = %{Is}",
                   cname, strm.rdstate ());
    }

#ifndef _RWSTD_NO_EXCEPTIONS

    if (0 <= rw_opt_exceptions) {

        test_streambuf<charT> tsb_strm (0);
        Ostream strm (&tsb_strm);

        test_streambuf<charT> tsb_tied (&strm);
        Ostream tied (&tsb_tied);

        strm.tie (&tied);

        tsb_tied.throws_ = Sync;

        // verify that an exception thrown during the call to
        // strm.tie()->flush() is caught and not propagated, and in
        // response to it badbit is set in the tied stream, but that
        // strm is not affected in any way
        try {

            // sentry ctor calls strm.tie()->flush() which should call
            // strm.tie()->flush() (or its equivalent), which in turn
            // calls rdbuf()->pubsync() which throws an exception (from
            // the virtual overridden above)
            // the sentry ctor must not prevent badbit from being set
            // in the tied stream but it must not allow the exception
            // to propagate to its caller
            const Sentry guard (strm);

            rw_assert (!!guard, 0, __LINE__,
                       "std::basic_ostream<%s>::sentry::operator bool() == "
                       "true after it failed to flush a tied stream due to "
                       "an exception (stream.rdstate() == %{Is})",
                       cname, strm.rdstate ());
        }
        catch (...) {
            rw_assert (false, 0, __LINE__,
                       "std::basic_ostream<%s>::sentry::sentry(basic_ostream&)"
                       " unexpectedly allowed an exception to propagate",
                       cname);
        }

        // verify that the sentry ctor did not affect the state of the stream
        // as a result of the exception thrown during the flushing of the tied
        // stream
        rw_assert (strm.good (), 0, __LINE__,
                   "std::basic_ostream<%s>::sentry::sentry(basic_ostream&) "
                   "unexpectedly affected the state of the stream as a result "
                   "pf an exception thrown during the flushing of a tied "
                   "stream; stream state = %{Is}", cname, strm.rdstate ());

        // verify that the the tied stream object's state has set badbit
        // in response to the exception thrown from its stream buffer
        rw_assert (tied.bad (), 0, __LINE__,
                   "std::basic_ostream<%s>::sentry::sentry(basic_ostream&) "
                   "failed to cause badbit to be set in stream's tied object "
                   "after an exception; tied stream's state = %{Is}",
                   cname, tied.rdstate ());
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    {
        rw_info (0, 0, __LINE__,
                 "std::basic_ostream<%s>::sentry::~sentry()", cname);

        // verify that sentry dtor handles streams with rdbuf() == 0

        // pguard dynamically constructed in sentry ctor block
        delete pguard;
    }

    {
        // verify 27.6.2.3, p4:
        //
        // ~sentry();
        //
        // -4-  If ((os.flags() & iose::unitbuf) && !uncaught_exception())
        //      is true, calls os.flush().

        test_streambuf<charT> tsb (0);
        Ostream strm (&tsb);

        int nsyncs;

        // verify that os.flush() is not called if ios::unitbuf is clear
        {
            const Sentry guard (strm);

            nsyncs = tsb.ncalls_ [Sync];
        }

        rw_assert (nsyncs == tsb.ncalls_ [Sync], 0, __LINE__,
                   "std::basic_ostream<%s>::sentry::~sentry() unexpectedly "
                   "called stream.flush () when ios::unitbuf is clear",
                   cname);

        // verify that os.flush() is called if ios::unitbuf is set
        // (assuming uncaugt_exception() returns false
        {
            strm.setf (std::ios_base::unitbuf);

            const Sentry guard (strm);

            nsyncs = tsb.ncalls_ [Sync];
        }

        rw_assert (1 + nsyncs == tsb.ncalls_ [Sync], 0, __LINE__,
                   "std::basic_ostream<%s>::sentry::~sentry() failed to call "
                   "stream.flush () when ios::unitbuf is set",
                   cname);

#ifndef _RWSTD_NO_EXCEPTIONS
#  ifndef _RWSTD_NO_UNCAUGHT_EXCEPTION

        if (0 <= rw_opt_exceptions) {

            // verify that os.flush() is not called if ios::unitbuf is set
            // but uncaught_exception() returns true

            nsyncs = tsb.ncalls_ [Sync];

            try {
                strm.setf (std::ios_base::unitbuf);

                DtorChek<charT> to_be_destroyed;

                to_be_destroyed.pstrm_ = &strm;

                throw 0;
            }
            catch (...) {
            }

            rw_assert (nsyncs == tsb.ncalls_ [Sync], 0, __LINE__,
                       "std::basic_ostream<%s>::sentry::~sentry() "
                       "unexpectedly called stream.flush() when "
                       "ios::unitbuf is set but uncaught_exception() "
                       "returns true", cname);

            rw_assert (uncaught, 0, __LINE__,
                       "std::uncaught_exception () == true when an exception "
                       "is pending (language runtime library error)");
        }

#  endif   // _RWSTD_NO_UNCAUGHT_EXCEPTION
#endif   // _RWSTD_NO_EXCEPTIONS

    }
}


/***************************************************************************/


// for convenience
#define Boolalpha   std::ios_base::boolalpha
#define Dec         std::ios_base::dec
#define Fixed       std::ios_base::fixed
#define Hex         std::ios_base::hex
#define Internal    std::ios_base::internal
#define Left        std::ios_base::left
#define Oct         std::ios_base::oct
#define Right       std::ios_base::right
#define Scientific  std::ios_base::scientific
#define Showbase    std::ios_base::showbase
#define Showpoint   std::ios_base::showpoint
#define Showpos     std::ios_base::showpos
#define Skipws      std::ios_base::skipws
#define Unitbuf     std::ios_base::unitbuf
#define Uppercase   std::ios_base::uppercase
#define Bin         std::ios_base::bin
#define Adjustfield std::ios_base::adjustfield
#define Basefield   std::ios_base::basefield
#define Floatfield  std::ios_base::floatfield
#define Nolock      std::ios_base::nolock
#define Nolockbuf   std::ios_base::nolockbuf

#define Bad         std::ios_base::badbit
#define Eof         std::ios_base::eofbit
#define Fail        std::ios_base::failbit
#define Good        std::ios_base::goodbit


const int Throw = 1 << 16;


template <class charT, class T>
void test_formatted (charT, int line1, int line2,
                     T           val,
                     const char *str,
                     int         fmtflags,
                     int         width,
                     int         fill,
                     int         rdstate,
                     int         exceptions,
                     int         failure = 0,
                     int         fail_when = 1)
{
    static const char* const cname = rw_any_t (charT ()).type_name ();
    static const char* const tname = rw_any_t (T ()).type_name ();

    typedef std::basic_ostream<charT, std::char_traits<charT> > Ostream;

    charT charbuf [256] = { 0 };

    // define prior to the definition of `tsb' below
    // to guarantee the proper order of destruction
    test_num_put<charT> tnp (1);

    test_streambuf<charT> tsb (charbuf, sizeof charbuf / sizeof *charbuf);

    if (failure & Throw) {
        failure &= ~Throw;

        if (failure < Bool)
            tsb.throws_ = streambuf_failure (failure);
        else
            tnp.throws_ = num_put_overload (failure);
    }
    else {
        if (failure < Bool)
            tsb.fails_ = streambuf_failure (failure);
        else
            tnp.fails_ = num_put_overload (failure);
    }

    tsb.fail_when_ = fail_when;

    Ostream os (&tsb);

    if (select_overload (T ())) {

        // for arithmetic types and void*, imbue a custom num_put facet
        os.imbue (std::locale (std::locale::classic (), &tnp));
    }

    os.flags (std::ios_base::fmtflags (fmtflags));
    os.width (std::streamsize (width));
    os.exceptions (std::ios_base::iostate (exceptions));

    if (-1 != fill)
        os.fill (charT (fill));

    enum { E_unknown = 1, E_failure };
    int caught = 0;

    const char *caught_what = "none (none thrown)";

    try {
        os << val;
    }
    catch (streambuf_failure e) {
        caught = e;
        caught_what = "streambuf_failure";
    }
    catch (num_put_overload e) {
        caught = e;
        caught_what = "num_put_overload";
    }
    catch (const std::ios_base::failure&) {
        caught = E_failure;
        caught_what = "std::ios_base::failure";
    }
    catch (...) {
        caught = E_unknown;
        caught_what = "unknown exception";
    }

    if (str) {
        char cbuf [256] = "";
        for (std::size_t i = 0; i != sizeof cbuf && str [i]; ++i)
            cbuf [i] = char (charbuf [i]);

        rw_assert (0 == std::strcmp (cbuf, str), __FILE__, line1,
                   "%d. std::basic_ostream<%s>::operator<<(%s = %{#lc}) "
                   "inserted \"%s\", expected \"%s\"",
                   line2, cname, tname, cbuf, str);
    }

    // verify that stream is in the expected state
    rw_assert (rdstate == os.rdstate (), __FILE__, line1,
               "%d. std::basic_ostream<%s>::operator<<(%s = %{#lc})"
               ".rdstate() == %{Is}, got %{Is}",
               line2, cname, tname, val, rdstate, os.rdstate ());

    if (!tsb.throws_ && !tnp.throws_) {

        bool pass;

#ifdef _RWSTD_NO_EXT_KEEP_WIDTH_ON_FAILURE
        // verify that width(0) has been called (unless there are exceptions
        // involved, in which case it's unspecified whether width(0) has or
        // has not been called
        pass = !((!exceptions || !tsb.fails_ && !tnp.fails_) && os.width ());

        rw_assert (pass, __FILE__, line1,
                   "%d. std::basic_ostream<%s>::operator<<(%s = %{#lc})"
                   ".width () == 0, got %d",
                   line2, cname, tname, val, os.width  ());
#endif   // _RWSTD_NO_EXT_KEEP_WIDTH_ON_FAILURE

        // verify that ios_base::failure has been thrown (and caught)
        // if badbit is set in exceptions
        pass = !(exceptions & os.rdstate () && caught != E_failure);

        rw_assert (pass, __FILE__, line1,
                   "%d. std::basic_ostream<%s>::operator<<(%s = %{#lc}) "
                   "set %{Is} but failed to throw ios_base::failure "
                   "when the same bit is set in exceptions",
                   line2, cname, tname, val, rdstate);
    }

    if (tsb.throws_) {

        // verify that the same exception (and not ios_base::failure)
        // as the one thrown from basic_filebuf has been propagated
        // and caught when badbit is set in exceptions, and that no
        // exception has been thrown if exceptions is clear
        if (exceptions & Bad && caught != tsb.throws_) {

            rw_assert (false, __FILE__, line1,
                       "%d. std::basic_ostream<%s>::operator<<(%s = %{#lc}) "
                       "failed to propagate an exception thrown by "
                       "basic_filebuf; caught %s instead",
                       line2, cname, tname, val, caught_what);
        }
        else {
            rw_assert ((exceptions & Bad) || !caught, 0, line1,
                       "%d. std::basic_ostream<%s>::operator<<(%s = %{#lc}) "
                       "propagated an exception thrown by basic_filebuf "
                       "when ios_base::badbit is clear in exceptions",
                       line2, cname, tname, val);
        }
    }

    if (tnp.throws_) {

        // verify that the same exception (and not ios_base::failure)
        // as the one thrown from num_put has been propagated and caught
        // when badbit is set in exceptions, and that no exception has
        // been thrown if exceptions is clear
        if (exceptions & Bad)
            rw_assert (caught == tnp.throws_, 0, line1,
                       "%d. std::basic_ostream<%s>::operator<<(%s = %{#lc}) "
                       "failed to propagate exception thrown by "
                       "basic_filebuf; caught %s instead",
                       line2, cname, tname, val, caught_what);
        else
            rw_assert (!caught, __FILE__, line1,
                       "%d. std::basic_ostream<%s>::operator<<(%s = %{#lc}) "
                       "propagated an exception thrown by basic_filebuf"
                       "when ios_base::badbit is clear in exceptions",
                       line2, cname, tname, val);
    }
}

/***************************************************************************/

template <class charT>
bool is_char (charT) { return false; }

bool is_char (char) { return true; }
bool is_char (signed char) { return true; }
bool is_char (unsigned char) { return true; }

bool is_char (const char*) { return true; }
bool is_char (const signed char*) { return true; }
bool is_char (const unsigned char*) { return true; }

template <class charT>
bool is_wchar_t (charT) { return false; }

#ifndef _RWSTD_NO_WCHAR_T

bool is_wchar_t (wchar_t) { return true; }
bool is_wchar_t (const wchar_t*) { return true; }

#endif   // _RWSTD_NO_WCHAR_T


template <class charT, class T>
void test_formatted (charT, int line, T val, 
                     int flags, int width, const char *str, int fill = -1)
{
    static const char* const cname = rw_any_t (charT ()).type_name ();
    static const char* const tname = rw_any_t (T ()).type_name ();

    static int done = 0;

    if (!done++) {
        // show message only the first time for each specialization

        if (is_char (T ()) || is_wchar_t (T ()))
            rw_info (0, 0, __LINE__,
                     "std::operator<<(basic_ostream<%s>&, %s)", cname, tname);
        else
            rw_info (0, 0, __LINE__,
                     "std::basic_ostream<%s>::operator<<(%s)", cname, tname);
    }

    // determine which overload of num_put::put() is expected
    // to be called for a value of type T; 0 for non-numeric
    const int npo = select_overload (val);

    typedef std::basic_ostream<charT, std::char_traits<charT> > Ostream;

#define T      charT (), line, __LINE__
#define TEST   test_formatted

    TEST (T, val, str, flags, width, fill, Good, 0, 0);

    if (npo) {
        // exercise the behavior of basic_ostream with exception bits
        // clear when num_put fails by returning an ostreambuf_iterator
        // in a failed state  (expect ios::badbit to be set w/o throwing
        // an exception)
        TEST (T, val, "", flags, width, fill, Bad, 0, npo);

        // exercise the behavior of basic_ostream with ios::badbit
        // set in exceptions when num_put fails by returning an
        // ostreambuf_iterator in a failed state  (expect ios::badbit
        // to be set and ios::failure to be thrown)
        TEST (T, val, "", flags, width, fill, Bad, Bad, npo);

        // exercise the behavior of basic_ostream with exceptions bits
        // clear when num_put fails by throwing an exception (expect
        // ios::badbit to be set and no exception to propagate)
        TEST (T, val, "", flags, width, fill, Bad, 0, Throw | npo);

        // exercise the behavior of basic_ostream with ios::badbit
        // set in exceptions when num_put fails by throwing an exception
        // (expect ios::badbit to be set and the original exception to
        // propagate)
        TEST (T, val, "", flags, width, fill, Bad, Bad, Throw | npo);
    }

    // exercise the behavior of basic_ostream with exception bits clear
    // when basic_streambuf::overflow() fails by returning eof() (expect
    // ios::badbit to be set w/o throwing an exception)
    TEST (T, val, "", flags, width, fill, Bad, 0, Overflow);

    // exercise the behavior of basic_ostream with ios::badbit set in
    // exceptions when basic_streambuf::overlow() fails by returning
    // eof() (expect ios::badbit to be set and ios::failure to be thrown)
    TEST (T, val, "", flags, width, fill, Bad, Bad, Overflow);

    // exercise the behavior of basic_ostream with exceptions bits clear
    // when basic_streambuf::overlow() fails by throwing an exception
    // (expect ios::badbit to be set and no exception to propagate)
    TEST (T, val, "", flags, width, fill, Bad, 0, Throw | Overflow);

    // exercise the behavior of basic_ostream with ios::badbit set in
    // exceptions when basic_streambuf::overflow() fails by throwing
    // an exception (expect ios::badbit to be set and the original
    // exception to propagate)
    TEST (T, val, "", flags, width, fill, Bad, Bad, Throw | Overflow);
}

/***************************************************************************/


template <class charT>
void test_formatted (charT)
{
#undef T
#define T(val)   charT (), __LINE__, val

#undef TEST
#define TEST test_formatted

    //////////////////////////////////////////////////////////////////
    // exercise operator<< (basic_ostream, char)

    //        +------------ character to insert
    //        |    +------- fmtflags
    //        |    |  +---- field width
    //        |    |  |  +- expected output
    //        |    |  |  |
    //        v    v  v  v
    TEST (T ('a'), 0, 0, "a");
    TEST (T ('A'), 0, 0, "A");
    TEST (T ('@'), 0, 1, "@");
    TEST (T ('#'), 0, 2, " #");

    if (is_wchar_t (charT ())) {

#ifndef _RWSTD_NO_NATIVE_WCHAR_T

        //////////////////////////////////////////////////////////////////
        // exercise operator<< (basic_ostream, wchar_t)
        TEST (T (L'b'), 0, 0, "b");
        TEST (T (L'B'), 0, 0, "B");

#endif   // _RWSTD_NO_NATIVE_WCHAR_T

    }
    else {

        // no operator<< (basic_ostream<wchar_t>, {signed,unsigned} char)

        //////////////////////////////////////////////////////////////////
        // exercise operator<< (basic_ostream, signed char)
        typedef signed char SChar;

        TEST (T (SChar ('b')), 0, 0, "b");
        TEST (T (SChar ('B')), 0, 0, "B");

        //////////////////////////////////////////////////////////////////
        // exercise operator<< (basic_ostream, signed char)
        typedef unsigned char UChar;

        TEST (T (UChar ('c')), 0, 0, "c");
        TEST (T (UChar ('C')), 0, 0, "C");
    }


#ifndef _RWSTD_NO_NATIVE_BOOL

    //////////////////////////////////////////////////////////////////
    // exercise basic_ostream::operator<< (bool)
    TEST (T (false), 0, 0, "0");
    TEST (T (true),  0, 0, "1");

    TEST (T (false), Hex | Showbase, 0, "0");
    TEST (T (true),  Hex | Showbase, 0, "0x1");

    TEST (T (false), Boolalpha, 0, "false");
    TEST (T (true),  Boolalpha, 0, "true");

#endif   // _RWSTD_NO_NATIVE_BOOL


    //////////////////////////////////////////////////////////////////
    // exercise basic_ostream::operator<< (short)
    TEST (T (short ( 0)), 0, 0,  "0");
    TEST (T (short ( 1)), 0, 0,  "1");
    TEST (T (short (-1)), 0, 0, "-1");

#if SHRT_MAX == 32767

    TEST (T (short (SHRT_MAX)), 0, 0,  "32767");
    TEST (T (short (SHRT_MIN)), 0, 0, "-32768");

    TEST (T (short (SHRT_MAX)), Hex, 0, "7fff");

// The following is a bug because a short is casted to a long, so for
// negative numbers, the result is sign extended.
#if DRQS
    TEST (T (short (SHRT_MIN)), Hex, 0, "8000");
#else
    TEST (T (short (SHRT_MIN)), Hex, 0, "ffff8000");
#endif

#elif SHRT_MAX == 2147483647

    TEST (T (short (SHRT_MAX)), 0, 0,  "2147483647");
    TEST (T (short (SHRT_MIN)), 0, 0, "-2147483648");

    TEST (T (short (SHRT_MAX)), Hex, 0, "ffffffff");
    TEST (T (short (SHRT_MIN)), Hex, 0, "80000000");

#endif   // SHRT_MAX

    //////////////////////////////////////////////////////////////////
    // exercise basic_ostream::operator<< (unsigned short)
    typedef unsigned short UShort;

    TEST (T (UShort (  0)), 0, 0,   "0");
    TEST (T (UShort (  1)), 0, 0,   "1");
    TEST (T (UShort (255)), 0, 0, "255");

#if USHRT_MAX == 0xffffU

    TEST (T (UShort (USHRT_MAX)), 0, 0,  "65535");
    TEST (T (UShort (USHRT_MAX)), Hex, 0, "ffff");

#elif USHRT_MAX == 0xffffffffU

    TEST (T (UShort (SHRT_MAX)), 0, 0,   "4294967295");
    TEST (T (UShort (SHRT_MAX)), Hex, 0,   "ffffffff");

#endif   // USHRT_MAX

    //////////////////////////////////////////////////////////////////
    // exercise basic_ostream::operator<< (int)
    TEST (T ( 0), 0, 0,  "0");
    TEST (T ( 1), 0, 0,  "1");
    TEST (T (-1), 0, 0, "-1");

#if INT_MAX == 32767

    TEST (T (INT_MAX), 0, 0,  "32767");
    TEST (T (INT_MIN), 0, 0, "-32768");

    TEST (T (INT_MAX), Hex, 0, "7fff");
    TEST (T (INT_MIN), Hex, 0, "8000");

#elif INT_MAX == 2147483647

    TEST (T (INT_MAX), 0, 0,  "2147483647");
    TEST (T (INT_MIN), 0, 0, "-2147483648");

    TEST (T (INT_MAX), Hex, 0, "7fffffff");
    TEST (T (INT_MIN), Hex, 0, "80000000");

#elif INT_MAX == 9223372036854775807

    TEST (T (INT_MAX), 0, 0,  "9223372036854775807");
    TEST (T (INT_MIN), 0, 0, "-9223372036854775808");

    TEST (T (INT_MAX), Hex, 0, "7fffffffffff");
    TEST (T (INT_MIN), Hex, 0, "800000000000");

#endif   // INT_MAX

    //////////////////////////////////////////////////////////////////
    // exercise basic_ostream::operator<< (unsigned int)
    TEST (T (  0), 0, 0,   "0");
    TEST (T (  1), 0, 0,   "1");
    TEST (T (255), 0, 0, "255");

#if UINT_MAX == 0xffffU

    TEST (T (UINT_MAX), 0, 0,  "65535");
    TEST (T (UINT_MAX), Hex, 0, "ffff");

#elif UINT_MAX == 0xffffffffU

    TEST (T (UINT_MAX), 0, 0,   "4294967295");
    TEST (T (UINT_MAX), Hex, 0,   "ffffffff");

#elif UINT_MAX == 0xffffffffffffffffU

    TEST (T (UINT_MAX), 0, 0,   "18446744073709551615");
    TEST (T (UINT_MAX), Hex, 0,     "ffffffffffffffff");

#endif   // UINT_MAX

    //////////////////////////////////////////////////////////////////
    // exercise basic_ostream::operator<< (long)
    TEST (T ( 0L), 0, 0,  "0");
    TEST (T ( 1L), 0, 0,  "1");
    TEST (T (-1L), 0, 0, "-1");

#if LONG_MAX == 32767L

    TEST (T (LONG_MAX), 0, 0,  "32767");
    TEST (T (LONG_MIN), 0, 0, "-32768");

    TEST (T (LONG_MAX), Hex, 0, "7fff");
    TEST (T (LONG_MIN), Hex, 0, "8000");

#elif LONG_MAX == 2147483647L

    TEST (T (LONG_MAX), 0, 0,  "2147483647");
    TEST (T (LONG_MIN), 0, 0, "-2147483648");

    TEST (T (LONG_MAX), Hex, 0, "7fffffff");
    TEST (T (LONG_MIN), Hex, 0, "80000000");

#elif LONG_MAX == 9223372036854775807L

    TEST (T (LONG_MAX), 0, 0,  "9223372036854775807");
    TEST (T (LONG_MIN), 0, 0, "-9223372036854775808");

    TEST (T (LONG_MAX), Hex, 0, "7fffffffffff");
    TEST (T (LONG_MIN), Hex, 0, "800000000000");

#endif   // LONG_MAX

    //////////////////////////////////////////////////////////////////
    // exercise basic_ostream::operator<< (unsigned int)

    TEST (T (  0UL), 0, 0,   "0");
    TEST (T (  1UL), 0, 0,   "1");
    TEST (T (257UL), 0, 0, "257");

#if ULONG_MAX == 0xffffUL

    TEST (T (ULONG_MAX), 0, 0,  "65535");
    TEST (T (ULONG_MAX), Hex, 0, "ffff");

#elif ULONG_MAX == 0xffffffffUL

    TEST (T (ULONG_MAX), 0, 0,   "4294967295");
    TEST (T (ULONG_MAX), Hex, 0,   "ffffffff");

#elif ULONG_MAX == 0xffffffffffffffffUL

    TEST (T (ULONG_MAX), 0, 0,   "18446744073709551615");
    TEST (T (ULONG_MAX), Hex, 0,     "ffffffffffffffff");

#endif   // ULONG_MAX

#ifdef _RWSTD_LONG_LONG

    //////////////////////////////////////////////////////////////////
    // exercise basic_ostream::operator<< (long long)
    typedef _RWSTD_LONG_LONG LongLong;

    TEST (T (LongLong ( 0)), 0, 0,  "0");
    TEST (T (LongLong ( 1)), 0, 0,  "1");
    TEST (T (LongLong (-1)), 0, 0, "-1");

    // avoid using the preprocessor here to prevent warnings
    // or errors about invalid preprocessor constants (e.g.,
    // PR #28595)
    if (_RWSTD_LLONG_MAX == 32767L) {

        TEST (T (_RWSTD_LLONG_MAX), 0, 0,  "32767");
        TEST (T (_RWSTD_LLONG_MIN), 0, 0, "-32768");

        TEST (T (_RWSTD_LLONG_MAX), Hex, 0, "7fff");
        TEST (T (_RWSTD_LLONG_MIN), Hex, 0, "8000");
    }
    else if (_RWSTD_LLONG_MAX == 2147483647L) {

        TEST (T (_RWSTD_LLONG_MAX), 0, 0,  "2147483647");
        TEST (T (_RWSTD_LLONG_MIN), 0, 0, "-2147483648");

        TEST (T (_RWSTD_LLONG_MAX), Hex, 0, "7fffffff");
        TEST (T (_RWSTD_LLONG_MIN), Hex, 0, "80000000");

    }
    else if (_RWSTD_LLONG_MAX > 2147483647L) {

        TEST (T (_RWSTD_LLONG_MAX), 0, 0,  "9223372036854775807");
        TEST (T (_RWSTD_LLONG_MIN), 0, 0, "-9223372036854775808");

        TEST (T (_RWSTD_LLONG_MAX), Hex, 0, "7fffffffffff");
        TEST (T (_RWSTD_LLONG_MIN), Hex, 0, "800000000000");

    }

    //////////////////////////////////////////////////////////////////
    // exercise basic_ostream::operator<< (unsigned long long)

    typedef unsigned _RWSTD_LONG_LONG ULongLong;

    TEST (T (ULongLong (  0)), 0, 0,   "0");
    TEST (T (ULongLong (  1)), 0, 0,   "1");
    TEST (T (ULongLong (257)), 0, 0, "257");

    if (_RWSTD_ULLONG_MAX == 0xffffUL) {
        TEST (T (_RWSTD_ULLONG_MAX), 0, 0,  "65535");
        TEST (T (_RWSTD_ULLONG_MAX), Hex, 0, "ffff");
    }
    else if (_RWSTD_ULLONG_MAX == 0xffffffffUL) {
        TEST (T (_RWSTD_ULLONG_MAX), 0, 0,   "4294967295");
        TEST (T (_RWSTD_ULLONG_MAX), Hex, 0,   "ffffffff");
    }
    else if (_RWSTD_ULLONG_MAX > 0xffffffffUL) {
        TEST (T (_RWSTD_ULLONG_MAX), 0, 0,   "18446744073709551615");
        TEST (T (_RWSTD_ULLONG_MAX), Hex, 0,     "ffffffffffffffff");
    }

#endif   // _RWSTD_LONG_LONG

    //////////////////////////////////////////////////////////////////
    // exercise basic_ostream::operator<< (float)
    TEST (T ( 0.0f), 0, 0, "0");
    TEST (T ( 1.0f), 0, 0, "1");
    TEST (T (-2.1f), 0, 0, "-2.1");

    // exercise the default precision of 6, which gives the maximum number
    // of significant digits for the "%g" or "%G" conversion, which is
    // the default for the default flags
    TEST (T (3.1415926f), 0, 0,          "3.14159");
    TEST (T (3.1415926f), Scientific, 0, "3.14159");

    // exercise the default precision of 6, which gives the number of
    // digits after the decimal point for the "%f" or "%F" conversion,
    // which is used for the fixed flag
    TEST (T (12.3456789f), Fixed, 0, "12.345679");

    //////////////////////////////////////////////////////////////////
    // exercise basic_ostream::operator<< (double)
    TEST (T ( 0.0), 0, 0, "0");
    TEST (T ( 2.0), 0, 0, "2");
    TEST (T (-4.2), 0, 0, "-4.2");

    // exercise the default precision of 6, which gives the maximum number
    // of significant digits for the "%g" or "%G" conversion, which is
    // the default for the default flags
    TEST (T (1.23456789), 0, 0,          "1.23457");
    TEST (T (9.87654321), Scientific, 0, "9.87654");

    // exercise the default precision of 6, which gives the number of
    // digits after the decimal point for the "%f" or "%F" conversion,
    // which is used for the fixed flag
    TEST (T (12.3456789), Fixed, 0, "12.345679");

#ifndef _RWSTD_NO_LONG_DOUBLE

    //////////////////////////////////////////////////////////////////
    // exercise basic_ostream::operator<< (long double)
    TEST (T ( 0.0L), 0, 0, "0");
    TEST (T ( 3.0L), 0, 0, "3");
    TEST (T (-5.3L), 0, 0, "-5.3");

    // exercise the default precision of 6, which gives the maximum number
    // of significant digits for the "%g" or "%G" conversion, which is
    // the default for the default flags
    TEST (T (1.23456789L), 0, 0,          "1.23457");
    TEST (T (9.87654321L), Scientific, 0, "9.87654");

    // exercise the default precision of 6, which gives the number of
    // digits after the decimal point for the "%f" or "%F" conversion,
    // which is used for the fixed flag
    TEST (T (12.3456789L), Fixed, 0, "12.345679");

#endif   // _RWSTD_NO_LONG_DOUBLE

    //////////////////////////////////////////////////////////////////
    // exercise basic_ostream::operator<< (const void*)
    TEST (T ((void*)0),      0, 0, 0 /* format implementation-defined */);
    TEST (T ((void*)1),      0, 0, 0);
    TEST (T ((void*)0x2345), 0, 0, 0);

    //////////////////////////////////////////////////////////////////
    // exercise basic_ostream::operator<< (const char*)
    TEST (T ("a"),     0,                        0, "a");
    TEST (T ("b"),     0,                       -1, "b");
    TEST (T ("ab"),    0,                        0, "ab");
    TEST (T ("abc"),   0,                        4, " abc");
    TEST (T ("def"),   Left,                     5, "def<<", '<');
    TEST (T ("ghi"),   Right,                    6, ">>>ghi", '>');
    TEST (T ("jklm"),  Internal,                 7, "   jklm");
    TEST (T ("nopqr"), Left | Right,             8, "   nopqr");
    TEST (T ("stuv"),  Left | Internal,          9, "     stuv");
    TEST (T ("wxy"),   Right | Internal,        10, "       wxy");
    TEST (T ("z"),     Left | Right | Internal, 11, "__________z", '_');

    if (is_char (charT ())) {

        // inserters overloaded on signed and unsigned char*
        // defined only for basic_ostream<char>, not wchar_t

        //////////////////////////////////////////////////////////////////
        // exercise basic_ostream::operator<< (const signed char*)

#undef SCH
#define SCH(val)      charT (), __LINE__, (const signed char*)val

        TEST (SCH ("A"),     0,                        0, "A");
        TEST (SCH ("B"),     0,                       -1, "B");
        TEST (SCH ("AB"),    0,                        0, "AB");
        TEST (SCH ("ABC"),   0,                        4, " ABC");
        TEST (SCH ("DEF"),   Left,                     5, "DEF++", '+');
        TEST (SCH ("GHI"),   Right,                    6, "---GHI", '-');
        TEST (SCH ("JKLM"),  Internal,                 7, "   JKLM");
        TEST (SCH ("NOPQR"), Left | Right,             8, "   NOPQR");
        TEST (SCH ("STUV"),  Left | Internal,          9, "     STUV");
        TEST (SCH ("WXY"),   Right | Internal,        10, "       WXY");
        TEST (SCH ("Z"),     Left | Right | Internal, 11, "##########Z", '#');

        //////////////////////////////////////////////////////////////////
        // exercise basic_ostream::operator<< (const unsigned char*)

#undef UCH
#define UCH(val)      charT (), __LINE__, (const unsigned char*)val

        TEST (UCH ("Z"),     0,                        0, "Z");
        TEST (UCH ("Y"),     0,                       -1, "Y");
        TEST (UCH ("YZ"),    0,                        0, "YZ");
        TEST (UCH ("XYZ"),   0,                        4, " XYZ");
        TEST (UCH ("UVW"),   Left,                     5, "UVW\\\\", '\\');
        TEST (UCH ("RST"),   Right,                    6, "///RST", '/');
        TEST (UCH ("OPQR"),  Internal,                 7, "   OPQR");
        TEST (UCH ("JKLMN"), Left | Right,             8, "   JKLMN");
        TEST (UCH ("FGHI"),  Left | Internal,          9, "     FGHI");
        TEST (UCH ("CDE"),   Right | Internal,        10, "       CDE");
        TEST (UCH ("AB"),    Left | Right | Internal, 11, "^^^^^^^^^AB", '^');

    }
}

/***************************************************************************/

template <class charT>
void test_flush (charT)
{
    static const char* const cname = rw_any_t (charT ()).type_name ();

    rw_info (0, 0, __LINE__,
             "std::basic_ostream<%s>::flush() ", cname);

    typedef std::basic_ostream<charT, std::char_traits<charT> >   Ostream;
    typedef std::basic_streambuf<charT, std::char_traits<charT> > Streambuf;

    {
        test_streambuf<charT> tsb (0);
        Ostream strm (&tsb);

        const int nsyncs = tsb.ncalls_ [Sync];

        strm.flush ();

        rw_assert (1 + nsyncs == tsb.ncalls_ [Sync], 0, __LINE__,
                   "std::basic_ostream<%s>::flush() called rdbuf()->sync() "
                   "once; got %d times",
                   cname, tsb.ncalls_ [Sync] - nsyncs);
    }

    {
        test_streambuf<charT> tsb (0);
        Ostream strm (&tsb);

        tsb.fails_     = Sync;
        tsb.fail_when_ = 1;

        const int nsyncs = tsb.ncalls_ [Sync];

        strm.flush ();

        rw_assert (1 + nsyncs == tsb.ncalls_ [Sync], 0, __LINE__,
                   "std::basic_ostream<%s>::flush() called rdbuf()->sync() "
                   "once; got %zu times",
                   cname, tsb.ncalls_ [Sync] - nsyncs);

        rw_assert (std::ios_base::badbit == strm.rdstate (), 0, __LINE__,
                   "std::basic_ostream<%s>::flush() failed to set badbit "
                   "after rdbuf()->pubsync() failed; state = %{Is}",
                   cname, strm.rdstate ());
    }

    {
        // exercise LWG issue 581: flush() not unformatted function

        test_streambuf<charT> tsb (0);
        Ostream strm (&tsb);

        strm.setstate (std::ios_base::failbit);

        const int nsyncs = tsb.ncalls_ [Sync];

        strm.flush ();

        rw_assert (nsyncs == tsb.ncalls_ [Sync], 0, __LINE__,
                   "std::basic_ostream<%s>::flush() unexpectedly made "
                   "%zu calls to rdbuf()->sync() when rdstate () == "
                   "failbit",
                   cname, tsb.ncalls_ [Sync] - nsyncs);

        rw_assert (std::ios_base::failbit == strm.rdstate (), 0, __LINE__,
                   "std::basic_ostream<%s>::flush() unexpectedly changed "
                   "state from std::ios_base::failbit to %{Is}",
                   cname, strm.rdstate ());
    }
}

/***************************************************************************/

template <class charT>
void run_test (charT)
{
    // exercise ostream::sentry
    if (rw_opt_sentry < 0)
        rw_note (0, 0, __LINE__, "sentry tests disabled");
    else
        test_sentry (charT ());

    // exercise formatted output functions
    if (rw_opt_formatted < 0)
        rw_note (0, 0, __LINE__, "tests of formatted functions disabled");
    else
        test_formatted (charT ());

    // exercise formatted output functions
    if (rw_opt_unformatted < 0)
        rw_note (0, 0, __LINE__, "tests of unformatted functions disabled");
    else {
        // FIXME: implement this
        // test_unformatted (charT ());
    }

    // exercise flush()
    if (rw_opt_flush < 0)
        rw_note (0, 0, __LINE__, "flush tests disabled");
    else
        test_flush (charT ());
}

/***************************************************************************/

static int
run_test (int, char**)
{
    run_test (char ());

#ifndef _RWSTD_NO_WCHAR_T

    run_test (wchar_t ());

#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}

/***************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.ostream",
                    0 /* no comment */,
                    run_test,
                    "|-sentry~ "
                    "|-formatted~ "
                    "|-unformatted~ "
                    "|-flush~ "
                    "|-no-exceptions~ ",
                    &rw_opt_sentry,
                    &rw_opt_formatted,
                    &rw_opt_unformatted,
                    &rw_opt_flush,
                    &rw_opt_exceptions);
}
