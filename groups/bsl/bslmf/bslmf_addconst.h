// bslmf_addconst.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDCONST
#define INCLUDED_BSLMF_ADDCONST

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for adding a top-level 'const'-qualifier.
//
//@CLASSES:
//  bsl::add_const: meta-function for adding a top-level 'const'-qualifier
//
//@SEE_ALSO: bslmf_removeconst
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::add_const', that
// may be used to add a top-level 'const'-qualifier to a type if it is not a
// reference type, nor a function type, nor already 'const'-qualified at the
// top-level.
//
// 'bsl::add_const' meets the requirements of the 'add_const' template defined
// in the C++11 standard [meta.trans.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Adding a 'const'-qualifier to a Type
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to add a 'const'-qualifier to a particular type.
//
// First, we create two 'typedef's -- a 'const'-qualified type ('MyConstType')
// and the same type without the 'const'-qualifier ('MyType'):
//..
//  typedef int       MyType;
//  typedef const int MyConstType;
//..
// Now, we add a 'const'-qualifier to 'MyType' using 'bsl::add_const' and
// verify that the resulting type is the same as 'MyConstType':
//..
//  assert(true ==
//           (bsl::is_same<bsl::add_const<MyType>::type, MyConstType>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONST
#include <bslmf_isconst.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNCTION
#include <bslmf_isfunction.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

namespace BloombergLP {
namespace bslmf {

                         // ===================
                         // struct AddConst_Imp
                         // ===================

template <class TYPE, bool ADD_CONST_FLAG>
struct AddConst_Imp {
    // This 'struct' template provides an alias 'Type' that adds a
    // 'const'-qualifier to the (template parameter) 'TYPE' if the (template
    // parameter) 'ADD_CONST_FLAG' is 'true'.  This generic default template
    // adds the 'const'-qualifier to 'TYPE' in the 'Type' alias.  A template
    // specialization (below) leaves 'TYPE' as-is in its 'Type' alias.


    // PUBLIC TYPES
    typedef TYPE const Type;
        // This 'typedef' is an alias to a type that is the same as the
        // (template parameter) 'TYPE' except that a top-level
        // 'const'-qualifier has been added.
};

                         // ================================
                         // struct AddConst_Imp<TYPE, false>
                         // ================================

template <class TYPE>
struct AddConst_Imp<TYPE, false> {
    // This partial specialization of 'AddConst_Imp', for when the (template
    // parameter) 'ADD_CONST_FLAG' is 'false', provides an alias 'Type' that
    // has the same type as the (template parameter) 'TYPE'.

    // PUBLIC TYPES
    typedef TYPE Type;
        // This 'typedef' is an alias to the (template parameter) 'TYPE'.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                         // ================
                         // struct add_const
                         // ================

template <class TYPE>
struct add_const {
    // This 'struct' template implements the 'add_const' meta-function defined
    // in the C++11 standard [meta.trans.cv], providing an alias, 'type', that
    // returns the result.  If the (template parameter) 'TYPE' is not a
    // reference type, nor a function type, nor already 'const'-qualified at
    // the top-level, then 'type' is an alias to 'TYPE' with a top-level
    // 'const'-qualifier added; otherwise, 'type' is an alias to 'TYPE'.

    // PUBLIC TYPES
    typedef typename BloombergLP::bslmf::AddConst_Imp<
                            TYPE,
                            !is_reference<TYPE>::value
                            && !is_function<TYPE>::value
                            && !is_const<TYPE>::value>::Type type;
        // This 'typedef' is an alias to the (template parameter) 'TYPE' with a
        // top-level 'const'-qualifier added if 'TYPE' is not a reference, nor
        // a function, nor already 'const'-qualified; otherwise, 'type' is an
        // alias to 'TYPE'.
};

}  // close namespace bsl

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
