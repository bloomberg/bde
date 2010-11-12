/************************************************************************
 *
 * 22.locale.moneypunct.mt.cpp
 *
 * test exercising the thread safety of the moneypunct facet
 *
 * $Id: 22.locale.moneypunct.mt.cpp 648752 2008-04-16 17:01:56Z faridz $
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

#include <ios>        // for ios
#include <locale>     // for locale, moneypunct

#include <cstdlib>    // for mbstowcs(), size_t
#include <cstring>    // for strcpy()

#include <rw_locale.h>
#include <rw_thread.h>
#include <driver.h>
#include <valcmp.h>


// maximum number of threads allowed by the command line interface
#define MAX_THREADS   32

// default number of threads (will be adjusted to the number
// of processors/cores later)
int rw_opt_nthreads = 1;

// the default number of times for each thread to iterate
#define DFLT_LOOPS   10000

// the number of times each thread should iterate (will be set to
// DFLT_LOOPS unless explicitly specified on the command line)
int rw_opt_nloops = -1;

/**************************************************************************/

// number of locales to test
static std::size_t
nlocales;

/**************************************************************************/

struct MoneypunctData
{
    // the name of the locale the data goes with
    const char* locale_name_;

    char decimal_point_;
    char thousands_sep_;
    char grouping_ [32];
    char curr_symbol_ [32];
    char positive_sign_ [32];
    char negative_sign_ [32];
    int  frac_digits_;
    char pos_format_ [32];
    char neg_format_ [32];

    char int_curr_symbol_ [32];

    int  int_frac_digits_;
    char int_pos_format_ [32];
    char int_neg_format_ [32];

#ifndef _RWSTD_NO_WCHAR_T

    wchar_t wdecimal_point_;
    wchar_t wthousands_sep_;
    wchar_t wcurr_symbol_ [32];
    wchar_t wpositive_sign_ [32];
    wchar_t wnegative_sign_ [32];

    wchar_t wint_curr_symbol_ [32];

#endif   // _RWSTD_NO_WCHAR_T

} punct_data [MAX_THREADS];

/**************************************************************************/

bool test_char;    // exercise num_put<char>
bool test_wchar;   // exercise num_put<wchar_t>

static void
thread_loop_body (std::size_t i)
{
    const std::size_t inx = std::size_t (i) % (nlocales ? nlocales : 1);

    const MoneypunctData* const data = punct_data + inx;

    // construct a named locale
    const std::locale loc (data->locale_name_);

    if (test_char) {
        // exercise the narrow char, local specialization of the facet

        typedef std::moneypunct<char, false> Punct;

        const Punct &mp = std::use_facet<Punct>(loc);

        const char           dp  = mp.decimal_point ();
        const char           ts  = mp.thousands_sep ();
        const std::string    grp = mp.grouping ();
        const std::string    cur = mp.curr_symbol ();
        const std::string    pos = mp.positive_sign ();
        const std::string    neg = mp.negative_sign ();
        const int            fd  = mp.frac_digits ();
        const Punct::pattern pfm = mp.pos_format ();
        const Punct::pattern nfm = mp.neg_format ();

        RW_ASSERT (dp == data->decimal_point_);
        RW_ASSERT (ts == data->thousands_sep_);
        RW_ASSERT (fd == data->frac_digits_);
        RW_ASSERT (!rw_strncmp (grp.c_str (), data->grouping_));
        RW_ASSERT (!rw_strncmp (cur.c_str (), data->curr_symbol_));
        RW_ASSERT (!rw_strncmp (pos.c_str (), data->positive_sign_));
        RW_ASSERT (!rw_strncmp (neg.c_str (), data->negative_sign_));

        RW_ASSERT (!std::memcmp (&pfm, data->pos_format_, 4));
        RW_ASSERT (!std::memcmp (&nfm, data->neg_format_, 4));
    }

    if (test_char) {
        // exercise the narrow char, international specialization

        typedef std::moneypunct<char, true> Punct;

        const Punct &mp = std::use_facet<Punct>(loc);

        const char           dp  = mp.decimal_point ();
        const char           ts  = mp.thousands_sep ();
        const std::string    grp = mp.grouping ();
        const std::string    cur = mp.curr_symbol ();
        const std::string    pos = mp.positive_sign ();
        const std::string    neg = mp.negative_sign ();
        const int            fd  = mp.frac_digits ();
        const Punct::pattern pfm = mp.pos_format ();
        const Punct::pattern nfm = mp.neg_format ();

        RW_ASSERT (dp == data->decimal_point_);
        RW_ASSERT (ts == data->thousands_sep_);
        RW_ASSERT (fd == data->frac_digits_);
        RW_ASSERT (!rw_strncmp (grp.c_str (), data->grouping_));
        RW_ASSERT (!rw_strncmp (cur.c_str (), data->int_curr_symbol_));
        RW_ASSERT (!rw_strncmp (pos.c_str (), data->positive_sign_));
        RW_ASSERT (!rw_strncmp (neg.c_str (), data->negative_sign_));

        RW_ASSERT (!std::memcmp (&pfm, data->int_pos_format_, 4));
        RW_ASSERT (!std::memcmp (&nfm, data->int_neg_format_, 4));
    }

    // both specializations may be tested at the same time

#ifndef _RWSTD_NO_WCHAR_T

    if (test_wchar) {
        // exercise the wide char, local specialization of the facet

        typedef std::moneypunct<wchar_t, false> Punct;

        const Punct &mp = std::use_facet<Punct>(loc);

        const wchar_t        dp  = mp.decimal_point ();
        const wchar_t        ts  = mp.thousands_sep ();
        const std::string    grp = mp.grouping ();
        const std::wstring   cur = mp.curr_symbol ();
        const std::wstring   pos = mp.positive_sign ();
        const std::wstring   neg = mp.negative_sign ();
        const int            fd  = mp.frac_digits ();
        const Punct::pattern pfm = mp.pos_format ();
        const Punct::pattern nfm = mp.neg_format ();

        RW_ASSERT (dp == data->wdecimal_point_);
        RW_ASSERT (ts == data->wthousands_sep_);
        RW_ASSERT (fd == data->frac_digits_);
        RW_ASSERT (!rw_strncmp (grp.c_str (), data->grouping_));
        RW_ASSERT (!rw_strncmp (cur.c_str (), data->wcurr_symbol_));
        RW_ASSERT (!rw_strncmp (pos.c_str (), data->wpositive_sign_));
        RW_ASSERT (!rw_strncmp (neg.c_str (), data->wnegative_sign_));

        RW_ASSERT (!std::memcmp (&pfm, data->pos_format_, 4));
        RW_ASSERT (!std::memcmp (&nfm, data->neg_format_, 4));
    }

    if (test_wchar) {
        // exercise the wide char, international specialization

        typedef std::moneypunct<wchar_t, true> Punct;

        const Punct &mp = std::use_facet<Punct>(loc);

        const wchar_t        dp  = mp.decimal_point ();
        const wchar_t        ts  = mp.thousands_sep ();
        const std::string    grp = mp.grouping ();
        const std::wstring   cur = mp.curr_symbol ();
        const std::wstring   pos = mp.positive_sign ();
        const std::wstring   neg = mp.negative_sign ();
        const int            fd  = mp.frac_digits ();
        const Punct::pattern pfm = mp.pos_format ();
        const Punct::pattern nfm = mp.neg_format ();

        RW_ASSERT (dp == data->wdecimal_point_);
        RW_ASSERT (ts == data->wthousands_sep_);
        RW_ASSERT (fd == data->frac_digits_);
        RW_ASSERT (!rw_strncmp (grp.c_str (), data->grouping_));
        RW_ASSERT (!rw_strncmp (cur.c_str (), data->wint_curr_symbol_));
        RW_ASSERT (!rw_strncmp (pos.c_str (), data->wpositive_sign_));
        RW_ASSERT (!rw_strncmp (neg.c_str (), data->wnegative_sign_));

        RW_ASSERT (!std::memcmp (&pfm, data->int_pos_format_, 4));
        RW_ASSERT (!std::memcmp (&nfm, data->int_neg_format_, 4));
    }

#endif   // _RWSTD_NO_WCHAR_T

}


extern "C" {

static void*
thread_func (void*)
{
    for (int i = 0; i != rw_opt_nloops; ++i) {

        thread_loop_body (std::size_t (i));
    }

    return 0;
}

}   // extern "C"

/**************************************************************************/

static int
run_test (int, char**)
{
    // find all installed locales for which setlocale(LC_ALL) succeeds
    const char* const locale_list =
        rw_opt_locales ? rw_opt_locales : rw_locales (_RWSTD_LC_ALL);

    // array of locale names to use for testing
    const char* locales [sizeof punct_data / sizeof *punct_data];

    const std::size_t maxinx = sizeof locales / sizeof *locales;

    // iterate over locales, initializing a global punct_data array
    for (const char *name = locale_list; *name; name += std::strlen (name) +1) {

        std::locale loc;

        MoneypunctData* const pdata = punct_data + nlocales;

        pdata->locale_name_ = name;
        locales [nlocales]  = name;

        try {
            loc = std::locale (name);

            typedef std::moneypunct<char, false> Punct;

            const Punct &mp = std::use_facet<Punct>(loc);

            const char           dp  = mp.decimal_point ();
            const char           ts  = mp.thousands_sep ();
            const std::string    grp = mp.grouping ();
            const std::string    cur = mp.curr_symbol ();
            const std::string    pos = mp.positive_sign ();
            const std::string    neg = mp.negative_sign ();
            const int            fd  = mp.frac_digits ();
            const Punct::pattern pfm = mp.pos_format ();
            const Punct::pattern nfm = mp.neg_format ();

            pdata->decimal_point_ = dp;
            pdata->thousands_sep_ = ts;
            pdata->frac_digits_   = fd;

            std::strcpy (pdata->grouping_, grp.c_str ());
            std::strcpy (pdata->curr_symbol_, cur.c_str ());
            std::strcpy (pdata->positive_sign_, pos.c_str ());
            std::strcpy (pdata->negative_sign_, neg.c_str ());
            std::memcpy (pdata->pos_format_, &pfm, sizeof pfm);
            std::memcpy (pdata->neg_format_, &nfm, sizeof nfm);
        }
        catch (...) {
            rw_warn (0, 0, __LINE__,
                     "std::locale(%#s) threw an exception, skipping", name);
            continue;
        }

        try {
            typedef std::moneypunct<char, true> Punct;

            const Punct &mp = std::use_facet<Punct>(loc);

            const std::string    cur = mp.curr_symbol ();
            const int            fd  = mp.frac_digits ();
            const Punct::pattern pfm = mp.pos_format ();
            const Punct::pattern nfm = mp.neg_format ();

            pdata->int_frac_digits_ = fd;

            std::strcpy (pdata->int_curr_symbol_, cur.c_str ());
            std::memcpy (pdata->int_pos_format_, &pfm, sizeof pfm);
            std::memcpy (pdata->int_neg_format_, &nfm, sizeof nfm);
        }
        catch (...) {
            rw_warn (0, 0, __LINE__,
                     "std::locale(%#s) threw an exception, skipping", name);
            continue;
        }

#ifndef _RWSTD_NO_WCHAR_T

        try {
            typedef std::moneypunct<wchar_t, false> Punct;

            const Punct &mp = std::use_facet<Punct>(loc);

            const wchar_t      dp  = mp.decimal_point ();
            const wchar_t      ts  = mp.thousands_sep ();
            const std::wstring cur = mp.curr_symbol ();
            const std::wstring pos = mp.positive_sign ();
            const std::wstring neg = mp.negative_sign ();

            pdata->wdecimal_point_ = dp;
            pdata->wthousands_sep_ = ts;

            typedef std::wstring::traits_type Traits;

            Traits::copy (pdata->wcurr_symbol_,   cur.data (), cur.size ());
            Traits::copy (pdata->wpositive_sign_, pos.data (), pos.size ());
            Traits::copy (pdata->wnegative_sign_, neg.data (), neg.size ());
        }
        catch (...) {
            rw_warn (0, 0, __LINE__,
                     "std::locale(%#s) threw an exception, skipping", name);
            continue;
        }

        try {
            typedef std::moneypunct<wchar_t, true> Punct;

            const Punct &mp = std::use_facet<Punct>(loc);

            const std::wstring cur = mp.curr_symbol ();
            const std::wstring pos = mp.positive_sign ();
            const std::wstring neg = mp.negative_sign ();

            typedef std::wstring::traits_type Traits;

            Traits::copy (pdata->wint_curr_symbol_, cur.data (), cur.size ());
        }
        catch (...) {
            rw_warn (0, 0, __LINE__,
                     "std::locale(%#s) threw an exception, skipping", name);
            continue;
        }

#endif   // _RWSTD_NO_WCHAR_T

        ++nlocales;

        if (nlocales == maxinx)
            break;
    }

    // unless the number of iterations was explicitly specified
    // on the command line, decrease the number to equal the number
    // of excericsed locales when only one thread is being tested
    if (1 == rw_opt_nthreads && rw_opt_nloops < 0)
        rw_opt_nloops = int (nlocales);

    // when the number of iterations wasn't explicitly specified
    // on the command line set it to the default value
    if (rw_opt_nloops < 0)
        rw_opt_nloops = DFLT_LOOPS;

    rw_fatal (0 < nlocales, 0, __LINE__,
              "must have at least one valid locale to test");

    rw_info (0, 0, 0,
             "testing std::moneypunct<charT> with %d thread%{?}s%{;}, "
             "%d iteration%{?}s%{;} each, in %zu locales { %{ .*A@} }",
             rw_opt_nthreads, 1 != rw_opt_nthreads,
             rw_opt_nloops, 1 != rw_opt_nloops,
             nlocales, int (nlocales), "%#s", locales);

    rw_info (0, 0, 0, "exercising std::moneypunct<char>");

    test_char  = true;
    test_wchar = false;

    // create and start a pool of threads and wait for them to finish
    int result =
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

#ifndef _RWSTD_NO_WCHAR_T

    rw_info (0, 0, 0, "exercising std::moneypunct<wchar_t>");

    test_char  = false;
    test_wchar = true;

    // start a pool of threads to exercise the thread safety
    // of the wchar_t specialization
    result =
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

    // exercise both the char and the wchar_t specializations
    // at the same time

    rw_info (0, 0, 0,
             "exercising both std::moneypunct<char> "
             "and std::moneypunct<wchar_t>");

    test_char  = true;
    test_wchar = true;

    // start a pool of threads to exercise wstring thread safety
    result =
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

#endif   // _RWSTD_NO_WCHAR_T

    return result;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
#ifdef _RWSTD_REENTRANT

    // set nthreads to the greater of the number of processors
    // and 2 (for uniprocessor systems) by default
    rw_opt_nthreads = rw_get_cpus ();
    if (rw_opt_nthreads < 2)
        rw_opt_nthreads = 2;

#endif   // _RWSTD_REENTRANT

    return rw_test (argc, argv, __FILE__,
                    "lib.locale.moneypunct",
                    "thread safety", run_test,
                    "|-nloops#0 "       // must be non-negative
                    "|-nthreads#0-* "   // must be in [0, MAX_THREADS]
                    "|-locales=",       // must be provided
                    &rw_opt_nloops,
                    int (MAX_THREADS),
                    &rw_opt_nthreads,
                    &rw_opt_setlocales);
}
