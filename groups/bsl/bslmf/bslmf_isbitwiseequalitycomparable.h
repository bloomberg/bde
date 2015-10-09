// bslmf_isbitwiseequalitycomparable.h                                -*-C++-*-
#ifndef INCLUDED_BSLMF_ISBITWISEEQUALITYCOMPARABLE
#define INCLUDED_BSLMF_ISBITWISEEQUALITYCOMPARABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a type trait for bitwise equality.
//
//@CLASSES:
//  bslmf::IsBitwiseEqualityComparable: trait metafunction
//
//@DESCRIPTION:

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#include <bslmf_detectnestedtrait.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTERTOMEMBER
#include <bslmf_ispointertomember.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif


namespace BloombergLP {

namespace bslmf {

template <class TYPE>
struct IsBitwiseEqualityComparable;

template <class TYPE>
struct IsBitwiseEqualityComparable_Imp
    : bsl::integral_constant<bool,
                             !bsl::is_reference<TYPE>::value
                          && (  IsFundamental<TYPE>::value
                             || IsEnum<TYPE>::value
                             || bsl::is_pointer<TYPE>::value
                             || bslmf::IsPointerToMember<TYPE>::value
                             || DetectNestedTrait<TYPE,
                                          IsBitwiseEqualityComparable>::value)>
{};

                     // ==================================
                     // struct IsBitwiseEqualityComparable
                     // ==================================

template <class TYPE>
struct IsBitwiseEqualityComparable
    : IsBitwiseEqualityComparable_Imp<typename bsl::remove_cv<TYPE>::type>
{
    // Trait metafunction that determines whether the specified parameter
    // 'TYPE' is bit-wise EqualityComparable.  If
    // 'IsBitwiseEqualityComparable<TYPE>' is derived from 'true_type' then
    // 'TYPE' is bit-wise EqualityComparable.  Otherwise, bit-wise moveability
    // cannot be inferred for 'TYPE'.  This trait can be associated with a
    // bit-wise EqualityComparable user-defined class by specializing this
    // class or by using the 'BSLMF_DECLARE_NESTED_TRAIT' macro.
};

}  // close package namespace

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMF_ISBITWISECOPYABLE)

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
