/************************************************************************
 *
 * allocator.cpp - definitions of allocator testsuite helpers
 *
 * $Id: allocator.cpp 430850 2006-08-11 18:33:20Z sebor $
 *
 ************************************************************************
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
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <rw_allocator.h>
#include <rw_new.h>         // for operator_new()
#include <driver.h>         // for rw_assert()
#include <new>              // for bad_alloc, placement new
#include <stdarg.h>         // for va_arg(), va_list
#include <string.h>         // for memset()
#include <rw_exception.h>   // for rw_throw()

/**************************************************************************/

static const char* const
_rw_func_names[] = {
    "UserAlloc ()",
    "UserAlloc (const UserAlloc&)",
    "UserAlloc (const UserAlloc<U>&)",
    "~UserAlloc ()",
    "operator= (const UserAlloc&)",
    "operator=<U>(const UserAlloc<U>&)",
    "allocate (size_type, void*)",
    "deallocate (pointer, size_type)",
    "construct (pointer, const_reference)",
    "destroy (pointer)",
    "address (reference) const",
    "max_size () const"
};

static int _rw_id_gen;   // generates unique ids


/**************************************************************************/

SharedAlloc::
SharedAlloc (size_t nbytes /* = MAX_SIZE */, size_t nblocks /* = MAX_SIZE */)
    : max_bytes_ (nbytes), max_blocks_ (nblocks),
      n_bytes_ (0), n_blocks_ (0), n_refs_ (0), id_ (0)
{
    memset (n_calls_, 0, sizeof n_calls_);
    memset (n_throws_, 0, sizeof n_throws_);
    memset (throw_at_calls_, 0, sizeof throw_at_calls_);
}


/* virtual */ SharedAlloc::
~SharedAlloc ()
{
    static size_t deadbeef = 0;
    if (0 == deadbeef) {
        deadbeef = size_t (0xdeadbeefU);
        if (deadbeef < deadbeef << 1) {
            // assume 64-bit size_t
            deadbeef <<= 31;
            deadbeef <<=  1;
            deadbeef  |= size_t (0xdeadbeefU);
        }
    }

    // invalidate
    size_t n = sizeof n_calls_ / sizeof *n_calls_;
    for (size_t i = 0; i != n; ++i)
        n_calls_ [i] = deadbeef;

    n = sizeof n_throws_ / sizeof *n_throws_;
    for (size_t i = 0; i != n; ++i)
        n_throws_ [i] = deadbeef;

    n = sizeof throw_at_calls_ / sizeof *throw_at_calls_;
    for (size_t i = 0; i != n; ++i)
        throw_at_calls_ [i] = deadbeef;
}


/* virtual */ void* SharedAlloc::
allocate (size_t nelems, size_t size, const void* /* = 0 */)
{
    funcall (m_allocate);

    const size_t nbytes = nelems * size;

    if (max_blocks_ < n_blocks_ + nelems)
        rw_throw (ex_bad_alloc, __FILE__, __LINE__, 0,
                  "allocate (%zu):  reached block limit of %zu",
                  nbytes, max_blocks_);

    if (max_bytes_ < n_bytes_ + nbytes)
        rw_throw (ex_bad_alloc, __FILE__, __LINE__, 0,
                  "allocate (%zu):  reached size limit of %zu",
                  nbytes, max_bytes_);

    return operator_new (nbytes, false);
}


/* virtual */ void SharedAlloc::
deallocate (void *ptr, size_t /* nelems */, size_t /* size */)
{
    funcall (m_deallocate);

    return operator_delete (ptr, false);
}


/* virtual */ size_t SharedAlloc::
max_size (size_t size /* = 1 */)
{
    funcall (m_max_size);

    return max_blocks_ / size;
}


/* virtual */ void SharedAlloc::
funcall (MemFun mf, const SharedAlloc *other /* = 0 */)
{
    // increment the number of calls regardless of success
    ++n_calls_ [mf];

    if (m_ctor == mf) {
        // ordinary (not a copy or converting) ctor
        if (id_ <= 0) {
            RW_ASSERT (0 == n_refs_);
            id_ = ++_rw_id_gen;
        }

        // increment the number of references to this allocator
        ++n_refs_;
    }
    else if (m_cpy_ctor == mf || m_cvt_ctor == mf) {
        // copy or converting ctor
        RW_ASSERT (0 < id_);
        RW_ASSERT (0 < n_refs_);

        // increment the number of references to this allocator
        ++n_refs_;
    }
    else if (m_cpy_assign == mf || m_cvt_assign == mf) {
        // assignment operator
        RW_ASSERT (0 <= id_ && id_ <= _rw_id_gen);
        RW_ASSERT (0 < n_refs_);

        RW_ASSERT (0 != other);

        if (this != other) {
            // decrement the number of references and invalidate
            // id if no object refers to *this
            if (0 == --n_refs_)
                id_ = -1;

            SharedAlloc* const po = _RWSTD_CONST_CAST (SharedAlloc*, other);

            if (other->id_ <= 0)
                po->id_ = ++_rw_id_gen;

            ++po->n_refs_;
        }
    }
    else if (m_dtor == mf) {
        // dtor
        RW_ASSERT (0 <= id_ && id_ <= _rw_id_gen);
        RW_ASSERT (0 < n_refs_);

        // decrement the number of references and invalidate
        // id if no object refers to *this
        if (0 == --n_refs_)
            id_ = -1;
    }

    // check the number of calls and throw an exception
    // if the specified limit has been reached
    if (n_calls_ [mf] == throw_at_calls_ [mf]) {
        // increment the exception counter for this function
        ++n_throws_ [mf];

        rw_throw (ex_bad_alloc, __FILE__, __LINE__, 0,
                  "UserAlloc::%s: reached call limit of %zu",
                  _rw_func_names [mf], throw_at_calls_);

        RW_ASSERT (!"logic error: should not reach");
    }
}


/* static */ SharedAlloc* SharedAlloc::
instance (SharedAlloc *pinst /* = 0 */)
{
    // get or a set a pointer to the global allocator object
    static SharedAlloc* pglobal = 0;

    if (0 == pglobal) {
        // construct the global allocator object in the static buffer
        static size_t instbuf [sizeof (SharedAlloc) / sizeof (size_t) + 1];
        static SharedAlloc* const pbuf = new (instbuf) SharedAlloc ();

        pglobal = pbuf;
    }

    if (pinst) {
        // set the global allocator object and return a pointer
        // to the previous one
        SharedAlloc* const tmp = pglobal;
        pglobal = pinst;
        pinst   = tmp;
    }
    else
        pinst = pglobal;

    RW_ASSERT (0 != pinst);

    return pinst;
}


void SharedAlloc::
reset_call_counters ()
{
    memset (n_calls_, 0, sizeof n_calls_);
}


/* static */ const char* SharedAlloc::
func_name (MemFun mf)
{
    return _rw_func_names [mf];
}

/**************************************************************************/

static void
_rw_check_leaks (int line, size_t expect_blocks, size_t expect_bytes)
{
    if (0 < line) {
        // determine and report memory leaks since last checkpoint

        /* const */ size_t nbytes;
        const       size_t nblocks = rwt_check_leaks (&nbytes, 0);

        if (_RWSTD_SIZE_MAX == expect_blocks)
            expect_blocks = nblocks;

        if (_RWSTD_SIZE_MAX == expect_bytes)
            expect_bytes = nbytes;

        rw_assert (nblocks == expect_blocks && nbytes == expect_bytes,
                   0, line,
                   "line %d. %{$FUNCALL} operator new allocated "
                   "%zu bytes in %zu blocks, expected %zu bytes "
                   "in %zu blocks",
                   __LINE__, nbytes, nblocks,
                   expect_bytes, expect_blocks);
    }
    else {
        // establish a checkpoint for memory leaks
        rwt_check_leaks (0, 0);
    }
}


_TEST_EXPORT void
rw_check_leaks (const SharedAlloc *palloc        /* =  0 */,
                int                line          /* =  0 */, 
                size_t             expect_blocks /* =  0 */,
                size_t             expect_bytes  /* = -1 */)
{
    _rw_check_leaks (line, expect_blocks, expect_bytes);

    if (0 == palloc)
        return;

    static size_t nbytes;
    static size_t nblocks;

    if (0 < line) {
        // determine and report memory leaks since last checkpoint
        const size_t leaked_bytes  = palloc->n_bytes_ - nbytes;
        const size_t leaked_blocks = palloc->n_blocks_ - nblocks;

        if (_RWSTD_SIZE_MAX == expect_blocks)   // don't care
            expect_blocks = leaked_blocks;

        if (_RWSTD_SIZE_MAX == expect_bytes)   // don't care
            expect_bytes = nbytes;

        rw_assert (   leaked_blocks == expect_blocks
                   && leaked_bytes == expect_bytes, 0, line,
                   "line %d. %{$FUNCALL} UserAlloc allocated "
                   "%zu bytes in %zu blocks, expected %zu bytes "
                   "in %zu blocks",
                   __LINE__, leaked_bytes, leaked_blocks,
                   expect_bytes, expect_blocks);
    }
    else {
        // establish a checkpoint for memory leaks
        nbytes  = palloc->n_bytes_;
        nblocks = palloc->n_blocks_;
    }
}
