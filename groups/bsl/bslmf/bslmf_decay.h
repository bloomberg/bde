// bslmf_decay.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLMF_DECAY
#define INCLUDED_BSLMF_DECAY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Convert a type to the type used for pass-by-value.
//
//@CLASSES:
//  bsl::decay: type trait computing return type for type parameter
//
//@SEE_ALSO: bslmf_removeextent
//
//@DESCRIPTION: This component provies a metafunction, 'bsl::decay', that
// applies array-to-pointer and function-to-pointer conversion and
// cv-qualification removal to a type, thus modeling the decay of an argument
// type when passed by-value into a function.  'bsl::decay' provides identical
// functionality to the C++11 standard metafunction 'std::decay'.  From the
// C++14, standard description of 'std::decay':
//
// Let 'U' be 'remove_reference_t<T>'.  If 'is_array<U>::value' is 'true', the
// member typedef 'type' shall equal 'remove_extent_t<U>*'.  If
// 'is_function<U>::value' is 'true', the member typedef 'type' shall equal
// 'add_pointer_t<U>'.  Otherwise the member typedef 'type' equals
// 'remove_cv_t<U>'.  [ *Note*: This behavior is similar to the
// lvalue-to-rvalue (4.1), array-to-pointer (4.2), and function-to-pointer
// (4.3) conversions applied when an lvalue expression is used as an rvalue,
// but also strips cv-qualifiers from class types in order to more closely
// model by-value argument passing.  - *end note* ]
//
///Usage
///-----
//
/// Usage Example 1
/// - - - - - - - -
// A class template needs to cache a value of type 'T'. There is nothing in the
// definition of the class that would prevent it from working for 'T' of
// function type or array-of-unknown bound, but one cannot simply declare a
// member of either of those types.  Instead, we use 'bsl::decay<T>::type',
// which can be stored, copied, and compared as needed:
//..
//  #ifndef INCLUDED_BSLMF_DECAY
//  #include <bslmf_decay.h>
//  #endif
//
//  template <class TYPE>
//  class Thing {
//  public:
//      typedef typename bsl::decay<TYPE>::type CacheType;
//
//  private:
//      CacheType d_cache;
//      // ...
//
//  public:
//      CacheType cache() const { return d_cache; }
//  };
//..
// Now verify that for function and array types, 'cache()' will return a simple
// pointer:
//..
//  int main()
//  {
//      typedef const int A1[];
//      typedef double A2[3][4];
//      typedef void F1(int);
//      typedef int (&F2)();
//
//      assert((bsl::is_same<const int*,    Thing<A1>::CacheType>::value));
//      assert((bsl::is_same<double(*)[4],  Thing<A2>::CacheType>::value));
//      assert((bsl::is_same<void (*)(int), Thing<F1>::CacheType>::value));
//      assert((bsl::is_same<int (*)(),     Thing<F2>::CacheType>::value));
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_ISARRAY
#include <bslmf_isarray.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNCTION
#include <bslmf_isfunction.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEEXTENT
#include <bslmf_removeextent.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#include <bslmf_removereference.h>
#endif

namespace bsl {

// Forward declaration
template <class TYPE, bool ISARRAY, bool ISFUNC> struct decay_imp;

                        // ====================
                        // class template decay
                        // ====================

template <class TYPE>
class decay
{
    // Metafunction to return the variant of the specified parameter 'TYPE'
    // used for pass-by-reference, e.g., by applying array-to-pointer and
    // function-to-pointer conversion.

    typedef typename bsl::remove_reference<TYPE>::type U;
    enum {
        k_ISARRAY = is_array<U>::value,
        k_ISFUNC  = is_function<U>::value
    };

  public:
    typedef typename decay_imp<U, k_ISARRAY, k_ISFUNC>::type type;
};

// ============================================================================
//                      CLASS TEMPLATE IMPLEMENTATION
// ============================================================================

template <class TYPE, bool IS_ARRAY, bool IS_FUNC>
struct decay_imp : remove_cv<TYPE>
{
    BSLMF_ASSERT( ! (IS_ARRAY || IS_FUNC));
};

template <class TYPE>
struct decay_imp<TYPE, true /* IS_ARRAY */, false /* IS_FUNC */>
{
    typedef typename remove_extent<TYPE>::type *type;
};

template <class TYPE>
struct decay_imp<TYPE, false /* IS_ARRAY */, true /* IS_FUNC */>
{
    typedef TYPE *type;
};

}  // close namespace bsl

#endif // ! defined(INCLUDED_BSLMF_DECAY)

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
