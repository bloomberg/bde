// bsltf_allocemplacabletesttype.cpp                                  -*-C++-*-
#include <bsltf_allocemplacabletesttype.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmf_movableref.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {
namespace bsltf {

typedef bslmf::MovableRefUtil MoveUtil;

int AllocEmplacableTestType::s_numDeletes = 0;

AllocEmplacableTestType::AllocEmplacableTestType(
    bslma::Allocator *basicAllocator)
: d_arg01(bslma::Default::allocator(basicAllocator))
, d_arg02(bslma::Default::allocator(basicAllocator))
, d_arg03(bslma::Default::allocator(basicAllocator))
, d_arg04(bslma::Default::allocator(basicAllocator))
, d_arg05(bslma::Default::allocator(basicAllocator))
, d_arg06(bslma::Default::allocator(basicAllocator))
, d_arg07(bslma::Default::allocator(basicAllocator))
, d_arg08(bslma::Default::allocator(basicAllocator))
, d_arg09(bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         arg01,
    bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(bslma::Default::allocator(basicAllocator))
, d_arg03(bslma::Default::allocator(basicAllocator))
, d_arg04(bslma::Default::allocator(basicAllocator))
, d_arg05(bslma::Default::allocator(basicAllocator))
, d_arg06(bslma::Default::allocator(basicAllocator))
, d_arg07(bslma::Default::allocator(basicAllocator))
, d_arg08(bslma::Default::allocator(basicAllocator))
, d_arg09(bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         arg01,
    ArgType02         arg02,
    bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(bslma::Default::allocator(basicAllocator))
, d_arg04(bslma::Default::allocator(basicAllocator))
, d_arg05(bslma::Default::allocator(basicAllocator))
, d_arg06(bslma::Default::allocator(basicAllocator))
, d_arg07(bslma::Default::allocator(basicAllocator))
, d_arg08(bslma::Default::allocator(basicAllocator))
, d_arg09(bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         arg01,
    ArgType02         arg02,
    ArgType03         arg03,
    bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(bslma::Default::allocator(basicAllocator))
, d_arg05(bslma::Default::allocator(basicAllocator))
, d_arg06(bslma::Default::allocator(basicAllocator))
, d_arg07(bslma::Default::allocator(basicAllocator))
, d_arg08(bslma::Default::allocator(basicAllocator))
, d_arg09(bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         arg01,
    ArgType02         arg02,
    ArgType03         arg03,
    ArgType04         arg04,
    bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(bslma::Default::allocator(basicAllocator))
, d_arg06(bslma::Default::allocator(basicAllocator))
, d_arg07(bslma::Default::allocator(basicAllocator))
, d_arg08(bslma::Default::allocator(basicAllocator))
, d_arg09(bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         arg01,
    ArgType02         arg02,
    ArgType03         arg03,
    ArgType04         arg04,
    ArgType05         arg05,
    bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(bslma::Default::allocator(basicAllocator))
, d_arg07(bslma::Default::allocator(basicAllocator))
, d_arg08(bslma::Default::allocator(basicAllocator))
, d_arg09(bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         arg01,
    ArgType02         arg02,
    ArgType03         arg03,
    ArgType04         arg04,
    ArgType05         arg05,
    ArgType06         arg06,
    bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(MoveUtil::move(arg06), bslma::Default::allocator(basicAllocator))
, d_arg07(bslma::Default::allocator(basicAllocator))
, d_arg08(bslma::Default::allocator(basicAllocator))
, d_arg09(bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         arg01,
    ArgType02         arg02,
    ArgType03         arg03,
    ArgType04         arg04,
    ArgType05         arg05,
    ArgType06         arg06,
    ArgType07         arg07,
    bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(MoveUtil::move(arg06), bslma::Default::allocator(basicAllocator))
, d_arg07(MoveUtil::move(arg07), bslma::Default::allocator(basicAllocator))
, d_arg08(bslma::Default::allocator(basicAllocator))
, d_arg09(bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         arg01,
    ArgType02         arg02,
    ArgType03         arg03,
    ArgType04         arg04,
    ArgType05         arg05,
    ArgType06         arg06,
    ArgType07         arg07,
    ArgType08         arg08,
    bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(MoveUtil::move(arg06), bslma::Default::allocator(basicAllocator))
, d_arg07(MoveUtil::move(arg07), bslma::Default::allocator(basicAllocator))
, d_arg08(MoveUtil::move(arg08), bslma::Default::allocator(basicAllocator))
, d_arg09(bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         arg01,
    ArgType02         arg02,
    ArgType03         arg03,
    ArgType04         arg04,
    ArgType05         arg05,
    ArgType06         arg06,
    ArgType07         arg07,
    ArgType08         arg08,
    ArgType09         arg09,
    bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(MoveUtil::move(arg06), bslma::Default::allocator(basicAllocator))
, d_arg07(MoveUtil::move(arg07), bslma::Default::allocator(basicAllocator))
, d_arg08(MoveUtil::move(arg08), bslma::Default::allocator(basicAllocator))
, d_arg09(MoveUtil::move(arg09), bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         arg01,
    ArgType02         arg02,
    ArgType03         arg03,
    ArgType04         arg04,
    ArgType05         arg05,
    ArgType06         arg06,
    ArgType07         arg07,
    ArgType08         arg08,
    ArgType09         arg09,
    ArgType10         arg10,
    bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(MoveUtil::move(arg06), bslma::Default::allocator(basicAllocator))
, d_arg07(MoveUtil::move(arg07), bslma::Default::allocator(basicAllocator))
, d_arg08(MoveUtil::move(arg08), bslma::Default::allocator(basicAllocator))
, d_arg09(MoveUtil::move(arg09), bslma::Default::allocator(basicAllocator))
, d_arg10(MoveUtil::move(arg10), bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         arg01,
    ArgType02         arg02,
    ArgType03         arg03,
    ArgType04         arg04,
    ArgType05         arg05,
    ArgType06         arg06,
    ArgType07         arg07,
    ArgType08         arg08,
    ArgType09         arg09,
    ArgType10         arg10,
    ArgType11         arg11,
    bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(MoveUtil::move(arg06), bslma::Default::allocator(basicAllocator))
, d_arg07(MoveUtil::move(arg07), bslma::Default::allocator(basicAllocator))
, d_arg08(MoveUtil::move(arg08), bslma::Default::allocator(basicAllocator))
, d_arg09(MoveUtil::move(arg09), bslma::Default::allocator(basicAllocator))
, d_arg10(MoveUtil::move(arg10), bslma::Default::allocator(basicAllocator))
, d_arg11(MoveUtil::move(arg11), bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         arg01,
    ArgType02         arg02,
    ArgType03         arg03,
    ArgType04         arg04,
    ArgType05         arg05,
    ArgType06         arg06,
    ArgType07         arg07,
    ArgType08         arg08,
    ArgType09         arg09,
    ArgType10         arg10,
    ArgType11         arg11,
    ArgType12         arg12,
    bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(MoveUtil::move(arg06), bslma::Default::allocator(basicAllocator))
, d_arg07(MoveUtil::move(arg07), bslma::Default::allocator(basicAllocator))
, d_arg08(MoveUtil::move(arg08), bslma::Default::allocator(basicAllocator))
, d_arg09(MoveUtil::move(arg09), bslma::Default::allocator(basicAllocator))
, d_arg10(MoveUtil::move(arg10), bslma::Default::allocator(basicAllocator))
, d_arg11(MoveUtil::move(arg11), bslma::Default::allocator(basicAllocator))
, d_arg12(MoveUtil::move(arg12), bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         arg01,
    ArgType02         arg02,
    ArgType03         arg03,
    ArgType04         arg04,
    ArgType05         arg05,
    ArgType06         arg06,
    ArgType07         arg07,
    ArgType08         arg08,
    ArgType09         arg09,
    ArgType10         arg10,
    ArgType11         arg11,
    ArgType12         arg12,
    ArgType13         arg13,
    bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(MoveUtil::move(arg06), bslma::Default::allocator(basicAllocator))
, d_arg07(MoveUtil::move(arg07), bslma::Default::allocator(basicAllocator))
, d_arg08(MoveUtil::move(arg08), bslma::Default::allocator(basicAllocator))
, d_arg09(MoveUtil::move(arg09), bslma::Default::allocator(basicAllocator))
, d_arg10(MoveUtil::move(arg10), bslma::Default::allocator(basicAllocator))
, d_arg11(MoveUtil::move(arg11), bslma::Default::allocator(basicAllocator))
, d_arg12(MoveUtil::move(arg12), bslma::Default::allocator(basicAllocator))
, d_arg13(MoveUtil::move(arg13), bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    ArgType01         arg01,
    ArgType02         arg02,
    ArgType03         arg03,
    ArgType04         arg04,
    ArgType05         arg05,
    ArgType06         arg06,
    ArgType07         arg07,
    ArgType08         arg08,
    ArgType09         arg09,
    ArgType10         arg10,
    ArgType11         arg11,
    ArgType12         arg12,
    ArgType13         arg13,
    ArgType14         arg14,
    bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(MoveUtil::move(arg06), bslma::Default::allocator(basicAllocator))
, d_arg07(MoveUtil::move(arg07), bslma::Default::allocator(basicAllocator))
, d_arg08(MoveUtil::move(arg08), bslma::Default::allocator(basicAllocator))
, d_arg09(MoveUtil::move(arg09), bslma::Default::allocator(basicAllocator))
, d_arg10(MoveUtil::move(arg10), bslma::Default::allocator(basicAllocator))
, d_arg11(MoveUtil::move(arg11), bslma::Default::allocator(basicAllocator))
, d_arg12(MoveUtil::move(arg12), bslma::Default::allocator(basicAllocator))
, d_arg13(MoveUtil::move(arg13), bslma::Default::allocator(basicAllocator))
, d_arg14(MoveUtil::move(arg14), bslma::Default::allocator(basicAllocator))
{
}

AllocEmplacableTestType::AllocEmplacableTestType(
    const AllocEmplacableTestType&  original,
    bslma::Allocator               *basicAllocator)
: d_arg01(original.d_arg01, bslma::Default::allocator(basicAllocator))
, d_arg02(original.d_arg02, bslma::Default::allocator(basicAllocator))
, d_arg03(original.d_arg03, bslma::Default::allocator(basicAllocator))
, d_arg04(original.d_arg04, bslma::Default::allocator(basicAllocator))
, d_arg05(original.d_arg05, bslma::Default::allocator(basicAllocator))
, d_arg06(original.d_arg06, bslma::Default::allocator(basicAllocator))
, d_arg07(original.d_arg07, bslma::Default::allocator(basicAllocator))
, d_arg08(original.d_arg08, bslma::Default::allocator(basicAllocator))
, d_arg09(original.d_arg09, bslma::Default::allocator(basicAllocator))
, d_arg10(original.d_arg10, bslma::Default::allocator(basicAllocator))
, d_arg11(original.d_arg11, bslma::Default::allocator(basicAllocator))
, d_arg12(original.d_arg12, bslma::Default::allocator(basicAllocator))
, d_arg13(original.d_arg13, bslma::Default::allocator(basicAllocator))
, d_arg14(original.d_arg14, bslma::Default::allocator(basicAllocator))
{
}


AllocEmplacableTestType::~AllocEmplacableTestType()
{
    ++s_numDeletes;

    BSLS_ASSERT(d_arg01.allocator() == d_arg02.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg03.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg04.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg05.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg06.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg07.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg08.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg09.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg10.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg11.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg12.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg13.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg14.allocator());
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
