// ball_recordstringformatter.cpp                                     -*-C++-*-
#include <ball_recordstringformatter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_recordstringformatter_cpp,"$Id$ $CSID$")

///Implementation Notes
///--------------------
// Using the insertion operator ('operator<<') with an 'ostream' introduces
// significant performance overhead.  For this reason, the 'operator()' method
// is implemented by writing the formatted string to a buffer before inserting
// to a stream.

#include <ball_managedattribute.h>
#include <ball_record.h>
#include <ball_recordattributes.h>
#include <ball_severity.h>
#include <ball_userfields.h>
#include <ball_userfieldvalue.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bdlma_bufferedsequentialallocator.h>

#include <bdls_pathutil.h>

#include <bdlsb_fixedmemoutstreambuf.h>

#include <bdlt_datetime.h>
#include <bdlt_currenttime.h>
#include <bdlt_localtimeoffset.h>
#include <bdlt_iso8601util.h>
#include <bdlt_iso8601utilconfiguration.h>

#include <bdlsb_overflowmemoutstreambuf.h>

#include <bslim_printer.h>

#include <bsls_annotation.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_climits.h>   // for 'INT_MAX'
#include <bsl_cstdio.h>    // for 'bsl::sprintf'
#include <bsl_cstring.h>   // for 'bsl::strcmp'
#include <bsl_c_stdlib.h>
#include <bsl_c_stdio.h>   // for 'snprintf'

#include <bsl_algorithm.h>
#include <bsl_iomanip.h>
#include <bsl_ostream.h>
#include <bsl_utility.h>

namespace {

const char *const DEFAULT_FORMAT_SPEC = "\n%d %p:%t %s %f:%l %c %m %u\n";

}  // close unnamed namespace

namespace BloombergLP {
namespace ball {
namespace {

                       // ============================
                       // class PublishInLocalTimeUtil
                       // ============================

struct PublishInLocalTimeUtil {
    // This "struct" provides a namespace for an enumeration that defines
    // constants to control the publication of a datetime in local time.

    // TYPES
    enum {
        // Enumeration used to enable or disable publishing in local time.


        k_DISABLE = INT_MIN,  // Reserved offset (a value corresponding to no
                              // known time zone) that indicates that the
                              // record time stamp (in UTC) is *not* adjusted
                              // to the current local time.

        k_ENABLE  = INT_MAX   // Reserved offset (a value corresponding to no
                              // known time zone) that indicates that the
                              // record time stamp (in UTC) is adjusted to the
                              // current local time.  Local time offsets of
                              // 'INT_MAX' *milliseconds* (about 23 days)
                              // should not appear in practice.  Real values
                              // are (always?) less than one day (plus or
                              // minus).
    };
};

                       // ===============
                       // class PrintUtil
                       // ===============

struct PrintUtil {
    // This "struct" provides a namespace for utility functions that render
    // values of various types to string.

    // TYPES
    enum FractionalSecondPrecision {
        // Enumeration used to distinguish among different fractional second
        // precision.
        e_FSP_NONE         = 0,
        e_FSP_MILLISECONDS = 3,
        e_FSP_MICROSECONDS = 6
    };

    enum TimestampFormat {
        // Enumeration used to distinguish among different formats of
        // timestamp representation.
        e_TF_DATETIME           = 0,
        e_TF_DATETIME_TZ_OFFSET = 1,
        e_TF_ISO8601            = 2
    };

    // CLASS METHODS
    static void appendAttribute(bsl::string             *result,
                                const ManagedAttribute&  attribute,
                                bool                     printKey = true);
        // Append the key of the specified 'attribute', followed by the value
        // of 'attribute' to the specified 'result' string.  Optionally specify
        // 'printKey' to indicate whether the key should be appended or
        // omitted.  If 'printKey' is not supplied the key will be appended.
        // Note that this method is invoked when processing "%a", "%a[key]" or
        // "%A" specifiers.

    static void appendCategory(bsl::string *result, const Record& record);
        // Append a category provided by the specified 'record' to the
        // specified 'result' string.  Note that this method is invoked when
        // processing "%c" specifier.

    static void appendDatetime(bsl::string                  *result,
                               const Record&                 record,
                               const bdlt::DatetimeInterval *timestampOffset,
                               TimestampFormat               timestampFormat,
                               FractionalSecondPrecision     secondPrecision);
        // Append to the specified 'result' the datetime provided by the
        // specified 'record' in the specified 'timestampFormat', having the
        // specified fractional 'secondPrecision' numbers, and the specified
        // 'timestampOffset'.  Note that this method is invoked when processing
        // "%d", "%D", "%dtz", "%Dtz", "%i", "%I" or  "%O" specifiers.

    static void appendFilename(bsl::string   *result,
                               bool           fullPath,
                               const Record&  record);
        // Append a path to a file-name provided by the specified 'record' to
        // the specified 'result' string if the specified 'fullPath' is true,
        // and a base-name only otherwise.  Note that this method is invoked
        // when processing "%f" or "%F" specifiers.

    static void appendHexDump(bsl::string             *result,
                              const bsl::string_view&  string);
        // Append to the specified 'result' string the uppercase hex encoding
        // of the byte sequence defined by the specified 'string'.

    static void appendLineNumber(bsl::string *result, const Record& record);
        // Append to the specified 'result' a line-number provided by the
        // specified 'record'.  Note that this method is invoked when
        // processing "%l" specifier.

    static void appendMessage(bsl::string *result, const Record& record);
        // Append a message provided by the specified 'record' to the specified
        // 'result' string.  Note that this method is invoked when processing
        // "%m" specifier.

    static void appendMessageNonPrintableChars(bsl::string   *result,
                                               const Record&  record);
        // Append a message with non-printable characters in hex provided by
        // the specified 'record' to the specified 'result' string.  Note that
        // this method is invoked when processing "%x" specifier.

    static void appendMessageAsHex(bsl::string *result, const Record& record);
        // Append a message provided by the specified 'record' to the specified
        // 'result' string in hex format.  Note that this method is invoked
        // when processing "%X" specifier.

    static void appendProcessId(bsl::string *result, const Record& record);
        // Append a process ID provided by the specified 'record' to the
        // specified 'result' string.  Note that this method is invoked when
        // processing "%p" specifier.

    static void appendString(bsl::string             *result,
                             const bsl::string_view&  value,
                             bool                     notPrintable = false);
        // Append the specified 'value' to the specified 'result' string.  If
        // the optionally specified 'notPrintable' flag is 'true', then all
        // non-printable characters in 'value' will be printed in their
        // hexadecimal representation ('\xHH').

    static void appendThreadId(bsl::string *result, const Record& record);
        // Append a thread ID provided by the specified 'record' to the
        // specified 'result' string.  Note that this method is invoked when
        // processing "%t" specifier.

    static void appendThreadIdAsHex(bsl::string *result, const Record& record);
        // Append a thread ID provided by the specified 'record' to the
        // specified 'result' string in hex format.  Note that this method is
        // invoked when processing "%T" specifier.

    static void appendSeverity(bsl::string *result, const Record& record);
        // Append a severity provided by the specified 'record' to the
        // specified 'result' string.  Note that this method is invoked when
        // processing "%s" specifier.

    template <class T>
    static void appendValue(bsl::string  *result,
                            const char   *format,
                            T             value);
        // Append the specified 'value' to the specified 'result' string
        // according to the specified 'format'.

    static void appendValue(bsl::string  *result, int                value);
    static void appendValue(bsl::string  *result, long               value);
    static void appendValue(bsl::string  *result, long long          value);
    static void appendValue(bsl::string  *result, unsigned int       value);
    static void appendValue(bsl::string  *result, unsigned long      value);
    static void appendValue(bsl::string  *result, unsigned long long value);
        // Append the specified 'value' to the specified 'result' string.

    static void appendUserFields(bsl::string *result, const Record& record);
        // Append user fields provided by the specified 'record' to the
        // specified 'result' string.  Note that this method is invoked when
        // processing "%u" specifier.
};

                       // ========================
                       // class AttributeFormatter
                       // ========================

class AttributeFormatter {
    // This class implements a functional object that renders an attribute
    // string.

    // PRIVATE TYPES
    typedef bsl::vector<ball::ManagedAttribute> Attributes;
        // 'Attributes' is an alias for the vector of 'ball::ManagedAttribute'
        // objects.

    enum { k_UNSET = -1 };  // Unspecified index

    // DATA
    const bsl::string_view d_key;        // attribute's key
    int                    d_index;      // cached attribute's index
    bool                   d_renderKey;  // print "key=" before value

  public:
    // CREATORS
    explicit
    AttributeFormatter(const bsl::string_view& key, bool renderKey = true);
        // Create an attribute formatter object having the specified 'key' of
        // an attribute to be rendered.  If optionally specified 'renderKey' is
        // 'true', render attribute as "key=value", and render only the value
        // of the attribute otherwize.

    // MANIPULATORS
    void operator()(bsl::string *result, const Record& record);
        // Render an attribute having the key supplied at construction of this
        // object and provided by the specified 'record' to the specified
        //'result' string.
};

                       // =========================
                       // class AttributesFormatter
                       // =========================

class AttributesFormatter {
    // This class implements a functional object that renders a collection of
    // attribute values to string.

    // PRIVATE TYPES
    typedef bsl::vector<ball::ManagedAttribute>        Attributes;
        // 'Attributes' is an alias for the vector of 'ball::ManagedAttribute'
        // objects.

    typedef bsl::vector<bsl::pair<bsl::string, bool> > AttributeCache;
        // 'AttributeCache' is an alias for a vector of pairs of an attribute's
        // key and a flag indicating whether the attribute should be displayed
        // or not.

    typedef bsl::set<bsl::string_view>                 SkipAttributes;
        // 'SkipAttributes' is an alias for a set of keys of attributes that
        // should not be printed as part of '%a' format specifier.

    // DATA
    const SkipAttributes *d_skipAttributes_p;  // collection of keys of skipped
                                               // attributes (held, not owned)

    AttributeCache        d_cache;             // cached attributes

    // PRIVATE MANIPULATORS
    void renderAllAttributes(bsl::string *result, const Record& record);
        // Render all attribute provided by the specified 'record' to the
        // specified 'result' string.

    void renderNonSkippedAttributes(bsl::string *result, const Record& record);
        // Render all attribute provided by the specified 'record' except
        // attributes whose keys are listed in the collection supplied at
        // construction of this object to the specified 'result' string.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(AttributesFormatter,
                                   bslma::UsesBslmaAllocator);

    // TYPES
    typedef bsl::allocator<char> allocator_type;

    // CREATORS
    explicit AttributesFormatter(
                          const SkipAttributes  *skipAttributes,
                          const allocator_type&  allocator = allocator_type());
        // Create an attribute formatter object having the specified
        // 'skipAttributes' collection.  Optionally specify an 'allocator'
        // (e.g., the address of a 'bslma::Allocator' object) to supply memory;
        // otherwise, the default allocator is used.

    AttributesFormatter(
                      const AttributesFormatter& original,
                      const allocator_type&      allocator = allocator_type());
        // Create an attribute formatter initialized to the value of the
        // specified 'original' record formatter.  Optionally specify an
        // 'allocator' (e.g., the address of a 'bslma::Allocator' object) to
        // supply memory; otherwise, the default allocator is used.

    // MANIPULATORS
    void operator()(bsl::string *result, const Record& record);
        // Render all attribute provided by the specified 'record' except
        // attributes whose keys are listed in the collection supplied at
        // construction of this object to the specified 'result' string.
};

                       // ---------------
                       // class PrintUtil
                       // ---------------

void PrintUtil::appendAttribute(bsl::string             *result,
                                const ManagedAttribute&  a,
                                bool                     printKey)
{
    if (printKey) {
        *result += a.key();
        *result += '=';
    }
    if (a.value().is<bsl::string>()) {
        *result += '"';
        *result += a.value().the<bsl::string>();
        *result += '"';
    }
    else if (a.value().is<int>()) {
        appendValue(result, a.value().the<int>());
    }
    else if (a.value().is<long>()) {
        appendValue(result, a.value().the<long>());
    }
    else if (a.value().is<long long>()) {
        appendValue(result, a.value().the<long long>());
    }
    else if (a.value().is<unsigned int>()) {
        appendValue(result, a.value().the<unsigned int>());
    }
    else if (a.value().is<unsigned long>()) {
        appendValue(result, a.value().the<unsigned long>());
    }
    else if (a.value().is<unsigned long long>()) {
        appendValue(result, a.value().the<unsigned long long>());
    }
    else if (a.value().is<const void *>()) {

        const int                   k_STORAGE_SIZE = 32;
        char                        storage[k_STORAGE_SIZE] = { 0 };
        bdlsb::FixedMemOutStreamBuf buffer(storage, k_STORAGE_SIZE - 1);
        bsl::ostream                stream(&buffer);
        bslim::Printer              printer(&stream, 0, -1);

        printer.printHexAddr(a.value().the<const void *>(), 0);

        appendString(result, &storage[1]);
    }
}

void PrintUtil::appendCategory(bsl::string *result, const Record& record)
{
    *result += record.fixedFields().category();
}

void PrintUtil::appendDatetime(bsl::string                  *result,
                               const Record&                 record,
                               const bdlt::DatetimeInterval *timestampOffset,
                               TimestampFormat               timestampFormat,
                               FractionalSecondPrecision     secondPrecision)
{
    bdlt::DatetimeInterval  offset;

    if (PublishInLocalTimeUtil::k_ENABLE ==
                                          timestampOffset->totalMilliseconds())
    {
        bsls::Types::Int64 localTimeOffsetInSeconds =
            bdlt::LocalTimeOffset::localTimeOffset(
                              record.fixedFields().timestamp()).totalSeconds();
        offset.setTotalSeconds(localTimeOffsetInSeconds);
    } else if (PublishInLocalTimeUtil::k_DISABLE !=
                                        timestampOffset->totalMilliseconds()) {
        offset = *timestampOffset;
    }

    int              offsetInMinutes = static_cast<int>(offset.totalMinutes());
    bdlt::DatetimeTz timestamp(record.fixedFields().timestamp() + offset,
                               offsetInMinutes);

    switch (timestampFormat) {
      case e_TF_ISO8601: {
        bdlt::Iso8601UtilConfiguration config;

        if (secondPrecision) {
            config.setFractionalSecondPrecision(secondPrecision);
        }
        config.setUseZAbbreviationForUtc(true);

        char buffer[bdlt::Iso8601Util::k_DATETIMETZ_STRLEN + 1];

        int outputLength = bdlt::Iso8601Util::generateRaw(buffer,
                                                          timestamp,
                                                          config);

        if (e_FSP_NONE == secondPrecision) {

            enum { k_DECIMAL_SIGN_OFFSET = 19,
                   k_TZINFO_OFFSET       = k_DECIMAL_SIGN_OFFSET + 4 };

            bsl::string_view head(buffer, k_DECIMAL_SIGN_OFFSET);
            bsl::string_view tail(buffer + k_TZINFO_OFFSET,
                                  outputLength - k_TZINFO_OFFSET);
            *result += head;
            *result += tail;
        }
        else {
            result->append(buffer, outputLength);
        }
      } break;
      case e_TF_DATETIME: {
        char buffer[32];

        timestamp.localDatetime().printToBuffer(buffer,
                                                sizeof buffer,
                                                secondPrecision);
        *result += buffer;
      } break;
      case e_TF_DATETIME_TZ_OFFSET: {
        char buffer[64];

        // Printing local time.

        int numChars = timestamp.localDatetime().printToBuffer(
                                                              buffer,
                                                              sizeof buffer,
                                                              secondPrecision);

        // Printing offset.

        char       *offsetBuffer = buffer + numChars;
        const char  sign         = offsetInMinutes < 0 ? '-' : '+';
        offsetInMinutes          = offsetInMinutes < 0 ? -offsetInMinutes
                                                       :  offsetInMinutes;
        const int   hours        = offsetInMinutes / 60;
        const int   minutes      = offsetInMinutes % 60;

        // Although an offset greater than 24 hours is undefined behavior, such
        // invalid 'DatetimeTz' objects still can be created under certain
        // circumstances.  We want to enable clients to detect these errors as
        // quickly as possible (DRQS 12693813).

        if (hours < 100) {
            bsl::sprintf(offsetBuffer, "%c%02d%02d", sign, hours, minutes);
        }
        else {
            bsl::sprintf(offsetBuffer, "%cXX%02d",   sign,        minutes);
        }

        *result += buffer;
      } break;
    }
}

void PrintUtil::appendFilename(bsl::string   *result,
                               bool           fullPath,
                               const Record&  record)
{
    const bsl::string_view filename(record.fixedFields().fileName());

    if (fullPath) {
        *result += filename;
    }
    else {
        bsl::string basename;
        int rc = bdls::PathUtil::getBasename(&basename, filename);

        if (0 == rc) {
            *result += basename;
        }
        else {
            *result += filename;
        }
    }
}

void PrintUtil::appendHexDump(bsl::string             *result,
                              const bsl::string_view&  string)
{
    static const char HEX[] = "0123456789ABCDEF";

    bsl::string_view::const_iterator i   = string.begin();
    bsl::string_view::const_iterator end = string.end();

    for (; i != end; ++i) {

        const unsigned char c = *i;

        result->push_back(HEX[(c >> 4) & 0xF]);
        result->push_back(HEX[ c       & 0xF]);
    }
}

void PrintUtil::appendLineNumber(bsl::string *result, const Record& record)
{
    appendValue(result, record.fixedFields().lineNumber());
}

void PrintUtil::appendMessage(bsl::string *result, const Record& record)
{
    appendString(result, record.fixedFields().messageRef());
}

void PrintUtil::appendMessageNonPrintableChars(bsl::string   *result,
                                               const Record&  record)
{
    appendString(result, record.fixedFields().messageRef(), true);
}

void PrintUtil::appendMessageAsHex(bsl::string *result, const Record& record)
{
    appendHexDump(result, record.fixedFields().messageRef());
}

template <class T>
void PrintUtil::appendValue(bsl::string *result,
                            const char  *format,
                            T            value)
{
    char buffer[16];

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf
#endif

    snprintf(buffer, sizeof buffer, format, value);

#if defined(BSLS_PLATFORM_CMP_MSVC)
#undef snprintf
#endif

    *result += buffer;
}

void PrintUtil::appendValue(bsl::string *result, int value)
{
    appendValue(result, "%d", value);
}

void PrintUtil::appendValue(bsl::string  *result, long value)
{
    appendValue(result, "%ld", value);
}

void PrintUtil::appendValue(bsl::string  *result, long long value)
{
    appendValue(result, "%lld", value);
}

void PrintUtil::appendValue(bsl::string  *result, unsigned int value)
{
    appendValue(result, "%u", value);
}

void PrintUtil::appendValue(bsl::string  *result, unsigned long value)
{
    appendValue(result, "%lu", value);
}

void PrintUtil::appendValue(bsl::string  *result, unsigned long long value)
{
    appendValue(result, "%llu", value);
}

void PrintUtil::appendProcessId(bsl::string   *result,
                                const Record&  record)
{
    appendValue(result, record.fixedFields().processID());
}

void PrintUtil::appendString(bsl::string             *result,
                             const bsl::string_view&  string,
                             bool                     notPrintable)
{
    if (notPrintable) {

        bsl::string_view::const_iterator startRange = string.begin();
        bsl::string_view::const_iterator endRange   = startRange;
        bsl::string_view::const_iterator end = string.end();

        while (endRange != end) {
            if (*endRange < 0x20 || *endRange > 0x7E) {  // not printable
                result->append(&*startRange, bsl::distance(startRange,
                                                           endRange));

                static const char HEX[] = "0123456789ABCDEF";
                const char        value = *endRange;

                *result += "\\x";
                *result += HEX[(value >> 4) & 0xF];
                *result += HEX[value        & 0xF];

                ++endRange;
                startRange = endRange;
            }
            else {
                ++endRange;
            }
        }
        if (startRange != end) {
            result->append(&*startRange, bsl::distance(startRange, endRange));
        }
    }
    else {
        result->append(string.data(), string.length());
    }
}

void PrintUtil::appendThreadId(bsl::string   *result,
                               const Record&  record)
{
    appendValue(result, "%llu", record.fixedFields().threadID());
}

void PrintUtil::appendThreadIdAsHex(bsl::string   *result,
                                    const Record&  record)
{
    appendValue(result, "%llX", record.fixedFields().threadID());
}

void PrintUtil::appendSeverity(bsl::string   *result,
                               const Record&  record)
{
    result->append(Severity::toAscii(static_cast<Severity::Level>(
                                            record.fixedFields().severity())));
}

void PrintUtil::appendUserFields(bsl::string *result, const Record& record)
{
    typedef UserFields Values;
    const Values& customFields    = record.customFields();
    const int     numCustomFields = customFields.length();

    enum { k_INITIAL_CAPACITY = 128 };
    char initialBuffer[k_INITIAL_CAPACITY];

    if (numCustomFields > 0) {
        Values::ConstIterator it = customFields.begin();

        bdlsb::OverflowMemOutStreamBuf streamBuffer(initialBuffer,
                                                    k_INITIAL_CAPACITY);
        bsl::ostream os(&streamBuffer);
        os << *it;
        ++it;
        for (; it != customFields.end(); ++it) {
            os << ' ' << *it;
        }
        result->append(streamBuffer.initialBuffer(),
                       streamBuffer.dataLengthInInitialBuffer());
        if (streamBuffer.overflowBuffer()) {
            result->append(streamBuffer.overflowBuffer(),
                           streamBuffer.dataLengthInOverflowBuffer());
        }
    }
}

                       // ------------------------
                       // class AttributeFormatter
                       // ------------------------

AttributeFormatter::AttributeFormatter(const bsl::string_view& key,
                                       bool                    renderKey)
: d_key(key)
, d_index(k_UNSET)
, d_renderKey(renderKey)
{
}

void AttributeFormatter::operator()(bsl::string *result, const Record& record)
{
    const Attributes& attributes = record.attributes();
    if (k_UNSET == d_index ||
        d_index >= static_cast<int>(attributes.size()) ||
        d_key   != attributes[d_index].key())
    {
        // If either d_index has not been cached before, or if it's value no
        // longer refers to the correct attribute (because the set of collected
        // attributes has changed) we need to try and set d_index.

        d_index = k_UNSET;
        for (Attributes::const_iterator i = attributes.begin();
             i != attributes.end();
             ++i)
        {
            if (d_key == i->key()) {
                d_index = static_cast<int>(bsl::distance(attributes.begin(),
                                                         i));
                break;                                                 // BREAK
            }
        }
        if (k_UNSET == d_index) {
            // If an attribute with the specified key is not found, print
            // nothing.
            return;                                                   // RETURN
        }
    }
    PrintUtil::appendAttribute(result, attributes.at(d_index), d_renderKey);
}

                       // -------------------------
                       // class AttributesFormatter
                       // -------------------------

AttributesFormatter::AttributesFormatter(const SkipAttributes  *skipAttributes,
                                         const allocator_type&  allocator)
: d_skipAttributes_p(skipAttributes)
, d_cache(allocator)
{
}

AttributesFormatter::AttributesFormatter(const AttributesFormatter& original,
                                         const allocator_type&      allocator)
: d_skipAttributes_p(original.d_skipAttributes_p)
, d_cache(original.d_cache, allocator)
{
}

void AttributesFormatter::renderAllAttributes(bsl::string   *result,
                                              const Record&  record)
{
    const Attributes&          attributes = record.attributes();
    Attributes::const_iterator i          = attributes.begin();
    Attributes::const_iterator end        = attributes.end();

    for (;i < end; ++i) {
        PrintUtil::appendAttribute(result, *i);
        result->push_back(' ');
    }
}

void AttributesFormatter::renderNonSkippedAttributes(bsl::string   *result,
                                                     const Record&  record)
{
    const Attributes& attributes = record.attributes();

    for (Attributes::size_type i = 0; i < attributes.size(); ++i) {
        const ManagedAttribute& a = attributes[i];

        if (i < d_cache.size()) {
            if (d_cache[i].first != a.key()) {
                d_cache[i].first  = a.key();
                d_cache[i].second = d_skipAttributes_p->end() ==
                          d_skipAttributes_p->find(bsl::string_view(a.key()));
            }
        }
        else {
            d_cache.emplace_back(
                bsl::make_pair(
                         a.key(),
                         d_skipAttributes_p->end() ==
                         d_skipAttributes_p->find(bsl::string_view(a.key()))));
        }
        if (d_cache[i].second) {
            PrintUtil::appendAttribute(result, a);
            result->push_back(' ');
        }
    }
}

void AttributesFormatter::operator()(bsl::string *result, const Record& record)
{
    const bsl::string::size_type len = result->length();

    if (0 == d_skipAttributes_p) {
        renderAllAttributes(result, record);
    }
    else {
        renderNonSkippedAttributes(result, record);
    }

    if (len != result->length()) {
        result->pop_back();
    }
}
}  // close unnamed namespace


                        // ---------------------------
                        // class RecordStringFormatter
                        // ---------------------------

// PUBLIC CONSTANTS
const char *RecordStringFormatter::k_DEFAULT_FORMAT =
    "\n%d %p:%t %s %f:%l %c %m %u\n";

const char *RecordStringFormatter::k_BASIC_ATTRIBUTE_FORMAT =
    "\n%d %p:%t %s %f:%l %c %a %m\n";

// PRIVATE MANIPULATORS
void RecordStringFormatter::parseFormatSpecification()
{
    d_fieldFormatters.clear();
    d_skipAttributes.clear();

    bsl::string::iterator i    = d_formatSpec.begin();
    bsl::string::iterator end  = d_formatSpec.end();
    bsl::string::iterator text = end;

    using namespace bdlf::PlaceHolders;

    while (i != end) {
        switch (*i) {
          default: {  // --------------------- text ---------------------------
            if (text == end) {
                text = i;
            }
          } break;
          case '\\': {  // ------------------- escape characters --------------
            if (i + 1 == end) {
                break;                                                 // BREAK
            }
            if (text != end) {
                // append text preceding to 'i'
                d_fieldFormatters.emplace_back(
                    bdlf::BindUtil::bind(&PrintUtil::appendString,
                                         _1,
                                         bsl::string_view(
                                                       text,
                                                       bsl::distance(text, i)),
                                         false));
                text = end;
            }
            ++i;
            switch (*i) {
              case 'n': {
                d_fieldFormatters.emplace_back(
                                 bdlf::BindUtil::bind(&PrintUtil::appendString,
                                                      _1,
                                                      "\n",
                                                      false));
              } break;
              case 't': {
                d_fieldFormatters.emplace_back(
                                bdlf::BindUtil::bind( &PrintUtil::appendString,
                                                      _1,
                                                      "\t",
                                                      false));
              } break;
              case '\\': {
                d_fieldFormatters.emplace_back(
                                 bdlf::BindUtil::bind(&PrintUtil::appendString,
                                                      _1,
                                                      "\\",
                                                      false));
              } break;
              default: {
                // Undefined: we just output the verbatim characters.

                text = i - 1;
              }
            }
          } break;
          case '%': {  // ------------------- %-specifier ---------------------
            if (i + 1 == end) {
                break;                                                 // BREAK
            }

            if (text != end) {
                // append text preceding to 'i'
                d_fieldFormatters.emplace_back(
                      bdlf::BindUtil::bind(&PrintUtil::appendString,
                                           _1,
                                           bsl::string_view(
                                                       text,
                                                       bsl::distance(text, i)),
                                           false));
                text = end;
            }

            ++i;

            switch (*i) {
              case '%': {  // ---------------- escape % -----------------------
                text = i;
              } break;
              case 'd': {  // ---------------- Datetime -----------------------
                if (end !=  (i + 1) &&
                    't' == *(i + 1) &&
                    end !=  (i + 2) &&
                    'z' == *(i + 2)) {  //  Datetime + timezone offset ('%dtz')
                    i += 2;
                    d_fieldFormatters.emplace_back(
                       bdlf::BindUtil::bind(&PrintUtil::appendDatetime,
                                            _1,
                                            _2,
                                            &d_timestampOffset,
                                            PrintUtil::e_TF_DATETIME_TZ_OFFSET,
                                            PrintUtil::e_FSP_MILLISECONDS));
                }
                else {
                    d_fieldFormatters.emplace_back(
                          bdlf::BindUtil::bind(&PrintUtil::appendDatetime,
                                               _1,
                                               _2,
                                               &d_timestampOffset,
                                               PrintUtil::e_TF_DATETIME,
                                               PrintUtil::e_FSP_MILLISECONDS));
                }
              } break;
              case 'D': {  // ---------------- Datetime -----------------------
                if (end !=  (i + 1) &&
                    't' == *(i + 1) &&
                    end !=  (i + 2) &&
                    'z' == *(i + 2)) {  //  Datetime + timezone offset ('%Dtz')
                    i += 2;
                    d_fieldFormatters.emplace_back(
                        bdlf::BindUtil::bind(&PrintUtil::appendDatetime,
                                            _1,
                                            _2,
                                            &d_timestampOffset,
                                            PrintUtil::e_TF_DATETIME_TZ_OFFSET,
                                            PrintUtil::e_FSP_MICROSECONDS));
                }
                else {
                    d_fieldFormatters.emplace_back(
                          bdlf::BindUtil::bind(&PrintUtil::appendDatetime,
                                               _1,
                                               _2,
                                               &d_timestampOffset,
                                               PrintUtil::e_TF_DATETIME,
                                               PrintUtil::e_FSP_MICROSECONDS));
                }
              } break;
              case 'i': {  // ---------------- Datetime ISO 8601 --------------
                d_fieldFormatters.emplace_back(
                          bdlf::BindUtil::bind(&PrintUtil::appendDatetime,
                                               _1,
                                               _2,
                                               &d_timestampOffset,
                                               PrintUtil::e_TF_ISO8601,
                                               PrintUtil::e_FSP_NONE));
              } break;
              case 'I': {  // ---------------- Datetime ISO 8601 --------------
                d_fieldFormatters.emplace_back(
                          bdlf::BindUtil::bind(&PrintUtil::appendDatetime,
                                               _1,
                                               _2,
                                               &d_timestampOffset,
                                               PrintUtil::e_TF_ISO8601,
                                               PrintUtil::e_FSP_MILLISECONDS));
              } break;
              case 'O': {  // ---------------- Datetime ISO 8601 --------------
                d_fieldFormatters.emplace_back(
                          bdlf::BindUtil::bind(&PrintUtil::appendDatetime,
                                               _1,
                                               _2,
                                               &d_timestampOffset,
                                               PrintUtil::e_TF_ISO8601,
                                               PrintUtil::e_FSP_MICROSECONDS));
              } break;
              case 'p': {  // ---------------- Process ID ---------------------
                d_fieldFormatters.emplace_back(
                              bdlf::BindUtil::bind(&PrintUtil::appendProcessId,
                                                   _1,
                                                   _2));
              } break;
              case 't': {  // ---------------- Thread ID ----------------------
                d_fieldFormatters.emplace_back(
                               bdlf::BindUtil::bind(&PrintUtil::appendThreadId,
                                                    _1,
                                                    _2));
              } break;
              case 'T': {  // ---------------- Thread ID hex ------------------
                d_fieldFormatters.emplace_back(
                          bdlf::BindUtil::bind(&PrintUtil::appendThreadIdAsHex,
                                               _1,
                                               _2));
              } break;
              case 's': {  // ---------------- Severity -----------------------
                d_fieldFormatters.emplace_back(
                               bdlf::BindUtil::bind(&PrintUtil::appendSeverity,
                                                    _1,
                                                    _2));
              } break;
              case 'f': {  // ---------------- Filename -----------------------
                d_fieldFormatters.emplace_back(
                               bdlf::BindUtil::bind(&PrintUtil::appendFilename,
                                                    _1,
                                                    true,
                                                    _2));
              } break;
              case 'F': {  // ---------------- Filename ----------------------
                d_fieldFormatters.emplace_back(
                               bdlf::BindUtil::bind(&PrintUtil::appendFilename,
                                                    _1,
                                                    false,
                                                    _2));
              } break;
              case 'l': {  // ---------------- Line Number --------------------
                d_fieldFormatters.emplace_back(
                             bdlf::BindUtil::bind(&PrintUtil::appendLineNumber,
                                                  _1,
                                                  _2));
              } break;
              case 'c': {  // ---------------- Category -----------------------
                d_fieldFormatters.emplace_back(
                               bdlf::BindUtil::bind(&PrintUtil::appendCategory,
                                                    _1,
                                                    _2));
              } break;
              case 'm': {  // ---------------- Message ------------------------
                d_fieldFormatters.emplace_back(
                                bdlf::BindUtil::bind(&PrintUtil::appendMessage,
                                                     _1,
                                                     _2));
              } break;
              case 'x': {  // ---------------- Message ------------------------
                d_fieldFormatters.emplace_back(
                    bdlf::BindUtil::bind(
                                    &PrintUtil::appendMessageNonPrintableChars,
                                    _1,
                                    _2));
              } break;
              case 'X': {  // ---------------- Message as hex -----------------
                d_fieldFormatters.emplace_back(
                           bdlf::BindUtil::bind(&PrintUtil::appendMessageAsHex,
                                                _1,
                                                _2));
              } break;
              case 'a': {  // ---------------- Attributes (%a/%av) ------------
                bsl::string::iterator j = i + 1;
                bool renderKey = true;

                if (j != end && 'v' == *j) {
                    renderKey = false;
                    ++j;
                }
                if (j != end && '[' == *j) {
                    bsl::string::iterator keyEnd = bsl::find(j + 1, end, ']');
                    if (keyEnd != end) {
                        const bsl::string_view key(j + 1,
                                                   bsl::distance(j+1, keyEnd));
                        d_fieldFormatters.emplace_back(
                                           AttributeFormatter(key, renderKey));
                        if (d_skipAttributes.end() ==
                            d_skipAttributes.find(key))
                        {
                            d_skipAttributes.emplace(key);
                        }
                        i = keyEnd;
                    }
                }
                else {
                    d_fieldFormatters.emplace_back(
                        AttributesFormatter(&d_skipAttributes,
                                            d_skipAttributes.get_allocator()));
                }
              } break;
              case 'A': {  // ---------------- Attributes (%A) ----------------
                d_fieldFormatters.emplace_back(
                        AttributesFormatter(0,
                                            d_skipAttributes.get_allocator()));
              } break;
              case 'u': {
                d_fieldFormatters.emplace_back(
                             bdlf::BindUtil::bind(&PrintUtil::appendUserFields,
                                                  _1,
                                                  _2));
              } break;
              default: {
                // Undefined: we just output the verbatim characters.

                text = i - 1;
              } break;
            }
          } break;
        }
        ++i;
    }

    if (text != end) {
        d_fieldFormatters.emplace_back(
                                 bdlf::BindUtil::bind(&PrintUtil::appendString,
                                                      _1,
                                                      text,
                                                      false));
    }
}

// CREATORS
RecordStringFormatter::RecordStringFormatter(const allocator_type& allocator)
: d_formatSpec(DEFAULT_FORMAT_SPEC, allocator)
, d_fieldFormatters(allocator)
, d_skipAttributes(allocator)
, d_timestampOffset(0)
{
    parseFormatSpecification();
}

RecordStringFormatter::RecordStringFormatter(bslma::Allocator *basicAllocator)
: d_formatSpec(DEFAULT_FORMAT_SPEC, basicAllocator)
, d_fieldFormatters(basicAllocator)
, d_skipAttributes(basicAllocator)
, d_timestampOffset(0)
{
    parseFormatSpecification();
}

RecordStringFormatter::RecordStringFormatter(const char            *format,
                                             const allocator_type&  allocator)
: d_formatSpec(format, allocator)
, d_fieldFormatters(allocator)
, d_skipAttributes(allocator)
, d_timestampOffset(0)
{
    parseFormatSpecification();
}

RecordStringFormatter::RecordStringFormatter(const char       *format,
                                             bslma::Allocator *basicAllocator)
: d_formatSpec(format, basicAllocator)
, d_fieldFormatters(basicAllocator)
, d_skipAttributes(basicAllocator)
, d_timestampOffset(0)
{
    parseFormatSpecification();
}


RecordStringFormatter::RecordStringFormatter(
                                      const bdlt::DatetimeInterval&  offset,
                                      const allocator_type&          allocator)
: d_formatSpec(DEFAULT_FORMAT_SPEC, allocator)
, d_fieldFormatters(allocator)
, d_skipAttributes(allocator)
, d_timestampOffset(offset)
{
    parseFormatSpecification();
}

RecordStringFormatter::RecordStringFormatter(
                                      bool                  publishInLocalTime,
                                      const allocator_type& allocator)
: d_formatSpec(DEFAULT_FORMAT_SPEC, allocator)
, d_fieldFormatters(allocator)
, d_skipAttributes(allocator)
, d_timestampOffset(0,
                    0,
                    0,
                    0,
                    publishInLocalTime
                    ? PublishInLocalTimeUtil::k_ENABLE
                    : PublishInLocalTimeUtil::k_DISABLE)
{
    parseFormatSpecification();
}

RecordStringFormatter::RecordStringFormatter(
                                      const char                    *format,
                                      const bdlt::DatetimeInterval&  offset,
                                      const allocator_type&          allocator)
: d_formatSpec(format, allocator)
, d_fieldFormatters(allocator)
, d_skipAttributes(allocator)
, d_timestampOffset(offset)
{
    parseFormatSpecification();
}

RecordStringFormatter::RecordStringFormatter(
                                     const char            *format,
                                     bool                   publishInLocalTime,
                                     const allocator_type&  allocator)
: d_formatSpec(format, allocator)
, d_fieldFormatters(allocator)
, d_skipAttributes(allocator)
, d_timestampOffset(0,
                    0,
                    0,
                    0,
                    publishInLocalTime
                    ? PublishInLocalTimeUtil::k_ENABLE
                    : PublishInLocalTimeUtil::k_DISABLE)
{
    parseFormatSpecification();
}

RecordStringFormatter::RecordStringFormatter(
                                        const RecordStringFormatter& original,
                                        const allocator_type&        allocator)
: d_formatSpec(original.d_formatSpec, allocator)
, d_fieldFormatters(allocator)
, d_skipAttributes(allocator)
, d_timestampOffset(original.d_timestampOffset)
{
    parseFormatSpecification();
}

// MANIPULATORS
void RecordStringFormatter::disablePublishInLocalTime()
{
    d_timestampOffset.setTotalMilliseconds(PublishInLocalTimeUtil::k_DISABLE);
}

void RecordStringFormatter::enablePublishInLocalTime()
{
    d_timestampOffset.setTotalMilliseconds(PublishInLocalTimeUtil::k_ENABLE);
}

bool RecordStringFormatter::isPublishInLocalTimeEnabled() const
{
    return PublishInLocalTimeUtil::k_ENABLE ==
                                         d_timestampOffset.totalMilliseconds();
}

RecordStringFormatter& RecordStringFormatter::operator=(
                                              const RecordStringFormatter& rhs)
{
    if (this != &rhs) {
        d_formatSpec      = rhs.d_formatSpec;
        d_fieldFormatters = rhs.d_fieldFormatters;
        d_skipAttributes  = rhs.d_skipAttributes;
        d_timestampOffset = rhs.d_timestampOffset;
    }

    return *this;
}

// ACCESSORS
void RecordStringFormatter::operator()(bsl::ostream& stream,
                                       const Record& record) const

{
    const int k_BUFFER_SIZE        = 512;
    const int k_STRING_RESERVATION = k_BUFFER_SIZE -
                                     bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

    char fixedBuffer[k_BUFFER_SIZE];
    bdlma::BufferedSequentialAllocator stringAllocator(fixedBuffer,
                                                       k_BUFFER_SIZE);
    bsl::string output(&stringAllocator);
    output.reserve(k_STRING_RESERVATION);

    for (FieldStringFormatters::const_iterator i = d_fieldFormatters.cbegin();
         i != d_fieldFormatters.cend();
         ++i)
    {
        (*i)(&output, record);
    }

    stream.write(output.c_str(), output.size());
    stream.flush();

    return;
}

}  // close package namespace

// FREE OPERATORS
bool ball::operator==(const RecordStringFormatter& lhs,
                      const RecordStringFormatter& rhs)
{
    return 0 == bsl::strcmp(lhs.format(), rhs.format())
        && lhs.timestampOffset() == rhs.timestampOffset();
}

bsl::ostream& ball::operator<<(bsl::ostream&                output,
                               const RecordStringFormatter& rhs)
{
    return output << "\'" << rhs.format() << "\' "
                  << (rhs.isPublishInLocalTimeEnabled() ? "local-time"
                                                        : "UTC");
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
