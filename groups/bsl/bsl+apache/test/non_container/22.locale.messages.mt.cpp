/************************************************************************
 *
 * 22.locale.messages.mt.cpp
 *
 * test exercising the thread safety of the messages facet
 *
 * $Id: 22.locale.messages.mt.cpp 648752 2008-04-16 17:01:56Z faridz $
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

#include <locale>

#include <driver.h>      // for rw_test()
#include <rw_locale.h>   // for rw_create_catalog()
#include <rw_thread.h>
#include <valcmp.h>      // for rw_strncmp ()

#include <cstring>   // for strlen()
#include <cstdio>    // for remove()

// maximum number of threads allowed by the command line interface
#define MAX_THREADS      32
#define MAX_LOOPS    100000

// default number of threads (will be adjusted to the number
// of processors/cores later)
int rw_opt_nthreads = 1;

// the number of times each thread should iterate (unless specified
// otherwise on the command line)
int rw_opt_nloops = 100000;

// locale for threads to share
static const
std::locale locale;

// message catalog for threads to share
static
std::messages_base::catalog catalog;

static
std::messages_base::catalog wcatalog;

/**************************************************************************/

#ifndef _WIN32
#  define CAT_NAME "./rwstdmessages.cat"
#  define MSG_NAME "rwstdmessages.msg"
#else
#  define CAT_NAME "rwstdmessages.dll"
#  define MSG_NAME "rwstdmessages.rc"
#endif

#define MAX_SETS 5
#define MAX_MESSAGES  5


int msg_id (int set, int id)
{
#ifdef _WIN32

    return (set - 1) * 5 + id;

#else

    _RWSTD_UNUSED (set);
    return id;

#endif
}

/**************************************************************************/

static const char* const
messages [MAX_SETS][MAX_MESSAGES] = {
    { "First set, first message",
      "First set, second message",
      "First set, third message",
      "First set, fourth message",
      "First set, fifth message"
    },
    { "Second set, first message",
      "Second set, second message",
      "Second set, third message",
      "Second set, fourth message",
      "Second set, fifth message"
    },
    { "Third set, first message",
      "Third set, second message",
      "Third set, third message",
      "Third set, fourth message",
      "Third set, fifth message"
    },
    { "Fourth set, first message",
      "Fourth set, second message",
      "Fourth set, third message",
      "Fourth set, fourth message",
      "Fourth set, fifth message"
    },
    { "Fifth set, first message",
      "Fifth set, second message",
      "Fifth set, third message",
      "Fifth set, fourth message",
      "Fifth set, fifth message"
    }
};

static std::string str_messages;

/**************************************************************************/

template <class T>
void test_open_close (const std::locale& loc,
                      const std::messages<T>& msgs,
                      const std::string& name)
{
    std::messages_base::catalog cat =
        (msgs.open) (name, loc);

    RW_ASSERT (! (cat < 0));

    (msgs.close) (cat);
}

template <class T>
void test_get (const std::messages<T>& msgs,
               const std::messages_base::catalog cat,
               int set, int msgid,
               const std::basic_string<T>& dflt)
{
    // the msg_id() thing seems like a bug to me. if anything, the user
    // should never need to write or call msg_id().

    const typename std::messages<T>::string_type res =
        msgs.get (cat, set, msg_id (set, msgid), dflt);

    RW_ASSERT (!rw_strncmp (messages [set-1][msgid-1], res.c_str ()));
}

/**************************************************************************/

extern "C" {

bool test_char;    // exercise messages<char>
bool test_wchar;   // exercise messages<wchar_t>

static void*
thread_func (void*)
{
    const std::string name (CAT_NAME);

    const std::messages<char>& nmsgs =
        std::use_facet<std::messages<char> >(locale);

    const std::string ndflt ("\1\2\3\4");

#ifndef _RWSTD_NO_WCHAR_T
    const std::messages<wchar_t>& wmsgs =
        std::use_facet<std::messages<wchar_t> >(locale);

    const std::wstring wdflt (L"\1\2\3\4");
#endif // _RWSTD_NO_WCHAR_T

    for (int i = 0; i != rw_opt_nloops; ++i) {

        int set   = 1 + i % MAX_SETS;
        int msgid = 1 + i % MAX_MESSAGES;

        if (test_char) {
#if TEST_RW_EXTENSIONS
    // Mesages catalog not implemented in stlport and not required by standard.
            if (i & 1) {
                test_get<char>(nmsgs, catalog, set, msgid, ndflt);
            }
            else {
                test_open_close<char>(locale, nmsgs, name);
            }
#endif
        }

        if (test_wchar) {

#ifndef _RWSTD_NO_WCHAR_T

#if TEST_RW_EXTENSIONS
    // Mesages catalog not implemented in stlport and not required by standard.
            if (i & 1) {
                test_get<wchar_t>(wmsgs, wcatalog, set, msgid, wdflt);
            }
            else {
                test_open_close<wchar_t>(locale, wmsgs, name);
            }
#endif

#endif // _RWSTD_NO_WCHAR_T

        }
    }

    return 0;
}

}   // extern "C"

/**************************************************************************/

static int
run_test (int, char**)
{
    for (int i = 0; i < MAX_SETS; ++i) {
        for (int j = 0; j < MAX_MESSAGES; ++j)
            str_messages.append (messages [i][j], std::strlen (messages [i][j]) + 1);

        str_messages.append (1, '\0');
    }

    // generate a message catalog
    rw_create_catalog (MSG_NAME, str_messages.c_str ());
    const std::string name (CAT_NAME);

    const std::messages<char>& nmsgs =
        std::use_facet<std::messages<char> >(locale);

    catalog = (nmsgs.open) (name, locale);

#ifndef _RWSTD_NO_WCHAR_T

    const std::messages<wchar_t>& wmsgs =
        std::use_facet<std::messages<wchar_t> >(locale);

    wcatalog = (wmsgs.open) (name, locale);

#endif

    rw_info (0, 0, 0,
             "testing std::messages<charT> with %d thread%{?}s%{;}, "
             "%d iteration%{?}s%{;} each",
             rw_opt_nthreads, 1 != rw_opt_nthreads,
             rw_opt_nloops, 1 != rw_opt_nloops);

    ///////////////////////////////////////////////////////////////////////

    test_char   = true;
    test_wchar  = false;

    rw_info (0, 0, 0, "exercising std::messages<char>");

    int result = 
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0,
                        thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

    ///////////////////////////////////////////////////////////////////////

#ifndef _RWSTD_NO_WCHAR_T

    test_char   = false;
    test_wchar  = true;

    rw_info (0, 0, 0, "exercising std::messages<wchar_t>");

    result = 
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0,
                        thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

    ///////////////////////////////////////////////////////////////////////

    test_char   = true;
    test_wchar  = true;

    rw_info (0, 0, 0, "exercising std::messages<char> and "
                      "std::messages<wchar_t>");

    result = 
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0,
                        thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

#endif // _RWSTD_NO_WCHAR_T

    ///////////////////////////////////////////////////////////////////////

    (nmsgs.close) (catalog);

#ifndef _RWSTD_NO_WCHAR_T

    (wmsgs.close) (wcatalog);
    
#endif // _RWSTD_NO_WCHAR_T

    std::remove (CAT_NAME);

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
                    "lib.locale.messages",
                    "thread safety", run_test,
                    "|-nloops#0 "        // must be non-negative
                    "|-nthreads#0-* ",    // must be in [0, MAX_THREADS]
                    &rw_opt_nloops,
                    int (MAX_THREADS),
                    &rw_opt_nthreads);
}
