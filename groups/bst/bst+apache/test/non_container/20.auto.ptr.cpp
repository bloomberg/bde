/***************************************************************************
 *
 * 20.autoptr.cpp - test exercising [lib.auto.ptr]
 *
 * $Id: 20.auto.ptr.cpp 596356 2007-11-19 16:23:21Z faridz $
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
 * Copyright 2000-2007 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#include <rw/_defs.h>

#if defined (__IBMCPP__) && !defined (_RWSTD_NO_IMPLICIT_INCLUSION)
  // disable implicit inclusion to work around
  // a limitation in IBM VisualAge 5.0.2.0 (see PR #26959)
#  define _RWSTD_NO_IMPLICIT_INCLUSION
#endif

#include <memory>

/**************************************************************************/

#ifndef _RWSTD_EXPLICIT_INSTANTIATION

   // explicitly instantiate

#  if !defined (_RWSTD_NO_NAMESPACE) && !defined (_RWSTD_NO_HONOR_STD)

     // verify that names are declared [only[ in namespace std

#    define TEST_CLASS_DEF(name, Tparam)                          \
            template class std::name Tparam                       \
            /* void name (void *name) */

#  else   // if defined (_RWSTD_NO_NAMESPACE) || defined (_RWSTD_NO_HONOR_STD)

     // verify that names do not collide with function argument names

#    define TEST_CLASS_DEF(name, Tparam)                          \
            template class std::name Tparam;                      \
            void foo (void *name)

#  endif   // !_RWSTD_NO_NAMESPACE && !_RWSTD_NO_HONOR_STD

#else   // if defined (_RWSTD_EXPLICIT_INSTANTIATION)

   // classes will implicitly instantiated below

#  if !defined (_RWSTD_NO_NAMESPACE) && !defined (_RWSTD_NO_HONOR_STD)

     // verify that names are declared [only] in namespace std

#    define TEST_CLASS_DEF(name, ignore)             void name (void *name)

#  else   // if defined (_RWSTD_NO_NAMESPACE) || defined (_RWSTD_NO_HONOR_STD)

#    define TEST_CLASS_DEF(name, ignore)             void foo (void *name)

#  endif   // !_RWSTD_NO_NAMESPACE && !_RWSTD_NO_HONOR_STD

#endif   // _RWSTD_EXPLICIT_INSTANTIATION


// auto_ptr_ref instantiated first to prevent bogus MSVC 6.0 warning C4660:
// template-class specialization 'auto_ptr_ref<int>' is already instantiated
// follows lwg issue 127
TEST_CLASS_DEF (auto_ptr_ref, <int>);

TEST_CLASS_DEF (auto_ptr, <int>);

/**************************************************************************/

#include <cmdopt.h>   // for rw_enabled()
#include <driver.h>   // for rw_assert(), rw_test(), ...

/**************************************************************************/

struct Base
{
    int   i_;          // unique object id

    static int cnt_;   // object counter
    static int gen_;   // unique id generator

    Base (): i_ (gen_++) { ++cnt_; }

    ~Base () {
        --cnt_;
    }

    static void sink (std::auto_ptr<Base>) { }
};

int Base::cnt_;   // Base object counter
int Base::gen_;   // Base unique id generator

struct Derived: Base
{
    static std::auto_ptr<Derived> source () {
        return std::auto_ptr<Derived> ();
    }

    static void sink (std::auto_ptr<Derived>) { }
};


// helpers to verify that each class' ctor is explicit
// not defined since they must not be referenced if test is successful
void is_explicit (const std::auto_ptr<Base>&);

struct has_implicit_ctor
{
    // NOT explicit
#ifndef _RWSTD_NO_NATIVE_BOOL

    has_implicit_ctor (bool*) { }

#endif   // _RWSTD_NO_NATIVE_BOOL

    has_implicit_ctor (char*) { }
    has_implicit_ctor (int*) { }
    has_implicit_ctor (double*) { }
    has_implicit_ctor (void**) { }
    has_implicit_ctor (Base*) { }
};

void is_explicit (const has_implicit_ctor&) { }


template <class T>
void test_auto_ptr (T*, const char *tname)
{
    rw_info (0, 0, 0, "std::auto_ptr<%s>", tname);

    if (!rw_enabled (tname)) {
        rw_note (0, 0, __LINE__, "auto_ptr<%s> test disabled", tname);
        return;
    }

    // exercise 20.4.5, p2 - auto_ptr<> interface

    typedef _TYPENAME std::auto_ptr<T>::element_type element_type;

    // verify that element_type is the same as T
    element_type *elem = (T*)0;

    // verify that default ctor is explicit
    is_explicit (elem);


// verify that a member function is accessible and has the appropriate
// signature, including return type and exception specification
#if defined(BSLS_PLATFORM_OS_AIX)
# define FUN(result, T, name, arg_list) do {                               \
        result (std::auto_ptr<T>::*pf) arg_list = &std::auto_ptr<T>::name; \
        _RWSTD_UNUSED (pf);                                                \
    } while (0)
#else
# define FUN(result, T, name, arg_list) do {                               \
        result (std::auto_ptr<T>::*pf) arg_list _PTR_THROWS (()) =         \
                                                  &std::auto_ptr<T>::name; \
        _RWSTD_UNUSED (pf);                                                \
    } while (0)
#endif

#if !defined (__HP_aCC) || \
                                                        _RWSTD_HP_aCC_MAJOR > 5

    // working around a bug in aCC (see PR #24430)
    FUN (std::auto_ptr<T>&, T, operator=, (std::auto_ptr<T>&));

#endif   // HP aCC > 5

    FUN (T&, T, operator*, () const);

#ifndef _RWSTD_NO_NONCLASS_ARROW_RETURN

    FUN (T*, T, operator->, () const);

#endif   // _RWSTD_NO_NONCLASS_ARROW_RETURN

    FUN (T*, T, get, () const);
    FUN (T*, T, release, ());
    FUN (void, T, reset, (T*));

#ifndef _RWSTD_NO_MEMBER_TEMPLATES

#  if !defined(__GNUG__) || __GNUG__ > 3 || __GNUG__ == 3 && __GNUC_MINOR__ > 2

     // g++ 2.95.2 and HP aCC can't take the address of a template member

#    if !defined (__HP_aCC) || _RWSTD_HP_aCC_MAJOR > 5

    // SunPro incorrectly warns here (see PR #27276)
    FUN (std::auto_ptr<Base>&, Base,
         operator=, (std::auto_ptr<Derived>&));

       // SunPro 5.4 can't decide between a ctor template
       // and a conversion operator (see PR #24476)
#      if !defined (__SUNPRO_CC) || __SUNPRO_CC > 0x540

#        if !defined (_MSC_VER) || _MSC_VER > 1310 || defined (__INTEL_COMPILER)
    FUN (std::auto_ptr_ref<Base>, Derived,
         operator std::auto_ptr_ref<Base>, ());

    FUN (std::auto_ptr<Base>, Derived,
         operator std::auto_ptr<Base>, ());

#        endif   // MSVC > 7.1

#      endif   // SunPro > 5.4

#    endif   // HP aCC > 5

#  endif   // gcc > 3.2

#endif   // _RWSTD_NO_MEMBER_TEMPLATES

    rw_info (0, 0, 0, "[lib.auto.ptr.cons]");

    T *pt = new T ();

    // 20.4.5.1, p1
    std::auto_ptr<T> ap1 (pt);
    rw_assert (pt == ap1.get (), 0, __LINE__,
               "auto_ptr<%s>::auto_ptr (%1$s*)", tname);

    // 20.4.5.1, p2
    std::auto_ptr<T> ap2 (ap1);
    rw_assert (0 == ap1.get (), 0, __LINE__,
               "auto_ptr<%s>::auto_ptr (auto_ptr&", tname);
    rw_assert (pt == ap2.get (), 0, __LINE__,
               "auto_ptr<%s>::auto_ptr (auto_ptr&)", tname);

    // 20.4.5.1, p7, 8, 9
    ap1 = ap2;
    rw_assert (0 == ap2.get (), 0, __LINE__,
               "auto_ptr<%s>::operator= (auto_ptr&)", tname);
    rw_assert (pt == ap1.get (), 0, __LINE__,
               "auto_ptr<%s>::operator= (auto_ptr&)", tname);


    rw_info (0, 0, 0, "[lib.auto.ptr.members]");

    // 20.4.5.2, p2
    rw_assert (*ap1.get () == ap1.operator*(), 0, __LINE__,
               "auto_ptr<%s>::operator*()", tname);

    // 20.4.5.2, p3
    rw_assert (ap1.get () == ap1.operator->(), 0, __LINE__,
               "auto_ptr<%s>::operator->()", tname);

    // 20.4.5.2, p4
    rw_assert (pt == ap1.get (), 0, __LINE__,
               "auto_ptr<%s>::get ()", tname);

    // 20.4.5.2, p5, 6
    rw_assert (pt == ap1.release () && 0 == ap1.get (), 0, __LINE__,
               "auto_ptr<%s>::release ()", tname);


    // 20.4.5.2, p7
    ap1.reset (pt);
    rw_assert (pt == ap1.get (),  0, __LINE__,
               "auto_ptr<%s>::reset ()", tname);
}

/**************************************************************************/

static void
test_auto_ptr_void ()
{
#if TEST_RW_EXTENSIONS
    // note that specializing auto_ptr on void is undefined
    // due to 17.4.3.6, p2; this is an extension of this
    // implementation
    rw_info (0, 0, 0, "std::auto_ptr<void> [extension]");

    std::auto_ptr<void> ap1;
    std::auto_ptr<void> ap2 ((void*)0);
    std::auto_ptr<void> ap3 (ap2);

    ap1 = ap1;
    ap1.operator= (ap1);

#ifndef _RWSTD_NO_MEMBER_TEMPLATES

#  if !defined (__HP_aCC) || 6 <=  _RWSTD_HP_aCC_MAJOR

    // working around an HP aCC 3 and 5 bug (STDCXX-655)

    ap1.operator=<void>(ap1);

#  endif   // !HP aCC or HP aCC 6 and better

    std::auto_ptr<int> ap4;
    ap1 = ap4;
    ap1.operator= (ap4);
    ap1.operator=<int>(ap4);

#endif // _RWSTD_NO_MEMBER_TEMPLATES

    // operator*() cannot be instantiated

    void* pv;

    pv = ap1.operator->();
    pv = ap1.get ();
    pv = ap1.release ();

    ap1.reset ();
    ap1.reset (pv);

#ifndef _RWSTD_NO_MEMBER_TEMPLATES

#  if !defined (__HP_aCC) || 6 <=  _RWSTD_HP_aCC_MAJOR

    // working around an HP aCC 3 and 5 bug (STDCXX-656)

    const std::auto_ptr_ref<void> ar = ap1.operator std::auto_ptr_ref<void>();
    const std::auto_ptr<void> ap5 = ap1.operator std::auto_ptr<void>();

    _RWSTD_UNUSED (ar);
    _RWSTD_UNUSED (ap5);

#  endif   // !HP aCC or HP aCC 6 and better
#endif // _RWSTD_NO_MEMBER_TEMPLATES

#endif // TEST_RW_EXTENSIONS
}

/**************************************************************************/

#ifndef _RWSTD_NO_MEMBER_TEMPLATES

// exercise 20.4.5.4
static std::auto_ptr<Derived>
test_auto_ptr_conversions ()
{
    rw_info (0, 0, 0, "[lib.auto.ptr.conv]");

    // 20.4.5.1, p4, 5, 6
    Derived *pd = new Derived;
    std::auto_ptr<Derived> ap1 (pd);
    rw_assert (pd == ap1.get (),  0, __LINE__,
               "auto_ptr::auto_ptr ()");

    std::auto_ptr<Base> ap2 (ap1);

    rw_assert (0 == ap1.get (), 0, __LINE__,
               "auto_ptr<Base>::auto_ptr(auto_ptr<Derived>&)");
    rw_assert (_RWSTD_STATIC_CAST (Base*, pd) == ap2.get (), 0, __LINE__,
               "auto_ptr<Base>::auto_ptr(auto_ptr<Derived>&)");

    ap2.reset (pd);

    // 20.4.5.2, p7 - must not delete owning pointer
    ap2.reset (pd);
    rw_assert (pd == ap2.get (),  0, __LINE__, "auto_ptr::reset ()");

    pd = new Derived;
    ap2.reset (pd);   // must delete owning pointer
    rw_assert (pd == ap2.get (),  0, __LINE__, "auto_ptr::reset ()");

    // 20.4.5.3, p1, 2, 3 - creates an auto_ptr_ref
    pd = new Derived;
    std::auto_ptr<Base> ap3 =
        std::auto_ptr<Base>(std::auto_ptr<Derived>(pd));

    rw_assert ((Base*)pd == ap3.get (), 0, __LINE__,
               "auto_ptr<>::auto_ptr(std::auto_ptr_ref)");

#if !defined (__HP_aCC) || _RWSTD_HP_aCC_MAJOR > 5

    pd = new Derived;
    std::auto_ptr<Derived> ap4 (pd);
    ap3 = std::auto_ptr<Base> (ap4);

    rw_assert (0 == ap4.get () && (Base*)pd == ap3.get (), 0, __LINE__,
               "auto_ptr<>::operator auto_ptr<>()");

#endif   // HP aCC > 5

    {
        // see CWG issue 84 for some background on the sequence below
        // http://anubis.dkuug.dk/jtc1/sc22/wg21/docs/cwg_defects.html#84

        std::auto_ptr<Derived> pd1 (Derived::source ());
        std::auto_ptr<Derived> pd2 (pd1);

#if !defined (__HP_aCC) || _RWSTD_HP_aCC_MAJOR > 5

        Derived::sink (Derived::source ());

#endif   // HP aCC > 5

        pd1 = pd2;
        pd1 = Derived::source();
        std::auto_ptr<Base> pb1 (Derived::source ());
        std::auto_ptr<Base> pb2 (pd1);

        // conversion sequence:
        //    1. auto_ptr<Derived>::operator auto_ptr<Base>()    [UDC]
        //    2. auto_ptr<Base>::operator auto_ptr_ref<Base>()   [UDC]
        //    3. auto_ptr<Base>(auto_ptr_ref<Base>)              [UDC]

        // since the conversion sequence involves more than one UDC
        // (User-Defined Conversion), it is illegal
        // Base::sink (Derived::source ());

        pb1 = pd2;
        pb1 = Derived::source ();

        return pd1;
    }
}

#endif   // _RWSTD_NO_MEMBER_TEMPLATES

/**************************************************************************/

static int rw_opt_no_conversions;     // for --no-conversions


static int
run_test (int, char**)
{

#ifndef _RWSTD_NO_NATIVE_BOOL

    test_auto_ptr ((bool*)0, "bool");

#endif   // _RWSTD_NO_NATIVE_BOOL

    test_auto_ptr ((char*)0, "char");
    test_auto_ptr ((int*)0, "int");
    test_auto_ptr ((double*)0, "double");
    test_auto_ptr ((void**)0, "void*");

#ifndef _RWSTD_NO_MEMBER_TEMPLATES

    int count = Base::cnt_;

    // exercise 20.4.5.4
    if (rw_opt_no_conversions)
        rw_note (0, 0, 0, "conversions test disabled");
    else
        test_auto_ptr_conversions ();

    // verify that no objects leaked
    rw_assert (count == Base::cnt_, 0, __LINE__,
               "autoptr leaked %d objects", Base::cnt_ - count);

#endif   // _RWSTD_NO_MEMBER_TEMPLATES

    if (!rw_enabled ("void"))
        rw_note (0, 0, 0, "auto_ptr<void> test disabled");
    else
        test_auto_ptr_void ();

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.auto.ptr",
                    0 /* no comment */,
                    run_test,
                    "|-no-conversions#",
                    &rw_opt_no_conversions,
                    (void*)0 /* sentinel */);
}
