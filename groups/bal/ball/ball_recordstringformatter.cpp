// ball_recordstringformatter.cpp                                     -*-C++-*-

///Implementation Notes
///--------------------
// Using the insertion operator (operator <<) with an 'ostream' introduces
// significant performance overhead.  For this reason, the 'operator()' method
// is implemented by writing the formatted string to a buffer before
// inserting to a stream.

#include <ball_recordstringformatter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_recordstringformatter_cpp,"$Id$ $CSID$")

#include <ball_record.h>
#include <ball_recordattributes.h>
#include <ball_severity.h>

#include <bdlma_bufferedsequentialallocator.h>
#include <bdlmxxx_list.h>
#include <bdlt_datetime.h>
#include <bdlt_currenttime.h>
#include <bdlb_print.h>

#include <bsls_platform.h>
#include <bsls_types.h>

#include <bslstl_stringref.h>

#include <bsl_climits.h>   // for 'INT_MAX'
#include <bsl_cstring.h>   // for 'bsl::strcmp'
#include <bsl_c_stdlib.h>
#include <bsl_c_stdio.h>   // for 'snprintf'

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

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf
#endif

    snprintf(buffer, sizeof buffer, "%d", value);

#if defined(BSLS_PLATFORM_CMP_MSVC)
#undef snprintf
#endif

    *result += buffer;
}

static void appendToString(bsl::string *result, bsls::Types::Uint64 value)
    // Convert the specified 'value' into ASCII characters and append it to the
    // specified 'result.
{
    char buffer[32];

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf
#endif

    snprintf(buffer, sizeof(buffer), "%llu", value);

#if defined(BSLS_PLATFORM_CMP_MSVC)
#undef snprintf
#endif

    *result += buffer;
}

                        // --------------------------------
                        // class ball::RecordStringFormatter
                        // --------------------------------

// CLASS DATA
const int ball::RecordStringFormatter::k_ENABLE_PUBLISH_IN_LOCALTIME  = INT_MAX;
const int ball::RecordStringFormatter::k_DISABLE_PUBLISH_IN_LOCALTIME = INT_MIN;

namespace ball {    // Local time offsets of 'INT_MAX' *milliseconds* (about 23 days) should
    // not appear in practice.  Real values are (always?) less than one day
    // (plus or minus).

// CREATORS
RecordStringFormatter::RecordStringFormatter(
                                              bslma::Allocator *basicAllocator)
: d_formatSpec(DEFAULT_FORMAT_SPEC, basicAllocator)
, d_timestampOffset(0)
{
}

RecordStringFormatter::RecordStringFormatter(
                                              const char       *format,
                                              bslma::Allocator *basicAllocator)
: d_formatSpec(format, basicAllocator)
, d_timestampOffset(0)
{
}

RecordStringFormatter::RecordStringFormatter(
                                  const bdlt::DatetimeInterval&  offset,
                                  bslma::Allocator             *basicAllocator)
: d_formatSpec(DEFAULT_FORMAT_SPEC, basicAllocator)
, d_timestampOffset(offset)
{
}

RecordStringFormatter::RecordStringFormatter(
                                          bool              publishInLocalTime,
                                          bslma::Allocator *basicAllocator)
: d_formatSpec(DEFAULT_FORMAT_SPEC, basicAllocator)
, d_timestampOffset(0,
                    0,
                    0,
                    0,
                    publishInLocalTime
                    ?  k_ENABLE_PUBLISH_IN_LOCALTIME
                    : k_DISABLE_PUBLISH_IN_LOCALTIME)
{
}

RecordStringFormatter::RecordStringFormatter(
                                  const char                   *format,
                                  const bdlt::DatetimeInterval&  offset,
                                  bslma::Allocator             *basicAllocator)
: d_formatSpec(format, basicAllocator)
, d_timestampOffset(offset)
{
}

RecordStringFormatter::RecordStringFormatter(
                                          const char       *format,
                                          bool              publishInLocalTime,
                                          bslma::Allocator *basicAllocator)
: d_formatSpec(format, basicAllocator)
, d_timestampOffset(0,
                    0,
                    0,
                    0,
                    publishInLocalTime
                    ?  k_ENABLE_PUBLISH_IN_LOCALTIME
                    : k_DISABLE_PUBLISH_IN_LOCALTIME)
{
}

RecordStringFormatter::RecordStringFormatter(
                             const RecordStringFormatter&  original,
                             bslma::Allocator                  *basicAllocator)
: d_formatSpec(original.d_formatSpec, basicAllocator)
, d_timestampOffset(original.d_timestampOffset)
{
}

RecordStringFormatter::~RecordStringFormatter()
{
}

// MANIPULATORS
RecordStringFormatter& RecordStringFormatter::operator=(
                                         const RecordStringFormatter& rhs)
{
    if (this != &rhs) {
        d_formatSpec      = rhs.d_formatSpec;
        d_timestampOffset = rhs.d_timestampOffset;
    }

    return *this;
}

// ACCESSORS
void RecordStringFormatter::operator()(bsl::ostream&      stream,
                                            const Record& record) const

{
    const RecordAttributes& fixedFields = record.fixedFields();
    bdlt::Datetime                timestamp   = fixedFields.timestamp();

    if (k_ENABLE_PUBLISH_IN_LOCALTIME ==
                                       d_timestampOffset.totalMilliseconds()) {
        int localTimeOffsetInSeconds =
            bdlt::LocalTimeOffset::localTimeOffset(timestamp).totalSeconds();
        timestamp.addSeconds(localTimeOffsetInSeconds);
    } else if(k_DISABLE_PUBLISH_IN_LOCALTIME ==
                                       d_timestampOffset.totalMilliseconds()) {
        // Do not adjust 'timestamp'.
    } else {
        timestamp += d_timestampOffset;
    }

    // Step through the format string, outputting the required elements.

    const char* iter = d_formatSpec.data();
    const char* end  = iter + d_formatSpec.length();

    // Create a buffer on the stack for formatting the record.  Note that the
    // size of the buffer should be slightly larger than the amount we reserve
    // in order to ensure only a single allocation occurs.

    const int BUFFER_SIZE        = 512;
    const int STRING_RESERVATION = BUFFER_SIZE -
                                   bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

    char fixedBuffer[BUFFER_SIZE];
    bdlma::BufferedSequentialAllocator stringAllocator(fixedBuffer,
                                                      BUFFER_SIZE);
    bsl::string output(&stringAllocator);
    output.reserve(STRING_RESERVATION);

#if defined(BSLS_PLATFORM_CMP_MSVC)
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
                timestamp.printToBuffer(buffer, sizeof buffer);

                output += buffer;
              } break;
              case 'I': // fall through intentionally
              case 'i': {
                // use ISO8601 "extended" format

                char buffer[32];

                snprintf(buffer,
                         sizeof(buffer),
                         "%04d-%02d-%02dT%02d:%02d:%02d",
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
                output += Severity::toAscii(
                                 (Severity::Level)fixedFields.severity());
              } break;
              case 'f': {
                output += fixedFields.fileName();
              } break;
              case 'F': {
                const bsl::string& filename = fixedFields.fileName();
                bsl::string::size_type rightmostSlashIndex =
#ifdef BSLS_PLATFORM_OS_WINDOWS
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
                bslstl::StringRef message = fixedFields.messageRef();
                output.append(message.data(), message.length());
              } break;
              case 'x': {
                bsl::stringstream ss;
                int length = fixedFields.messageStreamBuf().length();
                bdlb::Print::printString(ss,
                                        fixedFields.message(),
                                        length,
                                        false);
                output += ss.str();
              } break;
              case 'X': {
                bsl::stringstream ss;
                int length = fixedFields.messageStreamBuf().length();
                bdlb::Print::singleLineHexDump(ss,
                                              fixedFields.message(),
                                              length);
                output += ss.str();
              } break;
              case 'u': {
                const bdlmxxx::List& userFields    = record.userFields();
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

#if defined(BSLS_PLATFORM_CMP_MSVC)
#undef snprintf
#endif

    stream.write(output.c_str(), output.size());
    stream.flush();

}
}  // close package namespace

// FREE OPERATORS
bool ball::operator==(const RecordStringFormatter& lhs,
                const RecordStringFormatter& rhs)
{
    return 0 == bsl::strcmp(lhs.format(), rhs.format())
        && lhs.timestampOffset() == rhs.timestampOffset();
}

bsl::ostream& ball::operator<<(bsl::ostream&                     output,
                         const RecordStringFormatter& rhs)
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
