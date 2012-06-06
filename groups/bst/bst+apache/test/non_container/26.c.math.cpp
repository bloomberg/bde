/***************************************************************************
 *
 * c_math.cpp - test exercising [lib.c.math]
 *
 * $Id: 26.c.math.cpp 648752 2008-04-16 17:01:56Z faridz $
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

// this may be the "native" header that comes with the compiler
// do not assume that any of our stuff (i.e., macros) is available

#if defined (__IBMCPP__) && !defined (_RWSTD_NO_IMPLICIT_INCLUSION)
// Disable implicit inclusion to work around
// a limitation in IBM's VisualAge 5.0.2.0 (see PR#26959)

#  define _RWSTD_NO_IMPLICIT_INCLUSION
#endif

#include <cstdlib>
#include <cmath>

#include <any.h>
#include <driver.h>
#include <valcmp.h>


// define function templates with the same signatures as
// the required overloads; if the required overloads are
// not defined, the corresponding specializations of the
// function templates will be instantiated and called,
// triggering an assertion

#define FUN_1(name)                                                     \
    template <class T>                                                  \
    T name (T) {                                                        \
        static const char* const tname = rw_any_t (T ()).type_name ();  \
        rw_assert (0, 0, __LINE__, "std::%s(%s) not declared\n",        \
                   #name, tname);                                       \
        return T ();                                                    \
    } typedef void unused_typedef

#define FUN_2(name)                                                     \
    template <class T>                                                  \
    T name (T, T) {                                                     \
        static const char* const tname = rw_any_t (T ()).type_name ();  \
        rw_assert (0, 0, __LINE__, "std::%s (%s, %s) not declared\n",   \
                   #name, tname, tname);                                \
        return T ();                                                    \
    } typedef void unused_typedef

#define FUN_2_PTR(name)                                                 \
    template <class T>                                                  \
    T name (T, T*) {                                                    \
        static const char* const tname = rw_any_t (T ()).type_name ();  \
        rw_assert (0, 0, __LINE__, "std::%s (%s, %s*) not declared\n",  \
                   #name, tname, tname);                                \
        return T ();                                                    \
    } typedef void unused_typedef

#define FUN_2_INT(name)                                                 \
    template <class T>                                                  \
    T name (T, int) {                                                   \
        static const char* const tname = rw_any_t (T ()).type_name ();  \
        rw_assert (0, 0, __LINE__, "std::%s (%s, int) not declared\n",  \
                   #name, tname);                                       \
        return T ();                                                    \
    } typedef void unused_typedef

#define FUN_2_PINT(name)                                                \
    template <class T>                                                  \
    T name (T, int*) {                                                  \
        static const char* const tname = rw_any_t (T ()).type_name ();  \
        rw_assert (0, 0, __LINE__, "std::%s (%s, int*) not declared\n", \
                   #name, tname);                                       \
        return T ();                                                    \
    } typedef void unused_typedef


#ifndef __GNUC__ // gcc uses its own template versions of funcs in std
_RWSTD_NAMESPACE (std) {
#endif

// abs() is not declared in the C header <math.h> but is required
// to be declared in the C++ headers <math.h> and <cmath>
FUN_1 (abs);
FUN_1 (acos);
FUN_1 (asin);
FUN_1 (atan);
FUN_2 (atan2);
FUN_1 (ceil);
FUN_1 (cos);
FUN_1 (cosh);
FUN_1 (div);
FUN_1 (exp);
FUN_1 (fabs);
FUN_1 (floor);
FUN_2 (fmod);
FUN_2_PINT (frexp);
FUN_2_INT (ldexp);
FUN_1 (log);
FUN_1 (log10);
FUN_2_PTR (modf);
FUN_2 (pow);
FUN_2_INT (pow);
FUN_1 (sin);
FUN_1 (sinh);
FUN_1 (sqrt);
FUN_1 (tan);
FUN_1 (tanh);

#ifndef __GNUC__

}   // namespace std
#endif

// returns true if all `size' bytes starting at `s' are 0
static bool
check_bits (const char *s, unsigned size)
{
    while (size-- && !s [size]);
    return unsigned (-1) == size;
}


static void
clear_bytes (void *pb, _RWSTD_SIZE_T size)
{
    for (_RWSTD_SIZE_T i = 0; i != size; ++i)
        ((char*)pb)[i] = 0;
}


static void
test_behavior ()
{
    union {
        float       f;
        double      d;

#ifndef _RWSTD_NO_LONG_DOUBLE
        long double l;
#endif   // _RWSTD_NO_LONG_DOUBLE

        char buf [sizeof (long double) * 2];
    } u;

#if !defined (__SUNPRO_CC) || __SUNPRO_CC > 0x530

    // make sure functions do not overflow buffer
    clear_bytes (u.buf, sizeof u);

    const float f = std::modf (3.141592f, &u.f);

    rw_assert (   3000 == int (u.f * 1000) && 141592 == int (f * 1000000)
               && check_bits (u.buf + sizeof u.f, sizeof u - sizeof u.f),
               0, __LINE__, "float std::modf (float)");

#endif   // SunPro > 5.3

    clear_bytes (u.buf, sizeof u);
    const double d = std::modf (3.1415926, &u.d);

    rw_assert (   3000 == int (u.d * 1000) && 1415926 == int (d * 10000000)
               && check_bits (u.buf + sizeof u.d, sizeof u - sizeof u.d),
               0, __LINE__, "double std::modf (double)");

#ifndef _RWSTD_NO_LONG_DOUBLE

#  if !defined (__SUNPRO_CC) || __SUNPRO_CC > 0x530

    clear_bytes (u.buf, sizeof u);
    const long double l = std::modf (3.1415926L, &u.l);

    rw_assert (   3000 == int (u.l * 1000) && 1415926 == int (l * 10000000)
               && check_bits (u.buf + sizeof u.l, sizeof u - sizeof u.l),
               0, __LINE__, "long double std::modf (long double)");

#  endif   // SunPro > 5.3

#endif   // _RWSTD_NO_LONG_DOUBLE


    // check overloads of std::pow()
    for (int i = -10; i != 10; ++i) {

        for (int j = -10; j != 10; ++j) {

            if (-9 < j && j < 9) {
                const float fi = float (i);
                const float fj = float (j);

                // verify that both versions are equivalent
                const float xf = std::pow (fi, j);
                const float yf = std::pow (fi, fj);

                rw_assert (rw_equal (xf, yf) || !i && j < 0,
                           0, __LINE__,
                           "std::pow (%d.0f, %d) = %g, "
                           "std::pow (%d,0f, %d.0f) = %g",
                           i, j, xf, i, j, yf);
            }

            const double id = double (i);
            const double jd = double (j);

            const double xd = std::pow (id, j);
            const double yd = std::pow (id, jd);

            rw_assert (rw_equal (xd, yd) || !i && j < 0,
                       0, __LINE__,
                       "std::pow (%d.0, %d) = %g, "
                       "std::pow (%d.0, %d.0) = %g",
                       i, j, xd, i, j, yd);

#ifndef _RWSTD_NO_LONG_DOUBLE

            const long double il = _RWSTD_STATIC_CAST (long double, i);
            const long double jl = _RWSTD_STATIC_CAST (long double, j);

            const long double xl = std::pow (il, j);
            const long double yl = std::pow (il, jl);

            rw_assert (rw_equal (xl, yl) || !i && j < 0,
                       0, __LINE__,
                       "std::pow (%d.0L, %d) = %Lg, "
                       "std::pow (%d.0L, %d.0L) = %Lg",
                       i, j, xl, i, j, yl);

#endif   // _RWSTD_NO_LONG_DOUBLE

        }
    }
}


static void
test_declarations ()
{
    _USING (namespace std);

    int i = 1;
    long l = 1;

    float f = 0.1;
    double d = 0.1;

#ifndef _RWSTD_NO_LONG_DOUBLE

    long double ld = 0.1;

#  define CHECK_OVERLOADS(name)                                         \
          rw_info (0, 0, __LINE__, "std::%s() overloads ", #name);      \
          (void)name (f);                                               \
          (void)name (d);                                               \
          (void)name (ld)
#else   // if defined (_RWSTD_NO_LONG_DOUBLE)
#  define CHECK_OVERLOADS(name)                                         \
          rw_info (0, 0, __LINE__, "std::%s() overloads", #name);       \
          (void)name (f);                                               \
          (void)name (d)
#endif   // _RWSTD_NO_LONG_DOUBLE

    CHECK_OVERLOADS (abs);
    CHECK_OVERLOADS (acos);
    CHECK_OVERLOADS (asin);
    CHECK_OVERLOADS (atan);
    CHECK_OVERLOADS (atan);
    CHECK_OVERLOADS (ceil);
    CHECK_OVERLOADS (cos);
    CHECK_OVERLOADS (cosh);
    CHECK_OVERLOADS (exp);
    CHECK_OVERLOADS (fabs);
    CHECK_OVERLOADS (floor);

    rw_info (0, 0, __LINE__, "std::%s() overloads", "fmod");
    (void)fmod (f, f); (void)fmod (d, d);

    rw_info (0, 0, __LINE__, "std::%s() overloads", "frexp");
    (void)frexp (f, &i); (void)frexp (d, &i);

    rw_info (0, 0, __LINE__, "std::%s() overloads", "ldexp");
    (void)ldexp (f, i); (void)ldexp (d, i);

    CHECK_OVERLOADS (log);
    CHECK_OVERLOADS (log10);

    rw_info (0, 0, __LINE__, "std::%s() overloads", "modf");
    (void)modf (f, &f); (void)modf (d, &d);

    rw_info (0, 0, __LINE__, "std::%s() overloads", "pow");
    (void)pow (f, f); (void)pow (d, d);
    (void)pow (f, i); (void)pow (d, i);

    CHECK_OVERLOADS (sin);
    CHECK_OVERLOADS (sinh);
    CHECK_OVERLOADS (sqrt);
    CHECK_OVERLOADS (tan);
    CHECK_OVERLOADS (tanh);

#ifndef _RWSTD_NO_LONG_DOUBLE

    (void)atan2 (ld, ld);
    (void)fmod (ld, ld);
    (void)frexp (ld, &i);
    (void)ldexp (ld, i);
    (void)modf (ld, &ld);
    (void)pow (ld, ld);
    (void)pow (ld, i);

#endif   // _RWSTD_NO_LONG_DOUBLE

    rw_info (0, 0, __LINE__, "std::%s() overloads", "div");
    (void)div (i, i);
    (void)div (l, l);
}

/**************************************************************************/

static int no_declarations;
static int no_behavior;


int run_test (int, char*[])
{
    if (no_declarations)
        rw_note (0, __FILE__, __LINE__, "test of declarations disabled");
    else
        test_declarations ();

    if (no_behavior)
        rw_note (0, __FILE__, __LINE__, "test of behavior disabled");
    else
        test_behavior ();

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.c.math",
                    0 /* no comment */, run_test,
                    "|-no-declarations# |-no-behavior#",
                    &no_declarations, &no_behavior);
}
