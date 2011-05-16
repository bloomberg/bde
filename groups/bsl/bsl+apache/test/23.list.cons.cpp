/***************************************************************************
 *
 * 23.list.cons.cpp - test exercising list constructors [lib.list.cons]
 *
 * $Id: 23.list.cons.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 **************************************************************************/

#include <list>             // for list
#include <cstddef>          // for size_t

#include <23.list.h>        // for ListMembers
#include <alg_test.h>       // for InputIter
#include <driver.h>         // for rw_test()
#include <rw_allocator.h>   // for UserAlloc
#include <rw_new.h>         // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define Cons(sig)               ListIds::ctor_ ## sig
#define OpSet(sig)              ListIds::op_set_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "bad_alloc", "exception", "UserClass::CopyCtor"
};

/**************************************************************************/

// exercises:
// list (void)
// list (const allocator_type&)
#define alloc_test_cases void_test_cases
static const ContainerTestCase
void_test_cases [] = { 

#undef TEST
#define TEST(dummy) {                            \
        __LINE__, -1, -1, -1, -1, -1,            \
        0, 0, dummy, sizeof dummy - 1,           \
        "", 0, 0                                 \
    }

    //    +--------------------- dummy argument
    //    |
    //    V             
    TEST ("ab"),          

    TEST ("last test")
};

/**************************************************************************/

// exercises:
// list (const list&)
static const ContainerTestCase
ccont_test_cases [] = {

#undef TEST
#define TEST(arg, res) {                            \
        __LINE__, -1, -1, -1, -1, -1,               \
        0, 0, arg, sizeof arg - 1,                  \
        res, sizeof res - 1, 0                      \
    }

    //    +----------------------------------------- source sequence
    //    |               +------------------------- expected result sequence
    //    |               |             
    //    |               |            
    //    V               V           
    TEST ("ab",           "ab"),    

    TEST ("",             ""),   
    TEST ("<U0>",         "<U0>"),
    TEST ("<U0>@2",       "<U0>@2"),

    TEST ("a",            "a"),       
    TEST ("bcd",          "bcd"),       
    TEST ("cdefaihjb",    "cdefaihjb"),      

    TEST ("<U0>@2ab",     "<U0>@2ab"),  
    TEST ("a<U0>@2b",     "a<U0>@2b"), 
    TEST ("ab<U0>@2",     "ab<U0>@2"),  
    TEST ("abefdcc<U0>a", "abefdcc<U0>a"),  

#if TEST_RW_EXTENSIONS  // a bunch of these test cases doesn't add any value
                        // other than making the test run slower
    TEST ("x@128",        "x@128"),
    TEST ("x@207",        "x@207"),
    TEST ("x@334",        "x@334"),
    TEST ("x@540",        "x@540"),
    TEST ("x@873",        "x@873"),
    TEST ("x@1412",       "x@1412"),
    TEST ("x@3695",       "x@3695"),
    TEST ("x@4096",       "x@4096"),
#else
    TEST ("x@20",         "x@20"),
    TEST ("x@32",         "x@32"),
    TEST ("x@63",         "x@63"),
    TEST ("x@64",         "x@64"),
    TEST ("x@65",         "x@65"),
    TEST ("x@127",        "x@127"),
    TEST ("x@128",        "x@128"),
    TEST ("x@129",        "x@129"),
#endif

    TEST ("last test",    "last test")
};

/**************************************************************************/

// exercises:
// list (InputIterator, InputIterator)
// list (InputIterator, InputIterator, const allocator&)
#define range_alloc_test_cases range_test_cases
static const ContainerTestCase
range_test_cases [] = {

#undef TEST
#define TEST(arg, off, size, res, bthrow) {         \
        __LINE__, -1, -1, off, size, -1,            \
        0, 0, arg, sizeof arg - 1,                  \
        res, sizeof res - 1, bthrow                 \
    }

    //    +----------------------------------------- source sequence
    //    |               +------------------------- ctor off argument
    //    |               |   +--------------------- ctor n argument
    //    |               |   |    +---------------- expected result sequence
    //    |               |   |    |                +--- exception info
    //    |               |   |    |                |       0 - no exception
    //    |               |   |    |                |       1 - out_of_range
    //    |               |   |    |                |
    //    |               |   |    |                |
    //    V               V   V    V                V
    TEST ("ab",           0,  2,   "ab",            0),    

    TEST ("",             0,  0,   "",              0),   
    TEST ("<U0>",         0,  1,   "<U0>",          0),
    TEST ("<U0>@2",       0,  2,   "<U0>@2",        0),

    TEST ("a",            0,  1,   "a",             0),       
    TEST ("bcd",          0,  3,   "bcd",           0),       
    TEST ("cdefaihjb",    0,  9,   "cdefaihjb",     0),      

    TEST ("<U0>@2ab",     0,  4,   "<U0>@2ab",      0),  
    TEST ("a<U0>@2b",     0,  4,   "a<U0>@2b",      0), 
    TEST ("ab<U0>@2",     0,  4,   "ab<U0>@2",      0),  
    TEST ("abefdcc<U0>a", 0,  9,   "abefdcc<U0>a",  0),  

#if TEST_RW_EXTENSIONS  // a bunch of these tests doesn't add any value
    TEST ("x@207",        0,  207, "x@207",         0),
    TEST ("x@334",       10,  207, "x@207",         0),
    TEST ("x@207",       50,  128, "x@128",         0),
    TEST ("x@1412",     128,  873, "x@873",         0),
    TEST ("x@1412",       0, 1412, "x@1412",        0),
    TEST ("x@1412",     207,  540, "x@540",         0),
    TEST ("x@874",        1,  873, "x@873",         0),
    TEST ("x@3695",      10, 2284, "x@2284",        0),
    TEST ("x@4096",       0, 4096, "x@4096",        0),
#else
    TEST ("x@64",         0,  64,  "x@64",          0),
    TEST ("x@84",        10,  64,  "x@64",          0),
    TEST ("x@128",       50,  64,  "x@64",          0),
    TEST ("x@64",         1,  63,  "x@63",          0),
#endif
    TEST ("last test",    0,  9,  "last test",      0)
};

/**************************************************************************/

// exercises:
// list (size_type)
static const ContainerTestCase
size_test_cases [] = {

#undef TEST
#define TEST(size, res) {                      \
        __LINE__, -1, size, -1, -1, 0,         \
        0, 0, 0, 0,                            \
        res, sizeof res - 1, 0                 \
    }

    //    +---------------------------------------- ctor n argument 
    //    |         +------------------------------ expected result sequence
    //    |         |             
    //    |         |            
    //    V         V         
    TEST (1,        "<U0>"),
    TEST (2,        "<U0>@2"),
    TEST (5,        "<U0>@5"),
    TEST (10,       "<U0>@10"),
    TEST (128,      "<U0>@128"),
    TEST (207,      "<U0>@207"),
#if TEST_RW_EXTENSIONS  // not really useful other than making the test slower
    TEST (540,      "<U0>@540"),
    TEST (873,      "<U0>@873"),
    TEST (1412,     "<U0>@1412"),
    TEST (3695,     "<U0>@3695"),
    TEST (4096,     "<U0>@4096")
#endif
};

/**************************************************************************/

// exercises:
// list (size_type, const value_type&)
// list (size_type, const value_type&, const allocator_type&)
#define size_cref_alloc_test_cases size_cref_test_cases
static const ContainerTestCase
size_cref_test_cases [] = {

#undef TEST
#define TEST(size, val, res) {                      \
    __LINE__, -1, size, -1, -1, val,            \
    0, 0, 0, 0,                                 \
    res, sizeof res - 1, 0                      \
}

    //    +---------------------------------------- ctor n argument 
    //    |          +----------------------------- source value
    //    |          |    +------------------------ expected result sequence
    //    |          |    |             
    //    |          |    |            
    //    V          V    V         
    TEST (1,        'a',  "a"), 
    TEST (1,        '\0', "<U0>"),

    TEST (2,        'a',  "aa"), 
    TEST (2,        '\0', "<U0>@2"),

    TEST (5,        '\0', "<U0>@5"),
    TEST (10,       'a',  "aaaaaaaaaa"), 

    TEST (128,      'x',  "x@128"),
    TEST (207,      'x',  "x@207"),
#if TEST_RW_EXTENSIONS  // not really useful other than making the test run
                        // slower
    TEST (540,      'x',  "x@540"),
    TEST (873,      'x',  "x@873"),
    TEST (1412,     'x',  "x@1412"),
    TEST (3695,     'x',  "x@3695"),

    TEST (4096,     'x',  "x@4096"),
#endif
    TEST (4,        't',  "tttt") 
};

/**************************************************************************/
// exercises:
// operator= (const list&)
static const ContainerTestCase
ccont_op_set_test_cases [] = {

#undef TEST
#define TEST(str, arg, res, bthrow) {               \
    __LINE__, -1, -1, -1, -1, -1,               \
    str, sizeof str - 1, arg, sizeof arg - 1,   \
    res, sizeof res - 1, bthrow                 \
}

    //    +----------------------------------------- source sequence
    //    |          +------------------------------ argument sequence
    //    |          |              +--------------- expected result sequence
    //    |          |              |                +- exception info
    //    |          |              |                |    0 - no exception
    //    |          |              |                |   -1 - excpetion safety
    //    |          |              |                |  
    //    |          |              |                |            
    //    V          V              V                V
    TEST ("",       "ab",           "ab",            0),    

    TEST ("",       "",             "",              0),   
    TEST ("abc",    "",             "",              0),   
    TEST ("",       "<U0>",         "<U0>",          0), 
    TEST ("abc",    "<U0>",         "<U0>",          0),

    TEST ("",       "a",            "a",             0),  
    TEST ("<U0>@2", "a",            "a",             0), 
    TEST ("a",      "bcd",          "bcd",           0),       
    TEST ("",       "cdefaihjb",    "cdefaihjb",     0),      
    TEST ("a<U0>b", "cdefaihjb",    "cdefaihjb",     0),

    TEST ("",       "<U0>@2ab",     "<U0>@2ab",      0),  
    TEST ("c<U0>d", "<U0>@2ab",     "<U0>@2ab",      0),
    TEST ("",       "a<U0>@2b",     "a<U0>@2b",      0), 
    TEST ("bcd",    "a<U0>@2b",     "a<U0>@2b",      0), 
    TEST ("<U0>",   "ab<U0>@2",     "ab<U0>@2",      0), 

#if TEST_RW_EXTENSIONS  // some of these tests are too large
                        // to be valuable
    TEST ("x@4096", "bcd",          "bcd",           0), 
    TEST ("x@4096", "ab<U0>@2",     "ab<U0>@2",      0), 
    TEST ("<U0>a",  "abefdcc<U0>a", "abefdcc<U0>a",  0),  
    TEST ("x@4096", "abefdcc<U0>a", "abefdcc<U0>a",  0),

    TEST ("",       "x@207",        "x@207",         0),
    TEST ("x@128",  "x@207",        "x@207",         0),
    TEST ("x@540",  "x@207",        "x@207",         0),
    TEST ("",       "x@1412",       "x@1412",        0),
    TEST ("x@128",  "x@1412",       "x@1412",        0),
    TEST ("x@3695", "x@1412",       "x@1412",        0),
    TEST ("x@872",  "x@873",        "x@873",         0),
    TEST ("x@873",  "x@3695",       "x@3695",        0),

    TEST ("abc",    "x@4096",       "x@4096",        0),   

    TEST ("",           0,          "",              0),
    TEST ("a<U0>b<U0>", 0,          "a<U0>b<U0>",    0), 
    TEST ("x@4096",     0,          "x@4096",        0), 

#else
    TEST ("x@64", "bcd",          "bcd",             0), 
    TEST ("x@64", "ab<U0>@2",     "ab<U0>@2",        0), 
    TEST ("<U0>a",  "abefdcc<U0>a", "abefdcc<U0>a",  0),  
    TEST ("x@64", "abefdcc<U0>a", "abefdcc<U0>a",    0),

    TEST ("",       "x@64",         "x@64",          0),
    TEST ("x@20",   "x@64",         "x@64",          0),
    TEST ("x@127",  "x@64",         "x@64",          0),
    TEST ("x@63",   "x@64",         "x@64",          0),

    TEST ("abc",    "x@64",         "x@64",          0),   

    TEST ("",           0,          "",              0),
    TEST ("a<U0>b<U0>", 0,          "a<U0>b<U0>",    0), 
    TEST ("x@64",       0,          "x@64",          0), 

#endif
    TEST ("",       "last test",    "last test",     0)
};

/**************************************************************************/

// invokes specializations of the member function template
// on the required iterator categories
template <class List, class Iterator>
struct ConsRange: ContRangeBase<List> {

    typedef typename List::value_type     ListVal;
    typedef typename List::allocator_type ListAlloc;

    ConsRange () { }

    virtual List&
    operator() (List&, const ContainerTestCaseData<ListVal>& tdata) const {

        const ListVal* const beg = tdata.arg_ + tdata.off2_;
        const ListVal* const end = beg + tdata.ext2_;

        const Iterator first (beg, beg, end);
        const Iterator last  (end, beg, end);

        if (Cons (range_alloc) == tdata.func_.which_) {
            ListAlloc alloc;
            List* tmp = new List (first, last, alloc);
            return *tmp;
        }

        List* tmp = new List (first, last);
        return *tmp;
    }
};

/**************************************************************************/

// invokes specializations of the member function template
// on the required iterator categories
template <class List, class Iterator>
struct ConsRangePtrOverload: ContRangeBase<List> {

    typedef typename List::value_type     ListVal;
    typedef typename List::allocator_type ListAlloc;

    ConsRangePtrOverload () { }

    virtual List&
    operator() (List&, const ContainerTestCaseData<ListVal>& tdata) const {

        const ListVal* const beg = tdata.arg_ + tdata.off2_;
        const ListVal* const end = beg + tdata.ext2_;

        const Iterator first = _RWSTD_CONST_CAST (Iterator, beg);
        const Iterator last  = _RWSTD_CONST_CAST (Iterator, end);

        if (Cons (range_alloc) == tdata.func_.which_) {
            ListAlloc alloc;
            List* tmp = new List (first, last, alloc);
            return *tmp;
        }

        List* tmp = new List (first, last);
        return *tmp;
    }
};

/**************************************************************************/

template <class InputIterator, class Distance>
inline void
_rw_advance (InputIterator& it, Distance dist) {

    while (0 < dist) {
        --dist;
        ++it;
    }
}

// invokes possible overloads of the member function template
// on common RandomAccessIterator types
template <class List, class Iterator>
struct ConsRangeOverload: ContRangeBase<List>
{
    //typedef typename List::iterator       ListIter;
    typedef typename List::value_type     ListVal;
    typedef typename List::allocator_type ListAlloc;

    ConsRangeOverload () { }

    virtual List&
    operator() (List& list_arg, 
        const ContainerTestCaseData<ListVal>& tdata) const {

        bool reverse_iter =
            ContainerIds::ReverseIterator      == tdata.func_.iter_id_
         || ContainerIds::ConstReverseIterator == tdata.func_.iter_id_;

        const std::size_t off = reverse_iter ? 
            tdata.arglen_ - tdata.off2_ - tdata.ext2_ : tdata.off2_;
        const std::size_t ext = tdata.ext2_;

        Iterator first (this->begin (list_arg, (Iterator*)0));
        _rw_advance (first, off);

        Iterator last (first);
        _rw_advance (last, ext);

        if (Cons (range_alloc) == tdata.func_.which_) {
            ListAlloc alloc;
            List* tmp = new List (first, last, alloc);
            return *tmp;
        }

        List* tmp = new List (first, last);
        return *tmp;
    }
};

/**************************************************************************/

template <class T, class Allocator>
void test_cons (T*, Allocator*, const ContRangeBase<
                std::list <T, Allocator> > &rng,
                const ContainerTestCaseData<T>  &tdata)
{
    typedef std::list <T, Allocator>      List;
    typedef typename List::iterator       ListIter;
    typedef typename List::const_iterator ListCIter;

    const ContainerFunc     &func  = tdata.func_;
    const ContainerTestCase &tcase = tdata.tcase_;

    // construct the argument list 
    /* const */ List arg (tdata.arg_, tdata.arg_ + tdata.arglen_);

    // list function argument
    const List&        arg_list = arg;
    const T            arg_val = T::from_char (char (tcase.val));
    const Allocator    arg_alc;

    // have UserClass copy ctor throw an exception during
    // the copying of the last value
    for (UserClass::copy_ctor_throw_count_ = tdata.reslen_; ;
        ++UserClass::copy_ctor_throw_count_) {

        // (name of) expected and caught exception
        const char* expected = 0;
        const char* caught   = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

        if (0 == tcase.bthrow) {
            // by default exercise the exception safety of the
            // function by iteratively inducing an exception at
            // each call to UserClass::UserClass(const UserClass&)
            // until the call succeeds
            expected = exceptions [3];      // UserClass::CopyCtor
        }
        else {
            // exceptions disabled for this test case
        }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

        if (tcase.bthrow) {
            return;
        }

#endif   // _RWSTD_NO_EXCEPTIONS


        // pointer to the returned reference
        List* ret_ptr = 0;

        // start checking for memory leaks
        rwt_check_leaks (0, 0);

        const bool is_class = ListIds::UserClass == func.elem_id_;

        const std::size_t x_count_save = UserClass::count_;
        const std::size_t arg_count    = std::size_t (tcase.size);

        try {
            ListIds::OverloadId which =
                _RWSTD_STATIC_CAST (ListIds::OverloadId, func.which_);

            // reset function call counters
            if (is_class)
                UserClass::reset_totals ();

            switch (which) {

            case Cons (void):
                ret_ptr = new List ();
                break;

            case Cons (alloc):
                ret_ptr = new List (arg_alc);
                break;

            case Cons (ccont):
                ret_ptr = new List (arg_list);
                break;

            case Cons (size):
                ret_ptr = new List (arg_count);
                break;

            case Cons (size_cref):
                ret_ptr = new List (arg_count, arg_val);
                break;

            case Cons (size_cref_alloc):
                ret_ptr = new List (arg_count, arg_val, arg_alc);
                break;

            case Cons (range):
                ret_ptr = &rng (arg, tdata);
                break;

            case Cons (range_alloc):
                ret_ptr = &rng (arg, tdata);
                break;

            default:
                RW_ASSERT (!"logic error: unknown constructor overload");
            }

            // for convenience
            static const int cwidth = sizeof (T);

            // verify that returned pointer is valid
            rw_assert (0 != ret_ptr, 0, tcase.line,
                       "line %d. %{$FUNCALL} expected \"%{X=*}\", got null",
                       __LINE__, cwidth, int (tdata.reslen_), tdata.res_);

            if (0 != ret_ptr) {

                const std::size_t got_size = ret_ptr->size ();
                char* const got = new char [got_size + 1];

                std::size_t index = 0;
                for (ListCIter it = ret_ptr->begin (),
                    end = ret_ptr->end (); it != end; ++it) {
                    got [index++] = char (it->data_.val_);
                }

                got [got_size] = '\0';

                // verify that the length of the resulting list
                rw_assert (tdata.reslen_ == got_size, 0, tcase.line,
                           "line %d. %{$FUNCALL} expected \"%{X=*}\" with "
                           "length %zu, got %{/*.*Gs} with length %zu",
                           __LINE__,
                           cwidth, int (tdata.reslen_), tdata.res_,
                           tdata.reslen_,
                           1, int (got_size), got, got_size);

                if (tdata.reslen_ == got_size) {
                    // if the result length matches the expected length
                    // (and only then), also verify that the modified
                    // list matches the expected result
                    const std::size_t match =
                        rw_match (tcase.res, got, got_size);

                    rw_assert (match == tdata.reslen_, 0, tcase.line,
                               "line %d. %{$FUNCALL} expected \"%{X=*}\", got "
                               "%{/*.*Gs}, difference at offset %zu",
                               __LINE__,
                               cwidth, int (tdata.reslen_), tdata.res_,
                               1, int (got_size), got, match);
                }

                delete [] got;

                delete ret_ptr;
            }
        }

#ifndef _RWSTD_NO_EXCEPTIONS

        catch (const UserClass::CopyCtor &ex) {
            caught = exceptions [3];
            rw_assert (0 == tcase.bthrow, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                       "unexpectedly%{;} caught %s (id_ = %d)",
                       __LINE__, 0 != expected, expected, caught,
                       ex.id_);
        }
        catch (const std::bad_alloc &ex) {
            caught = exceptions [1];
            rw_assert (0, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                       "unexpectedly%{;} caught std::%s(%#s)",
                       __LINE__, 0 != expected, expected, caught,
                       ex.what ());
        }
        catch (const std::exception &ex) {
            caught = exceptions [2];
            rw_assert (0, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                       "unexpectedly%{;} caught std::%s(%#s)",
                       __LINE__, 0 != expected, expected, caught,
                       ex.what ());
        }
        catch (...) {
            caught = exceptions [0];
            rw_assert (0, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                       "unexpectedly%{;} caught %s",
                       __LINE__, 0 != expected, expected, caught);
        }

#endif   // _RWSTD_NO_EXCEPTIONS

        // disable UserClass::CopyCtor exception
        if (!caught)
            UserClass::copy_ctor_throw_count_ = std::size_t (-1);

        /* const */ std::size_t nbytes;
        const       std::size_t nblocks = rwt_check_leaks (&nbytes, 0);

        // FIXME: verify the number of blocks the function call
        // is expected to allocate and detect any memory leaks
        const std::size_t expect_blocks = nblocks;

        rw_assert (nblocks == expect_blocks, 0, tcase.line,
                   "line %d. %{$FUNCALL} allocated %td bytes in %td blocks",
                   __LINE__, nbytes, expect_blocks);

        if (is_class)
            rw_assert (x_count_save == UserClass::count_, 0, tcase.line,
                       "line %d. %{$FUNCALL} leaked %zu value(s) "
                       "of %zu after %{?}an exception %s%{:}call%{;}",
                       __LINE__, UserClass::count_ - x_count_save,
                       tdata.reslen_, 0 != caught, caught);

        if (caught) {
            if (0 == tcase.bthrow) {
                // allow this call to operator new to succeed and try
                // to make the next one to fail during the next call
                // to the same function again
                continue;
            }
        }
        else if (0 < tcase.bthrow) {
            rw_assert (caught == expected, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s, caught %s"
                       "%{:}unexpectedly caught %s%{;}",
                       __LINE__, 0 != expected, expected, caught, caught);
        }

        break;
    }
}

/**************************************************************************/

template <class T, class Allocator>
void test_op_set (T*, Allocator*,
                  const ContainerTestCaseData<T> &tdata)
{
    typedef std::list <T, Allocator>      List;
    typedef typename List::iterator       ListIter;
    typedef typename List::const_iterator ListCIter;

    const ContainerFunc     &func  = tdata.func_;
    const ContainerTestCase &tcase = tdata.tcase_;

    // construct the list object to be modified
    // and the (possibly unused) argument list
          List lst (tdata.str_, tdata.str_ + tdata.strlen_, Allocator ());
    const List arg (tdata.arg_, tdata.arg_ + tdata.arglen_, Allocator ());

    // list function argument
    const List&  arg_list = tcase.arg ? arg : lst;

    rwt_free_store* const pst = rwt_get_free_store (0);
    SharedAlloc*    const pal = SharedAlloc::instance ();

    // iterate for`throw_after' starting at the next call to operator new,
    // forcing each call to throw an exception, until the function finally
    // succeeds (i.e, no exception is thrown)
    std::size_t throw_count;
    for (throw_count = 0; ; ++throw_count) {

        // (name of) expected and caught exception
        const char* expected = 0;
        const char* caught   = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

        if (0 == tcase.bthrow) {
            // by default exercise the exception safety of the
            // function by iteratively inducing an exception at
            // each call to operator new or Allocator::allocate()
            // until the call succeeds
            expected = exceptions [1];      // bad_alloc
            *pst->throw_at_calls_ [0] =
                pst->new_calls_ [0] + throw_count + 1;
            pal->throw_at_calls_ [pal->m_allocate] =
                pal->throw_at_calls_ [pal->m_allocate] + throw_count + 1;
        }
        else {
            // exceptions disabled for this test case
        }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

        if (tcase.bthrow) {
            return;
        }

#endif   // _RWSTD_NO_EXCEPTIONS

        // start checking for memory leaks
        rw_check_leaks (lst.get_allocator ());

        try {
            const bool is_class = ListIds::UserClass == func.elem_id_;

            // reset function call counters
            if (is_class)
                UserClass::reset_totals ();

            ListIds::OverloadId which =
                _RWSTD_STATIC_CAST (ListIds::OverloadId, func.which_);

            switch (which) {

            case OpSet (ccont):
                lst = arg_list;
                break;

            default:
                RW_ASSERT (!"logic error: unknown operator= overload");
            }

            if (is_class && &lst != &arg_list) {

                const std::size_t total_copy = UserClass::n_total_copy_ctor_ +
                                               UserClass::n_total_op_assign_;

                bool success = 0 == UserClass::n_total_def_ctor_
                            && total_copy == tdata.reslen_;

                rw_assert (success, 0, tcase.line,
                           "line %d. %{$FUNCALL} called default ctor and "
                           "copy ctor + operator=() %zu, and %zu times, "
                           "respectively, 0, and %zu expected",
                           __LINE__,
                           UserClass::n_total_def_ctor_,
                           total_copy, tdata.reslen_);
            }

            // for convenience
            static const int cwidth = sizeof (T);

            const std::size_t got_size = lst.size ();
            char* const got = new char [got_size + 1];

            std::size_t index = 0;
            for (ListCIter it = lst.begin (),
                end = lst.end (); it != end; ++it) {
                got [index++] = char (it->data_.val_);
            }

            got [got_size] = '\0';

            // verify that the length of the resulting list
            rw_assert (tdata.reslen_ == got_size, 0, tcase.line,
                       "line %d. %{$FUNCALL} expected \"%{X=*}\" with "
                       "length %zu, got %{/*.*Gs} with length %zu",
                       __LINE__,
                       cwidth, int (tdata.reslen_), tdata.res_, 
                       tdata.reslen_, 1, int (got_size), got, got_size);

            if (tdata.reslen_ == got_size) {
                // if the result length matches the expected length
                // (and only then), also verify that the modified
                // list matches the expected result
                const std::size_t match =
                    rw_match (tcase.res, got, got_size);

                rw_assert (match == tdata.reslen_, 0, tcase.line,
                           "line %d. %{$FUNCALL} expected \"%{X=*}\", "
                           "got %{/*.*Gs}, difference at offset %zu",
                           __LINE__, cwidth, int (tdata.reslen_),
                           tdata.res_, 1, int (got_size), got, match);
            }

            delete [] got;
        }

#ifndef _RWSTD_NO_EXCEPTIONS

        catch (const std::bad_alloc &ex) {
            caught = exceptions [1];
            rw_assert (0 == tcase.bthrow, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                       "unexpectedly%{;} caught std::%s(%#s)",
                       __LINE__, 0 != expected, expected, caught,
                       ex.what ());
        }
        catch (const std::exception &ex) {
            caught = exceptions [2];
            rw_assert (0, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                       "unexpectedly%{;} caught std::%s(%#s)",
                       __LINE__, 0 != expected, expected, caught,
                       ex.what ());
        }
        catch (...) {
            caught = exceptions [0];
            rw_assert (0, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                       "unexpectedly%{;} caught %s",
                       __LINE__, 0 != expected, expected, caught);
        }

#endif   // _RWSTD_NO_EXCEPTIONS

        // FIXME: verify the number of blocks the function call
        // is expected to allocate and detect any memory leaks
        rw_check_leaks (lst.get_allocator (), tcase.line,
                        std::size_t (-1), std::size_t (-1));

        if (caught) {
            if (0 == tcase.bthrow) {
                // allow this call to operator new to succeed and try
                // to make the next one to fail during the next call
                // to the same function again
                continue;
            }
        }
        else if (0 < tcase.bthrow) {
            rw_assert (caught == expected, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s, caught %s"
                       "%{:}unexpectedly caught %s%{;}",
                       __LINE__, 0 != expected, expected, caught, caught);
        }

        break;
    }

    // disable bad_alloc exceptions
    *pst->throw_at_calls_ [0] = 0;
    pal->throw_at_calls_ [pal->m_allocate] = 0;
}

/**************************************************************************/

template <class T, class Allocator>
void test_cons (T*, Allocator*,
                const ContainerTestCaseData<T> &tdata)
{
    typedef std::list<T, Allocator> List;

    if (ContainerIds::fid_op_set == (tdata.func_.which_ & ContainerIds::fid_mask)) {
        test_op_set ((T*)0, (Allocator*)0, tdata);
        return;
    }

    if (tdata.func_.which_ == Cons (range) 
     || tdata.func_.which_ == Cons (range_alloc)) {

        switch (tdata.func_.iter_id_) {

        // exercise specializations of the member function template
        // on the required iterator categories

#undef TEST
#define TEST(Iterator) do {                             \
    typedef Iterator<T> Iter;                           \
    static const                                        \
    ConsRange<List, Iter> rng;                          \
    test_cons ((T*)0, (Allocator*)0, rng, tdata);       \
} while (0)

        case ContainerIds::Input: TEST (InputIter); break;
        case ContainerIds::Forward: TEST (ConstFwdIter); break;
        case ContainerIds::Bidir: TEST (ConstBidirIter); break;
        case ContainerIds::Random: TEST (ConstRandomAccessIter); break;

            // exercise possible overloads of the member function template
            // on pointer and const_pointer types
#undef TEST
#define TEST(Iterator) do {                             \
    typedef typename List::Iterator Iter;               \
    static const                                        \
    ConsRangePtrOverload<List, Iter> rng;               \
    test_cons ((T*)0, (Allocator*)0, rng, tdata);       \
} while (0)

        case ContainerIds::Pointer: TEST (pointer); break;
        case ContainerIds::ConstPointer: TEST (const_pointer); break;

            // exercise possible overloads of the member function template
            // on common RandomAccessIterator types except
            // pointer and const_pointer types
#undef TEST
#define TEST(Iterator) do {                             \
    typedef typename List::Iterator Iter;               \
    static const                                        \
    ConsRangeOverload<List, Iter> rng;                  \
    test_cons ((T*)0, (Allocator*)0, rng, tdata);       \
} while (0)

        case ContainerIds::Iterator: TEST (iterator); break;
        case ContainerIds::ConstIterator: TEST (const_iterator); break;

        case ContainerIds::ReverseIterator:
            TEST (reverse_iterator);
            break;
        case ContainerIds::ConstReverseIterator:
            TEST (const_reverse_iterator);
            break;

        default:
            rw_error (0, 0, __LINE__, "bad iterator id");
        }
    }
    else {
        // exercise ordinary overloads of the member function
        static const ContRangeBase<List> rng;
        test_cons ((T*)0, (Allocator*)0, rng, tdata);
    }
}

/**************************************************************************/

DEFINE_CONTAINER_TEST_FUNCTIONS (test_cons);

int main (int argc, char** argv)
{
    std::list<int> lst;

    static const ContainerTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        Cons (sig), sig ## _test_cases,                         \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (void),
        TEST (alloc),
        TEST (ccont),
        TEST (size),
        TEST (size_cref),
        TEST (size_cref_alloc),
        TEST (range),
        TEST (range_alloc),

#undef TEST
#define TEST(sig) {                             \
        OpSet (sig), sig ## _op_set_test_cases, \
          sizeof sig ## _op_set_test_cases      \
        / sizeof *sig ## _op_set_test_cases     \
    }

        TEST (ccont)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    const int status =
        rw_run_cont_test (argc, argv, __FILE__,
                          "lib.list.cons",
                          ContainerIds::List,
                          test_cons_func_array, tests, test_count);

    return status;
}
