// bdempu_aggregate.t.cpp              -*-C++-*-

#include <bdempu_aggregate.h>
#include <bdempu_elemtype.h>

#include <bdepu_parserimputil.h>
#include <bdepu_typesparser.h>

#include <bdem_aggregate.h>
#include <bdem_elemtype.h>

#include <bdetu_unset.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


enum { SUCCESS = 0, FAILURE = 1 };

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// We use the following abbreviations:
//  'cchar'      for 'const char'
//  'b_ca'       for 'bsl::vector<char>'
//
//-----------------------------------------------------------------------------
// [ 2] int parseList(cchar **endPos, bdem_List *result, cchar  *inputString);
// [ 3] int parseTable(cchar **endPos, bdem_Table  *result, cchar  *inputStr);
// [ 2] void generateList( b_ca *buf, const bdem_List& val, int l, int sp);
// [ 3] void generateTable(b_ca *buf, const bdem_Table& val, int l, int sp);
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
#define T_()  cout << '\t' << flush;          // Print a tab character

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

    typedef bdepu_ParserImpUtil ImpUtil;

    const char *BLANKS[] = {
        // This structure is used in "orthogonal perturbation" to test each
        // input string with every type of the <WHITESPACE>.
        "",
        "// C++ blank \n",
        "/*C blanks*/",
        "  \n\n\t/* //*/",
        "  ///*  \t\n ",
        " ",
        "  ",
        "\t",
        "\n",
        " \t",
        " \n",
        "\t\n         ",
        "\n\t           "
    };
    const int NUM_BLANKS = sizeof BLANKS / sizeof *BLANKS;

    template <class T>
    struct DataParsing {
        // Facilitate testing of parsing functions

        int         d_lineNum; // source line number
        const char *d_str;     // Input String
        int         d_blanks;  // Number of inserted 'BLANKS' before 'd_endPos'
                               // is encountered. (*)
        T           d_result;  // expected resulting value
        int         d_retVal;  // expected return value
        int         d_endPos;  // expected offset from the start position
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
        int         d_level;   // indentation level
        int         d_spaces;  // spaces per level
        T           d_val;     // input value used to generate a string
                               // representation
        const char *d_str;     // output String (combination of 'd_prefix' and
                               // a string generated from 'd_val'
    };

//=============================================================================
//                         GENERATOR FUNCTION 'g'
//-----------------------------------------------------------------------------

    static void appendToBuffer(bsl::vector<char> *buffer, const char *str)
    {
        while (*str) {
            buffer->push_back(*str);
            ++str;
        }
    }

    void g(bsl::vector<char> *buffer, const char *inStr, int index)
        // Append the content of the specified 'inStr' to the specified
        // 'buffer' replacing every occurrence of the character '@' with the
        // array element of 'BLANKS' at the specified 'index'.
    {
        // 'BLANKS' is a global array of strings representing sample set of
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
        buffer->push_back('\0');
    }


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PARSE/GENERATE TABLE
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
        //       2. The result is null-terminated.
        //       3. The generated result can be parsed back with the
        //          corresponding parser function.
        //       4. The values that were in the buffer prior to the call are
        //          not changed.
        //
        // Plan:
        //   To test the parsing method, create a table containing the input
        //   string and the expected output value.  Use nested loop
        //   ("orthogonal perturbation") to test each input to the parsing
        //   method with different <WHITESPACE> strings defined in global
        //   'BLANKS' structure.  In every iteration one of the <WHITESPACE>
        //   strings is inserted in the beginning of the input string.
        //
        //   Test with the following valid tables:
        //       1. Empty table
        //
        //       2. Table containing one element of each type
        //
        //       3. With table containing multiple elements
        //
        //       4. With table containing "NULL" element of each type
        //
        //    Test with the following invalid lists:
        //       1. Opening/closing curly brace is missing
        //       2. Element or element type is incorrect
        //
        //   To test generating method create a table containing the input
        //   object and the expected textual representation.  Test that the
        //   generate method correctly generates indentation with positive
        //   and negative levels.
        //
        // Testing:
        //    parseTable(cchar **endPos, Table *result, cchar *input);
        //    generateTable(b_ca *buff, cTable& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PARSE/GENERATE TABLE" << endl
                          << "============================" << endl;

        typedef bdem_Table T;

        typedef bsls_PlatformUtil::Int64 Int64;

        const char X_CHAR = 'a';
        const short X_SHORT = 1;
        const int X_INT = 1;
        const Int64 X_INT64 = 1;
        const float X_FLOAT = 1;
        const double X_DOUBLE = 1;
        const bsl::string X_STRING = "Hello!";
        const bdet_Date X_DATE(3, 3, 3);
        const bdet_Time X_TIME(3, 3, 3, 3);
        const bdet_Datetime X_DATETIME(X_DATE, X_TIME);
        const bsl::vector<char> X_A1(1, X_CHAR);

        bsl::vector<bdem_ElemType::Type> columnTypeArray;

        T stub;     // Identifies undefined value
        T initValue; // Used to initialize result
        columnTypeArray.push_back(bdem_ElemType::BDEM_STRING);
        initValue.reset(&columnTypeArray.front(), columnTypeArray.size());
        bdem_List initList;
        initList.appendString("init");
        initValue.appendRow(initList);

        T x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15;
        bdem_List xList, xCharList;

        xCharList.appendChar('a');

        xList.appendChar('a');
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_CHAR);
        x1.reset(&columnTypeArray.front(), columnTypeArray.size());
        x1.appendRow(xList);

        xList.removeAll();
        xList.appendShort(1);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_SHORT);
        x2.reset(&columnTypeArray.front(), columnTypeArray.size());
        x2.appendRow(xList);

        xList.removeAll();
        xList.appendInt(1);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_INT);
        x3.reset(&columnTypeArray.front(), columnTypeArray.size());
        x3.appendRow(xList);

        xList.removeAll();
        xList.appendInt64(1);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_INT64);
        x4.reset(&columnTypeArray.front(), columnTypeArray.size());
        x4.appendRow(xList);

        xList.removeAll();
        xList.appendFloat(1);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_FLOAT);
        x5.reset(&columnTypeArray.front(), columnTypeArray.size());
        x5.appendRow(xList);

        xList.removeAll();
        xList.appendDouble(1);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_DOUBLE);
        x6.reset(&columnTypeArray.front(), columnTypeArray.size());
        x6.appendRow(xList);

        xList.removeAll();
        xList.appendString("Hello!");
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_STRING);
        x7.reset(&columnTypeArray.front(), columnTypeArray.size());
        x7.appendRow(xList);

        xList.removeAll();
        xList.appendDatetime(X_DATETIME);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_DATETIME);
        x8.reset(&columnTypeArray.front(), columnTypeArray.size());
        x8.appendRow(xList);

        xList.removeAll();
        xList.appendDate(X_DATE);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_DATE);
        x9.reset(&columnTypeArray.front(), columnTypeArray.size());
        x9.appendRow(xList);

        xList.removeAll();
        xList.appendTime(X_TIME);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_TIME);
        x10.reset(&columnTypeArray.front(), columnTypeArray.size());
        x10.appendRow(xList);

        xList.removeAll();
        xList.appendList(xCharList);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_LIST);
        x11.reset(&columnTypeArray.front(), columnTypeArray.size());
        x11.appendRow(xList);

        T xCharTable;
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_CHAR);
        xCharTable.reset(&columnTypeArray.front(), columnTypeArray.size());
        xCharTable.appendRow(xCharList);
        xList.removeAll();
        xList.appendTable(xCharTable);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_TABLE);
        x12.reset(&columnTypeArray.front(), columnTypeArray.size());
        x12.appendRow(xList);

        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_CHAR);
        columnTypeArray.push_back(bdem_ElemType::BDEM_INT);
        columnTypeArray.push_back(bdem_ElemType::BDEM_STRING);
        xList.removeAll();
        xList.appendChar(X_CHAR);
        xList.appendInt(X_INT);
        xList.appendString(X_STRING);
        x13.reset(&columnTypeArray.front(), columnTypeArray.size());
        x13.appendRow(xList);

        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_CHAR);
        xList.removeAll();
        xList.appendChar(X_CHAR);
        x14.reset(&columnTypeArray.front(), columnTypeArray.size());
        x14.appendRow(xList);
        x14.appendRow(xList);
        x14.appendRow(xList);

        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_CHAR);
        columnTypeArray.push_back(bdem_ElemType::BDEM_INT);
        columnTypeArray.push_back(bdem_ElemType::BDEM_STRING);
        xList.removeAll();
        xList.appendChar(X_CHAR);
        xList.appendInt(X_INT);
        xList.appendString(X_STRING);
        x15.reset(&columnTypeArray.front(), columnTypeArray.size());
        x15.appendRow(xList);
        x15.appendRow(xList);
        x15.appendRow(xList);


        // Test date to check table with "NULL" element.
        T z1, z2, z3, z4, z5, z6, z7, z8, z9, z10, z11, z12;
        bdem_List zList;

        zList.removeAll();
        zList.appendChar(bdetu_Unset<char>::unsetValue());
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_CHAR);
        z1.reset(&columnTypeArray.front(), columnTypeArray.size());
        z1.appendRow(zList);

        zList.removeAll();
        zList.appendShort(bdetu_Unset<short>::unsetValue());
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_SHORT);
        z2.reset(&columnTypeArray.front(), columnTypeArray.size());
        z2.appendRow(zList);

        zList.removeAll();
        zList.appendInt(bdetu_Unset<int>::unsetValue());
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_INT);
        z3.reset(&columnTypeArray.front(), columnTypeArray.size());
        z3.appendRow(zList);

        zList.removeAll();
        zList.appendInt64(bdetu_Unset<bsls_PlatformUtil::Int64>::unsetValue());
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_INT64);
        z4.reset(&columnTypeArray.front(), columnTypeArray.size());
        z4.appendRow(zList);

        zList.removeAll();
        zList.appendFloat(bdetu_Unset<float>::unsetValue());
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_FLOAT);
        z5.reset(&columnTypeArray.front(), columnTypeArray.size());
        z5.appendRow(zList);

        zList.removeAll();
        zList.appendDouble(bdetu_Unset<double>::unsetValue());
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_DOUBLE);
        z6.reset(&columnTypeArray.front(), columnTypeArray.size());
        z6.appendRow(zList);

        zList.removeAll();
        zList.appendString(bdetu_Unset<bsl::string>::unsetValue());
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_STRING);
        z7.reset(&columnTypeArray.front(), columnTypeArray.size());
        z7.appendRow(zList);

        zList.removeAll();
        zList.appendDatetime(bdetu_Unset<bdet_Datetime>::unsetValue());
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_DATETIME);
        z8.reset(&columnTypeArray.front(), columnTypeArray.size());
        z8.appendRow(zList);

        zList.removeAll();
        zList.appendDate(bdetu_Unset<bdet_Date>::unsetValue());
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_DATE);
        z9.reset(&columnTypeArray.front(), columnTypeArray.size());
        z9.appendRow(zList);

        zList.removeAll();
        zList.appendTime(bdetu_Unset<bdet_Time>::unsetValue());
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_TIME);
        z10.reset(&columnTypeArray.front(), columnTypeArray.size());
        z10.appendRow(zList);

        bdem_List zeroList;
        zList.removeAll();
        zList.appendList(zeroList);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_LIST);
        z11.reset(&columnTypeArray.front(), columnTypeArray.size());
        z11.appendRow(zList);

        T zeroTable;
        zList.removeAll();
        zList.appendTable(zeroTable);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_TABLE);
        z12.reset(&columnTypeArray.front(), columnTypeArray.size());
        z12.appendRow(zList);

        T y1, y2, y3, y4, y5, y6, y7, y8, y9, y10;
        const bsl::vector<char> A1(1, X_CHAR);
        xList.removeAll();
        xList.appendCharArray(A1);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_CHAR_ARRAY);
        y1.reset(&columnTypeArray.front(), columnTypeArray.size());
        y1.appendRow(xList);

        xList.removeAll();
        const bsl::vector<short> A2(1, X_SHORT);
        xList.removeAll();
        xList.appendShortArray(A2);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_SHORT_ARRAY);
        y2.reset(&columnTypeArray.front(), columnTypeArray.size());
        y2.appendRow(xList);

        xList.removeAll();
        const bsl::vector<int> A3(1, X_INT);
        xList.removeAll();
        xList.appendIntArray(A3);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_INT_ARRAY);
        y3.reset(&columnTypeArray.front(), columnTypeArray.size());
        y3.appendRow(xList);

        xList.removeAll();
        const bsl::vector<Int64> A4(1, X_INT64);
        xList.removeAll();
        xList.appendInt64Array(A4);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_INT64_ARRAY);
        y4.reset(&columnTypeArray.front(), columnTypeArray.size());
        y4.appendRow(xList);

        xList.removeAll();
        const bsl::vector<float> A5(1, X_FLOAT);
        xList.removeAll();
        xList.appendFloatArray(A5);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_FLOAT_ARRAY);
        y5.reset(&columnTypeArray.front(), columnTypeArray.size());
        y5.appendRow(xList);

        xList.removeAll();
        const bsl::vector<double> A6(1, X_DOUBLE);
        xList.removeAll();
        xList.appendDoubleArray(A6);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_DOUBLE_ARRAY);
        y6.reset(&columnTypeArray.front(), columnTypeArray.size());
        y6.appendRow(xList);

        xList.removeAll();
        const bsl::vector<bsl::string> A7(1, X_STRING);
        xList.removeAll();
        xList.appendStringArray(A7);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_STRING_ARRAY);
        y7.reset(&columnTypeArray.front(), columnTypeArray.size());
        y7.appendRow(xList);

        xList.removeAll();
        const bsl::vector<bdet_Datetime> A8(1, X_DATETIME);
        xList.removeAll();
        xList.appendDatetimeArray(A8);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_DATETIME_ARRAY);
        y8.reset(&columnTypeArray.front(), columnTypeArray.size());
        y8.appendRow(xList);

        xList.removeAll();
        const bsl::vector<bdet_Date> A9(1, X_DATE);
        xList.removeAll();
        xList.appendDateArray(A9);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_DATE_ARRAY);
        y9.reset(&columnTypeArray.front(), columnTypeArray.size());
        y9.appendRow(xList);

        xList.removeAll();
        const bsl::vector<bdet_Time> A10(1, X_TIME);
        xList.appendTimeArray(A10);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_TIME_ARRAY);
        y10.reset(&columnTypeArray.front(), columnTypeArray.size());
        y10.appendRow(xList);

        T u1, u2, u3, u4, u5, u6, u7, u8, u9, u10;
        const bsl::vector<char> B1;
        xList.removeAll();
        xList.appendCharArray(B1);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_CHAR_ARRAY);
        u1.reset(&columnTypeArray.front(), columnTypeArray.size());
        u1.appendRow(xList);

        const bsl::vector<short> B2;
        xList.removeAll();
        xList.appendShortArray(B2);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_SHORT_ARRAY);
        u2.reset(&columnTypeArray.front(), columnTypeArray.size());
        u2.appendRow(xList);

        xList.removeAll();
        const bsl::vector<int> B3;
        xList.removeAll();
        xList.appendIntArray(B3);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_INT_ARRAY);
        u3.reset(&columnTypeArray.front(), columnTypeArray.size());
        u3.appendRow(xList);

        xList.removeAll();
        const bsl::vector<Int64> B4;
        xList.removeAll();
        xList.appendInt64Array(B4);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_INT64_ARRAY);
        u4.reset(&columnTypeArray.front(), columnTypeArray.size());
        u4.appendRow(xList);

        xList.removeAll();
        const bsl::vector<float> B5;
        xList.removeAll();
        xList.appendFloatArray(B5);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_FLOAT_ARRAY);
        u5.reset(&columnTypeArray.front(), columnTypeArray.size());
        u5.appendRow(xList);

        xList.removeAll();
        const bsl::vector<double> B6;
        xList.removeAll();
        xList.appendDoubleArray(B6);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_DOUBLE_ARRAY);
        u6.reset(&columnTypeArray.front(), columnTypeArray.size());
        u6.appendRow(xList);

        xList.removeAll();
        const bsl::vector<bsl::string> B7;
        xList.removeAll();
        xList.appendStringArray(B7);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_STRING_ARRAY);
        u7.reset(&columnTypeArray.front(), columnTypeArray.size());
        u7.appendRow(xList);

        xList.removeAll();
        const bsl::vector<bdet_Datetime> B8;
        xList.removeAll();
        xList.appendDatetimeArray(B8);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_DATETIME_ARRAY);
        u8.reset(&columnTypeArray.front(), columnTypeArray.size());
        u8.appendRow(xList);

        xList.removeAll();
        const bsl::vector<bdet_Date> B9;
        xList.removeAll();
        xList.appendDateArray(B9);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_DATE_ARRAY);
        u9.reset(&columnTypeArray.front(), columnTypeArray.size());
        u9.appendRow(xList);

        xList.removeAll();
        const bsl::vector<bdet_Time> B10;
        xList.appendTimeArray(B10);
        columnTypeArray.clear();
        columnTypeArray.push_back(bdem_ElemType::BDEM_TIME_ARRAY);
        u10.reset(&columnTypeArray.front(), columnTypeArray.size());
        u10.appendRow(xList);

        if (verbose)  cout << "\tTesting parser" << endl;
        {
            const DataParsing<T> DATA[] = {
 //-------------^

 //line                    d_str                #@ result retVal  endPos
 //---- --------------------------------------- -- ------ ------  ------
 { L_,  "@{@[@]@}",                             4,   x0,    0,      LEN    },
 { L_,  "@{@[@CHAR@]@{@\'a\'@}@}",              8,   x1,    0,      LEN    },
 { L_,  "@{@[@SHORT@]@{@1@}@}",                 8,   x2,    0,      LEN    },
 { L_,  "@{@[@INT@]@{@1@}@}",                   8,   x3,    0,      LEN    },
 { L_,  "@{@[@INT64@]@{@1@}@}",                 8,   x4,    0,      LEN    },
 { L_,  "@{@[@FLOAT@]@{@1@}@}",                 8,   x5,    0,      LEN    },
 { L_,  "@{@[@DOUBLE@]@{@1@}@}",                8,   x6,    0,      LEN    },
 { L_,  "@{@[@STRING@]@{@\"Hello!\"@}@}",       8,   x7,    0,      LEN    },
 { L_,  "@{@[@DATETIME@]@{@3/3/3 3:03:03.003@}@}",
                                                8,   x8,    0,      LEN    },
 { L_,  "@{@[@DATE@]@{@3/3/3@}@}",              8,   x9,    0,      LEN    },
 { L_,  "@{@[@TIME@]@{@3:03:03.003@}@}",        8,   x10,   0,      LEN    },
 { L_,  "@{@[@LIST@]@{@{CHAR \'a\'}@}@}",       8,   x11,   0,      LEN    },
 { L_,  "@{@[@TABLE@]@{@{[CHAR]{\'a\'}}@}@}",   8,   x12,   0,      LEN    },
 { L_,  "@{@[@CHAR@INT@STRING@]@"
        "{@\'a\'@1@\"Hello!\"@}@}",            16,   x13,   0,      LEN    },
 { L_,  "@{@[@CHAR@]"
         "@{@\'a\'@}"
         "@{@\'a\'@}"
         "@{@\'a\'@}@}",                       14,   x14,   0,      LEN    },
 { L_,  "@{@[@CHAR @INT @STRING@]@"
        "{@\'a\' @1 @\"Hello!\"@}"
        "{@\'a\' @1 @\"Hello!\"@}"
        "{@\'a\' @1 @\"Hello!\"@}@}",        27,   x15,   0,      LEN    },
 { L_,  "@{@[@CHAR_ARRAY@]@{@[\'a\']@}@}",      8,   y1,    0,      LEN    },
 { L_,  "@{@[@SHORT_ARRAY@]@{@[1]@}@}",         8,   y2,    0,      LEN    },
 { L_,  "@{@[@INT_ARRAY@]@{@[1]@}@}",           8,   y3,    0,      LEN    },
 { L_,  "@{@[@INT64_ARRAY@]@{@[1]@}@}",         8,   y4,    0,      LEN    },
 { L_,  "@{@[@FLOAT_ARRAY@]@{@[1]@}@}",         8,   y5,    0,      LEN    },
 { L_,  "@{@[@DOUBLE_ARRAY@]@{@[1]@}@}",        8,   y6,    0,      LEN    },
 { L_,  "@{@[@STRING_ARRAY@]@"
        "{@[\"Hello!\"]@}@}",                   8,   y7,    0,      LEN    },
 { L_,  "@{@[@DATETIME_ARRAY@]@"
        "{@[3/3/3 3:03:03.003]@}@}",            8,   y8,    0,      LEN    },
 { L_,  "@{@[@DATE_ARRAY@]@{@[3/3/3]@}@}",      8,   y9,    0,      LEN    },
 { L_,  "@{@[@TIME_ARRAY@]@{@[3:03:03.003]@}@}",8,   y10,   0,      LEN    },

 { L_,  "@{@[@CHAR@]@{@NULL@}@}",               8,   z1,    0,      LEN    },
 { L_,  "@{@[@SHORT@]@{@NULL@}@}",              8,   z2,    0,      LEN    },
 { L_,  "@{@[@INT@]@{@NULL@}@}",                8,   z3,    0,      LEN    },
 { L_,  "@{@[@INT64@]@{@NULL@}@}",              8,   z4,    0,      LEN    },
 { L_,  "@{@[@FLOAT@]@{@NULL@}@}",              8,   z5,    0,      LEN    },
 { L_,  "@{@[@DOUBLE@]@{@NULL@}@}",             8,   z6,    0,      LEN    },
 { L_,  "@{@[@STRING@]@{@NULL@}@}",             8,   z7,    0,      LEN    },
 { L_,  "@{@[@DATETIME@]@{@NULL@}@}",           8,   z8,    0,      LEN    },
 { L_,  "@{@[@DATE@]@{@NULL@}@}",               8,   z9,    0,      LEN    },
 { L_,  "@{@[@TIME@]@{@NULL@}@}",               8,   z10,   0,      LEN    },
 { L_,  "@{@[@LIST@]@{@NULL@}@}",               8,   z11,   0,      LEN    },
 { L_,  "@{@[@TABLE@]@{@NULL@}@}",              8,   z12,   0,      LEN    },

 { L_,  "@{@[@CHAR_ARRAY@]@{@[]@}@}",           8,   u1,    0,      LEN    },
 { L_,  "@{@[@SHORT_ARRAY@]@{@[]@}@}",          8,   u2,    0,      LEN    },
 { L_,  "@{@[@INT_ARRAY@]@{@[]@}@}",            8,   u3,    0,      LEN    },
 { L_,  "@{@[@INT64_ARRAY@]@{@[]@}@}",          8,   u4,    0,      LEN    },
 { L_,  "@{@[@FLOAT_ARRAY@]@{@[]@}@}",          8,   u5,    0,      LEN    },
 { L_,  "@{@[@DOUBLE_ARRAY@]@{@[]@}@}",         8,   u6,    0,      LEN    },
 { L_,  "@{@[@STRING_ARRAY@]@{@[]@}@}",         8,   u7,    0,      LEN    },
 { L_,  "@{@[@DATETIME_ARRAY@]@{@[]@}@}",       8,   u8,    0,      LEN    },
 { L_,  "@{@[@DATE_ARRAY@]@{@[]@}@}",           8,   u9,    0,      LEN    },
 { L_,  "@{@[@TIME_ARRAY@]@{@[]@}@}",           8,   u10,   0,      LEN    },

         // Testing with input containing characters after the parsed sequence

 { L_,  "@{@[@]@}X",                            2,   x0,    0,    LENSUB1  },
 { L_,  "@{@[@CHAR@]@{@\'a\'@}@}X",             8,   x1,    0,    LENSUB1  },
 { L_,  "@{@[@SHORT@]@{@1@}@}X",                8,   x2,    0,    LENSUB1  },
 { L_,  "@{@[@INT@]@{@1@}@}X",                  8,   x3,    0,    LENSUB1  },
 { L_,  "@{@[@INT64@]@{@1@}@}X",                8,   x4,    0,    LENSUB1  },
 { L_,  "@{@[@FLOAT@]@{@1@}@}X",                8,   x5,    0,    LENSUB1  },
 { L_,  "@{@[@DOUBLE@]@{@1@}@}X",               8,   x6,    0,    LENSUB1  },
 { L_,  "@{@[@STRING@]@{@\"Hello!\"@}@}X",      8,   x7,    0,    LENSUB1  },
 { L_,  "@{@[@DATETIME@]@{@3/3/3 3:03:03.003@}@}X",
                                                8,   x8,    0,    LENSUB1  },
 { L_,  "@{@[@DATE@]@{@3/3/3@}@}X",             8,   x9,    0,    LENSUB1  },
 { L_,  "@{@[@TIME@]@{@3:03:03.003@}@}X",       8,   x10,   0,    LENSUB1  },
 { L_,  "@{@[@LIST@]@{@{CHAR \'a\'}@}@}X",      8,   x11,   0,    LENSUB1  },
 { L_,  "@{@[@TABLE@]@{@{[CHAR]{\'a\'}}@}@}X",  8,   x12,   0,    LENSUB1  },
 { L_,  "@{@[@CHAR@@INT@@STRING@]@"
        "{@\'a\'@@1@@\"Hello!\"@}@}X",       16,   x13,   0,    LENSUB1  },
 { L_,  "@{@[@CHAR@]"
         "@{@\'a\'@}"
         "@{@\'a\'@}"
         "@{@\'a\'@}@}X",                      14,   x14,   0,    LENSUB1  },
 { L_,  "@{@[@CHAR@@INT@@STRING@]@"
        "{@\'a\'@@1@@\"Hello!\"@}"
        "{@\'a\'@@1@@\"Hello!\"@}"
        "{@\'a\'@@1@@\"Hello!\"@}@}X",       27,   x15,   0,    LENSUB1  },
 { L_,  "@{@[@CHAR_ARRAY@]@{@[\'a\']@}@}X",     8,   y1,    0,    LENSUB1  },
 { L_,  "@{@[@SHORT_ARRAY@]@{@[1]@}@}X",        8,   y2,    0,    LENSUB1  },
 { L_,  "@{@[@INT_ARRAY@]@{@[1]@}@}X",          8,   y3,    0,    LENSUB1  },
 { L_,  "@{@[@INT64_ARRAY@]@{@[1]@}@}X",        8,   y4,    0,    LENSUB1  },
 { L_,  "@{@[@FLOAT_ARRAY@]@{@[1]@}@}X",        8,   y5,    0,    LENSUB1  },
 { L_,  "@{@[@DOUBLE_ARRAY@]@{@[1]@}@}X",       8,   y6,    0,    LENSUB1  },
 { L_,  "@{@[@STRING_ARRAY@]@"
        "{@[\"Hello!\"]@}@}X",                  8,   y7,    0,    LENSUB1  },
 { L_,  "@{@[@DATETIME_ARRAY@]@"
        "{@[3/3/3 3:03:03.003]@}@}X",           8,   y8,    0,    LENSUB1  },
 { L_,  "@{@[@DATE_ARRAY@]@{@[3/3/3]@}@}X",     8,   y9,    0,    LENSUB1  },
 { L_,  "@{@[@TIME_ARRAY@]@{@[3:03:03.003]@}@}X",
                                                8,   y10,   0,    LENSUB1  },

                // Testing with invalid input

                // Opening curly brace is missing

 { L_,  "@}X",                                  1,  stub,  FAILURE,  0     },
 { L_,  "@[DOUBLE_ARRAY@ [1]@}",                1,  stub,  FAILURE,  0     },

                // Closing curly brace is missing
 { L_,  "@{@[@]",                               3,  stub,  FAILURE, LEN    },
 { L_,  "@{@[@CHAR@]@{@\'a\'@}",                7,  stub,  FAILURE, LEN    },

                // Opening/closing square bracket is missing

 { L_,  "@{@[@}",                               3,  stub,  FAILURE,   2    },
 { L_,  "@{@[@CHAR@{@\'a\'@}",                  4,  stub,  FAILURE,   6    },
 { L_,  "@{@]}",                                2,  stub,  FAILURE,   1    },
 { L_,  "@{@CHAR@{@\'a\'@}@}",                  2,  stub,  FAILURE,   1    },


                // Opening/closing row curly brace is missing

 { L_,  "@{@[@CHAR@@INT@@STRING@]@"
        "\'a\'@@1@@\"Hello!\"@}"
        "{@\'a\'@@1@@\"Hello!\"@}"
        "{@\'a\'@@1@@\"Hello!\"@}@}",         9,  stub,  FAILURE,    16  },

 { L_,  "@{@[@CHAR@@INT@@STRING@]@"
        "{@\'a\'@@1@@\"Hello!\"@}"
        "@\'a\'@@1@@\"Hello!\"@}"
        "{@\'a\'@@1@@\"Hello!\"@}@}",        16,  stub,  FAILURE,    30  },

 { L_,  "@{@[@CHAR@@INT@@STRING@]@"
        "{@\'a\'@@1@@\"Hello!\"@}"
        "{@\'a\'@@1@@\"Hello!\"@}"
        "@\'a\'@@1@@\"Hello!\"@}@}",         22,  stub,  FAILURE,    44  },

 { L_,  "@{@[@CHAR@@INT@@STRING@]@"
        "{@\'a\'@@1@@\"Hello!\"@"
        "{@\'a\'@@1@@\"Hello!\"@}"
        "{@\'a\'@@1@@\"Hello!\"@}@}",        15,  stub,  FAILURE,    29  },

 { L_,  "@{@[@CHAR@@INT@@STRING@]@"
        "{@\'a\'@@1@@\"Hello!\"@}"
        "{@\'a\'@@1@@\"Hello!\"@"
        "{@\'a\'@@1@@\"Hello!\"@}@}",        21,  stub,  FAILURE,    43  },

 { L_,  "@{@[@CHAR@@INT@@STRING@]@"
        "{@\'a\'@@1@@\"Hello!\"@}"
        "{@\'a\'@@1@@\"Hello!\"@}"
        "{@\'a\'@@1@@\"Hello!\"@}",          27,  stub,  FAILURE,    LEN },

                // Invalid element type

 { L_,  "@{@[@CHA@@INT@@STRING@]@"
        "{@\'a\'@@1@@\"Hello!\"@}"
        "{@\'a\'@@1@@\"Hello!\"@}"
        "{@\'a\'@@1@@\"Hello!\"@}@}",        3,  stub,  FAILURE,      5  },
 { L_,  "@{@[@CHAR@@IN@@STRING@]@"
        "{@\'a\'@@1@@\"Hello!\"@}"
        "{@\'a\'@@1@@\"Hello!\"@}"
        "{@\'a\'@@1@@\"Hello!\"@}@}",        5,  stub,  FAILURE,      8  },
 { L_,  "@{@[@CHAR@@INT@@TRING@]@"
        "{@\'a\'@@1@@\"Hello!\"@}"
        "{@\'a\'@@1@@\"Hello!\"@}"
        "{@\'a\'@@1@@\"Hello!\"@}@}",        7,  stub,  FAILURE,     10  },

                // Invalid element -- one test is sufficient here
                // because we are testing only that the delegated
                // functions' error return is propagated back to the
                // caller; we need not (re)test that each delegated
                // function recognizes an improperly formatted type.

 { L_,  "@{@[@CHAR@@INT@@STRING@]@"
        "{@\'a@@1@@\"Hello!\"@}"
        "{@\'a\'@@1@@\"Hello!\"@}"
        "{@\'a\'@@1@@\"Hello!\"@}@}",        10,  stub,  FAILURE,     19  },

 //-------------V
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bsl::vector<char> buffer;
            for (int i = 0; i < NUM_DATA; ++i) {
                // Orthogonal perturbation for optional whitespace
                const DataParsing<T>& DATA_I = DATA[i];
                const int LINE = DATA_I.d_lineNum;

                for (int j = 0; j < NUM_BLANKS; ++j) {
                    buffer.clear();
                    g(&buffer, DATA_I.d_str, j);

                    // Initialize 'result'
                    T result = initValue;

                    // Verify that expected value is different from initial
                    // value for valid inputs
                    if (0 == DATA_I.d_retVal) {
                        LOOP_ASSERT(LINE, DATA_I.d_result != initValue);
                    }

                    const char *endPos;
                    int res = bdempu_Aggregate::parseTable(
                        &endPos, &result, &buffer.front());
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

                    if (verbose && expectedOffset != actualOffset) {
                        T_(); T_(); P_(expectedOffset); P(actualOffset);
                        cout << &buffer.front() << endl;
                    }
                }
            }
        }

        if (verbose)
            cout << "\tTesting generator" << endl;
        {
            const DataGenerating<T> DATA[] = {
 //-------------^

 //line prefix level spaces d_val                   d_str
 //---- ------ ----- ------ ----- -----------------------------------------
 { L_,   "",    -1,   1,     x0,  "{\n"
                                  "  [\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x0,  "  {\n"
                                  "    [\n"
                                  "    ]\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x1,  "{\n"
                                  "  [\n"
                                  "   CHAR\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   \'a\'\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x1,  "  {\n"
                                  "    [\n"
                                  "      CHAR\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      \'a\'\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x2,  "{\n"
                                  "  [\n"
                                  "   SHORT\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   1\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x2,  "  {\n"
                                  "    [\n"
                                  "      SHORT\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      1\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x3,  "{\n"
                                  "  [\n"
                                  "   INT\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   1\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x3,  "  {\n"
                                  "    [\n"
                                  "      INT\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      1\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x4,  "{\n"
                                  "  [\n"
                                  "   INT64\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   1\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x4,  "  {\n"
                                  "    [\n"
                                  "      INT64\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      1\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x5,  "{\n"
                                  "  [\n"
                                  "   FLOAT\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   1\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x5,  "  {\n"
                                  "    [\n"
                                  "      FLOAT\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      1\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x6,  "{\n"
                                  "  [\n"
                                  "   DOUBLE\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   1\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x6,  "  {\n"
                                  "    [\n"
                                  "      DOUBLE\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      1\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x7,  "{\n"
                                  "  [\n"
                                  "   STRING\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   Hello! \n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x7,  "  {\n"
                                  "    [\n"
                                  "      STRING\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      Hello! \n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x8,  "{\n"
                                  "  [\n"
                                  "   DATETIME\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   0003/03/03 03:03:03.003\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x8,  "  {\n"
                                  "    [\n"
                                  "      DATETIME\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      0003/03/03 03:03:03.003\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x9,  "{\n"
                                  "  [\n"
                                  "   DATE\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   0003/03/03\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x9,  "  {\n"
                                  "    [\n"
                                  "      DATE\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      0003/03/03\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x10, "{\n"
                                  "  [\n"
                                  "   TIME\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   03:03:03.003\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x10, "  {\n"
                                  "    [\n"
                                  "      TIME\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      03:03:03.003\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x11, "{\n"
                                  "  [\n"
                                  "   LIST\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   {\n"
                                  "    CHAR \'a\'\n"
                                  "   }\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x11, "  {\n"
                                  "    [\n"
                                  "      LIST\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      {\n"
                                  "        CHAR \'a\'\n"
                                  "      }\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x12, "{\n"
                                  "  [\n"
                                  "   TABLE\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   {\n"
                                  "    [\n"
                                  "     CHAR\n"
                                  "    ]\n"
                                  "    {\n"
                                  "     \'a\'\n"
                                  "    }\n"
                                  "   }\n"
                                  "  }\n"
                                  " }"                                       },
 { L_,   "",     1,   2,     x12, "  {\n"
                                  "    [\n"
                                  "      TABLE\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      {\n"
                                  "        [\n"
                                  "          CHAR\n"
                                  "        ]\n"
                                  "        {\n"
                                  "          \'a\'\n"
                                  "        }\n"
                                  "      }\n"
                                  "    }\n"
                                  "  }",                                     },
 { L_,   "",    -1,   1,     x13, "{\n"
                                  "  [\n"
                                  "   CHAR\n"
                                  "   INT\n"
                                  "   STRING\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   \'a\'\n"
                                  "   1\n"
                                  "   Hello! \n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x13, "  {\n"
                                  "    [\n"
                                  "      CHAR\n"
                                  "      INT\n"
                                  "      STRING\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      \'a\'\n"
                                  "      1\n"
                                  "      Hello! \n"
                                  "    }\n"
                                  "  }",                                     },
 { L_,   "",    -1,   1,     x14, "{\n"
                                  "  [\n"
                                  "   CHAR\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   \'a\'\n"
                                  "  }\n"
                                  "  {\n"
                                  "   \'a\'\n"
                                  "  }\n"
                                  "  {\n"
                                  "   \'a\'\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x14, "  {\n"
                                  "    [\n"
                                  "      CHAR\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      \'a\'\n"
                                  "    }\n"
                                  "    {\n"
                                  "      \'a\'\n"
                                  "    }\n"
                                  "    {\n"
                                  "      \'a\'\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x15, "{\n"
                                  "  [\n"
                                  "   CHAR\n"
                                  "   INT\n"
                                  "   STRING\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   \'a\'\n"
                                  "   1\n"
                                  "   Hello! \n"
                                  "  }\n"
                                  "  {\n"
                                  "   \'a\'\n"
                                  "   1\n"
                                  "   Hello! \n"
                                  "  }\n"
                                  "  {\n"
                                  "   \'a\'\n"
                                  "   1\n"
                                  "   Hello! \n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x15, "  {\n"
                                  "    [\n"
                                  "      CHAR\n"
                                  "      INT\n"
                                  "      STRING\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      \'a\'\n"
                                  "      1\n"
                                  "      Hello! \n"
                                  "    }\n"
                                  "    {\n"
                                  "      \'a\'\n"
                                  "      1\n"
                                  "      Hello! \n"
                                  "    }\n"
                                  "    {\n"
                                  "      \'a\'\n"
                                  "      1\n"
                                  "      Hello! \n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     z1,  "{\n"
                                  "  [\n"
                                  "   CHAR\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   NULL\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     z1,  "  {\n"
                                  "    [\n"
                                  "      CHAR\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      NULL\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     z2,  "{\n"
                                  "  [\n"
                                  "   SHORT\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   NULL\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     z2,  "  {\n"
                                  "    [\n"
                                  "      SHORT\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      NULL\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     z3,  "{\n"
                                  "  [\n"
                                  "   INT\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   NULL\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     z3,  "  {\n"
                                  "    [\n"
                                  "      INT\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      NULL\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     z4,  "{\n"
                                  "  [\n"
                                  "   INT64\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   NULL\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     z4,  "  {\n"
                                  "    [\n"
                                  "      INT64\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      NULL\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     z5,  "{\n"
                                  "  [\n"
                                  "   FLOAT\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   NULL\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     z5,  "  {\n"
                                  "    [\n"
                                  "      FLOAT\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      NULL\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     z6,  "{\n"
                                  "  [\n"
                                  "   DOUBLE\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   NULL\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     z6,  "  {\n"
                                  "    [\n"
                                  "      DOUBLE\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      NULL\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     z7,  "{\n"
                                  "  [\n"
                                  "   STRING\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   NULL \n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     z7,  "  {\n"
                                  "    [\n"
                                  "      STRING\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      NULL \n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     z8,  "{\n"
                                  "  [\n"
                                  "   DATETIME\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   NULL\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     z8,  "  {\n"
                                  "    [\n"
                                  "      DATETIME\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      NULL\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     z9,  "{\n"
                                  "  [\n"
                                  "   DATE\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   NULL\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     z9,  "  {\n"
                                  "    [\n"
                                  "      DATE\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      NULL\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     z10, "{\n"
                                  "  [\n"
                                  "   TIME\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   NULL\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     z10, "  {\n"
                                  "    [\n"
                                  "      TIME\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      NULL\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     z11, "{\n"
                                  "  [\n"
                                  "   LIST\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   NULL\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     z11, "  {\n"
                                  "    [\n"
                                  "      LIST\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      NULL\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     z12, "{\n"
                                  "  [\n"
                                  "   TABLE\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   NULL\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     z12, "  {\n"
                                  "    [\n"
                                  "      TABLE\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      NULL\n"
                                  "    }\n"
                                  "  }",                                     },
 { L_,   "",    -1,   1,     y1,  "{\n"
                                  "  [\n"
                                  "   CHAR_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "    \'a\'\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y1,  "  {\n"
                                  "    [\n"
                                  "      CHAR_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "        \'a\'\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     y2,  "{\n"
                                  "  [\n"
                                  "   SHORT_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "    1\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y2,  "  {\n"
                                  "    [\n"
                                  "      SHORT_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "        1\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     y3,  "{\n"
                                  "  [\n"
                                  "   INT_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "    1\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y3,  "  {\n"
                                  "    [\n"
                                  "      INT_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "        1\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     y4,  "{\n"
                                  "  [\n"
                                  "   INT64_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "    1\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y4,  "  {\n"
                                  "    [\n"
                                  "      INT64_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "        1\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     y5,  "{\n"
                                  "  [\n"
                                  "   FLOAT_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "    1\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y5,  "  {\n"
                                  "    [\n"
                                  "      FLOAT_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "        1\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     y6,  "{\n"
                                  "  [\n"
                                  "   DOUBLE_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "    1\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y6,  "  {\n"
                                  "    [\n"
                                  "      DOUBLE_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "        1\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     y7,  "{\n"
                                  "  [\n"
                                  "   STRING_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "    Hello!\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y7,  "  {\n"
                                  "    [\n"
                                  "      STRING_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "        Hello!\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     y8,  "{\n"
                                  "  [\n"
                                  "   DATETIME_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "    0003/03/03 03:03:03.003\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y8,  "  {\n"
                                  "    [\n"
                                  "      DATETIME_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "        0003/03/03 03:03:03.003\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     y9,  "{\n"
                                  "  [\n"
                                  "   DATE_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "    0003/03/03\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y9,  "  {\n"
                                  "    [\n"
                                  "      DATE_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "        0003/03/03\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     y10, "{\n"
                                  "  [\n"
                                  "   TIME_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "    03:03:03.003\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y10, "  {\n"
                                  "    [\n"
                                  "      TIME_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "        03:03:03.003\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     u1,  "{\n"
                                  "  [\n"
                                  "   CHAR_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     u1,  "  {\n"
                                  "    [\n"
                                  "      CHAR_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     u2,  "{\n"
                                  "  [\n"
                                  "   SHORT_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     u2,  "  {\n"
                                  "    [\n"
                                  "      SHORT_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     u3,  "{\n"
                                  "  [\n"
                                  "   INT_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     u3,  "  {\n"
                                  "    [\n"
                                  "      INT_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     u4,  "{\n"
                                  "  [\n"
                                  "   INT64_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     u4,  "  {\n"
                                  "    [\n"
                                  "      INT64_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     u5,  "{\n"
                                  "  [\n"
                                  "   FLOAT_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     u5,  "  {\n"
                                  "    [\n"
                                  "      FLOAT_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     u6,  "{\n"
                                  "  [\n"
                                  "   DOUBLE_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     u6,  "  {\n"
                                  "    [\n"
                                  "      DOUBLE_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     u7,  "{\n"
                                  "  [\n"
                                  "   STRING_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     u7,  "  {\n"
                                  "    [\n"
                                  "      STRING_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     u8,  "{\n"
                                  "  [\n"
                                  "   DATETIME_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     u8,  "  {\n"
                                  "    [\n"
                                  "      DATETIME_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     u9,  "{\n"
                                  "  [\n"
                                  "   DATE_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     u9,  "  {\n"
                                  "    [\n"
                                  "      DATE_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     u10, "{\n"
                                  "  [\n"
                                  "   TIME_ARRAY\n"
                                  "  ]\n"
                                  "  {\n"
                                  "   [\n"
                                  "   ]\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     u10, "  {\n"
                                  "    [\n"
                                  "      TIME_ARRAY\n"
                                  "    ]\n"
                                  "    {\n"
                                  "      [\n"
                                  "      ]\n"
                                  "    }\n"
                                  "  }",                                     },
 //-------------V
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bsl::vector<char> buffer;
            for (int i = 0; i < NUM_DATA; ++i) {
                const DataGenerating<T>& DATA_I = DATA[i];
                const int LINE = DATA_I.d_lineNum;
                buffer.clear();

                appendToBuffer(&buffer, DATA_I.d_prefix);
                bdempu_Aggregate::generateTable(
                    &buffer, DATA_I.d_val, DATA_I.d_level, DATA_I.d_spaces);
                buffer.push_back('\0');

                int diff = strcmp(&buffer.front(), DATA_I.d_str);
                LOOP_ASSERT(LINE, !diff);
                if (verbose && diff) {
                    cout << "\t\texpected:"  << DATA_I.d_str    << endl;
                    cout << "\t\tgenerated:" << &buffer.front() << endl;
                }
                LOOP_ASSERT(LINE, buffer[buffer.size() - 1] == '\0');

                // Verify that the we can get the original value by parsing the
                // generated string.
                T result = initValue;
                LOOP_ASSERT(LINE, DATA_I.d_val != result);
                const char *endPos;
                int ret = bdempu_Aggregate::parseTable(&endPos, &result,
                    &buffer.front() + strlen(DATA_I.d_prefix));
                LOOP_ASSERT(LINE, 0 == ret);
                LOOP_ASSERT(LINE, DATA_I.d_val == result);
                LOOP_ASSERT(LINE, '\0' == *endPos);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PARSE/GENERATE LIST
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
        //       2. The result is null-terminated.
        //       3. The generated result can be parsed back with the
        //          corresponding parser function.
        //       4. The values that were in the buffer prior to the call are
        //          not changed.
        //
        // Plan:
        //   To test the parsing method, create a table containing the input
        //   string and the expected output value.  Use nested loop
        //   ("orthogonal perturbation") to test each input to the parsing
        //   method with different <WHITESPACE> strings defined in global
        //   'BLANKS' structure.  In every iteration one of the <WHITESPACE>
        //   strings is inserted in the beginning of the input string.
        //
        //   Test with the following valid lists:
        //       1. Empty list
        //
        //       2. List containing one element of each type
        //
        //       3. With list element containing multiple elements
        //
        //    Test with the following invalid lists:
        //       1. Opening/closing curly brace is missing
        //       2. Element or element type is incorrect
        //
        //   To test generating method create a table containing the input
        //   object and the expected textual representation.  Test that the
        //   generate method correctly generates indentation when 'level' is
        //   less than and greater than zero.
        //
        // Testing:
        //    parseList(cchar **endPos, List *result, cchar *input);
        //    generateList(b_ca *buff, cList& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING PARSE/GENERATE LIST" << endl
                 << "===========================" << endl;

        typedef bdem_List T;
        typedef bsls_PlatformUtil::Int64 Int64;

        const char X_CHAR = 'a';
        const short X_SHORT = 1;
        const int X_INT = 1;
        const Int64 X_INT64 = 1;
        const float X_FLOAT = 1;
        const double X_DOUBLE = 1;
        const bsl::string X_STRING = "Hello!";
        const bdet_Date X_DATE(3, 3, 3);
        const bdet_Time X_TIME(3, 3, 3, 3);
        const bdet_Datetime X_DATETIME(X_DATE, X_TIME);
        const bsl::vector<char> X_A1(1, X_CHAR);



        T stub;     // Identifies undefined value
        T initValue;
        initValue.appendString("init"); // Used to initialize result
        T x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13;

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
        x11.appendList(x1);

        bsl::vector<bdem_ElemType::Type> columnTypeArray;
        columnTypeArray.push_back(bdem_ElemType::BDEM_CHAR);
        bdem_Table xTable(&columnTypeArray.front(), columnTypeArray.size());
        xTable.appendRow(x1);
        x12.appendTable(xTable);
        x13.appendChar(X_CHAR);
        x13.appendInt(X_INT);
        x13.appendString(X_STRING);


        T y1, y2, y3, y4, y5, y6, y7, y8, y9, y10;
        const bsl::vector<char> A1(1, X_CHAR);
        y1.appendCharArray(A1);
        const bsl::vector<short> A2(1, X_SHORT);
        y2.appendShortArray(A2);
        const bsl::vector<int> A3(1, X_INT);
        y3.appendIntArray(A3);
        const bsl::vector<Int64> A4(1, X_INT64);
        y4.appendInt64Array(A4);
        const bsl::vector<float> A5(1, X_FLOAT);
        y5.appendFloatArray(A5);
        const bsl::vector<double> A6(1, X_DOUBLE);
        y6.appendDoubleArray(A6);
        const bsl::vector<bsl::string> A7(1, X_STRING);
        y7.appendStringArray(A7);
        const bsl::vector<bdet_Datetime> A8(1, X_DATETIME);
        y8.appendDatetimeArray(A8);
        const bsl::vector<bdet_Date> A9(1, X_DATE);
        y9.appendDateArray(A9);
        const bsl::vector<bdet_Time> A10(1, X_TIME);
        y10.appendTimeArray(A10);

        if (verbose)
            cout << "\tTesting parser" << endl;
        {
            const DataParsing<T> DATA[] = {
 //-------------^

 //line                    d_str                #@ result retVal  endPos
 //---- --------------------------------------- -- ------ ------  ------
 { L_,  "@{@}",                                 2,   x0,    0,      LEN    },
 { L_,  "@{@CHAR@ \'a\'@}",                     4,   x1,    0,      LEN    },
 { L_,  "@{@SHORT@ 1@}",                        4,   x2,    0,      LEN    },
 { L_,  "@{@INT@ 1@}",                          4,   x3,    0,      LEN    },
 { L_,  "@{@INT64@ 1@}",                        4,   x4,    0,      LEN    },
 { L_,  "@{@FLOAT@ 1@}",                        4,   x5,    0,      LEN    },
 { L_,  "@{@DOUBLE@ 1@}",                       4,   x6,    0,      LEN    },
 { L_,  "@{@STRING@ \"Hello!\"@}",              4,   x7,    0,      LEN    },
 { L_,  "@{@DATETIME@ 3/3/3 3:03:03.003@}",     4,   x8,    0,      LEN    },
 { L_,  "@{@DATE@ 3/3/3@}",                     4,   x9,    0,      LEN    },
 { L_,  "@{@TIME@ 3:03:03.003@}",               4,   x10,   0,      LEN    },
 { L_,  "@{@LIST@ {CHAR \'a\'}@}",              4,   x11,   0,      LEN    },
 { L_,  "@{@TABLE@ {[CHAR]{\'a\'}}@}",          4,   x12,   0,      LEN    },
 { L_,  "@{@CHAR@ \'a\'@,@INT@ 1@,@STRING@"
                               "\"Hello!\"@}", 10,   x13,   0,      LEN    },
 { L_,  "@{@CHAR@ \'a\'@INT@ 1@STRING@"
                               "\"Hello!\"@}", 10,   x13,   0,      LEN    },
 { L_,  "@{@CHAR_ARRAY@ ['a']@}",               4,   y1,    0,      LEN    },
 { L_,  "@{@SHORT_ARRAY@ [1]@}",                4,   y2,    0,      LEN    },
 { L_,  "@{@INT_ARRAY@ [1]@}",                  4,   y3,    0,      LEN    },
 { L_,  "@{@INT64_ARRAY@ [1]@}",                4,   y4,    0,      LEN    },
 { L_,  "@{@FLOAT_ARRAY@ [1]@}",                4,   y5,    0,      LEN    },
 { L_,  "@{@DOUBLE_ARRAY@ [1]@}",               4,   y6,    0,      LEN    },
 { L_,  "@{@STRING_ARRAY@ [\"Hello!\"]@}",      4,   y7,    0,      LEN    },
 { L_,  "@{@DATETIME_ARRAY@ [3/3/3 3:03:03.003]@}", 4,   y8,  0,    LEN    },
 { L_,  "@{@DATE_ARRAY@ [3/3/3]@}",             4,   y9,    0,      LEN    },
 { L_,  "@{@TIME_ARRAY@ [3:03:03.003]@}",       4,   y10,   0,      LEN    },

         // Testing with input containing characters after the parsed sequence

 { L_,  "@{@}X",                                2,   x0,    0,    LENSUB1  },
 { L_,  "@{@CHAR@\'a\'@}X",                     4,   x1,    0,    LENSUB1  },
 { L_,  "@{@SHORT@ 1@}X",                       4,   x2,    0,    LENSUB1  },
 { L_,  "@{@INT@ 1@}X",                         4,   x3,    0,    LENSUB1  },
 { L_,  "@{@INT64@ 1@}X",                       4,   x4,    0,    LENSUB1  },
 { L_,  "@{@FLOAT@ 1@}X",                       4,   x5,    0,    LENSUB1  },
 { L_,  "@{@DOUBLE@ 1@}X",                      4,   x6,    0,    LENSUB1  },
 { L_,  "@{@STRING@ \"Hello!\"@}X",             4,   x7,    0,    LENSUB1  },
 { L_,  "@{@DATETIME@ 3/3/3 3:03:03.003@}X",    4,   x8,    0,    LENSUB1  },
 { L_,  "@{@DATE@ 3/3/3@}X",                    4,   x9,    0,    LENSUB1  },
 { L_,  "@{@TIME@ 3:03:03.003@}X",              4,   x10,   0,    LENSUB1  },
 { L_,  "@{@LIST@ {CHAR \'a\'}@}X",             4,   x11,   0,    LENSUB1  },
 { L_,  "@{@TABLE@ {[CHAR]{\'a\'}}@}X",         4,   x12,   0,    LENSUB1  },
 { L_,  "@{@CHAR@\'a\'@,@INT@ 1@,@STRING@"
                              "\"Hello!\"@}X", 10,   x13,   0,    LENSUB1  },
 { L_,  "@{@CHAR@\'a\'@INT@ 1@STRING@"
                              "\"Hello!\"@}X", 10,   x13,   0,    LENSUB1  },
 { L_,  "@{@CHAR_ARRAY@ ['a']@}X",              4,   y1,    0,    LENSUB1  },
 { L_,  "@{@SHORT_ARRAY@ [1]@}X",               4,   y2,    0,    LENSUB1  },
 { L_,  "@{@INT_ARRAY@ [1]@}X",                 4,   y3,    0,    LENSUB1  },
 { L_,  "@{@INT64_ARRAY@ [1]@}X",               4,   y4,    0,    LENSUB1  },
 { L_,  "@{@FLOAT_ARRAY@ [1]@}X",               4,   y5,    0,    LENSUB1  },
 { L_,  "@{@DOUBLE_ARRAY@ [1]@}X",              4,   y6,    0,    LENSUB1  },
 { L_,  "@{@STRING_ARRAY@ [\"Hello!\"]@}X",     4,   y7,    0,    LENSUB1  },
 { L_,  "@{@DATETIME_ARRAY@[3/3/3 3:03:03.003]@}X", 4,  y8, 0,    LENSUB1  },
 { L_,  "@{@DATE_ARRAY@ [3/3/3]@}X",            4,   y9,    0,    LENSUB1  },
 { L_,  "@{@TIME_ARRAY@ [3:03:03.003]@}X",      4,   y10,   0,    LENSUB1  },


                // Testing with invalid input

                // Opening curly brace is missing

#if 0  // JEFF -- there is no convenient way to express what lengths
       // these should be, without writing LEN sub strlen(data) -1.
       // I am skipping that, since you are probably redoing this whole
       // section anyhow.
 { L_,  "@}X",                                  1,  stub,  FAILURE,  0     }
 { L_,  "@DOUBLE_ARRAY@ [1]@}",                 1,  stub,  FAILURE,  0     },

                // Closing curly brace is missing
 { L_,  "@{",                                   1,  stub,  FAILURE,  1     },
 { L_,  "@{@TIME@ 3:03:03.003",                 3,  stub,  FAILURE, LEN    },
 { L_,  "@{@DOUBLE_ARRAY@ [1]",                 3,  stub,  FAILURE, 17     },

                // Invalid Element type

 { L_,  "@{@TAME",                              2,  stub,  FAILURE, 3      },
 { L_,  "@{@x",                                 2,  stub,  FAILURE, 1      },
#endif
                // Element is missing

 { L_,  "@{@CHAR@}",                            3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@SHORT@}",                           3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@INT@}",                             3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@INT64@}",                           3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@FLOAT@}",                           3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@DOUBLE@}",                          3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@STRING@}",                          3,  stub, FAILURE,   LEN    },
 { L_,  "@{@DATETIME@}",                        3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@DATE@}",                            3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@TIME@}",                            3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@LIST@}",                            3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@TABLE@}",                           3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@CHAR_ARRAY@}",                      3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@SHORT_ARRAY@}",                     3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@INT_ARRAY@}",                       3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@INT64_ARRAY@}",                     3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@FLOAT_ARRAY@}",                     3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@DOUBLE_ARRAY@}",                    3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@STRING_ARRAY@}",                    3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@DATETIME_ARRAY@}",                  3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@DATE_ARRAY@}",                      3,  stub, FAILURE, LENSUB1  },
 { L_,  "@{@TIME_ARRAY@}",                      3,  stub, FAILURE, LENSUB1  },

                // Invalid element -- one test is sufficient here
                // because we are testing only that the delegated
                // functions' error return is propagated back to the
                // caller; we need not (re)test that each delegated
                // function recognizes an improperly formatted type.

 { L_,  "@{@CHAR@ \'a\'@,@INT@ 1@,@STRING@ H",  9,  stub,  FAILURE, LEN     },
 { L_,  "@{@CHAR@ \'a\'@INT@ 1@STRING@ H",      9,  stub,  FAILURE, LEN     },
            };
 //-------------V
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bsl::vector<char> buffer;
            for (int i = 0; i < NUM_DATA; ++i) {
                // Orthogonal perturbation for optional whitespace
                const DataParsing<T>& DATA_I = DATA[i];
                const int LINE = DATA_I.d_lineNum;

                for (int j = 0; j < NUM_BLANKS; ++j) {
                    buffer.clear();
                    g(&buffer, DATA_I.d_str, j);

                    // Initialize 'result'
                    T result = initValue;

                    // Verify that expected value is different from initial
                    // value for valid inputs
                    if (0 == DATA_I.d_retVal) {
                        LOOP_ASSERT(LINE, DATA_I.d_result != initValue);
                    }

                    const char *endPos;
                    int res = bdempu_Aggregate::parseList(
                        &endPos, &result, &buffer.front());
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

                    if (verbose && expectedOffset != actualOffset) {
                        T_(); T_(); P_(expectedOffset); P(actualOffset);
                        cout << &buffer.front() << endl;
                    }
                }
            }
        }

        if (verbose)
            cout << "\tTesting generator" << endl;
        {
            const DataGenerating<T> DATA[] = {
 //-------------^

 //line prefix level spaces d_val                   d_str
 //---- ------ ----- ------ ----- -----------------------------------------
 { L_,   "",    -1,   1,     x0,  "{\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x0,  "  {\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     x0,  "x {\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     x0,  "x "
                                  "  {\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x1,  "{\n"
                                  "  CHAR \'a\'\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x1,  "  {\n"
                                  "    CHAR \'a\'\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     x1,  "x {\n"
                                  "  CHAR \'a\'\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     x1,  "x "
                                  "  {\n"
                                  "    CHAR \'a\'\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x2,  "{\n"
                                  "  SHORT 1\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x2,  "  {\n"
                                  "    SHORT 1\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     x2,  "x {\n"
                                  "  SHORT 1\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     x2,  "x "
                                  "  {\n"
                                  "    SHORT 1\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x3,  "{\n"
                                  "  INT 1\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x3,  "  {\n"
                                  "    INT 1\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     x3,  "x {\n"
                                  "  INT 1\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     x3,  "x "
                                  "  {\n"
                                  "    INT 1\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x4,  "{\n"
                                  "  INT64 1\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x4,  "  {\n"
                                  "    INT64 1\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     x4,  "x {\n"
                                  "  INT64 1\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     x4,  "x "
                                  "  {\n"
                                  "    INT64 1\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x5,  "{\n"
                                  "  FLOAT 1\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x5,  "  {\n"
                                  "    FLOAT 1\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     x5,  "x {\n"
                                  "  FLOAT 1\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     x5,  "x "
                                  "  {\n"
                                  "    FLOAT 1\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x6,  "{\n"
                                  "  DOUBLE 1\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x6,  "  {\n"
                                  "    DOUBLE 1\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     x6,  "x {\n"
                                  "  DOUBLE 1\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     x6,  "x "
                                  "  {\n"
                                  "    DOUBLE 1\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x7,  "{\n"
                                  "  STRING Hello! \n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x7,  "  {\n"
                                  "    STRING Hello! \n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     x7,  "x {\n"
                                  "  STRING Hello! \n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     x7,  "x "
                                  "  {\n"
                                  "    STRING Hello! \n"
                                  "  }",                                     },
 { L_,   "",    -1,   1,     x8,  "{\n"
                                  "  DATETIME 0003/03/03 03:03:03.003\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x8,  "  {\n"
                                  "    DATETIME 0003/03/03 03:03:03.003\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     x8,  "x {\n"
                                  "  DATETIME 0003/03/03 03:03:03.003\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     x8,  "x "
                                  "  {\n"
                                  "    DATETIME 0003/03/03 03:03:03.003\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x9,  "{\n"
                                  "  DATE 0003/03/03\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x9,  "  {\n"
                                  "    DATE 0003/03/03\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     x9,  "x {\n"
                                  "  DATE 0003/03/03\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     x9,  "x "
                                  "  {\n"
                                  "    DATE 0003/03/03\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x10, "{\n"
                                  "  TIME 03:03:03.003\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x10, "  {\n"
                                  "    TIME 03:03:03.003\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     x10, "x {\n"
                                  "  TIME 03:03:03.003\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     x10, "x "
                                  "  {\n"
                                  "    TIME 03:03:03.003\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x11, "{\n"
                                  "  LIST {\n"
                                  "   CHAR \'a\'\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x11, "  {\n"
                                  "    LIST {\n"
                                  "      CHAR \'a\'\n"
                                  "    }\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     x11, "x {\n"
                                  "  LIST {\n"
                                  "   CHAR \'a\'\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     x11, "x   {\n"
                                  "    LIST {\n"
                                  "      CHAR \'a\'\n"
                                  "    }\n"
                                  "  }",                                     },
 { L_,   "",    -1,   1,     x12, "{\n"
                                  "  TABLE {\n"
                                  "   [\n"
                                  "    CHAR\n"
                                  "   ]\n"
                                  "   {\n"
                                  "    \'a\'\n"
                                  "   }\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x12, "  {\n"
                                  "    TABLE {\n"
                                  "      [\n"
                                  "        CHAR\n"
                                  "      ]\n"
                                  "      {\n"
                                  "        \'a\'\n"
                                  "      }\n"
                                  "    }\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     x12, "x {\n"
                                  "  TABLE {\n"
                                  "   [\n"
                                  "    CHAR\n"
                                  "   ]\n"
                                  "   {\n"
                                  "    \'a\'\n"
                                  "   }\n"
                                  "  }\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     x12, "x   {\n"
                                  "    TABLE {\n"
                                  "      [\n"
                                  "        CHAR\n"
                                  "      ]\n"
                                  "      {\n"
                                  "        \'a\'\n"
                                  "      }\n"
                                  "    }\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     x13, "{\n"
                                  "  CHAR \'a\',\n"
                                  "  INT 1,\n"
                                  "  STRING Hello! \n"
                                  " }",                                      },
 { L_,   "",     1,   2,     x13, "  {\n"
                                  "    CHAR \'a\',\n"
                                  "    INT 1,\n"
                                  "    STRING Hello! \n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     x13, "x {\n"
                                  "  CHAR \'a\',\n"
                                  "  INT 1,\n"
                                  "  STRING Hello! \n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     x13, "x "
                                  "  {\n"
                                  "    CHAR \'a\',\n"
                                  "    INT 1,\n"
                                  "    STRING Hello! \n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     y1,  "{\n"
                                  "  CHAR_ARRAY [\n"
                                  "   \'a\'\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y1,  "  {\n"
                                  "    CHAR_ARRAY [\n"
                                  "      \'a\'\n"
                                  "    ]\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     y1,  "x {\n"
                                  "  CHAR_ARRAY [\n"
                                  "   \'a\'\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     y1,  "x   {\n"
                                  "    CHAR_ARRAY [\n"
                                  "      \'a\'\n"
                                  "    ]\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     y2,  "{\n"
                                  "  SHORT_ARRAY [\n"
                                  "   1\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y2,  "  {\n"
                                  "    SHORT_ARRAY [\n"
                                  "      1\n"
                                  "    ]\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     y2,  "x {\n"
                                  "  SHORT_ARRAY [\n"
                                  "   1\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     y2,  "x   {\n"
                                  "    SHORT_ARRAY [\n"
                                  "      1\n"
                                  "    ]\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     y3,  "{\n"
                                  "  INT_ARRAY [\n"
                                  "   1\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y3,  "  {\n"
                                  "    INT_ARRAY [\n"
                                  "      1\n"
                                  "    ]\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     y3,  "x {\n"
                                  "  INT_ARRAY [\n"
                                  "   1\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     y3,  "x   {\n"
                                  "    INT_ARRAY [\n"
                                  "      1\n"
                                  "    ]\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     y4,  "{\n"
                                  "  INT64_ARRAY [\n"
                                  "   1\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y4,  "  {\n"
                                  "    INT64_ARRAY [\n"
                                  "      1\n"
                                  "    ]\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     y4,  "x {\n"
                                  "  INT64_ARRAY [\n"
                                  "   1\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     y4,  "x   {\n"
                                  "    INT64_ARRAY [\n"
                                  "      1\n"
                                  "    ]\n"
                                  "  }",                                     },
 { L_,   "",    -1,   1,     y5,  "{\n"
                                  "  FLOAT_ARRAY [\n"
                                  "   1\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y5,  "  {\n"
                                  "    FLOAT_ARRAY [\n"
                                  "      1\n"
                                  "    ]\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     y5,  "x {\n"
                                  "  FLOAT_ARRAY [\n"
                                  "   1\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     y5,  "x   {\n"
                                  "    FLOAT_ARRAY [\n"
                                  "      1\n"
                                  "    ]\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     y6,  "{\n"
                                  "  DOUBLE_ARRAY [\n"
                                  "   1\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y6,  "  {\n"
                                  "    DOUBLE_ARRAY [\n"
                                  "      1\n"
                                  "    ]\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     y6,  "x {\n"
                                  "  DOUBLE_ARRAY [\n"
                                  "   1\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     y6,  "x   {\n"
                                  "    DOUBLE_ARRAY [\n"
                                  "      1\n"
                                  "    ]\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     y7,  "{\n"
                                  "  STRING_ARRAY [\n"
                                  "   Hello!\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y7,  "  {\n"
                                  "    STRING_ARRAY [\n"
                                  "      Hello!\n"
                                  "    ]\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     y7,  "x {\n"
                                  "  STRING_ARRAY [\n"
                                  "   Hello!\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     y7,  "x   {\n"
                                  "    STRING_ARRAY [\n"
                                  "      Hello!\n"
                                  "    ]\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     y8,  "{\n"
                                  "  DATETIME_ARRAY [\n"
                                  "   0003/03/03 03:03:03.003\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y8,  "  {\n"
                                  "    DATETIME_ARRAY [\n"
                                  "      0003/03/03 03:03:03.003\n"
                                  "    ]\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     y8,  "x {\n"
                                  "  DATETIME_ARRAY [\n"
                                  "   0003/03/03 03:03:03.003\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     y8,  "x   {\n"
                                  "    DATETIME_ARRAY [\n"
                                  "      0003/03/03 03:03:03.003\n"
                                  "    ]\n"
                                  "  }",                                     },

 { L_,   "",    -1,   1,     y9,  "{\n"
                                  "  DATE_ARRAY [\n"
                                  "   0003/03/03\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y9,  "  {\n"
                                  "    DATE_ARRAY [\n"
                                  "      0003/03/03\n"
                                  "    ]\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     y9,  "x {\n"
                                  "  DATE_ARRAY [\n"
                                  "   0003/03/03\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     y9,  "x   {\n"
                                  "    DATE_ARRAY [\n"
                                  "      0003/03/03\n"
                                  "    ]\n"
                                  "  }",                                     },
 { L_,   "",    -1,   1,     y10, "{\n"
                                  "  TIME_ARRAY [\n"
                                  "   03:03:03.003\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "",     1,   2,     y10, "  {\n"
                                  "    TIME_ARRAY [\n"
                                  "      03:03:03.003\n"
                                  "    ]\n"
                                  "  }",                                     },
 { L_,   "x ",  -1,   1,     y10, "x {\n"
                                  "  TIME_ARRAY [\n"
                                  "   03:03:03.003\n"
                                  "  ]\n"
                                  " }",                                      },
 { L_,   "x ",   1,   2,     y10, "x   {\n"
                                  "    TIME_ARRAY [\n"
                                  "      03:03:03.003\n"
                                  "    ]\n"
                                  "  }",                                     },
 //-------------V
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bsl::vector<char> buffer;
            for (int i = 0; i < NUM_DATA; ++i) {
                const DataGenerating<T>& DATA_I = DATA[i];
                const int LINE = DATA_I.d_lineNum;
                buffer.clear();

                appendToBuffer(&buffer, DATA_I.d_prefix);
                bdempu_Aggregate::generateList(
                    &buffer, DATA_I.d_val, DATA_I.d_level, DATA_I.d_spaces);
                buffer.push_back('\0');

                int diff = strcmp(&buffer.front(), DATA_I.d_str);
                LOOP_ASSERT(LINE, !diff);
                if (verbose && diff) {
                    cout << "\t\texpected:"  << DATA_I.d_str    << endl;
                    cout << "\t\tgenerated:" << &buffer.front() << endl;
#if 0
if( diff )
for( int i = 0; i < strlen(&buffer.front()); ++i ) {
    cout << "DATA_I.d_str[" << i << "] is: " <<
        ((DATA_I.d_str[i] == ' ') ? '@' : DATA_I.d_str[i])  << endl;
    cout << "&buffer.front()[" << i << "] is: " <<
        (((&buffer.front())[i] == ' ') ? '@' : (&buffer.front())[i]) << endl;
}
#endif
                }
                LOOP_ASSERT(LINE, buffer[buffer.size() - 1] == '\0');

                // Verify that the we can get the original value by parsing the
                // generated string.
                T result = initValue;
                LOOP_ASSERT(LINE, DATA_I.d_val != result);
                const char *endPos;
                int ret = bdempu_Aggregate::parseList(&endPos, &result,
                    &buffer.front() + strlen(DATA_I.d_prefix));
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
        //     specification with every type of input.
        //
        // Plan:
        //   To test 'g' create a table containing the input string with '@'
        //   inserted at the beginning, middle, and at the end and the
        //   expected output sting for every index in 'BLANKS'.  Verify that
        //   the result generated by 'g' is identical to the expected for every
        //   string in 'BLANKS'.
        //
        // Testing:
        //    g(bsl::vector<char> *buffer, const char *inStr, int index)
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'g'" << endl
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
                {  L_,       "abcd",                "abcd",        0      },
                {  L_,       "abcd",                "@ab@cd@",     0      },
                {  L_,       "// C++ blank \n"
                             "ab"
                             "// C++ blank \n"
                             "cd"
                             "// C++ blank \n",     "@ab@cd@",     1      },
                {  L_,       "/*C blanks*/"
                             "ab"
                             "/*C blanks*/"
                             "cd"
                             "/*C blanks*/",        "@ab@cd@",     2      },
                {  L_,       "  \n\n\t/* //*/"
                             "ab"
                             "  \n\n\t/* //*/"
                             "cd"
                             "  \n\n\t/* //*/",     "@ab@cd@",     3      },
                {  L_,       "  ///*  \t\n "
                             "ab"
                             "  ///*  \t\n "
                             "cd"
                             "  ///*  \t\n ",       "@ab@cd@",     4      },
                {  L_,       " "
                             "ab"
                             " "
                             "cd"
                             " ",                   "@ab@cd@",     5      },
                {  L_,       "  "
                             "ab"
                             "  "
                             "cd"
                             "  ",                  "@ab@cd@",     6      },
                {  L_,       "\t"
                             "ab"
                             "\t"
                             "cd"
                             "\t",                  "@ab@cd@",     7      },
                {  L_,       "\n"
                             "ab"
                             "\n"
                             "cd"
                             "\n",                  "@ab@cd@",     8      },
                {  L_,       " \t"
                             "ab"
                             " \t"
                             "cd"
                             " \t",                 "@ab@cd@",     9      },
                {  L_,       " \n"
                             "ab"
                             " \n"
                             "cd"
                             " \n",                 "@ab@cd@",     10     },
                {  L_,       "\t\n         "
                             "ab"
                             "\t\n         "
                             "cd"
                             "\t\n         ",       "@ab@cd@",     11     },
                {  L_,       "\n\t           "
                             "ab"
                             "\n\t           "
                             "cd"
                             "\n\t           ",     "@ab@cd@",     12     },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bsl::vector<char> outputArray;
            for (int i = 0; i < NUM_DATA; ++i) {
                outputArray.clear();
                g(&outputArray, DATA[i].d_input, DATA[i].d_index);
                int diff = strcmp(DATA[i].d_generated, &outputArray.front());
                LOOP_ASSERT(DATA[i].d_line, 0 == diff);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
