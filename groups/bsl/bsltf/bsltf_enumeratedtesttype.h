// bsltf_enumeratedtesttype.h                                         -*-C++-*-
#ifndef INCLUDED_BSLTF_ENUMERATEDTESTTYPE
#define INCLUDED_BSLTF_ENUMERATEDTESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an enumerated test type.
//
//@CLASSES:
//   bsltf::EnumeratedTestType: enumerated test type
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides an enumeration type,
//'EnumeratedTestType', to facilitate the testing of templates.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using The Enumeration
/// - - - - - - - - - - - - - - - -
// First, we create an 'EnumeratedTestType::Enum' value and initialize it to
// the first possible value:
//..
// EnumeratedTestType::Enum first = EnumeratedTestType::FIRST;
// assert(static_cast<int>(first) == 0);
//..
// Finally, we create an 'EnumeratedTestType::Enum' value and initialize it to
// the last possible value:
//..
// EnumeratedTestType::Enum last = EnumeratedTestType::LAST;
// assert(static_cast<int>(last) == 127);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

namespace bsltf {

                        // =========================
                        // struct EnumeratedTestType
                        // =========================

struct EnumeratedTestType {
    // This 'struct' provides a namespace for defining an 'enum' type that
    // supports explicit conversion to and from an integral type for values
    // from 0 to 127 (according to 7.2.7 of the C++11 standard).

  public:
    // TYPES
    enum Enum {
        FIRST = 0,
        LAST = 127
    };
};

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================


}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
