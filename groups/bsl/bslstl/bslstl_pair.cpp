// bslstl_pair.cpp                                                    -*-C++-*-
//
// IMPLEMENTATION NOTES: The 'bslstl' package provides an implementation of
// 'bsl::pair' but not 'bsl::make_pair'.  This is because, if 'bsl::make_pair'
// were provided, it would cause compilation errors on valid code, as in the
// example below:
//..
//  namespace BloombergLP {
//
//  void foo()
//  {
//      std::string str;
//      double      price;
//
//      make_pair(str, price);  // error, won't compile
//  }
//
//  }  // close namespace BloombergLP
//..
// The above code will not compile because of ambiguity between
// 'std::make_pair' and 'bsl::make_pair' caused by ADL (Argument Dependent
// Lookup).  ADL is the process the compiler used to determine where's a
// function located when a namespace qualification is not specified.  The
// compiler will look at the namespaces that the function's arguments belong to
// to locate the function.  When the compiler finds the function in multiple
// namespaces, there is an ambiguity.  Ambiguity in the example above is caused
// by the type 'std::string', which is defined as:
//..
//  typedef basic_string<char, std::char_traits<char>, bsl::allocator<char> >
//                                                                      string;
//..
// ADL not only looks at the namespace of the argument, but also the namespaces
// of any templatized types that defines the argument itself.  Since both
// namespace 'std' and 'bsl' namespaces appear in the template parameters, both
// namespaces are considered as potential candidates that contain the function
// for 'make_pair'.  If 'make_pair' were defined in the 'bsl' namespace (i.e.,
// provided by Bloomberg), then the compiler would have a choice of two valid
// implementations ('bsl::make_pair' and 'std::make_pair') and would emit a
// compilation error.
//
// Another approach is to use purely 'std::pair' and 'std::make_pair'.  The
// downside of this approach is that we break code that already uses pair
// constructors with allocators.
//
// Therefore, this component provides 'bsl::pair', but lets the compiler
// provide 'std::make_pair'.  'bsl::make_pair' is simply defined as:
//..
//  namespace bsl {
//      using std::make_pair;
//  }
//..
// 'std::make_pair' will return a 'std::pair', which is then implicitly
// converted to a 'bsl::pair' by the conversion constructor.  This approach
// both maintain consistency with existing Bloomberg managed code (where pairs
// take allocators), and does not cause overloading ambiguity.  The only
// limitation is that conversion will not occur automatically for arguments of
// template functions:
//..
//  template <class T>
//  void foo(bsl::pair<int, int> p)
//  {
//  }
//
//  int main()
//  {
//      foo(std::make_pair(1, 2));  // error, won't compile
//  }
//..
// This will not compile because automatic conversions are not considered for
// template functions.  Users will have to perform an explicit cast before
// making the function call.  This approach seems to be the lessor of three
// evils, as there are only one case of this failure in robo.
//
// DELIBERATE DEVIATION FROM THE STANDARD INTERFACE
// ------------------------------------------------
// With improved test coverage for the C++14 standard interface, deliberate
// deviation from the published interface became necessary.  In addition to the
// constructors taking 'bslma::Allocator *' arguments for allocator-aware
// construction, the 5 special members of 'pair' are deliberately NOT declared,
// accepting the implicit declaration or deletion of those functions.  This is
// essential in order to support 'pair's with 'bslma::ManagedPtr' members, or
// any other type using 'std::auto_ptr'-like semantics.  The standard
// declaration of 'pair(const pair&) = default;' breaks for such types, as we
// instead need the implicitly decalred signature 'pair(pair&) = default;'
// without the 'const'.  For C++11, the '= default' declaration is required as
// the declaration of a move-assignment operator inhibits (deletes) the
// implicit declaration of a copy constructor.  We cannot simply default the
// declaration of the assignment operators, as a 'pair' with a reference member
// would not compile, but has well-defined semantics in the standard.  For the
// 'bsl' implementation, this is resolved by moving the problem down into our
// two base classes, which provide a partial specialization for reference
// elements, and so allow the 'pair' template to implicitly declare and define
// all 5 special members in terms of its base classes.  Note that the C++20
// language will update the definition of '= default' for copy constructors so
// that the standard interface /would/ work correctly for us - but we are a
// long way from being able to rely on (the not yet final) C++20 as our minimal
// language conformance level.

#include <bslstl_pair.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {

}  // close enterprise namespace

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
