// bslmf_isconvertible.cpp                                            -*-C++-*-
#include <bslmf_isconvertible.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

///IMPLEMENTATION NOTES
///--------------------
// The 'IsConvertible_Imp' meta-function is fairly simple at its core.  All
// that is needed is to have a function that can be called with 'TO_TYPE' and
// test if that function is callable with 'FROM_TYPE'.  However, there are two
// implementation complications, supporting incomplete type convertibility and
// suppressing compiler warnings.
//
///Supporting Incomplete Type Convertibility
///- - - - - - - - - - - - - - - - - - - - -
// 'IsConvertible_Imp' was implemented before 'bsl::is_convertible' was
// standardized.  'IsConvertible_Imp' was required to support incomplete type
// convertibility despite that the C++11 standard [meta.rel] disallows passing
// incomplete template parameter types to 'bsl::is_convertible'.
//
// It should be possible to determine that some conversions are valid even if
// the (template parameter) 'FROM_TYPE' is an incomplete type.  For example,
// given an incomplete type, 'X', the following can be assumed:
//..
//  'X' is convertible to 'X'.
//  'X' is convertible to 'const X'.
//  'const X' is convertible to 'X'.
//  cv-qualified 'X&' is convertible to cv-qualified 'X'.
//..
// It is not acceptable for 'IsConvertible_Imp' to fail compilation when it is
// instantiated on incomplete types, so we make the additional assumption that
// nothing else is convertible to or from 'X', even though additional
// information may prove that to be incorrect.
//
// In order to accept incomplete types, we must use 'const TO_TYPE&' instead of
// 'TO_TYPE' in our test function parameter.  This causes a separate problem:
// if 'FROM_TYPE' is 'volatile', then 'const TO_TYPE&' will cause incorrect
// matches.  Thus we suppress the addition of the 'const' reference if
// 'FROM_TYPE' is 'volatile', at the expense that compilation will fail in the
// very unlikely event that 'FROM_TYPE' is 'volatile' and 'TO_TYPE' is an
// incomplete type.  We must also suppress the addition of 'const' if 'TO_TYPE'
// is already a reference.
//
// Finally, we would like this meta-function to work even if 'FROM_TYPE' and/or
// 'TO_TYPE' are 'void', so we must add specializations for these cases.
//
// In the end, we end up with template for the general case, plus 11 partial or
// full specializations corresponding to the cross-product of the following
// parameter patterns:
//..
//  'FROM_TYPE' is a 'volatile' type.
//  'FROM_TYPE' is a 'volatile' reference type.
//  'FROM_TYPE' is a 'void' type.
//  'FROM_TYPE' is some other type.
//
//  'TO_TYPE' is a 'volatile' reference type.
//  'TO_TYPE' is a 'void' type.
//  'TO_TYPE' is some other type.
//..
//
///Suppressing Compiler Warnings
///- - - - - - - - - - - - - - -
// The GCC compiler generates copious warnings for floating-point to integral
// conversions, and so we add convertibility from any non-void fundamental type
// to any other non-void fundamental type as a special case.  Note that this
// does *not* suppress warnings when a floating-point to integral conversion is
// part of a conversion sequence, but it does take care of the most common
// case.  A full solution to this warning problem is likely to be much more
// involved.
//
// We use the following macros to define partial specializations of
// 'IsConvertible_Imp' when 'TO_TYPE' is a fundamental type (see
// bslmf_isconvertible.h):
//..
//  BSLMF_ISCONVERTIBLE_SAMETYPEVALUE
//  BSLMF_ISCONVERTIBLE_VALUE
//  BSLMF_ISCONVERTIBLE_FORWARD
//..
// The macros make the code clearer because of the large number of
// specializations: the sole purpose of these specializations is to suppress
// GCC warnings when converting from 'FLOAT' to 'INT', where 'INT' and 'FLOAT'
// are template parameter types representing an integral type and a
// floating-point type, respectively.
//
// Now, we will use the previously defined macros to define various partial
// specializations that we know never produce a warning.  There are two cases
// we must guard against:
//
// (1) When 'TO_TYPE' is fundamental (but not void), and the conversion from
// 'FROM_TYPE' to 'TO_TYPE' would succeed.  There is a risk that 'FROM_TYPE' is
// a floating-point type, or convertible to a floating-point type, and
// 'TO_TYPE' is integral.  But then the conversion would also succeed by
// replacing 'TO_TYPE' with 'double', which produces no warning.  This is
// *almost* an equivalence (i.e., the conversion fails if it fails when
// replacing 'TO_TYPE' with 'double'), with the only exception being when both
// types are fundamental, and the conversion is from 'volatile FROM_TYPE' to
// 'const TO_TYPE&' where both 'FROM_TYPE' and 'TO_TYPE' are not cv-qualified.
// The latter conversion should always fail if 'FROM_TYPE' is the same as
// 'TO_TYPE', and always succeed if 'FROM_TYPE' is also a fundamental type
// (note that testing so would trigger the warning).
//
// (2) When 'TO_TYPE' is not fundamental: there is a risk that 'FROM_TYPE' is a
// floating-point type, and 'TO_TYPE' has an implicit constructor from an
// integral type.  But then the conversion would also succeed by replacing
// 'FROM_TYPE' with 'int', which produces no warning.  In that case, there is a
// strict equivalence between the two definitions, since the 'volatile' to
// 'const&' conversion is only a problem when both types are fundamental.
//
// The following six partial specializations will match if 'TO_TYPE' is exactly
// a constant reference type:
//..
//  BSLMF_ISCONVERTIBLE_SAMETYPEVALUE(0, const volatile TYPE,
//                                       const          TYPE&, 1, 1);
//  BSLMF_ISCONVERTIBLE_SAMETYPEVALUE(0,       volatile TYPE,
//                                       const          TYPE&, 1, 1);
//
//  BSLMF_ISCONVERTIBLE_VALUE(1, const volatile FROM_TYPE,
//                               const          TO_TYPE&, 1, 1);
//  BSLMF_ISCONVERTIBLE_VALUE(1,       volatile FROM_TYPE,
//                               const          TO_TYPE&, 1, 1);
//
//  BSLMF_ISCONVERTIBLE_VALUE(1, const FROM_TYPE, const TO_TYPE&, 1, 1);
//  BSLMF_ISCONVERTIBLE_VALUE(1,       FROM_TYPE, const TO_TYPE&, 1, 1);
//..
// The next eight partial specializations will match if 'TO_TYPE' is a
// 'volatile' reference (const or not).  In that case, we can just pass through
// to the usual implementation since it is never going to make a temporary,
// hence no implicit conversions generating warnings.
//
// There are eight of them to avoid triggering ambiguities in the template
// instantiation process, when we systematically define any specialization for
// 'FROM_TYPE' with the four cv-qualifiers combinations:
//..
//  BSLMF_ISCONVERTIBLE_FORWARD(const volatile FROM_TYPE,
//                              const volatile TO_TYPE&, 1, 1);
//  BSLMF_ISCONVERTIBLE_FORWARD(      volatile FROM_TYPE,
//                              const volatile TO_TYPE&, 1, 1);
//  BSLMF_ISCONVERTIBLE_FORWARD(const          FROM_TYPE,
//                              const volatile TO_TYPE&, 1, 1);
//  BSLMF_ISCONVERTIBLE_FORWARD(               FROM_TYPE,
//                              const volatile TO_TYPE&, 1, 1);
//
//  BSLMF_ISCONVERTIBLE_FORWARD(const volatile FROM_TYPE,
//                                    volatile TO_TYPE&, 1, 1);
//  BSLMF_ISCONVERTIBLE_FORWARD(      volatile FROM_TYPE,
//                                    volatile TO_TYPE&, 1, 1);
//  BSLMF_ISCONVERTIBLE_FORWARD(const          FROM_TYPE,
//                                    volatile TO_TYPE&, 1, 1);
//  BSLMF_ISCONVERTIBLE_FORWARD(               FROM_TYPE,
//                                    volatile TO_TYPE&, 1, 1);
//..
// The next four partial specializations will match if 'TO_TYPE' is a
// non-cv-qualified reference to a fundamental type, since all matches to
// 'const volatile TO_TYPE&', 'const TO_TYPE&', and 'volatile TO_TYPE&' have
// been described above:
//..
//  BSLMF_ISCONVERTIBLE_FORWARD(const volatile FROM_TYPE, TO_TYPE&, 1, 1);
//  BSLMF_ISCONVERTIBLE_FORWARD(      volatile FROM_TYPE, TO_TYPE&, 1, 1);
//  BSLMF_ISCONVERTIBLE_FORWARD(const          FROM_TYPE, TO_TYPE&, 1, 1);
//  BSLMF_ISCONVERTIBLE_FORWARD(               FROM_TYPE, TO_TYPE&, 1, 1);
//..
// The next two partial specializations will be picked up by the compiler when
// 'TO_TYPE' is a non-reference fundamental type.  They do *forward* with a
// slight twist: casting to a 'double' instead of an 'int' to avoid the warning
// in case the 'TO_TYPE' is an integral type and the 'FROM_TYPE' is a
// floating-point type:
//..
//  template <class FROM_TYPE, class TO_TYPE>
//  struct IsConvertible_Imp<const FROM_TYPE, TO_TYPE, 1, 1>
//      : IsConvertible_Imp<const FROM_TYPE, double, 0, 0>::type {
//  };
//  template <class FROM_TYPE, class TO_TYPE>
//  struct IsConvertible_Imp<FROM_TYPE, TO_TYPE, 1, 1>
//      : IsConvertible_Imp<FROM_TYPE, double, 0, 0>::type {
//  };
//..
// The next partial specialization will be picked up by the compiler when
// 'TO_TYPE' is a (non-void) fundamental type, but 'FROM_TYPE' is not.  It does
// *forward* with a slight twist: casting to a 'double' to avoid the warning in
// case that the 'TO_TYPE' is an integral type and the 'FROM_TYPE' is a
// floating-point type:
//..
//  template <class FROM_TYPE, class TO_TYPE>
//  struct IsConvertible_Imp<FROM_TYPE, TO_TYPE, 0, 1>
//      : IsConvertible_Imp<FROM_TYPE, double, 0, 0>::type {
//  };
//..
// This partial specialization will be picked up by the compiler when
// 'FROM_TYPE' is a (non-void) fundamental type, but 'TO_TYPE' is not.  It does
// *forward* with a slight twist: casting from an 'int' to avoid the warning in
// case that the 'TO_TYPE' is an integral type and the 'FROM_TYPE' is a
// floating-point type:
//..
//  template <class FROM_TYPE, class TO_TYPE>
//  struct IsConvertible_Imp<FROM_TYPE, TO_TYPE, 1, 0>
//      : IsConvertible_Imp<int, TO_TYPE, 0, 0>::type {
//  };
//..

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
