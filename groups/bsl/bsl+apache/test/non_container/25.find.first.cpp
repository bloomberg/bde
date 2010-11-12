/***************************************************************************
 *
 * find.first.cpp - test exercising 25.1.4 [lib.alg.find.first.of]
 *
 * $Id: 25.find.first.cpp 510071 2007-02-21 15:58:53Z faridz $
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

#include <algorithm>    // for find_first_of
#include <cstring>      // for size_t, strlen()

#include <alg_test.h>   
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test

/**************************************************************************/

_RWSTD_NAMESPACE (std) { 

#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
FwdIter<eq_comp<base<> > >
find_first_of (FwdIter<eq_comp<base<> > >, FwdIter<eq_comp<base<> > >, 
               FwdIter<eq_comp<base<> > >, FwdIter<eq_comp<base<> > >);

template
FwdIter<eq_comp<base<> > >
find_first_of (FwdIter<eq_comp<base<> > >, FwdIter<eq_comp<base<> > >, 
               FwdIter<eq_comp<base<> > >, FwdIter<eq_comp<base<> > >, 
               binary_predicate<eq_comp<base<> > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

template <class T, class U>
struct EqualityPredicate
{
    static std::size_t funcalls_;

    // dummy arguments provided to prevent the class
    // from being default constructible
    EqualityPredicate (T* /* dummy */, U* /* dummy */) {
        funcalls_ = 0;
    }

    // return a type other than bool but one that is implicitly
    // convertible to bool to detect incorrect assumptions
    class ConvertibleToBool {
        bool result_;
    public:
        ConvertibleToBool (bool res): result_ (res) { /* empty */ }
        operator bool() const { return result_; }
    };

    ConvertibleToBool operator() (const T &x, const U &y) /* non-const */ {
        ++funcalls_;
        return x == y;
    }
};

template <class T, class U>
std::size_t EqualityPredicate<T, U>::funcalls_;

/**************************************************************************/

// used to initialize an array of objects of type T
static const char *tinit_begin;

int tinit ()
{
    typedef unsigned char UChar;

    return UChar (*tinit_begin++);
}


// exercises std::find_end()
template <class ForwardIterator1, class ForwardIterator2,
          class Predicate, class T>
void do_test (int               line,     // line number of test case
              const char       *src,      // source sequence
              const char       *fseq,     // sequence to be found
              std::size_t       resoff,   // offset of result
              ForwardIterator1 &dummy_iter1,
              ForwardIterator2 &dummy_iter2,
              const T*,
              const Predicate  *pred)
{
    static const char* const it1name = type_name (dummy_iter1, (T*)0);
    static const char* const it2name = type_name (dummy_iter2, (T*)0);
    static const char* const pname   = pred ? "Predicate" : "operator==";

    const std::size_t nsrc  = std::strlen (src);
    const std::size_t nfseq = std::strlen (fseq);

    if (std::size_t (-1) == resoff)
        resoff = nsrc;

    // have the T default ctor initialize objects from `src'
    tinit_begin = src;
    T::gen_     = tinit;

    T* const tsrc = new T [nsrc];

    T* const src_begin = tsrc;
    T* const src_end = tsrc + nsrc;

    // have the T default ctor initialize sequence to be found
    tinit_begin = fseq;

    T* const tfseq = new T [nfseq];

    T* const fseq_begin = tfseq;
    T* const fseq_end   = tfseq + nfseq;

    // create iterators
    const ForwardIterator1 first1 =
        make_iter (src_begin, src_begin, src_end, dummy_iter1);

    const ForwardIterator1 last1 =
        make_iter (src_end, src_begin, src_end, dummy_iter1);

    const ForwardIterator2 first2 =
        make_iter (fseq_begin, fseq_begin, fseq_end, dummy_iter2);

    const ForwardIterator2 last2 =
        make_iter (fseq_end, fseq_begin, fseq_end, dummy_iter2);

    // compute the number of invocations of the operator==()
    std::size_t last_n_op_assign = T::n_total_op_assign_;

    const Predicate fun((T*)0, (T*)0);   // dummy arguments

    const ForwardIterator1 res = pred ? 
          std::find_first_of (first1, last1, first2, last2, fun)
        : std::find_first_of (first1, last1, first2, last2);
    
    // silence a bogus EDG eccp remark #550-D:
    // variable "res" was set but never used
    _RWSTD_UNUSED (res);

    if (!rw_assert (res.cur_ == first1.cur_ + resoff, 0, line, 
                    "line %d: find_first_of<%s, %s>(it = \"%s\", \"%s\")"
                    " == (it + %zu), got (it + %td)",
                    __LINE__, it1name, it2name, src, fseq,
                    resoff, res.cur_ - first1.cur_)) {
        delete[] tsrc;
        delete[] tfseq;
        return;
    }

    const std::size_t npreds = pred ?
        fun.funcalls_ : T::n_total_op_assign_ - last_n_op_assign;

    rw_assert (npreds <= nfseq * nsrc, 0, line, 
               "line %d: find_first_of<%s, %s>(it = \"%s\", \"%s\")"
               "%s invoked %zu times, expected no more than %td",
               __LINE__, it1name, it2name, src, fseq, pname, 
               npreds, nfseq * nsrc);

    delete[] tsrc;
    delete[] tfseq;
}

/**************************************************************************/

template <class ForwardIterator1, class ForwardIterator2,
          class Predicate, class T>
void run_tests (const ForwardIterator1 &dummy_iter1, 
                const ForwardIterator2 &dummy_iter2,
                const T*,
                const Predicate        *pred)
{
    static const char* const it1name = type_name (dummy_iter1, (T*)0);
    static const char* const it2name = type_name (dummy_iter2, (T*)0);

    rw_info (0, 0, 0, 
             "%s std::find_first_of (%1$s, %1$s, %s, %2$s%{?}, %s%{;})", 
             it1name, it2name, 0 != pred, "Predicate");
    
#define TEST(src, fnd, off_res)                         \
    do_test (__LINE__, src, fnd, std::size_t (off_res), \
             dummy_iter1, dummy_iter2, (UserClass*)0, pred)

    //    +------------------ subject sequence
    //    |                 +--- sequence to be found
    //    |                 |   
    //    |                 |      +-- offset of returned iterator,
    //    |                 |      |   -1 denotes the end of sequence
    //    v                 v      v
    TEST ("abcdefghijk",    "a",   0);
    TEST ("abcdefghijk",    "b",   1);
    TEST ("abcdefghijk",    "c",   2);
    TEST ("abcdefghijk",    "d",   3);
    TEST ("abcdefghijk",    "e",   4);
    TEST ("abcdefghijk",    "f",   5);
    TEST ("abcdefghijk",    "g",   6);
    TEST ("abcdefghijk",    "h",   7);
    TEST ("abcdefghijk",    "i",   8);
    TEST ("abcdefghijk",    "j",   9);
    TEST ("abcdefghijk",    "k",  10);

    TEST ("aabcdefghijk",   "a",   0);
    TEST ("abbcdefghijk",   "b",   1);
    TEST ("abccdefghijk",   "c",   2);
    TEST ("abcddefghijk",   "d",   3);
    TEST ("abcdeefghijk",   "e",   4);
    TEST ("abcdeffghijk",   "f",   5);
    TEST ("abcdefgghijk",   "g",   6);
    TEST ("abcdefghhijk",   "h",   7);
    TEST ("abcdefghijjk",   "i",   8);
    TEST ("abcdefghijjk",   "j",   9);
    TEST ("abcdefghijkk",   "k",  10);

    TEST ("bbcdefghijk",   "ab",   0);
    TEST ("accdefghijk",   "bc",   1);
    TEST ("abddefghijk",   "cd",   2);
    TEST ("abceefghijk",   "de",   3);
    TEST ("abcdffghijk",   "ef",   4);
    TEST ("abcdegghijk",   "fg",   5);
    TEST ("abcdefhhijk",   "gh",   6);
    TEST ("abcdefgiijk",   "hi",   7);
    TEST ("abcdefghjjk",   "ij",   8);
    TEST ("abcdefghikk",   "jk",   9);

    TEST ("bbcdefghijk",    "a",  -1);
    TEST ("accdefghijk",    "b",  -1);
    TEST ("abddefghijk",    "c",  -1);
    TEST ("abceefghijk",    "d",  -1);
    TEST ("abcdffghijk",    "e",  -1);
    TEST ("abcdegghijk",    "f",  -1);
    TEST ("abcdefhhijk",    "g",  -1);
    TEST ("abcdefgiijk",    "h",  -1);
    TEST ("abcdefghjjk",    "i",  -1);
    TEST ("abcdefghikk",    "j",  -1);

    TEST ("",               "a",  -1);
    TEST ("abcdefghijk",    "",   -1);
}


/**************************************************************************/

/* extern */ int rw_opt_no_fwd_iter;     // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;   // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;     // --no-RandomAccessIterator
/* extern */ int rw_opt_no_predicate;    // --no-Predicate


template <class ForwardIterator, class Predicate, class T>
void run_test (const ForwardIterator &dummy, T*, Predicate* pred)
{
    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        run_tests (dummy, FwdIter<T>(), (T*)0, pred);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        run_tests (dummy, BidirIter<T>(), (T*)0, pred);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        run_tests (dummy, RandomAccessIter<T>(), (T*)0, pred);
    }
}

/**************************************************************************/

static void
run_test (bool test_predicate)
{
    rw_info (0, 0, 0, 
             "template <class %s, class %s> "
             "%1$s std::find_first_of (%1$s, %1$s, %2$s, %2$s%{?}, %s%{;})",
             "ForwardIterator1", "ForwardIterator2", 
             0 != test_predicate, "Predicate");

    typedef EqualityPredicate<UserClass, UserClass> EqPred;
    const EqPred* const pred = test_predicate ? (EqPred*)1 : (EqPred*)0;

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {       
        run_test (FwdIter<UserClass>(), (UserClass*)0, pred);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        run_test (BidirIter<UserClass>(), (UserClass*)0, pred);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        run_test (RandomAccessIter<UserClass>(), (UserClass*)0, pred);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    run_test (false);

    if (rw_opt_no_predicate) {
        rw_note (0, __FILE__, __LINE__, "Predicate test disabled");
    }
    else {
        run_test (true);
    }
                
    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.find.first.of",
                    0 /* no comment */, run_test,                    
                    "|-no-ForwardIterator#"
                    "|-no-BidirectionalIterator#"
                    "|-no-RandomAccessIterator#"
                    "|-no-Predicate#",
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter,
                    &rw_opt_no_predicate);
}
