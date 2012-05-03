/************************************************************************
*
* 23.containers.h - definitions of helpers used in clause 23 tests
*
* $Id: 23.containers.h$
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

#ifndef RW_23_CONTAINERS_H_INCLUDED
#define RW_23_CONTAINERS_H_INCLUDED

#include <testdefs.h>
#include <rw_value.h>       // for UserClass, UserPOD
#include <rw_char.h>        // for rw_expand ()

/**************************************************************************/

// defines enumerations identifying the general template arguments,
// sets of overloaded functions, member types used in the declarations
// of their signatures, and specific overloads of such member functions
struct ContainerIds {

    // identifiers for the T template argument
    enum ElemId { UserPOD, UserClass };

    // identifiers for the Allocator template argument
    enum AllocId { DefaultAlloc, UserAlloc };

    // identifiers for the Iterator template argument
    // used with member templates
    enum IteratorId {
        None,
        Input, Forward, Bidir, Random,
        Pointer, ConstPointer,
        Iterator, ConstIterator,
        ReverseIterator, ConstReverseIterator
    };

    enum ContainerId {
        List, Vector, Deque, Queue, Stack
    };

    // identifies a set of overloaded member or non-member
    // container functions
    enum FuncId {
        // 6 bits, 64 functions max
        // common
        /*  0 */ fid_ctor,
        /*  1 */ fid_op_set,
        /*  2 */ fid_assign,
        /*  3 */ fid_get_allocator,
        /*  4 */ fid_begin,
        /*    */ fid_begin_const = fid_begin,
        /*  5 */ fid_end,
        /*    */ fid_end_const = fid_end,
        /*  6 */ fid_rbegin,
        /*    */ fid_rbegin_const = fid_rbegin,
        /*  7 */ fid_rend,
        /*    */ fid_rend_const = fid_rend,
        /*  8 */ fid_empty,
        /*  9 */ fid_size,
        /* 10 */ fid_max_size,
        /* 11 */ fid_resize,
        /* 12 */ fid_insert,
        /* 13 */ fid_erase,
        /* 14 */ fid_swap,
        /* 15 */ fid_clear,
        /* 16 */ fid_op_equal,
        /* 17 */ fid_op_less,
        /* 18 */ fid_op_not_equal,
        /* 19 */ fid_op_greater,
        /* 20 */ fid_op_greater_equal,
        /* 21 */ fid_op_less_equal,
        /* 22 */ fid_push_back,

        // list, deque, vector
        /* 23 */ fid_front,
        /*    */ fid_front_const = fid_front,
        /* 24 */ fid_back,
        /*    */ fid_back_const = fid_back,
        /* 25 */ fid_pop_back,

        // list, deque
        /* 26 */ fid_push_front,
        /* 27 */ fid_pop_front,

        // list
        /* 28 */ fid_splice,
        /* 29 */ fid_remove,
        /* 30 */ fid_remove_if,
        /* 31 */ fid_unique,
        /* 32 */ fid_merge,
        /* 33 */ fid_sort,
        /* 34 */ fid_reverse,

        // vector, string, deque
        /* 35 */ fid_op_index,
        /*    */ fid_op_index_const = fid_op_index,
        /* 36 */ fid_at,
        /*    */ fid_at_const = fid_at,

        // vector, string
        /* 37 */ fid_capacity,
        /* 38 */ fid_reserve,

        // vector<bool>
        /* 39 */ fid_flip,

        /* -- */ fid_bits = 6,
        /* -- */ fid_mask = 63
    };

    // identifies the type of a function argument, including
    // the implicit this
    enum ArgId {
        // 4 bits, 16 types max
        /*  0 */ arg_void,    // void
        /*  1 */ arg_size,    // size_type
        /*  2 */ arg_val,     // value_type
        /*  3 */ arg_ref,     // reference
        /*  4 */ arg_cref,    // const_reference
        /*  5 */ arg_iter,    // iterator
        /*  6 */ arg_citer,   // const_iterator
        /*  7 */ arg_range,   // Iterator, Iterator
        /*  8 */ arg_cont,    // container& (or this for member functions)
        /*  9 */ arg_ccont,   // const container& (or const this for members)
        /* 10 */ arg_alloc,   // const allocator&
        /* 11 */ arg_pred,    // Predicate
        /* 12 */ arg_bpred,   // BinaryPredicate
        /* 13 */ arg_comp,    // Compare
        /* -- */ arg_bits = 4,
        /* -- */ arg_mask = 15
    };

    enum {
        // bit designating a member function
        bit_member = 1 << (fid_bits + 6 * arg_bits)
    };

    static ArgId arg_type (_RWSTD_SIZE_T id, int argno) {
        return ArgId (((id >> fid_bits) >> argno * arg_bits) & arg_mask);
    }
};

/**************************************************************************/

struct ContainerFunc
{
    ContainerIds::ElemId      elem_id_;
    ContainerIds::AllocId     alloc_id_;
    ContainerIds::IteratorId  iter_id_;
    ContainerIds::ContainerId cont_id_;
    _RWSTD_SIZE_T             which_;
};


// describes a single test case for any overload of any container
// function (the same test case can be used to exercise more
// than one overload of the same function)
struct ContainerTestCase
{
    int            line;      // test case line number

    int            off;       // offset (position argument)
    int            size;      // size (count argument)

    int            off2;      // offset 2 (position argument)
    int            size2;     // size 2 (count argument)

    int            val;       // value (single character to append)

    const char*    str;       // controlled sequence
    _RWSTD_SIZE_T  str_len;   // length of sequence

    const char*    arg;       // sequence to insert
    _RWSTD_SIZE_T  arg_len;   // length of sequence

    const char*    res;       // resulting sequence
    _RWSTD_SIZE_T  nres;      // length of sequence or expected result value

    int            bthrow;    // exception expected
};


// describes a set of test cases for a single overload of a function
struct ContainerTest
{
    // container function overload to exercise
    _RWSTD_SIZE_T which;

    // test cases to exercise overload with
    const ContainerTestCase *cases;

    // number of test cases
    _RWSTD_SIZE_T case_count;
};


typedef void ContainerTestFunc (const ContainerFunc&, const ContainerTestCase&);

_TEST_EXPORT int
rw_run_cont_test (int, char**, const char*, const char*,
                  ContainerIds::ContainerId, ContainerTestFunc*,
                  const ContainerTest*, _RWSTD_SIZE_T);

typedef void VoidFunc ();

_TEST_EXPORT int
rw_run_cont_test (int, char**, const char*, const char*,
                  ContainerIds::ContainerId, VoidFunc* const*,
                  const ContainerTest*, _RWSTD_SIZE_T);

/**************************************************************************/

template <class T>
class ContainerTestCaseData
{
private:

    // not defined, not copyable, not assignable
    ContainerTestCaseData (const ContainerTestCaseData&);
    void operator= (const ContainerTestCaseData&);

    // for convenience
    typedef _RWSTD_SIZE_T SizeType;

public:

    SizeType strlen_;   // the length of the expanded string
    SizeType arglen_;   // the length of the expanded argument
    SizeType reslen_;   // the length of the expanded result

    // the offset and extent (the number of elements) of
    // the first range into the container object being modified
    SizeType off1_;
    SizeType ext1_;

    // the offset and extent (the number of elements) of
    // the argument of the function call
    SizeType off2_;
    SizeType ext2_;

    const T* str_;   // pointer to the expanded string
    const T* arg_;   // pointer to the expanded argument
    const T* res_;   // pointer to the expanded result

    const ContainerFunc     &func_;
    const ContainerTestCase &tcase_;

    // converts the narrow (and possibly) condensed strings to fully
    // expanded wide character arrays that can be used to construct
    // container objects
    ContainerTestCaseData (const ContainerFunc&,
                           const ContainerTestCase&);

    ~ContainerTestCaseData ();
};


template <class T>
ContainerTestCaseData<T>::
ContainerTestCaseData (const ContainerFunc     &func,
                       const ContainerTestCase &tcase)
    : func_ (func), tcase_ (tcase)
{
    char buf [256];

    strlen_ = sizeof (buf);
    char* str = rw_expand (buf, tcase.str, tcase.str_len, &strlen_);
    str_ = T::from_char (str, strlen_);
    if (str != buf)
        delete[] str;

    arglen_ = sizeof (buf);
    str = rw_expand (buf, tcase.arg, tcase.arg_len, &arglen_);
    arg_ = T::from_char (str, arglen_);
    if (str != buf)
        delete[] str;

    reslen_ = sizeof (buf);
    str = rw_expand (buf, tcase.res, tcase.nres, &reslen_);
    res_ = T::from_char (str, reslen_);
    if (str != buf)
        delete[] str;

    // compute the offset and extent of the container object
    // representing the controlled sequence and the offset
    // and extent of the argument of the function call
    const SizeType argl = tcase_.arg ? arglen_ : strlen_;

    off1_ = SizeType (tcase_.off) < strlen_ ?
        SizeType (tcase_.off) : strlen_;

    ext1_ = off1_ + tcase_.size < strlen_ ?
        SizeType (tcase_.size) : strlen_ - off1_;

    off2_ = SizeType (tcase_.off2) < argl ?
        SizeType (tcase_.off2) : argl;

    ext2_ = off2_ + tcase_.size2 < argl ?
        SizeType (tcase_.size2) : argl - off2_;
}


template <class T>
ContainerTestCaseData<T>::
~ContainerTestCaseData ()
{
    // clean up dynamically allocated memory

    // cast away the constness of the pointers to work around
    // an HP aCC 6.16 and prior bug described in STDCXX-802
    delete[] _RWSTD_CONST_CAST (T*, str_);
    delete[] _RWSTD_CONST_CAST (T*, arg_);
    delete[] _RWSTD_CONST_CAST (T*, res_);
}

/**************************************************************************/

// base class-functor for the range template overloads testing
template <class Cont>
struct ContRangeBase {

    typedef typename Cont::value_type                 ContVal;
    typedef typename Cont::iterator                   ContIter;
    typedef typename Cont::const_iterator             ContConstIter;
    typedef typename Cont::reverse_iterator           ContRevIter;
    typedef typename Cont::const_reverse_iterator     ContConstRevIter;

    ContRangeBase () { }

    virtual ~ContRangeBase () { /* silence warnings */ }

    static ContIter
    begin (Cont &cont, ContIter*) {
        return cont.begin ();
    }

    static ContConstIter
    begin (const Cont &cont, ContConstIter*) {
        return cont.begin ();
    }

    static ContRevIter
    begin (Cont &cont, ContRevIter*) {
        return cont.rbegin ();
    }

    static ContConstRevIter
    begin (const Cont &cont, ContConstRevIter*) {
        return cont.rbegin ();
    }

    virtual Cont&
    operator() (Cont &cont, const ContainerTestCaseData<ContVal>&) const {
        RW_ASSERT (!"logic error: should be never called");
        return cont;
    }
};

/**************************************************************************/

#  define CONTAINER_TEST_DISPATCH(Alloc, fname, func, tcase)    \
    if (ContainerIds::UserPOD == func.elem_id_)                 \
        fname (UserPOD (), (Alloc<UserPOD>*)0, func, tcase);    \
    else                                                        \
        fname (UserClass (), (Alloc<UserClass>*)0, func, tcase)


#define DEFINE_CONTAINER_TEST_DISPATCH(fname)                             \
    static void                                                           \
    fname (const ContainerFunc     &func,                                 \
           const ContainerTestCase &tcase) {                              \
        if (ContainerIds::DefaultAlloc == func.alloc_id_) {               \
            CONTAINER_TEST_DISPATCH (std::allocator, fname, func, tcase); \
        }                                                                 \
        else if (ContainerIds::UserAlloc == func.alloc_id_) {             \
            CONTAINER_TEST_DISPATCH (UserAlloc, fname, func, tcase);      \
        }                                                                 \
        else                                                              \
            RW_ASSERT (!"logic error: bad allocator");                    \
    } typedef void rw_unused_typedef


#define CONTAINER_TFUNC(T, Allocator)         \
    void (*)(T*, Allocator<T>*,               \
             const ContainerTestCaseData<T>&)

#define CONTAINER_TFUNC_ADDR(fname, T, Allocator) \
    (VoidFunc*)(CONTAINER_TFUNC (T, Allocator))   \
        &fname<T, Allocator<T> >

#define DEFINE_CONTAINER_TEST_FUNCTIONS(fname)                 \
    static VoidFunc* const fname ## _func_array [] = {         \
      CONTAINER_TFUNC_ADDR (fname, UserPOD, std::allocator),   \
      CONTAINER_TFUNC_ADDR (fname, UserPOD, UserAlloc),        \
                                                               \
      CONTAINER_TFUNC_ADDR (fname, UserClass, std::allocator), \
      CONTAINER_TFUNC_ADDR (fname, UserClass, UserAlloc)       \
    }

#endif   // RW_23_CONTAINERS_H_INCLUDED
