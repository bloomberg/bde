/**************************************************************************
 *
 * rw_ctype.h - defines a User Defined ctype facet
 *
 * $Id: rw_ctype.h 590052 2007-10-30 12:44:14Z faridz $
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

#ifndef RW_CTYPE_H_INCLUDED
#define RW_CTYPE_H_INCLUDED


#include <locale>      // for ctype, locale
#include <rw_char.h>   // for UserChar
#include <testdefs.h>


struct _TEST_EXPORT UserCtypeBase: std::ctype_base
{
    enum MemFunc {
        mf_is,
        mf_is_range,
        mf_scan_is,
        mf_scan_not,
        mf_toupper,
        mf_toupper_range,
        mf_tolower, 
        mf_tolower_range,
        mf_widen,
        mf_widen_range,
        mf_narrow,
        mf_narrow_range,
        n_funs
    };

    struct Exception {
        virtual ~Exception ();
        virtual const char* what () const = 0;
    };

    // name of the character type ("char", "wchar_t", "UserChar")
    const char* const cname_;

    // array of masks corresponding to each of the characters in chars
    // the first negative element in chars denotes the last element
    const int* masks_;
    const int* chars_;

    // arrays of correspondning uppercase and lowercase characters
    const int* upper_;
    const int* lower_;

    // arrays of corresponding narrow and wide characters
    const int* narrow_;
    const int* wide_;

    // counter of the number of calls to all member functions
    // made throughout the lifetime of this object
    _RWSTD_SIZE_T n_all_calls_;

    // counter of the number of calls to each member function
    // made throughout the lifetime of this object
    _RWSTD_SIZE_T n_calls_ [n_funs];

    // counter of the number of exceptions thrown by each member
    // function throughout the lifetime of this object
    _RWSTD_SIZE_T n_throws_ [n_funs];

    // member function counter value that, when reached, will
    // cause an exception to be thrown
    _RWSTD_SIZE_T throw_at_calls_ [n_funs];

    // value of a character that, when encountered, will cause
    // an exception to be thrown
    int throw_char_;

    // when true the facet throws an exception when it encounters
    // an invalid character (unless otherwise specified)
    bool throw_on_invalid_;

protected:

    UserCtypeBase (const char*);

private:

    UserCtypeBase (const UserCtypeBase&);    // not CopyConstructible
    void operator= (const UserCtypeBase&);   // not Assignable
};


template <class _CharT>
class UserCtype;


_RWSTD_SPECIALIZED_CLASS
class _TEST_EXPORT UserCtype<char>
    : public std::ctype<char>, public UserCtypeBase
{
    typedef std::ctype<char_type> Base;

public:

    explicit
    UserCtype (_RWSTD_SIZE_T = 0);

    explicit
    UserCtype (const int*, const int*, _RWSTD_SIZE_T = 0);

protected:

    virtual char_type
    do_toupper (char_type) const;

    virtual const char_type*
    do_toupper (char_type*, const char_type*) const;

    virtual char_type
    do_tolower (char_type) const;

    virtual const char_type*
    do_tolower (char_type*, const char_type*) const;

    virtual char_type
    do_widen (char) const;

    virtual const char*
    do_widen (const char*, const char*, char_type*) const;

    virtual char
    do_narrow (char_type, char) const;

    virtual const char_type*
    do_narrow (const char_type*, const char_type*, char, char*) const;
};


#ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
class _TEST_EXPORT UserCtype<wchar_t>
    : public std::ctype<wchar_t>, public UserCtypeBase
{
    typedef std::ctype<char_type> Base;

public:

    explicit
    UserCtype (_RWSTD_SIZE_T = 0);

    explicit
    UserCtype (const int*, const int*, _RWSTD_SIZE_T = 0);

protected:

    virtual bool
    do_is (mask, char_type) const;

    virtual const char_type*
    do_is (const char_type*, const char_type*, mask*) const;

    virtual const char_type*
    do_scan_is (mask, const char_type*, const char_type*) const;

    virtual const char_type*
    do_scan_not (mask, const char_type*, const char_type*) const;

    virtual char_type
    do_toupper (char_type) const;

    virtual const char_type*
    do_toupper (char_type*, const char_type*) const;

    virtual char_type
    do_tolower (char_type) const;

    virtual const char_type*
    do_tolower (char_type*, const char_type*) const;

    virtual char_type
    do_widen (char) const;

    virtual const char*
    do_widen (const char*, const char*, char_type*) const;

    virtual char
    do_narrow (char_type, char) const;

    virtual const char_type*
    do_narrow (const char_type*, const char_type*, char, char*) const;
};


#endif   // _RWSTD_NO_WCHAR_T


//_RWSTD_NAMESPACE (std) {
#undef std  // needed to make this work

namespace std {

_RWSTD_SPECIALIZED_CLASS
class _TEST_EXPORT ctype<UserChar>
    : public native_std::locale::facet, public UserCtypeBase
{
    typedef native_std::locale::facet Base;
public:

    typedef UserChar char_type;

    explicit
    ctype (_RWSTD_SIZE_T = 0);

    explicit
    ctype (const int*, const int*, _RWSTD_SIZE_T = 0);

    bool
    is (mask m, char_type c) const {
        return do_is (m, c);
    }

    const char_type*
    is (const char_type* lo, const char_type *hi, mask *vec) const {
        return do_is (lo, hi, vec);
    }

    const char_type*
    scan_is (mask m, const char_type *lo, const char_type *hi) const {
        return do_scan_is ( m, lo, hi);
    }

    const char_type*
    scan_not (mask m, const char_type *lo, const char_type *hi) const {
        return do_scan_not (m, lo, hi);
    }

    char_type
    (toupper)(char_type c) const {
        return do_toupper (c);
    }

    const char_type*
    (toupper)(char_type *lo, const char_type *hi) const {
        return do_toupper (lo, hi);
    }

    char_type
    (tolower)(char_type c) const {
        return do_tolower (c);
    }

    const char_type*
    (tolower)(char_type *lo, const char_type *hi) const {
        return do_tolower (lo, hi);
    }

    char_type
    widen (char c) const {
        return do_widen (c);
    }

    const char*
    widen (const char *lo, const char *hi, char_type *to) const {
        return do_widen (lo, hi, to);
    }

    char
    narrow (char_type c, char dfault) const {
        return do_narrow (c, dfault);
    }

    const char_type*
    narrow (const char_type *lo, const char_type *hi,
                             char dfault, char *to) const {
        return do_narrow (lo, hi, dfault, to);
    }

    static native_std::locale::id id;

protected:

    virtual bool
    do_is (mask, char_type) const;

    virtual const char_type*
    do_is (const char_type*, const char_type*, mask*) const;

    virtual const char_type*
    do_scan_is (mask, const char_type*, const char_type*) const;

    virtual const char_type*
    do_scan_not (mask, const char_type*, const char_type*) const;

    virtual char_type
    do_toupper (char_type) const;

    virtual const char_type*
    do_toupper (char_type*, const char_type*) const;

    virtual char_type
    do_tolower (char_type c) const;

    virtual const char_type*
    do_tolower (char_type*, const char_type*) const;

    virtual char_type
    do_widen (char) const;

    virtual const char*
    do_widen (const char*, const char*, char_type*) const;

    virtual char
    do_narrow (char_type, char dfault) const;

    virtual const char_type*
    do_narrow (const char_type*, const char_type*, char, char*) const;
};

}   // namespace std
#define std bsl

_RWSTD_SPECIALIZED_CLASS
class _TEST_EXPORT UserCtype<UserChar>
    : public std::ctype<UserChar>
{
    typedef std::ctype<UserChar> Base;
public:

    typedef UserChar char_type;

    explicit
    UserCtype (_RWSTD_SIZE_T = 0);

    explicit
    UserCtype (const int*, const int*, _RWSTD_SIZE_T = 0);
};


#endif   // RW_CTYPE_H_INCLUDED
