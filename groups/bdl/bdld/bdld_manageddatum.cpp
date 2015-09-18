// bdld_manageddatum.cpp                                              -*-C++-*-

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

#include <bdld_manageddatum.h>
#include <bslmf_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdld {

                            // ------------------
                            // class ManagedDatum
                            // ------------------

BSLMF_ASSERT((bslalg::HasTrait<ManagedDatum,
                               bslalg::TypeTraitUsesBslmaAllocator>::VALUE));
BSLMF_ASSERT((bslalg::HasTrait<ManagedDatum,
                               bslalg::TypeTraitBitwiseMoveable>::VALUE));

// ACCESSORS
bsl::ostream& ManagedDatum::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    return d_data.print(stream, level, spacesPerLevel);
}

}  // close bdld namespace

// FREE OPERATORS
bsl::ostream& bdld::operator<<(bsl::ostream& output, const ManagedDatum& rhs)
{
    return (output << rhs.datum());
}

}  // close BloombergLP namespace

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
