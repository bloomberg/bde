// bsls_alignmentutil.cpp                                             -*-C++-*-
#include <bsls_alignmentutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_asserttest.h>  // for testing only
#include <bsls_types.h>       // for testing only

namespace BloombergLP {
namespace bsls {
namespace {
template <int INTEGER>
struct AlignmentUtil_Assert;
    // This parameterized 'struct' is declared but not defined except for the
    // single specialization below.  It is used with the 'sizeof' operator to
    // verify the assumption that 'BSLS_MAX_ALIGNMENT' is a positive, integral
    // power of 2.  Note that 'bslmf_assert' cannot be used in 'bsls'.

template <>
struct AlignmentUtil_Assert<1> {
    // Applying 'sizeof' to this specialization will allow compilation to
    // succeed (i.e., the associated compile-time assert will succeed).

    enum { VALUE = 1 };
};

// Assert, at compile time, that 'BSLS_MAX_ALIGNMENT' is a positive, integral
// power of 2.

enum {

    assertion1 = sizeof(AlignmentUtil_Assert<
                                1 <= bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT>),

    assertion2 = sizeof(AlignmentUtil_Assert<
                           0 <= (bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT
                             & (bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1))>)

};

}  // close unnamed namespace
}  // close package namespace
}  // close enterprise namespace

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
