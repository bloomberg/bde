/***************************************************************************
 *
 * 22.locale.time.get.cpp - tests exercising the std::time_get facet
 *
 * $Id: 22.locale.time.get.cpp 651098 2008-04-23 23:08:29Z sebor $
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
 * Copyright 2001-2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/
#include <bsls_platform.h>

#include <locale>
#include <sstream>   // for stringstream

#include <climits>   // for INT_{MIN,MAX}
#include <clocale>   // for LC_ALL
#include <cstdio>    // for fprintf()
#include <cstdlib>   // for getenv()
#include <cstring>   // for memcmp(), strlen()
#include <ctime>     // for struct tm

#include <driver.h>      // for rw_test()
#include <file.h>        // for rw_nextfd()
#include <rw_locale.h>   // for rw_locale_query()
#include <rw_printf.h>   // for rw_snprintf()

/**************************************************************************/

template <class charT>
void do_test (int lineno, charT, const char *cname,
              std::tm val, const char *pat, int consumed,
              const char *fmt, int flags, int err_expect = -1)
{
    std::basic_ostringstream<charT> ostrm;

    typedef std::istreambuf_iterator<charT> InIter;
    typedef std::ostreambuf_iterator<charT> OutIter;

    const std::time_get<charT, InIter> &tg =
        std::use_facet<std::time_get<charT, InIter> >(ostrm.getloc ());

    const std::time_put<charT, OutIter> &tp =
        std::use_facet<std::time_put<charT, OutIter> >(ostrm.getloc ());

    ostrm.flags (std::ios_base::fmtflags (flags));

    charT wpatbuf [256];
    for (std::size_t j = 0; !!(wpatbuf [j] = pat [j]); ++j);

    const charT *wpat    = wpatbuf;
    const charT *wpatend = wpat + std::char_traits<charT>::length (wpat);

    // format string
    OutIter it (ostrm);
    tp.put (it, ostrm, charT (' '), &val, wpat, wpatend);

    // working around dumb compilers such as MSVC 6.0 that
    // do not zero initialize POD structs when using tm()
    static std::tm zero_tm;
    std::tm x = zero_tm;

    std::basic_istringstream<charT> istrm (ostrm.str ());

    istrm.flags (std::ios_base::fmtflags (flags));

    std::ios_base::iostate err = std::ios_base::goodbit;

    const char *fname = 0;

    InIter ibeg = InIter (istrm);
    InIter iend;

    _TRY {
        if (   '\0' == fmt [1]
            || ('E' ==  fmt [0] || 'O' == fmt [0] && '\0' == fmt [2])) {

            // single-character format strings "a", "b", "x", "X", and "Y"
            // exercise the standard time_get interface, i.e., get_weekday(),
            // get_monthname(), and get_year()

            // other single or two character format strings, e.g., "Ec" or "Ex"
            // exercise the single character extension (format specifier and
            // an optional modifier)

            switch (*fmt) {
            case 'a':
                fname = "get_weekday";
                iend  = tg.get_weekday (ibeg, iend, istrm, err, &x);
                break;

            case 'b':
                fname = "get_monthname";
                iend  = tg.get_monthname (ibeg, iend, istrm, err, &x);
                break;

            case 'x':
                fname = "get_date";
                iend  = tg.get_date (ibeg, iend, istrm, err, &x);
                break;

            case 'X':
                fname = "get_time";
                iend  = tg.get_time (ibeg, iend, istrm, err, &x);
                break;

            case 'Y':
                fname = "get_year";
                iend  = tg.get_year (ibeg, iend, istrm, err, &x);
                break;

            default:
#if TEST_RW_EXTENSIONS
                fname = "get";
                if ('E' == fmt [0] || 'O' == fmt [0])
                    iend = tg.get (ibeg, iend, istrm, err, &x,
                                   fmt [1], fmt [0]);
                else
                    iend = tg.get (ibeg, iend, istrm, err, &x, *fmt);
#endif // TEST_RW_EXTENSIONS
                break;
            }
        }
        else {
#if TEST_RW_EXTENSIONS
            // multi-character format strings exercise the pattern
            // extensions to time_get, ala strptime()

            charT fmtstr [256];
            charT *end;

            const char *pfmt = fmt;
            for (end = fmtstr; *pfmt; *end++ = *pfmt++);

            fname = "get";
            iend  = tg.get (ibeg, iend, istrm, err, &x, fmtstr, end);
#endif // TEST_RW_EXTENSIONS
        }
    }
    _CATCH (...) {
        rw_error (0, 0, lineno,
                  "line %d. time_get<%s>::%s (%{*Ac}, ..., \"%s\") "
                  "unexpectedly threw an exception",
                  __LINE__, cname, fname,
                  int (sizeof (charT)), ostrm.str ().data (), fmt);

        return;
    }

    int extracted = -1 == consumed ? -1
        : int (istrm.rdbuf ()->pubseekoff (0, std::ios::cur, std::ios::in));

    if (extracted == -1)
        consumed = -1;

    int success =
        extracted == consumed && (-1 == err_expect || err == err_expect);

    rw_assert (success, __FILE__, lineno,
               "line %d. time_get<%s>::%s (%{*Ac}, ..., \"%s\") "
               "ate %d, expected %d, rdstate() == %{Is}, got %{Is}",
               __LINE__, cname, fname,
               int (sizeof (charT)), ostrm.str ().data (), fmt,
               extracted, consumed, err_expect, err);

    success = 0 == std::memcmp (&x, &val, sizeof x);

    rw_assert (success, __FILE__, lineno,
               "line %d. time_get<%s>::%s (%{*Ac}, ..., \"%s\") got %{t}, "
               "expected %{t}, flags = %{If}, locale (%s)",
               __LINE__, cname, fname,
               int (sizeof (charT)), ostrm.str ().data (), fmt, &x,
               &val, flags, std::locale ().name ().data ());
}


/**************************************************************************/

std::tm mktm (int sec = 0, int min = 0, int hour = 0,
              int mday = 0, int mon = 0, int year = 0,
              int wday = 0, int yday = 0, int isdst = 0)
{
    std::tm tmp = std::tm ();

    if (sec < 0) {
        // get the current local time
        std::time_t t = std::time (0);
        std::tm *ptm = std::localtime (&t);
        return ptm ? *ptm : tmp;
    }

    // use arguments to initialize struct
    tmp.tm_sec   = sec;     // seconds after the minute [0, 60]
    tmp.tm_min   = min;     // minutes after the hour   [0, 59]
    tmp.tm_hour  = hour;    // hours since midnight     [0, 23]
    tmp.tm_mday  = mday;    // day of the month         [1, 31]
    tmp.tm_mon   = mon;     // months since January     [0, 11]
    tmp.tm_year  = year;    // years since 1900
    tmp.tm_wday  = wday;    // days since Sunday        [0, 6]
    tmp.tm_yday  = yday;    // days since January 1     [0, 365]
    tmp.tm_isdst = isdst;   // Daylight Saving Time flag

    return tmp;
}


// for convenience
#define Boolalpha   std::ios_base::boolalpha
#define Dec         std::ios_base::dec
#define Fixed       std::ios_base::fixed
#define Hex         std::ios_base::hex
#define Internal    std::ios_base::internal
#define Left        std::ios_base::left
#define Oct         std::ios_base::oct
#define Right       std::ios_base::right
#define Scientific  std::ios_base::scientific
#define Showbase    std::ios_base::showbase
#define Showpoint   std::ios_base::showpoint
#define Showpos     std::ios_base::showpos
#define Skipws      std::ios_base::skipws
#define Unitbuf     std::ios_base::unitbuf
#define Uppercase   std::ios_base::uppercase
#define Bin         std::ios_base::bin
#define Adjustfield std::ios_base::adjustfield
#define Basefield   std::ios_base::basefield
#define Floatfield  std::ios_base::floatfield
#define Nolock      std::ios_base::nolock
#define Nolockbuf   std::ios_base::nolockbuf

// capitalize to prevent EDG eccp from expanding the macros
// in .cc files when implicit inclusion is used (causes errors
// in constructs such as traits_type::eof ())
#define Bad         std::ios_base::badbit
#define Eof         std::ios_base::eofbit
#define Fail        std::ios_base::failbit
#define Good        std::ios_base::goodbit


#define T __LINE__, charT (), cname, mktm
#define TEST do_test

#define FUNCTION(fun)                                           \
    rw_info (0, 0, __LINE__,                                    \
             "std::time_get<%s>::%s() in locale (\"%s\")",      \
             cname, fun, std::locale ().name ().c_str ())

#define STEP(desc)   rw_info (0, 0, __LINE__, "%s", desc)


template <class charT>
void test_posix (charT, const char *cname)
{
    // set the global locale object
    std::locale::global (std::locale ("C"));

    // exercise abbreviated weekday names (time_get::get_weekday())
    FUNCTION ("get_weekday");

    TEST (T (0, 0, 0, 0, 0, 0, 0), "%a", -1, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 1), "%a", -1, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 2), "%a", -1, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 3), "%a", -1, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 4), "%a", -1, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 5), "%a", -1, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 6), "%a", -1, "a", 0, Eof);

    // comparison is case insensitive
    TEST (T (0, 0, 0, 0, 0, 0, 0), "Sun", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 0), "SUN", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 0), "sun", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 1), "Mon", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 2), "Tue", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 3), "Wed", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 4), "Thu", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 5), "Fri", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 6), "Sat", 3, "a", 0, Eof);

    // abbreviation followed by a letter other than the one
    // that follows in a full name is acepted
    TEST (T (0, 0, 0, 0, 0, 0, 0), "suna", 3, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 1), "monb", 3, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 2), "tuec", 3, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 3), "wedd", 3, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 4), "thue", 3, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 5), "frif", 3, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 6), "satg", 3, "a", 0, Good);

    // exercise failure on invalid input including incomplete
    // full names (other than valid cases exercised above)
    TEST (T (0, 0, 0, 0, 0, 0, 0), " Sun",      0, "a", 0, Fail);
    TEST (T (0, 0, 0, 0, 0, 0, 0), "Mond",      4, "a", 0, Eof | Fail);
    TEST (T (0, 0, 0, 0, 0, 0, 0), "Tuesd",     5, "a", 0, Eof | Fail);
    TEST (T (0, 0, 0, 0, 0, 0, 0), "Wednesd",   7, "a", 0, Eof | Fail);
    TEST (T (0, 0, 0, 0, 0, 0, 0), "Thursda.",  7, "a", 0, Fail);
    TEST (T (0, 0, 0, 0, 0, 0, 0), "Fridax",    5, "a", 0, Fail);
    TEST (T (0, 0, 0, 0, 0, 0, 0), "6Saturday", 0, "a", 0, Fail);

    // exercise full weekday names
    TEST (T (0, 0, 0, 0, 0, 0, 0), "%A", -1, "a", 0, -1);
    TEST (T (0, 0, 0, 0, 0, 0, 1), "%A", -1, "a", 0, -1);
    TEST (T (0, 0, 0, 0, 0, 0, 2), "%A", -1, "a", 0, -1);
    TEST (T (0, 0, 0, 0, 0, 0, 3), "%A", -1, "a", 0, -1);
    TEST (T (0, 0, 0, 0, 0, 0, 4), "%A", -1, "a", 0, -1);
    TEST (T (0, 0, 0, 0, 0, 0, 5), "%A", -1, "a", 0, -1);
    TEST (T (0, 0, 0, 0, 0, 0, 6), "%A", -1, "a", 0, -1);

    // comparison is case insensitive
    TEST (T (0, 0, 0, 0, 0, 0, 0), "Sunday",    6, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 0), "SUNDAY",    6, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 0), "sunday",    6, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 1), "Monday",    6, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 2), "Tuesday",   7, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 3), "Wednesday", 9, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 4), "Thursday",  8, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 5), "Friday",    6, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 6), "Saturday",  8, "a", 0, Good);

    TEST (T (0, 0, 0, 0, 0, 0, 0), "Sunday ",    6, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 1), "Monday 0",   6, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 2), "Tuesdayy",   7, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 3), "Wednesday.", 9, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 4), "Thursday,",  8, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 5), "Friday/",    6, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 6), "Saturday:",  8, "a", 0, Good);

    // exercise abbreviated month names (time_get::get_monthname())
    FUNCTION ("get_monthname");
    TEST (T (0, 0, 0, 0,  0), "%b", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  1), "%b", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  2), "%b", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  3), "%b", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  4), "%b", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  5), "%b", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  6), "%b", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  7), "%b", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  8), "%b", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  9), "%b", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0, 10), "%b", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0, 11), "%b", -1, "b", 0, -1);

    TEST (T (0, 0, 0, 0,  0), "Jan", 3, "b", 0, Eof);
    TEST (T (0, 0, 0, 0,  1), "Feb", 3, "b", 0, Eof);
    TEST (T (0, 0, 0, 0,  2), "Mar", 3, "b", 0, Eof);
    TEST (T (0, 0, 0, 0,  3), "Apr", 3, "b", 0, Eof);
    TEST (T (0, 0, 0, 0,  4), "May", 3, "b", 0, Good);
    TEST (T (0, 0, 0, 0,  5), "Jun", 3, "b", 0, Eof);
    TEST (T (0, 0, 0, 0,  6), "Jul", 3, "b", 0, Eof);
    TEST (T (0, 0, 0, 0,  7), "Aug", 3, "b", 0, Eof);
    TEST (T (0, 0, 0, 0,  8), "Sep", 3, "b", 0, Eof);
    TEST (T (0, 0, 0, 0,  9), "Oct", 3, "b", 0, Eof);
    TEST (T (0, 0, 0, 0, 10), "Nov", 3, "b", 0, Eof);
    TEST (T (0, 0, 0, 0, 11), "Dec", 3, "b", 0, Eof);

    // exercise invalid abbreviated month names
    TEST (T (0, 0, 0, 0,  0), "J",   1, "b", 0, Eof | Fail);
    TEST (T (0, 0, 0, 0,  0), "J ",  1, "b", 0, Fail);
    TEST (T (0, 0, 0, 0,  0), "Ja",  2, "b", 0, Eof | Fail);
    TEST (T (0, 0, 0, 0,  0), "Ju",  2, "b", 0, Eof | Fail);

    // exercise full month names
    TEST (T (0, 0, 0, 0,  0), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  1), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  2), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  3), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  4), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  5), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  6), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  7), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  8), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  9), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0, 10), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0, 11), "%B", -1, "b", 0, -1);

    TEST (T (0, 0, 0, 0,  0), "January",   7, "b", 0, Good);
    TEST (T (0, 0, 0, 0,  0), "January ",  7, "b", 0, Good);
    TEST (T (0, 0, 0, 0,  1), "February",  8, "b", 0, Good);
    TEST (T (0, 0, 0, 0,  2), "March",     5, "b", 0, Good);
    TEST (T (0, 0, 0, 0,  2), "Marchh",    5, "b", 0, Good);
    TEST (T (0, 0, 0, 0,  3), "April",     5, "b", 0, Good);
    TEST (T (0, 0, 0, 0,  4), "May",       3, "b", 0, Good);
    TEST (T (0, 0, 0, 0,  5), "June",      4, "b", 0, Good);
    TEST (T (0, 0, 0, 0,  6), "July",      4, "b", 0, Good);
    TEST (T (0, 0, 0, 0,  6), "Jull",      3, "b", 0, Good);
    TEST (T (0, 0, 0, 0,  7), "August",    6, "b", 0, Good);
    TEST (T (0, 0, 0, 0,  8), "September", 9, "b", 0, Good);
    TEST (T (0, 0, 0, 0,  9), "October",   7, "b", 0, Good);
    TEST (T (0, 0, 0, 0, 10), "November",  8, "b", 0, Good);
    TEST (T (0, 0, 0, 0, 11), "December",  8, "b", 0, Good);

    // exercise invalid full month names
    TEST (T (0, 0, 0, 0, 0), "Januar",    6, "b", 0, Eof | Fail);
    TEST (T (0, 0, 0, 0, 0), "Februar",   7, "b", 0, Eof | Fail);
    TEST (T (0, 0, 0, 0, 0), " March",    0, "b", 0, Fail);
    TEST (T (0, 0, 0, 0, 0), "apri/",     4, "b", 0, Fail);
    TEST (T (0, 0, 0, 0, 0), "/MAY",      0, "b", 0, Fail);
    TEST (T (0, 0, 0, 0, 0), ":Jun",      0, "b", 0, Fail);
    TEST (T (0, 0, 0, 0, 0), "Ju ",       2, "b", 0, Fail);

    // exercise date (time_get::get_date())
    FUNCTION ("get_date");
    TEST (T (0, 0, 0,  1, 0, 100), "%x", 8, "x", 0, Eof);
    TEST (T (0, 0, 0,  2, 1, 111), "%x", 8, "x", 0, Eof);
    TEST (T (0, 0, 0,  3, 2, 122), "%x", 8, "x", 0, Eof);
    TEST (T (0, 0, 0,  4, 3, 133), "%x", 8, "x", 0, Eof);
    TEST (T (0, 0, 0,  5, 4, 144), "%x", 8, "x", 0, Eof);
    TEST (T (0, 0, 0,  6, 5, 155), "%x", 8, "x", 0, Eof);
    TEST (T (0, 0, 0,  7, 6, 166), "%x", 8, "x", 0, Eof);
    TEST (T (0, 0, 0,  8, 7, 168), "%x", 8, "x", 0, Eof);
    TEST (T (0, 0, 0,  9, 8,  69), "%x", 8, "x", 0, Eof);
    TEST (T (0, 0, 0, 10, 9,  99), "%x", 8, "x", 0, Eof);

    TEST (T (0, 0, 0, 11,  5, 155), "06/11/55", 8, "x", 0, Eof);
    TEST (T (0, 0, 0, 12,  6, 167), "07/12/67", 8, "x", 0, Eof);
    TEST (T (0, 0, 0, 23,  7,  78), "08/23/78", 8, "x", 0, Eof);
    TEST (T (0, 0, 0, 31, 11,  89), "12/31/89", 8, "x", 0, Eof);

    // exercise time (time_get::get_time())
    FUNCTION ("get_time");
    TEST (T ( 0,  0,  0), "%X", 8, "X", 0, Eof);
    TEST (T ( 1, 59, 11), "%X", 8, "X", 0, Eof);
    TEST (T (12, 48, 10), "%X", 8, "X", 0, Eof);
    TEST (T (23, 37,  9), "%X", 8, "X", 0, Eof);
    TEST (T (34, 26,  8), "%X", 8, "X", 0, Eof);
    TEST (T (45, 15,  7), "%X", 8, "X", 0, Eof);
    TEST (T (56,  4,  6), "%X", 8, "X", 0, Eof);

    TEST (T ( 0,  0,  0), "00:00:00", 8, "X", 0, Eof);
    TEST (T (60,  9, 23), "23:09:60", 8, "X", 0, Eof);
    TEST (T (59, 18, 22), "22:18:59", 8, "X", 0, Eof);
    TEST (T (58, 27, 21), "21:27:58", 8, "X", 0, Eof);
    TEST (T (57, 36, 20), "20:36:57", 8, "X", 0, Eof);
    TEST (T (56, 45, 19), "19:45:56", 8, "X", 0, Eof);
    TEST (T (55, 54, 18), "18:54:55", 8, "X", 0, Eof);

    // exercise year (time_get::get_year())
    FUNCTION ("get_year");
    TEST (T (0, 0, 0, 0, 0, -1900),     "0", 1, "Y", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, -1900),    "00", 2, "Y", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, -1899),   "001", 3, "Y", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,  -124),  "1776", 4, "Y", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,     0),  "1900", 4, "Y", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,     1),  "1901", 4, "Y", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,    99),  "1999", 4, "Y", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,   102),  "2002", 4, "Y", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 98099), "99999", 5, "Y", 0, Eof);

    FUNCTION ("get");

    // exercise extensions implementing complete IEEE Std 1003.1-2001 support
    // http://www.opengroup.org/onlinepubs/007904975/functions/strptime.html

    // capital format specifier (other than "X") implies the
    // testing of std::time_get<>::get (..., char, char)

    // %a The day of the week, using the locale's weekday names;
    //    either the abbreviated or full name may be specified.
    // %A Equivalent to %a.
    STEP ("%a, %A: the day of the week");
    TEST (T (0, 0, 0, 0, 0, 0, 0), "Sun", 3, "A", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 1), "Mon", 3, "A", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 2), "Tue", 3, "A", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 3), "Wed", 3, "A", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 4), "Thu", 3, "A", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 5), "Fri", 3, "A", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 6), "Sat", 3, "A", 0, Eof);

    TEST (T (0, 0, 0, 0, 0, 0, 0), "Sunday",    6, "A", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 1), "Monday",    6, "A", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 2), "Tuesday",   7, "A", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 3), "Wednesday", 9, "A", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 4), "Thursday",  8, "A", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 5), "Friday",    6, "A", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 6), "Saturday",  8, "A", 0, Good);

    // %b The month, using the locale's month names;
    //    either the abbreviated or full name may be specified.
    // %B Equivalent to %b.
    STEP ("%b, %B: the month");
    TEST (T (0, 0, 0, 0,  0), "January",   7, "B", 0, Good);
    TEST (T (0, 0, 0, 0,  1), "February",  8, "B", 0, Good);
    TEST (T (0, 0, 0, 0,  2), "March",     5, "B", 0, Good);
    TEST (T (0, 0, 0, 0,  3), "April",     5, "B", 0, Good);
    TEST (T (0, 0, 0, 0,  4), "May",       3, "B", 0, Good);
    TEST (T (0, 0, 0, 0,  5), "June",      4, "B", 0, Good);
    TEST (T (0, 0, 0, 0,  6), "July",      4, "B", 0, Good);
    TEST (T (0, 0, 0, 0,  7), "August",    6, "B", 0, Good);
    TEST (T (0, 0, 0, 0,  8), "September", 9, "B", 0, Good);
    TEST (T (0, 0, 0, 0,  9), "October",   7, "B", 0, Good);
    TEST (T (0, 0, 0, 0, 10), "November",  8, "B", 0, Good);
    TEST (T (0, 0, 0, 0, 11), "December",  8, "B", 0, Good);

    // %c Replaced by the locale's appropriate date and time representation.
    STEP ("%c: date and time representation");
    rw_warn (0, 0, __LINE__, "%%c not being exercised");


    // %C: The century number [00,99]; leading zeros are permitted
    //     but not required.
    STEP ("%C: the century number");
    TEST (T (0, 0, 0, 0, 0, -1900),   "0", 1, "C", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, -1800),   "1", 1, "C", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, -1700),   "2", 1, "C", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, -1600),   "3", 1, "C", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, -1500),  "04", 2, "C", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,     0),  "19", 2, "C", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,   100),  "20", 2, "C", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,   200),  "21", 2, "C", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,  1300),  "32", 2, "C", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,  8000),  "99", 2, "C", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,     0), "100", 3, "C", 0, Eof | Fail);

    // %d The day of the month [01,31]; leading zeros are permitted but not
    //    required.
    STEP ("%d: day of the month");
    TEST (T (0, 0, 0,  1),    "1", 1, "d", 0, Eof);
    TEST (T (0, 0, 0,  2),   "02", 2, "d", 0, Eof);
    TEST (T (0, 0, 0, 28),   "28", 2, "d", 0, Eof);
    TEST (T (0, 0, 0, 29),   "29", 2, "d", 0, Eof);
    TEST (T (0, 0, 0, 30),  "030", 3, "d", 0, Eof);
    TEST (T (0, 0, 0, 31), "0031", 4, "d", 0, Eof);
    TEST (T (0, 0, 0,  0),    "0", 1, "d", 0, Eof | Fail);
    // leading whitespace not allowed
    TEST (T (0, 0, 0,  0),   " 3", 0, "d", 0, Fail);
    TEST (T (0, 0, 0,  0),   "32", 2, "d", 0, Eof | Fail);

    // %D The date as %m/%d/%y.
    STEP ("%D: the date as %m/%d/%y");
    TEST (T (0, 0, 0, 14, 3, 102),      "4/14/2",  6, "D", 0, Eof);
    TEST (T (0, 0, 0, 14, 3, 102),    "04/14/02",  8, "D", 0, Eof);
    TEST (T (0, 0, 0, 14, 3, 102),   "04/14/002",  9, "D", 0, Eof);
    TEST (T (0, 0, 0,  0, 0,   0),    "0/1/2002",  1, "D", 0, Fail);
    TEST (T (0, 0, 0,  0, 0,   0),    "4/0/2002",  3, "D", 0, Fail);
    TEST (T (0, 0, 0,  0, 0,   0),   "4/32/2002",  4, "D", 0, Fail);

    // %e Equivalent to %d; leading zeros are permitted but not required.
    STEP ("%e: equivalent to %d");
    TEST (T (0, 0, 0,  1),  "01", 2, "e", 0, Eof);
    TEST (T (0, 0, 0,  9),   "9", 1, "e", 0, Eof);
    TEST (T (0, 0, 0, 31),  "31", 2, "e", 0, Eof);
    TEST (T (0, 0, 0,  0),   "0", 1, "e", 0, Eof | Fail);
    // leading whitespace not allowed
    TEST (T (0, 0, 0,  0),  " 2", 0, "e", 0, Fail);
    TEST (T (0, 0, 0,  0),  "99", 2, "e", 0, Eof | Fail);

    // %h Equivalent to %b.
    STEP ("%h: equivalent to %b");
    TEST (T (0, 0, 0, 0,  0), "Jan",      3, "h", 0, Eof);
    TEST (T (0, 0, 0, 0,  1), "February", 8, "h", 0, Good);
    TEST (T (0, 0, 0, 0,  2), "Mar",      3, "h", 0, Eof);
    TEST (T (0, 0, 0, 0,  3), "April",    5, "h", 0, Good);
    TEST (T (0, 0, 0, 0,  4), "May",      3, "h", 0, Good);
    TEST (T (0, 0, 0, 0,  5), "June",     4, "h", 0, Good);
    TEST (T (0, 0, 0, 0,  6), "Jul",      3, "h", 0, Eof);
    TEST (T (0, 0, 0, 0,  7), "August",   6, "h", 0, Good);
    TEST (T (0, 0, 0, 0,  8), "Sep",      3, "h", 0, Eof);
    TEST (T (0, 0, 0, 0,  9), "October",  7, "h", 0, Good);
    TEST (T (0, 0, 0, 0, 10), "Nov",      3, "h", 0, Eof);
    TEST (T (0, 0, 0, 0, 11), "December", 8, "h", 0, Good);

    // %H The hour (24-hour clock) [00,23]; leading zeros are permitted
    //    but not required.
    STEP ("%H: the hour (24-hour clock)");
    TEST (T (0, 0,  0),   "0", 1, "H", 0, Eof);
    TEST (T (0, 0,  0),  "00", 2, "H", 0, Eof);
    TEST (T (0, 0,  0), "000", 3, "H", 0, Eof);
    TEST (T (0, 0,  1),   "1", 1, "H", 0, Eof);
    TEST (T (0, 0,  2),   "2", 1, "H", 0, Eof);
    TEST (T (0, 0,  3),   "3", 1, "H", 0, Eof);
    TEST (T (0, 0, 11),  "11", 2, "H", 0, Eof);
    TEST (T (0, 0, 12),  "12", 2, "H", 0, Eof);
    TEST (T (0, 0, 13),  "13", 2, "H", 0, Eof);
    TEST (T (0, 0, 23),  "23", 2, "H", 0, Eof);
    TEST (T (0, 0, 23), "023", 3, "H", 0, Eof);
    TEST (T (0, 0,  0), "024", 3, "H", 0, Eof | Fail);
    TEST (T (0, 0,  0), "987", 3, "H", 0, Eof | Fail);

    // %I The hour (12-hour clock) [01,12]; leading zeros are permitted
    //    but not required.
    STEP ("%I: the hour (12-hour clock)");
    TEST (T (0, 0,  0),   "1", 1, "I", 0, Eof);
    TEST (T (0, 0,  1),   "2", 1, "I", 0, Eof);
    TEST (T (0, 0,  2),   "3", 1, "I", 0, Eof);
    TEST (T (0, 0,  3),   "4", 1, "I", 0, Eof);
    TEST (T (0, 0,  4),   "5", 1, "I", 0, Eof);
    TEST (T (0, 0,  5),   "6", 1, "I", 0, Eof);
    TEST (T (0, 0,  6),   "7", 1, "I", 0, Eof);
    TEST (T (0, 0,  7),   "8", 1, "I", 0, Eof);
    TEST (T (0, 0,  8),   "9", 1, "I", 0, Eof);
    TEST (T (0, 0,  9),  "10", 2, "I", 0, Eof);
    TEST (T (0, 0, 10),  "11", 2, "I", 0, Eof);
    TEST (T (0, 0, 11),  "12", 2, "I", 0, Eof);
    TEST (T (0, 0,  0),  "13", 2, "I", 0, Eof | Fail);
    TEST (T (0, 0,  0), "013", 3, "I", 0, Eof | Fail);
    TEST (T (0, 0,  0), "123", 3, "I", 0, Eof | Fail);
    TEST (T (0, 0,  0),   "0", 1, "I", 0, Eof | Fail);

    // %j The day number of the year [001,366]; leading zeros are permitted
    //    but not required.
    TEST (T (0, 0, 0, 0, 0, 0, 0,   0),   "1", 1, "j", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 0,   1),   "2", 1, "j", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 0,   2),  "03", 2, "j", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 0,   9),  "10", 2, "j", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 0, 364), "365", 3, "j", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 0, 365), "366", 3, "j", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 0,   0),   "0", 1, "j", 0, Eof | Fail);
    TEST (T (0, 0, 0, 0, 0, 0, 0,   0), "367", 3, "j", 0, Eof | Fail);

#ifdef __GLIBC__

    STEP ("%k: equivalent to %H (GNU glibc extension)");

    TEST (T (0, 0,  0),   "0", 1, "k", 0, Eof);
    TEST (T (0, 0,  1),   "1", 1, "k", 0, Eof);
    TEST (T (0, 0,  9),   "9", 1, "k", 0, Eof);
    TEST (T (0, 0, 10),  "10", 2, "k", 0, Eof);
    TEST (T (0, 0, 11),  "11", 2, "k", 0, Eof);
    TEST (T (0, 0, 12),  "12", 2, "k", 0, Eof);
    TEST (T (0, 0, 13),  "13", 2, "k", 0, Eof);
    TEST (T (0, 0, 23),  "23", 2, "k", 0, Eof);
    TEST (T (0, 0, 23), "023", 3, "k", 0, Eof);
    TEST (T (0, 0,  0), "024", 3, "k", 0, Eof | Fail);
    TEST (T (0, 0,  0), "987", 3, "k", 0, Eof | Fail);

#endif   // __GLIBC__

    // %m The month number [01,12]; leading zeros are permitted
    //    but not required.
    STEP ("%m: the month number");
    TEST (T (0, 0, 0, 0,  0, 0),  "1", 1, "m", 0, Eof);
    TEST (T (0, 0, 0, 0,  1, 0),  "2", 1, "m", 0, Eof);
    TEST (T (0, 0, 0, 0,  2, 0),  "3", 1, "m", 0, Eof);
    TEST (T (0, 0, 0, 0,  3, 0),  "4", 1, "m", 0, Eof);
    TEST (T (0, 0, 0, 0,  4, 0),  "5", 1, "m", 0, Eof);
    TEST (T (0, 0, 0, 0,  5, 0),  "6", 1, "m", 0, Eof);
    TEST (T (0, 0, 0, 0,  6, 0),  "7", 1, "m", 0, Eof);
    TEST (T (0, 0, 0, 0,  7, 0),  "8", 1, "m", 0, Eof);
    TEST (T (0, 0, 0, 0,  8, 0),  "9", 1, "m", 0, Eof);
    TEST (T (0, 0, 0, 0,  9, 0), "10", 2, "m", 0, Eof);
    TEST (T (0, 0, 0, 0, 10, 0), "11", 2, "m", 0, Eof);
    TEST (T (0, 0, 0, 0, 11, 0), "12", 2, "m", 0, Eof);
    TEST (T (0, 0, 0, 0,  0, 0), "01", 2, "m", 0, Eof);
    TEST (T (0, 0, 0, 0,  1, 0), "02", 2, "m", 0, Eof);
    TEST (T (0, 0, 0, 0,  0, 0),  "0", 1, "m", 0, Eof | Fail);
    TEST (T (0, 0, 0, 0,  0, 0), "13", 2, "m", 0, Eof | Fail);

    // %M The minute [00,59]; leading zeros are permitted but not required.
    STEP ("%M: the minute");
    TEST (T (0,  0),   "0", 1, "M", 0, Eof);
    TEST (T (0,  0),  "00", 2, "M", 0, Eof);
    TEST (T (0,  0), "000", 3, "M", 0, Eof);
    TEST (T (0,  1),   "1", 1, "M", 0, Eof);
    TEST (T (0,  2),   "2", 1, "M", 0, Eof);
    TEST (T (0, 19),  "19", 2, "M", 0, Eof);
    TEST (T (0, 29),  "29", 2, "M", 0, Eof);
    TEST (T (0, 59),  "59", 2, "M", 0, Eof);
    TEST (T (0,  0),  "60", 2, "M", 0, Eof | Fail);

    // %n Any white space.
    STEP ("%n: any whitespace");
    TEST (T (),     "", 0, "n", 0, Eof);
    TEST (T (),    " ", 1, "n", 0, Eof);
    TEST (T (),   "  ", 2, "n", 0, Eof);
    TEST (T (),  "   ", 3, "n", 0, Eof);
    TEST (T (),  "\n ", 2, "n", 0, Eof);
    TEST (T (),  " \t", 2, "n", 0, Eof);
    TEST (T (), "\n\t", 2, "n", 0, Eof);
    TEST (T (),    ".", 0, "n", 0, Good);
    TEST (T (),   " 1", 1, "n", 0, Good);
    TEST (T (), "\t 2", 2, "n", 0, Good);

    // %p The locale's equivalent of a.m or p.m.
    STEP ("%p: the equivalent of AM/PM");
    TEST (T (0, 0,  1), "AM", 2, "p", 0, Good);
    TEST (T (0, 0, 13), "PM", 2, "p", 0, Good);
    TEST (T (0, 0,  0),  "",  0, "p", 0, Eof | Fail);
    TEST (T (0, 0,  0),  "A", 1, "p", 0, Eof | Fail);
    TEST (T (0, 0,  0),  "P", 1, "p", 0, Eof | Fail);
    TEST (T (0, 0,  0),  "M", 0, "p", 0, Fail);
    TEST (T (0, 0,  0), "A ", 1, "p", 0, Fail);
    TEST (T (0, 0,  0), "P ", 1, "p", 0, Fail);

    // %r 12-hour clock time using the AM/PM notation if t_fmt_ampm is
    //    not an empty string in the LC_TIME portion of the current locale;
    //    in the POSIX locale, this shall be equivalent to %I:%M:%S %p.
    STEP ("%r: 12-hour clock time using the AM/PM notation");
    rw_warn (0, 0, __LINE__, "%%r not being exercised");


    // %R The time as %H:%M.
    STEP ("%R: the time as %H:%M");
    TEST (T (0,  0,  0),      "0:0", 3, "R", 0, Eof);
    TEST (T (0,  1,  0),      "0:1", 3, "R", 0, Eof);
    TEST (T (0,  2,  0),     "0:02", 4, "R", 0, Eof);
    TEST (T (0, 59,  0),     "0:59", 4, "R", 0, Eof);
    TEST (T (0, 48,  1),     "1:48", 4, "R", 0, Eof);
    TEST (T (0, 37, 11),    "11:37", 5, "R", 0, Eof);
    TEST (T (0, 26, 19),    "19:26", 5, "R", 0, Eof);
    TEST (T (0, 15, 22),    "22:15", 5, "R", 0, Eof);
    TEST (T (0,  4, 23),    "23:04", 5, "R", 0, Eof);
    TEST (T (0,  3, 23), "23:03:00", 5, "R", 0, Good);
    TEST (T (0,  2, 23),  "23:2:01", 4, "R", 0, Good);
    TEST (T (0,  0,  0),    "23:60", 5, "R", 0, Eof | Fail);
    TEST (T (0,  0,  0),    "24:00", 2, "R", 0, Fail);

    // %S The seconds [00,60]; leading zeros are permitted but not required.
    STEP ("%S: the seconds");
    TEST (T ( 0),   "0", 1, "S", 0, Eof);
    TEST (T ( 1),  "01", 2, "S", 0, Eof);
    TEST (T ( 2), "002", 3, "S", 0, Eof);
    TEST (T (13),  "13", 2, "S", 0, Eof);
    TEST (T (24),  "24", 2, "S", 0, Eof);
    TEST (T (34),  "34", 2, "S", 0, Eof);
    TEST (T (45),  "45", 2, "S", 0, Eof);
    TEST (T (56),  "56", 2, "S", 0, Eof);
    TEST (T (60),  "60", 2, "S", 0, Eof);
    TEST (T ( 0),  "61", 2, "S", 0, Eof | Fail);

    // %t Any white space.
    STEP ("%t: any whitespace");
    TEST (T (),   "\t", 1, "t", 0, Eof);
    TEST (T (),    " ", 1, "t", 0, Eof);
    TEST (T (),   "  ", 2, "t", 0, Eof);
    TEST (T (),  "\n ", 2, "t", 0, Eof);

    // %T The time as %H:%M:%S.
    STEP ("%T: the time as %H:%M:%S");
    TEST (T ( 0,  0,  0),    "0:0:00", 6, "T", 0, Eof);
    TEST (T ( 1,  1,  0),     "0:1:1", 5, "T", 0, Eof);
    TEST (T ( 2,  2,  0),    "0:02:2", 6, "T", 0, Eof);
    TEST (T ( 3, 59,  0),    "0:59:3", 6, "T", 0, Eof);
    TEST (T ( 4, 48,  1),   "1:48:04", 7, "T", 0, Eof);
    TEST (T ( 5, 37, 11),   "11:37:5", 7, "T", 0, Eof);
    TEST (T ( 6, 26, 19), "19:26:006", 9, "T", 0, Eof);
    TEST (T ( 7, 15, 22),  "22:015:7", 8, "T", 0, Eof);
    TEST (T ( 8,  4, 23), "023:004:8", 9, "T", 0, Eof);
    TEST (T (60, 59, 23), "023:59:60", 9, "T", 0, Eof);
    TEST (T ( 0,  0,  0),  "23:59:61", 8, "T", 0, Eof | Fail);
    TEST (T ( 0,  0,  0),    "0:0:62", 6, "T", 0, Eof | Fail);
    TEST (T ( 0,  0,  0),       "0:0", 3, "T", 0, Eof | Fail);
    TEST (T ( 0,  0,  0),       "999", 3, "T", 0, Eof | Fail);

    // %U The week number of the year (Sunday as the first day of the week)
    //    as a decimal number [00,53]; leading zeros are permitted but not
    //    required.
    STEP ("%U: the Sunday-based week of the year");
    TEST (T (0, 0, 0, 0, 0, 320, 2, 60), "9", 1, "U", 0, Eof);

    // %w The weekday as a decimal number [0,6], with 0 representing
    //    Sunday; leading zeros are permitted but not required.
    STEP ("%w: the Sunday-based weekday as a decimal number");
    TEST (T (0, 0, 0, 0, 0, 0, 0),  "0", 1, "w", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 0), "00", 2, "w", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 1),  "1", 1, "w", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 1), "01", 2, "w", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 2),  "2", 1, "w", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 3),  "3", 1, "w", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 4),  "4", 1, "w", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 5),  "5", 1, "w", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 6),  "6", 1, "w", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 0),  "7", 1, "w", 0, Eof | Fail);

    // %W The week number of the year (Monday as the first day of the
    //    week) as a decimal number [00,53]; leading zeros are permitted
    //    but not required.
    STEP ("%W: the Monday-based week of the year");
    TEST (T (0, 0, 0, 0, 0, 0, 0),  "0", 1, "W", 0, Eof);

    // rw_warn (0, 0, __LINE__, "%%W specifier not being exercised");

    // %x The date, using the locale's date format.
    STEP ("%x: the date");
    TEST (T (0, 0, 0, 25, 6, 67), "7/25/1967", 9, "%x", 0, Eof);

    // %X The time, using the locale's time format.
    STEP ("%X: the time");
    TEST (T (12, 34, 5),  "5:34:12", 7,  "%X", 0, Eof);
    TEST (T (23, 45, 6), " 6:45:23", 8, " %X", 0, Eof);

    // %y The year within century. When a century is not otherwise
    //    specified, values in the range [69,99] shall refer to years 1969
    //    to 1999 inclusive, and values in the range [00,68] shall refer
    //    to years 2000 to 2068 inclusive; leading zeros shall be permitted
    //    but shall not be required.
    //    Note: It is expected that in a future version of
    //          IEEE Std 1003.1-2001 the default century inferred from a
    //          2-digit year will change. (This would apply to all commands
    //          accepting a 2-digit year as input.)
    STEP ("%y: the year within century");
    TEST (T (0, 0, 0, 0, 0,   100),    "0", 1, "%y", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,   100),   "00", 2, "%y", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,   101),    "1", 1, "%y", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,   102),    "2", 1, "%y", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,   103),   "03", 2, "%y", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,   168),   "68", 2, "%y", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,    69),   "69", 2, "%y", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,    99),   "99", 2, "%y", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, -1800),  "100", 3, "%y", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,   102), "2002", 4, "%y", 0, Eof);

    // %Y The year, including the century (for example, 1988).
    STEP ("%Y: the year, including the century");
    TEST (T (0, 0, 0, 0, 0, -1900),      "0", 1, "%Y",  0, Eof);
    TEST (T (0, 0, 0, 0, 0, -1900),     " 0", 2, " %Y", 0, Eof);
    TEST (T (0, 0, 0, 0, 0,    99), "\t1999", 5, " %Y", 0, Eof);

    STEP ("invalid pattern");
    TEST (T (0), "0", 0, "E", 0, Fail);
    TEST (T (0), "0", 0, "i", 0, Fail);
    TEST (T (0), "0", 0, "J", 0, Fail);
    TEST (T (0), "0", 0, "N", 0, Fail);
    TEST (T (0), "0", 0, "P", 0, Fail);
    TEST (T (0), "0", 0, "s", 0, Fail);
    TEST (T (0), "0", 0, "u", 0, Fail);
    TEST (T (0), "0", 0, "z", 0, Fail);
    TEST (T (0), "0", 0, "Z", 0, Fail);


    // %Ec The locale's alternative date and time representation.
    // %EC The name of the base year (period) in the locale's alternative
    //     representation.
    // %Ex The locale's alternative date representation.
    // %EX The locale's alternative time representation.
    // %Ey The offset from %EC (year only) in the locale's alternative
    // representation.
    // %EY The full alternative year representation.
    // %Od The day of the month using the locale's alternative numeric
    //     symbols; leading zeros are permitted but not required.
    // %Oe Equivalent to %Od.
    // %OH The hour (24-hour clock) using the locale's alternative numeric
    //     symbols.
    // %OI The hour (12-hour clock) using the locale's alternative numeric
    //     symbols.
    // %Om The month using the locale's alternative numeric symbols.
    // %OM The minutes using the locale's alternative numeric symbols.
    // %OS The seconds using the locale's alternative numeric symbols.
    // %OU The week number of the year (Sunday as the first day of the week)
    //     using the locale's alternative numeric symbols.
    // %Ow The number of the weekday (Sunday=0) using the locale's
    //     alternative numeric symbols.
    // %OW The week number of the year (Monday as the first day of the
    //     week) using the locale's alternative numeric symbols.
    // %Oy The year (offset from %C ) using the locale's alternative numeric
    // symbols.
    rw_warn (0, 0, __LINE__, "E modifier not being exercised");
    rw_warn (0, 0, __LINE__, "O modifier not being exercised");

    // exercise pattern strings containing multiple format specifiers
    TEST (T (0, 0, 0, 0, 0, 0, 0), "Sun Jan",       7, "%a %b", 0, Eof);
    TEST (T (0, 0, 0, 0, 1, 0, 1), "Mon  Feb",      8, "%a %b", 0, Eof);
    TEST (T (0, 0, 0, 0, 2, 0, 2), "Tue%nMar",      7, "%a %b", 0, Eof);
    TEST (T (0, 0, 0, 0, 3, 0, 3), "Wed%t Apr",     8, "%a %b", 0, Eof);
    TEST (T (0, 0, 0, 0, 2, 0, 5), "Friday March", 12, "%a %b", 0, Good);
}

/**************************************************************************/


template <class charT>
void test_english (charT, const char *cname, const char *locname)
{
    _TRY {
        // set the global locale object to the german locale
        std::locale::global (std::locale (locname));
    }
    _CATCH (...) {
        rw_error (0, 0, __LINE__,
                  "locale(%#s) unexpectedly threw an exception", locname);

        return;
    }

    // exercise abbreviated weekday names
    FUNCTION ("get_weekday");
    TEST (T (0, 0, 0, 0, 0, 0, 0), "Sun", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 1), "Mon", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 2), "Tue", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 3), "Wed", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 4), "Thu", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 5), "Fri", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 6), "Sat", 3, "a", 0, Eof);

    // exercise full weekday names
    TEST (T (0, 0, 0, 0, 0, 0, 0), "Sunday",    6, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 1), "Monday",    6, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 2), "Tuesday",   7, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 3), "Wednesday", 9, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 4), "Thursday",  8, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 5), "Friday",    6, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 6), "Saturday",  8, "a", 0, Good);

    // exercise full month names
    FUNCTION ("get_monthname");
    TEST (T (0, 0, 0, 0,  0), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  1), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  2), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  3), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  4), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  5), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  6), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  7), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  8), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  9), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0, 10), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0, 11), "%B", -1, "b", 0, -1);

    // exercise date (time_get::get_date())
    FUNCTION ("get_date");
    TEST (T (0, 0, 0, 1, 0, 100), "%x", 8, "x", 0, Eof);

    // exercise time (time_get::get_time())
    FUNCTION ("get_time");

    int len;

    {
        // determine whether "%X" is equivalent to "%I:%M:%S %p"
        // (e.g., HP-UX or Linux) or to "%I:%M:%S" (e.g., Compaq
        // Tru64 UNIX)
        char buf [32];
        const std::tm tmb = mktm (0, 0, 1);
        len = std::strftime (buf, sizeof buf, "%X", &tmb);
    }

    const int hour = 11 == len ? 12 : 0;

    TEST (T ( 0, 0,  0), "%X", len, "X", 0, 11 == len ? Good : Eof);
    TEST (T ( 0, 0,  1), "%X", len, "X", 0, 11 == len ? Good : Eof);
    TEST (T ( 0, 0, 11), "%X", len, "X", 0, 11 == len ? Good : Eof);
    TEST (T ( 0, 0, 12), "%X", len, "X", 0, 11 == len ? Good : Eof);
    TEST (T ( 0, 0, 13), "%X", len, "X", 0, 11 == len ? Good : Eof);
    TEST (T ( 0, 0, 23), "%X", len, "X", 0, 11 == len ? Good : Eof);

    TEST (T ( 0, 0, 12 - hour), "12:00:00 AM", len, "X", 0, Good);
    TEST (T ( 0, 0,         1), "01:00:00 AM", len, "X", 0, Good);
    TEST (T ( 0, 0,         2), "02:00:00 AM", len, "X", 0, Good);
    TEST (T ( 0, 0,         3), "03:00:00 AM", len, "X", 0, Good);
    TEST (T ( 0, 0,         4), "04:00:00 AM", len, "X", 0, Good);
    TEST (T ( 0, 0,         5), "05:00:00 AM", len, "X", 0, Good);
    TEST (T ( 0, 0,         6), "06:00:00 AM", len, "X", 0, Good);
    TEST (T ( 0, 0,         7), "07:00:00 AM", len, "X", 0, Good);
    TEST (T ( 0, 0,         8), "08:00:00 AM", len, "X", 0, Good);
    TEST (T ( 0, 0,         9), "09:00:00 AM", len, "X", 0, Good);
    TEST (T ( 0, 0,        10), "10:00:00 AM", len, "X", 0, Good);
    TEST (T ( 0, 0,        11), "11:00:00 AM", len, "X", 0, Good);

    TEST (T ( 0, 0,        12), "12:00:00 PM", len, "X", 0, Good);
    TEST (T ( 0, 0, hour +  1), "01:00:00 PM", len, "X", 0, Good);
    TEST (T ( 0, 0, hour +  2), "02:00:00 PM", len, "X", 0, Good);
    TEST (T ( 0, 0, hour +  3), "03:00:00 PM", len, "X", 0, Good);
    TEST (T ( 0, 0, hour +  4), "04:00:00 PM", len, "X", 0, Good);
    TEST (T ( 0, 0, hour +  5), "05:00:00 PM", len, "X", 0, Good);
    TEST (T ( 0, 0, hour +  6), "06:00:00 PM", len, "X", 0, Good);
    TEST (T ( 0, 0, hour +  7), "07:00:00 PM", len, "X", 0, Good);
    TEST (T ( 0, 0, hour +  8), "08:00:00 PM", len, "X", 0, Good);
    TEST (T ( 0, 0, hour +  9), "09:00:00 PM", len, "X", 0, Good);
    TEST (T ( 0, 0, hour + 10), "10:00:00 PM", len, "X", 0, Good);
    TEST (T ( 0, 0, hour + 11), "11:00:00 PM", len, "X", 0, Good);

    TEST (T (60, 9, hour + 11), "11:09:60 PM", len, "X", 0, Good);
}

/**************************************************************************/


template <class charT>
void test_german (charT, const char *cname, const char *locname)
{
    _TRY {
        // set the global locale object to the german locale
        std::locale::global (std::locale (locname));
    }
    _CATCH (...) {
        rw_error (0, 0, __LINE__,
                  "locale(%#s) unexpectedly threw an exception", locname);

        return;
    }

    // exercise abbreviated weekday names
    // these may be { So, Mo, Di, Mi, Do, Sa } (e.g., Compaq Tru64 UNIX)
    // or { Son, Mon, Die, Mit, Don, Sam } (e.g., Linux, SunOS, or Win32)
    FUNCTION ("get_weekday");
    TEST (T (0, 0, 0, 0, 0, 0, 0), "%a", -1, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 1), "%a", -1, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 2), "%a", -1, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 3), "%a", -1, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 4), "%a", -1, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 5), "%a", -1, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 6), "%a", -1, "a", 0, Eof);

    // exercise full weekday names
    TEST (T (0, 0, 0, 0, 0, 0, 0), "Sonntag",     7, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 1), "Montag",      6, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 2), "Dienstag",    8, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 3), "Mittwoch",    8, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 4), "Donnerstag", 10, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 5), "Freitag",     7, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 6), "Samstag",     7, "a", 0, Good);

    // exercise full month names
    FUNCTION ("get_monthname");
    TEST (T (0, 0, 0, 0,  0), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  1), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  2), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  3), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  4), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  5), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  6), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  7), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  8), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  9), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0, 10), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0, 11), "%B", -1, "b", 0, -1);
}

/**************************************************************************/


template <class charT>
void test_danish (charT, const char *cname, const char *locname)
{
    _TRY {
        // set the global locale object to the german locale
        std::locale::global (std::locale (locname));
    }
    _CATCH (...) {
        rw_error (0, 0, __LINE__,
                  "locale(%#s) unexpectedly threw an exception", locname);

        return;
    }

    // exercise abbreviated weekday names
    FUNCTION ("get_weekday");
    TEST (T (0, 0, 0, 0, 0, 0, 0), "%a", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 1), "%a", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 2), "%a", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 3), "%a", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 4), "%a", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 5), "%a", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 6), "%a", 3, "a", 0, Eof);

    // avoid using <s><o/><n> or <s><o/><n><d><a><g> since it
    // contains the non-ASCII character <o/> (o with a slash)
    TEST (T (0, 0, 0, 0, 0, 0, 1), "man", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 2), "tir", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 3), "ons", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 4), "tor", 3, "a", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 5), "fre", 3, "a", 0, Eof);

    TEST (T (0, 0, 0, 0, 0, 0, 1), "mandag",   6, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 2), "tirsdag",  7, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 3), "onsdag",   6, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 4), "torsdag",  7, "a", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 5), "fredag",   6, "a", 0, Good);

    // exercise full month names
    FUNCTION ("get_monthname");
    TEST (T (0, 0, 0, 0,  0), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  1), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  2), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  3), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  4), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  5), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  6), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  7), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  8), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0,  9), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0, 10), "%B", -1, "b", 0, -1);
    TEST (T (0, 0, 0, 0, 11), "%B", -1, "b", 0, -1);
}

/**************************************************************************/


struct time_data
{
    const char*        abday [7];     // %a   abbreviated day name
    const char*        day [7];       // %A   full day name
    const char*        abmon [12];    // %b   abbreviated month name
    const char*        mon [12];      // %B   full month name
    const char*        am_pm [2];     // %p   AM/PM designator
    const char*        d_t_fmt;       // %c   date and time
    const char*        d_fmt;         // %x   date
    const char*        t_fmt;         // %X   time
    const char*        t_fmt_ampm;    // %r   time with the AM/PM designaror
    const char*        era_d_t_fmt;   // %Ec  alternative date and time
    const char*        era_d_fmt;     // %EX  alternative date
    const char*        era_t_fmt;     // %Ex  alternative time
    const char* const* alt_digits;    //      alternative numeric symbols

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


// the root of the locale directory (RWSTD_LOCALE_ROOT)
// set in main() instead of here to avoid Solaris 7 putenv() bug (PR #30017)
static const char*
locale_root;


static const char*
make_LC_TIME (const time_data *td)
{
    static char locnamebuf [1024];

    if (*locnamebuf)
        return locnamebuf;

    // create a temporary locale definition file
    char srcfname [1024];
    if (rw_snprintf (srcfname, sizeof srcfname, "%s%c%s",
                     locale_root, _RWSTD_PATH_SEP, "LC_TIME.src") < 0)
        return 0;

    std::FILE *fout = std::fopen (srcfname, "w");
    rw_fatal (0 != fout, 0, __LINE__,
              "fopen(%#s, \"w\") failed: %{#m} - %m", srcfname);

    // rw_fatal() doesn't return on failure
    RW_ASSERT (0 != fout);

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
    rw_fatal (0 != fout, 0, __LINE__,
              "fopen(%#s, \"w\") failed: %{#m} - %m", cmfname);

    // rw_fatal() doesn't return on failure
    RW_ASSERT (0 != fout);

    pcs_write (fout, 0);

    std::fclose (fout);

    // process the locale definition file and the character map
    const char* const locname =
        rw_localedef ("-w ", srcfname, cmfname, "test-locale");

    if (locname && (std::strlen(locname) < sizeof locnamebuf))
        std::strcpy (locnamebuf, locname);

    return locname;
}


static const char*
make_LC_TIME ()
{
    const char* const alt_digits[] = {
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

        { "[A.M.]", "[P.M.]" },                 // %p

        "[[%a][%b][%d][%H][%M][%S][%y]]",       // %c
        "[[%3m][%.4d][%4.3y]]",                 // %x
        "[[%3H][%.4M][%4.3S]]",                 // %X
        "[[%I][%M][%S][%p]]",                   // %r
        "[[%A][%B][%Od][%H][%M][%S][%Y]]",      // %Ec
        "%Ow weekday, %OU week, %Oy year",      // %Ex
        "%OH hour, %OM minute, %OS second",     // %EX

        alt_digits, era
    };

    const char* const locname = make_LC_TIME (&td);

    return locname;
}

/**************************************************************************/


template <class charT>
void test_user (charT, const char *cname, const char *locname)
{
    const char* const envar = std::getenv (LOCALE_ROOT_ENVAR);

    rw_info (0, 0, __LINE__,
             "std::time_get<%s>, in locale(\"%s\") "
             "created from a generated LC_TIME file; "
             LOCALE_ROOT_ENVAR "=%s",
             cname, locname, envar);

    // construct a locale object from the binary database
    std::locale loc;

    _TRY {
        loc = std::locale (loc, locname, std::locale::time);
    }
    _CATCH (...) {
        rw_error (0, 0, __LINE__,
                  "locale (locale (), \"%s\", locale::time) "
                  "unexpectedly threw an exception", locname);
        return;
    }

    // set the global locale (used by the test function)
    std::locale::global (loc);


    // %Ec: The locale's alternative date and time representation.
    STEP ("%Ec: alternative date and time representation");
    TEST (T (0, 0, 0, 1, 0, 0, 0),
          // "[[%A][%B][%Od][%H][%M][%S][%Y]]"
          "[[[Sunday]][[January]][1st][00][00][00][1900]]", 46,
          "%Ec", 0, Good);

    TEST (T (5, 4, 3, 2, 1, 1, 1),
          "[[[monday]][[february]][2nd][03][04][05][1901]]", 47,
          "%Ec", 0, Good);

    TEST (T (60, 59, 23, 31, 2, -1898, 2),
          // %Y requires a four digit year, 02 is not treated as 2002
          "[[[TUE]][[MAR]][31st][23][59][60][02]]", 38,
          "%Ec", 0, Good);

    // %EC: The name of the base year (period) in the locale's
    //      alternative representation.

    // %Ex: The locale's alternative date representation.
    STEP ("%Ex: alternative date representation");

    // verify that facet computes the remaining tm members from
    // the supplied data
    TEST (T (0, 0, 0, 21, 4, 2002, 2, 141),
          // "%Ow weekday, %OU week, %Oy year"
          "2nd weekday, 21st week, 2nd year", 32,
          // i.e., May 21, 2002
          "%Ex", 0, Good);

    // %EX: The locale's alternative time representation.
    STEP ("%EX: alternative time representation");
    // verify that 0 is correctly parsed if alternative numeric
    // symbols are expected and the symbol for zero is empty
    TEST (T (0, 1, 2),
          // "%OH hour, %OM minute, %OS second"
          "2nd hour, 1st minute, 0 second", 30,
          "%EX", 0, Good);

    TEST (T (21, 22, 23),
          // "%OH hour, %OM minute, %OS second"
          "23rd hour, 22nd minute, 21st second", 35,
          "%EX", 0, Good);

    // %Ey: The offset from %EC (year only) in the locale's
    //      alternative representation.
    STEP ("%Ey: offset from %EC in alternative representation.");
    rw_warn (0, 0, __LINE__, "%%Ey not being exercised");

    // %EY: The full alternative year representation.
    STEP ("%EY: the full alternative year representation");
    rw_warn (0, 0, __LINE__, "%%EY not being exercised");

    // %Od: The day of the month using the locale's alternative
    //      numeric symbols; leading zeros are permitted but not required.
    STEP ("%Od: the day of the month using alternative numeric symbols");
    TEST (T (0, 0, 0,  1),  "1st", 3, "%Od", 0, Good);
    TEST (T (0, 0, 0,  2),  "2nd", 3, "%Od", 0, Good);
    TEST (T (0, 0, 0, 11), "11th", 4, "%Od", 0, Good);
    TEST (T (0, 0, 0, 21), "21st", 4, "%Od", 0, Good);
    TEST (T (0, 0, 0, 30), "30th", 4, "%Od", 0, Good);
    TEST (T (0, 0, 0, 31), "31st", 4, "%Od", 0, Good);
    TEST (T (0, 0, 0,  0),   "32", 2, "%Od", 0, Eof | Fail);

    // %Oe: Equivalent to %Od.
    STEP ("%Oe: equivalent to %Od");
    TEST (T (0, 0, 0,  3),  "3rd", 3, "%Oe", 0, Good);
    TEST (T (0, 0, 0,  4),  "4th", 3, "%Oe", 0, Good);
    TEST (T (0, 0, 0,  9),  "9th", 3, "%Oe", 0, Good);
    TEST (T (0, 0, 0, 10), "10th", 4, "%Oe", 0, Good);
    TEST (T (0, 0, 0,  0), "32nd", 2, "%Oe", 0, Fail);
    TEST (T (0, 0, 0,  0),   "33", 2, "%Oe", 0, Eof | Fail);

    // %OH: The hour (24-hour clock) using the locale's alternative
    //      numeric symbols.
    STEP ("%OH: the hour (24-hour clock) using alternative numeric symbols");
    TEST (T (0, 0,  0),    "0", 1, "%OH", 0, Eof);
    TEST (T (0, 0,  0),   "00", 2, "%OH", 0, Eof);
    TEST (T (0, 0,  1),  "1st", 3, "%OH", 0, Good);
    TEST (T (0, 0,  2),  "2nd", 3, "%OH", 0, Good);
    TEST (T (0, 0, 11), "11th", 4, "%OH", 0, Good);
    TEST (T (0, 0, 12), "12th", 4, "%OH", 0, Good);
    TEST (T (0, 0, 13), "13th", 4, "%OH", 0, Good);
    TEST (T (0, 0, 23), "23rd", 4, "%OH", 0, Good);

    // %OI: The hour (12-hour clock) using the locale's alternative
    //      numeric symbols.
    STEP ("%OI: the hour (12-hour clock) using alternative numeric symbols");
    TEST (T (0, 0,  0),    "0", 1, "%OI", 0, Eof);
    TEST (T (0, 0,  0),  "000", 3, "%OI", 0, Eof);
    TEST (T (0, 0,  3),  "3rd", 3, "%OI", 0, Good);
    TEST (T (0, 0, 11), "11th", 4, "%OI", 0, Good);
    TEST (T (0, 0, 12), "12th", 4, "%OI", 0, Good);
    TEST (T (0, 0,  0), "13th", 4, "%OI", 0, Fail);
    TEST (T (0, 0,  0),   "14", 2, "%OI", 0, Eof | Fail);

    // %Om: The month using the locale's alternative numeric symbols.
    STEP ("%Om: the month using alternative numeric symbols");
    TEST (T (0, 0, 0, 0,  0),  "1st", 3, "%Om", 0, Good);
    TEST (T (0, 0, 0, 0,  1),  "2nd", 3, "%Om", 0, Good);
    TEST (T (0, 0, 0, 0,  2),  "3rd", 3, "%Om", 0, Good);
    TEST (T (0, 0, 0, 0,  3),  "4th", 3, "%Om", 0, Good);
    TEST (T (0, 0, 0, 0,  4),  "5th", 3, "%Om", 0, Good);
    TEST (T (0, 0, 0, 0,  5),  "6th", 3, "%Om", 0, Good);
    TEST (T (0, 0, 0, 0,  6),  "7th", 3, "%Om", 0, Good);
    TEST (T (0, 0, 0, 0,  7),  "8th", 3, "%Om", 0, Good);
    TEST (T (0, 0, 0, 0,  8),  "9th", 3, "%Om", 0, Good);
    TEST (T (0, 0, 0, 0,  9), "10th", 4, "%Om", 0, Good);
    TEST (T (0, 0, 0, 0, 10), "11th", 4, "%Om", 0, Good);
    TEST (T (0, 0, 0, 0, 11), "12th", 4, "%Om", 0, Good);
    TEST (T (0, 0, 0, 0,  0), "13th", 4, "%Om", 0, Fail);
    TEST (T (0, 0, 0, 0,  0),    "0", 1, "%Om", 0, Eof | Fail);
    TEST (T (0, 0, 0, 0,  0),   "00", 2, "%Om", 0, Eof | Fail);
    TEST (T (0, 0, 0, 0,  0),   "01", 2, "%Om", 0, Eof | Fail);

    // %OM: The minutes using the locale's alternative numeric symbols.
    STEP ("%OM: the minutes using alternative numeric symbols");
    TEST (T (0,  0),    "0", 1, "%OM", 0, Eof);
    TEST (T (0,  1),  "1st", 3, "%OM", 0, Good);
    TEST (T (0,  2),  "2nd", 3, "%OM", 0, Good);
    TEST (T (0,  3),  "3rd", 3, "%OM", 0, Good);
    TEST (T (0,  4),  "4th", 3, "%OM", 0, Good);
    TEST (T (0, 29), "29th", 4, "%OM", 0, Good);
    TEST (T (0, 30), "30th", 4, "%OM", 0, Good);
    TEST (T (0, 31), "31st", 4, "%OM", 0, Good);
    TEST (T (0, 32),   "32", 2, "%OM", 0, Eof);
    TEST (T (0,  0),   "60", 2, "%OM", 0, Eof | Fail);
    TEST (T (0,  0),    "2", 1, "%OM", 0, Eof | Fail);

    // %OS: The seconds using the locale's alternative numeric symbols.
    STEP ("%OS: the seconds using alternative numeric symbols");
    TEST (T ( 0),    "0", 1, "%OS", 0, Eof);
    TEST (T ( 1),  "1st", 3, "%OS", 0, Good);
    TEST (T ( 2),  "2nd", 3, "%OS", 0, Good);
    TEST (T ( 3),  "3rd", 3, "%OS", 0, Good);
    TEST (T ( 4),  "4th", 3, "%OS", 0, Good);
    TEST (T (29), "29th", 4, "%OS", 0, Good);
    TEST (T (30), "30th", 4, "%OS", 0, Good);
    TEST (T (31), "31st", 4, "%OS", 0, Good);
    TEST (T (32),   "32", 2, "%OS", 0, Eof);
    TEST (T (59),  "059", 3, "%OS", 0, Eof);
    TEST (T (60), "0060", 4, "%OS", 0, Eof);
    TEST (T ( 0),   "61", 2, "%OS", 0, Eof | Fail);
    TEST (T ( 0),   "21", 2, "%OS", 0, Eof | Fail);

    // %OU: The week number of the year (Sunday as the first day of the week)
    //      using the locale's alternative numeric symbols.
    STEP ("%OU: the Sunday-based week using alternative numeric symbols");
    rw_warn (0, 0, __LINE__, "%%OU not being exercised");

    // %Ow: The number of the weekday (Sunday=0) using the locale's
    //      alternative numeric symbols.
    STEP ("%Ow: Sunday-based weekday using alternative numeric symbols");
    TEST (T (0, 0, 0, 0, 0, 0, 0),   "0", 1, "%Ow", 0, Eof);
    TEST (T (0, 0, 0, 0, 0, 0, 1), "1st", 3, "%Ow", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 2), "2nd", 3, "%Ow", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 6), "6th", 3, "%Ow", 0, Good);
    TEST (T (0, 0, 0, 0, 0, 0, 0), "7th", 3, "%Ow", 0, Fail);
    TEST (T (0, 0, 0, 0, 0, 0, 0),   "8", 1, "%Ow", 0, Eof | Fail);

    // %OW: The week number of the year (Monday as the first day of the week)
    //      using the locale's alternative numeric symbols.
    STEP ("%OW: the Monday-based week using alternative numeric symbols");

    TEST (T (0, 0, 0, 0, 0, 0, 1), "1st", 3, "%OW", 0, Good);
    // rw_warn (0, 0, __LINE__, "%%OW not being exercised");

    // %Oy: The year (offset from %C ) using the locale's alternative
    //      numeric symbols.
    STEP ("%Oy: year offset from %C using alternative numeric symbols");
    rw_warn (0, 0, __LINE__, "%%Oy not being exercised");
}

/**************************************************************************/

static int
run_test (int, char**)
{
    // set up RWSTD_LOCALE_ROOT and other environment variables
    // here as opposed to at program startup to work around a
    // SunOS 5.7 bug in putenv() (PR ##30017)
    locale_root = rw_set_locale_root ();

    // store the next available file descriptor
    // in order to detect file descriptor leaks
    const int fd0 = rw_nextfd (0);

    test_posix (char (), "char");

    const char *locname;

    const char en[] = "en-*-*-*";
    const char de[] = "de-*-*-*";
    const char da[] = "da-*-*-*";

    int nnamed = 0;

#if TEST_RW_EXTENSIONS
    // Only classic 'C' locale defined for stlport.

    // try to find and exercise the english locale
    if ((locname = rw_locale_query (LC_ALL, en, 1)) && *locname) {
        test_english (char (), "char", locname);
        ++nnamed;
    }

    // try to find and exercise the german locale
    if ((locname = rw_locale_query (LC_ALL, de, 1)) && *locname) {
        test_german (char (), "char", locname);
        ++nnamed;
    }

    // try to find and exercise the danish locale
    if ((locname = rw_locale_query (LC_ALL, da, 1)) && *locname) {
        test_danish (char (), "char", locname);
        ++nnamed;
    }
#endif  // TEST_RW_EXTENSIONS

    //////////////////////////////////////////////////////////////////
    // exercise the time_get facets with a user-defined locale
    std::locale::global (std::locale ("C"));

    // create a LC_TIME database based on user-defined data

#if TEST_RW_EXTENSIONS
    // 'localedef' not working on these machines. Cannot create locale.
    locname = make_LC_TIME ();

    if (!locname) {
        rw_error (0, 0, __LINE__, "failed to generate LC_TIME locale data");
        return 0;
    }

    test_user (char (), "char", locname);

#ifndef _RWSTD_NO_WCHAR_T

    test_posix (wchar_t (), "wchar_t");

    test_user (wchar_t (), "wchar_t", locname);

#endif   // _RWSTD_NO_WCHAR_T

    // reset the gloab locale to free up resources (such as file
    // descriptors used by time_get_byname to map locale databases)
    std::locale::global (std::locale ("C"));

    const int fd1 = rw_nextfd (0);

    rw_assert (0 == fd1 - fd0, 0, __LINE__,
               "%d file descriptor leaks detected", fd1 - fd0);

    rw_assert (0 != nnamed, 0, __LINE__,
               "time_get not exercised in named locales");

#endif  // TEST_RW_EXTENSIONS
    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.locale.time.get",
                    0 /* no comment */,
                    run_test,
                    "",
                    0);
}
