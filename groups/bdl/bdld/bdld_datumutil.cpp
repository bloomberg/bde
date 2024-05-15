// bdld_datumutil.cpp                                                 -*-C++-*-
#include <bdld_datumutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(RCSid_bdld_datumutil_cpp, "$Id$ $CSID$")

#include <bdld_datum.h>

#include <bsla_fallthrough.h>

#include <bdlb_literalutil.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>

#include <bdlma_localbufferedobject.h>

#include <bdlsb_memoutstreambuf.h>

#include <bdlt_iso8601util.h>
#include <bdlt_iso8601utilconfiguration.h>

#include <bsl_algorithm.h>
#include <bsl_limits.h>
#include <bsl_string_view.h>
#include <bsl_unordered_set.h>

namespace BloombergLP {
namespace bdld {

// ============================================================================
//                             Utility Functions
// ----------------------------------------------------------------------------

namespace {

static
void safeTypedPrintIntMapEntry(
                             bsl::ostream&                     outputStream,
                             bsl::unordered_set<const void *>& seenSet,
                             const DatumIntMapEntry&           intMapEntry,
                             int                               level,
                             int                               spacesPerLevel);

static
void safeTypedPrintMapEntry(bsl::ostream&                     outputStream,
                            bsl::unordered_set<const void *>& seenSet,
                            const DatumMapEntry&              mapEntry,
                            int                               level,
                            int                               spacesPerLevel);

static
void safeTypedPrintImpl(bsl::ostream&                     outputStream,
                        bsl::unordered_set<const void *>& seenSet,
                        const Datum&                      object,
                        int                               level,
                        int                               spacesPerLevel)
{
    if (outputStream.bad()) {
        return;                                                       // RETURN
    }

    const void *ptr = 0;
    bsl::string_view cycleOutput1;
    bsl::string_view cycleOutput2;
    switch (object.type()) {
      case Datum::e_ARRAY: {
        ptr = object.theArray().data();
        cycleOutput1 = "<array@";
        cycleOutput2 = "[!CYCLE!]>";
      } break;

      case Datum::e_INT_MAP: {
        ptr = object.theIntMap().data();
        cycleOutput1 = "<intmap@";
        cycleOutput2 = "{!CYCLE!}>";
      } break;

      case Datum::e_MAP: {
        ptr = object.theMap().data();
        cycleOutput1 = "<map@";
        cycleOutput2 = "{!CYCLE!}>";
      } break;

      default: {
        // Nothing to do for other types
      } break;
    }

    if (ptr != 0) {
        if (!seenSet.insert(ptr).second) {  // Have seen this already
            if (level >= 0) {
                bdlb::Print::indent(outputStream, level, spacesPerLevel);
            }
            else {
                level = -level;
            }
            outputStream << cycleOutput1 << ptr << cycleOutput2;
            if (0 <= spacesPerLevel) {  // Multi-line output.
                outputStream << '\n';
            }
            return;                                                   // RETURN
        }
    }
    else {
        // These types are scalar, do not refer to anything
        DatumUtil::typedPrint(outputStream, object, level, spacesPerLevel);
        return;                                                       // RETURN
    }

    // When here, we need to print the "dangerous" types that may form cycles.
    // We need to fully implement the printing of those using functions that
    // are aware of already visited nodes.

    if (level >= 0) {
        bdlb::Print::indent(outputStream, level, spacesPerLevel);
    }
    else {
        level = -level;
    }

    switch (object.type()) {
      case Datum::e_ARRAY: {
        const DatumArrayRef aRef = object.theArray();
        outputStream << "<array@" << ptr << '[';
        if (0 <= spacesPerLevel) {
            // Multi-line output.
            outputStream << '\n';

            const int nextLevel = level + 1;
            for (bsl::size_t i = 0; i < aRef.length(); ++i) {
                safeTypedPrintImpl(outputStream,
                                   seenSet,
                                   aRef[i],
                                   nextLevel,
                                   spacesPerLevel);
            }

            bdlb::Print::indent(outputStream, level, spacesPerLevel);
        }
        else {
            // Single-line output.
            outputStream << ' ';

            // Output on a single line and suppress any further indentation.
            for (bsl::size_t i = 0; i < aRef.length(); ++i) {
                if (i > 0) {
                    outputStream << ", ";
                }
                safeTypedPrintImpl(outputStream, seenSet, aRef[i], 0, -1);
            }
            outputStream << ' ';
        }
        outputStream << "]>";
      } break;

      case Datum::e_INT_MAP: {
        const DatumIntMapRef imRef = object.theIntMap();
        outputStream << "<intmap@" << ptr << '{';
        if (0 <= spacesPerLevel) {
            // Multi-line output.
            outputStream << '\n';

            const int nextLevel = level + 1;
            for (bsl::size_t i = 0; i < imRef.size(); ++i) {
                safeTypedPrintIntMapEntry(outputStream,
                                          seenSet,
                                          imRef[i],
                                          nextLevel,
                                          spacesPerLevel);
            }

            bdlb::Print::indent(outputStream, level, spacesPerLevel);
        }
        else {
            // Single-line output.
            outputStream << ' ';

            // Output on a single line and suppress any further indentation.
            for (bsl::size_t i = 0; i < imRef.size(); ++i) {
                if (i > 0) {
                    outputStream << ", ";
                }
                safeTypedPrintIntMapEntry(outputStream,
                                          seenSet,
                                          imRef[i],
                                          0,
                                          -1);
            }
            outputStream << ' ';
        }
        outputStream << "}>";
      } break;

      case Datum::e_MAP: {
        const DatumMapRef mRef = object.theMap();
        outputStream << "<map@" << ptr << '{';
        if (0 <= spacesPerLevel) {
            // Multi-line output.
            outputStream << '\n';

            const int nextLevel = level + 1;
            for (bsl::size_t i = 0; i < mRef.size(); ++i) {
                safeTypedPrintMapEntry(outputStream,
                                       seenSet,
                                       mRef[i],
                                       nextLevel,
                                       spacesPerLevel);
            }

            bdlb::Print::indent(outputStream, level, spacesPerLevel);
        }
        else {
            // Single-line output.
            outputStream << ' ';

            // Output on a single line and suppress any further indentation.
            for (bsl::size_t i = 0; i < mRef.size(); ++i) {
                if (i > 0) {
                    outputStream << ", ";
                }
                safeTypedPrintMapEntry(outputStream, seenSet, mRef[i], 0, -1);
            }
            outputStream << ' ';
        }
        outputStream << "}>";
      } break;

      default: {
          // Nothing to do for other types
      } break;
    }

    if (0 <= spacesPerLevel) {
        // Multi-line output.
        outputStream << '\n';
    }

    seenSet.erase(ptr);
}

static
void dumpInterval(bsl::ostream&                 outputStream,
                  const bdlt::DatetimeInterval& dtInterval)
{
    bsl::string_view padding;

    if (dtInterval == bdlt::DatetimeInterval(0)) {  // Zero duration
        outputStream << "0s";
        return;                                                       // RETURN
    }

    if (dtInterval.days() > 0) {
        outputStream << padding << dtInterval.days() << 'd';
        padding = " ";
    }

    if (dtInterval.hours() > 0) {
        outputStream << padding << dtInterval.hours() << 'h';
        padding = " ";
    }

    if (dtInterval.minutes() > 0) {
        outputStream << padding << dtInterval.minutes() << 'm';
        padding = " ";
    }

    if (dtInterval.seconds() > 0) {
        outputStream << padding << dtInterval.seconds();
        padding = " ";
    }

    const int usec = dtInterval.milliseconds() * 1000 +
                                                     dtInterval.microseconds();

    if (usec > 0) {
        if (dtInterval.seconds() == 0) {
            outputStream << padding << "0";
        }
        outputStream << '.';
        const char old = outputStream.fill('0');
        outputStream.width(6);
        outputStream << usec;
        outputStream.fill(old);
    }

    if (dtInterval.seconds() > 0 || usec > 0) {
        outputStream << 's';
    }
}

static
void typedPrintIntMapEntry(bsl::ostream&           outputStream,
                           const DatumIntMapEntry& intMapEntry,
                           int                     level,
                           int                     spacesPerLevel)
{
    if (level >= 0) {
        bdlb::Print::indent(outputStream, level, spacesPerLevel);
    }
    else {
        level = -level;
    }

    outputStream << intMapEntry.key() << ": ";
    bdld::DatumUtil::typedPrint(outputStream,
                                intMapEntry.value(),
                                -level,
                                spacesPerLevel);
}

static
void safeTypedPrintIntMapEntry(
                              bsl::ostream&                     outputStream,
                              bsl::unordered_set<const void *>& seenSet,
                              const DatumIntMapEntry&           intMapEntry,
                              int                               level,
                              int                               spacesPerLevel)
{
    if (level >= 0) {
        bdlb::Print::indent(outputStream, level, spacesPerLevel);
    }
    else {
        level = -level;
    }

    outputStream << intMapEntry.key() << ": ";
    safeTypedPrintImpl(outputStream,
                       seenSet,
                       intMapEntry.value(),
                       -level,
                       spacesPerLevel);
}

static
void typedPrintMapEntry(bsl::ostream&        outputStream,
                        const DatumMapEntry& mapEntry,
                        int                  level,
                        int                  spacesPerLevel)
{
    if (level >= 0) {
        bdlb::Print::indent(outputStream, level, spacesPerLevel);
    }
    else {
        level = -level;
    }

    {
        using bdlb::LiteralUtil;

        bdlma::LocalBufferedObject<bsl::string, 64> toPrint;
        LiteralUtil::createQuotedEscapedCString(&*toPrint, mapEntry.key());
        outputStream << *toPrint << ": ";
    }
    bdld::DatumUtil::typedPrint(outputStream,
                                mapEntry.value(),
                                -level,
                                spacesPerLevel);
}

static
void safeTypedPrintMapEntry(bsl::ostream&                     outputStream,
                            bsl::unordered_set<const void *>& seenSet,
                            const DatumMapEntry&              mapEntry,
                            int                               level,
                            int                               spacesPerLevel)
{
    if (level >= 0) {
        bdlb::Print::indent(outputStream, level, spacesPerLevel);
    }
    else {
        level = -level;
    }

    {
        using bdlb::LiteralUtil;

        bdlma::LocalBufferedObject<bsl::string, 64> toPrint;
        LiteralUtil::createQuotedEscapedCString(&*toPrint, mapEntry.key());
        outputStream << *toPrint << ": ";
    }
    safeTypedPrintImpl(outputStream,
                       seenSet,
                       mapEntry.value(),
                       -level,
                       spacesPerLevel);
}

}  // close unnamed namespace

                            // ----------------
                            // struct DatumUtil
                            // ----------------

bsl::ostream& DatumUtil::safeTypedPrint(bsl::ostream& outputStream,
                                        const Datum&  object,
                                        int           level,
                                        int           spacesPerLevel)
{
    if (outputStream.bad()) {
        return outputStream;                                          // RETURN
    }

    bsl::unordered_set<const void *> seenSet;

    safeTypedPrintImpl(outputStream, seenSet, object, level, spacesPerLevel);

    return outputStream;
}

bsl::ostream& DatumUtil::typedPrint(bsl::ostream& outputStream,
                                    const Datum&  object,
                                    int           level,
                                    int           spacesPerLevel)
{
    if (outputStream.bad()) {
        return outputStream;                                          // RETURN
    }

    if (level >= 0) {
        bdlb::Print::indent(outputStream, level, spacesPerLevel);
    }
    else {
        level = -level;
    }

    switch (object.type()) {
      case Datum::e_NIL: {
        outputStream << "<nil>";
      } break;

      case Datum::e_BOOLEAN: {
          outputStream << (object.theBoolean() ? "<true>":"<false>");
      } break;

      case Datum::e_ERROR: {
          outputStream << '<' << object.theError() << '>';
      } break;

      case Datum::e_DOUBLE: {
        bdlsb::MemOutStreamBuf sb;
        bsl::ostream os(&sb);
        os.precision(bsl::numeric_limits<double>::digits10);
        os << object.theDouble();

        // Write the "core" value
        outputStream.write(sb.data(), sb.length());

        // Add a '.' if there wasn't one
        const bsl::string_view sv(sb.data(), sb.length());
        if (sv.find('.') == bsl::string_view::npos) {
            outputStream << '.';
        }
      } break;

      case Datum::e_DECIMAL64: {
        bdlsb::MemOutStreamBuf sb;
        bsl::ostream os(&sb);
        os << object.theDecimal64();

        // Write the "core" value
        outputStream.write(sb.data(), sb.length());

        // Add '.' if there wasn't one
        const bsl::string_view sv(sb.data(), sb.length());
        if (sv.find('.') == bsl::string_view::npos) {
            outputStream << '.';
        }
        // Add the decimal suffix unconditionally
        outputStream << 'd';
      } break;

      case Datum::e_INTEGER: {
        outputStream << object.theInteger() << 'i';
      } break;

      case Datum::e_INTEGER64: {
          outputStream << object.theInteger64() << 'L';
      } break;

      case Datum::e_DATE: {
        using bdlt::Iso8601Util;
        static const bsl::size_t k_BUFLEN = Iso8601Util::k_DATE_STRLEN + 1;
        char buffer[k_BUFLEN];
        Iso8601Util::generate(buffer, k_BUFLEN, object.theDate());
        outputStream << "<date(" << buffer << ")>";
      } break;

      case Datum::e_TIME: {
        using bdlt::Iso8601Util;
        using bdlt::Iso8601UtilConfiguration;
        static const bsl::size_t k_BUFLEN = Iso8601Util::k_TIME_STRLEN + 1;
        char buffer[k_BUFLEN];
        Iso8601UtilConfiguration cfg;
        cfg.setFractionalSecondPrecision(6);
        Iso8601Util::generate(buffer, k_BUFLEN, object.theTime(), cfg);
        outputStream << "<time(" << buffer << ")>";
      } break;

      case Datum::e_DATETIME: {
        using bdlt::Iso8601Util;
        using bdlt::Iso8601UtilConfiguration;
        static const bsl::size_t k_BUFLEN = Iso8601Util::k_DATETIME_STRLEN + 1;
        char buffer[k_BUFLEN];
        Iso8601UtilConfiguration cfg;
        cfg.setFractionalSecondPrecision(6);
        Iso8601Util::generate(buffer, k_BUFLEN, object.theDatetime(), cfg);

        // Replace 'T' with ' ', because we do not have time zone info

        char *p = bsl::find(buffer, buffer + k_BUFLEN, 'T');
        if (p != buffer + k_BUFLEN) {
            *p = ' ';
        }
        outputStream << "<datetime(" << buffer << ")>";
      } break;

      case Datum::e_DATETIME_INTERVAL: {
        outputStream << "<datetime-interval(";
        dumpInterval(outputStream, object.theDatetimeInterval());
        outputStream << ")>";
      } break;

      case Datum::e_STRING: {
        using bdlb::LiteralUtil;

        bdlma::LocalBufferedObject<bsl::string, 64> toPrint;
        LiteralUtil::createQuotedEscapedCString(&*toPrint, object.theString());
        outputStream << *toPrint;
      } break;

      case Datum::e_USERDEFINED: {
        const DatumUdt udt = object.theUdt();
        outputStream << "<udt(type:" << udt.type() << ", ptr:" << udt.data()
                     << ")>";
      } break;

      case Datum::e_BINARY: {
          const DatumBinaryRef binRef = object.theBinary();
          typedef bdlb::PrintStringSingleLineHexDumper HexWrap;
          const HexWrap asHex(static_cast<const char*>(binRef.data()),
                              static_cast<int>(binRef.size()));
          outputStream << "<binary(" << asHex << ")>";
      } break;

      case Datum::e_ARRAY: {
        const DatumArrayRef aRef = object.theArray();
        outputStream << "<array[";
        if (0 <= spacesPerLevel) {
            // Multi-line output.
            outputStream << '\n';

            const int nextLevel = level + 1;
            for (bsl::size_t i = 0; i < aRef.length(); ++i) {
                typedPrint(outputStream, aRef[i], nextLevel, spacesPerLevel);
            }

            bdlb::Print::indent(outputStream, level, spacesPerLevel);
        }
        else {
            // Single-line output.
            outputStream << ' ';

            // Output on a single line and suppress any further indentation.
            for (bsl::size_t i = 0; i < aRef.length(); ++i) {
                if (i > 0) {
                    outputStream << ", ";
                }
                typedPrint(outputStream, aRef[i], 0, -1);
            }
            outputStream << ' ';
        }
        outputStream << "]>";
      } break;

      case Datum::e_INT_MAP: {
        const DatumIntMapRef imRef = object.theIntMap();
        outputStream << "<intmap{";
        if (0 <= spacesPerLevel) {
            // Multi-line output.
            outputStream << '\n';

            const int nextLevel = level + 1;
            for (bsl::size_t i = 0; i < imRef.size(); ++i) {
                typedPrintIntMapEntry(outputStream,
                                      imRef[i],
                                      nextLevel,
                                      spacesPerLevel);
            }

            bdlb::Print::indent(outputStream, level, spacesPerLevel);
        }
        else {
            // Single-line output.
            outputStream << ' ';

            // Output on a single line and suppress any further indentation.
            for (bsl::size_t i = 0; i < imRef.size(); ++i) {
                if (i > 0) {
                    outputStream << ", ";
                }
                typedPrintIntMapEntry(outputStream, imRef[i], 0, -1);
            }
            outputStream << ' ';
        }
        outputStream << "}>";
      } break;

      case Datum::e_MAP: {
        const DatumMapRef mRef = object.theMap();
        outputStream << "<map{";
        if (0 <= spacesPerLevel) {
            // Multi-line output.
            outputStream << '\n';

            const int nextLevel = level + 1;
            for (bsl::size_t i = 0; i < mRef.size(); ++i) {
                typedPrintMapEntry(outputStream,
                                   mRef[i],
                                   nextLevel,
                                   spacesPerLevel);
            }

            bdlb::Print::indent(outputStream, level, spacesPerLevel);
        }
        else {
            // Single-line output.
            outputStream << ' ';

            // Output on a single line and suppress any further indentation.
            for (bsl::size_t i = 0; i < mRef.size(); ++i) {
                if (i > 0) {
                    outputStream << ", ";
                }
                typedPrintMapEntry(outputStream, mRef[i], 0, -1);
            }
            outputStream << ' ';
        }
        outputStream << "}>";
      } break;
    }

    if (0 <= spacesPerLevel) {
        // Multi-line output.
        outputStream << '\n';
    }

    return outputStream;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
