/***************************************************************************
 *
 * 19.std.exceptions.cpp - test exercising [lib.std.exceptions]
 *
 * $Id: 19.std.exceptions.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2001-2008 Rogue Wave Software.
 *
 **************************************************************************/

#include <rw/_defs.h>
#include <stdexcept>

/**************************************************************************/


template <class Exception>
int test_ex_spec (Exception*, const char *str)
{
#ifndef _RWSTD_NO_EXCEPTIONS

    try {
        // also tests that exception objects can be constructed
        // from a const char* argument w/o <string> having been
        // explicitly #included first
        Exception e (str);
        (void)&e;
    }
    catch (...) {
        return 1;
    }

#else   // if defined (_RWSTD_NO_EXCEPTIONS);

    _RWSTD_UNUSED (str);

#endif   // _RWSTD_NO_EXCEPTIONS

    return 0;
}

/**************************************************************************/

#include <string>

/**************************************************************************/


#ifdef DRQS // BUG: bsl::xyz_error should have explicit constructors
// helpers to verify that each exception's ctor is explicit
// not defined since they must not be referenced if test is successful
void is_explicit (const std::logic_error&);
void is_explicit (const std::domain_error&);
void is_explicit (const std::invalid_argument&);
void is_explicit (const std::length_error&);
void is_explicit (const std::out_of_range&);
void is_explicit (const std::runtime_error&);
void is_explicit (const std::range_error&);
void is_explicit (const std::overflow_error&);
void is_explicit (const std::underflow_error&);
#endif

struct bogus_exception
{
    // also verifies that std::string is declared
    bogus_exception (const std::string&) { }
    bogus_exception (const char*) { }
};

// calls to the overoaded is_explicit (std::string ()) must resolve
// to this function since there must be no implicit conversion from
// std::string to any of the exception classes
void is_explicit (const bogus_exception&) { }

// exercise the ability to construct exception objects w/o
// explicitly #including <string> first; std::string must still
// be declared (but not necessarily defined)

#if !defined (_RWSTD_NO_NAMESPACE) && !defined (_RWSTD_NO_HONOR_STD)
   // declare a global function with the same name as exception
#  define TEST_NAMESPACE_DEF(T) void T ()
#else
#  define TEST_NAMESPACE_DEF(ignore) (void)0
#endif   // !_RWSTD_NO_NAMESPACE && !_RWSTD_NO_HONOR_STD


#ifndef _RWSTD_NO_PTR_EXCEPTION_SPEC
#  define _PTR_THROWS(spec)   _THROWS (spec)
#else   // if defined (_RWSTD_NO_PTR_EXCEPTION_SPEC)
   // throw specs on pointers to functions not implemented...
#  define _PTR_THROWS(ignore)
#endif   // _RWSTD_NO_PTR_EXCEPTION_SPEC


// verify that each name is declared in namespace std
// w/o polluting the global namespace
#define TEST_DEF(T)                                             \
        typedef std::T T ## _type;                              \
        TEST_NAMESPACE_DEF (T);                                 \
        /* construct an object */                               \
        std::T obj_ ## T ("std::" _RWSTD_STR (T));              \
        /* assign the address of object to std::exception* */   \
        e = &obj_ ## T;                                         \
        /* verify that assignment can't throw */                \
        std::T& (std::T::*p_assign_ ## T)(const std::T&)        \
            _PTR_THROWS(()) = &std::T::operator=;               \
        _RWSTD_UNUSED (p_assign_ ## T);                         \
        /* verify that what() can't throw */                    \
        const char* (std::T::*p_what_ ## T)() const             \
            _PTR_THROWS(()) = &std::T::what;                    \
        _RWSTD_UNUSED (p_what_ ## T);                           \
        /* verify ctor postcondition */                         \
        result = result << 1 | cmp (obj_ ## T.what (),          \
                                    "std::" _RWSTD_STR (T))


int cmp (const char *s1, const char *s2)
{
    for (; *s1 && *s1 == *s2; ++s1, ++s2);
    return *s2 - *s1;
}


// returns 0 on success; on failure returns a bitmap with one bit set
// for every exception that failed its ctor postcondition
static int test_exception_defs ()
{
    int result = 0;

    // used below to verify public inheritance from std::exception
    std::exception *e = 0;

    TEST_DEF (logic_error);
    TEST_DEF (domain_error);
    TEST_DEF (invalid_argument);
    TEST_DEF (length_error);
    TEST_DEF (out_of_range);
    TEST_DEF (runtime_error);
    TEST_DEF (range_error);
    TEST_DEF (overflow_error);
    TEST_DEF (underflow_error);

    _RWSTD_UNUSED (e);

#ifndef _RWSTD_NO_EXPLICIT

    // verify that each exceptions converting ctor is explicit
    // use a pointer since std::string need not be a complete class
    const char s[40] = "";
    const std::string *ps = _RWSTD_REINTERPRET_CAST (const std::string*, s);
    is_explicit (*ps);

    // verify that each exceptions converting ctor from const char*
    // (if one exists) is also explicit
    is_explicit (s);

#endif   // _RWSTD_NO_EXPLICIT

    return result;
}

#undef _PTR_THROWS

/**************************************************************************/

#include <rw_new.h>
#include <driver.h>
#include <cstddef>

/**************************************************************************/


template <class Exception>
int test_throw (Exception*, const char *str)
{
#ifndef _RWSTD_NO_EXCEPTIONS

    //////////////////////////////////////////////////////////////////

    try {
        throw Exception (str);
    }
    catch (const Exception &e) {
        // caught by const reference
        rw_assert (e.what () && !cmp (e.what (), str),
                   0, __LINE__,
                   "caught by const reference; %s::what() == %#s",
                   str, str);
    }
    catch (...) {
        rw_assert (false,  0, __LINE__,
                   "threw %s, caught an unknown exception", str);
    }

    //////////////////////////////////////////////////////////////////

    try {
        throw Exception (str);
    }
    catch (Exception e) {
        // caught by value
        rw_assert (e.what () && !cmp (e.what (), str),
                   0, __LINE__,
                   "caught by value; %s::what() == %#s",
                   str, str);
    }
    catch (...) {
        rw_assert (false, 0, __LINE__,
                   "threw %s, caught an unknown exception", str);
    }

    //////////////////////////////////////////////////////////////////

    const Exception ex (str);

    try {
        throw ex;
    }
    catch (Exception e) {
        // caught copy by value
        rw_assert (e.what () && !cmp (e.what (), str),
                   0, __LINE__,
                   "caught copy by value; %s::what() == %#s",
                   str, str);
    }
    catch (...) {
        rw_assert (false, 0, __LINE__,
                   "threw %s, caught an unknown exception", str);
    }

    //////////////////////////////////////////////////////////////////

    try {
        throw ex;
    }
    catch (std::exception &e) {
        // caught by non-const reference to a base class
        rw_assert (e.what () && !cmp (e.what (), str),
                   0, __LINE__,
                   "caught by non-const reference to base; %s::what() == %#s",
                   str, str);
    }
    catch (...) {
        rw_assert (false, 0, __LINE__,
                   "threw %s, caught an unknown exception", str);
    }

    //////////////////////////////////////////////////////////////////

    try {
        try {
            throw Exception (str);
        }
        catch (...) {
            // rethrown
            throw;
        }
    }
    catch (Exception e) {
        // rethrown object caught by value
        rw_assert (e.what () && !cmp (e.what (), str),
                   0, __LINE__,
                   "caught rethrown by value; %s::what() == %#s", str, str);
    }
    catch (...) {
        rw_assert (false, 0, __LINE__,
                   "threw %s, caught an unknown exception", str);
    }

    //////////////////////////////////////////////////////////////////

    try {
        try {
            throw Exception (str);
        }
        catch (Exception e) {
            rw_assert (e.what () && !cmp (e.what (), str),
                       0, __LINE__,
                       "caught by value; %s::what() == %#s",
                       str, str);

            // rethrown by value
            throw e;
        }
    }
    catch (Exception e) {
        // rethrown object caught by value
        rw_assert (e.what () && !cmp (e.what (), str),
                   0, __LINE__,
                   "caught rethrown copy by value; %s::what() == %#s",
                   str, str);
    }
    catch (...) {
        rw_assert (false, 0, __LINE__,
                   "threw %s, caught an unknown exception", str);
    }

#else   // if defined (_RWSTD_NO_EXCEPTIONS);

    _RWSTD_UNUSED (t);
    _RWSTD_UNUSED (str);

#endif   // _RWSTD_NO_EXCEPTIONS

    return 0;
}

/**************************************************************************/


static int
run_test (int, char* [])
{
    // verify that exception's (and its derivatives') ctor is explicit
    is_explicit (std::string ());

    const char* const names[] = {
        "logic_error", "domain_error", "invalid_argument", "length_error",
        "out_of_range", "runtime_error", "range_error", "overflow_error",
        "underflow_error"
    };

    // if set, each bit corresponds to a failed exception assertion
    const int failures = test_exception_defs ();

    for (unsigned i = 0; i != sizeof names / sizeof *names; ++i) {
        rw_assert (!(failures & (1 << i)), 0, __LINE__,
                   "std::%s::%s (const std::string&) postcondition",
                    names [i]);
    }


    // have replacement operator new throw an exception
    rwt_free_store* const pst = rwt_get_free_store (0);
    *pst->throw_at_calls_ [0] = pst->new_calls_ [0] + 1;

    // create a very long string to guarantee that exception ctors
    // try to dynamically allocate storage even if it otherwise
    // use some clever scheme to avoid doing so
    char long_str [0x10000];
    for (unsigned i = 0; i != sizeof long_str - 1; ++i)
        long_str [i] = '*';

    long_str [sizeof long_str - 1] = '\0';

    int new_throws = 0;

    _TRY {
        // see if replacement operator new throws an exception
        // when called directly from the program
        void *p = ::operator new (sizeof long_str);
        ::operator delete (p);
        new_throws = 0;

#ifdef _RWSTD_NO_REPLACEABLE_NEW_DELETE

        // MSVC and VAC++ don't reliably replace operators
        // new and delete across shared librray boundaries

        rw_warn (false, 0, __LINE__,
                 "replacement ::operator new(std::size_t = %u) failed "
                 "to throw when called directly from a program: this "
                 "is an expected failure on this platform",
                 sizeof long_str);

#else   // if !defined (_RWSTD_NO_REPLACEABLE_NEW_DELETE)

        rw_assert (false, 0, __LINE__,
                   "replacement ::operator new(std::size_t = %u) "
                   "unexpectdly failed to throw when called directly "
                   "from a program",
                   sizeof long_str);

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    }
    _CATCH (...) {
        new_throws = 1;
    }

    *pst->throw_at_calls_ [0] = pst->new_calls_ [0] + 1;

    _TRY {
        // see if replacement operator new throws an exception
        // when called indirectly, i.e., from the library binary
        void* const p = _RW::__rw_allocate (sizeof long_str, 0);
        _RW::__rw_deallocate (p, 0);
        new_throws = 0;

#ifdef _RWSTD_NO_REPLACEABLE_NEW_DELETE

        // MSVC and VAC++ don't reliably replace operators
        // new and delete across shared librray boundaries

        rw_warn (false, 0, __LINE__,
                 "replacement ::operator new(std::size_t = %u) failed "
                 "to throw when called from the library: this is an "
                 "expected failure on this platform",
                 sizeof long_str);

#else   // if !defined (_RWSTD_NO_REPLACEABLE_NEW_DELETE)

        rw_assert (false, 0, __LINE__,
                   "replacement ::operator new(std::size_t = %u) "
                   "unexpectdly failed to throw when called from "
                   "the library",
                   sizeof long_str);

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    }
    _CATCH (...) {
        new_throws = 1;
    }

// exercise exception specification on class ctors
// and the ability to throw and catch exception objects
#define TEST_EX_SPEC(T) do {                                              \
   /* do not induce an exception from replacement operator new */         \
   *pst->throw_at_calls_ [0] = std::size_t (-1);                          \
   *pst->throw_at_calls_ [1] = std::size_t (-1);                          \
   /* verify that exception objects can be thrown, caught, and rethrown */\
   test_throw ((std:: T*)0, "std::" #T);                                  \
   /* induce an exception from replacement operator new */                \
   *pst->throw_at_calls_ [0] = pst->new_calls_ [0] + 1;                   \
   *pst->throw_at_calls_ [1] = pst->new_calls_ [1] + 1;                   \
   /* verify that each exception ctor propagates the exception thrown */  \
   /* from operator new(); failure wil cause a call to terminate() */     \
   /* tests are expected to silently fail if (0 == new_throws) holds  */  \
   const int threw = test_ex_spec ((std::T*)0, long_str);                 \
   rw_assert (threw == new_throws, 0, __LINE__,                           \
              "attempthing to construct a std::" #T " %s an exception",   \
               new_throws ? "failed to rethrow" : "unexpectedly threw");  \
  } while (0)

    TEST_EX_SPEC (logic_error);
    TEST_EX_SPEC (domain_error);
    TEST_EX_SPEC (invalid_argument);
    TEST_EX_SPEC (length_error);
    TEST_EX_SPEC (out_of_range);
    TEST_EX_SPEC (runtime_error);
    TEST_EX_SPEC (range_error);
    TEST_EX_SPEC (overflow_error);
    TEST_EX_SPEC (underflow_error);

    return 0;
}

/**************************************************************************/


int main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.std.exceptions",
                    0 /* no comment */,
                    run_test,
                    "",
                    (void*)0);
}
