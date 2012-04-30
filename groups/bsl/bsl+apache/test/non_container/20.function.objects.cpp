/***************************************************************************
 *
 * 20.function.objects.cpp - test exercising [lib.std.function_objects]
 *
 * $Id: 20.function.objects.cpp 426701 2006-07-28 23:19:41Z sebor $
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
 * Copyright 2000-2006 Rogue Wave Software.
 *
 **************************************************************************/

#include <functional>
#include <driver.h>

#if defined (__HP_aCC) && _RWSTD_HP_aCC_MAJOR < 6

// working around an HP aCC bug (see PR #25378)
#  include <string>

// working around an HP aCC bug (see PR #24417)
_USING (namespace std);

#endif   // HP aCC < 6

// ***** FOR BLOOMBERG, USE bsl INSTEAD OF std *****
#define std bsl

/**************************************************************************/

#ifdef _MSC_VER
   // shut up the idiotic MSVC 6.0 warning C4099:
   //   type name first seen using 'struct' now seen using  'class'
#  pragma warning (disable: 4099)

   // another bogus warning C4700:
   //   local variable used without having been initialized
#  pragma warning (disable: 4700)

#  if _MSC_VER <= 1300
     // work around a bug where the compiler thinks that what's a struct
     // is really a class with all private members (PR #23795)
#    define class struct
#  endif
#endif


#ifndef _RWSTD_EXPLICIT_INSTANTIATION

   // explicitly instantiate

#  if    !defined (_RWSTD_NO_NAMESPACE) && !defined (_RWSTD_NO_HONOR_STD) \
      && (!defined (_COMPILER_VERSION) || _COMPILER_VERSION > 730)        \
      && (!defined (__EDG_VERSION__) || __EDG_VERSION__ > 244)

     // work around an EDG front end bug (see, for example, PR #25292)

     // verify that names are declared [only] in namespace std

#    define TEST_CLASS_DEF(name, Tparam)                          \
            template class std::name Tparam;                      \
            void name (void *name)
#    define TEST_CLASS_DEF_2(name, Tparam1, Tparam2)              \
            template class std::name Tparam1, Tparam2;            \
            void name (void *name)
#    define TEST_CLASS_DEF_3(name, Tparam1, Tparam2, Tparam3)     \
            template class std::name Tparam1, Tparam2, Tparam3;   \
            void name (void *name)

#  else   // if defined (_RWSTD_NO_NAMESPACE) || defined (_RWSTD_NO_HONOR_STD)

     // verify that names do not collide with function argument names

#    define TEST_CLASS_DEF(name, Tparam)                          \
            template class std::name Tparam;                      \
            void foo (void *name)
#    define TEST_CLASS_DEF_2(name, Tparam1, Tparam2)              \
            template class std::name Tparam1, Tparam2;            \
            void foo (void *name)
#    define TEST_CLASS_DEF_3(name, Tparam1, Tparam2, Tparam3)     \
            template class std::name Tparam1, Tparam2, Tparam3;   \
            void foo (void *name)
#  endif   // !_RWSTD_NO_NAMESPACE && !_RWSTD_NO_HONOR_STD

#  if defined __GNUG__ && __GNUG__ < 3 && __GNU_MINOR__ < 96

     // working around a bug in g++ 2.95.2 (PR #23151)
#    define TEST_FUNCTION_DEF(decl)        \
            _RWSTD_NAMESPACE (std) {       \
               template decl;              \
            }   /* namespace std */        \
            typedef void unused_typedef
#  else
#    define TEST_FUNCTION_DEF(decl)   template decl
#  endif

#else   // if defined (_RWSTD_EXPLICIT_INSTANTIATION)

   // classes will implicitly instantiated below

#  if !defined (_RWSTD_NO_NAMESPACE) && !defined (_RWSTD_NO_HONOR_STD)

     // verify that names are declared [only[ in namespace std

#    define TEST_CLASS_DEF(name, ignore)             void name (void *name)
#    define TEST_CLASS_DEF_2(name, ign1, ign2)       void name (void *name)
#    define TEST_CLASS_DEF_3(name, ign1, ign2, ign3) void name (void *name)

#  else   // if defined (_RWSTD_NO_NAMESPACE) || defined (_RWSTD_NO_HONOR_STD)

#    define TEST_CLASS_DEF(name, ignore)             void foo (void *name)
#    define TEST_CLASS_DEF_2(name, ign1, ign2)       void foo (void *name)
#    define TEST_CLASS_DEF_3(name, ign1, ign2, ign3) void foo (void *name)

#  endif   // !_RWSTD_NO_NAMESPACE && !_RWSTD_NO_HONOR_STD

#  define TEST_FUNCTION_DEF(ignore)   void unused_typedef

#endif   // _RWSTD_EXPLICIT_INSTANTIATION


TEST_CLASS_DEF_2 (unary_function, <int, int>);
TEST_CLASS_DEF_3 (binary_function, <int, int, int>);

TEST_CLASS_DEF (plus, <int>);
TEST_CLASS_DEF (minus, <int>);
TEST_CLASS_DEF (multiplies, <int>);
TEST_CLASS_DEF (divides, <int>);
TEST_CLASS_DEF (modulus, <int>);
TEST_CLASS_DEF (negate, <int>);

TEST_CLASS_DEF (equal_to, <int>);
TEST_CLASS_DEF (not_equal_to, <int>);
TEST_CLASS_DEF (greater, <int>);
TEST_CLASS_DEF (less, <int>);
TEST_CLASS_DEF (greater_equal, <int>);
TEST_CLASS_DEF (less_equal, <int>);

TEST_CLASS_DEF (logical_and, <int>);
TEST_CLASS_DEF (logical_or, <int>);
TEST_CLASS_DEF (logical_not, <int>);

TEST_CLASS_DEF (unary_negate, <std::negate<int> >);

TEST_FUNCTION_DEF (
    std::unary_negate<std::negate<int> >
    std::not1 (const std::negate<int>&));

TEST_CLASS_DEF (binary_negate, <std::equal_to<int> >);

TEST_FUNCTION_DEF (
    std::binary_negate<std::equal_to<int> >
    std::not2 (const std::equal_to<int>&));

TEST_CLASS_DEF (binder1st, <std::plus<int> >);

TEST_FUNCTION_DEF (
    std::binder1st<std::plus<int> >
    std::bind1st (const std::plus<int>&, const int&));

TEST_CLASS_DEF (binder2nd , <std::plus<int> >);

TEST_FUNCTION_DEF (
    std::binder2nd<std::plus<int> >
    std::bind2nd (const std::plus<int>&, const int&));

/**************************************************************************/

// exercise 20.3.1 [lib.base]
static void
test_base ()
{
    rw_info (0, 0, __LINE__, "[lib.base]");

    // verify that member types really are of the correct types
    std::unary_function<int, void>::argument_type *argument = (int*)0;
    std::unary_function<int, void>::result_type *result = (void*)0;

    std::binary_function<int, char, void>::first_argument_type *a1 = (int*)0;
    std::binary_function<int, char, void>::second_argument_type *a2 = (char*)0;
    std::binary_function<int, char, void>::result_type *res = (void*)0;

    _RWSTD_UNUSED (argument);
    _RWSTD_UNUSED (result);
    _RWSTD_UNUSED (a1);
    _RWSTD_UNUSED (a2);
    _RWSTD_UNUSED (res);
}


#define TEST_UNARY_OP(fun, op, x) do {                                 \
    rw_info (0, 0, __LINE__, "std::" #fun);                            \
    N::fun f CTOR_ARG_LIST;                                            \
    /* verify that fun is copy constructible and assignable */         \
    const N::fun f_cpy1 (f);                                           \
    /* const to verify copy ctor const correctness */                  \
    const N::fun f_cpy2 = f;                                           \
    /* verify that fun publicly derives from unary_function */         \
    const std::unary_function<N::fun::argument_type,                   \
                              N::fun::result_type> *pf = &f;           \
    _RWSTD_UNUSED (pf);                                                \
    /* verify member types and signature of operator () */             \
    typedef N::fun fun_t;  /* in case fun contains commas */           \
    N::fun::result_type (N::fun::*pfun)                                \
        UNARY_OP_ARG_LIST (fun_t) const = &N::fun::operator();         \
    _RWSTD_UNUSED (pfun);                                              \
    /* exercise operator() */                                          \
    rw_assert (f.operator() (x) == (op x), 0, __LINE__,                \
               #fun "().operator()(%i) == %i, got %i",                 \
               x, (op x), f.operator()(x));                            \
    /* exercise operator() of the two copies */                        \
    rw_assert (f_cpy1.operator() (x) == (op x), 0, __LINE__,           \
               #fun "::" #fun "(const " #fun "&)");                    \
    rw_assert (f_cpy2.operator() (x) == (op x), 0, __LINE__,           \
               #fun "::operator=" #fun "(const " #fun "&)");           \
  } while (0)

#define TEST_BINARY_OP(fun, op, x, y) do {                             \
    rw_info (0, 0, __LINE__, "std::" #fun);                            \
    N::fun f CTOR_ARG_LIST;                                            \
    /* verify that fun is copy constructible and assignable */         \
    const N::fun f_cpy1 (f);                                           \
    /* const to verify copy ctor const correctness */                  \
    const N::fun f_cpy2 = f;                                           \
    /* verify that fun publicly derives from binary_function */        \
    const std::binary_function<N::fun::first_argument_type,            \
                               N::fun::second_argument_type,           \
                               N::fun::result_type> *pf = &f;          \
    _RWSTD_UNUSED (pf);                                                \
    /* verify member types and signature of operator () */             \
    typedef N::fun fun_t;  /* in case fun contains commas */           \
    N::fun::result_type (N::fun::*pfun)                                \
        BINARY_OP_ARG_LIST (fun_t) const = &N::fun::operator();        \
    _RWSTD_UNUSED (pfun);                                              \
    rw_assert (f.operator() (x, y) == (x op y), 0, __LINE__,           \
               #fun "().operator()(%i, %i) == %i, got %i",             \
               x, y, (x op y), f.operator()(x, y));                    \
    /* exercise operator() of the two copies */                        \
    rw_assert (f_cpy1.operator() (x, y) == (x op y), 0, __LINE__,      \
               #fun "::" #fun "(const " #fun "&)");                    \
    rw_assert (f_cpy2.operator() (x, y) == (x op y), 0, __LINE__,      \
               #fun "::operator=" #fun "(const " #fun "&)");           \
  } while (0)


// tested fununctions are in namespace std
#define N std

// most but not all operator() take arguments by const reference
#define UNARY_OP_ARG_LIST(fun) (const fun::argument_type&)
#define BINARY_OP_ARG_LIST(fun)   \
        (const fun::first_argument_type&, const fun::second_argument_type&)

// use default ctor in tests below
#define CTOR_ARG_LIST

// exercise 20.3.2 [lib.arithmetic.operations]
static void
test_arithmetic_operations ()
{
    rw_info (0, 0, __LINE__, "[lib.arithmetic.operations]");

    // 20.3.2, p2
    TEST_BINARY_OP (plus<int>,       +, 1, 2);

    // 20.3.2, p3
    TEST_BINARY_OP (minus<int>,      -, 3, 2);

    // 20.3.2, p4
    TEST_BINARY_OP (multiplies<int>, *, 2, 3);

    // 20.3.2, p5
    TEST_BINARY_OP (divides<int>,    /, 6, 3);

    // 20.3.2, p6
    TEST_BINARY_OP (modulus<int>,    %, 7, 3);

    // 20.3.2, p7
    TEST_UNARY_OP (negate<int>,      -, 1);

    // exercise extensions

// tested fununctions are in a private namespace
#undef N
#define N _RW

#if TEST_RW_EXTENSIONS
    TEST_UNARY_OP (identity<int>,           +, 3);
    TEST_UNARY_OP (unary_plus<int>,         +, 4);
    TEST_UNARY_OP (bitwise_complement<int>, ~, 5);

    TEST_BINARY_OP (bitwise_and<int>,       &, 0x0f, 0x05);
    TEST_BINARY_OP (bitwise_or<int>,        |, 0x0f, 0xf0);
    TEST_BINARY_OP (exclusive_or<int>,      ^, 0x0f, 0xf1);

    TEST_BINARY_OP (shift_left<int>,        <<,  3, 3);
    TEST_BINARY_OP (shift_right<int>,       >>, 15, 3);
#endif // TEST_RW_EXTENSIONS

// remaining tested fununctions are in namespace std
#undef N
#define N std
}

/**************************************************************************/

// exercise 20.3.3 [lib.comparisons]
static void
test_comparisons ()
{
    rw_info (0, 0, __LINE__, "[lib.comparisons]");

    // 20.3.3, p2
    TEST_BINARY_OP (equal_to<int>,         ==, 1, 1);
    TEST_BINARY_OP (equal_to<double>,      ==, 1.0, 2.0);

    // 20.3.3, p3
    TEST_BINARY_OP (not_equal_to<int>,     !=, 1, 1);
    TEST_BINARY_OP (not_equal_to<double>,  !=, 1.0, 2.0);

    // 20.3.3, p4
    TEST_BINARY_OP (greater<int>,          >,  1, 1);
    TEST_BINARY_OP (greater<double>,       >,  2.0, 1.0);

    // 20.3.3, p5
    TEST_BINARY_OP (less<int>,             <,  1, 1);
    TEST_BINARY_OP (less<double>,          <,  1.0, 2.0);

    // 20.3.3, p6
    TEST_BINARY_OP (greater_equal<int>,    >=, 1, 1);
    TEST_BINARY_OP (greater_equal<double>, >=, 1.0, 2.0);

    // 20.3.3, p7
    TEST_BINARY_OP (less_equal<int>,       <=, 1, 1);
    TEST_BINARY_OP (less_equal<double>,    <=, 2.0, 1.0);

    // 20.3.3, p8
    // ???
}

/**************************************************************************/

// exercise 20.3.4 [lib.logical.operations]
static void
test_logical_operations ()
{
    rw_info (0, 0, __LINE__, "[lib.logical.operations]");

    // 20.3.4, p2
    TEST_BINARY_OP (logical_and<int>,    &&, 0, 1);
    TEST_BINARY_OP (logical_and<double>, &&, 1.0, 2.0);

    // 20.3.4, p3
    TEST_BINARY_OP (logical_or<char>,    ||, '\0', '\0');
    TEST_BINARY_OP (logical_or<int>,     ||, 0, 1);
    TEST_BINARY_OP (logical_or<double>,  ||, 1.0, 2.0);

    // 20.3.4, p4
    TEST_UNARY_OP  (logical_not<int>,    !,  0);
    TEST_UNARY_OP  (logical_not<double>, !,  2.0);
}


// helpers to verify that each class' ctor is explicit
// not defined since they must not be referenced if test is successful
void is_explicit (const std::unary_negate<std::logical_not<int> >&);
void is_explicit (const std::binary_negate<std::equal_to<int> >&);
void is_explicit (const std::pointer_to_unary_function<int, int>&);
void is_explicit (const std::pointer_to_binary_function<int, int, int>&);

struct has_implicit_ctor
{
    // NOT explicit
    has_implicit_ctor (const std::logical_not<int>&) { }
    has_implicit_ctor (const std::equal_to<int>&) { }

    has_implicit_ctor (int (*)(int)) { }
    has_implicit_ctor (int (*)(int, int)) { }
};

/**************************************************************************/

void is_explicit (const has_implicit_ctor&) { }


// exercise 20.3.5 [lib.negators]
static void
test_negators ()
{
    rw_info (0, 0, __LINE__, "[lib.negators]");

#ifndef _RWSTD_NO_EXPLICIT

    // verify that std::unary_negate<>() and std::binary_negate<>()
    // ctors are declared explicit
    is_explicit (std::logical_not<int>());

#endif   // _RWSTD_NO_EXPLICIT

// use std::negate<> as an argument in negator ctors
#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST \
    = std::unary_negate<std::logical_not<int> > (std::logical_not<int> ())

    // 20.3.5, p2
    TEST_UNARY_OP (unary_negate<std::logical_not<int> >, !!, 0);
    TEST_UNARY_OP (unary_negate<std::logical_not<int> >, !!, 1);

// exercise the convenience function template std::not1()
#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST = std::not1 (std::logical_not<char> ())

    // 20.3.5, p3
    TEST_UNARY_OP (unary_negate<std::logical_not<char> >, !!, '\0');
    TEST_UNARY_OP (unary_negate<std::logical_not<char> >, !!, '1');

// use std::equal_to<> as an argument in negator ctors
#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST \
    = std::binary_negate<std::equal_to<double> > (std::equal_to<double> ())

    // 20.3.5, p4
    TEST_BINARY_OP (binary_negate<std::equal_to<double> >, !=, 0.0, 1.0);
    TEST_BINARY_OP (binary_negate<std::equal_to<double> >, !=, 1.0, 2.0);

// use std::not_equal_to<> as an argument in negator ctors
#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST \
    = std::binary_negate<std::not_equal_to<int> > (std::not_equal_to<int> ())

    TEST_BINARY_OP (binary_negate<std::not_equal_to<int> >, ==, 0, 1);
    TEST_BINARY_OP (binary_negate<std::not_equal_to<int> >, ==, 1, 2);

// exercise the convenience function template std::not2()
#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST = std::not2 (std::equal_to<int> ())

    // 20.3.5, p5
    TEST_BINARY_OP (binary_negate<std::equal_to<int> >, !=, 2, 2);
    TEST_BINARY_OP (binary_negate<std::equal_to<int> >, !=, 3, 4);
}

/**************************************************************************/

// exercise 20.3.6 [lib.binders]
static void
test_binders ()
{
    rw_info (0, 0, __LINE__, "[lib.binders]");

#ifdef DRQS // STLPort bug: op and value are incorrectly named _M_op & _M_value
    // make sure the protected names `op' and `value' are accessible
    struct binder1st_derivative: std::binder1st<std::minus<int> > {

        binder1st_derivative ()
            : std::binder1st<std::minus<int> >(std::minus<int> (), 0) {

            // test protected access
            rw_assert (0 == op (value, value), 0, __LINE__,
                       "binder1st<>::op");
            rw_assert (0 == operator()(value), 0, __LINE__,
                       "binder1st<>::operator()");

            value = 7;
            rw_assert (-7 == op (0, value), 0, __LINE__,
                       "binder1st<>::value");
            rw_assert (7 == operator()(0), 0, __LINE__,
                       "binder1st<>::operator()");
        }
    } b1st;

    _RWSTD_UNUSED (b1st);

    // make sure the protected names `op' and `value' are accessible
    struct binder2nd_derivative: std::binder2nd<std::minus<int> > {
        binder2nd_derivative ()
            : std::binder2nd<std::minus<int> >(std::minus<int> (), 0) {

            // test protected access
            rw_assert (0 == op (value, value), 0, __LINE__,
                       "binder2nd<>::op");
            rw_assert (0 == operator()(value), 0, __LINE__,
                       "binder2nd<>::operator()");

            value = 7;
            rw_assert (-7 == op (0, value), 0, __LINE__,
                       "binder2nd<>::value");
            rw_assert (-7 == operator()(0), 0, __LINE__,
                       "binder2nd<>::operator()");
        }
    } b2nd;

    _RWSTD_UNUSED (b2nd);
#endif // DRQS

#undef CTOR_ARG_LIST

    // broken out of the macro definition to work around
    // a bug in g++ 2.95.2 parser
    std::minus<int> obj_minus;

// use std::negate<> as an argument in negator ctors
#define CTOR_ARG_LIST (obj_minus, 1)

    // 20.3.6.1, p1 and p2
    TEST_UNARY_OP (binder1st<std::minus<int> >, 1 -, 0);
    TEST_UNARY_OP (binder1st<std::minus<int> >, 1 -, 1);

    // 20.3.6.3, p1 and p2
    TEST_UNARY_OP (binder2nd<std::minus<int> >, -1 +, 0);
    TEST_UNARY_OP (binder2nd<std::minus<int> >, 1 -, 1);

// exercise the convenience function template std::bind1st<>()
#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST = std::bind1st (std::divides<int>(), 6)

    // 20.3.6.2, p1
    TEST_UNARY_OP (binder1st<std::divides<int> >, 6 /, 3);
    TEST_UNARY_OP (binder1st<std::divides<int> >, 6 /, 2);

// exercise the convenience function template std::bind2nd<>()
#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST = std::bind2nd (std::plus<int>(), -2)

    // 20.3.6.4, p1
    TEST_UNARY_OP (binder2nd<std::plus<int> >, -2 +, 0);
    TEST_UNARY_OP (binder2nd<std::plus<int> >, -2 +, 1);
}

/**************************************************************************/

// helpers to exercise pointer to function adapters
int square (int i)        { return i * i; }
int shift  (int i, int n) { return i << n; }

// exercise 20.3.7 [lib.function.pointer.adaptors]
static void
test_function_pointer_adaptors ()
{
    rw_info (0, 0, __LINE__, "[lib.function.pointer.adaptors]");

#ifndef _RWSTD_NO_EXPLICIT

    // verify that the pointer to function adapters' ctors are explicit
    is_explicit ((int (*)(int))0);
    is_explicit ((int (*)(int, int))0);

#endif   // _RWSTD_NO_EXPLICIT

// initialize pointer_to_unary_function with the address of square
#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST (&::square)

// operator() takes a value (as opposed to const reference)
#undef UNARY_OP_ARG_LIST
#define UNARY_OP_ARG_LIST(fun) (fun::argument_type)

#define pointer_to_unary_function_int pointer_to_unary_function<int, int>

    // 20.3.7, p2
    TEST_UNARY_OP (pointer_to_unary_function_int, 4 *, 4);

// exercise the convenience function template ptr_fun()
#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST = std::ptr_fun (&::square)

    // 20.3.7, p3
    TEST_UNARY_OP (pointer_to_unary_function_int, 5 *, 5);

// initialize pointer_to_binary_function with the address of shift
#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST (&::shift)

// operator() takes values (as opposed to const references)
#undef BINARY_OP_ARG_LIST
#define BINARY_OP_ARG_LIST(fun) \
       (fun::first_argument_type, fun::second_argument_type)

#define pointer_to_binary_function_int pointer_to_binary_function<int, int, int>

    // 20.3.7, p4
    TEST_BINARY_OP (pointer_to_binary_function_int, <<, 1, 4);

// exercise the convenience function template ptr_fun()
#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST = std::ptr_fun (&::shift)

    // 20.3.7, p5
    TEST_BINARY_OP (pointer_to_binary_function_int, <<, 2, 5);
}

/**************************************************************************/

struct Integer
{
    int i_;

    // for convenience
    operator int () const { return i_; }

    // non-const members
    int square () { return i_ * i_; }
    int div (short n) { return i_ / n; }

    // const versions of the above members
    int const_square () const { return i_ * i_; }
    int const_div (short n) const { return i_ / n; }
};


// helper to verify that each class' ctor is explicit
// not defined since they must not be referenced if test is successful
void is_explicit (const std::mem_fun_t<int, Integer>&);

struct has_implicit_ctor_from_member
{
    // NOT explicit
    has_implicit_ctor_from_member (int (Integer::*)()) { }
    has_implicit_ctor_from_member (int (Integer::*)(short)) { }

    has_implicit_ctor_from_member (int (Integer::*)() const) { }
    has_implicit_ctor_from_member (int (Integer::*)(short) const) { }
};

void is_explicit (const has_implicit_ctor_from_member&) { }


// exercise 20.3.8 [lib.member.pointer.adaptors]
static void
test_member_pointer_adaptors ()
{
    rw_info (0, 0, __LINE__, "[lib.member.pointer.adaptors]");

#ifndef _RWSTD_NO_EXPLICIT

    // verify that the pointer to member adapters' ctors are explicit
    is_explicit ((int (Integer::*)())0);
    is_explicit ((int (Integer::*)(short))0);
    is_explicit ((int (Integer::*)() const)0);
    is_explicit ((int (Integer::*)(short) const)0);

#endif   // _RWSTD_NO_EXPLICIT

#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST (&Integer::square)

#define mem_fun_t_Integer_int       mem_fun_t<int, Integer>

    Integer int_obj = { 5 };

    // 20.3.8, p2
    TEST_UNARY_OP (mem_fun_t_Integer_int, 5 * (int)*, &int_obj);

#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST = std::mem_fun (&Integer::square)

    int_obj.i_ = 7;

    // 20.3.8, p4
    TEST_UNARY_OP (mem_fun_t_Integer_int, 7 * (int)*, &int_obj);


#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST (&Integer::div)

#define mem_fun1_t_int_Integer_short       mem_fun1_t<int, Integer, short>

    // 20.3.8, p3
    TEST_BINARY_OP (mem_fun1_t_int_Integer_short, ->operator int() /,
                    (&int_obj), 2);

#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST = std::mem_fun (&Integer::div)

    // 20.3.8, p4
    TEST_BINARY_OP (mem_fun1_t_int_Integer_short, ->operator int() / ,
                    (&int_obj), -1);


#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST (&Integer::square)

// operator() takes a reference (as opposed to const reference or value)
#undef UNARY_OP_ARG_LIST
#define UNARY_OP_ARG_LIST(fun) (fun::argument_type&)

#define mem_fun_ref_t_Integer_int   mem_fun_ref_t<int, Integer>

    int_obj.i_ = -9;

    // 20.3.8, p5
    TEST_UNARY_OP (mem_fun_ref_t_Integer_int, -9 * (int), int_obj);

// exercise the convenience function template mem_fun_ref()
#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST = std::mem_fun_ref (&Integer::square)

    // 20.3.8, p7
    TEST_UNARY_OP (mem_fun_ref_t_Integer_int, -9 * (int), int_obj);

#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST (&Integer::div)

// operator() takes a reference and a value
#undef BINARY_OP_ARG_LIST
#define BINARY_OP_ARG_LIST(fun) \
       (fun::first_argument_type&, fun::second_argument_type)

#define mem_fun1_ref_t_int_Integer_short   mem_fun1_ref_t<int, Integer, short>

    // 20.3.8, p6
    TEST_BINARY_OP (mem_fun1_ref_t_int_Integer_short, .operator int() /,
                    int_obj, 3);

// exercise the convenience function template mem_fun_ref()
#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST = std::mem_fun_ref (&Integer::div)

    // 20.3.8, p7
    TEST_BINARY_OP (mem_fun1_ref_t_int_Integer_short, .operator int() /,
                    int_obj, 3);

/***************************************************************************/

#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST (&Integer::const_square)

// operator() takes a const reference (as opposed to reference or value)
#undef UNARY_OP_ARG_LIST
#define UNARY_OP_ARG_LIST(fun) (fun::argument_type)

#define const_mem_fun_t_Integer_int   const_mem_fun_t<int, Integer>

    int_obj.i_ = 5;

    // verify const-correctness
    const Integer &int_cref = int_obj;

    // 20.3.8, p2
    TEST_UNARY_OP (const_mem_fun_t_Integer_int, 5 * (int)*, &int_cref);

#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST = std::mem_fun (&Integer::const_square)

    int_obj.i_ = 7;

    // 20.3.8, p4
    TEST_UNARY_OP (const_mem_fun_t_Integer_int, 7 * (int)*, &int_cref);


#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST (&Integer::const_div)

// operator() takes a const value and a value
#undef BINARY_OP_ARG_LIST
#define BINARY_OP_ARG_LIST(fun) \
       (fun::first_argument_type, fun::second_argument_type)

#define const_mem_fun1_t_int_Integer_short   \
        const_mem_fun1_t<int, Integer, short>

    // 20.3.8, p3
    TEST_BINARY_OP (const_mem_fun1_t_int_Integer_short, ->operator int() /,
                    (&int_cref), 2);

#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST = std::mem_fun (&Integer::const_div)

    // 20.3.8, p4
    TEST_BINARY_OP (const_mem_fun1_t_int_Integer_short, ->operator int() / ,
                    (&int_cref), -1);


#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST (&Integer::const_square)

// operator() takes a reference (as opposed to const reference or value)
#undef UNARY_OP_ARG_LIST
#define UNARY_OP_ARG_LIST(fun) (const fun::argument_type&)

#define const_mem_fun_ref_t_Integer_int   const_mem_fun_ref_t<int, Integer>

    int_obj.i_ = -9;

    // 20.3.8, p5
    TEST_UNARY_OP (const_mem_fun_ref_t_Integer_int, -9 * (int), int_cref);

// exercise the convenience function template const_mem_fun_ref()
#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST = std::mem_fun_ref (&Integer::const_square)

    // 20.3.8, p7
    TEST_UNARY_OP (const_mem_fun_ref_t_Integer_int, -9 * (int), int_cref);

#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST (&Integer::const_div)

// operator() takes a const reference and a value
#undef BINARY_OP_ARG_LIST
#define BINARY_OP_ARG_LIST(fun) \
       (const fun::first_argument_type&, fun::second_argument_type)

#define const_mem_fun1_ref_t_int_Integer_short   \
        const_mem_fun1_ref_t<int, Integer, short>

    // 20.3.8, p6
    TEST_BINARY_OP (const_mem_fun1_ref_t_int_Integer_short, .operator int() /,
                    int_cref, 3);

// exercise the convenience function template const_mem_fun_ref()
#undef CTOR_ARG_LIST
#define CTOR_ARG_LIST = std::mem_fun_ref (&Integer::const_div)

    // 20.3.8, p7
    TEST_BINARY_OP (const_mem_fun1_ref_t_int_Integer_short, .operator int() /,
                    int_cref, 3);
}

/**************************************************************************/

static int
run_test (int, char**)
{
    // exercise 20.3.1 [lib.base]
    test_base ();

    // exercise 20.3.2 [lib.arithmetic.operations]
    test_arithmetic_operations ();

    // exercise 20.3.3 [lib.comparisons]
    test_comparisons ();

    // exercise 20.3.4 [lib.logical.operations]
    test_logical_operations ();

    // exercise 20.3.5 [lib.negators]
    test_negators ();

    // exercise 20.3.6 [lib.binders]
    test_binders ();

    // exercise 20.3.7 [lib.function.pointer.adaptors]
    test_function_pointer_adaptors ();

    // exercise 20.3.8 [lib.member.pointer.adaptors]
    test_member_pointer_adaptors ();

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.std.function_objects",
                    0 /* no comment */,
                    run_test,
                    "",
                    (void*)0 /* sentinel */);
}
