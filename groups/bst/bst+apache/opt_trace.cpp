/************************************************************************
 *
 * opt_trace.cpp - definitions of tracing option handlers
 *
 * $Id: opt_trace.cpp 465641 2006-10-19 14:15:56Z faridz $
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
 * Copyright 1994-2005 Rogue Wave Software.
 *
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include "opt_trace.h"

#include <stdio.h>    // for fprintf()
#include <stdlib.h>   // for strtol()
#include <string.h>   // for strchar()

/************************************************************************/

// masked diagnostics (those that shouldn't be issued)
int _rw_diag_mask = 1 << diag_trace;

// bitmap of diagnostics to ignore
int _rw_diag_ignore = 0;


int
_rw_setopt_trace_mask (int argc, char *argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {
        static const char helpstr[] = {
            "Specifies the severity of diagnostic messages to be issued when\n"
            "active. By default, diagnostics with severity of 3 and and above\n"
            "are issued, all others are suppressed. The severity of an inactive"
            "\ndiagnostic is always zero, regardless of what the severity would"
            "\nbe if it were active.\n\n"
            "The syntax of <arg> is as follows:\n"
            "<arg>   ::= <range> [ , <range> ]\n"
            "<range> ::= [ - ] <digit> | <digit> - [ <digit> ]\n"
            "<digit> ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9\n"
            "\n"
            "The default names of the diagnostic messages arranged in order\n"
            "of increasing severity are as follows:\n"
            "ITRACE, TRACE, DEBUG, INFO, NOTE, WARNING, ASSERTION, UNUSED,\n"
            "ERROR, and FATAL.\n"
            "The name of each inactive diagnostic is preceded by the name\n"
            "of the severity-0 diagnostic (ITRACE_ by default). The severity\n"
            "of an inactive diagnostic is always zero, regardless of what\n"
            "the severity would be if it were active.\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    char *parg = strchr (argv [0], '=');

    if (0 == parg) {
        _rw_diag_mask = 0;
        return 0;
    }

    int diag_set = 0;

    const char* const argbeg = ++parg;

    // the lower bound of a range of severities to be enabled or disabled
    // negative values are not valid and denote an implicit lower bound
    // of 1 (such as in "-3" which is a shorthand for "1-3")
    long first = -1;

    for ( ; '\0' != *parg ; ) {

        // skip any leading whitespace
        for ( ; ' ' == *parg; ++parg);

        if ('-' == *parg) {
            if (first < 0) {
                first = 0;
                ++parg;
            }
            else {
                fprintf (stderr,
                         "invalid character '%c' at position %d: \"%s\"\n",
                         *parg, int (parg - argbeg), argv [0]);
                return 2;
            }
        }

        // parse a numeric argument
        char *end;
        long severity = strtol (parg, &end, 0);

        // skip any trailing whitespace
        for ( ; ' ' == *end; ++end);

        if (end == parg || '-' != *end && ',' != *end && '\0' != *end) {
            fprintf (stderr,
                     "invalid character '%c' at position %d: \"%s\"\n",
                     *end, int (parg - argbeg), argv [0]);
            return 2;
        }

        if (0 <= first) {
            if (severity < 0 || N_DIAG_TYPES < severity) {
                fprintf (stderr,
                         "invalid value %ld at position %d: \"%s\"\n",
                         severity, int (parg - argbeg), argv [0]);
                return 2;
            }

            ++severity;

            if ((',' == *end || '-' == *end) && end [1])
                ++end;
        }
        else if (',' == *end) {
            first = severity++;
            if ('\0' == end [1]) {
                fprintf (stderr,
                         "invalid character '%c' at position %d: \"%s\"\n",
                         *end, int (parg - argbeg), argv [0]);
                return 2;
            }

            ++end;
        }
        else if ('-' == *end) {
            first = severity;
            while (' ' == *++end);
            if ('\0' == *end) {
                severity = N_DIAG_TYPES;
            }
            else if  (',' == *end) {
                severity = N_DIAG_TYPES;
                ++end;
            }
            else
                severity = -1;
        }
        else if ('\0' == *end) {
            first = severity++;
        }
        else {
            fprintf (stderr,
                     "invalid character '%c' at position %d: \"%s\"\n",
                     *end, int (parg - argbeg), argv [0]);
            return 2;
        }

        parg = end;

        if (0 <= first && first < severity) {
            for (int i = first; i != severity; ++i)
                diag_set |= 1 << i;
            severity = -1;
        }
    }

    _rw_diag_mask = ~diag_set;

    return 0;
}

/************************************************************************/

int
_rw_setopt_trace (int argc, char *argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {
        static const char helpstr[] = {
            "Specifies that diagnostic messages of all severities be issued\n"
            "regardless of whether they are active or not. See the --severity\n"
            "option for details.\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    return _rw_setopt_trace_mask (argc, argv);
}
