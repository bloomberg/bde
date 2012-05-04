// -*- C++ -*-
/***************************************************************************
 *
 * any.h - definition of utility class rw_any_t
 *
 * $Id: any.h 590052 2007-10-30 12:44:14Z faridz $
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
 * Copyright 2006 Rogue Wave Software.
 *
 **************************************************************************/

#ifndef RW_ANY_H_INCLUDED
#define RW_ANY_H_INCLUDED


#include <testdefs.h>


class _TEST_EXPORT rw_any_t
{
public:

#ifndef _RWSTD_NO_BOOL
    rw_any_t (bool);
#endif   // _RWSTD_NO_BOOL

    rw_any_t (char);
    rw_any_t (signed char);
    rw_any_t (unsigned char);

    rw_any_t (signed short);
    rw_any_t (unsigned short);

    rw_any_t (signed int);
    rw_any_t (unsigned int);

    rw_any_t (signed long);
    rw_any_t (unsigned long);

#ifdef _RWSTD_LONG_LONG
    rw_any_t (signed _RWSTD_LONG_LONG);
    rw_any_t (unsigned _RWSTD_LONG_LONG);
#endif   // _RWSTD_LONG_LONG

    rw_any_t (float);
    rw_any_t (double);

#ifndef _RWSTD_NO_LONG_DOUBLE
    rw_any_t (long double);
#endif   // _RWSTD_NO_LONG_DOUBLE

    rw_any_t (void*);
    rw_any_t (volatile void*);
    rw_any_t (const void*);
    rw_any_t (const volatile void*);

    rw_any_t (char*);
    rw_any_t (const char*);
    rw_any_t (volatile char*);
    rw_any_t (const volatile char*);

#ifndef _RWSTD_NO_NATIVE_WCHAR_T
    rw_any_t (wchar_t);
#endif   // _RWSTD_NO_NATIVE_WCHAR_T

#ifndef _RWSTD_NO_WCHAR_T
    rw_any_t (wchar_t*);
    rw_any_t (const wchar_t*);
    rw_any_t (volatile wchar_t*);
    rw_any_t (const volatile wchar_t*);
#endif   // _RWSTD_NO_WCHAR_T

    rw_any_t (const rw_any_t&);

    ~rw_any_t ();

    rw_any_t& operator= (const rw_any_t&);

    const char* tostr (const char* = 0);
    const char* type_name () const;

    enum type_id_t {
        t_none,
        t_bool, t_schar, t_uchar, t_char,
        t_sshrt, t_ushrt, t_sint, t_uint, t_slong, t_ulong,
        t_sllong, t_ullong,
        t_flt, t_dbl, t_ldbl,
        t_wchar,
        t_pvoid, t_c_pvoid, t_v_pvoid, t_cv_pvoid,
        t_str, t_c_str, t_v_str, t_cv_str,
        t_wstr, t_c_wstr, t_v_wstr, t_cv_wstr
    };

private:

    union uval_t {

#ifndef _RWSTD_NO_LONG_DOUBLE
        long double ldbl_;
#endif   // _RWSTD_NO_LONG_DOUBLE
        const volatile void *pvoid_;
        double      dbl_;
#ifdef _RWSTD_LONG_LONG
        signed _RWSTD_LONG_LONG   sllong_;
        unsigned _RWSTD_LONG_LONG ullong_;
#endif   // _RWSTD_LONG_LONG
        float          flt_;
#ifndef _RWSTD_NO_NATIVE_WCHAR_T
        wchar_t        wchar_;
#endif   // _RWSTD_NO_NATIVE_WCHAR_T
        signed long    slong_;
        unsigned long  ulong_;
        signed int     sint_;
        unsigned int   uint_;
        signed short   sshrt_;
        unsigned short ushrt_;
        signed char    schar_;
        unsigned char  uchar_;
        char           char_;
#ifndef _RWSTD_NO_BOOL
        bool           bool_;
#endif   // _RWSTD_NO_BOOL
    } val_;

    char      *str_;
    type_id_t  tid_;
};


#define TOSTR(x)   rw_any_t (x).tostr ()


#endif   // RW_ANY_H_INCLUDED
