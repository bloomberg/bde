/************************************************************************
 *
 * rw_streambuf.h - definition of the MyStreambuf class template
 *
 * $Id: rw_streambuf.h 590052 2007-10-30 12:44:14Z faridz $
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
 * Copyright 2004-2006 Rogue Wave Software.
 *
 **************************************************************************/

#ifndef RW_STREAMBUF_H_INCLUDED
#define RW_STREAMBUF_H_INCLUDED


#include <cstring>          // for memset()
#include <streambuf>        // for basic_streambuf
#include <ios>

#include <testdefs.h>
#include <rw_char.h>        // for make_char()
#include <rw_exception.h>   // for rw_throw()


enum MemFun {
    // bitmask with a bit for each virtual member function
    None      = 0,
    Setbuf    = 0x0001,
    Seekoff   = 0x0002,
    Seekpos   = 0x0004,
    Showmanyc = 0x0008,
    Xsgetn    = 0x0010,
    Underflow = 0x0020,
    Uflow     = 0x0040,
    Overflow  = 0x0080,
    Pbackfail = 0x0100,
    Xsputn    = 0x0200,
    Sync      = 0x0400,
    // bit OR-ed with MemFun bits
    Throw     = 0x1000,
    Failure   = 0x2000
};

static const char* const streambuf_func_names[] = {
    "setbuf", "seekoff", "seekpos", "showmanyc", "xsgetn", "underflow",
    "uflow", "overflow", "pbackfail", "xsputn", "sync"
};

template <class charT, class Traits>
struct MyStreambuf: std::basic_streambuf<charT, Traits>
{
    typedef charT                                        char_type;
    typedef Traits                                       traits_type;
    typedef std::basic_streambuf<char_type, traits_type> Base;
    typedef typename Base::int_type                      int_type;
    typedef typename Base::off_type                      off_type;
    typedef typename Base::pos_type                      pos_type;

    MyStreambuf (std::streamsize, int, int);

    MyStreambuf (const char*, std::streamsize, int, int);

    ~MyStreambuf () {
        delete[] buf_;
    }

    // public interface to base class protected members
    char_type* pubeback () const {
        return this->eback ();
    }

    char_type* pubgptr () const {
        return this->gptr ();
    }

    char_type* pubegptr () const {
        return this->egptr ();
    }

    char_type* pubpbase () const {
        return this->pbase ();
    }

    char_type* pubpptr () const {
        return this->pptr ();
    }

    char_type* pubepptr () const {
        return this->epptr ();
    }

    void pubsetg (charT *beg, charT *cur, charT *end) {
        this->setg (beg, cur, end);
    }

    void pubsetp (charT *beg, charT *cur, charT *end) {
        this->setp (beg, end);
        this->pbump (cur - beg);
    }

private:

    // overridden protected virtual functions
    virtual Base* setbuf (char_type*, std::streamsize) {
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
        test (Showmanyc);
        return 0;
    }

    virtual std::streamsize xsgetn (char_type*, std::streamsize) {
        test (Xsgetn);
        return 0;
    }

    virtual int_type underflow ();

    virtual int_type uflow ();

    virtual int_type
    overflow (int_type = traits_type::eof ());

    virtual int_type
    pbackfail (int_type = traits_type::eof ());

    virtual std::streamsize
    xsputn (const char_type *buf, std::streamsize bufsize) {
        if (!test (Xsputn))
            return 0;
        return Base::xsputn (buf, bufsize);
    }

    virtual int sync () {
        test (Sync);
        return 0;
    }

public:

    int ncalls (MemFun) const;
    int memfun_inx (MemFun) const;

    char_type       *buf_;
    std::streamsize  bufsize_;

    int        throw_set_;       // functions that should throw
    int        fail_set_;        // functions that should fail
    MemFun     threw_;           // which function threw
    MemFun     failed_;          // which function failed

    int        fail_when_;       // call number on which to fail

    int        throw_when_ [11]; // call number on which to throw for each func
    int        allthrows_;       // total number of thrown exceptions

    // max size of the pending input sequence
    static std::streamsize in_pending_;

    // max size of the pending output sequence
    static std::streamsize out_pending_;

private:

    bool test (MemFun) const;

    int ncalls_ [11];  // number of calls made to each function

    int allcalls_;     // total number of calls
};


template <class charT, class Traits>
std::streamsize MyStreambuf<charT, Traits>::
in_pending_ = 1;


template <class charT, class Traits>
std::streamsize MyStreambuf<charT, Traits>::
out_pending_ = 1;


template <class charT, class Traits>
MyStreambuf<charT, Traits>::
MyStreambuf (std::streamsize bufsize, int fail_set, int when)
    : Base (), buf_ (0), bufsize_ (bufsize),
      throw_set_ (0), fail_set_ (0), threw_ (None), failed_ (None),
      fail_when_ (when), allthrows_ (0), allcalls_ (0)
{
    // reset the member function call counters
    std::memset (ncalls_, 0, sizeof ncalls_);

    // reset the member function throw counters
    std::memset (throw_when_, 0, sizeof throw_when_);

    // allocate a (possibly wide) character buffer for output
    buf_ = new charT [bufsize_];

    // invalidate the contents of the buffer
    traits_type::assign (buf_, bufsize_, make_char ('\xfe', buf_));

    // set the put area to 0 size to force a call to overflow()
    // on the first write attempt to the buffer
    this->setp (buf_, buf_);

    // set the fail and throw flags
    if (fail_set & Throw) {
        throw_set_ = fail_set & ~Throw;

        for (unsigned i = 0; i < 11; ++i)
            if (throw_set_ & (1U << i))
                throw_when_ [i] = when;
    }
    else {
        fail_set_ = fail_set;
    }
}


template <class charT, class Traits>
MyStreambuf<charT, Traits>::
MyStreambuf (const char *buf, std::streamsize bufsize, int fail_set, int when)
    : Base (), buf_ (0), bufsize_ (bufsize),
      throw_set_ (0), fail_set_ (0), threw_ (None), failed_ (None),
      fail_when_ (when), allthrows_ (0), allcalls_ (0)
{
    // reset the member function call counters
    std::memset (ncalls_, 0, sizeof ncalls_);

    // reset the member function throw counters
    std::memset (throw_when_, 0, sizeof throw_when_);

    // as a convenience, if `bufsize == -1' compute the size
    // from the length of `buf'
    if (std::streamsize (-1) == bufsize_)
        bufsize_ = std::streamsize (std::char_traits<char>::length (buf)) + 1;

    // allocate a (possibly wide) character buffer to copy
    // (and widen) the contents of `buf' into
    buf_ = new charT [bufsize_ + 1];

    for (std::streamsize inx = 0; inx != bufsize_; ++inx) {
        typedef unsigned char UChar;

        buf_ [inx] = make_char (buf [inx], buf_);
    }

    // zero out the (non-dereferenceable) element just past the end
    // so that the buffer can be printed out as an ordinary string
    buf_ [bufsize_] = charT ();

    // set the get area to 0 size to force a call to underflow()
    // on the first read attempt from the buffer
    this->setg (buf_, buf_, buf_);

    // set the fail and throw flags
    if (fail_set & Throw) {
        throw_set_ = fail_set & ~Throw;

        for (unsigned i = 0; i < 11; ++i)
            if (throw_set_ & (1U << i))
                throw_when_ [i] = when;
    }
    else {
        fail_set_ = fail_set;
    }
}


template <class charT, class Traits>
typename MyStreambuf<charT, Traits>::int_type
MyStreambuf<charT, Traits>::
underflow ()
{
    if (!test (Underflow))
        return traits_type::eof ();

    if (this->egptr () - this->gptr () > 0) {
        this->gbump (1);
        return traits_type::to_int_type (*this->gptr ());
    }

    if (this->egptr () < buf_ + bufsize_) {

        // increase the pending input sequence by no more than
        // the lesser of the available buffer space and `in_pending_'
        std::streamsize pending = buf_ + bufsize_ - this->egptr ();

        if (pending > in_pending_)
            pending = in_pending_;

        this->setg (this->eback (), this->gptr (), this->egptr () + pending);
        return traits_type::to_int_type (*this->gptr ());
    }

    failed_ = Underflow;
    return traits_type::eof ();
}


template <class charT, class Traits>
typename MyStreambuf<charT, Traits>::int_type
MyStreambuf<charT, Traits>::
overflow (int_type c /* = traits_type::eof () */)
{
    if (!test (Overflow))
        return traits_type::eof ();

    if (this->epptr () - this->pptr () > 0) {
        traits_type::assign (*this->pptr (), traits_type::to_char_type (c));
        this->pbump (1);
        return traits_type::not_eof (c);
    }

    if (this->epptr () < buf_ + bufsize_) {

        // increase the pending output sequence by no more than
        // the lesser of the available buffer space and `out_pending_'
        std::streamsize pending = buf_ + bufsize_ - this->epptr ();

        if (pending > out_pending_)
            pending = out_pending_;

        const std::streamsize pptr_off = this->pptr () - this->pbase ();

        this->setp (this->pbase (), this->epptr () + pending);

        this->pbump (pptr_off);

        traits_type::assign (*this->pptr (), traits_type::to_char_type (c));

        this->pbump (1);

        return traits_type::not_eof (c);
    }

    failed_ = Overflow;
    return traits_type::eof ();
}


template <class charT, class Traits>
typename MyStreambuf<charT, Traits>::int_type
MyStreambuf<charT, Traits>::
pbackfail (int_type c /* = traits_type::eof () */)
{
    if (!test (Pbackfail))
        return traits_type::eof ();

    if (this->gptr () == buf_) {
        failed_ = Pbackfail;
        return traits_type::eof ();
    }

    this->setg (this->gptr () - 1, this->gptr () - 1, this->gptr ());

    const int_type last = traits_type::to_int_type (*this->gptr ());

    if (!traits_type::eq_int_type (c, traits_type::eof ()))
        traits_type::assign (*this->gptr (), traits_type::to_char_type (c));

    return last;
}


template <class charT, class Traits>
typename MyStreambuf<charT, Traits>::int_type
MyStreambuf<charT, Traits>::
uflow ()
{
    if (!test (Uflow))
        return traits_type::eof ();

    if (this->egptr () - this->gptr () > 0) {
        this->gbump (1);
        return traits_type::to_int_type (*this->gptr ());
    }

    if (this->egptr () < buf_ + bufsize_) {

        // increase the pending input sequence by no more than
        // the lesser of the available buffer space and `in_pending_'
        std::streamsize pending = buf_ + bufsize_ - this->egptr ();

        if (pending > in_pending_)
            pending = in_pending_;

//        this->setg (this->eback (), this->gptr (), this->egptr () + pending);
        typename MyStreambuf<charT, Traits>::int_type ret =
                                     traits_type::to_int_type (*this->gptr ());
        this->setg (this->eback (), this->gptr () + 1, this->egptr () + pending);
        return ret;
    }

    failed_ = Uflow;
    return traits_type::eof ();
}

template <class charT, class Traits>
int
MyStreambuf<charT, Traits>::
memfun_inx (MemFun which) const
{
    int inx = -1;

    for (unsigned i = 0; i < sizeof (which) * _RWSTD_CHAR_BIT; ++i) {
        if (which & (1U << i)) {
            if (inx < 0)
                inx = i;
            else
                return -1;
        }
    }

    return inx;
}

template <class charT, class Traits>
int
MyStreambuf<charT, Traits>::
ncalls (MemFun which) const
{
    int inx = memfun_inx (which);
    if (0 <= inx)
        return ncalls_ [inx];

    return -1;
}


template <class charT, class Traits>
bool
MyStreambuf<charT, Traits>::
test (MemFun which) const
{
    MyStreambuf* const self = _RWSTD_CONST_CAST (MyStreambuf*, this);

    int inx = memfun_inx (which);
    if (-1 == inx)
        return true;

    // increment the counter tracking the number of calls made
    // to each member function; do so regardless of whether
    // an exception will be thrown below
    self->ncalls_ [inx] ++;
    self->allcalls_ ++;
    const int callno = self->ncalls_ [inx];

#ifndef _RWSTD_NO_EXCEPTIONS

    // if the call counter is equal to the `fail_when_' watermark
    // and `which' is set in the `throw_set_' bitmask, throw an
    // exception with the value of the member id
    if (callno == throw_when_ [inx] && throw_set_ & which) {
        self->threw_ = which;
        self->allthrows_++;

        rw_throw (ex_stream, __FILE__, __LINE__,
                  streambuf_func_names [inx],
                  "%s", "test exception");
    }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    if (callno == throw_when_ [inx] && throw_set_ & which) {
        self->threw_ = which;
        return false;
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    // ...otherwise check if the member should succeed or fail
    // and return true or false, respectively
    const bool success = !(fail_set_ & which) || callno != fail_when_;

    if (!success)
        self->failed_ = which;

    return success;
}


#endif   // RW_STREAMBUF_H_INCLUDED
