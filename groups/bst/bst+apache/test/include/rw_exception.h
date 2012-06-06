/************************************************************************
 *
 * rw_exception.h - defines a test driver exception
 *
 * $Id: rw_exception.h 590052 2007-10-30 12:44:14Z faridz $
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

#ifndef RW_EXCEPTION_H_INCLUDED
#define RW_EXCEPTION_H_INCLUDED

#include <testdefs.h>

enum ExceptionId
{
    ex_unknown = 0,
    // custom exceptions, i.e. not derived for std::exception
    ex_stream = 1,
    ex_custom = 7,
    // exceptions derived from std::exception
    ex_bad_alloc = 8,
    ex_std = (7 << 3)
};


struct _TEST_EXPORT Exception
{
    const ExceptionId id_;

    Exception (ExceptionId id) : id_ (id) {}

    virtual ~Exception ();

    virtual const char* what () const = 0;
};


_TEST_EXPORT void
rw_throw (ExceptionId exid, const char *file, int line,
          const char *function, const char *fmt, ...);

#endif   // RW_EXCEPTION_H_INCLUDED
