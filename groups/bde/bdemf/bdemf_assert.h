// bdemf_assert.h                  -*-C++-*-
#ifndef INCLUDED_BDEMF_ASSERT
#define INCLUDED_BDEMF_ASSERT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time-assert facility.
//
//@DEPRECATED: Use 'bslmf_assert' instead.
//
//@CLASSES:
//  BDEMF_ASSERT: compile-time assert macro
//
//@AUTHOR: Shawn Edwards (sedwards)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a macro that will generate a
// compile-time error if its constant integral expression argument evaluates to
// false (i.e., a value of '0').  The 'BDEMF_ASSERT' macro is similar to the
// standard run-time 'assert' macro, except that its argument is evaluated at
// compile-time, it produces no executable code, and it can be used safely in
// header files.
//..
//     BDEMF_ASSERT(sizeof(int) >= sizeof(char));   // OK
//     BDEMF_ASSERT(sizeof(double) < sizeof(char)); // COMPILE ERROR!
//..
// The second line will result in a compile error with a message stating that
// the type 'BDEMF_COMPILE_TIME_ASSERTION_FAILURE' is incomplete, usually
// preceeded by the file and line number where the macro was used.  The
// following errata is typical of most compilers:
//..
//     $ CC -g myfile.cpp
//     "myfile.cpp", line 86: Error: The type
//     "BDEMF_COMPILE_TIME_ASSERTION_FAILURE<0>" is incomplete.
//     1 Error(s) detected.
//..
// If the macro argument if true, the macro will generate an innocuous typedef
// with a name that is the concatenation of the symbol 'bdemf_Assert', and the
// value of __LINE__ (which will be the line number in the file where the macro
// was called).  For example, the first line from the example above might
// result in the following statement:
//..
//    typedef bdemf_AssertTest<1> bdemf_Assert_85;
//..
// Note that these generated typedefs are implementation details of the
// compile-time checking facility and are not intended to be used directly
// outside of this component.
//
// Attempting to invoke 'BDEMF_ASSERT' on a non-compile-time value will
// typically result in a compilation error.
//
// 'BDEMF_ASSERT' can be used at namespace, class, and function scope to
// assert compile-time conditions.  !WARNING:! a compiler bug on certain
// platforms produces an error when the 'BDEMF_ASSERT' macro is used more than
// once on the *same* line in class scope.
//..
//   +---------------------------------------------
//   | // mytype.h
//   | #ifndef INCLUDED_BDEMF_ASSERT
//   | #include <bdemf_assert.h>
//   | #endif
//   |
//   | class MyType {
//   |     BDEMF_ASSERT(sizeof(int) >= sizeof(char));  // OK
//   |     BDEMF_ASSERT(sizeof(int) >= sizeof(char));  // OK
//   |     BDEMF_ASSERT(1 == 1);                       // OK
//   |     BDEMF_ASSERT(1 == 1); BDEMF_ASSERT(1 == 1); // SAME LINE - MIGHT
//   |                                                 // CAUSE ERROR!
//   |
//   |     int d_data;
//   |     ...
//   |     void foo();
//   |     ...
//   | };
//
//         +---------------------------------------------
//         | // mytype.cpp
//         | #include <mytype.h>
//         | #include <bdemf_assert.h>
//         |
//         | BDEMF_ASSERT(sizeof(int) >= sizeof(char));
//         |
//         | void MyType::foo()
//         | {
//         |     BDEMF_ASSERT(sizeof(int) >= sizeof(char));
//         |     ...
//         | }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef BDEMF_ASSERT
#define BDEMF_ASSERT(expr)  BSLMF_ASSERT(expr)
#endif

namespace BloombergLP {

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
