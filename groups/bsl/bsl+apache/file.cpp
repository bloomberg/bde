/************************************************************************
 *
 * file.cpp - definitions of testsuite file I/O helpers
 *
 * $Id: file.cpp 587712 2007-10-24 00:21:53Z sebor $
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
 * Copyright 2004-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <file.h>

#ifdef _RWSTD_OS_LINUX
#  ifdef _RWSTD_NO_PURE_C_HEADERS
     // on Linux define _XOPEN_SOURCE to get CODESET defined in <langinfo.h>
     // (avoid this hackery when using pure "C' headers (i.e., with the EDG
     // eccp demo)
#    define _XOPEN_SOURCE   500   /* Single Unix conformance */
     // bring __int32_t into scope (otherwise <wctype.h> fails to compile)
#    include <sys/types.h>
#  endif
#endif   // Linux

#include <fcntl.h>
#include <sys/stat.h>

#if !defined  (_WIN32) && !defined (_WIN64)
#  include <langinfo.h>   // for CODESET
#  include <unistd.h>     // for close(), open()
#else
#  include <io.h>         // for _commit()
#endif

#include <assert.h>   // for assert
#include <errno.h>    // for errno
#include <locale.h>   // for LC_XXX macros
#include <stdio.h>    // for sprintf, ...
#include <stdlib.h>   // for free, malloc, realloc
#include <string.h>   // for strcat, strcpy, strlen, ...
#include <ctype.h>
#include <wchar.h>    // for wcslen, ...


#ifndef PATH_MAX
#  define PATH_MAX   1024
#endif

#ifndef P_tmpdir
   // P_tmpdir is an XSI (X/Open System Interfaces) extension
   // to POSIX which need not be provided by otherwise conforming
   // implementations
#  define P_tmpdir "/tmp/"
#endif

#ifndef _RWSTD_NO_PURE_C_HEADERS

extern "C" int mkstemp (char*);

#endif   // _RWSTD_NO_PURE_C_HEADERS

// write `str' using symbolic names from the Portable Character Set (PCS)
// or using the <U00XX> notations for narrow characters outside that set
// if (0 == str), writes out the CHARMAP section of the locale definition
// file for the Portable Character Set (in POSIX-compliant format) 
_TEST_EXPORT void pcs_write (void *fpv, const char *str)
{
    FILE* const fp = _RWSTD_STATIC_CAST (FILE*, fpv);

    // ASCII (ISO-646) character map definition
    static const char* charmap[] = {
        "<NUL>", "<SOH>", "<STX>", "<ETX>", "<EOT>", "<ENQ>", "<ACK>", "<BEL>",
        "<backspace>",
        "<tab>",
        "<newline>",
        "<vertical-tab>",
        "<form-feed>",
        "<carriage-return>",
        "<SO>", "<SI>", "<DLE>", "<DC1>", "<DC2>", "<DC3>", "<DC4>", "<NAK>",
        "<SYN>","<ETB>", "<CAN>", "<EM>", "<SUB>", "<ESC>", "<IS4>", "<IS3>",
        "<IS2>", "<IS1>",
        "<space>",
        /* ! */ "<exclamation-mark>",
        /* " */ "<quotation-mark>",
        /* # */ "<number-sign>",
        /* $ */ "<dollar-sign>",
        /* % */ "<percent-sign>",
        /* & */ "<ampersand>",
        /* ' */ "<apostrophe>",
        /* ( */ "<left-parenthesis>",
        /* ) */ "<right-parenthesis>",
        /* * */ "<asterisk>",
        /* + */ "<plus-sign>",
        /* , */ "<comma>",
        /* - */ "<hyphen>",
        /* . */ "<period>",
        /* / */ "<slash>",
        /* 0 */ "<zero>",
        /* 1 */ "<one>",
        /* 2 */ "<two>",
        /* 3 */ "<three>",
        /* 4 */ "<four>",
        /* 5 */ "<five>",
        /* 6 */ "<six>",
        /* 7 */ "<seven>",
        /* 8 */ "<eight>",
        /* 9 */ "<nine>",
        /* : */ "<colon>",
        /* ; */ "<semicolon>",
        /* < */ "<less-than-sign>",
        /* = */ "<equals-sign>",
        /* > */ "<greater-than-sign>",
        /* ? */ "<question-mark>",
        /* @ */ "<commercial-at>",
        "<A>", "<B>", "<C>", "<D>", "<E>", "<F>", "<G>", "<H>", "<I>", "<J>",
        "<K>", "<L>", "<M>", "<N>", "<O>", "<P>", "<Q>", "<R>", "<S>", "<T>",
        "<U>", "<V>", "<W>", "<X>", "<Y>", "<Z>",
        /* [ */ "<left-square-bracket>",
        /* \ */ "<backslash>",
        /* ] */ "<right-square-bracket>",
        /* ^ */ "<circumflex>",
        /* _ */ "<underscore>",
        /* ` */ "<grave-accent>",
        "<a>", "<b>", "<c>", "<d>", "<e>", "<f>", "<g>", "<h>", "<i>", "<j>",
        "<k>", "<l>", "<m>", "<n>", "<o>", "<p>", "<q>", "<r>", "<s>", "<t>",
        "<u>", "<v>", "<w>", "<x>", "<y>", "<z>",
        /* { */ "<left-brace>",
        /* | */ "<vertical-line>",
        /* } */ "<right-brace>",
        /* ~ */ "<tilde>",
        "<DEL>"
    };

    if (str) {
        // write out `str' using the charmap above
        for (; *str; ++str) {
            const unsigned char uc = _RWSTD_STATIC_CAST (unsigned char, *str);

            if (uc < sizeof charmap / sizeof *charmap)
                fprintf (fp, "%s", charmap [uc]);
            else
                fprintf (fp, "<U%04X>", uc);
        }
    }
    else {

#if !defined (_WIN32) && !defined (_WIN64)
        const char* const codeset = nl_langinfo (CODESET);
#else
        // FIXME: determine the current code page
        const char* const codeset = "UTF-8";
#endif   // _WIN{32,64}

        fprintf (fp, "<code_set_name> \"%s\"\n", codeset);
        fprintf (fp, "<mb_cur_max> 1\n");
        fprintf (fp, "<mb_cur_min> 1\n");

        fprintf (fp, "CHARMAP\n");

        // write out the charmap above
        for (unsigned i = 0; i != sizeof charmap / sizeof *charmap; ++i) {
            fprintf (fp, "%s \\x%02x\n", charmap [i], i);
        }

        // write out duplicate symbolic names to prevent warnings
        fprintf (fp, "<alert> \\x%02x\n", '\a');
        fprintf (fp, "<hyphen-minus> \\x%02x\n", '-');
        fprintf (fp, "<full-stop> \\x%02x\n", '.');
        fprintf (fp, "<solidus> \\x%02x\n", '/');
        fprintf (fp, "<reverse-solidus> \\x%02x\n", '\\');
        fprintf (fp, "<circumflex-accent> \\x%02x\n", '^');
        fprintf (fp, "<underline> \\x%02x\n", '_');
        fprintf (fp, "<low-line> \\x%02x\n", '_');
        fprintf (fp, "<left-curly-bracket> \\x%02x\n", '{');
        fprintf (fp, "<right-curly-bracket> \\x%02x\n", '}');

        fprintf (fp, "END CHARMAP\n");
    }
}


_TEST_EXPORT
const char* rw_tmpnam (char *buf)
{
#ifndef _RWSTD_NO_MKSTEMP
#  define TMP_TEMPLATE      "tmpfile-XXXXXX"

    if (!buf) {
        static char fname_buf [sizeof (P_tmpdir) + sizeof (TMP_TEMPLATE)];

        buf = fname_buf;
        *buf = '\0';
    }

    if ('\0' == *buf) {
        // copy the template to the buffer; make sure there is exactly
        // one path separator character between P_tmpdir and the file
        // name template (it doesn't really matter how many there are
        // as long as it's at least one, but one looks better than two
        // in diagnostic messages)
        size_t len = sizeof (P_tmpdir) - 1;

        memcpy (buf, P_tmpdir, len);
        if (_RWSTD_PATH_SEP != buf [len - 1])
            buf [len++] = _RWSTD_PATH_SEP;

        memcpy (buf + len, TMP_TEMPLATE, sizeof TMP_TEMPLATE);
    }

    // prevent annoying glibc warnings (issued by the linker):
    // the use of `tmpnam' is dangerous, better use `mkstemp'

    const int fd = mkstemp (buf);

    if (-1 == fd) {
        fprintf (stderr, "%s:%d: mkstemp(\"%s\") failed: %s\n",
                 __FILE__, __LINE__, buf, strerror (errno));
        return 0;
    }

    close (fd);

    const char* const fname = buf;

#  undef TMP_TEMPLATE
#else   // if defined (_RWSTD_NO_MKSTEMP)

#  if defined (_WIN32) || defined (_WIN64)

    // create a temporary file name
    char* fname = tempnam (P_tmpdir, ".rwtest-tmp");

    if (fname) {

        static char tmpbuf [256];

        if (0 == buf)
            buf = tmpbuf;

        _RWSTD_ASSERT (strlen (fname) < sizeof (tmpbuf));

        // copy the generated temporary file name to the provided buffer
        strcpy (buf, fname);

        // free the storage allocated by tempnam()
        free (fname);
        fname = buf;
    }
    else {
        fprintf (stderr, "%s:%d: tempnam(\"%s\", \"%s\") failed: %s\n",
                 __FILE__, __LINE__,
                 P_tmpdir, ".rwtest-tmp", strerror (errno));
    }

#  else
#    if defined (__hpux) && defined (_RWSTD_REENTRANT)

    // on HP-UX, in reentrant mode, tmpnam(0) fails by design

    if (!buf) {
        static char tmpbuf [L_tmpnam];
        buf = tmpbuf;
        *buf = '\0';
    }

#    endif   // __hpux && _REENTRANT

    const char* const fname = tmpnam (buf);

    if (!fname)
        fprintf (stderr, "%s:%d: tmpnam(\"%s\") failed: %s\n",
                 __FILE__, __LINE__, buf, strerror (errno));

#  endif   // _WIN{32,64}
#endif   // _RWSTD_NO_MKSTEMP

    return fname;
}


_TEST_EXPORT
size_t rw_fsize (const char *fname)
{
#if defined (_WIN32) || defined (_WIN64)

    // note: both method of obtaining the size of a file
    // just written by a process may fail (i.e., the size
    // will be 0)

#  if 1

    struct _stat sb;

    if (-1 == _stat (fname, &sb))
        return _RWSTD_SIZE_MAX;

    return sb.st_size;

#  else

    // #include <windows.h> for CreateFile() and GetFileSize()
    const HANDLE hfile =
        CreateFile (fname,
                    GENERIC_READ,
                    0,                     // dwShareMode,
                    0,                     // lpSecurityAttributes,
                    OPEN_EXISTING,         // dwCreationDisposition,
                    FILE_ATTRIBUTE_NORMAL, // dwFlagsAndAttributes,
                    0);                    // hTemplateFile

    if (INVALID_HANDLE_VALUE == hfile)
        return _RWSTD_SIZE_MAX;

    const size_t size = GetFileSize (hfile, 0);

    CloseHandle (hfile);

    return size;

#  endif   // 0/1

#else   // if !defined (_WIN{32,64})

    struct stat sb;

    if (stat (fname, &sb) == -1)
        return _RWSTD_SIZE_MAX;

    return sb.st_size;

#endif   // _WIN{32,64}

}


_TEST_EXPORT
void* rw_fread (const char *fname,
                size_t     *size   /* = 0 */,
                const char *mode   /* = "r" */)
{
    // buffer and size supplied by the user
    static char*  usrbuf = 0;
    static size_t usrsize = 0;

    // when called with 0 file name and non-0 size, set the static
    // local buffer for the functions to use in subsequent calls
    // with non-0 `fname' instead of dynamically allocating a new
    // buffer
    if (!fname && size) {

        char* const oldbuf = usrbuf;

        usrbuf  = _RWSTD_CONST_CAST (char*, mode);
        usrsize = usrbuf ? *size : 0;

        return oldbuf;
    }

    static char   buffer [1024];
    static char*  buf     = usrbuf ? usrbuf : buffer;
    static size_t bufsize = usrbuf ? usrsize : sizeof buffer;

    // open the file in the specified mode
    FILE* const fp = fopen (fname, mode);

    if (!fp)
        return 0;

    for (char *bufend = buf; ; ) {
        // compute the total number of bytes read from the file so far
        // and the number of bytes that are still available in the buffer
        const size_t bytes_read  = size_t (bufend - buf);
        const size_t bytes_avail = bufsize - bytes_read;

        // try to read the contents of the file into the buffer
        const size_t nbytes = fread (bufend, 1, bytes_avail, fp);

        if (0 == nbytes) {
            *bufend = '\0';

            // store the number of bytes read
            if (size)
                *size = bytes_read;

            break;
        }

        if (nbytes == bytes_avail) {

            // do not grow user-specified buffer
            if (buf == usrbuf)
                break;

            const size_t newsize = (bufsize + 1) * 2;

            // increase the size of the buffer and continue reading
            char *tmp = new char [newsize];
            memcpy (tmp, buf, bufsize);

            // deallocate buffer only if it's been
            // previously dynamically allocated
            if (buf != buffer)
                delete[] buf;

            bufsize = newsize;
            bufend  = tmp + bytes_read;
            buf     = tmp;
        }

        bufend += nbytes;
    }

    fclose (fp);

    return buf;
}


_TEST_EXPORT
size_t rw_fwrite (const char *fname,
                  const void *buf,
                  size_t      size /* = -1 */,
                  const char *mode /* = "w" */)
{
    FILE *fp = 0;

    if (buf)
        fp = fopen (fname, mode);
    else {
        remove (fname);
        return 0;
    }

    if (!fp)
        return size_t (-1);

    if (size_t (-1) == size)
        size = strlen (_RWSTD_STATIC_CAST (const char*, buf));

    // fwrite() returns the number of elements successfully written
    // set it up so that the number of elements == the number of bytes
    const size_t nbytes = fwrite (buf, 1 /* byte */, size, fp);

    fclose (fp);

    return nbytes;
}


/************************************************************************/

// get the file descriptor that will be returned by the next call
// to creat() or open() (used to detect file descriptor leaks)
_TEST_EXPORT int
rw_nextfd (int *count)
{
    if (count) {

        *count = 0;

#if defined (_WIN32) || defined (_WIN64)

        for (int i = 0; i != 256; ++i) {

            const int ret = _commit (i);

            if (-1 != ret || EBADF != errno)
                ++*count;
        }
#else   // if not Windoze

        for (int i = 0; i != 256; ++i) {
            if (-1 != fcntl (i, F_GETFD))
                ++*count;
        }

#endif   // WIN{32,64}

    }

    const int fd = open (DEV_NULL, O_RDONLY);

    close (fd);

    return fd;
}
