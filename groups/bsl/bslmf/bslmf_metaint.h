// bslmf_metaint.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLMF_METAINT
#define INCLUDED_BSLMF_METAINT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function to map integral constants to unique types.
//
//@DEPRECATED: Use 'bslstt_integer_constant' instead.
//
//@CLASSES:
//  bslmf::MetaInt: meta-function mapping integral constants to C++ types
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to map
// an integral constant to a C++ type.  'bslmf::MetaInt<int>' defines a
// different type for each distinct compile-time constant integral parameter.
// That is, instantiations with different integer values form distinct types,
// so that 'bslmf::MetaInt<0>' is a distinct type from 'bslmf::MetaInt<1>',
// which is also distinct from 'bslmf::MetaInt<2>', and so on.
//
///Usage
///-----
// This section illustates intended usage of this component
//
///Example 1: Compile-Time Function Dispatching
/// - - - - - - - - - - - - - - - - - - - - - - 
// The most common use of this structure is to perform static function
// dispatching based on a compile-time calculation.  Often the calculation is
// nothing more than a simple predicate, allowing us to select one of two
// functions.  The following function, 'doSomething', uses a fast
// implementation (e.g., 'memcpy') if the parameterized type allows for such
// operations, otherwise it will use a more generic and slower implementation
// (e.g., copy constructor).
//..
//  template <class T>
//  void doSomethingImp(T *t, bslmf::MetaInt<0>)
//  {
//      // slow generic implementation
//      (void) t;
//      // ...
//  }
//
//  template <class T>
//  void doSomethingImp(T *t, bslmf::MetaInt<1>)
//  {
//      // fast implementation (works only for some T's)
//      (void) t;
//      // ...
//  }
//
//  template <class T, bool IsFast>
//  void doSomething(T *t)
//  {
//      doSomethingImp(t, bslmf::MetaInt<IsFast>());
//  }
//..
// The power of this approach is that the compiler will compile only the
// implementation selected by the 'MetaInt' argument.  For some parameter
// types, the fast version of 'doSomethingImp' would be ill-formed.  This kind
// of compile-time dispatch prevents the ill-formed version from ever being
// instantiated.
//..
//  int main()
//  {
//      int i;
//      doSomething<int, true>(&i); // fast version selected for int
//
//      double m;
//      doSomething<double, false>(&m); // slow version selected for double
//
//      return 0;
//  }
//..
///Example 2: Reading the 'VALUE' member
/// - - - - - - - - - - - - - - - - - - 
// In addition to forming new types, the value of the integral paramameter to
// 'MetaInt' is "saved" in the enum member 'VALUE', and is accessible for use
// in compile-time or run-time operations.
//..
//  template <int V>
//  unsigned g()
//  {
//      bslmf::MetaInt<V> i;
//      assert(V == i.VALUE);
//      assert(V == bslmf::MetaInt<V>::VALUE);
//      return bslmf::MetaInt<V>::VALUE;
//  }
//
//  int main()
//  {
//      int v = g<1>();
//      assert(1 == v);
//      return 0;
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_TAG
#include <bslmf_tag.h>
#endif

#ifndef INCLUDED_BSLTT_INTEGERCONSTANT
#include <bsltt_integerconstant.h>
#endif

namespace BloombergLP {

namespace bslmf {

                           // ==============
                           // struct MetaInt
                           // ==============

template <unsigned INT_VALUE>
struct MetaInt : public bsltt::integer_constant<unsigned, INT_VALUE> {
    // Instantiating this template produces a distinct type for each
    // non-negative integer value.

    // TYPES
    typedef MetaInt<INT_VALUE>    Type;
    typedef bslmf::Tag<INT_VALUE> Tag;

    enum { VALUE = int(INT_VALUE) };

    // CREATORS
    MetaInt();
        // Does nothing ('MetaInt' is stateless).

    template <class TYPE>
    MetaInt(bsltt::integer_constant<TYPE, INT_VALUE>);
        // Convert from a 'bsltt::integer_constant' with the same value.

    //! MetaInt(const MetaInt&) = default;
    //! MetaInt& operator=(const MetaInt&) = default;
    //! ~MetaInt() = default;

    // CLASS METHODS
    static Tag& tag();
        // Declared but not defined.  Meta-function use only.  The tag can be
        // used to recover meta-information from an expression.  Example:
        // 'sizeof(f(expr).tag())' returns a different compile-time value
        // depending on the type of the result of calling the 'f' function but
        // does not actually call the 'f' function or the 'tag' method at
        // run-time.  Note that 'f(expr)::VALUE' or 'sizeof(f(expr)::Type)'
        // would be ill-formed and that 'f(expr).value' is not a compile-time
        // expression.

    // ACCESSORS
    operator int () const;
        // Return 'VALUE'.  (This operator is conversion operator to 'int'.)

    template <class TYPE>
    operator bsltt::integer_constant<TYPE, INT_VALUE> () const;
        // Convert to an 'integer_constant' of the same value.
};

}  // close package namespace

#define BSLMF_METAINT_TO_INT(expr)  BSLMF_TAG_TO_INT((expr).tag())
    // Given an integral value, 'V', and an expression, 'expr', of type
    // 'bslmf::MetaInt<V>', this macro returns a compile-time constant with
    // value, 'V'.  The expression, 'expr', is not evaluated at run-time.

#define BSLMF_METAINT_TO_BOOL(expr) BSLMF_TAG_TO_BOOL((expr).tag())
    // Given an integral value, 'V', and an expression, 'expr', of type
    // 'bslmf::MetaInt<V>', this macro returns a compile-time constant with
    // value, 'true' or 'false', according to the Boolean value of 'V'.  The
    // expression, 'expr', is not evaluated at run-time.

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_MetaInt
#undef bslmf_MetaInt
#endif
#define bslmf_MetaInt bslmf::MetaInt
    // This alias is defined for backward compatibility.

// ===========================================================================
//                            INLINE FUNCTIONS
// ===========================================================================

// CREATORS
template <unsigned INT_VALUE>
inline
bslmf::MetaInt<INT_VALUE>::MetaInt()
{
}

template <unsigned INT_VALUE>
template <class TYPE>
inline
bslmf::MetaInt<INT_VALUE>::MetaInt(bsltt::integer_constant<TYPE, INT_VALUE>)
{
}

// ACCESSORS
template <unsigned INT_VALUE>
inline
bslmf::MetaInt<INT_VALUE>::operator int () const
{
    return VALUE;
}

template <unsigned INT_VALUE>
template <class TYPE>
inline
bslmf::MetaInt<INT_VALUE>::
    operator bsltt::integer_constant<TYPE, INT_VALUE> () const
{
    return bsltt::integer_constant<TYPE, (TYPE) INT_VALUE>();
}

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004-2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
