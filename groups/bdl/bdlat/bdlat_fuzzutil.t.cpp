// bdlat_fuzzutil.t.cpp                                               -*-C++-*-
#include <bdlat_fuzzutil.h>
#include <bdlat_attributeinfo.h>
#include <bdlat_enumeratorinfo.h>
#include <bdlat_formattingmode.h>
#include <bdlat_selectioninfo.h>
#include <bdlat_sequencefunctions.h>
#include <bdlat_typetraits.h>

#include <bdlb_nullablevalue.h>
#include <bdlb_string.h>

#include <bslim_fuzzutil.h>
#include <bslim_fuzzdataview.h>
#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsl_iostream.h>

#include <stdlib.h>  // `atoi`

using namespace BloombergLP;
using bsl::cout;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// `bdlat::FuzzUtil` provides a function for consuming `bdlat`-types from fuzz
// data.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] static void consumeMessage(t_MESSAGE *m, bslim::FuzzDataView *fd);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

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

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

                           // ==============
                           // class MyChoice
                           // ==============

/// Sample choice type.
class MyChoice {
  public:
    // TYPES
    enum {
        NUM_SELECTIONS = 2
    };

    // PUBLIC DATA
    //union { // cannot contain `string` in C++03
        int         d_selection0;
        bsl::string d_selection1;
    //};
    int d_selectionId;

    // PUBLIC CLASS DATA
    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CREATORS
    MyChoice();

    // MANIPULATORS
    int makeSelection(int selectionId);

    template <class t_MANIPULATOR>
    int manipulateSelection(t_MANIPULATOR& manipulator);
};

                           // ============
                           // class MyEnum
                           // ============

/// Sample enumeration type.
class MyEnum {
  public:
    // TYPES
    enum Value {
        EnumFallbackValue = 0,
        EnumValue1 = 1,
        EnumValue2 = 2,
        EnumValue3 = 3
    };

    enum {
        NUM_ENUMERATORS = 4
    };

    // PUBLIC CLASS DATA
    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];

    // CLASS METHODS
    static int fromInt(MyEnum::Value *result, int number);
};

                           // ====================
                           // class MyCustomString
                           // ====================

/// Sample customized type.
class MyCustomString {
    // DATA
    bsl::string d_value;

    // HIDDEN FRIENDS
    friend bool operator==(const MyCustomString& o1, const MyCustomString& o2)
    {
        return o1.d_value == o2.d_value;
    }
  public:
    // TYPES
    typedef bsl::string BaseType;

    // MANIPULATORS
    int fromString(const bsl::string& value);
};

                           // ===================
                           // class MyDynamicType
                           // ===================

/// Sample dynamic type.
class MyDynamicType {
    // HIDDEN FRIENDS
    friend bool operator==(const MyDynamicType& o1, const MyDynamicType& o2)
    {
        return o1.d_value == o2.d_value;
    }

  public:
    // PUBLIC DATA
    bsl::string d_value;
};

                           // ===============
                           // class MyMessage
                           // ===============

/// A message type for USAGE EXAMPLE.
struct MyMessage {
    // PUBLIC DATA

    // Simple types
    int                d_field0;
    unsigned int       d_field1;
    char               d_field2;
    signed char        d_field3;
    unsigned char      d_field4;
    short              d_field5;
    unsigned short     d_field6;
    long               d_field7;
    unsigned long      d_field8;
    long long          d_field9;
    unsigned long long d_field10;
    bool               d_field11;
    float              d_field12;
    double             d_field13;
    bdldfp::Decimal64  d_field14;
    bsl::string        d_field15;
    bsl::vector<char>  d_field16;
    bdlt::Date         d_field17;
    bdlt::DateTz       d_field18;
    bdlt::Datetime     d_field19;
    bdlt::DatetimeTz   d_field20;
    bdlt::Time         d_field21;
    bdlt::TimeTz       d_field22;

    // Complex types
    bsl::vector<int>         d_field23;  // array
    MyChoice                 d_field24;  // choice
    MyEnum::Value            d_field25;  // enumeration
    bdlb::NullableValue<int> d_field26;  // nullable value
    MyCustomString           d_field27;  // customized type
    MyDynamicType            d_field28;  // dynamic type

    // PUBLIC CLASS DATA
    static const bdlat_AttributeInfo ATTRIBUTE_INFO[];
};

                           // --------------
                           // class MyChoice
                           // --------------

// TRAITS
namespace BloombergLP {
BDLAT_DECL_CHOICE_TRAITS(MyChoice)
}  // close enterprise namespace

// PUBLIC CLASS DATA
const bdlat_SelectionInfo MyChoice::SELECTION_INFO_ARRAY[] = {
    {
        0,
        "selection0",
        sizeof("selection0") - 1,
        "Selection 0",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        1,
        "selection1",
        sizeof("selection1") - 1,
        "Selection 1",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CREATORS
inline
MyChoice::MyChoice()
: d_selectionId(-1)
{
}

// MANIPULATORS
int MyChoice::makeSelection(int selectionId)
{
    switch (selectionId)
    {
      case 0: {
        d_selectionId = selectionId;
        d_selection0 = 0;
      } break;
      case 1: {
        d_selectionId = selectionId;
        d_selection1.clear();
      } break;
      default:
        return 1; // error                                            // RETURN
    }
    return 0;
}

template <class t_MANIPULATOR>
int MyChoice::manipulateSelection(t_MANIPULATOR& manipulator)
{
    switch (d_selectionId)
    {
      case 0:
        return manipulator(&d_selection0, SELECTION_INFO_ARRAY[0]);   // RETURN
      case 1:
        return manipulator(&d_selection1, SELECTION_INFO_ARRAY[1]);   // RETURN
      default:
        ASSERT(false);
    }
    return 0;
}

int bdlat_choiceMakeSelection(MyChoice *object, int selectionId)
{
    return object->makeSelection(selectionId);
}

template <class t_MANIPULATOR>
int bdlat_choiceManipulateSelection(MyChoice       *object,
                                    t_MANIPULATOR&  manipulator)
{
    return object->manipulateSelection(manipulator);
}

bool operator==(const MyChoice& o1, const MyChoice& o2)
{
    if(o1.d_selectionId != o2.d_selectionId) {
        return false;                                                 // RETURN
    }
    switch(o1.d_selectionId) {
      case 0: return o1.d_selection0 == o2.d_selection0;              // RETURN
      case 1: return o1.d_selection1 == o2.d_selection1;              // RETURN
    }
    ASSERT(false);
    return false;
}

                           // ------------
                           // class MyEnum
                           // ------------

// TRAITS
namespace BloombergLP {
BDLAT_DECL_ENUMERATION_TRAITS(MyEnum)
}  // close enterprise namespace

// PUBLIC CLASS DATA
const bdlat_EnumeratorInfo MyEnum::ENUMERATOR_INFO_ARRAY[] = {
    {
        MyEnum::EnumFallbackValue,
        "EnumFallbackValue",
        sizeof("EnumFallbackValue") - 1,
        ""
    },
    {
        MyEnum::EnumValue1,
        "EnumValue1",
        sizeof("EnumValue1") - 1,
        ""
    },
    {
        MyEnum::EnumValue2,
        "EnumValue2",
        sizeof("EnumValue2") - 1,
        ""
    },
    {
        MyEnum::EnumValue3,
        "EnumValue3",
        sizeof("EnumValue3") - 1,
        ""
    }
};

// CLASS METHODS
int MyEnum::fromInt(MyEnum::Value *result, int number)
{
    if(number < 0 || number >= NUM_ENUMERATORS) {
        return -1;                                                    // RETURN
    }
    *result = static_cast<Value>(number);
    return 0;
}

                           // --------------------
                           // class MyCustomString
                           // --------------------

// MANIPULATORS
int MyCustomString::fromString(const bsl::string& value)
{
    d_value = value;
    return 0;
}

int bdlat_customizedTypeConvertFromBaseType(MyCustomString     *object,
                                            const bsl::string&  value)
{
    return object->fromString(value);
}

namespace BloombergLP {
namespace bdlat_CustomizedTypeFunctions {

template <>
struct IsCustomizedType<MyCustomString> : bsl::true_type {
};

template <>
struct BaseType<MyCustomString> {
    typedef MyCustomString::BaseType Type;
};

}  // close namespace bdlat_CustomizedTypeFunctions
}  // close enterprise namespace

                           // -------------------
                           // class MyDynamicType
                           // -------------------

// MANIPULATORS
template <class t_MANIPULATOR>
int bdlat_typeCategoryManipulateSimple(MyDynamicType  *object,
                                       t_MANIPULATOR&  manipulator)
{
    return manipulator(&object->d_value, bdlat_TypeCategory::Simple());
}

// ACCESSORS
bdlat_TypeCategory::Value bdlat_typeCategorySelect(const MyDynamicType& )
{
    return bdlat_TypeCategory::e_SIMPLE_CATEGORY;
}

namespace BloombergLP {

template <>
struct bdlat_TypeCategoryDeclareDynamic<MyDynamicType> : bsl::true_type {
};

}  // close enterprise namespace

                           // ---------------
                           // class MyMessage
                           // ---------------

// PUBLIC CLASS DATA
#define DECLARE_FIELD(n) { n, "field"#n, sizeof("field"#n) - 1U, \
                           "Field "#n, bdlat_FormattingMode::e_DEFAULT }

const bdlat_AttributeInfo MyMessage::ATTRIBUTE_INFO[] = {
    DECLARE_FIELD(0),
    DECLARE_FIELD(1),
    DECLARE_FIELD(2),
    DECLARE_FIELD(3),
    DECLARE_FIELD(4),
    DECLARE_FIELD(5),
    DECLARE_FIELD(6),
    DECLARE_FIELD(7),
    DECLARE_FIELD(8),
    DECLARE_FIELD(9),
    DECLARE_FIELD(10),
    DECLARE_FIELD(11),
    DECLARE_FIELD(12),
    DECLARE_FIELD(13),
    DECLARE_FIELD(14),
    DECLARE_FIELD(15),
    DECLARE_FIELD(16),
    DECLARE_FIELD(17),
    DECLARE_FIELD(18),
    DECLARE_FIELD(19),
    DECLARE_FIELD(20),
    DECLARE_FIELD(21),
    DECLARE_FIELD(22),
    DECLARE_FIELD(23),
    DECLARE_FIELD(24),
    DECLARE_FIELD(25),
    DECLARE_FIELD(26),
    DECLARE_FIELD(27),
    DECLARE_FIELD(28)
};

#undef DECLARE_FIELD

template <class t_MANIPULATOR>
int bdlat_sequenceManipulateAttributes(MyMessage      *object,
                                       t_MANIPULATOR&  manipulator)
{
    int rc;
    rc = manipulator(&object->d_field0, MyMessage::ATTRIBUTE_INFO[0]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field1, MyMessage::ATTRIBUTE_INFO[1]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field2, MyMessage::ATTRIBUTE_INFO[2]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field3, MyMessage::ATTRIBUTE_INFO[3]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field4, MyMessage::ATTRIBUTE_INFO[4]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field5, MyMessage::ATTRIBUTE_INFO[5]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field6, MyMessage::ATTRIBUTE_INFO[6]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field7, MyMessage::ATTRIBUTE_INFO[7]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field8, MyMessage::ATTRIBUTE_INFO[8]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field9, MyMessage::ATTRIBUTE_INFO[9]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field10, MyMessage::ATTRIBUTE_INFO[10]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field11, MyMessage::ATTRIBUTE_INFO[11]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field12, MyMessage::ATTRIBUTE_INFO[12]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field13, MyMessage::ATTRIBUTE_INFO[13]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field14, MyMessage::ATTRIBUTE_INFO[14]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field15, MyMessage::ATTRIBUTE_INFO[15]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field16, MyMessage::ATTRIBUTE_INFO[16]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field17, MyMessage::ATTRIBUTE_INFO[17]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field18, MyMessage::ATTRIBUTE_INFO[18]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field19, MyMessage::ATTRIBUTE_INFO[19]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field20, MyMessage::ATTRIBUTE_INFO[20]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field21, MyMessage::ATTRIBUTE_INFO[21]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field22, MyMessage::ATTRIBUTE_INFO[22]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field23, MyMessage::ATTRIBUTE_INFO[23]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field24, MyMessage::ATTRIBUTE_INFO[24]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field25, MyMessage::ATTRIBUTE_INFO[25]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field26, MyMessage::ATTRIBUTE_INFO[26]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field27, MyMessage::ATTRIBUTE_INFO[27]);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }
    rc = manipulator(&object->d_field28, MyMessage::ATTRIBUTE_INFO[28]);
    return rc;
}

namespace BloombergLP {
namespace bdlat_SequenceFunctions {

template <>
struct IsSequence<MyMessage> : bsl::true_type {
};

}  // close namespace bdlat_SequenceFunctions
}  // close enterprise namespace

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a Message
// - - - - - - - - - - - - - - -
// Suppose we have a message type `MyMessage` and we wish to fuzz test it.
//
// We have to provide a function like this for libFuzzer:
// ```
   extern "C"
   int LLVMFuzzerTestOneInput(const uint8_t *bytes, size_t size)
   {
       bslim::FuzzDataView fuzzData(bytes, size);

       MyMessage msg;
       bdlat::FuzzUtil::consumeMessage(&msg, &fuzzData);
       // Use `msg`...

       return 0;
   }
// ```
// After the `FuzzUtil::consumeMessage` call, the `msg` object contains a
// well-formed message, which can then be serialized or processed in another
// way.

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;
    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        const uint8_t bytes[] = { 0 };
        LLVMFuzzerTestOneInput(bytes, sizeof bytes);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `consumeMessage`
        //
        // Concerns:
        // 1. The function correctly initializes any supported `bdlat`-type:
        //
        //    - Simple types use correct underlying consume-function.
        //
        //    - Other complex types recursively apply the `consumeMessage`
        //      function to the child elements using correct algorithm.
        //
        // Plan:
        // 1. Create an array of 274 bytes.
        //
        // 2. Create an object of type `MyMessage` that is a sequence and
        //    contains attributes of all categories and all supported simple
        //    types.
        //
        // 3. Fill the object with a content generated using the
        //    `bdlat::FuzzUtil::consumeMessage` function and the byte array.
        //
        // 4. Verify that exactly 1 byte in the used `FuzzDataView` remained on
        //    systems with 64-bit `long`, or 5 bytes remained on systems with
        //    32-bit `long`.
        //
        // 5. Create a `bslim::FuzzDataView` object again using the same byte
        //    array.
        //
        // 6. For each attribute of the `MyMessage` sequence do the following
        //    steps:
        //
        //    1. Save the current state of the `bslim::FuzzDataView` object by
        //       copying it.
        //
        //    2. Create an object of the current attribute type using the
        //       `bdlat::FuzzUtil::consumeMessage` function.
        //
        //    3. Verify that the object equals to the corresponding `MyMessage`
        //       attribute.
        //
        //    4. Create one more object of the same type using the saved copy
        //       of the `bslim::FuzzDataView` object reproducing the underlying
        //       generation algorithm used for the type.
        //
        //    5. Verify that this object equals to the object generated using
        //       the `bdlat::FuzzUtil::consumeMessage` function.
        //
        // 7. Verify that exactly 1 byte in the used `FuzzDataView` remained on
        //    systems with 64-bit `long`, or 5 bytes remained on systems with
        //    32-bit `long`.
        //
        // Testing:
        //   static void consumeMessage(t_MESSAGE *m, bslim::FuzzDataView *fd);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING `consumeMessage`"
                          << "\n========================" << endl;

        const bsl::uint8_t FUZZ_DATA[274] = {
            0x31, 0x62, 0xb0, 0x10, 0x3d, 0x1a, 0xf3, 0x65, 0x95, 0x6f,
            0x06, 0x69, 0x73, 0xe8, 0x2b, 0x4b, 0x6b, 0x06, 0xbd, 0xe5,
            0xf3, 0xff, 0x72, 0x3c, 0x1b, 0xe7, 0x3f, 0x5d, 0x82, 0xcf,
            0xe8, 0x07, 0x3c, 0x21, 0x78, 0x69, 0x57, 0x07, 0x01, 0xa9,
            0xe0, 0x03, 0xba, 0xba, 0x51, 0xee, 0xc8, 0xcb, 0x33, 0x87,
            0x94, 0x7f, 0x13, 0xb2, 0x40, 0x54, 0x21, 0x67, 0xb5, 0x5b,
            0xf9, 0x4c, 0xe2, 0x8b, 0x27, 0x83, 0x90, 0xac, 0x96, 0x51,
            0xc0, 0x7f, 0x57, 0xad, 0xbe, 0x34, 0xcb, 0xf6, 0x49, 0x6c,
            0xce, 0xd9, 0x22, 0x4f, 0xf2, 0x10, 0x71, 0x3e, 0x37, 0x87,
            0x96, 0x7d, 0x92, 0x91, 0x84, 0x20, 0xa6, 0x21, 0xea, 0x78,
            0x19, 0x40, 0x06, 0x41, 0xeb, 0x89, 0x27, 0x5e, 0x2f, 0xd4,
            0xd5, 0xeb, 0xb7, 0xd6, 0xd8, 0xf4, 0x0c, 0x64, 0xad, 0x01,
            0x82, 0xd2, 0xd1, 0x70, 0x8b, 0x67, 0x34, 0xeb, 0xf8, 0x80,
            0x8c, 0xe3, 0x24, 0xbc, 0x5f, 0x9c, 0xd2, 0x44, 0xcb, 0x32,
            0x04, 0x5c, 0x4a, 0xef, 0x5c, 0x0a, 0x6f, 0x4d, 0xd5, 0x2f,
            0xc5, 0x74, 0x2d, 0xca, 0x31, 0xca, 0x2c, 0x1e, 0x56, 0xca,
            0xb8, 0x49, 0xa1, 0x63, 0xe6, 0x6d, 0xcf, 0x62, 0x76, 0xec,
            0x61, 0x47, 0x66, 0x51, 0xa9, 0x84, 0x0e, 0xbc, 0x64, 0x1f,
            0xee, 0x0f, 0x89, 0x48, 0x76, 0xc2, 0x9d, 0xfd, 0x52, 0xb0,
            0x04, 0xa0, 0xe7, 0x6f, 0x9e, 0xae, 0x83, 0x9f, 0x72, 0xd2,
            0xfe, 0xb4, 0xa7, 0xa6, 0x56, 0xcd, 0x28, 0xf5, 0xd2, 0xfc,
            0xd2, 0xf5, 0xdd, 0x6b, 0x9b, 0x34, 0x4e, 0xa4, 0xc6, 0x4e,
            0x46, 0xb5, 0x2c, 0x94, 0xdc, 0x97, 0xd7, 0x3a, 0x8e, 0x64,
            0xf3, 0xc7, 0x3c, 0x34, 0x41, 0x8b, 0x9e, 0x7d, 0xf7, 0x83,
            0x67, 0xcc, 0xa2, 0x0f, 0x0b, 0xf7, 0x88, 0x23, 0x99, 0x2d,
            0x53, 0x3b, 0xc3, 0x41, 0xe5, 0x00, 0x48, 0x76, 0xc2, 0x7d,
            0x8c, 0xe3, 0x24, 0xbc, 0x5f, 0x9c, 0xd2, 0x44, 0xcb, 0x32,
            0x04, 0x5c, 0x4a, 0xef
        };
        unsigned FUZZ_DATA_REMAINDER = 1;
        if(sizeof(long) < 8) {
            FUZZ_DATA_REMAINDER += 4;
        }

        MyMessage msg;
        {
            bslim::FuzzDataView fuzzData(FUZZ_DATA, sizeof FUZZ_DATA);
            bdlat::FuzzUtil::consumeMessage(&msg, &fuzzData);
            ASSERTV(fuzzData.length(),
                    fuzzData.length() == FUZZ_DATA_REMAINDER);
        }

        // "Iterate" over the fields again
        bslim::FuzzDataView fuzzData(FUZZ_DATA, sizeof FUZZ_DATA);
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            int value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field0);

            int value2 = bslim::FuzzUtil::consumeNumber<int>(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            unsigned int value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field1);

            unsigned int value2 =
                   bslim::FuzzUtil::consumeNumber<unsigned int>(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            char value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field2);

            char value2 = bslim::FuzzUtil::consumeNumber<char>(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            signed char value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field3);

            signed char value2 =
                    bslim::FuzzUtil::consumeNumber<signed char>(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            unsigned char value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field4);

            unsigned char value2 =
                  bslim::FuzzUtil::consumeNumber<unsigned char>(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            short value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field5);

            short value2 =
                          bslim::FuzzUtil::consumeNumber<short>(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            unsigned short value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field6);

            unsigned short value2 =
                 bslim::FuzzUtil::consumeNumber<unsigned short>(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            long value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field7);

            long value2 = bslim::FuzzUtil::consumeNumber<long>(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            unsigned long value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field8);

            unsigned long value2 =
                  bslim::FuzzUtil::consumeNumber<unsigned long>(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            long long value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field9);

            long long value2 =
                      bslim::FuzzUtil::consumeNumber<long long>(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            unsigned long long value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field10);

            unsigned long long value2 =
             bslim::FuzzUtil::consumeNumber<unsigned long long>(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            bool value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field11);

            bool value2 = bslim::FuzzUtil::consumeBool(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            float value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field12);

            float value2 =
                          bslim::FuzzUtil::consumeNumber<float>(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            double value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field13);

            double value2 =
                         bslim::FuzzUtil::consumeNumber<double>(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            bdldfp::Decimal64 value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field14);


            bdldfp::Decimal64 value2(
                        bslim::FuzzUtil::consumeNumber<double>(&fuzzDataCopy));
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            bsl::string value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field15);

            bsl::string value2;
            bslim::FuzzUtil::consumeRandomLengthString(
                          &value2,
                          &fuzzDataCopy,
                          bdlat::FuzzUtilOptions::k_MAX_STRING_LENGTH_DEFAULT);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            bsl::vector<char> value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field16);

            bsl::vector<char> value2;
            bslim::FuzzUtil::consumeRandomLengthChars(
                          &value2,
                          &fuzzDataCopy,
                          bdlat::FuzzUtilOptions::k_MAX_STRING_LENGTH_DEFAULT);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            bdlt::Date value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field17);

            bdlt::Date value2 = bdlt::FuzzUtil::consumeDate(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            bdlt::DateTz value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field18);

            bdlt::DateTz value2 = bdlt::FuzzUtil::consumeDateTz(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            bdlt::Datetime value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field19);

            bdlt::Datetime value2 =
                                bdlt::FuzzUtil::consumeDatetime(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            bdlt::DatetimeTz value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field20);

            bdlt::DatetimeTz value2 =
                              bdlt::FuzzUtil::consumeDatetimeTz(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            bdlt::Time value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field21);

            bdlt::Time value2 = bdlt::FuzzUtil::consumeTime(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            bdlt::TimeTz value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field22);

            bdlt::TimeTz value2 = bdlt::FuzzUtil::consumeTimeTz(&fuzzDataCopy);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            bsl::vector<int> value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field23);

            bsl::vector<int> value2;
            unsigned size = bslim::FuzzUtil::consumeNumberInRange<unsigned>(
                           &fuzzDataCopy,
                           0U,
                           bdlat::FuzzUtilOptions::k_MAX_ARRAY_LENGTH_DEFAULT);
            value2.resize(size);
            for(unsigned i = 0; i < size; i++) {
                value2[i] = bslim::FuzzUtil::consumeNumber<int>(&fuzzDataCopy);
            }
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            MyChoice value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field24);

            MyChoice value2;
            int selectionIndex = bslim::FuzzUtil::consumeNumberInRange<int>(
                                                 &fuzzDataCopy,
                                                 0,
                                                 MyChoice::NUM_SELECTIONS - 1);
            int selectionId =
                           MyChoice::SELECTION_INFO_ARRAY[selectionIndex].id();
            ASSERT(value2.makeSelection(selectionId) == 0);
            bdlat::FuzzUtil_Manipulator manipulator = {&fuzzDataCopy, 0};
            value2.manipulateSelection(manipulator);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            MyEnum::Value value = MyEnum::EnumFallbackValue;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field25);

            MyEnum::Value value2 = MyEnum::EnumFallbackValue;
            int index = bslim::FuzzUtil::consumeNumberInRange<int>(
                                                  &fuzzDataCopy,
                                                  0,
                                                  MyEnum::NUM_ENUMERATORS - 1);
            int intValue = MyEnum::ENUMERATOR_INFO_ARRAY[index].value();
            ASSERT(MyEnum::fromInt(&value2, intValue) == 0);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            bdlb::NullableValue<int> value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field26);

            bdlb::NullableValue<int> value2;
            bool addValue = bslim::FuzzUtil::consumeBool(&fuzzDataCopy);
            if (addValue) {
                bdlat_NullableValueFunctions::makeValue(&value2);
                bdlat::FuzzUtil_Manipulator manipulator = {&fuzzDataCopy, 0};
                bdlat_NullableValueFunctions::manipulateValue(&value2,
                                                              manipulator);
            }
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            MyCustomString value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field27);

            MyCustomString value2;
            MyCustomString::BaseType base;
            bdlat::FuzzUtil::consumeMessage(&base, &fuzzDataCopy);
            bdlat_CustomizedTypeFunctions::convertFromBaseType(&value2, base);
            ASSERT(value2 == value);
        }
        {
            bslim::FuzzDataView fuzzDataCopy = fuzzData;

            MyDynamicType value;
            bdlat::FuzzUtil::consumeMessage(&value, &fuzzData);
            ASSERT(value == msg.d_field28);

            MyDynamicType value2;
            bdlat::FuzzUtil_ManipulatorWithCategory manipulator =
                                                            {&fuzzDataCopy, 0};
            bdlat_TypeCategoryUtil::manipulateByCategory(&value2, manipulator);
            ASSERT(value2 == value);
        }
        ASSERTV(fuzzData.length(), fuzzData.length() == FUZZ_DATA_REMAINDER);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Add any component-related code here.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        ASSERT(true);
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND.\n";
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << endl;
    }

    return testStatus;
}

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
