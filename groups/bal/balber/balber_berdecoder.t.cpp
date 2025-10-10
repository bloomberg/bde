// balber_berdecoder.t.cpp                                            -*-C++-*-
#include <balber_berdecoder.h>

#include <balber_berdecoderoptions.h>
#include <balber_berencoder.h>
#include <balber_berencoderoptions.h>

#include <s_baltst_address.h>
#include <s_baltst_basicrecord.h>
#include <s_baltst_bigrecord.h>
#include <s_baltst_customizedstring.h>
#include <s_baltst_depthtestmessageutil.h>
#include <s_baltst_employee.h>
#include <s_baltst_mychoice.h>
#include <s_baltst_myenumeration.h>
#include <s_baltst_myintenumeration.h>
#include <s_baltst_myenumerationwithfallback.h>
#include <s_baltst_mysequence.h>
#include <s_baltst_mysequencewithanonymouschoice.h>
#include <s_baltst_mysequencewitharray.h>
#include <s_baltst_mysequencewithnillable.h>
#include <s_baltst_mysequencewithnullable.h>
#include <s_baltst_mysequencewiththreearrays.h>
#include <s_baltst_rawdata.h>
#include <s_baltst_rawdataswitched.h>
#include <s_baltst_rawdataunformatted.h>
#include <s_baltst_request.h>
#include <s_baltst_sqrt.h>
#include <s_baltst_timingrequest.h>

#include <balb_testmessages.h>          // for testing only

#include <balxml_decoder.h>             // for testing only
#include <balxml_decoderoptions.h>      // for testing only
#include <balxml_minireader.h>          // for testing only
#include <balxml_errorinfo.h>           // for testing only

#include <bdlat_attributeinfo.h>
#include <bdlat_selectioninfo.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_string.h>
#include <bdlsb_memoutstreambuf.h>      // for testing only
#include <bdlsb_fixedmeminstreambuf.h>  // for testing only

#include <bdlb_chartype.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_prolepticdateimputil.h>
#include <bdlt_time.h>

#include <bslim_fuzzdataview.h>
#include <bslim_fuzzutil.h>
#include <bslim_printer.h>
#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_objectbuffer.h>
#include <bsls_review.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_fstream.h>
#include <bsl_iostream.h>
#include <bsl_iomanip.h>
#include <bsl_iterator.h>

using namespace BloombergLP;
using namespace bsl;
namespace test = BloombergLP::s_baltst;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// TBD...
// ----------------------------------------------------------------------------
// [18] int BerDecoder_Node::decode(bsl::vector<char>          *variable,
//                                  bdlat_TypeCategory::Array  )
// [18] int BerDecoder_Node::decode(bsl::vector<unsigned char> *variable,
//                                  bdlat_TypeCategory::Array  )
//
// [19] int BerDecoder::decode(bsl::streambuf *streamBuf, TYPE *variable)
// [19] int BerDeocder::decode(bsl::istream&   stream   , TYPE *variable)
//
// [20] int decode(bsl::streambuf *streamBuf, TYPE *variable)
// ----------------------------------------------------------------------------
// [ 1] FUNDAMENTALS TEST
// [ 2] ENCODE AND DECODE REAL/FLOATING-POINT
// [ 3] ENCODE AND DECODE VOCABULARY TYPES
// [ 4] ENCODE AND DECODE CUSTOMIZED TYPES
// [ 5] ENCODE AND DECODE ENUMERATIONS
// [ 6] ENCODE AND DECODE SEQUENCES
// [ 7] ENCODE AND DECODE CHOICES
// [ 8] ENCODE AND DECODE NULLABLE VALUES
// [ 9] ENCODE AND DECODE ARRAYS
// [10] ENCODE AND DECODE ANONYMOUS CHOICES
// [11] ENCODE AND DECODE NILLABLE VALUES
// [12] CHOICE DECODED AS SEQUENCE
// [13] SEQUENCE DECODED AS CHOICE
// [14] FUNCTIONS RELATED TO SKIPPED ELEMENTS
// [15] ENCODE AND DECODE DATE/TIME
// [16] ENCODE AND DECODE DATE/TIME COMPONENTS
// [17] DECODE DATE/TIME COMPONENTS USING A VARIANT
// [18] ENCODE AND DECODE `RawData`
// [19] DECODE COMPLEX TYPES WITH DEFAULT ELEMENTS
// [20] DECODE SEQUENCES OF MAXIMUM SIZE
// [21] DECODE INTS AS ENUMS AND VICE VERSA
// [22] DECODE DATE/TIME WITH LENGTH ANOMALIES
// [23] FUZZ TEST BUG (DRQS 175594554)
// [24] FUZZ TEST BUG (DRQS 175741365)
// [25] MAXDEPTH IS RESPECTED
// [26] USAGE EXAMPLE
//
// [-1] PERFORMANCE TEST

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

#define ASSERT_EQ(X,Y) BSLIM_TESTUTIL_LOOP2_ASSERT(X,Y,X == Y)
#define ASSERT_NE(X,Y) BSLIM_TESTUTIL_LOOP2_ASSERT(X,Y,X != Y)

#define ASSERT1_EQ(L,X,Y) BSLIM_TESTUTIL_LOOP3_ASSERT(L,X,Y,X == Y)
#define ASSERT1_NE(L,X,Y) BSLIM_TESTUTIL_LOOP3_ASSERT(L,X,Y,X != Y)

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

static bool         verbose = false;
static bool     veryVerbose = false;
static bool veryVeryVerbose = false;

static const char        *k_ALH_PREFIX        = "\x9f\x87\xff\xff\xff\x7f";
static const char        *k_ALH_SKIP          = "\x9f\x87\xfe\xff\xff\x7f";
static const bsl::size_t  k_ALH_PREFIX_LENGTH = strlen(k_ALH_PREFIX);

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

bsl::vector<char> loadFromHex(const char *hexData)
{
    char firstDigit = 0;

    bsl::vector<char> result;

    char hexValueTable[128] =
    {
        0 , //   0   0
        0 , //   1   1
        0 , //   2   2
        0 , //   3   3
        0 , //   4   4
        0 , //   5   5
        0 , //   6   6
        0 , //   7   7
        0 , //   8   8 - BACKSPACE
        0 , //   9   9 - TAB
        0 , //  10   a - LF
        0 , //  11   b
        0 , //  12   c
        0 , //  13   d - CR
        0 , //  14   e
        0 , //  15   f
        0 , //  16  10
        0 , //  17  11
        0 , //  18  12
        0 , //  19  13
        0 , //  20  14
        0 , //  21  15
        0 , //  22  16
        0 , //  23  17
        0 , //  24  18
        0 , //  25  19
        0 , //  26  1a
        0 , //  27  1b
        0 , //  28  1c
        0 , //  29  1d
        0 , //  30  1e
        0 , //  31  1f
        0 , //  32  20 - SPACE
        0 , //  33  21 - !
        0 , //  34  22 - "
        0 , //  35  23 - #
        0 , //  36  24 - $
        0 , //  37  25 - %
        0 , //  38  26 - &
        0 , //  39  27 - '
        0 , //  40  28 - (
        0 , //  41  29 - )
        0 , //  42  2a - *
        0 , //  43  2b - +
        0 , //  44  2c - ,
        0 , //  45  2d - -
        0 , //  46  2e - .
        0 , //  47  2f - /
        0 , //  48  30 - 0
        1 , //  49  31 - 1
        2 , //  50  32 - 2
        3 , //  51  33 - 3
        4 , //  52  34 - 4
        5 , //  53  35 - 5
        6 , //  54  36 - 6
        7 , //  55  37 - 7
        8 , //  56  38 - 8
        9 , //  57  39 - 9
        0 , //  58  3a - :
        0 , //  59  3b - ;
        0 , //  60  3c - <
        0 , //  61  3d - =
        0 , //  62  3e - >
        0 , //  63  3f - ?
        0 , //  64  40 - @
        10 , //  65  41 - A
        11 , //  66  42 - B
        12 , //  67  43 - C
        13 , //  68  44 - D
        14 , //  69  45 - E
        15 , //  70  46 - F
        0 , //  71  47 - G
        0 , //  72  48 - H
        0 , //  73  49 - I
        0 , //  74  4a - J
        0 , //  75  4b - K
        0 , //  76  4c - L
        0 , //  77  4d - M
        0 , //  78  4e - N
        0 , //  79  4f - O
        0 , //  80  50 - P
        0 , //  81  51 - Q
        0 , //  82  52 - R
        0 , //  83  53 - S
        0 , //  84  54 - T
        0 , //  85  55 - U
        0 , //  86  56 - V
        0 , //  87  57 - W
        0 , //  88  58 - X
        0 , //  89  59 - Y
        0 , //  90  5a - Z
        0 , //  91  5b - [
        0 , //  92  5c - '\'
        0 , //  93  5d - ]
        0 , //  94  5e - ^
        0 , //  95  5f - _
        0 , //  96  60 - `
        10 , //  97  61 - a
        11 , //  98  62 - b
        12 , //  99  63 - c
        13 , // 100  64 - d
        14 , // 101  65 - e
        15 , // 102  66 - f
        0 , // 103  67 - g
        0 , // 104  68 - h
        0 , // 105  69 - i
        0 , // 106  6a - j
        0 , // 107  6b - k
        0 , // 108  6c - l
        0 , // 109  6d - m
        0 , // 110  6e - n
        0 , // 111  6f - o
        0 , // 112  70 - p
        0 , // 113  71 - q
        0 , // 114  72 - r
        0 , // 115  73 - s
        0 , // 116  74 - t
        0 , // 117  75 - u
        0 , // 118  76 - v
        0 , // 119  77 - w
        0 , // 120  78 - x
        0 , // 121  79 - y
        0 , // 122  7a - z
        0 , // 123  7b - {
        0 , // 124  7c - |
        0 , // 125  7d - }
        0 , // 126  7e - ~
        0   // 127  7f - DEL
    };

    while (*hexData) {
        if (' ' == *hexData) {
            ++hexData;
            continue;
        }

        if (0 == firstDigit) {
            firstDigit = *hexData;
        }
        else {
            char value = static_cast<char>(  (hexValueTable[firstDigit] << 4)
                                           | hexValueTable[*hexData]);

            result.push_back(value);
            firstDigit = 0;
        }

        ++hexData;
    }

    ASSERT(0 == firstDigit);

    return result;
}

/// Print the specified `buffer` of the specified `length` in hex form
void printBuffer(const char *buffer, bsl::size_t length)
{
    cout << hex;
    int numOutput = 0;
    for (bsl::size_t i = 0; i < length; ++i) {
        if ((unsigned char) buffer[i] < 16) {
            cout << '0';
        }
        cout << (int) (unsigned char) buffer[i];
        numOutput += 2;
        if (0 == numOutput % 8) {
            cout << " ";
        }
    }
    cout << dec << endl;
}

void printDiagnostic(balber::BerDecoder & decoder)
{
    if (veryVerbose) {
        cout << decoder.loggedMessages();
    }
}

template <class DATE_TYPE>
void verifyDateTypesLengthAnomalies(bsl::size_t             MAX_LEN,
                                    bsl::size_t             ISOSTR_LEN,
                                    int                     LINE,
                                    const DATE_TYPE&        EXPECTED,
                                    const bsl::string_view& encoded)
{
    const bsl::size_t LEN_POS = encoded.length() - ISOSTR_LEN - 1;

    balber::BerDecoder decoder;
    int rc;

    // Verify that it decodes as is
    {
        bdlsb::FixedMemInStreamBuf isb(encoded.data(), encoded.length());
        DATE_TYPE value;
        rc = decoder.decode(&isb, &value);
        ASSERTV(LINE, rc, 0 == rc);

        // Verify that we get the expected value
        ASSERTV(LINE, EXPECTED, value, EXPECTED == value);
    }

    // Modifiable input to change encoded length
    bsl::string encAsStr(encoded.data(), encoded.length());

    // Verify that we are writing the length octet to the proper position
    {
        // Write the same length
        encAsStr[LEN_POS] = static_cast<char>(ISOSTR_LEN);

        // Verify that decoding still succeeds
        bdlsb::FixedMemInStreamBuf isb(encAsStr.data(), encAsStr.length());
        DATE_TYPE value;
        rc = decoder.decode(&isb, &value);
        ASSERTV(LINE, rc, 0 == rc);

        // Verify that we get the expected value
        ASSERTV(LINE, EXPECTED, value, EXPECTED == value);
    }

    // Verify that length is obeyed
    {
        // Write reduced length
        encAsStr[LEN_POS] = static_cast<char>(ISOSTR_LEN - 1);

        // Verify that decoding now fails
        bdlsb::FixedMemInStreamBuf isb(encAsStr.data(), encAsStr.length());
        DATE_TYPE value;
        rc = decoder.decode(&isb, &value);
        ASSERTV(LINE, rc, 0 != rc);
    }

    // Verify that length longer than data leads to an error
    {
        // Write increased length
        encAsStr[LEN_POS] = static_cast<char>(ISOSTR_LEN + 1);

        // Verify that decoding now fails
        bdlsb::FixedMemInStreamBuf isb(encAsStr.data(), encAsStr.length());
        DATE_TYPE value;
        rc = decoder.decode(&isb, &value);
        ASSERTV(LINE, rc, 0 != rc);
    }

    // Verify that length longer than maximum won't read data
    {
        // Write absurd length
        encAsStr[LEN_POS] = static_cast<char>(MAX_LEN + 1);

        bdlsb::FixedMemInStreamBuf isb(encAsStr.data(), encAsStr.length());

        // Double-check that decoding fails
        DATE_TYPE value;
        rc = decoder.decode(&isb, &value);
        ASSERTV(LINE, rc, 0 != rc);

        // Verify that decoding did not read the data part at all
        ASSERTV(LINE, ISOSTR_LEN, isb.length(), ISOSTR_LEN == isb.length());
    }

    // Verify that with length exactly at maximum we read the data
    {
        encAsStr[LEN_POS] = static_cast<char>(MAX_LEN);

        bdlsb::FixedMemInStreamBuf isb(encAsStr.data(), encAsStr.length());

        // Decoding fails because we don't have enough data
        DATE_TYPE value;
        rc = decoder.decode(&isb, &value);
        ASSERTV(LINE, rc, 0 != rc);

        // Verify that decoding read all the available data
        ASSERTV(LINE, isb.length(), 0 == isb.length());
    }
}

template <class TIME_TYPE>
void verifyTimeTypesLengthAnomalies(bsl::size_t             MAX_LEN,
                                    bsl::size_t             ISOSTR_LEN,
                                    bsl::size_t             INVALIDATE_LEN,
                                    int                     LINE,
                                    const TIME_TYPE&        EXPECTED,
                                    const bsl::string_view& encoded)
{
    const bsl::size_t LEN_POS = encoded.length() - ISOSTR_LEN - 1;

    balber::BerDecoder decoder;
    int rc;

    // Verify that it decodes as is
    {
        bdlsb::FixedMemInStreamBuf isb(encoded.data(), encoded.length());
        TIME_TYPE value;
        rc = decoder.decode(&isb, &value);
        ASSERTV(LINE, rc, 0 == rc);

        // Verify that we get the expected value
        ASSERTV(LINE, EXPECTED, value, EXPECTED == value);
    }

    // Modifiable input to change encoded length
    bsl::string encAsStr(encoded.data(), encoded.length());

    // Verify that we are writing the length octet to the proper position
    {
        // Write the same length
        encAsStr[LEN_POS] = static_cast<char>(ISOSTR_LEN);

        // Verify that decoding still succeeds
        bdlsb::FixedMemInStreamBuf isb(encAsStr.data(), encAsStr.length());
        TIME_TYPE value;
        rc = decoder.decode(&isb, &value);
        ASSERTV(LINE, rc, 0 == rc);

        // Verify that we get the expected value
        ASSERTV(LINE, EXPECTED, value, EXPECTED == value);
    }

    // Verify that length is obeyed
    {
        // Write reduced length
        encAsStr[LEN_POS] = static_cast<char>(ISOSTR_LEN - INVALIDATE_LEN);

        // Verify that decoding now fails
        bdlsb::FixedMemInStreamBuf isb(encAsStr.data(), encAsStr.length());
        TIME_TYPE value;
        rc = decoder.decode(&isb, &value);
        ASSERTV(LINE, rc, 0 != rc);
    }

    // Verify that length longer than data leads to an error
    {
        // Write increased length
        encAsStr[LEN_POS] = static_cast<char>(ISOSTR_LEN + 1);

        // Verify that decoding now fails
        bdlsb::FixedMemInStreamBuf isb(encAsStr.data(), encAsStr.length());
        TIME_TYPE value;
        rc = decoder.decode(&isb, &value);
        ASSERTV(LINE, rc, 0 != rc);
    }

    // Verify that length longer than maximum won't read data
    {
        // Write absurd length
        encAsStr[LEN_POS] = static_cast<char>(MAX_LEN + 1);

        bdlsb::FixedMemInStreamBuf isb(encAsStr.data(), encAsStr.length());

        // Double-check that decoding fails
        TIME_TYPE value;
        rc = decoder.decode(&isb, &value);
        ASSERTV(LINE, rc, 0 != rc);

        // Verify that decoding did not read the data part at all
        ASSERTV(LINE, ISOSTR_LEN, isb.length(), ISOSTR_LEN == isb.length());
    }

    // Verify that with length exactly at maximum we read the data
    {
        encAsStr[LEN_POS] = static_cast<char>(MAX_LEN);

        bdlsb::FixedMemInStreamBuf isb(encAsStr.data(), encAsStr.length());

        // Decoding fails because we don't have enough data
        TIME_TYPE value;
        rc = decoder.decode(&isb, &value);
        ASSERTV(LINE, rc, 0 != rc);

        // Verify that decoding read all the available data
        ASSERTV(LINE, isb.length(), 0 == isb.length());
    }
}

// ============================================================================
//                     GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -MR01

namespace BloombergLP {
namespace s_baltst {

                      // ===============================
                      // class MyChoiceWithDefaultValues
                      // ===============================

class MyChoiceWithDefaultValues {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< int >                  d_selection0;
        bsls::ObjectBuffer< bsl::string >          d_selection1;
        bsls::ObjectBuffer< MyEnumeration::Value > d_selection2;
    };

    int                                            d_selectionId;
    bslma::Allocator                              *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED  = -1
      , SELECTION_ID_SELECTION0 = 0
      , SELECTION_ID_SELECTION1 = 1
      , SELECTION_ID_SELECTION2 = 2
    };

    enum {
        NUM_SELECTIONS = 3
    };

    enum {
        SELECTION_INDEX_SELECTION0 = 0
      , SELECTION_INDEX_SELECTION1 = 1
      , SELECTION_INDEX_SELECTION2 = 2
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const int DEFAULT_INITIALIZER_SELECTION0;

    static const char DEFAULT_INITIALIZER_SELECTION1[];

    static const MyEnumeration::Value DEFAULT_INITIALIZER_SELECTION2;

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS

    /// Return selection information for the selection indicated by the
    /// specified `id` if the selection exists, and 0 otherwise.
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);

    /// Return selection information for the selection indicated by the
    /// specified `name` of the specified `nameLength` if the selection
    /// exists, and 0 otherwise.
    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type `MyChoiceWithDefaultValues` having the
    /// default value.  Use the optionally specified `basicAllocator` to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    explicit MyChoiceWithDefaultValues(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `MyChoiceWithDefaultValues` having the
    /// value of the specified `original` object.  Use the optionally
    /// specified `basicAllocator` to supply memory.  If `basicAllocator` is
    /// 0, the currently installed default allocator is used.
    MyChoiceWithDefaultValues(const MyChoiceWithDefaultValues& original,
                             bslma::Allocator *basicAllocator = 0);

    /// Destroy this object.
    ~MyChoiceWithDefaultValues();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    MyChoiceWithDefaultValues& operator=(const MyChoiceWithDefaultValues& rhs);

    /// Reset this object to the default value (i.e., its value upon default
    /// construction).
    void reset();

    /// Set the value of this object to be the default for the selection
    /// indicated by the specified `selectionId`.  Return 0 on success, and
    /// non-zero value otherwise (i.e., the selection is not found).
    int makeSelection(int selectionId);

    /// Set the value of this object to be the default for the selection
    /// indicated by the specified `name` of the specified `nameLength`.
    /// Return 0 on success, and non-zero value otherwise (i.e., the
    /// selection is not found).
    int makeSelection(const char *name, int nameLength);

    /// Set the value of this object to be a "Selection0" value.  Optionally
    /// specify the `value` of the "Selection0".  If `value` is not
    /// specified, the default "Selection0" value is used.
    int& makeSelection0();
    int& makeSelection0(int value);

    /// Set the value of this object to be a "Selection1" value.  Optionally
    /// specify the `value` of the "Selection1".  If `value` is not
    /// specified, the default "Selection1" value is used.
    bsl::string& makeSelection1();
    bsl::string& makeSelection1(const bsl::string& value);

    /// Set the value of this object to be a "Selection2" value.  Optionally
    /// specify the `value` of the "Selection2".  If `value` is not
    /// specified, the default "Selection2" value is used.
    MyEnumeration::Value& makeSelection2();
    MyEnumeration::Value& makeSelection2(MyEnumeration::Value value);

    /// Invoke the specified `manipulator` on the address of the modifiable
    /// selection, supplying `manipulator` with the corresponding selection
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if this object has a defined selection,
    /// and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);

    /// Return a reference to the modifiable "Selection0" selection of this
    /// object if "Selection0" is the current selection.  The behavior is
    /// undefined unless "Selection0" is the selection of this object.
    int& selection0();

    /// Return a reference to the modifiable "Selection1" selection of this
    /// object if "Selection1" is the current selection.  The behavior is
    /// undefined unless "Selection1" is the selection of this object.
    bsl::string& selection1();

    /// Return a reference to the modifiable "Selection2" selection of this
    /// object if "Selection2" is the current selection.  The behavior is
    /// undefined unless "Selection2" is the selection of this object.
    MyEnumeration::Value& selection2();

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

    /// Return the id of the current selection if the selection is defined,
    /// and -1 otherwise.
    int selectionId() const;

    /// Invoke the specified `accessor` on the non-modifiable selection,
    /// supplying `accessor` with the corresponding selection information
    /// structure.  Return the value returned from the invocation of
    /// `accessor` if this object has a defined selection, and -1 otherwise.
    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;

    /// Return a reference to the non-modifiable "Selection0" selection of
    /// this object if "Selection0" is the current selection.  The behavior
    /// is undefined unless "Selection0" is the selection of this object.
    const int& selection0() const;

    /// Return a reference to the non-modifiable "Selection1" selection of
    /// this object if "Selection1" is the current selection.  The behavior
    /// is undefined unless "Selection1" is the selection of this object.
    const bsl::string& selection1() const;

    /// Return a reference to the non-modifiable "Selection2" selection of
    /// this object if "Selection2" is the current selection.  The behavior
    /// is undefined unless "Selection2" is the selection of this object.
    const MyEnumeration::Value& selection2() const;

    /// Return `true` if the value of this object is a "Selection0" value,
    /// and return `false` otherwise.
    bool isSelection0Value() const;

    /// Return `true` if the value of this object is a "Selection1" value,
    /// and return `false` otherwise.
    bool isSelection1Value() const;

    /// Return `true` if the value of this object is a "Selection2" value,
    /// and return `false` otherwise.
    bool isSelection2Value() const;

    /// Return `true` if the value of this object is undefined, and `false`
    /// otherwise.
    bool isUndefinedValue() const;

    /// Return the symbolic name of the current selection of this object.
    const char *selectionName() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  Two `MyChoiceWithDefaultValues` objects
/// have the same value if either the selections in both objects have the
/// same ids and the same values, or both selections are undefined.
inline
bool operator==(const MyChoiceWithDefaultValues& lhs,
                const MyChoiceWithDefaultValues& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the
/// same values, as determined by `operator==`, and `false` otherwise.
inline
bool operator!=(const MyChoiceWithDefaultValues& lhs,
                const MyChoiceWithDefaultValues& rhs);

/// Format the specified `rhs` to the specified output `stream` and
/// return a reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream&                   stream,
                        const MyChoiceWithDefaultValues& rhs);

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
                                               test::MyChoiceWithDefaultValues)

namespace s_baltst {

                      // -------------------------------
                      // class MyChoiceWithDefaultValues
                      // -------------------------------

// CREATORS
inline
MyChoiceWithDefaultValues::MyChoiceWithDefaultValues(
                                              bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
MyChoiceWithDefaultValues::~MyChoiceWithDefaultValues()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int MyChoiceWithDefaultValues::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case MyChoiceWithDefaultValues::SELECTION_ID_SELECTION0:
        return manipulator(&d_selection0.object(),                    // RETURN
                           SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION0]);
      case MyChoiceWithDefaultValues::SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),                    // RETURN
                           SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case MyChoiceWithDefaultValues::SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),                    // RETURN
                           SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      default:
        BSLS_ASSERT(MyChoiceWithDefaultValues::SELECTION_ID_UNDEFINED ==
                                                                d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
int& MyChoiceWithDefaultValues::selection0()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION0 == d_selectionId);
    return d_selection0.object();
}

inline
bsl::string& MyChoiceWithDefaultValues::selection1()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
MyEnumeration::Value& MyChoiceWithDefaultValues::selection2()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

// ACCESSORS
inline
int MyChoiceWithDefaultValues::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int MyChoiceWithDefaultValues::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION0:
        return accessor(d_selection0.object(),                        // RETURN
                        SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION0]);
      case SELECTION_ID_SELECTION1:
        return accessor(d_selection1.object(),                        // RETURN
                        SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),                        // RETURN
                        SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
const int& MyChoiceWithDefaultValues::selection0() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION0 == d_selectionId);
    return d_selection0.object();
}

inline
const bsl::string& MyChoiceWithDefaultValues::selection1() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const MyEnumeration::Value& MyChoiceWithDefaultValues::selection2() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
bool MyChoiceWithDefaultValues::isSelection0Value() const
{
    return SELECTION_ID_SELECTION0 == d_selectionId;
}

inline
bool MyChoiceWithDefaultValues::isSelection1Value() const
{
    return SELECTION_ID_SELECTION1 == d_selectionId;
}

inline
bool MyChoiceWithDefaultValues::isSelection2Value() const
{
    return SELECTION_ID_SELECTION2 == d_selectionId;
}

inline
bool MyChoiceWithDefaultValues::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

// CONSTANTS
const char MyChoiceWithDefaultValues::CLASS_NAME[] =
                                                   "MyChoiceWithDefaultValues";

const int MyChoiceWithDefaultValues::DEFAULT_INITIALIZER_SELECTION0 = 100;

const char MyChoiceWithDefaultValues::DEFAULT_INITIALIZER_SELECTION1[] =
                                                                     "default";

const MyEnumeration::Value
MyChoiceWithDefaultValues::DEFAULT_INITIALIZER_SELECTION2 =
                                                         MyEnumeration::VALUE2;

const bdlat_SelectionInfo MyChoiceWithDefaultValues::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION0,
        "selection0",
        sizeof("selection0") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        SELECTION_ID_SELECTION1,
        "selection1",
        sizeof("selection1") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        SELECTION_ID_SELECTION2,
        "selection2",
        sizeof("selection2") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS
const bdlat_SelectionInfo *MyChoiceWithDefaultValues::lookupSelectionInfo(
                                                        const char *name,
                                                        int         nameLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                            MyChoiceWithDefaultValues::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;                                    // RETURN
        }
    }

    return 0;
}

const bdlat_SelectionInfo *
MyChoiceWithDefaultValues::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SELECTION0:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION0];     // RETURN
      case SELECTION_ID_SELECTION1:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];     // RETURN
      case SELECTION_ID_SELECTION2:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];     // RETURN
      default:
        return 0;                                                     // RETURN
    }
}

// CREATORS
MyChoiceWithDefaultValues::MyChoiceWithDefaultValues(
                              const MyChoiceWithDefaultValues&  original,
                              bslma::Allocator                 *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION0: {
        new (d_selection0.buffer()) int(original.d_selection0.object());
      } break;
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer()) bsl::string(original.d_selection1.object(),
                                    d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer()) MyEnumeration::Value(
                                               original.d_selection2.object());
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

// MANIPULATORS
MyChoiceWithDefaultValues&
MyChoiceWithDefaultValues::operator=(const MyChoiceWithDefaultValues& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION0: {
            makeSelection0(rhs.d_selection0.object());
          } break;
          case SELECTION_ID_SELECTION1: {
            makeSelection1(rhs.d_selection1.object());
          } break;
          case SELECTION_ID_SELECTION2: {
            makeSelection2(rhs.d_selection2.object());
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}

void MyChoiceWithDefaultValues::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION0: {
        // no destruction required
      } break;
      case SELECTION_ID_SELECTION1: {
        typedef bsl::string Type;
        d_selection1.object().~Type();
      } break;
      case SELECTION_ID_SELECTION2: {
        typedef MyEnumeration::Value Type;
        d_selection2.object().~Type();
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int MyChoiceWithDefaultValues::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_SELECTION0: {
        makeSelection0();
      } break;
      case SELECTION_ID_SELECTION1: {
        makeSelection1();
      } break;
      case SELECTION_ID_SELECTION2: {
        makeSelection2();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int MyChoiceWithDefaultValues::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo = lookupSelectionInfo(name,
                                                                   nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

int& MyChoiceWithDefaultValues::makeSelection0()
{
    if (SELECTION_ID_SELECTION0 == d_selectionId) {
        d_selection0.object() = DEFAULT_INITIALIZER_SELECTION0;
    }
    else {
        reset();
        new (d_selection0.buffer()) int(DEFAULT_INITIALIZER_SELECTION0);
        d_selectionId = SELECTION_ID_SELECTION0;
    }

    return d_selection0.object();
}

int& MyChoiceWithDefaultValues::makeSelection0(int value)
{
    if (SELECTION_ID_SELECTION0 == d_selectionId) {
        d_selection0.object() = value;
    }
    else {
        reset();
        new (d_selection0.buffer()) int(value);
        d_selectionId = SELECTION_ID_SELECTION0;
    }

    return d_selection0.object();
}

bsl::string& MyChoiceWithDefaultValues::makeSelection1()
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = DEFAULT_INITIALIZER_SELECTION1;
    }
    else {
        reset();
        new (d_selection1.buffer()) bsl::string(DEFAULT_INITIALIZER_SELECTION1,
                                                d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

bsl::string&
MyChoiceWithDefaultValues::makeSelection1(const bsl::string& value)
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = value;
    }
    else {
        reset();
        new (d_selection1.buffer()) bsl::string(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

MyEnumeration::Value& MyChoiceWithDefaultValues::makeSelection2()
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = DEFAULT_INITIALIZER_SELECTION2;
    }
    else {
        reset();
        new (d_selection2.buffer()) MyEnumeration::Value(
                                               DEFAULT_INITIALIZER_SELECTION2);
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

MyEnumeration::Value&
MyChoiceWithDefaultValues::makeSelection2(MyEnumeration::Value value)
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = value;
    }
    else {
        reset();
        new (d_selection2.buffer()) MyEnumeration::Value(value);
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

// ACCESSORS
bsl::ostream&
MyChoiceWithDefaultValues::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION0: {
        printer.printAttribute("selection0", d_selection0.object());
      }  break;
      case SELECTION_ID_SELECTION1: {
        printer.printAttribute("selection1", d_selection1.object());
      }  break;
      case SELECTION_ID_SELECTION2: {
        printer.printAttribute("selection2", d_selection2.object());
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *MyChoiceWithDefaultValues::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION0:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION0].name();//RETURN
      case SELECTION_ID_SELECTION1:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1].name();//RETURN
      case SELECTION_ID_SELECTION2:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2].name();//RETURN
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";                                     // RETURN
    }
}

}  // close package namespace

namespace s_baltst {

                     // =================================
                     // class MySequenceWithDefaultValues
                     // =================================

class MySequenceWithDefaultValues {

    // INSTANCE DATA
    bsl::string           d_attribute1;
    int                   d_attribute0;
    MyEnumeration::Value  d_attribute2;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE0 = 0
      , ATTRIBUTE_ID_ATTRIBUTE1 = 1
      , ATTRIBUTE_ID_ATTRIBUTE2 = 2
    };

    enum {
        NUM_ATTRIBUTES = 3
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE0 = 0
      , ATTRIBUTE_INDEX_ATTRIBUTE1 = 1
      , ATTRIBUTE_INDEX_ATTRIBUTE2 = 2
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const int DEFAULT_INITIALIZER_ATTRIBUTE0;

    static const char DEFAULT_INITIALIZER_ATTRIBUTE1[];

    static const MyEnumeration::Value DEFAULT_INITIALIZER_ATTRIBUTE2;

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

    /// Create an object of type `MySequenceWithDefaultValues` having the
    /// default value.  Use the optionally specified `basicAllocator` to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    explicit MySequenceWithDefaultValues(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `MySequenceWithDefaultValues` having the
    /// value of the specified `original` object.  Use the optionally
    /// specified `basicAllocator` to supply memory.  If `basicAllocator` is
    /// 0, the currently installed default allocator is used.
    MySequenceWithDefaultValues(const MySequenceWithDefaultValues& original,
                                bslma::Allocator *basicAllocator = 0);

    /// Destroy this object.
    ~MySequenceWithDefaultValues();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    MySequenceWithDefaultValues&
    operator=(const MySequenceWithDefaultValues& rhs);

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

    /// Return a reference to the modifiable "Attribute0" attribute of this
    /// object.
    int& attribute0();

    /// Return a reference to the modifiable "Attribute1" attribute of this
    /// object.
    bsl::string& attribute1();

    /// Return a reference to the modifiable "Attribute2" attribute of this
    /// object.
    MyEnumeration::Value& attribute2();

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

    /// Return a reference to the non-modifiable "Attribute0" attribute of
    /// this object.
    int attribute0() const;

    /// Return a reference to the non-modifiable "Attribute1" attribute of
    /// this object.
    const bsl::string& attribute1() const;

    /// Return a reference to the non-modifiable "Attribute2" attribute of
    /// this object.
    MyEnumeration::Value attribute2() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` attribute objects have
/// the same value, and `false` otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline
bool operator==(const MySequenceWithDefaultValues& lhs,
                const MySequenceWithDefaultValues& rhs);

/// Return `true` if the specified `lhs` and `rhs` attribute objects do not
/// have the same value, and `false` otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline
bool operator!=(const MySequenceWithDefaultValues& lhs,
                const MySequenceWithDefaultValues& rhs);

/// Format the specified `rhs` to the specified output `stream` and
/// return a reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream&                     stream,
                        const MySequenceWithDefaultValues& rhs);

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
                                             test::MySequenceWithDefaultValues)

namespace s_baltst {

                     // ---------------------------------
                     // class MySequenceWithDefaultValues
                     // ---------------------------------

// MANIPULATORS
template <class MANIPULATOR>
int MySequenceWithDefaultValues::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute0,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int MySequenceWithDefaultValues::manipulateAttribute(MANIPULATOR& manipulator,
                                                     int          id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return manipulator(&d_attribute0,                             // RETURN
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1,                             // RETURN
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2,                             // RETURN
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
int MySequenceWithDefaultValues::manipulateAttribute(MANIPULATOR&  manipulator,
                                                     const char   *name,
                                                     int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo = lookupAttributeInfo(name,
                                                                   nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
int& MySequenceWithDefaultValues::attribute0()
{
    return d_attribute0;
}

inline
bsl::string& MySequenceWithDefaultValues::attribute1()
{
    return d_attribute1;
}

inline
MyEnumeration::Value& MySequenceWithDefaultValues::attribute2()
{
    return d_attribute2;
}

// ACCESSORS
template <class ACCESSOR>
int MySequenceWithDefaultValues::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute0,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute1,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int MySequenceWithDefaultValues::accessAttribute(ACCESSOR& accessor,
                                                 int       id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return accessor(d_attribute0,                                 // RETURN
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1,                                 // RETURN
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2,                                 // RETURN
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
int MySequenceWithDefaultValues::accessAttribute(ACCESSOR&   accessor,
                                                 const char *name,
                                                 int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo = lookupAttributeInfo(name,
                                                                   nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;                                              // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
int MySequenceWithDefaultValues::attribute0() const
{
    return d_attribute0;
}

inline
const bsl::string& MySequenceWithDefaultValues::attribute1() const
{
    return d_attribute1;
}

inline
MyEnumeration::Value MySequenceWithDefaultValues::attribute2() const
{
    return d_attribute2;
}

// CONSTANTS

const char MySequenceWithDefaultValues::CLASS_NAME[] =
                                                 "MySequenceWithDefaultValues";

const int MySequenceWithDefaultValues::DEFAULT_INITIALIZER_ATTRIBUTE0 = 100;

const char MySequenceWithDefaultValues::DEFAULT_INITIALIZER_ATTRIBUTE1[] =
                                                                     "default";

const MyEnumeration::Value
MySequenceWithDefaultValues::DEFAULT_INITIALIZER_ATTRIBUTE2 =
                                                         MyEnumeration::VALUE2;

const bdlat_AttributeInfo
MySequenceWithDefaultValues::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE0,
        "attribute0",
        sizeof("attribute0") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "attribute2",
        sizeof("attribute2") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS
const bdlat_AttributeInfo *
MySequenceWithDefaultValues::lookupAttributeInfo(const char *name,
                                                 int         nameLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithDefaultValues::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;                                    // RETURN
        }
    }

    return 0;
}

const bdlat_AttributeInfo *
MySequenceWithDefaultValues::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0];     // RETURN
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];     // RETURN
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];     // RETURN
      default:
        return 0;                                                     // RETURN
    }
}

// CREATORS

MySequenceWithDefaultValues::MySequenceWithDefaultValues(
                                              bslma::Allocator *basicAllocator)
: d_attribute1(DEFAULT_INITIALIZER_ATTRIBUTE1, basicAllocator)
, d_attribute0(DEFAULT_INITIALIZER_ATTRIBUTE0)
, d_attribute2(DEFAULT_INITIALIZER_ATTRIBUTE2)
{
}

MySequenceWithDefaultValues::MySequenceWithDefaultValues(
                            const MySequenceWithDefaultValues&  original,
                            bslma::Allocator                   *basicAllocator)
: d_attribute1(original.d_attribute1, basicAllocator)
, d_attribute0(original.d_attribute0)
, d_attribute2(original.d_attribute2)
{
}

MySequenceWithDefaultValues::~MySequenceWithDefaultValues()
{
}

// MANIPULATORS
MySequenceWithDefaultValues&
MySequenceWithDefaultValues::operator=(const MySequenceWithDefaultValues& rhs)
{
    if (this != &rhs) {
        d_attribute0 = rhs.d_attribute0;
        d_attribute1 = rhs.d_attribute1;
        d_attribute2 = rhs.d_attribute2;
    }

    return *this;
}

void MySequenceWithDefaultValues::reset()
{
    d_attribute0 = DEFAULT_INITIALIZER_ATTRIBUTE0;
    d_attribute1 = DEFAULT_INITIALIZER_ATTRIBUTE1;
    d_attribute2 = DEFAULT_INITIALIZER_ATTRIBUTE2;
}

// ACCESSORS

bsl::ostream&
MySequenceWithDefaultValues::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute0", d_attribute0);
    printer.printAttribute("attribute1", d_attribute1);
    printer.printAttribute("attribute2", d_attribute2);
    printer.end();
    return stream;
}

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
        cout << "should not be called\n";
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

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------
// ```
    namespace BloombergLP {
    namespace usage {

    /// This struct represents a sequence containing a `string` member, an
    /// `int` member, and a `float` member.
    struct EmployeeRecord {

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

        /// Create an `EmployeeRecord` having the attributes:
        /// ```
        /// d_name   == ""
        /// d_age    == 0
        /// d_salary = 0.0
        /// ```
        EmployeeRecord();

        /// Create an `EmployeeRecord` object having the specified
        /// `name`, `age`, and `salary` attributes.
        EmployeeRecord(const bsl::string& name, int age, float salary);

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

    }  // close namespace usage

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

    }  // close namespace usage

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
        struct IsSequence<usage::EmployeeRecord> : public bsl::true_type {
        };

    }  // close namespace bdlat_SequenceFunctions
    }  // close enterprise namespace

static void usageExample()
{
    using namespace BloombergLP;
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Decoding an Employee Record
/// - - - - - - - - - - - - - - - - - - -
// Suppose that an "employee record" consists of a sequence of attributes --
// `name`, `age`, and `salary` -- that are of types `bsl::string`, `int`, and
// `float`, respectively.  Furthermore, we have a need to BER encode employee
// records as a sequence of values (for out-of-process consumption).
//
// Assume that we have defined a `usage::EmployeeRecord` class to represent
// employee record values, and assume that we have provided the `bdlat`
// specializations that allow the `balber` codec components to represent class
// values as a sequence of BER primitive values.  See
// {`bdlat_sequencefunctions`|Usage} for details of creating specializations
// for a sequence type.
//
// First, we create an employee record object having typical values:
// ```
    usage::EmployeeRecord bob("Bob", 56, 1234.00);
    ASSERT("Bob"   == bob.name());
    ASSERT(  56    == bob.age());
    ASSERT(1234.00 == bob.salary());
// ```
// Next, we create a `balber::Encoder` object and use it to encode our `bob`
// object.  Here, to facilitate the examination of our results, the BER
// encoding data is delivered to a `bslsb::MemOutStreamBuf` object:
// ```
    bdlsb::MemOutStreamBuf osb;
    balber::BerEncoder     encoder;
    int                    rc = encoder.encode(&osb, bob);
    ASSERT( 0 == rc);
    ASSERT(18 == osb.length());
// ```
// Now, we create a `bdlsb::FixedMemInStreamBuf` object to manage our access
// to the data portion of the `bdlsb::MemOutStreamBuf` (where our BER encoding
// resides), decode the values found there, and use them to set the value
// of an `usage::EmployeeRecord` object.
// ```
    balber::BerDecoderOptions  options;
    balber::BerDecoder         decoder(&options);
    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
    usage::EmployeeRecord      obj;

    rc = decoder.decode(&isb, &obj);
    ASSERT(0 == rc);
// ```
// Finally, we confirm that the object defined by the BER encoding has the
// same value as the original object.
// ```
    ASSERT(bob.name()   == obj.name());
    ASSERT(bob.age()    == obj.age());
    ASSERT(bob.salary() == obj.salary());
// ```
}

// ============================================================================
//                              FUZZ TESTING
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The following function, `LLVMFuzzerTestOneInput`, is the entry point for the
// clang fuzz testing facility.  See {http://bburl/BDEFuzzTesting} for details
// on how to build and run with fuzz testing enabled.
//-----------------------------------------------------------------------------

#ifdef BDE_ACTIVATE_FUZZ_TESTING
#define main test_driver_main
#endif

/// Try to deserialize an object of the specified `TYPE` from the specified
/// span of bytes [`bytes`, `bytes` + `size`).  Return `true` on success.
template <class TYPE>
bool tryDeserialize(const uint8_t                    *bytes,
                    size_t                            size,
                    const balber::BerDecoderOptions&  options)
{
    balber::BerDecoder         decoder(&options);
    bdlsb::FixedMemInStreamBuf streamBuf(reinterpret_cast<const char*>(bytes),
                                         size);
    TYPE                       outValue;

    int rc = decoder.decode(&streamBuf, &outValue);
    return rc == 0;
}

/// Try to deserialize an object of the specified `TYPE` from the specified
/// `data`.
template <class TYPE>
inline
bool tryDeserialize(const bslim::FuzzDataView&       data,
                    const balber::BerDecoderOptions& options)
{
    return tryDeserialize<TYPE>(data.data(), data.length(), options);
}

extern "C"
/// Use the specified `bytes` array of `size` bytes as input to methods of
/// this component and return zero.
int LLVMFuzzerTestOneInput(const uint8_t *bytes, size_t size)
{
    typedef bslim::FuzzUtil FuzzUtil;
    bslim::FuzzDataView data(bytes, size);

    balber::BerDecoderOptions options;
    options.setMaxDepth(FuzzUtil::consumeNumberInRange<int>(&data, 3, 10));
    options.setMaxSequenceSize(FuzzUtil::consumeNumberInRange<int>(
                        &data,
                        1,
                        balber::BerDecoderOptions::DEFAULT_MAX_SEQUENCE_SIZE));
    options.setSkipUnknownElements(FuzzUtil::consumeBool(&data));
    options.setDefaultEmptyStrings(FuzzUtil::consumeBool(&data));

    using BloombergLP::s_baltst::Request;
    tryDeserialize<Request>(data, options);

    if (testStatus > 0) {
        BSLS_ASSERT_INVOKE("FUZZ TEST FAILURES");
    }
    return 0;
}

void reproduceFuzzTest(const char *hexDump)
{
    bsl::vector<char> testData = loadFromHex(hexDump);
    LLVMFuzzerTestOneInput(reinterpret_cast<const uint8_t *>(testData.data()),
                           testData.size());
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

    bslma::TestAllocator ta("testAllocator", veryVeryVerbose);
    bslma::TestAllocatorMonitor tam(&ta);

    // CONCERN: `BSLS_REVIEW` failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    balber::BerEncoderOptions encodeALHOptions;
    encodeALHOptions.setEncodeArrayLengthHints(true);

    balber::BerEncoder encoder(0);
    balber::BerEncoder encoderALH(&encodeALHOptions);

    balber::BerDecoderOptions options;
    if (veryVeryVerbose) {
       options.setTraceLevel(1);
    }

    balber::BerDecoder decoder(&options);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 26: {
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

        if (verbose) cout << "\nUSAGE EXAMPLE"
                             "\n=============\n";
        usageExample();

        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // MAXDEPTH IS RESPECTED
        //
        // Concerns:
        // 1. The `maxDepth` configuration option is repected.
        //
        // Plan:
        // 1. Manually determine the `maxDepth` value required for various
        //    messages in a table-driven technique.
        //
        // 2. Using those messages, ensure that decoding fails if the
        //    `maxDepth` configuration option is too low, and succeeds when it
        //    is correct.
        //
        // Testing:
        //   `option.setMaxDepth()`
        // --------------------------------------------------------------------

        if (verbose) cout
             << "\nMAXDEPTH IS RESPECTED"
             << "\n====================="
             << endl;

        balxml::MiniReader     xml_reader;
        balxml::ErrorInfo      e;

        typedef s_baltst::DepthTestMessage DepthTestMessage;
        typedef s_baltst::DepthTestMessageUtil DTMU;

        for (int i = 0; i < DTMU::k_NUM_MESSAGES; ++i) {
            balb::FeatureTestMessage object;

            const DepthTestMessage& TEST_MESSAGE = DTMU::s_TEST_MESSAGES[i];

            const char *XML   = TEST_MESSAGE.d_XML_text_p;
            const int   LINE  = i;
            const int   DEPTH = TEST_MESSAGE.d_depthBER;

            bsl::istringstream ss(XML);
            balxml::DecoderOptions xml_options;
            xml_options.setSkipUnknownElements(true);
            balxml::Decoder xml_decoder(&xml_options, &xml_reader, &e);

            int rc = xml_decoder.decode(ss.rdbuf(), &object);
            if (0 != rc) {
                cout << "Failed to decode from initialization data (i="
                     << i << ", LINE=" << LINE
                     << "): " << xml_decoder.loggedMessages() << endl;
            }
            if (balb::FeatureTestMessage::SELECTION_ID_UNDEFINED ==
                object.selectionId()) {
                cout
                    << "Decoded unselected choice from initialization data"
                    << " (LINE =" << LINE
                    << "):" << endl;
                rc = 9;
            }

            bsl::stringstream ber_stream;
            balber::BerEncoder   encoder;

            ASSERTV(LINE, encoder.loggedMessages(),
                    0 == encoder.encode(ber_stream, object));
            bsl::string ber_payload = ber_stream.str();

            // Set depth too low, expect failure
            {
                int depth = DEPTH - 2;
                ber_stream.str(ber_payload);
                balber::BerDecoderOptions options;

                options.setMaxDepth(depth);
                options.setTraceLevel(1);

                balber::BerDecoder decoder(&options);
                balb::FeatureTestMessage ber_object;

                int rc = decoder.decode(ber_stream.rdbuf(),
                                        &ber_object);

                if (0 == rc) {
                    int delta = DEPTH - depth;

                    cout << "Unexpected success for ";
                    P_(LINE);
                    P_(DEPTH);
                    P_(depth);
                    P(delta);

                    if (verbose) {
                        cout << "<<XML\n" << XML << ">>\n";
                        cout << "<<BER\n"
                             << decoder.loggedMessages() << ">>\n";
                    }
                }
            }

            // Set depth correctly, expect success
            {
                ber_stream.str(ber_payload);
                balber::BerDecoderOptions options;

                options.setMaxDepth(DEPTH);
                options.setTraceLevel(1);

                balber::BerDecoder decoder(&options);

                balb::FeatureTestMessage ber_object;

                int rc = decoder.decode(ber_stream.rdbuf(),
                                        &ber_object);
                if (0 != rc) {
                    cout << "Unexpectedly failed to decode from"
                         << " initialization data (i="
                         << i << ", LINE=" << LINE
                         << "): " << decoder.loggedMessages() << endl;
                }
                if (balb::FeatureTestMessage::SELECTION_ID_UNDEFINED ==
                    object.selectionId()) {
                    cout
                        << "Decoded unselected choice from initialization data"
                        << " (LINE =" << LINE
                        << "):" << endl;
                    rc = 9;
                }

                ASSERTV(object, ber_object, object == ber_object);
                ASSERTV(LINE, DEPTH, options.maxDepth(), rc, 0 == rc);
            }
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // FUZZ TEST BUG (DRQS 175741365)
        //
        // Concerns:
        // 1. The input sample mustn't cause a crash.
        //
        // Plan:
        // 1. Reproduce the failing fuzz test.
        //
        // Testing:
        //   FUZZ TEST BUG (DRQS 175741365)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nFUZZ TEST BUG (DRQS 175741365)"
                             "\n==============================\n";

        reproduceFuzzTest("3dc8ff4c 01230a30 31bf0010 bf0100bf"
                          "0010a710 bf0010bf 00109f09 0000001a"
                          "1000bf00 10bf0600 ffff05ff ffffffff"
                          "ffffffff ffffffff ffffffff ffffffff"
                          "ffffffff ffffffff ffffffff ffffffff"
                          "ffffffff ff10ff");
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // FUZZ TEST BUG (DRQS 175594554)
        //
        // Concerns:
        // 1. The input sample mustn't cause a crash.
        //
        // Plan:
        // 1. Reproduce the failing fuzz test.
        //
        // Testing:
        //   FUZZ TEST BUG (DRQS 175594554)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nFUZZ TEST BUG (DRQS 175594554)"
                             "\n==============================\n";

        reproduceFuzzTest("2a0a353d ff87ff30 5da05da1 00a024a1 10000031 00");
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // DECODE DATE/TIME WITH LENGTH ANOMALIES
        //
        // Concerns:
        // 1. Length above 127 characters results in an error regardless of the
        //    content.
        //
        // 2. Length precisely at 127 characters is decoded as usual.
        //
        // 3. Length beyond available data results in an error regardless of
        //    the available content.
        //
        // Plan:
        // 1. Table based test for each sub-type.
        //
        // 2. For each valid type for a test data ISO 8601 string:
        //   1. Encoded input is generated from the string
        //
        //   2. Encoded input is then manipulated to change the length for
        //      concerns that require a length different from the size of
        //      available data
        //
        //   3. Decoding return value is verified using assertions
        //
        // Testing:
        //   DECODE DATE/TIME WITH LENGTH ANOMALIES
        // --------------------------------------------------------------------

        if (verbose) cout << "\nDECODE DATE/TIME WITH LENGTH ANOMALIES"
                             "\n======================================\n";

        // Kept at 126 for "+1" to fit in one byte, BER-encoded
        const int MAX_LEN = 126;

        if (verbose) cout << "\t`bdlt::Date`/`DateTz`\n";
        {
            // Create the encoder that encodes date as ISO 8601 strings
            balber::BerEncoderOptions mOptions;
            mOptions.setEncodeDateAndTimeTypesAsBinary(false);
            const balber::BerEncoderOptions OPTS = mOptions;
            balber::BerEncoder encoder(&OPTS);

            using bdlt::Date;
            using bdlt::DateTz;

            static struct {
                int  d_line;
                Date d_date;
                int  d_offset;
            } TEST_DATA[] = {
                { L_, Date(2024,5,15), 120 },
                { L_, Date(2042,2,28),   0 },
            };

            const bsl::size_t TEST_SIZE = sizeof TEST_DATA / sizeof *TEST_DATA;

            for (bsl::size_t ti = 0; ti < TEST_SIZE; ++ti) {
                const int   LINE   = TEST_DATA[ti].d_line;
                const Date& DATE   = TEST_DATA[ti].d_date;
                const int   OFFSET = TEST_DATA[ti].d_offset;

                if (veryVeryVerbose) { P_(LINE) P_(DATE) P(OFFSET); }

                {
                    // Create a complete & valid encoded `Date` octets
                    bdlsb::MemOutStreamBuf buff;
                    int rc = encoder.encode(&buff, DATE);
                    ASSERTV(rc, 0 == rc);

                    const bsl::string_view encoded(buff.data(), buff.length());

                    const bsl::size_t DATE_LEN = 10; // YYYY-MM-DD
                    verifyDateTypesLengthAnomalies(MAX_LEN,
                                                   DATE_LEN,
                                                   LINE,
                                                   DATE,
                                                   encoded);
                }

                {
                    // Create a complete & valid encoded `DateTz` octets
                    const DateTz DATETZ(DATE, OFFSET);

                    bdlsb::MemOutStreamBuf buff;
                    int rc = encoder.encode(&buff, DATETZ);
                    ASSERTV(rc, 0 == rc);

                    const bsl::string_view encoded(buff.data(), buff.length());

                    const bsl::size_t DATETZ_LEN = 16; // YYYY-MM-DD+HH:MM
                    verifyDateTypesLengthAnomalies(MAX_LEN,
                                                   DATETZ_LEN,
                                                   LINE,
                                                   DATETZ,
                                                   encoded);
                }
            }
        }

        if (verbose) cout << "\t`bdlt::Time`/`TimeTz`\n";
        {
            // Create the encoder that encodes times as ISO 8601 strings
            balber::BerEncoderOptions mOptions;
            mOptions.setEncodeDateAndTimeTypesAsBinary(false);
            const bsl::size_t SEC_FRAC_PREC = 6;
            mOptions.setDatetimeFractionalSecondPrecision(SEC_FRAC_PREC);
            const balber::BerEncoderOptions OPTS = mOptions;
            balber::BerEncoder encoder(&OPTS);

            using bdlt::Time;
            using bdlt::TimeTz;

            static struct {
                int  d_line;
                Time d_time;
                int  d_offset;
            } TEST_DATA[] = {
                { L_, Time(12,21,42,999,999), 120 },
                { L_, Time(12,21,42,999,999),   0 },
            };

            const bsl::size_t TEST_SIZE = sizeof TEST_DATA / sizeof *TEST_DATA;

            for (bsl::size_t ti = 0; ti < TEST_SIZE; ++ti) {
                const int   LINE   = TEST_DATA[ti].d_line;
                const Time& TIME   = TEST_DATA[ti].d_time;
                const int   OFFSET = TEST_DATA[ti].d_offset;

                if (veryVeryVerbose) { P_(LINE) P_(TIME) P(OFFSET); }

                {
                    // Create a complete & valid encoded `Date` octets
                    bdlsb::MemOutStreamBuf buff;
                    int rc = encoder.encode(&buff, TIME);
                    ASSERTV(rc, 0 == rc);

                    const bsl::string_view encoded(buff.data(), buff.length());

                    const bsl::size_t BASE_LEN = 8; // hh:mm:ss       [.ssssss]
                    const bsl::size_t TIME_LEN = BASE_LEN + SEC_FRAC_PREC + 1;
                    verifyTimeTypesLengthAnomalies(MAX_LEN,
                                                   TIME_LEN,
                                                   SEC_FRAC_PREC,
                                                   LINE,
                                                   TIME,
                                                   encoded);
                }

                {
                    // Create a complete & valid encoded `TimeTz` octets
                    const TimeTz TIMETZ(TIME, OFFSET);

                    bdlsb::MemOutStreamBuf buff;
                    int rc = encoder.encode(&buff, TIMETZ);
                    ASSERTV(rc, 0 == rc);

                    const bsl::string_view encoded(buff.data(), buff.length());

                    const bsl::size_t BASE_LEN = 8; // hh:mm:ss       [.ssssss]
                    const bsl::size_t TIME_LEN = BASE_LEN + SEC_FRAC_PREC + 1;
                    const bsl::size_t TZ_LEN = 6; // +HH:MM
                    verifyTimeTypesLengthAnomalies(MAX_LEN,
                                                   TIME_LEN + TZ_LEN,
                                                   1,
                                                   LINE,
                                                   TIMETZ,
                                                   encoded);
                }
            }
        }

        if (verbose) cout << "\t`bdlt::Datetime`/`DatetimeTz`\n";
        {
            // Create the encoder that encodes datetimes as ISO 8601 strings
            balber::BerEncoderOptions mOptions;
            mOptions.setEncodeDateAndTimeTypesAsBinary(false);
            const bsl::size_t SEC_FRAC_PREC = 6;
            mOptions.setDatetimeFractionalSecondPrecision(SEC_FRAC_PREC);
            const balber::BerEncoderOptions OPTS = mOptions;
            balber::BerEncoder encoder(&OPTS);

            using bdlt::Datetime;
            using bdlt::DatetimeTz;

            static struct {
                int      d_line;
                Datetime d_datetime;
                int      d_offset;
            } TEST_DATA[] = {
                { L_, Datetime(2024,5,15,12,21,42,999,999), 120 },
                { L_, Datetime(2024,5,15,12,21,42,999,999),   0 },
            };

            const bsl::size_t TEST_SIZE = sizeof TEST_DATA / sizeof *TEST_DATA;

            for (bsl::size_t ti = 0; ti < TEST_SIZE; ++ti) {
                const int       LINE     = TEST_DATA[ti].d_line;
                const Datetime& DATETIME = TEST_DATA[ti].d_datetime;
                const int       OFFSET   = TEST_DATA[ti].d_offset;

                if (veryVeryVerbose) { P_(LINE) P_(DATETIME) P(OFFSET); }

                {
                    // Create a complete & valid encoded `Date` octets
                    bdlsb::MemOutStreamBuf buff;
                    int rc = encoder.encode(&buff, DATETIME);
                    ASSERTV(rc, 0 == rc);

                    const bsl::string_view encoded(buff.data(), buff.length());

                    const bsl::size_t BASE_LEN = 19; // YYYY-MM-DD+hh:mm:ss
                    const bsl::size_t DT_LEN   = BASE_LEN + SEC_FRAC_PREC + 1;
                                                                     // .ssssss
                    verifyTimeTypesLengthAnomalies(MAX_LEN,
                                                   DT_LEN,
                                                   SEC_FRAC_PREC,
                                                   LINE,
                                                   DATETIME,
                                                   encoded);
                }

                {
                    // Create a complete & valid encoded `TimeTz` octets
                    const DatetimeTz DATETIMETZ(DATETIME, OFFSET);

                    bdlsb::MemOutStreamBuf buff;
                    int rc = encoder.encode(&buff, DATETIMETZ);
                    ASSERTV(rc, 0 == rc);

                    const bsl::string_view encoded(buff.data(), buff.length());

                    const bsl::size_t BASE_LEN = 19; // YYYY-MM-DD+hh:mm:ss
                    const bsl::size_t DT_LEN   = BASE_LEN + SEC_FRAC_PREC + 1;
                                                                     // .ssssss
                    const bsl::size_t TZ_LEN   = 6; // +HH:MM
                    verifyTimeTypesLengthAnomalies(MAX_LEN,
                                                   DT_LEN + TZ_LEN,
                                                   1,
                                                   LINE,
                                                   DATETIMETZ,
                                                   encoded);
                }
            }
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // DECODE INTS AS ENUMS AND VICE VERSA
        //
        // Concerns:
        // 1. The encoding produced by `balber_berencoder` when encoding an
        //    integer enumeration can be decoded into either a plain integral
        //    type or a customized type (as produced by bas_codegen) whose base
        //    type is integral.
        //
        // 2. The encoding produced by `balber_berencoder` when encoding either
        //    a plain integral type or a customized type (as produced by
        //    bas_codegen) whose base type is integral can be decoded into an
        //    integer enumeration type.
        //
        // Plan:
        // 1. Define a type, `Enumeration1`, that is a `bdlat` enumeration, can
        //    hold an integer value of either 0 or 1, and whose string
        //    representation is just the decimal form of its value (as with
        //    `bcem_Aggregate`).
        //
        // 2. Using `balber_berencoder`, encode objects of type `Enumeration1`
        //    having values of 0 and 1, and decode them into plain `int`s.
        //    Verify that the resulting values are the same as the original
        //    values.  Then, repeat using the generated type
        //    `test::MyIntEnumeration`.  (C-1)
        //
        // 3. Using `balber_berencoder`, encode plain `int`s having values of 0
        //    and 1, decode them into `Enumeration1`, and verify that the
        //    resulting values are the same as the original values.  Then,
        //    repeat using the generated type `test::MyIntEnumeration`.  (C-2)
        //
        // Testing:
        //   DECODE INTS AS ENUMS AND VICE VERSA
        // --------------------------------------------------------------------

        if (verbose) cout << "\nDECODE INTS AS ENUMS AND VICE VERSA"
                             "\n===================================\n";

        balber::BerEncoder encoder;
        balber::BerDecoder decoder;

        // P-1
        for (int i = 0; i < 2; i++) {
            if (veryVerbose) P(i);

            test::Enumeration1 e;
            ASSERT(0 == bdlat_enumFromInt(&e, i));

            bdlsb::MemOutStreamBuf osb1;
            ASSERT(0 == encoder.encode(&osb1, e));
            bdlsb::FixedMemInStreamBuf isb1(osb1.data(), osb1.length());
            int x;
            ASSERT(0 == decoder.decode(&isb1, &x));
            ASSERT(i == x);

            bdlsb::MemOutStreamBuf osb2;
            ASSERT(0 == encoder.encode(&osb2, e));
            bdlsb::FixedMemInStreamBuf isb2(osb2.data(), osb2.length());
            test::MyIntEnumeration c;
            ASSERT(0 == decoder.decode(&isb2, &c));
            ASSERT(test::MyIntEnumeration(i) == c);
        }

        // P-2
        for (int i = 0; i < 2; i++) {
            if (veryVerbose) P(i);

            test::Enumeration1 e;
            int                value;

            bdlsb::MemOutStreamBuf osb1;
            ASSERT(0 == encoder.encode(&osb1, i));
            bdlsb::FixedMemInStreamBuf isb1(osb1.data(), osb1.length());
            ASSERT(0 == decoder.decode(&isb1, &e));
            bdlat_enumToInt(&value, e);
            ASSERT(i == value);

            bdlsb::MemOutStreamBuf osb2;
            ASSERT(0 == encoder.encode(&osb2, test::MyIntEnumeration(i)));
            bdlsb::FixedMemInStreamBuf isb2(osb2.data(), osb2.length());
            ASSERT(0 == decoder.decode(&isb2, &e));
            bdlat_enumToInt(&value, e);
            ASSERT(i == value);
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // DECODE SEQUENCES OF MAXIMUM SIZE
        //   Ensure that decoding sequences smaller than the configured maximum
        //   sequence size succeeds and decoding sequences larger than the
        //   configured maximum sequence size fails.
        //
        // Concerns:
        // 1. Decoding an empty `vector<char>` with a degerate maximum decoded
        //    sequence size of 0 successfully decodes the empty vector.
        //
        // 2. Decoding a single-element `vector<char>` with a degenerate
        //    maximum decoded sequence size of 0 fails.
        //
        // 3. Decoding en empty `vector<char>` with a maximum decoded sequence
        //    size of 1 successfully decodes the empty vector.
        //
        // 4. Decoding an single-element `vector<char>` with a maximum decoded
        //    sequence size of 1 successfully decodes the vector.
        //
        // 5. For various maximum decoded sequence sizes, decoding a
        //    `vector<char>` less than or exactly equal to that size succeeds.
        //
        // 6. For various maximum decoded sequence sizes, decoding a
        //    `vector<char>` of a greater size fails.
        //
        // Plan:
        // 1. For maximum sequence sizes of 0, 1, 1 Megabyte, and 1 Gigabyte,
        //    attempt to decode a `vector<char>` of 1 less than, exactly equal
        //    to, and 1 greater than the maximum sequence size.
        //
        // 2. Verify that decoding vectors of sizes less than or equal to the
        //    configured maximum sequence size succeeds.
        //
        // 3. Verify that decoding vectors of sizes greater than the configured
        //    maximum sequence size fails.
        //
        // Testing:
        //   int decode(bsl::streambuf *streamBuf, TYPE *variable);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nDECODE SEQUENCES OF MAXIMUM SIZE"
                             "\n================================\n";

        // First, ensure that the default maximum sequence size is 1GB.  This
        // assertion will need to be changed as the default value for the
        // maximum sequence size in `balber::BerDecoderOptions` changes.

        balber::BerDecoderOptions options;
        ASSERT(1 * 1024 * 1024 * 1024 == options.maxSequenceSize());

        static const int DEFAULT_MAX_SIZE = -1;

        static const struct {
            int  d_lineNum;   // source line number
            int  d_maxSize;   // maximum sequence size to allow for decoding
            int  d_size;      // size of data to encode and decode
            bool d_canDecode; // flag indicating if decoding should succeed
        } DATA[] = {
            // Line Max Sequence Size        Sequence Size        Can Decode
            // ---- ----------------- --------------------------- ----------
            {    L_,                0,                          0, true     },
            {    L_,                0,                          1, false    },
            {    L_,                0,        1 * 1024 * 1024 - 1, false    },
            {    L_,                0,        1 * 1024 * 1024    , false    },
            {    L_,                0,        1 * 1024 * 1024 + 1, false    },
            {    L_,                1,                          0, true     },
            {    L_,                1,                          1, true     },
            {    L_,                1,        1 * 1024 * 1024 - 1, false    },
            {    L_,                1,        1 * 1024 * 1024    , false    },
            {    L_,                1,        1 * 1024 * 1024 + 1, false    },
            {    L_,  1 * 1024 * 1024,                          0, true     },
            {    L_,  1 * 1024 * 1024,                          1, true     },
            {    L_,  1 * 1024 * 1024,        1 * 1024 * 1024 - 1, true     },
            {    L_,  1 * 1024 * 1024,        1 * 1024 * 1024    , true     },
            {    L_,  1 * 1024 * 1024,        1 * 1024 * 1024 + 1, false    },
            {    L_,  1 * 1024 * 1024,       32 * 1024 * 1024 - 1, false    },
            {    L_,  1 * 1024 * 1024,       32 * 1024 * 1024    , false    },
            {    L_,  1 * 1024 * 1024,       32 * 1024 * 1024 + 1, false    },
            {    L_, DEFAULT_MAX_SIZE,                          0, true     },
            {    L_, DEFAULT_MAX_SIZE,                          1, true     },
            {    L_, DEFAULT_MAX_SIZE,       32 * 1024 * 1024 - 1, true     },
            {    L_, DEFAULT_MAX_SIZE,       32 * 1024 * 1024    , true     },
            {    L_, DEFAULT_MAX_SIZE,       32 * 1024 * 1024 + 1, true     },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i != NUM_DATA; ++i) {
            const int  LINE       = DATA[i].d_lineNum;
            const int  MAX_SIZE   = DATA[i].d_maxSize;
            const int  SIZE       = DATA[i].d_size;
            const bool CAN_DECODE = DATA[i].d_canDecode;

            if (veryVeryVerbose) {
                P_(LINE) P_(MAX_SIZE) P_(SIZE) P(CAN_DECODE)
            }

            const bsl::vector<char> DATA(SIZE, '\xAB');

            bdlsb::MemOutStreamBuf osb;
            balber::BerEncoder encoder;

            int rc = encoder.encode(&osb, DATA);
            ASSERT(0 == rc);

            balber::BerDecoderOptions decoderOptions;
            if (MAX_SIZE != DEFAULT_MAX_SIZE) {
                decoderOptions.setMaxSequenceSize(MAX_SIZE);
            }

            balber::BerDecoder decoder(&decoderOptions);

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());

            if (CAN_DECODE) {
                bsl::vector<char> decodedData;
                rc = decoder.decode(&isb, &decodedData);
                ASSERT(0 == rc);
                ASSERT(DATA == decodedData);
            }
            else {
                bsl::vector<char> decodedData;
                rc = decoder.decode(&isb, &decodedData);
                ASSERT(0 != rc);
            }
        }
        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // DECODE COMPLEX TYPES WITH DEFAULT ELEMENTS
        //
        // Concerns:
        // 1. The "DefaultEmptyStrings" decoder option instructs a BER decoder
        //    to load the default value of elements only when it is true,
        //    the decoder reads an empty string, and the attribute has a
        //    default string value.
        //
        // 2. The "DefaultEmptyStrings" decoder option does not affect whether
        //    or not a BER decoder loads the default value of non-string
        //    types.
        //
        // 3. The "DefaultEmptyStrings" decoder option applies to elements of
        //    both sequences and choices.
        //
        // Plan:
        // 1. Create a choice type having an integer, string, and enumeration
        //    selection, each having a default value.
        //
        // 2. Perform a depth-ordered enumeration of encoding and then
        //    decoding each permutation of an instance of the choice type
        //    having a zero, unset, default, or arbitary value.
        //
        // 3. Validate that if the encoded object was an empty string
        //    selection, the decoded object is loaded with the default value
        //    if the "DefaultEmptyStrings" decoder option is set, and otherwise
        //    the decoded object is loaded with an empty string value.
        //
        // 4. Create a sequence type having integer, string, and enumeration
        //    attributes, each having a default value.
        //
        // 5. Perform a locally depth-ordered enumeration of encoding and
        //    an instance of the sequence type, testing each attribute in
        //    isolation.
        //
        // 6. For each attribute test, validate that the if the string
        //    attribute of the encoded object was empty, the decoded object
        //    is loaded with the default value if the "DefaultEmptyStrings"
        //    decoder option is set, and otherwise the decoded attribute is
        //    loaded with an empty string value.
        //
        // Testing:
        //   Given `TYPE` is a choice or sequence having selections or
        //   attributes with default values, respectively:
        //
        //   int BerDecoder::decode(bsl::streambuf *streamBuf, TYPE *variable)
        //   int BerDeocder::decode(bsl::istream&   stream   , TYPE *variable)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nDECODE COMPLEX TYPES WITH DEFAULT ELEMENTS"
                             "\n==========================================\n";

        if (veryVerbose)
                   cout << "\nDECODE CHOICES WHOSE SELECTIONS HAVE DEFAULTS"
                           "\n---------------------------------------------\n";
        {
            enum Selection {
                INT,
                STRING,
                ENUM
            };

            enum Value {
                ZERO,
                UNSET,
                DEFAULT,
                ARBITRARY
            };

            typedef test::MyEnumeration::Value Enum;

            static const int  ARBITRARY_INT      = 42;
            static const char ARBITRARY_STRING[] = "I'm arbitrary!";
            static const Enum ARBITRARY_ENUM     = test::MyEnumeration::VALUE1;

            static const bool NO  = false;
            static const bool YES = true;

            static const struct {
                int       d_line;
                bool      d_defaultStrings;
                Selection d_selection;
                Value     d_encodedValue;
                Value     d_decodedValue;
            } DATA[] = {
                //LINE DEFAULT STRINGS SELECTION ENCODED VALUE DECODED VALUE
                //---- --------------- --------- ------------- -------------
                {  L_ , NO            , INT     , ZERO        , ZERO        },
                {  L_ , NO            , INT     , UNSET       , DEFAULT     },
                {  L_ , NO            , INT     , DEFAULT     , DEFAULT     },
                {  L_ , NO            , INT     , ARBITRARY   , ARBITRARY   },
                {  L_ , NO            , STRING  , ZERO        , ZERO        },
                {  L_ , NO            , STRING  , UNSET       , DEFAULT     },
                {  L_ , NO            , STRING  , DEFAULT     , DEFAULT     },
                {  L_ , NO            , STRING  , ARBITRARY   , ARBITRARY   },
                {  L_ , NO            , ENUM    , ZERO        , ZERO        },
                {  L_ , NO            , ENUM    , UNSET       , DEFAULT     },
                {  L_ , NO            , ENUM    , DEFAULT     , DEFAULT     },
                {  L_ , NO            , ENUM    , ARBITRARY   , ARBITRARY   },
                {  L_ , YES           , INT     , ZERO        , ZERO        },
                {  L_ , YES           , INT     , UNSET       , DEFAULT     },
                {  L_ , YES           , INT     , DEFAULT     , DEFAULT     },
                {  L_ , YES           , INT     , ARBITRARY   , ARBITRARY   },
                {  L_ , YES           , STRING  , ZERO        , DEFAULT     },
                {  L_ , YES           , STRING  , UNSET       , DEFAULT     },
                {  L_ , YES           , STRING  , DEFAULT     , DEFAULT     },
                {  L_ , YES           , STRING  , ARBITRARY   , ARBITRARY   },
                {  L_ , YES           , ENUM    , ZERO        , ZERO        },
                {  L_ , YES           , ENUM    , UNSET       , DEFAULT     },
                {  L_ , YES           , ENUM    , DEFAULT     , DEFAULT     },
                {  L_ , YES           , ENUM    , ARBITRARY   , ARBITRARY   }
            };

            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int i = 0; i != NUM_DATA; ++i) {
                const int       LINE            = DATA[i].d_line;
                const bool      DEFAULT_STRINGS = DATA[i].d_defaultStrings;
                const Selection SELECTION       = DATA[i].d_selection;
                const Value     ENCODED_VALUE   = DATA[i].d_encodedValue;
                const Value     DECODED_VALUE   = DATA[i].d_decodedValue;

                typedef test::MyChoiceWithDefaultValues SerializableObject;

                SerializableObject object(&ta);

                switch (SELECTION) {
                  case INT: {
                    switch (ENCODED_VALUE) {
                      case ZERO: {
                        object.makeSelection0(0);
                      } break;
                      case UNSET: {
                        object.makeSelection0();
                      } break;
                      case DEFAULT: {
                        object.makeSelection0(
                           SerializableObject::DEFAULT_INITIALIZER_SELECTION0);
                      } break;
                      case ARBITRARY: {
                        object.makeSelection0(ARBITRARY_INT);
                      } break;
                    }
                  } break;
                  case STRING: {
                    switch (ENCODED_VALUE) {
                      case ZERO: {
                        object.makeSelection1("");
                      } break;
                      case UNSET: {
                        object.makeSelection1();
                      } break;
                      case DEFAULT: {
                        object.makeSelection1(
                           SerializableObject::DEFAULT_INITIALIZER_SELECTION1);
                      } break;
                      case ARBITRARY: {
                        object.makeSelection1(ARBITRARY_STRING);
                      } break;
                    }
                  } break;
                  case ENUM: {
                    switch (ENCODED_VALUE) {
                      case ZERO: {
                        object.makeSelection2(test::MyEnumeration::VALUE1);
                      } break;
                      case UNSET: {
                        object.makeSelection2();
                      } break;
                      case DEFAULT: {
                        object.makeSelection2(
                           SerializableObject::DEFAULT_INITIALIZER_SELECTION2);
                      } break;
                      case ARBITRARY: {
                        object.makeSelection2(ARBITRARY_ENUM);
                      } break;
                    }
                  } break;
                }

                balber::BerEncoderOptions encoderOptions;
                balber::BerEncoder encoder(&encoderOptions, &ta);

                bdlsb::MemOutStreamBuf outStreamBuf(&ta);
                int rc = encoder.encode(&outStreamBuf, object);
                ASSERTV(rc, 0 == rc);

                bdlsb::FixedMemInStreamBuf inStreamBuf(outStreamBuf.data(),
                                                       outStreamBuf.length());

                balber::BerDecoderOptions decoderOptions;
                if (!DEFAULT_STRINGS) {
                    decoderOptions.setDefaultEmptyStrings(false);
                }
                balber::BerDecoder decoder(&decoderOptions, &ta);

                SerializableObject decodedObject;
                rc = decoder.decode(&inStreamBuf, &decodedObject);
                ASSERTV(rc, 0 == rc);

                switch (SELECTION) {
                  case INT: {
                    ASSERTV(decodedObject.selectionId(),
                            decodedObject.isSelection0Value());
                    switch (DECODED_VALUE) {
                      case ZERO: {
                        ASSERT1_EQ(LINE, 0, decodedObject.selection0());
                      } break;
                      case UNSET: {
                        ASSERT1_EQ(LINE, 0, decodedObject.selection0());
                      } break;
                      case DEFAULT: {
                        ASSERT1_EQ(
                            LINE,
                            SerializableObject::DEFAULT_INITIALIZER_SELECTION0,
                            decodedObject.selection0());
                      } break;
                      case ARBITRARY: {
                        ASSERT1_EQ(
                            LINE, ARBITRARY_INT, decodedObject.selection0());
                      } break;
                    }
                  } break;
                  case STRING: {
                    ASSERTV(decodedObject.selectionId(),
                            decodedObject.isSelection1Value());
                    switch (DECODED_VALUE) {
                      case ZERO: {
                        ASSERT1_EQ(LINE, "", decodedObject.selection1());
                      } break;
                      case UNSET: {
                        ASSERT1_EQ(LINE, "", decodedObject.selection1());
                      } break;
                      case DEFAULT: {
                        ASSERT1_EQ(
                            LINE,
                            SerializableObject::DEFAULT_INITIALIZER_SELECTION1,
                            decodedObject.selection1());
                      } break;
                      case ARBITRARY: {
                        ASSERT1_EQ(LINE,
                                   ARBITRARY_STRING,
                                   decodedObject.selection1());
                      } break;
                    }
                  } break;
                  case ENUM: {
                    ASSERTV(decodedObject.selectionId(),
                            decodedObject.isSelection2Value());
                    switch (DECODED_VALUE) {
                      case ZERO: {
                        ASSERT1_EQ(LINE, 0, decodedObject.selection2());
                      } break;
                      case UNSET: {
                        ASSERT1_EQ(LINE, 0, decodedObject.selection2());
                      } break;
                      case DEFAULT: {
                        ASSERT1_EQ(
                            LINE,
                            SerializableObject::DEFAULT_INITIALIZER_SELECTION2,
                            decodedObject.selection2());
                      } break;
                      case ARBITRARY: {
                        ASSERT1_EQ(
                            LINE, ARBITRARY_ENUM, decodedObject.selection2());
                      } break;
                    }
                  } break;
                }
            }
        }

        if (veryVerbose)
            cout << "\nDECODE SEQUENCES WHOSE ATTRIBUTES HAVE DEFAULTS"
                    "\n-----------------------------------------------\n";
        {
            enum Value {
                ZERO,
                UNSET,
                DEFAULT,
                ARBITRARY
            };

            static const Value ARB = ARBITRARY;

            typedef test::MyEnumeration::Value Enum;

            static const int  ARBITRARY_INT      = 42;
            static const char ARBITRARY_STRING[] = "I'm arbitrary";
            static const Enum ARBITRARY_ENUM     = test::MyEnumeration::VALUE1;

            static const bool N = false;
            static const bool Y = true;

            static const struct {
                int   d_line;
                bool  d_defaultStrings;
                Value d_encodedAttr0;
                Value d_encodedAttr1;
                Value d_encodedAttr2;
                Value d_decodedAttr0;
                Value d_decodedAttr1;
                Value d_decodedAttr2;
            } DATA[] = {
                //LN D  ATTR 0   ATTR 1   ATTR 2  DEC A 0  DEC A 1  DEC A 2
                //-- - -------- -------- -------- -------- -------- --------
                {L_, Y, UNSET  , UNSET  , UNSET  , DEFAULT, DEFAULT, DEFAULT},
                {L_, Y, UNSET  , UNSET  , ZERO   , DEFAULT, DEFAULT, ZERO   },
                {L_, Y, UNSET  , UNSET  , DEFAULT, DEFAULT, DEFAULT, DEFAULT},
                {L_, Y, UNSET  , UNSET  , ARB    , DEFAULT, DEFAULT, ARB    },

                {L_, Y, UNSET  , UNSET  , UNSET  , DEFAULT, DEFAULT, DEFAULT},
                {L_, Y, UNSET  , ZERO   , UNSET  , DEFAULT, DEFAULT, DEFAULT},
                {L_, Y, UNSET  , DEFAULT, UNSET  , DEFAULT, DEFAULT, DEFAULT},
                {L_, Y, UNSET  , ARB    , UNSET  , DEFAULT, ARB    , DEFAULT},

                {L_, Y, UNSET  , UNSET  , UNSET  , DEFAULT, DEFAULT, DEFAULT},
                {L_, Y, ZERO   , UNSET  , UNSET  , ZERO   , DEFAULT, DEFAULT},
                {L_, Y, DEFAULT, UNSET  , UNSET  , DEFAULT, DEFAULT, DEFAULT},
                {L_, Y, ARB    , UNSET  , UNSET  , ARB    , DEFAULT, DEFAULT},

                {L_, N, UNSET  , UNSET  , UNSET  , DEFAULT, DEFAULT, DEFAULT},
                {L_, N, UNSET  , UNSET  , ZERO   , DEFAULT, DEFAULT, ZERO   },
                {L_, N, UNSET  , UNSET  , DEFAULT, DEFAULT, DEFAULT, DEFAULT},
                {L_, N, UNSET  , UNSET  , ARB    , DEFAULT, DEFAULT, ARB    },

                {L_, N, UNSET  , UNSET  , UNSET  , DEFAULT, DEFAULT, DEFAULT},
                {L_, N, UNSET  , ZERO   , UNSET  , DEFAULT, ZERO   , DEFAULT},
                {L_, N, UNSET  , DEFAULT, UNSET  , DEFAULT, DEFAULT, DEFAULT},
                {L_, N, UNSET  , ARB    , UNSET  , DEFAULT, ARB    , DEFAULT},

                {L_, N, UNSET  , UNSET  , UNSET  , DEFAULT, DEFAULT, DEFAULT},
                {L_, N, ZERO   , UNSET  , UNSET  , ZERO   , DEFAULT, DEFAULT},
                {L_, N, DEFAULT, UNSET  , UNSET  , DEFAULT, DEFAULT, DEFAULT},
                {L_, N, ARB    , UNSET  , UNSET  , ARB    , DEFAULT, DEFAULT},
            };

            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int i = 0; i != NUM_DATA; ++i) {
                const int   LINE               = DATA[i].d_line;
                const bool  DEFAULT_STRINGS    = DATA[i].d_defaultStrings;
                const Value ENCODED_ATTRIBUTE0 = DATA[i].d_encodedAttr0;
                const Value ENCODED_ATTRIBUTE1 = DATA[i].d_encodedAttr1;
                const Value ENCODED_ATTRIBUTE2 = DATA[i].d_encodedAttr2;
                const Value DECODED_ATTRIBUTE0 = DATA[i].d_decodedAttr0;
                const Value DECODED_ATTRIBUTE1 = DATA[i].d_decodedAttr1;
                const Value DECODED_ATTRIBUTE2 = DATA[i].d_decodedAttr2;

                typedef test::MySequenceWithDefaultValues SerializableObject;

                SerializableObject object(&ta);

                switch (ENCODED_ATTRIBUTE0) {
                  case ZERO: {
                    object.attribute0() = 0;
                  } break;
                  case UNSET: {
                    // do nothing
                  } break;
                  case DEFAULT: {
                    object.attribute0() = SerializableObject::
                    DEFAULT_INITIALIZER_ATTRIBUTE0;
                  } break;
                  case ARBITRARY: {
                    object.attribute0() = ARBITRARY_INT;
                  } break;
                }

                switch (ENCODED_ATTRIBUTE1) {
                  case ZERO: {
                    object.attribute1().clear();
                  } break;
                  case UNSET: {
                    // do nothing
                  } break;
                  case DEFAULT: {
                    object.attribute1() = SerializableObject::
                    DEFAULT_INITIALIZER_ATTRIBUTE1;
                  } break;
                  case ARBITRARY: {
                    object.attribute1() = ARBITRARY_STRING;
                  } break;
                }

                switch (ENCODED_ATTRIBUTE2) {
                  case ZERO: {
                    object.attribute2() =
                        static_cast<test::MyEnumeration::Value>(0);
                  } break;
                  case UNSET: {
                    // do nothing
                  } break;
                  case DEFAULT: {
                    object.attribute2() = SerializableObject::
                    DEFAULT_INITIALIZER_ATTRIBUTE2;
                  } break;
                  case ARBITRARY: {
                    object.attribute2() = ARBITRARY_ENUM;
                  } break;
                }

                balber::BerEncoderOptions encoderOptions;
                balber::BerEncoder        encoder(&encoderOptions, &ta);

                bdlsb::MemOutStreamBuf outStreamBuf(&ta);
                int rc = encoder.encode(&outStreamBuf, object);
                ASSERTV(LINE, rc, 0 == rc);

                bdlsb::FixedMemInStreamBuf inStreamBuf(outStreamBuf.data(),
                                                       outStreamBuf.length());


                balber::BerDecoderOptions decoderOptions;
                if (!DEFAULT_STRINGS) {
                    decoderOptions.setDefaultEmptyStrings(false);
                }
                balber::BerDecoder decoder(&decoderOptions, &ta);

                SerializableObject decodedObject;
                rc = decoder.decode(&inStreamBuf, &decodedObject);
                ASSERTV(LINE, rc, 0 == rc);

                switch (DECODED_ATTRIBUTE0) {
                  case ZERO: {
                    ASSERT1_EQ(LINE, 0, decodedObject.attribute0());
                  } break;
                  case UNSET: {
                      ASSERT1_EQ(LINE, 0, decodedObject.attribute0());
                  } break;
                  case DEFAULT: {
                    ASSERT1_EQ(
                        LINE,
                        SerializableObject::DEFAULT_INITIALIZER_ATTRIBUTE0,
                        object.attribute0());
                  } break;
                  case ARBITRARY: {
                    ASSERT1_EQ(LINE, ARBITRARY_INT, object.attribute0());
                  } break;
                }

                switch (DECODED_ATTRIBUTE1) {
                  case ZERO: {
                    ASSERT1_EQ(LINE, "", decodedObject.attribute1());
                  } break;
                  case UNSET: {
                    ASSERT1_EQ(LINE, "", decodedObject.attribute1());
                  } break;
                  case DEFAULT: {
                    ASSERT1_EQ(
                        LINE,
                        SerializableObject::DEFAULT_INITIALIZER_ATTRIBUTE1,
                        decodedObject.attribute1());
                  } break;
                  case ARBITRARY: {
                    ASSERT1_EQ(
                        LINE, ARBITRARY_STRING, decodedObject.attribute1());
                  } break;
                }

                switch (DECODED_ATTRIBUTE2) {
                  case ZERO: {
                    ASSERT1_EQ(LINE, 0, decodedObject.attribute2());
                  } break;
                  case UNSET: {
                    ASSERT1_EQ(LINE, 0, decodedObject.attribute2());
                  } break;
                  case DEFAULT: {
                    ASSERT1_EQ(
                        LINE,
                        SerializableObject::DEFAULT_INITIALIZER_ATTRIBUTE2,
                        object.attribute2());
                  } break;
                  case ARBITRARY: {
                    ASSERT1_EQ(LINE, ARBITRARY_ENUM, object.attribute2());
                  } break;
                }
            }
        }
        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // ENCODE AND DECODE `RawData`
        //
        // Concerns:
        // 1. A `vector<char>` with certain formatting modes will be encoded
        //    by `BerEncoder` as a primitive array.
        //
        // 2. A `vector<char>` with any other formatting mode, or a
        //    `vector<unsigned char>`, will be encoded by `BerEncoder` as a
        //    sequence of individual values.
        //
        // 3. `BerDecoder` when decoding a `vector<char>` or a
        //    `vector<unsigned char>` should be able to decode either of those
        //    formats, independent of which encoding choice was made.
        //
        // 4. Similarly, since `char` and `unsigned char` are generally really
        //    used interchangeably as `byte`, the type when encoded shouldn't
        //    prevent decoding even if it does not match the type when
        //    decoding.
        //
        // Plan:
        // 1. For each of the differently structured `RawData` types from the
        //    test schema (`test::RawData`, `test::RawDataSwitched`,
        //    `test::RawDataUnformatted`) populate an object with test data and
        //    encode it.
        //
        // 2. Use a `BerDecoder` to decode into a `test::RawData`.
        //
        // 3. Verify that the 2 vectors from the decoded object match the
        //    test data that was populated into the encoded object.
        //
        // Testing:
        //   int BerDecoder_Node::decode(bsl::vector<char>          *variable,
        //                               bdlat_TypeCategory::Array  )
        //   int BerDecoder_Node::decode(bsl::vector<unsigned char> *variable,
        //                               bdlat_TypeCategory::Array  )
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODE AND DECODE `RawData`"
                             "\n===========================\n";

        bsl::vector<char>          testData = loadFromHex(
                                            "300C8001 22A10082 0548656C 6C6F");
        bsl::vector<unsigned char> utestData;
        bsl::copy(testData.begin(), testData.end(),
                                                bsl::back_inserter(utestData));
        ASSERT(testData.size() == utestData.size());

        if (verbose) cout << "\nTesting with `RawData`.\n";
        {
            test::RawData rawData;
            rawData.charvec() = testData;
            bsl::copy(testData.begin(),testData.end(),
                                       bsl::back_inserter(rawData.ucharvec()));

            ASSERT( testData == rawData.charvec());
            ASSERT(utestData == rawData.ucharvec());

            balber::BerEncoderOptions encoderOptions;
            balber::BerEncoder        encoder(&encoderOptions);
            bdlsb::MemOutStreamBuf    osb;
            ASSERT(0 == encoder.encode(&osb, rawData));

            if (veryVerbose) {
                P(osb.length());
                printBuffer(osb.data(),osb.length());
            }

            balber::BerDecoderOptions  decoderOptions;
            balber::BerDecoder         decoder(&decoderOptions);
            test::RawData              rawDataParsed;
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &rawDataParsed));

            ASSERT(  rawData == rawDataParsed);
            ASSERT( testData == rawDataParsed.charvec());
            ASSERT(utestData == rawDataParsed.ucharvec());
        }

        if (verbose) cout << "\nTesting with `RawDataUnformatted`.\n";
        {
            test::RawDataUnformatted rawDataUf;
            rawDataUf.charvec() = testData;
            bsl::copy(testData.begin(),testData.end(),
                                     bsl::back_inserter(rawDataUf.ucharvec()));

            ASSERT( testData == rawDataUf.charvec());
            ASSERT(utestData == rawDataUf.ucharvec());

            balber::BerEncoderOptions encoderOptions;
            balber::BerEncoder        encoder(&encoderOptions);
            bdlsb::MemOutStreamBuf    osb;
            ASSERT(0 == encoder.encode(&osb, rawDataUf));

            if (veryVerbose) {
                P(osb.length());
                printBuffer(osb.data(),osb.length());
            }

            balber::BerDecoderOptions  decoderOptions;
            balber::BerDecoder         decoder(&decoderOptions);
            test::RawData              rawDataParsed;
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &rawDataParsed));

            ASSERT( testData == rawDataParsed.charvec());
            ASSERT(utestData == rawDataParsed.ucharvec());
        }

        if (verbose) cout << "\nTesting uchar->char with `RawDataSwitched`.\n";
        {
            test::RawDataSwitched rawDataSw;
            bsl::copy(testData.begin(),testData.end(),
                                     bsl::back_inserter(rawDataSw.ucharvec()));

            ASSERT(0 == rawDataSw.charvec().size());
            ASSERT(utestData == rawDataSw.ucharvec());

            balber::BerEncoderOptions encoderOptions;
            balber::BerEncoder        encoder(&encoderOptions);
            bdlsb::MemOutStreamBuf    osb;
            ASSERT(0 == encoder.encode(&osb, rawDataSw));

            if (veryVerbose) {
                P(osb.length());
                printBuffer(osb.data(),osb.length());
            }

            balber::BerDecoderOptions  decoderOptions;
            balber::BerDecoder         decoder(&decoderOptions);
            test::RawData              rawDataParsed;
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            int rc = decoder.decode(&isb,&rawDataParsed);
            if (rc != 0) {
                cerr << "Errors parsing:" << decoder.loggedMessages() << '\n';
                ASSERT(0 == rc);
            }

            ASSERT(0 == rawDataParsed.ucharvec().size());
            ASSERT(testData == rawDataParsed.charvec());
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // DECODE DATE/TIME COMPONENTS USING A VARIANT
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nDECODE DATE/TIME COMPONENTS USING A VARIANT"
                             "\n===========================================\n";

        balber::BerEncoderOptions options;
        options.setEncodeDateAndTimeTypesAsBinary(true);
        const balber::BerEncoderOptions DEFOPTS;

        static const struct {
            int d_lineNum;   // source line number
            int d_year;      // year under test
            int d_month;     // month under test
            int d_day;       // day under test
            int d_hour;      // hour under test
            int d_minutes;   // minutes under test
            int d_seconds;   // seconds under test
            int d_milliSecs; // milli seconds under test
            int d_tzoffset;  // time zone offset
        } DATA[] = {
   //line no.  year   month   day   hour    min   sec    ms  offset
   //-------   -----  -----   ---   ----    ---   ---    --  ------
    {      L_,      1,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,     45     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    1,     1,     1,    1,    1,      0     },
    {      L_,      1,     1,    1,     1,     1,    1,    1,    500     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    1,     1,    23,   59,   59,      0     },
    {      L_,      1,     1,    1,     1,    23,   59,   59,   1439     },
    {      L_,      1,     1,    1,     1,    23,   59,   59,  -1439     },

    {      L_,      1,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,    2,     0,     0,    0,    0,   1439     },
    {      L_,      1,     1,    2,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    2,     1,     1,    1,    1,      0     },
    {      L_,      1,     1,    2,     1,     1,    1,    1,    500     },

    {      L_,      1,     1,    2,     1,    23,   59,   59,      0     },
    {      L_,      1,     1,    2,     1,    23,   59,   59,    500     },
    {      L_,      1,     1,    2,     1,    23,   59,   59,   -500     },

    {      L_,      1,     1,   10,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,   10,     1,     1,    1,    1,     99     },

    {      L_,      1,     1,   30,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,   31,     0,     0,    0,    0,   1439     },
    {      L_,      1,     1,   31,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     2,    1,     0,     0,    0,    0,      0     },
    {      L_,      1,     2,    1,    23,    59,   59,    0,   1439     },

    {      L_,      1,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,      1,    12,   31,    23,    59,   59,    0,   1439     },

    {      L_,      2,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      2,     1,    1,    23,    59,   59,    0,   1439     },

    {      L_,      4,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      4,     1,    1,    23,    59,   59,    0,   1439     },

    {      L_,      4,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,      4,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,      4,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,      4,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,      4,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,      4,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,      4,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,      4,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,      4,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,      8,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,      8,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,      8,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,      8,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,      8,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,      8,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,    100,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    100,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,    100,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,    100,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    100,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,    100,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    400,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,    400,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,    400,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,    400,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    400,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,    400,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,    500,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    500,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,    500,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    500,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,    800,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    800,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,    800,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,    800,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,    800,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    800,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   1000,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   1000,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,   1000,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   1000,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   2000,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,   2000,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   2000,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   2016,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2017,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2018,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2019,    12,   31,     0,     0,    0,    0,      0     },

    {      L_,   2020,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,   2020,     1,    1,     0,     0,    0,    0,   1439     },
    {      L_,   2020,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,   2020,     1,    1,    23,    59,   59,  999,      0     },
    {      L_,   2020,     1,    1,    23,    59,   59,  999,   1439     },
    {      L_,   2020,     1,    1,    23,    59,   59,  999,  -1439     },

    {      L_,   2020,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,   2020,     1,    2,     0,     0,    0,    0,   1439     },
    {      L_,   2020,     1,    2,     0,     0,    0,    0,  -1439     },

    {      L_,   2020,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   2020,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,   2020,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,   2020,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,   2020,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   2020,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,   2021,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,   2022,     1,    2,     0,     0,    0,    0,      0     },

    {      L_,   9999,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   9999,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,   9999,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,   9999,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   9999,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,   9999,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,   9999,    12,   30,     0,     0,    0,    0,      0     },
    {      L_,   9999,    12,   30,    23,    59,   59,    0,   1439     },

    {      L_,   9999,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   9999,    12,   31,    23,    59,   59,    0,   1439     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\n'bdlt::Date'.\n";
        {
            typedef bdlb::Variant2<bdlt::Date, bdlt::DateTz> Variant;

            typedef bdlt::Date Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;

                if (veryVerbose) { P_(Y) P_(M) P(D) }

                const Type VALUE(Y, M, D);
                Variant    value1, value2;

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value1));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value1.is<bdlt::Date>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::Date>());
                    if (veryVerbose) {
                        P_(VALUE) P(value1);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value2));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value2.is<bdlt::Date>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::Date>());
                    if (veryVerbose) {
                        P_(VALUE) P(value2);
                    }
                }
            }
        }

        if (verbose) cout << "\n'bdlt::DateTz'.\n";
        {
            typedef bdlb::Variant2<bdlt::Date, bdlt::DateTz> Variant;

            typedef bdlt::DateTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdlt::Date(Y, M, D), OFF);
                Variant    value1, value2;

                if (veryVerbose) { P_(Y) P_(M) P_(D) P(OFF) }

                if (OFF) {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value1));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value1.is<bdlt::DateTz>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::DateTz>());
                    if (veryVerbose) {
                        P_(VALUE) P(value1);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value2));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value2.is<bdlt::DateTz>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::DateTz>());
                    if (veryVerbose) {
                        P_(VALUE) P(value1);
                    }
                }
            }
        }

        if (verbose) cout << "\n'bdlt::Time'.\n";
        {
            typedef bdlb::Variant2<bdlt::Time, bdlt::TimeTz> Variant;

            typedef bdlt::Time Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;

                if (veryVerbose) { P_(H) P_(MM) P_(S) P(MS) }

                const Type VALUE(H, MM, S, MS);
                Variant    value1, value2;

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value1));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value1.is<bdlt::Time>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::Time>());
                    if (veryVerbose) {
                        P_(VALUE) P(value1);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value2));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value2.is<bdlt::Time>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::Time>());
                    if (veryVerbose) {
                        P_(VALUE) P(value2);
                    }
                }
            }
        }

        if (verbose) cout << "\n'bdlt::TimeTz'.\n";
        {
            typedef bdlb::Variant2<bdlt::Time, bdlt::TimeTz> Variant;

            typedef bdlt::TimeTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdlt::Time(H, MM, S, MS), OFF);
                Variant    value1, value2;

                if (veryVerbose) { P_(H) P_(MM) P_(S) P_(MS) P(OFF) }

                if (OFF) {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value1));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value1.is<bdlt::TimeTz>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::TimeTz>());
                    if (veryVerbose) {
                        P_(VALUE) P(value1);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value2));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value2.is<bdlt::TimeTz>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::TimeTz>());
                    if (veryVerbose) {
                        P_(VALUE) P(value1);
                    }
                }
            }
        }

        if (verbose) cout << "\n'bdlt::Datetime'.\n";
        {
            typedef bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz> Variant;
            typedef bdlt::Datetime                                   Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;

                const Type VALUE(Y, M, D, H, MM, S, MS);
                Variant    value1, value2;

                if (veryVerbose) { P_(Y) P_(M) P_(D) P_(H)
                                   P_(MM) P_(S) P(MS) }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (osb.length() > 6) {
                        // Datetime objects having length greater that 6 bytes
                        // are always encoded with a time zone.
                        continue;                                   // CONTINUE
                    }

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value1));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value1.is<bdlt::Datetime>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::Datetime>());
                    if (veryVerbose) {
                        P_(VALUE) P(value1);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value2));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value2.is<bdlt::Datetime>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::Datetime>());
                    if (veryVerbose) {
                        P_(VALUE) P(value2);
                    }
                }
            }
        }

        if (verbose) cout << "\n'bdlt::DatetimeTz'.\n";
        {
            typedef bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz> Variant;

            typedef bdlt::DatetimeTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdlt::Datetime(Y, M, D, H, MM, S, MS), OFF);
                Variant    value1, value2;

                if (veryVerbose) { P_(Y) P_(M) P_(D) P_(H)
                                   P_(MM) P_(S) P_(MS) P(OFF) }

                if (OFF) {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value1));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value1.is<bdlt::DatetimeTz>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::DatetimeTz>());
                    if (veryVerbose) {
                        P_(VALUE) P(value1);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value2));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value2.is<bdlt::DatetimeTz>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::DatetimeTz>());
                    if (veryVerbose) {
                        P_(VALUE) P(value2);
                    }
                }
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // ENCODE AND DECODE DATE/TIME COMPONENTS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODE AND DECODE DATE/TIME COMPONENTS"
                             "\n======================================\n";

        balber::BerEncoderOptions options;
        options.setEncodeDateAndTimeTypesAsBinary(true);
        const balber::BerEncoderOptions DEFOPTS;

        if (verbose) cout << "\nDate brute force testing\n";
        {
            const int YEARS[] = { 1, 4, 96, 100, 400, 500, 800, 1000, 1600,
                                  1700, 1751, 1752, 1753, 1930, 2000, 2010,
                                  2012, 2019, 2020, 2021, 6478, 6479, 6480,
                                  9998, 9999 };
            const int NUM_YEARS = sizeof YEARS / sizeof *YEARS;

            const int MONTHS[] = { 1, 2, 5, 8, 9, 12 };
            const int NUM_MONTHS = sizeof MONTHS / sizeof *MONTHS;

            const int DAYS[] = { 1, 2, 5, 10, 15, 20, 28, 29, 30, 31 };
            const int NUM_DAYS = sizeof DAYS / sizeof *DAYS;

            for (int i = 0; i < NUM_YEARS; ++i) {
            for (int j = 0; j < NUM_MONTHS; ++j) {
            for (int k = 0; k < NUM_DAYS; ++k) {

                const int YEAR  = YEARS[i];
                const int MONTH = MONTHS[j];
                const int DAY   = DAYS[k];

                if (bdlt::Date::isValidYearMonthDay(YEAR, MONTH, DAY)
                 && bdlt::ProlepticDateImpUtil::isValidYearMonthDay(YEAR,
                                                                    MONTH,
                                                                    DAY))
                {
                    if (veryVerbose) { P_(YEAR) P_(MONTH) P(DAY) }

                    const bdlt::Date VALUE(YEAR, MONTH, DAY); bdlt::Date value;
                    const int OFF1 = 0, OFF2 =-1439, OFF3 =1439;
                    const bdlt::DateTz VALUE1(bdlt::Date(YEAR, MONTH, DAY),
                                              OFF1);
                    const bdlt::DateTz VALUE2(bdlt::Date(YEAR, MONTH, DAY),
                                              OFF2);
                    const bdlt::DateTz VALUE3(bdlt::Date(YEAR, MONTH, DAY),
                                              OFF3);
                    bdlt::DateTz value1, value2, value3;

                    {
                        bdlsb::MemOutStreamBuf osb;
                        balber::BerEncoder     encoder(&DEFOPTS);
                        ASSERT(0 == encoder.encode(&osb, VALUE));

                        if (veryVerbose) {
                            P(osb.length());
                            printBuffer(osb.data(), osb.length());
                        }

                        bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                       osb.length());
                        ASSERT(0 == decoder.decode(&isb, &value));
                        printDiagnostic(decoder);

                        LOOP2_ASSERT(VALUE, value, VALUE == value);
                        if (veryVerbose) {
                            P(VALUE);
                            P(value);
                        }
                    }

                    {
                        bdlsb::MemOutStreamBuf osb1, osb2, osb3;
                        balber::BerEncoder     encoder(&DEFOPTS);
                        ASSERT(0 == encoder.encode(&osb1, VALUE1));
                        ASSERT(0 == encoder.encode(&osb2, VALUE2));
                        ASSERT(0 == encoder.encode(&osb3, VALUE3));

                        if (veryVerbose) {
                            P(osb1.length());
                            P(osb2.length());
                            P(osb3.length());
                            printBuffer(osb1.data(), osb1.length());
                            printBuffer(osb2.data(), osb2.length());
                            printBuffer(osb3.data(), osb3.length());
                        }

                        bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                        osb1.length());
                        bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                        osb2.length());
                        bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                        osb3.length());

                        ASSERT(0 == decoder.decode(&isb1, &value1));
                        ASSERT(0 == decoder.decode(&isb2, &value2));
                        ASSERT(0 == decoder.decode(&isb3, &value3));
                        printDiagnostic(decoder);

                        LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                        LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                        LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                        if (veryVerbose) {
                            P(VALUE1); P(value1);
                            P(VALUE2); P(value2);
                            P(VALUE3); P(value3);
                        }
                    }

                    {
                        bdlsb::MemOutStreamBuf osb;
                        balber::BerEncoder     encoder(&DEFOPTS);
                        ASSERT(0 == encoder.encode(&osb, VALUE));

                        if (veryVerbose) {
                            P(osb.length());
                            printBuffer(osb.data(), osb.length());
                        }

                        bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                       osb.length());
                        ASSERT(0 == decoder.decode(&isb, &value));
                        printDiagnostic(decoder);

                        LOOP2_ASSERT(VALUE, value, VALUE == value);
                        if (veryVerbose) {
                            P(VALUE);
                            P(value);
                        }
                    }

                    {
                        bdlsb::MemOutStreamBuf osb;
                        balber::BerEncoder     encoder(&options);
                        ASSERT(0 == encoder.encode(&osb, VALUE));

                        if (veryVerbose) {
                            P(osb.length());
                            printBuffer(osb.data(), osb.length());
                        }

                        bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                       osb.length());
                        ASSERT(0 == decoder.decode(&isb, &value));
                        printDiagnostic(decoder);

                        LOOP2_ASSERT(VALUE, value, VALUE == value);
                        if (veryVerbose) {
                            P(VALUE);
                            P(value);
                        }
                    }

                    {
                        bdlsb::MemOutStreamBuf osb1, osb2, osb3;
                        balber::BerEncoder     encoder(&options);
                        ASSERT(0 == encoder.encode(&osb1, VALUE1));
                        ASSERT(0 == encoder.encode(&osb2, VALUE2));
                        ASSERT(0 == encoder.encode(&osb3, VALUE3));

                        if (veryVerbose) {
                            P(osb1.length());
                            P(osb2.length());
                            P(osb3.length());
                            printBuffer(osb1.data(), osb1.length());
                            printBuffer(osb2.data(), osb2.length());
                            printBuffer(osb3.data(), osb3.length());
                        }

                        bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                        osb1.length());
                        bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                        osb2.length());
                        bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                        osb3.length());

                        ASSERT(0 == decoder.decode(&isb1, &value1));
                        ASSERT(0 == decoder.decode(&isb2, &value2));
                        ASSERT(0 == decoder.decode(&isb3, &value3));
                        printDiagnostic(decoder);

                        LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                        LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                        LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                        if (veryVerbose) {
                            P(VALUE1); P(value1);
                            P(VALUE2); P(value2);
                            P(VALUE3); P(value3);
                        }
                    }
                }
            }
            }
            }
        }

        if (verbose) cout << "\nTesting Time Brute force.\n";
        {
            for (int hour = 0; hour <= 23; ++hour) {
                for (int min = 0; min < 60; ++min) {
                    for (int sec = 0; sec < 60; ++sec) {
                        if (veryVerbose) { P_(hour) P_(min) P(sec) }

                        const int MS = 0;
                        const bdlt::Time VALUE(hour, min, sec, MS);
                        bdlt::Time value;
                        const int MS1 = 0, MS2 = 500, MS3 = 999;
                        const int OFF1 = 0, OFF2 =-1439, OFF3 =1439;
                        const bdlt::TimeTz VALUE1(bdlt::Time(hour,
                                                             min,
                                                             sec,
                                                             MS1),
                                                 OFF1);
                        const bdlt::TimeTz VALUE2(bdlt::Time(hour,
                                                             min,
                                                             sec,
                                                             MS2),
                                                 OFF2);
                        const bdlt::TimeTz VALUE3(bdlt::Time(hour,
                                                             min,
                                                             sec,
                                                             MS3),
                                                 OFF3);
                        bdlt::TimeTz value1, value2, value3;

                        {
                            bdlsb::MemOutStreamBuf osb;
                            balber::BerEncoder     encoder(&DEFOPTS);
                            ASSERT(0 == encoder.encode(&osb, VALUE));

                            if (veryVerbose) {
                                P(osb.length());
                                printBuffer(osb.data(), osb.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                           osb.length());
                            ASSERT(0 == decoder.decode(&isb, &value));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                            if (veryVerbose) {
                                P(VALUE);
                                P(value);
                            }
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;
                            balber::BerEncoder     encoder(&DEFOPTS);
                            ASSERT(0 == encoder.encode(&osb1, VALUE1));
                            ASSERT(0 == encoder.encode(&osb2, VALUE2));
                            ASSERT(0 == encoder.encode(&osb3, VALUE3));

                            if (veryVerbose) {
                                P(osb1.length());
                                P(osb2.length());
                                P(osb3.length());
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                            osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                            osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                            osb3.length());

                            ASSERT(0 == decoder.decode(&isb1, &value1));
                            ASSERT(0 == decoder.decode(&isb2, &value2));
                            ASSERT(0 == decoder.decode(&isb3, &value3));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                            LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                            LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                            if (veryVerbose) {
                                P(VALUE1); P(value1);
                                P(VALUE2); P(value2);
                                P(VALUE3); P(value3);
                            }
                        }

                        {
                            bdlsb::MemOutStreamBuf osb;
                            balber::BerEncoder     encoder(&options);
                            ASSERT(0 == encoder.encode(&osb, VALUE));

                            if (veryVerbose) {
                                P(osb.length());
                                printBuffer(osb.data(), osb.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                           osb.length());
                            ASSERT(0 == decoder.decode(&isb, &value));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                            if (veryVerbose) {
                                P(VALUE);
                                P(value);
                            }
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;
                            balber::BerEncoder     encoder(&options);
                            ASSERT(0 == encoder.encode(&osb1, VALUE1));
                            ASSERT(0 == encoder.encode(&osb2, VALUE2));
                            ASSERT(0 == encoder.encode(&osb3, VALUE3));

                            if (veryVerbose) {
                                P(osb1.length());
                                P(osb2.length());
                                P(osb3.length());
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                            osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                            osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                            osb3.length());

                            ASSERT(0 == decoder.decode(&isb1, &value1));
                            ASSERT(0 == decoder.decode(&isb2, &value2));
                            ASSERT(0 == decoder.decode(&isb3, &value3));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                            LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                            LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                            if (veryVerbose) {
                                P(VALUE1); P(value1);
                                P(VALUE2); P(value2);
                                P(VALUE3); P(value3);
                            }
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nDatetime brute force testing\n";
        {
            const int YEARS[] = { 1, 4, 96, 100, 400, 500, 800, 1000, 1600,
                                  1700, 1751, 1752, 1753, 1930, 2000, 2010,
                                  2012, 2019, 2020, 2021, 6478, 6479, 6480,
                                  9998, 9999 };
            const int NUM_YEARS = sizeof YEARS / sizeof *YEARS;

            const int MONTHS[] = { 1, 2, 5, 8, 9, 12 };
            const int NUM_MONTHS = sizeof MONTHS / sizeof *MONTHS;

            const int DAYS[] = { 1, 2, 5, 10, 15, 20, 28, 29, 30, 31 };
            const int NUM_DAYS = sizeof DAYS / sizeof *DAYS;

            for (int di = 0; di < NUM_YEARS; ++di) {
            for (int dj = 0; dj < NUM_MONTHS; ++dj) {
            for (int dk = 0; dk < NUM_DAYS; ++dk) {

                const int YEAR  = YEARS[di];
                const int MONTH = MONTHS[dj];
                const int DAY   = DAYS[dk];

                if (bdlt::Date::isValidYearMonthDay(YEAR, MONTH, DAY)
                 && bdlt::ProlepticDateImpUtil::isValidYearMonthDay(YEAR,
                                                                    MONTH,
                                                                    DAY)) {

                    const int HOURS[] = { 0, 12, 23 };
                    const int NUM_HOURS = sizeof HOURS / sizeof *HOURS;

                    const int MINS[] = { 0, 30, 59 };
                    const int NUM_MINS = sizeof MINS / sizeof *MINS;

                    const int SECONDS[] = { 0, 30, 59 };
                    const int NUM_SECS = sizeof SECONDS / sizeof *SECONDS;

                    for (int ti = 0; ti < NUM_HOURS; ++ti) {
                    for (int tj = 0; tj < NUM_MINS; ++tj) {
                    for (int tk = 0; tk < NUM_SECS; ++tk) {

                        const int HOUR = HOURS[ti];
                        const int MIN  = MINS[tj];
                        const int SECS = SECONDS[tk];

                        if (veryVerbose) { P_(YEAR) P_(MONTH) P(DAY) }
                        if (veryVerbose) { P_(HOUR) P_(MIN) P(SECS) }

                        const int MS = 0;
                        const bdlt::Date DATE(YEAR, MONTH, DAY);
                        const bdlt::Time TIME(HOUR, MIN, SECS, MS);
                        const bdlt::Datetime VALUE(DATE, TIME);
                        bdlt::Datetime value;
                        const int MS1 = 0, MS2 = 500, MS3 = 999;
                        const int OFF1 = 0, OFF2 =-1439, OFF3 =1439;
                        const bdlt::Date DATE1(YEAR, MONTH, DAY);
                        const bdlt::Time TIME1(HOUR, MIN, SECS, MS1);

                        const bdlt::Date DATE2(YEAR, MONTH, DAY);
                        const bdlt::Time TIME2(HOUR, MIN, SECS, MS2);

                        const bdlt::Date DATE3(YEAR, MONTH, DAY);
                        const bdlt::Time TIME3(HOUR, MIN, SECS, MS3);

                        const bdlt::Datetime DT1(DATE1, TIME1);
                        const bdlt::Datetime DT2(DATE2, TIME2);
                        const bdlt::Datetime DT3(DATE3, TIME3);

                        const bdlt::DatetimeTz VALUE1(DT1, OFF1);
                        const bdlt::DatetimeTz VALUE2(DT2, OFF2);
                        const bdlt::DatetimeTz VALUE3(DT3, OFF3);

                        bdlt::DatetimeTz value1, value2, value3;

                        test::BasicRecord VALUE4;
                        VALUE4.i1() = 100;
                        VALUE4.i2() = 200;
                        VALUE4.dt() = VALUE1;
                        VALUE4.s()  = "Hello World";

                        test::BasicRecord VALUE5(VALUE4), VALUE6(VALUE4);
                        VALUE5.dt() = VALUE2;
                        VALUE6.dt() = VALUE3;

                        test::BasicRecord value4, value5, value6;

                        {
                            bdlsb::MemOutStreamBuf osb;
                            balber::BerEncoder     encoder(&DEFOPTS);
                            ASSERT(0 == encoder.encode(&osb, VALUE));

                            if (veryVerbose) {
                                P(osb.length());
                                printBuffer(osb.data(), osb.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                           osb.length());
                            ASSERT(0 == decoder.decode(&isb, &value));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                            if (veryVerbose) {
                                P(VALUE);
                                P(value);
                            }
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;
                            balber::BerEncoder     encoder(&DEFOPTS);
                            ASSERT(0 == encoder.encode(&osb1, VALUE1));
                            ASSERT(0 == encoder.encode(&osb2, VALUE2));
                            ASSERT(0 == encoder.encode(&osb3, VALUE3));

                            if (veryVerbose) {
                                P(osb1.length());
                                P(osb2.length());
                                P(osb3.length());
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                            osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                            osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                            osb3.length());

                            ASSERT(0 == decoder.decode(&isb1, &value1));
                            ASSERT(0 == decoder.decode(&isb2, &value2));
                            ASSERT(0 == decoder.decode(&isb3, &value3));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                            LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                            LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                            if (veryVerbose) {
                                P(VALUE1); P(value1);
                                P(VALUE2); P(value2);
                                P(VALUE3); P(value3);
                            }
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;
                            balber::BerEncoder     encoder(&DEFOPTS);
                            ASSERT(0 == encoder.encode(&osb1, VALUE4));
                            ASSERT(0 == encoder.encode(&osb2, VALUE5));
                            ASSERT(0 == encoder.encode(&osb3, VALUE6));

                            if (veryVerbose) {
                                P(osb1.length());
                                P(osb2.length());
                                P(osb3.length());
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                            osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                            osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                            osb3.length());

                            ASSERT(0 == decoder.decode(&isb1, &value4));
                            ASSERT(0 == decoder.decode(&isb2, &value5));
                            ASSERT(0 == decoder.decode(&isb3, &value6));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE4, value4, VALUE4 == value4);
                            LOOP2_ASSERT(VALUE5, value5, VALUE5 == value5);
                            LOOP2_ASSERT(VALUE6, value6, VALUE6 == value6);
                            if (veryVerbose) {
                                P(VALUE4); P(value4);
                                P(VALUE5); P(value5);
                                P(VALUE6); P(value6);
                            }
                        }

                        {
                            bdlsb::MemOutStreamBuf osb;
                            balber::BerEncoder     encoder(&options);
                            ASSERT(0 == encoder.encode(&osb, VALUE));

                            if (veryVerbose) {
                                P(osb.length());
                                printBuffer(osb.data(), osb.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                           osb.length());
                            ASSERT(0 == decoder.decode(&isb, &value));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                            if (veryVerbose) {
                                P(VALUE);
                                P(value);
                            }
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;
                            balber::BerEncoder     encoder(&options);
                            ASSERT(0 == encoder.encode(&osb1, VALUE1));
                            ASSERT(0 == encoder.encode(&osb2, VALUE2));
                            ASSERT(0 == encoder.encode(&osb3, VALUE3));

                            if (veryVerbose) {
                                P(osb1.length());
                                P(osb2.length());
                                P(osb3.length());
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                            osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                            osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                            osb3.length());

                            ASSERT(0 == decoder.decode(&isb1, &value1));
                            ASSERT(0 == decoder.decode(&isb2, &value2));
                            ASSERT(0 == decoder.decode(&isb3, &value3));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                            LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                            LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                            if (veryVerbose) {
                                P(VALUE1); P(value1);
                                P(VALUE2); P(value2);
                                P(VALUE3); P(value3);
                            }
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;
                            balber::BerEncoder     encoder(&options);
                            ASSERT(0 == encoder.encode(&osb1, VALUE4));
                            ASSERT(0 == encoder.encode(&osb2, VALUE5));
                            ASSERT(0 == encoder.encode(&osb3, VALUE6));

                            if (veryVerbose) {
                                P(osb1.length());
                                P(osb2.length());
                                P(osb3.length());
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                            osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                            osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                            osb3.length());

                            ASSERT(0 == decoder.decode(&isb1, &value4));
                            ASSERT(0 == decoder.decode(&isb2, &value5));
                            ASSERT(0 == decoder.decode(&isb3, &value6));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE4, value4, VALUE4 == value4);
                            LOOP2_ASSERT(VALUE5, value5, VALUE5 == value5);
                            LOOP2_ASSERT(VALUE6, value6, VALUE6 == value6);
                            if (veryVerbose) {
                                P(VALUE4); P(value4);
                                P(VALUE5); P(value5);
                                P(VALUE6); P(value6);
                            }
                        }
                    }
                    }
                    }
                }
            }
            }
            }
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // ENCODE AND DECODE DATE/TIME
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODE AND DECODE DATE/TIME"
                             "\n===========================\n";

        balber::BerEncoderOptions options;
        options.setEncodeDateAndTimeTypesAsBinary(true);
        const balber::BerEncoderOptions DEFOPTS;

        if (verbose) cout << "\nDefine data\n";

        static const struct {
            int d_lineNum;   // source line number
            int d_year;      // year under test
            int d_month;     // month under test
            int d_day;       // day under test
            int d_hour;      // hour under test
            int d_minutes;   // minutes under test
            int d_seconds;   // seconds under test
            int d_milliSecs; // milli seconds under test
            int d_tzoffset;  // time zone offset
        } DATA[] = {
   //line no.  year   month   day   hour    min   sec    ms  offset
   //-------   -----  -----   ---   ----    ---   ---    --  ------
    {      L_,      1,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,     45     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    1,     1,     1,    1,    1,      0     },
    {      L_,      1,     1,    1,     1,     1,    1,    1,    500     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    1,     1,    23,   59,   59,      0     },
    {      L_,      1,     1,    1,     1,    23,   59,   59,   1439     },
    {      L_,      1,     1,    1,     1,    23,   59,   59,  -1439     },

    {      L_,      1,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,    2,     0,     0,    0,    0,   1439     },
    {      L_,      1,     1,    2,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    2,     1,     1,    1,    1,      0     },
    {      L_,      1,     1,    2,     1,     1,    1,    1,    500     },

    {      L_,      1,     1,    2,     1,    23,   59,   59,      0     },
    {      L_,      1,     1,    2,     1,    23,   59,   59,    500     },
    {      L_,      1,     1,    2,     1,    23,   59,   59,   -500     },

    {      L_,      1,     1,   10,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,   10,     1,     1,    1,    1,     99     },

    {      L_,      1,     1,   30,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,   31,     0,     0,    0,    0,   1439     },
    {      L_,      1,     1,   31,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     2,    1,     0,     0,    0,    0,      0     },
    {      L_,      1,     2,    1,    23,    59,   59,    0,   1439     },

    {      L_,      1,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,      1,    12,   31,    23,    59,   59,    0,   1439     },

    {      L_,      2,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      2,     1,    1,    23,    59,   59,    0,   1439     },

    {      L_,      4,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      4,     1,    1,    23,    59,   59,    0,   1439     },

    {      L_,      4,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,      4,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,      4,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,      4,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,      4,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,      4,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,      4,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,      4,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,      4,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,      8,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,      8,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,      8,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,      8,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,      8,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,      8,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,    100,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    100,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,    100,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,    100,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    100,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,    100,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    400,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,    400,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,    400,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,    400,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    400,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,    400,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,    500,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    500,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,    500,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    500,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,    800,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    800,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,    800,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,    800,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,    800,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    800,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   1000,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   1000,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,   1000,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   1000,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   2000,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,   2000,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   2000,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   2016,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2017,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2018,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2019,    12,   31,     0,     0,    0,    0,      0     },

    {      L_,   2020,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,   2020,     1,    1,     0,     0,    0,    0,   1439     },
    {      L_,   2020,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,   2020,     1,    1,    23,    59,   59,  999,      0     },
    {      L_,   2020,     1,    1,    23,    59,   59,  999,   1439     },
    {      L_,   2020,     1,    1,    23,    59,   59,  999,  -1439     },

    {      L_,   2020,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,   2020,     1,    2,     0,     0,    0,    0,   1439     },
    {      L_,   2020,     1,    2,     0,     0,    0,    0,  -1439     },

    {      L_,   2020,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   2020,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,   2020,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,   2020,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,   2020,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   2020,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,   2021,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,   2022,     1,    2,     0,     0,    0,    0,      0     },

    {      L_,   9999,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   9999,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,   9999,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,   9999,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   9999,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,   9999,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,   9999,    12,   30,     0,     0,    0,    0,      0     },
    {      L_,   9999,    12,   30,    23,    59,   59,    0,   1439     },

    {      L_,   9999,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   9999,    12,   31,    23,    59,   59,    0,   1439     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\n'bdlt::Date'.\n";
        {
            typedef bdlt::Date Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;

                if (veryVerbose) { P_(Y) P_(M) P(D) }

                const Type VALUE(Y, M, D); Type value;

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }
            }
        }

        if (verbose) cout << "\n'bdlt::DateTz'.\n";
        {
            typedef bdlt::DateTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdlt::Date(Y, M, D), OFF); Type value;

                if (veryVerbose) { P_(Y) P_(M) P_(D) P(OFF) }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }
            }
        }

        if (verbose) cout << "\n'bdlt::Time'.\n";
        {
            typedef bdlt::Time Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;

                if (veryVerbose) { P_(H) P_(MM) P_(S) P(MS) }

                const Type VALUE(H, MM, S, MS); Type value;
                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }
            }
        }

        if (verbose) cout << "\n'bdlt::TimeTz'.\n";
        {
            typedef bdlt::TimeTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdlt::Time(H, MM, S, MS), OFF); Type value;

                if (veryVerbose) { P_(H) P_(MM) P_(S) P_(MS) P(OFF) }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }
            }
        }

        if (verbose) cout << "\n'bdlt::Datetime'.\n";
        {
            typedef bdlt::Datetime Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;

                const Type VALUE(Y, M, D, H, MM, S, MS); Type value;

                if (veryVerbose) { P_(Y) P_(M) P_(D) P_(H)
                                   P_(MM) P_(S) P(MS) }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }
            }
        }

        if (verbose) cout << "\n'bdlt::DatetimeTz'.\n";
        {
            typedef bdlt::DatetimeTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdlt::Datetime(Y, M, D, H, MM, S, MS), OFF);
                Type value;

                if (veryVerbose) { P_(Y) P_(M) P_(D) P_(H)
                                   P_(MM) P_(S) P_(MS) P(OFF) }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder     encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // FUNCTIONS RELATED TO SKIPPED ELEMENTS
        //   This test exercises functions that apply to skipped elements.
        //
        // Concerns:
        //   a. The setNumUnknownElementsSkipped sets the number of skipped
        //      elements correctly.
        //   b. The numUnknownElementsSkipped returns the number of skipped
        //      elements correctly.
        //
        // Plan:
        //
        // Testing:
        //   void setNumUnknownElementsSKipped(int value);
        //   int numUnknownElementsSkipped() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nFUNCTIONS RELATED TO SKIPPED ELEMENTS"
                             "\n=====================================\n";

        if (verbose) cout << "\nSETTING AND GETTING NUM SKIPPED ELEMS\n";
        {
            balber::BerDecoder mX; const balber::BerDecoder& X = mX;
            ASSERT(0 == X.numUnknownElementsSkipped());

            const int DATA[] = { 0, 1, 5, 100, 2000 };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int i = 0; i < NUM_DATA; ++i) {
                const int NUM_SKIPPED_ELEMS = DATA[i];
                mX.setNumUnknownElementsSkipped(NUM_SKIPPED_ELEMS);
                LOOP3_ASSERT(i, NUM_SKIPPED_ELEMS,
                           X.numUnknownElementsSkipped(),
                           NUM_SKIPPED_ELEMS == X.numUnknownElementsSkipped());
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // SEQUENCE DECODED AS CHOICE
        //
        // Concerns:
        //   Decoder needs to be able to handle decoding a sequence as a
        //   choice without aborting, should simply log error messages.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nSEQUENCE DECODED AS CHOICE"
                             "\n==========================\n";

        if (verbose) cout << "\nNon-Empty Sequence\n";
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequence valueOut;
            valueOut.attribute1() = 34;
            valueOut.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MyChoice valueIn;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 != decoder.decode(&isb, &valueIn));
            ASSERT(decoder.loggedMessages().length() > 0);
            if (veryVerbose) {
                cout << "We expect the decoder to complain.  Here is what it "
                                                                     "says:\n";
                printDiagnostic(decoder);
            }
        }

        if (verbose) cout << "\nEmpty Sequence\n";
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequence valueOut;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MyChoice valueIn;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 != decoder.decode(&isb, &valueIn));
            ASSERT(decoder.loggedMessages().length() > 0);
            if (veryVerbose) {
                cout << "We expect the decoder to complain.  Here is what it "
                                                                     "says:\n";
                printDiagnostic(decoder);
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // CHOICE DECODED AS SEQUENCE
        //
        // Concerns:
        //   Decoder needs to be able to handle decoding a choice as a
        //   sequence without aborting, should simply log error messages.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nCHOICE DECODED AS SEQUENCE"
                             "\n==========================\n";

        if (verbose) cout << "\nNon-Null Choice\n";
        {

            bdlsb::MemOutStreamBuf osb;

            test::MyChoice valueOut;
            valueOut.makeSelection1();
            valueOut.selection1() = 34;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequence valueIn;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 != decoder.decode(&isb, &valueIn));
            ASSERT(decoder.loggedMessages().length() > 0);
            if (veryVerbose) {
                cout << "We expect the decoder to complain.  Here is what it "
                                                                     "says:\n";
                printDiagnostic(decoder);
            }
        }

        if (verbose) cout << "\nNull Choice\n";
        {

            bdlsb::MemOutStreamBuf osb;

            test::MyChoice valueOut;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequence valueIn;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 != decoder.decode(&isb, &valueIn));
            ASSERT(decoder.loggedMessages().length() > 0);
            if (veryVerbose) {
                cout << "We expect the decoder to complain.  Here is what it "
                                                                     "says:\n";
                printDiagnostic(decoder);
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // ENCODE AND DECODE NILLABLE VALUES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODE AND DECODE NILLABLE VALUES"
                             "\n=================================\n";

        if (verbose) cout << "\nNull value\n";
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithNillable valueOut;
            valueOut.attribute1() = 34;
            valueOut.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithNillable valueIn;
            valueIn.myNillable() = "Hello";

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose)
                cout << "\tDecoding from Nokalva (DEFINITE LENGTH).\n";

            {
                const char NOKALVA_DATA[] = "300C8001 22A10082 0548656C 6C6F";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithNillable valueFromNokalva;
                valueFromNokalva.myNillable() = "Hello";

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                               nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }

            if (veryVerbose)
                cout << "\tDecoding from Nokalva (INDEFINITE LENGTH).\n";
            {
                const char NOKALVA_DATA[] = "30808001 22A18000 00820548 "
                                            "656C6C6F 0000";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithNillable valueFromNokalva;
                valueFromNokalva.myNillable() = "Hello";

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                               nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) cout << "\nNon-null value.\n";
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithNillable valueOut;
            valueOut.attribute1() = 34;
            valueOut.myNillable() = "World!";
            valueOut.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithNillable valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose)
                cout << "\tDecoding from Nokalva (DEFINITE LENGTH).";
            {
                const char NOKALVA_DATA[] = "30148001 22A10880 06576F72 "
                                            "6C642182 0548656C 6C6F";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithNillable valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                               nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }

            if (veryVerbose)
                cout << "\tDecoding from Nokalva (INDEFINITE LENGTH).\n";
            {
                const char NOKALVA_DATA[] = "30808001 22A18080 06576F72 "
                                            "6C642100 00820548 656C6C6F 0000";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithNillable valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                               nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // ENCODE AND DECODE ANONYMOUS CHOICES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODE AND DECODE ANONYMOUS CHOICES"
                             "\n===================================\n";

        if (verbose) cout << "\nChoice with no selection.\n";
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithAnonymousChoice valueOut;
            valueOut.attribute1() = 34;
            valueOut.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithAnonymousChoice valueIn;
            valueIn.choice().makeMyChoice2("Hello");

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }

        if (verbose) cout << "\nChoice with selection.\n";
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithAnonymousChoice valueOut;
            valueOut.attribute1() = 34;
            valueOut.choice().makeMyChoice1(65);
            valueOut.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithAnonymousChoice valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose)
                cout << "\tDecoding from Nokalva (DEFINITE LENGTH).\n";
            {
                const char NOKALVA_DATA[] = "300F8001 22A10380 01418205 "
                                            "48656C6C 6F";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithAnonymousChoice valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }

            if (veryVerbose)
                cout << "\tDecoding from Nokalva (INDEFINITE LENGTH).\n";
            {
                const char NOKALVA_DATA[] = "30808001 22A18080 01410000 "
                                            "82054865 6C6C6F00 00";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithAnonymousChoice valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                               nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithAnonymousChoice valueOut;
            valueOut.attribute1() = 34;
            valueOut.choice().makeMyChoice2("World!");
            valueOut.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithAnonymousChoice valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose)
                cout << "\tDecoding from Nokalva (DEFINITE LENGTH).\n";
            {
                const char NOKALVA_DATA[] = "30148001 22A10881 06576F72 "
                                            "6C642182 0548656C 6C6F";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithAnonymousChoice valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }

            if (veryVerbose)
                cout << "\tDecoding from Nokalva (INDEFINITE LENGTH).\n";
            {
                const char NOKALVA_DATA[] = "30808001 22A18081 06576F72 "
                                            "6C642100 00820548 656C6C6F 0000";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithAnonymousChoice valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // ENCODE AND DECODE ARRAYS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODE AND DECODE ARRAYS"
                             "\n========================\n";

        if (verbose) cout << "\nEmpty array.\n";
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithArray valueOut;
            valueOut.attribute1() = 34;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithArray valueIn;
            valueIn.attribute2().push_back("This");
            valueIn.attribute2().push_back("is");
            valueIn.attribute2().push_back("a");
            valueIn.attribute2().push_back("test.");

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose)
                cout << "\tDecoding from Nokalva (DEFINITE LENGTH).\n";
            {
                const char NOKALVA_DATA[] = "30058001 22A100";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithArray valueFromNokalva;
                valueFromNokalva.attribute2().push_back("This");
                valueFromNokalva.attribute2().push_back("is");
                valueFromNokalva.attribute2().push_back("a");
                valueFromNokalva.attribute2().push_back("test.");

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                               nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }

            if (veryVerbose)
                cout << "\tDecoding from Nokalva (INDEFINITE LENGTH).\n";
            {
                const char NOKALVA_DATA[] = "30808001 22A18000 000000";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithArray valueFromNokalva;
                valueFromNokalva.attribute2().push_back("This");
                valueFromNokalva.attribute2().push_back("is");
                valueFromNokalva.attribute2().push_back("a");
                valueFromNokalva.attribute2().push_back("test.");

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                               nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) cout << "\nEmpty array and `encodeEmptyArrays` option "
                                                            "set to `false`\n";
        {
            balber::BerEncoderOptions options;
            options.setEncodeEmptyArrays(false);

            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithArray valueOut;
            valueOut.attribute1() = 34;

            balber::BerEncoder encoder(&options);
            ASSERT(0 == encoder.encode(&osb, valueOut));

            // verify array length hint is present
            bsl::size_t pos = bsl::string(osb.data(),
                                          osb.length()).find(k_ALH_PREFIX);
            ASSERT(bsl::string::npos == pos);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithArray valueIn;
            valueIn.attribute2().push_back("This");
            valueIn.attribute2().push_back("is");
            valueIn.attribute2().push_back("a");
            valueIn.attribute2().push_back("test.");

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }

        if (verbose) cout << "\nNon-empty array.\n";
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithArray valueOut;
            valueOut.attribute1() = 34;
            valueOut.attribute2().push_back("Hello");
            valueOut.attribute2().push_back("World!");

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithArray valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose)
                cout << "\tDecoding from Nokalva (DEFINITE LENGTH).\n";
            {
                const char NOKALVA_DATA[] = "30148001 22A10F0C 0548656C "
                                            "6C6F0C06 576F726C 6421";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithArray valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                               nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }

            if (veryVerbose)
                cout << "\tDecoding from Nokalva (INDEFINITE LENGTH).\n";
            {
                const char NOKALVA_DATA[] = "30808001 22A1800C 0548656C "
                                            "6C6F0C06 576F726C 64210000 0000";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithArray valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                               nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) cout << "\nEmpty array with `encodeArrayLengthHints`.\n";
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithArray valueOut;
            valueOut.attribute1() = 34;

            ASSERT(0 == encoderALH.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithArray valueIn;
            valueIn.attribute2().push_back("This");
            valueIn.attribute2().push_back("is");
            valueIn.attribute2().push_back("a");
            valueIn.attribute2().push_back("test.");

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }

        if (verbose) {
            cout << "\nNon-empty array with `encodeArrayLengthHints`.\n";
        }
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithArray valueOut;
            valueOut.attribute1() = 34;
            valueOut.attribute2().push_back("Hello");
            valueOut.attribute2().push_back("World!");

            ASSERT(0 == encoderALH.encode(&osb, valueOut));

            // verify array length hint is present
            bsl::string output(osb.data(), osb.length());
            bsl::size_t pos = output.find(k_ALH_PREFIX);
            ASSERT(bsl::string::npos != pos);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            {
                test::MySequenceWithArray valueIn;

                ASSERT(valueOut != valueIn);
                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &valueIn));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueIn);
                ASSERT(2 == valueIn.attribute2().capacity());
                ASSERT(0 == decoder.numUnknownElementsSkipped());
            }

            // simulate having a hint in old code
            output.replace(pos,
                           k_ALH_PREFIX_LENGTH,
                           k_ALH_SKIP,
                           k_ALH_PREFIX_LENGTH);

            {  // simulate decoding failure due to an unknown tag (the hint)
                balber::BerDecoderOptions optionsLocal(options);
                optionsLocal.setSkipUnknownElements(false);

                balber::BerDecoder decoder(&optionsLocal);

                test::MySequenceWithArray valueIn;

                ASSERT(valueOut != valueIn);
                bdlsb::FixedMemInStreamBuf isb(output.data(), output.length());
                ASSERT(0 != decoder.decode(&isb, &valueIn));
                printDiagnostic(decoder);

                ASSERT(valueOut != valueIn);
                ASSERT(1 == decoder.numUnknownElementsSkipped());
            }

            {  // simulate skipping an unknown tag (the hint)
                test::MySequenceWithArray valueIn;

                ASSERT(valueOut != valueIn);
                bdlsb::FixedMemInStreamBuf isb(output.data(), output.length());
                ASSERT(0 == decoder.decode(&isb, &valueIn));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueIn);
                ASSERT(1 == decoder.numUnknownElementsSkipped());
            }

            // replace correct hint
            output.replace(pos,
                           k_ALH_PREFIX_LENGTH,
                           k_ALH_PREFIX,
                           k_ALH_PREFIX_LENGTH);

            // set hint to 9, larger than it should be
            output[pos + k_ALH_PREFIX_LENGTH + 1] = '\x09';

            {  // verify reserved capacity
                test::MySequenceWithArray valueIn;

                ASSERT(valueOut != valueIn);
                bdlsb::FixedMemInStreamBuf isb(output.data(), output.length());
                ASSERT(0 == decoder.decode(&isb, &valueIn));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueIn);
                ASSERT(9 == valueIn.attribute2().capacity());
                ASSERT(0 == decoder.numUnknownElementsSkipped());
            }

            {  // verify reserved capacity when capacity is larger than max
                balber::BerDecoderOptions optionsLocal(options);
                optionsLocal.setMaxSequenceSize(5);

                balber::BerDecoder decoder(&optionsLocal);

                test::MySequenceWithArray valueIn;

                ASSERT(valueOut != valueIn);
                bdlsb::FixedMemInStreamBuf isb(output.data(), output.length());
                ASSERT(0 == decoder.decode(&isb, &valueIn));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueIn);
                ASSERT(5 == valueIn.attribute2().capacity());
                ASSERT(0 == decoder.numUnknownElementsSkipped());
            }
        }

        if (verbose) {
            cout << "\nThree arrays with `encodeArrayLengthHints`.\n";
        }
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithThreeArrays valueOut;
            {
                valueOut.attribute1().push_back("Hello");
                valueOut.attribute1().push_back("World!");
                valueOut.attribute1().push_back("alpha");
                valueOut.attribute1().push_back("beta");
                valueOut.attribute2().push_back(1);
                valueOut.attribute2().push_back(2);
                valueOut.attribute2().push_back(3);
                valueOut.attribute3().push_back(4.0);
                valueOut.attribute3().push_back(5.0);
                valueOut.attribute3().push_back(6.0);
                valueOut.attribute3().push_back(7.0);
                valueOut.attribute3().push_back(8.0);
            }

            ASSERT(0 == encoderALH.encode(&osb, valueOut));

            // verify array length hint is present exactly three times
            bsl::string output(osb.data(), osb.length());
            bsl::size_t pos = output.find(k_ALH_PREFIX);
            ASSERT(bsl::string::npos != pos);
            pos = output.find(k_ALH_PREFIX, pos + 1);
            ASSERT(bsl::string::npos != pos);
            pos = output.find(k_ALH_PREFIX, pos + 1);
            ASSERT(bsl::string::npos != pos);
            pos = output.find(k_ALH_PREFIX, pos + 1);
            ASSERT(bsl::string::npos == pos);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            {
                test::MySequenceWithThreeArrays valueIn;

                ASSERT(valueOut != valueIn);
                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &valueIn));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueIn);
                ASSERT(0 == decoder.numUnknownElementsSkipped());
            }
        }

        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // ENCODE AND DECODE NULLABLE VALUES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nENCODE AND DECODE NULLABLE VALUES"
                                  "\n=================================\n";

        if (verbose) cout << "\nNull value.\n";
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithNullable valueOut;
            valueOut.attribute1() = 34;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithNullable valueIn;
            valueIn.attribute1() = 356;
            valueIn.attribute2().makeValue("Hello");

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose)
                cout << "\tDecoding from Nokalva (DEFINITE LENGTH).\n";
            {
                const char NOKALVA_DATA[] = "30038001 22";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithNullable valueFromNokalva;
                valueFromNokalva.attribute1() = 356;
                valueFromNokalva.attribute2().makeValue("Hello");

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                               nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }

            if (veryVerbose)
                cout << "\tDecoding from Nokalva (INDEFINITE LENGTH).\n";
            {
                const char NOKALVA_DATA[] = "30808001 220000";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithNullable valueFromNokalva;
                valueFromNokalva.attribute1() = 356;
                valueFromNokalva.attribute2().makeValue("Hello");

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                               nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) cout << "\nNon-null value.\n";
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithNullable valueOut;
            valueOut.attribute1() = 34;
            valueOut.attribute2().makeValue("Hello");

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithNullable valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose)
                cout << "\tDecoding from Nokalva (DEFINITE LENGTH).\n";
            {
                const char NOKALVA_DATA[] = "300A8001 22810548 656C6C6F";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithNullable valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }

            if (veryVerbose)
                cout << "\tDecoding from Nokalva (INDEFINITE LENGTH).\n";
            {
                const char NOKALVA_DATA[] = "30808001 22810548 656C6C6F 0000";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithNullable valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // ENCODE AND DECODE CHOICES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nENCODE AND DECODE CHOICES"
                                  "\n=========================\n";

        if (verbose) cout << "\nChoice with no selection.\n";
        {
            bdlsb::MemOutStreamBuf osb;

            test::MyChoice valueOut;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MyChoice valueIn;
            valueIn.makeSelection2();
            valueIn.selection2() = "Hello";

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }

        if (verbose) cout << "\nChoice with selection.\n";
        {
            bdlsb::MemOutStreamBuf osb;

            test::MyChoice valueOut;
            valueOut.makeSelection1();
            valueOut.selection1() = 34;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MyChoice valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose)
                cout << "\tDecoding from Nokalva (DEFINITE LENGTH).\n";
            {
                const char NOKALVA_DATA[] = "3005A003 800122";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MyChoice valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                               nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }

            if (veryVerbose)
                cout << "\tDecoding from Nokalva (INDEFINITE LENGTH).\n";
            {
                const char NOKALVA_DATA[] = "3080A080 80012200 000000";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MyChoice valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                               nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // ENCODE AND DECODE SEQUENCES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODE AND DECODE SEQUENCES"
                             "\n===========================\n";

        bdlsb::MemOutStreamBuf osb;

        test::MySequence valueOut;
        valueOut.attribute1() = 34;
        valueOut.attribute2() = "Hello";

        ASSERT(0 == encoder.encode(&osb, valueOut));

        if (veryVerbose) {
            P(osb.length())
            printBuffer(osb.data(), osb.length());
        }

        test::MySequence valueIn;

        ASSERT(valueOut != valueIn);
        bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
        ASSERT(0 == decoder.decode(&isb, &valueIn));
        printDiagnostic(decoder);

        ASSERT(valueOut == valueIn);

        static const struct {
            int         d_line;
            const char *d_nokalvaData;
            int         d_numUnknownElements;
        } DATA[] = {
            //Line Nokalva Data                                     Num Unknown
            //==== ============                                     ===========

            // Definite length:
            { L_,  "300A 800122         810548656C6C6F"                  , 0 },
            { L_,  "300A 810548656C6C6F 800122"                          , 0 },

            { L_,  "300D 820199         800122         810548656C6C6F"   , 1 },
            { L_,  "300D 820199         810548656C6C6F 800122"           , 1 },
            { L_,  "300F A203820199     810548656C6C6F 800122"           , 1 },
            { L_,  "3011 A2808201990000 810548656C6C6F 800122"           , 1 },
            //           ^^^^^^: Extra (unknown) attr3

            { L_,  "300D 800122         820199         810548656C6C6F"   , 1 },
            { L_,  "300D 810548656C6C6F 820199         800122"           , 1 },
            { L_,  "300F 810548656C6C6F A203820199     800122"           , 1 },
            { L_,  "3011 810548656C6C6F A2808201990000 800122"           , 1 },
            //   Extra (unknown) attr3: ^^^^^^

            { L_,  "300D 800122         810548656C6C6F 820199"           , 1 },
            { L_,  "300D 810548656C6C6F 800122         820199"           , 1 },
            { L_,  "300F 810548656C6C6F 800122         A203820199"       , 1 },
            { L_,  "3011 810548656C6C6F 800122         A2808201990000"   , 1 },
            //                  Extra (unknown) attr3: ^^^^^^

            // Indefinite length:
            { L_,  "3080 800122         810548656C6C6F 0000"             , 0 },
            { L_,  "3080 810548656C6C6F 800122         0000"             , 0 },

            { L_,  "3080 820199         800122         810548656C6C6F 0000",1},
            { L_,  "3080 820199         810548656C6C6F 800122         0000",1},
            { L_,  "3080 A203820199     810548656C6C6F 800122         0000",1},
            { L_,  "3080 A2808201990000 810548656C6C6F 800122         0000",1},
            //           ^^^^^^: Extra (unknown) attr3

            { L_,  "3080 800122         820199         810548656C6C6F 0000",1},
            { L_,  "3080 810548656C6C6F 820199         800122         0000",1},
            { L_,  "3080 810548656C6C6F A203820199     800122         0000",1},
            { L_,  "3080 810548656C6C6F A2808201990000 800122         0000",1},
            //   Extra (unknown) attr3: ^^^^^^

            { L_,  "3080 800122         810548656C6C6F 820199         0000",1},
            { L_,  "3080 810548656C6C6F 800122         820199         0000",1},
            { L_,  "3080 810548656C6C6F 800122         A203820199     0000",1},
            { L_,  "3080 810548656C6C6F 800122         A2808201990000 0000",1},
            //                  Extra (unknown) attr3: ^^^^^^
        };

        static const int DATA_LEN = sizeof(DATA) / sizeof(DATA[0]);

        if (verbose) cout << "skipUnknownElements == true\n";
        for (int i = 0; i < DATA_LEN; ++i)
        {
            const int   LINE         = DATA[i].d_line;
            const char* NOKALVA_DATA = DATA[i].d_nokalvaData;
            const int   NUM_UNKNOWN  = DATA[i].d_numUnknownElements;

            bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);
            const char *berData = &nokalvaData[0];
            const bsl::size_t berDataLen = nokalvaData.size();

            test::MySequence valueFromNokalva;

            ASSERT(valueOut != valueFromNokalva);
            bdlsb::FixedMemInStreamBuf isb(berData, berDataLen);
            decoder.setNumUnknownElementsSkipped(0);
            int ret = decoder.decode(&isb, &valueFromNokalva);
            printDiagnostic(decoder);
            LOOP3_ASSERT(LINE,
                         NUM_UNKNOWN,
                         decoder.numUnknownElementsSkipped(),
                         NUM_UNKNOWN == decoder.numUnknownElementsSkipped());

            LOOP_ASSERT(LINE, 0 == ret);
            LOOP_ASSERT(LINE,
     berDataLen == static_cast<bsl::size_t>(isb.pubseekoff(0, bsl::ios::cur)));
            LOOP_ASSERT(LINE, valueOut == valueFromNokalva);
        }

        if (verbose) cout << "skipUnknownElements == false\n";
        for (int i = 0; i < DATA_LEN; ++i)
        {
            const int   LINE         = DATA[i].d_line;
            const char* NOKALVA_DATA = DATA[i].d_nokalvaData;
            const int   NUM_UNKNOWN  = DATA[i].d_numUnknownElements;

            bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);
            const char *berData = &nokalvaData[0];
            const bsl::size_t berDataLen = nokalvaData.size();

            test::MySequence valueFromNokalva;

            ASSERT(valueOut != valueFromNokalva);
            bdlsb::FixedMemInStreamBuf isb(berData, berDataLen);

            balber::BerDecoderOptions opts;
            opts.setSkipUnknownElements(false);
            if (veryVeryVerbose) {
               opts.setTraceLevel(1);
            }

            balber::BerDecoder decoder2(&opts);
            int ret = decoder2.decode(&isb, &valueFromNokalva);
            printDiagnostic(decoder2);

            if (NUM_UNKNOWN) {
                LOOP_ASSERT(LINE, 0 != ret);
                LOOP_ASSERT(LINE,
                          NUM_UNKNOWN == decoder2.numUnknownElementsSkipped());
            }
            else {
                LOOP_ASSERT(LINE,
     berDataLen == static_cast<bsl::size_t>(isb.pubseekoff(0, bsl::ios::cur)));
                LOOP_ASSERT(LINE, valueOut == valueFromNokalva);
            }
        }

        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // ENCODE AND DECODE ENUMERATIONS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODE AND DECODE ENUMERATIONS"
                             "\n==============================\n";

        if (verbose) cout << "\nTesting with VALUE1.\n";
        {
            bdlsb::MemOutStreamBuf osb;

            test::MyEnumeration::Value valueOut = test::MyEnumeration::VALUE1;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MyEnumeration::Value valueIn = test::MyEnumeration::VALUE2;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose) cout << "\tDecoding from Nokalva.\n";
            {
                const char NOKALVA_DATA[] = "0A0100";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MyEnumeration::Value valueFromNokalva
                                                 = test::MyEnumeration::VALUE2;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) cout << "\nVALUE2.\n";
        {
            bdlsb::MemOutStreamBuf osb;

            test::MyEnumeration::Value valueOut = test::MyEnumeration::VALUE2;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MyEnumeration::Value valueIn = test::MyEnumeration::VALUE1;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose) cout << "\tDecoding from Nokalva.\n";
            {
                const char NOKALVA_DATA[] = "0A0101";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MyEnumeration::Value valueFromNokalva
                                                 = test::MyEnumeration::VALUE1;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) cout << "\nUnknown value and no fallback.\n";
        {
            if (veryVerbose)
                cout << "\tDecoding from explicit binary string.\n";

            const char DATA[] = "0A0102";

            bsl::vector<char> data = loadFromHex(DATA);

            test::MyEnumeration::Value valueIn = test::MyEnumeration::VALUE1;

            bdlsb::FixedMemInStreamBuf isb(&data[0], data.size());
            ASSERT(0 != decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(test::MyEnumeration::VALUE1 == valueIn);  // unchanged
        }

        if (verbose) cout << "\nTesting with unknown value and fallback.\n";
        {
            if (veryVerbose)
                cout << "\tDecoding from explicit binary string.\n";

            const char DATA[] = "0A0103";

            bsl::vector<char> data = loadFromHex(DATA);

            test::MyEnumerationWithFallback::Value valueIn =
                                       test::MyEnumerationWithFallback::VALUE1;

            bdlsb::FixedMemInStreamBuf isb(&data[0], data.size());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(test::MyEnumerationWithFallback::UNKNOWN == valueIn);
        }

        if (verbose)
            cout << "\nEncode with no fallback then decode with fallback\n";
        {
            const char *enumValueStrings[] = {"VALUE1", "VALUE2"};
            const bsl::size_t numEnumValueStrings =
                                                   bsl::size(enumValueStrings);
            for (bsl::size_t i = 0; i < numEnumValueStrings; i++) {
                const char *enumValueString = enumValueStrings[i];
                const int   enumValueLength =
                                static_cast<int>(bsl::strlen(enumValueString));
                bdlsb::MemOutStreamBuf osb;
                test::MyEnumeration::Value valueOut;
                ASSERT(0 == test::MyEnumeration::fromString(&valueOut,
                                                            enumValueString,
                                                            enumValueLength));
                test::MyEnumerationWithFallback::Value valueExpected;
                ASSERT(0 == test::MyEnumerationWithFallback::fromString(
                                                             &valueExpected,
                                                             enumValueString,
                                                             enumValueLength));

                ASSERT(0 == encoder.encode(&osb, valueOut));

                if (veryVerbose) {
                    P(enumValueString)
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                test::MyEnumerationWithFallback::Value valueIn =
                                      test::MyEnumerationWithFallback::UNKNOWN;

                ASSERT(valueExpected != valueIn);
                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &valueIn));
                printDiagnostic(decoder);

                ASSERT(valueExpected == valueIn);
            }
        }

        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // ENCODE AND DECODE CUSTOMIZED TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODE AND DECODE CUSTOMIZED TYPES"
                             "\n==================================\n";

        const bsl::string VALUE = "Hello";

        if (verbose) cout << "\nEncoding customized string.\n";
        {
            bdlsb::MemOutStreamBuf osb;

            test::CustomizedString valueOut;
            valueOut.fromString(VALUE);

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::CustomizedString valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose) cout << "\tDecoding from Nokalva.\n";
            {
                const char NOKALVA_DATA[] = "0C054865 6C6C6F";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::CustomizedString valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                               nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) cout << "\nEncoding bsl::string (control).\n";
        {
            bdlsb::MemOutStreamBuf osb;

            bsl::string valueOut = VALUE;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::CustomizedString valueIn;

            ASSERT(valueOut != valueIn.toString());
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn.toString());
        }

        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // ENCODE AND DECODE VOCABULARY TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODE AND DECODE VOCABULARY TYPES"
                             "\n==================================\n";

        if (verbose) bsl::cout << "\t'bdlt::Date'\n";
        {
            const int YEAR = 2005, MONTH = 12, DAY = 15;

            bdlt::Date valueOut(YEAR, MONTH, DAY);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            bdlt::Date valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }

        if (verbose) bsl::cout << "\t'bdlt::DateTz'\n";
        {
            const int YEAR = 2005, MONTH = 12, DAY = 15, OFFSET = 45;

            bdlt::DateTz valueOut(bdlt::Date(YEAR, MONTH, DAY), OFFSET);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            bdlt::DateTz valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }

        if (verbose) bsl::cout << "\t'bdlt::Time'\n";
        {
            const int HOUR = 12, MIN = 56, SECS = 9, MILLISECS = 134;

            bdlt::Time valueOut(HOUR, MIN, SECS, MILLISECS);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            bdlt::Time valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }

        if (verbose) bsl::cout << "\t'bdlt::TimeTz'\n";
        {
            const int HOUR   = 12, MIN = 56, SECS = 9, MILLISECS = 134,
                      OFFSET = 45;

            bdlt::TimeTz valueOut(bdlt::Time(HOUR, MIN, SECS, MILLISECS),
                                  OFFSET);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            bdlt::TimeTz valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }

        if (verbose) bsl::cout << "\t'bdlt::Datetime'\n";
        {
            const int YEAR = 2005, MONTH = 12, DAY = 15;
            const int HOUR = 12, MIN = 56, SECS = 9, MILLISECS = 134;

            bdlt::Datetime valueOut(YEAR, MONTH, DAY,
                                    HOUR, MIN, SECS, MILLISECS);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            bdlt::Datetime valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }

        if (verbose) bsl::cout << "\t'bdlt::DatetimeTz'\n";
        {
            const int YEAR   = 2005, MONTH = 12, DAY = 15;
            const int HOUR   = 12, MIN = 56, SECS = 9, MILLISECS = 134;
            const int OFFSET = 45;

            bdlt::DatetimeTz valueOut(bdlt::Datetime(YEAR, MONTH, DAY,
                                                     HOUR, MIN,
                                                     SECS, MILLISECS),
                                      OFFSET);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            bdlt::DatetimeTz valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // ENCODE AND DECODE REAL/FLOATING-POINT
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODE AND DECODE REAL/FLOATING-POINT"
                             "\n=====================================\n";

        {
            const float  XA1 = 99.234f;              float  XA2;
            const float  XB1 = -100.987f;            float  XB2;
            const float  XC1 = -77723.875f;          float  XC2;
            const float  XD1 = 1.4E-45f;             float  XD2;
            const float  XE1 = -1.4E-45f;            float  XE2;
            const float  XF1 = 3.402823466E+38f;     float  XF2;
            const float  XG1 = 1.175494351E-38f;     float  XG2;

            const double XH1 = 19998989.1234;        double  XH2;
            const double XI1 = -7.8752345;           double  XI2;
            const double XJ1 = 1.4E-45;              double  XJ2;
            const double XK1 = -1.4E-45;             double  XK2;
            const double XL1 = 3.402823466E+38;      double  XL2;
            const double XM1 = 1.175494351E-38;      double  XM2;
            const double XN1 = 1e-307;               double  XN2;

            const float  XO1 = 0;                     float XO2;
            const double XP1 = 0;                     double XP2;

            const double XQ1 = -1E-307;                     double XQ2;
            const double XR1 = -1.98347E-296;               double XR2;
            const double XS1 = 78979.23E-304;               double XS2;
            const double XT1 = 79879879249686698E-100;      double XT2;
            const double XU1 = -9999999999999999E+25;       double XU2;

            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XA1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XA2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XA1, XA2, XA1 == XA2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XB1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XB2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XB1, XB2, XB1 == XB2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XC1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XC2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XC1, XC2, XC1 == XC2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XD1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XD2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XD1, XD2, XD1 == XD2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XE1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XE2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XE1, XE2, XE1 == XE2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XF1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XF2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XF1, XF2, XF1 == XF2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XG1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XG2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XG1, XG2, XG1 == XG2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XH1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XH2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XH1, XH2, XH1 == XH2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XI1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XI2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XI1, XI2, XI1 == XI2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XJ1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XJ2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XJ1, XJ2, XJ1 == XJ2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XK1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XK2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XK1, XK2, XK1 == XK2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XL1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XL2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XL1, XL2, XL1 == XL2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XM1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XM2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XM1, XM2, XM1 == XM2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XN1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XN2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XN1, XN2, XN1 == XN2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XO1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XO2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XO1, XO2, XO1 == XO2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XP1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XP2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XP1, XP2, XP1 == XP2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XQ1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XQ2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XQ1, XQ2, XQ1 == XQ2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XR1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XR2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XR1, XR2, XR1 == XR2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XS1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XS2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XS1, XS2, XS1 == XS2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XT1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XT2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XT1, XT2, XT1 == XT2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XU1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XU2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XU1, XU2, XU1 == XU2);
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
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "\nFUNDAMENTALS TEST"
                             "\n=================\n";

        {
            const unsigned char   XA1 = UCHAR_MAX;    unsigned char     XA2;
            const signed   char   XB1 = SCHAR_MIN;    signed   char     XB2;
            const          char   XC1 = SCHAR_MAX;             char     XC2;

            const unsigned short  XD1 = USHRT_MAX;    unsigned short    XD2;
            const signed   short  XE1 = SHRT_MIN;     signed   short    XE2;
            const          short  XF1 = SHRT_MAX;              short    XF2;

            const unsigned int    XG1 = UINT_MAX;     unsigned int      XG2;
            const signed   int    XH1 = INT_MIN;      signed   int      XH2;
            const          int    XI1 = INT_MAX;               int      XI2;

            const unsigned long   XJ1 = ULONG_MAX;    unsigned long     XJ2;
            const signed   long   XK1 = LONG_MIN;     signed   long     XK2;
            const          long   XL1 = LONG_MAX;              long     XL2;

            const bsls::Types::Int64  XM1 = 0xff34567890123456LL;
            bsls::Types::Int64        XM2;
            const bsls::Types::Uint64 XN1 = 0x1234567890123456LL;
            bsls::Types::Uint64       XN2;

            const          bool   XO1 = true;             bool         XO2;

            const bsl::string     XP1("This is a really long line");
            bsl::string                                               XP2;

            const float        XQ1 = 99.234f;           float         XQ2;
            const float        XR1 = -100.987f;         float         XR2;
            const float        XS1 = -77723.875f;       float         XS2;

            const double       XT1 = 19998989.1234;     double        XT2;
            const double       XU1 = -7.8752345;        double        XU2;

            const double       XV1 = 100.1;             double        XV2;
            const double       XW1 = 200.1;             double        XW2;
            const double       XX1 = 2500.1;            double        XX2;
            const double       XY1 = 1600.1;            double        XY2;
            const double       XZ1 = 100;               double        XZ2;

            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XA1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XA2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XA1, XA2, XA1 == XA2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XB1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XB2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XB1, XB2, XB1 == XB2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XC1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XC2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XC1, XC2, XC1 == XC2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XD1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XD2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XD1, XD2, XD1 == XD2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XE1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XE2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XE1, XE2, XE1 == XE2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XF1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XF2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XF1, XF2, XF1 == XF2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XG1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XG2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XG1, XG2, XG1 == XG2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XH1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XH2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XH1, XH2, XH1 == XH2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XI1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XI2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XI1, XI2, XI1 == XI2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XJ1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XJ2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XJ1, XJ2, XJ1 == XJ2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XK1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XK2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XK1, XK2, XK1 == XK2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XL1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XL2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XL1, XL2, XL1 == XL2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XM1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XM2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XM1, XM2, XM1 == XM2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XN1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XN2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XN1, XN2, XN1 == XN2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XO1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XO2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XO1, XO2, XO1 == XO2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XP1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XP2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XP1, XP2, XP1 == XP2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XQ1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XQ2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XQ1, XQ2, XQ1 == XQ2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XR1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XR2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XR1, XR2, XR1 == XR2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XS1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XS2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XS1, XS2, XS1 == XS2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XT1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XT2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XT1, XT2, XT1 == XT2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XU1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XU2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XU1, XU2, XU1 == XU2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XV1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XV2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XV1, XV2, XV1 == XV2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XW1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XW2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XW1, XW2, XW1 == XW2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XX1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XX2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XX1, XX2, XX1 == XX2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XY1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XY2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XY1, XY2, XY1 == XY2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XZ1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XZ2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XZ1, XZ2, XZ1 == XZ2);
            }
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        // --------------------------------------------------------------------

        static const bsl::size_t MAX_BUF_SIZE = 100000;

        int reps         = 1000;
        int arraySize    = 200;
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
        bsl::size_t         minOutputSize;
        switch (requestType) {
          case 's': {
              cout << "sqrt request\n";

              test::Sqrt sqrt;
              sqrt.value() = 3.1415927;
              request.makeSqrt(sqrt);
              minOutputSize = 2;
          } break;
          case 'r': {
              cout << "basicRecord request\n";

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
              cout << "bigRecord request with array size of "
                   << arraySize << '\n';

              test::BasicRecord basicRec;
              basicRec.i1() = 11;
              basicRec.i2() = 22;
              basicRec.dt() = bdlt::DatetimeTz(
                  bdlt::Datetime(bdlt::Date(2007, 9, 3),
                                bdlt::Time(16, 30)), 0);
              basicRec.s() = "The quick brown fox jumped over the lazy dog.";
              bsl::size_t minBasicRecSize =
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
            bsl::cerr << "Unknown request type: -" << requestType << '\n';
            return 1;                                                 // RETURN
        }

        cout << "  " << reps << " repetitions...\n";

        bdlsb::MemOutStreamBuf osb;
        osb.reserveCapacity(MAX_BUF_SIZE);

        bsls::Stopwatch stopwatch;
        double elapsed;

        // Measure ber encoding and decoding times:
        stopwatch.reset();
        stopwatch.start();
        for (int i = 0; i < reps; ++i) {
            osb.pubseekpos(0);
            balber::BerEncoder encoder;  // Typical usage: single-use object
            encoder.encode(&osb, request);
        }
        stopwatch.stop();

        ASSERT(minOutputSize <= osb.length());
        ASSERT(osb.length() <= MAX_BUF_SIZE);
        elapsed = stopwatch.elapsedTime();
        ASSERT(elapsed > 0);

        cout << "    balber::BerEncoder: "
             << elapsed          << " seconds, "
             << (reps / elapsed) << " reps/sec, "
             << osb.length()     << " bytes\n";

        test::TimingRequest        *inRequests = 0;
        bdlsb::FixedMemInStreamBuf  isb(osb.data(), osb.length());

        isb.pubsetbuf(osb.data(), osb.length());
        inRequests = new test::TimingRequest[reps];
        stopwatch.reset();
        stopwatch.start();
        for (int i = 0; i < reps; ++i) {
            isb.pubseekpos(0);
            balber::BerDecoder decoder;  // Typical usage: single-use object
            decoder.decode(&isb, &inRequests[i]);
        }
        stopwatch.stop();

        if (veryVeryVerbose) { P(request); P(*inRequests); }
        ASSERT(*inRequests == request);
        elapsed = stopwatch.elapsedTime();
        ASSERT(elapsed > 0);
        delete[] inRequests;

        cout << "    balber::BerDecoder: "
             << elapsed          << " seconds, "
             << (reps / elapsed) << " reps/sec\n";
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
