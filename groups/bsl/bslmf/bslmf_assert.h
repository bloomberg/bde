// bslmf_assert.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLMF_ASSERT
#define INCLUDED_BSLMF_ASSERT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time assertion facility.
//
//@CLASSES:
//
//@MACROS:
//  BSLMF_ASSERT: compile-time assert macro
//
//@DESCRIPTION: This component defines a macro that will generate a
// compile-time error if its constant integral expression argument evaluates to
// false (i.e., a value of '0').  The 'BSLMF_ASSERT' macro is similar to the
// standard run-time 'assert' macro, except that its argument is evaluated at
// compile-time, it produces no executable code, and it can be used safely in
// header files.
//..
//  BSLMF_ASSERT(sizeof(int) >= sizeof(char));    // OK
//  BSLMF_ASSERT(sizeof(double) < sizeof(char));  // COMPILE ERROR!
//..
// The second line will result in a compile error with a message stating that
// the type 'BSLMF_COMPILE_TIME_ASSERTION_FAILURE' is incomplete, usually
// preceded by the file and line number where the macro was used.  The
// following error message is typical of most compilers:
//..
//  $ CC -g myfile.cpp
//  "myfile.cpp", line 86: Error: The type
//  "BSLMF_COMPILE_TIME_ASSERTION_FAILURE<0>" is incomplete.
//  1 Error(s) detected.
//..
// If the macro argument is true, the macro will generate an innocuous
// 'typedef' with a name that is the concatenation of the symbol
// 'bslmf::Assert', and the value of '__LINE__' (which will be the line number
// in the file where the macro was called).  For example, the first line from
// the example above might result in the following statement:
//..
//  typedef bslmf::AssertTest<1> bslmf_Assert_85;
//..
// Note that these generated typedefs are implementation details of the
// compile-time checking facility and are not intended to be used directly
// outside of this component.
//
// Attempting to invoke 'BSLMF_ASSERT' on a non-compile-time value will
// typically result in a compilation error.
//
// 'BSLMF_ASSERT' can be used at namespace, class, and function scope to assert
// compile-time conditions.  !WARNING:! a compiler bug on certain platforms
// produces an error when the 'BSLMF_ASSERT' macro is used more than once on
// the *same* line in class scope.
//..
//  +---------------------------------------------
//  | // mytype.h
//  | #ifndef INCLUDED_BSLMF_ASSERT
//  | #include <bslmf_assert.h>
//  | #endif
//  |
//  | class MyType {
//  |     BSLMF_ASSERT(sizeof(int) >= sizeof(char));  // OK
//  |     BSLMF_ASSERT(sizeof(int) >= sizeof(char));  // OK
//  |     BSLMF_ASSERT(1 == 1);                       // OK
//  |     BSLMF_ASSERT(1 == 1); BSLMF_ASSERT(1 == 1); // SAME LINE - MIGHT
//  |                                                 // CAUSE ERROR!
//  |
//  |     int d_data;
//  |     ...
//  |     void foo();
//  |     ...
//  | };
//
//        +---------------------------------------------
//        | // mytype.cpp
//        | #include <mytype.h>
//        | #include <bslmf_assert.h>
//        |
//        | BSLMF_ASSERT(sizeof(int) >= sizeof(char));
//        |
//        | void MyType::foo()
//        | {
//        |     BSLMF_ASSERT(sizeof(int) >= sizeof(char));
//        |     ...
//        | }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ANNOTATION
#include <bsls_annotation.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
#define BSLMF_ASSERT(BSLMF_CONSTANT_EXPRESSION) \
    static_assert((BSLMF_CONSTANT_EXPRESSION), #BSLMF_CONSTANT_EXPRESSION)
#else

namespace BloombergLP {

                             // ==============
                             // Support macros
                             // ==============

#define BSLMF_ASSERT_CAT(X, Y) BSLMF_ASSERT_CAT_IMP1(X, Y)
#define BSLMF_ASSERT_CAT_IMP1(X, Y) BSLMF_ASSERT_CAT_IMP2(X, Y)
#define BSLMF_ASSERT_CAT_IMP2(X, Y) X##Y

                             // =============
                             // Support types
                             // =============

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VER_MAJOR < 0x5130

// Using a different implementation on Sun; see BSLMF_ASSERT for details.

namespace bslmf {

struct Assert_TrueType {
    typedef int BSLMF_COMPILE_TIME_ASSERTION_FAILURE;
};

struct Assert_FalseType {
};

template <bool COND>
struct Assert_If : Assert_TrueType {
};

template <>
struct Assert_If<false> : Assert_FalseType {
};

}  // close package namespace

#else

template <bool IS_VALID>
struct BSLMF_COMPILE_TIME_ASSERTION_FAILURE;
    // Declared but not defined.  If assert macro references this type, then
    // compilation will fail (assert failure).

template <>
struct BSLMF_COMPILE_TIME_ASSERTION_FAILURE<true> {
    // Specialization for value 1 (true).  Referencing this specialization will
    // allow compilation to succeed (assert succeeded).

    enum { VALUE = 1 };
};

namespace bslmf {

template <bool IS_VALID>
struct AssertTest {
    // Instantiating this type involves instantiating its template parameter.
    // This dummy type is just used to force instantiation of a meta-function
    // used as its argument.
};

}  // close package namespace

#endif

                             // ==================
                             // macro BSLMF_ASSERT
                             // ==================

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VER_MAJOR < 0x5130

// The usual definition of the 'BSLMF_ASSERT' macro doesn't work with SunCC
// (version 10 and below) inside template classes.  Note that Sun CC has a
// quite non-conformant (read 'broken') template instantiation mechanism.  See
// DRQS 29636421 for an example of code Sun CC didn't compile correctly with
// the usual definition of 'BSLMF_ASSERT'.  Below is the definition that works
// more reliably.  This definition is not well-formed, it just happens to work
// with SunCC.  So don't use it with other compilers.

#define BSLMF_ASSERT(expr)                                         \
    struct BSLMF_ASSERT_CAT(bslmf_Assert_, __LINE__)               \
        : ::BloombergLP::bslmf::Assert_If<!!(int)(expr)>           \
    {                                                              \
        BSLMF_COMPILE_TIME_ASSERTION_FAILURE * dummy;              \
    };                                                             \
                                                                   \
    enum { BSLMF_ASSERT_CAT(bslmf_Assert_Check_, __LINE__)         \
           = sizeof(BSLMF_ASSERT_CAT(bslmf_Assert_, __LINE__)) }   \

#elif defined(BSLS_PLATFORM_CMP_MSVC)
// MSVC: __LINE__ macro breaks when /ZI is used (see Q199057 or KB199057)

#define BSLMF_ASSERT(expr) \
typedef BloombergLP::bslmf::AssertTest< \
        sizeof(BloombergLP::BSLMF_COMPILE_TIME_ASSERTION_FAILURE<!!(expr)>)> \
                bslmf_Assert_MSVC_ZI_BUG
#else

#define BSLMF_ASSERT(expr) \
typedef BloombergLP::bslmf::AssertTest< \
        sizeof(BloombergLP::BSLMF_COMPILE_TIME_ASSERTION_FAILURE<!!(expr)>)> \
               BSLMF_ASSERT_CAT(bslmf_Assert_, __LINE__) BSLS_ANNOTATION_UNUSED

#endif


}  // close enterprise namespace

#endif  // BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
