// ball_attribute.cpp                                                 -*-C++-*-
#include <ball_attribute.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_attribute_cpp,"$Id$ $CSID$")

#include <bdlb_bitutil.h>
#include <bdlb_hashutil.h>

#include <bslim_printer.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_functional.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ball {

                        // ---------------
                        // class Attribute
                        // ---------------

// CLASS METHODS
int Attribute::hash(const Attribute& attribute, int size)
{
    BSLS_ASSERT(0 < size);

    if (attribute.d_hashValue < 0 || attribute.d_hashSize != size) {

        unsigned int hash = bdlb::HashUtil::hash1(
                              attribute.d_name,
                              static_cast<int>(bsl::strlen(attribute.d_name)));

        if (attribute.d_value.is<int>()) {
            hash += bdlb::HashUtil::hash1(attribute.d_value.the<int>());
        } else if (attribute.d_value.is<long>()) {
            hash += bdlb::HashUtil::hash1(attribute.d_value.the<long>());
        } else if (attribute.d_value.is<long long>()) {
            hash += bdlb::HashUtil::hash1(attribute.d_value.the<long long>());
        } else if (attribute.d_value.is<unsigned int>()) {
            hash += bdlb::HashUtil::hash1(
                                       attribute.d_value.the<unsigned int>());
        } else if (attribute.d_value.is<unsigned long>()) {
            hash += bdlb::HashUtil::hash1(
                                       attribute.d_value.the<unsigned long>());
        } else if (attribute.d_value.is<unsigned long long>()) {
            hash += bdlb::HashUtil::hash1(
                                  attribute.d_value.the<unsigned long long>());
        }
        else  if (attribute.d_value.is<bsl::string>()) {
            hash += bdlb::HashUtil::hash1(
                attribute.d_value.the<bsl::string>().c_str(),
                static_cast<int>(
                               attribute.d_value.the<bsl::string>().length()));
        }
        else  if (attribute.d_value.is<const void *>()) {
            hash += bdlb::HashUtil::hash1(
                                        attribute.d_value.the<const void *>());
        }
        else {
            BSLS_ASSERT_INVOKE_NORETURN("unreachable");
        }

        attribute.d_hashValue = hash % size;
        attribute.d_hashSize  = size;
    }
    return attribute.d_hashValue;
}

// ACCESSORS
bsl::ostream& Attribute::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    // We use a negative spacesPerLevel to ensure the output is rendered on
    // one line.

    bslim::Printer indent(&stream, level, spacesPerLevel);
    indent.printIndentation();

    bslim::Printer printer(&stream, 0, -1);
    stream << "[";
    printer.printValue(d_name);
    stream << " =";
    if (d_value.is<const void *>()) {
        printer.printHexAddr(d_value.the<const void *>(), 0);
    }
    else {
        printer.printValue(d_value);
    }
    stream << " ]";

    if (spacesPerLevel >= 0) {
        stream << "\n";
    }
    return stream;
}

}  // close package namespace

// FREE OPERATORS
bsl::ostream& ball::operator<<(bsl::ostream&    output,
                               const Attribute& attribute)
{
    attribute.print(output, 0, -1);
    return output;
}

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
