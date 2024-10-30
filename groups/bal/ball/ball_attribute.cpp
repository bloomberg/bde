// ball_attribute.cpp                                                 -*-C++-*-
#include <ball_attribute.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_attribute_cpp,"$Id$ $CSID$")

#include <bdlb_hashutil.h>

#include <bslim_printer.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>
#include <bsl_string_view.h>

namespace BloombergLP {

namespace {

/// This visitor, when invoked as a non-modifiable function object returns the
/// hash value for the value supplied to the function call operator.
struct ValueHashVisitor {
    /// Return the hash value for the specified `string`.
    unsigned int operator()(const bsl::string& string) const
    {
        return bdlb::HashUtil::hash1(string.data(),
                                     static_cast<int>(string.size()));
    }

    /// Return the hash value for the specified `guid`.
    unsigned int operator()(const bdlb::Guid& guid) const
    {
        return bdlb::HashUtil::hash1(
                                   reinterpret_cast<const char *>(guid.data()),
                                   bdlb::Guid::k_GUID_NUM_BYTES);
    }

    /// Return the hash value for the specified `value` of the parameterized
    /// `t_TYPE`.
    template <class t_TYPE>
    unsigned int operator()(const t_TYPE& value) const
    {
        return bdlb::HashUtil::hash1(value);
    }
};

}  // close unnamed namespace

namespace ball {

                        // ---------------
                        // class Attribute
                        // ---------------

// CLASS METHODS
int Attribute::hash(const Attribute& attribute, int size)
{
    BSLS_ASSERT(0 < size);

    if (attribute.d_hashValue < 0 || attribute.d_hashSize != size) {
        const unsigned int hash =
            bdlb::HashUtil::hash1(attribute.d_name.data(),
                                  static_cast<int>(
                                               bsl::ssize(attribute.d_name))) +
            attribute.d_value.applyRaw<unsigned int>(ValueHashVisitor());

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
    else if (d_value.is<bdlb::Guid>()) {
        char buffer[bdlb::Guid::k_GUID_NUM_CHARS];
        d_value.the<bdlb::Guid>().format(buffer);
        printer.printValue(bsl::string_view(buffer, bsl::size(buffer)));
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
