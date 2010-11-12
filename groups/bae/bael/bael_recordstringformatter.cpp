// bael_recordstringformatter.cpp                -*-C++-*-
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

#include <bsl_cstring.h>   // for 'bsl::strcmp'
#include <bsl_iomanip.h>
#include <bsl_ostream.h>

namespace {

const char *const DEFAULT_FORMAT_SPEC = "\n%d %p:%t %s %f:%l %c %m %u\n";

}  // close unnamed namespace

namespace BloombergLP {

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
    const char* runBegin = d_formatSpec.data();
    const char* end      = runBegin + d_formatSpec.length();
    const char* iter     = runBegin;

    while (iter != end) {
        switch (*iter) {
          case '%': {
            // Flush run, output fields, and then start a new run (below).
            stream.write(runBegin, iter - runBegin);
            if (++iter == end) {
                break;
            }
            switch (*iter) {
              case '%': {
                stream << '%';
              } break;
              case 'd': {
                // use 'bdet_Datetime' built-in format
                stream << timestamp;
              } break;
              case 'I': // fall through intentionally
              case 'i': {
                // use ISO8601 "extended" format
                stream
                    << bsl::setw(4) << bsl::setfill('0') << timestamp.year()
                    << '-'
                    << bsl::setw(2) << bsl::setfill('0') << timestamp.month()
                    << '-'
                    << bsl::setw(2) << bsl::setfill('0') << timestamp.day()
                    << ' '
                    << bsl::setw(2) << bsl::setfill('0') << timestamp.hour()
                    << ':'
                    << bsl::setw(2) << bsl::setfill('0') << timestamp.minute()
                    << ':'
                    << bsl::setw(2) << bsl::setfill('0') << timestamp.second();

                if ('I' == *iter) {
                    stream << '.' << bsl::setw(3) << bsl::setfill('0')
                           << timestamp.millisecond();
                }

                if (0 == d_timestampOffset.totalMilliseconds()) {
                    stream << 'Z';
                }
              } break;
              case 'p': {
                stream << fixedFields.processID();
              } break;
              case 't': {
                stream << fixedFields.threadID();
              } break;
              case 's': {
                stream << bael_Severity::toAscii(
                                 (bael_Severity::Level)fixedFields.severity());
              } break;
              case 'f': {
                stream << fixedFields.fileName();
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
                    stream << filename;
                }
                else {
                    stream << filename.substr(rightmostSlashIndex + 1);
                }
              } break;
              case 'l': {
                stream << fixedFields.lineNumber();
              } break;
              case 'c': {
                stream << fixedFields.category();
              } break;
              case 'm': {
                stream << fixedFields.message();
              } break;
              case 'x': {
                int length = fixedFields.messageStreamBuf().length();
                bdeu_Print::printString(stream,
                                        fixedFields.message(),
                                        length,
                                        false);
              } break;
              case 'X': {
                int length = fixedFields.messageStreamBuf().length();
                bdeu_Print::singleLineHexDump(stream,
                                              fixedFields.message(),
                                              length);
              } break;
              case 'u': {
                const bdem_List& userFields    = record.userFields();
                const int        numUserFields = userFields.length();
                for (int i = 0; i < numUserFields; ++i) {
                    stream << userFields[i] << " ";
                }
              } break;
              default:
                // Undefined: we just output the verbatim characters.
                stream << '%' << *iter;
            }
            // Start a new run from here.
            runBegin = ++iter;
          } break;
          case '\\': {
            // Flush run, output fields, and then start a new run (below).
            stream.write(runBegin, iter - runBegin);
            if (++iter == end) {
                break;
            }
            switch (*iter) {
              case 'n': {
                stream << '\n';
              } break;
              case 't': {
                stream << '\t';
              } break;
              case '\\': {
                stream << '\\';
              } break;
              default: {
                // Undefined: we just output the verbatim characters.
                stream << '\\' << *iter;
              }
            }
            // Start a new run from here.
            runBegin = ++iter;
          } break;
          default: {
            // Do nothing, instead, let run accumulate.
            ++iter;
          }
        }
    }

    stream.write(runBegin, iter - runBegin);
    stream << bsl::flush;
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
