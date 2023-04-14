// bslmf_integersequence.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMF_INTEGERSEQUENCE
#define INCLUDED_BSLMF_INTEGERSEQUENCE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a template parameter pack of integers.
//
//@CLASSES:
//  bslmf::IntegerSequence: a compile-time sequence of integers
//
//@DESCRIPTION: This component defines a class template,
// 'bslmf::IntegerSequence', which can be used in parameter pack expansions.
//
// 'bslmf::IntegerSequence' meets the requirements of the 'integer_sequence'
// template introduced in the C++14 standard [intseq.intseq] and can be used in
// a client's code if the compiler supports the C++11 standard.
//
// Note, that this component has no (emulated) support for pre-standard C++11
// compilers, as its only purpose is to be used in deduction with template
// parameter packs.  There is no language support to emulate in earlier
// compilers.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Pass C-array as a parameter to a function with variadic template
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to initialize a C-Array of known size 't_N' with data read
// from a data source using a library class that provides a variadic template
// interface that loads a data of variable length into the supplied parameter
// pack.
//
// First, define a class template 'DataReader',
//..
// template <std::size_t t_N>
// class DataReader {
//   public:
//..
// Then, implement a method that loads the specified parameter pack 'args' with
// data read from a data source.
//..
//     template <class ...t_T>
//     void read(t_T*... args) const
//     {
//         static_assert(sizeof...(args) == t_N, "");
//         read_impl(args...);
//     }
//..
// Next, for the test purpose provide simple implementation of the recursive
// variadic 'read_impl' function that streams the index of the C-Array's
// element to 'stdout'.
//..
// private:
//     template <class t_U, class ...t_T>
//     void read_impl(t_U*, t_T*... args) const
//     {
//         printf("read element #%i\n",
//                static_cast<int>(t_N - 1 - sizeof...(args)));
//         read_impl(args...);
//     }
//..
// Then, implement the recursion break condition.
//..
//     void read_impl() const
//     {
//     }
// };
//..
// Next, define a helper function template 'readData' that expands the
// parameter pack of indices 't_I' and invokes the variadic template 'read'
// method of the specified 'reader' object.
//..
// namespace {
// template<class t_R, class t_T, std::size_t... t_I>
// void readData(const t_R&  reader,
//               t_T      *data,
//               bslmf::IntegerSequence<std::size_t, t_I...>)
// {
//     reader.read(&data[t_I]...);
//         // In pseudocode, this is equivalent to:
//         // reader.read(&data[0],
//         //             &data[1],
//         //             &data[2],
//         //             ...
//         //             &data[t_N-1]);
// }
// }
//..
// Finally, define a 'data' C-Array and 'reader' variables and pass them to the
// 'readData' function as parameters.
//..
// constexpr int      k_SIZE = 5;
// DataReader<k_SIZE> reader;
// int                data[k_SIZE] = {0};
//
// readData(reader,
//          data,
//          bslmf::IntegerSequence<size_t, 0, 1, 2, 3, 4>());
//..
// Note that using a direct call to the 'bslmf::IntegerSequence' constructor
// looks a bit clumsy here.  The better approach is to use alias template
// 'bslmf::MakeIntegerSequence', that creates a collection of increasing
// integer values, having the specified t_N-value length.  The usage example in
// that component shows this method more clearly.  But we can not afford its
// presence here to avoid a cycle/levelization violation.
//
// The streaming operator produces output in the following format on 'stdout':
//..
// read element #0
// read element #1
// read element #2
// read element #3
// read element #4
//..

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>

#include <bsls_libraryfeatures.h>

#include <cstddef>  // 'std::size_t'

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE

namespace BloombergLP {
namespace bslmf {
                            // ======================
                            // struct IntegerSequence
                            // ======================

template <class t_T, t_T ...t_INTS>
struct IntegerSequence {
    // This class template represents a compile-time sequence of integers.
    // When passed as an argument to a function template, the specified
    // parameter pack 't_INTS' can be deduced and used a in pack expansion.

    // TYPES
    typedef t_T value_type;
        // 'value_type' is an alias to the template parameter 't_T' that
        // represents an integer type to use for the elements of the sequence.

    // CLASS METHODS
    static constexpr std::size_t size() noexcept;
        // Return the number of elements in 't_INTS'.
};

}  // close package namespace

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                            // ----------------------
                            // struct IntegerSequence
                            // ----------------------

template <class t_T, t_T... t_INTS>
inline
constexpr std::size_t bslmf::IntegerSequence<t_T, t_INTS...>::size() noexcept
{
    return sizeof...(t_INTS);
}

}  // close enterprise namespace

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE

#endif  // INCLUDED_BSLMF_INTEGERSEQUENCE

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
