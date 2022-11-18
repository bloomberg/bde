// bdljsn_json.cpp                                                    -*-C++-*-
#include <bdljsn_json.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdljsn_json_cpp, "$Id$ $CSID$")

#include <bdljsn_stringutil.h>

#include <bsl_ostream.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace bdljsn {
namespace {

class PrintVisitor {
    bsl::ostream *const d_stream_p;        // stream to which is printed
    const int           d_level;           // initial level of indentation
    const int           d_spacesPerLevel;  // spaces per level of indentation

  public:
    PrintVisitor(bsl::ostream *stream, int level, int spacesPerLevel);
        // Create a 'PrintVisitor' object configured to print to the specified
        // 'stream', indented to the specified 'level', using the specified
        // 'spacesPerLevel'.

    void operator()(const JsonNumber& value) const;
        // Stream the specified 'value' to the 'bsl::ostream' provided at
        // construction.

    void operator()(const bsl::string& value) const;
        // Stream the specified 'value' to the 'bsl::ostream' provided at
        // construction.

    void operator()(bool value) const;
        // Stream the specified 'value' to the 'bsl::ostream' provided at
        // construction.

    void operator()(const JsonNull&) const;
        // Stream the specified 'value' to the 'bsl::ostream' provided at
        // construction.

    void operator()(const JsonObject& value) const;
        // Stream the specified 'value' to the 'bsl::ostream' provided at
        // construction.

    void operator()(const JsonArray& value) const;
        // Stream the specified 'value' to the 'bsl::ostream' provided at
        // construction.

    void operator()(bslmf::Nil) const;
        // The behavior is undefined when this method is called.
};

PrintVisitor::PrintVisitor(bsl::ostream *stream, int level, int spacesPerLevel)
: d_stream_p(stream)
, d_level(level)
, d_spacesPerLevel(spacesPerLevel)
{
}

void PrintVisitor::operator()(const JsonNumber& value) const
{
    *d_stream_p << value;
}

void PrintVisitor::operator()(const bsl::string& value) const
{
    StringUtil::writeString(*d_stream_p, value);
}

void PrintVisitor::operator()(bool value) const
{
    *d_stream_p << (value ? "true" : "false");
}

void PrintVisitor::operator()(const JsonNull&) const
{
    *d_stream_p << "null";
}

void PrintVisitor::operator()(const JsonObject& value) const
{
    value.print(*d_stream_p, d_level, d_spacesPerLevel);
}

void PrintVisitor::operator()(const JsonArray& value) const
{
    value.print(*d_stream_p, d_level, d_spacesPerLevel);
}

void PrintVisitor::operator()(bslmf::Nil) const
{
    BSLS_ASSERT(false);
}

}  // close unnamed namespace

                              // ---------------
                              // class JsonArray
                              // ---------------

bsl::ostream& JsonArray::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    if (level > 0) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }
    else {
        level = -level;
    }

    stream << "[";
    ConstIterator it = d_elements.begin();
    for (; it != d_elements.end(); ++it) {
        if (it != d_elements.begin()) {
            stream << ',';
        }
        if (spacesPerLevel >= 0 || it != d_elements.begin()) {
            bdlb::Print::newlineAndIndent(stream, level + 1, spacesPerLevel);
        }
        it->print(stream, -(level + 1), spacesPerLevel);
    }
    if (spacesPerLevel >= 0) {
        bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
    }
    stream << "]";
    return stream;
}

                              // ----------------
                              // class JsonObject
                              // ----------------

bsl::ostream& JsonObject::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    if (level > 0) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }
    else {
        level = -level;
    }

    stream << "{";
    ConstIterator it = d_members.begin();
    for (; it != d_members.end(); ++it) {
        if (it != d_members.begin()) {
            stream << ",";
        }
        if (spacesPerLevel >= 0 || it != d_members.begin()) {
            bdlb::Print::newlineAndIndent(stream, level + 1, spacesPerLevel);
        }
        stream << "\"" << it->first << "\""
               << ": ";
        it->second.print(stream, -(level + 1), spacesPerLevel);
    }
    if (spacesPerLevel >= 0) {
        bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
    }
    stream << "}";

    return stream;
}

                                 // ----------
                                 // class Json
                                 // ----------

bsl::ostream& Json::print(bsl::ostream& stream,
                          int           level,
                          int           spacesPerLevel) const
{
    if (level > 0) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
        level = -level;
    }

    PrintVisitor visitor(&stream, level, spacesPerLevel);
    d_value.apply(visitor);
    return stream;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
