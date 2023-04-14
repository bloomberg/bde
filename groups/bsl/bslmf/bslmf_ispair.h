// bslmf_ispair.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLMF_ISPAIR
#define INCLUDED_BSLMF_ISPAIR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for the bsl::pair type.
//
//@CLASSES:
//  bslmf::IsPair: meta-function for determining the bsl::pair type.
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's parameter is a 'bsl::pair' type.  A 't_TYPE' that has
// this trait fulfills the following requirements, where 'mX' is a modifiable
// object and 'X' a non-modifiable object of 't_TYPE':
//..
//  Valid expression     Type
//  ----------------     ----
//  t_TYPE::first_type
//  t_TYPE::second_type
//
//  mX.first             first_type
//  mX.second            second_type
//  X.first              const first_type
//  X.second             const second_type
//..
// Note that 'first' and 'second' are *not* member functions, but data members.
//
///Usage notes
///-----------
// The 'bslmf::IsPair' trait is not used to construct pair objects, as a pair
// behaves like any other type w.r.t. the constructors and 'bslma::Allocator'.
// Nevertheless, it is used for different purposes in libraries that depend on
// 'bslalg'.  For instance, some libraries use it for printing (a pair does not
// have a printing method, but it is possible to use the trait to forward the
// call to 'print' to its two members if they both have the printable trait).

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>

namespace BloombergLP {

namespace bslmf {

template <class t_TYPE>
struct IsPair : bsl::false_type {};

}  // close package namespace
}  // close enterprise namespace

#endif

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
