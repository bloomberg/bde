/************************************************************************
 *
 * new.cpp - definitions of replacement operator new and delete
 *
 * $Id: new.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2003-2006 Rogue Wave Software.
 * 
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <stdlib.h>   // for abort(), getenv()
#include <string.h>   // for memset()

#include <driver.h>
#include <rw_printf.h>
#include <rw_new.h>
#include <rw_exception.h>   // for rw_throw()

/************************************************************************/

#ifndef _RWSTD_BAD_ALLOC
   // _RWSTD_BAD_ALLOC is #defined in <new> but some compilers (e.g.,
   // SunPro) insist on #including their own new no matter what the
   // preprocessor path or what command line options are used
#  if    !defined (_RWSTD_NO_STD_BAD_ALLOC) \
      || !defined (_RWSTD_NO_RUNTIME_IN_STD)
#    define _RWSTD_BAD_ALLOC _APACHE_STD::bad_alloc
  #else   // if _RWSTD_NO_STD_BAD_ALLOC && _RWSTD_NO_RUNTIME_IN_STD

     // working around a gcc 2.x bug (PR #24400)
#    if !defined (__GNUG__) || __GNUG__ > 2

#      define _RWSTD_BAD_ALLOC _APACHE_STD::bad_alloc
#    else
#      define _RWSTD_BAD_ALLOC ::bad_alloc
#    endif   // gcc > 2.x
#  endif   // _RWSTD_NO_STD_BAD_ALLOC || !_RWSTD_NO_RUNTIME_IN_STD
#endif   // _RWSTD_BAD_ALLOC

#ifndef _RWSTD_BAD_ALLOC
#  ifndef _RWSTD_NO_RUNTIME_IN_STD
#    define _RWSTD_BAD_ALLOC std::bad_alloc
#  else
#    define _RWSTD_BAD_ALLOC bad_alloc
#  endif   // _RWSTD_NO_RUNTIME_IN_STD
#endif   // _RWSTD_BAD_ALLOC

/************************************************************************/

// keeps track of the number of pending calls to global dtors
static size_t static_dtors;

static size_t throw_at_calls [2] = { _RWSTD_SIZE_MAX, _RWSTD_SIZE_MAX };
static size_t throw_at_blocks [2] = { _RWSTD_SIZE_MAX, _RWSTD_SIZE_MAX };
static size_t throw_at_bytes [2] = { _RWSTD_SIZE_MAX, _RWSTD_SIZE_MAX };
static size_t break_at_seqno = _RWSTD_SIZE_MAX;

// enables tracing to stderr between the two sequence numbers
static size_t trace_sequence [2] = { 0, 0 };

static rwt_free_store store = {
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { throw_at_calls, throw_at_calls + 1 },
    { throw_at_blocks, throw_at_blocks + 1 },
    { throw_at_bytes, throw_at_bytes + 1 },
    &break_at_seqno
};

static rwt_free_store* pst = &store;


struct Header
{
    Header *prev_;   // link to the previous allocated block
    Header *next_;   // link to the next allocated block
    void   *ptr_;    // pointer to user block (== this + 1)
    size_t  size_;   // size of user block in bytes
    size_t  id_;     // unique id
    void*   self_;   // padded to an even multiple of sizeof(size_t)
                     // with the value of this
};


static Header *last;   // pointer to the most recently allocated block


// guard block stored immediately after the end of the user block
static const char Guard [8] = {
    '\xf8', '\xf9', '\xfa', '\xfb', '\xfc', '\xfd', '\xfe', '\xff'
};


static void
_rw_print_heap ();

// find a block by the user pointer in the list of allocated blocks
static Header*
_rw_find_block (void *ptr, bool check_heap, const char *caller)
{
    size_t nblocks = 0;
    size_t nbytes  = 0;

    Header *res = 0;

    for (Header *hdr = last; hdr; hdr = hdr->prev_) {

        nblocks += 1;
        nbytes  += hdr->size_;

        if (check_heap) {
            // check the lead guard
            if (hdr->self_ != hdr) {
                rw_error (0, 0, __LINE__,
                          "%s:%d: header guard corruption at %#p: "
                          "expected %#p, got %#p",
                          hdr->ptr_, (const void*)hdr, hdr->self_);
                abort ();
            }

            // check that the stored pointer has the expected value
            if (hdr->ptr_ != hdr + 1) {
                rw_error (0, 0, __LINE__,
                          "%s:%d: block address mismatch: "
                          "expected %#p, got %#p",
                          __FILE__, __LINE__,
                          (const void*)(hdr + 1), hdr->ptr_);

                abort ();
            }

            // check the trailing guard
            const char* const grd = (char*)hdr->ptr_ + hdr->size_;

            if (memcmp (grd, Guard, sizeof Guard)) {

                size_t off = 0;
                while (grd [off] == Guard [off])
                    ++off;

                typedef unsigned char UChar;

                rw_error (0, 0, __LINE__,
                          "%s:%d: trailing guard corruption at %#p "
                          "+ %zu of a %zu byte block: '0x%02x' != '0x%x'",
                          __FILE__, __LINE__, hdr->ptr_,
                          hdr->size_ + off + 1, hdr->size_,
                          UChar (grd [off]), UChar (Guard [off]));

                abort ();
            }
        }

        if (ptr == hdr->ptr_) {
            res = hdr;

            if (!check_heap)
                return res;
        }
    }

    if (check_heap) {
        // check that block and byte counters match the totals computed above
        const size_t sum_blocks = pst->blocks_ [0] + pst->blocks_ [1];
        const size_t sum_bytes  = pst->bytes_ [0] + pst->bytes_ [1];

        if (sum_blocks != nblocks || sum_bytes != nbytes) {
            rw_error (0, 0, __LINE__,
                      "%s:%d: counts mismatch: found %zu "
                      "bytes in %zu blocks, expected "
                      "%zu in %zu", __FILE__, __LINE__, 
                      nbytes, nblocks, sum_bytes, sum_blocks);

            abort ();
        }
    }

    if (caller && ptr && !res) {

#if !defined (__DECCXX_VER) || __DECCXX_VER >= 60600000

        rw_error (0, 0, __LINE__,  
                  "%s:%d: %s (%#p): invalid pointer",
                  __FILE__, __LINE__, caller, ptr);

        _rw_print_heap ();

        abort ();

#else   // Compaq C++ < 6.6

        // working around a bug in Compaq C++ libcxx
        // Classic Iostreams library (see bug #359)
        if (static_dtors) {

            rw_error (0, 0, __LINE__, 
                      "%s:%d: %s (%#p): invalid pointer",
                      __FILE__, __LINE__, caller, ptr);

            _rw_print_heap ();

            abort ();
        }
        else {

            static int warned;

            rw_warn (0 < warned++, 0, __LINE__,
                     "%s:%d: %s (%#p): warning: invalid pointer; "
                     "ignoring memory errors from here on out",
                     __FILE__, __LINE__, caller, ptr);
        }

#endif   // Compaq C++ >= 6.6

    }

    return res;
}


// print the list of allocated blocks and check heap consistency
static void
_rw_print_heap ()
{
    rw_info (0, 0, __LINE__,
             "%s:%d: heap dump:\n%zu bytes in %zu blocks%s",
             __FILE__, __LINE__, pst->bytes_, pst->blocks_, last ? ":" : "");

    for (Header *hdr = last; hdr; hdr = hdr->prev_) {

        const size_t seq = hdr->id_;
        const bool array = !!(seq >> (_RWSTD_CHAR_BIT * sizeof (size_t) - 1));

        rw_info (0, 0, __LINE__, 
                 "%zu: %zu bytes at %#p allocated by operator new%s()",
                 hdr->id_, hdr->size_, hdr->ptr_, array ? "[]" : "");
    }

    if (last) 
        // check heap consistency
        _rw_find_block (last, true, 0);
}


static size_t seq_gen;   // sequence number generator


_TEST_EXPORT void*
operator_new (size_t nbytes, bool array)
{
    if (0 == trace_sequence [0] && trace_sequence [0] == trace_sequence [1]) {

        // the first time opetato new is called try to get options
        // from the environment by checking the RWSTD_NEW_FLAGS
        // environment variable in the following format:
        //
        // RWSTD_NEW_FLAGS=[<seqrange>][:<break-seqno>]
        // seqrange ::= <start-seqno>[-<end-seqno>]

        static const char* envvar = getenv ("RWSTD_NEW_FLAGS");

        if (envvar) {
            char *end = _RWSTD_CONST_CAST (char*, envvar);

            if ('-' == *end) {
                // begin tracing with the sequence number 0
                trace_sequence [0] = 0;
            }
            else {
                // begin tracing with the given sequence number
                trace_sequence [0] = strtoul (end, &end, 10);
            }

            if ('-' == *end) {
                // end tracing with the given sequence number
                ++end;
                trace_sequence [1] = strtoul (end, &end, 10);
            }
            else {
                // continue tracing indefinitely
                trace_sequence [1] = _RWSTD_SIZE_MAX;
            }

            if (':' == *end) {
                // break at the given seuqence number
                ++end;
                *pst->break_at_seqno_ = strtoul (end, &end, 10);
            }
        }
    }

    static const char* const name[] = {
        "operator new", "operator new[]"
    };

    // increment the call counter regardless of success
    ++pst->new_calls_ [array];

    // prevent warnings about unused variable and/or unreachable statement
    void *ptr = 0;

    const bool reached_call_limit =
        pst->new_calls_ [array] == *pst->throw_at_calls_ [array];
    const bool reached_block_limit =
        pst->blocks_ [array] >= *pst->throw_at_blocks_ [array];
    const bool reached_size_limit =
        pst->bytes_ [array] + nbytes >= *pst->throw_at_bytes_ [array];

    const bool reached_breakpoint = seq_gen == *pst->break_at_seqno_;

    if (reached_breakpoint) {
        char buffer [128];
        rw_snprintfa (buffer, 128, 
                      "%s (%zu): breakpoint at sequence number %zu",
                      name [array], nbytes, *pst->break_at_seqno_);

        // abort() when a breakpoint has been reached
        _RW::__rw_assert_fail (buffer, __FILE__, __LINE__, 0);

        // should not get here except when the program handles
        // SIGABRT and returns from the handler
    }

    if (   reached_call_limit
        || reached_block_limit
        || reached_size_limit) {

#ifndef _RWSTD_NO_EXCEPTIONS

        try {
            const char * threw = "threw bad_alloc:";

            if (reached_call_limit)
                rw_throw (ex_bad_alloc,
                __FILE__, __LINE__, name [array], 
                "(%zu) %s reached call limit of %zu", 
                nbytes, threw, pst->new_calls_ [array]);

            else if (reached_block_limit)
                rw_throw (ex_bad_alloc,
                __FILE__, __LINE__, name [array], 
                "(%zu) %s reached block limit of %zu: %zu",
                nbytes, threw, *pst->throw_at_blocks_ [array], 
                pst->blocks_ [array]);

            else if (reached_size_limit)
                rw_throw (ex_bad_alloc,
                __FILE__, __LINE__, name [array], 
                "(%zu) %s reached size limit of %zu: %zu",
                nbytes, threw, *pst->throw_at_bytes_ [array], 
                pst->bytes_ [array]);
        }
        catch (const std::exception & ex) {
            if (trace_sequence [0] <= seq_gen && seq_gen < trace_sequence [1])
                rw_fprintf (rw_stderr, "%s\n", ex.what ());

            throw;
        }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

        if (reached_breakpoint) {
            char buf[4096];

            rw_snprintfa (buf, sizeof(buf),
                "reached a breakpoint at of %zu calls", *pst->break_at_seqno_);

            _RW::__rw_assert_fail (buf, __FILE__, __LINE__, name [array]);
        }

        if (trace_sequence [0] <= seq_gen && seq_gen < trace_sequence [1])
            rw_fprintf (rw_stderr, "%s:%d: %s (%zu) --> %#p\n",
                        __FILE__, __LINE__, name [array], nbytes, ptr);

        return 0;

#endif   // _RWSTD_NO_EXCEPTIONS

    }

    const size_t block_size = nbytes + sizeof (Header) + sizeof (Guard);

    // prevent arithmetic overflow
    if (nbytes < block_size)
        ptr = malloc (block_size);

    if (!ptr) {

#ifndef _RWSTD_NO_EXCEPTIONS

        try {
            rw_throw (ex_bad_alloc,
                __FILE__, __LINE__, name [array], 
                "(%zu): malloc() returned 0", nbytes);
        }
        catch (const std::exception & ex) {
            if (trace_sequence [0] <= seq_gen && seq_gen < trace_sequence [1])
                rw_fprintf (rw_stderr, "%s\n", ex.what ());

            throw;
        }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)
        return ptr;
#endif   // _RWSTD_NO_EXCEPTIONS
    }

    // invalidate storage
    memset (ptr, -1, block_size);

    // increment counters
    pst->blocks_ [array] += 1;
    pst->bytes_ [array]  += nbytes;

    // adjust the maximum total number of blocks ever allocated
    if (pst->blocks_ [array] > pst->max_blocks_ [array])
        pst->max_blocks_ [array] = pst->blocks_ [array];

    // adjust the maximum total number of bytes ever allocated
    if (pst->bytes_ [array] > pst->max_bytes_ [array])
        pst->max_bytes_ [array] = pst->bytes_ [array];

    // adjust the size of the single largest block ever allocated
    if (nbytes > pst->max_block_size_ [array])
        pst->max_block_size_ [array] = nbytes;

    // copy guard to the end of the allocated block
    memcpy ((char*)ptr + sizeof (Header) + nbytes, Guard, sizeof (Guard));

    Header* const hdr = (Header*)ptr;

    hdr->ptr_  = (Header*)ptr + 1;
    hdr->size_ = nbytes;
    hdr->id_   = array ? ~seq_gen : seq_gen;
    hdr->self_ = hdr;

    if (0 == last) {
        hdr->prev_ = 0;
        hdr->next_ = 0;
    }
    else {
        hdr->next_  = 0;
        hdr->prev_  = last;
        last->next_ = hdr;
    }

    last = hdr;

    if (trace_sequence [0] <= seq_gen && seq_gen < trace_sequence [1])
        rw_note (0, 0, __LINE__,
                 "%s:%d: %3zi. %s (%zu) --> %#p", 
                 __FILE__, __LINE__, seq_gen, 
                 name [array], nbytes, hdr->ptr_);

    ++seq_gen;

    _rw_find_block (hdr->ptr_, true, 0);

    return hdr->ptr_;
}


_TEST_EXPORT void
operator_delete (void *ptr, bool array)
{
    static const char* const name[] = {
        "operator delete", "operator delete[]"
    };

    // increment the call counter regardless of success
    ++pst->delete_calls_ [array];

    if (ptr) {

        // find the block of memory that `ptr' was allocated from
        // and check the whole heap in the process; the call will
        // abort if any block has been corrupted
        Header* const hdr = _rw_find_block (ptr, true, name [array]);

        if (!hdr) {
            // hdr should never be 0 except under special circumstances
            // such as when the compiler's runtime library itself passes
            // the wrong argument to operator delete (such as libcxx
            // on True64 with Compaq C++ -- see bug #359)
            free (ptr);

            return;
        }

        const size_t nbytes = hdr->size_;
        
        bool mismatch;

        size_t seq = hdr->id_;
        if (seq >> (_RWSTD_CHAR_BIT * sizeof (size_t) - 1)) {

            // the MSB of the stored sequence number is set
            // for blocks allocated with the array form of
            // operator new and must be deallocated with
            // the corresponding array form of operator
            // delete
            mismatch = !array;
            seq      = ~seq;
        }
        else {
            mismatch = array;
        }

        if (trace_sequence [0] <= seq && seq < trace_sequence [1]) {
            if (mismatch)
                rw_error (0, 0, __LINE__, "%s:%d: %3zi. %s (%#p); size = %zu"
                          ": array form mismatch",
                          __FILE__, __LINE__, seq, name [array], ptr, nbytes);
            else
                rw_note (0, 0, __LINE__, "%s:%d: %3zi. %s (%#p); size = %zu",
                         __FILE__, __LINE__, seq, name [array], ptr, nbytes);
        }
        else if (mismatch) {

            const size_t ord = (seq + 1) % 10;

            const char* const ord_sfx =
                1 == ord ? "st" : 2 == ord ? "nd" : 3 == ord ? "rd" : "th";

            rw_error (0, 0, __LINE__,
                      "%s:%d: deallocation mismatch: "
                      "pointer allocated %zu%s in the program "
                      "with a call to operator new%s(%zu) "
                      "being deallocated with the wrong form of %s(%#p)",
                      __FILE__, __LINE__,
                      seq + 1, ord_sfx, array ? "" : "[]",
                      nbytes, name [array], ptr);
            abort ();
        }

        // decrement block and byte counters and remove block
        // from the list only after all checks have succeeded
        // so that tests that catch SIGABRT sent by one of the
        // calls to abort() above and retry the operation may
        // succeed
        pst->blocks_ [array] -= 1;
        pst->bytes_ [array]  -= nbytes;

        if (hdr->prev_)
            hdr->prev_->next_ = hdr->next_;
        if (hdr->next_)
            hdr->next_->prev_ = hdr->prev_;

        if (hdr == last)
            last = hdr->prev_;

        const size_t block_size = nbytes + sizeof (Header) + sizeof (Guard);
        
        // invalidate the entire block including bookkeeping data
        memset (hdr, -1, block_size);

        free (hdr);
    }
    else {
        ++pst->delete_0_calls_ [array];

        if (trace_sequence [0] <= seq_gen && seq_gen < trace_sequence [1])
            rw_note (0, 0, __LINE__, "%s:%d: %s (0)", 
                     __FILE__, __LINE__, name [array]);
    }
}

_TEST_EXPORT rwt_free_store*
rwt_get_free_store (rwt_free_store *st)
{
    rwt_free_store* const ret = pst;

    if (st)
        pst = st;

    return ret;
}

static void
rwt_checkpoint_compare (size_t* diffs, size_t n,
                        const size_t* st0, const size_t* st1,
                        bool& diff_0)
{
    for (size_t i = 0; i != n; ++i) {
        diffs [i] = st1 [i] - st0 [i];
        if (diffs [i])
            diff_0 = false;
    }
}

_TEST_EXPORT rwt_free_store*
rwt_checkpoint (const rwt_free_store *st0, const rwt_free_store *st1)
{
    static rwt_free_store checkpoint;

    if (st0 && st1) {

        // compute the difference between the two states
        // of the free_store specified by the arguments

        static rwt_free_store diff;
        memset (&diff, 0, sizeof diff);

        bool diff_0 = true;   // difference of 0 (i.e., none)

#define EXTENT(array) (sizeof (array) / sizeof(*array))
#define COMPARE(member) \
        rwt_checkpoint_compare (diff.member, EXTENT(diff.member), \
                                st0->member, st1->member, diff_0)

        COMPARE (new_calls_);
        COMPARE (delete_calls_);
        COMPARE (delete_0_calls_);
        COMPARE (blocks_);
        COMPARE (bytes_);
        COMPARE (max_blocks_);
        COMPARE (max_bytes_);
        COMPARE (max_block_size_);

        if (diff_0)
            return 0;

        return &diff;
    }

    if (!st0 && !st1) {
        // compute the difference between the most recent checkpoint
        // and the current state of the free store; store the current
        // state as the new checkpoint

        _rw_find_block (0, true, 0);

        rwt_free_store* const ckpt = rwt_checkpoint (&checkpoint, pst);

        memcpy (&checkpoint, pst, sizeof checkpoint);

        return ckpt;
    }

    if (st0) {
        // compute the difference between the checkpoint specified by
        // the first argument and the current state of the free store
        // store the current state as the last checkpoint

        return rwt_checkpoint (st0, pst);
    }

    // compute the difference between the most recent checkpoint and
    // the checkpoint specified by the second argument

    return rwt_checkpoint (&checkpoint, st1);
}


_TEST_EXPORT _RWSTD_SIZE_T
rwt_check_leaks (_RWSTD_SIZE_T *bytes, const rwt_free_store *st)
{
    const rwt_free_store* const ckpt = rwt_checkpoint (st, 0);

    if (ckpt) {
        if (bytes)
            *bytes = ckpt->bytes_ [0] + ckpt->bytes_ [1];

        return ckpt->blocks_ [0] + ckpt->blocks_ [1];
    }

    if (bytes)
        *bytes = 0;

    return 0;
}


MyNewInit::MyNewInit ()
{
    ++static_dtors;
}

MyNewInit::~MyNewInit ()
{
    --static_dtors;
}
