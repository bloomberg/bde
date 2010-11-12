// bdempu_schemaaggregate.t.cpp              -*-C++-*-

#include <bdempu_schemaaggregate.h>

#include <bdempu_elemtype.h>
#include <bdempu_schema.h>

#include <bdepu_parserimputil.h>
#include <bdepu_typesparser.h>

#include <bdetu_null.h>
#include <bdem_elemtype.h>
#include <bdem_aggregate.h>
#include <bdem_schema.h>

#include <cstdlib>     // atoi()
#include <iostream>
#include <string>
#include <strstream>
#include <vector>

using namespace BloombergLP;
using namespace std;

enum { SUCCESS = 0, FAILURE = 1 };

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// We use the following abbreviations:
//  'b_ca'       for 'std::vector<char>'
//  'Type'       for 'bdem_ElemType::Type'
//  'List'       for 'bdem_List'
//  'Table'      for 'bdem_Table'
//  'RecDef'     for 'bdem_RecordDef'
//
//  prefix 'c' specifies that the type is a constant.  For example,
//  'cchar' stands for 'const char'
//
//-----------------------------------------------------------------------------
// [2] parseConstrainedList(cchar **, List *, cchar *, cRecDef& );
// [3] parseConstrainedTable(cchar **, Table *, cchar *, cRecDef& );
// [4] parseScope(cchar **, bdem_Schema *, int *, List *, cchar *);
// [2] generateConstrainedList(b_ca *, cList&, cRecDef&, int, int);
// [3] generateConstrainedTable(b_ca *, cTable&, cRecDef&, int, int);
// [4] void generateScope(b_ca *, cbdem_Schema&, cRecDef&, int, int);

//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

    typedef bdepu_ParserImpUtil ImpUtil;

    const char *BLANKS[] = {
        // This structure is used in "orthogonal perturbation" to test each
        // input string with every type of the <WHITESPACE>.
        "",
        " // C++ blank \n",
        " /*C blanks*/",
        "  \n\n\t/* //*/",
        "  ///*  \t\n ",
        " ",
        "  ",
        " \t",
        " \n",
        " \t\n",
        " \n\t"
    };
    const int NUM_BLANKS = sizeof BLANKS / sizeof *BLANKS;



    template <class T>
    struct DataParsing {
        // Facilitate testing of parsing functions

        int         d_lineNum;    // source line number
        const char *d_str;        // input string
        int         d_blanks;     // number of inserted 'BLANKS' before
                                  // 'd_endPos' is encountered (*)
        int         d_constraint; // constraint
        T           d_result;     // expected resulting value
        int         d_retVal;     // expected return value
        int         d_endPos;     // expected offset from the start position
                                  // when blanks are not inserted
    };

    // (*) 'd_blanks' is used to calculate the expected 'd_endPos' (where
    // 'd_endPos' is a parameter in the parse function).  It is computed as a
    // sum of 'd_endPos' (from the 'DataParsing' 'struct') and a product of
    // 'd_blanks' and the length of the currently used element of 'BLANKS'.
    // In some cases it is more convenient to define the expected ending
    // position from the end of the input string.  The following two constants
    // are used to identify the end position as a length of the input string
    // and length of the input string minus one.

    const int LEN = 1000;     // Ending offset is length of the input
    const int LENSUB1 = 999;  // Ending offset is length of the input minus
                              // one.


    template <class T>
    struct DataGenerating {
        // Facilitate testing generating functions

        int         d_lineNum; // source line number
        const char *d_prefix;  // characters in output buffer before  a string
                               // generated from 'd_val' is placed in it
        int         d_level;   // identation level
        int         d_spaces;  // spaces per level
        T           d_val;     // input value used to generate a string
                               // representation
        int         d_constr;  // input constraint index used
                               // to generate a string representation
        const char *d_str;     // output String (combination of 'd_prefix' and
                               // a string generated from 'd_val'
    };

//=============================================================================
//              GENERATOR FUNCTION 'g'
//-----------------------------------------------------------------------------

    static void appendToBuffer(std::vector<char> *buffer, const char *str)
    {
        while (*str) {
            buffer->push_back(*str);
            ++str;
        }
    }

    void g(std::vector<char> *buffer, const char *inStr, int index)
        // Append the content of the specified 'inStr' to the specified
        // 'buffer' replacing every occurrence of the character '@' with the
        // array element of 'BLANKS' at the specified 'index'.
    {
        // 'BLANKS' is a global array of strings representing a sample set of
        // possible <WHITESPACE> strings.

        buffer->clear();
        while (*inStr != '\0') {
            if (*inStr == '@') {
                appendToBuffer(buffer, BLANKS[index]);
            }
            else {
                buffer->push_back(*inStr);
            }
            ++inStr;
        }
    }


//=============================================================================
//                        USAGE EXAMPLE IMPLEMENTATION
//-----------------------------------------------------------------------------

// This section provides functionality as either illustrated in the Usage
// Example or else as needed to implement working code in a Test Driver.

                      // -------------------
                      // Stream & File Utils
                      // -------------------

struct my_IoUtil {
    // This class provides a namespace for a set of pure procedures on
    // streams.

    // CLASS METHODS
    static
    int readStream(std::vector<char> *buffer, std::istream& stream)
        // Load into the specified 'buffer' the contents of the specified
        // 'stream' and return the number of characters read into 'buffer'.
    {
        int  originalLength = buffer->size();
        int c;
        while ((c = stream.get()) && c != EOF) {
            buffer->push_back(c);
        }
        int numCharsRead = buffer->size() - originalLength;

        buffer->push_back('\0');

        return numCharsRead;
    }

    static
    int writeStream(std::ostream& stream, const std::vector<char>& buffer)
        // Write to the specified 'stream' the contents of the the
        // specified 'buffer' and return the number of bytes written.
    {
        int length = buffer.size();
        for (int i = 0; i < length; ++i) {
            stream << buffer[i];
        }
        stream.flush();

        return length;
    }

    static
    int writeStream(std::ostream&            stream,
                    const std::vector<char>& buffer,
                    int                      startIndex,
                    int                      numChars)
        // Write to the specified 'stream' the specified 'numChars' from
        // the the specified 'buffer', starting at the specified
        // 'startIndex, and return the number of bytes written.
    {
        ASSERT(0 < numChars);
        ASSERT(startIndex + numChars < (int)buffer.size());

        int stopIndex = startIndex + numChars;
        for (int i = startIndex; i < stopIndex; ++i) {
            stream << buffer[i];
        }
        stream.flush();

        return numChars;
    }
};

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

                      // -------------
                      // Parsing Utils
                      // -------------

struct my_ParseUtil {
    // Namespace for pure procedures that parse and validate text
    // representations of 'bdem' objects.

    // CLASS METHODS
        static int
        validateAndLoadConstrainedList(
                                   bdem_List                *list,
                                   std::ostream&             errorStream,
                                   const std::vector<char>&  inputBuffer,
                                   const bdem_Schema&        schema,
                                   int                       recordIndex);
        // Parse the ascii representation of a constrained list from the
        // specified 'inputBuffer', using the specified 'recordIndex' in
        // the specified 'schema' as the constraint, and load the
        // specified 'list' with the parsed value if there is no parse
        // error.  In the event of a parse error, format an error
        // message containing the line number, text line, and an
        // indication of the character causing the parse error to the
        // specified 'errorStream'.  Return 0 on successful parsing, or
        // the line number of the first syntax error.  Note that the
        // content of 'list' is undefined in the event of a parse error.
};

static
int getLineNumber(int                      *lineStartIndex,
                  int                      *lineStopIndex,
                  const std::vector<char>&  buffer,
                  int                       index)
    // Load into the specified 'lineStartIndex' and 'lineStopIndex',
    // respectively, the indices of the first and last characters in the
    // line containing the character at 'index'.  Return the line number
    // of this line.  The behavior is undefined unless
    // 0 <= index < buffer.length().  Note that the line number starts
    // at 1, and that the newline character is the interior delimiter of
    // lines of text, but that 'buffer' need neither begin nor end with
    // a newline.
{
    ASSERT(0 <= index);
    ASSERT(index < (int)buffer.size());

    int lineNumber = 1;
    int length     = buffer.size();

    *lineStartIndex = 0;
    for (int i = 0; i <= index; ++i) {
        if ('\n' == buffer[i]) {
            ++lineNumber;
            *lineStartIndex = i + 1;
        }
    }

    // find end of line, because seeing entire line is more useful
    int eol = index;
    while (eol < length && buffer[eol] != '\n' && buffer[eol] != '\0') {
        ++eol;
    }
    *lineStopIndex = eol;

    return lineNumber;
}


int my_ParseUtil::validateAndLoadConstrainedList(
                                     bdem_List                *list,
                                     std::ostream&             errorStream,
                                     const std::vector<char>&  inputBuffer,
                                     const bdem_Schema&        schema,
                                     int                       recordIndex)
{
    ASSERT(list);

    const bdem_RecordDef *constraint = &schema.record(recordIndex);
    const char           *inputText  = &inputBuffer.front();
    const char           *endPos;
    int                   errorLineStart;
    int                   errorLineStop;

    // Parse the constrained list
    int result = bdempu_SchemaAggregate::parseConstrainedList(
                                  &endPos, list, inputText, *constraint);

    // Display errors, if any
    if(0 != result) {
        result = getLineNumber(&errorLineStart, &errorLineStop,
                               inputBuffer, endPos - inputText);

        errorStream << "Parse error on line " << result << std::endl;
        my_IoUtil::writeStream(errorStream, inputBuffer,
                               errorLineStart, errorLineStop - errorLineStart);
        errorStream << std::endl;

        int indicatorLength = endPos - inputText - errorLineStart + 1;
        std::vector<char> indicator(indicatorLength, '.');
        indicator[indicatorLength - 1] = '^';
        my_IoUtil::writeStream(errorStream, indicator);
        errorStream << std::endl;

        return result;
    }
    else {
        return 0;
    }
}

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

                      // ----------------
                      // "Well-Known Data
                      // ----------------

static const char *globalConstrainedListString =
"{  // start \"Scope\" \n "
" Schedule = { "
"  [ Name      OpenPeriods ] "
"  { \"US\"     { [ OpenDays  Period ] "
"                { 12345   { Start = 08:00:00.000 Finish = 16:15:00.000 }}} "
"  } "
"  { \"BEAR_TEST\" "
"             { [ OpenDays Period ] "
"               { 12345    { Start = 08:00:00.000 Finish = 16:15:00.000 }} "
"               { 6        { Start = 10:00:00.000 Finish = 14:00:00.000 }} "
"             } "
"  } "
" }  // end Schedule \n "
" Connections = { "
"   [ Name     Schedule       Description                    ClickAction ] "
"   { \"BEAR1\" \"US\"          \"Government Bond Prices\"      \"AECI\" } "
"   { \"BEAR2\" \"BEAR_TEST\"   \"Mortgage Pool Prices\"        \"AECI\" } "
"   { \"GMSC1\" \"Schedule-US\" \"Government Bond Prices\"      \"PWHO\" } "
"   { \"GMSC2\" \"Schedule-US\" \"High-Grade Corp Bond Prices\" \"AECI\" } "
"   { \"GMSC3\" \"Schedule-US\" \"High-Yield Corp Bond Prices\" \"AECI\" } "
" }  "
"} // end Constrained List Data \n";

static const char *globalSchemaString =
"{  // start Schema \n "
"    RECORD TimeRange {  "
"      TIME Start ; "
"      TIME Finish ; "
"    } "
"    RECORD OpenPeriodSubStruct { "
"      INT OpenDays ; "
"      RECORD < TimeRange > Period ; "
"    } "
"    RECORD ScheduleStruct { "
"      STRING Name ; "
"      RECORD_ARRAY < OpenPeriodSubStruct > OpenPeriods ; "
"    } "
"    RECORD ConnectionStruct { "
"      STRING Name; "
"      STRING Schedule; "
"      STRING Description; "
"      STRING ClickAction; "
"    } "
"    RECORD ConfigFile { "
"      RECORD_ARRAY <ScheduleStruct > Schedule ; "
"      RECORD_ARRAY < ConnectionStruct> Connections ; "
"    } "
"} // SCHEMA \n";

int populateSchema(bdem_Schema *schema, std::istream& stream)
{
    std::vector<char> schemaBuffer;
    my_IoUtil::readStream(&schemaBuffer, stream);
    const char *endPos;

    return bdempu_Schema::parseSchema(&endPos, schema, &schemaBuffer.front());
}

static istrstream  globalSchemaInStream(globalSchemaString);
static bdem_Schema globalSchema;               // Well-known schema
static int         globalConstraintIndex = 4;  // Well-known index

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    std::cout << "TEST " << __FILE__ << " CASE " << test << std::endl;;
    switch (test) {
      case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  Suppress
        //   all 'cout' statements in non-verbose modes, and add streaming to
        //   a buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Examples"
                          << "\n======================" << endl;

        {
            bdem_Schema       schema;
            int               constraintIndex = globalConstraintIndex;
            istrstream        schemaIn(globalSchemaString);
            std::vector<char> schemaBuffer;

            bdem_List         list;
            istrstream        listIn(globalConstrainedListString);
            std::vector<char> listBuffer;

            const char       *endPos;
            int               status;

            my_IoUtil::readStream(&schemaBuffer, schemaIn);
            status = bdempu_Schema::parseSchema(&endPos, &schema,
                                                &schemaBuffer.front());
            ASSERT(0 == status && "while parsing schema");

            if (veryVerbose) {
                int offset = endPos - &schemaBuffer.front();
                P(endPos);  P(offset);
            }

            if (0 == status) {
                my_IoUtil::readStream(&listBuffer, listIn);

                status = my_ParseUtil::validateAndLoadConstrainedList(
                             &list, cerr, listBuffer, schema, constraintIndex);

                ASSERT(0 == status && "while parsing list");
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING PARSE/GENERATE SCOPE
        //   Concerns:
        //     For the parsing method, the following properties must hold:
        //       When parsing is successful
        //           1.  Return value is correct
        //           2.  Ending position is correct
        //           3.  'result' value is correct
        //           4.  <WHITESPACE> characters are skipped
        //       When parsing is not successful
        //           1.  Return value is correct
        //           2.  Ending position is correct
        //
        //     For the generating method, the following properties must hold:
        //       1. Indentation is generated properly.
        //       2. The generated result can be parsed back with the
        //          corresponding parser function.
        //
        // Plan:
        //   To test the parsing method, create a table containing the input
        //   string and the expected output value.  Use nested loop
        //   ("orthogonal perturbation") to test each input to the parsing
        //   method with different <WHITESPACE> strings defined in global
        //   'BLANKS' structure.  In every iteration one of the <WHITESPACE>
        //   strings is inserted in the beginning of the input string.
        //
        //   As all the methods called from the 'parseScope' were already
        //   tested, we only need to test with following inputs
        //       1. Empty and not empty <COMPLIANT_RECORD>
        //
        //   To test generating method create a table containing the input
        //   object and the expected textual representation.  Test that the
        //   generate method correctly generates identation when 'level' is
        //   less than and greater than zero.
        //
        // Testing:
        //    parseScope(cchar **, Schema *, int, List *, cchar *);
        //    generateScope(b_ca *, cSchema&, int, cList&, int, int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PARSE/GENERATE SCOPE" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting parsing method" << endl;

        // Create test schema

        bdem_Schema x0, x1, x2;
        bdem_RecordDef *recordDef;
        recordDef = x1.createRecord();
        recordDef->appendField(bdem_ElemType::CHAR);
        recordDef->appendField(bdem_ElemType::STRING);
        recordDef->appendField(bdem_ElemType::INT);

        x2.createRecord();

        // Create test records

        bdem_List y0, y1, y2;
        y1.appendChar('a');
        y1.appendString("Hello!");
        y1.appendInt(1);

        y2.appendChar(bdetu_Null<char>::nullValue());
        y2.appendString(bdetu_Null<std::string>::nullValue());
        y2.appendInt(bdetu_Null<int>::nullValue());

        P(NUM_BLANKS)

        // gcc-3.4.3 segfaults below if bdem_Schema/bdem_List are passed by
        // value into the table below, even though it is a const table and
        // copies should be made and destroyed.  Upon end of scope of the
        // table, it dumps when trying to destroy x1 twice.  This is not a
        // problem with gcc-4.1.1 or any other compiler, but the fix is to
        // simply pass the structures by value.  Note that no const is given in
        // the struct, since DATA is already declared const, which propagates
        // to all the members. 

        if (verbose) cout << "\tTesting parser" << endl;
        {
            struct ScopeParsing {
                int          d_lineNum;    // source line number
                char        *d_str;        // input string
                int          d_blanks;     // number of inserted 'BLANKS'
                                           // before 'd_endPos' is
                                           // encountered.
                bdem_Schema *d_schema;     // expected result
                int          d_constraint; // expected result
                bdem_List   *d_list;       // expected result
                int          d_retVal;     // expected return value
                int          d_endPos;     // expected offset from the
                                                 // start
            };

            const ScopeParsing DATA[] = {
 //-------------^

 //line              d_str             #@  schema constr list retVal endPos
 //---- ------------------------------ --- ------ ------ ---- ------ ------

 { L_,  "@{"
          "@SCHEMA@{"
            "@RECORD@{"
              "@CHAR@;"
              "@STRING;"
              "@INT;"
            "@}"
          "@}"
           "DATA@<@{0}@>@{"
             "@{@0@}@=@\'a\'@ "
             "@{@1@}@=@\"Hello!\"@"
             "@{@2@}@=@1"
          "@}"
        "@}",                            0,  &x1,   0,   &y1,    0,    LEN  },

 { L_,  "@{"
          "@SCHEMA@{"
            "@RECORD@{"
              "@CHAR@;"
              "@STRING;"
              "@INT;"
            "@}"
          "@}"
           "DATA@<@{0}@>@{"
          "@}"
        "@}",                            0,  &x1,   0,   &y2,    0,    LEN  },

            };
 //-------------V
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            std::vector<char> buffer;
            bdem_Schema       resultSchema;
            int               resultConstraintIndex;
            bdem_List         resultData;

            for (int i = 0; i < NUM_DATA; ++i) {
                // Orthogonal perturbation for optional whitespace
                const ScopeParsing& DATA_I = DATA[i];
                const int LINE = DATA_I.d_lineNum;

                for (int j = 0; j < NUM_BLANKS; ++j) {
                    buffer.clear();
                    g(&buffer, DATA_I.d_str, j);
                    buffer.push_back('\0');

                    resultSchema.removeAll();
                    resultConstraintIndex = 111;
                    resultData.removeAll();

                    // Verify that expected value is different from initial
                    // value for valid inputs
                    if (0 == DATA_I.d_retVal) {
                        LOOP_ASSERT(LINE, *DATA_I.d_schema != resultSchema);
                        LOOP_ASSERT(LINE,
                            DATA_I.d_constraint != resultConstraintIndex);
                        LOOP_ASSERT(LINE, *DATA_I.d_list != resultData);
                    }

                    const char *endPos;
                    int res = bdempu_SchemaAggregate::parseScope(
                        &endPos, &resultSchema, &resultConstraintIndex,
                        &resultData, &buffer.front());

                    LOOP_ASSERT(LINE, DATA_I.d_retVal == res);
                    if (0 == DATA_I.d_retVal) {
                        LOOP_ASSERT(LINE, *DATA_I.d_schema == resultSchema);
                        LOOP_ASSERT(LINE,
                            DATA_I.d_constraint == resultConstraintIndex);
                        LOOP_ASSERT(LINE, *DATA_I.d_list == resultData);
                    }

                    // Verify correctness of the ending position offset
                    int expectedOffset;
                    if (LEN == DATA_I.d_endPos) {
                        expectedOffset = strlen(&buffer.front());
                    }
                    else if (LENSUB1 == DATA_I.d_endPos) {
                        expectedOffset = strlen(&buffer.front()) - 1;
                    }
                    else {
                        expectedOffset = DATA_I.d_endPos +
                                         DATA_I.d_blanks * strlen(BLANKS[j]);
                    }
                    int actualOffset = endPos - &buffer.front();
                    LOOP_ASSERT(LINE, expectedOffset == actualOffset);

                    if (expectedOffset != actualOffset) {
                        cout << "\t\texpected: " << expectedOffset
                             << ", " << "parsed: "
                             << actualOffset << endl;

                        cout << &buffer.front() << endl;
                    }
                }
            }
        }
        if (verbose) cout << "\tTesting generator" << endl;
        {
            struct ScopeGenerating {
                int          d_lineNum;    // source line number
                char        *d_prefix;     // characters in output buffer
                                           // before  a string
                int          d_level;      // identation level
                int          d_spaces;     // spaces per level
                bdem_Schema *d_schema;     // input
                int          d_constraint; // input
                bdem_List   *d_list;       // input
                char        *d_str;        // output string
            };

            const ScopeGenerating DATA[] = {
 //-------------^

 //line prefix  level sp schema constr list               d_str
 //---- ------- ----- -- ------ ------ ---- -------------------------------
 { L_,  "",       1,  2,  &x1,    0,   &y1, "  {\n"
                                            "    SCHEMA {\n"
                                            "      RECORD {\n"
                                            "        CHAR ;\n"
                                            "        STRING ;\n"
                                            "        INT ;\n"
                                            "      }\n"
                                            "    }\n"
                                            "    DATA < {0} > {\n"
                                            "      {0} = \'a\'\n"
                                            "      {1} = Hello!\n"
                                            "      {2} = 1\n"
                                            "    }\n"
                                            "  }"                           },
            };
 //-------------V

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            std::vector<char> buffer;
            for (int i = 0; i < NUM_DATA; ++i) {
                const ScopeGenerating& DATA_I = DATA[i];
                const int LINE = DATA_I.d_lineNum;
                buffer.clear();

                appendToBuffer(&buffer, DATA_I.d_prefix);

                bdempu_SchemaAggregate::generateScope(
                    &buffer, *DATA_I.d_schema, DATA_I.d_constraint,
                    *DATA_I.d_list, DATA_I.d_level, DATA_I.d_spaces);

                buffer.push_back('\0');
                int diff = strcmp(&buffer.front(), DATA_I.d_str);
                LOOP_ASSERT(LINE, !diff);
                if (diff) {
                    cout << "\t\texpected:"  << DATA_I.d_str    << endl;
                    cout << "\t\tgenerated:" << &buffer.front() << endl;
                }
                LOOP_ASSERT(LINE, buffer[buffer.size() - 1] == '\0');

                // Verify that the we can get the original value by parsing the
                // generated string.
                bdem_Schema      resultSchema;
                int              index = 111;
                bdem_List        resultData;
                LOOP_ASSERT(LINE, *DATA_I.d_schema != resultSchema);
                LOOP_ASSERT(LINE, DATA_I.d_constraint != index);
                LOOP_ASSERT(LINE, *DATA_I.d_list != resultData);

                const char *endPos;
                int res = bdempu_SchemaAggregate::parseScope(
                    &endPos, &resultSchema, &index, &resultData,
                    &buffer.front() + strlen(DATA_I.d_prefix));
                LOOP_ASSERT(LINE, 0 == res);
                LOOP_ASSERT(LINE, *DATA_I.d_schema == resultSchema);
                LOOP_ASSERT(LINE, DATA_I.d_constraint == index);
                LOOP_ASSERT(LINE, *DATA_I.d_list == resultData);
                LOOP_ASSERT(LINE, '\0' == *endPos);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PARSE/GENERATE CONSTRAINED TABLE
        //   Concerns:
        //     For the parsing method, the following properties must hold:
        //       When parsing is successful
        //           1.  Return value is correct
        //           2.  Ending position is correct
        //           3.  'result' value is correct
        //           4.  <WHITESPACE> characters are skipped
        //       When parsing is not successful
        //           1.  Return value is correct
        //           2.  Ending position is correct
        //
        //     For the generating method, the following properties must hold:
        //       1. Indentation is generated properly.
        //       2. The generated result can be parsed back with the
        //          corresponding parser function.
        //
        // Plan:
        //   To test the parsing method, create a table containing the input
        //   string and the expected output value.  Use nested loop
        //   ("orthogonal perturbation") to test each input to the parsing
        //   method with different <WHITESPACE> strings defined in global
        //   'BLANKS' structure.  In every iteration one of the <WHITESPACE>
        //   strings is inserted in the beginning of the input string.
        //
        //   Test with the following valid constrained lists:
        //       1. NULL
        //       2. Empty table
        //       3. Table with one association of each type
        //       4. Table with multiple associations
        //       5. Table where field locator is specified by <INTEGER_LOCATOR>
        //       6. Table where field locator is specified by <IDENTIFIER>
        //       7. Table where field locator is specified by <QUOTED_STRING>
        //
        //   Test with the following invalid inputs:
        //       1. Opening/closing curly brace is missing
        //       2. Coma separating <ASSOCIATIONS> is missing
        //       3. Invalid field locator, where field locator is either
        //          <INTEGER_LOCATOR>, <IDENTIFIER>, or <QUOTED_STRING>
        //       4. Mismatch between value and type of the field locator
        //
        //   To test generating method create a table containing the input
        //   object and the expected textual representation.  Test that the
        //   generate method correctly generates identation when 'level' is
        //   less than and greater than zero.
        //
        // Testing:
        //    parseConstrainedTable(cchar **, Table *, cchar *, cRecDef&);
        //    generateConstrainedTable(b_ca *, cTable&, cRecDef&, int, int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PARSE/GENERATE TABLE" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting parsing method" << endl;

        typedef bdem_Table T;
        typedef bdes_PlatformUtil::Int64 Int64;

        const char X_CHAR = 'a';
        const short X_SHORT = 1;
        const int X_INT = 1;
        const Int64 X_INT64 = 1;
        const float X_FLOAT = 1;
        const double X_DOUBLE = 1;
        const std::string X_STRING = "Hello!";
        const bdet_Date X_DATE(3, 3, 3);
        const bdet_Time X_TIME(3, 3, 3, 3);
        const bdet_Datetime X_DATETIME(X_DATE, X_TIME);

        T stub;      // Identifies undefined value
        T initValue; // Used to initialize result
        std::vector<bdem_ElemType::Type> columnTypeArray;
        columnTypeArray.push_back(bdem_ElemType::STRING);
        initValue.reset(&columnTypeArray.front(), columnTypeArray.size());
        bdem_List initList;
        initList.appendString("init");
        initValue.appendRow(initList);

        T x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
        bdem_List xList, xCharList;
        xCharList.appendChar('a');

        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::CHAR);
        columnTypeArray.push_back(bdem_ElemType::INT);
        columnTypeArray.push_back(bdem_ElemType::STRING);
        x0.reset(&columnTypeArray.front(), columnTypeArray.size());

        xList.appendChar('a');
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::CHAR);
        x1.reset(&columnTypeArray.front(), columnTypeArray.size());
        x1.appendRow(xList);

        xList.removeAll();
        xList.appendShort(1);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::SHORT);
        x2.reset(&columnTypeArray.front(), columnTypeArray.size());
        x2.appendRow(xList);

        xList.removeAll();
        xList.appendInt(1);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::INT);
        x3.reset(&columnTypeArray.front(), columnTypeArray.size());
        x3.appendRow(xList);

        xList.removeAll();
        xList.appendInt64(1);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::INT64);
        x4.reset(&columnTypeArray.front(), columnTypeArray.size());
        x4.appendRow(xList);

        xList.removeAll();
        xList.appendFloat(1);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::FLOAT);
        x5.reset(&columnTypeArray.front(), columnTypeArray.size());
        x5.appendRow(xList);

        xList.removeAll();
        xList.appendDouble(1);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::DOUBLE);
        x6.reset(&columnTypeArray.front(), columnTypeArray.size());
        x6.appendRow(xList);

        xList.removeAll();
        xList.appendString("Hello!");
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::STRING);
        x7.reset(&columnTypeArray.front(), columnTypeArray.size());
        x7.appendRow(xList);

        xList.removeAll();
        xList.appendDatetime(X_DATETIME);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::DATETIME);
        x8.reset(&columnTypeArray.front(), columnTypeArray.size());
        x8.appendRow(xList);

        xList.removeAll();
        xList.appendDate(X_DATE);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::DATE);
        x9.reset(&columnTypeArray.front(), columnTypeArray.size());
        x9.appendRow(xList);

        xList.removeAll();
        xList.appendTime(X_TIME);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::TIME);
        x10.reset(&columnTypeArray.front(), columnTypeArray.size());
        x10.appendRow(xList);

        T x11, x12, x13, x14, x15, x16, x17, x18, x19, x20;
        const std::vector<char> A1(1, X_CHAR);
        xList.removeAll();
        xList.appendCharArray(A1);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::CHAR_ARRAY);
        x11.reset(&columnTypeArray.front(), columnTypeArray.size());
        x11.appendRow(xList);

        xList.removeAll();
        const std::vector<short> A2(1, X_SHORT);
        xList.removeAll();
        xList.appendShortArray(A2);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::SHORT_ARRAY);
        x12.reset(&columnTypeArray.front(), columnTypeArray.size());
        x12.appendRow(xList);

        xList.removeAll();
        const std::vector<int> A3(1, X_INT);
        xList.removeAll();
        xList.appendIntArray(A3);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::INT_ARRAY);
        x13.reset(&columnTypeArray.front(), columnTypeArray.size());
        x13.appendRow(xList);

        xList.removeAll();
        const std::vector<Int64> A4(1, X_INT64);
        xList.removeAll();
        xList.appendInt64Array(A4);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::INT64_ARRAY);
        x14.reset(&columnTypeArray.front(), columnTypeArray.size());
        x14.appendRow(xList);

        xList.removeAll();
        const std::vector<float> A5(1, X_FLOAT);
        xList.removeAll();
        xList.appendFloatArray(A5);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::FLOAT_ARRAY);
        x15.reset(&columnTypeArray.front(), columnTypeArray.size());
        x15.appendRow(xList);

        xList.removeAll();
        const std::vector<double> A6(1, X_DOUBLE);
        xList.removeAll();
        xList.appendDoubleArray(A6);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::DOUBLE_ARRAY);
        x16.reset(&columnTypeArray.front(), columnTypeArray.size());
        x16.appendRow(xList);

        xList.removeAll();
        const std::vector<std::string> A7(1, X_STRING);
        xList.removeAll();
        xList.appendStrArray(A7);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::STRING_ARRAY);
        x17.reset(&columnTypeArray.front(), columnTypeArray.size());
        x17.appendRow(xList);

        xList.removeAll();
        const std::vector<bdet_Datetime> A8(1, X_DATETIME);
        xList.removeAll();
        xList.appendDatetimeArray(A8);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::DATETIME_ARRAY);
        x18.reset(&columnTypeArray.front(), columnTypeArray.size());
        x18.appendRow(xList);

        xList.removeAll();
        const std::vector<bdet_Date> A9(1, X_DATE);
        xList.removeAll();
        xList.appendDateArray(A9);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::DATE_ARRAY);
        x19.reset(&columnTypeArray.front(), columnTypeArray.size());
        x19.appendRow(xList);

        xList.removeAll();
        const std::vector<bdet_Time> A10(1, X_TIME);
        xList.appendTimeArray(A10);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::TIME_ARRAY);
        x20.reset(&columnTypeArray.front(), columnTypeArray.size());
        x20.appendRow(xList);

        T x21, x22, x23h, x23, x24, x25;

        xList.removeAll();
        xList.appendList(xCharList);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::LIST);
        x21.reset(&columnTypeArray.front(), columnTypeArray.size());
        x21.appendRow(xList);

        xList.removeAll();
        xList.appendTable(x1);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::TABLE);
        x22.reset(&columnTypeArray.front(), columnTypeArray.size());
        x22.appendRow(xList);

        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::CHAR);
        columnTypeArray.push_back(bdem_ElemType::INT);
        columnTypeArray.push_back(bdem_ElemType::STRING);
        xList.removeAll();
        xList.appendChar(X_CHAR);
        xList.appendInt(X_INT);
        xList.appendString(X_STRING);
        x23.reset(&columnTypeArray.front(), columnTypeArray.size());
        x23.appendRow(xList);

        bdem_List rowList;
        rowList.appendChar(X_CHAR);
        rowList.appendInt(X_INT);
        rowList.appendString(X_STRING);
        xList.removeAll();
        xList.appendList(rowList);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::LIST);
        x23h.reset(&columnTypeArray.front(), columnTypeArray.size());
        x23h.appendRow(xList);

        // TABLE 24
        // Set column types for inner tables
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::CHAR);
        columnTypeArray.push_back(bdem_ElemType::INT);
        columnTypeArray.push_back(bdem_ElemType::STRING);
        T hTable, iTable;
        iTable.reset(&columnTypeArray.front(), columnTypeArray.size());
        hTable.reset(&columnTypeArray.front(), columnTypeArray.size());

        // Set column types for outer table
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::TABLE);
        x24.reset(&columnTypeArray.front(), columnTypeArray.size());

        // Append null table
        xList.removeAll();
        xList.appendTable(iTable);
        x24.appendRow(xList);

        // Populate other inner tables
        xList.removeAll();
        xList.appendChar(bdetu_Null<char>::nullValue());
        xList.appendInt(bdetu_Null<int>::nullValue());
        xList.appendString(bdetu_Null<std::string>::nullValue());
        hTable.appendRow(xList);
        hTable.appendRow(xList);

        // Insert inner table into the outer table
        xList.removeAll();
        xList.appendTable(hTable);
        x24.appendRow(xList);
        x24.appendRow(xList);
        x24.appendRow(xList);

        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::CHAR);
        columnTypeArray.push_back(bdem_ElemType::INT);
        columnTypeArray.push_back(bdem_ElemType::STRING);
        xList.removeAll();
        xList.appendChar(X_CHAR);
        xList.appendInt(X_INT);
        xList.appendString(X_STRING);
        T sTable;
        sTable.reset(&columnTypeArray.front(), columnTypeArray.size());
        sTable.appendRow(xList);
        sTable.appendRow(xList);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::TABLE);
        x25.reset(&columnTypeArray.front(), columnTypeArray.size());
        xList.removeAll();
        xList.appendTable(sTable);
        x25.appendRow(xList);
        x25.appendRow(xList);

        const int MAX_RECDEF = 30;
        const int MAX_NONAME = 7;
        bdem_Schema s;
        bdem_RecordDef *recDef[MAX_RECDEF];
        char name[20];
        for (int i = 0; i < MAX_NONAME; ++i) {
            recDef[i] = s.createRecord();
        }

        for (int i = MAX_NONAME; i < MAX_RECDEF; ++i) {
            sprintf(name, "c%d", i);
            recDef[i] = s.createRecord(name);
        }

        recDef[ 0]->appendField(bdem_ElemType::CHAR,           "0");
        recDef[ 0]->appendField(bdem_ElemType::INT,            "1");
        recDef[ 0]->appendField(bdem_ElemType::STRING,         "2");

        recDef[ 1]->appendField(bdem_ElemType::CHAR,           "x1");
        recDef[ 2]->appendField(bdem_ElemType::SHORT,          "x2");
        recDef[ 3]->appendField(bdem_ElemType::INT,            "x3");
        recDef[ 4]->appendField(bdem_ElemType::INT64,          "x4");
        recDef[ 5]->appendField(bdem_ElemType::FLOAT,          "x5");
        recDef[ 6]->appendField(bdem_ElemType::DOUBLE,         "x6");
        recDef[ 7]->appendField(bdem_ElemType::STRING,         "x7");
        recDef[ 8]->appendField(bdem_ElemType::DATETIME,       "x8");
        recDef[ 9]->appendField(bdem_ElemType::DATE,           "x9");
        recDef[10]->appendField(bdem_ElemType::TIME,           "x10");
        recDef[11]->appendField(bdem_ElemType::CHAR_ARRAY,     "x11");
        recDef[12]->appendField(bdem_ElemType::SHORT_ARRAY,    "x12");
        recDef[13]->appendField(bdem_ElemType::INT_ARRAY,      "x13");
        recDef[14]->appendField(bdem_ElemType::INT64_ARRAY,    "x14");
        recDef[15]->appendField(bdem_ElemType::FLOAT_ARRAY,    "x15");
        recDef[16]->appendField(bdem_ElemType::DOUBLE_ARRAY,   "x16");
        recDef[17]->appendField(bdem_ElemType::STRING_ARRAY,   "x17");
        recDef[18]->appendField(bdem_ElemType::DATETIME_ARRAY, "x18");
        recDef[19]->appendField(bdem_ElemType::DATE_ARRAY,     "x19");
        recDef[20]->appendField(bdem_ElemType::TIME_ARRAY,     "x20");
        recDef[21]->appendField(bdem_ElemType::LIST,           "x21");
        recDef[22]->appendField(bdem_ElemType::TABLE,          "x22");

        recDef[23]->appendConstrainedList(*recDef[0],          "x23");
        recDef[24]->appendConstrainedTable(*recDef[0],         "x24");

        if (verbose) cout << "\tTesting parser" << endl;
        {
            const DataParsing<T> DATA[] = {
 //-------------^

 //line               d_str               #@  constr result retVal  endPos
 //---- --------------------------------- --- ------ ------ ------- -------

 { L_,  "@NULL",                           1,    0,    x0,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@{@\'a\'@}@}",       10,    1,    x1,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@{@1@}@}",           10,    2,    x2,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@{@1@}@}",           10,    3,    x3,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@{@1@}@}",           10,    4,    x4,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@{@1@}@}",           10,    5,    x5,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@{@1@}@}",           10,    6,    x6,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@{@\"Hello!\"@}@}",  10,    7,    x7,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@"
         "{@3/3/3 3:03:03.003@}@}",       10,    8,    x8,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@{@3/3/3@}@}",       10,    9,    x9,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@{@3:03:03.003@}@}", 10,   10,   x10,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@{@['a']@}@}",       10,   11,   x11,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@{@[1]@}@}",         10,   12,   x12,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@{@[1]@}@}",         10,   13,   x13,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@{@[1]@}@}",         10,   14,   x14,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@{@[1]@}@}",         10,   15,   x15,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@{@[1]@}@}",         10,   16,   x16,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@"
         "{@[\"Hello!\"]@}@}",            10,   17,   x17,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@"
         "{@[3/3/3 3:03:03.003]@}@}",     10,   18,   x18,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@{@[3/3/3]@}@}",     10,   19,   x19,     0,      LEN  },
 { L_,  "@{@[@{@0@}@]@{@[3:03:03.003]@}@}",  10, 20,  x20,     0,      LEN  },

 { L_,  "@{@[@{@0@}@]"
         "@{@{CHAR \'a\'}@}@}",           10,   21,   x21,     0,      LEN  },

 { L_,  "@{@[@{@0@}@]"
         "@{@{[CHAR]{\'a\'}}@}@}",        10,   22,   x22,     0,      LEN  },

 { L_,  "@{@[@"
             "{@0@}@@"
             "{@1@}@@"
             "{@2@}@"
           "]@"
           "{@"
             "\'a\' "
             "1 "
             "\"Hello!\" "
          "@}"
        "@}",                             18,    0,   x23,     0,      LEN  },

 { L_,  "@{@[@"
             "{@0@}@"
           "]@"
           "{@"
             "{"
               "{0}=\'a\' "
               "{1}=1 "
               "{2}=\"Hello!\" "
            "@}"
          "@}"
        "@}",                             10,   23,  x23h,     0,      LEN  },

 { L_,  "@{@"
           "[@"
             "{@0@}@"
           "]@"
           "{@"
             "NULL"
          "@}"
           "{@"
             "{@"
               "[@"
                 "{@1@}@"
               "]@"
               "{@"
                 "NUll"
              "@}@"
               "{@"
                 "Null"
              "@}"
            "@}"
          "@}"
           "{@"
             "{@"
               "[@"
                 "{@0@}@ "
                 "{@1@}@ "
                 "{@2@}@"
               "]@"
               "{@NULL@ @NULL@ @NULL@}@"
               "{@NULL@ @NULL@ @NULL@}@"
            "@}"
          "@}"
           "{@"
             "{@"
               "[@"
                 "{@1@}@ "
                 "{@1@}@ "
                 "{@1@}@"
               "]@"
               "{@1@ @2@ @NULL@}@"
               "{@NULL@ @NULL@ @NULL@}@"
            "@}"
          "@}"
        "@}",                             48,   24,   x24,     0,      LEN  },

 { L_,  "@{@"
           "[@"
             "{@0@}@"
           "]@"
           "{@"
             "{@"
               "[@"
                 "{@0@}@ "
                 "{@1@}@ "
                 "{@2@}@"
               "]@"
               "{@"
                 "\'a\' "
                 "1 "
                 "\"Hello!\" "
              "@}@"
               "{@"
                 "\'a\' "
                 "1 "
                 "\"Hello!\" "
              "@}"
            "@}"
          "@}"
           "{@"
             "{@"
               "[@"
                 "{@0@}@ "
                 "{@1@}@ "
                 "{@2@}@"
               "]@"
               "{@"
                 "\'a\' "
                 "1 "
                 "\"Hello!\" "
              "@}@"
               "{@"
                 "\'a\' "
                 "1 "
                 "\"Hello!\" "
              "@}"
            "@}"
          "@}"
        "@}",                             48,   24,   x25,     0,      LEN  },

                // Testing with invalid input

                // Integer locator is out of range
 { L_,  "@{@[@{@100@}@]@{@\'a\'@}@}",      5,    1,    x1,    FAILURE,  6    },
 { L_,  "@{@[@\"100\"@]@{@\'a\'@}@}",      3,    1,    x1,    FAILURE,  6    },
 { L_,  "@{@[@ abc @]@{@\'a\'@}@}",        3,    1,    x1,    FAILURE,  5    },

            };
 //-------------V
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            std::vector<char> buffer;
            for (int i = 0; i < NUM_DATA; ++i) {
                // Orthogonal perturbation for optional whitespace
                const DataParsing<T>& DATA_I = DATA[i];
                const int LINE = DATA_I.d_lineNum;

                for (int j = 0; j < 2/*NUM_BLANKS*/; ++j) {
                    buffer.clear();
                    g(&buffer, DATA_I.d_str, j);
                    buffer.push_back('\0');

                    // Initialize 'result'
                    T result = initValue;

                    // Verify that expected value is different from initial
                    // value for valid inputs
                    if (0 == DATA_I.d_retVal) {
                        LOOP_ASSERT(LINE, DATA_I.d_result != initValue);
                    }

                    const char *endPos;
                    int res = bdempu_SchemaAggregate::parseConstrainedTable(
                        &endPos, &result, &buffer.front(),
                        s.record(DATA_I.d_constraint));
                    LOOP_ASSERT(LINE, DATA_I.d_retVal == res);
                    if (0 == DATA_I.d_retVal) {
                        LOOP_ASSERT(LINE, DATA_I.d_result == result);
                    }

                    // Verify correctness of the ending position offset
                    int expectedOffset;
                    if (LEN == DATA_I.d_endPos) {
                        expectedOffset = strlen(&buffer.front());
                    }
                    else if (LENSUB1 == DATA_I.d_endPos) {
                        expectedOffset = strlen(&buffer.front()) - 1;
                    }
                    else {
                        expectedOffset = DATA_I.d_endPos +
                                         DATA_I.d_blanks * strlen(BLANKS[j]);
                    }
                    int actualOffset = endPos - &buffer.front();
                    LOOP_ASSERT(LINE, expectedOffset == actualOffset);

                    if (expectedOffset != actualOffset) {
                        cout << "\t\texpected: " << expectedOffset
                             << ", " << "parsed: "
                             << actualOffset << endl;

                        cout << &buffer.front() << endl;
                    }
                }
            }
        }

        if (verbose) cout << "\tTesting generator" << endl;
        {
            const DataGenerating<T> DATA[] = {
 //-------------^

 //line prefix   level sp d_val constr                 d_str
 //---- -------  ----- -- ----- ------ --------------------------------------

 { L_,  "",        1,  2,  x0,    0,   "  NULL"                              },
 { L_,  " z = ",  -1,  1,  x0,    0,   " z = NULL"                           },

 { L_,  "",        1,  2,  x1,    1,   "  {\n"
                                       "    {\n"
                                       "      x1 = \'a\'\n"
                                       "    }\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x1,    1,   " z = {\n"
                                       "  {\n"
                                       "   x1 = \'a\'\n"
                                       "  }\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x23h, 23,   "  {\n"
                                       "    {\n"
                                       "      x23 = {\n"
                                       "        \"0\" = \'a\'\n"
                                       "        \"1\" = 1\n"
                                       "        \"2\" = Hello!\n"
                                       "      }\n"
                                       "    }\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x23h, 23,   " z = {\n"
                                       "  {\n"
                                       "   x23 = {\n"
                                       "    \"0\" = \'a\'\n"
                                       "    \"1\" = 1\n"
                                       "    \"2\" = Hello!\n"
                                       "   }\n"
                                       "  }\n"
                                       " }",                                 },
            };
 //-------------V

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            std::vector<char> buffer;
            for (int i = 0; i < NUM_DATA; ++i) {
                const DataGenerating<T>& DATA_I = DATA[i];
                const int LINE = DATA_I.d_lineNum;
                buffer.clear();

                appendToBuffer(&buffer, DATA_I.d_prefix);
                bdempu_SchemaAggregate::generateConstrainedTable(
                    &buffer, DATA_I.d_val, s.record(DATA_I.d_constr),
                    DATA_I.d_level, DATA_I.d_spaces);

                buffer.push_back('\0');
                int diff = strcmp(&buffer.front(), DATA_I.d_str);
                LOOP_ASSERT(LINE, !diff);
                if (diff) {
                    cout << "\t\texpected:"  << DATA_I.d_str    << endl;
                    cout << "\t\tgenerated:" << &buffer.front() << endl;
                }
                LOOP_ASSERT(LINE, buffer[buffer.size() - 1] == '\0');

                // Verify that the we can get the original value by parsing the
                // generated string.
                T result = initValue;
                LOOP_ASSERT(LINE, DATA_I.d_val != result);
                const char *endPos;
                int ret = bdempu_SchemaAggregate::parseConstrainedTable(
                    &endPos, &result,
                    &buffer.front() + strlen(DATA_I.d_prefix),
                    s.record(DATA_I.d_constr));
                LOOP_ASSERT(LINE, 0 == ret);
                LOOP_ASSERT(LINE, DATA_I.d_val == result);
                LOOP_ASSERT(LINE, '\0' == *endPos);
             }
          }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PARSE/GENERATE CONSTRAINED LIST
        //   Concerns:
        //     For the parsing method, the following properties must hold:
        //       When parsing is successful
        //           1.  Return value is correct
        //           2.  Ending position is correct
        //           3.  'result' value is correct
        //           4.  <WHITESPACE> characters are skipped
        //       When parsing is not successful
        //           1.  Return value is correct
        //           2.  Ending position is correct
        //
        //     For the generating method, the following properties must hold:
        //       1. Indentation is generated properly.
        //       2. The generated result can be parsed back with the
        //          corresponding parser function.
        //
        // Plan:
        //   To test the parsing method, create a table containing the input
        //   string and the expected output value.  Use nested loop
        //   ("orthogonal perturbation") to test each input to the parsing
        //   method with different <WHITESPACE> strings defined in global
        //   'BLANKS' structure.  In every iteration one of the <WHITESPACE>
        //   strings is inserted in the beginning of the input string.
        //
        //   Test with the following valid constrained lists:
        //       1. NULL
        //       2. Empty list
        //       3. List with one association of each type
        //       4. List with multiple associations
        //       5. List where field locator is specified by <INTEGER_LOCATOR>
        //       6. List where field locator is specified by <IDENTIFIER>
        //       7. List where field locator is specified by <QUOTED_STRING>
        //       8. Test with each field having a NULL value.
        //
        //   Test with the following invalid inputs:
        //       1. Opening/closing curly brace is missing
        //       2. Invalid field locator, where field locator is either
        //          <INTEGER_LOCATOR>, <IDENTIFIER>, or <QUOTED_STRING>
        //       3. Mismatch between value and type of the field locator
        //
        //   To test generating method create a table containing the input
        //   object and the expected textual representation.  Test that the
        //   generate method correctly generates identation when 'level' is
        //   less than and greater than zero.
        //
        // Testing:
        //    parseConstrainedList(cchar **, List *, cchar *, cRecDef&);
        //    generateConstrainedList(b_ca *, cList&, cRecDef&, int, int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PARSE/GENERATE LIST" << endl
                          << "===========================" << endl;

        typedef bdem_List T;
        typedef bdes_PlatformUtil::Int64 Int64;

        const char X_CHAR = 'a';
        const short X_SHORT = 1;
        const int X_INT = 1;
        const Int64 X_INT64 = 1;
        const float X_FLOAT = 1;
        const double X_DOUBLE = 1;
        const std::string X_STRING = "Hello!";
        const bdet_Date X_DATE(3, 3, 3);
        const bdet_Time X_TIME(3, 3, 3, 3);
        const bdet_Datetime X_DATETIME(X_DATE, X_TIME);
        const std::vector<char> X_A1(1, X_CHAR);

        T stub;     // Identifies undefined value
        T initValue;
        initValue.appendString("init"); // Used to initialize result
        T x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;

        x0.appendChar(bdetu_Null<char>::nullValue());
        x0.appendInt(bdetu_Null<int>::nullValue());
        x0.appendString(bdetu_Null<std::string>::nullValue());

        x1.appendChar('a');
        x2.appendShort(1);
        x3.appendInt(1);
        x4.appendInt64(1);
        x5.appendFloat(1);
        x6.appendDouble(1);
        x7.appendString("Hello!");
        x8.appendDatetime(X_DATETIME);
        x9.appendDate(X_DATE);
        x10.appendTime(X_TIME);

        T x11, x12, x13, x14, x15, x16, x17, x18, x19, x20;
        const std::vector<char> A1(1, X_CHAR);
        x11.appendCharArray(A1);
        const std::vector<short> A2(1, X_SHORT);
        x12.appendShortArray(A2);
        const std::vector<int> A3(1, X_INT);
        x13.appendIntArray(A3);
        const std::vector<Int64> A4(1, X_INT64);
        x14.appendInt64Array(A4);
        const std::vector<float> A5(1, X_FLOAT);
        x15.appendFloatArray(A5);
        const std::vector<double> A6(1, X_DOUBLE);
        x16.appendDoubleArray(A6);
        const std::vector<std::string> A7(1, X_STRING);
        x17.appendStrArray(A7);
        const std::vector<bdet_Datetime> A8(1, X_DATETIME);
        x18.appendDatetimeArray(A8);
        const std::vector<bdet_Date> A9(1, X_DATE);
        x19.appendDateArray(A9);
        const std::vector<bdet_Time> A10(1, X_TIME);
        x20.appendTimeArray(A10);

        T x21, x22, x23h, x23, x24;

        x21.appendList(x1);
        std::vector<bdem_ElemType::Type> columnTypeArray;
        columnTypeArray.push_back(bdem_ElemType::CHAR);
        bdem_Table xTable(&columnTypeArray.front(), columnTypeArray.size());
        xTable.appendRow(x1);
        x22.appendTable(xTable);

        x23h.appendChar(X_CHAR);
        x23h.appendInt(X_INT);
        x23h.appendString(X_STRING);

        x23.appendList(x23h);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::CHAR);
        columnTypeArray.push_back(bdem_ElemType::INT);
        columnTypeArray.push_back(bdem_ElemType::STRING);
        xTable.removeAll();
        xTable.reset(&columnTypeArray.front(), columnTypeArray.size());
        xTable.appendRow(x23h);
        xTable.appendRow(x23h);
        x24.appendTable(xTable);

        T x25h, x25, x26;

        x25h.appendChar(bdetu_Null<char>::nullValue());
        x25h.appendInt(X_INT);
        x25h.appendString(bdetu_Null<std::string>::nullValue());

        x25.appendList(x25h);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::CHAR);
        columnTypeArray.push_back(bdem_ElemType::INT);
        columnTypeArray.push_back(bdem_ElemType::STRING);
        xTable.removeAll();
        xTable.reset(&columnTypeArray.front(), columnTypeArray.size());
        xTable.appendRow(x25h);
        xTable.appendRow(x25h);
        x26.appendTable(xTable);

        const int MAX_RECDEF = 30;
        const int MAX_NONAME = 7;
        bdem_Schema s;
        bdem_RecordDef *recDef[MAX_RECDEF];
        char name[20];
        for (int i = 0; i < MAX_NONAME; ++i) {
            recDef[i] = s.createRecord();
        }

        for (int i = MAX_NONAME; i < MAX_RECDEF; ++i) {
            sprintf(name, "c%d", i);
            recDef[i] = s.createRecord(name);
        }

        recDef[ 0]->appendField(bdem_ElemType::CHAR,           "0");
        recDef[ 0]->appendField(bdem_ElemType::INT,            "1");
        recDef[ 0]->appendField(bdem_ElemType::STRING,         "2");

        recDef[ 1]->appendField(bdem_ElemType::CHAR,           "x1");
        recDef[ 2]->appendField(bdem_ElemType::SHORT,          "x2");
        recDef[ 3]->appendField(bdem_ElemType::INT,            "x3");
        recDef[ 4]->appendField(bdem_ElemType::INT64,          "x4");
        recDef[ 5]->appendField(bdem_ElemType::FLOAT,          "x5");
        recDef[ 6]->appendField(bdem_ElemType::DOUBLE,         "x6");
        recDef[ 7]->appendField(bdem_ElemType::STRING,         "x7");
        recDef[ 8]->appendField(bdem_ElemType::DATETIME,       "x8");
        recDef[ 9]->appendField(bdem_ElemType::DATE,           "x9");
        recDef[10]->appendField(bdem_ElemType::TIME,           "x10");
        recDef[11]->appendField(bdem_ElemType::CHAR_ARRAY,     "x11");
        recDef[12]->appendField(bdem_ElemType::SHORT_ARRAY,    "x12");
        recDef[13]->appendField(bdem_ElemType::INT_ARRAY,      "x13");
        recDef[14]->appendField(bdem_ElemType::INT64_ARRAY,    "x14");
        recDef[15]->appendField(bdem_ElemType::FLOAT_ARRAY,    "x15");
        recDef[16]->appendField(bdem_ElemType::DOUBLE_ARRAY,   "x16");
        recDef[17]->appendField(bdem_ElemType::STRING_ARRAY,   "x17");
        recDef[18]->appendField(bdem_ElemType::DATETIME_ARRAY, "x18");
        recDef[19]->appendField(bdem_ElemType::DATE_ARRAY,     "x19");
        recDef[20]->appendField(bdem_ElemType::TIME_ARRAY,     "x20");
        recDef[21]->appendField(bdem_ElemType::LIST,           "x21");
        recDef[22]->appendField(bdem_ElemType::TABLE,          "x22");

        recDef[23]->appendConstrainedList(*recDef[0],          "x23");
        recDef[24]->appendConstrainedTable(*recDef[0],         "x24");

        if (verbose) cout << "\tTesting parser" << endl;
        {
            const DataParsing<T> DATA[] = {
 //-------------^

 //line               d_str               #@  constr result retVal  endPos
 //---- --------------------------------- --- ------ ------ ------- -------
 { L_,  "@NULL",                           2,    0,    x0,     0,      LEN  },
 { L_,  "@{@{@0@}@=@\'a\'@}",              7,    1,    x1,     0,      LEN  },
 { L_,  "@{@x1@=@\'a\'@}",                 5,    1,    x1,     0,      LEN  },
 { L_,  "@{@\"x1\"@=@\'a\'@}",             5,    1,    x1,     0,      LEN  },
 { L_,  "@{@{@0@}@=@1@}",                  7,    2,    x2,     0,      LEN  },
 { L_,  "@{@x2@=@1@}",                     5,    2,    x2,     0,      LEN  },
 { L_,  "@{@\"x2\"@=@1@}",                 5,    2,    x2,     0,      LEN  },
 { L_,  "@{@{@0@}@=@1@}",                  7,    3,    x3,     0,      LEN  },
 { L_,  "@{@x3@=@1@}",                     5,    3,    x3,     0,      LEN  },
 { L_,  "@{@\"x3\"@=@1@}",                 5,    3,    x3,     0,      LEN  },
 { L_,  "@{@{@0@}@=@1@}",                  7,    4,    x4,     0,      LEN  },
 { L_,  "@{@x4@=@1@}",                     5,    4,    x4,     0,      LEN  },
 { L_,  "@{@\"x4\"@=@1@}",                 5,    4,    x4,     0,      LEN  },
 { L_,  "@{@{@0@}@=@1@}",                  7,    5,    x5,     0,      LEN  },
 { L_,  "@{@x5@=@1@}",                     5,    5,    x5,     0,      LEN  },
 { L_,  "@{@\"x5\"@=@1@}",                 5,    5,    x5,     0,      LEN  },
 { L_,  "@{@{@0@}@=@1@}",                  7,    6,    x6,     0,      LEN  },
 { L_,  "@{@x6@=@1@}",                     5,    6,    x6,     0,      LEN  },
 { L_,  "@{@\"x6\"@=@1@}",                 5,    6,    x6,     0,      LEN  },
 { L_,  "@{@{@0@}@=@\"Hello!\"@}",         7,    7,    x7,     0,      LEN  },
 // hard-coded whitespace necessary in this next vector because fully general
 // unquoted strings will consume "//" or "/*" as part of previous char seq.
 { L_,  "@{@x7@=@Hello! @}",               5,    7,    x7,     0,      LEN  },
 { L_,  "@{@\"x7\"@=@\"Hello!\"@}",        5,    7,    x7,     0,      LEN  },
 { L_,  "@{@{@0@}@=@3/3/3 3:03:03.003@}",  7,    8,    x8,     0,      LEN  },
 { L_,  "@{@x8@=@3/3/3 3:03:03.003@}",     5,    8,    x8,     0,      LEN  },
 { L_,  "@{@\"x8\"@=@3/3/3 3:03:03.003@}", 5,    8,    x8,     0,      LEN  },
 { L_,  "@{@{@0@}@=@3/3/3@}",              7,    9,    x9,     0,      LEN  },
 { L_,  "@{@x9@=@3/3/3@}",                 5,    9,    x9,     0,      LEN  },
 { L_,  "@{@\"x9\"@=@3/3/3@}",             5,    9,    x9,     0,      LEN  },
 { L_,  "@{@{@00@}@=@3:03:03.003@}",       7,   10,    x10,    0,      LEN  },
 { L_,  "@{@x10@=@3:03:03.003@}",          5,   10,    x10,    0,      LEN  },
 { L_,  "@{@\"x10\"@=@3:03:03.003@}",      5,   10,    x10,    0,      LEN  },
 { L_,  "@{@{@00@}@=@['a']@}",             7,   11,    x11,    0,      LEN  },
 { L_,  "@{@x11@=@['a']@}",                5,   11,    x11,    0,      LEN  },
 { L_,  "@{@\"x11\"@=@['a']@}",            5,   11,    x11,    0,      LEN  },
 { L_,  "@{@{@00@}@=@[1]@}",               7,   12,    x12,    0,      LEN  },
 { L_,  "@{@x12@=@[1]@}",                  5,   12,    x12,    0,      LEN  },
 { L_,  "@{@\"x12\"@=@[1]@}",              5,   12,    x12,    0,      LEN  },
 { L_,  "@{@{@00@}@=@[1]@}",               7,   13,    x13,    0,      LEN  },
 { L_,  "@{@x13@=@[1]@}",                  5,   13,    x13,    0,      LEN  },
 { L_,  "@{@\"x13\"@=@[1]@}",              5,   13,    x13,    0,      LEN  },
 { L_,  "@{@{@00@}@=@[1]@}",               7,   14,    x14,    0,      LEN  },
 { L_,  "@{@x14@=@[1]@}",                  5,   14,    x14,    0,      LEN  },
 { L_,  "@{@\"x14\"@=@[1]@}",              5,   14,    x14,    0,      LEN  },
 { L_,  "@{@{@00@}@=@[1]@}",               7,   15,    x15,    0,      LEN  },
 { L_,  "@{@x15@=@[1]@}",                  5,   15,    x15,    0,      LEN  },
 { L_,  "@{@\"x15\"@=@[1]@}",              5,   15,    x15,    0,      LEN  },
 { L_,  "@{@{@00@}@=@[1]@}",               7,   16,    x16,    0,      LEN  },
 { L_,  "@{@x16@=@[1]@}",                  5,   16,    x16,    0,      LEN  },
 { L_,  "@{@\"x16\"@=@[1]@}",              5,   16,    x16,    0,      LEN  },
 { L_,  "@{@{@00@}@=@[\"Hello!\"]@}",      7,   17,    x17,    0,      LEN  },
 { L_,  "@{@x17@=@[\"Hello!\"]@}",         5,   17,    x17,    0,      LEN  },
 { L_,  "@{@\"x17\"@=@[\"Hello!\"]@}",     5,   17,    x17,    0,      LEN  },
 { L_,  "@{@{@00@}@=@[3/3/3 3:03:03.003]@}",  7,  18,  x18,    0,      LEN  },
 { L_,  "@{@x18@=@[3/3/3 3:03:03.003]@}",  5,  18,  x18,    0,      LEN  },
 { L_,  "@{@\"x18\"@=@[3/3/3 3:03:03.003]@}", 5,  18,  x18,    0,      LEN  },
 { L_,  "@{@{@00@}@=@[3/3/3]@}",           7,   19,    x19,    0,      LEN  },
 { L_,  "@{@x19@=@[3/3/3]@}",              5,   19,    x19,    0,      LEN  },
 { L_,  "@{@\"x19\"@=@[3/3/3]@}",          5,   19,    x19,    0,      LEN  },
 { L_,  "@{@{@00@}@=@[3:03:03.003]@}",     7,   20,    x20,    0,      LEN  },
 { L_,  "@{@x20@=@[3:03:03.003]@}",        5,   20,    x20,    0,      LEN  },
 { L_,  "@{@\"x20\"@=@[3:03:03.003]@}",    5,   20,    x20,    0,      LEN  },
 { L_,  "@{@{@00@}@=@{CHAR \'a\'}@}",      7,   21,    x21,    0,      LEN  },
 { L_,  "@{@x21@=@{CHAR \'a\'}@}",       5,   21,    x21,    0,      LEN  },
 { L_,  "@{@\"x21\"@=@{CHAR \'a\'}@}",     5,   21,    x21,    0,      LEN  },
 { L_,  "@{@{@00@}@=@{[CHAR]{\'a\'}}@}",   7,   22,    x22,    0,      LEN  },
 { L_,  "@{@x22@ = @{[CHAR]{\'a\'}}@}",    5,   22,    x22,    0,      LEN  },
 { L_,  "@{@\"x22\"@=@{[CHAR]{\'a\'}}@}",  5,   22,    x22,    0,      LEN  },
 { L_,  "@{@{@00@}@="
            "@{@{@0@}@=@\'a\'@"
              "@{@1@}@=@1@"
              "@{@2@}@=@Hello! "
            "@}"
        "@ }",                            24,   23,    x23,    0,      LEN  },
 { L_,  "@{@x23@="
            "@{@{@0@}@=@\'a\'@"
              "@{@1@}@=@1@"
              "@{@2@}@=@\"Hello!\""
            "@}"
        "@}",                             22,   23,    x23,    0,      LEN  },
 { L_,  "@{@\"x23\"@="
            "@{@{@0@}@=@\'a\'@ "
              "@{@1@}@=@1@ "
              "@{@2@}@=@\"Hello!\""
            "@}"
        "@}",                             22,   23,    x23,    0,      LEN  },
 { L_,  "@{@{@00@}@="
            "@{"
              "@{"
                "@{@0@}@=@\'a\'@"
                "@{@1@}@=@1@"
                "@{@2@}@=@\"Hello!\""
              "@}"
              "@{"
                "@{@0@}@=@\'a\'@"
                "@{@1@}@=@1@"
                "@{@2@}@=@\"Hello!\""
              "@}"
            "@}"
        "@}",                             46,   24,    x24,    0,      LEN  },
 { L_,  "@{@{@00@}@="
            "@{@{@0@}@=@NULL@"
              "@{@1@}@=@1@"
              "@{@2@}@=@NULL"
            "@}"
        "@}",                             24,   23,    x25,    0,      LEN  },
 { L_,  "@{@{@00@}@="
            "@{"
              "@{@1@}@=@1@"
            "@}"
        "@}",                             14,   23,    x25,    0,      LEN  },
 { L_,  "@{@{@00@}@="
            "@{"
              "@{"
                "@{@0@}@=@NULL@"
                "@{@1@}@=@1@"
                "@{@2@}@=@NULL"
              "@}"
              "@{"
                "@{@1@}@=@1"
              "@}"
            "@}"
        "@}",                             34,   24,    x26,    0,      LEN  },
                // Testing input with multiple constraints
 { L_,  "@{@{@0@}@=@\'a\'@"
          "@{@1@}@=@1@"
          "@{@2@}@=@\"Hello!\"@}",        19,    0,    x23h,   0,      LEN  },
 { L_,  "@{@{@2@}@=@\"Hello!\"@"
          "@{@1@}@=@1@"
          "@{@0@}@=@\'a\'@}",             19,    0,    x23h,   0,      LEN  },
 { L_,  "@{@\"0\"@=@\'a\'@"
          "@\"1\"@=@1@"
          "@\"2\"@=@\"Hello!\"@}",        13,    0,    x23h,   0,      LEN  },

                // Testing input with multiple constraints with NULL
 { L_,  "@{@{@0@}@=@NULL@"
          "@{@1@}@=@1@"
          "@{@2@}@=@NULL@}",              19,    0,    x25h,   0,      LEN  },
 { L_,  "@{@{@1@}@=@1@}",                  7,    0,    x25h,   0,      LEN  },
 { L_,  "@{@\"0\"@=@NULL@"
          "@\"1\"@=@1@}",                  9,    0,    x25h,   0,      LEN  },

                // Testing input with multiple definition of a field
 { L_,  "@{@\"0\"@=@\'a\'@"
          "@{@1@}@=@1@"
          "@{@0@}@=@NULL@}",              17,    0,    x25h,   0,      LEN  },


                // Testing with invalid input

                // Opening curly brace is missing
 { L_,  "@\"0\"@=@NULL@"
        "@{@1@}@=@1@"
        "@{@2@}@=@NULL@}",                 1,    0,    stub,  FAILURE,  0    },

                // Closing curly brace is missing
 { L_,  "@{@\"0\"@=@\'a\'@"
            "@{@1@}@=@1@"
            "@{@2@}@=@NULL",              16,    0,    stub,  FAILURE,  LEN  },

                // Equal sign is missing
 { L_,  "@{@{@0@}"
            "@{@{@0@}@=@NULL@"
              "@{@1@}@=@1@"
              "@{@2@}@=@NULL"
            "@}"
        "@}",                              5,   23,    x25, FAILURE,    4    },

 { L_,  "@{@{@00@}@="
            "@{@{@0@}@N",                 10,   23,    x25, FAILURE, LENSUB1 },

               // Identifier is missing
 { L_,  "@{@{@}",                          3,   23,    x25, FAILURE, LENSUB1 },
            };
 //-------------V
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            std::vector<char> buffer;
            for (int i = 0; i < NUM_DATA; ++i) {
                // Orthogonal perturbation for optional whitespace
                const DataParsing<T>& DATA_I = DATA[i];
                const int LINE = DATA_I.d_lineNum;

                for (int j = 0; j < NUM_BLANKS; ++j) {
                    buffer.clear();
                    g(&buffer, DATA_I.d_str, j);
                    buffer.push_back('\0');

                    // Initialize 'result'
                    T result = initValue;

                    // Verify that expected value is different from initial
                    // value for valid inputs
                    if (0 == DATA_I.d_retVal) {
                        LOOP_ASSERT(LINE, DATA_I.d_result != initValue);
                    }

                    const char *endPos;
                    int res = bdempu_SchemaAggregate::parseConstrainedList(
                        &endPos, &result, &buffer.front(),
                        s.record(DATA_I.d_constraint));
                    LOOP_ASSERT(LINE, DATA_I.d_retVal == res);
                    if (0 == DATA_I.d_retVal) {
                        LOOP_ASSERT(LINE, DATA_I.d_result == result);
                    }

                    // Verify correctness of the ending position offset
                    int expectedOffset;
                    if (LEN == DATA_I.d_endPos) {
                        expectedOffset = strlen(&buffer.front());
                    }
                    else if (LENSUB1 == DATA_I.d_endPos) {
                        expectedOffset = strlen(&buffer.front()) - 1;
                    }
                    else {
                        expectedOffset = DATA_I.d_endPos +
                                         DATA_I.d_blanks * strlen(BLANKS[j]);
                    }
                    int actualOffset = endPos - &buffer.front();
                    LOOP_ASSERT(LINE, expectedOffset == actualOffset);

                    if (expectedOffset != actualOffset) {
                        cout << "\t\texpected: " << expectedOffset
                             << ", " << "parsed: "
                             << actualOffset << endl;

                        cout << &buffer.front() << endl;
                    }
                }
            }
        }

        if (verbose) cout << "\tTesting generator" << endl;
        {
            const DataGenerating<T> DATA[] = {
 //-------------^

 //line prefix   level sp d_val constr                 d_str
 //---- -------  ----- -- ----- ------ --------------------------------------
 { L_,  "",        1,  2,  x0,    0,   "  NULL"                              },
 { L_,  " z = ",  -1,  1,  x0,    0,   " z = NULL"                           },
 { L_,  "",        1,  2,  x1,    1,   "  {\n"
                                       "    x1 = \'a\'\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x1,    1,   " z = {\n"
                                       "  x1 = \'a\'\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x2,    2,   "  {\n"
                                       "    x2 = 1\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x2,    2,   " z = {\n"
                                       "  x2 = 1\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x3,    3,   "  {\n"
                                       "    x3 = 1\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x3,    3,   " z = {\n"
                                       "  x3 = 1\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x4,    4,   "  {\n"
                                       "    x4 = 1\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x4,    4,   " z = {\n"
                                       "  x4 = 1\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x5,    5,   "  {\n"
                                       "    x5 = 1\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x5,    5,   " z = {\n"
                                       "  x5 = 1\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x6,    6,   "  {\n"
                                       "    x6 = 1\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x6,    6,   " z = {\n"
                                       "  x6 = 1\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x7,    7,   "  {\n"
                                       "    x7 = Hello!\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x7,    7,   " z = {\n"
                                       "  x7 = Hello!\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x8,    8,   "  {\n"
                                       "    x8 = 0003/03/03 03:03:03.003\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x8,    8,   " z = {\n"
                                       "  x8 = 0003/03/03 03:03:03.003\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x9,    9,   "  {\n"
                                       "    x9 = 0003/03/03\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x9,    9,   " z = {\n"
                                       "  x9 = 0003/03/03\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x10,  10,   "  {\n"
                                       "    x10 = 03:03:03.003\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x10,  10,   " z = {\n"
                                       "  x10 = 03:03:03.003\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x11,  11,   "  {\n"
                                       "    x11 = [\n"
                                       "      \'a\'\n"
                                       "    ]\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x11,  11,   " z = {\n"
                                       "  x11 = [\n"
                                       "   \'a\'\n"
                                       "  ]\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x12,  12,   "  {\n"
                                       "    x12 = [\n"
                                       "      1\n"
                                       "    ]\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x12,  12,   " z = {\n"
                                       "  x12 = [\n"
                                       "   1\n"
                                       "  ]\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x13,  13,   "  {\n"
                                       "    x13 = [\n"
                                       "      1\n"
                                       "    ]\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x13,  13,   " z = {\n"
                                       "  x13 = [\n"
                                       "   1\n"
                                       "  ]\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x14,  14,   "  {\n"
                                       "    x14 = [\n"
                                       "      1\n"
                                       "    ]\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x14,  14,   " z = {\n"
                                       "  x14 = [\n"
                                       "   1\n"
                                       "  ]\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x15,  15,   "  {\n"
                                       "    x15 = [\n"
                                       "      1\n"
                                       "    ]\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x15,  15,   " z = {\n"
                                       "  x15 = [\n"
                                       "   1\n"
                                       "  ]\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x16,  16,   "  {\n"
                                       "    x16 = [\n"
                                       "      1\n"
                                       "    ]\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x16,  16,   " z = {\n"
                                       "  x16 = [\n"
                                       "   1\n"
                                       "  ]\n"
                                       " }",                                 },
 { L_,  "",        1,  2,  x17,  17,   "  {\n"
                                       "    x17 = [\n"
                                       "      Hello!\n"
                                       "    ]\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x17,  17,   " z = {\n"
                                       "  x17 = [\n"
                                       "   Hello!\n"
                                       "  ]\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x18,  18,   "  {\n"
                                       "    x18 = [\n"
                                       "      0003/03/03 03:03:03.003\n"
                                       "    ]\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x18,  18,   " z = {\n"
                                       "  x18 = [\n"
                                       "   0003/03/03 03:03:03.003\n"
                                       "  ]\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x19,  19,   "  {\n"
                                       "    x19 = [\n"
                                       "      0003/03/03\n"
                                       "    ]\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x19,  19,   " z = {\n"
                                       "  x19 = [\n"
                                       "   0003/03/03\n"
                                       "  ]\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x20,  20,   "  {\n"
                                       "    x20 = [\n"
                                       "      03:03:03.003\n"
                                       "    ]\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x20,  20,   " z = {\n"
                                       "  x20 = [\n"
                                       "   03:03:03.003\n"
                                       "  ]\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x21,  21,   "  {\n"
                                       "    x21 = {\n"
                                       "      CHAR 'a'\n"
                                       "    }\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x21,  21,   " z = {\n"
                                       "  x21 = {\n"
                                       "   CHAR 'a'\n"
                                       "  }\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x22,  22,   "  {\n"
                                       "    x22 = {\n"
                                       "      [\n"
                                       "        CHAR\n"
                                       "      ]\n"
                                       "      {\n"
                                       "        \'a\'\n"
                                       "      }\n"
                                       "    }\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x22,  22,   " z = {\n"
                                       "  x22 = {\n"
                                       "   [\n"
                                       "    CHAR\n"
                                       "   ]\n"
                                       "   {\n"
                                       "    \'a\'\n"
                                       "   }\n"
                                       "  }\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x23,  23,   "  {\n"
                                       "    x23 = {\n"
                                       "      \"0\" = \'a\'\n"
                                       "      \"1\" = 1\n"
                                       "      \"2\" = Hello!\n"
                                       "    }\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x23,  23,   " z = {\n"
                                       "  x23 = {\n"
                                       "   \"0\" = \'a\'\n"
                                       "   \"1\" = 1\n"
                                       "   \"2\" = Hello!\n"
                                       "  }\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x24,  24,   "  {\n"
                                       "    x24 = {\n"
                                       "      {\n"
                                       "        \"0\" = \'a\'\n"
                                       "        \"1\" = 1\n"
                                       "        \"2\" = Hello!\n"
                                       "      }\n"
                                       "      {\n"
                                       "        \"0\" = \'a\'\n"
                                       "        \"1\" = 1\n"
                                       "        \"2\" = Hello!\n"
                                       "      }\n"
                                       "    }\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x24,  24,   " z = {\n"
                                       "  x24 = {\n"
                                       "   {\n"
                                       "    \"0\" = \'a\'\n"
                                       "    \"1\" = 1\n"
                                       "    \"2\" = Hello!\n"
                                       "   }\n"
                                       "   {\n"
                                       "    \"0\" = \'a\'\n"
                                       "    \"1\" = 1\n"
                                       "    \"2\" = Hello!\n"
                                       "   }\n"
                                       "  }\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x25,  23,   "  {\n"
                                       "    x23 = {\n"
                                       "      \"1\" = 1\n"
                                       "    }\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x25,  23,   " z = {\n"
                                       "  x23 = {\n"
                                       "   \"1\" = 1\n"
                                       "  }\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x26,  24,   "  {\n"
                                       "    x24 = {\n"
                                       "      {\n"
                                       "        \"1\" = 1\n"
                                       "      }\n"
                                       "      {\n"
                                       "        \"1\" = 1\n"
                                       "      }\n"
                                       "    }\n"

                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x26,  24,   " z = {\n"
                                       "  x24 = {\n"
                                       "   {\n"
                                       "    \"1\" = 1\n"
                                       "   }\n"
                                       "   {\n"
                                       "    \"1\" = 1\n"
                                       "   }\n"
                                       "  }\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x23h,  0,   "  {\n"
                                       "    \"0\" = \'a\'\n"
                                       "    \"1\" = 1\n"
                                       "    \"2\" = Hello!\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x23h,  0,   " z = {\n"
                                       "  \"0\" = \'a\'\n"
                                       "  \"1\" = 1\n"
                                       "  \"2\" = Hello!\n"
                                       " }",                                 },

 { L_,  "",        1,  2,  x25h,  0,   "  {\n"
                                       "    \"1\" = 1\n"
                                       "  }",                                },
 { L_,  " z = ",  -1,  1,  x25h,  0,   " z = {\n"
                                       "  \"1\" = 1\n"
                                       " }",                                 },
            };
 //-------------V

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            std::vector<char> buffer;
            for (int i = 0; i < NUM_DATA; ++i) {
                const DataGenerating<T>& DATA_I = DATA[i];
                const int LINE = DATA_I.d_lineNum;
                buffer.clear();

                appendToBuffer(&buffer, DATA_I.d_prefix);
                bdempu_SchemaAggregate::generateConstrainedList(
                    &buffer, DATA_I.d_val, s.record(DATA_I.d_constr),
                    DATA_I.d_level, DATA_I.d_spaces);

                buffer.push_back('\0');
                int diff = strcmp(&buffer.front(), DATA_I.d_str);
                LOOP_ASSERT(LINE, !diff);
                if (diff) {
                    cout << "\t\texpected:"  << DATA_I.d_str    << endl;
                    cout << "\t\tgenerated:" << &buffer.front() << endl;
cout << "Diff is " << diff << endl;
for( int i = 0; i < (int)buffer.size(); ++i)
{
  cout << "DATA_I.d_str[" << i << "] is: " <<
    ((DATA_I.d_str[i] == ' ') ? '@' :
        ((DATA_I.d_str[i] == '\n') ? 'N' : DATA_I.d_str[i])) << endl;
  cout << "buffer[" << i << "] is: " <<
    ((buffer[i] == ' ') ? '@' :
         ((buffer[i] == '\n') ? 'N' : buffer[i])) << endl;
  if( DATA_I.d_str[i] != buffer[i] ) {
    cout << "The two strings differ at position " << i << endl;
    if(buffer[i] == ' ') cout << "buffer holds a Space" << endl;
    if(buffer[i] == '\t') cout << "buffer holds a Tab" << endl;
    if(buffer[i] == '\n') cout << "buffer holds a Newline" << endl;
    else cout << "buffer holds " << (unsigned int)buffer[i] << endl;
  }
}
                }
                LOOP_ASSERT(LINE, buffer[buffer.size() - 1] == '\0');

                // Verify that the we can get the original value by parsing the
                // generated string.
                T result = initValue;
                LOOP_ASSERT(LINE, DATA_I.d_val != result);
                const char *endPos;
                int ret = bdempu_SchemaAggregate::parseConstrainedList(
                    &endPos, &result,
                    &buffer.front() + strlen(DATA_I.d_prefix),
                    s.record(DATA_I.d_constr));
                LOOP_ASSERT(LINE, 0 == ret);
                LOOP_ASSERT(LINE, DATA_I.d_val == result);
                LOOP_ASSERT(LINE, '\0' == *endPos);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS USED IN TESTING
        //   Concerns:
        //     Test that these methods generate output according to their
        //     specifiecation with every type of input.
        //
        // Plan:
        //   To test 'g' create a table containing the input string with '@'
        //   inserted at the beginning, middle, and at the end and the
        //   expected output sting for every index in 'BLANKS'.  Verify that
        //   thearesult generated by 'g' is identical to the expected for every
        //   string in 'BLANKS'.
        //
        // Testing:
        //    g(std::vector<char> *buffer, const char *inStr, int index)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'g'" << endl
                                  << "===========" << endl;
        {
            const struct TestData {
                int         d_line;
                const char *d_generated;
                const char *d_input;
                int         d_index;
            } DATA[] = {
                //d_line    d_generated             d_input    d_index
                //------    -----------             -------    -------

                {  L_,       "abcd",                "abcd",        0       },
                {  L_,       "abcd",                "@ab@cd@",     0       },
                {  L_,       " // C++ blank \n"
                             "ab"
                             " // C++ blank \n"
                             "cd"
                             " // C++ blank \n",     "@ab@cd@",     1      },
                {  L_,       " /*C blanks*/"
                             "ab"
                             " /*C blanks*/"
                             "cd"
                             " /*C blanks*/",        "@ab@cd@",     2      },
                {  L_,       "  \n\n\t/* //*/"
                             "ab"
                             "  \n\n\t/* //*/"
                             "cd"
                             "  \n\n\t/* //*/",     "@ab@cd@",     3       },
                {  L_,       "  ///*  \t\n "
                             "ab"
                             "  ///*  \t\n "
                             "cd"
                             "  ///*  \t\n ",       "@ab@cd@",     4       },
                {  L_,       " "
                             "ab"
                             " "
                             "cd"
                             " ",                   "@ab@cd@",     5       },
                {  L_,       "  "
                             "ab"
                             "  "
                             "cd"
                             "  ",                  "@ab@cd@",     6       },
                {  L_,       " \t"
                             "ab"
                             " \t"
                             "cd"
                             " \t",                 "@ab@cd@",     7       },
                {  L_,       " \n"
                             "ab"
                             " \n"
                             "cd"
                             " \n",                 "@ab@cd@",     8       },
                {  L_,       " \t\n"
                             "ab"
                             " \t\n"
                             "cd"
                             " \t\n",               "@ab@cd@",     9       },
                {  L_,       " \n\t"
                             "ab"
                             " \n\t"
                             "cd"
                             " \n\t",               "@ab@cd@",     10      },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            std::vector<char> outputArray;
            for (int i = 0; i < NUM_DATA; ++i) {
                outputArray.clear();
                g(&outputArray, DATA[i].d_input, DATA[i].d_index);
                outputArray.push_back('\0');
                int diff = strcmp(DATA[i].d_generated, &outputArray.front());
                if (veryVerbose) {
                    cout << "EXPECTED:"          << endl;
                    cout << DATA[i].d_generated  << endl;
                    cout << "ACTUAL:"            << endl;
                    cout << &outputArray.front() << endl;
                }
                LOOP_ASSERT(DATA[i].d_line, 0 == diff);
            }
        }
      }  break;
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
