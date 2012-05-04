/************************************************************************
 *
 * valcmp.h - declarations of the rw_valcmp() family of helper functions
 *
 * $Id: valcmp.h 550991 2007-06-26 23:58:07Z sebor $
 *
 ************************************************************************
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
 * Copyright 1994-2006 Rogue Wave Software.
 *
 **************************************************************************/

#ifndef RW_VALCMP_H_INCLUDED
#define RW_VALCMP_H_INCLUDED

#include <testdefs.h>


#define CMP_NULTERM   1   /* the first 0 terminates processing */
#define CMP_RETOFF    2   /* return offset of the first mismatch */
#define CMP_NOCASE    4   /* case-insensitive character comparison */
#define CMP_FP        8   /* safe floating pointing comparison */


_TEST_EXPORT int
rw_valcmp (const void*, const void*,
           _RWSTD_SIZE_T, _RWSTD_SIZE_T, _RWSTD_SIZE_T, int);

/**
 * Compares the contents of two arrays of objects of integral types,
 * possibly of different sizes, for equality, in a strncmp/memcmp
 * way.
 *
 * @param buf1  Pointer to an array of 0 or more objects of integral type.
 * @param buf2  Pointer to an array of 0 or more objects of integral type.
 * @param nelems  The maximum number of elements to compare.
 * @param flags  Bitmap of flags that determine how the objects are
 *               compared.
 * @return  Returns -1, 0, or +1, depending on whether the first array
 *          is less than, equal to, or greater than the second array.
 */

template <class T, class U>
inline int
rw_valcmp (const T*      buf1,
           const U*      buf2,
           _RWSTD_SIZE_T nelems,
           int           flags = 0)
{
    return rw_valcmp (buf1, buf2, nelems, sizeof (T), sizeof (U), flags);
}


/**************************************************************************/

// compares up to a maximum number of characters from the two strings
// posisbly including any embedded NULs (when the cmp_nul bit is set)
// and returns -1, 0, or +1 if the first string compares less, equal,
// or greater, respectively, than the second string, or the offset+1
// of the first mismatched character (when the cmp_off bit is set)
// or 0 otherwise
//
// rw_strncmp(s1, s2) is equivalent to a call to strcmp(s1, s2) when
// the type of s1 and s2 is char*, wcscmp(s1, s2) when the type is
// wchar_t*
//
// rw_strncmp(s1, s2, n) with (n != ~0U) is equivalent to a call to
// strncmp(s1, s2, n) or wcsncmp(s1, s2, n), respectively
//
// rw_strncmp(s1, s2, n, cmp_nul) with (n != ~0U) is equivalent to
// a call to memcmp(s1, s2, n) or wmemcmp(s1, s2, n), respectively

_TEST_EXPORT int
rw_strncmp (const char*, const char*,
            _RWSTD_SIZE_T = _RWSTD_SIZE_MAX, int = CMP_NULTERM);

#ifndef _RWSTD_NO_WCHAR_T

_TEST_EXPORT int
rw_strncmp (const char*, const wchar_t*,
            _RWSTD_SIZE_T = _RWSTD_SIZE_MAX, int = CMP_NULTERM);

_TEST_EXPORT int
rw_strncmp (const wchar_t*, const char*,
            _RWSTD_SIZE_T = _RWSTD_SIZE_MAX, int = CMP_NULTERM);

_TEST_EXPORT int
rw_strncmp (const wchar_t*, const wchar_t*,
            _RWSTD_SIZE_T = _RWSTD_SIZE_MAX, int = CMP_NULTERM);

#endif   // _RWSTD_NO_WCHAR_T


/**
 * Compares two floating point numbers for equality.
 *
 * @param x  The left hand side of the comparison.
 * @param y  The right hand side of the comparison.
 *
 * @return  Returns a negative value, 0, or a positive value, depending
 *          on whether the first number is less than, equal to, or greater
 *          than the second number. The magnitude of the returned value
 *          indicates the number of distinct values representable in
 *          the type of the number between the two arguments.
 */
_TEST_EXPORT int
rw_fltcmp (float x, float y);


/**
 * @see rw_fltcmp.
 */
_TEST_EXPORT int
rw_dblcmp (double x, double y);

#ifndef _RWSTD_NO_LONG_DOUBLE

/**
 * @see rw_fltcmp.
 */
_TEST_EXPORT int
rw_ldblcmp (long double x, long double y);

#endif   // _RWSTD_NO_LONG_DOUBLE

/**************************************************************************/

/**
 * Compares two values of the same type for equality.
 *
 * @param x  The left hand side of the comparison.
 * @param y  The right hand side of the comparison.
 *
 * @return  Returns 1 if the the values are the same, 0 otherwise.
 */
template <class T>
inline int rw_equal (T x, T y)
{
    return x == y;
}

/**
 * @see rw_equal.
 */
inline int rw_equal (float x, float y)
{
    return 0 == rw_fltcmp (x, y);
}

/**
 * @see rw_equal.
 */
inline int rw_equal (double x, double y)
{
    return 0 == rw_dblcmp (x, y);
}

#ifndef _RWSTD_NO_LONG_DOUBLE

/**
 * @see rw_equal.
 */
inline int rw_equal (long double x, long double y)
{
    return 0 == rw_ldblcmp (x, y);
}

#endif   // _RWSTD_NO_LONG_DOUBLE

#endif   // RW_VALCMP_H_INCLUDED
