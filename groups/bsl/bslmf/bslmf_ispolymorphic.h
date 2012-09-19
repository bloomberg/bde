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
//  bslmf::IsPolymorphic: meta-function for detecting polymorphic types
//
//@AUTHOR: Clay Wilson (cwilson9)
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether its single type parameter is of polymorphic type.  A class
// is polymorphic if it has virtual functions.  Note that the destructor of
// such a class should *always* be declared 'virtual'.  Therefore another
// definition of polymorphic is whether a class has a virtual destructor.
//
// 'bslmf::IsPolymorphic' defines a 'VALUE' member that is initialized (at
// compile-time) to 1 if the parameter is a polymorphic type (or a reference to
// such a type), and to 0 otherwise.
//
// Note that if 'bslmf::IsPolymorphic' is applied to a 'union' type, then the
// compilation will fail, unless the complier provides an intrinsic operation
// to detect this specific trait.  The compilers known to support this trait
// natively are:
//: o gcc 4.3 or later
//: o Visual C++ 2008 or later
//
// Further note that for some compilers this trait will yield a false positive,
// claiming that a non-polymorphic type *is* polymorphic, if using virtual
// inheritance.  This case is known to be handled *correctly* only for:
//: o Compilers with intrinsic support, listed above
//: o IBM XLC
//
///Usage
///-----
// For example:
//..
//  struct MyStruct {
//      void nonvirtualMethod();
//  };
//  struct MyDerivedStruct : public MyStruct {};
//..
// defines a non-polymorphic hierarchy, while:
//..
//  class MyClass {
//      MyClass();
//      virtual ~MyClass();  // makes 'MyClass' polymorphic
//  };
//
//  class MyDerivedClass : public MyClass {
//      MyDerivedClass();
//      ~MyDerivedClass();
//  };
//..
// defines a polymorphic hierarchy.  With these definitions:
//..
//  assert(0 == bslmf::IsPolymorphic<MyStruct          >::VALUE);
//  assert(0 == bslmf::IsPolymorphic<MyStruct         *>::VALUE);
//  assert(0 == bslmf::IsPolymorphic<MyDerivedStruct&  >::VALUE);
//  assert(0 == bslmf::IsPolymorphic<MyDerivedStruct  *>::VALUE);
//
//  assert(1 == bslmf::IsPolymorphic<      MyClass    >::VALUE);
//  assert(1 == bslmf::IsPolymorphic<const MyClass&   >::VALUE);
//  assert(0 == bslmf::IsPolymorphic<      MyClass   *>::VALUE);
//  assert(1 == bslmf::IsPolymorphic<MyDerivedClass&  >::VALUE);
//  assert(0 == bslmf::IsPolymorphic<MyDerivedClass  *>::VALUE);
//..
//
// The following class is detected as polymorphic by this component, but should
// really have a virtual destructor.  Note that 'gcc' issues a warning for such
// infractions.
//..
//  class MyIncorrectPolymorphicClass {
//
//      MyIncorrectPolymorphicClass();
//      ~MyIncorrectPolymorphicClass();
//      virtual void virtualMethod();
//  };
//
//  assert(1 == bslmf::IsPolymorphic<MyIncorrectPolymorphicClass&  >::VALUE);
//  assert(0 == bslmf::IsPolymorphic<MyIncorrectPolymorphicClass  *>::VALUE);
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

#if (defined(BSLS_PLATFORM__CMP_GNU) && BSLS_PLATFORM__CMP_VER_MAJOR >= 40300)\
 || (defined(BSLS_PLATFORM__CMP_MSVC) && BSLS_PLATFORM__CMP_VER_MAJOR >= 1500)
#define BSLMF_ISPOLYMORPHIC_HAS_INTRINSIC
#endif

namespace BloombergLP {

namespace bslmf {

                       // ========================
                       // struct IsPolymorphic_Imp
                       // ========================

#if defined(BSLMF_ISPOLYMORPHIC_HAS_INTRINSIC)
// Use type traits intrinsics, where avaialble, to give the correct answer for
// the tricky cases where existing ABIs prevent programatic detection.
template <typename TYPE>
struct IsPolymorphic_Imp : MetaInt<__is_polymorphic(TYPE)>::Type {
};
#else
template <typename TYPE, int IS_CLASS = IsClass<TYPE>::VALUE>
struct IsPolymorphic_Imp {
    typedef MetaInt<0> Type;
};

template <typename TYPE>
struct IsPolymorphic_Imp<TYPE, 1> {
    typedef typename RemoveCvq<TYPE>::Type NONCV_TYPE;

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

    typedef MetaInt<sizeof(IsPoly) == sizeof(MaybePoly)> Type;
};
#endif

                         // ====================
                         // struct IsPolymorphic
                         // ====================

template <typename TYPE>
struct IsPolymorphic : IsPolymorphic_Imp<TYPE>::Type {
    // This metafunction class derives from 'MetaInt<1>' if the specified
    // 'TYPE' is a class type (or a reference to a class type) with a v-table,
    // or from 'MetaInt<0>' otherwise.
};

template <typename TYPE>
struct IsPolymorphic<TYPE&> : IsPolymorphic_Imp<TYPE>::Type {
    // This metafunction class derives from 'MetaInt<1>' if the specified
    // 'TYPE' is a class type (or a reference to a class type) with a v-table,
    // or from 'MetaInt<0>' otherwise.
};

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_IsPolymorphic
#undef bslmf_IsPolymorphic
#endif
#define bslmf_IsPolymorphic bslmf::IsPolymorphic
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

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
