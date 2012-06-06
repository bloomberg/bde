/************************************************************************
 *
 * printf.h - declarations of the rw_printf family of functions
 *
 * $Id: rw_printf.h 550991 2007-06-26 23:58:07Z sebor $
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
 * Copyright 1994-2006 Rogue Wave Software.
 *
 **************************************************************************/

#ifndef RW_PRINTF_H_INCLUDED
#define RW_PRINTF_H_INCLUDED

#include <testdefs.h>

struct rw_file;

// the equivalent of stdout and stderr
extern _TEST_EXPORT rw_file* const rw_stdout;
extern _TEST_EXPORT rw_file* const rw_stderr;


/************************************************************************
 * Formatted file output.
 ************************************************************************/

/**
 *  Prints to rw_stdout.
 */
_TEST_EXPORT int
rw_printf (const char*, ...);

/**
 * Prints to a file.
 */
_TEST_EXPORT int
rw_fprintf (rw_file*, const char*, ...);


/************************************************************************
 * Formatted string output.
 ************************************************************************/

/**
 * Prints to a character buffer.
 */
_TEST_EXPORT int
rw_sprintf (char*, const char*, ...);

/**
 * Prints to a character buffer of given size.
 */
_TEST_EXPORT int
rw_snprintf (char*, _RWSTD_SIZE_T, const char*, ...);


/************************************************************************
 * Formatted string output into a dynamically allocated buffer.
 ************************************************************************/

/**
 * Prints to a dynamically allocated character buffer.
 *
 * @param fmt  Format specifier.
 *
 * @return  On success, returns a pointer to the dynamically allocated
 *          character buffer. Otherwise, returns 0.
 */
_TEST_EXPORT char*
rw_sprintfa (const char* fmt, ...);


/**
 * Prints to a dynamically allocated character buffer.
 *
 * @param buf  A pointer to character buffer where the function should
 *        store its output.
 * @param bufise  The size of the character buffer in bytes.
 *
 * @return  On success, if the size of the supplied buffer was sufficient
 *          to format all characters including the terminating NUL, returns
 *          buf. Otherwise, if the size of the supplied buffer was not
 *          sufficient, returns a pointer to the newly allocated character
 *          buffer of sufficient size. Returns 0 on failure.
 */
_TEST_EXPORT char*
rw_snprintfa (char *buf, _RWSTD_SIZE_T bufsize, const char* fmt, ...);


/**
 * Prints to a dynamically allocated character buffer of sufficient size.
 * Provided for portability with the BSD and GNU C libraries:
 *
 * http://www.freebsd.org/cgi/man.cgi?query=asprintf
 * http://www.openbsd.org/cgi-bin/man.cgi?query=asprintf
 * http://netbsd.gw.com/cgi-bin/man-cgi?asprintf++NetBSD-current
 * http://www.gnu.org/software/libc/manual/html_node/Dynamic-Output.html
 *
 * @param pbuf  Pointer to a pointer to character set by the caller to
 *        to address of the inital character buffer, or 0 of no such
 *        buffer exists. The function sets the pointer to a the address
 *        of the dynamically allocated character buffer, or leaves it
 *        unchanged if it doesn't allocate any buffer.
 * @param pbufsize  Pointer to a size_t set by the caller to the size of
 *        the inital character buffer. The function sets the value pointed
 *        to by this argument to the size of the dynamically allocated
 *        character buffer, or leaves it unchanged if it doesn't allocate
 *        any buffer.
 * @param fmt  Format specifier.
 *        The format specifier string has the same syntax as C99 sprintf
 *        (see 7.19.6.1 of ISO/IEC 9899:1999) with the following extensions:
 *
 *        %n$          where n is a integer (see IEEE Std 1003.1)
 *        %m           the value of strerror(errno)
 *
 *        %{?}         if clause (extracts an int)
 *        %{:}         else clause
 *        %{;}         end of if/else clause
 *
 *        %{Ac}        quoted array of narrow characters
 *        %{*Ac}       quoted array of characters of width '*' each
 *                     where '*' is an int argument extracted from
 *                     the argument list
 *        %{#s}        quoted narrow character string
 *        %{#ls}       quoted wide character string
 *        %{$envvar}   value of an environment variable envvar
 *        %{f}         function pointer
 *        %{K}         signal name (such as "SIGABRT")
 *        %{M}         member pointer
 *        %{#m}        name of the errno constant (such as "EINVAL")
 *        %{n}         buffer size
 *        %{S}         pointer to std::string
 *        %{lS}        pointer to std::wstring
 *        %{tm}        pointer to struct tm
 *        %{InJ}       where n is one of { 8, 16, 32, 64 }
 *                     and J is one of { d, o, x, X }
 *
 * @return  On success, returns the number of characters formatted into
 *          the buffer, otherwise -1.
 */
_TEST_EXPORT int
rw_asnprintf (char** pbuf, _RWSTD_SIZE_T *pbufsize, const char *fmt, ...);


#endif   // RW_PRINTF_H_INCLUDED
