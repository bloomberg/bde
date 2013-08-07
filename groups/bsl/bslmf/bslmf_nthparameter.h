// bslmf_nthparameter.h                  -*-C++-*-
#ifndef INCLUDED_BSLMF_NTHPARAMETER
#define INCLUDED_BSLMF_NTHPARAMETER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Metafunction to return the Nth type parameter in a parameter pack
//
//@CLASSES: bslmf::NthParameter<N, PARAM_0, PARAMS...>
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component contains a metafunction that treates a
// parameter pack of types as compile-time array of types, returning the Nth
// type (counting from zero).  It is useful for implementing types like
// 'tuple' that need access to a specific element of a parameter pack.
//
///Usage
///-----
// We wish to implement a 'tuple'-like class that holds a heterogenous
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
// class my_tuple_element;  // Not defined
//
// template <std::size_t I, class... ELEMS>
// class my_tuple_element<I, my_tuple<ELEMS...> > {
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

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_CSTDLIB
#include <cstdlib>
#define INCLUDED_CSTDLIB
#endif

namespace BloombergLP {

namespace bslmf {

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

                        // ===========================
                        // class template NthParameter
                        // ===========================

template <std::size_t N, class PARAM_0 = void, class... PARAMS>
struct NthParameter {
    // Metafunction to compute the specified 'N'th element of the spcified
    // 'PARAMS' template parameter pack.  The 'Type' nested typedef will match
    // the 'N'th element of 'PARAMS', where 'N' is zero-based (so that an 'N'
    // of zero corresponds to the first parameter.

    typedef typename NthParameter<N - 1, PARAMS...>::Type Type;
        // The type of the Nth parameter, computed by recursively stripping
        // off the first parameter until N == 0.
};

// ===========================================================================
//                      IMPLEMENTATION
// ===========================================================================

template <class PARAM_0, class... PARAMS>
struct NthParameter<0, PARAM_0, PARAMS...> {
    // Specialization of 'NthParameter' for when 'N' is zero.
    
    typedef PARAM_0 Type;
        // The type of the 0th parameter.
};

#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslmf_nthparameter.h


template <std::size_t N,
          class PARAM_0,
          class PARAMS_0 = BSLS_COMPILERFEATURES_NILT,
          class PARAMS_1 = BSLS_COMPILERFEATURES_NILT,
          class PARAMS_2 = BSLS_COMPILERFEATURES_NILT,
          class PARAMS_3 = BSLS_COMPILERFEATURES_NILT,
          class PARAMS_4 = BSLS_COMPILERFEATURES_NILT,
          class PARAMS_5 = BSLS_COMPILERFEATURES_NILT,
          class PARAMS_6 = BSLS_COMPILERFEATURES_NILT,
          class PARAMS_7 = BSLS_COMPILERFEATURES_NILT,
          class PARAMS_8 = BSLS_COMPILERFEATURES_NILT,
          class PARAMS_9 = BSLS_COMPILERFEATURES_NILT,
          class = BSLS_COMPILERFEATURES_NILT,
          class PARAM_0,
          class PARAMS_0 = BSLS_COMPILERFEATURES_NILT,
          class PARAMS_1 = BSLS_COMPILERFEATURES_NILT,
          class PARAMS_2 = BSLS_COMPILERFEATURES_NILT,
          class PARAMS_3 = BSLS_COMPILERFEATURES_NILT,
          class PARAMS_4 = BSLS_COMPILERFEATURES_NILT,
          class PARAMS_5 = BSLS_COMPILERFEATURES_NILT,
          class PARAMS_6 = BSLS_COMPILERFEATURES_NILT,
          class PARAMS_7 = BSLS_COMPILERFEATURES_NILT,
          class PARAMS_8 = BSLS_COMPILERFEATURES_NILT,
          class PARAMS_9 = BSLS_COMPILERFEATURES_NILT,
          class = BSLS_COMPILERFEATURES_NILT>
struct NthParameter;

template <std::size_t N, class PARAM_0 = void>
struct NthParameter<N, PARAM_0, PARAM_0> {

    typedef typename NthParameter<N - 1>::Type Type;
};

template <std::size_t N, class PARAM_0 = void, class PARAMS_01>
struct NthParameter<N, PARAM_0, PARAMS_01, PARAM_0, PARAMS_01> {

    typedef typename NthParameter<N - 1, PARAMS_01>::Type Type;
};

template <std::size_t N, class PARAM_0 = void, class PARAMS_01,
                                               class PARAMS_02>
struct NthParameter<N, PARAM_0, PARAMS_01,
                                PARAMS_02, PARAM_0, PARAMS_01,
                                                    PARAMS_02> {

    typedef typename NthParameter<N - 1, PARAMS_01,
                                         PARAMS_02>::Type Type;
};

template <std::size_t N, class PARAM_0 = void, class PARAMS_01,
                                               class PARAMS_02,
                                               class PARAMS_03>
struct NthParameter<N, PARAM_0, PARAMS_01,
                                PARAMS_02,
                                PARAMS_03, PARAM_0, PARAMS_01,
                                                    PARAMS_02,
                                                    PARAMS_03> {

    typedef typename NthParameter<N - 1, PARAMS_01,
                                         PARAMS_02,
                                         PARAMS_03>::Type Type;
};

template <std::size_t N, class PARAM_0 = void, class PARAMS_01,
                                               class PARAMS_02,
                                               class PARAMS_03,
                                               class PARAMS_04>
struct NthParameter<N, PARAM_0, PARAMS_01,
                                PARAMS_02,
                                PARAMS_03,
                                PARAMS_04, PARAM_0, PARAMS_01,
                                                    PARAMS_02,
                                                    PARAMS_03,
                                                    PARAMS_04> {

    typedef typename NthParameter<N - 1, PARAMS_01,
                                         PARAMS_02,
                                         PARAMS_03,
                                         PARAMS_04>::Type Type;
};

template <std::size_t N, class PARAM_0 = void, class PARAMS_01,
                                               class PARAMS_02,
                                               class PARAMS_03,
                                               class PARAMS_04,
                                               class PARAMS_05>
struct NthParameter<N, PARAM_0, PARAMS_01,
                                PARAMS_02,
                                PARAMS_03,
                                PARAMS_04,
                                PARAMS_05, PARAM_0, PARAMS_01,
                                                    PARAMS_02,
                                                    PARAMS_03,
                                                    PARAMS_04,
                                                    PARAMS_05> {

    typedef typename NthParameter<N - 1, PARAMS_01,
                                         PARAMS_02,
                                         PARAMS_03,
                                         PARAMS_04,
                                         PARAMS_05>::Type Type;
};

template <std::size_t N, class PARAM_0 = void, class PARAMS_01,
                                               class PARAMS_02,
                                               class PARAMS_03,
                                               class PARAMS_04,
                                               class PARAMS_05,
                                               class PARAMS_06>
struct NthParameter<N, PARAM_0, PARAMS_01,
                                PARAMS_02,
                                PARAMS_03,
                                PARAMS_04,
                                PARAMS_05,
                                PARAMS_06, PARAM_0, PARAMS_01,
                                                    PARAMS_02,
                                                    PARAMS_03,
                                                    PARAMS_04,
                                                    PARAMS_05,
                                                    PARAMS_06> {

    typedef typename NthParameter<N - 1, PARAMS_01,
                                         PARAMS_02,
                                         PARAMS_03,
                                         PARAMS_04,
                                         PARAMS_05,
                                         PARAMS_06>::Type Type;
};

template <std::size_t N, class PARAM_0 = void, class PARAMS_01,
                                               class PARAMS_02,
                                               class PARAMS_03,
                                               class PARAMS_04,
                                               class PARAMS_05,
                                               class PARAMS_06,
                                               class PARAMS_07>
struct NthParameter<N, PARAM_0, PARAMS_01,
                                PARAMS_02,
                                PARAMS_03,
                                PARAMS_04,
                                PARAMS_05,
                                PARAMS_06,
                                PARAMS_07, PARAM_0, PARAMS_01,
                                                    PARAMS_02,
                                                    PARAMS_03,
                                                    PARAMS_04,
                                                    PARAMS_05,
                                                    PARAMS_06,
                                                    PARAMS_07> {

    typedef typename NthParameter<N - 1, PARAMS_01,
                                         PARAMS_02,
                                         PARAMS_03,
                                         PARAMS_04,
                                         PARAMS_05,
                                         PARAMS_06,
                                         PARAMS_07>::Type Type;
};

template <std::size_t N, class PARAM_0 = void, class PARAMS_01,
                                               class PARAMS_02,
                                               class PARAMS_03,
                                               class PARAMS_04,
                                               class PARAMS_05,
                                               class PARAMS_06,
                                               class PARAMS_07,
                                               class PARAMS_08>
struct NthParameter<N, PARAM_0, PARAMS_01,
                                PARAMS_02,
                                PARAMS_03,
                                PARAMS_04,
                                PARAMS_05,
                                PARAMS_06,
                                PARAMS_07,
                                PARAMS_08, PARAM_0, PARAMS_01,
                                                    PARAMS_02,
                                                    PARAMS_03,
                                                    PARAMS_04,
                                                    PARAMS_05,
                                                    PARAMS_06,
                                                    PARAMS_07,
                                                    PARAMS_08> {

    typedef typename NthParameter<N - 1, PARAMS_01,
                                         PARAMS_02,
                                         PARAMS_03,
                                         PARAMS_04,
                                         PARAMS_05,
                                         PARAMS_06,
                                         PARAMS_07,
                                         PARAMS_08>::Type Type;
};

template <std::size_t N, class PARAM_0 = void, class PARAMS_01,
                                               class PARAMS_02,
                                               class PARAMS_03,
                                               class PARAMS_04,
                                               class PARAMS_05,
                                               class PARAMS_06,
                                               class PARAMS_07,
                                               class PARAMS_08,
                                               class PARAMS_09>
struct NthParameter<N, PARAM_0, PARAMS_01,
                                PARAMS_02,
                                PARAMS_03,
                                PARAMS_04,
                                PARAMS_05,
                                PARAMS_06,
                                PARAMS_07,
                                PARAMS_08,
                                PARAMS_09, PARAM_0, PARAMS_01,
                                                    PARAMS_02,
                                                    PARAMS_03,
                                                    PARAMS_04,
                                                    PARAMS_05,
                                                    PARAMS_06,
                                                    PARAMS_07,
                                                    PARAMS_08,
                                                    PARAMS_09> {

    typedef typename NthParameter<N - 1, PARAMS_01,
                                         PARAMS_02,
                                         PARAMS_03,
                                         PARAMS_04,
                                         PARAMS_05,
                                         PARAMS_06,
                                         PARAMS_07,
                                         PARAMS_08,
                                         PARAMS_09>::Type Type;
};

template <std::size_t N, class PARAM_0 = void, class PARAMS_01,
                                               class PARAMS_02,
                                               class PARAMS_03,
                                               class PARAMS_04,
                                               class PARAMS_05,
                                               class PARAMS_06,
                                               class PARAMS_07,
                                               class PARAMS_08,
                                               class PARAMS_09,
                                               class PARAMS_10>
struct NthParameter<N, PARAM_0, PARAMS_01,
                                PARAMS_02,
                                PARAMS_03,
                                PARAMS_04,
                                PARAMS_05,
                                PARAMS_06,
                                PARAMS_07,
                                PARAMS_08,
                                PARAMS_09,
                                PARAMS_10, PARAM_0, PARAMS_01,
                                                    PARAMS_02,
                                                    PARAMS_03,
                                                    PARAMS_04,
                                                    PARAMS_05,
                                                    PARAMS_06,
                                                    PARAMS_07,
                                                    PARAMS_08,
                                                    PARAMS_09,
                                                    PARAMS_10> {

    typedef typename NthParameter<N - 1, PARAMS_01,
                                         PARAMS_02,
                                         PARAMS_03,
                                         PARAMS_04,
                                         PARAMS_05,
                                         PARAMS_06,
                                         PARAMS_07,
                                         PARAMS_08,
                                         PARAMS_09,
                                         PARAMS_10>::Type Type;
};



template <class PARAM_0>
struct NthParameter<0, PARAM_0> {
    
    typedef PARAM_0 Type;
};

template <class PARAM_0, class PARAMS_01>
struct NthParameter<0, PARAM_0, PARAMS_01> {
    
    typedef PARAM_0 Type;
};

template <class PARAM_0, class PARAMS_01,
                         class PARAMS_02>
struct NthParameter<0, PARAM_0, PARAMS_01,
                                PARAMS_02> {
    
    typedef PARAM_0 Type;
};

template <class PARAM_0, class PARAMS_01,
                         class PARAMS_02,
                         class PARAMS_03>
struct NthParameter<0, PARAM_0, PARAMS_01,
                                PARAMS_02,
                                PARAMS_03> {
    
    typedef PARAM_0 Type;
};

template <class PARAM_0, class PARAMS_01,
                         class PARAMS_02,
                         class PARAMS_03,
                         class PARAMS_04>
struct NthParameter<0, PARAM_0, PARAMS_01,
                                PARAMS_02,
                                PARAMS_03,
                                PARAMS_04> {
    
    typedef PARAM_0 Type;
};

template <class PARAM_0, class PARAMS_01,
                         class PARAMS_02,
                         class PARAMS_03,
                         class PARAMS_04,
                         class PARAMS_05>
struct NthParameter<0, PARAM_0, PARAMS_01,
                                PARAMS_02,
                                PARAMS_03,
                                PARAMS_04,
                                PARAMS_05> {
    
    typedef PARAM_0 Type;
};

template <class PARAM_0, class PARAMS_01,
                         class PARAMS_02,
                         class PARAMS_03,
                         class PARAMS_04,
                         class PARAMS_05,
                         class PARAMS_06>
struct NthParameter<0, PARAM_0, PARAMS_01,
                                PARAMS_02,
                                PARAMS_03,
                                PARAMS_04,
                                PARAMS_05,
                                PARAMS_06> {
    
    typedef PARAM_0 Type;
};

template <class PARAM_0, class PARAMS_01,
                         class PARAMS_02,
                         class PARAMS_03,
                         class PARAMS_04,
                         class PARAMS_05,
                         class PARAMS_06,
                         class PARAMS_07>
struct NthParameter<0, PARAM_0, PARAMS_01,
                                PARAMS_02,
                                PARAMS_03,
                                PARAMS_04,
                                PARAMS_05,
                                PARAMS_06,
                                PARAMS_07> {
    
    typedef PARAM_0 Type;
};

template <class PARAM_0, class PARAMS_01,
                         class PARAMS_02,
                         class PARAMS_03,
                         class PARAMS_04,
                         class PARAMS_05,
                         class PARAMS_06,
                         class PARAMS_07,
                         class PARAMS_08>
struct NthParameter<0, PARAM_0, PARAMS_01,
                                PARAMS_02,
                                PARAMS_03,
                                PARAMS_04,
                                PARAMS_05,
                                PARAMS_06,
                                PARAMS_07,
                                PARAMS_08> {
    
    typedef PARAM_0 Type;
};

template <class PARAM_0, class PARAMS_01,
                         class PARAMS_02,
                         class PARAMS_03,
                         class PARAMS_04,
                         class PARAMS_05,
                         class PARAMS_06,
                         class PARAMS_07,
                         class PARAMS_08,
                         class PARAMS_09>
struct NthParameter<0, PARAM_0, PARAMS_01,
                                PARAMS_02,
                                PARAMS_03,
                                PARAMS_04,
                                PARAMS_05,
                                PARAMS_06,
                                PARAMS_07,
                                PARAMS_08,
                                PARAMS_09> {
    
    typedef PARAM_0 Type;
};

template <class PARAM_0, class PARAMS_01,
                         class PARAMS_02,
                         class PARAMS_03,
                         class PARAMS_04,
                         class PARAMS_05,
                         class PARAMS_06,
                         class PARAMS_07,
                         class PARAMS_08,
                         class PARAMS_09,
                         class PARAMS_10>
struct NthParameter<0, PARAM_0, PARAMS_01,
                                PARAMS_02,
                                PARAMS_03,
                                PARAMS_04,
                                PARAMS_05,
                                PARAMS_06,
                                PARAMS_07,
                                PARAMS_08,
                                PARAMS_09,
                                PARAMS_10> {
    
    typedef PARAM_0 Type;
};

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.


template <std::size_t N, class PARAM_0 = void, class... PARAMS>
struct NthParameter {

    typedef typename NthParameter<N - 1, PARAMS...>::Type Type;
};


template <class PARAM_0, class... PARAMS>
struct NthParameter<0, PARAM_0, PARAMS...> {
    
    typedef PARAM_0 Type;
};

// }}} END GENERATED CODE
#endif

}  // close package namespace

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMF_NTHPARAMETER)

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
