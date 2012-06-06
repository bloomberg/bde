/***************************************************************************
 *
 * 22.locale.messages.cpp - tests exercising the std::messages facet
 *
 * $Id: 22.locale.messages.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2001-2006 Rogue Wave Software.
 *
 **************************************************************************/

#include <locale>

#include <driver.h>      // for rw_test()
#include <environ.h>     // for rw_putenv()
#include <file.h>        // for rw_nextfd()
#include <rw_locale.h>   // for rw_locales(), rw_create_catalog()
#include <rw_process.h>  // for rw_system()

#include <cstring>   // for strlen()
#include <cstdlib>   // for getenv()
#include <cstdio>    // for remove()
#include <clocale>   // for LC_ALL

#include <cwchar>    // for mbsinit()

#ifndef _RWSTD_NO_NEW_HEADER
#  if defined (_MSC_VER)
#    include <io.h>          // for _open()
#    include <direct.h>      // for getcwd()
#  else
#    include <sys/types.h>
#    include <sys/stat.h>
#    include <unistd.h>      // for getcwd()
#  endif
#  include <fcntl.h>         // for mode flags for _open
#endif   // _RWSTD_NO_NEW_HEADER

#undef open
#undef close

#if defined (_MSC_VER)
#  define open(f,m) _open  (f, _##m)
#  define close(f)  _close (f)
#else
#  define open(f,m) open  (f, m)
#  define close(f)  close (f)
#endif // defined (_MSC_VER)

/***************************************************************************/

template <class charT>
struct CodeCvt: std::codecvt<charT, char, std::mbstate_t>
{
    typedef std::codecvt<charT, char, std::mbstate_t> Base;
    typedef typename Base::extern_type                extern_type;
    typedef typename Base::intern_type                intern_type;
    typedef typename Base::state_type                 state_type;
    typedef typename Base::result                     result;

    static bool used_;
    static bool partial_;
    static bool noconv_;
    static bool check_state_;

    static bool valid_state_;

    CodeCvt (std::size_t ref = 1U)
        : Base (ref) {
        used_ = partial_ = noconv_ = check_state_ = valid_state_ = false;
    }

protected:

    // override base class virtual function to make the messages
    // facet use codecvt<char, char>::in() (the base returns true)
    bool do_always_noconv () const _THROWS (()) {
        return false;
    }

    result
    do_in (state_type         &state,
           const extern_type  *from,
           const extern_type  *from_end,
           const extern_type *&from_next,
           intern_type        *to,
           intern_type        *to_end,
           intern_type       *&to_next) const {

        used_ = true;

#ifndef _RWSTD_NO_MBSINIT

        if (check_state_)
            valid_state_ = std::mbsinit (&state) != 0;

#else   // if (_RWSTD_NO_MBSINIT)

        if (check_state_)
            valid_state_ = true;

#endif   // _RWSTD_NO_MBSINIT

        const result res =
            Base::do_in (state, from, from_end, from_next,
                         to, to_end, to_next);

        if (partial_)
            return std::codecvt_base::partial;

        if (noconv_)
            return std::codecvt_base::noconv;

        return res;
    }
};

template <class charT>
bool CodeCvt<charT>::used_;
template <class charT>
bool CodeCvt<charT>::partial_;
template <class charT>
bool CodeCvt<charT>::noconv_;
template <class charT>
bool CodeCvt<charT>::check_state_;
template <class charT>
bool CodeCvt<charT>::valid_state_;

/***************************************************************************/

// finds the first named locale other than "C" or "POSIX"; returns
// a pointer to the name of the locale on success, 0 on failure
const char*
find_named_locale ()
{
    const char *name = rw_locales (LC_ALL, "");

    if (!name)
        return 0;

    char namebuf [256];

    // get the full name of the "C" locale for comparison with aliases
    const char* const classic_name =
        std::strcpy (namebuf, std::setlocale (LC_ALL, "C"));

    do {
        if (std::strcmp (name, "C") && std::strcmp (name, "POSIX")) {

            // skip over any aliases of the "C" and "POSIX" locales
            const char* const fullname = std::setlocale (LC_ALL, name);
            if (std::strcmp (fullname, classic_name)) {

                // skip over the C and POSIX locales
                _TRY {
                    const std::locale loc (name);
                    _RWSTD_UNUSED (loc);

                    std::setlocale (LC_ALL, "C");
                    return name;
                }
                _CATCH (...) { }
            }
        }
        name += std::strlen (name) + 1;

    } while (*name);

    std::setlocale (LC_ALL, "C");

    return 0;
}

/***************************************************************************/

#ifndef _WIN32
#  define CAT_NAME "./rwstdmessages.cat"
#  define MSG_NAME "rwstdmessages.msg"
#else
#  define CAT_NAME "rwstdmessages.dll"
#  define MSG_NAME "rwstdmessages.rc"
#endif

#define NLS_CAT_NAME "rwstdmessages"

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

/***************************************************************************/

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

static std::string catalog;

/***************************************************************************/


template <class charT>
std::basic_string<charT> widen (std::string message)
{
    std::basic_string<charT> result;
    for (std::size_t i = 0; i < message.length(); i++) {
        result.push_back ((charT)message[i]);
    }

    return result;
}

/***************************************************************************/

// Test that the message facet exists in the locale and that
// we can get a reference to it.
template <class charT>
void test_has_facet (const char *loc_name, const char *cname)
{
    // construct a copy of the named locale or default
    // when no name is specified
    const std::locale loc =
        loc_name ? std::locale (loc_name) : std::locale ();

    typedef std::messages<charT> Messages;

    // verify that the facet exists in the locale
    bool facet_exists = std::has_facet<Messages>(loc);

    rw_assert (facet_exists, 0, __LINE__,
               "has_facet<messages<%s> >(locale(%{?}%#s%{;})) == true",
               cname, 0 != loc_name, loc_name);

    try {
        // verify that use facet doesn't throw an exception
        std::use_facet<Messages>(loc);
    }
#ifndef _RWSTD_NO_EXCEPTIONS

    catch (std::exception &ex) {
        rw_assert (0, 0, __LINE__,
                   "use_fact<messages<%s> >(locale(%{?}%#s%{;})) "
                   "unexpectedly threw exception(%#s)",
                   cname, 0 != loc_name, loc_name, ex.what ());
    }
    catch (...) {
        rw_assert (0, 0, __LINE__,
                   "use_fact<messages<%s> >(locale(%{?}%#s%{;})) "
                   "unexpectedly threw an unknown exception",
                   cname, 0 != loc_name, loc_name);
    }

    typedef std::messages_byname<charT> MessagesByname;

    const bool byname = loc_name
        && std::strcmp (loc_name, "C") && std::strcmp (loc_name, "POSIX");

    facet_exists = std::has_facet<MessagesByname>(loc);

    rw_assert (byname == facet_exists, 0, __LINE__,
               "has_fact<messages_byname<%s> >(locale(%{?}%#s%{;})) == %b",
               cname, 0 != loc_name, loc_name);

    try {
        // verify that use facet throws an exception only
        // for the default and "C" locales
        std::use_facet<MessagesByname>(loc);

        rw_assert (byname, 0, __LINE__,
                   "use_fact<messages_byname<%s> >(locale(%{?}%#s%{;})) "
                   "failed to throw",
                   cname, 0 != loc_name, loc_name);
    }
    catch (std::exception &ex) {
        rw_assert (!byname, 0, __LINE__,
                   "use_fact<messages_byname<%s> >(locale(%{?}%#s%{;})) "
                   "unexpectedly threw exception(%#s)",
                   cname, 0 != loc_name, loc_name, ex.what ());
    }
    catch (...) {
        rw_assert (0, 0, __LINE__,
                   "use_fact<messages<%s> >(locale(%{?}%#s%{;})) "
                   "unexpectedly threw an unknown exception",
                   cname, 0 != loc_name, loc_name);
    }

#endif   // _RWSTD_NO_EXCEPTIONS

}

/***************************************************************************/

template <class charT>
std::messages_base::catalog
open_catalog (const std::messages<charT> &msgs,
              const char *cat_name, const std::locale &loc,
              int expect, const char *cname, int line)
{
    std::messages_base::catalog cat = -1;

    // expect is set to:
    //   *   0 (or greater) when the caller expects the call to open()
    //         to succeed
    //   *  -1 when the caller expects the call to open() to fail
    //         by returning -1
    //   *  -2 (or less) when the caller expects the call to open()
    //         to exit by throwing an exception
   const bool expect_success   = expect >= 0;
   const bool expect_failure   = expect == -1;
   const bool expect_exception = expect <  -1;

    const char* const nlspath = std::getenv ("NLSPATH");

    try {
        // an already closed cat should throw an exception
        cat = (msgs.open)(cat_name, loc);

        const bool success =
            expect_success && -1 < cat || expect_failure && cat < 0;

        rw_assert (success, 0, line,
                   "messages<%s>::open(%#s, locale(%#s))"
                   "%{?} > -1%{:}%{?} == -1 %{;}%{;}, got %d"
                   "%{?}; failed to throw an exception%{;}; NLSPATH=%s",
                   cname, cat_name, loc.name ().c_str (),
                   expect_success, expect_failure, cat,
                   expect_exception, nlspath);
    }

#ifndef _RWSTD_NO_EXCEPTIONS

    catch (std::exception &ex) {
        rw_assert (expect_exception, 0, line,
                   "messages<%s>::open(%#s, locale(%#s)) unexpectedly "
                   "threw exception(%#s); NLSPATH=%s",
                   cname, cat_name, loc.name ().c_str (), ex.what (), nlspath);
    }
    catch (...) {
        rw_assert (expect_exception, 0, line,
                   "messages<%s>::open(%#s, locale(%#s)) unexpectedly "
                   "threw an unknown exception; NLSPATH=%s",
                   cname, cat_name, loc.name ().c_str (), nlspath);
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    return cat;
}

/***************************************************************************/

template <class charT>
void
close_catalog (const std::messages<charT> &msgs,
               std::messages_base::catalog cat,
               bool expect_exception,
               const char *cname, int line)
{
    try {
        // an already closed cat should throw an exception
        (msgs.close)(cat);

        rw_assert (!expect_exception, 0, line,
                   "messages<%s>::close(%d) failed "
                   "to throw an expected exception",
                   cname, cat);
    }

#ifndef _RWSTD_NO_EXCEPTIONS

    catch (std::exception &ex) {
        rw_assert (expect_exception, 0, line,
                   "messages<%s>::close(%d) unexpectedly "
                   "threw exception(%#s)",
                   cname, cat, ex.what ());
    }
    catch (...) {
        rw_assert (expect_exception, 0, line,
                   "messages<%s>::close(%d) unexpectedly "
                   "threw an unknown exception",
                   cname, cat);
    }

#endif   // _RWSTD_NO_EXCEPTIONS

}

/***************************************************************************/

template <class charT>
void test_open_close (const char *loc_name, const char *cname)
{
    int fdcount [2];
    int next_fd [2];

#ifndef _WIN32
    next_fd [0] = rw_nextfd (fdcount + 0);
#else
    // don't test file descriptor leaking on Win32 to avoid
    // invalid parameter error
    // catalog functions not uses file descriptors
    next_fd [0] = fdcount [0] = 0;
#endif

    rw_info (0, 0, __LINE__,
             "std::messages<%s>::open() and close() in locale(#%s)",
             cname, loc_name);

    // construct a copy of the named locale or default
    // when no name is specified
    const std::locale loc =
        loc_name ? std::locale (loc_name) : std::locale ();

    const std::messages<charT>& msgs =
        std::use_facet<std::messages<charT> >(loc);

    const std::messages_base::catalog cat =
        open_catalog (msgs, CAT_NAME, loc, 0, cname, __LINE__);

    // close a (presumably successfully) opened catalog
    close_catalog (msgs, cat, cat < 0, cname, __LINE__);

    // an already closed cat should throw an exception
    close_catalog (msgs, cat, true, cname, __LINE__);

    // verify that no file descriptor has leaked
#ifndef _WIN32
    next_fd [1] = rw_nextfd (fdcount + 1);
#else
    next_fd [1] = fdcount [1] = 0;
#endif

    rw_assert (next_fd [1] == next_fd [0] && fdcount [0] == fdcount [1],
               0, __LINE__,
               "std::messages<%s>::close() leaked %d file descriptor(s) "
               "(or descriptor mismatch)",
               cname, fdcount [1] - fdcount [0]);
}

/***************************************************************************/

template <class charT>
void test_get (const char *loc_name,
               const char* const text[5][5],
               const char *cname)
{
    // construct a copy of the named locale or default
    // when no name is specified
    const std::locale loc =
        loc_name ? std::locale (loc_name) : std::locale ();

    const std::messages<charT>& msgs =
        std::use_facet<std::messages<charT> > (loc);

    const charT def[] = {
        'N', 'o', ' ', 'm', 'e', 's', 's', 'a', 'g', 'e', '.', '\0'
    };

    std::messages_base::catalog cat = -1;

    cat = open_catalog (msgs, CAT_NAME, loc, 0, cname, __LINE__);

    if (cat < 0)
        return;

    typedef std::char_traits<charT>                     Traits;
    typedef std::allocator<charT>                       Allocator;
    typedef std::basic_string<charT, Traits, Allocator> String;

    for (int setId = 1; setId <= MAX_SETS; ++setId) {

        for (int msgId = 1; msgId <= MAX_MESSAGES; ++msgId) {

            const int id = msg_id (setId, msgId);
            const String got = msgs.get (cat, setId, id, def);

            rw_assert (got == widen<charT>(text [setId - 1][msgId - 1]),
                       0, __LINE__,
                       "messages<%s>::get(%d, %d, %d, %{*Ac}) == %#s, "
                       "got %{#*S}",
                       cname, cat, setId, id,
                       int (sizeof *def), def,
                       text [setId - 1][msgId - 1],
                       int (sizeof (charT)), &got);
        }
    }

    // bad catalog id
    const std::messages_base::catalog bad_cat = 777;

    try {
        // get() will either throw or return `def'
        // when passed a bad catalog id
        const String got = msgs.get (bad_cat, 1, 1, def);

        rw_assert (def == got, 0, __LINE__,
                   "messages<%s>::get(%d, 1, 1, %{*Ac}) == %{*Ac}, got %{#*S}",
                   cname, bad_cat, int (sizeof *def), def,
                   int (sizeof *def), def, int (sizeof (charT)), &got);
    }
#ifndef _RWSTD_NO_EXCEPTIONS

    catch (std::exception &ex) {
        rw_assert (true, 0, __LINE__,
                   "messages<%s>::get(%d, 1, 1, %{*Ac}) unexpectedly threw "
                   "exception(%#s)", cname, bad_cat, int (sizeof *def), def,
                   ex.what ());
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    // Bad set id
#if !defined (_WIN32) && !defined (_WIN64)
    // When we use resource files for the message catalogs
    // the set ids are ignored.
    rw_assert (msgs.get (cat, 777, 1, def) == def, 0, __LINE__,
               "messages<%s>::get(%d, 777, 1, %{#*Ac}) == %{#*Ac}",
               cname, cat, int (sizeof *def), def, int (sizeof *def), def);
#endif   // !defined (_WIN32) && !defined (_WIN64)

    // Bad message id
    rw_assert (msgs.get (cat, 1, 777, def) == def, 0, __LINE__,
               "messages<%s>::get(%d, 777, 1) == \"\"", cname);

    close_catalog (msgs, cat, false, cname, __LINE__);
}

/***************************************************************************/

template <class charT>
void test_use_codecvt (const char *cname)
{
    CodeCvt<charT> cvt (1);

    const std::locale loc (std::locale::classic (), &cvt);

    const charT def[] = {
        'N', 'o', ' ', 'm', 'e', 's', 's', 'a', 'g', 'e', '.', '\0'
    };

    const std::messages<charT>& msgs =
        std::use_facet <std::messages<charT> >(loc);

    cvt.used_ = false;

    std::messages_base::catalog cat = -1;

    cat = open_catalog (msgs, CAT_NAME, loc, 0, cname, __LINE__);

    if (cat < 0)
        return;

    cvt.check_state_ = true;

    const int msgid = msg_id (1, 1);
    std::basic_string<charT> got = msgs.get (cat, 1, msgid, def);
    rw_assert (cvt.used_, 0, __LINE__,
               "messages<%s>::get(%d, 1, %d, const char_type*) "
               "uses codecvt<%1$s, char>", cname, cat, msgid);

    rw_assert (cvt.valid_state_, 0, __LINE__,
               "messages<%s>::get(%d, 1, %d, const char_type*) "
               "initializes mbstate_t argument", cname, cat, msgid);

    cvt.check_state_ = false;
    cvt.partial_     = true;

    got = msgs.get (cat, 1, msgid, def);
    rw_assert (got == def, 0, __LINE__,
               "messages<%s>::get(%d, 1, %d, %{*Ac}) == %{*Ac}",
               cname, cat, msgid, int (sizeof *def), def,
               int (sizeof *def), def);

    cvt.partial_ = false;
    cvt.noconv_  = true;

    got = msgs.get (cat, 1, msgid, def);
    rw_assert (got == widen<charT>(std::string(messages[0][0])), 0, __LINE__,
               "messages<%s>::get(%d, 1, %d, %{*Ac}) == %#s, got %{#*S}",
               cname, cat, msgid, int (sizeof *def), def,
               messages [0][0], int (sizeof (charT)), &got);

    cvt.noconv_ = false;

    close_catalog (msgs, cat, false, cname, __LINE__);

    cvt.used_ = false;
}

/***************************************************************************/


template <class charT>
void test_use_nls_path (const char *cname)
{
    // get working directory
    char cwd [2048];
    cwd [0] = '\0';
    if (!rw_error (0 != getcwd (cwd, 2048), 0, __LINE__,
                   "getcwd(%#p, %u) failed: %{#m} (%m)", cwd, 2048))
        return;

    char* const nlspath = new char [std::strlen (cwd) + 512];

    // create NLSPATH string
    std::sprintf (nlspath, "NLSPATH=%s/%%N.cat", cwd);
    rw_putenv (nlspath);

    const std::locale loc;

    const std::messages<charT>& msgs =
        std::use_facet <std::messages<charT> >(loc);

    const std::messages_base::catalog cat =
        open_catalog (msgs, NLS_CAT_NAME, loc, 0, "char", __LINE__);

    if (-1 < cat)
        close_catalog (msgs, cat, false, cname, __LINE__);

    delete[] nlspath;
}

/***************************************************************************/


template <class charT>
void stress_test (const char *cname)
{
    // NLSPATH must be defined
    RW_ASSERT (0 != std::getenv ("NLSPATH"));

    char catalog_names [24][24];
    const std::size_t NCATS = sizeof catalog_names / sizeof *catalog_names;

    std::messages_base::catalog cats [NCATS];

    // detect descriptor leaks
    const int fd1 = open (__FILE__, O_RDONLY);

    const std::locale loc = std::locale::classic ();

    const std::messages<charT>& msgs =
        std::use_facet<std::messages<charT> > (loc);

    std::size_t i;

    for (i = 0; i < NCATS; i++) {

        char msg_name [NCATS];

#ifndef _WIN32
        std::sprintf (msg_name, "rwstdmessages_%d.msg", int (i));
#else
        std::sprintf (msg_name, "rwstdmessages_%d.rc", int (i));
#endif

        rw_create_catalog (msg_name, catalog.c_str ());

        const char* const dot = std::strrchr (msg_name, '.');
        std::strncpy (catalog_names[i], msg_name, dot - msg_name);
        *(catalog_names[i] + (dot - msg_name)) = '\0';

        // open each catalog (expect success)
        cats [i] = open_catalog (msgs, catalog_names [i],
                                 loc, 0, cname, __LINE__);
    }

    // close smallest first and check for descriptor leaks
    for (i = 0; i < NCATS; ++i) {
        if (-1 != cats [i])
            close_catalog (msgs, cats [i], false, cname, __LINE__);
    }

    int fd2 = open (__FILE__, O_RDONLY);
    rw_assert (fd2 - fd1 == 1, 0, __LINE__,
               "messages<%s>::close() leaked %d file descriptors",
               cname, fd2 - fd1 - 1);

    // open again, close largest first and check for descriptor leaks
    for (i = 0; i < NCATS; ++i) {
        cats [i] = open_catalog (msgs, catalog_names [i],
                                 loc, 0, cname, __LINE__);
    }

    for (i = NCATS; i-- > 0; ) {
        if (-1 != cats [i])
            close_catalog (msgs, cats [i], false, cname, __LINE__);
    }

    // close again fd2
    close (fd2);

    fd2 = open (__FILE__, O_RDONLY);
    rw_assert (fd2 - fd1 == 1, 0, __LINE__,
               "messages<%s>::close() leaked %d file descriptors",
               cname, fd2 - fd1 - 1);

    close (fd1);
    close (fd2);

    rw_system (SHELL_RM_F "rwstdmessages_*");
}

/**************************************************************************/

int opt_has_facet;
int opt_open_close;
int opt_get;
int opt_codecvt;
int opt_nlspath;
int opt_stress_test;


template <class charT>
void test_messages (charT, const char *cname, const char *locname)
{
    if (rw_note (0 <= opt_has_facet, 0, __LINE__,
                 "has_facet<messages<%s> > tests disabled", cname)) {

        // exercise has_facet and use_facet in the default locale
        test_has_facet<charT>(0, cname);

        // exercise has_facet and use_facet in locale("C")
        test_has_facet<charT>("C", cname);

        // exercise has_facet and use_facet in a named locale
        test_has_facet<charT>(locname, cname);
    }

    if (rw_note (0 <= opt_open_close, 0, __LINE__,
                 "messages<%s>::open() and close() tests disabled", cname)) {
        test_open_close<charT>("C", cname);
        test_open_close<charT>(locname, cname);
    }

    if (rw_note (0 <= opt_has_facet, 0, __LINE__,
                 "messages<%s>::get() tests disabled", cname)) {
        test_get<charT>("C", messages, cname);
        test_get<charT>(locname, messages, cname);
    }

    if (rw_note (0 <= opt_has_facet, 0, __LINE__,
                 "messages<%s> codecvt tests disabled", cname))
        test_use_codecvt<charT>(cname);

    if (rw_note (0 <= opt_nlspath, 0, __LINE__,
                 "messages<%s> NLSPATH tests disabled", cname))
        test_use_nls_path<charT>(cname);

    if (rw_note (0 <= opt_has_facet, 0, __LINE__,
                 "messages<%s> codecvt tests disabled", cname))
        stress_test<charT>(cname);
}

/**************************************************************************/

static int
run_test (int, char*[])
{

#ifdef _RWSTD_OS_AIX

    // must do this so that NLSPATH lookup works correctly for both
    // the C and POSIX locales.
    const int p = rw_putenv ("LC__FASTMSG=false");
    rw_note (!p, 0, __LINE__, "failed to set LC__FASTMSG");

#endif    // _RWSTD_OS_AIX

#ifdef _RWSTD_NO_DYNAMIC_CAST

    // if dynamic_cast isn't supported then has_facet() can't
    // reliably detect if a facet is installed or not.
    rw_warn (0 != opt_has_facet, 0, __LINE__,
             "dynamic_cast not supported, disabling has_facet tests");

    opt_has_facet = -1;

#endif    // _RWSTD_NO_DYNAMIC_CAST


    for (int i = 0; i < MAX_SETS; ++i) {
        for (int j = 0; j < MAX_MESSAGES; ++j)
            catalog.append (messages [i][j], std::strlen (messages [i][j]) + 1);

        catalog.append (1, '\0');
    }

    const char* const locname = find_named_locale ();

    rw_create_catalog (MSG_NAME, catalog.c_str ());

    test_messages (char (), "char", locname);

#ifndef _RWSTD_NO_WCHAR_T

    test_messages (wchar_t (), "wchar_t", locname);

#endif   // _RWSTD_NO_WCHAR_T

    std::remove (CAT_NAME);

    return 0;
}


/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.category.messages",
                    0 /* no comment */,
                    run_test,
                    "|-has_facet~ "
                    "|-open_close~ "
                    "|-get~ "
                    "|-codecvt~ "
                    "|-nlspath~ "
                    "|-stress-test~ ",
                    &opt_has_facet,
                    &opt_open_close,
                    &opt_get,
                    &opt_codecvt,
                    &opt_nlspath,
                    &opt_stress_test,
                    (void*)0   /* sentinel */);
}
