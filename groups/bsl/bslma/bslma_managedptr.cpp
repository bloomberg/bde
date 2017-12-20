// bslma_managedptr.cpp                                               -*-C++-*-
#include <bslma_managedptr.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_defaultallocatorguard.h>    // for testing only
#include <bslma_testallocator.h>            // for testing only
#include <bslma_testallocatormonitor.h>     // for testing only

// Design Notes
// ------------
// These notes are recorded here as they are not part of the public interface,
// but reflect certain design decisions taken when implementing this component.
// A number of redundant constructors were removed from earlier designs in
// order to produce the leanest component with minimal template bloat.  One
// side-effect of this is that some signatures, such as the constructor for
// converting from a managed-pointer-of-a-different-type, are implicit and so
// are no longer clearly documented in an obvious place of their own.
//
// A second design decision was to implement aliasing as member-function
// templates taking references to arbitrary managed pointer types.  The other
// approach considered was to take 'ManagedPtr_Ref' objects by value, like the
// move operations.  This was ruled out due to the static-assert in the
// conversion-to-_Ref operator, that enforces a type-compatibility relationship
// between the aliased type and the managed pointer target type.  This
// restriction is not present in the current contract, and the risks opened by
// removing the static-assert and allowing general conversions everywhere (as
// undefined behavior) were seen as too large.

namespace BloombergLP {
namespace bslma {

                       // ---------------------
                       // struct ManagedPtrUtil
                       // ---------------------

// CLASS METHODS
void ManagedPtrUtil::noOpDeleter(void *, void *)
{
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
