// baljsn_encoder.t.cpp                                               -*-C++-*-
#include <baljsn_encoder.h>
#include <baljsn_encoder_testtypes.h>

#include <balb_testmessages.h>

#include <bslim_testutil.h>

#include <bdlat_attributeinfo.h>
#include <bdlat_choicefunctions.h>
#include <bdlat_enumeratorinfo.h>
#include <bdlat_selectioninfo.h>
#include <bdlat_sequencefunctions.h>
#include <bdlat_typetraits.h>
#include <bdlat_valuetypefunctions.h>

#include <bdlb_printmethods.h>  // for printing vector
#include <bdlb_chartype.h>

#include <bdlde_utf8util.h>

#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_fixedmemoutstreambuf.h>
#include <bdlsb_memoutstreambuf.h>

// These headers are for testing only and the hierarchy level of 'baljsn' was
// increased because of them.  They should be removed when possible.
#include <balxml_decoder.h>
#include <balxml_decoderoptions.h>
#include <balxml_encoder.h>
#include <balxml_encoderoptions.h>
#include <balxml_minireader.h>
#include <balxml_errorinfo.h>

#include <bdlb_nullableallocatedvalue.h>
#include <bdlb_nullablevalue.h>
#include <bdlb_variant.h>

#include <bdlpcre_regex.h>

#include <bdlt_date.h>
#include <bdlt_datetz.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bsla_maybeunused.h>

#include <bslim_printer.h>

#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bsl_climits.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
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
#include <s_baltst_mysequencewithchoice.h>
#include <s_baltst_simplerequest.h>
#include <s_baltst_mysequencewithnullableanonymouschoice.h>
#include <s_baltst_generatetestarray.h>
#include <s_baltst_generatetestchoice.h>
#include <s_baltst_generatetestcustomizedtype.h>
#include <s_baltst_generatetestdynamictype.h>
#include <s_baltst_generatetestenumeration.h>
#include <s_baltst_generatetestnullablevalue.h>
#include <s_baltst_generatetestsequence.h>
#include <s_baltst_generatetesttaggedvalue.h>
#include <s_baltst_mysequencewithchoice.h>
#include <s_baltst_mysequencewithnullableanonymouschoice.h>
#include <s_baltst_testchoice.h>
#include <s_baltst_testcustomizedtype.h>
#include <s_baltst_testdynamictype.h>
#include <s_baltst_testenumeration.h>
#include <s_baltst_testnilvalue.h>
#include <s_baltst_testplaceholder.h>
#include <s_baltst_testselection.h>
#include <s_baltst_testsequence.h>
#include <s_baltst_testtaggedvalue.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements an encoder for outputting
// 'bdeat'-compatible objects in the JSON format.  The object types that can be
// encoded include 'bdeat' sequence, choice, array, enumeration, customized,
// simple, and dynamic types.  In addition, the encoder supports options to
// allow compact and pretty formatting.
//
// We use standard table-based approach to testing where we put both input and
// expected output in the same table row and verify that the actual result
// matches the expected value.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] baljsn::Encoder(bslma::Allocator *basicAllocator = 0);
// [ 2] ~baljsn::Encoder();
//
// MANIPULATORS
// [13] int encode(bsl::streambuf *streamBuf, const TYPE& v, options);
// [13] int encode(bsl::ostream& stream, const TYPE& v, options);
// [13] int encode(bsl::streambuf *streamBuf, const TYPE& v, &options);
// [13] int encode(bsl::ostream& stream, const TYPE& v, &options);
//
// ACCESSORS
// [13] bsl::string loggedMessages() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [20] ENCODING UNSET CHOICE
// [21] ENCODING NULL CHOICE
// [22] ENCODING VECTORS OF VECTORS
// [23] USAGE EXAMPLE

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
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_IBM)
#define U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
#endif

typedef baljsn::Encoder                       Obj;
typedef baljsn::Encoder_EncodeImplUtil        ImplUtil;
typedef baljsn::EncoderOptions                Options;
typedef baljsn::EncoderOptions::EncodingStyle Style;
typedef bsls::Types::Int64                    Int64;
typedef bsls::Types::Uint64                   Uint64;

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

void printStringDifferences(const bsl::string& lhs, const bsl::string& rhs)
    // Print to stdout the length and the characters that are difference in the
    // specified 'lhs' and 'rhs' strings.
{
    const bsl::size_t len1 = lhs.size();
    const bsl::size_t len2 = rhs.size();
    P(len1) P(len2)
    for (bsl::size_t i = 0; i < len1; ++i) {
        if (lhs[i] != rhs[i]) {
            P(i)
            cout << '\'' << lhs[i] << '\'' << endl;
            cout << '\'' << rhs[i] << '\'' << endl;
            P(bsl::string(lhs.begin(), lhs.begin() + i));
            P(bsl::string(rhs.begin(), rhs.begin() + i));
            return;                                                   // RETURN
        }
    }
}

void constructFeatureTestMessage(
                            bsl::vector<s_baltst::FeatureTestMessage> *objects)
    // Decode the sequence of 's_baltst::FeatureTestMessage' objects defined
    // by 's_baltst::FeatureTestMessage::s_XML_MESSAGES' as if by using
    // 'balxml::Decoder' and load the sequence to the specified 'objects'.
{
    balxml::MiniReader reader;
    balxml::DecoderOptions options;
    balxml::ErrorInfo e;
    balxml::Decoder decoder(&options, &reader, &e);

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

template <class TYPE>
int populateTestObject(TYPE *object, const bsl::string& xmlString)
    // Decode the specified 'xmlString' to an object of the specified 'TYPE'
    // as if by using 'balxml::Decoder' and load the object to the specified
    // 'object'.  Return 0 on success, and a non-zero value otherwise.
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

template <class TYPE>
void testNumber()
    // Assert that 'baljsn::Encoder' can encode a diverse set of values of
    // the specified integral 'TYPE' to a JSON number literal, and that the
    // encoding loses no precision of the original value.
{
    // Test non-negative numbers.

    const struct {
        int         d_line;
        Uint64      d_value;
        const char *d_result;
    } DATA_1[] = {
        //LINE   VALUE            RESULT
        //---- ---------  -----------------------
        { L_,          0,                    "0" },
        { L_,          1,                    "1" },
        { L_,  UCHAR_MAX,                  "255" },
        { L_,   SHRT_MAX,                "32767" },
        { L_,  USHRT_MAX,                "65535" },
        { L_,    INT_MAX,           "2147483647" },
        { L_,   UINT_MAX,           "4294967295" },
#if   defined(BSLS_PLATFORM_CPU_32_BIT)                                       \
  || (defined(BSLS_PLATFORM_CPU_64_BIT) && defined(BSLS_PLATFORM_OS_WINDOWS))
        { L_,   LONG_MAX,           "2147483647" },
        { L_,  ULONG_MAX,           "4294967295" },
#elif defined(BSLS_PLATFORM_CPU_64_BIT)
        { L_,   LONG_MAX,  "9223372036854775807" },
        { L_,  ULONG_MAX, "18446744073709551615" },
#else
# error "baljsn_encoder.t.cpp does not support the platform's bitness."
#endif
        { L_,  LLONG_MAX,  "9223372036854775807" },
        { L_, ULLONG_MAX, "18446744073709551615" }
    };

    const int NUM_DATA_1 = sizeof DATA_1 / sizeof *DATA_1;

    for (int ti = 0; ti < NUM_DATA_1; ++ti) {
        const int         LINE  = DATA_1[ti].d_line;
        const Uint64      VALUE = DATA_1[ti].d_value;
        const char *const EXP   = DATA_1[ti].d_result;

        if (VALUE > static_cast<Uint64>(bsl::numeric_limits<TYPE>::max())) {
            continue;                                               // CONTINUE
        }

        const TYPE VALUE_AS_TYPE = static_cast<TYPE>(VALUE);

        Obj                encoder;
        bsl::ostringstream oss;
        ASSERTV(LINE, 0 == ImplUtil::encode(&oss, VALUE_AS_TYPE));

        bsl::string result = oss.str();
        ASSERTV(LINE, result, EXP, result == EXP);
    }

    // Test negative numbers.

    const struct {
        int         d_line;
        Int64       d_value;
        const char *d_result;
    } DATA_2[] = {
        //LINE   VALUE            RESULT
        //---- ---------  -----------------------
        { L_,         -1,                   "-1" },
        { L_,  SCHAR_MIN,                 "-128" },
        { L_,   SHRT_MIN,               "-32768" },
        { L_,    INT_MIN,          "-2147483648" },
#if   defined(BSLS_PLATFORM_CPU_32_BIT)                                       \
  || (defined(BSLS_PLATFORM_CPU_64_BIT) && defined(BSLS_PLATFORM_OS_WINDOWS))
        { L_,   LONG_MIN,          "-2147483648" },
#elif defined(BSLS_PLATFORM_CPU_64_BIT)
        { L_,   LONG_MIN, "-9223372036854775808" },
#else
# error "baljsn_encoder.t.cpp does not support the platform's bitness."
#endif
        { L_,  LLONG_MIN, "-9223372036854775808" }
    };

    const int NUM_DATA_2 = sizeof DATA_2 / sizeof *DATA_2;

    for (int ti = 0; ti < NUM_DATA_2; ++ti) {
        const int         LINE  = DATA_2[ti].d_line;
        const Int64       VALUE = DATA_2[ti].d_value;
        const char *const EXP   = DATA_2[ti].d_result;

        if (VALUE < static_cast<Int64>(bsl::numeric_limits<TYPE>::min())) {
            continue;                                               // CONTINUE
        }

        const TYPE VALUE_AS_TYPE = static_cast<TYPE>(VALUE);

        Obj                encoder;
        bsl::ostringstream oss;
        ASSERTV(LINE, 0 == ImplUtil::encode(&oss, VALUE_AS_TYPE));

        bsl::string result = oss.str();
        ASSERTV(LINE, result, EXP, result == EXP);
    }
}

                             // ==================
                             // struct TestMessage
                             // ==================

struct TestMessage {

    // DATA
    int         d_line;
    const char *d_text_p;
};


struct EncodeNullElements {
    enum Enum {
        e_NO,
        e_YES
    };
};

struct EncodeEmptyArrays {
    enum Enum {
        e_NO,
        e_YES
    };
};

                               // ===============
                               // struct TestUtil
                               // ===============

struct TestUtil {
    // CLASS METHODS
    template <class VALUE_TYPE>
    static void assertEncodedValueIsEqual(
                             int                          LINE,
                             baljsn::EncodingStyle::Value ENCODING_STYLE,
                             EncodeNullElements::Enum     ENCODE_NULL_ELEMENTS,
                             EncodeEmptyArrays::Enum      ENCODE_EMPTY_ARRAYS,
                             const VALUE_TYPE&            VALUE,
                             const bsl::string_view&      EXPECTED_JSON_STRING)
        // Assert that encoding the specified 'VALUE' using 'baljsn::Encoder'
        // with the specified 'ENCODING_STYLE', 'ENCODE_NULL_ELEMENTS', and
        // 'ENCODE_EMPTY_ARRAYS' options yields the specified
        // 'EXPECTED_JSON_STRING'.
    {
        bdlsb::MemOutStreamBuf outStreamBuf;
        bsl::ostream           outStream(&outStreamBuf);

        baljsn::EncoderOptions options;
        options.setEncodingStyle(ENCODING_STYLE);
        options.setEncodeNullElements(EncodeNullElements::e_YES ==
                                      ENCODE_NULL_ELEMENTS);
        options.setEncodeEmptyArrays(EncodeEmptyArrays::e_YES ==
                                     ENCODE_EMPTY_ARRAYS);
        options.setInitialIndentLevel(0);
        options.setSpacesPerLevel(4);

        baljsn::Encoder encoder;
        int             rc = encoder.encode(&outStreamBuf, VALUE, &options);
        LOOP1_ASSERT_EQ(LINE, 0, rc);
        if (0 != rc) {
            P_(encoder.loggedMessages());
        }

        const bsl::string_view jsonStringRef(outStreamBuf.data(),
                                             outStreamBuf.length());
        LOOP1_ASSERT_EQ(LINE, EXPECTED_JSON_STRING, jsonStringRef);
    }
};

                               // =============
                               // class Replace
                               // =============

class Replace {
  public:
    // CREATORS
    Replace() { }

    // ACCESSORS
    bsl::string operator()(const bsl::string_view& original,
                           const bsl::string_view& subject,
                           const bsl::string_view& replacement) const
        // Return the result of replacing each occurrence of the specified
        // 'subject' with the specified 'replacement' in the specified
        // 'original' string.
    {
        bdlpcre::RegEx regex;
        bsl::string    prepareErrorMessage;
        bsl::size_t    prepareErrorOffset = 0;
        bsl::string    subjectCopy(subject);
        int rc = regex.prepare(
            &prepareErrorMessage, &prepareErrorOffset, subjectCopy.c_str());
        if (0 != rc) {
            P(prepareErrorMessage);
        }
        BSLS_ASSERT(0 == rc);
        BSLS_ASSERT(0 == prepareErrorOffset);


        bsl::string result;
        int         errorOffset = 0;

        rc = regex.replace(&result,
                           &errorOffset,
                           original,
                           replacement,
                           bdlpcre::RegEx::k_REPLACE_LITERAL |
                               bdlpcre::RegEx::k_REPLACE_GLOBAL);
        if (0 > rc) {
            P(prepareErrorMessage);
        }
        BSLS_ASSERT(0 <= rc);
        BSLS_ASSERT(0 == errorOffset);
        return result;
    }
};

                // ==============================================
                // class AssertEncodingOverflowIsDetectedFunction
                // ==============================================

class AssertEncodingOverflowIsDetectedFunction {
  private:
    // PRIVATE CLASS METHODS
    static void assertExpectations(
                         bdlsb::FixedMemOutStreamBuf *streamBuf,
                         const int                    rc,
                         const baljsn::Encoder&       encoder,
                         const int                    LINE,
                         const int                    SUCCESS,
                         const bsl::string_view&      EXPECTED_JSON_STRING,
                         const bsl::string_view&      EXPECTED_LOGGED_MESSAGES)
        // Assert that the content of the specified 'streamBuf' is equal to the
        // specified 'EXPECTED_JSON_STRING', that the specified 'rc' is
        // 0 if the specified 'SUCCESS' is true, that 'rc' is non-zero if
        // 'SUCCESS' is false, and that the 'loggedMessages' of the specified
        // 'encoder' is equal to the specified 'EXPECTED_LOGGED_MESSAGES'.
    {
        if (SUCCESS) {
            LOOP1_ASSERT_EQ(LINE, 0, rc);
        }
        else {
            LOOP1_ASSERT_NE(LINE, 0, rc);
        }

        const bsl::streampos streamBufPosition =
            streamBuf->pubseekoff(0, bsl::ios_base::cur);

        const bsl::string_view jsonString(streamBuf->data(),
                                          streamBufPosition);

        LOOP1_ASSERT_EQ(LINE, EXPECTED_JSON_STRING, jsonString);

        LOOP1_ASSERT_EQ(
            LINE, EXPECTED_LOGGED_MESSAGES, encoder.loggedMessages());
    }

  public:
    // CREATORS
    AssertEncodingOverflowIsDetectedFunction() {}

    // ACCESSORS
    template <class VALUE_TYPE>
    void operator()(const int               LINE,
                    const VALUE_TYPE&       VALUE,
                    const int               BUFFER_SIZE,
                    const bool              SUCCESS,
                    const bsl::string_view& EXPECTED_JSON_STRING,
                    const bsl::string_view& EXPECTED_LOGGED_MESSAGES) const
        // Assert that encoding the specified 'VALUE' using 'baljsn::Encoder'
        // and a stream buffer that fails after its output sequence exceeds the
        // specified 'BUFFER_SIZE' number of bytes succeeds if the specified
        // 'SUCCESS' flag is 'true', and fails otherwise.  Then, in any case,
        // assert that the content of the stream buffer is equal to the
        // specified 'EXPECTED_JSON_STRING', and the content of the
        // 'loggedMessages' of the 'baljsn::Encoder' is equal to the specified
        // 'EXPECTED_LOGGED_MESSAGES'.
    {
        {
            baljsn::Encoder encoder;

            bsl::vector<char>           buffer(BUFFER_SIZE);
            bdlsb::FixedMemOutStreamBuf streamBuffer(buffer.data(),
                                                     BUFFER_SIZE);
            baljsn::EncoderOptions      encoderOptions;

            const int rc =
                encoder.encode(&streamBuffer, VALUE, encoderOptions);

            assertExpectations(&streamBuffer,
                               rc,
                               encoder,
                               LINE,
                               SUCCESS,
                               EXPECTED_JSON_STRING,
                               EXPECTED_LOGGED_MESSAGES);
        }

        {
            baljsn::Encoder encoder;

            bsl::vector<char>           buffer(BUFFER_SIZE);
            bdlsb::FixedMemOutStreamBuf streamBuffer(buffer.data(),
                                                     BUFFER_SIZE);
            baljsn::EncoderOptions      encoderOptions;

            int rc = encoder.encode(&streamBuffer, VALUE, &encoderOptions);

            assertExpectations(&streamBuffer,
                               rc,
                               encoder,
                               LINE,
                               SUCCESS,
                               EXPECTED_JSON_STRING,
                               EXPECTED_LOGGED_MESSAGES);
        }

        {
            baljsn::Encoder encoder;

            bsl::vector<char>           buffer(BUFFER_SIZE);
            bdlsb::FixedMemOutStreamBuf streamBuffer(buffer.data(),
                                                     BUFFER_SIZE);
            bsl::ostream                stream(&streamBuffer);
            baljsn::EncoderOptions      encoderOptions;

            int rc = encoder.encode(stream, VALUE, encoderOptions);

            assertExpectations(&streamBuffer,
                               rc,
                               encoder,
                               LINE,
                               SUCCESS,
                               EXPECTED_JSON_STRING,
                               EXPECTED_LOGGED_MESSAGES);
        }

        {
            baljsn::Encoder encoder;

            bsl::vector<char>           buffer(BUFFER_SIZE);
            bdlsb::FixedMemOutStreamBuf streamBuffer(buffer.data(),
                                                     BUFFER_SIZE);
            bsl::ostream                stream(&streamBuffer);
            baljsn::EncoderOptions      encoderOptions;

            int rc = encoder.encode(stream, VALUE, &encoderOptions);

            assertExpectations(&streamBuffer,
                               rc,
                               encoder,
                               LINE,
                               SUCCESS,
                               EXPECTED_JSON_STRING,
                               EXPECTED_LOGGED_MESSAGES);
        }

        {
            baljsn::Encoder encoder;

            bsl::vector<char>           buffer(BUFFER_SIZE);
            bdlsb::FixedMemOutStreamBuf streamBuffer(buffer.data(),
                                                     BUFFER_SIZE);

            int rc = encoder.encode(&streamBuffer, VALUE);

            assertExpectations(&streamBuffer,
                               rc,
                               encoder,
                               LINE,
                               SUCCESS,
                               EXPECTED_JSON_STRING,
                               EXPECTED_LOGGED_MESSAGES);
        }

        {
            baljsn::Encoder encoder;

            bsl::vector<char>           buffer(BUFFER_SIZE);
            bdlsb::FixedMemOutStreamBuf streamBuffer(buffer.data(),
                                                     BUFFER_SIZE);
            bsl::ostream                stream(&streamBuffer);

            int rc = encoder.encode(stream, VALUE);

            assertExpectations(&streamBuffer,
                               rc,
                               encoder,
                               LINE,
                               SUCCESS,
                               EXPECTED_JSON_STRING,
                               EXPECTED_LOGGED_MESSAGES);
        }
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
    void operator()(int                     LINE,
                    const VALUE_TYPE&       VALUE,
                    const bsl::string_view& EXPECTED_JSON_STRING) const
        // Assert that encoding the specified 'VALUE' using 'baljsn::Encoder'
        // with default options yields the specified 'EXPECTED_JSON_STRING'.
    {
        TestUtil::assertEncodedValueIsEqual(LINE,
                                            baljsn::EncodingStyle::e_COMPACT,
                                            EncodeNullElements::e_NO,
                                            EncodeEmptyArrays::e_NO,
                                            VALUE,
                                            EXPECTED_JSON_STRING);
    }

    template <class VALUE_TYPE>
    void operator()(int                     LINE,
                    bool                    ENCODE_NULL_ELEMENTS,
                    const VALUE_TYPE&       VALUE,
                    const bsl::string_view& EXPECTED_JSON_STRING) const
        // Assert that encoding the specified 'VALUE' using 'baljsn::Encoder'
        // with the specified 'ENCODE_NULL_ELEMENTS' option yields the
        // specified 'EXPECTED_JSON_STRING'.
    {
        return TestUtil::assertEncodedValueIsEqual(
            LINE,
            baljsn::EncodingStyle::e_COMPACT,
            ENCODE_NULL_ELEMENTS ? EncodeNullElements::e_YES
                                 : EncodeNullElements::e_NO,
            EncodeEmptyArrays::e_NO,
            VALUE,
            EXPECTED_JSON_STRING);
    }

    template <class VALUE_TYPE>
    void operator()(int                          LINE,
                    baljsn::EncodingStyle::Value ENCODING_STYLE,
                    bool                         ENCODE_NULL_ELEMENTS,
                    const VALUE_TYPE&            VALUE,
                    const bsl::string_view&      EXPECTED_JSON_STRING) const
        // Assert that encoding the specified 'VALUE' using 'baljsn::Encoder'
        // with the specified 'ENCODING_STYLE' and 'ENCODE_NULL_ELEMENTS'
        // options yields the specified 'EXPECTED_JSON_STRING'.
    {
        TestUtil::assertEncodedValueIsEqual(LINE,
                                            ENCODING_STYLE,
                                            ENCODE_NULL_ELEMENTS
                                                ? EncodeNullElements::e_YES
                                                : EncodeNullElements::e_NO,
                                            EncodeEmptyArrays::e_NO,
                                            VALUE,
                                            EXPECTED_JSON_STRING);
    }

    template <class VALUE_TYPE>
    void operator()(int                     LINE,
                    EncodeEmptyArrays::Enum ENCODE_EMPTY_ARRAYS,
                    const VALUE_TYPE&       VALUE,
                    const bsl::string_view& EXPECTED_JSON_STRING) const
        // Assert that encoding the specified 'VALUE' using 'baljsn::Encoder'
        // with the specified 'ENCODE_EMPTY_ARRAYS' option yields the specified
        // 'EXPECTED_JSON_STRING'.
    {
        TestUtil::assertEncodedValueIsEqual(LINE,
                                            baljsn::EncodingStyle::e_COMPACT,
                                            EncodeNullElements::e_NO,
                                            ENCODE_EMPTY_ARRAYS,
                                            VALUE,
                                            EXPECTED_JSON_STRING);
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
    template <class ELEMENT_TYPE>
    void operator()(int                     LINE,
                    const ELEMENT_TYPE&     ELEMENT,
                    const bsl::string_view& EXPECTED_ELEMENT_STRING) const
        // Assert that encoding various nested and non-nested 'bsl::vector'
        // objects with copies of the specified 'ELEMENT' as terminal
        // element(s) using 'baljsn::Encoder' with all permutations of the
        // 'EncodeEmptyArrays' option yields an equivalently-structured nested
        // or non-nested JSON array with the specified terminal
        // 'EXPECTED_ELEMENT_STRING'.
    {
        const AssertEncodedValueIsEqualFunction       TEST;
        const s_baltst::TestPlaceHolder<ELEMENT_TYPE> e;
        const s_baltst::GenerateTestArray             a;
        const Replace                                 fmt;

        const int              L = LINE;
        const ELEMENT_TYPE&    E = ELEMENT;
        const bsl::string_view S = EXPECTED_ELEMENT_STRING;

        typedef u::EncodeEmptyArrays Option;
        static const Option::Enum    Y = Option::e_YES;
        static const Option::Enum    N = Option::e_NO;

        //     LINE
        //    .----
        //    |    ENCODE EMPTY ARRAYS
        //    |   .-------------------
        //    |  /     ARRAY OBJECT                  EXPECTED JSON
        //   -- - ------------------------ ----------------------------------
        TEST(L, N, a(e)                   , fmt(""                   , "S",S));
        TEST(L, N, a(E)                   , fmt("[S]"                , "S",S));
        TEST(L, N, a(E,E)                 , fmt("[S,S]"              , "S",S));
        TEST(L, N, a(E,E,E)               , fmt("[S,S,S]"            , "S",S));
#ifndef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
        TEST(L, N, a(a(e))                , fmt("[]"                 , "S",S));
        TEST(L, N, a(a(E))                , fmt("[[S]]"              , "S",S));
        TEST(L, N, a(a(E,E))              , fmt("[[S,S]]"            , "S",S));
        TEST(L, N, a(a(E,E,E))            , fmt("[[S,S,S]]"          , "S",S));
        TEST(L, N, a(a(e),a(e))           , fmt("[]"                 , "S",S));
        TEST(L, N, a(a(e),a(E))           , fmt("[[S]]"              , "S",S));
        TEST(L, N, a(a(e),a(E,E))         , fmt("[[S,S]]"            , "S",S));
        TEST(L, N, a(a(e),a(E,E,E))       , fmt("[[S,S,S]]"          , "S",S));
        TEST(L, N, a(a(E),a(e))           , fmt("[[S]]"              , "S",S));
        TEST(L, N, a(a(E),a(E))           , fmt("[[S],[S]]"          , "S",S));
        TEST(L, N, a(a(E),a(E,E))         , fmt("[[S],[S,S]]"        , "S",S));
        TEST(L, N, a(a(E),a(E,E,E))       , fmt("[[S],[S,S,S]]"      , "S",S));
        TEST(L, N, a(a(E,E),a(e))         , fmt("[[S,S]]"            , "S",S));
        TEST(L, N, a(a(E,E),a(E))         , fmt("[[S,S],[S]]"        , "S",S));
        TEST(L, N, a(a(E,E),a(E,E))       , fmt("[[S,S],[S,S]]"      , "S",S));
        TEST(L, N, a(a(E,E),a(E,E,E))     , fmt("[[S,S],[S,S,S]]"    , "S",S));
        TEST(L, N, a(a(E,E,E),a(e))       , fmt("[[S,S,S]]"          , "S",S));
        TEST(L, N, a(a(E,E,E),a(E))       , fmt("[[S,S,S],[S]]"      , "S",S));
        TEST(L, N, a(a(E,E,E),a(E,E))     , fmt("[[S,S,S],[S,S]]"    , "S",S));
        TEST(L, N, a(a(E,E,E),a(E,E,E))   , fmt("[[S,S,S],[S,S,S]]"  , "S",S));
        TEST(L, N, a(a(e),a(e),a(e))      , fmt("[]"                 , "S",S));
        TEST(L, N, a(a(e),a(e),a(E))      , fmt("[[S]]"              , "S",S));
        TEST(L, N, a(a(e),a(e),a(E,E))    , fmt("[[S,S]]"            , "S",S));
        TEST(L, N, a(a(e),a(E),a(e))      , fmt("[[S]]"              , "S",S));
        TEST(L, N, a(a(e),a(E),a(E))      , fmt("[[S],[S]]"          , "S",S));
        TEST(L, N, a(a(e),a(E),a(E,E))    , fmt("[[S],[S,S]]"        , "S",S));
        TEST(L, N, a(a(e),a(E,E),a(e))    , fmt("[[S,S]]"            , "S",S));
        TEST(L, N, a(a(e),a(E,E),a(E))    , fmt("[[S,S],[S]]"        , "S",S));
        TEST(L, N, a(a(e),a(E,E),a(E,E))  , fmt("[[S,S],[S,S]]"      , "S",S));
        TEST(L, N, a(a(E),a(e),a(e))      , fmt("[[S]]"              , "S",S));
        TEST(L, N, a(a(E),a(e),a(E))      , fmt("[[S],[S]]"          , "S",S));
        TEST(L, N, a(a(E),a(e),a(E,E))    , fmt("[[S],[S,S]]"        , "S",S));
        TEST(L, N, a(a(E),a(E),a(e))      , fmt("[[S],[S]]"          , "S",S));
        TEST(L, N, a(a(E),a(E),a(E))      , fmt("[[S],[S],[S]]"      , "S",S));
        TEST(L, N, a(a(E),a(E),a(E,E))    , fmt("[[S],[S],[S,S]]"    , "S",S));
        TEST(L, N, a(a(E),a(E,E),a(e))    , fmt("[[S],[S,S]]"        , "S",S));
        TEST(L, N, a(a(E),a(E,E),a(E))    , fmt("[[S],[S,S],[S]]"    , "S",S));
        TEST(L, N, a(a(E),a(E,E),a(E,E))  , fmt("[[S],[S,S],[S,S]]"  , "S",S));
        TEST(L, N, a(a(E,E),a(e),a(e))    , fmt("[[S,S]]"            , "S",S));
        TEST(L, N, a(a(E,E),a(e),a(E))    , fmt("[[S,S],[S]]"        , "S",S));
        TEST(L, N, a(a(E,E),a(e),a(E,E))  , fmt("[[S,S],[S,S]]"      , "S",S));
        TEST(L, N, a(a(E,E),a(E),a(e))    , fmt("[[S,S],[S]]"        , "S",S));
        TEST(L, N, a(a(E,E),a(E),a(E))    , fmt("[[S,S],[S],[S]]"    , "S",S));
        TEST(L, N, a(a(E,E),a(E),a(E,E))  , fmt("[[S,S],[S],[S,S]]"  , "S",S));
        TEST(L, N, a(a(E,E),a(E,E),a(e))  , fmt("[[S,S],[S,S]]"      , "S",S));
        TEST(L, N, a(a(E,E),a(E,E),a(E))  , fmt("[[S,S],[S,S],[S]]"  , "S",S));
        TEST(L, N, a(a(E,E),a(E,E),a(E,E)), fmt("[[S,S],[S,S],[S,S]]", "S",S));
        TEST(L, Y, a(e)                   , fmt("[]"                 , "S",S));
        TEST(L, Y, a(E)                   , fmt("[S]"                , "S",S));
        TEST(L, Y, a(E,E)                 , fmt("[S,S]"              , "S",S));
        TEST(L, Y, a(E,E,E)               , fmt("[S,S,S]"            , "S",S));
        TEST(L, Y, a(a(e))                , fmt("[[]]"               , "S",S));
        TEST(L, Y, a(a(E))                , fmt("[[S]]"              , "S",S));
        TEST(L, Y, a(a(E,E))              , fmt("[[S,S]]"            , "S",S));
        TEST(L, Y, a(a(E,E,E))            , fmt("[[S,S,S]]"          , "S",S));
        TEST(L, Y, a(a(e),a(e))           , fmt("[[],[]]"            , "S",S));
        TEST(L, Y, a(a(e),a(E))           , fmt("[[],[S]]"           , "S",S));
        TEST(L, Y, a(a(e),a(E,E))         , fmt("[[],[S,S]]"         , "S",S));
        TEST(L, Y, a(a(e),a(E,E,E))       , fmt("[[],[S,S,S]]"       , "S",S));
        TEST(L, Y, a(a(E),a(e))           , fmt("[[S],[]]"           , "S",S));
        TEST(L, Y, a(a(E),a(E))           , fmt("[[S],[S]]"          , "S",S));
        TEST(L, Y, a(a(E),a(E,E))         , fmt("[[S],[S,S]]"        , "S",S));
        TEST(L, Y, a(a(E),a(E,E,E))       , fmt("[[S],[S,S,S]]"      , "S",S));
        TEST(L, Y, a(a(E,E),a(e))         , fmt("[[S,S],[]]"         , "S",S));
        TEST(L, Y, a(a(E,E),a(E))         , fmt("[[S,S],[S]]"        , "S",S));
        TEST(L, Y, a(a(E,E),a(E,E))       , fmt("[[S,S],[S,S]]"      , "S",S));
        TEST(L, Y, a(a(E,E),a(E,E,E))     , fmt("[[S,S],[S,S,S]]"    , "S",S));
        TEST(L, Y, a(a(E,E,E),a(e))       , fmt("[[S,S,S],[]]"       , "S",S));
        TEST(L, Y, a(a(E,E,E),a(E))       , fmt("[[S,S,S],[S]]"      , "S",S));
        TEST(L, Y, a(a(E,E,E),a(E,E))     , fmt("[[S,S,S],[S,S]]"    , "S",S));
        TEST(L, Y, a(a(E,E,E),a(E,E,E))   , fmt("[[S,S,S],[S,S,S]]"  , "S",S));
        TEST(L, Y, a(a(e),a(e),a(e))      , fmt("[[],[],[]]"         , "S",S));
        TEST(L, Y, a(a(e),a(e),a(E))      , fmt("[[],[],[S]]"        , "S",S));
        TEST(L, Y, a(a(e),a(e),a(E,E))    , fmt("[[],[],[S,S]]"      , "S",S));
        TEST(L, Y, a(a(e),a(E),a(e))      , fmt("[[],[S],[]]"        , "S",S));
        TEST(L, Y, a(a(e),a(E),a(E))      , fmt("[[],[S],[S]]"       , "S",S));
        TEST(L, Y, a(a(e),a(E),a(E,E))    , fmt("[[],[S],[S,S]]"     , "S",S));
        TEST(L, Y, a(a(e),a(E,E),a(e))    , fmt("[[],[S,S],[]]"      , "S",S));
        TEST(L, Y, a(a(e),a(E,E),a(E))    , fmt("[[],[S,S],[S]]"     , "S",S));
        TEST(L, Y, a(a(e),a(E,E),a(E,E))  , fmt("[[],[S,S],[S,S]]"   , "S",S));
        TEST(L, Y, a(a(E),a(e),a(e))      , fmt("[[S],[],[]]"        , "S",S));
        TEST(L, Y, a(a(E),a(e),a(E))      , fmt("[[S],[],[S]]"       , "S",S));
        TEST(L, Y, a(a(E),a(e),a(E,E))    , fmt("[[S],[],[S,S]]"     , "S",S));
        TEST(L, Y, a(a(E),a(E),a(e))      , fmt("[[S],[S],[]]"       , "S",S));
        TEST(L, Y, a(a(E),a(E),a(E))      , fmt("[[S],[S],[S]]"      , "S",S));
        TEST(L, Y, a(a(E),a(E),a(E,E))    , fmt("[[S],[S],[S,S]]"    , "S",S));
        TEST(L, Y, a(a(E),a(E,E),a(e))    , fmt("[[S],[S,S],[]]"     , "S",S));
        TEST(L, Y, a(a(E),a(E,E),a(E))    , fmt("[[S],[S,S],[S]]"    , "S",S));
        TEST(L, Y, a(a(E),a(E,E),a(E,E))  , fmt("[[S],[S,S],[S,S]]"  , "S",S));
        TEST(L, Y, a(a(E,E),a(e),a(e))    , fmt("[[S,S],[],[]]"      , "S",S));
        TEST(L, Y, a(a(E,E),a(e),a(E))    , fmt("[[S,S],[],[S]]"     , "S",S));
        TEST(L, Y, a(a(E,E),a(e),a(E,E))  , fmt("[[S,S],[],[S,S]]"   , "S",S));
        TEST(L, Y, a(a(E,E),a(E),a(e))    , fmt("[[S,S],[S],[]]"     , "S",S));
        TEST(L, Y, a(a(E,E),a(E),a(E))    , fmt("[[S,S],[S],[S]]"    , "S",S));
        TEST(L, Y, a(a(E,E),a(E),a(E,E))  , fmt("[[S,S],[S],[S,S]]"  , "S",S));
#endif
        TEST(L, Y, a(a(E,E),a(E,E),a(e))  , fmt("[[S,S],[S,S],[]]"   , "S",S));
        TEST(L, Y, a(a(E,E),a(E,E),a(E))  , fmt("[[S,S],[S,S],[S]]"  , "S",S));
        TEST(L, Y, a(a(E,E),a(E,E),a(E,E)), fmt("[[S,S],[S,S],[S,S]]", "S",S));
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

// TRAITS
void bdlat_enumToString(bsl::string *result, const Enumeration0&);

}  // close u namespace
}  // close unnamed namespace

template <>
struct bdlat_IsBasicEnumeration<u::Enumeration0> : bsl::true_type {
};

namespace bdlat_EnumFunctions {

template <>
struct IsEnumeration<u::Enumeration0> {
    enum { VALUE = 1 };
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
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    static_cast<void>(verbose);
    static_cast<void>(veryVerbose);
    static_cast<void>(veryVeryVerbose);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 23: {
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

        if (verbose)
            cout << endl
                 << "TESTING USAGE EXAMPLE" << endl
                 << "=====================" << endl;

        namespace test = ::BloombergLP::s_baltst;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Encoding a 'bas_codegen.pl'-generated object into JSON
///-----------------------------------------------------------------
// Consider that we want to exchange an employee's information between two
// processes.  To allow this information exchange we will define the XML schema
// representation for that class, use 'bas_codegen.pl' to create the 'Employee'
// 'class' for storing that information, populate an 'Employee' object, and
// encode that object using the baljsn encoder.
//
// First, we will define the XML schema inside a file called 'employee.xsd':
//..
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
//..
// Then, we will use the 'bas_codegen.pl' tool, to generate the C++ classes for
// this schema.  The following command will generate the header and
// implementation files for the all the classes in the 'test_messages'
// components in the current directory:
//..
//  $ bas_codegen.pl -m msg -p test xsdfile.xsd
//..
// Next, we will populate a 'test::Employee' object:
//..
    test::Employee employee;
    employee.name()                 = "Bob";
    employee.homeAddress().street() = "Lexington Ave";
    employee.homeAddress().city()   = "New York City";
    employee.homeAddress().state()  = "New York";
    employee.age()                  = 21;
//..
// Then, we will create a 'baljsn::Encoder' object:
//..
    baljsn::Encoder encoder;
//..
// Now, we will output this object in the JSON format by invoking the 'encode'
// method of the encoder.  We will also create a 'baljsn::EncoderOptions'
// object that allows us to specify that the encoding should be done in a
// pretty format, and what the initial indent level and spaces per level should
// be.  We will then pass that object to the 'encode' method:
//..
    bsl::ostringstream os;

    baljsn::EncoderOptions options;
    options.setEncodingStyle(baljsn::EncoderOptions::e_PRETTY);
    options.setInitialIndentLevel(1);
    options.setSpacesPerLevel(4);

    const int rc = encoder.encode(os, employee, options);
    ASSERT(!rc);
    ASSERT(os);
//..
// Finally, we will verify that the output is as expected:
//..
    const char EXP_OUTPUT[] = "    {\n"
                              "        \"name\" : \"Bob\",\n"
                              "        \"homeAddress\" : {\n"
                              "            \"street\" : \"Lexington Ave\",\n"
                              "            \"city\" : \"New York City\",\n"
                              "            \"state\" : \"New York\"\n"
                              "        },\n"
                              "        \"age\" : 21\n"
                              "    }\n";

    ASSERT(EXP_OUTPUT == os.str());
//..
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING ENCODING VECTORS OF VECTORS
        //   This case tests that 'baljsn::Encoder::encode' supports encoding
        //   vectors of vectors.
        //
        // Concerns:
        //: 1 Encoding a vector of vectors of a 'bdlat'-compatible type
        //:   emits valid JSON array of arrays.
        //
        // Plan:
        //: 1 Perform a depth-ordered enumeration of encoding all vectors of
        //:   vectors of integers up to depth 2 and width 2, and verify that
        //:   the encoded JSON is the corresponding array of arrays.
        //:
        //: 2 For some terminal 'bdlat' element types, enumerate a large
        //:   number of different nested and non-nested arrays of these types
        //:   and verify that the array encodes to a corresponding nested or
        //:   non-nested JSON array.
        //
        // Testing:
        //   ENCODING VECTORS OF VECTORS
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING ENCODING VECTORS OF VECTORS" << endl
                 << "===================================" << endl;


        namespace ux = baljsn::encoder::u;

        typedef s_baltst::TestAttribute<1, ux::k_ATTRIBUTE_1_NAME>   At1;
        typedef s_baltst::TestAttribute<2, ux::k_ATTRIBUTE_2_NAME>   At2;
        typedef s_baltst::TestEnumerator<1, ux::k_ENUMERATOR_1_NAME> En1;
        typedef s_baltst::TestSelection<1, ux::k_SELECTION_1_NAME>   Se1;

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


#define JSON(...) #__VA_ARGS__

        const u::AssertEncodedArrayOfValuesIsEqualFunction TEST;


        //      LINE
        //     .----
        //    /  OBJECT TO ENCODE        EXPECTED JSON
        //   -- ------------------- ----------------------
        TEST(L_, 1                 , JSON(1)              );
        TEST(L_, a_(1)             , JSON([1])            );
#ifndef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
        TEST(L_, c_(se1, 1)        , JSON({"s1":1})       );
        TEST(L_, t_(1, ip)         , JSON(1)              );
        TEST(L_, e_(en1, 1)        , JSON("e1")           );
        TEST(L_, n_(ip)            , JSON(null)           );
        TEST(L_, n_(1)             , JSON(1)              );
        TEST(L_, s_(at1, 1)        , JSON({"a1":1})       );
        TEST(L_, s_(at1, at2, 1, 2), JSON({"a1":1,"a2":2}));
#endif

        const u::AssertEncodedValueIsEqualFunction TEST2;

        //      LINE
        //     .----
        //    /       OBJECT TO ENCODE                 EXPECTED JSON
        //   -- ------------------------------ ------------------------------
        TEST2(L_, c_(se1,a_(ip))              , JSON({"s1":[]})              );
        TEST2(L_, c_(se1,a_(1))               , JSON({"s1":[1]})             );
        TEST2(L_, c_(se1,a_(1,1))             , JSON({"s1":[1,1]})           );
        TEST2(L_, c_(se1,a_(1,1,1))           , JSON({"s1":[1,1,1]})         );
        TEST2(L_, s_(at1,a_(ip))              , JSON({})                     );
        TEST2(L_, s_(at1,a_(1))               , JSON({"a1":[1]})             );
        TEST2(L_, s_(at1,a_(1,1))             , JSON({"a1":[1,1]})           );
        TEST2(L_, s_(at1,a_(1,1,1))           , JSON({"a1":[1,1,1]})         );
        TEST2(L_, s_(at1,at2,a_(ip),a_(ip))   , JSON({})                     );
        TEST2(L_, s_(at1,at2,a_(1),a_(ip))    , JSON({"a1":[1]})             );
        TEST2(L_, s_(at1,at2,a_(1,1),a_(ip))  , JSON({"a1":[1,1]})           );
        TEST2(L_, s_(at1,at2,a_(1,1,1),a_(ip)), JSON({"a1":[1,1,1]})         );
        TEST2(L_, s_(at1,at2,a_(ip),a_(1))    , JSON({"a2":[1]})             );
        TEST2(L_, s_(at1,at2,a_(1),a_(1))     , JSON({"a1":[1],"a2":[1]})    );
        TEST2(L_, s_(at1,at2,a_(1,1),a_(1))   , JSON({"a1":[1,1],"a2":[1]})  );
        TEST2(L_, s_(at1,at2,a_(1,1,1),a_(1)) , JSON({"a1":[1,1,1],"a2":[1]}));

#undef JSON

      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING ENCODING NULL CHOICE
        //   This case tests that 'baljsn::Encoder::encode' returns a non-zero
        //   value if the supplied object contains a null choice.
        //
        // Concerns:
        //: 1 When encoding an object containing null nullable choice, and
        //:   'encodeNullElements' is 'false' then the encoder succeeds and
        //:   returns valid json (DRQS 165224090)
        //: 2 When encoding an object containing null nullable choice, and
        //:   'encodeNullElements' is 'false' then the encoder returns a non-0
        //:   value, and does not trigger any assertions (DRQS 165224090)
        //
        // Plan:
        //: 1 Create a message object of type
        //:   'MySequenceWithNullableAnonymousChoice' which contains an null
        //:   choice and ensure that 'encode' returns a 0 value and returns
        //:   json "{}".
        //: 2 Create a message object of type
        //:   'MySequenceWithNullableAnonymousChoice' which contains an null
        //:   choice and ensure that, when 'encodeNullElements' is set,
        //:   'encode' returns a non-0 value.
        //
        // Testing:
        //   ENCODING NULL CHOICE
        // --------------------------------------------------------------------

        // Important: Note that this test case has been (temporarily) modified
        // to verify that encoding a null nullable choice succeeds (returns 0)
        // and yields malformed json (e.g., "{null}").  It was found that a
        // client depended upon this behavior.

        if (verbose) cout << "\nTESTING ENCODING UNSET CHOICE"
                          << "\n=============================" << endl;

        {
            bsl::ostringstream                          out;
            BloombergLP::s_baltst::MySequenceWithChoice obj;

            baljsn::Encoder        encoder;
            baljsn::EncoderOptions options;
            options.setEncodingStyle(baljsn::EncoderOptions::e_PRETTY);

            const int rc = encoder.encode(out, obj, options);
            ASSERT(0 != rc);
            if (verbose) { P(rc); P(encoder.loggedMessages()); P(out.str()); }
        }
        // Two iterations to cover both possible settings for
        // 'encodeNullElements'
        for (int loop = 0; loop <= 1; loop++) {
            if (veryVerbose)
                bsl::cout << "Running test for encodeNullElements="
                          << ((0 == loop) ? "false\n" : "true\n");

            bsl::ostringstream                                           out;
            BloombergLP::s_baltst::MySequenceWithNullableAnonymousChoice obj;

            baljsn::Encoder        encoder;
            baljsn::EncoderOptions options;
            options.setEncodingStyle(baljsn::EncoderOptions::e_PRETTY);
            if (0 == loop) {
                options.setEncodeNullElements(false);
            }
            else {
                options.setEncodeNullElements(true);
            }

            const int rc = encoder.encode(out, obj, options);

            if (0 == loop) {
                ASSERTV(rc, 0 == rc);
                ASSERTV(out.str(), "{\n\n}\n" == out.str());
            }
            else {
                ASSERTV(rc, 0 == rc);
                ASSERTV(out.str(), "{\nnull\n}\n" == out.str());
            }
            if (veryVerbose) {
                P(rc);
                P(encoder.loggedMessages());
                P(out.str());
            }
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING ENCODING UNSET CHOICE
        //   This case tests that 'baljsn::Encoder::encode' returns a non-zero
        //   value if the supplied object contains an unset choice.
        //
        // Concerns:
        //: 1 When encoding an object containing an unset choice, the encoder
        //:   returns a non-0 value, and does not trigger any assertions.
        //
        // Plan:
        //: 1 Create a message object of type 'MySequenceWithChoice' which
        //:   contains an unset choice in its sub-object of type
        //:   'MySequenceWithChoice' and ensure than 'encode' returns a
        //:   non-0 value.
        //
        // Testing:
        //   ENCODING UNSET CHOICE
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING ENCODING UNSET CHOICE" << endl
                 << "=============================" << endl;
        {
            bsl::ostringstream                          out;
            BloombergLP::s_baltst::MySequenceWithChoice obj;

            baljsn::Encoder        encoder;
            baljsn::EncoderOptions options;
            options.setEncodingStyle(baljsn::EncoderOptions::e_PRETTY);

            const int rc = encoder.encode(out, obj, options);
            ASSERT(0 != rc);
            if (verbose) { P(rc); P(encoder.loggedMessages()); P(out.str()); }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING ENCODING OVERFLOW DETECTION
        //   This case tests that 'baljsn::Encoder::encode' returns a non-zero
        //   value if the supplied stream or stream buffer indicates an error
        //   occurs during any output operation.
        //
        // Concerns:
        //: 1 If all output operations on the supplied stream or stream buffer
        //:   succeed, the encoder returns a 0 value, successfully encodes the
        //:   JSON representation of the supplied value to the stream or stream
        //:   buffer, and has no logged messages.
        //:
        //: 2 If any output operation on the supplied stream or stream buffer
        //:   fails, the encoder returns a non-zero value, encodes a prefix of
        //:   the JSON representation of the supplied value -- up to and
        //:   including all successfully written characters -- to the stream or
        //:   stream buffer, and has logged messages.
        //
        // Plan:
        //: 1 Create 2 's_baltst::SimpleRequest' objects, 'obj0' and 'obj1',
        //:   that have different values for their attributes so that they
        //:   will encode to JSON representations that have different lengths.
        //:
        //: 2 For both objects 'obj0' and 'obj1', do the following:
        //:
        //:   1 Identify the length 'L' of the JSON representation of the
        //:     object.
        //:
        //:   2 Initialize output stream buffers that fail after 0, 1, 2,
        //:     L - 2, and L - 1 bytes have been written, respectively.
        //:
        //:   3 Encode the object to the output sequence of each stream buffer.
        //:
        //:   4 Observe that the encoding operation fails, a prefix of the
        //:     encoded data is in the output sequence of the stream buffer,
        //:     and that the encoder's logged messages contain a description of
        //:     the error.
        //:
        //:   5 Initialize a second set of output stream buffers that fail
        //:     after L, L + 1, and L + 2 bytes have been written,
        //:     respectively.
        //:
        //:   6 Encode the object to the output sequence of each stream buffer.
        //:
        //:   7 Observe that the encoding operation succeeds, the full JSON
        //:     representation of the object is in the output sequence of the
        //:     stream buffer, and that the encoder's logged messages are
        //:     empty.
        //
        // Testing:
        //   int encode(*streamBuf, value, options);
        //   int encode(*streamBuf, value, *options);
        //   int encode(stream, value, options);
        //   int encode(stream, value, *options);
        //   int encode(*streamBuf, value);
        //   int encode(stream, value);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING ENCODING OVERFLOW DETECTION"
                          << "\n===================================" << endl;

        u::AssertEncodingOverflowIsDetectedFunction t;

        static const bool F = false;
        static const bool T = true;

        static const char *const ERROR_P =
                                    "Unable to encode element name: 'data'.\n";
            // Error at document preamble.

        static const char *const ERROR_C =
            "An error occurred when writing to the supplied output stream or"
            " stream buffer.\n";
            // Error at document close.

        s_baltst::SimpleRequest obj0;
        obj0.data()           = "";
        obj0.responseLength() = 0;

        s_baltst::SimpleRequest obj1;
        obj1.data()           = "Lorem ipsum dolor sit amet.";
        obj1.responseLength() = 42;

        //            OBJECT
        //           .------
        //          /    BUFFER SIZE
        //   LINE  /    .-----------
        //  .---- /    /   ENCODING SUCCEEDS   EXPECTED ERROR MESSAGE
        //  |    /    /   .-----------------   ----------------------.
        //  |   /    /   /     EXPECTED STREAM BUFFER CONTENTS       |
        // -- ----- --- -- --------------------------------------- -------
        t( L_, obj0,  0, F, ""                                    , ERROR_P );
        t( L_, obj0,  1, F, "{"                                   , ERROR_P );
        t( L_, obj0,  2, F, "{\""                                 , ERROR_P );
        t( L_, obj0, 28, F, "{\"data\":\"\",\"responseLength\":"  , ERROR_C );
        t( L_, obj0, 29, F, "{\"data\":\"\",\"responseLength\":0" , ERROR_C );
        t( L_, obj0, 30, T, "{\"data\":\"\",\"responseLength\":0}", ""    );
        t( L_, obj0, 31, T, "{\"data\":\"\",\"responseLength\":0}", ""    );

        t( L_, obj1,  0, F, ""                                    , ERROR_P );
        t( L_, obj1,  1, F, "{"                                   , ERROR_P );
        t( L_, obj1,  2, F, "{"
                                "\""                              , ERROR_P );
        t( L_, obj1, 57, F, "{"
                                "\"data\":\"Lorem ipsum dolor"
                                           " sit amet.\","
                                "\"responseLength\":42"           , ERROR_C );
        t( L_, obj1, 58, T, "{"
                                "\"data\":\"Lorem ipsum dolor"
                                           " sit amet.\","
                                "\"responseLength\":42"
                            "}"                                   , ""    );
        t( L_, obj1, 59, T, "{"
                                "\"data\":\"Lorem ipsum dolor"
                                           " sit amet.\","
                                "\"responseLength\":42"
                            "}"                                   , ""    );
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING DEGENERATE CHOICE VALUES
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
        //: 1 The encoder emits empty-object token sequences for choice values
        //:   having an empty selection with the untagged formatting mode.
        //
        // Plan:
        //: 1 Define a choice type that has 1 selection, which is an empty,
        //:   anonymous sequence.
        //:
        //: 2 Identify the 2 boolean options that affect relevant aspects of
        //:   the encoding of such choices:
        //:
        //:   1 Whether the encoding style is compact or pretty
        //:
        //:   2 Whether or not to encode empty values
        //:
        //: 3 For each combination a compact or pretty encoding style, and a
        //:   'true' or 'false' empty-value encoding setting, perform the
        //:   following:
        //:
        //:   1 Encode the choice using the specified encoding style and
        //:     empty-value encoding setting
        //:
        //:   2 Verify that the emitted JSON is an empty-object
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING DEGENERATE CHOICE VALUES"
                 << "\n================================" << endl;

        const u::AssertEncodedValueIsEqualFunction t;

        static const baljsn::EncodingStyle::Value C =
            baljsn::EncodingStyle::e_COMPACT;
        static const baljsn::EncodingStyle::Value P =
            baljsn::EncodingStyle::e_PRETTY;

        static const bool T = true;
        static const bool F = false;

        // 'Type0' is unsupported and causes the encoder to violate the
        // invariants of the formatter.  This was last verified April 20, 2020.
        //typedef baljsn::EncoderTestDegenerateChoice0 Type0;
        typedef baljsn::EncoderTestDegenerateChoice1 Type1;

        // Do not test degenerate choice type 0.
        //Type0 obj0;
        //obj0.makeChoice();

        Type1  obj1;
        obj1.makeSequence();

        //               ENCODE EMPTIES
        //              .--------------
        // LINE STYLE  /  OBJECT EXPECTED RESULT
        // ---- ----- --- ------ --------------------------------------
        // do not test degenerate choice type 0
        //t( L_  , C   , F , obj0 , "{}"                                 );
        //t( L_  , C   , T , obj0 , "{}"                                 );
        //t( L_  , P   , F , obj0 , "{\n\n}\n"                           );
        //t( L_  , P   , T , obj0 , "{\n\n}\n"                           );
        t( L_  , C   , F , obj1 , "{}"                                 );
        t( L_  , C   , T , obj1 , "{}"                                 );
        t( L_  , P   , F , obj1 , "{\n\n}\n"                           );
        t( L_  , P   , T , obj1 , "{\n\n}\n"                           );
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING SEQUENCES WITH ATTRIBUTES OF ALL VALUE CATEGORIES
        //   This case tests that the encoder emits valid JSON when encoding
        //   sequence attributes having any of the 7 'bdlat' value categories
        //   that the encoder supports for attributes.
        //
        // Concerns:
        //: 1 Regardless of value category, the encoder emits a valid and
        //:   correct JSON representation of a sequence attribute.
        //
        // Plan:
        //: 1 Define a sequence type that has 7 attributes whose types cover
        //:   all 7 'bdlat' value categories that are supported for selections.
        //:
        //: 2 Identify the 2 boolean options that affect relevant aspects of
        //:   the encoding of such sequences:
        //:
        //:   1 Whether the encoding style is compact or pretty
        //:
        //:   2 Whether or not to encode empty values
        //:
        //: 3 For each combination a compact or pretty encoding style, and a
        //:   'true' or 'false' empty-value encoding setting, perform the
        //:   following:
        //:
        //:   1 Encode the sequence using the specified encoding style and
        //:     empty-value encoding setting
        //:
        //:   2 Verify that the emitted JSON is valid
        //:
        //:   3 Verify that the emitted JSON is a correct representation of
        //:     the encoded object
        //
        // Testing:
        //   int encode(bsl::streambuf *streambuf, const TYPE& v, options);
        // --------------------------------------------------------------------

        if (verbose) cout
               << "\nTESTING SEQUENCES WITH ATTRIBUTES OF ALL VALUE CATEGORIES"
               << "\n========================================================="
               << endl;

        const u::AssertEncodedValueIsEqualFunction t;

        static const baljsn::EncodingStyle::Value C =
            baljsn::EncodingStyle::e_COMPACT;
        static const baljsn::EncodingStyle::Value P =
            baljsn::EncodingStyle::e_PRETTY;

        static const bool T = true;
        static const bool F = false;

        typedef baljsn::EncoderTestSequenceWithAllCategories Type;

        Type obj;
        obj.choice().makeSelection0();

        //               ENCODE EMPTIES
        //              .--------------
        // LINE STYLE  /  OBJECT EXPECTED RESULT
        // ---- ----- --- ------ --------------------------------------
        t( L_  , C   , F , obj , "{"
                                     "\"charArray\":\"\","
                                     "\"choice\":{\"selection0\":0},"
                                     "\"customizedType\":\"\","
                                     "\"enumeration\":\"A\","
                                     "\"sequence\":{\"attribute\":0},"
                                     "\"simple\":0"
                                 "}"                                   );

        t( L_  , C   , T , obj , "{"
                                     "\"charArray\":\"\","
                                     "\"choice\":{\"selection0\":0},"
                                     "\"customizedType\":\"\","
                                     "\"enumeration\":\"A\","
                                     "\"nullableValue\":null,"
                                     "\"sequence\":{\"attribute\":0},"
                                     "\"simple\":0"
                                 "}"                                   );

        t( L_  , P   , F , obj , "{\n"
                                 "    \"charArray\" : \"\",\n"
                                 "    \"choice\" : {\n"
                                 "        \"selection0\" : 0\n"
                                 "    },\n"
                                 "    \"customizedType\" : \"\",\n"
                                 "    \"enumeration\" : \"A\",\n"
                                 "    \"sequence\" : {\n"
                                 "        \"attribute\" : 0\n"
                                 "    },\n"
                                 "    \"simple\" : 0\n"
                                 "}\n"                                 );

        t( L_  , P   , T , obj , "{\n"
                                 "    \"charArray\" : \"\",\n"
                                 "    \"choice\" : {\n"
                                 "        \"selection0\" : 0\n"
                                 "    },\n"
                                 "    \"customizedType\" : \"\",\n"
                                 "    \"enumeration\" : \"A\",\n"
                                 "    \"nullableValue\" : null,\n"
                                 "    \"sequence\" : {\n"
                                 "        \"attribute\" : 0\n"
                                 "    },\n"
                                 "    \"simple\" : 0\n"
                                 "}\n"                                 );
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING CHOICES WITH SELECTIONS OF ALL VALUE CATEGORIES
        //   This case tests that the encoder emits valid JSON when encoding
        //   a choice selection having any of the 6 'bdlat' value categories
        //   that the encoder supports for selections.
        //
        // Concerns:
        //: 1 Regardless of value category, the encoder emits a valid and
        //:   correct JSON representation of a choice selection.
        //
        // Plan:
        //: 1 Define a choice type that has 6 selections whose types cover
        //:   all 6 'bdlat' value categories that are supported for selections.
        //:
        //: 2 Identify the 2 boolean options that affect relevant aspects of
        //:   the encoding of such choices:
        //:
        //:   1 Whether the encoding style is compact or pretty
        //:
        //:   2 Whether or not to encode empty values
        //:
        //: 3 For each combination of one of the 6 selections, a compact or
        //:   pretty encoding style, and a 'true' or 'false' empty-value
        //:   encoding setting, perform the following:
        //:
        //:   1 Encode the selection using the specified encoding style and
        //:     empty-value encoding setting
        //:
        //:   2 Verify that the emitted JSON is valid
        //:
        //:   3 Verify that the emitted JSON is a correct representation of
        //:     the encoded object
        //
        // Testing:
        //   int encode(bsl::streambuf *streambuf, const TYPE& v, options);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING CHOICES WITH SELECTIONS OF ALL VALUE CATEGORIES"
                 << "\n======================================================="
                 << endl;

        const u::AssertEncodedValueIsEqualFunction t;

        static const baljsn::EncodingStyle::Value C =
            baljsn::EncodingStyle::e_COMPACT;
        static const baljsn::EncodingStyle::Value P =
            baljsn::EncodingStyle::e_PRETTY;

        static const bool T = true;
        static const bool F = false;

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

        //               ENCODE EMPTIES
        //              .--------------
        // LINE STYLE  /  OBJECT EXPECTED RESULT
        // ---- ----- --- ------ ---------------------------------------
        t( L_  , C   , F , obj0 , "{\"charArray\":\"\"}"                );
        t( L_  , C   , F , obj1 , "{\"choice\":{\"selection0\":0}}"     );
        t( L_  , C   , F , obj2 , "{\"customizedType\":\"\"}"           );
        t( L_  , C   , F , obj3 , "{\"enumeration\":\"A\"}"             );
        t( L_  , C   , F , obj4 , "{\"sequence\":{\"attribute\":0}}"    );
        t( L_  , C   , F , obj5 , "{\"simple\":0}"                      );

        t( L_  , C   , T , obj0 , "{\"charArray\":\"\"}"                );
        t( L_  , C   , T , obj1 , "{\"choice\":{\"selection0\":0}}"     );
        t( L_  , C   , T , obj2 , "{\"customizedType\":\"\"}"           );
        t( L_  , C   , T , obj3 , "{\"enumeration\":\"A\"}"             );
        t( L_  , C   , T , obj4 , "{\"sequence\":{\"attribute\":0}}"    );
        t( L_  , C   , T , obj5 , "{\"simple\":0}"                      );

        t( L_  , P   , F , obj0 , "{\n"
                                  "    \"charArray\" : \"\"\n"
                                  "}\n"                                 );
        t( L_  , P   , F , obj1 , "{\n"
                                  "    \"choice\" : {\n"
                                  "        \"selection0\" : 0\n"
                                  "    }\n"
                                  "}\n"                                 );
        t( L_  , P   , F , obj2 , "{\n"
                                  "    \"customizedType\" : \"\"\n"
                                  "}\n"                                 );
        t( L_  , P   , F , obj3 , "{\n"
                                  "    \"enumeration\" : \"A\"\n"
                                  "}\n"                                 );
        t( L_  , P   , F , obj4 , "{\n"
                                  "    \"sequence\" : {\n"
                                  "        \"attribute\" : 0\n"
                                  "    }\n"
                                  "}\n"                                 );
        t( L_  , P   , F , obj5 , "{\n"
                                  "    \"simple\" : 0\n"
                                  "}\n"                                 );

        t( L_  , P   , T , obj0 , "{\n"
                                  "    \"charArray\" : \"\"\n"
                                  "}\n"                                 );
        t( L_  , P   , T , obj1 , "{\n"
                                  "    \"choice\" : {\n"
                                  "        \"selection0\" : 0\n"
                                  "    }\n"
                                  "}\n"                                 );
        t( L_  , P   , T , obj2 , "{\n"
                                  "    \"customizedType\" : \"\"\n"
                                  "}\n"                                 );
        t( L_  , P   , T , obj3 , "{\n"
                                  "    \"enumeration\" : \"A\"\n"
                                  "}\n"                                 );
        t( L_  , P   , T , obj4 , "{\n"
                                  "    \"sequence\" : {\n"
                                  "        \"attribute\" : 0\n"
                                  "    }\n"
                                  "}\n"                                 );
        t( L_  , P   , T , obj5 , "{\n"
                                  "    \"simple\" : 0\n"
                                  "}\n"                                 );
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING FORMATTING OF SEQUENCES WITH EMPTY ELEMENTS
        //   This case tests that the encoder emits valid JSON when encoding
        //   sequences having attributes with untagged-empty-sequence type.
        //
        // Concerns:
        //: 1 The encoder emits no tokens for sequence attributes having
        //:   untagged-empty-sequence type.
        //
        // Plan:
        //: 1 Define 14 types that enumerate the first 14 elements of the
        //:   depth-ordered enumeration of sequences having attributes of
        //:   either integer or untagged-empty-sequence type.
        //:
        //: 2 Identify the 2 boolean options that affect relevant aspects of
        //:   the encoding of sequences with untagged-empty-sequence types:
        //:
        //:   1 Whether the encoding style is compact or pretty
        //:
        //:   2 Whether or not to encode empty values
        //:
        //: 3 For each combination of one of the 14 types, a compact or pretty
        //:   encoding style, and a 'true' or 'false' empty-value encoding
        //:   setting, perform the following:
        //:
        //:   1 Encode a default-initialized object of each type.
        //:
        //:   2 Verify that the emitted JSON is valid.
        //:
        //:   3 Verify that the emitted JSON is the correct representation of
        //:     the encoded object.
        //
        // Testing:
        //   int encode(bsl::streambuf *streambuf, const TYPE& v, options);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING FORMATTING OF SEQUENCES WITH EMPTY ELEMENTS"
                 << "\n==================================================="
                 << endl;

        const u::AssertEncodedValueIsEqualFunction t;

        static const baljsn::EncodingStyle::Value C =
            baljsn::EncodingStyle::e_COMPACT;
        static const baljsn::EncodingStyle::Value P =
            baljsn::EncodingStyle::e_PRETTY;

        static const bool T = true;
        static const bool F = false;

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

        //               ENCODE EMPTIES
        //              .--------------
        // LINE STYLE  /  OBJECT EXPECTED RESULT
        // ---- ----- --- ------ ---------------------------------------
        t( L_  , C   , F , obj0 , "{}"                                  );
        t( L_  , C   , F , obj1 , "{}"                                  );
        t( L_  , C   , F , obj2 , "{\"attribute0\":0}"                  );
        t( L_  , C   , F , obj3 , "{}"                                  );
        t( L_  , C   , F , obj4 , "{\"attribute0\":0}"                  );
        t( L_  , C   , F , obj5 , "{\"attribute0\":0}"                  );
        t( L_  , C   , F , obj6 , "{\"attribute0\":0,\"attribute1\":0}" );
        t( L_  , C   , F , obj7 , "{}"                                  );
        t( L_  , C   , F , obj8 , "{\"attribute0\":0}"                  );
        t( L_  , C   , F , obj9 , "{\"attribute0\":0}"                  );
        t( L_  , C   , F , obj10, "{\"attribute0\":0,\"attribute1\":0}" );
        t( L_  , C   , F , obj11, "{\"attribute0\":0}"                  );
        t( L_  , C   , F , obj12, "{\"attribute0\":0,\"attribute1\":0}" );
        t( L_  , C   , F , obj13, "{\"attribute0\":0,\"attribute1\":0}" );
        t( L_  , C   , F , obj14,
                 "{\"attribute0\":0,\"attribute1\":0,\"attribute2\":0}" );

        t( L_  , C   , T , obj0 , "{}"                                  );
        t( L_  , C   , T , obj1 , "{}"                                  );
        t( L_  , C   , T , obj2 , "{\"attribute0\":0}"                  );
        t( L_  , C   , T , obj3 , "{}"                                  );
        t( L_  , C   , T , obj4 , "{\"attribute0\":0}"                  );
        t( L_  , C   , T , obj5 , "{\"attribute0\":0}"                  );
        t( L_  , C   , T , obj6 , "{\"attribute0\":0,\"attribute1\":0}" );
        t( L_  , C   , T , obj7 , "{}"                                  );
        t( L_  , C   , T , obj8 , "{\"attribute0\":0}"                  );
        t( L_  , C   , T , obj9 , "{\"attribute0\":0}"                  );
        t( L_  , C   , T , obj10, "{\"attribute0\":0,\"attribute1\":0}" );
        t( L_  , C   , T , obj11, "{\"attribute0\":0}"                  );
        t( L_  , C   , T , obj12, "{\"attribute0\":0,\"attribute1\":0}" );
        t( L_  , C   , T , obj13, "{\"attribute0\":0,\"attribute1\":0}" );
        t( L_  , C   , T , obj14,
            "{\"attribute0\":0,\"attribute1\":0,\"attribute2\":0}"      );

        t( L_  , P   , F , obj0 , "{\n\n}\n"                            );
        t( L_  , P   , F , obj1 , "{\n\n}\n"                            );
        t( L_  , P   , F , obj2 , "{\n    \"attribute0\" : 0\n}\n"      );
        t( L_  , P   , F , obj3 , "{\n\n}\n"                            );
        t( L_  , P   , F , obj4 , "{\n    \"attribute0\" : 0\n}\n"      );
        t( L_  , P   , F , obj5 , "{\n    \"attribute0\" : 0\n}\n"      );
        t( L_  , P   , F , obj6 ,
           "{\n    \"attribute0\" : 0,\n    \"attribute1\" : 0\n}\n"    );
        t( L_  , P   , F , obj7 , "{\n\n}\n"                            );
        t( L_  , P   , F , obj8 , "{\n    \"attribute0\" : 0\n}\n"      );
        t( L_  , P   , F , obj9 , "{\n    \"attribute0\" : 0\n}\n"      );
        t( L_  , P   , F , obj10,
           "{\n    \"attribute0\" : 0,\n    \"attribute1\" : 0\n}\n"    );
        t( L_  , P   , F , obj11, "{\n    \"attribute0\" : 0\n}\n"      );
        t( L_  , P   , F , obj12,
           "{\n    \"attribute0\" : 0,\n    \"attribute1\" : 0\n}\n"    );
        t( L_  , P   , F , obj13,
           "{\n    \"attribute0\" : 0,\n    \"attribute1\" : 0\n}\n"    );
        t( L_  , P   , F , obj14,
           "{\n    \"attribute0\" : 0,\n    \"attribute1\" : 0,\n"
           "    \"attribute2\" : 0\n}\n"                                );

        t( L_  , P   , T , obj0 , "{\n\n}\n"                            );
        t( L_  , P   , T , obj1 , "{\n\n}\n"                            );
        t( L_  , P   , T , obj2 , "{\n    \"attribute0\" : 0\n}\n"      );
        t( L_  , P   , T , obj3 , "{\n\n}\n"                            );
        t( L_  , P   , T , obj4 , "{\n    \"attribute0\" : 0\n}\n"      );
        t( L_  , P   , T , obj5 , "{\n    \"attribute0\" : 0\n}\n"      );
        t( L_  , P   , T , obj6 ,
           "{\n    \"attribute0\" : 0,\n    \"attribute1\" : 0\n}\n"    );
        t( L_  , P   , T , obj7 , "{\n\n}\n"                            );
        t( L_  , P   , T , obj8 , "{\n    \"attribute0\" : 0\n}\n"      );
        t( L_  , P   , T , obj9 , "{\n    \"attribute0\" : 0\n}\n"      );
        t( L_  , P   , T , obj10,
           "{\n    \"attribute0\" : 0,\n    \"attribute1\" : 0\n}\n"    );
        t( L_  , P   , T , obj11, "{\n    \"attribute0\" : 0\n}\n"      );
        t( L_  , P   , T , obj12,
           "{\n    \"attribute0\" : 0,\n    \"attribute1\" : 0\n}\n"    );
        t( L_  , P   , T , obj13,
           "{\n    \"attribute0\" : 0,\n    \"attribute1\" : 0\n}\n"    );
        t( L_  , P   , T , obj14,
           "{\n    \"attribute0\" : 0,\n    \"attribute1\" : 0,\n"
           "    \"attribute2\" : 0\n}\n"                                );
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING the log buffer clears on each 'encode' call
        //   This case tests that the log buffer is reset on each call to
        //   'encode'.
        //
        // Concerns:
        //: 1 The string returned from 'loggedMessages' resets each time
        //:   'encode' is invoked, such that the contents of the logged
        //:   messages refer to only the most recent invocation of 'encode'.
        //
        // Plan:
        //: 1 Create 2 objects of different types that 'baljsn::Encoder'
        //:   always successfully encodes.
        //:
        //: 2 Create 2 objects of different types that 'baljsn::Encoder'
        //:   always fails to encode, and which cause a failure as early
        //:   in the encoding process as possible.  (Generally this means
        //:   that each object will implement a 'bdlat' category that
        //:   the encoder does not support).
        //:
        //: 3 Verify that, after performing various sequences of encoding
        //:   operations where some/all succeed/fail, the 'loggedMessages' are
        //:   empty if the last operation succeeds, and contain an expected
        //:   message if and only if the last operation fails.
        //
        // Testing:
        //   int encode(bsl::streambuf *streambuf, const TYPE& v, options);
        // --------------------------------------------------------------------

        if (veryVerbose)
            cout << "\nTESTING the log buffer clears on each 'encode' call"
                 << "\n==================================================="
                 << endl;

        s_baltst::Address mS1;
            // 'mS1' is a modifiable object for which 'baljsn::Encoder::encode'
            // will succeed.
        mS1.street() = "1st";
        mS1.city()   = "New York";
        mS1.state()  = "New York";

        const s_baltst::Address& S1 = mS1;
            // 'S1' is a non-modifiable reference to 'mS1'.

        s_baltst::Employee mS2;
            // 'mS2' is a modifiable object for which 'baljsn::Encoder::encode'
            // will succeed.
        mS2.name() = "John Doe";
        mS2.homeAddress() = S1;
        mS2.age() = 50;

        const s_baltst::Employee& S2 = mS2;
            // 'S2' is a non-modifiable reference to 'mS2'.

        u::Enumeration0 mF1;
            // 'mF1' is a modifiable object for which 'baljsn::Encoder::encode'
            // will fail very early in the encoding process.

        const u::Enumeration0& F1 = mF1;
            // 'F1' is a non-modifiable reference to 'mF1'.

        bdlb::NullableValue<s_baltst::Address> mF2;
            // 'mF2' is a modifiable object for which 'baljsn::Encoder::encode'
            // will fail very early in the encoding process.

        const bdlb::NullableValue<s_baltst::Address>& F2 = mF2;
            // 'F2' is a non-modifiable reference to 'mF2'.

        enum Instruction {
            // This enumeration provides a set of integer constants that
            // indicate individual operations that may be performed by the
            // testing apparatus.

            NOOP = 0, // indicates to do nothing (no operation)
            ES1,      // indicates to encode 'S1'
            ES2,      // indicates to encode 'S2'
            EF1,      // indicates to encode 'F1'
            EF2       // indicates to encode 'F2'
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

            failure = false, // indicates that at least one encode op fails
            success = true   // indicates that all encode ops succeed
        };


        static const char SMsg1[] = "";
            // 'SMsg1' is a string that is equivalent to the 'loggedMessages'
            // of a 'baljsn::Encoder' after a successful encoding operation.


        static const char FMsg1[] =
            "Encoded object must be a Sequence, Choice, or Array type.\n";
            // 'FMsg1' is a string that is equivalent to the 'loggedMessages'
            // of a 'baljsn::Encoder' after an encoding operation that fails
            // due to the type of the target object having an unsupported
            // 'bdlat' category.

        static const struct {
            int         d_line;
                // line number

            Instruction d_instructions[k_MAX_INSTRUCTIONS];
                // instructions for test apparatus

            bool        d_encodeSuccessStatus;
                // whether all operations succeed

            const char *d_loggedMessages;
                // messages from final operation

        } DATA[] = {
            //LINE    INSTRUCTIONS     STATUS  LOGGED MESSAGES
            //---- ------------------ -------- ---------------
            {   L_, {               }, success,         SMsg1 },
                // Verify that the 'loggedMessages' are empty if no encoding
                // operations are performed.

            {   L_, { ES1           }, success,         SMsg1 },
            {   L_, { ES2           }, success,         SMsg1 },
                // Verify that the 'loggedMessages' are empty if one
                // encoding operation is performed, and that operation
                // succeeds.

            {   L_, { EF1           }, failure,         FMsg1 },
            {   L_, { EF2           }, failure,         FMsg1 },
                // Verify that the 'loggedMessages' have an expected message
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
                // Verify that the 'loggedMessages' have an expected message
                // when, after performing 2 encoding operations, the second
                // operation fails, and otherwise are empty.

            {   L_, { ES1, ES1, ES1 }, success,         SMsg1 },
            {   L_, { ES1, ES1, EF1 }, failure,         FMsg1 },
            {   L_, { ES1, EF1, ES1 }, failure,         SMsg1 },
            {   L_, { EF1, ES1, ES1 }, failure,         SMsg1 }
                // Verify that the 'loggedMessages' have an expected message
                // when the last encoding operation in a sequence of encoding
                // operations fails, and otherwise are empty.
        };

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i != NUM_DATA; ++i) {
            const int          LINE          = DATA[i].d_line;
            const Instruction *INSTRUCTIONS  = DATA[i].d_instructions;
            const bool ENCODE_SUCCESS_STATUS = DATA[i].d_encodeSuccessStatus;
            const bsl::string_view& LOGGED_MESSAGES = DATA[i].d_loggedMessages;

            typedef baljsn::Encoder Obj;
            Obj                     mX;

            int encodeStatus = 0;

            for (const Instruction *instructionPtr = &INSTRUCTIONS[0];
                 instructionPtr != &INSTRUCTIONS[0] + k_MAX_INSTRUCTIONS;
                 ++instructionPtr) {
                switch (*instructionPtr) {
                  case NOOP: {
                      // do nothing
                  } break;
                  case ES1: {
                    bdlsb::MemOutStreamBuf streamBuf;
                    encodeStatus |=
                        mX.encode(&streamBuf, S1, baljsn::EncoderOptions());
                  } break;
                  case ES2: {
                    bdlsb::MemOutStreamBuf streamBuf;
                    encodeStatus |=
                        mX.encode(&streamBuf, S2, baljsn::EncoderOptions());
                  } break;
                  case EF1: {
                    bdlsb::MemOutStreamBuf streamBuf;
                    encodeStatus |=
                        mX.encode(&streamBuf, F1, baljsn::EncoderOptions());
                  } break;
                  case EF2: {
                    bdlsb::MemOutStreamBuf streamBuf;
                    encodeStatus |=
                        mX.encode(&streamBuf, F2, baljsn::EncoderOptions());
                  } break;
                }
            }

            ASSERTV(LINE,
                    encodeStatus,
                    ENCODE_SUCCESS_STATUS ? encodeStatus == 0
                                          : encodeStatus != 0);

            ASSERTV(LINE,
                    LOGGED_MESSAGES,
                    mX.loggedMessages(),
                    LOGGED_MESSAGES == mX.loggedMessages());
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'encodeNullElements' option
        //
        // Concerns:
        //: 1 Encoder outputs "null" for null elements only if the
        //:   'encodeNullElements' option is set.
        //:
        //: 2 The encoder correctly generates the output in pretty or compact
        //:   style when used in consonance with this option.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a table with
        //:   a set of distinct rows of encoding style, initial indent level,
        //:   spaces per level, input XML, 'encodeNullElements' option value,
        //:   and the expected JSON corresponding to that object.
        //:
        //: 2 For each row in the tables of P-1:
        //:
        //:   1 Populate a 'balb::Sequence3' object based on the input XML
        //:     using the XML decoder.
        //:
        //:   2 Create a 'baljsn::Encoder' object.
        //:
        //:   3 Create a 'ostringstream' object and encode the object
        //:     using the specified style, formatting options, and
        //:     'encodeNullElements' option.
        //:
        //:   4 Compare the generated JSON with the expected JSON.
        //
        // Testing:
        //   int encode(bsl::streambuf *streamBuf, const TYPE& v, options);
        //   int encode(bsl::ostream& stream, const TYPE& v, options);
        //   int encode(bsl::streambuf *streamBuf, const TYPE& v, &options);
        //   int encode(bsl::ostream& stream, const TYPE& v, &options);
        // --------------------------------------------------------------------

        typedef Options::EncodingStyle Style;
        static const struct {
            int         d_line;        // source line number
            Style       d_style;       // encoding style
            int         d_indent;      // initial indent level
            int         d_spl;         // spaces per level
            const char *d_xmlText_p;   // xml text
            bool        d_encodeNulls; // if nulls should be encoded
            const char *d_jsonText_p;  // json text
        } DATA[] = {
            {
                L_,
                Options::e_PRETTY,
                0,
                2,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "</element1>\n",
                false,
                // JSON Text
                "{\n"
                "  \"element1\" : [\n"
                "    \"LONDON\"\n"
                "  ],\n"
                "  \"element2\" : [\n"
                "    \"arbitrary string value\"\n"
                "  ]\n"
                "}\n"
            },
            {
                L_,
                Options::e_PRETTY,
                0,
                2,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "</element1>\n",
                true,
                // JSON Text
                "{\n"
                "  \"element1\" : [\n"
                "    \"LONDON\"\n"
                "  ],\n"
                "  \"element2\" : [\n"
                "    \"arbitrary string value\"\n"
                "  ],\n"
                "  \"element3\" : null,\n"
                "  \"element4\" : null,\n"
                "  \"element5\" : null\n"
                "}\n"
            },
            {
                L_,
                Options::e_COMPACT,
                0,
                0,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "</element1>\n",
                false,
                // JSON Text
                "{\"element1\":[\"LONDON\"],"
                "\"element2\":[\"arbitrary string value\"]}"
            },
            {
                L_,
                Options::e_COMPACT,
                0,
                0,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "</element1>\n",
                true,
                // JSON Text
                "{\"element1\":[\"LONDON\"],"
                "\"element2\":[\"arbitrary string value\"],"
                "\"element3\":null,\"element4\":null,\"element5\":null}"
            },
            {
                L_,
                Options::e_PRETTY,
                0,
                2,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "  <element6/>\n"
                "  <element6/>\n"
                "</element1>\n",
                false,
                // JSON Text
                "{\n"
                "  \"element1\" : [\n"
                "    \"LONDON\"\n"
                "  ],\n"
                "  \"element2\" : [\n"
                "    \"arbitrary string value\"\n"
                "  ],\n"
                "  \"element6\" : [\n"
                "    null,\n"
                "    null\n"
                "  ]\n"
                "}\n"
            },
            {
                L_,
                Options::e_PRETTY,
                0,
                2,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "  <element6/>\n"
                "  <element6/>\n"
                "</element1>\n",
                true,
                // JSON Text
                "{\n"
                "  \"element1\" : [\n"
                "    \"LONDON\"\n"
                "  ],\n"
                "  \"element2\" : [\n"
                "    \"arbitrary string value\"\n"
                "  ],\n"
                "  \"element3\" : null,\n"
                "  \"element4\" : null,\n"
                "  \"element5\" : null,\n"
                "  \"element6\" : [\n"
                "    null,\n"
                "    null\n"
                "  ]\n"
                "}\n"
            },
            {
                L_,
                Options::e_COMPACT,
                0,
                0,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "  <element6/>\n"
                "  <element6/>\n"
                "</element1>\n",
                false,
                // JSON Text
                "{\"element1\":[\"LONDON\"],"
                "\"element2\":[\"arbitrary string value\"],"
                "\"element6\":[null,null]}"
            },
            {
                L_,
                Options::e_COMPACT,
                0,
                0,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "  <element6/>\n"
                "  <element6/>\n"
                "</element1>\n",
                true,
                // JSON Text
                "{\"element1\":[\"LONDON\"],"
                "\"element2\":[\"arbitrary string value\"],"
                "\"element3\":null,\"element4\":null,\"element5\":null,"
                "\"element6\":[null,null]}"
            },
            {
                L_,
                Options::e_PRETTY,
                0,
                2,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "  <element3>true</element3>\n"
                "  <element6/>\n"
                "  <element6/>\n"
                "</element1>\n",
                false,
                // JSON Text
                "{\n"
                "  \"element1\" : [\n"
                "    \"LONDON\"\n"
                "  ],\n"
                "  \"element2\" : [\n"
                "    \"arbitrary string value\"\n"
                "  ],\n"
                "  \"element3\" : true,\n"
                "  \"element6\" : [\n"
                "    null,\n"
                "    null\n"
                "  ]\n"
                "}\n"
            },
            {
                L_,
                Options::e_PRETTY,
                0,
                2,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "  <element3>true</element3>\n"
                "  <element6/>\n"
                "  <element6/>\n"
                "</element1>\n",
                true,
                // JSON Text
                "{\n"
                "  \"element1\" : [\n"
                "    \"LONDON\"\n"
                "  ],\n"
                "  \"element2\" : [\n"
                "    \"arbitrary string value\"\n"
                "  ],\n"
                "  \"element3\" : true,\n"
                "  \"element4\" : null,\n"
                "  \"element5\" : null,\n"
                "  \"element6\" : [\n"
                "    null,\n"
                "    null\n"
                "  ]\n"
                "}\n"
            },
            {
                L_,
                Options::e_COMPACT,
                0,
                0,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "  <element3>true</element3>\n"
                "  <element6/>\n"
                "  <element6/>\n"
                "</element1>\n",
                false,
                // JSON Text
                "{\"element1\":[\"LONDON\"],"
                "\"element2\":[\"arbitrary string value\"],"
                "\"element3\":true,"
                "\"element6\":[null,null]}"
            },
            {
                L_,
                Options::e_COMPACT,
                0,
                0,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "  <element3>true</element3>\n"
                "  <element6/>\n"
                "  <element6/>\n"
                "</element1>\n",
                true,
                // JSON Text
                "{\"element1\":[\"LONDON\"],"
                "\"element2\":[\"arbitrary string value\"],"
                "\"element3\":true,\"element4\":null,\"element5\":null,"
                "\"element6\":[null,null]}"
            },
            {
                L_,
                Options::e_PRETTY,
                0,
                2,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "  <element3>true</element3>\n"
                "  <element4>arbitrary string value</element4>\n"
                "  <element6/>\n"
                "  <element6/>\n"
                "</element1>\n",
                false,
                // JSON Text
                "{\n"
                "  \"element1\" : [\n"
                "    \"LONDON\"\n"
                "  ],\n"
                "  \"element2\" : [\n"
                "    \"arbitrary string value\"\n"
                "  ],\n"
                "  \"element3\" : true,\n"
                "  \"element4\" : \"arbitrary string value\",\n"
                "  \"element6\" : [\n"
                "    null,\n"
                "    null\n"
                "  ]\n"
                "}\n"
            },
            {
                L_,
                Options::e_PRETTY,
                0,
                2,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "  <element3>true</element3>\n"
                "  <element4>arbitrary string value</element4>\n"
                "  <element6/>\n"
                "  <element6/>\n"
                "</element1>\n",
                true,
                // JSON Text
                "{\n"
                "  \"element1\" : [\n"
                "    \"LONDON\"\n"
                "  ],\n"
                "  \"element2\" : [\n"
                "    \"arbitrary string value\"\n"
                "  ],\n"
                "  \"element3\" : true,\n"
                "  \"element4\" : \"arbitrary string value\",\n"
                "  \"element5\" : null,\n"
                "  \"element6\" : [\n"
                "    null,\n"
                "    null\n"
                "  ]\n"
                "}\n"
            },
            {
                L_,
                Options::e_COMPACT,
                0,
                0,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "  <element3>true</element3>\n"
                "  <element4>arbitrary string value</element4>\n"
                "  <element6/>\n"
                "  <element6/>\n"
                "</element1>\n",
                false,
                // JSON Text
                "{\"element1\":[\"LONDON\"],"
                "\"element2\":[\"arbitrary string value\"],"
                "\"element3\":true,\"element4\":\"arbitrary string value\","
                "\"element6\":[null,null]}"
            },
            {
                L_,
                Options::e_COMPACT,
                0,
                0,
                // XML Text
                "<element1>\n"
                "  <element1>LONDON</element1>\n"
                "  <element2>arbitrary string value</element2>\n"
                "  <element3>true</element3>\n"
                "  <element4>arbitrary string value</element4>\n"
                "  <element6/>\n"
                "  <element6/>\n"
                "</element1>\n",
                true,
                // JSON Text
                "{\"element1\":[\"LONDON\"],"
                "\"element2\":[\"arbitrary string value\"],"
                "\"element3\":true,\"element4\":\"arbitrary string value\","
                "\"element5\":null,"
                "\"element6\":[null,null]}"
            }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const Style       STYLE  = DATA[ti].d_style;
            const int         INDENT = DATA[ti].d_indent;
            const int         SPL    = DATA[ti].d_spl;
            const bsl::string XML    = DATA[ti].d_xmlText_p;
            const int         ENE    = DATA[ti].d_encodeNulls;
            const bsl::string EXP    = DATA[ti].d_jsonText_p;

            balb::Sequence3 object;
            ASSERT(0 == u::populateTestObject(&object, XML));

            baljsn::Encoder     encoder;

            baljsn::EncoderOptions options;
            options.setEncodingStyle(STYLE);
            options.setInitialIndentLevel(INDENT);
            options.setSpacesPerLevel(SPL);
            options.setEncodeNullElements(ENE);

            const Options& mO = options;

            {
                bsl::ostringstream oss;
                ASSERTV(0 == encoder.encode(oss, object, mO));
                ASSERTV(LINE, oss.str(), EXP, oss.str() == EXP);
            }
            {
                bsl::ostringstream oss;
                ASSERTV(0 == encoder.encode(oss, object, &mO));
                ASSERTV(LINE, oss.str(), EXP, oss.str() == EXP);
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // ENCODING COMPLEX TEST MESSAGES
        //
        // Concerns:
        //: 1 Encoder produces the expected results for a variety of complex
        //:   'bas-codegen' generated objects.
        //:
        //: 2 The encoder correctly generates the output in pretty or compact
        //:   style based on the passed in encoder options.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify three tables: one with
        //:   a set of distinct rows of XML string value corresponding to a
        //:   's_baltst::FeatureTestMessage' object, the second with the
        //:   expected JSON corresponding to that object's value in pretty
        //:   format, and third with the expected JSON in compact format.
        //:
        //: 2 For each row in the tables of P-1:
        //:
        //:   1 Construct a 's_baltst::FeatureTestMessage' object from the XML
        //:     string using the XML decoder.
        //:
        //:   2 Create a 'baljsn::Encoder' object.
        //:
        //:   3 Create a 'bdlsb::MemOutStreamBuf' object and encode the
        //:     's_baltst::FeatureTestMessage' object specifying the pretty
        //:     print option.
        //:
        //:   4 Compare the generated JSON with the expected JSON in the
        //:     pretty format.
        //:
        //:   5 Repeat steps 1 - 4 in the compact format.
        //
        // Testing:
        //   int encode(bsl::streambuf *streamBuf, const TYPE& v, options);
        //   int encode(bsl::ostream& stream, const TYPE& v, options);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING COMPLEX TEST MESSAGES"
                          << "\n==============================" << endl;

        bsl::vector<s_baltst::FeatureTestMessage> testObjects;
        u::constructFeatureTestMessage(&testObjects);

        // PRETTY PRINTING
        typedef s_baltst::FeatureTestMessageUtil MessageUtil;

        for (int i = 0; i != MessageUtil::k_NUM_MESSAGES; ++i) {
            const bsl::string EXP = MessageUtil::s_PRETTY_JSON_MESSAGES[i];

            baljsn::Encoder encoder;

            baljsn::EncoderOptions options;
            options.setEncodingStyle(baljsn::EncoderOptions::e_PRETTY);
            options.setInitialIndentLevel(0);
            options.setSpacesPerLevel(2);

            {
                bdlsb::MemOutStreamBuf osb;
                ASSERTV(0 == encoder.encode(&osb, testObjects[i], options));

                bsl::string ACTUAL(osb.data(), osb.length());
                ASSERTV(i, ACTUAL, EXP, ACTUAL == EXP);
                if (ACTUAL != EXP) {
                    u::printStringDifferences(ACTUAL, EXP);
                }
            }

            {
                bdlsb::MemOutStreamBuf osb;
                bsl::ostream          oss(&osb);
                ASSERTV(0 == encoder.encode(oss, testObjects[i], options));

                bsl::string ACTUAL(osb.data(), osb.length());
                ASSERTV(i, ACTUAL, EXP, ACTUAL == EXP);
                if (ACTUAL != EXP) {
                    u::printStringDifferences(ACTUAL, EXP);
                }
            }
        }

        // COMPACT PRINTING
        for (int i = 0; i < MessageUtil::k_NUM_MESSAGES; ++i) {
            const bsl::string EXP = MessageUtil::s_COMPACT_JSON_MESSAGES[i];

            baljsn::Encoder        encoder;
            baljsn::EncoderOptions options;

            {
                bdlsb::MemOutStreamBuf osb;
                ASSERTV(0 == encoder.encode(&osb, testObjects[i], options));

                bsl::string ACTUAL(osb.data(), osb.length());
                ASSERTV(i, ACTUAL, EXP, ACTUAL == EXP);
                if (ACTUAL != EXP) {
                    u::printStringDifferences(ACTUAL, EXP);
                }
            }

            {
                bdlsb::MemOutStreamBuf osb;
                bsl::ostream          oss(&osb);
                ASSERTV(0 == encoder.encode(oss, testObjects[i], options));

                bsl::string ACTUAL(osb.data(), osb.length());
                ASSERTV(i, testObjects[i], ACTUAL, EXP, ACTUAL == EXP);
                if (ACTUAL != EXP) {
                    u::printStringDifferences(ACTUAL, EXP);
                }
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // Encode test
        //
        // Concerns:
        //: 1 'encode' only works for Choice or Sequence type.
        //:
        //: 2 'encode' a bad stream returns an error.
        //
        // Testing:
        //  int encode(const TYPE& value);
        // --------------------------------------------------------------------

        {
            Obj encoder;
            bsl::ostringstream oss;
            ASSERTV("" == encoder.loggedMessages());

            ASSERTV(oss.good());
            ASSERTV(0 != encoder.encode(oss, ' '));
            ASSERTV("" != encoder.loggedMessages());
            oss.clear();
            oss.str("");

            ASSERTV(oss.good());
            ASSERTV(0 != encoder.encode(oss, 0));
            ASSERTV("" != encoder.loggedMessages());
            oss.clear();
            oss.str("");

            ASSERTV(oss.good());
            ASSERTV(0 != encoder.encode(oss, balb::Enumerated::Value()));
            ASSERTV("" != encoder.loggedMessages());

            // Encoding with a bad stream.
            oss.str("");
            ASSERTV(!oss.good());
            ASSERTV(0 != encoder.encode(oss, balb::VoidSequence()));
            ASSERTV("" != encoder.loggedMessages());

            oss.clear();
            oss.str("");
            ASSERTV(oss.good());
            ASSERTV(0 == encoder.encode(oss, balb::VoidSequence()));
            ASSERTV("" == encoder.loggedMessages());
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // ENCODING SEQUENCES
        //
        // Concerns:
        //: 1 Sequence objects are encoded as name-value pairs.
        //:
        //: 2 Null elements are not encoded.
        //:
        //: 3 Empty sequence will be encoded as an object without any member.
        //:
        //: 4 Error occurred when encoding an element of a sequence causes
        //:   error in encoding a sequence.
        //
        // Plan:
        //: 1 Encode a void sequence and verify the output is an empty JSON
        //:   object.
        //:
        //: 2 Encode a sequence with some null values an verify only the
        //:   non-null values are encoded.
        //:
        //: 3 Encoded a sequence with an unselected Choice and verify an error
        //:   is returned.
        //
        // Testing:
        //   int encode(bsl::ostream& stream, const TYPE& v, options);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING SEQUENCES"
                          << "\n==================" << endl;

        if (verbose) cout << "Encode empty sequence." << endl;
        {
            const balb::VoidSequence X;

            bsl::ostringstream oss;
            ASSERTV(0 == ImplUtil::encode(&oss, X));

            bsl::string result = oss.str();
            ASSERTV(result, result == "{}");
        }

        {
            balb::Sequence2 mX; const balb::Sequence2& X = mX;
            mX.element1() = balb::CustomString("Hello");
            mX.element2() = 42;
            mX.element3() = bdlt::DatetimeTz(
                                 bdlt::Datetime(2012, 12, 31, 12, 59, 59, 999),
                                 -720);

            {
                bsl::ostringstream oss;
                ASSERTV(0 == ImplUtil::encode(&oss, X));

                const char *EXP =
                    "{"
                        "\"element1\":\"Hello\","
                        "\"element2\":42,"
                        "\"element3\":\"2012-12-31T12:59:59.999-12:00\""
                    "}";
                bsl::string result = oss.str();
                ASSERTV(result, EXP, result == EXP);
            }

            mX.element4().makeValue();
            mX.element5().makeValue(3.14);
            {
                // 'element4' is an unselected Choice.  Ensure encode sequence
                // propagate errors.

                bsl::ostringstream lss;
                bsl::ostringstream oss;
                ASSERTV(0 != ImplUtil::encode(&lss, &oss, X));
                ASSERTV("" != lss.str());
            }

            mX.element4().value().makeSelection1(99);
            {
                bsl::ostringstream oss;
                ASSERTV(0 == ImplUtil::encode(&oss, X));
                const char *EXP =
                    "{"
                        "\"element1\":\"Hello\","
                        "\"element2\":42,"
                        "\"element3\":\"2012-12-31T12:59:59.999-12:00\","
                        "\"element4\":{\"selection1\":99},"
                        "\"element5\":3.14"
                    "}";
                bsl::string result = oss.str();
                ASSERTV(result, EXP, result == EXP);
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // ENCODING CHOICES
        //
        // Concerns:
        //: 1 Encoding a Choice object results in a JSON object with one
        //:   name-value pair, where the name is the selection name and value
        //:   is the selected value.
        //:
        //: 2 Unselected Choice returns an error.
        //:
        //: 3 Error when encoding the selection is propagated.
        //:
        //: 4 Array Choice selections are encoded correctly.
        //
        // Plan:
        //: 1 Use a brute force approach:  (C-1..3)
        //:
        //:   1 Encode an unselected Choice object and verify it returns an
        //:     error.  (C-2)
        //:
        //:   2 Encode a selected Choice and verify it returns a name-value
        //:     pair.  (C-1)
        //:
        //:   3 Encode a selected Choice, where the selection is an unselected
        //:     Choice and verify it returns an error.  (C-3)
        //:
        //: 2 Use the table-driven approach to verify that array Choice
        //:   selections encode as expected.  Test empty and non-empty arrays
        //:   with both settings for the 'encodeEmptyArrays' encoder option.
        //:   (C-4)
        //
        // Testing:
        //   int encode(bsl::ostream& stream, const TYPE& v, options);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING CHOICES"
                          << "\n================" << endl;

        if (verbose) cout << "Encode Choice" << endl;
        {
            balb::Choice2 mX; const balb::Choice2& X = mX;
            {
                // Test that it fails without selection.

                bsl::ostringstream lss;
                bsl::ostringstream oss;
                ASSERTV(0 != ImplUtil::encode(&lss, &oss, X));
                ASSERTV("" != lss.str());
            }
            {
                mX.makeSelection1(true);

                bsl::ostringstream oss;
                ASSERTV(0 == ImplUtil::encode(&oss, X));

                bsl::string result = oss.str();
                ASSERTV(result, result == "{\"selection1\":true}");
            }
            {
                mX.makeSelection2("A quick brown fox");

                bsl::ostringstream oss;
                ASSERTV(0 == ImplUtil::encode(&oss, X));

                bsl::string result = oss.str();
                ASSERTV(result,
                        result == "{\"selection2\":\"A quick brown fox\"}");
            }
            {
                mX.makeSelection3();

                bsl::ostringstream lss;
                bsl::ostringstream oss;
                ASSERTV(0 != ImplUtil::encode(&lss, &oss, X));
                ASSERTV("" != lss.str());
            }
            {
                mX.selection3().makeSelection1(42);

                bsl::ostringstream oss;
                ASSERTV(0 == ImplUtil::encode(&oss, X));

                bsl::string result = oss.str();
                ASSERTV(result,
                        result == "{\"selection3\":{\"selection1\":42}}");
            }
        }

        if (verbose) cout << "Encode Choice w/Array Selection" << endl;
        {
            const struct {
                int         d_line;
                const char *d_input_p;            // input for array selection
                bool        d_encodeEmptyArrays;  // encoder option
                const char *d_result_p;           // expected result
            } DATA[] = {
                //LINE  INPUT   "EEA"   RESULT
                //----  -----   -----   ------
                { L_,       0,  false,  "{\"selection1\":[]}"                },
                { L_,       0,  true,   "{\"selection1\":[]}"                },
                { L_,   "XyZ",  false,  "{\"selection1\":[\"XyZ\"]}"         },
                { L_,   "XyZ",  true,   "{\"selection1\":[\"XyZ\"]}"         },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_line;
                const char *INPUT = DATA[ti].d_input_p;
                const bool  EEA   = DATA[ti].d_encodeEmptyArrays;
                const char *EXP   = DATA[ti].d_result_p;

                bsl::vector<bsl::string> ARRAY;
                if (INPUT) {  // test with non-empty array selection
                    ARRAY.push_back(INPUT);
                }

                Options options;
                options.setEncodeEmptyArrays(EEA);

                balb::Choice4 mX; const balb::Choice4& X = mX;
                mX.makeSelection1(ARRAY);

                if (veryVerbose) { P_(X); P(EXP); }

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, X, options));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }
        }
        {
            const struct {
                int         d_line;
                const char *d_input_p;            // input for array selection
                bool        d_encodeEmptyArrays;  // encoder option
                const char *d_result_p;           // expected result
            } DATA[] = {
                //LINE  INPUT   "EEA"   RESULT
                //----  -----   -----   ------
                { L_,       0, false, "{\"selection1\":{\"selection1\":[]}}" },
                { L_,       0,  true, "{\"selection1\":{\"selection1\":[]}}" },
                { L_,   "XyZ",  false,
                               "{\"selection1\":{\"selection1\":[\"XyZ\"]}}" },
                { L_,   "XyZ",  true,
                               "{\"selection1\":{\"selection1\":[\"XyZ\"]}}" },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_line;
                const char *INPUT = DATA[ti].d_input_p;
                const bool  EEA   = DATA[ti].d_encodeEmptyArrays;
                const char *EXP   = DATA[ti].d_result_p;

                bsl::vector<bsl::string> ARRAY;
                if (INPUT) {  // test with non-empty array selection
                    ARRAY.push_back(INPUT);
                }

                Options options;
                options.setEncodeEmptyArrays(EEA);

                balb::Choice5 mX; const balb::Choice5& X = mX;
                mX.makeSelection1().makeSelection1(ARRAY);

                if (veryVerbose) { P_(X); P(EXP); }

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, X, options));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // ENCODING ARRAYS
        //
        // Concerns:
        //: 1 'bsl::vector<char>' is encoded into as a JSON string type in
        //:   base64 encoding.
        //:
        //: 2 Empty 'bsl::vector<char>' results in an empty string.
        //:
        //: 3 Empty arrays of other types by will not be encoded by default.
        //:
        //: 4 Empty vectors with the 'encodeEmptyArrays' option set to 'true'
        //:   result in an '[' and ']' being output.
        //:
        //: 5 The formatting is appropriately output for all vectors including
        //:   empty vectors when used in consonance with the
        //:   'encodeEmptyArrays' option.
        //
        // Plan:
        //: 1 Use a table-driven approach:
        //:
        //:   1 Create a set of values with various length of 'vector<char>'.
        //:
        //:   2 Encode each values and verify the result is in base64 format.
        //:
        //: 2 Repeat step one with 'vector<int>' instead.
        //:
        //: 3 Ensure that the output is as expected in all cases.
        //
        // Testing:
        //   int encode(bsl::ostream& s, const bsl::vector<TYPE>& v, options);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING ARRAYS"
                          << "\n===============" << endl;

        if (verbose) cout << "Encode 'vector<char>'" << endl;
        {
            const struct {
                int         d_line;
                const char *d_input_p;
                int         d_inputLength;
                const char *d_result_p;
            } DATA[] = {

            //LINE  INPUT  LEN  RESULT
            //----  -----  ---  ------

            { L_,   "",     0,   "\"\""  },
            { L_,   "\x00", 1,   "\"AA==\""  },
            { L_,   "\x01", 1,   "\"AQ==\""  },
            { L_,   "\xFF", 1,   "\"\\/w==\""  },
            { L_,   "\x00\x00\x00\x00\x00\x00\x00\x00\00", 9,
                                                           "\"AAAAAAAAAAAA\"" }

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char *const INPUT  = DATA[ti].d_input_p;
                const int         LENGTH = DATA[ti].d_inputLength;
                const char *const EXP    = DATA[ti].d_result_p;
                const bsl::vector<char> VALUE(INPUT, INPUT + LENGTH);

                {
                    bsl::ostringstream oss;
                    ASSERTV(LINE, 0 == ImplUtil::encode(&oss, VALUE));

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                {
                    Options options;
                    options.setEncodeEmptyArrays(true);

                    bsl::ostringstream oss;
                    ASSERTV(LINE, 0 == ImplUtil::encode(&oss, VALUE, options));

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }
            }
        }

        if (verbose) cout << "Encode 'vector<int>'" << endl;
        {
            typedef Options::EncodingStyle Style;
            Style P = Options::e_PRETTY;
            Style C = Options::e_COMPACT;

#define NL "\n"

            const struct {
                int            d_line;
                const char    *d_input_p;
                bool           d_encodeEmptyArrays;
                Style          d_encodingStyle;
                int            d_initialIndentLevel;
                int            d_spacesPerLevel;
                const char    *d_result_p;
            } DATA[] = {

            //LINE  INPUT  "EEA"   Style  INDENT    "SPL"   RESULT
            //----  -----  -----   -----  ------    -----   ------

             { L_,    "",  false,     C,      0,      0,    ""           },
             { L_,    "",  false,     P,      0,      0,    ""           },
             { L_,    "",  false,     C,      1,      2,    ""           },
             { L_,    "",  false,     P,      1,      2,    ""           },

             { L_,    "",  true,      C,      0,      0,    "[]"         },
             { L_,    "",  true,      P,      0,      0,    "[]"         },
             { L_,    "",  true,      C,      1,      2,    "[]"         },
             { L_,    "",  true,      P,      1,      2,    "  []"         },

             { L_,    "2", false,     C,      0,      0,    "[2]"        },

             { L_,    "2", false,     P,      0,      0,    "["      NL
                                                            "2"      NL
                                                            "]"          },

             { L_,    "2", false,     C,      1,      2,    "[2]"        },

             { L_,    "2", false,     P,      1,      2,    "  ["    NL
                                                            "    2"  NL
                                                            "  ]"        },

             { L_,    "2", true,      C,      0,      0,    "[2]"        },

             { L_,    "2", true,      P,      0,      0,    "["      NL
                                                            "2"      NL
                                                            "]"          },

             { L_,    "2", true,      C,      1,      2,    "[2]"        },

             { L_,    "2", true,      P,      1,      2,    "  ["    NL
                                                            "    2"  NL
                                                            "  ]"        },

             { L_,  "012", false,     C,      0,      0,    "[0,1,2]"    },

             { L_,  "012", false,     P,      0,      0,    "["      NL
                                                            "0,"     NL
                                                            "1,"     NL
                                                            "2"      NL
                                                            "]"          },

             { L_,  "012", false,     C,      1,      2,    "[0,1,2]"    },

             { L_,  "012", false,     P,      1,      2,    "  ["    NL
                                                            "    0," NL
                                                            "    1," NL
                                                            "    2"  NL
                                                            "  ]"        },

             { L_,  "012", true,      C,      0,      0,    "[0,1,2]"    },

             { L_,  "012", true,      P,      0,      0,    "["      NL
                                                            "0,"     NL
                                                            "1,"     NL
                                                            "2"      NL
                                                            "]"          },

             { L_,  "012", true,      C,      1,      2,    "[0,1,2]"    },

             { L_,  "012", true,      P,      1,      2,    "  ["    NL
                                                            "    0," NL
                                                            "    1," NL
                                                            "    2"  NL
                                                            "  ]"        },

            };
#undef NL
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const bsl::string INPUT  = DATA[ti].d_input_p;
                const bool        EEA    = DATA[ti].d_encodeEmptyArrays;
                const Style       STYLE  = DATA[ti].d_encodingStyle;
                const int         INDENT = DATA[ti].d_initialIndentLevel;
                const int         SPL    = DATA[ti].d_spacesPerLevel;
                const bsl::string EXP    = DATA[ti].d_result_p;

                bsl::vector<int> value;
                for (bsl::size_t i = 0; i < INPUT.size(); ++i) {
                    value.push_back(INPUT[i] - '0');
                }

                Options options;
                options.setEncodingStyle(STYLE);
                options.setInitialIndentLevel(INDENT);
                options.setSpacesPerLevel(SPL);
                options.setEncodeEmptyArrays(EEA);

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, value, options));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }
        }

        if (verbose) cout << "Encode arrays in a sequence" << endl;
        {
            static const struct {
                int         d_line;              // source line number
                const char *d_xmlText_p;         // xml text
                bool        d_encodeEmptyArrays; // encode empty arrays flag
                const char *d_jsonText_p;        // json text
            } DATA[] = {
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "</element1>\n",
                    false,
                    // JSON Text
                    "{\n"
                    "\n"
                    "}\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "</element1>\n",
                    true,
                    // JSON Text
                    "{\n"
                    "  \"element1\" : [],\n"
                    "  \"element2\" : [],\n"
                    "  \"element6\" : []\n"
                    "}\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element1>LONDON</element1>\n"
                    "</element1>\n",
                    false,
                    // JSON Text
                    "{\n"
                    "  \"element1\" : [\n"
                    "    \"LONDON\"\n"
                    "  ]\n"
                    "}\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element1>LONDON</element1>\n"
                    "</element1>\n",
                    true,
                    // JSON Text
                    "{\n"
                    "  \"element1\" : [\n"
                    "    \"LONDON\"\n"
                    "  ],\n"
                    "  \"element2\" : [],\n"
                    "  \"element6\" : []\n"
                    "}\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element1>LONDON</element1>\n"
                    "  <element1>NEW_JERSEY</element1>\n"
                    "</element1>\n",
                    false,
                    // JSON Text
                    "{\n"
                    "  \"element1\" : [\n"
                    "    \"LONDON\",\n"
                    "    \"NEW_JERSEY\"\n"
                    "  ]\n"
                    "}\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element1>LONDON</element1>\n"
                    "  <element1>NEW_JERSEY</element1>\n"
                    "</element1>\n",
                    true,
                    // JSON Text
                    "{\n"
                    "  \"element1\" : [\n"
                    "    \"LONDON\",\n"
                    "    \"NEW_JERSEY\"\n"
                    "  ],\n"
                    "  \"element2\" : [],\n"
                    "  \"element6\" : []\n"
                    "}\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element2>arbitrary string value</element2>\n"
                    "</element1>\n",
                    false,
                    // JSON Text
                    "{\n"
                    "  \"element2\" : [\n"
                    "    \"arbitrary string value\"\n"
                    "  ]\n"
                    "}\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element2>arbitrary string value</element2>\n"
                    "</element1>\n",
                    true,
                    // JSON Text
                    "{\n"
                    "  \"element1\" : [],\n"
                    "  \"element2\" : [\n"
                    "    \"arbitrary string value\"\n"
                    "  ],\n"
                    "  \"element6\" : []\n"
                    "}\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element6>NEW_YORK</element6>\n"
                    "</element1>\n",
                    false,
                    // JSON Text
                    "{\n"
                    "  \"element6\" : [\n"
                    "    \"NEW_YORK\"\n"
                    "  ]\n"
                    "}\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element6>NEW_YORK</element6>\n"
                    "</element1>\n",
                    true,
                    // JSON Text
                    "{\n"
                    "  \"element1\" : [],\n"
                    "  \"element2\" : [],\n"
                    "  \"element6\" : [\n"
                    "    \"NEW_YORK\"\n"
                    "  ]\n"
                    "}\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element1>LONDON</element1>\n"
                    "  <element2>arbitrary string value</element2>\n"
                    "</element1>\n",
                    false,
                    // JSON Text
                    "{\n"
                    "  \"element1\" : [\n"
                    "    \"LONDON\"\n"
                    "  ],\n"
                    "  \"element2\" : [\n"
                    "    \"arbitrary string value\"\n"
                    "  ]\n"
                    "}\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element1>LONDON</element1>\n"
                    "  <element2>arbitrary string value</element2>\n"
                    "</element1>\n",
                    true,
                    // JSON Text
                    "{\n"
                    "  \"element1\" : [\n"
                    "    \"LONDON\"\n"
                    "  ],\n"
                    "  \"element2\" : [\n"
                    "    \"arbitrary string value\"\n"
                    "  ],\n"
                    "  \"element6\" : []\n"
                    "}\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element1>LONDON</element1>\n"
                    "  <element2>arbitrary string value</element2>\n"
                    "  <element6>NEW_YORK</element6>\n"
                    "</element1>\n",
                    false,
                    // JSON Text
                    "{\n"
                    "  \"element1\" : [\n"
                    "    \"LONDON\"\n"
                    "  ],\n"
                    "  \"element2\" : [\n"
                    "    \"arbitrary string value\"\n"
                    "  ],\n"
                    "  \"element6\" : [\n"
                    "    \"NEW_YORK\"\n"
                    "  ]\n"
                    "}\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element1>LONDON</element1>\n"
                    "  <element2>arbitrary string value</element2>\n"
                    "  <element6>NEW_YORK</element6>\n"
                    "</element1>\n",
                    true,
                    // JSON Text
                    "{\n"
                    "  \"element1\" : [\n"
                    "    \"LONDON\"\n"
                    "  ],\n"
                    "  \"element2\" : [\n"
                    "    \"arbitrary string value\"\n"
                    "  ],\n"
                    "  \"element6\" : [\n"
                    "    \"NEW_YORK\"\n"
                    "  ]\n"
                    "}\n"
                },
                {
                    L_,
                    // XML Text
                    "<element1>\n"
                    "  <element1>LONDON</element1>\n"
                    "  <element2>arbitrary string value</element2>\n"
                    "  <element6>NEW_YORK</element6>\n"
                    "</element1>\n",
                    false,
                    // JSON Text
                    "{\n"
                    "  \"element1\" : [\n"
                    "    \"LONDON\"\n"
                    "  ],\n"
                    "  \"element2\" : [\n"
                    "    \"arbitrary string value\"\n"
                    "  ],\n"
                    "  \"element6\" : [\n"
                    "    \"NEW_YORK\"\n"
                    "  ]\n"
                    "}\n"
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
                    "</element1>\n",
                    true,
                    // JSON Text
                    "{\n"
                    "  \"element1\" : [\n"
                    "    \"NEW_JERSEY\",\n"
                    "    \"LONDON\"\n"
                    "  ],\n"
                    "  \"element2\" : [\n"
                    "    \"something random\",\n"
                    "    \"arbitrary string value\"\n"
                    "  ],\n"
                    "  \"element6\" : [\n"
                    "    \"NEW_YORK\",\n"
                    "    \"NEW_JERSEY\"\n"
                    "  ]\n"
                    "}\n"
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
                    "</element1>\n",
                    false,
                    // JSON Text
                    "{\n"
                    "  \"element1\" : [\n"
                    "    \"NEW_JERSEY\"\n"
                    "  ],\n"
                    "  \"element5\" : {\n"
                    "    \"element1\" : {\n"
                    "      \"element2\" : [\n"
                    "        \"arbitrary string value\"\n"
                    "      ]\n"
                    "    }\n"
                    "  }\n"
                    "}\n"
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
                    "</element1>\n",
                    true,
                    // JSON Text
                    "{\n"
                    "  \"element1\" : [\n"
                    "    \"NEW_JERSEY\"\n"
                    "  ],\n"
                    "  \"element2\" : [],\n"
                    "  \"element5\" : {\n"
                    "    \"element1\" : {\n"
                    "      \"element1\" : [],\n"
                    "      \"element2\" : [\n"
                    "        \"arbitrary string value\"\n"
                    "      ],\n"
                    "      \"element6\" : []\n"
                    "    },\n"
                    "    \"element2\" : [],\n"
                    "    \"element3\" : [],\n"
                    "    \"element4\" : [],\n"
                    "    \"element5\" : [],\n"
                    "    \"element6\" : [],\n"
                    "    \"element7\" : []\n"
                    "  },\n"
                    "  \"element6\" : []\n"
                    "}\n"
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
                    "</element1>\n",
                    true,
                    // JSON Text
                    "{\n"
                    "  \"element1\" : [],\n"
                    "  \"element2\" : [],\n"
                    "  \"element5\" : {\n"
                    "    \"element1\" : {\n"
                    "      \"element1\" : [],\n"
                    "      \"element2\" : [],\n"
                    "      \"element6\" : []\n"
                    "    },\n"
                    "    \"element2\" : [],\n"
                    "    \"element3\" : [],\n"
                    "    \"element4\" : [],\n"
                    "    \"element5\" : [\n"
                    "      123456,\n"
                    "      7890\n"
                    "    ],\n"
                    "    \"element6\" : [],\n"
                    "    \"element7\" : []\n"
                    "  },\n"
                    "  \"element6\" : []\n"
                    "}\n"
                },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const bsl::string XML    = DATA[ti].d_xmlText_p;
                const int         EEA    = DATA[ti].d_encodeEmptyArrays;
                const bsl::string EXP    = DATA[ti].d_jsonText_p;

                balb::Sequence3 object;
                ASSERT(0 == u::populateTestObject(&object, XML));

                baljsn::EncoderOptions options;
                options.setEncodingStyle(Options::e_PRETTY);
                options.setInitialIndentLevel(0);
                options.setSpacesPerLevel(2);
                options.setEncodeEmptyArrays(EEA);

                baljsn::Encoder encoder;
                const Options& mO = options;
                {
                    bsl::ostringstream oss;
                    ASSERTV(0 == encoder.encode(oss, object, mO));
                    ASSERTV(LINE, oss.str(), EXP, oss.str() == EXP);
                }
                {
                    bsl::ostringstream oss;
                    ASSERTV(0 == encoder.encode(oss, object, &mO));
                    ASSERTV(LINE, oss.str(), EXP, oss.str() == EXP);
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // ENCODING NULL-ABLES
        //
        // Concerns:
        //: 1 Null value is encoded to "null".
        //:
        //: 2 Encoding a Nullable object with non-null value is the same as it
        //:   the encoding is performed on the value directly.
        //
        // Plan:
        //: 1 Use the brute force approach:
        //:
        //:   1 Create a Nullable object.
        //:
        //:   2 Encode the Nullable object and verify it is encoded as "null".
        //:
        //:   3 Make the value non-null.
        //:
        //:   4 Encode the value and verify the result is as expected.
        //
        // Testing:
        //   int encode(ostream& s, const bdlb::NullableValue<TYPE>& v, o);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING NULL-ABLES"
                          << "\n===================" << endl;

        if (verbose) cout << "Encode null value" << endl;
        {
            bdlb::NullableValue<int> mX;
            const bdlb::NullableValue<int>& X = mX;

            {
                bsl::ostringstream oss;
                ASSERTV(0 == ImplUtil::encode(&oss, X));

                bsl::string result = oss.str();
                ASSERTV(result, result == "null");
            }

            mX = 0;
            {
                bsl::ostringstream oss;
                ASSERTV(0 == ImplUtil::encode(&oss, X));

                bsl::string result = oss.str();
                ASSERTV(result, result == "0");
            }

            mX = 42;
            {
                bsl::ostringstream oss;
                ASSERTV(0 == ImplUtil::encode(&oss, X));

                bsl::string result = oss.str();
                ASSERTV(result, result == "42");
            }

            mX.reset();
            {
                bsl::ostringstream oss;
                ASSERTV(0 == ImplUtil::encode(&oss, X));

                bsl::string result = oss.str();
                ASSERTV(result, result == "null");
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // ENCODING ENUMERATIONS
        //
        // Concerns:
        //: 1 Encoding an Enumeration object result in a JSON string of the
        //:   string representation of the Enumeration value.
        //
        // Plan:
        //: 1 Use a generated Enumeration type and encode each enumeration
        //:   value.
        //:
        //: 2 Verify that the result is equal to the value of the 'toString'
        //:   method enclosed in double quotes.
        //
        // Testing:
        //   int encode(ostream& s, const TYPE& v, o);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING ENUMERATIONS"
                          << "\n=====================" << endl;

        const int NUM_ENUMERATORS = balb::Enumerated::NUM_ENUMERATORS;
        for (int ti = 0; ti < NUM_ENUMERATORS; ++ti) {
            balb::Enumerated::Value mX = (balb::Enumerated::Value) ti;
            const balb::Enumerated::Value& X = mX;

            bsl::string exp("\"");
            exp += balb::Enumerated::toString(X);
            exp += '\"';

            bsl::ostringstream oss;
            ASSERTV(ti, 0 == ImplUtil::encode(&oss, X));

            bsl::string result = oss.str();
            ASSERTV(ti, result, exp, result == exp);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // ENCODING DATE AND TIME TYPES
        //
        // Concerns:
        //: 1 Date/time are encoded in ISO 8601 format.
        //:
        //: 2 Output contains only information contained in the type being
        //:   encoded.  (i.e., encoding 'bdlt::Date' will not print out a time
        //:   or offset.)
        //:
        //: 3 The encoder option "datetimeFractionalSecondPrecision" affects
        //:   the number of digits of precision in the outputs.
        //
        // Plan:
        //: 1 Use the table-driven technique:
        //:
        //:   1 Specify a set of valid values.
        //:
        //:   2 Encode each value and verify the output is as expected.
        //:
        //: 2 Perform step one for every date/time types.
        //:
        //: 3 For a Datetime and DatetimeTz value, set the
        //:   "datetimeFractionalSecondPrecision" attribute from 0 to 6, and
        //:   verify that the output is as expected.
        //
        // Testing:
        //   int encode(ostream& s, const TYPE& v, o);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING DATE AND TIME TYPES"
                          << "\n============================" << endl;

        typedef bdlb::Variant2<bdlt::Date, bdlt::DateTz>  DateOrDateTz;
        typedef bdlb::Variant2<bdlt::Time, bdlt::TimeTz>  TimeOrTimeTz;
        typedef bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
                                                          DatetimeOrDatetimeTz;

        const struct {
            int        d_line;
            int        d_year;
            int        d_month;
            int        d_day;
            int        d_hour;
            int        d_minute;
            int        d_second;
            int        d_millisecond;
            int        d_offset;
        } DATA[] = {
            //Line Year   Mon  Day  Hour  Min  Sec     ms   offset
            //---- ----   ---  ---  ----  ---  ---     --   ------

            // Valid dates and times
            { L_,     1,   1,   1,    0,   0,   0,     0,        0 },
            { L_,  2005,   1,   1,    0,   0,   0,     0,      -90 },
            { L_,   123,   6,  15,   13,  40,  59,     0,     -240 },
            { L_,  1999,  10,  12,   23,   0,   1,     0,     -720 },

            // Vary milliseconds
            { L_,  1999,  10,  12,   23,   0,   1,     0,       90 },
            { L_,  1999,  10,  12,   23,   0,   1,   456,      240 },
            { L_,  1999,  10,  12,   23,   0,   1,   999,      720 },
            { L_,  1999,  12,  31,   23,  59,  59,   999,      720 }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const char *expectedDate[] = {
            "\"0001-01-01\"",
            "\"2005-01-01\"",
            "\"0123-06-15\"",
            "\"1999-10-12\"",
            "\"1999-10-12\"",
            "\"1999-10-12\"",
            "\"1999-10-12\"",
            "\"1999-12-31\""
        };

        const char *expectedDateTz[] = {
            "\"0001-01-01+00:00\"",
            "\"2005-01-01-01:30\"",
            "\"0123-06-15-04:00\"",
            "\"1999-10-12-12:00\"",
            "\"1999-10-12+01:30\"",
            "\"1999-10-12+04:00\"",
            "\"1999-10-12+12:00\"",
            "\"1999-12-31+12:00\""
        };

        const char *expectedTime[] = {
            "\"00:00:00.000\"",
            "\"00:00:00.000\"",
            "\"13:40:59.000\"",
            "\"23:00:01.000\"",
            "\"23:00:01.000\"",
            "\"23:00:01.456\"",
            "\"23:00:01.999\"",
            "\"23:59:59.999\""
        };

        const char *expectedTimeTz[] = {
            "\"00:00:00.000+00:00\"",
            "\"00:00:00.000-01:30\"",
            "\"13:40:59.000-04:00\"",
            "\"23:00:01.000-12:00\"",
            "\"23:00:01.000+01:30\"",
            "\"23:00:01.456+04:00\"",
            "\"23:00:01.999+12:00\"",
            "\"23:59:59.999+12:00\""
        };

        const char *expectedDatetime[] = {
            "\"0001-01-01T00:00:00.000\"",
            "\"2005-01-01T00:00:00.000\"",
            "\"0123-06-15T13:40:59.000\"",
            "\"1999-10-12T23:00:01.000\"",
            "\"1999-10-12T23:00:01.000\"",
            "\"1999-10-12T23:00:01.456\"",
            "\"1999-10-12T23:00:01.999\"",
            "\"1999-12-31T23:59:59.999\""
        };

        const char *expectedDatetimeTz[] = {
            "\"0001-01-01T00:00:00.000+00:00\"",
            "\"2005-01-01T00:00:00.000-01:30\"",
            "\"0123-06-15T13:40:59.000-04:00\"",
            "\"1999-10-12T23:00:01.000-12:00\"",
            "\"1999-10-12T23:00:01.000+01:30\"",
            "\"1999-10-12T23:00:01.456+04:00\"",
            "\"1999-10-12T23:00:01.999+12:00\"",
            "\"1999-12-31T23:59:59.999+12:00\""
        };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE        = DATA[ti].d_line;
            const int YEAR        = DATA[ti].d_year;
            const int MONTH       = DATA[ti].d_month;
            const int DAY         = DATA[ti].d_day;
            const int HOUR        = DATA[ti].d_hour;
            const int MINUTE      = DATA[ti].d_minute;
            const int SECOND      = DATA[ti].d_second;
            const int MILLISECOND = DATA[ti].d_millisecond;
            const int OFFSET      = DATA[ti].d_offset;;

            bdlt::Date theDate(YEAR, MONTH, DAY);
            bdlt::Time theTime(HOUR, MINUTE, SECOND, MILLISECOND);
            bdlt::Datetime theDatetime(YEAR, MONTH, DAY,
                                      HOUR, MINUTE, SECOND, MILLISECOND);

            bdlt::DateTz     theDateTz(theDate, OFFSET);
            bdlt::TimeTz     theTimeTz(theTime, OFFSET);
            bdlt::DatetimeTz theDatetimeTz(theDatetime, OFFSET);

            if (verbose) cout << "Encode Date" << endl;
            {
                const char *EXP = expectedDate[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, theDate));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode DateTz" << endl;
            {
                const char *EXP = expectedDateTz[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, theDateTz));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode Time" << endl;
            {
                const char *EXP = expectedTime[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, theTime));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode TimeTz" << endl;
            {
                const char *EXP = expectedTimeTz[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, theTimeTz));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode Datetime" << endl;
            {
                const char *EXP = expectedDatetime[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, theDatetime));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode DatetimeTz" << endl;
            {
                const char *EXP = expectedDatetimeTz[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, theDatetimeTz));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode DateOrDateTz" << endl;
            {
                const char *EXP_DATE   = expectedDate[ti];
                const char *EXP_DATETZ = expectedDateTz[ti];

                DateOrDateTz dateVariant(theDate);


                bsl::ostringstream ossDate;
                ASSERTV(LINE, 0 == ImplUtil::encode(&ossDate, dateVariant));

                bsl::string result = ossDate.str();
                ASSERTV(LINE, result, EXP_DATE, result == EXP_DATE);

                dateVariant = theDateTz;

                bsl::ostringstream ossDateTz;
                ASSERTV(LINE, 0 == ImplUtil::encode(&ossDateTz, dateVariant));

                result = ossDateTz.str();
                ASSERTV(LINE, result, EXP_DATETZ, result == EXP_DATETZ);
            }

            if (verbose) cout << "Encode TimeOrTimeTz" << endl;
            {
                const char *EXP_TIME   = expectedTime[ti];
                const char *EXP_TIMETZ = expectedTimeTz[ti];

                TimeOrTimeTz timeVariant(theTime);


                bsl::ostringstream ossTime;
                ASSERTV(LINE, 0 == ImplUtil::encode(&ossTime, timeVariant));

                bsl::string result = ossTime.str();
                ASSERTV(LINE, result, EXP_TIME, result == EXP_TIME);

                timeVariant = theTimeTz;

                bsl::ostringstream ossTimeTz;
                ASSERTV(LINE, 0 == ImplUtil::encode(&ossTimeTz, timeVariant));

                result = ossTimeTz.str();
                ASSERTV(LINE, result, EXP_TIMETZ, result == EXP_TIMETZ);
            }

            if (verbose) cout << "Encode DatetimeOrDatetimeTz" << endl;
            {
                const char *EXP_DATETIME   = expectedDatetime[ti];
                const char *EXP_DATETIMETZ = expectedDatetimeTz[ti];

                DatetimeOrDatetimeTz datetimeVariant(theDatetime);


                bsl::ostringstream ossDatetime;
                ASSERTV(LINE,
                        0 == ImplUtil::encode(&ossDatetime, datetimeVariant));

                bsl::string result = ossDatetime.str();
                ASSERTV(LINE, result, EXP_DATETIME, result == EXP_DATETIME);

                datetimeVariant = theDatetimeTz;

                bsl::ostringstream ossDatetimeTz;
                ASSERTV(LINE,
                        0 == ImplUtil::encode(&ossDatetimeTz,
                                              datetimeVariant));

                result = ossDatetimeTz.str();
                ASSERTV(LINE, result, EXP_DATETIMETZ,
                        result == EXP_DATETIMETZ);
            }
        }
        {
            bdlt::Datetime theDatetime(2010, 03, 15, 17, 23, 45, 123, 456);

            const char *expectedDatetime[] = {
                "\"2010-03-15T17:23:45\"",
                "\"2010-03-15T17:23:45.1\"",
                "\"2010-03-15T17:23:45.12\"",
                "\"2010-03-15T17:23:45.123\"",
                "\"2010-03-15T17:23:45.1234\"",
                "\"2010-03-15T17:23:45.12345\"",
                "\"2010-03-15T17:23:45.123456\"",
            };

            for (int pi = 0; pi <= 6; ++pi) {
                const char *EXP = expectedDatetime[pi];
                bsl::ostringstream oss;
                Options opt;
                opt.setDatetimeFractionalSecondPrecision(pi);
                ASSERTV(0 == ImplUtil::encode(&oss, theDatetime, opt));
                bsl::string result = oss.str();
                ASSERTV(pi, result, EXP, result == EXP);
            }
        }
        {
            bdlt::Datetime theDatetime(2010, 03, 15, 17, 23, 45, 123, 456);
            bdlt::DatetimeTz theDatetimeTz(theDatetime, -300);

            const char *expectedDatetimeTz[] = {
                "\"2010-03-15T17:23:45-05:00\"",
                "\"2010-03-15T17:23:45.1-05:00\"",
                "\"2010-03-15T17:23:45.12-05:00\"",
                "\"2010-03-15T17:23:45.123-05:00\"",
                "\"2010-03-15T17:23:45.1234-05:00\"",
                "\"2010-03-15T17:23:45.12345-05:00\"",
                "\"2010-03-15T17:23:45.123456-05:00\"",
            };

            for (int pi = 0; pi <= 6; ++pi) {
                const char *EXP = expectedDatetimeTz[pi];
                bsl::ostringstream oss;
                Options opt;
                opt.setDatetimeFractionalSecondPrecision(pi);
                ASSERTV(0 == ImplUtil::encode(&oss, theDatetimeTz, opt));
                bsl::string result = oss.str();
                ASSERTV(pi, result, EXP, result == EXP);
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // ENCODING NUMBERS
        //
        // Concerns:
        //: 1 Encoded numbers have the expected precisions.
        //:
        //: 2 Encoded numbers used default format.
        //:
        //: 3 Encoding 'unsigned char' prints a number instead of string.
        //
        // Plan:
        //: 1 Use the table-driven technique:
        //:
        //:   1 Specify a set of valid values, including those that will test
        //:     the precision of the output.
        //:
        //:   2 Encode each value and verify the output is as expected.
        //
        // Testing:
        //   int encode(ostream& s, const TYPE& v, o);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING NUMBERS"
                          << "\n================" << endl;

        if (verbose) cout << "Encode 'double'" << endl;
        {
            typedef bsl::numeric_limits<double> Limits;

            const double negZero = -1 / Limits::infinity();

            const struct {
                int         d_line;
                double      d_value;
                const char *d_result;
            } DATA[] = {
                //LINE  VALUE                RESULT
                //----  ----------------     -------------------------
                { L_,    0.0,                    "0"                   },
                { L_,    1.0e-1,                 "0.1"                 },
                { L_,    0.125,                  "0.125"               },
                { L_,    1.0,                    "1"                   },
                { L_,    1.5,                    "1.5"                 },
                { L_,   10.0,                   "10"                   },
                { L_,    1.5e1,                 "15"                   },
                { L_,    9.9e100,                "9.9e+100"            },
                { L_,    3.14e300,               "3.14e+300"           },
                { L_,    2.23e-308,              "2.23e-308"           },
                { L_,    0.12345678912345,       "0.12345678912345"    },
                { L_,    0.12345678901234567,    "0.12345678901234566" },
                { L_,    0.123456789012345678,   "0.12345678901234568" },

                { L_, negZero,                  "-0"                   },
                { L_,   -1.0e-1,                "-0.1"                 },
                { L_,   -0.125,                 "-0.125"               },
                { L_,   -1.0,                   "-1"                   },
                { L_,   -1.5,                   "-1.5"                 },
                { L_,  -10.0,                  "-10"                   },
                { L_,   -1.5e1,                "-15"                   },
                { L_,   -9.9e100,               "-9.9e+100"            },
                { L_,   -3.14e300,              "-3.14e+300"           },
                { L_,   -2.23e-308,             "-2.23e-308"           },
                { L_,   -0.12345678912345,      "-0.12345678912345"    },
                { L_,   -0.12345678901234567,   "-0.12345678901234566" },
                { L_,   -0.123456789012345678,  "-0.12345678901234568" },

                // Small Integers
                { L_, 123456789012345.,           "123456789012345"    },
                { L_, 1234567890123456.,          "1234567890123456"   },

                // Full Mantissa Integers
                { L_, 1.0 * 0x1FFFFFFFFFFFFFull, "9007199254740991"     },
                { L_, 1.0 * 0x1FFFFFFFFFFFFFull  // This is also limits::max()
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 26),         "1.7976931348623157e+308" },

                // Boundary Values
                { L_,  Limits::min(),         "2.2250738585072014e-308" },
                { L_,  Limits::denorm_min(),  "5e-324"                  },
                { L_,  Limits::max(),         "1.7976931348623157e+308" },
                { L_, -Limits::min(),        "-2.2250738585072014e-308" },
                { L_, -Limits::denorm_min(), "-5e-324"                  },
                { L_, -Limits::max(),        "-1.7976931348623157e+308" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE      = DATA[ti].d_line;
                const double      VALUE     = DATA[ti].d_value;
                const char * const EXPECTED = DATA[ti].d_result;

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, VALUE));

                const bsl::string result = oss.str();
                ASSERTV(LINE, result, EXPECTED, result == EXPECTED);
            }
        }

        if (verbose) cout << "Encode invalid 'double'" << endl;
        {
            bsl::ostringstream oss;

            oss.clear();
            ASSERTV(0 != ImplUtil::encode(
                             &oss, bsl::numeric_limits<double>::infinity()));

            oss.clear();
            ASSERTV(0 != ImplUtil::encode(
                             &oss, bsl::numeric_limits<double>::infinity()));

            oss.clear();
            ASSERTV(0 != ImplUtil::encode(
                             &oss, bsl::numeric_limits<double>::quiet_NaN()));

            oss.clear();
            ASSERTV(0 !=
                    ImplUtil::encode(
                        &oss, bsl::numeric_limits<double>::signaling_NaN()));
        }

        if (verbose) cout << "Encode 'float'" << endl;
        {
            typedef bsl::numeric_limits<float> Limits;

            const float negZero = -1 / Limits::infinity();

            const struct {
                int         d_line;
                float       d_value;
                const char *d_result;
            } DATA[] = {
                //LINE        VALUE         RESULT
                //----  -------------  ---------------
                { L_,           0.0f,   "0"           },
                { L_,         0.125f,   "0.125"       },
                { L_,        1.0e-1f,   "0.1"         },
                { L_,      0.123456f,   "0.123456"    },
                { L_,           1.0f,   "1"           },
                { L_,           1.5f,   "1.5"         },
                { L_,          10.0f,  "10"           },
                { L_,         1.5e1f,  "15"           },
                { L_,   1.23456e-20f,   "1.23456e-20" },
                { L_,   0.123456789f,   "0.12345679"  },
                { L_,  0.1234567891f,   "0.12345679"  },

                { L_, negZero,         "-0"           },
                { L_,        -0.125f,  "-0.125"       },
                { L_,       -1.0e-1f,  "-0.1"         },
                { L_,     -0.123456f,  "-0.123456"    },
                { L_,          -1.0f,  "-1"           },
                { L_,          -1.5f,  "-1.5"         },
                { L_,         -10.0f, "-10"           },
                { L_,        -1.5e1f,  "-15"          },
                { L_,  -1.23456e-20f,  "-1.23456e-20" },
                { L_,  -0.123456789f,  "-0.12345679"  },
                { L_, -0.1234567891f,  "-0.12345679"  },

                // {DRQS 165162213} regression, 2^24 loses precision as float
                { L_, 1.0f * 0xFFFFFF,  "16777215"    },

                // Full Mantissa Integers
                { L_, 1.0f * 0xFFFFFF,  "16777215"        },
                { L_, 1.0f * 0xFFFFFF      // this happens to be also
                       * (1ull << 63)      // 'Limits::max()'
                       * (1ull << 41),    "3.4028235e+38" },

                // Boundary Values
                { L_,  Limits::min(),         "1.1754944e-38" },
                { L_,  Limits::denorm_min(),  "1e-45"         },
                { L_,  Limits::max(),         "3.4028235e+38" },
                { L_, -Limits::min(),        "-1.1754944e-38" },
                { L_, -Limits::denorm_min(), "-1e-45"         },
                { L_, -Limits::max(),        "-3.4028235e+38" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const float        VALUE    = DATA[ti].d_value;
                const char * const EXPECTED = DATA[ti].d_result;

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, VALUE));

                const bsl::string result = oss.str();
                ASSERTV(LINE, result, EXPECTED, result == EXPECTED);
            }
        }

        if (verbose) cout << "Encode 'Decimal64'" << endl;
        {
#define DEC(X) BDLDFP_DECIMAL_DD(X)
            using bdldfp::Decimal64;

            typedef bsl::numeric_limits<Decimal64> Limits;

            const struct {
                int         d_line;
                Decimal64   d_value;
                const char *d_result;
            } DATA[] = {
                //LINE  VALUE       RESULT
                //----  -----       ------
                { L_,   DEC(0.0),    "\"0.0\""                    },
                { L_,   DEC(-0.0),  "\"-0.0\""                    },
                { L_,   DEC(1.13),   "\"1.13\""                   },

                { L_,   DEC(-9.876543210987654e307),
                                    "\"-9.876543210987654e+307\"" },
                { L_,   DEC(-9.8765432109876548e307),
                                    "\"-9.876543210987655e+307\"" },
                { L_,   DEC(-9.87654321098765482e307),
                                    "\"-9.876543210987655e+307\"" },

                { L_,    Limits::min(),         "\"1e-383\""                 },
                { L_,    Limits::denorm_min(),  "\"1e-398\""                 },
                { L_,    Limits::max(),         "\"9.999999999999999e+384\"" },
                { L_,   -Limits::min(),        "\"-1e-383\""                 },
                { L_,   -Limits::denorm_min(), "\"-1e-398\""                 },
                { L_,   -Limits::max(),        "\"-9.999999999999999e+384\"" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const Decimal64    VALUE    = DATA[ti].d_value;
                const char * const EXPECTED = DATA[ti].d_result;

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, VALUE));

                const bsl::string result = oss.str();
                ASSERTV(LINE, result, EXPECTED, result == EXPECTED);
            }
#undef DEC
        }

        if (verbose) cout << "Encode integral types" << endl;
        {
            u::testNumber<short>();
            u::testNumber<int>();
            u::testNumber<Int64>();
            u::testNumber<unsigned char>();
            u::testNumber<unsigned short>();
            u::testNumber<unsigned int>();
            u::testNumber<Uint64>();
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // ENCODING STRINGS
        //
        // Concerns:
        //: 1 Character are encoded as a single character string.
        //:
        //: 2 All escape characters are encoded corrected.
        //:
        //: 3 Control characters are encoded as hex.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of values that include all escaped characters and
        //:     some control characters.
        //:
        //:   2 Encode the value and verify the results.
        //:
        //: 2 Repeat for strings and Customized type.
        //
        // Testing:
        //  int encode(const bsl::string & value);
        //  int encode(const char *value);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING STRINGS"
                          << "\n================" << endl;

        if (verbose) cout << "Encode char" << endl;
        {
            const struct {
                int         d_line;
                char        d_value;
                const char *d_result;
            } DATA[] = {
                //LINE    VAL     RESULT
                //----    ---     ------
                { L_,    '\0',      "0"     },
                { L_,    '\t',      "9"     },
                { L_,    '\n',     "10"     },
                { L_,    '\r',     "13"     },
                { L_,     ' ',     "32"     },
                { L_,     '!',     "33"     },
                { L_,     '"',     "34"     },
                { L_,     '/',     "47"     },
                { L_,     '0',     "48"     },
                { L_,     '9',     "57"     },
                { L_,     'A',     "65"     },
                { L_,     'Z',     "90"     },
                { L_,    '\\',     "92"     },
                { L_,     'a',     "97"     },
                { L_,     'z',    "122"     },
                { L_,     '~',    "126"     },

                { L_,  '\x7F',    "127"     },
                { L_,  '\x80',   "-128"     },
                { L_,  '\xff',     "-1"     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const char        VALUE = DATA[ti].d_value;
                const char *const EXP   = DATA[ti].d_result;

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, VALUE));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }
        }

        if (verbose) cout << "Encode string" << endl;
        {
            const struct {
                int         d_line;
                const char *d_value;
                const char *d_result;
            } DATA[] = {
                //LINE    VAL  RESULT
                //----    ---  ------
                { L_,  "",     "\"\"" },
                { L_,  " ",    "\" \"" },
                { L_,  "~",    "\"~\"" },
                { L_,  "test", "\"test\"" },
                { L_,  "A quick brown fox jump over a lazy dog!",
                               "\"A quick brown fox jump over a lazy dog!\"" },
                { L_,  "\"",   "\"\\\"\"" },
                { L_,  "\\",   "\"\\\\\"" },
                { L_,  "/",    "\"\\/\"" },
                { L_,  "\b",   "\"\\b\"" },
                { L_,  "\f",   "\"\\f\"" },
                { L_,  "\n",   "\"\\n\"" },
                { L_,  "\r",   "\"\\r\"" },
                { L_,  "\t",   "\"\\t\"" },
                { L_,  "\x01", "\"\\u0001\"" },
                { L_,  "\x1f", "\"\\u001f\"" },
                { L_,  "\\/\b\f\n\r\t",   "\"\\\\\\/\\b\\f\\n\\r\\t\"" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const char *const VALUE = DATA[ti].d_value;
                const char *const EXP   = DATA[ti].d_result;

                if (veryVeryVerbose) cout << "Test 'char *'" << endl;
                {
                    bsl::ostringstream oss;
                    ASSERTV(LINE, 0 == ImplUtil::encode(&oss, VALUE));

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (veryVeryVerbose) cout << "Test 'string'" << endl;
                {
                    bsl::ostringstream oss;
                    ASSERTV(LINE,
                            0 == ImplUtil::encode(&oss, bsl::string(VALUE)));

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (veryVeryVerbose) cout << "Test Customized" << endl;
                {
                    bsl::ostringstream oss;
                    balb::CustomString str;
                    if (0 == str.fromString(VALUE)) {
                        ASSERTV(LINE, 0 == ImplUtil::encode(&oss, str));

                        bsl::string result = oss.str();
                        ASSERTV(LINE, result, EXP, result == EXP);
                    }
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // ENCODING BOOLEAN
        //
        // Concerns:
        //: 1 'true' is encoded into "true" and 'false' is encoded into
        //:   "false".
        //
        // Plan:
        //: 1 Use a brute force approach to test both cases.
        //
        // Testing:
        //   int encode(const bool& value);
        //   baljsn::Encoder(bslma::Allocator *basicAllocator = 0);
        //   ~baljsn::Encoder();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING BOOLEAN"
                          << "\n================" << endl;

        if (verbose) cout << "Encode 'true'" << endl;
        {
            bsl::ostringstream oss;
            ASSERTV(0 == ImplUtil::encode(&oss, true));

            bsl::string result = oss.str();
            ASSERTV(result, result == "true");
        }

        if (verbose) cout << "Encode 'false'" << endl;
        {
            bsl::ostringstream oss;
            ASSERTV(0 == ImplUtil::encode(&oss, false));

            bsl::string result = oss.str();
            ASSERTV(result, result == "false");
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

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

            baljsn::Encoder encoder;
            bsl::ostringstream oss;
            baljsn::EncoderOptions options;
            ASSERTV(0 == encoder.encode(oss, bob, options));
            ASSERTV(oss.str() == jsonTextCompact);
            if (verbose) {
                P(oss.str());
            }
        }


        baljsn::EncoderOptions options;
        options.setEncodingStyle(baljsn::EncoderOptions::e_PRETTY);
        options.setInitialIndentLevel(1);
        options.setSpacesPerLevel(4);
        const Options& mO = options;

        baljsn::Encoder encoder;

        {
            bsl::istringstream iss(jsonTextPretty);
            bsl::ostringstream oss;

            ASSERTV(0 == encoder.encode(oss, bob, mO));
            ASSERTV(oss.str() == jsonTextPretty);
            if (verbose) {
                P(oss.str()); P(bsl::string(jsonTextPretty));
            }
        }
        {
            bsl::istringstream iss(jsonTextPretty);
            bsl::ostringstream oss;

            ASSERTV(0 == encoder.encode(oss, bob, &mO));
            ASSERTV(oss.str() == jsonTextPretty);
            if (verbose) {
                P(oss.str()); P(bsl::string(jsonTextPretty));
            }
        }

        u::EmptySequenceExample sequenceExample;
        sequenceExample.simpleValue() = 1;

        bsl::stringstream ss;
        encoder.encode(ss, sequenceExample);

        ASSERTV(ss.str(), "{\"simpleValue\":1}" == ss.str());
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

#ifdef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
#undef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
#endif

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
