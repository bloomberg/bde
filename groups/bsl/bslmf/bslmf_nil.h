// bslmf_nil.h                                                        -*-C++-*-
#ifndef INCLUDED_BSLMF_NIL
#define INCLUDED_BSLMF_NIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a nil type.
//
//@CLASSES:
//  bslmf::Nil: class representing a nil (non-existent) type.
//  bslmf::IsNil: meta-function to test for nil.
//
//@AUTHOR: Oleg Grunin (ogrunin)
//
//@DESCRIPTION: 'bslmf::Nil' is mainly used for template meta-programming.  It
// is useful for providing defaults for template parameters and terminating
// template recursions.  'bslmf::Nil' can also be used to represent an unset
// state (i.e., nil) in components that require one.
//
///Usage
///-----
// The following usage illustrates a typical use of 'bslmf::Nil' as a default
// template parameter for another meta-function.
//..
//  template <int   CONDITION,
//            class TRUE_TYPE = bslmf::Nil,
//            class FALSE_TYPE = bslmf::Nil>
//  struct my_If {
//      typedef TRUE_TYPE Type;
//  };
//
//  template <class TRUE_TYPE, class FALSE_TYPE>
//  struct my_If<0, TRUE_TYPE, FALSE_TYPE> {
//      typedef FALSE_TYPE Type;
//  };
//..
// Next, we can use the 'my_If' meta-function to implement a print function
// that checks whether a type has a print method (using a fictitious
// meta-function 'my_IsPrintable'), and print the value.  Note the use of
// 'bslmf::Nil' as the type for overloading:
//..
//  template <class TYPE>
//  void print(const TYPE& value, int)
//  {
//      bsl::cout << value << bsl::endl;
//  }
//
//  template <class TYPE>
//  void print(const TYPE&, bslmf::Nil)
//  {
//      bsl::cout << "Cannot be printed." << bsl::endl;
//  }
//
//  template <class TYPE>
//  void print(const TYPE& value)
//  {
//      my_If<my_IsPrintable<TYPE>::value, int>::Type Type;  // default false
//                                                           // type is
//                                                           // 'bslmf::Nil'.
//      print(value, Type());
//  }
//..
// Using the above functions, the following code:
//..
//  int         x = 3;
//  bslmf::Nil nil;
//..
// Will print the following to 'stdout':
//..
//  3
//  Cannot be printed.
//..
// Finally, the 'bslmf::IsNil' meta-function returns true if the type passed to
// it is 'bslmf::Nil', and false otherwise:
//..
//  assert(1 == bslmf::IsNil<bslmf::Nil>::value)
//  assert(0 == bslmf::IsNil<int>::value)
//  assert(0 == bslmf::IsNil<char>::value)
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

namespace BloombergLP {

namespace bslmf {

                      // ==========
                      // struct Nil
                      // ==========

struct Nil {
    // This struct is empty and represents a nil type.
};

                      // ============
                      // struct IsNil
                      // ============

template <class T>
struct IsNil : bsl::false_type {
};

template <>
struct IsNil<Nil> : bsl::true_type {
};

}  // close package namespace

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_IsNil
#undef bslmf_IsNil
#endif
#define bslmf_IsNil bslmf::IsNil
    // This alias is defined for backward compatibility.

typedef bslmf::Nil bslmf_Nil;
    // This alias is defined for backward compatibility.

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
