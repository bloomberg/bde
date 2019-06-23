// bostst_map.h                                                    -*-C++-*-
#ifndef INCLUDED_BOSTST_MAP
#define INCLUDED_BOSTST_MAP

#define BSL_OVERRIDES_STD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Verify that 'bsl::map' and 'std::map' are the same type if
//          'BSL_OVERRIDES_STD' is defined.
//
//@CLASSES:
//  bostst::BosMap: mechanism to test that 'bsl::map' and 'std::map' are the
//                  same type
//
//@AUTHOR: Mike Giroux (mgiroux)
//
//@DESCRIPTION: This component provides a mechanism class, 'bostst::BosMap',
// that simply checks that 'bsl::map' and 'std::map' are the same type.
//
///Usage
///-----
// This component has no externally useable functionality.  Its successful
// compilation is the test.

#include <bosscm_version.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <map>
#include <bsl_map.h>

namespace BloombergLP {

namespace bostst {

class BosMap {
    BSLMF_ASSERT(true == (bsl::is_same<bsl::map<int, int>,
                                       std::map<int, int> >::value));
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
