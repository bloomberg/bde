/***************************************************************************
 *
 * 18.support.dynamic.cpp - test exercising [lib.support.dynamic]
 *
 * $Id: 18.support.dynamic.cpp 648752 2008-04-16 17:01:56Z faridz $
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

#include <new>
#include <memory>
#include <driver.h>

#include <rw/_defs.h>
#include <rw/_error.h>

#include <cstdlib>   // for malloc() and free()
#include <cstring>   // for strlen()


#ifdef _MSC_VER
   // verify that <new> et al can coexist with MSVC's <new.h>
#  include <new.h>
#endif

/**************************************************************************/

// detects recursive implementation of operator new (size_t, nothrow_t)
static int recursive_new_nothrow = 0;
static int recursive_delete_nothrow = 0;

// MemoryAllocator is used to replace the global new and delete
// in order to test conformance of of the other operators that
// might be defined in <new>

struct MemoryAllocator
{
    MemoryAllocator () {
        init ();
    }

    void* operatorNew (std::size_t n) _THROWS ((std::bad_alloc)) {
        _allocPtr = 0;
        _newCalled = true;
        _bytesRequested = n;
        if (_failAlloc) {
            _failAlloc = false;
            _THROW (std::bad_alloc ());
        }
#if !defined (_RWSTD_NO_EXT_OPERATOR_NEW) && \
     defined (_RWSTD_NO_OPERATOR_NEW_NOTHROW)

        // test our implementation of
        // ::operator new(size_t, const std::nothrow)
        // this operator must not be implemented in terms
        // of ::operator new(size_t) to prevent recursion

        if (recursive_new_nothrow++)
            _allocPtr = std::malloc (_bytesRequested);
        else
            _allocPtr = ::operator new (_bytesRequested, std::nothrow);

        --recursive_new_nothrow;

#else
        _allocPtr = std::malloc (_bytesRequested);
#endif
        return _allocPtr;
    }

    void operatorDelete (void* ptr) _THROWS (()) {
        _deallocPtr = ptr;
#if !defined (_RWSTD_NO_EXT_OPERATOR_NEW) && \
     defined (_RWSTD_NO_OPERATOR_NEW_NOTHROW)
        // memory allocated by our nothrow operator new()
        if (recursive_delete_nothrow++ || recursive_new_nothrow)
            std::free (ptr);
        else
            ::operator delete (ptr, std::nothrow);

        --recursive_delete_nothrow;
#else
        std::free (ptr);
#endif
        _deleteCalled = true;
    }

    void init () {
        _newCalled = _deleteCalled = _failAlloc = false;
        _bytesRequested = 0;
        _deallocPtr = _allocPtr = 0;
    }

    bool newCalled () {
        bool tmp = _newCalled;
        _newCalled = 0;
        return tmp;
    }

    bool deleteCalled () {
        bool tmp = _deleteCalled;
        _deleteCalled = 0;
        return tmp;
    }

    std::size_t  _bytesRequested;
    bool              _newCalled;
    bool              _deleteCalled;
    bool              _failAlloc;
    void*             _allocPtr;
    void*             _deallocPtr;
};


// static instance of memory allocator
static MemoryAllocator alloc;

/**************************************************************************/


// replace the global operators with MemoryAllocator versions
void* operator new (std::size_t size) _THROWS ((std::bad_alloc))
{
    return alloc.operatorNew (size);
}

void operator delete (void* p) _THROWS (())
{
    alloc.operatorDelete (p);
}

/**************************************************************************/


#define TEST_OP_THROWS(expr,cond,tag)                \
    _TRY {                                           \
        alloc._failAlloc = true;                     \
        expr;                                        \
        rw_assert (cond, 0, __LINE__, tag);          \
    }                                                \
    _CATCH (std::bad_alloc) {                        \
        RW_ASSERT (!cond, 0, __LINE__, tag);         \
    }                                                \
    _CATCH (...) {                                   \
        rw_assert (!cond, 0, __LINE__, tag);         \
    }                                                \
    (void)0

/**************************************************************************/


// provide a handler for unexpected exceptions so that at least the
//  test will log the event rather than just aborting
void my_unexpected_handler ()
{
    rw_assert (false, 0, __LINE__, "caught an unexpected exception\n");
}

// used in exists_set_new_handler below
void my_new_handler ()
{
}

//  grab default throw procedure
void (*default_throw_proc)(int, char*) = _RW::__rw_throw_proc;

bool my_throw_proc_called = false;

// my_throw_proc just sets a global flag to indicate
// that it was called, and then dispatches the call
// to the default throw procedure
static void my_throw_proc (int id, char* what)
{
    my_throw_proc_called = true;
    default_throw_proc (id, what);
}

/**************************************************************************/


static
int run_test (int, char* [])
{
    std::set_unexpected (&my_unexpected_handler);

    if (1) {
        // exists_std_nothrow
        const std::nothrow_t* p = &std::nothrow;
        rw_assert (p != 0, 0, __LINE__, "std::nothrow defined");
    }

    if (1) {
        // exists_bad_alloc
        std::bad_alloc ba1;         // default ctor
        std::bad_alloc ba2 = ba1;   // copy ctor

        std::exception* e = &ba1;   // derived from exception
        (void) &e;
        ba1 = ba2;                  // assignment operator

        // verify that a member function is accessible and has the
        // appropriate signature, including return type and exception
        // specification

        std::bad_alloc& (std::bad_alloc::*p_op_assign)
                (const std::bad_alloc&) _PTR_THROWS (())
            = &std::bad_alloc::operator=;
        _RWSTD_UNUSED (p_op_assign);

        const char*
        (std::bad_alloc::*p_what_fn)() const _PTR_THROWS (())
             = &std::bad_alloc::what;
        _RWSTD_UNUSED (p_what_fn);

        const char* s = ba1.what ();
        rw_assert (s && 0 != std::strlen (s), 0, __LINE__,
                   "std::bad_alloc::what() returned bad string");
    }

    if (1) {
        // set_new_handler
        typedef void (*new_handler_t) ();

        new_handler_t (*f) (new_handler_t) _PTR_THROWS (());
        f = &std::set_new_handler;

        rw_assert (f != 0, 0, __LINE__,
                   "std::set_new_handler() defined");

        new_handler_t original_handler
            = std::set_new_handler (my_new_handler);

#if !defined (__EDG__) || __EDG_VERSION__ > 245 || defined (__DECCXX)

        // EDG eccp 2.45 standalone demo is known to fail
        rw_assert (original_handler == 0, 0, __LINE__,
                   "std::set_new_handler() unexpectedly returned "
                   "installed handler");

#define _RWSTD_NO_EXT_OPERATOR_NEW

#endif   // !__EDG__ || __EDG_VERSION__ > 245 || __DECCXX

        new_handler_t replacement_handler
            = std::set_new_handler (original_handler);
        rw_assert (my_new_handler == replacement_handler, 0, __LINE__,
                   "std::set_new_handler() failed to return previously "
                   "installed handler");
    }

    // test all operators that we have provided definitions for
    // in <new>; exercises the operators by setting the
    // replacement allocator to fail, and asserting that an
    // exception is thrown or not as appropriate for the operator
    if (1) {
        // operator new throws
        void* ptr = 0;
        _RWSTD_UNUSED (ptr);

        alloc.init ();      // reset the allocator

#ifdef _RWSTD_NO_OPERATOR_NEW_NOTHROW

        // verify that the nothrow version of operator new provided
        // by our library doesn't call the ordinary operator new
        // (if it did it would cause recursion if a replacement operator
        // new were to be implemented in terms of the nothrow version)

        // also verify that it returns 0 on failure

        // force a failure by requesting too large a block of storage
        // size_t (~0) alone isn't good enough since it causes annoying
        // dialog boxes to be popped up by the MSVCRTD, the MSVC runtime
        TEST_OP_THROWS ((ptr = operator new (~0U - 4096U, std::nothrow)),
                        (ptr == 0),
                        "operator new(size_t, nothrow_t) returns 0");

        rw_assert (!alloc.newCalled (), 0, __LINE__,
                   "operator new called by operator new(size_t, nothrow)");

#endif // defined _RWSTD_NO_OPERATOR_NEW_NOTHROW

#if defined (_RWSTD_NO_OPERATOR_DELETE_NOTHROW) && \
   !defined (_RWSTD_NO_PLACEMENT_DELETE)

        // verify that the nothrow version of operator delete provided
        // by our library prevents exceptions from propagating
        TEST_OP_THROWS ((operator delete (0, std::nothrow)), true,
                        "operator delete(void*, nothrow_t) doesn't throw");

        // verify that the nothrow version of operator delete provided
        // by our library doesn't call the ordinary operator delete
        // (if it did it might cause recursion if a replacement operator
        // delete were to be implemented in terms of the nothrow version)
        rw_assert (!alloc.deleteCalled (), 0, __LINE__,
                   "operator delete(void*) called by "
                   "operator delete(void*, nothrow_t)");

#endif   // _RWSTD_NO_OPERATOR_DELETE_NOTHROW && _RWSTD_NO_PLACEMENT_DELETE

#ifdef _RWSTD_NO_OPERATOR_NEW_ARRAY

        // verify that the array form of operator new throws bad_alloc on
        // failure
        TEST_OP_THROWS ((operator new[] (1)), false,
                        "operator new[] (size_t) throws bad_alloc");

        // verify that the array form of operator new provided by our
        // library calls the ordinary operator new
        rw_assert (alloc.newCalled (), 0, __LINE__,
                   "operator new called by operator new[](size_t)");

#endif // defined _RWSTD_NO_OPERATOR_NEW_ARRAY

#ifdef _RWSTD_NO_OPERATOR_NEW_ARRAY_NOTHROW

        TEST_OP_THROWS ((ptr = operator new[](1, std::nothrow)),
                        (ptr == 0),
                        "operator new[] (size_t, nothrow_t) returns 0");

        rw_assert (alloc.newCalled (), 0, __LINE__,
                   "operator new(size_t) called by "
                   "operator new[](size_t, nothrow_t)");

#endif // defined _RWSTD_NO_OPERATOR_NEW_ARRAY_NOTHROW

#ifdef _RWSTD_NO_OPERATOR_DELETE_ARRAY

        TEST_OP_THROWS ((operator delete[] (0)), true,
                        "operator delete[] (void*) doesn't throw");

        rw_assert (alloc.deleteCalled (), 0, __LINE__,
                   "operator delete called by operator delete[](void*)");

#endif // defined _RWSTD_NO_OPERATOR_DELETE_ARRAY

#if defined (_RWSTD_NO_OPERATOR_DELETE_ARRAY_NOTHROW) && \
   !defined ( _RWSTD_NO_PLACEMENT_DELETE)

        TEST_OP_THROWS ((operator delete[] (0, std::nothrow)), true,
                        "operator delete[] (void*, nothrow_t) doesn't "
                        "throw");

        rw_assert (alloc.deleteCalled (), 0, __LINE__,
                   "operator delete(void*) called by "
                   "operator delete[](void*, nothrow_t)");

#endif // _RWSTD_NO_OPERATOR_DELETE_ARRAY_NOTHROW &&
        // !_RWSTD_NO_PLACEMENT_DELETE

        // verify that the nothrow form of operator new provided by our
        // library isn't recursively implemented in terms of the ordinary
        // operator new
        rw_assert (0 == recursive_new_nothrow, 0, __LINE__,
                   "operator new (size_t, nothrow_t) causes recursion");

        rw_assert (0 == recursive_delete_nothrow, 0, __LINE__,
                   "operator delete (void*, nothrow_t) causes recursion");
    }

    // __rw_allocate() is like ::operator new() except that it calls
    // __rw_throw() on failure (which may throw std::bad_alloc).
    //  This test checks that __rw_throw() is called,
    //  that it properly calls __rw_throw_proc(), and that bad_alloc
    //  is thrown. We'll use the replaced ::operator new() to
    //  control when a memory allocation request will fail.

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    const bool test_rw_allocate = true;

#else   // if defined (_RWSTD_NO_REPLACEABLE_NEW_DELETE)

    // avoid tests that depend on the replacement operators new
    // and delete on platforms like AIX or Win32 where they cannot
    // be reliably replaced
    const bool test_rw_allocate = false;

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    if (test_rw_allocate) {

        alloc.init ();      // reset the allocator


        _RW::__rw_throw_proc = my_throw_proc; // set the throw func
        my_throw_proc_called = false;

        for (std::size_t i = 0; i < 10; ++i) {

            char* ptr = 0;

            const bool doFail = !!(i % 2);    // prevent MSVC warning 4800
            if (doFail)
                alloc._failAlloc = true;

            try {
                // allocation
                alloc._bytesRequested = ~i;   // reset

                ptr = _RWSTD_STATIC_CAST (char*, _RW::__rw_allocate (i));
                rw_assert (alloc._bytesRequested == i, 0, __LINE__,
                           "__rw_allocate (%u) requested %u bytes",
                           i, alloc._bytesRequested);

                rw_assert (alloc.newCalled (), 0, __LINE__,
                           "operator new() called");
                rw_assert (!doFail,  0, __LINE__,
                           "allocation succeeded");
                rw_assert (my_throw_proc_called == false,
                           0, __LINE__,
                           "my_throw_proc called");

                // deallocation
                _RW::__rw_deallocate (ptr, i);

                rw_assert (alloc.deleteCalled (),
                           0, __LINE__,
                           "operator delete() called");
                rw_assert (alloc._deallocPtr == ptr,
                           0, __LINE__,
                           "allocated memory deallocated");

            }
            catch (std::bad_alloc) {
                rw_assert (doFail, 0, __LINE__,
                           "expected bad_alloc exception");

                rw_assert (my_throw_proc_called == true, 0, __LINE__,
                           "my_throw_proc called");
            }
            catch (...) {
                rw_assert (false, 0, __LINE__,
                           "expected std::bad_alloc exception");
            }

            my_throw_proc_called = false;
        }
    }
    else {
        rw_warn (false, 0, __LINE__,
                 "__rw::__rw_allocate() not exercised: "
                 "_RWSTD_NO_REPLACEABLE_NEW_DELETE #defined");
    }

    return 0;
}

/**************************************************************************/


int main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.support.dynamic",
                    0 /* no comment */,
                    run_test,
                    "",
                    (void*)0);
}
