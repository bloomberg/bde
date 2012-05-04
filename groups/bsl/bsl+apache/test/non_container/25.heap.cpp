/***************************************************************************
 *
 * 25.heap.cpp - test exercising 25.3.6 [lib.alg.heap.operations]
 *
 * $Id: 25.heap.cpp 510970 2007-02-23 14:57:45Z faridz $
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

// MSVC 6.0 gets an ICE in std::__adjust_heap() in <algorithm.cc>
#if defined (_MSC_VER) && _MSC_VER <= 1300
#  define _RWSTD_NO_EXPLICIT_INSTANTIATION
#endif

#include <algorithm>    // for make_heap, sort_heap, push_heap, pop_heap
#include <cstring>      // for strlen, size_t

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

_RWSTD_NAMESPACE (std) {

#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

// 25.3.6.1 - push_heap [lib.push.heap]
template
void
push_heap (RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
           RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >);

template
void
push_heap (RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
           RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
           binary_predicate<lt_comp<assign<base<cpy_ctor> > > >);

// 25.3.6.2 - pop_heap [lib.pop.heap]
template
void
pop_heap (RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
          RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >);

template
void
pop_heap (RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
          RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
          binary_predicate<lt_comp<assign<base<cpy_ctor> > > >);

// 25.3.6.3 - make_heap [lib.make.heap]
template
void
make_heap (RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
           RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >);

template
void
make_heap (RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
           RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
           binary_predicate<lt_comp<assign<base<cpy_ctor> > > >);

// 25.3.6.4 - sort_heap [lib.sort.heap]
template
void
sort_heap (RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
                RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >);

template
void
sort_heap (RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
           RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
           binary_predicate<lt_comp<assign<base<cpy_ctor> > > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

template <class T>
struct Less
{
    static std::size_t funcalls_;

    // dummy arguments provided to prevent the class from being
    // default constructible and implicit conversion from int
    Less (int /* dummy */, int /* dummy */) {
        funcalls_ = 0;
    }

    // return a type other than bool but one that is implicitly
    // convertible to bool to detect incorrect assumptions
    conv_to_bool operator() (const T &x, const T &y) /* non-const */ {
        ++funcalls_;
        return conv_to_bool::make (x.data_.val_ < y.data_.val_);
    }

    static const char* name () { return "Less"; }

private:
    void operator= (Less&);   // not assignable
};

template<class T> std::size_t Less<T>::funcalls_;

/**************************************************************************/

// helper function to verify that the sequence is a valid heap
template <class T>
bool is_heap (const T* seq, const std::size_t len)
{
  std::size_t parent = 0;
  for (std::size_t child = 1; child < len; ++child) {

      if (seq [parent].data_.val_ < seq [child].data_.val_)
          return false;

      if ((child & 1) == 0)
          ++parent;
  }

  return true;
}

/**************************************************************************/

// exercises push_heap, pop_heap, make_heap, sort_heap: 25.3.6.1 - 25.3.6.4
template <class T, class Predicate>
void test_heap_operation (int               line,
                          const char       *src,
                          const T*,
                          const Predicate  *ppred,
                          const std::size_t finx)
{
    static const char* const fnames[] = {
        "make_heap", "push_heap", "pop_heap", "sort_heap"
    };

    typedef RandomAccessIter<T> RandIter;
    RandIter rand_iter (0, 0, 0);

    const char* const itname  = type_name (rand_iter, (T*)0);
    const char* const fname   = fnames[finx];
    const char* const funname = Predicate::name();

    const std::size_t nsrc = std::strlen (src);

    T* const xsrc     = T::from_char (src, nsrc);
    T* const xsrc_end = xsrc + nsrc;

    int val = xsrc[0].data_.val_;

    const RandIter first = make_iter (xsrc,     xsrc, xsrc_end, rand_iter);
    const RandIter last  = make_iter (xsrc_end, xsrc, xsrc_end, rand_iter);

    const Predicate pred (0, 0);

    const std::size_t last_n_op_lt = T::n_total_op_lt_;

    // for convenience
#define HEAP_OP(opname) \
    if (ppred) opname (first, last, pred); else opname (first, last)

    std::size_t ops = 0;

    // exercise heap operation and compute the expected complexity
    switch (finx) {
    case 0: HEAP_OP (std::make_heap); ops = 3 * nsrc; break;
    case 1: HEAP_OP (std::push_heap); ops = ilog2 (nsrc); break;
    case 2: HEAP_OP (std::pop_heap);  ops = 2 * ilog2 (nsrc); break;
    case 3: HEAP_OP (std::sort_heap); ops = nsrc * ilog2 (nsrc); break;
    }

    const std::size_t n_ops_lt = ppred ?
        Predicate::funcalls_ : T::n_total_op_lt_ - last_n_op_lt;

    const std::size_t len = 2 != finx ? nsrc : nsrc - 1;

    if (2 == finx) {     // pop_heap special verification
        // verify that the maximal element is in
        // the end of the sequence: 25.3.6.2, p2
        rw_assert (val == xsrc[nsrc - 1].data_.val_, 0, line,
                   "line %d std::%s <%s%{?}, %s%{;}> (\"%s\", ...) ==> "
                   "\"%{X=*.*}\", last is %#c, expected %#c",
                   __LINE__, fname, itname, ppred, funname, src,
                   int (nsrc), -1, xsrc, xsrc [nsrc - 1].data_.val_, val);
    }

    if (3 == finx) {     // sort_heap special verification
        // verify that we got a sorted sequence: 25.3.6.4, p1
        rw_assert (is_sorted_lt (xsrc, xsrc_end), 0, line,
                   "line %d std::%s <%s%{?}, %s%{;}> (\"%s\", ...) ==> "
                   "\"%{X=*.*}\" is not sorted",
                   __LINE__, fname, itname, ppred, funname, src,
                   int (nsrc), -1, xsrc);
    }
    else {
        // verify that we got a heap:
        // 25.3.6.1, p2; 25.3.6.2, p2; 25.3.6.3, p1
        rw_assert (is_heap (xsrc, len), 0, line,
                   "line %d std::%s <%s%{?}, %s%{;}> (\"%s\", ...) ==> "
                   "\"%{X=*.*}\" is not a heap",
                   __LINE__, fname, itname, ppred, funname, src,
                   int (len), -1, xsrc);
    }

    // verify the complexity:
    // 25.3.6.1, p3; 25.3.6.2, p3; 25.3.6.3, p2; 25.3.6.4, p2
    rw_assert (n_ops_lt <= ops, 0, line,
               "line %d std::%s <%s%{?}, %s%{;}> (\"%s\", ...) ==> "
               "\"%{X=*.*}\" complexity: got %zu invocations of %s, "
               "expected no more %zu",
               __LINE__, fname, itname, ppred, funname, src,
               int (nsrc), -1, xsrc, n_ops_lt,
               ppred ? "predicate" : "operator< ()", ops);

    delete[] xsrc;
}

/**************************************************************************/

template <class T, class Predicate>
void test_make_heap (const T*,
                     const Predicate *ppred)
{
    rw_info (0, 0, 0,
             "std::make_heap (%s, %1$s%{?}, %s%{;})",
             "RandomAccessIterator", 0 != ppred, Predicate::name ());

#define TEST(src)                                               \
    test_heap_operation (__LINE__, src, (T*)0, ppred, 0)

    TEST ("a");
    TEST ("ab");
    TEST ("ba");

    TEST ("abcde");
    TEST ("acbed");
    TEST ("cbade");
    TEST ("debca");
    TEST ("ecace");
    TEST ("accee");
    TEST ("caced");

    TEST ("abcdefghij");
    TEST ("bcdeafghij");
    TEST ("dcbeafihgj");
    TEST ("badcfehgji");
    TEST ("ebadcfhijg");

    TEST ("jihgfedcba");
    TEST ("ijhgefdcab");

    TEST ("ababababab");
    TEST ("babbbababb");

#undef TEST
}

/**************************************************************************/

template <class T, class Predicate>
void test_push_heap (const T*,
                     const Predicate *ppred)
{
    rw_info (0, 0, 0,
             "std::push_heap (%s, %1$s%{?}, %s%{;})",
             "RandomAccessIterator", 0 != ppred, Predicate::name ());

#define TEST(src)                                               \
    test_heap_operation (__LINE__, src, (T*)0, ppred, 1)

    TEST ("a");
    TEST ("ab");
    TEST ("ba");

    TEST ("bac");
    TEST ("cba");
    TEST ("aac");

    TEST ("ecdaba");
    TEST ("ecdabc");
    TEST ("ecdabe");

    TEST ("jighefcadj");
    TEST ("jighefcada");
    TEST ("jighefcadh");

    TEST ("jhigcfbeda");
    TEST ("jhigcfbedj");
    TEST ("jhigcfbedh");
    TEST ("jhigcfbedf");
    TEST ("jhigcfbede");
    TEST ("jhigcfbedg");

    TEST ("bbbbbaaaaa");
    TEST ("bbbbbaaaab");

#undef TEST
}

/**************************************************************************/

template <class T, class Predicate>
void test_pop_heap (const T*,
                     const Predicate *ppred)
{
    rw_info (0, 0, 0,
             "std::pop_heap (%s, %1$s%{?}, %s%{;})",
             "RandomAccessIterator", 0 != ppred, Predicate::name ());

#define TEST(src)                                               \
    test_heap_operation (__LINE__, src, (T*)0, ppred, 2)

    TEST ("a");
    TEST ("ba");

    TEST ("aaa");
    TEST ("cba");
    TEST ("caa");

    TEST ("ecdab");
    TEST ("ecdab");
    TEST ("ecdab");

    TEST ("jighefcadb");
    TEST ("jighcfdbea");
    TEST ("jhigcfbeda");
    TEST ("jihgfedacb");
    TEST ("iihegfabdc");
    TEST ("iihgfedcba");
    TEST ("iihgefdcab");

    TEST ("bbbbbaaaaa");

#undef TEST
}

/**************************************************************************/

template <class T, class Predicate>
void test_sort_heap (const T*,
                     const Predicate *ppred)
{
    rw_info (0, 0, 0,
             "std::sort_heap (%s, %1$s%{?}, %s%{;})",
             "RandomAccessIterator", 0 != ppred, Predicate::name());

#define TEST(src)                                               \
    test_heap_operation (__LINE__, src, (T*)0, ppred, 3)

    TEST ("a");
    TEST ("ba");

    TEST ("aaa");
    TEST ("cba");
    TEST ("caa");

    TEST ("ecdab");
    TEST ("ecdab");
    TEST ("ecdab");

    TEST ("jighefcadb");
    TEST ("jighcfdbea");
    TEST ("jhigcfbeda");
    TEST ("jihgfedacb");
    TEST ("iihegfabdc");
    TEST ("iihgfedcba");
    TEST ("iihgefdcab");

    TEST ("bbbbbaaaaa");

#undef TEST
}

/**************************************************************************/

/* extern */ int rw_opt_no_make_heap;          // --no-make_heap
/* extern */ int rw_opt_no_push_heap;          // --no-push_heap
/* extern */ int rw_opt_no_pop_heap;           // --no-pop_heap
/* extern */ int rw_opt_no_sort_heap;          // --no-sort_heap
/* extern */ int rw_opt_no_predicate;          // --no-predicate

/**************************************************************************/

template <class T, class Predicate>
void test_heap_operations (const T*,
                           const Predicate *ppred)
{
    if (rw_opt_no_make_heap) {
        rw_note (0, __FILE__, __LINE__,
                 "std::make_heap test disabled");
    }
    else {
        test_make_heap ((T*)0, ppred);
    }

    if (rw_opt_no_push_heap) {
        rw_note (0, __FILE__, __LINE__,
                 "std::push_heap test disabled");
    }
    else {
        test_push_heap ((T*)0, ppred);
    }

    if (rw_opt_no_pop_heap) {
        rw_note (0, __FILE__, __LINE__,
                 "std::pop_heap test disabled");
    }
    else {
        test_pop_heap ((T*)0, ppred);
    }

    if (rw_opt_no_sort_heap) {
        rw_note (0, __FILE__, __LINE__,
                 "std::sort_heap test disabled");
    }
    else {
        test_sort_heap ((T*)0, ppred);
    }
}

/**************************************************************************/

static int run_test (int, char*[])
{
    test_heap_operations ((UserClass*)0, (Less<UserClass>*)0);

    if (rw_opt_no_predicate) {
        rw_note (0, __FILE__, __LINE__,
                 "heap operations predicate test disabled");
    }
    else {
        test_heap_operations ((UserClass*)0, (Less<UserClass>*)1);
    }

    return 0;
}


/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.heap.operations",
                    0 /* no comment */, run_test,
                    "|-no-make_heap#"
                    "|-no-push_heap#"
                    "|-no-pop_heap#"
                    "|-no-sort_heap#"
                    "|-no-predicate",
                    &rw_opt_no_make_heap,
                    &rw_opt_no_push_heap,
                    &rw_opt_no_pop_heap,
                    &rw_opt_no_sort_heap,
                    &rw_opt_no_predicate);
}
