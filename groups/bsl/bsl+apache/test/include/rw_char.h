/**************************************************************************
 *
 * rw_char.h - defines a User Defined Character type and its traits
 *
 * $Id: rw_char.h 448928 2006-09-22 13:43:18Z faridz $
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

#ifndef RW_CHAR_INCLUDED
#define RW_CHAR_INCLUDED


// avoid dependencies on any library headers
// to prevent the pollution of the namespace(s)
#include <rw/_traits.h>
#include <testdefs.h>
#include <bsls_nativestd.h>

struct UserChar   // user-defined character type (must be POD)
{
#if    !defined (_RWSTD_NO_LONG_DOUBLE) \
    && !defined (__SUNPRO_CC) || __SUNPRO_CC > 0x540
    long double f;    // exercise correct alignment
#else
    // cannot use long double with SunPro due to a compiler
    // bug that prevents assignments of UserChar() (PR #28328)
    double f;
#endif   // _RWSTD_NO_LONG_DOUBLE

    unsigned char c;   // underlying character representation

    static UserChar eos () {
        // use UserChar::eos() instead of UserChar() to work around broken
        // compilers (e.g., MSVC 6) that do not zero out POD structs
        // on default construction
        const UserChar tmp = { 0, 0 };
        return tmp;
    }

    // definition of a POD-struct (9, p4):
    // A POD-struct is an aggregate class that has no non-static data
    // members of type pointer to member, non-POD-struct, non-POD-union
    // (or array of such types) or reference, and has no user-defined
    // copy assign-ment operator and no user-defined destructor.

    // definition of an aggregate (8.5.1, p1):
    // An aggregate is an array or a class (clause 9) with no user-declared
    // constructors (12.1), no private or protected non-static data members
    // (clause 11), no base classes (clause 10), and no virtual functions
    // (10.3).
};


inline UserChar make_char (char c, UserChar*)
{
    const UserChar ch = { 0.0, c };
    return ch;
}

inline char make_char (char c, char*)
{
    return c;
}

#ifndef _RWSTD_NO_WCHAR_T

inline wchar_t make_char (char c, wchar_t*)
{
    typedef unsigned char UChar;
    return wchar_t (UChar (c));
}

#endif   // _RWSTD_NO_WCHAR_T

///////////////////////////////////////////////////////////////////////////
// 21.1.2, p2 of C++ 03:
//
//   typedef INT_T int_type;
//
// -2- Requires: For a certain character container type char_type, a related
//     container type INT_T shall be a type or class which can represent all
//     of the valid characters converted from the corresponding char_type
//     values, as well as an end-of-file value, eof(). The type int_type
//     represents a character container type which can hold end-of-file to
//     be used as a return type of the iostream class member functions.224)
// ____________
// Footnote 224: If eof() can be held in char_type then some iostreams
//               operations may give surprising results.

class UserInt
{
    void* ptr_;   // to check validity
    int   i_;     // underlying int representation

    // valid values are 0 through 256 with 256 representing EOF
public:

    // default ctor creates a valid object but assigns it an invalid
    // value so that the only legal operations on it are assignment
    // and copy
    UserInt (): ptr_ (&i_ /* valid */), i_ (-1 /* invalid */) { /* empty */ }

    UserInt (const UserInt &rhs)
        : ptr_ (&i_), i_ (rhs.i_) {
        // verify rhs is valid
        // const_cast used to avoid MSVC 7.0 error C2446:
        // '==' : no conversion from 'const int *' to 'void *const '
        RW_ASSERT (_RWSTD_CONST_CAST (const void*, rhs.ptr_) == &rhs.i_);
        RW_ASSERT (-1 <= rhs.i_  && rhs.i_ < 257);   // i may be invalid
    }

    // (almost) assignable; i.e., returns void instead of UserInt&
    // for extra robustness
    void operator= (const UserInt &rhs) {
        RW_ASSERT (ptr_     == &i_);                // verify *this is valid
        // verify rhs is valid
        RW_ASSERT (_RWSTD_CONST_CAST (const void*, rhs.ptr_) == &rhs.i_);
        RW_ASSERT (-1 <= i_     && i_     < 257);   // i may be invalid
        RW_ASSERT (-1 <  rhs.i_ && rhs.i_ < 257);   // rhs.i must ve valid

        i_ = rhs.i_;
    }

    static UserInt eof () {
        UserInt tmp;
        tmp.i_ = 256;   // EOF
        return tmp;
    }

    static UserInt from_char (const UserChar &c) {
        UserInt tmp;
        tmp.i_ = int (c.c);
        return tmp;
    }

    UserChar to_char () const {
        // working around an Intel C++ 9.1 bug #380635
        /* const */ UserChar tmp /* = { 0, i_ }*/;
        tmp.f = 0;
        tmp.c = (unsigned char)(i_);
        return tmp;
    }

    ~UserInt () {
        RW_ASSERT (ptr_ == &i_);                // verify *this is valid
        RW_ASSERT ((-1 <= i_) == (i_ < 257));   // i may be invalid

        i_   = _RWSTD_INT_MIN;   // invalidate
        ptr_ = &ptr_;            // invalidate
    }

    bool equal (const UserInt &rhs) const {
        // verify *this is valid
        RW_ASSERT (_RWSTD_CONST_CAST (const void*, ptr_) == &i_);
        // verify rhs is valid
        RW_ASSERT (_RWSTD_CONST_CAST (const void*, rhs.ptr_) == &rhs.i_);
        RW_ASSERT (-1 < i_     && i_     < 257);   // i must ve valid
        RW_ASSERT (-1 < rhs.i_ && rhs.i_ < 257);   // rhs.i must be valid

        return i_ == rhs.i_;
    }
};


// user-defined character traits template
// (declared but not defined)
template <class charT>
struct UserTraits;


struct TraitsMemFunc
{
    enum {
        assign, eq, lt, compare, length, find, copy, move,
        assign2, not_eof, to_char_type, to_int_type, eq_int_type,
        eof,
        n_funs
    };
};


// required specialization on char
_RWSTD_SPECIALIZED_CLASS
struct _TEST_EXPORT UserTraits<char>: native_std::char_traits<char>
{
    typedef native_std::char_traits<char> Base;
    typedef Base::char_type        char_type;
    typedef Base::int_type         int_type;
    typedef TraitsMemFunc          MemFun;

    // avoid any dependency on the library
    typedef int                   off_type;     // std::streamoff
    typedef int                   state_type;   // std::mbstate_t
    typedef native_std::fpos<state_type> pos_type;     // std::fpos<state_type>

    static void
    assign (char_type&, const char_type&);

    static bool
    eq (const char_type&, const char_type&);

    static bool
    lt (const char_type&, const char_type&);

    static int
    compare (const char_type*, const char_type*, _RWSTD_SIZE_T);

    static _RWSTD_SIZE_T
    length (const char_type*);

    static const char_type*
    find (const char_type*, _RWSTD_SIZE_T, const char_type&);

    static char_type*
    copy (char_type*, const char_type*, _RWSTD_SIZE_T);

    static char_type*
    move (char_type*, const char_type*, _RWSTD_SIZE_T);

    static char_type*
    assign (char_type*, _RWSTD_SIZE_T, char_type);

    static int_type
    not_eof (const int_type&);

    static char_type
    to_char_type (const int_type&);

    static int_type
    to_int_type (const char_type&);

    static bool
    eq_int_type (const int_type&, const int_type&);

    static int_type
    eof ();

    static _RWSTD_SIZE_T n_calls_ [];

    static int_type eof_;

private:

    // not defined to detect bad assumptions made by the library
    UserTraits ();
    ~UserTraits ();
    void operator= (UserTraits&);
};


#ifndef _RWSTD_NO_WCHAR_T

// required specialization on wchar_t
_RWSTD_SPECIALIZED_CLASS
struct _TEST_EXPORT UserTraits<wchar_t>: native_std::char_traits<wchar_t>
{
    typedef native_std::char_traits<wchar_t> Base;
    typedef Base::char_type           char_type;
    typedef Base::int_type            int_type;
    typedef TraitsMemFunc             MemFun;

    // avoid any dependency on the library
    typedef int                   off_type;     // std::streamoff
    typedef int                   state_type;   // std::mbstate_t
    typedef native_std::fpos<state_type> pos_type;     // std::fpos<state_type>

    static void
    assign (char_type&, const char_type&);

    static bool
    eq (const char_type&, const char_type&);

    static bool
    lt (const char_type&, const char_type&);

    static int
    compare (const char_type*, const char_type*, _RWSTD_SIZE_T);

    static _RWSTD_SIZE_T
    length (const char_type*);

    static const char_type*
    find (const char_type*, _RWSTD_SIZE_T, const char_type&);

    static char_type*
    copy (char_type*, const char_type*, _RWSTD_SIZE_T);

    static char_type*
    move (char_type*, const char_type*, _RWSTD_SIZE_T);

    static char_type*
    assign (char_type*, _RWSTD_SIZE_T, char_type);

    static int_type
    not_eof (const int_type&);

    static char_type
    to_char_type (const int_type&);

    static int_type
    to_int_type (const char_type&);

    static bool
    eq_int_type (const int_type&, const int_type&);

    static int_type
    eof ();

    static _RWSTD_SIZE_T n_calls_ [];

    static int_type eof_;

private:

    // not defined to detect bad assumptions made by the library
    UserTraits ();
    ~UserTraits ();
    void operator= (UserTraits&);
};

#endif   // _RWSTD_NO_WCHAR_T


_RWSTD_SPECIALIZED_CLASS
struct _TEST_EXPORT UserTraits<UserChar>   // user-defined character traits
{
    typedef TraitsMemFunc MemFun;

    typedef UserChar char_type;
    typedef UserInt  int_type;

    // avoid any dependency on the library
    typedef int                   off_type;     // std::streamoff
    typedef int                   state_type;   // std::mbstate_t
    typedef native_std::fpos<state_type> pos_type;     // std::fpos<state_type>

    // accesses to the char_type::f member may trigger a SIGBUS
    // on some architectures (e.g., PA or SPARC) if the member
    // isn't appropriately aligned

    static void assign (char_type&, const char_type&);

    static bool eq (const char_type&, const char_type&);

    static bool lt (const char_type&, const char_type&);

    static int
    compare (const char_type*, const char_type*, _RWSTD_SIZE_T);

    static _RWSTD_SIZE_T length (const char_type*);

    static const char_type*
    find (const char_type*, _RWSTD_SIZE_T, const char_type&);

    static char_type*
    copy (char_type*, const char_type*, _RWSTD_SIZE_T);

    static char_type*
    move (char_type*, const char_type*, _RWSTD_SIZE_T);

    static char_type*
    assign (char_type*, _RWSTD_SIZE_T, char_type);

    static int_type not_eof (const int_type&);

    static char_type to_char_type (const int_type&);

    static int_type to_int_type (const char_type&);

    static bool eq_int_type (const int_type&, const int_type&);

    static int_type eof ();

    static _RWSTD_SIZE_T n_calls_ [];

private:

    // not defined to detect bad assumptions made by the library
    UserTraits ();
    ~UserTraits ();
    void operator= (UserTraits&);
};


// rw_widen() widens successive elements of src into the buffer dst
// when (dst != 0) the last character in the buffer is guaranteed to be NUL,
// regardless of the value of str [len]
// when (len == SIZE_MAX && src != 0), computes len as if by evaluating
// len = strlen(src)
// when (src == 0 && len < SIZE_MAX), initializes the first len elements
// of dst to NUL (i.e., such a call is the equivalent of calling
// memset(dst, 0, len * sizeof *dst))
// returns dst
_TEST_EXPORT char*
rw_widen (char*         /* dst */,
          const char*   /* src */,
          _RWSTD_SIZE_T /* len */ = _RWSTD_SIZE_MAX);

// rw_expand() interprets string src as a sequence of directives and
// widens the result of their processing into the provided buffer dst
// when (dst != 0), otherwise into a dynamically allocated buffer of
// sufficient size
// each directive consists of a character C, optionally followed by
// the '@' sign followed by a non-negative decimal number N; the result
// of the processing of a directive is N consecutive copies of the
// character C where (N = 1) when the '@' is absent
// when (dst_len != 0) the function sets *dst_len to the number of
// produced characters not counting the terminating NUL
// the function returns the dynamically allocated buffer; the caller
// must deallocate the buffer using the delete expression
_TEST_EXPORT char*
rw_expand (char*          /* dst     */,
           const char*    /* src     */,
           _RWSTD_SIZE_T  /* src_len */ = _RWSTD_SIZE_MAX,
           _RWSTD_SIZE_T* /* dst_len */ = 0);


// rw_narrow() narrows successive elements of src into the buffer dst
// (see rw_widen() for details)
_TEST_EXPORT char*
rw_narrow (char*         /* dst */,
           const char*   /* src */,
           _RWSTD_SIZE_T /* len */ = _RWSTD_SIZE_MAX);

// rw_match() compares up to len successive elements of s1 and s2 for
// equality until it finds a pair that do not compare equal or until
// len comparisons has been made
// if (len == SIZE_MAX) is true elements are compared until the first
// mismatch is found or until the NUL character is encountered
// returns the number of matching elements
_TEST_EXPORT _RWSTD_SIZE_T
rw_match (const char*   /* s1  */,
          const char*   /* s2  */,
          _RWSTD_SIZE_T /* len */ = _RWSTD_SIZE_MAX);

#ifndef _RWSTD_WCHAR_T

_TEST_EXPORT wchar_t*
rw_widen (wchar_t*, const char*, _RWSTD_SIZE_T = _RWSTD_SIZE_MAX);

_TEST_EXPORT wchar_t*
rw_expand (wchar_t*       /* dst     */,
           const char*    /* src     */,
           _RWSTD_SIZE_T  /* src_len */ = _RWSTD_SIZE_MAX,
           _RWSTD_SIZE_T* /* dst_len */ = 0);

_TEST_EXPORT char*
rw_narrow (char*, const wchar_t*, _RWSTD_SIZE_T = _RWSTD_SIZE_MAX);

_TEST_EXPORT _RWSTD_SIZE_T
rw_match (const char*, const wchar_t*,
          _RWSTD_SIZE_T = _RWSTD_SIZE_MAX);

#endif   // _RWSTD_WCHAR_T

_TEST_EXPORT UserChar*
rw_widen (UserChar*, const char*, _RWSTD_SIZE_T = _RWSTD_SIZE_MAX);

_TEST_EXPORT UserChar*
rw_expand (UserChar*      /* dst     */,
           const char*    /* src     */,
           _RWSTD_SIZE_T  /* src_len */ = _RWSTD_SIZE_MAX,
           _RWSTD_SIZE_T* /* dst_len */ = 0);

_TEST_EXPORT char*
rw_narrow (char*, const UserChar*, _RWSTD_SIZE_T = _RWSTD_SIZE_MAX);

_TEST_EXPORT _RWSTD_SIZE_T
rw_match (const char*, const UserChar*, _RWSTD_SIZE_T = _RWSTD_SIZE_MAX);


// accessors to n_calls array, if present
template <class charT>
inline _RWSTD_SIZE_T*
rw_get_call_counters (native_std::char_traits<charT>*, charT*)
{
    return 0;
}

template <class charT>
inline _RWSTD_SIZE_T*
rw_get_call_counters (UserTraits<charT>*, charT*)
{
    return UserTraits<charT>::n_calls_;
}


static const struct _TEST_EXPORT UserCharFmatInit {
    UserCharFmatInit ();
} _rw_user_char_fmat_init;

#endif   // RW_CHAR_INCLUDED
