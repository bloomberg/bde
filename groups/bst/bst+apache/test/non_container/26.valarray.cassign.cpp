/***************************************************************************
 *
 * 26.valarray.cassign.cpp - tests exercising valarray computed assignment
 *
 * $Id: 26.valarray.cassign.cpp 650706 2008-04-23 00:32:39Z sebor $
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
 * Copyright (C) 2007-2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#include <cstdlib>       // for free(), strtod(), size_t
#include <valarray>      // for indirect_array, valarray

#include <rw_value.h>    // for UserClass
#include <driver.h>      // for rw_test()
#include <rw_printf.h>   // for rw_asnprintf()

/**************************************************************************/

// returns an array of size elements of type T constructed from a string
// of comma-separated values
template <class T>
T*
make_array (const T*, const char *s, std::size_t *psize)
{
    std::size_t bufsize = 0;   // capacity of buffer
    std::size_t nelems  = 0;   // number of elements in buffer

    if (psize)
        *psize = 0;

    if (0 == s)
        s = "";

    T* buf = 0;

    while (*s) {

        // get the next value from the string
        char *end = 0;
        const double val = std::strtod (s, &end);

        unsigned long repeat = 1;

        if ('@' == *end) {
            // process repeat directive (e.g., "123@5" expands into
            // 5 copies of the number 123)
            char *e = 0;
            repeat = std::strtoul (++end, &e, 10);

            // skip trailing whitespace
            for (; ' ' == *end; ++end);

            // the next character must be either a NUL or comma
            RW_ASSERT ('\0' == *e || ',' == *e);

            if (',' == *e)
                ++e;

            s = e;
        }
        else {
            // skip trailing whitespace
            for (; ' ' == *end; ++end);

            // the next character must be NUL or a comma
            RW_ASSERT ('\0' == *end || ',' == *end);
            s = end;

            if (*s)
                ++s;
        }

        while (repeat--) {

            if (nelems == bufsize) {
                // reallocate buffer

                const std::size_t newsize = (bufsize + 1) * 2;
                T* const tmp = new T [newsize];

                // copy data to the newly allocated buffer
                for (std::size_t i = 0; i != nelems; ++i)
                     tmp [i] = buf [i];

                // increase the capacity
                bufsize = newsize;

                // deallocate old buffer
                delete[] buf;
                buf = tmp;
            }

            // append value to the end of the buffer
            buf [nelems++] = T (val);
        }
    }

    if (psize)
        *psize = nelems;

    return buf;
}


// deletes an array of elements of type T returned from make_array
template <class T>
void
delete_array (const T *array, std::size_t)
{
    T* const a = _RWSTD_CONST_CAST (T*, array);
    delete[] a;
}


template <class T>
const std::size_t* count (const T*) { return 0; }


template <class T>
T value (const T &val) { return val; }

/**************************************************************************/

// deletes an array of elements of type T returned from make_array
void
delete_array (const UserClass *array, std::size_t nelems)
{
    UserClass* const a = _RWSTD_CONST_CAST (UserClass*, array);

    for (std::size_t i = 0; i != nelems; ++i)
        (a + i)->~UserClass ();

    operator delete (a);
}


// returns an array of size elements of type UserClass
// constructed from a string of comma-separated values
UserClass*
make_array (const UserClass*, const char *s, std::size_t *psize)
{
    std::size_t bufsize = 0;   // capacity of buffer
    std::size_t nelems  = 0;   // number of elements in buffer

    if (psize)
        *psize = 0;

    if (0 == s)
        s = "";

    UserClass* buf = 0;

    while (*s) {

        // get the next value from the string
        char *end = 0;
        const double val = std::strtod (s, &end);

        unsigned long repeat = 1;

        if ('@' == *end) {
            // process repeat directive (e.g., "123@5" expands into
            // 5 copies of the number 123)
            char *e = 0;
            repeat = std::strtoul (++end, &e, 10);

            // skip trailing whitespace
            for (; ' ' == *end; ++end);

            // the next character must be either a NUL or comma
            RW_ASSERT ('\0' == *e || ',' == *e);

            if (',' == *e)
                ++e;

            s = e;
        }
        else {
            // skip trailing whitespace
            for (; ' ' == *end; ++end);

            // the next character must be NUL or  a comma
            RW_ASSERT ('\0' == *end || ',' == *end);

            s = end;
            if (*s)
                ++s;
        }

        while (repeat--) {

            if (nelems == bufsize) {
                static const std::size_t size = sizeof (UserClass);

                void* const raw = operator new ((bufsize + 1) * 2 * size);
                UserClass* const tmp = _RWSTD_STATIC_CAST (UserClass*, raw);

                for (std::size_t i = 0; i != nelems; ++i)
                    new (tmp +i) UserClass (buf [i]);

                bufsize = (bufsize + 1) * 2;

                delete_array (buf, nelems);
                buf = tmp;
            }

            new (buf + nelems) UserClass ();
            buf [nelems].data_.val_ = int (val);
            ++nelems;
        }
    }

    if (psize)
        *psize = nelems;

    return buf;
}


const std::size_t* count (const UserClass*) { return &UserClass::count_; }

int value (const UserClass &val) { return val.data_.val_; }

/**************************************************************************/

template <class T>
void
test_op_assign (const T*,
                std::valarray<T>&
                (std::valarray<T>::*op_assign)(const std::valarray<T>&),
                const char *tname,     // T's type name
                const char *opname,    // which assignment operator
                int         line,      // test case line number
                const char *lhs_str,   // left hand side of assignment
                const char *rhs_str,   // right hand side of assignment
                const char *res_str)   // result of assignment
{
    std::size_t nelems = 0;
    std::size_t tmp;

    // create an array of values of type T from the string lhs_str
    // representing the left-hand side argument of the assignment
    const T* const lhs_array = make_array ((const T*)0, lhs_str, &nelems);

    // create an array of values of type T from the string rhs_str
    // representing the right-hand side argument of the assignment
    const T* const rhs_array = make_array ((const T*)0, rhs_str, &tmp);

    // both arguments of the assignment must have the same size
    RW_ASSERT (tmp == nelems);

    // create an array of values of type T from the string res_str
    // representing the result of the assignment
    const T* const res_array = make_array ((const T*)0, res_str, &tmp);

    // the result of the assignment must have the same size as both
    // arguments
    RW_ASSERT (tmp == nelems);

    // construct valarray arguments from the arrays created above
    /* const */ std::valarray<T> lhs_va (lhs_array, nelems);
    const       std::valarray<T> rhs_va (rhs_array, nelems);

    char*       fname = 0;
    std::size_t size  = 0;

    // pointer to a counter keeping track of all objects of type T
    // in existence (non-null only for T=UserClass)
    const std::size_t* const pcounter = count ((const T*)0);

    // get the number of objects of type T (only possible for user
    // defined T) before invoking the operator
    std::size_t nobjects = pcounter ? *pcounter : 0;

    // format the name of the function call to be used in diagnostic
    // messages below
    rw_asnprintf (&fname, &size,
                  "valarray<%s>(%s) %s std::valarray<%1$s>(%s)",
                  tname, lhs_str, opname, rhs_str);

    // invoke the assignment operator through the member pointer
    std::valarray<T> &res = (lhs_va.*op_assign)(rhs_va);

    // verify that the resturned reference refers to the assignee
    rw_assert (&res == &lhs_va, 0, line,
               "line %d == %#p, got %#p",
               __LINE__, fname, &lhs_va, &res);


    // verify the size of the array
    rw_assert (lhs_va.size () == nelems, 0, line,
               "line %d. %s.size() == %zu, got %zu",
               __LINE__, fname, nelems, lhs_va.size ());

    if (pcounter) {
        // verify that the assignment didn't leak any objects
        nobjects = *pcounter - nobjects;

        rw_assert (0 == nobjects, 0, line,
                   "line %d. %s constucted %zu objects, expected %zu",
                   __LINE__, fname, nobjects, nelems);
    }

    // verify the element values
    for (std::size_t i = 0; i != nelems; ++i) {
        if (!rw_assert (lhs_va [i] == res_array [i], 0, line,
                        "line %d. %s: element at index %zu == %d, got %d",
                        __LINE__, fname, i, value (res_array [i]),
                        value (lhs_va [i])))
            break;

    }

    delete_array (lhs_array, nelems);
    delete_array (rhs_array, nelems);

    std::free (fname);
}


/**************************************************************************/

#define OP(lhs, rhs, res) \
    test_op_assign ((T*)0, op, tname, opname, __LINE__, lhs, rhs, res)


template <class T>
void test_mul_assign (const T*, const char *tname, const char *opname)
{
    rw_info (0, 0, __LINE__,
             "std::valarray<%s>::operator%s (const std::valarray<%1$s>&)",
             tname, opname);

    std::valarray<T>& (std::valarray<T>::*op)(const std::valarray<T>&) =
        &std::valarray<T>::operator*=;

    OP ("", "", "");

    OP ("0", "0", "0");
    OP ("0", "1", "0");
    OP ("1", "0", "0");
    OP ("1", "1", "1");
    OP ("1", "2", "2");

    OP ("1,2", "1,1", "1,2");
    OP ("1,2", "1,2", "1,4");
    OP ("2,3", "4,5", "8,15");
    OP ("3,4", "5,6", "15,24");

    OP ("1,2,3",    "4,5,6",    "4,10,18");
    OP ("-1,-2,-3", "-4,-5,-6", "4,10,18");
    OP ("1,-2,3",   "-4,5,-6",  "-4,-10,-18");
}

/**************************************************************************/

template <class T>
void test_div_assign (const T*, const char *tname, const char *opname)
{
    rw_info (0, 0, __LINE__,
             "std::valarray<%s>::operator%s (const std::valarray<%1$s>&)",
             tname, opname);

    std::valarray<T>& (std::valarray<T>::*op)(const std::valarray<T>&) =
        &std::valarray<T>::operator/=;

    OP ("", "", "");

    OP ("0", "1", "0");
    OP ("1", "1", "1");
    OP ("2", "1", "2");
    OP ("2", "2", "1");
    OP ("3", "1", "3");
    OP ("3", "2", "1.5");
    OP ("4", "2", "2");

    OP ("0,1,2,3,4,5,6,7",     "1,1,1,1,1,1,1,1", "0,1,2,3,4,5,6,7");
    OP ("2,4,6,8,10,12,14,16", "2,2,2,2,2,2,2,2", "1,2,3,4,5,6,7,8");
    OP ("2,4,6,8,10,12,14,16", "2,2,2,2,2,2,2,2", "1,2,3,4,5,6,7,8");

    OP ("0@127", "1@127", "0@127");
    OP ("1@128", "1@128", "1@128");
    OP ("2@255", "1@255", "2@255");
    OP ("2@256", "2@256", "1@256");
    OP ("3@256", "1@256", "3@256");
    OP ("3@511", "2@511", "1.5@511");
    OP ("4@512", "2@512", "2@512");
}

/**************************************************************************/

template <class T>
void test_mod_assign (const T*, const char *tname, const char *opname)
{
    rw_info (0, 0, __LINE__,
             "std::valarray<%s>::operator%s (const std::valarray<%1$s>&)",
             tname, opname);

    std::valarray<T>& (std::valarray<T>::*op)(const std::valarray<T>&) =
        &std::valarray<T>::operator%=;

    OP ("", "", "");

    OP ("0", "1", "0");
    OP ("1", "1", "0");
    OP ("2", "1", "0");
    OP ("2", "2", "0");
    OP ("3", "1", "0");
    OP ("3", "2", "1");
    OP ("3", "3", "0");
    OP ("4", "1", "0");
    OP ("4", "2", "0");
    OP ("4", "3", "1");
    OP ("4", "4", "0");

    OP ("0,1",     "2,2",     "0,1");
    OP ("1,2,3",   "4,4,4",   "1,2,3");
    OP ("2,3,4,5", "6,6,6,6", "2,3,4,5");

    OP ("0,1,2,3,4,5,6,7,8,9", "10,9,8,7,6,5,4,3,2,1", "0,1,2,3,4,0,2,1,0,0");
}

#define CANNOT_TEST(T, name) \
    void test_##name (const T*, const char*, const char*) { } \
    typedef void swallow_semicolon


CANNOT_TEST (float, mod_assign);
CANNOT_TEST (double, mod_assign);
CANNOT_TEST (UserClass, mod_assign);

/**************************************************************************/

template <class T>
void test_add_assign (const T*, const char *tname, const char *opname)
{
    rw_info (0, 0, __LINE__,
             "std::valarray<%s>::operator%s (const std::valarray<%1$s>&)",
             tname, opname);

    std::valarray<T>& (std::valarray<T>::*op)(const std::valarray<T>&) =
        &std::valarray<T>::operator+=;

    OP ("", "", "");

    OP ("0",  "0", "0");
    OP ("0",  "1", "1");
    OP ("1",  "0", "1");
    OP ("1", "-1", "0");
    OP ("-1", "1", "0");

    OP ("0@31",  "0@31", "0@31");
    OP ("0@31",  "1@31", "1@31");
    OP ("1@31",  "0@31", "1@31");
    OP ("1@31", "-1@31", "0@31");
    OP ("-1@31", "1@31", "0@31");

    OP ("2@32",  "3@32", "5@32");
    OP ("3@32",  "5@32", "8@32");
    OP ("5@32",  "7@32", "12@32");
    OP ("9@32", "-9@32", "0@32");

    OP ("0@63",  "0@63", "0@63");
    OP ("0@63",  "1@63", "1@63");
    OP ("1@63",  "0@63", "1@63");
    OP ("1@63", "-1@63", "0@63");
    OP ("-1@63", "1@63", "0@63");

    OP ("0@63",  "0@63", "0@63");
    OP ("0@63",  "1@63", "1@63");
    OP ("1@63",  "0@63", "1@63");
    OP ("1@63", "-1@63", "0@63");
    OP ("-1@63", "1@63", "0@63");

    OP ("0,0", "0,0", "0,0");
    OP ("0,0", "0,1", "0,1");
    OP ("0,0", "1,0", "1,0");
    OP ("0,0", "1,1", "1,1");
    OP ("0,1", "0,1", "0,2");
    OP ("0,1", "1,0", "1,1");
    OP ("0,1", "1,1", "1,2");
    OP ("1,0", "0,0", "1,0");
    OP ("1,0", "0,1", "1,1");
    OP ("1,0", "1,0", "2,0");
    OP ("1,0", "1,1", "2,1");
    OP ("1,1", "0,0", "1,1");
    OP ("1,1", "0,1", "1,2");
    OP ("1,1", "1,0", "2,1");
    OP ("1,1", "1,1", "2,2");
}

/**************************************************************************/

template <class T>
void test_sub_assign (const T*, const char *tname, const char *opname)
{
    rw_info (0, 0, __LINE__,
             "std::valarray<%s>::operator%s (const std::valarray<%1$s>&)",
             tname, opname);

    std::valarray<T>& (std::valarray<T>::*op)(const std::valarray<T>&) =
        &std::valarray<T>::operator-=;

    OP ("", "", "");

    OP (" 0", " 0", " 0");
    OP (" 0", "+1", "-1");
    OP ("+1", " 0", "+1");
    OP ("+1", "-1", "+2");
    OP ("-1", "+1", "-2");

    OP (" 0@12", " 0@12", " 0@12");
    OP (" 0@23", "+1@23", "-1@23");
    OP ("+1@34", " 0@34", "+1@34");
    OP ("+1@45", "-1@45", "+2@45");
    OP ("-1@56", "+1@56", "-2@56");
}

/**************************************************************************/

template <class T>
void test_xor_assign (const T*, const char *tname, const char *opname)
{
    rw_info (0, 0, __LINE__,
             "std::valarray<%s>::operator%s (const std::valarray<%1$s>&)",
             tname, opname);

    std::valarray<T>& (std::valarray<T>::*op)(const std::valarray<T>&) =
        &std::valarray<T>::operator^=;

    OP ("", "", "");

    OP ("0", "0", "0");
    OP ("0", "1", "1");
    OP ("1", "0", "1");
    OP ("1", "1", "0");
    OP ("3", "0", "3");
    OP ("3", "1", "2");
    OP ("3", "2", "1");
    OP ("3", "3", "0");

    OP ("0@10", "0@10", "0@10");
    OP ("0@11", "1@11", "1@11");
    OP ("1@12", "0@12", "1@12");
    OP ("1@13", "1@13", "0@13");
    OP ("3@14", "0@14", "3@14");
    OP ("3@15", "1@15", "2@15");
    OP ("3@16", "2@16", "1@16");
    OP ("3@17", "3@17", "0@17");
}

CANNOT_TEST (float, xor_assign);
CANNOT_TEST (double, xor_assign);
CANNOT_TEST (UserClass, xor_assign);

/**************************************************************************/

template <class T>
void test_and_assign (const T*, const char *tname, const char *opname)
{
    rw_info (0, 0, __LINE__,
             "std::valarray<%s>::operator%s (const std::valarray<%1$s>&)",
             tname, opname);

    std::valarray<T>& (std::valarray<T>::*op)(const std::valarray<T>&) =
        &std::valarray<T>::operator&=;

    OP ("", "", "");

    OP ("0", "0", "0");
    OP ("0", "1", "0");
    OP ("1", "0", "0");
    OP ("1", "1", "1");
    OP ("3", "0", "0");
    OP ("3", "1", "1");
    OP ("3", "2", "2");
    OP ("3", "3", "3");

    OP ("0@10", "0@10", "0@10");
    OP ("0@11", "1@11", "0@11");
    OP ("1@12", "0@12", "0@12");
    OP ("1@13", "1@13", "1@13");
    OP ("3@14", "0@14", "0@14");
    OP ("3@15", "1@15", "1@15");
    OP ("3@16", "2@16", "2@16");
    OP ("3@17", "3@17", "3@17");
}

CANNOT_TEST (float, and_assign);
CANNOT_TEST (double, and_assign);
CANNOT_TEST (UserClass, and_assign);

/**************************************************************************/

template <class T>
void test_or_assign (const T*, const char *tname, const char *opname)
{
    rw_info (0, 0, __LINE__,
             "std::valarray<%s>::operator%s (const std::valarray<%1$s>&)",
             tname, opname);

    std::valarray<T>& (std::valarray<T>::*op)(const std::valarray<T>&) =
        &std::valarray<T>::operator|=;

    OP ("", "", "");

    OP ("0", "0", "0");
    OP ("0", "1", "1");
    OP ("1", "0", "1");
    OP ("1", "1", "1");
    OP ("3", "0", "3");
    OP ("3", "1", "3");
    OP ("3", "2", "3");
    OP ("3", "3", "3");

    OP ("0@10", "0@10", "0@10");
    OP ("0@11", "1@11", "1@11");
    OP ("1@12", "0@12", "1@12");
    OP ("1@13", "1@13", "1@13");
    OP ("3@14", "0@14", "3@14");
    OP ("3@15", "1@15", "3@15");
    OP ("3@16", "2@16", "3@16");
    OP ("3@17", "3@17", "3@17");
}

CANNOT_TEST (float, or_assign);
CANNOT_TEST (double, or_assign);
CANNOT_TEST (UserClass, or_assign);

/**************************************************************************/

template <class T>
void test_shl_assign (const T*, const char *tname, const char *opname)
{
    rw_info (0, 0, __LINE__,
             "std::valarray<%s>::operator%s (const std::valarray<%1$s>&)",
             tname, opname);

    std::valarray<T>& (std::valarray<T>::*op)(const std::valarray<T>&) =
        &std::valarray<T>::operator<<=;

    OP ("", "", "");

    OP ("0", "0", "0");
    OP ("0", "1", "0");
    OP ("1", "0", "1");
    OP ("1", "1", "2");
    OP ("3", "0", "3");
    OP ("3", "1", "6");
    OP ("3", "2", "12");
    OP ("3", "3", "24");

    OP ("0@10", "0@10", "0@10");
    OP ("0@11", "1@11", "0@11");
    OP ("1@12", "0@12", "1@12");
    OP ("1@13", "1@13", "2@13");
    OP ("3@14", "0@14", "3@14");
    OP ("3@15", "1@15", "6@15");
    OP ("3@16", "2@16", "12@16");
    OP ("3@17", "3@17", "24@17");
}

CANNOT_TEST (float, shl_assign);
CANNOT_TEST (double, shl_assign);
CANNOT_TEST (UserClass, shl_assign);

/**************************************************************************/

template <class T>
void test_shr_assign (const T*, const char *tname, const char *opname)
{
    rw_info (0, 0, __LINE__,
             "std::valarray<%s>::operator%s (const std::valarray<%1$s>&)",
             tname, opname);

    std::valarray<T>& (std::valarray<T>::*op)(const std::valarray<T>&) =
        &std::valarray<T>::operator>>=;

    OP ("", "", "");

    OP ("0",  "0", "0");
    OP ("0",  "1", "0");
    OP ("1",  "0", "1");
    OP ("2",  "1", "1");
    OP ("3",  "0", "3");
    OP ("6",  "1", "3");
    OP ("12", "2", "3");
    OP ("24", "3", "3");

    OP ("0@21",  "0@21", "0@21");
    OP ("0@21",  "1@21", "0@21");
    OP ("1@21",  "0@21", "1@21");
    OP ("2@21",  "1@21", "1@21");
    OP ("3@21",  "0@21", "3@21");
    OP ("6@21",  "1@21", "3@21");
    OP ("12@21", "2@21", "3@21");
    OP ("24@21", "3@21", "3@21");
}

CANNOT_TEST (float, shr_assign);
CANNOT_TEST (double, shr_assign);
CANNOT_TEST (UserClass, shr_assign);

/**************************************************************************/

enum {
    inx_mul_assign,
    inx_div_assign,
    inx_mod_assign,
    inx_add_assign,
    inx_sub_assign,
    inx_xor_assign,
    inx_and_assign,
    inx_or_assign,
    inx_shl_assign,
    inx_shr_assign,
    n_assign_ops
};

static int opt_assign [n_assign_ops];


template <class T>
void test_op_assign (const T*, const char *tname)
{
    const std::size_t nassigns = sizeof opt_assign / sizeof *opt_assign;
    for (std::size_t i = 0; i != nassigns; ++i) {
        if (0 < opt_assign [i]) {
            for (std::size_t j = 0; j != nassigns; ++j) {
                if (0 == opt_assign [j])
                    opt_assign [j] = -1;
            }
            break;
        }
    }

    if (rw_note (0 <= opt_assign [inx_mul_assign], 0, __LINE__,
                 "tests of operator*= disabled"))
        test_mul_assign ((const T*)0, tname, "*=");

    if (rw_note (0 <= opt_assign [inx_div_assign], 0, __LINE__,
                 "tests of operator/= disabled"))
        test_div_assign ((const T*)0, tname, "/=");

    if (rw_note (0 <= opt_assign [inx_mod_assign], 0, __LINE__,
                 "tests of operator%= disabled"))
        test_mod_assign ((const T*)0, tname, "%=");

    if (rw_note (0 <= opt_assign [inx_add_assign], 0, __LINE__,
                 "tests of operator+= disabled"))
        test_add_assign ((const T*)0, tname, "+=");

    if (rw_note (0 <= opt_assign [inx_sub_assign], 0, __LINE__,
                 "tests of operator-= disabled"))
        test_sub_assign ((const T*)0, tname, "-=");

    if (rw_note (0 <= opt_assign [inx_xor_assign], 0, __LINE__,
                 "tests of operator^= disabled"))
        test_xor_assign ((const T*)0, tname, "^=");

    if (rw_note (0 <= opt_assign [inx_and_assign], 0, __LINE__,
                 "tests of operator&= disabled"))
        test_and_assign ((const T*)0, tname, "&=");

    if (rw_note (0 <= opt_assign [inx_or_assign], 0, __LINE__,
                 "tests of operator|= disabled"))
        test_or_assign ((const T*)0, tname, "|=");

    if (rw_note (0 <= opt_assign [inx_shl_assign], 0, __LINE__,
                 "tests of operator<<= disabled"))
        test_shr_assign ((const T*)0, tname, "<<=");

    if (rw_note (0 <= opt_assign [inx_shr_assign], 0, __LINE__,
                 "tests of operator>>= disabled"))
        test_shr_assign ((const T*)0, tname, ">>=");
}

/**************************************************************************/

static int
run_test (int, char**)
{
#undef TEST
#define TEST(T)   test_op_assign ((const T*)0, #T)

    TEST (int);
    TEST (double);

#if 0x04020100 >= _RWSTD_VER

    // test fails to compile with stdcxx 4.2.1 and prior due to
    // STDCXX-512: http://issues.apache.org/jira/browse/STDCXX-512
    rw_warn (0, 0, __LINE__,
             "test of UserClass disabled in stdcxx 4.2.0 and prior "
             "due to STDCXX-512");

#else   // stdcxx >= 4.2.1

    TEST (UserClass);

#endif   // stdcxx version

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    // FIXME: add command line options to enable/disable each operator
    return rw_test (argc, argv, __FILE__,
                    "valarray.cassign",
                    0 /* no comment */,
                    run_test,
                    "|-times~ "
                    "|-divide~ "
                    "|-modulo~ "
                    "|-plus~ "
                    "|-minus~ "
                    "|-xor~ "
                    "|-and~ "
                    "|-or~ "
                    "|-shift_left~ "
                    "|-shift_right~",
                    opt_assign + inx_mul_assign,
                    opt_assign + inx_div_assign,
                    opt_assign + inx_mod_assign,
                    opt_assign + inx_add_assign,
                    opt_assign + inx_sub_assign,
                    opt_assign + inx_xor_assign,
                    opt_assign + inx_and_assign,
                    opt_assign + inx_or_assign,
                    opt_assign + inx_shl_assign,
                    opt_assign + inx_shr_assign,
                    (void*)0   /* sentinel */);
}
