/************************************************************************
 *
 * locale.cpp - definitions of locale helpers
 *
 * $Id: locale.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2001-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC


#include <rw_locale.h>

#include <environ.h>      // for rw_putenv()
#include <file.h>         // for SHELL_RM_RF, rw_tmpnam
#include <rw_process.h>   // for rw_system()
#include <rw_printf.h>    // for rw_snprintf()
#include <rw_fnmatch.h>   // for rw_fnmatch()
#include <rw_braceexp.h>  // for rw_shell_expand()
#include <driver.h>       // for rw_error()

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
#include <sys/stat.h>   // for stat

#if !defined  (_WIN32) && !defined (_WIN64)
#  include <unistd.h>
#  include <sys/wait.h>   // for WIFEXITED(), WIFSIGNALED(), WTERMSIG()
#else
#  include <io.h>
#  include <crtdbg.h> // for _malloc_dbg()
#endif

#include <ios>        // for ios::*
#include <limits>     // for numeric_limits
#include <locale>     // for money_base::pattern

#include <assert.h>   // for assert
#include <limits.h>   // for PATH_MAX
#include <locale.h>   // for LC_XXX macros, setlocale
#include <stdarg.h>   // for va_copy, va_list, ...
#include <stdio.h>    // for fgets, remove, sprintf, ...
#include <stdlib.h>   // for getenv, free, malloc, realloc
#include <string.h>   // for strcat, strcpy, strlen, ...
#include <ctype.h>
#include <wchar.h>    // for wcslen, ...

#ifndef _MSC_VER
#  include <clocale>
#  ifndef LC_MESSAGES
#    define LC_MESSAGES _RWSTD_LC_MESSAGES
#  endif   // LC_MESSAGES
#  define EXE_SUFFIX    ""
#else   // if MSVC
#  define _RWSTD_NO_LANGINFO
#  define EXE_SUFFIX    ".exe"
#endif  // _MSC_VER

#ifndef _RWSTD_NO_LANGINFO
#  include <langinfo.h>
#endif

#if !defined (PATH_MAX) || PATH_MAX < 128 || 4096 < PATH_MAX
   // deal  with undefined, bogus, or excessive values
#  undef  PATH_MAX
#  define PATH_MAX   1024
#endif


#define TOPDIR   "TOPDIR"   /* the TOPDIR environment variable */
#define BINDIR   "BINDIR"   /* the BINDIR environment variable */


#if _RWSTD_PATH_SEP == '/'
#  define SLASH    "/"
#  define IS_ABSOLUTE_PATHNAME(path)   (_RWSTD_PATH_SEP == *(path))
#else
#  define SLASH    "\\"
#  define IS_ABSOLUTE_PATHNAME(path)                    \
       (   (   'A' <= *(path) && 'Z' >= *(path)         \
            || 'a' <= *(path) && 'z' >= *(path))        \
        && ':' == (path)[1]                             \
        && _RWSTD_PATH_SEP == (path)[2])
#endif

// relative paths to the etc/nls directory and its subdirectories
#define RELPATH        "etc" SLASH "nls"
#define TESTS_ETC_PATH "tests" SLASH "etc"
            
// extension of the catalog file
#ifndef _WIN32
#  define RW_CAT_EXT ".cat"
#else
#  define RW_CAT_EXT ".dll"
#endif

/**************************************************************************/

_TEST_EXPORT int
rw_locale (const char *args, const char *fname)
{
    // use BINDIR to determine the location of the locale command
    const char* bindir = getenv (BINDIR);
    if (!bindir)
        bindir = ".." SLASH "bin";

    int ret;

    if (fname)
        ret = rw_system ("%s%slocale%s %s",
                         bindir, SLASH, EXE_SUFFIX, args);
    else
        ret = rw_system ("%s%slocale%s %s >%s",
                         bindir, SLASH, EXE_SUFFIX, args, fname);

    return ret;
}

/**************************************************************************/

_TEST_EXPORT const char*
rw_localedef (const char *args,
              const char* src, const char *charmap, const char *locname)
{
    assert (src && charmap);

    // create a fully qualified pathname of the locale database
    // when (locname == 0), the pathname is computed by appending
    // the name of the character map file `charmap' to the name
    // of the locale definition file `src'
    // otherwise, when `locname' is not a pathname, the pathname
    // of the locale database is formed by appending `locname'
    // to the name of the locale root directory
    static char locale_path [PATH_MAX];

    const char* locale_root = getenv (LOCALE_ROOT_ENVAR);
    if (!locale_root)
        locale_root = ".";

    assert (  strlen (locale_root)
            + strlen (src)
            + strlen (charmap)
            + 2 < sizeof locale_path);

    strcpy (locale_path, locale_root);

    if (locname) {
        if (strchr (locname, _RWSTD_PATH_SEP))
            strcpy (locale_path, locname);
        else {
            strcat (locale_path, SLASH);
            strcat (locale_path, locname);
        }
    }
    else {
        // compute the locale pathname from `src', `charmap',
        // and `locale_root'
        strcpy (locale_path, locale_root);
        strcat (locale_path, SLASH);

        const char *slash = strrchr (src, _RWSTD_PATH_SEP);
        slash = slash ? slash + 1 : src;

        strcat (locale_path, src);
        strcat (locale_path, ".");

        slash = strrchr (charmap, _RWSTD_PATH_SEP);
        slash = slash ? slash + 1 : charmap;

        strcat (locale_path, slash);
    }

    // check to see if the locale database already exists and
    // if so, return immediately the locale filename to the caller
#if !defined (_MSC_VER)
    struct stat sb;
    if (!stat (locale_path, &sb)) {
#else
    struct _stat sb;
    if (!_stat (locale_path, &sb)) {
#endif
        return strrchr (locale_path, _RWSTD_PATH_SEP) + 1;
    }

    // otherwise, try to create the locale database

    // use TOPDIR to determine the root of the source tree
    const char* const topdir = getenv (TOPDIR);
    if (!topdir || !*topdir) {
        rw_error (0, __FILE__, __LINE__,
                  "the environment variable %s is %s",
                  TOPDIR, topdir ? "empty" : "undefined");
        return 0;
    }

    // use BINDIR to determine the location of the localedef command
    const char* bindir = getenv (BINDIR);
    if (!bindir)
        bindir = ".." SLASH "bin";

    // if `src' is relative pathname (or a filename) construct the fully
    // qualified absolute pathname to the locale definition file from it
    char src_path [PATH_MAX];

    if (!IS_ABSOLUTE_PATHNAME (src)) {
        strcpy (src_path, topdir);
        strcat (src_path, SLASH RELPATH SLASH "src" SLASH);
        strcat (src_path, src);

        // if the file doesn't exist, see if there is a file
        // with that name in the locale root directory (e.g.,
        // a temporary file)
        FILE* const file_exists = fopen (src_path, "r");
        if (file_exists)
            fclose (file_exists);
        else {
            strcpy (src_path, locale_root);
            strcat (src_path, SLASH);
            strcat (src_path, src);
        }

        src = src_path;
    }

    char charmap_path [PATH_MAX];
    if (!IS_ABSOLUTE_PATHNAME (charmap)) {
        strcpy (charmap_path, topdir);
        strcat (charmap_path, SLASH RELPATH SLASH "charmaps" SLASH);
        strcat (charmap_path, charmap);

        // if the file doesn't exist, see if there is a file
        // with that name in the locale root directory (e.g.,
        // a temporary file)
        FILE* const file_exists = fopen (charmap_path, "r");
        if (file_exists)
            fclose (file_exists);
        else {
            strcpy (charmap_path, locale_root);
            strcat (charmap_path, SLASH);
            strcat (charmap_path, charmap);
        }

        charmap = charmap_path;
    }

    if (!args)
        args = "";

    const int ret = rw_system ("%s%slocaledef%s %s -c -f %s -i %s %s",
                               bindir, SLASH, EXE_SUFFIX, args,
                               charmap, src, locale_path);

    // return the unqualified locale file name on success or 0 on failure
    return ret ? (char*)0 : strrchr (locale_path, _RWSTD_PATH_SEP) + 1;
}

/**************************************************************************/

extern "C" {

static char rw_locale_root [PATH_MAX];

static void atexit_rm_locale_root ()
{
    const bool e = rw_enable (rw_error, false);
    const bool n = rw_enable (rw_note , false);

    // remove temporary locale databases created by the test
    rw_system (SHELL_RM_RF "%s", rw_locale_root);

    rw_enable (rw_note , n);
    rw_enable (rw_error, e);
}

}

_TEST_EXPORT const char*
rw_set_locale_root ()
{
    // set any additional environment variables defined in
    // the RW_PUTENV environment variable (if it exists)
    rw_putenv (0);

    // create a temporary directory for files created by the test
    const char* const locale_root = rw_tmpnam (rw_locale_root);
    if (!locale_root)
        return 0;

    char envvar [sizeof LOCALE_ROOT_ENVAR + sizeof rw_locale_root] =
        LOCALE_ROOT_ENVAR "=";

    strcat (envvar, locale_root);

    // remove temporary file if mkstemp() rw_tmpnam() called mkstemp()
    if (rw_system (SHELL_RM_RF " %s", locale_root)) {

#if defined (_WIN32) || defined (_WIN64)
        // ignore errors on WIN32 where the stupid DEL command
        // fails even with /Q /S when the files don't exist
#else
        // assume a sane implementation of SHELL_RM_RF
        return 0;
#endif   // _WIN{32,64}
    }

    if (rw_system ("mkdir %s", locale_root))
        return 0;

    // set the "RWSTD_LOCALE_ROOT" environment variable
    // where std::locale looks for locale database files
    rw_putenv (envvar);

    rw_error (0 == atexit (atexit_rm_locale_root), __FILE__, __LINE__,
              "atexit(atexit_rm_locale_root) failed: %m");

    return locale_root;
}

/**************************************************************************/

_TEST_EXPORT char*
rw_locales (int loc_cat, const char* grep_exp, bool prepend_c_loc)
{
    static char deflocname [3] = "C\0";
    static char* slocname = 0;

    static const size_t grow_size = 5120;
    static size_t size       = 0;         // the number of elements in the array
    static size_t total_size = grow_size; // the size of the array
    static int    last_cat   = loc_cat;   // last category

#ifndef _MSC_VER
#  define _QUIET_MALLOC(n)  malloc(n)
#  define _QUIET_FREE(p)    free(p)
#else
    // prevent allocation from causing failures in tests that
    // keep track of storage allocated in _NORMAL_BLOCKS
#  define _QUIET_MALLOC(n) _malloc_dbg (n, _CLIENT_BLOCK, 0, 0)
#  define _QUIET_FREE(p)   _free_dbg (p, _CLIENT_BLOCK);
#endif

    // allocate first time through
    if (!slocname) {
        slocname = _RWSTD_STATIC_CAST (char*, _QUIET_MALLOC (total_size));

        if (!slocname)
            return deflocname;

        *slocname = '\0';
    }

    // return immediately if buffer is already initialized
    if (*slocname && loc_cat == last_cat)
        return slocname;

    // remmeber the category we were last called with
    last_cat = loc_cat;

    char* locname = slocname;

    char* save_localename = 0;
    char  namebuf [PATH_MAX];

    if (loc_cat != _UNUSED_CAT) {
        // copy the locale name, the original may be overwitten by libc
        save_localename = strcpy (namebuf, setlocale (loc_cat, 0));
    }

    const char* const fname = rw_tmpnam (0);

    if (!fname) {
        return deflocname;   // error
    }

    // make sure that grep_exp is <= 80
    if (grep_exp && 80 < strlen (grep_exp)) {
        abort ();
    }

    // execute a shell command and redirect its output into the file
    const int exit_status =
        grep_exp && *grep_exp
      ? rw_system ("locale -a | grep \"%s\" > %s", grep_exp, fname)
      : rw_system ("locale -a > %s", fname);

    if (exit_status) {
        return deflocname;   // error
    }

    // open file containing the list of installed locales
    FILE *file = fopen (fname, "r");

    if (file) {

        char linebuf [256];

        // even simple locale names can be very long (e.g., on HP-UX,
        // where a locale name always consists of the names of all
        // categories, such as "C C C C C C")
        char last_name [256];
        *last_name = '\0';

        // put the C locale at the front
        if (prepend_c_loc) {
            strcpy (locname, deflocname);

            const size_t defnamelen = strlen (deflocname) + 1;
            locname += defnamelen; 
            size    += defnamelen;
        }

        // if successful, construct a char array with the locales
        while (fgets (linebuf, sizeof linebuf, file)) {

            const size_t linelen = strlen (linebuf);

            linebuf [linelen ? linelen - 1 : 0] = '\0';

            // don't allow C locale to be in the list again
            // if we put it at the front of the locale list
            if (prepend_c_loc && !strcmp (linebuf, deflocname))
                continue;

#ifdef _RWSTD_OS_SUNOS

            const char iso_8859_pfx[] = "iso_8859_";

            // avoid locales named common and iso_8859_* on SunOS
            // since they are known to cause setlocale() to fail
            if (   !strcmp ("common", linebuf)
                || sizeof iso_8859_pfx <= linelen 
                && !memcmp (iso_8859_pfx, linebuf, sizeof iso_8859_pfx - 1))
                continue;

#endif   // _RWSTD_OS_SUNOS

            // if our buffer is full then dynamically allocate a new one
            size += linelen;
            if (total_size < size) {
                total_size += grow_size;

                char* tmp =
                    _RWSTD_STATIC_CAST (char*, _QUIET_MALLOC (total_size));
                if (!tmp)
                    break;

                memcpy (tmp, slocname, total_size - grow_size);

                _QUIET_FREE (slocname);

                slocname = tmp;
                locname  = slocname + size - linelen;
            }

#ifdef _WIN64

            // prevent a hang (OS/libc bug?)
            strcpy (locname, linebuf);
            locname += linelen;

#else   // if !defined (_WIN64)
            if (loc_cat != _UNUSED_CAT) {

                // set the C locale to verify that the name is valid
                const char *name = setlocale (loc_cat, linebuf);

                // if it is and if the actual locale name different
                // from the last one, append it to the list
                if (name && strcmp (last_name, name)) {
                    strcpy (locname, linebuf);
                    locname += linelen;

                    // save the last locale name
                    assert (strlen (name) < sizeof last_name);
                    strcpy (last_name, name);
                }
            }
            else {
                strcpy (locname, linebuf);
                locname += linelen;
            }

#endif   // _WIN64

        }
        *locname = '\0';
    }

    if (loc_cat != _UNUSED_CAT)
        setlocale (loc_cat, save_localename);

    // close before removing
    fclose (file);

    remove (fname);

    return *slocname ? slocname : deflocname;
}

/**************************************************************************/

// finds a multibyte character that is `bytes' long if `bytes' is less
// than or equal to MB_CUR_MAX, or the longest multibyte sequence in
// the current locale
static const char*
_get_mb_char (char *buf, size_t bytes)
{
    _RWSTD_ASSERT (0 != buf);

    *buf = '\0';

    if (0 == bytes)
        return buf;

    const bool exact = bytes <= size_t (MB_CUR_MAX);

    if (!exact)
        bytes = MB_CUR_MAX;

    wchar_t wc;

    // search the first 64K characters sequentially
    for (wc = wchar_t (1); wc != wchar_t (0xffff); ++wc) {

        if (   int (bytes) == wctomb (buf, wc)
            && int (bytes) == mblen (buf, bytes)) {
            // NUL-terminate the multibyte character of the requested length
            buf [bytes] = '\0';
            break;
        }

        *buf = '\0';
    }

#if 2 < _RWSTD_WCHAR_SIZE

    // if a multibyte character of the requested size is not found
    // in the low 64K range, try to find one using a random search
    if (wchar_t (0xffff) == wc) {

        // iterate only so many times to prevent an infinite loop
        // in case when MB_CUR_MAX is greater than the longest
        // multibyte character
        for (int i = 0; i != 0x100000; ++i) {

            wc = wchar_t (rand ());

            if (RAND_MAX < 0x10000) {
                wc <<= 16;
                wc |=  wchar_t (rand ());
            }

            if (   int (bytes) == wctomb (buf, wc)
                && int (bytes) == mblen (buf, bytes)) {
                // NUL-terminate the multibyte character
                buf [bytes] = '\0';
                break;
            }

            *buf = '\0';
        }
    }

#endif   // 2 < _RWSTD_WCHAR_SIZE

    // return 0 on failure to find a sequence exactly `bytes' long
    return !exact || bytes == strlen (buf) ? buf : 0;
}


_TEST_EXPORT size_t
rw_get_mb_chars (rw_mbchar_array_t mb_chars)
{
    _RWSTD_ASSERT (0 != mb_chars);

    const char* mbc = _get_mb_char (mb_chars [0], size_t (-1));

    if (0 == rw_note (0 != mbc, __FILE__, __LINE__,
                      "failed to find any multibyte characters "
                      "in locale \"%s\" with MB_CUR_MAX = %u",
                      setlocale (LC_CTYPE, 0), MB_CUR_MAX))
        return 0;

    size_t mb_cur_max = strlen (mbc);

    if (_RWSTD_MB_LEN_MAX < mb_cur_max)
        mb_cur_max = _RWSTD_MB_LEN_MAX;

    // fill each element of `mb_chars' with a multibyte character
    // of the corresponding length
    for (size_t i = mb_cur_max; i; --i) {

        // try to generate a multibyte character `i' bytes long
        mbc = _get_mb_char (mb_chars [i - 1], i);

        if (0 == mbc) {
            // zh_CN.gb18030 and zh_TW.euctw on Linux are examples
            // of multibyte locales where MB_CUR_MAX == 4 but,
            // apparently, no 3-byte characters
            if (0 == rw_note (mb_cur_max <= i, __FILE__, __LINE__,
                              "failed to find %u-byte characters "
                              "in locale \"%s\" with MB_CUR_MAX = %u",
                              i, setlocale (LC_CTYPE, 0), MB_CUR_MAX)) {
                mb_cur_max = 0;
                break;
            }
            --mb_cur_max;
        }
    }

    return mb_cur_max;
}


_TEST_EXPORT size_t
rw_get_wchars (wchar_t *wbuf, size_t bufsize, int nbytes /* = 0 */)
{
    if (0 == bufsize)
        return 0;

    char tmp [_RWSTD_MB_LEN_MAX];

    size_t nchars = 0;

    for (int i = 0; i != 65536; ++i) {

        // determine whether the wide character is valid
        // and if so, the length of the multibyte character
        // that corresponds to it
        const wchar_t wc = wchar_t (i);
        const int len = wctomb (tmp, wc);

        if (nbytes == 0 && 0 < len || nbytes != 0 && nbytes == len) {
            // if the requested length is 0 (i.e., the caller doesn't
            // care) and the character is valid, store it
            // if the requested length is non-zero (including -1),
            // and the value returned from mblen() is the same, store
            // it (this makes it possible to find invalid characters
            // as well as valid ones)
            wbuf [nchars++] = wc;
            if (nchars == bufsize)
                return nchars;
        }
    }

#if 2 < _RWSTD_WCHAR_SIZE

    // try to find the remaining wide characters by a random
    // search, iterating only so many times to prevent an
    // infinite loop
    for (int i = 0; i != 0x100000; ++i) {

        // make a wide character with a random bit pattern
        wchar_t wc = wchar_t (rand ());

        if (RAND_MAX < 0x10000) {
            wc <<= 16;
            wc |= wchar_t (rand ());
        }

        const int len = wctomb (tmp, wc);

        if (nbytes == 0 && 0 < len || nbytes != 0 && nbytes == len) {
            wbuf [nchars++] = wc;
            if (nchars == bufsize)
                return nchars;
        }
    }

#endif   // 2 < _RWSTD_WCHAR_SIZE

    return nchars;

}


_TEST_EXPORT const char*
rw_find_mb_locale (size_t            *mb_cur_max,
                   rw_mbchar_array_t  mb_chars)
{
    _RWSTD_ASSERT (0 != mb_cur_max);
    _RWSTD_ASSERT (0 != mb_chars);

    if (2 > _RWSTD_MB_LEN_MAX) {
        rw_warn (0, __FILE__, __LINE__, "MB_LEN_MAX = %d, giving up",
                 _RWSTD_MB_LEN_MAX);
        return 0;
    }

    static const char *mb_locale_name;

    char saved_locale_name [1024];
    strcpy (saved_locale_name, setlocale (LC_CTYPE, 0));

    _RWSTD_ASSERT (strlen (saved_locale_name) < sizeof saved_locale_name);

    *mb_cur_max = 0;

    // iterate over all installed locales
    for (const char *name = rw_locales (_RWSTD_LC_CTYPE, 0); name && *name;
         name += strlen (name) + 1) {

        if (setlocale (LC_CTYPE, name)) {

            // try to generate a set of multibyte characters
            // with lengths from 1 and MB_CUR_MAX (or less)
            const size_t cur_max = rw_get_mb_chars (mb_chars);

            if (*mb_cur_max < cur_max) {
                *mb_cur_max    = cur_max;
                mb_locale_name = name;

                // break when we've found a multibyte locale
                // with the longest possible encoding
                if (_RWSTD_MB_LEN_MAX == *mb_cur_max)
                    break;
            }
        }
    }

    if (*mb_cur_max < 2) {
        rw_warn (0, __FILE__, __LINE__,
                 "failed to find a full set of multibyte "
                 "characters in locale \"%s\" with MB_CUR_MAX = %u "
                 "(computed)", mb_locale_name, *mb_cur_max);
        mb_locale_name = 0;
    }
    else {
        // (re)generate the multibyte characters for the saved locale
        // as they may have been overwritten in subsequent iterations
        // of the loop above (while searching for a locale with greater
        // value of MB_CUR_MAX)
        setlocale (LC_CTYPE, mb_locale_name);
        rw_get_mb_chars (mb_chars);
    }

    setlocale (LC_CTYPE, saved_locale_name);

    return mb_locale_name;
}


/**************************************************************************/

_TEST_EXPORT const char*
rw_create_locale (const char *charmap, const char *locale)
{
    // only one locale is enough (avoid invoking localedef more than once)
    static const char* locname;
    const char* locale_root;

    if (locname)
        return locname;

    // set up RWSTD_LOCALE_ROOT and other environment variables
    locale_root = rw_set_locale_root ();

    if (0 == locale_root)
        return 0;

    // create a temporary locale definition file that exercises as
    // many different parts of the collate standard as possible
    char srcfname [PATH_MAX];
    if (rw_snprintf (srcfname, PATH_MAX, "%s%slocale.src",
                     locale_root, SLASH) < 0) {
        return 0;
    }

    FILE *fout = fopen (srcfname, "w");

    if (!fout) {
        rw_error (0, __FILE__, __LINE__,
                  "fopen(#%s, \"w\") failed: %m", srcfname);
        return 0;
    }

    fprintf (fout, "%s", locale);

    fclose (fout);

    // create a temporary character map file
    char cmfname [PATH_MAX];
    if (rw_snprintf (cmfname, PATH_MAX, "%s%scharmap.src",
                     locale_root, SLASH) < 0) {
        return 0;
    }

    fout = fopen (cmfname, "w");

    if (!fout) {
        rw_error (0, __FILE__, __LINE__,
                  "fopen(%#s, \"w\") failed: %m", cmfname);
        return 0;
    }

    fprintf (fout, "%s", charmap);

    fclose (fout);

    locname = "test-locale";

    // process the locale definition file and character map
    if (0 == rw_localedef ("-w", srcfname, cmfname, locname))
        locname = 0;

    return locname;
}


/**************************************************************************/

static const char*
_rw_locale_names;

_TEST_EXPORT const char* const&
rw_opt_locales = _rw_locale_names;


_TEST_EXPORT int
rw_opt_setlocales (int argc, char* argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {
        static const char helpstr[] = {
            "Use the locales specified by the space-parated list of locale"
            "names given by <arg>.\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    // the option requires an equals sign followed by an optional argument
    char *args = strchr (argv [0], '=');

    RW_ASSERT (0 != args);

    // small static buffer should be sufficient in most cases
    static char buffer [256];

    const size_t len = strlen (++args);

    // dynamically allocate a bigger buffer when the small buffer
    // isn't big enough (let the dynamically allocated buffer leak)
    char* const locale_names =
        sizeof buffer < len + 2 ? (char*)malloc (len + 2) : buffer;

    if (0 == locale_names)
        return 1;

    locale_names [len]     = '\0';
    locale_names [len + 1] = '\0';

    memcpy (locale_names, args, len);

    for (char *next = locale_names; ; ) {
        next = strpbrk (next, ", ");
        if (next)
            *next++ = '\0';
        else
            break;
    }

    _rw_locale_names = locale_names;

    // return 0 on success
    return 0;
}


/**************************************************************************/

_TEST_EXPORT int
rw_create_catalog (const char * catname, const char * catalog)
{
    RW_ASSERT (catname && catalog);

    FILE* const f = fopen (catname, "w");

    if (!f)
        return -1;

#ifndef _WIN32

    for (int i = 1; *catalog; ++catalog, ++i) {
        fprintf (f, "$set %d This is Set %d\n", i, i);
        for (int j = 1; *catalog; catalog += strlen (catalog) + 1, ++j)
             fprintf (f, "%d %s\n", j, catalog);
    }

#else   // if defined (_WIN32)

    fprintf (f, "STRINGTABLE\nBEGIN\n");

    for (int i = 1; *catalog; ++catalog) {
        for (; *catalog; catalog += strlen (catalog) + 1, ++i)
            fprintf (f, "%d \"%s\"\n", i, catalog);
    }

    fprintf (f, "END\n");

#endif   // _WIN32

    fclose (f);

    char *cat_name = new char [strlen (catname) + 1];
    strcpy (cat_name, catname);
    if (char *dot = strrchr (cat_name, '.'))
        *dot = '\0';

    const int ret = rw_system ("gencat %s" RW_CAT_EXT " %s",
                               cat_name, catname);

    delete[] cat_name;

    remove (catname);

    return ret;
}

inline bool
_rw_isspace (char ch)
{
    return 0 != isspace ((unsigned char)ch);
}

inline char
_rw_toupper (char ch)
{
    return toupper ((unsigned char)ch);
}

inline char
_rw_tolower (char ch)
{
    return tolower ((unsigned char)ch);
}

// our locale database is a big array of these
struct _rw_locale_entry {
    char locale_name    [64]; // English_United States.1252
    char canonical_name [32]; // en-US-1-1252
    struct _rw_locale_entry* next;
};

struct _rw_locale_array {
    _rw_locale_entry* entries;
    _RWSTD_SIZE_T count;
};

struct _rw_lookup_entry_t {
    const char* native;
    const char* canonical;
};

extern "C" {

static int
_rw_lookup_comparator (const void* _lhs, const void* _rhs)
{
    const _rw_lookup_entry_t* lhs = (const _rw_lookup_entry_t*)_lhs;
    const _rw_lookup_entry_t* rhs = (const _rw_lookup_entry_t*)_rhs;

    return strcmp (lhs->native, rhs->native);
}

}  // extern "C"

struct _rw_lookup_table_t {

    _rw_lookup_table_t ()
        : entries_ (0), count_ (0), table_data_ (0) {
    }

    ~_rw_lookup_table_t () {
        if (entries_)
            free (entries_);
        entries_ = 0;
        count_   = 0;

        if (table_data_)
            free (table_data_);
        table_data_ = 0;
    }

    bool load_from_file (const char* path, const char* file, int upper_or_lower);

    const _rw_lookup_entry_t* get_entries () const {
        return entries_;
    }

    size_t get_num_entries () const {
        return count_;
    }

    const char* get_canonical_name (const char* name) const;

private:

    _rw_lookup_entry_t* entries_;
    size_t              count_;
    char*               table_data_;

private:
    // intentionally hidden
    _rw_lookup_table_t (const _rw_lookup_table_t& rhs);
    _rw_lookup_table_t& operator= (const _rw_lookup_table_t& rhs);
};


static void
_rw_reset_locales (_rw_locale_array* a)
{
    // reset the next pointers so that all locales are included
    for (size_t i = 0; i < a->count; ++i)
        a->entries [i].next = &a->entries [i+1];
    a->entries [a->count - 1].next = 0;
}

//
// this function gets a list of all of the locales that are installed. it
// only queries the system once and caches the result for use in future
// requests.
//
static _rw_locale_array
_rw_all_locales ()
{
    static _rw_locale_array result;

    // if we have already collection, reuse it
    if (result.entries && result.count != 0) {
        _rw_reset_locales (&result);
        return result;
    }

    static _rw_locale_entry fallback = {
        "C", "C", 0
    };

    result.entries = &fallback;
    result.count   = 1;

    const char* const fname = rw_tmpnam (0);
    if (!fname) {
        return result;
    }

    const int status = rw_system ("locale -a > %s", fname);
    if (status) {
        return result;
    }

    FILE* file = fopen (fname, "r");
    if (file) {

        // looks to be the first time, get a list of all locales
        const size_t entry_size = sizeof (_rw_locale_entry);
        const size_t grow_size  = 64;
        
        _rw_locale_entry* entries = 0;
        size_t capacity = 0;
        size_t size     = 0;

        // load the native to canonical lookup table
        _rw_lookup_table_t languages_map;
        _rw_lookup_table_t countries_map;
        _rw_lookup_table_t encodings_map;

        // use TOPDIR to determine the root of the source tree
        const char* const topdir = getenv (TOPDIR);
        if (!topdir || !*topdir) {
            rw_error (0, __FILE__, __LINE__,
                      "the environment variable %s is %s",
                      TOPDIR, topdir ? "empty" : "undefined");
        }
        else {
            // we should be loading this from some other well
            // known path so we don't depend on $TOPDIR. sadly
            // __FILE__ is not an absolute path on msvc

            char path [PATH_MAX];
            strcpy (path, topdir);
            strcat (path, SLASH RELPATH SLASH);

            // load mapping from local to canonical names
            languages_map.load_from_file (path, "languages", -1);
            countries_map.load_from_file (path, "countries",  1);
            encodings_map.load_from_file (path, "encodings",  1);
        }

        char locale [128];
        while (fgets (locale, sizeof (locale), file)) {

            // ensure sufficient space in array
            if (! (size < capacity)) {
                capacity += grow_size;

                _rw_locale_entry* new_entries =
                    _RWSTD_STATIC_CAST(_rw_locale_entry*,
                                    _QUIET_MALLOC (entry_size * capacity));
                if (!new_entries) {
                    break;
                }

                memcpy (new_entries, entries, entry_size * size);

                // deallocate the old buffer
                _QUIET_FREE (entries);

                entries = new_entries;
            }

            // grab entry to update
            _rw_locale_entry* const entry = &entries [size];
            entry->next = 0;

            const size_t len = strlen (locale);
            locale [len ? len - 1 : 0] = '\0';

            // make sure that the named locale is one that we can use
            if (!setlocale (LC_CTYPE, locale)) {
                
                rw_note (0, __FILE__, __LINE__,
                         "setlocale() failed for '%s'", locale);

                continue;

            }

            // is not an alias for the C or POSIX locale
            else if (!strcmp (locale, "C") || !strcmp (locale, "POSIX")) {
                continue; // we don't do C/POSIX locale
            }

            // has a name that is short enough for our buffer
            else if (sizeof (entry->locale_name) < len) {

                rw_note (0, __FILE__, __LINE__,
                         "locale name '%s' was to long for fixed buffer",
                         locale);

                continue; // locale name didn't fit, so we skip it
            }

#ifndef _RWSTD_NO_LANGINFO
            char codeset [40];

            int i = 0;
            for (const char* charset = nl_langinfo (CODESET);
                 *charset;
                 ++charset) {
                codeset [i++] = _rw_toupper (*charset);
            }

            codeset [i] = '\0';
#endif

            // copy the locale name
            strcpy (entry->locale_name, locale);

            // attempt to split line into parts
            char* extension = strrchr (locale, '@');
            if (extension) {
                *extension++ = '\0';
            }

            char* encoding = strrchr (locale, '.');
            if (encoding) {
                *encoding++ = '\0';

                for (int n = 0; encoding [n]; ++n)
                    encoding [n] = _rw_toupper (encoding [n]);
            }

            char* country = strrchr (locale, '_');
            if (country) {
                *country++ = '\0';

                for (int n = 0; country [n]; ++n)
                    country [n] = _rw_toupper (country [n]);
            }
            
            char* language = locale;

            for (int n = 0; language [n]; ++n)
                language [n] = _rw_tolower (language [n]);

            // use mapping databases to find the canonical
            // names for each part of the locale name

            const char* planguage =
                languages_map.get_canonical_name (language);
            if (!planguage)
                planguage = language;

            // if country name was provided, then lookup in the country
            // mapping. otherwise use language to guess country.
            const char* pcountry =
                  countries_map.get_canonical_name (country);
            if (!pcountry)
                pcountry = country;

#ifndef _RWSTD_NO_LANGINFO
            const char* pencoding =
                encodings_map.get_canonical_name (codeset);
            if (!pencoding)
                pencoding = codeset;
#else
            const char* pencoding =
                encodings_map.get_canonical_name (encoding);
            if (!pencoding)
                pencoding = encoding;
#endif

            // require all three mappings are valid
            if (!planguage || !*planguage) {

                //rw_note (0, __FILE__, __LINE__,
                //    "failed to get language for locale '%s'",
                //    entry->locale_name);

                continue;
            }
            else if (!pcountry || !*pcountry) {

                //rw_note (0, __FILE__, __LINE__,
                //    "failed to get country for locale '%s'",
                //    entry->locale_name);

                continue;
            }
            else if (!pencoding || !*pencoding) {

                //rw_note (0, __FILE__, __LINE__,
                //    "failed to get codeset for locale '%s'",
                //    entry->locale_name);

                continue;
            }

            // the canonical name for lookup
            sprintf (entry->canonical_name, "%s-%s-%d-%s",
                     planguage, pcountry, int (MB_CUR_MAX), pencoding);

            //
            // eliminate locales that are duplicates according to
            // canonical name. we do this because the setlocale()
            // doesn't seem to tell us about aliases.
            //

            bool duplicate = false;

            // search backward as matches are more likely to be near
            // the back
            for (size_t e = size; 0 != e; --e) {

                if (!strcmp (entries [e-1].canonical_name,
                             entry->canonical_name)) {

                    //rw_note (0, __FILE__, __LINE__,
                    //         "ignoring duplicate locale '%s'",
                    //         entry->locale_name);

                    duplicate = true;

                    break;
                }
            }

            if (!duplicate)
               size += 1;
        }

        fclose (file);

        // delete temp file
        remove (fname);

        // link all of the nodes into result
        if (size) {
            result.entries = entries;
            result.count   = size;
        }
        else
            _QUIET_FREE (entries);
    }

    // link each node to the next. if the array is sorted,
    // the list will be sorted.
    _rw_reset_locales (&result);

    return result;
}

_TEST_EXPORT char*
rw_locale_query (int loc_cat, const char* query, size_t wanted)
{
    // query format <language>-<COUNTRY>-<MB_CUR_LEN>-<CODESET>

    // the null query string will return any locale
    if (!query)
        query = "*";

    if (!wanted)
        wanted = _RWSTD_SIZE_MAX;

    char buf [256];

    // get a brace expanded representation of query, each expansion
    // is a null terminated string. the entire buffer is also null 
    // terminated
    char* res = rw_shell_expand (query, 0, buf, sizeof (buf), '\0');
    if (!res)
        return 0;

    // cache the locale name so we can restore later, this must happen
    // before _rw_all_locales() because that function just changes the
    // locale without restoring it
    char save_locale [PATH_MAX];
    strcpy (save_locale, setlocale (LC_ALL, 0));

    const _rw_locale_array all = _rw_all_locales ();

    // make these local and require the user to deallocate
    // with free?
    static char*  string   = 0;
    static size_t length   = 0;
    static size_t capacity = 0;

    _rw_locale_entry rejects;
    rejects.canonical_name [0] = '\0';
    rejects.locale_name    [0] = '\0';
    rejects.next = all.entries;

    // for each result locale name
    size_t count = 0;
    for (const char* name = res; *name; name += strlen (name) + 1)
    {
        _rw_locale_entry* dummy = &rejects;

        // linear search for matches in the reject list
        while (dummy->next)
        {
            // append name to the output buffer
            const _rw_locale_entry* entry = dummy->next;

            // see if we found a match
            if (rw_fnmatch (name, entry->canonical_name, 0)) {

                // not a match, advance past it leaving it in the
                // rejects list
                dummy = dummy->next;

                // and move along to next one
                continue;

            }

            // remove the accepted entry from the reject list
            // so we will not include it again
            dummy->next = entry->next;

            // if the user requested locales from a specific category
            if (loc_cat != _UNUSED_CAT) {

                // make sure that the matching locale has the specified
                // locale category and that we can use it.
                if (!setlocale (loc_cat, entry->locale_name)) {

                    // if we can't use it, then bail. this effectively
                    // removes the locale from the rejects list and
                    // doesn't add it to the accepted list.
                    continue;
                }
            }

            const size_t add_length = strlen (entry->locale_name) + 1;
            const size_t new_length = length + add_length;

            // grow buffer if necessary
            if (! (new_length < capacity)) {

                while (capacity < new_length)
                    capacity += 256;

                // one additional character for the second null
                char* new_string =
                    _RWSTD_STATIC_CAST(char*, _QUIET_MALLOC (capacity + 1));
                if (!new_string) {

                    // setup to get out of outer loop
                    count = wanted;

                    // get out of inner loop
                    break;
                }

                memcpy (new_string, string, length);

                _QUIET_FREE (string);

                string = new_string;
            }

            // append the name, and update the length
            memcpy (string + length, entry->locale_name, add_length);

            length = new_length;

            count += 1;
            if (! (count < wanted))
                break;
        }

        if (! (count < wanted))
            break;
    }

    // restore the previous locale
    setlocale (LC_ALL, save_locale);

    // deallocate the shell expand buffer if needed
    if (res != buf)
        free (res);

    // double null terminated
    if (string) {
        string [length+0] = '\0';
        string [length+1] = '\0';
    }

    return string;
}


const char*
_rw_lookup_table_t::get_canonical_name (const char* name) const
{
    if (!name)
        return 0; // don't search for null string

    const _rw_lookup_entry_t entry = { name, 0 };

    const _rw_lookup_entry_t* found =
        (const _rw_lookup_entry_t*)bsearch (&entry,
                                            entries_,
                                            count_,
                                            sizeof (_rw_lookup_entry_t),
                                            _rw_lookup_comparator);
    if (found)
        return found->canonical;

    return 0;
}

bool
_rw_lookup_table_t::load_from_file (const char* path, const char* name, int upper_or_lower)
{
    if (entries_)
        return false; // should never happen

    // buffer overflow!
    char filename [PATH_MAX];
    strcpy (filename, path);
    strcat (filename, name);

    FILE* file = fopen (filename, "rb");
    if (file) {

        // get the size of the file in bytes
        fseek (file, 0, SEEK_END);
        const size_t table_data_size = ftell (file);
        fseek (file, 0, SEEK_SET);

        char* table_data =
            (char*)malloc (table_data_size + 1);
        
        if (!table_data) {
            fclose (file);
            return false;
        }

        // read the entire file into the data buffer
        const size_t bytes_read =
            fread (table_data, 1, table_data_size, file);
        if (bytes_read != table_data_size) {
            free (table_data);
            fclose (file);
            return false;
        }

        // null terminate
        table_data [bytes_read] = '\0';

        const size_t entry_size = sizeof (_rw_lookup_entry_t);
        
        _rw_lookup_entry_t* entries = 0;
        size_t capacity = 0;
        size_t size     = 0;

        const char* canonical_name = 0;
        
        for (size_t offset = 0; offset < bytes_read; /**/) {

            char* key = table_data + offset;

            const size_t len = strcspn (key, "\r\n");
            key [len] = '\0';

            // skip the newline if it is there
            offset += (len + 1);

            // special handling for line ends and comments
            if (!*key || *key == '\n'
                      || *key == '#')
                continue;

            // make upper or lower case as requested
            if (upper_or_lower < 0) {
                for (char* s = key; *s; ++s)
                    *s = _rw_tolower (*s);
            }
            else if (0 < upper_or_lower) {
                for (char* s = key; *s; ++s)
                    *s = _rw_toupper (*s);
            }

            // if first character of new line is not whitespace, then we have a new
            // canonical name token
            if (!_rw_isspace (*key)) {

                canonical_name = key;

                // increment key past cannonical name
                for (/**/; *key; ++key)
                    if (_rw_isspace (*key))
                        break;
            }

            // kill whitespace
            while (_rw_isspace (*key))
                *key++ = '\0';

            // key points to first non-whitespace after canonical name

            while (*key) {

                // key is first non-whitespace character, which is the
                // next native name we should record
                const char* native_name = key;

                // find first comma character, that is the end of the
                // native name
                while (*key && *key != ',')
                    ++key;

                // if we found a comma, setup next name
                if (*key)
                    *key++ = '\0';

                // kill any whitespace before comma
                for (char* bey = key - 1; _rw_isspace (*bey); --bey)
                    *bey = '\0';

                // kill whitespace after comma
                while (_rw_isspace (*key))
                    *key++ = '\0';

                // ensure we have enough entries
                if (! (size < capacity)) {

                    capacity += 64;

                    _rw_lookup_entry_t* new_entries =
                        (_rw_lookup_entry_t*)malloc (entry_size * capacity);
                    if (!new_entries) {

                        free (entries);

                        free (table_data);

                        fclose (file);

                        return false;
                    }

                    memcpy (new_entries, entries, entry_size * size);

                    free (entries);

                    entries = new_entries;
                }

                // add the new mapping entry
                _rw_lookup_entry_t* const entry = &entries [size];
                entry->native = native_name;
                entry->canonical = canonical_name;

                // increment number of entries
                size += 1;
            }
        }

        fclose (file);

        // sort the entries by native name for efficient searching
        qsort (entries, size, entry_size, _rw_lookup_comparator);

        // setup the table for return
        entries_  = entries;
        count_    = size;
        table_data_ = table_data;
    }
    else {
        rw_error (0, __FILE__, __LINE__,
                  "failed to open the file %s", filename);
    }

    return true;
}


