/************************************************************************
 *
 * environ.cpp - definitions of testsuite helpers
 *
 * $Id: environ.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2001-2006 Rogue Wave Software.
 *
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <environ.h>

#include <assert.h>   // for assert
#include <stdlib.h>   // for getenv, malloc, putenv
#include <string.h>   // for strchr, strlen, ...

#include <bsls_platform.h>

extern "C" {

// char** environ;

#ifndef BSLS_PLATFORM__OS_AIX
#if !defined(_RWSTD_NO_PUTENV_CONST_CHAR) \
 || (defined(BSLS_PLATFORM__OS_HPUX) && defined(BSLS_PLATFORM__CMP_GNU))

_RWSTD_DLLIMPORT int putenv (const char*) _LIBC_THROWS ();

#else   // if defined (_RWSTD_NO_PUTENV_CONST_CHAR)

_RWSTD_DLLIMPORT int putenv (char*) _LIBC_THROWS ();

#endif   // _RWSTD_NO_PUTENV_CONST_CHAR
#endif


#ifdef _RWSTD_NO_UNSETENV
#  ifndef _RWSTD_NO_UNSETENV_IN_LIBC

_RWSTD_DLLIMPORT int unsetenv (const char*) _LIBC_THROWS ();

#    undef _RWSTD_NO_UNSETENV

#  endif   // _RWSTD_NO_UNSETENV_IN_LIBC
#endif   // _RWSTD_NO_UNSETENV

}   // extern "C"


// sets (or unsets) one or more sep-separated environment variables
_TEST_EXPORT int
rw_putenv (const char* str, int sep /* = -1 */)
{
    int nset = 0;
    int ret  = 0;

    if (!str) {
        str = getenv ("RW_PUTENV");
        if (str)
            sep = *str++;
        else
            return 0;
    }

    // set separator to NUL if it's invalid
    if (sep < 0 || int (_RWSTD_UCHAR_MAX) < sep)
        sep = 0;

    for (const char *pvar = str; pvar && *pvar; ++nset) {

        // look for separator (or the terminating NUL by default)
        const char *pend = strchr (pvar, sep);

        if (0 == pend)
            pend = pvar + strlen (pvar);

        const size_t varlen = pend - pvar;

        // reserve the one more character for Windows mode
        char* const envvar = (char*)malloc (varlen + 2);
        if (0 == envvar)
            return -1;

        memcpy (envvar, pvar, varlen);
        envvar [varlen] = '\0';

        // look for the first equals sign
        char* const equals = strchr (envvar, '=');

        char *var = 0;

        // putenv mode: 0 - POSIX; 1 - undefined; 2 - Windows
        static int mode = 1;

        if (equals) {
            // add the variable to the environment or modify it if
            // it's already defined

            // Note: calling Solaris 7 putenv() during program startup
            // (i.e., from ctors of namespace-scope objects) prevents
            // getenv() from finding that variable at program runtime

            char namebuf [256];
            const size_t namelen = equals - envvar;
            assert (namelen < sizeof (namebuf));

            memcpy (namebuf, envvar, namelen);
            namebuf [namelen] = '\0';

            switch (!equals [1] * mode) {
            case 0:
                ret = putenv (envvar);
                break;
            case 1:
                ret = putenv (envvar);
                if (getenv (namebuf)) {
                    mode = 0;
                    break;
                }
                mode = 2;
                // fall through
            case 2:
                // on Windows it's impossible to set empty environment variable
                // append any character after '='
                equals [1] = '1';
                equals [2] = '\0';
                ret = putenv (envvar);
                equals [1] = '\0';
                break;
            }

            // determine wheteher putenv() made copy of the variable
            // or if it simply used the pointer passed to it; if the
            // former, deallocate the buffer dynamically allocated
            // above
            var = getenv (namebuf);

            // empty the environment variable directly on Windows
            if (!equals [1] && 2 == mode)
                *var = '\0';

            if (equals + 1 != var)
                free (envvar);
        }
        else if ((var = getenv (envvar))) {
            // try to remove variable from the environment

#ifndef _RWSTD_NO_UNSETENV
#  if defined (_RWSTD_OS_FREEBSD) || defined (_RWSTD_OS_DARWIN)
            // FreeBSD (and Darwin) declares void unsetenv(const char*)
            // http://www.freebsd.org/cgi/man.cgi?query=unsetenv&sektion=3
            unsetenv (envvar);
            ret = 0;
#  else   // !FreeBSD ...
            ret = unsetenv (envvar);
#  endif   // FreeBSD ...
#else   // ifdef _RWSTD_NO_UNSETENV
            switch (mode) {
            case 0:
                ret = putenv (envvar);
                break;
            case 1:
                ret = putenv (envvar);
                if (!getenv (envvar)) {
                    mode = 0;
                    break;
                }
                mode = 2;
                // fall through
            case 2:
                // on Windows append '=' character to remove
                // the environment variable
                envvar [varlen] = '=';
                envvar [varlen + 1] = '\0';
                ret = putenv (envvar);
                envvar [varlen] = '\0';
                break;
            }
#endif   // _RWSTD_NO_UNSETENV

            if (0 == ret) {
                // see if the variable has been removed
                var = getenv (envvar);
                if (var) {
                    // if not, zero-out the first byte of its name
                    // FIXME: make this more robust, e.g., by calling
                    // unsetenv() when provided or by manipulating
                    // the environment directly
                    *(var - 1 - varlen) = '\0';

#if 0   // disabled

                    char **penv = environ;
                    if (penv) {
                        while (*penv && *penv != (var - 1 - varlen))
                            ++penv;

                        while (*penv)
                            *penv = penv [1];
                    }

#endif   // 0/1

                }
            }

            free (envvar);
        }

        pvar = pend + !!*pend;
    }

    if (1 == nset)
        return ret;

    return nset;
}
