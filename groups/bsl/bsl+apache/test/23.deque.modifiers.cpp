/***************************************************************************
 *
 * 23.deque.modifiers.cpp - test exercising [lib.deque.modifiers]
 *
 * $Id: 23.deque.modifiers.cpp 550991 2007-06-26 23:58:07Z sebor $
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#ifdef _MSC_VER
   // silence warning C4244: 'argument' : conversion from 'T' to
   // 'const std::allocator<_TypeT>::value_type', possible loss of data
   // issued for deque::assign(InputIterator a, InputIterator b) and
   // deque::insert(iterator, InputIterator a, InputIterator b) due
   // the implicit conversion of a to size_type and b to value_type
   // required by DR 438:
   // http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#438
#  pragma warning (disable: 4244)
#endif

#include <deque>     // for deque

#include <cstdlib>   // for free()

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE
   // disabled for MSVC since it can't reliably replace the operators
#  include <rw_new.h>
#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

#include <rw_value.h>    // for UserClass
#include <driver.h>      // for rw_test(), ...
#include <rw_printf.h>   // for rw_asnprintf

/**************************************************************************/

// Runtime options
/* extern */ int rw_opt_no_assign = 0;
/* extern */ int rw_opt_no_erase  = 0;
/* extern */ int rw_opt_no_insert = 0;
/* extern */ int rw_opt_no_dr438  = 0;
/* extern */ int rw_opt_no_input_iterator = 0;
/* extern */ int rw_opt_no_forward_iterator = 0;
/* extern */ int rw_opt_no_bidirectional_iterator = 0;
/* extern */ int rw_opt_no_random_iterator = 0;
/* extern */ int rw_opt_no_right_thing = 0;

/**************************************************************************/

// For konvenience
typedef unsigned char UChar;

/**************************************************************************/

typedef std::deque<UserClass, std::allocator<UserClass> > Deque;

Deque::size_type new_capacity;

#if TEST_RW_EXTENSIONS
namespace __rw {

_RWSTD_SPECIALIZED_FUNCTION
inline Deque::size_type
__rw_new_capacity<Deque>(Deque::size_type n, const Deque*)
{
    if (n) {
        // non-zero size argument indicates a request for an increase
        // in the capacity of a deque object's dynamically sizable
        // vector of nodes
        return n * 2;
    }

    // zero size argument is a request for the initial size of a deque
    // object's dynamically sizable vector of nodes or for the size of
    // the objects's fixed-size buffer for elements
    return new_capacity;
}

}
#endif // TEST_RW_EXTENSIONS

/**************************************************************************/

enum {
    NewThrows = 0x1          /* cause operator new to throw */,
    CopyCtorThrows = 0x2     /* cause element's copy ctor to throw */,
    AssignmentThrows = 0x4   /* cause element's assignment to throw */
};

enum MemberFunction {
    Assign_n    /* deque::assign (size_type, const_reference) */,
    AssignRange /* deque::assign (InputIterator, InputIterator) */,

    Erase_1     /* deque::erase (iterator) */,
    EraseRange  /* deque::erase (iterator, iterator) */,

    Insert_1    /* deque::insert (iterator, const_reference) */,
    Insert_n    /* deque::insert (iterator, size_type, const_reference) */,
    InsertRange /* deque::insert (iterator, InputIterator, InputIterator) */
};


// causes operator new, deque element's copy ctor, or assignment operator
// to throw an exception and iterates as long as the member function exits
// by throwing an exception; verifies that the exception had no effects
// on the container
template <class Iterator>
void exception_loop (int              line /* line number in caller*/,
                     MemberFunction   mfun /* deque member function */,
                     const char      *fcall /* function call string */,
                     int              exceptions /* enabled exceptions */,
                     Deque           &deq /* container to call function on */,
                     const Deque::iterator &it /* iterator into container */,
                     int              n /* number of elements or offset */,
                     const UserClass *x /* pointer to an element or 0 */,
                     const Iterator  &first /* beginning of range */,
                     const Iterator  &last /* end of range to insert */,
                     int             *n_copy /* number of copy ctors */,
                     int             *n_asgn /* number of assignments */)
{
    std::size_t throw_after = 0;

    // get the initial size of the container and its begin() iterator
    // to detect illegal changes after an exception (i.e., violations
    // if the strong exception guarantee)
    const std::size_t           size  = deq.size ();
    const Deque::const_iterator begin = deq.begin ();
    const Deque::const_iterator end   = deq.end ();

#ifdef DEFINE_REPLACEMENT_NEW_AND_DELETE

    rwt_free_store* const pst = rwt_get_free_store (0);

#endif   // DEFINE_REPLACEMENT_NEW_AND_DELETE

    // repeatedly call the specified member function until it returns
    // without throwing an exception
    for ( ; ; ) {

        // detect objects constructed but not destroyed after an exception
        std::size_t x_count = UserClass::count_;

        _RWSTD_ASSERT (n_copy);
        _RWSTD_ASSERT (n_asgn);

        *n_copy = UserClass::n_total_copy_ctor_;
        *n_asgn = UserClass::n_total_op_assign_;

#ifndef _RWSTD_NO_EXCEPTIONS

        // iterate for `n=throw_after' starting at the next call to operator
        // new, forcing each call to throw an exception, until the insertion
        // finally succeeds (i.e, no exception is thrown)

#  ifdef DEFINE_REPLACEMENT_NEW_AND_DELETE

        if (exceptions & NewThrows) {
            *pst->throw_at_calls_ [0] = pst->new_calls_ [0] + throw_after + 1;
        }

#  endif   // DEFINE_REPLACEMENT_NEW_AND_DELETE

        if (exceptions & CopyCtorThrows) {
            UserClass::copy_ctor_throw_count_ =
                UserClass::n_total_copy_ctor_ + throw_after;
        }

        if (exceptions & AssignmentThrows) {
            UserClass::op_assign_throw_count_ =
                UserClass::n_total_op_assign_ + throw_after;
        }

#endif   // _RWSTD_NO_EXCEPTIONS

        _TRY {

            switch (mfun) {
            case Assign_n:
                _RWSTD_ASSERT (x);
                deq.assign (n, *x);
                break;
            case AssignRange:
                deq.assign (first, last);
                break;

            case Erase_1:
                deq.erase (it);
                break;
            case EraseRange: {
                const Deque::iterator erase_end (it + n);
                deq.erase (it, erase_end);
                break;
            }

            case Insert_1:
                _RWSTD_ASSERT (x);
                deq.insert (it, *x);
                break;
            case Insert_n:
                _RWSTD_ASSERT (x);
                deq.insert (it, n, *x);
                break;
            case InsertRange:
                deq.insert (it, first, last);
                break;
            }
        }
        _CATCH (...) {

            // verify that an exception thrown from the member function
            // didn't cause a change in the state of the container

            rw_assert (deq.size () == size, 0, line,
                       "line %d: %s: size unexpectedly changed "
                       "from %zu to %zu after an exception",
                       __LINE__, fcall, size, deq.size ());
            
            rw_assert (deq.begin () == begin, 0, line,
                       "line %d: %s: begin() unexpectedly "
                       "changed after an exception by %td",
                       __LINE__, fcall, deq.begin () - begin);

            rw_assert (deq.end () == end, 0, line,
                       "line %d: %s: end() unexpectedly "
                       "changed after an exception by %td",
                       __LINE__, fcall, deq.end () - end);
            

            // count the number of objects to detect leaks
            x_count = UserClass::count_ - x_count;
            rw_assert (x_count == deq.size () - size, 0, line,
                       "line %d: %s: leaked %zu objects after an exception",
                       __LINE__, fcall, x_count - (deq.size () - size));
            
            if (exceptions) {

                // increment to allow this call to operator new to succeed
                // and force the next one to fail, and try to insert again
                ++throw_after;
            }
            else
                break;

            continue;
        }

        // count the number of objects to detect leaks
        x_count = UserClass::count_ - x_count;
        rw_assert (x_count == deq.size () - size, 0, line,
                   "line %d: %s: leaked %zu objects "
                   "after a successful insertion",
                   __LINE__, fcall, x_count - (deq.size () - size));

        break;
    }

#ifdef DEFINE_REPLACEMENT_NEW_AND_DELETE

    // disable exceptions from replacement operator new
    *pst->throw_at_calls_ [0] = _RWSTD_SIZE_MAX;

#endif   // DEFINE_REPLACEMENT_NEW_AND_DELETE

    UserClass::copy_ctor_throw_count_ = 0;
    UserClass::op_assign_throw_count_ = 0;

    // compute the number of calls to UserClass copy ctor and assignment
    // operator and set `n_copy' and `n_assgn' to the value of the result
    *n_copy = UserClass::n_total_copy_ctor_ - *n_copy;
    *n_asgn = UserClass::n_total_op_assign_ - *n_asgn;
}


// used to determine whether insert() can or cannot use
// an algorithm optimized for BidirectionalIterators
bool is_bidirectional (std::input_iterator_tag) { return false; }
bool is_bidirectional (std::bidirectional_iterator_tag) { return true; }

// returns the number of invocations of the assignment operators
// for a call to deque::insert(iterator, InputIterator, InputIterator)
// (the value depends on the iterator category)
template <class Iterator>
std::size_t insert_assignments (Iterator it,
                                int nelems,
                                std::size_t off,
                                std::size_t seqlen,
                                std::size_t inslen)
{
    if (is_bidirectional (_RWSTD_ITERATOR_CATEGORY (Iterator, it)))
        return 0 == nelems ? 0 : off < seqlen - off ? off : seqlen - off;

    if (0 < nelems)
        --nelems;

    if (0 == nelems || 0 == inslen)
        return 0;

    // compute the number of assignments done
    // to insert the first element in the sequence
    const std::size_t first = off < seqlen - off ? off : seqlen - off;

    // recursively compute the numner of assignments
    // for the rest of the elements in the sequence
    const std::size_t rest =
        insert_assignments (it, nelems, off + 1, seqlen + 1, inslen - 1);

    return first + rest;
}


template <class Iterator>
void test_insert (int line, int exceptions,
                  const Iterator &dummy, int nelems,
                  const char *seq, std::size_t seqlen, std::size_t off, 
                  const char *ins, std::size_t inslen, 
                  const char *res, std::size_t reslen)
{
    // Ensure that xsrc, xins are always dereferenceable
    const UserClass* const xseq = UserClass::from_char (seq, seqlen + 1);
          UserClass* const xins = UserClass::from_char (ins, inslen + 1);

    Deque deq = seqlen ? Deque (xseq, xseq + seqlen) : Deque ();

    // offset must be valid
    _RWSTD_ASSERT (off <= deq.size ());
    const Deque::iterator iter = deq.begin () + off;

    // only insert() at either end of the container is exception safe
    // insertions into the middle of the container are not (i.e., the
    // container may grow or may even become inconsistent)
    if (off && off < deq.size ())
        exceptions = 0;

    // format a string describing the function call being exercised
    // (used in diagnostic output below)
    char* funcall = 0;
    std::size_t len = 0;

    rw_asnprintf (&funcall, &len, "deque(\"%{X=*.*}\").insert("
                  "%{?}begin(), %{:}%{?}end (), %{:}begin () + %zu%{;}%{;}"
                  "%{?}%d)%{:}%{?}\"%{X=*.*}\")%{:}%d, %d)%{;}%{;}",
                  int (seqlen), -1, xseq, 0 == off, seqlen == off, off,
                  nelems == -2, *ins, nelems == -1, 
                  int (inslen), -1, xins, nelems, *ins);

    int n_copy = UserClass::n_total_copy_ctor_;
    int n_asgn = UserClass::n_total_op_assign_;

    if (-2 == nelems) {   // insert(iterator, const_reference)

        exception_loop (line, Insert_1, funcall, exceptions,
                        deq, iter, nelems, xins, dummy, dummy,
                        &n_copy, &n_asgn);

    }
    else if (-1 == nelems) {   // insert(iterator, Iterator, Iterator)

        if (inslen > 1)
            exceptions = 0;

        const Iterator first =
            make_iter (xins, xins, xins + inslen, dummy);

        const Iterator last =
            make_iter (xins + inslen, xins, xins + inslen, dummy);

        exception_loop (line, InsertRange, funcall, exceptions,
                        deq, iter, nelems, 0, first, last,
                        &n_copy, &n_asgn);

    }
    else {   // insert(iterator, size_type, const_reference)

        if (nelems > 1)
            exceptions = 0;

        exception_loop (line, Insert_n, funcall, exceptions,
                        deq, iter, nelems, xins, dummy, dummy,
                        &n_copy, &n_asgn);

    }

    // verify the expected size of the deque after insertion
    rw_assert (deq.size () == reslen, __FILE__, line,
               "line %d: %s: size == %zu, got %zu\n",
               __LINE__, funcall, reslen, deq.size ());

    // verify the expected contents of the deque after insertion
    const Deque::const_iterator resbeg = deq.begin ();
    const Deque::const_iterator resend = deq.end ();

    for (Deque::const_iterator it = resbeg; it != resend; ++it) {
        if ((*it).data_.val_ != UChar (res [it - resbeg])) {

            char* const got = new char [deq.size () + 1];

            for (Deque::const_iterator i = resbeg; i != resend; ++i) {
                got [i - resbeg] = char ((*i).data_.val_);
            }

            got [deq.size ()] = '\0';

            rw_assert (false, __FILE__, line,
                       "line %d: %s: expected %s, got %s\n",
                       __LINE__, funcall, res, got);

            delete[] got;
            break;
        }
    }

    // verify the complexity of the operation in terms of the number
    // of calls to the copy ctor and assignment operator on value_type
    const std::size_t expect_copy = nelems < 0 ? inslen : nelems;

#if TEST_RW_EXTENSIONS  // doesn't make sense to test the number of copies,
                        // especially with input iterators.  Definitely need
                        // more than this.
    rw_assert (n_copy == int (expect_copy),
               __FILE__, line,
               "line %d: %s: expected %zu invocations "
               "of UserClass::UserClass(const UserClass&), got %d\n",
               __LINE__, funcall, expect_copy, n_copy);
#endif

    // compute the number of calls to the assignment operator
    const std::size_t expect_asgn =
        insert_assignments (dummy, nelems, off, seqlen, inslen);
#if TEST_RW_EXTENSIONS  // doesn't make sense to test the number of assignments
    rw_assert (n_asgn == int (expect_asgn), 
               __FILE__, line,
               "line %d: %s: expected %zu invocations "
               "of UserClass::operator=(const UserClass&), got %d\n",
               __LINE__, funcall, expect_asgn, n_asgn);
#endif

    // Free funcall storage
    std::free (funcall);

    delete[] xins;
    delete[] xseq;
}

/**************************************************************************/

template <class Iterator>
void test_insert_range (const Iterator &it, const char* itname)
{
    rw_info (0, 0 ,0, 
             "std::deque<UserClass>::insert(iterator, %s, %s)", itname, itname);

#undef TEST
#define TEST(seq, off, ins, res)                        \
    test_insert (__LINE__, -1,                          \
                            it, -1,                     \
                            seq, sizeof seq - 1,        \
                            std::size_t (off),          \
                            ins, sizeof ins - 1,        \
                            res, sizeof res - 1)

    //    +---------------------------------------- seq
    //    |      +--------------------------------- off
    //    |      |   +----------------------------- ins
    //    |      |   |      +---------------------- res
    //    |      |   |      |
    //    v      v   v      v
    TEST ("",    +0, "",    "");
    TEST ("",    +0, "a",   "a");
    TEST ("",    +0, "ab",  "ab");
    TEST ("",    +0, "abc", "abc");
    TEST ("a",   +0, "",    "a");
    TEST ("b",   +0, "a",   "ab");
    TEST ("c",   +0, "ab",  "abc");
    TEST ("cd",  +0, "ab",  "abcd");
    TEST ("def", +0, "abc", "abcdef");

    TEST ("a",   +1, "",    "a");
    TEST ("a",   +1, "b",   "ab");
    TEST ("a",   +1, "bc",  "abc");
    TEST ("a",   +1, "bcd", "abcd");

    TEST ("ab",  +1, "",    "ab");
    TEST ("ac",  +1, "b",   "abc");
    TEST ("acd", +1, "b",   "abcd");

    TEST ("ab",  +2, "",    "ab");
    TEST ("ab",  +2, "c",   "abc");
    TEST ("ab",  +2, "cd",  "abcd");

    TEST ("abc", +2, "",    "abc");
    TEST ("abd", +2, "c",   "abcd");
    TEST ("abe", +2, "cd",  "abcde");
    TEST ("abf", +2, "cde", "abcdef");

    TEST ("abc", +3, "",    "abc");
    TEST ("abc", +3, "d",   "abcd");
    TEST ("abc", +3, "de",  "abcde");
    TEST ("abc", +3, "def", "abcdef");
    

#define UPPER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define LOWER "abcdefghijklmnopqrstuvwxyz"

    TEST (UPPER,  +0, LOWER, "" LOWER "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER,  +1, LOWER, "A" LOWER "BCDEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER,  +2, LOWER, "AB" LOWER "CDEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER,  +3, LOWER, "ABC" LOWER "DEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER,  +4, LOWER, "ABCD" LOWER "EFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER,  +5, LOWER, "ABCDE" LOWER "FGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER,  +6, LOWER, "ABCDEF" LOWER "GHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER,  +7, LOWER, "ABCDEFG" LOWER "HIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER,  +8, LOWER, "ABCDEFGH" LOWER "IJKLMNOPQRSTUVWXYZ");
    TEST (UPPER,  +9, LOWER, "ABCDEFGHI" LOWER "JKLMNOPQRSTUVWXYZ");
    TEST (UPPER, +10, LOWER, "ABCDEFGHIJ" LOWER "KLMNOPQRSTUVWXYZ");
    TEST (UPPER, +11, LOWER, "ABCDEFGHIJK" LOWER "LMNOPQRSTUVWXYZ");
    TEST (UPPER, +12, LOWER, "ABCDEFGHIJKL" LOWER "MNOPQRSTUVWXYZ");
    TEST (UPPER, +13, LOWER, "ABCDEFGHIJKLM" LOWER "NOPQRSTUVWXYZ");
    TEST (UPPER, +14, LOWER, "ABCDEFGHIJKLMN" LOWER "OPQRSTUVWXYZ");
    TEST (UPPER, +15, LOWER, "ABCDEFGHIJKLMNO" LOWER "PQRSTUVWXYZ");
    TEST (UPPER, +16, LOWER, "ABCDEFGHIJKLMNOP" LOWER "QRSTUVWXYZ");
    TEST (UPPER, +17, LOWER, "ABCDEFGHIJKLMNOPQ" LOWER "RSTUVWXYZ");
    TEST (UPPER, +18, LOWER, "ABCDEFGHIJKLMNOPQR" LOWER "STUVWXYZ");
    TEST (UPPER, +19, LOWER, "ABCDEFGHIJKLMNOPQRS" LOWER "TUVWXYZ");
    TEST (UPPER, +20, LOWER, "ABCDEFGHIJKLMNOPQRST" LOWER "UVWXYZ");
    TEST (UPPER, +21, LOWER, "ABCDEFGHIJKLMNOPQRSTU" LOWER "VWXYZ");
    TEST (UPPER, +22, LOWER, "ABCDEFGHIJKLMNOPQRSTUV" LOWER "WXYZ");
    TEST (UPPER, +23, LOWER, "ABCDEFGHIJKLMNOPQRSTUVW" LOWER "XYZ");
    TEST (UPPER, +24, LOWER, "ABCDEFGHIJKLMNOPQRSTUVWX" LOWER "YZ");
    TEST (UPPER, +25, LOWER, "ABCDEFGHIJKLMNOPQRSTUVWXY" LOWER "Z");
    TEST (UPPER, +26, LOWER, "ABCDEFGHIJKLMNOPQRSTUVWXYZ" LOWER "");
}

/**************************************************************************/

template <class T, class IntType>
void test_insert_int_range (const T&, const IntType&,
                            const char* t_name, const char* int_name)
{
    rw_info (0, 0, 0, 
             "std::deque<%s>::insert(iterator, %s, %s)", 
             t_name, int_name, int_name);

    std::deque<T> d;

    typename std::deque<T>::iterator it = d.begin ();

    // deque<T>::insert(iterator, size_type, const_reference)

    d.insert (it, IntType (1), IntType (0));

    rw_assert (1 == d.size (), 0, __LINE__,
               "deque<%s>::insert(begin(), %s = 1, %s = 0); size() == 1,"
               " got %zu", t_name, int_name, int_name, d.size ());

    it = d.begin ();
    ++it;

    d.insert (it, IntType (3), IntType (2));

    rw_assert (4 == d.size (), 0, __LINE__,
               "deque<%s>::insert(begin() + 1, %s = 3, %s = 2); size() == 4,"
               " got %zu", t_name, int_name, int_name, d.size ());

    it = d.begin ();
    ++it;

    d.insert (it, IntType (2), IntType (1));

    rw_assert (6 == d.size (), 0, __LINE__,
               "deque<%s>::insert(begin() + 1, %s = 2, %s = 1); size() == 6,"
               " got %zu", t_name, int_name, int_name, d.size ());
}


template <class T>
void test_insert_int_range (const T &dummy, const char* tname)
{
    test_insert_int_range (dummy, (signed char)0, tname, "signed char");
    test_insert_int_range (dummy, (unsigned char)0, tname, "unsigned char");
    test_insert_int_range (dummy, short (), tname, "short");
    test_insert_int_range (dummy, (unsigned short)0, tname, "unsigned short");
    test_insert_int_range (dummy, int (), tname, "int");
    test_insert_int_range (dummy, (unsigned int)0, tname, "unsigned int");
    test_insert_int_range (dummy, long (), tname, "long");
    test_insert_int_range (dummy, (unsigned long)0, tname, "unsigned long");

#ifdef _RWSTD_LONG_LONG

    test_insert_int_range (dummy, (_RWSTD_LONG_LONG)0, 
                           tname, "long long");
    test_insert_int_range (dummy, (unsigned _RWSTD_LONG_LONG)0, 
                           tname, "unsigned long long");

#endif   // _RWSTD_LONG_LONG

}

/**************************************************************************/

void test_insert ()
{
    //////////////////////////////////////////////////////////////////
    // exercise deque::insert(iterator, const_reference)

    rw_info (0, 0, 0,
             "std::deque<UserClass>::insert(iterator, const_reference)");

#undef TEST
#define TEST(seq, off, ins, res) do {                   \
      const char insseq [] = { ins, '\0' };             \
      test_insert (__LINE__, -1,                        \
                              (UserClass*)0, -2,        \
                              seq, sizeof seq - 1,      \
                              std::size_t (off),        \
                              insseq, 1,                \
                              res, sizeof res - 1);     \
    } while (0)

    //    +------------------- original sequence
    //    |       +----------- insertion offset
    //    |       |    +------ element to insert
    //    |       |    |   +-- resulting sequence
    //    |       |    |   |
    //    V       V    V   V
    TEST ("",     +0, 'a', "a");
    TEST ("b",    +0, 'a', "ab");
    TEST ("bc",   +0, 'a', "abc");
    TEST ("bcd",  +0, 'a', "abcd");
    TEST ("bcde", +0, 'a', "abcde");

    TEST ("a",    +1, 'b', "ab");
    TEST ("ac",   +1, 'b', "abc");
    TEST ("acd",  +1, 'b', "abcd");
    TEST ("acde", +1, 'b', "abcde");

    TEST ("ab",   +2, 'c', "abc");
    TEST ("abd",  +2, 'c', "abcd");
    TEST ("abde", +2, 'c', "abcde");

    TEST ("abc",  +3, 'd', "abcd");
    TEST ("abce", +3, 'd', "abcde");

    TEST ("abcd", +4, 'e', "abcde");

#define A_to_B "AB"
#define A_to_C "ABC"
#define A_to_D "ABCD"
#define A_to_E "ABCDE"
#define A_to_F "ABCDEF"
#define A_to_G "ABCDEFG"
#define A_to_H "ABCDEFGH"
#define A_to_I "ABCDEFGHI"
#define A_to_J "ABCDEFGHIJ"
#define A_to_K "ABCDEFGHIJK"
#define A_to_L "ABCDEFGHIJKL"
#define A_to_M "ABCDEFGHIJKLM"
#define A_to_N "ABCDEFGHIJKLMN"
#define A_to_O "ABCDEFGHIJKLMNO"
#define A_to_P "ABCDEFGHIJKLMNOP"
#define A_to_Q "ABCDEFGHIJKLMNOPQ"
#define A_to_R "ABCDEFGHIJKLMNOPQR"
#define A_to_S "ABCDEFGHIJKLMNOPQRS"
#define A_to_T "ABCDEFGHIJKLMNOPQRST"
#define A_to_U "ABCDEFGHIJKLMNOPQRSTU"
#define A_to_V "ABCDEFGHIJKLMNOPQRSTUV"
#define A_to_W "ABCDEFGHIJKLMNOPQRSTUVW"
#define A_to_X "ABCDEFGHIJKLMNOPQRSTUVWX"
#define A_to_Y "ABCDEFGHIJKLMNOPQRSTUVWXY"
#define A_to_Z "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define B_to_Z "BCDEFGHIJKLMNOPQRSTUVWXYZ"
#define C_to_Z "CDEFGHIJKLMNOPQRSTUVWXYZ"
#define D_to_Z "DEFGHIJKLMNOPQRSTUVWXYZ"
#define E_to_Z "EFGHIJKLMNOPQRSTUVWXYZ"
#define F_to_Z "FGHIJKLMNOPQRSTUVWXYZ"
#define G_to_Z "GHIJKLMNOPQRSTUVWXYZ"
#define H_to_Z "HIJKLMNOPQRSTUVWXYZ"
#define I_to_Z "IJKLMNOPQRSTUVWXYZ"
#define J_to_Z "JKLMNOPQRSTUVWXYZ"
#define K_to_Z "KLMNOPQRSTUVWXYZ"
#define L_to_Z "LMNOPQRSTUVWXYZ"
#define M_to_Z "MNOPQRSTUVWXYZ"
#define N_to_Z "NOPQRSTUVWXYZ"
#define O_to_Z "OPQRSTUVWXYZ"
#define P_to_Z "PQRSTUVWXYZ"
#define Q_to_Z "QRSTUVWXYZ"
#define R_to_Z "RSTUVWXYZ"
#define S_to_Z "STUVWXYZ"
#define T_to_Z "TUVWXYZ"
#define U_to_Z "UVWXYZ"
#define V_to_Z "VWXYZ"
#define W_to_Z "WXYZ"
#define X_to_Z "XYZ"
#define Y_to_Z "YZ"

    TEST (A_to_Z, + 0, '^', ""     "^" A_to_Z);
    TEST (A_to_Z, + 1, '^', "A"    "^" B_to_Z);
    TEST (A_to_Z, + 2, '^', A_to_B "^" C_to_Z);
    TEST (A_to_Z, + 3, '^', A_to_C "^" D_to_Z);
    TEST (A_to_Z, + 4, '^', A_to_D "^" E_to_Z);
    TEST (A_to_Z, + 5, '^', A_to_E "^" F_to_Z);
    TEST (A_to_Z, + 6, '^', A_to_F "^" G_to_Z);
    TEST (A_to_Z, + 7, '^', A_to_G "^" H_to_Z);
    TEST (A_to_Z, + 8, '^', A_to_H "^" I_to_Z);
    TEST (A_to_Z, + 9, '^', A_to_I "^" J_to_Z);
    TEST (A_to_Z, +10, '^', A_to_J "^" K_to_Z);
    TEST (A_to_Z, +11, '^', A_to_K "^" L_to_Z);
    TEST (A_to_Z, +12, '^', A_to_L "^" M_to_Z);
    TEST (A_to_Z, +13, '^', A_to_M "^" N_to_Z);
    TEST (A_to_Z, +14, '^', A_to_N "^" O_to_Z);
    TEST (A_to_Z, +15, '^', A_to_O "^" P_to_Z);
    TEST (A_to_Z, +16, '^', A_to_P "^" Q_to_Z);
    TEST (A_to_Z, +17, '^', A_to_Q "^" R_to_Z);
    TEST (A_to_Z, +18, '^', A_to_R "^" S_to_Z);
    TEST (A_to_Z, +19, '^', A_to_S "^" T_to_Z);
    TEST (A_to_Z, +20, '^', A_to_T "^" U_to_Z);
    TEST (A_to_Z, +21, '^', A_to_U "^" V_to_Z);
    TEST (A_to_Z, +22, '^', A_to_V "^" W_to_Z);
    TEST (A_to_Z, +23, '^', A_to_W "^" X_to_Z);
    TEST (A_to_Z, +24, '^', A_to_X "^" Y_to_Z);
    TEST (A_to_Z, +25, '^', A_to_Y "^" "Z");
    TEST (A_to_Z, +26, '^', A_to_Z "^" "");

    //////////////////////////////////////////////////////////////////
    // exercise deque::insert(iterator, size_type, const_reference)

    rw_info (0, 0, 0, 
             "std::deque<UserClass>::insert(iterator, size_type, "
             "const_reference)");

#undef TEST
#define TEST(seq, off, n, ins, res) do {                \
      const char insseq [] = { ins, '\0' };             \
      test_insert (__LINE__, -1,                        \
                              (UserClass*)0, n,         \
                              seq, sizeof seq - 1,      \
                              std::size_t (off),        \
                              insseq, 1,                \
                              res, sizeof res - 1);     \
    } while (0)
    
    TEST ("",     +0, 0, 'a', "");
    TEST ("",     +0, 1, 'a', "a");
    TEST ("",     +0, 2, 'b', "bb");
    TEST ("",     +0, 3, 'c', "ccc");

    TEST ("a",    +0, 0, 'a', "a");
    TEST ("b",    +0, 1, 'a', "ab");
    TEST ("b",    +0, 2, 'a', "aab");
    TEST ("b",    +0, 3, 'a', "aaab");

    TEST ("ab",   +1, 0, 'b', "ab");
    TEST ("ac",   +1, 1, 'b', "abc");
    TEST ("ac",   +1, 2, 'b', "abbc");
    TEST ("ac",   +1, 3, 'b', "abbbc");

    TEST ("abcd", +2, 0, 'c', "abcd");
    TEST ("abde", +2, 1, 'c', "abcde");
    TEST ("abde", +2, 2, 'c', "abccde");
    TEST ("abde", +2, 3, 'c', "abcccde");

    //////////////////////////////////////////////////////////////////
    // exercise deque::insert(iterator, InputIterator, InputIterator)

    rw_info (0, 0, 0, 
             "template <class InputIterator> std::deque<UserClass>::"
             "insert(iterator, InputIterator, InputIterator)");

    if (0 == rw_opt_no_input_iterator)
        test_insert_range (InputIter<UserClass>(0, 0, 0),
                           "InputIter<UserClass>");
    else 
        rw_note (0, 0, __LINE__, 
                 "template <class T> "
                 "std::deque<UserClass>::insert(iterator, T, T) "
                 "[with T = InputIterator] test disabled.");

    if (0 == rw_opt_no_forward_iterator)
        test_insert_range (FwdIter<UserClass>(), "FwdIter<UserClass>");
    else 
        rw_note (0, 0, __LINE__, 
                 "template <class T> "
                 "std::deque<UserClass>::insert(iterator, T, T) "
                 "[with T = ForwardIterator] test disabled.");

    if (0 == rw_opt_no_bidirectional_iterator)
        test_insert_range (BidirIter<UserClass>(), "BidirIter<UserClass>");
    else 
        rw_note (0, 0, __LINE__, 
                 "template <class T> "
                 "std::deque<UserClass>::insert(iterator, T, T) "
                 "[with T = BidirectionalIterator] test disabled.");

    if (0 == rw_opt_no_random_iterator)
        test_insert_range (RandomAccessIter<UserClass>(),
                           "RandomAccessIter<UserClass>");
    else 
        rw_note (0, 0, __LINE__, 
                 "template <class T> "
                 "std::deque<UserClass>::insert(iterator, T, T) "
                 "[with T = RandomAccessIterator] test disabled."); 

    //////////////////////////////////////////////////////////////////
    // exercise deque::insert(iterator, int, int)

    rw_info (0, 0, 0, 
             "template <class IntType> "
             "std::deque<IntType>::"
             "insert(iterator, IntType, IntType)");

    if (0 == rw_opt_no_right_thing) {
        test_insert_int_range ((signed char)0, "signed char");
        test_insert_int_range ((unsigned char)0, "unsigned char");
        test_insert_int_range (short (), "short");
        test_insert_int_range ((unsigned short)0, "unsigned short");
        test_insert_int_range (int (), "int");
        test_insert_int_range ((unsigned int)0, "unsigned int");
        test_insert_int_range (long (), "long");
        test_insert_int_range ((unsigned long)0, "unsigned long");

#ifdef _RWSTD_LONG_LONG

        test_insert_int_range ((_RWSTD_LONG_LONG)0, 
                               "long long");
        test_insert_int_range ((unsigned _RWSTD_LONG_LONG)0, 
                               "unsigned long long");

#endif   // _RWSTD_LONG_LONG
    }
    else
        rw_note (0, 0, __LINE__, 
                 "template <class T> "
                 "std::deque<UserClass>::insert(iterator, T, T) "
                 "[with T = IntegralType] tests disabled."); 

}

/**************************************************************************/

template <class Iterator>
void test_assign (int line, int exceptions,
                  const Iterator &dummy, int nelems,
                  const char *seq, std::size_t seqlen,
                  const char *asn, std::size_t asnlen,
                  const char *res, std::size_t reslen)
{
    const UserClass* const xseq = UserClass::from_char (seq, seqlen + 1);
          UserClass* const xasn = UserClass::from_char (asn, asnlen + 1);

    Deque deq = seqlen ? Deque (xseq, xseq + seqlen) : Deque ();

    // format a string describing the function call being exercised
    // (used in diagnostic output below)
    char* funcall = 0;
    std::size_t len = 0;

    rw_asnprintf (&funcall, &len, 
                  "deque(\"%{X=*.*}\").assign("
                  "%{?}\"%{X=*.*}\")%{:}%d, %d)%{;}", 
                  seqlen, -1, xseq, 
                  nelems < 0, 
                  asnlen, -1, xasn, 
                  nelems, *asn);

    int n_copy = UserClass::n_total_copy_ctor_;
    int n_asgn = UserClass::n_total_op_assign_;

    // create a dummy deque iterator to pass to exception_loop
    // (the object will not be used by the functiuon)
    const Deque::iterator dummy_it = deq.begin ();

    if (nelems < 0) {   // assign(Iterator, Iterator)

        if (asnlen > 1)
            exceptions = 0;

        const Iterator first =
            make_iter (xasn, xasn, xasn + asnlen, dummy);

        const Iterator last =
            make_iter (xasn + asnlen, xasn, xasn + asnlen, dummy);

        exception_loop (line, AssignRange, funcall, exceptions,
                        deq, dummy_it, nelems, 0, first, last,
                        &n_copy, &n_asgn);
    }
    else {   // assign(size_type, const_reference)
        if (nelems > 1)
            exceptions = 0;

        exception_loop (line, Assign_n, funcall, exceptions,
                        deq, dummy_it, nelems, xasn, dummy, dummy,
                        &n_copy, &n_asgn);
    }

    // verify the expected size of the deque after assignment
    rw_assert (deq.size () == reslen, 0, line,
               "line %d: %s: size == %zu, got %zu\n",
               __LINE__, funcall, reslen, deq.size ());

    // verify the expected contents of the deque after assignment
    const Deque::const_iterator resbeg = deq.begin ();
    const Deque::const_iterator resend = deq.end ();

    for (Deque::const_iterator it = resbeg; it != resend; ++it) {

        const Deque::size_type inx = it - resbeg;

        _RWSTD_ASSERT (inx < deq.size ());

        if ((*it).data_.val_ != UChar (res [inx])) {

            char* const got = new char [deq.size () + 1];

            for (Deque::const_iterator i = resbeg; i != resend; ++i) {

                const Deque::size_type inx_2 = i - resbeg;

                _RWSTD_ASSERT (inx_2 < deq.size ());

                got [inx_2] = char ((*i).data_.val_);
            }

            got [deq.size ()] = '\0';

            rw_assert (false, 0, line,
                       "line %d: %s: expected %s, got %s\n",
                       __LINE__, funcall, res, got);

            delete[] got;
            break;
        }
    }

    // set asnlen to the number of elements assigned to the container
    if (0 <= nelems)
        asnlen = std::size_t (nelems);

    // verify the complexity of the operation in terms of the number
    // of calls to the copy ctor and assignment operator on value_type

    // the number of invocations of the copy ctor and the assignment
    // operator depends on whether the implementation of assign()
    // strictly follows the requirements in 23.2.1.1, p7 or p8 and
    // destroys the existing elements before inserting the new ones,
    // or whether it assigns the new elements over the existing ones

#ifndef _RWSTD_NO_EXT_DEQUE_ASSIGN_IN_PLACE
    const std::size_t expect_copy = seqlen < asnlen ? asnlen - seqlen : 0;
    const std::size_t expect_asgn = asnlen < seqlen ? asnlen : seqlen;
#else   // if defined (_RWSTD_NO_EXT_DEQUE_ASSIGN_IN_PLACE)
    const std::size_t expect_copy = asnlen;
    const std::size_t expect_asgn = 0;
#endif   // _RWSTD_NO_EXT_DEQUE_ASSIGN_IN_PLACE

    rw_assert (n_copy == int (expect_copy), 0, line,
               "line %d: %s: expected %zu invocations "
               "of UserClass::UserClass(const UserClass&), got %d\n",
               __LINE__, funcall, expect_copy, n_copy);

    rw_assert (n_asgn == int (expect_asgn), 0, line,
               "line %d: %s: expected %zu invocations "
               "of UserClass::operator=(const UserClass&), got %d\n",
               __LINE__, funcall, expect_asgn, n_asgn);

    // Free funcall storage
    std::free (funcall);

    delete[] xasn;
    delete[] xseq;
}


template <class Iterator>
void test_assign_range (const Iterator &it, const char* itname)
{
    rw_info (0, 0, 0, "std::deque<UserClass>::assign(%s, %s)", itname, itname);

    static const char seq[] = "abcdefghijklmnopqrstuvwxyz";
    static const char asn[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (std::size_t i = 0; i != sizeof seq - 1; ++i) {
        for (std::size_t j = 0; j != sizeof asn - 1; ++j) {

            test_assign (__LINE__, 0, it, -1, seq, i, asn, j, asn, j);
        }
    }
}


void test_assign ()
{
    //////////////////////////////////////////////////////////////////
    // exercise
    //   deque::assign(size_type, const_reference)

    rw_info (0, 0, 0,
             "std::deque<UserClass>::assign(size_type, const_reference)");

    static const char seq[] = "abcdefghijklmnopqrstuvwxyz";
    static const char res[] = "AAAAAAAAAAAAAAAAAAAAAAAAAA";

    for (std::size_t i = 0; i != sizeof seq - 1; ++i) {
        for (std::size_t j = 0; j != sizeof seq - 1; ++j) {

            test_assign (__LINE__, -1, (UserClass*)0, int (j),
                         seq, i, res, 1U, res, j);
        }
    }

    //////////////////////////////////////////////////////////////////
    // exercise
    //   template <class InputIterator>
    //   deque::assign(InputIterator, InputIterator)

    rw_info (0, 0, 0, 
             "template <class InputIterator> "
             "std::deque<UserClass>::assign(InputIterator, InputIterator)");

    if (0 == rw_opt_no_input_iterator)
        test_assign_range (InputIter<UserClass>(0, 0, 0),
                           "InputIter<UserClass>");
    else 
        rw_note (0, 0, __LINE__, 
                 "template <class T> "
                 "std::deque<UserClass>::assign(T, T) [with T = InputIterator]"
                 "test disabled.");

    if (0 == rw_opt_no_forward_iterator)
        test_assign_range (FwdIter<UserClass>(), "FwdIter<UserClass>");
    else 
        rw_note (0, 0, __LINE__, 
                 "template <class T> "
                 "std::deque<UserClass>::assign(T, T) "
                 "[with T = ForwardIterator] test disabled.");

    if (0 == rw_opt_no_bidirectional_iterator)
        test_assign_range (BidirIter<UserClass>(), "BidirIter<UserClass>");
    else 
        rw_note (0, 0, __LINE__, 
                 "template <class T> "
                 "std::deque<UserClass>::assign(T, T) "
                 "[with T = BidirectionalIterator] test disabled.");

    if (0 == rw_opt_no_random_iterator)
        test_assign_range (RandomAccessIter<UserClass>(),
                           "RandomAccessIter<UserClass>");
    else 
        rw_note (0, 0, __LINE__, 
                 "template <class T> "
                 "std::deque<UserClass>::assign(T, T) "
                 "[with T = RandomAccessIterator] test disabled.");
}

/**************************************************************************/

void test_erase (int line,
                 const char *seq, std::size_t seqlen,
                 std::size_t begoff, std::size_t len,
                 const char *res, std::size_t reslen)
{
    const UserClass* const xseq = UserClass::from_char (seq, seqlen + 1);

    Deque deq = seqlen ? Deque (xseq, xseq + seqlen) : Deque ();
    const Deque::iterator start = deq.begin () + begoff;
 
    int n_copy = UserClass::n_total_copy_ctor_;
    int n_asgn = UserClass::n_total_op_assign_;

    char* funcall = 0;
    std::size_t buflen = 0;

    if (std::size_t (-1) == len) {   // erase(iterator)

        rw_asnprintf (&funcall, &buflen,
                      "deque(\"%{X=*.*}\").erase(%{?}end()%{:}"
                      "%{?}begin () + %zu%{:}begin ()%{;}%{;}",
                      seqlen, -1, xseq,
                      begoff == deq.size (), begoff, begoff);
        
        exception_loop (line, Erase_1, funcall, 0,
                        deq, start, 1, 0, (UserClass*)0, (UserClass*)0,
                        &n_copy, &n_asgn);
    }
    else {   // assign(size_type, const_reference)

        const Deque::iterator end = start + len;

        rw_asnprintf (&funcall, &buflen,
                      "deque(\"%{X=*.*}\").erase(%{?}end()%{:}"
                      "%{?}begin () + %zu%{:}begin ()%{;}%{;}"
                      "%{?})%{:}%{?}, end ())%{:}%{?}, begin ())"
                      "%{:}begin () + %zu%{;}%{;}%{;}",
                      seqlen, -1, xseq,
                      begoff == deq.size (), begoff, begoff,
                      std::size_t (-1) == len,
                      end == deq.end (),
                      end == deq.begin (),
                      end - deq.begin ());
        
        exception_loop (line, EraseRange, funcall, 0,
                        deq, start, len, 0, (UserClass*)0, (UserClass*)0,
                        &n_copy, &n_asgn);

    }

    // verify the expected size of the deque after erasure
    rw_assert (deq.size () == reslen, 0, line,
               "line %d: %s: size == %zu, got %zu\n",
               __LINE__, funcall, reslen, deq.size ());

    // verify the expected contents of the deque after assignment
    const Deque::const_iterator resbeg = deq.begin ();
    const Deque::const_iterator resend = deq.end ();

    for (Deque::const_iterator it = resbeg; it != resend; ++it) {
        if ((*it).data_.val_ != UChar (res [it - resbeg])) {

            char* const got = new char [deq.size () + 1];

            for (Deque::const_iterator i = resbeg; i != resend; ++i) {
                got [i - resbeg] = char ((*i).data_.val_);
            }

            got [deq.size ()] = '\0';

            rw_assert (false, 0, line,
                       "line %d: %s: expected %s, got %s\n",
                       __LINE__, funcall, res, got);

            delete[] got;
            break;
        }
    }

#if 0
    // set asnlen to the number of elements assigned to the container
    if (0 <= nelems)
        asnlen = std::size_t (nelems);

    // verify the complexity of the operation in terms of the number
    // of calls to the copy ctor and assignment operator on value_type

    // the number of invocations of the copy ctor and the assignment
    // operator depends on whether the implementation of assign()
    // strictly follows the requirements in 23.2.1.1, p7 or p8 and
    // destroys the existing elements before inserting the new ones,
    // or whether it assigns the new elements over the existing ones

#ifndef _RWSTD_NO_EXT_DEQUE_ASSIGN_IN_PLACE
    const std::size_t expect_copy = seqlen < asnlen ? asnlen - seqlen : 0;
    const std::size_t expect_asgn = asnlen < seqlen ? asnlen : seqlen;
#else   // if defined (_RWSTD_NO_EXT_DEQUE_ASSIGN_IN_PLACE)
    const std::size_t expect_copy = asnlen;
    const std::size_t expect_asgn = 0;
#endif   // _RWSTD_NO_EXT_DEQUE_ASSIGN_IN_PLACE

    rw_assert (n_copy == int (expect_copy), 0, line,
               "line %d: %s: expected %zu invocations "
               "of UserClass::UserClass(const UserClass&), got %d\n",
               __LINE__, funcall, expect_copy, n_copy);

    rw_assert (n_asgn == int (expect_asgn), 0, line,
               "line %d: %s: expected %zu invocations "
               "of UserClass::operator=(const UserClass&), got %d\n",
               __LINE__, funcall, expect_asgn, n_asgn);
#endif

    std::free (funcall);

    delete[] xseq;
}

void test_erase ()
{
    //////////////////////////////////////////////////////////////////
    // exercise deque::erase(iterator)

    rw_info (0, 0, 0, "std::deque<UserClass>::erase(iterator)");

#undef TEST
#define TEST(seq, off, res) do {                        \
      test_erase (__LINE__,                             \
                             seq, sizeof seq - 1,       \
                             std::size_t (off),         \
                             std::size_t (-1),          \
                             res, sizeof res - 1);      \
    } while (0)

    TEST ("a", 0, "");

    TEST ("ab", 0, "b");
    TEST ("ab", 1, "a");

    TEST ("abc", 0, "bc");
    TEST ("abc", 1, "ac");
    TEST ("abc", 2, "ab");

    TEST ("abcd", 0, "bcd");
    TEST ("abcd", 1, "acd");
    TEST ("abcd", 2, "abd");
    TEST ("abcd", 3, "abc");

    TEST ("abcde", 0, "bcde");
    TEST ("abcde", 1, "acde");
    TEST ("abcde", 2, "abde");
    TEST ("abcde", 3, "abce");
    TEST ("abcde", 4, "abcd");

    TEST ("abcdef", 0, "bcdef");
    TEST ("abcdef", 1, "acdef");
    TEST ("abcdef", 2, "abdef");
    TEST ("abcdef", 3, "abcef");
    TEST ("abcdef", 4, "abcdf");
    TEST ("abcdef", 5, "abcde");

    TEST ("abcdefg", 0, "bcdefg");
    TEST ("abcdefg", 1, "acdefg");
    TEST ("abcdefg", 2, "abdefg");
    TEST ("abcdefg", 3, "abcefg");
    TEST ("abcdefg", 4, "abcdfg");
    TEST ("abcdefg", 5, "abcdeg");
    TEST ("abcdefg", 6, "abcdef");

    TEST ("abcdefgh", 0, "bcdefgh");
    TEST ("abcdefgh", 1, "acdefgh");
    TEST ("abcdefgh", 2, "abdefgh");
    TEST ("abcdefgh", 3, "abcefgh");
    TEST ("abcdefgh", 4, "abcdfgh");
    TEST ("abcdefgh", 5, "abcdegh");
    TEST ("abcdefgh", 6, "abcdefh");
    TEST ("abcdefgh", 7, "abcdefg");

    //////////////////////////////////////////////////////////////////
    // exercise deque::erase(iterator, iterator)

    rw_info (0, 0, 0, "std::deque<UserClass>::erase(iterator, iterator)");
}

/**************************************************************************/

#ifndef _RWSTD_NO_EXPLICIT
#  if !defined (_MSC_VER) || _MSC_VER > 1200

struct DR_438
{
    static bool cast_used;

    DR_438 () { }

    explicit DR_438 (std::size_t) { cast_used = true; }

    template <class T> DR_438 (T) { }
};

bool DR_438::cast_used;

#  else   // if MSVC <= 6.0
       // avoid an MSVC 6.0 ICE on this code
#    define NO_DR_438_TEST "this version of MSVC is too broken"
#  endif   // !MSVC || MSVC > 6.0
#else
#  define NO_DR_438_TEST "_RWSTD_NO_EXPLICIT #defined"
#endif   // _RWSTD_NO_EXPLICIT


void test_dr_438 ()
{
    //////////////////////////////////////////////////////////////////
    // exercise the resolution of DR 438:
    //////////////////////////////////////////////////////////////////
    //
    // For every sequence defined in clause [lib.containers]
    // and in clause [lib.strings]:

    // *  If the constructor
    //
    //    template <class InputIterator>
    //    UserClass (InputIterator f, InputIterator l,
    //       const allocator_type& a = allocator_type())
    //
    //    is called with a type InputIterator that does not qualify
    //    as an input iterator, then the constructor will behave
    //    as if the overloaded constructor:
    //
    //    UserClass (size_type, const value_type& = value_type(),
    //       const allocator_type& = allocator_type())
    //
    //    were called instead, with the arguments static_cast<size_type>(f),
    //    l and a, respectively.
    //
    // *  If the member functions of the forms:
    //
    //    template <class InputIterator>       // such as insert()
    //    rt fx1(iterator p, InputIterator f, InputIterator l);
    //
    //    template <class InputIterator>       // such as append(), assign()
    //    rt fx2(InputIterator f, InputIterator l);
    //
    //    template <class InputIterator>        // such as replace()
    //    rt fx3(iterator i1, iterator i2, InputIterator f, InputIterator l);
    //
    //    are called with a type InputIterator that does not qualify
    //    as an input iterator, then these functions will behave
    //    as if the overloaded member functions:
    //
    //    rt fx1(iterator, size_type, const value_type&);
    //
    //    rt fx2(size_type, const value_type&);
    //
    //    rt fx3(iterator, iterator, size_type, const value_type&);
    //
    //    were called instead, with the same arguments.
    //
    // In the previous paragraph the alternative binding will fail
    // if f is not implicitly convertible to UserClass::size_type or
    // if l is not implicitly convertible to UserClass::value_type.
    //
    // The extent to which an implementation determines that a type
    // cannot be an input iterator is unspecified, except that
    // as a minimum integral types shall not qualify as input iterators.
    //////////////////////////////////////////////////////////////////

    rw_info (0, 0, 0, "resolution of DR 438");

#ifndef NO_DR_438_TEST

    std::deque<DR_438, std::allocator<DR_438> > dq;

#if DRQS // deque::assign() does not follow the "do the right thing" clause
    dq.assign (1, 2);
#else
    dq.assign ((std::size_t)1, 2);
#endif

    rw_assert (!DR_438::cast_used, 0, __LINE__,
               "deque::assign(InputIterator, InputIterator)"
               "[ with InputIterator = <integral type> ] unexpectedly "
               "used explicit argument conversion");
    
    dq.insert (dq.begin (), 1, 2);

    rw_assert (!DR_438::cast_used, 0, __LINE__,
               "deque::insert(iterator, InputIterator, InputIterator) "
               "[ with InputIterator = <integral type> ] unexpectedly "
               "used explicit argument conversion");
#else   // if defined (NO_DR_438_TEST)

    rw_warning (0, 0, __LINE__, "%s; skipping test", NO_DR_438_TEST);

#endif   // NO_DR_438_TEST

}

/**************************************************************************/

int run_test (int, char**)
{
    if (0 == rw_opt_no_dr438)
        test_dr_438 ();

    static const std::size_t caps[] = {
        2, 3, 4, 5, 16, 32
    };

    for (std::size_t i = 0; i != sizeof caps / sizeof *caps; ++i) {

#if TEST_RW_EXTENSIONS
        new_capacity = caps [i];

        rw_info (0, 0, 0, 
                 "__rw::__rw_new_capacity<std::deque<UserClass> >(0) = %zu",
                 _RW::__rw_new_capacity (0, (Deque*)0));
#endif // TEST_RW_EXTENSIONS

        if (0 == rw_opt_no_assign)
            test_assign ();
        
        if (0 == rw_opt_no_erase)
            test_erase ();
        
        if (0 == rw_opt_no_insert)
            test_insert ();
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char** argv)
{
    return rw_test (argc, argv, __FILE__,
                    "lib.deque.modifiers",
                    0 /* no comment */, run_test,
                    "|-no-dr438#"
                    "|-no-assign#"
                    "|-no-erase#"
                    "|-no-insert#"
                    "|-no-InputIterator#"
                    "|-no-ForwardIterator#"
                    "|-no-BidirectionalIterator#"
                    "|-no-RandomIterator#"
                    "|-no-right-thing#",
                    &rw_opt_no_dr438,
                    &rw_opt_no_assign,
                    &rw_opt_no_erase,
                    &rw_opt_no_insert,
                    &rw_opt_no_input_iterator,
                    &rw_opt_no_forward_iterator,
                    &rw_opt_no_bidirectional_iterator,
                    &rw_opt_no_random_iterator,
                    &rw_opt_no_right_thing);
}
