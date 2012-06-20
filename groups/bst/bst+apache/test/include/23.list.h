/************************************************************************
*
* 23.list.h - definitions of helpers used in clause 23.list tests
*
* $Id: 23.list.h
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

#ifndef RW_23_LIST_H_INCLUDED
#define RW_23_LIST_H_INCLUDED

#include <23.containers.h>
#include <driver.h>         // for rw_assert()
#include <rw_sigdefs.h>     // for helper macros

/**************************************************************************/

// defines enumerations identifying list template arguments,
// sets of overloaded functions, member types used in the declarations
// of their signatures, and specific overloads of such member functions
struct ListIds : ContainerIds
{
    // unique identifiers for all overloads of each member function
    //     6 bits for FuncId
    // 6 * 4 bits for ArgId (at most 6 arguments including this)
    //     1 bit for membership
    enum OverloadId {
        //////////////////////////////////////////////////////////////
        // list ()
        MEMBER_0 (ctor, cont),
        // list (const allocator_type&)
        MEMBER_1 (ctor, cont, alloc),
        // list (const list&)
        MEMBER_1 (ctor, cont, ccont),
        // list (size_type)
        MEMBER_1 (ctor, cont, size),
        // list (size_type, const value_type&)
        MEMBER_2 (ctor, cont, size, cref),
        // list (size_type, const value_type&, const allocator_type&)
        MEMBER_3 (ctor, cont, size, cref, alloc),
        // list (InputIterator, InputIterator)
        MEMBER_1 (ctor, cont, range),
        // list (InputIterator, InputIterator, const allocator&)
        MEMBER_2 (ctor, cont, range, alloc),

        //////////////////////////////////////////////////////////////
        // operator= (const list&)
        MEMBER_1 (op_set, cont, ccont),

        //////////////////////////////////////////////////////////////
        // assign (size_type, const value_type&)
        MEMBER_2 (assign, cont, size, cref),
        // assign (InputIterator, InputIterator)
        MEMBER_1 (assign, cont, range),

        //////////////////////////////////////////////////////////////
        // get_allocator () const
        MEMBER_0 (get_allocator, ccont),

        //////////////////////////////////////////////////////////////
        // begin ()
        MEMBER_0 (begin, cont),
        // begin () const
        MEMBER_0 (begin_const, ccont),
        // end ()
        MEMBER_0 (end, cont),
        // end () const
        MEMBER_0 (end_const, ccont),

        //////////////////////////////////////////////////////////////
        // rbegin ()
        MEMBER_0 (rbegin, cont),
        // rbegin () const
        MEMBER_0 (rbegin_const, ccont),
        // rend ()
        MEMBER_0 (rend, cont),
        // rend () const
        MEMBER_0 (rend_const, ccont),

        //////////////////////////////////////////////////////////////
        // empty ()
        MEMBER_0 (empty, ccont),

        //////////////////////////////////////////////////////////////
        // size ()
        MEMBER_0 (size, ccont),

        //////////////////////////////////////////////////////////////
        // max_size ()
        MEMBER_0 (max_size, ccont),

        //////////////////////////////////////////////////////////////
        // resize (size_type)
        MEMBER_1 (resize, cont, size),
        // resize (size_type, value_type)
        MEMBER_2 (resize, cont, size, val),

        //////////////////////////////////////////////////////////////
        // front ()
        MEMBER_0 (front, cont),
        // front () const
        MEMBER_0 (front_const, ccont),
        // back ()
        MEMBER_0 (back, cont),
        // back () const
        MEMBER_0 (back_const, ccont),

        //////////////////////////////////////////////////////////////
        // push_front (const value_type&)
        MEMBER_1 (push_front, cont, cref),
        // pop_front ()
        MEMBER_0 (pop_front, cont),
        // push_back (const value_type&)
        MEMBER_1 (push_back, cont, cref),
        // pop_back ()
        MEMBER_0 (pop_back, cont),

        //////////////////////////////////////////////////////////////
        // insert (iterator, const value_type&)
        MEMBER_2 (insert, cont, iter, cref),
        // insert (iterator, size_type, const value_type&)
        MEMBER_3 (insert, cont, iter, size, cref),
        // insert (iterator, InputIterator, InputIterator)
        MEMBER_2 (insert, cont, iter, range),

        //////////////////////////////////////////////////////////////
        // erase (iterator)
        MEMBER_1 (erase, cont, iter),
        // erase (iterator, iterator)
        MEMBER_2 (erase, cont, iter, iter),

        //////////////////////////////////////////////////////////////
        // swap (list&)
        MEMBER_1 (swap, cont, cont),
        // swap (list&, list&)
        NON_MEMBER_2 (swap, cont, cont),

        //////////////////////////////////////////////////////////////
        // clear ()
        MEMBER_0 (clear, cont),

        //////////////////////////////////////////////////////////////
        // splice (iterator, list&)
        MEMBER_2 (splice, cont, iter, cont),
        // splice (iterator, list&, iterator)
        MEMBER_3 (splice, cont, iter, cont, iter),
        // splice (iterator, list&, iterator, iterator)
        MEMBER_4 (splice, cont, iter, cont, iter, iter),

        //////////////////////////////////////////////////////////////
        // remove (const value_type&)
        MEMBER_1 (remove, cont, cref),
        // remove_if (Predicate)
        MEMBER_1 (remove_if, cont, pred),

        //////////////////////////////////////////////////////////////
        // unique (BinaryPredicate)
        MEMBER_1 (unique, cont, bpred),

        //////////////////////////////////////////////////////////////
        // merge (list&)
        MEMBER_1 (merge, cont, cont),
        // merge (list&, Compare)
        MEMBER_2 (merge, cont, cont, comp),

        //////////////////////////////////////////////////////////////
        // sort ()
        MEMBER_0 (sort, cont),
        // sort (Compare)
        MEMBER_1 (sort, cont, comp),

        //////////////////////////////////////////////////////////////
        // reverse ()
        MEMBER_0 (reverse, cont),

        //////////////////////////////////////////////////////////////
        // operator== (const list&, const list&)
        NON_MEMBER_2 (op_equal, ccont, ccont),

        //////////////////////////////////////////////////////////////
        // operator< (const list&, const list&)
        NON_MEMBER_2 (op_less, ccont, ccont),

        //////////////////////////////////////////////////////////////
        // operator!= (const list&, const list&)
        NON_MEMBER_2 (op_not_equal, ccont, ccont),

        //////////////////////////////////////////////////////////////
        // operator> (const list&, const list&)
        NON_MEMBER_2 (op_greater, ccont, ccont),

        //////////////////////////////////////////////////////////////
        // operator>= (const list&, const list&)
        NON_MEMBER_2 (op_greater_equal, ccont, ccont),

        //////////////////////////////////////////////////////////////
        // operator<= (const list&, const list&)
        NON_MEMBER_2 (op_less_equal, ccont, ccont)
    };

// clean up helper macros used above
#include <rw_sigdefs.h>

};

/**************************************************************************/

template <class InputIter1, class InputIter2>
inline bool
_rw_equal (InputIter1 first1, InputIter1 last1, InputIter2 first2)
{
    for (; first1 != last1 && *first1 == *first2; ++first1, ++first2) ;

    return first1 == last1;
}

// encapsulates the state of a list object
// used in exception safety tests to determine changes to the state
// after a modifying operation throws an exception
template <class List>
struct ListState
{
    typedef typename List::const_iterator  ListCIter;
    typedef typename List::const_pointer   ListCPtr;

    _RWSTD_SIZE_T size_;
    ListCIter*    iters_;
    ListCPtr*     ptrs_;

    ListState (List const & lst) : size_ (lst.size ()), iters_ (0), ptrs_ (0) {
        iters_ = new ListCIter [size_];
        ptrs_  = new ListCPtr  [size_];

        _RWSTD_SIZE_T index = 0;
        for (ListCIter it = lst.begin (), end = lst.end ();
            it != end; ++it, ++index) {

            iters_ [index] = it;
            ptrs_  [index] = &*it;
        }
    }

    ~ListState () {
        delete [] iters_;
        delete [] ptrs_;
    }

    // invokes rw_assert() to verify that two states are the same
    void assert_equal (const ListState& state, int line,
                       int case_line, const char* when) const {

        const bool equal =    size_ == state.size_
                           && _rw_equal (iters_, iters_ + size_, state.iters_)
                           && _rw_equal (ptrs_,  ptrs_ + size_,  state.ptrs_);

        rw_assert (equal, 0, case_line,
                   "line %d: %{$FUNCALL}: object state unexpectedly changed "
                   "after %s", line, when);
    }
};

/**************************************************************************/

#endif   // RW_23_LIST_H_INCLUDED
