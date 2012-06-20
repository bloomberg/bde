/************************************************************************
 *
 * 21.strings.h - definitions of helpers used in clause 21 tests
 *
 * $Id: 21.strings.h 648752 2008-04-16 17:01:56Z faridz $
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

#ifndef RW_21_STRINGS_H_INCLUDED
#define RW_21_STRINGS_H_INCLUDED

#include <rw_char.h>        // for rw_expand()
#include <testdefs.h>
#include <23.containers.h>  // for ContainerIds

/**************************************************************************/

// defines enumerations identifying basic_string template arguments,
// sets of overloaded functions, member types used in the declarations
// of their signatures, and specific overloads of such member functions
struct StringIds: ContainerIds {

    // identifiers for the charT template argument
    enum CharId { Char, WChar, UChar };

    // identifiers for the Traits template argument
    enum TraitsId { DefaultTraits, UserTraits };

    // identifies a set of overloaded member or non-member
    // string functions
    enum FuncId {
        // 6 bits, 64 functions max
        /*  0 */ fid_append,
        /*  1 */ fid_assign,
        /*  2 */ fid_erase,
        /*  3 */ fid_insert,
        /*  4 */ fid_replace,
        /*  5 */ fid_op_plus_eq,
        /*  6 */ fid_find,
        /*  7 */ fid_rfind,
        /*  8 */ fid_find_first_of,
        /*  9 */ fid_find_last_of,
        /* 10 */ fid_find_first_not_of,
        /* 11 */ fid_find_last_not_of,
        /* 12 */ fid_compare,
        /* 13 */ fid_substr,
        /* 14 */ fid_op_index,
        /*    */ fid_op_index_const = fid_op_index,
        /* 15 */ fid_at,
        /*    */ fid_at_const = fid_at,
        /* 16 */ fid_copy,
        /* 17 */ fid_ctor,
        /* 18 */ fid_op_set,
        /* 19 */ fid_swap,
        /* 20 */ fid_push_back,
        /* 21 */ fid_op_plus,
        /* 22 */ fid_op_equal,
        /* 23 */ fid_op_not_equal,
        /* 24 */ fid_op_less,
        /* 25 */ fid_op_less_equal,
        /* 26 */ fid_op_greater,
        /* 27 */ fid_op_greater_equal,
        /* 28 */ fid_size,
        /* 29 */ fid_length,
        /* 30 */ fid_max_size,
        /* 31 */ fid_resize,
        /* 32 */ fid_capacity,
        /* 33 */ fid_reserve,
        /* 34 */ fid_clear,
        /* 35 */ fid_empty,
        /* 36 */ fid_begin,
        /*    */ fid_begin_const = fid_begin,
        /* 37 */ fid_end,
        /*    */ fid_end_const = fid_end,
        /* 38 */ fid_rbegin,
        /*    */ fid_rbegin_const = fid_rbegin,
        /* 39 */ fid_rend,
        /*    */ fid_rend_const = fid_rend,
        /* 40 */ fid_c_str,
        /* 41 */ fid_data,
        /* 42 */ fid_get_allocator,
        /* 43 */ fid_extractor,
        /* 44 */ fid_inserter,
        /* 45 */ fid_getline,
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
        /*  3 */ arg_ptr,     // pointer
        /*  4 */ arg_cptr,    // const_pointer
        /*  5 */ arg_ref,     // reference
        /*  6 */ arg_cref,    // const_reference
        /*  7 */ arg_iter,    // iterator
        /*  8 */ arg_citer,   // const_iterator
        /*  9 */ arg_range,   // Iterator, Iterator
        /* 10 */ arg_str,     // string& (or this for member functions)
        /* 11 */ arg_cstr,    // const string& (or const this for members)
        /* 12 */ arg_alloc,   // const allocator&
        /* 13 */ arg_istream, // istream&
        /* 14 */ arg_ostream, // ostream&
        /* -- */ arg_bits = 4,
        /* -- */ arg_mask = 15
    };

    enum {
        // bit designating a member function
        bit_member = 1 << (fid_bits + 6 * arg_bits)
    };

// define the helper macros
#include <rw_sigdefs.h>

    // unique identifiers for all overloads of each member function
    //     6 bits for FuncId
    // 6 * 4 bits for ArgId (at most 6 arguments including this)
    //     1 bit for membership
    enum OverloadId {
        //////////////////////////////////////////////////////////////
        // append (const_pointer)
        MEMBER_1 (append, str, cptr),
        // append (const basic_string&)
        MEMBER_1 (append, str, cstr),
        // append (const_pointer, size_type)
        MEMBER_2 (append, str, cptr, size),
        // append (const basic_string&, size_type, size_type)
        MEMBER_3 (append, str, cstr, size, size),
        // append (size_type, value_type)
        MEMBER_2 (append, str, size, val),
        // append (InputIterator, InputIterator)
        MEMBER_1 (append, str, range),

        //////////////////////////////////////////////////////////////
        // assign (const_pointer)
        MEMBER_1 (assign, str, cptr),
        // assign (const basic_string&)
        MEMBER_1 (assign, str, cstr),
        // assign (const_pointer, size_type)
        MEMBER_2 (assign, str, cptr, size),
        // assign (const basic_string&, size_type, size_type)
        MEMBER_3 (assign, str, cstr, size, size),
        // assign (size_type, value_type)
        MEMBER_2 (assign, str, size, val),
        // assign (InputIterator, InputIterator)
        MEMBER_1 (assign, str, range),

        //////////////////////////////////////////////////////////////
        // erase ()
        MEMBER_0 (erase, str),
        // erase (size_type)
        MEMBER_1 (erase, str, size),
        // erase (size_type, size_type)
        MEMBER_2 (erase, str, size, size),
        // erase (iterator)
        MEMBER_1 (erase, str, iter),
        // erase (iterator, iterator)
        MEMBER_2 (erase, str, iter, iter),

        //////////////////////////////////////////////////////////////
        // insert (size_type, const_pointer)
        MEMBER_2 (insert, str, size, cptr),
        // insert (size_type, const basic_string&)
        MEMBER_2 (insert, str, size, cstr),
        // insert (size_type, const_pointer, size_type)
        MEMBER_3 (insert, str, size, cptr, size),
        // insert (size_type, const basic_string&, size_type, size_type)
        MEMBER_4 (insert, str, size, cstr, size, size),
        // insert (size_type, size_type, value_type)
        MEMBER_3 (insert, str, size, size, val),
        // insert (iterator, value_type)
        MEMBER_2 (insert, str, iter, val),
        // insert (iterator, size_type, value_type)
        MEMBER_3 (insert, str, iter, size, val),
        // insert (iterator, InputIterator, InputIterator)
        MEMBER_2 (insert, str, iter, range),

        //////////////////////////////////////////////////////////////
        // (size_type, size_type, const_pointer)
        MEMBER_3 (replace, str, size, size, cptr),
        // (size_type, size_type, const basic_string&)
        MEMBER_3 (replace, str, size, size, cstr),
        // (size_type, size_type, const_pointer, size_type)
        MEMBER_4 (replace, str, size, size, cptr, size),
        // (size_type, size_type, const basic_string&, size_type, size_type)
        MEMBER_5 (replace, str, size, size, cstr, size, size),
        // (size_type, size_type, size_type, value_type)
        MEMBER_4 (replace, str, size, size, size, val),
        // (iterator, iterator, const_pointer)
        MEMBER_3 (replace, str, iter, iter, cptr),
        // (iterator, iterator, const basic_string&)
        MEMBER_3 (replace, str, iter, iter, cstr),
        // (iterator, iterator, const_pointer, size_type)
        MEMBER_4 (replace, str, iter, iter, cptr, size),
        // (iterator, iterator, size_type, value_type)
        MEMBER_4 (replace, str, iter, iter, size, val),
        // (iterator, iterator, InputIterator, InputIterator)
        MEMBER_3 (replace, str, iter, iter, range),

        //////////////////////////////////////////////////////////////
        // operator+= (const_pointer)
        MEMBER_1 (op_plus_eq, str, cptr),
        // operator+= (const basic_string&)
        MEMBER_1 (op_plus_eq, str, cstr),
        // operator+= (value_type)
        MEMBER_1 (op_plus_eq, str, val),

        //////////////////////////////////////////////////////////////
        // overloads of find, rfind, find_first_of, find_last_of,
        // find_first_not_of, find_last_not_of

        // find (const_pointer) const
        MEMBER_1 (find, cstr, cptr),
        // find (const basic_string&) const
        MEMBER_1 (find, cstr, cstr),
        // find (const_pointer, size_type) const
        MEMBER_2 (find, cstr, cptr, size),
        // find (const_pointer, size_type, size_type) const
        MEMBER_3 (find, cstr, cptr, size, size),
        // find (const basic_string&, size_type) const
        MEMBER_2 (find, cstr, cstr, size),
        // find (value_type) const
        MEMBER_1 (find, cstr, val),
        // find (value_type, size_type) const
        MEMBER_2 (find, cstr, val, size),

        //////////////////////////////////////////////////////////////
        // rfind (const_pointer) const
        MEMBER_1 (rfind, cstr, cptr),
        // rfind (const basic_string&) const
        MEMBER_1 (rfind, cstr, cstr),
        // rfind (const_pointer, size_type) const
        MEMBER_2 (rfind, cstr, cptr, size),
        // rfind (const_pointer, size_type, size_type) const
        MEMBER_3 (rfind, cstr, cptr, size, size),
        // rfind (const basic_string&, size_type) const
        MEMBER_2 (rfind, cstr, cstr, size),
        // rfind (value_type) const
        MEMBER_1 (rfind, cstr, val),
        // rfind (value_type, size_type) const
        MEMBER_2 (rfind, cstr, val, size),

        //////////////////////////////////////////////////////////////
        // find_first_of (const_pointer) const
        MEMBER_1 (find_first_of, cstr, cptr),
        // find_first_of (const basic_string&) const
        MEMBER_1 (find_first_of, cstr, cstr),
        // find_first_of (const_pointer, size_type) const
        MEMBER_2 (find_first_of, cstr, cptr, size),
        // find_first_of (const_pointer, size_type, size_type) const
        MEMBER_3 (find_first_of, cstr, cptr, size, size),
        // find_first_of (const basic_string&, size_type) const
        MEMBER_2 (find_first_of, cstr, cstr, size),
        // find_first_of (value_type) const
        MEMBER_1 (find_first_of, cstr, val),
        // find_first_of (value_type, size_type) const
        MEMBER_2 (find_first_of, cstr, val, size),

        //////////////////////////////////////////////////////////////
        // find_last_of (const_pointer) const
        MEMBER_1 (find_last_of, cstr, cptr),
        // find_last_of (const basic_string&) const
        MEMBER_1 (find_last_of, cstr, cstr),
        // find_last_of (const_pointer, size_type) const
        MEMBER_2 (find_last_of, cstr, cptr, size),
        // find_last_of (const_pointer, size_type, size_type) const
        MEMBER_3 (find_last_of, cstr, cptr, size, size),
        // find_last_of (const basic_string&, size_type) const
        MEMBER_2 (find_last_of, cstr, cstr, size),
        // find_last_of (value_type) const
        MEMBER_1 (find_last_of, cstr, val),
        // find_last_of (value_type, size_type) const
        MEMBER_2 (find_last_of, cstr, val, size),

        //////////////////////////////////////////////////////////////
        // find_first_not_of (const_pointer) const
        MEMBER_1 (find_first_not_of, cstr, cptr),
        // find_first_not_of (const basic_string&) const
        MEMBER_1 (find_first_not_of, cstr, cstr),
        // find_first_not_of (const_pointer, size_type) const
        MEMBER_2 (find_first_not_of, cstr, cptr, size),
        // find_first_not_of (const_pointer, size_type, size_type) const
        MEMBER_3 (find_first_not_of, cstr, cptr, size, size),
        // find_first_not_of (const basic_string&, size_type) const
        MEMBER_2 (find_first_not_of, cstr, cstr, size),
        // find_first_not_of (value_type) const
        MEMBER_1 (find_first_not_of, cstr, val),
        // find_first_not_of (value_type, size_type) const
        MEMBER_2 (find_first_not_of, cstr, val, size),

        //////////////////////////////////////////////////////////////
        // find_last_not_of (const_pointer) const
        MEMBER_1 (find_last_not_of, cstr, cptr),
        // find_last_not_of (const basic_string&) const
        MEMBER_1 (find_last_not_of, cstr, cstr),
        // find_last_not_of (const_pointer, size_type) const
        MEMBER_2 (find_last_not_of, cstr, cptr, size),
        // find_last_not_of (const_pointer, size_type, size_type) const
        MEMBER_3 (find_last_not_of, cstr, cptr, size, size),
        // find_last_not_of (const basic_string&, size_type) const
        MEMBER_2 (find_last_not_of, cstr, cstr, size),
        // find_last_not_of (value_type) const
        MEMBER_1 (find_last_not_of, cstr, val),
        // find_last_not_of (value_type, size_type) const
        MEMBER_2 (find_last_not_of, cstr, val, size),

        //////////////////////////////////////////////////////////////
        // compare (const_pointer) const
        MEMBER_1 (compare, cstr, cptr),
        // compare (const basic_string&) const
        MEMBER_1 (compare, cstr, cstr),
        // compare (size_type, size_type, const_pointer) const
        MEMBER_3 (compare, cstr, size, size, cptr),
        // compare (size_type, size_type, const basic_string&) const
        MEMBER_3 (compare, cstr, size, size, cstr),
        // compare (size_type, size_type, const_pointer, size_type) const
        MEMBER_4 (compare, cstr, size, size, cptr, size),
        // compare (size_type, size_type, const basic_string&,
        //          size_type, size_type) const
        MEMBER_5 (compare, cstr, size, size, cstr, size, size),

        //////////////////////////////////////////////////////////////
        // substr (void) const
        MEMBER_0 (substr, cstr),
        // substr (size_type) const
        MEMBER_1 (substr, cstr, size),
        // substr (size_type, size_type) const
        MEMBER_2 (substr, cstr, size, size),

        //////////////////////////////////////////////////////////////
        // operator[] (size_type)
        MEMBER_1 (op_index, str, size),
        // operator[] (size_type) const
        MEMBER_1 (op_index_const, cstr, size),
        // at (size_type)
        MEMBER_1 (at, str, size),
        // at (size_type) const
        MEMBER_1 (at_const, cstr, size),

        //////////////////////////////////////////////////////////////
        // copy (pointer, size_type) const
        MEMBER_2 (copy, cstr, ptr, size),
        // copy (pointer, size_type, size_type) const
        MEMBER_3 (copy, cstr, ptr, size, size),

        //////////////////////////////////////////////////////////////
        // basic_string (void)
        MEMBER_0 (ctor, str),
        // basic_string (const allocator_type&)
        MEMBER_1 (ctor, str, alloc),
        // basic_string (const_pointer)
        MEMBER_1 (ctor, str, cptr),
        // basic_string (const_pointer, const allocator_type&)
        MEMBER_2 (ctor, str, cptr, alloc),
        // basic_string (const basic_string&)
        MEMBER_1 (ctor, str, cstr),
        // basic_string (const_pointer, size_type)
        MEMBER_2 (ctor, str, cptr, size),
        // basic_string (const_pointer, size_type, const allocator_type&)
        MEMBER_3 (ctor, str, cptr, size, alloc),
        // basic_string (const basic_string&, size_type)
        MEMBER_2 (ctor, str, cstr, size),
        // basic_string (const basic_string&, size_type, const allocator&)
        MEMBER_3 (ctor, str, cstr, size, alloc),
        // basic_string (const basic_string&, size_type, size_type)
        MEMBER_3 (ctor, str, cstr, size, size),
        // basic_string (const basic_string&, size_type, size_type, allocator&)
        MEMBER_4 (ctor, str, cstr, size, size, alloc),
        // basic_string (size_type, value_type)
        MEMBER_2 (ctor, str, size, val),
        // basic_string (size_type, value_type, const allocator_type&)
        MEMBER_3 (ctor, str, size, val, alloc),
        // basic_string (InputIterator, InputIterator)
        MEMBER_1 (ctor, str, range),
        // basic_string (InputIterator, InputIterator, const allocator&)
        MEMBER_2 (ctor, str, range, alloc),

        //////////////////////////////////////////////////////////////
        // operator= (const_pointer)
        MEMBER_1 (op_set, str, cptr),
        // operator= (const basic_string&)
        MEMBER_1 (op_set, str, cstr),
        // operator= (value_type)
        MEMBER_1 (op_set, str, val),

        //////////////////////////////////////////////////////////////
        // swap (basic_string&)
        MEMBER_1 (swap, str, str),

        //////////////////////////////////////////////////////////////
        // push_back (value_type)
        MEMBER_1 (push_back, str, val),

        //////////////////////////////////////////////////////////////
        // operator+ (const_pointer, const basic_string&)
        NON_MEMBER_2 (op_plus, cptr, cstr),
        // operator+ (const basic_string&, const basic_string&)
        NON_MEMBER_2 (op_plus, cstr, cstr),
        // operator+ (const basic_string&, const_pointer)
        NON_MEMBER_2 (op_plus, cstr, cptr),
        // operator+ (const basic_string&, value_type)
        NON_MEMBER_2 (op_plus, cstr, val),
        // operator+ (value_type, const basic_string&)
        NON_MEMBER_2 (op_plus, val, cstr),

        //////////////////////////////////////////////////////////////
        // operator== (const_pointer, const basic_string&)
        NON_MEMBER_2 (op_equal, cptr, cstr),
        // operator== (const basic_string&, const basic_string&)
        NON_MEMBER_2 (op_equal, cstr, cstr),
        // operator== (const basic_string&, const_pointer)
        NON_MEMBER_2 (op_equal, cstr, cptr),

        //////////////////////////////////////////////////////////////
        // operator!= (const_pointer, const basic_string&)
        NON_MEMBER_2 (op_not_equal, cptr, cstr),
        // operator!= (const basic_string&, const basic_string&)
        NON_MEMBER_2 (op_not_equal, cstr, cstr),
        // operator!= (const basic_string&, const_pointer)
        NON_MEMBER_2 (op_not_equal, cstr, cptr),

        //////////////////////////////////////////////////////////////
        // operator< (const_pointer, const basic_string&)
        NON_MEMBER_2 (op_less, cptr, cstr),
        // operator< (const basic_string&, const basic_string&)
        NON_MEMBER_2 (op_less, cstr, cstr),
        // operator< (const basic_string&, const_pointer)
        NON_MEMBER_2 (op_less, cstr, cptr),

        //////////////////////////////////////////////////////////////
        // operator<= (const_pointer, const basic_string&)
        NON_MEMBER_2 (op_less_equal, cptr, cstr),
        // operator<= (const basic_string&, const basic_string&)
        NON_MEMBER_2 (op_less_equal, cstr, cstr),
        // operator<= (const basic_string&, const_pointer)
        NON_MEMBER_2 (op_less_equal, cstr, cptr),

        //////////////////////////////////////////////////////////////
        // operator> (const_pointer, const basic_string&)
        NON_MEMBER_2 (op_greater, cptr, cstr),
        // operator> (const basic_string&, const basic_string&)
        NON_MEMBER_2 (op_greater, cstr, cstr),
        // operator> (const basic_string&, const_pointer)
        NON_MEMBER_2 (op_greater, cstr, cptr),

        //////////////////////////////////////////////////////////////
        // operator>= (const_pointer, const basic_string&)
        NON_MEMBER_2 (op_greater_equal, cptr, cstr),
        // operator>= (const basic_string&, const basic_string&)
        NON_MEMBER_2 (op_greater_equal, cstr, cstr),
        // operator>= (const basic_string&, const_pointer)
        NON_MEMBER_2 (op_greater_equal, cstr, cptr),

        //////////////////////////////////////////////////////////////
        // size () const
        MEMBER_0 (size, cstr),

        //////////////////////////////////////////////////////////////
        // length () const
        MEMBER_0 (length, cstr),

        //////////////////////////////////////////////////////////////
        // max_size () const
        MEMBER_0 (max_size, cstr),

        //////////////////////////////////////////////////////////////
        // resize (size_type, value_type)
        MEMBER_2 (resize, str, size, val),
        // resize (size_type)
        MEMBER_1 (resize, str, size),

        //////////////////////////////////////////////////////////////
        // capacity () const
        MEMBER_0 (capacity, cstr),

        //////////////////////////////////////////////////////////////
        // reserve (size_type)
        MEMBER_1 (reserve, str, size),
        // reserve ()
        MEMBER_0 (reserve, str),

        //////////////////////////////////////////////////////////////
        // clear ()
        MEMBER_0 (clear, str),

        //////////////////////////////////////////////////////////////
        // empty () const
        MEMBER_0 (empty, cstr),

        //////////////////////////////////////////////////////////////
        // begin ()
        MEMBER_0 (begin, str),
        // begin () const
        MEMBER_0 (begin_const, cstr),
        // end ()
        MEMBER_0 (end, str),
        // end () const
        MEMBER_0 (end_const, cstr),

        //////////////////////////////////////////////////////////////
        // rbegin ()
        MEMBER_0 (rbegin, str),
        // rbegin () const
        MEMBER_0 (rbegin_const, cstr),
        // rend ()
        MEMBER_0 (rend, str),
        // rend () const
        MEMBER_0 (rend_const, cstr),

        //////////////////////////////////////////////////////////////
        // c_str () const
        MEMBER_0 (c_str, cstr),
        // data () const
        MEMBER_0 (data, cstr),

        //////////////////////////////////////////////////////////////
        // get_allocator () const
        MEMBER_0 (get_allocator, cstr),

        //////////////////////////////////////////////////////////////
        // operator>> (istream&, basic_string&)
        NON_MEMBER_2 (extractor, istream, str),
        // operator<< (ostream&, const basic_string&)
        NON_MEMBER_2 (inserter, ostream, cstr),

        //////////////////////////////////////////////////////////////
        // getline (istream&, basic_string&)
        NON_MEMBER_2 (getline, istream, str),
        // getline (istream&, basic_string&, value_type)
        NON_MEMBER_3 (getline, istream, str, val)
    };

// clean up helper macros used above
#include <rw_sigdefs.h>

    static ArgId arg_type (OverloadId id, int argno) {
        return ArgId (((int (id) >> fid_bits) >> argno * arg_bits) & arg_mask);
    }
};

/**************************************************************************/

static const _RWSTD_SIZE_T
NPOS = _RWSTD_SIZE_MAX;
/*
#if ! defined (_STLP_STATIC_CONST_INIT_BUG) && !DRQS // Missing def for npos
_STLP_BEGIN_NAMESPACE
template <class _CharT, class _Traits, class _Alloc>
const size_t basic_string<_CharT, _Traits, _Alloc>::npos;
const size_t basic_string<char, char_traits<char>, allocator<char> >::npos;
const size_t basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >::npos;
_STLP_END_NAMESPACE
#endif
*/

/**************************************************************************/

struct StringFunc
{
    StringIds::CharId     char_id_;
    StringIds::TraitsId   traits_id_;
    StringIds::AllocId    alloc_id_;
    StringIds::IteratorId iter_id_;
    StringIds::OverloadId which_;
};


// describes a single test case for any overload of any string
// function (the same test case can be used to exercise more
// than one overload of the same function)
struct StringTestCase
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
    _RWSTD_SIZE_T  nres;      // length of sequence or expected result
                              // value for find, rfind, compare, etc

    int            bthrow;    // exception expected
};


// describes a set of test cases for a single overload of a function
struct StringTest
{
    // string function overload to exercise
    StringIds::OverloadId which;

    // test cases to exercise overload withh
    const StringTestCase *cases;

    // number of test cases
    _RWSTD_SIZE_T case_count;
};


// sets the {CLASS}, {FUNC}, {FUNCSIG}, and optionally {FUNCALL}
// environment variables as follows:
// CLASS:   the name of basic_string specialization
// FUNC:    the name of the basic_string function
// FUNCSIG: the name and signature of a specific overload
//          of the basic_string function
// FUNCALL: a string describing the call to the basic_string function
//          with function with function arguments expanded (as specified
//          by the TestCase argument)
_TEST_EXPORT void
rw_setvars (const StringFunc&, const StringTestCase* = 0);

typedef void StringTestFunc (const StringFunc&, const StringTestCase&);

_TEST_EXPORT int
rw_run_string_test (int, char**, const char*, const char*,
                    StringTestFunc*, const StringTest*, _RWSTD_SIZE_T);

typedef void VoidFunc ();

_TEST_EXPORT int
rw_run_string_test (int, char**, const char*, const char*,
                    VoidFunc* const*, const StringTest*, _RWSTD_SIZE_T);

/**************************************************************************/

template <class charT>
class StringTestCaseData
{
private:

    enum { BUFSIZE = 256 };

    // small buffers to avoid expensive dynamic memory allocation
    // in most test cases (will dynamically allocate sufficient
    // storage if necessary)
    charT str_buf_ [BUFSIZE];
    charT arg_buf_ [BUFSIZE];
    charT res_buf_ [BUFSIZE];

    // not defined, not copiable, not assignable
    StringTestCaseData (const StringTestCaseData&);
    void operator= (const StringTestCaseData&);

    // for convenience
    typedef _RWSTD_SIZE_T SizeType;

public:

    SizeType strlen_;   // the length of the expanded string
    SizeType arglen_;   // the length of the expanded argument
    SizeType reslen_;   // the length of the expanded result

    // the offset and extent (the number of elements) of
    // the first range into the string object being modified
    SizeType off1_;
    SizeType ext1_;

    // the offset and extent (the number of elements) of
    // the argument of the function call
    SizeType off2_;
    SizeType ext2_;

    const charT* const str_;   // pointer to the expanded string
    const charT* const arg_;   // pointer to the expanded argument
    const charT* const res_;   // pointer to the expanded result

    const StringFunc     &func_;
    const StringTestCase &tcase_;

    // converts the narrow (and possibly) condensed strings to fully
    // expanded wide character arrays that can be used to construct
    // basic_string objects
    StringTestCaseData (const StringFunc &func, const StringTestCase &tcase)
        : strlen_ (BUFSIZE), arglen_ (BUFSIZE), reslen_ (BUFSIZE),
          str_ (rw_expand (str_buf_, tcase.str, tcase.str_len, &strlen_)),
          arg_ (rw_expand (arg_buf_, tcase.arg, tcase.arg_len, &arglen_)),
          res_ (rw_expand (res_buf_, tcase.res, tcase.nres,    &reslen_)),
          func_ (func), tcase_ (tcase) {
        // compute the offset and extent of the string object
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

    ~StringTestCaseData () {
        // clean up dynamically allocated memory (if any)
        if (str_ != str_buf_)
            delete[] _RWSTD_CONST_CAST (charT*, str_);
        if (arg_ != arg_buf_)
            delete[] _RWSTD_CONST_CAST (charT*, arg_);
        if (res_ != res_buf_)
            delete[] _RWSTD_CONST_CAST (charT*, res_);
    }
};

/**************************************************************************/

// encapsulates the state of a string object without regard to type
// used in exception safety tests to determine changes to the state
// after a modifying operation throws an exception
struct _TEST_EXPORT StringState
{
    const void*   data_;
    _RWSTD_SIZE_T size_;
    _RWSTD_SIZE_T capacity_;

    // invokes rw_assert() to verify that two states are the same
    // returns 1 when the two states compare equal, and 0 otherwise
    int assert_equal (const StringState&, int, int, const char*) const;
};


// creates a StringState object from a basic_string
template <class String>
inline StringState
rw_get_string_state (const String &str)
{
    const StringState state = {
        str.data (), str.size (), str.capacity ()
    };

    return state;
}


/**************************************************************************/

// base class-functor for the range template overloads testing
template <class String>
struct RangeBase {

    typedef typename String::value_type                 StringChar;
    typedef typename String::pointer                    StringPtr;
    typedef typename String::const_pointer              StringConstPtr;
    typedef typename String::iterator                   StringIter;
    typedef typename String::const_iterator             StringConstIter;
    typedef typename String::reverse_iterator           StringRevIter;
    typedef typename String::const_reverse_iterator     StringConstRevIter;

    RangeBase () { }

    virtual ~RangeBase () { /* silence warnings */ }

    static StringPtr
    begin (String &str, StringPtr*) {
        return _RWSTD_CONST_CAST (StringPtr, str.data ());
    }

    static StringConstPtr
    begin (const String &str, StringConstPtr*) {
        return str.data ();
    }

#ifndef _RWSTD_NO_DEBUG_ITER

    // when debugging iterators are enabled string::iterator and
    // string::pointer are distinct types; otherwise they are the
    // same type

    static StringIter
    begin (String &str, StringIter*) {
        return str.begin ();
    }

    static StringConstIter
    begin (const String &str, StringConstIter*) {
        return str.begin ();
    }

#endif   // _RWSTD_NO_DEBUG_ITER

    static StringRevIter
    begin (String &str, StringRevIter*) {
        return str.rbegin ();
    }

    static StringConstRevIter
    begin (const String &str, StringConstRevIter*) {
        return str.rbegin ();
    }

    virtual String&
    operator() (String &str, const StringTestCaseData<StringChar>&) const {
        RW_ASSERT (!"logic error: should be never called");
        return str;
    }
};

/**************************************************************************/

#define Disabled(which)   \
    StringIds::opt_memfun_disabled [which & ~StringIds::fid_mask]


#ifndef _RWSTD_NO_WCHAR_T
#  define TEST_DISPATCH(Alloc, fname, func, tcase)              \
    if (StringIds::DefaultTraits == func.traits_id_) {          \
        if (StringIds::Char == func.char_id_)                   \
            fname (char (), (std::char_traits<char>*)0,         \
                   (Alloc<char>*)0, func, tcase);               \
        else if (StringIds::WChar == func.char_id_)             \
            fname (wchar_t (), (std::char_traits<wchar_t>*)0,   \
                   (Alloc<wchar_t>*)0, func, tcase);            \
        else                                                    \
            rw_note (0, 0, 0,                                   \
                     "%{$CLASS} tests not implemented");        \
    }                                                           \
    else {                                                      \
        if (StringIds::Char == func.char_id_)                   \
            fname (char (), (UserTraits<char>*)0,               \
                   (Alloc<char>*)0, func, tcase);               \
       else if (StringIds::WChar == func.char_id_)              \
            fname (wchar_t (), (UserTraits<wchar_t>*)0,         \
                   (Alloc<wchar_t>*)0, func, tcase);            \
       else                                                     \
           fname (UserChar (), (UserTraits<UserChar>*)0,        \
                  (Alloc<UserChar>*)0, func, tcase);            \
    }                                                           \
    (void)0

#else   // if defined (_RWSTD_NO_WCHAR_T)
#  define TEST_DISPATCH(Alloc, fname, func, tcase)              \
    if (StringIds::DefaultTraits == func.traits_id_) {          \
        if (StringIds::Char == func.char_id_)                   \
            fname (char (), (std::char_traits<char>*)0,         \
                   (Alloc<char>*)0, func, tcase);               \
        else if (StringIds::WChar == func.char_id_)             \
            RW_ASSERT (!"logic error: wchar_t disabled");       \
        else                                                    \
            rw_note (0, 0, 0,                                   \
                     "%{$CLASS} tests not implemented");        \
        }                                                       \
    }                                                           \
    else {                                                      \
        if (StringIds::Char == func.char_id_)                   \
            fname (char (), (UserTraits<char>*)0,               \
                   (Alloc<char>*)0, func, tcase);               \
        else if (StringIds::WChar == func.char_id_)             \
             RW_ASSERT (!"logic error: wchar_t disabled");      \
        else if (StringIds::UChar == func.char_id_)             \
            fname (UserChar (), (UserTraits<UserChar>*)0,       \
                   (Alloc<UserChar>*)0, func, tcase);           \
    }                                                           \
    (void)0

#endif   // _RWSTD_NO_WCHAR_T


#define DEFINE_STRING_TEST_DISPATCH(fname)                      \
    static void                                                 \
    fname (const StringFunc     &func,                          \
           const StringTestCase &tcase) {                       \
        if (StringIds::DefaultAlloc == func.alloc_id_) {        \
            TEST_DISPATCH (std::allocator, fname, func, tcase); \
        }                                                       \
        else if (StringIds::UserAlloc == func.alloc_id_) {      \
            TEST_DISPATCH (UserAlloc, fname, func, tcase);      \
        }                                                       \
        else                                                    \
            RW_ASSERT (!"logic error: bad allocator");          \
    } typedef void rw_unused_typedef


#define TFUNC(charT, Traits, Allocator)                 \
    void (*)(charT*, Traits<charT>*, Allocator<charT>*, \
             const StringTestCaseData<charT>&)

#define TFUNC_ADDR(fname, charT, Traits, Allocator)     \
    (VoidFunc*)(TFUNC (charT, Traits, Allocator))       \
        &fname<charT, Traits<charT>, Allocator<charT> >

#ifndef _RWSTD_NO_WCHAR_T
#  define DEFINE_STRING_TEST_FUNCTIONS(fname)                           \
    static VoidFunc* const fname ## _func_array [] = {                  \
      TFUNC_ADDR (fname, char, std::char_traits, std::allocator),       \
      TFUNC_ADDR (fname, char, std::char_traits, UserAlloc),            \
      TFUNC_ADDR (fname, char, UserTraits,       std::allocator),       \
      TFUNC_ADDR (fname, char, UserTraits,       UserAlloc),            \
                                                                        \
      TFUNC_ADDR (fname, wchar_t, std::char_traits, std::allocator),    \
      TFUNC_ADDR (fname, wchar_t, std::char_traits, UserAlloc),         \
      TFUNC_ADDR (fname, wchar_t, UserTraits,       std::allocator),    \
      TFUNC_ADDR (fname, wchar_t, UserTraits,       UserAlloc),         \
                                                                        \
      (VoidFunc*)0, /* std::char_traits<UserChar> not allowed */        \
      (VoidFunc*)0, /* std::char_traits<UserChar> not allowed */        \
      TFUNC_ADDR (fname, UserChar, UserTraits, std::allocator),         \
      TFUNC_ADDR (fname, UserChar, UserTraits, UserAlloc)               \
    }

#else   // if defined (_RWSTD_NO_WCHAR_T)
#  define DEFINE_STRING_TEST_FUNCTIONS(fname)                           \
    static VoidFunc* const fname ## _func_array [] = {                  \
      TFUNC_ADDR (fname, char, std::char_traits, std::allocator),       \
      TFUNC_ADDR (fname, char, std::char_traits, UserAlloc),            \
      TFUNC_ADDR (fname, char, UserTraits,       std::allocator),       \
      TFUNC_ADDR (fname, char, UserTraits,       UserAlloc),            \
                                                                        \
      (VoidFunc*)0, /* wchar_t disabled */                              \
      (VoidFunc*)0, /* wchar_t disabled */                              \
      (VoidFunc*)0, /* wchar_t disabled */                              \
      (VoidFunc*)0, /* wchar_t disabled */                              \
                                                                        \
      (VoidFunc*)0, /* std::char_traits<UserChar> not allowed */        \
      (VoidFunc*)0, /* std::char_traits<UserChar> not allowed */        \
      TFUNC_ADDR (fname, UserChar, UserTraits, std::allocator),         \
      TFUNC_ADDR (fname, UserChar, UserTraits, UserAlloc)               \
    }

#endif   // _RWSTD_NO_WCHAR_T


#endif   // RW_21_STRINGS_H_INCLUDED
