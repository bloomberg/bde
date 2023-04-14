// bdlb_pairutil.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLB_PAIRUTIL
#define INCLUDED_BDLB_PAIRUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a function that creates a pair of references.
//
//@CLASSES:
//  bdlb::PairUtil: namespace for the 'bdlb::PairUtil::tie' function
//
//@DESCRIPTION: This component provides the class 'bdlb::PairUtil', which has a
// single static member function, 'tie', which is intended to be used in place
// of 'bsl::tie' when the right-hand side of the assignment is a 'bsl::pair'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
// Suppose we need to implement a function that takes a 'bsl::map' and stores
// into out-parameters the key and value corresponding to the first entry in
// the map.  Using 'bsl::map's container interface, we can obtain a reference
// to a 'bsl::pair' of the key and value.  We can then use
// 'bdlb::PairUtil::tie' to assign from both the key and value in a single
// expression:
//..
//  bool getFirst(int                              *key,
//                bsl::string                      *value,
//                const bsl::map<int, bsl::string>& map)
//      // Load into the specified 'key' and the specified 'value' the key and
//      // value for the first entry in the specified 'map' and return 'true',
//      // or else fail by storing 0 and an empty string and return 'false'
//      // when 'map' is empty.
//  {
//      if (map.empty()) {
//          *key = 0;
//          value->clear();
//          return false;                                             // RETURN
//      }
//      bdlb::PairUtil::tie(*key, *value) = *map.begin();
//      return true;
//  }
//
//  void usageExample()
//  {
//      bsl::map<int, bsl::string> map;
//      map[30782530] = "bbi10";
//
//      int         uuid;
//      bsl::string username;
//
//      bool result = getFirst(&uuid, &username, map);
//      assert(result);
//      assert(30782530 == uuid);
//      assert("bbi10"  == username);
//  }
//..

#include <bdlscm_version.h>

#include <bsl_utility.h>

namespace BloombergLP {
namespace bdlb {

                               // ==============
                               // class PairUtil
                               // ==============

struct PairUtil {
    // This 'struct' provides a namespace for the 'tie' static function.

    // CLASS METHODS
    template <class t_FIRST, class t_SECOND>
    static bsl::pair<t_FIRST&, t_SECOND&> tie(t_FIRST&  first,
                                              t_SECOND& second);
        // Return a 'bsl::pair' object holding a reference to the specified
        // 'first' and a reference to the specified 'second', respectively.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ---------------
                              // struct PairUtil
                              // ---------------

// CLASS METHODS
template <class t_FIRST, class t_SECOND>
inline
bsl::pair<t_FIRST&, t_SECOND&> PairUtil::tie(t_FIRST& first, t_SECOND& second)
{
    return bsl::pair<t_FIRST&, t_SECOND&>(first, second);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
