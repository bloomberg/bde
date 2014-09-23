// bsls_alignmenttotype.cpp                                           -*-C++-*-
#include <bsls_alignmenttotype.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

///IMPLEMENTATION NOTES
///--------------------
// The core of the implementation logic to compute the most primitive type
// corresponding to a given 'ALIGNMENT' value is provided by the
// 'bsls_alignmentimp' component.  Ideally, we would like to create a simple
// template class that is specialized for each alignment value and that
// provides the corresponding fundamental type.  But since multiple types can
// have the same alignment, we need a mechanism to disambiguate between them.
// So we assign a priority value to each type matching a specified alignment.
// This priority value is computed by the 'bsls::AlignmentImpMatch::match'
// function, which is specialized on a given type's alignment and size.  Once
// the priority is calculated we use the 'bsls::AlignmentImpPriorityToType'
// meta-function to define the most primitive type for a specified 'ALIGNMENT'
// value.

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
