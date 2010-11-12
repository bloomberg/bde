/************************************************************************
 *
 * exception.cpp - exceptions testsuite helpers
 *
 * $Id: exception.cpp 427177 2006-07-31 17:23:01Z sebor $
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
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <rw_exception.h>
#include <driver.h>   
#include <rw_printf.h>
#include <string.h>         // for size_t, strcpy()
#include <stdlib.h>         // for free()
#include <stdarg.h>         // for va_arg(), va_list
#include <new>              // for std::bad_alloc

/**************************************************************************/

_TEST_EXPORT int
rw_vasnprintf (char**, size_t*, const char*, va_list);

static int
_rw_format (char** pbuf, size_t* pbufsize, const char* file, int line,
            const char* function, const char* fmt, va_list va);

/**************************************************************************/

Exception::~Exception ()
{
}

/**************************************************************************/

struct ExceptionBase
{
private:
    struct ExceptionString;

public:
    char             buf_ [256];
    ExceptionString* ex_str_;

    ExceptionBase ();
    ExceptionBase (const ExceptionBase&);

    ~ExceptionBase ();

    ExceptionBase& operator= (const ExceptionBase&);

    void format (const char* file, int line, const char* function,
        const char* fmt, va_list va);

    const char* what () const;

private:

    struct ExceptionString
    {
        long  refs_;
        char* str_;

        void init (char* str)
        {
            refs_ = 1;
            str_ = str;
        }

        void addref () { ++refs_; }

        long release ()
        {
            if (0 == --refs_) {
                free (str_);
                str_ = 0;
            }
            return refs_;
        }

    private:
        // not defined
        ExceptionString (const ExceptionString&);
        ExceptionString& operator= (const ExceptionString&);
    };


    void free_ ();
};

ExceptionBase::ExceptionBase () : ex_str_ (0)
{
    buf_ [0] = '\0';
}

ExceptionBase::ExceptionBase (const ExceptionBase& ex) : ex_str_ (0)
{
    *this = ex;
}

ExceptionBase::~ExceptionBase ()
{
    free_ ();
}

ExceptionBase& ExceptionBase::operator= (const ExceptionBase& ex)
{
    if (&ex != this) {

        free_ ();

        if (ex.ex_str_) {
            ex_str_ = ex.ex_str_;
            ex_str_->addref ();
        }
        else
            strcpy (buf_, ex.buf_);
    }

    return *this;
}


void ExceptionBase::
format (const char* file, int line,
        const char* function, const char* fmt, va_list va)
{
    free_ ();

    char* tmpbuf = 0;
    size_t bufsize = 0;

    const int nchars =
        _rw_format (&tmpbuf, &bufsize, file, line, function, fmt, va);

    if (0 > nchars)
        return;

    if (sizeof buf_ <= size_t (nchars)) {
        // tmpbuf_ size is too small
        ex_str_ = _RWSTD_STATIC_CAST (ExceptionString*,
                                      malloc (sizeof (*ex_str_)));

        if (ex_str_) {
            ex_str_->init (tmpbuf);
            return;
        }
    }

    // buf_ size if enough or cannot allocate memory for the ex_str_
    const size_t len =
        size_t (nchars) < sizeof buf_ ? size_t (nchars) : sizeof buf_ - 1;
    memcpy (buf_, tmpbuf, len);
    buf_ [len] = '\0';
    free (tmpbuf);
}


const char* ExceptionBase::what () const
{
    return ex_str_ && ex_str_->str_ ? ex_str_->str_ : buf_;
}

void ExceptionBase::free_ ()
{
    buf_ [0] = '\0';

    if (ex_str_ && 0 == ex_str_->release ()) {
        free (ex_str_);
        ex_str_ = 0;
    }
}

/**************************************************************************/

struct BadAlloc : std::bad_alloc, ExceptionBase
{
    BadAlloc ()
    {
    }

    BadAlloc (const ExceptionBase& ex) : ExceptionBase (ex)
    {
    }

    ~BadAlloc () _THROWS (()) {}

    const char* what () const _THROWS (())
    {
        const char* msg = ExceptionBase::what ();
        return (msg && msg[0]) ? msg : std::bad_alloc::what ();
    }
};

/**************************************************************************/

struct StreamException : Exception, ExceptionBase
{
    StreamException () : Exception (ex_stream)
    {
    }

    StreamException (const ExceptionBase& ex) :
        Exception (ex_stream), ExceptionBase (ex)
    {
    }

    const char* what () const
    {
        return ExceptionBase::what ();
    }
};

/**************************************************************************/

static int
_rw_format (char** pbuf, size_t* pbufsize, const char* file, int line,
            const char* function, const char* fmt, va_list va)
{
    const int nchars1 = function ?
        rw_asnprintf (pbuf, pbufsize, "\"%s\", %d, \"%s\" ",
                      file, line, function) :
        rw_asnprintf (pbuf, pbufsize, "\"%s\", %d, ", file, line);

    if (0 > nchars1)
        return nchars1;

    char *tmpbuf = 0;
    size_t tmpsize = 0;

    const int nchars2 = rw_vasnprintf (&tmpbuf, &tmpsize, fmt, va);

    if (0 > nchars2) {
        free (tmpbuf);
        return nchars1;
    }

    const int nchars3 = rw_asnprintf (pbuf, pbufsize, "%{+}%s", tmpbuf);

    free (tmpbuf);

    return 0 > nchars3 ? nchars1 : nchars1 + nchars3;
}

_TEST_EXPORT void
rw_throw (ExceptionId exid, const char *file, int line,
          const char *function, const char *fmt, ...)
{
    va_list va;
    va_start (va, fmt);

    ExceptionBase base;
    base.format (file, line, function, fmt, va);

    va_end (va);

    switch (exid) {

    case ex_stream:
        throw StreamException (base);

    case ex_bad_alloc:
        throw BadAlloc (base);

    default:
        throw ex_unknown;
    }
}
