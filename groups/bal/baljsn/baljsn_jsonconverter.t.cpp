// baljsn_jsonconverter.t.cpp                                         -*-C++-*-
#include <baljsn_jsonconverter.h>

#include <baljsn_encoder.h>
#include <baljsn_encoder_testtypes.h>
#include <baljsn_decoder.h>

#include <balb_testmessages.h>

#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_fixedmemoutstreambuf.h>
#include <bdlsb_memoutstreambuf.h>

// These headers are for testing only and the hierarchy level of `baljsn` was
// increased because of them.  They should be removed when possible.
#include <balxml_decoder.h>
#include <balxml_decoderoptions.h>
#include <balxml_minireader.h>
#include <balxml_errorinfo.h>

#include <bdlb_nullablevalue.h>

#include <bdljsn_error.h>
#include <bdljsn_json.h>
#include <bdljsn_jsonutil.h>

#include <bdldfp_decimal.h>
#include <bdldfp_decimalutil.h>

#include <bdlt_date.h>
#include <bdlt_datetimetz.h>

#include <bslim_printer.h>
#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_defaultallocatorguard.h>

#include <bsla_maybeunused.h>

#include <bsls_asserttest.h>
#include <bsls_nameof.h>
#include <bsls_platform.h>

#include <bsl_climits.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>  // `bsl::memcmp`
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_memory.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

#include <s_baltst_address.h>
#include <s_baltst_employee.h>
#include <s_baltst_featuretestmessage.h>
#include <s_baltst_featuretestmessageutil.h>
#include <s_baltst_generatetestarray.h>
#include <s_baltst_generatetestchoice.h>
#include <s_baltst_generatetestcustomizedtype.h>
#include <s_baltst_generatetestenumeration.h>
#include <s_baltst_generatetestnullablevalue.h>
#include <s_baltst_generatetestsequence.h>
#include <s_baltst_mychoice.h>
#include <s_baltst_myintenumeration.h>
#include <s_baltst_mysequence.h>
#include <s_baltst_mysequencewithchoice.h>
#include <s_baltst_mysequencewithnullableanonymouschoice.h>
#include <s_baltst_mysequencewithprecisiondecimalattribute.h> // TC20 Decimal64
#include <s_baltst_testchoice.h>
#include <s_baltst_testcustomizedtype.h>
#include <s_baltst_testenumeration.h>
#include <s_baltst_testplaceholder.h>
#include <s_baltst_testselection.h>
#include <s_baltst_testsequence.h>

#include <s_baltst_mysimplecontent.h>
#include <s_baltst_mysequencewithnullable.h>
#include <s_baltst_topchoice.h>
#include <s_baltst_sqrt.h>
#include <s_baltst_basicrecord.h>
#include <s_baltst_mysequencewitharray.h>
#include <s_baltst_employee.h>
#include <s_baltst_mysequence.h>
#include <s_baltst_sequencewithanonymity.h>

#include <s_baltst_myenumerationwithfallback.h> // TC 17
#include <s_baltst_sqrtf.h>                     // TC 16

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
namespace test = BloombergLP::s_baltst;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements an converter mechanism for setting
// a `bdjsn:Json` objects with values extracted from a `bdlat`-compatible
// object.  The object types that can be handled include `bdlat` sequence,
// choice, array, enumeration, customized, simple, and dynamic types.
//
// We use standard table-based approach to testing where we put both input and
// expected output in the same table row and verify that the actual result
// matches the expected value.
//
// Most of these tests look for equality between a `bdljsn::Json` object
// created by `convert` directly from a `bdlat` object with a "reference"
// object obtained from an "oracle" function.
//
//   1. The oracle function uses the (thoroughly tested) `bdljsn::Encoder` to
//      represent a `bdlat` object as JSON document.
//
//      * Note that the encoder is always called with options to
//      `encodeNullElements` and `encodeEmptyArrays` since those policies are
//      hard-coded into `convert`.
//
//   2. That JSON document is used to create a `bdljsn::Json` object using the
//      (again, thoroughly tested) `bdjsn::JsonUtil::read` function.
//
// Note that sometimes this test case uses methods from the `Oracle` helper
// utility.  Sometimes the equivalent is explicitly coded into the test case.
//
// The set of `bdlat` object used for testing here is derived from the
// set of objects appearing in `baljsn_encoder.t.cpp`.  Much of the
// infrastructure of that test driver is replicated here but in
// simplified form since this test driver need not cover the parameter
// space of JSON document formatting (e.g., pretty vs compact styles,
// indentation levels).
//
// This test driver shows that every `bdlat` object from the encoder test
// driver converts correctly except for two:
//
//  * In "COMPLEX TEST MESSAGES" (TC 4) one of the classes does not encode
//    correctly and is skipped in this test driver.
//
//  * In "ENCODING NULL CHOICE" (TC 13) the encoder intentionally creates
//    a grammatically incorrect JSON document.  The `convert` method handles
//    that situation by returning a run-time error.
//
// Testing of the conversion from a bdljsn::Json object to a compatible `bdlat`
// object is by using the result from each of the tests described above to
// create a `bdlat` object that is compared to the original for equality.
// Typically, the recreated `bdlat` object has a  name ending with `FromJson`.
// As we must start with a (correct) 'Json' object, neither of the two
// problematic cases listed above can be converted back to `bdlat`.
//
// Additionally, `baljsn_decoder.t.cpp` was reviewed for test scenarios that
// were not found in the `baljsn_encoder.t.cpp` and those cases are replicated
// here using `convert`.  Notable particular are the tests for
// `skipUnknownElements`, a feature that is not encoder related.
// ----------------------------------------------------------------------------
// CREATORS
// [ 3] JsonConverter();
// [ 3] explicit JsonConverter(const allocator_type& allocator);
//
// MANIPULATORS
// [ *] int convert(bdljsn::Json *json, const TYPE& value);
// [ *] int convert(TYPE *v, const Json& j, const DecoderOptions& o);
//
// ACCESSORS
// [ 6] bsl::string loggedMessages() const;
//
// [ 3] allocator_type get_allocator() const;
// ----------------------------------------------------------------------------
// [21] CONCERN: USAGE EXAMPLE
// [20] CONCERN: TESTING `Decimal64`
// [19] CONCERN: SKIPPING UNKNOWN ELEMENTS
// [18] CONCERN: DECODING INTS AS ENUMS AND VICE VERSA
// [17] CONCERN: FALLBACK ENUMERATORS
// [16] CONCERN: FLOATING-POINT VALUES ROUND-TRIP
// [15] CONCERN: DECODING OF ENUM TYPES WITH ESCAPED CHARS
// [14] CONCERN: DECODING OF `hexBinary` CUSTOMIZED TYPE
// [13] CONCERN: CONVERTING VECTORS OF VECTORS
// [12] CONCERN: ENCODING NULL CHOICE
// [11] CONCERN: CONVERTING UNSET CHOICE
// [10] CONCERN: DEGENERATE CHOICE VALUES
// [ 9] CONCERN: SEQUENCE WITH ATTRIBUTES OF ALL VALUE CATEGORIES
// [ 8] CONCERN: CHOICES  WITH SELECTIONS OF ALL VALUE CATEGORIES
// [ 7] CONCERN: SEQUENCES WITH EMPTY ELEMENTS
// [ 6] CONCERN: LOG BUFFER CLEARS ON EACH `convert` TO   JSON CALL
// [ x] CONCERN: LOG BUFFER CLEARS ON EACH `convert` FROM JSON CALL
// [ 5] CONCERN: NULL ELEMENTS
// [ 4] CONCERN: COMPLEX TEST MESSAGES
// [ 3] CONCERN: CONSTRUCTOR AND ACCEPTED CATEGORIES
// [ 2] CONCERN: ENCODING ARRAYS
// [ 1] BREATHING TEST
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

#define ASSERT_EQ(X, Y) ASSERTV(X,Y,X == Y)
#define ASSERT_NE(X, Y) ASSERTV(X,Y,X != Y)
#define ASSERT_LE(X, Y) ASSERTV(X,Y,X <= Y)
#define ASSERT_GE(X, Y) ASSERTV(X,Y,X >= Y)

#define LOOP1_ASSERT_EQ(L,X,Y) ASSERTV(L,X,Y,X == Y)
#define LOOP1_ASSERT_NE(L,X,Y) ASSERTV(L,X,Y,X != Y)

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

#define ASSERT_FAIL(expr)      BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr)      BSLS_ASSERTTEST_ASSERT_PASS(expr)

#define ASSERT_OPT_FAIL(expr)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(expr)
#define ASSERT_OPT_PASS(expr)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(expr)

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef baljsn::JsonConverter  Obj;
typedef bdljsn::Json           Json;
typedef bdljsn::Error          Error;

typedef bdljsn::JsonNull       JsonNull;
typedef bdljsn::JsonNumber     JsonNumber;
typedef bdljsn::JsonArray      JsonArray;
typedef bdljsn::JsonObject     JsonObject;

typedef baljsn::Encoder        Encoder;
typedef baljsn::Decoder        Decoder;
typedef baljsn::EncoderOptions Options;
typedef baljsn::DecoderOptions Doptions;

bool g_veryVeryVerbose;

// ============================================================================
//                                TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(bslma::UsesBslmaAllocator<Obj>::value);

// ============================================================================
//                          TEST ENTITY DECLARATIONS
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace baljsn {
namespace encoder {
namespace u {

                      // ===============================
                      // NAMES FOR BLDAT TYPE GENERATION
                      // ===============================

extern const char k_ATTRIBUTE_1_NAME[]  = "a1";
extern const char k_ATTRIBUTE_2_NAME[]  = "a2";
extern const char k_ENUMERATOR_1_NAME[] = "e1";
extern const char k_ENUMERATOR_2_NAME[] = "e2";
extern const char k_SELECTION_1_NAME[]  = "s1";
extern const char k_SELECTION_2_NAME[]  = "s2";

extern const char k_EMPTY_STRING[] = "";

}  // close namespace u
}  // close namespace encoder
}  // close package namespace

namespace {
namespace u {

                               // ==============
                               // FREE FUNCTIONS
                               // ==============

/// Decode the sequence of `s_baltst::FeatureTestMessage` objects defined
/// by `s_baltst::FeatureTestMessage::s_XML_MESSAGES` as if by using
/// `balxml::Decoder` and load the sequence to the specified `objects`.
void constructFeatureTestMessage(
                            bsl::vector<s_baltst::FeatureTestMessage> *objects)
{
    balxml::MiniReader     reader;
    balxml::DecoderOptions options;
    balxml::ErrorInfo      e;
    balxml::Decoder        decoder(&options, &reader, &e);

    typedef s_baltst::FeatureTestMessageUtil MessageUtil;

    for (int i = 0; i < MessageUtil::k_NUM_MESSAGES; ++i) {
        s_baltst::FeatureTestMessage object;
        bsl::istringstream ss(MessageUtil::s_XML_MESSAGES[i]);

        balxml::MiniReader     reader;
        balxml::DecoderOptions options;
        balxml::ErrorInfo      e;
        balxml::Decoder        decoder(&options, &reader, &e);

        int rc = decoder.decode(ss.rdbuf(), &object);
        if (0 != rc) {
            bsl::cout << "Failed to decode from initialization data (i="
                      << i << "): "
                      << decoder.loggedMessages() << bsl::endl;
        }

        if (s_baltst::FeatureTestMessage::SELECTION_ID_UNDEFINED ==
                                                        object.selectionId()) {
            bsl::cout << "Decoded unselected choice from initialization data"
                      << " (i =" << i << ")"
                      << bsl::endl;
            rc = 9;
        }

        ASSERTV(i, 0 == rc); // test invariant

        objects->push_back(object);
    }
}

/// Decode the specified `xmlString` to an object of the specified `TYPE`
/// as if by using `balxml::Decoder` and load the object to the specified
/// `object`.  Return 0 on success, and a non-zero value otherwise.
template <class TYPE>
int populateTestObject(TYPE *object, const bsl::string& xmlString)
{
    bsl::istringstream ss(xmlString);

    balxml::MiniReader reader;
    balxml::DecoderOptions options;
    balxml::ErrorInfo e;
    balxml::Decoder decoder(&options, &reader, &e);

    int rc = decoder.decode(ss.rdbuf(), object);

    if (0 != rc) {
        bsl::cout << "Failed to decode from initialization data: "
                  << decoder.loggedMessages() << bsl::endl;
    }
    return rc;
}

                        // =============
                        // struct Oracle
                        // =============

struct Oracle {
    static int toJsonFromTxt(bdljsn::Json            *json,
                             bdljsn::Error           *errorDescription,
                             const bsl::string_view&  input);

    template <class TYPE>
    static int toJsonFromObj(bdljsn::Json *json,
                             const TYPE&   value);

    template <class TYPE>
    static int toObjFromJson(TYPE                *value,
                             const bdljsn::Json&  json,
                             bool                 skipUnknownElements);
};

                        // -------------
                        // struct Oracle
                        // -------------

int Oracle::toJsonFromTxt(bdljsn::Json            *json,
                          bdljsn::Error           *errorDescription,
                          const bsl::string_view&  input)
{
    BSLS_ASSERT(json);
    BSLS_ASSERT(errorDescription);

    return bdljsn::JsonUtil::read(json, errorDescription, input);
}

template <class TYPE>
int Oracle::toJsonFromObj(bdljsn::Json *json, const TYPE& value)
{
    ASSERT(json);

    if (g_veryVeryVerbose) {
        P(bsls::NameOf<TYPE>());
    }

    Options            options;  options.setEncodeEmptyArrays       (true);
                                 options.setEncodeNullElements      (true);
                                 options.setEncodeInfAndNaNAsStrings(true);
    Encoder            encoder;
    bsl::ostringstream oss;
    int                rc = encoder.encode(oss, value, options);

    if (g_veryVeryVerbose) {
        P(oss.str());
    }

    if (0 == rc) {
        return bdljsn::JsonUtil::read(json, oss.str());
    }

    return rc;
}

template <class TYPE>
int Oracle::toObjFromJson(TYPE                *value,
                          const bdljsn::Json&  json,
                          bool                 skipUnknownElements)
{
    ASSERT(value);

    if (g_veryVeryVerbose) {
        P(bsls::NameOf<TYPE>());
    }

    if (g_veryVeryVerbose) {
        P(json);
    }

    bsl::ostringstream oss;
    int                rc = bdljsn::JsonUtil::write(oss, json);

    if (g_veryVeryVerbose) {
        P(oss.str());
    }

    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    bsl::istringstream iss(oss.str());

    Decoder  decoder;
    Doptions options;  options.setSkipUnknownElements(skipUnknownElements);

    rc = decoder.decode(iss, value, options);
    return rc;
}

                               // ===============
                               // struct TestUtil
                               // ===============

static int  assertEncodedValueIsEqualCount = 0;

struct TestUtil {
    // CLASS METHODS

    /// `ASSERT` that converting the specified (`bdlat`-compatible) `VALUE`
    /// using `baljsn::JsonConverter` to the same `bdljsn::Json` value
    /// as that obtained from the oracle function.
    template <class VALUE_TYPE>
    static void assertEncodedValueIsEqual(int               LINE,
                                          const VALUE_TYPE& VALUE)
    {
        if (g_veryVeryVerbose) {
            P(LINE);
        }

        bdlsb::MemOutStreamBuf outStreamBuf;
        bsl::ostream           outStream(&outStreamBuf);

        Options options;
        options.setEncodeNullElements(true);
        options.setEncodeEmptyArrays (true);

        Encoder encoder;
        int     rc = encoder.encode(&outStreamBuf, VALUE, &options);
        LOOP1_ASSERT_EQ(LINE, 0, rc);
        if (0 != rc) {
            P_(encoder.loggedMessages());
        }

        const bsl::string_view jsonStringRef(outStreamBuf.data(),
                                             outStreamBuf.length());

        Json  expectJson;
        Error error;
        int   expectRc = u::Oracle::toJsonFromTxt(&expectJson,
                                                  &error,
                                                  jsonStringRef);
        ASSERTV(expectRc, 0       == expectRc);
        ASSERTV(error,    Error() == error);

        Obj  converter;
        Json actualJson;
        int  actualRc = converter.convert(&actualJson, VALUE);          // TEST
        ASSERTV(actualRc, 0 == actualRc);
        ASSERTV(      converter.loggedMessages(),
                "" == converter.loggedMessages());

        ASSERTV(expectJson,   actualJson,
                expectJson == actualJson);

       if (g_veryVeryVerbose) {
            P(expectJson);
            P(actualJson);
       }

        VALUE_TYPE valueFromJson;
        int        rcFromJson = converter.convert(&valueFromJson,
                                                  actualJson);          // TEST
        ASSERTV(rcFromJson, 0 == rcFromJson);
        ASSERTV(      converter.loggedMessages(),
                "" == converter.loggedMessages());

        ASSERTV(// VALUE,   valueFromJson,
                VALUE == valueFromJson);

        ++assertEncodedValueIsEqualCount;
        P(++assertEncodedValueIsEqualCount);
    }
};

                   // =======================================
                   // class AssertEncodedValueIsEqualFunction
                   // =======================================

class AssertEncodedValueIsEqualFunction {
  public:
    // CREATORS
    AssertEncodedValueIsEqualFunction()
    {
    }

    // ACCESSORS
    template <class VALUE_TYPE>
    void operator()(int               LINE,
                    const VALUE_TYPE& VALUE) const
    {
        TestUtil::assertEncodedValueIsEqual(LINE, VALUE);
    }
};

              // ===============================================
              // class AssertEncodedArrayOfValuesIsEqualFunction
              // ===============================================

class AssertEncodedArrayOfValuesIsEqualFunction {
  public:
    // CREATORS
    AssertEncodedArrayOfValuesIsEqualFunction() {}

    // ACCESSORS

    /// Assert that converting various nested and non-nested `bsl::vector`
    /// objects with copies of the specified `ELEMENT` as terminal
    /// element(s) using `baljsn::Coverter`.  The resulting `bdljsn::Json`
    /// object is tested for equality with one obtained from the oracle
    /// function.
    template <class ELEMENT_TYPE>
    void operator()(int                     LINE,
                    const ELEMENT_TYPE&     ELEMENT) const
                //  const bsl::string_view& EXPECTED_ELEMENT_STRING) const
    {
        const AssertEncodedValueIsEqualFunction       TEST;
        const s_baltst::TestPlaceHolder<ELEMENT_TYPE> e;
        const s_baltst::GenerateTestArray             a;

        const int              L = LINE;
        const ELEMENT_TYPE&    E = ELEMENT;

        //     LINE
        //    .----
        //    |
        //    |
        //    | ARRAY OBJECT
        //   -- -------------------------
        TEST(L, a(e)                   );
        TEST(L, a(E)                   );
        TEST(L, a(E,E)                 );
        TEST(L, a(E,E,E)               );
        TEST(L, a(a(e))                );
        TEST(L, a(a(E))                );
        TEST(L, a(a(E,E))              );
        TEST(L, a(a(E,E,E))            );
        TEST(L, a(a(e),a(e))           );
        TEST(L, a(a(e),a(E))           );
        TEST(L, a(a(e),a(E,E))         );
        TEST(L, a(a(e),a(E,E,E))       );
        TEST(L, a(a(E),a(e))           );
        TEST(L, a(a(E),a(E))           );
        TEST(L, a(a(E),a(E,E))         );
        TEST(L, a(a(E),a(E,E,E))       );
        TEST(L, a(a(E,E),a(e))         );
        TEST(L, a(a(E,E),a(E))         );
        TEST(L, a(a(E,E),a(E,E))       );
        TEST(L, a(a(E,E),a(E,E,E))     );
        TEST(L, a(a(E,E,E),a(e))       );
        TEST(L, a(a(E,E,E),a(E))       );
        TEST(L, a(a(E,E,E),a(E,E))     );
        TEST(L, a(a(E,E,E),a(E,E,E))   );
        TEST(L, a(a(e),a(e),a(e))      );
        TEST(L, a(a(e),a(e),a(E))      );
        TEST(L, a(a(e),a(e),a(E,E))    );
        TEST(L, a(a(e),a(E),a(e))      );
        TEST(L, a(a(e),a(E),a(E))      );
        TEST(L, a(a(e),a(E),a(E,E))    );
        TEST(L, a(a(e),a(E,E),a(e))    );
        TEST(L, a(a(e),a(E,E),a(E))    );
        TEST(L, a(a(e),a(E,E),a(E,E))  );
        TEST(L, a(a(E),a(e),a(e))      );
        TEST(L, a(a(E),a(e),a(E))      );
        TEST(L, a(a(E),a(e),a(E,E))    );
        TEST(L, a(a(E),a(E),a(e))      );
        TEST(L, a(a(E),a(E),a(E))      );
        TEST(L, a(a(E),a(E),a(E,E))    );
        TEST(L, a(a(E),a(E,E),a(e))    );
        TEST(L, a(a(E),a(E,E),a(E))    );
        TEST(L, a(a(E),a(E,E),a(E,E))  );
        TEST(L, a(a(E,E),a(e),a(e))    );
        TEST(L, a(a(E,E),a(e),a(E))    );
        TEST(L, a(a(E,E),a(e),a(E,E))  );
        TEST(L, a(a(E,E),a(E),a(e))    );
        TEST(L, a(a(E,E),a(E),a(E))    );
        TEST(L, a(a(E,E),a(E),a(E,E))  );
        TEST(L, a(a(E,E),a(E,E),a(e))  );
        TEST(L, a(a(E,E),a(E,E),a(E))  );
        TEST(L, a(a(E,E),a(E,E),a(E,E)));
    }
};

                             // ==================
                             // class Enumeration0
                             // ==================

class Enumeration0 {
  public:
    // CREATORS
    Enumeration0();
};

}  // close u namespace
}  // close unnamed namespace

template <>
struct bdlat_IsBasicEnumeration<u::Enumeration0> : bsl::true_type {
};

namespace bdlat_EnumFunctions {

template <>
struct IsEnumeration<u::Enumeration0> : public bsl::true_type {
};

}  // close bdlat_EnumFunctions namespace

namespace {
namespace u {

                     // ==================================
                     // class EmptySequenceExampleSequence
                     // ==================================

class EmptySequenceExampleSequence {

  public:
    // TYPES
    enum { NUM_ATTRIBUTES = 0 };

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();

    BSLA_MAYBE_UNUSED
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    BSLA_MAYBE_UNUSED
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS
    EmptySequenceExampleSequence();

    // MANIPULATORS
    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);

    template <class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    template <class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    template <class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    void reset();

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;

    template <class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    template <class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    template <class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
};

}  // close u namespace
}  // close unnamed namespace

// TRAITS
BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(
                                             u::EmptySequenceExampleSequence)

namespace {
namespace u {

                         // ==========================
                         // class EmptySequenceExample
                         // ==========================

class EmptySequenceExample {

  public:
    // TYPES
    enum { ATTRIBUTE_ID_SIMPLE_VALUE = 0, ATTRIBUTE_ID_SEQUENCE = 1 };

    enum { NUM_ATTRIBUTES = 2 };

    enum { ATTRIBUTE_INDEX_SIMPLE_VALUE = 0, ATTRIBUTE_INDEX_SEQUENCE = 1 };

    // CLASS DATA
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  private:
    // DATA
    int                          d_simpleValue;
    EmptySequenceExampleSequence d_sequence;

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();

    BSLA_MAYBE_UNUSED
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    BSLA_MAYBE_UNUSED
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS
    EmptySequenceExample();

    // MANIPULATORS
    BSLA_MAYBE_UNUSED
    EmptySequenceExample& operator=(const EmptySequenceExample& rhs);

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);

    template <class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    template <class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    template <class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    BSLA_MAYBE_UNUSED
    void reset();

    EmptySequenceExampleSequence& sequence();

    int& simpleValue();

    // ACCESSORS
    template <class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    template <class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

     template <class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;

    BSLA_MAYBE_UNUSED
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    const EmptySequenceExampleSequence& sequence() const;

    int simpleValue() const;
};

}  // close u namespace
}  // close unnamed namespace

// TRAITS
BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(u::EmptySequenceExample)

}  // close enterprise namespace

// ============================================================================
//                               HEXBINARY CLASSES
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace s_baltst {

                       // =============================
                       // class HexBinaryCustomizedType
                       // =============================

class HexBinaryCustomizedType {

    // INSTANCE DATA
    bsl::vector<char> d_value;

    // PRIVATE CLASS METHODS

    /// Check if the specified `value` having the specified `size` satisfies
    /// the restrictions of this class.  Return 0 if successful (i.e., the
    /// restrictions are satisfied) and non-zero otherwise.
    static int checkRestrictions(const char *value, int size);

    /// Check if the specified `value` satisfies the restrictions of this
    /// class.  Return 0 if successful (i.e., the restrictions are
    /// satisfied) and non-zero otherwise.
    static int checkRestrictions(const bsl::vector<char>& value);

  public:
    // TYPES
    typedef bsl::vector<char> BaseType;

    // CONSTANTS
    static const char CLASS_NAME[];

    // CREATORS

    /// Create an object of type `HexBinaryCustomizedType` having the
    /// default value.  Optionally specify a `basicAllocator` used to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    explicit HexBinaryCustomizedType(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `HexBinaryCustomizedType` having the value
    /// of the specified `original` object.  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is 0,
    /// the currently installed default allocator is used.
    HexBinaryCustomizedType(
                           const HexBinaryCustomizedType&  original,
                           bslma::Allocator               *basicAllocator = 0);

    /// Destroy this object.
    ~HexBinaryCustomizedType();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    HexBinaryCustomizedType& operator=(const HexBinaryCustomizedType& rhs);

    /// Convert from the specified `value` to this type.  Return 0 if
    /// successful and non-zero otherwise.
    int fromVector(const bsl::vector<char>& value);

    /// Reset this object to the default value (i.e., its value upon default
    /// construction).
    void reset();

    /// Return the array encapsulated by this object.
    bsl::vector<char>& array();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;

    /// Return the array encapsulated by this object.
    const bsl::vector<char>& array() const;

    const bsl::vector<char>& toVector() const;
        // Needed per `bdlat_customizedtypefunctions.h`.
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` attribute objects have
/// the same value, and `false` otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline
bool operator==(const HexBinaryCustomizedType& lhs,
                const HexBinaryCustomizedType& rhs);

/// Return `true` if the specified `lhs` and `rhs` attribute objects do not
/// have the same value, and `false` otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline
bool operator!=(const HexBinaryCustomizedType& lhs,
                const HexBinaryCustomizedType& rhs);

/// Format the specified `rhs` to the specified output `stream` and return a
/// reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const HexBinaryCustomizedType& rhs);

}  // close namespace s_baltst

// TRAITS

BDLAT_DECL_CUSTOMIZEDTYPE_TRAITS(test::HexBinaryCustomizedType)

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

namespace s_baltst {

                       // -----------------------------
                       // class HexBinaryCustomizedType
                       // -----------------------------

// PRIVATE CLASS METHODS
int HexBinaryCustomizedType::checkRestrictions(const char *, int)
{
    return 0;
}

int HexBinaryCustomizedType::checkRestrictions(const bsl::vector<char>&)
{
    return 0;
}

// CREATORS
inline
HexBinaryCustomizedType::HexBinaryCustomizedType(
    bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
HexBinaryCustomizedType::HexBinaryCustomizedType(
    const HexBinaryCustomizedType& original, bslma::Allocator *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

inline
HexBinaryCustomizedType::~HexBinaryCustomizedType()
{
}

// MANIPULATORS
inline
HexBinaryCustomizedType& HexBinaryCustomizedType::operator=(
                                            const HexBinaryCustomizedType& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

inline
int HexBinaryCustomizedType::fromVector(const bsl::vector<char>& value)
{
    int ret = checkRestrictions(value);
    if (0 == ret) {
        d_value = value;
    }

    return ret;
}

inline
void HexBinaryCustomizedType::reset()
{
    d_value.clear();
}

inline
bsl::vector<char>& HexBinaryCustomizedType::array()
{
    return d_value;
}

// ACCESSORS
bsl::ostream& HexBinaryCustomizedType::print(
                                            bsl::ostream& stream,
                                            int,
                                            int) const
{
    if (d_value.empty()) {
        stream << "";
    }
    else {
        stream << 'x'
               << '\''
               << bdlb::PrintStringSingleLineHexDumper(
                                 &d_value[0], static_cast<int>(d_value.size()))
               << '\'';
    }
    return stream;
}

inline
const bsl::vector<char>& HexBinaryCustomizedType::array() const
{
    return d_value;
}

inline
const bsl::vector<char>& HexBinaryCustomizedType::toVector() const
{
    return array();
}

// FREE FUNCTIONS
inline
bool operator==(const HexBinaryCustomizedType& lhs,
                const HexBinaryCustomizedType& rhs)
{
    const bsl::vector<char>& lhsArray = lhs.array();
    const bsl::vector<char>& rhsArray = rhs.array();

    if (lhsArray.size() != rhsArray.size()) {
        return false;                                                 // RETURN
    }

    for (size_t i = 0; i < lhsArray.size(); ++i) {
        if (lhsArray[i] != rhsArray[i]) {
            return false;                                             // RETURN
        }
    }

    return true;
}

inline
bool operator!=(const HexBinaryCustomizedType& lhs,
                const HexBinaryCustomizedType& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const HexBinaryCustomizedType& rhs)
{
    return rhs.print(stream, 0, -1);
}

// CONSTANTS

const char HexBinaryCustomizedType::CLASS_NAME[] = "HexBinaryCT";

                          // =======================
                          // class HexBinarySequence
                          // =======================

class HexBinarySequence {

    // INSTANCE DATA
    HexBinaryCustomizedType d_element1;

  public:
    // TYPES
    enum {
        k_ATTRIBUTE_ID_ELEMENT1 = 0
    };

    enum {
        k_NUM_ATTRIBUTES = 1
    };

    enum {
        k_ATTRIBUTE_INDEX_ELEMENT1 = 0
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type `HexBinarySequence` having the default
    /// value.
    explicit HexBinarySequence(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `HexBinarySequence` having the value of the
    /// specified `original` object.
    HexBinarySequence(const HexBinarySequence&  original,
                      bslma::Allocator         *basicAllocator = 0);

    /// Destroy this object.
    ~HexBinarySequence();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    HexBinarySequence& operator=(const HexBinarySequence& rhs);

    /// Reset this object to the default value (i.e., its value upon default
    /// construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the last
    /// invocation of `manipulator` (i.e., the invocation that terminated
    /// the sequence).
    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of the
    /// (modifiable) attribute indicated by the specified `id`, supplying
    /// `manipulator` with the corresponding attribute information
    /// structure.  Return the value returned from the invocation of
    /// `manipulator` if `id` identifies an attribute of this class, and -1
    /// otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of the
    /// (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Element1" attribute of this
    /// object.
    HexBinaryCustomizedType& element1();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor` with
    /// the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the last
    /// invocation of `accessor` (i.e., the invocation that terminated the
    /// sequence).
    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute of
    /// this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute of
    /// this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return a reference to the modifiable "Element1" attribute of this
    /// object.
    const HexBinaryCustomizedType& element1() const;

};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` attribute objects have
/// the same value, and `false` otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline
bool operator==(const HexBinarySequence& lhs, const HexBinarySequence& rhs);

/// Return `true` if the specified `lhs` and `rhs` attribute objects do not
/// have the same value, and `false` otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline
bool operator!=(const HexBinarySequence& lhs, const HexBinarySequence& rhs);

/// Format the specified `rhs` to the specified output `stream` and return a
/// reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream& stream, const HexBinarySequence& rhs);

}  // close namespace s_baltst

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(test::HexBinarySequence)

namespace s_baltst {

                          // -----------------------
                          // class HexBinarySequence
                          // -----------------------

// MANIPULATORS
template <class MANIPULATOR>
int HexBinarySequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1,
                      ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int HexBinarySequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { e_NOT_FOUND = -1 };

    switch (id) {
      case k_ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1,
                           ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      default:
        return e_NOT_FOUND;                                           // RETURN
    }
}

template <class MANIPULATOR>
int HexBinarySequence::manipulateAttribute(MANIPULATOR&  manipulator,
                                           const char   *name,
                                           int           nameLength)
{
    enum { e_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return e_NOT_FOUND;                                           // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
HexBinaryCustomizedType& HexBinarySequence::element1()
{
    return d_element1;
}

// ACCESSORS
template <class ACCESSOR>
int HexBinarySequence::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1,
                   ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int HexBinarySequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { e_NOT_FOUND = -1 };

    switch (id) {
      case k_ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1,
                        ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      default:
        return e_NOT_FOUND;                                           // RETURN
    }
}

template <class ACCESSOR>
int HexBinarySequence::accessAttribute(ACCESSOR&   accessor,
                                       const char *name,
                                       int         nameLength) const
{
    enum { e_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return e_NOT_FOUND;                                            // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const HexBinaryCustomizedType& HexBinarySequence::element1() const
{
    return d_element1;
}

// FREE FUNCTIONS

inline
bool operator==(const HexBinarySequence& lhs, const HexBinarySequence& rhs)
{
    return lhs.element1() == rhs.element1();
}

inline
bool operator!=(const HexBinarySequence& lhs, const HexBinarySequence& rhs)
{
    return !(lhs == rhs);
}

                          // -----------------------
                          // class HexBinarySequence
                          // -----------------------

// CONSTANTS

const char HexBinarySequence::CLASS_NAME[] = "HexBinarySequence";

const bdlat_AttributeInfo HexBinarySequence::ATTRIBUTE_INFO_ARRAY[] = {
    {
        k_ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *HexBinarySequence::lookupAttributeInfo(
                                                        const char *name,
                                                        int         nameLength)
{
    if (nameLength < 8) {
        return 0;                                                     // RETURN
    }
    if (name[0]=='e'
     && name[1]=='l'
     && name[2]=='e'
     && name[3]=='m'
     && name[4]=='e'
     && name[5]=='n'
     && name[6]=='t'
     && name[7]=='1') {
        return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_ELEMENT1];     // RETURN
    }
    return 0;
}

const bdlat_AttributeInfo *HexBinarySequence::lookupAttributeInfo(int id)
{
    switch (id) {
      case k_ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_ELEMENT1];
      default:
        return 0;
    }
}

// CREATORS

HexBinarySequence::HexBinarySequence(bslma::Allocator *basicAllocator)
: d_element1(basicAllocator)
{
}

HexBinarySequence::HexBinarySequence(const HexBinarySequence&  original,
                                     bslma::Allocator         *basicAllocator)
: d_element1(original.d_element1, basicAllocator)
{
}

HexBinarySequence::~HexBinarySequence()
{
}

// MANIPULATORS

HexBinarySequence&
HexBinarySequence::operator=(const HexBinarySequence& rhs)
{
    if (this != &rhs) {
        d_element1 = rhs.d_element1;
    }
    return *this;
}

void HexBinarySequence::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_element1);
}

// ACCESSORS

bsl::ostream& HexBinarySequence::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;
    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element1 = ";
        bdlb::PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Element1 = ";
        bdlb::PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

bsl::ostream& operator<<(bsl::ostream&            stream,
                         const HexBinarySequence& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace s_baltst
}  // close enterprise namespace

// ============================================================================
//                               COLOR/PALETTE
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace s_baltst {
                                // ============
                                // class Colors
                                // ============

struct Colors {

  public:
    // TYPES
    enum Value {
        BLUE_QUOTE_YELLOW                                               = 0
      , GREY_BACKSLASH_BLUE                                             = 1
      , RED_SLASH_GREEN                                                 = 2
      , RED_BACKSLASH_B_BACKSLASH_R_BACKSLASH_T_BACKSLASH_F_BACKSLASH_N = 3
      , WHITE_TICK_BLACK                                                = 4
    };

    enum {
        NUM_ENUMERATORS = 5
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];

    // CLASS METHODS

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration `value`.
    static const char *toString(Value value);

    /// Load into the specified `result` the enumerator matching the
    /// specified `string` of the specified `stringLength`.  Return 0 on
    /// success, and a non-zero value with no effect on `result` otherwise
    /// (i.e., `string` does not match any enumerator).
    static int fromString(Value        *result,
                          const char   *string,
                          int           stringLength);

    /// Load into the specified `result` the enumerator matching the
    /// specified `string`.  Return 0 on success, and a non-zero value with
    /// no effect on `result` otherwise (i.e., `string` does not match any
    /// enumerator).
    static int fromString(Value              *result,
                          const bsl::string&  string);

    /// Load into the specified `result` the enumerator matching the
    /// specified `number`.  Return 0 on success, and a non-zero value with
    /// no effect on `result` otherwise (i.e., `number` does not match any
    /// enumerator).
    static int fromInt(Value *result, int number);

    /// Write to the specified `stream` the string representation of
    /// the specified enumeration `value`.  Return a reference to
    /// the modifiable `stream`.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

// FREE OPERATORS

/// Format the specified `rhs` to the specified output `stream` and
/// return a reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream& stream, Colors::Value rhs);

}  // close namespace s_baltst

// TRAITS

BDLAT_DECL_ENUMERATION_TRAITS(test::Colors)

namespace s_baltst {

                               // =============
                               // class Palette
                               // =============

class Palette {

    // INSTANCE DATA
    bsl::vector<Colors::Value>  d_colors;
    Colors::Value               d_color;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_COLOR  = 0
      , ATTRIBUTE_ID_COLORS = 1
    };

    enum {
        NUM_ATTRIBUTES = 2
    };

    enum {
        ATTRIBUTE_INDEX_COLOR  = 0
      , ATTRIBUTE_INDEX_COLORS = 1
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);

    // CREATORS

    /// Create an object of type `Palette` having the default value.
    /// Optionally specify a `basicAllocator` used to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    explicit Palette(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `Palette` having the value of the specified
    /// `original` object.  Optionally specify a `basicAllocator` used to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    Palette(const Palette& original,
            bslma::Allocator *basicAllocator = 0);

    /// Destroy this object.
    ~Palette();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    Palette& operator=(const Palette& rhs);

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `manipulator` (i.e., the invocation that
    /// terminated the sequence).
    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `id`,
    /// supplying `manipulator` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if `id` identifies an attribute of this
    /// class, and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Color" attribute of this
    /// object.
    Colors::Value& color();

    /// Return a reference to the modifiable "Colors" attribute of this
    /// object.
    bsl::vector<Colors::Value>& colors();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `accessor` (i.e., the invocation that terminated
    /// the sequence).
    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return a reference to the non-modifiable "Color" attribute of this
    /// object.
    Colors::Value color() const;

    /// Return a reference to the non-modifiable "Colors" attribute of this
    /// object.
    const bsl::vector<Colors::Value>& colors() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` attribute objects have
/// the same value, and `false` otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline
bool operator==(const Palette& lhs, const Palette& rhs);

/// Return `true` if the specified `lhs` and `rhs` attribute objects do not
/// have the same value, and `false` otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline
bool operator!=(const Palette& lhs, const Palette& rhs);

/// Format the specified `rhs` to the specified output `stream` and
/// return a reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream& stream, const Palette& rhs);

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(test::Palette)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                                // ------------
                                // class Colors
                                // ------------

// CLASS METHODS
inline
int Colors::fromString(Value *result, const bsl::string& string)
{
    return fromString(result,
                      string.c_str(),
                      static_cast<int>(string.length()));
}

inline
bsl::ostream& Colors::print(bsl::ostream&      stream,
                                 Colors::Value value)
{
    return stream << toString(value);
}

                               // -------------
                               // class Palette
                               // -------------

// CLASS METHODS
template <class MANIPULATOR>
int Palette::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_color, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLOR]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_colors, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLORS]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int Palette::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_COLOR: {
        return manipulator(&d_color,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLOR]);
      } break;
      case ATTRIBUTE_ID_COLORS: {
        return manipulator(&d_colors,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLORS]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int Palette::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
Colors::Value& Palette::color()
{
    return d_color;
}

inline
bsl::vector<Colors::Value>& Palette::colors()
{
    return d_colors;
}

// ACCESSORS
template <class ACCESSOR>
int Palette::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_color, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLOR]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_colors, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLORS]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int Palette::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_COLOR: {
        return accessor(d_color, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLOR]);
      } break;
      case ATTRIBUTE_ID_COLORS: {
        return accessor(d_colors,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLORS]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int Palette::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
Colors::Value Palette::color() const
{
    return d_color;
}

inline
const bsl::vector<Colors::Value>& Palette::colors() const
{
    return d_colors;
}

}  // close package namespace

// FREE FUNCTIONS

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        test::Colors::Value rhs)
{
    return test::Colors::print(stream, rhs);
}

inline
bool test::operator==(
        const test::Palette& lhs,
        const test::Palette& rhs)
{
    return  lhs.color() == rhs.color()
         && lhs.colors() == rhs.colors();
}

inline
bool test::operator!=(
        const test::Palette& lhs,
        const test::Palette& rhs)
{
    return  lhs.color() != rhs.color()
         || lhs.colors() != rhs.colors();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::Palette& rhs)
{
    return rhs.print(stream, 0, -1);
}

namespace s_baltst {

                                // ------------
                                // class Colors
                                // ------------

// CONSTANTS

const char Colors::CLASS_NAME[] = "Colors";

const bdlat_EnumeratorInfo Colors::ENUMERATOR_INFO_ARRAY[] = {
    {
        Colors::BLUE_QUOTE_YELLOW,
        "BLUE\"YELLOW",
        sizeof("BLUE\"YELLOW") - 1,
        ""
    },
    {
        Colors::GREY_BACKSLASH_BLUE,
        "GREY\\BLUE",
        sizeof("GREY\\BLUE") - 1,
        ""
    },
    {
        Colors::RED_SLASH_GREEN,
        "RED/GREEN",
        sizeof("RED/GREEN") - 1,
        ""
    },
    {
       Colors::RED_BACKSLASH_B_BACKSLASH_R_BACKSLASH_T_BACKSLASH_F_BACKSLASH_N,
        "RED\b\r\t\f\n",
        sizeof("RED\b\r\t\f\n") - 1,
        ""
    },
    {
        Colors::WHITE_TICK_BLACK,
        "WHITE'BLACK",
        sizeof("WHITE'BLACK") - 1,
        ""
    }
};

// CLASS METHODS

int Colors::fromInt(Colors::Value *result, int number)
{
    switch (number) {
      case Colors::BLUE_QUOTE_YELLOW:
      case Colors::GREY_BACKSLASH_BLUE:
      case Colors::RED_SLASH_GREEN:
      case
       Colors::RED_BACKSLASH_B_BACKSLASH_R_BACKSLASH_T_BACKSLASH_F_BACKSLASH_N:
      case Colors::WHITE_TICK_BLACK:
        *result = (Colors::Value)number;
        return 0;
      default:
        return -1;
    }
}

int Colors::fromString(
        Colors::Value *result,
        const char         *string,
        int                 stringLength)
{
    for (int i = 0; i < 5; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo =
                    Colors::ENUMERATOR_INFO_ARRAY[i];

        if (stringLength == enumeratorInfo.d_nameLength
        &&  0 == bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength))
        {
            *result = (Colors::Value)enumeratorInfo.d_value;
            return 0;
        }
    }

    return -1;
}

const char *Colors::toString(Colors::Value value)
{
    switch (value) {
      case BLUE_QUOTE_YELLOW: {
        return "BLUE\"YELLOW";
      } break;
      case GREY_BACKSLASH_BLUE: {
        return "GREY\\BLUE";
      } break;
      case RED_SLASH_GREEN: {
        return "RED/GREEN";
      } break;
      case RED_BACKSLASH_B_BACKSLASH_R_BACKSLASH_T_BACKSLASH_F_BACKSLASH_N: {
        return "RED\b\r\t\f\n";
      } break;
      case WHITE_TICK_BLACK: {
        return "WHITE'BLACK";
      } break;
    }

    BSLS_ASSERT(0 == "invalid enumerator");
    return 0;
}

                               // -------------
                               // class Palette
                               // -------------

// CONSTANTS

const char Palette::CLASS_NAME[] = "Palette";

const bdlat_AttributeInfo Palette::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_COLOR,
        "color",
        sizeof("color") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_COLORS,
        "colors",
        sizeof("colors") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Palette::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    Palette::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *Palette::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_COLOR:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLOR];
      case ATTRIBUTE_ID_COLORS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLORS];
      default:
        return 0;
    }
}

// CREATORS

Palette::Palette(bslma::Allocator *basicAllocator)
: d_colors(basicAllocator)
, d_color(static_cast<Colors::Value>(0))
{
}

Palette::Palette(const Palette& original,
                 bslma::Allocator *basicAllocator)
: d_colors(original.d_colors, basicAllocator)
, d_color(original.d_color)
{
}

Palette::~Palette()
{
}

// MANIPULATORS

Palette&
Palette::operator=(const Palette& rhs)
{
    if (this != &rhs) {
        d_color = rhs.d_color;
        d_colors = rhs.d_colors;
    }

    return *this;
}

void Palette::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_color);
    bdlat_ValueTypeFunctions::reset(&d_colors);
}

// ACCESSORS

bsl::ostream& Palette::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("color", d_color);
    printer.printAttribute("colors", d_colors);
    printer.end();
    return stream;
}

}  // close namespace s_baltst
}  // close enterprise namespace

// ============================================================================
//                               BDLAT TEST TYPES
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace s_baltst {

                             // ==================
                             // class Enumeration0
                             // ==================

class Enumeration0 {

  public:
    Enumeration0()
    {
    }
};

int bdlat_enumFromInt(Enumeration0 *, int number)
{
    if (0 == number) {
        return 0;                                                     // RETURN
    }

    return -1;
}

int bdlat_enumFromString(Enumeration0 *, const char *string,
                         int stringLength)
{
    const bsl::string_view stringRef(string, stringLength);

    if ("zero" == stringRef) {
        return 0;                                                     // RETURN
    }

    return -1;
}

void bdlat_enumToInt(int *result, const Enumeration0&)
{
    *result = 0;
}

void bdlat_enumToString(bsl::string *result, const Enumeration0&)
{
    *result = "zero";
}

}  // close namespace s_baltst

// TRAITS
template <>
struct bdlat_IsBasicEnumeration<test::Enumeration0> : bsl::true_type {};

namespace bdlat_EnumFunctions {

template <>
struct IsEnumeration<test::Enumeration0> : public bsl::true_type {
};

}  // close bdlat_EnumFunctions namespace
namespace s_baltst {

                             // ==================
                             // class Enumeration1
                             // ==================

class Enumeration1 {
  private:
    int d_value;

    friend int bdlat_enumFromInt(Enumeration1 *dest, int val)
    {
        if (val == 0 || val == 1) {
            dest->d_value = val;
            return 0;                                                 // RETURN
        }
        return -1;
    }

    friend int bdlat_enumFromString(Enumeration1 *, const char *, int)
    {
        bsl::cout << "should not be called\n";
        ASSERT(false);
        return -1;
    }

    friend void bdlat_enumToInt(int *result, const Enumeration1& src)
    {
        *result = src.d_value;
    }

    friend void bdlat_enumToString(bsl::string         *result,
                                   const Enumeration1&  src)
    {
        if (src.d_value == 0) {
            *result = "0";
        } else {
            ASSERT(1 == src.d_value);
            *result = "1";
        }
    }
};
}  // close namespace s_baltst
namespace bdlat_EnumFunctions {
template <>
struct IsEnumeration<test::Enumeration1> : public bsl::true_type {
};
}  // close namespace bdlat_EnumFunctions
}  // close enterprise namespace

namespace BloombergLP {
namespace baljsn {
namespace decoder {
namespace u {

                      // ===============================
                      // NAMES FOR BLDAT TYPE GENERATION
                      // ===============================

extern const char k_ATTRIBUTE_1_NAME[]  = "a1";
extern const char k_ATTRIBUTE_2_NAME[]  = "a2";
extern const char k_ATTRIBUTE_3_NAME[]  = "a3";
extern const char k_ENUMERATOR_1_NAME[] = "e1";
extern const char k_ENUMERATOR_2_NAME[] = "e2";
extern const char k_SELECTION_1_NAME[]  = "s1";
extern const char k_SELECTION_2_NAME[]  = "s2";

extern const char k_EMPTY_STRING[] = "";

}  // close namespace u
}  // close namespace encoder
}  // close package namespace
}  // close enterprise namespace

// ============================================================================
//                               SUPPORT FOR FLOATING POINT TEST
// ----------------------------------------------------------------------------

                        // =============================
                        // template SelectEncodeableType
                        // =============================

template <class TESTED_TYPE> struct SelectEncodeableType;

template <> struct SelectEncodeableType<float> {
    typedef s_baltst::SqrtF Type;
};

template <> struct SelectEncodeableType<double> {
    typedef s_baltst::Sqrt Type;
};

                        // ======================================
                        // template roundTripTestNonNumericValues
                        // ======================================

template <class FLOAT_TYPE>
void roundTripTestNonNumericValues()
{
    if (g_veryVeryVerbose) {
        P(bsls::NameOf<FLOAT_TYPE>());
    }

    typedef          FLOAT_TYPE                             Type;
    typedef typename SelectEncodeableType<FLOAT_TYPE>::Type TestType;
    typedef          bsl::numeric_limits<FLOAT_TYPE>        Limits;

    if (g_veryVeryVerbose) {
        P(bsls::NameOf<TestType>());
    }

    const Type NAN_P = Limits::quiet_NaN();
    // Negative NaN does not print for any floating point type, so we
    // don't test it for round-trip (on purpose).
    //const Type NAN_N = -NAN_P;
    const Type INF_P = Limits::infinity();
    const Type INF_N = -INF_P;

    const struct {
        int         d_line;
        Type        d_value;
    } DATA[] = {
        //---------------
        // LINE | VALUE |
        //---------------
        { L_,    NAN_P },
        // Negative NaN does not print for any floating point type,
        // so we don't test it for round-trip (on purpose).
     // { L_,    NAN_N },
        { L_,    INF_P },
        { L_,    INF_N },
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int  LINE  = DATA[ti].d_line;
        const Type VALUE = DATA[ti].d_value;

        if (g_veryVeryVerbose) {
            P_(ti); P_(LINE); P(VALUE);
        }

        TestType toEncode;
        toEncode.value() = VALUE;

        Obj  converter;
        Json json;
        Q(convertTo);
        ASSERTV(LINE, converter.loggedMessages(),
                0  == converter.convert(&json, toEncode));              // TEST

        if (g_veryVeryVerbose) {
            P(json);
        }

        TestType  decoded;
        decoded.value() = Limits::quiet_NaN(); // A value we don't use

        ASSERTV(LINE, converter.loggedMessages(),
                0  == converter.convert(&decoded, json));               // TEST
        Type DECODED = decoded.value();

        if (VALUE != VALUE) { // A NaN
            ASSERTV(LINE, json, VALUE, DECODED, DECODED != DECODED);
        }
        else {
            ASSERTV(LINE, json, VALUE, DECODED, VALUE == DECODED);
        }
        // We also use `memcmp` to ensure that we really get back the
        // same binary IEEE-754.
        ASSERTV(LINE, json, VALUE, DECODED,
                0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));
    }
}

// ============================================================================
//                      TEST `skipUnknownElements`
// ----------------------------------------------------------------------------

template <class TYPE>
void testSkipUnknownElements(int line, const TYPE& obj)
{

    if (g_veryVeryVerbose) {
        P_(line); P(bsls::NameOf<TYPE>());
    }

    Json jsonExpect;
    int rcExpect = u::Oracle::toJsonFromObj(&jsonExpect, obj);
    ASSERTV(rcExpect, 0 == rcExpect);

    if (!rcExpect) {
        return;
    }

    TYPE objExpect;
    Obj  converter;
    rcExpect = converter.convert(&objExpect, jsonExpect);
    ASSERTV(rcExpect, 0 == rcExpect);

    if (!rcExpect) {
        return;
    }

    Json extraSimpleValue("extraSimpleValue");

    Json extraObject;
    extraObject.makeObject();
    extraObject["moe"  ] = true;
    extraObject["larry"] = 1.0;
    extraObject["curly"] = "Jerome";
    extraObject["shemp"] = bdljsn::JsonNull();

    Json extraArray;
    extraArray.makeArray();
    extraArray.theArray().pushBack(Json(false));
    extraArray.theArray().pushBack(Json(2.0));
    extraArray.theArray().pushBack(Json("Jerry"));
    extraArray.theArray().pushBack(Json(bdljsn::JsonNull()));

    struct {
        int         d_line;
        const Json *d_json_p;
        const char *d_name_p;
    } DATA[] = {
    //  LINE JSON_EXTRA         NAME
    //  ---- -----------------  ------------------
      { L_,  &extraSimpleValue, "extraSimpleValue" }
    , { L_,  &extraArray,       "extraArray"       }
    , { L_,  &extraObject,      "extraObject"      }
    };

    const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    for (bsl::size_t i  = 0; i < NUM_DATA; ++i) {
        const int         LINE       =  DATA[i].d_line;
        const Json&       JSON_EXTRA = *DATA[i].d_json_p;
        const char *const NAME       =  DATA[i].d_name_p;

        if (g_veryVeryVerbose) {
            P_(LINE); P_(NAME); P(JSON_EXTRA);
        }

        Json jsonWithExtra  = jsonExpect;  // Initial object.
        jsonWithExtra[NAME] = JSON_EXTRA;  // Add extraneous data.

        if (g_veryVeryVerbose) {
            Q(omit options: expect to skip extra; expect to pass);
        }

        TYPE objFromJsonWithExtraOracleSkip;
        int rcOracleSkip = u::Oracle::toObjFromJson(
                                               &objFromJsonWithExtraOracleSkip,
                                               jsonWithExtra,
                                               true);
        TYPE objFromJsonWithExtraOracleFlop;
        int rcOracleFlop = u::Oracle::toObjFromJson(
                                               &objFromJsonWithExtraOracleFlop,
                                               jsonWithExtra,
                                               false);

        TYPE objFromJsonWithExtra0;
        int   rcFromJsonWithExtra0 = converter.convert(                 // TEST
                                                &objFromJsonWithExtra0,
                                                jsonWithExtra);
                                             // default option: skip is `true`.

        ASSERTV(rcOracleSkip,    rcFromJsonWithExtra0,
                rcOracleSkip  == rcFromJsonWithExtra0);

        if (0 == rcOracleSkip) {
            ASSERTV(objFromJsonWithExtraOracleSkip,   objFromJsonWithExtra0,
                    objFromJsonWithExtraOracleSkip == objFromJsonWithExtra0);
        }

        Doptions options;   // default decode options
        ASSERTV(        options.skipUnknownElements(),
                true == options.skipUnknownElements());

        if (g_veryVeryVerbose) {
            Q(specify default options: expect to skip extra; expect to pass);
        }

        TYPE objFromJsonWithExtra1;
        int   rcFromJsonWithExtra1 = converter.convert(                 // TEST
                                                &objFromJsonWithExtra1,
                                                jsonWithExtra,
                                                options);

        ASSERTV(rcOracleSkip,    rcFromJsonWithExtra1,
                rcOracleSkip  == rcFromJsonWithExtra1);

        if (0 == rcOracleSkip) {
            ASSERTV(objFromJsonWithExtraOracleSkip,   objFromJsonWithExtra1,
                    objFromJsonWithExtraOracleSkip == objFromJsonWithExtra1);
        }

        options.setSkipUnknownElements(false);
        ASSERTV(         options.skipUnknownElements(),
                false == options.skipUnknownElements());

        if (g_veryVeryVerbose) {
            Q(specify: to not skip: expect error);
        }

        TYPE objFromJsonWithExtra2;
        int   rcFromJsonWithExtra2 = converter.convert(                 // TEST
                                                &objFromJsonWithExtra2,
                                                jsonWithExtra,
                                                options);

        ASSERTV(rcOracleFlop,    rcFromJsonWithExtra2,
                rcOracleFlop  == rcFromJsonWithExtra2);

        if (0 == rcOracleFlop) {
            ASSERTV(objFromJsonWithExtraOracleFlop,   objFromJsonWithExtra2,
                    objFromJsonWithExtraOracleFlop == objFromJsonWithExtra2);
        }
    }
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4; g_veryVeryVerbose = veryVeryVerbose;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 21: {
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

        if (verbose)
            cout << endl
                 << "USAGE EXAMPLE" << endl
                 << "=============" << endl;

        namespace test = ::BloombergLP::s_baltst;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Encoding a `bas_codegen.pl`-generated object into JSON
///-----------------------------------------------------------------
// Consider that we want to exchange an employee's information between two
// processes.  To allow this information exchange we will define the XML schema
// representation for that class, use `bas_codegen.pl` to create the `Employee`
// `class` for storing that information, populate an `Employee` object, and
// encode that object using the baljsn encoder.
//
// First, we will define the XML schema inside a file called `employee.xsd`:
// ```
//  <?xml version='1.0' encoding='UTF-8'?>
//  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
//             xmlns:test='http://bloomberg.com/schemas/test'
//             targetNamespace='http://bloomberg.com/schemas/test'
//             elementFormDefault='unqualified'>
//
//      <xs:complexType name='Address'>
//          <xs:sequence>
//              <xs:element name='street' type='xs:string'/>
//              <xs:element name='city'   type='xs:string'/>
//              <xs:element name='state'  type='xs:string'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name='Employee'>
//          <xs:sequence>
//              <xs:element name='name'        type='xs:string'/>
//              <xs:element name='homeAddress' type='test:Address'/>
//              <xs:element name='age'         type='xs:int'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:element name='Employee' type='test:Employee'/>
//
//  </xs:schema>
// ```
// Then, we will use the `bas_codegen.pl` tool, to generate the C++ classes for
// this schema.  The following command will generate the header and
// implementation files for the all the classes in the `test_messages`
// components in the current directory:
// ```
//  $ bas_codegen.pl -m msg -p test xsdfile.xsd
// ```
// Next, we will populate a `test::Employee` object:
// ```
    test::Employee employee;
    employee.name()                 = "Bob";
    employee.homeAddress().street() = "Lexington Ave";
    employee.homeAddress().city()   = "New York City";
    employee.homeAddress().state()  = "New York";
    employee.age()                  = 21;
// ```
// Then, we will create a `baljsn::JsonConverter` object:
// ```
    baljsn::JsonConverter converter;
// ```
// Now, we will create a `bdljsn::Json` object having elements that
// match the respecitve elements of `employee`.
// ```
    bdljsn::Json json;
    int          rc = converter.convert(&json, employee);
    ASSERT(0  == rc);
    ASSERT("" == converter.loggedMessages());
// ```
// Next, we verify that the `json` object has the expected elements, each
// containing the expected value, and having the expected type.
// ```
    ASSERT(employee.name()                 == json["name"].theString());
    ASSERT(employee.homeAddress().street() == json["homeAddress"]["street"]
                                                                 .theString());
    ASSERT(employee.homeAddress().city()   == json["homeAddress"]["city"]
                                                                 .theString());
    ASSERT(employee.homeAddress().state()  == json["homeAddress"]["state"]
                                                                 .theString());
    int intValue;                        rc = json["age"]
                                                 .theNumber().asInt(&intValue);
    ASSERT(0  == rc);
    ASSERT("" == converter.loggedMessages());
    ASSERT(employee.age()                  == intValue);
// ```
// Finally, we verify that the `json` object can be converted back to an
// `Employee` object having the same value as the original:
// ```
    test::Employee employeeFromJson;
    rc = converter.convert(&employeeFromJson, json);
    ASSERT(0        == rc);
    ASSERT(""       == converter.loggedMessages());
    ASSERT(employee == employeeFromJson);
// ```
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING `Decimal64`
        //   The converter can set a `bdljsn::Json` object to hold the same
        //   value as a `bdlat`-compatible object having type
        //   `bdldfp::Decimal64` values.  That `Json` object can be used to
        //   set another `bdlat`-compatible object having the same type as
        //   the source object to the same value as that source object.
        //
        // Concerns:
        // 1. A `Json` object can be set from a `bdlat`-compatible, source
        //    object containing any `bdldfp::Decimal64` value, including all
        //    limit values of that type.  The resulting `Json` object having
        //    type `isNumber()` except as described in C-2 below.
        //
        // 2. The source values can be positive infinity, negative infinity,
        //    and not-a-number and result in `"+inf"`, `"-inf"`, and
        //    `"nan`" in the `Json` object that has type `isString()`.
        //
        // 4. The converter returns zero for successful conversions and a
        //    non-zero value for invalid input.
        //
        // 5. A zero return value implies that an immediate call to the
        //    `loggedMessages()` method returns an empty string.
        //
        // 6. A `Json` object set by `convert` can be used to set an other
        //    object of the same type as the source object to the same value
        //    as that source object.
        //
        // Plan:
        // 1. This test case uses a series of table-driven tests where the
        //    tables are replicated from the `baljsn_parserutil.t.cpp` test
        //    cases of the `bdldfp::Decimal64` overload of
        //    `ParserUtil::getValue`, a function that converts text to a
        //    numeric value.
        //
        // 2. Each of the table input values are used to synthesize a JSON
        //    document representing
        //    `s_baltst::MySequenceWithPrecisionDecimalAttribute`.  The single
        //    attribute of that class holds a `bdldfp::Decimal64` value.
        //
        //    * Depending on the table, the input value is either used directly
        //      or converted to a textual representation using lower-level
        //      components.
        //
        //    * Note that two table entries were considered invalid in their
        //      original table but classified as valid below.  Those entries
        //      had trailing characters that cause failure if one tries to
        //      convert them Decimal64 but when used in a JSON document, they
        //      cause no error -- the normal decoder tokenization avoids them.
        //
        // 2. Each of the *valid* table input values are used to initialize a
        //    `s_baltst::MySequenceWithPrecisionDecimalAttribute` object --
        //    the "test object".  The single attribute of that class holds a
        //    `bdldfp::Decimal64` value.
        //
        //    * Depending on the table, the input value is either used directly
        //      or converted to a textual representation using lower-level
        //      components.
        //
        //    * The textual representation of the value is used to synthesize
        //      a JSON document representing the expected `Json` object.
        //
        // 3. Each test object is passed to the `convert` method.  The return
        //    value, the result of `loggedMessages`, and JSON
        //    document of the loaded `Json` object are compared to their
        //    expected values.
        //
        // 4. Each loaded `Json` object is passed the other `convert` overload
        //    to copy its value to to an object having the same type as
        //    the original test object.  The type loaded from the `Json` object
        //    and the original target object are compared for equality.
        //
        // Testing:
        //   TESTING `Decimal64`
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING `Decimal64`" <<
                             "\n===================" << endl;

        typedef s_baltst::MySequenceWithPrecisionDecimalAttribute SeqDec64;
        typedef baljsn::PrintUtil                                 Print;
        typedef baljsn::EncoderOptions                            EOptions;
        typedef bdldfp::Decimal64                                 Decimal64;
        typedef bdldfp::DecimalUtil                               DecUtil;
        typedef baljsn::ParserUtil                                ParUtil;
        typedef bdljsn::JsonUtil                                  JsnUtil;

        if (verbose) cout << endl << "Regular values and limits" << endl;
        {
#define DEC(X) BDLDFP_DECIMAL_DD(X)

            typedef bsl::numeric_limits<Decimal64> Limits;

            const struct {
                int       d_line;
                Decimal64 d_value;
            } DATA[] = {
                //LINE  VALUE
                //----  -----------------------------
                {L_,    DEC( 0.0),                    },
                {L_,    DEC(-0.0),                    },
                {L_,    DEC( 1.13),                   },
                {L_,    DEC(-9.876543210987654e307)   },
                {L_,    DEC(-9.8765432109876548e307)  },
                {L_,    DEC(-9.87654321098765482e307) },

                // Boundary values
                { L_,    Limits::min()        },
                { L_,    Limits::denorm_min() },
                { L_,    Limits::max()        },
                { L_,   -Limits::min()        },
                { L_,   -Limits::denorm_min() },
                { L_,   -Limits::max()        },
            };
            const int NUM_DATA = sizeof DATA / sizeof DATA[0];

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int       LINE  = DATA[ti].d_line;
                const Decimal64 VALUE = DATA[ti].d_value;

                if (veryVerbose) {
                    P_(LINE); P(VALUE);
                }

                // Create source object.
                SeqDec64 seqDec64;
                seqDec64.attribute1() = VALUE;

                if (veryVeryVerbose) {
                    P(seqDec64);
                }

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Print::printValue(oss, VALUE));

                bsl::string result(oss.str());

                // Synthesize expected JSON Doc(oss.str());.
                bsl::string expectJsonDoc;
                expectJsonDoc.append("{");
                expectJsonDoc.append("\"attribute1\"");
                expectJsonDoc.append(":");
                expectJsonDoc.append(result);
                expectJsonDoc.append("}");

                if (veryVeryVerbose) {
                    P(expectJsonDoc);
                }

                // Do the test
                oss.clear(); oss.str("");

                Obj  converter;
                Json json;

                int rc = converter.convert(&json, seqDec64);            // TEST

                // Check the results;
                ASSERTV(LINE,          rc,
                              0     == rc);
                ASSERTV(LINE,          converter.loggedMessages(),
                              ""    == converter.loggedMessages());

                if (veryVeryVerbose) {
                    P(json);
                }

                bsl::string actualJsonDoc;

                ASSERTV(LINE, 0 == JsnUtil::write(&actualJsonDoc, json));

                if (veryVeryVerbose) {
                    P(actualJsonDoc);
                }

                ASSERTV(LINE, expectJsonDoc,   actualJsonDoc,
                              expectJsonDoc == actualJsonDoc);

                // Now, convert back to the original type.
                SeqDec64 seqDec64FromJson;
                rc = converter.convert(&seqDec64FromJson, json);        // TEST

                ASSERTV(LINE,          rc,
                              0     == rc);
                ASSERTV(LINE,          converter.loggedMessages(),
                              ""    == converter.loggedMessages());

                if (veryVeryVerbose) {
                    P(seqDec64FromJson);
                }

                ASSERTV(LINE, seqDec64,   seqDec64FromJson,
                              seqDec64 == seqDec64FromJson);
            }
#undef DEC
        }

        if (verbose) cout << endl << "+Inf, -Inf, and NaN" << endl;
        {
            typedef  bsl::numeric_limits<Decimal64> Limits;

            const Decimal64 NAN_P = Limits::quiet_NaN();
            // Negative NaN does not print for any floating point type, so we
            // don't test it for round-trip (on purpose).
            //const Type NAN_N = -NAN_P;
            const Decimal64 INF_P = Limits::infinity();
            const Decimal64 INF_N = -INF_P;

            const struct {
                int       d_line;
                Decimal64 d_value;
            } DATA[] = {
                // LINE   VALUE |
                // ----   ------
                {  L_,    NAN_P },

              // Negative NaN does not print for any floating point type, so
              // we don't test it for round-trip (on purpose).
              //{  L_,    NAN_N },

                {  L_,    INF_P },
                {  L_,    INF_N },
            };
            const int NUM_DATA = sizeof DATA / sizeof DATA[0];

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int       LINE  = DATA[ti].d_line;
                const Decimal64 VALUE = DATA[ti].d_value;

                if (veryVerbose) {
                    P_(LINE); P(VALUE);
                }

                SeqDec64 seqDec64;
                seqDec64.attribute1() = VALUE;

                if (veryVeryVerbose) {
                    P(seqDec64);
                }

                // Synthesize expected JSON document.
                bsl::ostringstream oss;
                EOptions           eOptions;
                eOptions.setEncodeInfAndNaNAsStrings(true);
                int                rc = Print::printValue(oss,
                                                          VALUE,
                                                          &eOptions);
                ASSERTV(LINE, rc, 0 == rc);

                bsl::string result(oss.str());

                if (veryVeryVerbose) {
                    P(result);
                }

                bsl::string expectJsonDoc;
                expectJsonDoc.append("{");
                expectJsonDoc.append("\"attribute1\"");
                expectJsonDoc.append(":");
                expectJsonDoc.append(result);
                expectJsonDoc.append("}");

                if (veryVeryVerbose) {
                    P(expectJsonDoc);
                }

                Obj  converter;
                Json json;

                rc = converter.convert(&json, seqDec64);                // TEST

                // Check the results;
                ASSERTV(LINE,       rc,
                              0  == rc);
                ASSERTV(LINE,       converter.loggedMessages(),
                              "" == converter.loggedMessages());

                if (veryVeryVerbose) {
                    P(json);
                }

                bsl::string actualJsonDoc;

                ASSERTV(LINE, 0 == JsnUtil::write(&actualJsonDoc, json));

                if (veryVeryVerbose) {
                    P(actualJsonDoc);
                }

                ASSERTV(LINE, expectJsonDoc,   actualJsonDoc,
                              expectJsonDoc == actualJsonDoc);

                // Now, convert back to the original type.

                SeqDec64 seqDec64FromJson;
                rc = converter.convert(&seqDec64FromJson, json);        // TEST

                if (veryVeryVerbose) {
                    P(json);
                }

                ASSERTV(LINE,       rc,
                              0  == rc);
                ASSERTV(LINE,       converter.loggedMessages(),
                              "" == converter.loggedMessages());

                if (veryVeryVerbose) {
                    P(seqDec64FromJson);
                }

                if (DecUtil::isNan(seqDec64.attribute1().value())) {
                    ASSERTV(LINE,      seqDec64FromJson,
                            DecUtil::isNan(
                                       seqDec64FromJson.attribute1().value()));
                } else {
                    ASSERTV(LINE, seqDec64,   seqDec64FromJson,
                                  seqDec64 == seqDec64FromJson);
                }
            }
        }

        if (verbose) cout << endl << "Quoted input and invalid input" << endl;
        {
#define DEC(X) BDLDFP_DECIMAL_DD(X)

            typedef Decimal64 Type;

            const Type NAN_P = bsl::numeric_limits<Type>::quiet_NaN();
            const Type NAN_N = -NAN_P;
            const Type INF_P = bsl::numeric_limits<Type>::infinity();
            const Type INF_N = -INF_P;

            const Type ERROR_VALUE = BDLDFP_DECIMAL_DD(999.0);

            static const struct {
                int         d_line;    // line number
                const char *d_input_p; // input on the stream
                Type        d_exp;     // exp unsigned value
                bool        d_isValid; // isValid flag
            } DATA[] = {
     //---------v

     // line  input                       exp                         isValid
     // ----  -----                       ---                         -------
     {  L_,    "0",                       DEC(0.0),                    true  },
     {  L_,   "-0",                       DEC(0.0),                    true  },
     {  L_,    "0.0",                     DEC(0.0),                    true  },
     {  L_,   "-0.0",                     DEC(0.0),                    true  },
     {  L_,    "1",                       DEC(1.0),                    true  },
     {  L_,   "-1",                       DEC(-1.0),                   true  },
     {  L_,    "1.2",                     DEC(1.2),                    true  },
     {  L_,    "1.23",                    DEC(1.23),                   true  },
     {  L_,    "1.234",                   DEC(1.234),                  true  },
     {  L_,   "12.34",                    DEC(12.34),                  true  },
     {  L_,  "123.4",                     DEC(123.4),                  true  },
     {  L_,   "-1.2",                     DEC(-1.2),                   true  },
     {  L_,   "-1.23",                    DEC(-1.23),                  true  },
     {  L_,   "-1.234",                   DEC(-1.234),                 true  },
     {  L_,  "-12.34",                    DEC(-12.34),                 true  },
     {  L_, "-123.4",                     DEC(-123.4),                 true  },
     {  L_,   "+1.2",                     DEC(1.2),                    true  },
     {  L_,   "+1.23",                    DEC(1.23),                   true  },
     {  L_,   "+1.234",                   DEC(1.234),                  true  },
     {  L_,  "+12.34",                    DEC(12.34),                  true  },
     {  L_, "+123.4",                     DEC(123.4),                  true  },
     {  L_,   "-9.876543210987654e307",   DEC(-9.876543210987654e307), true  },
     {  L_, "\"-0.1\"",                   DEC(-0.1),                   true  },
     {  L_,  "\"0\"",                     DEC(0.0),                    true  },
     {  L_, "\"-0\"",                     DEC(0.0),                    true  },
     {  L_,  "\"0.0\"",                   DEC(0.0),                    true  },
     {  L_, "\"-0.0\"",                   DEC(0.0),                    true  },
     {  L_,  "\"1\"",                     DEC(1.0),                    true  },
     {  L_, "\"-1\"",                     DEC(-1.0),                   true  },
     {  L_,  "\"1.2\"",                   DEC(1.2),                    true  },
     {  L_,  "\"1.23\"",                  DEC(1.23),                   true  },
     {  L_,  "\"1.234\"",                 DEC(1.234),                  true  },
     {  L_, "\"12.34\"",                  DEC(12.34),                  true  },
     {  L_, "\"123.4\"",                  DEC(123.4),                  true  },
     {  L_, "\"-1.2\"",                   DEC(-1.2),                   true  },
     {  L_, "\"-1.23\"",                  DEC(-1.23),                  true  },
     {  L_, "\"-1.234\"",                 DEC(-1.234),                 true  },
     {  L_, "\"-12.34\"",                 DEC(-12.34),                 true  },
     {  L_, "\"-123.4\"",                 DEC(-123.4),                 true  },
     {  L_, "\"+1.2\"",                   DEC(1.2),                    true  },
     {  L_, "\"+1.23\"",                  DEC(1.23),                   true  },
     {  L_, "\"+1.234\"",                 DEC(1.234),                  true  },
     {  L_, "\"+12.34\"",                 DEC(12.34),                  true  },
     {  L_, "\"+123.4\"",                 DEC(123.4),                  true  },
     {  L_, "\"-9.876543210987654e307\"", DEC(-9.876543210987654e307), true  },
     {  L_,   "-0.1",                     DEC(-0.1),                   true  },
     {  L_,  "\"NaN\"",                   NAN_P,                       true  },
     {  L_,  "\"nan\"",                   NAN_P,                       true  },
     {  L_,  "\"NAN\"",                   NAN_P,                       true  },
     {  L_, "\"+NaN\"",                   NAN_P,                       true  },
     {  L_, "\"+nan\"",                   NAN_P,                       true  },
     {  L_, "\"+NAN\"",                   NAN_P,                       true  },
     {  L_, "\"-NaN\"",                   NAN_N,                       true  },
     {  L_, "\"-nan\"",                   NAN_N,                       true  },
     {  L_, "\"-NAN\"",                   NAN_N,                       true  },
     {  L_,  "\"INF\"",                   INF_P,                       true  },
     {  L_,  "\"inf\"",                   INF_P,                       true  },
     {  L_,  "\"infinity\"",              INF_P,                       true  },
     {  L_, "\"+INF\"",                   INF_P,                       true  },
     {  L_, "\"+inf\"",                   INF_P,                       true  },
     {  L_, "\"+infinity\"",              INF_P,                       true  },
     {  L_, "\"-INF\"",                   INF_N,                       true  },
     {  L_, "\"-inf\"",                   INF_N,                       true  },
     {  L_, "\"-infinity\"",              INF_N,                       true  },
     {  L_,         "-",                  ERROR_VALUE,                 false },
     {  L_,       "E-1",                  ERROR_VALUE,                 false },
     {  L_,  "Z34.56e1",                  ERROR_VALUE,                 false },
     {  L_,  "3Z4.56e1",                  ERROR_VALUE,                 false },
     {  L_,      "1.1}",                  ERROR_VALUE,                 false },
     {  L_,     "1.1\n",                  ERROR_VALUE,                 false },
     {  L_,   "1.10xFF",                  ERROR_VALUE,                 false },
     {  L_,  "DEADBEEF",                  ERROR_VALUE,                 false },
     {  L_,      "JUNK",                  ERROR_VALUE,                 false },
     {  L_,     "\"0\"",                  DEC(0.0),                    true  },
     {  L_,       "0\"",                  ERROR_VALUE,                 false },
     {  L_,       "\"0",                  ERROR_VALUE,                 false },
     {  L_,        "\"",                  ERROR_VALUE,                 false },
     {  L_,      "\"\"",                  ERROR_VALUE,                 false },
     {  L_,     "\"X\"",                  ERROR_VALUE,                 false },
     {  L_,  "\" NaN\"",                  ERROR_VALUE,                 false },

     //---------v
            };
            const int NUM_DATA = sizeof DATA / sizeof DATA[0];

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE     = DATA[i].d_line;
                const bsl::string INPUT    = DATA[i].d_input_p;
                const Type        EXP      = DATA[i].d_exp;
                const bool        IS_VALID = DATA[i].d_isValid;

                if (veryVerbose) {
                    P_(LINE); P(IS_VALID);
                    P(INPUT);
                    P(EXP);
                }

                if (IS_VALID) {

                    Decimal64 value;
                    ASSERTV(LINE, IS_VALID,                INPUT,
                            0 == ParUtil::getValue(&value, INPUT));

                    SeqDec64 seqDec64;
                    seqDec64.attribute1() = value;

                    if (veryVeryVerbose) {
                        P(seqDec64);
                    }

                    // Synthesize expected JSON document.
                    bsl::ostringstream oss;
                    EOptions           eOptions;
                    eOptions.setEncodeInfAndNaNAsStrings(true);
                    eOptions.setEncodeQuotedDecimal64(false);
                    int                rc = Print::printValue(oss,
                                                              value,
                                                              &eOptions);
                    ASSERTV(LINE, rc, 0 == rc);

                    bsl::string result(oss.str());

                    if (veryVeryVerbose) {
                        P(result);
                    }

                    bsl::string expectJsonDoc;
                    expectJsonDoc.append("{");
                    expectJsonDoc.append("\"attribute1\"");
                    expectJsonDoc.append(":");
                    expectJsonDoc.append(result);
                    expectJsonDoc.append("}");

                    if (veryVeryVerbose) {
                        P(expectJsonDoc);
                    }

                    Obj  converter;
                    Json json;

                    rc = converter.convert(&json, seqDec64);            // TEST

                    // Check the results;
                    ASSERTV(LINE,       rc,
                                  0  == rc);
                    ASSERTV(LINE,       converter.loggedMessages(),
                                  "" == converter.loggedMessages());

                    if (veryVeryVerbose) {
                        P(json);
                    }

                    bsl::string actualJsonDoc;

                    ASSERTV(LINE, 0 == JsnUtil::write(&actualJsonDoc, json));

                    if (veryVeryVerbose) {
                        P(actualJsonDoc);
                    }

                    ASSERTV(LINE, expectJsonDoc,   actualJsonDoc,
                                  expectJsonDoc == actualJsonDoc);

                    // Now, convert back to the original type.

                    SeqDec64 seqDec64FromJson;
                    rc = converter.convert(&seqDec64FromJson, json);    // TEST

                    if (veryVeryVerbose) {
                        P(json);
                    }

                    ASSERTV(LINE,       rc,
                                  0  == rc);
                    ASSERTV(LINE,       converter.loggedMessages(),
                                  "" == converter.loggedMessages());

                    if (veryVeryVerbose) {
                        P(seqDec64FromJson);
                    }

                    if (DecUtil::isNan(seqDec64.attribute1().value())) {
                        ASSERTV(LINE,  seqDec64FromJson,
                                DecUtil::isNan(
                                       seqDec64FromJson.attribute1().value()));
                    } else {
                        ASSERTV(LINE, seqDec64,   seqDec64FromJson,
                                      seqDec64 == seqDec64FromJson);
                    }
                }
            }
        }
#undef DEC
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // SKIPPING UNKNOWN ELEMENTS
        //
        // Concerns:
        // 1. The decoder correctly skips unknown elements if the
        //    `skipUnknownElement` decoder option is specified.
        //
        // 2. The decoder returns an error on encountering unknown elements if
        //    the `skipUnknownElement` decoder option is *not* specified.
        //
        // 3. Does `convert` to a `bdlat` object perform identically to
        //    `decode` for objects having anonymous members and objects
        //    that are Choices?
        //
        // Plan:
        // 1. Using the table-driven technique, specify a table with JSON text.
        //
        // 2. For each row in the tables of P-1:
        //
        //   1. Construct a test object.
        //
        //   2. Create a `baljsn::Decoder` object.
        //
        //   3. Create a `bsl::istringstream` object with the JSON text.
        //
        //   4. Convert that JSON into a test object specifying
        //      that unknown elements be skipped.
        //
        //   5. Verify that the decoded object has the expected data.
        //
        //   6. Verify that the return code from `decode` is 0.  (C-1)
        //
        //   7. Repeat steps 1 - 6 with the `skipUnknownElements` option set
        //      to `false`.  Verify that an error code is returned by `decode`.
        //      (C-2)
        //
        // 3. Using the table-driven technique, specify a table with malformed
        //    JSON texts.
        //
        // 4. For each row in the tables of P-3:
        //
        //   1. Construct a test object.
        //
        //   2. Create a `baljsn::Decoder` object.
        //
        //   3. Create a `bsl::istringstream` object with the JSON text.
        //
        //   4. Decode that JSON into a test object specifying
        //      that unknown elements be skipped.
        //
        //   5. Verify that an error code is returned by `convert`.
        //
        //   6. Repeat steps 1 - 5 with the `skipUnknownElements` option set
        //      to `false`.  (C-3)
        //
        // 5. Create several test objects and pass each to
        //    `testSkipUnknownElements`.
        //
        //   1. The test function uses two `Oracle` functions -- each a wrapper
        //      for `encode` and `decode`, respectively -- to establish the
        //      expected behavior.
        //
        //   2. The given object is converted to a `Json` object (each
        //      conversion is expected to be successful).
        //
        //   3. The conversion from the `Json` object back to a `bdlat` object
        //      is tested:
        //
        //      1. `convert` back is invoked three times
        //          1. `convert` having no option (skip defaults to `true`).
        //          2. `convert` with skip option explicitly set to `true`.
        //          3. `convert` with skip option explicitly set to `false`.
        //
        //      2. The result (success or failure) should match that obversed
        //         from the `Oracle` call.
        //
        //      3. Note that some of these calls are required to fail.  For
        //         example:
        //
        //         * a sequence holding an anonymous sequence
        //         * a choice
        //
        // Testing:
        //   CONCERN: SKIPPING UNKNOWN ELEMENTS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SKIPPING UNKNOWN ELEMENTS" << endl
                          << "=========================" << endl;

        if (veryVerbose) cout << "Testing valid JSON strings" << endl;

        typedef baljsn::DecoderOptions Doptions;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_text_p;   // json text
        } DATA[] = {
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"age\" : 21,\n"
                "       \"id\"  : 21\n"                 // <--- unknown element
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"id\"  : 21,\n"                // <--- unknown element
                "       \"age\" : 21\n"
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"age\" : 21,\n"
                "       \"nickname\" : \"Robert\"\n"    // <--- unknown element
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"nickname\" : \"Robert\",\n"   // <--- unknown element
                "       \"age\" : 21\n"
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"age\" : 21,\n"
                "       \"id\"  : 21,\n"                // <--- unknown element
                "       \"nickname\" : \"Robert\"\n"    // <--- unknown element
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"id\"  : 21,\n"                // <--- unknown element
                "       \"nickname\" : \"Robert\",\n"   // <--- unknown element
                "       \"age\" : 21\n"
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"age\" : 21,\n"
                "       \"ids\" : [ 1, 2 ]\n"           // <--- unknown element
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"ids\" : [ 1, 2 ],\n"          // <--- unknown element
                "       \"age\" : 21\n"
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"age\" : 21,\n"
                "       \"aliases\" : [ \"Foo\", \"Bar\" ]\n"   // <--- unknown
                                                                //      element
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"aliases\" : [ \"Foo\", \"Bar\" ],\n"  // <--- unknown
                                                                //      element
                "       \"age\" : 21\n"
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"age\" : 21,\n"
                "       \"officeAddress\" : {\n"        // <--- unknown element
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       }\n"
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"officeAddress\" : {\n"        // <--- unknown element
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"age\" : 21\n"
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"age\" : 21,\n"
                "       \"officeAddress\" : {\n"        // <--- unknown element
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\",\n"
                "           \"ids\" : [ 1, 2, 3]\n"
                "       }\n"
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"officeAddress\" : {\n"        // <--- unknown element
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\",\n"
                "           \"ids\" : [ 1, 2, 3]\n"
                "       },\n"
                "       \"age\" : 21\n"
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"officeAddress\" : {\n"        // <--- unknown element
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\",\n"
                "           \"misc\" : {\n"
                "               \"country\" : \"USA\",\n"
                "               \"timezone\" : \"EST\"\n"
                "           }\n"
                "       },\n"
                "       \"age\" : 21\n"
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"age\" : 21,\n"
                "       \"officeAddress\" : {\n"        // <--- unknown element
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\",\n"
                "           \"misc\" : {\n"
                "               \"country\" : \"USA\",\n"
                "               \"timezone\" : \"EST\"\n"
                "           }\n"
                "       }\n"
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"addrs\" : [\n"                // <--- unknown element
                "           {\n"
                "               \"officeAddress\" : {\n"
                "                   \"street\" : \"Some Street\",\n"
                "                   \"city\" : \"Some City\",\n"
                "                   \"state\" : \"Some State\",\n"
                "                   \"times\" : [ 1, 2, 3 ],\n"
                "                   \"misc\" : {\n"
                "                       \"country\" : \"USA\",\n"
                "                       \"timezone\" : \"EST\"\n"
                "                   }\n"
                "               }\n"
                "           }\n"
                "       ],\n"
                "       \"age\" : 21\n"
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"age\" : 21,\n"
                "       \"addrs\" : [\n"                // <--- unknown element
                "           {\n"
                "               \"officeAddress\" : {\n"
                "                   \"street\" : \"Some Street\",\n"
                "                   \"city\" : \"Some City\",\n"
                "                   \"state\" : \"Some State\",\n"
                "                   \"times\" : [ 1, 2, 3 ],\n"
                "                   \"misc\" : {\n"
                "                       \"country\" : \"USA\",\n"
                "                       \"timezone\" : \"EST\"\n"
                "                   }\n"
                "               }\n"
                "           }\n"
                "       ]\n"
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"addrs\" : [\n"                // <--- unknown element
                "           {\n"
                "               \"officeAddress\" : {\n"
                "                   \"street\" : \"Some Street\",\n"
                "                   \"city\" : \"Some City\",\n"
                "                   \"state\" : \"Some State\",\n"
                "                   \"times\" : [ 1, 2, 3 ],\n"
                "                   \"misc\" : {\n"
                "                       \"country\" : \"USA\",\n"
                "                       \"timezone\" : \"EST\"\n"
                "                   }\n"
                "               }\n"
                "           }\n"
                "       ],\n"
                "       \"age\" : 21\n"
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"age\" : 21,\n"
                "       \"addrs\" : [\n"                // <--- unknown element
                "           {\n"
                "               \"officeAddress\" : {\n"
                "                   \"street\" : \"Some Street\",\n"
                "                   \"city\" : \"Some City\",\n"
                "                   \"state\" : \"Some State\",\n"
                "                   \"times\" : [ 1, 2, 3 ],\n"
                "                   \"misc\" : {\n"
                "                       \"country\" : \"USA\",\n"
                "                       \"timezone\" : \"EST\"\n"
                "                   }\n"
                "               }\n"
                "           },\n"
                "           {\n"
                "               \"shippingAddress\" : {\n"
                "                   \"street\" : \"Some Street\",\n"
                "                   \"city\" : \"Some City\",\n"
                "                   \"state\" : \"Some State\",\n"
                "                   \"times\" : [ 1, 2, 3 ],\n"
                "                   \"misc\" : {\n"
                "                       \"country\" : \"USA\",\n"
                "                       \"timezone\" : \"EST\"\n"
                "                   }\n"
                "               }\n"
                "           }\n"
                "       ]\n"
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"addrs\" : [\n"                // <--- unknown element
                "           {\n"
                "               \"officeAddress\" : {\n"
                "                   \"street\" : \"Some Street\",\n"
                "                   \"city\" : \"Some City\",\n"
                "                   \"state\" : \"Some State\",\n"
                "                   \"times\" : [ 1, 2, 3 ],\n"
                "                   \"misc\" : {\n"
                "                       \"country\" : \"USA\",\n"
                "                       \"timezone\" : \"EST\"\n"
                "                   }\n"
                "               }\n"
                "           },\n"
                "           {\n"
                "               \"shippingAddress\" : {\n"
                "                   \"street\" : \"Some Street\",\n"
                "                   \"city\" : \"Some City\",\n"
                "                   \"state\" : \"Some State\",\n"
                "                   \"times\" : [ 1, 2, 3 ],\n"
                "                   \"misc\" : {\n"
                "                       \"country\" : \"USA\",\n"
                "                       \"timezone\" : \"EST\"\n"
                "                   }\n"
                "               }\n"
                "           }\n"
                "       ],\n"
                "       \"age\" : 21\n"
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"age\" : 21,\n"
                "       \"misc\" : {\n"                 // <--- unknown element
                "           \"name\" : \"Bob\",\n"
                "           \"homeAddress\" : {\n"
                "               \"street\" : \"Some Street\",\n"
                "               \"city\" : \"Some City\",\n"
                "               \"state\" : \"Some State\"\n"
                "           },\n"
                "           \"age\" : 21,\n"
                "           \"addrs\" : [\n"
                "               {\n"
                "                   \"officeAddress\" : {\n"
                "                       \"street\" : \"Some Street\",\n"
                "                       \"city\" : \"Some City\",\n"
                "                       \"state\" : \"Some State\",\n"
                "                       \"times\" : [ 1, 2, 3 ],\n"
                "                       \"misc\" : {\n"
                "                           \"country\" : \"USA\",\n"
                "                           \"timezone\" : \"EST\"\n"
                "                       }\n"
                "                   }\n"
                "               },\n"
                "               {\n"
                "                   \"shippingAddress\" : {\n"
                "                       \"street\" : \"Some Street\",\n"
                "                       \"city\" : \"Some City\",\n"
                "                       \"state\" : \"Some State\",\n"
                "                       \"times\" : [ 1, 2, 3 ],\n"
                "                       \"misc\" : {\n"
                "                           \"country\" : \"USA\",\n"
                "                           \"timezone\" : \"EST\"\n"
                "                       }\n"
                "                   }\n"
                "               }\n"
                "           ]\n"
                "       }\n"
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"misc\" : {\n"                 // <--- unknown element
                "           \"name\" : \"Bob\",\n"
                "           \"homeAddress\" : {\n"
                "               \"street\" : \"Some Street\",\n"
                "               \"city\" : \"Some City\",\n"
                "               \"state\" : \"Some State\"\n"
                "           },\n"
                "           \"age\" : 21,\n"
                "           \"addrs\" : [\n"
                "               {\n"
                "                   \"officeAddress\" : {\n"
                "                       \"street\" : \"Some Street\",\n"
                "                       \"city\" : \"Some City\",\n"
                "                       \"state\" : \"Some State\",\n"
                "                       \"times\" : [ 1, 2, 3 ],\n"
                "                       \"misc\" : {\n"
                "                           \"country\" : \"USA\",\n"
                "                           \"timezone\" : \"EST\"\n"
                "                       }\n"
                "                   }\n"
                "               },\n"
                "               {\n"
                "                   \"shippingAddress\" : {\n"
                "                       \"street\" : \"Some Street\",\n"
                "                       \"city\" : \"Some City\",\n"
                "                       \"state\" : \"Some State\",\n"
                "                       \"times\" : [ 1, 2, 3 ],\n"
                "                       \"misc\" : {\n"
                "                           \"country\" : \"USA\",\n"
                "                           \"timezone\" : \"EST\"\n"
                "                       }\n"
                "                   }\n"
                "               }\n"
                "           ]\n"
                "       },\n"
                "       \"age\" : 21\n"
                "}"
            },
            // `baljsn::Decoder` fails to decode heterogeneous arrays, but
            // since we skip an unknown element it is allowed to contain such
            // arrays.  See {DRQS 171296111} for more details.
            {
                L_,
                "{\n"
                "       \"name\" : \"Bob\",\n"
                "       \"homeAddress\" : {\n"
                "           \"street\" : \"Some Street\",\n"
                "           \"city\" : \"Some City\",\n"
                "           \"state\" : \"Some State\"\n"
                "       },\n"
                "       \"build-depends\": [\n"         // <--- unknown element
                "           \"cmake-breg-generate-code\",\n"
                "           \"cmake-configure-bb-target\",\n"
                "           \"cmake-upside-down-shared-objects\",\n"
                "           {\n"
                "               \"name\": \"liba-basfs-dev\",\n"
                "               \"architectures\":\n"
                "               [\n"
                "                   \"aix6-powerpc\",\n"
                "                   \"solaris10-sparc\"\n"
                "               ]\n"
                "           },\n"
                "           \"liba-iaabass-dev\"\n"
                "       ],\n"
                "       \"age\" : 21\n"
                "}"
            },
        };
        const int NUM_DATA = sizeof DATA/ sizeof *DATA;

        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const int          tj       = ti / 2;
        //  const bool         UTF8     = ti & 1;
            const int          LINE     = DATA[tj].d_lineNum;
            const bsl::string& jsonText = DATA[tj].d_text_p;

            if (veryVeryVerbose) {
                P_(tj); P_(LINE); P(jsonText);
            }

            Json  json;
            Error error;
            int   rc = bdljsn::JsonUtil::read(&json, &error, jsonText);
            ASSERTV(rc, 0 == rc);

            Obj converter;

            Doptions options; const Doptions& Options = options;

            options.setSkipUnknownElements(false);
            {
                test::Employee bob;
                ASSERTV(LINE, 0 != converter.convert(&bob,
                                                     json,
                                                     Options));         // TEST
            }

            options.setSkipUnknownElements(true);
            {
                test::Employee     bob;
                ASSERTV(LINE, 0 == converter.convert(&bob,
                                                     json,
                                                     Options));         // TEST
                ASSERTV(bob.name(), "Bob" == bob.name());
                ASSERT("Some Street" == bob.homeAddress().street());
                ASSERT("Some City"   == bob.homeAddress().city());
                ASSERT("Some State"  == bob.homeAddress().state());
                ASSERTV(LINE, 21     == bob.age());
            }
            {
                test::Employee     bob;
                ASSERTV(LINE, 0 == converter.convert(&bob,   // default options
                                                     json));            // TEST
                ASSERTV(bob.name(), "Bob" == bob.name());
                ASSERT("Some Street" == bob.homeAddress().street());
                ASSERT("Some City"   == bob.homeAddress().city());
                ASSERT("Some State"  == bob.homeAddress().state());
                ASSERTV(LINE, 21     == bob.age());
            }
        }

        if (veryVerbose) cout << "Test more exotic `bdlat` types" << endl;
        {
            s_baltst::MySequence mySequence;
            mySequence.attribute1() = 1234;
            mySequence.attribute2() = "To be or not to be...";

            testSkipUnknownElements(L_, mySequence);                    // TEST

            s_baltst::SequenceWithAnonymity sequenceWithAnonymity;
            sequenceWithAnonymity.choice ().makeSelection2();
            sequenceWithAnonymity.choice1().makeSelection5();
            s_baltst::Sequence4 sequence4;
            sequenceWithAnonymity.choice2().makeValue().makeSelection7(
                                                                    sequence4);

            testSkipUnknownElements(L_, sequenceWithAnonymity);         // TEST

            baljsn::EncoderTestSequenceWithUntagged14 etswu14;
            etswu14.attribute0() =  7;
            etswu14.attribute1() = 13;
            etswu14.attribute2() = 42;

            testSkipUnknownElements(L_, etswu14);                       // TEST

            s_baltst::Topchoice topchoice;
            topchoice.makeSelection5(bdlt::DatetimeTz());

            testSkipUnknownElements(L_, topchoice);                     // TEST

        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // DECODING INTS AS ENUMS AND VICE VERSA
        //
        // Concerns:
        // 1. The encoding produced by `baljsn_encoder` when encoding an
        //    integer enumeration can be decoded into either a plain integral
        //    type or a customized type (as produced by bas_codegen) whose base
        //    type is integral.
        //
        // 2. The encoding produced by `baljsn_encoder` when encoding either a
        //    plain integral type or a customized type (as produced by
        //    bas_codegen) whose base type is integral can be decoded into an
        //    integer enumeration type.
        //
        // Plan:
        // 1. Define a type, `Enumeration1`, that is a `bdlat` enumeration, can
        //    hold an integer value of either 0 or 1, and whose string
        //    representation is just the decimal form of its value (as with
        //    `bcem_Aggregate`).
        //
        // 2. Using `baljsn_encoder`, encode objects of type `Enumeration1`
        //    having values of 0 and 1, and decode them into plain `int`s.
        //    Verify that the resulting values are the same as the original
        //    values.  Then, repeat using the generated type
        //    `test::MyIntEnumeration`.  (C-1)
        //
        // 3. Using `baljsn_encoder`, encode plain `int`s having values of 0
        //    and 1, decode them into `Enumeration1`, and verify that the
        //    resulting values are the same as the original values.  Then,
        //    repeat using the generated type `test::MyIntEnumeration`.  (C-2)
        //
        // Testing:
        //   CONERN: DECODING INTS AS ENUMS AND VICE VERSA
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DECODING INTS AS ENUMS AND VICE VERSA" << endl
                          << "=====================================" << endl;

        baljsn::Encoder   encoder;
        baljsn::Decoder   decoder;
        bsl::stringstream ss;

        bsl::vector<test::Enumeration1>     ve(2);
        bsl::vector<int>                    vi(2);
        bsl::vector<test::MyIntEnumeration> vc(2);

        // P-1
        ASSERT(0 == bdlat_EnumFunctions::fromInt(&ve[0], 0));
        ASSERT(0 == bdlat_EnumFunctions::fromInt(&ve[1], 1));
        ASSERT(0 == encoder.encode(ss,  ve, baljsn::EncoderOptions()));

        Json expected; expected.makeString(ss.str());

        if (veryVeryVerbose) {
            P(expected);
        }

        ASSERT(0 == decoder.decode(ss, &vi, baljsn::DecoderOptions()));
        ASSERT(0 == vi[0]);
        ASSERT(1 == vi[1]);
        ss.seekg(0);
        ASSERT(0 == decoder.decode(ss, &vc, baljsn::DecoderOptions()));
        ASSERT(test::MyIntEnumeration(0) == vc[0]);
        ASSERT(test::MyIntEnumeration(1) == vc[1]);

        // Test 'convert' to/from Json

        bsl::vector<test::Enumeration1>     veFromJson(2);
        bsl::vector<int>                    viFromJson(2);
        bsl::vector<test::MyIntEnumeration> vcFromJson(2);

        Obj  converter;
        Json json;
        int rc = converter.convert(&json, ve);

        ASSERTV(rc, 0 == rc);
        ASSERTV(      converter.loggedMessages(),
                "" == converter.loggedMessages());

        if (veryVeryVerbose) {
            P(json);
        }

        rc = converter.convert(&viFromJson, json);

        ASSERTV(rc, 0 == rc);
        ASSERTV(      converter.loggedMessages(),
                "" == converter.loggedMessages());
        ASSERTV(vi == viFromJson);
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // FALLBACK ENUMERATORS
        //
        // Concerns:
        // 1. When decoding into an enumeration type with a fallback
        //    enumerator, if the encoded value corresponds to a known
        //    enumerator, that enumerator is the result of the decoding.
        // 2. When decoding into an enumeration type with a fallback
        //    enumerator, if the encoded value does not correspond to any known
        //    enumerator, the decoding still succeeds and produces the fallback
        //    enumerator value.
        //
        // Plan:
        // 1. Use the table-driven technique, specify a set of valid encoded
        //    values of the enumeration `test:MyEnumerationWithFallback`,
        //    including ones that correspond to both known and unknown
        //    enumerators, together with the expected result of decoding that
        //    value (either the corresponding enumerator value or the fallback
        //    enumerator value, respectively).
        // 2. Generate a JSON input string representing an array of encoded
        //    values by concatenating the encoded values specified in P-1,
        //    separated by commas, and delimited by '[' and ']'.
        //    Simultaneously, generate a vector of expected values, which
        //    consists of the enumerator values specified in P-1.
        // 3. Verify that the result of decoding the JSON input string created
        //    in P-2 equals the vector of expected values created in P-2
        //    (C-1..2).
        //
        // Note: The reason why this is a separate test case is to avoid
        // further increasing the size of {`balb_testmessages`}, which would
        // affect every component that depends on it.
        //
        // Testing:
        //   CONCERN: FALLBACK ENUMERATORS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "FALLBACK ENUMERATORS" << endl
                          << "====================" << endl;

        typedef test::MyEnumerationWithFallback Enum;

        static const struct {
            const char *d_json_p;
            Enum::Value d_expected;
        } DATA[] = {
            { "\"VALUE1\"",  Enum::VALUE1  },
            { "\"VALUE2\"",  Enum::VALUE2  },
            { "\"UNKNOWN\"", Enum::UNKNOWN },
            { "\"VALUE3\"",  Enum::UNKNOWN },
        };
        static const int DATA_LEN = sizeof(DATA) / sizeof(DATA[0]);

        bsl::string input = "[";
        bsl::vector<Enum::Value> expected;
        for (int i = 0; i < DATA_LEN; i++) {
            if (i > 0) {
                input.push_back(',');
            }
            input.append(DATA[i].d_json_p);
            expected.push_back(DATA[i].d_expected);
        }
        input.push_back(']');

        for (int UTF8 = 0; UTF8 < 2; UTF8++) {
            if (veryVerbose) { T_; P(UTF8); }

            bsl::vector<Enum::Value>   value;
            baljsn::DecoderOptions     options;
            bdlsb::FixedMemInStreamBuf isb(input.data(), input.length());
            Decoder                    decoder;
            if (UTF8) options.setValidateInputIsUtf8(true);

            int rc = decoder.decode(&isb, &value, options);

            ASSERTV(decoder.loggedMessages(), rc, 0 == rc);
            ASSERTV(isb.length(), 0 == isb.length());
            ASSERTV(decoder.loggedMessages(),
                    expected.size(),   value.size(),
                    expected.size() == value.size());
            if (expected.size() == value.size()) {
                for (bsl::size_t i = 0; i < expected.size(); i++) {
                    ASSERTV(expected[i], value[i], expected[i] == value[i]);
                }
            }

            Obj  converter;
            Json json;
            rc = converter.convert(&json, value);

            ASSERTV(rc, 0 == rc);
            ASSERTV(      converter.loggedMessages(),
                    "" == converter.loggedMessages());

            bsl::vector<Enum::Value>   valueFromJson;
            rc = converter.convert(&valueFromJson, json);

            ASSERTV(rc, 0 == rc);
            ASSERTV(      converter.loggedMessages(),
                    "" == converter.loggedMessages());

            ASSERTV(// value,   valueFromJson,
                    value == valueFromJson);
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // FLOATING-POINT VALUES ROUND-TRIP
        //
        // Concerns:
        // 1. Numbers encoded without precision restrictions decode back to the
        //    same number (round-trip).
        //
        // Plan:
        // 1. Use the table-driven technique:
        //
        //   1. Specify a set of valid values, including those that will test
        //      the precision of the output.
        //
        //   2. Encode, then decode each value and verify that the decoded
        //      value is as expected.
        //
        // 2. Do those for `float` as well as `double` values.
        //
        // Testing:
        //   CONCERN: FLOATING-POINT VALUES ROUND-TRIP
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "FLOATING-POINT VALUES ROUND-TRIP" << endl
                          << "================================" << endl;

        if (veryVerbose) cout << "Round-trip `float`" << endl;
        {
            typedef bsl::numeric_limits<float> Limits;

            const float neg0 = copysignf(0.0f, -1.0f);

            const struct {
                int   d_line;
                float d_value;
            } DATA[] = {
                //LINE        VALUE
                //----  -------------
                { L_,           0.0f },
                { L_,         0.125f },
                { L_,        1.0e-1f },
                { L_,      0.123456f },
                { L_,           1.0f },
                { L_,           1.5f },
                { L_,          10.0f },
                { L_,         1.5e1f },
                { L_,   1.23456e-20f },
                { L_,   0.123456789f },
                { L_,  0.1234567891f },

                { L_,           neg0 },
                { L_,        -0.125f },
                { L_,       -1.0e-1f },
                { L_,     -0.123456f },
                { L_,          -1.0f },
                { L_,          -1.5f },
                { L_,         -10.0f },
                { L_,        -1.5e1f },
                { L_,  -1.23456e-20f },
                { L_,  -0.123456789f },
                { L_, -0.1234567891f },

                // {DRQS 165162213} regression, 2^24 loses precision as float
                { L_, 1.0f * 0xFFFFFF },

                // Full Mantissa Integers
                { L_, 1.0f * 0xFFFFFF },
                { L_, 1.0f * 0xFFFFFF     // this happens to be also
                       * (1ull << 63)     // `NumLimits::max()`
                       * (1ull << 41) },

                // Boundary Values
                { L_,  Limits::min()        },
                { L_,  Limits::denorm_min() },
                { L_,  Limits::max()        },
                { L_, -Limits::min()        },
                { L_, -Limits::denorm_min() },
                { L_, -Limits::max()        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_line;
                const float VALUE = DATA[ti].d_value;

                if (veryVerbose) {
                    P_(LINE); P(VALUE);
                }

                Obj  converter;
                Json json;

                s_baltst::SqrtF toEncode;
                toEncode.value() = VALUE;

                ASSERTV(LINE, converter.loggedMessages(),
                        0 == converter.convert(&json, toEncode));       // TEST

                s_baltst::SqrtF  decoded;
                decoded.value() = Limits::quiet_NaN(); // A value we don't use

                ASSERTV(LINE, converter.loggedMessages(),
                        0 == converter.convert(&decoded, json));
                float DECODED = decoded.value();

                ASSERTV(LINE,    json, VALUE, DECODED, VALUE == DECODED);
                // We also use `memcmp` to ensure that we really get back the
                // exact same binary IEEE-754, bit-by-bit.
                ASSERTV(LINE,    json, VALUE, DECODED,
                        0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));
            }
        }

        if (veryVerbose) cout << "Round-trip `float` Inf and NaN" << endl;
        {
            roundTripTestNonNumericValues<float>();
        }

        if (veryVerbose) cout << "Round-trip `double`" << endl;
        {
            typedef bsl::numeric_limits<double> Limits;

            double neg0 = copysign(0.0, -1.0);

            const struct {
                int    d_line;
                double d_value;
            } DATA[] = {
                //LINE            VALUE
                //----   ----------------------
                { L_,      0.0                  },
                { L_,      1.0e-1               },
                { L_,      0.125                },
                { L_,      1.0                  },
                { L_,      1.5                  },
                { L_,     10.0                  },
                { L_,      1.5e1                },
                { L_,      9.9e100              },
                { L_,      3.14e300             },
                { L_,      2.23e-308            },
                { L_,      0.12345678912345     },
                { L_,      0.12345678901234567  },
                { L_,      0.123456789012345678 },

                { L_,   neg0                    },
                { L_,     -1.0e-1               },
                { L_,     -0.125                },
                { L_,     -1.0                  },
                { L_,     -1.5                  },
                { L_,    -10.0                  },
                { L_,     -1.5e1                },
                { L_,     -9.9e100              },
                { L_,     -3.14e300             },
                { L_,     -2.23e-308            },
                { L_,     -0.12345678912345     },
                { L_,     -0.12345678901234567  },
                { L_,     -0.123456789012345678 },

                // Small Integers
                { L_,      123456789012345.     },
                { L_,      1234567890123456.    },

                // Full Mantissa Integers
                { L_, 1.0 * 0x1FFFFFFFFFFFFFull },
                { L_, 1.0 * 0x1FFFFFFFFFFFFFull  // This is also limits::max()
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 26)            },

                // Boundary Values
                { L_,  Limits::min()        },
                { L_,  Limits::denorm_min() },
                { L_,  Limits::max()        },
                { L_, -Limits::min()        },
                { L_, -Limits::denorm_min() },
                { L_, -Limits::max()        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE  = DATA[ti].d_line;
                const double VALUE = DATA[ti].d_value;

                if (veryVerbose) {
                    P_(LINE); P(VALUE);
                }

                s_baltst::Sqrt toEncode;
                toEncode.value() = VALUE;

                Json json;
                Obj  converter;
                ASSERTV(LINE, converter.loggedMessages(),
                        0 == converter.convert(&json, toEncode));       // TEST

                s_baltst::Sqrt decoded;
                decoded.value() = Limits::quiet_NaN(); // A value we don't use

                ASSERTV(LINE, converter.loggedMessages(),
                        0 == converter.convert(&decoded, json));        // TEST

                double DECODED = decoded.value();

                ASSERTV(LINE, json, VALUE, DECODED, VALUE == DECODED);
                // We also use `memcmp` to ensure that we really get back the
                // exact same binary IEEE-754, bit-by-bit.
                ASSERTV(LINE, json, VALUE, DECODED,
                        0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));
            }
        }

        if (veryVerbose) cout << "Round-trip `double` Inf and NaN" << endl;
        {
            roundTripTestNonNumericValues<double>();
        }
      } break;
      case 15: {
        // ------------------------------------------------------------------
        // DECODING OF ENUM TYPES WITH ESCAPED CHARS
        //
        // Concerns:
        // 1. Decoding of an enum type with escaped characters works as
        //    expected.
        //
        // Plan:
        // 1. Using the table-driven technique, specify a table with JSON text
        //    that contains an enum string with escaped chars.
        //
        // 2. For each row in the tables of P-1:
        //
        //   1. Create a `baljsn_Decoder` object.
        //
        //   2. Create a `bsl::istringstream` object with the JSON text.
        //
        //   3. Decode that JSON into a `test::Palette` object.
        //
        //   4. Verify that the return code from `decode` and the decoded
        //      object is as expected.
        //
        // Testing:
        //   CONCERN: DECODING OF ENUM TYPES WITH ESCAPED CHARS
        // ------------------------------------------------------------------

        if (verbose) cout
                        << endl
                        << "DECODING OF ENUM TYPES WITH ESCAPED CHARS" << endl
                        << "=========================================" << endl;

        using bsl::string;

        const char *S1 = "RED/GREEN";
        const char *S2 = "GREY\\BLUE";
        const char *S3 = "WHITE'BLACK";
        const char *S4 = "BLUE\"YELLOW";
        const char *S5 = "RED\b\r\t\f\n";

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_text_p;   // text string
            const char *d_output_p; // json output
        } DATA[] = {
            // line  text   output
            // ----  ----   ----
            {   L_,   S1,   "{ \"color\" : \"RED\\/GREEN\" }"    },
            {   L_,   S2,   "{ \"color\" : \"GREY\\\\BLUE\" }"   },
            {   L_,   S3,   "{ \"color\" : \"WHITE\'BLACK\" }"   },
            {   L_,   S4,   "{ \"color\" : \"BLUE\\\"YELLOW\" }" },
            {   L_,   S5,   "{ \"color\" : \"RED\b\r\t\f\n\" }"  }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const int          tj     = ti / 2;
            const bool         UTF8   = ti & 1;
            const int          LINE   = DATA[tj].d_lineNum;
            const bsl::string& TEXT   = DATA[tj].d_text_p;
            const bsl::string& OUTPUT = DATA[tj].d_output_p;

            test::Palette palette;
            bsl::istringstream iss(OUTPUT);

            baljsn::DecoderOptions options;
            options.setValidateInputIsUtf8(UTF8);
            baljsn::Decoder        decoder;
            const int rc = decoder.decode(iss, &palette, options);
            ASSERTV(LINE, rc, 0 == rc);
            ASSERTV(LINE, TEXT,
                    TEXT == bsl::string(test::Colors::toString(palette
                                                                   .color())));
            if (veryVerbose) {
                P(decoder.loggedMessages());
                P(palette);
            }
        }

        for (int tu = 0; tu < 2; ++tu) {
            const bool UTF8 = tu;

            const bsl::string& ALL_OUTPUT   =
                "{ \"colors\" : [\n"
                                   "\"RED\\/GREEN\",    \n"
                                   "\"GREY\\\\BLUE\",   \n"
                                   "\"WHITE\'BLACK\",   \n"
                                   "\"BLUE\\\"YELLOW\", \n"
                                   "\"RED\b\r\t\f\n\"   \n"
                               "]\n"
                 "}";

            test::Palette palette;

            bsl::istringstream iss(ALL_OUTPUT);

            baljsn::DecoderOptions options;
            options.setValidateInputIsUtf8(UTF8);
            baljsn::Decoder        decoder;
            int rc = decoder.decode(iss, &palette, options);
            ASSERTV(rc, 0 == rc);
            const bsl::vector<test::Colors::Value>& V = palette.colors();

            ASSERTV(5  == V.size());
            ASSERTV(string(S1) == string(test::Colors::toString(V[0])));
            ASSERTV(string(S2) == string(test::Colors::toString(V[1])));
            ASSERTV(string(S3) == string(test::Colors::toString(V[2])));
            ASSERTV(string(S4) == string(test::Colors::toString(V[3])));
            ASSERTV(string(S5) == string(test::Colors::toString(V[4])));

            if (veryVerbose) {
                P(decoder.loggedMessages());
                P(palette);
            }

            Obj  converter;
            Json json;

            rc = converter.convert(&json, palette);
            ASSERTV(rc, 0 == rc);
            ASSERTV(      converter.loggedMessages(),
                    "" == converter.loggedMessages());

            test::Palette paletteFromJson;

            rc = converter.convert(&paletteFromJson, json);

            ASSERTV(rc, 0 == rc);
            ASSERTV(      converter.loggedMessages(),
                    "" == converter.loggedMessages());

            ASSERTV(palette,   paletteFromJson,
                    palette == paletteFromJson);
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // DECODING OF `hexBinary` CUSTOMIZED TYPE
        //
        // Concerns:
        // 1. Decoding of a customized type of type `hexBinary` works as
        //    expected.
        //
        // Plan:
        // 1. Using the table-driven technique, specify a table with JSON text
        //    that contains a `hexBinary` customized type element.
        //
        // 2. For each row in the tables of P-1:
        //
        //   1. Construct a `HexBinarySequence` object.
        //
        //   2. Create a `baljsn::Decoder` object.
        //
        //   3. Create a `bsl::istringstream` object with the JSON text.
        //
        //   4. Decode that JSON into a `HexBinarySequence` object.
        //
        //   5. Verify that the return code from `decode` is as expected.
        //
        // Testing:
        //   DRQS 43702912
        //   CONCERN: DECODING OF `hexBinary` CUSTOMIZED TYPE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DECODING OF `hexBinary` CUSTOMIZED TYPE" << endl
                          << "=======================================" << endl;

        static const struct {
            int         d_line;      // source line number
            const char *d_input_p;   // input
            const char *d_output_p;  // output
            int         d_outputLen; // output length
            bool        d_isValid;   // isValid flag
            } DATA[] = {
          {  L_,  "\"\"",            "",                   0, true  },

          {  L_,  "\"Ug==\"",        "R",                  1, true  },
          {  L_,  "\"QVY=\"",        "AV",                 2, true  },

          {  L_,  "\"AA==\"",        "\x00",               1, true  },
          {  L_,  "\"AQ==\"",        "\x01",               1, true  },
          {  L_,  "\"\\/w==\"",      "\xFF",               1, true  },

          {  L_,  "\"UVE=\"",        "QQ",                 2, true  },

          {  L_,  "\"YQ==\"",        "a",                  1, true  },
          {  L_,  "\"YWI=\"",        "ab",                 2, true  },
          {  L_,  "\"YWJj\"",        "abc",                3, true  },
          {  L_,  "\"YWJjZA==\"",    "abcd",               4, true  },

          {  L_,  "\"Qmxvb21iZXJnTFA=\"", "BloombergLP",  11, true  },

          {  L_,     "",               "",                   0, false },
          {  L_,     "\"Q\"",          "",                   0, false },
          {  L_,     "\"QV\"",         "",                   0, false },
          {  L_,     "\"QVE\"",        "",                   0, false },
          {  L_,     "\"QVE==\"",      "",                   0, false },
            };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const int   tj          = ti / 2;
            const bool  UTF8        = ti & 1;
            const int   LINE        = DATA[tj].d_line;
            const char *INPUT       = DATA[tj].d_input_p;
            const char *OUTPUT      = DATA[tj].d_output_p;
            const int   LEN         = DATA[tj].d_outputLen;
            const bool  IS_VALID    = DATA[tj].d_isValid;

            if (!IS_VALID) {
                continue; // Cannot create `Json` objects from invalid input.
            }

            bsl::vector<char> vc(OUTPUT, OUTPUT + LEN);
            const bsl::vector<char>& VC = vc;

            test::HexBinarySequence exp;
            const test::HexBinarySequence& EXP = exp;
            exp.element1().array() = VC;

            test::HexBinarySequence value;

            bsl::ostringstream os;
            os << "{ \"element1\" : " << INPUT << '}';

            bsl::istringstream is(os.str());

            baljsn::DecoderOptions options;
            options.setValidateInputIsUtf8(UTF8);
            baljsn::Decoder        decoder;
            const int rc = decoder.decode(is, &value, options);
            if (veryVerbose) {
                P(decoder.loggedMessages());
            }

            if (IS_VALID) {
                LOOP2_ASSERT(LINE, rc, 0 == rc);

                bool result = EXP == value;
                LOOP3_ASSERT(LINE, EXP, value, result);
                if (!result) {
                    cout << "EXP: ";
                    bdlb::PrintMethods::print(cout, EXP, 0, -1);
                    cout << endl << "VALUE: ";
                    bdlb::PrintMethods::print(cout, value, 0, -1);
                    cout << endl;
                }

                Obj          converter;
                bdljsn::Json json;
                int          rcConvTo = converter.convert(&json, value);
                ASSERTV(rcConvTo, 0 == rcConvTo);
                ASSERTV(json.type(),             json.isObject());
                ASSERTV(json["element1"].type(), json["element1"].isString());

                test::HexBinarySequence valueFromJson;
                int                     rcConvFrom = converter.convert(
                                                                &valueFromJson,
                                                                json);
                ASSERTV(rcConvFrom, 0 == rcConvFrom);
                ASSERTV(value,   valueFromJson,
                        value == valueFromJson);
            }
            else {
                LOOP2_ASSERT(LINE, rc, rc);
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // CONVERTING VECTORS OF VECTORS
        //   This case tests that `baljsn::JsonConverter::convert` supports
        //   conversion of vectors of vectors.
        //
        // Concerns:
        // 1. Converting a vector of vectors of a `bdlat`-compatible type
        //    emits valid `bdljsn::Json` array of arrays.
        //
        // Plan:
        // 1. Perform a depth-ordered enumeration of converting all vectors of
        //    vectors of integers up to depth 2 and width 2, and verify that
        //    the resulting `bdljsn::Json` object contains the corresponding
        //    array of arrays.
        //
        // 2. For some terminal `bdlat` element types, enumerate a large
        //    number of different nested and non-nested arrays of these types
        //    and verify that the array converts to a corresponding nested or
        //    non-nested JSON array.
        //
        // Testing:
        //   CONCERN: CONVERTING VECTORS OF VECTORS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONVERTING VECTORS OF VECTORS" << endl
                          << "=============================" << endl;

        namespace ux = baljsn::encoder::u;

        typedef s_baltst::TestAttribute <1, ux::k_ATTRIBUTE_1_NAME>   At1;
        typedef s_baltst::TestAttribute <2, ux::k_ATTRIBUTE_2_NAME>   At2;
        typedef s_baltst::TestEnumerator<1, ux::k_ENUMERATOR_1_NAME>  En1;
        typedef s_baltst::TestSelection <1, ux::k_SELECTION_1_NAME>   Se1;

        const At1 at1;
        const At2 at2;
        const En1 en1;
        const Se1 se1;

        const s_baltst::TestPlaceHolder<int>                  ip;
        const s_baltst::GenerateTestArray                     a_;
        const s_baltst::GenerateTestArrayPlaceHolder          ap;
        const s_baltst::GenerateTestChoice                    c_;
        const s_baltst::GenerateTestChoicePlaceHolder         cp;
        const s_baltst::GenerateTestCustomizedType            t_;
        const s_baltst::GenerateTestEnumeration               e_;
        const s_baltst::GenerateTestEnumerationPlaceHolder    ep;
        const s_baltst::GenerateTestNullableValue             n_;
        const s_baltst::GenerateTestSequence                  s_;
        const s_baltst::GenerateTestSequencePlaceHolder       sp;

        const u::AssertEncodedArrayOfValuesIsEqualFunction TEST;

        //      LINE
        //     .----
        //    /  OBJECT TO ENCODE
        //   --  -------------------
        TEST(L_, 1                 );
        TEST(L_, a_(1)             );
        TEST(L_, c_(se1, 1)        );
        TEST(L_, t_(1, ip)         );
        TEST(L_, e_(en1, 1)        );
        TEST(L_, n_(ip)            );
        TEST(L_, n_(1)             );
        TEST(L_, s_(at1, 1)        );
        TEST(L_, s_(at1, at2, 1, 2));

        const u::AssertEncodedValueIsEqualFunction TEST2;

        //       LINE
        //      .----
        //     /  OBJECT TO CONVERT
        //    --  ----------------------------
        TEST2(L_, c_(se1,a_(ip))              );
        TEST2(L_, c_(se1,a_(1))               );
        TEST2(L_, c_(se1,a_(1,1))             );
        TEST2(L_, c_(se1,a_(1,1,1))           );
        TEST2(L_, s_(at1,a_(ip))              );
        TEST2(L_, s_(at1,a_(1))               );
        TEST2(L_, s_(at1,a_(1,1))             );
        TEST2(L_, s_(at1,a_(1,1,1))           );
        TEST2(L_, s_(at1,at2,a_(ip),a_(ip))   );
        TEST2(L_, s_(at1,at2,a_(1),a_(ip))    );
        TEST2(L_, s_(at1,at2,a_(1,1),a_(ip))  );
        TEST2(L_, s_(at1,at2,a_(1,1,1),a_(ip)));
        TEST2(L_, s_(at1,at2,a_(ip),a_(1))    );
        TEST2(L_, s_(at1,at2,a_(1),a_(1))     );
        TEST2(L_, s_(at1,at2,a_(1,1),a_(1))   );
        TEST2(L_, s_(at1,at2,a_(1,1,1),a_(1)) );

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // CONVERTING NULL CHOICE
        //   This case tests that `baljsn::JsonConverter:convertencode` returns
        //   a non-zero value if the supplied object contains a null choice.
        //
        // Concerns:
        // 1. When converting an object containing null nullable choice
        //    `convert` returns a non-zero value, a helpful error message,
        //    and does not trigger any assertions.
        //
        // Plan:
        // 1. Create a message object of type
        //    `s_baltst::MySequenceWithNullableAnonymousChoice` which
        //    is anonymous and optional (nullable).
        //
        // 2. Demonstrate the different behaviors between `encode` and
        //    `convert` for this object.
        //
        // Testing:
        //   CONCERN: ENCODING NULL CHOICE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONVERTING UNSET CHOICE" << endl
                          << "=======================" << endl;

        if (veryVerbose) cout << "`s_baltst::MySequenceWithChoice`" << endl;
        {
            bsl::ostringstream             out;
            s_baltst::MySequenceWithChoice object;
                // The type defines a choice -- actually,  a choice of one --
                // but none have been selected for `object`.

            Encoder     encoder;
            int         rcEncode    = encoder.encode(out, object);
            bsl::string jsonText    = out.str();
            bsl::string errorEncode = encoder.loggedMessages();
            ASSERT(0 !=  rcEncode);
            ASSERT("" != errorEncode);

            if (veryVerbose) {
                P(rcEncode); P(errorEncode); P(jsonText);
            }

            Json json;
            Obj  obj;
            const int            rcConvert = obj.convert(&json, object);
            const bsl::string errorConvert = obj.loggedMessages();

            ASSERTV(   rcConvert, 0  !=    rcConvert);
            ASSERTV(errorConvert, "" != errorConvert);

            const bsl::string expectedError =
                                     "Undefined selection for Choice object\n";
            if (veryVeryVerbose) {
                P(expectedError.size());
                P(errorConvert .size());

                P(expectedError);
                P(errorConvert);
            }

            ASSERTV(expectedError,   errorConvert,
                    expectedError == errorConvert);
        }

        // Important: Note that this test case has been (temporarily) modified
        // to verify that encoding a null nullable choice succeeds (returns 0)
        // and yields malformed json (e.g., "{null}").  It was found that a
        // client depended upon this behavior.

        // Important: While `encode` has freedom to generate an invalid
        // JSON document, `convert` cannot.  Consequently, `convert` handles
        // this case as an error.

        if (veryVerbose) cout <<
                   "`s_baltst::MySequenceWithNullableAnonymousChoice`" << endl;
        {
            bsl::ostringstream                              out;
            s_baltst::MySequenceWithNullableAnonymousChoice object;

            if (veryVeryVerbose) {
                P(object);
            }

            Encoder encoder;
            Options options; options.setEncodeNullElements(true);

            const int         rcEncode  = encoder.encode(out, object, options);
            const bsl::string logEncode = encoder.loggedMessages();

            ASSERTV( rcEncode, 0  ==  rcEncode);  // Encode passes.
            ASSERTV(logEncode, "" == logEncode);

            const bsl::string expectedJson = "{null}";
                                            // Note this value is not a
                                            // grammatically correct JSON
                                            // document.

            ASSERTV(out.str(),   expectedJson,
                    out.str() == expectedJson);

            if (veryVerbose) {
                P(rcEncode);
                P(logEncode);
                P(out.str());
            }

            const int         expectRc    = -666; // Convert should fail.
            const bsl::string expectLog =
                        "Unable to encode value of element named: 'Choice'.\n";

            Json              json;
            Obj               obj;
            const int         convertRc  = obj.convert(&json, object);
            const bsl::string convertLog = obj.loggedMessages();

            ASSERTV(expectRc,    convertRc,
                    expectRc  == convertRc);

            ASSERTV(expectLog,   convertLog,
                    expectLog == convertLog);

            // Since we cannot construct '{null}' the reverse conversion
            // cannot be tested.
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // CONVERTING UNSET CHOICE
        //   This case tests that `baljsn::JsonConverter::convert` returns a
        //   non-zero value if the supplied object contains an unset choice.
        //
        // Concerns:
        // 1. When converting an object containing an unset choice, the convert
        //    returns a non-zero value, an appropriate error message, and does
        //    not trigger any assertions.
        //
        // 2. The return code and error message of the `convert` function
        //    matches that from the encoder.
        //
        // Plan:
        // 1. Create a message object of type `MySequenceWithChoice` which
        //    contains an unset choice in its sub-object of type
        //    `MySequenceWithChoice` and ensure than `convert` returns a
        //    non-zero value.
        //
        // Testing:
        //   CONCERN: CONVERTING UNSET CHOICE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONVERTING UNSET CHOICE" << endl
                          << "=======================" << endl;
        {
            bsl::ostringstream                          out;
            BloombergLP::s_baltst::MySequenceWithChoice object;

            int selectionId = object.mode().selectionId();
            ASSERTV(      selectionId,
                    -1 == selectionId);  //

            if (veryVeryVerbose) {
                P(object);
            }

            Encoder           encoder;
            const int         rcEncode  = encoder.encode(out, object);
            const bsl::string logEncode = encoder.loggedMessages();

            const bsl::string expectedError =
                                     "Undefined selection for Choice object\n";

            ASSERTV(                  rcEncode,
                    0             !=  rcEncode);
            ASSERTV(expectedError,   logEncode,
                    expectedError == logEncode);

            if (veryVerbose) {
                P(rcEncode); P(logEncode); P(out.str());
            }

            Json json;
            Obj  converter;

            const int          rcConvert = converter.convert(&json, object);
            const bsl::string logConvert = converter.loggedMessages();

            ASSERTV(             rcConvert,
                    0         != rcConvert);
            ASSERTV(logEncode,   logConvert,
                    logEncode == logConvert);

            if (veryVerbose) {
                P(rcConvert); P(logConvert); P(json);
            }

            ASSERT( rcEncode ==  rcConvert);
            ASSERT(logEncode == logConvert);

            ASSERT(json.isObject());
            ASSERT(0 == json.theObject().size());

            BloombergLP::s_baltst::MySequenceWithChoice objectFromJson;
            int rcFromJson = converter.convert(&objectFromJson, json);

            ASSERTV(rcFromJson, 0 == rcFromJson);
            ASSERTV(object,   objectFromJson,
                    object == objectFromJson);
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // DEGENERATE CHOICE VALUES
        //   This case tests that the encoder emits valid JSON when encoding
        //   choices that have active selections with the "untagged" formatting
        //   mode and the empty value.  More specifically, that the encoder
        //   encodes such choices as an empty object "{}".  Note that, while
        //   the encoded JSON is valid, it is not in possible for the decoder
        //   to decode the encoded representation back to the same value
        //   because the member name that is ordinarily required to
        //   disambiguate the selection is not present.
        //
        // Concerns:
        // 1. The encoder emits empty-object token sequences for choice values
        //    having an empty selection with the untagged formatting mode.
        //
        // Plan:
        // 1. Define a choice type that has 1 selection, which is an empty,
        //    anonymous sequence.
        //
        // 2. Compare the converted `Json` object to one obtained from the
        //    oracle.
        //
        // Testing:
        //   CONCERN: DEGENERATE CHOICE VALUES
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DEGENERATE CHOICE VALUES" << endl
                          << "========================" << endl;

        // `Type0` is unsupported and causes the encoder to violate the
        // invariants of the formatter.  This was last verified April 20, 2020.
     // typedef baljsn::EncoderTestDegenerateChoice0 Type0;
        typedef baljsn::EncoderTestDegenerateChoice1 Type1;

        // Do not test degenerate choice type 0.
     // Type0 obj0;
     // obj0.makeChoice();

        Type1  obj1;
        obj1.makeSequence();

        if (veryVeryVerbose) P(obj1);  // An empty sequence.

        Json jsonExpect;
        int  rcExpect = u::Oracle::toJsonFromObj(&jsonExpect, obj1);
        ASSERTV(rcExpect, 0 == rcExpect);

        if (veryVeryVerbose) P(jsonExpect);

        Json jsonActual;
        Obj  converter;
        int  rcActual = converter.convert(&jsonActual, obj1);           // TEST

        ASSERTV(rcActual, 0 == rcActual);
        ASSERTV(      converter.loggedMessages(),
                "" == converter.loggedMessages());

        if (veryVeryVerbose) {
            P(jsonExpect);
            P(jsonActual);
        }

        ASSERTV(jsonExpect,   jsonActual,
                jsonExpect == jsonActual);  // Note: They are both empty
                                            // `JsonObject`s.

        ASSERTV(jsonActual.type(),      jsonActual.isObject())
        ASSERTV(jsonActual.size(), 0 == jsonActual.size());

        if (veryVeryVerbose) {
            P(jsonActual);
        }

        Type1 obj1FromJson;
        int   rcFromJson = converter.convert(&obj1FromJson, jsonActual); //TEST

        ASSERTV(rcFromJson, 0 == rcFromJson);      // No problem converting.
        ASSERTV(      converter.loggedMessages(),  // Nothing to be set to
                "" == converter.loggedMessages()); // `obj1FromJson`.

        ASSERTV(Type1(),   obj1FromJson,           // `obj1FromJson` is left in
                Type1() == obj1FromJson);          // default state
        ASSERTV(obj1,      obj1FromJson,           // ... and *not* equal to
                obj1    != obj1FromJson);          // `obj1`.

        // What if the `Json` object is empty but of another `Json` type?

        Json jsonAlternate; jsonAlternate.makeArray();

        ASSERTV(jsonAlternate.type(),      jsonAlternate.isArray())
        ASSERTV(jsonAlternate.size(), 0 == jsonAlternate.size());

        if (veryVeryVerbose) {
            P(jsonAlternate);
        }

        Type1 obj1FromJsonAlternate;
        int   rcFromJsonAlternate = converter.convert(&obj1FromJsonAlternate,
                                                      jsonAlternate);    //TEST

        const char *msgExp = "Could not decode choice, missing starting {\n";

        ASSERTV(rcFromJsonAlternate, 0      != rcFromJsonAlternate);
        ASSERTV(                               converter.loggedMessages(),
                                     msgExp == converter.loggedMessages());
        if (veryVeryVerbose) {
            P(rcFromJsonAlternate);
            P(converter.loggedMessages());
        }

        ASSERTV(Type1(),   obj1FromJson,
                Type1() == obj1FromJson);
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // SEQUENCE WITH ATTRIBUTES OF ALL VALUE CATEGORIES
        //   This case tests that the encoder emits valid JSON when encoding
        //   sequence attributes having any of the seve `bdlat` value
        //   categories that the encoder supports for attributes.
        //
        // Concerns:
        // 1. Regardless of value category, the encoder emits a valid and
        //    correct JSON representation of a sequence attribute.
        //
        // Plan:
        // 1. Define a sequence type that has seven attributes whose types
        //    cover all seve `bdlat` value categories that are supported for
        //    selections.
        //
        //    * See 1baljsn::EncoderTestSequenceWithAllCategories1 from
        //      `baljsn_encoder_testtypes`.
        //
        // 2. Compare the converted `Json` object to one obtained from the
        //    oracle.
        //
        // Testing:
        //   CONCERN: SEQUENCE WITH ATTRIBUTES OF ALL VALUE CATEGORIES
        // --------------------------------------------------------------------

        if (verbose) cout
                 << endl
                 << "SEQUENCE WITH ATTRIBUTES OF ALL VALUE CATEGORIES" << endl
                 << "================================================" << endl;

        typedef baljsn::EncoderTestSequenceWithAllCategories Type;

        Type obj;
        obj.choice().makeSelection0();

        Json jsonExpect;
        int rcExpect = u::Oracle::toJsonFromObj(&jsonExpect, obj);
        ASSERTV(rcExpect, 0 == rcExpect);

        Json jsonActual;
        Obj  converter;
        int  rcActual = converter.convert(&jsonActual, obj);

        ASSERTV(rcActual, 0 == rcActual);
        ASSERTV(               converter.loggedMessages(),
                ""          == converter.loggedMessages());

        ASSERTV(jsonExpect,    jsonActual,
                jsonExpect  == jsonActual);

        if (veryVeryVerbose) {
            P(jsonExpect);
            P(jsonActual);
        }

        Type objFromJson;
        int  rcFromJson = converter.convert(&objFromJson, jsonActual);
        ASSERTV(rcFromJson, 0 == rcFromJson);
        ASSERTV(                 converter.loggedMessages(),
                ""            == converter.loggedMessages());

        ASSERTV(obj,    objFromJson,
                obj  == objFromJson);

        if (veryVerbose) {
            Q(Add Extra Stuff);
        }

        Json extraObject;
        extraObject.makeObject();
        extraObject["moe"  ] = true;
        extraObject["larry"] = 1.0;
        extraObject["curly"] = "Jerome";
        extraObject["shemp"] = bdljsn::JsonNull();

        Json extraArray;
        extraArray.makeArray();
        extraArray.theArray().pushBack(Json(false));
        extraArray.theArray().pushBack(Json(2.0));
        extraArray.theArray().pushBack(Json("Jerry"));
        extraArray.theArray().pushBack(Json(bdljsn::JsonNull()));

        Json jsonWithExtra          = jsonActual;
        jsonWithExtra["extraArray" ] = extraArray;
        jsonWithExtra["extraObject"] = extraObject;
        jsonWithExtra["extraScalar"] = "extra simple value";

        if (veryVeryVerbose) {
            P(jsonWithExtra);
        }

        if (veryVerbose) {
            Q(from JsonWithExtra and default options);
        }

        baljsn::DecoderOptions defaultDecoderOptions;

        if (veryVeryVerbose) {
            P(defaultDecoderOptions);
        }

        if (veryVerbose) {
            Q(convert to objFromJsonWithExtra);
        }

        Type objFromJsonWithExtra;
        int   rcFromJsonWithExtra = converter.convert(&objFromJsonWithExtra,
                                                      jsonWithExtra,
                                                      defaultDecoderOptions);
                                                                        // TEST
        ASSERTV(      rcFromJsonWithExtra,
                0  == rcFromJsonWithExtra);
        ASSERTV(      converter.loggedMessages(),
                "" == converter.loggedMessages());

        ASSERTV(objFromJson,    objFromJsonWithExtra,
                objFromJson  == objFromJsonWithExtra);

        baljsn::DecoderOptions adjustedDecoderOptions;
        adjustedDecoderOptions.setSkipUnknownElements(false);

        if (veryVeryVerbose) {
            P(adjustedDecoderOptions);
        }

        Q(convert to objFromJsonWithExtra with adjusted);

        rcFromJsonWithExtra = converter.convert(&objFromJsonWithExtra,
                                                jsonWithExtra,
                                                adjustedDecoderOptions);// TEST

        if (veryVerbose) {
            Q(returned from convert with adjusted);
        }

        const char *const expectedMsgs[] = {
                                      "Unknown element 'extraScalar' found\n",
                                      "Unknown element 'extraObject' found\n",
                                      "Unknown element 'extraArray' found\n" };

        ASSERTV(     rcFromJsonWithExtra,
                0 != rcFromJsonWithExtra);
        ASSERTV(                    converter.loggedMessages(),
              (  expectedMsgs[0] == converter.loggedMessages()
              || expectedMsgs[1] == converter.loggedMessages()
              || expectedMsgs[2] == converter.loggedMessages()
              ));  // Member order of JsonObjects is unspecified.

        if (veryVerbose) {
            Q("passed ASSERTV tests");
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // CHOICES WITH SELECTIONS OF ALL VALUE CATEGORIES
        //   This case tests that the encoder emits valid JSON when encoding
        //   a choice selection having any of the six `bdlat` value categories
        //   that the encoder supports for selections.
        //
        // Concerns:
        // 1. Regardless of value category, the converter supplies a valid and
        //    correct JSON representation of a choice selection.
        //
        // Plan:
        // 1. Define a choice type that has 6 selections whose types cover
        //    all six `bdlat` value categories that are supported for
        //    selections.
        //
        // 2. For of the six possible choices, convert the `bdlat` object
        //    to a `Json` object and compare to one obtained from the
        //    oracle function.
        //
        // Testing:
        //   CONCERN: CHOICES WITH SELECTIONS OF ALL VALUE CATEGORIES
        // --------------------------------------------------------------------

        if (verbose) cout
                  << endl
                  << "CHOICES WITH SELECTIONS OF ALL VALUE CATEGORIES" << endl
                  << "===============================================" << endl;

        const u::AssertEncodedValueIsEqualFunction t;

        typedef baljsn::EncoderTestChoiceWithAllCategories Type;

        Type obj0;
        Type obj1;
        Type obj2;
        Type obj3;
        Type obj4;
        Type obj5;

        obj0.makeCharArray();
        obj1.makeChoice();
        obj1.choice().makeSelection0();
        obj2.makeCustomizedType();
        obj3.makeEnumeration();
        obj4.makeSequence();
        obj5.makeSimple();

        t( L_  , obj0 );
        t( L_  , obj1 );
        t( L_  , obj2 );
        t( L_  , obj3 );
        t( L_  , obj4 );
        t( L_  , obj5 );
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // SEQUENCES WITH EMPTY ELEMENTS
        //   This case tests that the encoder emits valid JSON when encoding
        //   sequences having attributes with untagged-empty-sequence type.
        //
        // Concerns:
        // 1. The encoder emits no tokens for sequence attributes having
        //    untagged-empty-sequence type.
        //
        // Plan:
        // 1. Define 14 types that enumerate the first 15 elements of the
        //    depth-ordered enumeration of sequences having attributes of
        //    either integer or untagged-empty-sequence type.
        //
        //    * See `baljsn::EncoderTestSequenceWithUntagged*` in
        //      `baljsn_encoder_testtypes`.
        //
        // 2. Convert each of the objects to a `Json` object and compare
        //    to the `Json` object from the oracle function.
        //
        // Testing:
        //   CONCERN: SEQUENCES WITH EMPTY ELEMENTS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SEQUENCES WITH EMPTY ELEMENTS" << endl
                          << "=============================" << endl;

        const u::AssertEncodedValueIsEqualFunction t;

        typedef baljsn::EncoderTestSequenceWithUntagged0  T0;
        typedef baljsn::EncoderTestSequenceWithUntagged1  T1;
        typedef baljsn::EncoderTestSequenceWithUntagged2  T2;
        typedef baljsn::EncoderTestSequenceWithUntagged3  T3;
        typedef baljsn::EncoderTestSequenceWithUntagged4  T4;
        typedef baljsn::EncoderTestSequenceWithUntagged5  T5;
        typedef baljsn::EncoderTestSequenceWithUntagged6  T6;
        typedef baljsn::EncoderTestSequenceWithUntagged7  T7;
        typedef baljsn::EncoderTestSequenceWithUntagged8  T8;
        typedef baljsn::EncoderTestSequenceWithUntagged9  T9;
        typedef baljsn::EncoderTestSequenceWithUntagged10 T10;
        typedef baljsn::EncoderTestSequenceWithUntagged11 T11;
        typedef baljsn::EncoderTestSequenceWithUntagged12 T12;
        typedef baljsn::EncoderTestSequenceWithUntagged13 T13;
        typedef baljsn::EncoderTestSequenceWithUntagged14 T14;

        const T0  obj0;
        const T1  obj1;
        const T2  obj2;
        const T3  obj3;
        const T4  obj4;
        const T5  obj5;
        const T6  obj6;
        const T7  obj7;
        const T8  obj8;
        const T9  obj9;
        const T10 obj10;
        const T11 obj11;
        const T12 obj12;
        const T13 obj13;
        const T14 obj14;

        t( L_, obj0  );
        t( L_, obj1  );
        t( L_, obj2  );
        t( L_, obj3  );
        t( L_, obj4  );
        t( L_, obj5  );
        t( L_, obj6  );
        t( L_, obj7  );
        t( L_, obj8  );
        t( L_, obj9  );
        t( L_, obj10 );
        t( L_, obj11 );
        t( L_, obj12 );
        t( L_, obj13 );
        t( L_, obj14 );

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // LOG BUFFER CLEARS ON EACH `convert` CALL
        //
        // Concerns:
        // 1. The string returned from `loggedMessages` resets each time
        //    `convert` is invoked, such that the contents of the logged
        //    messages refer to only the most recent invocation of `encode`.
        //
        // Plan:
        // 1. Create two objects of different types that
        //    `baljsn::JsonConverter` always successfully converts.
        //
        // 2. Create two objects of different types that
        //    `baljsn::JsonConverter` always fails to convert, and which cause
        //    a failure as early in the encoding process as possible.
        //    (Generally this means that each object will implement a `bdlat`
        //    category that the encoder does not support).
        //
        // 3. Verify that, after performing various sequences of encoding
        //    operations where some/all succeed/fail, the `loggedMessages` are
        //    empty if the last operation succeeds, and contain an expected
        //    message if and only if the last operation fails.
        //
        // Testing:
        //   bsl::string loggedMessages() const;
        //   CONCERN: LOG BUFFER CLEARS ON EACH `convert` to JSON CALL
        // --------------------------------------------------------------------

        if (veryVerbose) cout
                         << endl
                         << "LOG BUFFER CLEARS ON EACH `convert` CALL" << endl
                         << "========================================" << endl;

        // `mS1` is a modifiable object for which
        // `baljsn::JsonConverter::convert` will succeed.
        s_baltst::Address mS1;
        mS1.street() = "1st";
        mS1.city()   = "New York";
        mS1.state()  = "New York";

        // `S1` is a non-modifiable reference to `mS1`.
        const s_baltst::Address& S1 = mS1;

        // `mS2` is a modifiable object for which
        // `baljsn::JsonConverter::convert` will succeed.
        s_baltst::Employee mS2;
        mS2.name() = "John Doe";
        mS2.homeAddress() = S1;
        mS2.age() = 50;

        // `S2` is a non-modifiable reference to `mS2`.
        const s_baltst::Employee& S2 = mS2;

        // `mF1` is a modifiable object for which
        // `baljsn::JsonConverter::convert` will fail very early in the
        // encoding process.
        u::Enumeration0 mF1;

        // `F1` is a non-modifiable reference to `mF1`.
        const u::Enumeration0& F1 = mF1;

        // `mF2` is a modifiable object for which
        // `baljsn::JsonConverter::convert` will fail very early in the
        // encoding process.
        bdlb::NullableValue<s_baltst::Address> mF2;

        // `F2` is a non-modifiable reference to `mF2`.
        const bdlb::NullableValue<s_baltst::Address>& F2 = mF2;

        enum Instruction {
            // This enumeration provides a set of integer constants that
            // indicate individual operations that may be performed by the
            // testing apparatus.

            NOOP = 0, // indicates to do nothing (no operation)
            ES1,      // indicates to convert `S1`
            ES2,      // indicates to convert `S2`
            EF1,      // indicates to convert `F1`
            EF2       // indicates to convert `F2`
        };

        enum {
            k_MAX_INSTRUCTIONS = 3 // maximum number of instructions that may
                                   // be performed in one row of the table
                                   // that drives the testing apparatus
        };

        enum {
            // This enumeration provides aliases for whether all encoding
            // operations in a test row shall succeed, or if at least one shall
            // fail.

            failure = false, // indicates that at least one convert op fails
            success = true   // indicates that all convert ops succeed
        };

        // `SMsg1` is a string that is equivalent to the `loggedMessages`
        // of a `baljsn::JsonConverter` after a successful conversion
        // operation.
        static const char SMsg1[] = "";

        // `FMsg1` is a string that is equivalent to the `loggedMessages`
        // of a `baljsn::Encoder` after an encoding operation that fails
        // due to the type of the target object having an unsupported
        // `bdlat` category.
        static const char FMsg1[] =
            "Encoded object must be a Sequence, Choice, or Array type.\n";

        /// line number
        static const struct {
            int         d_line;

            /// instructions for test apparatus
            Instruction d_instructions[k_MAX_INSTRUCTIONS];

            /// whether all operations succeed
            bool        d_convertSuccessStatus;

            /// messages from final operation
            const char *d_loggedMessages;

        } DATA[] = {
            //LINE    INSTRUCTIONS     STATUS  LOGGED MESSAGES
            //---- ------------------ -------- ---------------
            {   L_, {               }, success,         SMsg1 },
                // Verify that the `loggedMessages` are empty if no encoding
                // operations are performed.

            {   L_, { ES1           }, success,         SMsg1 },
            {   L_, { ES2           }, success,         SMsg1 },
                // Verify that the `loggedMessages` are empty if one
                // encoding operation is performed, and that operation
                // succeeds.

            {   L_, { EF1           }, failure,         FMsg1 },
            {   L_, { EF2           }, failure,         FMsg1 },
                // Verify that the `loggedMessages` have an expected message
                // if one encoding operation is performed, and that operation
                // fails.

            {   L_, { ES1, ES1      }, success,         SMsg1 },
            {   L_, { ES1, ES2      }, success,         SMsg1 },
            {   L_, { ES1, EF1      }, failure,         FMsg1 },
            {   L_, { ES1, EF2      }, failure,         FMsg1 },
            {   L_, { ES2, ES1      }, success,         SMsg1 },
            {   L_, { ES2, ES2      }, success,         SMsg1 },
            {   L_, { ES2, EF1      }, failure,         FMsg1 },
            {   L_, { ES2, EF2      }, failure,         FMsg1 },
            {   L_, { EF1, ES1      }, failure,         SMsg1 },
            {   L_, { EF1, ES2      }, failure,         SMsg1 },
            {   L_, { EF1, EF1      }, failure,         FMsg1 },
            {   L_, { EF1, EF2      }, failure,         FMsg1 },
            {   L_, { EF2, ES1      }, failure,         SMsg1 },
            {   L_, { EF2, ES2      }, failure,         SMsg1 },
            {   L_, { EF2, EF1      }, failure,         FMsg1 },
            {   L_, { EF2, EF2      }, failure,         FMsg1 },
                // Verify that the `loggedMessages` have an expected message
                // when, after performing 2 encoding operations, the second
                // operation fails, and otherwise are empty.

            {   L_, { ES1, ES1, ES1 }, success,         SMsg1 },
            {   L_, { ES1, ES1, EF1 }, failure,         FMsg1 },
            {   L_, { ES1, EF1, ES1 }, failure,         SMsg1 },
            {   L_, { EF1, ES1, ES1 }, failure,         SMsg1 }
                // Verify that the `loggedMessages` have an expected message
                // when the last encoding operation in a sequence of encoding
                // operations fails, and otherwise are empty.
        };

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i != NUM_DATA; ++i) {
            const int          LINE          = DATA[i].d_line;
            const Instruction *INSTRUCTIONS  = DATA[i].d_instructions;
            const bool CONVERT_SUCCESS_STATUS
                                             = DATA[i].d_convertSuccessStatus;
            const bsl::string_view& LOGGED_MESSAGES
                                             = DATA[i].d_loggedMessages;

            if (veryVerbose) {
                P_(LINE); P_(CONVERT_SUCCESS_STATUS); P(LOGGED_MESSAGES);
            }

            Obj mX; const Obj& X = mX;

            int convertStatus = 0;

            for (const Instruction *instructionPtr  = &INSTRUCTIONS[0];
                                    instructionPtr != &INSTRUCTIONS[0]
                                                    + k_MAX_INSTRUCTIONS;
                                  ++instructionPtr) {
                switch (*instructionPtr) {
                  case NOOP: {
                      // do nothing
                  } break;
                  case ES1: {
                    Json json;
                    convertStatus |= mX.convert(&json, S1);
                  } break;
                  case ES2: {
                    Json json;
                    convertStatus |= mX.convert(&json, S2);
                  } break;
                  case EF1: {
                    Json json;
                    convertStatus |= mX.convert(&json, F1);
                  } break;
                  case EF2: {
                    Json json;
                    convertStatus |= mX.convert(&json, F2);
                  } break;
                }
            }

            ASSERTV(LINE,
                    convertStatus,
                    CONVERT_SUCCESS_STATUS ? convertStatus == 0
                                           : convertStatus != 0);
            ASSERTV(LINE,
                    LOGGED_MESSAGES,   X.loggedMessages(),
                    LOGGED_MESSAGES == X.loggedMessages());
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // NULL ELEMENTS
        //
        // Concerns:
        // 1. The converter always generates a `bdljsn::JsonNull` element
        //    for each null element in the input message.
        //
        // Plan:
        // 1. Construct a table that provies a test set of `bldat` values in
        //    XML representation.
        //
        //    * In particular, this table has entries where `element6` (a
        //      `nillable` enumerated type) is specified but not set.
        //
        // 2. For each entry in the table (P-1) create a `bdlb::Sequence3`
        //    object can convert it to a `Json` object.
        //
        // 3. Compare the converted `Json` object to one obtained from the
        //    oracle.
        //
        // Plan:
        //
        // Testing:
        //   CONERN: NULL ELEMENTS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "NULL ELEMENTS"
                          << "=============" << endl;

        static const struct {
            int         d_line;        // source line number
            const char *d_xmlText_p;   // xml text
        } DATA[] = {
            {
                L_,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "</element1>\n"
            },
            {
                L_,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "  <element6/>\n"
                "  <element6/>\n"
                "</element1>\n"
            },
            {
                L_,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "  <element3>true</element3>\n"
                "  <element6/>\n"
                "  <element6/>\n"
                "</element1>\n"
            },
            {
                L_,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "  <element3>true</element3>\n"
                "  <element4>arbitrary string value</element4>\n"
                "  <element6/>\n"
                "  <element6/>\n"
                "</element1>\n"
            }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (veryVeryVerbose) P(NUM_DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE = DATA[ti].d_line;
            const bsl::string XML  = DATA[ti].d_xmlText_p;

            if (veryVeryVerbose) {
                P_(LINE); P(XML);
            }

            balb::Sequence3 object;
            ASSERT(0 == u::populateTestObject(&object, XML));

            Encoder encoder;
            Options options; const Options& mO = options;
            options.setEncodeNullElements(true);
            options.setEncodeEmptyArrays (true);

            bsl::ostringstream oss;
            ASSERTV(0 == encoder.encode(oss, object, mO));

            Json  expectJson;
            Error error;
            int   expectRc = u::Oracle::toJsonFromTxt(&expectJson,
                                                      &error,
                                                      oss.str());
            ASSERTV(expectRc, 0       == expectRc);
            ASSERTV(error,    Error() == error);

            Obj  converter;
            Json actualJson;
            int  actualRc = converter.convert(&actualJson, object);     // TEST
            ASSERTV(actualRc, 0 == actualRc);

            if (veryVeryVerbose) {
                P(expectJson);
                P(actualJson);
            }

            ASSERTV(expectJson,   actualJson,
                    expectJson == actualJson);

            balb::Sequence3 objectFromJson;

            int rcFromJson = converter.convert(&objectFromJson,
                                               actualJson);             // TEST

                ASSERTV(rcFromJson, 0 == rcFromJson);
                ASSERTV(      converter.loggedMessages(),
                        "" == converter.loggedMessages());

                ASSERTV(object,   objectFromJson,
                        object == objectFromJson);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // COMPLEX TEST MESSAGES
        //
        // Concerns:
        // 1. The complex test messages provided by
        //    `s_baltst::FeatureTestMessage>` can be converted to `Json`
        //    objects.
        //
        // Plan:
        // 1. Use `u::constructFeatureTestMessage` to load a vector of test
        //    objects.
        //
        // 2. Convert each test object and compare the resulting
        //    `Json` object with one obtained from the oracle function.
        //
        // Testing:
        //   CONCERN: COMPLEX TEST MESSAGES
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COMPLEX TEST MESSAGES" << endl
                          << "=====================" << endl;

        bsl::vector<s_baltst::FeatureTestMessage> testObjects;
        u::constructFeatureTestMessage(&testObjects);

        typedef s_baltst::FeatureTestMessageUtil MessageUtil;

        for (int i = 0; i != MessageUtil::k_NUM_MESSAGES; ++i) {

        // See {DRQS 178183006<GO>} regarding 'truenull'.
#if   defined(BSLS_PLATFORM_OS_WINDOWS)
        const int problematicMessage = 44;
#elif defined(BSLS_PLATFORM_OS_UNIX)
        const int problematicMessage = 46;
#else
      #error Unexpected platform
#endif
        if (problematicMessage == i) {
                continue;
            }

            const bsl::string EXP = MessageUtil::s_PRETTY_JSON_MESSAGES[i];

            if (veryVerbose) {
                P_(i) P(testObjects[i]);
            }

            Obj     converter;
            Encoder encoder;
            Options options;
            options.setEncodeEmptyArrays (true);
            options.setEncodeNullElements(true);

            {
                bdlsb::MemOutStreamBuf osb;
                int rcEncode = encoder.encode(&osb, testObjects[i], options);
                ASSERTV(rcEncode, 0 == rcEncode);

                const bsl::string JSON_TEXT(osb.data(), osb.length());

                Json          jsonOracle;
                bdljsn::Error error;
                int           rcOracle = u::Oracle::toJsonFromTxt(&jsonOracle,
                                                                  &error,
                                                                  JSON_TEXT);
                if (0 != rcOracle) {
                    P(i);
                    P(JSON_TEXT);
                    P(error);
                    P(jsonOracle);
                }

                ASSERTV(rcOracle, 0 == rcOracle);

                Json jsonActual;
                int    rcActual = converter.convert(&jsonActual,
                                                    testObjects[i]);    // TEST
                ASSERTV(rcActual, 0 == rcActual);

                ASSERTV(jsonOracle,   jsonActual,
                        jsonOracle == jsonActual);

                if (veryVeryVerbose) {
                    P(jsonOracle);
                    P(jsonActual);
                }

                s_baltst::FeatureTestMessage ftmFromJson;

                int rcFromJson = converter.convert(&ftmFromJson,
                                                   jsonActual);         // TEST

                ASSERTV(rcFromJson, 0 == rcFromJson);
                ASSERTV(      converter.loggedMessages(),
                        "" == converter.loggedMessages());

                ASSERTV(testObjects[i],   ftmFromJson,
                        testObjects[i] == ftmFromJson);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR AND ACCEPTED CATEGORIES
        //
        // Concerns:
        // 1. The `JsonConverter` constructor installs the specified allocator
        //    and leaves a clear log.
        //
        // 2. QoI: Asserted precondition violations are detected when enabled.
        //
        // 3. `convert` accepts works for `bdlat` classes in `Sequence`,
        //    `Choice`, and `Array` categories and returns an error otherwise.
        //     (i.e., `bsl::vector`) types.
        //
        // 4. The output object (a `bdljsn::Json`) can be of any `Json` type.
        //
        // Plan:
        // 1. Use the "footprint" idiom to confirm that he intended allocator
        //    is installed and used by a constructed `JsonConverter` object.
        //
        // 2. Create an instance of each of the required categories and
        //    confirm that `convert` returns a status of zero and a clear log.
        //
        //    1. Confirm that the output object, `bdljsn::Json1 object, can
        //       be of any initial type without changing the result.
        //
        // Testing:
        //   CONCERN: CONSTRUCTOR AND ACCEPTED CATEGORIES
        //   JsonConverter();
        //   explicit JsonConverter(const allocator_type& allocator);
        //   allocator_type get_allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONSTRUCTOR AND ACCEPTED CATEGORIES" << endl
                          << "===================================" << endl;

        if (veryVerbose) cout << "Test CTOR" << endl;
        {
            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                if (veryVeryVerbose) {
                    P(CONFIG);
                }

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);

                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj *objPtr =
                        'a' == CONFIG ? new (fa) Obj()                        :
                        'b' == CONFIG ? new (fa) Obj(Obj::allocator_type())   :
                        'c' == CONFIG ? new (fa) Obj(&sa)                     :
                        'd' == CONFIG ? new (fa) Obj(Obj::allocator_type(&sa)):
                        /* default */   0                                     ;
                ASSERTV(CONFIG, objPtr);

                bslma::TestAllocator *objAllocatorPtr = 'a' == CONFIG ? &da :
                                                        'b' == CONFIG ? &da :
                                                        'c' == CONFIG ? &sa :
                                                        'd' == CONFIG ? &sa :
                                                        /* default */   0   ;
                ASSERTV(CONFIG, objAllocatorPtr);

                Obj&                   mX = *objPtr;  const Obj& X = mX;
                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

                ASSERTV(CONFIG,                     &oa == X.get_allocator());
                ASSERTV(CONFIG, X.loggedMessages(), ""  == X.loggedMessages());

                s_baltst::MySequence mySequence;
                Json                 json;

                bslma::TestAllocatorMonitor  oam(& oa);
                bslma::TestAllocatorMonitor noam(&noa);

                int rc = mX.convert(&json, mySequence);
                ASSERTV(CONFIG, rc,                 0  == rc);
                ASSERTV(CONFIG, X.loggedMessages(), "" == X.loggedMessages());

                ASSERTV(CONFIG,  oam.isTotalUp());

                if ('c' == CONFIG
                 || 'd' == CONFIG) {
                    ASSERTV(CONFIG, noam.isTotalUp());
                        // `noam` is default allocator in this `CONFIG`;
                        // however, the default allocator is always
                        // intentionally used for function temporaries in the
                        // `convert` method.
                } else {
                    ASSERTV(CONFIG, noam.isTotalSame());
                }

                fa.deleteObject(objPtr);
            }
        }

        if (veryVerbose) cout << "CTOR: Negative Testing" << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj obj;

            ASSERT_FAIL(obj.convert(0,     balb::VoidSequence()));  // TEST

            Json json; json.makeObject();
            ASSERT_PASS(obj.convert(&json, balb::VoidSequence()));  // TEST
            ASSERT_FAIL(obj.convert(0,     json));                  // TEST
        }

        if (verbose) cout << "Are required categories are accepted?" << endl;
        {
            const Json initValues[] = { Json(JsonNull())
                                      , Json(true)
                                      , Json(JsonNumber())
                                      , Json("string")
                                      , Json(JsonArray())
                                      , Json(JsonObject())
                                      };
            const bsl::size_t NUM_ELEMENTS = sizeof  initValues
                                           / sizeof *initValues;
            ASSERTV(NUM_ELEMENTS, 6 == NUM_ELEMENTS);

            for (bsl::size_t i = 0; i < NUM_ELEMENTS; ++i) {

                const Json initValue = initValues[i];

                {
                    Obj obj;
                    ASSERTV("" == obj.loggedMessages());
                }
                {   // OK: Choice
                    s_baltst::MyChoice myChoice; myChoice.makeSelection(0);

                    Obj         obj;
                    Json        json           = initValue;
                    int         rc             =  obj.convert(&json, myChoice);
                    bsl::string loggedMessages =  obj.loggedMessages();
                    ASSERTV(rc,             0  == rc);
                    ASSERTV(loggedMessages, "" == loggedMessages);
                }
                {   // OK: Sequence
                    s_baltst::MySequence mySequence;

                    Obj         obj;
                    Json        json            = initValue;
                    int         rc              = obj.convert(&json,
                                                              mySequence);
                    bsl::string loggedMessages  = obj.loggedMessages();
                    ASSERTV(rc,             0  == rc);

                }
                {   // OK: Array (i.e., `bsl::vector` of some other type).
                    typedef s_baltst::MyIntEnumeration MIE;
                    typedef             MIE  ElementType;
                    typedef bsl::vector<MIE> MyVector;

                    ElementType zero; const ElementType& Zero = zero;
                    ElementType one;  const ElementType& One  = one;

                    zero.fromInt(0);
                    one .fromInt(1);

                    MyVector myVector;

                    myVector.push_back(Zero);
                    myVector.push_back(One );
                    myVector.push_back(Zero);

                    Obj         obj;
                    Json        json            = initValue;
                    int         rc              = obj.convert(&json, myVector);
                    bsl::string loggedMessages  = obj.loggedMessages();
                    ASSERTV(rc,             0  == rc);
                    ASSERTV(loggedMessages, "" == loggedMessages);

                    MyVector myVectorFromJson;

                    int         rcFromJson      = obj.convert(
                                                             &myVectorFromJson,
                                                             json);
                    bsl::string loggedMessagesFromJson  = obj.loggedMessages();
                    ASSERTV(      rcFromJson,
                            0  == rcFromJson);
                    ASSERTV(      loggedMessagesFromJson,
                            "" == loggedMessagesFromJson);

                }
                {   // NG: Scalar value
                    int          intValue       = 0;
                    Obj          converter;
                    Json         json           = initValue;
                    int          rc             = converter.convert(&json,
                                                                    intValue);
                    bsl::string loggedMessages  = converter.loggedMessages();
                    ASSERTV(rc,             0  != rc);
                    ASSERTV(loggedMessages, "" != loggedMessages);

                    if (veryVerbose) {
                        P(loggedMessages);
                    }

                    int intValueFromJson;
                    int rcFromJson  = converter.convert(&intValueFromJson,
                                                             json);
                    bsl::string loggedMessagesFromJson = converter
                                                             .loggedMessages();
                    ASSERTV(      rcFromJson,
                            0  != rcFromJson);
                    ASSERTV(      loggedMessagesFromJson,
                            "" != loggedMessagesFromJson);

                    ASSERTV(initValue,   json,
                            initValue == json);

                    if (veryVerbose) {
                        P(loggedMessagesFromJson);
                    }
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // ENCODING ARRAYS
        //
        // Concerns:
        // 1. Sequence objects can contain arrays of difference `bdlat` types
        //    that include:
        //
        //    * Enumerated values
        //    * Strings
        //    * boolean
        //    * other complex types
        //    * nullable values
        //
        // Plan:
        // 1. Construct a table that provies a test set of `bldat` values in
        //    XML representation.
        //
        // 2. For each entry in the table (P-1) create a `bdlb::Sequence3`
        //    object can convert it to a `Json` object.
        //
        // 3. Compare the converted `Json` object to one obtained from the
        //    oracle.
        //
        // Testing:
        //   CONCERN: ENCODING ARRAYS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ENCODING ARRAYS" << endl
                          << "===============" << endl;

        if (veryVerbose) cout << "Encode arrays in a sequence" << endl;
        {
            static const struct {
                int         d_line;              // source line number
                const char *d_xmlText_p;         // xml text
            } DATA[] = {
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "</element1>\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element1>LONDON</element1>\n"
                    "</element1>\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element1>LONDON</element1>\n"
                    "  <element1>NEW_JERSEY</element1>\n"
                    "</element1>\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element2>arbitrary string value</element2>\n"
                    "</element1>\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element6>NEW_YORK</element6>\n"
                    "</element1>\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element1>LONDON</element1>\n"
                    "  <element2>arbitrary string value</element2>\n"
                    "</element1>\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element1>LONDON</element1>\n"
                    "  <element2>arbitrary string value</element2>\n"
                    "  <element6>NEW_YORK</element6>\n"
                    "</element1>\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element1>LONDON</element1>\n"
                    "  <element2>arbitrary string value</element2>\n"
                    "  <element6>NEW_YORK</element6>\n"
                    "</element1>\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element1>NEW_JERSEY</element1>\n"
                    "  <element1>LONDON</element1>\n"
                    "  <element2>something random</element2>\n"
                    "  <element2>arbitrary string value</element2>\n"
                    "  <element6>NEW_YORK</element6>\n"
                    "  <element6>NEW_JERSEY</element6>\n"
                    "</element1>\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element1>NEW_JERSEY</element1>\n"
                    "  <element5>\n"
                    "    <element1>\n"
                    "      <element2>arbitrary string value</element2>\n"
                    "    </element1>\n"
                    "  </element5>\n"
                    "</element1>\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element5>\n"
                    "    <element1>\n"
                    "    </element1>\n"
                    "    <element5>123456</element5>\n"
                    "    <element5>7890</element5>\n"
                    "  </element5>\n"
                    "</element1>\n"
                },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const bsl::string XML    = DATA[ti].d_xmlText_p;

                if (veryVerbose) {
                    P_(LINE); P(XML);
                }

                balb::Sequence3 objectFromXml;
                ASSERT(0 == u::populateTestObject(&objectFromXml, XML));

                if (veryVerbose) {
                    P(objectFromXml);
                }

                Options options;

                options.setEncodeEmptyArrays(true);
                options.setEncodeNullElements(true);

                const Options& mO = options;

                Encoder            encoder;
                bsl::ostringstream oss;
                ASSERTV(0 == encoder.encode(oss, objectFromXml, mO));

                bdljsn::Json expected;
                int          rcExpected = bdljsn::JsonUtil::read(&expected,
                                                                 oss.str());
                ASSERTV(rcExpected, 0 == rcExpected);

                Obj  obj;
                Json actual;
                int  rcActual = obj.convert(&actual, objectFromXml);    // TEST
                ASSERTV(rcActual, 0 == rcActual);

                ASSERTV(expected,   actual,
                        expected == actual);

                if (veryVerbose) {
                    T_; P(actual);
                }

                balb::Sequence3 objectFromJson;
                int             rcFromJson = obj.convert(&objectFromJson,
                                                         actual);       // TEST
                ASSERTV(rcFromJson, 0 == rcFromJson);

                ASSERTV(objectFromXml,   objectFromJson,
                        objectFromXml == objectFromJson);
            }
        }
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
        // 1. A series of ad hoc tests.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        s_baltst::Employee bob;

        char jsonTextCompact[] =
            "{"
                "\"name\":\"Bob\","
                "\"homeAddress\":{"
                    "\"street\":\"Some Street\","
                    "\"city\":\"Some City\","
                    "\"state\":\"Some State\""
                "},"
                "\"age\":21"
            "}";

        char jsonTextPretty[] =
            "    {\n"
            "        \"name\" : \"Bob\",\n"
            "        \"homeAddress\" : {\n"
            "            \"street\" : \"Some Street\",\n"
            "            \"city\" : \"Some City\",\n"
            "            \"state\" : \"Some State\"\n"
            "        },\n"
            "        \"age\" : 21\n"
            "    }\n";

        bob.name()                 = "Bob";
        bob.homeAddress().street() = "Some Street";
        bob.homeAddress().city()   = "Some City";
        bob.homeAddress().state()  = "Some State";
        bob.age()                  = 21;

        ASSERTV("Bob"        == bob.name());
        ASSERT("Some Street" == bob.homeAddress().street());
        ASSERT("Some City"   == bob.homeAddress().city());
        ASSERT("Some State"  == bob.homeAddress().state());
        ASSERT(21            == bob.age());

        {
            bsl::istringstream iss(jsonTextCompact);

            Encoder            encoder;
            bsl::ostringstream oss;
            Options options;
            ASSERTV(0 == encoder.encode(oss, bob, options));
            ASSERTV(oss.str() == jsonTextCompact);
            if (verbose) {
                P(oss.str());
            }

            bdljsn::Json exp;
            int          rcExp = bdljsn::JsonUtil::read(&exp, oss.str());
            ASSERTV(rcExp, 0 == rcExp);

            Json json;
            Obj  converter;
            int  rcCon = converter.convert(&json, bob);
            ASSERTV(rcCon, 0 == rcCon);
            ASSERTV(exp,    json,
                    exp  == json);
        }

        Encoder encoder;
        Options options; const Options& mO = options;
        options.setEncodingStyle(Options::e_PRETTY);
        options.setInitialIndentLevel(1);
        options.setSpacesPerLevel(4);

        {
            bsl::istringstream iss(jsonTextPretty);
            bsl::ostringstream oss;

            ASSERTV(0 == encoder.encode(oss, bob, mO));
            ASSERTV(oss.str() == jsonTextPretty);
            if (veryVerbose) {
                P(oss.str()); P(bsl::string(jsonTextPretty));
            }
        }
        {
            bsl::istringstream iss(jsonTextPretty);
            bsl::ostringstream oss;

            ASSERTV(0 == encoder.encode(oss, bob, &mO));
            ASSERTV(oss.str() == jsonTextPretty);
            if (veryVerbose) {
                P(oss.str()); P(bsl::string(jsonTextPretty));
            }

            bdljsn::Json exp;
            int          rcExp = bdljsn::JsonUtil::read(&exp, oss.str());
            ASSERTV(rcExp, 0 == rcExp);

            Json json;
            Obj  converter;
            int  rcCon = converter.convert(&json, bob);
            ASSERTV(rcCon, 0 == rcCon);
            ASSERTV(exp,    json,
                    exp  == json);
        }

        if (veryVerbose) {
            cout << endl << "Empty Sequence Example" << endl;
        }
        {
            u::EmptySequenceExample sequenceExample;
            sequenceExample.simpleValue() = 1;

            bsl::stringstream ss;
            encoder.encode(ss, sequenceExample);

            ASSERTV(ss.str(), "{\"simpleValue\":1}" == ss.str());

            bdljsn::Json exp;
            int          rcExp = bdljsn::JsonUtil::read(&exp, ss.str());
            ASSERTV(rcExp, 0 == rcExp);

            Json json;
            Obj  converter;
            int  rcCon = converter.convert(&json, sequenceExample);
            ASSERTV(rcCon, 0 == rcCon);
            ASSERTV(exp,    json,
                    exp  == json);
        }

        if (veryVerbose) {
            cout << "A Sequence of two simple values." << endl;
        }
        {
            s_baltst::MySequence ms;
            ms.attribute1() = 666;
            ms.attribute2() = "Kilroy was here.";

            P(ms);

            Obj  converter;
            Json json;
            int  rcToJson = converter.convert(&json, ms);
            ASSERTV(rcToJson, 0 == rcToJson);
            ASSERTV(      converter.loggedMessages(),
                    "" == converter.loggedMessages());

            P(json);

            s_baltst::MySequence msFromJson;
            int                  rcFromJson = converter.convert(&msFromJson,
                                                                json);
            ASSERTV(rcFromJson, 0 == rcFromJson);
            ASSERTV(      converter.loggedMessages(),
                    "" == converter.loggedMessages());

            P(msFromJson);

            ASSERTV(ms,   msFromJson,
                    ms == msFromJson);
        }

        if (veryVerbose) {
            cout << "Sequence having a member holding a sequence." << endl;
        }
        {
            s_baltst::Employee employee;
            employee.name()                 = "Bob";
            employee.homeAddress().street() = "Lexington Ave";
            employee.homeAddress().city()   = "New York City";
            employee.homeAddress().state()  = "New York";
            employee.age()                  = 21;

            if (veryVeryVerbose) {
                P(employee)
            }

            Obj  converter;
            Json json;
            int  rcToJson = converter.convert(&json, employee);
            ASSERTV(rcToJson, 0 == rcToJson);
            ASSERTV(      converter.loggedMessages(),
                    "" == converter.loggedMessages());

            if (veryVeryVerbose) {
                P(json);
            }

            s_baltst::Employee employeeFromJson;
            int                rcFromJson = converter.convert(
                                                             &employeeFromJson,
                                                             json);
            ASSERTV(rcFromJson, 0 == rcFromJson);
            ASSERTV(      converter.loggedMessages(),
                    "" == converter.loggedMessages());

            if (veryVeryVerbose) {
                P(employeeFromJson);
            }

            ASSERTV(employee,   employeeFromJson,
                    employee == employeeFromJson);
        }

        if (veryVerbose) {
            cout << "Sequence having an member holding an array." << endl;
        }
        {
            s_baltst::MySequenceWithArray mswa;
            mswa.attribute1() = 3;
            mswa.attribute2().push_back("Moe");
            mswa.attribute2().push_back("Larry");
            mswa.attribute2().push_back("Curly");
            mswa.attribute2().push_back("Shemp");

            bdljsn::Json json; json.makeObject();

            json["attribute1"] = 3;
            json["attribute2"].makeArray();
            json["attribute2"].theArray().pushBack(Json("Moe"  ));
            json["attribute2"].theArray().pushBack(Json("Larry"));
            json["attribute2"].theArray().pushBack(Json("Curly"));
            json["attribute2"].theArray().pushBack(Json("Shemp"));

            if (veryVeryVerbose) {
                P(json);
            }

            Obj                           converter;
            s_baltst::MySequenceWithArray mswaFromJson;
            int                           rcFromJson = converter.convert(
                                                                 &mswaFromJson,
                                                                 json);
            ASSERTV(rcFromJson, 0 == rcFromJson);
            ASSERTV(      converter.loggedMessages(),
                    "" == converter.loggedMessages());

            if (veryVeryVerbose) {
                P(mswaFromJson);
            }

            ASSERTV(mswa,   mswaFromJson,
                    mswa == mswaFromJson);
        }

        if (veryVerbose) {
            cout << "Sequence having a member holding a `DatetimeTz` value."
                 << endl;
        }
        {
            typedef s_baltst::BasicRecord BR;
            BR mV1; const BR& V1 = mV1;
            mV1.i1() = 11;
            mV1.i2() = 12;
            mV1.dt() = bdlt::DatetimeTz();
            mV1. s() = "I am 'BasicRecord' value1.";

            if (veryVerbose) {
                P(V1);
            }

            bdljsn::Json json; json.makeObject();
            json["i1"] = 11;
            json["i2"] = 12;
            json["dt"] = "0001-01-01T24:00:00.000+00:00";
            json["s" ] = "I am 'BasicRecord' value1.";

            if (veryVerbose) {
                P(json);
            }

            Obj converter;
            BR  brFromJson;
            int rcFromJson = converter.convert(&brFromJson, json);
            ASSERTV(rcFromJson, 0 == rcFromJson);
            ASSERTV(      converter.loggedMessages(),
                    "" == converter.loggedMessages());

            if (veryVerbose) {
                P(brFromJson);
            }

            ASSERTV(V1,   brFromJson,
                    V1 == brFromJson);
        }

        if (veryVerbose) {
            cout << "Sequence having a member holding a floating point value."
                 << endl;
        }
        {
            s_baltst::Sqrt sqrt;
            sqrt.value() = 1.414;

            if (veryVerbose) {
                P(sqrt);
            }

            bdljsn::Json json; json.makeObject();
            json["value"] = 1.414;

            if (veryVerbose) {
                P(json);
            }

            s_baltst::Sqrt sqrtFromJson;
            Obj            converter;
            int            rcFromJson = converter.convert(&sqrtFromJson, json);
            ASSERTV(rcFromJson, 0 == rcFromJson);
            ASSERTV(      converter.loggedMessages(),
                    "" == converter.loggedMessages());

            if (veryVerbose) {
                P(sqrtFromJson);
            }

            ASSERTV(sqrt,   sqrtFromJson,
                    sqrt == sqrtFromJson);
        }

        if (veryVerbose) {

            cout << "Sequence having a member holding \"+inf\"." << endl;
        }
        {
            s_baltst::Sqrt sqrt;
            sqrt.value() = bsl::numeric_limits<double>::infinity();

            bdljsn::Json json; json.makeObject();
            json["value"] = "+inf";

            if (veryVerbose) {
                P(sqrt);
                P(json);
            }

            s_baltst::Sqrt sqrtFromJson;
            Obj            converter;
            int            rcFromJson = converter.convert(&sqrtFromJson, json);
            ASSERTV(rcFromJson, 0 == rcFromJson);
            ASSERTV(      converter.loggedMessages(),
                    "" == converter.loggedMessages());

            ASSERTV(sqrt,   sqrtFromJson,
                    sqrt == sqrtFromJson);
        }

        if (veryVerbose) {
            cout
             << "Sequences having a member holding INF/-INF/NaN (round-trip)."
             << endl;
        }
        {
                double DATA[] = {
                    bsl::numeric_limits<double>::infinity()
                ,  -bsl::numeric_limits<double>::infinity()
                ,   bsl::numeric_limits<double>::quiet_NaN()
                };

                Obj     converter;
                Encoder encoder;

                bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

                for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                    const double VALUE = DATA[i];

                    if (veryVeryVerbose) {
                        P(VALUE);
                    }

                    s_baltst::Sqrt sqrt;
                    sqrt.value() = VALUE;

                    if (veryVeryVerbose) {
                        P(sqrt);
                    }

                    Json json;
                    int rc = converter.convert(&json, sqrt);

                    ASSERTV(rc, 0 == rc);
                    ASSERTV(      converter.loggedMessages(),
                            "" == converter.loggedMessages());

                    if (veryVeryVerbose) {
                        P(json);
                    }

                    s_baltst::Sqrt sqrtFromJson;
                    int rcFromJson = converter.convert(&sqrtFromJson, json);

                    ASSERTV(rcFromJson, 0 == rcFromJson);
                    ASSERTV(      converter.loggedMessages(),
                            "" == converter.loggedMessages());

                    const double VALUE_INIT  = sqrt.value();
                    const double VALUE_FINAL = sqrtFromJson.value();
                    if (VALUE_FINAL == VALUE_FINAL) {  // not NaN
                        ASSERTV(sqrt,   sqrtFromJson,
                                sqrt == sqrtFromJson);
                    } else {                           //  is NaN
                        ASSERTV(VALUE_INIT, VALUE_INIT != VALUE_INIT);
                                                       // also NaN
                    }

                    if (veryVeryVerbose) {
                        P(sqrtFromJson);
                    }
                }
        }

        if (veryVerbose) {
            cout << "Choice holding an enumerated value." << endl;
        }
        {
            s_baltst::Topchoice tc; tc.makeSelection7(
                                                 s_baltst::Enumerated::LONDON);

            if (veryVerbose) {
                P(tc);
            }

            bdljsn::Json json; json.makeObject();
            json["selection7"] = "LONDON";

            if (veryVerbose) {
                P(json);
            }

            s_baltst::Topchoice tcFromJson;
            Obj                 converter;
            int                 rcFromJson = converter.convert(&tcFromJson,
                                                               json);
            ASSERTV(rcFromJson, 0 == rcFromJson);
            ASSERTV(      converter.loggedMessages(),
                    "" == converter.loggedMessages());

            ASSERTV(tc,   tcFromJson,
                    tc == tcFromJson);
        }

        if (veryVerbose) {
            cout << "Sequence having a member holding a nullable value."
                 << endl;
        }
        {
            s_baltst::MySequenceWithNullable mswn;
            mswn.attribute1() = 1732;

            if (veryVeryVerbose) {
                P(mswn);
            }

            bdljsn::Json json; json.makeObject();
            json["attribute1"] = 1732;
            json["attribute2"] = bdljsn::JsonNull();

            if (veryVerbose) {
                P(json);
            }

            Obj                              converter;
            s_baltst::MySequenceWithNullable mswnFromJson;
            int                              rcFromJson = converter.convert(
                                                                 &mswnFromJson,
                                                                 json);
            ASSERTV(rcFromJson, 0 == rcFromJson);
            ASSERTV(      converter.loggedMessages(),
                    "" == converter.loggedMessages());

            ASSERTV(mswn,   mswnFromJson,
                    mswn == mswnFromJson);
        }

        if (veryVerbose) {
            cout << "Sequence having two members holding nullable values."
                 << endl;
        }
        {

            if (veryVerbose) {
                Q(Type having two nullable attributes.)
            }

            s_baltst::MySimpleContent msc;
            ASSERT("" == msc.theContent());
            ASSERT(msc.attribute1().isNull());
            ASSERT(msc.attribute2().isNull());

            if (veryVerbose) {
                P(msc);
            }

            Obj  converter;
            Json jsonFromBdlat;
            int  rcToJson = converter.convert(&jsonFromBdlat, msc);
            ASSERTV(rcToJson, 0 == rcToJson);
            ASSERTV(      converter.loggedMessages(),
                    "" == converter.loggedMessages());

            if (veryVerbose) {
                P(jsonFromBdlat);
            }

            s_baltst::MySimpleContent mscFromJson;
            int                       rcFromJson = converter.convert(
                                                                &mscFromJson,
                                                                jsonFromBdlat);
            ASSERTV(rcFromJson, 0 == rcFromJson);
            ASSERTV(      converter.loggedMessages(),
                    "" == converter.loggedMessages());

            ASSERTV(msc,   mscFromJson,
                    msc == mscFromJson);

            if (veryVerbose) {
                Q(Let us try non-null values.)
            }

            msc.theContent() = "non-optional-string";
            msc.attribute1() = true;           // nullable
            msc.attribute2() = "hello";        // nullable

            if (veryVerbose) {
                P(msc);
            }

            Json json; json.makeObject();
            json["TheContent"] = "non-optional-string";
            json["attribute1"] = bdljsn::Json().makeBoolean(true);
            json["attribute2"] = "hello";

            if (veryVerbose) {
                P(json);
            }

            rcFromJson = converter.convert(&mscFromJson, json);

            ASSERTV(rcFromJson, 0 == rcFromJson);
            ASSERTV(      converter.loggedMessages(),
                    "" == converter.loggedMessages());

            ASSERTV(msc,   mscFromJson,
                    msc == mscFromJson);

            if (veryVerbose) {
                P(mscFromJson);
            }
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ============================================================================
//                          TEST ENTITY DEFINITIONS
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace {
namespace u {

                             // ------------------
                             // class Enumeration0
                             // ------------------

// CREATORS
Enumeration0::Enumeration0()
{
}

// TRAITS
void bdlat_enumToString(bsl::string *result, const Enumeration0&)
{
    *result = "zero";
}

                     // ----------------------------------
                     // class EmptySequenceExampleSequence
                     // ----------------------------------

// CLASS METHODS
inline
int EmptySequenceExampleSequence::maxSupportedBdexVersion()
{
    return 1;
}

BSLA_MAYBE_UNUSED
const bdlat_AttributeInfo *EmptySequenceExampleSequence::lookupAttributeInfo(
                                                        const char *name,
                                                        int         nameLength)
{
#ifndef BSLA_MAYBE_UNUSED_IS_ACTIVE_
    typedef const bdlat_AttributeInfo* (*StaticFunPtr)(const char *, int);
    (void)(StaticFunPtr)(&EmptySequenceExampleSequence::lookupAttributeInfo);
#endif

    (void)name;
    (void)nameLength;
    return 0;
}

BSLA_MAYBE_UNUSED
const bdlat_AttributeInfo *EmptySequenceExampleSequence::lookupAttributeInfo(
                                                                        int id)
{
#ifndef BSLA_MAYBE_UNUSED_IS_ACTIVE_
    typedef const bdlat_AttributeInfo* (*StaticFunPtr)(int);
    (void)(StaticFunPtr)(&EmptySequenceExampleSequence::lookupAttributeInfo);
#endif

    switch (id) {
      default:
        return 0;
    }
}

// CREATORS
EmptySequenceExampleSequence::EmptySequenceExampleSequence()
{
}

// MANIPULATORS
template <class STREAM>
STREAM& EmptySequenceExampleSequence::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int EmptySequenceExampleSequence::manipulateAttribute(MANIPULATOR& manipulator,
                                                      int          id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };
    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EmptySequenceExampleSequence::manipulateAttribute(
                                                     MANIPULATOR&  manipulator,
                                                     const char   *name,
                                                     int           nameLength)
{
    enum { NOT_FOUND = -1 };
    const bdlat_AttributeInfo *attributeInfo = lookupAttributeInfo(name       ,
                                                                   nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;
    }
    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <class MANIPULATOR>
int EmptySequenceExampleSequence::manipulateAttributes(
                                                      MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;
    return ret;
}

void EmptySequenceExampleSequence::reset()
{
}

// ACCESSORS
template <class ACCESSOR>
int EmptySequenceExampleSequence::accessAttribute(ACCESSOR& accessor,
                                                  int       id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };
    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EmptySequenceExampleSequence::accessAttribute(ACCESSOR&   accessor,
                                                  const char *name,
                                                  int         nameLength) const
{
    enum { NOT_FOUND = -1 };
    const bdlat_AttributeInfo *attributeInfo = lookupAttributeInfo(name       ,
                                                                   nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;
    }
    return accessAttribute(accessor, attributeInfo->d_id);
}

template <class ACCESSOR>
int EmptySequenceExampleSequence::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;
    return ret;
}

template <class STREAM>
STREAM& EmptySequenceExampleSequence::bdexStreamOut(STREAM& stream,
                                                    int     version) const
{
    switch (version) {
      case 1: {
      } break;
    }
    return stream;
}

bsl::ostream& EmptySequenceExampleSequence::print(
                                            bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}

                         // --------------------------
                         // class EmptySequenceExample
                         // --------------------------

// CLASS DATA
const bdlat_AttributeInfo EmptySequenceExample::ATTRIBUTE_INFO_ARRAY[] = {
    {ATTRIBUTE_ID_SIMPLE_VALUE,
     "simpleValue",
     sizeof("simpleValue") - 1,
     "",
     bdlat_FormattingMode::e_DEC},
    {ATTRIBUTE_ID_SEQUENCE,
     "Sequence",
     sizeof("Sequence") - 1,
     "",
     bdlat_FormattingMode::e_DEFAULT | bdlat_FormattingMode::e_UNTAGGED}};

// CLASS METHODS
inline
int EmptySequenceExample::maxSupportedBdexVersion()
{
    return 1;
}

BSLA_MAYBE_UNUSED
const bdlat_AttributeInfo *EmptySequenceExample::lookupAttributeInfo(int id)
{
#ifndef BSLA_MAYBE_UNUSED_IS_ACTIVE_
    typedef const bdlat_AttributeInfo* (*StaticFunPtr)(int);
    (void)(StaticFunPtr)(&EmptySequenceExample::lookupAttributeInfo);
#endif

    switch (id) {
      case ATTRIBUTE_ID_SIMPLE_VALUE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SIMPLE_VALUE];
      case ATTRIBUTE_ID_SEQUENCE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE];
      default:
        return 0;                                                     // RETURN
    }
}

BSLA_MAYBE_UNUSED
const bdlat_AttributeInfo *EmptySequenceExample::lookupAttributeInfo(
                                                        const char *name,
                                                        int         nameLength)
{
#ifndef BSLA_MAYBE_UNUSED_IS_ACTIVE_
    typedef const bdlat_AttributeInfo* (*StaticFunPtr)(const char *, int);
    (void)(StaticFunPtr)(&EmptySequenceExample::lookupAttributeInfo);
#endif

    for (int i = 0; i < 2; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
            EmptySequenceExample::ATTRIBUTE_INFO_ARRAY[i];
        if (nameLength == attributeInfo.d_nameLength &&
            0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength)) {
            return &attributeInfo;
        }
    }
    return 0;
}

// CREATORS
EmptySequenceExample::EmptySequenceExample()
: d_simpleValue()
, d_sequence()
{
}

// MANIPULATORS
template <class STREAM>
STREAM& EmptySequenceExample::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bslx::InStreamFunctions::bdexStreamIn(stream, d_simpleValue, 1);
            bslx::InStreamFunctions::bdexStreamIn(stream, d_sequence, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int EmptySequenceExample::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };
    switch (id) {
      case ATTRIBUTE_ID_SIMPLE_VALUE: {
        return manipulator(&d_simpleValue,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SIMPLE_VALUE]);
      } break;
      case ATTRIBUTE_ID_SEQUENCE: {
        return manipulator(&d_sequence,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EmptySequenceExample::manipulateAttribute(MANIPULATOR&  manipulator,
                                              const char   *name,
                                              int           nameLength)
{
    enum { NOT_FOUND = -1 };
    const bdlat_AttributeInfo *attributeInfo = lookupAttributeInfo(name       ,
                                                                   nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <class MANIPULATOR>
int EmptySequenceExample::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;
    ret = manipulator(&d_simpleValue,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SIMPLE_VALUE]);
    if (ret) {
        return ret;
    }
    ret = manipulator(&d_sequence,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }
    return ret;
}

BSLA_MAYBE_UNUSED
void EmptySequenceExample::reset()
{
#ifndef BSLA_MAYBE_UNUSED_IS_ACTIVE_
    (void)&EmptySequenceExample::reset;
#endif

    bdlat_ValueTypeFunctions::reset(&d_simpleValue);
    bdlat_ValueTypeFunctions::reset(&d_sequence);
}

inline
EmptySequenceExampleSequence& EmptySequenceExample::sequence()
{
    return d_sequence;
}

inline
int& EmptySequenceExample::simpleValue()
{
    return d_simpleValue;
}

// ACCESSORS
template <class ACCESSOR>
int EmptySequenceExample::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };
    switch (id) {
      case ATTRIBUTE_ID_SIMPLE_VALUE: {
        return accessor(d_simpleValue,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SIMPLE_VALUE]);
      } break;
      case ATTRIBUTE_ID_SEQUENCE: {
        return accessor(d_sequence,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EmptySequenceExample::accessAttribute(ACCESSOR&   accessor,
                                          const char *name,
                                          int         nameLength) const
{
    enum { NOT_FOUND = -1 };
    const bdlat_AttributeInfo *attributeInfo = lookupAttributeInfo(name       ,
                                                                   nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;
    }
    return accessAttribute(accessor, attributeInfo->d_id);
}

template <class ACCESSOR>
int EmptySequenceExample::accessAttributes(ACCESSOR& accessor) const
{
    int ret;
    ret = accessor(d_simpleValue                                      ,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SIMPLE_VALUE]);
    if (ret) {
        return ret;
    }
    ret = accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }
    return ret;
}

template <class STREAM>
STREAM& EmptySequenceExample::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bslx::OutStreamFunctions::bdexStreamOut(stream, d_simpleValue, 1);
        bslx::OutStreamFunctions::bdexStreamOut(stream, d_sequence, 1);
      } break;
    }
    return stream;
}

BSLA_MAYBE_UNUSED
bsl::ostream& EmptySequenceExample::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
#ifndef BSLA_MAYBE_UNUSED_IS_ACTIVE_
    (void)&EmptySequenceExample::print;
#endif

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("simpleValue", d_simpleValue);
    printer.printAttribute("sequence", d_sequence);
    printer.end();
    return stream;
}

inline
int EmptySequenceExample::simpleValue() const
{
    return d_simpleValue;
}

inline
const EmptySequenceExampleSequence& EmptySequenceExample::sequence() const
{
    return d_sequence;
}

}  // close u namespace
}  // close unnamed namespace
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
