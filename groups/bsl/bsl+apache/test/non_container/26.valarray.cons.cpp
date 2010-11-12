/***************************************************************************
 *
 * 26.valarray.cons.cpp - tests exercising valarray constructors
 *
 * $Id: 26.valarray.cons.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <cstdlib>       // for free(), strtol(), size_t
#include <valarray>      // for indirect_array, valarray

#include <rw_value.h>    // for UserClass
#include <driver.h>      // for rw_test()
#include <rw_printf.h>   // for rw_asnprintf()

/**************************************************************************/

// returns an array of size elements of type T constructed from a string
// of comma-separated values
template <class T>
T*
make_array (const T*, const char *s, std::size_t *psize)
{
    std::size_t nelems = psize ? *psize : 0;

    T* const buf = new T [nelems ? nelems : 4096];

    if (0 == nelems && (0 == s || '\0' == *s))
        return buf;

    std::size_t i;

    for (i = 0; ; ++i) {

        char *end = 0;
        long val = s ? std::strtol (s, &end, 0) : 0L;

        RW_ASSERT (0 == end || '\0' == *end || ',' == *end);

        buf [i] = T (val);

        if (0 == end || '\0' == *end) {
            while (++i < nelems)
                buf [i] = buf [i - 1];

            break;
        }

        s = end + 1;
    }

    if (psize)
        *psize = i;

    return buf;
}


// deletes an array of elements of type T returned from make_array
template <class T>
void
delete_array (const T *array, std::size_t)
{
    T* const a = _RWSTD_CONST_CAST (T*, array);
    delete[] a;
}


template <class T>
const std::size_t* count (const T*) { return 0; }


template <class T>
T value (const T &val) { return val; }

/**************************************************************************/

// returns an array of size elements of type UserClass
// constructed from a string of comma-separated values
UserClass*
make_array (const UserClass*, const char *s, std::size_t *psize)
{
    std::size_t nelems = psize ? *psize : 0;

    const std::size_t size = sizeof (UserClass);
    void* const raw = operator new ((nelems ? nelems : 1024) * size);
    UserClass* const buf = _RWSTD_STATIC_CAST (UserClass*, raw);

    if (0 == nelems && (0 == s || '\0' == *s))
        return buf;

    std::size_t i;

    for (i = 0; ; ++i) {

        char *end = 0;
        long val = s ? std::strtol (s, &end, 0) : 0L;

        RW_ASSERT (0 == end || '\0' == *end || ',' == *end);

        new (buf + i) UserClass ();
        buf [i].data_.val_ = int (val);

        if (0 == end || '\0' == *end) {
            while (++i < nelems)
                new (buf + i) UserClass (buf [i - 1]);

            break;
        }

        s = end + 1;
    }

    if (psize)
        *psize = i;

    return buf;
}


// deletes an array of elements of type T returned from make_array
void
delete_array (const UserClass *array, std::size_t nelems)
{
    UserClass* const a = _RWSTD_CONST_CAST (UserClass*, array);
    
    for (std::size_t i = 0; i != nelems; ++i)
        (a + i)->~UserClass ();

    operator delete (a);
}


const std::size_t* count (const UserClass*) { return &UserClass::count_; }

int value (const UserClass &val) { return val.data_.val_; }

/**************************************************************************/

enum CtorId {
    DefaultCtor,   // valarray<T>::valarray()
    SizeCtor,      // valarray<T>::valarra(size_t)
    ValueCtor,     // valarray<T>::valarray(const T&, size_t)
    ArrayCtor      // valarray<T>::valarray(const T*, size_t)
};


template <class T>
void
test_ctor (const T*, const char *tname, CtorId which, bool copy,
           int line, const char *str, std::size_t nelems)
{
    std::valarray<T> *pva = 0;

    T* const array = make_array ((const T*)0, str, &nelems);

    char*       fname = 0;
    std::size_t size  = 0;

    // pointer to a counter keepint track of all objects of type T
    // in existence (non-null only for T=UserClass)
    const std::size_t* const pcounter = count ((const T*)0);

    // get the number of objects of type T before invoking the ctor
    std::size_t nobjects = pcounter ? *pcounter : 0;

    switch (which) {

    case DefaultCtor:
        rw_asnprintf (&fname, &size, "valarray<%s>::valarray()", tname);
        pva = new std::valarray<T>;
        break;

    case SizeCtor:
        rw_asnprintf (&fname, &size,
                      "valarray<%s>::valarray(size_t = %zu)",
                      tname, nelems);
        pva = new std::valarray<T>(nelems);
        break;

    case ValueCtor: {
        rw_asnprintf (&fname, &size,
                      "valarray<%s>::valarray(const %1$s& = %1$s(%d), "
                      "size_t = %zu)",
                      tname, value (array [0]), nelems);
        pva = new std::valarray<T>(array [0], nelems);
        break;
    }

    case ArrayCtor: {
        rw_asnprintf (&fname, &size,
                      "valarray<%s>::valarray(const %1$s* = {%s}, "
                      "size_t = %zu)",
                      tname, str, nelems);
        pva = new std::valarray<T>(array, nelems);
        break;
    }

    }

    std::valarray<T> *psave = 0;

    if (copy) {
        char *tmpbuf        = 0;
        std::size_t tmpsize = 0;

        rw_asnprintf (&tmpbuf, &tmpsize, "valarray<%s>::valarray(%s)",
                      tname, fname);

        std::free (fname);
        fname = tmpbuf;
        size  = tmpsize;

        // replace the stored object counter value
        nobjects = pcounter ? *pcounter : 0;

        // save the original and replace it with the new array
        psave = pva;

        // invoke the copy ctor
        pva = new std::valarray<T>(*pva);
    }
        
    // verify the size of the array
    rw_assert (pva->size () == nelems, 0, line,
               "line %d. %s.size() == %zu, got %zu",
               __LINE__, fname, nelems, pva->size ());

    if (pcounter) {
        // compute the number of objects of type T constructed
        // by the ctor (valid only for T=UserClass)
        nobjects = *pcounter - nobjects;
        
        rw_assert (nobjects == nelems, 0, line,
                   "line %d. %s constucted %zu objects, expected %zu",
                   __LINE__, fname, nobjects, nelems);
    }

    // verify the element values
    for (std::size_t i = 0; i != nelems; ++i) {
        if (!((*pva)[i] == array [i])) {
            rw_assert (i == nelems, 0, line,
                       "line %d. %s[%zu] == %s(%d), got %4$s(%d)",
                       __LINE__, fname, i, tname,
                       value (array [i]), value ((*pva)[i]));

            break;
        }
    }

    delete_array (array, nelems);

    // get the number of objects of type T before invoking the dtor
    nobjects = pcounter ? *pcounter : 0;

    delete pva;

    if (pcounter) {
        // compute the number of objects of type T destroyed by the dtor
        nobjects = nobjects - *pcounter;

        // verify that all objects constructed by the ctor have been
        // destroyed (i.e., none leaked)
        rw_assert (nobjects == nelems, 0, line,
                   "line %d. %s dtor destroyed %zu objects, expected %zu",
                   __LINE__, fname, nobjects, nelems);
    }

    delete psave;
    std::free (fname);
}


/**************************************************************************/

template <class T>
void
test_default_ctor (const T*, const char *tname, bool copy)
{
    if (!copy)
        rw_info (0, 0, __LINE__, "std::valarray<%s>::valarray()", tname);

    test_ctor ((const T*)0, tname, DefaultCtor, copy, __LINE__, 0, 0);
}

/**************************************************************************/

template <class T>
void
test_size_ctor (const T*, const char *tname, bool copy)
{
    if (!copy)
        rw_info (0, 0, __LINE__, "std::valarray<%s>::valarray(size_t)",
                 tname);

#undef TEST
#define TEST(n) \
    test_ctor ((const T*)0, tname, SizeCtor, copy, __LINE__, "0", n)

    TEST (0);
    TEST (1);
    TEST (2);
    TEST (3);
    TEST (4);
    TEST (5);
    TEST (6);
    TEST (7);
    TEST (8);
    TEST (9);
    TEST (10);
    TEST (123);
    TEST (1023);
}

/**************************************************************************/

template <class T>
void
test_value_ctor (const T*, const char *tname, bool copy)
{
    if (!copy)
        rw_info (0, 0, __LINE__,
                 "std::valarray<%s>::valarray(const %1$s&, size_t)",
                 tname);
#undef TEST
#define TEST(str, n) \
    test_ctor ((const T*)0, tname, ValueCtor, copy, __LINE__, str, n)

    TEST ("0", 0);
    TEST ("0", 1);
    TEST ("1", 1);
    TEST ("2", 2);
    TEST ("3", 3);
    TEST ("4", 4);
    TEST ("5", 5);
    TEST ("6", 12345);
}

/**************************************************************************/

template <class T>
void
test_array_ctor (const T*, const char *tname, bool copy)
{
    if (!copy)
        rw_info (0, 0, __LINE__,
                 "std::valarray<%s>::valarray(const %1$s*, size_t)",
                 tname);

#undef TEST
#define TEST(str) \
    test_ctor ((const T*)0, tname, ArrayCtor, copy, __LINE__, str, 0)

    TEST ("");   // empty array
    TEST ("0");
    TEST ("0,1");
    TEST ("0,1,2");
    TEST ("0,1,2,3");
    TEST ("0,1,2,3,4");
    TEST ("0,1,2,3,4,5");
    TEST ("0,1,2,3,4,5,6");
    TEST ("0,1,2,3,4,5,6,7");
    TEST ("0,1,2,3,4,5,6,7,8");
    TEST ("0,1,2,3,4,5,6,7,8,9");
}

/**************************************************************************/

template <class T>
void
test_copy_ctor (const T*, const char *tname)
{
    rw_info (0, 0, __LINE__,
             "std::valarray<%s>::valarray(const valarray<%1$s>&)", tname);
}

/**************************************************************************/

template <class T>
void
test_ctors (const T*, const char *tname)
{
    for (int i = 0; i != 2; ++i) {

        // exercise the respective ctor in the first iteration
        // and the copy ctor invoked an object constructed with
        // the same respective ctor as in the first iteration
        // then

        const bool test_copy_ctor = 0 < i;

        test_default_ctor ((T*)0, tname, test_copy_ctor);
        test_size_ctor ((T*)0, tname, test_copy_ctor);
        test_value_ctor ((T*)0, tname, test_copy_ctor);
        test_array_ctor ((T*)0, tname, test_copy_ctor);
    }
}

/**************************************************************************/

static int
run_test (int, char**)
{
#undef TEST
#define TEST(T)   test_ctors ((const T*)0, #T)
    TEST (char);
    TEST (int);
    TEST (double);

    TEST (UserClass);

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    // FIXME: add command line options to enable/disable each operator
    return rw_test (argc, argv, __FILE__,
                    "valarray.cons",
                    0 /* no comment */,
                    run_test,
                    "",
                    (void*)0   /* sentinel */);
}
