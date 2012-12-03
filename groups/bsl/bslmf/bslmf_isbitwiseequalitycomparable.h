// bslmf_isbitwiseequalitycomparable.h                  -*-C++-*-
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

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTERTOMEMBER
#include <bslmf_ispointertomember.h>
#endif

#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#include <bslmf_detectnestedtrait.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif


namespace BloombergLP {

namespace bslmf {

template <typename TYPE>
struct IsBitwiseEqualityComparable;

template <typename TYPE>
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

template <typename TYPE>
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

}  // close namespace BloombergLP

#endif // ! defined(INCLUDED_BSLMF_ISBITWISECOPYABLE)

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
