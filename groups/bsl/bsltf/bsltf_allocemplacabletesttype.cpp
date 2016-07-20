// bsltf_allocemplacabletesttype.cpp                                  -*-C++-*-
#include <bsltf_allocemplacabletesttype.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmf_movableref.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace {
// INTERNAL DATA FOR THE WHOLE COMPONENT
static int s_numDeletes;
    // Track number of times the destructor is called.
}  // close unnamed namespace

namespace BloombergLP {
namespace bsltf {

typedef bslmf::MovableRefUtil MoveUtil;

AllocEmplacableTestType::AllocEmplacableTestType(
    bslma::Allocator *basicAllocator)
: d_a01(bslma::Default::allocator(basicAllocator))
, d_a02(bslma::Default::allocator(basicAllocator))
, d_a03(bslma::Default::allocator(basicAllocator))
, d_a04(bslma::Default::allocator(basicAllocator))
, d_a05(bslma::Default::allocator(basicAllocator))
, d_a06(bslma::Default::allocator(basicAllocator))
, d_a07(bslma::Default::allocator(basicAllocator))
, d_a08(bslma::Default::allocator(basicAllocator))
, d_a09(bslma::Default::allocator(basicAllocator))
, d_a10(bslma::Default::allocator(basicAllocator))
, d_a11(bslma::Default::allocator(basicAllocator))
, d_a12(bslma::Default::allocator(basicAllocator))
, d_a13(bslma::Default::allocator(basicAllocator))
, d_a14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         a01,
    bslma::Allocator *basicAllocator)
: d_a01(MoveUtil::move(a01), bslma::Default::allocator(basicAllocator))
, d_a02(bslma::Default::allocator(basicAllocator))
, d_a03(bslma::Default::allocator(basicAllocator))
, d_a04(bslma::Default::allocator(basicAllocator))
, d_a05(bslma::Default::allocator(basicAllocator))
, d_a06(bslma::Default::allocator(basicAllocator))
, d_a07(bslma::Default::allocator(basicAllocator))
, d_a08(bslma::Default::allocator(basicAllocator))
, d_a09(bslma::Default::allocator(basicAllocator))
, d_a10(bslma::Default::allocator(basicAllocator))
, d_a11(bslma::Default::allocator(basicAllocator))
, d_a12(bslma::Default::allocator(basicAllocator))
, d_a13(bslma::Default::allocator(basicAllocator))
, d_a14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         a01,
    ArgType02         a02,
    bslma::Allocator *basicAllocator)
: d_a01(MoveUtil::move(a01), bslma::Default::allocator(basicAllocator))
, d_a02(MoveUtil::move(a02), bslma::Default::allocator(basicAllocator))
, d_a03(bslma::Default::allocator(basicAllocator))
, d_a04(bslma::Default::allocator(basicAllocator))
, d_a05(bslma::Default::allocator(basicAllocator))
, d_a06(bslma::Default::allocator(basicAllocator))
, d_a07(bslma::Default::allocator(basicAllocator))
, d_a08(bslma::Default::allocator(basicAllocator))
, d_a09(bslma::Default::allocator(basicAllocator))
, d_a10(bslma::Default::allocator(basicAllocator))
, d_a11(bslma::Default::allocator(basicAllocator))
, d_a12(bslma::Default::allocator(basicAllocator))
, d_a13(bslma::Default::allocator(basicAllocator))
, d_a14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         a01,
    ArgType02         a02,
    ArgType03         a03,
    bslma::Allocator *basicAllocator)
: d_a01(MoveUtil::move(a01), bslma::Default::allocator(basicAllocator))
, d_a02(MoveUtil::move(a02), bslma::Default::allocator(basicAllocator))
, d_a03(MoveUtil::move(a03), bslma::Default::allocator(basicAllocator))
, d_a04(bslma::Default::allocator(basicAllocator))
, d_a05(bslma::Default::allocator(basicAllocator))
, d_a06(bslma::Default::allocator(basicAllocator))
, d_a07(bslma::Default::allocator(basicAllocator))
, d_a08(bslma::Default::allocator(basicAllocator))
, d_a09(bslma::Default::allocator(basicAllocator))
, d_a10(bslma::Default::allocator(basicAllocator))
, d_a11(bslma::Default::allocator(basicAllocator))
, d_a12(bslma::Default::allocator(basicAllocator))
, d_a13(bslma::Default::allocator(basicAllocator))
, d_a14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         a01,
    ArgType02         a02,
    ArgType03         a03,
    ArgType04         a04,
    bslma::Allocator *basicAllocator)
: d_a01(MoveUtil::move(a01), bslma::Default::allocator(basicAllocator))
, d_a02(MoveUtil::move(a02), bslma::Default::allocator(basicAllocator))
, d_a03(MoveUtil::move(a03), bslma::Default::allocator(basicAllocator))
, d_a04(MoveUtil::move(a04), bslma::Default::allocator(basicAllocator))
, d_a05(bslma::Default::allocator(basicAllocator))
, d_a06(bslma::Default::allocator(basicAllocator))
, d_a07(bslma::Default::allocator(basicAllocator))
, d_a08(bslma::Default::allocator(basicAllocator))
, d_a09(bslma::Default::allocator(basicAllocator))
, d_a10(bslma::Default::allocator(basicAllocator))
, d_a11(bslma::Default::allocator(basicAllocator))
, d_a12(bslma::Default::allocator(basicAllocator))
, d_a13(bslma::Default::allocator(basicAllocator))
, d_a14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         a01,
    ArgType02         a02,
    ArgType03         a03,
    ArgType04         a04,
    ArgType05         a05,
    bslma::Allocator *basicAllocator)
: d_a01(MoveUtil::move(a01), bslma::Default::allocator(basicAllocator))
, d_a02(MoveUtil::move(a02), bslma::Default::allocator(basicAllocator))
, d_a03(MoveUtil::move(a03), bslma::Default::allocator(basicAllocator))
, d_a04(MoveUtil::move(a04), bslma::Default::allocator(basicAllocator))
, d_a05(MoveUtil::move(a05), bslma::Default::allocator(basicAllocator))
, d_a06(bslma::Default::allocator(basicAllocator))
, d_a07(bslma::Default::allocator(basicAllocator))
, d_a08(bslma::Default::allocator(basicAllocator))
, d_a09(bslma::Default::allocator(basicAllocator))
, d_a10(bslma::Default::allocator(basicAllocator))
, d_a11(bslma::Default::allocator(basicAllocator))
, d_a12(bslma::Default::allocator(basicAllocator))
, d_a13(bslma::Default::allocator(basicAllocator))
, d_a14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         a01,
    ArgType02         a02,
    ArgType03         a03,
    ArgType04         a04,
    ArgType05         a05,
    ArgType06         a06,
    bslma::Allocator *basicAllocator)
: d_a01(MoveUtil::move(a01), bslma::Default::allocator(basicAllocator))
, d_a02(MoveUtil::move(a02), bslma::Default::allocator(basicAllocator))
, d_a03(MoveUtil::move(a03), bslma::Default::allocator(basicAllocator))
, d_a04(MoveUtil::move(a04), bslma::Default::allocator(basicAllocator))
, d_a05(MoveUtil::move(a05), bslma::Default::allocator(basicAllocator))
, d_a06(MoveUtil::move(a06), bslma::Default::allocator(basicAllocator))
, d_a07(bslma::Default::allocator(basicAllocator))
, d_a08(bslma::Default::allocator(basicAllocator))
, d_a09(bslma::Default::allocator(basicAllocator))
, d_a10(bslma::Default::allocator(basicAllocator))
, d_a11(bslma::Default::allocator(basicAllocator))
, d_a12(bslma::Default::allocator(basicAllocator))
, d_a13(bslma::Default::allocator(basicAllocator))
, d_a14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         a01,
    ArgType02         a02,
    ArgType03         a03,
    ArgType04         a04,
    ArgType05         a05,
    ArgType06         a06,
    ArgType07         a07,
    bslma::Allocator *basicAllocator)
: d_a01(MoveUtil::move(a01), bslma::Default::allocator(basicAllocator))
, d_a02(MoveUtil::move(a02), bslma::Default::allocator(basicAllocator))
, d_a03(MoveUtil::move(a03), bslma::Default::allocator(basicAllocator))
, d_a04(MoveUtil::move(a04), bslma::Default::allocator(basicAllocator))
, d_a05(MoveUtil::move(a05), bslma::Default::allocator(basicAllocator))
, d_a06(MoveUtil::move(a06), bslma::Default::allocator(basicAllocator))
, d_a07(MoveUtil::move(a07), bslma::Default::allocator(basicAllocator))
, d_a08(bslma::Default::allocator(basicAllocator))
, d_a09(bslma::Default::allocator(basicAllocator))
, d_a10(bslma::Default::allocator(basicAllocator))
, d_a11(bslma::Default::allocator(basicAllocator))
, d_a12(bslma::Default::allocator(basicAllocator))
, d_a13(bslma::Default::allocator(basicAllocator))
, d_a14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         a01,
    ArgType02         a02,
    ArgType03         a03,
    ArgType04         a04,
    ArgType05         a05,
    ArgType06         a06,
    ArgType07         a07,
    ArgType08         a08,
    bslma::Allocator *basicAllocator)
: d_a01(MoveUtil::move(a01), bslma::Default::allocator(basicAllocator))
, d_a02(MoveUtil::move(a02), bslma::Default::allocator(basicAllocator))
, d_a03(MoveUtil::move(a03), bslma::Default::allocator(basicAllocator))
, d_a04(MoveUtil::move(a04), bslma::Default::allocator(basicAllocator))
, d_a05(MoveUtil::move(a05), bslma::Default::allocator(basicAllocator))
, d_a06(MoveUtil::move(a06), bslma::Default::allocator(basicAllocator))
, d_a07(MoveUtil::move(a07), bslma::Default::allocator(basicAllocator))
, d_a08(MoveUtil::move(a08), bslma::Default::allocator(basicAllocator))
, d_a09(bslma::Default::allocator(basicAllocator))
, d_a10(bslma::Default::allocator(basicAllocator))
, d_a11(bslma::Default::allocator(basicAllocator))
, d_a12(bslma::Default::allocator(basicAllocator))
, d_a13(bslma::Default::allocator(basicAllocator))
, d_a14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         a01,
    ArgType02         a02,
    ArgType03         a03,
    ArgType04         a04,
    ArgType05         a05,
    ArgType06         a06,
    ArgType07         a07,
    ArgType08         a08,
    ArgType09         a09,
    bslma::Allocator *basicAllocator)
: d_a01(MoveUtil::move(a01), bslma::Default::allocator(basicAllocator))
, d_a02(MoveUtil::move(a02), bslma::Default::allocator(basicAllocator))
, d_a03(MoveUtil::move(a03), bslma::Default::allocator(basicAllocator))
, d_a04(MoveUtil::move(a04), bslma::Default::allocator(basicAllocator))
, d_a05(MoveUtil::move(a05), bslma::Default::allocator(basicAllocator))
, d_a06(MoveUtil::move(a06), bslma::Default::allocator(basicAllocator))
, d_a07(MoveUtil::move(a07), bslma::Default::allocator(basicAllocator))
, d_a08(MoveUtil::move(a08), bslma::Default::allocator(basicAllocator))
, d_a09(MoveUtil::move(a09), bslma::Default::allocator(basicAllocator))
, d_a10(bslma::Default::allocator(basicAllocator))
, d_a11(bslma::Default::allocator(basicAllocator))
, d_a12(bslma::Default::allocator(basicAllocator))
, d_a13(bslma::Default::allocator(basicAllocator))
, d_a14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         a01,
    ArgType02         a02,
    ArgType03         a03,
    ArgType04         a04,
    ArgType05         a05,
    ArgType06         a06,
    ArgType07         a07,
    ArgType08         a08,
    ArgType09         a09,
    ArgType10         a10,
    bslma::Allocator *basicAllocator)
: d_a01(MoveUtil::move(a01), bslma::Default::allocator(basicAllocator))
, d_a02(MoveUtil::move(a02), bslma::Default::allocator(basicAllocator))
, d_a03(MoveUtil::move(a03), bslma::Default::allocator(basicAllocator))
, d_a04(MoveUtil::move(a04), bslma::Default::allocator(basicAllocator))
, d_a05(MoveUtil::move(a05), bslma::Default::allocator(basicAllocator))
, d_a06(MoveUtil::move(a06), bslma::Default::allocator(basicAllocator))
, d_a07(MoveUtil::move(a07), bslma::Default::allocator(basicAllocator))
, d_a08(MoveUtil::move(a08), bslma::Default::allocator(basicAllocator))
, d_a09(MoveUtil::move(a09), bslma::Default::allocator(basicAllocator))
, d_a10(MoveUtil::move(a10), bslma::Default::allocator(basicAllocator))
, d_a11(bslma::Default::allocator(basicAllocator))
, d_a12(bslma::Default::allocator(basicAllocator))
, d_a13(bslma::Default::allocator(basicAllocator))
, d_a14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         a01,
    ArgType02         a02,
    ArgType03         a03,
    ArgType04         a04,
    ArgType05         a05,
    ArgType06         a06,
    ArgType07         a07,
    ArgType08         a08,
    ArgType09         a09,
    ArgType10         a10,
    ArgType11         a11,
    bslma::Allocator *basicAllocator)
: d_a01(MoveUtil::move(a01), bslma::Default::allocator(basicAllocator))
, d_a02(MoveUtil::move(a02), bslma::Default::allocator(basicAllocator))
, d_a03(MoveUtil::move(a03), bslma::Default::allocator(basicAllocator))
, d_a04(MoveUtil::move(a04), bslma::Default::allocator(basicAllocator))
, d_a05(MoveUtil::move(a05), bslma::Default::allocator(basicAllocator))
, d_a06(MoveUtil::move(a06), bslma::Default::allocator(basicAllocator))
, d_a07(MoveUtil::move(a07), bslma::Default::allocator(basicAllocator))
, d_a08(MoveUtil::move(a08), bslma::Default::allocator(basicAllocator))
, d_a09(MoveUtil::move(a09), bslma::Default::allocator(basicAllocator))
, d_a10(MoveUtil::move(a10), bslma::Default::allocator(basicAllocator))
, d_a11(MoveUtil::move(a11), bslma::Default::allocator(basicAllocator))
, d_a12(bslma::Default::allocator(basicAllocator))
, d_a13(bslma::Default::allocator(basicAllocator))
, d_a14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         a01,
    ArgType02         a02,
    ArgType03         a03,
    ArgType04         a04,
    ArgType05         a05,
    ArgType06         a06,
    ArgType07         a07,
    ArgType08         a08,
    ArgType09         a09,
    ArgType10         a10,
    ArgType11         a11,
    ArgType12         a12,
    bslma::Allocator *basicAllocator)
: d_a01(MoveUtil::move(a01), bslma::Default::allocator(basicAllocator))
, d_a02(MoveUtil::move(a02), bslma::Default::allocator(basicAllocator))
, d_a03(MoveUtil::move(a03), bslma::Default::allocator(basicAllocator))
, d_a04(MoveUtil::move(a04), bslma::Default::allocator(basicAllocator))
, d_a05(MoveUtil::move(a05), bslma::Default::allocator(basicAllocator))
, d_a06(MoveUtil::move(a06), bslma::Default::allocator(basicAllocator))
, d_a07(MoveUtil::move(a07), bslma::Default::allocator(basicAllocator))
, d_a08(MoveUtil::move(a08), bslma::Default::allocator(basicAllocator))
, d_a09(MoveUtil::move(a09), bslma::Default::allocator(basicAllocator))
, d_a10(MoveUtil::move(a10), bslma::Default::allocator(basicAllocator))
, d_a11(MoveUtil::move(a11), bslma::Default::allocator(basicAllocator))
, d_a12(MoveUtil::move(a12), bslma::Default::allocator(basicAllocator))
, d_a13(bslma::Default::allocator(basicAllocator))
, d_a14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         a01,
    ArgType02         a02,
    ArgType03         a03,
    ArgType04         a04,
    ArgType05         a05,
    ArgType06         a06,
    ArgType07         a07,
    ArgType08         a08,
    ArgType09         a09,
    ArgType10         a10,
    ArgType11         a11,
    ArgType12         a12,
    ArgType13         a13,
    bslma::Allocator *basicAllocator)
: d_a01(MoveUtil::move(a01), bslma::Default::allocator(basicAllocator))
, d_a02(MoveUtil::move(a02), bslma::Default::allocator(basicAllocator))
, d_a03(MoveUtil::move(a03), bslma::Default::allocator(basicAllocator))
, d_a04(MoveUtil::move(a04), bslma::Default::allocator(basicAllocator))
, d_a05(MoveUtil::move(a05), bslma::Default::allocator(basicAllocator))
, d_a06(MoveUtil::move(a06), bslma::Default::allocator(basicAllocator))
, d_a07(MoveUtil::move(a07), bslma::Default::allocator(basicAllocator))
, d_a08(MoveUtil::move(a08), bslma::Default::allocator(basicAllocator))
, d_a09(MoveUtil::move(a09), bslma::Default::allocator(basicAllocator))
, d_a10(MoveUtil::move(a10), bslma::Default::allocator(basicAllocator))
, d_a11(MoveUtil::move(a11), bslma::Default::allocator(basicAllocator))
, d_a12(MoveUtil::move(a12), bslma::Default::allocator(basicAllocator))
, d_a13(MoveUtil::move(a13), bslma::Default::allocator(basicAllocator))
, d_a14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         a01,
    ArgType02         a02,
    ArgType03         a03,
    ArgType04         a04,
    ArgType05         a05,
    ArgType06         a06,
    ArgType07         a07,
    ArgType08         a08,
    ArgType09         a09,
    ArgType10         a10,
    ArgType11         a11,
    ArgType12         a12,
    ArgType13         a13,
    ArgType14         a14,
    bslma::Allocator *basicAllocator)
: d_a01(MoveUtil::move(a01), bslma::Default::allocator(basicAllocator))
, d_a02(MoveUtil::move(a02), bslma::Default::allocator(basicAllocator))
, d_a03(MoveUtil::move(a03), bslma::Default::allocator(basicAllocator))
, d_a04(MoveUtil::move(a04), bslma::Default::allocator(basicAllocator))
, d_a05(MoveUtil::move(a05), bslma::Default::allocator(basicAllocator))
, d_a06(MoveUtil::move(a06), bslma::Default::allocator(basicAllocator))
, d_a07(MoveUtil::move(a07), bslma::Default::allocator(basicAllocator))
, d_a08(MoveUtil::move(a08), bslma::Default::allocator(basicAllocator))
, d_a09(MoveUtil::move(a09), bslma::Default::allocator(basicAllocator))
, d_a10(MoveUtil::move(a10), bslma::Default::allocator(basicAllocator))
, d_a11(MoveUtil::move(a11), bslma::Default::allocator(basicAllocator))
, d_a12(MoveUtil::move(a12), bslma::Default::allocator(basicAllocator))
, d_a13(MoveUtil::move(a13), bslma::Default::allocator(basicAllocator))
, d_a14(MoveUtil::move(a14), bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    const AllocEmplacableTestType&  original,
    bslma::Allocator               *basicAllocator)
: d_a01(original.d_a01, bslma::Default::allocator(basicAllocator))
, d_a02(original.d_a02, bslma::Default::allocator(basicAllocator))
, d_a03(original.d_a03, bslma::Default::allocator(basicAllocator))
, d_a04(original.d_a04, bslma::Default::allocator(basicAllocator))
, d_a05(original.d_a05, bslma::Default::allocator(basicAllocator))
, d_a06(original.d_a06, bslma::Default::allocator(basicAllocator))
, d_a07(original.d_a07, bslma::Default::allocator(basicAllocator))
, d_a08(original.d_a08, bslma::Default::allocator(basicAllocator))
, d_a09(original.d_a09, bslma::Default::allocator(basicAllocator))
, d_a10(original.d_a10, bslma::Default::allocator(basicAllocator))
, d_a11(original.d_a11, bslma::Default::allocator(basicAllocator))
, d_a12(original.d_a12, bslma::Default::allocator(basicAllocator))
, d_a13(original.d_a13, bslma::Default::allocator(basicAllocator))
, d_a14(original.d_a14, bslma::Default::allocator(basicAllocator))
{
}


AllocEmplacableTestType::~AllocEmplacableTestType()
{
    ++s_numDeletes;

    BSLS_ASSERT(d_a01.getAllocator() == d_a02.getAllocator());
    BSLS_ASSERT(d_a01.getAllocator() == d_a03.getAllocator());
    BSLS_ASSERT(d_a01.getAllocator() == d_a04.getAllocator());
    BSLS_ASSERT(d_a01.getAllocator() == d_a05.getAllocator());
    BSLS_ASSERT(d_a01.getAllocator() == d_a06.getAllocator());
    BSLS_ASSERT(d_a01.getAllocator() == d_a07.getAllocator());
    BSLS_ASSERT(d_a01.getAllocator() == d_a08.getAllocator());
    BSLS_ASSERT(d_a01.getAllocator() == d_a09.getAllocator());
    BSLS_ASSERT(d_a01.getAllocator() == d_a10.getAllocator());
    BSLS_ASSERT(d_a01.getAllocator() == d_a11.getAllocator());
    BSLS_ASSERT(d_a01.getAllocator() == d_a12.getAllocator());
    BSLS_ASSERT(d_a01.getAllocator() == d_a13.getAllocator());
    BSLS_ASSERT(d_a01.getAllocator() == d_a14.getAllocator());
}

int AllocEmplacableTestType::getNumDeletes()
{
    return s_numDeletes;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
