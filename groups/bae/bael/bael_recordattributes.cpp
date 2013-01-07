// bael_recordattributes.cpp                                          -*-C++-*-
#include <bael_recordattributes.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_recordattributes_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>
#include <bslma_default.h>

#include <bsl_cstring.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                        // ---------------------------
                        // class bael_RecordAttributes
                        // ---------------------------

// CREATORS
bael_RecordAttributes::bael_RecordAttributes(bslma_Allocator *basicAllocator)
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

bael_RecordAttributes::bael_RecordAttributes(
                                     const bdet_Datetime&       timestamp,
                                     int                        processID,
                                     bsls_PlatformUtil::Uint64  threadID,
                                     const char                *fileName,
                                     int                        lineNumber,
                                     const char                *category,
                                     int                        severity,
                                     const char                *message,
                                     bslma_Allocator           *basicAllocator)
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

bael_RecordAttributes::bael_RecordAttributes(
                                  const bael_RecordAttributes& original,
                                  bslma_Allocator              *basicAllocator)
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
void bael_RecordAttributes::setMessage(const char *message)
{
    d_messageStreamBuf.pubseekpos(0);
    while (*message) {
        d_messageStreamBuf.sputc(*message);
        ++message;
    }
}

bael_RecordAttributes& bael_RecordAttributes::operator=(
                                              const bael_RecordAttributes& rhs)
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
const char *bael_RecordAttributes::message() const
{
    const int length = d_messageStreamBuf.length();
    if (0 == length || '\0' != *(d_messageStreamBuf.data() + length - 1)) {
        // Null terminate the string.

        bdesb_MemOutStreamBuf& streamBuf =
            const_cast<bael_RecordAttributes *>(this)->d_messageStreamBuf;
        streamBuf.sputc('\0');
        streamBuf.pubseekoff(-1, bsl::ios_base::cur);
    }

    return d_messageStreamBuf.data();
}

bslstl_StringRef bael_RecordAttributes::messageRef() const
{
    int length = d_messageStreamBuf.length();
    const char *str = d_messageStreamBuf.data();
    return bslstl_StringRef(str,
                            (!length || '\0' != str[length - 1]) ? length
                                                                 : length - 1);
}

bsl::ostream& bael_RecordAttributes::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    if (0 <= spacesPerLevel) {
        bdeu_Print::indent(stream, level, spacesPerLevel);
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
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_timestamp;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_processID;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_threadID;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_fileName;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_lineNumber;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_category;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_severity;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    bslstl_StringRef message = messageRef();
    stream.write(message.data(), message.length());

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        stream << " ]";
    }

    return stream << bsl::flush;
}

// FREE OPERATORS
bool operator==(const bael_RecordAttributes& lhs,
                const bael_RecordAttributes& rhs)
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

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
