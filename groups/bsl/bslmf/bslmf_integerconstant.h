// bslmf_integerconstant.h                  -*-C++-*-
#ifndef INCLUDED_BSLMF_INTEGERCONSTANT
#define INCLUDED_BSLMF_INTEGERCONSTANT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mapping from integral constants to unique types.
//
//@CLASSES:
// bsl::integer_constant<TYPE,VAL>: A compile-time type representing 'VAL'
// bsl::false_type: 'typedef' for 'integer_constant<bool, false>'
// bsl::true_type: 'typedef' for 'integer_constant<bool, true>'
// bslmf::integer_constant<TYPE,VAL>: An alias for 'bsl::integer_constant'
// bslmf::false_type: An alias for 'bsl::false_type'
// bslmf::true_type: An alias for 'bsl::true_type'
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component describes a simple class template,
// 'integer_constant', that is used to map an inteeger constant to a C++ type.
// 'integer_constant<TYPE, VAL>' generates a unique type for each distinct
// compile-time integral 'TYPE' and constant integer 'VAL' parameter.  That
// is, instantiations with different integer types and values form distinct
// types, so that 'integer_constant<int, 0>' is a different type from
// 'integer_constant<int, 1>', which is also distinct from
// 'integer_constant<unsigned, 1>', and so on.  This mapping of integer values
// to types allows for "overloading by value", i.e., multiple functions with
// the same name can be overloaded on the "value" of an 'integer_constant'
// argument, provided that the value is known at compile-time.
//
///Usage
///-----
// This section illustates intended usage of this component
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
// typedefs for 'integer_constant<bool, true>' and
// 'integer_constant<bool, false>', respectively.
//..
//    #include <bslmf_integerconstant.h>
//    
//    template <class T>
//    int doSomethingImp(T *t, bslmf::true_type)
//    {
//        // slow, generic implementation
//        // ...
//        (void) t;
//        return 11; 
//    }
//    
//    template <class T>
//    int doSomethingImp(T *t, bslmf::false_type)
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
//        return doSomethingImp(t, bslmf::integer_constant<bool, IsSlow>());
//    }
//..
// For some parameter types, the fast version of 'doSomethingImp' is not
// legal.  The power of this approach is that the compiler will not attempt
// semantic anlysis on the implementation that does not match the appropriate
// 'integer_constant' argument.
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
///Example 2: Base class for metafunctions
/// - - - - - - - - - - - - - - - - - - - 
// Hard-coding the value of an 'integer_constant' is not especially useful.
// Rather, 'integer_constant' is typically used as the base class for
// "metafunction" classes, classes that yield the value of compile-time
// properties, including properties that are associated with types, rather
// than with values.  For example, the following metafunction can be used at
// compile time to determine whether a type is a floating point type:
//..
//    template <class TYPE> struct IsFloatingPoint    : bslmf::false_type { };
//    template <> struct IsFloatingPoint<float>       : bslmf::true_type { };
//    template <> struct IsFloatingPoint<double>      : bslmf::true_type { };
//    template <> struct IsFloatingPoint<long double> : bslmf::true_type { };
//..
// The value 'IsFloatingPoint<int>::value' is false and
// 'IsFloatingPoint<double>::value' is true.  The 'integer_constant' base
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
//        return doSomethingImp(t, bslmf::integer_constant<bool, isSlow>());
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

namespace bsl {

                        // ===============================
                        // class template integer_constant
                        // ===============================

template <class TYPE, TYPE VAL>
struct integer_constant {
    // Generate a unique type for the given 'TYPE' and 'VAL'.  This 'struct'
    // is used for compile-time dispatch of overloaded functions and as the
    // base class for many metafunctions.

  public:
    // CREATORS
    //! integer_constant() = default;
    //! integer_constant(const integer_constant&) = default;
    //! integer_constant operator=(const integer_constant&) = default;
    //! ~integer_constant() = default;

    // PUBLIC TYPES
    typedef TYPE             value_type;
    typedef integer_constant type;

    // PUBLIC CLASS DATA
    static const TYPE value = VAL;

    // ACCESSORS
    operator value_type() const;
        // Return 'VAL'.
};

                        // ===============
                        // type false_type
                        // ===============

typedef integer_constant<bool, false> false_type;

                        // ===============
                        // type true_type
                        // ===============

typedef integer_constant<bool, true> true_type;

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

    // Import 'bsl' classes into 'BloombergLP::bslmf'
    using bsl::integer_constant;
    using bsl::true_type;
    using bsl::false_type;

}  // close namespace bslmf
}  // close namespace BloombergLP

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// STATIC MEMBER VARIABLES
template <class TYPE, TYPE VAL>
const TYPE bsl::integer_constant<TYPE, VAL>::value;

// ACCESSORS
template <class TYPE, TYPE VAL>
inline
bsl::integer_constant<TYPE, VAL>::operator TYPE() const
{
    return VAL;
}

#endif // ! defined(INCLUDED_BSLMF_INTEGERCONSTANT)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
