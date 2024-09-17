// bslmf_floatingtypestructuraltraits.cpp                             -*-C++-*-

#include <bslmf_floatingtypestructuraltraits.h>

#include <float.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmf_floatingtypestructuraltraits_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace bslmf {
                               // `float`

const int FloatingTypeStructuralTraits<float>::k_GENERAL_MAX_OUTPUT_LENGTH;
const int FloatingTypeStructuralTraits<float>::k_MAX_FIXED_PRECISION;
const int FloatingTypeStructuralTraits<float>::k_MAX_SCIENTIFIC_PRECISION;

const int32_t FloatingTypeStructuralTraits<float>::k_MANTISSA_HEX_DIGITS;
const int32_t FloatingTypeStructuralTraits<float>::k_MANTISSA_BITS;
const int32_t FloatingTypeStructuralTraits<float>::k_EXPONENT_BITS;
const int32_t FloatingTypeStructuralTraits<float>::k_MAX_BINARY_EXPONENT;
const int32_t FloatingTypeStructuralTraits<float>::k_MIN_BINARY_EXPONENT;
const int32_t FloatingTypeStructuralTraits<float>::k_EXPONENT_BIAS;
const int32_t FloatingTypeStructuralTraits<float>::k_SIGN_SHIFT;
const int32_t FloatingTypeStructuralTraits<float>::k_EXPONENT_SHIFT;

const uint32_t FloatingTypeStructuralTraits<float>::k_EXPONENT_MASK;
const uint32_t FloatingTypeStructuralTraits<float>::k_NORMAL_MANTISSA_MASK;
const uint32_t FloatingTypeStructuralTraits<float>::k_DENORM_MANTISSA_MASK;
const uint32_t FloatingTypeStructuralTraits<float>::k_SPEC_NAN_MANTISSA_MASK;
const uint32_t FloatingTypeStructuralTraits<float>::k_SHIFTED_SIGN_MASK;
const uint32_t FloatingTypeStructuralTraits<float>::k_SHIFTED_EXPONENT_MASK;

const float
FloatingTypeStructuralTraits<float>::k_MIN_VALUE = FLT_MIN; // 0x1.000000p-126f
const float
FloatingTypeStructuralTraits<float>::k_MAX_VALUE = FLT_MAX; // 0x1.FFFFFEp+127f


                                // `double`

const int FloatingTypeStructuralTraits<double>::k_GENERAL_MAX_OUTPUT_LENGTH;
const int FloatingTypeStructuralTraits<double>::k_MAX_FIXED_PRECISION;
const int FloatingTypeStructuralTraits<double>::k_MAX_SCIENTIFIC_PRECISION;

const int32_t FloatingTypeStructuralTraits<double>::k_MANTISSA_HEX_DIGITS;
const int32_t FloatingTypeStructuralTraits<double>::k_MANTISSA_BITS;
const int32_t FloatingTypeStructuralTraits<double>::k_EXPONENT_BITS;
const int32_t FloatingTypeStructuralTraits<double>::k_MAX_BINARY_EXPONENT;
const int32_t FloatingTypeStructuralTraits<double>::k_MIN_BINARY_EXPONENT;
const int32_t FloatingTypeStructuralTraits<double>::k_EXPONENT_BIAS;
const int32_t FloatingTypeStructuralTraits<double>::k_SIGN_SHIFT;
const int32_t FloatingTypeStructuralTraits<double>::k_EXPONENT_SHIFT;

const uint64_t FloatingTypeStructuralTraits<double>::k_EXPONENT_MASK;
const uint64_t FloatingTypeStructuralTraits<double>::k_NORMAL_MANTISSA_MASK;
const uint64_t FloatingTypeStructuralTraits<double>::k_DENORM_MANTISSA_MASK;
const uint64_t FloatingTypeStructuralTraits<double>::k_SPEC_NAN_MANTISSA_MASK;
const uint64_t FloatingTypeStructuralTraits<double>::k_SHIFTED_SIGN_MASK;
const uint64_t FloatingTypeStructuralTraits<double>::k_SHIFTED_EXPONENT_MASK;

const double                                         // 0x1.0000000000000p-1022
FloatingTypeStructuralTraits<double>::k_MIN_VALUE = DBL_MIN;
const double                                         // 0x1.FFFFFFFFFFFFFp+1023
FloatingTypeStructuralTraits<double>::k_MAX_VALUE = DBL_MAX;

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.
//
// Licensed under the Apache-2.0 License WITH LLVM-exception;
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception.  You may not use
// this file except in compliance with the License.
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
