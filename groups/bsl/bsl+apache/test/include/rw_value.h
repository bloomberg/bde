/***************************************************************************
 *
 * rw_value.h - defines a User Defined Class type and User Defined POD type
 *
 * $Id: rw_value.h 510970 2007-02-23 14:57:45Z faridz $
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
 * Copyright 1994-2005 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef RW_VALUE_H_INCLUDED
#define RW_VALUE_H_INCLUDED

#include <testdefs.h>
#include <alg_test.h>         // for conv_to_bool

/**************************************************************************/

struct UserData
{
    int       val_;       // object's value
};

/**************************************************************************/

struct _TEST_EXPORT UserPOD
{
    UserData data_;

    static UserPOD
    from_char (char c)
    {
        UserPOD x;
        x.data_.val_ = c;
        return x;
    }

    // construct an array of objects of type UserPOD each initialized
    // from the corresponding element of the character array
    // when the last argument is true and the character array
    // is not sorted in ascending order the function fails by
    // returning 0
    static UserPOD*
    from_char (const char*, _RWSTD_SIZE_T = _RWSTD_SIZE_MAX,
               bool = false);

    // returns a pointer to the first element in the sequence of UserClass
    // whose value is not equal to the corresponding element of
    // the character string or 0 when no such element exists
    static const UserPOD* mismatch (const UserPOD*, const char*,
                                    _RWSTD_SIZE_T /* len */ = _RWSTD_SIZE_MAX);
};

/**************************************************************************/

// objects of class UserClass maintain a count of their instances in existence,
// the number of defaut and copy ctor calls, assignment operators, and
// the number of calls to operator==() and operator<()
struct _TEST_EXPORT UserClass
{
    UserData   data_;

    const int  id_;        // a unique non-zero id of the object
    int        origin_;    // id of the original object that this
                           // is a (perhaps indirect) copy of (id_
                           // when this is the original)
    int        src_id_;    // id of the object that this is a direct
                           // copy of (id_ when this the original)

    // number of times the object has been copied into another object,
    // regardless of whether the operation threw an exception or not
    _RWSTD_SIZE_T n_copy_ctor_;

    // number of times the object's assignment operator has been invoked,
    // regardless of whether the operation threw an exception or not
    _RWSTD_SIZE_T n_op_assign_;

    // number of times the object's operator+=() has been invoked,
    // regardless of whether the operation threw an exception or not
    _RWSTD_SIZE_T n_op_plus_assign_;

    // number of times the object's operator-=() has been invoked,
    // regardless of whether the operation threw an exception or not
    _RWSTD_SIZE_T n_op_minus_assign_;

    // number of times the object's operator*=() has been invoked,
    // regardless of whether the operation threw an exception or not
    _RWSTD_SIZE_T n_op_times_assign_;

    // number of times the object's operator/=() has been invoked,
    // regardless of whether the operation threw an exception or not
    _RWSTD_SIZE_T n_op_div_assign_;

    // number of times the object's operator== was invoked
    // regardless of whether the operation threw an exception
    _RWSTD_SIZE_T n_op_eq_;

    // number of times the object's operator< was invoked
    // regardless of whether the operation threw an exception
    _RWSTD_SIZE_T n_op_lt_;

    static _RWSTD_SIZE_T count_;   // number of objects in existence (>= 0)
    static int id_gen_;            // generates a unique non-zero id
    static int (*gen_)();          // extern "C++" int (*)()

    static _RWSTD_SIZE_T n_total_def_ctor_;    // number of default ctor calls
    static _RWSTD_SIZE_T n_total_copy_ctor_;   // ... copy ctors ...
    static _RWSTD_SIZE_T n_total_dtor_;        // ... dtors ...
    static _RWSTD_SIZE_T n_total_op_assign_;   // ... assignment operators ...
    static _RWSTD_SIZE_T n_total_op_plus_assign_;    // ... operator+=
    static _RWSTD_SIZE_T n_total_op_minus_assign_;   // ... operator-=
    static _RWSTD_SIZE_T n_total_op_times_assign_;   // ... operator*=
    static _RWSTD_SIZE_T n_total_op_div_assign_;     // ... operator/=
    static _RWSTD_SIZE_T n_total_op_eq_;       // ... equality operators ...
    static _RWSTD_SIZE_T n_total_op_lt_;       // ... operators <= ...

    // classes thrown from the respective functions
    struct Exception { int id_; };
    struct DefCtor: Exception { };
    struct CopyCtor: Exception { };
    struct Dtor: Exception { };
    struct OpAssign: Exception { };
    struct OpPlusAssign: Exception { };
    struct OpMinusAssign: Exception { };
    struct OpTimesAssign: Exception { };
    struct OpDivAssign: Exception { };
    struct OpEq: Exception { };
    struct OpLt: Exception { };

    // throw object's `id' wrapped in the appropriate struct when the
    // corresponding n_total_xxx_ counter reaches the value pointed to
    // by the respective pointer below
    static _RWSTD_SIZE_T* def_ctor_throw_ptr_;
    static _RWSTD_SIZE_T* copy_ctor_throw_ptr_;
    static _RWSTD_SIZE_T* dtor_throw_ptr_;
    static _RWSTD_SIZE_T* op_assign_throw_ptr_;
    static _RWSTD_SIZE_T* op_plus_assign_throw_ptr_;
    static _RWSTD_SIZE_T* op_minus_assign_throw_ptr_;
    static _RWSTD_SIZE_T* op_times_assign_throw_ptr_;
    static _RWSTD_SIZE_T* op_div_assign_throw_ptr_;
    static _RWSTD_SIZE_T* op_eq_throw_ptr_;
    static _RWSTD_SIZE_T* op_lt_throw_ptr_;

    // objects to which the pointers above initally point
    static _RWSTD_SIZE_T def_ctor_throw_count_;
    static _RWSTD_SIZE_T copy_ctor_throw_count_;
    static _RWSTD_SIZE_T dtor_throw_count_;
    static _RWSTD_SIZE_T op_assign_throw_count_;
    static _RWSTD_SIZE_T op_plus_assign_throw_count_;
    static _RWSTD_SIZE_T op_minus_assign_throw_count_;
    static _RWSTD_SIZE_T op_times_assign_throw_count_;
    static _RWSTD_SIZE_T op_div_assign_throw_count_;
    static _RWSTD_SIZE_T op_eq_throw_count_;
    static _RWSTD_SIZE_T op_lt_throw_count_;

    UserClass ();

    UserClass (const UserClass&);

    ~UserClass ();

    UserClass& operator= (const UserClass&);
    UserClass& operator+= (const UserClass&);
    UserClass& operator-= (const UserClass&);
    UserClass& operator*= (const UserClass&);
    UserClass& operator/= (const UserClass&);

    bool operator== (const UserClass&) const;
    bool operator< (const UserClass&) const;

    // the following operators are not declared or defined in order
    // to detect any unwarranted assumptions made in algorithms
    //    bool operator!= (const UserClass &rhs) const;
    //    bool operator> (const UserClass &rhs) const;
    //    bool operator>= (const UserClass &rhs) const;
    //    bool operator<= (const UserClass &rhs) const;
    //    UserClass operator- () const;
    //    UserClass operator+ () const;
    
    bool
    is_count (_RWSTD_SIZE_T copy_ctor,
              _RWSTD_SIZE_T op_assign,
              _RWSTD_SIZE_T op_eq,
              _RWSTD_SIZE_T op_lt) const;

    static bool
    is_total (_RWSTD_SIZE_T count,
              _RWSTD_SIZE_T n_def_ctor,
              _RWSTD_SIZE_T n_copy_ctor,
              _RWSTD_SIZE_T n_op_assign,
              _RWSTD_SIZE_T n_op_eq,
              _RWSTD_SIZE_T n_op_lt);

    // returns a pointer to the first element in the sequence whose value
    // is less than the value of the immediately preceding element, or 0
    // when no such element exists
    static const UserClass*
    first_less (const UserClass*, _RWSTD_SIZE_T);

    static void reset_totals ();

    static UserClass
    from_char (char c)
    {
        UserClass x;
        x.data_.val_ = c;
        return x;
    }

    // construct an array of objects of type UserClass each initialized
    // from the corresponding element of the character array
    // when the last argument is true and the character array
    // is not sorted in ascending order the function fails by
    // returning 0
    static UserClass*
    from_char (const char*, _RWSTD_SIZE_T = _RWSTD_SIZE_MAX,
               bool = false);

    // returns -1 when less, 0 when same, or +1 when the array
    // of UserClass objects is greater than the character string
    static int compare (const UserClass*, const char*,
                        _RWSTD_SIZE_T = _RWSTD_SIZE_MAX);
    static int compare (const char*, const UserClass*,
                        _RWSTD_SIZE_T = _RWSTD_SIZE_MAX);

    // returns -1 when less, 0 when same, or +1 when the first
    // array of UserClass objects is greater than the second array
    static int compare (const UserClass*, const UserClass*, _RWSTD_SIZE_T);

    // returns a pointer to the first element in the sequence of UserClass
    // whose value is not equal to the corresponding element of
    // the character string or 0 when no such element exists
    static const UserClass* mismatch (const UserClass*, const char*,
                                      _RWSTD_SIZE_T);

    struct Less;

private:

    enum assign_op {
        op_assign, op_plus_assign, op_minus_assign,
        op_times_assign, op_div_assign
    };

    void assign (assign_op, const UserClass&);
};

/**************************************************************************/

struct _TEST_EXPORT UnaryPredicate
{
    // total number of times operator() was invoked
    static _RWSTD_SIZE_T n_total_op_fcall_;

    UnaryPredicate ();

    UnaryPredicate (const UnaryPredicate&);

    UnaryPredicate& operator= (const UnaryPredicate&);

    virtual ~UnaryPredicate ();

    virtual conv_to_bool operator()(const UserClass&) const;
};


struct _TEST_EXPORT BinaryPredicate
{
    // total number of times operator() was invoked
    static _RWSTD_SIZE_T n_total_op_fcall_;

    enum binary_op {
        op_equals,
        op_not_equals,
        op_less,
        op_less_equal,
        op_greater,
        op_greater_equal
    };

    BinaryPredicate (binary_op);

    virtual ~BinaryPredicate ();

    virtual conv_to_bool operator()(const UserClass&,
                                    const UserClass&) /* non-const */;

private:

    // not assignable
    void operator= (const BinaryPredicate&);

    binary_op op_;
};


struct UserClass::Less: BinaryPredicate
{
    // dummy arguments provided to prevent the class from being
    // default constructible and implicit conversion from int
    Less (int /* dummy */, int /* dummy */)
        : BinaryPredicate (BinaryPredicate::op_less) { /* no-op */ }
};


static const struct _TEST_EXPORT UserClassFmatInit {
    UserClassFmatInit ();
} _rw_user_class_fmat_init;


#endif   // RW_VALUE_H_INCLUDED
