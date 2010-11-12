/***************************************************************************
 *
 * 18.limits.cvqual.cpp:
 *
 *   Test exercising lib.numeric.limits, numeric_limits specializations
 *   on cv-qualified scalar types. See DR 559 for details:
 *   http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#559
 *
 * $Id: 18.limits.cvqual.cpp 580483 2007-09-28 20:55:52Z sebor $
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  License); you may  not use  this file
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
 * Copyright 2006-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <limits>
#include <driver.h>

/**************************************************************************/

template <class T>
struct UserType
{
    const T val_;

    UserType (T val = T ()): val_ (val) { /* empty */}

    operator T () const { return val_; }
};


template <class T, class CVQualifiedT>
void test_limits (const volatile T*, CVQualifiedT*,
                  const char *tname,
                  const char *quals)
{
#ifndef _RWSTD_NO_EXT_CV_QUALIFIED_LIMITS

    typedef std::numeric_limits<T>            limits;
    typedef std::numeric_limits<CVQualifiedT> cv_limits;

   // verify that the member has the same value as the same member
   // of a specialization of numeric_limits on the cv-qualified T
#  define TEST(member)                                                      \
      rw_assert (limits::member == cv_limits::member, 0, __LINE__,          \
                 "numeric_limits<%s>::%s == numeric_limits<%s %1$s>::%2$s", \
                 tname, #member, quals)
#else   // if defined (_RWSTD_NO_EXT_CV_QUALIFIED_LIMITS)

    typedef std::numeric_limits<UserType<T> > limits;
    typedef std::numeric_limits<CVQualifiedT> cv_limits;

   // verify that the member has the same value as the same member
   // of a specialization of numeric_limits on some user-defined T
   // (i.e., the primary template)
#  define TEST(member)                                                      \
      rw_assert (limits::member == cv_limits::member, 0, __LINE__,          \
                 "numeric_limits<%s>::%s == numeric_limits<%s %1$s>::%2$s", \
                 tname, #member, quals)
#endif   // _RWSTD_NO_EXT_CV_QUALIFIED_LIMITS

    TEST (is_specialized);
    TEST (min ());
    TEST (max ());
    TEST (digits);
    TEST (digits10);
    TEST (is_signed);
    TEST (is_integer);
    TEST (is_exact);
    TEST (radix);

    TEST (epsilon ());
    TEST (round_error ());

    TEST (min_exponent);
    TEST (min_exponent10);
    TEST (max_exponent);
    TEST (max_exponent10);

    TEST (has_infinity);
    TEST (has_quiet_NaN);
    TEST (has_signaling_NaN);
    TEST (has_denorm);
    TEST (has_denorm_loss);

    TEST (infinity ());

    if (limits::is_integer) {
        TEST (quiet_NaN ());
        TEST (signaling_NaN ());
    }
    else {
        const bool expect = limits::has_quiet_NaN;

        rw_assert (expect == (limits::quiet_NaN () != cv_limits::quiet_NaN ()),
                   0, __LINE__,
                   "numeric_limits<%s>::qNaN() %{?}!=%{:}==%{;} "
                   "numeric_limits<%s %1$s>::qNaN ()",
                   tname, expect, quals);
    }

    TEST (denorm_min ());

    TEST (is_iec559);
    TEST (is_bounded);
    TEST (is_modulo);
    TEST (traps);
    TEST (tinyness_before);
    TEST (round_style);
}

/**************************************************************************/

template <class T>
void test_limits (const volatile T*, const char *tname)
{
    typedef const T          const_T;
    typedef volatile T       volatile_T;
    typedef const volatile T const_volatile_T;
    
    rw_info (0, 0, __LINE__,
             "std::numeric_limits<T> with T = cv-qualified %s", tname);

    test_limits ((T*)0, (const_T*)0, tname, "const");
    test_limits ((T*)0, (volatile_T*)0, tname, "volatile");
    test_limits ((T*)0, (const_volatile_T*)0, tname, "const volatile");
}

/**************************************************************************/

static int
run_test (int, char*[])
{
#undef TEST
#define TEST(T)   test_limits ((T*)0, _RWSTD_STRSTR (T))

#ifndef _RWSTD_NO_NATIVE_BOOL
    TEST (bool);
#endif   // _RWSTD_NO_NATIVE_BOOL

    TEST (char);
    TEST (signed char);
    TEST (unsigned char);

    TEST (short);
    TEST (unsigned short);

    TEST (int);
    TEST (unsigned);

    TEST (long);
    TEST (unsigned long);

#ifdef _RWSTD_LONG_LONG
    TEST (_RWSTD_LONG_LONG);
    TEST (unsigned _RWSTD_LONG_LONG);
#endif

    TEST (float);
    TEST (double);

#ifndef _RWSTD_NO_LONG_DOUBLE
    TEST (long double);
#endif   // _RWSTD_NO_LONG_DOUBLE

#ifndef _RWSTD_NO_NATIVE_WCHAR_T
    TEST (wchar_t);
#endif   // _RWSTD_NO_NATIVE_WCHAR_T

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.numeric.limits",

#ifdef _RWSTD_NO_EXT_CV_QUALIFIED_LIMITS
                    "testing the absence of "
#endif   // _RWSTD_NO_EXT_CV_QUALIFIED_LIMITS
                    "specializations on cv-qualifed types",
                    run_test,
                    0,
                    (void*)0);
}
