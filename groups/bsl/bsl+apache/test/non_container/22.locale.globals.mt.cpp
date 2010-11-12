/************************************************************************
 *
 * 22.locale.globals.mt.cpp
 *
 * test exercising the thread safety of [locale.global.templates]
 *
 * $Id: 22.locale.globals.mt.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 **************************************************************************/

#include <bsls_platform.h>

#include <locale>     // for locale

#include <cwchar>     // for mbstate_t
#include <cstring>    // for strlen()

#include <rw_locale.h>   // for rw_locales()
#include <rw_thread.h>   // for rw_get_processors(), rw_thread_pool()
#include <driver.h>      // for rw_test()


// maximum number of threads allowed by the command line interface
#define MAX_THREADS   16

// default number of threads (will be adjusted to the number
// of processors/cores later)
int opt_nthreads = 1;

// the number of times each thread should iterate (unless specified
// otherwise on the command line)
int opt_nloops = 20000;

enum {
    opt_inx_codecvt,
    opt_inx_collate,
    opt_inx_ctype,
    opt_inx_messages,
    opt_inx_moneypunct,
    opt_inx_moneypunct_intl,
    opt_inx_numpunct,
    opt_inx_money_get,
    opt_inx_money_put,
    opt_inx_num_get,
    opt_inx_num_put,
    opt_inx_time_get,
    opt_inx_time_put,
    opt_nfacets
};

// tri-state options to enable/disable individual facets
//   opt_facets [i] <  0: facet is initially explicitly disabled
//   opt_facets [i] == 0: option not specified on the command line
//   opt_facets [i] >  0: facet is initially explicitly enabled
int opt_facets [opt_nfacets];

// disable exceptions?
int opt_no_exceptions;

/**************************************************************************/

// array of locale names to use for testing
static const char*
locales [MAX_THREADS];

// number of locale names in the array
static std::size_t
nlocales;

/**************************************************************************/

// convenience typedefs for all required specializations of standard facets
typedef std::codecvt<char, char, std::mbstate_t> Codecvt;
typedef std::collate<char>                       Collate;
typedef std::ctype<char>                         Ctype;
typedef std::messages<char>                      Messages;
typedef std::moneypunct<char, false>             Moneypunct0;
typedef std::moneypunct<char, true>              Moneypunct1;
typedef std::numpunct<char>                      Numpunct;
typedef std::money_get<char>                     MoneyGet;
typedef std::money_put<char>                     MoneyPut;
typedef std::num_get<char>                       NumGet;
typedef std::num_put<char>                       NumPut;
typedef std::time_get<char>                      TimeGet;
typedef std::time_put<char>                      TimePut;

typedef std::collate_byname<char>                       CollateByname;
typedef std::ctype_byname<char>                         CtypeByname;
typedef std::codecvt_byname<char, char, std::mbstate_t> CodecvtByname;
typedef std::messages_byname<char>                      MessagesByname;
typedef std::moneypunct_byname<char, false>             Moneypunct0Byname;
typedef std::moneypunct_byname<char, true>              Moneypunct1Byname;
typedef std::numpunct_byname<char>                      NumpunctByname;
typedef std::time_get_byname<char>                      TimeGetByname;
typedef std::time_put_byname<char>                      TimePutByname;


#ifndef _RWSTD_NO_WCHAR_T

typedef std::collate<wchar_t>                       WCollate;
typedef std::ctype<wchar_t>                         WCtype;
typedef std::codecvt<wchar_t, char, std::mbstate_t> WCodecvt;
typedef std::messages<wchar_t>                      WMessages;
typedef std::moneypunct<wchar_t, false>             WMoneypunct0;
typedef std::moneypunct<wchar_t, true>              WMoneypunct1;
typedef std::numpunct<wchar_t>                      WNumpunct;
typedef std::money_get<wchar_t>                     WMoneyGet;
typedef std::money_put<wchar_t>                     WMoneyPut;
typedef std::num_get<wchar_t>                       WNumGet;
typedef std::num_put<wchar_t>                       WNumPut;
typedef std::time_get<wchar_t>                      WTimeGet;
typedef std::time_put<wchar_t>                      WTimePut;

typedef std::collate_byname<wchar_t>                       WCollateByname;
typedef std::ctype_byname<wchar_t>                         WCtypeByname;
typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> WCodecvtByname;
typedef std::messages_byname<wchar_t>                      WMessagesByname;
typedef std::moneypunct_byname<wchar_t, false>             WMoneypunct0Byname;
typedef std::moneypunct_byname<wchar_t, true>              WMoneypunct1Byname;
typedef std::numpunct_byname<wchar_t>                      WNumpunctByname;
typedef std::time_get_byname<wchar_t>                      WTimeGetByname;
typedef std::time_put_byname<wchar_t>                      WTimePutByname;

#endif   // _RWSTD_NO_WCHAR_T


extern "C" {

static void*
test_has_facet (void*)
{
    const std::locale classic (std::locale::classic ());

    for (int i = 0; i != opt_nloops; ++i) {

        // save the name of the locale
        const char* const locale_name = locales [i % nlocales];

        // construct a named locale and imbue it in the ios object
        // so that the locale is used not only by the num_put facet
        // but also by the numpunct facet
        const std::locale loc (locale_name);

        const bool byname = loc != classic;

// call has_facet for each base facet only of it's enabled
#define HAS(Facet, opt) \
    (0 <= opt_facets [opt_inx_ ## opt] ? std::has_facet<Facet>(loc) : true)

        HAS (Codecvt, codecvt);
        HAS (Collate, collate);
        HAS (Ctype, ctype);
        HAS (Messages, messages);
        HAS (Moneypunct0, moneypunct);
        HAS (Moneypunct1, moneypunct_intl);
        HAS (MoneyGet, money_get);
        HAS (MoneyPut, money_put);
        HAS (Numpunct, numpunct);
        HAS (NumGet, num_get); 
        HAS (NumPut, num_put);
        HAS (TimeGet, time_get);
        HAS (TimePut, time_put);

// call has_facet for each _byname facet only of it's enabled
#define HAS_BYNAME(Facet, opt)           \
    (0 <= opt_facets [opt_inx_ ## opt] ? \
         byname == std::has_facet<Facet>(loc) : true)

        HAS_BYNAME (CodecvtByname, codecvt);
        HAS_BYNAME (CollateByname, collate);
        HAS_BYNAME (CtypeByname, ctype);
        HAS_BYNAME (MessagesByname, messages);
        HAS_BYNAME (Moneypunct0Byname, moneypunct);
        HAS_BYNAME (Moneypunct1Byname, moneypunct_intl);
        HAS_BYNAME (TimeGetByname, time_get);
        HAS_BYNAME (TimePutByname, time_put);

#ifndef _RWSTD_NO_WCHAR_T

        HAS (WCodecvt, codecvt);
        HAS (WCollate, collate);
        HAS (WCtype, ctype);
        HAS (WMessages, messages);
        HAS (WMoneypunct0, moneypunct);
        HAS (WMoneypunct1, moneypunct_intl);
        HAS (WMoneyGet, money_get);
        HAS (WMoneyPut, money_put);
        HAS (WNumpunct, numpunct);
        HAS (WNumGet, num_get); 
        HAS (WNumPut, num_put);
        HAS (WTimeGet, time_get);
        HAS (WTimePut, time_put);

        HAS_BYNAME (WCodecvtByname, codecvt);
        HAS_BYNAME (WCollateByname, collate);
        HAS_BYNAME (WCtypeByname, ctype);
        HAS_BYNAME (WMessagesByname, messages);
        HAS_BYNAME (WMoneypunct0Byname, moneypunct);
        HAS_BYNAME (WMoneypunct1Byname, moneypunct_intl);
        HAS_BYNAME (WTimeGetByname, time_get);
        HAS_BYNAME (WTimePutByname, time_put);

#endif   // _RWSTD_NO_WCHAR_T

    }

    return 0;
}

}   // extern "C"


static void
use_facet_loop (const std::locale &classic, int i)
{
    static const std::locale::facet* const dummy =
        (std::locale::facet*)1;

    // save the name of the locale
    const char* const locale_name = locales [i % nlocales];

    // construct a named locale and imbue it in the ios object
    // so that the locale is used not only by the num_put facet
    // but also by the numpunct facet
    const std::locale loc (locale_name);

    const bool byname = loc != classic;

    {

#define USE(Facet, opt) \
    (0 <= opt_facets [opt_inx_ ## opt] ? &std::use_facet<Facet>(loc) : dummy)

        const std::locale::facet* const bases[] = {
            // get pointers to the char specializations of facets
            USE (Collate, collate),
            USE (Ctype, ctype),
            USE (Codecvt, codecvt),
            USE (Messages, messages),
            USE (Moneypunct0, moneypunct),
            USE (Moneypunct1, moneypunct_intl),
            USE (MoneyGet, money_get),
            USE (MoneyPut, money_put),
            USE (Numpunct, numpunct),
            USE (NumGet, num_get),
            USE (NumPut, num_put),
            USE (TimeGet, time_get),
            USE (TimePut, time_put)
        };

        if (byname) {

            // get pointers to the char specializations
            // of byname facets
            const std::locale::facet* const derived[] = {
                USE (CollateByname, collate),
                USE (CtypeByname, ctype),
                USE (CodecvtByname, codecvt),
                USE (MessagesByname, messages),
                USE (Moneypunct0Byname, moneypunct),
                USE (Moneypunct1Byname, moneypunct_intl),
                // no money_get_byname or money_put_byname
                USE (MoneyGet, money_get),
                USE (MoneyPut, money_put),
                USE (NumpunctByname, numpunct),
                // no num_get_byname or num_put_byname
                USE (NumGet, num_get),
                USE (NumPut, num_put),
                USE (TimeGetByname, time_get),
                USE (TimePutByname, time_put)
            };

            const std::size_t nbases = sizeof bases / sizeof *bases;

            for (std::size_t j = 0; j != nbases; ++j) {
                RW_ASSERT (bases [j] != 0);
                RW_ASSERT (bases [j] == derived [j]);
            }
        }
        else if (0 == opt_no_exceptions) {

            bool threw;

#define TEST_USE_FACET(Facet, opt)                              \
    try {                                                       \
        threw = false;                                          \
        if (0 <= opt_facets [opt_inx_ ## opt])                  \
            std::use_facet<Facet>(loc);                         \
    }                                                           \
    catch (...) { threw = true; }                               \
    RW_ASSERT (threw || 0 <= opt_facets [opt_inx_ ## opt])
//    RW_ASSERT (threw || opt_facets [opt_inx_ ## opt] < 0)

            TEST_USE_FACET (CollateByname, collate);
            TEST_USE_FACET (CtypeByname, ctype);
            TEST_USE_FACET (CodecvtByname, codecvt);
            TEST_USE_FACET (MessagesByname, messages);
            TEST_USE_FACET (Moneypunct0Byname, moneypunct);
            TEST_USE_FACET (Moneypunct1Byname, moneypunct_intl);
            TEST_USE_FACET (NumpunctByname, numpunct);
            TEST_USE_FACET (TimeGetByname, time_get);
            TEST_USE_FACET (TimePutByname, time_put);
        }
    }

#ifndef _RWSTD_NO_WCHAR_T

    {
        const std::locale::facet* const bases[] = {
            // get pointers to the char specializations of facets
            USE (WCollate, collate),
            USE (WCtype, ctype),
            USE (WCodecvt, codecvt),
            USE (WMessages, messages),
            USE (WMoneypunct0, moneypunct),
            USE (WMoneypunct1, moneypunct_intl),
            USE (WMoneyGet, money_get),
            USE (WMoneyPut, money_put),
            USE (WNumpunct, numpunct),
            USE (WNumGet, num_get),
            USE (WNumPut, num_put),
            USE (WTimeGet, time_get),
            USE (WTimePut, time_put)
        };

        if (byname) {
            // get pointers to the char specializations
            // of byname facets
            const std::locale::facet* const derived[] = {
                USE (WCollateByname, collate),
                USE (WCtypeByname, ctype),
                USE (WCodecvtByname, codecvt),
                USE (WMessagesByname, messages),
                USE (WMoneypunct0Byname, moneypunct),
                USE (WMoneypunct1Byname, moneypunct_intl),
                // no money_get_byname or money_put_byname
                USE (WMoneyGet, money_get),
                USE (WMoneyPut, money_put),
                USE (WNumpunctByname, numpunct),
                // no num_get_byname or num_put_byname
                USE (WNumGet, num_get),
                USE (WNumPut, num_put),
                USE (WTimeGetByname, time_get),
                USE (WTimePutByname, time_put)
            };

            const std::size_t nbases = sizeof bases / sizeof *bases;

            for (std::size_t j = 0; j != nbases; ++j) {
                RW_ASSERT (bases [j] != 0);
                RW_ASSERT (bases [j] == derived [j]);
            }
        }
        else if (0 == opt_no_exceptions) {

            bool threw;

            TEST_USE_FACET (WCollateByname, collate);
            TEST_USE_FACET (WCtypeByname, ctype);
            TEST_USE_FACET (WCodecvtByname, codecvt);
            TEST_USE_FACET (WMessagesByname, messages);
            TEST_USE_FACET (WMoneypunct0Byname, moneypunct);
            TEST_USE_FACET (WMoneypunct1Byname, moneypunct_intl);
            TEST_USE_FACET (WNumpunctByname, numpunct);
            TEST_USE_FACET (WTimeGetByname, time_get);
            TEST_USE_FACET (WTimePutByname, time_put);
        }
    }

#endif   // _RWSTD_NO_WCHAR_T

}


extern "C" {

static void*
test_use_facet (void*)
{
    const std::locale classic (std::locale::classic ());

    for (int i = 0; i != opt_nloops; ++i) {
        try {
            use_facet_loop (classic, i);
        }
        catch (...) {
            // what to do here?
        }
    }

    return 0;
}


}   // extern "C"

/**************************************************************************/

int opt_has_facet;   // exercise std::has_facet?
int opt_use_facet;   // exercise std::use_facet?


static int
run_test (int, char**)
{
    for (std::size_t i = 0; i != opt_nfacets; ++i) {
        if (0 < opt_facets [i]) {
            for (std::size_t j = 0; j != opt_nfacets; ++j) {
                if (opt_facets [j] == 0)
                    opt_facets [j] = -1;
            }
            break;
        }
    }

    rw_note (0 <= opt_facets [opt_inx_codecvt], 0, __LINE__,
             "std::codecvt tests disabled");

    rw_note (0 <= opt_facets [opt_inx_collate], 0, __LINE__,
             "std::collate tests disabled");

    rw_note (0 <= opt_facets [opt_inx_ctype], 0, __LINE__,
             "std::ctype tests disabled");

    rw_note (0 <= opt_facets [opt_inx_messages], 0, __LINE__,
             "std::messages tests disabled");

    rw_note (0 <= opt_facets [opt_inx_moneypunct], 0, __LINE__,
             "std::moneypunct<charT, false> tests disabled");

    rw_note (0 <= opt_facets [opt_inx_moneypunct_intl], 0, __LINE__,
             "std::moneypunct<charT, true> tests disabled");

    rw_note (0 <= opt_facets [opt_inx_money_get], 0, __LINE__,
             "std::money_get tests disabled");

    rw_note (0 <= opt_facets [opt_inx_money_put], 0, __LINE__,
             "std::money_put tests disabled");

    rw_note (0 <= opt_facets [opt_inx_numpunct], 0, __LINE__,
             "std::numpunct tests disabled");

    rw_note (0 <= opt_facets [opt_inx_num_get], 0, __LINE__,
             "std::num_get tests disabled");

    rw_note (0 <= opt_facets [opt_inx_num_put], 0, __LINE__,
             "std::num_put tests disabled");

    rw_note (0 <= opt_facets [opt_inx_time_get], 0, __LINE__,
             "std::time_get tests disabled");

    rw_note (0 <= opt_facets [opt_inx_time_put], 0, __LINE__,
             "std::time_put tests disabled");

    rw_note (0 == opt_no_exceptions, 0, __LINE__,
             "tests involving exceptions disabled");

    // find all installed locales for which setlocale(LC_ALL) succeeds
#if TEST_RW_EXTENSIONS
    // Only classic 'C' locale implemented by stlport.
    const char* const locale_list =
        rw_opt_locales ? rw_opt_locales : rw_locales (_RWSTD_LC_ALL);
#else
    const char* const locale_list = "C\0";
#endif

    const std::size_t maxinx = sizeof locales / sizeof *locales;

    // set to true if the classic "C" locale is on the lost
    bool has_classic = false;

    for (const char *name = locale_list; *name; name += std::strlen (name) +1) {

        locales [nlocales++] = name;

        if (!has_classic && 0 == std::strcmp ("C", name))
            has_classic = true;

        if (nlocales == maxinx)
            break;
    }

    // when the classic "C" locale isn't on the list put it there
    // unless the list was explicitly specified on the command line
    if (1 < nlocales && !has_classic && 0 == rw_opt_locales)
        locales [0] = "C";

    int result;

    rw_info (0, 0, 0,
             "testing std::locale globals with %d thread%{?}s%{;}, "
             "%d iteration%{?}s%{;} each, in %zu locales { %{ .*A@} }",
             opt_nthreads, 1 != opt_nthreads,
             opt_nloops, 1 != opt_nloops,
             nlocales, int (nlocales), "%#s", locales);

    if (opt_has_facet >= 0) {
        rw_info (0, 0, 0,
                 "template <class T> bool std::has_facet (const locale&)");

        // create and start a pool of threads and wait for them to finish
        result = rw_thread_pool (0, std::size_t (opt_nthreads), 0,
                                 test_has_facet, 0);

        rw_error (result == 0, 0, __LINE__,
                  "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
                  opt_nthreads, test_has_facet);
    }
    else {
        rw_note (0, 0, 0, "std::has_facet test disabled");
    }

    if (opt_use_facet >= 0) {
        rw_info (0, 0, 0,
                 "template <class T> const T& std::use_facet (const locale&)");

#ifdef _RWSTD_NO_DYNAMIC_CAST

        // if dynamic_cast isn't supported, then [has,use]_facet()
        // can't reliably detect if a facet is installed or not.
        rw_warn (0 != opt_no_exceptions, 0, __LINE__,
                 "dynamic_cast not supported "
                 "(macro _RWSTD_NO_DYNAMIC_CAST is #defined), "
                 "disabling exceptions tests");

        opt_no_exceptions = 1;

#endif   // _RWSTD_NO_DYNAMIC_CAST


#ifdef _RWSTD_NO_THREAD_SAFE_EXCEPTIONS

        // avoid exercising exceptions (triggered by use_facet) if
        // their implementation in the runtime isn't thread-safe
        rw_warn (0, 0, 0,
                 "exceptions not thread safe (macro "
                 "_RWSTD_NO_THREAD_SAFE_EXCEPTIONS is #defined), "
                 "disabling exceptions tests");

        opt_no_exceptions = 1;

#endif   // _RWSTD_NO_THREAD_SAFE_EXCEPTIONS


        // create and start a pool of threads and wait for them to finish
        result = rw_thread_pool (0, std::size_t (opt_nthreads), 0,
                                 test_use_facet, 0);

        rw_error (result == 0, 0, __LINE__,
                  "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
                  opt_nthreads, test_use_facet);
    }
    else {
        rw_note (0, 0, 0, "std::use_facet test disabled");
    }

    return result;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
#ifdef _RWSTD_REENTRANT

    // set nthreads to the greater of the number of processors
    // and 2 (for uniprocessor systems) by default
    opt_nthreads = rw_get_cpus ();
    if (opt_nthreads < 2)
        opt_nthreads = 2;

#endif   // _RWSTD_REENTRANT

    return rw_test (argc, argv, __FILE__,
                    "lib.locale.global.templates",
                    "thread safety", run_test,
                    "|-has_facet~ "
                    "|-use_facet~ "
                    "|-nloops#0 "        // arg must be non-negative
                    "|-nthreads#0-* "    // arg must be in [0, MAX_THREADS]
                    "|-locales= "        // argument must be provided
                    "|-no-exceptions# "  // disable exceptions
                    "|-codecvt~ "        // enable/disable individual facets
                    "|-collate~ "
                    "|-ctype~ "
                    "|-messages~ "
                    "|-moneypunct~ "
                    "|-moneypunct_intl~ "
                    "|-money_get~ "
                    "|-money_put~ "
                    "|-numpunct~ "
                    "|-num_get~ "
                    "|-num_put~ "
                    "|-time_get~ "
                    "|-time_put~ ",
                    &opt_has_facet,
                    &opt_use_facet,
                    &opt_nloops,
                    int (MAX_THREADS),
                    &opt_nthreads,
                    &rw_opt_setlocales,
                    &opt_no_exceptions,
                    opt_facets + opt_inx_codecvt,
                    opt_facets + opt_inx_collate,
                    opt_facets + opt_inx_ctype,
                    opt_facets + opt_inx_messages,
                    opt_facets + opt_inx_moneypunct,
                    opt_facets + opt_inx_moneypunct_intl,
                    opt_facets + opt_inx_money_get,
                    opt_facets + opt_inx_money_put,
                    opt_facets + opt_inx_numpunct,
                    opt_facets + opt_inx_num_get,
                    opt_facets + opt_inx_num_put,
                    opt_facets + opt_inx_time_get,
                    opt_facets + opt_inx_time_put);
}
