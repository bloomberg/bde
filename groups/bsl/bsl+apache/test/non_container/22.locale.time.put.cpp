/***************************************************************************
 *
 * 22.time.put.cpp - tests exercising lib.locale.time.put
 *
 * $Id: 22.locale.time.put.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2002-2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#include <bsls_platform.h>

#include <ios>           // for basic_ios
#include <locale>        // for locale, time_put

#include <climits>       // for INT_MAX, INT_MIN, TZNAME_MAX
#include <cstdio>        // for fprintf(), ...
#include <cstdlib>       // for mbstowcs(), wcstombs()
#include <cstring>       // for memset(), memcpy(), strlen(), strcat()
#include <ctime>         // for tm, tzset()
#include <cwchar>        // for wcsftime(), wcslen()
#include <cassert>       // for assert()

#include <driver.h>      // for rw_test(), ...
#include <environ.h>     // for rw_putenv()
#include <file.h>        // for pcs_write(), ...
#include <rw_locale.h>   // for rw_localedef(), ...
#include <rw_printf.h>   // for rw_snprintf()
#include <valcmp.h>      // for rw_strncmp()

#include <rw/_defs.h>    // for TEST_RW_EXTENSIONS

#if _RWSTD_PATH_SEP == '/'
#  define SLASH    "/"
#else
#  define SLASH    "\\"
#endif


// the root of the locale directory (RWSTD_LOCALE_ROOT)
// set in main() instead of here to avoid Solaris 7 putenv() bug (PR #30017)
const char* locale_root;

#if defined (_RWSTD_NO_TZSET) && !defined (_RWSTD_NO_TZSET_IN_LIBC)
#  undef _RWSTD_NO_TZSET
extern "C" _RWSTD_DLLIMPORT void tzset () _LIBC_THROWS();
#endif   // _RWSTD_NO_TZSET && !_RWSTD_NO_TZSET_IN_LIBC

inline void set_TZ (const char* str)
{
    rw_putenv (str);

#ifndef _RWSTD_NO_TZSET
    tzset ();
#endif
}

/**************************************************************************/

const char* narrow (char *dst, const char *src)
{
    if (src == dst || !src || !dst)
        return src;

    std::memcpy (dst, src, std::strlen (src) + 1);
    return dst;
}


const char* widen (char *dst, const char *src)
{
    if (src == dst || !src || !dst)
        return src;

    std::memcpy (dst, src, std::strlen (src) + 1);
    return dst;
}


const char* narrow (char *dst, const wchar_t *src)
{
    static char buf [4096];

    if (!src)
        return 0;

    if (!dst)
        dst = buf;

    RW_ASSERT (std::wcslen (src) < sizeof buf);

    const std::size_t len = std::wcstombs (dst, src, sizeof buf / sizeof *buf);

    if (std::size_t (-1) == len)
        *dst = 0;

    return dst;
}


const wchar_t* widen (wchar_t *dst, const char *src)
{
    static wchar_t buf [4096];

    if (!src)
        return 0;

    if (!dst)
        dst = buf;

    RW_ASSERT (std::strlen (src) < sizeof buf /sizeof *buf);

    const std::size_t len = std::mbstowcs (dst, src, sizeof buf / sizeof *buf);

    if (std::size_t (-1) == len)
        *dst = 0;

    return dst;
}


std::size_t rw_strftime (char *buf, std::size_t bufsize,
                         const char *pat, const std::tm *tmb)
{
    static const std::tm tmp = std::tm ();

    if (!tmb)
        tmb = &tmp;

#ifdef _MSC_VER

    // ms crt aborts if you use out of range values in debug
    if (tmb->tm_hour < 0 || 24 <= tmb->tm_hour)
        return 0;

    if (tmb->tm_min < 0 || 60 <= tmb->tm_min)
        return 0;

    if (tmb->tm_sec < 0 || 60 <= tmb->tm_sec)
        return 0;

    // create a new pattern buffer that is windows strftime compatible...
    char patbuf [512];
    char *fpat = patbuf;

    // assumes tm_year is relative to 1900
    const int year    = (tmb->tm_year + 1900);
    const int century = (tmb->tm_year + 1900) / 100;

    while (*pat) {

        if (*pat == '%') {

            // ensure we are not within 16 chars of the end of the
            // local pattern buffer to avoid overflow
            assert ((fpat + 16) < (patbuf + 512));

            pat += 1; // step past %
            switch (*pat)
            {
            case '\0':
                // fails miserably if the last char is %
                *fpat++ = '%'; *fpat++ = '%';
                break;
            case 'C':
                // %C is replaced by the century number (the year divided by
                // 100 and truncated to an integer) as a decimal number.
                *fpat++ = '0' + (century / 10) % 10;
                *fpat++ = '0' + (century % 10);
                pat += 1;
                break;
            case 'D':
                // %D same as %m/%d/%y.
                *fpat++ = '%'; *fpat++ = 'm'; *fpat++ = '/';
                *fpat++ = '%'; *fpat++ = 'd'; *fpat++ = '/';
                *fpat++ = '%'; *fpat++ = 'y'; pat += 1;
                break;
            case 'e':
                // %e is replaced by the day of the month as a decimal
                // number [1,31]; a single digit is preceded by a space.
                if (tmb->tm_mday < 10)
                    *fpat++ = ' ';
                else
                    *fpat++ = '0' + (tmb->tm_mday / 10) % 10;
                *fpat++ = '0' + (tmb->tm_mday % 10);
                pat += 1;
                break;
            case 'F':
                // %F Equivalent to %Y - %m - %d (the ISO 8601:2000 standard
                // date format). [ tm_year, tm_mon, tm_mday]
                *fpat++ = '%'; *fpat++ = 'Y';
                *fpat++ = ' '; *fpat++ = '-'; *fpat++ = ' ';
                *fpat++ = '%'; *fpat++ = 'm';
                *fpat++ = ' '; *fpat++ = '-'; *fpat++ = ' ';
                *fpat++ = '%'; *fpat++ = 'd'; pat += 1;
                break;
            case 'g':
                // %g Replaced by the last 2 digits of the week-based year
                // as a decimal number [00,99]. [ tm_year, tm_wday, tm_yday]
                return 0;
            case 'G':
                // %G Replaced by the week-based year as a decimal number
                // (for example, 1977). [ tm_year, tm_wday, tm_yday]
                return 0;
            case 'h':
                // %h same as %b.
                *fpat++ = '%'; *fpat++ = 'b'; pat += 1;
                break;
            case 'n':
                // %n is replaced by a newline character.
                *fpat++ = '\n'; pat += 1;
                break;
            case 'r':
                // %r is replaced by the time in a.m. and p.m. notation; in
                // the POSIX locale this is equivalent to %I:%M:%S %p.
                *fpat++ = '%'; *fpat++ = 'I'; *fpat++ = ':';
                *fpat++ = '%'; *fpat++ = 'M'; *fpat++ = ':';
                *fpat++ = '%'; *fpat++ = 'S'; *fpat++ = ' ';
                *fpat++ = '%'; *fpat++ = 'p'; pat += 1;
                break;
            case 'R':
                // %R is replaced by the time in 24 hour notation (%H:%M).
                *fpat++ = '%'; *fpat++ = 'H'; *fpat++ = ':';
                *fpat++ = '%'; *fpat++ = 'M'; pat += 1;
                break;
            case 't':
                // %t is replaced by a tab character.
                *fpat++ = '\t'; pat += 1;
                break;
            case 'T':
                // %T is replaced by the time (%H:%M:%S).
                *fpat++ = '%'; *fpat++ = 'H'; *fpat++ = ':';
                *fpat++ = '%'; *fpat++ = 'M'; *fpat++ = ':';
                *fpat++ = '%'; *fpat++ = 'S'; pat += 1;
                break;
            case 'u':
                // %u is replaced by the weekday as a decimal number [1,7],
                // with 1 representing Monday.
                *fpat++ = '0' + (tmb->tm_wday + 1);
                pat += 1;
                break;
            case 'V':
                // %V is replaced by the week number of the year (Monday as
                // the first day of the week) as a decimal number [01,53].
                // If the week containing 1 January has four or more days
                // in the new year, then it is considered week 1. Otherwise,
                // it is the last week of the previous year, and the next
                // week is week 1.
                return 0;
            default:
                // copy percent and format
                *fpat++ = '%';
                *fpat++ = *pat++;
                break;
            }

            *fpat = 0; // null terminate
        }
        else {
            *fpat++ = *pat++;
        }
    }

    // copy the null
    *fpat = *pat;

    const std::size_t n = std::strftime (buf, bufsize, patbuf, tmb);

#else   // if !defined (_MSC_VER)

    const std::size_t n = std::strftime (buf, bufsize, pat, tmb);

#endif   // _MSC_VER

    RW_ASSERT (n < bufsize);

    return n;
}


std::size_t rw_strftime (wchar_t *wbuf, std::size_t bufsize,
                         const wchar_t *wpat, const std::tm *tmb)
{
    static const std::tm tmp = std::tm ();

#if !defined (_RWSTD_NO_WCSFTIME_WCHAR_T_FMAT) && !defined (_MSC_VER)

    std::size_t n = std::wcsftime (wbuf, bufsize, wpat, tmb ? tmb : &tmp);

#else   // if defined (_RWSTD_NO_WCSFTIME) || defined (_MSC_VER)

    char pat [1024];
    char buf [1024];

    narrow (pat, wpat);
    std::size_t n = rw_strftime (buf, bufsize, pat, tmb ? tmb : &tmp);
    widen (wbuf, buf);

#endif   // _RWSTD_NO_WCSFTIME, _MSC_VER

    RW_ASSERT (n < bufsize);

    return n;
}

/**************************************************************************/


template <class charT>
struct Ios: std::basic_ios<charT>
{
    Ios () {
        this->init (0);
    }
};


template <class charT>
struct Buffer: std::basic_streambuf<charT>
{
    Buffer (charT *beg, charT *end) {
        this->setp (beg, end);
    }
    void pubsetp (charT *beg, charT *end) {
        this->setp (beg, end);
    }
};

/**************************************************************************/

template <class charT>
void do_test (int             lineno,  // line number containing tests
              charT,                   // character type of test
              const char     *tname,   // character type name
              const std::tm  *tmb,     // may be 0 (extension)
              const char     *pat,     // format pattern string
              int             flags,   // ios_base flags
              int             width,   // ios_base width
              char            fill,    // fill character
              const char     *str)     // expected result or strftime() format
{
    Ios<charT> ios;

    const std::time_put<charT> &tp =
        std::use_facet<std::time_put<charT> >(ios.getloc ());

    ios.flags (std::ios_base::fmtflags (flags));
    ios.width (std::streamsize (width));

    charT buf [1024] = { 0 };

    charT result [sizeof buf / sizeof *buf];

    // convert narrow pattern string a (possibly) wide representation
    charT wpatbuf [256];
    const charT* const wpat = widen (wpatbuf, pat);

    // `str' may be 0 if testing uspecified behavior (i.e.,
    // just that the input doesn't cause undefined behavior
    // such as a crash, but the actual result is unspecified)
    if (str && '%' == str [0] && tmb) {

        // if expected result string starts with a '%', use it as a format
        // string to strftime() to obtain the actual result string

        charT wfmtstr [256];
        widen (wfmtstr, str);
        std::size_t n = rw_strftime (result, sizeof result / sizeof *result,
                                     wfmtstr, tmb);

        RW_ASSERT (n < sizeof result);

        result [n] = charT ();

        if (0 == n || !std::memcmp (result, str, n)) {
            // unable to convert pattern using strftime, bail out
            rw_warn (0, 0, __LINE__, "strftime failed");
            return;
        }
    }
    else if (str)
        widen (result, str);


    const char* const patend =
        pat + std::char_traits<char>::length (pat);

    // construct a streambuf-derived object to use for formatting
    Buffer<charT> sb (buf, buf + sizeof buf / sizeof *buf);

    std::ostreambuf_iterator<charT> it (&sb);

    if (patend - pat == 2 && '%' == pat [0]) {

        // format character, no modifier
        *tp.put (it, ios, fill, tmb, char (wpat [1])) = charT ();

        const bool success = 0 == rw_strncmp (buf, result);

        rw_assert (!str || success , __FILE__, lineno,
                   "line %d. time_put<%s>::do_put (%{t}, ..., %#c) "
                   "%{?}with TZ=\"%{$TZ}\" %{;}==>"
                   " %{*Ac}, expected %{*Ac}%{?} (strftime format %#s)%{;}, "
                   "flags = %{If}",
                   __LINE__, tname, tmb, pat [1],
                   'Z' == pat [1] || 'z' == pat [1],
                   int (sizeof *buf), buf, int (sizeof *result), result,
                   str && '%' == str [0], str, flags);
    }
    else if (patend - pat == 3 && '%' == pat [0]) {

        // format character preceded by a format modifier
        *tp.put (it, ios, fill, tmb, char (wpat [2]), char (wpat [1])) =
            charT ();

        const bool success = 0 == rw_strncmp (buf, result);

        rw_assert (!str || success , __FILE__, lineno,
                   "line %d. time_put<%s>::do_put (%{t}, ..., %#c, %#c) ==>"
                   " %{*Ac}, expected %{*Ac}%{?} (strftime format %#s)%{;}, "
                   "flags = %{If}",
                   __LINE__, tname, tmb, pat [1], pat [2],
                   int (sizeof *buf), buf, int (sizeof *result), result,
                   str && '%' == str [0], str, flags);
    }

    sb.pubsetp (buf, buf + sizeof buf / sizeof *buf);

    // format string
    *tp.put (it, ios, fill, tmb, wpat, wpat + (patend - pat)) = charT ();

    const bool success = 0 == rw_strncmp (buf, result);

    rw_assert (!str || success , __FILE__, lineno,
               "line %d. time_put<%s>::do_put (%{t}, ..., %#s) "
               "%{?}with TZ=\"%{$TZ}\" %{;}==>"
               " %{*Ac}, expected %{*Ac}%{?} (strftime format %#s)%{;}, "
               "flags = %{If}",
               __LINE__, tname, tmb, pat,
               std::strstr (pat, "%Z") || std::strstr (pat, "%z"),
               int (sizeof *buf), buf, int (sizeof *result), result,
               str && '%' == str [0], str, flags);
}

/**************************************************************************/

struct time_data
{
    const char*  abday [7];     // %a   abbreviated day name
    const char*  day [7];       // %A   full day name
    const char*  abmon [12];    // %b   abbreviated month name
    const char*  mon [12];      // %B   full month name
    const char*  am_pm [2];     // %p   AM/PM designator
    const char*  d_t_fmt;       // %c   date and time
    const char*  d_fmt;         // %x   date
    const char*  t_fmt;         // %X   time
    const char*  t_fmt_ampm;    // %r   time with the AM/PM designaror
    const char*  era_d_t_fmt;   // %Ec  alternative date and time
    const char*  era_d_fmt;     // %EX  alternative date
    const char*  era_t_fmt;     // %Ex  alternative time
    const char** alt_digits;    //      alternative numeric symbols

    struct era_data {
        int         offset;
        int         start_day;   // [1..31]
        int         start_mon;   // [0..11]
        int         start_year;
        int         end_day;     // [1..31]
        int         end_mon;     // [0..11]
        int         end_year;    // INT_MIN, [0..9999], INT_MAX
        const char *name;
        const char *format;
    };

    const era_data* era;
};


const char* make_LC_TIME (const time_data *td)
{
    static char locnamebuf [1024];

    // assume `td' points to the same data as the first time
    // the function was called and reuse the same database
    if (*locnamebuf)
        return locnamebuf;

    // create a temporary locale definition file
    char srcfname [1024];
    if (rw_snprintf (srcfname, sizeof srcfname, "%s%c%s",
                     locale_root, _RWSTD_PATH_SEP, "LC_TIME.src") < 0)
        return 0;

    std::FILE *fout = std::fopen (srcfname, "w");

    std::fprintf (fout, "LC_TIME\n");

    unsigned i;

    std::fprintf (fout, "abday ");

    for (i = 0; i != sizeof td->abday / sizeof *td->abday; ++i) {
        std::fprintf (fout, "\"");
        pcs_write (fout, td->abday [i]);
        std::fprintf (fout, "\"%c", i < 6 ? ';' : '\n');
    }

    std::fprintf (fout, "day ");

    for (i = 0; i != sizeof td->day / sizeof *td->day; ++i) {
        std::fprintf (fout, "\"");
        pcs_write (fout, td->day [i]);
        std::fprintf (fout, "\"%c", i < 6 ? ';' : '\n');
    }

    std::fprintf (fout, "abmon ");

    for (i = 0; i != sizeof td->abmon / sizeof *td->abmon; ++i) {
        std::fprintf (fout, "\"");
        pcs_write (fout, td->abmon [i]);
        std::fprintf (fout, "\"%c", i < 11 ? ';' : '\n');
    }

    std::fprintf (fout, "mon ");

    for (i = 0; i != sizeof td->mon / sizeof *td->mon; ++i) {
        std::fprintf (fout, "\"");
        pcs_write (fout, td->mon [i]);
        std::fprintf (fout, "\"%c", i < 11 ? ';' : '\n');
    }

    std::fprintf (fout, "am_pm ");

    for (i = 0; i != sizeof td->am_pm / sizeof *td->am_pm; ++i) {
        std::fprintf (fout, "\"");
        pcs_write (fout, td->am_pm [i]);
        std::fprintf (fout, "\"%c", i < 1 ? ';' : '\n');
    }

    // write out d_t_fmt, d_fmt, t_fmt, etc.
    // preface each line with a comment giving
    // the specification in a human-readable format
    std::fprintf (fout, "\n# d_t_fmt \"%s\"\nd_t_fmt \"",
                  td->d_t_fmt);
    pcs_write (fout, td->d_t_fmt);

    std::fprintf (fout, "\"\n\n# d_fmt \"%s\"\nd_fmt \"",
                  td->d_fmt);
    pcs_write (fout, td->d_fmt);

    std::fprintf (fout, "\"\n\n# t_fmt \"%s\"\nt_fmt \"",
                  td->t_fmt);
    pcs_write (fout, td->t_fmt);

    std::fprintf (fout, "\"\n\n# t_fmt_ampm \"%s\"\nt_fmt_ampm \"",
                  td->t_fmt_ampm);
    pcs_write (fout, td->t_fmt_ampm);

    std::fprintf (fout, "\"\n\n# era_d_t_fmt \"%s\"\nera_d_t_fmt \"",
                  td->era_d_t_fmt);
    pcs_write (fout, td->era_d_t_fmt);

    std::fprintf (fout, "\"\n\n# era_d_fmt \"%s\"\nera_d_fmt \"",
                  td->era_d_fmt);
    pcs_write (fout, td->era_d_fmt);

    std::fprintf (fout, "\"\n\n# era_t_fmt \"%s\"\nera_t_fmt \"",
                  td->era_t_fmt);
    pcs_write (fout, td->era_t_fmt);

    std::fprintf (fout, "\"\nalt_digits ");

    for (i = 0; td->alt_digits [i]; ++i) {
        std::fprintf (fout, "\"");
        pcs_write (fout, td->alt_digits [i]);
        std::fprintf (fout, "\"%c", td->alt_digits [i + 1] ? ';' : '\n');
    }

    std::fprintf (fout, "\n\nera ");

    for (i = 0; INT_MIN != td->era [i].offset; ++i) {

        char segment [256];

        std::sprintf (segment, "%c:%d:%04d/%02d/%02d:",
                      td->era [i].offset < 0 ? '-' : '+',
                      td->era [i].offset < 0 ? -td->era [i].offset
                                             : td->era [i].offset,
                      td->era [i].start_year + 1900,
                      td->era [i].start_mon + 1,
                      td->era [i].start_day);

        const char *end_date = 0;

        if (INT_MIN == td->era [i].end_year)
            end_date = "-*";   // beginning of time
        else if (INT_MAX == td->era [i].end_year)
            end_date = "+*";   // end of time

        if (end_date) {
            std::sprintf (segment + std::strlen (segment), "%s:%s:%s",
                          end_date, td->era [i].name, td->era [i].format);
        }
        else {
            std::sprintf (segment + std::strlen (segment),
                          "%04d/%02d/%02d:%s:%s",
                          td->era [i].end_year + 1900,
                          td->era [i].end_mon + 1,
                          td->era [i].end_day,
                          td->era [i].name,
                          td->era [i].format);
        }

        std::fprintf (fout, "\"");
        pcs_write (fout, segment);
        std::fprintf (fout, "\"%c",
                      INT_MIN == td->era [i + 1].offset ? '\n' : ';');
    }

    std::fprintf (fout, "\nEND LC_TIME\n");

    std::fclose (fout);

    // create a temporary character map file
    char cmfname [1024];
    if (rw_snprintf (cmfname, sizeof cmfname, "%s%c%s",
                     locale_root, _RWSTD_PATH_SEP, "LC_TIME.cm") < 0)
        return 0;

    fout = std::fopen (cmfname, "w");
    pcs_write (fout, 0);

    std::fclose (fout);

    const char* const locname =
        rw_localedef ("", srcfname, cmfname, "test-locale");

    // avoid copying into buffer if it's not big enough: this
    // this affect the efficiency of the function but not its
    // behavior
    if (locname && std::strlen (locname) < sizeof locnamebuf)
        std::strcpy (locnamebuf, locname);

    // remove temporary files
    std::remove (cmfname);
    std::remove (srcfname);

    return locname;
}

/**************************************************************************/

const std::tm* mktm (int sec = 0,            // [0,60]
                     int min = 0,            // [0,59]
                     int hour = 0,           // [0,23]
                     int mday = 1,           // [1,31]
                     int mon = 0,            // [0,11]
                     int year = 0,           // Gregorian year - 1900
                     int wday = 0,           // [0,6]; 0 = Sunday
                     int yday = 0,           // [0,365]
                     int isdst = 0,          // < 0, 0, > 0
                     long gmtoff = 0,        // offset from GMT in seconds,
                     const char *zone = 0)   // timezone name
{
    static std::tm tmp = std::tm ();

    if (sec < 0) {
        // get the current local time
        std::time_t t = std::time (0);
        std::tm *tmb = std::localtime (&t);
        return tmb ? tmb : &tmp;
    }
    else if (INT_MAX == sec) {
        // return 0 to exercise extensions
        return 0;
    }

    // use arguments to initialize struct
    tmp.tm_sec   = sec;
    tmp.tm_min   = min;
    tmp.tm_hour  = hour;
    tmp.tm_mday  = mday;
    tmp.tm_mon   = mon;
    tmp.tm_year  = year;
    tmp.tm_wday  = wday;
    tmp.tm_yday  = yday;
    tmp.tm_isdst = isdst;

#if defined (__linux__) && defined (_RWSTD_NO_PURE_C_HEADERS)

    // support for glibc extension:

    // GNU glibc uses gmtoff and zone instead of timezone and
    // tzname when computing/formatting time zone information
    //
    // http://www.gnu.org/manual/glibc-2.2.3/html_node/libc_425.html#SEC434

#  ifndef __USE_BSD

    tmp.__tm_gmtoff = gmtoff;
    tmp.__tm_zone   = zone;

#  else   // if defined (__USE_BSD)

    tmp.tm_gmtoff = gmtoff;
    tmp.tm_zone   = zone;

#  endif   // __USE_BSD

#else

    _RWSTD_UNUSED (gmtoff);
    _RWSTD_UNUSED (zone);

#endif   // __linux__ && _RWSTD_NO_PURE_C_HEADERS

    return &tmp;
}


#define T __LINE__, charT (), tname, mktm
#define TEST do_test


template <class charT>
void test_POSIX (charT, const char *tname)
{
    rw_info (0, 0, 0, "std::time_put<%s>, std::locale (\"C\")", tname);

    // exercise the formatting of struct tm in "C" locale

    // %a: the locale's abbreviated weekday name. [tm_wday]
    rw_info (0, 0, __LINE__, "%%a: the locale's abbreviated weekday name");
    TEST (T (0, 0, 0, 1, 0, 0, 0), "%a", 0, 0, ' ', "%a");
    TEST (T (0, 0, 0, 1, 0, 0, 0), "%a", 0, 0, ' ', "Sun");
    TEST (T (0, 0, 0, 1, 0, 0, 1), "%a", 0, 0, ' ', "Mon");
    TEST (T (0, 0, 0, 1, 0, 0, 2), "%a", 0, 0, ' ', "Tue");
    TEST (T (0, 0, 0, 1, 0, 0, 3), "%a", 0, 0, ' ', "Wed");
    TEST (T (0, 0, 0, 1, 0, 0, 4), "%a", 0, 0, ' ', "Thu");
    TEST (T (0, 0, 0, 1, 0, 0, 5), "%a", 0, 0, ' ', "Fri");
    TEST (T (0, 0, 0, 1, 0, 0, 6), "%a", 0, 0, ' ', "Sat");
    TEST (T (0, 0, 0, 1, 0, 0, 0), "%aday", 0, 0, ' ', "Sunday");
    TEST (T (0, 0, 0, 1, 0, 0, 0), "Sun", 0, 0, ' ', "%a");
    TEST (T (-1 /* now */),        "%a", 0, 0, ' ', "%a");
    TEST (T (-1),                   "%a %a", 0, 0, ' ', "%a %a");
    TEST (T (0, 0, 0, 1, 0, 0, -1), "%a", 0, 0, ' ', 0 /* unspecified */);

    // %A: the locale's full weekday name. [tm_wday]
    rw_info (0, 0, __LINE__, "%%A: the locale's full weekday name");
    TEST (T (0, 0, 0, 1, 0, 0, 0), "%A", 0, 0, ' ', "%A");
    TEST (T (0, 0, 0, 1, 0, 0, 0), "%A", 0, 0, ' ', "Sunday");
    TEST (T (0, 0, 0, 1, 0, 0, 1), "%A", 0, 0, ' ', "Monday");
    TEST (T (0, 0, 0, 1, 0, 0, 2), "%A", 0, 0, ' ', "Tuesday");
    TEST (T (0, 0, 0, 1, 0, 0, 3), "%A", 0, 0, ' ', "Wednesday");
    TEST (T (0, 0, 0, 1, 0, 0, 4), "%A", 0, 0, ' ', "Thursday");
    TEST (T (0, 0, 0, 1, 0, 0, 5), "%A", 0, 0, ' ', "Friday");
    TEST (T (0, 0, 0, 1, 0, 0, 6), "%A", 0, 0, ' ', "Saturday");
    TEST (T (0, 0, 0, 1, 0, 0, 0), "Sunday", 0, 0, ' ', "%A");
    TEST (T (-1 /* now */),        "%A", 0, 0, ' ', "%A");
    TEST (T (-1),                  "%A %a", 0, 0, ' ', "%A %a");
    TEST (T (-1),                  "%a %A", 0, 0, ' ', "%a %A");
    TEST (T (0, 0, 0, 1, 0, 0, -1), "%A", 0, 0, ' ', 0 /* unspecified */);

    // %b: the locale's abbreviated month name. [tm_mon]
    rw_info (0, 0, __LINE__, "%%b: the locale's abbreviated month name");
    TEST (T (0, 0, 0, 1,  0), "%b", 0, 0, ' ', "%b");
    TEST (T (0, 0, 0, 1,  0), "%b", 0, 0, ' ', "Jan");
    TEST (T (0, 0, 0, 1,  1), "%b", 0, 0, ' ', "Feb");
    TEST (T (0, 0, 0, 1,  2), "%b", 0, 0, ' ', "Mar");
    TEST (T (0, 0, 0, 1,  3), "%b", 0, 0, ' ', "Apr");
    TEST (T (0, 0, 0, 1,  4), "%b", 0, 0, ' ', "May");
    TEST (T (0, 0, 0, 1,  5), "%b", 0, 0, ' ', "Jun");
    TEST (T (0, 0, 0, 1,  6), "%b", 0, 0, ' ', "Jul");
    TEST (T (0, 0, 0, 1,  7), "%b", 0, 0, ' ', "Aug");
    TEST (T (0, 0, 0, 1,  8), "%b", 0, 0, ' ', "Sep");
    TEST (T (0, 0, 0, 1,  9), "%b", 0, 0, ' ', "Oct");
    TEST (T (0, 0, 0, 1, 10), "%b", 0, 0, ' ', "Nov");
    TEST (T (0, 0, 0, 1, 11), "%b", 0, 0, ' ', "Dec");
    TEST (T (0, 0, 0, 1,  0), " %buary", 0, 0, ' ', " January");
    TEST (T (0, 0, 0, 1,  0), "Jan", 0, 0, ' ', "%b");
    TEST (T (-1 /* now */),   "%b", 0, 0, ' ', "%b");
    TEST (T (-1),             "%b %b", 0, 0, ' ', "%b %b");
    TEST (T (0, 0, 0, 1, -1), "%b", 0, 0, ' ', 0 /* unspecified */);

    // %B: the locale's full month name. [tm_mon]
    rw_info (0, 0, __LINE__, "%%B: the locale's full month name");
    TEST (T (0, 0, 0, 1,  0), "%B", 0, 0, ' ', "%B");
    TEST (T (0, 0, 0, 1,  0), "%B", 0, 0, ' ', "January");
    TEST (T (0, 0, 0, 1,  1), "%B", 0, 0, ' ', "February");
    TEST (T (0, 0, 0, 1,  2), "%B", 0, 0, ' ', "March");
    TEST (T (0, 0, 0, 1,  3), "%B", 0, 0, ' ', "April");
    TEST (T (0, 0, 0, 1,  4), "%B", 0, 0, ' ', "May");
    TEST (T (0, 0, 0, 1,  5), "%B", 0, 0, ' ', "June");
    TEST (T (0, 0, 0, 1,  6), "%B", 0, 0, ' ', "July");
    TEST (T (0, 0, 0, 1,  7), "%B", 0, 0, ' ', "August");
    TEST (T (0, 0, 0, 1,  8), "%B", 0, 0, ' ', "September");
    TEST (T (0, 0, 0, 1,  9), "%B", 0, 0, ' ', "October");
    TEST (T (0, 0, 0, 1, 10), "%B", 0, 0, ' ', "November");
    TEST (T (0, 0, 0, 1, 11), "%B", 0, 0, ' ', "December");
    TEST (T (),               "January", 0, 0, ' ', "%B");
    TEST (T (-1 /* now */),   "%B", 0, 0, ' ', "%B");
    TEST (T (-1),             "%B %b", 0, 0, ' ', "%B %b");
    TEST (T (-1),             "%A %B", 0, 0, ' ', "%A %B");
    TEST (T (-1),             "%A %B ", 0, 0, ' ', "%A %B ");
    TEST (T (0, 0, 0, 1, -1), "%B", 0, 0, ' ', 0 /* unspecified */);

    // %c: the locale's appropriate date and time representation.
    //     [all specified in 7.23.1 of C99]; equivalent format
    //     specified by POSIX
    rw_info (0, 0, __LINE__,
             "%%c: the locale's appropriate date and time representation");

    // avoid comparing against the result of strftime() which
    // may not conform to the latest POSIX (IEEE Std 1003.1-2001)
    // (e.g., SunOS 5.7)
    TEST (T (),      "%c",   0, 0, ' ', "%a %b %e %T %Y");
#if TEST_RW_EXTENSIONS
    TEST (T (),      "%c %", 0, 0, ' ', "%a %b %e %T %Y %");
#endif
    TEST (T (),      "%c",   0, 0, ' ', "%a %b %e %H:%M:%S %Y");
    TEST (T (),      "%c",   0, 0, ' ', "Sun Jan  1 00:00:00 1900");
    TEST (T (-1),    "%c",   0, 0, ' ', "%a %b %e %T %Y");
    TEST (T (0, -1), "%c",   0, 0, ' ', 0 /* unspecified */);

    TEST (T ( 1,  2,  3,  4,  5,   6,  0), "%c", 0, 0, ' ',
          "Sun Jun  4 03:02:01 1906");   // incorrect weekday
    TEST (T ( 2,  3,  4,  5,  6,   7,  0), "%c", 0, 0, ' ',
          "Sun Jul  5 04:03:02 1907");   // all fields correct
    TEST (T (25,  7, 13, 30,  0, 103,  4), "%c", 0, 0, ' ',
          "Thu Jan 30 13:07:25 2003");   // all fields correct

    // extension: passing (std::tm*)0 to std::time_put<>::put()
    // and 'c' as the format specifier produces the formatting
    // pattern corresponding to the locale date and date format

#if TEST_RW_EXTENSIONS
    TEST (T (INT_MAX), "%c", 0, 0, ' ', "%a %b %e %H:%M:%S %Y");
#endif

    // %C: the year divided by 100 and truncated to an integer,
    //     as a decimal number (00-99). [tm_year]
    rw_info (0, 0, __LINE__,
             "%%C: the year divided by 100 and truncated to an integer");

    // avoid comparing against the result of strftime() which
    // may not conform to the latest POSIX (IEEE Std 1003.1-2001)
    // (e.g., SunOS 5.7)
    TEST (T (0, 0, 0, 1, 0,     0), "19", 0, 0, ' ', "19");
    TEST (T (0, 0, 0, 1, 0,     1), "%C", 0, 0, ' ', "19");
    TEST (T (0, 0, 0, 1, 0,    -1), "%C", 0, 0, ' ', "18");
    TEST (T (0, 0, 0, 1, 0,    99), "%C", 0, 0, ' ', "19");
    TEST (T (0, 0, 0, 1, 0,   -99), "%C", 0, 0, ' ', "18");
    TEST (T (0, 0, 0, 1, 0,   100), "%C", 0, 0, ' ', "20");
    TEST (T (0, 0, 0, 1, 0,  -100), "%C", 0, 0, ' ', "18");
    TEST (T (0, 0, 0, 1, 0,   103), "%C", 0, 0, ' ', "20");
    TEST (T (0, 0, 0, 1, 0,   199), "%C", 0, 0, ' ', "20");
    TEST (T (0, 0, 0, 1, 0,   200), "%C", 0, 0, ' ', "21");
    TEST (T (0, 0, 0, 1, 0,  1000), "%C", 0, 0, ' ', "29");
    TEST (T (0, 0, 0, 1, 0, -1000), "%C", 0, 0, ' ', "09");
    TEST (T (0, 0, 0, 1, 0,  8100), "%C", 0, 0, ' ', "00");
    TEST (T (0, 0, 0, 1, 0, -1900), "%C", 0, 0, ' ', "00");

    // %EC: the name of the base year (period) in the locale's
    //      alternative representation.
    rw_info (0, 0, __LINE__,
             "%%EC: the name of the period in alternative representation");
    TEST (T (0, 0, 0, 1, 0, -1899), "%EC", 0, 0, ' ', "00");
    TEST (T (0, 0, 0, 1, 0,  1234), "%EC", 0, 0, ' ', "31");
    TEST (T (0, 0, 0, 1, 0,  8099), "%EC", 0, 0, ' ', "99");


    // %d: the day of the month as a decimal number (01-31). [tm_mday]
    rw_info (0, 0, __LINE__, "%%d: the day of the month as a decimal number");
    TEST (T (), "%d", 0, 0, ' ', "%d");
    TEST (T (), "%d", 0, 0, ' ', "01");
    TEST (T (), "01", 0, 0, ' ', "%d");
    TEST (T (-1), "%d", 0, 0, ' ', "%d");

    TEST (T (0, 0, 0,  1), "%d", 0, 0, ' ', "%d");
    TEST (T (0, 0, 0,  2), "%d", 0, 0, ' ', "%d");
    TEST (T (0, 0, 0, 10), "%d", 0, 0, ' ', "%d");
    TEST (T (0, 0, 0, 11), "%d", 0, 0, ' ', "%d");
    TEST (T (0, 0, 0, 20), "%d", 0, 0, ' ', "%d");
    TEST (T (0, 0, 0, 21), "%d", 0, 0, ' ', "%d");
    TEST (T (0, 0, 0, 30), "%d", 0, 0, ' ', "%d");
    TEST (T (0, 0, 0, 31), "%d", 0, 0, ' ', "%d");
    TEST (T (0, 0, 0, -1), "%d", 0, 0, ' ', 0 /* unspecified */);

    // %Od: the day of the month, using the locale's alternative numeric
    //      symbols (filled as needed with leading zeros, or with leading
    //      spaces if there is no alternative symbol for zero).
    rw_info (0, 0, __LINE__,
             "%%Od: the day of the month, using alternative digits");
    TEST (T (0, 0, 0,  1), "%Od", 0, 0, ' ', "01");
    TEST (T (0, 0, 0,  9), "%Od", 0, 0, ' ', "09");
    TEST (T (0, 0, 0, 10), "%Od", 0, 0, ' ', "10");
    TEST (T (0, 0, 0, 13), "%Od", 0, 0, ' ', "13");
    TEST (T (0, 0, 0, 22), "%Od", 0, 0, ' ', "22");
    TEST (T (0, 0, 0, 31), "%Od", 0, 0, ' ', "31");
    TEST (T (0, 0, 0, -1), "%Od", 0, 0, ' ', 0 /* unspecified */);

    // %D: equivalent to "%m/%d/%y". [tm_mon, tm_mday, tm_year]
    rw_info (0, 0, __LINE__, "%%D: equivalent to \"%%m/%%d/%%y\"");
    TEST (T (), "%D",       0, 0, ' ', "%D");
    TEST (T (), "%D",       0, 0, ' ', "%m/%d/%y");
    TEST (T (), "%D",       0, 0, ' ', "01/01/00");
    TEST (T (), "01/01/00", 0, 0, ' ', "%D");
    TEST (T (-1), "%D",     0, 0, ' ', "%D");
    TEST (T (0, 0, 0, -1), "%D",     0, 0, ' ', 0 /* unspecified */);

    // %e: the day of the month as a decimal number (1-31);
    //     a single digit is preceded by a space. [tm_mday]
    rw_info (0, 0, __LINE__, "%%e: the day of the month as a decimal number");
    TEST (T (), "%e", 0, 0, ' ', "%e");
    TEST (T (), "%e", 0, 0, ' ', " 1");
    TEST (T (), " 1", 0, 0, ' ', "%e");
    TEST (T (-1), "%e", 0, 0, ' ', "%e");

    TEST (T (0, 0, 0,  1), "%e", 0, 0, ' ', "%e");
    TEST (T (0, 0, 0,  2), "%e", 0, 0, ' ', "%e");
    TEST (T (0, 0, 0, 10), "%e", 0, 0, ' ', "%e");
    TEST (T (0, 0, 0, 11), "%e", 0, 0, ' ', "%e");
    TEST (T (0, 0, 0, 20), "%e", 0, 0, ' ', "%e");
    TEST (T (0, 0, 0, 21), "%e", 0, 0, ' ', "%e");
    TEST (T (0, 0, 0, 30), "%e", 0, 0, ' ', "%e");
    TEST (T (0, 0, 0, 31), "%e", 0, 0, ' ', "%e");
    TEST (T (0, 0, 0, -1), "%e", 0, 0, ' ', 0 /* unspecified */);

    // %Oe: is replaced by the day of the month, using the locale's
    //      alternative numeric symbols (filled as needed with leading
    //      spaces).
    rw_info (0, 0, __LINE__,
             "%%Oe: the day of the month using alternative digits");
    TEST (T (0, 0, 0,  1), "%Oe", 0, 0, ' ', " 1");
    TEST (T (0, 0, 0, 29), "%Oe", 0, 0, ' ', "29");
    TEST (T (0, 0, 0, 30), "%Oe", 0, 0, ' ', "30");
    TEST (T (0, 0, 0, 31), "%Oe", 0, 0, ' ', "31");
    TEST (T (0, 0, 0, -1), "%Oe", 0, 0, ' ', 0 /* unspecified */);

    // %F: equivalent to "%Y-%m-%d" (the ISO 8601 date format).
    //     [tm_year, tm_mon, tm_mday]
    rw_info (0, 0, __LINE__, "%%F: equivalent to \"%%Y-%%m-%%d\"");
    TEST (T (),            "%F", 0, 0, ' ', "%Y-%m-%d");
    TEST (T (),            "%F", 0, 0, ' ', "1900-01-01");
    TEST (T (0, 0, 0, -1), "%F", 0, 0, ' ', 0 /* unspecified */);

    // %g: the last 2 digits of the week-based year as a decimal number
    //     (00-99). [tm_year, tm_wday, tm_yday]
    rw_info (0, 0, __LINE__, "%%g: the last 2 digits of the week-based year");
    TEST (T (0, 0, 0, 1, 0,    0, 0,   0), "%g", 0, 0, ' ', "99");
    TEST (T (0, 0, 0, 1, 0,  102, 2, 120), "%g", 0, 0, ' ', "02");
    TEST (T (0, 0, 0, 1, 0, 1901, 4,   0), "%g", 0, 0, ' ', "01");
    TEST (T (0, 0, 0, 1, 0, 1902, 3,   5), "%g", 0, 0, ' ', "02");
    TEST (T (0, 0, 0, 1, 0,    0, 0,  -1), "%g", 0, 0, ' ', 0 /* unspec. */);

    // %G: the week-based year as a decimal number (e.g., 1997).
    //     [tm_year, tm_wday, tm_yday]
    rw_info (0, 0, __LINE__, "%%G: the week-based year");
    TEST (T (0, 0, 0, 1, 0,     0, 0,   0), "%G", 0, 0, ' ', "%G");
    TEST (T (0, 0, 0, 1, 0, -1899, 0,   0), "%G", 0, 0, ' ', "0");
    TEST (T (0, 0, 0, 1, 0,  -100, 0,   0), "%G", 0, 0, ' ', "1799");
    TEST (T (0, 0, 0, 1, 0,     0, 0,   0), "%G", 0, 0, ' ', "1899");
    TEST (T (0, 0, 0, 1, 0,   102, 2, 120), "%G", 0, 0, ' ', "2002");
    TEST (T (0, 0, 0, 1, 0,  2668, 5,   0), "%G", 0, 0, ' ', "4567");
    TEST (T (0, 0, 0, 1, 0,  2668, 0,   3), "%G", 0, 0, ' ', "4568");
    TEST (T (-1),                           "%G", 0, 0, ' ', "%G");
    TEST (T (0, 0, 0, 1, 0,      0, 0, -1), "%G", 0, 0, ' ', 0 /* unspec. */);

    // %h: equivalent to "%b". [tm_mon]
    rw_info (0, 0, __LINE__, "%%h: equivalent to \"%%b\"");

    // %H: the hour (24-hour clock) as a decimal number (00-23). [tm_hour]
    rw_info (0, 0, __LINE__, "%%H: the hour (24-hour clock)");
    TEST (T (), "%H", 0, 0, ' ', "%H");
    TEST (T (), "%H", 0, 0, ' ', "00");
    TEST (T (), "00", 0, 0, ' ', "%H");
    TEST (T (-1), "%H", 0, 0, ' ', "%H");

    TEST (T (0, 0,  1), "%H", 0, 0, ' ', "%H");
    TEST (T (0, 0,  2), "%H", 0, 0, ' ', "%H");
    TEST (T (0, 0, 10), "%H", 0, 0, ' ', "%H");
    TEST (T (0, 0, 11), "%H", 0, 0, ' ', "%H");
    TEST (T (0, 0, 20), "%H", 0, 0, ' ', "%H");
    TEST (T (0, 0, 21), "%H", 0, 0, ' ', "%H");
    TEST (T (0, 0, 22), "%H", 0, 0, ' ', "%H");
    TEST (T (0, 0, 23), "%H", 0, 0, ' ', "%H");
    TEST (T (0, 0, -1), "%H", 0, 0, ' ', 0 /* unspecified */);

    // make sure output is unaffected by members other than tm_hour
    TEST (T (0,    99,  9), "%H", 0, 0, ' ', "09");
    TEST (T (3600, 60, 18), "%H", 0, 0, ' ', "18");
    TEST (T (7200, 60, 19), "%H", 0, 0, ' ', "19");

    // %OH: the hour (24-hour clock), using the locale's alternative
    //      numeric symbols.
    rw_info (0, 0, __LINE__,
             "%%OH: the hour (24-hour clock), using alternative digits");
    TEST (T (0, 0,  0), "%OH", 0, 0, ' ', "00");
    TEST (T (0, 0,  3), "%OH", 0, 0, ' ', "03");
    TEST (T (0, 0, 12), "%OH", 0, 0, ' ', "12");
    TEST (T (0, 0, 19), "%OH", 0, 0, ' ', "19");
    TEST (T (0, 0, 18), "%OH", 0, 0, ' ', "18");
    TEST (T (0, 0, 17), "%OH", 0, 0, ' ', "17");
    TEST (T (0, 0, -7), "%OH", 0, 0, ' ', 0 /* unspecified */);

    // %I: the hour (12-hour clock) as a decimal number (01-12). [tm_hour]
    rw_info (0, 0, __LINE__, "%%I: the hour (12-hour clock)");
    TEST (T (0, 0,  0), "%I", 0, 0, ' ', "%I");
    TEST (T (0, 0,  0), "%I", 0, 0, ' ', "%I");
    TEST (T (0, 0,  1), "%I", 0, 0, ' ', "%I");
    TEST (T (0, 0,  1), "%I", 0, 0, ' ', "01");
    TEST (T (0, 0,  2), "%I", 0, 0, ' ', "02");
    TEST (T (0, 0,  3), "%I", 0, 0, ' ', "03");
    TEST (T (0, 0, 11), "%I", 0, 0, ' ', "11");
    TEST (T (0, 0, 12), "%I", 0, 0, ' ', "%I");
    TEST (T (0, 0, 12), "%I", 0, 0, ' ', "12");
    TEST (T (0, 0, 13), "%I", 0, 0, ' ', "%I");
    TEST (T (0, 0, 13), "%I", 0, 0, ' ', "01");
    TEST (T (0, 0, 21), "%I", 0, 0, ' ', "09");
    TEST (T (0, 0, 22), "%I", 0, 0, ' ', "10");
    TEST (T (0, 0, 23), "%I", 0, 0, ' ', "11");
    TEST (T (0, 0, 24), "%I", 0, 0, ' ', "%I");
    TEST (T (0, 0,  0), "12", 0, 0, ' ', "%I");
    TEST (T (-1),       "%I", 0, 0, ' ', "%I");
    TEST (T (0, 0, -2), "%I", 0, 0, ' ', 0 /* unspecified */);

    // %OI: the hour (12-hour clock), using the locale's alternative
    //      numeric symbols.
    rw_info (0, 0, __LINE__,
             "%%OI: the hour (12-hour clock) using alternative digits");
    TEST (T (0, 0, 24), "%OI", 0, 0, ' ', "12");
    TEST (T (0, 0, 16), "%OI", 0, 0, ' ', "04");
    TEST (T (0, 0, 15), "%OI", 0, 0, ' ', "03");
    TEST (T (0, 0, 14), "%OI", 0, 0, ' ', "02");
    TEST (T (0, 0, 12), "%OI", 0, 0, ' ', "12");
    TEST (T (0, 0, -3), "%OI", 0, 0, ' ', 0 /* unspecified */);

    // %j: the day of the year as a decimal number (001-366). [tm_yday]
    rw_info (0, 0, __LINE__, "%%j: the day of the year");
    TEST (T (0, 0, 0, 0, 0, 0, 0,   0), "%j", 0, 0, ' ', "%j");
    TEST (T (0, 0, 0, 0, 0, 0, 0,   0), "%j", 0, 0, ' ', "001");
    TEST (T (0, 0, 0, 0, 0, 0, 0,   1), "%j", 0, 0, ' ', "002");
    TEST (T (0, 0, 0, 0, 0, 0, 0,  12), "%j", 0, 0, ' ', "013");
    TEST (T (0, 0, 0, 0, 0, 0, 0, 123), "%j", 0, 0, ' ', "124");
    TEST (T (0, 0, 0, 0, 0, 0, 0, 365), "%j", 0, 0, ' ', "366");
    TEST (T (0, 0, 0, 0, 0, 0, 0,   0), "001", 0, 0, ' ', "%j");
    TEST (T (-1),                       "%j", 0, 0, ' ', "%j");
    TEST (T (0, 0, 0, 0, 0, 0, 0,  -4), "%j", 0, 0, ' ', 0 /* unspecified */);

    // %k: the hour (24-hour clock) with single digits preceded by a blank
    rw_info (0, 0, __LINE__,
             "%%k: the hour (24-hour clock) with single digits "
             "preceded by a blank [AIX, Linux, Solaris extension]");
    TEST (T (0, 0,  0), "%k", 0, 0, ' ', " 0");
    TEST (T (0, 0,  1), "%k", 0, 0, ' ', " 1");
    TEST (T (0, 0,  9), "%k", 0, 0, ' ', " 9");
    TEST (T (0, 0, 11), "%k", 0, 0, ' ', "11");
    TEST (T (0, 0, 12), "%k", 0, 0, ' ', "12");
    TEST (T (0, 0, 23), "%k", 0, 0, ' ', "23");

#ifdef _RWSTD_OS_SUNOS
    // on Solaris, in addition to the results hardcoded above,
    // compare the output with that of strftime() and wcsftime()
    TEST (T (0, 0,  0), "%k", 0, 0, ' ', "%k");
    TEST (T (0, 0,  2), "%k", 0, 0, ' ', "%k");
    TEST (T (0, 0,  9), "%k", 0, 0, ' ', "%k");
    TEST (T (0, 0, 10), "%k", 0, 0, ' ', "%k");
    TEST (T (0, 0, 11), "%k", 0, 0, ' ', "%k");
    TEST (T (0, 0, 12), "%k", 0, 0, ' ', "%k");
    TEST (T (0, 0, 13), "%k", 0, 0, ' ', "%k");
    TEST (T (0, 0, 23), "%k", 0, 0, ' ', "%k");
#endif   // SunOS

    // %l: the hour (12-hour clock) with single digits preceded by a blank
    rw_info (0, 0, __LINE__,
             "%%l: the hour (12-hour clock) with single digits "
             "preceded by a blank [Linux, Solaris extension]");
    TEST (T (0, 0,  0), "%l", 0, 0, ' ', "12");
    TEST (T (0, 0,  1), "%l", 0, 0, ' ', " 1");
    TEST (T (0, 0,  9), "%l", 0, 0, ' ', " 9");
    TEST (T (0, 0, 11), "%l", 0, 0, ' ', "11");
    TEST (T (0, 0, 12), "%l", 0, 0, ' ', "12");
    TEST (T (0, 0, 13), "%l", 0, 0, ' ', " 1");
    TEST (T (0, 0, 21), "%l", 0, 0, ' ', " 9");
    TEST (T (0, 0, 23), "%l", 0, 0, ' ', "11");

#ifdef _RWSTD_OS_SUNOS
    // on Solaris, in addition to the results hardcoded above,
    // compare the output with that of strftime() and wcsftime()
    TEST (T (0, 0,  0), "%l", 0, 0, ' ', "%l");
    TEST (T (0, 0,  2), "%l", 0, 0, ' ', "%l");
    TEST (T (0, 0,  8), "%l", 0, 0, ' ', "%l");
    TEST (T (0, 0, 11), "%l", 0, 0, ' ', "%l");
    TEST (T (0, 0, 12), "%l", 0, 0, ' ', "%l");
    TEST (T (0, 0, 13), "%l", 0, 0, ' ', "%l");
    TEST (T (0, 0, 20), "%l", 0, 0, ' ', "%l");
    TEST (T (0, 0, 23), "%l", 0, 0, ' ', "%l");
#endif   // SunOS

    // %m: the month as a decimal number (01-12). [tm_mon]
    rw_info (0, 0, __LINE__, "%%m: the month");
    TEST (T (), "%m", 0, 0, ' ', "%m");
    TEST (T (), "%m", 0, 0, ' ', "01");
    TEST (T (), "01", 0, 0, ' ', "%m");
    TEST (T (-1), "%m", 0, 0, ' ', "%m");

    // %Om: the month, using the locale's alternative numeric symbols.
    rw_info (0, 0, __LINE__, "%%Om: the month, using alternative digits");
    TEST (T (0, 0, 0, 0,  9), "%Om", 0, 0, ' ', "10");
    TEST (T (0, 0, 0, 0, 10), "%Om", 0, 0, ' ', "11");
    TEST (T (0, 0, 0, 0, 11), "%Om", 0, 0, ' ', "12");
    TEST (T (0, 0, 0, 0, -5), "%Om", 0, 0, ' ', 0 /* unspecified */);

    // %M: the minute as a decimal number (00-59). [tm_min]
    rw_info (0, 0, __LINE__, "%%M: the minute");
    TEST (T (0,  0), "%M", 0, 0, ' ', "%M");
    TEST (T (0,  0), "%M", 0, 0, ' ', "00");
    TEST (T (0,  1), "%M", 0, 0, ' ', "01");
    TEST (T (0,  9), "%M", 0, 0, ' ', "09");
    TEST (T (0, 19), "%M", 0, 0, ' ', "19");
    TEST (T (0, 59), "%M", 0, 0, ' ', "59");
    TEST (T (0,  0), "00", 0, 0, ' ', "%M");
    TEST (T (-1),    "%M", 0, 0, ' ', "%M");
    TEST (T (0, -6), "00", 0, 0, ' ', 0 /* unspecified */);

    // %OM: the minutes, using the locale's alternative numeric symbols.
    rw_info (0, 0, __LINE__, "%%OM: the minutes, using  alternative digits");
    TEST (T (0,  0), "%OM", 0, 0, ' ', "00");
    TEST (T (0,  7), "%OM", 0, 0, ' ', "07");
    TEST (T (0, 11), "%OM", 0, 0, ' ', "11");
    TEST (T (0, 22), "%OM", 0, 0, ' ', "22");
    TEST (T (0, 33), "%OM", 0, 0, ' ', "33");
    TEST (T (0, 44), "%OM", 0, 0, ' ', "44");
    TEST (T (0, -7), "%OM", 0, 0, ' ', 0 /* unspecified */);

    // %n: a new-line character.
    rw_info (0, 0, __LINE__, "%%n: a new-line character");
    TEST (T (), "%n",    0, 0, ' ', "%n");
    TEST (T (), "%n",    0, 0, ' ', "\n");
    TEST (T (), "%n%n",  0, 0, ' ', "\n\n");
    TEST (T (), "\n",    0, 0, ' ', "%n");
    TEST (T (), "%n \n", 0, 0, ' ', "\n \n");

    // %p: the locale's equivalent of the AM/PM designations
    //     associated with a 12-hour clock. [tm_hour]
    rw_info (0, 0, __LINE__,
             "%%p: the locale's equivalent of the AM/PM designations");
    TEST (T ( 0,  0,  0), "%p", 0, 0, ' ', "%p");
    TEST (T ( 0,  0,  1), "%p", 0, 0, ' ', "AM");
    TEST (T ( 0,  0,  2), "%p", 0, 0, ' ', "AM");
    TEST (T ( 0,  0, 11), "%p", 0, 0, ' ', "AM");
    TEST (T ( 0,  0, 12), "%p", 0, 0, ' ', "%p");
    TEST (T ( 0,  0, 12), "%p", 0, 0, ' ', "PM");
    TEST (T ( 0,  0, 13), "%p", 0, 0, ' ', "%p");
    TEST (T ( 0,  0, 23), "%p", 0, 0, ' ', "%p");
    TEST (T ( 0,  0, -8), "%p", 0, 0, ' ', 0 /* unspecified */);

    // %r: the locale's 12-hour clock time. [tm_hour, tm_min, tm_sec]
    rw_info (0, 0, __LINE__, "%%r: the locale's 12-hour clock time");
    TEST (T ( 0,  0,  0), "%r",       0, 0, ' ', "%r");
    TEST (T ( 0,  0,  0), "%r",       0, 0, ' ', "%I:%M:%S %p");
    TEST (T ( 0,  0,  0), "%r",       0, 0, ' ', "12:00:00 AM");
    TEST (T ( 1, 12,  1), "%r",       0, 0, ' ', "01:12:01 AM");
    TEST (T ( 3, 23,  2), "%r",       0, 0, ' ', "02:23:03 AM");
    TEST (T ( 5, 34, 11), "%r",       0, 0, ' ', "11:34:05 AM");
    TEST (T ( 7, 45, 12), "%r",       0, 0, ' ', "12:45:07 PM");
    TEST (T ( 9, 56, 13), "%r",       0, 0, ' ', "01:56:09 PM");
    TEST (T (59, 59, 23), "%r",       0, 0, ' ', "11:59:59 PM");
    TEST (T ( 0,  0,  0), "12:00:00 AM", 0, 0, ' ', "%r");
    TEST (T (-1),         "%r",       0, 0, ' ', "%r");
    TEST (T ( 0,  0, -9), "%r",       0, 0, ' ', 0 /* unspecified */);

    // %R: equivalent to "%H:%M". [tm_hour, tm_min]
    rw_info (0, 0, __LINE__, "%%R: equivalent to \"%%H:%%M\"");
    TEST (T (), "%R", 0, 0, ' ', "%R");
    TEST (T (), "%R", 0, 0, ' ', "%H:%M");
    TEST (T (), "%R", 0, 0, ' ', "00:00");
    TEST (T (), "00:00", 0, 0, ' ', "%R");
    TEST (T (-1), "%R", 0, 0, ' ', "%R");

    // %S: the second as a decimal number (00-60). [tm_sec]
    rw_info (0, 0, __LINE__, "%%S: the seconds");
    TEST (T (),   "%S", 0, 0, ' ', "%S");
    TEST (T (),   "%S", 0, 0, ' ', "00");
    TEST (T (),   "00", 0, 0, ' ', "%S");
    TEST (T ( 1), "%S", 0, 0, ' ', "%S");
    TEST (T ( 2), "%S", 0, 0, ' ', "02");
    TEST (T (11), "%S", 0, 0, ' ', "%S");
    TEST (T (21), "%S", 0, 0, ' ', "%S");
    TEST (T (39), "%S", 0, 0, ' ', "%S");
    TEST (T (59), "%S", 0, 0, ' ', "%S");
    TEST (T (60), "%S", 0, 0, ' ', "%S");
    TEST (T (60), "%S", 0, 0, ' ', "60");
    TEST (T (-1), "%S", 0, 0, ' ', "%S");

    // %OS: the seconds, using the locale's alternative numeric symbols.
    rw_info (0, 0, __LINE__, "%%OS: the seconds, using alternate digits");
    TEST (T ( 6), "%OS", 0, 0, ' ', "06");
    TEST (T (17), "%OS", 0, 0, ' ', "17");
    TEST (T (28), "%OS", 0, 0, ' ', "28");
    TEST (T (39), "%OS", 0, 0, ' ', "39");

    // %t: a horizontal-tab character.
    rw_info (0, 0, __LINE__, "%%t: a horizontal-tab character");
    TEST (T (), "%t", 0, 0, ' ', "%t");
    TEST (T (), "%t", 0, 0, ' ', "\t");
    TEST (T (), "\t", 0, 0, ' ', "%t");
    TEST (T (), "%t\t%t", 0, 0, ' ', "\t\t\t");

    // %T: equivalent to "%H:%M:%S" (the ISO 8601 time format).
    //     [tm_hour, tm_min, tm_sec]
    rw_info (0, 0, __LINE__, "%%T: equivalent to \"%%H:%%M:%%S\"");
    TEST (T (), "%T",       0, 0, ' ', "%T");
    TEST (T (), "%T",       0, 0, ' ', "%H:%M:%S");
    TEST (T (), "%T",       0, 0, ' ', "00:00:00");
    TEST (T (), "00:00:00", 0, 0, ' ', "%T");
    TEST (T (-1), "%T",     0, 0, ' ', "%T");

    // %u: the ISO 8601 weekday as a decimal number (1-7),
    //     where Monday is 1. [tm_wday]
    rw_info (0, 0, __LINE__, "%%u: the ISO 8601 weekday");
    TEST (T (0, 0, 0, 1, 0, 0, 1), "%u", 0, 0, ' ', "1");
    TEST (T (0, 0, 0, 1, 0, 0, 2), "%u", 0, 0, ' ', "2");
    TEST (T (0, 0, 0, 1, 0, 0, 3), "%u", 0, 0, ' ', "3");
    TEST (T (0, 0, 0, 1, 0, 0, 4), "%u", 0, 0, ' ', "4");
    TEST (T (0, 0, 0, 1, 0, 0, 5), "%u", 0, 0, ' ', "5");
    TEST (T (0, 0, 0, 1, 0, 0, 6), "%u", 0, 0, ' ', "6");
    TEST (T (0, 0, 0, 1, 0, 0, 0), "%u", 0, 0, ' ', "7");
    TEST (T (0, 0, 0, 1, 0, 0, 7), "%u", 0, 0, ' ', 0 /* unspecified */);

    // %Ou: the ISO 8601 weekday as a number in the locale's alternative
    //      representation, where Monday is 1.
    rw_info (0, 0, __LINE__,
             "%%Ou: the ISO 8601 weekday using alternative digits");
    TEST (T (0, 0, 0, 1, 0, 0, 1), "%Ou", 0, 0, ' ', "1");
    TEST (T (0, 0, 0, 1, 0, 0, 2), "%Ou", 0, 0, ' ', "2");
    TEST (T (0, 0, 0, 1, 0, 0, 3), "%Ou", 0, 0, ' ', "3");
    TEST (T (0, 0, 0, 1, 0, 0, 4), "%Ou", 0, 0, ' ', "4");
    TEST (T (0, 0, 0, 1, 0, 0, 5), "%Ou", 0, 0, ' ', "5");
    TEST (T (0, 0, 0, 1, 0, 0, 6), "%Ou", 0, 0, ' ', "6");
    TEST (T (0, 0, 0, 1, 0, 0, 0), "%Ou", 0, 0, ' ', "7");
    TEST (T (0, 0, 0, 1, 0, 0, 8), "%Ou", 0, 0, ' ', 0 /* unspecified */);

    // %U: the week number of the year (the first Sunday as the first
    //     day of week 1) as a decimal number (00-53).
    //     [tm_year, tm_wday, tm_yday]
    rw_info (0, 0, __LINE__, "%%U: the week number of the year");
    TEST (T (), "%U", 0, 0, ' ', "%U");
    TEST (T (), "%U", 0, 0, ' ', "01");
    TEST (T (), "01", 0, 0, ' ', "%U");
    TEST (T (-1), "%U", 0, 0, ' ', "%U");
    TEST (T (0, 0, 0, 1, 0, 320, 2, 60), "%U", 0, 0, ' ', "09");

    // %OU: the week number, using the locale's alternative numeric
    //      symbols.
    rw_info (0, 0, __LINE__, "%%OU: the week number, using alternative digits");
    TEST (T (0, 0, 0, 1, 0, 67, 2, 206), "%U", 0, 0, ' ', "30");
    TEST (T (0, 0, 0, 1, 0, 99, 5, 365), "%U", 0, 0, ' ', "52");

    // %V: the ISO 8601 week number (see below) as a decimal number
    //     (01-53). [tm_year, tm_wday, tm_yday]
    rw_info (0, 0, __LINE__, "%%V: the ISO 8601 week number");

    // 4.3.2.2 of ISO 8601:
    // A calendar week is identified within a calendar year by the calendar
    // week number. This is its ordinal position within the year, applying
    // the rule that the first calendar week of a year is the one that
    // includes the first Thursday of that year and that the last calendar
    // week of a calendar year is the week immediately preceding the first
    // calendar week of the next calendar year.

    // NOTE 1 These rules provide for a calendar year to have 52 or 53
    //        calendar weeks;
    //
    // NOTE 2 The first calendar week of a calendar year may include up
    //        to three days from the previous calendar year; the last
    //        calendar week of a calendar year may include up to three
    //        days from the following calendar year;
    //
    // NOTE 3 The time-interval formed by the week dates of a calendar
    //        year is not the same as the time-interval formed by the
    //        calendar dates or ordinal dates for the same year. For
    //        instance:
    //
    //        Sun 1995 Jan 1 is the 7th day of the 52nd week of 1994
    //        Tue 1996 Dec 31 is the 2nd day of the 1st week 1997.
    //
    // NOTE 4 The rule for determining the first calendar week is
    //        equivalent with the rule the first calendar week is
    //        the week which includes January 4.

    TEST (T (0, 0, 0,  1,  0, 95, 0,   0), "%V", 0, 0, ' ', "52");
    TEST (T (0, 0, 0, 31, 11, 96, 2, 365), "%V", 0, 0, ' ', "01");

    // only tm_year, tm_wday, and tm_yday matter
    TEST (T (0, 0, 0, 1, 0, 95, 0,   0), "%V", 0, 0, ' ', "52");
    TEST (T (0, 0, 0, 1, 0, 96, 2, 365), "%V", 0, 0, ' ', "01");

    TEST (T (0, 0, 0, 1, 0,  99, 5, 364), "%V", 0, 0, ' ', "52");

    TEST (T (0, 0, 0, 1, 0, 103, 0, 361), "%V", 0, 0, ' ', "52");
    TEST (T (0, 0, 0, 1, 0, 103, 1, 362), "%V", 0, 0, ' ', "01");
    TEST (T (0, 0, 0, 1, 0, 104, 0,   3), "%V", 0, 0, ' ', "01");
    TEST (T (0, 0, 0, 1, 0, 104, 1,   4), "%V", 0, 0, ' ', "02");

    TEST (T (0, 0, 0, 1, 0, 109, 0, 360), "%V", 0, 0, ' ', "52");
    TEST (T (0, 0, 0, 1, 0, 109, 1, 361), "%V", 0, 0, ' ', "53");
    TEST (T (0, 0, 0, 1, 0, 109, 4, 364), "%V", 0, 0, ' ', "53");
    TEST (T (0, 0, 0, 1, 0, 110, 5,   0), "%V", 0, 0, ' ', "53");
    TEST (T (0, 0, 0, 1, 0, 110, 0,   2), "%V", 0, 0, ' ', "53");
    TEST (T (0, 0, 0, 1, 0, 110, 1,   3), "%V", 0, 0, ' ', "01");

    // %OV: the ISO 8601 week number, using the locale's alternative
    //      numeric symbols.
    rw_info (0, 0, __LINE__,
             "%%OV: the ISO 8601 week number, using alternative digits");
    TEST (T (0, 0, 0, 1, 0, 104, 0, 360), "%V", 0, 0, ' ', "52");
    TEST (T (0, 0, 0, 1, 0, 104, 1, 361), "%V", 0, 0, ' ', "53");
    TEST (T (0, 0, 0, 1, 0, 104, 5, 365), "%V", 0, 0, ' ', "53");
    TEST (T (0, 0, 0, 1, 0, 105, 6,   0), "%V", 0, 0, ' ', "53");
    TEST (T (0, 0, 0, 1, 0, 105, 0,   1), "%V", 0, 0, ' ', "53");
    TEST (T (0, 0, 0, 1, 0, 105, 1,   2), "%V", 0, 0, ' ', "01");


    // %w: the weekday as a decimal number (0-6), where Sunday is 0.
    //     [tm_wday]
    rw_info (0, 0, __LINE__, "%%w: the weekday");
    TEST (T (), "%w", 0, 0, ' ', "%w");
    TEST (T (), "%w", 0, 0, ' ', "0");
    TEST (T (), "0", 0, 0, ' ', "%w");
    TEST (T (-1), "%w", 0, 0, ' ', "%w");

    TEST (T (0, 0, 0, 1, 0, 0, 1), "%w", 0, 0, ' ', "%w");
    TEST (T (0, 0, 0, 1, 0, 0, 2), "%w", 0, 0, ' ', "%w");
    TEST (T (0, 0, 0, 1, 0, 0, 3), "%w", 0, 0, ' ', "%w");
    TEST (T (0, 0, 0, 1, 0, 0, 4), "%w", 0, 0, ' ', "%w");
    TEST (T (0, 0, 0, 1, 0, 0, 5), "%w", 0, 0, ' ', "%w");
    TEST (T (0, 0, 0, 1, 0, 0, 6), "%w", 0, 0, ' ', "%w");
    TEST (T (0, 0, 0, 1, 0, 0, 6), "%w", 0, 0, ' ', "6");

    // make sure output is not affected by members other than tm_wday
    TEST (T (0, 0, 0, 2, 0, 0, 0), "%w", 0, 0, ' ', "%w");
    TEST (T (0, 0, 0, 2, 0, 0, 1), "%w", 0, 0, ' ', "%w");
    TEST (T (0, 0, 0, 2, 0, 0, 2), "%w", 0, 0, ' ', "%w");
    TEST (T (0, 0, 0, 2, 0, 0, 3), "%w", 0, 0, ' ', "%w");
    TEST (T (0, 0, 0, 2, 0, 0, 4), "%w", 0, 0, ' ', "%w");
    TEST (T (0, 0, 0, 2, 0, 0, 5), "%w", 0, 0, ' ', "%w");
    TEST (T (0, 0, 0, 2, 0, 0, 6), "%w", 0, 0, ' ', "%w");
    TEST (T (0, 0, 0, 2, 0, 0, 6), "%w", 0, 0, ' ', "6");

    // %Ow: the weekday as a number, using the locale's alternative
    //      numeric symbols.
    rw_info (0, 0, __LINE__,
             "%%Ow: the weekday as a number using alternative digits");
    TEST (T (0, 0, 0, 1, 0, 0, 0), "%Ow", 0, 0, ' ', "0");
    TEST (T (0, 0, 0, 1, 0, 0, 1), "%Ow", 0, 0, ' ', "1");
    TEST (T (0, 0, 0, 1, 0, 0, 2), "%Ow", 0, 0, ' ', "2");
    TEST (T (0, 0, 0, 1, 0, 0, 3), "%Ow", 0, 0, ' ', "3");
    TEST (T (0, 0, 0, 1, 0, 0, 4), "%Ow", 0, 0, ' ', "4");
    TEST (T (0, 0, 0, 1, 0, 0, 5), "%Ow", 0, 0, ' ', "5");
    TEST (T (0, 0, 0, 1, 0, 0, 6), "%Ow", 0, 0, ' ', "6");

    // %W: the week number of the year (the first Monday as the first
    //     day of week 1) as a decimal number (00-53).
    //     [tm_year, tm_wday, tm_yday]
    rw_info (0, 0, __LINE__, "%%W: the week number of the year");
    TEST (T (), "%W", 0, 0, ' ', "%W");
    TEST (T (), "%W", 0, 0, ' ', "00");
    TEST (T (), "00", 0, 0, ' ', "%W");
    TEST (T (-1), "%W", 0, 0, ' ', "%W");

    // Monday, 1/1/1900 is week 1 of 1900
    TEST (T (0, 0, 0, 1, 0, 0, 1, 0), "%W", 0, 0, ' ', "%W");
    // Sunday, 1/7/1900 is week 2 of 1900
    TEST (T (0, 0, 0, 1, 0, 0, 0, 6), "%W", 0, 0, ' ', "%W");
    // Monday, 1/1/1901 is week 1 of 1901
    TEST (T (0, 0, 0, 1, 0, 1, 1, 6), "%W", 0, 0, ' ', "%W");

    // %OW: the week number of the year, using the locale's alternative
    //      numeric symbols.

    // Thursday, 7/4/1776 is week 27 of 1776
    TEST (T (0, 0, 0, 1, 0, -124, 4, 185), "%OW", 0, 0, ' ', "27");

    // %x: the locale's appropriate date representation.
    //     [all specified in 7.23.1 of C99]
    rw_info (0, 0, __LINE__,
             "%%x: the locale's appropriate date representation");
    TEST (T (), "%x", 0, 0, ' ', "%x");
    TEST (T (-1), "%x", 0, 0, ' ', "%x");

    // extension: passing (std::tm*)0 to std::time_put<>::put()
    // and 'X' as the format specifier produces the formatting
    // pattern corresponding to the locale date format
#if TEST_RW_EXTENSIONS
    TEST (T (INT_MAX), "%x", 0, 0, ' ', "%m/%d/%y");
#endif

    // %Ex: the locale's alternative date representation.
    rw_info (0, 0, __LINE__,
             "%%Ex: the locale's alternative date representation");
    TEST (T (0, 0, 0, 18, 2, 90), "%Ex", 0, 0, ' ', "03/18/90");

    // %X: the locale's appropriate time representation.
    //     [all specified in 7.23.1 of C99]
    rw_info (0, 0, __LINE__,
             "%%X: the locale's appropriate time representation");
    TEST (T (), "%X", 0, 0, ' ', "%X");
    TEST (T (-1), "%X", 0, 0, ' ', "%X");

    // extension: passing (std::tm*)0 to std::time_put<>::put()
    // and 'X' as the format specifier produces the formatting
    // pattern corresponding to the locale time format
#if TEST_RW_EXTENSIONS
    TEST (T (INT_MAX), "%X", 0, 0, ' ', "%H:%M:%S");
#endif

    // %y: the last 2 digits of the year as a decimal number (00-99).
    //     [tm_year]
    rw_info (0, 0, __LINE__, "%%y: the last 2 digits of the year");
    TEST (T (), "%y", 0, 0, ' ', "%y");
    TEST (T (), "%y", 0, 0, ' ', "00");
    TEST (T (), "00", 0, 0, ' ', "%y");
    TEST (T (-1), "%y", 0, 0, ' ', "%y");

    TEST (T (0, 0, 0, 1, 0,   99), "%y", 0, 0, ' ', "%y");
    TEST (T (0, 0, 0, 1, 0,   99), "%y", 0, 0, ' ', "99");
    TEST (T (0, 0, 0, 1, 0,  199), "%y", 0, 0, ' ', "99");
    TEST (T (0, 0, 0, 1, 0,  299), "%y", 0, 0, ' ', "99");
    TEST (T (0, 0, 0, 1, 0,  399), "%y", 0, 0, ' ', "99");
    TEST (T (0, 0, 0, 1, 0,   -1), "%y", 0, 0, ' ', "99");
    TEST (T (0, 0, 0, 1, 0, -101), "%y", 0, 0, ' ', "99");
    TEST (T (0, 0, 0, 1, 0, -201), "%y", 0, 0, ' ', "99");

    // %Y: the year as a decimal number (e.g., 1997). [tm_year]
    rw_info (0, 0, __LINE__, "%%Y: the year");
    TEST (T (), "%Y", 0, 0, ' ', "%Y");
    TEST (T (), "%Y", 0, 0, ' ', "1900");
    TEST (T (), "1900", 0, 0, ' ', "%Y");
    TEST (T (-1), "%Y", 0, 0, ' ', "%Y");

    //////////////////////////////////////////////////////////////////
    // %z: the offset from UTC in the ISO 8601:2000 standard format
    //     (+hhmm or -hhmm), or no characters if no timezone can be
    //     determined.
    //     For example, "-0430" means 4 hours 30 minutes behind UTC
    //     (west of Greenwich).
    //     If tm_isdst is zero, the standard time offset is used.
    //     If tm_isdst is greater than zero, the daylight savings
    //     time offset is used.
    //     If tm_isdst is negative, no characters are returned.
    //     [tm_isdst]

    rw_info (0, 0, __LINE__, "%%z: the offset from UTC in the ISO 8601 format");

    // UNIX 98 TZ format
    //
    // If TZ is of the first format (that is, if the first character
    // is a colon), the characters following the colon are handled
    // in an implementation-dependent manner. The expanded format
    // (for all TZs whose value does not have a colon as the first
    // character) is as follows:
    //
    //     stdoffset[dst[offset][,start[/time],end[/time]]]
    //
    // Where:
    //
    // std and dst
    // Indicates no less than three, nor more than {TZNAME_MAX}, bytes
    // that are the designation for the standard (std) or the alternative
    // (dst - such as Daylight Savings Time) timezone. Only std is required;
    // if dst is missing, then the alternative time does not apply in this
    // locale.
    // Upper- and lower-case letters are explicitly allowed. Any graphic
    // characters except a leading colon (:) or digits, the comma (,),
    // the minus (-), the plus (+), and the null character are permitted
    // to appear in these fields, but their meaning is unspecified.

    // IEEE Std 1003.1-2001
    //
    // Where:
    //
    // std and dst
    // Indicate no less than three, nor more than {TZNAME_MAX}, bytes
    // that are the designation for the standard (std) or the alternative
    // (dst - such as Daylight Savings Time) timezone. Only std is required;
    // if dst is missing, then the alternative time does not apply in this
    // locale.
    //
    // Each of these fields may occur in either of two formats quoted
    // or unquoted:
    //
    // In the quoted form, the first character shall be the less-than ('<')
    // character and the last character shall be the greater-than ('>')
    // character. All characters between these quoting characters shall
    // be alphanumeric characters from the portable character set in
    // the current locale, the plus-sign ('+') character, or the minus-sign
    // ('-') character. The std and dst fields in this case shall not include
    // the quoting characters.
    //
    // In the unquoted form, all characters in these fields shall be alphabetic
    // characters from the portable character set in the current locale.
    //
    // The interpretation of these fields is unspecified if either field is
    // less than three bytes (except for the case when dst is missing), more
    // than {TZNAME_MAX} bytes, or if they contain characters other than those
    // specified.

    // set TZ to a value in the standard format, arbitrarily choosing
    // strings for the std and, optionally, dst parts of the variable
    // (they're not used for anything except the name(s) assigned to
    // the tznames global variable by tzset())

    // unset TZ -- expect no output
    set_TZ ("TZ=");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0, +1), "%z", 0, 0, ' ', "");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0, -1), "%z", 0, 0, ' ', "");

    // set TZ to GMT (UTC) plus zero hours, no DST
    set_TZ ("TZ=std0");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "+0000");
    // no DST specified, zone info not applicable
    // TEST (T (0, 0, 1, 0, 0, 0, 0, 0, +1), "%z", 0, 0, ' ', "+0000");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0, -1), "%z", 0, 0, ' ', "");

    // set TZ to GMT+7 hours, no DST
    set_TZ ("TZ=foo+7");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "+0700");
    // no DST specified, zone info not applicable
    // TEST (T (0, 0, 1, 0, 0, 0, 0, 0,  1), "%z", 0, 0, ' ', "+0700");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0, -1), "%z", 0, 0, ' ', "");

    // set TZ to GMT+1 hours and DST to GMT+2 hours
    set_TZ ("TZ=abc1def2");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "+0100");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%z", 0, 0, ' ', "+0200");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0, -1), "%z", 0, 0, ' ', "");

    // set TZ to GMT+2 hours and DST to GMT+4 hours
    set_TZ ("TZ=abc2def4");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "+0200");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%z", 0, 0, ' ', "+0400");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0, -1), "%z", 0, 0, ' ', "");

    // set TZ to GMT+3 hours and DST to GMT+4 hours
    set_TZ ("TZ=abc3:00def4:00");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "+0300");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%z", 0, 0, ' ', "+0400");

    // set TZ to GMT+11:30 hours and DST to GMT+11:45 hours
    set_TZ ("TZ=abc11:30:00def11:45");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "+1130");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%z", 0, 0, ' ', "+1145");

    // TZ: STD=GMT+12:34, empty DST
    // According to POSIX, when no offset follows DST, the alternative
    // time is assumed to be one hour ahead of standard time.
    set_TZ ("TZ=bcd12:34efg");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "+1234");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%z", 0, 0, ' ', "+1334");

    // TZ: STD=GMT+23:01, empty DST
    set_TZ ("TZ=cde23:01fgh");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "+2301");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%z", 0, 0, ' ', "+0001");

    // TZ: STD=GMT+23:59, empty DST
    set_TZ ("TZ=def23:59ghi");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "+2359");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%z", 0, 0, ' ', "+0059");

    set_TZ ("TZ=EST03:00<EDT1>04:00");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "+0300");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%z", 0, 0, ' ', "+0400");

#  if 0   // disabled tests of funky std/dst strings

    set_TZ ("TZ=<EST+2>03:21EDT04:32");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "+0321");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%z", 0, 0, ' ', "+0432");

    set_TZ ("TZ=EST04:32:10<EDT+03>05:43:21");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "+0432");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%z", 0, 0, ' ', "+0543");

#  endif   // 0/1

    set_TZ ("TZ=EST-4EDT-5");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "-0400");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%z", 0, 0, ' ', "-0500");

    set_TZ ("TZ=EST-5:00EDT-6:00");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "-0500");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%z", 0, 0, ' ', "-0600");

    set_TZ ("TZ=EST-5:43EDT-6:54");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "-0543");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%z", 0, 0, ' ', "-0654");

#ifdef __GLIBC__

    rw_info (0, 0, __LINE__, "%%z: with tm::tm_gmtoff - GNU glibc extension");

    // exercise support for glibc `tm_gmtoff' extension
    // the extension is supported for all non-zero values of tm_gmtoff
    // (all other struct tm members, including tm_isdst, are ignored)
    // with the idea being that if a program sets the value, it most
    // likely expects it to take effect; otherwise POSIX-compliant
    // behavior is assumed to be expected

    set_TZ ("TZ");   // unset
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0,     60), "%z", 0, 0, ' ', "+0001");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1,    120), "%z", 0, 0, ' ', "+0002");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0, -1,    180), "%z", 0, 0, ' ', "");

    // setting TZ to an arbitrary value should have no effect
    // regardless of the value of the tm_isdst flag
    set_TZ ("TZ=PST8");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0,   3600), "%z", 0, 0, ' ', "+0100");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1,  -7260), "%z", 0, 0, ' ', "-0201");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0, -1,   1234), "%z", 0, 0, ' ', "");

    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0,     60), "%z", 0, 0, ' ', "%z");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1,    120), "%z", 0, 0, ' ', "%z");

    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0,  10800), "%z", 0, 0, ' ', "%z");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1, -14400), "%z", 0, 0, ' ', "%z");

#endif   // __GLIBC__

#ifdef _RWSTD_OS_SUNOS

    // exercise platform-specific behavior -- see man -s 5 environ
    // and the /usr/share/lib/zoneinfo/ Solaris zone info database

    rw_info (0, 0, __LINE__, "%%z: SunOS UZ/zone format [platform-specific]");

    set_TZ ("TZ=US/Eastern");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "+0500");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%z", 0, 0, ' ', "+0600");

    set_TZ ("TZ=US/Central");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "+0600");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%z", 0, 0, ' ', "+0700");

    set_TZ ("TZ=US/Mountain");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "+0700");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%z", 0, 0, ' ', "+0800");

    set_TZ ("TZ=US/Pacific");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%z", 0, 0, ' ', "+0800");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%z", 0, 0, ' ', "+0900");

#endif   // _RWSTD_OS_SUNOS

    //////////////////////////////////////////////////////////////////
    // %Z: the locale's time zone name or abbreviation, or by no
    //     characters if no time zone is determinable. [tm_isdst]
    rw_info (0, 0, __LINE__,
             "%%Z: the locale's time zone name or abbreviation");

    set_TZ ("TZ=foo0bar1");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%Z", 0, 0, ' ', "foo");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%Z", 0, 0, ' ', "bar");

    // the behavior when tmb is invalid (e.g., when (tm_isdst < 0))
    // is undefined, avoid exercising it
    // TEST (T (0, 0, 0, 1, 0, 0, 0, 0, -1), "%Z", 0, 0, ' ', "bar");

    set_TZ ("TZ=EST1EDT2");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%Z", 0, 0, ' ', "EST");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%Z", 0, 0, ' ', "EDT");

    // the behavior when tmb is invalid (e.g., when (tm_isdst < 0))
    // is undefined, avoid exercising it
    // TEST (T (0, 0, 0, 1, 0, 0, 0, 0, -1), "%Z", 0, 0, ' ', "EDT");

    // exercise two-digit hour
    set_TZ ("TZ=ABC01DEF02");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%Z", 0, 0, ' ', "ABC");
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%Z", 0, 0, ' ', "DEF");

    // exercise time zone name of maximum length
#if defined (TZNAME_MAX) && !defined (_MSC_VER)
    // MSVC may define TZNAME_MAX (if _POSIX_ is defined),
    // but tzset() uses hardcoded value equal to 3 instead

    char std_name [TZNAME_MAX + 1];
    std::memset (std_name, 'A', TZNAME_MAX);
    std_name [TZNAME_MAX] = '\0';

    char dst_name [TZNAME_MAX + 1];
    std::memset (dst_name, 'B', TZNAME_MAX);
    dst_name [TZNAME_MAX] = '\0';

    char tz_buf [2 * TZNAME_MAX + 8] = "TZ=";
    std::strcat (tz_buf, std_name);
    std::strcat (tz_buf, "01");
    std::strcat (tz_buf, dst_name);
    std::strcat (tz_buf, "02");

    set_TZ (tz_buf);

    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  0), "%Z", 0, 0, ' ', std_name);
    TEST (T (0, 0, 0, 1, 0, 0, 0, 0,  1), "%Z", 0, 0, ' ', dst_name);
#endif   // TZNAME_MAX && !_MSC_VER

    //////////////////////////////////////////////////////////////////
    // %%: replaced by %
    TEST (T (), " %%",    0, 0, ' ', " %");
#if TEST_RW_EXTENSIONS
    TEST (T (), " %%%",   0, 0, ' ', " %%");
#endif
    TEST (T (), " %%%%",  0, 0, ' ', " %%");
    TEST (T (), " %% %%", 0, 0, ' ', " % %");


    // exercise non-trivial format strings
    TEST (T (), "%%C ", 0, 0, ' ', "%%C ");
    TEST (T (), " %%C", 0, 0, ' ', " %C");
#if TEST_RW_EXTENSIONS
    TEST (T (), " %%C%", 0, 0, ' ', " %C%");
#endif
    TEST (T (), " %%C%%", 0, 0, ' ', " %C%");
    TEST (T (), " %%%C%%", 0, 0, ' ', " %19%");

#if TEST_RW_EXTENSIONS
    TEST (T (), " %%%d%%%", 0, 0, ' ', " %01%%");
#endif
    TEST (T (), " %%%d%%%%", 0, 0, ' ', " %01%%");

    TEST (T (), "%Y%%%y", 0, 0, ' ', "1900%00");
    TEST (T (), "%C%y", 0, 0, ' ', "1900");
    TEST (T (-1), "%Y", 0, 0, ' ', "%Y");

    // exercise padding
    TEST (T (), "%w", 0, 10, ' ', "         0");
    TEST (T (), "%w", 0, 10, '0', "0000000000");
    TEST (T (0, 0, 0, 1, 0, 0, 1), "%w", 0, 10, '0', "0000000001");

    TEST (T (1), "%S", std::ios::left,     10, '*', "01********");
    TEST (T (2), "%S", std::ios::right,    10, '*', "********02");
    TEST (T (3), "%S", std::ios::internal, 10, '*', "********03");

    TEST (T (13), "%S", std::ios::left | std::ios::right, 11, '_',
          "_________13");

    TEST (T (24), "%S", std::ios::left | std::ios::internal, 12, '_',
          "__________24");

    TEST (T (35), "%S",
          std::ios::left | std::ios::right | std::ios::internal, 13, '_',
          "___________35");

    // 7.23.3.5, p5 of C99:
    // %g, %G, and %V give values according to the ISO 8601 week-based
    // year. In this system, weeks begin on a Monday and week 1 of the
    // year is the week that includes January 4th, which is also the
    // week that includes the first Thursday of the year, and is also
    // the first week that contains at least four days in the year.

    // If the first Monday of January is the 2nd, 3rd, or 4th, the
    // preceding days are part of the last week of the preceding year;
    // thus, for Saturday 2nd January 1999, %G is replaced by 1998 and
    // %V is replaced by 53. If December 29th, 30th, or 31st is a Monday,
    // it and any following days are part of week 1 of the following
    // year. Thus, for Tuesday 30th December 1997, %G is replaced by
    // 1998 and %V is replaced by 01.

    // Saturday 2nd January 1999
    TEST (T (0, 0, 0, 2, 0, 99, 6, 1), "%g", 0, 0, ' ', "98");
    TEST (T (0, 0, 0, 2, 0, 99, 6, 1), "%G", 0, 0, ' ', "1998");
    TEST (T (0, 0, 0, 2, 0, 99, 6, 1), "%V", 0, 0, ' ', "53");

    // only [tm_year, tm_wday, tm_yday] are considered
    // leave tm_mday 0 and check that the result is the same
    TEST (T (0, 0, 0, 1, 0, 99, 6, 1), "%g", 0, 0, ' ', "98");
    TEST (T (0, 0, 0, 1, 0, 99, 6, 1), "%G", 0, 0, ' ', "1998");
    TEST (T (0, 0, 0, 1, 0, 99, 6, 1), "%V", 0, 0, ' ', "53");

    // Tuesday 30th December 1997
    TEST (T (0, 0, 0, 30, 11, 97, 2, 363), "%g", 0, 0, ' ', "98");
    TEST (T (0, 0, 0, 30, 11, 97, 2, 363), "%G", 0, 0, ' ', "1998");
    TEST (T (0, 0, 0, 30, 11, 97, 2, 363), "%V", 0, 0, ' ', "01");

    // only [tm_year, tm_wday, tm_yday] are considered
    // leave tm_mday 0 and check the result
    TEST (T (0, 0, 0, 1, 0, 97, 2, 363), "%g", 0, 0, ' ', "98");
    TEST (T (0, 0, 0, 1, 0, 97, 2, 363), "%G", 0, 0, ' ', "1998");
    TEST (T (0, 0, 0, 1, 0, 97, 2, 363), "%V", 0, 0, ' ', "01");
}

/**************************************************************************/


template <class charT>
void test_user (charT, const char *tname)
{
    rw_info (0, 0, 0,
             "std::time_put<%s>, using generated LC_TIME data", tname);

    const char* /* const */ alt_digits[] = {
        "",      "1st",  "2nd",  "3rd",  "4th",
         "5th",  "6th",  "7th",  "8th",  "9th",
        "10th", "11th", "12th", "13th", "14th",
        "15th", "16th", "17th", "18th", "19th",
        "20th", "21st", "22nd", "23rd", "24th",
        "25th", "26th", "27th", "28th", "29th",
        "30th", "31st", 0   // sentinel
    };

    const time_data::era_data era[] = {
        // some entries copied and modified from HP-UX 11.11 ja_JP.utf8
        //
        // # 1990-01-02 and onward:    "Heisei%EyNen" (1990 is Heisei 2 Nen.)
        // # 1990-01-01 to 1990-01-02: "%C"           (no era specified)
        // # 1989-01-08 to 1989-12-31: "HeiseiGannen"
        // # 1927-01-01 to 1989-01-07: "Showa%EyNen"  (1927 is Showa 2 Nen.)
        // # 1927-01-01 and prior:     "foo"
        // era "+:2:1990/01/02:+*:<U????><U????>:%EC%Ey<U????>";
        //     "+:1:1989/01/08:1989/12/31:<U????>:%EC<U????><U????>";
        //     "+:2:1927/01/01:1989/01/07:<U????><U????>:%EC%Ey<U????>";
        //     "-:7:1926/12/31:-*:foo:bar%EC%Ey"

        //   7|2        |1        | |2   starting year in an era
        // <<<|>>>>>>>>>|>>>>>>>>>|.|>>> direction in which years increase
        //    ^         ^         ^ ^
        //    |         |         | |
        //    |         |         | +-> Jan 2, 1990 (Heisei %Ey Nen)
        //    |         |         +-> Dec 31, 1989 (%C)
        //    |         +-> Jan 7, 1989 (Heisei Gannen)
        //    +-> Jan 1, 1927 (Showa %Ey Nen)
        //    +-< Dec 31, 1926 (foobar %Ey)

        {  2,  2,  0, 90,  0,  0, INT_MAX, "Heisei", "%EC %Ey Nen" },
        {  1,  8,  0, 89, 31, 11,      89, "Heisei", "%EC Gannen" },
        {  2,  1,  0, 27,  7,  0,      89, "Showa",  "%EC %Ey Nen" },
        { -7, 31, 11, 26,  0,  0, INT_MIN, "bar",    "foo%EC %Ey" },
        { INT_MIN /* sentinel entry */, 0, 0, 0, 0, 0, 0, 0, 0 }
    };

    const time_data td = {
        // %a
        { "[Sun]", "[Mon]", "[Tue]", "[Wed]", "[Thu]", "[Fri]", "[Sat]" },

        // %A
        {
            "[Sunday]", "[Monday]", "[Tuesday]", "[Wednesday]", "[Thursday]",
            "[Friday]", "[Saturday]"
        },

        // %b
        {
            "[Jan]", "[Feb]", "[Mar]", "[Apr]", "[May]", "[Jun]",
            "[Jul]", "[Aug]", "[Sep]", "[Oct]", "[Nov]", "[Dec]",
        },

        // %B
        {
            "[January]", "[February]", "[March]", "[April]", "[May]", "[June]",
            "[July]", "[August]", "[September]", "[October]", "[November]",
            "[December]",
        },

        { "[A.M.]", "[P.M.]" },             // %p

        "[[%a][%b][%d][%H][%M][%S][%y]]",   // %c
        "[[%3m][%.4d][%4.3y]]",             // %x
        "[[%3H][%.4M][%4.3S]]",             // %X
        "[[%I][%M][%S][%p]]",               // %r
        "[[%A][%B][%d][%H][%M][%S][%Y]]",   // %Ec
        "[[%B][%d][%Y]]",                   // %Ex
        "[[%H hrs][%M min][%S sec]]",       // %EX

        alt_digits, era
    };

    // create a LC_TIME database based on data above
    const char* const locname = make_LC_TIME (&td);

    if (!rw_error (0 != locname, 0, __LINE__,
                   "failed to generate LC_TIME locale data"))
        return;

    // construct a locale object from the binary database
    std::locale loc;

    _TRY {
        loc = std::locale (std::locale (), locname, std::locale::time);
    }
    _CATCH (...) {
        rw_error (0, 0, __LINE__,
                  "std::locale (std::locale (), \"%s\", std::locale::time) "
                  "unexpectedly threw an exception", locname);
        return;
    }

    // set the global locale (used by the test function)
    std::locale::global (loc);

    unsigned i;

    rw_info (0, 0, __LINE__, "%%a: the locale's abbreviated weekday name");
    rw_info (0, 0, __LINE__, "%%A: the locale's full weekday name");

    // verify that data formatted by std::time_put match that specified above
    for (i = 0; i != 7; ++i) {
        TEST (T (0, 0, 0, 1, 0, 0, i), "%a", 0, 0, ' ', td.abday [i]);
        TEST (T (0, 0, 0, 1, 0, 0, i), "%A", 0, 0, ' ', td.day   [i]);
    }

    rw_info (0, 0, __LINE__, "%%b: the locale's abbreviated month name");
    rw_info (0, 0, __LINE__, "%%B: the locale's full month name");

    for (i = 0; i != 12; ++i) {
        TEST (T (0, 0, 0, 1, i), "%b", 0, 0, ' ', td.abmon [i]);
        TEST (T (0, 0, 0, 1, i), "%B", 0, 0, ' ', td.mon   [i]);
    }

    rw_info (0, 0, __LINE__,
             "%%c: the locale's appropriate date and time representation");
    TEST (T (INT_MAX), "%c", 0, 0, ' ', td.d_t_fmt);

    rw_info (0, 0, __LINE__,
             "%%r: the locale's time representation in the AM/PM notation");
    TEST (T (INT_MAX), "%r", 0, 0, ' ', td.t_fmt_ampm);

    rw_info (0, 0, __LINE__,
             "%%X: the locale's appropriate time representation");
    TEST (T (INT_MAX), "%X", 0, 0, ' ', td.t_fmt);

    // extension: format specifies width and optional precision
    // not available through time_put interface (only allowed in
    // formats contained in the database)
    //
    // format "[[%3H][%.4M][%4.3S]]" specifies:
    //            ^    ^^    ^ ^
    //            |    ||    | |
    //            +----|+----|-+-- minimum number of digits, padded with '0'
    //                 +-----+---- minimum field with, padded with ' '
    //
    TEST (T (1, 2, 3),          "%X", 0, 0, ' ', "[[  3][0002][ 001]]");

    rw_info (0, 0, __LINE__,
             "%%x: the locale's appropriate date and time representation");
    TEST (T (0, 0, 0, 4, 5, 6), "%x", 0, 0, ' ', "[[  6][0004][ 006]]");
    TEST (T (INT_MAX),          "%x", 0, 0, ' ', td.d_fmt);


    // %Ec: the alternative date and time representation.
    rw_info (0, 0, __LINE__,
             "%%Ec: the alternative date and time representation");
    TEST (T (INT_MAX),                  "%Ec", 0, 0, ' ', td.era_d_t_fmt);
    TEST (T (22, 5, 18, 26, 3, 102, 5), "%Ec", 0, 0, ' ',
          // "[[%A][%B][%d][%H][%M][%S][%Y]]"
          "[[[Friday]][[April]][26][18][05][22][2002]]");


    // %EC: the name of the base year (period) in the locale's
    //      alternative representation.
    rw_info (0, 0, __LINE__,
             "%%Ec: the name of the base year (period) in the locale's "
             "alternative representation");
    TEST (T (0, 0, 0,  1,  0, -100), "%EC", 0, 0, ' ', era [3].name);
    TEST (T (0, 0, 0,  1,  0,    0), "%EC", 0, 0, ' ', era [3].name);
    TEST (T (0, 0, 0, 31, 11,   26), "%EC", 0, 0, ' ', era [3].name);
    TEST (T (0, 0, 0,  1,  0,   27), "%EC", 0, 0, ' ', era [2].name);
    TEST (T (0, 0, 0,  7,  0,   89), "%EC", 0, 0, ' ', era [2].name);
    TEST (T (0, 0, 0,  8,  0,   89), "%EC", 0, 0, ' ', era [1].name);
    TEST (T (0, 0, 0, 31, 11,   89), "%EC", 0, 0, ' ', era [1].name);
    TEST (T (0, 0, 0,  1,  0,   90), "%EC", 0, 0, ' ', "19");
    TEST (T (0, 0, 0,  2,  0,   90), "%EC", 0, 0, ' ', era [0].name);
    TEST (T (0, 0, 0, 30,  3,  102), "%EC", 0, 0, ' ', era [0].name);


    // %Ex: the locale's alternative date representation.
    rw_info (0, 0, __LINE__,
             "%%Ex: the locale's alternative date representation");
    TEST (T (INT_MAX),             "%Ex", 0, 0, ' ', td.era_d_fmt);
    //                                               "[[%B][%d][%Y]]"
    TEST (T (0, 0, 0, 26, 3, 102), "%Ex", 0, 0, ' ', "[[[April]][26][2002]]");

    // %EX: the locale's alternative time representation.
    rw_info (0, 0, __LINE__,
             "%%EX: the locale's alternative time representation");
    TEST (T (INT_MAX),   "%EX", 0, 0, ' ', td.era_t_fmt);
    //                                     "[[%H hrs][%M min][%S sec]]"
    TEST (T (22, 5, 18), "%EX", 0, 0, ' ', "[[18 hrs][05 min][22 sec]]");

    // %Ey: the offset from %EC (year only) in the alternative representation.
    rw_info (0, 0, __LINE__,
             "%%Ey: the offset from %%EC in the alternative representation");
    TEST (T (0, 0, 0,  1,  0, -100), "%Ey", 0, 0, ' ', "133");   // grows down
    TEST (T (0, 0, 0,  1,  0,    0), "%Ey", 0, 0, ' ', "33");    // grows down
    TEST (T (0, 0, 0,  1,  0,    2), "%Ey", 0, 0, ' ', "31st");  // grows down
    TEST (T (0, 0, 0,  1,  0,   20), "%Ey", 0, 0, ' ', "13th");  // grows down
    TEST (T (0, 0, 0, 31, 11,   26), "%Ey", 0, 0, ' ', "7th");   // grows down
    TEST (T (0, 0, 0,  1,  0,   27), "%Ey", 0, 0, ' ', "2nd");   // grows up
    TEST (T (0, 0, 0,  7,  0,   89), "%Ey", 0, 0, ' ', "64");    // no era
    TEST (T (0, 0, 0,  8,  0,   89), "%Ey", 0, 0, ' ', "1st");   // grows up
    TEST (T (0, 0, 0, 31, 11,   89), "%Ey", 0, 0, ' ', "1st");   // grows up
    TEST (T (0, 0, 0,  1,  0,   90), "%Ey", 0, 0, ' ', "90");    // grows up
    TEST (T (0, 0, 0,  2,  0,   90), "%Ey", 0, 0, ' ', "2nd");   // grows up
    TEST (T (0, 0, 0, 30,  3,  102), "%Ey", 0, 0, ' ', "14th");  // grows up

    // %EY: the full alternative year representation.
    rw_info (0, 0, __LINE__, "%%EY: the full alternative year representation");
    TEST (T (0, 0, 0,  1,  0,    0), "%EY", 0, 0, ' ', "foobar 33");
    TEST (T (0, 0, 0, 31, 11,   26), "%EY", 0, 0, ' ', "foobar 7th");
    TEST (T (0, 0, 0,  1,  0,   27), "%EY", 0, 0, ' ', "Showa 2nd Nen");
    TEST (T (0, 0, 0,  7,  0,   89), "%EY", 0, 0, ' ', "Showa 64 Nen");
    TEST (T (0, 0, 0,  8,  0,   89), "%EY", 0, 0, ' ', "Heisei Gannen");
    TEST (T (0, 0, 0, 31, 11,   89), "%EY", 0, 0, ' ', "Heisei Gannen");
    TEST (T (0, 0, 0,  1,  0,   90), "%EY", 0, 0, ' ', "1990");
    TEST (T (0, 0, 0,  2,  0,   90), "%EY", 0, 0, ' ', "Heisei 2nd Nen");
    TEST (T (0, 0, 0, 30,  3,  102), "%EY", 0, 0, ' ', "Heisei 14th Nen");

    // reset the global locale and delete the temporary combined locale
    std::locale::global (std::locale ("C"));
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    // set up RWSTD_LOCALE_ROOT and other environment variables
    // here as opposed to at program startup to work around a
    // SunOS 5.7 putenv() bug (PR #30017)
    locale_root = rw_set_locale_root ();

    // exercise std::time_put in the C/POSIX locale
    test_POSIX (char (), "char");

#if TEST_RW_EXTENSIONS
    // 'localedef' not working on these machines. Cannot create locale.

    // exercise std::time_put in a user-specified locale
    test_user (char (), "char");
#endif

#ifndef _RWSTD_NO_WCHAR_T

    test_POSIX (wchar_t (), "wchar_t");

#if TEST_RW_EXTENSIONS
    // 'localedef' not working on these machines. Cannot create locale.

    test_user (wchar_t (), "wchar_t");
#endif

#endif   // _RWSTD_NO_WCHAR_T

    return 0;

}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.locale.time.put",
                    0 /* no comment */,
                    run_test,
                    "",
                    0);
}
