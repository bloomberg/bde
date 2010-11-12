// baexml_errorinfo.t.cpp                  -*-C++-*-

#include <baexml_errorinfo.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsl_iostream.h>
#include <bsl_cstdlib.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// baexml_ErrorInfo is an in-core value-semantic attribute type.  As such, the
// test driver follows a pre-set formula consisting of a breathing test, then
// tests of the default constructor, basic manipulators and basic accessors,
// then equality operator, then copy construction and assignment, then
// printing, and finally a real-world usage example.  To test a good value
// set, a generator function is used to generate a set of input and output
// values for each of the tests.
//-----------------------------------------------------------------------------
// [2] baexml_ErrorInfo(bslma_Allocator *basicAllocator = 0);
// [4] baexml_ErrorInfo(const baexml_ErrorInfo&  other,
//                      bslma_Allocator         *basicAllocator = 0);
// [2] ~baexml_ErrorInfo();
// [4] baexml_ErrorInfo& operator=(const baexml_ErrorInfo& rhs);
// [2] void setError(Severity               severity,
//                   int                    lineNumber,
//                   int                    columnNumber,
//                   const bdeut_StringRef& source,
//                   const bdeut_StringRef& errorMsg);
// [2] void setError(const baexml_ErrorInfo& other);
// [2] void reset();
// [2] bool isNoError() const;
// [2] bool isWarning() const;
// [2] bool isError() const;
// [2] bool isFatalError() const;
// [2] bool isAnyError() const;
// [2] Severity severity() const;
// [2] int lineNumber() const;
// [2] int columnNumber() const;
// [2] const bsl::string& source() const;
// [2] const bsl::string& message() const;
// [3] bool operator==(const baexml_ErrorInfo& lhs,
//                     const baexml_ErrorInfo& rhs);
// [3] bool operator!=(const baexml_ErrorInfo& lhs,
//                     const baexml_ErrorInfo& rhs);
// [5] bsl::ostream& operator<<(bsl::ostream&           stream,
//                              const baexml_ErrorInfo& errInfo);
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [6] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
                          << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\n";           \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J    \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\t" << #M << ": " << M << "\n";         \
               aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J     \
                         << "\t" << #K << ": " << K << "\t" << #L << ": "  \
                         << L << "\t" << #M << ": " << M << "\t" << #N     \
                         << ": " << N << "\n"; aSsErT(1, #X, __LINE__); } }

// Allow compilation of individual test-cases (for test drivers that take a
// very long time to compile).  Specify '-DSINGLE_TEST=<testcase>' to compile
// only the '<testcase>' test case.
#define TEST_IS_ENABLED(num) (! defined(SINGLE_TEST) || SINGLE_TEST == (num))

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl; // Print ID and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;  // Quote ID literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << flush; // P(X) w/o '\n'
#define L_ __LINE__                                // current Line number
#define T_ bsl::cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

//=============================================================================
//                  GLOBAL HELPER TYPES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#define NUM_ELEMENTS(array) (sizeof(array) / sizeof(*(array)))

// Abbreviation for baexml_ErrorInfo
typedef baexml_ErrorInfo Obj;

// Abbreviations for severities.
const Obj::Severity NO_ERROR    = Obj::BAEXML_NO_ERROR;
const Obj::Severity WARNING     = Obj::BAEXML_WARNING;
const Obj::Severity ERROR       = Obj::BAEXML_ERROR;
const Obj::Severity FATAL_ERROR = Obj::BAEXML_FATAL_ERROR;

struct TestVector
{
    // Test vector structure

    Obj::Severity   d_severity;
    int             d_lineNumber;
    int             d_columnNumber;
    bdeut_StringRef d_source;
    bdeut_StringRef d_message;
    bool            d_isNoError;
    bool            d_isWarning;
    bool            d_isError;
    bool            d_isFatalError;
    bool            d_isAnyError;
};

bool getTestVector(TestVector *v, int index, bool skipUnsettable = true)
    // Set the specified 'v' test vector to the test vector at the specified
    // 'index' in a simulated array of test vectors and return true on success
    // and false if there are no more test vectors.  If the (optionally)
    // specified 'skipUnsettable' is true (the default), then test vectors
    // that can never be translated into a 'baexml_ErrorInfo' object are
    // skipped.  Specifically, if severity is NO_ERROR, then any values other
    // than the defaults for line number, column number, source, and message
    // can not be set in an object.
{
    static const int NUM_LINE_NUMBERS = 2;
    static const int NUM_COLUMN_NUMBERS = 2;

    static const char *const SOURCES[] = {
        "", "inputDoc.xml", "schemaSource.xsd"
    };
    static const int NUM_SOURCES = NUM_ELEMENTS(SOURCES);

    static const char *const MESSAGES[] = {
        "", "You goofed here", "Someone else goofed here"
    };
    static const int NUM_MESSAGES = NUM_ELEMENTS(MESSAGES);

    static const int NUM_SEVERITIES = FATAL_ERROR + 1;

    static const int TESTVECTORS_PER_SEVERITY = (NUM_LINE_NUMBERS   *
                                                 NUM_COLUMN_NUMBERS *
                                                 NUM_SOURCES        *
                                                 NUM_MESSAGES);

    // The total number of test vectors depends on whether we are skipping
    // those vectors that cannot be used to set a 'baexml_ErrorInfo' object.
    // If we are skipping unsettable test vectors, then we have a full set of
    // test vectors for each severity except BAEXML_NO_ERROR but only one
    // test vector for BAEXML_NO_ERROR.  If we are not skipping any vectors,
    // then we simply have a full set of vectors for each severity.
    int numTestVectors = (skipUnsettable ?
                          TESTVECTORS_PER_SEVERITY * (NUM_SEVERITIES - 1) + 1 :
                          TESTVECTORS_PER_SEVERITY * NUM_SEVERITIES);

    if (index >= numTestVectors) {
        return false;
    }

    v->d_lineNumber = 5 * (index % NUM_LINE_NUMBERS);
    index /= NUM_LINE_NUMBERS;

    v->d_columnNumber = 7 * (index % NUM_COLUMN_NUMBERS);
    index /= NUM_COLUMN_NUMBERS;

    v->d_source = bdeut_StringRef(SOURCES[index % NUM_SOURCES]);
    index /= NUM_SOURCES;

    v->d_message = bdeut_StringRef(MESSAGES[index % NUM_MESSAGES]);
    index /= NUM_MESSAGES;

    // Severity is the high-order field.  This formula is designed so that
    // BAEXML_NO_ERROR is the last (instead of the first) severity
    // generated.  That way, if 'skipUnsettable' is true, then the test
    // vectors that are eliminated will all be for BAEXML_NO_ERROR.
    v->d_severity = (Obj::Severity) ((index + 1) % NUM_SEVERITIES);
    index /= NUM_SEVERITIES;

    v->d_isNoError    = (v->d_severity == NO_ERROR);
    v->d_isWarning    = (v->d_severity == WARNING);
    v->d_isError      = (v->d_severity == ERROR);
    v->d_isFatalError = (v->d_severity == FATAL_ERROR);
    v->d_isAnyError   = (v->d_severity == ERROR ||
                         v->d_severity == FATAL_ERROR);

    return true;
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

// In this example, we create a parser for a simple file of percentages.
// The file is formatted as a sequence of lines, with each line containing a
// decimal number in the range "0" to "100", inclusive.  Leading whitespace
// and blank lines are ignored.  When an error occurs during parsing, the
// error data is stored in a 'baexml_ErrorInfo' object.  Our parser's
// interface is as follows:
//..
    #include <baexml_errorinfo.h>
    #include <bsl_sstream.h>
    #include <bsl_cstdlib.h>
    #include <bsl_cstring.h>
    #include <bsl_climits.h>

    class PercentParser {
        // Parse a document stream consisting of a sequence of integral
        // percentages (0 to 100) in decimal text format, one per line.

        bsl::istream *d_input;    // Input document stream
        bsl::string   d_docName;  // Document name
        int           d_line;     // Current line number

      public:
        PercentParser(bsl::istream       *input,
                      const bsl::string&  docName = "INPUT");
            // Construct a parser to parse the data in the specified 'input'
            // stream having the (optional) specified 'docName'.  A valid
            // 'input' stream contains a sequence of integers in the range 0
            // to 100, one per line, in decimal text format.  Each line may
            // contain leading but not trailing tabs and spaces.  Characters
            // after the 20th character on each line are ignored and will
            // generate a warning.

        int parseNext(baexml_ErrorInfo *errorInfo);
            // Read and parse the next percentage in the input stream and
            // return the percentage or -1 on eof or -2 on error.  Set the
            // value of the specified 'errorInfo' structure on error or
            // warning and leave it unchanged otherwise.  Do nothing and
            // return -2 if 'errorInfo->severity()' >= 'BAEXML_ERROR'.
    };
//..
// The constructor is straight-forward:
//..
    PercentParser::PercentParser(bsl::istream       *input,
                                 const bsl::string&  docName)
    : d_input(input), d_docName(docName), d_line(0)
    {
    }
//..
// The 'parseNext' function begins by testing if a previous error occurred.  By
// testing this condition, we can call 'parseNext' several times, knowing that
// the first error will stop the parse operation.
//..
    int PercentParser::parseNext(baexml_ErrorInfo *errorInfo)
    {
        static const int MAX_LINE = 20;

        if (errorInfo->isAnyError()) {
            // Don't advance if errorInfo shows a previous error.
            return -2;
        }
//..
// The parser skips leading whitespace and lines containing only whitespace.
// It loops until a non-empty line is found:
//..
        char buffer[MAX_LINE + 1];
        buffer[0] = '\0';

        // Skip empty lines empty line
        int len = 0;
        int startColumn = 0;
        while (startColumn == len) {
            ++d_line;
            d_input->getline(buffer, MAX_LINE + 1, '\n');
            len = bsl::strlen(buffer);
//..
// The input stream reports that the input line is longer than 'MAX_LINE' by
// setting the fail() condition.  In this case, we set the error object to a
// warning state, indicating the line and column where the problem occurred.
// Then we clear the stream condition and discard the rest of the line.
//..
            if (MAX_LINE == len && d_input->fail()) {
                // 20 characters read without encountering newline.
                // Warn about long line and discard rest of line.
                errorInfo->setError(baexml_ErrorInfo::BAEXML_WARNING,
                                    d_line, len, d_docName,
                                    "Text after 20th column was discarded");
                d_input->clear();
                d_input->ignore(INT_MAX, '\n');
            }
//..
// If we detect an EOF condition, we just return -1.  Otherwise, we skip the
// leading whitespace and go on.
//..
            else if (0 == len && d_input->eof()) {
                // Encountered eof before any other characters.
                return -1;
            }

            // Skip leading whitespace
            startColumn = bsl::strspn(buffer, " \t");
        }

//..
// Now we perform two more error checks: one or superfluous characters after
// the integer, the other for an out-of-range integer.  If the 'errorInfo'
// object is already in warning state, either of these errors will overwrite
// the existing warning with the new error condition.
//..
        char *endp = 0;
        long result = bsl::strtol(buffer + startColumn, &endp, 10);
        int endColumn = endp - buffer;
        if (endColumn < len) {
            // Conversion did not consume rest of buffer.
            errorInfo->setError(baexml_ErrorInfo::BAEXML_ERROR,
                                d_line, endColumn + 1, d_docName,
                                "Bad input character");
            return -2;
        } else if (result < 0 || 100 < result) {
            // Range error.
            errorInfo->setError(baexml_ErrorInfo::BAEXML_ERROR,
                                d_line, startColumn + 1, d_docName,
                                "Value is not between 0 and 100");
            return -2;
        }
//..
// If there were no errors, return the result.  Note that the 'errorInfo'
// object may contain a warning, but warnings typically do not cause a change
// in the error value.
//..
        return result;
    }
//..
// The main program uses the 'PercentParser' class to parse a list of values
// and compute the average.  Typically, the data would be stored in a file,
// but we'll use a literal string for demonstration purposes:
//..
    int usageExample()
    {
        static const char INPUTS[] =
            "    20\n"                  // OK
            "                   30\n"   // Warning ('0' truncated)
            "  \n"                      // Skipped: empty line
            "99x\n"                     // Error: bad character
            "     101\n"                // Error: out of range
            "                 1010\n";  // Out-of-range overrides warning
//..
// We convert the string into a stream and initialize the parser.  We name our
// input stream "Inputs" for the purpose of error handling.  We also
// initialize our working variables:
//..
        bsl::istringstream inputstream(INPUTS);
        PercentParser parser(&inputstream, "Inputs");
        int result;
        int sum = 0;
        int numValues = 0;
//..
// Any error in parsing will be stored in the 'errorInfo' object.  When first
// constructed, it has a severity of 'BAEXML_NO_ERROR'.
//..
        baexml_ErrorInfo errorInfo;
        ASSERT(errorInfo.isNoError());
//..
// Normally, parsing would proceed in a loop.  However, to illustrate the
// different error-handling situations, we have unrolled the loop below.
//
// The first parse succeeds, and no error is reported:
//..
        result = parser.parseNext(&errorInfo);
        ASSERT(20 == result);
        ASSERT(errorInfo.isNoError());
        sum += result;
        ++numValues;
//..
// The next parse also succeeds but, because the input line was very long, a
// warning was generated:
//..
        result = parser.parseNext(&errorInfo);
        ASSERT(3 == result);  // Truncated at 20th column
        ASSERT(errorInfo.isWarning());
        ASSERT(2 == errorInfo.lineNumber());
        ASSERT(20 == errorInfo.columnNumber());
        ASSERT("Text after 20th column was discarded" == errorInfo.message());
        sum += result;
        ++numValues;
//..
// After resetting the 'errorInfo' object, the we call 'nextParse' again.  This
// time it fails with an error.  The line, column, and source of the error are
// reported in the object.
//..
        errorInfo.reset();
        result = parser.parseNext(&errorInfo);
        ASSERT(-2 == result);
        ASSERT("Inputs" == errorInfo.source());
        ASSERT(errorInfo.isError());
        ASSERT(4 == errorInfo.lineNumber());
        ASSERT(3 == errorInfo.columnNumber());
        ASSERT("Bad input character" == errorInfo.message());
//..
// If the 'errorInfo' object is not reset, calling 'parseNext' becomes a
// no-op:
//..
        result = parser.parseNext(&errorInfo);
        ASSERT(-2 == result);
        ASSERT(errorInfo.isError());
        ASSERT(4 == errorInfo.lineNumber());
        ASSERT(3 == errorInfo.columnNumber());
        ASSERT("Bad input character" == errorInfo.message());
//..
// After calling 'reset', the next call to 'parseNext' produces a different
// error message:
//..
        errorInfo.reset();
        result = parser.parseNext(&errorInfo);
        ASSERT(-2 == result);
        ASSERT(errorInfo.isError());
        ASSERT(5 == errorInfo.lineNumber());
        ASSERT(6 == errorInfo.columnNumber());
        ASSERT("Value is not between 0 and 100" == errorInfo.message());
//..
// The last line of the file contains two problems: a long line, which would
// produce a warning, and a range error, which would produce an error.  The
// warning message is overwritten by the error message because the error has a
// higher severity.  Therefore, on return from 'parseNext', only the error
// message is stored in 'errorInfo' and the warning is lost:
//..
        errorInfo.reset();
        result = parser.parseNext(&errorInfo);
        ASSERT(-2 == result);
        ASSERT(errorInfo.isError());
        ASSERT(6 == errorInfo.lineNumber());
        ASSERT(18 == errorInfo.columnNumber());
        ASSERT("Value is not between 0 and 100" == errorInfo.message());
//..
// Writing the 'errorInfo' object to a log or file will produce a readable
// error message:
//..
        bsl::cerr << errorInfo << bsl::endl;
//..
// The resulting message to standard error looks as follows:
//..
//  Inputs:6.18: Error: Value is not between 0 and 100
//..
// Finally, we reach the end of the input stream and can compute our average.
//..
        errorInfo.reset();
        result = parser.parseNext(&errorInfo);
        ASSERT(-1 == result);
        ASSERT(errorInfo.isNoError());

        int average = sum / numValues;
        ASSERT(11 == average);  // (20 + 3) / 2

        return 0;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    bslma_TestAllocator da;
    bslma_DefaultAllocatorGuard dag(&da);
    bslma_TestAllocator ta;

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                               << "\n=============" << bsl::endl;

        // Redirect bsl::cerr to a string stream:
        bsl::ostringstream errorStream;
        bsl::streambuf *cerrStreambuf = bsl::cerr.rdbuf();
        bsl::cerr.rdbuf(errorStream.rdbuf());

        usageExample();

        bsl::cerr.rdbuf(cerrStreambuf); // Cancel redirection

        LOOP_ASSERT(errorStream.str(), errorStream.str() ==
                    "Inputs:6.18: Error: Value is not between 0 and 100\n");

      } break;

      case 5: {
        // --------------------------------------------------------------------
        // PRINTING
        //
        // Concerns:
        //   - Printing an object for NO_ERROR produces no output.
        //   - Printing an object for a warning or error produces an
        //     output of the expected form.
        //
        // Plan:
        //   - Construct and initialize four 'baexml_ErrorInfo' objects, one
        //     at each severity.
        //   - Print each object to an bsl::stringstream
        //   - Verify that the output is as expected.
        //
        // Testing:
        //     bsl::ostream& operator<<(bsl::ostream&           stream,
        //                              const baexml_ErrorInfo& errInfo);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING COPY OPERATIONS"
                               << "\n=======================" << bsl::endl;

        static const int LINE_NUM   = 12;
        static const int COLUMN_NUM = 15;
        static const char SOURCE[]  = "data.xsd";
        static const char MESSAGE[] = "error message";

        const struct {
            Obj::Severity  d_severity;
            const char    *d_output;
        } DATA[] = {
            // Severity    Output
            // ========    ======
            { NO_ERROR,    "" },
            { WARNING,     "data.xsd:12.15: Warning: error message"     },
            { ERROR,       "data.xsd:12.15: Error: error message"       },
            { FATAL_ERROR, "data.xsd:12.15: Fatal Error: error message" }
        };

        static const int DATA_LEN = NUM_ELEMENTS(DATA);

        bsl::ostringstream outputStream;
        for (int i = 0; i < DATA_LEN; ++i) {
            const Obj::Severity        SEVERITY = DATA[i].d_severity;
            const char          *const OUTPUT   = DATA[i].d_output;

            Obj e(&ta); const Obj& E = e;
            e.setError(SEVERITY, LINE_NUM, COLUMN_NUM, SOURCE, MESSAGE);

            outputStream.str("");
            bsl::ostream& result = (outputStream << E);
            ASSERT(&result == &outputStream);
            ASSERT(outputStream.good());
            LOOP2_ASSERT(OUTPUT, outputStream.str(),
                         OUTPUT == outputStream.str());
        }

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // COPY OPERATIONS
        //
        // Concerns:
        //  - Copy-construction produces an object that is equal to but
        //    distinct from the original.
        //  - Assignment produces an object that is equal to but distinct from
        //    the original.
        //  - Copy-construction uses the supplied allocator (or default
        //    allocator if none supplied).
        //  - Assignment does not move allocators.
        //  - The return value of assignment is a reference to the
        //    left-hand-side.
        //
        // Plan:
        //  - In a loop, create a 'baexml_ErrorInfo' object and set it
        //    according to a test vector.
        //  - Copy-construct another 'baexml_ErrorInfo' object from the first,
        //    using the default constructor.  Verify that the copy compares
        //    equal to the original and that the correct allocator was used.
        //  - Copy-construct another 'baexml_ErrorInfo' object from the first,
        //    using a test constructor.  Verify that the copy compares
        //    equal to the original and that the correct allocator was used.
        //  - Construct an empty 'baexml_ErrorInfo' object and give it a value.
        //  - Assign the new object the value of the first object.  Verify
        //    that copy matches the original and that the copy uses the
        //    allocator that with which it was first constructed.
        //
        // Testing:
        //     baexml_ErrorInfo(const baexml_ErrorInfo&  other,
        //                      bslma_Allocator         *basicAllocator = 0);
        //     baexml_ErrorInfo& operator=(const baexml_ErrorInfo& rhs);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING COPY OPERATIONS"
                               << "\n=======================" << bsl::endl;

        bslma_TestAllocator tb;

        TestVector v1;
        for (int i = 0; getTestVector(&v1, i); ++i) {

            const TestVector& V1 = v1;

            Obj e1(&ta); const Obj& E1 = e1;
            e1.setError(V1.d_severity,
                        V1.d_lineNumber,
                        V1.d_columnNumber,
                        V1.d_source,
                        V1.d_message);

            {
                Obj e2(E1); const Obj& E2 = e2;
                LOOP_ASSERT(E2, V1.d_isNoError    == E2.isNoError());
                LOOP_ASSERT(E2, V1.d_isWarning    == E2.isWarning());
                LOOP_ASSERT(E2, V1.d_isError      == E2.isError());
                LOOP_ASSERT(E2, V1.d_isFatalError == E2.isFatalError());
                LOOP_ASSERT(E2, V1.d_isAnyError   == E2.isAnyError());
                LOOP_ASSERT(E2, V1.d_severity     == E2.severity());
                LOOP_ASSERT(E2, V1.d_lineNumber   == E2.lineNumber());
                LOOP_ASSERT(E2, V1.d_columnNumber == E2.columnNumber());
                LOOP_ASSERT(E2, V1.d_source       == E2.source());
                LOOP_ASSERT(E2, V1.d_message      == E2.message());
                LOOP_ASSERT(E2, da.numBlocksInUse() == ta.numBlocksInUse());
            }
            ASSERT(0 == da.numBlocksInUse());

            {
                Obj e2(E1, &tb); const Obj& E2 = e2;
                LOOP_ASSERT(E2, V1.d_isNoError    == E2.isNoError());
                LOOP_ASSERT(E2, V1.d_isWarning    == E2.isWarning());
                LOOP_ASSERT(E2, V1.d_isError      == E2.isError());
                LOOP_ASSERT(E2, V1.d_isFatalError == E2.isFatalError());
                LOOP_ASSERT(E2, V1.d_isAnyError   == E2.isAnyError());
                LOOP_ASSERT(E2, V1.d_severity     == E2.severity());
                LOOP_ASSERT(E2, V1.d_lineNumber   == E2.lineNumber());
                LOOP_ASSERT(E2, V1.d_columnNumber == E2.columnNumber());
                LOOP_ASSERT(E2, V1.d_source       == E2.source());
                LOOP_ASSERT(E2, V1.d_message      == E2.message());
                LOOP_ASSERT(E2, tb.numBlocksInUse() == ta.numBlocksInUse());
            }
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(0 == tb.numBlocksInUse());

            const int NUM_ALLOCS = (0 == V1.d_source.length()) +
                                   (0 == V1.d_message.length());

            {
                Obj e2(&tb); const Obj& E2 = e2;
                e2.setError(WARNING, 99, 100, "junk.xsd", "junk message");
                e2 = E1;
                LOOP_ASSERT(E2, V1.d_isNoError    == E2.isNoError());
                LOOP_ASSERT(E2, V1.d_isWarning    == E2.isWarning());
                LOOP_ASSERT(E2, V1.d_isError      == E2.isError());
                LOOP_ASSERT(E2, V1.d_isFatalError == E2.isFatalError());
                LOOP_ASSERT(E2, V1.d_isAnyError   == E2.isAnyError());
                LOOP_ASSERT(E2, V1.d_severity     == E2.severity());
                LOOP_ASSERT(E2, V1.d_lineNumber   == E2.lineNumber());
                LOOP_ASSERT(E2, V1.d_columnNumber == E2.columnNumber());
                LOOP_ASSERT(E2, V1.d_source       == E2.source());
                LOOP_ASSERT(E2, V1.d_message      == E2.message());
                LOOP_ASSERT(E2, tb.numBlocksInUse() ==
                                             NUM_ALLOCS + ta.numBlocksInUse());
            }
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(0 == tb.numBlocksInUse());

        } // End for i

      } break;

      case 3: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        //
        // Concerns:
        //  - Objects that are equal compare equal.
        //  - Objects that are not equal don't compare equal .
        //  - 'operator!=' has the opposite result of 'operator=='.
        //
        // Plan:
        //  - In a 2-level nested loop, generate two test vectors.
        //  - Create two 'baexml_ErrorInfo' objects and set their values
        //    according to each test vector.
        //  - If the test vectors were generated from the same index,
        //    verify that calling 'operator==' on the two objects returns true
        //    and calling 'operator!=' on the two objects returns false.
        //  - If the test vectors were generated from the different indexes,
        //    verify that calling 'operator==' on the two objects returns false
        //    and calling 'operator!=' on the two objects returns true.
        //
        // Testing:
        //     bool operator==(const baexml_ErrorInfo& lhs,
        //                     const baexml_ErrorInfo& rhs);
        //     bool operator!=(const baexml_ErrorInfo& lhs,
        //                     const baexml_ErrorInfo& rhs);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING EQUALITY OPERATORS"
                               << "\n==========================" << bsl::endl;

        TestVector v1;
        for (int i = 0; getTestVector(&v1, i); ++i) {

            const TestVector& V1 = v1;

            Obj e1(&ta); const Obj& E1 = e1;
            e1.setError(V1.d_severity,
                        V1.d_lineNumber,
                        V1.d_columnNumber,
                        V1.d_source,
                        V1.d_message);

            TestVector v2;
            for (int j = 0; getTestVector(&v2, j); ++j) {

                const TestVector& V2 = v2;

                Obj e2(&ta); const Obj& E2 = e2;
                e2.setError(V2.d_severity,
                            V2.d_lineNumber,
                            V2.d_columnNumber,
                            V2.d_source,
                            V2.d_message);

                if (i == j) {
                    LOOP2_ASSERT(E1, E2,    E1 == E2 );
                    LOOP2_ASSERT(E1, E2, ! (E1 != E2));
                }
                else {
                    LOOP2_ASSERT(E1, E2, ! (E1 == E2));
                    LOOP2_ASSERT(E1, E2,    E1 != E2 );
                }
            } // End for j
        } // End for i

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // BASIC MANIPULATORS
        //
        // Concerns:
        //  - The default constructor produces a NO_ERROR object.
        //  - 'setError' on a NO_ERROR object overwrites the current settings
        //  - 'setError' on an object with lower severity overwrites the
        //    current settings.
        //  - 'setError' on an object with same or higher priority does
        //    nothing.
        //  - reset sets the object back to its initial state.
        //  - The allocator passed to the constructor is used for all strings.
        //
        // Plan:
        //  - Construct a 'baexml_ErrorInfo' using the default allocator.
        //    Verify default attributes of the newly-constructed object.
        //  - Set attributes on the new object.  Verify the attributes and the
        //    use of the allocator.
        //  - Repeat the previous 2 steps using a test allocator.
        //  - Loop through two sets of data values
        //  - Within the loop, construct three 'baexml_ErrorInfo' objects using
        //    a test allocator.  Call them 'e1', 'e2', and 'e3'.
        //  - Use the multiple-argument 'setError' to set 'e1' to one data
        //    value.  Verify the attributes of 'e1'
        //  - Use the multiple-argument 'setError' to set 'e2' and 'e3' to the
        //    other data value.
        //  - Use the multiple-argument 'setError' to set 'e2' to the first
        //    data value.
        //  - If the first data value has a higher severity than the second
        //    data value, verify that 'e2' now has the first data value, else
        //    verify that it still has the second data value.
        //  - Use the single-argument 'setError' to set 'e3' to 'e1'.
        //  - If the first data value has a higher severity than the second
        //    data value, verify that 'e3' now has the first data value, else
        //    verify that it still has the second data value.
        //  - Verify that the test allocator was used and the default
        //    allocator was not.
        //  - Call 'reset' on 'e3'.  Verify that it is back to the default
        //    state.
        //
        // Testing
        //     baexml_ErrorInfo(bslma_Allocator *basicAllocator = 0);
        //     ~baexml_ErrorInfo();
        //     void setError(Severity               severity,
        //                   int                    lineNumber,
        //                   int                    columnNumber,
        //                   const bdeut_StringRef& source,
        //                   const bdeut_StringRef& errorMsg);
        //     void setError(const baexml_ErrorInfo& other);
        //     void reset();
        //     bool isNoError() const;
        //     bool isWarning() const;
        //     bool isError() const;
        //     bool isFatalError() const;
        //     bool isAnyError() const;
        //     Severity severity() const;
        //     int lineNumber() const;
        //     int columnNumber() const;
        //     const bsl::string& source() const;
        //     const bsl::string& message() const;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING BASIC MANIPULATORS"
                               << "\n==========================" << bsl::endl;

        if (verbose) bsl::cout << "Testing default constructor." << bsl::endl;
        {
            // Default allocator
            Obj e0; const Obj& E0 = e0;
            LOOP_ASSERT(E0,   E0.isNoError());
            LOOP_ASSERT(E0, ! E0.isWarning());
            LOOP_ASSERT(E0, ! E0.isError());
            LOOP_ASSERT(E0, ! E0.isFatalError());
            LOOP_ASSERT(E0, ! E0.isAnyError());
            LOOP_ASSERT(E0, ! E0.isWarning());
            LOOP_ASSERT(E0, NO_ERROR    == E0.severity());
            LOOP_ASSERT(E0, 0           == E0.lineNumber());
            LOOP_ASSERT(E0, 0           == E0.columnNumber());
            LOOP_ASSERT(E0, ""          == E0.source());
            LOOP_ASSERT(E0, ""          == E0.message());

            e0.setError(ERROR, 1, 2, "MySource.xml", "The error");
            LOOP_ASSERT(E0, ! E0.isNoError());
            LOOP_ASSERT(E0, ! E0.isWarning());
            LOOP_ASSERT(E0,   E0.isError());
            LOOP_ASSERT(E0, ! E0.isFatalError());
            LOOP_ASSERT(E0,   E0.isAnyError());
            LOOP_ASSERT(E0, ! E0.isWarning());
            LOOP_ASSERT(E0, ERROR          == E0.severity());
            LOOP_ASSERT(E0, 1              == E0.lineNumber());
            LOOP_ASSERT(E0, 2              == E0.columnNumber());
            LOOP_ASSERT(E0, "MySource.xml" == E0.source());
            LOOP_ASSERT(E0, "The error"    == E0.message());
            LOOP_ASSERT(E0, 0 <  da.numBlocksInUse());
        }
        ASSERT(0 == da.numBlocksInUse());
        ASSERT(0 == ta.numBlocksInUse());

        {
            // Explicit allocator
            Obj e0(&ta); const Obj& E0 = e0;
            LOOP_ASSERT(E0,   E0.isNoError());
            LOOP_ASSERT(E0, ! E0.isWarning());
            LOOP_ASSERT(E0, ! E0.isError());
            LOOP_ASSERT(E0, ! E0.isFatalError());
            LOOP_ASSERT(E0, ! E0.isAnyError());
            LOOP_ASSERT(E0, ! E0.isWarning());
            LOOP_ASSERT(E0, NO_ERROR    == E0.severity());
            LOOP_ASSERT(E0, 0           == E0.lineNumber());
            LOOP_ASSERT(E0, 0           == E0.columnNumber());
            LOOP_ASSERT(E0, ""          == E0.source());
            LOOP_ASSERT(E0, ""          == E0.message());
            LOOP_ASSERT(E0, 0 == da.numBlocksInUse());

            e0.setError(ERROR, 1, 2, "MySource.xml", "The error");
            LOOP_ASSERT(E0, ! E0.isNoError());
            LOOP_ASSERT(E0, ! E0.isWarning());
            LOOP_ASSERT(E0,   E0.isError());
            LOOP_ASSERT(E0, ! E0.isFatalError());
            LOOP_ASSERT(E0,   E0.isAnyError());
            LOOP_ASSERT(E0, ! E0.isWarning());
            LOOP_ASSERT(E0, ERROR          == E0.severity());
            LOOP_ASSERT(E0, 1              == E0.lineNumber());
            LOOP_ASSERT(E0, 2              == E0.columnNumber());
            LOOP_ASSERT(E0, "MySource.xml" == E0.source());
            LOOP_ASSERT(E0, "The error"    == E0.message());
            LOOP_ASSERT(E0, 0 == da.numBlocksInUse());
            LOOP_ASSERT(E0, 0 <  ta.numBlocksInUse());
        }
        ASSERT(0 == da.numBlocksInUse());
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) bsl::cout << "Testing manipulators and accessors"
                               << bsl::endl;
        TestVector v1;
        for (int i = 0; getTestVector(&v1, i, false); ++i) {

            const TestVector& V1 = v1;

            Obj e1(&ta); const Obj& E1 = e1;
            e1.setError(V1.d_severity,
                        V1.d_lineNumber,
                        V1.d_columnNumber,
                        V1.d_source,
                        V1.d_message);
            const int EMPTY1 = V1.d_severity <= NO_ERROR ||
                               (0 == V1.d_source.length() &&
                                0 == V1.d_message.length());
            LOOP_ASSERT(E1, V1.d_isNoError    == E1.isNoError());
            LOOP_ASSERT(E1, V1.d_isWarning    == E1.isWarning());
            LOOP_ASSERT(E1, V1.d_isError      == E1.isError());
            LOOP_ASSERT(E1, V1.d_isFatalError == E1.isFatalError());
            LOOP_ASSERT(E1, V1.d_isAnyError   == E1.isAnyError());
            LOOP_ASSERT(E1, V1.d_severity     == E1.severity());
            if (V1.d_severity > NO_ERROR) {
                LOOP_ASSERT(E1, V1.d_lineNumber   == E1.lineNumber());
                LOOP_ASSERT(E1, V1.d_columnNumber == E1.columnNumber());
                LOOP_ASSERT(E1, V1.d_source       == E1.source());
                LOOP_ASSERT(E1, V1.d_message      == E1.message());
                LOOP_ASSERT(E1, EMPTY1 || 0 < ta.numBlocksInUse());
            }
            else {
                LOOP_ASSERT(E1, 0                 == E1.lineNumber());
                LOOP_ASSERT(E1, 0                 == E1.columnNumber());
                LOOP_ASSERT(E1, ""                == E1.source());
                LOOP_ASSERT(E1, ""                == E1.message());
            }
            LOOP_ASSERT(E1, 0 == da.numBlocksInUse());

            TestVector v2;
            for (int j = 0; getTestVector(&v2, j); ++j) {

                const TestVector& V2 = v2;

                Obj e2(&ta); const Obj& E2 = e2;
                e2.setError(V2.d_severity,
                            V2.d_lineNumber,
                            V2.d_columnNumber,
                            V2.d_source,
                            V2.d_message);
                const int EMPTY2 = (0 == V2.d_source.length() &&
                                    0 == V2.d_message.length());
                LOOP_ASSERT(E2, V2.d_isNoError    == E2.isNoError());
                LOOP_ASSERT(E2, V2.d_isWarning    == E2.isWarning());
                LOOP_ASSERT(E2, V2.d_isError      == E2.isError());
                LOOP_ASSERT(E2, V2.d_isFatalError == E2.isFatalError());
                LOOP_ASSERT(E2, V2.d_isAnyError   == E2.isAnyError());
                LOOP_ASSERT(E2, V2.d_severity     == E2.severity());
                LOOP_ASSERT(E2, V2.d_lineNumber   == E2.lineNumber());
                LOOP_ASSERT(E2, V2.d_columnNumber == E2.columnNumber());
                LOOP_ASSERT(E2, V2.d_source       == E2.source());
                LOOP_ASSERT(E2, V2.d_message      == E2.message());
                LOOP_ASSERT(E2, 0 == da.numBlocksInUse());
                LOOP_ASSERT(E2, EMPTY1 && EMPTY2 ||
                                0 <  ta.numBlocksInUse());

                // Set 'e2' to 'V1' if 'V1' severity is greater
                e2.setError(V1.d_severity,
                            V1.d_lineNumber,
                            V1.d_columnNumber,
                            V1.d_source,
                            V1.d_message);
                if (V1.d_severity > V2.d_severity) {
                    LOOP_ASSERT(E2, V1.d_isNoError    == E2.isNoError());
                    LOOP_ASSERT(E2, V1.d_isWarning    == E2.isWarning());
                    LOOP_ASSERT(E2, V1.d_isError      == E2.isError());
                    LOOP_ASSERT(E2, V1.d_isFatalError == E2.isFatalError());
                    LOOP_ASSERT(E2, V1.d_isAnyError   == E2.isAnyError());
                    LOOP_ASSERT(E2, V1.d_severity     == E2.severity());
                    LOOP_ASSERT(E2, V1.d_lineNumber   == E2.lineNumber());
                    LOOP_ASSERT(E2, V1.d_columnNumber == E2.columnNumber());
                    LOOP_ASSERT(E2, V1.d_source       == E2.source());
                    LOOP_ASSERT(E2, V1.d_message      == E2.message());
                }
                else {
                    LOOP_ASSERT(E2, V2.d_isNoError    == E2.isNoError());
                    LOOP_ASSERT(E2, V2.d_isWarning    == E2.isWarning());
                    LOOP_ASSERT(E2, V2.d_isError      == E2.isError());
                    LOOP_ASSERT(E2, V2.d_isFatalError == E2.isFatalError());
                    LOOP_ASSERT(E2, V2.d_isAnyError   == E2.isAnyError());
                    LOOP_ASSERT(E2, V2.d_severity     == E2.severity());
                    LOOP_ASSERT(E2, V2.d_lineNumber   == E2.lineNumber());
                    LOOP_ASSERT(E2, V2.d_columnNumber == E2.columnNumber());
                    LOOP_ASSERT(E2, V2.d_source       == E2.source());
                    LOOP_ASSERT(E2, V2.d_message      == E2.message());
                }
                LOOP_ASSERT(E2, 0 == da.numBlocksInUse());
                LOOP_ASSERT(E2, EMPTY1 ||
                                0 <  ta.numBlocksInUse());

                Obj e3(&ta); const Obj& E3 = e3;
                e3.setError(V2.d_severity,
                            V2.d_lineNumber,
                            V2.d_columnNumber,
                            V2.d_source,
                            V2.d_message);

                // Set 'e3' to 'e1' if 'e1' severity is greater
                e3.setError(E1);
                if (V1.d_severity > V2.d_severity) {
                    LOOP_ASSERT(E3, V1.d_isNoError    == E3.isNoError());
                    LOOP_ASSERT(E3, V1.d_isWarning    == E3.isWarning());
                    LOOP_ASSERT(E3, V1.d_isError      == E3.isError());
                    LOOP_ASSERT(E3, V1.d_isFatalError == E3.isFatalError());
                    LOOP_ASSERT(E3, V1.d_isAnyError   == E3.isAnyError());
                    LOOP_ASSERT(E3, V1.d_severity     == E3.severity());
                    LOOP_ASSERT(E3, V1.d_lineNumber   == E3.lineNumber());
                    LOOP_ASSERT(E3, V1.d_columnNumber == E3.columnNumber());
                    LOOP_ASSERT(E3, V1.d_source       == E3.source());
                    LOOP_ASSERT(E3, V1.d_message      == E3.message());
                }
                else {
                    LOOP_ASSERT(E3, V2.d_isNoError    == E3.isNoError());
                    LOOP_ASSERT(E3, V2.d_isWarning    == E3.isWarning());
                    LOOP_ASSERT(E3, V2.d_isError      == E3.isError());
                    LOOP_ASSERT(E3, V2.d_isFatalError == E3.isFatalError());
                    LOOP_ASSERT(E3, V2.d_isAnyError   == E3.isAnyError());
                    LOOP_ASSERT(E3, V2.d_severity     == E3.severity());
                    LOOP_ASSERT(E3, V2.d_lineNumber   == E3.lineNumber());
                    LOOP_ASSERT(E3, V2.d_columnNumber == E3.columnNumber());
                    LOOP_ASSERT(E3, V2.d_source       == E3.source());
                    LOOP_ASSERT(E3, V2.d_message      == E3.message());
                }
                LOOP_ASSERT(E3, 0 == da.numBlocksInUse());
                LOOP_ASSERT(E3, EMPTY1 && EMPTY2 ||
                                0 <  ta.numBlocksInUse());

                e3.reset();
                LOOP_ASSERT(E3,   E3.isNoError());
                LOOP_ASSERT(E3, ! E3.isWarning());
                LOOP_ASSERT(E3, ! E3.isError());
                LOOP_ASSERT(E3, ! E3.isFatalError());
                LOOP_ASSERT(E3, ! E3.isAnyError());
                LOOP_ASSERT(E3, ! E3.isWarning());
                LOOP_ASSERT(E3, NO_ERROR    == E3.severity());
                LOOP_ASSERT(E3, 0           == E3.lineNumber());
                LOOP_ASSERT(E3, 0           == E3.columnNumber());
                LOOP_ASSERT(E3, ""          == E3.source());
                LOOP_ASSERT(E3, ""          == E3.message());
            }
        }
        ASSERT(0 == da.numBlocksInUse());
        ASSERT(0 == ta.numBlocksInUse());

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns: That the basic functionality of baexml_ErrorInfo works.
        //
        // Plan: Construct a variety of baexml_ErrorInfo objects and verify
        //   their values using the basic accessors.  Change the value of the
        //   object and verify that more severe errors overwrite less-severe
        //   ones.  Make copies using assignment and copy construction and
        //   verify that the copies are as expected and that the equality
        //   operators work as expected.
        //
        // Testing:
        //     Breathing test
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBREATHING TEST"
                               << "\n==============" << bsl::endl;

        Obj errInfo;

        //Fatal Error
        errInfo.setError(baexml_ErrorInfo::BAEXML_FATAL_ERROR,
                         111,
                         222,
                         "FatalSource",
                         "FatalError");
        ASSERT(errInfo.isFatalError());
        ASSERT(errInfo.lineNumber() == 111);
        ASSERT(errInfo.columnNumber() == 222);
        ASSERT(errInfo.source() == "FatalSource");
        ASSERT(errInfo.message() == "FatalError");

        //Error
        errInfo.setError(baexml_ErrorInfo::BAEXML_ERROR,
                         333,
                         444,
                         "Source",
                         "Error");

        // should be reject as we have more serious error
        ASSERT(errInfo.isFatalError());
        ASSERT(errInfo.lineNumber() == 111);
        ASSERT(errInfo.columnNumber() == 222);
        ASSERT(errInfo.source() == "FatalSource");
        ASSERT(errInfo.message() == "FatalError");

        // reset and set again
        errInfo.reset();
        errInfo.setError(baexml_ErrorInfo::BAEXML_ERROR,
                         333,
                         444,
                         "Source",
                         "Error");
        ASSERT(errInfo.isError());
        ASSERT(errInfo.lineNumber() == 333);
        ASSERT(errInfo.columnNumber() == 444);
        ASSERT(errInfo.source() == "Source");
        ASSERT(errInfo.message() == "Error");

        // Warning
        errInfo.setError(baexml_ErrorInfo::BAEXML_WARNING,
                         555,
                         666,
                         "WarningSource",
                         "Warning");
        // should be reject as we have more serious error
        ASSERT(errInfo.isError());
        ASSERT(errInfo.lineNumber() == 333);
        ASSERT(errInfo.columnNumber() == 444);
        ASSERT(errInfo.source() == "Source");
        ASSERT(errInfo.message() == "Error");

        // reset and set again
        errInfo.reset();
        errInfo.setError(baexml_ErrorInfo::BAEXML_WARNING,
                         555,
                         666,
                         "WarningSource",
                         "Warning");
        ASSERT(errInfo.isWarning());
        ASSERT(errInfo.lineNumber() == 555);
        ASSERT(errInfo.columnNumber() == 666);
        ASSERT(errInfo.source() == "WarningSource");
        ASSERT(errInfo.message() == "Warning");

        // Copy construct
        const Obj copy1(errInfo);
        ASSERT(copy1 == errInfo);

        // Assignment
        Obj copy2;
        ASSERT(copy2 != errInfo);
        copy2 = errInfo;
        ASSERT(copy2 == errInfo);
        ASSERT(copy1 == copy2);

        // reset
        errInfo.reset();
        ASSERT(errInfo.severity() == baexml_ErrorInfo::BAEXML_NO_ERROR);
        ASSERT(errInfo.lineNumber() == 0);
        ASSERT(errInfo.columnNumber() == 0);
        ASSERT(errInfo.source().empty());
        ASSERT(errInfo.message().empty());
        ASSERT(copy1 != errInfo);
        ASSERT(copy2 != errInfo);

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
