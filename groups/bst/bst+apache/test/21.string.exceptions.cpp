/***************************************************************************
 *
 * 21.string.exceptions.cpp - test string members that throw exceptions
 *
 * $Id$
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
 * Copyright 1994-2008 Rogue Wave Software.
 *
 **************************************************************************/

#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <string>

#include <driver.h>
#include <rw/_error.h>

// uses setjmp() and longjmp() to verify correct function if exception
// exception support is disabled
#ifdef _RWSTD_NO_EXCEPTIONS
#  include <csetjmp>

int     id;   // "thrown" from user_throw () defined below
jmp_buf env;  // used to implement fake exception handling

#  undef try
#  define try           if (0 == (id = setjmp (env)))

#  undef catch
#  define catch(ignore) else if (0 != id)
#endif   // _RWSTD_NO_EXCEPTIONS


// use a type that string isn't instantiated/specialized on by the library
// also use a native type to avoid having to specialize char_traits
typedef int Char;

typedef std::char_traits<Char> CharTraits;
typedef std::allocator<Char> CharAllocator;

typedef std::basic_string<Char, CharTraits, CharAllocator> String;


// specialize in order to easily induce exceptional conditions
#ifdef INCLUDED_BSLSTL_STRING
#define _RWSTD_NEW_CAPACITY(BString, p, n) ((p)->capacity())
namespace bsl {

typedef basic_string<Char, CharTraits, CharAllocator> BString;
_RWSTD_SPECIALIZED_CLASS
String::size_type BString::max_size () const
{
    //return _RWSTD_NEW_CAPACITY (BString, this, 0) ? this->capacity() : INT_MAX;
    return 100;  // needed to make exception happen
}

}   // close namespace bsl
#else
_RWSTD_NAMESPACE (std) {

_RWSTD_SPECIALIZED_CLASS
String::size_type String::max_size () const
{
    return _RWSTD_NEW_CAPACITY (String, this, 0) ? this->capacity() : INT_MAX;
}

}   // namespace std
#endif


#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE
#  include <rw_new.h>
#else
   // can't check for memory leaks using the replacement
   // operators new and delete
#  define rwt_check_leaks(psize, ignore) \
          ((psize) ? *(std::size_t*)(psize) = 0 : 0)
#endif


/* static */ void
user_throw (int ex_id, char *what)
{
    delete[] what;

#ifndef _RWSTD_NO_EXCEPTIONS

    throw ex_id;

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    longjmp (env, ex_id);

#endif   // _RWSTD_NO_EXCEPTIONS
}

/**************************************************************************/

static void
test_simple_throw ()
{
    rw_info (0, __FILE__, __LINE__, "exception handling test setup");

    // establish a chekpoint for memory leaks
    rwt_check_leaks (0, 0);

#ifdef _RWSTD_NO_EXCEPTIONS

    // prevent library from aborting if exception support is disabled
    _RW::__rw_throw_proc = user_throw;

#endif   // _RWSTD_NO_EXCEPTIONS

    int thrown = 0;

    try {
        // throw and catch a bogus exception in order to initialize
        // data structures internal to the library to prevent any
        // memory allocation from throwing off memory leak detection
        _RWSTD_REQUIRES (0, (_RWSTD_ERROR_OUT_OF_RANGE,
                             _RWSTD_FUNC ("test_simple_throw ()"), 0, 0));
    }
    catch (int id) {
        thrown = id == _RWSTD_ERROR_OUT_OF_RANGE;
    }
    catch (...) {
        thrown = -1;
    }

    rw_assert (1 == thrown, __FILE__, __LINE__,
               "_RWSTD_REQUIRES (_RWSTD_ERROR_OUT_OF_RANGE) failed to "
               "throw std::out_of_range");
}

/**************************************************************************/

// const objects used throughout remaining test cases

const Char s[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f', '\0'
};

const String s0 (s);

/**************************************************************************/

// constructor tests

static void
test_size_ctor ()
{
    rw_info (0, __FILE__, __LINE__, "21.3.1, p4 (size)");

    // establish a chekpoint for memory leaks
    rwt_check_leaks (0, 0);

    int thrown = 0;

    try {
        // throws std::out_of_range if pos > str.size ()
        String s1 (s0, s0.size () + 1);
    }
#ifndef _RWSTD_NO_EXCEPTIONS

    catch (std::out_of_range&) {
        thrown = 1;
    }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    catch (int id) {
        thrown = id == _RWSTD_ERROR_OUT_OF_RANGE;
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    catch (...) {
        thrown = -1;
    }

    std::size_t nbytes; /* uninitialized */
    std::size_t nblocks = rwt_check_leaks (&nbytes, 0);
    _RWSTD_UNUSED (nblocks);

    rw_assert (1 == thrown, __FILE__, __LINE__,
               ("string::string (const string&, size_type, size_type, "
                "const allocator_type&) failed to throw std::out_of_range"));

    rw_assert (s == s0, __FILE__, __LINE__,
               "original const string modified");

    rw_assert (0 == nbytes, __FILE__, __LINE__,
               "string::string (const string&, size_type, size_type, "
               "const allocator_type&) leaked %u bytes", nbytes);
}

static void
test_npos_ctor ()
{
    rw_info (0, __FILE__, __LINE__, "21.3.1, p4 (npos)");

    _RW::__rw_throw_proc = user_throw;

    // establish a chekpoint for memory leaks
    rwt_check_leaks (0, 0);

    int thrown = 0;

    try {
        // throws std::out_of_range if pos > str.size ()
        String s1 (s0, String::npos);
    }
    catch (const std::out_of_range&) {
        thrown = 1;
    }
    catch (int id) {
        thrown = _RWSTD_ERROR_OUT_OF_RANGE == id;
    }
    catch (...) { /* empty */ }

    std::size_t nbytes; /* uninitialized */
    std::size_t nblocks = rwt_check_leaks (&nbytes, 0);
    _RWSTD_UNUSED (nblocks);

    rw_assert (1 == thrown, __FILE__, __LINE__,
               "string::string (const string&, size_type, size_type, "
               "const allocator_type&) failed to use __rw::__rw_throw()");

    rw_assert (0 == nbytes, __FILE__, __LINE__,
               "string::string (const string&, size_type, size_type, "
               "const allocator_type&) leaked %u bytes", nbytes);

    rw_assert (s == s0, __FILE__, __LINE__,
               "original const string modified");
}

static void
test_max_size_ctor1 ()
{
    rw_info (0, __FILE__, __LINE__, "21.3.1, p7");

    // establish a chekpoint for memory leaks
    rwt_check_leaks (0, 0);

    int thrown = 0;

    try {
        // throws std::length_error if n > max_size () (*)
        // (*) see also lwg issue 83
        String s1 (s0.data (), s0.max_size () + 1);
    }
    catch (const std::length_error&) {
        thrown = 1;
    }
    catch (int id) {
        thrown = _RWSTD_ERROR_LENGTH_ERROR == id;
    }
    catch (...) { /* empty */ }

    std::size_t nbytes; /* uninitialized */
    std::size_t nblocks = rwt_check_leaks (&nbytes, 0);
    _RWSTD_UNUSED (nblocks);

    rw_assert (1 == thrown, __FILE__, __LINE__,
               "string::string (const char_type*, size_type) "
               "failed to use __rw::__rw_throw()");

    rw_assert (0 == nbytes, __FILE__, __LINE__,
               "string::string (const char_type*, size_type)"
               "leaked %u bytes", nbytes);

    rw_assert (s == s0, __FILE__, __LINE__,
               "original const string modified");
}

static void
test_max_size_ctor2 ()
{
    rw_info (0, __FILE__, __LINE__, "21.3.1, p13");

    int thrown = 0;

    try {
        // throws std::length_error if n > max_size () (*)
        // (*) see also lwg issue 83
        String s1 (s0.max_size () + 1, Char ());
    }
    catch (const std::length_error&) {
        thrown = 1;
    }
    catch (int id) {
        thrown = _RWSTD_ERROR_LENGTH_ERROR == id;
    }
    catch (...) { /* empty */ }

    rw_assert (1 == thrown, __FILE__, __LINE__,
               "string::string (size_type, char_type) "
               "failed to use __rw::__rw_throw()");
}

static void
test_len_ctor ()
{
#ifndef _RWSTD_NO_MEMBER_TEMPLATES

    rw_info (0, __FILE__, __LINE__, "21.3.1, p15");

    int thrown = 0;

    try {
        // same as 21.3.1, p13 for integral arguments
        String s1 = String (long (String::npos), long (' '));
    }
    catch (const std::length_error&) {
        thrown = 1;
    }
    catch (int id) {
        thrown = _RWSTD_ERROR_LENGTH_ERROR == id;
    }
    catch (...) { /* empty */ }

    rw_assert (1 == thrown, __FILE__, __LINE__,
               "string::string (InputIterator, InputIterator)"
               "failed to use __rw::__rw_throw()");

#endif   // _RWSTD_NO_MEMBER_TEMPLATES
}

/**************************************************************************/

// member functions

static void
test_resize ()
{
    rw_info (0, __FILE__, __LINE__, "21.3.3, p6");

    String s1 (s0);

    // save data, size and capacity of a constructed string
    const String::const_pointer s1_data = s1.data ();
    const String::size_type     s1_size = s1.size ();
    const String::size_type     s1_cap  = s1.capacity ();

    rw_assert (s1.size () == s0.size () && s1 == s0, __FILE__, __LINE__,
               "std::string::string (const string&)");

    int thrown = 0;

    try {
        // throws std::length_error if n > max_size ()
        s1.resize (s1.max_size () + 1U /* , char () */);
    }
    catch (const std::length_error&) {
        thrown = 1;
    }
    catch (int id) {
        thrown = _RWSTD_ERROR_LENGTH_ERROR == id;
    }
    catch (...) { /* empty */ }

    rw_assert (1 == thrown, __FILE__, __LINE__,
               "string::resize (size_type, char_type)"
               "failed to use __rw::__rw_throw()");

    // verify that string wasn't modified
    rw_assert (s1_data == s1.data () && s1_size == s1.size ()
               && s1_cap == s1.capacity (), __FILE__, __LINE__,
               "string::resize (size_type, char_type) modified *this");
}

static void
test_reserve ()
{
    rw_info (0, __FILE__, __LINE__, "21.3.3, p12");

    int thrown = 0;

    String s1 (s0);

    const String::const_pointer s1_data = s1.data ();
    const String::size_type     s1_size = s1.size ();
    const String::size_type     s1_cap  = s1.capacity ();

    try {
        // throws std::length_error if n > max_size ()
        s1.reserve (s1.max_size () + 1U);
    }
    catch (const std::length_error&) {
        thrown = 1;
    }
    catch (int id) {
        thrown = _RWSTD_ERROR_LENGTH_ERROR == id;
    }
    catch (...) { /* empty */ }

    rw_assert (1 == thrown, __FILE__, __LINE__,
               "string::reserve (size_type)"
               "failed to use __rw::__rw_throw()");

    // verify that string wasn't modified
    rw_assert (s1_data == s1.data () && s1_size == s1.size ()
               && s1_cap == s1.capacity (), __FILE__, __LINE__,
               "string::reserve (size_type) modified *this");
}

static void
test_at ()
{
    rw_info (0, __FILE__, __LINE__, "21.3.4, p3");

    int thrown = 0;

    String s1 (s0);

    const String::const_pointer s1_data = s1.data ();
    const String::size_type     s1_size = s1.size ();
    const String::size_type     s1_cap  = s1.capacity ();

    try {
        // throws std::out_of_range if pos > size ()
        s1.at (s1.size ());
    }
    catch (const std::out_of_range&) {
        thrown = 1;
    }
    catch (int id) {
        thrown = _RWSTD_ERROR_OUT_OF_RANGE == id;
    }
    catch (...) { /* empty */ }

    rw_assert (1 == thrown, __FILE__, __LINE__,
               "string::at (size_type) failed to use __rw::__rw_throw()");

    // verify that string wasn't modified
    rw_assert (s1_data == s1.data () && s1_size == s1.size ()
               && s1_cap == s1.capacity (), __FILE__, __LINE__,
               "string::at (size_type) modified *this");
}

static void
test_append ()
{
    rw_info (0, __FILE__, __LINE__, "21.3.5.2, p3");

    int thrown = 0;

    String s1 (s0);

    const String::const_pointer s1_data = s1.data ();
    const String::size_type     s1_size = s1.size ();
    const String::size_type     s1_cap  = s1.capacity ();

    try {
        // throws std::out_of_range if pos > size ()
        s1.append (s1, s1.size () + 1, 0);
    }
    catch (const std::out_of_range&) {
        thrown = 1;
    }
    catch (int id) {
        thrown = _RWSTD_ERROR_OUT_OF_RANGE == id;
    }
    catch (...) { /* empty */ }

    rw_assert (1 == thrown, __FILE__, __LINE__,
               "string::append (const string&, size_type, size_type) "
               "failed to use __rw::__rw_throw()");

    // verify that string wasn't modified
    rw_assert (s1_data == s1.data () && s1_size == s1.size ()
               && s1_cap == s1.capacity (), __FILE__, __LINE__,
               "string::append (const string&, size_type, size_type) "
               "modified *this");
}

static void
test_assign ()
{
    rw_info (0, __FILE__, __LINE__, "21.3.5.3, p3");

    int thrown = 0;

    String s1 (s0);

    const String::const_pointer s1_data = s1.data ();
    const String::size_type     s1_size = s1.size ();
    const String::size_type     s1_cap  = s1.capacity ();

    try {
        // throws std::out_of_range if pos > size ()
        s1.assign (s1, s1.size () + 1, 0);
    }
    catch (const std::out_of_range&) {
        thrown = 1;
    }
    catch (int id) {
        thrown = _RWSTD_ERROR_OUT_OF_RANGE == id;
    }
    catch (...) { /* empty */ }

    rw_assert (1 == thrown, __FILE__, __LINE__,
               "string::assign (const string&, size_type, size_type) "
               "failed to use __rw::__rw_throw()");

    // verify that string wasn't modified
    rw_assert (s1_data == s1.data () && s1_size == s1.size ()
               && s1_cap == s1.capacity (), __FILE__, __LINE__,
               "string::assign (const string&, size_type, size_type) "
               "modified *this");
}

static void
test_insert1 ()
{
    rw_info (0, __FILE__, __LINE__, "21.3.5.4, p3 (pos1 > size ())");

    int thrown = 0;

    String s1 (s0);

    const String::const_pointer s1_data = s1.data ();
    const String::size_type     s1_size = s1.size ();
    const String::size_type     s1_cap  = s1.capacity ();

    try {
        // throws std::out_of_range if:
        // 1)    pos1 > size ()       <-- testing
        // 2) or pos2 > str.size ()
        s1.insert (s1.size () + 1, s1, 0, 0);
    }
    catch (const std::out_of_range&) {
        thrown = 1;
    }
    catch (int id) {
        thrown = _RWSTD_ERROR_OUT_OF_RANGE == id;
    }
    catch (...) { /* empty */ }

    rw_assert (1 == thrown, __FILE__, __LINE__,
               "string::insert (size_type, const string&, size_type, "
               "size_type) failed to use __rw::__rw_throw()");

    // verify that string wasn't modified
    rw_assert (s1_data == s1.data () && s1_size == s1.size ()
               && s1_cap == s1.capacity (), __FILE__, __LINE__,
               "string::insert (size_type, const string&, size_type, "
               "modified *this");
}

static void
test_insert2 ()
{
    rw_info (0, __FILE__, __LINE__, "21.3.5.4, p3 (pos2 > str.size ())");

    int thrown = 0;

    String s1 (s0);

    const String::const_pointer s1_data = s1.data ();
    const String::size_type     s1_size = s1.size ();
    const String::size_type     s1_cap  = s1.capacity ();

    try {
        // throws std::out_of_range if:
        // 1)    pos1 > size ()
        // 2) or pos2 > str.size ()   <-- testing
        s1.insert (s1.size (), s1, s1.size () + 1, 0);
    }
    catch (const std::out_of_range&) {
        thrown = 1;
    }
    catch (int id) {
        thrown = _RWSTD_ERROR_OUT_OF_RANGE == id;
    }
    catch (...) { /* empty */ }

    rw_assert (1 == thrown, __FILE__, __LINE__,
               "string::insert (size_type, const string&, size_type, "
               "size_type) failed to use __rw::__rw_throw()");

    // verify that string wasn't modified
    rw_assert (s1_data == s1.data () && s1_size == s1.size ()
               && s1_cap == s1.capacity (), __FILE__, __LINE__,
               "string::insert (size_type, const string&, size_type, "
               "modified *this");
}

static void
test_erase ()
{
    rw_info (0, __FILE__, __LINE__, "21.3.5.5, p2");

    int thrown = 0;

    String s1 (s0);

    const String::const_pointer s1_data = s1.data ();
    const String::size_type     s1_size = s1.size ();
    const String::size_type     s1_cap  = s1.capacity ();

    try {
        // throws std::out_of_range if pos > size ()
        s1.erase (s1.size () + 1 /* , String::npos */);
    }
    catch (const std::out_of_range&) {
        thrown = 1;
    }
    catch (int id) {
        thrown = _RWSTD_ERROR_OUT_OF_RANGE == id;
    }
    catch (...) { /* empty */ }

    rw_assert (1 == thrown, __FILE__, __LINE__,
               "string::erase (size_type, size_type) "
               "failed to use __rw::__rw_throw()");

    // verify that string wasn't modified
    rw_assert (s1_data == s1.data () && s1_size == s1.size ()
               && s1_cap == s1.capacity (), __FILE__, __LINE__,
               "string::erase (size_type, size_type) modified *this");
}

static void
test_replace1 ()
{
    rw_info (0, __FILE__, __LINE__, "21.3.5.6, p3 (pos1 > size ())");

    int thrown = 0;

    String s1 (s0);

    const String::const_pointer s1_data = s1.data ();
    const String::size_type     s1_size = s1.size ();
    const String::size_type     s1_cap  = s1.capacity ();

    try {
        // throws std::out_of_range if:
        // 1)    pos1 > size ()      <-- testing
        // 2) or pos2 > str.size ()
        s1.replace (s1.size () + 1, 0, s0, s0.size (), 0);
    }
    catch (const std::out_of_range&) {
        thrown = 1;
    }
    catch (int id) {
        thrown = _RWSTD_ERROR_OUT_OF_RANGE == id;
    }
    catch (...) { /* empty */ }

    rw_assert (1 == thrown, __FILE__, __LINE__,
               "string::replace (size_type, size_type, const string&, "
               "size_type, size_type) failed to use __rw::__rw_throw()");

    // verify that string wasn't modified
    rw_assert (s1_data == s1.data () && s1_size == s1.size ()
               && s1_cap == s1.capacity (), __FILE__, __LINE__,
               "string::replace (size_type, size_type, const string&, "
               "size_type, size_type) modified *this");
}

static void
test_replace2 ()
{
    rw_info (0, __FILE__, __LINE__, "21.3.5.6, p3 (pos2 > str.size ())");

    int thrown = 0;

    String s1 (s0);

    const String::const_pointer s1_data = s1.data ();
    const String::size_type     s1_size = s1.size ();
    const String::size_type     s1_cap  = s1.capacity ();

    try {
        // throws std::out_of_range if:
        // 1)    pos1 > size ()
        // 2) or pos2 > str.size ()   <-- testing
        s1.replace (s1.size (), 0, s1, s1.size () + 1, 0);
    }
    catch (const std::out_of_range&) {
        thrown = 1;
    }
    catch (int id) {
        thrown = _RWSTD_ERROR_OUT_OF_RANGE == id;
    }
    catch (...) { /* empty */ }

    rw_assert (1 == thrown, __FILE__, __LINE__,
               "string::replace (size_type, size_type, const string&, "
               "size_type, size_type) failed to use __rw::__rw_throw()");

    // verify that string wasn't modified
    rw_assert (s1_data == s1.data () && s1_size == s1.size ()
               && s1_cap == s1.capacity (), __FILE__, __LINE__,
               "string::replace (size_type, size_type, const string&, "
               "size_type, size_type) modified *this");
}

static void
test_replace3 ()
{
    rw_info (0, __FILE__, __LINE__, "21.3.5.6, p5");

    int thrown = 0;

    String s1 (s0);

    const String::const_pointer s1_data = s1.data ();
    const String::size_type     s1_size = s1.size ();
    const String::size_type     s1_cap  = s1.capacity ();

    // establish a chekpoint for memory leaks
    rwt_check_leaks (0, 0);

    try {
        // make sure max_size() isn't too big
        //assert (s1.max_size () == _RWSTD_NEW_CAPACITY (String, &s1, 0));

        thrown = -1;

        // must not throw
        String s2 (s1.max_size () - s1.size () + 2, Char ());

        thrown = 0;

        // throws std::length_error if:
        //               size () - xlen >= max_size () - rlen (*)
        // where xlen = min (n1, this->size () - pos1)
        // and   rlen = min (n2, str.size () - pos2)
        // (*) see also lwg issue 86
        s1.replace (0, 1, s2, 0, s2.size ());
    }
    catch (const std::length_error&) {
        thrown = 1;
    }
    catch (int id) {
        thrown = 0 == thrown && _RWSTD_ERROR_LENGTH_ERROR == id;
    }
    catch (...) { /* empty */ }

    std::size_t nbytes; /* uninitialized */
    std::size_t nblocks = rwt_check_leaks (&nbytes, 0);
    _RWSTD_UNUSED (nblocks);

    rw_assert (1 == thrown, __FILE__, __LINE__,
               "string::replace (size_type, size_type, const string&, "
               "size_type, size_type) failed to use __rw::__rw_throw()");

    // verify that string wasn't modified
    rw_assert (s1_data == s1.data () && s1_size == s1.size ()
               && s1_cap == s1.capacity (), __FILE__, __LINE__,
               "string::replace (size_type, size_type, const string&, "
               "size_type, size_type) modified *this");

    // tests not only replace() but also string ctor (s2 above)
    rw_assert (1 == thrown, __FILE__, __LINE__,
               "string::replace (size_type, size_type, const string&, "
               "size_type, size_type) leaked %u bytes", nbytes);
}

static void
test_copy ()
{
    rw_info (0, __FILE__, __LINE__, "21.3.5.7, p2");

    int thrown = 0;

    Char c = '\1';

    String s1 (s0);

    try {
        // throws std::out_of_range if pos > size ()
        s1.copy (&c, 1, s1.size () + 1);
    }
    catch (const std::out_of_range&) {
        thrown = 1;
    }
    catch (int id) {
        thrown = _RWSTD_ERROR_OUT_OF_RANGE == id;
    }
    catch (...) { /* empty */ }

    rw_assert (1 == thrown, __FILE__, __LINE__,
               "string::copy (pointer, size_type, size_type) "
               "failed to use __rw::__rw_throw()");

    // verify that destination buffer wasn't modified
    rw_assert ('\1' == c, __FILE__, __LINE__,
               "string::copy (pointer, size_type, size_type) "
               "modified buffer");
}

static void
test_substr ()
{
    rw_info (0, __FILE__, __LINE__, "21.3.6.7, p2");

    int thrown = 0;

    String s1 (s0);

    try {
        // throws std::out_of_range if pos > size ()
        s1.substr (s1.size () + 1 /* , String::npos */);
    }
    catch (const std::out_of_range&) {
        thrown = 1;
    }
    catch (int id) {
        thrown = _RWSTD_ERROR_OUT_OF_RANGE == id;
    }
    catch (...) { /* empty */ }

    rw_assert (1 == thrown, __FILE__, __LINE__,
               "string::substr (size_type, size_type) "
               "failed to use __rw::__rw_throw()");
}

/**************************************************************************/

static int
run_test (int /*unused*/, char* /*unused*/ [])
{
    test_simple_throw ();

    // constructors
    test_size_ctor ();
    test_npos_ctor ();

    test_max_size_ctor1 ();
    test_max_size_ctor2 ();

    test_len_ctor ();

    // member functions
    test_resize ();
    test_reserve ();
    test_at ();
    test_append ();
    test_assign ();

    test_insert1 ();
    test_insert2 ();

    test_erase ();

    test_replace1 ();
    test_replace2 ();
    test_replace3 ();

    test_copy ();
    test_substr ();

    return 0;
}

/*extern*/ int
main (int argc, char* argv [])
{

    return rw_test (argc, argv, __FILE__,
                    "lib.basic.string",
                    "exception saftety",
                    run_test, "", 0);
}

