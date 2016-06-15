// bslma_constructionutil.cpp                                         -*-C++-*-
#include <bslma_constructionutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// IMPLEMENTATION NOTES: The 'bslmf::IsPair' trait is slated to
// disappear, as a pair is like any other type w.r.t. the constructors and
// 'bslma::Allocator'.  Nevertheless, it is used for different purposes in
// libraries that depend on 'bslalg'.  For instance, 'bdepu' uses it for
// printing (a pair does not have a printing method, but it is possible to use
// the trait to forward the call to 'print' to its two members if they both
// have the printable trait).  More importantly, it is used in 'mgustd_pair' to
// forward allocators if needed (this relies on an older version of 'std::pair'
// which did not support allocators).  'mgustd_Pair' should be redesigned, but
// for now we have to support it or else risk breaking the Robo build.  For
// these reasons, we have retained the trait and support it in the
// implementations of 'bslalg::ScalarPrimitives::construct' and
// 'copyConstruct'.
#endif // BDE_OMIT_INTERNAL_DEPRECATED

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
