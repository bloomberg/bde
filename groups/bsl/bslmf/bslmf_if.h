// bslmf_if.h                                                         -*-C++-*-
#ifndef INCLUDED_BSLMF_IF
#define INCLUDED_BSLMF_IF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time 'if/else' (conditional) meta-function.
//
//@DEPRECATED: Use 'bslmf_conditional' instead.
//
//@CLASSES:
//  bslmf::If: meta-function for compile-time selection of one of two types
//
//@SEE_ALSO: bslmf_assert, bslmf_nil
//
//@DESCRIPTION: This component contains the template class meta-function
// 'bslmf::If' that is parameterized on three arguments.  The first argument is
// a (compile-time constant) integral expression that is interpreted by the
// meta-function as a boolean conditional; the other two parameters accept type
// arguments.  If the value of the first argument is non-zero (true), the
// meta-function "returns" its second argument (i.e., corresponding to the
// first type parameter); otherwise it returns its third argument (the second
// type parameter).  If the selected type argument is not explicitly specified,
// the meta-function returns the default 'bslmf::Nil' type.
//
///Meta-Function Return Types and Values
///-------------------------------------
// A meta-function is a class template that evaluates, at compile-time, to one
// or more types and values.  An example of a simple meta-function that adds
// two (compile-type constant) integer values is the following 'Plus' class
// template:
//..
//  template <int A, int B>
//  struct Plus {
//      enum { VALUE = A + B };  // 'VALUE' is meta-function result
//  };
//..
// The initializer of the 'VALUE' enumerator is the compile-time summation of
// the constant values 'A' and 'B'.  The result "returned" by 'Plus' is
// provided by the 'VALUE' enumerator.
//
// An example where a type is returned from a meta-function rather than a value
// is illustrated by 'SelectLarger' below.  The 'SelectLarger' meta-function
// selects the larger of two types.  The result "returned" by 'SelectLarger' is
// provided by the 'SelectLarger<...>::Type' 'typedef':
//..
//  template <class T1, class T2>
//  struct SelectLarger {
//      template <class U1, class U2, bool V = (sizeof(T1) > sizeof(T2))>
//      struct Impl {
//          typedef T1 Type;
//      };
//      template <class U1, class U2>
//      struct Impl <U1, U2, false> {
//          typedef T2 Type;
//      };
//
//      typedef typename Impl<T1, T2>::Type Type;  // 'Type' is meta-function
//                                                 // result
//  };
//..
// The preceding two examples illustrate the naming conventions used throughout
// 'bslmf' to denote the types and values returned by meta-functions.  In
// particular, enumerators or 'static const' integral variables named 'VALUE'
// provide the results of value-returning meta-functions, and nested types
// named 'Type' provide the results of type-returning meta-functions.
//
///Usage
///-----
// The following snippets of code illustrate basic use of the 'bslmf::If'
// meta-function.  The examples make use of the following declarations to
// identify the type that is selected by a given constant integral expression:
//..
//  enum TypeCode { T_UNKNOWN = 0, T_CHAR = 1, T_INT = 2, T_NIL = 3 };
//
//  TypeCode whatType(char)       { return T_CHAR; }
//  TypeCode whatType(int)        { return T_INT; }
//  TypeCode whatType(bslmf::Nil) { return T_NIL; }
//  TypeCode whatType(...)        { return T_UNKNOWN; }
//..
// In the following example, the meta-function condition (the first argument to
// 'bslmf::If') evaluates to true (non-zero).  Thus, 'bslmf::If<...>::Type' is
// a synonym for 'int'; i.e., it "evaluates" (at compile time) to 'int':
//..
//  typedef int  T1;  assert(1 <  sizeof(T1));
//  typedef char T2;  assert(1 == sizeof(T2));
//
//  typedef bslmf::If<(sizeof(T1) > sizeof(T2)), T1, T2>::Type LargerType;
//  assert(T_INT == whatType(LargerType()));
//..
// In the next example, the condition argument evaluates to false (zero).  In
// this case, 'bslmf::If<...>::Type' evaluates to 'bslmf::Nil' since the third
// template argument (the "else" type) is not explicitly specified:
//..
//  typedef bslmf::If<(sizeof(T2) > 1), int>::Type Type2;
//  assert(T_NIL == whatType(Type2()));
//..

#include <bslscm_version.h>

#include <bslmf_conditional.h>
#include <bslmf_nil.h>

namespace BloombergLP {

namespace bslmf {

                                 // =========
                                 // struct If
                                 // =========

template <int t_CONDITION,
          class t_IF_TRUE_TYPE  = Nil,
          class t_IF_FALSE_TYPE = Nil>
struct If {
    // This meta-function selects 't_IF_TRUE_TYPE' if 't_CONDITION' is
    // non-zero.  and 't_IF_FALSE_TYPE' otherwise.

    typedef typename bsl::
        conditional<t_CONDITION, t_IF_TRUE_TYPE, t_IF_FALSE_TYPE>::type Type;
};

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslmf_If
#undef bslmf_If
#endif
#define bslmf_If bslmf::If
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

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
