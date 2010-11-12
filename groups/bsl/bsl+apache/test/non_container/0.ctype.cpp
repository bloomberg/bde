/************************************************************************
 *
 * 0.ctype.cpp - test exercising the UserCtype helper class template
 *
 * $Id: 0.ctype.cpp 590052 2007-10-30 12:44:14Z faridz $
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
 * Copyright 2006 Rogue Wave Software.
 *
 **************************************************************************/

#include <rw_char.h>     // for UserChar
#include <rw_ctype.h>    // for UserCtype
#include <rw_printf.h>   // for rw_printf
#include <driver.h>

#include <stdlib.h>      // for free()
#include <string.h>      // for memset(), size_t, strlen()

/***********************************************************************/

extern const char
cntrl[] = {

#if 'A' == 0x41   // ASCII
    "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
    "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\x7f"
#elif 'A' == 0xc1   // EBCDIC
    ""
#endif

};

extern const std::size_t
n_cntrl = sizeof cntrl - 1;

extern const char
digits[] = "0123456789";

extern const std::size_t
n_digits = sizeof digits - 1;

extern const char
graph[] = "!\"#$%&\\()*+,-./0123456789:;<=>?@[]^_`{|}~";

extern const std::size_t
n_graph = sizeof graph - 1;

extern const char
lower[] = "abcdefghijklmnopqrstuvwxyz";

extern const std::size_t
n_lower = sizeof lower - 1;

extern const char
punct[] = "!#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~";

extern const std::size_t
n_punct = sizeof punct - 1;

extern const char
print[] = {
    "!\"#$%&\\()*+,-./0123456789:;<=>?@[]^_`{|}~"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
};

extern const std::size_t
n_print = sizeof print - 1;

extern const char
spaces[] = " \t\v\f\r\n";

extern const std::size_t
n_spaces = sizeof spaces - 1;

extern const char
upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

extern const std::size_t
n_upper = sizeof upper - 1;

extern const char
xdigits[] = "0123456789ABCDEFabcdef";

extern const std::size_t
n_xdigits = sizeof xdigits - 1;

/***********************************************************************/

template <class charT>
void
test_id (charT*, const char *cname)
{
    rw_info (0, 0, __LINE__, "UserCtype<%s>::id", cname);

    std::locale::id* const pid_b = &std::ctype<charT>::id;
    std::locale::id* const pid_d = &UserCtype<charT>::id;

    rw_assert (pid_b == pid_d, 0, __LINE__,
               "&ctype<%s>::id == &UserCtype<%1$s>::id "
               "(%#p == %2$#p, got %#p)",
               cname, pid_b, pid_d);
}

/***********************************************************************/

template <class charT>
void
test_is (charT*, const char *cname,
         std::ctype_base::mask m,
         const char *chars, std::size_t n_chars)
{
    UserCtype<charT> ctp;

    std::size_t n_calls  = ctp.n_calls_ [ctp.mf_is];
    std::size_t n_throws = ctp.n_throws_ [ctp.mf_is];

    std::size_t expect_calls  = 0;
    std::size_t expect_throws = 0;

    for (std::size_t i = 0; i != n_chars; ++i) {
        const charT ch = make_char (chars [i], (charT*)0);

        int result = -1;

#ifndef _RWSTD_NO_EXCEPTIONS

        // let the first call call succeed and trigger an exception
        // on the second call to UserCtype::is()
        ctp.throw_at_calls_ [ctp.mf_is] =
            ctp.n_calls_ [ctp.mf_is] + 2;

        for (int j = 0; j != 2; ++j) {

            if (1 < sizeof (charT)) {
                // can only count UserCtype<charT>::is() calls when
                // charT != char since there is no virtual do_is()
                // in the char explicit specialization
                ++expect_calls;
            }
            else if (j) {
                // cannot induce an exception out of UserCtype<char>::is()
                // since there is no virtual do_is() in the specialization
                break;
            }

            if (j)
                ++expect_throws;

            int threw = 0;

            try {
                result = ctp.is (m, ch);
            }
            catch (...) {
                threw = 1;
            }

            rw_assert (j == threw, 0, __LINE__,
                       "UserCtype<%s>::is(%{LC}, %{#c}) "
                       "%{?}unexpectedly threw%{:}failed to throw%{;}",
                       cname, m, ch, j == threw);
        }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

        ++expect_calls;
        result = ctp.is (m, ch);

#endif   // _RWSTD_NO_EXCEPTIONS

        rw_assert (result, 0, __LINE__,
                   "UserCtype<%s>::is(%{LC}, %{#c}) == true",
                   cname, m, chars [i]);
    }

    n_calls = ctp.n_calls_ [ctp.mf_is] - n_calls;

    rw_assert (expect_calls == n_calls, 0, __LINE__,
               "expected %zu calls to UserCtype<%s>::is(%{LC}, char_type), "
               "got %zu", expect_calls, cname, m, n_calls);

    n_throws = ctp.n_throws_ [ctp.mf_is] - n_throws;

    rw_assert (expect_throws == n_throws, 0, __LINE__,
               "expected %zu exceptions thrown by UserCtype<%s>::is(%{LC}, "
               "char_type), got %zu", expect_throws, cname, m, n_throws);
}

template <class charT>
void
test_is (charT*, const char *cname)
{
    rw_info (0, 0, __LINE__, "UserCtype<%s>::is"
             "(mask, char_type) const", cname);

    //////////////////////////////////////////////////////////////////
    // exercise default behavior

    test_is ((charT*)0, cname, std::ctype_base::alnum,  lower,   n_lower);
    test_is ((charT*)0, cname, std::ctype_base::alnum,  upper,   n_upper);
    test_is ((charT*)0, cname, std::ctype_base::alnum,  digits,  n_digits);
    test_is ((charT*)0, cname, std::ctype_base::alnum,  xdigits, n_xdigits);
    test_is ((charT*)0, cname, std::ctype_base::alpha,  lower,   n_lower);
    test_is ((charT*)0, cname, std::ctype_base::alpha,  upper,   n_upper);
    test_is ((charT*)0, cname, std::ctype_base::cntrl,  cntrl,   n_cntrl);
    test_is ((charT*)0, cname, std::ctype_base::digit,  digits,  n_digits);
    test_is ((charT*)0, cname, std::ctype_base::graph,  graph,   n_graph);
    test_is ((charT*)0, cname, std::ctype_base::lower,  lower,   n_lower);
    test_is ((charT*)0, cname, std::ctype_base::print,  print,   n_print);
    test_is ((charT*)0, cname, std::ctype_base::punct,  punct,   n_punct);
    test_is ((charT*)0, cname, std::ctype_base::space,  spaces,  n_spaces);
    test_is ((charT*)0, cname, std::ctype_base::xdigit, xdigits, n_xdigits);

    //////////////////////////////////////////////////////////////////
    // exercise custom behavior

    static const int chars[] = {
        '0', '1', '2', '3', 'a', 'b', 'c',
        0x100, 0x7fff, 0x8000, 0xffff,
#if 2 < _RWSTD_INT_SIZE
        0x10000, 0x7fffffff, 0x80000000, 0xffffffff,
#else
        0x1ff, 0x700, 0x8fff, 0xf000,
#endif
        -1   // end of chars
    };

    static const int masks[] = {
        /* '0'        */ std::ctype_base::alpha,
        /* '1'        */ std::ctype_base::cntrl,
        /* '2'        */ std::ctype_base::digit,
        /* '3'        */ std::ctype_base::lower,
        /* 'a'        */ std::ctype_base::print,
        /* 'b'        */ std::ctype_base::punct,
        /* 'c'        */ std::ctype_base::space,
        /* 0x100      */ std::ctype_base::upper,
        /* 0x7fff     */ std::ctype_base::xdigit,
        /* 0x8000     */ std::ctype_base::alpha | std::ctype_base::cntrl,
        /* 0xffff     */ std::ctype_base::cntrl | std::ctype_base::digit,
        /* 0x10000    */ std::ctype_base::digit | std::ctype_base::lower,
        /* 0x7fffffff */ std::ctype_base::lower | std::ctype_base::print,
        /* 0x80000000 */ std::ctype_base::print | std::ctype_base::punct,
        /* 0xffffffff */ std::ctype_base::punct | std::ctype_base::space,
        -1            // end of masks
    };

    static const int mask_all = 
          std::ctype_base::alpha
        | std::ctype_base::cntrl
        | std::ctype_base::digit
        | std::ctype_base::lower
        | std::ctype_base::print
        | std::ctype_base::punct
        | std::ctype_base::space
        | std::ctype_base::upper
        | std::ctype_base::xdigit;

    const UserCtype<charT> cust (chars, masks);

    for (std::size_t i = 0; 0 <= chars [i]; ++i) {

        // FIXME: enable tests of characters greater than CHAR_MAX
        if (   _RWSTD_UCHAR_MAX < std::size_t (chars [i])
            /* && sizeof (char) == sizeof (charT) */) {
            // when charT == char break out when the character
            // value is greater than CHAR_MAX
            break;
        }

        const charT ch = make_char (chars [i], (charT*)0);

        const std::ctype_base::mask m_is =
            std::ctype_base::mask (masks [i]);

        rw_assert (cust.is (m_is, ch),
                   0, __LINE__,
                   "UserCtype<%s>::is(%{LC}, %{#lc}) == true",
                   cname, m_is, chars [i]);

        const std::ctype_base::mask m_not =
            std::ctype_base::mask (~m_is & mask_all);

        rw_assert (!cust.is (m_not, ch), 0, __LINE__,
                   "UserCtype<%s>::is(%{LC}, %{#lc}) == false",
                   cname, m_not, chars [i]);
    }
}

/***********************************************************************/

template <class charT>
void
test_scan_is (charT*, const char *cname)
{
    rw_info (0, 0, __LINE__, "UserCtype<%s>::scan_is"
             "(mask, const char_type*, const char_type*) const", cname);

    rw_warn (0, 0, __LINE__, "UserCtype<%s>::scan_is"
             "(mask, const char_type*, const char_type*) const"
             " not exercised", cname);
}

/***********************************************************************/

template <class charT>
void
test_scan_not (charT*, const char *cname)
{
    rw_info (0, 0, __LINE__, "UserCtype<%s>::scan_not"
             "(mask, const char_type*, const char_type*) const", cname);

    rw_warn (0, 0, __LINE__, "UserCtype<%s>::scan_not"
             "(mask, const char_type*, const char_type*) const "
             "not exercised", cname);
}

/***********************************************************************/

template <class charT>
void
test_toupper (charT*, const char *cname)
{
    rw_info (0, 0, __LINE__, "UserCtype<%s>::toupper"
             "(char_type) const", cname);

    rw_warn (0, 0, __LINE__, "UserCtype<%s>::toupper"
             "(char_type) const not exercised", cname);

    rw_info (0, 0, __LINE__, "UserCtype<%s>::toupper"
             "(char_type*, const char_type) const", cname);

    rw_warn (0, 0, __LINE__, "UserCtype<%s>::toupper"
             "(char_type*, const char_type) const not exercised", cname);
}

/***********************************************************************/

template <class charT>
void
test_tolower (charT*, const char *cname)
{
    rw_info (0, 0, __LINE__, "UserCtype<%s>::tolower"
             "(char_type) const", cname);

    rw_warn (0, 0, __LINE__, "UserCtype<%s>::tolower"
             "(char_type) const not exercised", cname);

    rw_info (0, 0, __LINE__, "UserCtype<%s>::tolower"
             "(char_type*, const char_type) const", cname);

    rw_warn (0, 0, __LINE__, "UserCtype<%s>::tolower"
             "(char_type*, const char_type) const not exercised", cname);
}

/***********************************************************************/

template <class charT>
void
test_widen (charT*, const char *cname)
{
    rw_info (0, 0, __LINE__, "UserCtype<%s>::widen"
             "(char) const", cname);

    rw_warn (0, 0, __LINE__, "UserCtype<%s>::widen"
             "(char) const not exercised", cname);

    rw_info (0, 0, __LINE__, "UserCtype<%s>::widen"
             "(const char*, const char*, char_type*) const", cname);

    rw_warn (0, 0, __LINE__, "UserCtype<%s>::widen"
             "(const char*, const char*, char_type*) const not exercised",
             cname);
}

/***********************************************************************/

template <class charT>
void
test_narrow (charT*, const char *cname)
{
    rw_info (0, 0, __LINE__, "UserCtype<%s>::narrow"
             "(char_type, char) const", cname);

    rw_warn (0, 0, __LINE__, "UserCtype<%s>::narrow"
             "(char_type, char) const not exercised", cname);

    rw_info (0, 0, __LINE__, "UserCtype<%s>::narrow"
             "(const char_type*, const char_type*, char, char*) const",
             cname);

    rw_warn (0, 0, __LINE__, "UserCtype<%s>::narrow"
             "(const char_type*, const char_type*, char, char*) const"
             " not exercised",
             cname);
}

/***********************************************************************/

/* extern */ int opt_id;

/* extern */ int opt_is;
/* extern */ int opt_scan_is;
/* extern */ int opt_scan_not;

/* extern */ int opt_toupper;
/* extern */ int opt_tolower;

/* extern */ int opt_widen;
/* extern */ int opt_narrow;


template <class charT>
void
test (charT*, const char *cname)
{
#define TEST(func)                                      \
    if (0 <= opt_ ## func)                              \
        test_ ## func ((charT*)0, cname);               \
    else                                                \
        rw_note (0, 0, __LINE__,                        \
                 "UserCtype<%s>::%s() tests disabled",  \
                 cname, # func)

    TEST (id);

    TEST (is);
    TEST (scan_is);
    TEST (scan_not);

    TEST (toupper);
    TEST (tolower);

    TEST (widen);
    TEST (narrow);
}

/***********************************************************************/

static int
run_test (int, char**)
{
    test ((char*)0, "char");

#ifndef _RWSTD_NO_WCHAR_T
    test ((wchar_t*)0, "wchar_t");
#endif   // _RWSTD_NO_WCHAR_T

    test ((UserChar*)0, "UserChar");

    return 0;
}

/***********************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "",
                    0,
                    run_test,
                    "|-id~ "
                    "|-is~ "
                    "|-scan_is~ "
                    "|-scan_not~ "
                    "|-toupper~ "
                    "|-tolower~ "
                    "|-widen~ "
                    "|-narrow~ ",
                    &opt_id,
                    &opt_is,
                    &opt_scan_is,
                    &opt_scan_not,
                    &opt_toupper,
                    &opt_tolower,
                    &opt_widen,
                    &opt_narrow,
                    (void*)0 /* sentinel */);
}
