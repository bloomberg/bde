// baejsn_tokenizer.t.cpp                                             -*-C++-*-
#include <baejsn_tokenizer.h>

#include <baejsn_parserutil.h>

#include <bsl_sstream.h>
#include <bsl_cfloat.h>
#include <bsl_climits.h>
#include <bsl_limits.h>
#include <bsl_iostream.h>
#include <bdepu_typesparser.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bdesb_memoutstreambuf.h>            // for testing only
#include <bdesb_fixedmemoutstreambuf.h>       // for testing only
#include <bdesb_fixedmeminstreambuf.h>        // for testing only

#include <bcem_aggregate.h>

using namespace BloombergLP;
using namespace bsl;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a tokenizer for traversing a stream
// filled with JSON data and allows populating an in-memory structure with
// almost no memory allocations.  The implementation works as a finite state
// machine moving from one token to another when the 'advanceToNextToken'
// function is called.  The majority of this test driver tests that function by
// starting at a particular token, calling that function, and ensuring that
// after the advance the next token and the data value is as expected.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] baejsn_Tokenizer(bslma_Allocator *bA = 0);
// [ 2] ~baejsn_Tokenizer();
//
// MANIPULATORS
// [ 9] void reset(bsl::streambuf &streamBuf);
// [ 3] int advanceToNextToken();
//
// ACCESSORS
// [ 3] TokenType tokenType() const;
// [ 3] int value(bslstl::StringRef *data) const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] USAGE EXAMPLE

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define LOOP0_ASSERT ASSERT

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP1_ASSERT LOOP_ASSERT

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// The 'BSLS_BSLTESTUTIL_EXPAND' macro is required to workaround a
// pre-proccessor issue on windows that prevents __VA_ARGS__ to be expanded in
// the definition of 'BSLS_BSLTESTUTIL_NUM_ARGS'
#define EXPAND(X)                                            \
    X

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)        \
    N

#define NUM_ARGS(...)                                        \
    EXPAND(NUM_ARGS_IMPL( __VA_ARGS__, 5, 4, 3, 2, 1, 0, ""))

#define LOOPN_ASSERT_IMPL(N, ...)                            \
    EXPAND(LOOP ## N ## _ASSERT(__VA_ARGS__))

#define LOOPN_ASSERT(N, ...)                                 \
    LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...)                                         \
    LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)


#define WS "   \t       \n      \v       \f       \r       "

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

enum { SUCCESS = 0, FAILURE = -1 };

typedef baejsn_Tokenizer Obj;

bsl::ostream& operator<<(bsl::ostream& stream, Obj::TokenType value)
{
#define CASE(X) case(Obj::X): stream << #X; break;

    switch (value) {
      CASE(BAEJSN_BEGIN)
      CASE(BAEJSN_ELEMENT_NAME)
      CASE(BAEJSN_START_OBJECT)
      CASE(BAEJSN_END_OBJECT)
      CASE(BAEJSN_START_ARRAY)
      CASE(BAEJSN_END_ARRAY)
      CASE(BAEJSN_ELEMENT_VALUE)
      CASE(BAEJSN_ERROR)
      default: stream << "(* UNKNOWN *)"; break;
    }
    return stream;

#undef CASE
}

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    bool verbose         = argc > 2;
    bool veryVerbose     = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma_TestAllocator          globalAllocator("global", veryVeryVerbose);
    bslma_Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
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

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Extracting JSON data into an object
///----------------------------------------------
// For this example, we will use 'baejsn_Tokenizer' to read each node in a
// JSON document and populate a simple 'Employee' object.
//
// First, we will define the JSON data that the tokenizer will traverse over:
//..
    const char *INPUT = "    {\n"
                        "        \"street\" : \"Lexington Ave\",\n"
                        "        \"state\" : \"New York\",\n"
                        "        \"zipcode\" : 10022\n"
                        "    }";
//..
// Next, we will construct populate a 'streambuf' with this data:
//..
    bdesb_FixedMemInStreamBuf isb(INPUT, bsl::strlen(INPUT));
//..
// Then, we will create a 'baejsn_Tokenizer' object and associate the above
// streambuf with it:
//..
    baejsn_Tokenizer tokenizer;
    tokenizer.reset(&isb);
//..
// Next, we will create a 'bcem_Aggregate' representing an employee:
//..
    bcema_SharedPtr<bdem_Schema> schema(new bdem_Schema);
//
    bdem_RecordDef *addressRecord = schema->createRecord("Address");
    addressRecord->appendField(bdem_ElemType::BDEM_STRING, "street");
    addressRecord->appendField(bdem_ElemType::BDEM_STRING, "state");
    addressRecord->appendField(bdem_ElemType::BDEM_INT, "zipcode");
//
    bcem_Aggregate address(schema, "Address");
//..
// Then, we will traverse the JSON data one node at a time:
//..
    // Read '{'

    int rc = tokenizer.advanceToNextToken();
    ASSERT(!rc);

    baejsn_Tokenizer::TokenType token = tokenizer.tokenType();
    ASSERT(baejsn_Tokenizer::BAEJSN_START_OBJECT == token);

    rc = tokenizer.advanceToNextToken();
    ASSERT(!rc);
    token = tokenizer.tokenType();

    // Continue reading elements till '}' is encountered

    while (baejsn_Tokenizer::BAEJSN_END_OBJECT != token) {
        ASSERT(baejsn_Tokenizer::BAEJSN_ELEMENT_NAME == token);

        // Read element name

        bslstl::StringRef nodeValue;
        rc = tokenizer.value(&nodeValue);
        ASSERT(!rc);

        bsl::string elementName = nodeValue;

        // Read element value

        int rc = tokenizer.advanceToNextToken();
        ASSERT(!rc);

        token = tokenizer.tokenType();
        ASSERT(baejsn_Tokenizer::BAEJSN_ELEMENT_VALUE == token);

        rc = tokenizer.value(&nodeValue);
        ASSERT(!rc);

        // Extract the simple type with the data

        if (bdem_ElemType::BDEM_STRING == address.fieldType(elementName)) {

            bsl::string data;
            rc = baejsn_ParserUtil::getValue(&data, nodeValue);
            ASSERT(!rc);

            // Populate the element with the read value

            address.setField(elementName, data);
        }
        else if (bdem_ElemType::BDEM_INT == address.fieldType(elementName)) {
            int data;
            rc = baejsn_ParserUtil::getValue(&data, nodeValue);
            ASSERT(!rc);

            // Populate the element with the read value

            address.setField(elementName, data);
        }

        rc = tokenizer.advanceToNextToken();
        ASSERT(!rc);
        token = tokenizer.tokenType();
    }
//..
// Finally, we will verify that the 'address' aggregate has the correct values:
//..
    ASSERT("Lexington Ave" == address["street"].asString());
    ASSERT("New York"      == address["state"].asString());
    ASSERT(10022           == address["zipcode"].asInt());
//..
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING that large values (greater than 1K) are handled correctly
        //
        // Concerns:
        //: 1 Values of larger sizes are handled correctly.
        //:
        //: 2 Only values larger than 1K result in an allocation.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text and whether external memory is
        //:   allocated.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'iss', with the input text and
        //:     including the opening brace and name.
        //:
        //:   2 Create a 'baejsn_Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'iss' with 'mX'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' the number of times
        //:     to get to a value token.
        //:
        //:   4 Confirm that the value of that token is as expected.
        //:
        //:   5 Verify that memory is allocated when expected.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING that large values (greater than 1K) "
                          << "are handled correctly" << endl;

        const bsl::string LARGE_STRING =
            "\"selection11selection1element1255element43123123123elementasdd52"
            "element6999999element7customelement8999element10255255elementsd11"
            "element12element13255255element14element1531231231233123123112323"
            "selection6arbitrarystringvalueselection7element1element1elements2"
            "element4element5element1element1element2element4element5elemenst1"
            "element1element2element4element6LONDONLONDONelement2truetruement6"
            "element31.51.5element4element5-980123-980123element6elemen123t608"
            "2012-08-18T132500.000+00002012-08-18T132500.000+0000element7ment6"
            "element6LONDONLONDONelement2truetrueelement31.51.5elemenst4ment68"
            "element5-980123-980123element62012-08-18T132500.000+0000element68"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelem123ent1"
            "element2element4element5element1element1element2element4edlement5"
            "element1element1element2element4element6LONDONLONDONelemendt2true"
            "trueelement31.51.5element4element5-980123-980123element6ement2tre"
            "2012-08-18T132500.000+00002012-08-18T132500.000+0000elemement2nt7"
            "element6LONDONLONDONelement2truetrueelement31.51.5element4ent2tue"
            "element5-980123-980123element62012-08-18T132500.000+0000ement2tue"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelemesdfnt1"
            "element2element4element5element1element1element2element4elemendt5"
            "element1element1element2element4element6LONDONLONDONelement2trdue"
            "trueelement31.51.5element4element5-980123-980123element6ent2tuesd"
            "2012-08-18T132500.000+00002012-08-18T132500.000+0000element7sdsdd"
            "element6LONDONLONDONelement2truetrueelement31.515element4element5"
            "-980123-980123element62012-08-18T132500.000+00005element4element5"
            "2012-08-18T132500.000+0000element7element62012-08-18Tsdf132500000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeldfsement1"
            "element2element4element5element1element1element2element4eledment5"
            "element1element1element2element4element6LONDONLONDONelement2dtrue"
            "trueelement31.51.5element4element5-980123-980123element6201+0d000"
            "2012-08-18T132500.000+0000element7element62012-08-18Tsdf132500000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeldfsement1"
            "element2element4element5element1element1element2element4eledment5"
            "element1element1element2element4element6LONDONLONDONelement2dtrue"
            "trueelement31.51.5element4element5-980123-980123element6201+0d000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeleme2trdue"
            "trueelement31.51.5element4element5-980123-980123element62010000ds"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelemsdfent2"
            "selection11selection1element1255element43123123123elementasdd5234"
            "element6999999element7customelement8999element10255255elementsd11"
            "element12element13255255element14element1531231231233123123112323"
            "selection6arbitrarystringvalueselection7element1element1elements2"
            "element4element5element1element1element2element4element5elemenst1"
            "element1element2element4element6LONDONLONDONelement2truetruement6"
            "element31.51.5element4element5-980123-980123element6elemen123t608"
            "2012-08-18T132500.000+00002012-08-18T132500.000+0000element7ment6"
            "element6LONDONLONDONelement2truetrueelement31.51.5elemenst4ment68"
            "element5-980123-980123element62012-08-18T132500.000+0000element68"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelem123ent1"
            "element2element4element5element1element1element2element4edlement5"
            "element1element1element2element4element6LONDONLONDONelemendt2true"
            "trueelement31.51.5element4element5-980123-980123element6ement2tre"
            "2012-08-18T132500.000+00002012-08-18T132500.000+0000elemement2nt7"
            "element6LONDONLONDONelement2truetrueelement31.515element4element5"
            "-980123-980123element62012-08-18T132500.000+0000980123elemefnt608"
            "2012-08-18T132500.000+0000element7element62012-08-18T132501230000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelems12ent1"
            "element2element4element5element1element1element2element4eledment5"
            "element1element1element2element4element6LONDONLONDONelement2dtrue"
            "trueelement31.51.5element4element5-980123-980123element6201+0d000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeleme2trdue"
            "trueelement31.51.5element4element5-980123-980123element62010000df"
            "selection11selection1element1255element43123123123elementasdd5255"
            "element6999999element7customelement8999element10255255elementsd11"
            "element12element13255255element14element1531231231233123123112323"
            "selection6arbitrarystringvalueselection7element1element1elements2"
            "element4element5element1element1element2element4element5elemenst1"
            "element1element2element4element6LONDONLONDONelement2truetruement6"
            "element31.51.5element4element5-980123-980123element6elemen123t608"
            "2012-08-18T132500.000+00002012-08-18T132500.000+0000element7ment6"
            "element6LONDONLONDONelement2truetrueelement31.51.5elemenst4ment68"
            "element5-980123-980123element62012-08-18T132500.000+0000element68"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelem123ent1"
            "element2element4element5element1element1element2element4edlement5"
            "element1element1element2element4element6LONDONLONDONelemendt2true"
            "trueelement31.51.5element4element5-980123-980123element6ement2tre"
            "2012-08-18T132500.000+00002012-08-18T132500.000+0000elemement2nt7"
            "element6LONDONLONDONelement2truetrueelement31.515element4element5"
            "-980123-980123element62012-08-18T132500.000+0000980123elemefnt608"
            "2012-08-18T132500.000+0000element7element62012-08-18T132501230000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelems12ent1"
            "element2element4element5element1element1element2element4eledment5"
            "element1element1element2element4element6LONDONLONDONelement2dtrue"
            "trueelement31.51.5element4element5-980123-980123element6201+0d000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeleme2trdue"
            "trueelement31.51.5element4element5-980123-980123element62010000df"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelemensaft2"
            "selection11selection1element1255element43123123123elemensdfft5255"
            "element6999999element7customelement8999element10255255elementdf11"
            "element12element13255255element14element15312312312331231231d2dd3"
            "selection6arbitrarystringvalueselection7element1element1elemednt2"
            "element4element5element1element1element2element4element5elemdent1"
            "element1element2element4element6LONDONLONDONelement2truasdfdetrue"
            "element31.51.5element4element5-980123-980123element6ement2truetre"
            "2012-08-18T132500.000+00002012-08-18T132500.000+0000elasdffement7"
            "element6LONDONLONDONelement2truetrueelement31.51.5element4ent2tue"
            "element5-980123-980123element62012-08-18T132500.000+0000ement2tue"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelemesdfnt1"
            "element2element4element5element1element1element2element4elemendt5"
            "element1element1element2element4element6LONDONLONDONelement2trdue"
            "trueelement31.51.5element4element5-980123-980123element6ent2tuesd"
            "2012-08-18T132500.000+00002012-08-18T132500.000+0000element7sdsdd"
            "element6LONDONLONDONelement2truetrueelement31.515element4element5"
            "-980123-980123element62012-08-18T132500.000+00005element4element5"
            "2012-08-18T132500.000+0000element7element62012-08-18Tsdf132500000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeldfsement1"
            "element2element4element5element1element1element2element4eledment5"
            "element1element1element2element4element6LONDONLONDONelement2dtrue"
            "trueelement31.51.5element4element5-980123-980123element6201+0d000"
            "2012-08-18T132500.000+0000element7element62012-08-18Tsdf132500000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeldfsement1"
            "element2element4element5element1element1element2element4eledment5"
            "element1element1element2element4element6LONDONLONDONelement2dtrue"
            "trueelement31.51.5element4element5-980123-980123element6201+0d000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeleme2trdue"
            "trueelement31.51.5element4element5-980123-980123element62010000ds"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelemsdfent2"
            "element1element1element2element4element6LONDONLONDONelement2dtrue"
            "trueelement31.51.5element4element5-980123-980123element6201+0d000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeleme2trdue"
            "trueelement31.51.5element4element5-980123-980123element62010000ds"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelemsdfent2"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeleme2trdue"
            "trueelement31.51.5element4element5-980123-980123element62010000ds"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelemsdfent2"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelemsdfent2"
            "2012-08-18T132500.000+0000element7element6LONDONLONDON";

        const struct {
            int               d_line;
            const bsl::string d_suffixText;
            bool              d_allocatesMemory;
        } DATA[] = {

        // Line  Suffix Text                    Allocates memory flag
        // ----  -----------                    --------------------

#if !defined(BSLS_PLATFORM_CPU_64_BIT)
        // 32-bit

        {   L_,  "ABC\"",                              false                 },
        {   L_,  "ABCD\"",                             false                 },
        {   L_,  "ABCDE\"",                            true                  },
        {   L_,  "ABCDE12345678901234567890\"",        true                  },

#else
        // 64-bit

        {   L_,  "12345678ABC\"",                      false                 },
        {   L_,  "12345678ABCD\"",                     false                 },
        {   L_,  "12345678ABCDE\"",                    true                  },
        {   L_,  "12345678ABCDE1234567890123456789\"", true                  },

#endif

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ ti) {
            const int    LINE   = DATA[ti].d_line;
            const string SUFFIX = DATA[ti].d_suffixText;
            const bool   ALLOC  = DATA[ti].d_allocatesMemory;
            const string TEXT   = LARGE_STRING + SUFFIX;

            if (veryVerbose) {
                P(LINE) P(TEXT) P(ALLOC)
            }

            bsl::ostringstream os;
            os << "{\"n\":" << TEXT << "}";

            bsl::istringstream is(os.str());

            bslma::TestAllocator ta;

            Obj mX(&ta);  const Obj& X = mX;
            mX.reset(is.rdbuf());

            ASSERTV(0                         == mX.advanceToNextToken());
            ASSERTV(Obj::BAEJSN_START_OBJECT  == X.tokenType());

            ASSERTV(0                         == mX.advanceToNextToken());
            ASSERTV(Obj::BAEJSN_ELEMENT_NAME  == X.tokenType());

            ASSERTV(LINE, 0                   == mX.advanceToNextToken());
            ASSERTV(Obj::BAEJSN_ELEMENT_VALUE == X.tokenType());

            bslstl::StringRef data;
            ASSERTV(0                      == X.value(&data));
            ASSERTV(LINE, TEXT, data, TEXT == data);

            if (ALLOC) {
                ASSERTV(LINE, ta.numBlocksTotal() > 0);
            }
            else {
                ASSERTV(LINE, 0 == ta.numBlocksTotal());
            }
        }

        {
            bsl::ostringstream os;
            os << "{\"Sequence\":{";
            for (int ti = 0; ti < NUM_DATA; ++ ti) {
                const int    LINE   = DATA[ti].d_line;
                const string SUFFIX = DATA[ti].d_suffixText;
                const string TEXT   = LARGE_STRING + SUFFIX;

                os << "\"" << 'a' << "\":" << TEXT << ",";
            }

            bsl::ostringstream ds;
            const string S = LARGE_STRING + DATA[NUM_DATA - 1].d_suffixText;
            const string T(S.begin() + 1, S.end() - 1);
            const string VERY_LARGE_STRING = "\"" + T + T + T + T + "\"";

            for (int ti = 0; ti < NUM_DATA - 1; ++ ti) {
                os << "\"" << 'a' << "\":" << VERY_LARGE_STRING << ",";
            }

            os << "\"" << 'a' << "\":" << VERY_LARGE_STRING << "}";

            bsl::istringstream is(os.str());

            if (veryVerbose) {
                P(os.str())
            }

            Obj mX;  const Obj& X = mX;
            mX.reset(is.rdbuf());

            ASSERTV(0                         == mX.advanceToNextToken());
            ASSERTV(Obj::BAEJSN_START_OBJECT  == X.tokenType());

            ASSERTV(0                         == mX.advanceToNextToken());
            ASSERTV(Obj::BAEJSN_ELEMENT_NAME  == X.tokenType());

            ASSERTV(0                         == mX.advanceToNextToken());
            ASSERTV(Obj::BAEJSN_START_OBJECT  == X.tokenType());

            for (int ti = 0; ti < NUM_DATA; ++ ti) {
                const int    LINE   = DATA[ti].d_line;
                const string SUFFIX = DATA[ti].d_suffixText;
                const string TEXT   = LARGE_STRING + SUFFIX;

                ASSERTV(LINE, 0                    == mX.advanceToNextToken());
                ASSERTV(LINE, Obj::BAEJSN_ELEMENT_NAME  == X.tokenType());

                ASSERTV(LINE, 0                    == mX.advanceToNextToken());
                ASSERTV(LINE, Obj::BAEJSN_ELEMENT_VALUE == X.tokenType());

                bslstl::StringRef data;
                ASSERTV(LINE, 0                == X.value(&data));
                ASSERTV(LINE, TEXT, data, TEXT == data);
            }

            for (int ti = 0; ti < NUM_DATA; ++ ti) {
                ASSERTV(0                         == mX.advanceToNextToken());
                ASSERTV(Obj::BAEJSN_ELEMENT_NAME  == X.tokenType());

                ASSERTV(0                         == mX.advanceToNextToken());
                ASSERTV(Obj::BAEJSN_ELEMENT_VALUE == X.tokenType());

                bslstl::StringRef data;
                ASSERTV(0                                 == X.value(&data));
                ASSERTV(VERY_LARGE_STRING, data, VERY_LARGE_STRING == data);
            }

            ASSERTV(0                       == mX.advanceToNextToken());
            ASSERTV(Obj::BAEJSN_END_OBJECT  == X.tokenType());
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'reset'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'reset'" << endl
                          << "===============" << endl;

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' TO BAEJSN_END_ARRAY
        //
        // Concerns:
        //: 1 The following transitions are correctly handled:
        //:
        //:   1 START_ARRAY    -> END_ARRAY                          '[' -> ']'
        //:
        //:   2 VALUE (number) -> END_ARRAY                        VALUE -> ']'
        //:
        //:   3 VALUE (string) -> END_ARRAY                          '"' -> ']'
        //:
        //:   4 END_OBJECT     -> END_ARRAY                          '}' -> ']'
        //:
        //:   5 END_ARRAY      -> END_ARRAY                          ']' -> ']'
        //:
        //: 2 The return code is 0 on success and non-zero on failure.
        //
        // Errors:
        //: 1 The following transitions return an error:
        //:
        //:   1 NAME (no ':')         -> END_ARRAY                   '"' -> ']'
        //:
        //:   2 NAME (with ':')       -> END_ARRAY                   ':' -> ']'
        //:
        //:   3 NAME (with ',')       -> END_ARRAY           NAME -> ',' -> ']'
        //:
        //:   4 VALUE (with ',')      -> END_ARRAY          VALUE -> ',' -> ']'
        //:
        //:   5 START_OBJECT          -> END_ARRAY                   '{' -> ']'
        //:
        //:   6 END_OBJECT (with ',') -> END_ARRAY            '}' -> ',' -> ']'
        //:
        //:   7 END_ARRAY (with ',')  -> END_ARRAY            ']' -> ',' -> ']'
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text, the number of times to invoke
        //:   'advanceToNextToken', the result of an additional invocation of
        //:   'advanceToNextToken', and the expected token and value, if
        //:   applicable, after that invocation.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'iss', with the input text.
        //:
        //:   2 Create a 'baejsn_Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'iss' with 'mX'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' the number of times
        //:     specified in that row.
        //:
        //:   4 Invoke 'advanceToNextToken' one more time and record the
        //:     return value, the token type, and the value of that token.
        //:
        //:   5 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.  Also confirm that the token type is as expected.
        //:     Finally, if that token is expected to have a value, then the
        //:     value of that token is as expected.
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                 << "TESTING 'advanceToNextToken' TO BAEJSN_END_ARRAY" << endl
                 << "================================================" << endl;

        const struct {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {

            // '[' - ']'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                              "]",
                3,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS               "]" WS,
                3,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "["
                WS                     "]",
                3,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock " WS " price\""
                WS                          ":"
                WS                            "["
                WS                            "]" WS,
                3,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },

            // value (integer) -> ']'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012"
                                         "]",
                4,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1, 2"
                                     "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012"
                WS                          "]",
                4,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "["
                WS                       "1.500012"
                WS                                "]",
                4,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },

            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012,"
                                "-2.12345"
                                          "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012,"
                                "-2.12345,"
                                "0"
                                          "]",
                6,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "["
                WS                       "1.500012" WS ","
                WS                       "-2.12345"
                WS                                       "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012,"
                WS                 "-2.12345,"
                WS                 "0"
                WS                           "]",
                6,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012" WS ","
                WS                 "-2.12345" WS ","
                WS                 "0"
                WS                                 "]",
                6,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },

            // value (string) -> ']'
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\""
                                         "]",
                4,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""
                WS                          "]",
                4,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS            "["
                WS             "\"" WS "John" WS "\""
                WS                                   "]" WS,
                4,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"full name\""
                WS                 ":"
                WS                   "["
                WS                     "\"" WS "John" WS "Doe" WS "\""
                WS                                                   "]",
                4,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },

            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "\"Smith\""
                                          "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "\"Smith\","
                                "\"Ryan\""
                                           "]",
                6,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\","
                WS                 "\"Smith\""
                WS                           "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\","
                WS                 "\"Smith\","
                WS                 "\"Ryan\""
                WS                           "]",
                6,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""  WS ","
                WS                 "\"Smith\"" WS 
                WS                                  "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""  WS ","
                WS                 "\"Smith\"" WS ","
                WS                 "\"Ryan\""  WS 
                WS                                  "]",
                6,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS            "["
                WS              "\"" WS "John"  WS "\"" WS ","
                WS              "\"" WS "Smith" WS "\"" WS
                WS                                           "]" WS,
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS            "["
                WS              "\"" WS "John"  WS "\"" WS ","
                WS              "\"" WS "Smith" WS "\"" WS ","
                WS              "\"" WS "Ryan" WS "\""  WS
                WS                                           "]" WS,
                6,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"full name\""
                WS                 ":"
                WS                   "["
                WS                    "\"" WS "John"  WS "Doe"  WS "\"" WS ","
                WS                    "\"" WS "Black" WS "Jack" WS "\"" WS 
                WS                                                         "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"full name\""
                WS                 ":"
                WS                   "["
                WS                    "\"" WS "John"  WS "Doe"  WS "\"" WS ","
                WS                    "\"" WS "Black" WS "Jack" WS "\"" WS "," 
                WS                    "\"" WS "New"   WS "Deal" WS "\"" WS
                WS                                                         "]",
                6,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },

            // '}' -> ']'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                "}"
                              "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "}"
                WS               "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                  "\"bid_price\""
                                                ":"
                                                  "1.500012"
                                "}"
                              "]",
                7,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS               "]",
                7,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                  "\"name\""
                                           ":"
                                             "\"CompanyName\""
                                "},"
                                "{"
                                  "\"bid_price\""
                                                ":"
                                                  "1.500012"
                                "}"
                               "]",
                11,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"name\""
                WS                            ":"
                WS                              "\"CompanyName\""
                WS                 "},"
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS               "]",
                11,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"name\""
                WS                            ":"
                WS                              "\"CompanyName\""
                WS                 "},"
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS               "]" WS,
                11,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },

            // ']' -> ']'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "["
                                "]"
                              "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "["
                WS                 "]"
                WS               "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "["
                                  "\"bid_price\""
                                "]"
                              "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "["
                WS                   "\"bid_price\""
                WS                 "]"
                WS               "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "["
                                "],"
                                "["
                                "]"
                              "]",
                6,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "["
                WS                 "],"
                WS                 "["
                WS                 "]"
                WS               "]",
                6,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "["
                                  "123"
                                "],"
                                "["
                                  "456"
                                "]"
                              "]",
                8,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "["
                WS                   "123"
                WS                 "],"
                WS                 "["
                WS                   "456"
                WS                 "]"
                WS               "]",
                8,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },

            // Error
            {
                L_,
                "{"
                  "\"name\""
                           "]",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            "]",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "]",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "]",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ","
                             "]",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ","
                WS              "]",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "\"John\""
                                      ","
                                        "]",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "\"John\""
                WS                       ","
                WS                         "]",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "{"
                               "]",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "{"
                WS                "]",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "["
                               "1,"
                             "]",
                4,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "["
                WS                "1,"
                WS              "]",
                4,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "["
                               "{"
                               "},"
                             "]",
                5,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "["
                WS                "{"
                WS                "},"
                WS              "]",
                5,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "["
                               "["
                               "],"
                             "]",
                5,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "["
                WS                "["
                WS                "],"
                WS              "]",
                5,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ ti) {
            const int            LINE      = DATA[ti].d_line;
            const string         TEXT      = DATA[ti].d_text_p;
            const int            PRE_MOVES = DATA[ti].d_preMoves;
            const bool           IS_VALID  = DATA[ti].d_validFlag;
            const Obj::TokenType EXP_TOKEN = DATA[ti].d_expTokenType;
            const bool           HAS_VALUE = DATA[ti].d_hasValue;
            const string         EXP_VALUE = DATA[ti].d_value_p;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::BAEJSN_BEGIN == X.tokenType());

            mX.reset(iss.rdbuf());

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(LINE, i, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(),
                        Obj::BAEJSN_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' TO BAEJSN_START_ARRAY
        //
        // Concerns:
        //: 1 The following transitions are correctly handled:
        //:
        //:   1 NAME             -> START_ARRAY                      ':' -> '['
        //:
        //:   2 START_ARRAY      -> START_ARRAY                      '[' -> '['
        //:
        //:   3 END_ARRAY        -> START_ARRAY               ']' -> ',' -> '['
        //:
        //: 2 The return code is 0 on success and non-zero on failure.
        //
        // Errors:
        //: 1 The following transitions return an error:
        //:
        //:   1 NAME (no ':')    -> START_ARRAY                      '"' -> '['
        //:
        //:   2 VALUE (with ',') -> START_ARRAY             VALUE -> ',' -> '['
        //:
        //:   3 START_OBJECT     -> START_ARRAY                      '{' -> '['
        //:
        //:   4 END_OBJECT       -> START_ARRAY                      '}' -> '['
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text, the number of times to invoke
        //:   'advanceToNextToken', the result of an additional invocation of
        //:   'advanceToNextToken', and the expected token and value, if
        //:   applicable, after that invocation.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'iss', with the input text.
        //:
        //:   2 Create a 'baejsn_Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'iss' with 'mX'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' the number of times
        //:     specified in that row.
        //:
        //:   4 Invoke 'advanceToNextToken' one more time and record the
        //:     return value, the token type, and the value of that token.
        //:
        //:   5 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.  Also confirm that the token type is as expected.
        //:     Finally, if that token is expected to have a value, then the
        //:     value of that token is as expected.
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                      << "TESTING 'advanceToNextToken' TO BAEJSN_NAME" << endl
                      << "===========================================" << endl;

        const struct {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {

            // name -> value (array), i.e. name -> '['
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "[",
                2,
                true,
                Obj::BAEJSN_START_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "[" WS,
                2,
                true,
                Obj::BAEJSN_START_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "[" WS,
                2,
                true,
                Obj::BAEJSN_START_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock " WS " price\""
                WS                          ":"
                WS                            "[" WS,
                2,
                true,
                Obj::BAEJSN_START_ARRAY,
                false,
                ""
            },

            // '[' -> '[' (array of arrays)
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "[",
                3,
                true,
                Obj::BAEJSN_START_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "[",
                3,
                true,
                Obj::BAEJSN_START_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "[" WS,
                3,
                true,
                Obj::BAEJSN_START_ARRAY,
                false,
                ""
            },

            // '[' -> '[' (array of arrays)
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "["
                                "],"
                                "[",
                5,
                true,
                Obj::BAEJSN_START_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "["
                WS                 "],"
                WS                 "[",
                5,
                true,
                Obj::BAEJSN_START_ARRAY,
                false,
                ""
            },

            // Error
            {
                L_,
                "{"
                  "\"price\""
                            "[",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             "[",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "1.500012,"
                                        "[",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "1.500012,"
                WS                         "[",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "[",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "[",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                              "},"
                              "[",
                4,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS               "},"
                WS               "[",
                4,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ ti) {
            const int            LINE      = DATA[ti].d_line;
            const string         TEXT      = DATA[ti].d_text_p;
            const int            PRE_MOVES = DATA[ti].d_preMoves;
            const bool           IS_VALID  = DATA[ti].d_validFlag;
            const Obj::TokenType EXP_TOKEN = DATA[ti].d_expTokenType;
            const bool           HAS_VALUE = DATA[ti].d_hasValue;
            const string         EXP_VALUE = DATA[ti].d_value_p;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::BAEJSN_BEGIN == X.tokenType());

            mX.reset(iss.rdbuf());

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, 0 == mX.advanceToNextToken());
                ASSERTV(X.tokenType(), Obj::BAEJSN_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' TO BAEJSN_END_OBJECT
        //
        // Concerns:
        //: 1 The following transitions are correctly handled:
        //:
        //:   1 START_OBJECT     -> END_OBJECT                       '{' -> '}'
        //:
        //:   2 VALUE (number)   -> END_OBJECT                     VALUE -> '}'
        //:
        //:   3 VALUE (string)   -> END_OBJECT                       '"' -> '}'
        //:
        //:   4 START_OBJECT     -> END_OBJECT                '[' -> '{' -> '}'
        //:
        //:   5 START_OBJECT     -> END_OBJECT                '{' -> '{' -> '}'
        //:
        //:   6 END_OBJECT       -> END_OBJECT         '{' -> '{' -> '}' -> '}'
        //:
        //:   7 END_OBJECT       -> END_OBJECT  '{' -> '{' -> '{' -> '}' -> '}'
        //:
        //:   8 END_ARRAY        -> END_OBJECT                '[' -> ']' -> '}'
        //:
        //: 2 The return code is 0 on success and non-zero on failure.
        //
        // Errors:
        //: 1 The following transitions return an error:
        //:
        //:   1 NAME             -> END_OBJECT                    '"'  -> '}'
        //:
        //:   2 NAME (with ':')  -> END_OBJECT                    ':'  -> '}'
        //:
        //:   3 VALUE (with ',') -> END_OBJECT                  VALUE  -> '}'
        //:
        //:   4 START_ARRAY      -> END_OBJECT                  '['    -> VALUE
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text, the number of times to invoke
        //:   'advanceToNextToken', the result of an additional invocation of
        //:   'advanceToNextToken', and the expected token and value, if
        //:   applicable, after that invocation.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'iss', with the input text.
        //:
        //:   2 Create a 'baejsn_Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'iss' with 'mX'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' the number of times
        //:     specified in that row.
        //:
        //:   4 Invoke 'advanceToNextToken' one more time and record the
        //:     return value, the token type, and the value of that token.
        //:
        //:   5 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.  Also confirm that the token type is as expected.
        //:     Finally, if that token is expected to have a value, then the
        //:     value of that token is as expected.
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                << "TESTING 'advanceToNextToken' TO BAEJSN_END_OBJECT" << endl
                << "=================================================" << endl;

        const struct {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {

            // '{' -> '}'
            {
                L_,
                "{"
                "}",
                1,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{\n"
                "}",
                1,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                   "}",
                1,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{" WS
                "}",
                1,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                "}" WS,
                1,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{\n"
                WS "}",
                1,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS "}"
                WS,
                1,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },

            // value (integer) -> '}'
            {
                L_,
                "{"
                 "\"price\""
                          ":"
                           "1.500012"
                                    "}",
                3,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"price\""
                WS           ":"
                WS            "1.500012"
                WS                     "}" WS,
                3,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"stock " WS " price\""
                WS                        ":"
                WS                          "1.500012"
                WS                                   "}" WS,
                3,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },

            // value (string) -> '}'
            {
                L_,
                "{"
                 "\"name\""
                         ":"
                          "\"John\""
                                   "}",
                3,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS           "\"John\""
                WS                    "}" WS,
                3,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS           "\"" WS "John" WS "\""
                WS                                "}" WS,
                3,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"full " WS " name\""
                WS                      ":"
                WS                       "\"" WS "John" WS " Doe" WS "\""
                WS                                                      "}" WS,
                3,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },

            // '[' -> '{' -> '}'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                "}",
                4,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "}",
                4,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "}" WS,
                4,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },

            // '{' -> '{' -> '}'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "\"full name\""
                                              ":"
                                                "{"
                                                "}",
                5,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "{"
                WS                                 "}",
                5,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "{"
                WS                                 "}" WS,
                5,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },

            // '{' -> '{' -> '}' -> '}'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "\"full name\""
                                              ":"
                                                "{"
                                                "}"
                              "}",
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "{"
                WS                                 "}"
                WS               "}",
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "{"
                WS                                 "}" WS
                WS               "}",
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },

            // '{' -> '{' -> '{' -> '}' -> '}' -> '}
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "\"full name\""
                                              ":"
                                                "{"
                                                "}"
                              "}"
                "}",
                7,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "{"
                WS                                 "}"
                WS               "}"
                WS "}",
                7,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "{"
                WS                                 "}" WS
                WS               "}"
                WS "}",
                7,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },

            // '[' -> '{' -> '}' -> '{' -> '}'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                "},"
                                "{"
                                "}",
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "},"
                WS                 "{"
                WS                 "}",
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "},"
                WS                 "{"
                WS                 "}" WS,
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },

            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                "}"
                              "]"
                "}",
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "}"
                WS               "]"
                WS "}",
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                  "\"bid_price\":1.500012"
                                "}"
                              "]"
                "}",
                8,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS               "]"
                WS "}",
                8,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                  "\"name\":\"CompanyName\","
                                  "\"bid_price\":1.500012"
                                 "}"
                               "]"
                "}",
                10,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"name\""
                WS                            ":"
                WS                              "\"CompanyName\""
                WS                 "},"
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS               "]"
                WS "}",
                12,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },

            // '[' -> ']' -> '}'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "\"full name\""
                                              ":"
                                                "["
                                                "]"
                              "}",
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "["
                WS                                 "]"
                WS               "}",
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "["
                WS                                 "]" WS
                WS               "}",
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },

            // Error
            {
                L_,
                "{"
                  "\"name\""
                           "}",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            "}",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "}",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "}",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ","
                             "}",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ","
                WS              "}",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "\"John\""
                                      ","
                                        "}",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "\"John\""
                WS                       ","
                WS                         "}",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "["
                               "}",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "["
                WS                "}",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ ti) {
            const int            LINE      = DATA[ti].d_line;
            const string         TEXT      = DATA[ti].d_text_p;
            const int            PRE_MOVES = DATA[ti].d_preMoves;
            const bool           IS_VALID  = DATA[ti].d_validFlag;
            const Obj::TokenType EXP_TOKEN = DATA[ti].d_expTokenType;
            const bool           HAS_VALUE = DATA[ti].d_hasValue;
            const string         EXP_VALUE = DATA[ti].d_value_p;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::BAEJSN_BEGIN == X.tokenType());

            mX.reset(iss.rdbuf());

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, 0 == mX.advanceToNextToken());
                ASSERTV(X.tokenType(), Obj::BAEJSN_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' TO BAEJSN_VALUE
        //
        // Concerns:
        //: 1 The following transitions are correctly handled:
        //:
        //:   1 NAME           -> VALUE (number)                   ':' -> VALUE
        //:
        //:   2 NAME           -> VALUE (string)                   ':' -> VALUE
        //:
        //:   3 START_ARRAY    -> VALUE (number)                   '[' -> VALUE
        //:
        //:   4 START_ARRAY    -> VALUE (string)                   '[' -> VALUE
        //:
        //:   5 VALUE (number) -> VALUE (number)                 VALUE -> VALUE
        //:
        //:   6 VALUE (string) -> VALUE (string)                 VALUE -> VALUE
        //:
        //:   7 VALUE (number) -> VALUE (string)                 VALUE -> VALUE
        //:
        //:   8 VALUE (string) -> VALUE (number)                 VALUE -> VALUE
        //:
        //: 2 The return code is 0 on success and non-zero on failure.
        //
        // Errors:
        //: 1 The following transitions return an error:
        //:
        //:   1 VALUE (no ,)   -> VALUE                         VALUE  -> VALUE
        //:
        //:   2 END_OBJECT     -> VALUE                           '}'  -> VALUE
        //:
        //:   3 END_ARRAY      -> VALUE                           ']'  -> VALUE
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text, the number of times to invoke
        //:   'advanceToNextToken', the result of an additional invocation of
        //:   'advanceToNextToken', and the expected token and value, if
        //:   applicable, after that invocation.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'iss', with the input text.
        //:
        //:   2 Create a 'baejsn_Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'iss' with 'mX'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' the number of times
        //:     specified in that row.
        //:
        //:   4 Invoke 'advanceToNextToken' one more time and record the
        //:     return value, the token type, and the value of that token.
        //:
        //:   5 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.  Also confirm that the token type is as expected.
        //:     Finally, if that token is expected to have a value, then the
        //:     value of that token is as expected.
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                     << "TESTING 'advanceToNextToken' TO BAEJSN_VALUE" << endl
                     << "============================================" << endl;

        const struct {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {

            // name -> value (integer)
            {
                L_,
                "{"
                 "\"price\""
                          ":"
                           "1.500012"
                                    "}",
                2,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "1.500012"
            },
            {
                L_,
                WS "{"
                WS  "\"price\""
                WS           ":"
                WS            "1.500012"
                WS                     "}" WS,
                2,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "1.500012"
            },
            {
                L_,
                WS "{"
                WS  "\"stock " WS " price\""
                WS                        ":"
                WS                          "1.500012"
                WS                                   "}" WS,
                2,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "1.500012"
            },

            // name -> value (string)
            {
                L_,
                "{"
                 "\"name\""
                         ":"
                          "\"John\""
                                   "}",
                2,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"John\""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS           "\"John\""
                WS                    "}" WS,
                2,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"John\""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS           "\"" WS "John" WS "\""
                WS                                "}" WS,
                2,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"" WS "John" WS "\""
            },
            {
                L_,
                WS "{"
                WS  "\"full " WS " name\""
                WS                      ":"
                WS                       "\"" WS "John" WS " Doe" WS "\""
                WS                                                      "}" WS,
                2,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"" WS "John" WS " Doe" WS "\""
            },
            {
                L_,
                WS "{"
                WS  "\"full " WS " name\""
                WS                      ":"
                WS                        "\"John " WS " Doe\""
                WS                                            "}" WS,
                2,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"John " WS " Doe\""
            },

            // '[' -> value (integer)
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012"
                                         "]",
                3,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "1.500012"
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012"
                WS                          "]",
                3,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "1.500012"
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "["
                WS                       "1.500012"
                WS                                "]",
                3,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "1.500012"
            },

            // '[' -> value (string)
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\""
                                         "]",
                3,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"John\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""
                WS                          "]",
                3,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"John\""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS            "["
                WS             "\"" WS "John" WS "\""
                WS                                   "]" WS,
                3,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"" WS "John" WS "\""
            },
            {
                L_,
                WS "{"
                WS   "\"full name\""
                WS                 ":"
                WS                   "["
                WS                     "\"" WS "John" WS "Doe" WS "\""
                WS                                                   "]",
                3,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"" WS "John" WS "Doe" WS "\""
            },

            // value (integer) -> value (integer)
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012,"
                                "-2.12345"
                                          "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "-2.12345"
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012,"
                                "-2.12345,"
                                "0"
                                          "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "0"
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "["
                WS                       "1.500012" WS ","
                WS                       "-2.12345"
                WS                                       "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "-2.12345"
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012,"
                WS                 "-2.12345,"
                WS                 "0"
                WS                           "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "0"
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012" WS ","
                WS                 "-2.12345" WS ","
                WS                 "0"
                WS                                 "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "0"
            },

            // value (string) -> value (string)
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "\"Smith\""
                                          "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Smith\""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "\"Smith\","
                                "\"Ryan\""
                                           "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Ryan\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\","
                WS                 "\"Smith\""
                WS                           "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Smith\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\","
                WS                 "\"Smith\","
                WS                 "\"Ryan\""
                WS                           "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Ryan\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "["
                WS                "\"John\""  WS ","
                WS                "\"Smith\"" WS 
                WS                                 "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Smith\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""  WS ","
                WS                 "\"Smith\"" WS ","
                WS                 "\"Ryan\""  WS 
                WS                                  "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Ryan\""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS            "["
                WS              "\"" WS "John"  WS "\"" WS ","
                WS              "\"" WS "Smith" WS "\"" WS
                WS                                           "]" WS,
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"" WS "Smith" WS "\""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS            "["
                WS              "\"" WS "John"  WS "\"" WS ","
                WS              "\"" WS "Smith" WS "\"" WS ","
                WS              "\"" WS "Ryan" WS "\"" WS
                WS                                           "]" WS,
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"" WS "Ryan" WS "\""
            },
            {
                L_,
                WS "{"
                WS   "\"full name\""
                WS                 ":"
                WS                   "["
                WS                    "\"" WS "John"  WS "Doe"  WS "\"" WS ","
                WS                    "\"" WS "Black" WS "Jack" WS "\"" WS 
                WS                                                         "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"" WS "Black" WS "Jack" WS "\""
            },
            {
                L_,
                WS "{"
                WS   "\"full name\""
                WS                 ":"
                WS                   "["
                WS                    "\"" WS "John"  WS "Doe"  WS "\"" WS ","
                WS                    "\"" WS "Black" WS "Jack" WS "\"" WS "," 
                WS                    "\"" WS "New"   WS "Deal" WS "\"" WS 
                WS                                                         "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"" WS "New" WS "Deal" WS "\""
            },

            // value (integer) -> value (string)
            // value (string)  -> value (integer)
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "12345,"
                                "\"Smith\""
                                          "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Smith\""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "12345,"
                                "\"Ryan\""
                                           "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Ryan\""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"Smith\","
                                "12345"
                                          "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "12345"
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "\"Ryan\","
                                "12345"
                                          "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "12345"
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "12345,"
                WS                 "\"Smith\""
                WS                           "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Smith\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\","
                WS                 "12345,"
                WS                 "\"Ryan\""
                WS                           "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Ryan\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "["
                WS                "\"John\""  WS ","
                WS                "12345"     WS 
                WS                                 "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "12345"
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""  WS ","
                WS                 "\"Smith\"" WS ","
                WS                 "12345"     WS 
                WS                                  "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "12345"
            },

            // Error
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "\"John\""
                             " "
                             "12345",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "\"John\""
                WS              " "
                WS              "12345",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "12345"
                             " "
                             "\"John\"",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "12345"
                WS              " "
                WS              "\"John\"",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "["
                               "\"John\""
                               " "
                               "12345",
                4,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "["
                WS                "\"John\""
                WS                " "
                WS                "12345",
                4,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"bid_price\""
                                ":"
                                  "{"
                                  "}"
                                    "12345",
                4,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"bid_price\""
                WS                 ":"
                WS                   "{"
                WS                   "}"
                WS                     "12345",
                4,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"bid_price\""
                                ":"
                                  "["
                                  "]"
                                    "12345",
                4,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"bid_price\""
                WS                 ":"
                WS                   "["
                WS                   "]"
                WS                     "12345",
                4,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"bid_price\""
                                ":"
                                  "["
                                    "1"
                                    " "
                                    "2"
                                  "]",
                4,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"bid_price\""
                WS                 ":"
                WS                   "["
                WS                     "1"
                WS                     " "
                WS                     "2"
                WS                   "]",
                4,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ ti) {
            const int            LINE      = DATA[ti].d_line;
            const string         TEXT      = DATA[ti].d_text_p;
            const int            PRE_MOVES = DATA[ti].d_preMoves;
            const bool           IS_VALID  = DATA[ti].d_validFlag;
            const Obj::TokenType EXP_TOKEN = DATA[ti].d_expTokenType;
            const bool           HAS_VALUE = DATA[ti].d_hasValue;
            const string         EXP_VALUE = DATA[ti].d_value_p;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::BAEJSN_BEGIN == X.tokenType());

            mX.reset(iss.rdbuf());

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(),
                        Obj::BAEJSN_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' TO BAEJSN_NAME
        //
        // Concerns:
        //: 1 The following transitions are correctly handled:
        //:
        //:   1 START_OBJECT   -> NAME                               '{' -> '"'
        //:
        //:   2 END_OBJECT     -> NAME          ':' -> '{' -> '}' -> ',' -> '"'
        //:
        //:   3 END_ARRAY      -> NAME          ':' -> '[' -> ']' -> ',' -> '"'
        //:
        //:   4 VALUE (number) -> NAME               ':' -> VALUE -> ',' -> '"'
        //:
        //:   5 VALUE (string) -> NAME               ':' -> VALUE -> ',' -> '"'
        //:
        //: 2 The return code is 0 on success and non-zero on failure.
        //
        // Errors:
        //: 1 The following transitions return an error:
        //:
        //:   1 END_OBJECT (no ',') -> NAME                         '}'  -> '"'
        //:
        //:   2 END_ARRAY (no ',')  -> NAME                         ']'  -> '"'
        //:
        //:   3 NAME                -> NAME                         '"'  -> '"'
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text, the number of times to invoke
        //:   'advanceToNextToken', the result of an additional invocation of
        //:   'advanceToNextToken', and the expected token and value, if
        //:   applicable, after that invocation.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'iss', with the input text.
        //:
        //:   2 Create a 'baejsn_Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'iss' with 'mX'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' the number of times
        //:     specified in that row.
        //:
        //:   4 Invoke 'advanceToNextToken' one more time and record the
        //:     return value, the token type, and the value of that token.
        //:
        //:   5 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.  Also confirm that the token type is as expected.
        //:     Finally, if that token is expected to have a value, then the
        //:     value of that token is as expected.
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                      << "TESTING 'advanceToNextToken' TO BAEJSN_NAME" << endl
                      << "===========================================" << endl;

        const struct {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {

            // '{' -> name
            {
                L_,
                "{"
                 "\"name\"",
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                "{\n"
                 "\"name\"",
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                "{\n"
                 "\"name\"\n",
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                WS "{"
                    "\"name\"",
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                WS "{"
                WS " \"name\"",
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                WS "{"
                WS " \"name\"" WS,
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                WS "{\"element name\"",
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "element name"
            },
            {
                L_,
                WS "{\"element " WS " name\"",
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "element " WS " name"
            },
            {
                L_,
                WS "{"
                WS " \"element " WS " name\"",
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "element " WS " name"
            },
            {
                L_,
                WS "{"
                WS " \"element " WS " name\"" WS,
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "element " WS " name"
            },

            // value (number) -> '"'
            {
                L_,
                "{"
                  "\"bid_price\""
                                ":"
                                  "1.500012,"
                  "\"name\"",
                3,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                WS "{"
                WS   "\"bid_price\""
                WS                 ":"
                WS                   "1.500012,"
                WS   "\"name\"",
                3,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "\"John\","
                  "\"bid_price\"",
                3,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "bid_price"
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "\"John\","
                WS   "\"bid_price\"",
                3,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "bid_price"
            },

            // '}' -> '"'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "\"bid_price\":1.500012"
                              "},"
                  "\"date\"",
                6,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"bid_price\":1.500012"
                WS               "},"
                WS   "\"date\"",
                6,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "\"name\":\"CompanyName\","
                                "\"bid_price\":1.500012"
                              "},"
                  "\"date\"",
                8,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"name\":\"CompanyName\","
                WS                 "\"bid_price\":1.500012"
                WS               "},"
                WS   "\"date\"",
                8,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "date"
            },

            // ']' -> '"'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012"
                              "],"
                  "\"date\"",
                5,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012"
                WS               "],"
                WS   "\"date\"",
                5,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012,"
                                "-2.12345"
                              "],"
                  "\"date\"",
                6,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012,"
                WS                 "-2.12345"
                WS               "],"
                WS   "\"date\"",
                6,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\""
                              "],"
                  "\"date\"",
                5,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""
                WS               "],"
                WS   "\"date\"",
                5,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "\"Smith\""
                              "],"
                  "\"date\"",
                6,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\","
                WS                 "\"Smith\""
                WS               "],"
                WS   "\"date\"",
                6,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "date"
            },

            // Error
            {
                L_,
                "{"
                  "\"bid_price\""
                                " "
                                  "\"value\"",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"bid_price\""
                WS                 " "
                WS                   "\"value\"",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"bid_price\""
                                ":"
                                  "{"
                                  "}"
                                    "\"value\"",
                4,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"bid_price\""
                WS                 ":"
                WS                   "{"
                WS                   "}"
                WS                     "\"value\"",
                4,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"bid_price\""
                                ":"
                                  "["
                                  "]"
                                    "\"value\"",
                4,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"bid_price\""
                WS                 ":"
                WS                   "["
                WS                   "]"
                WS                     "\"value\"",
                4,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ ti) {
            const int            LINE      = DATA[ti].d_line;
            const string         TEXT      = DATA[ti].d_text_p;
            const int            PRE_MOVES = DATA[ti].d_preMoves;
            const bool           IS_VALID  = DATA[ti].d_validFlag;
            const Obj::TokenType EXP_TOKEN = DATA[ti].d_expTokenType;
            const bool           HAS_VALUE = DATA[ti].d_hasValue;
            const string         EXP_VALUE = DATA[ti].d_value_p;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::BAEJSN_BEGIN == X.tokenType());

            mX.reset(iss.rdbuf());

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(),
                        Obj::BAEJSN_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' TO BAEJSN_START_OBJECT
        //
        // Concerns:
        //: 1 The following transitions are correctly handled:
        //:
        //:   1 BEGIN          -> START_OBJECT                     BEGIN -> '{'
        //:
        //:   2 NAME           -> START_OBJECT                       ':' -> '{'
        //:
        //:   3 START_ARRAY    -> START_OBJECT                       '[' -> '{'
        //:
        //:   4 END_OBJECT     -> START_OBJECT  '[' -> '{' -> '}' -> ',' -> '{'
        //:
        //: 2 The return code is 0 on success and non-zero on failure.
        //
        // Errors:
        //: 1 The following transitions return an error:
        //:
        //:   1 NAME (no ':')  -> START_OBJECT                      '"'  -> '{'
        //:
        //:   2 START_OBJECT   -> START_OBJECT                      '{'  -> '{'
        //:
        //:   3 END_ARRAY      -> START_OBJECT                      ']'  -> '{'
        //:
        //:   4 VALUE          -> START_OBJECT                     VALUE -> '{'
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text, the number of times to invoke
        //:   'advanceToNextToken', the result of an additional invocation of
        //:   'advanceToNextToken', and the expected token and value, if
        //:   applicable, after that invocation.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'iss', with the input text.
        //:
        //:   2 Create a 'baejsn_Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'iss' with 'mX'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' the number of times
        //:     specified in that row.
        //:
        //:   4 Invoke 'advanceToNextToken' one more time and record the
        //:     return value, the token type, and the value of that token.
        //:
        //:   5 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.  Also confirm that the token type is as expected.
        //:     Finally, if that token is expected to have a value, then the
        //:     value of that token is as expected.
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
              << "TESTING 'advanceToNextToken' TO BAEJSN_START_OBJECT" << endl
              << "===================================================" << endl;

        const struct {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {

            // BEGIN -> '{'
            {
                L_,
                "{",
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{",
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{" WS,
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{" WS,
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                 "\"name\"",
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{\n"
                 "\"name\"",
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{\n"
                 "\"name\"\n",
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                    "\"name\"",
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                WS "\"name\"",
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                 "\"name\"" WS,
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\"",
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS " \"name\"" WS,
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },

            // name -> value (object), i.e. name -> '{'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{",
                2,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{" WS,
                2,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "{" WS,
                2,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock" WS "price\""
                WS                        ":"
                WS                          "{" WS,
                2,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },

            // '[' -> '{' (array of objects)
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{",
                3,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{",
                3,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{" WS,
                3,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },

            // '}' -> '{' (array of objects)
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                "},"
                                "{",
                5,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "},"
                WS                 "{",
                5,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                  "\"bid_price\":1.500012"
                                "},"
                                "{",
                7,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"bid_price\":1.500012"
                WS                 "},"
                WS                 "{",
                7,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "},"
                WS                 "{",
                7,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS                   ","
                WS                 "{" WS,
                7,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                  "\"name\":\"CompanyName\","
                                  "\"bid_price\":1.500012"
                                "},"
                                "{",
                9,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"name\""
                WS                            ":"
                WS                              "\"CompanyName\","
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "},"
                WS                 "{",
                9,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"name\""
                WS                            ":"
                WS                              "\"CompanyName\","
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS                   ","
                WS                 "{" WS,
                9,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },

            // Error
            {
                L_,
                "{"
                  "\"price\""
                            "{",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             "{" WS,
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "{",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "{" WS,
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "\"John\""
                                      "{",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS           "\"John\""
                WS                    "{",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                 "\"name\""
                         ":"
                          "\"John\","
                                    "{",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS           "\"John\","
                WS                     "{",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "12345"
                                   "{",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "12345"
                WS                    "{",
                3,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            "}"
                              "{",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             "}"
                WS               "{" WS,
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            "]"
                              "{",
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             "]"
                WS               "{" WS,
                2,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ ti) {
            const int            LINE      = DATA[ti].d_line;
            const string         TEXT      = DATA[ti].d_text_p;
            const int            PRE_MOVES = DATA[ti].d_preMoves;
            const bool           IS_VALID  = DATA[ti].d_validFlag;
            const Obj::TokenType EXP_TOKEN = DATA[ti].d_expTokenType;
            const bool           HAS_VALUE = DATA[ti].d_hasValue;
            const string         EXP_VALUE = DATA[ti].d_value_p;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::BAEJSN_BEGIN == X.tokenType());

            mX.reset(iss.rdbuf());

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(),
                        Obj::BAEJSN_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' FIRST CHARACTER
        //
        // Concerns:
        //: 1 The first character is always '{' or '['
        //:
        //: 2 The return code is 0 on success and non-zero on failure.
        //
        // Errors:
        //: 1 The following transitions return an error:
        //:
        //:   1 WHITESPACE ONLY                                  " \t\n\v\f\r"
        //:
        //:   2 BEGIN -> START_ARRAY                             BEGIN -> '['
        //:
        //:   3 BEGIN -> END_ARRAY                               BEGIN -> ']'
        //:
        //:   4 BEGIN -> END_OBJECT                              BEGIN -> '}'
        //:
        //:   5 BEGIN -> '"'                                     BEGIN -> '"'
        //:
        //:   6 BEGIN -> ','                                     BEGIN -> ','
        //:
        //:   7 BEGIN -> ':'                                     BEGIN -> ':'
        //:
        //:   8 BEGIN -> VALUE                                   BEGIN -> VALUE
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text, the number of times to invoke
        //:   'advanceToNextToken', the result of an additional invocation of
        //:   'advanceToNextToken', and the expected token and value, if
        //:   applicable, after that invocation.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'iss', with the input text.
        //:
        //:   2 Create a 'baejsn_Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'iss' with 'mX'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' the number of times
        //:     specified in that row.
        //:
        //:   4 Invoke 'advanceToNextToken' one more time and record the
        //:     return value, the token type, and the value of that token.
        //:
        //:   5 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.  Also confirm that the token type is as expected.
        //:     Finally, if that token is expected to have a value, then the
        //:     value of that token is as expected.
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                    << "TESTING 'advanceToNextToken' FIRST CHARACTER" << endl
                    << "============================================" << endl;

        const struct {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {
            // Whitespace only
            {
                L_,
                "",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "          ",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "   \t       ",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "\n          ",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "      \n    ",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "\n\t          ",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "    \t  \n    ",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                " \t\n\v\f\r",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS,
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },

            // Error - First character
            {
                L_,
                "]",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "]",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },

            {
                L_,
                "}",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "}",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },

            {
                L_,
                "\"",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "\"",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                ",",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS ",",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                ":",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS ":",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "1",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "1",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "*",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "*",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "A",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "A",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ ti) {
            const int            LINE      = DATA[ti].d_line;
            const string         TEXT      = DATA[ti].d_text_p;
            const int            PRE_MOVES = DATA[ti].d_preMoves;
            const bool           IS_VALID  = DATA[ti].d_validFlag;
            const Obj::TokenType EXP_TOKEN = DATA[ti].d_expTokenType;
            const bool           HAS_VALUE = DATA[ti].d_hasValue;
            const string         EXP_VALUE = DATA[ti].d_value_p;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::BAEJSN_BEGIN == X.tokenType());

            mX.reset(iss.rdbuf());

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, 0 == mX.advanceToNextToken());
                ASSERTV(X.tokenType(), Obj::BAEJSN_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
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

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        Obj mX;  const Obj& X = mX;
        ASSERTV(X.tokenType(), Obj::BAEJSN_BEGIN == X.tokenType());
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global/default allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
