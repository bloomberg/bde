// bdeu_arrayutil.h                                                   -*-C++-*-
#ifndef INCLUDED_BDEU_ARRAYUTIL
#define INCLUDED_BDEU_ARRAYUTIL

#ifndef INCLUDED_BDES_IDENT
#  include <bdes_ident.h>
#endif
#ifndef INCLUDED_BDESCM_VERSION
#  include <bdescm_version.h>
#endif
BDES_IDENT("$Id: $")
#ifndef INCLUDED_BSL_ITERATOR
#  include <bsl_iterator.h>
#endif
#ifndef INCLUDED_BSL_CSTDDEF
#  include <bsl_cstddef.h>
#endif

//@PURPOSE: Determine properties of statically sized built-in arrays.
//
//@CLASSES:
// bdeu_ArrayUtil: namespace for array access function templates.
//
//@AUTHOR: Dietmar Kuhl (dkuhl)
//
//@DESCRIPTION: This component provides a utility 'struct',
// 'bdeu_ArrayUtil', that serves as a namespace for a collection of function
// templates providing access to the size and iterators of statically sized
// built-in arrays.
// 
// The basic idea is that the compiler knows the size of staticly sized arrays
// and the corresponding information can be exposed using simple function
// templates. The use of these function templates is easier and safer than
// the alternatives like use of 'sizeof()' (turning the array into a pointer
// doesn't cause the use of 'sizeof()' to fail at compile-time but it yields
// a wrong result) or manually specifying the size of an array.

#define BDEU_ARRAYUTIL_SIZE(a) sizeof(BloombergLP::bdeu_ArrayUtil::sizer(a))
    // Return the number of elements in the passed array as a constant
    // expression.

namespace BloombergLP
{
                            // ====================
                            // struct bdeuArrayUtil
                            // ====================

struct bdeu_ArrayUtil {
    // This 'struct' provides a namespace for a collection of function
    // templates providing access to the size and iterator of statically
    // sized built-in arrays

    // CLASS METHODS

    template <class T, bsl::size_t Size>
    static T* begin(T (&array)[Size]);
        // Return an iterator pointing to the first element of the 'array'
        // passed as argument. 

    template <class T, bsl::size_t Size>
    static T* end(T (&array)[Size]);
    	// Return the past-the-end iterator for the 'array' passed as argument.

    template <class T, bsl::size_t Size>
    static bsl::reverse_iterator<T*> rbegin(T (&array)[Size]);
        // Return a reverse iterator pointing to the last element of the
        // 'array' passed as argument.

    template <class T, bsl::size_t Size>
    static bsl::reverse_iterator<T*> rend(T (&array)[Size]);
        // Return the past-the-end reverse iterator for the 'array' passed as
        // argument.

    template <class T, bsl::size_t Size>
    static bsl::size_t size(T (&array)[Size]);
        // Return the number of elements in the passed 'array'.

    template <class T, bsl::size_t Size>
    static char (&sizer(T (&array)[Size]))[Size];
        // Return a reference to an array of char with the same number of
        // elements as the passed array. This function is *not* implemented
        // and can only be used in a non-evaluated context. The function is
        // used together with 'sizeof()' to get a constant expression with the
        // size of the array.
        //@SEE_ALSO: BDEU_ARRAYUTIL_SIZE
};

}

// ---------------------------------------------------------------------------

template <class T, bsl::size_t Size>
T* BloombergLP::bdeu_ArrayUtil::begin(T (&array)[Size])
{
    return array;
}

template <class T, bsl::size_t Size>
T* BloombergLP::bdeu_ArrayUtil::end(T (&array)[Size])
{
    return array + Size;
}

template <class T, bsl::size_t Size>
bsl::reverse_iterator<T*> BloombergLP::bdeu_ArrayUtil::rbegin(T (&array)[Size])
{
    return bsl::reverse_iterator<T*>(end(array));
}

template <class T, bsl::size_t Size>
bsl::reverse_iterator<T*> BloombergLP::bdeu_ArrayUtil::rend(T (&array)[Size])
{
    return bsl::reverse_iterator<T*>(begin(array));
}

template <class T, bsl::size_t Size>
bsl::size_t BloombergLP::bdeu_ArrayUtil::size(T (&)[Size])
{
    return Size;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
#endif
