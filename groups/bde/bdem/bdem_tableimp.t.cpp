// bdem_tableimp.t.cpp                                                -*-C++-*-

#include <bdem_tableimp.h>

#include <bdem_descriptor.h>
#include <bdem_elemtype.h>
#include <bdem_functiontemplates.h>
#include <bdem_properties.h>

#include <bdetu_unset.h>

#include <bdex_byteinstream.h>
#include <bdex_byteoutstream.h>
#include <bdex_testinstream.h>
#include <bdex_testoutstream.h>
#include <bdex_testinstreamexception.h>       // for testing only

#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bslmf_isconvertible.h>

#include <bsls_alignmentutil.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                          *** Overview ***
//
// 'bdem_TableImp' provides the innards of a bdem_Table.  It has most, but
// not all, of the manipulators and accessors of bdem_Table, especially
// missing the '[]' operator.  In this module, we write static routines
// 'getElemRef' and 'getModElemRef' to make up for this lack.
//
// In addition to test all the manipulators and accessors in the component,
// this module also verifies proper allocation strategies and aliasing
// behavior.
//
// Note that since this component is at a lower level than 'bdem_List' or
// 'bdem_Table' we do not do any testing of 'bdem_TableImp's that contain
// aggregate types.
//
// Much of the behavior is supported by 'bdem_RowData', which is beneath this
// component.  It is assumed in these tests that 'bdem_RowData' is tested
// and sound.
//
// Note that in the majority of the tests we primarily use three types ('int',
// 'bsl::string', and 'char') to demonstrate that type-based arguments are
// handled correctly.  For these types we also rely on a small number of
// file-scope 'const' variables to use in "set/test" contexts.
//
// There is no requirement for a 'gg' function.  To make up for the lack of
// one, the static function 'fillTable' is supplied, which fills a table
// of columns of types "int, string, char, (repeating)".
//
//-----------------------------------------------------------------------------
//
////CREATORS
// [ 2] bdem_TableImp(allocMode, Z)
// [ 2] reset(elemTypes[], numElements, attrLookupTbl)
// [ 2] bdem_TableImp(allocMode, initialMemory, Z)
// [ 2] bdem_TableImp(elemTypes[], numElements, attrLookupTbl, allocMode, Z)
// [ 2] bdem_TableImp(elemTypes[], numElements, attrLookupTbl, allocMode,
//                                                            initialMemory, Z)
// [ 2] bdem_TableImp(rowDef&, allocMode, Z)
// [ 2] bdem_TableImp(rowDef&, allocMode, initialMemory, Z)
// [ 2] bdem_TableImp(original&, allocMode, Z)
// [ 2] bdem_TableImp(original&, allocMode, initialMemory, Z)
// [ 2] ~bdem_TableImp()
//
////MANIPULATORS
// [ 7] insertRow()
// [ 7] row()
// [ 7] operator=()
// [ 7] removeRow()
// [ 7] removeRows()
// [ 7] removeAll();
// [ 7] clear()
// [ 7] makeColumnNull()
// [ 7] makeRowsNull()
// [ 7] makeAllNull()
// [ 7] insertRows()
// [ 7] insertNullRows()
// [ 7] setColumnValue()
// [ 7] setColumnValueRaw()
// [ 5] reserveMemory()
// [11] void reserveRaw(bsl::size_t numRows);
// [ 8] bdexStreamInImp()
// TBD
// [  ] rowElemRef()
//
////ACCESSORS
// [ 7] columnType()
// [12] bsl::size_t capacityRaw() const;
// [ 7] numRows()
// [ 7] numColumns()
// [ 7] row()
// [ 7] isAnyInColumnNonNull()
// [ 7] isAnyInColumnNull()
// [ 7] isAnyNonNull()
// [ 7] isAnyNull()
// TBD
// [  ] rowElemRef()
// [ 8] bdexStreamOutImp()
// [10] print()
// [ 1] BREATHING TEST

// PRIVATE GEOMETRIC MEMORY GROWTH
// [13] bdem_TableImp_enableGeometricMemoryGrowth()
// [13] bdem_TableImp_disableGeometricMemoryGrowth()

//=============================================================================
//            STANDARD BDE ASSERT TEST MACRO
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

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) {                                                    \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) {                                                 \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                  \
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

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\t" << #N << ": " << N << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                              // current Line number
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_()  cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdem_TableImp Obj;
typedef char DescriptorType;
typedef bdem_ElemType ET;
typedef bdem_AggregateOption::AllocationStrategy Strategy;


const bdem_ElemType::Type DESCRIPTOR_TYPE_ENUM = bdem_ElemType::BDEM_CHAR;

static const bdem_AggregateOption::AllocationStrategy BDEM_PASS_THROUGH =
             bdem_AggregateOption::BDEM_PASS_THROUGH;

static const bdem_AggregateOption::AllocationStrategy BDEM_WRITE_MANY =
             bdem_AggregateOption::BDEM_WRITE_MANY;

static const bdem_AggregateOption::AllocationStrategy BDEM_WRITE_ONCE =
             bdem_AggregateOption::BDEM_WRITE_ONCE;

static const char        CHAR_VALUE1   = '3';
static const char        CHAR_VALUE2   = '4';
static const char        UNSET_CHAR   = bdetu_Unset<char>::unsetValue();
static const int         INT_VALUE1    = 3;
static const int         INT_VALUE2    = 4;
static const int         UNSET_INT    = bdetu_Unset<int>::unsetValue();
static const bsl::string STRING_VALUE1 = "TEST";
static const bsl::string STRING_VALUE2 = "test";
static const bsl::string UNSET_STRING = bdetu_Unset<bsl::string>::unsetValue();

//=============================================================================
//                        GLOBAL FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// so 'bdem_TableImp's can be printed out by LOOP_ASSERT
template <class STREAM>
STREAM& operator<<(STREAM& stream, const Obj& obj) {
    obj.print(stream, 0, -1);
    return stream;
}

// helps make up for the lack of 'operator[]'
static
bdem_ConstElemRef getElemRef(const bdem_TableImp& table, int row, int col) {
    const bdem_RowData *TRH = &table.theRow(row);

    return TRH->elemRef(col);
}

// helps make up for the lack of 'operator[]'
static
bdem_ElemRef getModElemRef(bdem_TableImp *table, int row, int col) {
    bdem_RowData& trh = table->theModifiableRow(row);

    return trh.elemRef(col);
}

bool compareTablesWithoutNullnessBits(const bdem_TableImp& lhs,
                                      const bdem_TableImp& rhs)
    // Compare the data stored by the specified 'lhs' and 'rhs' tables while
    // ignoring their respective nullness bits, and return 'true' if they are
    // equal and 'false' otherwise.
{
    if (&lhs == &rhs) {
        return true;                                                  // RETURN
    }

    int numCols = lhs.numColumns();
    if (numCols != rhs.numColumns()) {
        return false;                                                 // RETURN
    }

    int numRows = lhs.numRows();
    if (numRows != rhs.numRows()) {
        return false;                                                 // RETURN
    }

    for (int i = 0; i < numRows; ++i) {
        const bdem_RowData& lhsRowHdr = lhs.theRow(i);
        const bdem_RowData& rhsRowHdr = rhs.theRow(i);
        for (int j = 0; j < numCols; ++j) {
            const bdem_Descriptor *lhsDesc =
                                      (*lhsRowHdr.rowLayout())[j].attributes();
            const bdem_Descriptor *rhsDesc =
                                      (*rhsRowHdr.rowLayout())[j].attributes();
            if (lhsDesc != rhsDesc) {
                return false;                                         // RETURN
            }

            const bdem_ConstElemRef  lhsERef = lhsRowHdr.elemRef(j);
            const bdem_ConstElemRef  rhsERef = rhsRowHdr.elemRef(j);
            if (!lhsDesc->areEqual(lhsERef.data(), rhsERef.data())) {
                return false;                                         // RETURN
            }
        }
    }

    return true;
}

#if 0
// This routine was used to generate the fixed bdex input examples.  Keep it
// around in case we want to generate more some day.
static
void hexDump(const void *memory_arg, int length) {
    const unsigned char *memory = (const unsigned char *) memory_arg;

    int i = 0;
    printf("\"");
    for (; i < length; ++i) {
        if (i && 0 == i % 15) {
            printf("\"\n\"");
        }
        printf("\\x%02x", (int) *memory++);
    }
    printf("\"\n");
}
#endif

static
void stringDump(const void *memory_arg, int length) {
    const unsigned char *memory = (const unsigned char *) memory_arg;

    int i = 0;
    printf("\"");
    for (; i < length; ++i) {
        unsigned char c = *memory++;
        if (c == '\n') {
            printf("\\n\"\n\"");
        }
        else {
            if (c < 0x20 || c >= 0x7f) {
                printf("\\x%02x", (int) c);
            }
            else {
                printf("%c", c);
            }
        }
    }
    printf("\"\n");
}

static
void fillTable(bdem_TableImp *table, int pattern) {
    const int numRows    = table->numRows();
    const int numColumns = table->numColumns();

    for (int row = 0; row < numRows; ++row) {
        bdem_RowData *trh = &table->theModifiableRow(row);

        if (5 == pattern) {
            table->makeRowsNull(row, 1);
            continue;
        }

        for (int col = 0; col < numColumns; ++col) {
            int val = (pattern + row + col) % 4;

            if (0 == val || 1 == val) {
                trh->elemRef(col).makeNull();
            }
            else {
                switch (col % 3) {
                  case 0: {
                    ASSERT(ET::BDEM_INT == table->columnType(col));
                    trh->elemRef(col).theModifiableInt() =
                                            val == 2 ? INT_VALUE1 : INT_VALUE2;
                  } break;
                  case 1: {
                    ASSERT(ET::BDEM_STRING == table->columnType(col));
                    trh->elemRef(col).theModifiableString() =
                                      val == 2 ? STRING_VALUE1 : STRING_VALUE2;
                  } break;
                  case 2: {
                    ASSERT(ET::BDEM_CHAR == table->columnType(col));
                    trh->elemRef(col).theModifiableChar() =
                                          val == 2 ? CHAR_VALUE1 : CHAR_VALUE2;
                  } break;
                  default: {
                    ASSERT(0);
                  }
                }
            }
        }
    }
}

bool isUnset(const bdem_ConstElemRef& ref)
    // Return 'true' if the specified element 'ref' references an element that
    // has the "unset" value for its type, and 'false' otherwise.
{
    return ref.descriptor()->isUnset(ref.data());
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    const bdem_Descriptor* ATTR_LOOKUP_TBL[] = {
        &bdem_Properties::d_charAttr,
        &bdem_Properties::d_shortAttr,
        &bdem_Properties::d_intAttr,
        &bdem_Properties::d_int64Attr,
        &bdem_Properties::d_floatAttr,
        &bdem_Properties::d_doubleAttr,
        &bdem_Properties::d_stringAttr,
        &bdem_Properties::d_datetimeAttr,
        &bdem_Properties::d_dateAttr,
        &bdem_Properties::d_timeAttr,
        &bdem_Properties::d_charArrayAttr,
        &bdem_Properties::d_shortArrayAttr,
        &bdem_Properties::d_intArrayAttr,
        &bdem_Properties::d_int64ArrayAttr,
        &bdem_Properties::d_floatArrayAttr,
        &bdem_Properties::d_doubleArrayAttr,
        &bdem_Properties::d_stringArrayAttr,
        &bdem_Properties::d_datetimeArrayAttr,
        &bdem_Properties::d_dateArrayAttr,
        &bdem_Properties::d_timeArrayAttr
    };

    typedef bdex_TestInStream In;
    bdem_DescriptorStreamIn<In> STREAMIN_TABLE_TEST[] = {
        { &bdem_FunctionTemplates::streamInFundamental<char,In> },
        { &bdem_FunctionTemplates::streamInFundamental<short,In> },
        { &bdem_FunctionTemplates::streamInFundamental<int,In> },
        { &bdem_FunctionTemplates::
                                   streamInFundamental<bsls_Types::Int64,In> },
        { &bdem_FunctionTemplates::streamInFundamental<float,In> },
        { &bdem_FunctionTemplates::streamInFundamental<double,In> },
        { &bdem_FunctionTemplates::streamInFundamental<bsl::string,In> },
        { &bdem_FunctionTemplates::streamIn<bdet_Datetime,In> },
        { &bdem_FunctionTemplates::streamIn<bdet_Date,In> },
        { &bdem_FunctionTemplates::streamIn<bdet_Time,In> },
        { &bdem_FunctionTemplates::streamInArray<char,In> },
        { &bdem_FunctionTemplates::streamInArray<short,In> },
        { &bdem_FunctionTemplates::streamInArray<int,In> },
        { &bdem_FunctionTemplates::streamInArray<bsls_Types::Int64,In>},
        { &bdem_FunctionTemplates::streamInArray<float,In> },
        { &bdem_FunctionTemplates::streamInArray<double,In> },
        { &bdem_FunctionTemplates::streamInArray<bsl::string,In> },
        { &bdem_FunctionTemplates::streamInArray<bdet_Datetime,In> },
        { &bdem_FunctionTemplates::streamInArray<bdet_Date,In> },
        { &bdem_FunctionTemplates::streamInArray<bdet_Time, In> }
    };

    typedef bdex_ByteInStream In2;
    bdem_DescriptorStreamIn<In2> STREAMIN_TABLE_BYTE[] = {
        { &bdem_FunctionTemplates::streamInFundamental<char,In2> },
        { &bdem_FunctionTemplates::streamInFundamental<short,In2> },
        { &bdem_FunctionTemplates::streamInFundamental<int,In2> },
        { &bdem_FunctionTemplates::
                                  streamInFundamental<bsls_Types::Int64,In2> },
        { &bdem_FunctionTemplates::streamInFundamental<float,In2> },
        { &bdem_FunctionTemplates::streamInFundamental<double,In2> },
        { &bdem_FunctionTemplates::streamInFundamental<bsl::string,In2> },
        { &bdem_FunctionTemplates::streamIn<bdet_Datetime,In2> },
        { &bdem_FunctionTemplates::streamIn<bdet_Date,In2> },
        { &bdem_FunctionTemplates::streamIn<bdet_Time,In2> },
        { &bdem_FunctionTemplates::streamInArray<char,In2> },
        { &bdem_FunctionTemplates::streamInArray<short,In2> },
        { &bdem_FunctionTemplates::streamInArray<int,In2> },
        { &bdem_FunctionTemplates::streamInArray<bsls_Types::Int64,In2> },
        { &bdem_FunctionTemplates::streamInArray<float,In2> },
        { &bdem_FunctionTemplates::streamInArray<double,In2> },
        { &bdem_FunctionTemplates::streamInArray<bsl::string,In2> },
        { &bdem_FunctionTemplates::streamInArray<bdet_Datetime,In2> },
        { &bdem_FunctionTemplates::streamInArray<bdet_Date,In2> },
        { &bdem_FunctionTemplates::streamInArray<bdet_Time, In2> }
    };

    typedef bdex_TestOutStream Out;
    bdem_DescriptorStreamOut<Out> STREAMOUT_TABLE_TEST[] = {
        { &bdem_FunctionTemplates::streamOutFundamental<char,Out> },
        { &bdem_FunctionTemplates::streamOutFundamental<short,Out> },
        { &bdem_FunctionTemplates::streamOutFundamental<int,Out> },
        { &bdem_FunctionTemplates::
                                 streamOutFundamental<bsls_Types::Int64,Out> },
        { &bdem_FunctionTemplates::streamOutFundamental<float,Out> },
        { &bdem_FunctionTemplates::streamOutFundamental<double,Out> },
        { &bdem_FunctionTemplates::streamOutFundamental<bsl::string,Out> },
        { &bdem_FunctionTemplates::streamOut<bdet_Datetime,Out> },
        { &bdem_FunctionTemplates::streamOut<bdet_Date,Out> },
        { &bdem_FunctionTemplates::streamOut<bdet_Time,Out> },
        { &bdem_FunctionTemplates::streamOutArray<char,Out> },
        { &bdem_FunctionTemplates::streamOutArray<short,Out> },
        { &bdem_FunctionTemplates::streamOutArray<int,Out> },
        { &bdem_FunctionTemplates::streamOutArray<bsls_Types::Int64,Out> },
        { &bdem_FunctionTemplates::streamOutArray<float,Out> },
        { &bdem_FunctionTemplates::streamOutArray<double,Out> },
        { &bdem_FunctionTemplates::streamOutArray<bsl::string,Out> },
        { &bdem_FunctionTemplates::streamOutArray<bdet_Datetime,Out> },
        { &bdem_FunctionTemplates::streamOutArray<bdet_Date,Out> },
        { &bdem_FunctionTemplates::streamOutArray<bdet_Time, Out> }
    };

#if 0
    // This was used to streamout the examples in the bdex streaming
    // test case.  Keep it around in case we ever want to stream out
    // more examples.

    typedef bdex_ByteOutStream Out2;
    bdem_DescriptorStreamOut<Out2> STREAMOUT_TABLE_BYTE[] = {
        { &bdem_FunctionTemplates::streamOutFundamental<char,Out2> },
        { &bdem_FunctionTemplates::streamOutFundamental<short,Out2> },
        { &bdem_FunctionTemplates::streamOutFundamental<int,Out2> },
        { &bdem_FunctionTemplates::
                                streamOutFundamental<bsls_Types::Int64,Out2> },
        { &bdem_FunctionTemplates::streamOutFundamental<float,Out2> },
        { &bdem_FunctionTemplates::streamOutFundamental<double,Out2> },
        { &bdem_FunctionTemplates::streamOutFundamental<bsl::string,Out2> },
        { &bdem_FunctionTemplates::streamOut<bdet_Datetime,Out2> },
        { &bdem_FunctionTemplates::streamOut<bdet_Date,Out2> },
        { &bdem_FunctionTemplates::streamOut<bdet_Time,Out2> },
        { &bdem_FunctionTemplates::streamOutArray<char,Out2> },
        { &bdem_FunctionTemplates::streamOutArray<short,Out2> },
        { &bdem_FunctionTemplates::streamOutArray<int,Out2> },
        { &bdem_FunctionTemplates::streamOutArray<bsls_Types::Int64,Out2> },
        { &bdem_FunctionTemplates::streamOutArray<float,Out2> },
        { &bdem_FunctionTemplates::streamOutArray<double,Out2> },
        { &bdem_FunctionTemplates::streamOutArray<bsl::string,Out2> },
        { &bdem_FunctionTemplates::streamOutArray<bdet_Datetime,Out2> },
        { &bdem_FunctionTemplates::streamOutArray<bdet_Date,Out2> },
        { &bdem_FunctionTemplates::streamOutArray<bdet_Time, Out2> }
    };
#endif

    switch (test) { case 0:  // Zero is always the leading case.
      case 13: {
        // --------------------------------------------------------------------
        // PRIVATE MEMORY GROWTH (Temporary)
        //
        // Concerns:
        //: 1 Geometric memory growth is disabled by default.
        //: 2 When geometric memory growth is enabled, memory actually grows
        //:   geometrically.
        //: 3 When geometric memory growth is disabled, memory grows linearly.
        //: 4 Disabling geometric memory growth when enabled restores the
        //:   original behavior.
        //: 5 Disabling geometric memory growth when already disables has no
        //:   effect on memory growth.
        //
        // Plan:
        //: 1 Insert rows and verify that, by default, capacity does not grow
        //:   geometrically.  [C-1,3]
        //: 2 Disable geometric growth with
        //:   'bdem_TableImp_disableGeometricMemoryGrowth' and verify that the
        //:    memory does not grow geometrically for progressive insertions.
        //:    [C-1,3,5]
        //: 3 Enable geometric growth with
        //:   'bdem_TableImp_enableGeometricMemoryGrowth' and verify that
        //:   the capacity of the table grows geometrically for progressive
        //:   insertions.  [C-2]
        //: 4 Disable geometric growth with
        //:   'bdem_TableImp_disableGeometricMemoryGrowth' and verify that the
        //:    memory does not grow geometrically for progressive insertions.
        //:    [C-1,3,4]
        //
        //  Testing:
        //    bdem_TableImp_enableGeometricMemoryGrowth();
        //    bdem_TableImp_disableGeometricMemoryGrowth();
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING PRIVATE MEMORY GROWTH FLAG" << endl
                          << "==================================" << endl;

        static const Strategy STRATEGY_DATA[] = {
                BDEM_PASS_THROUGH,
                BDEM_WRITE_ONCE,
                BDEM_WRITE_MANY
        };
        enum { STRATEGY_LEN = sizeof(STRATEGY_DATA) / sizeof(*STRATEGY_DATA) };

        const static struct {
               const int d_position;          // single row position
               const int d_expectedCapacity;  // expected capacity
         } DATA[] = { // POS   EXP_CAPACITY
                      // ---   ------------
                          0,             1,
                          1,             2,
                          2,             4,
                          3,             4,
                          4,             8,
                          5,             8,
                          6,             8,
                          7,             8,
                          8,            16,
                          9,            16,
                         10,            16,
                         11,            16,
                         12,            16,
                         13,            16,
                         14,            16,
                         15,            16,
                         16,            32,
                         17,            32,
                         18,            32,
                         19,            32, };

        enum { DATA_LEN = sizeof(DATA) / sizeof(*DATA) };

        if (verbose) cout << "\nTesting the default memory growth" << endl;
        {
            const int NUM_ROWS = 1024;

            for (int i = 0; i < STRATEGY_LEN; ++i) {
                bslma_TestAllocator ta("TestAllocator", veryVeryVeryVerbose);
                Obj mX(STRATEGY_DATA[i], &ta); const Obj& X = mX;

                for (int j = 0; j < NUM_ROWS; ++j) {
                    mX.insertNullRows(j, 1);
                    LOOP2_ASSERT(j,
                                 X.capacityRaw(),
                                 j + 1 == X.capacityRaw());
                }
            }
        }

        if (verbose) cout << "\nDisabling geometric memory growth" << endl;
        {
            bdem_TableImp_disableGeometricMemoryGrowth();
            const int NUM_ROWS = 1024;

            for (int i = 0; i < STRATEGY_LEN; ++i) {
                bslma_TestAllocator ta("TestAllocator", veryVeryVeryVerbose);
                Obj mX(STRATEGY_DATA[i], &ta); const Obj& X = mX;

                for (int j = 0; j < NUM_ROWS; ++j) {
                    mX.insertNullRows(j, 1);
                    LOOP2_ASSERT(j,
                                 X.capacityRaw(),
                                 j + 1 == X.capacityRaw());
                }
            }
        }

        if (verbose) cout << "\nEnabling geometric memory growth" << endl;
        {
            bdem_TableImp_enableGeometricMemoryGrowth();

            for (int i = 0; i < STRATEGY_LEN; ++i) {

                bslma_TestAllocator ta("TestAllocator", veryVeryVeryVerbose);
                Obj mX(STRATEGY_DATA[i], &ta); const Obj& X = mX;

                for (int j = 0; j < DATA_LEN; j++) {
                    const int POSITION     = DATA[j].d_position;
                    const int EXP_CAPACITY = DATA[j].d_expectedCapacity;
                    mX.insertNullRows(POSITION, 1);
                    LOOP4_ASSERT(i,
                                 j,
                                 EXP_CAPACITY,
                                 X.capacityRaw(),
                                 EXP_CAPACITY == X.capacityRaw());
                }
            }
        }
        if (verbose) cout << "\nTesting with reserve" << endl;
        {
            for (int i = 0; i < STRATEGY_LEN; ++i) {
                for (int j = 0; j < DATA_LEN; j++) {
                    bslma_TestAllocator ta("TestAllocator",
                                           veryVeryVeryVerbose);
                    Obj mX(STRATEGY_DATA[i], &ta); const Obj& X = mX;
                    mX.reserveRaw(j + 1);
                    LOOP3_ASSERT(i,
                                 j,
                                 X.capacityRaw(),
                                 j + 1 == X.capacityRaw());
                }
            }
        }

        if (verbose) cout << "\nDisabling geometric memory growth" << endl;
        {
            bdem_TableImp_disableGeometricMemoryGrowth();
            const int NUM_ROWS = 1024;

            for (int i = 0; i < STRATEGY_LEN; ++i) {
                bslma_TestAllocator ta("TestAllocator", veryVeryVeryVerbose);
                Obj mX(STRATEGY_DATA[i], &ta); const Obj& X = mX;

                for (int j = 0; j < NUM_ROWS; ++j) {
                    mX.insertNullRows(j, 1);
                    LOOP2_ASSERT(j,
                                 X.capacityRaw(),
                                 j + 1 == X.capacityRaw());
                }
            }

        }
        if (verbose) cout << "\nTesting with reserve" << endl;
        {
            for (int i = 0; i < STRATEGY_LEN; ++i) {
                for (int j = 0; j < DATA_LEN; j++) {
                    bslma_TestAllocator ta("TestAllocator",
                                           veryVeryVeryVerbose);
                    Obj mX(STRATEGY_DATA[i], &ta); const Obj& X = mX;
                    mX.reserveRaw(j + 1);
                    LOOP3_ASSERT(i,
                                 j,
                                 X.capacityRaw(),
                                 j + 1 == X.capacityRaw());
                }
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // 'capacityRaw' METHOD
        //
        // Concerns:
        //: 1 Reserving memory actually causes capacity to change consistently.
        //: 2 Inserting rows less in number than difference between the rows
        //:   contained in the table and its capacity, does not alter capacity.
        //: 3 Inserting more rows than 'capacity' makes 'capacity' increase.
        //
        // Plan:
        //: 1 Verify that the 'capacityRaw' method returns the exact
        //:   capacity reserved through 'reserveRaw' for progressive
        //:   calls of 'reserveRaw'.
        //: 2 Verify that  the 'capacityRaw' method returns the exact
        //:   amount of rows inserted via 'insertNullRows', for progressive
        //:   insertions of rows.
        //
        // Testing:
        //   bsl::size_t capacityRaw() const
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING: 'capacityRaw'" << endl
                          << "======================" << endl;

        static const Strategy STRATEGY_DATA[] = {
            BDEM_PASS_THROUGH,
            BDEM_WRITE_ONCE,
            BDEM_WRITE_MANY
        };
        enum { STRATEGY_LEN = sizeof(STRATEGY_DATA) / sizeof(*STRATEGY_DATA) };

        const int MAX_NUM_ROWS = 1024;  // power of 2

        if (veryVerbose) cout << "\tTesting all allocation strategies\n"
                              << endl;

        for (int i = 0; i < STRATEGY_LEN; ++i) {

            const Strategy STRATEGY = STRATEGY_DATA[i];

            if (verbose) cout << "\nTesting using 'reserveRaw'" << endl;

            for (int j = 1; j <= MAX_NUM_ROWS; j <<= 1) {

                const int EXPECTED_CAPACITY = j;
                if (veryVerbose) {  P(i) P_(j) }
                 bslma_TestAllocator ta("TestAllocator",
                                        veryVeryVeryVerbose);

                 Obj mX(STRATEGY, &ta); const Obj& X = mX;
                 ASSERT(0 == X.capacityRaw());

                 if (veryVeryVerbose) cout << "\t\tReserving memory\n" << endl;
                 mX.reserveRaw(EXPECTED_CAPACITY);
                 LOOP2_ASSERT(EXPECTED_CAPACITY,
                              X.capacityRaw(),
                              EXPECTED_CAPACITY == X.capacityRaw());
            }

            if (verbose) cout << "\nTesting using 'insertNullRows'" << endl;
            for (int j = 1; j <= MAX_NUM_ROWS; j <<= 1) {

                const int EXPECTED_CAPACITY = j;
                if (veryVerbose) { P(i) P_(j) }

                Obj mX(STRATEGY); const Obj& X = mX;
                ASSERT(0 == X.capacityRaw());

                if (veryVeryVerbose) cout << "\t\tInserting rows.\n" << endl;
                for (int k = 0;
                         k < MAX_NUM_ROWS;
                         k += j) {
                    mX.insertNullRows(k, j);
                    const int EXPECTED_CAPACITY = k + j;
                    LOOP2_ASSERT(EXPECTED_CAPACITY,
                                 X.capacityRaw(),
                                 EXPECTED_CAPACITY == X.capacityRaw());
                }
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // 'reserveRaw' METHOD
        //
        // Concerns:
        //: 1 Enough memory is reserved to minimize the allocation upon calls
        //:   of 'insertNullRows'.
        //: 2 No allocation is performed by the object allocator, when
        //:   inserting rows that were previously reserved, minus the minimal
        //:   allocation that cannot be avoided.
        //
        // Plan:
        //: 1 Verify that inserting rows in different chunk sizes, after enough
        //:   memory is reserved through 'reserveRaw' does not allocate
        //:   any extra memory.
        //
        // Testing:
        //   void reserveRaw(bsl::size_t numRows);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING: 'reserveRaw'" << endl
                          << "=====================" << endl;

        typedef bdem_AggregateOption::AllocationStrategy Strategy ;

        static
        const Strategy STRATEGY_DATA[] = {
            BDEM_PASS_THROUGH,
            BDEM_WRITE_ONCE,
            BDEM_WRITE_MANY
        };

        enum { STRATEGY_LEN = sizeof(STRATEGY_DATA) / sizeof(*STRATEGY_DATA) };

        const int MAX_NUM_ROWS = 4096;  // power of 2

        if (verbose) cout << "\nTesting without geometric memory growth\n";
        {
            if (veryVerbose) cout << "\tTesting Strategies\n" << endl;

            for (int i = 0; i < STRATEGY_LEN; ++i) {

                const Strategy STRATEGY = STRATEGY_DATA[i];

                if (verbose) cout << "Testing 'reserveRaw(0)'" << endl;
                {
                    bslma_TestAllocator ta("TestAllocator",
                                            veryVeryVeryVerbose);

                    Obj mX(STRATEGY, &ta); const Obj& X = mX;
                    const size_t NUM_BYTES = ta.numBytesMax();
                    mX.reserveRaw(0);
                    ASSERT(ta.numBytesMax() == NUM_BYTES);
                }

                if (veryVerbose) cout << "\tInserting Rows\n" << endl;

                for (int j = 1; j <= MAX_NUM_ROWS; j <<= 1) {

                    if (veryVerbose) {  P(i) P_(j) }
                     bslma_TestAllocator ta("TestAllocator",
                                            veryVeryVeryVerbose);

                     Obj mX(STRATEGY, &ta); const Obj& X = mX;

                     if (veryVeryVerbose) {
                         cout << "\t\tReserving memory\n" << endl;
                     }
                     mX.reserveRaw(MAX_NUM_ROWS);

                     size_t NUM_BYTES = ta.numBytesMax();
                     if (BDEM_PASS_THROUGH == STRATEGY) {

                         // Add extra memory for Pass Through - 'bdem_RowData'
                         // allocate extra pointers when inserting null rows.

                         NUM_BYTES += bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT
                                      * 2 * MAX_NUM_ROWS;
                     }

                     for (int k = 0;
                              k < MAX_NUM_ROWS;
                              k += j) {
                         mX.insertNullRows(k, j);
                     }

                     // Do not execute this test on 64-bit platforms for
                     // 'BDEM_WRITE_MANY'.  'bdema_AllocatorManager' has a
                     // broken behavior, the memory gets reserved in the first
                     // pool: if you ask for a different size, it will allocate
                     // again.

#ifdef BSLS_PLATFORM_CPU_64_BIT
                     if(BDEM_WRITE_MANY == STRATEGY) {
                         continue;
                     }
#endif
                     LOOP4_ASSERT(i,
                                  j,
                                  ta.numBytesMax(),
                                  NUM_BYTES,
                                  ta.numBytesMax() <= NUM_BYTES);
                 }
            }
        }

        if (verbose) cout << "\nTesting with geometric memory growth" << endl;
        {
            bdem_TableImp_enableGeometricMemoryGrowth();

            if (veryVerbose) cout << "\tTesting Strategies\n" << endl;

            for (int i = 0; i < STRATEGY_LEN; ++i) {

                const Strategy STRATEGY = STRATEGY_DATA[i];

                if (verbose) cout << "Testing 'reserveRaw(0)'" << endl;
                {
                    bslma_TestAllocator ta("TestAllocator",
                                            veryVeryVeryVerbose);

                    Obj mX(STRATEGY, &ta); const Obj& X = mX;
                    const size_t NUM_BYTES = ta.numBytesMax();
                    mX.reserveRaw(0);
                    ASSERT(ta.numBytesMax() == NUM_BYTES);
                }

                if (veryVerbose) cout << "\tInserting Rows\n" << endl;

                for (int j = 1; j <= MAX_NUM_ROWS; j <<= 1) {

                    if (veryVerbose) {  P(i) P_(j) }
                     bslma_TestAllocator ta("TestAllocator",
                                            veryVeryVeryVerbose);

                     Obj mX(STRATEGY, &ta); const Obj& X = mX;

                     if (veryVeryVerbose) {
                          cout << "\t\tReserving memory\n" << endl;
                     }
                     mX.reserveRaw(MAX_NUM_ROWS);

                     size_t NUM_BYTES = ta.numBytesMax();
                     if (BDEM_PASS_THROUGH == STRATEGY) {

                         // Add extra memory for Pass Through - 'bdem_RowData'
                         // allocate extra pointers when inserting null rows.

                         NUM_BYTES += bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT
                                      * 2 * MAX_NUM_ROWS;
                     }

                     for (int k = 0;
                              k < MAX_NUM_ROWS;
                              k += j) {
                         mX.insertNullRows(k, j);
                     }

                     // Do not execute this test on 64-bit platforms for
                     // 'BDEM_WRITE_MANY'.  'bdema_AllocatorManager' has a
                     // broken behavior, the memory gets reserved in the first
                     // pool: if you ask for a different size, it will allocate
                     // again.

#ifdef BSLS_PLATFORM_CPU_64_BIT
                     if(BDEM_WRITE_MANY == STRATEGY) {
                         continue;
                     }
#endif
                     LOOP4_ASSERT(i,
                                  j,
                                  ta.numBytesMax(),
                                  NUM_BYTES,
                                  ta.numBytesMax() <= NUM_BYTES);
                  }
            }
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING PRINT MEMBER FUNCTION
        //
        // Concerns:
        //   That the print function prints TableImp's as expected.
        //
        // Plan:
        //   Using a Stored set of expected outputs from printing out tables in
        //   'expected' strings, create tables from (rows, columns, pattern)
        //   data and print them out to ostrstreams twice, once with
        //   spacesPerLevel of 4, once with spacePerLevel of -1, and verify
        //   the outputs created are what we expect.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING print\n"
                             "=============\n";

        bslma_TestAllocator allocator, *Z = &allocator;

        ET::Type COL_TYPES[] = {
            ET::BDEM_INT, ET::BDEM_STRING, ET::BDEM_CHAR
        };

        struct {
            int         d_line;
            int         d_rows;
            int         d_columns;
            int         d_pattern;
            const char *d_expectedMultiLine;
            const char *d_expectedSingleLine;
        } DATA[] = {
            { L_, 0, 0, 0,
                "    {\n"
                "        Column Types: [\n"
                "        ]\n"
                "    }\n" ,
                " { Column Types: [ ] }" },
            { L_, 0, 3, 0,
                "    {\n"
                "        Column Types: [\n"
                "            INT\n"
                "            STRING\n"
                "            CHAR\n"
                "        ]\n"
                "    }\n" ,
                " { Column Types: [ INT STRING CHAR ] }" },
            { L_, 1, 1, 0,
                "    {\n"
                "        Column Types: [\n"
                "            INT\n"
                "        ]\n"
                "        Row 0: {\n"
                "            NULL\n"
                "        }\n"
                "    }\n" ,
                " { Column Types: [ INT ] Row 0: { NULL } }" },
            { L_, 1, 1, 1,
                "    {\n"
                "        Column Types: [\n"
                "            INT\n"
                "        ]\n"
                "        Row 0: {\n"
                "            NULL\n"
                "        }\n"
                "    }\n" ,
                " { Column Types: [ INT ] Row 0: { NULL } }" },
            { L_, 1, 1, 2,
                "    {\n"
                "        Column Types: [\n"
                "            INT\n"
                "        ]\n"
                "        Row 0: {\n"
                "            3\n"
                "        }\n"
                "    }\n" ,
                " { Column Types: [ INT ] Row 0: { 3 } }" },
            { L_, 1, 1, 3,
                "    {\n"
                "        Column Types: [\n"
                "            INT\n"
                "        ]\n"
                "        Row 0: {\n"
                "            4\n"
                "        }\n"
                "    }\n" ,
                " { Column Types: [ INT ] Row 0: { 4 } }" },
            { L_, 1, 1, 5,
                "    {\n"
                "        Column Types: [\n"
                "            INT\n"
                "        ]\n"
                "        Row 0: NULL"
                "    }\n" ,
                " { Column Types: [ INT ] Row 0: NULL }" },
            { L_, 1, 3, 0,
                "    {\n"
                "        Column Types: [\n"
                "            INT\n"
                "            STRING\n"
                "            CHAR\n"
                "        ]\n"
                "        Row 0: {\n"
                "            NULL\n"
                "            NULL\n"
                "            3\n"
                "        }\n"
                "    }\n" ,
                " { Column Types: [ INT STRING CHAR ] "
                                                  "Row 0: { NULL NULL 3 } }" },
            { L_, 2, 2, 0,
                "    {\n"
                "        Column Types: [\n"
                "            INT\n"
                "            STRING\n"
                "        ]\n"
                "        Row 0: {\n"
                "            NULL\n"
                "            NULL\n"
                "        }\n"
                "        Row 1: {\n"
                "            NULL\n"
                "            TEST\n"
                "        }\n"
                "    }\n" ,
                " { Column Types: [ INT STRING ] Row 0: { NULL NULL } "
                                             "Row 1: { NULL TEST } }" },
            { L_, 2, 2, 1,
                "    {\n"
                "        Column Types: [\n"
                "            INT\n"
                "            STRING\n"
                "        ]\n"
                "        Row 0: {\n"
                "            NULL\n"
                "            TEST\n"
                "        }\n"
                "        Row 1: {\n"
                "            3\n"
                "            test\n"
                "        }\n"
                "    }\n" ,
                " { Column Types: [ INT STRING ] Row 0: { NULL TEST } "
                                                       "Row 1: { 3 test } }" },
            { L_, 2, 2, 2,
                "    {\n"
                "        Column Types: [\n"
                "            INT\n"
                "            STRING\n"
                "        ]\n"
                "        Row 0: {\n"
                "            3\n"
                "            test\n"
                "        }\n"
                "        Row 1: {\n"
                "            4\n"
                "            NULL\n"
                "        }\n"
                "    }\n" ,
                " { Column Types: [ INT STRING ] Row 0: { 3 test } "
                                                       "Row 1: { 4 NULL } }" },
            { L_, 2, 2, 3,
                "    {\n"
                "        Column Types: [\n"
                "            INT\n"
                "            STRING\n"
                "        ]\n"
                "        Row 0: {\n"
                "            4\n"
                "            NULL\n"
                "        }\n"
                "        Row 1: {\n"
                "            NULL\n"
                "            NULL\n"
                "        }\n"
                "    }\n" ,
                " { Column Types: [ INT STRING ] Row 0: { 4 NULL } "
                                                    "Row 1: { NULL NULL } }" },
            { L_, 2, 3, 0,
                "    {\n"
                "        Column Types: [\n"
                "            INT\n"
                "            STRING\n"
                "            CHAR\n"
                "        ]\n"
                "        Row 0: {\n"
                "            NULL\n"
                "            NULL\n"
                "            3\n"
                "        }\n"
                "        Row 1: {\n"
                "            NULL\n"
                "            TEST\n"
                "            4\n"
                "        }\n"
                "    }\n",
                " { Column Types: [ INT STRING CHAR ] Row 0: { NULL NULL 3 } "
                                         "Row 1: { NULL TEST 4 } }" } };

        enum { DATA_LEN = sizeof(DATA) / sizeof(*DATA),
               BUF_LEN = 10000 };

        for (int i = 0; i < DATA_LEN; ++i) {
            const int LINE          = DATA[i].d_line;
            const int ROWS          = DATA[i].d_rows;
            const int COLUMNS       = DATA[i].d_columns;
            const int PATTERN       = DATA[i].d_pattern;
            const char * const EXPECTED_MULTI_LINE =
                                      DATA[i].d_expectedMultiLine;
            const char * const EXPECTED_SINGLE_LINE =
                                      DATA[i].d_expectedSingleLine;

            Obj table(COL_TYPES,
                      COLUMNS,
                      ATTR_LOOKUP_TBL,
                      BDEM_PASS_THROUGH,
                      Z);

            const Obj& TABLE = table;

            table.insertNullRows(0, ROWS);
            ASSERT(table.numRows() == ROWS);
            ASSERT(table.numColumns() == COLUMNS);

            fillTable(&table, PATTERN);

            char buf[BUF_LEN];

            {
                buf[BUF_LEN - 1] = 1;
                ostrstream out(buf, BUF_LEN);
                table.print(out, 1, 4) << ends;
                ASSERT(1 == buf[BUF_LEN - 1]);

                LOOP_ASSERT(LINE, strlen(buf) == strlen(EXPECTED_MULTI_LINE));
                LOOP3_ASSERT(LINE, buf, EXPECTED_MULTI_LINE,
                             0 == memcmp(buf, EXPECTED_MULTI_LINE,
                                                                 strlen(buf)));

                if (veryVerbose) {
                    cout << "(" << ROWS << " x " << COLUMNS <<
                                             "), pattern: " << PATTERN << endl;

                    stringDump(buf, strlen(buf));
                }
            }

            {
                buf[BUF_LEN - 1] = 1;
                ostrstream out(buf, BUF_LEN);
                table.print(out, 1, -1) << ends;
                ASSERT(1 == buf[BUF_LEN - 1]);

                LOOP_ASSERT(LINE, strlen(buf) == strlen(EXPECTED_SINGLE_LINE));
                LOOP_ASSERT(LINE, 0 == memcmp(buf, EXPECTED_SINGLE_LINE,
                                                                 strlen(buf)));

                if (veryVerbose) {
                    cout << "(" << ROWS << " x " << COLUMNS <<
                                             "), pattern: " << PATTERN << endl;

                    stringDump(buf, strlen(buf));
                }
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING BDEX STREAMS ACROSS PLATFORMS, INCOMPLETE STREAMS
        //
        // Concerns:
        //   That bdex streams generated on one platform will be correctly
        //   read by all platforms, and that incomplete streams are handled
        //   badly -- if a problem is encountered on the first byte, the
        //   object being read to should not be altered.
        //
        // Plan:
        //   Try streaming in a bunch of streams generated on 32 bit sun,
        //   and also generate the expected table from the corresponding
        //   parameters (rows, columns, pattern) using 'fillTable'.  Iterate
        //   for len from 0 to LENGTH where LENGTH is the length of the
        //   complete stream.  If len == LENGTH verify that the stream is valid
        //   and empty, and that the table input is equivalent to what was
        //   expected.  Partial streams should result in the stream being
        //   invalidated.  If the stream is invalidated or empty immediately
        //   after the version is read, the destination object being read into
        //   should remain unchanged.  The destruction of the destination
        //   object on every loop should assert that object invariants were
        //   preserved.
        // --------------------------------------------------------------------

        if (verbose) cout << "bdex streaming test\n"
                             "===================\n";

        bslma_TestAllocator allocator, *Z = &allocator;

        ET::Type DEST_COL_TYPES[] = {
            ET::BDEM_FLOAT, ET::BDEM_FLOAT, ET::BDEM_STRING
        };

        enum { NUM_DEST_COL_TYPES = sizeof(DEST_COL_TYPES) /
                                                     sizeof(*DEST_COL_TYPES) };

        Obj destOrig(DEST_COL_TYPES,
                     NUM_DEST_COL_TYPES,
                     ATTR_LOOKUP_TBL,
                     BDEM_PASS_THROUGH,
                     Z);

        const Obj& DEST_ORIG = destOrig;
        destOrig.insertNullRows(0, 10);
        destOrig.makeRowsNull(1, 1);
        {
            double floatVal = 17.2;
            string stringVal = "arf";
            for (int row = 2; row < 10; ++row) {
                for (int col = 0; col < 2; ++col) {
                    getModElemRef(&destOrig, row, col).theModifiableFloat() =
                                                                      floatVal;
                    floatVal += 9.1;
                    floatVal /= 2.2;
                }
                getModElemRef(&destOrig, row, 2).theModifiableString() =
                                                                     stringVal;
                char tmpChar = (char) ('a' + row);
                unsigned char constString[] = { tmpChar, 0 };
                stringVal.append((const char *) constString);
                if (stringVal.length() >= 8) {
                    stringVal = stringVal.substr(4, 3);
                }
            }
        }

        ET::Type COL_TYPES[] = {
            ET::BDEM_INT, ET::BDEM_STRING, ET::BDEM_CHAR
        };

// TBD: Uncomment
#if 0
        const int bdexVersion = 2;

        struct {
            int         d_line;
            int         d_rows;
            int         d_columns;
            int         d_pattern;
            int         d_length;
            const char *d_streamData;
        } DATA[] = {
            { L_, 0, 0, 0, 3,
                "\x02\x00\x00" },
            { L_, 0, 3, 0, 6,
                "\x02\x03\x02\x06\x00\x00" },
            { L_, 1, 1, 0, 8,
                "\x02\x01\x02\x01\x80\x00\x00\x00" },
            { L_, 1, 1, 1, 12,
                "\x02\x01\x02\x01\x00\x00\x00\x00\x80\x00\x00\x00" },
            { L_, 1, 1, 2, 12,
                "\x02\x01\x02\x01\x00\x00\x00\x00\x00\x00\x00\x03" },
            { L_, 1, 1, 3, 12,
                "\x02\x01\x02\x01\x00\x00\x00\x00\x00\x00\x00\x04" },
            { L_, 1, 3, 0, 12,
                "\x02\x03\x02\x06\x00\x01\x80\x00\x00\x00\x00\x33" },
            { L_, 2, 2, 0, 23,
                "\x02\x02\x02\x06\x02\x80\x00\x00\x00\x00\x00\x00\x00\x00\x80"
                "\x00\x00\x00\x04\x54\x45\x53\x54" },
            { L_, 2, 2, 1, 31,
                "\x02\x02\x02\x06\x02\x00\x00\x00\x00\x80\x00\x00\x00\x04\x54"
                "\x45\x53\x54\x00\x00\x00\x00\x00\x00\x00\x03\x04\x74\x65\x73"
                "\x74" },
            { L_, 2, 2, 2, 26,
                "\x02\x02\x02\x06\x02\x00\x00\x00\x00\x00\x00\x00\x03\x04\x74"
                "\x65\x73\x74\x40\x00\x00\x00\x00\x00\x00\x04" },
            { L_, 2, 2, 3, 18,
                "\x02\x02\x02\x06\x02\x40\x00\x00\x00\x00\x00\x00\x04\x80\x00"
                "\x00\x00\x00" },
            { L_, 2, 3, 0, 26,
                "\x02\x03\x02\x06\x00\x02\x80\x00\x00\x00\x00\x33\x00\x00\x00"
                "\x00\x80\x00\x00\x00\x04\x54\x45\x53\x54\x34" } };

        enum { DATA_LEN = sizeof(DATA) / sizeof(*DATA) };

        for (int i = 0; i < DATA_LEN; ++i) {
            const int ROWS          = DATA[i].d_rows;
            const int COLUMNS       = DATA[i].d_columns;
            const int PATTERN       = DATA[i].d_pattern;
            const int LENGTH        = DATA[i].d_length;
            const char * const STREAM_DATA =
                                      DATA[i].d_streamData;

            Obj table(COL_TYPES,
                      COLUMNS,
                      ATTR_LOOKUP_TBL,
                      BDEM_PASS_THROUGH,
                      Z);

            const Obj& TABLE = table;

            table.insertNullRows(0, ROWS);
            ASSERT(table.numRows() == ROWS);
            ASSERT(table.numColumns() == COLUMNS);

            fillTable(&table, PATTERN);

            bool success = false;
            for (int len = 0; len <= LENGTH; ++len) {
                bdex_ByteInStream testInStream(STREAM_DATA, len);
                ASSERT(testInStream);
                if (0 == len) ASSERT(testInStream.isEmpty());

                Obj dest(DEST_ORIG, BDEM_PASS_THROUGH, Z);
                const Obj& DEST = dest;

                int version;
                testInStream.getVersion(version);
                if (testInStream) {
                    ASSERT(bdexVersion == version);
                }

                bool badAfterVersion = !testInStream || testInStream.isEmpty();
                dest.bdexStreamInImp(testInStream, bdexVersion,
                                     STREAMIN_TABLE_BYTE, ATTR_LOOKUP_TBL);
                if (LENGTH == len) {
                    ASSERT(testInStream);    ASSERT(testInStream.isEmpty());
                    ASSERT(DEST == TABLE);
                    success = true;
                }
                else {
                    ASSERT(!testInStream);
                }
                if (badAfterVersion) {
                    ASSERT(len < LENGTH);
                    ASSERT(DEST == DEST_ORIG);
                }

                // destructor of dest will assert invariants of object
            }
            ASSERT(success);
        }
#endif
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING BDEX STREAMING OUT AND IN
        //
        // Concerns:
        //   That bdex streaming works properly, and handles exceptions
        //   properly.
        //
        // Plan:
        //   Create a variety of tables with different dimensions, filling
        //   them with different patterns of values using the 'fillTable'
        //   function, then stream them to a test stream, then stream that
        //   stream into a separate table, and verify that the tables are
        //   equivalent afterward.  Use the (BEGIN, END)_BDEX_EXCEPTION_TEST
        //   macros to inject exceptions into the streaming in to verify
        //   that the 'bdexStreamInImp' function is well-behaved on exceptions.
        // --------------------------------------------------------------------

        if (verbose) cout << "bdex streaming test\n"
                             "===================\n";

        bslma_TestAllocator testAllocator, *Z = &testAllocator;

        ET::Type DEST_COL_TYPES[] = {
            ET::BDEM_DOUBLE, ET::BDEM_DOUBLE, ET::BDEM_STRING
        };

        enum { NUM_DEST_COL_TYPES = sizeof(DEST_COL_TYPES) /
                                                     sizeof(*DEST_COL_TYPES) };

        // make destination object that will be assigned to and overwritten
        // by test data
        Obj destOrig(DEST_COL_TYPES,
                     NUM_DEST_COL_TYPES,
                     ATTR_LOOKUP_TBL,
                     BDEM_PASS_THROUGH,
                     Z);
        const Obj& DEST_ORIG = destOrig;
        destOrig.insertNullRows(0, 10);
        destOrig.makeRowsNull(1, 1);
        {
            double doubleVal = 87.3;
            string stringVal = "woof";
            for (int row = 2; row < 10; ++row) {
                for (int col = 0; col < 2; ++col) {
                    getModElemRef(&destOrig, row, col).theModifiableDouble() =
                                                                     doubleVal;
                    doubleVal += 4.7;
                    doubleVal /= 3.1;
                }
                getModElemRef(&destOrig, row, 2).theModifiableString() =
                                                                     stringVal;
                char tmpChar = (char) ('a' + row);
                unsigned char constString[] = { tmpChar, 0 };
                stringVal.append((const char *) constString);
                if (stringVal.length() >= 8) {
                    stringVal = stringVal.substr(4, 3);
                }
            }
        }

        ET::Type COL_TYPES[] = {
            ET::BDEM_INT, ET::BDEM_STRING, ET::BDEM_CHAR,
            ET::BDEM_INT, ET::BDEM_STRING, ET::BDEM_CHAR,
            ET::BDEM_INT, ET::BDEM_STRING, ET::BDEM_CHAR
        };
        enum { NUM_COL_TYPES = sizeof(COL_TYPES) / sizeof(*COL_TYPES) };

        const int bdexVersion = 3;

        for (int version = 1; version <= 3; ++version) {
            for (int rows = 0;
                 rows <= NUM_COL_TYPES;
                 rows += rows < 3 ? 1 : 3) {
                for (int cols = 0;
                     cols <= NUM_COL_TYPES;
                     cols += cols < 3 ? 1 : 3) {
                    for (int pattern = 0; pattern < 4; ++pattern) {
                        Obj table(COL_TYPES,
                                  cols,
                                  ATTR_LOOKUP_TBL,
                                  BDEM_PASS_THROUGH,
                                  Z);
                        const Obj& TABLE = table;

                        table.insertNullRows(0, rows);
                        ASSERT(table.numRows() == rows);
                        ASSERT(table.numColumns() == cols);

                        fillTable(&table, pattern);

                        if (veryVerbose) { P(rows) P(cols)
                                           P(pattern) P(table) }

                        bdex_TestOutStream out;

                        table.bdexStreamOutImp(out,
                                               version,
                                               STREAMOUT_TABLE_TEST);

                        const char *DATA = out.data();
                        int LENGTH = out.length();

                        bdex_TestInStream testInStream(DATA, LENGTH);

                        Obj dest(BDEM_PASS_THROUGH, Z);
                        const Obj& DEST = dest;

                        int passes = 0;
                        testInStream.setSuppressVersionCheck(1);
                        BEGIN_BDEX_EXCEPTION_TEST {
#ifdef BDE_BUILD_TARGET_EXC
                            bdexExceptionLimit = 0;
#endif
                            testInStream.reset();

                            ++passes;

                            dest = DEST_ORIG;

                            dest.bdexStreamInImp(testInStream,
                                                 version,
                                                 STREAMIN_TABLE_TEST,
                                                 ATTR_LOOKUP_TBL);
                        } END_BDEX_EXCEPTION_TEST

                        if (veryVerbose) { P(dest); }
#ifdef BDE_BUILD_TARGET_EXC
                        ASSERT(passes > 1);
#endif

                        if (version < 3) {
                            LOOP5_ASSERT(rows, cols, pattern, DEST, TABLE,
                                      compareTablesWithoutNullnessBits(DEST,
                                                                       TABLE));
                        }
                        else {
                            LOOP5_ASSERT(rows, cols, pattern, DEST, TABLE,
                                         DEST == TABLE);
                        }

                        if (veryVerbose) cout << "(" << rows << " x " << cols
                                              << "), pattern: " << pattern
                                              << ", passes: " << passes
                                              << endl;
                        if (veryVerbose && NUM_COL_TYPES * 2 == rows
                         && NUM_COL_TYPES == cols && 3 == pattern) {
                            cout << "Biggest Table:\n";
                            TABLE.print(cout, 0, -1);
                        }
                    }
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS AND ACCESSORS
        //
        // Concerns:
        //   That all the major manipulators and accessors work.
        //
        // Plan:
        //   Manipulate tables with manipulators and verify with accessors
        //   and equality/inequality comparisons that the results are as
        //   expected.  Note once T1 is fully built, it is kept constant for
        //   the remainder of this test, only t2 and t3 are manipulated.
        //
        // Testing:
        //   //MANIPULATORS
        //   insertRow()
        //   row()
        //   operator=()
        //   removeRow()
        //   removeRows()
        //   removeAll();
        //   clear()
        //   makeColumnNull()
        //   makeRowsNull()
        //   makeAllNull()
        //   insertRows()
        //   insertNullRows()
        //   setColumnValue()
        //   setColumnValueRaw()
        //
        //   //ACCESSORS
        //   columnType()
        //   numRows()
        //   numColumns()
        //   row()
        //   isAnyInColumnNonNull()
        //   isAnyInColumnNull()
        //   isAnyNonNull()
        //   isAnyNull()
        // --------------------------------------------------------------------

        if (verbose) cout << "Manipulators and accessors\n"
                             "==========================\n";

        bdem_ElemType::Type COL_TYPES[] = {
            ET::BDEM_INT, ET::BDEM_STRING, ET::BDEM_CHAR
        };

        enum { COL_TYPES_LEN = sizeof(COL_TYPES) / sizeof(*COL_TYPES) };

        bslma_TestAllocator alloc, *Z = &alloc;

        Obj t1(COL_TYPES,
               COL_TYPES_LEN,
               ATTR_LOOKUP_TBL,
               BDEM_PASS_THROUGH,
               Z);

        const Obj& T1 = t1;
        ASSERT(T1.numRows() == 0);        ASSERT(T1.numColumns() == 3);

        Obj t2(BDEM_PASS_THROUGH, Z);
        const Obj& T2 = t2;
        ASSERT(T2.numRows() == 0);        ASSERT(T2.numColumns() == 0);

        ASSERT(T1.columnType(0) == ET::BDEM_INT);
        ASSERT(T1.columnType(1) == ET::BDEM_STRING);
        ASSERT(T1.columnType(2) == ET::BDEM_CHAR);

        bdem_RowLayout rowLayout(COL_TYPES,
                                 COL_TYPES_LEN,
                                 ATTR_LOOKUP_TBL,
                                 Z);
        bdem_RowData  rowData(&rowLayout, BDEM_PASS_THROUGH, Z);
        rowData.setValue(0, &INT_VALUE1);
        rowData.setValue(1, &STRING_VALUE1);
        rowData.setValue(2, &CHAR_VALUE1);

        // --------------------------------------------------------------------

        if (verbose) cout << "Insert Row, row()\n";

        t1.insertRow(0, rowData);
        t1.insertRow(0, rowData);
        t1.makeRowsNull(0, 1);
        t1.insertRow(0, rowData);
        ASSERT(T1 != T2);

        ASSERT(T1.numRows() == 3);        ASSERT(T1.numColumns() == 3);

        getModElemRef(&t1, 0, 0).theModifiableInt()    = INT_VALUE2;
        getModElemRef(&t1, 0, 1).theModifiableString() = STRING_VALUE2;
        getModElemRef(&t1, 2, 2).theModifiableChar()   = CHAR_VALUE2;

        for (int row = 0; row < 3; ++row) {
            const bdem_RowData& TRH = T1.theRow(row);
            ASSERT(TRH.elemRef(0).type() == ET::BDEM_INT);
            ASSERT(TRH.elemRef(1).type() == ET::BDEM_STRING);
            ASSERT(TRH.elemRef(2).type() == ET::BDEM_CHAR);

            LOOP2_ASSERT(row, TRH.elemRef(0).theInt(),
                         TRH.elemRef(0).theInt() ==
                                         (0 == row ? INT_VALUE2
                                        : 1 == row ? UNSET_INT : INT_VALUE1));
            LOOP2_ASSERT(row, TRH.elemRef(1).theString(),
                         TRH.elemRef(1).theString() ==
                                         (0 == row ? STRING_VALUE2
                                         : 1 == row ? UNSET_STRING
                                           : STRING_VALUE1));
            LOOP2_ASSERT(row, TRH.elemRef(2).theChar(),
                         TRH.elemRef(2).theChar() ==
                                         (0 == row ? CHAR_VALUE1
                                       : 1 == row ? UNSET_CHAR : CHAR_VALUE2));
        }

        t2 = T1;
        ASSERT(T1 == T2);

        ASSERT(T2.numRows() == 3);        ASSERT(T2.numColumns() == 3);

        for (int row = 0; row < 3; ++row) {
            const bdem_RowData& TRH = T2.theRow(row);
            ASSERT(TRH.elemRef(0).type() == ET::BDEM_INT);
            ASSERT(TRH.elemRef(1).type() == ET::BDEM_STRING);
            ASSERT(TRH.elemRef(2).type() == ET::BDEM_CHAR);

            LOOP2_ASSERT(row, TRH.elemRef(0).theInt(),
                         TRH.elemRef(0).theInt() ==
                                         (0 == row ? INT_VALUE2
                                        : 1 == row ? UNSET_INT : INT_VALUE1));
            LOOP2_ASSERT(row, TRH.elemRef(1).theString(),
                         TRH.elemRef(1).theString() ==
                                         (0 == row ? STRING_VALUE2
                                         : 1 == row ? UNSET_STRING
                                           : STRING_VALUE1));
            LOOP2_ASSERT(row, TRH.elemRef(2).theChar(),
                         TRH.elemRef(2).theChar() ==
                                         (0 == row ? CHAR_VALUE1
                                       : 1 == row ? UNSET_CHAR : CHAR_VALUE2));
        }

        t2 = T2;    // aliasing
        ASSERT(T1 == T2);

        ASSERT(T2.numRows() == 3);        ASSERT(T2.numColumns() == 3);

        for (int row = 0; row < 3; ++row) {
            const bdem_RowData& TRH = T2.theRow(row);
            ASSERT(TRH.elemRef(0).type() == ET::BDEM_INT);
            ASSERT(TRH.elemRef(1).type() == ET::BDEM_STRING);
            ASSERT(TRH.elemRef(2).type() == ET::BDEM_CHAR);

            LOOP2_ASSERT(row, TRH.elemRef(0).theInt(),
                         TRH.elemRef(0).theInt() ==
                                         (0 == row ? INT_VALUE2
                                        : 1 == row ? UNSET_INT : INT_VALUE1));
            LOOP2_ASSERT(row, TRH.elemRef(1).theString(),
                         TRH.elemRef(1).theString() ==
                                         (0 == row ? STRING_VALUE2
                                         : 1 == row ? UNSET_STRING
                                           : STRING_VALUE1));
            LOOP2_ASSERT(row, TRH.elemRef(2).theChar(),
                         TRH.elemRef(2).theChar() ==
                                         (0 == row ? CHAR_VALUE1
                                       : 1 == row ? UNSET_CHAR : CHAR_VALUE2));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "RemoveRow, RemoveRows\n";

        t2.removeRow(0);
        ASSERT(T2 != T1);
        ASSERT(2 == T2.numRows());
        ASSERT(1 == T2.isAnyNull());
        ASSERT(1 == T2.isAnyNonNull());
        ASSERT(1 == T2.isAnyRowNull(0, 1));
        ASSERT(0 == T2.isAnyRowNull(1, 1));
        ASSERT(0 == T2.isAnyRowNonNull(0, 1));
        ASSERT(1 == T2.isAnyRowNonNull(1, 1));

        ASSERT(getElemRef(T2, 0, 0).theInt()  == UNSET_INT);
        ASSERT(getElemRef(T2, 1, 2).theChar() == CHAR_VALUE2);

        t2 = T1;
        ASSERT(T1 == T2);

        t2.removeRows(0, 2);
        ASSERT(1 == T2.numRows());
        ASSERT(0 == T2.isAnyNull());
        ASSERT(1 == T2.isAnyNonNull());
        ASSERT(0 == T2.isAnyRowNull(0, 1));
        ASSERT(1 == T2.isAnyRowNonNull(0, 1));

        ASSERT(getElemRef(T2, 0, 2).theChar() == CHAR_VALUE2);

        // --------------------------------------------------------------------

        if (verbose) cout << "RemoveAll, clear\n";

        t2 = T1;
        ASSERT(T1 == T2);

        t2.removeAll();
        ASSERT(0 == T2.numRows());
        ASSERT(0 == T2.numColumns());
        ASSERT(0 == T2.isAnyNull());
        ASSERT(0 == T2.isAnyNonNull());

        t2 = T1;
        ASSERT(T1 == T2);

        t2.clear();
        ASSERT(0 == T2.numRows());
        ASSERT(0 == T2.numColumns());
        ASSERT(0 == T2.isAnyNull());
        ASSERT(0 == T2.isAnyNonNull());

        // --------------------------------------------------------------------

        if (verbose) cout << "Fully populated table\n";

        t2 = T1;
        ASSERT(T1 == T2);

        for (int i = 0; i < 3; ++i) {
            ASSERT(T2.isAnyInColumnNonNull(i));
            ASSERT(T2.isAnyInColumnNull(i));
        }
        ASSERT(T2.isAnyNonNull());
        ASSERT(T2.isAnyNull());

        // --------------------------------------------------------------------

        if (verbose) cout << "Null the middle element only\n";

        t2 = T1;
        // Replace the null row with a filled row
        t2.removeRow(1);
        t2.insertRow(1, t2.theRow(0));
        getModElemRef(&t2, 1, 1).makeNull();

        ASSERT( T2.isAnyNonNull());
        ASSERT( T2.isAnyNull());

        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                if (1 == row && 1 == col) {
                    ASSERT(getElemRef(T2, row, col).isNull());
                    ASSERT(isUnset(getElemRef(T2, row, col)));
                }
                else {
                    ASSERT(getElemRef(T2, row, col).isNonNull());
                }
            }
        }

        for (int col = 0; col < 3; ++col) {
            ASSERT( T2.isAnyInColumnNonNull(col));
            ASSERT((1 == col) == T2.isAnyInColumnNull(col));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "Null out the middle\n";

        t2 = T1;
        ASSERT(T2 == T1);

        // Replace the null row with a filled row
        t2.removeRow(1);
        t2.insertRow(1, t2.theRow(0));
        getModElemRef(&t2, 1, 1).makeNull();

        ASSERT( T2.isAnyNonNull());
        ASSERT( T2.isAnyNull());

        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                if (1 == row && 1 == col) {
                    ASSERT(getElemRef(T2, row, col).isNull());
                    ASSERT(isUnset(getElemRef(T2, row, col)));
                }
                else {
                    ASSERT(!getElemRef(T2, row, col).isNull());
                }
            }
        }

        for (int col = 0; col < 3; ++col) {
            ASSERT((1 == col) == T2.isAnyInColumnNull(col));
            ASSERT( T2.isAnyInColumnNonNull(col));
        }

        t2 = T1;
        ASSERT(T1 == T2);

        for (int i = 0; i < 3; ++i) {
            ASSERT( T2.isAnyInColumnNonNull(i));
            ASSERT( T2.isAnyInColumnNull(i));
        }
        ASSERT( T2.isAnyNonNull());
        ASSERT( T2.isAnyNull());

        // --------------------------------------------------------------------

        if (verbose) cout << "Make the middle column null\n";

        t2 = T1;
        ASSERT(T2 == T1);

        t2.removeRow(1);
        t2.insertRow(1, t2.theRow(0));
        t2.makeColumnNull(1);

        ASSERT( T2.isAnyNonNull());
        ASSERT( T2.isAnyNull());

        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                ASSERT((1 != col) == getElemRef(T2, row, col).isNonNull());
            }
        }

        for (int col = 0; col < 3; ++col) {
            ASSERT((1 == col) == T2.isAnyInColumnNull(col));
            ASSERT((1 != col) == T2.isAnyInColumnNonNull(col));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "Null out the middle column\n";

        t2 = T1;
        t2.removeRow(1);
        t2.insertRow(1, t2.theRow(0));
        t2.makeColumnNull(1);

        ASSERT( T2.isAnyNonNull());
        ASSERT( T2.isAnyNull());

        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                ASSERT((1 == col) == getElemRef(T2, row, col).isNull());
                ASSERT((1 == col) == isUnset(getElemRef(T2, row, col)));
            }
        }

        for (int col = 0; col < 3; ++col) {
            ASSERT((1 == col) == T2.isAnyInColumnNull(col));
            ASSERT((1 != col) == T2.isAnyInColumnNonNull(col));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "Make the middle row null\n";

        t2 = T1;
        ASSERT(T2 == T1);

        t2.makeRowsNull(1, 1);

        ASSERT( T2.isAnyNonNull());
        ASSERT( T2.isAnyNull());

        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                ASSERT((1 != row) == getElemRef(T2, row, col).isNonNull());
            }
        }

        for (int col = 0; col < 3; ++col) {
            ASSERT( T2.isAnyInColumnNull(col));
            ASSERT( T2.isAnyInColumnNonNull(col));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "Null out the middle row\n";

        t2 = T1;
        t2.makeRowsNull(1, 1);

        ASSERT( T2.isAnyNonNull());
        ASSERT( T2.isAnyNull());

        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                ASSERT((1 == row) == getElemRef(T2, row, col).isNull());
                ASSERT((1 == row) == isUnset(getElemRef(T2, row, col)));
            }
        }

        for (int col = 0; col < 3; ++col) {
            ASSERT( T2.isAnyInColumnNull(col));
            ASSERT( T2.isAnyInColumnNonNull(col));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "Make all unset\n";

        t2 = T1;
        ASSERT(T1 == T2);

        t2.makeAllNull();

        ASSERT(!T2.isAnyNonNull());
        ASSERT( T2.isAnyNull());

        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                ASSERT(getElemRef(T2, row, col).isNull());
                ASSERT(isUnset(getElemRef(T2, row, col)));
            }
        }

        for (int col = 0; col < 3; ++col) {
            ASSERT( T2.isAnyInColumnNull(col));
            ASSERT(!T2.isAnyInColumnNonNull(col));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "Make all Null\n";

        t2 = T1;
        ASSERT(T1 == T2);

        t2.makeAllNull();

        ASSERT(!T2.isAnyNonNull());
        ASSERT( T2.isAnyNull());

        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                ASSERT(getElemRef(T2, row, col).isNull());
                ASSERT(isUnset(getElemRef(T2, row, col)));
            }
        }

        for (int col = 0; col < 3; ++col) {
            ASSERT( T2.isAnyInColumnNull(col));
            ASSERT(!T2.isAnyInColumnNonNull(col));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "Testing insertRows()\n";

        t2 = T1;
        ASSERT(T2 == T1);

        Obj t3(T1, BDEM_PASS_THROUGH, Z);    const Obj& T3 = t3;

        t3.removeRows(0, 3);
        ASSERT(t3 != T1);
        ASSERT(0 == T3.isAnyNull());
        ASSERT(0 == T3.isAnyNonNull());

        t3.insertRows(0, T2, 1, 1);
        ASSERT(1 == T3.numRows());
        ASSERT(1 == T3.isAnyNull());
        ASSERT(0 == T3.isAnyNonNull());
        ASSERT(1 == T3.isAnyRowNull(0, 1));
        ASSERT(0 == T3.isAnyRowNonNull(0, 1));
        for (int col = 0; col < 3; ++col) {
            ASSERT( T3.isAnyInColumnNull(col));
            ASSERT(!T3.isAnyInColumnNonNull(col));
        }

        t3.insertRows(0, T2, 0, 1);
        ASSERT(2 == T3.numRows());
        ASSERT(1 == T3.isAnyNull());
        ASSERT(1 == T3.isAnyNonNull());
        ASSERT(1 == T3.isAnyRowNull(0, 2));
        ASSERT(1 == T3.isAnyRowNonNull(0, 2));
        for (int col = 0; col < 3; ++col) {
            ASSERT( T3.isAnyInColumnNull(col));
            ASSERT( T3.isAnyInColumnNonNull(col));
        }

        t3.insertRows(2, T2, 2, 1);
        ASSERT(3 == T3.numRows());
        ASSERT(1 == T3.isAnyNull());
        ASSERT(1 == T3.isAnyNonNull());
        ASSERT(1 == T3.isAnyRowNull(0, 2));
        ASSERT(1 == T3.isAnyRowNonNull(0, 2));
        for (int col = 0; col < 3; ++col) {
            ASSERT( T3.isAnyInColumnNull(col));
            ASSERT( T3.isAnyInColumnNonNull(col));
        }

        LOOP2_ASSERT(T3, T2, T3 == T2);

        t3.removeRows(0, 3);
        ASSERT(0 == T3.isAnyNull());
        ASSERT(0 == T3.isAnyNonNull());

        t3.insertRows(0, T2, 2, 1);
        t3.insertRows(0, T2, 0, 2);
        ASSERT(3 == T3.numRows());

        LOOP2_ASSERT(T3, T2, T3 == T2);

        // --------------------------------------------------------------------

        if (verbose) cout << "Testing insertNullRows()\n";

        t2 = T1;
        ASSERT(T2 == T1);
        t3 = T1;
        ASSERT(T3 == T1);

        t3.removeRows(0, 3);
        ASSERT(0 == T3.numRows());
        ASSERT(3 == T3.numColumns());

        t3.insertNullRows(0, 3);
        t2.makeAllNull();

        ASSERT(T3 == T2);

        // --------------------------------------------------------------------

        if (verbose) cout << "Testing insertNullRows()\n";

        t2 = T1;
        ASSERT(T2 == T1);

        t3 = T1;
        ASSERT(T2 == T3);

        t3.removeRows(0, 3);
        ASSERT(0 == T3.numRows());
        ASSERT(3 == T3.numColumns());

        t3.insertNullRows(0, 3);
        t2.makeAllNull();

        ASSERT(T3 == T2);

        // --------------------------------------------------------------------

        if (verbose) cout << "set column value\n";

        t2 = T1;
        ASSERT(T2 == T1);

        bsl::string woof = "woof";

        t2.setColumnValue(1, &woof);
        ASSERT( t2.isAnyNonNull());
        ASSERT( t2.isAnyNull());
        for (int row = 0; row < 3; ++row) {
            ASSERT(getElemRef(T2, row, 1).theString() == "woof");
        }
        t2.setColumnValue(1, &STRING_VALUE1);
        ASSERT( t2.isAnyNonNull());
        ASSERT( t2.isAnyNull());
        for (int row = 0; row < 3; ++row) {
            ASSERT(getElemRef(T2, row, 1).theString() == STRING_VALUE1);
        }
        getModElemRef(&t2, 0, 1).theModifiableString() = STRING_VALUE2;
        t2.makeRowsNull(1, 1);
        LOOP2_ASSERT(T2, T1, T2 == T1);

        // --------------------------------------------------------------------

        if (verbose) cout << "set column value raw\n";

        const int intValue3 = 84;

        t2.setColumnValueRaw(0, &intValue3);
        ASSERT( t2.isAnyNonNull());
        ASSERT( t2.isAnyNull());
        for (int row = 0; row < 3; ++row) {
            ASSERT(getElemRef(T2, row, 0).theInt() == intValue3);
        }
        t2.setColumnValueRaw(0, &INT_VALUE1);
        ASSERT( t2.isAnyNonNull());
        ASSERT( t2.isAnyNull());
        for (int row = 0; row < 3; ++row) {
            ASSERT(getElemRef(T2, row, 0).theInt() == INT_VALUE1);
        }

        getModElemRef(&t2, 0, 0).theModifiableInt() = INT_VALUE2;
        t2.makeRowsNull(1, 1);
        LOOP2_ASSERT(T1, T2, T2 == T1);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING BDEMA ALLOCATOR MODEL AND ALLOCATOR TRAITS
        //
        // Concerns: That the type under testing, which uses an allocator, when
        //   it is placed into a container, is correctly propagated the
        //   allocator of the container and will use the allocator (not the
        //   default allocator) for its future memory needs.
        //
        // Plan: It suffices to assert that the traits is defined.  One way is
        //   by using 'BSLALG_DECLARE_NESTED_TRAITS' and another is by sniffing
        //   that there is an implicit conversion construction from
        //   'bslma_Allocator*'.  We also want to discourage the second way, as
        //   that constructor should be made explicit.
        //
        // Testing:
        //   bdema allocator model
        //   correct declaration of bslalg_TypeTraitUsesBslmaAllocator
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing allocator traits\n"
                             "========================\n";

        ASSERT((0 == bslmf_IsConvertible<bslma_Allocator*, Obj>::VALUE));
        ASSERT((1 ==
             bslalg_HasTrait<Obj, bslalg_TypeTraitUsesBslmaAllocator>::VALUE));
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'reserveMemory'
        //
        // Concerns:
        //   That 'reserveMemory' really does consume memory.
        //
        // Plan:
        //   It would be nice to verify that no more memory is allocated
        //   for modest increases in table size after reserving memory, but
        //   this is not the case -- there are 2 allocators in a 'TableImp',
        //   'd_aggregateImp' and 'd_pool', and only 'd_aggregateImp' is
        //   affected by the 'reserveMemory'.  So all we can confirm is
        //   'reserveMemory's consumption of memory.
        //
        // Testing:
        //   void reserveMemory(int numBytes);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'reserveMemory'\n"
                             "=======================\n";

        const int SIZE = 4000;

        // --------------------------------------------------------------------

        if (verbose) cout << "Using 'BDEM_WRITE_ONCE'" << endl;
        {
            bslma_TestAllocator allocator;
            Obj mX(BDEM_WRITE_ONCE, &allocator);

            const int beforeSize = allocator.numBytesInUse();

            mX.reserveMemory(SIZE);

            const int afterReserveSize = allocator.numBytesInUse();

            LOOP2_ASSERT(allocator.numBytesInUse(), beforeSize,
                                        SIZE <= afterReserveSize - beforeSize);
        }

        if (verbose) cout << "Using 'BDEM_WRITE_MANY'" << endl;
        {
            bslma_TestAllocator allocator;
            Obj mX(BDEM_WRITE_MANY, &allocator);

            const int beforeSize = allocator.numBytesInUse();

            mX.reserveMemory(SIZE);

            LOOP2_ASSERT(allocator.numBytesInUse(), beforeSize,
                         SIZE <= allocator.numBytesInUse() - beforeSize);
        }

        if (verbose) cout << "\nEnd of 'reserveMemory' Test." << endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CTORS WITH INITIAL MEMORY
        //
        // Concerns:
        //    That initialMemory arg is paid attention to by c'tors
        //
        // Plan:
        //    Use every c'tor that takes InitialMemory with InitialMemory
        //    set to a large amount, and verify that more than that amount
        //    has been allocated.  Note that a 'bdem_TableImp' allocates
        //    memory from two sources, 'd_aggregateImp', which is affected
        //    by the 'initialMemory' field, and 'd_pool', which is not,
        //    therefore subsequent additions to the 'TableImp' will always
        //    have an impact the test allocator.
        //
        // Testing:
        //   InitialMemory
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING CTORS WITH INITIAL MEMORY\n"
                             "=================================\n";

        const int SIZE = 4000;  // initial memory

        if (verbose) cout << "With allocMode only.\n";
        {
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_ONCE'.\n";
            {
                bslma_TestAllocator allocator;

                Obj mX(BDEM_WRITE_ONCE, SIZE, &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_MANY'.\n";
            {
                bslma_TestAllocator allocator;

                Obj mX(BDEM_WRITE_MANY, SIZE, &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
        }

        if (verbose) cout << "With element types.\n";
        {
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_ONCE'.\n";
            {
                bslma_TestAllocator allocator;

                Obj mX(&DESCRIPTOR_TYPE_ENUM,
                       1,
                       ATTR_LOOKUP_TBL,
                       BDEM_WRITE_ONCE,
                       SIZE,
                       &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_MANY'.\n";
            {
                bslma_TestAllocator allocator;

                Obj mX(&DESCRIPTOR_TYPE_ENUM,
                       1,
                       ATTR_LOOKUP_TBL,
                       BDEM_WRITE_MANY,
                       SIZE,
                       &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
        }

        if (verbose) cout << "With copy ctor.\n";
        {
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_ONCE'.\n";
            {
                bslma_TestAllocator allocator;
                Obj original(bdem_AggregateOption::BDEM_PASS_THROUGH,
                             &allocator);

                const int startSize = allocator.numBytesInUse();

                Obj mX(original,
                       BDEM_WRITE_ONCE,
                       SIZE,
                       &allocator);

                LOOP2_ASSERT(allocator.numBytesInUse(), startSize,
                             SIZE <= allocator.numBytesInUse() - startSize);
            }
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_MANY'.\n";
            {
                bslma_TestAllocator allocator;
                Obj original(bdem_AggregateOption::BDEM_PASS_THROUGH,
                             &allocator);

                const int startSize = allocator.numBytesInUse();

                Obj mX(original,
                       BDEM_WRITE_MANY,
                       SIZE,
                       &allocator);

                LOOP2_ASSERT(allocator.numBytesInUse(), startSize,
                             SIZE <= allocator.numBytesInUse() - startSize);
            }
        }

        if (verbose) cout << "With row def.\n";
        {
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_ONCE'.\n";
            {
                bslma_TestAllocator allocator;
                bdem_RowLayout rowLayout(&allocator);
                bdem_RowData   rowData(&rowLayout,
                                       BDEM_PASS_THROUGH,
                                       &allocator);

                const int startSize = allocator.numBytesInUse();

                Obj mX(rowData,
                       BDEM_WRITE_ONCE,
                       SIZE,
                       &allocator);

                LOOP2_ASSERT(allocator.numBytesInUse(), startSize,
                             SIZE <= allocator.numBytesInUse() - startSize);
            }
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_MANY'.\n";
            {
                bslma_TestAllocator allocator;
                bdem_RowLayout rowLayout(&allocator);
                bdem_RowData   rowData(&rowLayout,
                                       BDEM_PASS_THROUGH,
                                       &allocator);

                const int startSize = allocator.numBytesInUse();

                Obj mX(rowData,
                       BDEM_WRITE_MANY,
                       SIZE,
                       &allocator);

                LOOP2_ASSERT(allocator.numBytesInUse(), startSize,
                             SIZE <= allocator.numBytesInUse() - startSize);
            }
        }

        if (verbose) cout << "End of 'InitialMemory' Test.\n";
      } break;
      case 3: {
        //--------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //
        // Concerns:
        //   That equality operators work properly.
        //
        // Plan:
        //   Create multiple tables, sometimes identical and sometimes not,
        //   verifying that they are equal exactly when and only when they
        //   should be.  Cover aliasing and the case where tables are identical
        //   except for having different memory allocators and allocation
        //   strategies.
        //--------------------------------------------------------------------

        if (verbose) cout << "Equality Operators\n"
                             "==================\n";

        bdem_ElemType::Type COL_TYPES[] = {
            ET::BDEM_INT, ET::BDEM_STRING, ET::BDEM_INT
        };

        enum { COL_TYPES_LEN = sizeof(COL_TYPES) / sizeof(*COL_TYPES) };

        bslma_TestAllocator alloc1;
        bslma_NewDeleteAllocator alloc2;
        bslma_Allocator *Z1 = &alloc1, *Z2 = &alloc2;

        Obj t1(COL_TYPES,
               COL_TYPES_LEN,
               ATTR_LOOKUP_TBL,
               BDEM_WRITE_MANY,
               Z1);

        Obj t2(COL_TYPES,
               COL_TYPES_LEN,
               ATTR_LOOKUP_TBL,
               BDEM_PASS_THROUGH,
               Z2);

        const Obj& T1 = t1;
        const Obj& T2 = t2;

        ASSERT(T1 == t1);       ASSERT(!(T1 != T1));
        ASSERT(T2 == T2);       ASSERT(!(t2 != T2));
        ASSERT(T1 == T2);       ASSERT(!(T1 != T2));
        ASSERT(T2 == t1);       ASSERT(!(t2 != T1));

        bdem_RowLayout rowLayout(COL_TYPES,
                                 COL_TYPES_LEN,
                                 ATTR_LOOKUP_TBL,
                                 Z1);
        bdem_RowData rowData(&rowLayout, BDEM_PASS_THROUGH, Z1);
        rowData.setValue(0, &INT_VALUE1);;
        rowData.setValue(1, &STRING_VALUE1);;
        rowData.setValue(2, &INT_VALUE2);;

        t1.insertRow(0, rowData);
        ASSERT(T1 == T1);       ASSERT(!(T1 != T1));
        ASSERT(T2 == T2);       ASSERT(!(T2 != T2));
        ASSERT(T1 != t2);       ASSERT(!(T1 == T2));
        ASSERT(t2 != T1);       ASSERT(!(t2 == T1));

        t2.insertRow(0, rowData);
        ASSERT(T1 == T1);       ASSERT(!(T1 != T1));
        ASSERT(T2 == T2);       ASSERT(!(T2 != T2));
        ASSERT(T1 == T2);       ASSERT(!(T1 != T2));
        ASSERT(T2 == T1);       ASSERT(!(T2 != T1));

        bdem_RowData& trh1 = t1.theModifiableRow(0);
        ASSERT(trh1.elemRef(0).theInt() == INT_VALUE1);
        ASSERT(trh1.elemRef(1).theString() == STRING_VALUE1);
        ASSERT(trh1.elemRef(2).theInt() == INT_VALUE2);

        trh1.elemRef(0).theModifiableInt() = 7;
        ASSERT(T1 == T1);       ASSERT(!(T1 != T1));
        ASSERT(T2 == T2);       ASSERT(!(T2 != T2));
        ASSERT(T1 != t2);       ASSERT(!(T1 == T2));
        ASSERT(t2 != T1);       ASSERT(!(t2 == T1));

        trh1.elemRef(0).theModifiableInt() = UNSET_INT;
        ASSERT(T1 == T1);       ASSERT(!(T1 != T1));
        ASSERT(T2 == T2);       ASSERT(!(T2 != T2));
        ASSERT(T1 != t2);       ASSERT(!(T1 == T2));
        ASSERT(t2 != T1);       ASSERT(!(t2 == T1));

        bdem_RowData& trh2 = t2.theModifiableRow(0);
        ASSERT(trh2.elemRef(0).theInt() == INT_VALUE1);
        ASSERT(trh2.elemRef(1).theString() == STRING_VALUE1);
        ASSERT(trh2.elemRef(2).theInt() == INT_VALUE2);

        trh2.elemRef(0).theModifiableInt() = UNSET_INT;
        ASSERT(T1 == T1);       ASSERT(!(T1 != T1));
        ASSERT(T2 == T2);       ASSERT(!(T2 != T2));
        ASSERT(T1 == T2);       ASSERT(!(T1 != T2));
        ASSERT(T2 == T1);       ASSERT(!(T2 != T1));

        // make sure NULL and Unset are distinguished from each other
        trh1.elemRef(0).makeNull();
        ASSERT(T1 == T1);       ASSERT(!(T1 != T1));
        ASSERT(T2 == T2);       ASSERT(!(T2 != T2));
        ASSERT(T1 != t2);       ASSERT(!(T1 == T2));
        ASSERT(t2 != T1);       ASSERT(!(t2 == T1));

        trh2.elemRef(0).makeNull();
        ASSERT(T1 == T1);       ASSERT(!(T1 != T1));
        ASSERT(T2 == T2);       ASSERT(!(T2 != T2));
        ASSERT(T1 == t2);       ASSERT(!(T1 != T2));
        ASSERT(t2 == T1);       ASSERT(!(t2 != T1));

        t1.makeRowsNull(0, 1);
        ASSERT(T1 == T1);       ASSERT(!(T1 != T1));
        ASSERT(T2 == T2);       ASSERT(!(T2 != T2));
        ASSERT(T1 != t2);       ASSERT(!(T1 == T2));
        ASSERT(t2 != T1);       ASSERT(!(t2 == T1));

        t2.makeRowsNull(0, 1);
        ASSERT(T1 == T1);       ASSERT(!(T1 != T1));
        ASSERT(T2 == T2);       ASSERT(!(T2 != T2));
        ASSERT(T1 == t2);       ASSERT(!(T1 != T2));
        ASSERT(t2 == T1);       ASSERT(!(t2 != T1));

        t1.insertNullRows(0, 32);
        ASSERT(T1 == T1);       ASSERT(!(T1 != T1));
        ASSERT(T2 == T2);       ASSERT(!(T2 != T2));
        ASSERT(T1 != t2);       ASSERT(!(T1 == T2));
        ASSERT(t2 != T1);       ASSERT(!(t2 == T1));

        t2.insertNullRows(0, 32);
        ASSERT(T1 == T1);       ASSERT(!(T1 != T1));
        ASSERT(T2 == T2);       ASSERT(!(T2 != T2));
        ASSERT(T1 == t2);       ASSERT(!(T1 != T2));
        ASSERT(t2 == T1);       ASSERT(!(t2 != T1));

        t1.insertNullRows(0, 64);
        ASSERT(T1 == T1);       ASSERT(!(T1 != T1));
        ASSERT(T2 == T2);       ASSERT(!(T2 != T2));
        ASSERT(T1 != t2);       ASSERT(!(T1 == T2));
        ASSERT(t2 != T1);       ASSERT(!(t2 == T1));

        t2.insertNullRows(0, 64);
        ASSERT(T1 == T1);       ASSERT(!(T1 != T1));
        ASSERT(T2 == T2);       ASSERT(!(T2 != T2));
        ASSERT(T1 == t2);       ASSERT(!(T1 != T2));
        ASSERT(t2 == T1);       ASSERT(!(t2 != T1));

        t1.removeRow(0);
        ASSERT(T1 == T1);       ASSERT(!(T1 != T1));
        ASSERT(T2 == T2);       ASSERT(!(T2 != T2));
        ASSERT(T1 != t2);       ASSERT(!(T1 == T2));
        ASSERT(t2 != T1);       ASSERT(!(t2 == T1));

        t2.removeRow(0);
        ASSERT(T1 == T1);       ASSERT(!(T1 != T1));
        ASSERT(T2 == T2);       ASSERT(!(T2 != T2));
        ASSERT(T1 == T2);       ASSERT(!(T1 != T2));
        ASSERT(T2 == T1);       ASSERT(!(T2 != T1));

        t1.removeRows(0, 96);
        ASSERT(T1.numRows()    == 0);
        ASSERT(T1.numColumns() == 3);
        ASSERT(T1 == T1);       ASSERT(!(T1 != T1));
        ASSERT(T2 == T2);       ASSERT(!(T2 != T2));
        ASSERT(T1 != t2);       ASSERT(!(T1 == T2));
        ASSERT(t2 != T1);       ASSERT(!(t2 == T1));

        t2.removeRows(0, 96);
        ASSERT(T2.numRows()    == 0);
        ASSERT(T2.numColumns() == 3);
        ASSERT(T1 == T1);       ASSERT(!(T1 != T1));
        ASSERT(T2 == T2);       ASSERT(!(T2 != T2));
        ASSERT(T1 == T2);       ASSERT(!(T1 != T2));
        ASSERT(T2 == T1);       ASSERT(!(T2 != T1));

        // ensure that tables with no rows, if they have different col types,
        // are not equal
        bdem_ElemType::Type COL_TYPES2[] = { ET::BDEM_INT, ET::BDEM_STRING };

        enum { COL_TYPES2_LEN = sizeof(COL_TYPES2) / sizeof(*COL_TYPES2) };

        Obj t3(COL_TYPES2,
               COL_TYPES2_LEN,
               ATTR_LOOKUP_TBL,
               BDEM_WRITE_MANY,
               Z1);

        const Obj& T3 = t3;

        ASSERT(T3.numRows() == 0);
        ASSERT(T3.numColumns() == 2);

        ASSERT(T3 == T3);       ASSERT(!(T3 != T3));
        ASSERT(T1 != T3);       ASSERT(!(T1 == T3));
        ASSERT(T3 != T1);       ASSERT(!(T3 == T1));

        ASSERT(t2 != T3);       ASSERT(!(t2 == T3));
        ASSERT(T3 != t2);       ASSERT(!(T3 == T2));
      } break;
      case 2: {
        //--------------------------------------------------------------------
        // TEST ALL CONSTRUCTORS & RESET, BOOTSTRAP
        //
        // Concerns:
        //   That all c'tors will create the table intended.
        //
        // Plan:
        //   Run a table-driven test that will create the same table using
        //   every existing c'tor (coupled with 'reset' if necessary) and
        //   then manipulate and access the table to ensure that it's healthy.
        //
        // Testing:
        //   bdem_TableImp(allocMode, Z)
        //   reset(elemTypes[], numElements, attrLookupTbl)
        //   bdem_TableImp(allocMode, initialMemory, Z)
        //   bdem_TableImp(elemTypes[], numElements, attrLookupTbl,allocMode,Z)
        //   bdem_TableImp(elemTypes[], numElements, attrLookupTbl, allocMode,
        //                                                    initialMemory, Z)
        //   bdem_TableImp(rowDef&, allocMode, Z)
        //   bdem_TableImp(rowDef&, allocMode, initialMemory, Z)
        //   bdem_TableImp(original&, allocMode, Z)
        //   bdem_TableImp(original&, allocMode, initialMemory, Z)
        //   ~bdem_TableImp()
        //--------------------------------------------------------------------

        if (verbose) cout << "Basic Constructor Test\n"
                             "======================\n";

        struct {
            int    d_line;
            int    d_int1;
            string d_string1;
            int    d_int2;
        } DATA[] = {
            { L_, INT_VALUE1, STRING_VALUE1, INT_VALUE1 },
            { L_, INT_VALUE1, STRING_VALUE1, INT_VALUE2 },
            { L_, INT_VALUE1, STRING_VALUE2, INT_VALUE1 },
            { L_, INT_VALUE1, STRING_VALUE2, INT_VALUE2 },
            { L_, INT_VALUE2, STRING_VALUE1, INT_VALUE1 },
            { L_, INT_VALUE2, STRING_VALUE1, INT_VALUE2 },
            { L_, INT_VALUE2, STRING_VALUE2, INT_VALUE1 },
            { L_, INT_VALUE2, STRING_VALUE2, INT_VALUE2 },
            { L_, INT_VALUE1, STRING_VALUE1,  UNSET_INT },
            { L_, INT_VALUE1,  UNSET_STRING, INT_VALUE1 },
            { L_, INT_VALUE1,  UNSET_STRING,  UNSET_INT },
            { L_,  UNSET_INT, STRING_VALUE1, INT_VALUE1 },
            { L_,  UNSET_INT, STRING_VALUE1,  UNSET_INT },
            { L_,  UNSET_INT,  UNSET_STRING, INT_VALUE1 },
            { L_,  UNSET_INT,  UNSET_STRING,  UNSET_INT } };

        bdem_ElemType::Type COL_TYPES[] = {
            ET::BDEM_INT, ET::BDEM_STRING, ET::BDEM_INT
        };

        enum {
            DATA_LEN = sizeof(DATA) / sizeof(*DATA),
            COL_TYPES_LEN = sizeof(COL_TYPES) / sizeof(*COL_TYPES)
        };

        for (int i = 0; i < DATA_LEN; ++i) {
            const int    LINE    = DATA[i].d_line;
            const int    INT1    = DATA[i].d_int1;
            const string STRING1 = DATA[i].d_string1;
            const int    INT2    = DATA[i].d_int2;

            if (veryVerbose) {
                cout << (INT1 == INT_VALUE1 ? "I1" :
                         INT1 == INT_VALUE2 ? "I2" : "IU") << ", " <<
                        (STRING1 == STRING_VALUE1 ? "S1" :
                         STRING1 == STRING_VALUE2 ? "S2" : "SU") << ", " <<
                        (INT2 == INT_VALUE1 ? "I1" :
                         INT2 == INT_VALUE2 ? "I2" : "IU") << ": ";
            }

            const bool is0Unset = (UNSET_INT    == INT1);
            const bool is1Unset = (UNSET_STRING == STRING1);
            const bool is2Unset = (UNSET_INT    == INT2);

            bool leave = false;
            for (int ctor = 0; !leave; ++ctor) {
                bslma_TestAllocator allocator;
                bslma_TestAllocator *Z = &allocator;

                const bdem_RowLayout rowLayout(COL_TYPES,
                                               COL_TYPES_LEN,
                                               ATTR_LOOKUP_TBL,
                                               Z);
                bdem_RowData rowData(&rowLayout, BDEM_PASS_THROUGH, Z);

                Obj *pObj = (Obj *) allocator.allocate(sizeof(Obj));
                Obj& OBJ = *pObj;

                if (veryVerbose) cout << ctor;

                switch (ctor) {
                  case 0: {
                    new (pObj) Obj(BDEM_WRITE_MANY, Z);

                    ASSERT(OBJ.numRows() == 0);  ASSERT(OBJ.numColumns() == 0);

                    pObj->reset(COL_TYPES, COL_TYPES_LEN, ATTR_LOOKUP_TBL);
                  } break;
                  case 1: {
                    new (pObj) Obj(BDEM_WRITE_MANY, 4000, Z);

                    ASSERT(OBJ.numRows() == 0);  ASSERT(OBJ.numColumns() == 0);

                    pObj->reset(COL_TYPES, COL_TYPES_LEN, ATTR_LOOKUP_TBL);
                  } break;
                  case 2: {
                    new (pObj) Obj(COL_TYPES, COL_TYPES_LEN, ATTR_LOOKUP_TBL,
                                   BDEM_WRITE_MANY, Z);
                  } break;
                  case 3: {
                    new (pObj) Obj(COL_TYPES, COL_TYPES_LEN, ATTR_LOOKUP_TBL,
                                   BDEM_WRITE_MANY, 4000, Z);
                  } break;
                  case 4: {
                    new (pObj) Obj(rowData, BDEM_WRITE_MANY, Z);
                  } break;
                  case 5: {
                    new (pObj) Obj(rowData, BDEM_WRITE_MANY, 0, Z);
                  } break;
                  default: {
                    ASSERT(6 == ctor);

                    leave = true;
                    allocator.deallocate(pObj);
                    continue;
                  }
                }

                ASSERT(0 == pObj->numRows());
                ASSERT(3 == pObj->numColumns());
                ASSERT(ET::BDEM_INT    == pObj->columnType(0));
                ASSERT(ET::BDEM_STRING == pObj->columnType(1));
                ASSERT(ET::BDEM_INT    == pObj->columnType(2));
                ASSERT(0 == pObj->isAnyNull());
                ASSERT(0 == pObj->isAnyNonNull());

                rowData.setValue(0, &INT1);
                rowData.setValue(1, &STRING1);
                rowData.setValue(2, &INT2);

                LOOP2_ASSERT(LINE, ctor, 0 == pObj->numRows());

                pObj->insertRow(0, rowData);
                ASSERT(1 == pObj->numRows());
                ASSERT(0 == pObj->isAnyNull());
                ASSERT(0 == pObj->isAnyRowNull(0, 1));
                ASSERT(1 == pObj->isAnyNonNull());
                ASSERT(1 == pObj->isAnyRowNonNull(0, 1));

                LOOP_ASSERT(LINE, 0 == OBJ.isAnyInColumnNull(0));
                LOOP_ASSERT(LINE, 0 == OBJ.isAnyInColumnNull(1));
                LOOP_ASSERT(LINE, 0 == OBJ.isAnyInColumnNull(2));

                LOOP_ASSERT(LINE,  OBJ.isAnyNonNull());
                LOOP_ASSERT(LINE, !OBJ.isAnyNull());

                // verify both incarnations of the copy c'tor
                Obj obj2(*pObj, BDEM_WRITE_MANY, Z);   const Obj& OBJ2 = obj2;
                LOOP2_ASSERT(LINE, ctor, OBJ2 == OBJ);
                ASSERT(1 == obj2.numRows());
                ASSERT(0 == obj2.isAnyNull());
                ASSERT(0 == obj2.isAnyRowNull(0, 1));
                ASSERT(1 == obj2.isAnyNonNull());
                ASSERT(1 == obj2.isAnyRowNonNull(0, 1));

                Obj obj3(OBJ2,  BDEM_WRITE_MANY, 4000, Z);
                const Obj& OBJ3 = obj3;
                LOOP2_ASSERT(LINE, ctor, OBJ2 == OBJ3);
                LOOP2_ASSERT(LINE, ctor, OBJ3 == OBJ);
                ASSERT(1 == obj2.numRows());
                ASSERT(0 == obj2.isAnyNull());
                ASSERT(0 == obj2.isAnyRowNull(0, 1));
                ASSERT(1 == obj2.isAnyNonNull());
                ASSERT(1 == obj2.isAnyRowNonNull(0, 1));

                bdem_RowData& trh = pObj->theModifiableRow(0);
                trh.elemRef(0).theModifiableInt() = 7;
                trh.elemRef(1).theModifiableString() = "woof";
                trh.elemRef(2).theModifiableInt() = 12;

                LOOP2_ASSERT(LINE, ctor, OBJ2 != OBJ);
                LOOP2_ASSERT(LINE, ctor, OBJ3 != OBJ);

                LOOP2_ASSERT(LINE, ctor, !OBJ.isAnyNull());
                LOOP_ASSERT(LINE, OBJ.isAnyInColumnNonNull(0));
                LOOP_ASSERT(LINE, OBJ.isAnyInColumnNonNull(1));
                LOOP_ASSERT(LINE, OBJ.isAnyInColumnNonNull(2));
                LOOP_ASSERT(LINE, 0 == OBJ.isAnyInColumnNull(0));
                LOOP_ASSERT(LINE, 0 == OBJ.isAnyInColumnNull(1));
                LOOP_ASSERT(LINE, 0 == OBJ.isAnyInColumnNull(2));

                pObj->insertRow(1, rowData);
                ASSERT(2 == pObj->numRows());
                ASSERT(0 == pObj->isAnyNull());
                ASSERT(0 == pObj->isAnyRowNull(0, 2));
                ASSERT(1 == pObj->isAnyNonNull());
                ASSERT(1 == pObj->isAnyRowNonNull(0, 2));

                LOOP_ASSERT(LINE, 0 == OBJ.isAnyInColumnNull(0));
                LOOP_ASSERT(LINE, 0 == OBJ.isAnyInColumnNull(1));
                LOOP_ASSERT(LINE, 0 == OBJ.isAnyInColumnNull(2));

                LOOP2_ASSERT(LINE, ctor, OBJ2 != OBJ);
                LOOP2_ASSERT(LINE, ctor, OBJ3 != OBJ);

                // verify BDEM_WRITE_MANY
                int bytesUsed = allocator.numBytesInUse();
                pObj->removeRows(0, 2);
                ASSERT(0 == pObj->numRows());
                ASSERT(0 == pObj->isAnyNull());
                ASSERT(0 == pObj->isAnyNonNull());
                LOOP2_ASSERT(LINE, ctor, allocator.numBytesInUse() ==
                                                                bytesUsed);

                obj2.removeRow(0);
                ASSERT(0 == obj2.numRows());
                ASSERT(0 == obj2.isAnyNull());
                ASSERT(0 == obj2.isAnyNonNull());
                LOOP2_ASSERT(LINE, ctor, OBJ2 == *pObj);
                LOOP2_ASSERT(LINE, ctor, allocator.numBytesInUse() ==
                                                                bytesUsed);

                obj3.removeRow(0);
                ASSERT(0 == obj3.numRows());
                ASSERT(0 == obj3.isAnyNull());
                ASSERT(0 == obj3.isAnyNonNull());
                LOOP2_ASSERT(LINE, ctor, OBJ3 == *pObj);
                LOOP2_ASSERT(LINE, ctor, OBJ3 == OBJ2);
                LOOP2_ASSERT(LINE, ctor, allocator.numBytesInUse() ==
                                                                bytesUsed);

                allocator.deleteObjectRaw(pObj);
            }
            if (veryVerbose) cout << endl;
        }
      } break;
      case 1: {
        //--------------------------------------------------------------------
        // BREATHING TEST
        // Concerns:
        //   We are concerned that the basic functionality of 'bdem_TableImp'
        //   works properly.
        //
        // Plan:
        //   Breathing test.  Create a table, manipulate it a bit, access it
        //   a bit.
        //
        // Testing:
        //   This breathing test exercises basic functionality as a preliminary
        //   to the more thorough testing done in subsequent cases.
        // --------------------------------------------------------------------

        if (verbose) cout << "Breathing Test\n"
                             "==============\n";

        bdem_ElemType::Type COL_TYPES[] = {
            ET::BDEM_INT, ET::BDEM_STRING, ET::BDEM_INT
        };

        enum { COL_TYPES_LEN = sizeof(COL_TYPES) / sizeof(*COL_TYPES) };

        bslma_TestAllocator  alloc;
        bslma_Allocator     *Z = &alloc;

        Obj t(COL_TYPES, COL_TYPES_LEN, ATTR_LOOKUP_TBL, BDEM_WRITE_MANY, Z);
        const Obj& T = t;
        ASSERT(0 == T.numRows());
        ASSERT(COL_TYPES_LEN == T.numColumns());
        ASSERT(0 == T.isAnyNull());
        ASSERT(0 == T.isAnyNonNull());

        bdem_RowLayout rowLayout(COL_TYPES,
                                 COL_TYPES_LEN,
                                 ATTR_LOOKUP_TBL,
                                 Z);
        bdem_RowData   rowData(&rowLayout, BDEM_PASS_THROUGH, Z);

        rowData.setValue(0, &INT_VALUE1);;
        rowData.setValue(1, &STRING_VALUE1);;
        rowData.setValue(2, &INT_VALUE2);;

        t.insertRow(0, rowData);
        ASSERT(1 == T.numRows());
        ASSERT(COL_TYPES_LEN == T.numColumns());
        ASSERT(0 == T.isAnyNull());
        ASSERT(0 == T.isAnyRowNull(0, 1));
        ASSERT(1 == T.isAnyNonNull());
        ASSERT(1 == T.isAnyRowNonNull(0, 1));

        t.insertRow(0, rowData);
        ASSERT(2 == T.numRows());
        ASSERT(COL_TYPES_LEN == T.numColumns());
        ASSERT(0 == T.isAnyNull());
        ASSERT(0 == T.isAnyRowNull(0, 2));
        ASSERT(1 == T.isAnyNonNull());
        ASSERT(1 == T.isAnyRowNonNull(0, 2));

        t.insertRow(0, rowData);
        ASSERT(3 == T.numRows());
        ASSERT(COL_TYPES_LEN == T.numColumns());
        ASSERT(0 == T.isAnyNull());
        ASSERT(0 == T.isAnyRowNull(0, 3));
        ASSERT(1 == T.isAnyNonNull());
        ASSERT(1 == T.isAnyRowNonNull(0, 3));

        t.insertRow(3, rowData);
        ASSERT(4 == T.numRows());
        ASSERT(COL_TYPES_LEN == T.numColumns());
        ASSERT(0 == T.isAnyNull());
        ASSERT(0 == T.isAnyRowNull(3, 1));
        ASSERT(1 == T.isAnyNonNull());
        ASSERT(1 == T.isAnyRowNonNull(3, 1));

        t.insertRow(3, rowData);
        ASSERT(5 == T.numRows());
        ASSERT(COL_TYPES_LEN == T.numColumns());
        ASSERT(0 == T.isAnyNull());
        ASSERT(0 == T.isAnyRowNull(3, 2));
        ASSERT(1 == T.isAnyNonNull());
        ASSERT(1 == T.isAnyRowNonNull(3, 2));

        t.insertRow(3, rowData);
        ASSERT(6 == T.numRows());
        ASSERT(COL_TYPES_LEN == T.numColumns());
        ASSERT(0 == T.isAnyNull());
        ASSERT(0 == T.isAnyRowNull(3, 3));
        ASSERT(1 == T.isAnyNonNull());
        ASSERT(1 == T.isAnyRowNonNull(3, 3));

        t.insertRow(3, rowData);
        ASSERT(7 == T.numRows());
        ASSERT(COL_TYPES_LEN == T.numColumns());
        ASSERT(0 == T.isAnyNull());
        ASSERT(0 == T.isAnyRowNull(3, 4));
        ASSERT(1 == T.isAnyNonNull());
        ASSERT(1 == T.isAnyRowNonNull(3, 4));

        ASSERT(ET::BDEM_INT    == T.columnType(0));
        ASSERT(ET::BDEM_STRING == T.columnType(1));
        ASSERT(ET::BDEM_INT    == T.columnType(2));

        ASSERT(getElemRef(T, 0, 0).theInt() == INT_VALUE1);
        ASSERT(getElemRef(T, 3, 1).theString() == STRING_VALUE1);
        ASSERT(getElemRef(T, 6, 2).theInt() == INT_VALUE2);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND.\n";
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << ".\n";
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
