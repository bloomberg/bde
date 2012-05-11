/************************************************************************
 *
 * 23.vector.allocator.cpp:
 *
 *     Test exercising vector specialized on a user-defined allocator
 *     with a user-defined pointer type.
 *
 * $Id: 23.vector.allocator.cpp 588744 2007-10-26 18:51:03Z faridz $
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
 * Copyright 2007 Rogue Wave Software, Inc.
 *
 **************************************************************************/

// disable debug iterators to prevent compilation errors
#include <rw/_config.h>

#ifndef _RWSTD_NO_DEBUG_ITER
#  define _RWSTD_NO_DEBUG_ITER
#endif    // _RWSTD_NO_DEBUG_ITER

#include <memory>
#include <vector>
#include <cstddef>   // for ptrdiff_t, size_t

#include <driver.h>

/**************************************************************************/

// DESCRIPTION:
//   Test instantiates std::vector on a user-defined type, user-defined
//   allocator and a user-defined pointer type, making sure that everything
//   still compiles and runs with expected results

//   Note: 20.1.5, p2, Allocator requirements specifies that the type
//         std::allocator<T>::pointer be a "pointer to T", which might
//         make this test case not well-defined; it is expected that
//         this will be changed in a future revision of the Standard

// user-defined pointer type
template <class T>
struct Pointer
{
    typedef T                               value_type;
    typedef std::size_t                     size_type;
    typedef std::ptrdiff_t                  difference_type;
    typedef value_type*                     pointer;
    typedef const value_type*               const_pointer;
    typedef value_type&                     reference;
    typedef const value_type&               const_reference;
    typedef std::random_access_iterator_tag iterator_category;

    Pointer (): offset_ (0) { }

    Pointer (const Pointer &rhs)
        : offset_ (rhs.offset_) { }

    Pointer (difference_type off, int) : offset_ (off) {}

    reference operator* () const {
        return *_RWSTD_REINTERPRET_CAST (pointer, offset_);
    }

    bool operator== (const Pointer &rhs) const {
        return offset_ == rhs.offset_;
    }

    bool operator!= (const Pointer &rhs) const {
        return !operator== (rhs);
    }

    bool operator< (const Pointer &rhs) const {
        return offset_ < rhs.offset_;
    }

    bool operator<= (const Pointer &rhs) const {
        return !(*this > rhs);
    }

    bool operator> (const Pointer &rhs) const {
        return rhs < *this;
    }

    bool operator>= (const Pointer &rhs) const {
        return !(*this < rhs);
    }

    Pointer& operator++ () {
        pointer ptr = _RWSTD_REINTERPRET_CAST (pointer, offset_);
        offset_ = _RWSTD_REINTERPRET_CAST (difference_type, ++ptr);
        return *this;
    }

    Pointer& operator-- () {
        pointer ptr = _RWSTD_REINTERPRET_CAST (pointer, offset_);
        offset_ = _RWSTD_REINTERPRET_CAST (difference_type, --ptr);
        return *this;
    }

    Pointer& operator+= (difference_type i) {
        pointer ptr = _RWSTD_REINTERPRET_CAST (pointer, offset_);
        offset_ = _RWSTD_REINTERPRET_CAST (difference_type, ptr += i);
        return *this;
    }

    Pointer& operator-= (difference_type i) {
        return *this += -i;
    }

    Pointer operator+ (difference_type i) const {
        return Pointer (*this) += i;
    }

    Pointer operator- (difference_type i) const {
        return Pointer (*this) -= i;
    }

    difference_type operator- (const Pointer &rhs) const {
        pointer p1 = _RWSTD_REINTERPRET_CAST (pointer, offset_);
        pointer p2 = _RWSTD_REINTERPRET_CAST (pointer, rhs.offset_);
        return p1 - p2;
    }

    difference_type offset_;

private:

    // deliberately not defined to detect their use
    void operator++ (int);
    void operator-- (int);

};

/**************************************************************************/

// user-defined allocator
template <class T>
class Allocator
{
public:
    typedef T                               value_type;
    typedef std::size_t                     size_type;
    typedef std::ptrdiff_t                  difference_type;
#if DRQS // bdestl_Vector does not support non-raw pointers
    typedef Pointer<T>                      pointer;
    typedef const Pointer<T>                const_pointer;
#else
    typedef T*                              pointer;
    typedef const T*                        const_pointer;
#endif
    typedef T&                              reference;
    typedef const T&                        const_reference;
    typedef std::random_access_iterator_tag iterator_category;

    pointer address (reference r) const {
        return pointer (_RWSTD_REINTERPRET_CAST (difference_type, &r), 1);
    }

    const_pointer address (const_reference r) const {
        return const_pointer (_RWSTD_REINTERPRET_CAST (difference_type, &r),
                              1);
    }

    Allocator () throw () {}

    template <class U>
    Allocator (const Allocator<U>&) throw () {}

#if DRQS // bdestl_Vector does not support non-raw pointers
    pointer allocate (size_type n, const void* = 0) {
        void* const ptr = operator new (n * sizeof (T));
        return pointer (_RWSTD_REINTERPRET_CAST (difference_type, ptr), 1);
    }

    void deallocate (pointer ptr, size_type) {
        operator delete (_RWSTD_REINTERPRET_CAST (void*, ptr.offset_));
    }

    void construct (pointer ptr, const_reference val) {
        new (_RWSTD_REINTERPRET_CAST (void*, ptr.offset_)) value_type (val);
    }

    void destroy (pointer ptr) {
        _RWSTD_REINTERPRET_CAST (T*, ptr.offset_)->~value_type ();
    }
#else // !DRQS
    pointer allocate (size_type n, const void* = 0) {
        return (pointer) operator new (n * sizeof (T));
    }

    void deallocate (pointer ptr, size_type) {
        operator delete (ptr);
    }

    void construct (pointer ptr, const_reference val) {
        new ((void*) ptr) value_type (val);
    }

    void destroy (pointer ptr) {
        ptr->~value_type ();
    }
#endif // !DRQS

    size_type max_size () const throw () {
        return size_type (-1) / sizeof (T);
    }

    template <class U>
    struct rebind { typedef Allocator<U> other; };

    bool operator== (const Allocator&) const {
        return 1;
    }

    bool operator!= (const Allocator &rhs) const {
        return !operator== (rhs);
    }
};

/**************************************************************************/

// user-defined type
struct MyClass
{
    int i_;
    int j_;
};

/**************************************************************************/

#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

// explicitly instantiate to better exercise the template
template class std::vector<MyClass, Allocator<MyClass> >;

#endif   // _RWSTD_NO_EXPLICIT_INSTANTIATION


static int
run_test (int, char**)
{
    rw_warn (0, 0, __LINE__, "debugging iterators disabled");

    typedef std::vector<MyClass, Allocator<MyClass> > Vector;

    const Vector::size_type nelems = 256;

    const MyClass value = { 0, 0 };

    Vector v (nelems, value);

    rw_assert (nelems == v.size (), 0, __LINE__,
               "vector(%zu, value_type).size() == %1$zu, got %zu",
               nelems, v.size ());

    Vector ().swap (v);

    rw_assert (0 == v.size (), 0, __LINE__,
               "vector().swap(vector&).size() == 0, got %zu",
               v.size ());

    Vector::size_type i;

    for (i = 0; i != nelems; ++i)
        v.push_back (value);

    rw_assert (nelems == v.size (), 0, __LINE__,
               "vector::size() == %1$zu, got %zu",
               nelems, v.size ());

    return 0;
}

/**************************************************************************/

int main (int argc, char** argv)
{
    return rw_test (argc, argv, __FILE__,
                    "lib.vector",
                    "with a user-defined allocator and pointer types",
                    run_test,
                    "",
                    (void*)0);
}
