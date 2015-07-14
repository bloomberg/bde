// balb_xxxcommandline.cpp                                               -*-C++-*-
#include <balb_xxxcommandline.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balb_xxxcommandline_cpp,"$Id$ $CSID$")

///IMPLEMENTATION NOTES: This is a somewhat unique component in that it lives
// only in offlines, and is most often used with command-line specs that *must*
// *be* a statically-created array.  For that reason, there are two classes
// that serve the same purpose: 'balb::CommandLineOptionInfo' is a
// statically-initializable class and violates the 'bdema' allocator protocol,
// while 'balb::CommandLineOption' is convertible from
// 'balb::CommandLineOptionInfo' takes allocators and is suitable for storing
// into containers.
//
// As a result, Purify gets somewhat confused about UMRs, memory leaks, and
// ABRs.  Moreover, in optimized mode, Purify would also get confused about the
// ordering of the variables and crash with core dumped...  Somehow, adding
// 'volatile' to the loop variables seemed to fix the issue, we still don't
// know why.  This is fragile and it would be a good idea to keep purifying the
// test driver for each release and see if the crash re-occurs.
//
// Another note: the class 'balb::CommandLineOptionType' contains static
// initializers (see 'CLASS DATA' below) which are null pointers of the
// respective types, and are used only for readability of the
// 'balb::CommandLineTypeInfo' constructor calls (specifying the type of an
// option without a linked variable).

#include <bdlmxxx_aggregateoption.h>
#include <bdlmxxx_descriptor.h>
#include <bdlmxxx_elemattrlookup.h>
#include <bdlmxxx_properties.h>
#include <bdlmxxx_schemaaggregateutil.h>

#include <bdlpuxxx_typesparser.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_time.h>

#include <bdlb_print.h>

#include <bslalg_autoscalardestructor.h>
#include <bslalg_scalardestructionprimitives.h>
#include <bslalg_scalarprimitives.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_cstring.h>

namespace BloombergLP {

#define EXIT_IF_FALSE(X) do {                                             \
    if (!(X)) {                                                           \
        BloombergLP::bsls::Assert::invokeHandler(#X, __FILE__, __LINE__); \
    }                                                                     \
} while (false)
    // This macro is enabled *unconditionally*.  Note that 'BSLS_ASSERT'
    // can be disabled for certain build targets (e.g., optimized non-debug),
    // and is not appropriate where the control flow actually demands to abort,
    // throw, or whatever the assertion handler is configured to do (such as in
    // validating the command-line spec in the command-line constructor).  Note
    // that we don't want to hard-code an 'abort' there, to retain testability
    // (the test driver should verify good behavior by changing the assertion
    // handler to throw a custom exception, pass in invalid command-line specs,
    // and make sure it catches that exception).

enum {
    // This 'enum' is used in place of 0 for readability in the assertion
    // macros.

    INVALID_COMMAND_LINE_SPEC = 0  // must be 0 to trigger an assert
};

// ===========================================================================
//                       LOCAL CLASS AND FUNCTION DEFINITIONS
// ===========================================================================

namespace {

// STATIC DATA
const char *recordName = "COMMAND_LINE_PARAMETERS";
    // This identifier gives a unique name to the record in the 'bdem' schema
    // used to create the 'bdem_ConstRowBinding' containing the option names
    // and their values in this command line.

                        // =============================
                        // local functions format(. . .)
                        // =============================

void format(int                             start,
            int                             end,
            const bsl::vector<bsl::string>& strings,
            bsl::ostream&                   stream,
            int                             col = 0)
    // Output the specified 'strings' sequence of words to the specified
    // 'stream', separated by spaces, formatted to fit between the columns at
    // the specified 'start' and 'end' indices in a greedy fashion, and
    // returning to a new line prefixed by 'start' spaces in case the words
    // would write into the 'end' column.  Note that a single word of length
    // larger than 'end - start' characters is printed on a single line at the
    // 'start' position, and is the only possibility for this stream to reach
    // or exceed 'end' characters on a single line.
{
    if (0 == strings.size()) {
        return;
    }

    if (col < start) {
        stream << bsl::string(start - col, ' ') << strings[0];
        col = start + strings[0].size();
    } else {
        stream << strings[0];
        col += strings[0].size();
    }

    for (unsigned int i = 1; i < strings.size(); ++i) {
        if (strings[i].empty()) {
            continue;
        }
        if (col + 1 + (int)strings[i].size() >= end) {
            stream << '\n' << bsl::string(start, ' ') << strings[i];
            col = start + strings[i].size();
        } else {
            stream << ' ' << strings[i];
            col += 1 + strings[i].size();
        }
    }
}

void format(int                start,
            int                end,
            const bsl::string& string,
            bsl::ostream&      stream,
            int                col = 0)
    // Format the specified 'string' to the specified 'stream', with words
    // separated by spaces, formatted to fit between the columns at the
    // specified 'start' and 'end' indices as specified by the previous
    // function (in particular, returning to a new line prefixed by 'start'
    // spaces in case the words would write into the 'end' column).  Note that
    // words are delimited by either a space, tab, or newline character, but
    // are rendered as separated by spaces (thus the original separating
    // character is lost).
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

}  // close unnamed namespace

namespace balb {
                            // =========================
                            // local struct Ordinal
                            // =========================

struct Ordinal {
    // This 'struct' assists in printing numbers as ordinals (1st, 2nd, etc.).

    bsl::size_t d_rank;  // rank (starting at 0)

    Ordinal(bsl::size_t n) : d_rank(n) {}
        // Create an ordinal for the specified position 'n' (starting at 0).
};

bsl::ostream& operator<<(bsl::ostream& stream, Ordinal position);
}  // close package namespace
    // Output the specified 'position' (starting at 0) to the specified
    // 'stream' as an ordinal, mapping 0 to "1st", 1 to "2nd", 3 to "3rd", 4
    // to "4th", etc. following correct English usage.

bsl::ostream& balb::operator<<(bsl::ostream& stream, Ordinal position)
{
    int n = position.d_rank + 1;  // ranks start at 0, but are displayed as
                                  // 1st, 2nd, etc.
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

namespace {
                         // =========================
                         // local function parseValue
                         // =========================

bool parseValue(void *value, const char *input, bdlmxxx::ElemType::Type type)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(input);

    bool result;
    const char *end;

    switch (type) {
      case bdlmxxx::ElemType::BDEM_CHAR: {
        *(char *)value = *input;
        result = *input;  // succeeds if non-null character
      } break;
      case bdlmxxx::ElemType::BDEM_SHORT: {
        result = !bdlpuxxx::TypesParser::parseShort(&end, (short *)value, input);
      } break;
      case bdlmxxx::ElemType::BDEM_INT: {
        result = !bdlpuxxx::TypesParser::parseInt(&end, (int *)value, input);
      } break;
      case bdlmxxx::ElemType::BDEM_INT64: {
        result = !bdlpuxxx::TypesParser::parseInt64(&end,
                                                (bsls::Types::Int64 *)value,
                                                input);
      } break;
      case bdlmxxx::ElemType::BDEM_FLOAT: {
        result = !bdlpuxxx::TypesParser::parseFloat(&end, (float *)value, input);
      } break;
      case bdlmxxx::ElemType::BDEM_DOUBLE: {
        result = !bdlpuxxx::TypesParser::parseDouble(&end, (double *)value, input);
      } break;
      case bdlmxxx::ElemType::BDEM_STRING: {
        *(bsl::string *)value = input;
        result = true;
      } break;
      case bdlmxxx::ElemType::BDEM_DATETIME: {
        result = !bdlpuxxx::TypesParser::parseDatetime(&end,
                                                   (bdlt::Datetime *)value,
                                                   input);
      } break;
      case bdlmxxx::ElemType::BDEM_DATE: {
        result = !bdlpuxxx::TypesParser::parseDate(&end,
                                               (bdlt::Date *)value,
                                               input);
      } break;
      case bdlmxxx::ElemType::BDEM_TIME: {
        result = !bdlpuxxx::TypesParser::parseTime(&end,
                                               (bdlt::Time *)value,
                                               input);
      } break;
      default: {
        BSLS_ASSERT(0);
        result = false;
      } break;
    }
    return result;
}

                         // ===============================
                         // local function elemTypeToString
                         // ===============================

const char *elemTypeToString(bdlmxxx::ElemType::Type elemType)
{
    const char *typeString;

    switch (elemType) {
      case bdlmxxx::ElemType::BDEM_CHAR: {
        typeString = "A char";
      } break;
      case bdlmxxx::ElemType::BDEM_SHORT: {
        typeString = "A short";
      } break;
      case bdlmxxx::ElemType::BDEM_INT: {
        typeString = "An integer";
      } break;
      case bdlmxxx::ElemType::BDEM_INT64: {
        typeString = "A 64-bit integer";
      } break;
      case bdlmxxx::ElemType::BDEM_FLOAT: {
        typeString = "A float";
      } break;
      case bdlmxxx::ElemType::BDEM_DOUBLE: {
        typeString = "A double";
      } break;
      case bdlmxxx::ElemType::BDEM_STRING: {
        typeString = "A string";
      } break;
      case bdlmxxx::ElemType::BDEM_DATETIME: {
        typeString = "A 'bdlt::Datetime'";
      } break;
      case bdlmxxx::ElemType::BDEM_DATE: {
        typeString = "A 'bdlt::Date'";
      } break;
      case bdlmxxx::ElemType::BDEM_TIME: {
        typeString = "A 'bdlt::Time'";
      } break;
      default: {
        BSLS_ASSERT(0);
        typeString = "An unknown type";
      } break;
    }

    return typeString;
}

}  // close unnamed namespace

namespace balb {
                   // =======================================
                   // local class CommandLine_Constraint
                   // =======================================

class CommandLine_Constraint {

  public:
    // CREATORS
    virtual ~CommandLine_Constraint();
        // Destroy this polymorphic constraint object.

    // ACCESSORS
    virtual bdlmxxx::ElemType::Type type() const = 0;
        // Return the 'bdem' element type of this constraint object.

    virtual bool parse(const bdlmxxx::ElemRef& element,
                       bsl::ostream&       stream,
                       const bsl::string&  input) const = 0;
        // Load into the specified 'element' the result of parsing the
        // specified 'input', parsed as a value of the 'bdem' element type of
        // this constraint, and output to the specified 'stream' any error
        // message.  Return 'true' if this value, parsed without error,
        // satisfies this constraint, and 'false' if parsing fails or the value
        // does not satisfy the constraint.  Note that, if parsing fails,
        // 'element' is unchanged.  The behavior is undefined unless 'element'
        // has the same type as this constraint.

    virtual bool validate(const bdlmxxx::ConstElemRef& element) const = 0;
    virtual bool validate(const bdlmxxx::ConstElemRef& element,
                          bsl::ostream&            stream) const = 0;
        // Return 'true' if the specified 'element' satisfies this constraint,
        // and 'false' otherwise.  Optionally specify a 'stream' to which a
        // descriptive error message is written if this constraint is not
        // satisfied.  The behavior is undefined unless 'element' is bound to a
        // variable of the same type as this constraint.
};

// CREATORS
CommandLine_Constraint::~CommandLine_Constraint()
{
}
}  // close package namespace

namespace {

                         // ==========================
                         // local class BoolConstraint
                         // ==========================

class BoolConstraint : public balb::CommandLine_Constraint {
    // This concrete implementation of the 'balb::CommandLine_Constraint'
    // protocol provides a 'bool' empty constraint which always returns 'true'
    // after this option has been parsed.  The individual contracts for each
    // method are identical to the protocol and not repeated here.

    // NOT IMPLEMENTED
    BoolConstraint(const BoolConstraint&);
    BoolConstraint& operator=(const BoolConstraint&);

  public:
    // CREATORS
    BoolConstraint(bslma::Allocator *basicAllocator = 0);
        // Create a 'bool' constraint object.  Optionally specify a
        // 'basicAllocator', which is ignored.

    ~BoolConstraint();
        // Destroy this object.

    // ACCESSORS
    bdlmxxx::ElemType::Type type() const;
        // Return 'bdlmxxx::ElemType::BDEM_BOOL'.

    bool parse(const bdlmxxx::ElemRef& element,
               bsl::ostream&       stream,
               const bsl::string&  input) const;
        // Store the value 'true' into the specified 'element'.

    bool validate(const bdlmxxx::ConstElemRef& element) const;
    bool validate(const bdlmxxx::ConstElemRef& element,
                  bsl::ostream&            stream) const;
        // Return 'true', and leave the optionally specified 'stream'
        // unchanged.  The behavior is undefined unless 'element' is bound to a
        // variable of 'bool' type.
};

// CREATORS
BoolConstraint::BoolConstraint(bslma::Allocator *)
{
}

BoolConstraint::~BoolConstraint()
{
}

// ACCESSORS
bdlmxxx::ElemType::Type BoolConstraint::type() const
{
    return bdlmxxx::ElemType::BDEM_BOOL;
}

bool BoolConstraint::parse(const bdlmxxx::ElemRef& element,
                           bsl::ostream&       ,
                           const bsl::string&) const
{
    BSLS_ASSERT(element.isBound());
    BSLS_ASSERT(bdlmxxx::ElemType::BDEM_BOOL == element.type());

    element.theModifiableBool() = true;
    return true;
}

bool BoolConstraint::validate(const bdlmxxx::ConstElemRef&) const
{
    return true;
}

bool BoolConstraint::validate(const bdlmxxx::ConstElemRef&, bsl::ostream&) const
{
    return true;
}

                   // =================================
                   // local class ScalarConstraint<...>
                   // =================================

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
class ScalarConstraint : public balb::CommandLine_Constraint {
    // This concrete implementation of the 'balb::CommandLine_Constraint'
    // protocol provides a constraint of the parameterized 'CONSTRAINT_TYPE',
    // which enables the parsing and validation of values of parameterized
    // 'TYPE'.

    // DATA
    CONSTRAINT_TYPE d_constraint;

    // NOT IMPLEMENTED
    ScalarConstraint(const ScalarConstraint&);
    ScalarConstraint& operator=(const ScalarConstraint&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(ScalarConstraint,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    ScalarConstraint(const CONSTRAINT_TYPE&  constraint,
                     bslma::Allocator       *basicAllocator = 0);
        // Create a constraint object storing the specified 'constraint'
        // functor.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently-installed default
        // allocator is used.

    ~ScalarConstraint();
        // Destroy this polymorphic constraint object.

    // ACCESSORS
    bdlmxxx::ElemType::Type type() const;
        // Return the 'bdem' element type of this constraint.

    bool parse(const bdlmxxx::ElemRef& element,
               bsl::ostream&       stream,
               const bsl::string&  input) const;
        // Load into the instance of parameterized 'TYPE' stored in the
        // specified 'element' the result of parsing the specified 'input',
        // interpreted as an instance of 'TYPE'.  Return 'true' if parsing
        // succeeds and the parsed value satisfies the constraint.  Return
        // 'false' otherwise, and write to the specified 'stream' a descriptive
        // error message.  The behavior is undefined unless 'element' is bound
        // to a valid instance of 'TYPE'.

    bool validate(const bdlmxxx::ConstElemRef& element) const;
    bool validate(const bdlmxxx::ConstElemRef& element,
                  bsl::ostream&            stream) const;
        // Return 'true' if the specified 'element' satisfies this constraint,
        // and 'false' otherwise.  Optionally specify a 'stream' to which a
        // descriptive error message is written if this constraint is not
        // satisfied.  The behavior is undefined unless 'element' is bound to a
        // variable of the same type as this constraint.
};

// CREATORS
template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
ScalarConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::ScalarConstraint(
                                        const CONSTRAINT_TYPE&  constraint,
                                        bslma::Allocator       *basicAllocator)
: d_constraint(constraint, basicAllocator)
{
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
ScalarConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::~ScalarConstraint()
{
}

// ACCESSORS
template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bdlmxxx::ElemType::Type
ScalarConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::type() const
{
    return bdlmxxx::ElemType::Type(ELEM_TYPE);
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bool
ScalarConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::parse(
                                               const bdlmxxx::ElemRef& element,
                                               bsl::ostream&       stream,
                                               const bsl::string&  input) const
{
    const bdlmxxx::ElemType::Type elemType = element.type();

    BSLS_ASSERT(element.isBound());
    BSLS_ASSERT(ELEM_TYPE == elemType);

    TYPE value = TYPE();  // initialize to quell purify UMR
    if (!parseValue(&value, input.c_str(), elemType)) {
        stream << elemTypeToString(elemType)
               << " value was expected, instead of \""
               << input << "\"" << bsl::endl;
        return false;
    }
    const bdlmxxx::Descriptor *desc = bdlmxxx::ElemAttrLookup::lookupTable()[elemType];
    element.replaceValue(bdlmxxx::ConstElemRef(&value, desc));
    return validate(element, stream);
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bool
ScalarConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::validate(
                                        const bdlmxxx::ConstElemRef& element) const
{
    bsl::ostringstream oss;
    return validate(element, oss);
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bool
ScalarConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::validate(
                                         const bdlmxxx::ConstElemRef& element,
                                         bsl::ostream&            stream) const
{
    BSLS_ASSERT(element.isBound());
    BSLS_ASSERT(ELEM_TYPE == element.type());

    return d_constraint
         ? d_constraint((const TYPE *)element.data(), stream)
         : true;
}

// SPECIALIZATIONS
template
class ScalarConstraint<char,
                       balb::CommandLineConstraint::CharConstraint,
                       bdlmxxx::ElemType::BDEM_CHAR>;
template
class ScalarConstraint<short,
                       balb::CommandLineConstraint::ShortConstraint,
                       bdlmxxx::ElemType::BDEM_SHORT>;
template
class ScalarConstraint<int,
                       balb::CommandLineConstraint::IntConstraint,
                       bdlmxxx::ElemType::BDEM_INT>;
template
class ScalarConstraint<bsls::Types::Int64,
                       balb::CommandLineConstraint::Int64Constraint,
                       bdlmxxx::ElemType::BDEM_INT64>;
template
class ScalarConstraint<float,
                       balb::CommandLineConstraint::FloatConstraint,
                       bdlmxxx::ElemType::BDEM_FLOAT>;
template
class ScalarConstraint<double,
                       balb::CommandLineConstraint::DoubleConstraint,
                       bdlmxxx::ElemType::BDEM_DOUBLE>;
template
class ScalarConstraint<bsl::string,
                       balb::CommandLineConstraint::StringConstraint,
                       bdlmxxx::ElemType::BDEM_STRING>;
template
class ScalarConstraint<bdlt::Datetime,
                       balb::CommandLineConstraint::DatetimeConstraint,
                       bdlmxxx::ElemType::BDEM_DATETIME>;
template
class ScalarConstraint<bdlt::Date,
                       balb::CommandLineConstraint::DateConstraint,
                       bdlmxxx::ElemType::BDEM_DATE>;
template
class ScalarConstraint<bdlt::Time,
                       balb::CommandLineConstraint::TimeConstraint,
                       bdlmxxx::ElemType::BDEM_TIME>;
    // Explicit specializations (to force instantiation of all variants).

                 // ================================
                 // local class ArrayConstraint<...>
                 // ================================

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
class ArrayConstraint : public balb::CommandLine_Constraint {
    // This concrete implementation of the 'balb::CommandLine_Constraint'
    // protocol provides a constraint of the parameterized 'CONSTRAINT_TYPE',
    // which enables the parsing and validation of values of parameterized
    // 'bsl::vector<TYPE>' type.

    // DATA
    CONSTRAINT_TYPE d_constraint;

    // NOT IMPLEMENTED
    ArrayConstraint(const ArrayConstraint&);
    ArrayConstraint& operator=(const ArrayConstraint&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(ArrayConstraint,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    ArrayConstraint(const CONSTRAINT_TYPE&  constraint,
                    bslma::Allocator       *basicAllocator);
        // Create a constraint object storing the specified 'constraint'
        // functor.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently-installed default
        // allocator is used.

    ~ArrayConstraint();
        // Destroy this polymorphic constraint object.

    // ACCESSORS
    bdlmxxx::ElemType::Type type() const;
        // Return the 'bdem' element type of this constraint.

    bool parse(const bdlmxxx::ElemRef& element,
               bsl::ostream&       stream,
               const bsl::string&  input) const;
        // Append to the instance of 'bsl::vector<TYPE>' stored in the
        // specified 'element' the result of parsing the specified 'input',
        // interpreted as an instance of parameterized 'TYPE'.  Return 'true'
        // if parsing succeeds and the parsed value satisfies the constraint.
        // Return 'false' otherwise, and write to the specified 'stream' a
        // descriptive error message.  The behavior is undefined unless
        // 'element' is bound to a valid instance of 'bsl::vector<TYPE>'.

    bool validate(const bdlmxxx::ConstElemRef& element) const;
    bool validate(const bdlmxxx::ConstElemRef& element,
                  bsl::ostream&            stream) const;
        // Return 'true' if the specified 'element' satisfies this constraint,
        // and 'false' otherwise.  Optionally specify a 'stream' to which a
        // descriptive error message is written if this constraint is not
        // satisfied.  The behavior is undefined unless 'element' is bound to a
        // variable of the same type as this constraint.
};

// CREATORS
template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
ArrayConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::ArrayConstraint(
                                        const CONSTRAINT_TYPE&  constraint,
                                        bslma::Allocator       *basicAllocator)
: d_constraint(constraint, basicAllocator)
{
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
ArrayConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::~ArrayConstraint()
{
}

// ACCESSORS
template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bdlmxxx::ElemType::Type
ArrayConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::type() const
{
    return bdlmxxx::ElemType::Type(ELEM_TYPE);
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bool
ArrayConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::parse(
                                               const bdlmxxx::ElemRef& element,
                                               bsl::ostream&       stream,
                                               const bsl::string&  input) const
{
    const bdlmxxx::ElemType::Type elemType = element.type();

    BSLS_ASSERT(element.isBound());
    BSLS_ASSERT(ELEM_TYPE == elemType);

    bdlmxxx::ElemType::Type scalarType = bdlmxxx::ElemType::fromArrayType(elemType);
    TYPE value;
    if (!parseValue(&value, input.c_str(), scalarType)) {
        stream << elemTypeToString(scalarType)
               << " value was expected, instead of \""
               << input << "\"" << bsl::endl;
        return false;
    }

    if (d_constraint && !d_constraint(&value, stream)) {
        return false;
    }

    if (element.isNull()) {
        bsl::vector<TYPE> tmp;
        const bdlmxxx::Descriptor *desc =
                                  bdlmxxx::ElemAttrLookup::lookupTable()[elemType];
        element.replaceValue(bdlmxxx::ConstElemRef(&tmp, desc));
    }
    ((bsl::vector<TYPE> *)element.data())->push_back(value);

    return true;
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bool
ArrayConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::validate(
                                        const bdlmxxx::ConstElemRef& element) const
{
    bsl::ostringstream oss;
    return validate(element, oss);
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bool
ArrayConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::validate(
                                         const bdlmxxx::ConstElemRef& element,
                                         bsl::ostream&            stream) const
{
    BSLS_ASSERT(element.isBound());
    BSLS_ASSERT(ELEM_TYPE == element.type());

    typedef typename bsl::vector<TYPE>::size_type size_type;
    const bsl::vector<TYPE>& vec = *((const bsl::vector<TYPE>*)element.data());

    if (d_constraint) {
        for (size_type i = 0; i < vec.size(); ++i) {
            if (!d_constraint(&vec[i], stream)) {
                stream << "The above error occurred while parsing the "
                       << balb::Ordinal(i)
                       << " element of the vector."
                       << bsl::endl;
                return false;
            }
        }
    }
    return true;
}

// SPECIALIZATIONS
template
class ArrayConstraint<char,
                      balb::CommandLineConstraint::CharConstraint,
                      bdlmxxx::ElemType::BDEM_CHAR_ARRAY>;
template
class ArrayConstraint<short,
                      balb::CommandLineConstraint::ShortConstraint,
                      bdlmxxx::ElemType::BDEM_SHORT_ARRAY>;
template
class ArrayConstraint<int,
                      balb::CommandLineConstraint::IntConstraint,
                      bdlmxxx::ElemType::BDEM_INT_ARRAY>;
template
class ArrayConstraint<bsls::Types::Int64,
                      balb::CommandLineConstraint::Int64Constraint,
                      bdlmxxx::ElemType::BDEM_INT64_ARRAY>;
template
class ArrayConstraint<float,
                      balb::CommandLineConstraint::FloatConstraint,
                      bdlmxxx::ElemType::BDEM_FLOAT_ARRAY>;
template
class ArrayConstraint<double,
                      balb::CommandLineConstraint::DoubleConstraint,
                      bdlmxxx::ElemType::BDEM_DOUBLE_ARRAY>;
template
class ArrayConstraint<bsl::string,
                      balb::CommandLineConstraint::StringConstraint,
                      bdlmxxx::ElemType::BDEM_STRING_ARRAY>;
template
class ArrayConstraint<bdlt::Datetime,
                      balb::CommandLineConstraint::DatetimeConstraint,
                      bdlmxxx::ElemType::BDEM_DATETIME_ARRAY>;
template
class ArrayConstraint<bdlt::Date,
                      balb::CommandLineConstraint::DateConstraint,
                      bdlmxxx::ElemType::BDEM_DATE_ARRAY>;
template
class ArrayConstraint<bdlt::Time,
                      balb::CommandLineConstraint::TimeConstraint,
                      bdlmxxx::ElemType::BDEM_TIME_ARRAY>;
    // Explicit specializations (to force instantiation of all variants).

                      // ==================================
                      // local typedefs TYPEConstraint<...>
                      // ==================================

// Note: for 'BoolConstraint', see non-'typedef' implementations.

typedef ScalarConstraint<char,
                         balb::CommandLineConstraint::CharConstraint,
                         bdlmxxx::ElemType::BDEM_CHAR>        CharConstraint;
    // The type 'CharConstraint' is an alias for a scalar constraint on type
    // 'char' using the 'balb::CommandLineConstraint::CharConstraint' functor.

typedef ScalarConstraint<short,
                         balb::CommandLineConstraint::ShortConstraint,
                         bdlmxxx::ElemType::BDEM_SHORT>       ShortConstraint;
    // The type 'ShortConstraint' is an alias for a scalar constraint on type
    // 'short' using the 'balb::CommandLineConstraint::ShortConstraint' functor.

typedef ScalarConstraint<int,
                         balb::CommandLineConstraint::IntConstraint,
                         bdlmxxx::ElemType::BDEM_INT>         IntConstraint;
    // The type 'IntConstraint' is an alias for a scalar constraint on type
    // 'int' using the 'balb::CommandLineConstraint::IntConstraint' functor.

typedef ScalarConstraint<bsls::Types::Int64,
                         balb::CommandLineConstraint::Int64Constraint,
                         bdlmxxx::ElemType::BDEM_INT64>       Int64Constraint;
    // The type 'Int64Constraint' is an alias for a scalar constraint on type
    // 'bsls::Types::Int64' using the
    // 'balb::CommandLineConstraint::Int64Constraint' functor.

typedef ScalarConstraint<float,
                         balb::CommandLineConstraint::FloatConstraint,
                         bdlmxxx::ElemType::BDEM_FLOAT>       FloatConstraint;
    // The type 'FloatConstraint' is an alias for a scalar constraint on type
    // 'float' using the 'balb::CommandLineConstraint::FloatConstraint'
    // functor.

typedef ScalarConstraint<double,
                         balb::CommandLineConstraint::DoubleConstraint,
                         bdlmxxx::ElemType::BDEM_DOUBLE>      DoubleConstraint;
    // The type 'DoubleConstraint' is an alias for a scalar constraint on type
    // 'double' using the 'balb::CommandLineConstraint::DoubleConstraint'
    // functor.

typedef ScalarConstraint<bsl::string,
                         balb::CommandLineConstraint::StringConstraint,
                         bdlmxxx::ElemType::BDEM_STRING>      StringConstraint;
    // The type 'StringConstraint' is an alias for a scalar constraint on type
    // 'bsl::string' using the 'balb::CommandLineConstraint::StringConstraint'
    // functor.

typedef ScalarConstraint<bdlt::Datetime,
                         balb::CommandLineConstraint::DatetimeConstraint,
                         bdlmxxx::ElemType::BDEM_DATETIME>    DatetimeConstraint;
    // The type 'DatetimeConstraint' is an alias for a scalar constraint of
    // type 'bdlt::Datetime' using the
    // 'balb::CommandLineConstraint::DatetimeConstraint' functor.

typedef ScalarConstraint<bdlt::Date,
                         balb::CommandLineConstraint::DateConstraint,
                         bdlmxxx::ElemType::BDEM_DATE>        DateConstraint;
    // The type 'DateConstraint' is an alias for a scalar constraint on type
    // 'bdlt::Date' using the 'balb::CommandLineConstraint::DateConstraint'
    // functor.

typedef ScalarConstraint<bdlt::Time,
                         balb::CommandLineConstraint::TimeConstraint,
                         bdlmxxx::ElemType::BDEM_TIME>        TimeConstraint;
    // The type 'TimeConstraint' is an alias for a scalar constraint on type
    // 'bdlt::Time' using the 'balb::CommandLineConstraint::TimeConstraint'
    // functor.

                    // =======================================
                    // local typedefs TYPEArrayConstraint<...>
                    // =======================================

typedef ArrayConstraint<char,
                        balb::CommandLineConstraint::CharConstraint,
                        bdlmxxx::ElemType::BDEM_CHAR_ARRAY>  CharArrayConstraint;
    // The type 'CharArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<char>' using the
    // 'balb::CommandLineConstraint::CharConstraint' functor.

typedef ArrayConstraint<short,
                        balb::CommandLineConstraint::ShortConstraint,
                        bdlmxxx::ElemType::BDEM_SHORT_ARRAY> ShortArrayConstraint;
    // The type 'ShortArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<short>' using the
    // 'balb::CommandLineConstraint::ShortConstraint' functor.

typedef ArrayConstraint<int,
                        balb::CommandLineConstraint::IntConstraint,
                        bdlmxxx::ElemType::BDEM_INT_ARRAY>   IntArrayConstraint;
    // The type 'IntArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<int>' using the
    // 'balb::CommandLineConstraint::IntConstraint' functor.

typedef ArrayConstraint<bsls::Types::Int64,
                       balb::CommandLineConstraint::Int64Constraint,
                       bdlmxxx::ElemType::BDEM_INT64_ARRAY> Int64ArrayConstraint;
    // The type 'Int64ArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<bsls::Types::Int64>' using the
    // 'balb::CommandLineConstraint::Int64Constraint' functor.

typedef ArrayConstraint<float,
                        balb::CommandLineConstraint::FloatConstraint,
                        bdlmxxx::ElemType::BDEM_FLOAT_ARRAY> FloatArrayConstraint;
    // The type 'FloatArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<float>' using the
    // 'balb::CommandLineConstraint::FloatConstraint' functor.

typedef ArrayConstraint<double,
                        balb::CommandLineConstraint::DoubleConstraint,
                        bdlmxxx::ElemType::BDEM_DOUBLE_ARRAY>
                                                         DoubleArrayConstraint;
    // The type 'DoubleArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<double>' using the
    // 'balb::CommandLineConstraint::DoubleConstraint' functor.

typedef ArrayConstraint<bsl::string,
                        balb::CommandLineConstraint::StringConstraint,
                        bdlmxxx::ElemType::BDEM_STRING_ARRAY>
                                                         StringArrayConstraint;
    // The type 'StringArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<bsl::string>' using the
    // 'balb::CommandLineConstraint::StringConstraint' functor.

typedef ArrayConstraint<bdlt::Datetime,
                        balb::CommandLineConstraint::DatetimeConstraint,
                        bdlmxxx::ElemType::BDEM_DATETIME_ARRAY>
                                                       DatetimeArrayConstraint;
    // The type 'DatetimeArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<bdlt::Datetime>' using the
    // 'balb::CommandLineConstraint::DatetimeConstraint' functor.

typedef ArrayConstraint<bdlt::Date,
                        balb::CommandLineConstraint::DateConstraint,
                        bdlmxxx::ElemType::BDEM_DATE_ARRAY>  DateArrayConstraint;
    // The type 'DateArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<bdlt::Date>' using the
    // 'balb::CommandLineConstraint::DateConstraint' functor.

typedef ArrayConstraint<bdlt::Time,
                        balb::CommandLineConstraint::TimeConstraint,
                        bdlmxxx::ElemType::BDEM_TIME_ARRAY>  TimeArrayConstraint;
    // The type 'TimeArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<bdlt::Time>' using the
    // 'balb::CommandLineConstraint::TimeConstraint' functor.

}  // close unnamed namespace

namespace balb {
// ============================================================================
//                                  COMPONENT
// ============================================================================

                    // ------------------------------------
                    // class CommandLineOccurrenceInfo
                    // ------------------------------------

// CREATORS
CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                              bslma::Allocator *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                              OccurrenceType    type,
                                              bslma::Allocator *basicAllocator)
: d_isRequired(BAEA_REQUIRED == type)
, d_isHidden(  BAEA_HIDDEN   == type)
, d_defaultValue(basicAllocator)
{
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                              char              charValue,
                                              bslma::Allocator *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendChar(charValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                              short             shortValue,
                                              bslma::Allocator *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendShort(shortValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                              int               intValue,
                                              bslma::Allocator *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendInt(intValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                            bsls::Types::Int64  int64Value,
                                            bslma::Allocator   *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendInt64(int64Value);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                              float             floatValue,
                                              bslma::Allocator *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendFloat(floatValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                              double            doubleValue,
                                              bslma::Allocator *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendDouble(doubleValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                            const bsl::string&  stringValue,
                                            bslma::Allocator   *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendString(stringValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                          const bdlt::Datetime&  datetimeValue,
                                          bslma::Allocator     *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendDatetime(datetimeValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                              const bdlt::Date&  dateValue,
                                              bslma::Allocator *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendDate(dateValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                              const bdlt::Time&  timeValue,
                                              bslma::Allocator *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendTime(timeValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                      const bsl::vector<char>&  charArrayValue,
                                      bslma::Allocator         *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendCharArray(charArrayValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                    const bsl::vector<short>&  shortArrayValue,
                                    bslma::Allocator          *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendShortArray(shortArrayValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                       const bsl::vector<int>&  intArrayValue,
                                       bslma::Allocator        *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendIntArray(intArrayValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                       const bsl::vector<bsls::Types::Int64>&  int64ArrayValue,
                       bslma::Allocator                       *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendInt64Array(int64ArrayValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                    const bsl::vector<float>&  floatArrayValue,
                                    bslma::Allocator          *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendFloatArray(floatArrayValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                  const bsl::vector<double>&  doubleArrayValue,
                                  bslma::Allocator           *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendDoubleArray(doubleArrayValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                             const bsl::vector<bsl::string>&  stringArrayValue,
                             bslma::Allocator                *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendStringArray(stringArrayValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                         const bsl::vector<bdlt::Datetime>&  datetimeArrayValue,
                         bslma::Allocator                  *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendDatetimeArray(datetimeArrayValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                 const bsl::vector<bdlt::Date>&  dateArrayValue,
                                 bslma::Allocator              *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendDateArray(dateArrayValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                                 const bsl::vector<bdlt::Time>&  timeArrayValue,
                                 bslma::Allocator              *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendTimeArray(timeArrayValue);
}

CommandLineOccurrenceInfo::CommandLineOccurrenceInfo(
                         const CommandLineOccurrenceInfo&  original,
                         bslma::Allocator                      *basicAllocator)
: d_isRequired(original.d_isRequired)
, d_isHidden(original.d_isHidden)
, d_defaultValue(original.d_defaultValue, basicAllocator)
{
}

CommandLineOccurrenceInfo::~CommandLineOccurrenceInfo()
{
}

// MANIPULATORS
CommandLineOccurrenceInfo& CommandLineOccurrenceInfo::operator=(
                                     const CommandLineOccurrenceInfo& rhs)
{
    if (&rhs != this) {
        d_isRequired   = rhs.d_isRequired;
        d_isHidden     = rhs.d_isHidden;
        d_defaultValue = rhs.d_defaultValue;
    }
    return *this;
}

void CommandLineOccurrenceInfo::setDefaultValue(
                                                const bdlmxxx::ConstElemRef& value)
{
    BSLS_ASSERT(!d_isRequired);
    BSLS_ASSERT(value.isBound());
    BSLS_ASSERT(bdlmxxx::ElemType::BDEM_CHAR           == value.type()
             || bdlmxxx::ElemType::BDEM_SHORT          == value.type()
             || bdlmxxx::ElemType::BDEM_INT            == value.type()
             || bdlmxxx::ElemType::BDEM_INT64          == value.type()
             || bdlmxxx::ElemType::BDEM_FLOAT          == value.type()
             || bdlmxxx::ElemType::BDEM_DOUBLE         == value.type()
             || bdlmxxx::ElemType::BDEM_STRING         == value.type()
             || bdlmxxx::ElemType::BDEM_DATETIME       == value.type()
             || bdlmxxx::ElemType::BDEM_DATE           == value.type()
             || bdlmxxx::ElemType::BDEM_TIME           == value.type()
             || bdlmxxx::ElemType::BDEM_CHAR_ARRAY     == value.type()
             || bdlmxxx::ElemType::BDEM_SHORT_ARRAY    == value.type()
             || bdlmxxx::ElemType::BDEM_INT_ARRAY      == value.type()
             || bdlmxxx::ElemType::BDEM_INT64_ARRAY    == value.type()
             || bdlmxxx::ElemType::BDEM_FLOAT_ARRAY    == value.type()
             || bdlmxxx::ElemType::BDEM_DOUBLE_ARRAY   == value.type()
             || bdlmxxx::ElemType::BDEM_STRING_ARRAY   == value.type()
             || bdlmxxx::ElemType::BDEM_DATETIME_ARRAY == value.type()
             || bdlmxxx::ElemType::BDEM_DATE_ARRAY     == value.type()
             || bdlmxxx::ElemType::BDEM_TIME_ARRAY     == value.type());

    if (0 < d_defaultValue.length()) {
        d_defaultValue.removeAll();
    }
    d_defaultValue.appendElement(value);
}

void CommandLineOccurrenceInfo::setHidden()
{
    BSLS_ASSERT(!d_isRequired);

    d_isHidden = true;
}

// ACCESSORS
bdlmxxx::ConstElemRef CommandLineOccurrenceInfo::defaultValue() const
{
    BSLS_ASSERT(0 < d_defaultValue.length());

    return d_defaultValue[0];
}

bool CommandLineOccurrenceInfo::hasDefaultValue() const
{
    return 0 < d_defaultValue.length();
}

bool CommandLineOccurrenceInfo::isHidden() const
{
    return d_isHidden;
}

bool CommandLineOccurrenceInfo::isRequired() const
{
    return d_isRequired;
}

CommandLineOccurrenceInfo::OccurrenceType
CommandLineOccurrenceInfo::occurrenceType() const
{
    return d_isRequired ? BAEA_REQUIRED : (d_isHidden ? BAEA_HIDDEN
                                                      : BAEA_OPTIONAL);
}

bsl::ostream&
CommandLineOccurrenceInfo::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    // BSLS_ASSERT(0 <= spacesPerLevel);

    if (stream.bad()) {
        return stream;
    }

    if (level < 0) {
        level = -level;
    }
    else {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    if (hasDefaultValue()) {
        stream << "{";
        bdlb::Print::newlineAndIndent(stream, ++level, spacesPerLevel);
    }
    const char *typeString;
    switch (occurrenceType()) {
      case CommandLineOccurrenceInfo::BAEA_REQUIRED: {
        typeString = "REQUIRED";
      } break;
      case CommandLineOccurrenceInfo::BAEA_HIDDEN: {
        typeString = "HIDDEN";
      } break;
      case CommandLineOccurrenceInfo::BAEA_OPTIONAL: {
        typeString = "OPTIONAL";
      } break;
      default: {
        typeString = "***UNKNOWN***";
      } break;
    }
    stream << typeString;
    if (hasDefaultValue()) {
        bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << "DEFAULT_TYPE  " << defaultValue().type();
        bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << "DEFAULT_VALUE ";
        defaultValue().print(stream, -level, -spacesPerLevel);
        bdlb::Print::newlineAndIndent(stream, --level, spacesPerLevel);
        stream << "}";
    }

    return stream;
}
}  // close package namespace

// FREE OPERATORS
bool balb::operator==(const CommandLineOccurrenceInfo& lhs,
                const CommandLineOccurrenceInfo& rhs)
{
    return lhs.occurrenceType()  == rhs.occurrenceType()
        && lhs.hasDefaultValue() == rhs.hasDefaultValue()
        && (!lhs.hasDefaultValue() ||
                                     lhs.defaultValue() == rhs.defaultValue());
}

bool balb::operator!=(const CommandLineOccurrenceInfo& lhs,
                const CommandLineOccurrenceInfo& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream&
balb::operator<<(bsl::ostream& stream, const CommandLineOccurrenceInfo& rhs)
{
    return rhs.print(stream);
}

                      // ---------------------------------
                      // struct balb::CommandLineOptionType
                      // ---------------------------------

typedef balb::CommandLineOptionType CLOT;

// CLASS DATA
bool                                  * const CLOT::BAEA_BOOL           = 0;
char                                  * const CLOT::BAEA_CHAR           = 0;
short                                 * const CLOT::BAEA_SHORT          = 0;
int                                   * const CLOT::BAEA_INT            = 0;
bsls::Types::Int64                    * const CLOT::BAEA_INT64          = 0;
float                                 * const CLOT::BAEA_FLOAT          = 0;
double                                * const CLOT::BAEA_DOUBLE         = 0;
bsl::string                           * const CLOT::BAEA_STRING         = 0;
bdlt::Datetime                         * const CLOT::BAEA_DATETIME       = 0;
bdlt::Date                             * const CLOT::BAEA_DATE           = 0;
bdlt::Time                             * const CLOT::BAEA_TIME           = 0;
bsl::vector<char>                     * const CLOT::BAEA_CHAR_ARRAY     = 0;
bsl::vector<short>                    * const CLOT::BAEA_SHORT_ARRAY    = 0;
bsl::vector<int>                      * const CLOT::BAEA_INT_ARRAY      = 0;
bsl::vector<bsls::Types::Int64>       * const CLOT::BAEA_INT64_ARRAY    = 0;
bsl::vector<float>                    * const CLOT::BAEA_FLOAT_ARRAY    = 0;
bsl::vector<double>                   * const CLOT::BAEA_DOUBLE_ARRAY   = 0;
bsl::vector<bsl::string>              * const CLOT::BAEA_STRING_ARRAY   = 0;
bsl::vector<bdlt::Datetime>            * const CLOT::BAEA_DATETIME_ARRAY = 0;
bsl::vector<bdlt::Date>                * const CLOT::BAEA_DATE_ARRAY     = 0;
bsl::vector<bdlt::Time>                * const CLOT::BAEA_TIME_ARRAY     = 0;

namespace balb {
                       // ------------------------------
                       // class CommandLineTypeInfo
                       // ------------------------------

// CREATORS
CommandLineTypeInfo::CommandLineTypeInfo(
                                              bslma::Allocator *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_STRING)
, d_linkedVariable_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                           bool             *variable,
                                           bslma::Allocator *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_BOOL)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                              char             *variable,
                                              bslma::Allocator *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_CHAR)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
             char                                              *variable,
             const CommandLineConstraint::CharConstraint&  constraint,
             bslma::Allocator                                  *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_CHAR)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                              short            *variable,
                                              bslma::Allocator *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_SHORT)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
            short                                              *variable,
            const CommandLineConstraint::ShortConstraint&  constraint,
            bslma::Allocator                                   *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_SHORT)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                              int              *variable,
                                              bslma::Allocator *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_INT)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
              int                                              *variable,
              const CommandLineConstraint::IntConstraint&  constraint,
              bslma::Allocator                                 *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_INT)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                            bsls::Types::Int64 *variable,
                                            bslma::Allocator   *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_INT64)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
            bsls::Types::Int64                                 *variable,
            const CommandLineConstraint::Int64Constraint&  constraint,
            bslma::Allocator                                   *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_INT64)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                              float            *variable,
                                              bslma::Allocator *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_FLOAT)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
            float                                              *variable,
            const CommandLineConstraint::FloatConstraint&  constraint,
            bslma::Allocator                                   *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_FLOAT)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                              double           *variable,
                                              bslma::Allocator *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_DOUBLE)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
           double                                              *variable,
           const CommandLineConstraint::DoubleConstraint&  constraint,
           bslma::Allocator                                    *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_DOUBLE)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                              bsl::string      *variable,
                                              bslma::Allocator *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_STRING)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
           bsl::string                                         *variable,
           const CommandLineConstraint::StringConstraint&  constraint,
           bslma::Allocator                                    *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_STRING)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                              bdlt::Datetime    *variable,
                                              bslma::Allocator *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_DATETIME)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
         bdlt::Datetime                                         *variable,
         const CommandLineConstraint::DatetimeConstraint&  constraint,
         bslma::Allocator                                      *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_DATETIME)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                              bdlt::Date        *variable,
                                              bslma::Allocator *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_DATE)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
             bdlt::Date                                         *variable,
             const CommandLineConstraint::DateConstraint&  constraint,
             bslma::Allocator                                  *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_DATE)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                              bdlt::Time        *variable,
                                              bslma::Allocator *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_TIME)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
             bdlt::Time                                         *variable,
             const CommandLineConstraint::TimeConstraint&  constraint,
             bslma::Allocator                                  *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_TIME)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                             bsl::vector<char> *variable,
                                             bslma::Allocator  *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_CHAR_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
             bsl::vector<char>                                 *variable,
             const CommandLineConstraint::CharConstraint&  constraint,
             bslma::Allocator                                  *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_CHAR_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                            bsl::vector<short> *variable,
                                            bslma::Allocator   *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_SHORT_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
            bsl::vector<short>                                 *variable,
            const CommandLineConstraint::ShortConstraint&  constraint,
            bslma::Allocator                                   *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_SHORT_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                              bsl::vector<int> *variable,
                                              bslma::Allocator *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_INT_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
              bsl::vector<int>                                 *variable,
              const CommandLineConstraint::IntConstraint&  constraint,
              bslma::Allocator                                 *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_INT_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                               bsl::vector<bsls::Types::Int64> *variable,
                               bslma::Allocator                *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_INT64_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
            bsl::vector<bsls::Types::Int64>                    *variable,
            const CommandLineConstraint::Int64Constraint&  constraint,
            bslma::Allocator                                   *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_INT64_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                            bsl::vector<float> *variable,
                                            bslma::Allocator   *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_FLOAT_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
            bsl::vector<float>                                 *variable,
            const CommandLineConstraint::FloatConstraint&  constraint,
            bslma::Allocator                                   *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_FLOAT_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                           bsl::vector<double> *variable,
                                           bslma::Allocator    *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_DOUBLE_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
           bsl::vector<double>                                 *variable,
           const CommandLineConstraint::DoubleConstraint&  constraint,
           bslma::Allocator                                    *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_DOUBLE_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                      bsl::vector<bsl::string> *variable,
                                      bslma::Allocator         *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_STRING_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
           bsl::vector<bsl::string>                            *variable,
           const CommandLineConstraint::StringConstraint&  constraint,
           bslma::Allocator                                    *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_STRING_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                    bsl::vector<bdlt::Datetime> *variable,
                                    bslma::Allocator           *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_DATETIME_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
         bsl::vector<bdlt::Datetime>                            *variable,
         const CommandLineConstraint::DatetimeConstraint&  constraint,
         bslma::Allocator                                      *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_DATETIME_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                        bsl::vector<bdlt::Date> *variable,
                                        bslma::Allocator       *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_DATE_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
             bsl::vector<bdlt::Date>                            *variable,
             const CommandLineConstraint::DateConstraint&  constraint,
             bslma::Allocator                                  *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_DATE_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                                        bsl::vector<bdlt::Time> *variable,
                                        bslma::Allocator       *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_TIME_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

CommandLineTypeInfo::CommandLineTypeInfo(
             bsl::vector<bdlt::Time>                            *variable,
             const CommandLineConstraint::TimeConstraint&  constraint,
             bslma::Allocator                                  *basicAllocator)
: d_elemType(bdlmxxx::ElemType::BDEM_TIME_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

CommandLineTypeInfo::CommandLineTypeInfo(
                               const CommandLineTypeInfo&  original,
                               bslma::Allocator                *basicAllocator)
: d_elemType(original.d_elemType)
, d_linkedVariable_p(original.d_linkedVariable_p)
, d_constraint_p(original.d_constraint_p)  // share constraint
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

CommandLineTypeInfo::~CommandLineTypeInfo()
{
}

// MANIPULATORS
CommandLineTypeInfo&
CommandLineTypeInfo::operator=(const CommandLineTypeInfo& rhs)
{
    if (this != &rhs) {
        d_elemType         = rhs.d_elemType;
        d_linkedVariable_p = rhs.d_linkedVariable_p;
        d_constraint_p     = rhs.d_constraint_p;
    }

    return *this;
}

void CommandLineTypeInfo::resetLinkedVariable()
{
    d_linkedVariable_p = 0;
    resetConstraint();
}

void CommandLineTypeInfo::resetConstraint()
{
    // STATIC DATA
    static BoolConstraint           defaultBoolConstraint;
    static CharConstraint           defaultCharConstraint(
                              CommandLineConstraint::CharConstraint(), 0);
    static ShortConstraint          defaultShortConstraint(
                             CommandLineConstraint::ShortConstraint(), 0);
    static IntConstraint            defaultIntConstraint(
                               CommandLineConstraint::IntConstraint(), 0);
    static Int64Constraint          defaultInt64Constraint(
                             CommandLineConstraint::Int64Constraint(), 0);
    static FloatConstraint          defaultFloatConstraint(
                             CommandLineConstraint::FloatConstraint(), 0);
    static DoubleConstraint         defaultDoubleConstraint(
                            CommandLineConstraint::DoubleConstraint(), 0);
    static StringConstraint         defaultStringConstraint(
                            CommandLineConstraint::StringConstraint(), 0);
    static DatetimeConstraint       defaultDatetimeConstraint(
                          CommandLineConstraint::DatetimeConstraint(), 0);
    static DateConstraint           defaultDateConstraint(
                              CommandLineConstraint::DateConstraint(), 0);
    static TimeConstraint           defaultTimeConstraint(
                              CommandLineConstraint::TimeConstraint(), 0);
    static CharArrayConstraint      defaultCharArrayConstraint(
                              CommandLineConstraint::CharConstraint(), 0);
    static ShortArrayConstraint     defaultShortArrayConstraint(
                             CommandLineConstraint::ShortConstraint(), 0);
    static IntArrayConstraint       defaultIntArrayConstraint(
                               CommandLineConstraint::IntConstraint(), 0);
    static Int64ArrayConstraint     defaultInt64ArrayConstraint(
                             CommandLineConstraint::Int64Constraint(), 0);
    static FloatArrayConstraint     defaultFloatArrayConstraint(
                             CommandLineConstraint::FloatConstraint(), 0);
    static DoubleArrayConstraint    defaultDoubleArrayConstraint(
                            CommandLineConstraint::DoubleConstraint(), 0);
    static StringArrayConstraint    defaultStringArrayConstraint(
                            CommandLineConstraint::StringConstraint(), 0);
    static DatetimeArrayConstraint  defaultDatetimeArrayConstraint(
                          CommandLineConstraint::DatetimeConstraint(), 0);
    static DateArrayConstraint      defaultDateArrayConstraint(
                              CommandLineConstraint::DateConstraint(), 0);
    static TimeArrayConstraint      defaultTimeArrayConstraint(
                              CommandLineConstraint::TimeConstraint(), 0);
        // These static variables, default-initialized, are shared among all
        // type infos that do not have constraints.  Note that these objects
        // are initialized with a default allocator and should never be
        // deleted.

    switch (d_elemType) {
      case bdlmxxx::ElemType::BDEM_BOOL: {
        d_constraint_p.reset(&defaultBoolConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_CHAR: {
        d_constraint_p.reset(&defaultCharConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_SHORT: {
        d_constraint_p.reset(&defaultShortConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_INT: {
        d_constraint_p.reset(&defaultIntConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_INT64: {
        d_constraint_p.reset(&defaultInt64Constraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_FLOAT: {
        d_constraint_p.reset(&defaultFloatConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_DOUBLE: {
        d_constraint_p.reset(&defaultDoubleConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_STRING: {
        d_constraint_p.reset(&defaultStringConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_DATETIME: {
        d_constraint_p.reset(&defaultDatetimeConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_DATE: {
        d_constraint_p.reset(&defaultDateConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_TIME: {
        d_constraint_p.reset(&defaultTimeConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_CHAR_ARRAY: {
        d_constraint_p.reset(&defaultCharArrayConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_SHORT_ARRAY: {
        d_constraint_p.reset(&defaultShortArrayConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_INT_ARRAY: {
        d_constraint_p.reset(&defaultIntArrayConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_INT64_ARRAY: {
        d_constraint_p.reset(&defaultInt64ArrayConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_FLOAT_ARRAY: {
        d_constraint_p.reset(&defaultFloatArrayConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_DOUBLE_ARRAY: {
        d_constraint_p.reset(&defaultDoubleArrayConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_STRING_ARRAY: {
        d_constraint_p.reset(&defaultStringArrayConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_DATETIME_ARRAY: {
        d_constraint_p.reset(&defaultDatetimeArrayConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_DATE_ARRAY: {
        d_constraint_p.reset(&defaultDateArrayConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      case bdlmxxx::ElemType::BDEM_TIME_ARRAY: {
        d_constraint_p.reset(&defaultTimeArrayConstraint,
                             bslstl::SharedPtrNilDeleter());
      } break;
      default: {
        BSLS_ASSERT(0);
      } break;
    }
}

void CommandLineTypeInfo::setConstraint(
                  const CommandLineConstraint::CharConstraint& constraint)
{
    if (bdlmxxx::ElemType::BDEM_CHAR == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p) CharConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (bdlmxxx::ElemType::BDEM_CHAR_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                            CharArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void CommandLineTypeInfo::setConstraint(
                 const CommandLineConstraint::ShortConstraint& constraint)
{
    if (bdlmxxx::ElemType::BDEM_SHORT == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                                ShortConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (bdlmxxx::ElemType::BDEM_SHORT_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                           ShortArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void CommandLineTypeInfo::setConstraint(
                   const CommandLineConstraint::IntConstraint& constraint)
{
    if (bdlmxxx::ElemType::BDEM_INT == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p) IntConstraint(constraint,
                                                               d_allocator_p),
                             d_allocator_p);
    } else if (bdlmxxx::ElemType::BDEM_INT_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                             IntArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void CommandLineTypeInfo::setConstraint(
                 const CommandLineConstraint::Int64Constraint& constraint)
{
    if (bdlmxxx::ElemType::BDEM_INT64 == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                                Int64Constraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (bdlmxxx::ElemType::BDEM_INT64_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                           Int64ArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void CommandLineTypeInfo::setConstraint(
                 const CommandLineConstraint::FloatConstraint& constraint)
{
    if (bdlmxxx::ElemType::BDEM_FLOAT == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                                FloatConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (bdlmxxx::ElemType::BDEM_FLOAT_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                           FloatArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void CommandLineTypeInfo::setConstraint(
                const CommandLineConstraint::DoubleConstraint& constraint)
{
    if (bdlmxxx::ElemType::BDEM_DOUBLE == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                               DoubleConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (bdlmxxx::ElemType::BDEM_DOUBLE_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                          DoubleArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void CommandLineTypeInfo::setConstraint(
                const CommandLineConstraint::StringConstraint& constraint)
{
    if (bdlmxxx::ElemType::BDEM_STRING == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                               StringConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (bdlmxxx::ElemType::BDEM_STRING_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                          StringArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void CommandLineTypeInfo::setConstraint(
              const CommandLineConstraint::DatetimeConstraint& constraint)
{
    if (bdlmxxx::ElemType::BDEM_DATETIME == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                             DatetimeConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (bdlmxxx::ElemType::BDEM_DATETIME_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                        DatetimeArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void CommandLineTypeInfo::setConstraint(
                  const CommandLineConstraint::DateConstraint& constraint)
{
    if (bdlmxxx::ElemType::BDEM_DATE == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p) DateConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (bdlmxxx::ElemType::BDEM_DATE_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                            DateArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void CommandLineTypeInfo::setConstraint(
                  const CommandLineConstraint::TimeConstraint& constraint)
{
    if (bdlmxxx::ElemType::BDEM_TIME == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                                TimeConstraint(constraint,
                                                               d_allocator_p),
                             d_allocator_p);
    } else if (bdlmxxx::ElemType::BDEM_TIME_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                            TimeArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void CommandLineTypeInfo::setConstraint(
                const bsl::shared_ptr<CommandLine_Constraint>& constraint)
{
    d_constraint_p = constraint;
}

void CommandLineTypeInfo::setLinkedVariable(bool *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_BOOL;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(char *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_CHAR;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(short *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_SHORT;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(int *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_INT;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(bsls::Types::Int64 *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_INT64;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(float *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_FLOAT;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(double *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_DOUBLE;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(bsl::string *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_STRING;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(bdlt::Datetime *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_DATETIME;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(bdlt::Date *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_DATE;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(bdlt::Time *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_TIME;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(bsl::vector<char> *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_CHAR_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(bsl::vector<short> *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_SHORT_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(bsl::vector<int> *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_INT_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(
                                     bsl::vector<bsls::Types::Int64> *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_INT64_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(bsl::vector<float> *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_FLOAT_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(bsl::vector<double> *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_DOUBLE_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(
                                            bsl::vector<bsl::string> *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_STRING_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(
                                          bsl::vector<bdlt::Datetime> *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_DATETIME_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(
                                              bsl::vector<bdlt::Date> *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_DATE_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void CommandLineTypeInfo::setLinkedVariable(
                                              bsl::vector<bdlt::Time> *variable)
{
    d_elemType         = bdlmxxx::ElemType::BDEM_TIME_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

// ACCESSORS
bsl::shared_ptr<CommandLine_Constraint>
CommandLineTypeInfo::constraint() const
{
    return d_constraint_p;
}

bool CommandLineTypeInfo::checkConstraint(
                                        const bdlmxxx::ConstElemRef& element) const
{
    return d_constraint_p->validate(element);
}

bool CommandLineTypeInfo::checkConstraint(
                                         const bdlmxxx::ConstElemRef& element,
                                         bsl::ostream&            stream) const
{
    return d_constraint_p->validate(element, stream);
}

bool CommandLineTypeInfo::checkConstraint(const void *variable) const
{
    const bdlmxxx::Descriptor *desc =
                                bdlmxxx::ElemAttrLookup::lookupTable()[d_elemType];
    return d_constraint_p->validate(bdlmxxx::ConstElemRef(variable, desc));
}

bool CommandLineTypeInfo::checkConstraint(const void    *variable,
                                               bsl::ostream&  stream) const
{
    const bdlmxxx::Descriptor *desc =
                                bdlmxxx::ElemAttrLookup::lookupTable()[d_elemType];
    return d_constraint_p->validate(bdlmxxx::ConstElemRef(variable, desc), stream);
}

bdlmxxx::ElemType::Type CommandLineTypeInfo::type() const
{
    return d_elemType;
}

void *CommandLineTypeInfo::linkedVariable() const
{
    return d_linkedVariable_p;
}

bsl::ostream&
CommandLineTypeInfo::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    BSLS_ASSERT(0 <= spacesPerLevel);

    if (stream.bad()) {
        return stream;
    }

    if (level < 0) {
        level = -level;
    }
    else {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    stream << "{";
    bdlb::Print::newlineAndIndent(stream, ++level, spacesPerLevel);
    stream << "TYPE       " << type();
    if (linkedVariable()) {
        bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << "VARIABLE   " << (void *)linkedVariable();
    }
    if (constraint()) {
        bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << "CONSTRAINT " << (void *)constraint().get();
    }
    bdlb::Print::newlineAndIndent(stream, --level, spacesPerLevel);
    stream << "}";

    return stream;
}
}  // close package namespace

// FREE OPERATORS
bool balb::operator==(const CommandLineTypeInfo& lhs,
                const CommandLineTypeInfo& rhs)
{
    return lhs.type()           == rhs.type()
        && lhs.linkedVariable() == rhs.linkedVariable()
        && lhs.constraint()     == rhs.constraint();
}

bool balb::operator!=(const CommandLineTypeInfo& lhs,
                const CommandLineTypeInfo& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream&
balb::operator<<(bsl::ostream& stream, const CommandLineTypeInfo& rhs)
{
    return rhs.print(stream);
}

                     // ---------------------------------
                     // struct balb::CommandLineOptionInfo
                     // ---------------------------------

// FREE OPERATORS
bool balb::operator==(const CommandLineOptionInfo& lhs,
                const CommandLineOptionInfo& rhs)
{
    return lhs.d_tag         == rhs.d_tag
        && lhs.d_name        == rhs.d_name
        && lhs.d_description == rhs.d_description
        && lhs.d_typeInfo    == rhs.d_typeInfo
        && lhs.d_defaultInfo == rhs.d_defaultInfo;
}

bool balb::operator!=(const CommandLineOptionInfo& lhs,
                const CommandLineOptionInfo& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream&
balb::operator<<(bsl::ostream& stream, const CommandLineOptionInfo& rhs)
{
    stream << "{" << bsl::endl;
    if (rhs.d_tag.empty()) {
        stream << "    NON_OPTION" << bsl::endl;
    } else {
        stream << "    TAG             \"" << rhs.d_tag << "\"" << bsl::endl;
    }
    stream << "    NAME            \""     << rhs.d_name << "\"" << bsl::endl;
    stream << "    DESCRIPTION     \""     << rhs.d_description << "\""
                                                                  << bsl::endl;
    stream << "    TYPE_INFO       ";   rhs.d_typeInfo.print(stream, -1);
                                        stream << bsl::endl;
    stream << "    OCCURRENCE_INFO ";   rhs.d_defaultInfo.print(stream, -1);
                                        stream << bsl::endl;
    stream << "}";
    return stream;
}

namespace balb {
                          // ----------------------------
                          // class CommandLineOption
                          // ----------------------------

// PRIVATE MANIPULATORS
void CommandLineOption::init(const CommandLineOptionInfo& optionInfo)
{
    bslalg::ScalarPrimitives::copyConstruct(&d_optionInfo.object().d_tag,
                                            optionInfo.d_tag,
                                            d_allocator_p);
    bslalg::AutoScalarDestructor<bsl::string> tagGuard(
                                                 &d_optionInfo.object().d_tag);

    bslalg::ScalarPrimitives::copyConstruct(&d_optionInfo.object().d_name,
                                            optionInfo.d_name,
                                            d_allocator_p);
    bslalg::AutoScalarDestructor<bsl::string> nameGuard(
                                                &d_optionInfo.object().d_name);

    bslalg::ScalarPrimitives::copyConstruct(
                                          &d_optionInfo.object().d_description,
                                          optionInfo.d_description,
                                          d_allocator_p);
    bslalg::AutoScalarDestructor<bsl::string> descGuard(
                                         &d_optionInfo.object().d_description);

    bslalg::ScalarPrimitives::copyConstruct(&d_optionInfo.object().d_typeInfo,
                                            optionInfo.d_typeInfo,
                                            d_allocator_p);
    bslalg::AutoScalarDestructor<CommandLineTypeInfo> typeGuard(
                                            &d_optionInfo.object().d_typeInfo);

    bslalg::ScalarPrimitives::copyConstruct(
                                          &d_optionInfo.object().d_defaultInfo,
                                          optionInfo.d_defaultInfo,
                                          d_allocator_p);
    typeGuard.release();
    descGuard.release();
    nameGuard.release();
    tagGuard.release();

    // Erase trailing "=...." from the name.

    bsl::string::size_type posEqual = d_optionInfo.object().d_name.find('=');

    if (posEqual != bsl::string::npos) {
        d_optionInfo.object().d_name.erase(posEqual, bsl::string::npos);
    }
}

// CREATORS
CommandLineOption::CommandLineOption(
                                              bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    bslalg::ScalarPrimitives::defaultConstruct(
                                 &d_optionInfo.object().d_tag, d_allocator_p);
    bslalg::AutoScalarDestructor<bsl::string> tagGuard(
                                                 &d_optionInfo.object().d_tag);

    bslalg::ScalarPrimitives::defaultConstruct(
                                &d_optionInfo.object().d_name, d_allocator_p);
    bslalg::AutoScalarDestructor<bsl::string> nameGuard(
                                                &d_optionInfo.object().d_name);

    bslalg::ScalarPrimitives::defaultConstruct(
                         &d_optionInfo.object().d_description, d_allocator_p);
    bslalg::AutoScalarDestructor<bsl::string> descGuard(
                                         &d_optionInfo.object().d_description);

    bslalg::ScalarPrimitives::defaultConstruct(
                            &d_optionInfo.object().d_typeInfo, d_allocator_p);
    bslalg::AutoScalarDestructor<CommandLineTypeInfo> typeGuard(
                                            &d_optionInfo.object().d_typeInfo);

    bslalg::ScalarPrimitives::defaultConstruct(
                         &d_optionInfo.object().d_defaultInfo, d_allocator_p);
    typeGuard.release();
    descGuard.release();
    nameGuard.release();
    tagGuard.release();
}

CommandLineOption::CommandLineOption(
                                 const CommandLineOption&  original,
                                 bslma::Allocator              *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    init(static_cast<const CommandLineOptionInfo&>(original));
}

CommandLineOption::CommandLineOption(
                             const CommandLineOptionInfo&  optionInfo,
                             bslma::Allocator                  *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    init(optionInfo);
}

CommandLineOption::~CommandLineOption()
{
    bslalg::ScalarDestructionPrimitives::destroy(&d_optionInfo.object());
}

// MANIPULATORS
CommandLineOption&
CommandLineOption::operator=(const CommandLineOption& rhs)
{
    const CommandLineOptionInfo& optionInfo = rhs;
    return *this = optionInfo;
}

CommandLineOption&
CommandLineOption::operator=(const CommandLineOptionInfo& rhs)
{
    if (&d_optionInfo.object() != &rhs) {
        d_optionInfo.object().d_tag         = rhs.d_tag;
        d_optionInfo.object().d_name        = rhs.d_name;
        d_optionInfo.object().d_description = rhs.d_description;
        d_optionInfo.object().d_typeInfo    = rhs.d_typeInfo;
        d_optionInfo.object().d_defaultInfo = rhs.d_defaultInfo;
    }
    return *this;
}
}  // close package namespace

// ACCESSORS
balb::CommandLineOption::operator const balb::CommandLineOptionInfo&() const
{
    return d_optionInfo.object();
}

namespace balb {
CommandLineOptionInfo::ArgType CommandLineOption::argType() const
{
    if (d_optionInfo.object().d_tag.empty()) {
        return CommandLineOptionInfo::BAEA_NON_OPTION;
    }

    if (bdlmxxx::ElemType::BDEM_BOOL ==
                       d_optionInfo.object().d_typeInfo.constraint()->type()) {
        return CommandLineOptionInfo::BAEA_FLAG;
    }

    return CommandLineOptionInfo::BAEA_OPTION;
}

const bsl::string& CommandLineOption::description() const
{
    return d_optionInfo.object().d_description;
}

bool CommandLineOption::isArray() const
{
    return bdlmxxx::ElemType::isArrayType(d_optionInfo.object().d_typeInfo.type());
}

bool CommandLineOption::isLongTagValid(const char    *longTag,
                                            bsl::ostream&  stream) const
{
    bool result = true;  // attempt to give all possible error messages

    if ('-' == *longTag) {
        stream << "Long tag cannot begin with '-'." << bsl::endl;
        result = false;
    }

    if (0 != bsl::strchr(longTag, '|')) {
        stream << "Long tag cannot contain '|'." << bsl::endl;
        result = false;
    }

    if (bsl::strlen(longTag) < 2) {
        stream << "Long tag must be 2 or more characters." << bsl::endl;
        result = false;
    }

    return result;
}

bool CommandLineOption::isTagValid(bsl::ostream& stream) const
{
    bool result = true;   // attempt to give all possible error messages
    const char *str = d_optionInfo.object().d_tag.c_str();

    if (0 == *str) {  // non-option
        if (bdlmxxx::ElemType::BDEM_BOOL ==
                                     d_optionInfo.object().d_typeInfo.type()) {
            stream << "Flags cannot have an empty tag." << bsl::endl;
            return false;
        }
        return true;
    }

    if (0 != bsl::strchr(str, ' ')) {  // for all tag types
        stream << "Tag cannot contain spaces." << bsl::endl;
        result = false;
    }

    if (0 == bsl::strchr(str, '|')) {
        // Long-tag only: can have multiple '-' (except in first position), and
        // no '|' in the name.

        return isLongTagValid(str, stream) && result;  // no short-circuit!
    }

    // Short tag: must be specified as "s|long", where 's' is not '|'.

    if ('|' == *str) {
        if (0 != bsl::strchr(str + 1, '|')) {
            stream << "Too many '|' in tag string." << bsl::endl;
            result = false;
        } else {
            stream << "Short tag cannot be empty if '|' present." << bsl::endl;
            result = false;
        }
        return isLongTagValid(str + 1, stream) && result;  // no short-circuit!
    }

    if ('-' == *str) {
        stream << "Short tag cannot be '-'." << bsl::endl;
        result = false;
    }

    if ('|' != str[1]) {
        stream << "Short tag must be exactly one character, followed by '|'."
               << bsl::endl;
        return false;
    }

    if (0 == str[1]) {
        stream << "Short tag must have associated long tag." << bsl::endl;
        return false;
    }

    return result && isLongTagValid(str + 2, stream);
}

bool CommandLineOption::isDescriptionValid(bsl::ostream& stream) const
{
    if (d_optionInfo.object().d_description.empty()) {
        stream << "Description cannot be an empty string." << bsl::endl;
        return false;
    }
    return true;
}

bool CommandLineOption::isNameValid(bsl::ostream& stream) const
{
    if (d_optionInfo.object().d_name.empty()) {
        stream << "Name cannot be an empty string." << bsl::endl;
        return false;
    }
    return true;
}

const char *CommandLineOption::longTag() const
{
    BSLS_ASSERT(argType() != CommandLineOptionInfo::BAEA_NON_OPTION);

    const char *tagString = d_optionInfo.object().d_tag.c_str();
    return '|' == tagString[1] ? tagString + 2 : tagString;
}

const bsl::string& CommandLineOption::name() const
{
    return d_optionInfo.object().d_name;
}

const CommandLineOccurrenceInfo&
CommandLineOption::occurrenceInfo() const
{
    return d_optionInfo.object().d_defaultInfo;
}

char CommandLineOption::shortTag() const
{
    BSLS_ASSERT(argType() != CommandLineOptionInfo::BAEA_NON_OPTION);

    const char *tagString = d_optionInfo.object().d_tag.c_str();
    return '|' == tagString[1] ? *tagString : 0;
}

const bsl::string& CommandLineOption::tagString() const
{
    return d_optionInfo.object().d_tag;
}

const CommandLineTypeInfo&
CommandLineOption::typeInfo() const
{
    return d_optionInfo.object().d_typeInfo;
}

bsl::ostream&
CommandLineOption::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    BSLS_ASSERT(0 <= spacesPerLevel);

    if (stream.bad()) {
        return stream;
    }

    if (level < 0) {
        level = -level;
    }
    else {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    stream << "{";
    bdlb::Print::newlineAndIndent(stream, ++level, spacesPerLevel);
    if (tagString().empty()) {
        stream << "NON_OPTION";
    } else {
        stream << "TAG            \"" << tagString() << "\"";
    }
    bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "NAME           \"" << name() << "\"";
    bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "DESCRIPTION    \"" << description() << "\"";
    bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "TYPE_INFO      ";
    typeInfo().print(stream, -2, spacesPerLevel);
    bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "OCCURRENCE_INFO ";
    occurrenceInfo().print(stream, -2, spacesPerLevel);
    bdlb::Print::newlineAndIndent(stream, --level, spacesPerLevel);
    stream << "}";

    return stream;
}
}  // close package namespace

// FREE OPERATORS
bool balb::operator==(const CommandLineOption& lhs,
                const CommandLineOption& rhs)
{
    return static_cast<const CommandLineOptionInfo&>(lhs) ==
                           static_cast<const CommandLineOptionInfo&>(rhs);
}

bool balb::operator!=(const CommandLineOption& lhs,
                const CommandLineOption& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream&
balb::operator<<(bsl::ostream& stream, const CommandLineOption& rhs)
{
    return rhs.print(stream);
}

namespace balb {
                          // ----------------------
                          // class CommandLine
                          // ----------------------

// PRIVATE MANIPULATORS
void CommandLine::clear()
{
    d_options.clear();
    d_positions.clear();
    d_nonOptionIndices.clear();
    d_state = BAEA_INVALID;
    d_arguments.clear();
    d_schema.removeAll();
    d_data.removeAll();
    d_data1.removeAll();
    d_data2.removeAll();
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
    BSLS_ASSERT(BAEA_NOT_PARSED == d_state);

    int nonOptionRank = 0;
    for (volatile unsigned int i = 1; i < d_arguments.size(); ++i) {
        // Implementation note: rather than a deeply-nested binary
        // 'if .. else', we opt for a sequence of 'if (...) { ...; continue; }'
        // to keep the level of indentation at one (or at most two).

        // 1. Parse non-options:

        if ('-' != d_arguments[i][0]) {

            if ((int)d_nonOptionIndices.size() <= nonOptionRank) {
                stream << "The argument \"" << d_arguments[i]
                       << "\" was unexpected." << bsl::endl;
                location(stream, i);
                return -1;
            }

            int nonOptionIndex = d_nonOptionIndices[nonOptionRank];
            d_positions[nonOptionIndex].push_back(i);

            if (!d_options[nonOptionIndex].typeInfo().constraint()->parse(
                                                     d_data[nonOptionIndex],
                                                     stream,
                                                     d_arguments[i].c_str())) {
                location(stream, i);
                return -1;
            }

            // Check for multiple values for the non-option.  (Note that it
            // already has been validated that an array-valued non-option must
            // be the last non-option on the command line.)

            if (!d_options[nonOptionIndex].isArray()) {
                ++nonOptionRank;
            }

            continue;  // to next argument (++i)
        }

        // From now on, '-' == d_arguments[i][0] holds true.
        // 2. Parse single '-' character token:

        if (0 == d_arguments[i][1]) {  // -
            stream << "The character \"-\" was unexpected." << bsl::endl;
            location(stream, i);
            return -1;
        }

        // 3. Parse non-options following "--" :

        if ('-' == d_arguments[i][1] && 0 == d_arguments[i][2]) {
            // Error if no nonOptions or all nonOptions already provided.

            if ((int)d_nonOptionIndices.size() == nonOptionRank) {
                stream << "The argument \"--\" was unexpected." << bsl::endl;
                location(stream, i);
                return -1;
            }

            // Error if missing required option.  Note that we still have a
            // chance to provide values for required non-options, so do not
            // trigger an error for those; they will be checked in 'postParse'.

            int missingIndex = missing(false);
            if (missingIndex != -1) {
                stream << "No value supplied for the option \""
                       << d_options[missingIndex].tagString()
                       << "\"." << bsl::endl;
                return -1;
            }

            for (++i; i < d_arguments.size(); ++i) {
                int nonOptionIndex = d_nonOptionIndices[nonOptionRank];
                if (!d_options[nonOptionIndex].typeInfo().constraint()->parse(
                                                     d_data[nonOptionIndex],
                                                     stream,
                                                     d_arguments[i].c_str())) {
                    location(stream, i);
                    return -1;
                }
                d_positions[nonOptionIndex].push_back(i);

                if (!d_options[nonOptionIndex].isArray()) {
                    ++nonOptionRank;
                }
            }

            break;  // done with this command line
        }

        // 4. Parse long option: "--" followed by non-null char

        if ('-' == d_arguments[i][1]) {
            BSLS_ASSERT(0 != d_arguments[i][2]);

            if ('-' == d_arguments[i][2]) {
                stream << "Long option expected, instead of \"---\".\n"
                          "(Note: long options may not begin with '-'.)"
                       << bsl::endl;
                location(stream, i);
                return -1;
            }

            // Find syntax used: "--option=value" or "--option value".

            const char *start = &d_arguments[i].c_str()[2];
            const char *s     = start;
            while (*s && *s != '=') {
                ++s;
            }

            volatile int j = i;        // idx of value, either i or i + 1
            volatile int index = -1;   // index of long option in 'd_options'

            if ('=' == *s) { // example: i:"--port=13"
                index = findTag(start, s - start);
            } else {         // example: i:"--port" i+1:"13"
                index = findTag(&d_arguments[i][2], d_arguments[i].size() - 2);
            }

            // Check valid long option name.

            if (-1 == index) {
                stream << "The string \"" << bsl::string(start, s - start)
                       << "\" does not match any long tag." << bsl::endl;
                location(stream, i, 2, s - start + 1);
                return -1;
            }

            // Only now, compute string holding argument value.

            const CommandLineOption& thisOption = d_options[index];
            const char *str;  // string for parsing value
            if ('=' == *s) {
                str = (CommandLineOptionInfo::BAEA_FLAG ==
                                            thisOption.argType() ? "" : s + 1);
            } else {
                if (thisOption.argType() !=
                                       CommandLineOptionInfo::BAEA_FLAG) {
                    if (i == d_arguments.size() - 1) {
                        stream << "Error: No value has been provided "
                                  "for the option \""
                               << thisOption.tagString()
                               << "\"." << bsl::endl;
                        return -1;
                    }
                    str = d_arguments[j = i + 1].c_str();
                } else {
                    str = "";
                }
            }

            // Check for multiple values for the selected option.  Allow
            // multiple values for 'FLAG' options and array type options.  See
            // DRQS 14930958 for more details.

            if (!thisOption.isArray()
             && (CommandLineOptionInfo::BAEA_FLAG != thisOption.argType())
             && !d_positions[index].empty()) {
                stream << "Only one value can be provided for the option \""
                       << thisOption.tagString()
                       << "\"." << bsl::endl;
                location(stream, i, 2, s - &d_arguments[i][0] - 1);
                return -1;
            }

            // Parse value for (non-flag) option, using either syntax.
            // (Note: It is ok to use "--port=-1" to specify negative values.)

            if (!thisOption.typeInfo().constraint()->parse(
                                                 d_data[index], stream, str)) {
                location(stream,
                         i,
                         (s+1) - &d_arguments[i][0],
                         d_arguments[i].size() - 1);
                return -1;
            }
            d_positions[index].push_back(i);

            i = j;     // advance if syntax '--option value' (not a flag)
            continue;  // ++i
        }

        // 5. Parse short option: -a...
        // (Note: we allow to parse a series of short options concatenated but
        // have to stop whenever an option isn't a flag, as in Gnu tar, e.g.,
        // "-zcvf filename".)

        volatile int index;
        const char *s = &d_arguments[i].c_str()[1];
        while (*s && (index = findTag(*s)) != -1
           && CommandLineOptionInfo::BAEA_FLAG ==
                                                  d_options[index].argType()) {

            const CommandLineOption& thisOption = d_options[index];

            // Don't check for multiple values due to DRQS 14930958, allow
            // multiply-specified bool options.

            // Parsing a bool value should always succeed.

            if (!thisOption.typeInfo().constraint()->parse(
                                                  d_data[index], stream, "")) {
                BSLS_ASSERT(0);

                return -1;
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
                       << bsl::endl;
                location(stream, i, s - d_arguments[i].c_str());
                return -1;
            }

            // Process last short option, which has a value, using either
            // syntax "-ztvf=filename" or "-ztvf filename").

            volatile int j = i;
            const char *str = s + 1;
            const CommandLineOption& thisOption = d_options[index];

            if (0 == *str) {
                if (i == d_arguments.size() - 1) {
                    stream << "Error: No value has been provided for "
                              "the option \""
                           << thisOption.tagString()
                           << "\"." << bsl::endl;
                    return -1;
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
                       << "\"." << bsl::endl;
                location(stream, j, str - d_arguments[j].c_str());
                return -1;
            }

            // Parse value for (non-flag) option, using either syntax.  (Note:
            // It is ok to use "-p-1" or "-p=-1" to specify negative values.)

            if (!thisOption.typeInfo().constraint()->parse(
                                                   d_data[index], stream, str))
            {
                location(stream,
                         j,
                         str - d_arguments[j].c_str(),
                         d_arguments[j].size() - 1);
                return -1;
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
        const CommandLineOption& thisOption = d_options[index];

        if (CommandLineOptionInfo::BAEA_NON_OPTION ==
                                                        thisOption.argType()) {
            stream << "Error: No value supplied for the non-option \""
                   << thisOption.name()
                   << "\"." << bsl::endl;
        } else {
            stream << "Error: No value supplied for the option \""
                   << thisOption.tagString()
                   << "\"." << bsl::endl;
        }
        return -1;
    }

    bdlmxxx::RecordDef *record = d_schema.createRecord(recordName);

    for (volatile unsigned int i = 0; i < d_options.size(); ++i) {
        const CommandLineOption& thisOption = d_options[i];
        const CommandLineTypeInfo& info = thisOption.typeInfo();
        const CommandLineOccurrenceInfo& defaultInfo =
                                                   thisOption.occurrenceInfo();

        record->appendField(info.type(), thisOption.name().c_str());

        if (d_data[i].isNonNull()) {
     // if (positions[i].size() != 0
     //  || CommandLineOptionInfo::BAEA_FLAG == thisOption.argType()) {
            // Use p (i.e., entered value or bool false values) to load linked
            // var and binding.

            if (info.linkedVariable()) {
                const bdlmxxx::Descriptor *desc =
                               bdlmxxx::ElemAttrLookup::lookupTable()[info.type()];
                bdlmxxx::ElemRef(info.linkedVariable(), desc).replaceValue(
                                                                    d_data[i]);
            }

            d_data1.appendElement(d_data[i]);
        }
        else if (defaultInfo.hasDefaultValue()) {
            // Use default value to load linked variable and binding.
            // TBD  This might be redundant, since 'data[i]' was set in
            // 'validateAndInitialize'.  But to be safe...

            if (info.linkedVariable()) {
                const bdlmxxx::Descriptor *desc =
                               bdlmxxx::ElemAttrLookup::lookupTable()[info.type()];
                bdlmxxx::ElemRef(info.linkedVariable(), desc).replaceValue(
                                                   defaultInfo.defaultValue());
            }

            d_data1.appendElement(defaultInfo.defaultValue());
        }
        else {
            BSLS_ASSERT(!defaultInfo.isRequired());

            d_data1.appendNullElement(thisOption.typeInfo().type());
        }
    }
    return 0;
}

void CommandLine::validateAndInitialize()
{
    CommandLine::validateAndInitialize(bsl::cerr);
}

void CommandLine::validateAndInitialize(bsl::ostream& stream)
{
    bool isValid = true;

    for (volatile unsigned int i = 0; i < d_options.size(); ++i) {
        const CommandLineOption& thisOption = d_options[i];

        // Start with basic validity checks.  Try to do as many checks at a
        // time, in order to maximize the feedback to the user.

        if (!thisOption.isTagValid(stream)) {
            stream << "The error occurred while validating the "
                   << Ordinal(i) << " option." << bsl::endl;

            isValid = false;
        }

        if (!thisOption.isNameValid(stream)) {
            stream << "The error occurred while validating the "
                   << Ordinal(i) << " option." << bsl::endl;

            isValid = false;
        }

        if (!thisOption.isDescriptionValid(stream)) {
            stream << "The error occurred while validating the "
                   << Ordinal(i) << " option." << bsl::endl;

            isValid = false;
        }

        if (CommandLineOptionInfo::BAEA_FLAG == thisOption.argType()
         && thisOption.occurrenceInfo().hasDefaultValue())
        {
            stream << "No default value is allowed for the flag." << bsl::endl
                   << "The error occurred while validating the "
                   << Ordinal(i) << " option." << bsl::endl;

            isValid = false;
        }

        if (thisOption.occurrenceInfo().hasDefaultValue()
         && thisOption.occurrenceInfo().defaultValue().type() !=
                                              thisOption.typeInfo().type()) {
            stream << "The type of default value does not "
                      "match the type specified for the option."
                   << bsl::endl;
            stream << "The error occurred while validating the "
                   << Ordinal(i) << " option." << bsl::endl;

            isValid = false;
        }

        // Tags and names must be unique.

        for (volatile unsigned int j = 0; j < i; ++j) {
            if (thisOption.name() == d_options[j].name()) {
                stream << "Error: The names for the "
                       << Ordinal(i) << " and "
                       << Ordinal(j) << " options are equal."
                       << bsl::endl;

                isValid = false;
            }
        }

        if (thisOption.argType() !=
                                 CommandLineOptionInfo::BAEA_NON_OPTION) {
            for (volatile unsigned int j = 0; j < i; ++j) {
                if (d_options[j].argType() ==
                                 CommandLineOptionInfo::BAEA_NON_OPTION) {
                    continue;
                }

                if (thisOption.shortTag()
                 && thisOption.shortTag() == d_options[j].shortTag()) {
                    stream << "Error: short tags for the "
                           << Ordinal(i) << " and "
                           << Ordinal(j) << " options are equal."
                           << bsl::endl;

                    isValid = false;
                }

                if (!bsl::strcmp(thisOption.longTag(),
                                 d_options[j].longTag())) {
                    stream << "Error: long tags for the "
                           << Ordinal(i) << " and "
                           << Ordinal(j) << " options are equal."
                           << bsl::endl;

                    isValid = false;
                }
            }
        }

        // Additional checks for non-options.

        if (CommandLineOptionInfo::BAEA_NON_OPTION ==
                                                        thisOption.argType()) {
            // Non-options cannot be hidden.

            if (thisOption.occurrenceInfo().isHidden()) {
                stream << "Error: A non-option cannot be hidden." << bsl::endl
                       << "The error occurred while validating the "
                       << Ordinal(i) << " option." << bsl::endl;

                isValid = false;
            }
        }
    }

    if (!isValid) {
        EXIT_IF_FALSE(INVALID_COMMAND_LINE_SPEC);
    }

    for (volatile unsigned int i = 0; i < d_options.size(); ++i) {
        const CommandLineOption& thisOption = d_options[i];

        // Initialize 'd_positions' and 'd_data' for this option.

        bsl::vector<int> v;
        d_positions.push_back(v);
        d_data.appendNullElement(thisOption.typeInfo().type());

        // Validate and initialize with the default values.

        if (thisOption.occurrenceInfo().hasDefaultValue()) {
            if (!thisOption.typeInfo().checkConstraint(
                       thisOption.occurrenceInfo().defaultValue(), stream)) {
                stream << "The error occurred while validating the "
                       << Ordinal(i) << " option." << bsl::endl;

                EXIT_IF_FALSE(INVALID_COMMAND_LINE_SPEC);
            }
        }

        // Additional checks for non-options.

        if (CommandLineOptionInfo::BAEA_NON_OPTION ==
                                                        thisOption.argType()) {
            // Only last non-option can be multi-valued.

            if (!d_nonOptionIndices.empty()
             && d_options[d_nonOptionIndices.back()].isArray()) {
                stream << "Error: A multi-valued non-option was already "
                          "specified as the "
                       << Ordinal(d_nonOptionIndices.back())
                       << " option." << bsl::endl;
                stream << "The error occurred while validating the "
                       << Ordinal(i) << " option." << bsl::endl;

                EXIT_IF_FALSE(INVALID_COMMAND_LINE_SPEC);
            }

            // Once default value is provided, must *always* be provided for
            // subsequent non-options.

            if (!thisOption.occurrenceInfo().hasDefaultValue()
             && !d_nonOptionIndices.empty()
             && d_options[d_nonOptionIndices.back()].
                                          occurrenceInfo().hasDefaultValue()) {
                stream << "Error: A default value was provided "
                          "for the previous non-option, specified as\nthe "
                       << Ordinal(d_nonOptionIndices.back())
                       << " option, but not for this non-option." << bsl::endl;
                stream << "The error occurred while validating the "
                       << Ordinal(i) << " option." << bsl::endl;

                EXIT_IF_FALSE(INVALID_COMMAND_LINE_SPEC);
            }

            // Set 'd_nonOptionIndices' for this option.

            d_nonOptionIndices.push_back(i);
        }
    }
}

// PRIVATE ACCESSORS
int CommandLine::findTag(const char *longTag, int tagLength) const
{
    for (unsigned int i = 0; i < d_options.size(); ++i) {
        if (d_options[i].argType() !=
                                    CommandLineOptionInfo::BAEA_NON_OPTION
         && !bsl::strncmp(d_options[i].longTag(), longTag, tagLength)
         && bsl::strlen(d_options[i].longTag()) == (unsigned int)tagLength) {
            return i;
        }
    }
    return -1;
}

int CommandLine::findTag(char shortTag) const
{
    for (unsigned int i = 0; i < d_options.size(); ++i) {
        if (d_options[i].argType() !=
                                    CommandLineOptionInfo::BAEA_NON_OPTION
         && d_options[i].shortTag() == shortTag) {
            return i;
        }
    }
    return -1;
}

int CommandLine::findName(const bsl::string& name) const
{
    for (unsigned int i = 0; i < d_options.size(); ++i) {
        if (d_options[i].name() == name) {
            return i;
        }
    }
    return -1;
}

void CommandLine::location(bsl::ostream& stream,
                                int           argc,
                                int           start,
                                int           end) const
{
    if (end != -1) {
        stream << "The error occurred while parsing the "
               << Ordinal(start) << " to "
               << Ordinal(end) << " characters of the "
               << Ordinal(argc - 1) << " argument." << bsl::endl;
    }
    else if (start != -1) {
        stream << "The error occurred while parsing the "
               << Ordinal(start) << " character of the "
               << Ordinal(argc - 1) << " argument." << bsl::endl;
    }
    else {
        stream << "The error occurred while parsing the "
               << Ordinal(argc - 1) << " argument." << bsl::endl;
    }
}

int CommandLine::longestTagSize() const
{
    int max = 0;
    int len;
    for (unsigned int i = 0; i < d_options.size(); ++i) {
        if (d_options[i].argType() !=
                                    CommandLineOptionInfo::BAEA_NON_OPTION
         && (len = bsl::strlen(d_options[i].longTag())) > max) {
          max = len;
        }
    }
    return max;
}

int CommandLine::longestNameSize() const
{
    int max = 0;
    for (unsigned int i = 0; i < d_options.size(); ++i) {
        if ((int) d_options[i].name().size() > max) {
            max = d_options[i].name().size();
        }
    }
    return max;
}

int CommandLine::missing(bool checkAlsoNonOptions) const
{
    for (unsigned int i = 0; i < d_options.size(); ++i) {
        CommandLineOptionInfo::ArgType argType = d_options[i].argType();
        if (0 == d_positions[i].size()  // no value specified
         && CommandLineOptionInfo::BAEA_FLAG != argType
         && d_options[i].occurrenceInfo().isRequired()
         && (CommandLineOptionInfo::BAEA_OPTION == argType ||
                                                        checkAlsoNonOptions)) {
            return i;
        }
    }
    return -1;
}

// CREATORS
CommandLine::CommandLine(
                              const CommandLineOptionInfo *specTable,
                              int                               length,
                              bslma::Allocator                 *basicAllocator)
: d_options(basicAllocator)
, d_positions(basicAllocator)
, d_nonOptionIndices(basicAllocator)
, d_state(BAEA_INVALID)
, d_arguments(basicAllocator)
, d_schema(basicAllocator)
, d_data(basicAllocator)
, d_data1(basicAllocator)
, d_data2(basicAllocator)
, d_isBindin2Valid(false)
{
    d_options.reserve(length);
    for (int i = 0; i < length; ++i) {
        CommandLineOption tmp(specTable[i]);
        d_options.push_back(tmp);
    }
    validateAndInitialize(bsl::cerr);
    d_state = BAEA_NOT_PARSED;
}

CommandLine::CommandLine(
                              const CommandLineOptionInfo *specTable,
                              int                               length,
                              bsl::ostream&                     stream,
                              bslma::Allocator                 *basicAllocator)
: d_options(basicAllocator)
, d_positions(basicAllocator)
, d_nonOptionIndices(basicAllocator)
, d_state(BAEA_INVALID)
, d_arguments(basicAllocator)
, d_schema(basicAllocator)
, d_data(basicAllocator)
, d_data1(basicAllocator)
, d_data2(basicAllocator)
, d_isBindin2Valid(false)
{
    d_options.reserve(length);
    for (int i = 0; i < length; ++i) {
        CommandLineOption tmp(specTable[i]);
        d_options.push_back(tmp);
    }
    validateAndInitialize(stream);
    d_state = BAEA_NOT_PARSED;
}

CommandLine::CommandLine(const CommandLine&  original,
                                   bslma::Allocator        *basicAllocator)
: d_options(original.d_options, basicAllocator)
, d_positions(basicAllocator)
, d_nonOptionIndices(basicAllocator)
, d_state(BAEA_INVALID)
, d_arguments(basicAllocator)
, d_schema(basicAllocator)
, d_data(basicAllocator)
, d_data1(basicAllocator)
, d_data2(basicAllocator)
, d_isBindin2Valid(false)
{
    BSLS_ASSERT(original.d_state != BAEA_INVALID);

    // TBD
    // This isn't a typical copy constructor, because it has side-effects like
    // outputting to 'bsl::cerr'.  On the other hand, it could be replaced by a
    // more traditional one, because the assertions indicate that nothing
    // *should* be output to 'bsl::cerr'.  There are no other reasons to have
    // such a complicated implementation.  This should be checked in the test
    // driver.

    validateAndInitialize(bsl::cerr);
    d_state = BAEA_NOT_PARSED;
    if (BAEA_PARSED == original.d_state) {
        d_arguments = original.d_arguments;
        const int status = parse(bsl::cerr);

        BSLS_ASSERT(0 == status);

        d_state = BAEA_PARSED;
    }
}

CommandLine::~CommandLine()
{
}

// MANIPULATORS
CommandLine& CommandLine::operator=(const CommandLine& rhs)
{
    if (&rhs != this) {
        BSLS_ASSERT(d_state != BAEA_INVALID);
        BSLS_ASSERT(rhs.d_state != BAEA_INVALID);

        clear();

        d_options.reserve(rhs.d_options.size());
        for (unsigned int i = 0; i < rhs.d_options.size(); ++i) {
            d_options.push_back(rhs.d_options[i]);
        }

        validateAndInitialize(bsl::cerr);
        d_state = BAEA_NOT_PARSED;

        if (BAEA_PARSED == rhs.d_state) {
            d_arguments = rhs.d_arguments;
            const int status = parse(bsl::cerr);

            BSLS_ASSERT(0 == status);

            d_state = BAEA_PARSED;
        }
    }
    return *this;
}

int CommandLine::parse(int argc, const char * const argv[])
{
    return parse(argc, argv, bsl::cerr);
}

int CommandLine::parse(int argc, const char * const argv[],
                            bsl::ostream& stream)
{
    initArgs(argc, argv);

    if (parse(stream)) {
        d_state = BAEA_INVALID;
        return -1;
    }
    d_state = BAEA_PARSED;
    return 0;
}

// ACCESSORS
bdem_ConstRowBinding CommandLine::data() const
{
    BSLS_ASSERT(BAEA_PARSED == d_state);

    return bdem_ConstRowBinding(&d_data1, &d_schema, recordName);
}

bool CommandLine::isInvalid() const
{
    return BAEA_INVALID == d_state;
}

bool CommandLine::isParsed() const
{
    return BAEA_PARSED == d_state;
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

int CommandLine::numSpecified(const bsl::string& name) const
{
    const int index = findName(name);
    return 0 <= index ? d_positions[index].size() : 0;
}

const bsl::vector<int>&
CommandLine::positions(const bsl::string& name) const
{
    const int index = findName(name);

    BSLS_ASSERT(index >= 0);

    return d_positions[index];
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

bdem_ConstRowBinding CommandLine::specifiedData() const
{
    BSLS_ASSERT(BAEA_PARSED == d_state);

    if (!d_isBindin2Valid) {
        // This is a 'const' method that needs to "cache" the values of
        // 'd_data2', which is therefore declared 'mutable' along with the
        // 'd_isBindin2Valid' flag.  This is acceptable because those members
        // will never change after being initialized, unless 'clear' is called
        // by 'operator=', and thus will remain "in sync" with the value of
        // this command line object.

        d_isBindin2Valid = true;
        d_data2 = d_data1;
        for (int index = 0; index < d_data1.length(); ++index) {
            if (!isSpecified(d_options[index].name())) {
                d_data2.resetElement(index, d_data1.elemType(index));
            }
        }
    }
    return bdem_ConstRowBinding(&d_data2, &d_schema, recordName);
}

bool CommandLine::theBool(const bsl::string& name) const
{
    const int index = findName(name);

    BSLS_ASSERT(index >= 0);
    BSLS_ASSERT(
          CommandLineOptionInfo::BAEA_FLAG == d_options[index].argType());

    return data().theBool(name.c_str());
}

char CommandLine::theChar(const bsl::string& name) const
{
    return data().theChar(name.c_str());
}

short CommandLine::theShort(const bsl::string& name) const
{
    return data().theShort(name.c_str());
}

int CommandLine::theInt(const bsl::string& name) const
{
    return data().theInt(name.c_str());
}

bsls::Types::Int64
CommandLine::theInt64(const bsl::string& name) const
{
    return data().theInt64(name.c_str());
}

float CommandLine::theFloat(const bsl::string& name) const
{
    return data().theFloat(name.c_str());
}

double CommandLine::theDouble(const bsl::string& name) const
{
    return data().theDouble(name.c_str());
}

const bsl::string& CommandLine::theString(const bsl::string& name) const
{
    return data().theString(name.c_str());
}

const bdlt::Datetime&
CommandLine::theDatetime(const bsl::string& name) const
{
    return data().theDatetime(name.c_str());
}

const bdlt::Date& CommandLine::theDate(const bsl::string& name) const
{
    return data().theDate(name.c_str());
}

const bdlt::Time& CommandLine::theTime(const bsl::string& name) const
{
    return data().theTime(name.c_str());
}

const bsl::vector<char>&
CommandLine::theCharArray(const bsl::string& name) const
{
    return data().theCharArray(name.c_str());
}

const bsl::vector<short>&
CommandLine::theShortArray(const bsl::string& name) const
{
    return data().theShortArray(name.c_str());
}

const bsl::vector<int>&
CommandLine::theIntArray(const bsl::string& name) const
{
    return data().theIntArray(name.c_str());
}

const bsl::vector<bsls::Types::Int64>&
CommandLine::theInt64Array(const bsl::string& name) const
{
    return data().theInt64Array(name.c_str());
}

const bsl::vector<float>&
CommandLine::theFloatArray(const bsl::string& name) const
{
    return data().theFloatArray(name.c_str());
}

const bsl::vector<double>&
CommandLine::theDoubleArray(const bsl::string& name) const
{
    return data().theDoubleArray(name.c_str());
}

const bsl::vector<bsl::string>&
CommandLine::theStringArray(const bsl::string& name) const
{
    return data().theStringArray(name.c_str());
}

const bsl::vector<bdlt::Datetime>&
CommandLine::theDatetimeArray(const bsl::string& name) const
{
    return data().theDatetimeArray(name.c_str());
}

const bsl::vector<bdlt::Date>&
CommandLine::theDateArray(const bsl::string& name) const
{
    return data().theDateArray(name.c_str());
}

const bsl::vector<bdlt::Time>&
CommandLine::theTimeArray(const bsl::string& name) const
{
    return data().theTimeArray(name.c_str());
}

void CommandLine::printUsage() const
{
    printUsage(bsl::cerr);
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
          case CommandLineOptionInfo::BAEA_FLAG:
          case CommandLineOptionInfo::BAEA_OPTION: {
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
            if (CommandLineOptionInfo::BAEA_FLAG != 
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
          case CommandLineOptionInfo::BAEA_NON_OPTION: {
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
    stream << bsl::endl;
    stream << usage;
    format(usage.size(), end, options, stream, usage.size());

    stream << "\nWhere:\n";

    bsl::string temp;
    temp.append(start, ' ');
    temp.append(1, '-');
    temp.append(1, 'X');
    temp.append(" | --");

    int maxTag         = longestTagSize();
    int nameAngleStart = temp.size() + maxTag + 2;
    int maxName        = longestNameSize();
    int dscrStart      = nameAngleStart + 1 + maxName + 3;

    for (unsigned int i = 0; i < d_options.size(); ++i) {
        if (d_options[i].argType() !=
                                 CommandLineOptionInfo::BAEA_NON_OPTION) {
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
            str.append(nameAngleStart - str.size(), ' ');

            if (CommandLineOptionInfo::BAEA_OPTION ==
                                                      d_options[i].argType()) {
                str.append(1, '<').append(d_options[i].name()).append(1, '>');
            }

            str.append(dscrStart - str.size(), ' ');
            stream << str;

            str = d_options[i].description();
            if (CommandLineOptionInfo::BAEA_OPTION ==
                                                         d_options[i].argType()
             && d_options[i].occurrenceInfo().hasDefaultValue()) {
                bsl::ostringstream oss;
                oss << d_options[i].occurrenceInfo().defaultValue();
                str += " (default: " + oss.str() + ')';
            }
            format(dscrStart, end, str, stream, dscrStart);
        }
        else {
            bsl::string str;
            str.append(nameAngleStart, ' ');
            str.append(1, '<');
            str.append(d_options[i].name());
            str.append(1, '>');
            str.append(dscrStart - str.size(), ' ');
            stream << str;

            str = d_options[i].description();
            if (d_options[i].occurrenceInfo().hasDefaultValue()) {
                bsl::ostringstream oss;
                oss << d_options[i].occurrenceInfo().defaultValue();
                str += " (default: " + oss.str() + ')';
            }
            format(dscrStart, end, str, stream, dscrStart);
        }
        stream << bsl::endl;
    }
}

bsl::ostream& CommandLine::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    BSLS_ASSERT(0 <= spacesPerLevel);

    if (BAEA_PARSED == d_state) {
        const bdlmxxx::Descriptor *desc =
                  bdlmxxx::ElemAttrLookup::lookupTable()[bdlmxxx::ElemType::BDEM_LIST];
        bdlmxxx::SchemaAggregateUtil::print(stream,
                                        bdlmxxx::ConstElemRef(&d_data1, desc),
                                        d_schema.lookupRecord(recordName),
                                        level,
                                        spacesPerLevel);
    } else if (BAEA_INVALID == d_state) {
        stream << "INVALID";
    } else {
        stream << "UNPARSED";
    }
    return stream;
}
}  // close package namespace

// FREE OPERATORS
bool balb::operator==(const CommandLine& lhs, const CommandLine& rhs)
{
    return lhs.isParsed() && rhs.isParsed() && lhs.data() == rhs.data();
}

bool balb::operator!=(const CommandLine& lhs, const CommandLine& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& balb::operator<<(bsl::ostream& stream, const CommandLine& rhs)
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

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
