/***************************************************************************
 *
 * pairs.cpp - test exercising [lib.pairs]
 *
 * $Id: 20.pairs.cpp 550991 2007-06-26 23:58:07Z sebor $
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

#include <utility>

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>

/**************************************************************************/

struct Y: UserClass { };


int less_used;


_RWSTD_NAMESPACE (std) {

_RWSTD_SPECIALIZED_CLASS
bool less<Y>::operator() (const Y &a, const Y &b) const
{
    ++less_used;
    return a < b;
}

}   // namespace std


#ifndef _RWSTD_NO_EXPLICIT_CTOR_INSTANTIATION

typedef base<def_ctor> DefaultConstructible;
typedef base<cpy_ctor> CopyConstructible;

_RWSTD_NAMESPACE (std) {

// enclosing explicit instantiation within the declaring namespace
// instead of qualifying it with its name to work around compiler
// bugs (e.g., SunPro 5.4 -- see PR #28117)

#ifndef _RWSTD_NO_EMPTY_MEM_INITIALIZER

template pair<DefaultConstructible, DefaultConstructible>::pair ();

#endif  // _RWSTD_NO_EMPTY_MEM_INITIALIZER

template pair<CopyConstructible, CopyConstructible>::
         pair (const CopyConstructible&, const CopyConstructible&);

}   // namespace std

#endif   // _RWSTD_NO_EXPLICIT_CTOR_INSTANTIATION

/**************************************************************************/

template <class T, class U>
void test_pair (T, const char *tname, U, const char *uname)
{
    typedef std::pair<T, U> Pair;

    T *pt = (typename Pair::first_type*)0;
    U *pu = (typename Pair::second_type*)0;

    rw_info (0, 0, __LINE__, "std::pair<%s, %s> data members", tname, uname);

    if (0) {   // compile only

        // verify pair data members and their types
        Pair *p = 0;
        pt = &p->first;
        pu = &p->second;

        _RWSTD_UNUSED (pt);
        _RWSTD_UNUSED (pu);
    }

    if (0) {   // compile only
        Pair *p0 = 0;
        std::pair<long, double> p1 (*p0);

        _RWSTD_UNUSED (p1);
    }

    {
        rw_info (0, 0, __LINE__, "std::pair<%s, %s>::pair()", tname, uname);

        UserClass::reset_totals ();

        std::pair<Y, Y> py;

        rw_assert (UserClass::n_total_def_ctor_ == 2, 0, __LINE__,
                   "pair<Y, Y>::pair() called %d default ctors, "
                   "expected 2", UserClass::n_total_def_ctor_, 2);

#ifndef _RWSTD_NO_EMPTY_MEM_INITIALIZER

        // exercise lwg issue 265
        rw_assert (UserClass::n_total_copy_ctor_ == 0, 0, __LINE__,
                   "pair<Y, Y>::pair() called %d copy ctors, "
                   "expected 0", UserClass::n_total_copy_ctor_);

#else   // if defined (_RWSTD_NO_EMPTY_MEM_INITIALIZER)

        // 20.2.2, p2: commented out -- calls to copy ctor may be elided
        // RW_ASSERT (t, UserClass::n_total_copy_ctor_ == 2,
        //            ("std::pair<Y, Y>::pair() called %d copy ctors, "
        //            "expected 2", UserClass::n_total_copy_ctor_));

#endif   // _RWSTD_NO_EMPTY_MEM_INITIALIZER

        rw_info (0, 0, __LINE__,
                 "std::pair<%s, %s>::pair(const %s&, const %s&)",
                 tname, uname, tname, uname);

        UserClass x0;
        UserClass x1;

        UserClass::reset_totals ();

        // 20.2.2, p3
        std::pair<UserClass, UserClass> px0 (x0, x1);

        rw_assert (UserClass::n_total_copy_ctor_ == 2, 0, __LINE__,
                   "pair<T, U>::pair (const T&, const U&) called %d cpy "
                   "ctors, expected 2", UserClass::n_total_copy_ctor_);


        rw_info (0, 0, __LINE__,
                 "template <class T, class U> "
                 "std::pair<%s, %s>::pair(const pair<T, U>&)",
                 tname, uname);

        UserClass::reset_totals ();

        // 20.2.2, p4
        std::pair<UserClass, UserClass> px (py);

        rw_assert (UserClass::n_total_copy_ctor_ == 2, 0, __LINE__,
                   "template <class T, class U> pair<UserClass, "
                   "UserClass>::pair(const std::pair<T, U>&) called "
                   "%d cpy ctors, expected 2",
                   UserClass::n_total_copy_ctor_);


        rw_info (0, 0, __LINE__,
                 "template <class T, class U> "
                 "std::pair<%s, %s>::operator= (const pair<T, U>&)",
                 tname, uname);

        UserClass::reset_totals ();

        // exercise template assignment if provided,
        // otherwise template ctor and ordinary assignment
        px = py;

        rw_assert (UserClass::n_total_copy_ctor_ == 0, 0, __LINE__,
                   "template <class T, class U> pair<UserClass, "
                   "UserClass>::operator=(const pair<T, U>&) "
                   "called %d cpy ctors, expected 0",
                   UserClass::n_total_copy_ctor_);

        rw_assert (UserClass::n_total_op_assign_ == 2, 0, __LINE__,
                   "template <class T, class U> pair<UserClass, "
                   "UserClass>::operator=(const pair<T, U>&) "
                   "called %d assignment operators, "
                   "expected 2", UserClass::n_total_op_assign_);
    }

    {

// exercise the signature of a nonmember (or static member) function
#define FUN(result, name, arg_list) do {                                \
        rw_info (0, 0, __LINE__, "%s(const pair<%s, %s>&, "             \
                                  "const pair<%s, %s>&)", #name,        \
                                  tname, uname, tname, uname);          \
        /* make name unique to prevent bogus gcc -Wshadow warnings */   \
        result (*_RWSTD_PASTE (pf, __LINE__)) arg_list = &name;         \
        _RWSTD_UNUSED (_RWSTD_PASTE (pf, __LINE__));                    \
    } while (0)

        FUN (bool, std::operator==, (const Pair&, const Pair&));
        FUN (bool, std::operator!=, (const Pair&, const Pair&));
        FUN (bool, std::operator<,  (const Pair&, const Pair&));
        FUN (bool, std::operator<=, (const Pair&, const Pair&));
        FUN (bool, std::operator>,  (const Pair&, const Pair&));
        FUN (bool, std::operator>=, (const Pair&, const Pair&));

        // 20.2.2, p5
        std::pair<Y, Y> p0;

        Y::n_total_op_lt_ = 0;

        bool b = p0 == p0;
        rw_assert (b, 0, __LINE__,
                   "operator== (const pair<UserClass, UserClass>&, "
                   "const pair<UserClass, UserClass>&)");
        rw_assert (2 == Y::n_total_op_eq_, 0, __LINE__,
                   "operator== (const pair<UserClass, UserClass>&, "
                   "const pair<UserClass, UserClass>&)");


        // exercise lwg issue 348
        // (std::less required to be used in order to satisfy 20.3.3, p8)
        std::pair<Y, Y> p1;

        p0.first.data_.val_  = 1;
        p0.second.data_.val_ = 2;

        p1.first.data_.val_  = 3;
        p1.second.data_.val_ = 4;

        less_used = 0;

        b = p0 < p1;

        rw_assert (b, 0, __LINE__,
                   "operator< (const pair<Y, Y>&, const pair<Y, Y>&)");

        rw_assert (1 == less_used, 0, __LINE__,
                   "less<Y>::operator() called %d time(s), expected 1",
                   less_used);

        p1.first.data_.val_ = 1;

        less_used = 0;

        b = p0 < p1;

        rw_assert (b, 0, __LINE__,
                   "operator< (const pair<Y, Y>&, const pair<Y, Y>&)");

        rw_assert (3 == less_used, 0, __LINE__,
                   "less<Y>::operator() called %d time(s), expected 3",
                   less_used);

        p1.first.data_.val_ = 0;

        less_used = 0;

        b = p0 < p1;

        rw_assert (!b, 0, __LINE__,
                   "operator< (const pair<Y, Y>&, const pair<Y, Y>&)");

        rw_assert (2 == less_used, 0, __LINE__,
                   "less<Y>::operator() called %d time(s), expected 2",
                   less_used);
    }
}

/**************************************************************************/

static int
run_test (int, char**)
{
    test_pair (char (), "char", long (), "long");
    test_pair (int (), "int", float (), "float");

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.pairs",
                    0 /* no comment */,
                    run_test,
                    "",
                    (void*)0 /* sentinel */);
}
