// bdlm_instancecount.h                                               -*-C++-*-
#ifndef INCLUDED_BDLM_INSTANCECOUNT
#define INCLUDED_BDLM_INSTANCECOUNT

//@PURPOSE: Provide a type specific instance count.
//
//@CLASSES:
// bdlm::InstanceCount: type specific instance count
//
//@DESCRIPTION: This component provides a mechanism, `bdlm::InstanceCount`, for
// providing type specific instance counts.
//
///Thread Safety
///-------------
// This class is *fully thread-safe*
// (see {`bsldoc_glossary`|Fully Thread-Safe}).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using `bdlm::InstanceCount`
///- - - - - - - - - - - - - - - - - - -
// This example demonstrates the usage of `bdlm::InstanceCount` to obtain type
// specific instance counts.
//
// First, we obtain and verify the instance counts for a few types:
// ```
// assert(1 == bdlm::InstanceCount::nextInstanceNumber<char>());
// assert(1 == bdlm::InstanceCount::nextInstanceNumber<int>());
// assert(1 == bdlm::InstanceCount::nextInstanceNumber<double>());
// ```
// Then, we obtain and verify the instance counts for the previous types and
// some new types:
// ```
// assert(2 == bdlm::InstanceCount::nextInstanceNumber<char>());
// assert(2 == bdlm::InstanceCount::nextInstanceNumber<int>());
// assert(1 == bdlm::InstanceCount::nextInstanceNumber<unsigned>());
// assert(1 == bdlm::InstanceCount::nextInstanceNumber<float>());
// assert(2 == bdlm::InstanceCount::nextInstanceNumber<double>());
// ```
// Finally, we obtain and verify the next instance counts for these types:
// ```
// assert(3 == bdlm::InstanceCount::nextInstanceNumber<char>());
// assert(3 == bdlm::InstanceCount::nextInstanceNumber<int>());
// assert(2 == bdlm::InstanceCount::nextInstanceNumber<unsigned>());
// assert(2 == bdlm::InstanceCount::nextInstanceNumber<float>());
// assert(3 == bdlm::InstanceCount::nextInstanceNumber<double>());
// ```

#include <bsls_ident.h>
BSLS_IDENT("$Id$")

#include <bsls_atomicoperations.h>
#include <bsls_types.h>

namespace BloombergLP {
namespace bdlm {

                           // ===================
                           // class InstanceCount
                           // ===================

/// This mechanism provides type specific instance counts.
struct InstanceCount {

    // TYPES
    typedef bsls::Types::Uint64 Value;  // storage type for instance counts

    // CLASS METHODS

    /// Return the next instance number for the specified `OBJECT_TYPE`.
    template <class OBJECT_TYPE>
    static Value nextInstanceNumber();
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                           // -------------------
                           // class InstanceCount
                           // -------------------

// CLASS METHODS
template <class OBJECT_TYPE>
inline
InstanceCount::Value InstanceCount::nextInstanceNumber()
{
    static bsls::AtomicOperations::AtomicTypes::Uint64 count;
    return bsls::AtomicOperations::incrementUint64Nv(&count);
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
