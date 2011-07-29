// bael_recordstringformatter.cpp                                     -*-C++-*-

///Implementation Notes
///--------------------
// Using the insertion operator (operator <<) with an 'ostream' introduces
// significant performance overhead.  For this reason, the 'operator()' method
// is implemented by writing the formatted string to a buffer before
// inserting to a stream.

#include <bael_recordstringformatter.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_recordstringformatter_cpp,"$Id$ $CSID$")

#include <bael_record.h>
#include <bael_recordattributes.h>
#include <bael_severity.h>

#include <bdem_list.h>

#include <bdet_datetime.h>

#include <bdeu_print.h>

#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_cstring.h>   // for 'bsl::strcmp'
#include <bsl_c_stdlib.h>
#include <bsl_c_stdio.h>   // for snprintf
#include <bsl_iomanip.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>



namespace {

const char *const DEFAULT_FORMAT_SPEC = "\n%d %p:%t %s %f:%l %c %m %u\n";

}  // close unnamed namespace

namespace BloombergLP {

static void appendToString(bsl::string *result, int value)
    // Convert the specified 'value' into ASCII characters and append it to the
    // specified 'result.
{
    char buffer[16];

#if defined(BSLS_PLATFORM__CMP_MSVC)
#define snprintf _snprintf
#endif

    snprintf(buffer, sizeof buffer, "%d", value);

#if defined(BSLS_PLATFORM__CMP_MSVC)
#undef snprintf
#endif

    *result += buffer;
}

static void appendToString(bsl::string *result, bsls_Types::Uint64 value)
    // Convert the specified 'value' into ASCII characters and append it to the
    // specified 'result.
{
    char buffer[32];

#if defined(BSLS_PLATFORM__CMP_MSVC)
#define snprintf _snprintf
#endif

    snprintf(buffer, sizeof(buffer), "%llu", value);

#if defined(BSLS_PLATFORM__CMP_MSVC)
#undef snprintf
#endif

    *result += buffer;
}

                        // --------------------------------
                        // class bael_RecordStringFormatter
                        // --------------------------------

// CREATORS
bael_RecordStringFormatter::bael_RecordStringFormatter(
                                               bslma_Allocator *basicAllocator)
: d_formatSpec(DEFAULT_FORMAT_SPEC, basicAllocator)
, d_timestampOffset(0)
{
}

bael_RecordStringFormatter::bael_RecordStringFormatter(
                                               const char      *format,
                                               bslma_Allocator *basicAllocator)
: d_formatSpec(format, basicAllocator)
, d_timestampOffset(0)
{
}

bael_RecordStringFormatter::bael_RecordStringFormatter(
                                  const bdet_DatetimeInterval&  offset,
                                  bslma_Allocator              *basicAllocator)
: d_formatSpec(DEFAULT_FORMAT_SPEC, basicAllocator)
, d_timestampOffset(offset)
{
}

bael_RecordStringFormatter::bael_RecordStringFormatter(
                                  const char                   *format,
                                  const bdet_DatetimeInterval&  offset,
                                  bslma_Allocator              *basicAllocator)
: d_formatSpec(format, basicAllocator)
, d_timestampOffset(offset)
{
}

bael_RecordStringFormatter::bael_RecordStringFormatter(
                             const bael_RecordStringFormatter&  original,
                             bslma_Allocator                   *basicAllocator)
: d_formatSpec(original.d_formatSpec, basicAllocator)
, d_timestampOffset(original.d_timestampOffset)
{
}

bael_RecordStringFormatter::~bael_RecordStringFormatter()
{
}

// MANIPULATORS
bael_RecordStringFormatter& bael_RecordStringFormatter::operator=(
                                         const bael_RecordStringFormatter& rhs)
{
    if (this != &rhs) {
        d_formatSpec      = rhs.d_formatSpec;
        d_timestampOffset = rhs.d_timestampOffset;
    }

    return *this;
}

// ACCESSORS
void bael_RecordStringFormatter::operator()(bsl::ostream&      stream,
                                            const bael_Record& record) const

{
    const bael_RecordAttributes& fixedFields = record.fixedFields();

    bdet_Datetime timestamp = fixedFields.timestamp() + d_timestampOffset;

    // Step through the format string, outputting the required elements.

    const char* iter = d_formatSpec.data();
    const char* end  = iter + d_formatSpec.length();

    bsl::string output;
    output.reserve(1024);

#if defined(BSLS_PLATFORM__CMP_MSVC)
#define snprintf _snprintf
#endif

    while (iter != end) {
        switch (*iter) {
          case '%': {
            if (++iter == end) {
                break;
            }
            switch (*iter) {
              case '%': {
                output += '%';
              } break;
              case 'd': {
                char buffer[32];
                int length = timestamp.printToBuffer(buffer, sizeof buffer);

                output += buffer;
              } break;
              case 'I': // fall through intentionally
              case 'i': {
                // use ISO8601 "extended" format

                char buffer[32];

                snprintf(buffer,
                         sizeof(buffer),
                         "%04d-%02d-%02d %02d:%02d:%02d",
                         timestamp.year(),
                         timestamp.month(),
                         timestamp.day(),
                         timestamp.hour(),
                         timestamp.minute(),
                         timestamp.second());
                output += buffer;

                if ('I' == *iter) {
                    snprintf(buffer,
                             sizeof(buffer),
                             ".%03d",
                             timestamp.millisecond());

                    output += buffer;
                }

                if (0 == d_timestampOffset.totalMilliseconds()) {
                    output += 'Z';
                }
              } break;
              case 'p': {
                appendToString(&output, fixedFields.processID());
              } break;
              case 't': {
                appendToString(&output, fixedFields.threadID());
              } break;
              case 's': {
                output += bael_Severity::toAscii(
                                 (bael_Severity::Level)fixedFields.severity());
              } break;
              case 'f': {
                output += fixedFields.fileName();
              } break;
              case 'F': {
                const bsl::string& filename = fixedFields.fileName();
                bsl::string::size_type rightmostSlashIndex =
#ifdef BSLS_PLATFORM__OS_WINDOWS
                    filename.rfind('\\');
#else
                    filename.rfind('/');
#endif
                if (bsl::string::npos == rightmostSlashIndex) {
                    output += filename;
                }
                else {
                    output += filename.substr(rightmostSlashIndex + 1);
                }
              } break;
              case 'l': {
                appendToString(&output, fixedFields.lineNumber());
              } break;
              case 'c': {
                output += fixedFields.category();
              } break;
              case 'm': {
                output += fixedFields.message();
              } break;
              case 'x': {
                bsl::stringstream ss;
                int length = fixedFields.messageStreamBuf().length();
                bdeu_Print::printString(ss,
                                        fixedFields.message(),
                                        length,
                                        false);
                output += ss.str();
              } break;
              case 'X': {
                bsl::stringstream ss;
                int length = fixedFields.messageStreamBuf().length();
                bdeu_Print::singleLineHexDump(ss,
                                              fixedFields.message(),
                                              length);
                output += ss.str();
              } break;
              case 'u': {
                const bdem_List& userFields    = record.userFields();
                const int        numUserFields = userFields.length();

                if (numUserFields > 0) {
                    bsl::stringstream ss;
                    for (int i = 0; i < numUserFields; ++i) {
                        ss << userFields[i] << " ";
                    }
                    output += ss.str();
                }
              } break;
              default: {
                // Undefined: we just output the verbatim characters.

                output += '%';
                output += *iter;
              }
            }
            ++iter;
          } break;
          case '\\': {
            if (++iter == end) {
                break;
            }
            switch (*iter) {
              case 'n': {
                output += '\n';
              } break;
              case 't': {
                output += '\t';
              } break;
              case '\\': {
                output += '\\';
              } break;
              default: {
                // Undefined: we just output the verbatim characters.

                output += '\\';
                output += *iter;
              }
            }
            ++iter;
          } break;
          default: {
            output += *iter;
            ++iter;
          }
        }
    }

#if defined(BSLS_PLATFORM__CMP_MSVC)
#undef snprintf
#endif

    stream.write(output.c_str(), output.size());
    stream.flush();

}

// FREE OPERATORS
bool operator==(const bael_RecordStringFormatter& lhs,
                const bael_RecordStringFormatter& rhs)
{
    return 0 == bsl::strcmp(lhs.format(), rhs.format())
        && lhs.timestampOffset() == rhs.timestampOffset();
}

bsl::ostream& operator<<(bsl::ostream&                     output,
                         const bael_RecordStringFormatter& rhs)
{
    return output << "\'" << rhs.format() << "\' " << rhs.timestampOffset();
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
