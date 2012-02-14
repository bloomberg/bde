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

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#include <bslmf_removereference.h>
#endif

#endif

#ifdef BDE_BUILD_TARGET_EXC
#define BSLMF_ISPOLYMORPHIC_NOTHROW throw()
#else
#define BSLMF_ISPOLYMORPHIC_NOTHROW
#endif

namespace BloombergLP {

                       // ==============================
                       // struct bslmf_IsPolymorphic_Imp
                       // ==============================

template <typename TYPE, int IS_CLASS = bslmf_IsClass<TYPE>::VALUE>
struct bslmf_IsPolymorphic_Imp {
    typedef bslmf_MetaInt<0> Type;
};

template <typename TYPE>
struct bslmf_IsPolymorphic_Imp<TYPE, 1> {
    typedef typename bslmf_RemoveCvq<TYPE>::Type NONCV_TYPE;

    struct IsPoly : public NONCV_TYPE {
        IsPoly();
        virtual ~IsPoly() BSLMF_ISPOLYMORPHIC_NOTHROW;
        char dummy[256];
    };
    struct MaybePoly : public NONCV_TYPE {
        MaybePoly();
        ~MaybePoly() BSLMF_ISPOLYMORPHIC_NOTHROW;
        char dummy[256];
    };

    typedef bslmf_MetaInt<sizeof(IsPoly) == sizeof(MaybePoly)> Type;
};

                         // ==========================
                         // struct bslmf_IsPolymorphic
                         // ==========================

template <typename TYPE>
struct bslmf_IsPolymorphic : bslmf_IsPolymorphic_Imp<TYPE>::Type {
    // This metafunction class derives from 'bslmf_MetaInt<1>' if the specified
    // 'TYPE' is a class type (or a reference to a class type) with a v-table,
    // or from 'bslmf_MetaInt<0>' otherwise.
};

template <typename TYPE>
struct bslmf_IsPolymorphic<TYPE&> : bslmf_IsPolymorphic_Imp<TYPE>::Type {
    // This metafunction class derives from 'bslmf_MetaInt<1>' if the specified
    // 'TYPE' is a class type (or a reference to a class type) with a v-table,
    // or from 'bslmf_MetaInt<0>' otherwise.
};

}  // close namespace BloombergLP

#undef BSLMF_ISPOLYMORPHIC_NOTHROW

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
