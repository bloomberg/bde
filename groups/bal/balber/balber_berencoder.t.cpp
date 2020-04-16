// balber_berencoder.t.cpp                                            -*-C++-*-

#include <balber_berencoder.h>

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balber_berconstants.h>
#include <balber_berutil.h>

#include <s_baltst_address.h>
#include <s_baltst_basicrecord.h>
#include <s_baltst_bigrecord.h>
#include <s_baltst_customizedstring.h>
#include <s_baltst_employee.h>
#include <s_baltst_mychoice.h>
#include <s_baltst_myenumeration.h>
#include <s_baltst_mysequence.h>
#include <s_baltst_mysequencewithanonymouschoice.h>
#include <s_baltst_mysequencewithanonymouschoicechoice.h>
#include <s_baltst_mysequencewitharray.h>
#include <s_baltst_mysequencewithnillable.h>
#include <s_baltst_mysequencewithnullable.h>
#include <s_baltst_sqrt.h>
#include <s_baltst_timingrequest.h>

#include <bdlat_attributeinfo.h>
#include <bdlat_enumeratorinfo.h>
#include <bdlat_formattingmode.h>
#include <bdlat_selectioninfo.h>
#include <bdlat_sequencefunctions.h>
#include <bdlat_typetraits.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_chartype.h>
#include <bdlb_nullablevalue.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>
#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_memoutstreambuf.h>
#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_time.h>

#include <bslim_testutil.h>

#include <bslalg_typetraits.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsl_cctype.h>
#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_fstream.h>
#include <bsl_iomanip.h>
#include <bsl_iosfwd.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace test = BloombergLP::s_baltst;
using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::dec;
using bsl::hex;
using bsl::flush;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

int numOctets(const char *s)
    // Return the number of octets contained in the specified 's'.  Note that
    // it is assumed that each octet in 's' is specified in hex format.
{
    int length = 0;
    for (; *s; ++s) {
        if (' ' == *s) {
            continue;
        }

        ++length;
    }
    return length / 2;
}

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };
enum { SUCCESS = 0, FAILURE = -1 };

static bool         verbose = false;
static bool     veryVerbose = false;
static bool veryVeryVerbose = false;

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

int getIntValue(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';                                               // RETURN
    }
    c = static_cast<char>(toupper(c));
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;                                          // RETURN
    }
    ASSERT(0);
    return -1;
}

int compareBuffers(const char *stream, const char *buffer)
    // Compare the data written to the  specified 'stream' with the data in the
    // specified 'buffer'.  Return 0 on success, and -1 otherwise.
{
    while (*buffer) {
        if (' ' == *buffer) {
            ++buffer;
            continue;
        }
        char temp = static_cast<char>(getIntValue(*buffer) << 4);
        ++buffer;
        temp = static_cast<char>(temp | getIntValue(*buffer));
        if (*stream != temp) {
           return -1;                                                 // RETURN
        }
        ++stream;
        ++buffer;
    }
    return 0;
}

void printBuffer(const char *buffer, int length)
    // Print the specified 'buffer' of the specified 'length' in hex form.
{
    bsl::cout << bsl::hex;
    int numOutput = 0;
    for (int i = 0; i < length; ++i) {
        if ((unsigned char) buffer[i] < 16) {
            bsl::cout << '0';
        }
        bsl::cout << (int) (unsigned char) buffer[i];
        numOutput += 2;
        if (0 == numOutput % 8) {
            cout << " ";
        }
    }
    bsl::cout << bsl::dec << bsl::endl;
}

#define DOUBLE_MANTISSA_MASK   0xfffffffffffffLL
#define DOUBLE_SIGN_MASK       ((long long) ((long long) 1                   \
                                               << (sizeof(long long) * 8 - 1)))

void assembleDouble(double *value, int sign, int exponent, long long mantissa)
{
    enum {
        DOUBLE_EXPONENT_SHIFT  = 52,
        DOUBLE_BIAS            = 1023
    };

    unsigned long long *longLongValue
                               = reinterpret_cast<unsigned long long *>(value);
    *longLongValue = 0;

    exponent += DOUBLE_BIAS;

    *longLongValue  = (unsigned long long) exponent << DOUBLE_EXPONENT_SHIFT;
    *longLongValue |= mantissa & DOUBLE_MANTISSA_MASK;

    if (sign) {
        *longLongValue |= DOUBLE_SIGN_MASK;
    }
}

void printDiagnostic(balber::BerEncoder & encoder)
{
    if (veryVerbose) {
        bsl::cout << encoder.loggedMessages();
    }
}

// ============================================================================
//                     GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------


namespace BloombergLP {

namespace s_baltst { class MyChoice; }
namespace s_baltst { class MySequenceWithNullable; }
namespace s_baltst { class Address; }
namespace s_baltst { class MySequence; }
namespace s_baltst { class MySequenceWithNillable; }
namespace s_baltst { class Sqrt; }
namespace s_baltst { class CustomizedString; }
namespace s_baltst { class BasicRecord; }
namespace s_baltst { class MySequenceWithArray; }
namespace s_baltst { class MySequenceWithAnonymousChoiceChoice; }
namespace s_baltst { class Employee; }
namespace s_baltst { class BigRecord; }
namespace s_baltst { class MySequenceWithAnonymousChoice; }
namespace s_baltst { class TimingRequest; }

namespace s_baltst {

                               // ==============
                               // class Messages
                               // ==============

struct Messages {
    // This class serves as a place holder to reserve a type having the same
    // name as this component.  Doing so ensures that such a type cannot be
    // defined outside of this component in the current namespace.
};

}  // close namespace s_baltst
}  // close enterprise namespace


// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------
//..
    namespace BloombergLP {
    namespace usage {

    struct EmployeeRecord {
        // This struct represents a sequence containing a 'string' member, an
        // 'int' member, and a 'float' member.

        // CONSTANTS
        enum {
            NAME_ATTRIBUTE_ID   = 1,
            AGE_ATTRIBUTE_ID    = 2,
            SALARY_ATTRIBUTE_ID = 3
        };

        // DATA
        bsl::string d_name;
        int         d_age;
        float       d_salary;

        // CREATORS
        EmployeeRecord();
            // Create an 'EmployeeRecord' having the attributes:
            //..
            //  d_name   == ""
            //  d_age    == 0
            //  d_salary = 0.0
            //..
        EmployeeRecord(const bsl::string& name, int age, float salary);
            // Create an 'EmployeeRecord' object having the specified
            // 'name', 'age', and 'salary' attributes.

        // ACCESSORS
        const bsl::string& name()   const;
        int                age()    const;
        float              salary() const;
    };

    // CREATORS
    EmployeeRecord::EmployeeRecord()
    : d_name()
    , d_age()
    , d_salary()
    {
    }

    EmployeeRecord::EmployeeRecord(const bsl::string& name,
                                   int               age,
                                   float             salary)
    : d_name(name)
    , d_age(age)
    , d_salary(salary)
    {
    }

    // ACCESSORS
    const bsl::string& EmployeeRecord::name() const
    {
        return d_name;
    }

    int EmployeeRecord::age() const
    {
        return d_age;
    }

    float EmployeeRecord::salary() const
    {
        return d_salary;
    }

    }  // close namespace 'usage'

    namespace usage {

    template <typename MANIPULATOR>
    int bdlat_sequenceManipulateAttribute(EmployeeRecord *object,
                                          MANIPULATOR&    manipulator,
                                          const char     *attributeName,
                                          int             attributeNameLength);
    template <typename MANIPULATOR>
    int bdlat_sequenceManipulateAttribute(EmployeeRecord *object,
                                          MANIPULATOR&    manipulator,
                                          int             attributeId);
    template <typename MANIPULATOR>
    int bdlat_sequenceManipulateAttributes(EmployeeRecord *object,
                                           MANIPULATOR&    manipulator);
    template <typename ACCESSOR>
    int bdlat_sequenceAccessAttribute(
                                   const EmployeeRecord&  object,
                                   ACCESSOR&              accessor,
                                   const char            *attributeName,
                                   int                    attributeNameLength);
    template <typename ACCESSOR>
    int bdlat_sequenceAccessAttribute(const EmployeeRecord& object,
                                      ACCESSOR&             accessor,
                                      int                   attributeId);
    template <typename ACCESSOR>
    int bdlat_sequenceAccessAttributes(const EmployeeRecord& object,
                                       ACCESSOR&             accessor);
    bool bdlat_sequenceHasAttribute(
                                   const EmployeeRecord&  object,
                                   const char            *attributeName,
                                   int                    attributeNameLength);
    bool bdlat_sequenceHasAttribute(const EmployeeRecord& object,
                                    int                   attributeId);

    }  // close namespace 'usage'

    template <typename MANIPULATOR>
    int usage::bdlat_sequenceManipulateAttribute(
                                           EmployeeRecord *object,
                                           MANIPULATOR&    manipulator,
                                           const char     *attributeName,
                                           int             attributeNameLength)
    {
        enum { k_NOT_FOUND = -1 };

        if (bdlb::String::areEqualCaseless("name",
                                           attributeName,
                                           attributeNameLength)) {

            return bdlat_sequenceManipulateAttribute(
                                            object,
                                            manipulator,
                                            EmployeeRecord::NAME_ATTRIBUTE_ID);
        }

        if (bdlb::String::areEqualCaseless("age",
                                           attributeName,
                                           attributeNameLength)) {

            return bdlat_sequenceManipulateAttribute(
                                             object,
                                             manipulator,
                                             EmployeeRecord::AGE_ATTRIBUTE_ID);
        }

        if (bdlb::String::areEqualCaseless("salary",
                                           attributeName,
                                           attributeNameLength)) {

            return bdlat_sequenceManipulateAttribute(
                                          object,
                                          manipulator,
                                          EmployeeRecord::SALARY_ATTRIBUTE_ID);
        }

        return k_NOT_FOUND;
    }

    template <typename MANIPULATOR>
    int usage::bdlat_sequenceManipulateAttribute(EmployeeRecord  *object,
                                                 MANIPULATOR&     manipulator,
                                                 int              attributeId)
    {
        enum { k_NOT_FOUND = -1 };

        switch (attributeId) {
          case EmployeeRecord::NAME_ATTRIBUTE_ID: {
            bdlat_AttributeInfo info;

            info.annotation()     = "Name of employee";
            info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
            info.id()             = EmployeeRecord::NAME_ATTRIBUTE_ID;
            info.name()           = "name";
            info.nameLength()     = 4;

            return manipulator(&object->d_name, info);
          }
          case EmployeeRecord::AGE_ATTRIBUTE_ID: {
            bdlat_AttributeInfo info;

            info.annotation()     = "Age of employee";
            info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
            info.id()             = EmployeeRecord::AGE_ATTRIBUTE_ID;
            info.name()           = "age";
            info.nameLength()     = 3;

            return manipulator(&object->d_age, info);
          }
          case EmployeeRecord::SALARY_ATTRIBUTE_ID: {
            bdlat_AttributeInfo info;

            info.annotation()     = "Salary of employee";
            info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
            info.id()             = EmployeeRecord::SALARY_ATTRIBUTE_ID;
            info.name()           = "salary";
            info.nameLength()     = 6;

            return manipulator(&object->d_salary, info);
          }
          default: {
              return k_NOT_FOUND;
          }
        }
    }

    template <typename MANIPULATOR>
    int usage::bdlat_sequenceManipulateAttributes(
                                                 EmployeeRecord   *object,
                                                 MANIPULATOR&      manipulator)
    {
        int retVal;

        retVal = bdlat_sequenceManipulateAttribute(
                                            object,
                                            manipulator,
                                            EmployeeRecord::NAME_ATTRIBUTE_ID);

        if (0 != retVal) {
            return retVal;
        }

        retVal = bdlat_sequenceManipulateAttribute(
                                             object,
                                             manipulator,
                                             EmployeeRecord::AGE_ATTRIBUTE_ID);

        if (0 != retVal) {
            return retVal;
        }

        retVal = bdlat_sequenceManipulateAttribute(
                                          object,
                                          manipulator,
                                          EmployeeRecord::SALARY_ATTRIBUTE_ID);

        return retVal;
    }

    // ACCESSORS
    template <typename ACCESSOR>
    int usage::bdlat_sequenceAccessAttribute(
                                    const EmployeeRecord&  object,
                                    ACCESSOR&              accessor,
                                    const char            *attributeName,
                                    int                    attributeNameLength)
    {
        enum { k_NOT_FOUND = -1 };

        if (bdlb::String::areEqualCaseless("name",
                                           attributeName,
                                           attributeNameLength)) {
            return bdlat_sequenceAccessAttribute(
                                            object,
                                            accessor,
                                            EmployeeRecord::NAME_ATTRIBUTE_ID);
        }

        if (bdlb::String::areEqualCaseless("age",
                                           attributeName,
                                           attributeNameLength)) {
            return bdlat_sequenceAccessAttribute(
                                             object,
                                             accessor,
                                             EmployeeRecord::AGE_ATTRIBUTE_ID);
        }

        if (bdlb::String::areEqualCaseless("salary",
                                           attributeName,
                                           attributeNameLength)) {

            return bdlat_sequenceAccessAttribute(
                                          object,
                                          accessor,
                                          EmployeeRecord::SALARY_ATTRIBUTE_ID);
        }

        return k_NOT_FOUND;
    }

    template <typename ACCESSOR>
    int usage::bdlat_sequenceAccessAttribute(const EmployeeRecord& object,
                                             ACCESSOR&             accessor,
                                             int                   attributeId)
    {
        enum { k_NOT_FOUND = -1 };

        switch (attributeId) {
          case EmployeeRecord::NAME_ATTRIBUTE_ID: {
            bdlat_AttributeInfo info;

            info.annotation()     = "Name of employee";
            info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
            info.id()             = EmployeeRecord::NAME_ATTRIBUTE_ID;
            info.name()           = "name";
            info.nameLength()     = 4;

            return accessor(object.d_name, info);
          }
          case EmployeeRecord::AGE_ATTRIBUTE_ID: {
            bdlat_AttributeInfo info;

            info.annotation()     = "Age of employee";
            info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
            info.id()             = EmployeeRecord::AGE_ATTRIBUTE_ID;
            info.name()           = "age";
            info.nameLength()     = 3;

            return accessor(object.d_age, info);
          }
          case EmployeeRecord::SALARY_ATTRIBUTE_ID: {
            bdlat_AttributeInfo info;

            info.annotation()     = "Salary of employee";
            info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
            info.id()             = EmployeeRecord::SALARY_ATTRIBUTE_ID;
            info.name()           = "salary";
            info.nameLength()     = 6;

            return accessor(object.d_salary, info);
          }
          default: {
              return k_NOT_FOUND;
          }
        }
    }

    template <typename ACCESSOR>
    int usage::bdlat_sequenceAccessAttributes(const EmployeeRecord& object,
                                              ACCESSOR&             accessor)
    {
        int retVal;

        retVal = bdlat_sequenceAccessAttribute(
                                            object,
                                            accessor,
                                            EmployeeRecord::NAME_ATTRIBUTE_ID);

        if (0 != retVal) {
            return retVal;
        }

        retVal = bdlat_sequenceAccessAttribute(
                                             object,
                                             accessor,
                                             EmployeeRecord::AGE_ATTRIBUTE_ID);

        if (0 != retVal) {
            return retVal;
        }

        retVal = bdlat_sequenceAccessAttribute(
                                          object,
                                          accessor,
                                          EmployeeRecord::SALARY_ATTRIBUTE_ID);

        return retVal;
    }

    bool usage::bdlat_sequenceHasAttribute(
                                    const EmployeeRecord&  ,
                                    const char            *attributeName,
                                    int                    attributeNameLength)
    {
        return bdlb::String::areEqualCaseless("name",
                                              attributeName,
                                              attributeNameLength)
            || bdlb::String::areEqualCaseless("age",
                                              attributeName,
                                              attributeNameLength)
            || bdlb::String::areEqualCaseless("salary",
                                              attributeName,
                                              attributeNameLength);
    }

    bool usage::bdlat_sequenceHasAttribute(const EmployeeRecord& ,
                                           int                   attributeId)
    {
        return EmployeeRecord::NAME_ATTRIBUTE_ID   == attributeId
            || EmployeeRecord::AGE_ATTRIBUTE_ID    == attributeId
            || EmployeeRecord::SALARY_ATTRIBUTE_ID == attributeId;
    }

    namespace bdlat_SequenceFunctions {

        template <>
        struct IsSequence<usage::EmployeeRecord> {
            enum { VALUE = 1 };
        };

    }  // close namespace 'bdlat_SequenceFunctions'
    }  // close enterprise namespace

static void usageExample()
{
    using namespace BloombergLP;
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Encoding an Employee Record
/// - - - - - - - - - - - - - - - - - - -
// Suppose that an "employee record" consists of a sequence of attributes --
// 'name', 'age', and 'salary' -- that are of types 'bsl::string', 'int', and
// 'float', respectively.  Furthermore, we have a need to BER encode employee
// records as a sequence of values (for out-of-process consumption).
//
// Assume that we have defined a 'usage::EmployeeRecord' class to represent
// employee record values, and assume that we have provided the 'bdlat'
// specializations that allow the 'balber' codec components to represent class
// values as a sequence of BER primitive values.  See
// {'bdlat_sequencefunctions'|Usage} for details of creating specializations
// for a sequence type.
//
// First, we create an employee record object having typical values:
//..
    usage::EmployeeRecord bob("Bob", 56, 1234.00);
    ASSERT("Bob"   == bob.name());
    ASSERT(  56    == bob.age());
    ASSERT(1234.00 == bob.salary());
//..
// Now, we create a 'balber::Encoder' object and use it to encode our 'bob'
// object.  Here, to facilitate the examination of our results, the BER
// encoding data is delivered to a 'bslsb::MemOutStreamBuf' object:
//..
    bdlsb::MemOutStreamBuf osb;
    balber::BerEncoder     encoder;
    int                    rc = encoder.encode(&osb, bob);
    ASSERT( 0 == rc);
    ASSERT(18 == osb.length());
//..
// Finally, we confirm that the generated BER encoding has the expected layout
// and values.  We create an 'bdlsb::FixedMemInStreamBuf' to manage our access
// to the data portion of the 'bdlsb::MemOutStreamBuf' where our BER encoding
// resides:
//..
    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
//..
// The 'balber_berutil' component provides functions that allow us to decode
// the descriptive fields and values of the BER encoded sequence:
//..
    balber::BerConstants::TagClass tagClass;
    balber::BerConstants::TagType  tagType;
    int                            tagNumber;
    int                            accumNumBytesConsumed = 0;
    int                            length;

    rc = balber::BerUtil::getIdentifierOctets(&isb,
                                              &tagClass,
                                              &tagType,
                                              &tagNumber,
                                              &accumNumBytesConsumed);
    ASSERT(0                                             == rc);
    ASSERT(balber::BerConstants::e_UNIVERSAL             == tagClass);
    ASSERT(balber::BerConstants::e_CONSTRUCTED           == tagType);
    ASSERT(balber::BerUniversalTagNumber::e_BER_SEQUENCE == tagNumber);

    rc = balber::BerUtil::getLength(&isb, &length, &accumNumBytesConsumed);
    ASSERT(0                                    == rc);
    ASSERT(balber::BerUtil::e_INDEFINITE_LENGTH == length);
//..
// The 'UNIVERSAL' value in 'tagClass' indicates that the 'tagNumber' value
// represents a type in the BER standard, a 'BER_SEQUENCE', as we requested of
// the infrastructure (see the 'IsSequence' specialization above).  The
// 'tagType' value of 'CONSTRUCTED' indicates that this is a non-primitive
// type.  The 'INDEFINITE' value for length is typical for sequence encodings.
// In these cases, the end-of-data is indicated by a sequence to two null
// bytes.
//
// We now examine the tags and values corresponding to each of the data members
// of 'usage::EmployeeRecord' class.  For each of these the 'tagClass' is
// 'CONTEXT_SPECIFIC' (i.e., member of a larger construct) and the 'tagType' is
// 'PRIMITIVE' ('bsl::string', 'int', and 'float' each correspond to a
// primitive BER type.  The 'tagNumber' for each field was defined (in the
// elided definiton) to correspond the position of the field in the
// 'usage::EmployeeRecord' class.
//..
    rc = balber::BerUtil::getIdentifierOctets(&isb,
                                              &tagClass,
                                              &tagType,
                                              &tagNumber,
                                              &accumNumBytesConsumed);
    ASSERT(0                                        == rc);
    ASSERT(balber::BerConstants::e_CONTEXT_SPECIFIC == tagClass);
    ASSERT(balber::BerConstants::e_PRIMITIVE        == tagType);
    ASSERT(1                                        == tagNumber);

    bsl::string name;
    rc = balber::BerUtil::getValue(&isb, &name, &accumNumBytesConsumed);
    ASSERT(0     == rc);
    ASSERT("Bob" == name);

    rc = balber::BerUtil::getIdentifierOctets(&isb,
                                              &tagClass,
                                              &tagType,
                                              &tagNumber,
                                              &accumNumBytesConsumed);
    ASSERT(0                                        == rc);
    ASSERT(balber::BerConstants::e_CONTEXT_SPECIFIC == tagClass);
    ASSERT(balber::BerConstants::e_PRIMITIVE        == tagType);
    ASSERT(2                                        == tagNumber);

    int age = 0;
    rc      = balber::BerUtil::getValue(&isb, &age, &accumNumBytesConsumed);
    ASSERT(0  == rc);
    ASSERT(56 == age);

    rc = balber::BerUtil::getIdentifierOctets(&isb,
                                              &tagClass,
                                              &tagType,
                                              &tagNumber,
                                              &accumNumBytesConsumed);
    ASSERT(0 == rc);
    ASSERT(balber::BerConstants::e_CONTEXT_SPECIFIC == tagClass);
    ASSERT(balber::BerConstants::e_PRIMITIVE        == tagType);
    ASSERT(3                                        == tagNumber);

    float salary = 0.0;
    rc = balber::BerUtil::getValue(&isb, &salary, &accumNumBytesConsumed);
    ASSERT(0       == rc);
    ASSERT(1234.00 == salary);
//..
// Lastly, we confirm that end-of-data sequence (two null bytes) are found we
// expect them and that we have entirely consumed the data that we generated by
// our encoding.
//..
    rc = balber::BerUtil::getEndOfContentOctets(&isb, &accumNumBytesConsumed);
    ASSERT(0            == rc);
    ASSERT(osb.length() == static_cast<bsl::size_t>(accumNumBytesConsumed));
//..
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? bsl::atoi(argv[1]) : 0;
            verbose = argc > 2;
        veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    balber::BerEncoder encoder(0);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 14: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;
        usageExample();

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'encode' for date/time components
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'encode' for date/time"
                               << "\n=============================="
                               << bsl::endl;

        if (verbose) bsl::cout << "\nTesting Date." << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                bool        d_useBinary; // whether to use binary format
                const char *d_exp;       // expected output
            } DATA[] = {
  //------------^
  //line  year   month   day   opt  exp
  //----  -----  -----   ---   ---  ---
  {   L_, 2020,      1,    1,    1, "04 01 00"                       },
  {   L_, 2020,      1,    1,    0, "1a 0a 323032 302d3031 2d3031"   },

  {   L_, 2020,      1,    2,    1, "04 01 01"                       },
  {   L_, 2020,      1,    2,    0, "1a 0a 323032 302d3031 2d3032"   },

  {   L_, 2020,      1,   31,    1, "04 01 1E"                       },
  {   L_, 2020,      1,   31,    0, "1a 0a 323032 302d3031 2d3331"   },

  {   L_, 2020,      2,    1,    1, "04 01 1F"                       },
  {   L_, 2020,      2,    1,    0, "1a 0a 323032 302d3032 2d3031"   },

  {   L_, 2020,      2,   28,    1, "04 01 3A"                       },
  {   L_, 2020,      2,   28,    0, "1a 0a 323032 302d3032 2d3238"   },

  {   L_, 2020,      2,   29,    1, "04 01 3B"                       },
  {   L_, 2020,      2,   29,    0, "1a 0a 323032 302d3032 2d3239"   },

  {   L_, 2020,      3,    1,    1, "04 01 3C"                       },
  {   L_, 2020,      3,    1,    0, "1a 0a 323032 302d3033 2d3031"   },

  {   L_, 2020,      5,    7,    1, "04 01 7F"                       },
  {   L_, 2020,      5,    7,    0, "1a 0a 323032 302d3035 2d3037"   },

  {   L_, 2020,      5,    8,    1, "04 02 0080"                     },
  {   L_, 2020,      5,    8,    0, "1a 0a 323032 302d3035 2d3038"   },

  {   L_, 2020,     12,   31,    1, "04 02 016D"                     },
  {   L_, 2020,     12,   31,    0, "1a 0a 323032 302d3132 2d3331"   },

  {   L_, 2021,      1,    1,    1, "04 02 016E"                     },
  {   L_, 2021,      1,    1,    0, "1a 0a 323032 312d3031 2d3031"   },

  {   L_, 2023,     12,   31,    1, "04 02 05B4"                     },
  {   L_, 2023,     12,   31,    0, "1a 0a 323032 332d3132 2d3331"   },

  {   L_, 2024,      1,    1,    1, "04 02 05B5"                     },
  {   L_, 2024,      1,    1,    0, "1a 0a 323032 342d3031 2d3031"   },

  {   L_, 2099,     12,   31,    1, "04 02 7223"                     },
  {   L_, 2099,     12,   31,    0, "1a 0a 323039 392d3132 2d3331"   },

  {   L_, 2100,      1,    1,    1, "04 02 7224"                     },
  {   L_, 2100,      1,    1,    0, "1a 0a 323130 302d3031 2d3031"   },

  {   L_, 2100,      2,   28,    1, "04 02 725E"                     },
  {   L_, 2100,      2,   28,    0, "1a 0a 323130 302d3032 2d3238"   },

  {   L_, 2100,      3,    1,    1, "04 02 725F"                     },
  {   L_, 2100,      3,    1,    0, "1a 0a 323130 302d3033 2d3031"   },

  {   L_, 2109,      9,   18,    1, "04 02 7FFF"                     },
  {   L_, 2109,      9,   18,    0, "1a 0a 323130 392d3039 2d3138"   },

  {   L_, 2109,      9,   19,    1, "04 03 008000"                   },
  {   L_, 2109,      9,   19,    0, "1a 0a 323130 392d3039 2d3139"   },

  {   L_, 2119,     12,   31,    1, "04 03 008EAB"                   },
  {   L_, 2119,     12,   31,    0, "1a 0a 323131 392d3132 2d3331"   },

  {   L_, 2120,      1,    1,    1, "04 03 008EAC"                   },
  {   L_, 2120,      1,    1,    0, "1a 0a 323132 302d3031 2d3031"   },

  {   L_, 2019,     12,   31,    1, "04 01 FF"                       },
  {   L_, 2019,     12,   31,    0, "1a 0a 323031 392d3132 2d3331"   },

  {   L_, 2019,      8,   26,    1, "04 01 80"                       },
  {   L_, 2019,      8,   26,    0, "1a 0a 323031 392d3038 2d3236"   },

  {   L_, 2019,      8,   25,    1, "04 02 FF7F"                     },
  {   L_, 2019,      8,   25,    0, "1a 0a 323031 392d3038 2d3235"   },

  {   L_, 2016,      2,   28,    1, "04 02 FA85"                     },
  {   L_, 2016,      2,   28,    0, "1a 0a 323031 362d3032 2d3238"   },

  {   L_, 2016,      2,   29,    1, "04 02 FA86"                     },
  {   L_, 2016,      2,   29,    0, "1a 0a 323031 362d3032 2d3239"   },

  {   L_, 2016,      3,    1,    1, "04 02 FA87"                     },
  {   L_, 2016,      3,    1,    0, "1a 0a 323031 362d3033 2d3031"   },

  {   L_, 2012,      2,   28,    1, "04 02 F4D0"                     },
  {   L_, 2012,      2,   28,    0, "1a 0a 323031 322d3032 2d3238"   },

  {   L_, 2012,      2,   29,    1, "04 02 F4D1"                     },
  {   L_, 2012,      2,   29,    0, "1a 0a 323031 322d3032 2d3239"   },

  {   L_, 2012,      3,    1,    1, "04 02 F4D2"                     },
  {   L_, 2012,      3,    1,    0, "1a 0a 323031 322d3033 2d3031"   },

  {   L_, 2000,      2,   28,    1, "04 02 E3B1"                     },
  {   L_, 2000,      2,   28,    0, "1a 0a 323030 302d3032 2d3238"   },

  {   L_, 2000,      2,   29,    1, "04 02 E3B2"                     },
  {   L_, 2000,      2,   29,    0, "1a 0a 323030 302d3032 2d3239"   },

  {   L_, 2000,      3,    1,    1, "04 02 E3B3"                     },
  {   L_, 2000,      3,    1,    0, "1a 0a 323030 302d3033 2d3031"   },

  {   L_, 1930,      4,   15,    1, "04 02 8000"                     },
  {   L_, 1930,      4,   15,    0, "1a 0a 313933 302d3034 2d3135"   },

  {   L_, 1930,      4,   14,    1, "04 03 FF7FFF"                   },
  {   L_, 1930,      4,   14,    0, "1a 0a 313933 302d3034 2d3134"   },

  {   L_, 1066,     10,   14,    1, "04 03 FAB005"                   },
  {   L_, 1066,     10,   14,    0, "1a 0a 313036 362d3130 2d3134"   },

  {   L_, 9999,     12,   31,    1, "04 03 2C794A"                   },
  {   L_, 9999,     12,   31,    0, "1a 0a 393939 392d3132 2d3331"   },

  {   L_,    1,      1,    1,    1, "04 03 F4BF70"                   },
  {   L_,    1,      1,    1,    0, "1a 0a 303030 312d3031 2d3031"   },
  //------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE  = DATA[i].d_lineNum;
                const int   YEAR  = DATA[i].d_year;
                const int   MONTH = DATA[i].d_month;
                const int   DAY   = DATA[i].d_day;
                const bool  BIN   = DATA[i].d_useBinary;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                LOOP_ASSERT(LINE, bdlt::Date::isValidYearMonthDay(YEAR,
                                                                  MONTH,
                                                                  DAY));

                if (veryVerbose) { P_(YEAR) P_(MONTH) P_(DAY) P(EXP) }

                balber::BerEncoderOptions options;
                options.setEncodeDateAndTimeTypesAsBinary(BIN);

                const bdlt::Date VALUE(YEAR, MONTH, DAY);

                bdlsb::MemOutStreamBuf osb;
                balber::BerEncoder encoder(&options);
                ASSERT(0 == encoder.encode(&osb, VALUE));
                LOOP2_ASSERT(LEN, osb.length(), LEN == (int)osb.length());
                LOOP2_ASSERT(osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting DateTz." << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                int         d_tzoffset;  // time zone offset
                bool        d_useBinary; // whether to use binary format
                const char *d_exp;       // expected output
            } DATA[] = {
  //------------^
  //line  year   month   day   off   opt  exp
  //----  -----  -----   ---   ---   ---  ---
  {   L_, 2020,      1,    1,    0,    1, "04 01 00"                         },
  {   L_, 2020,      1,    1,    0,    0,
                              "1A 10 323032 302d3031 2d30312B 30303A30 30"   },

  {   L_, 2020,      1,    1, 1439,    1, "04 04 059F0000"                   },
  {   L_, 2020,      1,    1, 1439,    0,
                              "1A 10 323032 302d3031 2d30312B 32333A35 39"   },

  {   L_, 2020,      1,    1,-1439,    1, "04 04 FA610000"                   },
  {   L_, 2020,      1,    1,-1439,    0,
                              "1A 10 323032 302d3031 2d30312D 32333A35 39"   },

  {   L_, 2020,      1,    2,    0,    1, "04 01 01"                         },
  {   L_, 2020,      1,    2,    0,    0,
                              "1A 10 323032 302d3031 2d30322B 30303A30 30"   },

  {   L_, 2020,      1,    2, 1439,    1, "04 04 059F0001"                   },
  {   L_, 2020,      1,    2, 1439,    0,
                              "1A 10 323032 302d3031 2d30322B 32333A35 39"   },

  {   L_, 2020,      1,    2,-1439,    1, "04 04 FA610001"                   },
  {   L_, 2020,      1,    2,-1439,    0,
                              "1A 10 323032 302d3031 2d30322D 32333A35 39"   },

  {   L_, 2019,     12,   31,    0,    1, "04 01 FF"                         },
  {   L_, 2019,     12,   31,    0,    0,
                              "1A 10 323031 392d3132 2d33312B 30303A30 30"   },

  {   L_, 2019,     12,   31, 1439,    1, "04 04 059FFFFF"                   },
  {   L_, 2019,     12,   31, 1439,    0,
                              "1A 10 323031 392d3132 2d33312B 32333A35 39"   },

  {   L_, 2019,     12,   31,-1439,    1, "04 04 FA61FFFF"                   },
  {   L_, 2019,     12,   31,-1439,    0,
                              "1A 10 323031 392d3132 2d33312D 32333A35 39"   },

  {   L_,    1,      1,    1,    0,    1, "04 03 F4BF70"                     },
  {   L_,    1,      1,    1,    0,    0,
                              "1A 10 303030 312d3031 2d30312B 30303A30 30"   },

  {   L_,    1,      1,    1, 1439,    1, "04 05 059FF4BF 70"                },
  {   L_,    1,      1,    1, 1439,    0,
                              "1A 10 303030 312d3031 2d30312B 32333A35 39"   },

  {   L_,    1,      1,    1,-1439,    1, "04 05 FA61F4BF 70"                },
  {   L_,    1,      1,    1,-1439,    0,
                              "1A 10 303030 312d3031 2d30312D 32333A35 39"   },

  {   L_, 2020,      5,    7,    0,    1, "04 01 7F"                         },
  {   L_, 2020,      5,    7,    0,    0,
                              "1A 10 323032 302d3035 2d30372B 30303A30 30"   },

  {   L_, 2020,      5,    7, 1439,    1, "04 04 059F007F"                   },
  {   L_, 2020,      5,    7, 1439,    0,
                              "1A 10 323032 302d3035 2d30372B 32333A35 39"   },

  {   L_, 2020,      5,    7,-1439,    1, "04 04 FA61007F"                   },
  {   L_, 2020,      5,    7,-1439,    0,
                              "1A 10 323032 302d3035 2d30372D 32333A35 39"   },

  {   L_, 2020,      5,    8,    0,    1, "04 02 0080"                       },
  {   L_, 2020,      5,    8,    0,    0,
                              "1A 10 323032 302d3035 2d30382B 30303A30 30"   },

  {   L_, 2020,      5,    8, 1439,    1, "04 04 059F0080"                   },
  {   L_, 2020,      5,    8, 1439,    0,
                              "1A 10 323032 302d3035 2d30382B 32333A35 39"   },

  {   L_, 2020,      5,    8,-1439,    1, "04 04 FA610080"                   },
  {   L_, 2020,      5,    8,-1439,    0,
                              "1A 10 323032 302d3035 2d30382D 32333A35 39"   },

  {   L_, 2109,      9,   18,    0,    1, "04 02 7FFF"                       },
  {   L_, 2109,      9,   18,    0,    0,
                              "1A 10 323130 392d3039 2d31382B 30303A30 30"   },

  {   L_, 2109,      9,   18, 1439,    1, "04 04 059F7FFF"                   },
  {   L_, 2109,      9,   18, 1439,    0,
                              "1A 10 323130 392d3039 2d31382B 32333A35 39"   },

  {   L_, 2109,      9,   18,-1439,    1, "04 04 FA617FFF"                   },
  {   L_, 2109,      9,   18,-1439,    0,
                              "1A 10 323130 392d3039 2d31382D 32333A35 39"   },

  {   L_, 2109,      9,   19,    0,    1, "04 03 008000"                     },
  {   L_, 2109,      9,   19,    0,    0,
                              "1A 10 323130 392d3039 2d31392B 30303A30 30"   },

  {   L_, 2109,      9,   19, 1439,    1, "04 05 059F008000"                 },
  {   L_, 2109,      9,   19, 1439,    0,
                              "1A 10 323130 392d3039 2d31392B 32333A35 39"   },

  {   L_, 2109,      9,   19,-1439,    1, "04 05 FA61008000"                 },
  {   L_, 2109,      9,   19,-1439,    0,
                              "1A 10 323130 392d3039 2d31392D 32333A35 39"   },

  {   L_, 2019,      8,   26,    0,    1, "04 01 80"                         },
  {   L_, 2019,      8,   26,    0,    0,
                              "1A 10 323031 392d3038 2d32362B 30303A30 30"   },

  {   L_, 2019,      8,   26, 1439,    1, "04 04 059FFF80"                   },
  {   L_, 2019,      8,   26, 1439,    0,
                              "1A 10 323031 392d3038 2d32362B 32333A35 39"   },

  {   L_, 2019,      8,   26,-1439,    1, "04 04 FA61FF80"                   },
  {   L_, 2019,      8,   26,-1439,    0,
                              "1A 10 323031 392d3038 2d32362D 32333A35 39"   },

  {   L_, 2019,      8,   25,    0,    1, "04 02 FF7F"                       },
  {   L_, 2019,      8,   25,    0,    0,
                              "1A 10 323031 392d3038 2d32352B 30303A30 30"   },

  {   L_, 2019,      8,   25, 1439,    1, "04 04 059FFF7F"                   },
  {   L_, 2019,      8,   25, 1439,    0,
                              "1A 10 323031 392d3038 2d32352B 32333A35 39"   },

  {   L_, 2019,      8,   25,-1439,    1, "04 04 FA61FF7F"                   },
  {   L_, 2019,      8,   25,-1439,    0,
                              "1A 10 323031 392d3038 2d32352D 32333A35 39"   },

  {   L_, 1930,      4,   15,    0,    1, "04 02 8000"                       },
  {   L_, 1930,      4,   15,    0,    0,
                              "1A 10 313933 302d3034 2d31352B 30303A30 30"   },

  {   L_, 1930,      4,   15, 1439,    1, "04 04 059F8000"                   },
  {   L_, 1930,      4,   15, 1439,    0,
                              "1A 10 313933 302d3034 2d31352B 32333A35 39"   },

  {   L_, 1930,      4,   15,-1439,    1, "04 04 FA618000"                   },
  {   L_, 1930,      4,   15,-1439,    0,
                              "1A 10 313933 302d3034 2d31352D 32333A35 39"   },

  {   L_, 1930,      4,   14,    0,    1, "04 03 FF7FFF"                     },
  {   L_, 1930,      4,   14,    0,    0,
                              "1A 10 313933 302d3034 2d31342B 30303A30 30"   },

  {   L_, 1930,      4,   14, 1439,    1, "04 05 059FFF7FFF"                 },
  {   L_, 1930,      4,   14, 1439,    0,
                              "1A 10 313933 302d3034 2d31342B 32333A35 39"   },

  {   L_, 1930,      4,   14,-1439,    1, "04 05 FA61FF7FFF"                 },
  {   L_, 1930,      4,   14,-1439,    0,
                              "1A 10 313933 302d3034 2d31342D 32333A35 39"   },

  {   L_, 1066,     10,   14,    0,    1, "04 03 FAB005"                     },
  {   L_, 1066,     10,   14,    0,    0,
                              "1A 10 313036 362d3130 2d31342B 30303A30 30"   },

  {   L_, 1066,     10,   14, 1439,    1, "04 05 059FFAB0 05"                },
  {   L_, 1066,     10,   14, 1439,    0,
                              "1A 10 313036 362d3130 2d31342B 32333A35 39"   },

  {   L_, 1066,     10,   14,-1439,    1, "04 05 FA61FAB0 05"                },
  {   L_, 1066,     10,   14,-1439,    0,
                              "1A 10 313036 362d3130 2d31342D 32333A35 39"   },

  {   L_, 9999,     12,   31,    0,    1, "04 03 2C794A"                     },
  {   L_, 9999,     12,   31,    0,    0,
                              "1A 10 393939 392d3132 2d33312B 30303A30 30"   },

  {   L_, 9999,     12,   31, 1439,    1, "04 05 059F2C79 4A"                },
  {   L_, 9999,     12,   31, 1439,    0,
                              "1A 10 393939 392d3132 2d33312B 32333A35 39"   },

  {   L_, 9999,     12,   31,-1439,    1, "04 05 FA612C79 4A"                },
  {   L_, 9999,     12,   31,-1439,    0,
                              "1A 10 393939 392d3132 2d33312D 32333A35 39"   },
  //------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_lineNum;
                const int   YEAR  = DATA[i].d_year;
                const int   MONTH = DATA[i].d_month;
                const int   DAY   = DATA[i].d_day;
                const int   OFF   = DATA[i].d_tzoffset;
                const bool  BIN   = DATA[i].d_useBinary;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                LOOP_ASSERT(LINE, bdlt::Date::isValidYearMonthDay(YEAR,
                                                                  MONTH,
                                                                  DAY));

                if (veryVerbose) { P_(YEAR) P_(MONTH) P_(DAY) P_(OFF) P(EXP) }

                balber::BerEncoderOptions options;
                options.setEncodeDateAndTimeTypesAsBinary(BIN);

                const bdlt::DateTz VALUE(bdlt::Date(YEAR, MONTH, DAY), OFF);

                bdlsb::MemOutStreamBuf osb;
                balber::BerEncoder encoder(&options);
                ASSERT(0 == encoder.encode(&osb, VALUE));
                LOOP2_ASSERT(LEN, osb.length(), LEN == (int)osb.length());
                LOOP2_ASSERT(osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting Time." << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_hour;      // hour under test
                int         d_min;       // min under test
                int         d_sec;       // sec under test
                int         d_milliSec;  // milliSec under test
                bool        d_useBinary; // whether to use binary format
                const char *d_exp;       // expected output
            } DATA[] = {
  //------------^
  //line  hour   min  sec  ms   opt  exp
  //----  -----  ---  ---  ---  ---  ---
  {   L_,     0,   0,   0,   0,   1, "04 01 00"                              },
  {   L_,     0,   0,   0,   0,   0, "1A 0C 30303A30 303A3030 2E303030"      },

  {   L_,     0,   0,   0, 127,   1, "04 01 7F"                              },
  {   L_,     0,   0,   0, 127,   0, "1A 0C 30303A30 303A3030 2E313237"      },

  {   L_,     0,   0,   0, 128,   1, "04 02 0080"                            },
  {   L_,     0,   0,   0, 128,   0, "1A 0C 30303A30 303A3030 2E313238"      },

  {   L_,     0,   0,   0, 255,   1, "04 02 00FF"                            },
  {   L_,     0,   0,   0, 255,   0, "1A 0C 30303A30 303A3030 2E323535"      },

  {   L_,     0,   0,   0, 256,   1, "04 02 0100"                            },
  {   L_,     0,   0,   0, 256,   0, "1A 0C 30303A30 303A3030 2E323536"      },

  {   L_,     0,   0,  32, 767,   1, "04 02 7FFF"                            },
  {   L_,     0,   0,  32, 767,   0, "1A 0C 30303A30 303A3332 2E373637"      },

  {   L_,     0,   0,  32, 768,   1, "04 03 008000"                          },
  {   L_,     0,   0,  32, 768,   0, "1A 0C 30303A30 303A3332 2E373638"      },

  {   L_,     2,  19,  48, 607,   1, "04 03 7FFFFF"                          },
  {   L_,     2,  19,  48, 607,   0, "1A 0C 30323A31 393A3438 2E363037"      },

  {   L_,     2,  19,  48, 608,   1, "04 04 00800000"                        },
  {   L_,     2,  19,  48, 608,   0, "1A 0C 30323A31 393A3438 2E363038"      },

  {   L_,    12,  33,  45, 999,   1, "04 04 02B2180F"                        },
  {   L_,    12,  33,  45, 999,   0, "1A 0C 31323A33 333A3435 2E393939"      },

  {   L_,    23,  59,  59, 999,   1, "04 04 05265BFF"                        },
  {   L_,    23,  59,  59, 999,   0, "1A 0C 32333A35 393A3539 2E393939"      },

  {   L_,    24,   0,   0,   0,   1, "04 01 00"                              },
// TBD: Current doesnt work
// {  L_,    24,   0,   0,   0,   0, "1A 0C 30303A30 303A3030 2E303030"      },
  //------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE  = DATA[i].d_lineNum;
                const int   HOUR  = DATA[i].d_hour;
                const int   MIN   = DATA[i].d_min;
                const int   SECS  = DATA[i].d_sec;
                const int   MSEC  = DATA[i].d_milliSec;
                const bool  BIN   = DATA[i].d_useBinary;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                if (veryVerbose) { P_(HOUR) P_(MIN) P_(SECS) P(MSEC) }

                balber::BerEncoderOptions options;
                options.setEncodeDateAndTimeTypesAsBinary(BIN);

                const bdlt::Time VALUE(HOUR, MIN, SECS, MSEC);

                bdlsb::MemOutStreamBuf osb;
                balber::BerEncoder encoder(&options);
                ASSERT(0 == encoder.encode(&osb, VALUE));
                LOOP3_ASSERT(LINE, LEN, osb.length(),
                             LEN == (int)osb.length());
                LOOP3_ASSERT(LINE, osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting TimeTz." << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_hour;      // hour under test
                int         d_min;       // min under test
                int         d_sec;       // sec under test
                int         d_milliSec;  // milliSec under test
                int         d_offset;    // offset
                bool        d_useBinary; // whether to use binary format
                const char *d_exp;       // expected output
            } DATA[] = {
  //------------^
  //line  hour   min  sec  ms     off  opt  exp
  //----  -----  ---  ---  ---    ---  ---  ---
  {   L_,     0,   0,   0,   0,     0,  1, "04 01 00"                        },
  {   L_,     0,   0,   0,   0,     0,  0,
                            "1A 12 30303A30 303A3030 2E303030 2B30303A 3030" },

  {   L_,     0,   0,   0,   0,  1439,  1, "04 05 059F0000 00"               },
  {   L_,     0,   0,   0,   0,  1439,  0,
                            "1A 12 30303A30 303A3030 2E303030 2B32333A 3539" },

  {   L_,     0,   0,   0,   0, -1439,  1, "04 05 FA610000 00"               },
  {   L_,     0,   0,   0,   0, -1439,  0,
                            "1A 12 30303A30 303A3030 2E303030 2D32333A 3539" },

  {   L_,     0,   0,   0, 127,     0,  1, "04 01 7F"                        },
  {   L_,     0,   0,   0, 127,     0,  0,
                            "1A 12 30303A30 303A3030 2E313237 2B30303A 3030" },

  {   L_,     0,   0,   0, 127,  1439,  1, "04 05 059F0000 7F"               },
  {   L_,     0,   0,   0, 127,  1439,  0,
                            "1A 12 30303A30 303A3030 2E313237 2B32333A 3539" },

  {   L_,     0,   0,   0, 127, -1439,  1, "04 05 FA610000 7F"               },
  {   L_,     0,   0,   0, 127, -1439,  0,
                            "1A 12 30303A30 303A3030 2E313237 2D32333A 3539" },

  {   L_,     0,   0,   0, 128,     0,  1, "04 02 0080"                      },
  {   L_,     0,   0,   0, 128,     0,  0,
                            "1A 12 30303A30 303A3030 2E313238 2B30303A 3030" },

  {   L_,     0,   0,   0, 128,  1439,  1, "04 05 059F0000 80"               },
  {   L_,     0,   0,   0, 128,  1439,  0,
                            "1A 12 30303A30 303A3030 2E313238 2B32333A 3539" },

  {   L_,     0,   0,   0, 128, -1439,  1, "04 05 FA610000 80"               },
  {   L_,     0,   0,   0, 128, -1439,  0,
                            "1A 12 30303A30 303A3030 2E313238 2D32333A 3539" },

  {   L_,     0,   0,  32, 767,     0,  1, "04 02 7FFF"                      },
  {   L_,     0,   0,  32, 767,     0,  0,
                            "1A 12 30303A30 303A3332 2E373637 2B30303A 3030" },

  {   L_,     0,   0,  32, 767,  1439,  1, "04 05 059F007F FF"               },
  {   L_,     0,   0,  32, 767,  1439,  0,
                            "1A 12 30303A30 303A3332 2E373637 2B32333A 3539" },

  {   L_,     0,   0,  32, 767, -1439,  1, "04 05 FA61007F FF"               },
  {   L_,     0,   0,  32, 767, -1439,  0,
                            "1A 12 30303A30 303A3332 2E373637 2D32333A 3539" },

  {   L_,     0,   0,  32, 768,     0,  1, "04 03 008000"                    },
  {   L_,     0,   0,  32, 768,     0,  0,
                            "1A 12 30303A30 303A3332 2E373638 2B30303A 3030" },

  {   L_,     0,   0,  32, 768,  1439,  1, "04 05 059F0080 00"               },
  {   L_,     0,   0,  32, 768,  1439,  0,
                            "1A 12 30303A30 303A3332 2E373638 2B32333A 3539" },

  {   L_,     0,   0,  32, 768, -1439,  1, "04 05 FA610080 00"               },
  {   L_,     0,   0,  32, 768, -1439,  0,
                            "1A 12 30303A30 303A3332 2E373638 2D32333A 3539" },

  {   L_,     2,  19,  48, 607,     0,  1, "04 03 7FFFFF"                    },
  {   L_,     2,  19,  48, 607,     0,  0,
                            "1A 12 30323A31 393A3438 2E363037 2B30303A 3030" },

  {   L_,     2,  19,  48, 607,  1439,  1, "04 05 059F7FFFFF"                },
  {   L_,     2,  19,  48, 607,  1439,  0,
                            "1A 12 30323A31 393A3438 2E363037 2B32333A 3539" },

  {   L_,     2,  19,  48, 607, -1439,  1, "04 05 FA617FFFFF"                },
  {   L_,     2,  19,  48, 607, -1439,  0,
                            "1A 12 30323A31 393A3438 2E363037 2D32333A 3539" },

  {   L_,     2,  19,  48, 608,     0,  1, "04 04 00800000"                  },
  {   L_,     2,  19,  48, 608,     0,  0,
                            "1A 12 30323A31 393A3438 2E363038 2B30303A 3030" },

  {   L_,     2,  19,  48, 608,  1439,  1, "04 06 059F0080 0000"             },
  {   L_,     2,  19,  48, 608,  1439,  0,
                            "1A 12 30323A31 393A3438 2E363038 2B32333A 3539" },

  {   L_,     2,  19,  48, 608, -1439,  1, "04 06 FA610080 0000"             },
  {   L_,     2,  19,  48, 608, -1439,  0,
                            "1A 12 30323A31 393A3438 2E363038 2D32333A 3539" },

  {   L_,    12,  33,  45, 999,     0,  1, "04 04 02B2180F"                  },
  {   L_,    12,  33,  45, 999,     0,  0,
                            "1A 12 31323A33 333A3435 2E393939 2B30303A 3030" },

  {   L_,    12,  33,  45, 999,  1439,  1, "04 06 059F02B2 180F"             },
  {   L_,    12,  33,  45, 999,  1439,  0,
                            "1A 12 31323A33 333A3435 2E393939 2B32333A 3539" },

  {   L_,    12,  33,  45, 999, -1439,  1, "04 06 FA6102B2 180F"             },
  {   L_,    12,  33,  45, 999, -1439,  0,
                            "1A 12 31323A33 333A3435 2E393939 2D32333A 3539" },

  {   L_,    23,  59,  59, 999,     0,  1, "04 04 05265BFF"                  },
  {   L_,    23,  59,  59, 999,     0,  0,
                            "1A 12 32333A35 393A3539 2E393939 2B30303A 3030" },

  {   L_,    23,  59,  59, 999,  1439,  1, "04 06 059F0526 5BFF"             },
  {   L_,    23,  59,  59, 999,  1439,  0,
                            "1A 12 32333A35 393A3539 2E393939 2B32333A 3539" },

  {   L_,    23,  59,  59, 999, -1439,  1, "04 06 FA610526 5BFF"             },
  {   L_,    23,  59,  59, 999, -1439,  0,
                            "1A 12 32333A35 393A3539 2E393939 2D32333A 3539" },

  {   L_,    24,   0,   0,   0,     0,  1, "04 01 00"                        },
// TBD: Current doesnt work
// {  L_,    24,   0,   0,   0,   0, "04 0C 30303A30 303A3030 2E303030"      },
  //------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE  = DATA[i].d_lineNum;
                const int   HOUR  = DATA[i].d_hour;
                const int   MIN   = DATA[i].d_min;
                const int   SECS  = DATA[i].d_sec;
                const int   MSEC  = DATA[i].d_milliSec;
                const int   OFF   = DATA[i].d_offset;
                const bool  BIN   = DATA[i].d_useBinary;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                if (veryVerbose) { P_(HOUR) P_(MIN) P_(SECS) P_(MSEC) P(OFF) }

                balber::BerEncoderOptions options;
                options.setEncodeDateAndTimeTypesAsBinary(BIN);

                const bdlt::TimeTz VALUE(bdlt::Time(HOUR, MIN, SECS, MSEC),
                                        OFF);

                bdlsb::MemOutStreamBuf osb;
                balber::BerEncoder encoder(&options);
                ASSERT(0 == encoder.encode(&osb, VALUE));
                LOOP3_ASSERT(LINE, LEN, osb.length(),
                             LEN == (int)osb.length());
                LOOP3_ASSERT(LINE, osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting Datetime." << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                int         d_hour;      // hour under test
                int         d_min;       // min under test
                int         d_sec;       // sec under test
                int         d_milliSec;  // milliSec under test
                int         d_microSec;  // microSec under test
                bool        d_useBinary; // whether to use binary format
                const char *d_exp;       // expected output
            } DATA[] = {
  //------------^
  //line  year  mon  day  hour   min  sec    ms    opt  exp
  //----  ----- ---  ---  ----   ---  ---    --    ---  ---
  {   L_, 2020,   1,   1,    0,    0,   0,    0,   0,   1,
                                            "04 01 00"                       },
  {   L_, 2020,   1,   1,    0,    0,   0,    0,   0,   0,
          "1A 1A 32303230 2d30312d 30315430 303A3030 3A30302E 303030 303030" },

  {   L_, 2020,   1,   1,    0,    0,   0,    1,   0,   1,
                                            "04 01 01"                       },
  {   L_, 2020,   1,   1,    0,    0,   0,    1,   0,   0,
          "1A 1A 32303230 2d30312d 30315430 303A3030 3A30302E 303031 303030" },
  {   L_, 2020,   1,   1,    0,    0,   0,    0,   1,   0,
          "1A 1A 32303230 2d30312d 30315430 303A3030 3A30302E 303030 303031" },

  {   L_, 2020,   1,   1,    0,    0,   0,  127,   0,   1,
                                            "04 01 7F"                       },
  {   L_, 2020,   1,   1,    0,    0,   0,  127, 127,   0,
          "1A 1A 32303230 2d30312d 30315430 303A3030 3A30302E 313237 313237" },

  {   L_, 2020,   1,   1,    0,    0,   0,  128,   0,   1,
                                            "04 02 0080"                     },
  {   L_, 2020,   1,   1,    0,    0,   0,  128, 128,   0,
          "1A 1A 32303230 2d30312d 30315430 303A3030 3A30302E 313238 313238" },

  {   L_, 2020,   1,   1,    0,    0,   0,  999,   0,   1,
                                            "04 02 03E7"                     },
  {   L_, 2020,   1,   1,    0,    0,   0,  999, 999,   0,
          "1A 1A 32303230 2d30312d 30315430 303A3030 3A30302E 393939 393939" },

  {   L_, 2020,   1,   1,    0,    0,   1,    0,   0,   1,
                                            "04 02 03E8"                     },
  {   L_, 2020,   1,   1,    0,    0,   1,    0,   0,   0,
          "1A 1A 32303230 2d30312d 30315430 303A3030 3A30312E 303030 303030" },

  {   L_, 2020,   1,   1,    0,    0,   1,    1,   0,   1,
                                            "04 02 03E9"                     },
  {   L_, 2020,   1,   1,    0,    0,   1,    1,   0,   0,
          "1A 1A 32303230 2d30312d 30315430 303A3030 3A30312E 303031 303030" },
  {   L_, 2020,   1,   1,    0,    0,   1,    0,   1,   0,
          "1A 1A 32303230 2d30312d 30315430 303A3030 3A30312E 303030 303031" },

  {   L_, 2020,   1,   1,    0,    0,  32,  767,   0,   1,
                                            "04 02 7FFF"                     },
  {   L_, 2020,   1,   1,    0,    0,  32,  767, 898,   0,
          "1A 1A 32303230 2d30312d 30315430 303A3030 3A33322E 373637 383938" },

  {   L_, 2020,   1,   1,    0,    0,  32,  768,   0,   1,
                                            "04 03 008000"                   },
  {   L_, 2020,   1,   1,    0,    0,  32,  768, 898,   0,
          "1A 1A 32303230 2d30312d 30315430 303A3030 3A33322E 373638 383938" },

  {   L_, 2020,   1,   1,    0,    0,  59,  999,   0,   1,
                                            "04 03 00EA5F"                   },
  {   L_, 2020,   1,   1,    0,    0,  59,  999, 999,   0,
          "1A 1A 32303230 2d30312d 30315430 303A3030 3A35392E 393939 393939" },

  {   L_, 2020,   1,   1,    0,    1,   0,    0,   0,   1,
                                            "04 03 00EA60"                   },
  {   L_, 2020,   1,   1,    0,    1,   0,    0,   0,   0,
          "1A 1A 32303230 2d30312d 30315430 303A3031 3A30302E 303030 303030" },

  {   L_, 2020,   1,   1,    0,    1,   0,    1,   0,   1,
                                            "04 03 00EA61"                   },
  {   L_, 2020,   1,   1,    0,    1,   0,    1,   0,   0,
          "1A 1A 32303230 2d30312d 30315430 303A3031 3A30302E 303031 303030" },
  {   L_, 2020,   1,   1,    0,    1,   0,    0,   1,   0,
          "1A 1A 32303230 2d30312d 30315430 303A3031 3A30302E 303030 303031" },

  {   L_, 2020,   1,   1,    0,   59,  59,  999,   0,   1,
                                            "04 03 36EE7F"                   },
  {   L_, 2020,   1,   1,    0,   59,  59,  999, 999,   0,
          "1A 1A 32303230 2d30312d 30315430 303A3539 3A35392E 393939 393939" },

  {   L_, 2020,   1,   1,    1,    0,   0,    0,   0,   1,
                                            "04 03 36EE80"                   },
  {   L_, 2020,   1,   1,    1,    0,   0,    0,   0,   0,
          "1A 1A 32303230 2d30312d 30315430 313A3030 3A30302E 303030 303030" },

  {   L_, 2020,   1,   1,    1,    0,   0,    1,   0,   1,
                                            "04 03 36EE81"                   },
  {   L_, 2020,   1,   1,    1,    0,   0,    1,   0,   0,
          "1A 1A 32303230 2d30312d 30315430 313A3030 3A30302E 303031 303030" },
  {   L_, 2020,   1,   1,    1,    0,   0,    0,   1,   0,
          "1A 1A 32303230 2d30312d 30315430 313A3030 3A30302E 303030 303031" },

  {   L_, 2020,   1,   1,    2,   19,  48,  607,   0,   1,
                                            "04 03 7FFFFF"                   },
  {   L_, 2020,   1,   1,    2,   19,  48,  607, 809,   0,
          "1A 1A 32303230 2d30312d 30315430 323A3139 3A34382E 363037 383039" },

  {   L_, 2020,   1,   1,    2,   19,  48,  608,   0,   1,
                                            "04 04 00800000"                 },
  {   L_, 2020,   1,   1,    2,   19,  48,  608, 901,   0,
          "1A 1A 32303230 2d30312d 30315430 323A3139 3A34382E 363038 393031" },

  {   L_, 2020,   1,   1,   23,   59,  59,  999,   0,   1,
                                            "04 04 05265BFF"                 },
  {   L_, 2020,   1,   1,   23,   59,  59,  999, 999,   0,
          "1A 1A 32303230 2d30312d 30315432 333A3539 3A35392E 393939 393939" },

  {   L_, 2020,   1,   2,    0,    0,   0,    0,   0,   1,
                                            "04 04 05265C00"                 },
  {   L_, 2020,   1,   2,    0,    0,   0,    0,   0,   0,
          "1A 1A 32303230 2d30312d 30325430 303A3030 3A30302E 303030 303030" },

  {   L_, 2020,   1,   2,    0,    0,   0,    1,   0,   1,
                                            "04 04 05265C01"                 },
  {   L_, 2020,   1,   2,    0,    0,   0,    1,   0,   0,
          "1A 1A 32303230 2d30312d 30325430 303A3030 3A30302E 303031 303030" },
  {   L_, 2020,   1,   2,    0,    0,   0,    0,   1,   0,
          "1A 1A 32303230 2d30312d 30325430 303A3030 3A30302E 303030 303031" },

  {   L_, 2020,   1,  25,   20,   31,  23,  647,   0,   1,
                                            "04 04 7FFFFFFF"                 },
  {   L_, 2020,   1,  25,   20,   31,  23,  647,   534,   0,
          "1A 1A 32303230 2d30312d 32355432 303A3331 3A32332E 363437 353334" },

  {   L_, 2020,   1,  25,   20,   31,  23,  648,   0,   1,
                                            "04 05 00800000 00"              },
  {   L_, 2020,   1,  25,   20,   31,  23,  648,   537,   0,
          "1A 1A 32303230 2d30312d 32355432 303A3331 3A32332E 363438 353337" },

  {   L_, 2020,   1,  25,   20,   31,  23,  649,   0,   1,
                                            "04 05 00800000 01"              },
  {   L_, 2020,   1,  25,   20,   31,  23,  649,   538,   0,
          "1A 1A 32303230 2d30312d 32355432 303A3331 3A32332E 363439 353338" },

  {   L_, 2020,   1,  31,   23,   59,  59,  999,   0,   1,
                                            "04 05 009FA523 FF"              },
  {   L_, 2020,   1,  31,   23,   59,  59,  999, 999,   0,
          "1A 1A 32303230 2d30312d 33315432 333A3539 3A35392E 393939 393939" },

  {   L_, 2020,   2,   1,    0,    0,   0,    0,   0,   1,
                                            "04 05 009FA524 00"              },
  {   L_, 2020,   2,   1,    0,    0,   0,    0,   0,   0,
          "1A 1A 32303230 2d30322d 30315430 303A3030 3A30302E 303030 303030" },

  {   L_, 2020,   2,   1,    0,    0,   0,    1,   0,   1,
                                            "04 05 009FA524 01"              },
  {   L_, 2020,   2,   1,    0,    0,   0,    1,   0,   0,
          "1A 1A 32303230 2d30322d 30315430 303A3030 3A30302E 303031 303030" },
  {   L_, 2020,   2,   1,    0,    0,   0,    0,   1,   0,
          "1A 1A 32303230 2d30322d 30315430 303A3030 3A30302E 303030 303031" },

  {   L_, 2020,   2,  28,   23,   59,  59,  999,   0,   1,
                                            "04 05 012FD733 FF"              },
  {   L_, 2020,   2,  28,   23,   59,  59,  999, 999,   0,
          "1A 1A 32303230 2d30322d 32385432 333A3539 3A35392E 393939 393939" },

  {   L_, 2020,   2,  29,    0,    0,   0,    0,   0,   1,
                                            "04 05 012FD734 00"              },
  {   L_, 2020,   2,  29,    0,    0,   0,    0,   0,   0,
          "1A 1A 32303230 2d30322d 32395430 303A3030 3A30302E 303030 303030" },

  {   L_, 2020,   2,  29,   23,   59,  59,  999,   0,   1,
                                            "04 05 0134FD8F FF"              },
  {   L_, 2020,   2,  29,   23,   59,  59,  999, 999,   0,
          "1A 1A 32303230 2d30322d 32395432 333A3539 3A35392E 393939 393939" },

  {   L_, 2020,   3,   1,    0,    0,   0,    0,   0,   1,
                                            "04 05 0134FD90 00"              },
  {   L_, 2020,   3,   1,    0,    0,   0,    0,   0,   0,
          "1A 1A 32303230 2d30332d 30315430 303A3030 3A30302E 303030 303030" },

  {   L_, 2020,  12,  31,   23,   59,  59,  999,   0,   1,
                                            "04 05 075CD787 FF"              },
  {   L_, 2020,  12,  31,   23,   59,  59,  999, 999,   0,
          "1A 1A 32303230 2d31322d 33315432 333A3539 3A35392E 393939 393939" },

  {   L_, 2021,   1,   1,    0,    0,   0,    0,   0,   1,
                                            "04 05 075CD788 00"              },
  {   L_, 2021,   1,   1,    0,    0,   0,    0,   0,   0,
          "1A 1A 32303231 2d30312d 30315430 303A3030 3A30302E 303030 303030" },

  {   L_, 2023,  12,  31,   23,   59,  59,  999,   0,   1,
                                            "04 05 1D63EB0B FF"              },
  {   L_, 2023,  12,  31,   23,   59,  59,  999, 999,   0,
          "1A 1A 32303233 2d31322d 33315432 333A3539 3A35392E 393939 393939" },

  {   L_, 2024,   1,   1,    0,    0,   0,    0,   0,   1,
                                            "04 05 1D63EB0C 00"              },
  {   L_, 2024,   1,   1,    0,    0,   0,    0,   0,   0,
          "1A 1A 32303234 2d30312d 30315430 303A3030 3A30302E 303030 303030" },

  {   L_, 2024,   1,   1,    0,    0,   0,    1,   0,   1,
                                            "04 05 1D63EB0C 01"              },
  {   L_, 2024,   1,   1,    0,    0,   0,    1,   0,   0,
          "1A 1A 32303234 2d30312d 30315430 303A3030 3A30302E 303031 303030" },
  {   L_, 2024,   1,   1,    0,    0,   0,    0,   1,   0,
          "1A 1A 32303234 2d30312d 30315430 303A3030 3A30302E 303030 303031" },

  {   L_, 2037,   6,   2,   21,   56,  53,  887,   0,   1,
                                            "04 05 7FFFFFFF FF"              },
  {   L_, 2037,   6,   2,   21,   56,  53,  888, 887,   0,
          "1A 1A 32303337 2d30362d 30325432 313A3536 3A35332E 383838 383837" },

  {   L_, 2037,   6,   2,   21,   56,  53,  888,   0,   1,
                                            "04 06 00800000 0000"            },
  {   L_, 2037,   6,   2,   21,   56,  53,  888, 888,   0,
          "1A 1A 32303337 2d30362d 30325432 313A3536 3A35332E 383838 383838" },

  {   L_, 2037,   6,   2,   21,   56,  53,  889,   0,   1,
                                            "04 06 00800000 0001"            },
  {   L_, 2037,   6,   2,   21,   56,  53,  888, 889,   0,
          "1A 1A 32303337 2d30362d 30325432 313A3536 3A35332E 383838 383839" },

  {   L_, 2099,  12,  31,   23,   59,  59,  999,   0,   1,
                                            "04 06 024BCE5C EFFF"            },
  {   L_, 2099,  12,  31,   23,   59,  59,  999, 999,   0,
          "1A 1A 32303939 2d31322d 33315432 333A3539 3A35392E 393939 393939" },

  {   L_, 2100,   1,   1,    0,    0,   0,    0,   0,   1,
                                            "04 06 024BCE5C F000"            },
  {   L_, 2100,   1,   1,    0,    0,   0,    0,   0,   0,
          "1A 1A 32313030 2d30312d 30315430 303A3030 3A30302E 303030 303030" },

  {   L_, 6479,  10,  17,    2,   45,  55,  327,   0,   1,
                                            "04 06 7FFFFFFF FFFF"            },
  {   L_, 6479,  10,  17,    2,   45,  55,  327, 216,   0,
          "1A 1A 36343739 2d31302d 31375430 323A3435 3A35352E 333237 323136" },

  {   L_, 6479,  10,  17,    2,   45,  55,  328,   0,   1,
                                            "04 09 00000080 00000000 00"     },
  {   L_, 6479,  10,  17,    2,   45,  55,  328, 217,   0,
          "1A 1A 36343739 2d31302d 31375430 323A3435 3A35352E 333238 323137" },

  {   L_, 6479,  10,  17,    2,   45,  55,  329,   0,   1,
                                            "04 09 00000080 00000000 01"     },
  {   L_, 6479,  10,  17,    2,   45,  55,  329, 218,   0,
          "1A 1A 36343739 2d31302d 31375430 323A3435 3A35352E 333239 323138" },

  {   L_, 2019,  12,  31,   23,   59,  59,  999,   0,   1,
                                            "04 01 FF"                       },
  {   L_, 2019,  12,  31,   23,   59,  59,  999, 999,   0,
          "1A 1A 32303139 2d31322d 33315432 333A3539 3A35392E 393939 393939" },

  {   L_, 2019,  12,  31,   23,   59,  59,  872,   0,   1,
                                            "04 01 80"                       },
  {   L_, 2019,  12,  31,   23,   59,  59,  872, 761,   0,
          "1A 1A 32303139 2d31322d 33315432 333A3539 3A35392E 383732 373631" },

  {   L_, 2019,  12,  31,   23,   59,  59,  871,   0,   1,
                                            "04 02 FF7F"                     },
  {   L_, 2019,  12,  31,   23,   59,  59,  871, 760,   0,
          "1A 1A 32303139 2d31322d 33315432 333A3539 3A35392E 383731 373630" },

  {   L_, 2019,  12,  31,   23,   59,  59,    1,   0,   1,
                                            "04 02 FC19"                     },
  {   L_, 2019,  12,  31,   23,   59,  59,    1,   0,   0,
          "1A 1A 32303139 2d31322d 33315432 333A3539 3A35392E 303031 303030" },
  {   L_, 2019,  12,  31,   23,   59,  59,    0,   1,   0,
          "1A 1A 32303139 2d31322d 33315432 333A3539 3A35392E 303030 303031" },

  {   L_, 2019,  12,  31,   23,   59,  59,    0,   0,   1,
                                            "04 02 FC18"                     },
  {   L_, 2019,  12,  31,   23,   59,  59,    0,   0,   0,
          "1A 1A 32303139 2d31322d 33315432 333A3539 3A35392E 303030 303030" },

  {   L_, 2019,  12,  31,   23,   59,  58,  999,   0,   1,
                                            "04 02 FC17"                     },
  {   L_, 2019,  12,  31,   23,   59,  58,  999, 999,   0,
          "1A 1A 32303139 2d31322d 33315432 333A3539 3A35382E 393939 393939" },

  {   L_, 2019,  12,  31,   23,   59,  27,  232,   0,   1,
                                            "04 02 8000"                     },
  {   L_, 2019,  12,  31,   23,   59,  27,  232, 343,   0,
          "1A 1A 32303139 2d31322d 33315432 333A3539 3A32372E 323332 333433" },

  {   L_, 2019,  12,  31,   23,   59,  27,  231,   0,   1,
                                            "04 03 FF7FFF"                   },
  {   L_, 2019,  12,  31,   23,   59,  27,  231, 342,   0,
          "1A 1A 32303139 2d31322d 33315432 333A3539 3A32372E 323331 333432" },

  {   L_, 2019,  12,  31,   23,   59,   0,    1,   0,   1,
                                            "04 03 FF15A1"                   },
  {   L_, 2019,  12,  31,   23,   59,   0,    1,   0,   0,
          "1A 1A 32303139 2d31322d 33315432 333A3539 3A30302E 303031 303030" },
  {   L_, 2019,  12,  31,   23,   59,   0,    0,   1,   0,
          "1A 1A 32303139 2d31322d 33315432 333A3539 3A30302E 303030 303031" },

  {   L_, 2019,  12,  31,   23,   59,   0,    0,   0,   1,
                                            "04 03 FF15A0"                   },
  {   L_, 2019,  12,  31,   23,   59,   0,    0,   0,   0,
          "1A 1A 32303139 2d31322d 33315432 333A3539 3A30302E 303030 303030" },

  {   L_, 2019,  12,  31,   23,   58,  59,  999,   0,   1,
                                            "04 03 FF159F"                   },
  {   L_, 2019,  12,  31,   23,   58,  59,  999, 999,   0,
          "1A 1A 32303139 2d31322d 33315432 333A3538 3A35392E 393939 393939" },

  {   L_, 2019,  12,  31,   23,    0,   0,    1,   0,   1,
                                            "04 03 C91181"                   },
  {   L_, 2019,  12,  31,   23,    0,   0,    1,   0,   0,
          "1A 1A 32303139 2d31322d 33315432 333A3030 3A30302E 303031 303030" },

  {   L_, 2019,  12,  31,   23,    0,   0,    0,   0,   1,
                                            "04 03 C91180"                   },
  {   L_, 2019,  12,  31,   23,    0,   0,    0,   0,   0,
          "1A 1A 32303139 2d31322d 33315432 333A3030 3A30302E 303030 303030" },

  {   L_, 2019,  12,  31,   22,   59,  59,  999,   0,   1,
                                            "04 03 C9117F"                   },
  {   L_, 2019,  12,  31,   22,   59,  59,  999, 999,   0,
          "1A 1A 32303139 2d31322d 33315432 323A3539 3A35392E 393939 393939" },

  {   L_, 2019,  12,  31,   21,   40,  11,  392,   0,   1,
                                            "04 03 800000"                   },
  {   L_, 2019,  12,  31,   21,   40,  11,  392, 281,   0,
          "1A 1A 32303139 2d31322d 33315432 313A3430 3A31312E 333932 323831" },

  {   L_, 2019,  12,  31,   21,   40,  11,  391,   0,   1,
                                            "04 04 FF7FFFFF"                 },
  {   L_, 2019,  12,  31,   21,   40,  11,  391, 280,   0,
          "1A 1A 32303139 2d31322d 33315432 313A3430 3A31312E 333931 323830" },

  {   L_, 2019,  12,  07,    3,   28,  36,  352,   0,   1,
                                            "04 04 80000000"                 },
  {   L_, 2019,  12,  07,    3,   28,  36,  352, 463,   0,
          "1A 1A 32303139 2d31322d 30375430 333A3238 3A33362E 333532 343633" },

  {   L_, 2019,  12,  07,    3,   28,  36,  351,   0,   1,
                                            "04 05 FF7FFFFF FF"              },
  {   L_, 2019,  12,  07,    3,   28,  36,  351, 462,   0,
          "1A 1A 32303139 2d31322d 30375430 333A3238 3A33362E 333531 343632" },

  {   L_, 2002,   7,  31,    2,    3,   6,  112,   0,   1,
                                            "04 05 80000000 00"              },
  {   L_, 2002,   7,  31,    2,    3,   6,  112, 223,   0,
          "1A 1A 32303032 2d30372d 33315430 323A3033 3A30362E 313132 323233" },

  {   L_, 2002,   7,  31,    2,    3,   6,  111,   0,   1,
                                            "04 06 FF7FFFFF FFFF"            },
  {   L_, 2002,   7,  31,    2,    3,   6,  111, 222,   0,
          "1A 1A 32303032 2d30372d 33315430 323A3033 3A30362E 313131 323232" },

  {   L_,    1,   1,   1,    0,    0,   0,    0,   0,   1,
                                            "04 06 C60D8F6C 4000"            },
  {   L_,    1,   1,   1,    0,    0,   0,    0,   0,   0,
          "1A 1A 30303031 2d30312d 30315430 303A3030 3A30302E 303030 303030" },

  {   L_, 9999,  12,  31,   23,   59,  59,  999,   0,   1,
                                            "04 09 000000E5 0873B8F3 FF"     },
  {   L_, 9999,  12,  31,   23,   59,  59,  999, 999,   0,
          "1A 1A 39393939 2d31322d 33315432 333A3539 3A35392E 393939 393939" },
  //------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_lineNum;
                const int   YEAR  = DATA[i].d_year;
                const int   MONTH = DATA[i].d_month;
                const int   DAY   = DATA[i].d_day;
                const int   HOUR  = DATA[i].d_hour;
                const int   MIN   = DATA[i].d_min;
                const int   SECS  = DATA[i].d_sec;
                const int   MSEC  = DATA[i].d_milliSec;
                const int   USEC  = DATA[i].d_microSec;
                const bool  BIN   = DATA[i].d_useBinary;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                LOOP_ASSERT(LINE, bdlt::Date::isValidYearMonthDay(YEAR,
                                                                  MONTH,
                                                                  DAY));

                if (veryVerbose) { P_(YEAR) P_(MONTH) P_(DAY)
                                   P_(HOUR) P_(MIN) P_(SECS)
                                   P(MSEC) P(USEC) P(EXP) }

                balber::BerEncoderOptions options;
                options.setEncodeDateAndTimeTypesAsBinary(BIN);
                options.setDatetimeFractionalSecondPrecision(6);

                const bdlt::Datetime VALUE(YEAR, MONTH, DAY,
                                           HOUR, MIN, SECS, MSEC, USEC);

                bdlsb::MemOutStreamBuf osb;
                balber::BerEncoder encoder(&options);
                ASSERT(0 == encoder.encode(&osb, VALUE));
                LOOP2_ASSERT(LEN, osb.length(), LEN == (int)osb.length());
                LOOP2_ASSERT(osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting DatetimeTz." << bsl::endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                int         d_hour;      // hour under test
                int         d_min;       // min under test
                int         d_sec;       // sec under test
                int         d_milliSec;  // milliSec under test
                int         d_microSec;  // microSec under test
                int         d_offset;    // timezone offset
                bool        d_useBinary; // whether to use binary format
                const char *d_exp;       // expected output
            } DATA[] = {
  //------------^
  //line  year  mon  day  hour   min  sec    ms    off    opt  exp
  //----  ----- ---  ---  ----   ---  ---    --    ---    ---  ---
  {   L_, 2020,   1,   1,    0,    0,   0,    0,    0,     0,     1,
                                            "04 01 00"                       },
  {   L_, 2020,   1,   1,    0,    0,   0,    0,    0,     0,     0,
   "1A 20"
   "32303230 2d30312d 30315430 303A3030 3A30302E 30303030 30302B30 303A3030" },

  {   L_, 2020,   1,   1,    0,    0,   0,    0,    0,  1439,     1,
                                            "04 07 059F0000 000000"          },
  {   L_, 2020,   1,   1,    0,    0,   0,    0,    0,  1439,     0,
   "1A 20"
   "32303230 2d30312d 30315430 303A3030 3A30302E 30303030 30302B32 333A3539" },

  {   L_, 2020,   1,   1,    0,    0,   0,    0,    0, -1439,     1,
                                            "04 07 FA610000 000000"          },
  {   L_, 2020,   1,   1,    0,    0,   0,    0,    0, -1439,     0,
   "1A 20"
   "32303230 2d30312d 30315430 303A3030 3A30302E 30303030 30302D32 333A3539" },

  {   L_, 2020,   1,   1,    0,    0,   0,  127,    0,     0,     1,
                                            "04 01 7F"                       },
  {   L_, 2020,   1,   1,    0,    0,   0,  127,  127,     0,     0,
   "1A 20"
   "32303230 2d30312d 30315430 303A3030 3A30302E 31323731 32372B30 303A3030" },

  {   L_, 2020,   1,   1,    0,    0,   0,  127,    0,  1439,     1,
                                            "04 07 059F0000 00007F"          },
  {   L_, 2020,   1,   1,    0,    0,   0,  127,  127,  1439,     0,
   "1A 20"
   "32303230 2d30312d 30315430 303A3030 3A30302E 31323731 32372B32 333A3539" },

  {   L_, 2020,   1,   1,    0,    0,   0,  127,    0, -1439,     1,
                                            "04 07 FA610000 00007F"          },
  {   L_, 2020,   1,   1,    0,    0,   0,  127,  127, -1439,     0,
   "1A 20"
   "32303230 2d30312d 30315430 303A3030 3A30302E 31323731 32372D32 333A3539" },

  {   L_, 2020,   1,   1,    0,    0,   0,  128,    0,     0,     1,
                                            "04 02 0080"                     },
  {   L_, 2020,   1,   1,    0,    0,   0,  128,  128,     0,     0,
   "1A 20"
   "32303230 2d30312d 30315430 303A3030 3A30302E 31323831 32382B30 303A3030" },

  {   L_, 2020,   1,   1,    0,    0,   0,  128,    0,  1439,     1,
                                            "04 07 059F0000 000080"          },
  {   L_, 2020,   1,   1,    0,    0,   0,  128,  128,  1439,     0,
   "1A 20"
   "32303230 2d30312d 30315430 303A3030 3A30302E 31323831 32382B32 333A3539" },

  {   L_, 2020,   1,   1,    0,    0,   0,  128,    0, -1439,     1,
                                            "04 07 FA610000 000080"          },
  {   L_, 2020,   1,   1,    0,    0,   0,  128,  128, -1439,     0,
   "1A 20"
   "32303230 2d30312d 30315430 303A3030 3A30302E 31323831 32382D32 333A3539" },

  {   L_, 2020,   1,   1,    0,    0,  32,  767,    0,     0,     1,
                                            "04 02 7FFF"                     },
  {   L_, 2020,   1,   1,    0,    0,  32,  767,  767,     0,     0,
   "1A 20"
   "32303230 2d30312d 30315430 303A3030 3A33322E 37363737 36372B30 303A3030" },

  {   L_, 2020,   1,   1,    0,    0,  32,  767,    0,  1439,     1,
                                            "04 07 059F0000 007FFF"          },
  {   L_, 2020,   1,   1,    0,    0,  32,  767,  767,  1439,     0,
   "1A 20"
   "32303230 2d30312d 30315430 303A3030 3A33322E 37363737 36372B32 333A3539" },

  {   L_, 2020,   1,   1,    0,    0,  32,  767,    0, -1439,     1,
                                            "04 07 FA610000 007FFF"          },

  {   L_, 2020,   1,   1,    0,    0,   0,  127,  127, -1439,     0,
   "1A 20"
   "32303230 2d30312d 30315430 303A3030 3A30302E 31323731 32372D32 333A3539" },

  {   L_, 2020,   1,   1,    0,    0,  32,  768,    0,     0,     1,
                                            "04 03 008000"                   },
  {   L_, 2020,   1,   1,    0,    0,  32,  768,  768,     0,     0,
   "1A 20"
   "32303230 2d30312d 30315430 303A3030 3A33322E 37363837 36382B30 303A3030" },

  {   L_, 2020,   1,   1,    0,    0,  32,  768,    0,  1439,     1,
                                            "04 07 059F0000 008000"          },
  {   L_, 2020,   1,   1,    0,    0,  32,  768,  768,  1439,     0,
   "1A 20"
   "32303230 2d30312d 30315430 303A3030 3A33322E 37363837 36382B32 333A3539" },

  {   L_, 2020,   1,   1,    0,    0,  32,  768,    0, -1439,     1,
                                            "04 07 FA610000 008000"          },
  {   L_, 2020,   1,   1,    0,    0,  32,  768,  768, -1439,     0,
   "1A 20"
   "32303230 2d30312d 30315430 303A3030 3A33322E 37363837 36382D32 333A3539" },

  {   L_, 2020,   1,   1,    2,   19,  48,  607,    0,     0,     1,
                                            "04 03 7FFFFF"                   },
  {   L_, 2020,   1,   1,    2,   19,  48,  607,  607,     0,     0,
   "1A 20"
   "32303230 2d30312d 30315430 323A3139 3A34382E 36303736 30372B30 303A3030" },

  {   L_, 2020,   1,   1,    2,   19,  48,  607,    0,  1439,     1,
                                            "04 07 059F0000 7FFFFF"          },
  {   L_, 2020,   1,   1,    2,   19,  48,  607,  607,  1439,     0,
   "1A 20"
   "32303230 2d30312d 30315430 323A3139 3A34382E 36303736 30372B32 333A3539" },

  {   L_, 2020,   1,   1,    2,   19,  48,  607,    0, -1439,     1,
                                            "04 07 FA610000 7FFFFF"          },
  {   L_, 2020,   1,   1,    2,   19,  48,  607,  607, -1439,     0,
   "1A 20"
   "32303230 2d30312d 30315430 323A3139 3A34382E 36303736 30372D32 333A3539" },

  {   L_, 2020,   1,   1,    2,   19,  48,  608,    0,     0,     1,
                                            "04 04 00800000"                 },
  {   L_, 2020,   1,   1,    2,   19,  48,  608,  608,     0,     0,
   "1A 20"
   "32303230 2d30312d 30315430 323A3139 3A34382E 36303836 30382B30 303A3030" },

  {   L_, 2020,   1,   1,    2,   19,  48,  608,    0,  1439,     1,
                                            "04 07 059F0000 800000"          },
  {   L_, 2020,   1,   1,    2,   19,  48,  608,  608,  1439,     0,
   "1A 20"
   "32303230 2d30312d 30315430 323A3139 3A34382E 36303836 30382B32 333A3539" },

  {   L_, 2020,   1,   1,    2,   19,  48,  608,    0, -1439,     1,
                                            "04 07 FA610000 800000"          },
  {   L_, 2020,   1,   1,    2,   19,  48,  608,  608, -1439,     0,
   "1A 20"
   "32303230 2d30312d 30315430 323A3139 3A34382E 36303836 30382D32 333A3539" },

  {   L_, 2020,   1,  25,   20,   31,  23,  647,    0,     0,     1,
                                            "04 04 7FFFFFFF"                 },
  {   L_, 2020,   1,  25,   20,   31,  23,  647,  647,     0,     0,
   "1A 20"
   "32303230 2d30312d 32355432 303A3331 3A32332E 36343736 34372B30 303A3030" },

  {   L_, 2020,   1,  25,   20,   31,  23,  647,    0,  1439,     1,
                                            "04 07 059F007F FFFFFF"          },
  {   L_, 2020,   1,  25,   20,   31,  23,  647,  647,  1439,     0,
   "1A 20"
   "32303230 2d30312d 32355432 303A3331 3A32332E 36343736 34372B32 333A3539" },

  {   L_, 2020,   1,  25,   20,   31,  23,  647,    0, -1439,     1,
                                            "04 07 FA61007F FFFFFF"          },
  {   L_, 2020,   1,  25,   20,   31,  23,  647,  647, -1439,     0,
   "1A 20"
   "32303230 2d30312d 32355432 303A3331 3A32332E 36343736 34372D32 333A3539" },

  {   L_, 2020,   1,  25,   20,   31,  23,  648,    0,     0,     1,
                                            "04 05 00800000 00"              },
  {   L_, 2020,   1,  25,   20,   31,  23,  648,  648,     0,     0,
   "1A 20"
   "32303230 2d30312d 32355432 303A3331 3A32332E 36343836 34382B30 303A3030" },

  {   L_, 2020,   1,  25,   20,   31,  23,  648,    0,  1439,     1,
                                            "04 07 059F0080 000000"          },
  {   L_, 2020,   1,  25,   20,   31,  23,  648,  648,  1439,     0,
   "1A 20"
   "32303230 2d30312d 32355432 303A3331 3A32332E 36343836 34382B32 333A3539" },

  {   L_, 2020,   1,  25,   20,   31,  23,  648,    0, -1439,     1,
                                            "04 07 FA610080 000000"          },
  {   L_, 2020,   1,  25,   20,   31,  23,  648,  648, -1439,     0,
   "1A 20"
   "32303230 2d30312d 32355432 303A3331 3A32332E 36343836 34382D32 333A3539" },

  {   L_, 2037,   6,   2,   21,   56,  53,  887,    0,     0,     1,
                                            "04 05 7FFFFFFF FF"              },
  {   L_, 2037,   6,   2,   21,   56,  53,  888,  887,     0,     0,
   "1A 20"
   "32303337 2d30362d 30325432 313A3536 3A35332E 38383838 38372B30 303A3030" },

  {   L_, 2037,   6,   2,   21,   56,  53,  887,    0,  1439,     1,
                                            "04 07 059F7FFF FFFFFF"          },
  {   L_, 2037,   6,   2,   21,   56,  53,  888,  887,  1439,     0,
   "1A 20"
   "32303337 2d30362d 30325432 313A3536 3A35332E 38383838 38372B32 333A3539" },

  {   L_, 2037,   6,   2,   21,   56,  53,  887,    0, -1439,     1,
                                            "04 07 FA617FFF FFFFFF"          },
  {   L_, 2037,   6,   2,   21,   56,  53,  888,  887, -1439,     0,
   "1A 20"
   "32303337 2d30362d 30325432 313A3536 3A35332E 38383838 38372D32 333A3539" },

  {   L_, 2037,   6,   2,   21,   56,  53,  888,    0,     0,     1,
                                            "04 06 00800000 0000"            },
  {   L_, 2037,   6,   2,   21,   56,  53,  888,  888,     0,     0,
   "1A 20"
   "32303337 2d30362d 30325432 313A3536 3A35332E 38383838 38382B30 303A3030" },

  {   L_, 2037,   6,   2,   21,   56,  53,  888,    0,  1439,     1,
                                            "04 08 059F0080 00000000"        },
  {   L_, 2037,   6,   2,   21,   56,  53,  888,  888,  1439,     0,
   "1A 20"
   "32303337 2d30362d 30325432 313A3536 3A35332E 38383838 38382B32 333A3539" },

  {   L_, 2037,   6,   2,   21,   56,  53,  888,    0, -1439,     1,
                                            "04 08 FA610080 00000000"        },
  {   L_, 2037,   6,   2,   21,   56,  53,  888,  888, -1439,     0,
   "1A 20"
   "32303337 2d30362d 30325432 313A3536 3A35332E 38383838 38382D32 333A3539" },

  {   L_, 6479,  10,  17,    2,   45,  55,  327,    0,     0,     1,
                                            "04 06 7FFFFFFF FFFF"            },
  {   L_, 6479,  10,  17,    2,   45,  55,  327,  327,     0,     0,
   "1A 20"
   "36343739 2d31302d 31375430 323A3435 3A35352E 33323733 32372B30 303A3030" },

  {   L_, 6479,  10,  17,    2,   45,  55,  327,    0,  1439,     1,
                                            "04 08 059F7FFF FFFFFFFF"        },
  {   L_, 6479,  10,  17,    2,   45,  55,  327,  327,  1439,     0,
   "1A 20"
   "36343739 2d31302d 31375430 323A3435 3A35352E 33323733 32372B32 333A3539" },

  {   L_, 6479,  10,  17,    2,   45,  55,  327,    0, -1439,     1,
                                            "04 08 FA617FFF FFFFFFFF"        },
  {   L_, 6479,  10,  17,    2,   45,  55,  327,  327, -1439,     0,
   "1A 20"
   "36343739 2d31302d 31375430 323A3435 3A35352E 33323733 32372D32 333A3539" },

  {   L_, 6479,  10,  17,    2,   45,  55,  328,    0,     0,     1,
                                            "04 09 00000080 00000000 00 "    },
  {   L_, 6479,  10,  17,    2,   45,  55,  328,  328,     0,     0,
   "1A 20"
   "36343739 2d31302d 31375430 323A3435 3A35352E 33323833 32382B30 303A3030" },

  {   L_, 6479,  10,  17,    2,   45,  55,  328,    0,  1439,     1,
                                            "04 09 059F0080 00000000 00"     },
  {   L_, 6479,  10,  17,    2,   45,  55,  328,  328,  1439,     0,
   "1A 20"
   "36343739 2d31302d 31375430 323A3435 3A35352E 33323833 32382B32 333A3539" },

  {   L_, 6479,  10,  17,    2,   45,  55,  328,    0, -1439,     1,
                                            "04 09 FA610080 00000000 00"     },
  {   L_, 6479,  10,  17,    2,   45,  55,  328,  328, -1439,     0,
   "1A 20"
   "36343739 2d31302d 31375430 323A3435 3A35352E 33323833 32382D32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  59,  999,    0,     0,     1,
                                            "04 01 FF"                       },
  {   L_, 2019,  12,  31,   23,   59,  59,  999,  999,     0,     0,
   "1A 20"
   "32303139 2d31322d 33315432 333A3539 3A35392E 39393939 39392B30 303A3030" },

  {   L_, 2019,  12,  31,   23,   59,  59,  999,    0,  1439,     1,
                                            "04 07 059FFFFF FFFFFF"          },
  {   L_, 2019,  12,  31,   23,   59,  59,  999,  999,  1439,     0,
   "1A 20"
   "32303139 2d31322d 33315432 333A3539 3A35392E 39393939 39392B32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  59,  999,    0, -1439,     1,
                                            "04 07 FA61FFFF FFFFFF"          },
  {   L_, 2019,  12,  31,   23,   59,  59,  999,  999, -1439,     0,
   "1A 20"
   "32303139 2d31322d 33315432 333A3539 3A35392E 39393939 39392D32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  59,  872,    0,     0,     1,
                                            "04 01 80"                       },
  {   L_, 2019,  12,  31,   23,   59,  59,  872,  872,     0,     0,
   "1A 20"
   "32303139 2d31322d 33315432 333A3539 3A35392E 38373238 37322B30 303A3030" },

  {   L_, 2019,  12,  31,   23,   59,  59,  872,    0,  1439,     1,
                                            "04 07 059FFFFF FFFF80"          },
  {   L_, 2019,  12,  31,   23,   59,  59,  872,  872,  1439,     0,
   "1A 20"
   "32303139 2d31322d 33315432 333A3539 3A35392E 38373238 37322B32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  59,  872,    0, -1439,     1,
                                            "04 07 FA61FFFF FFFF80"          },
  {   L_, 2019,  12,  31,   23,   59,  59,  872,  872, -1439,     0,
   "1A 20"
   "32303139 2d31322d 33315432 333A3539 3A35392E 38373238 37322D32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  59,  871,    0,     0,     1,
                                            "04 02 FF7F"                     },
  {   L_, 2019,  12,  31,   23,   59,  59,  871,  871,     0,     0,
   "1A 20"
   "32303139 2d31322d 33315432 333A3539 3A35392E 38373138 37312B30 303A3030" },

  {   L_, 2019,  12,  31,   23,   59,  59,  871,    0,  1439,     1,
                                            "04 07 059FFFFF FFFF7F"          },
  {   L_, 2019,  12,  31,   23,   59,  59,  871,  871,  1439,     0,
   "1A 20"
   "32303139 2d31322d 33315432 333A3539 3A35392E 38373138 37312B32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  59,  871,    0, -1439,     1,
                                            "04 07 FA61FFFF FFFF7F"          },
  {   L_, 2019,  12,  31,   23,   59,  59,  871,  871, -1439,     0,
   "1A 20"
   "32303139 2d31322d 33315432 333A3539 3A35392E 38373138 37312D32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  27,  232,    0,     0,     1,
                                            "04 02 8000"                     },
  {   L_, 2019,  12,  31,   23,   59,  27,  232,  232,     0,     0,
   "1A 20"
   "32303139 2d31322d 33315432 333A3539 3A32372E 32333232 33322B30 303A3030" },

  {   L_, 2019,  12,  31,   23,   59,  27,  232,    0,  1439,     1,
                                            "04 07 059FFFFF FF8000"          },
  {   L_, 2019,  12,  31,   23,   59,  27,  232,  232,  1439,     0,
   "1A 20"
   "32303139 2d31322d 33315432 333A3539 3A32372E 32333232 33322B32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  27,  232,    0, -1439,     1,
                                            "04 07 FA61FFFF FF8000"          },
  {   L_, 2019,  12,  31,   23,   59,  27,  232,  232, -1439,     0,
   "1A 20"
   "32303139 2d31322d 33315432 333A3539 3A32372E 32333232 33322D32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  27,  231,    0,     0,     1,
                                            "04 03 FF7FFF"                   },
  {   L_, 2019,  12,  31,   23,   59,  27,  231,  231,     0,     0,
   "1A 20"
   "32303139 2d31322d 33315432 333A3539 3A32372E 32333132 33312B30 303A3030" },

  {   L_, 2019,  12,  31,   23,   59,  27,  231,    0,  1439,     1,
                                            "04 07 059FFFFF FF7FFF"          },
  {   L_, 2019,  12,  31,   23,   59,  27,  231,  231,  1439,     0,
   "1A 20"
   "32303139 2d31322d 33315432 333A3539 3A32372E 32333132 33312B32 333A3539" },

  {   L_, 2019,  12,  31,   23,   59,  27,  231,    0, -1439,     1,
                                            "04 07 FA61FFFF FF7FFF"          },
  {   L_, 2019,  12,  31,   23,   59,  27,  231,  231, -1439,     0,
   "1A 20"
   "32303139 2d31322d 33315432 333A3539 3A32372E 32333132 33312D32 333A3539" },

  {   L_, 2019,  12,  31,   21,   40,  11,  392,    0,     0,     1,
                                            "04 03 800000"                   },
  {   L_, 2019,  12,  31,   21,   40,  11,  392,  392,     0,     0,
   "1A 20"
   "32303139 2d31322d 33315432 313A3430 3A31312E 33393233 39322B30 303A3030" },

  {   L_, 2019,  12,  31,   21,   40,  11,  392,    0,  1439,     1,
                                            "04 07 059FFFFF 800000"          },
  {   L_, 2019,  12,  31,   21,   40,  11,  392,  392,  1439,     0,
   "1A 20"
   "32303139 2d31322d 33315432 313A3430 3A31312E 33393233 39322B32 333A3539" },

  {   L_, 2019,  12,  31,   21,   40,  11,  392,    0, -1439,     1,
                                            "04 07 FA61FFFF 800000"          },
  {   L_, 2019,  12,  31,   21,   40,  11,  392,  392, -1439,     0,
   "1A 20"
   "32303139 2d31322d 33315432 313A3430 3A31312E 33393233 39322D32 333A3539" },

  {   L_, 2019,  12,  31,   21,   40,  11,  391,    0,     0,     1,
                                            "04 04 FF7FFFFF"                 },
  {   L_, 2019,  12,  31,   21,   40,  11,  391,  391,     0,     0,
   "1A 20"
   "32303139 2d31322d 33315432 313A3430 3A31312E 33393133 39312B30 303A3030" },

  {   L_, 2019,  12,  31,   21,   40,  11,  391,    0,  1439,     1,
                                            "04 07 059FFFFF 7FFFFF"          },
  {   L_, 2019,  12,  31,   21,   40,  11,  391,  391,  1439,     0,
   "1A 20"
   "32303139 2d31322d 33315432 313A3430 3A31312E 33393133 39312B32 333A3539" },

  {   L_, 2019,  12,  31,   21,   40,  11,  391,    0, -1439,     1,
                                            "04 07 FA61FFFF 7FFFFF"          },
  {   L_, 2019,  12,  31,   21,   40,  11,  391,  391, -1439,     0,
   "1A 20"
   "32303139 2d31322d 33315432 313A3430 3A31312E 33393133 39312D32 333A3539" },

  {   L_, 2019,  12,  07,    3,   28,  36,  352,    0,     0,     1,
                                            "04 04 80000000"                 },
  {   L_, 2019,  12,  07,    3,   28,  36,  352,  352,     0,     0,
   "1A 20"
   "32303139 2d31322d 30375430 333A3238 3A33362E 33353233 35322B30 303A3030" },

  {   L_, 2019,  12,  07,    3,   28,  36,  352,    0,  1439,     1,
                                            "04 07 059FFF80 000000"          },
  {   L_, 2019,  12,  07,    3,   28,  36,  352,  352,  1439,     0,
   "1A 20"
   "32303139 2d31322d 30375430 333A3238 3A33362E 33353233 35322B32 333A3539" },

  {   L_, 2019,  12,  07,    3,   28,  36,  352,    0, -1439,     1,
                                            "04 07 FA61FF80 000000"          },
  {   L_, 2019,  12,  07,    3,   28,  36,  352,  352, -1439,     0,
   "1A 20"
   "32303139 2d31322d 30375430 333A3238 3A33362E 33353233 35322D32 333A3539" },

  {   L_, 2019,  12,  07,    3,   28,  36,  351,    0,     0,     1,
                                            "04 05 FF7FFFFF FF"              },
  {   L_, 2019,  12,  07,    3,   28,  36,  351,  351,     0,     0,
   "1A 20"
   "32303139 2d31322d 30375430 333A3238 3A33362E 33353133 35312B30 303A3030" },

  {   L_, 2019,  12,  07,    3,   28,  36,  351,    0,  1439,     1,
                                            "04 07 059FFF7F FFFFFF"          },
  {   L_, 2019,  12,  07,    3,   28,  36,  351,  351,  1439,     0,
   "1A 20"
   "32303139 2d31322d 30375430 333A3238 3A33362E 33353133 35312B32 333A3539" },

  {   L_, 2019,  12,  07,    3,   28,  36,  351,    0, -1439,     1,
                                            "04 07 FA61FF7F FFFFFF"          },
  {   L_, 2019,  12,  07,    3,   28,  36,  351,  351, -1439,     0,
   "1A 20"
   "32303139 2d31322d 30375430 333A3238 3A33362E 33353133 35312D32 333A3539" },

  {   L_, 2002,   7,  31,    2,    3,   6,  112,    0,     0,     1,
                                            "04 05 80000000 00"              },
  {   L_, 2002,   7,  31,    2,    3,   6,  112,  112,     0,     0,
   "1A 20"
   "32303032 2d30372d 33315430 323A3033 3A30362E 31313231 31322B30 303A3030" },

  {   L_, 2002,   7,  31,    2,    3,   6,  112,    0,  1439,     1,
                                            "04 07 059F8000 000000"          },
  {   L_, 2002,   7,  31,    2,    3,   6,  112,  112,  1439,     0,
   "1A 20"
   "32303032 2d30372d 33315430 323A3033 3A30362E 31313231 31322B32 333A3539" },

  {   L_, 2002,   7,  31,    2,    3,   6,  112,    0, -1439,     1,
                                            "04 07 FA618000 000000"          },
  {   L_, 2002,   7,  31,    2,    3,   6,  112,  112, -1439,     0,
   "1A 20"
   "32303032 2d30372d 33315430 323A3033 3A30362E 31313231 31322D32 333A3539" },

  {   L_, 2002,   7,  31,    2,    3,   6,  111,    0,     0,     1,
                                            "04 06 FF7FFFFF FFFF"            },
  {   L_, 2002,   7,  31,    2,    3,   6,  111,  111,     0,     0,
   "1A 20"
   "32303032 2d30372d 33315430 323A3033 3A30362E 31313131 31312B30 303A3030" },

  {   L_, 2002,   7,  31,    2,    3,   6,  111,    0,  1439,     1,
                                            "04 08 059FFF7F FFFFFFFF"        },
  {   L_, 2002,   7,  31,    2,    3,   6,  111,  111,  1439,     0,
   "1A 20"
   "32303032 2d30372d 33315430 323A3033 3A30362E 31313131 31312B32 333A3539" },

  {   L_, 2002,   7,  31,    2,    3,   6,  111,    0, -1439,     1,
                                            "04 08 FA61FF7F FFFFFFFF"        },
  {   L_, 2002,   7,  31,    2,    3,   6,  111,  111, -1439,     0,
   "1A 20"
   "32303032 2d30372d 33315430 323A3033 3A30362E 31313131 31312D32 333A3539" },

  {   L_,    1,   1,   1,    0,    0,   0,    0,    0,     0,     1,
                                            "04 06 C60D8F6C 4000"            },
  {   L_,    1,   1,   1,    0,    0,   0,    0,    0,     0,     0,
   "1A 20"
   "30303031 2d30312d 30315430 303A3030 3A30302E 30303030 30302B30 303A3030" },

  {   L_,    1,   1,   1,    0,    0,   0,    0,    0,  1439,     1,
                                            "04 08 059FC60D 8F6C4000"        },
  {   L_,    1,   1,   1,    0,    0,   0,    0,    0,  1439,     0,
   "1A 20"
   "30303031 2d30312d 30315430 303A3030 3A30302E 30303030 30302B32 333A3539" },

  {   L_,    1,   1,   1,    0,    0,   0,    0,    0, -1439,     1,
                                            "04 08 FA61C60D 8F6C4000"        },
  {   L_,    1,   1,   1,    0,    0,   0,    0,    0, -1439,     0,
   "1A 20"
   "30303031 2d30312d 30315430 303A3030 3A30302E 30303030 30302D32 333A3539" },

  {   L_, 9999,  12,  31,   23,   59,  59,  999,    0,     0,     1,
                                            "04 09 000000E5 0873B8F3 FF"     },
  {   L_, 9999,  12,  31,   23,   59,  59,  999,  999,     0,     0,
   "1A 20"
   "39393939 2d31322d 33315432 333A3539 3A35392E 39393939 39392B30 303A3030" },

  {   L_, 9999,  12,  31,   23,   59,  59,  999,    0,  1439,     1,
                                            "04 09 059F00E5 0873B8F3 FF"     },
  {   L_, 9999,  12,  31,   23,   59,  59,  999,  999,  1439,     0,
   "1A 20"
   "39393939 2d31322d 33315432 333A3539 3A35392E 39393939 39392B32 333A3539" },

  {   L_, 9999,  12,  31,   23,   59,  59,  999,    0, -1439,     1,
                                            "04 09 FA6100E5 0873B8F3 FF"     },
  {   L_, 9999,  12,  31,   23,   59,  59,  999,  999, -1439,     0,
   "1A 20"
   "39393939 2d31322d 33315432 333A3539 3A35392E 39393939 39392D32 333A3539" },
  //------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE  = DATA[i].d_lineNum;
                const int   YEAR  = DATA[i].d_year;
                const int   MONTH = DATA[i].d_month;
                const int   DAY   = DATA[i].d_day;
                const int   HOUR  = DATA[i].d_hour;
                const int   MIN   = DATA[i].d_min;
                const int   SECS  = DATA[i].d_sec;
                const int   MSEC  = DATA[i].d_milliSec;
                const int   USEC  = DATA[i].d_microSec;
                const int   OFF   = DATA[i].d_offset;
                const bool  BIN   = DATA[i].d_useBinary;
                const char *EXP   = DATA[i].d_exp;
                const int   LEN   = numOctets(EXP);

                LOOP_ASSERT(LINE, bdlt::Date::isValidYearMonthDay(YEAR,
                                                                  MONTH,
                                                                  DAY));

                if (veryVerbose) { P_(YEAR) P_(MONTH) P_(DAY) P_(OFF) P(BIN)
                                   P_(HOUR) P_(MIN) P_(SECS) P(MSEC)
                                   P(USEC) P(EXP) }

                balber::BerEncoderOptions options;
                options.setEncodeDateAndTimeTypesAsBinary(BIN);
                options.setDatetimeFractionalSecondPrecision(6);

                const bdlt::DatetimeTz VALUE(bdlt::Datetime(YEAR, MONTH, DAY,
                                                            HOUR, MIN, SECS,
                                                            MSEC, USEC),
                                             OFF);

                bdlsb::MemOutStreamBuf osb;
                balber::BerEncoder encoder(&options);
                ASSERT(0 == encoder.encode(&osb, VALUE));
                LOOP2_ASSERT(LEN, osb.length(), LEN == (int)osb.length());
                LOOP2_ASSERT(osb.data(), EXP,
                             0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    cout << "Output Buffer:";
                    printBuffer(osb.data(), osb.length());
                }
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING ARRAYS WITH the 'encodeEmptyArrays' option (DRQS 29114951)
        //
        // Concerns:
        //: 1 If 'balber::BerEncoderOptions' is not specified then empty arrays
        //:   are encoded.
        //:
        //: 2 If 'balber::BerEncoderOptions' is specified but the
        //:   'encodeEmptyArrays' is set to 'false' then empty arrays
        //:   are not encoded.
        //:
        //: 3 If 'balber::BerEncoderOptions' is specified and the
        //:   'encodeEmptyArrays' option is not set or set to 'true' then
        //:   empty arrays are encoded.
        //:
        //: 4 Non-empty arrays are always encoded.
        //
        // Plan:
        //: 1 Create three 'balber::BerEncoderOptions' objects.  Set the
        //:   'encodeEmptyArrays' option in one encoder options object to
        //:   'true' and to 'false' in the another object.  Leave the third
        //:   encoder options object unmodified.
        //:
        //: 2 Create four 'balber::BerEncoder' objects passing the three
        //:   'balber::BerEncoderOptions' objects created in step 1 to the
        //:   first three encoder objects.  The fourth encoder object is not
        //:   passed any encoder options.
        //:
        //: 3 Create four 'bdlsb::MemOutStreamBuf' objects.
        //:
        //: 4 Populate a 'MySequenceWithArray' object ensuring that its
        //:   underlying vector data member is empty.
        //:
        //: 5 Encode the 'MySequenceWithArray' object onto a
        //:   'bdlsb::MemOutStreamBuf' using one of the created
        //:   'balber::BerEncoder' objects.
        //:
        //: 6 Ensure that the empty vector is encoded in all cases except when
        //:   the encoder options are explicitly provided and the
        //:   'encodeEmptyArrays' option on that object is set to 'false'.
        //:
        //: 7 Repeat steps 1 - 6 for a 'MySequenceWithArray' object that has a
        //:   non-empty vector.
        //:
        //: 8 Ensure that the non-empty vector is encoded in all cases.
        //
        // Testing:
        //  Encoding of vectors
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Arrays with 'encodeEmptyArrays'"
                               << "\n======================================="
                               << bsl::endl;

        if (verbose) bsl::cout << "\nTesting with empty array." << bsl::endl;
        {
            balber::BerEncoderOptions options1, options2, options3;
            options1.setEncodeEmptyArrays(true);
            options2.setEncodeEmptyArrays(false);

            balber::BerEncoder encoder1(&options1), encoder2(&options2),
                            encoder3(&options3), encoder4;

            bdlsb::MemOutStreamBuf osb1, osb2, osb3, osb4;

            test::MySequenceWithArray value;
            value.attribute1() = 34;

            ASSERT(0 == encoder1.encode(&osb1, value));
            ASSERT(0 == encoder2.encode(&osb2, value));
            ASSERT(0 == encoder3.encode(&osb3, value));
            ASSERT(0 == encoder4.encode(&osb4, value));

            ASSERT(osb1.length()  > osb2.length());
            ASSERT(osb1.length() == osb3.length());
            ASSERT(osb1.length() == osb4.length());
            ASSERT(0 == memcmp(osb1.data(), osb3.data(), osb1.length()));
            ASSERT(0 == memcmp(osb1.data(), osb4.data(), osb1.length()));

            printDiagnostic(encoder1);
            printDiagnostic(encoder2);
            printDiagnostic(encoder3);
            printDiagnostic(encoder4);

            if (veryVerbose) {
                P(osb1.length())
                printBuffer(osb1.data(), osb1.length());

                P(osb2.length())
                printBuffer(osb2.data(), osb2.length());

                P(osb3.length())
                printBuffer(osb3.data(), osb3.length());

                P(osb4.length())
                printBuffer(osb4.data(), osb4.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting with a non-empty array."
                               << bsl::endl;
        {
            balber::BerEncoderOptions options1, options2, options3;
            options1.setEncodeEmptyArrays(true);
            options2.setEncodeEmptyArrays(false);

            balber::BerEncoder encoder1(&options1), encoder2(&options2),
                            encoder3(&options3), encoder4;

            bdlsb::MemOutStreamBuf osb1, osb2, osb3, osb4;

            test::MySequenceWithArray value;
            value.attribute1() = 34;
            value.attribute2().push_back("Hello");
            value.attribute2().push_back("World!");

            ASSERT(0 == encoder1.encode(&osb1, value));
            ASSERT(0 == encoder2.encode(&osb2, value));
            ASSERT(0 == encoder3.encode(&osb3, value));
            ASSERT(0 == encoder4.encode(&osb4, value));

            ASSERT(osb1.length() == osb2.length())
            ASSERT(osb1.length() == osb3.length())
            ASSERT(osb1.length() == osb4.length())
            ASSERT(0 == memcmp(osb1.data(), osb2.data(), osb1.length()));
            ASSERT(0 == memcmp(osb1.data(), osb3.data(), osb1.length()));
            ASSERT(0 == memcmp(osb1.data(), osb4.data(), osb1.length()));

            printDiagnostic(encoder1);
            printDiagnostic(encoder2);
            printDiagnostic(encoder3);
            printDiagnostic(encoder4);

            if (veryVerbose) {
                P(osb1.length())
                printBuffer(osb1.data(), osb1.length());

                P(osb2.length())
                printBuffer(osb2.data(), osb2.length());

                P(osb3.length())
                printBuffer(osb3.data(), osb3.length());

                P(osb4.length())
                printBuffer(osb4.data(), osb4.length());
            }
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING NILLABLE VALUES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Nillable Values"
                               << "\n=======================" << bsl::endl;

        if (verbose) bsl::cout << "\nTesting with null value." << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithNillable value;
            value.attribute1() = 34;
            value.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting with non-null value."
                               << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithNillable value;
            value.attribute1() = 34;
            value.myNillable() = "World!";
            value.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING ANONYMOUS CHOICES
        //
        // Concerns:
        //: 1 If 'balber::BerEncoderOptions' is not specified then the encoder
        //:   will successfully encode an input with an unselected choice
        //
        //: 2 If 'balber::BerEncoderOptions' is specified but the
        //:   'disableUnselectedChoiceEncoding' is set to 'false' then
        //:   the encoder will successfully encode an input with an unselected
        //:   choice
        //:
        //: 3 If 'balber::BerEncoderOptions' is specified but the
        //:   'disableUnselectedChoiceEncoding' is set to 'true' then
        //:   the encoder will fail to encode an input with an unselected
        //:   choice
        //
        // Plan:
        //: 1 Create two 'balber::BerEncoderOptions' objects.  Set the
        //:   'disableUnselectedChoiceEncoding' option in one encoder options
        //:   object to 'true' and to 'false' in the another object.
        //:
        //: 2 Create three 'balber::BerEncoder' objects passing the two
        //:   'balber::BerEncoderOptions' objects created in step 1 to the
        //:   first two encoder objects.  The third encoder object is not
        //:   passed any encoder options.
        //:
        //: 3 Populate two 'MyChoice' objects.  Ensuring that the first
        //:   defaults to an unselected choice.  The second populated with
        //:   a choice value
        //:
        //: 4 Encode the two 'MyChoice' objects onto a
        //:   'bdlsb::MemOutStreamBuf' using each of the created
        //:   'balber::BerEncoder' objects.
        //:
        //: 5 Ensure that the encoder fails to encode the object when
        //:   'disableUnselectedChoiceEncoding' option is set and the
        //:   objects choice is unselected.  Otherwise the encoding
        //:   should be successful.
        //:
        //
        // Testing:
        //  Encoding of anonymous choices
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Anonymous Choice"
                               << "\n========================" << bsl::endl;

        if (verbose) bsl::cout << "\nTesting with no selection." << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithAnonymousChoice value;
            value.attribute1() = 34;
            ASSERT(value.choice().isUndefinedValue());
            value.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }
        {
            balber::BerEncoderOptions options;
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithAnonymousChoice value;
            value.attribute1() = 34;
            ASSERT(value.choice().isUndefinedValue());
            value.attribute2() = "Hello";

            balber::BerEncoder encoder(&options);
            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }
        {
            balber::BerEncoderOptions options;
            options.setDisableUnselectedChoiceEncoding(true);

            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithAnonymousChoice value;
            value.attribute1() = 34;
            ASSERT(value.choice().isUndefinedValue());
            value.attribute2() = "Hello";

            balber::BerEncoder encoderDisableUnselectedChoices(&options);
            ASSERT(0 != encoderDisableUnselectedChoices.encode(&osb, value));
            printDiagnostic(encoderDisableUnselectedChoices);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting with selection." << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithAnonymousChoice value;
            value.attribute1() = 34;
            value.choice().makeMyChoice1(58);
            value.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithAnonymousChoice value;
            value.attribute1() = 34;
            value.choice().makeMyChoice2("World!");
            value.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }
        {
            balber::BerEncoderOptions options;

            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithAnonymousChoice value;
            value.attribute1() = 34;
            value.choice().makeMyChoice1(58);
            value.attribute2() = "Hello";

            balber::BerEncoder encoder(&options);
            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }
        {
            balber::BerEncoderOptions options;

            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithAnonymousChoice value;
            value.attribute1() = 34;
            value.choice().makeMyChoice2("World!");
            value.attribute2() = "Hello";

            balber::BerEncoder encoder(&options);
            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }
        {
            balber::BerEncoderOptions options;
            options.setDisableUnselectedChoiceEncoding(true);

            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithAnonymousChoice value;
            value.attribute1() = 34;
            value.choice().makeMyChoice1(58);
            value.attribute2() = "Hello";

            balber::BerEncoder encoderDisableUnselectedChoices(&options);
            ASSERT(0 == encoderDisableUnselectedChoices.encode(&osb, value));
            printDiagnostic(encoderDisableUnselectedChoices);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }
        {
            balber::BerEncoderOptions options;
            options.setDisableUnselectedChoiceEncoding(true);

            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithAnonymousChoice value;
            value.attribute1() = 34;
            value.choice().makeMyChoice2("World!");
            value.attribute2() = "Hello";

            balber::BerEncoder encoderDisableUnselectedChoices(&options);
            ASSERT(0 == encoderDisableUnselectedChoices.encode(&osb, value));
            printDiagnostic(encoderDisableUnselectedChoices);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ARRAYS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Arrays"
                               << "\n==============" << bsl::endl;

        if (verbose) bsl::cout << "\nTesting with empty array." << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithArray value;
            value.attribute1() = 34;

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting with non-empty array."
                               << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithArray value;
            value.attribute1() = 34;
            value.attribute2().push_back("Hello");
            value.attribute2().push_back("World!");

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING NULLABLE VALUES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Nullable Values"
                               << "\n=======================" << bsl::endl;

        if (verbose) bsl::cout << "\nTesting with null value." << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithNullable value;
            value.attribute1() = 34;

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting with non-null value."
                               << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithNullable value;
            value.attribute1() = 34;
            value.attribute2().makeValue("Hello");

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING CHOICES
        //
        // Concerns:
        //: 1 If 'balber::BerEncoderOptions' is not specified then the encoder
        //:   will successfully encode an input with an unselected choice
        //
        //: 2 If 'balber::BerEncoderOptions' is specified but the
        //:   'disableUnselectedChoiceEncoding' is set to 'false' then
        //:   the encoder will successfully encode an input with an unselected
        //:   choice
        //:
        //: 3 If 'balber::BerEncoderOptions' is specified but the
        //:   'disableUnselectedChoiceEncoding' is set to 'true' then
        //:   the encoder will fail to encode an input with an unselected
        //:   choice
        //
        // Plan:
        //: 1 Create two 'balber::BerEncoderOptions' objects.  Set the
        //:   'disableUnselectedChoiceEncoding' option in one encoder options
        //:   object to 'true' and to 'false' in the another object.
        //:
        //: 2 Create three 'balber::BerEncoder' objects passing the two
        //:   'balber::BerEncoderOptions' objects created in step 1 to the
        //:   first two encoder objects.  The third encoder object is not
        //:   passed any encoder options.
        //:
        //: 3 Populate two 'MyChoice' objects.  Ensuring that the first
        //:   defaults to an unselected choice.  The second populated with
        //:   a choice value
        //:
        //: 4 Encode the two 'MyChoice' objects onto a
        //:   'bdlsb::MemOutStreamBuf' using each of the created
        //:   'balber::BerEncoder' objects.
        //:
        //: 5 Ensure that the encoder fails to encode the object when
        //:   'disableUnselectedChoiceEncoding' option is set and the
        //:   objects choice is unselected.  Otherwise the encoding
        //:   should be successful.
        //:
        //
        // Testing:
        //  Encoding of choices
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Choices"
                               << "\n===============" << bsl::endl;

        if (verbose) bsl::cout << "\nTesting with no selection." << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MyChoice value;
            ASSERT(value.isUndefinedValue());

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }
        {
            balber::BerEncoderOptions options;
            bdlsb::MemOutStreamBuf osb;

            test::MyChoice value;
            ASSERT(value.isUndefinedValue());

            balber::BerEncoder encoder(&options);
            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }
        {
            balber::BerEncoderOptions options;
            options.setDisableUnselectedChoiceEncoding(true);

            bdlsb::MemOutStreamBuf osb;

            test::MyChoice value;
            ASSERT(value.isUndefinedValue());

            balber::BerEncoder encoderDisableUnselectedChoices(&options);
            ASSERT(0 != encoderDisableUnselectedChoices.encode(&osb, value));
            printDiagnostic(encoderDisableUnselectedChoices);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting with selection." << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MyChoice value;
            value.makeSelection1();
            value.selection1() = 34;

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);


            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }
        {
            balber::BerEncoderOptions options;

            bdlsb::MemOutStreamBuf osb;

            test::MyChoice value;
            value.makeSelection1();
            value.selection1() = 34;

            balber::BerEncoder encoder(&options);
            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);


            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }
        {
            balber::BerEncoderOptions options;
            options.setDisableUnselectedChoiceEncoding(true);

            bdlsb::MemOutStreamBuf osb;

            test::MyChoice value;
            value.makeSelection1();
            value.selection1() = 34;

            balber::BerEncoder encoderDisableUnselectedChoices(&options);
            ASSERT(0 == encoderDisableUnselectedChoices.encode(&osb, value));
            printDiagnostic(encoderDisableUnselectedChoices);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING SEQUENCES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Sequences"
                               << "\n=================" << bsl::endl;

        bdlsb::MemOutStreamBuf osb;

        test::MySequence value;
        value.attribute1() = 34;
        value.attribute2() = "Hello";

        ASSERT(0 == encoder.encode(&osb, value));
        printDiagnostic(encoder);

        if (veryVerbose) {
            P(osb.length())
            printBuffer(osb.data(), osb.length());
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING ENUMERATIONS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Enumerations"
                               << "\n====================" << bsl::endl;

        bdlsb::MemOutStreamBuf osb;

        test::MyEnumeration::Value value = test::MyEnumeration::VALUE1;

        ASSERT(0 == encoder.encode(&osb, value));
        printDiagnostic(encoder);

        if (veryVerbose) {
            P(osb.length())
            printBuffer(osb.data(), osb.length());
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CUSTOMIZED TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Customized Types"
                               << "\n========================" << bsl::endl;

        bdlsb::MemOutStreamBuf osb1, osb2;

        const bsl::string VALUE = "Hello";

        if (verbose) bsl::cout << "\nEncoding customized string." << bsl::endl;
        {
            test::CustomizedString value;
            value.fromString(VALUE);

            ASSERT(0 == encoder.encode(&osb1, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb1.length())
                printBuffer(osb1.data(), osb1.length());
            }
        }

        if (verbose) bsl::cout << "\nEncoding bsl::string (control)."
                               << bsl::endl;
        {
            bsl::string value = VALUE;

            ASSERT(0 == encoder.encode(&osb2, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb2.length())
                printBuffer(osb2.data(), osb2.length());
            }
        }

        LOOP2_ASSERT(osb1.length(),   osb2.length(),
                     osb1.length() == osb2.length());
        ASSERT(0 == bsl::memcmp(osb1.data(), osb2.data(), osb1.length()));

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VOCABULARY TYPES TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nVOCABULARY TYPES TEST"
                               << "\n=====================" << bsl::endl;

        if (verbose) bsl::cout << "\nTesting bdlt::Date"
                               << "\n=================" << bsl::endl;

        {
            const int YEAR = 2005, MONTH = 12, DAY = 15;

            bdlt::Date d(YEAR, MONTH, DAY);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, d));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdlt::DateTz"
                               << "\n===================" << bsl::endl;

        {
            const int YEAR = 2005, MONTH = 12, DAY = 15, OFFSET = 45;

            bdlt::DateTz d(bdlt::Date(YEAR, MONTH, DAY), OFFSET);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, d));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdlt::Time"
                               << "\n=================" << bsl::endl;

        {
            const int HOUR = 12, MIN = 56, SECS = 9, MILLISECS = 134;

            bdlt::Time t(HOUR, MIN, SECS, MILLISECS);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, t));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdlt::TimeTz"
                               << "\n===================" << bsl::endl;

        {
            const int HOUR   = 12, MIN = 56, SECS = 9, MILLISECS = 134,
                      OFFSET = 45;

            bdlt::TimeTz t(bdlt::Time(HOUR, MIN, SECS, MILLISECS), OFFSET);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, t));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdlt::Datetime"
                               << "\n=====================" << bsl::endl;

        {
            const int YEAR = 2005, MONTH = 12, DAY = 15;
            const int HOUR = 12, MIN = 56, SECS = 9, MILLISECS = 134;

            bdlt::Datetime dt(YEAR, MONTH, DAY, HOUR, MIN, SECS, MILLISECS);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, dt));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdlt::DatetimeTz"
                               << "\n=======================" << bsl::endl;

        {
            const int YEAR   = 2005, MONTH = 12, DAY = 15;
            const int HOUR   = 12, MIN = 56, SECS = 9, MILLISECS = 134;
            const int OFFSET = 45;

            bdlt::DatetimeTz dt(bdlt::Datetime(YEAR, MONTH, DAY,
                                             HOUR, MIN, SECS, MILLISECS),
                               OFFSET);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, dt));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // REAL TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nREAL TEST"
                               << "\n=========" << bsl::endl;

        {
            static const struct {
                int         d_lineNum; // source line number
                double      d_value;   // real value
                const char *d_exp;     // encoded result
            } REAL[] = {
                //line  value               buffer
                //----  -----               ------
                { L_,   0,             "09 00" },
                { L_,   1.25,          "09 03 80 FE 05" },
                { L_,   1.1,           "09 09 80 CD 08 CC CC CC CC CC CD" },
                { L_,   0.0176,        "09 09 80 C6 12 05 BC 01 A3 6E 2F" },
                { L_,   -7.8752345,    "09 09 C0 CE 1F 80 3D 79 07 52 DB" },
                { L_,   99.234,
                                       "09 09 80 D2 18 CE F9 DB 22 D0 E5" },
                { L_,   -100.987,
                                       "09 09 C0 D3 0C 9F 95 81 06 24 DD" },
                { L_,   -77723.875,    "09 05 C0 FD 09 7C DF"             },
                { L_,   19998989.1234, "09 09 80 E4 13 12 90 D1 F9 72 47"    },
                { L_,   79879879249686698E-100,
                                       "09 0A 81 FE B7 1F 09 39 59 03 93 B3" },
                { L_,   -9999999999999999E25,
                                       "09 09 C0 55 09 2E FD 1B 8D 0C F3"    },

                // Single precision denormalized numbers
                { L_,   1.4E-45,       "09 0A 81 FF 37 0F FC 34 5F A6 CA B5" },
                { L_,   -1.4E-45,      "09 0A C1 FF 37 0F FC 34 5F A6 CA B5" },

                // Double precision denormalized numbers
                { L_,   1E-314,        "09 07 81 FB CE 78 A4 22 05" },
                { L_,   -1E-314,       "09 07 C1 FB CE 78 A4 22 05" },

                // FLT_MAX & FLT_MIN
                { L_,   1.175494351E-38,
                                       "09 0A 81 FF 4E 10 00 00 00 0A 63 9B" },
                { L_,   3.402823466E+38,
                                       "09 09 80 4C 0F FF FF EF F8 38 1B" },

            };

            const int NUM_DATA = sizeof REAL / sizeof *REAL;

            if (verbose) { cout << "\nTesting normal real values" << endl; }

            for (int di = 0; di < NUM_DATA; ++di) {
                const int     LINE  = REAL[di].d_lineNum;
                const double  VALUE = REAL[di].d_value;
                const char   *EXP   = REAL[di].d_exp;

                bdlsb::MemOutStreamBuf osb;

                LOOP_ASSERT(LINE, 0 == encoder.encode(&osb, VALUE));
                printDiagnostic(encoder);
                LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    T_ T_ P(osb.length())
                    T_ T_ P(EXP)
                    T_ T_ cout << "ACTUAL: ";
                    printBuffer(osb.data(), osb.length());
                }
            }

            if (verbose) { cout << "\nTesting for special values" << endl; }
            {
                static const struct {
                    int         d_lineNum;  // source line number
                    int         d_sign;     // sign value (0 - +ve, 1 - -ve)
                    int         d_exponent; // unbiased exponent value
                    long long   d_mantissa; // mantissa value
                    const char *d_buffer;   // encoded result
                    int         d_result;   // SUCCESS (0) / FAILURE (-1)
                } REAL[] = {
                    //line  sign  exp      man            buffer       res
                    //----  ----  ---      ---            ------       ---
                    // +ve & -ve infinity
                    { L_,      0, 1024,     0,         "09 01 40",   SUCCESS },
                    { L_,      1, 1024,     0,         "09 01 41",   SUCCESS },

                    // +ve & -ve NaN
                    { L_,      0, 1024,     1,         "09 01 42",   SUCCESS },
                    { L_,      1, 1024,     1,         "09 01 42",   SUCCESS },
                    { L_,      0, 1024, 0x000fffffffffffffLL,
                                                       "09 01 42",   SUCCESS },
                    { L_,      1, 1024, 0x000fffffffffffffLL,
                                                       "09 01 42",   SUCCESS },
                    { L_,      0, 1024, 0x000123456789ABCDLL,
                                                       "09 01 42",   SUCCESS },
                    { L_,      0, 1024, 0x000123456789ABCDLL,
                                                       "09 01 42",   SUCCESS },
                };

                bdlsb::MemOutStreamBuf osb;

                const int NUM_DATA = sizeof REAL / sizeof *REAL;

                for (int di = 0; di < NUM_DATA; ++di) {
                    const int        LINE     = REAL[di].d_lineNum;
                    const int        SIGN     = REAL[di].d_sign;
                    const int        EXPONENT = REAL[di].d_exponent;
                    const long long  MANTISSA = REAL[di].d_mantissa;
                    const char      *BUFFER   = REAL[di].d_buffer;
                    const int        RESULT   = REAL[di].d_result;
                          double     value;

                    bdlsb::MemOutStreamBuf osb;

                    assembleDouble(&value, SIGN, EXPONENT, MANTISSA);

                    ASSERT(RESULT == encoder.encode(&osb, value));
                    printDiagnostic(encoder);

                    if (0 == RESULT) {
                        LOOP_ASSERT(LINE,
                                    0 == compareBuffers(osb.data(), BUFFER));
                    }
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // FUNDAMENTALS TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nFUNDAMENTALS TEST"
                               << "\n=================" << bsl::endl;

        {
            const unsigned char   XA1 = UCHAR_MAX;
            const signed   char   XB1 = SCHAR_MIN;
            const          char   XC1 = SCHAR_MAX;

            const unsigned short  XD1 = USHRT_MAX;
            const signed   short  XE1 = SHRT_MIN;
            const          short  XF1 = SHRT_MAX;

            const unsigned int    XG1 = UINT_MAX;
            const signed   int    XH1 = INT_MIN;
            const          int    XI1 = INT_MAX;

            const unsigned long   XJ1 = ULONG_MAX;
            const signed   long   XK1 = LONG_MIN;
            const          long   XL1 = LONG_MAX;

            const bsls::Types::Int64  XM1 = 0xff34567890123456LL;
            const bsls::Types::Uint64 XN1 = 0x1234567890123456LL;

            const          bool   XO1 = true;

            const bsl::string     XP1("This is a really long line");
                  bsl::string     XP2;

            const float        XQ1 = 99.234f;
            const float        XR1 = -100.987f;
            const float        XS1 = -77723.875f;

            const double       XT1 = 19998989.1234;
            const double       XU1 = 100;

            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XA1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XB1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XC1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XD1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XE1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XF1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XG1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XH1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XI1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XJ1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XK1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XL1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XM1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XN1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XO1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XP1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XQ1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XR1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XS1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XT1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdlsb::MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XU1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        // --------------------------------------------------------------------

        static const int MAX_BUF_SIZE = 100000;

        int reps = 1000;
        int arraySize = 200;
        char requestType = 'b';

        if (argc > 2) {
            ASSERT('-' == argv[2][0]);

            // Request type:
            //  -s == sqrt request
            //  -r == basicRecord request
            //  -b == bigRecord request
            //  -2 == bigRecord2 request
            requestType = argv[2][1];
            if (('b' == requestType || '2' == requestType) && argc > 4) {
                // Get array size for types contain arrays
                arraySize = bsl::atoi(argv[4]);
                veryVeryVerbose = argc > 5;
            }
        }

        if (argc > 3) {
            // Get number of repetitions
            reps = bsl::atoi(argv[3]);
        }

        // Create request object:
        test::TimingRequest request;
        int minOutputSize;
        switch (requestType) {
          case 's': {
              bsl::cout << "sqrt request" << bsl::endl;

              test::Sqrt sqrt;
              sqrt.value() = 3.1415927;
              request.makeSqrt(sqrt);
              minOutputSize = 2;
          } break;
          case 'r': {
              bsl::cout << "basicRecord request" << bsl::endl;

              test::BasicRecord basicRec;
              basicRec.i1() = 11;
              basicRec.i2() = 22;
              basicRec.dt() = bdlt::DatetimeTz(
                  bdlt::Datetime(bdlt::Date(2007, 9, 3),
                                bdlt::Time(16, 30)), 0);
              basicRec.s() = "The quick brown fox jumped over the lazy dog.";
              request.makeBasic(basicRec);
              minOutputSize = 3 * sizeof(int) + basicRec.s().length();
          } break;
          case 'b': {
              bsl::cout << "bigRecord request with array size of "
                        << arraySize << bsl::endl;

              test::BasicRecord basicRec;
              basicRec.i1() = 11;
              basicRec.i2() = 22;
              basicRec.dt() = bdlt::DatetimeTz(
                  bdlt::Datetime(bdlt::Date(2007, 9, 3),
                                bdlt::Time(16, 30)), 0);
              basicRec.s() = "The quick brown fox jumped over the lazy dog.";
              int minBasicRecSize =
                  minOutputSize = 3 * sizeof(int) + basicRec.s().length();

              test::BigRecord   bigRec;
              bigRec.name() = "This record is so big, it has its own gravity.";

              for (int i = 0; i < arraySize; ++i) {
                  bigRec.array().push_back(basicRec);
              }

              request.makeBig(bigRec);
              minOutputSize =
                  bigRec.name().length() + arraySize * minBasicRecSize;
          } break;
          default:
            bsl::cerr << "Unknown request type: " << '-' << requestType
                      << bsl::endl;
            return 1;                                                 // RETURN
        }

        bsl::cout << "  " << reps << " repetitions..." << bsl::endl;

        // Measure old ber encoding times:
        bdlsb::MemOutStreamBuf osb;
        osb.reserveCapacity(MAX_BUF_SIZE);

        bsls::Stopwatch stopwatch;
        double elapsed;

        // Measure ber encoding times:
        stopwatch.reset();
        stopwatch.start();
        for (int i = 0; i < reps; ++i) {
            osb.pubseekpos(0);
            balber::BerEncoder encoder;  // Typical usage: single-use object
            encoder.encode(&osb, request);
        }
        stopwatch.stop();

        ASSERT(minOutputSize     <= (int)osb.length());
        ASSERT((int)osb.length() <= MAX_BUF_SIZE);
        elapsed = stopwatch.elapsedTime();
        ASSERT(elapsed > 0);

        bsl::cout << "    balber::BerEncoder: "
                  << elapsed          << " seconds, "
                  << (reps / elapsed) << " reps/sec, "
                  << osb.length()     << " bytes" << bsl::endl;
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }

    return testStatus;
}

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
