/************************************************************************
 *
 * 22.locale.codecvt.mt.cpp
 *
 * test exercising the thread safety of the codecvt facet
 *
 * $Id: 22.locale.codecvt.mt.cpp 648752 2008-04-16 17:01:56Z faridz $
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
#include <iterator>   // for ostreambuf_iterator
#include <locale>     // for locale, codecvt

#include <cstring>    // for strlen()
#include <cwchar>     // for codecvt

#include <rw_locale.h>
#include <rw_thread.h>
#include <driver.h>
#include <valcmp.h>    // for rw_strncmp ()


// maximum number of threads allowed by the command line interface
#define MAX_THREADS      32
#define MAX_LOOPS    100000

// default number of threads (will be adjusted to the number
// of processors/cores later)
int rw_opt_nthreads = 1;

// the number of times each thread should iterate (unless specified
// otherwise on the command line)
int rw_opt_nloops = 5000;

// number of locales to use
int rw_opt_nlocales = MAX_THREADS;

// should all threads share the same set of locale objects instead
// of creating their own?
int rw_opt_shared_locale;

/**************************************************************************/

// array of locale names to use for testing
static const char*
locales [MAX_THREADS];

// number of locale names in the array
static std::size_t
nlocales;

/**************************************************************************/

//
template <class internT> //, class externT = char, class stateT = std::mbstate_t>
struct MyCodecvtData_T
{
    enum { BufferSize = 16 };

    typedef char externT;
    typedef std::mbstate_t stateT;
    typedef std::codecvt_base::result resultT;
    typedef std::size_t sizeT;

    int encoding_;
    sizeT length_;
    int max_length_;
    bool always_noconv_;

    externT out_buffer_ [BufferSize];
    resultT out_result_;
    sizeT out_length_;
    stateT out_state_;

    internT in_buffer_  [BufferSize];
    resultT in_result_;
    sizeT in_length_;
    stateT in_state_;
};

//
struct MyCodecvtData
{
    // out, unshift, in, encoding, always_noconv, length, max_length
    enum CvtId {
        cvt_out,
        cvt_unshift,
        cvt_in,
        cvt_encoding,
        cvt_length,
        cvt_max_length,
        cvt_always_noconv,
        cvt_max
    };

    // name of the locale the data corresponds to
    const char* locale_name_;

    // optionally set to the named locale for threads to share
    std::locale locale_;

    MyCodecvtData_T<char> char_data_;
    MyCodecvtData_T<wchar_t> wchar_data_;

} my_codecvt_data [MAX_THREADS];

template <class charT>
struct MyBuffer {
    const charT* const str;
    const int str_len;
};

const MyBuffer<char> nsrc [] = {
    { "a\x80",       2 },
    { "b",           1 },
    { "c\0c",        3 },
    { "ddd",         3 },
    { "e\fce\0",     4 },
    { "ff\0ffff",    5 },
    { "gggg\0g",     6 },
    { "hh\0hhhh",    7 },
    { "i\ni\tiiii",  8 },
    { "jjjjjjjjj",   9 },
    { "kkkkkkkkkk", 10 }
};

const MyBuffer<wchar_t> wsrc [] = {
    { L"\x0905\x0916", 2 },
    { L"bb",           2 },
    { L"\x106c",       2 },
    { L"dddd",         4 },
    { L"\xd800\xd801", 2 },
    { L"ffffff",       6 },
    { L"\xdfff\xffff", 2 },
    { L"hhhhhhhh",     8 },
    { L"i\0i\1i",      4 },
    { L"jjjjjjjjjj",  10 },
    { L"kkkkkkkkkkk", 11 }
};

/**************************************************************************/

template <typename T>
struct test_is_char
{
    enum { value = 0 };
};

template <>
struct test_is_char<char>
{
    enum { value = 1 };
};

/**************************************************************************/

template <class internT>
void test_codecvt (const std::locale& loc,
                   const MyBuffer<internT>& in,
                   const MyBuffer<char>& out,
                   const MyCodecvtData_T<internT>& data,
                   const MyCodecvtData::CvtId id)
{

    typedef char externT;
    typedef std::mbstate_t stateT;
    typedef std::size_t sizeT;

    typedef std::codecvt<internT, externT, stateT> code_cvt_type;

    const code_cvt_type& cvt =
        std::use_facet<code_cvt_type>(loc);

    switch (id) {
    case MyCodecvtData::cvt_out:
    {
        externT out_buffer [MyCodecvtData_T<internT>::BufferSize];
        out_buffer [0] = externT ();

        const int out_len = RW_COUNT_OF (out_buffer);

        const internT* from      = in.str;
        const internT* from_end  = in.str + in.str_len;
        const internT* from_next = 0;

        externT* to       = out_buffer;
        externT* to_limit = out_buffer + out_len;
        externT* to_next  = 0;

        std::mbstate_t state = std::mbstate_t ();

        const typename MyCodecvtData_T<internT>::resultT result =
            cvt.out (state, from, from_end, from_next,
                     to, to_limit, to_next);

        const sizeT len = to_next - to;

        RW_ASSERT (data.out_result_ == result);
        RW_ASSERT (len == data.out_length_);
        RW_ASSERT (!rw_strncmp (out_buffer, data.out_buffer_, len));
    }
        break;
    case MyCodecvtData::cvt_unshift:
    {
        externT out_buffer [MyCodecvtData_T<internT>::BufferSize];
        out_buffer [0] = externT ();

        const int out_len = RW_COUNT_OF (out_buffer);

        externT* to       = out_buffer;
        externT* to_limit = out_buffer + out_len;
        externT* to_next  = 0;

        std::mbstate_t state = std::mbstate_t ();

        const typename MyCodecvtData_T<internT>::resultT result =
            cvt.unshift (state, to, to_limit, to_next);

        // 22.2.1.5.2 p5
        RW_ASSERT (to == to_next);

        // 22.2.1.5.2 p6 required only for codecvt<char, char, mbstate_t>
        RW_ASSERT (  !test_is_char<internT>::value
                   || result == std::codecvt_base::noconv);
    }
        break;
    case MyCodecvtData::cvt_in:
    {
        internT in_buffer [MyCodecvtData_T<internT>::BufferSize];
        in_buffer [0] = internT ();

        const int in_len  = RW_COUNT_OF (in_buffer);
        const int out_len = RW_COUNT_OF (data.out_buffer_);

        const externT* from      = data.out_buffer_;
        const externT* from_end  = data.out_buffer_ + out_len;
        const externT* from_next = 0;

        internT* to       = in_buffer;
        internT* to_limit = in_buffer + in_len;
        internT* to_next  = 0;

        std::mbstate_t state = std::mbstate_t ();

        const typename MyCodecvtData_T<internT>::resultT result =
            cvt.in (state, from, from_end, from_next,
                    to, to_limit, to_next);

        const sizeT len = to_next - to;

        RW_ASSERT (data.in_result_ == result);
        RW_ASSERT (len == data.in_length_);
        RW_ASSERT (!rw_strncmp (in_buffer, data.in_buffer_, len));
    }
        break;
    case MyCodecvtData::cvt_encoding:
        RW_ASSERT (data.encoding_ == cvt.encoding ());
        break;

    case MyCodecvtData::cvt_length:
    {
        const externT* from = out.str;
        const externT* from_end  = out.str + out.str_len;
        const size_t   max  = 32;

        std::mbstate_t state = std::mbstate_t ();

        const sizeT len = cvt.length (state, from, from_end, max);

        RW_ASSERT (data.length_ == len);
    }
        break;

    case MyCodecvtData::cvt_max_length:
        RW_ASSERT (data.max_length_ == cvt.max_length ());
        break;
    case MyCodecvtData::cvt_always_noconv:
        RW_ASSERT (data.always_noconv_ == cvt.always_noconv ());
        break;
    case MyCodecvtData::cvt_max:
        break;
    }
}

/**************************************************************************/

extern "C" {

bool test_char;    // exercise codecvt<char,char>
bool test_wchar;   // exercise codecvt<wchar_t,char>

static void*
thread_func (void*)
{
    const int ni = RW_COUNT_OF (nsrc);
    const int wi = RW_COUNT_OF (wsrc);

    for (int i = 0; i != rw_opt_nloops; ++i) {

        const int inx = i % nlocales;
        const MyCodecvtData& data = my_codecvt_data [inx];

        // construct a named locale, get a reference to the codecvt
        // facet from it and use it to format a random money value
        const std::locale loc =
            rw_opt_shared_locale ? data.locale_
                                 : std::locale (data.locale_name_);

        const MyCodecvtData::CvtId op =
            MyCodecvtData::CvtId (i % MyCodecvtData::cvt_max);

        if (test_char) {
            test_codecvt<char>(loc, nsrc [inx % ni], nsrc [inx % ni],
                               data.char_data_, op);
        }

        if (test_wchar) {
            test_codecvt<wchar_t>(loc, wsrc [inx % wi], nsrc [inx % ni],
                                  data.wchar_data_, op);
        }
    }

    return 0;
}

}   // extern "C"

/**************************************************************************/

template <class internT>
void run_test_fill (const std::locale& loc,
                    const MyBuffer<internT>& in,
                    const MyBuffer<char>& out,
                    MyCodecvtData_T<internT>& data)
{

    typedef char externT;
    typedef std::mbstate_t stateT;

    typedef std::codecvt<internT, externT, stateT> code_cvt_type;

    const code_cvt_type& cvt =
        std::use_facet<code_cvt_type>(loc);

    //fill in the appropriate fields of data
    data.encoding_      = cvt.encoding ();
    data.max_length_    = cvt.max_length ();
    data.always_noconv_ = cvt.always_noconv ();

    // length
    {
        const externT* from = out.str;
        const externT* from_end  = out.str + out.str_len;
        const size_t   max  = 32;

        std::mbstate_t state = std::mbstate_t ();

        data.length_ = cvt.length (state, from, from_end, max);
    }

    // out
    {
        const int out_len = RW_COUNT_OF (data.out_buffer_);

        const internT* from      = in.str;
        const internT* from_end  = in.str + in.str_len;
        const internT* from_next = 0;

        externT* to       = data.out_buffer_;
        externT* to_limit = data.out_buffer_ + out_len;
        externT* to_next  = 0;

        data.out_result_ = cvt.out (data.out_state_,
                                    from, from_end, from_next,
                                    to, to_limit, to_next);

        data.out_length_ = to_next - to;
    }

    //// unshift
    //{
    //    externT* to       = data.out_buffer_;
    //    externT* to_limit = data.out_buffer_ + RW_COUNT_OF (data.out_buffer_);
    //    externT* to_next  = 0;

    //    std::mbstate_t state = std::mbstate_t ();

    //    data.shift_result_ = cvt.unshift (state, to, to_limit, to_next);
    //}

    // in
    {
        const int out_len = RW_COUNT_OF (data.out_buffer_);
        const int in_len  = RW_COUNT_OF (data.in_buffer_);

        const externT* from      = data.out_buffer_;
        const externT* from_end  = data.out_buffer_ + out_len;
        const externT* from_next = 0;

        internT* to       = data.in_buffer_;
        internT* to_limit = data.in_buffer_ + in_len;
        internT* to_next  = 0;

        data.in_result_ = cvt.in (data.in_state_,
                                  from, from_end, from_next,
                                  to, to_limit, to_next);

        data.in_length_ = to_next - to;
    }
}

static int
run_test (int, char**)
{
    // find all installed locales for which setlocale (LC_ALL) succeeds
    const char* const locale_list =
        rw_opt_locales ? rw_opt_locales : rw_locales (_RWSTD_LC_ALL);

    const std::size_t maxinx = RW_COUNT_OF (locales);

    const int ni = RW_COUNT_OF (nsrc);
    const int wi = RW_COUNT_OF (wsrc);

    for (const char* name = locale_list;
         *name;
         name += std::strlen (name) + 1) {

        const std::size_t inx = nlocales;
        locales [inx] = name;

        // fill in the money and results for this locale
        MyCodecvtData& data = my_codecvt_data [inx];
        data.locale_name_ = name;

        try {
            const std::locale loc (data.locale_name_);

            run_test_fill<char>
                (loc, nsrc [inx % ni], nsrc [inx % ni], data.char_data_);

            run_test_fill<wchar_t>
                (loc, wsrc [inx % wi], nsrc [inx % ni], data.wchar_data_);

            if (rw_opt_shared_locale)
                data.locale_ = loc;

            nlocales += 1;

        }
        catch (...) {
            rw_warn (!rw_opt_locales, 0, __LINE__,
                     "failed to create locale(%#s)", name);
        }

        if (nlocales == maxinx || nlocales == std::size_t (rw_opt_nlocales))
            break;
    }

    // avoid divide by zero in thread if there are no locales to test
    rw_fatal (nlocales != 0, 0, __LINE__,
              "failed to create one or more usable locales!");

    rw_info (0, 0, 0,
             "testing std::codecvt<charT> with %d thread%{?}s%{;}, "
             "%d iteration%{?}s%{;} each, in %zu locales { %{ .*A@} }",
             rw_opt_nthreads, 1 != rw_opt_nthreads,
             rw_opt_nloops, 1 != rw_opt_nloops,
             nlocales, int (nlocales), "%#s", locales);

    ///////////////////////////////////////////////////////////////////////

    rw_info (0, 0, 0, "exercising std::codecvt<char, char>");

    test_char   = true;
    test_wchar  = false;

    // create and start a pool of threads and wait for them to finish
    int result = 
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

    ///////////////////////////////////////////////////////////////////////

    rw_info (0, 0, 0, "exercising std::codecvt<wchar_t, char>");

    test_char   = false;
    test_wchar  = true;

    result = 
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

    ///////////////////////////////////////////////////////////////////////

    rw_info (0, 0, 0, "exercising std::codecvt<char, char> and "
                      "std::codecvt<wchar_t, char>");

    test_char   = true;
    test_wchar  = true;

    result = 
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

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
                    "lib.locale.codecvt",
                    "thread safety", run_test,
                    "|-nloops#0 "        // must be non-negative
                    "|-nthreads#0-* "    // must be in [0, MAX_THREADS]
                    "|-nlocales#0 "      // arg must be non-negative
                    "|-locales= "        // must be provided
                    "|-shared-locale# ",
                    &rw_opt_nloops,
                    int (MAX_THREADS),
                    &rw_opt_nthreads,
                    &rw_opt_nlocales,
                    &rw_opt_setlocales,
                    &rw_opt_shared_locale);
}
