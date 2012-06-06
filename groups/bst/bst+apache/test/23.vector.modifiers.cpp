/***************************************************************************
 *
 * 23.vector.modifiers.cpp - test exercising [lib.vector.modifiers]
 *
 * $Id: 23.vector.modifiers.cpp 550991 2007-06-26 23:58:07Z sebor $
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

#include <rw_printf.h>
#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE
   // disabled for compilers such as IBM VAC++ or MSVC
   // that can't reliably replace the operators
#  include <rw_new.h>
#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

/**************************************************************************/

static unsigned rw_opt_nloops;
static int      rw_opt_no_input_iter;
static int      rw_opt_no_forward_iter;
static int      rw_opt_no_bidir_iter;
static int      rw_opt_no_random_iter;
static int      rw_opt_no_insert;
static int      rw_opt_no_insert_range;
static int      rw_opt_no_insert_at_end;
static int      rw_opt_no_complexity;
static int      rw_opt_no_push_back;

/**************************************************************************/

typedef std::vector<UserClass, std::allocator<UserClass> > Vector;

Vector::size_type new_capacity;

#if TEST_RW_EXTENSIONS
namespace __rw {

_RWSTD_SPECIALIZED_FUNCTION
inline Vector::size_type
__rw_new_capacity<Vector>(Vector::size_type n, const Vector*)
{
    if (n)
        return n * 2;

    return new_capacity;
}

}
#endif

/**************************************************************************/

// used to determine whether insert() can or cannot use
// an algorithm optimized for ForwardIterators
bool is_forward_category (std::input_iterator_tag)   { return false; }
bool is_forward_category (std::forward_iterator_tag) { return true;  }

template <class Iterator>
bool is_forward (const Iterator &it)
{
    _RWSTD_UNUSED (it);

    return is_forward_category (_RWSTD_ITERATOR_CATEGORY (Iterator, it));

}

// exercises the exception safety of each tested function by successively
// forcing operator new to thow a bad_alloc exception at each iteration
// of the loop until the call succeeds (i.e., until no exception is
// thrown)
template <class Iterator>
void exception_loop (int line, const char *fcall, bool capchg,
                     Vector &vec, const Vector::iterator &it,
                     int n, const UserClass *x,
                     const Iterator &first, const Iterator &last,
                     std::size_t *n_copy, std::size_t *n_asgn)
{
    std::size_t throw_after = 0;

    const std::size_t            size     = vec.size ();
    const std::size_t            capacity = vec.capacity ();
    const Vector::const_iterator begin    = vec.begin ();

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    rwt_free_store* const pst = rwt_get_free_store (0);

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    // iterate for`n=throw_after' starting at the next call to operator
    // new, forcing each call to throw an exception, until the insertion
    // finally succeeds (i.e, no exception is thrown)
    for ( ; ; ) {

        *n_copy = UserClass::n_total_copy_ctor_;
        *n_asgn = UserClass::n_total_op_assign_;

#ifndef _RWSTD_NO_EXCEPTIONS

        // detect objects constructed but not destroyed after an exception
        const std::size_t count = UserClass::count_;

#  ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

        // disable out of memory exceptions before copying iterators
        // (InputIter ctors may dynamically allocate memory)
        *pst->throw_at_calls_ [0] = std::size_t (-1);
#  endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE
#endif   // _RWSTD_NO_EXCEPTIONS

        // create "deep" copies of the iterators to thwart
        // InpuIter's multi-pass detection
        const Iterator beg = copy_iter (first, (UserClass*)0);
        const Iterator end = copy_iter (last, (UserClass*)0);

#ifndef _RWSTD_NO_EXCEPTIONS
#  ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

        *pst->throw_at_calls_ [0] = pst->new_calls_ [0] + throw_after + 1;

#  endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE
#endif   // _RWSTD_NO_EXCEPTIONS

        _TRY {
            if (-2 == n) {
                vec.insert (it, *x);
            }
            else if (-1 == n) {

#ifndef _RWSTD_NO_EXT_VECTOR_INSERT_IN_PLACE
#  ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE
                // disable exceptions if Iterator is not at least
                // a ForwardIterator since the extension doesn't
                // provide the strong exception safety guarantee
                // required in 23.2.4.3, p1
                if (!is_forward (beg)) {
                    *pst->throw_at_calls_ [0] = std::size_t (-1);
                }
#  endif   //_RWSTD_NO_REPLACEABLE_NEW_DELETE
#endif   // _RWSTD_NO_EXT_VECTOR_INSERT_IN_PLACE

                vec.insert (it, beg, end);
            }
            else {
                vec.insert (it, n, *x);
            }
            break;
        }
        _CATCH (...) {

#ifndef _RWSTD_NO_EXCEPTIONS

            // verify that an exception thrown during allocation
            // doesn't cause a change in the state of the vector

            rw_assert (vec.size () == size, 0, line,
                       "line %d: %s: size unexpectedly changed "
                       "from %zu to %zu after an exception",
                       __LINE__, fcall, size, vec.size ());

            rw_assert (vec.capacity () == capacity, 0, line,
                       "line %d: %s: capacity unexpectedly "
                       "changed from %zu to %zu after an exception",
                       __LINE__, fcall, capacity, vec.capacity ());


            rw_assert (vec.begin () == begin, 0, line,
                       "line %d: %s: begin() unexpectedly "
                       "changed from after an exception by %d",
                       __LINE__, fcall, vec.begin () - begin);


            rw_assert (count == UserClass::count_, 0, line,
                       "line %d: %s: leaked %d objects "
                       "after an exception",
                       __LINE__, fcall, UserClass::count_ - count);

            // increment to allow this call to operator new to succeed
            // and force the next one to fail, and try to insert again
            ++throw_after;

#endif   // _RWSTD_NO_EXCEPTIONS

        }   // catch
    }   // for

#ifndef _RWSTD_NO_EXCEPTIONS
#  ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    // verify that if exceptions are enabled and when capacity changes
    // at least one exception is thrown
    rw_assert (   *pst->throw_at_calls_ [0] == std::size_t (-1)
               || !capchg
               || throw_after,
               0, line,
               "line %d: %s: failed to throw an expected exception",
               __LINE__, fcall);

    // commented out: just because capacity doesn't change doesn't mean
    // that no exception is thrown (e.g., if the function dynamically
    // allocates temporary storage)
    //
    //    rw_assert (capchg || !throw_after,
    //               0, line,
    //               "line %d: %s: unexpectedly threw an exception",
    //               __LINE__, fcall);

#  endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE
#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    _RWSTD_UNUSED (t);
    _RWSTD_UNUSED (line);
    _RWSTD_UNUSED (fcall);
    _RWSTD_UNUSED (capchg);
    _RWSTD_UNUSED (size);
    _RWSTD_UNUSED (capacity);
    _RWSTD_UNUSED (throw_after);

#endif   // _RWSTD_NO_EXCEPTIONS

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    *pst->throw_at_calls_ [0] = std::size_t (-1);

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    // compute the number of calls to UserClass copy ctor
    // and assignment operator
    *n_copy = UserClass::n_total_copy_ctor_ - *n_copy;
    *n_asgn = UserClass::n_total_op_assign_ - *n_asgn;
}

/**************************************************************************/

template <class Iterator>
void test_insert (int             line,     // line number of call site
                  const Iterator &dummy,    // unused dummy iterator
                  int             n,        // number of elements to insert
                                            // or the special values -1 and -2
                  const char     *seq,      // initial container contents
                  std::size_t     seqlen,   // initial container size
                  std::size_t     seqcap,   // initial container capacity
                  int             off,      // offset at which to insert
                  const char     *ins,      // inserted sequence
                  std::size_t     inslen,   // length of inserted sequence
                  const char     *res,      // resulting sequence
                  std::size_t     reslen    /* length of resulting sequence */)
{
    // create arrays of `seqlen' and `inslen' elements of type UserClass
    // initialized from the sequences `seq' and `ins', respectively
    const UserClass* const xseq = UserClass::from_char (seq, seqlen);
          UserClass* const xins = UserClass::from_char (ins, inslen);

    // construct a std::vector from the sequence of elements
    Vector vec = seqlen ? Vector (xseq, xseq + seqlen) : Vector ();

    // set the capacity of the container (if specified)
    if (std::size_t (-1) == seqcap)
        seqcap = vec.capacity ();
    else
        vec.reserve (seqcap);

    // create an iterator pointing `off' past the beginning of the vector
    const Vector::iterator it = vec.begin () + off;

    char* fcall = 0;
    std::size_t len = 0;

    // format a string describing the function call being tested
    // (used in diagnostic messages, if any, below)
    rw_asnprintf (&fcall, &len, "vector(\"%{X=*.*}\").insert("
                  "%{?}begin()%{:}%{?}end()%{:}begin() + %zu%{;}%{;}, "
                  "%{?}%d%{:}%{?}\"%{X=*.*}\"%{:}%d, %d%{;}%{;})",
                  int (seqlen), -1, xseq,
                  0 == off, int (seqlen) == off, off,
                  n == -2, *ins, n == -1,
                  int (inslen), -1, xins, n, *ins);

    // 23.2.4.3, p1
    // Notes: Causes reallocation if the new size is greater than
    // the old capacity. If no reallocation happens, all the iterators
    // and references before the insertion point remain valid.
    // If an exception is thrown other than by the copy constructor
    // an assignment operator of T there are no effects.

    // determine whether the insertion must or must not change capacity
    const bool capchg = seqlen + (n < 0 ? inslen : n) > seqcap;

    std::size_t n_copy = UserClass::n_total_copy_ctor_;
    std::size_t n_asgn = UserClass::n_total_op_assign_;

    if (-2 == n) {

        exception_loop (line, fcall, capchg,
                        vec, it, n, xins, dummy, dummy,
                        &n_copy, &n_asgn);
    }
    else if (-1 == n) {

        const Iterator first =
            make_iter (xins, xins, xins + inslen, dummy);

        const Iterator last =
            make_iter (xins + inslen, xins, xins + inslen, dummy);

        exception_loop (line, fcall, capchg,
                        vec, it, n, 0, first, last,
                        &n_copy, &n_asgn);
    }
    else /* if (n != -2 && n != -1) */ {
        // n must be in the range [-2, INT_MAX]
        _RWSTD_ASSERT (n >= 0);

        exception_loop (line, fcall, capchg,
                        vec, it, n, xins, dummy, dummy,
                        &n_copy, &n_asgn);
    }

    // verify the expected size of the vector after insertion
    rw_assert (vec.size () == reslen, 0, line,
               "line %d: %s: size == %zu, got %zu",
               __LINE__, fcall, reslen, vec.size ());

    // verify the expected change in capacity of the vactor
    rw_assert (capchg == (vec.capacity () != seqcap),
               0, line, "line %d: %s: capacity %c = %zu, got %zu",
               __LINE__, fcall, capchg ? '!' : '=', seqcap, vec.capacity ());

    // verify the expected contents of the vector after insertion
    const Vector::const_pointer resbeg = vec.size () ? &*vec.begin () : 0;
    const Vector::const_pointer resend = resbeg + vec.size ();

    for (Vector::const_pointer ptr = resbeg; ptr != resend; ++ptr) {

        typedef unsigned char UChar;

        if (ptr->data_.val_ != UChar (res [ptr - resbeg])) {

            char* const got = new char [vec.size () + 1];

            for (Vector::const_pointer p = resbeg; p != resend; ++p) {
                got [p - resbeg] = char (p->data_.val_);
            }

            got [vec.size ()] = '\0';

            rw_assert (0, 0, line,
                       "line %d: %s: expected %{#s}, got %{#s}",
                       __LINE__, fcall, res, got);

            delete [] got;
            break;
        }
    }

    // 23.2.4.3, p2
    // Complexity:
    // --  If first and last are forward iterators, bidirectional iterators,
    //     or random access iterators, the complexity is linear in the number
    //     of elements in the range [first, last) plus the distance to
    //     the end of the vector.
    // --  If they are input iterators, the complexity is proportional
    //     to the  number of elements in the range [first, last) times
    //     the distance to the end of the vector.

    // verify the complexity of the operation in terms of the number
    // of calls to the copy ctor and assignment operator on value_type
    // note that the complexity requirement imposes an upper bound on
    // the number of operations and implementations are allowed to do
    // better (see 17.3.1.3, p5)

    std::size_t expect_copy;   // expected number of copy ctor calls
    std::size_t expect_asgn;   // expected number of assignments

    if (is_forward (dummy)) {
        // ForwardIterator or better

        if (capchg) {
            // when the insertion causes a change in capacity
            // (i.e., reallocation), the number of copy ctor
            // invocations is equal to the size of the resulting
            // sequence; no assignment operators are invoked
            expect_copy = reslen;
            expect_asgn = 0;
        }
        else {
            // when no change in capacity occurs the number of copy
            // ctor invocations is equal to the number of elements
            // between the end of the inserted range and the end of
            // the original sequence
            // the number of assignments is equal to the distance
            // from the point of insertion to the end of the original
            // sequence
#if TEST_RW_EXTENSIONS
    // This check assumed no aliasing.
            expect_copy = n < 0 ? inslen : n;
#else
            expect_copy = n < 0 ? inslen + 1 : n;  // temp copy made for
                                                   // aliasing case
#endif
            expect_asgn = n && inslen ? reslen - off : 0;
        }
    }
    else {
        // InputIterator
        expect_copy = n_copy;
        expect_asgn = n_asgn;

        // don't bother testing the complexity of the functions with
        // InputIterators, the standard's messed up (see issue 247
        // and the indirectly related issue 406)
    }

    rw_assert (expect_copy >= n_copy,
               0, line,
               "line %d: %s: expected at most %zu "
               "invocation(s) of UserClass::UserClass(const UserClass&), "
               "got %zu", __LINE__, fcall, expect_copy, n_copy);

    rw_assert (expect_asgn >= n_asgn,
               0, line,
               "line %d: %s: expected at most %zu invocation(s) "
               "of UserClass::operator=(const UserClass&), got %zu",
               __LINE__, fcall, expect_asgn, n_asgn);

    delete [] xins;
    delete [] _RWSTD_CONST_CAST (UserClass*, xseq);
}

/**************************************************************************/

void test_insert ()
{
    //////////////////////////////////////////////////////////////////
    // exercise vector::insert(iterator, const_reference)

    rw_info (0, 0, 0,
             "std::vector<UserClass>::insert(iterator, const_reference)");

#undef  TEST
#define TEST(seq, seqcap, off, ins, res) do {   \
      const char insseq [] = { ins, '\0' };     \
      test_insert (__LINE__, (UserClass*)0, -2, \
                   seq, sizeof seq - 1,         \
                   std::size_t (seqcap), off,   \
                   insseq, 1,                   \
                   res, sizeof res - 1);        \
    } while (0)

    //    +----------------------- initial container contents
    //    |       +--------------- initial container capacity
    //    |       |   +----------- offset at which to insert
    //    |       |   |    +------ element to insert
    //    |       |   |    |   +-- resulting sequence
    //    |       |   |    |   |
    //    V       V   V    V   V
    TEST ("",     -1, +0, 'a', "a");
    TEST ("b",    -1, +0, 'a', "ab");
    TEST ("bc",   -1, +0, 'a', "abc");
    TEST ("bcd",  -1, +0, 'a', "abcd");
    TEST ("bcde", -1, +0, 'a', "abcde");

    TEST ("a",    -1, +1, 'b', "ab");
    TEST ("ac",   -1, +1, 'b', "abc");
    TEST ("acd",  -1, +1, 'b', "abcd");
    TEST ("acde", -1, +1, 'b', "abcde");

    TEST ("ab",   -1, +2, 'c', "abc");
    TEST ("abd",  -1, +2, 'c', "abcd");
    TEST ("abde", -1, +2, 'c', "abcde");

    TEST ("abc",  -1, +3, 'd', "abcd");
    TEST ("abce", -1, +3, 'd', "abcde");

    TEST ("abcd", -1, +4, 'e', "abcde");

    //////////////////////////////////////////////////////////////////
    // exercise vector::insert(iterator, size_type, const_reference)

    rw_info (0, 0, 0,
             "std::vector<UserClass>::insert(iterator, "
             "size_type, const_reference)");

#undef TEST
#define TEST(seq, seqcap, off, n, ins, res) do {        \
      const char insseq [] = { ins, '\0' };             \
      test_insert (__LINE__, (UserClass*)0, n,          \
                   seq, sizeof seq - 1,                 \
                   std::size_t (seqcap), off,           \
                   insseq, 1,                           \
                   res, sizeof res - 1);                \
    } while (0)

    //    +-------------------------- initial container contents
    //    |       +------------------ initial container capacity
    //    |       |   +-------------- offset at which to insert
    //    |       |   |   +---------- number of elements to insert
    //    |       |   |   |   +------ element value to insert
    //    |       |   |   |   |   +-- resulting sequence
    //    |       |   |   |   |   |
    //    V       V   V   V   V   V
    TEST ("",     -1, +0, 0, 'a', "");
    TEST ("",     -1, +0, 1, 'a', "a");
    TEST ("",     -1, +0, 2, 'b', "bb");
    TEST ("",     -1, +0, 3, 'c', "ccc");

    TEST ("a",    -1, +0, 0, 'a', "a");
    TEST ("b",    -1, +0, 1, 'a', "ab");
    TEST ("b",    -1, +0, 2, 'a', "aab");
    TEST ("b",    -1, +0, 3, 'a', "aaab");

    TEST ("ab",   -1, +1, 0, 'b', "ab");
    TEST ("ac",   -1, +1, 1, 'b', "abc");
    TEST ("ac",   -1, +1, 2, 'b', "abbc");
    TEST ("ac",   -1, +1, 3, 'b', "abbbc");

    TEST ("abcd", -1, +2, 0, 'c', "abcd");
    TEST ("abde", -1, +2, 1, 'c', "abcde");
    TEST ("abde", -1, +2, 2, 'c', "abccde");
    TEST ("abde", -1, +2, 3, 'c', "abcccde");
}

/**************************************************************************/

template <class Iterator>
void test_insert_range (const Iterator &dummy)
{
    static const char* const itname = type_name (dummy, (UserClass*)0);

    rw_info (0, 0, 0,
             "std::vector<UserClass>::insert(iterator, %s, %s)",
             itname, itname);

#undef  TEST
#define TEST(seq, seqcap, off, ins, res)        \
    test_insert (__LINE__, dummy, -1,           \
                 seq, sizeof seq - 1,           \
                 std::size_t (seqcap), off,     \
                 ins, sizeof ins - 1,           \
                 res, sizeof res - 1)

    //    +-------------------------------------------- seq(uence)
    //    |      +------------------------------------- seqcap(acity)
    //    |      |   +--------------------------------- off(set of insertion)
    //    |      |   |   +----------------------------- ins(erted sequence)
    //    |      |   |   |      +---------------------- res(ulting sequence)
    //    |      |   |   |      |
    //    v      v   v   v      v
    TEST ("",    -1, +0, "",    "");
    TEST ("",    -1, +0, "a",   "a");
    TEST ("",    -1, +0, "ab",  "ab");
    TEST ("",    -1, +0, "abc", "abc");

    TEST ("A",   -1, +0, "",    "A");
    TEST ("B",   -1, +0, "a",   "aB");
    TEST ("C",   -1, +0, "ab",  "abC");
    TEST ("D",   -1, +0, "abc", "abcD");

    TEST ("AB",  -1, +0, "",    "AB");
    TEST ("AB",  -1, +0, "c",   "cAB");
    TEST ("AB",  -1, +0, "cd",  "cdAB");
    TEST ("AB",  -1, +0, "cde", "cdeAB");

    TEST ("A",   -1, +1, "",    "A");
    TEST ("A",   -1, +1, "b",   "Ab");
    TEST ("A",   -1, +1, "bc",  "Abc");
    TEST ("A",   -1, +1, "bcd", "Abcd");

    TEST ("AB",  -1, +1, "",    "AB");
    TEST ("AC",  -1, +1, "b",   "AbC");
    TEST ("AD",  -1, +1, "bc",  "AbcD");
    TEST ("AE",  -1, +1, "bcd", "AbcdE");

    TEST ("AB",  -1, +2, "",    "AB");
    TEST ("AB",  -1, +2, "c",   "ABc");
    TEST ("AB",  -1, +2, "cd",  "ABcd");
    TEST ("AB",  -1, +2, "cde", "ABcde");

    TEST ("ABC", -1, +2, "",    "ABC");
    TEST ("ABD", -1, +2, "c",   "ABcD");
    TEST ("ABE", -1, +2, "cd",  "ABcdE");
    TEST ("ABF", -1, +2, "cde", "ABcdeF");

    TEST ("ABC", -1, +3, "",    "ABC");
    TEST ("ABC", -1, +3, "d",   "ABCd");
    TEST ("ABC", -1, +3, "de",  "ABCde");
    TEST ("ABC", -1, +3, "def", "ABCdef");


#define UPPER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define LOWER "abcdefghijklmnopqrstuvwxyz"

    TEST ("",    -1,  +0, LOWER, LOWER);

    TEST ("A",   -1,  +0, LOWER, LOWER "A");
    TEST ("A",   -1,  +1, LOWER, "A" LOWER);

    TEST ("AB",  -1,  +0, LOWER, LOWER "AB");
    TEST ("AB",  -1,  +1, LOWER, "A" LOWER "B");
    TEST ("AB",  -1,  +2, LOWER, "AB" LOWER);

    TEST ("ABC", -1,  +0, LOWER, LOWER "ABC");
    TEST ("ABC", -1,  +1, LOWER, "A" LOWER "BC");
    TEST ("ABC", -1,  +2, LOWER, "AB" LOWER "C");
    TEST ("ABC", -1,  +3, LOWER, "ABC" LOWER);

    TEST (UPPER, -1,  +0, "!",   "!ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +1, "!",   "A!BCDEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +2, "!",   "AB!CDEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +3, "!",   "ABC!DEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +9, "!",   "ABCDEFGHI!JKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1, +25, "!",   "ABCDEFGHIJKLMNOPQRSTUVWXY!Z");
    TEST (UPPER, -1, +26, "!",   "ABCDEFGHIJKLMNOPQRSTUVWXYZ!");

    TEST (UPPER, -1,  +0, "12",  "12ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +1, "12",  "A12BCDEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +2, "12",  "AB12CDEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +3, "12",  "ABC12DEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +9, "12",  "ABCDEFGHI12JKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1, +25, "12",  "ABCDEFGHIJKLMNOPQRSTUVWXY12Z");
    TEST (UPPER, -1, +26, "12",  "ABCDEFGHIJKLMNOPQRSTUVWXYZ12");

    TEST (UPPER, -1,  +0, "123", "123ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +1, "123", "A123BCDEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +2, "123", "AB123CDEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +3, "123", "ABC123DEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +9, "123", "ABCDEFGHI123JKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1, +25, "123", "ABCDEFGHIJKLMNOPQRSTUVWXY123Z");
    TEST (UPPER, -1, +26, "123", "ABCDEFGHIJKLMNOPQRSTUVWXYZ123");

    TEST (UPPER, -1,  +0, LOWER, "" LOWER "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +1, LOWER, "A" LOWER "BCDEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +2, LOWER, "AB" LOWER "CDEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +3, LOWER, "ABC" LOWER "DEFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +4, LOWER, "ABCD" LOWER "EFGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +5, LOWER, "ABCDE" LOWER "FGHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +6, LOWER, "ABCDEF" LOWER "GHIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +7, LOWER, "ABCDEFG" LOWER "HIJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +8, LOWER, "ABCDEFGH" LOWER "IJKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1,  +9, LOWER, "ABCDEFGHI" LOWER "JKLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1, +10, LOWER, "ABCDEFGHIJ" LOWER "KLMNOPQRSTUVWXYZ");
    TEST (UPPER, -1, +11, LOWER, "ABCDEFGHIJK" LOWER "LMNOPQRSTUVWXYZ");
    TEST (UPPER, -1, +12, LOWER, "ABCDEFGHIJKL" LOWER "MNOPQRSTUVWXYZ");
    TEST (UPPER, -1, +13, LOWER, "ABCDEFGHIJKLM" LOWER "NOPQRSTUVWXYZ");
    TEST (UPPER, -1, +14, LOWER, "ABCDEFGHIJKLMN" LOWER "OPQRSTUVWXYZ");
    TEST (UPPER, -1, +15, LOWER, "ABCDEFGHIJKLMNO" LOWER "PQRSTUVWXYZ");
    TEST (UPPER, -1, +16, LOWER, "ABCDEFGHIJKLMNOP" LOWER "QRSTUVWXYZ");
    TEST (UPPER, -1, +17, LOWER, "ABCDEFGHIJKLMNOPQ" LOWER "RSTUVWXYZ");
    TEST (UPPER, -1, +18, LOWER, "ABCDEFGHIJKLMNOPQR" LOWER "STUVWXYZ");
    TEST (UPPER, -1, +19, LOWER, "ABCDEFGHIJKLMNOPQRS" LOWER "TUVWXYZ");
    TEST (UPPER, -1, +20, LOWER, "ABCDEFGHIJKLMNOPQRST" LOWER "UVWXYZ");
    TEST (UPPER, -1, +21, LOWER, "ABCDEFGHIJKLMNOPQRSTU" LOWER "VWXYZ");
    TEST (UPPER, -1, +22, LOWER, "ABCDEFGHIJKLMNOPQRSTUV" LOWER "WXYZ");
    TEST (UPPER, -1, +23, LOWER, "ABCDEFGHIJKLMNOPQRSTUVW" LOWER "XYZ");
    TEST (UPPER, -1, +24, LOWER, "ABCDEFGHIJKLMNOPQRSTUVWX" LOWER "YZ");
    TEST (UPPER, -1, +25, LOWER, "ABCDEFGHIJKLMNOPQRSTUVWXY" LOWER "Z");
    TEST (UPPER, -1, +26, LOWER, "ABCDEFGHIJKLMNOPQRSTUVWXYZ" LOWER "");
}

/**************************************************************************/

void test_insert_at_end ()
{
    //////////////////////////////////////////////////////////////////
    // exercise vector::insert() at the end of the container and
    // verify that the functions perform in amortized constant time
    // (see also PR #30615)

    rw_info (0, 0, 0,
             "std::vector<UserClass>::insert (end(), const_reference)");

    typedef Vector::size_type size_type;

    Vector v;

    size_type nextcap = 0;

    // iterate to grow vector excactly 8 times
    for (int i = 0; i != 8; ++i) {

        // count the number of copy ctor and assignment operator invocations
        std::size_t n_copy;
        std::size_t n_asgn;

#if TEST_RW_EXTENSIONS
        // compute the next (non-zero) capacity of the container
        nextcap = _RW::__rw_new_capacity (nextcap + 1, (Vector*)0);
#endif // TEST_RW_EXTENSIONS

        // insert elements at the end of the container until its size
        // reaches its capacity, verifying that each of the insertions
        // performed in (amortized) constant time
        while (v.size () < nextcap) {

            n_copy = UserClass::n_total_copy_ctor_;
            n_asgn = UserClass::n_total_op_assign_;

            v.insert (v.end (), Vector::value_type ());

            rw_assert (nextcap >= v.capacity (), 0, __LINE__,
                       "vector<UserClass>::insert (end(), value_type()); "
                       "capacity == %zu, got %zu after %zu insertions",
                       nextcap, v.capacity (), v.size ());

            if (nextcap < v.capacity ())  break;

            // compute the number of copy ctor and assignment operator calls
            n_copy = UserClass::n_total_copy_ctor_ - n_copy;
            n_asgn = UserClass::n_total_op_assign_ - n_asgn;

            // verify that the copy ctor was invoked exactly once
            rw_assert (1U == n_copy, 0, __LINE__,
                       "vector<UserClass>::insert (end(), value_type()); "
                       "expected 1 invocation of the copy ctor, got %zu",
                       n_copy);

            // verify that the assignment operator was not invoked
            rw_assert (n_asgn == 0, 0, __LINE__,
                       "vector<UserClass>::insert (end(), value_type()); "
                       "expected 0 invocations of the assignment "
                       "operator, got %zu", n_asgn);
        }

        // inserting the next element must reallocate
        v.insert (v.end (), Vector::value_type ());

        rw_assert (v.capacity () > nextcap, 0, __LINE__,
                   "vector<UserClass>::insert (end(), value_type()); "
                   "capacity > %zu, got %zu after %zu insertions",
                   nextcap, v.capacity (), v.size ());
    }
}

/**************************************************************************/

// exercise vector<>::insert() and vector::erase()
// focus on the complexity of the function

void test_complexity ()
{
    rw_info (0, 0, 0, "std::vector<UserClass>::insert(iterator, "
                      "const_reference)");
    rw_info (0, 0, 0, "std::ector<UserClass>::insert(iterator, size_type, "
                      "const_reference)");
    rw_info (0, 0, 0, "template <class InputIterator> "
                      "std::vector<UserClass>::insert"
                      "(iterator, InputIterator, InputIterator)");
    rw_info (0, 0, 0, "std::vector<UserClass>::erase(iterator)");
    rw_info (0, 0, 0, "std::vector<UserClass>::erase(iterator, iterator)");

    typedef std::vector<UserClass, std::allocator<UserClass> > Vector;
    typedef Vector::iterator                                   VectorIter;
    typedef Vector::size_type                                  VectorSize;

    bool success = true;

    Vector v0;
    Vector v1;
    Vector v2;

    VectorIter it = v0.begin ();
    VectorIter it1 = v1.begin ();
    VectorIter it2 = v2.begin ();

    VectorSize i;

    for (i = 0; i < rw_opt_nloops; i++) {
        // initialize the current size, capacity and begin iterator variables
        VectorSize v0_size = v0.size ();
        VectorSize v0_cap = v0.capacity ();
        VectorIter v0_beg = v0.begin ();

        UserClass val;

        // reset the UserClass totals
        UserClass::reset_totals ();

        // inseert val into the vector
        it = v0.insert (it, val);

        // 23.2.4.3, p1: causes reallocation if the new size
        //               is greater than the old capacity
        rw_assert (v0_cap <= v0.size () || v0_beg == v0.begin (),
                   0, __LINE__,
                   "vector<UserClass>::insert(iterator, const_reference) "
                   "invalidated iterators w/o reallocation");

        // make sure that the vector is now one element larger
        rw_assert (v0.size () == v0_size + 1,
                   0, __LINE__,
                   "vector<UserClass>::insert(iterator, const_reference); "
                   "new size = %zu, expected %zu", v0.size (), v0_size + 1);

        // make sure that the returned iterator points to the new value
        rw_assert (*it == val,
                   0, __LINE__,
                   "vector<UserClass>::insert(iterator, const_reference); "
                   "returned iterator doesn't point at inserted value");

        // 23.2.4.3, p2: the complexity is linear in the number of elements
        //               in the range [first, last) plus the distance to the
        //               end of the vector.
        if(v0.begin () != v0_beg) {
            rw_assert ((VectorSize)UserClass::n_total_copy_ctor_ == v0.size (),
                       0, __LINE__,
                       "vector<UserClass>::insert() - fails complexity "
                       "copy_ctor called %zu; size = %zu",
                       UserClass::n_total_copy_ctor_, v0.size ());
        }
        else {
            rw_assert (   (UserClass::n_total_copy_ctor_
                         + UserClass::n_total_op_assign_)
                       == std::size_t (v0.end() - it),
                       0, __LINE__,
                       "vector<UserClass>::insert(iterator, const_reference) - "
                       "fails complexity copy_ctor and assign called "
                       "%zu correct = %zu", UserClass::n_total_copy_ctor_ +
                        UserClass::n_total_op_assign_, (v0.end() - it));
        }

        // every 5 loops erase 'it'
        if (i % 5) {
            it1 = it;
            Vector::value_type next_val;

            // if we are not at the end of the vector then increment it1 and
            // if we are still not at the end then set next_val = *it1
            // because when we erase it, that will be the value it should
            // point to
            if (it != v0.end () && ++it1 != v0.end())
                next_val = *it1;

            VectorSize num_left =  v0.end () - it - 1;
            VectorSize X_count =  UserClass::count_;

            v0_size = v0.size ();

            // reset the totals for the complexity test
            UserClass::reset_totals ();
            it = v0.erase (it);

            // make sure only one element was erased
            rw_assert (v0.size () != v0_size - 1, 0, __LINE__,
                       "vector<UserClass>::erase(iterator); new size = %zu, "
                       "expected %zu", v0.size (), v0_size - 1);

            // if we were not at the end of the vector then *it should be
            // next_val
            rw_assert (it == v0.end () || *it == next_val, 0, __LINE__,
                       "vector<UserClass>::erase(iterator); "
                       "returned iterator doesn't point at next value ");

            // 23.2.4.2, p4: The assignment operator of UserClass should be
            //               called the number of times equal to the number of
            //               elements in the vector after the erased elements.
            rw_assert ((VectorSize)UserClass::n_total_op_assign_ != num_left,
                       0, __LINE__,
                       "vector<UserClass>::erase(iterator) - fails "
                       "complexity assign called %zu correct = %zu",
                       UserClass::n_total_op_assign_, num_left);


            // 23.2.4.2, p4: The destructor of UserClass should be called
            //               the number of times equal to the number of
            //               elements erased
            rw_assert ((VectorSize)X_count - UserClass::count_ == 1,
                       0, __LINE__,
                       "vector<UserClass>::erase(iterator) - "
                       "fails complexity destructor called %zu "
                       "correct = %zu", X_count - UserClass::count_ , 1);
        }

        // Don't always insert at the beginning
        if (i % 2 && it != v0.end ())
            it++;
    }

    // now lets test nultiple insert and erase functions
    for (i = 0; i < rw_opt_nloops; i++) {
        VectorSize v1_size = v1.size ();
        VectorSize v1_cap = v1.capacity ();
        VectorIter v1_beg = v1.begin ();

        UserClass val;
        UserClass::reset_totals ();

        // insert `i' copies of `val' in the middle
        VectorSize offset = (v1.size () ? (i / 2) % v1.size () : 0);
        it1 = v1.begin () + offset;
        v1.insert (it1, i, val);

        // since insert doesn't return the next position after the
        //inserted elements we will calculate this ourself
        it1 = v1.begin () + offset + i;

        // 23.2.4.3, p1: causes reallocation if the new size
        //               is greater than the old capacity
        rw_assert (v1_cap <= v1.size () || v1_beg == v1.begin (),
                   0, __LINE__,
                   "vector<UserClass>::insert(iterator, const_reference) "
                   "invalidated iterators w/o reallocation");

        // make sure that the vector increased in size by the correct number
        rw_assert (v1.size () == v1_size + i,
                   0, __LINE__,
                   "vector<UserClass>::insert(iterator, const_reference); "
                   "new size = %zu, expected %zu", v1.size (), v1_size + i);

        // 23.2.4.3, p2: the complexity is linear in the number of elements
        //               in the range [first, last) plus the distance to the
        //               end of the vector.
        if (v1.begin () != v1_beg) {
            rw_assert ((VectorSize)UserClass::n_total_copy_ctor_ == v1.size (),
                       0, __LINE__,
                       "vector<UserClass>::insert(iterator, const_reference) - "
                       "fails complexity copy_ctor called %zu size =%zu",
                       UserClass::n_total_copy_ctor_, v1.size());
        }
        else {
            rw_assert (   i + (v1.end() - it1)
                       == (VectorSize)(UserClass::n_total_copy_ctor_ +
                                       UserClass::n_total_op_assign_),
                       0, __LINE__,
                       "vector<UserClass>::insert(iterator, const_reference) - "
                       "fails complexity copy_ctor and assign "
                       "called %zu correct = %zu",
                       UserClass::n_total_copy_ctor_ +
                       UserClass::n_total_op_assign_,
                       (v1.end() - it1));
        }

        VectorSize old_size = v1.size();

        UserClass new_val;

        Vector::value_type expected_val =
            v1.size () ? *(v1.begin () + (v1.size () / 2)) : new_val;

        UserClass::reset_totals ();
        // set up some variable that will keep track of how many we are going
        // to erase, how many are left after the last element we are erasing
        // and what the current number of Xs are in existance.
        VectorSize num_destroy = v1.size () / 2;

        VectorSize num_left =
            v1.end () - (v1.begin () + (v1.size () / 2));

        VectorSize X_count =  UserClass::count_;

        it1 = v1.erase(v1.begin (), v1.begin () + (v1.size () / 2));

        // make sure the correct number of elements were erased
        rw_assert (v1.size () == old_size - (old_size / 2),
                   0, __LINE__,
                   "vector<UserClass>::erase(iterator, iterator); "
                   "new size = %zu, expected %zu", v1.size (),
                   old_size - (old_size / 2));

        // if there is at least one element left then make sure that it1 now
        // points to the expected next element in the vector.
        rw_assert (v1.size () == 0 || *it1 == expected_val,
                   0, __LINE__,
                   "vector<UserClass>::erase(iterator, iterator); "
                   "returned iterator doesn't point at next value ");

        // 23.2.4.2, p4: The assignment operator of UserClass should be called
        //               the number of times equal to the number of elements
        //               in the vector after the erased elements.
        rw_assert ((VectorSize)UserClass::n_total_op_assign_ == num_left,
                   0, __LINE__,
                   "vector<UserClass>::erase(iterator, iterator) - fails "
                   "complexity assign called %zu correct = %zu",
                   UserClass::n_total_op_assign_, num_left);

        // 23.2.4.2, p4: The destructor of UserClass should be called the number
        //               of times equal to the number of elements erased
        rw_assert (X_count - UserClass::count_ == num_destroy,
                   0, __LINE__,
                   "vector<UserClass>::erase(iterator, iterator) - fails "
                   "complexity destructor called %zu correct = %zu",
                   X_count - UserClass::count_ , num_destroy);


    }

#if !defined (_MSC_VER) || _MSC_VER >= 1300

    v0.clear ();
    for (i = 0; i < rw_opt_nloops; i++) {
        VectorSize v2_size = v2.size ();
        VectorSize v2_cap  = v2.capacity ();
        VectorIter v2_beg  = v2.begin ();

        UserClass val;
        v0.insert (v0.begin(), val);

        // 23.2.4.3, p1: causes reallocation if the new size
        //               is greater than the old capacity
        rw_assert (v2_cap <= v2.size () || v2_beg == v2.begin (),
                   0, __LINE__,
                   "vector<UserClass>::insert(iterator, const_reference) "
                   "invalidated iterators w/o reallocation");

        UserClass::reset_totals ();
        v2.insert(v2.begin(), v0.begin(), v0.end());

        if (v2.begin () != v2_beg)
            rw_assert ((VectorSize)UserClass::n_total_copy_ctor_ == v2.size (),
                       0, __LINE__,
                       "vector<UserClass>::insert(iterator, const_reference) - "
                       "fails complexity copy_ctor called %zu size =%zu",
                       UserClass::n_total_copy_ctor_, v2.size());
        else {
            rw_assert (    (VectorSize)(UserClass::n_total_copy_ctor_
                                      + UserClass::n_total_op_assign_)
                       ==  v0.size () + (v2.end() - (v2.begin() + v0.size())),
                       0, __LINE__,
                       "vector<UserClass>::insert(iterator, const_reference) - "
                       "fails complexity copy_ctor and assign called "
                       "%zu correct = %zu",
                       UserClass::n_total_copy_ctor_ +
                       UserClass::n_total_op_assign_,
                       v0.size() + (v2.end() - (v2.begin() + v0.size())));
        }


        rw_assert (v2.size () == v2_size + v0.size (),
                   0, __LINE__,
                   "vector<UserClass>::insert(iterator, const_reference); "
                   "new size = %zu, expected %zu", v2.size (),
                   v2_size + v0.size ());

        VectorSize old_size = v2.size();

        VectorIter expected_it = v2.begin () + (v2.size () / 2) ;

        Vector::value_type expected_val = *expected_it;

        UserClass::reset_totals ();

        VectorSize num_destroy =  v2.size () / 2;

        VectorSize num_left =
            v2.end () - (v2.begin () + (v2.size () / 2));

        VectorSize X_count =  UserClass::count_;

        it2 = v2.erase(v2.begin (), v2.begin () + (v2.size () / 2));


        rw_assert (v2.size () == v2_size + v0.size() - (old_size / 2),
                   0, __LINE__,
                   "vector<UserClass>::erase(iterator, iterator); "
                   "new size = %zu, expected %zu",
                   v2.size (), v2_size + i - (old_size / 2));

        rw_assert (v2.size () == 0 || *it2 == expected_val,
                   0, __LINE__,
                   "vector<UserClass>::erase(iterator, iterator); "
                   "returned iterator doesn't point at next value ");

        rw_assert ((VectorSize)UserClass::n_total_op_assign_ == num_left,
                   0, __LINE__,
                   "vector<UserClass>::erase(iterator, iterator) - "
                   "fails complexity assign called %zu correct = %zu",
                   UserClass::n_total_op_assign_, num_left);

        rw_assert (X_count - UserClass::count_ == num_destroy,
                   0, __LINE__,
                   "vector<UserClass>::erase(iterator, iterator) - "
                   "fails complexity destructor called %zu correct = %zu",
                   X_count - UserClass::count_ , num_destroy);

    }
#endif   // !defined (_MSC_VER) || _MSC_VER >= 1300

    rw_assert (success, 0, __LINE__, "vector<UserClass>::insert()");
    rw_assert (success, 0, __LINE__, "vector<UserClass>::erase()");
}

/**************************************************************************/

template <class T, class Allocator>
void test_push_back (const std::vector<T, Allocator>*)
{
    rw_info (0, 0, 0, "std::vector<UserClass>::push_back(const_reference)");

    typedef std::vector<T, Allocator>        MyVector;
    typedef typename MyVector::value_type    ValueType;
    typedef typename MyVector::size_type     SizeType;
    typedef typename MyVector::pointer       Pointer;
    typedef typename MyVector::const_pointer ConstPointer;
    typedef typename MyVector::iterator      Iterator;

    MyVector v1;
    MyVector v2;

    const ValueType val = ValueType ();

    for (SizeType i = 0; i != SizeType (4096); ++i) {

        // perform the two operations from Table 68
        v1.push_back (val);

        v2.insert (v2.end (), val);

        // verify that effects on both sequences
        // are identical as required by Table 68
        rw_assert (v1.size () == v2.size (),
                   0, __LINE__,
                   "Expected std::vector::size to "
                   "compare equal; got %zu and %zu",
                   v1.size (), v2.size ());

        rw_assert (v1.capacity () == v2.capacity (),
                   0, __LINE__,
                   "Expected std::vector::capacity to "
                   "compare equal; got %zu and %zu",
                   v1.capacity (), v2.capacity ());

        rw_assert (v1.end () - v1.begin () == v2.end () - v2.begin (),
                   0, __LINE__,
                   "Expected std::vector::end() - std::vector::begin() "
                   "to  compare equal; got %zu and %zu",
                   v1.end () - v1.begin (), v2.end () - v2.begin ());
    }
}

/**************************************************************************/

static int
run_test (int /* argc */, char** /* argv */)
{
    if (rw_opt_no_complexity) {
        rw_note (0, 0, __LINE__, "complexity test disabled");
    }
    else {
        test_complexity ();
    }

    if (rw_opt_no_push_back) {
        rw_note (0, 0, __LINE__, "push_back test disabled");
    }
    else {
        test_push_back ((std::vector<UserClass, std::allocator<UserClass> >*)0);
    }

    if (rw_opt_no_insert) {
        rw_note (0, 0, __LINE__, "insert test disabled");
    }
    else {
        test_insert ();
    }

    if (rw_opt_no_insert_range) {
        rw_note (0, 0, __LINE__, "insert range test disabled");
    }
    else {
        if (rw_opt_no_input_iter) {
            rw_note (0, 0, __LINE__,
                     "InputIterator test disabled");
        }
        else {
            test_insert_range (InputIter<UserClass>(0, 0, 0));
        }

        if (rw_opt_no_forward_iter) {
            rw_note (0, 0, __LINE__,
                     "ForwardIterator test disabled");
        }
        else {
            test_insert_range (ConstFwdIter<UserClass>());
        }

        if (rw_opt_no_bidir_iter) {
            rw_note (0, 0, __LINE__,
                     "BidirectionalIterator test disabled");
        }
        else {
            test_insert_range (ConstBidirIter<UserClass>());
        }

        if (rw_opt_no_random_iter) {
            rw_note (0, 0, __LINE__,
                     "RandomAccessIterator test disabled");
        }
        else {
            test_insert_range (ConstRandomAccessIter<UserClass>());
        }

    }

    if (rw_opt_no_insert_at_end) {
        rw_note (0, 0, __LINE__, "insert at end test disabled");
    }
    else {
        test_insert_at_end ();
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char** argv)
{
    return rw_test (argc, argv, __FILE__,
                    "lib.vector.modifiers",
                    0 /* no comment */,
                    run_test,
                    "|-nloops#1 "
                    "|-no-insert# "
                    "|-no-insert_range# "
                    "|-no-insert_at_end# "
                    "|-no-complexity# "
                    "|-no-input_iter# "
                    "|-no-forward_iter# "
                    "|-no-bidir_iter# "
                    "|-no-random_iter# "
                    "|-no-push_back#",
                    &rw_opt_nloops,
                    &rw_opt_no_insert,
                    &rw_opt_no_insert_range,
                    &rw_opt_no_insert_at_end,
                    &rw_opt_no_complexity,
                    &rw_opt_no_input_iter,
                    &rw_opt_no_forward_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_random_iter,
                    &rw_opt_no_push_back,
                    0 /* sentinel*/);
}
