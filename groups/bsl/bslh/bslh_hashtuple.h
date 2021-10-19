// bslh_hashtuple.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLH_HASHTUPLE
#define INCLUDED_BSLH_HASHTUPLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide 'hashAppend' for 'std::tuple'.
//
//@DESCRIPTION: This component provides a free function template,
// 'bslh::hashAppend', overloaded for the 'std::tuple' class template.
// Including this function allows for 'std::tuple' types (and types that
// contain them) to be used as keys in BDE hashed containers.
//
// Note that use of this component requires that the language standard be 2011
// or later, as that is when 'std::tuple' first appears.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Hashing a Tuple of Integer Values
/// - - - - - - - - - - - - - - - - - - - - - -
// Suppose one must compute has that combines the hashes of several integer
// values, each of a different type:
//..
//  char  c = 'a';
//  short s = static_cast<short>(1);
//  int   i = 2;
//  long  l = 3L;
//..
// First, we can make that calculation by repeated invocations of a
// 'bslh::DefaultHashAlogorithm object:
//..
//  bslh::DefaultHashAlgorithm hasherS;
//  hasherS(&c, sizeof(char));
//  hasherS(&s, sizeof(short));
//  hasherS(&i, sizeof(int));
//  hasherS(&l, sizeof(long));
//
// bslh::DefaultHashAlgorithm::result_type hashS = hasherS.computeHash();
//..
// Now, the same calculation can be expressed more concisely if those same
// values are contained in a single 'std::tuple' object.
//..
//  std::tuple<char, short, int, long> t = std::make_tuple(c, s, i, l);
//
//  bslh::DefaultHashAlgorithm hasherT;
//  bslh::hashAppend(hasherT, t);
//
//  bslh::DefaultHashAlgorithm::result_type hashT = hasherT.computeHash();
//..
// Finally, we confirm that we computed the same result.
//..
//  assert(hashS == hashT);
//..

#include <bslscm_version.h>

#include <bsls_libraryfeatures.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE

#include <bslh_hash.h>

#include <type_traits> // 'std::remove_reference'
#include <tuple>       // 'std::tuple'
#include <cstddef>     // 'std::size_t'

namespace BloombergLP {
namespace bslh {

                        // =====================
                        // struct hashTuple_Util
                        // =====================

template <std::size_t BACK_INDEX, class HASH_ALGORITHM, class TUPLE_TYPE>
struct hashTuple_Util
{
    // CLASS METHODS
    static void visit(HASH_ALGORITHM& algorithm, const TUPLE_TYPE& input);
        // Invoke the (appropriate) 'hashAppend' function using the specified
        // 'algorithm' and the "current" element of the specified 'input' tuple
        // and then visit the next element.  The index of the current element
        // is the size of (template parameter) 'TUPLE_TYPE' minus (template
        // parameter) 'BACK_INDEX'.  'BACK_INDEX' is reduced on each recursion
        // and eventually, when 'BACK_INDEX' is 0, a "do nothing" function is
        // called to stop the recursion.
};

            // The partial specialization that stops the recursion.

template <class HASH_ALGORITHM, class TUPLE_TYPE>
struct hashTuple_Util<0, HASH_ALGORITHM, TUPLE_TYPE>
{
    // CLASS METHODS
    static void visit(HASH_ALGORITHM& algorithm, const TUPLE_TYPE& input);
        // Do nothing -- ignore the specified 'algorithm', ignore the specified
        // 'input', do not try to visit the "next" element -- and return.  Note
        // that this function is called only after the last element of 'input'
        // has been visited.
};

// FREE FUNCTIONS
template <class HASH_ALGORITHM, class ... TYPE>
void hashAppend(HASH_ALGORITHM& algorithm, const std::tuple<TYPE ...> &input);
    // Invoke 'hashAppend', with the specified 'algorithm', on each element of
    // the specified 'input' in order of increasing index as defined by
    // 'std::get'.

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                        // ---------------------
                        // struct hashTuple_Util
                        // ---------------------

template <std::size_t BACK_INDEX, class HASH_ALGORITHM, class TUPLE_TYPE>
inline
void hashTuple_Util<BACK_INDEX, HASH_ALGORITHM, TUPLE_TYPE>::visit(
                                                   HASH_ALGORITHM&   algorithm,
                                                   const TUPLE_TYPE& input)
{
    // Hash current element.

    const std::size_t tupleSize = std::tuple_size<TUPLE_TYPE>::value;
    const std::size_t index      = tupleSize - BACK_INDEX;

    hashAppend(algorithm, std::get<index>(input));

    // Visit next element.

    const std::size_t nextBackIndex = BACK_INDEX - 1;

    hashTuple_Util<nextBackIndex, HASH_ALGORITHM, TUPLE_TYPE>::visit(algorithm,
                                                                     input);
}

template <class HASH_ALGORITHM, class TUPLE_TYPE>
inline
void hashTuple_Util<0, HASH_ALGORITHM, TUPLE_TYPE>::visit(HASH_ALGORITHM&   ,
                                                          const TUPLE_TYPE& )
{
}

// FREE FUNCTIONS
template <class HASH_ALGORITHM, class ... TYPE>
inline
void
hashAppend(HASH_ALGORITHM& algorithm, const std::tuple<TYPE ...> &input)
{
    using TupleType = typename std::remove_reference<decltype(input)>::type;

    const std::size_t tupleSize = std::tuple_size<TupleType>::value;

    if (0 == tupleSize) {
        return;                                                       // RETURN
    }

    hashTuple_Util<tupleSize, HASH_ALGORITHM, TupleType>::visit(algorithm,
                                                                input);
}

}  // close package namespace
}  // close enterprise namespace

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
#endif

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
