/************************************************************************
 *
 * fmt_defs.h - declarations and definitions of types and functions
 *              used to implement the snprintfa helper functions
 *
 * $Id: fmt_defs.h 590052 2007-10-30 12:44:14Z faridz $
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
 * Copyright 2005-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef RW_FMT_DEFS_H_INCLUDED
#define RW_FMT_DEFS_H_INCLUDED

#include <testdefs.h>
#include <stddef.h>     // for ptrdiff_t, size_t

/********************************************************************/

// convenience typedefs
typedef unsigned char  UChar;
typedef unsigned short UShrt;
typedef unsigned int   UInt;
typedef unsigned long  ULong;

#ifdef _RWSTD_LONG_LONG

typedef unsigned _RWSTD_LONG_LONG ULLong;

#endif   // _RWSTD_LONG_LONG

typedef void (*funptr_t)();

struct DummyStruct { };
typedef void (DummyStruct::*memptr_t)() const;

/********************************************************************/

struct Buffer {
    char   **pbuf;       // pointer to the output buffer
    size_t  *pbufsize;   // pointer to the size of the buffer
    size_t   maxsize;    // maximum not-to-exceed size
    size_t   endoff;     // offset of the last character
};

/********************************************************************/

struct FmtSpec
{
    // optional flags
    unsigned fl_minus  : 1;
    unsigned fl_plus   : 1;
    unsigned fl_pound  : 1;
    unsigned fl_space  : 1;
    unsigned fl_zero   : 1;

    // optional length modifier
    enum Modifier {
        mod_none = 0,
        mod_h,       // short modifier
        mod_hh,      // char modifier
        mod_l,       // long modifier
        mod_ll,      // long long modifier
        mod_j,       // intmax_t modifier
        mod_z,       // size_t modifier
        mod_t,       // ptrdiff_t modifier
        mod_L,       // long double modifier
        mod_ext_A,   // extension: arrays
        mod_ext_I    // extension: int as ios::iostate
    };

    Modifier mod : 5;

    unsigned cond       : 1;   // have an if/else clause
    unsigned cond_true  : 1;   // if/else clause is active (true)
    unsigned cond_begin : 1;   // beginning of an if/else clause
    unsigned cond_end   : 1;   // end of an if/else clause

    // note that the signedness of a bitfield is implementation-defined
    // unless explicitly declared signed or unsigned

    // extension: 8, 16, 32, and 64 bit integer width modifier
    signed int iwidth : 4;

    // extension: optional numerical base 2 - 36
    signed int base   : 7;

    // extension: optional parameter number
    long paramno;

    // optional field width and precision
    int width;
    int prec;

    // extension: string argument
    char *strarg;

    // required conversion specifier
    int cvtspec;

    // extension: fill character
    int fill;

#ifndef _RWSTD_NO_LONG_DOUBLE
    typedef long double ldbl_t;
#else
    typedef double      ldbl_t;   // bogus (for convenience)
#endif   // _RWSTD_NO_LONG_DOUBLE

#ifdef _RWSTD_LONG_LONG
    typedef _RWSTD_LONG_LONG llong_t;
#else
    typedef long             llong_t;   // bogus (for convenience)
#endif   // _RWSTD_LONG_LONG

#ifdef _RWSTD_INT64_T
    typedef _RWSTD_INT64_T i64_t;
#else
    typedef int            i64_t;   // likely bogus (for convenience)
#endif   // _RWSTD_INT64_T

#ifdef _RWSTD_INT32_T
    typedef _RWSTD_INT32_T i32_t;
#else
    typedef int            i32_t;
#endif   // _RWSTD_INT64_T

#ifdef _RWSTD_WINT_T
    typedef _RWSTD_WINT_T wint_t;
#else
    typedef int           wint_t;
#endif   // _RWSTD_WINT_T

    typedef ::size_t   size_t;
    typedef ptrdiff_t  diff_t;
    typedef ::funptr_t funptr_t;
    typedef ::memptr_t memptr_t;

    typedef int        int_t;
    typedef long       long_t;
    typedef void*      ptr_t;
    typedef double     dbl_t;

    union {
        ldbl_t    ldbl_;
        llong_t   llong_;
        i64_t     i64_;
        ptr_t     ptr_;
        long_t    long_;
        i32_t     i32_;
        int_t     int_;
        diff_t    diff_;
        size_t    size_;
        wint_t    wint_;

        dbl_t     dbl_;
        memptr_t  memptr_;
        funptr_t  funptr_;
    } param;
};

/********************************************************************/

// format a character string
extern int
_rw_fmtstr (const FmtSpec&, Buffer&, const char*, size_t);

// format errno value/name
extern int
_rw_fmterrno (const FmtSpec&, Buffer&, int);

// format Windows GetLastError(), or errno value/name
extern int
_rw_fmtlasterror (const FmtSpec&, Buffer&, int);

// format the name/value of an LC_XXX constant/environment variable
extern int
_rw_fmtlc (const FmtSpec&, Buffer&, int);

// format ctype_base::mask
extern int
_rw_fmtmask (const FmtSpec&, Buffer&, int);

// format a character mask (alpha|alnum|...|xdigit)
extern int
_rw_fmtcharmask (const FmtSpec&, Buffer&, int);

// format ios_base::iostate
extern int
_rw_fmtiostate (const FmtSpec&, Buffer&, int);

// format ios_base::fmtflags
extern int
_rw_fmtflags (const FmtSpec&, Buffer&, int);

// format ios_base::openmode
extern int
_rw_fmtopenmode (const FmtSpec&, Buffer&, int);

// format ios_base::seekdir
extern int
_rw_fmtseekdir (const FmtSpec&, Buffer&, int);

// format ios_base::event
extern int
_rw_fmtevent (const FmtSpec&, Buffer&, int);

extern int
_rw_fmtmonpat (const FmtSpec&, Buffer&, const char [4]);

// format a signal value/name
extern int
_rw_fmtsignal (const FmtSpec&, Buffer&, int);

// format a bitset of arbitrary size
extern int
_rw_fmtbits (const FmtSpec&, Buffer&, const void*, size_t);

#endif   // RW_FMT_DEFS_H_INCLUDED
