/***************************************************************************
 *
 * 19.cerrno.cpp - test exercising the lib.errno
 *
 * $Id: 19.cerrno.cpp 550991 2007-06-26 23:58:07Z sebor $
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

// include only <cerrno> here to prevent namespace pollution
#include <cerrno>

/**************************************************************************/

const char* const errors[] = {

#ifndef errno
    "errno",
#endif   // errno

#ifndef EDOM
    "EDOM not defined",
#elif EDOM <= 0
    "EDOM not positive",
#endif   // EDOM

#ifndef EILSEQ
    "EILSEQ",   // lwg issue 288
#elif EILSEQ <= 0
    "EILSEQ not positive",
#elif EILSEQ == EDOM
    "EILSEQ not distinct from EDOM",
#endif   // EILSEQ

#ifndef ERANGE
    "ERANGE not defined",
#elif ERANGE <= 0
    "ERANGE not positive",
#elif ERANGE == EDOM
    "ERANGE not distinct from EDOM",
#elif ERANGE == EILSEQ
    "ERANGE not distinct from EILSEQ",
#endif   // ERANGE

    0
};

/**************************************************************************/

// include all other headers here
#include <any.h>      // for rw_any_t
#include <driver.h>   // for rw_test(), ...

/**************************************************************************/

template <class T> void set_errno_value (T, int) { /* empty */ }
void set_errno_value (int &errno_ref, int val) { errno_ref = val; }

/**************************************************************************/

static int
errno_at_startup;

static int
run_test (int, char**)
{
    rw_info (0, 0, 0, "exercising the contents of the <cerrno> header");

    for (unsigned i = 0; errors [i]; ++i) {
        rw_assert (0 == errors [i], 0, 0,
                   "macro %s", errors [i]);
    }

    // get the type of errno
    const char* const errno_type = rw_any_t (errno).type_name ();

    // 7.5, p2 of C99: the type of errno must be int
    rw_assert (   'i' == errno_type [0]
               && 'n' == errno_type [1]
               && 't' == errno_type [2]
               && '\0' == errno_type [3],
               0, 0, 
               "the type of errno is int, got %s", errno_type);

    // 7.5, p3 of C99: errno must be 0 at program startup
    rw_assert (0 == errno_at_startup, 0, 0,
               "errno == 0 at program startup, got %d", errno_at_startup);

#ifndef EDOM
#  define EDOM   33 /* Solaris value */
#endif   // EDOM

    // 7.5, p2 of C99: errno must be a modifiable lvalue
    set_errno_value (errno, int (EDOM));

    rw_assert (EDOM == errno, 0, 0,
               "errno == %d (%{#*m}, got %d (%{#m}) "
               "(errno not a modifiable lvalue?)",
               EDOM, EDOM, errno, errno);

#ifndef ERANGE
#  define ERANGE   34 /* Solaris value */
#endif   // ERANGE

    set_errno_value (errno, int (ERANGE));

    rw_assert (ERANGE == errno, 0, 0,
               "errno == %d (%{#*m}, got %d (%{#m}) "
               "(errno not a modifiable lvalue?)",
               ERANGE, ERANGE, errno, errno);

#ifndef EILSEQ
#  define EILSEQ   84 /* Solaris value */
#endif   // EILSEQ

    set_errno_value (errno, int (EILSEQ));

    rw_assert (EILSEQ == errno, 0, 0,
               "errno == %d (%{#*m}, got %d (%{#m}) "
               "(errno not a modifiable lvalue?)",
               EILSEQ, EILSEQ, errno, errno);

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    errno_at_startup = errno;

    return rw_test (argc, argv, __FILE__,
                    "lib.errno",
                    0 /* no comment */, run_test,
                    0 /* no command line options */, 0);
}
