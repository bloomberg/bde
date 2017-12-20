// bdlf_memfn.cpp                                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlf_memfn.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlf_memfn_cpp,"$Id$ $CSID$")

///IMPLEMENTATION NOTES
///--------------------
// bdlf::MemFn: The 'operator()' methods of the 'bdlf::MemFn' wrapper need to
// be implemented within the class body for SUNPRO CC (even for version 5.5,
// aka Studio 8).  This is because that compiler accepts out-of-class-body
// definitions for template members, but not for template operators.
//
// bdlf::MemFnInstance: There are two ways to handle instance wrappers around
// class types that take a 'bslma' allocator.  One is to store the wrapper in a
// constructor proxy.  The other is to simply disallow it (asserting that
// 'INSTANCE_WRAPPER' does not have the 'bslma::UsesBslmaAllocator' trait).  We
// probably could do the latter because nothing we would usually store in a
// 'bdlf::MemFnInstance' would require it (in all known uses the instance
// wrapper is a raw pointer, or some kind of smart pointer, to the
// 'ObjectType').  For the sake of correctness and generality, we do the
// former.

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
