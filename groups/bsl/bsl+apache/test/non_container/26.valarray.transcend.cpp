/***************************************************************************
 *
 * 26.valarray.transcend.cpp - verify [lib.valarray.transcend] requirements
 *
 * $Id$
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
 * Copyright 1994-2008 Rogue Wave Software.
 *
 **************************************************************************/

// disable implicit inclusion to work around a limitation in
// IBM's VisualAge 5.0.2.0 (see PR#26959)
#if defined __IBMCPP__ && !defined _RWSTD_NO_IMPLICIT_INCLUSION
#  define _RWSTD_NO_IMPLICIT_INCLUSION
#endif   // defined __IBMCPP__ && !defined _RWSTD_NO_IMPLICIT_INCLUSION

#include <cmath>
#include <cstddef> // for std::size_t
#include <valarray>

#include <driver.h>
#include <valcmp.h>

/**************************************************************************/

template <class T>
static void
test_val (const char* tname)
{


// 'std::fun' modified to be 'bsl::fun' for 'valarray' since specializations
// for 'valarray' lived in bsl namespaces.

#define TEST(fun) {                                                        \
        rw_info (0, __FILE__, __LINE__, "template <> std::valarray<%s> "   \
                 "std::" #fun " (std::valarray<%s>)", tname, tname);       \
                                                                           \
        std::valarray<T> val (10);                                         \
        for (std::size_t i = 0; i != val.size (); ++i)                     \
            val [i] = T (i);                                               \
                                                                           \
        const std::valarray<T> result = bsl::fun (val);                    \
                                                                           \
        bool success = 10 == val.size () && val.size () == result.size (); \
                                                                           \
        for (std::size_t j = 0; success && j != result.size (); ++j) {     \
            if (!rw_equal (result [j], T (std::fun (T (j)))))              \
                success = false;                                           \
        }                                                                  \
                                                                           \
        rw_assert (success, __FILE__, __LINE__,                            \
                   "template <> std::valarray<%s> std::" #fun              \
                   "(std::valarray<%s>)", tname, tname);                   \
    } (void)0

#define TEST_2(fun, x) {                                                   \
        if (x < 0)                                                         \
            rw_info (0, __FILE__, __LINE__,                                \
                     "template <> std::valarray<%s> "                      \
                     "std::" #fun " (const %s&, const std::"               \
                     "valarray<%s>&)", tname, tname, tname);               \
        else if (x > 0)                                                    \
            rw_info (0, __FILE__, __LINE__,                                \
                     "template <> std::valarray<%s> "                      \
                     "std::" #fun " (const std::valarray<%s>&,"            \
                     "const %s&)", tname, tname, tname);                   \
        else                                                               \
            rw_info (0, __FILE__, __LINE__,                                \
                     "template <> std::valarray<%s> "                      \
                     "std::" #fun " (const std::valarray<%s>&," \
                     "const std::valarray<%s>&)", tname, tname, tname);    \
                                                                           \
        std::valarray<T> val (10);                                         \
        for (std::size_t i = 0; i != val.size (); ++i)                     \
            val [i] = T (i);                                               \
                                                                           \
        const std::valarray<T> result =                                    \
             x < 0 ? bsl::fun (T (x), val)                                 \
           : x > 0 ? bsl::fun (val, T (x))                                 \
                   : bsl::fun (val, val);                                  \
                                                                           \
        bool success = 10 == val.size () && val.size () == result.size (); \
                                                                           \
        for (std::size_t j = 0; success && j != result.size (); ++j) {     \
            T res;                                                         \
            if (x < 0) res = T (std::fun (T (x), T (j)));                  \
            if (x > 0) res = T (std::fun (T (j), T (x)));                  \
            if (!x) res = T (std::fun (T (j), T (j)));                     \
            if (!rw_equal (result [j], res))                               \
                success = false;                                           \
        }                                                                  \
                                                                           \
        rw_assert (success, __FILE__, __LINE__,                            \
                   "template <> std::valarray<%s> std::" #fun "(...)",     \
                   tname);                                                 \
    } (void)0

    TEST (abs);
    TEST (acos);
    TEST (asin);
    TEST (atan);

    TEST_2 (atan2, -2);
    TEST_2 (atan2, +3);
    TEST_2 (atan2,  0);

    TEST (cos);
    TEST (cosh);
    TEST (exp);
    TEST (log);
    TEST (log10);

    TEST_2 (pow, -4);
    TEST_2 (pow, +5);
    TEST_2 (pow,  0);

    TEST (sin);
    TEST (sinh);
    TEST (sqrt);
    TEST (tan);
    TEST (tanh);
}

/**************************************************************************/

static int
run_test (int /*unused*/, char* /*unused*/ [])
{

    test_val<float> ("float");
    test_val<double> ("double");


#ifndef _RWSTD_NO_LONG_DOUBLE

    test_val<long double> ("long double");

#endif   // _RWSTD_NO_LONG_DOUBLE

    return 0;
}

/*extern*/ int
main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.valarray.transcend",
                    0,   // no comment
                    run_test, "", 0);
}

