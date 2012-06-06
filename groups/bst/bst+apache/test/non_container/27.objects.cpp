/***************************************************************************
 *
 * 27.objects.cpp - test exercising lib.iostream.objects
 *
 * $Id: 27.objects.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2000-2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#include <driver.h>

// If the following macro is defined, then new header provides
// an implementation for operator new [] & delete []; undef them here
// so that the test can provide its own implementation for these operators
#undef _RWSTD_NO_OPERATOR_NEW_ARRAY
#undef _RWSTD_NO_OPERATOR_DELETE_ARRAY


#if defined (__IBMCPP__) && !defined (_RWSTD_NO_IMPLICIT_INCLUSION)
  // disable implicit inclusion to work around
  // a limitation in IBM VisualAge 5.0.2.0 (PR #26959)

#  define _RWSTD_NO_IMPLICIT_INCLUSION
#endif

#if !defined (_MSC_VER) || _MSC_VER > 1300
   // prevent out-of-line template definitions in .cc files from being
   // explicitly #included during the processing of library headers (faster
   // compilation) assumes that the test doesn't instantiate those templates
   // on types other than those they were explcitly instantiated on (will
   // have no effect if explicit instantiation is disabled or unsupported)
#  define _RWSTD_NO_TEMPLATE_DEFINITIONS
#endif   // !defined (_MSC_VER) || _MSC_VER > 1300


struct LifetimeChecker
{
    LifetimeChecker ();

    ~LifetimeChecker ();

} lifetime_check;


#if defined (_WIN32) | defined (_WIN64)
   // bring in <windows.h> as a compilation test to exercise
   // any potential collisions with our declarations
#  include <windows.h>
#endif

// include <iostream> *after* the definition of the global
// lifetime_checker object to exercise initialization and
// lifetime dependencies
#include <iostream>   // for cin, cout, ...
#include <cstdlib>    // for free(), malloc()

// make sure using declarations work (the test doesn't actually exploit
// the fact that the names are introduced into the global namespace)

_USING (std::cin);
_USING (std::cout);
_USING (std::cerr);
_USING (std::clog);

#ifndef _RWSTD_NO_WCHAR_T

_USING (std::wcin);
_USING (std::wcout);
_USING (std::wcerr);
_USING (std::wclog);

#endif   // _RWSTD_NO_WCHAR_T

/**************************************************************************/

int init_new_calls;    // number of calls to operator new during initialization
int init_delete_calls; // and operator delete
int new_calls;         // total number of calls to operator new
int delete_calls;      // and operator delete

int rw_opt_no_global_dtors;         // for --no-global-dtors
int rw_opt_no_replacement_new;      // for --no-replacement-new
int rw_opt_no_replacement_delete;   // for --no-replacement-delete

/**************************************************************************/

LifetimeChecker::LifetimeChecker ()
{
    // iostreams not yet initialized here
    // <iostream> must be included before the definition of any
    // global object whose ctor uses any of the standard objects
}


LifetimeChecker::~LifetimeChecker ()
{
    if (rw_opt_no_global_dtors) {
        rw_note (0, 0, __LINE__, "test of global dtors disabled");
    }
    else {
        // objects must not be destroyed during program lifetime
        // try to use them in the destructor of a global object...
        std::cout << "[output from the dtor of a global object]";

        std::cout << "[cout]";
        std::cerr << "[cerr]";
        std::clog << "[clog]";

#ifndef _RWSTD_NO_WCHAR_T

        std::wcout << "[wcout]";
        std::wcerr << "[wcerr]";
        std::wclog << "[wclog]";

#endif   // _RWSTD_NO_WCHAR_T

    }
}

/**************************************************************************/

template <class charT>
void do_test (charT)
{
    rw_info (0, 0, 0, "checking whether objects are initialized");

    // objects must exist
    rw_assert (   0 != &std::cin  && 0 != &std::cout
               && 0 != &std::cerr && 0 != &std::clog,
               0, __LINE__, "std::cin, cout, cerr, or clog address 0");

    rw_info (0, 0, __LINE__, "checking tied stream objects");

    int new_0 = new_calls;

    // exercise 27.3.1, p2
    rw_assert (&std::cout == std::cin.tie (&std::cout), 0, __LINE__,
               "&std::cout == std::cin.tie (&std::cout)");
    rw_assert (0 == std::cout.tie (), 0, __LINE__,
               "0 == std::cout.tie ()");
    rw_assert (0 == std::cerr.tie (), 0, __LINE__,
               "0 == std::cerr.tie ()");
    rw_assert (0 == std::clog.tie (), 0, __LINE__,
               "0 == std::clog.tie ()");

    rw_assert (0 == new_calls - new_0, 0, __LINE__,
               "unexpected dynamic memory allocation");

    rw_info (0, 0, __LINE__, "checking for std::ios_base::unitbuf in flags");

    new_0 = new_calls;

    // exercise 27.3.1, p3
    rw_assert (!(std::ios::unitbuf & std::cout.flags ()), 0, __LINE__,
               "std::ios::unitbuf & std::cout.flags ()");

    // exercise 27.3.1, p5
    rw_assert (!!(std::ios::unitbuf & std::cerr.flags ()), 0, __LINE__,
               "std::ios::unitbuf & std::cerr.flags ()");

    // exercise 27.3.1, p6
    rw_assert (!(std::ios::unitbuf & std::clog.flags ()), 0, __LINE__,
               "std::ios::unitbuf & std::clog.flags ()");

    rw_assert (0 == new_calls - new_0, 0, __LINE__,
               "unexpected dynamic memory allocation");

#ifndef _RWSTD_NO_WCHAR_T

    rw_assert (   0 != &std::wcin  && 0 != &std::wcout
               && 0 != &std::wcerr && 0 != &std::wclog,
               0, __LINE__, "std::wcin, wcout, wcerr, or wclog address 0");

    new_0 = new_calls;

    // exercise 27.3.2, p2
    rw_assert (&std::wcout == std::wcin.tie (&std::wcout), 0, __LINE__,
               "&std::wcout == std::wcin.tie (&std::wcout)");
    rw_assert (0 == std::wcout.tie (), 0, __LINE__,
               "0 == std::wcout.tie ()");
    rw_assert (0 == std::wcerr.tie (), 0, __LINE__,
               "0 == std::wcerr.tie ()");
    rw_assert (0 == std::wclog.tie (), 0, __LINE__,
               "0 == std::wclog.tie ()");

    rw_assert (0 == new_calls - new_0, 0, __LINE__,
               "unexpected dynamic memory allocation");

    new_0 = new_calls;

    // exercise 27.3.2, p3
    rw_assert (!(std::wios::unitbuf & std::wcout.flags ()), 0, __LINE__,
               "std::wios::unitbuf & std::wcout.flags ()");

    // exercise 27.3.2, p5
    rw_assert (!!(std::wios::unitbuf & std::wcerr.flags ()), 0, __LINE__,
               "std::wios::unitbuf & std::wcerr.flags ()");

    // exercise 27.3.2, p6
    rw_assert (!(std::wios::unitbuf & std::wclog.flags ()), 0, __LINE__,
               "std::wios::unitbuf & std::wclog.flags ()");

    rw_assert (0 == new_calls - new_0, 0, __LINE__,
               "unexpected dynamic memory allocation");

#endif   // _RWSTD_NO_WCHAR_T

    std::cout << 0.0;
    std::cerr << 0.0;
    std::clog << 0.0;

#ifndef _RWSTD_NO_WCHAR_T

    std::wcout << 0.0;
    std::wcerr << 0.0;
    std::wclog << 0.0;

#endif   // _RWSTD_NO_WCHAR_T

    std::cout << std::endl;


    rw_info (0, 0, __LINE__, "exercising the ability to tie each stream "
             "to itself without causing a deadlock");

    std::cout.tie (&std::cout);
    std::cout << std::cout.tie () << ' ';

    std::cerr.tie (&std::cerr);
    std::cerr << std::cerr.tie () << ' ';

    std::clog.tie (&std::clog);
    std::clog << std::clog.tie () << ' ';

#ifndef _RWSTD_NO_WCHAR_T

    std::wcout.tie (&std::wcout);
    std::wcout << std::wcout.tie () << ' ';

    std::wcerr.tie (&std::wcerr);
    std::wcerr << std::wcerr.tie () << ' ';

    std::wclog.tie (&std::wclog);
    std::wclog << std::wclog.tie () << ' ';

#endif   // _RWSTD_NO_WCHAR_T


    rw_info (0, 0, __LINE__, "exercising the ability to tie stream objects "
             "together without causing a deadlock");

    std::cout.tie (&std::cerr);
    std::cout << std::cout.tie () << ' ';

    std::cerr.tie (&std::cout);
    std::cerr << std::cerr.tie () << ' ';

    std::clog.tie (&std::cout);
    std::clog << std::clog.tie () << ' ';

    // untie all streams to free any memory dynamically
    // allocated by calling tie (p) with (p != 0)
    std::cin.tie (0);
    std::cout.tie (0);
    std::cerr.tie (0);
    std::clog.tie (0);

#ifndef _RWSTD_NO_WCHAR_T

    std::wcout.tie (&std::wcerr);
    std::wcout << std::wcout.tie () << ' ';

    std::wcerr.tie (&std::wcout);
    std::wcerr << std::wcerr.tie () << ' ';

    std::wclog.tie (&std::wcout);
    std::wclog << std::wclog.tie () << ' ';

    // untie all streams to free any memory dynamically
    // allocated by calling tie (p) with (p != 0)
    std::wcin.tie (0);
    std::wcout.tie (0);
    std::wcerr.tie (0);
    std::wclog.tie (0);

#endif   // _RWSTD_NO_WCHAR_T

    std::cout << std::endl;
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    const int new_0    = new_calls;
    const int delete_0 = delete_calls;

    do_test (char ());

    const int new_1    = new_calls - new_0;
    const int delete_1 = delete_calls - delete_0;

    // verify that test doesn't leak any dynamically allocated storage
    rw_assert (0 == new_1 - delete_1, 0, __LINE__,
               "test leaked %d blocks of memory", new_1 - delete_1);
    rw_assert (0 == init_new_calls, 0, __LINE__,
               "iostream initialization called operator new() %d times, "
               "0 expected", init_new_calls);

#ifdef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    rw_warn (0, 0, __LINE__,
                "replacement operators new and delete not tested: "
                "_RWSTD_NO_REPLACEABLE_NEW_DELETE #defined");

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    return 0;
}

/**************************************************************************/

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

#  include <new>

#  ifndef _RWSTD_BAD_ALLOC
     // #define if not #defined by <new> (SunPro #includes its
     // own <new> regardless of the preprocessor search path)
#    define _RWSTD_BAD_ALLOC _APACHE_STD::bad_alloc
#  endif   // _RWSTD_BAD_ALLOC


void* operator new (std::size_t n) _THROWS ((_RWSTD_BAD_ALLOC))
{
    static int recursion;

    ++recursion;

    if (rw_opt_no_replacement_new) {
        static int noted;   // prevent multiple notes

        rw_note (noted++, 0, __LINE__,
                 "test of replacement operator new disabled");
    }
    else if (1 != recursion) {
        // avoid calling rw_warn() except when it has something
        // to warn about in case operator new() is being called
        // by the compiler's C++ runtime (e.g., HP aCC 6.16)
        // when the driver hasn't been initialized yet (otherwise
        // the driver aborts)
#if TEST_RW_EXTENSIONS
    // The driver hasn't been initialized, and new is called many times
    // already.
        rw_warn (1 == recursion, 0, __LINE__,
                 "recursive call to replacement operator new()");
#endif
    }
    else {
        // initialize iostreams (verifies that initialization
        // succeeds and is complete even if it occurs recursively)
        static std::ios_base::Init io_initializer;

        // use cout from within new to verify that the static local
        // ios_base::Init properly initializes iostream objects
        std::cout << "[replacement operator new (" << n << ")]\n";
    }

    ++new_calls;

    --recursion;

    void* const ptr = std::malloc (n);
    if (!ptr)
        _THROW (_RWSTD_BAD_ALLOC ());

    return ptr;
}


void operator delete (void *ptr) _THROWS (())
{
    static int recursion;

    ++recursion;

    if (rw_opt_no_replacement_delete) {
        static int noted;   // prevent multiple notes

        rw_note (noted++, 0, __LINE__,
                 "test of replacement operator delete disabled");
    }
#if TEST_RW_EXTENSIONS
    // stlport implementation has new and delete interleaving, so can't use
    // cout at this point yet.
    else if (rw_warn (1 == recursion, 0, __LINE__,
                      "recursive call to replacement operator delete()")) {
        std::cout << "[replacement operator delete (" << ptr << ")]\n";
    }
#endif

    --recursion;

    if (ptr)
        ++delete_calls;

    std::free (ptr);
}


void* operator new[] (std::size_t n) _THROWS ((_RWSTD_BAD_ALLOC))
{
    return operator new (n);
}


void operator delete[] (void *p) _THROWS (())
{
    operator delete (p);
}

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

/**************************************************************************/

int main (int argc, char *argv[])
{
    init_new_calls = new_calls;
    init_delete_calls = delete_calls;

    return rw_test (argc, argv, __FILE__,
                    "lib.iostream.objects",
                    0,   // no comment
                    run_test,
                    "|-no-global-dtors# "
                    "|-no-replacement-new# "
                    "|-no-replacement-delete#",
                    &rw_opt_no_global_dtors,
                    &rw_opt_no_replacement_new,
                    &rw_opt_no_replacement_delete);
}
