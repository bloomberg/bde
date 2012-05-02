/***************************************************************************
 *
 * exception.cpp - test exercising [lib.support.exception]
 *
 * $Id: 18.exception.cpp 587886 2007-10-24 13:02:05Z faridz $
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
 * Copyright 2001-2007 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#if TEST_RW_EXTENSIONS

#ifdef __SUNPRO_CC
    // working around a SunPro/SunOS 5.8 bug (PR #26255)
#  include <time.h>
#endif   // __SUNPRO_CC

#include <exception>      // for bad_exception, exception
#include <ios>            // for ios_base::failure
#include <new>            // for bad_alloc
#include <stdexcept>      // for exception classes
#include <typeinfo>       // for bad_cast, bad_typeid

#include <csignal>        // for signal(), SIGABRT
#include <cstdio>         // for size_t, sprintf()
#include <cstring>        // for strcmp(), strlen()

#include <rw/_error.h>    // for _RWSTD_ERROR_XXX constants


#ifndef _RWSTD_NO_SETRLIMIT
# include <sys/resource.h>   // for setrlimit()
#endif   // _RWSTD_NO_SETRLIMIT

#include <driver.h>


#ifdef _RWSTD_OS_LINUX
   // use siglongjmp() and sigsetjmp() on Linux to avoid
   // http://sourceware.org/bugzilla/show_bug.cgi?id=2351
#  include <setjmp.h>    // for siglongjmp(), sigsetjmp()

#  define RW_JMP_BUF             jmp_buf
#  define RW_SETJMP(env)         sigsetjmp (env, 0)
#  define RW_LONGJMP(env, val)   siglongjmp (env, val)
#else   // if !defined (_RWSTD_OS_LINUX)
#  include <csetjmp>    // for longjmp(), setjmp()

#  define RW_JMP_BUF             std::jmp_buf
#  ifdef setjmp
#    define RW_SETJMP(env)       setjmp (env)
#  else
#    define RW_SETJMP(env)       std::setjmp (env)
#  endif
#  define RW_LONGJMP(env, val)   std::longjmp (env, val)
#endif   // _RWSTD_OS_LINUX

/**************************************************************************/

// check header <exception> synopsis
static void
test_synopsis ()
{
    // check header <exception> synopsis
    rw_info (0, 0, __LINE__, "header <exception> synopsis");

    // verify that classes are declared
    std::exception *pex = (std::exception*)0;

    // verify that bad_exception publicly derives from exception
    pex = (std::bad_exception*)0;
    _RWSTD_UNUSED (pex);

    // check handler types
    void (*phandler)() = (std::unexpected_handler)0;
    phandler = (std::terminate_handler)0;

    // check unexpected and terminate
    phandler = &std::unexpected;
    phandler = &std::terminate;
    _RWSTD_UNUSED (phandler);

    // check set_{unexpected,terminate}
    std::unexpected_handler (*pset)(std::unexpected_handler) _PTR_THROWS (());

    pset = &std::set_unexpected;
    pset = &std::set_terminate;
    _RWSTD_UNUSED (pset);

    // check uncaught_exception()
    // see http://anubis.dkuug.dk/jtc1/sc22/wg21/docs/lwg-defects.html#70
    // for exception specification details
    bool (*pue)() _PTR_THROWS (()) = &std::uncaught_exception;
    _RWSTD_UNUSED (pue);
}

/**************************************************************************/


// defined at global scope rather than as locals
// to work around an MSVC 6.0 bug (see PR #26305)
static bool dtor_virtual = false;
static bool what_virtual = false;

// check the signatures of class exception and bad_exception members
static void
test_signatures ()
{
// verify that a member function is accessible and has the appropriate
// signature, including return type and exception specification
#define MEMFUN(result, T, name, arg_list) do {                   \
        result (T::*pf) arg_list _PTR_THROWS (()) = &T::name;    \
        _RWSTD_UNUSED (pf);                                      \
    } while (0)

    rw_info (0, 0, __LINE__, "std::exception member function signatures");

    // verify 18.6.1 [lib.exception]

    // verify that a public default and copy ctors exist
    std::exception e1;
    std::exception e2 (e1);

    MEMFUN (std::exception&, std::exception, operator=,
            (const std::exception&));
    MEMFUN (const char*, std::exception, what, () const);

    struct test_exception_virtuals: std::exception {

        ~test_exception_virtuals () _THROWS (()) {
            dtor_virtual = true;
        }
        const char* what () const _THROWS (()) {
            what_virtual = true;

            // working around an MSVC 6.0 bug (PR #26330)
            typedef std::exception Base;
            return Base::what ();
        }
    };

    std::exception *pe = new test_exception_virtuals;

    // verify that destructor and what() are virtual
    pe->what ();
    rw_assert (what_virtual, 0, __LINE__,
               "std::exception::what() not virtual");

    delete pe;
    rw_assert (dtor_virtual, 0, __LINE__,
               "std::exception::~exception() not virtual");


    rw_info (0, 0, __LINE__, "std::bad_exception member function signatures");

    // verify 18.6.2.1 [lib.bad.exception]
    // verify that a public default and copy ctors exist
    std::bad_exception be1;
    std::bad_exception be2 (be1);

    MEMFUN (std::bad_exception&, std::bad_exception, operator=,
            (const std::bad_exception&));
    MEMFUN (const char*, std::bad_exception, what, () const);

    dtor_virtual = false;
    what_virtual = false;

    struct test_bad_exception_virtuals: std::bad_exception {

        ~test_bad_exception_virtuals () _THROWS (()) {
            dtor_virtual = true;
        }
        const char* what () const _THROWS (()) {
            what_virtual = true;

            // working around an MSVC 6.0 bug (PR #26330)
            typedef std::bad_exception Base;
            return Base::what ();
        }
    };

    pe = new test_bad_exception_virtuals;

    // verify that destructor and what() are virtual
    pe->what ();
    rw_assert (what_virtual, 0, __LINE__,
               "std::bad_exception::what() not virtual");

    delete pe;
    rw_assert (dtor_virtual, 0, __LINE__,
               "std::bad_exception::~bad_exception() not virtual");
}

/**************************************************************************/

// test globals (to make the accessible in signal and other handlers)
std::unexpected_handler puh;                 // previous unexpected_handler
std::terminate_handler  pth;                 // previous terminate_handler
int                     expect_abort;        // SIGABRT expected if 1
int                     expect_terminate;    // terminate expected if 1
int                     expect_unexpected;   // unexpected expected if 1
int                     expect_throw_proc;   // throw_proc expected if 1
RW_JMP_BUF              jmpenv;

/**************************************************************************/

// called in response to abort() (called from std::terminate())
extern "C" {

static void
SIGABRT_handler (int signo)
{
    rw_assert (1 == expect_abort && SIGABRT == signo, 0, __LINE__,
               "SIGABRT unexpected");

    // reestablish handler
    std::signal (SIGABRT, SIGABRT_handler);

    expect_abort = -1;

    RW_LONGJMP (jmpenv, 1);
}

}   // extern "C"

/**************************************************************************/

static void
test_terminate_handler ()
{
    rw_assert (1 == expect_terminate, 0, __LINE__,
               "std::terminate() unexpected");

    expect_terminate = -1;

    // establish a handler for SIGABRT (raised from abort())
    std::signal (SIGABRT, SIGABRT_handler);

    // invoke default terminate handler
    pth ();

    // shouldn't be reached
    rw_assert (false, 0, __LINE__,
               "std::terminate() not called or returned");

    RW_LONGJMP (jmpenv, -1);
}

/**************************************************************************/

static void
test_unexpected_handler ()
{
    rw_assert (1 == expect_unexpected, 0, __LINE__,
               "std::unexpected() unexpected");

    expect_unexpected = -1;

    // establish a handler for SIGABRT (raised from abort())
    std::signal (SIGABRT, SIGABRT_handler);

    expect_abort     = 1;
    expect_terminate = 1;

    // throw an exception outside of any try block
    // useless conditional used to prevent warnings
    if (expect_abort)
        throw 1;

    // shouldn't be reached
    rw_assert (false, 0, __LINE__,
               "std::terminate() not called or returned");

    RW_LONGJMP (jmpenv, -1);
}

/**************************************************************************/

// test the effects of 18.6
static void
test_effects ()
{
    // verify 18.6.1, p8: what() returns an implementation-defined NTBS
    std::exception e1;
    const char *what = e1.what ();
    rw_assert (what && 1 <= 1 + std::strlen (what), 0, __LINE__,
               "std::exception::what() != 0");

    std::exception e2 (e1);
    what = e2.what ();
    rw_assert (what && 1 <= 1 + std::strlen (what), 0, __LINE__,
               "std::exception::what() != 0");

    // verify 18.6.2.1, p5: what() returns an implementation-defined NTBS
    std::bad_exception e3;
    what = e3.what ();
    rw_assert (what && 1 <= 1 + std::strlen (what), 0, __LINE__,
               "std::bad_exception::what() != 0");

    std::exception e4 (e3);
    what = e4.what ();
    rw_assert (what && 1 <= 1 + std::strlen (what), 0, __LINE__,
               "std::bad_exception::what()");

#if    !defined (_RWSTD_NO_EXCEPTIONS) \
    && !defined (_RWSTD_NO_EXCEPTION_SPECIFICATION)

    struct S {
        static void foo () throw (double) {
            throw 1;
        }
    };

    rw_info (0, 0, __LINE__, "std::set_unexpected()");
    rw_info (0, 0, __LINE__, "std::set_terminate()");

    // determine the address of the default
    // handlers and replace them with our own
    puh = std::set_unexpected (test_unexpected_handler);
    pth = std::set_terminate  (test_terminate_handler);

    // invoke a function that throws an exception
    // that is not in its exception specification
    expect_unexpected = 1;

    try {
        S::foo ();
    }
    catch (...) {
        rw_assert (0,  0, __LINE__, "incompatible exception propagated");
    }

    RW_LONGJMP (jmpenv, -1);

#else

    // prevent failures due to functionality not implemented in compiler
    expect_abort = -1;
    expect_terminate = -1;
    expect_unexpected = -1;

#endif   // _RWSTD_NO_EXCEPTIONS && ...
}

/**************************************************************************/

struct UncaughtExceptionCheck {
    bool *presult_;

    ~UncaughtExceptionCheck () {
        *presult_ = std::uncaught_exception ();
    }
};

static void
test_uncaught_exception ()
{
    rw_info (0, 0, __LINE__, "std::uncaught_exception()");

    bool expect;
    bool uncaught = std::uncaught_exception ();

#if    !defined (_RWSTD_NO_STD_UNCAUGHT_EXCEPTION)   \
    || !defined (_RWSTD_NO_GLOBAL_UNCAUGHT_EXCEPTION)

    expect = false;

#else

#  ifndef _RWSTD_NO_EXCEPTIONS

    expect = true;

#  else   // if defined (_RWSTD_NO_EXCEPTIONS)

    expect = false;

#  endif   // _RWSTD_NO_EXCEPTIONS)

#endif

    rw_assert (uncaught == expect, 0, __LINE__,
               "std::uncaught_exception() == %d, got %d", expect, uncaught);

#ifndef _RWSTD_NO_EXCEPTIONS

    expect = true;

    try {
        UncaughtExceptionCheck chk;

        chk.presult_ = &uncaught;

        throw 0;
    }
    catch (...) {
    }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    expect = false;

    {
        UncaughtExceptionCheck chk;

        chk.presult_ = &uncaught;
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    rw_assert (uncaught == expect, 0, __LINE__,
               "std::uncaught_exception() == %d, got %d", expect, uncaught);
}

/**************************************************************************/

// original throw proc
static void (*const pthrow_proc_save)(int, char*) = _RW::__rw_throw_proc;

// replaces the original throw proc
static void
test_throw_proc (int id, char *s)
{
    rw_assert (id == expect_throw_proc,  0, __LINE__,
               "throw_proc expected %d, got %d (\"%s\")",
               expect_throw_proc, id, s);

    _RW::__rw_free_what_buf (s);

    // signal that we were successfully called and with what value
    expect_throw_proc = -id;
}

/**************************************************************************/

// exercise __rw::__rw_throw() and __rw::__rw_throw_proc()
static void
test_rw_throw ()
{
#ifndef _RWSTD_NO_EXCEPTIONS

    rw_info (0, 0, __LINE__, "__rw::__rw_throw()");

    // exception format strings
    static const char* strings[] = { _RWSTD_ERROR_STRINGS };

    // exception id's: strings [i + 1] corresponds to expect [i]
    static const int expect[] = {
        _RWSTD_ERROR_FIRST + 1,   // _RWSTD_ERROR_EXCEPTION
        _RWSTD_ERROR_FIRST + 2,   // _RWSTD_ERROR_BAD_EXCEPTION
        _RWSTD_ERROR_FIRST + 3,   // _RWSTD_ERROR_BAD_ALLOC
        _RWSTD_ERROR_FIRST + 4,   // _RWSTD_ERROR_BAD_CAST
        _RWSTD_ERROR_LOGIC_ERROR,
        _RWSTD_ERROR_DOMAIN_ERROR,
        _RWSTD_ERROR_INVALID_ARGUMENT,
        _RWSTD_ERROR_LENGTH_ERROR,
        _RWSTD_ERROR_OUT_OF_RANGE,
        _RWSTD_ERROR_RUNTIME_ERROR,
        _RWSTD_ERROR_RANGE_ERROR,
        _RWSTD_ERROR_OVERFLOW_ERROR,
        _RWSTD_ERROR_UNDERFLOW_ERROR,

        _RWSTD_ERROR_FAILBIT_SET,
        _RWSTD_ERROR_BADBIT_SET,
        _RWSTD_ERROR_EOFBIT_SET,
        _RWSTD_ERROR_IOSTATE_BIT_SET
    };

    const char* const ex_names[] = {
        "std::exception",
        "std::bad_exception",
        "std::bad_alloc",
        "std::bad_cast",
        "std::logic_error",
        "std::domain_error",
        "std::invalid_argument",
        "std::length_error",
        "std::out_of_range",
        "std::runtime_error",
        "std::range_error",
        "std::overflow_error",
        "std::underflow_error",

        "std::ios_base::failbit_set",
        "std::ios_base::badbit_set",
        "std::ios_base::eofbit_set",
        "std::ios_base::failure"
    };

    const char empty[] = "";

    // raise each exception by its id, check that
    // an exception object of the correct type is thrown
    for (unsigned i = 0; i != sizeof expect / sizeof *expect; ++i) {

        rw_info (0, 0, __LINE__, "%s", ex_names [i]);

        int caught = -1;

        // set up a new throw proc (prevent exceptions)
        _RW::__rw_throw_proc = test_throw_proc;

        // no exception should be thrown at this point
        expect_throw_proc = expect [i];

        const char format[] = _RWSTD_FILE_LINE;
        const char func[]   = "void test_rwt_hrow (Test&)";

        _RW::__rw_throw (expect [i], format, func, empty, empty, empty);

        rw_assert (expect [i] == -expect_throw_proc, 0, __LINE__,
                   "%d. throw_proc (%d, ...) not called", i, expect [i]);

        try {
            // reestablish original throw proc
            _RW::__rw_throw_proc = pthrow_proc_save;

            // expect an exception
            _RW::__rw_throw (expect [i], format, func, empty, empty, empty);

            rw_assert (false, 0, __LINE__,
                       "%d. __rw::__rw_throw(%d, ...) returned",
                       i, expect [i]);
        }
        catch (std::domain_error &ex) {
            rw_assert (!!ex.what (),  0, __LINE__,
                       "domain_error::what() != 0");
            caught = _RWSTD_ERROR_DOMAIN_ERROR;
        }
        catch (std::invalid_argument &ex) {
            rw_assert (!!ex.what (),  0, __LINE__,
                       "invalid_argument::what() != 0");
            caught = _RWSTD_ERROR_INVALID_ARGUMENT;
        }
        catch (std::length_error &ex) {
            rw_assert (!!ex.what (),  0, __LINE__,
                       "length_error::what() != 0");
            caught = _RWSTD_ERROR_LENGTH_ERROR;
        }
        catch (std::out_of_range &ex) {
            rw_assert (!!ex.what (),  0, __LINE__,
                       "out_of_range::what() != 0");
            caught = _RWSTD_ERROR_OUT_OF_RANGE;
        }
        catch (std::range_error &ex) {
            rw_assert (!!ex.what (),  0, __LINE__,
                       "range_error::what() != 0");
            caught = _RWSTD_ERROR_RANGE_ERROR;
        }
        catch (std::overflow_error &ex) {
            rw_assert (!!ex.what (),  0, __LINE__,
                       "overflow_error::what() != 0");
            caught = _RWSTD_ERROR_OVERFLOW_ERROR;
        }
        catch (std::underflow_error &ex) {
            rw_assert (!!ex.what (),  0, __LINE__,
                       "underflow_error::what() != 0");
            caught = _RWSTD_ERROR_UNDERFLOW_ERROR;
        }
        catch (std::logic_error &ex) {
            rw_assert (!!ex.what (),  0, __LINE__,
                       "logic_error::what() != 0");
            caught = _RWSTD_ERROR_LOGIC_ERROR;
        }
        catch (std::runtime_error &ex) {
            rw_assert (!!ex.what (),  0, __LINE__,
                       "runtime_error::what() != 0");
            caught = _RWSTD_ERROR_RUNTIME_ERROR;
        }
        catch (std::bad_alloc &ex) {
            rw_assert (!!ex.what (),  0, __LINE__,
                       "bad_alloc::what() != 0");
            caught = _RWSTD_ERROR_FIRST + 3;   // _RWSTD_BAD_ALLOC
        }
        catch (std::bad_exception &ex) {
            rw_assert (!!ex.what (),  0, __LINE__,
                       "bad_exception::what() != 0");
            caught = _RWSTD_ERROR_FIRST + 2;   // _RWSTD_ERROR_BAD_EXCEPTION
        }
        catch (std::bad_cast &ex) {
            rw_assert (!!ex.what (),  0, __LINE__,
                       "bad_cast::what() != 0");
            caught = _RWSTD_ERROR_FIRST + 4;   // _RWSTD_ERROR_BAD_CAST;
        }

#ifdef _RWSTD_ERROR_BAD_TYPEID

        catch (std::bad_typeid &ex) {
            rw_assert (!!ex.what (),  0, __LINE__,
                       "bad_typeid::what() != 0");
            caught = _RWSTD_ERROR_BAD_TYPEID;
        }

#endif   // _RWSTD_ERROR_BAD_TYPEID;

#ifndef _RWSTD_NO_EXT_FAILURE

        catch (std::ios_base::failbit_set &ex) {
            rw_assert (!!ex.what (),  0, __LINE__,
                       "ios_base::failbit_set::what() != 0");
            caught = _RWSTD_ERROR_FAILBIT_SET;
        }
        catch (std::ios_base::badbit_set &ex) {
            rw_assert (!!ex.what (),  0, __LINE__,
                       "ios_base::badbit_set::what() != 0");
            caught = _RWSTD_ERROR_BADBIT_SET;
        }
        catch (std::ios_base::eofbit_set &ex) {
            rw_assert (!!ex.what (),  0, __LINE__,
                       "ios_base::eofbit_set::what() != 0");
            caught = _RWSTD_ERROR_EOFBIT_SET;
        }

#endif   // _RWSTD_NO_EXT_FAILURE

        catch (std::ios_base::failure &ex) {
            rw_assert (!!ex.what (),  0, __LINE__,
                       "ios_base::failure::what() != 0");
            caught = _RWSTD_ERROR_IOSTATE_BIT_SET;
        }
        catch (std::exception &ex) {
            rw_assert (!!ex.what (),  0, __LINE__,
                       "exception::what() != 0");
            caught = _RWSTD_ERROR_FIRST + 1;   // _RWSTD_ERROR_EXCEPTION
        }
        catch (...) {
            caught = -1;
        }

        rw_assert (expect [i] == caught,  0, __LINE__,
                   "%d. expected %d, caught %d", i, expect [i], caught);
    }


    // reestablish original throw proc
    _RW::__rw_throw_proc = pthrow_proc_save;

    char str    [1024];
    char result [2 * sizeof str];

    rw_info (0, 0, __LINE__,
             "exercising throwing standard exception objects "
             "constructed with string arguments up to %zu "
             "characters long (not including context info)",
             sizeof str);

    // exercise the ability or __rw_throw() to correctly format
    // strings of arbitrary length, also verify that format string
    // macros are used to format the what() strings as expected
    for (unsigned j = 0; j != sizeof str - 1; ++j) {

        // exclude exception, bad_alloc, bad_cast, and bad_exception
        // they are typically generated by the compiler and their
        // what() strings are implementation-specific
        unsigned en = j % ((sizeof expect / sizeof *expect) - 5);

        // null-terminate str
        str [j] = '\0';

        std::sprintf (result, strings [en + 5], __FILE__, str,
                      empty, empty, empty);

        try {
            // expect an exception
            _RW::__rw_throw (expect [en + 4], __FILE__, str,
                             empty, empty, empty);

            rw_assert (false, 0, __LINE__,
                       "%d. __rw::__rw_throw (%d, ...) returned",
                       j, expect [en + 4]);
        }
        catch (std::exception &e) {
            rw_assert (e.what () && 0 == std::strcmp (e.what (), result),
                       0, __LINE__,
                       "%u. \"%s\" != \"%s\" [%u]",
                       j, e.what (), result, en + 3);
        }

        // append a decimal digit
        str [j] = '0' + j % 10;
    }

#endif   // _RWSTD_NO_EXCEPTIONS
}

/**************************************************************************/

static void
throwing_unexpected_handler ()
{
    // throw an exception that can't otherwise be possibly thrown
    // to induce std::bad_exception to be rethrown by the langauage
    // runtime library

    struct PrivateStruct { };

    // prevent assertions from the installed SIGABRT handler
    // and terminate_handler in case they are invoked as a
    // result of throwing the exception below
    expect_terminate = 1;
    expect_abort     = 1;

    throw PrivateStruct ();
}

/**************************************************************************/

enum RuntimeExceptionId {
    E_bad_alloc, E_bad_cast, E_bad_exception, E_bad_typeid, E_error
};


// helper classes used by the bad_cast test
// defined at namespace scope rather than at local scope
// to work around an Intel C++ 10.0 ICE (see STDCXX-475)
struct Base { virtual ~Base () { } };
struct Derived1: Base { };
struct Derived2: Base { };


// induce the language runtime into throwing an exception
// returns e if exception cannot bee thrown
static RuntimeExceptionId
induce_exception (RuntimeExceptionId reid, const char *name)
{
    const char *why = 0;

    switch (reid) {

    case E_bad_alloc: {   ////////////////////////////////////////////

#ifndef _RWSTD_NO_NEW_THROWS

#  ifndef _RWSTD_NO_SETRLIMIT

#    if !defined (__HP_aCC)

        // work around an HP aCC 5.xx (IPF) bug (PR #29014)

        // retrieve the current resource limits
        struct rlimit rl = { 0, 0 };
        if (getrlimit (RLIMIT_DATA, &rl))
            return E_error;

        // set the soft limit, leave hard limit unchanged
        rl.rlim_cur = 0;
        rw_warn (0 == setrlimit (RLIMIT_DATA, &rl), 0, __LINE__,
                 "setrlimit (RLIMIT_DATA, ...) failed: %m");

        try {

#    endif   // __HP_aCC
#  endif   // _RWSTD_NO_SETRLIMIT

            // try to allocate a huge amount of memory to induce bad_alloc
            const std::size_t huge_amount = _RWSTD_SIZE_MAX - 4096;

            ::operator new (huge_amount);

#  ifndef _RWSTD_NO_SETRLIMIT
#    if !defined (__HP_aCC)

        }
        catch (...) {

            // reset the soft limit back to the value of the hard limit
            rl.rlim_cur = rl.rlim_max;
            rw_warn (0 == setrlimit (RLIMIT_DATA, &rl), 0, __LINE__,
                     "setrlimit (RLIMIT_DATA, ...) failed: %m");

            // rethrow bad_alloc
            throw;
        }

#    endif   // __HP_aCC

        return E_error;

#  endif   // _RWSTD_NO_SETRLIMIT

#else   // if defined (_RWSTD_NO_NEW_THROWS)

        why = "_RWSTD_NO_NEW_THROWS is #defined";

        break;   // unable to induce bad_alloc

#endif   // NO_NEW_THROWS

    }

    case E_bad_cast: {   /////////////////////////////////////////////

#ifndef _RWSTD_NO_DYNAMIC_CAST

        Derived1 b;
        Base &a = b;

        // induce bad_cast
        dynamic_cast<Derived2&>(a);

        return E_error;

#else   // if defined (_RWSTD_NO_DYNAMIC_CAST)

        why = "_RWSTD_NO_DYNAMIC_CAST is #defined";

        break;   // unable to induce bad_cast

#endif   // _RWSTD_NO_DYNAMIC_CAST
    }

    case E_bad_exception: {   ////////////////////////////////////////

#ifndef _RWSTD_NO_EXCEPTION_SPECIFICATION

        std::set_unexpected (throwing_unexpected_handler);

        struct S {
            // induce bad_exception
            S () throw (std::bad_exception) { throw 0; }
        } s;

        _RWSTD_UNUSED (s);
        return E_error;

#else   // if defined (_RWSTD_NO_EXCEPTION_SPECIFICATION)

        why = "_RWSTD_NO_EXCEPTION_SPECIFICATION is #defined";

        break;   // unable to induce bad_exception

#endif   // _RWSTD_NO_EXCEPTION_SPECIFICATION

    }

    case E_bad_typeid: {   ///////////////////////////////////////////

#if    !defined (_RWSTD_NO_GLOBAL_BAD_TYPEID) \
    || !defined (_RWSTD_NO_STD_BAD_TYPEID)

        struct S { virtual ~S () { } } *s = 0;

        // induce bad_typeid
        typeid (*s);

        return E_error;

#else   // if _RWSTD_NO_GLOBAL_BAD_TYPEID && _RWSTD_NO_STD_BAD_TYPEID

        why = "both _RWSTD_NO_GLOBAL_BAD_TYPEID and _RWSTD_NO_STD_BAD_TYPEID "
            "are #defined";

        break;   // unable to induce bad_typeid

#endif   // NO_BAD_TYPEID

    }

    default:
        break;
    }

    rw_warn (0, 0, __LINE__, "unable to induce std::%s: %s\n", name, why);

    return reid;
}

/**************************************************************************/

static const RuntimeExceptionId
rt_exceptions[] = {
    E_bad_alloc, E_bad_cast, E_bad_exception, E_bad_typeid,
    E_error
};

static const char* const
rt_exception_names[] = {
    "bad_alloc", "bad_cast", "bad_exception", "bad_typeid"
};

static int
opt_rt_exception [E_error];


static void
test_runtime ()
{
#ifndef _RWSTD_NO_EXCEPTIONS

    rw_info (0, 0, __LINE__, "runtime support for exceptions");


    // using static to avoid gcc 3.x warning: variable
    // might be clobbered by `longjmp' or `vfork'

    for (static unsigned i = 0; E_error != rt_exceptions [i]; ++i) {

        const RuntimeExceptionId ex_id   = rt_exceptions [i];
        const char* const        ex_name = rt_exception_names [i];

        if (0 == rw_note (0 <= opt_rt_exception [i],
                          0, __LINE__,
                          "std::%s test disabled", ex_name))
            continue;

        rw_info (0, 0, __LINE__, "std::%s", ex_name);

        static int ex0;
        static int ex1;

        ex0 = ex1 = 0;

        try {
            try {
                // jump back here if the induced exception causes
                // a call to terminate() and/or raises SIGABRT
                if (0 == RW_SETJMP (jmpenv)) {

                    // try to induce the standard exception
                    if (ex_id == induce_exception (ex_id, ex_name)) {
                        ex0 = -1;
                        ex1 = -1;
                    }
                }
                else {
                    rw_assert (false, 0, __LINE__,
                               "inducing std::%s caused a call "
                               "to std::terminate()", ex_name);
                    // prevent additional assertions
                    ex0 = -1;
                }
            }
            catch (std::exception&) {
                // so far so good, rethrow and try to catch again
                ex0 = 0;
                throw;
            }
            catch (...) {
                // failure to catch a standard exception using std::exception
                // most likely indication of class exception having been
                // defined in a different namespace by the C++ Standard
                // library than by the language support library (compiler
                // runtime)
                ex0 = -2;
                throw;
            }
        }
        catch (std::bad_alloc&)     { ex1 = E_bad_alloc; }
        catch (std::bad_cast&)      { ex1 = E_bad_cast;  }
        catch (std::bad_exception&) { ex1 = E_bad_exception; }
        catch (std::bad_typeid&)    { ex1 = E_bad_typeid; }
        catch (...)                 { ex1 = -2; }

        rw_assert (-1 == ex0 || 0 == ex0, 0, __LINE__,
                   "std::%s thrown, std::exception not caught",
                   ex_name);

        rw_assert (-1 == ex1 || rt_exceptions [i] == ex1, 0, __LINE__,
                   "std::%s thrown, unknown exception caught",
                   ex_name);
    }

#endif   // _RWSTD_NO_EXCEPTIONS
}

/**************************************************************************/

static int opt_synopsis;     // for --<toggle>-synopis
static int opt_signatures;   // for --<toggle>-signatures
static int opt_uncaught;     // for --<toggle>-uncaught_exception
static int opt_effects;      // for --<toggle>-effects
static int opt_rw_throw;     // for --<toggle>-rw_throw
static int opt_runtime;      // for --<toggle>-runtime

static int
run_test (int, char**)
{
    if (rw_note (0 <= opt_synopsis, 0, __LINE__,
                 "test of <exception> synopsis disabled"))
        test_synopsis ();

    // check the signatures of class exception and bad_exception members
    if (rw_note (0 <= opt_signatures, 0, __LINE__,
                 "test of function signatures disabled"))
        test_signatures ();

    // exercise std::uncaught_exception() before running any other tests
    // since some of them might affect the correct behavior of the function
    // (if they violate such constraints as returning from a call to
    // std::terminate())
    if (rw_note (0 <= opt_uncaught, 0, __LINE__,
                 "test of uncaught_exception() disabled"))
        test_uncaught_exception ();

    // exercise __rw::__rw_throw() and __rw::__rw_throw_proc()
    if (rw_note (0 <= opt_rw_throw, 0, __LINE__,
                 "test of __rw_throw() disabled"))
        test_rw_throw ();

    // exercise the cooperation between the C++ standard library and
    // the runtime support library when throwing standard exceptions
    if (rw_note (0 <= opt_runtime, 0, __LINE__,
                 "test of runtime support disabled"))
        test_runtime ();

    // exercise the effects last to defer potential problems
    // due to the tests returning (jumping) out of the handlers
    if (rw_note (0 <= opt_effects, 0, __LINE__,
                 "test of effects disabled")) {
        // test the effects of 18.6
        if (0 == RW_SETJMP (jmpenv)) {
            test_effects ();
        }

        // verify that test worked as expected (each handler sets
        // its own expect_xxx variable to -1 after it's been called)
        rw_error (-1 == expect_abort, 0, __LINE__,
              "abort() was called unexpectedly");

        rw_error (-1 == expect_terminate, 0, __LINE__,
                  "terminate() was called unexpectedly");

        rw_error (-1 == expect_unexpected, 0, __LINE__,
                  "unexpected() was called unexpectedly");
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.support.exception",
                    0 /* no comment */,
                    run_test,
                    "|-synopsis~ "
                    "|-signatures~ "
                    "|-uncaught_exception~ "
                    "|-effects~ "
                    "|-rw_throw~ "
                    "|-runtime~ "
                    "|-bad_alloc~ "
                    "|-bad_cast~ "
                    "|-bad_exception~ "
                    "|-bad_typeid~",
                    &opt_synopsis,
                    &opt_signatures,
                    &opt_uncaught,
                    &opt_effects,
                    &opt_rw_throw,
                    &opt_runtime,
                    opt_rt_exception + E_bad_alloc,
                    opt_rt_exception + E_bad_cast,
                    opt_rt_exception + E_bad_exception,
                    opt_rt_exception + E_bad_typeid,
                    0 /* sentinel */);
}

#else // ! TEST_RW_EXTENSIONS

int main()
{
    return 0;
}

#endif // ! TEST_RW_EXTENSIONS
