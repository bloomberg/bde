// s_baltst_depthtestmessageutil.h                                  -*-C++-*-
#ifndef INCLUDED_S_BALTST_DEPTHTESTMESSAGEUTIL
#define INCLUDED_S_BALTST_DEPTHTESTMESSAGEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide messages for testing codec decoding depth limits.
//
//@CLASSES:
//  s_baltst::DepthTestMessage: XML and depth requirements for a depth-test
//  s_baltst::DepthTestMessageUtil: utilities for depth-test messages
//
//@SEE_ALSO: s_baltst_featuretestmessage
//
//@DESCRIPTION: This component provides two utility `struct`s:
// 1: `s_baltst::DepthTestMessage`, providing an `XML` message and the
// depth limits different encodings needed to decode it
// 2: `s_baltst::DepthTestMessageUtil`, providing an array of
// `DepthTestMessage` values and the size of that array.

#include <bsls_platform.h>

namespace BloombergLP {
namespace s_baltst {

                          // =======================
                          // struct DepthTestMessage
                          // =======================

/// This utility `struct` provides the XML text representations of a test
/// message object, as well as the depth limit required to decode that message
/// with the `BER`, `JSON`, and `XML` decoders.  It also provides additional
/// fields for testing the `JSON` decoder.
struct DepthTestMessage {
    const char *d_XML_text_p;     // XML text
    int         d_depthBER;       // required `maxDepth` value to parse (BER)
    int         d_depthJSON;      // required `maxDepth` value to parse (JSON)
    int         d_depthXML;       // required `maxDepth` value to parse (XML)
    const char *d_prettyJSON_p;   // 'PRETTY' `JSON`
    const char *d_compactJSON_p;  // 'COMPACT' `JSON`
    bool d_isValidForGeneratedMessages;  // Can JSON decode to generated msg
    bool d_isValidForAggregate;          // Can JSON decode to aggregate
};

                        // ===========================
                        // struct DepthTestMessageUtil
                        // ===========================

/// This utility `struct` provides a namespace for a set of `DepthTestMessage`
/// objects.  The `s_TEST_MESSAGES` static data member provides the objects,
/// for all integer indices `0 <= i < k_NUM_MESSAGES`.
struct DepthTestMessageUtil {
    // CLASS DATA

    /// the number of encoded values stored in the `s_TEST_MESSAGES`
    /// array.
    static const int k_NUM_MESSAGES;

    /// an array of `s_baltst::DepthTestMessage` entries
    static const DepthTestMessage *s_TEST_MESSAGES;
};

}  // close enterprise namespace
}  // close package namespace

#endif // ! defined(INCLUDED_S_BALTST_DEPTHTESTMESSAGEUTIL)

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
