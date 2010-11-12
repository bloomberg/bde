/***************************************************************************
 *
 * 23.vector.cons.cpp - test exercising [lib.vector.cons]
 *
 * $Id: 23.vector.cons.cpp 550991 2007-06-26 23:58:07Z sebor $
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

#include <memory>      // for placement operator new()
#include <vector>      // for vector

#include <cstddef>     // for size_t

#include <alg_test.h>
#include <rw_value.h>  // for UserClass
#include <driver.h>
#include <rw_new.h>

/**************************************************************************/

static unsigned rw_opt_nloops;
static int      rw_opt_no_int_spec;
static int      rw_opt_no_short_spec;
static int      rw_opt_no_types;
static int      rw_opt_no_signatures;
static int      rw_opt_no_ctors;

/**************************************************************************/

template <class RandomAccessIterator,
          class T,
          class Reference,
          class Pointer,
          class Difference>
struct test_iterator
{
    test_iterator ();
};

/**************************************************************************/

// outlined to prevent gcc -Winline warnings
template <class RandomAccessIterator,
          class T,
          class Reference,
          class Pointer,
          class Difference>
test_iterator<RandomAccessIterator, T, Reference, Pointer, Difference>::
test_iterator ()
{
    static int i;   // zeroed out
    if (++i)        // prevent "unreachable code" warnings
        return;     // compile only

    // exercise RandomAccessIterator requirements

    RandomAccessIterator it = RandomAccessIterator ();
    RandomAccessIterator it_cpy (it);

    Difference d (it - it);

#if DRQS // Mismatch between bslstl_ReverseIterator and vector::reverse_iterator
    RandomAccessIterator &it_ref1 = ++it;
    const RandomAccessIterator &it_ref2 = it++;

    RandomAccessIterator &it_ref3 = --it;
    const RandomAccessIterator &it_ref4 = it--;

    RandomAccessIterator &it_ref5 = it += 1;
    it_cpy = it + d;
    it_cpy = d + it;

    RandomAccessIterator &it_ref6 = it -= 1;
    it_cpy = it - d;
#endif // DRQS

    T t (it [d]);

    Reference ref1 (*it);
    Reference ref2 (*it++);

    bool b (it == it);
    b = it != it;
    b = it >  it;
    b = it >= it;
    b = it <= it;


    _RWSTD_UNUSED (it);
    _RWSTD_UNUSED (it_cpy);
#ifdef DRQS // undefined variables because of above errors
    _RWSTD_UNUSED (it_ref1);
    _RWSTD_UNUSED (it_ref2);
    _RWSTD_UNUSED (it_ref3);
    _RWSTD_UNUSED (it_ref4);
    _RWSTD_UNUSED (it_ref5);
    _RWSTD_UNUSED (it_ref6);
#endif // DRQS
    _RWSTD_UNUSED (d);
    _RWSTD_UNUSED (t);
    _RWSTD_UNUSED (ref1);
    _RWSTD_UNUSED (ref2);
    _RWSTD_UNUSED (b);
}

/***************************************************************************/

// verify that nested types are properly defined
template <class Vector, class T, class Allocator>
void test_types (Vector*, T*, Allocator)
{
#define TEST_TYPE(T, vectorT)   {              \
        T *t = (typename Vector::vectorT*)0;   \
        _RWSTD_UNUSED (t);                     \
    }

#define TEST_REFERENCE(T, vectorT)   {   \
        typename Vector::value_type v;   \
        T t = v;                         \
        _RWSTD_UNUSED (t);               \
    }

    // verify nested types
    TEST_TYPE (T,                                   value_type);
    TEST_TYPE (Allocator,                           allocator_type);
    TEST_TYPE (typename Allocator::pointer,         pointer);
    TEST_TYPE (typename Allocator::const_pointer,   const_pointer);

    typedef std::reverse_iterator<typename Vector::iterator>
        ReverseIterator;
    typedef std::reverse_iterator<typename Vector::const_iterator>
      ConstReverseIterator;

#if TEST_RW_PEDANTIC || DRQS  // bslstl_ReverseIterator<int*>*’ differs from ‘bsl::reverse_iterator<int*>*’
    TEST_TYPE (ReverseIterator, reverse_iterator);
    TEST_TYPE (ConstReverseIterator, const_reverse_iterator);
#endif // TEST_RW_PEDANTIC || !DRQS

    test_iterator<typename Vector::iterator, T, T&, T*, std::ptrdiff_t>
        ti;   // exercise iterator requirements
    test_iterator<typename Vector::const_iterator, T, const T&, const T*,
                  std::ptrdiff_t>
        tci;   // exercise iterator requirements

    test_iterator<ReverseIterator, T, T&, T*, std::ptrdiff_t>
        tri;   // exercise iterator requirements
    test_iterator<ConstReverseIterator, T, const T&, const T*, std::ptrdiff_t>
        tcri;   // exercise iterator requirements

    _RWSTD_UNUSED (ti);  _RWSTD_UNUSED (tci);
    _RWSTD_UNUSED (tri); _RWSTD_UNUSED (tcri);

    // special treatment
    TEST_REFERENCE (typename Allocator::reference,       reference);
    TEST_REFERENCE (typename Allocator::const_reference, const_reference);
}

/**************************************************************************/

// verify that member functions are properly defined
template <class Vector, class T, class Allocator>
void test_signatures (Vector*, T*, Allocator)
{
// verify that a member function is accessible and has the appropriate
// signature, including return type and exception specification
#define MEMFUN(result, name, arg_list) do {              \
        result (Vector::*pf) arg_list = &Vector::name;   \
        _RWSTD_UNUSED (pf);                              \
    } while (0)

    MEMFUN (Vector&, operator=, (const Vector&));

    // helpers to work around a SunPro 5.3 bug (see PR #25972)
    typedef typename Vector::size_type  SizeType;
    typedef typename Vector::value_type ValueType;
    typedef typename Vector::iterator   Iterator;

    // verify signatures of iterator accessors
    MEMFUN (typename Vector::iterator,       begin, ());
    MEMFUN (typename Vector::const_iterator, begin, () const);
    MEMFUN (typename Vector::iterator,       end, ());
    MEMFUN (typename Vector::const_iterator, end, () const);

    // verify signatures of reverse iterator accessors
    MEMFUN (typename Vector::reverse_iterator,       rbegin, ());
    MEMFUN (typename Vector::const_reverse_iterator, rbegin, () const);
    MEMFUN (typename Vector::reverse_iterator,       rend, ());
    MEMFUN (typename Vector::const_reverse_iterator, rend, () const);

    // verify signatures of capacity accessors
    MEMFUN (SizeType, size, () const);
    MEMFUN (SizeType, max_size, () const);
#if TEST_RW_PEDANTIC // Pointer-to-function si signature mismatch
    MEMFUN (void, resize, (SizeType, ValueType));
#endif
    MEMFUN (SizeType, capacity, () const);
    MEMFUN (bool, empty, () const);
    MEMFUN (void, reserve, (SizeType));

    // verify signature of the element access functions
    MEMFUN (const T&, operator[], (SizeType) const);
    MEMFUN (T&,       operator[], (SizeType));
    MEMFUN (const T&, at,         (SizeType) const);
    MEMFUN (T&,       at,         (SizeType));
    MEMFUN (const T&, front, () const);
    MEMFUN (T&,       front, ());
    MEMFUN (const T&, back, () const);
    MEMFUN (T&,       back, ());

    // verify signatures of modifiers
    MEMFUN (void, push_back, (const T&));
    MEMFUN (void, pop_back, ());

#if TEST_RW_PEDANTIC // Pointer-to-function mismatch
    MEMFUN (Iterator, insert, (Iterator, const T&));

    MEMFUN (void, insert, (Iterator, SizeType, const T&));

#if !defined (_MSC_VER) || _MSC_VER >= 1300
    // member function template insert
    MEMFUN (void, insert, (Iterator, InputIter<T>, InputIter<T>));
#endif   // !defined (_MSC_VER) || _MSC_VER >= 1300

    MEMFUN (Iterator, erase, (Iterator));
    MEMFUN (Iterator, erase, (Iterator, Iterator));

    MEMFUN (void, swap, (Vector&));
#endif // TEST_RW_PEDANTIC
    MEMFUN (void, clear, ());

#if !defined (_MSC_VER) || _MSC_VER > 1300

#define FUN(result, name, arg_list) do {  \
        result (*pf) arg_list = &name;    \
        _RWSTD_UNUSED (pf);               \
    } while (0)

    // verify signatures of non-member functions
    FUN (bool, std::operator==, (const Vector&, const Vector&));
    FUN (bool, std::operator<,  (const Vector&, const Vector&));
    FUN (bool, std::operator!=, (const Vector&, const Vector&));
    FUN (bool, std::operator>,  (const Vector&, const Vector&));
    FUN (bool, std::operator>=, (const Vector&, const Vector&));
    FUN (bool, std::operator<=, (const Vector&, const Vector&));
    FUN (void, std::swap,       (Vector&, Vector&));

#else   // MSVC <= 7.0

    // working around a bug in MSVC 7 and prior (see PR #26625)

    if (0 /* compile only */) {

        Vector *pv = 0;
        const Vector *pcv = 0;

        bool b;
        b = std::operator== (*pcv, *pcv);
        b = std::operator<  (*pcv, *pcv);
        b = std::operator!= (*pcv, *pcv);
        b = std::operator>  (*pcv, *pcv);
        b = std::operator>= (*pcv, *pcv);
        b = std::operator<= (*pcv, *pcv);
        std::swap (*pv, *pv);
    }

#endif   // MSVC <= 7.0

}

/**************************************************************************/

// exercise [lib.vector.cons] and vector<>::operator=()
// focus on the correct construction and destruction of values
void test_ctors ()
{
    typedef std::vector<UserClass, std::allocator<UserClass> > Vector;

    if (1) {

        rw_info (0, 0, 0,
                 "std::vector<UserClass>::vector(size_type, "
                 "const_reference, const allocator_type&)");

        rw_info (0, 0, 0, "std::vector<UserClass>::vector(const vector&)");

        // reset function call counters
        UserClass::reset_totals ();

        // total number of objects of type UserClass in existence
        const std::size_t x_count = UserClass::count_;

        for (Vector::size_type i = 0; i != rw_opt_nloops; ++i) {

            rw_assert (UserClass::count_ == x_count, 0, __LINE__,
                       "vector<UserClass>::vector(size_type, const_reference); "
                       "leaked %zu objects of value_type",
                       UserClass::count_ - x_count);

            // reset function call counters
            UserClass::reset_totals ();

            const UserClass val;

            // initialize a vector with `i' copies of `val'
            const Vector v0 (i, val);
            rw_assert (   i == v0.size ()
                       && v0.begin ()  + i  == v0.end ()
                       && v0.rbegin () + i  == v0.rend (),
                       0, __LINE__,
                       "vector<UserClass>::vector(size_type, const_reference)");

            // verify that the vector ctor calls only copy ctor
            // of UserClass and anly the given number of times each
            rw_assert (UserClass::is_total (i + 1, 1, i, 0, 0, 0), 0, __LINE__,
                       "vector<UserClass>::vector(size_type, const_reference); "
                       "called default/copy ctor and operator=() %zu, %zu, "
                       "and %zu times, respectively, 0, %zu, 0 expected",
                       UserClass::n_total_def_ctor_ - 1,
                       UserClass::n_total_copy_ctor_,
                       UserClass::n_total_op_assign_, i);

            // create a copy
            Vector v1 (v0);
            rw_assert (   i == v1.size ()
                       && v1.begin  () + i == v1.end ()
                       && v1.rbegin () + i == v1.rend (),
                       0, __LINE__,
                       "vector<UserClass>::vector(const vector&)");

            // verify that the vector copy ctor calls only copy ctor
            // of UserClass and anly the given number of times each
            rw_assert (UserClass::is_total (2 * i + 1, 1, 2 * i, 0, 0, 0),
                       0, __LINE__,
                       "vector<UserClass>::vector(size_type, const_reference); "
                       "called default/copy ctor and operator=() %zu, %zu, "
                       "and %zu times, respectively, 0, %zu, 0 expected",
                       UserClass::n_total_def_ctor_ - 1,
                       UserClass::n_total_copy_ctor_,
                       UserClass::n_total_op_assign_, i);

            // exercise vector<>operator=(const vector&)
            Vector v2 (i, val);

            for (Vector::size_type j = 0; j != rw_opt_nloops; ++j) {

                Vector v3 (j, val);

                // assign a vector (of a possibly unequal size)
                v3 = v2;
                rw_assert (v3.size () == v2.size (), 0, __LINE__,
                           "%zu. vector<UserClass>::operator=(const vector&)",
                           j);

                rw_assert (v3 == v2, 0, __LINE__,
                           "%zu. vector<UserClass>::operator=(const vector&)",
                           j);
            }

#ifndef _RWSTD_NO_EXCEPTIONS

            // exercise vector exception safety

            bool thrown = false;

            // create a suitably aligned buffer in which to construct
            // the vector object
            union {
                void *pad;
                char buf [sizeof (Vector)];
            } buf = { 0 };

            std::size_t x_count_save = UserClass::count_;

            try {
                // have UserClass copy ctor throw an exception during
                // the copying of the last value
                UserClass::copy_ctor_throw_count_ =
                    UserClass::n_total_copy_ctor_ + i;

                // create a vector object, throw an exception
                // expect vector ctor to destroy any values
                // constructed prior to throwing the exception
                // and to free all already allocated memory

                // use placement new to prevent vector destruction
                // at scope exit (the object should be destroyed
                // during stack unwinding instead)
                Vector *p = new (&buf.buf) Vector (i, val);

                // destroy explicitly if ctor doesn't propagate exception

#if !defined (__HP_aCC) || _RWSTD_HP_aCC_MINOR > 3800

                p->~Vector ();
#else
                // work around aCC bug (see PR #25356)
                p->~vector ();

#endif   // HP aCC

            }
            catch (...) {
                thrown = true;
            }

            // no exception thrown if (i == 0), no elements constructed
            rw_assert (i == 0 || thrown, 0, __LINE__,
                       "logic error: failed to throw");

            rw_assert (x_count_save == UserClass::count_, 0, __LINE__,
                       "vector<UserClass>::vector(size_type, const_reference) "
                       "leaked %zu value(s) of %zu after an exception",
                       UserClass::count_ - x_count_save, i);

            // exercise vector<>::vector(const vector&)
            thrown       = false;
            x_count_save = UserClass::count_;

            try {
                // have UserClass copy ctor throw an exception during
                // the copying of the last value
                UserClass::copy_ctor_throw_count_ =
                    UserClass::n_total_copy_ctor_ + i;

                // use placement new to prevent vector destruction
                // at scope exit (the object should be destroyed
                // during stack unwinding instead)
                Vector *p = new (&buf) Vector (v1);

                // destroy explicitly if ctor doesn't propagate exception

#if !defined (__HP_aCC) || _RWSTD_HP_aCC_MINOR > 3800

                p->~Vector ();
#else
                // work around aCC bug (see PR #25356)
                p->~vector ();

#endif   // HP aCC

            }
            catch (...) {
                // vector should have been destroyed
                thrown = true;
            }

            // no exception thrown if (i == 0), no elements constructed
            rw_assert(i == 0 || thrown, 0, __LINE__,
                      "logic error: failed to throw");

            rw_assert (x_count_save == UserClass::count_, 0, __LINE__,
                       "vector<UserClass>::vector(const vector&) leaked "
                       "%zu value(s) of %zu after an exception",
                       UserClass::count_ - x_count_save, i);


            // disable exceptions
            UserClass::copy_ctor_throw_count_ = std::size_t (-1);

            // remember v1's size and capacity
            const Vector::size_type v1_size = v1.size ();
            const Vector::size_type v1_cap  = v1.capacity ();

            // create and initialize vector with some values
            // make sure v3's capacity is greater than that of v1
            // (otherwise the assignment isn't exception safe since
            // it simply overwrites existing values for efficiency)
            const Vector v3 (v1_cap * 2, val);

            // exrecise vector<>::operator=(const vector&)
            thrown       = false;
            x_count_save = UserClass::count_;

            try {
                // have UserClass copy ctor throw an exception during
                // the copying of the last value
                UserClass::copy_ctor_throw_count_ =
                    UserClass::n_total_copy_ctor_ + v3.size ();

                // assign over the existing elements, the last copy ctor or
                // operator=() throws, destroying all values assigned so far
                v1 = v3;
            }
            catch (...) {
                // original vector must remain unchanged
                thrown = true;
            }

            // disable exceptions
            UserClass::copy_ctor_throw_count_ = std::size_t (-1);

            rw_assert (i == 0 || thrown, 0, __LINE__,
                       "logic error: failed to throw");

            // verify that no values leaked
            rw_assert (x_count_save == UserClass::count_, 0, __LINE__,
                       "vector<UserClass>::vector(const vector&) leaked "
                       "%zu value(s) of %zu after an exception",
                       UserClass::count_ - x_count_save, i);

            // verify that the size of the left hand size operand
            // of the assignment hasn't changed
            rw_assert (v1.size () == v1_size && v1.capacity () == v1_cap,
                       0, __LINE__, "vector<UserClass>::operator="
                       "(const vector&) changed size of *this from "
                       "%zu to %zu after exception",
                       i / 2, v3.size ());

#endif   // _RWSTD_NO_EXCEPTIONS

            // vectors go out of scope, must destroy all elements
        }
    }
}

// exercise [lib.vector.cons]
template <class Vector, class T, class Alloc>
void test_ctors (Vector*, T*, Alloc alloc)
{
    if (1) {

        rw_info (0, 0, 0,
                 "std::vector<UserClass>::vector(const allocator_type&)");

        // verify default ctor arguments
        Vector v0;
        Vector v1 (alloc);

        rw_assert (   0 == v0.size ()
                   && v0.empty () && v0.begin () == v0.end ()
                   && v0.rbegin () == v0.rend (), 0, __LINE__,
                      ("vector<UserClass>::vector()"));

        rw_assert (   0 == v1.size ()
                   && v1.empty () && v1.begin () == v1.end ()
                   && v1.rbegin () == v1.rend (), 0, __LINE__,
                      "vector<UserClass>::vector()");
    }

    if (1) {

        rw_info (0, 0, 0,
                 "std::vector<UserClass>::vector(size_type, "
                 "const_reference, const allocator_type&)");

        for (typename Vector::size_type i = 0; i != rw_opt_nloops; ++i) {

            const T val = i;

            Vector v0 (i, val);
            Vector v1 (i, val, alloc);

            rw_assert (   i == v0.size ()
                       && v0.begin  () + i == v0.end ()
                       && v0.rbegin () + i == v0.rend (),
                          0, __LINE__,
                          "vector<UserClass>::vector"
                          "(size_type, const_reference)");

            rw_assert (   i == v1.size ()
                       && v1.begin  () + i == v1.end ()
                       && v1.rbegin () + i == v1.rend (),
                          0, __LINE__,
                          "vector<UserClass>::vector(size_type, "
                          "const_reference, const allocator_type&)");

            bool success = true;
            for (typename Vector::size_type j = 0; j != i; ++j) {
                if (!(success = v0 [j] == val))
                    break;
                if (!(success = v1 [j] == val))
                    break;
            }

            rw_assert (success, 0, __LINE__,
                       "vector<UserClass>::vector(size_type, const_reference); "
                       "all elements initialized");
        }
    }

#if !defined (_MSC_VER) || _MSC_VER >= 1300

    if (1) {

        rw_info (0, 0, 0,
                 "template <class InputIterator> std::vector<UserClass>::vector"
                 "(InputIterator, InputIterator)");

        bool success = true;

        // allocate nloops elements, do not initialize
        typename Vector::value_type *vals = alloc.allocate (rw_opt_nloops);

        for (typename Vector::size_type i = 0; i != rw_opt_nloops; ++i) {

            // construct an element at then end of array
            alloc.construct (vals + i, i);

            // verify ctor with a strict InputIterator
            InputIter<T> first (vals,     vals,     vals + i);
            InputIter<T> last  (vals + i, vals + i, vals + i);

            const Vector v0 (first, last);

            // reset iterators since they are single-pass and their
            // copies have been passed through by the ctor above
            first = InputIter<T>(vals,     vals,     vals + i);
            last  = InputIter<T>(vals + i, vals + i, vals + i);

            const Vector v1 (first, last, alloc);

            if (   i != v0.size ()
                || (!(i && !v0.empty () || !i && v0.empty ()))
                || (i != v1.size ())
                || (!(i && !v1.empty () || !i && v1.empty ())))
                success = false;

            // verify size() and empty()
            rw_assert (i == v0.size (), 0, __LINE__,
                       "size () == %zu, got %zu", i, v0.size ());

            rw_assert (i && !v0.empty () || !i && v0.empty (),
                       0, __LINE__, "size () == %zu, empty () == %d",
                       v0.size (), v0.empty ());

            rw_assert (i == v1.size (), 0, __LINE__,
                       "size () == %zu, got %zu", i, v1.size ());

            rw_assert (i && !v1.empty () || !i && v1.empty (),
                       0, __LINE__, "size () == %zu, empty () == %d",
                       v1.size (), v1.empty ());
        }

        rw_assert (success, 0, __LINE__,
                   "template <class InputIterator> "
                   "std::vector<UserClass>::vector"
                   "(InputIterator, InputIterator)");

        // destroy and deallocate...
        for (typename Vector::size_type j = 0; j != rw_opt_nloops; ++j)
            alloc.destroy (vals + j);

        alloc.deallocate (vals, rw_opt_nloops);
    }

#endif   // !defined (_MSC_VER) || _MSC_VER >= 1300

}

/**************************************************************************/

static int
run_test (int /* argc */, char** /* argv */)
{
    static int int_noted   = 0;
    static int short_noted = 0;
    static int types_noted = 0;
    static int sign_noted  = 0;
    static int ctors_noted = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

#  if TEST_RW_EXTENSIONS // ??? What do they mean "initialize the lib"
    try {
        // throw an exception to initialize the lib (allocates
        // memory that's never deallocated; shows up as leaks)
        _RW::__rw_throw (_RWSTD_ERROR_LOGIC_ERROR, "", "");
    }
    catch (...) {
    }
#  endif     // TEST_RW_EXTENSIONS

#endif   // _RWSTD_NO_EXCEPTIONS

// for convenience (in case default template arguments are disabled)
#define VECTOR(T)  std::vector<T, std::allocator<T> >

#define TEST(what, T)                                                     \
    do {                                                                  \
        /* establish a checkpoint for memory leaks */                     \
        rwt_check_leaks (0, 0);                                           \
        test_ ##what ((VECTOR (T)*)0, (T*)0, std::allocator<T>());        \
        /* verify that no memory leaked */                                \
        std::size_t nbytes;                                               \
        const std::size_t nblocks = rwt_check_leaks (&nbytes, 0);         \
        rw_assert (!nblocks && !nbytes, 0, __LINE__,                      \
                   #what " test leaked %lu bytes in %lu blocks",          \
                   nbytes, nblocks);                                      \
    } while (0)

    if (rw_opt_no_int_spec) {
        rw_note (int_noted++, 0, __LINE__,
                 "int specializations test disabled.");
    }
    else {
        // exercise vector and its default template argument
        if (rw_opt_no_types) {
            rw_note (types_noted++, 0, __LINE__, "Types test disabled.");
        }
        else {
            TEST (types, int);
        }

        if (rw_opt_no_signatures) {
            rw_note (sign_noted++, 0, __LINE__, "Signatures test disabled.");
        }
        else {
            TEST (signatures, int);
        }

        if (rw_opt_no_ctors) {
            rw_note (ctors_noted++, 0, __LINE__, "Ctors test disabled.");
        }
        else {
            TEST (ctors, int);
        }
    }

    // exercise vector with a template argument other than the default
#undef  VECTOR
#define VECTOR(T, A) std::vector<T, A<T> >

#undef  TEST
#define TEST(what,T,A) test_ ##what ((VECTOR (T, A)*)0, (T*)0, A<T>())

    if (rw_opt_no_short_spec) {
        rw_note (short_noted++, 0, __LINE__,
                 "short specializations test disabled.");
    }
    else {
        if (rw_opt_no_types) {
            rw_note (types_noted++, 0, __LINE__, "Types test disabled.");
        }
        else {
            TEST (types, short, std::allocator);
        }

        if (rw_opt_no_signatures) {
            rw_note (sign_noted++, 0, __LINE__, "Signatures test disabled.");
        }
        else {
            TEST (signatures, short, std::allocator);
        }

        if (rw_opt_no_ctors) {
            rw_note (ctors_noted++, 0, __LINE__, "Ctors test disabled.");
        }
        else {
            TEST (ctors, short, std::allocator);
        }
    }

    if (rw_opt_no_ctors) {
        rw_note (ctors_noted++, 0, __LINE__, "Ctors test disabled.");
    }
    else {
        test_ctors ();
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char** argv)
{
    return rw_test (argc, argv, __FILE__,
                    "lib.vector.cons",
                    0 /* no comment */,
                    run_test,
                    "|-nloops#1 "
                    "|-no-int_specializations# "
                    "|-no-short_specializations# "
                    "|-no-types# "
                    "|-no-signatures# "
                    "|-no-ctors#",
                    &rw_opt_nloops,
                    &rw_opt_no_int_spec,
                    &rw_opt_no_short_spec,
                    &rw_opt_no_types,
                    &rw_opt_no_signatures,
                    &rw_opt_no_ctors);
}
