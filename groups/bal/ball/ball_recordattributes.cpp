// ball_recordattributes.cpp                                          -*-C++-*-
#include <ball_recordattributes.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_recordattributes_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ball {

                        // ----------------------
                        // class RecordAttributes
                        // ----------------------

// CREATORS
RecordAttributes::RecordAttributes(bslma::Allocator *basicAllocator)
: d_timestamp()
, d_processID(0)
, d_threadID(0)
, d_fileName(basicAllocator)
, d_lineNumber(0)
, d_category(basicAllocator)
, d_severity(0)
, d_messageStreamBuf(basicAllocator)
{
}

RecordAttributes::RecordAttributes(const bdlt::Datetime&  timestamp,
                                   int                    processID,
                                   bsls::Types::Uint64    threadID,
                                   const char            *fileName,
                                   int                    lineNumber,
                                   const char            *category,
                                   int                    severity,
                                   const char            *message,
                                   bslma::Allocator      *basicAllocator)
: d_timestamp(timestamp)
, d_processID(processID)
, d_threadID(threadID)
, d_fileName(fileName, basicAllocator)
, d_lineNumber(lineNumber)
, d_category(category, basicAllocator)
, d_severity(severity)
, d_messageStreamBuf(basicAllocator)
{
    setMessage(message);
}

RecordAttributes::RecordAttributes(const RecordAttributes&  original,
                                   bslma::Allocator        *basicAllocator)
: d_timestamp(original.d_timestamp)
, d_processID(original.d_processID)
, d_threadID(original.d_threadID)
, d_fileName(original.d_fileName, basicAllocator)
, d_lineNumber(original.d_lineNumber)
, d_category(original.d_category, basicAllocator)
, d_severity(original.d_severity)
, d_messageStreamBuf(basicAllocator)
{
    d_messageStreamBuf.pubseekpos(0);
    d_messageStreamBuf.sputn(original.d_messageStreamBuf.data(),
                             original.d_messageStreamBuf.length());
}

// MANIPULATORS
void RecordAttributes::setMessage(const char *message)
{
    d_messageStreamBuf.pubseekpos(0);
    while (*message) {
        d_messageStreamBuf.sputc(*message);
        ++message;
    }
}

RecordAttributes& RecordAttributes::operator=(const RecordAttributes& rhs)
{
    if (this != &rhs) {
        d_timestamp  = rhs.d_timestamp;
        d_processID  = rhs.d_processID;
        d_threadID   = rhs.d_threadID;
        d_fileName   = rhs.d_fileName;
        d_lineNumber = rhs.d_lineNumber;
        d_category   = rhs.d_category;
        d_severity   = rhs.d_severity;
        d_messageStreamBuf.pubseekpos(0);
        d_messageStreamBuf.sputn(rhs.d_messageStreamBuf.data(),
                                 rhs.d_messageStreamBuf.length());
    }
    return *this;
}

// ACCESSORS
const char *RecordAttributes::message() const
{
    const bsl::size_t length = d_messageStreamBuf.length();
    if (0 == length || '\0' != *(d_messageStreamBuf.data() + length - 1)) {
        // Null terminate the string.

        bdlsb::MemOutStreamBuf& streamBuf =
            const_cast<RecordAttributes *>(this)->d_messageStreamBuf;
        streamBuf.sputc('\0');
        streamBuf.pubseekoff(-1, bsl::ios_base::cur);
    }

    return d_messageStreamBuf.data();
}

bslstl::StringRef RecordAttributes::messageRef() const
{
    const bsl::size_t length = d_messageStreamBuf.length();
    const char *str = d_messageStreamBuf.data();
#if defined(BSLS_PLATFORM_OS_SOLARIS) || defined(BSLS_PLATFORM_OS_SUNOS)
    if (0 == str) {
        // There are some places where observers call
        // 'std::fstream::write(ref.data(), ref.length())' where 'ref'
        // is the value returned from this expression, and at the time of this
        // writing, Solaris has a bug where the fail bit of the stream will be
        // set if '0 == ref.data()'.  This is a workaround to prevent that
        // outcome by substituting a valid non-null pointer for the first
        // argument.

        BSLS_ASSERT(0 == length);
        str = "";
    }
#endif
    const bsl::size_t effectiveLength = (!length || '\0' != str[length - 1])
                                        ? length
                                        : length - 1;
    return bslstl::StringRef(str, effectiveLength);
}

bsl::ostream& RecordAttributes::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    if (0 <= spacesPerLevel) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "[\n";
    }
    else {
        // No newlines if 0 > spacesPerLevel.
        stream << '[';
    }

    if (level < 0) {
        level = -level;
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }

    const int SIZE = 32;
    char buffer[SIZE];

    const int numWritten = d_timestamp.printToBuffer(buffer, SIZE, 3);

    stream.write(buffer, numWritten);

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_processID;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_threadID;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_fileName;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_lineNumber;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_category;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_severity;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    bslstl::StringRef message = messageRef();
    stream.write(message.data(), message.length());

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close package namespace

// FREE OPERATORS
bool ball::operator==(const RecordAttributes& lhs, const RecordAttributes& rhs)
{
    return lhs.d_timestamp  == rhs.d_timestamp
        && lhs.d_processID  == rhs.d_processID
        && lhs.d_threadID   == rhs.d_threadID
        && lhs.d_severity   == rhs.d_severity
        && lhs.d_lineNumber == rhs.d_lineNumber
        && lhs.d_fileName   == rhs.d_fileName
        && lhs.d_category   == rhs.d_category
        && lhs.messageRef() == rhs.messageRef();
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
