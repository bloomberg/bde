// bdlar_refdatautil.h                                                -*-C++-*-
#ifndef INCLUDED_BDLAR_REFDATAUTIL
#define INCLUDED_BDLAR_REFDATAUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::RefDataUtil: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_anyrefdata.h>
#include <bdlar_vtableutil.h>

namespace BloombergLP {
namespace bdlar {

                           // ==================
                           // struct RefDataUtil
                           // ==================

struct RefDataUtil {
    // CLASS METHODS
    template <class t_TYPE>
    static AnyConstRefData makeAnyConstRefData(const t_TYPE *object);

    template <class t_TYPE>
    static AnyRefData makeAnyRefData(t_TYPE *object);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ------------------
                           // struct RefDataUtil
                           // ------------------

template <class t_TYPE>
inline
AnyConstRefData RefDataUtil::makeAnyConstRefData(const t_TYPE *object)
{
    AnyConstRefData result = {
        object,
        VtableUtil::getConstVtable<t_TYPE>(),
        SelectCategory<t_TYPE>::e_SELECTION
    };
    return result;
}

template <class t_TYPE>
inline
AnyRefData RefDataUtil::makeAnyRefData(t_TYPE *object)
{
    AnyRefData result = {
        object,
        VtableUtil::getVtable<t_TYPE>(),
        SelectCategory<t_TYPE>::e_SELECTION
    };
    return result;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
