// s_baltst_featuretestmessageutil.h                                  -*-C++-*-
#ifndef INCLUDED_S_BALTST_FEATURETESTMESSAGEUTIL
#define INCLUDED_S_BALTST_FEATURETESTMESSAGEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities for testing codecs with feature-test messages.
//
//@CLASSES:
//  s_baltst::FeatureTestMessageUtil: utilities for feature-test messages
//
//@SEE_ALSO: s_baltst_featuretestmessage
//
//@DESCRIPTION: This component provides a utility 'struct',
// 's_baltst::FeatureTestMessageUtil', that provides the encoded representation
// of many 's_baltst::FeatureTestMessage' values in 3 different formats:
// pretty JSON, compact JSON, and XML.  These encoded representations may be
// incorporated into the testing infrastructure of codecs.

#include <bsls_platform.h>

namespace BloombergLP {
namespace s_baltst {

                       // =============================
                       // struct FeatureTestMessageUtil
                       // =============================

struct FeatureTestMessageUtil {
    // This utility 'struct' provides a namespace for a set of encoded
    // representations of 's_baltst::FeatureTestMessage' objects in 3
    // different formats: pretty JSON, compact JSON, and XML.  The
    // 's_COMPACT_JSON_MESSAGES', 's_PRETTY_JSON_MESSAGES', and
    // 's_XML_MESSAGES' static data members provide these sets as arrays of
    // string literals.  For all integer indices '0 <= i < k_NUM_MESSAGES', the
    // string literal at index 'i' in each array is an encoded representation
    // of the same value.

    // TYPES
    enum {
#ifdef BSLS_PLATFORM_OS_WINDOWS
        k_NUM_MESSAGES = 68,
#else
        k_NUM_MESSAGES = 70
#endif
            // the number of encoded values stored the
            // 's_COMPACT_JSON_MESSAGES', 's_PRETTY_JSON_MESSAGES', and
            // 's_XML_MESSAGES' arrays
    };

    // CLASS DATA
    static const char *s_COMPACT_JSON_MESSAGES[k_NUM_MESSAGES];
        // an array of 's_baltst::FeatureTestMessage' values encoded in
        // human-readable ("pretty") JSON

    static const char *s_PRETTY_JSON_MESSAGES[k_NUM_MESSAGES];
        // an array of 's_baltst::FeatureTestMessage' values encoded in
        // JSON having no unnecessary whitespace

    static const char *s_XML_MESSAGES[k_NUM_MESSAGES];
        // an array of 's_baltst::FeatureTestMessage' values encoded in
        // human-readable ("pretty") XML
};

}  // close enterprise namespace
}  // close package namespace

#endif // ! defined(INCLUDED_S_BALTST_FEATURETESTMESSAGEUTIL)

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
