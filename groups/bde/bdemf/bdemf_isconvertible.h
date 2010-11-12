// bdemf_isconvertible.h           -*-C++-*-
#ifndef INCLUDED_BDEMF_ISCONVERTIBLE
#define INCLUDED_BDEMF_ISCONVERTIBLE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time type conversion checker.
//
//@DEPRECATED: Use 'bslmf_isconvertible' instead.
//
//@CLASSES:
//  bdemf_IsConvertible: compile-time type conversion checker
//
//@AUTHOR: Paul Staniforth (pstaniforth)
//
//@SEE_ALSO: 'bdemf_metavalue.h'
//
//@DESCRIPTION: This component defines a meta-function (i.e., a compile-time
// function using the C++ type system) for checking whether a conversion exists
// from one C++ type to another.  'bdemf_IsConvertible' is a template struct
// that takes two parameters and provides an enumerator with a predicate value
// of '1' if a conversion from the first parameter to the second exists, and a
// value of '0' otherwise.  For example, a conversion exists from an 'int' to
// a 'char', but one does not exists from an 'int' to an 'char' pointer.
//..
//   static const int A = bdemf_IsConvertible<int, char >::VALUE; // A is 1
//   static const int B = bdemf_IsConvertible<int, char*>::VALUE; // B is 0
//..
// Note that the result is undefined if either parameter is 'void'.  Note also
// that 'bdemf_IsConvertible' can produce compiler errors if the conversion is
// ambiguous.  For example,
//..
//   struct A {};
//   struct B : public A {};
//   struct C : public A {};
//   struct D : public B, public C {};
//
//   static int const C = bdemf_IsConvertible<D*, A*>::VALUE; // ERROR!
//..
//
///USAGE
///-----
// This meta-function can be used to select an appropriate function (at compile
// time) based on the convertibility of one type to another without causing a
// compiler error by actually trying the conversion.  This implementation
// technique is especially useful when building generic containers that use an
// allocator protocol to acquire resources.  As a design goal, we want to pass
// the container's allocator to contained types if they provide an appropriate
// constructor.
//
// Consider the following example of an implementation of some container's
// 'addObj' method that adds a new instance of its contained type (in the
// type's default state).  The method calls an overloaded function 'createObj'
// to create a new instance of the parameterized type in its internal array.
// The idea is to invoke one version of 'createObj' if the type provides a
// constructor that takes a pointer to an allocator as its sole argument, and
// another version if the type provides only a default constructor.
//
// The first 'createObj' function takes a 'bdemf_MetaInt<0>' as its last
// argument, whereas the second 'createObj' function takes a
// 'bdemf_MetaInt<1>' object.  The result of the 'isConvertible'
// meta-function (i.e., its 'Type' member) is used to create the last argument
// to 'createObj'.  Neither version of 'createObj' makes use of this argument
// -- it is used only to differentiate the argument list so we can overload
// the function.
//..
//     template<class T>
//     void createObj(T *space, MyAllocator *, bdemf_MetaInt<1>)
//     {
//        // Use the type's default constructor if
//        // bdemf_IsConvertible<MyAllocator*, T>::VALUE == 0 -- i.e., there is
//        // no conversion from a MyAllocator pointer to a T.
//
//        new (space) T();
//     }
//
//     template<class T>
//     void createObj(T *space, MyAllocator *alloc, bdemf_MetaInt<0>)
//     {
//        // Use the type's constructor that takes a pointer to an allocator if
//        // bdemf_IsConvertible<MyAllocator*, T>::VALUE == 1, i.e., there is
//        // a conversion from a MyAllocator pointer to a T.
//
//        new (space) T(alloc);
//     }
//
//     template <class T>
//     class MyContainer {
//         T *d_array_p;           // underlying array
//         MyAllocator *d_alloc_p; // allocator protocol
//         int d_length;           // logical length of array
//         // ...
//
//         void resizeInternalArrayIfNeeded() { /* ... */ };
//       public:
//         MyContainer(MyAllocator *alloc)
//         : d_alloc_p(alloc)
//         , d_length(0)
//         {
//             d_array_p = (T*) d_alloc_p->allocate(sizeof(T));
//         }
//
//         void addObj()
//         {
//             resizeInternalArrayIfNeeded();
//             createObj(d_array_p + d_length++,
//                       d_alloc_p,
//                       bdemf_IsConvertible<MyAllocator*, T>::Type());
//         }
//     };
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef bdemf_IsConvertible
#define bdemf_IsConvertible   bslmf_IsConvertible
    // Implement a meta function which computes -- at compile time -- whether
    // 'FROM_TYPE' is convertible to 'TO_TYPE'.  Note that if 'TO_TYPE' is not
    // a reference type, then the cv-qualification of 'TO_TYPE' is ignored.

    // We pass in a const& because the 'TO_TYPE' may not have a
    // copy-constructor but the 'FROM_TYPE' could have a conversion operator to
    // a const& (not a value), and we still want the conversion to succeed.
    // Also, if the TO_TYPE is incomplete, we don't want to require a
    // copy-constructor.
#endif

#ifndef bdemf_IsConvertible_Imp
#define bdemf_IsConvertible_Imp bslmf_IsConvertible_Imp
    // Private class.

    // General instance.  Implements 'bslmf_IsConvertible<FROM_TYPE, TO_TYPE>'
    // where conversion to 'TO_TYPE' is not necessarily the same as conversion
    // to 'const TO_TYPE&'.  Will not compile if 'TO_TYPE' is an incomplete
    // type (although it may be a pointer or reference to incomplete type).
#endif



namespace BloombergLP {

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
