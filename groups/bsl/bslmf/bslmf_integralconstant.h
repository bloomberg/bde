// bslmf_integralconstant.h                                           -*-C++-*-
#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#define INCLUDED_BSLMF_INTEGRALCONSTANT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mapping from integral constants to unique types.
//
//@CLASSES:
// bsl::integral_constant<TYPE,VAL>: A compile-time type representing 'VAL'
// bsl::false_type: 'typedef' for 'integral_constant<bool, false>'
// bsl::true_type: 'typedef' for 'integral_constant<bool, true>'
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component describes a simple class template,
// 'integral_constant', that is used to map an integer constant to a C++ type.
// 'integral_constant<TYPE, VAL>' generates a unique type for each distinct
// compile-time integral 'TYPE' and constant integer 'VAL' parameter.  That
// is, instantiations with different integer types and values form distinct
// types, so that 'integral_constant<int, 0>' is a different type from
// 'integral_constant<int, 1>', which is also distinct from
// 'integral_constant<unsigned, 1>', and so on.  This mapping of integer values
// to types allows for "overloading by value", i.e., multiple functions with
// the same name can be overloaded on the "value" of an 'integral_constant'
// argument, provided that the value is known at compile-time.
//
///Usage
///-----
// This section illustrates intended usage of this component
//
///Example 1: Compile-Time Function Dispatching
/// - - - - - - - - - - - - - - - - - - - - - -
// The most common use of this structure is to perform compile-time function
// dispatching based on a compile-time calculation.  Often the calculation is
// nothing more than a simple predicate, allowing us to select one of two
// functions based on whether the predicate holds.  The following function,
// 'doSomething', uses a fast implementation (e.g., using 'memcpy') if the
// parameterized type allows for such operations, otherwise it will use a more
// generic and slower implementation (e.g., using the copy constructor).  This
// example uses the types 'true_type' and 'false_type', which are simple
// typedefs for 'integral_constant<bool, true>' and
// 'integral_constant<bool, false>', respectively.
//..
//    #include <bslmf_integralconstant.h>
//
//    template <class T>
//    int doSomethingImp(T *t, bsl::true_type)
//    {
//        // slow, generic implementation
//        // ...
//        (void) t;
//        return 11;
//    }
//
//    template <class T>
//    int doSomethingImp(T *t, bsl::false_type)
//    {
//        // fast implementation that works only for some types of T
//        // ...
//        (void) t;
//        return 55;
//    }
//
//    template <bool IsSlow, class T>
//    int doSomething(T *t)
//    {
//        // Dispatch to an implementation depending on the (compile-time)
//        // value of 'IsSlow'.
//        return doSomethingImp(t, bsl::integral_constant<bool, IsSlow>());
//    }
//..
// For some parameter types, the fast version of 'doSomethingImp' is not
// legal.  The power of this approach is that the compiler will not attempt
// semantic analysis on the implementation that does not match the appropriate
// 'integral_constant' argument.
//..
//    int main()
//    {
//        int r;
//
//        int i;
//        r = doSomething<false>(&i);   // select fast version for int
//        assert(55 == r);
//
//        double m;
//        r = doSomething<true>(&m); // select slow version for double
//        assert(11 == r);
//
//        return 0;
//    }
//..
//
///Example 2: Base Class For Metafunctions
///- - - - - - - - - - - - - - - - - - - -
// Hard-coding the value of an 'integral_constant' is not especially useful.
// Rather, 'integral_constant' is typically used as the base class for
// "metafunction" classes, classes that yield the value of compile-time
// properties, including properties that are associated with types, rather
// than with values.  For example, the following metafunction can be used at
// compile time to determine whether a type is a floating point type:
//..
//    template <class TYPE> struct IsFloatingPoint    : bsl::false_type { };
//    template <> struct IsFloatingPoint<float>       : bsl::true_type { };
//    template <> struct IsFloatingPoint<double>      : bsl::true_type { };
//    template <> struct IsFloatingPoint<long double> : bsl::true_type { };
//..
// The value 'IsFloatingPoint<int>::value' is false and
// 'IsFloatingPoint<double>::value' is true.  The 'integral_constant' base
// class has a member type, 'type', that refers to itself and is inherited by
// 'IsFloatingPoint'. Thus 'IsFloatingPoint<float>::type' is 'true_type' and
// 'IsFloatingPoint<char>::type' is 'false_type'.  'IsFloatingPoint' is an a
// member of a common category of metafunctions known as "type traits" because
// they express certain properties (traits) of a type.  Using this
// metafunction, we can rewrite the 'doSomething' function from first example
// so that it does not require the user to specify the 'IsSlow' template
// argument:
//..
//    template <class T>
//    int doSomething2(T *t)
//    {
//        // Automatically detect whether to use slow or fast imp.
//        const bool isSlow = IsFloatingPoint<T>::value;
//        return doSomethingImp(t, bsl::integral_constant<bool, isSlow>());
//    }
//
//    int main()
//    {
//        int r;
//
//        int i;
//        r = doSomething2(&i); // select fast version for int
//        assert(55 == r);
//
//        double m;
//        r = doSomething2(&m); // select slow version for double
//        assert(11 == r);
//
//        return 0;
//    }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {
namespace bslmf {
template <int> struct MetaInt;
}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                        // ================================
                        // class template integral_constant
                        // ================================

template <class TYPE, TYPE VAL>
struct integral_constant {
    // Generate a unique type for the given 'TYPE' and 'VAL'.  This 'struct'
    // is used for compile-time dispatch of overloaded functions and as the
    // base class for many metafunctions.

  public:
    // CREATORS
    //! integral_constant() = default;
    //! integral_constant(const integral_constant&) = default;
    //! integral_constant operator=(const integral_constant&) = default;
    //! ~integral_constant() = default;

    // PUBLIC TYPES
    typedef TYPE              value_type;
    typedef integral_constant type;

    // PUBLIC CLASS DATA
    static const TYPE value = VAL;

    // ACCESSORS
    operator value_type() const;
        // Return 'VAL'.
};

template <bool VAL>
struct integral_constant<bool, VAL> {
  public:
    // CREATORS
    //! integral_constant() = default;
    //! integral_constant(const integral_constant&) = default;
    //! integral_constant operator=(const integral_constant&) = default;
    //! ~integral_constant() = default;

    // PUBLIC TYPES
    typedef bool              value_type;
    typedef integral_constant type;

    // PUBLIC CLASS DATA
    static const bool value = VAL;

    // ACCESSORS
    operator value_type() const;
        // Return 'VAL'.

    // COMPATIBILITY MEMBERS
    typedef BloombergLP::bslmf::MetaInt<VAL> Type;
    static const bool VALUE = VAL;
};

                        // ===============
                        // type false_type
                        // ===============

typedef integral_constant<bool, false> false_type;

                        // ===============
                        // type true_type
                        // ===============

typedef integral_constant<bool, true> true_type;

}  // close namespace bsl

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// STATIC MEMBER VARIABLES
template <class TYPE, TYPE VAL>
const TYPE bsl::integral_constant<TYPE, VAL>::value;

template <bool VAL>
const bool bsl::integral_constant<bool, VAL>::value;
template <bool VAL>
const bool bsl::integral_constant<bool, VAL>::VALUE;

// ACCESSORS
template <class TYPE, TYPE VAL>
inline
bsl::integral_constant<TYPE, VAL>::operator TYPE() const
{
    return VAL;
}

template <bool VAL>
inline
bsl::integral_constant<bool, VAL>::operator bool() const
{
    return VAL;
}

#endif // ! defined(INCLUDED_BSLMF_INTEGRALCONSTANT)

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
