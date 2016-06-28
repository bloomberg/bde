// bsltf_emplacabletesttype.cpp                                       -*-C++-*-
#include <bsltf_emplacabletesttype.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {
namespace bsltf {

int EmplacableTestType::s_numDeletes = 0;

EmplacableTestType::EmplacableTestType()
{
}

EmplacableTestType::EmplacableTestType(ArgType01 a01)
: d_a01(a01)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 a01, ArgType02 a02)
: d_a01(a01)
, d_a02(a02)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 a01,
                                       ArgType02 a02,
                                       ArgType03 a03)
: d_a01(a01), d_a02(a02), d_a03(a03)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 a01,
                                       ArgType02 a02,
                                       ArgType03 a03,
                                       ArgType04 a04)
: d_a01(a01), d_a02(a02), d_a03(a03), d_a04(a04)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 a01,
                                       ArgType02 a02,
                                       ArgType03 a03,
                                       ArgType04 a04,
                                       ArgType05 a05)
: d_a01(a01), d_a02(a02), d_a03(a03), d_a04(a04), d_a05(a05)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 a01,
                                       ArgType02 a02,
                                       ArgType03 a03,
                                       ArgType04 a04,
                                       ArgType05 a05,
                                       ArgType06 a06)
: d_a01(a01), d_a02(a02), d_a03(a03), d_a04(a04), d_a05(a05), d_a06(a06)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 a01,
                                       ArgType02 a02,
                                       ArgType03 a03,
                                       ArgType04 a04,
                                       ArgType05 a05,
                                       ArgType06 a06,
                                       ArgType07 a07)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
, d_a05(a05)
, d_a06(a06)
, d_a07(a07)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 a01,
                                       ArgType02 a02,
                                       ArgType03 a03,
                                       ArgType04 a04,
                                       ArgType05 a05,
                                       ArgType06 a06,
                                       ArgType07 a07,
                                       ArgType08 a08)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
, d_a05(a05)
, d_a06(a06)
, d_a07(a07)
, d_a08(a08)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 a01,
                                       ArgType02 a02,
                                       ArgType03 a03,
                                       ArgType04 a04,
                                       ArgType05 a05,
                                       ArgType06 a06,
                                       ArgType07 a07,
                                       ArgType08 a08,
                                       ArgType09 a09)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
, d_a05(a05)
, d_a06(a06)
, d_a07(a07)
, d_a08(a08)
, d_a09(a09)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 a01,
                                       ArgType02 a02,
                                       ArgType03 a03,
                                       ArgType04 a04,
                                       ArgType05 a05,
                                       ArgType06 a06,
                                       ArgType07 a07,
                                       ArgType08 a08,
                                       ArgType09 a09,
                                       ArgType10 a10)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
, d_a05(a05)
, d_a06(a06)
, d_a07(a07)
, d_a08(a08)
, d_a09(a09)
, d_a10(a10)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 a01,
                                       ArgType02 a02,
                                       ArgType03 a03,
                                       ArgType04 a04,
                                       ArgType05 a05,
                                       ArgType06 a06,
                                       ArgType07 a07,
                                       ArgType08 a08,
                                       ArgType09 a09,
                                       ArgType10 a10,
                                       ArgType11 a11)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
, d_a05(a05)
, d_a06(a06)
, d_a07(a07)
, d_a08(a08)
, d_a09(a09)
, d_a10(a10)
, d_a11(a11)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 a01,
                                       ArgType02 a02,
                                       ArgType03 a03,
                                       ArgType04 a04,
                                       ArgType05 a05,
                                       ArgType06 a06,
                                       ArgType07 a07,
                                       ArgType08 a08,
                                       ArgType09 a09,
                                       ArgType10 a10,
                                       ArgType11 a11,
                                       ArgType12 a12)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
, d_a05(a05)
, d_a06(a06)
, d_a07(a07)
, d_a08(a08)
, d_a09(a09)
, d_a10(a10)
, d_a11(a11)
, d_a12(a12)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 a01,
                                       ArgType02 a02,
                                       ArgType03 a03,
                                       ArgType04 a04,
                                       ArgType05 a05,
                                       ArgType06 a06,
                                       ArgType07 a07,
                                       ArgType08 a08,
                                       ArgType09 a09,
                                       ArgType10 a10,
                                       ArgType11 a11,
                                       ArgType12 a12,
                                       ArgType13 a13)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
, d_a05(a05)
, d_a06(a06)
, d_a07(a07)
, d_a08(a08)
, d_a09(a09)
, d_a10(a10)
, d_a11(a11)
, d_a12(a12)
, d_a13(a13)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 a01,
                                       ArgType02 a02,
                                       ArgType03 a03,
                                       ArgType04 a04,
                                       ArgType05 a05,
                                       ArgType06 a06,
                                       ArgType07 a07,
                                       ArgType08 a08,
                                       ArgType09 a09,
                                       ArgType10 a10,
                                       ArgType11 a11,
                                       ArgType12 a12,
                                       ArgType13 a13,
                                       ArgType14 a14)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
, d_a05(a05)
, d_a06(a06)
, d_a07(a07)
, d_a08(a08)
, d_a09(a09)
, d_a10(a10)
, d_a11(a11)
, d_a12(a12)
, d_a13(a13)
, d_a14(a14)
{
}

EmplacableTestType::EmplacableTestType(const EmplacableTestType& original)
: d_a01(original.d_a01)
, d_a02(original.d_a02)
, d_a03(original.d_a03)
, d_a04(original.d_a04)
, d_a05(original.d_a05)
, d_a06(original.d_a06)
, d_a07(original.d_a07)
, d_a08(original.d_a08)
, d_a09(original.d_a09)
, d_a10(original.d_a10)
, d_a11(original.d_a11)
, d_a12(original.d_a12)
, d_a13(original.d_a13)
, d_a14(original.d_a14)
{
}

EmplacableTestType::~EmplacableTestType()
{
    ++s_numDeletes;
}

int EmplacableTestType::getNumDeletes()
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
