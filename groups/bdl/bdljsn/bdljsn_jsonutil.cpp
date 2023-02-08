// bdljsn_jsonutil.cpp                                                -*-C++-*-
#include <bdljsn_jsonutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdljsn_jsonutil_cpp, "$Id$ $CSID$")

#include <bdljsn_json.h>
#include <bdljsn_jsonnumber.h>

#include <bdljsn_location.h>
#include <bdljsn_readoptions.h>
#include <bdljsn_stringutil.h>
#include <bdljsn_tokenizer.h>
#include <bdljsn_writeoptions.h>
#include <bdljsn_writestyle.h>

#include <bdlb_numericparseutil.h>
#include <bdlde_utf8util.h>
#include <bdlma_localsequentialallocator.h>

#include <bsls_alignedbuffer.h>
#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>

#include <bsl_ostream.h>
#include <bsl_unordered_map.h>
#include <bsl_vector.h>

#include <bsl_iostream.h> // testing only

// BDE_VERIFY pragma: -CD01
// BDE_VERIFY pragma: -TR10

namespace BloombergLP {
namespace bdljsn {
namespace {
namespace u {

                                // Read Implementation

void setError(Error                   *result,
              const Tokenizer&         tokenizer,
              const bsl::string_view&  message)
    // Populate into the specified '*result' an 'Error' with the specified
    // 'message' and taking its location from the specified 'tokenizer'.
{
    result->setMessage(message);
    result->setLocation(Location(tokenizer.currentPosition()));
}

int read(Json *result, Error *error, Tokenizer *tokenizer, int maxNestedDepth);
    // Read into the specified 'result' from the specified 'tokenizer', not
    // exceeding the specified 'maxNestedDepth'.  Return 0 on success, and a
    // non-0 value populating the specified 'error' accordingly on failure.

int readObject(JsonObject *result,
               Error      *error,
               Tokenizer  *tokenizer,
               int         maxNestedDepth)
    // Read into the specified 'result' from the specified 'tokenizer', not
    // exceeding the specified 'maxNestedDepth'.  Return 0 on success, and a
    // non-0 value populating the specified 'error' accordingly on failure.
{
    if (maxNestedDepth < 0) {
        setError(error, *tokenizer, "Maximum nesting depth exceeded");
        return -4;                                                    // RETURN
    }

    // Advance from e_START_OBJECT
    tokenizer->advanceToNextToken();
    if (Tokenizer::e_ERROR == tokenizer->tokenType()) {
        setError(error, *tokenizer, "Unexpected character");
        return -1;                                                    // RETURN
    }

    while (Tokenizer::e_END_OBJECT != tokenizer->tokenType()) {
        if (Tokenizer::e_ELEMENT_NAME != tokenizer->tokenType()) {
            setError(error, *tokenizer, "Expected e_ELEMENT_NAME in object");
            return -2;                                                // RETURN
        }

        bsl::string_view tokenContents;
        tokenizer->value(&tokenContents);

        bsl::string key;
        if (0 != StringUtil::readUnquotedString(&key, tokenContents)) {
            setError(error, *tokenizer, "Invalid UTF-8 string");
            return -3;                                                // RETURN
        }

        // Advance from e_ELEMENT_NAME.  'decodeValue' checks the token, so we
        // don't need to do it here.
        tokenizer->advanceToNextToken();

        int rc;
        // Keep first value found for a given key - discard others.
        if (result->contains(key)) {
            Json temp;

            rc = read(&temp, error, tokenizer, maxNestedDepth);
        }
        else {
            rc = read(&(*result)[key], error, tokenizer, maxNestedDepth);
        }

        if (0 != rc) {
            return rc;                                                // RETURN
        }

        // Advance from e_ELEMENT_VALUE to e_ELEMENT_NAME or e_END_OBJECT
        tokenizer->advanceToNextToken();
    }

    return 0;
}

int readArray(JsonArray *result,
              Error     *error,
              Tokenizer *tokenizer,
              int        maxNestedDepth)
    // Read into the specified 'result' from the specified 'tokenizer', not
    // exceeding the specified 'maxNestedDepth'.  Return 0 on success, and a
    // non-0 value populating the specified 'error' accordingly on failure.
{
    if (maxNestedDepth < 0) {
        setError(error, *tokenizer, "Maximum nesting depth exceeded");
        return -4;                                                    // RETURN
    }

    // Advance from e_START_ARRAY
    tokenizer->advanceToNextToken();
    if (Tokenizer::e_ERROR == tokenizer->tokenType()) {
        setError(error, *tokenizer, "Unexpected character");
        return -1;                                                    // RETURN
    }

    while (Tokenizer::e_END_ARRAY != tokenizer->tokenType()) {
        result->pushBack(Json());

        // 'decodeValue' checks the token, so we don't need to do it here.
        int rc =
            read(&result->back(), error, tokenizer, maxNestedDepth);

        if (0 != rc) {
            return rc;                                                // RETURN
        }

        // Advance from e_ELEMENT_VALUE to e_ELEMENT_VALUE or e_END_ARRAY
        tokenizer->advanceToNextToken();
    }

    return 0;
}

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FABC01
int readScalar(Json *result, Error *error, Tokenizer *tokenizer)
    // Read into the specified 'result' from the specified 'tokenizer', not
    // exceeding the specified 'maxNestedDepth'.  Return 0 on success, and a
    // non-0 value populating the specified 'error' accordingly on failure.
{
    bsl::string_view value;
    tokenizer->value(&value);

    if ("null" == value) {
        BSLS_ASSERT(result->type() == JsonType::e_NULL);
        return 0;                                                     // RETURN
    }

    if ("true" == value || "false" == value) {
        result->makeBoolean("true" == value);
        return 0;                                                     // RETURN
    }

    if ('"' == value[0]) {
        bsl::string str(result->allocator());
        if (0 != StringUtil::readString(&str, value)) {
            setError(error, *tokenizer, "Invalid UTF-8 string");
            return -1;                                                // RETURN
        }
        result->makeString(bslmf::MovableRefUtil::move(str));
        return 0;                                                     // RETURN
    }

    if (NumberUtil::isValidNumber(value)) {
        result->makeNumber(JsonNumber(value));
        return 0;                                                     // RETURN
    }

    setError(error, *tokenizer, "Invalid JSON Number");
    return -1;
}
// BDE_VERIFY pragma: pop

int read(Json *result, Error *error, Tokenizer *tokenizer, int maxNestedDepth)
{
    switch (tokenizer->tokenType()) {
      case Tokenizer::e_START_OBJECT: {
        int rc = u::readObject(
            &result->makeObject(), error, tokenizer, maxNestedDepth - 1);
        if (0 != rc) {
            return rc;                                                // RETURN
        }
      } break;
      case Tokenizer::e_START_ARRAY: {
        int rc = u::readArray(
            &result->makeArray(), error, tokenizer, maxNestedDepth - 1);
        if (0 != rc) {
            return rc;                                                // RETURN
        }
      } break;
      case Tokenizer::e_ELEMENT_VALUE: {
        int rc = u::readScalar(result, error, tokenizer);
        if (0 != rc) {
            return rc;                                                // RETURN
        }
      } break;
      case Tokenizer::e_ERROR: {
        setError(error, *tokenizer, "Unexpected character");
        return -1;                                                    // RETURN
      } break;
      default: {
        bsl::ostringstream ss;
        ss << "Unexpected JSON element: " << tokenizer->tokenType();
        setError(error, *tokenizer, ss.str());
        return -3;                                                    // RETURN
      } break;
    }

    return 0;
}

                                // Write Implementation

// Implementation Note:  The write implementation below uses the visitor
// pattern (though it requires a top level 'write' function because 'Json' does
// not support a 'visit' method).  The class 'WriteVisitor' is a template with
// template parameters that allow for customizing how the visitor iterates over
// the members of a 'JsonObject' (sorted or unsorted), and how white-space is
// formatted (compact, one-line, or pretty).
//
// Below we define 2 "concepts" required by those template parameters for a
// 'WriteVisitor':

template <class VISITOR>
void write(bsl::ostream&  stream,
           const Json&    value,
           int            level,
           const VISITOR& visitor);
    // Write the specified 'value' to the specified 'stream' at the specified
    // 'level' using the specified 'visitor' to write 'JsonObject' and
    // 'JsonArray' types.  Note that this class implements a recursive visit
    // for a 'Json' object with the supplied 'visitor' (this free function is
    // needed because 'Json' itself does not have a 'visit' function at this
    // time).

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202002L
template <typename T>
concept JsonObjectMemberIterator =
    std::constructible_from<const Json *> && requires(T object) {
    // A 'JsonObjectMemberIterator' provides an iterator over the members of an
    // object.  Note that sorted and unsorted implementation are currently
    // defined.

    { object.isValid() }   -> std::same_as<bool>;
    { object.isFirst() }   -> std::same_as<bool>;
    { object.next() }      -> std::same_as<bool>;
    { object.member() }    ->
                       std::same_as<const bsl::pair<const bsl::string, Json>&>;
};

template <typename T>
concept WhitespaceWriter = requires(T            object,
                                    bsl::ostream stream,
                                    int          spacesPerLevel) {
    // A 'WhiteSpaceFormatter' provides functions to write white-space after an
    // object/array element, after a member name separator (i.e., ':'), and
    // after the initial ('[' or '{') or before the final (']' or '}')
    // separator.  Note that pretty, compact, and one-line implementations are
    // currently defined.

    {object.separator(stream, spacesPerLevel)} -> std::same_as<void>;
    {object.braceOrBracketSeparator(stream, spacesPerLevel)}
                                               -> std::same_as<void>;
    {object.memberNameSeparator(stream)}       -> std::same_as<void>;
};

#endif

class JsonObjectUnsortedMemberIterator {
    // This class provides a (non-standard conforming) iterator object that
    // iterates over the elements of a JsonObject (*not* in sorted order).
    // This type matches the constraints (described above) of the
    // JsonObjectMemberIterator concept.

    // DATA
    JsonObject::ConstIterator d_begin;
    JsonObject::ConstIterator d_end;
    JsonObject::ConstIterator d_it;

    // PRIVATE
    JsonObjectUnsortedMemberIterator(const JsonObjectUnsortedMemberIterator&);
    JsonObjectUnsortedMemberIterator& operator=(
                                      const JsonObjectUnsortedMemberIterator&);

  public:
    // CREATORS
    explicit JsonObjectUnsortedMemberIterator(const JsonObject *object)
        // Construct this object, wrapping the specified 'object'.
    : d_begin(object->begin())
    , d_end(object->end())
    , d_it(object->begin())
    {
    }

    // MANIPULATORS
    bool next()
        // Advance to the next member in the object.  Return 'true' if the new
        // position is valid.
    {
        ++d_it;
        return d_it != d_end;
    }

    // ACCESSORS
    bool isFirst() const
        // Return 'true' if this object is in its initial state.
    {
        return d_it == d_begin;
    }

    bool isValid() const
        // Return 'true' if the current position of this iterator is valid.
    {
        return d_it != d_end;
    }

    const bsl::pair<const bsl::string, Json>& member() const
        // Return the current member.
    {
        return *d_it;
    }
};

struct ObjectMemberLess {
    // This struct provides a comparator allowing object entries to be compared
    // lexicographically based on their 'bsl::string' keys.
    bool operator()(const JsonObject::ConstIterator& lhs,
                    const JsonObject::ConstIterator& rhs)
        // Return true if the key of the specified 'lhs' is lexicographically
        // less than the key of the specified 'rhs'.
    {
        return lhs->first < rhs->first;
    }
};

class JsonObjectSortedMemberIterator {
    // This class provides a (non-standard comforming) iterator object that
    // iterates over the elements of a JsonObject in *sorted* order.  This type
    // matches the constraints (described above) of the
    // JsonObjectMemberIterator concept.

    // PRIVATE TYPES
    typedef bsl::vector<JsonObject::ConstIterator> SortedMembers;

    // DATA
    SortedMembers                 d_sortedMembers;
    SortedMembers::const_iterator d_it;

    // PRIVATE
    JsonObjectSortedMemberIterator(const JsonObjectSortedMemberIterator&);
    JsonObjectSortedMemberIterator& operator=(
                                        const JsonObjectSortedMemberIterator&);

  public:
    // CREATORS
    explicit JsonObjectSortedMemberIterator(const JsonObject *object)
        // Construct a 'JsonObjectSortedMemberIterator' to iterate over the
        // specified 'object' in sorted order.
    : d_sortedMembers()
    , d_it()
    {
        d_sortedMembers.reserve(object->size());
        for (JsonObject::ConstIterator it = object->begin();
             it != object->end();
             ++it) {
            d_sortedMembers.push_back(it);
        }
        bsl::sort(d_sortedMembers.begin(),
                  d_sortedMembers.end(),
                  ObjectMemberLess());

        d_it = d_sortedMembers.begin();
    }

    // MANIPULATORS
    bool next()
        // Advance to the next member in the object.  Return 'true' if the new
        // position is valid.
    {
        ++d_it;
        return d_it == d_sortedMembers.end();
    }

    // ACCESSORS
    bool isFirst() const
        // Return 'true' if this object is in its initial state.
    {
        return d_it == d_sortedMembers.begin();
    }

    bool isValid() const
        // Return 'true' if the current position of this iterator is valid.
    {
        return d_it != d_sortedMembers.end();
    }

    const bsl::pair<const bsl::string, Json>& member() const
        // Return the current member.
    {
        return **d_it;
    }
};

class CompactWhitespaceWriter {
    // A 'WhiteSpaceWriter' implementation for rendering white-space for the
    // 'e_COMPACT' style.

  public:
    void braceOrBracketSeparator(bsl::ostream& , int ) const {}
        // Do nothing for this style.
    void memberNameSeparator(bsl::ostream& stream) const { stream << ":"; }
        // Output the ":" separator to the specified 'stream' with no other
        // formatting.
    void separator(bsl::ostream& , int ) const {}
        // Do nothing for this style.
};

class OnelineWhitespaceWriter {
    // A 'WhiteSpaceWriter' implementation for rendering white-space for the
    // 'e_ONELINE' style.

  public:
    void braceOrBracketSeparator(bsl::ostream& , int ) const {}
        // Do nothing for this style.

    void memberNameSeparator(bsl::ostream& stream) const
        // Output the ":" separator to the specified 'stream' with a trailing
        // space for this style.
    {
        stream << bsl::string_view(": ", 2);
    }

    void separator(bsl::ostream& stream, int ) const
        // Output the " " separator to the specified 'stream' for this style.
    {
        stream << ' ';
    }
};

class PrettyWhitespaceWriter {
    // A 'WhiteSpaceWriter' implementation for rendering white-space for the
    // 'e_PRETTY' style, with 'spacesPerLevel' of 0 (one-line format).

    int d_spacesPerLevel;

  public:
    explicit PrettyWhitespaceWriter(int spacesPerLevel)
        // Initialize this object with the specified 'spacesPerLevel'.
    : d_spacesPerLevel(spacesPerLevel)
    {
    }

    void braceOrBracketSeparator(bsl::ostream& stream, int level) const
        // Output to the specified 'stream' a newline followed by the specified
        // 'level' of indentation (where each level of indentation is the
        // number of spaces passed at construction of this
        // 'PrettyWhitespaceWriter').
    {
        bdlb::Print::newlineAndIndent(stream, level, d_spacesPerLevel);
    }

    void memberNameSeparator(bsl::ostream& stream) const
        // Output the ":" separator to the specified 'stream' with a trailing
        // space for this style.
    {
        stream << bsl::string_view(": ", 2);
    }

    void separator(bsl::ostream& stream, int level) const
        // Output to the specified 'stream' a newline followed by the specified
        // 'level' of indentation (where each level of indentation is the
        // number of spaces passed at construction of this
        // 'PrettyWhitespaceWriter').
    {
        bdlb::Print::newlineAndIndent(stream, level, d_spacesPerLevel);
    }
};

template <class WHITESPACE_WRITER, class OBJECT_MEMBER_ITERATOR>
#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202002L
requires WhitespaceWriter<WHITESPACE_WRITER>  &&
         JsonObjectMemberIterator<OBJECT_MEMBER_ITERATOR>
#endif
class WriteVisitor {
    // This type provides a visitor for writing the elements of a 'JsonObejct'
    // and 'JsonArray' types, that uses the constructor argument
    // 'whitespaceWriter' (of template parameter type) 'WHITESPACE_WRITER' to
    // render white-space, and the (template parameter type)
    // 'OBJECT_MEMBER_ITERATOR' to iterate over the members of a 'JsonObject'.
    // Note that this class implements a recursive visit for a 'Json' object
    // with the 'write' free function below.

    // DATA
    WHITESPACE_WRITER d_whitespaceWriter;

  public:
    // CREATOR
    explicit WriteVisitor(const WHITESPACE_WRITER& whitespaceWriter)
        // Construct this object with the specified 'whitespaceWriter' object.
    : d_whitespaceWriter(whitespaceWriter)
    {
    }

    void operator()(bsl::ostream&     stream,
                    const JsonObject& value,
                    int               level) const
        // Output to the specified 'stream' the specified 'value' at the
        // specified 'level' of indentation.
    {
        stream.put('{');

        OBJECT_MEMBER_ITERATOR it(&value);

        bool itemsOutput = it.isValid();

        if (itemsOutput) {
            d_whitespaceWriter.braceOrBracketSeparator(stream, level + 1);
        }

        while (it.isValid()) {
            if (!it.isFirst()) {
                stream.put(',');
                d_whitespaceWriter.separator(stream, level + 1);
            }

            StringUtil::writeString(stream, it.member().first);
            d_whitespaceWriter.memberNameSeparator(stream);
            write(stream, it.member().second, level + 1, *this);
            it.next();
        }

        if (itemsOutput) {
            d_whitespaceWriter.braceOrBracketSeparator(stream, level);
        }

        stream.put('}');
    }

    void operator()(bsl::ostream&    stream,
                    const JsonArray& value,
                    int              level) const
        // Output to the specified 'stream' the specified 'value' at the
        // specified 'level' of indentation.
    {
        stream.put('[');
        JsonArray::ConstIterator it = value.begin();

        bool itemsOutput = (it != value.end());

        if (itemsOutput) {
            d_whitespaceWriter.braceOrBracketSeparator(stream, level + 1);
        }

        for (; it != value.end(); ++it) {
            if (it != value.begin()) {
                stream.put(',');
                d_whitespaceWriter.separator(stream, level + 1);
            }
            write(stream, *it, level + 1, *this);
        }

        // Always represent empty arrays as '[]'.
        if (itemsOutput) {
            d_whitespaceWriter.braceOrBracketSeparator(stream, level);
        }

        stream.put(']');
    }
};

template <class VISITOR>
void write(bsl::ostream&  stream,
           const Json&    value,
           int            level,
           const VISITOR& visitor)
    // Output to the specified 'stream' the specified 'value' at the specified
    // 'level' of indentation, using the specified 'visitor' for formatting.
{
    switch (value.type()) {
      case JsonType::e_ARRAY: {
        visitor(stream, value.theArray(), level);
      } break;
      case JsonType::e_OBJECT: {
        visitor(stream, value.theObject(), level);
      } break;
      case JsonType::e_BOOLEAN: {
        stream << (value.theBoolean() ? bsl::string_view("true", 4)
                                      : bsl::string_view("false", 5));
      } break;
      case JsonType::e_STRING: {
        StringUtil::writeString(stream, value.theString());
      } break;
      case JsonType::e_NUMBER: {
        stream << value.theNumber();
      } break;
      case JsonType::e_NULL: {
        stream << "null";
      } break;
    }
}

template <class OBJECT_MEMBER_ITERATOR>
void writeDispatchFormatting(bsl::ostream&       output,
                             const Json&         json,
                             const WriteOptions& options)
    // Call the 'write' method with the appropriate 'WriteVisitor' type,
    // composed from the (template parameter) 'OBJECT_MEMBER_ITERATOR' and the
    // implementation of the 'WhiteSpaceWriter' concept appropriate for the
    // specified 'options', passing 'write' the specified 'output' and
    // specified 'json'.
{
    if (WriteStyle::e_COMPACT == options.style()) {
        u::CompactWhitespaceWriter writer;

        u::WriteVisitor<u::CompactWhitespaceWriter,
                        OBJECT_MEMBER_ITERATOR> visitor(writer);
        u::write(output, json, options.initialIndentLevel(), visitor);
    }
    else if (WriteStyle::e_ONELINE == options.style()) {
        u::OnelineWhitespaceWriter writer;

        bdlb::Print::indent(
            output, options.initialIndentLevel(), options.spacesPerLevel());
        u::WriteVisitor<u::OnelineWhitespaceWriter,
                        OBJECT_MEMBER_ITERATOR> visitor(writer);
        u::write(output, json, options.initialIndentLevel(), visitor);
    }
    else {
        u::PrettyWhitespaceWriter writer(options.spacesPerLevel());

        bdlb::Print::indent(
            output, options.initialIndentLevel(), options.spacesPerLevel());
        u::WriteVisitor<u::PrettyWhitespaceWriter,
                        OBJECT_MEMBER_ITERATOR> visitor(writer);
        u::write(output, json, options.initialIndentLevel(), visitor);
    }
}

}  // close namespace u
}  // close unnamed namespace

                              // ---------------
                              // struct JsonUtil
                              // ---------------

// CLASS METHODS
int JsonUtil::read(Json               *result,
                   Error              *errorDescription,
                   bsl::streambuf     *input,
                   const ReadOptions&  options)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(errorDescription);
    BSLS_ASSERT(input);

    Error *error = errorDescription;

    bdlma::LocalSequentialAllocator<8 * 1024> bsa;

    Tokenizer tokenizer(&bsa);
    tokenizer.setAllowNonUtf8StringLiterals(false);
    tokenizer.setAllowTrailingTopLevelComma(false);
    tokenizer.reset(input);

    // Advance from e_BEGIN
    tokenizer.advanceToNextToken();
    if (Tokenizer::e_ERROR == tokenizer.tokenType()) {
        u::setError(errorDescription,
                    tokenizer,
                    "Unexpected initial character");
        return -1;                                                    // RETURN
    }

    Json json(result->allocator());
    int  rc =
        u::read(&json, errorDescription, &tokenizer, options.maxNestedDepth());
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    if (!options.allowTrailingText()) {
        rc = tokenizer.advanceToNextToken();
        if (0 == rc) {
            // The tokenizer should report an error if there
            // 'advanceToNextToken' is advanced in an invalid state.

            u::setError(error,
                        tokenizer,
                        "Additional text found after document");
            return -1;                                                // RETURN
        }
        else if (Tokenizer::k_EOF != tokenizer.readStatus()) {
            u::setError(error,
                        tokenizer,
                        "Additional text found after document");
            return -1;                                                // RETURN
        }
    }
    tokenizer.resetStreamBufGetPointer();
    result->swap(json);
    return 0;
}
bsl::ostream& JsonUtil::printError(bsl::ostream&   stream,
                                   bsl::streambuf *input,
                                   const Error&    error)
{
    BSLS_ASSERT(input);

    typedef bdlde::Utf8Util::Uint64 Uint64;

    Uint64 line, column, byteOffset;

    int rc = bdlde::Utf8Util::getLineAndColumnNumber(
        &line, &column, &byteOffset, input, error.location().offset());

    if (0 != rc) {
        stream << "Error (offset " << error.location().offset()
               << "): " << error.message();
        return stream;                                                // RETURN
    }

    stream << "Error (line " << line << ", "
           << "col " << column << "): " << error.message();
    return stream;
}

int JsonUtil::write(bsl::ostream&       output,
                    const Json&         json,
                    const WriteOptions& options)
{
    // Here we select the implementation of the 'JsonObjectMemberIterator'
    // concept appropriate for the specified 'options', then delegate to
    // 'writeDispatchFormatting' (to determine the appropriate implementation
    // of the 'WhiteSpaceFormatter' concept).

    typedef u::JsonObjectUnsortedMemberIterator UnsortedIterator;
    typedef u::JsonObjectSortedMemberIterator   SortedIterator;

    if (options.sortMembers()) {
        u::writeDispatchFormatting<SortedIterator>(output, json, options);
    }
    else {
        u::writeDispatchFormatting<UnsortedIterator>(output, json, options);
    }
    return output.good() ? 0 : -1;
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
