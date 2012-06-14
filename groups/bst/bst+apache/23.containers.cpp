/************************************************************************
*
* 23.containers.cpp - definitions of helpers used in clause 23 tests
*
* $Id: 23.containers.cpp
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

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <memory>           // for allocator

#include <23.containers.h>
#include <23.list.h>

#include <cmdopt.h>         // for rw_enabled()
#include <driver.h>         // for rw_note()
#include <rw_allocator.h>   // for UserAlloc
#include <rw_printf.h>      // for rw_asnprintf()

#include <ctype.h>          // for isdigit()
#include <stdarg.h>         // for va_arg, ...
#include <stddef.h>         // for size_t
#include <stdlib.h>         // for free()
#include <string.h>         // for strcpy()

/**************************************************************************/

static const char
_rw_this_file[] = __FILE__;


static const char* const
_rw_elem_names[] = {
    "UserPOD", "UserClass"
};


static const char* const
_rw_alloc_names[] = {
    "allocator", "UserAlloc"
};


static const char* const
_rw_iter_names[] = {
    "",
    "InputIterator", "ForwardIterator", "BidirectionalIterator",
    "RandomAccessIterator",
    "pointer", "const_pointer",
    "iterator", "const_iterator",
    "reverse_iterator", "const_reverse_iterator"
};


static const char* const
_rw_cont_names[] = {
    "list", "vector", "deque", "queue", "stack"
};


// order of elements depends on the values of ContainerIds::FuncId
static const char* const
_rw_func_names[] = {
    0 /* special handling for the ctor */, "operator=", "assign",
    "get_allocator", "begin", "end", "rbegin", "rend", "empty",
    "size", "max_size", "resize", "insert", "erase", "swap",
    "clear", "operator==", "operator<", "operator!=", "operator>",
    "operator>=", "operator<=", "push_back", "front", "back",
    "pop_back", "push_front", "pop_front", "splice", "remove",
    "remove_if", "unique", "merge", "sort", "reverse", "operator[]",
    "at", "capacity", "reserve", "flip"
};

/**************************************************************************/

const size_t MAX_OVERLOADS = 32;

// disabled (-1) or explicitly enabled (+1) for each overload
// of the cont function being tested
static int
_rw_opt_func [MAX_OVERLOADS];

// array of tests each exercising a single cont function
static const ContainerTest*
_rw_cont_tests;

// size of the array above
static size_t
_rw_cont_test_count;

static int
_rw_opt_elem_types [sizeof _rw_elem_names / sizeof *_rw_elem_names];

static int
_rw_opt_alloc_types [sizeof _rw_alloc_names / sizeof *_rw_alloc_names];

static int
_rw_opt_iter_types [sizeof _rw_iter_names / sizeof *_rw_iter_names];

static int
_rw_opt_no_exceptions;

static int
_rw_opt_no_exception_safety;

static int
_rw_opt_self_ref;

/**************************************************************************/

static size_t
_rw_get_func_inx (size_t fid)
{
    size_t inx = _RWSTD_SIZE_MAX;

    for (size_t i = 0; _rw_cont_test_count; ++i) {
        if (fid == _rw_cont_tests [i].which) {
            inx = i;
            break;
        }
    }

    RW_ASSERT (inx < _RWSTD_SIZE_MAX);

    return inx;
}

/**************************************************************************/

static const char*
_rw_class_name (const ContainerFunc & func)
{
    return _rw_cont_names [func.cont_id_];
}

/**************************************************************************/

// appends the signature of the function specified by which
// to the provided buffer; when the second argument is null,
// appends the mnemonic representing the signature, including
// the name of the function, as specified by the third argument
static void
_rw_sigcat (char **pbuf, size_t *pbufsize,
            const ContainerFunc *func,
            size_t               which = 0)
{
    // for convenience
    typedef ContainerIds Ids;

    if (func)
        which = func->which_;

    // determine whether the function is a member function
    const bool is_member = 0 != (Ids::bit_member & which);

    // get the bitmap describing the function's argument types
    int argmap = (which & ~Ids::bit_member) >> Ids::fid_bits;

    // determine whether the function is a const member function
    bool is_const_member =
        is_member && Ids::arg_ccont == (argmap & Ids::arg_mask);

    // remove the *this argument if the function is a member
    if (is_member)
        argmap >>= Ids::arg_bits;

    const char* funcname = 0;

    if (0 == func) {
        const Ids::FuncId fid = Ids::FuncId (which & ContainerIds::fid_mask);

        switch (fid) {
            // translate names with funky characters to mnemonics
        case Ids::fid_ctor:          funcname = "ctor"; break;
        case Ids::fid_op_index:      funcname = "op_index"; break;
        case Ids::fid_op_set:        funcname = "op_assign"; break;
        case Ids::fid_op_equal:      funcname = "op_equal"; break;
        case Ids::fid_op_less:       funcname = "op_less"; break;
        case Ids::fid_op_not_equal:  funcname = "op_not_equal"; break;
        case Ids::fid_op_greater:    funcname = "op_greater"; break;
        case Ids::fid_op_greater_equal: funcname = "op_greater_equal"; break;
        case Ids::fid_op_less_equal: funcname = "op_less_equal"; break;

        case Ids::fid_get_allocator:
        case Ids::fid_empty:
        case Ids::fid_size:
        case Ids::fid_max_size:
            // prevent appending the "_const" bit to the mnemonics
            // of member functions not overloaded on const
            is_const_member = false;

            // fall through

        default: {
            // determine the cont function name (for brief output)
            const size_t nfuncs =
                sizeof _rw_func_names / sizeof *_rw_func_names;

            RW_ASSERT (size_t (fid) < nfuncs);

            funcname = _rw_func_names [fid];
            RW_ASSERT (0 != funcname);
            break;
        }
        }
    }

    // iterator name for member templates, empty cont for other functions
    const char* const iname = func ? _rw_iter_names [func->iter_id_] : "";

    rw_asnprintf (pbuf, pbufsize,
        "%{+}%{?}%s%{?}_const%{;}%{:}%{?}<%s>%{;}(%{;}",
        0 == func, funcname, is_const_member, 0 != *iname, iname);

    char iname_buf [80];
    *iname_buf = '\0';

    // iterate through the map of argument types one field at a time
    // determining and formatting the type of each argument until
    // void is reached
    for (size_t argno = 0; argmap; ++argno, argmap >>= Ids::arg_bits) {

        const char* pfx = "";
        const char* sfx = "";

        const int argtype = argmap & Ids::arg_mask;

        const char* tname = 0;

        if (func) {
            switch (argtype) {
            case Ids::arg_size:  tname = "size_type"; break;
            case Ids::arg_val:   tname = "value_type"; break;
            case Ids::arg_ref:   tname = "reference"; break;
            case Ids::arg_cref:  tname = "const_reference"; break;
            case Ids::arg_iter:  tname = "iterator"; break;
            case Ids::arg_citer: tname = "const_iterator"; break;
            case Ids::arg_range:
                if ('\0' == *iname_buf) {
                    strcpy (iname_buf, iname);
                    strcat (iname_buf, ", ");
                    strcat (iname_buf, iname);
                }
                tname = iname_buf;
                break;

            case Ids::arg_ccont:
                pfx   = "const ";
                // fall through
            case Ids::arg_cont:
                tname = _rw_class_name (*func);
                sfx   = "&";
                break;

            case Ids::arg_alloc: tname = "const allocator_type&"; break;
            case Ids::arg_pred:  tname = "Predicate"; break;
            case Ids::arg_bpred: tname = "BinaryPredicate"; break;
            case Ids::arg_comp:  tname = "Compare"; break;
            }
        }
        else {
            switch (argtype) {
            case Ids::arg_size:  tname = "size"; break;
            case Ids::arg_val:   tname = "val"; break;
            case Ids::arg_ref:   tname = "ref"; break;
            case Ids::arg_cref:  tname = "cref"; break;
            case Ids::arg_iter:  tname = "iter"; break;
            case Ids::arg_citer: tname = "citer"; break;
            case Ids::arg_range: tname = "range"; break;
            case Ids::arg_cont:  tname = "cont"; break;
            case Ids::arg_ccont: tname = "ccont"; break;
            case Ids::arg_alloc: tname = "alloc"; break;
            case Ids::arg_pred:  tname = "pred"; break;
            case Ids::arg_bpred: tname = "bpred"; break;
            case Ids::arg_comp:  tname = "comp"; break;
            }
        }

        RW_ASSERT (0 != tname);

        if (   0 == func || is_member
            || Ids::arg_cont != argtype && Ids::arg_ccont != argtype) {
                // append the name or mnemonic of the argument type
                rw_asnprintf (pbuf, pbufsize, "%{+}%{?}_%{:}%{?}, %{;}%{;}%s%s%s",
                    0 == func, 0 < argno, pfx, tname, sfx);
            }
        else {
            // in non-member functions use ${CLASS} to format
            // the cont argument in order to expand
            // its template argument cont
            rw_asnprintf (pbuf, pbufsize,
                "%{+}%{?}, %{;}%{?}const %{;}%{$CLASS}&",
                0 < argno, Ids::arg_ccont == argtype);

        }
    }

    if (func)
        rw_asnprintf (pbuf, pbufsize, "%{+})%{?} const%{;}", is_const_member);
}

/**************************************************************************/

// returns the zero-based index of the argument type specified
// by arg in the function signature given by which
static int
_rw_argno (size_t which, int arg)
{
    // get the bitmap describing the function's argument types
    int argmap = (which & ~ContainerIds::bit_member) >> ContainerIds::fid_bits;

    int argno = 0;

    // iterate over argument types looking for the first one
    // that equals arg
    for (; argmap; argmap >>= ContainerIds::arg_bits, ++argno) {
        if ((argmap & ContainerIds::arg_mask) == arg) {
            return argno;
        }
    }

    // return -1 when the function doesn't take an argument
    // of the specified type
    return -1;
}

/**************************************************************************/

static void
_rw_list_sigcat (char** pbuf, size_t * pbufsize, ListIds::OverloadId which,
                 bool self, const char* str, size_t str_len,
                 const char* arg, size_t arg_len,
                 const ContainerTestCase &tcase)
{
    // compute the end offsets for convenience
    const size_t range1_end = tcase.off + tcase.size;
    const size_t range2_end = tcase.off2 + tcase.size2;

    // determine whether the function takes an allocator_type argument
    const bool use_alloc = 0 < _rw_argno (which, ContainerIds::arg_alloc);

    // format and append cont function arguments abbreviating complex
    // expressions as much as possible to make them easy to understand
    switch (which) {
    case ListIds::ctor_ccont:
    case ListIds::op_set_ccont:
    case ListIds::swap_cont:
    case ListIds::merge_cont:
        // format self-referential cont argument as *this
        rw_asnprintf (pbuf, pbufsize,
                      "%{+} (%{?}*this%{:}%{$Container}(%{#*s})%{;}"
                      "%{?}, const allocator_type&%{;})",
                      self, int (arg_len), arg, use_alloc);
        break;

    case ListIds::assign_size_cref:
    case ListIds::ctor_size_cref:
    case ListIds::ctor_size_cref_alloc:
        rw_asnprintf (pbuf, pbufsize,
                      "%{+} (%zu, %{#c}%{?}, const allocator_type&%{;})",
                      tcase.size, tcase.val, use_alloc);
        break;

    case ListIds::assign_range:
    case ListIds::ctor_range:
    case ListIds::ctor_range_alloc:
        rw_asnprintf (pbuf, pbufsize, "%{+}<%{$Iterator:-Iterator}>("
                      "%{?}begin()%{:}%{$Iterator:-Iterator}(%{#*s})%{;}"
                      "%{?} + %zu%{;}, "
                      "%{?}begin()%{:}%{$Iterator:-Iterator}(...)%{;}"
                      "%{?} + %zu%{;}"
                      "%{?}, const allocator_type&%{;})",
                      self, int (arg_len), arg,
                      0 != tcase.off2, tcase.off2,
                      self, 0 != range2_end, range2_end, use_alloc);
        break;

    case ListIds::insert_iter_cref:
        rw_asnprintf (pbuf, pbufsize,
                      "%{+} (begin()%{?} + %zu%{;}, %{#c})",
                      0 != tcase.off, tcase.off, tcase.val);
        break;

    case ListIds::insert_iter_size_cref:
        rw_asnprintf (pbuf, pbufsize,
                      "%{+} (begin()%{?} + %zu%{;}, %zu, %{#c})",
                      0 != tcase.off, tcase.off, tcase.size, tcase.val);
        break;

    case ListIds::insert_iter_range:
        rw_asnprintf (pbuf, pbufsize, "%{+}<%{$Iterator:-Iterator}>"
                      "(begin()%{?} + %zu%{;}, "
                      "%{?}begin()%{:}%{$Iterator:-Iterator}(%{#*s})%{;}"
                      "%{?} + %zu%{;}, "
                      "%{?}begin()%{:}%{$Iterator:-Iterator}(...)%{;}"
                      "%{?} + %zu%{;})",
                      0 != tcase.off, tcase.off,
                      self, int (arg_len), arg,
                      0 != tcase.off2, tcase.off2,
                      self, 0 != range2_end, range2_end);
        break;

    case ListIds::push_front_cref:
    case ListIds::push_back_cref:
    case ListIds::remove_cref:
        rw_asnprintf (pbuf, pbufsize,
                      "%{+} (%{#c})", tcase.val);
        break;

    case ListIds::ctor_void:
    case ListIds::get_allocator_void:
    case ListIds::begin_void:
    case ListIds::end_void:
    case ListIds::rbegin_void:
    case ListIds::rend_void:
    case ListIds::empty_void:
    case ListIds::size_void:
    case ListIds::max_size_void:
    case ListIds::front_void:
    case ListIds::back_void:
    case ListIds::pop_front_void:
    case ListIds::pop_back_void:
    case ListIds::clear_void:
    case ListIds::sort_void:
    case ListIds::reverse_void:
        rw_asnprintf (pbuf, pbufsize,
                      "%{+} ()");
        break;

    case ListIds::begin_const_void:
    case ListIds::end_const_void:
    case ListIds::rbegin_const_void:
    case ListIds::rend_const_void:
    case ListIds::front_const_void:
    case ListIds::back_const_void:
        rw_asnprintf (pbuf, pbufsize,
                      "%{+} () const");
        break;

    case ListIds::ctor_alloc:
        rw_asnprintf (pbuf, pbufsize,
                      "%{+} (const allocator_type&)");
        break;

    case ListIds::erase_iter:
        rw_asnprintf (pbuf, pbufsize,
                      "%{+} (begin()%{?} + %zu%{;})",
                      0 != tcase.off, tcase.off);
        break;

    case ListIds::erase_iter_iter:
        rw_asnprintf (pbuf, pbufsize,
                      "%{+} (begin()%{?} + %zu%{;}, begin()%{?} + %zu%{;})",
                      0 != tcase.off, tcase.off,
                      0 != range1_end, range1_end);
        break;

    case ListIds::op_equal_ccont_ccont:
    case ListIds::op_less_ccont_ccont:
    case ListIds::op_not_equal_ccont_ccont:
    case ListIds::op_greater_ccont_ccont:
    case ListIds::op_greater_equal_ccont_ccont:
    case ListIds::op_less_equal_ccont_ccont:
        // format zero cont argument without size as arg
        rw_asnprintf (pbuf, pbufsize,
                      "%{+} (%{?}arg2%{:}%{$CLASS}(%{#*s})%{;}, "
                      "%{?}arg1%{:}%{$CLASS}(%{#*s})%{;})",
                      0 == str, int (str_len), str, self, int (arg_len), arg);
        break;

    case ListIds::ctor_size:
    case ListIds::resize_size:
        rw_asnprintf (pbuf, pbufsize,
                      "%{+} (%zu)", tcase.size);
        break;

    case ListIds::resize_size_val:
        rw_asnprintf (pbuf, pbufsize,
                      "%{+} (%zu, %{#c})", tcase.size, tcase.val);
        break;

    case ListIds::swap_cont_cont:
    case ListIds::splice_iter_cont:
    case ListIds::splice_iter_cont_iter:
    case ListIds::splice_iter_cont_iter_iter:
    case ListIds::remove_if_pred:
    case ListIds::unique_bpred:
    case ListIds::merge_cont_comp:
    case ListIds::sort_comp:
        rw_asnprintf (pbuf, pbufsize,
                      "not yet defined");
        break;

    default:
        RW_ASSERT (!"test logic error: unknown list overload");
    }
}

/**************************************************************************/

// temporary, should be defined in 23.deque.h
struct VectorIds : ContainerIds
{
    enum OverloadId {
        dummy
    };
};

static void
_rw_vector_sigcat (char** pbuf, size_t * pbufsize, VectorIds::OverloadId which,
                   bool self, const char* str, size_t str_len,
                   const char* arg, size_t arg_len,
                   const ContainerTestCase &tcase)
{
    // temporary empty

    _RWSTD_UNUSED (pbuf);
    _RWSTD_UNUSED (pbufsize);
    _RWSTD_UNUSED (which);
    _RWSTD_UNUSED (self);
    _RWSTD_UNUSED (str);
    _RWSTD_UNUSED (str_len);
    _RWSTD_UNUSED (arg);
    _RWSTD_UNUSED (arg_len);
    _RWSTD_UNUSED (tcase);
}

/**************************************************************************/

// temporary, should be defined in 23.deque.h
struct DequeIds : ContainerIds
{
    enum OverloadId {
        dummy
    };
};

static void
_rw_deque_sigcat (char** pbuf, size_t * pbufsize, DequeIds::OverloadId which,
                  bool self, const char* str, size_t str_len,
                  const char* arg, size_t arg_len,
                  const ContainerTestCase &tcase)
{
    // temporary empty

    _RWSTD_UNUSED (pbuf);
    _RWSTD_UNUSED (pbufsize);
    _RWSTD_UNUSED (which);
    _RWSTD_UNUSED (self);
    _RWSTD_UNUSED (str);
    _RWSTD_UNUSED (str_len);
    _RWSTD_UNUSED (arg);
    _RWSTD_UNUSED (arg_len);
    _RWSTD_UNUSED (tcase);
}

/**************************************************************************/

// temporary, should be defined in 23.queue.h
struct QueueIds : ContainerIds
{
    enum OverloadId {
        dummy
    };
};

static void
_rw_queue_sigcat (char** pbuf, size_t * pbufsize, QueueIds::OverloadId which,
                  bool self, const char* str, size_t str_len,
                  const char* arg, size_t arg_len,
                  const ContainerTestCase &tcase)
{
    // temporary empty

    _RWSTD_UNUSED (pbuf);
    _RWSTD_UNUSED (pbufsize);
    _RWSTD_UNUSED (which);
    _RWSTD_UNUSED (self);
    _RWSTD_UNUSED (str);
    _RWSTD_UNUSED (str_len);
    _RWSTD_UNUSED (arg);
    _RWSTD_UNUSED (arg_len);
    _RWSTD_UNUSED (tcase);
}

/**************************************************************************/

// temporary, should be defined in 23.stack.h
struct StackIds : ContainerIds
{
    enum OverloadId {
        dummy
    };
};

static void
_rw_stack_sigcat (char** pbuf, size_t * pbufsize, StackIds::OverloadId which,
                  bool self, const char* str, size_t str_len,
                  const char* arg, size_t arg_len,
                  const ContainerTestCase &tcase)
{
    // temporary empty

    _RWSTD_UNUSED (pbuf);
    _RWSTD_UNUSED (pbufsize);
    _RWSTD_UNUSED (which);
    _RWSTD_UNUSED (self);
    _RWSTD_UNUSED (str);
    _RWSTD_UNUSED (str_len);
    _RWSTD_UNUSED (arg);
    _RWSTD_UNUSED (arg_len);
    _RWSTD_UNUSED (tcase);
}

/**************************************************************************/

// sets the {CLASS}, {FUNC}, {FUNCSIG}, and optionally {FUNCALL}
// environment variables as follows:
// CLASS:   the name of cont specialization
// FUNC:    the name of the cont function
// FUNCSIG: the name and signature of a specific overload
//          of the cont function
// FUNCALL: a cont describing the call to the cont function
//          with function with function arguments expanded (as specified
//          by the TestCase argument)
static void
_rw_setvars (const ContainerFunc     &func,
             const ContainerTestCase *pcase = 0)
{
    char*  buf     = 0;
    size_t bufsize = 0;

    const char* const class_name = _rw_class_name (func);

    if (0 == pcase) {
        // set the {Elem}, {Allocator}, {Iterator} and {Container}
        // environment variables to the name of the element type
        // and the Allocator and Iterator and Container specializations
        rw_fprintf (0, "%{$Elem!:*}", _rw_elem_names [func.elem_id_]);

        rw_fprintf (0, "%{$Allocator!:*}", _rw_alloc_names [func.alloc_id_]);

        rw_fprintf (0, "%{$Iterator!:*}", _rw_iter_names [func.iter_id_]);

        rw_fprintf (0, "%{$Container!:*}", class_name);

        // set the {CLASS}, {FUNC}, and {FUNCSIG} environment variables
        // to the name of the specialization of the template, the name
        // of the container function, and the name of the overload of the
        // container function, respectively, when no test case is given

        // format container specializations, leaving out the name
        // of the default allocator for brevity
        rw_asnprintf (&buf, &bufsize,
                      "std::%s<%s%{?}, %s<%2$s>%{;}>",
                      class_name,
                      _rw_elem_names [func.elem_id_],
                      ContainerIds::DefaultAlloc != func.alloc_id_,
                      _rw_alloc_names [func.alloc_id_]);

        // set the {CLASS} variable to the name of the specialization
        // of container
        rw_fprintf (0, "%{$CLASS!:*}", buf);
        free (buf);
        buf     = 0;
        bufsize = 0;

        // determine the container function name
        const size_t funcinx = func.which_ & ContainerIds::fid_mask;
        const size_t nfuncs =  sizeof _rw_func_names / sizeof *_rw_func_names;

        RW_ASSERT (funcinx < nfuncs);

        // get the undecorated function name; ctors are treated
        // specially so that we can have string, wstring, or
        // cont, depending on the template arguments
        const char* const funcname = _rw_func_names [funcinx] ?
            _rw_func_names [funcinx] : class_name;

        // determine whether the function is a member function
        const bool is_member = 0 != (func.which_ & ContainerIds::bit_member);

        // set the {FUNC} variable to the unqualified/undecorated
        // name of the container function (member or otherwise)
        rw_asnprintf (&buf, &bufsize, "%{?}std::%{;}%s",
            !is_member, funcname);

        rw_fprintf (0, "%{$FUNC!:*}", buf);

        // append the function signature
        _rw_sigcat (&buf, &bufsize, &func);

        rw_fprintf (0, "%{$FUNCSIG!:*}", buf);
        free (buf);

        return;
    }

    // do the function call arguments reference *this?
    const bool self = 0 == pcase->arg;

    char str_buf [256];
    char arg_buf [256];

    char *str;
    char *arg;

    size_t str_len = sizeof str_buf;
    size_t arg_len = sizeof arg_buf;

    if (pcase->str)
        str = rw_expand (str_buf, pcase->str, pcase->str_len, &str_len);
    else
        str = 0;

    if (pcase->arg)
        arg = rw_expand (arg_buf, pcase->arg, pcase->arg_len, &arg_len);
    else
        arg = 0;

    // determine whether the function is a member function
    const bool is_member = 0 != (func.which_ & ContainerIds::bit_member);

    // determine whether the function is a ctor
    bool is_ctor = ContainerIds::fid_ctor == (func.which_ & ContainerIds::fid_mask);

    if (is_ctor) {
        // for ctors append just the class name here
        // the class name will inserted below during argument
        // formatting
        rw_asnprintf (&buf, &bufsize, "%{$CLASS}::%s", class_name);
    }
    else if (is_member) {
        // for other members append the ctor argument(s) followed
        // by the cont member function name
        rw_asnprintf (&buf, &bufsize,
                      "%{$CLASS} (%{?}%{#*s}%{;}).%{$FUNC}",
                      str != 0, int (str_len), str);
    }
    else {
        // for non-members append just the function name here
        // the class name will inserted below during argument
        // formatting
        rw_asnprintf (&buf, &bufsize, "%{$FUNC}");
    }

    // format and append container function arguments abbreviating complex
    // expressions as much as possible to make them easy to understand
    switch (func.cont_id_) {

    case ContainerIds::List:
        _rw_list_sigcat (&buf, &bufsize,
            _RWSTD_STATIC_CAST (ListIds::OverloadId, func.which_),
            self, str, str_len, arg, arg_len, *pcase);
        break;

    case ContainerIds::Vector:
        _rw_vector_sigcat (&buf, &bufsize,
            _RWSTD_STATIC_CAST (VectorIds::OverloadId, func.which_),
            self, str, str_len, arg, arg_len, *pcase);
        break;

    case ContainerIds::Deque:
        _rw_deque_sigcat (&buf, &bufsize,
            _RWSTD_STATIC_CAST (DequeIds::OverloadId, func.which_),
            self, str, str_len, arg, arg_len, *pcase);
        break;

    case ContainerIds::Queue:
        _rw_queue_sigcat (&buf, &bufsize,
            _RWSTD_STATIC_CAST (QueueIds::OverloadId, func.which_),
            self, str, str_len, arg, arg_len, *pcase);
        break;

    case ContainerIds::Stack:
        _rw_stack_sigcat (&buf, &bufsize,
            _RWSTD_STATIC_CAST (StackIds::OverloadId, func.which_),
            self, str, str_len, arg, arg_len, *pcase);
        break;

    default:
        RW_ASSERT (!"unknown container id");
    }

    rw_fprintf (0, "%{$FUNCALL!:*}", buf);
    free (buf);

    if (str != str_buf)
        delete[] str;

    if (arg != arg_buf)
        delete[] arg;
}

/**************************************************************************/

// helper function to reverse substring in the resulting sequence
_RWSTD_INTERNAL ContainerTestCase
_rw_reverse_results (const ContainerTestCase &tsrc,
                     size_t off, size_t ext)
{
    // expand expected results
    size_t res_len = 0;
    char* const new_res = rw_expand ((char*)0, tsrc.res, tsrc.nres, &res_len);

    // reverse them
    const size_t res_off = off;
    const size_t res_ext = (ext < res_len ? ext : res_len) - 1;

    char* beg = new_res + res_off;
    char* end = beg + res_ext;

    for (; beg < end; ++beg, --end) {
        const char tmp = *beg;
        *beg = *end;
        *end = tmp;
    }

    // form new test case
    const ContainerTestCase new_case = {
        tsrc.line, tsrc.off, tsrc.size, tsrc.off2,
        tsrc.size2, tsrc.val, tsrc.str, tsrc.str_len,
        tsrc.arg, tsrc.arg_len, new_res, res_len, tsrc.bthrow
    };

    return new_case;
}


template <class T, class Allocator>
void
_rw_dispatch (T*, Allocator*,
              VoidFunc* const    *farray,
              const ContainerFunc     &func,
              const ContainerTestCase &tcase)
{
    typedef ContainerTestCaseData<T> Data;
    typedef void TestFunc (T*, Allocator*, const Data&);

    const size_t inx = func.elem_id_ * 2 + func.alloc_id_;

    TestFunc* const tfunc = _RWSTD_REINTERPRET_CAST (TestFunc*, farray [inx]);

    if (0 == tfunc) {
        rw_error (0, __FILE__, __LINE__,
                  "logic error: null test function for %{$FUNCSIG}");
        return;
    }

    const bool reverse_iter =
           ContainerIds::ReverseIterator      == func.iter_id_
        || ContainerIds::ConstReverseIterator == func.iter_id_;

    const Data tdata (func, tcase);

    if (reverse_iter) {
        // special processing for reverse iterators

        const size_t func_id = tdata.func_.which_ & ContainerIds::fid_mask;

        const bool like_ctor =
               ContainerIds::fid_ctor   == func_id
            || ContainerIds::fid_assign == func_id;

        // ctor and assignment operator require the full container reverse
        const size_t off = like_ctor ? 0 : tdata.off1_;
        const size_t ext = like_ctor ? tdata.reslen_ : tdata.ext2_;

        const ContainerTestCase rev_tcase =
            _rw_reverse_results (tcase, off, ext);

        const Data rev_tdata (func, rev_tcase);

        tfunc ((T*)0, (Allocator*)0, rev_tdata);

        // clean up allocated memory, if any
        delete[] rev_tcase.res;
    }
    else
        tfunc ((T*)0, (Allocator*)0, tdata);
}


template <class T>
void
_rw_dispatch (T*,
              VoidFunc* const    *farray,
              const ContainerFunc     &func,
              const ContainerTestCase &tcase)
{
    if (ContainerIds::DefaultAlloc == func.alloc_id_) {
        typedef std::allocator<T> Alloc;
        _rw_dispatch ((T*)0, (Alloc*)0, farray, func, tcase);
    }
    else if (ContainerIds::UserAlloc == func.alloc_id_) {
        typedef UserAlloc<T> Alloc;
        _rw_dispatch ((T*)0, (Alloc*)0, farray, func, tcase);
    }
    else {
        RW_ASSERT (!"logic error: unknown Allocator argument");
    }
}


static void
_rw_dispatch (VoidFunc* const    *farray,
              const ContainerFunc     &func,
              const ContainerTestCase &tcase)
{
    if (ContainerIds::UserPOD == func.elem_id_) {
        _rw_dispatch ((UserPOD*)0, farray, func, tcase);
    }
    else if (ContainerIds::UserClass == func.elem_id_) {
        _rw_dispatch ((UserClass*)0, farray, func, tcase);
    }
    else {
        RW_ASSERT (!"logic error: unknown T argument");
    }
}

/**************************************************************************/

// exercise a single test case for the given function
static void
_rw_test_case (const ContainerFunc     &func,
               const ContainerTestCase &tcase,
               ContainerTestFunc       *test_callback,
               VoidFunc* const         *farray)
{
    // check to see if this is an exception safety test case
    // and avoid running it when exception safety has been
    // disabled via a command line option
    if (-1 == tcase.bthrow && _rw_opt_no_exception_safety) {

        // issue only the first note
        rw_note (1 < _rw_opt_no_exception_safety++, _rw_this_file, __LINE__,
            "exception safety tests disabled");
        return;
    }

    // check to see if this is a test case that involves the throwing
    // of an exception and avoid running it when exceptions have been
    // disabled
    if (tcase.bthrow && _rw_opt_no_exceptions) {

        // issue only the first note
        rw_note (1 < _rw_opt_no_exceptions++, _rw_this_file, __LINE__,
                 "exception tests disabled");
        return;
    }

    const bool self_ref = 0 == tcase.arg;

    // check for tests exercising self-referential modifications
    // (e.g., insert(1, *this))
    if (_rw_opt_self_ref < 0 && self_ref) {
        // issue only the first note
        rw_note (0, _rw_this_file, tcase.line,
                 "self-referential test disabled");
        return;
    }
    else if (0 < _rw_opt_self_ref && !self_ref) {
        // issue only the first note
        rw_note (0, _rw_this_file, tcase.line,
                 "non-self-referential test disabled");
        return;
    }

    // check to see if the test case is enabled
    if (rw_enabled (tcase.line)) {

        // set the {FUNCALL} environment variable to describe
        // the function call specified by this test case
        _rw_setvars (func, &tcase);

        if (test_callback) {
            // invoke the test callback function
            test_callback (func, tcase);
        }
        else {
            _rw_dispatch (farray, func, tcase);
        }
    }
    else
        rw_note (0, _rw_this_file, tcase.line,
                 "test on line %d disabled", tcase.line);
}

/**************************************************************************/

static ContainerIds::ContainerId
_rw_container_id;

static ContainerTestFunc*
_rw_test_callback;


static VoidFunc* const*
_rw_func_array;


// exercise all test cases defined for the given function
static void
_rw_run_cases (const ContainerFunc &func,
               const ContainerTest &test)
{
    // set the {CLASS}, {FUNC}, and {FUNCSIG} environment
    // variable to the name of the container specialization
    // and the container function being exercised
    _rw_setvars (func);

    // determine whether the function is a member function
    const bool is_member = 0 != (ContainerIds::bit_member & test.which);

    // compute the function overload's 0-based index
    const size_t siginx = _rw_get_func_inx (test.which);

    // check if tests of the function overload
    // have been disabled
    if (0 == rw_note (0 <= _rw_opt_func [siginx], _rw_this_file, __LINE__,
                      "%{?}%{$CLASS}::%{;}%{$FUNCSIG} tests disabled",
                      is_member))
        return;

    rw_info (0, 0, 0, "%{?}%{$CLASS}::%{;}%{$FUNCSIG}", is_member);

    const size_t case_count = test.case_count;

    // iterate over all test cases for this function
    // overload invoking the test case handler for each
    // in turn
    for (size_t n = 0; n != case_count; ++n) {

        const ContainerTestCase& tcase = test.cases [n];

        _rw_test_case (func, tcase, _rw_test_callback, _rw_func_array);
    }
}

/**************************************************************************/

void
_rw_toggle_options (int *opts, size_t count)
{
    for (size_t i = 0; i != count; ++i) {
        if (0 < opts [i]) {
            // if one or more options has been explicitly enabled
            // treat all those that haven't been as if they had
            // been disabled
            for (i = 0; i != count; ++i) {
                if (0 == opts [i])
                    opts [i] = -1;
            }
            break;
        }
    }
}


static int
_rw_run_test (int, char*[])
{
#ifdef _RWSTD_NO_EXCEPTIONS

    rw_note (0, 0, 0, "exception tests disabled (macro "
        "_RWSTD_NO_EXCEPTIONS #defined)");

    // disable all exception tests and avoid further notes
    _rw_no_exceptions       = 2;
    _rw_no_exception_safety = 2;

#endif   // _RWSTD_NO_EXCEPTIONS

    // see if any option controlling a container function has been
    // explicitly enabled and if so disable all those that haven't
    // been (i.e., so that --enable-foo-size will have the effect
    // of specifying --disable-foo-val and --disable-foo-range,
    // given the three overloads of foo)
    const size_t nopts = sizeof _rw_opt_func / sizeof *_rw_opt_func;
    _rw_toggle_options (_rw_opt_func, nopts);

    static const ContainerIds::ElemId elem_types[] = {
        ContainerIds::UserPOD, ContainerIds::UserClass
    };

    static const ContainerIds::AllocId alloc_types[] = {
        ContainerIds::DefaultAlloc, ContainerIds::UserAlloc
    };

    static const ContainerIds::IteratorId iter_types[] = {
        ContainerIds::Input, ContainerIds::Forward,
        ContainerIds::Bidir, ContainerIds::Random,
        ContainerIds::Pointer, ContainerIds::ConstPointer,
        ContainerIds::Iterator, ContainerIds::ConstIterator,
        ContainerIds::ReverseIterator, ContainerIds::ConstReverseIterator
    };

    const size_t n_elem_types   = sizeof elem_types / sizeof *elem_types;
    const size_t n_alloc_types  = sizeof alloc_types / sizeof *alloc_types;
    const size_t n_iter_types   = sizeof iter_types / sizeof *iter_types;

    // see if any option controlling the container template arguments
    // explicitly enabled and if so disable all those that haven't been
    _rw_toggle_options (_rw_opt_elem_types, n_elem_types);
    _rw_toggle_options (_rw_opt_alloc_types, n_alloc_types);
    _rw_toggle_options (_rw_opt_iter_types, n_iter_types);

    // exercise different T specializations last
    for (size_t i = 0; i != n_elem_types; ++i) {

        if (_rw_opt_elem_types [i] < 0) {
            // issue only the first note
            rw_note (-1 > _rw_opt_elem_types [i]--,
                     _rw_this_file, __LINE__,
                     "%s tests disabled", _rw_elem_names [i]);
            continue;
        }

        for (size_t k = 0; k != n_alloc_types; ++k) {

            if (_rw_opt_alloc_types [k] < 0) {
                // issue only the first note
                rw_note (-1 > _rw_opt_alloc_types [k]--,
                         _rw_this_file, __LINE__,
                         "%s tests disabled", _rw_alloc_names [k]);
                continue;
            }

            for (size_t m = 0; m != _rw_cont_test_count; ++m) {

                const ContainerTest& test = _rw_cont_tests [m];

                // create an object uniquely identifying the overload
                // of the container function exercised by the set of test
                // cases defined to exercise it
                ContainerFunc func = {
                    elem_types [i],
                    alloc_types [k],
                    ContainerIds::None,
                    _rw_container_id,
                    test.which
                };

                // determine whether the function is a template
                if (-1 < _rw_argno (test.which, ContainerIds::arg_range)) {

                    // iterate over the standard iterator categories
                    // and iterator types the template might perhaps
                    // be specialized on
                    for (size_t l = 0; l != n_iter_types; ++l) {

                        if (_rw_opt_iter_types [l] < 0) {
                            // issue only the first note
                            rw_note (-1 > _rw_opt_iter_types [l]--,
                                     _rw_this_file, __LINE__,
                                     "%s tests disabled",
                                     _rw_iter_names [l]);
                            continue;
                        }

                        func.iter_id_ = iter_types [l];

                        // exercise all test cases defined for
                        // the function template
                        _rw_run_cases (func, test);
                    }
                }
                else {
                    // exercise all test cases defined for the ordinary
                    // (i.e., non-template) function
                    _rw_run_cases (func, test);
                }
            }
        }
    }

    return 0;
}

/**************************************************************************/

// add a bunch of toggle-type command line options based on the names array
static void
_rw_add_toggles (char **pbuf, size_t *pbufsize,
                 const char* const names[], size_t count)
{
    for (size_t i = 0; i != count; ++i) {
        rw_asnprintf (pbuf, pbufsize, "%{+}|-%s~ ", names [i]);
    }
}

static void
_rw_add_container_toggles (char **pbuf, size_t *pbufsize)
{
    _rw_add_toggles (pbuf, pbufsize, _rw_alloc_names,
                     sizeof _rw_alloc_names / sizeof *_rw_alloc_names);
    _rw_add_toggles (pbuf, pbufsize, _rw_iter_names + 1,
                     sizeof _rw_iter_names / sizeof *_rw_iter_names - 1);
}


static int
_rw_run_test  (int                       argc,
               char                     *argv [],
               const char               *file,
               const char               *clause,
               ContainerIds::ContainerId container,
               ContainerTestFunc        *test_callback,
               VoidFunc* const          *func_array,
               const ContainerTest      *tests,
               size_t                    test_count)
{
    // set the global variables accessed in _rw_run_test
    _rw_container_id      = container;
    _rw_test_callback     = test_callback;
    _rw_func_array        = func_array,
    _rw_cont_tests        = tests;
    _rw_cont_test_count   = test_count;

    // put together a command line option specification with options
    // to enable and disable tests exercising functions for all known
    // specializations of the functions specified by the test array
    char   *optbuf     = 0;
    size_t  optbufsize = 0;

    rw_asnprintf (&optbuf, &optbufsize,
                  "|-no-exceptions# "
                  "|-no-exception-safety# "
                  "|-self-ref~ ");

    const size_t n_elems  = sizeof _rw_elem_names / sizeof *_rw_elem_names;

    // see if any option has been explicitly enabled and if so,
    // unconditionally disable all those that have not been
    _rw_add_toggles (&optbuf, &optbufsize, _rw_elem_names, n_elems);
    _rw_add_container_toggles (&optbuf, &optbufsize);

    for (size_t i = 0; i != test_count; ++i) {

        // for each function append a command line option specification
        // to allow to enable or disable it
        rw_asnprintf (&optbuf, &optbufsize, "%{+}|-");
        _rw_sigcat (&optbuf, &optbufsize, 0, tests [i].which);
        rw_asnprintf (&optbuf, &optbufsize, "%{+}~ ");
    }

    RW_ASSERT (test_count <= 32);
    RW_ASSERT (test_count <= MAX_OVERLOADS);

    // process command line arguments run tests
    const int status =
        rw_test (argc, argv, file, clause,
        0,              // comment
        _rw_run_test,   // test callback
        optbuf,         // option specification

        // handlers controlling exceptions
        &_rw_opt_no_exceptions,
        &_rw_opt_no_exception_safety,

        // handler controlling self-referential modifiers
        &_rw_opt_self_ref,

        // handlers controlling specializations of the template
        // ...on the T template parameter
        _rw_opt_elem_types + 0,
        _rw_opt_elem_types + 1,

        // ...on the Allocator template parameter
        _rw_opt_alloc_types + 0,
        _rw_opt_alloc_types + 1,

        // FIXME: add handlers (and options) only for tests
        // that exercise member templates

        // handlers controlling specializations of the member
        // template (if this is one) on the InputIterator
        // template parameter
        _rw_opt_iter_types + 0,
        _rw_opt_iter_types + 1,
        _rw_opt_iter_types + 2,
        _rw_opt_iter_types + 3,
        _rw_opt_iter_types + 4,
        _rw_opt_iter_types + 5,
        _rw_opt_iter_types + 6,
        _rw_opt_iter_types + 7,
        _rw_opt_iter_types + 8,
        _rw_opt_iter_types + 9,

        // FIXME: install exactly as many handlers (and options)
        // as there are distinct functions being exercised

        // handlers for up to 32 overloads
        _rw_opt_func +  0,
        _rw_opt_func +  1,
        _rw_opt_func +  2,
        _rw_opt_func +  3,
        _rw_opt_func +  4,
        _rw_opt_func +  5,
        _rw_opt_func +  6,
        _rw_opt_func +  7,
        _rw_opt_func +  8,
        _rw_opt_func +  9,
        _rw_opt_func + 10,
        _rw_opt_func + 11,
        _rw_opt_func + 12,
        _rw_opt_func + 13,
        _rw_opt_func + 14,
        _rw_opt_func + 15,
        _rw_opt_func + 16,
        _rw_opt_func + 17,
        _rw_opt_func + 18,
        _rw_opt_func + 19,
        _rw_opt_func + 20,
        _rw_opt_func + 21,
        _rw_opt_func + 22,
        _rw_opt_func + 23,
        _rw_opt_func + 24,
        _rw_opt_func + 25,
        _rw_opt_func + 26,
        _rw_opt_func + 27,
        _rw_opt_func + 28,
        _rw_opt_func + 29,
        _rw_opt_func + 30,
        _rw_opt_func + 31,

        // sentinel
        (void*)0);

    // free storage allocated for the option specification
    free (optbuf);

    return status;
}



_TEST_EXPORT int
rw_run_cont_test (int                        argc,
                  char                      *argv [],
                  const char                *file,
                  const char                *clause,
                  ContainerIds::ContainerId  container,
                  ContainerTestFunc         *callback,
                  const ContainerTest       *tests,
                  size_t                     count)
{
    return _rw_run_test (argc, argv, file, clause, container,
                         callback, 0, tests, count);
}


_TEST_EXPORT int
rw_run_cont_test (int                        argc,
                  char                      *argv [],
                  const char                *file,
                  const char                *clause,
                  ContainerIds::ContainerId  container,
                  VoidFunc* const           *farray,
                  const ContainerTest       *tests,
                  size_t                     count)
{
    return _rw_run_test (argc, argv, file, clause, container,
                         0, farray, tests, count);
}
