// bslstl_optional.cpp                                                -*-C++-*-

#include <bslstl_optional.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslstl_optional_cpp, "$Id$ $CSID$")

///Implementation Notes
///--------------------
// 'bsl::optional<T>' is implemented using structural inheritance from a
// component-private class, 'Optional_Base<T>', in order to have one
// implementation when 'T' is allocator-aware, and a different one when 'T' is
// not allocator-aware.  The non-allocator-aware implementation is further
// bifurcated into a C++17 version and a pre-C++17 version, which enables the
// C++17 version to be publicly derived from 'std::optional'.
//
// In order to prevent the allocator-extended constructors of
// 'bsl::optional<T>' from being viable when 'T' is not allocator-aware,
// 'bsl::optional<T>' inherits a protected typedef, 'AllocType', from
// 'Optional_Base<T>'.  When 'T' is allocator-aware, 'AllocType' is
// 'bsl::allocator<char>', but when 'T' is not allocator-aware, 'AllocType' is
// a class of which it is not possible to create objects.  All the
// allocator-extended constructors of 'bsl::optional<T>' are declared using
// 'AllocType', so they are dormant and harmless when 'T' is not
// allocator-aware.
//
///Constraints on Constructors and Assignment Operators
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// Most 'bsl::optional' constructors are constrained in some way, as specified
// by the C++ standard.  In order to avoid repeating those constraints on the
// declarations of the 'Optional_Base' constructors, we use a set of
// component-private tag types that allows each 'optional' constructor to
// explicitly specify which constructor overload should be called to initialize
// the 'Optional_Base' subobject.
//
// 'BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_FORWARD_REF(t_TYPE, t_ANY_TYPE)' has the
// same constraints as those specified by the standard, including the
// constraint that if 't_ANY_TYPE' is a scalar type then it must be different
// from 't_TYPE'.  This constraint is necessary because of cases like
//..
//  bsl::optional<int> oi;
//  oi = {};
//..
// The conversion from '{}' to 'int', yielding the value 0, is a standard
// conversion sequence, so it would ordinarily take precedence over the move
// assignment operator of 'optional'.  To prevent this, the converting
// assignment operator is disabled in this case.
//
///'bsl::nullopt_t'
///- - - - - - - -
// An issue that arises when implementing 'nullopt_t' (as we must do prior to
// C++17) is that copy-list-initialization will actually see explicit
// constructors, but the program is ill formed if an explicit constructor is
// actually chosen.  As a result, in a declaration of the form
//..
//  bsl::optional<T> o = {{...}};
//..
// the constructor 'optional(nullopt_t)' may compete in overload resolution if
// the inner list, '{...}', has an implicit conversion sequence to 'nullopt_t',
// notwithstanding the fact that the implicit conversion sequence would be ill
// formed if actually used (due to the constructor of 'nullopt_t' being
// explicit).  The fact that the 'optional(nullopt_t)' constructor competes in
// overload resolution is undesirable, as the user presumably intends for the
// contained 'T' object to be initialized from '{...}'.  In order to solve this
// problem, we give 'nullopt_t' a constructor template, so that any attempt to
// implicitly convert a braced-init-list to 'nullopt_t' will fail: the template
// parameter cannot be deduced.

namespace bsl {

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES)
const nullopt_t nullopt =
              nullopt_t(BloombergLP::bslstl::Optional_NulloptConstructToken());
#endif

}  // close namespace bsl

namespace BloombergLP {
namespace bslstl {

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
