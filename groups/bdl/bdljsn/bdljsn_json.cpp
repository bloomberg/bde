// bdljsn_json.cpp                                                    -*-C++-*-
#include <bdljsn_json.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdljsn_json_cpp, "$Id$ $CSID$")

#include <bdljsn_stringutil.h>

#include <bsla_unreachable.h>

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
    /// Create a `PrintVisitor` object configured to print to the specified
    /// `stream`, indented to the specified `level`, using the specified
    /// `spacesPerLevel`.
    PrintVisitor(bsl::ostream *stream, int level, int spacesPerLevel);

    /// Stream the specified `value` to the `bsl::ostream` provided at
    /// construction.
    void operator()(const JsonNumber& value) const;

    /// Stream the specified `value` to the `bsl::ostream` provided at
    /// construction.
    void operator()(const bsl::string& value) const;

    /// Stream the specified `value` to the `bsl::ostream` provided at
    /// construction.
    void operator()(bool value) const;

    /// Stream the specified `value` to the `bsl::ostream` provided at
    /// construction.
    void operator()(const JsonNull&) const;

    /// Stream the specified `value` to the `bsl::ostream` provided at
    /// construction.
    void operator()(const JsonObject& value) const;

    /// Stream the specified `value` to the `bsl::ostream` provided at
    /// construction.
    void operator()(const JsonArray& value) const;

    /// The behavior is undefined when this method is called.
    void operator()(bslmf::Nil) const;
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
    BSLA_UNREACHABLE;
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
                    // ======================
                    // class ConstructVisitor
                    // ======================

class ConstructVisitor {
    Json *d_this_p;

  public:
    explicit ConstructVisitor(Json *json);
        // Create a 'ConstructVisitor' object configured write into the
        // specified 'json'.

    void operator()(const JsonNull &jNull) const;
        // Store the specified 'jNull' into the Json object pointed to by
        // 'd_this_p'.

    void operator()(bool b) const;
        // Store the specified 'b' into the Json object pointed to by
        // 'd_this_p'.

    void operator()(long long ll) const;
        // Store the specified 'll' into the Json object pointed to by
        // 'd_this_p'.

    void operator()(unsigned long long ull) const;
        // Store the specified 'ull' into the Json object pointed to by
        // 'd_this_p'.

    void operator()(double d) const;
        // Store the specified 'd' into the Json object pointed to by
        // 'd_this_p'.

    void operator()(bdldfp::Decimal64 value) const;
        // Store the specified 'value' into the Json object pointed to by
        // 'd_this_p'.

    void operator()(const bsl::string_view& sv) const;
        // Store the specified 'sv' into the Json object pointed to by
        // 'd_this_p'.

    void operator()(const JsonObject *jObj) const;
        // Store the object pointed to by the specified 'jObj' to the 'Json'
        // object pointed to by 'd_this_p'.

    void operator()(const JsonArray *jArr) const;
        // Store the object pointed to by the specified 'jArr' to the 'Json'
        // object pointed to by 'd_this_p'.

    void operator()(const JsonNumber *jNum) const;
        // Store the object pointed to by the specified 'jNum' to the 'Json'
        // object pointed to by 'd_this_p'.

    void operator()(const Json *json) const;
        // Store the object pointed to by the specified 'json' to the 'Json'
        // object pointed to by 'd_this_p'.

    void operator()(std::initializer_list<Json_Initializer> il) const;
        // Store a JsonArray containing the elements of the specified 'il' into
        // the Json object pointed to by 'd_this_p'.

    void operator()(bslmf::Nil) const;
        // The behavior is undefined when this method is called.
};

                    // ---------------------
                    // class ConstructVisitor
                    // ---------------------

ConstructVisitor::ConstructVisitor(Json *json)
: d_this_p(json)
{
}

void ConstructVisitor::operator()(const JsonNull &jNull) const
{
    (void)jNull;
}

void ConstructVisitor::operator()(bool b) const
{
    d_this_p->makeBoolean(b);
}

void ConstructVisitor::operator()(long long ll) const
{
    d_this_p->makeNumber(JsonNumber(ll));
}

void ConstructVisitor::operator()(unsigned long long ull) const
{
    d_this_p->makeNumber(JsonNumber(ull));
}

void ConstructVisitor::operator()(double d) const
{
    d_this_p->makeNumber(JsonNumber(d));
}

void ConstructVisitor::operator()(bdldfp::Decimal64 value) const
{
    d_this_p->makeNumber(JsonNumber(value));
}

void ConstructVisitor::operator()(const bsl::string_view& sv) const
{
    d_this_p->makeString(sv);
}

void ConstructVisitor::operator()(const JsonObject *jObj) const
{
    d_this_p->makeObject(*jObj);
}

void ConstructVisitor::operator()(const JsonArray *jArr) const
{
    d_this_p->makeArray(*jArr);
}

void ConstructVisitor::operator()(const JsonNumber *jNum) const
{
    d_this_p->makeNumber(*jNum);
}

void ConstructVisitor::operator()(const Json *json) const
{
    *d_this_p = *json;
}

void ConstructVisitor::operator()(
                              std::initializer_list<Json_Initializer> il) const
{
    JsonArray arr;
    for (const auto & val : il) {
        arr.pushBack(val);
    }
    d_this_p->makeArray(bslmf::MovableRefUtil::move(arr));
}

void ConstructVisitor::operator()(bslmf::Nil) const
{
    BSLA_UNREACHABLE;
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

#endif
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

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
bsl::pair<JsonObject::Iterator, bool> JsonObject::insert(
                                                  const bsl::string_view& key,
                                                  const Json_Initializer& init)
{
    BSLS_ASSERT(bdlde::Utf8Util::isValid(key.data(), key.size()));
    Json   json(init, allocator());
    Member member(key, Json(), allocator());
    member.second = bslmf::MovableRefUtil::move(json);
    return insert(bslmf::MovableRefUtil::move(member));
}
#endif

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

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
Json::Json(const Json_Initializer& init, bslma::Allocator *basicAllocator)
: d_value(JsonNull(), basicAllocator)
{
    init.get_storage().apply(ConstructVisitor(this));
}
#endif

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
