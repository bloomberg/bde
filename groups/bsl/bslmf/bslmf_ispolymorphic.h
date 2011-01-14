// bslmf_ispolymorphic.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMF_ISPOLYMORPHIC
#define INCLUDED_BSLMF_ISPOLYMORPHIC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for polymorphic types.
//
//@CLASSES:
//    bslmf_IsPolymorphic: meta-function for detecting polymorphic types
//
//@AUTHOR: Clay Wilson (cwilson9)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether its single type parameter is of polymorphic type.
// A class is polymorphic if it has virtual functions.  Note that the
// destructor of such a class should *always* be declared 'virtual'.  Therefore
// another definition of polymorphic is whether a class has a virtual
// destructor.
//
// 'bslmf_IsPolymorphic' defines a 'VALUE' member that is initialized
// (at compile-time) to 1 if the parameter is a polymorphic type (or a
// reference to such a type), and to 0 otherwise.
//
// Note that if 'bslmf_IsPolymorphic' is applied to a 'union' type, then the
// compilation will fail.  To date, it is not possible to detect 'union' types
// in C++.
//
///Usage
///-----
// For example:
//..
//   struct MyStruct {
//       void nonvirtualMethod();
//   };
//   struct MyDerivedStruct : public MyStruct {};
//..
// defines a non-polymorphic hierarchy, while:
//..
//   class MyClass {
//       MyClass();
//       virtual ~MyClass();  // makes 'MyClass' polymorphic
//   };
//
//   class MyDerivedClass : public MyClass {
//       MyDerivedClass();
//       ~MyDerivedClass();
//   };
//..
// defines a polymorphic hierarchy.  With these definitions:
//..
//   assert(0 == bslmf_IsPolymorphic<MyStruct          >::VALUE);
//   assert(0 == bslmf_IsPolymorphic<MyStruct         *>::VALUE);
//   assert(0 == bslmf_IsPolymorphic<MyDerivedStruct&  >::VALUE);
//   assert(0 == bslmf_IsPolymorphic<MyDerivedStruct  *>::VALUE);
//
//   assert(1 == bslmf_IsPolymorphic<      MyClass    >::VALUE);
//   assert(1 == bslmf_IsPolymorphic<const MyClass&   >::VALUE);
//   assert(0 == bslmf_IsPolymorphic<      MyClass   *>::VALUE);
//   assert(1 == bslmf_IsPolymorphic<MyDerivedClass&  >::VALUE);
//   assert(0 == bslmf_IsPolymorphic<MyDerivedClass  *>::VALUE);
//..
//
// The following class is detected as polymorphic by this component, but should
// really have a virtual destructor.  Note that 'gcc' issues a warning for such
// infractions.
//..
//   class MyIncorrectPolymorphicClass {
//
//       MyIncorrectPolymorphicClass();
//       ~MyIncorrectPolymorphicClass();
//       virtual void virtualMethod();
//   };
//
//   assert(1 == bslmf_IsPolymorphic<MyIncorrectPolymorphicClass&  >::VALUE);
//   assert(0 == bslmf_IsPolymorphic<MyIncorrectPolymorphicClass  *>::VALUE);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISCLASS
#include <bslmf_isclass.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#include <bslmf_removereference.h>
#endif

namespace BloombergLP {

                         // ==========================
                         // struct bslmf_IsPolymorphic
                         // ==========================

template <typename TYPE>
struct bslmf_IsPolymorphic_NonClassImp {
    enum { VALUE = 0 };
};

template <typename TYPE>
struct bslmf_IsPolymorphic_ClassImp {
    typedef typename bslmf_RemoveReference<TYPE>::Type NONREF_TYPE;
    typedef typename bslmf_RemoveCvq<NONREF_TYPE>::Type NONCV_TYPE;

    struct IsPoly : public NONCV_TYPE {
        IsPoly();
#ifdef BDE_BUILD_TARGET_EXC
        virtual ~IsPoly() throw();
#else
        virtual ~IsPoly();
#endif
        char dummy[256];
    };
    struct MaybePoly : public NONCV_TYPE {
        MaybePoly();
#ifdef BDE_BUILD_TARGET_EXC
        ~MaybePoly() throw();
#else
        ~MaybePoly();
#endif
        char dummy[256];
    };
    enum { VALUE = sizeof(IsPoly) == sizeof(MaybePoly) };
};

template <int IS_CLASS>
struct bslmf_IsPolymorphic_Selector {
    template <typename NON_CLASS_TYPE>
    struct rebind {
        typedef bslmf_IsPolymorphic_NonClassImp<NON_CLASS_TYPE> Type;
    };
};

template <>
struct bslmf_IsPolymorphic_Selector<1> {
    template <typename CLASS_TYPE>
    struct rebind {
        typedef bslmf_IsPolymorphic_ClassImp<CLASS_TYPE> Type;
    };
};

template <typename TYPE>
struct bslmf_IsPolymorphic {
    typedef bslmf_IsPolymorphic_Selector<
                                      bslmf_IsClass<TYPE>::VALUE> SelectorType;
    typedef typename SelectorType::template rebind<TYPE> BinderType;
    typedef typename BinderType::Type ImpType;
    enum { VALUE = ImpType::VALUE };
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
