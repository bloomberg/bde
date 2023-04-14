// bdljsn_jsonliterals.h                                              -*-C++-*-
#ifndef INCLUDED_BDLJSN_JSONLITERALS
#define INCLUDED_BDLJSN_JSONLITERALS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide user-defined literals for 'bdljsn::Json' objects.
//
//@CLASSES:
//  bdljsn::JsonLiterals: a namespace for user-defined literal operators
//
//@DESCRIPTION: This component provides a namespace, 'bdljsn::JsonLiterals', in
// which operators for the user-defined literal suffix "_json" are defined.
// Users can define a using declaration for the 'JsonLiterals' namespace, and
// apply the "_json" suffix to literal text containing JSON to create
// 'bdljsn::Json' objects (on platforms that support user defined literals).
//
// For example:
//..
//  using namespace bdljsn::JsonLiterals;
//  bdljsn::Json json = R"({"price": 2.1})"_json;
//..
//
///Use of the Global Allocator
///---------------------------
// The 'operator "" _json' returns a 'bdljsn::Json' object that allocates
// memory using the currently installed global allocator.  Using the global
// allocator prevents inadvertently locking the default allocator, as may
// happen before 'main' when using a JSON literal to initialize an object at
// global file-scope static storage duration.  Note that while file scoped
// static objects can be useful in testing, we discourage their use in
// production code.
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Creating a 'bdljsn::Json' Object with a User Defined Literal
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This component provides a namespace in which user-defined literal operators
// for 'bdljsn::Json' are defined.  In this example we use a '_json' literal
// to initialize a 'bdljsn::Json' object.
//
// First, we define a user declaration for the appropriate namespace:
//..
//  using namespace bdljsn::JsonLiterals;
//..
// Then we create a 'bdljsn::Json' object:
//..
//  bdljsn::Json json = R"({ "number": 4, "array": [0, 2, null] })"_json;
//
//  assert(bdljsn::JsonType::e_NUMBER == json["number"].type());
//  assert(bdljsn::JsonType::e_ARRAY  == json["array"].type());
//..
// Notice that the user-defined literal operator will unconditionally invoke
// the 'bsls::Assert' handler if the literal text is not valid JSON.

#include <bdlscm_version.h>

#include <bdljsn_json.h>

#include <bsls_compilerfeatures.h>

namespace BloombergLP {
namespace bdljsn {

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)  && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS)
inline namespace literals {
inline namespace JsonLiterals {
bdljsn::Json operator "" _json (const char *text, bsl::size_t numBytes);
    // Return a 'bdljsn::Json' object having the value of the JSON described in
    // the specified 'text' of the specified 'numBytes'.  If 'text' is not a
    // valid JSON document then invoke the currently installed 'bsls::Assert'
    // failure handler.

}  // close JsonLiterals namespace
}  // close literals namespace
#endif


// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================



}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BDLJSN_JSONLITERALS

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
