// bslmf_typeidentity.h                                               -*-C++-*-
#ifndef INCLUDED_BSLMF_TYPEIDENTITY
#define INCLUDED_BSLMF_TYPEIDENTITY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a template metafunction that returns its argument.
//
//@CLASSES:
//  bsl::type_identity: template metafunction that returns its argument.
//  bsl::type_identity_t: alias for the return type of 'type_identity'
//
//@MACROS:
//  BSLMF_TYPEIDENTITY_T: Macro equivalent to 'bsl::type_identity_t' for C++03
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a trivial template metafunction class,
// 'bsl::type_identity' that takes one type template argument and produces the
// argument type as its result; i.e., 'bsl::type_identity<t_TYPE>::type' is
// simply an alias for 't_TYPE'.  This metafunction is used in situations where
// a no-op metafunction is desired or where template type deduction should be
// suppressed.  This component also provides, for C++11 and later, an alias,
// 'bsl::type_identity_t' such that 'bsl::type_identity_t<t_TYPE>' is short
// hand for 'typename bsl::type_identity<t_TYPE>::type'.
//
// The templates in this component have identical functionality to the standard
// templates, 'std::type_identity' and 'std::type_identity_t' defined in the
// '<type_traits>' header starting with C++20.
//
///Usage
///-----
//
///Example 1: Requiring explicit function template arguments
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A function template can often deduce the types of its arguments, but
// sometimes we wish to prevent such deduction and require the user to supply
// the desired type explicitly.  In this example, we'll declare a cast
// function, 'implicitCast', that is invoked 'implicitCast<T>(arg)'.  The goal
// is cast the 'arg' to type 'T', but only if 'arg' is implicitly convertible
// to 'T'.
//
// First, we'll define a type 'TestType', that is implicitly convertible from
// 'int' but only explicitly convertible from 'const char *':
//..
//  struct TestType {
//      TestType(int) { }                                           // IMPLICIT
//      explicit TestType(const char*) { }
//  };
//..
// Next, we'll define 'implicitCastNAIVE', a naive and insufficient attempt at
// defining 'implicitCast':
//..
//  template <class t_TYPE>
//  t_TYPE implicitCastNAIVE(t_TYPE arg)
//  {
//      return arg;
//  }
//..
// Next, we try to use 'implicitCastNAIVE'.  The first invocation below
// correctly casts an 'int' to 'TestType'.  The second invocation should, and
// does, fail to compile because 'const char*' is not implicitly convertible to
// 'TestType'.  In the third invocation, we forgot the '<TestType>' template
// parameter.  Surprisingly (for the user), the code compiles anyway because
// 'implicitCastNAIVE' *deduced* 't_TYPE' to be 'const char*' and returns its
// argument unmodified, i.e., doing no casting whatsoever:
//..
//  TestType v1(implicitCastNAIVE<TestType>(5));      // OK
//  TestType v2(implicitCastNAIVE<TestType>("bye"));  // Fails correctly.
//  TestType v3(implicitCastNAIVE("hello"));          // Succeeds incorrectly.
//..
// Now, we implement 'implicitCast' correctly, using 'bsl::type_identity' to
// prevent implicit template-argument deduction:
//..
//  template <class t_TYPE>
//  t_TYPE implicitCast(typename bsl::type_identity<t_TYPE>::type arg)
//  {
//      return arg;
//  }
//..
// Finally, we try using 'implicitCast' both correctly and incorrectly.  As
// before, the first invocation below correctly casts an 'int' to 'TestType'
// and second invocation correctly fails to compile.  Unlike the
// 'implicitCastNAIVE' example, however, the third invocation correctly fails
// to compile because 't_TYPE' is not deduceable for a parameter of type
// 'bsl::type_identity<t_TYPE>::type'.
//..
//  TestType v4(implicitCast<TestType>(5));      // OK
//  TestType v5(implicitCast<TestType>("bye"));  // Fails correctly.
//  TestType v6(implicitCast("hello"));          // Fails correctly.
//..
// Note that 'typename bsl::type_identity<t_TYPE>::type' can be replaced by the
// more concise 'bsl::type_identity_t<t_TYPE>' (compatible with C++11 and
// later) or 'BSLMF_TYPEIDENTITY_T(t_TYPE)' (compatible with all C++ versions).
//
///Example 2: preventing ambiguous argument deduction in function templates
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we illustrate how to prevent ambiguities when type
// deductions occurs on multiple function-template arguments.  Our sample
// function returns a number within a range two-thirds of the way between the
// start and end of the range.  The types of the arguments determine the type
// of the result.
//
// First, we implement the function using a simple but potentially ambiguous
// interface:
//..
//  template <class NUMTYPE>
//  inline NUMTYPE twoThirdsOfTheWay1(NUMTYPE first, NUMTYPE last)
//  {
//      return first + (last - first) * 2 / 3;
//  }
//..
// Now, try to invoke our function.  We get into trouble when the two arguments
// have different types; the compiler is unable to deduce a single 'NUMTYPE':
//..
//  int i1 = twoThirdsOfTheWay1(0, 6);        // OK, 'NUMTYPE' is 'int'
//  double d1 = twoThirdsOfTheWay1(0, 0.75);  // Ambiguous: 'int' vs 'double'
//..
// Next, we try again, this time using 'bsl::type_identity' to suppress type
// deduction on the first argument.  The *first* argument, rather than the
// *second* argument is chosen for this treatment because the first argument of
// a numeric range is so often 0, which happens to be an 'int' but is often
// used, without losing precision, with 'unsigned', 'float', and 'double'
// values.  The second argument, conversely, usually carries a significant
// value whose type is important:
//..
//  template <class NUMTYPE>
//  inline NUMTYPE twoThirdsOfTheWay(BSLMF_TYPEIDENTITY_T(NUMTYPE) first,
//                                   NUMTYPE                       last)
//  {
//      return first + (last - first) * 2 / 3;
//  }
//
//  int i2 = twoThirdsOfTheWay(0, 6);        // OK, 'NUMTYPE' is 'int'
//  double d2 = twoThirdsOfTheWay(0, 0.75);  // OK, 'NUMTYPE' is 'double'
//..
// Finally, we verify that our 'twoThirdsOfTheWay' function worked correctly:
//..
//  int main()
//  {
//      assert(4 == i2);
//      assert(0.5 == d2);
//      assert(0 == twoThirdsOfTheWay(4U, -2));
//  }
//..

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>

namespace bsl {

                    // ============================
                    // class template type_identity
                    // ============================

template <class t_TYPE>
struct type_identity {
    // Metafunction returning 't_TYPE' unchanged.

    // TYPES
    typedef t_TYPE type;
};

                    // ==============================
                    // alias template type_identity_t
                    // ==============================

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

template <class t_TYPE>
using type_identity_t = typename type_identity<t_TYPE>::type;
    // Metafunction returning 't_TYPE' unchanged.  Shorthand alias for
    // 'type_identity<t_TYPE>::type'.

#endif // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

}  // close namespace bsl

                    // ==========================
                    // macro BSLMF_TYPEIDENTITY_T
                    // ==========================

#define BSLMF_TYPEIDENTITY_T(...) \
    typename ::bsl::type_identity<__VA_ARGS__ >::type
    // Metafunction returning its type argument unchanged.  Shorthand alias for
    // 'typename type_identity<...>::type'.  The type argument to this macro
    // must be a *dependent type* of the function template or class template in
    // which it is used, i.e., a template parameter or type related to a
    // template parameter.  Note that this macro exists for compatibility with
    // C++03; for C++11 and later compilers, 'bsl::type_identity_t' is
    // preferred.
    //
    // Implementation note: space before closing '>' is important to avoid
    // '>>' issues in C++03.

#endif // ! defined(INCLUDED_BSLMF_TYPEIDENTITY)

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
