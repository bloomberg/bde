// s_baltst_testattribute.cpp                                         -*-C++-*-
#include <s_baltst_testattribute.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_testattribute_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace s_baltst {


                        // ----------------------------
                        // struct TestAttributeDefaults
                        // ----------------------------

// CLASS DATA
const char TestAttributeDefaults::k_DEFAULT_ANNOTATION[1] = "";
const char TestAttributeDefaults::k_DEFAULT_NAME[1]       = "";

                         // -------------------------
                         // struct TestAttribute_Data
                         // -------------------------

// PUBLIC CLASS DATA
const char TestAttribute_Data::k_ATTRIBUTE_1_NAME[sizeof("attr1")]  = "attr1";
const char TestAttribute_Data::k_ATTRIBUTE_1_ANNOTATION[sizeof("")] = "";

const char TestAttribute_Data::k_ATTRIBUTE_2_NAME[sizeof("attr2")]  = "attr2";
const char TestAttribute_Data::k_ATTRIBUTE_2_ANNOTATION[sizeof("")] = "";

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
