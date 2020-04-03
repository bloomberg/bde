// balcl_commandline.cpp                                              -*-C++-*-
#include <balcl_commandline.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balcl_commandline_cpp,"$Id$ $CSID$")

#include <balcl_occurrenceinfo.h>
#include <balcl_typeinfo.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_time.h>

#include <bslim_printer.h>

#include <bsls_assert.h>

#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace balcl {

#define EXIT_IF_FALSE(X) do {                                                 \
    if (!(X)) {                                                               \
        BloombergLP::bsls::AssertViolation violation(                         \
                                  #X,                                         \
                                  __FILE__,                                   \
                                  __LINE__,                                   \
                                  BloombergLP::bsls::Assert::k_LEVEL_INVOKE); \
        BloombergLP::bsls::Assert::invokeHandler(violation);                  \
    }                                                                         \
} while (false)
    // This macro is enabled *unconditionally*.  Note that 'BSLS_ASSERT' can be
    // disabled for certain build targets (e.g., optimized non-debug), and is
    // not appropriate where the control flow actually demands to abort, throw,
    // or whatever the assertion handler is configured to do (such as in
    // validating the command-line spec in the command-line constructor).  Note
    // that we don't want to hard-code an 'abort' there, to retain testability
    // (the test driver should verify good behavior by changing the assertion
    // handler to throw a custom exception, pass in invalid command-line specs,
    // and make sure it catches that exception).

enum {
    // This 'enum' is used in place of 0 for readability in the assertion
    // macros.

    k_INVALID_COMMAND_LINE_SPEC = 0  // must be 0 to trigger an assert
};

// ============================================================================
//                       LOCAL CLASS AND FUNCTION DEFINITIONS
// ============================================================================

namespace {
namespace u {
                        // =============================
                        // local functions format(. . .)
                        // =============================

void format(bsl::size_t                     start,
            bsl::size_t                     end,
            const bsl::vector<bsl::string>& strings,
            bsl::ostream&                   stream,
            bsl::size_t                     col = 0)
    // Output the specified 'strings' sequence of words to the specified
    // 'stream', separated by spaces, formatted to fit between the columns at
    // the specified 'start' and 'end' indices in a greedy fashion, and
    // returning to a new line prefixed by 'start' spaces in case the words
    // would write into the 'end' column.  Optionally specify 'col', the
    // current "column" position.  Note that a single word of length larger
    // than 'end - start' characters is printed on a single line at the 'start'
    // position, and is the only possibility for this stream to reach or exceed
    // 'end' characters on a single line.
{
    if (0 == strings.size()) {
        return;                                                       // RETURN
    }

    if (col < start) {
        stream << bsl::string(start - col, ' ') << strings[0];
        col = start + strings[0].size();
    } else {
        stream << strings[0];
        col += strings[0].size();
    }

    for (bsl::size_t i = 1; i < strings.size(); ++i) {
        if (strings[i].empty()) {
            continue;
        }
        if (col + 1 + strings[i].size() >= end) {
            stream << '\n' << bsl::string(start, ' ') << strings[i];
            col = start + strings[i].size();
        } else {
            stream << ' ' << strings[i];
            col += 1 + strings[i].size();
        }
    }
}

void format(bsl::size_t   start,
            bsl::size_t   end,
            bsl::string&  string,
            bsl::ostream& stream,
            bsl::size_t   col = 0)
    // Format the specified 'string' to the specified 'stream', with words
    // separated by spaces, formatted to fit between the columns at the
    // specified 'start' and 'end' indices as specified by the previous
    // function (in particular, returning to a new line prefixed by 'start'
    // spaces in case the words would write into the 'end' column).  Optionally
    // specify 'col', the current "column" position.  Note that words are
    // delimited by either a space, tab, or newline character, but are rendered
    // as separated by spaces (thus the original separating character is lost).
    // Also note that 'string' is passed by non-'const' reference and may be
    // modified.
{
    bsl::vector<bsl::string> strings;

    char *temp;
    char *src = const_cast<char *>(string.c_str());

    while ((temp = bsl::strtok(src, " \t\n"))) {
        strings.push_back(temp);
        src = 0;
    }

    format(start, end, strings, stream, col);
}

                            // ==============
                            // struct Ordinal
                            // ==============

struct Ordinal {
    // This 'struct' assists in printing numbers as ordinals (1st, 2nd, etc.).

    bsl::size_t d_rank;  // rank (starting at 0)

    // CREATORS
    explicit Ordinal(bsl::size_t n);
        // Create an ordinal for the specified position 'n' (starting at 0).
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, Ordinal position);
    // Output the specified 'position' (starting at 0) to the specified
    // 'stream' as an ordinal, mapping 0 to "1st", 1 to "2nd", 3 to "3rd", 4 to
    // "4th", etc. following correct English usage.

                            // --------------
                            // struct Ordinal
                            // --------------

// CREATORS
Ordinal::Ordinal(bsl::size_t n)
: d_rank(n)
{
}

}  // close namespace u

// FREE OPERATORS
bsl::ostream& u::operator<<(bsl::ostream& stream, Ordinal position)
{
    // ranks start at 0, but are displayed as 1st, 2nd, etc.
    int n = static_cast<int>(position.d_rank + 1);
    switch (n % 10) {
      case 1: {
        if (n % 100 == 11) {
            stream << n << "th";
        } else {
            stream << n << "st";
        }
      } break;
      case 2: {
        if (n % 100 == 12) {
            stream << n << "th";
        } else {
            stream << n << "nd";
        }
      } break;
      case 3: {
        if (n % 100 == 13) {
            stream << n << "th";
        } else {
            stream << n << "rd";
        }
      } break;
      default: {
        stream << n << "th";
      } break;
    }
    return stream;
}

namespace u {

                        // =====================
                        // class OptionValueUtil
                        // =====================

struct OptionValueUtil {
    static void setValue(void *dst, const OptionValue& src);
        // Assign to the object at the specified 'dst' the value of the
        // specified 'src'.  The behavior is undefined unless
        // 'OptionType::e_VOID != src.type()' and 'dst' can be cast to a
        // pointer to an object of 'OptionType::EnumToType<src.type()>::type'.
};

                        // ---------------------
                        // class OptionValueUtil
                        // ---------------------

void OptionValueUtil::setValue(void               *dst,
                               const OptionValue&  src)
{
    BSLS_ASSERT(dst);

    typedef OptionType Ot;

    switch (src.type()) {
      case Ot::e_VOID: {
          BSLS_ASSERT_OPT(!"Reached");
      } break;
      case Ot::e_BOOL: {
        *(static_cast<Ot::Bool          *>(dst)) = src.the<Ot::Bool>();
      } break;
      case Ot::e_CHAR: {
        *(static_cast<Ot::Char          *>(dst)) = src.the<Ot::Char>();
      } break;
      case Ot::e_INT: {
        *(static_cast<Ot::Int           *>(dst)) = src.the<Ot::Int>();
      } break;
      case Ot::e_INT64: {
        *(static_cast<Ot::Int64         *>(dst)) = src.the<Ot::Int64>();
      } break;
      case Ot::e_DOUBLE: {
        *(static_cast<Ot::Double        *>(dst)) = src.the<Ot::Double>();
      } break;
      case Ot::e_STRING: {
        *(static_cast<Ot::String        *>(dst)) = src.the<Ot::String>();
      } break;
      case Ot::e_DATETIME: {
        *(static_cast<Ot::Datetime      *>(dst)) = src.the<Ot::Datetime>();
      } break;
      case Ot::e_DATE: {
        *(static_cast<Ot::Date          *>(dst)) = src.the<Ot::Date>();
      } break;
      case Ot::e_TIME: {
        *(static_cast<Ot::Time          *>(dst)) = src.the<Ot::Time>();
      } break;
      case Ot::e_CHAR_ARRAY: {
        *(static_cast<Ot::CharArray     *>(dst)) = src.the<Ot::CharArray>();
      } break;
      case Ot::e_INT_ARRAY: {
        *(static_cast<Ot::IntArray      *>(dst)) = src.the<Ot::IntArray>();
      } break;
      case Ot::e_INT64_ARRAY: {
        *(static_cast<Ot::Int64Array    *>(dst)) = src.the<Ot::Int64Array>();
      } break;
      case Ot::e_DOUBLE_ARRAY: {
        *(static_cast<Ot::DoubleArray   *>(dst)) = src.the<Ot::DoubleArray>();
      } break;
      case Ot::e_STRING_ARRAY: {
        *(static_cast<Ot::StringArray   *>(dst)) = src.the<Ot::StringArray>();
      } break;
      case Ot::e_DATETIME_ARRAY: {
        *(static_cast<Ot::DatetimeArray *>(dst)) =
                                                  src.the<Ot::DatetimeArray>();
      } break;
      case Ot::e_DATE_ARRAY: {
        *(static_cast<Ot::DateArray     *>(dst)) = src.the<Ot::DateArray>();
      } break;
      case Ot::e_TIME_ARRAY: {
        *(static_cast<Ot::TimeArray     *>(dst)) = src.the<Ot::TimeArray>();
      } break;
    }
}

int validate(const bsl::vector<Option>& options,
             bsl::ostream&              stream)
    // Validate the specified 'options' against the constraints described in
    // {Valid 'balcl::OptionInfo' Specifications} in the component-level
    // documentation.  Return 0 if 'options' are valid, and a non-zero value
    // otherwise.  If 'options' is invalid, a descriptive message is written to
    // the specified 'stream'.
{
    int status = 0;

    for (bsl::size_t i = 0; i < options.size(); ++i) {
        const Option& thisOption = options[i];

        // Start with basic validity checks.  Try to do as many checks at a
        // time, in order to maximize the feedback to the user.

        if (!thisOption.isTagValid(stream)) {
            stream << "The error occurred while validating the "
                   << u::Ordinal(i) << " option." << '\n'
                   << bsl::flush;

            status = 1;
        }

        if (!thisOption.isNameValid(stream)) {
            stream << "The error occurred while validating the "
                   << u::Ordinal(i) << " option." << '\n'
                   << bsl::flush;

            status = 2;
        }

        if (!thisOption.isDescriptionValid(stream)) {
            stream << "The error occurred while validating the "
                   << u::Ordinal(i) << " option." << '\n'
                   << bsl::flush;

            status = 3;
        }

        if (OptionInfo::e_FLAG == thisOption.argType()
         && thisOption.occurrenceInfo().hasDefaultValue())
        {
            stream << "No default value is allowed for the flag." << '\n'
                   << "The error occurred while validating the "
                   << u::Ordinal(i) << " option." << '\n'
                   << bsl::flush;

            status = 4;
        }

        if (thisOption.occurrenceInfo().hasDefaultValue()
         && thisOption.occurrenceInfo().defaultValue().type() !=
                                              thisOption.typeInfo().type()) {
            stream << "The type of default value does not "
                      "match the type specified for the option." << '\n'
                   << "The error occurred while validating the "
                   << u::Ordinal(i) << " option." << '\n'
                   << bsl::flush;

            status = 5;
        }

        // Tags and names must be unique.

        for (bsl::size_t j = 0; j < i; ++j) {
            if (thisOption.name() == options[j].name()) {
                stream << "Error: The names for the "
                       << u::Ordinal(i) << " and "
                       << u::Ordinal(j) << " options are equal."
                       << '\n' << bsl::flush;

                status = 6;
            }
        }

        if (thisOption.argType() != OptionInfo::e_NON_OPTION) {
            for (bsl::size_t j = 0; j < i; ++j) {
                if (options[j].argType() == OptionInfo::e_NON_OPTION) {
                    continue;
                }

                if (thisOption.shortTag()
                 && thisOption.shortTag() == options[j].shortTag()) {
                    stream << "Error: short tags for the "
                           << u::Ordinal(i) << " and "
                           << u::Ordinal(j) << " options are equal."
                           << '\n' << bsl::flush;

                    status = 7;
                }

                if (!bsl::strcmp(thisOption.longTag(),
                                 options[j].longTag())) {
                    stream << "Error: long tags for the "
                           << u::Ordinal(i) << " and "
                           << u::Ordinal(j) << " options are equal."
                           << '\n' << bsl::flush;

                    status = 8;
                }
            }
        }

        // Additional checks for non-option arguments.

        if (OptionInfo::e_NON_OPTION == thisOption.argType()) {
            // Non-option arguments cannot be hidden.

            if (thisOption.occurrenceInfo().isHidden()) {
                stream << "Error: A non-option argument cannot be hidden.\n"
                          "The error occurred while validating the "
                       << u::Ordinal(i) << " option.\n"
                       << bsl::flush;

                status = 9;
            }
        }
    }

    if (0 != status) {
        return status;                                                // RETURN
    }

    bool seenNonOption      = false;
    int  lastNonOptionIndex = -1;

    for (bsl::size_t i = 0; i < options.size(); ++i) {
        const Option& thisOption = options[i];

        // Validate and initialize with the default values.

        if (thisOption.occurrenceInfo().hasDefaultValue()) {
            if (!TypeInfoUtil::satisfiesConstraint(
                                    thisOption.occurrenceInfo().defaultValue(),
                                    thisOption.typeInfo(),
                                    stream)) {
                stream << "Error: default value violates constraint." << '\n';
                stream << "The error occurred while validating the "
                       << u::Ordinal(i) << " option." << '\n'
                       << bsl::flush;

                status = -1;

                return status;                                        // RETURN
            }
        }

        // Additional checks for non-option arguments.

        if (OptionInfo::e_NON_OPTION == thisOption.argType()) {
            // Only last non-option argument can be multi-valued.

            if (seenNonOption && options[lastNonOptionIndex].isArray()) {
                stream << "Error: A multi-valued non-option argument was "
                          "already specified as the "
                       << u::Ordinal(lastNonOptionIndex)
                       << " option." << '\n'
                       << "The error occurred while validating the "
                       << u::Ordinal(i) << " option." << '\n'
                       << bsl::flush;

                status = -2;

                return status;                                        // RETURN
            }

            // Once default value is provided, must *always* be provided for
            // subsequent non-option arguments.

            if (!thisOption.occurrenceInfo().hasDefaultValue()
             && seenNonOption
             && options[lastNonOptionIndex].occurrenceInfo().hasDefaultValue())
                                                                              {
                stream << "Error: A default value was provided "
                          "for the previous non-option argument, "
                          "specified as\nthe "
                       << u::Ordinal(lastNonOptionIndex)
                       << " option, but not for this non-option." << '\n'
                       << "The error occurred while validating the "
                       << u::Ordinal(i) << " option." << '\n'
                       << bsl::flush;

                status = -3;

                return status;                                        // RETURN
            }

            seenNonOption      = true;
            lastNonOptionIndex = static_cast<int>(i);
        }
    }

    return status;
}

void initialize(bsl::vector<OptionValue>       *data,
                bsl::vector<bsl::vector<int> > *positions,
                bsl::vector<int>               *nonOptionIndices,
                const bsl::vector<Option>&      options)
    // Initialize the specified 'data', 'positions', and 'nonOptionIndices'
    // according to the specified 'options'.  The initializations are:
    //
    //: o 'data' is initialized with 'option.size()' elements, each in the
    //:   "null" state, and each having the same type as the corresponding
    //:   option element in 'options'.
    //:
    //: o 'positions' is initialized with 'options.size()' empty vectors of
    //:   'int'.
    //:
    //: o 'nonOptionIndices' is initialized with the indices in 'options', in
    //:   ascending order, where non-option arguments are found, if any.
    //
    // The behavior is undefined unless 'data', 'positions, and
    // 'nonOptionIndices' are empty, and unless 'options' are valid according
    // to the 'validate' function.  Note that there is no guarantee that
    // 'data', 'positions', and 'nonOptionIndices' remain empty if a non-zero
    // value is returned.
{
    BSLS_ASSERT(data);
    BSLS_ASSERT(positions);
    BSLS_ASSERT(nonOptionIndices);

    bsl::ostringstream oss;  (void)oss;
    BSLS_ASSERT_SAFE(0 == validate(options, oss));

    for (bsl::size_t i = 0; i < options.size(); ++i) {
        const Option& thisOption = options[i];

        bsl::vector<int> v;
        positions->push_back(v);

        balcl::OptionValue nullElement(thisOption.typeInfo().type());
        nullElement.setNull();

        data->push_back(nullElement);

        if (OptionInfo::e_NON_OPTION == thisOption.argType()) {
            nonOptionIndices->push_back(static_cast<int>(i));
        }
    }
}

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                                  COMPONENT
// ============================================================================

                          // -----------------
                          // class CommandLine
                          // -----------------

// PRIVATE MANIPULATORS
void CommandLine::clear()
{
    d_options.clear();
    d_positions.clear();
    d_nonOptionIndices.clear();
    d_state = e_INVALID;
    d_arguments.clear();
    d_schema.clear();
    d_data.clear();
    d_data1.clear();
    d_data2.clear();
    d_isBindin2Valid = false;
}

void CommandLine::initArgs(int argc, const char * const argv[])
{
    for (int i = 0; i < argc; ++i) {
        d_arguments.push_back(argv[i]);
    }
}

int CommandLine::parse(bsl::ostream& stream)
{
    BSLS_ASSERT(e_NOT_PARSED == d_state);

    bsl::size_t nonOptionRank = 0;
    for (unsigned int i = 1; i < d_arguments.size(); ++i) {
        // Implementation note: rather than a deeply-nested binary
        // 'if .. else', we opt for a sequence of 'if (...) { ...; continue; }'
        // to keep the level of indentation at one (or at most two).

        // 1) Parse non-option arguments.

        if ('-' != d_arguments[i][0]) {

            if (d_nonOptionIndices.size() <= nonOptionRank) {
                stream << "The argument \"" << d_arguments[i]
                       << "\" was unexpected." << '\n' << bsl::flush;
                location(stream, i);
                return -1;                                            // RETURN
            }

            bsl::size_t nonOptionIndex = d_nonOptionIndices[nonOptionRank];
            d_positions[nonOptionIndex].push_back(i);

            if (!TypeInfoUtil::parseAndValidate(
                                          &d_data[nonOptionIndex],
                                          d_arguments[i],
                                          d_options[nonOptionIndex].typeInfo(),
                                          stream)) {
                location(stream, i);
                return -1;                                            // RETURN
            }

            // Check for multiple values for the non-option argument.  (Note
            // that it already has been validated that an array-valued
            // non-option argument must be the last non-option argument on the
            // command line.)

            if (!d_options[nonOptionIndex].isArray()) {
                ++nonOptionRank;
            }

            continue;  // to next argument
        }

        // From now on 'd_arguments[i][0]' equals '-'.
        //
        // 2) Parse single '-' character token.

        if (0 == d_arguments[i][1]) {  // -
            stream << "The character \"-\" was unexpected." << '\n'
                    << bsl::flush;
            location(stream, i);
            return -1;                                                // RETURN
        }

        // 3) Parse non-option arguments following "--".

        if ('-' == d_arguments[i][1] && 0 == d_arguments[i][2]) {
            // Error if no nonOptions or all nonOptions already provided.

            if (d_nonOptionIndices.size() == nonOptionRank) {
                stream << "The argument \"--\" was unexpected." << '\n'
                       << bsl::flush;
                location(stream, i);
                return -1;                                            // RETURN
            }

            // Error if missing required option.  Note that we still have a
            // chance to provide values for required non-option arguments, so
            // do not trigger an error for those; they will be checked in
            // 'postParse'.

            int missingIndex = missing(false);
            if (missingIndex != -1) {
                stream << "No value supplied for the option \""
                       << d_options[missingIndex].tagString()
                       << "\"." << '\n' << bsl::flush;
                return -1;                                            // RETURN
            }

            for (++i; i < d_arguments.size(); ++i) {
                int nonOptionIndex = d_nonOptionIndices[nonOptionRank];
                if (!TypeInfoUtil::parseAndValidate(
                                          &d_data[nonOptionIndex],
                                          d_arguments[i],
                                          d_options[nonOptionIndex].typeInfo(),
                                          stream)) {
                    location(stream, i);
                    return -1;                                        // RETURN
                }
                d_positions[nonOptionIndex].push_back(i);

                if (!d_options[nonOptionIndex].isArray()) {
                    ++nonOptionRank;
                }
            }

            break;  // done with this command line
        }

        // 4) Parse long option: "--" followed by non-null 'char'.

        if ('-' == d_arguments[i][1]) {
            {
                const int temp = i;
                BSLS_ASSERT(0 != d_arguments[temp][2]);
                (void)temp;
            }

            if ('-' == d_arguments[i][2]) {
                stream << "Long option expected, instead of \"---\".\n"
                          "(Note: long options may not begin with '-'.)"
                       << '\n' << bsl::flush;
                location(stream, i);
                return -1;                                            // RETURN
            }

            // Find syntax used: "--option=value" or "--option value".

            const char *start = &d_arguments[i].c_str()[2];
            const char *s     = start;
            while (*s && *s != '=') {
                ++s;
            }

            int j     = i;   // index of value, either 'i' or 'i + 1'
            int index = -1;  // index of long option in 'd_options'

            if ('=' == *s) { // example: i:"--port=13"
                index = findTag(start, static_cast<int>(s - start));
            } else {         // example: i:"--port" i+1:"13"
                index = findTag(&d_arguments[i][2],
                                static_cast<int>(d_arguments[i].size() - 2));
            }

            // Check valid long option name.

            if (-1 == index) {
                stream << "The string \"" << bsl::string(start, s - start)
                       << "\" does not match any long tag." << '\n'
                       << bsl::flush;
                location(stream, i, 2, static_cast<int>(s - start + 1));
                return -1;                                            // RETURN
            }

            // Only now, compute string holding argument value.

            const Option&  thisOption = d_options[index];
            const char    *str;  // string for parsing value
            if ('=' == *s) {
                str = s + 1;
            } else {
                if (thisOption.argType() != OptionInfo::e_FLAG) {
                    if (i == d_arguments.size() - 1) {
                        stream << "Error: No value has been provided "
                                  "for the option \""
                               << thisOption.tagString()
                               << "\"." << '\n' << bsl::flush;
                        return -1;                                    // RETURN
                    }
                    str = d_arguments[j = i + 1].c_str();
                } else {
                    str = "";
                }
            }

            // Check for multiple values for the selected option.  Allow
            // multiple values for 'FLAG' options and array type options.

            if (!thisOption.isArray()
             && (OptionInfo::e_FLAG != thisOption.argType())
             && !d_positions[index].empty()) {
                stream << "Only one value can be provided for the option \""
                       << thisOption.tagString()
                       << "\"." << '\n' << bsl::flush;
                location(stream,
                         i,
                         2,
                         static_cast<int>(s - &d_arguments[i][0] - 1));
                return -1;                                            // RETURN
            }

            // Parse value for (non-flag) option, using either syntax.  (Note:
            // It is ok to use "--port=-1" to specify negative values.)

            if (!TypeInfoUtil::parseAndValidate(&d_data[index],
                                                str,
                                                thisOption.typeInfo(),
                                                stream)) {
                location(stream,
                         i,
                         static_cast<int>((s+1) - &d_arguments[i][0]),
                         static_cast<int>(d_arguments[i].size() - 1));
                return -1;                                            // RETURN
            }
            d_positions[index].push_back(i);

            i = j;     // advance if syntax '--option value' (not a flag)
            continue;  // ++i
        }

        // 5) Parse short option: e.g., -a...
        //
        // (Note: we allow to parse a series of short options concatenated but
        // have to stop whenever an option isn't a flag, as in Gnu tar, e.g.,
        // "-zcvf filename".)

        int         index;
        const char *s = &d_arguments[i].c_str()[1];
        while (*s && (index = findTag(*s)) != -1
            && OptionInfo::e_FLAG == d_options[index].argType()) {

            const Option& thisOption = d_options[index];

            // Allow multiply-specified bool options.  Parsing a bool value
            // should always succeed.

            if (!TypeInfoUtil::parseAndValidate(&d_data[index],
                                                "",
                                                thisOption.typeInfo(),
                                                stream)) {
                BSLS_ASSERT(0);

                return -1;                                            // RETURN
            }
            d_positions[index].push_back(i);
            ++s;
        }

        if (*s) {
            // Check exit condition of previous loop: either 'index == -1' or
            // the last option isn't a flag.

            if (-1 == index) {
                stream << "The character \""
                       << *s
                       << "\" does not match any short tag."
                       << '\n' << bsl::flush;
                location(
                    stream, i, static_cast<int>(s - d_arguments[i].c_str()));
                return -1;                                            // RETURN
            }

            // Process last short option, which has a value, using either
            // syntax "-ztvf=filename" or "-ztvf filename").

            int            j          = i;
            const char    *str        = s + 1;
            const Option&  thisOption = d_options[index];

            if (0 == *str) {
                if (i == d_arguments.size() - 1) {
                    stream << "Error: No value has been provided for "
                              "the option \""
                           << thisOption.tagString()
                           << "\"." << '\n' << bsl::flush;
                    return -1;                                        // RETURN
                }
                str = d_arguments[j = i + 1].c_str();
            }
            else {
                if ('=' == *str) {  // -bbbp=13 otherwise -bbbp13
                    ++str;
                }
            }

            // Check for multiple values for the selected option.

            if (!thisOption.isArray() && !d_positions[index].empty()) {
                stream << "Only one value can be provided for the option \"-"
                       << thisOption.tagString()
                       << "\"." <<  '\n' << bsl::flush;
                location(stream,
                         j,
                         static_cast<int>(str - d_arguments[j].c_str()));
                return -1;                                            // RETURN
            }

            // Parse value for (non-flag) option, using either syntax.
            //
            // (Note: It is ok to use "-p-1" or "-p=-1" to specify negative
            // values.)

            if (!TypeInfoUtil::parseAndValidate(&d_data[index],
                                                str,
                                                thisOption.typeInfo(),
                                                stream)) {
                location(stream,
                         j,
                         static_cast<int>(str - d_arguments[j].c_str()),
                         static_cast<int>(d_arguments[j].size() - 1));
                return -1;                                            // RETURN
            }

            d_positions[index].push_back(i);
            i = j;  // advance if syntax "-o value" (not a flag)
        }
    }

    return postParse(stream);
}

int CommandLine::postParse(bsl::ostream& stream)
{
    int index = missing();
    if (index >= 0) {
        const Option& thisOption = d_options[index];

        if (OptionInfo::e_NON_OPTION == thisOption.argType()) {
            stream << "Error: No value supplied for the non-option argument \""
                   << thisOption.name()
                   << "\"." << '\n' << bsl::flush;
        } else {
            stream << "Error: No value supplied for the option \""
                   << thisOption.tagString()
                   << "\"." << '\n' << bsl::flush;
        }
        return -1;                                                    // RETURN
    }

    for (unsigned int i = 0; i < d_options.size(); ++i) {
        const Option&         thisOption  = d_options[i];
        const TypeInfo&       info        = thisOption.typeInfo();
        const OccurrenceInfo& defaultInfo = thisOption.occurrenceInfo();

        CommandLine_SchemaData item = {info.type(), thisOption.name().c_str()};
        d_schema.push_back(item);

        if (!d_data[i].isNull()) {

            if (info.linkedVariable()) {
                u::OptionValueUtil::setValue(info.linkedVariable(), d_data[i]);
            }

            d_data1.push_back(d_data[i]);
        }
        else if (defaultInfo.hasDefaultValue()) {
            // Use default value to load linked variable and binding.
            //
            // This might be redundant, since 'data[i]' was set in
            // 'validateAndInitialize'.  But to be safe...

            if (info.linkedVariable()) {
                u::OptionValueUtil::setValue(info.linkedVariable(),
                                             defaultInfo.defaultValue());
            }

            d_data1.push_back(defaultInfo.defaultValue());
        }
        else {
            BSLS_ASSERT(!defaultInfo.isRequired());

            balcl::OptionValue element(thisOption.typeInfo().type());
            if (OptionInfo::e_FLAG == thisOption.argType()) {
                    element.set(false);
            } else {
                element.setNull();
            }
            d_data1.push_back(element);
        }
    }

    BSLS_ASSERT(d_data1.size() == d_schema.size());
    return 0;
}

void CommandLine::validateAndInitialize()
{
    CommandLine::validateAndInitialize(bsl::cerr);
}

void CommandLine::validateAndInitialize(bsl::ostream& stream)
{
    int status = u::validate(d_options, stream);

    if (0 != status) {
        EXIT_IF_FALSE(k_INVALID_COMMAND_LINE_SPEC);
    }

    u::initialize(&d_data, &d_positions, &d_nonOptionIndices, d_options);
}

// PRIVATE ACCESSORS
int CommandLine::findName(const bsl::string& name) const
{
    for (unsigned int i = 0; i < d_options.size(); ++i) {
        if (d_options[i].name() == name) {
            return i;                                                 // RETURN
        }
    }
    return -1;
}

int CommandLine::findTag(const char *longTag, bsl::size_t tagLength) const
{
    for (unsigned int i = 0; i < d_options.size(); ++i) {
        if (d_options[i].argType() != OptionInfo::e_NON_OPTION
         && !bsl::strncmp(d_options[i].longTag(), longTag, tagLength)
         && bsl::strlen(d_options[i].longTag()) == tagLength) {
            return i;                                                 // RETURN
        }
    }
    return -1;
}

int CommandLine::findTag(char shortTag) const
{
    for (unsigned int i = 0; i < d_options.size(); ++i) {
        if (d_options[i].argType() != OptionInfo::e_NON_OPTION
         && d_options[i].shortTag() == shortTag) {
            return i;                                                 // RETURN
        }
    }
    return -1;
}

void CommandLine::location(bsl::ostream& stream,
                           int           index,
                           int           start,
                           int           end) const
{
    if (end != -1) {
        stream << "The error occurred while parsing the "
               << u::Ordinal(start) << " to "
               << u::Ordinal(end) << " characters of the "
               << u::Ordinal(index - 1) << " argument." << '\n';
    }
    else if (start != -1) {
        stream << "The error occurred while parsing the "
               << u::Ordinal(start) << " character of the "
               << u::Ordinal(index - 1) << " argument." << '\n';
    }
    else {
        stream << "The error occurred while parsing the "
               << u::Ordinal(index - 1) << " argument." << '\n';
    }
    stream << bsl::flush;
}

int CommandLine::longestNonFlagNameSize() const
{
    int max = 0;
    for (unsigned int i = 0; i < d_options.size(); ++i) {
        if (d_options[i].argType() != OptionInfo::e_FLAG) {
            int len = static_cast<int>(d_options[i].name().size());
            if (len > max) {
                max = len;
            }
        }
    }
    return max;
}

int CommandLine::longestTagSize() const
{
    int max = 0;
    int len;
    for (unsigned int i = 0; i < d_options.size(); ++i) {
        if (d_options[i].argType() != OptionInfo::e_NON_OPTION
         && (len = static_cast<int>(bsl::strlen(d_options[i].longTag()))) >
                                                                         max) {
            max = len;
        }
    }
    return max;
}

int CommandLine::missing(bool checkAlsoNonOptions) const
{
    for (unsigned int i = 0; i < d_options.size(); ++i) {
        OptionInfo::ArgType argType = d_options[i].argType();
        if (0 == d_positions[i].size()  // no value specified
         && OptionInfo::e_FLAG != argType
         && d_options[i].occurrenceInfo().isRequired()
         && (OptionInfo::e_OPTION == argType || checkAlsoNonOptions)) {
            return i;                                                 // RETURN
        }
    }
    return -1;
}

// CLASS METHODS
bool CommandLine::isValidOptionSpecificationTable(const OptionInfo *specTable,
                                                  int               length,
                                                  bsl::ostream&     stream)
{
    BSLS_ASSERT(specTable);
    BSLS_ASSERT(0 <= length);

    bsl::vector<Option> options;  options.reserve(length);

    for (int i = 0; i < length; ++i) {
        options.push_back(Option(specTable[i]));
    }

    int status = u::validate(options, stream);

    return 0 == status;
}

// CREATORS
CommandLine::CommandLine(const OptionInfo *specTable,
                         int               length,
                         bslma::Allocator *basicAllocator)
: d_options(basicAllocator)
, d_positions(basicAllocator)
, d_nonOptionIndices(basicAllocator)
, d_state(e_INVALID)
, d_arguments(basicAllocator)
, d_schema(basicAllocator)
, d_data(basicAllocator)
, d_data1(basicAllocator)
, d_data2(basicAllocator)
, d_isBindin2Valid(false)
{
    d_options.reserve(length);
    for (int i = 0; i < length; ++i) {
        Option tmp(specTable[i]);
        d_options.push_back(tmp);
    }
    validateAndInitialize(bsl::cerr);
    d_state = e_NOT_PARSED;
}

CommandLine::CommandLine(const OptionInfo *specTable,
                         int               length,
                         bsl::ostream&     stream,
                         bslma::Allocator *basicAllocator)
: d_options(basicAllocator)
, d_positions(basicAllocator)
, d_nonOptionIndices(basicAllocator)
, d_state(e_INVALID)
, d_arguments(basicAllocator)
, d_schema(basicAllocator)
, d_data(basicAllocator)
, d_data1(basicAllocator)
, d_data2(basicAllocator)
, d_isBindin2Valid(false)
{
    d_options.reserve(length);
    for (int i = 0; i < length; ++i) {
        Option tmp(specTable[i]);
        d_options.push_back(tmp);
    }
    validateAndInitialize(stream);
    d_state = e_NOT_PARSED;
}

CommandLine::CommandLine(const CommandLine&  original,
                         bslma::Allocator   *basicAllocator)
: d_options(original.d_options, basicAllocator)
, d_positions(basicAllocator)
, d_nonOptionIndices(basicAllocator)
, d_state(e_INVALID)
, d_arguments(basicAllocator)
, d_schema(basicAllocator)
, d_data(basicAllocator)
, d_data1(basicAllocator)
, d_data2(basicAllocator)
, d_isBindin2Valid(false)
{
    BSLS_ASSERT(original.d_state != e_INVALID);

    // This isn't a typical copy constructor, because it has side-effects like
    // outputting to 'bsl::cerr'.  On the other hand, it could be replaced by a
    // more traditional one, because the assertions indicate that nothing
    // *should* be output to 'bsl::cerr'.  There are no other reasons to have
    // such a complicated implementation.  This should be checked in the test
    // driver.

    validateAndInitialize(bsl::cerr);
    d_state = e_NOT_PARSED;
    if (e_PARSED == original.d_state) {
        d_arguments = original.d_arguments;
        const int status = parse(bsl::cerr);

        BSLS_ASSERT(0 == status);  (void)status;

        d_state = e_PARSED;
    }
}

CommandLine::~CommandLine()
{
}

// MANIPULATORS
CommandLine& CommandLine::operator=(const CommandLine& rhs)
{
    BSLS_ASSERT(d_state != e_INVALID);

    if (&rhs != this) {
        BSLS_ASSERT(rhs.d_state != e_INVALID);

        clear();

        d_options.reserve(rhs.d_options.size());
        for (unsigned int i = 0; i < rhs.d_options.size(); ++i) {
            d_options.push_back(rhs.d_options[i]);
        }

        validateAndInitialize(bsl::cerr);
        d_state = e_NOT_PARSED;

        if (e_PARSED == rhs.d_state) {
            d_arguments = rhs.d_arguments;
            const int status = parse(bsl::cerr);

            BSLS_ASSERT(0 == status);  (void)status;

            d_state = e_PARSED;
        }
    }
    return *this;
}

int CommandLine::parse(int argc, const char * const argv[])
{
    return parse(argc, argv, bsl::cerr);
}

int CommandLine::parse(int                argc,
                       const char * const argv[],
                       bsl::ostream&      stream)
{
    initArgs(argc, argv);

    if (parse(stream)) {
        d_state = e_INVALID;
        return -1;                                                    // RETURN
    }
    d_state = e_PARSED;
    return 0;
}

// ACCESSORS
bool CommandLine::isParsed() const
{
    return e_PARSED == d_state;
}

bool CommandLine::isSpecified(const bsl::string& name) const
{
    return numSpecified(name) > 0;
}

bool CommandLine::isSpecified(const bsl::string& name, int *count) const
{
    const int n = numSpecified(name);
    if (n > 0 && count) {
        *count = n;
    }
    return n > 0;
}

bool CommandLine::isValid() const
{
    return e_INVALID != d_state;
}

int CommandLine::numSpecified(const bsl::string& name) const
{
    const int index = findName(name);
    return static_cast<int>(0 <= index ? d_positions[index].size() : 0);
}

CommandLineOptionsHandle CommandLine::options() const
{
    BSLS_ASSERT(e_PARSED == d_state);

    return CommandLineOptionsHandle(&d_data1, &d_schema);
}

int CommandLine::position(const bsl::string& name) const
{
    // The behavior is undefined unless 'name' exists and it is not an array.
    // -1 is returned if option is never entered on command line (possible in
    // case of flags, or options with default values).

    const int index = findName(name);

    BSLS_ASSERT(index >= 0);
    BSLS_ASSERT(!d_options[index].isArray());

    return 0 == d_positions[index].size() ? -1 : d_positions[index][0];
}

const bsl::vector<int>& CommandLine::positions(const bsl::string& name) const
{
    const int index = findName(name);

    BSLS_ASSERT(index >= 0);

    return d_positions[index];
}

void CommandLine::printUsage() const
{
    printUsage(bsl::cerr);
}

CommandLineOptionsHandle CommandLine::specifiedOptions() const
{
    BSLS_ASSERT(e_PARSED == d_state);

    if (!d_isBindin2Valid) {
        // This is a 'const' method that needs to "cache" the values of
        // 'd_data2', which is therefore declared 'mutable' along with the
        // 'd_isBindin2Valid' flag.  This is acceptable because those members
        // will never change after being initialized, unless 'clear' is called
        // by 'operator=', and thus will remain "in sync" with the value of
        // this command line object.

        d_isBindin2Valid = true;
        d_data2          = d_data1;

        for (bsl::size_t index = 0; index < d_data1.size(); ++index) {
            if (!isSpecified(d_options[index].name())) {
                balcl::OptionValue nullElement(
                                                        d_data1[index].type());
                nullElement.setNull();

                d_data2[index] = nullElement;
            }
        }
    }
    return CommandLineOptionsHandle(&d_data2, &d_schema);
}

                        // 'the*' Accessors

// BDE_VERIFY pragma: -FABC01  // not in alphabetic order
bool CommandLine::theBool(const bsl::string& name) const
{
    const int index = findName(name);

    BSLS_ASSERT(index >= 0);
    BSLS_ASSERT(
          OptionInfo::e_FLAG == d_options[index].argType());
    (void)index;

    return options().the<OptionType::Bool>(name.c_str());
}

char CommandLine::theChar(const bsl::string& name) const
{
    return options().the<OptionType::Char>(name.c_str());
}

int CommandLine::theInt(const bsl::string& name) const
{
    return options().the<OptionType::Int>(name.c_str());
}

bsls::Types::Int64 CommandLine::theInt64(const bsl::string& name) const
{
    return options().the<OptionType::Int64>(name.c_str());
}

double CommandLine::theDouble(const bsl::string& name) const
{
    return options().the<OptionType::Double>(name.c_str());
}

const bsl::string& CommandLine::theString(const bsl::string& name) const
{
    return options().the<OptionType::String>(name.c_str());
}

const bdlt::Datetime& CommandLine::theDatetime(const bsl::string& name) const
{
    return options().the<OptionType::Datetime>(name.c_str());
}

const bdlt::Date& CommandLine::theDate(const bsl::string& name) const
{
    return options().the<OptionType::Date>(name.c_str());
}

const bdlt::Time& CommandLine::theTime(const bsl::string& name) const
{
    return options().the<OptionType::Time>(name.c_str());
}

const bsl::vector<char>&
CommandLine::theCharArray(const bsl::string& name) const
{
    return options().the<OptionType::CharArray>(name.c_str());
}

const bsl::vector<int>&
CommandLine::theIntArray(const bsl::string& name) const
{
    return options().the<OptionType::IntArray>(name.c_str());
}

const bsl::vector<bsls::Types::Int64>&
CommandLine::theInt64Array(const bsl::string& name) const
{
    return options().the<OptionType::Int64Array>(name.c_str());
}

const bsl::vector<double>&
CommandLine::theDoubleArray(const bsl::string& name) const
{
    return options().the<OptionType::DoubleArray>(name.c_str());
}

const bsl::vector<bsl::string>&
CommandLine::theStringArray(const bsl::string& name) const
{
    return options().the<OptionType::StringArray>(name.c_str());
}

const bsl::vector<bdlt::Datetime>&
CommandLine::theDatetimeArray(const bsl::string& name) const
{
    return options().the<OptionType::DatetimeArray>(name.c_str());
}

const bsl::vector<bdlt::Date>&
CommandLine::theDateArray(const bsl::string& name) const
{
    return options().the<OptionType::DateArray>(name.c_str());
}

const bsl::vector<bdlt::Time>&
CommandLine::theTimeArray(const bsl::string& name) const
{
    return options().the<OptionType::TimeArray>(name.c_str());
}
// BDE_VERIFY pragma: +FABC01  // not in alphabetic order
                                  // Aspects

bslma::Allocator *CommandLine::allocator() const
{
    return d_options.get_allocator().mechanism();
}

void CommandLine::printUsage(bsl::ostream& stream) const
{
    bsl::string usage("Usage: ");
    usage.append(d_arguments.size() ? d_arguments[0] : "programName");
    usage.append(1, ' ');

    // Prepare array of options, formatted, with optional flags grouped in
    // first.

    bsl::vector<bsl::string> options;
    bsl::vector<bsl::string> nonOptions;

    for (unsigned int i = 0; i < d_options.size(); ++i) {
        switch (d_options[i].argType()) {
          case OptionInfo::e_FLAG:
          case OptionInfo::e_OPTION: {
            if (d_options[i].occurrenceInfo().isHidden()) {
                break;
            }
            char start = 0, end = 0, multiIndicator = 0;
            if (d_options[i].isArray()
             || !d_options[i].occurrenceInfo().isRequired()) {
                start = '[';
                end   = ']';
            }
            if (d_options[i].isArray()) {
                multiIndicator =
                        d_options[i].occurrenceInfo().isRequired() ? '+' : '*';
            }

            options.push_back("");
            bsl::string& latest = options.back();
            if (start) {
                latest.append(1, start);
            }
            if (d_options[i].shortTag()) {
                latest.append(1, '-');
                latest.append(1, d_options[i].shortTag());
                latest.append(1, '|');
            } else {
                latest.append(2, '-');
            }
            latest.append(d_options[i].longTag());
            if (OptionInfo::e_FLAG !=
                                                      d_options[i].argType()) {
                latest.append(1, ' ');
                latest.append(1, '<');
                latest.append(d_options[i].name());
                latest.append(1, '>');
            }
            if (end) {
                latest.append(1, end);
            }
            if (multiIndicator) {
                latest.append(1, multiIndicator);
            }
          } break;                                                     // BREAK
          case OptionInfo::e_NON_OPTION: {
            char start = 0, end = 0, multiIndicator = 0;

            if (d_options[i].isArray()
             || !d_options[i].occurrenceInfo().isRequired()) {
                start = '[';
                end = ']';
            }

            if (d_options[i].isArray()) {
                multiIndicator =
                      !d_options[i].occurrenceInfo().isRequired() ? '*' : '+';
            }

            nonOptions.push_back("");
            bsl::string& latest = nonOptions.back();
            if (start) {
                latest.append(1, start);
            }
            latest.append(1, '<');
            latest.append(d_options[i].name());
            latest.append(1, '>');
            if (end) {
                latest.append(1, end);
            }
            if (multiIndicator) {
                latest.append(1, multiIndicator);
            }
          } break;                                                     // BREAK
          default: {
            BSLS_ASSERT(0);
          } break;
        }
    }
    options.insert(options.end(), nonOptions.begin(), nonOptions.end());

    int start = 2;
    int end   = 80;
    stream << '\n';
    stream << usage;
    u::format(static_cast<int>(usage.size()),
              end,
              options,
              stream,
              static_cast<int>(usage.size()));

    stream << "\nWhere:\n";

    bsl::string temp;
    temp.append(start, ' ');
    temp.append(1, '-');
    temp.append(1, 'X');
    temp.append(" | --");

    const int k_NAME_START_POSITION =
                          static_cast<int>(temp.size() + longestTagSize() + 2);
    const int k_DESCRIPTION_START_POSITION =
                      k_NAME_START_POSITION + 1 + longestNonFlagNameSize() + 3;

    // If more that 30 characters of the 80 character line have already been
    // consumed by the tag and name then begin the descriptions on the next
    // line with an 8 space indent to avoid having an excessively wrapped
    // description.

    const int  k_NEW_LINE_LIMIT       = 30;
    const int  k_NEW_LINE_INDENTATION = start + 8;
    const bool k_DESCRIPTION_NEW_LINE =
                             (k_DESCRIPTION_START_POSITION > k_NEW_LINE_LIMIT);

    for (unsigned int i = 0; i < d_options.size(); ++i) {
        if (d_options[i].argType() !=
                                 OptionInfo::e_NON_OPTION) {
            if (d_options[i].occurrenceInfo().isHidden()) {
                continue;
            }
            bsl::string str;
            str.append(start, ' ');
            if (d_options[i].shortTag()) {
                str.append(1, '-');
                str.append(1, d_options[i].shortTag());
                str.append(  " | --");
            } else {
                str.append("     --");
            }
            str.append(d_options[i].longTag());

            if (OptionInfo::e_OPTION ==
                                                      d_options[i].argType()) {
                str.append(k_NAME_START_POSITION - str.size(), ' ');
                str.append(1, '<').append(d_options[i].name()).append(1, '>');
            }

            if (k_DESCRIPTION_NEW_LINE) {
                str.append("\n");
            } else {
                str.append(k_DESCRIPTION_START_POSITION - str.size(), ' ');
            }
            stream << str;

            str = d_options[i].description();
            if (OptionInfo::e_OPTION == d_options[i].argType()
             && d_options[i].occurrenceInfo().hasDefaultValue()) {
                bsl::ostringstream oss;
                oss << d_options[i].occurrenceInfo().defaultValue();
                str += " (default: " + oss.str() + ')';
            }
            if (k_DESCRIPTION_NEW_LINE) {
                u::format(k_NEW_LINE_INDENTATION, end, str, stream);
            } else {
                u::format(k_DESCRIPTION_START_POSITION,
                          end,
                          str,
                          stream,
                          k_DESCRIPTION_START_POSITION);
            }
            stream << '\n' << bsl::flush;
        }
        else {
            // As we concentrate on non-option arguments at the end of the
            // 'Usage' line, we want to preserve the same order in the 'Where'
            // lines.

            continue;
        }
    }

    // And now we print non-option argument descriptions.

    for (unsigned int i = 0; i < d_options.size(); ++i) {
        if (d_options[i].argType() ==
                                    OptionInfo::e_NON_OPTION) {
            bsl::string str;
            str.append(k_NAME_START_POSITION, ' ');
            str.append(1, '<');
            str.append(d_options[i].name());
            str.append(1, '>');
            if (k_DESCRIPTION_NEW_LINE) {
                str.append("\n");
            } else {
                str.append(k_DESCRIPTION_START_POSITION - str.size(), ' ');
            }
            stream << str;

            str = d_options[i].description();
            if (d_options[i].occurrenceInfo().hasDefaultValue()) {
                bsl::ostringstream oss;
                oss << d_options[i].occurrenceInfo().defaultValue();
                str += " (default: " + oss.str() + ')';
            }
            if (k_DESCRIPTION_NEW_LINE) {
                u::format(k_NEW_LINE_INDENTATION, end, str, stream);
            } else {
                u::format(k_DESCRIPTION_START_POSITION,
                          end,
                          str,
                          stream,
                          k_DESCRIPTION_START_POSITION);
            }
            stream << '\n' << bsl::flush;
        }
    }
}

bsl::ostream& CommandLine::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    BSLS_ASSERT(0 <= spacesPerLevel);

    if (e_PARSED == d_state) {
        bslim::Printer printer(&stream, level, spacesPerLevel);
        printer.start();
        for (bsl::size_t i = 0; i < d_data1.size(); ++i) {
            printer.printAttribute(d_schema[i].d_name_p, d_data1[i]);
        }
        printer.end();
    }
    else {
        bdlb::Print::indent(stream, level, spacesPerLevel);
        const char *output = e_INVALID == d_state
                           ? "INVALID\n"
                           : "UNPARSED\n";
        stream << output;
    }
    return stream;
}

}  // close package namespace

// FREE OPERATORS
bool balcl::operator==(const CommandLine& lhs, const CommandLine& rhs)
{
    return lhs.isParsed() && rhs.isParsed() && lhs.options() == rhs.options();
}

bool balcl::operator!=(const CommandLine& lhs, const CommandLine& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& balcl::operator<<(bsl::ostream&     stream,
                               const CommandLine& rhs)
{
    // NOTE: this is *not* the standard implementation for 'operator<<', which
    // is usually:
    //..
    //  rhs.print(stream, 0, -1);
    //..
    // But we depart from the standard for this component because a one-line
    // format would really not be human-readable.

    return rhs.print(stream);
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
