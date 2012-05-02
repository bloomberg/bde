/**************************************************************************
 *
 * rw_allocator.h - user-defined allocator type
 *
 * $Id: rw_allocator.h 550991 2007-06-26 23:58:07Z sebor $
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
 * Copyright 2006 Rogue Wave Software.
 *
 **************************************************************************/

#ifndef RW_ALLOCATOR_INCLUDED
#define RW_ALLOCATOR_INCLUDED


#include <testdefs.h>
#include <memory>

struct _TEST_EXPORT SharedAlloc
{
    // identifies each member function of a standard allocator class
    enum MemFun {
        m_ctor,         // ordinary constructor
        m_cpy_ctor,     // copy constructor
        m_cvt_ctor,     // converting (template) constructor
        m_cpy_assign,   // ordinary assignment operator
        m_cvt_assign,   // converting (template) assignment operator
        m_dtor,         // destructor
        m_allocate, m_deallocate,
        m_construct, m_destroy,
        m_address, m_max_size,
        n_funs
    };

    // constructs an allocator object managing a memory pool
    // of the specified size (bytes and blocks)
    SharedAlloc (_RWSTD_SIZE_T /* max_bytes  */ = _RWSTD_SIZE_MAX,
                 _RWSTD_SIZE_T /* max_blocks */ = _RWSTD_SIZE_MAX);

    virtual ~SharedAlloc ();

    // attempts to allocate storage for nelems objects of elemsize
    // bytes each and returns a pointer to the allocated storage
    // on success; throws std::bad_alloc on failure
    virtual void*
    allocate (_RWSTD_SIZE_T /* nelems */,
              _RWSTD_SIZE_T /* elemsize */,
              const void*   /* hint */);

    // deallocates storage at ptr allocated by a call to allocate
    // for nelems objects of elemsize bytes each
    virtual void
    deallocate (void*         /* ptr */,
                _RWSTD_SIZE_T /* nelems */,
                _RWSTD_SIZE_T /* elemsize */);

    // returns the maximum number of objects of elemsize each
    // that can be allocated from the pool managed by *this
    virtual _RWSTD_SIZE_T
    max_size (_RWSTD_SIZE_T /* elemsize */ = 1);

    // records a call to the allocator member function fun and
    // throws an exception derived from std::bad_alloc if the
    // number of calls to the member function reaches the limit
    // specified by throw_at_calls_
    virtual void
    funcall (MemFun /* fun */, const SharedAlloc* = 0);

    // gets or sets a pointer to the global allocator object
    static SharedAlloc*
    instance (SharedAlloc* = 0);

    // returns a unique id of this allocator object
    int id () const { return id_; }

    // resets the n_calls_ array to all zeros
    void reset_call_counters ();

    // returns the name of the member function
    static const char* func_name (MemFun);

    _RWSTD_SIZE_T max_bytes_;    // memory pool size in bytes
    _RWSTD_SIZE_T max_blocks_;   // memory pool size in blocks

    _RWSTD_SIZE_T n_bytes_;      // number of allocated bytes
    _RWSTD_SIZE_T n_blocks_;     // number of allocated blocks

    _RWSTD_SIZE_T n_refs_;       // number of references

    // counter of the number of calls to each allocator member function
    // made throughout the lifetime of this object
    _RWSTD_SIZE_T n_calls_ [n_funs];

    // counter of the number of exceptions thrown by each member function
    // throughout the lifetime of this object
    _RWSTD_SIZE_T n_throws_ [n_funs];

    // member function counter value that, when reached, will cause
    // an exception derived from std::bad_alloc to be thrown
    _RWSTD_SIZE_T throw_at_calls_ [n_funs];

private:
    int id_;
};


// allocator types for easy specialization
template <class T>
struct AllocTypes
{
    typedef _RWSTD_SIZE_T    size_type;
    typedef _RWSTD_PTRDIFF_T difference_type;
    typedef T*               pointer;
    typedef const T*         const_pointer;
    typedef T&               reference;
    typedef const T&         const_reference;
};


template <class T, class Types = AllocTypes<T> >
struct UserAlloc
{
// private:
    SharedAlloc* pal_;

public:
    typedef T                               value_type;
    typedef typename Types::size_type       size_type;
    typedef typename Types::difference_type difference_type;
    typedef typename Types::pointer         pointer;
    typedef typename Types::const_pointer   const_pointer;
    typedef typename Types::reference       reference;
    typedef typename Types::const_reference const_reference;

    UserAlloc (SharedAlloc *pal = 0) _THROWS (())
        : pal_ (pal ? pal : SharedAlloc::instance ()) {
        RW_ASSERT (0 != pal_);
        pal_->funcall (pal_->m_ctor);
    }

    UserAlloc (const UserAlloc &rhs) _THROWS (())
        : pal_ (rhs.pal_) {
        RW_ASSERT (0 != pal_);
        pal_->funcall (pal_->m_cpy_ctor);
    }

    template <class U>
    UserAlloc (const UserAlloc<U> &rhs) _THROWS (())
        : pal_ (rhs.pal_) {
        RW_ASSERT (0 != pal_);
        pal_->funcall (pal_->m_cvt_ctor);
    }

    template <class U>
    void operator= (const UserAlloc<U> &rhs) {
        pal_->funcall (pal_->m_cvt_assign, rhs.pal_);
        pal_ = rhs.pal_;
    }

    template <class U>
    struct rebind { typedef UserAlloc<U> other; };

    void operator= (const UserAlloc &rhs) {
        pal_->funcall (pal_->m_cpy_assign, rhs.pal_);
        pal_ = rhs.pal_;
    }

    ~UserAlloc () {
        pal_->funcall (pal_->m_dtor);
    }

    pointer allocate (size_type nelems, const void *hint = 0) {
        void* const ptr = pal_->allocate (nelems, sizeof (value_type), hint);
        return _RWSTD_STATIC_CAST (pointer, ptr);
    }

    void deallocate (pointer ptr, size_type nelems) {
        pal_->deallocate (ptr, nelems, sizeof (value_type));
    }

    void construct (pointer ptr, const_reference val) {
        pal_->funcall (pal_->m_construct);
        new (ptr) value_type (val);
    }

    void destroy (pointer ptr) {
        pal_->funcall (pal_->m_destroy);
        ptr->~T ();
    }

    pointer address (reference ref) const {
        pal_->funcall (pal_->m_address);
        return &ref;
    }

    const_pointer address (const_reference ref) const {
        pal_->funcall (pal_->m_address);
        return &ref;
    }

    size_type max_size () const _THROWS (()) {
        return pal_->max_size (sizeof (value_type));
    }

    bool operator== (const UserAlloc &rhs) const {
        return pal_->id () == rhs.pal_->id ();
    }

    bool operator!= (const UserAlloc &rhs) const {
        return !operator== (rhs);
    }
};


// when (line <= 0) establishes a new check point for memory leaks
// by storing the number and size of blocks of storage currently
// allocated by operator new and by the SharedAlloc object pointed
// to by palloc (when non-zero)
// when (line > 0) reports the difference between the the number
// and size of blocks of storage allocated at the last checkpoint
// and the values specified by expect_blocks and expect_bytes
_TEST_EXPORT void
rw_check_leaks (const SharedAlloc* /* palloc        */ = 0,
                int                /* line          */ = 0,
                _RWSTD_SIZE_T      /* expect_blocks */ = 0,
                _RWSTD_SIZE_T      /* expect_bytes  */ = _RWSTD_SIZE_MAX);


// when (line <= 0) establishes a new check point for memory leaks
// by storing the number and size of blocks of storage currently
// allocated by operator new
// when (line > 0) reports the difference between the the number
// and size of blocks of storage allocated at the last checkpoint
// and the values specified by expect_blocks and expect_bytes
template <class charT>
inline void
rw_check_leaks (const std::allocator<charT>& /* unused */,
                int                          line          = 0,
                _RWSTD_SIZE_T                expect_blocks = 0,
                _RWSTD_SIZE_T                expect_bytes  = _RWSTD_SIZE_MAX)
{
    // can't track memory leaks here
    rw_check_leaks ((SharedAlloc*)0, line, expect_blocks, expect_bytes);
}


// when (line <= 0) establishes a new check point for memory leaks
// by storing the number and size of blocks of storage currently
// allocated by operator new and by the user-defined allocator
// specified by the first argument
// when (line > 0) reports the difference between the the number
// and size of blocks of storage allocated at the last checkpoint
// and the values specified by expect_blocks and expect_bytes
template <class charT, class Types>
inline void
rw_check_leaks (const UserAlloc<charT, Types> &alloc,
                int                            line          = 0,
                _RWSTD_SIZE_T                  expect_blocks = 0,
                _RWSTD_SIZE_T                  expect_bytes  = _RWSTD_SIZE_MAX)
{
    // track memory leaks via SharedAlloc
    rw_check_leaks (alloc.pal_, line, expect_blocks, expect_bytes);
}

#endif   // RW_ALLOCATOR_INCLUDED
