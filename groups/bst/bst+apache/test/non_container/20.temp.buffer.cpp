/***************************************************************************
 *
 * 20.temp.buffer.cpp - test exercising lib.temporary.buffer
 *
 * $Id: 20.temp.buffer.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2003-2006 Rogue Wave Software.
 *
 **************************************************************************/

#include <memory>      // for get_temporary_buffer()

#include <cerrno>      // for errno
#include <cstddef>     // for ptrdiff_t, size_t
#include <cstdio>      // for sprintf()
#include <cstring>     // for memset()

#ifdef _MSC_VER
#  include <climits>     // for INT_MAX
#  include <crtdbg.h>    // for _CrtSetReportMode()
#endif

#include <rw_new.h>
#include <rw_printf.h>
#include <driver.h>

#ifndef _RWSTD_NO_SETRLIMIT
  // #undef works around SunPro bug #568
# undef _TIME_T
# include <sys/resource.h>   // for setrlimit()
# include <unistd.h>         // for sbrk()
#endif   // _RWSTD_NO_SETRLIMIT

#ifdef _AIX

// declare the loader symbol _edata defined by the AIX loader:
// The first address following the initialized data region.
extern "C" {
extern void* _edata;
}   // extern "C"

#endif   // _AIX

/**************************************************************************/

int compare (const void *beg, const void *end, int val)
{
    typedef unsigned char UChar;

    const UChar ucval = UChar (val);

    for (const UChar *pc = (const UChar*)beg; pc != end; ++pc) {
        if (*pc != ucval)
            return *pc - ucval;
    }

    return 0;
}

/**************************************************************************/

template <class T>
void test_success (T*, const char *tname)
{
    RW_ASSERT (0 != tname);

    rw_info (0, 0, __LINE__, "std::get_temporary_buffer<%s>(ptrdiff_t)",
             tname);

    // verify that passing 0 as the argument either returns pair (0, 0)
    // or pair (p, N) with p being a unique pointer in consecutive calls
    // and N >= 0

    static const unsigned pa_size = 32;

#ifndef __HP_aCC

    std::pair<T*, std::ptrdiff_t> pa [pa_size];

#else   // if defined (__HP_aCC)

    // working around an HP aCC bug (PR #27302)
    std::pair<T*, std::ptrdiff_t>* const pa =
        new std::pair<T*, std::ptrdiff_t>[pa_size];

#endif   // __HP_aCC

    // establish a checkpoint for memory leaks
    rwt_check_leaks (0, 0);

    pa [0] = std::get_temporary_buffer<T>(0);

    if (pa [0].first) {

        std::memset (pa [0].first, ~0U, pa [0].second * sizeof (T));

        pa [1] = std::get_temporary_buffer<T>(0);

        if (pa [1].first)
            std::memset (pa [1].first, ~1U, pa [1].second * sizeof (T));

        rw_assert (pa [0].first != pa [1].first, 0, __LINE__,
                   "get_temporary_buffer<%s>(0).first not unique: "
                   "got %#p and %#p", tname, pa [0].first, pa [1].first);
    }
    else {
        rw_assert (0 == pa [0].second, 0, __LINE__,
                   "get_temporary_buffer<%s>(0) == { 0, 0 }, got "
                   "{ %#p, %td }", tname, pa [0].first, pa [0].second);
    }

    pa [2] = std::get_temporary_buffer<T>(2);

    rw_assert (0 != pa [2].first, 0, __LINE__,
               "get_temporary_buffer<%s>(2).first != 0, got 0", tname);

    if (0 == pa [2].first)
        return;

    std::memset (pa [2].first, ~2U, pa [2].second * sizeof (T));

    rw_assert (2 <= pa [2].second, 0, __LINE__,
               "get_temporary_buffer<%s>(2).second >= 2, got %td",
               tname, pa [2].second);

    pa [3] = std::get_temporary_buffer<T>(3);


    rw_assert (0 != pa [3].first, 0, __LINE__,
               "get_temporary_buffer<%s>(3).first != 0, got 0", tname);

    if (!pa [3].first)
        return;

    rw_assert (3 <= pa [3].second, 0, __LINE__,
               "get_temporary_buffer<%s>(3).second >= 3, got %td",
               tname, pa [3].second);

    // verify correct alignment (if the storage isn't properly aligned,
    // expect SIGBUS on RISC machines, or SIGSEGV on HP-UX/PA-RISC)
    pa [3].first [0] = T ();
    pa [3].first [1] = T ();
    pa [3].first [2] = T ();

    std::memset (pa [3].first, ~3U, pa [3].second * sizeof (T));

    // get the remaining temporary buffers and verify that they
    // are each distinct from one another
    for (unsigned i = 4; i != pa_size; ++i) {

        const std::ptrdiff_t size =
            std::ptrdiff_t (i % 2 ? i : _RWSTD_TMPBUF_SIZE + i);

        pa [i] = std::get_temporary_buffer<T>(size);

        if (pa [i].first)
            std::memset (pa [i].first, ~i, pa [i].second * sizeof (T));
    }

    // verify the uniqueness of all ranges
    for (unsigned i = 0; i < pa_size; ++i) {
        for (unsigned j = 0; j < pa_size; ++j) {
            const bool fail =
                   i != j && pa [i].first
                && pa [i].first >= pa [j].first
                && pa [i].first <  pa [j].first + pa [j].second;

            rw_assert (!fail, 0, __LINE__,
                       "pair { %#p, %td } returned from call %u overlaps "
                       "pair { %#p, %td } returned from call %u",
                       pa [i].first, pa [i].second, i,
                       pa [j].first, pa [j].second, j);

            if (fail) {
                // break out of both loops
                i = j = unsigned (-1);
            }
        }
    }

    rw_info (0, 0, __LINE__,
             "std::return_temporary_buffer<%s>(%1$s*)", tname);

    // call return_temporary_buffer() on each returned pointer
    // and verify that the contents of the buffers pointed to
    // by all remaining unallocated pointers are unchanged
    for (unsigned i = 0; i < pa_size; ++i) {

        std::return_temporary_buffer (pa [i].first);

        for (unsigned j = i + 1; j < pa_size; ++j) {

            const bool success =
                0 == compare (pa [j].first, pa [j].first + pa [j].second, ~j);

            rw_assert (success, 0, __LINE__,
                       "return_temporary_buffer<%s>(%#p) corrupted "
                       "a buffer designated by { %#p %td }",
                       tname, pa [i].first, pa [j].first, pa [j].second);

            if (!success) {
                // break out of both loops
                i = j = unsigned (-1);
            }
        }
    }

    std::size_t nbytes;
    const std::size_t nblocks = rwt_check_leaks (&nbytes, 0);

    // verify the absence of memory leaks
    rw_assert (!nblocks && !nbytes, 0, __LINE__,
               "temporary buffer leaked %d bytes in %d blocks",
               nbytes, nblocks);

#ifdef __HP_aCC

    delete[] pa;

#endif   // __HP_aCC

}

/**************************************************************************/

template <class T>
void test_failure (T*, const char *tname)
{
    if (0 == tname) {
        static char buf [40];
        rw_sprintf (buf, "char[%zu]", sizeof (T));
        tname = buf;
    }

    rw_info (0, 0, __LINE__,
             "std::get_temporary_buffer<%s>(ptrdiff_t) on arithmetic overflow",
             tname);

    std::ptrdiff_t nelems = -1;

    std::pair<T*, std::ptrdiff_t> pa;

    pa = std::get_temporary_buffer<T>(nelems);

    rw_assert (0 == pa.first && 0 == pa.second, 0, __LINE__,
               "std::get_temporary_buffer<%s>(%td) == { 0, 0 }, "
               "got { %#p, %td }",
               tname, nelems, pa.first, pa.second);

    for (std::size_t i = 2; i && i < sizeof (T); i <<= 1) {
        // exercise arithmetic overflow (not to be confused
        // with the out-of-memory tests below)

        // attempts to allocate space for an array of elements
        // with a total size that exceeds SIZE_MAX must fail
        nelems = (_RWSTD_PTRDIFF_MAX / i) + 1;

        pa = std::get_temporary_buffer<T>(nelems);

        rw_assert (0 == pa.first && 0 == pa.second, 0, __LINE__,
                   "get_temporary_buffer<%s>(%ld) == { 0, 0 }, "
                   "got { %#p, %td }",
                   tname, nelems, pa.first, pa.second);
    }

#ifndef _RWSTD_NO_SETRLIMIT

    // exercise get_temporary_buffer in the presence of allocation
    // failure (caused by setting the soft data limit to the current
    // value)

    // retrieve the current soft (rlim_cur) and hard (rlim_max) limits
    struct rlimit rlim_old;
    if (getrlimit (RLIMIT_DATA, &rlim_old)) {
        rw_warn (0, 0, __LINE__,
                 "getrlimit(RLIMIT_DATA, { .rlim_max=%zu, .rlim_cur=%zu}) "
                 "failed: %{#m}: %m", rlim_old.rlim_max, rlim_old.rlim_cur);
        return;
    }

    // set the soft limit to 0, leaving the hard limit unchanged
    struct rlimit rlim_new = rlim_old;

#ifdef _AIX
    {
        // AIX setrlimit() fails to lower the limit for resource
        // whose current usage is already higher than the new limit.
        // Instead of setting the limit to 0, compute the current
        // usage and use it to set the soft limit
        const char* const brk_min = (char*)_edata;
        const char* const brk_cur = (char*)sbrk (0);

        rlim_new.rlim_cur = brk_cur - brk_min;
    }
#else   // if !defined (_AIX)
    rlim_new.rlim_cur = 0;
#endif   // _AIX

    errno = 0;

    // IEEE Std 1003.1, 2004 Edition (SUSv3):
    // RLIMIT_DATA
    // ...is the maximum size of a process' data segment, in bytes.
    // If this limit is exceeded, the malloc() function shall fail
    // with errno set to [ENOMEM].
    if (setrlimit (RLIMIT_DATA, &rlim_new)) {
        rw_warn (0, 0, __LINE__,
                 "setrlimit(RLIMIT_DATA, { .rlim_max=%zu, .rlim_cur=%zu}) "
                 "failed to lower the soft limit: %{#m}: %m",
                 rlim_new.rlim_max, rlim_new.rlim_cur);
        return;
    }

    std::size_t nbytes = rlim_old.rlim_max;

#if 0 < _RWSTD_TMPBUF_SIZE

    // make sure the size is larger than the size of the temp buff
    if (nbytes <= _RWSTD_TMPBUF_SIZE)
        nbytes = _RWSTD_TMPBUF_SIZE + 1;

#else   // if _RWSTD_TMPBUF_SIZE <= 0

    nbytes = 65536;

#endif   // _RWSTD_TMPBUF_SIZE

    if (nbytes < _RWSTD_SIZE_MAX)
        ++nbytes;

    if (nbytes <= std::size_t (_RWSTD_PTRDIFF_MAX))
        nelems = std::ptrdiff_t (nbytes);
    else
        nelems = _RWSTD_PTRDIFF_MAX;

    nelems /= sizeof (T);

    // retrieve the current limit just to show it in the info message
    // ignore any errors
    getrlimit (RLIMIT_DATA, &rlim_new);

    rw_info (0, 0, __LINE__,
             "std::get_temporary_buffer<%s>(%td) in low memory conditions "
             ": { .rlim_max = %zu, .rlim_cur = %zu }",
             tname, nelems, rlim_new.rlim_max, rlim_new.rlim_cur);

    // expect the function to fail
    pa = std::get_temporary_buffer<T>(nelems);

    // reset the soft limit to the original value (do it before any
    // test output in case memory needs to be allocated during the
    // output)
    if (setrlimit (RLIMIT_DATA, &rlim_old)) {
        rw_warn (0, 0, __LINE__,
                 "setrlimit(RLIMIT_DATA, { .rlim_max=%zu, .rlim_cur=%zu }) "
                 "failed to restore the soft limit: %{#m}: %m",
                 rlim_old.rlim_max, rlim_old.rlim_cur);
    }

    rw_assert (0 == pa.first && 0 == pa.second, 0, __LINE__,
               "get_temporary_buffer<%s>(%td) == { 0, 0 } "
               "in low memory conditions, got { %#p, %td }",
               tname, nbytes, pa.first, pa.second);

#endif   // _RWSTD_NO_SETRLIMIT

}

/**************************************************************************/

typedef void (*FunctionPointer)();

struct MyStruct { };
typedef void (MyStruct::*MemberPointer)();

template <std::size_t N>
struct BigStruct { char dummy [N]; };

/**************************************************************************/

template <class T>
void test_get_temporary_buffer (T *dummy, const char *tname)
{
    RW_ASSERT (0 != tname);

    test_success (dummy, tname);
    test_failure (dummy, tname);
}

/**************************************************************************/

static int
run_test (int, char**)
{
    test_get_temporary_buffer ((char*)0, "char");

    test_get_temporary_buffer ((int*)0, "int");

#ifdef _RWSTD_LONG_LONG

    test_get_temporary_buffer ((_RWSTD_LONG_LONG*)0, "long long");

#else   // if !defined (_RWSTD_LONG_LONG)

    test_get_temporary_buffer ((long*)0, "long");

#endif   // _RWSTD_LONG_LONG

#ifndef _RWSTD_NO_LONG_DOUBLE

    test_get_temporary_buffer ((long double*)0, "long double");

#else   // if defined (_RWSTD_NO_LONG_DOUBLE)

    test_get_temporary_buffer ((double*)0, "double");

#endif   // _RWSTD_NO_LONG_DOUBLE

    // exercise ordinary pointers
    test_get_temporary_buffer ((void**)0, "void*");

    // exercise function pointers
    test_get_temporary_buffer ((FunctionPointer*)0, "void (*)()");

    // exercise pointers to members
    test_get_temporary_buffer ((MemberPointer*)0, "void (struct::*)()");

#if    (!defined (__IBMCPP__) || __IBMCPP__ > 700) \
    && !defined (__HP_aCC)

#  ifndef _MSC_VER
    const std::size_t MAX_SIZE = _RWSTD_PTRDIFF_MAX;
#  else
    // the MSVC and ICC/Windows has maximum size of
    // the array equal to 0x7fffffff bytes
    const std::size_t MAX_SIZE = INT_MAX;
#  endif

    // avoid instantiating test on very large structs
    // to prevent failures (at compile or run-time) due
    // to compiler bugs
    test_failure ((BigStruct<MAX_SIZE / 2>*)0, 0);
    test_failure ((BigStruct<MAX_SIZE - 1>*)0, 0);
    test_failure ((BigStruct<MAX_SIZE>*)0, 0);

#else

    // work around VAC++ 7.0 (and prior) bug #549
    // work around HP aCC 3,5,6 bug #565
    rw_warn (0, 0, __LINE__, "get_temp_buffer<large-struct>() "
             "not tested due to a compiler bug");

#endif   // VAC++ > 7.0

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
#ifdef _MSC_VER
    // disable "Invalid allocation size: 4294967292 bytes"
    // message box from malloc()
    _CrtSetReportMode (_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif

    return rw_test (argc, argv, __FILE__,
                    "lib.temporary.buffer",
                    0 /* no comment */,
                    run_test,
                    "",
                    (void*)0 /* sentinel */);
}
