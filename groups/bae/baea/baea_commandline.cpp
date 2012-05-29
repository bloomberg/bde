// baea_commandline.cpp                                               -*-C++-*-
#include <baea_commandline.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baea_commandline_cpp,"$Id$ $CSID$")

///IMPLEMENTATION NOTES: This is a somewhat unique component in that it lives
// only in offlines, and is most often used with command-line specs that *must*
// *be* a statically-created array.  For that reason, there are two classes
// that serve the same purpose: 'baea_CommandLineOptionInfo' is a
// statically-initializable class and violates the 'bdema' allocator protocol,
// while 'baea_CommandLineOption' is convertible from
// 'baea_CommandLineOptionInfo' takes allocators and is suitable for storing
// into containers.
//
// As a result, Purify gets somewhat confused about UMRs, memory leaks, and
// ABRs.  Moreover, in optimized mode, Purify would also get confused about the
// ordering of the variables and crash with core dumped...  Somehow, adding
// 'volatile' to the loop variables seemed to fix the issue, we still don't
// know why.  This is fragile and it would be a good idea to keep purifying the
// test driver for each release and see if the crash re-occurs.
//
// Another note: the class 'baea_CommandLineOptionType' contains static
// initializers (see 'CLASS DATA' below) which are null pointers of the
// respective types, and are used only for readability of the
// 'baea_CommandLineTypeInfo' constructor calls (specifying the type of an
// option without a linked variable).

#include <bdem_aggregateoption.h>
#include <bdem_descriptor.h>
#include <bdem_elemattrlookup.h>
#include <bdem_properties.h>
#include <bdem_schemaaggregateutil.h>

#include <bdepu_typesparser.h>

#include <bdet_date.h>
#include <bdet_datetime.h>
#include <bdet_time.h>

#include <bdeu_print.h>

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
        BloombergLP::bsls_Assert::invokeHandler(#X, __FILE__, __LINE__);  \
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

                            // =========================
                            // local struct baea_Ordinal
                            // =========================

struct baea_Ordinal {
    // This 'struct' assists in printing numbers as ordinals (1st, 2nd, etc.).

    bsl::size_t d_rank;  // rank (starting at 0)

    baea_Ordinal(bsl::size_t n) : d_rank(n) {}
        // Create an ordinal for the specified position 'n' (starting at 0).
};

bsl::ostream& operator<<(bsl::ostream& stream, baea_Ordinal position);
    // Output the specified 'position' (starting at 0) to the specified
    // 'stream' as an ordinal, mapping 0 to "1st", 1 to "2nd", 3 to "3rd", 4
    // to "4th", etc. following correct English usage.

bsl::ostream& operator<<(bsl::ostream& stream, baea_Ordinal position)
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

bool parseValue(void *value, const char *input, bdem_ElemType::Type type)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(input);

    bool result;
    const char *end;

    switch (type) {
      case bdem_ElemType::BDEM_CHAR: {
        *(char *)value = *input;
        result = *input;  // succeeds if non-null character
      } break;
      case bdem_ElemType::BDEM_SHORT: {
        result = !bdepu_TypesParser::parseShort(&end, (short *)value, input);
      } break;
      case bdem_ElemType::BDEM_INT: {
        result = !bdepu_TypesParser::parseInt(&end, (int *)value, input);
      } break;
      case bdem_ElemType::BDEM_INT64: {
        result = !bdepu_TypesParser::parseInt64(
                                             &end,
                                             (bsls_PlatformUtil::Int64 *)value,
                                             input);
      } break;
      case bdem_ElemType::BDEM_FLOAT: {
        result = !bdepu_TypesParser::parseFloat(&end, (float *)value, input);
      } break;
      case bdem_ElemType::BDEM_DOUBLE: {
        result = !bdepu_TypesParser::parseDouble(&end, (double *)value, input);
      } break;
      case bdem_ElemType::BDEM_STRING: {
        *(bsl::string *)value = input;
        result = true;
      } break;
      case bdem_ElemType::BDEM_DATETIME: {
        result = !bdepu_TypesParser::parseDatetime(&end,
                                                   (bdet_Datetime *)value,
                                                   input);
      } break;
      case bdem_ElemType::BDEM_DATE: {
        result = !bdepu_TypesParser::parseDate(&end,
                                               (bdet_Date *)value,
                                               input);
      } break;
      case bdem_ElemType::BDEM_TIME: {
        result = !bdepu_TypesParser::parseTime(&end,
                                               (bdet_Time *)value,
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

const char *elemTypeToString(bdem_ElemType::Type elemType)
{
    const char *typeString;

    switch (elemType) {
      case bdem_ElemType::BDEM_CHAR: {
        typeString = "A char";
      } break;
      case bdem_ElemType::BDEM_SHORT: {
        typeString = "A short";
      } break;
      case bdem_ElemType::BDEM_INT: {
        typeString = "An integer";
      } break;
      case bdem_ElemType::BDEM_INT64: {
        typeString = "A 64-bit integer";
      } break;
      case bdem_ElemType::BDEM_FLOAT: {
        typeString = "A float";
      } break;
      case bdem_ElemType::BDEM_DOUBLE: {
        typeString = "A double";
      } break;
      case bdem_ElemType::BDEM_STRING: {
        typeString = "A string";
      } break;
      case bdem_ElemType::BDEM_DATETIME: {
        typeString = "A 'bdet_Datetime'";
      } break;
      case bdem_ElemType::BDEM_DATE: {
        typeString = "A 'bdet_Date'";
      } break;
      case bdem_ElemType::BDEM_TIME: {
        typeString = "A 'bdet_Time'";
      } break;
      default: {
        BSLS_ASSERT(0);
        typeString = "An unknown type";
      } break;
    }

    return typeString;
}

}  // close unnamed namespace

                   // =======================================
                   // local class baea_CommandLine_Constraint
                   // =======================================

class baea_CommandLine_Constraint {

  public:
    // CREATORS
    virtual ~baea_CommandLine_Constraint();
        // Destroy this polymorphic constraint object.

    // ACCESSORS
    virtual bdem_ElemType::Type type() const = 0;
        // Return the 'bdem' element type of this constraint object.

    virtual bool parse(const bdem_ElemRef& element,
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

    virtual bool validate(const bdem_ConstElemRef& element) const = 0;
    virtual bool validate(const bdem_ConstElemRef& element,
                          bsl::ostream&            stream) const = 0;
        // Return 'true' if the specified 'element' satisfies this constraint,
        // and 'false' otherwise.  Optionally specify a 'stream' to which a
        // descriptive error message is written if this constraint is not
        // satisfied.  The behavior is undefined unless 'element' is bound to a
        // variable of the same type as this constraint.
};

// CREATORS
baea_CommandLine_Constraint::~baea_CommandLine_Constraint()
{
}

namespace {

                         // ==========================
                         // local class BoolConstraint
                         // ==========================

class BoolConstraint : public baea_CommandLine_Constraint {
    // This concrete implementation of the 'baea_CommandLine_Constraint'
    // protocol provides a 'bool' empty constraint which always returns 'true'
    // after this option has been parsed.  The individual contracts for each
    // method are identical to the protocol and not repeated here.

    // NOT IMPLEMENTED
    BoolConstraint(const BoolConstraint&);
    BoolConstraint& operator=(const BoolConstraint&);

  public:
    // CREATORS
    BoolConstraint(bslma_Allocator *basicAllocator = 0);
        // Create a 'bool' constraint object.  Optionally specify a
        // 'basicAllocator', which is ignored.

    ~BoolConstraint();
        // Destroy this object.

    // ACCESSORS
    bdem_ElemType::Type type() const;
        // Return 'bdem_ElemType::BDEM_BOOL'.

    bool parse(const bdem_ElemRef& element,
               bsl::ostream&       stream,
               const bsl::string&  input) const;
        // Store the value 'true' into the specified 'element'.

    bool validate(const bdem_ConstElemRef& element) const;
    bool validate(const bdem_ConstElemRef& element,
                  bsl::ostream&            stream) const;
        // Return 'true', and leave the optionally specified 'stream'
        // unchanged.  The behavior is undefined unless 'element' is bound to a
        // variable of 'bool' type.
};

// CREATORS
BoolConstraint::BoolConstraint(bslma_Allocator *)
{
}

BoolConstraint::~BoolConstraint()
{
}

// ACCESSORS
bdem_ElemType::Type BoolConstraint::type() const
{
    return bdem_ElemType::BDEM_BOOL;
}

bool BoolConstraint::parse(const bdem_ElemRef& element,
                           bsl::ostream&       ,
                           const bsl::string&) const
{
    BSLS_ASSERT(element.isBound());
    BSLS_ASSERT(bdem_ElemType::BDEM_BOOL == element.type());

    element.theModifiableBool() = true;
    return true;
}

bool BoolConstraint::validate(const bdem_ConstElemRef&) const
{
    return true;
}

bool BoolConstraint::validate(const bdem_ConstElemRef&, bsl::ostream&) const
{
    return true;
}

                   // =================================
                   // local class ScalarConstraint<...>
                   // =================================

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
class ScalarConstraint : public baea_CommandLine_Constraint {
    // This concrete implementation of the 'baea_CommandLine_Constraint'
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
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    ScalarConstraint(const CONSTRAINT_TYPE&  constraint,
                     bslma_Allocator        *basicAllocator = 0);
        // Create a constraint object storing the specified 'constraint'
        // functor.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently-installed default
        // allocator is used.

    ~ScalarConstraint();
        // Destroy this polymorphic constraint object.

    // ACCESSORS
    bdem_ElemType::Type type() const;
        // Return the 'bdem' element type of this constraint.

    bool parse(const bdem_ElemRef& element,
               bsl::ostream&       stream,
               const bsl::string&  input) const;
        // Load into the instance of parameterized 'TYPE' stored in the
        // specified 'element' the result of parsing the specified 'input',
        // interpreted as an instance of 'TYPE'.  Return 'true' if parsing
        // succeeds and the parsed value satisfies the constraint.  Return
        // 'false' otherwise, and write to the specified 'stream' a descriptive
        // error message.  The behavior is undefined unless 'element' is bound
        // to a valid instance of 'TYPE'.

    bool validate(const bdem_ConstElemRef& element) const;
    bool validate(const bdem_ConstElemRef& element,
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
                                        bslma_Allocator        *basicAllocator)
: d_constraint(constraint, basicAllocator)
{
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
ScalarConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::~ScalarConstraint()
{
}

// ACCESSORS
template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bdem_ElemType::Type
ScalarConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::type() const
{
    return bdem_ElemType::Type(ELEM_TYPE);
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bool
ScalarConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::parse(
                                               const bdem_ElemRef& element,
                                               bsl::ostream&       stream,
                                               const bsl::string&  input) const
{
    const bdem_ElemType::Type elemType = element.type();

    BSLS_ASSERT(element.isBound());
    BSLS_ASSERT(ELEM_TYPE == elemType);

    TYPE value = TYPE();  // initialize to quell purify UMR
    if (!parseValue(&value, input.c_str(), elemType)) {
        stream << elemTypeToString(elemType)
               << " value was expected, instead of \""
               << input << "\"" << bsl::endl;
        return false;
    }
    const bdem_Descriptor *desc = bdem_ElemAttrLookup::lookupTable()[elemType];
    element.replaceValue(bdem_ConstElemRef(&value, desc));
    return validate(element, stream);
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bool
ScalarConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::validate(
                                        const bdem_ConstElemRef& element) const
{
    bsl::ostringstream oss;
    return validate(element, oss);
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bool
ScalarConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::validate(
                                         const bdem_ConstElemRef& element,
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
                       baea_CommandLineConstraint::CharConstraint,
                       bdem_ElemType::BDEM_CHAR>;
template
class ScalarConstraint<short,
                       baea_CommandLineConstraint::ShortConstraint,
                       bdem_ElemType::BDEM_SHORT>;
template
class ScalarConstraint<int,
                       baea_CommandLineConstraint::IntConstraint,
                       bdem_ElemType::BDEM_INT>;
template
class ScalarConstraint<bsls_PlatformUtil::Int64,
                       baea_CommandLineConstraint::Int64Constraint,
                       bdem_ElemType::BDEM_INT64>;
template
class ScalarConstraint<float,
                       baea_CommandLineConstraint::FloatConstraint,
                       bdem_ElemType::BDEM_FLOAT>;
template
class ScalarConstraint<double,
                       baea_CommandLineConstraint::DoubleConstraint,
                       bdem_ElemType::BDEM_DOUBLE>;
template
class ScalarConstraint<bsl::string,
                       baea_CommandLineConstraint::StringConstraint,
                       bdem_ElemType::BDEM_STRING>;
template
class ScalarConstraint<bdet_Datetime,
                       baea_CommandLineConstraint::DatetimeConstraint,
                       bdem_ElemType::BDEM_DATETIME>;
template
class ScalarConstraint<bdet_Date,
                       baea_CommandLineConstraint::DateConstraint,
                       bdem_ElemType::BDEM_DATE>;
template
class ScalarConstraint<bdet_Time,
                       baea_CommandLineConstraint::TimeConstraint,
                       bdem_ElemType::BDEM_TIME>;
    // Explicit specializations (to force instantiation of all variants).

                 // ================================
                 // local class ArrayConstraint<...>
                 // ================================

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
class ArrayConstraint : public baea_CommandLine_Constraint {
    // This concrete implementation of the 'baea_CommandLine_Constraint'
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
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    ArrayConstraint(const CONSTRAINT_TYPE&  constraint,
                    bslma_Allocator        *basicAllocator);
        // Create a constraint object storing the specified 'constraint'
        // functor.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently-installed default
        // allocator is used.

    ~ArrayConstraint();
        // Destroy this polymorphic constraint object.

    // ACCESSORS
    bdem_ElemType::Type type() const;
        // Return the 'bdem' element type of this constraint.

    bool parse(const bdem_ElemRef& element,
               bsl::ostream&       stream,
               const bsl::string&  input) const;
        // Append to the instance of 'bsl::vector<TYPE>' stored in the
        // specified 'element' the result of parsing the specified 'input',
        // interpreted as an instance of parameterized 'TYPE'.  Return 'true'
        // if parsing succeeds and the parsed value satisfies the constraint.
        // Return 'false' otherwise, and write to the specified 'stream' a
        // descriptive error message.  The behavior is undefined unless
        // 'element' is bound to a valid instance of 'bsl::vector<TYPE>'.

    bool validate(const bdem_ConstElemRef& element) const;
    bool validate(const bdem_ConstElemRef& element,
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
                                        bslma_Allocator        *basicAllocator)
: d_constraint(constraint, basicAllocator)
{
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
ArrayConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::~ArrayConstraint()
{
}

// ACCESSORS
template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bdem_ElemType::Type
ArrayConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::type() const
{
    return bdem_ElemType::Type(ELEM_TYPE);
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bool
ArrayConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::parse(
                                               const bdem_ElemRef& element,
                                               bsl::ostream&       stream,
                                               const bsl::string&  input) const
{
    const bdem_ElemType::Type elemType = element.type();

    BSLS_ASSERT(element.isBound());
    BSLS_ASSERT(ELEM_TYPE == elemType);

    bdem_ElemType::Type scalarType = bdem_ElemType::fromArrayType(elemType);
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
        const bdem_Descriptor *desc =
                                  bdem_ElemAttrLookup::lookupTable()[elemType];
        element.replaceValue(bdem_ConstElemRef(&tmp, desc));
    }
    ((bsl::vector<TYPE> *)element.data())->push_back(value);

    return true;
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bool
ArrayConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::validate(
                                        const bdem_ConstElemRef& element) const
{
    bsl::ostringstream oss;
    return validate(element, oss);
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bool
ArrayConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::validate(
                                         const bdem_ConstElemRef& element,
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
                       << baea_Ordinal(i)
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
                      baea_CommandLineConstraint::CharConstraint,
                      bdem_ElemType::BDEM_CHAR_ARRAY>;
template
class ArrayConstraint<short,
                      baea_CommandLineConstraint::ShortConstraint,
                      bdem_ElemType::BDEM_SHORT_ARRAY>;
template
class ArrayConstraint<int,
                      baea_CommandLineConstraint::IntConstraint,
                      bdem_ElemType::BDEM_INT_ARRAY>;
template
class ArrayConstraint<bsls_PlatformUtil::Int64,
                      baea_CommandLineConstraint::Int64Constraint,
                      bdem_ElemType::BDEM_INT64_ARRAY>;
template
class ArrayConstraint<float,
                      baea_CommandLineConstraint::FloatConstraint,
                      bdem_ElemType::BDEM_FLOAT_ARRAY>;
template
class ArrayConstraint<double,
                      baea_CommandLineConstraint::DoubleConstraint,
                      bdem_ElemType::BDEM_DOUBLE_ARRAY>;
template
class ArrayConstraint<bsl::string,
                      baea_CommandLineConstraint::StringConstraint,
                      bdem_ElemType::BDEM_STRING_ARRAY>;
template
class ArrayConstraint<bdet_Datetime,
                      baea_CommandLineConstraint::DatetimeConstraint,
                      bdem_ElemType::BDEM_DATETIME_ARRAY>;
template
class ArrayConstraint<bdet_Date,
                      baea_CommandLineConstraint::DateConstraint,
                      bdem_ElemType::BDEM_DATE_ARRAY>;
template
class ArrayConstraint<bdet_Time,
                      baea_CommandLineConstraint::TimeConstraint,
                      bdem_ElemType::BDEM_TIME_ARRAY>;
    // Explicit specializations (to force instantiation of all variants).

                      // ==================================
                      // local typedefs TYPEConstraint<...>
                      // ==================================

// Note: for 'BoolConstraint', see non-'typedef' implementations.

typedef ScalarConstraint<char,
                         baea_CommandLineConstraint::CharConstraint,
                         bdem_ElemType::BDEM_CHAR>        CharConstraint;
    // The type 'CharConstraint' is an alias for a scalar constraint on type
    // 'char' using the 'baea_CommandLineConstraint::CharConstraint' functor.

typedef ScalarConstraint<short,
                         baea_CommandLineConstraint::ShortConstraint,
                         bdem_ElemType::BDEM_SHORT>       ShortConstraint;
    // The type 'ShortConstraint' is an alias for a scalar constraint on type
    // 'short' using the 'baea_CommandLineConstraint::ShortConstraint' functor.

typedef ScalarConstraint<int,
                         baea_CommandLineConstraint::IntConstraint,
                         bdem_ElemType::BDEM_INT>         IntConstraint;
    // The type 'IntConstraint' is an alias for a scalar constraint on type
    // 'int' using the 'baea_CommandLineConstraint::IntConstraint' functor.

typedef ScalarConstraint<bsls_PlatformUtil::Int64,
                         baea_CommandLineConstraint::Int64Constraint,
                         bdem_ElemType::BDEM_INT64>       Int64Constraint;
    // The type 'Int64Constraint' is an alias for a scalar constraint on type
    // 'bsls_PlatformUtil::Int64' using the
    // 'baea_CommandLineConstraint::Int64Constraint' functor.

typedef ScalarConstraint<float,
                         baea_CommandLineConstraint::FloatConstraint,
                         bdem_ElemType::BDEM_FLOAT>       FloatConstraint;
    // The type 'FloatConstraint' is an alias for a scalar constraint on type
    // 'float' using the 'baea_CommandLineConstraint::FloatConstraint'
    // functor.

typedef ScalarConstraint<double,
                         baea_CommandLineConstraint::DoubleConstraint,
                         bdem_ElemType::BDEM_DOUBLE>      DoubleConstraint;
    // The type 'DoubleConstraint' is an alias for a scalar constraint on type
    // 'double' using the 'baea_CommandLineConstraint::DoubleConstraint'
    // functor.

typedef ScalarConstraint<bsl::string,
                         baea_CommandLineConstraint::StringConstraint,
                         bdem_ElemType::BDEM_STRING>      StringConstraint;
    // The type 'StringConstraint' is an alias for a scalar constraint on type
    // 'bsl::string' using the 'baea_CommandLineConstraint::StringConstraint'
    // functor.

typedef ScalarConstraint<bdet_Datetime,
                         baea_CommandLineConstraint::DatetimeConstraint,
                         bdem_ElemType::BDEM_DATETIME>    DatetimeConstraint;
    // The type 'DatetimeConstraint' is an alias for a scalar constraint of
    // type 'bdet_Datetime' using the
    // 'baea_CommandLineConstraint::DatetimeConstraint' functor.

typedef ScalarConstraint<bdet_Date,
                         baea_CommandLineConstraint::DateConstraint,
                         bdem_ElemType::BDEM_DATE>        DateConstraint;
    // The type 'DateConstraint' is an alias for a scalar constraint on type
    // 'bdet_Date' using the 'baea_CommandLineConstraint::DateConstraint'
    // functor.

typedef ScalarConstraint<bdet_Time,
                         baea_CommandLineConstraint::TimeConstraint,
                         bdem_ElemType::BDEM_TIME>        TimeConstraint;
    // The type 'TimeConstraint' is an alias for a scalar constraint on type
    // 'bdet_Time' using the 'baea_CommandLineConstraint::TimeConstraint'
    // functor.

                    // =======================================
                    // local typedefs TYPEArrayConstraint<...>
                    // =======================================

typedef ArrayConstraint<char,
                        baea_CommandLineConstraint::CharConstraint,
                        bdem_ElemType::BDEM_CHAR_ARRAY>  CharArrayConstraint;
    // The type 'CharArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<char>' using the
    // 'baea_CommandLineConstraint::CharConstraint' functor.

typedef ArrayConstraint<short,
                        baea_CommandLineConstraint::ShortConstraint,
                        bdem_ElemType::BDEM_SHORT_ARRAY> ShortArrayConstraint;
    // The type 'ShortArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<short>' using the
    // 'baea_CommandLineConstraint::ShortConstraint' functor.

typedef ArrayConstraint<int,
                        baea_CommandLineConstraint::IntConstraint,
                        bdem_ElemType::BDEM_INT_ARRAY>   IntArrayConstraint;
    // The type 'IntArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<int>' using the
    // 'baea_CommandLineConstraint::IntConstraint' functor.

typedef ArrayConstraint<bsls_PlatformUtil::Int64,
                       baea_CommandLineConstraint::Int64Constraint,
                       bdem_ElemType::BDEM_INT64_ARRAY> Int64ArrayConstraint;
    // The type 'Int64ArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<bsls_PlatformUtil::Int64>' using the
    // 'baea_CommandLineConstraint::Int64Constraint' functor.

typedef ArrayConstraint<float,
                        baea_CommandLineConstraint::FloatConstraint,
                        bdem_ElemType::BDEM_FLOAT_ARRAY> FloatArrayConstraint;
    // The type 'FloatArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<float>' using the
    // 'baea_CommandLineConstraint::FloatConstraint' functor.

typedef ArrayConstraint<double,
                        baea_CommandLineConstraint::DoubleConstraint,
                        bdem_ElemType::BDEM_DOUBLE_ARRAY>
                                                         DoubleArrayConstraint;
    // The type 'DoubleArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<double>' using the
    // 'baea_CommandLineConstraint::DoubleConstraint' functor.

typedef ArrayConstraint<bsl::string,
                        baea_CommandLineConstraint::StringConstraint,
                        bdem_ElemType::BDEM_STRING_ARRAY>
                                                         StringArrayConstraint;
    // The type 'StringArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<bsl::string>' using the
    // 'baea_CommandLineConstraint::StringConstraint' functor.

typedef ArrayConstraint<bdet_Datetime,
                        baea_CommandLineConstraint::DatetimeConstraint,
                        bdem_ElemType::BDEM_DATETIME_ARRAY>
                                                       DatetimeArrayConstraint;
    // The type 'DatetimeArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<bdet_Datetime>' using the
    // 'baea_CommandLineConstraint::DatetimeConstraint' functor.

typedef ArrayConstraint<bdet_Date,
                        baea_CommandLineConstraint::DateConstraint,
                        bdem_ElemType::BDEM_DATE_ARRAY>  DateArrayConstraint;
    // The type 'DateArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<bdet_Date>' using the
    // 'baea_CommandLineConstraint::DateConstraint' functor.

typedef ArrayConstraint<bdet_Time,
                        baea_CommandLineConstraint::TimeConstraint,
                        bdem_ElemType::BDEM_TIME_ARRAY>  TimeArrayConstraint;
    // The type 'TimeArrayConstraint' is an alias for a constraint on type
    // 'bsl::vector<bdet_Time>' using the
    // 'baea_CommandLineConstraint::TimeConstraint' functor.

}  // close unnamed namespace

// ============================================================================
//                                  COMPONENT
// ============================================================================

                    // ------------------------------------
                    // class baea_CommandLineOccurrenceInfo
                    // ------------------------------------

// CREATORS
baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                               bslma_Allocator *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                               OccurrenceType   type,
                                               bslma_Allocator *basicAllocator)
: d_isRequired(BAEA_REQUIRED == type)
, d_isHidden(  BAEA_HIDDEN   == type)
, d_defaultValue(basicAllocator)
{
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                               char             charValue,
                                               bslma_Allocator *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendChar(charValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                               short            shortValue,
                                               bslma_Allocator *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendShort(shortValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                               int              intValue,
                                               bslma_Allocator *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendInt(intValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                      bsls_PlatformUtil::Int64  int64Value,
                                      bslma_Allocator          *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendInt64(int64Value);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                               float            floatValue,
                                               bslma_Allocator *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendFloat(floatValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                               double           doubleValue,
                                               bslma_Allocator *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendDouble(doubleValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                            const bsl::string&  stringValue,
                                            bslma_Allocator    *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendString(stringValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                          const bdet_Datetime&  datetimeValue,
                                          bslma_Allocator      *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendDatetime(datetimeValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                              const bdet_Date&  dateValue,
                                              bslma_Allocator  *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendDate(dateValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                              const bdet_Time&  timeValue,
                                              bslma_Allocator  *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendTime(timeValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                      const bsl::vector<char>&  charArrayValue,
                                      bslma_Allocator          *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendCharArray(charArrayValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                    const bsl::vector<short>&  shortArrayValue,
                                    bslma_Allocator           *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendShortArray(shortArrayValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                       const bsl::vector<int>&  intArrayValue,
                                       bslma_Allocator         *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendIntArray(intArrayValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                 const bsl::vector<bsls_PlatformUtil::Int64>&  int64ArrayValue,
                 bslma_Allocator                              *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendInt64Array(int64ArrayValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                    const bsl::vector<float>&  floatArrayValue,
                                    bslma_Allocator           *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendFloatArray(floatArrayValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                  const bsl::vector<double>&  doubleArrayValue,
                                  bslma_Allocator            *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendDoubleArray(doubleArrayValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                             const bsl::vector<bsl::string>&  stringArrayValue,
                             bslma_Allocator                 *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendStringArray(stringArrayValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                         const bsl::vector<bdet_Datetime>&  datetimeArrayValue,
                         bslma_Allocator                   *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendDatetimeArray(datetimeArrayValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                 const bsl::vector<bdet_Date>&  dateArrayValue,
                                 bslma_Allocator               *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendDateArray(dateArrayValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                                 const bsl::vector<bdet_Time>&  timeArrayValue,
                                 bslma_Allocator               *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
    d_defaultValue.appendTimeArray(timeArrayValue);
}

baea_CommandLineOccurrenceInfo::baea_CommandLineOccurrenceInfo(
                         const baea_CommandLineOccurrenceInfo&  original,
                         bslma_Allocator                       *basicAllocator)
: d_isRequired(original.d_isRequired)
, d_isHidden(original.d_isHidden)
, d_defaultValue(original.d_defaultValue, basicAllocator)
{
}

baea_CommandLineOccurrenceInfo::~baea_CommandLineOccurrenceInfo()
{
}

// MANIPULATORS
baea_CommandLineOccurrenceInfo& baea_CommandLineOccurrenceInfo::operator=(
                                     const baea_CommandLineOccurrenceInfo& rhs)
{
    if (&rhs != this) {
        d_isRequired   = rhs.d_isRequired;
        d_isHidden     = rhs.d_isHidden;
        d_defaultValue = rhs.d_defaultValue;
    }
    return *this;
}

void baea_CommandLineOccurrenceInfo::setDefaultValue(
                                                const bdem_ConstElemRef& value)
{
    BSLS_ASSERT(!d_isRequired);
    BSLS_ASSERT(value.isBound());
    BSLS_ASSERT(bdem_ElemType::BDEM_CHAR           == value.type()
             || bdem_ElemType::BDEM_SHORT          == value.type()
             || bdem_ElemType::BDEM_INT            == value.type()
             || bdem_ElemType::BDEM_INT64          == value.type()
             || bdem_ElemType::BDEM_FLOAT          == value.type()
             || bdem_ElemType::BDEM_DOUBLE         == value.type()
             || bdem_ElemType::BDEM_STRING         == value.type()
             || bdem_ElemType::BDEM_DATETIME       == value.type()
             || bdem_ElemType::BDEM_DATE           == value.type()
             || bdem_ElemType::BDEM_TIME           == value.type()
             || bdem_ElemType::BDEM_CHAR_ARRAY     == value.type()
             || bdem_ElemType::BDEM_SHORT_ARRAY    == value.type()
             || bdem_ElemType::BDEM_INT_ARRAY      == value.type()
             || bdem_ElemType::BDEM_INT64_ARRAY    == value.type()
             || bdem_ElemType::BDEM_FLOAT_ARRAY    == value.type()
             || bdem_ElemType::BDEM_DOUBLE_ARRAY   == value.type()
             || bdem_ElemType::BDEM_STRING_ARRAY   == value.type()
             || bdem_ElemType::BDEM_DATETIME_ARRAY == value.type()
             || bdem_ElemType::BDEM_DATE_ARRAY     == value.type()
             || bdem_ElemType::BDEM_TIME_ARRAY     == value.type());

    if (0 < d_defaultValue.length()) {
        d_defaultValue.removeAll();
    }
    d_defaultValue.appendElement(value);
}

void baea_CommandLineOccurrenceInfo::setHidden()
{
    BSLS_ASSERT(!d_isRequired);

    d_isHidden = true;
}

// ACCESSORS
bdem_ConstElemRef baea_CommandLineOccurrenceInfo::defaultValue() const
{
    BSLS_ASSERT(0 < d_defaultValue.length());

    return d_defaultValue[0];
}

bool baea_CommandLineOccurrenceInfo::hasDefaultValue() const
{
    return 0 < d_defaultValue.length();
}

bool baea_CommandLineOccurrenceInfo::isHidden() const
{
    return d_isHidden;
}

bool baea_CommandLineOccurrenceInfo::isRequired() const
{
    return d_isRequired;
}

baea_CommandLineOccurrenceInfo::OccurrenceType
baea_CommandLineOccurrenceInfo::occurrenceType() const
{
    return d_isRequired ? BAEA_REQUIRED : (d_isHidden ? BAEA_HIDDEN
                                                      : BAEA_OPTIONAL);
}

bsl::ostream&
baea_CommandLineOccurrenceInfo::print(bsl::ostream& stream,
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
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    if (hasDefaultValue()) {
        stream << "{";
        bdeu_Print::newlineAndIndent(stream, ++level, spacesPerLevel);
    }
    const char *typeString;
    switch (occurrenceType()) {
      case baea_CommandLineOccurrenceInfo::BAEA_REQUIRED: {
        typeString = "REQUIRED";
      } break;
      case baea_CommandLineOccurrenceInfo::BAEA_HIDDEN: {
        typeString = "HIDDEN";
      } break;
      case baea_CommandLineOccurrenceInfo::BAEA_OPTIONAL: {
        typeString = "OPTIONAL";
      } break;
      default: {
        typeString = "***UNKNOWN***";
      } break;
    }
    stream << typeString;
    if (hasDefaultValue()) {
        bdeu_Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << "DEFAULT_TYPE  " << defaultValue().type();
        bdeu_Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << "DEFAULT_VALUE ";
        defaultValue().print(stream, -level, -spacesPerLevel);
        bdeu_Print::newlineAndIndent(stream, --level, spacesPerLevel);
        stream << "}";
    }

    return stream;
}

// FREE OPERATORS
bool operator==(const baea_CommandLineOccurrenceInfo& lhs,
                const baea_CommandLineOccurrenceInfo& rhs)
{
    return lhs.occurrenceType()  == rhs.occurrenceType()
        && lhs.hasDefaultValue() == rhs.hasDefaultValue()
        && (!lhs.hasDefaultValue() ||
                                     lhs.defaultValue() == rhs.defaultValue());
}

bool operator!=(const baea_CommandLineOccurrenceInfo& lhs,
                const baea_CommandLineOccurrenceInfo& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream&
operator<<(bsl::ostream& stream, const baea_CommandLineOccurrenceInfo& rhs)
{
    return rhs.print(stream);
}

                      // ---------------------------------
                      // struct baea_CommandLineOptionType
                      // ---------------------------------

typedef baea_CommandLineOptionType CLOT;

// CLASS DATA
bool                                  * const CLOT::BAEA_BOOL           = 0;
char                                  * const CLOT::BAEA_CHAR           = 0;
short                                 * const CLOT::BAEA_SHORT          = 0;
int                                   * const CLOT::BAEA_INT            = 0;
bsls_PlatformUtil::Int64              * const CLOT::BAEA_INT64          = 0;
float                                 * const CLOT::BAEA_FLOAT          = 0;
double                                * const CLOT::BAEA_DOUBLE         = 0;
bsl::string                           * const CLOT::BAEA_STRING         = 0;
bdet_Datetime                         * const CLOT::BAEA_DATETIME       = 0;
bdet_Date                             * const CLOT::BAEA_DATE           = 0;
bdet_Time                             * const CLOT::BAEA_TIME           = 0;
bsl::vector<char>                     * const CLOT::BAEA_CHAR_ARRAY     = 0;
bsl::vector<short>                    * const CLOT::BAEA_SHORT_ARRAY    = 0;
bsl::vector<int>                      * const CLOT::BAEA_INT_ARRAY      = 0;
bsl::vector<bsls_PlatformUtil::Int64> * const CLOT::BAEA_INT64_ARRAY    = 0;
bsl::vector<float>                    * const CLOT::BAEA_FLOAT_ARRAY    = 0;
bsl::vector<double>                   * const CLOT::BAEA_DOUBLE_ARRAY   = 0;
bsl::vector<bsl::string>              * const CLOT::BAEA_STRING_ARRAY   = 0;
bsl::vector<bdet_Datetime>            * const CLOT::BAEA_DATETIME_ARRAY = 0;
bsl::vector<bdet_Date>                * const CLOT::BAEA_DATE_ARRAY     = 0;
bsl::vector<bdet_Time>                * const CLOT::BAEA_TIME_ARRAY     = 0;

                       // ------------------------------
                       // class baea_CommandLineTypeInfo
                       // ------------------------------

// CREATORS
baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                               bslma_Allocator *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_STRING)
, d_linkedVariable_p(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                           bool            *variable,
                                           bslma_Allocator *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_BOOL)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                               char            *variable,
                                               bslma_Allocator *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_CHAR)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
             char                                              *variable,
             const baea_CommandLineConstraint::CharConstraint&  constraint,
             bslma_Allocator                                   *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_CHAR)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                               short           *variable,
                                               bslma_Allocator *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_SHORT)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
            short                                              *variable,
            const baea_CommandLineConstraint::ShortConstraint&  constraint,
            bslma_Allocator                                    *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_SHORT)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                               int             *variable,
                                               bslma_Allocator *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_INT)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
              int                                              *variable,
              const baea_CommandLineConstraint::IntConstraint&  constraint,
              bslma_Allocator                                  *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_INT)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                      bsls_PlatformUtil::Int64 *variable,
                                      bslma_Allocator          *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_INT64)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
            bsls_PlatformUtil::Int64                           *variable,
            const baea_CommandLineConstraint::Int64Constraint&  constraint,
            bslma_Allocator                                    *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_INT64)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                               float           *variable,
                                               bslma_Allocator *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_FLOAT)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
            float                                              *variable,
            const baea_CommandLineConstraint::FloatConstraint&  constraint,
            bslma_Allocator                                    *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_FLOAT)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                               double          *variable,
                                               bslma_Allocator *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_DOUBLE)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
           double                                              *variable,
           const baea_CommandLineConstraint::DoubleConstraint&  constraint,
           bslma_Allocator                                     *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_DOUBLE)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                               bsl::string     *variable,
                                               bslma_Allocator *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_STRING)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
           bsl::string                                         *variable,
           const baea_CommandLineConstraint::StringConstraint&  constraint,
           bslma_Allocator                                     *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_STRING)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                               bdet_Datetime   *variable,
                                               bslma_Allocator *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_DATETIME)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
         bdet_Datetime                                         *variable,
         const baea_CommandLineConstraint::DatetimeConstraint&  constraint,
         bslma_Allocator                                       *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_DATETIME)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                               bdet_Date       *variable,
                                               bslma_Allocator *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_DATE)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
             bdet_Date                                         *variable,
             const baea_CommandLineConstraint::DateConstraint&  constraint,
             bslma_Allocator                                   *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_DATE)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                               bdet_Time       *variable,
                                               bslma_Allocator *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_TIME)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
             bdet_Time                                         *variable,
             const baea_CommandLineConstraint::TimeConstraint&  constraint,
             bslma_Allocator                                   *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_TIME)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                             bsl::vector<char> *variable,
                                             bslma_Allocator   *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_CHAR_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
             bsl::vector<char>                                 *variable,
             const baea_CommandLineConstraint::CharConstraint&  constraint,
             bslma_Allocator                                   *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_CHAR_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                            bsl::vector<short> *variable,
                                            bslma_Allocator    *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_SHORT_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
            bsl::vector<short>                                 *variable,
            const baea_CommandLineConstraint::ShortConstraint&  constraint,
            bslma_Allocator                                    *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_SHORT_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                              bsl::vector<int> *variable,
                                              bslma_Allocator  *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_INT_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
              bsl::vector<int>                                 *variable,
              const baea_CommandLineConstraint::IntConstraint&  constraint,
              bslma_Allocator                                  *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_INT_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                         bsl::vector<bsls_PlatformUtil::Int64> *variable,
                         bslma_Allocator                       *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_INT64_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
            bsl::vector<bsls_PlatformUtil::Int64>              *variable,
            const baea_CommandLineConstraint::Int64Constraint&  constraint,
            bslma_Allocator                                    *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_INT64_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                            bsl::vector<float> *variable,
                                            bslma_Allocator    *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_FLOAT_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
            bsl::vector<float>                                 *variable,
            const baea_CommandLineConstraint::FloatConstraint&  constraint,
            bslma_Allocator                                    *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_FLOAT_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                           bsl::vector<double> *variable,
                                           bslma_Allocator     *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_DOUBLE_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
           bsl::vector<double>                                 *variable,
           const baea_CommandLineConstraint::DoubleConstraint&  constraint,
           bslma_Allocator                                     *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_DOUBLE_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                      bsl::vector<bsl::string> *variable,
                                      bslma_Allocator          *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_STRING_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
           bsl::vector<bsl::string>                            *variable,
           const baea_CommandLineConstraint::StringConstraint&  constraint,
           bslma_Allocator                                     *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_STRING_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                    bsl::vector<bdet_Datetime> *variable,
                                    bslma_Allocator            *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_DATETIME_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
         bsl::vector<bdet_Datetime>                            *variable,
         const baea_CommandLineConstraint::DatetimeConstraint&  constraint,
         bslma_Allocator                                       *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_DATETIME_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                        bsl::vector<bdet_Date> *variable,
                                        bslma_Allocator        *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_DATE_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
             bsl::vector<bdet_Date>                            *variable,
             const baea_CommandLineConstraint::DateConstraint&  constraint,
             bslma_Allocator                                   *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_DATE_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                                        bsl::vector<bdet_Time> *variable,
                                        bslma_Allocator        *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_TIME_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    resetConstraint();
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
             bsl::vector<bdet_Time>                            *variable,
             const baea_CommandLineConstraint::TimeConstraint&  constraint,
             bslma_Allocator                                   *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_TIME_ARRAY)
, d_linkedVariable_p(variable)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

baea_CommandLineTypeInfo::baea_CommandLineTypeInfo(
                               const baea_CommandLineTypeInfo&  original,
                               bslma_Allocator                 *basicAllocator)
: d_elemType(original.d_elemType)
, d_linkedVariable_p(original.d_linkedVariable_p)
, d_constraint_p(original.d_constraint_p)  // share constraint
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

baea_CommandLineTypeInfo::~baea_CommandLineTypeInfo()
{
}

// MANIPULATORS
baea_CommandLineTypeInfo&
baea_CommandLineTypeInfo::operator=(const baea_CommandLineTypeInfo& rhs)
{
    if (this != &rhs) {
        d_elemType         = rhs.d_elemType;
        d_linkedVariable_p = rhs.d_linkedVariable_p;
        d_constraint_p     = rhs.d_constraint_p;
    }

    return *this;
}

void baea_CommandLineTypeInfo::resetLinkedVariable()
{
    d_linkedVariable_p = 0;
    resetConstraint();
}

void baea_CommandLineTypeInfo::resetConstraint()
{
    // STATIC DATA
    static BoolConstraint           defaultBoolConstraint;
    static CharConstraint           defaultCharConstraint(
                              baea_CommandLineConstraint::CharConstraint(), 0);
    static ShortConstraint          defaultShortConstraint(
                             baea_CommandLineConstraint::ShortConstraint(), 0);
    static IntConstraint            defaultIntConstraint(
                               baea_CommandLineConstraint::IntConstraint(), 0);
    static Int64Constraint          defaultInt64Constraint(
                             baea_CommandLineConstraint::Int64Constraint(), 0);
    static FloatConstraint          defaultFloatConstraint(
                             baea_CommandLineConstraint::FloatConstraint(), 0);
    static DoubleConstraint         defaultDoubleConstraint(
                            baea_CommandLineConstraint::DoubleConstraint(), 0);
    static StringConstraint         defaultStringConstraint(
                            baea_CommandLineConstraint::StringConstraint(), 0);
    static DatetimeConstraint       defaultDatetimeConstraint(
                          baea_CommandLineConstraint::DatetimeConstraint(), 0);
    static DateConstraint           defaultDateConstraint(
                              baea_CommandLineConstraint::DateConstraint(), 0);
    static TimeConstraint           defaultTimeConstraint(
                              baea_CommandLineConstraint::TimeConstraint(), 0);
    static CharArrayConstraint      defaultCharArrayConstraint(
                              baea_CommandLineConstraint::CharConstraint(), 0);
    static ShortArrayConstraint     defaultShortArrayConstraint(
                             baea_CommandLineConstraint::ShortConstraint(), 0);
    static IntArrayConstraint       defaultIntArrayConstraint(
                               baea_CommandLineConstraint::IntConstraint(), 0);
    static Int64ArrayConstraint     defaultInt64ArrayConstraint(
                             baea_CommandLineConstraint::Int64Constraint(), 0);
    static FloatArrayConstraint     defaultFloatArrayConstraint(
                             baea_CommandLineConstraint::FloatConstraint(), 0);
    static DoubleArrayConstraint    defaultDoubleArrayConstraint(
                            baea_CommandLineConstraint::DoubleConstraint(), 0);
    static StringArrayConstraint    defaultStringArrayConstraint(
                            baea_CommandLineConstraint::StringConstraint(), 0);
    static DatetimeArrayConstraint  defaultDatetimeArrayConstraint(
                          baea_CommandLineConstraint::DatetimeConstraint(), 0);
    static DateArrayConstraint      defaultDateArrayConstraint(
                              baea_CommandLineConstraint::DateConstraint(), 0);
    static TimeArrayConstraint      defaultTimeArrayConstraint(
                              baea_CommandLineConstraint::TimeConstraint(), 0);
        // These static variables, default-initialized, are shared among all
        // type infos that do not have constraints.  Note that these objects
        // are initialized with a default allocator and should never be
        // deleted.

    switch (d_elemType) {
      case bdem_ElemType::BDEM_BOOL: {
        d_constraint_p.reset(&defaultBoolConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_CHAR: {
        d_constraint_p.reset(&defaultCharConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_SHORT: {
        d_constraint_p.reset(&defaultShortConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_INT: {
        d_constraint_p.reset(&defaultIntConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_INT64: {
        d_constraint_p.reset(&defaultInt64Constraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_FLOAT: {
        d_constraint_p.reset(&defaultFloatConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_DOUBLE: {
        d_constraint_p.reset(&defaultDoubleConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_STRING: {
        d_constraint_p.reset(&defaultStringConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_DATETIME: {
        d_constraint_p.reset(&defaultDatetimeConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_DATE: {
        d_constraint_p.reset(&defaultDateConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_TIME: {
        d_constraint_p.reset(&defaultTimeConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_CHAR_ARRAY: {
        d_constraint_p.reset(&defaultCharArrayConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_SHORT_ARRAY: {
        d_constraint_p.reset(&defaultShortArrayConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_INT_ARRAY: {
        d_constraint_p.reset(&defaultIntArrayConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_INT64_ARRAY: {
        d_constraint_p.reset(&defaultInt64ArrayConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_FLOAT_ARRAY: {
        d_constraint_p.reset(&defaultFloatArrayConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_DOUBLE_ARRAY: {
        d_constraint_p.reset(&defaultDoubleArrayConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_STRING_ARRAY: {
        d_constraint_p.reset(&defaultStringArrayConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_DATETIME_ARRAY: {
        d_constraint_p.reset(&defaultDatetimeArrayConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_DATE_ARRAY: {
        d_constraint_p.reset(&defaultDateArrayConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      case bdem_ElemType::BDEM_TIME_ARRAY: {
        d_constraint_p.reset(&defaultTimeArrayConstraint,
                             bcema_SharedPtrNilDeleter());
      } break;
      default: {
        BSLS_ASSERT(0);
      } break;
    }
}

void baea_CommandLineTypeInfo::setConstraint(
                  const baea_CommandLineConstraint::CharConstraint& constraint)
{
    if (bdem_ElemType::BDEM_CHAR == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p) CharConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (bdem_ElemType::BDEM_CHAR_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                            CharArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void baea_CommandLineTypeInfo::setConstraint(
                 const baea_CommandLineConstraint::ShortConstraint& constraint)
{
    if (bdem_ElemType::BDEM_SHORT == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                                ShortConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (bdem_ElemType::BDEM_SHORT_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                           ShortArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void baea_CommandLineTypeInfo::setConstraint(
                   const baea_CommandLineConstraint::IntConstraint& constraint)
{
    if (bdem_ElemType::BDEM_INT == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p) IntConstraint(constraint,
                                                               d_allocator_p),
                             d_allocator_p);
    } else if (bdem_ElemType::BDEM_INT_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                             IntArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void baea_CommandLineTypeInfo::setConstraint(
                 const baea_CommandLineConstraint::Int64Constraint& constraint)
{
    if (bdem_ElemType::BDEM_INT64 == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                                Int64Constraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (bdem_ElemType::BDEM_INT64_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                           Int64ArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void baea_CommandLineTypeInfo::setConstraint(
                 const baea_CommandLineConstraint::FloatConstraint& constraint)
{
    if (bdem_ElemType::BDEM_FLOAT == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                                FloatConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (bdem_ElemType::BDEM_FLOAT_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                           FloatArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void baea_CommandLineTypeInfo::setConstraint(
                const baea_CommandLineConstraint::DoubleConstraint& constraint)
{
    if (bdem_ElemType::BDEM_DOUBLE == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                               DoubleConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (bdem_ElemType::BDEM_DOUBLE_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                          DoubleArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void baea_CommandLineTypeInfo::setConstraint(
                const baea_CommandLineConstraint::StringConstraint& constraint)
{
    if (bdem_ElemType::BDEM_STRING == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                               StringConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (bdem_ElemType::BDEM_STRING_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                          StringArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void baea_CommandLineTypeInfo::setConstraint(
              const baea_CommandLineConstraint::DatetimeConstraint& constraint)
{
    if (bdem_ElemType::BDEM_DATETIME == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                             DatetimeConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (bdem_ElemType::BDEM_DATETIME_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                        DatetimeArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void baea_CommandLineTypeInfo::setConstraint(
                  const baea_CommandLineConstraint::DateConstraint& constraint)
{
    if (bdem_ElemType::BDEM_DATE == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p) DateConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (bdem_ElemType::BDEM_DATE_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                            DateArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void baea_CommandLineTypeInfo::setConstraint(
                  const baea_CommandLineConstraint::TimeConstraint& constraint)
{
    if (bdem_ElemType::BDEM_TIME == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                                TimeConstraint(constraint,
                                                               d_allocator_p),
                             d_allocator_p);
    } else if (bdem_ElemType::BDEM_TIME_ARRAY == d_elemType) {
        d_constraint_p.reset(new(*d_allocator_p)
                                            TimeArrayConstraint(constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void baea_CommandLineTypeInfo::setConstraint(
                const bcema_SharedPtr<baea_CommandLine_Constraint>& constraint)
{
    d_constraint_p = constraint;
}

void baea_CommandLineTypeInfo::setLinkedVariable(bool *variable)
{
    d_elemType         = bdem_ElemType::BDEM_BOOL;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(char *variable)
{
    d_elemType         = bdem_ElemType::BDEM_CHAR;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(short *variable)
{
    d_elemType         = bdem_ElemType::BDEM_SHORT;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(int *variable)
{
    d_elemType         = bdem_ElemType::BDEM_INT;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(
                                            bsls_PlatformUtil::Int64 *variable)
{
    d_elemType         = bdem_ElemType::BDEM_INT64;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(float *variable)
{
    d_elemType         = bdem_ElemType::BDEM_FLOAT;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(double *variable)
{
    d_elemType         = bdem_ElemType::BDEM_DOUBLE;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(bsl::string *variable)
{
    d_elemType         = bdem_ElemType::BDEM_STRING;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(bdet_Datetime *variable)
{
    d_elemType         = bdem_ElemType::BDEM_DATETIME;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(bdet_Date *variable)
{
    d_elemType         = bdem_ElemType::BDEM_DATE;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(bdet_Time *variable)
{
    d_elemType         = bdem_ElemType::BDEM_TIME;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(bsl::vector<char> *variable)
{
    d_elemType         = bdem_ElemType::BDEM_CHAR_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(bsl::vector<short> *variable)
{
    d_elemType         = bdem_ElemType::BDEM_SHORT_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(bsl::vector<int> *variable)
{
    d_elemType         = bdem_ElemType::BDEM_INT_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(
                               bsl::vector<bsls_PlatformUtil::Int64> *variable)
{
    d_elemType         = bdem_ElemType::BDEM_INT64_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(bsl::vector<float> *variable)
{
    d_elemType         = bdem_ElemType::BDEM_FLOAT_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(bsl::vector<double> *variable)
{
    d_elemType         = bdem_ElemType::BDEM_DOUBLE_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(
                                            bsl::vector<bsl::string> *variable)
{
    d_elemType         = bdem_ElemType::BDEM_STRING_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(
                                          bsl::vector<bdet_Datetime> *variable)
{
    d_elemType         = bdem_ElemType::BDEM_DATETIME_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(
                                              bsl::vector<bdet_Date> *variable)
{
    d_elemType         = bdem_ElemType::BDEM_DATE_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

void baea_CommandLineTypeInfo::setLinkedVariable(
                                              bsl::vector<bdet_Time> *variable)
{
    d_elemType         = bdem_ElemType::BDEM_TIME_ARRAY;
    d_linkedVariable_p = variable;
    resetConstraint();
}

// ACCESSORS
bcema_SharedPtr<baea_CommandLine_Constraint>
baea_CommandLineTypeInfo::constraint() const
{
    return d_constraint_p;
}

bool baea_CommandLineTypeInfo::checkConstraint(
                                        const bdem_ConstElemRef& element) const
{
    return d_constraint_p->validate(element);
}

bool baea_CommandLineTypeInfo::checkConstraint(
                                         const bdem_ConstElemRef& element,
                                         bsl::ostream&            stream) const
{
    return d_constraint_p->validate(element, stream);
}

bool baea_CommandLineTypeInfo::checkConstraint(const void *variable) const
{
    const bdem_Descriptor *desc =
                                bdem_ElemAttrLookup::lookupTable()[d_elemType];
    return d_constraint_p->validate(bdem_ConstElemRef(variable, desc));
}

bool baea_CommandLineTypeInfo::checkConstraint(const void    *variable,
                                               bsl::ostream&  stream) const
{
    const bdem_Descriptor *desc =
                                bdem_ElemAttrLookup::lookupTable()[d_elemType];
    return d_constraint_p->validate(bdem_ConstElemRef(variable, desc), stream);
}

bdem_ElemType::Type baea_CommandLineTypeInfo::type() const
{
    return d_elemType;
}

void *baea_CommandLineTypeInfo::linkedVariable() const
{
    return d_linkedVariable_p;
}

bsl::ostream&
baea_CommandLineTypeInfo::print(bsl::ostream& stream,
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
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    stream << "{";
    bdeu_Print::newlineAndIndent(stream, ++level, spacesPerLevel);
    stream << "TYPE       " << type();
    if (linkedVariable()) {
        bdeu_Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << "VARIABLE   " << (void *)linkedVariable();
    }
    if (constraint()) {
        bdeu_Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << "CONSTRAINT " << (void *)constraint().ptr();
    }
    bdeu_Print::newlineAndIndent(stream, --level, spacesPerLevel);
    stream << "}";

    return stream;
}

// FREE OPERATORS
bool operator==(const baea_CommandLineTypeInfo& lhs,
                const baea_CommandLineTypeInfo& rhs)
{
    return lhs.type()           == rhs.type()
        && lhs.linkedVariable() == rhs.linkedVariable()
        && lhs.constraint()     == rhs.constraint();
}

bool operator!=(const baea_CommandLineTypeInfo& lhs,
                const baea_CommandLineTypeInfo& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream&
operator<<(bsl::ostream& stream, const baea_CommandLineTypeInfo& rhs)
{
    return rhs.print(stream);
}

                     // ---------------------------------
                     // struct baea_CommandLineOptionInfo
                     // ---------------------------------

// FREE OPERATORS
bool operator==(const baea_CommandLineOptionInfo& lhs,
                const baea_CommandLineOptionInfo& rhs)
{
    return lhs.d_tag         == rhs.d_tag
        && lhs.d_name        == rhs.d_name
        && lhs.d_description == rhs.d_description
        && lhs.d_typeInfo    == rhs.d_typeInfo
        && lhs.d_defaultInfo == rhs.d_defaultInfo;
}

bool operator!=(const baea_CommandLineOptionInfo& lhs,
                const baea_CommandLineOptionInfo& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream&
operator<<(bsl::ostream& stream, const baea_CommandLineOptionInfo& rhs)
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

                          // ----------------------------
                          // class baea_CommandLineOption
                          // ----------------------------

// PRIVATE MANIPULATORS
void baea_CommandLineOption::init(const baea_CommandLineOptionInfo& optionInfo)
{
    bslalg_ScalarPrimitives::copyConstruct(&d_optionInfo.object().d_tag,
                                           optionInfo.d_tag,
                                           d_allocator_p);
    bslalg_AutoScalarDestructor<bsl::string> tagGuard(
                                                 &d_optionInfo.object().d_tag);

    bslalg_ScalarPrimitives::copyConstruct(&d_optionInfo.object().d_name,
                                           optionInfo.d_name,
                                           d_allocator_p);
    bslalg_AutoScalarDestructor<bsl::string> nameGuard(
                                                &d_optionInfo.object().d_name);

    bslalg_ScalarPrimitives::copyConstruct(
                                          &d_optionInfo.object().d_description,
                                          optionInfo.d_description,
                                          d_allocator_p);
    bslalg_AutoScalarDestructor<bsl::string> descGuard(
                                         &d_optionInfo.object().d_description);

    bslalg_ScalarPrimitives::copyConstruct(&d_optionInfo.object().d_typeInfo,
                                           optionInfo.d_typeInfo,
                                           d_allocator_p);
    bslalg_AutoScalarDestructor<baea_CommandLineTypeInfo> typeGuard(
                                            &d_optionInfo.object().d_typeInfo);

    bslalg_ScalarPrimitives::copyConstruct(
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
baea_CommandLineOption::baea_CommandLineOption(bslma_Allocator *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    bslalg_ScalarPrimitives::defaultConstruct(
                                 &d_optionInfo.object().d_tag, d_allocator_p);
    bslalg_AutoScalarDestructor<bsl::string> tagGuard(
                                                 &d_optionInfo.object().d_tag);

    bslalg_ScalarPrimitives::defaultConstruct(
                                &d_optionInfo.object().d_name, d_allocator_p);
    bslalg_AutoScalarDestructor<bsl::string> nameGuard(
                                                &d_optionInfo.object().d_name);

    bslalg_ScalarPrimitives::defaultConstruct(
                         &d_optionInfo.object().d_description, d_allocator_p);
    bslalg_AutoScalarDestructor<bsl::string> descGuard(
                                         &d_optionInfo.object().d_description);

    bslalg_ScalarPrimitives::defaultConstruct(
                            &d_optionInfo.object().d_typeInfo, d_allocator_p);
    bslalg_AutoScalarDestructor<baea_CommandLineTypeInfo> typeGuard(
                                            &d_optionInfo.object().d_typeInfo);

    bslalg_ScalarPrimitives::defaultConstruct(
                         &d_optionInfo.object().d_defaultInfo, d_allocator_p);
    typeGuard.release();
    descGuard.release();
    nameGuard.release();
    tagGuard.release();
}

baea_CommandLineOption::baea_CommandLineOption(
                                 const baea_CommandLineOption&  original,
                                 bslma_Allocator               *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    init(static_cast<const baea_CommandLineOptionInfo&>(original));
}

baea_CommandLineOption::baea_CommandLineOption(
                             const baea_CommandLineOptionInfo&  optionInfo,
                             bslma_Allocator                   *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    init(optionInfo);
}

baea_CommandLineOption::~baea_CommandLineOption()
{
    bslalg_ScalarDestructionPrimitives::destroy(&d_optionInfo.object());
}

// MANIPULATORS
baea_CommandLineOption&
baea_CommandLineOption::operator=(const baea_CommandLineOption& rhs)
{
    const baea_CommandLineOptionInfo& optionInfo = rhs;
    return *this = optionInfo;
}

baea_CommandLineOption&
baea_CommandLineOption::operator=(const baea_CommandLineOptionInfo& rhs)
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

// ACCESSORS
baea_CommandLineOption::operator const baea_CommandLineOptionInfo&() const
{
    return d_optionInfo.object();
}

baea_CommandLineOptionInfo::ArgType baea_CommandLineOption::argType() const
{
    if (d_optionInfo.object().d_tag.empty()) {
        return baea_CommandLineOptionInfo::BAEA_NON_OPTION;
    }

    if (bdem_ElemType::BDEM_BOOL ==
                       d_optionInfo.object().d_typeInfo.constraint()->type()) {
        return baea_CommandLineOptionInfo::BAEA_FLAG;
    }

    return baea_CommandLineOptionInfo::BAEA_OPTION;
}

const bsl::string& baea_CommandLineOption::description() const
{
    return d_optionInfo.object().d_description;
}

bool baea_CommandLineOption::isArray() const
{
    return bdem_ElemType::isArrayType(d_optionInfo.object().d_typeInfo.type());
}

bool baea_CommandLineOption::isLongTagValid(const char    *longTag,
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

bool baea_CommandLineOption::isTagValid(bsl::ostream& stream) const
{
    bool result = true;   // attempt to give all possible error messages
    const char *str = d_optionInfo.object().d_tag.c_str();

    if (0 == *str) {  // non-option
        if (bdem_ElemType::BDEM_BOOL ==
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

bool baea_CommandLineOption::isDescriptionValid(bsl::ostream& stream) const
{
    if (d_optionInfo.object().d_description.empty()) {
        stream << "Description cannot be an empty string." << bsl::endl;
        return false;
    }
    return true;
}

bool baea_CommandLineOption::isNameValid(bsl::ostream& stream) const
{
    if (d_optionInfo.object().d_name.empty()) {
        stream << "Name cannot be an empty string." << bsl::endl;
        return false;
    }
    return true;
}

const char *baea_CommandLineOption::longTag() const
{
    BSLS_ASSERT(argType() != baea_CommandLineOptionInfo::BAEA_NON_OPTION);

    const char *tagString = d_optionInfo.object().d_tag.c_str();
    return '|' == tagString[1] ? tagString + 2 : tagString;
}

const bsl::string& baea_CommandLineOption::name() const
{
    return d_optionInfo.object().d_name;
}

const baea_CommandLineOccurrenceInfo&
baea_CommandLineOption::occurrenceInfo() const
{
    return d_optionInfo.object().d_defaultInfo;
}

char baea_CommandLineOption::shortTag() const
{
    BSLS_ASSERT(argType() != baea_CommandLineOptionInfo::BAEA_NON_OPTION);

    const char *tagString = d_optionInfo.object().d_tag.c_str();
    return '|' == tagString[1] ? *tagString : 0;
}

const bsl::string& baea_CommandLineOption::tagString() const
{
    return d_optionInfo.object().d_tag;
}

const baea_CommandLineTypeInfo&
baea_CommandLineOption::typeInfo() const
{
    return d_optionInfo.object().d_typeInfo;
}

bsl::ostream&
baea_CommandLineOption::print(bsl::ostream& stream,
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
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    stream << "{";
    bdeu_Print::newlineAndIndent(stream, ++level, spacesPerLevel);
    if (tagString().empty()) {
        stream << "NON_OPTION";
    } else {
        stream << "TAG            \"" << tagString() << "\"";
    }
    bdeu_Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "NAME           \"" << name() << "\"";
    bdeu_Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "DESCRIPTION    \"" << description() << "\"";
    bdeu_Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "TYPE_INFO      ";
    typeInfo().print(stream, -2, spacesPerLevel);
    bdeu_Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "OCCURRENCE_INFO ";
    occurrenceInfo().print(stream, -2, spacesPerLevel);
    bdeu_Print::newlineAndIndent(stream, --level, spacesPerLevel);
    stream << "}";

    return stream;
}

// FREE OPERATORS
bool operator==(const baea_CommandLineOption& lhs,
                const baea_CommandLineOption& rhs)
{
    return static_cast<const baea_CommandLineOptionInfo&>(lhs) ==
                           static_cast<const baea_CommandLineOptionInfo&>(rhs);
}

bool operator!=(const baea_CommandLineOption& lhs,
                const baea_CommandLineOption& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream&
operator<<(bsl::ostream& stream, const baea_CommandLineOption& rhs)
{
    return rhs.print(stream);
}

                          // ----------------------
                          // class baea_CommandLine
                          // ----------------------

// PRIVATE MANIPULATORS
void baea_CommandLine::clear()
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

void baea_CommandLine::initArgs(int argc, const char * const argv[])
{
    for (int i = 0; i < argc; ++i) {
        d_arguments.push_back(argv[i]);
    }
}

int baea_CommandLine::parse(bsl::ostream& stream)
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

            const baea_CommandLineOption& thisOption = d_options[index];
            const char *str;  // string for parsing value
            if ('=' == *s) {
                str = (baea_CommandLineOptionInfo::BAEA_FLAG ==
                                            thisOption.argType() ? "" : s + 1);
            } else {
                if (thisOption.argType() !=
                                       baea_CommandLineOptionInfo::BAEA_FLAG) {
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
             && (baea_CommandLineOptionInfo::BAEA_FLAG != thisOption.argType())
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
           && baea_CommandLineOptionInfo::BAEA_FLAG ==
                                                  d_options[index].argType()) {

            const baea_CommandLineOption& thisOption = d_options[index];

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
            const baea_CommandLineOption& thisOption = d_options[index];

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

int baea_CommandLine::postParse(bsl::ostream& stream)
{
    int index = missing();
    if (index >= 0) {
        const baea_CommandLineOption& thisOption = d_options[index];

        if (baea_CommandLineOptionInfo::BAEA_NON_OPTION ==
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

    bdem_RecordDef *record = d_schema.createRecord(recordName);

    for (volatile unsigned int i = 0; i < d_options.size(); ++i) {
        const baea_CommandLineOption& thisOption = d_options[i];
        const baea_CommandLineTypeInfo& info = thisOption.typeInfo();
        const baea_CommandLineOccurrenceInfo& defaultInfo =
                                                   thisOption.occurrenceInfo();

        record->appendField(info.type(), thisOption.name().c_str());

        if (d_data[i].isNonNull()) {
     // if (positions[i].size() != 0
     //  || baea_CommandLineOptionInfo::BAEA_FLAG == thisOption.argType()) {
            // Use p (i.e., entered value or bool false values) to load linked
            // var and binding.

            if (info.linkedVariable()) {
                const bdem_Descriptor *desc =
                               bdem_ElemAttrLookup::lookupTable()[info.type()];
                bdem_ElemRef(info.linkedVariable(), desc).replaceValue(
                                                                    d_data[i]);
            }

            d_data1.appendElement(d_data[i]);
        }
        else if (defaultInfo.hasDefaultValue()) {
            // Use default value to load linked variable and binding.
            // TBD  This might be redundant, since 'data[i]' was set in
            // 'validateAndInitialize'.  But to be safe...

            if (info.linkedVariable()) {
                const bdem_Descriptor *desc =
                               bdem_ElemAttrLookup::lookupTable()[info.type()];
                bdem_ElemRef(info.linkedVariable(), desc).replaceValue(
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

void baea_CommandLine::validateAndInitialize()
{
    baea_CommandLine::validateAndInitialize(bsl::cerr);
}

void baea_CommandLine::validateAndInitialize(bsl::ostream& stream)
{
    bool isValid = true;

    for (volatile unsigned int i = 0; i < d_options.size(); ++i) {
        const baea_CommandLineOption& thisOption = d_options[i];

        // Start with basic validity checks.  Try to do as many checks at a
        // time, in order to maximize the feedback to the user.

        if (!thisOption.isTagValid(stream)) {
            stream << "The error occurred while validating the "
                   << baea_Ordinal(i) << " option." << bsl::endl;

            isValid = false;
        }

        if (!thisOption.isNameValid(stream)) {
            stream << "The error occurred while validating the "
                   << baea_Ordinal(i) << " option." << bsl::endl;

            isValid = false;
        }

        if (!thisOption.isDescriptionValid(stream)) {
            stream << "The error occurred while validating the "
                   << baea_Ordinal(i) << " option." << bsl::endl;

            isValid = false;
        }

        if (baea_CommandLineOptionInfo::BAEA_FLAG == thisOption.argType()
         && thisOption.occurrenceInfo().hasDefaultValue())
        {
            stream << "No default value is allowed for the flag." << bsl::endl
                   << "The error occurred while validating the "
                   << baea_Ordinal(i) << " option." << bsl::endl;

            isValid = false;
        }

        if (thisOption.occurrenceInfo().hasDefaultValue()
         && thisOption.occurrenceInfo().defaultValue().type() !=
                                              thisOption.typeInfo().type()) {
            stream << "The type of default value does not "
                      "match the type specified for the option."
                   << bsl::endl;
            stream << "The error occurred while validating the "
                   << baea_Ordinal(i) << " option." << bsl::endl;

            isValid = false;
        }

        // Tags and names must be unique.

        for (volatile unsigned int j = 0; j < i; ++j) {
            if (thisOption.name() == d_options[j].name()) {
                stream << "Error: The names for the "
                       << baea_Ordinal(i) << " and "
                       << baea_Ordinal(j) << " options are equal."
                       << bsl::endl;

                isValid = false;
            }
        }

        if (thisOption.argType() !=
                                 baea_CommandLineOptionInfo::BAEA_NON_OPTION) {
            for (volatile unsigned int j = 0; j < i; ++j) {
                if (d_options[j].argType() ==
                                 baea_CommandLineOptionInfo::BAEA_NON_OPTION) {
                    continue;
                }

                if (thisOption.shortTag()
                 && thisOption.shortTag() == d_options[j].shortTag()) {
                    stream << "Error: short tags for the "
                           << baea_Ordinal(i) << " and "
                           << baea_Ordinal(j) << " options are equal."
                           << bsl::endl;

                    isValid = false;
                }

                if (!bsl::strcmp(thisOption.longTag(),
                                 d_options[j].longTag())) {
                    stream << "Error: long tags for the "
                           << baea_Ordinal(i) << " and "
                           << baea_Ordinal(j) << " options are equal."
                           << bsl::endl;

                    isValid = false;
                }
            }
        }

        // Additional checks for non-options.

        if (baea_CommandLineOptionInfo::BAEA_NON_OPTION ==
                                                        thisOption.argType()) {
            // Non-options cannot be hidden.

            if (thisOption.occurrenceInfo().isHidden()) {
                stream << "Error: A non-option cannot be hidden." << bsl::endl
                       << "The error occurred while validating the "
                       << baea_Ordinal(i) << " option." << bsl::endl;

                isValid = false;
            }
        }
    }

    if (!isValid) {
        EXIT_IF_FALSE(INVALID_COMMAND_LINE_SPEC);
    }

    for (volatile unsigned int i = 0; i < d_options.size(); ++i) {
        const baea_CommandLineOption& thisOption = d_options[i];

        // Initialize 'd_positions' and 'd_data' for this option.

        bsl::vector<int> v;
        d_positions.push_back(v);
        d_data.appendNullElement(thisOption.typeInfo().type());

        // Validate and initialize with the default values.

        if (thisOption.occurrenceInfo().hasDefaultValue()) {
            if (!thisOption.typeInfo().checkConstraint(
                       thisOption.occurrenceInfo().defaultValue(), stream)) {
                stream << "The error occurred while validating the "
                       << baea_Ordinal(i) << " option." << bsl::endl;

                EXIT_IF_FALSE(INVALID_COMMAND_LINE_SPEC);
            }
        }

        // Additional checks for non-options.

        if (baea_CommandLineOptionInfo::BAEA_NON_OPTION ==
                                                        thisOption.argType()) {
            // Only last non-option can be multi-valued.

            if (!d_nonOptionIndices.empty()
             && d_options[d_nonOptionIndices.back()].isArray()) {
                stream << "Error: A multi-valued non-option was already "
                          "specified as the "
                       << baea_Ordinal(d_nonOptionIndices.back())
                       << " option." << bsl::endl;
                stream << "The error occurred while validating the "
                       << baea_Ordinal(i) << " option." << bsl::endl;

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
                       << baea_Ordinal(d_nonOptionIndices.back())
                       << " option, but not for this non-option." << bsl::endl;
                stream << "The error occurred while validating the "
                       << baea_Ordinal(i) << " option." << bsl::endl;

                EXIT_IF_FALSE(INVALID_COMMAND_LINE_SPEC);
            }

            // Set 'd_nonOptionIndices' for this option.

            d_nonOptionIndices.push_back(i);
        }
    }
}

// PRIVATE ACCESSORS
int baea_CommandLine::findTag(const char *longTag, int tagLength) const
{
    for (unsigned int i = 0; i < d_options.size(); ++i) {
        if (d_options[i].argType() !=
                                    baea_CommandLineOptionInfo::BAEA_NON_OPTION
         && !bsl::strncmp(d_options[i].longTag(), longTag, tagLength)
         && bsl::strlen(d_options[i].longTag()) == (unsigned int)tagLength) {
            return i;
        }
    }
    return -1;
}

int baea_CommandLine::findTag(char shortTag) const
{
    for (unsigned int i = 0; i < d_options.size(); ++i) {
        if (d_options[i].argType() !=
                                    baea_CommandLineOptionInfo::BAEA_NON_OPTION
         && d_options[i].shortTag() == shortTag) {
            return i;
        }
    }
    return -1;
}

int baea_CommandLine::findName(const bsl::string& name) const
{
    for (unsigned int i = 0; i < d_options.size(); ++i) {
        if (d_options[i].name() == name) {
            return i;
        }
    }
    return -1;
}

void baea_CommandLine::location(bsl::ostream& stream,
                                int           argc,
                                int           start,
                                int           end) const
{
    if (end != -1) {
        stream << "The error occurred while parsing the "
               << baea_Ordinal(start) << " to "
               << baea_Ordinal(end) << " characters of the "
               << baea_Ordinal(argc - 1) << " argument." << bsl::endl;
    }
    else if (start != -1) {
        stream << "The error occurred while parsing the "
               << baea_Ordinal(start) << " character of the "
               << baea_Ordinal(argc - 1) << " argument." << bsl::endl;
    }
    else {
        stream << "The error occurred while parsing the "
               << baea_Ordinal(argc - 1) << " argument." << bsl::endl;
    }
}

int baea_CommandLine::longestTagSize() const
{
    int max = 0;
    int len;
    for (unsigned int i = 0; i < d_options.size(); ++i) {
        if (d_options[i].argType() !=
                                    baea_CommandLineOptionInfo::BAEA_NON_OPTION
         && (len = bsl::strlen(d_options[i].longTag())) > max) {
          max = len;
        }
    }
    return max;
}

int baea_CommandLine::longestNameSize() const
{
    int max = 0;
    for (unsigned int i = 0; i < d_options.size(); ++i) {
        if ((int) d_options[i].name().size() > max) {
            max = d_options[i].name().size();
        }
    }
    return max;
}

int baea_CommandLine::missing(bool checkAlsoNonOptions) const
{
    for (unsigned int i = 0; i < d_options.size(); ++i) {
        baea_CommandLineOptionInfo::ArgType argType = d_options[i].argType();
        if (0 == d_positions[i].size()  // no value specified
         && baea_CommandLineOptionInfo::BAEA_FLAG != argType
         && d_options[i].occurrenceInfo().isRequired()
         && (baea_CommandLineOptionInfo::BAEA_OPTION == argType ||
                                                        checkAlsoNonOptions)) {
            return i;
        }
    }
    return -1;
}

// CREATORS
baea_CommandLine::baea_CommandLine(
                              const baea_CommandLineOptionInfo *specTable,
                              int                               size,
                              bslma_Allocator                  *basicAllocator)
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
    d_options.reserve(size);
    for (int i = 0; i < size; ++i) {
        baea_CommandLineOption tmp(specTable[i]);
        d_options.push_back(tmp);
    }
    validateAndInitialize(bsl::cerr);
    d_state = BAEA_NOT_PARSED;
}

baea_CommandLine::baea_CommandLine(
                              const baea_CommandLineOptionInfo *specTable,
                              int                               size,
                              bsl::ostream&                    stream,
                              bslma_Allocator                  *basicAllocator)
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
    d_options.reserve(size);
    for (int i = 0; i < size; ++i) {
        baea_CommandLineOption tmp(specTable[i]);
        d_options.push_back(tmp);
    }
    validateAndInitialize(stream);
    d_state = BAEA_NOT_PARSED;
}

baea_CommandLine::baea_CommandLine(const baea_CommandLine&  original,
                                   bslma_Allocator         *basicAllocator)
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

baea_CommandLine::~baea_CommandLine()
{
}

// MANIPULATORS
baea_CommandLine& baea_CommandLine::operator=(const baea_CommandLine& rhs)
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

int baea_CommandLine::parse(int argc, const char * const argv[])
{
    return parse(argc, argv, bsl::cerr);
}

int baea_CommandLine::parse(int argc, const char * const argv[],
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
bdem_ConstRowBinding baea_CommandLine::data() const
{
    BSLS_ASSERT(BAEA_PARSED == d_state);

    return bdem_ConstRowBinding(&d_data1, &d_schema, recordName);
}

bool baea_CommandLine::isInvalid() const
{
    return BAEA_INVALID == d_state;
}

bool baea_CommandLine::isParsed() const
{
    return BAEA_PARSED == d_state;
}

bool baea_CommandLine::isSpecified(const bsl::string& name) const
{
    return numSpecified(name) > 0;
}

bool baea_CommandLine::isSpecified(const bsl::string& name, int *count) const
{
    const int n = numSpecified(name);
    if (n > 0 && count) {
        *count = n;
    }
    return n > 0;
}

int baea_CommandLine::numSpecified(const bsl::string& name) const
{
    const int index = findName(name);
    return 0 <= index ? d_positions[index].size() : 0;
}

const bsl::vector<int>&
baea_CommandLine::positions(const bsl::string& name) const
{
    const int index = findName(name);

    BSLS_ASSERT(index >= 0);

    return d_positions[index];
}

int baea_CommandLine::position(const bsl::string& name) const
{
    // The behavior is undefined unless 'name' exists and it is not an array.
    // -1 is returned if option is never entered on command line (possible in
    // case of flags, or options with default values).

    const int index = findName(name);

    BSLS_ASSERT(index >= 0);
    BSLS_ASSERT(!d_options[index].isArray());

    return 0 == d_positions[index].size() ? -1 : d_positions[index][0];
}

bdem_ConstRowBinding baea_CommandLine::specifiedData() const
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

bool baea_CommandLine::theBool(const bsl::string& name) const
{
    const int index = findName(name);

    BSLS_ASSERT(index >= 0);
    BSLS_ASSERT(
          baea_CommandLineOptionInfo::BAEA_FLAG == d_options[index].argType());

    return data().theBool(name.c_str());
}

char baea_CommandLine::theChar(const bsl::string& name) const
{
    return data().theChar(name.c_str());
}

short baea_CommandLine::theShort(const bsl::string& name) const
{
    return data().theShort(name.c_str());
}

int baea_CommandLine::theInt(const bsl::string& name) const
{
    return data().theInt(name.c_str());
}

bsls_PlatformUtil::Int64
baea_CommandLine::theInt64(const bsl::string& name) const
{
    return data().theInt64(name.c_str());
}

float baea_CommandLine::theFloat(const bsl::string& name) const
{
    return data().theFloat(name.c_str());
}

double baea_CommandLine::theDouble(const bsl::string& name) const
{
    return data().theDouble(name.c_str());
}

const bsl::string& baea_CommandLine::theString(const bsl::string& name) const
{
    return data().theString(name.c_str());
}

const bdet_Datetime&
baea_CommandLine::theDatetime(const bsl::string& name) const
{
    return data().theDatetime(name.c_str());
}

const bdet_Date& baea_CommandLine::theDate(const bsl::string& name) const
{
    return data().theDate(name.c_str());
}

const bdet_Time& baea_CommandLine::theTime(const bsl::string& name) const
{
    return data().theTime(name.c_str());
}

const bsl::vector<char>&
baea_CommandLine::theCharArray(const bsl::string& name) const
{
    return data().theCharArray(name.c_str());
}

const bsl::vector<short>&
baea_CommandLine::theShortArray(const bsl::string& name) const
{
    return data().theShortArray(name.c_str());
}

const bsl::vector<int>&
baea_CommandLine::theIntArray(const bsl::string& name) const
{
    return data().theIntArray(name.c_str());
}

const bsl::vector<bsls_PlatformUtil::Int64>&
baea_CommandLine::theInt64Array(const bsl::string& name) const
{
    return data().theInt64Array(name.c_str());
}

const bsl::vector<float>&
baea_CommandLine::theFloatArray(const bsl::string& name) const
{
    return data().theFloatArray(name.c_str());
}

const bsl::vector<double>&
baea_CommandLine::theDoubleArray(const bsl::string& name) const
{
    return data().theDoubleArray(name.c_str());
}

const bsl::vector<bsl::string>&
baea_CommandLine::theStringArray(const bsl::string& name) const
{
    return data().theStringArray(name.c_str());
}

const bsl::vector<bdet_Datetime>&
baea_CommandLine::theDatetimeArray(const bsl::string& name) const
{
    return data().theDatetimeArray(name.c_str());
}

const bsl::vector<bdet_Date>&
baea_CommandLine::theDateArray(const bsl::string& name) const
{
    return data().theDateArray(name.c_str());
}

const bsl::vector<bdet_Time>&
baea_CommandLine::theTimeArray(const bsl::string& name) const
{
    return data().theTimeArray(name.c_str());
}

void baea_CommandLine::printUsage() const
{
    printUsage(bsl::cerr);
}

void baea_CommandLine::printUsage(bsl::ostream& stream) const
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
          case baea_CommandLineOptionInfo::BAEA_FLAG:
          case baea_CommandLineOptionInfo::BAEA_OPTION: {
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
            if (baea_CommandLineOptionInfo::BAEA_FLAG != 
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
          case baea_CommandLineOptionInfo::BAEA_NON_OPTION: {
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
                                 baea_CommandLineOptionInfo::BAEA_NON_OPTION) {
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

            if (baea_CommandLineOptionInfo::BAEA_OPTION ==
                                                      d_options[i].argType()) {
                str.append(1, '<').append(d_options[i].name()).append(1, '>');
            }

            str.append(dscrStart - str.size(), ' ');
            stream << str;

            str = d_options[i].description();
            if (baea_CommandLineOptionInfo::BAEA_OPTION ==
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

bsl::ostream& baea_CommandLine::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    BSLS_ASSERT(0 <= spacesPerLevel);

    if (BAEA_PARSED == d_state) {
        const bdem_Descriptor *desc =
                  bdem_ElemAttrLookup::lookupTable()[bdem_ElemType::BDEM_LIST];
        bdem_SchemaAggregateUtil::print(stream,
                                        bdem_ConstElemRef(&d_data1, desc),
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

// FREE OPERATORS
bool operator==(const baea_CommandLine& lhs, const baea_CommandLine& rhs)
{
    return lhs.isParsed() && rhs.isParsed() && lhs.data() == rhs.data();
}

bool operator!=(const baea_CommandLine& lhs, const baea_CommandLine& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const baea_CommandLine& rhs)
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
