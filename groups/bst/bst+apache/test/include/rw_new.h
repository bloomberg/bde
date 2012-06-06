/************************************************************************
 *
 * rw_new.h - definitions of replacement operator new and delete
 *
 * $Id: rw_new.h 590052 2007-10-30 12:44:14Z faridz $
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
 * Copyright 2004-2006 Rogue Wave Software.
 *
 **************************************************************************/

#ifndef RW_NEW_H_INCLUDED
#define RW_NEW_H_INCLUDED


// this file must be #included in at most one translation unit in a program
// (replacement operators new and delete must be defined in at most one
// translation unit in order not to violate the ODR)

#include <new>          // for bad_alloc

#include <testdefs.h>   // for test config macros


struct rwt_free_store
{
    // cumulative number of all calls to the ordinary operator new
    // and the array form of the operator, respectively, regardless
    // of whether they exited successfully or by throwing an exception
    _RWSTD_SIZE_T new_calls_ [2];

    // cumulative number of calls to the ordinary operator delete
    // and the array form of the operator, respectively, regardless
    // of whether they exited successfully or by throwing an exception
    _RWSTD_SIZE_T delete_calls_ [2];

    // cumulative number of calls to the ordinary operator delete
    // and the array form of the operator, respectively, with the
    // argument of 0
    _RWSTD_SIZE_T delete_0_calls_ [2];

    // number of blocks currently allocated by the ordinary operator new,
    // and the array form of the operator, respectively
    _RWSTD_SIZE_T blocks_ [2];

    // number of bytes currently allocated by the ordinary operator new,
    // and the array form of the operator, respectively
    _RWSTD_SIZE_T bytes_ [2];

    // the maximum number of blocks allocated so far by the ordinary
    // operator new, and the array form of the operator, respectively
    _RWSTD_SIZE_T max_blocks_ [2];

    // the maximum total amount of storage allocated so far by the ordinary
    // operator new, and the array form of the operator, respectively
    _RWSTD_SIZE_T max_bytes_ [2];

    // the size of the largest block allocated so far by the ordinary
    // operator new, and the array form of the operator, respectively
    _RWSTD_SIZE_T max_block_size_ [2];

    // pointer to a value which, when equal to new_calls_ [i],
    // the ordinary operator new (for i == 0) or the array form
    // of the operator (for i == 1), respectively, will throw
    // a std::bad_alloc exception
    _RWSTD_SIZE_T* throw_at_calls_ [2];

    // pointer to a value which, when less than or equal to blocks_ [i]
    // during the next call to the ordinary operator new (for i == 0) or
    // the array form of the operator (for i == 1), respectively, will
    // throw a std::bad_alloc exception
    _RWSTD_SIZE_T* throw_at_blocks_ [2];

    // pointer to a value which, when less than or equal to bytes_ [i]
    // during the next call to the ordinary operator new (for i == 0) or
    // the array form of the operator (for i == 1), respectively, will
    // throw a std::bad_alloc exception
    _RWSTD_SIZE_T* throw_at_bytes_ [2];

    // pointer to a value which, when equal to the next block's sequence
    // number operator new will break or abort
    _RWSTD_SIZE_T* break_at_seqno_;
};


// returns a pointer to the global rwt_free_store object
// with a non-zero argument sets the global pointer to the rwt_free_store
// object to the value of the argument
_TEST_EXPORT rwt_free_store*
rwt_get_free_store (rwt_free_store*);

// computes the difference between two states of the free store
// returns 0 when no difference exists, otherwise a pointer to
// a rwt_free_store structure describing the differences
// when both arguments are 0, returns  the difference between
// the last checkpoint and the current states of the free store,
// and establishes a new checkpoint
// when the first argument is 0, returns  the difference between
// the last checkpoint and the current states of the free store
// when the second argument is 0, returns the difference between
// the state specified by the first argument and the current state
// of the free store
_TEST_EXPORT rwt_free_store*
rwt_checkpoint (const rwt_free_store*, const rwt_free_store*);

// returns the number of blocks allocated and not freed since
// the checkpoint specified by the second argument
// when the second argument is 0, returns the number of blocks
// allocated and not freed since the last established checkpoint
// and establishes a new checkpoint
_TEST_EXPORT _RWSTD_SIZE_T
rwt_check_leaks (_RWSTD_SIZE_T*, const rwt_free_store*);


// define replacement operator new and delete to keep track
// of allocated memory and allow for exceptions to be thrown

_TEST_EXPORT void* operator_new (_RWSTD_SIZE_T, bool);
_TEST_EXPORT void  operator_delete (void*, bool);

#  ifndef _RWSTD_BAD_ALLOC
     // #define if not #defined by <new> (SunPro #includes its
     // own <new> regardless of the preprocessor search path)
#    define _RWSTD_BAD_ALLOC _APACHE_STD::bad_alloc
#  endif   // _RWSTD_BAD_ALLOC


struct _TEST_EXPORT MyNewInit
{
    MyNewInit ();
    ~MyNewInit ();

private:

    // not defined
    MyNewInit (const MyNewInit&);
    void operator= (const MyNewInit&);
};

// keeps track of dynamic intiatlization
static MyNewInit mynew_init_tracker;


#  ifndef _RWSTD_TEST_SRC
#    ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

// prevent defining the replacement operator
// when compiling in the test suite framework

void* operator new (_RWSTD_SIZE_T n) _THROWS ((_RWSTD_BAD_ALLOC))
{
    return operator_new (n, false);
}

void operator delete (void *ptr) _THROWS (())
{
    operator_delete (ptr, false);
}


#      if    !defined (_RWSTD_NO_OPERATOR_NEW_ARRAY) \
          || defined (_RWSTD_NO_EXT_OPERATOR_NEW)

// replaceable only if we don't provide a definition in <new>
void* operator new[] (_RWSTD_SIZE_T n) _THROWS ((_RWSTD_BAD_ALLOC))
{
    return operator_new (n, true);
}

#      endif   // !_RWSTD_NO_OPERATOR_NEW_ARRAY || _RWSTD_NO_EXT_OPERATOR_NEW

#      if   !defined (_RWSTD_NO_OPERATOR_DELETE_ARRAY) \
          || defined (_RWSTD_NO_EXT_OPERATOR_NEW)

// replaceable only if we don't provide a definition in <new>
void operator delete[] (void *ptr) _THROWS (())
{
    operator_delete (ptr, true);
}


#      endif   // !_NO_OPERATOR_DELETE_ARRAY || _NO_EXT_OPERATOR_NEW
#    endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE
#  endif   // !_RWSTD_TEST_SRC
#endif   // RW_NEW_H_INCLUDED
