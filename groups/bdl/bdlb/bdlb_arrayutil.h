// bdlb_arrayutil.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLB_ARRAYUTIL
#define INCLUDED_BDLB_ARRAYUTIL

#ifndef INCLUDED_BSLS_IDENT
#  include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities to determine properties of fixed-sized arrays.
//
//@CLASSES:
// bdlb::ArrayUtil: namespace for array access function templates.
//
//@MACROS:
// BDLB_ARRAYUTIL_SIZE: get a constant expression with an array's length.
// BDLB_ARRAYUTIL_LENGTH: get a constant expression with an array's length.
//
//@DESCRIPTION: This component provides a utility 'struct', 'bdlb::ArrayUtil',
// that serves as a namespace for a collection of function templates providing
// access to the length and iterators of statically sized built-in arrays.
//
// The basic idea is that the compiler knows the length of statically sized
// arrays and the corresponding information can be exposed using simple
// function templates.  The use of these function templates is easier and safer
// than the alternatives like use of 'sizeof' (turning the array into a pointer
// doesn't cause the use of 'sizeof' to fail at compile-time but it yields a
// wrong result) or manually specifying the length of an array.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// When creating a sequence of values it is often easy to write the sequence as
// an initialized array and use this array to initialize a container.  Since
// the array's length may be changed as the program is maintained, the code
// using the array should automatically determine the array's length or
// automatically determine iterators to the beginning and the end of the array.
//
// For example, to initialize a 'bsl::vector<int>' with the first few prime
// numbers stored in an array the following code uses the 'begin' and 'end'
// methods of 'bdlb::ArrayUtil':
//..
//  void usePrimes(bool verbose)
//  {
//      if (verbose) cout << "\n" << "'usePrimes'" << "\n"
//                                << "===========" << "\n";
//
//      const int        primes[] = { 2, 3, 5, 7, 11, 13, 17 };
//      bsl::vector<int> values(bdlb::ArrayUtil::begin(primes),
//                              bdlb::ArrayUtil::end(primes));
//
//      assert(values.size() == bdlb::ArrayUtil::size(primes));
//..
// Notice that, after constructing 'values' with the content of the array
// 'primes' the assertion verifies that the correct number of values is stored
// in 'values'.
//
// When the length is needed as a constant expression, e.g., to use it for the
// length of another array, the macro 'BDLB_ARRAYUTIL_LENGTH(array)' can be
// used:
//..
//      int reversePrimes[BDLB_ARRAYUTIL_SIZE(primes)];
//
//      bsl::copy(values.rbegin(),
//                values.rend(),
//                bdlb::ArrayUtil::begin(reversePrimes));
//
//      assert(bsl::mismatch(bdlb::ArrayUtil::rbegin(primes),
//                           bdlb::ArrayUtil::rend(primes),
//                           bdlb::ArrayUtil::begin(reversePrimes)).second
//             == bdlb::ArrayUtil::end(reversePrimes));
//  }
//..
// After defining the array 'reversePrimes' with the same length as 'primes'
// the elements of 'values' are copied in reverse order into this array.  The
// assertion verifies that 'reversePrimes' contains the values from 'primes'
// but in reverse order: 'bsl::mismatch' is used with a reverse sequence of
// 'primes' by using the 'rbegin' and 'rend' methods for 'primes' and normal
// sequence using the 'begin' and 'end' methods for 'reversePrimes'.
//
///Example 2: Use with Database Interfaces
///- - - - - - - - - - - - - - - - - - - -
// The functions 'begin', 'end', and 'size' provided by this component are
// similar to functions provided by containers.  The main difference is that
// they reside in a utility component rather than being member functions.
//
// A typical use case for the 'size' function is a function expecting a pointer
// to a sequence of keys (e.g., columns in a database) and the number of the
// keys in the sequence:
//..
//  void query(bsl::string       *result,
//             const bsl::string *columns,
//             int                numberOfColumns)
//  {
//
//      // Query the database....
//
//  }
//
//  void loadData(bsl::vector<bsl::string> *data)
//  {
//      const bsl::string columns[] = { "column1", "column2", "column3" };
//      bsl::string       result[BDLB_ARRAYUTIL_SIZE(columns)];
//
//      query(result, columns, bdlb::ArrayUtil::size(columns));
//      data->assign(bdlb::ArrayUtil::begin(result),
//                   bdlb::ArrayUtil::end(result));
//  }
//..
// The 'loadData' function shows how to use the different function templates.
// The array 'columns' doesn't have a length specified.  It is determined from
// the number of elements it is initialized with.  In this case it is easy to
// see that there are three elements but in real situations the number of
// elements can be non-trivial to get right.  Also, changing the number of
// elements would make it necessary to apply the corresponding change in
// multiple places.  Thus, the length is determined using 'bdlb::ArrayUtil':
//
//: o The length of 'result' should match the length of 'columns'.  When
//:   specifying the length of an array a constant expression is necessary.  In
//:   C++ 2011 the function 'bdlb::ArrayUtil::size' could return a constant
//:   expression but compilers not, yet, implementing the standard a trick must
//:   be used (using 'sizeof' with a reference to suitably sized array of
//:   'char').  This trick is packaged into the macro 'BDLB_ARRAYUTIL_SIZE()'.
//:
//: o When the length is needed in a context where a 'const' expression is not
//:   required, e.g., when calling 'query', the `bdlb::ArrayUtil::size'
//:   function can be used with the array.
//:
//: o The 'bdlb::ArrayUtil::begin' and 'bdlb::ArrayUtil::end' functions are
//:   used to obtain 'begin' and 'end' iterators used with the vector's
//:   'assign' function to put the 'result' obtained from the call to 'query'
//:   into the 'vector' pointed to by 'data'.
//
// Similar needs for an array of a sequence of values frequently arise when
// using one of the database interfaces.
//
// Another common use case are test cases where the content of a computed
// sequence must be compared with an expected result:
//..
//  void checkData(const bsl::vector<bsl::string>& data)
//  {
//      const bsl::string expect[] = {
//                                       // ...
//                                   };
//      assert(data.size() == bdlb::ArrayUtil::size(expect));
//      assert(bdlb::ArrayUtil::end(expect)
//          == bsl::mismatch(bdlb::ArrayUtil::begin(expect),
//                           bdlb::ArrayUtil::end(expect),
//                           data.begin()).first);
//  }
//..
// In the code below the actual result in 'data' is compared to the values in
// the array 'expect':
//
//: 1 We make sure that the lengths of 'data' and 'expect' are identical using
//:   'bdlb::ArrayUtil::size'.
//:
//: 2 The sequences are compared using the 'mismatch' algorithm: To get the
//:   begin and of the 'expect' array 'bdlb::ArrayUtil::begin' and
//:   'bdlb::ArrayUtil::end', respectively, are used.
//..
//  void getAndCheckData(bool verbose)
//  {
//      if (verbose) cout << "\n" << "'getAndCheckData'" << "\n"
//                                << "=================" << "\n";
//      bsl::vector<bsl::string> data;
//      loadData(&data);
//      checkData(data);
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#define BDLB_ARRAYUTIL_SIZE(a) sizeof(BloombergLP::bdlb::ArrayUtil::sizer(a))
    // Return the number of elements in the passed array 'a' as a constant
    // expression.

#define BDLB_ARRAYUTIL_LENGTH(a) sizeof(BloombergLP::bdlb::ArrayUtil::sizer(a))
    // Return the number of elements in the passed array 'a' as a constant
    // expression.

namespace BloombergLP {
namespace bdlb {
                            // ====================
                            // struct bdeuArrayUtil
                            // ====================

struct ArrayUtil {
    // This 'struct' provides a namespace for a collection of function
    // templates providing access to the length and iterators of statically
    // sized built-in arrays

    // CLASS METHODS

                      // ** iterators **

    template <class TYPE, bsl::size_t LENGTH>
    static TYPE *begin(TYPE (&array)[LENGTH]);
        // Return an iterator pointing to the first element of the specified
        // 'array' of template parameter 'LENGTH' elements of template
        // parameter 'TYPE'.

    template <class TYPE, bsl::size_t LENGTH>
    static TYPE *end(TYPE (&array)[LENGTH]);
        // Return the past-the-end iterator for the specified 'array' of
        // template parameter 'LENGTH' elements of template parameter 'TYPE'.

    template <class TYPE, bsl::size_t LENGTH>
    static bsl::reverse_iterator<TYPE *> rbegin(TYPE (&array)[LENGTH]);
        // Return a reverse iterator pointing to the last element of the
        // specified 'array' of template parameter 'LENGTH' elements of
        // template parameter 'TYPE'.

    template <class TYPE, bsl::size_t LENGTH>
    static bsl::reverse_iterator<TYPE *> rend(TYPE (&array)[LENGTH]);
        // Return the past-the-end reverse iterator for the specified 'array'
        // of template parameter 'LENGTH' elements of template parameter
        // 'TYPE'.

                      // ** size and length **

    template <class TYPE, bsl::size_t LENGTH>
    static bsl::size_t length(TYPE (&array)[LENGTH]);
        // Return the number of elements in the specified 'array' of template
        // parameter 'LENGTH' elements of template parameter 'TYPE'.  This
        // method is synonymous with the 'size' method.

    template <class TYPE, bsl::size_t LENGTH>
    static bsl::size_t size(TYPE (&array)[LENGTH]);
        // Return the number of elements in the specified 'array' of template
        // parameter 'LENGTH' elements of template parameter 'TYPE'.  This
        // method is synonymous with the 'length' method.

    template <class TYPE, bsl::size_t LENGTH>
    static char (&sizer(TYPE (&array)[LENGTH]))[LENGTH];
        // Return a reference to an array of 'char's with the same number of
        // elements as the specified 'array' of template parameter 'LENGTH'
        // elements of template parameter 'TYPE'.  This function is *not*
        // implemented and can only be used in a non-evaluated context.  The
        // function is used together with 'sizeof' to obtain a constant
        // expression with the 'LENGTH' of the 'array'.  See
        // 'BDLB_ARRAYUTIL_SIZE' and 'BDLB_ARRAYUTIL_LENGTH'.
};

// ============================================================================
//                 INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                            // --------------------
                            // struct bdeuArrayUtil
                            // --------------------

// CLASS METHODS

                      // ** iterators **

template <class TYPE, bsl::size_t LENGTH>
inline
TYPE *ArrayUtil::begin(TYPE (&array)[LENGTH])
{
    return array;
}

template <class TYPE, bsl::size_t LENGTH>
inline
TYPE *ArrayUtil::end(TYPE (&array)[LENGTH])
{
    return array + LENGTH;
}

template <class TYPE, bsl::size_t LENGTH>
inline
bsl::reverse_iterator<TYPE *> ArrayUtil::rbegin(TYPE (&array)[LENGTH])
{
    return bsl::reverse_iterator<TYPE *>(end(array));
}

template <class TYPE, bsl::size_t LENGTH>
inline
bsl::reverse_iterator<TYPE *> ArrayUtil::rend(TYPE (&array)[LENGTH])
{
    return bsl::reverse_iterator<TYPE *>(begin(array));
}

                      // ** size and length **

template <class TYPE, bsl::size_t LENGTH>
inline
bsl::size_t ArrayUtil::length(TYPE (&)[LENGTH])
{
    return LENGTH;
}

template <class TYPE, bsl::size_t LENGTH>
inline
bsl::size_t ArrayUtil::size(TYPE (&)[LENGTH])
{
    return LENGTH;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
// ----------------------------- END-OF-FILE ----------------------------------
