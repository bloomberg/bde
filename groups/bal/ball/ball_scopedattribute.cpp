// ball_scopedattribute.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_scopedattribute.h>

#include <bdlb_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace ball {
                    // -------------------------------
                    // class ScopedAttribute_Container
                    // -------------------------------

// CREATORS
ScopedAttribute_Container::~ScopedAttribute_Container()
{
}

// ACCESSORS
bsl::ostream&
ScopedAttribute_Container::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    const char EL = spacesPerLevel < 0 ? ' ' : '\n';
    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "[ " << d_attribute << ']' << EL;
    return stream;
}

}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
