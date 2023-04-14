// bslmf_nthparameter.h                                               -*-C++-*-
#ifndef INCLUDED_BSLMF_NTHPARAMETER
#define INCLUDED_BSLMF_NTHPARAMETER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Metafunction to return the Nth type parameter in a parameter pack
//
//@CLASSES: bslmf::NthParameter<t_N, PARAM_0, t_PARAMS...>
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component contains a metafunction that treats a
// parameter pack of types as compile-time array of types, returning the Nth
// type (counting from zero).  It is useful for implementing types like
// 'tuple' that need access to a specific element of a parameter pack.
//
///Usage
///-----
// We wish to implement a 'tuple'-like class that holds a heterogeneous
// collection of elements, each of which might have a different type.  The
// metafunction, 'my_tuple_element<I, my_tuple<ELEMS...>>::Type' would be type
// of the 'I'th element in the tuple (where 'I' is zero-based).
//
// First, we define our 'my_tuple' class template.  The body of the class is
// unimportant for this usage examples:
//..
// template <class... ELEMS>
// class my_tuple {
//     // ...
// };
//..
// Then, we use 'bslmf::NthParameter' to implement 'my_tuple_element':
//..
// #include <bslmf_nthparameter.h>
//
// template <std::size_t I, class TUPLE>
// struct my_tuple_element;  // Not defined
//
// template <std::size_t I, class... ELEMS>
// struct my_tuple_element<I, my_tuple<ELEMS...> > {
//     typedef typename bslmf::NthParameter<I, ELEMS...>::Type Type;
// };
//..
// Finally, we test this implementation using 'bsl::is_same':
//..
// #include <bslmf_issame.h>
//
// int main()
// {
//    typedef my_tuple<int, short, char*> ttype;
//
//    assert((bsl::is_same<int,    my_tuple_element<0, ttype>::Type>::value));
//    assert((bsl::is_same<short,  my_tuple_element<1, ttype>::Type>::value));
//    assert((bsl::is_same<char *, my_tuple_element<2, ttype>::Type>::value));
//
//    assert(! (bsl::is_same<short, my_tuple_element<0, ttype>::Type>::value));
// }
//..

#include <bsls_compilerfeatures.h>

#include <bslmf_assert.h>

#include <bsls_compilerfeatures.h>

#include <cstddef>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Fri Dec 16 11:47:20 2022
// Command line: sim_cpp11_features.pl bslmf_nthparameter.h
# define COMPILING_BSLMF_NTHPARAMETER_H
# include <bslmf_nthparameter_cpp03.h>
# undef COMPILING_BSLMF_NTHPARAMETER_H
#else

namespace BloombergLP {

namespace bslmf {

                        // ===========================
                        // class template NthParameter
                        // ===========================

struct NthParameter_Sentinel;  // Declared but not defined

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=15

template <std::size_t t_N,
          class t_FIRST_PARAM = NthParameter_Sentinel,
          class... t_PARAMS>
struct NthParameter {
    // Metafunction to compute the specified 't_N'th element of the specified
    // 't_PARAMS' template parameter pack.  The 'Type' nested typedef will
    // match the 't_N'th element of 't_PARAMS', where 't_N' is zero-based (so
    // that an 't_N' of zero corresponds to the first parameter.

    typedef typename NthParameter<t_N - 1, t_PARAMS...>::Type Type;
        // The type of the Nth parameter, computed by recursively stripping off
        // the first parameter until t_N == 0.
};

// ============================================================================
//                              IMPLEMENTATION
// ============================================================================

template <class t_FIRST_PARAM, class... t_PARAMS>
struct NthParameter<0, t_FIRST_PARAM, t_PARAMS...> {
    // Specialization of 'NthParameter' for when 't_N' is zero.

    typedef t_FIRST_PARAM Type;
        // The type of the 0th parameter.
};

#endif

template <>
struct NthParameter<0, NthParameter_Sentinel> {
    // Specialization of 'NthParameter' for when 't_N' exceeds the actual
    // number of parameters.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
    // No 'Type' member is defined.
#else
    // There are no dependent parameters because this is a full specialization.
    // When used in another simulated variadic template, the compiler may
    // attempt to evaluate the 'Type' member even when that client is not
    // actually instantiated.  To avoid a spurious compilation error, we must
    // therefore make sure that 'Type' is defined, even if it is defined as an
    // incomplete class.
    typedef NthParameter_Sentinel Type;
#endif
};

}  // close package namespace
}  // close enterprise namespace

#endif // End C++11 code

#endif // ! defined(INCLUDED_BSLMF_NTHPARAMETER)

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
