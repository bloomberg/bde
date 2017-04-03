// bdlb_nullablevalue.cpp                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlb_nullablevalue.h>

// TBD temporary!! (to reproduce compilation failure w/xlC)
namespace ParamUtil {
    extern const char L_EQD_CONTRACTS[];
}

namespace ParamUtil {
    const char L_EQD_CONTRACTS[] = "L_EQD_CONTRACTS";
}

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_nullablevalue_cpp,"$Id$ $CSID$")

///IMPLEMENTATION NOTES
///--------------------
// An apparent bug in the IBM xlC compiler (Version: 12.01.0000.0012) required
// that this otherwise unnecessary overload be included in the interface:
//..
//  NullableValue(const TYPE& value);                               // IMPLICIT
//..
// The obscure test case that demonstrates this could not be replicated in the
// test driver:
//..
//  // paramutil.h
//  namespace ParamUtil {
//      extern const char L_SOME_STRING[];
//  }
//
//  // paramutil.cpp
//  #include <paramutil.h>
//  namespace ParamUtil {
//      const char L_SOME_STRING[] = "L_SOME_STRING";
//  }
//
//  // client.cpp
//  #include <paramutil.h>
//  ...
//      bdlb::NullableValue<bsl::string> mX;
//      mX.makeValue(ParamUtil::L_SOME_STRING);
//  ...
//..
//
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
