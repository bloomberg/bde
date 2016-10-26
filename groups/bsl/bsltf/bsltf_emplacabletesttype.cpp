// bsltf_emplacabletesttype.cpp                                       -*-C++-*-
#include <bsltf_emplacabletesttype.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {
namespace bsltf {

                        // ------------------------
                        // class EmplacableTestType
                        // ------------------------

// CLASS DATA
int EmplacableTestType::s_numDeletes = 0;

// CLASS METHODS
int EmplacableTestType::getNumDeletes()
{
    return s_numDeletes;
}

// CREATORS
EmplacableTestType::EmplacableTestType()
{
}

EmplacableTestType::EmplacableTestType(ArgType01 arg01)
: d_arg01(arg01)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 arg01, ArgType02 arg02)
: d_arg01(arg01)
, d_arg02(arg02)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 arg01,
                                       ArgType02 arg02,
                                       ArgType03 arg03)
: d_arg01(arg01)
, d_arg02(arg02)
, d_arg03(arg03)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 arg01,
                                       ArgType02 arg02,
                                       ArgType03 arg03,
                                       ArgType04 arg04)
: d_arg01(arg01)
, d_arg02(arg02)
, d_arg03(arg03)
, d_arg04(arg04)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 arg01,
                                       ArgType02 arg02,
                                       ArgType03 arg03,
                                       ArgType04 arg04,
                                       ArgType05 arg05)
: d_arg01(arg01)
, d_arg02(arg02)
, d_arg03(arg03)
, d_arg04(arg04)
, d_arg05(arg05)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 arg01,
                                       ArgType02 arg02,
                                       ArgType03 arg03,
                                       ArgType04 arg04,
                                       ArgType05 arg05,
                                       ArgType06 arg06)
: d_arg01(arg01)
, d_arg02(arg02)
, d_arg03(arg03)
, d_arg04(arg04)
, d_arg05(arg05)
, d_arg06(arg06)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 arg01,
                                       ArgType02 arg02,
                                       ArgType03 arg03,
                                       ArgType04 arg04,
                                       ArgType05 arg05,
                                       ArgType06 arg06,
                                       ArgType07 arg07)
: d_arg01(arg01)
, d_arg02(arg02)
, d_arg03(arg03)
, d_arg04(arg04)
, d_arg05(arg05)
, d_arg06(arg06)
, d_arg07(arg07)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 arg01,
                                       ArgType02 arg02,
                                       ArgType03 arg03,
                                       ArgType04 arg04,
                                       ArgType05 arg05,
                                       ArgType06 arg06,
                                       ArgType07 arg07,
                                       ArgType08 arg08)
: d_arg01(arg01)
, d_arg02(arg02)
, d_arg03(arg03)
, d_arg04(arg04)
, d_arg05(arg05)
, d_arg06(arg06)
, d_arg07(arg07)
, d_arg08(arg08)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 arg01,
                                       ArgType02 arg02,
                                       ArgType03 arg03,
                                       ArgType04 arg04,
                                       ArgType05 arg05,
                                       ArgType06 arg06,
                                       ArgType07 arg07,
                                       ArgType08 arg08,
                                       ArgType09 arg09)
: d_arg01(arg01)
, d_arg02(arg02)
, d_arg03(arg03)
, d_arg04(arg04)
, d_arg05(arg05)
, d_arg06(arg06)
, d_arg07(arg07)
, d_arg08(arg08)
, d_arg09(arg09)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 arg01,
                                       ArgType02 arg02,
                                       ArgType03 arg03,
                                       ArgType04 arg04,
                                       ArgType05 arg05,
                                       ArgType06 arg06,
                                       ArgType07 arg07,
                                       ArgType08 arg08,
                                       ArgType09 arg09,
                                       ArgType10 arg10)
: d_arg01(arg01)
, d_arg02(arg02)
, d_arg03(arg03)
, d_arg04(arg04)
, d_arg05(arg05)
, d_arg06(arg06)
, d_arg07(arg07)
, d_arg08(arg08)
, d_arg09(arg09)
, d_arg10(arg10)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 arg01,
                                       ArgType02 arg02,
                                       ArgType03 arg03,
                                       ArgType04 arg04,
                                       ArgType05 arg05,
                                       ArgType06 arg06,
                                       ArgType07 arg07,
                                       ArgType08 arg08,
                                       ArgType09 arg09,
                                       ArgType10 arg10,
                                       ArgType11 arg11)
: d_arg01(arg01)
, d_arg02(arg02)
, d_arg03(arg03)
, d_arg04(arg04)
, d_arg05(arg05)
, d_arg06(arg06)
, d_arg07(arg07)
, d_arg08(arg08)
, d_arg09(arg09)
, d_arg10(arg10)
, d_arg11(arg11)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 arg01,
                                       ArgType02 arg02,
                                       ArgType03 arg03,
                                       ArgType04 arg04,
                                       ArgType05 arg05,
                                       ArgType06 arg06,
                                       ArgType07 arg07,
                                       ArgType08 arg08,
                                       ArgType09 arg09,
                                       ArgType10 arg10,
                                       ArgType11 arg11,
                                       ArgType12 arg12)
: d_arg01(arg01)
, d_arg02(arg02)
, d_arg03(arg03)
, d_arg04(arg04)
, d_arg05(arg05)
, d_arg06(arg06)
, d_arg07(arg07)
, d_arg08(arg08)
, d_arg09(arg09)
, d_arg10(arg10)
, d_arg11(arg11)
, d_arg12(arg12)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 arg01,
                                       ArgType02 arg02,
                                       ArgType03 arg03,
                                       ArgType04 arg04,
                                       ArgType05 arg05,
                                       ArgType06 arg06,
                                       ArgType07 arg07,
                                       ArgType08 arg08,
                                       ArgType09 arg09,
                                       ArgType10 arg10,
                                       ArgType11 arg11,
                                       ArgType12 arg12,
                                       ArgType13 arg13)
: d_arg01(arg01)
, d_arg02(arg02)
, d_arg03(arg03)
, d_arg04(arg04)
, d_arg05(arg05)
, d_arg06(arg06)
, d_arg07(arg07)
, d_arg08(arg08)
, d_arg09(arg09)
, d_arg10(arg10)
, d_arg11(arg11)
, d_arg12(arg12)
, d_arg13(arg13)
{
}

EmplacableTestType::EmplacableTestType(ArgType01 arg01,
                                       ArgType02 arg02,
                                       ArgType03 arg03,
                                       ArgType04 arg04,
                                       ArgType05 arg05,
                                       ArgType06 arg06,
                                       ArgType07 arg07,
                                       ArgType08 arg08,
                                       ArgType09 arg09,
                                       ArgType10 arg10,
                                       ArgType11 arg11,
                                       ArgType12 arg12,
                                       ArgType13 arg13,
                                       ArgType14 arg14)
: d_arg01(arg01)
, d_arg02(arg02)
, d_arg03(arg03)
, d_arg04(arg04)
, d_arg05(arg05)
, d_arg06(arg06)
, d_arg07(arg07)
, d_arg08(arg08)
, d_arg09(arg09)
, d_arg10(arg10)
, d_arg11(arg11)
, d_arg12(arg12)
, d_arg13(arg13)
, d_arg14(arg14)
{
}

EmplacableTestType::EmplacableTestType(const EmplacableTestType& original)
: d_arg01(original.d_arg01)
, d_arg02(original.d_arg02)
, d_arg03(original.d_arg03)
, d_arg04(original.d_arg04)
, d_arg05(original.d_arg05)
, d_arg06(original.d_arg06)
, d_arg07(original.d_arg07)
, d_arg08(original.d_arg08)
, d_arg09(original.d_arg09)
, d_arg10(original.d_arg10)
, d_arg11(original.d_arg11)
, d_arg12(original.d_arg12)
, d_arg13(original.d_arg13)
, d_arg14(original.d_arg14)
{
}

EmplacableTestType::~EmplacableTestType()
{
    ++s_numDeletes;
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
